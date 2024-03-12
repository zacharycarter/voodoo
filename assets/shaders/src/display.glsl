@ctype mat4 HMM_Mat4 

@vs vd_v3d_display_vs
uniform vd_v3d_display_vs_uniforms {
    mat4 mvp;
};

in vec4 position;
in vec4 color0;

out vec4 color;

void main() {
    gl_Position = mvp * position;
    color = color0;
}
@end

@fs vd_v3d_display_fs 
in vec4 color;
out vec4 frag_color;

void main() {
    frag_color = color;
}
@end

@program  vd_v3d_display vd_v3d_display_vs vd_v3d_display_fs
