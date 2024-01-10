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

#include "janet.h"

extern void jsfun_vd_app_update(void);

static Janet cfun_vd_app_width(int32_t argc, Janet *argv);
static Janet cfun_vd_app_height(int32_t argc, Janet *argv);
static Janet cfun_vd_cam_new(int32_t argc, Janet *argv);
static Janet cfun_vd_cam_handle_event(int32_t argc, Janet *argv);
static Janet cfun_vd_cam_update(int32_t argc, Janet *argv);
static Janet cfun_vd_dbg_draw_camera(int32_t argc, Janet *argv);
static Janet cfun_vd_dbg_draw_cube(int32_t argc, Janet *argv);
static Janet cfun_vd_dbg_draw_grid(int32_t argc, Janet *argv);

static void vd__janet_cdefs(JanetTable *env)
{
}

static const JanetReg vd__cfuns[] = {
  {"app/width", cfun_vd_app_width, "(voodoo/app/width)\n\nWidth of the current application."},
  {"app/height", cfun_vd_app_height, "(voodoo/app/height)\n\nHeight of the current application."},
  {"cam/new", cfun_vd_cam_new, "(voodoo/cam/new)\n\nCreates a new camera."},
  {"cam/handle-event", cfun_vd_cam_handle_event,
   "(voodoo/cam/handle-event)\n\nHandle an event with an existing camera."},
  {"cam/update", cfun_vd_cam_update, "(voodoo/cam/update)\n\nUpdate an existing camera."},
  {"dbg/draw/camera", cfun_vd_dbg_draw_camera,
   "(voodoo/dbg/draw/camera)\n\nSet camera matricies for debug draw operations."},
  {"dbg/draw/cube", cfun_vd_dbg_draw_cube, "(voodoo/dbg/draw/cube)\n\nDraw a debug cube."},
  {"dbg/draw/grid", cfun_vd_dbg_draw_grid, "(voodoo/dbg/draw/grid)\n\nDraw a debug grid."},
  {NULL, NULL, NULL}};

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
#include "hmm.h"
#define SOKOL_IMPL
#define SOKOL_WGPU
#include "sokol_app.h"
#include "sokol_args.h"
#include "sokol_gfx.h"
#include "sokol_gl.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_shape.h"
#include "sokol_time.h"
#include "shapes.glsl.h"

//    _____________  __  _________________
//   / __/_  __/ _ \/ / / / ___/_  __/ __/
//  _\ \  / / / , _/ /_/ / /__  / / _\ \
// /___/ /_/ /_/|_|\____/\___/ /_/ /___/
//
// >>structs

enum
{
  VD__BOX = 0,
  VD__PLANE,
  VD__SPHERE,
  VD__CYLINDER,
  VD__TORUS,
  VD__NUM_SHAPES
};

typedef struct
{
  HMM_Vec3 pos;
  sshape_element_range_t draw;
} vd__dbg_shape;

static struct
{
  struct
  {
    JanetFiber *main_fiber;
    JanetFunction *mod_init_cb;
    JanetFunction *mod_event_cb;
    JanetFunction *mod_update_cb;
    JanetFunction *mod_shutdown_cb;
  } app;

  struct
  {
    struct
    {
      struct
      {
        sg_buffer buf;
        int offset;
      } transform;
      HMM_Mat4 vp;

      sg_buffer idx_buf;
      sg_buffer vtx_buf;
      sg_shader shd;
      sg_pipeline pip;

      vd__dbg_shape shapes[VD__NUM_SHAPES];

    } draw;
  } dbg;
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
static const HMM_Mat4 vd__identity4 = {
  .Elements = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}};

static float vd__idx_getfloat(JanetView idx, int index)
{
  if (index >= idx.len)
  {
    janet_panicf("index %d outside of range [0, %d)", idx.len);
  }
  if (!janet_checktype(idx.items[index], JANET_NUMBER))
  {
    janet_panicf("expected number, got %v", idx.items[index]);
  }
  return (float)janet_unwrap_number(idx.items[index]);
}

static HMM_Vec3 vd__unwrap_vec3(const Janet val)
{
  JanetView view;
  if (!janet_indexed_view(val, &view.items, &view.len))
  {
    janet_panic("expected vec to be an indexed type");
  }

  if (view.len != 3)
  {
    janet_panic("vec must have exactly 2 elements");
  }

  float x = vd__idx_getfloat(view, 0);
  float y = vd__idx_getfloat(view, 1);
  float z = vd__idx_getfloat(view, 2);
  return HMM_V3(x, y, z);
}

