@ctype mat4 HMM_Mat4
@ctype vec3 HMM_Vec3
@ctype vec2 HMM_Vec2

@block util

vec4 encode_depth(float v) {
    vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
    enc = fract(enc);
    enc -= enc.yzww * vec4(1.0/255.0,1.0/255.0,1.0/255.0,0.0);
    return enc;
}

float decodeDepth(vec4 rgba) {
    return dot(rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0));
}

float sample_shadow(texture2D tex, sampler smp, vec2 uv, float compare) {
    float depth = decodeDepth(texture(sampler2D(tex, smp), vec2(uv.x, uv.y)));
    return step(compare, depth);
}

float sample_shadow_pcf(texture2D tex, sampler smp, vec3 uv_depth, vec2 sm_size) {
    float result = 0.0;
    for (int x = -2; x <= 2; x++) {
        for (int y =- 2; y <= 2; y++) {
            vec2 offset = vec2(x, y) / sm_size;
            result += sample_shadow(tex, smp, uv_depth.xy + offset, uv_depth.z);
        }
    }
    return result / 25.0;
}

@end

@block skin_utils
void skinned_pos_nrm(in vec4 pos, in vec4 nrm, in vec4 skin_weights, in vec4 skin_indices, in vec2 joint_uv, out vec4 skin_pos, out vec4 skin_nrm) {
    skin_pos = vec4(0.0, 0.0, 0.0, 1.0);
    skin_nrm = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 weights = skin_weights / dot(skin_weights, vec4(1.0));
    vec2 step = vec2(joint_pixel_width, 0.0);
    vec2 uv;
    vec4 xxxx, yyyy, zzzz;
    if (weights.x > 0.0) {
        uv = vec2(joint_uv.x + (3.0 * skin_indices.x)*joint_pixel_width, joint_uv.y);
        xxxx = textureLod(sampler2D(joint_tex, smp), uv, 0.0);
        yyyy = textureLod(sampler2D(joint_tex, smp), uv + step, 0.0);
        zzzz = textureLod(sampler2D(joint_tex, smp), uv + 2.0 * step, 0.0);
        skin_pos.xyz += vec3(dot(pos,xxxx), dot(pos,yyyy), dot(pos,zzzz)) * weights.x;
        skin_nrm.xyz += vec3(dot(nrm,xxxx), dot(nrm,yyyy), dot(nrm,zzzz)) * weights.x;
    }
    if (weights.y > 0.0) {
        uv = vec2(joint_uv.x + (3.0 * skin_indices.y)*joint_pixel_width, joint_uv.y);
        xxxx = textureLod(sampler2D(joint_tex, smp), uv, 0.0);
        yyyy = textureLod(sampler2D(joint_tex, smp), uv + step, 0.0);
        zzzz = textureLod(sampler2D(joint_tex, smp), uv + 2.0 * step, 0.0);
        skin_pos.xyz += vec3(dot(pos,xxxx), dot(pos,yyyy), dot(pos,zzzz)) * weights.y;
        skin_nrm.xyz += vec3(dot(nrm,xxxx), dot(nrm,yyyy), dot(nrm,zzzz)) * weights.y;
    }
    if (weights.z > 0.0) {
        uv = vec2(joint_uv.x + (3.0 * skin_indices.z)*joint_pixel_width, joint_uv.y);
        xxxx = textureLod(sampler2D(joint_tex, smp), uv, 0.0);
        yyyy = textureLod(sampler2D(joint_tex, smp), uv + step, 0.0);
        zzzz = textureLod(sampler2D(joint_tex, smp), uv + 2.0 * step, 0.0);
        skin_pos.xyz += vec3(dot(pos,xxxx), dot(pos,yyyy), dot(pos,zzzz)) * weights.z;
        skin_nrm.xyz += vec3(dot(nrm,xxxx), dot(nrm,yyyy), dot(nrm,zzzz)) * weights.z;
    }
    if (weights.w > 0.0) {
        uv = vec2(joint_uv.x + (3.0 * skin_indices.w)*joint_pixel_width, joint_uv.y);
        xxxx = textureLod(sampler2D(joint_tex, smp), uv, 0.0);
        yyyy = textureLod(sampler2D(joint_tex, smp), uv + step, 0.0);
        zzzz = textureLod(sampler2D(joint_tex, smp), uv + 2.0 * step, 0.0);
        skin_pos.xyz += vec3(dot(pos,xxxx), dot(pos,yyyy), dot(pos,zzzz)) * weights.w;
        skin_nrm.xyz += vec3(dot(nrm,xxxx), dot(nrm,yyyy), dot(nrm,zzzz)) * weights.w;
    }
}
@end

@vs vd_v3d_doll_vs
uniform vd_v3d_doll_vs_uniforms {
    mat4 mvp;
    mat4 model;
    vec2 joint_uv;
    float joint_pixel_width;
    mat4 light_mvp;
    vec3 diff_color;
};

in vec4 position;
in vec3 normal;
@image_sample_type joint_tex unfilterable_float
uniform texture2D joint_tex;
@sampler_type smp nonfiltering
uniform sampler smp;
in vec4 joint_indices;
in vec4 joint_weights;

@include_block skin_utils

out vec3 color;
out vec4 light_proj_pos;
out vec4 world_pos;
out vec3 world_norm;

void main() {
    vec4 pos, nrm;
    skinned_pos_nrm(position, vec4(normal, 0.0), joint_weights, joint_indices * 255.0, joint_uv, pos, nrm);
    gl_Position = mvp * pos;
    light_proj_pos = light_mvp * pos;
    #if !SOKOL_GLSL
        light_proj_pos.y = -light_proj_pos.y;
    #endif
    world_pos = model * pos;
    world_norm = normalize((model * nrm).xyz);
    color = diff_color;
}
@end

@fs vd_v3d_doll_fs
@include_block util

uniform vd_v3d_doll_fs_uniforms {
    vec3 light_dir;
    vec3 eye_pos;
};

uniform texture2D shadow_map;
uniform sampler shadow_sampler;

in vec3 color;
in vec4 light_proj_pos;
in vec4 world_pos;
in vec3 world_norm;

out vec4 frag_color;

vec4 gamma(vec4 c) {
    float p = 1.0 / 2.2;
    return vec4(pow(c.xyz, vec3(p)), c.w);
}

void main() {
    vec2 sm_size = textureSize(sampler2D(shadow_map, shadow_sampler), 0);
    float spec_power = 2.2;
    float ambient_intensity = 0.25;
    vec3 l = normalize(light_dir);
    vec3 n = normalize(world_norm);
    float n_dot_l = dot(n, l);
    if (n_dot_l > 0.0) {

        vec3 light_pos = light_proj_pos.xyz / light_proj_pos.w;
        vec3 sm_pos = vec3((light_pos.xy + 1.0) * 0.5, light_pos.z);
        float s = sample_shadow_pcf(shadow_map, shadow_sampler, sm_pos, sm_size);
        float diff_intensity = max(n_dot_l * s, 0.0);

        vec3 v = normalize(eye_pos - world_pos.xyz);
        vec3 r = reflect(-l, n);
        float r_dot_v = max(dot(r, v), 0.0);
        float spec_intensity = pow(r_dot_v, spec_power) * n_dot_l * s;

        frag_color = vec4(vec3(spec_intensity) + (diff_intensity + ambient_intensity) * color, 1.0);
    } else {
        frag_color = vec4(color * ambient_intensity, 1.0);
    }
    frag_color = gamma(frag_color);
}
@end

@program  vd_v3d_doll vd_v3d_doll_vs vd_v3d_doll_fs
