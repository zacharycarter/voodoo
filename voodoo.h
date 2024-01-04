#ifndef VOODOO_INCLUDED
/*

    █ ██ ███ ██████████████████████████████████████▀▀▀▀▀▀▀█████████████████████████
    ▀ ▀▀ ▀▀▀ ▀▀▀▀▀▀▀ ▄▄▄▄▄▄▄ ▀▀▀▀ ▄▄▄▄▄▄▄ ▀▀▀▀ ▄▄▄▄▄█████ ▀ ▄▄▄▄▄▄▄ ▀▀▀▀ ▄▄▄▄▄▄▄ ▀▀
     █████   ██████████   ██████████   █████▐████▌  ██████████   ██████████   █████
     █████   ██████████   ██████████   ██████████   ██████████   ██████████   █████
     ▐████▌ ▐████▌▐████▌ ▐████▌▐████▌ ▐████▌▐████▌ ▐████▌▐████▌ ▐████▌▐████▌ ▐████▌
      ▀▀███ ███▀▀  ▀▀███ ███▀▀  ▀▀███ ███▀▀  ▀▀███ ███▀▀  ▀▀███ ███▀▀  ▀▀███ ███▀▀
          ▀▀▀          ▀▀▀          ▀▀▀          ▀▀▀          ▀▀▀          ▀▀▀

                                         ▄■
                                        ▐▌
                                         ▀▄▄
                           ▀▄▄             ▄█▀▀▄▄         ▄▄▀
                             ▀▀██▄▄▄▄     ▐██▌   ▀ ▄▄▄▄██▀▀
                                ▀▀▀███████▄██▄███████▀▀▀
                                      ▀▀▀██████▀▀▀
                                     ▄▄▀ ▐████▌
                                    ▀▄    ████
                                      ▀▀▀▄▄▄▄▄▄▄
                                         ▄▄▄▄▄▄ ▀▀▀▄
                                        ▐██▌▐██▌  ▄▄▀
                                      ▄▄▐██  ██▌▀▀
                                    ▄▀  ██▌  ▐██
                                     ▀▀▄▄▄▄▄  ▀▀
                                        ▄▄  ▀▀▀▀▀▄▄
                                       ▐██    ██▌ ▄▀
                                       ▀▀  ▄▀  ▀▀
                                          ▐▌
                                           ▀▀■

    voodoo.h -- 3D game programming library

*/
#define VOODOO_INCLUDED (1)

#ifndef VOODOO_API_DECL
#if defined(_WIN32) && defined(VOODOO_DLL) && defined(VOODOO_IMPL)
#define VOODOO_API_DECL __declspec(dllexport)
#elif defined(_WIN32) && defined(VOODOO_DLL)
#define VOODOO_API_DECL __declspec(dllimport)
#else
#define VOODOO_API_DECL extern
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
} // extern "C"

// reference-based equivalents for c++

#endif
#endif // VOODOO_INCLUDED

//     ______  _______  __    ________  __________   ___________  ______________  _   __
//    /  _/  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/  _/ __ \/ | / /
//    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
//  _/ // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ / _/ // /_/ / /|  /
// /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
//
// >>implementation

#ifdef VOODOO_IMPL
#define VOODOO_IMPL_INCLUDED (1)

#ifdef _MSC_VER
#pragma warning(push)
#endif

#define ZPL_IMPL
#define ZPL_NANO
#define ZPL_ENABLE_HASHING
#include <zpl.h>

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SIMD
#define HANDMADE_MATH_USE_DEGREES
#include "hmm.h"
#include "flecs.h"
#include "janet.h"
#define SOKOL_IMPL
#define SOKOL_WGPU
#include "sokol_app.h"
#include "sokol_args.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_shape.h"
#include "sokol_time.h"
#include "cube-sapp.glsl.h"

//    _____________  __  _________________
//   / __/_  __/ _ \/ / / / ___/_  __/ __/
//  _\ \  / / / , _/ /_/ / /__  / / _\ \
// /___/ /_/ /_/|_|\____/\___/ /_/ /___/
//
// >>structs

typedef struct
{
  float min_dist;
  float max_dist;
  float min_lat;
  float max_lat;
  float distance;
  float latitude;
  float longitude;
  float aspect;
  float nearz;
  float farz;
  HMM_Vec3 center;
} vd__camera_desc_t;

