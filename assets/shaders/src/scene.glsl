@ctype mat4 HMM_Mat4
@ctype vec3 HMM_Vec3
@ctype vec4 HMM_Vec4

@vs vd_v3d_scene_vs

uniform vd_v3d_scene_vs_uniforms {
  mat4 mvp;
  mat4 model;
  mat4 light_mvp;
  vec3 diff_color;
};

in vec4 pos;
in vec3 norm;

out vec3 color;
out vec4 light_proj_pos;
out vec4 world_pos;
out vec3 world_norm;

void main() {
  gl_Position = mvp * pos;
  light_proj_pos = light_mvp * pos;
  #if !SOKOL_GLSL
    light_proj_pos.y = -light_proj_pos.y;
  #endif
  world_pos = model * pos;
  world_norm = normalize((model * vec4(norm, 0.0)).xyz);
  color = diff_color;
}

@end

@fs vd_v3d_scene_fs

uniform vd_v3d_scene_fs_uniforms {
  vec3 ambient;
  vec3 light_dir;
  vec3 light_color;
  vec3 eye_pos;
  float shadow_far;
};

uniform texture2D shadow_map;
uniform sampler shadow_sampler;

in vec3 color;
in vec4 light_proj_pos;
in vec4 world_pos;
in vec3 world_norm;

out vec4 frag_color;

const int pcf_count = 4;
const int pcf_samples = (2 * pcf_count + 1) * (2 * pcf_count + 1);
const float texel_c = 1.0;
const float soft_shadow_edge = 10.0;

float rgba_to_float(const in vec4 rgba) {
  return dot(rgba, vec4(1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 160581375.0));
}

float sampleShadow(texture2D tex, sampler smp, vec2 uv, float compare, float bias) {
  float depth = rgba_to_float(texture(sampler2D(tex, smp), vec2(uv.x, uv.y)));
  depth += bias;
  return step(compare, depth);
}

float sampleShadowPCF(texture2D tex, sampler smp, vec2 uv, float texel_size, float compare, float n_dot_l, float d) {
  float result = 0.0;
  float cos_theta = clamp(n_dot_l, 0.0, 1.0);
  float bias = 0.003 * tan(acos(cos_theta));
  
  float edge = clamp(d - (shadow_far - soft_shadow_edge), 0.0, soft_shadow_edge + 0.1);
  if (edge >= soft_shadow_edge) {
    return 1.0;
  }

  edge *= (1.0 / soft_shadow_edge);

  for (int x = -pcf_count; x <= pcf_count; x++) {
      for (int y = -pcf_count; y <= pcf_count; y++) {
          result += sampleShadow(tex, smp, uv + vec2(x, y) * texel_size * texel_c, compare, bias);
      }
  }

  result /= float(pcf_samples);
  result = mix(result, 1.0, edge);
  return result;
}

void main() {
  float specular_power = 2.2; 
  float shininess = 1.0; 
  float emissive = 0.0;
  float sm_size = 4096.0; 
  vec3 l = normalize(light_dir);
  vec3 n = normalize(world_norm);
  float n_dot_l = dot(n, l);

  if (n_dot_l >= 0.0) {
    vec3 vd = eye_pos - world_pos.xyz;
    vec3 v = normalize(vd);
    vec3 r = reflect(l, n);
    vec3 light_pos = light_proj_pos.xyz / light_proj_pos.w;
    vec2 sm_uv = (light_pos.xy + 1.0) * 0.5;
    float depth = light_proj_pos.z;
    float texel_size = 1.0 / sm_size; 
    float d = length(vd);
    float s = sampleShadowPCF(shadow_map, shadow_sampler, sm_uv, texel_size, depth, n_dot_l, d);

    s = max(s, emissive);
    float r_dot_v = max(dot(r, v), 0.0);
    float l_shiny = pow(r_dot_v * n_dot_l, shininess);
    vec3 l_specular = vec3(specular_power * l_shiny * light_color);
    vec3 l_diffuse = vec3(light_color) * n_dot_l;
    vec3 l_light = (ambient + s * l_diffuse);
    frag_color = vec4(max(vec3(emissive), l_light) * color.xyz + s * l_specular, 1.0);
  } else {
    vec3 light = emissive + clamp(1.0 - emissive, 0.0, 1.0) * (ambient);
    frag_color = vec4(light * color.xyz, 1.0);
  }
}

@end

@program vd_v3d_scene vd_v3d_scene_vs vd_v3d_scene_fs
