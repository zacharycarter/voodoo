@ctype mat4 HMM_Mat4
@ctype vec3 HMM_Vec3

@vs vd_v3d_depth_vs

uniform vd_v3d_depth_vs_uniforms {
  mat4 mvp;
};

in vec4 vertex_pos;
out vec3 pos;

void main() {
  gl_Position = mvp * vertex_pos;
  pos = gl_Position.xyz;
}
@end

@fs vd_v3d_depth_fs

uniform vd_v3d_depth_fs_uniforms {
  float far;
  float depth;
};

in vec3 pos;
out vec4 frag_color;

vec4 float_to_rgba(const in float v) {
  vec4 enc = vec4(1.0, 255.0, 65025.0, 160581375.0) * v;
  enc = fract(enc);
  enc -= enc.yzww * vec4(1.0 / 255.0, 1.0 / 255.0, 1.0 / 250.0, 0.0);
  return enc;
}

vec4 depth_to_rgba(vec3 pos) {
  vec3 pos_norm = pos / far;
  float d = length(pos_norm);
  d = clamp(d, 0.0, 1.0);
  return float_to_rgba(d);
}

void main() {
  frag_color = depth_to_rgba(pos);
}

@end

@program vd_v3d_depth vd_v3d_depth_vs vd_v3d_depth_fs