ECS_STRUCT(Camera, {
  float min_dist;
  float max_dist;
  float min_lat;
  float max_lat;
  float distance;
  float latitude;
  float longitude;
  float aspect;
  float nearz;
  float farz;
  HMM_Vec3 center;
  HMM_Vec3 eye_pos;
  HMM_Mat4 view;
  HMM_Mat4 proj;
  HMM_Mat4 view_proj;
});
typedef Camera vd__camera_t;

typedef ecs_id_t vd__ecs_component_registry_entry;

ZPL_TABLE_DECLARE(static, vd__ecs_component_registry, vd__ecs_component_registry_, vd__ecs_component_registry_entry);

ZPL_TABLE_DEFINE(vd__ecs_component_registry, vd__ecs_component_registry_, vd__ecs_component_registry_entry);

static struct
{
  struct
  {
    JanetFiber *main_fiber;
    JanetFunction *mod_init_cb;
    JanetFunction *mod_update_cb;
    JanetFunction *mod_shutdown_cb;
  } app;

  struct
  {
    struct
    {
      sg_pipeline pip;
      sg_bindings bind;
    } draw;
  } dbg;

  struct
  {
    vd__ecs_component_registry components;
  } ecs;

} vd__state;

//    __  ______ ________ __
//   /  |/  / _ /_  __/ // /
//  / /|_/ / __ |/ / / _  /
// /_/  /_/_/ |_/_/ /_//_/
//
// >>math

static const float vd__pi = HMM_PI32;
static const float vd__2pi = HMM_PI32 * 2.0f;
static const float vd__pi_div2 = HMM_PI32 / 2.0f;

static const HMM_Vec3 vd__unit_x = {1.0f, 0.0f, 0.0f};
static const HMM_Vec3 vd__unit_y = {0.0f, 1.0f, 0.0f};
static const HMM_Vec3 vd__unit_z = {0.0f, 0.0f, 1.0f};

static const HMM_Mat2 vd__identity2 = {.Elements = {{1.0f, 0.0f}, {0.0f, 1.0f}}};
static const HMM_Mat3 vd__identity3 = {.Elements = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}};
static const HMM_Mat4 vd_identity4 = {
  .Elements = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}};

//   ________   __  __________  ___
//  / ___/ _ | /  |/  / __/ _ \/ _ |
// / /__/ __ |/ /|_/ / _// , _/ __ |
// \___/_/ |_/_/  /_/___/_/|_/_/ |_|
//
// >>camera

#define CAMERA_DEFAULT_MIN_DIST (2.0f)
#define CAMERA_DEFAULT_MAX_DIST (30.0f)
#define CAMERA_DEFAULT_MIN_LAT (-85.0f)
#define CAMERA_DEFAULT_MAX_LAT (85.0f)
#define CAMERA_DEFAULT_DIST (5.0f)
#define CAMERA_DEFAULT_ASPECT (60.0f)
#define CAMERA_DEFAULT_NEARZ (0.01f)
#define CAMERA_DEFAULT_FARZ (100.0f)

ECS_COMPONENT_DECLARE(Camera);

static float vd__cam_def(float val, float def)
{
  return ((val == 0.0f) ? def : val);
}

static void vd__cam_orbit(vd__camera_t *cam, const vd__camera_desc_t *desc)
{
  assert(cam && desc);
  memset(cam, 0, sizeof(vd__camera_t));
  cam->min_dist = vd__cam_def(desc->min_dist, CAMERA_DEFAULT_MIN_DIST);
  cam->max_dist = vd__cam_def(desc->max_dist, CAMERA_DEFAULT_MAX_DIST);
  cam->min_lat = vd__cam_def(desc->min_lat, CAMERA_DEFAULT_MIN_LAT);
  cam->max_lat = vd__cam_def(desc->max_lat, CAMERA_DEFAULT_MAX_LAT);
  cam->distance = vd__cam_def(desc->distance, CAMERA_DEFAULT_DIST);
  cam->center = desc->center;
  cam->latitude = desc->latitude;
  cam->longitude = desc->longitude;
  cam->aspect = vd__cam_def(desc->aspect, CAMERA_DEFAULT_ASPECT);
  cam->nearz = vd__cam_def(desc->nearz, CAMERA_DEFAULT_NEARZ);
  cam->farz = vd__cam_def(desc->farz, CAMERA_DEFAULT_FARZ);
}

//    ___  _______  __  _______  ___  ___  ___ _      __
//   / _ \/ __/ _ )/ / / / ___/ / _ \/ _ \/ _ | | /| / /
//  / // / _// _  / /_/ / (_ / / // / , _/ __ | |/ |/ /
// /____/___/____/\____/\___/ /____/_/|_/_/ |_|__/|__/
//
// >debugdraw