//   ________   __  __________  ___
//  / ___/ _ | /  |/  / __/ _ \/ _ |
// / /__/ __ |/ /|_/ / _// , _/ __ |
// \___/_/ |_/_/  /_/___/_/|_/_/ |_|
//
// >>camera

static const JanetAbstractType vd__cam_camera = {"voodoo/cam/camera", JANET_ATEND_NAME};
static const JanetAbstractType vd__cam_event = {"voodoo/cam/event", JANET_ATEND_NAME};

#define CAMERA_DEFAULT_MIN_DIST (2.0f)
#define CAMERA_DEFAULT_MAX_DIST (30.0f)
#define CAMERA_DEFAULT_MIN_LAT (-85.0f)
#define CAMERA_DEFAULT_MAX_LAT (85.0f)
#define CAMERA_DEFAULT_DIST (5.0f)
#define CAMERA_DEFAULT_ASPECT (vd__pi_div2)
#define CAMERA_DEFAULT_NEARZ (0.01f)
#define CAMERA_DEFAULT_FARZ (100.0f)

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
} vd__camera_desc;

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
  HMM_Vec3 eye_pos;
  HMM_Mat4 view;
  HMM_Mat4 proj;
  HMM_Mat4 view_proj;
} vd__camera;

static float vd__cam_def(float val, float def)
{
  return ((val == 0.0f) ? def : val);
}

static void vd__cam_init(vd__camera *cam, const vd__camera_desc *desc)
{
  assert(cam && desc);
  memset(cam, 0, sizeof(vd__camera));
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
  cam->view = HMM_M4D(1.0f);
  cam->proj = HMM_M4D(1.0f);
  cam->view_proj = HMM_M4D(1.0f);
}

static void vd__cam_orbit(vd__camera *cam, float dx, float dy)
{
  assert(cam);
  cam->longitude -= dx;
  if (cam->longitude < 0.0f)
  {
    cam->longitude += 360.0f;
  }
  if (cam->longitude > 360.0f)
  {
    cam->longitude -= 360.0f;
  }
  cam->latitude = HMM_Clamp(cam->min_lat, cam->latitude + dy, cam->max_lat);
}

static void vd__cam_zoom(vd__camera *cam, float d)
{
  assert(cam);
  cam->distance = HMM_Clamp(cam->min_dist, cam->distance + d, cam->max_dist);
}

static HMM_Vec3 vd__cam_euclidean(float latitude, float longitude)
{
  const float lat = HMM_AngleDeg(latitude);
  const float lng = HMM_AngleDeg(longitude);
  return HMM_V3(cosf(lat) * sinf(lng), sinf(lat), cosf(lat) * cosf(lng));
}

static void vd__cam_update(vd__camera *cam, float fb_width, float fb_height)
{
  assert(cam);
  assert((fb_width > 0) && (fb_height > 0));
  const float w = fb_width;
  const float h = fb_height;
  cam->eye_pos = HMM_AddV3(cam->center, HMM_MulV3F(vd__cam_euclidean(cam->latitude, cam->longitude), cam->distance));
  cam->view = HMM_LookAt_RH(cam->eye_pos, cam->center, HMM_V3(0.0f, 1.0f, 0.0f));
  cam->proj = HMM_Perspective_RH_ZO(cam->aspect, w / h, cam->nearz, cam->farz);
  cam->view_proj = HMM_MulM4(cam->proj, cam->view);
}

static void vd__cam_handle_event(vd__camera *cam, const sapp_event *ev)
{
  assert(cam);
  switch (ev->type)
  {
  case SAPP_EVENTTYPE_MOUSE_DOWN:
    if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT)
    {
      sapp_lock_mouse(true);
    }
    break;
  case SAPP_EVENTTYPE_MOUSE_UP:
    if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT)
    {
      sapp_lock_mouse(false);
    }
    break;
  case SAPP_EVENTTYPE_MOUSE_SCROLL:
    vd__cam_zoom(cam, ev->scroll_y * 0.5f);
    break;
  case SAPP_EVENTTYPE_MOUSE_MOVE:
    if (sapp_mouse_locked())
    {
      vd__cam_orbit(cam, ev->mouse_dx * 0.25f, ev->mouse_dy * 0.25f);
    }
    break;
  default:
    break;
  }
}

static Janet cfun_vd_cam_handle_event(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 2);

  if (!janet_checktype(argv[0], JANET_ABSTRACT))
  {
    janet_panic("expected camera");
  }

  if (!janet_checktype(argv[1], JANET_ABSTRACT))
  {
    janet_panic("expected event");
  }

  vd__cam_handle_event(janet_unwrap_abstract(argv[0]), janet_unwrap_abstract(argv[1]));
  return janet_wrap_nil();
}

