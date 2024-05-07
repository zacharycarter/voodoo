-----------------------------------------------------------
//  float/rgba8 encoding/decoding so that we can use an RGBA8
//  shadow map instead of floating point render targets which might
//  not be supported everywhere
//
//  http://aras-p.info/blog/2009/07/30/encoding-floats-to-rgba-the-final/
//
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

//=== shadow pass
@vs vd_v3d_shadow_doll_vs

uniform vd_v3d_shadow_doll_vs_uniforms {
    mat4 mvp;
    vec2 joint_uv;
    float joint_pixel_width;
};

in vec4 position;
in vec4 normal;
@image_sample_type joint_tex unfilterable_float
uniform texture2D joint_tex;
@sampler_type smp nonfiltering
uniform sampler smp;
in vec4 joint_indices;
in vec4 joint_weights;

out vec2 proj_zw;

@include_block skin_utils

void main() {
    vec4 pos, nrm;
    skinned_pos_nrm(position, normal, joint_weights, joint_indices * 255.0, joint_uv, pos, nrm);
    gl_Position = mvp * pos;
    proj_zw = gl_Position.zw;
}
@end

@fs vd_v3d_shadow_doll_fs
@include_block util

in vec2 proj_zw;
out vec4 frag_color;

void main() {
    float depth = proj_zw.x / proj_zw.y;
    frag_color = encode_depth(depth);
}
@end

@program vd_v3d_shadow_doll vd_v3d_shadow_doll_vs vd_v3d_shadow_doll_fs