static const float vd__dbg_draw_cube_vertices[] = {
  -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  1.0f,  1.0f,  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

  -1.0f, -1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f,
  1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f,

  -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  -1.0f, 1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,

  1.0f,  -1.0f, -1.0f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f,  1.0f,  -1.0f, 1.0f, 0.5f, 0.0f, 1.0f,
  1.0f,  1.0f,  1.0f,  1.0f, 0.5f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f,  1.0f, 0.5f, 0.0f, 1.0f,

  -1.0f, -1.0f, -1.0f, 0.0f, 0.5f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0.5f, 1.0f, 1.0f,
  1.0f,  -1.0f, 1.0f,  0.0f, 0.5f, 1.0f, 1.0f, 1.0f,  -1.0f, -1.0f, 0.0f, 0.5f, 1.0f, 1.0f,

  -1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 0.5f, 1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 0.0f, 0.5f, 1.0f,
  1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,  1.0f,  -1.0f, 1.0f, 0.0f, 0.5f, 1.0f};

static const uint16_t vd__dbg_draw_cube_indicies[] = {0,  1,  2,  0,  2,  3,  6,  5,  4,  7,  6,  4,
                                                      8,  9,  10, 8,  10, 11, 14, 13, 12, 15, 14, 12,
                                                      16, 17, 18, 16, 18, 19, 22, 21, 20, 23, 22, 20};

void vd__dbg_draw_cube()
{
  /* NOTE: the vs_params_t struct has been code-generated by the shader-code-gen */
  vs_params_t vs_params;
  const float w = sapp_widthf();
  const float h = sapp_heightf();
  const float t = (float)(sapp_frame_duration() * 60.0);
  HMM_Mat4 proj = HMM_Perspective_RH_NO(60.0f, w / h, 0.01f, 10.0f);
  HMM_Mat4 view = HMM_LookAt_RH(HMM_V3(0.0f, 1.5f, 6.0f), HMM_V3(0.0f, 0.0f, 0.0f), HMM_V3(0.0f, 1.0f, 0.0f));
  HMM_Mat4 view_proj = HMM_MulM4(proj, view);
  HMM_Mat4 model = vd_identity4;
  vs_params.mvp = HMM_MulM4(view_proj, model);

  sg_pass_action pass_action = {
    .colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.25f, 0.5f, 0.75f, 1.0f}}};
  sg_begin_default_pass(&pass_action, (int)w, (int)h);
  sg_apply_pipeline(vd__state.dbg.draw.pip);
  sg_apply_bindings(&vd__state.dbg.draw.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
  sg_draw(0, 36, 1);
  sg_end_pass();
  sg_commit();
}

void vd__dbg_draw_init()
{
  sg_buffer vbuf = sg_make_buffer(
    &(sg_buffer_desc){.data = SG_RANGE(vd__dbg_draw_cube_vertices), .label = "vd-dbg-draw-cube-vertices"});

  sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){.type = SG_BUFFERTYPE_INDEXBUFFER,
                                                    .data = SG_RANGE(vd__dbg_draw_cube_indicies),
                                                    .label = "vd-dbg-draw-cube-indices"});

  /* create shader */
  sg_shader shd = sg_make_shader(cube_shader_desc(sg_query_backend()));

  /* create pipeline object */
  vd__state.dbg.draw.pip = sg_make_pipeline(&(sg_pipeline_desc){
    .layout =
      {/* test to provide buffer stride, but no attr offsets */
       .buffers[0].stride = 28,
       .attrs = {[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3, [ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4}},
    .shader = shd,
    .index_type = SG_INDEXTYPE_UINT16,
    .cull_mode = SG_CULLMODE_BACK,
    .depth =
      {
        .write_enabled = true,
        .compare = SG_COMPAREFUNC_LESS_EQUAL,
      },
    .label = "vd-dbg-draw-cube-pipeline"});

  /* setup resource bindings */
  vd__state.dbg.draw.bind = (sg_bindings){.vertex_buffers[0] = vbuf, .index_buffer = ibuf};
}

//    _____________
//   / __/ ___/ __/
//  / _// /___\ \
// /___/\___/___/
//
// >>ecs

typedef HMM_Vec3 Vec3;

ECS_COMPONENT_DECLARE(Vec3);

ECS_STRUCT(Transform, {
  Vec3 pos;
  Vec3 rot;
  Vec3 scale;
});

static void vd__ecs_janet_callback(ecs_iter_t *it)
{
  Janet out = janet_wrap_nil();
  const Janet arg = janet_wrap_abstract(it);
  JanetFiber *fiber = NULL;
  int sign = janet_pcall(it->binding_ctx, 1, &arg, &out, &fiber);
  if (sign != JANET_SIGNAL_OK)
  {
    janet_stacktrace(fiber, out);
  }
}

static Janet vd__ecs_system(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 4);

  ecs_entity_t e = 0;

  ecs_world_t *w = janet_checktype(argv[0], JANET_POINTER) ? janet_getpointer(argv, 0) : NULL;
  JanetFunction *cb = janet_checktype(argv[1], JANET_FUNCTION) ? janet_getfunction(argv, 1) : NULL;
  uint64_t phase = janet_checktype(argv[2], JANET_NUMBER) ? janet_getuinteger64(argv, 2) : 0;
  const char *query = janet_checktype(argv[3], JANET_STRING) ? janet_getcstring(argv, 3) : NULL;

  ecs_system_desc_t desc = {0};
  ecs_entity_desc_t edesc = {0};
  edesc.name = (const char *)cb->def->name;
  edesc.add[0] = ((phase) ? ecs_pair(EcsDependsOn, (phase)) : 0);
  edesc.add[1] = (phase);
  desc.entity = ecs_entity_init(w, &edesc);
  desc.query.filter.expr = query;
  desc.callback = vd__ecs_janet_callback;
  desc.binding_ctx = cb;
  e = ecs_system_init(w, &desc);

  return janet_wrap_u64(e);
}