static Janet cfun_vd_cam_update(int32_t argc, Janet *argv)
{
  janet_arity(argc, 1, 3);

  if (!janet_checktype(argv[0], JANET_ABSTRACT))
  {
    janet_panic("expected camera");
  }

  float fb_width, fb_height;
  if (argc == 3 && janet_checktype(argv[1], JANET_NUMBER) && janet_checktype(argv[2], JANET_NUMBER))
  {
    fb_width = janet_unwrap_number(argv[1]);
    fb_height = janet_unwrap_number(argv[2]);
  }
  else if (argc == 1)
  {
    fb_width = sapp_widthf();
    fb_height = sapp_heightf();
  }
  else
  {
    janet_panic("expected numbers for width and height");
  }

  vd__cam_update(janet_getabstract(argv, 0, &vd__cam_camera), fb_width, fb_height);

  return janet_wrap_nil();
}

static Janet cfun_vd_cam_new(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 1);
  JanetTable *desc = janet_gettable(argv, 0);
  vd__camera *cam = janet_abstract(&vd__cam_camera, sizeof(vd__camera));
  vd__cam_init(cam, &(vd__camera_desc){
                      .min_dist = janet_unwrap_number(janet_table_rawget(desc, janet_ckeywordv("min-dist"))),
                      .max_dist = janet_unwrap_number(janet_table_rawget(desc, janet_ckeywordv("max-dist"))),
                      .center = vd__unwrap_vec3(janet_table_rawget(desc, janet_ckeywordv("center"))),
                      .distance = janet_unwrap_number(janet_table_rawget(desc, janet_ckeywordv("distance"))),
                      .latitude = janet_unwrap_number(janet_table_rawget(desc, janet_ckeywordv("latitude"))),
                      .longitude = janet_unwrap_number(janet_table_rawget(desc, janet_ckeywordv("longitude"))),
                    });
  return janet_wrap_abstract(cam);
}

//   __________  __
//  / ___/ __/ |/_/
// / (_ / _/_>  <
// \___/_/ /_/|_|
//
// >>gfx

sg_buffer vd__gfx_buffer_append(sg_buffer buf, size_t size, const void *data, int *offset)
{
  if (buf.id == SG_INVALID_ID || sg_query_buffer_will_overflow(buf, size))
  {
    sg_buffer_info bi = sg_query_buffer_info(buf);
    buf = sg_make_buffer(&(sg_buffer_desc){.size = bi.append_pos + size, .usage = SG_USAGE_STREAM});
  }
  *offset = sg_append_buffer(buf, &(sg_range){.ptr = data, .size = size});
  return buf;
}

void vd__gfx_init()
{
  sg_setup(&(sg_desc){
    .context = sapp_sgcontext(),
    .logger.func = slog_func,
  });

  sgl_setup(&(sgl_desc_t){
    .logger.func = slog_func,
  });
}

void vd__gfx_shutdown()
{
  sgl_shutdown();
  sg_shutdown();
}

//    ___  _______  __  _______  ___  ___  ___ _      __
//   / _ \/ __/ _ )/ / / / ___/ / _ \/ _ \/ _ | | /| / /
//  / // / _// _  / /_/ / (_ / / // / , _/ __ | |/ |/ /
// /____/___/____/\____/\___/ /____/_/|_/_/ |_|__/|__/
//
// >>dbg

void vd__dbg__draw_ground(float scale)
{
  sgl_c3f(1.0f, 1.0f, 1.0f);
  sgl_begin_lines();
  for (float i = -scale, c = 0; c <= 20; c += 20, i += c * (scale / 10))
  {
    sgl_v3f(-scale, 0, i);
    sgl_v3f(+scale, 0, i);
    sgl_v3f(i, 0, -scale);
    sgl_v3f(i, 0, +scale);
  }
  sgl_c3f(0.5843f, 0.5843f, 0.5843f);
  for (float i = -scale + scale / 10, c = 1; c < 20; ++c, i += (scale / 10))
  {
    sgl_v3f(-scale, 0, i);
    sgl_v3f(+scale, 0, i);
    sgl_v3f(i, 0, -scale);
    sgl_v3f(i, 0, +scale);
  }
  sgl_end();
}

void vd__dbg_draw_ground(float scale)
{
  if (scale)
  {
    vd__dbg__draw_ground(scale);
  }
  else
  {
    vd__dbg__draw_ground(100.0f);
    vd__dbg__draw_ground(10.0f);
    vd__dbg__draw_ground(1.0f);
    vd__dbg__draw_ground(01.0f);
    vd__dbg__draw_ground(001.0f);
  }
}

