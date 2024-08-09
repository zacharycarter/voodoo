//------------------------------------------------------------------------------
//  ozz_util.cc
//------------------------------------------------------------------------------
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/blending_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/containers/vector.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/base/maths/math_ex.h"
#include "framework/mesh.h"
#include "framework/utils.h"

#include "ozz_util.h"

const int MAX_SAMPLERS = 10;

static struct {
    bool valid;
    ozz_desc_t desc;
    int joint_texture_width;    // in number of pixels
    int joint_texture_height;   // in number of pixels
    int joint_texture_pitch;    // in number of floats
    sg_image joint_texture;
    sg_sampler smp;
    float* joint_upload_buffer;
} state;

typedef struct ozz_sampler_t {
    ozz::sample::PlaybackController controller;
    float weight = 0.f;
    ozz::animation::Animation animation;
    ozz::animation::SamplingJob::Context cache;
    ozz::vector<ozz::math::SoaTransform> local_matrices;
} ozz_sampler_t;

struct ozz_private_t {
    int index;
    ozz::animation::Skeleton skel;
    ozz::animation::Animation anim;
    ozz_sampler_t samplers[MAX_SAMPLERS];
    ozz::vector<uint16_t> joint_remaps;
    ozz::vector<ozz::math::Float4x4> mesh_inverse_bindposes;
    ozz::vector<ozz::math::Float4x4> model_matrices;
    ozz::vector<ozz::math::SoaTransform> blended_locals;
    sg_buffer vbuf = { };
    sg_buffer ibuf = { };
    int num_samplers = -1;
    int num_skin_joints;
    int num_triangle_indices;
    float blend_ratio = .3f;
    bool skel_loaded = false;
    bool anim_loaded = false;
    bool mesh_loaded = false;
    bool load_failed = false;
};

void ozz_setup(const ozz_desc_t* desc) {
    assert(!state.valid);
    assert(desc);
    assert(desc->max_palette_joints > 0);
    assert(desc->max_instances > 0);

    state.valid = true;
    state.desc = *desc;
    state.joint_texture_width = desc->max_palette_joints * 3;
    state.joint_texture_height = desc->max_instances;
    state.joint_texture_pitch = state.joint_texture_width * 4;

    sg_image_desc img_desc = { };
    img_desc.width = state.joint_texture_width;
    img_desc.height = state.joint_texture_height;
    img_desc.num_mipmaps = 1;
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA32F;
    img_desc.usage = SG_USAGE_STREAM;
    state.joint_texture = sg_make_image(&img_desc);

    sg_sampler_desc smp_desc = { };
    smp_desc.min_filter = SG_FILTER_NEAREST;
    smp_desc.mag_filter = SG_FILTER_NEAREST;
    smp_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    smp_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    state.smp = sg_make_sampler(&smp_desc);

    state.joint_upload_buffer = (float*) calloc(state.joint_texture_pitch * state.joint_texture_height, sizeof(float));
}

void ozz_shutdown(void) {
    assert(state.valid);
    assert(state.joint_upload_buffer);
    free(state.joint_upload_buffer);
    // it's ok to call sg_destroy_image with an invalid id
    sg_destroy_image(state.joint_texture);
    state.valid = false;
}

sg_image ozz_joint_texture(void) {
    assert(state.valid);
    return state.joint_texture;
}

sg_sampler ozz_joint_sampler(void) {
    assert(state.valid);
    return state.smp;
}

ozz_instance_t* ozz_create_instance(int index) {
    assert(state.valid);
    assert(index < state.desc.max_instances);
    ozz_private_t* self = new ozz_private_t();
    self->index = index;
    return (ozz_instance_t*) self;
}

void ozz_destroy_instance(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    ozz_private_t* self = (ozz_private_t*) ozz;
    // it's ok to call sg_destroy_buffer with an invalid id
    sg_destroy_buffer(self->vbuf);
    sg_destroy_buffer(self->ibuf);
    delete self;
}

void ozz_load_skeleton(ozz_instance_t* ozz, const void* data, size_t num_bytes) {
    assert(state.valid && ozz && data && (num_bytes > 0));
    ozz_private_t* self = (ozz_private_t*) ozz;
    ozz::io::MemoryStream stream;
    stream.Write(data, num_bytes);
    stream.Seek(0, ozz::io::Stream::kSet);
    ozz::io::IArchive archive(&stream);
    if (archive.TestTag<ozz::animation::Skeleton>()) {
        archive >> self->skel;
        self->skel_loaded = true;
        const int num_soa_joints = self->skel.num_soa_joints();
        const int num_joints = self->skel.num_joints();
        printf("num joints in skeleton: %d\n", num_joints);
        self->model_matrices.resize(num_joints);
        printf("successfully loaded skeleton!\n");
    }
    else {
        printf("loading skeleton failed!\n");
        self->load_failed = true;
    }
}