static Janet vd__ecs_prefab(int32_t argc, Janet *argv)
{
  janet_arity(argc, 2, 3);

  ecs_world_t *w = janet_getpointer(argv, 0);
  const char *id = janet_getcstring(argv, 1);
  const char *sig = janet_optcstring(argv, argc, 2, NULL);

  ecs_entity_t e = ecs_entity_init(w, &(ecs_entity_desc_t){.name = id, .add_expr = sig, .add = {EcsPrefab}});
  return janet_wrap_u64(e);
}

static void vd__ecs_init_scope(ecs_world_t *w, ecs_entity_t id)
{
  ecs_entity_t scope = ecs_get_scope(w);

  if (scope)
    ecs_add_pair(w, id, EcsChildOf, scope);
}

static Janet vd__ecs_component(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 3);

  ecs_world_t *w = janet_getpointer(argv, 0);
  const uint8_t *name = janet_getkeyword(argv, 1);
  const char *desc = janet_getcstring(argv, 2);

  //   if(ecs_lookup_fullpath(ent_state.world, name) ||
  //   ecs_lookup(ent_state.world, name)) luaL_argerror(L, 1, "component already
  //   exists");

  ecs_entity_t component = ecs_entity_init(w, &(ecs_entity_desc_t){.use_low_id = true});

  ecs_set_name(w, component, (char *)name);

  int err = ecs_meta_from_desc(w, component, EcsStructType, desc);

  // if(err) return luaL_argerror(L, 2, "invalid descriptor");

  ecs_set(w, component, EcsMetaType, {.kind = EcsStructType});

  vd__ecs_init_scope(w, component);

  return janet_wrap_u64(component);
}

static void vd__ecs_deserialize_type(Janet *argv, int idx, ecs_meta_cursor_t *c, int depth)
{
  int ktype, vtype, ret, mtype, prev_ikey;
  bool in_array = false, designated_initializer = false;

  ret = ecs_meta_push(c);
  ecs_assert(!ret, ECS_INTERNAL_ERROR, NULL);
  in_array = ecs_meta_is_collection(c);

  const JanetKV *kv = NULL;
  const JanetDictView dict = janet_getdictionary(argv, idx);
  while ((kv = janet_dictionary_next(dict.kvs, dict.cap, kv)))
  {
    const char *key;
    if (janet_type(kv->key) == JANET_KEYWORD)
    {
      key = (const char *)janet_unwrap_keyword(kv->key);
      printf("meta_member field: %s\n", key);
      ret = ecs_meta_member(c, key);
    }

    if (janet_type(kv->value) == JANET_NUMBER)
    {
      double val = janet_unwrap_number(kv->value);
      printf("meta_member field: %s and value: %lf\n", key, val);
      ret = ecs_meta_set_float(c, val);
    }
  }
  ret = ecs_meta_pop(c);
  ecs_assert(!ret, ECS_INTERNAL_ERROR, NULL);
}

