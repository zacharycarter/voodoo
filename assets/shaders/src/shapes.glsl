@ctype mat4 HMM_Mat4

@vs vs
uniform vs_params {
    mat4 vp;
};

layout(location=0) in vec4 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;
layout(location=3) in vec4 color0;
layout(location=4) in mat4 model;

out vec4 color;

void main() {
    gl_Position = vp * model * position;
    color = color0;
}
@end

@fs fs
in vec4 color;
out vec4 frag_color;

void main() {
    frag_color = color;
}
@end

@program shapes vs fs