void ozz_load_animation(ozz_instance_t* ozz, const void* data, size_t num_bytes) {
    assert(state.valid && ozz && data && (num_bytes > 0));
    ozz_private_t* self = (ozz_private_t*) ozz;
    ozz::io::MemoryStream stream;
    stream.Write(data, num_bytes);
    stream.Seek(0, ozz::io::Stream::kSet);
    ozz::io::IArchive archive(&stream);
    if (archive.TestTag<ozz::animation::Animation>() && self->num_samplers < MAX_SAMPLERS) {
        self->num_samplers++;
        archive >> self->samplers[self->num_samplers].animation;
        // archive >> self->anim;
        self->anim_loaded = true;
    }
    else {
        self->load_failed = true;
    }
}

static uint32_t pack_u32(uint8_t x, uint8_t y, uint8_t z, uint8_t w) {
    return (uint32_t)(((uint32_t)w<<24)|((uint32_t)z<<16)|((uint32_t)y<<8)|x);
}

static uint32_t pack_f4_byte4n(float x, float y, float z, float w) {
    int8_t x8 = (int8_t) (x * 127.0f);
    int8_t y8 = (int8_t) (y * 127.0f);
    int8_t z8 = (int8_t) (z * 127.0f);
    int8_t w8 = (int8_t) (w * 127.0f);
    return pack_u32((uint8_t)x8, (uint8_t)y8, (uint8_t)z8, (uint8_t)w8);
}

static uint32_t pack_f4_ubyte4n(float x, float y, float z, float w) {
    uint8_t x8 = (uint8_t) (x * 255.0f);
    uint8_t y8 = (uint8_t) (y * 255.0f);
    uint8_t z8 = (uint8_t) (z * 255.0f);
    uint8_t w8 = (uint8_t) (w * 255.0f);
    return pack_u32(x8, y8, z8, w8);
}

void ozz_load_mesh(ozz_instance_t* ozz, const void* data, size_t num_bytes) {
    assert(state.valid && ozz && data && (num_bytes > 0));
    ozz_private_t* self = (ozz_private_t*) ozz;
    ozz::io::MemoryStream stream;
    stream.Write(data, num_bytes);
    stream.Seek(0, ozz::io::Stream::kSet);
    ozz::io::IArchive archive(&stream);

    // only load the first part of the first mesh
    if (archive.TestTag<ozz::sample::Mesh>()) {
        ozz::sample::Mesh mesh;
        archive >> mesh;
        self->mesh_loaded = true;
        self->num_skin_joints = mesh.num_joints();
        self->num_triangle_indices = (int)mesh.triangle_index_count();
        self->joint_remaps = std::move(mesh.joint_remaps);
        self->mesh_inverse_bindposes = std::move(mesh.inverse_bind_poses);

        // convert mesh data into packed vertices
        size_t num_vertices = (mesh.parts[0].positions.size() / 3);
        const int8_t normal_stride = mesh.parts[0].normals.size() / num_vertices;
        const int8_t joint_index_stride = mesh.parts[0].joint_indices.size() / num_vertices;
        const int8_t joint_weight_stride = mesh.parts[0].joint_weights.size() / num_vertices;
        const float* positions = &mesh.parts[0].positions[0];
        const float* normals = &mesh.parts[0].normals[0];
        const uint16_t* joint_indices = &mesh.parts[0].joint_indices[0];
        const float* joint_weights = &mesh.parts[0].joint_weights[0];
        ozz_vertex_t* vertices = (ozz_vertex_t*) calloc(num_vertices, sizeof(ozz_vertex_t));
        for (int i = 0; i < (int)num_vertices; i++) {
            ozz_vertex_t* v = &vertices[i];
            v->position[0] = positions[i * 3 + 0];
            v->position[1] = positions[i * 3 + 1];
            v->position[2] = positions[i * 3 + 2];
            const float nx = normals[i * normal_stride + 0];
            const float ny = normals[i * normal_stride + 1];
            const float nz = normals[i * normal_stride + 2];
            v->normal = pack_f4_byte4n(nx, ny, nz, 0.0f);
            const uint8_t ji0 = (uint8_t) joint_indices[i * joint_index_stride + 0];
            const uint8_t ji1 = (uint8_t) joint_indices[i * joint_index_stride + 1];
            const uint8_t ji2 = (uint8_t) joint_indices[i * joint_index_stride + 2];
            const uint8_t ji3 = (uint8_t) joint_indices[i * joint_index_stride + 3];
            v->joint_indices = pack_u32(ji0, ji1, ji2, ji3);
            const float jw0 = joint_weights[i * joint_weight_stride + 0];
            const float jw1 = joint_weights[i * joint_weight_stride + 1];
            const float jw2 = joint_weights[i * joint_weight_stride + 2];
            const float jw3 = 1.0f - (jw0 + jw1 + jw2);
            v->joint_weights = pack_f4_ubyte4n(jw0, jw1, jw2, jw3);
        }

        // create vertex- and index-buffer
        sg_buffer_desc vbuf_desc = { };
        vbuf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
        vbuf_desc.data.ptr = vertices;
        vbuf_desc.data.size = num_vertices * sizeof(ozz_vertex_t);
        self->vbuf = sg_make_buffer(&vbuf_desc);
        free(vertices); vertices = nullptr;

        sg_buffer_desc ibuf_desc = { };
        ibuf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
        ibuf_desc.data.ptr = &mesh.triangle_indices[0];
        ibuf_desc.data.size = self->num_triangle_indices * sizeof(uint16_t);
        self->ibuf = sg_make_buffer(&ibuf_desc);
    }
    else {
        self->load_failed = true;
    }
}