static ecs_meta_cursor_t *vd__ecs_janet_cursor(ecs_world_t *w, ecs_entity_t type, void *base)
{
  ecs_meta_cursor_t *cursor = janet_smalloc(sizeof(ecs_meta_cursor_t));
  *cursor = ecs_meta_cursor(w, type, base);
  return cursor;
}

static void vd__ecs_janet_to_ptr(ecs_world_t *w, Janet *argv, int idx, ecs_entity_t type, void *ptr)
{
  ecs_meta_cursor_t *c = vd__ecs_janet_cursor(w, type, ptr);
  vd__ecs_deserialize_type(argv, idx, c, 0);
}

static Janet vd__ecs_set(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 4);

  ecs_world_t *w = janet_getpointer(argv, 0);
  ecs_entity_t e = janet_getuinteger64(argv, 1);

  ecs_entity_t component =
    janet_checktype(argv[2], JANET_NUMBER)
      ? janet_getuinteger64(argv, 2)
      : ecs_get_type_info(
          w, *vd__ecs_component_registry_get(
               &vd__state.ecs.components, zpl_fnv64(janet_getcstring(argv, 2), zpl_strlen(janet_getcstring(argv, 2)))))->component;
  if (!e)
  {
    e = ecs_new_id(w);
    ecs_entity_t scope = ecs_get_scope(w);
    if (scope)
      ecs_add_pair(w, e, EcsChildOf, scope);
  }

  void *ptr = ecs_get_mut_id(w, e, component);
  vd__ecs_janet_to_ptr(w, argv, 3, component, ptr);
  ecs_modified_id(w, e, component);

  return janet_wrap_u64(e);
}

static Janet vd__ecs_new_id(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 1);

  ecs_world_t *w = janet_getpointer(argv, 0);

  return janet_wrap_u64(ecs_new_id(w));
}

static Janet vd__ecs_update(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 1);

  ecs_world_t *w = janet_getpointer(argv, 0);

  ecs_progress(w, 0);

  return janet_wrap_nil();
}

static Janet vd__ecs_create_world(int32_t argc, Janet *argv)
{
  ecs_world_t *w = ecs_init();

  ecs_id(Vec3) = ecs_struct_init(
    w, &(ecs_struct_desc_t){.entity = ecs_entity(w, {.name = "Vec3", .symbol = "HMM_Vec3", .id = ecs_id(Vec3)}),
                            .members = {{"x", ecs_id(ecs_f32_t)}, {"y", ecs_id(ecs_f32_t)}, {"z", ecs_id(ecs_f32_t)}}});

  ECS_META_COMPONENT(w, Transform);
  ECS_TAG(w, Renderable);

  vd__ecs_component_registry_init(&vd__state.ecs.components, zpl_heap_allocator());
  vd__ecs_component_registry_set(&vd__state.ecs.components,
                                 zpl_fnv64("Transform", zpl_strlen("Transform")), ecs_id(Transform));

  return janet_wrap_pointer(w);
}

static Janet vd__ecs_destroy_world(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 1);

  ecs_world_t *w = janet_getpointer(argv, 0);

  ecs_fini(w);

  return janet_wrap_nil();
}

//   __________  __
//  / ___/ __/ |/_/
// / (_ / _/_>  <
// \___/_/ /_/|_|
//
// >>gfx

void vd__gfx_render()
{
}

void vd__gfx_init()
{
  sg_setup(&(sg_desc){
    .context = sapp_sgcontext(),
    .logger.func = slog_func,
  });
}

//    ___   ___  ___
//   / _ | / _ \/ _ \
//  / __ |/ ___/ ___/
// /_/ |_/_/  /_/
//
// >>app

JanetSignal _vd_app_janet_pcall_keep_env(JanetFunction *fun, int32_t argc, const Janet *argv, Janet *out,
                                         JanetFiber **f)
{
  JanetFiber *fiber;
  if (f && *f)
  {
    JanetTable *env = (*f)->env;
    fiber = janet_fiber_reset(*f, fun, argc, argv);
    fiber->env = env;
  }
  else
  {
    fiber = janet_fiber(fun, 64, argc, argv);
  }
  if (f)
    *f = fiber;
  if (!fiber)
  {
    *out = janet_cstringv("arity mismatch");
    return JANET_SIGNAL_ERROR;
  }
  return janet_continue(fiber, janet_wrap_nil(), out);
}