void vd__dbg_draw_cube(HMM_Vec3 pos)
{
  HMM_Mat4 transform = HMM_Translate(pos);
  vd__state.dbg.draw.transform.buf = vd__gfx_buffer_append(vd__state.dbg.draw.transform.buf, sizeof(HMM_Mat4),
                                                           &transform, &vd__state.dbg.draw.transform.offset);
}

void vd__dbg_draw_camera(vd__camera *cam)
{
  vd__state.dbg.draw.vp = cam->view_proj;
}

void vd__dbg_draw()
{
  /* NOTE: the vs_params_t struct has been code-generated by the shader-code-gen */
  vs_params_t vs_params;
  vs_params.vp = vd__state.dbg.draw.vp;

  sgl_load_matrix(&vd__state.dbg.draw.vp.Elements[0][0]);

  sg_pass_action pass_action = {
    .colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.13f, 0.13f, 0.13f, 1.0f}}};
  sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());

  sg_apply_pipeline(vd__state.dbg.draw.pip);

  sg_apply_bindings(&(sg_bindings){.vertex_buffers[0] = vd__state.dbg.draw.vtx_buf,
                                   .vertex_buffers[1] = vd__state.dbg.draw.transform.buf,
                                   .vertex_buffer_offsets[1] = vd__state.dbg.draw.transform.offset,
                                   .index_buffer = vd__state.dbg.draw.idx_buf});

  sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
  sg_draw(vd__state.dbg.draw.shapes[VD__BOX].draw.base_element, vd__state.dbg.draw.shapes[VD__BOX].draw.num_elements,
          1);
  sgl_draw();
  sg_end_pass();
  sg_commit();
}

void vd__dbg_draw_init()
{
  /* create shader */
  vd__state.dbg.draw.shd = sg_make_shader(shapes_shader_desc(sg_query_backend()));

  /* create pipeline object */
  vd__state.dbg.draw.pip = sg_make_pipeline(&(sg_pipeline_desc){
    .shader = vd__state.dbg.draw.shd,
    .layout = {.buffers = {[0] = sshape_vertex_buffer_layout_state(),
                           [1] = {.stride = sizeof(HMM_Mat4), .step_func = SG_VERTEXSTEP_PER_INSTANCE}},
               .attrs = {[0] = sshape_position_vertex_attr_state(),
                         [1] = sshape_normal_vertex_attr_state(),
                         [2] = sshape_texcoord_vertex_attr_state(),
                         [3] = sshape_color_vertex_attr_state(),
                         [4] = {.buffer_index = 1, .offset = 0, .format = SG_VERTEXFORMAT_FLOAT4},
                         [5] = {.buffer_index = 1, .offset = 16, .format = SG_VERTEXFORMAT_FLOAT4},
                         [6] = {.buffer_index = 1, .offset = 32, .format = SG_VERTEXFORMAT_FLOAT4},
                         [7] = {.buffer_index = 1, .offset = 48, .format = SG_VERTEXFORMAT_FLOAT4}}},
    .index_type = SG_INDEXTYPE_UINT16,
    .cull_mode = SG_CULLMODE_NONE,
    .depth = {.compare = SG_COMPAREFUNC_LESS_EQUAL, .write_enabled = true},
  });

  sshape_vertex_t vertices[6 * 1024];
  uint16_t indices[16 * 1024];
  sshape_buffer_t buf = {
    .vertices.buffer = SSHAPE_RANGE(vertices),
    .indices.buffer = SSHAPE_RANGE(indices),
  };
  buf = sshape_build_box(&buf, &(sshape_box_t){
                                 .width = 1.0f,
                                 .height = 1.0f,
                                 .depth = 1.0f,
                                 .tiles = 10,
                                 .random_colors = true,
                               });
  vd__state.dbg.draw.shapes[VD__BOX].draw = sshape_element_range(&buf);
  buf = sshape_build_plane(&buf, &(sshape_plane_t){
                                   .width = 1.0f,
                                   .depth = 1.0f,
                                   .tiles = 10,
                                   .random_colors = true,
                                 });
  vd__state.dbg.draw.shapes[VD__PLANE].draw = sshape_element_range(&buf);
  buf = sshape_build_sphere(&buf, &(sshape_sphere_t){
                                    .radius = 0.75f,
                                    .slices = 36,
                                    .stacks = 20,
                                    .random_colors = true,
                                  });
  vd__state.dbg.draw.shapes[VD__SPHERE].draw = sshape_element_range(&buf);
  buf = sshape_build_cylinder(&buf, &(sshape_cylinder_t){
                                      .radius = 0.5f,
                                      .height = 1.5f,
                                      .slices = 36,
                                      .stacks = 10,
                                      .random_colors = true,
                                    });
  vd__state.dbg.draw.shapes[VD__CYLINDER].draw = sshape_element_range(&buf);
  buf = sshape_build_torus(&buf, &(sshape_torus_t){
                                   .radius = 0.5f,
                                   .ring_radius = 0.3f,
                                   .rings = 36,
                                   .sides = 18,
                                   .random_colors = true,
                                 });
  vd__state.dbg.draw.shapes[VD__TORUS].draw = sshape_element_range(&buf);
  assert(buf.valid);

  // one vertex/index-buffer-pair for all shapes
  const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
  const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
  vd__state.dbg.draw.vtx_buf = sg_make_buffer(&vbuf_desc);
  vd__state.dbg.draw.idx_buf = sg_make_buffer(&ibuf_desc);
}