void ozz_initialize_instance(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    ozz_private_t* self = (ozz_private_t*) ozz;

    for (int i = 0; i < self->num_samplers; i++) {
        self->samplers[i].local_matrices.resize(self->skel.num_soa_joints());
        self->samplers[i].cache.Resize(self->skel.num_joints());
    }
    self->blended_locals.resize(self->skel.num_soa_joints());
}

void ozz_set_load_failed(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    ozz_private_t* self = (ozz_private_t*) ozz;
    self->load_failed = true;
}

bool ozz_all_loaded(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    ozz_private_t* self = (ozz_private_t*) ozz;
    bool all_loaded = self->skel_loaded && self->anim_loaded && self->mesh_loaded && !self->load_failed;
    if (all_loaded) ozz_initialize_instance(ozz);
    return all_loaded;
}

bool ozz_load_failed(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    return ((ozz_private_t*)ozz)->load_failed;
}

sg_buffer ozz_vertex_buffer(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    return ((ozz_private_t*)ozz)->vbuf;
}

sg_buffer ozz_index_buffer(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    return ((ozz_private_t*)ozz)->ibuf;
}

void ozz_update_instance(ozz_instance_t* ozz, float dt) {
    assert(state.valid && ozz);
    assert(state.joint_upload_buffer);

    ozz_private_t* self = (ozz_private_t*) ozz;

    // Computes weight parameters for all samplers.
    const float kNumIntervals = self->num_samplers - 1;
    const float kInterval = 1.f / kNumIntervals;
    for (int i = 0; i < self->num_samplers; ++i) {
      const float med = i * kInterval;
      const float x = self->blend_ratio - med;
      const float y = ((x < 0.f ? x : -x) + kInterval) * kNumIntervals;
      self->samplers[i].weight = ozz::math::Max(0.f, y);
    }

    // Synchronizes animations.
    // First computes loop cycle duration. Selects the 2 samplers that define
    // interval that contains blend_ratio.
    // Uses a maximum value smaller that 1.f (-epsilon) to ensure that
    // (relevant_sampler + 1) is always valid.
    const int relevant_sampler =
        static_cast<int>((self->blend_ratio - 1e-3f) * (self->num_samplers - 1));
    assert(relevant_sampler + 1 < self->num_samplers);
    ozz_sampler_t& sampler_l = self->samplers[relevant_sampler];
    ozz_sampler_t& sampler_r = self->samplers[relevant_sampler + 1];

    // Interpolates animation durations using their respective weights, to
    // find the loop cycle duration that matches blend_ratio.
    const float loop_duration =
        sampler_l.animation.duration() * sampler_l.weight +
        sampler_r.animation.duration() * sampler_r.weight;

    // Finally finds the speed coefficient for all samplers.
    const float inv_loop_duration = 1.f / loop_duration;
    for (int i = 0; i < self->num_samplers; ++i) {
      ozz_sampler_t& sampler = self->samplers[i];
      const float speed = sampler.animation.duration() * inv_loop_duration;
      sampler.controller.set_playback_speed(speed);
    }

    for (int i = 0; i < self->num_samplers; i++) {
        ozz_sampler_t& sampler = self->samplers[i];

        sampler.controller.Update(sampler.animation, dt);

        if (sampler.weight <= 0.f) continue;

        ozz::animation::SamplingJob sampling_job;
        sampling_job.animation = &sampler.animation;
        sampling_job.context = &sampler.cache;
        sampling_job.ratio = sampler.controller.time_ratio();
        sampling_job.output = make_span(sampler.local_matrices);
        sampling_job.Run();
    }

    ozz::animation::BlendingJob::Layer* layers = (ozz::animation::BlendingJob::Layer*)calloc(self->num_samplers, sizeof(ozz::animation::BlendingJob::Layer));
    for (int i = 0; i < self->num_samplers; i++) {
        layers[i].transform = make_span(self->samplers[i].local_matrices);
        layers[i].weight = self->samplers[i].weight;
    }

    ozz::animation::BlendingJob blend_job;
    blend_job.threshold = ozz::animation::BlendingJob().threshold;
    blend_job.layers = ozz::span<ozz::animation::BlendingJob::Layer>(layers, self->num_samplers);
    blend_job.rest_pose = self->skel.joint_rest_poses();
    blend_job.output = make_span(self->blended_locals);
    blend_job.Run();

    ozz::animation::LocalToModelJob ltm_job;
    ltm_job.skeleton = &self->skel;
    ltm_job.input = make_span(self->blended_locals);
    ltm_job.output = make_span(self->model_matrices);
    ltm_job.Run();

    for (int i = 0; i < self->num_skin_joints; i++) {
        ozz::math::Float4x4 skin_matrix = self->model_matrices[self->joint_remaps[i]] * self->mesh_inverse_bindposes[i];
        const ozz::math::SimdFloat4& c0 = skin_matrix.cols[0];
        const ozz::math::SimdFloat4& c1 = skin_matrix.cols[1];
        const ozz::math::SimdFloat4& c2 = skin_matrix.cols[2];
        const ozz::math::SimdFloat4& c3 = skin_matrix.cols[3];

        float* ptr = &state.joint_upload_buffer[self->index*state.joint_texture_pitch + i*12];
        *ptr++ = ozz::math::GetX(c0); *ptr++ = ozz::math::GetX(c1); *ptr++ = ozz::math::GetX(c2); *ptr++ = ozz::math::GetX(c3);
        *ptr++ = ozz::math::GetY(c0); *ptr++ = ozz::math::GetY(c1); *ptr++ = ozz::math::GetY(c2); *ptr++ = ozz::math::GetY(c3);
        *ptr++ = ozz::math::GetZ(c0); *ptr++ = ozz::math::GetZ(c1); *ptr++ = ozz::math::GetZ(c2); *ptr++ = ozz::math::GetZ(c3);
    }
}

void ozz_update_joint_texture(void) {
    assert(state.valid);
    assert(state.joint_upload_buffer);

    // FIXME: upload partial texture? (needs sokol-gfx fixes)
    sg_image_data img_data = { };
    img_data.subimage[0][0].ptr = state.joint_upload_buffer;
    img_data.subimage[0][0].size = (size_t) (state.joint_texture_pitch * state.joint_texture_height * sizeof(float));
    sg_update_image(state.joint_texture, img_data);
}

float ozz_joint_texture_pixel_width(void) {
    assert(state.valid);
    return 1.0f / (float)state.joint_texture_width;
}

float ozz_joint_texture_u(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    (void)ozz;
    const float half_pixel_x = 0.5f / (float)state.joint_texture_width;
    return half_pixel_x;
}

float ozz_joint_texture_v(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    ozz_private_t* self = (ozz_private_t*) ozz;
    const float half_pixel_y = 0.5f / (float)state.joint_texture_height;
    return half_pixel_y + (self->index / (float)state.joint_texture_height);
}

int ozz_num_triangle_indices(ozz_instance_t* ozz) {
    assert(state.valid && ozz);
    ozz_private_t* self = (ozz_private_t*) ozz;
    return self->num_triangle_indices;
}