void vd__app_init(void)
{
  vd__gfx_init();
  vd__dbg_draw_init();

  Janet ret;
  JanetSignal status =
    _vd_app_janet_pcall_keep_env(vd__state.app.mod_init_cb, 0, NULL, &ret, &vd__state.app.main_fiber);
  if (status == JANET_SIGNAL_ERROR)
  {
    printf("error loading game\n");
    janet_stacktrace(vd__state.app.main_fiber, ret);
    sapp_quit();
  }
}

void vd__app_update(void)
{
  vd__dbg_draw_cube();

  Janet ret;
  JanetSignal status =
    _vd_app_janet_pcall_keep_env(vd__state.app.mod_update_cb, 0, NULL, &ret, &vd__state.app.main_fiber);
  if (status == JANET_SIGNAL_ERROR)
  {
    printf("error loading game\n");
    janet_stacktrace(vd__state.app.main_fiber, ret);
    sapp_quit();
  }
}

void vd__app_shutdown(void)
{
  sg_shutdown();
  sargs_shutdown();
}

static void vd__janet_cdefs(JanetTable *env)
{
  janet_def(env, "ecs/on-update", janet_wrap_integer(EcsOnUpdate), "(voodoo/ecs/on-update)\n\nEcsOnUpdate");
}

static const JanetReg vd__cfuns[] = {
  {"ecs/create-world", vd__ecs_create_world, "(voodoo/ecs/create-world)\n\nCreate a new world."},
  {"ecs/component", vd__ecs_component, "(voodoo/ecs/component)\n\nDefine a new component."},
  {"ecs/prefab", vd__ecs_prefab, "(voodoo/ecs/prefab)\n\nDefine a new prefab."},
  //     // {"ecs/field", vecs_field, "(voodoo/ecs/field)\n\nTODO."},
  {"ecs/new-id", vd__ecs_new_id, "(voodoo/ecs/new-id)\n\nCreate a new entity id."},
  {"ecs/update", vd__ecs_update,
   "(voodoo/ecs/update)\n\nUpdate a world by running systems against"
   "matching entities."},
  {"ecs/set", vd__ecs_set, "(voodoo/ecs/set)\n\nSet the value of a component."},
  {"ecs/system", vd__ecs_system, "(voodoo/ecs/system)\n\nDefines a new system."},
  {"ecs/destroy-world", vd__ecs_destroy_world, "(voodoo/ecs/destroy-world)\n\nDestroy a world."},
  {NULL, NULL, NULL}};

sapp_desc sokol_main(int argc, char *argv[])
{
  sargs_setup(&(sargs_desc){.argc = argc, .argv = argv});

  const char *mod = NULL, *err = NULL;
  if (sargs_exists("run"))
  {
    mod = sargs_value("run");
  }

  if (!mod)
  {
    fprintf(stderr, "missing run argument");
    sargs_shutdown();
    exit(1);
  }

  janet_init();
  JanetTable *core_env = janet_core_env(NULL);
  JanetTable *lookup = janet_env_lookup(core_env);

  vd__janet_cdefs(core_env);
  janet_cfuns(core_env, NULL, vd__cfuns);

  Janet ret;
  JanetSignal status = janet_dostring(core_env,
                                      "(setdyn :syspath \"./assets/scripts\")\n"
                                      "(import game :prefix \"\")\n"
                                      // "@{:init init :frame frame :shutdown shutdown}",
                                      "(voodoo)",
                                      "main", &ret);

  if (status == JANET_SIGNAL_ERROR)
  {
    fprintf(stderr, "missing run argument");
    sargs_shutdown();
    exit(1);
  }

  JanetTable *cfg = janet_unwrap_table(ret);
  vd__state.app.mod_init_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("init")));
  vd__state.app.mod_update_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("update")));
  vd__state.app.mod_shutdown_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("shutdown")));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_init_cb));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_update_cb));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_shutdown_cb));

  return (sapp_desc){
    .init_cb = vd__app_init,
    .frame_cb = vd__app_update,
    .cleanup_cb = vd__app_shutdown,
    .width = janet_unwrap_integer(janet_table_rawget(cfg, janet_ckeywordv("width"))),
    .height = janet_unwrap_integer(janet_table_rawget(cfg, janet_ckeywordv("height"))),
    .sample_count = 4,
    .window_title = "Cube (sokol-app)",
    .icon.sokol_default = true,
    .logger.func = slog_func,
  };
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // VOODOO_IMPL