static Janet cfun_vd_dbg_draw_camera(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 1);

  if (!janet_checktype(argv[0], JANET_ABSTRACT))
  {
    janet_panic("expected camera");
  }

  vd__dbg_draw_camera(janet_getabstract(argv, 0, &vd__cam_camera));
  return janet_wrap_nil();
}

static Janet cfun_vd_dbg_draw_cube(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 1);
  vd__dbg_draw_cube(vd__unwrap_vec3(argv[0]));
  return janet_wrap_nil();
}

static Janet cfun_vd_dbg_draw_grid(int32_t argc, Janet *argv)
{
  janet_fixarity(argc, 1);

  if (!janet_checktype(argv[0], JANET_NUMBER))
  {
    janet_panicf("expected number, got %v", argv[0]);
  }

  vd__dbg_draw_ground(janet_unwrap_number(argv[0]));

  return janet_wrap_nil();
}

//    ___   ___  ___
//   / _ | / _ \/ _ \
//  / __ |/ ___/ ___/
// /_/ |_/_/  /_/
//
// >>app

static Janet cfun_vd_app_width(int32_t argc, Janet *argv)
{
  return janet_wrap_integer(sapp_width());
}

static Janet cfun_vd_app_height(int32_t argc, Janet *argv)
{
  return janet_wrap_integer(sapp_height());
}

JanetSignal vd__app_janet_pcall_keep_env(JanetFunction *fun, int32_t argc, const Janet *argv, Janet *out,
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
    vd__app_janet_pcall_keep_env(vd__state.app.mod_init_cb, 0, NULL, &ret, &vd__state.app.main_fiber);
  if (status == JANET_SIGNAL_ERROR)
  {
    printf("error loading game\n");
    janet_stacktrace(vd__state.app.main_fiber, ret);
    sapp_quit();
  }
}

void vd__app_event(const sapp_event *ev)
{
  Janet evv = janet_wrap_abstract(ev);

  Janet ret;
  JanetSignal status =
    vd__app_janet_pcall_keep_env(vd__state.app.mod_event_cb, 1, &evv, &ret, &vd__state.app.main_fiber);
  if (status == JANET_SIGNAL_ERROR)
  {
    printf("error calling app event handler\n");
    janet_stacktrace(vd__state.app.main_fiber, ret);
    sapp_quit();
  }
}

void vd__app_update(void)
{
  // jsfun_vd_app_update();
  // EM_ASM({ console.log(Module.Signal.get(Module.signals.scriptDirty)); });

  Janet ret;
  JanetSignal status =
    vd__app_janet_pcall_keep_env(vd__state.app.mod_update_cb, 0, NULL, &ret, &vd__state.app.main_fiber);
  if (status == JANET_SIGNAL_ERROR)
  {
    printf("error loading game\n");
    janet_stacktrace(vd__state.app.main_fiber, ret);
    sapp_quit();
  }

  vd__dbg_draw();
}

void vd__app_shutdown(void)
{
  vd__gfx_shutdown();
  sargs_shutdown();
}

sapp_desc sokol_main(int argc, char *argv[])
{
  // EM_ASM({ console.log(FS.readFile("assets/scripts/game.janet", {encoding : "utf8"})); });

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
  vd__state.app.mod_event_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("event")));
  vd__state.app.mod_update_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("update")));
  vd__state.app.mod_shutdown_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("shutdown")));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_init_cb));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_event_cb));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_update_cb));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_shutdown_cb));

  return (sapp_desc){
    .init_cb = vd__app_init,
    .event_cb = vd__app_event,
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