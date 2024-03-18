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

// static Janet cfun_vd_ecs_component(int32_t argc, Janet *argv);

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
  // {"ecs/component", cfun_vd_ecs_component, "(voodoo/ecs/component)\n\nCreate a component."},
  // {"v3d/doll", cfun_vd_v3d_doll, "(voodoo/v3d/doll)\n\nCreate a doll asset."}
  {NULL, NULL, NULL}};
#endif // VOODOO_INCLUDED

//     ______  _______  __    ________  __________   ___________  ______________  _   __
//    /  _/  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/  _/ __ \/ | / /
//    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
//  _/ // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ / _/ // /_/ / /|  /
// /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
//
// >>implementation

#define VOODOO_IMPL
#ifdef VOODOO_IMPL
#define VOODOO_IMPL_INCLUDED (1)

#ifdef _MSC_VER
#pragma warning(push)
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "sx/allocator.h"
#include "sx/array.h"
#include "sx/atomic.h"
#include "sx/handle.h"
#include "sx/hash.h"
#include "sx/io.h"
#include "sx/jobs.h"
#include "sx/lin-alloc.h"
#include "sx/lockless.h"
#include "sx/os.h"
#include "sx/platform.h"
#include "sx/pool.h"
#include "sx/string.h"
#include "sx/threads.h"
#include "sx/vmem.h"

#include "stackwalkerc.h"

#define CJ5_IMPLEMENT
#include "cj5.h"

#define FLECS_CUSTOM_BUILD
#define FLECS_MODULE
#define FLECS_SYSTEM
#define FLECS_META
#define FLECS_META_C

#include "flecs.h"

#define HANDMADE_MATH_IMPLEMENTATION
#include "hmm.h"
#include "ozz_util.h"
#define SOKOL_IMPL
#define SOKOL_WGPU
#include "sokol_app.h"
#include "sokol_args.h"
#include "sokol_fetch.h"
#include "sokol_gfx.h"
#include "sokol_gl.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_shape.h"
#include "sokol_time.h"

#include "display.glsl.h"
// #include "shadows.glsl.h"
#include "shapes.glsl.h"

#define vd__to_id(index) ((uint32_t)(index) + 1)
#define vd__to_index(id) ((int)(id)-1)

static const sx_alloc *vd__core_heap_alloc(void);
int64_t vd__core_frame_index();
int vd__app_width();
int vd__app_height();

//   _________  _  _______________
//  / ___/ __ \/ |/ / __/  _/ ___/
// / /__/ /_/ /    / _/_/ // (_ /
// \___/\____/_/|_/_/ /___/\___/
//
// >>config

#ifndef VD__MAX_PATH
#define VD__MAX_PATH 256
#endif

#ifndef VD__CONFIG_ASSET_POOL_SIZE
#define VD__CONFIG_ASSET_POOL_SIZE 256
#endif

#ifndef VD__CONFIG_SHADOW_MAP_SIZE
#define VD__CONFIG_SHADOW_MAP_SIZE 4096
#endif

//    __   ____  _______________  _______
//   / /  / __ \/ ___/ ___/  _/ |/ / ___/
//  / /__/ /_/ / (_ / (_ // //    / (_ /
// /____/\____/\___/\___/___/_/|_/\___/
//
// >>logging

typedef enum
{
  VD__LOG_LEVEL_ERROR = 0,
  VD__LOG_LEVEL_WARNING,
  VD__LOG_LEVEL_INFO,
  VD__LOG_LEVEL_VERBOSE,
  VD__LOG_LEVEL_DEBUG,
  VD__LOG_LEVEL_COUNT
} vd__log_level;

void (*vd__core_print_info)(uint32_t channels, const char *source_file, int line, const char *fmt, ...);
void (*vd__core_print_debug)(uint32_t channels, const char *source_file, int line, const char *fmt, ...);
void (*vd__core_print_verbose)(uint32_t channels, const char *source_file, int line, const char *fmt, ...);
void (*vd__core_print_error)(uint32_t channels, const char *source_file, int line, const char *fmt, ...);
void (*vd__core_print_warning)(uint32_t channels, const char *source_file, int line, const char *fmt, ...);
void (*vd__core_set_log_level)(vd__log_level level);

#define vd__log_info(_text, ...) vd__core_print_info(0, __FILE__, __LINE__, _text, ##__VA_ARGS__)
#define vd__log_debug(_text, ...) vd__core_print_debug(0, __FILE__, __LINE__, _text, ##__VA_ARGS__)
#define vd__log_verbose(_text, ...) vd__core_print_verbose(0, __FILE__, __LINE__, _text, ##__VA_ARGS__)
#define vd__log_error(_text, ...) vd__core_print_error(0, __FILE__, __LINE__, _text, ##__VA_ARGS__)
#define vd__log_warn(_text, ...) vd__core_print_warning(0, __FILE__, __LINE__, _text, ##__VA_ARGS__)

#define vd__log_info_channels(_channels, _text, ...)                                                                   \
  vd__core_print_info((_channels), __FILE__, __LINE__, _text, ##__VA_ARGS__)
#define vd__log_debug_channels(_channels, _text, ...)                                                                  \
  vd__core_print_debug((_channels), __FILE__, __LINE__, _text, ##__VA_ARGS__)
#define vd__log_verbose_channels(_channels, _text, ...)                                                                \
  vd__core_print_verbose((_channels), __FILE__, __LINE__, _text, ##__VA_ARGS__)
#define vd__log_error_channels(_channels, _text, ...)                                                                  \
  vd__core_print_error((_channels), __FILE__, __LINE__, _text, ##__VA_ARGS__)
#define vd__log_warn_channels(_channels, _text, ...)                                                                   \
  vd__core_print_warning((_channels), __FILE__, __LINE__, _text, ##__VA_ARGS__)

//    _____________  __  _________________
//   / __/_  __/ _ \/ / / / ___/_  __/ __/
//  _\ \  / / / , _/ /_/ / /__  / / _\ \
// /___/ /_/ /_/|_|\____/\___/ /_/ /___/
//
// >>structs

typedef union {
  uintptr_t id;
  void *ptr;
} vd__asset_obj;
typedef struct
{
  uint32_t id;
} vd__asset_handle;
typedef struct
{
  uint32_t id;
} vd__asset_group_handle;

enum
{
  VD__ASSET_LOAD_FLAG_NONE = 0x0,
  VD__ASSET_LOAD_FLAG_ABSOLUTE_PATH = 0x1,
  VD__ASSET_LOAD_FLAG_WAIT_ON_LOAD = 0x2,
  VD__ASSET_LOAD_FLAG_RELOAD = 0x4,
  VD__ASSET_LOAD_FLAG_TEXT_FILE = 0x8
};
typedef uint32_t vd__asset_load_flags;

typedef struct
{
  char key[32];
  char value[32];
} vd__asset_meta_keyval;

typedef struct
{
  const char *path;           // path to asset file
  const void *params;         // must cast to asset-specific implementation type
  const sx_alloc *alloc;      // allocator that is user sends for loading asset data
  uint32_t tags;              // user-defined tag bits
  vd__asset_load_flags flags; // flags that are used for loading
  uint32_t num_meta;          // meta key-value pairs, embedded in custom _vd__ assets
  const vd__asset_meta_keyval *metas;
} vd__asset_load_params;

typedef struct
{
  vd__asset_obj obj; // valid internal object
  void *user1;       // user-data can be allocated and filled with anything specific to asset loader
  void *user2;       // same as user1
} vd__asset_load_data;

typedef enum
{
  VD__ASSET_STATE_ZOMBIE = 0,
  VD__ASSET_STATE_OK,
  VD__ASSET_STATE_FAILED,
  VD__ASSET_STATE_LOADING
} vd__asset_state;

typedef struct
{
  vd__asset_load_data (*on_prepare)(const vd__asset_load_params *params, const sx_mem_block *mem);
  bool (*on_load)(vd__asset_load_data *data, const vd__asset_load_params *params, const sx_mem_block *mem);
  void (*on_finalize)(vd__asset_load_data *data, const vd__asset_load_params *params, const sx_mem_block *mem);
  void (*on_reload)(vd__asset_handle handle, vd__asset_obj prev_obj, const sx_alloc *alloc);
  void (*on_release)(vd__asset_obj obj, const sx_alloc *alloc);
} vd__asset_callbacks;

// fourcc code for embedded asset meta data
static uint32_t VD__ASSET_FLAG = sx_makefourcc('R', 'I', 'Z', 'Z');

// Asset managers are managers for each type of asset
// For example, 'texture' has it's own manager, 'model' has it's manager, ...
// They handle loading, unloading, reloading asset objects
// They also have metdata and parameters memory containers
typedef struct vd__asset_mgr
{
  char name[32];
  vd__asset_callbacks callbacks;
  uint32_t name_hash;
  int params_size;
  char params_type_name[32];
  vd__asset_obj failed_obj;
  vd__asset_obj async_obj;
  uint8_t *SX_ARRAY params_buff;     // (byte-array, item-size: params_size)
  vd__asset_load_flags forced_flags; // these flags are foced upon every load-call
  bool unreg;
} vd__asset_mgr;

// Assets consist of files (resource) on disk and load params combination
// One file may be loaded with different parameters (different allocator, mip-map, LOD, etc.)
// Each asset is binded to a resource and params data
typedef struct
{
  const sx_alloc *alloc;
  sx_handle_t handle;
  uint32_t params_id;   // id-to: vd__asset_mgr.params_buff
  uint32_t resource_id; // id-to: vd__asset_lib.resources
  int asset_mgr_id;     // index-to: vd__asset_lib.asset_mgrs
  int ref_count;
  vd__asset_obj obj;
  vd__asset_obj dead_obj;
  uint32_t hash;
  uint32_t tags;
  vd__asset_load_flags load_flags;
  vd__asset_state state;
} vd__asset;

// Resources are the actual files on the file-system
// Each resource has a metadata
// Likely to be populated by asset-db, but can grow on run-time
typedef struct
{
  char path[VD__MAX_PATH];      // path that is referenced in database and code
  char real_path[VD__MAX_PATH]; // real path on disk, resolved by asset-db and variation
  uint32_t path_hash;           // hash of 'real_path'
  uint64_t last_modified;       // last-modified time-stamp
  int asset_mgr_id;             // index-to: vd__asset_lib.asset_mgrs
  bool used;
} vd__asset_resource;

// Async loads are queued for each new async file loads
// To track which file points to which asset
typedef struct
{
  uint32_t path_hash; // hash of real_path
  vd__asset_handle asset;
} vd__asset_async_load_req;

typedef enum
{
  VD__ASSET_JOB_STATE_SPAWN = 0,
  VD__ASSET_JOB_STATE_LOAD_FAILED,
  VD__ASSET_JOB_STATE_SUCCESS,
  VD__ASSET_JOB_STATE_COUNT = INT32_MAX
} vd__asset_job_state;

typedef struct vd__asset_async_job
{
  vd__asset_load_data load_data;
  sx_mem_block *mem;
  vd__asset_mgr *amgr;
  vd__asset_load_params lparams;
  sx_job_t job;
  vd__asset_job_state state;
  vd__asset_handle asset;
  struct vd__asset_async_job *next;
  struct vd__asset_async_job *prev;
} vd__asset_async_job;

typedef struct vd__asset_group
{
  vd__asset_handle *SX_ARRAY assets;
} vd__asset_group;

enum
{
  VD__CORE_FLAG_LOG_TO_FILE = 0x01,        // log to file defined by `app_name.log`
  VD__CORE_FLAG_LOG_TO_PROFILER = 0x02,    // log to remote profiler
  VD__CORE_FLAG_PROFILE_GPU = 0x04,        // enable GPU profiling
  VD__CORE_FLAG_DUMP_UNUSED_ASSETS = 0x08, // write `unused-assets.json` on exit
  VD__CORE_FLAG_DETECT_LEAKS = 0x10,       // Detect memory leaks (default on in _DEBUG builds)
  VD__CORE_FLAG_HEAP_TEMP_ALLOCATOR =
    0x20, // Replace temp allocator backends with heap, so we can better trace out-of-bounds and corruption
  VD__CORE_FLAG_HOT_RELOAD_PLUGINS =
    0x40, // Enables hot reloading for all modules and plugins including the game itself
  VD__CORE_FLAG_TRACE_TEMP_ALLOCATOR =
    0x80 // Enable memory tracing on temp allocators, slows them down, but provides more insight on temp allocations
};
typedef uint32_t vd__core_flags;

typedef struct
{
  vd__core_flags core_flags;
  vd__log_level log_level; // default = VD__LOG_LEVEL_INFO

  int job_num_threads;
  int job_max_fibers;
  int job_stack_size;

  int tmp_mem_max;
} vd__config;

typedef struct vd__tmp_alloc vd__tmp_alloc;
typedef struct
{
  sx_alloc alloc;
  vd__tmp_alloc *owner;
  size_t end_offset;
  size_t start_offset;
  size_t start_lastptr_offset;
  uint32_t depth;
  const char *file;
  uint32_t line;
} vd__tmp_alloc_inst;

typedef struct vd__tmp_alloc
{
  sx_alloc *tracer;
  sx_vmem_context vmem;
  vd__tmp_alloc_inst *SX_ARRAY alloc_stack;
  sx_atomic_uint32 stack_depth;
  float wait_time;
  size_t peak;
  size_t frame_peak;
};

typedef struct
{
  void *ptr;
  size_t size;
  char file[32];
  uint32_t line;
} vd__tmp_alloc_heapmode_item;

typedef struct vd__tmp_alloc_heapmode vd__tmp_alloc_heapmode;
typedef struct
{
  sx_alloc alloc;
  vd__tmp_alloc_heapmode *owner;
  int item_idx;
  const char *file;
  uint32_t line;
} vd__tmp_alloc_heapmode_inst;

typedef struct vd__tmp_alloc_heapmode
{
  size_t offset;
  size_t max_size;
  size_t peak;
  size_t frame_peak;
  sx_atomic_uint32 stack_depth;
  float wait_time;
  vd__tmp_alloc_heapmode_item *SX_ARRAY items;
  vd__tmp_alloc_heapmode_inst *SX_ARRAY alloc_stack;
};

typedef struct
{
  bool init;
  uint32_t tid;
  float idle_tm;
  union {
    vd__tmp_alloc alloc;
    vd__tmp_alloc_heapmode heap_alloc;
  };

  sx_alloc *tracer_front; // This is the trace-allocator that is filled with trace data during
                          // the frame exec
  sx_alloc *tracer_back;  // This is the trace-allocator that information is saved from the
                          // previous frame and can be viewed in imgui
} vd__tmp_alloc_tls;

typedef enum
{
  VD__BOX = 0,
  VD__PLANE,
  VD__SPHERE,
  VD__CYLINDER,
  VD__TORUS,
  VD__NUM_SHAPES
} vd__dbg_shapes;

typedef struct
{
  HMM_Vec3 pos;
  sshape_element_range_t draw;
} vd__dbg_shape;

typedef struct vd__mem_trace_context vd__mem_trace_context;

typedef enum
{
  VD__MEM_ACTION_MALLOC = 0,
  VD__MEM_ACTION_FREE,
  VD__MEM_ACTION_REALLOC
} vd__mem_action;

typedef struct vd__mem_item
{
  vd__mem_trace_context *owner;
  uint16_t num_callstack_items; // = 0, then try to read 'source_file' and 'source_func'
  vd__mem_action action;
  size_t size;
  void *ptr;

  union {
    void *callstack[SW_MAX_FRAMES];

    struct
    {
      char source_file[128];
      char source_func[32];
      uint32_t source_line;
    };
  };

  uint32_t callstack_hash;
  struct vd__mem_item *next;
  struct vd__mem_item *prev;
  int64_t frame; // record frame number
  bool freed;    // for mallocs, reallocs that got freed, but we want to keep the record
} vd__mem_item;

typedef struct
{
  int item_idx;
  vd__mem_item *item;
  uint32_t count;
  char entry_symbol[64];
  size_t size;
} vd__mem_item_collapsed;

typedef struct vd__mem_trace_context
{
  char name[32];
  char name_view[32];
  sx_alloc my_alloc;       // all allocations are redirected from this to redirect_alloc
  sx_alloc redirect_alloc; // receives all alloc calls and performs main allocations
  uint32_t name_hash;
  uint32_t options;
  bool disabled;
  bool viewDisabled;
  sx_atomic_uint32 num_items;
  sx_atomic_uint64 alloc_size;
  sx_atomic_uint64 peak_size;
  sx_pool *item_pool;       // item_size = sizeof(vd__mem_item)
  vd__mem_item *items_list; // first node
  sx_mutex mtx;
  vd__mem_item_collapsed *SX_ARRAY cached; // keep sorted cached data

  struct vd__mem_trace_context *parent;
  struct vd__mem_trace_context *child;
  struct vd__mem_trace_context *next;
  struct vd__mem_trace_context *prev;
} vd__mem_trace_context;

typedef struct
{
  char name[32];
  uint64_t start_tm;
  sx_mutex mtx;
  vd__mem_item **SX_ARRAY items;
} vd__mem_capture_context;

typedef struct
{
  sg_pass_action pass_action;
  sg_pass pass;
  sg_pipeline pip;
  sg_pipeline pip_2;
  sg_image depth_target;
  sg_image color_target;
  sg_sampler sampler;
  int32_t sample_count;
} vd__v3d_offscreen_pass;

typedef struct
{
  sg_pass_action pass_action;
  sg_pipeline pip;
} vd__v3d_display_pass;

typedef struct
{
  ozz_instance_t *ozz;
} vd__v3d_doll;

enum
{
  VD__VFS_FLAG_NONE = 0x01,
  VD__VFS_FLAG_ABSOLUTE_PATH = 0x02,
  VD__VFS_FLAG_TEXT_FILE = 0x04,
  VD__VFS_FLAG_APPEND = 0x08,
};
typedef uint32_t vd__vfs_flags;

typedef enum
{
  VD__VFS_CMD_READ,
  VD__VFS_CMD_WRITE,
} vd__vfs_async_command;

typedef enum
{
  VD__VFS_RESPONSE_READ_FAILED,
  VD__VFS_RESPONSE_READ_OK,
  VD__VFS_RESPONSE_WRITE_FAILED,
  VD__VFS_RESPONSE_WRITE_OK,
} vd__vfs_response_code;

typedef void(vd__vfs_async_read_cb)(const char *path, sx_mem_block *mem, void *user_data);
typedef void(vd__vfs_async_write_cb)(const char *path, int64_t bytes_written, sx_mem_block *mem, void *user_data);

typedef struct
{
  vd__vfs_async_command cmd;
  vd__vfs_flags flags;
  char path[VD__MAX_PATH];
  sx_mem_block *write_mem;
  const sx_alloc *alloc;
  union {
    vd__vfs_async_read_cb *read_fn;
    vd__vfs_async_write_cb *write_fn;
  };
  void *user_data;
} vd__vfs_async_request;

typedef struct
{
  vd__vfs_response_code code;
  union {
    sx_mem_block *read_mem;
    sx_mem_block *write_mem;
  };
  union {
    vd__vfs_async_read_cb *read_fn;
    vd__vfs_async_write_cb *write_fn;
  };
  void *user_data;
  int64_t bytes_written;
  char path[VD__MAX_PATH];
} vd__vfs_async_response;

typedef struct
{
  char path[VD__MAX_PATH];
  char alias[VD__MAX_PATH];
  int alias_len;
} vd__vfs_mount_point;

static struct
{
  struct
  {
    int width;
    int height;
    JanetFiber *main_fiber;
    JanetFunction *mod_init_cb;
    JanetFunction *mod_event_cb;
    JanetFunction *mod_update_cb;
    JanetFunction *mod_shutdown_cb;
  } app;

  struct
  {
    sx_alloc *alloc; // allocator passed on init
    char asset_db_file[VD__MAX_PATH];
    char variation[32];
    vd__asset_mgr *SX_ARRAY asset_mgrs;
    uint32_t *SX_ARRAY asset_name_hashes; // (count = count(asset_mgrs))
    vd__asset *assets;                    // loaded assets
    sx_handle_pool *asset_handles;
    sx_hashtbl *asset_tbl;         // key: hash(path+params), value: handle (asset_handles)
    sx_hashtbl *resource_tbl;      // key  hash(path), value: index-to resources
    vd__asset_resource *resources; // resource database
    sx_hash_xxh32_t *hasher;
    vd__asset_async_load_req *async_reqs;
    vd__asset_async_job *async_job_list;
    vd__asset_async_job *async_job_list_last;
    vd__asset_group *groups;
    sx_handle_pool *group_handles;
    vd__asset_group_handle cur_group;
    sx_lock_t assets_lk; // used for locking assets-array
  } asset;

  struct
  {
    const sx_alloc *heap_alloc;
    sx_alloc *alloc;

    sx_job_context *jobs;

    uint32_t flags;
    int tmp_mem_max;
    int num_threads;
    sx_alloc *temp_alloc_dummy;

    int64_t frame_idx;

    sx_mutex tmp_allocs_mtx;
    vd__tmp_alloc_tls **SX_ARRAY tmp_allocs;
  } core;

  struct
  {
    ecs_world_t *world;
  } ecs;

  struct
  {
#if SX_PLATFORM_WINDOWS
    sw_context *sw;
#endif

    const sx_alloc *alloc;
    sx_atomic_uint64 debug_mem_size;
    vd__mem_trace_context *root;
    vd__mem_capture_context capture;
    sx_atomic_uint32 in_capture;
  } mem;

  struct
  {
    sx_alloc *alloc;
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
    } dbg;

    struct
    {
      vd__v3d_offscreen_pass shadow;
      vd__v3d_display_pass display;
      vd_v3d_display_vs_uniforms_t display_uniforms;
    } fwd;

    ecs_query_t *static_mesh_query;
  } v3d;

  struct
  {
    sx_alloc *alloc;
    vd__vfs_mount_point *mounts;
    sx_thread *worker;
    sx_queue_spsc *req_queue;
    sx_queue_spsc *res_queue;
    sx_sem worker_sem;
    int quit;
  } vfs;
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

//    __  _________  _______  _____  __
//   /  |/  / __/  |/  / __ \/ _ \ \/ /
//  / /|_/ / _// /|_/ / /_/ / , _/\  /
// /_/  /_/___/_/  /_/\____/_/|_| /_/
//
// >>memory

#define vd__mem_trace_context_mutex_enter(opts, mtx)                                                                   \
  if (opts & VD__MEMOPTION_MULTITHREAD)                                                                                \
  sx_mutex_enter(&mtx)
#define vd__mem_trace_context_mutex_exit(opts, mtx)                                                                    \
  if (opts & VD__MEMOPTION_MULTITHREAD)                                                                                \
  sx_mutex_exit(&mtx)

enum
{
  VD__MEMOPTION_TRACE_CALLSTACK = 0x1, // Stores callstacks per allocation call
  VD__MEMOPTION_INSERT_CANARIES = 0x4, // inserts canaries for out of boundary detection
  VD__MEMOPTION_MULTITHREAD = 0x8,     // allocation calls can be called from multiple threads
  VD__MEMOPTION_ALL = 0xf              // all options above
};
typedef uint32_t vd___mem_options;

#define VD__MEMOPTION_INHERIT 0xffffffff

static vd__mem_trace_context *vd__mem_find_trace_context(uint32_t name_hash, vd__mem_trace_context *node)
{
  if (node->name_hash == name_hash)
  {
    return node;
  }

  vd__mem_trace_context *child = node->child;
  while (child)
  {
    vd__mem_trace_context *found = vd__mem_find_trace_context(name_hash, child);
    if (found)
    {
      return found;
    }
    child = child->next;
  }

  return NULL;
}

static vd__mem_trace_context *vd__mem_create_trace_context(const char *name, uint32_t mem_opts, const char *parent)
{
  sx_assert(name);
  uint32_t name_hash = sx_hash_fnv32_str(name);
  if (vd__state.mem.root)
  {
    if (vd__mem_find_trace_context(name_hash, vd__state.mem.root))
    {
      sx_assert_alwaysf(0, "duplicate name exists for vd__mem_trace_context: %s", name);
      return NULL;
    }
  }

  vd__mem_trace_context *parent_ctx = vd__state.mem.root;
  if (parent && parent[0])
  {
    parent_ctx = vd__mem_find_trace_context(sx_hash_fnv32_str(parent), vd__state.mem.root);
    if (!parent_ctx)
    {
      sx_assert_alwaysf(0, "parent does not exist for vd__mem_trace_context: %s, parent: %s", name, parent);
      return NULL;
    }
  }

  vd__mem_trace_context *ctx = sx_calloc(vd__state.mem.alloc, sizeof(vd__mem_trace_context));
  if (!ctx)
  {
    sx_memory_fail();
    return NULL;
  }

  sx_strcpy(ctx->name, sizeof(ctx->name), name);
  ctx->name_hash = name_hash;
  ctx->options = (mem_opts == VD__MEMOPTION_INHERIT && parent_ctx) ? parent_ctx->options : mem_opts;
  ctx->parent = parent_ctx;
  ctx->item_pool = sx_pool_create(vd__state.mem.alloc, sizeof(vd__mem_item), 150);
  if (!ctx->item_pool)
  {
    sx_free(vd__state.mem.alloc, ctx);
    sx_memory_fail();
    return NULL;
  }

  if (parent_ctx)
  {
    if (parent_ctx->child)
    {
      parent_ctx->child->prev = ctx;
      ctx->next = parent_ctx->child;
    }
    parent_ctx->child = ctx;
  }

  if (ctx->options & VD__MEMOPTION_MULTITHREAD)
  {
    sx_mutex_init(&ctx->mtx);
  }

  return ctx;
}

static vd__mem_item *mem_find_occupied_item(vd__mem_trace_context *ctx, void *ptr)
{
  vd__mem_item *it = ctx->items_list;
  while (it)
  {
    if (it->ptr == ptr && !it->freed)
    {
      return it;
    }
    it = it->next;
  }

  return NULL;
}

static void vd__mem_destroy_trace_item(vd__mem_trace_context *ctx, vd__mem_item *item)
{
  sx_assert(ctx);

  // unlink
  vd__mem_trace_context_mutex_enter(ctx->options, ctx->mtx);
  if (item->next)
  {
    item->next->prev = item->prev;
  }
  if (item->prev)
  {
    item->prev->next = item->next;
  }
  if (ctx->items_list == item)
  {
    ctx->items_list = item->next;
  }
  item->next = item->prev = NULL;
  item->freed = false;

  sx_pool_del(ctx->item_pool, item);

  sx_array_clear(ctx->cached);
  vd__mem_trace_context_mutex_exit(ctx->options, ctx->mtx);
}

static void vd__mem_create_trace_item(vd__mem_trace_context *ctx, void *ptr, void *old_ptr, size_t size,
                                      const char *file, const char *func, uint32_t line)
{
  vd__mem_item item = {0};
  if (old_ptr)
  {
    item.action = (size == 0) ? VD__MEM_ACTION_FREE : VD__MEM_ACTION_REALLOC;
  }
  bool save_current_call = item.action != VD__MEM_ACTION_FREE;

  if (save_current_call)
  {
    sx_atomic_fetch_add64_explicit(&vd__state.mem.debug_mem_size, sizeof(vd__mem_item), SX_ATOMIC_MEMORYORDER_RELAXED);

    if (ctx->options & VD__MEMOPTION_TRACE_CALLSTACK)
    {
#if SX_PLATFORM_WINDOWS
      item.num_callstack_items = sw_capture_current(vd__state.mem.sw, item.callstack, &item.callstack_hash);
      if (item.num_callstack_items == 0)
      {
        if (file)
          sx_strcpy(item.source_file, sizeof(item.source_file), file);
        if (func)
          sx_strcpy(item.source_func, sizeof(item.source_func), func);
        item.source_line = line;
        item.callstack_hash = sx_hash_xxh32(item.callstack, sizeof(item.callstack), 0);
      }
#elif SX_PLATFORM_OSX || SX_PLATFORM_LINUX
      item.num_callstack_items = backtrace(item.callstack, SW_MAX_FRAMES);
      if (item.num_callstack_items == 0)
      {
        if (file)
          sx_strcpy(item.source_file, sizeof(item.source_file), file);
        if (func)
          sx_strcpy(item.source_func, sizeof(item.source_func), file);
        item.source_line = line;
        item.callstack_hash = sx_hash_xxh32(item.callstack, item.num_callstack_items * sizeof(void *), 0);
      }
      else
      {
        item.callstack_hash = sx_hash_xxh32(item.callstack, sizeof(item.callstack), 0);
      }
#else
      if (file)
        sx_strcpy(item.source_file, sizeof(item.source_file), file);
      if (func)
        sx_strcpy(item.source_func, sizeof(item.source_func), func);
      item.source_line = line;
      item.callstack_hash = sx_hash_xxh32(item.callstack, sizeof(item.callstack), 0);
#endif
    }
  }

  item.owner = ctx;
  item.size = size;
  item.ptr = ptr;
  item.frame = vd__core_frame_index();
  bool in_capture = sx_atomic_load32_explicit(&vd__state.mem.in_capture, SX_ATOMIC_MEMORYORDER_ACQUIRE);

  // special case: FREE and REALLOC, always have previous malloc trace items that we should take care of
  if (item.action == VD__MEM_ACTION_FREE || item.action == VD__MEM_ACTION_REALLOC)
  {
    vd__mem_trace_context_mutex_enter(ctx->options, ctx->mtx);
    vd__mem_item *old_item = mem_find_occupied_item(ctx, old_ptr);
    vd__mem_trace_context_mutex_enter(ctx->options, ctx->mtx);

    // old_ptr should be either malloc or realloc and belong to this vd__mem_trace_context
    sx_assert(old_item && old_item->action == VD__MEM_ACTION_MALLOC || old_item->action == VD__MEM_ACTION_REALLOC);
    if (old_item)
    {
      sx_assert(old_item->size > 0);
      vd__mem_trace_context *_ctx = ctx;
      while (_ctx)
      {
        sx_atomic_fetch_sub32_explicit(&_ctx->num_items, 1, SX_ATOMIC_MEMORYORDER_RELAXED);
        sx_atomic_fetch_sub64_explicit(&_ctx->alloc_size, old_item->size, SX_ATOMIC_MEMORYORDER_RELAXED);
        _ctx = _ctx->parent;
      }

      if (item.action == VD__MEM_ACTION_FREE)
      {
        item.size = old_item->size;
      }

      if (!in_capture && item.action == VD__MEM_ACTION_FREE)
      {
        vd__mem_destroy_trace_item(ctx, old_item);
        old_item->freed = true;
      }
      else if (in_capture)
      {
        old_item->freed = true;
        sx_mutex_lock(vd__state.mem.capture.mtx)
        {
          sx_array_push(vd__state.mem.alloc, vd__state.mem.capture.items, old_item);
        }
      }
    }
  }

  // propogate memory usage to the context and it's parents
  if (size > 0)
  {
    vd__mem_trace_context *_ctx = ctx;
    while (_ctx)
    {
      sx_atomic_fetch_add32_explicit(&_ctx->num_items, 1, SX_ATOMIC_MEMORYORDER_RELAXED);
      sx_atomic_fetch_add64_explicit(&_ctx->alloc_size, (int64_t)size, SX_ATOMIC_MEMORYORDER_ACQUIRE);

      unsigned long long cur_peak = _ctx->peak_size;
      while (cur_peak < ctx->alloc_size &&
             !sx_atomic_compare_exchange64_strong(&_ctx->peak_size, &cur_peak, _ctx->alloc_size))
      {
        sx_relax_cpu();
      }
      _ctx = _ctx->parent;
    }
  }

  // create tracking item and add to linked-list
  if (save_current_call)
  {
    vd__mem_trace_context_mutex_enter(ctx->options, ctx->mtx);
    vd__mem_item *new_item = sx_align_ptr(sx_pool_new_and_grow(ctx->item_pool, vd__state.mem.alloc), 0, 8);
    if (!new_item)
    {
      vd__mem_trace_context_mutex_enter(ctx->options, ctx->mtx);
      sx_memory_fail();
      return;
    }
    sx_memcpy(new_item, &item, sizeof(item));

    if (ctx->items_list)
    {
      ctx->items_list->prev = new_item;
    }
    new_item->next = ctx->items_list;
    ctx->items_list = new_item;
    sx_array_clear(ctx->cached);
    vd__mem_trace_context_mutex_enter(ctx->options, ctx->mtx);

    if (in_capture)
    {
      sx_mutex_lock(vd__state.mem.capture.mtx)
      {
        sx_array_push(vd__state.mem.alloc, vd__state.mem.capture.items, new_item);
      }
    }
  }
}

static void vd__mem_trace_context_clear(vd__mem_trace_context *ctx)
{
  vd__mem_trace_context_mutex_enter(ctx->options, ctx->mtx);
  vd__mem_item *item = ctx->items_list;
  while (item)
  {
    vd__mem_item *next = item->next;
    sx_pool_del(ctx->item_pool, item);
    item = next;
  }

  ctx->items_list = NULL;
  ctx->num_items = 0;
  ctx->alloc_size = 0;
  vd__mem_trace_context_mutex_exit(ctx->options, ctx->mtx);
}

static void *vd__mem_alloc_cb(void *ptr, size_t size, uint32_t align, const char *file, const char *func, uint32_t line,
                              void *user_data)
{
  vd__mem_trace_context *ctx = user_data;
  void *p = ctx->redirect_alloc.alloc_cb(ptr, size, align, file, func, line, user_data);
  // if (!ctx->disabled)
  //  vd__mem_create_trace_item(ctx, p, ptr, size, file, func, line);
  return p;
}

sx_alloc *vd__mem_create_allocator(const char *name, uint32_t mem_opts, const char *parent, const sx_alloc *alloc)
{
  // sx_assert(alloc);
  vd__mem_trace_context *ctx = vd__mem_create_trace_context(name, mem_opts, parent);

  if (ctx)
  {
    ctx->my_alloc.alloc_cb = vd__mem_alloc_cb;
    ctx->my_alloc.user_data = ctx;

    if (alloc)
      ctx->redirect_alloc = *alloc;

    return &ctx->my_alloc;
  }

  return NULL;
}

void vd__mem_allocator_clear_trace(sx_alloc *alloc)
{
  vd__mem_trace_context *ctx = alloc->user_data;
  vd__mem_trace_context_clear(ctx);
}

static void vd__mem_destroy_trace_context(vd__mem_trace_context *ctx)
{
  if (ctx)
  {
    if (vd__state.mem.root && !vd__mem_find_trace_context(sx_hash_fnv32_str(ctx->name), vd__state.mem.root))
    {
      sx_assert_alwaysf(0, "vd__mem_trace_context already destroyed/invalid: %s", ctx->name);
      return;
    }

    // destroy children recursively
    if (ctx->child)
    {
      vd__mem_trace_context *child = ctx->child;
      while (child)
      {
        vd__mem_trace_context *next = child->next;
        vd__mem_destroy_trace_context(child);
        child = next;
      }
    }

    // unlink
    if (ctx->parent)
    {
      vd__mem_trace_context *parent = ctx->parent;
      if (parent->child == ctx)
      {
        parent->child = ctx->next;
      }
    }

    if (ctx->next)
    {
      ctx->next->prev = ctx->prev;
    }

    if (ctx->prev)
    {
      ctx->prev->next = ctx->next;
    }

    if (ctx->options & VD__MEMOPTION_MULTITHREAD)
    {
      sx_mutex_release(&ctx->mtx);
    }

    sx_array_free(vd__state.mem.alloc, ctx->cached);
    sx_pool_destroy(ctx->item_pool, vd__state.mem.alloc);
    sx_free(vd__state.mem.alloc, ctx);
  }
}

void vd__mem_destroy_allocator(sx_alloc *alloc)
{
  if (alloc)
  {
    vd__mem_trace_context *ctx = alloc->user_data;
    vd__mem_destroy_trace_context(ctx);
  }
}

bool vd__mem_init(uint32_t opts)
{
  static const char *k_hardcoded_vspaths[] = {
    "C:\\Program Files (x86)\\Microsoft Visual "
    "Studio\\2019\\Community\\Common7\\IDE\\Extensions\\TestPlatform\\Extensions\\Cpp\\x64"};

  sx_assert(opts != VD__MEMOPTION_INHERIT);

  vd__state.mem.alloc = vd__core_heap_alloc();

  // #if SX_PLATFORM_WINDOWS
  //   vd__state.mem.sw = sw_create_context_capture(SW_OPTIONS_SYMBOL | SW_OPTIONS_SOURCEPOS | SW_OPTIONS_MODULEINFO |
  //                                                  SW_OPTIONS_SYMBUILDPATH,
  //                                                (sw_callbacks){.load_module = mem_callstack_load_module}, NULL);
  //   if (!vd__state.mem.sw)
  //   {
  //     return false;
  //   }

  //   char vspath[SW_MAX_NAME_LEN] = {0};

  //   size_t num_hardcoded_paths = sizeof(k_hardcoded_vspaths) / sizeof(char *);
  //   for (size_t i = 0; i < num_hardcoded_paths; i++)
  //   {
  //     if (sx_os_path_isdir(k_hardcoded_vspaths[i]))
  //     {
  //       sx_strcpy(vspath, sizeof(vspath), k_hardcoded_vspaths[i]);
  //       break;
  //     }
  //   }

  //   // This is a long process, it uses all kinds of shenagians to find visual studio (damn you msvc!)
  //   // check previous hardcoded paths that we use to find dbghelp.dll, it is better to add to those instead
  //   if (vspath[0] == 0 && vd__win_get_vstudio_dir(vspath, sizeof(vspath)))
  //   {
  // #if SX_ARCH_64BIT
  //     sx_strcat(vspath, sizeof(vspath), "Common7\\IDE\\Extensions\\TestPlatform\\Extensions\\Cpp\\x64");
  // #elif SX_ARCH_32BIT
  //     sx_strcat(vspath, sizeof(vspath), "Common7\\IDE\\Extensions\\TestPlatform\\Extensions\\Cpp");
  // #endif
  //   }

  //   if (vspath[0])
  //     sw_set_dbghelp_hintpath(vspath);

  //   sw_set_callstack_limits(vd__state.mem.sw, 3, SW_MAX_FRAMES);
  // #endif // SX_PLATFORM_WINDOWS

  // dummy root trace context
  vd__state.mem.root = vd__mem_create_trace_context("<memory>", opts, NULL);
  if (!vd__state.mem.root)
  {
    sx_memory_fail();
    return false;
  }

  sx_mutex_init(&vd__state.mem.capture.mtx);

  // g_mem_imgui.collapse_items = true;

  return true;
}

static void vd__mem_shutdown(void)
{
  // #if SX_PLATFORM_WINDOWS
  //   sw_destroy_context(g_mem.sw);
  // #endif

  if (vd__state.mem.root)
  {
    vd__mem_destroy_trace_context(vd__state.mem.root);
  }

  sx_mutex_release(&vd__state.mem.capture.mtx);
}

void vd__mem_merge_peak(sx_alloc *alloc1, sx_alloc *alloc2)
{
  vd__mem_trace_context *ctx1 = alloc1->user_data;
  vd__mem_trace_context *ctx2 = alloc2->user_data;

  int64_t max_peak = sx_max(ctx1->peak_size, ctx2->peak_size);
  ctx1->peak_size = max_peak;
  ctx2->peak_size = max_peak;
}

void vd__mem_enable_trace(sx_alloc *alloc)
{
  vd__mem_trace_context *ctx = alloc->user_data;
  ctx->disabled = false;
}

void vd__mem_disable_trace(sx_alloc *alloc)
{
  vd__mem_trace_context *ctx = alloc->user_data;
  ctx->disabled = true;
}

void vd__mem_enable_trace_view(sx_alloc *alloc)
{
  vd__mem_trace_context *ctx = alloc->user_data;
  ctx->viewDisabled = false;
}

void vd__mem_disable_trace_view(sx_alloc *alloc)
{
  vd__mem_trace_context *ctx = alloc->user_data;
  ctx->viewDisabled = true;
}

void vd__mem_set_view_name(sx_alloc *alloc, const char *name)
{
  vd__mem_trace_context *ctx = alloc->user_data;
  sx_strcpy(ctx->name_view, sizeof(ctx->name_view), name);
}

//   _________  ___  ____
//  / ___/ __ \/ _ \/ __/
// / /__/ /_/ / , _/ _/
// \___/\____/_/|_/___/
//
// >>core

#define VD__MAX_TEMP_ALLOC_WAIT_TIME 5.0
#define DEFAULT_TMP_SIZE 0xA00000 // 10mb

static void vd__vfs_init();
static void vd__vfs_update();

static void vd__asset_update();

void vd__dbg_draw();
static void vd__v3d_render();

static _Thread_local vd__tmp_alloc_tls tl_tmp_alloc;

static void *vd__core_tmp_alloc_stub_cb(void *ptr, size_t size, uint32_t align, const char *file, const char *func,
                                        uint32_t line, void *user_data)
{
  sx_unused(user_data);

  vd__tmp_alloc_tls *tmpalloc = &tl_tmp_alloc;
  if (vd__state.core.flags & VD__CORE_FLAG_HEAP_TEMP_ALLOCATOR)
  {
    vd__tmp_alloc_heapmode *talloc = &tmpalloc->heap_alloc;
    vd__tmp_alloc_heapmode_inst *inst = &talloc->alloc_stack[sx_array_count(talloc->alloc_stack) - 1];
    return inst->alloc.alloc_cb(ptr, size, align, file, func, line, inst->alloc.user_data);
  }
  else
  {
    vd__tmp_alloc *talloc = &tmpalloc->alloc;
    vd__tmp_alloc_inst *inst = &talloc->alloc_stack[sx_array_count(talloc->alloc_stack) - 1];
    return inst->alloc.alloc_cb(ptr, size, align, file, func, line, inst->alloc.user_data);
  }
}

static bool vd__core_init_tmp_alloc_tls(vd__tmp_alloc_tls *tmpalloc)
{
  sx_assert(!tmpalloc->init);

  static sx_alloc stub_alloc = {.alloc_cb = vd__core_tmp_alloc_stub_cb, .user_data = NULL};

  size_t page_sz = sx_os_pagesz();
  size_t tmp_size =
    sx_align_mask(vd__state.core.tmp_mem_max > 0 ? vd__state.core.tmp_mem_max * 1024 : DEFAULT_TMP_SIZE, page_sz - 1);
  uint32_t tid = sx_thread_tid();

  if (!(vd__state.core.flags & VD__CORE_FLAG_HEAP_TEMP_ALLOCATOR))
  {
    int num_tmp_pages = sx_vmem_get_needed_pages(tmp_size);

    if (!sx_vmem_init(&tmpalloc->alloc.vmem, 0, num_tmp_pages))
    {
      sx_out_of_memory();
      return false;
    }
    sx_vmem_commit_pages(&tmpalloc->alloc.vmem, 0, num_tmp_pages);
  }
  else
  {
    tmpalloc->heap_alloc.max_size = tmp_size;
  }

  if (vd__state.core.flags & VD__CORE_FLAG_TRACE_TEMP_ALLOCATOR)
  {
    char alloc_name[32];
    sx_snprintf(alloc_name, sizeof(alloc_name), "TempA (tid:#%u)", tid);
    tmpalloc->tracer_front = vd__mem_create_allocator(alloc_name, VD__MEMOPTION_TRACE_CALLSTACK, "Temp", &stub_alloc);
    sx_snprintf(alloc_name, sizeof(alloc_name), "TempB (tid:#%u)", tid);
    tmpalloc->tracer_back = vd__mem_create_allocator(alloc_name, VD__MEMOPTION_TRACE_CALLSTACK, "Temp", &stub_alloc);

    sx_snprintf(alloc_name, sizeof(alloc_name), "Temp (tid:#%u)", tid);
    vd__mem_set_view_name(tmpalloc->tracer_front, alloc_name);
    vd__mem_set_view_name(tmpalloc->tracer_back, alloc_name);

    sx_assert(tmpalloc->tracer_front && tmpalloc->tracer_back);
  }

  vd__log_info("(init) temp allocator created in thread: %u, memory: %d kb", tid, tmp_size / 1024);
  tmpalloc->init = true;
  tmpalloc->tid = tid;
  tmpalloc->idle_tm = 0;
  return true;
}

static void vd__core_release_tmp_alloc_tls(vd__tmp_alloc_tls *tmpalloc)
{
  const sx_alloc *alloc = vd__state.core.alloc;
  if (vd__state.core.flags & VD__CORE_FLAG_HEAP_TEMP_ALLOCATOR)
  {
    sx_array_free(alloc, tmpalloc->heap_alloc.items);
    sx_array_free(alloc, tmpalloc->heap_alloc.alloc_stack);
    sx_assertf(tmpalloc->heap_alloc.stack_depth == 0, "invalid push/pop order on thread: %u temp allocator",
               tmpalloc->tid);
  }
  else
  {
    sx_vmem_release(&tmpalloc->alloc.vmem);
    sx_array_free(alloc, tmpalloc->alloc.alloc_stack);
    sx_assertf(tmpalloc->alloc.stack_depth == 0, "invalid push/pop order on thread: %u temp allocator", tmpalloc->tid);
  }

  if (vd__state.core.flags & VD__CORE_FLAG_TRACE_TEMP_ALLOCATOR)
  {
    vd__mem_destroy_allocator(tmpalloc->tracer_front);
    vd__mem_destroy_allocator(tmpalloc->tracer_back);
  }

  tmpalloc->init = false;
}

static sx_job_t vd__core_job_dispatch(int count, void (*callback)(int start, int end, int thrd_index, void *user),
                                      void *user, sx_job_priority priority, uint32_t tags)
{
  sx_assert(vd__state.core.jobs);
  return sx_job_dispatch(vd__state.core.jobs, count, callback, user, priority, tags);
}

static void vd__core_job_wait_and_del(sx_job_t job)
{
  sx_assert(vd__state.core.jobs);
  sx_job_wait_and_del(vd__state.core.jobs, job);
}

static bool vd__core_job_test_and_del(sx_job_t job)
{
  sx_assert(vd__state.core.jobs);
  return sx_job_test_and_del(vd__state.core.jobs, job);
}

static int vd__core_job_num_threads(void)
{
  return vd__state.core.num_threads;
}

static int vd__core_job_thread_index(void)
{
  sx_assert(vd__state.core.jobs);
  return sx_job_thread_index(vd__state.core.jobs);
}

static int vd__core_thread_func(void *user1, void *user2)
{
  typedef int (*thread_fn)(void *user_data);
  thread_fn func = (thread_fn)user2;
  sx_assert(func);

  int r = func(user1);

  // find the any existing thread temp allocator and destroy it
  uint32_t tid = sx_thread_tid();
  sx_mutex_lock(vd__state.core.tmp_allocs_mtx)
  {
    for (int i = 0, c = sx_array_count(vd__state.core.tmp_allocs); i < c; i++)
    {
      if (vd__state.core.tmp_allocs[i]->tid == tid)
      {
        vd__core_release_tmp_alloc_tls(vd__state.core.tmp_allocs[i]);
        sx_swap(vd__state.core.tmp_allocs[i], vd__state.core.tmp_allocs[c - 1], vd__tmp_alloc_tls *);
        sx_array_pop_last(vd__state.core.tmp_allocs);
        break;
      }
    }
  }

  return r;
}

static sx_thread *vd__core_thread_create(int (*thread_fn)(void *user_data), void *user_data, const char *debug_name)
{
  sx_assert(thread_fn);
  return sx_thread_create(vd__state.core.alloc, vd__core_thread_func, user_data, 1024 * 1024, debug_name,
                          (void *)thread_fn);
}

static int vd__core_thread_destroy(sx_thread *thrd)
{
  return sx_thread_destroy((sx_thread *)thrd, vd__state.core.alloc);
}

static const sx_alloc *vd__core_heap_alloc(void)
{
  return vd__state.core.heap_alloc;
}

int64_t vd__core_frame_index(void)
{
  return vd__state.core.frame_idx;
}

static void vd__core_update(void)
{
  // reset temp allocators
  sx_mutex_enter(&vd__state.core.tmp_allocs_mtx);
  for (int i = 0, c = sx_array_count(vd__state.core.tmp_allocs); i < c; i++)
  {
    vd__tmp_alloc_tls *tmpalloc = vd__state.core.tmp_allocs[i];
    sx_assert(tmpalloc->init);

    // tmpalloc->idle_tm += dt;
    if (tmpalloc->idle_tm > VD__MAX_TEMP_ALLOC_WAIT_TIME)
    {
      vd__log_debug("destroying thread temp allocator (tid=%u) because it seems to be idle for so long", tmpalloc->tid);

      vd__core_release_tmp_alloc_tls(tmpalloc);
      sx_swap(vd__state.core.tmp_allocs[i], vd__state.core.tmp_allocs[c - 1], vd__tmp_alloc_tls *);
      sx_array_pop_last(vd__state.core.tmp_allocs);
      --c;
      continue;
    }

    if (vd__state.core.flags & VD__CORE_FLAG_HEAP_TEMP_ALLOCATOR)
    {
      vd__tmp_alloc_heapmode *t = &tmpalloc->heap_alloc;
      if (t->stack_depth > 0)
      {
        // t->wait_time += dt;
        if (t->wait_time > VD__MAX_TEMP_ALLOC_WAIT_TIME)
        {
          // vd__core_print_error(0, t->alloc_stack[t->stack_depth - 1].file, t->alloc_stack[t->stack_depth - 1].line,
          // "tmp_alloc_push doesn't seem to have the pop call (Thread: %d)", i);
          sx_assertf(0, "not all tmp_allocs are popped.");
        }
      }
      else
      {
        sx_assertf(sx_array_count(t->items) == 0, "not all tmp_alloc items are freed");
        sx_array_clear(t->items);
        sx_array_clear(t->alloc_stack);
        t->offset = 0;
        t->stack_depth = 0;
        t->frame_peak = 0;
        t->wait_time = 0;
      }
    }
    else
    {
      vd__tmp_alloc *t = &tmpalloc->alloc;

      if (t->stack_depth > 0)
      {
        // t->wait_time += dt;
        if (t->wait_time > VD__MAX_TEMP_ALLOC_WAIT_TIME)
        {
          // vd__core_print_error(0, t->alloc_stack[t->stack_depth - 1].file, t->alloc_stack[t->stack_depth - 1].line,
          // "tmp_alloc_push doesn't seem to have the pop call (Thread: %d)", i);
          sx_assertf(0, "not all tmp_allocs are popped.");
        }
      }
      else
      {
        sx_array_clear(t->alloc_stack);
        t->stack_depth = 0;
        t->frame_peak = 0;
        t->wait_time = 0;
      }
    }

    if (vd__state.core.flags & VD__CORE_FLAG_TRACE_TEMP_ALLOCATOR)
    {
      sx_assert(tmpalloc->tracer_front && tmpalloc->tracer_back);
      sx_swap(tmpalloc->tracer_front, tmpalloc->tracer_back, sx_alloc *);
      vd__mem_allocator_clear_trace(tmpalloc->tracer_front);
      vd__mem_merge_peak(tmpalloc->tracer_front, tmpalloc->tracer_back);
      vd__mem_disable_trace(tmpalloc->tracer_back);
      vd__mem_enable_trace_view(tmpalloc->tracer_back);
      vd__mem_enable_trace(tmpalloc->tracer_front);
      vd__mem_disable_trace_view(tmpalloc->tracer_front);
    }
  }
  sx_mutex_exit(&vd__state.core.tmp_allocs_mtx);

  vd__vfs_update();
  vd__asset_update();
  vd__v3d_render();
  // vd__dbg_draw();
}

static void vd__core_job_thread_init_cb(sx_job_context *ctx, int thread_index, uint32_t thread_id, void *user)
{
  // sx_unused(thread_id);
  // sx_unused(user);
  // sx_unused(ctx);

  // char name[32];
  // sx_snprintf(name, sizeof(name), "Thread #%d", thread_index + 1 /* 0 is for main-thread */);
  // rmt_SetCurrentThreadName(name);
  sx_unused(ctx);
  sx_unused(thread_index);
  sx_unused(thread_id);
  sx_unused(user);
}

static void vd__core_job_thread_shutdown_cb(sx_job_context *ctx, int thread_index, uint32_t thread_id, void *user)
{
  sx_unused(ctx);
  sx_unused(thread_index);
  sx_unused(thread_id);
  sx_unused(user);
}

static void vd__core_init(const vd__config *conf)
{
  vd__state.core.heap_alloc =
    (conf->core_flags & VD__CORE_FLAG_DETECT_LEAKS) ? sx_alloc_malloc_leak_detect() : sx_alloc_malloc();

  // vd__mem_init(VD__MEMOPTION_TRACE_CALLSTACK | VD__MEMOPTION_MULTITHREAD);
  vd__mem_init(VD__MEMOPTION_TRACE_CALLSTACK | VD__MEMOPTION_MULTITHREAD);

  vd__state.core.alloc = vd__mem_create_allocator("core", VD__MEMOPTION_INHERIT, NULL, vd__state.core.heap_alloc);
  sx_assert_alwaysf(vd__state.core.alloc, "Fatal error: could not create core allocator");
  sx_mutex_init(&vd__state.core.tmp_allocs_mtx);

  // resolve number of worker threads if not defined explicitly
  // NOTE: we always have at least one extra worker thread not matter what input is
  //       default number of "worker threads" are total number of CPU cores minus one, because main thread is obviously
  //       running on a thread
  int num_worker_threads = 2; // conf->job_num_threads >= 0 ? conf->job_num_threads : (sx_os_numcores() - 1);
  num_worker_threads = sx_max(1, num_worker_threads);
  vd__state.core.num_threads = num_worker_threads + 1;

  // Temp allocator information
  sx_mutex_init(&vd__state.core.tmp_allocs_mtx);
  vd__state.core.tmp_mem_max = conf->tmp_mem_max;
  if (vd__state.core.flags & VD__CORE_FLAG_HEAP_TEMP_ALLOCATOR)
  {
    // vd__log_info("(init) using debug temp allocators");
    printf("(init) using debug temp allocators\n");
  }
  else if (vd__state.core.flags & VD__CORE_FLAG_TRACE_TEMP_ALLOCATOR)
  {
    // vd__log_info("(init) using memory tracing in temp allocators");
    printf("(init) using memory tracing in temp allocators\n");
  }

  if (vd__state.core.flags & VD__CORE_FLAG_TRACE_TEMP_ALLOCATOR)
  {
    vd__state.core.temp_alloc_dummy = vd__mem_create_allocator("temp", VD__MEMOPTION_INHERIT, NULL, NULL);
  }

  vd__vfs_init();

  vd__state.core.jobs = sx_job_create_context(
    vd__state.core.alloc, &(sx_job_context_desc){.num_threads = num_worker_threads,
                                                 .max_fibers = 64,
                                                 .fiber_stack_sz = 5120 * 1024,
                                                 .thread_init_cb = vd__core_job_thread_init_cb,
                                                 .thread_shutdown_cb = vd__core_job_thread_shutdown_cb});
  // if (!vd__state.core.jobs)
  // {
  // vd__profile_startup_end();
  // vd__log_error("initializing job dispatcher failed");
  // return false;
  //   return;
  // }
  // vd__log_info("(init) jobs: threads=%d, max_fibers=%d, stack_size=%dkb",
  //  sx_job_num_worker_threads(vd__state.core.jobs), conf->job_max_fibers,
  //  conf->job_stack_size);
}

static void vd__core_shutdown(void)
{
  if (!vd__state.core.heap_alloc)
    return;
  const sx_alloc *alloc = vd__state.core.alloc;

  if (vd__state.core.jobs)
  {
    sx_job_destroy_context(vd__state.core.jobs, alloc);
    vd__state.core.jobs = NULL;
  }

  sx_mutex_lock(vd__state.core.tmp_allocs_mtx)
  {
    for (int i = 0; i < sx_array_count(vd__state.core.tmp_allocs); i++)
    {
      vd__core_release_tmp_alloc_tls(vd__state.core.tmp_allocs[i]);
    }
    sx_array_free(vd__state.core.alloc, vd__state.core.tmp_allocs);
    vd__state.core.tmp_allocs = NULL;
    if (vd__state.core.temp_alloc_dummy)
      vd__mem_destroy_allocator(vd__state.core.temp_alloc_dummy);
  }

  sx_mutex_release(&vd__state.core.tmp_allocs_mtx);

  vd__mem_destroy_allocator(vd__state.core.alloc);
  vd__mem_shutdown();

  sx_memset(&vd__state.core, 0x0, sizeof(vd__state.core));
}

//    _____________
//   / __/ ___/ __/
//  / _// /___\ \
// /___/\___/___/
//
// >>ecs

static void vd__ecs_init()
{
  ecs_world_t *world = ecs_init();
  vd__state.ecs.world = world;
}

static void vd__ecs_shutdown()
{
  ecs_fini(vd__state.ecs.world);
}

//   _   __________
//  | | / / __/ __/
//  | |/ / _/_\ \
//  |___/_/ /___/
//
// >>vfs

static void vd__vfs_read_async(const char *path, vd__vfs_flags flags, const sx_alloc *alloc,
                               vd__vfs_async_read_cb *read_fn, void *user_data)
{
  vd__vfs_async_request req = {.cmd = VD__VFS_CMD_READ, //
                               .flags = flags,
                               .alloc = alloc,
                               .read_fn = read_fn,
                               .user_data = user_data};
  sx_strcpy(req.path, sizeof(req.path), path);
  sx_queue_spsc_produce_and_grow(vd__state.vfs.req_queue, &req, vd__state.vfs.alloc);
  sx_semaphore_post(&vd__state.vfs.worker_sem, 1);
}

static void vd__vfs_write_async(const char *path, sx_mem_block *mem, vd__vfs_flags flags,
                                vd__vfs_async_write_cb *write_fn, void *user_data)
{
  vd__vfs_async_request req = {
    .cmd = VD__VFS_CMD_WRITE, .flags = flags, .write_mem = mem, .write_fn = write_fn, .user_data = user_data};
  sx_strcpy(req.path, sizeof(req.path), path);
  sx_queue_spsc_produce_and_grow(vd__state.vfs.req_queue, &req, vd__state.vfs.alloc);
  sx_semaphore_post(&vd__state.vfs.worker_sem, 1);
}

bool vd__vfs_mount(const char *path, const char *alias, bool watch)
{
  sx_unused(watch);

  if (sx_os_path_isdir(path))
  {
    vd__vfs_mount_point mp = {0};
    sx_os_path_normpath(mp.path, sizeof(mp.path), path);
    sx_os_path_unixpath(mp.alias, sizeof(mp.alias), alias);
    mp.alias_len = sx_strlen(mp.alias);

    // #if VD__CONFIG_HOT_LOADING
    //     if (watch)
    //         mp.watch_id = dmon_watch(mp.path, dmon__event_cb, DMON_WATCHFLAGS_RECURSIVE, NULL).id;
    // #endif

    // check that the mount path is not already registered
    for (int i = 0, c = sx_array_count(vd__state.vfs.mounts); i < c; i++)
    {
      if (sx_strequal(vd__state.vfs.mounts[i].path, mp.path))
      {
        // vd__log_error("(vfs) path '%s' is already mounted on '%s'", mp.path, mp.alias);
        return false;
      }
    }

    sx_array_push(vd__state.vfs.alloc, vd__state.vfs.mounts, mp);
    printf("(vfs) mounted '%s' on '%s'\n", mp.alias, mp.path);
    // vd__log_info("(vfs) mounted '%s' on '%s'", mp.alias, mp.path);
    return true;
  }
  else
  {
    // vd__log_error("(vfs) mount path is not valid: %s", path);
    return false;
  }
}

static bool vd__vfs_resolve_path(char *out_path, int out_path_sz, const char *path, vd__vfs_flags flags)
{
  if (flags & VD__VFS_FLAG_ABSOLUTE_PATH)
  {
    sx_os_path_normpath(out_path, out_path_sz, path);
    return true;
  }
  else
  {
    // search mount points and see if we find the path
    for (int i = 0, c = sx_array_count(vd__state.vfs.mounts); i < c; i++)
    {
      const vd__vfs_mount_point *mp = &vd__state.vfs.mounts[i];
      if (sx_strnequal(path, mp->alias, mp->alias_len))
      {
        char tmp_path[VD__MAX_PATH];
        sx_os_path_normpath(tmp_path, sizeof(tmp_path), path + mp->alias_len);
        sx_os_path_join(out_path, out_path_sz, mp->path, tmp_path);
        return true;
      }
    }

    // check absolute path
    sx_os_path_normpath(out_path, out_path_sz, path);
    return sx_os_stat(out_path).type != SX_FILE_TYPE_INVALID;
  }
}

static sx_mem_block *vd__vfs_read(const char *path, vd__vfs_flags flags, const sx_alloc *alloc)
{
  if (!alloc)
    alloc = vd__state.vfs.alloc;

  char resolved_path[VD__MAX_PATH];
  vd__vfs_resolve_path(resolved_path, sizeof(resolved_path), path, flags);

  return (flags & VD__VFS_FLAG_TEXT_FILE) ? sx_file_load_text(alloc, resolved_path)
                                          : sx_file_load_bin(alloc, resolved_path);
}

static int64_t vd__vfs_write(const char *path, const sx_mem_block *mem, vd__vfs_flags flags)
{
  char resolved_path[VD__MAX_PATH];
  sx_file f;

  vd__vfs_resolve_path(resolved_path, sizeof(resolved_path), path, flags);
  bool r = sx_file_open(&f, resolved_path,
                        SX_FILE_WRITE | SX_FILE_NOCACHE | ((flags & VD__VFS_FLAG_APPEND) ? SX_FILE_APPEND : 0));
  if (r)
  {
    int64_t written = sx_file_write(&f, mem->data, mem->size);
    sx_file_close(&f);
    return written;
  }
  else
  {
    return -1;
  }
}

static void vd__vfs_update(void)
{
  vd__vfs_async_response res;
  while (sx_queue_spsc_consume(vd__state.vfs.res_queue, &res))
  {
    switch (res.code)
    {
    case VD__VFS_RESPONSE_READ_OK:
    case VD__VFS_RESPONSE_READ_FAILED:
      res.read_fn(res.path, res.read_mem, res.user_data);
      break;

    case VD__VFS_RESPONSE_WRITE_OK:
    case VD__VFS_RESPONSE_WRITE_FAILED:
      res.write_fn(res.path, res.bytes_written, res.write_mem, res.user_data);
      break;
    }
  }
}

static int vd__vfs_worker(void *user)
{
  sx_unused(user);

  while (!vd__state.vfs.quit)
  {
    vd__vfs_async_request req;
    if (sx_queue_spsc_consume(vd__state.vfs.req_queue, &req))
    {
      vd__vfs_async_response res = {.bytes_written = -1};
      sx_strcpy(res.path, sizeof(res.path), req.path);
      res.user_data = req.user_data;

      switch (req.cmd)
      {
      case VD__VFS_CMD_READ: {
        res.read_fn = req.read_fn;
        sx_mem_block *mem = vd__vfs_read(req.path, req.flags, req.alloc);

        if (mem)
        {
          res.code = VD__VFS_RESPONSE_READ_OK;
          res.read_mem = mem;
        }
        else
        {
          res.code = VD__VFS_RESPONSE_READ_FAILED;
        }
        sx_queue_spsc_produce_and_grow(vd__state.vfs.res_queue, &res, vd__state.vfs.alloc);
        break;
      }

      case VD__VFS_CMD_WRITE: {
        res.write_fn = req.write_fn;
        int64_t written = vd__vfs_write(req.path, req.write_mem, req.flags);

        if (written > 0)
        {
          res.code = VD__VFS_RESPONSE_WRITE_OK;
          res.bytes_written = written;
          res.write_mem = req.write_mem;
        }
        else
        {
          res.code = VD__VFS_RESPONSE_WRITE_FAILED;
        }
        sx_queue_spsc_produce_and_grow(vd__state.vfs.res_queue, &res, vd__state.vfs.alloc);
        break;
      }
      }
    } // if (queue_consume)

    // wait on more jobs
    sx_semaphore_wait(&vd__state.vfs.worker_sem, -1);
  }

  return 0;
}

static void vd__vfs_init()
{
  vd__state.vfs.alloc = vd__mem_create_allocator("vfs", VD__MEMOPTION_INHERIT, "core", vd__core_heap_alloc());

  vd__state.vfs.req_queue = sx_queue_spsc_create(vd__state.vfs.alloc, sizeof(vd__vfs_async_request), 128);
  vd__state.vfs.res_queue = sx_queue_spsc_create(vd__state.vfs.alloc, sizeof(vd__vfs_async_response), 128);
  if (!vd__state.vfs.req_queue || !vd__state.vfs.res_queue)
    return;

  sx_semaphore_init(&vd__state.vfs.worker_sem);
  vd__state.vfs.worker = vd__core_thread_create(vd__vfs_worker, NULL, "vfs_worker");
}

static void vd__vfs_shutdown()
{
  if (!vd__state.vfs.alloc)
    return;

  if (vd__state.vfs.worker)
  {
    vd__state.vfs.quit = 1;
    sx_semaphore_post(&vd__state.vfs.worker_sem, 1);
    vd__core_thread_destroy(vd__state.vfs.worker);
    sx_semaphore_release(&vd__state.vfs.worker_sem);
  }

  if (vd__state.vfs.req_queue)
    sx_queue_spsc_destroy(vd__state.vfs.req_queue, vd__state.vfs.alloc);
  if (vd__state.vfs.res_queue)
    sx_queue_spsc_destroy(vd__state.vfs.res_queue, vd__state.vfs.alloc);

  // sx_array_free(vd__state.vfs.alloc, vd__state.vfs.modify_cbs);
  sx_array_free(vd__state.vfs.alloc, vd__state.vfs.mounts);

  vd__mem_destroy_allocator(vd__state.vfs.alloc);
  vd__state.vfs.alloc = NULL;
}

static uint64_t vd__vfs_last_modified(const char *path)
{
  char resolved_path[VD__MAX_PATH];
  if (vd__vfs_resolve_path(resolved_path, sizeof(resolved_path), path, 0))
    return sx_os_stat(resolved_path).last_modified;
  else
    return 0;
}

static const sx_alloc *vd__vfs_alloc(void)
{
  return vd__state.vfs.alloc;
}

//    ___   __________________
//   / _ | / __/ __/ __/_  __/
//  / __ |_\ \_\ \/ _/  / /
// /_/ |_/___/___/___/ /_/
//
// >>asset

static inline void vd__asset_job_add_list(vd__asset_async_job **pfirst, vd__asset_async_job **plast,
                                          vd__asset_async_job *node)
{
  // Add to the end of the list
  if (*plast)
  {
    (*plast)->next = node;
    node->prev = *plast;
  }
  *plast = node;
  if (*pfirst == NULL)
    *pfirst = node;
}

static inline void vd__asset_job_remove_list(vd__asset_async_job **pfirst, vd__asset_async_job **plast,
                                             vd__asset_async_job *node)
{
  if (node->prev)
    node->prev->next = node->next;
  if (node->next)
    node->next->prev = node->prev;
  if (*pfirst == node)
    *pfirst = node->next;
  if (*plast == node)
    *plast = node->prev;
  node->prev = node->next = NULL;
}

static void vd__asset_load_job_cb(int start, int end, int thrd_index, void *user)
{
  sx_unused(start);
  sx_unused(end);
  sx_unused(thrd_index);
  vd__asset_async_job *ajob = user;

  ajob->state = ajob->amgr->callbacks.on_load(&ajob->load_data, &ajob->lparams, ajob->mem)
                  ? VD__ASSET_JOB_STATE_SUCCESS
                  : VD__ASSET_JOB_STATE_LOAD_FAILED;
}

static bool vd__asset_checkandfix_asset_type(sx_mem_block *mem, const char *filepath, char *outpath, size_t outpath_sz,
                                             uint32_t *num_meta)
{
  if (mem->size < 4)
  {
    return false;
  }

  sx_mem_reader r;
  sx_mem_init_reader(&r, mem->data, mem->size);

  int64_t bytes;
  uint32_t flag;
  bytes = sx_mem_read_var(&r, flag);
  if (flag != VD__ASSET_FLAG)
  {
    return false;
  }

  char ext[5] = {0};
  bytes = sx_mem_read(&r, ext, 4);
  sx_assert_alwaysf(bytes == 4, "invalid _voodoo_ header for asset: %s", filepath);

  // fix path: remove the extension and append real extension to the end of the path string
  char path_ext[32];
  sx_os_path_splitext(path_ext, sizeof(path_ext), outpath, (int)outpath_sz, filepath);
  if (ext[0] != '.')
  {
    sx_strcat(outpath, (int)outpath_sz, ".");
  }
  sx_strcat(outpath, (int)outpath_sz, ext);

  bytes = sx_mem_read(&r, num_meta, sizeof(uint32_t));
  sx_assert_alwaysf(bytes == sizeof(uint32_t), "invalid _voodoo_ header for asset: %s", filepath);

  // increment the offset on the memory pointer
  sx_mem_addoffset(mem, r.pos);
  return true;
}

static inline int vd__asset_find_async_req(const char *path)
{
  uint32_t path_hash = sx_hash_fnv32_str(path);
  for (int i = 0, c = sx_array_count(vd__state.asset.async_reqs); i < c; i++)
  {
    vd__asset_async_load_req *req = &vd__state.asset.async_reqs[i];
    if (req->path_hash == path_hash)
      return i;
  }

  return -1;
}

// async callback
static void vd__asset_on_read(const char *path, sx_mem_block *mem, void *user)
{
  sx_unused(user);

  int async_req_idx = vd__asset_find_async_req(path);

  if (!mem)
  {
    // error opening the file
    if (async_req_idx != -1)
    {
      vd__asset_async_load_req *req = &vd__state.asset.async_reqs[async_req_idx];
      vd__asset_handle asset = req->asset;
      vd__asset *a = &vd__state.asset.assets[sx_handle_index(asset.id)];
      sx_assert(a->resource_id);
      vd__asset_resource *res = &vd__state.asset.resources[vd__to_index(a->resource_id)];
      vd__asset_mgr *amgr = &vd__state.asset.asset_mgrs[a->asset_mgr_id];

      // vd__asset_errmsg(res->path, res->real_path, "opening");
      printf("failed opening asset: %s\n", res->path);
      a->state = VD__ASSET_STATE_FAILED;
      a->obj = amgr->failed_obj;

      sx_array_pop(vd__state.asset.async_reqs, async_req_idx);
    }
    return;
  }
  else if (async_req_idx == -1)
  {
    sx_mem_destroy_block(mem);
    return;
  }

  vd__asset_async_load_req *req = &vd__state.asset.async_reqs[async_req_idx];
  vd__asset_handle asset = req->asset;
  vd__asset *a = &vd__state.asset.assets[sx_handle_index(asset.id)];
  sx_assert(a->resource_id);
  vd__asset_resource *res = &vd__state.asset.resources[vd__to_index(a->resource_id)];
  vd__asset_mgr *amgr = &vd__state.asset.asset_mgrs[a->asset_mgr_id];

  const void *params_ptr = NULL;
  if (a->params_id)
  {
    params_ptr = &amgr->params_buff[vd__to_index(a->params_id)];
  }

  vd__asset_load_params aparams = (vd__asset_load_params){
    .path = res->path, .params = params_ptr, .alloc = a->alloc, .tags = a->tags, .flags = a->load_flags};

  char fixed_path[VD__MAX_PATH];
  bool path_is_fixed = vd__asset_checkandfix_asset_type(mem, path, fixed_path, sizeof(fixed_path), &aparams.num_meta);
  vd__asset_meta_keyval *metas = NULL;
  if (path_is_fixed)
  {
    aparams.path = fixed_path;
    if (aparams.num_meta > 0)
    {
      sx_assert(aparams.num_meta < 64);
      metas = alloca(sizeof(vd__asset_meta_keyval) * aparams.num_meta);
      sx_assert_always(metas);
      sx_memcpy(metas, mem->data, sizeof(vd__asset_meta_keyval) * aparams.num_meta);
      sx_mem_addoffset(mem, sizeof(vd__asset_meta_keyval) * aparams.num_meta);
      aparams.metas = metas;
    }
  }

  vd__asset_load_data load_data = amgr->callbacks.on_prepare(&aparams, mem);

  sx_array_pop(vd__state.asset.async_reqs, async_req_idx);
  if (!load_data.obj.id)
  {
    // vd__asset_errmsg(res->path, res->real_path, "preparing");
    sx_mem_destroy_block(mem);
    return;
  }

  // dispatch job request for on_load
  // allocate the whole buffer and save a copy of path and params
  uint8_t *buff = sx_malloc(vd__state.asset.alloc, sizeof(vd__asset_async_job) + amgr->params_size + VD__MAX_PATH +
                                                     sizeof(vd__asset_meta_keyval) * aparams.num_meta);
  if (!buff)
  {
    sx_memory_fail();
    return;
  }

  vd__asset_async_job *ajob = (vd__asset_async_job *)buff;
  buff += sizeof(vd__asset_async_job);
  aparams.path = (const char *)buff;
  sx_memcpy(buff, path_is_fixed ? fixed_path : res->path, VD__MAX_PATH);
  buff += VD__MAX_PATH;
  if (aparams.num_meta)
  {
    sx_assert((uintptr_t)buff % 8 == 0);
    aparams.metas = (vd__asset_meta_keyval *)buff;
    sx_memcpy(buff, metas, sizeof(vd__asset_meta_keyval) * aparams.num_meta);
    buff += sizeof(vd__asset_meta_keyval) * aparams.num_meta;
  }

  if (params_ptr)
  {
    sx_assert((uintptr_t)buff % 8 == 0);
    aparams.params = buff;
    sx_memcpy(buff, params_ptr, amgr->params_size);
  }

  *ajob = (vd__asset_async_job){.load_data = load_data, .mem = mem, .amgr = amgr, .lparams = aparams, .asset = asset};

  ajob->job = vd__core_job_dispatch(1, vd__asset_load_job_cb, ajob, SX_JOB_PRIORITY_HIGH, 0);
  vd__asset_job_add_list(&vd__state.asset.async_job_list, &vd__state.asset.async_job_list_last, ajob);
}

static inline uint32_t vd__asset_hash(const char *path, const void *params, int params_size, const sx_alloc *alloc)
{
  sx_hash_xxh32_init(vd__state.asset.hasher, 0);
  sx_hash_xxh32_update(vd__state.asset.hasher, path, sx_strlen(path));
  if (params_size)
    sx_hash_xxh32_update(vd__state.asset.hasher, params, params_size);
  sx_hash_xxh32_update(vd__state.asset.hasher, &alloc, sizeof(alloc));
  return sx_hash_xxh32_digest(vd__state.asset.hasher);
}

static inline int vd__asset_find_asset_mgr(uint32_t name_hash)
{
  for (int i = 0, c = sx_array_count(vd__state.asset.asset_name_hashes); i < c; i++)
  {
    if (vd__state.asset.asset_name_hashes[i] == name_hash)
      return i;
  }
  return -1;
}

static vd__asset_handle vd__asset_create_new(const char *path, const void *params, vd__asset_obj obj,
                                             uint32_t name_hash, const sx_alloc *obj_alloc, vd__asset_load_flags flags,
                                             uint32_t tags)
{
  // find asset manager
  int amgr_id = vd__asset_find_asset_mgr(name_hash);
  sx_assertf(amgr_id != -1, "asset type is not registered");
  vd__asset_mgr *amgr = &vd__state.asset.asset_mgrs[amgr_id];

  // check resources, if doesn't exist, add new resource
  uint32_t path_hash = sx_hash_fnv32_str(path);
  int res_idx = sx_hashtbl_find_get(vd__state.asset.resource_tbl, path_hash, -1);
  if (res_idx == -1)
  {
    vd__asset_resource res = {.used = true};
    sx_strcpy(res.path, sizeof(res.path), path);
    sx_strcpy(res.real_path, sizeof(res.real_path), path);
    res.path_hash = path_hash;
    res.asset_mgr_id = amgr_id;
#if !SX_PLATFORM_ANDROID && !SX_PLATFORM_IOS
    res.last_modified = vd__vfs_last_modified(res.real_path);
#endif
    res_idx = sx_array_count(vd__state.asset.resources);
    sx_array_push(vd__state.asset.alloc, vd__state.asset.resources, res);
    sx_hashtbl_add_and_grow(vd__state.asset.resource_tbl, path_hash, res_idx, vd__state.asset.alloc);
  }
  else
  {
    vd__state.asset.resources[res_idx].used = true;
  }

  // new param
  int params_size = amgr->params_size;
  uint32_t params_id = 0;
  if (params_size > 0)
  {
    params_id = vd__to_id(sx_array_count(amgr->params_buff)); // actually, it stores the byte-offset
    sx_memcpy(sx_array_add(vd__state.asset.alloc, amgr->params_buff, params_size), params, params_size);
  }

  sx_handle_t handle = sx_handle_new_and_grow(vd__state.asset.asset_handles, vd__state.asset.alloc);
  sx_assert(handle);

  // add asset to database
  vd__asset asset = (vd__asset){.alloc = obj_alloc,
                                .handle = handle,
                                .params_id = params_id,
                                .resource_id = vd__to_id(res_idx),
                                .asset_mgr_id = amgr_id,
                                .ref_count = 1,
                                .obj = obj,
                                .hash = vd__asset_hash(path, params, params_size, obj_alloc),
                                .tags = tags,
                                .load_flags = flags,
                                .state = VD__ASSET_STATE_ZOMBIE};

  // have to protected this block of code with a lock
  // because we may regrow the asset-array
  // asset-array may be accessed in worker-threads with `obj()` function
  sx_lock(vd__state.asset.assets_lk)
  {
    sx_array_push_byindex(vd__state.asset.alloc, vd__state.asset.assets, asset, sx_handle_index(handle));
  }

  sx_hashtbl_add_and_grow(vd__state.asset.asset_tbl, asset.hash, handle, vd__state.asset.alloc);

  return (vd__asset_handle){handle};
}

static vd__asset_handle vd__asset_add(const char *path, const void *params, vd__asset_obj obj, uint32_t name_hash,
                                      const sx_alloc *obj_alloc, vd__asset_load_flags flags, uint32_t tags,
                                      vd__asset_handle override_asset)
{
  vd__asset_handle asset = override_asset;
  if (asset.id)
  {
    // this block of code actually happens on RELOAD process
    sx_assert(flags & VD__ASSET_LOAD_FLAG_RELOAD);
    vd__asset *a = &vd__state.asset.assets[sx_handle_index(asset.id)];
    vd__asset_mgr *amgr = &vd__state.asset.asset_mgrs[a->asset_mgr_id];

    // keep the current object so it can be released later
    sx_assert(a->handle == asset.id);
    if (a->state == VD__ASSET_STATE_OK)
      a->dead_obj = a->obj;
    a->obj = obj;
    sx_assertf(a->alloc == obj_alloc, "allocator must not change in reload");
    if (amgr->params_size > 0)
    {
      sx_assert(a->params_id);
      sx_memcpy(&amgr->params_buff[vd__to_index(a->params_id)], params, amgr->params_size);
    }
  }
  else
  {
    asset = vd__asset_create_new(path, params, obj, name_hash, obj_alloc, flags, tags);
  }

  return asset;
}

static vd__asset_handle vd__asset_load_hashed(uint32_t name_hash, const char *path, const void *params,
                                              vd__asset_load_flags flags, const sx_alloc *obj_alloc, uint32_t tags)
{
  if (!path[0])
  {
    // vd__log_warn("empty path for asset");
    printf("empty path for asset\n");
    return (vd__asset_handle){0};
  }

  // sx_assertf(vd__core_job_thread_index() == 0, "must call this function in the main thread");

  if (flags & VD__ASSET_LOAD_FLAG_RELOAD)
    flags |= VD__ASSET_LOAD_FLAG_WAIT_ON_LOAD;

  int amgr_id = vd__asset_find_asset_mgr(name_hash);
  sx_assertf(amgr_id != -1, "asset type is not registered");
  vd__asset_mgr *amgr = &vd__state.asset.asset_mgrs[amgr_id];
  flags |= amgr->forced_flags;

  // check if the params are provided and valid
  if (amgr->params_size && !params)
  {
    // vd__log_warn("you must provide `params` of type '%s' for this asset", amgr->params_type_name);
    sx_assertf(0, "params must not be null for this asset type");
  }

  // find if asset is already loaded
  vd__asset_handle asset = (vd__asset_handle){
    sx_hashtbl_find_get(vd__state.asset.asset_tbl, vd__asset_hash(path, params, amgr->params_size, obj_alloc), 0)};
  if (asset.id && !(flags & VD__ASSET_LOAD_FLAG_RELOAD))
  {
    ++vd__state.asset.assets[sx_handle_index(asset.id)].ref_count;
  }
  else
  {
    // find resource and resolve the real file path
    int res_idx = sx_hashtbl_find_get(vd__state.asset.resource_tbl, sx_hash_fnv32_str(path), -1);
    const char *real_path = path;
    vd__asset_resource *res = NULL;
    if (res_idx != -1)
    {
      res = &vd__state.asset.resources[res_idx];
      real_path = res->real_path;
    }

    vd__vfs_flags vfs_flags = 0;
    if (flags & VD__ASSET_LOAD_FLAG_ABSOLUTE_PATH)
      vfs_flags |= VD__VFS_FLAG_ABSOLUTE_PATH;
    if (flags & VD__ASSET_LOAD_FLAG_TEXT_FILE)
      vfs_flags |= VD__VFS_FLAG_TEXT_FILE;
    if (!(flags & VD__ASSET_LOAD_FLAG_WAIT_ON_LOAD))
    {
      // Async load
      asset = vd__asset_create_new(path, params, amgr->async_obj, name_hash, obj_alloc, flags, tags);
      vd__asset *a = &vd__state.asset.assets[sx_handle_index(asset.id)];
      a->state = VD__ASSET_STATE_LOADING;

      vd__asset_async_load_req req =
        (vd__asset_async_load_req){.path_hash = sx_hash_fnv32_str(real_path), .asset = asset};
      sx_array_push(vd__state.asset.alloc, vd__state.asset.async_reqs, req);

      vd__vfs_read_async(real_path, vfs_flags, vd__core_heap_alloc(), vd__asset_on_read, NULL);
    }
    else
    {
      // Blocking load (+ reloads)
      asset = vd__asset_add(path, params, amgr->failed_obj, name_hash, obj_alloc, flags, tags,
                            (flags & VD__ASSET_LOAD_FLAG_RELOAD) ? asset : (vd__asset_handle){0});

      sx_mem_block *mem = vd__vfs_read(real_path, vfs_flags, vd__vfs_alloc());

      if (!mem)
      {
        // vd__asset_errmsg(path, real_path, "opening");
        return asset;
      }

      vd__asset *a = &vd__state.asset.assets[sx_handle_index(asset.id)];

      if (!res)
      {
        sx_assert(a->resource_id);
        res = &vd__state.asset.resources[vd__to_index(a->resource_id)];
      }

      vd__asset_load_params aparams =
        (vd__asset_load_params){.path = path, .params = params, .alloc = obj_alloc, .tags = tags, .flags = flags};

      bool success = false;
      char fixed_path[VD__MAX_PATH];
      vd__asset_meta_keyval *metas;
      if ((vd__asset_checkandfix_asset_type(mem, path, fixed_path, sizeof(fixed_path), &aparams.num_meta)))
      {
        aparams.path = fixed_path;
        if (aparams.num_meta > 0)
        {
          sx_assert(aparams.num_meta < 64);
          metas = alloca(sizeof(vd__asset_meta_keyval) * aparams.num_meta);
          sx_assert_always(metas);
          sx_memcpy(metas, mem->data, sizeof(vd__asset_meta_keyval) * aparams.num_meta);
          sx_mem_addoffset(mem, sizeof(vd__asset_meta_keyval) * aparams.num_meta);
          aparams.metas = metas;
        }
      }

      vd__asset_load_data load_data = amgr->callbacks.on_prepare(&aparams, mem);

      // revive pointer to asset, because during `on_prepare` some resource dependencies
      // may be loaded and `assets` array may be resized
      a = &vd__state.asset.assets[sx_handle_index(asset.id)];
      if (load_data.obj.id)
      {
        if (amgr->callbacks.on_load(&load_data, &aparams, mem))
        {
          amgr->callbacks.on_finalize(&load_data, &aparams, mem);
          success = true;
        }
      }

      sx_mem_destroy_block(mem);
      if (success)
      {
        a->state = VD__ASSET_STATE_OK;
        a->obj = load_data.obj;
      }
      else
      {
        if (load_data.obj.id)
          amgr->callbacks.on_release(load_data.obj, a->alloc);
        // vd__asset_errmsg(path, real_path, "loading");
        if (a->obj.id && !a->dead_obj.id)
        {
          a->state = VD__ASSET_STATE_FAILED;
        }
        else
        {
          a->obj = a->dead_obj; // rollback
          a->dead_obj = (vd__asset_obj){.id = 0};
        }
      }

      // do we have extra work in reload?
      if (flags & VD__ASSET_LOAD_FLAG_RELOAD)
      {
        amgr->callbacks.on_reload(asset, a->dead_obj, obj_alloc);
        if (a->dead_obj.id)
        {
          amgr->callbacks.on_release(a->dead_obj, obj_alloc);
          a->dead_obj = (vd__asset_obj){.id = 0};
        }
      }
    }
  }

  return asset;
}

static vd__asset_handle vd__asset_load(const char *name, const char *path, const void *params,
                                       vd__asset_load_flags flags, const sx_alloc *alloc, uint32_t tags)
{
  vd__asset_handle asset = vd__asset_load_hashed(sx_hash_fnv32_str(name), path, params, flags, alloc, tags);
  if (asset.id && vd__state.asset.cur_group.id)
  {
    vd__asset_group *g = &vd__state.asset.groups[sx_handle_index(vd__state.asset.cur_group.id)];
    sx_array_push(vd__state.asset.alloc, g->assets, asset);
  }
  return asset;
}

static void vd__asset_register_asset_type(const char *name, vd__asset_callbacks callbacks, const char *params_type_name,
                                          int params_size, vd__asset_obj failed_obj, vd__asset_obj async_obj,
                                          vd__asset_load_flags forced_flags)
{
  uint32_t name_hash = sx_hash_fnv32_str(name);
  for (int i = 0; i < sx_array_count(vd__state.asset.asset_name_hashes); i++)
  {
    if (name_hash == vd__state.asset.asset_name_hashes[i])
    {
      sx_assertf(0, "asset manager already registered");
      return;
    }
  }

  vd__asset_mgr amgr = {.callbacks = callbacks,
                        .name_hash = name_hash,
                        .params_size = params_size,
                        .failed_obj = failed_obj,
                        .async_obj = async_obj,
                        .forced_flags = forced_flags};
  sx_strcpy(amgr.name, sizeof(amgr.name), name);
  sx_strcpy(amgr.params_type_name, sizeof(amgr.params_type_name), params_type_name ? params_type_name : "");

  sx_array_push(vd__state.asset.alloc, vd__state.asset.asset_mgrs, amgr);
  sx_array_push(vd__state.asset.alloc, vd__state.asset.asset_name_hashes, name_hash);
}

static void vd__asset_update()
{
  vd__asset_async_job *ajob = vd__state.asset.async_job_list;
  while (ajob)
  {
    vd__asset_async_job *next = ajob->next;
    if (vd__core_job_test_and_del(ajob->job))
    {
      vd__asset *a = &vd__state.asset.assets[sx_handle_index(ajob->asset.id)];
      sx_assert(a->resource_id);
      vd__asset_resource *res = &vd__state.asset.resources[vd__to_index(a->resource_id)];

      switch (ajob->state)
      {
      case VD__ASSET_JOB_STATE_SUCCESS:
        ajob->amgr->callbacks.on_finalize(&ajob->load_data, &ajob->lparams, ajob->mem);
        a->obj = ajob->load_data.obj;
        a->state = VD__ASSET_STATE_OK;
        break;

      case VD__ASSET_JOB_STATE_LOAD_FAILED:
        // vd__asset_errmsg(res->path, res->real_path, "loading");
        a->obj = ajob->amgr->failed_obj;
        a->state = VD__ASSET_STATE_FAILED;

        if (ajob->load_data.obj.id)
          ajob->amgr->callbacks.on_release(ajob->load_data.obj, ajob->lparams.alloc);

        break;

      default:
        sx_assertf(0, "finished job should not be any other state");
        break;
      }

      sx_assert(!(ajob->lparams.flags & VD__ASSET_LOAD_FLAG_RELOAD));

      sx_mem_destroy_block(ajob->mem);

      vd__asset_job_remove_list(&vd__state.asset.async_job_list, &vd__state.asset.async_job_list_last, ajob);
      sx_free(vd__state.asset.alloc, ajob);
    } // if (job-is-done)

    ajob = next;
  }
}

static void vd__asset_init()
{
  vd__state.asset.alloc = vd__mem_create_allocator("asset", VD__MEMOPTION_INHERIT, "core", vd__core_heap_alloc());

  // the__vfs.register_modify(vd__asset_on_modified);

  vd__state.asset.asset_tbl = sx_hashtbl_create(vd__state.asset.alloc, VD__CONFIG_ASSET_POOL_SIZE);
  vd__state.asset.resource_tbl = sx_hashtbl_create(vd__state.asset.alloc, VD__CONFIG_ASSET_POOL_SIZE);
  sx_assert(vd__state.asset.asset_tbl);

  vd__state.asset.asset_handles = sx_handle_create_pool(vd__state.asset.alloc, VD__CONFIG_ASSET_POOL_SIZE);
  sx_assert(vd__state.asset.asset_handles);

  vd__state.asset.group_handles = sx_handle_create_pool(vd__state.asset.alloc, 32);
  sx_assert(vd__state.asset.group_handles);

  vd__state.asset.hasher = sx_hash_create_xxh32(vd__state.asset.alloc);
  sx_assert(vd__state.asset.hasher);
}

void vd__asset_shutdown()
{
  if (!vd__state.asset.alloc)
    return;

  const sx_alloc *alloc = vd__state.asset.alloc;

  if (vd__state.asset.asset_handles)
  {
    for (int i = 0; i < vd__state.asset.asset_handles->count; i++)
    {
      sx_handle_t handle = sx_handle_at(vd__state.asset.asset_handles, i);
      vd__asset *a = &vd__state.asset.assets[sx_handle_index(handle)];
      if (a->state == VD__ASSET_STATE_OK)
      {
        sx_assert(a->resource_id);
        // vd__log_warn("un-released asset: %s (ref_count = %d)",
        //  vd__state.asset.resources[vd__to_index(a->resource_id)].path, a->ref_count);
        if (a->obj.id)
        {
          vd__asset_mgr *amgr = &vd__state.asset.asset_mgrs[a->asset_mgr_id];
          if (!amgr->unreg)
            amgr->callbacks.on_release(a->obj, a->alloc);
        }
      }
    }
  }

  for (int i = 0; i < sx_array_count(vd__state.asset.asset_mgrs); i++)
  {
    vd__asset_mgr *amgr = &vd__state.asset.asset_mgrs[i];
    sx_array_free(alloc, amgr->params_buff);
  }

  for (int i = 0; i < sx_array_count(vd__state.asset.groups); i++)
  {
    sx_array_free(alloc, vd__state.asset.groups[i].assets);
  }

  sx_array_free(alloc, vd__state.asset.asset_mgrs);
  sx_array_free(alloc, vd__state.asset.assets);
  sx_array_free(alloc, vd__state.asset.asset_name_hashes);
  sx_array_free(alloc, vd__state.asset.resources);
  sx_array_free(alloc, vd__state.asset.groups);
  sx_array_free(alloc, vd__state.asset.async_reqs);

  if (vd__state.asset.asset_handles)
    sx_handle_destroy_pool(vd__state.asset.asset_handles, alloc);
  if (vd__state.asset.asset_tbl)
    sx_hashtbl_destroy(vd__state.asset.asset_tbl, alloc);
  if (vd__state.asset.resource_tbl)
    sx_hashtbl_destroy(vd__state.asset.resource_tbl, alloc);
  if (vd__state.asset.group_handles)
    sx_handle_destroy_pool(vd__state.asset.group_handles, alloc);

  if (vd__state.asset.hasher)
    sx_hash_destroy_xxh32(vd__state.asset.hasher, vd__state.asset.alloc);

  vd__asset_async_job *ajob = vd__state.asset.async_job_list;
  while (ajob)
  {
    vd__asset_async_job *next = ajob->next;
    if (ajob->mem)
      sx_mem_destroy_block(ajob->mem);
    sx_free(vd__state.asset.alloc, ajob);
    ajob = next;
  }
  vd__state.asset.async_job_list = vd__state.asset.async_job_list_last = NULL;

  vd__mem_destroy_allocator(vd__state.asset.alloc);
  vd__state.asset.alloc = NULL;
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
    .environment = sglue_environment(), 
    .logger.func = slog_func,
  });

  sgl_setup(&(sgl_desc_t){
    .logger.func = slog_func,
  });

  ozz_setup(&(ozz_desc_t){.max_palette_joints = 64, .max_instances = 1});

  sg_image_desc img_desc = (sg_image_desc){
    .render_target = true,
    .width = sapp_width(),
    .height = sapp_height(),
    .pixel_format = SG_PIXELFORMAT_RGBA32F,
  };

  sg_image_desc depth_desc = img_desc;
  depth_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
}

static void vd__gfx_shutdown()
{
  sgl_shutdown();
  sg_shutdown();
}

//    ____________  _______  ______
//   / __/ ___/ _ \/  _/ _ \/_  __/
//  _\ \/ /__/ , _// // ___/ / /
// /___/\___/_/|_/___/_/    /_/
//
// >>script

static void vd__app_init();
static void vd__app_shutdown();

typedef struct
{
  bool error;
  const char *script_src;
  const char *error_msg;
} vd__compilation_result;

EMSCRIPTEN_KEEPALIVE int vd__script_evaluate(const char *src)
{
  vd__app_shutdown();
  vd__app_init();
  return 1;
}

static void vd__janet_cdefs(JanetTable *env)
{
}

static void vd__script_init(const char *module_name)
{
  janet_init();
  JanetTable *core_env = janet_core_env(NULL);
  JanetTable *lookup = janet_env_lookup(core_env);

  janet_cfuns(core_env, NULL, vd__cfuns);

  char module_path[VD__MAX_PATH];
  if (module_name)
  {
    sx_snprintf(module_path, sizeof(module_path), "/voodoo/%s.janet", module_name);
  }

  char module_wrapper[512];
  sx_os_path_exists(module_path) ? sx_snprintf(module_wrapper, sizeof(module_wrapper),
                                               "(setdyn :syspath \"./voodoo\")\n"
                                               "(import %s :prefix \"\")\n"
                                               "(voodoo)",
                                               module_name)
                                 : sx_snprintf(module_wrapper, sizeof(module_wrapper),
                                               "(setdyn :syspath \"./assets/scripts\")\n"
                                               "(import game :prefix \"\")\n"
                                               "(voodoo)");

  Janet ret;
  JanetSignal status = janet_dostring(core_env, module_wrapper, "main", &ret);

  if (status == JANET_SIGNAL_ERROR)
  {
    fprintf(stderr, "failed executing module\n");
    janet_deinit();
    sargs_shutdown();
    exit(1);
  }

  JanetTable *cfg = janet_unwrap_table(ret);
  vd__state.app.width = janet_unwrap_integer(janet_table_rawget(cfg, janet_ckeywordv("width")));
  vd__state.app.height = janet_unwrap_integer(janet_table_rawget(cfg, janet_ckeywordv("height")));
  vd__state.app.mod_init_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("init")));
  vd__state.app.mod_event_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("event")));
  vd__state.app.mod_update_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("update")));
  vd__state.app.mod_shutdown_cb = janet_unwrap_function(janet_table_rawget(cfg, janet_ckeywordv("shutdown")));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_init_cb));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_event_cb));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_update_cb));
  janet_gcroot(janet_wrap_function(vd__state.app.mod_shutdown_cb));
}

static void vd__script_shutdown(void)
{
  janet_deinit();
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
  assert(desc);
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
  vd__camera_desc cd = (vd__camera_desc){
    .min_dist = janet_unwrap_number(janet_table_get(desc, janet_ckeywordv("min-dist"))),
    .max_dist = janet_unwrap_number(janet_table_get(desc, janet_ckeywordv("max-dist"))),
    .center = vd__unwrap_vec3(janet_table_get(desc, janet_ckeywordv("center"))),
    .distance = janet_unwrap_number(janet_table_get(desc, janet_ckeywordv("distance"))),
    .latitude = janet_unwrap_number(janet_table_get(desc, janet_ckeywordv("latitude"))),
    .longitude = janet_unwrap_number(janet_table_get(desc, janet_ckeywordv("longitude"))),
    .nearz = janet_unwrap_number(janet_table_get(desc, janet_ckeywordv("nearz"))),
    .farz = janet_unwrap_number(janet_table_get(desc, janet_ckeywordv("farz"))),
  };
  vd__cam_init(cam, &cd);
  // return janet_wrap_abstract(cam);
  return janet_wrap_abstract(cam);
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
  vd__state.v3d.dbg.transform.buf = vd__gfx_buffer_append(vd__state.v3d.dbg.transform.buf, sizeof(HMM_Mat4), &transform,
                                                          &vd__state.v3d.dbg.transform.offset);
}

void vd__dbg_draw_camera(vd__camera *cam)
{
  vd__state.v3d.dbg.vp = cam->view_proj;
}

void vd__dbg_draw()
{
  /* NOTE: the vs_params_t struct has been code-generated by the shader-code-gen */
  vs_params_t vs_params;
  vs_params.vp = vd__state.v3d.dbg.vp;

  sgl_load_matrix(&vd__state.v3d.dbg.vp.Elements[0][0]);

/*  sg_begin_pass(&(sg_pass){
    .action = {
      .colors[0] = {
        .load_action = SG_LOADACTION_CLEAR,
        .clear_value = { 0.13f, 0.13f, 0.13f, 1.0f } },
      },
      .swapchain = sglue_swapchain()
    });*/
  sg_apply_pipeline(vd__state.v3d.dbg.pip);

  sg_apply_bindings(&(sg_bindings){.vertex_buffers[0] = vd__state.v3d.dbg.vtx_buf,
                                   .vertex_buffers[1] = vd__state.v3d.dbg.transform.buf,
                                   .vertex_buffer_offsets[1] = vd__state.v3d.dbg.transform.offset,
                                   .index_buffer = vd__state.v3d.dbg.idx_buf});

  sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
  sg_draw(vd__state.v3d.dbg.shapes[VD__BOX].draw.base_element, vd__state.v3d.dbg.shapes[VD__BOX].draw.num_elements, 1);
  sgl_draw();
  // sg_end_pass();
  // sg_commit();
}

void vd__dbg_draw_init()
{
  /* create shader */
  vd__state.v3d.dbg.shd = sg_make_shader(shapes_shader_desc(sg_query_backend()));

  /* create pipeline object */
  vd__state.v3d.dbg.pip = sg_make_pipeline(&(sg_pipeline_desc){
    .shader = vd__state.v3d.dbg.shd,
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
  vd__state.v3d.dbg.shapes[VD__BOX].draw = sshape_element_range(&buf);
  buf = sshape_build_plane(&buf, &(sshape_plane_t){
                                   .width = 1.0f,
                                   .depth = 1.0f,
                                   .tiles = 10,
                                   .random_colors = true,
                                 });
  vd__state.v3d.dbg.shapes[VD__PLANE].draw = sshape_element_range(&buf);
  buf = sshape_build_sphere(&buf, &(sshape_sphere_t){
                                    .radius = 0.75f,
                                    .slices = 36,
                                    .stacks = 20,
                                    .random_colors = true,
                                  });
  vd__state.v3d.dbg.shapes[VD__SPHERE].draw = sshape_element_range(&buf);
  buf = sshape_build_cylinder(&buf, &(sshape_cylinder_t){
                                      .radius = 0.5f,
                                      .height = 1.5f,
                                      .slices = 36,
                                      .stacks = 10,
                                      .random_colors = true,
                                    });
  vd__state.v3d.dbg.shapes[VD__CYLINDER].draw = sshape_element_range(&buf);
  buf = sshape_build_torus(&buf, &(sshape_torus_t){
                                   .radius = 0.5f,
                                   .ring_radius = 0.3f,
                                   .rings = 36,
                                   .sides = 18,
                                   .random_colors = true,
                                 });
  vd__state.v3d.dbg.shapes[VD__TORUS].draw = sshape_element_range(&buf);
  assert(buf.valid);

  // one vertex/index-buffer-pair for all shapes
  const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
  const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
  vd__state.v3d.dbg.vtx_buf = sg_make_buffer(&vbuf_desc);
  vd__state.v3d.dbg.idx_buf = sg_make_buffer(&ibuf_desc);
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

//   _   ______ ___
//  | | / /_  // _ \
//  | |/ //_ </ // /
//  |___/____/____/
//
// >>v3d

typedef struct
{
  sg_buffer vbuf;
  sg_buffer ibuf;
  uint32_t num_indices;
} vd__static_mesh;

ECS_COMPONENT_DECLARE(vd__static_mesh);

typedef struct vd__animation_load_params vd__animation_load_params;

typedef struct
{
} vd__skeleton_load_params;

typedef struct vd__mesh_load_params vd__mesh_load_params;

typedef struct
{

} vd__v3d_doll_load_params;

static vd__asset_load_data vd__v3d_skeleton_on_prepare(const vd__asset_load_params *params, const sx_mem_block *mem)
{
  vd__skeleton_load_params *lparams = (vd__skeleton_load_params *)params->params;
  // ozz_instance_t *ozz = (ozz_instance_t *)lparams->ozz;
  // ozz_load_skeleton(ozz, mem->data, mem->size);
  printf("done preparing skeleton\n");
  return (vd__asset_load_data){.obj.id = 1};
}

static bool vd__v3d_skeleton_on_load(vd__asset_load_data *data, const vd__asset_load_params *params,
                                     const sx_mem_block *mem)
{
  printf("inside skeleton on load!!!\n");
  return true;
}

static void vd__v3d_skeleton_on_finalize(vd__asset_load_data *data, const vd__asset_load_params *params,
                                         const sx_mem_block *mem)
{
  printf("inside skeleton on finalize!\n");
}

static void vd__v3d_skeleton_on_reload(vd__asset_handle handle, vd__asset_obj prev_obj, const sx_alloc *alloc)
{
}

static void vd__v3d_skeleton_on_release(vd__asset_obj obj, const sx_alloc *alloc)
{
}

static vd__asset_load_data vd__v3d_doll_on_prepare(const vd__asset_load_params *params, const sx_mem_block *mem)
{
  const sx_alloc *alloc = params->alloc ? params->alloc : vd__state.v3d.alloc;

  cj5_token tokens[256];
  const int max_tokens = sizeof(tokens) / sizeof(cj5_token);
  cj5_result jres = cj5_parse((const char *)mem->data, mem->size - 1, tokens, max_tokens);
  if (jres.error)
  {
    if (jres.error == CJ5_ERROR_OVERFLOW)
    {
      cj5_token *ntokens = (cj5_token *)sx_malloc(alloc, sizeof(cj5_token) * jres.num_tokens);
      if (!ntokens)
      {
        printf("failed parsing doll file: oom\n");
        sx_out_of_memory();
        return (vd__asset_load_data){{0}};
      }
      jres = cj5_parse((const char *)mem->data, mem->size - 1, ntokens, jres.num_tokens);
      if (jres.error)
      {
        // vd__log_error("loading shader reflection failed: invalid json");
        printf("failed parsing doll file: invalid json\n");
        return (vd__asset_load_data){{0}};
      }
    }

    // vd__log_error("loading shader reflection failed: invalid json, line: %d", jres.error_line);
    printf("failed parsing doll file: unknown\n");
  }

  char skeleton_filepath[VD__MAX_PATH];
  char animation_filepath[VD__MAX_PATH];
  char mesh_filepath[VD__MAX_PATH];

  char dirname[VD__MAX_PATH];
  char tmpstr[VD__MAX_PATH];
  sx_os_path_dirname(dirname, sizeof(dirname), params->path);
  sx_os_path_join(skeleton_filepath, sizeof(skeleton_filepath), dirname,
                  cj5_seekget_string(&jres, 0, "skeleton", tmpstr, sizeof(tmpstr), ""));
  sx_os_path_unixpath(skeleton_filepath, sizeof(skeleton_filepath), skeleton_filepath);
  sx_os_path_join(animation_filepath, sizeof(animation_filepath), dirname,
                  cj5_seekget_string(&jres, 0, "animation", tmpstr, sizeof(tmpstr), ""));
  sx_os_path_unixpath(animation_filepath, sizeof(animation_filepath), animation_filepath);
  sx_os_path_join(mesh_filepath, sizeof(mesh_filepath), dirname,
                  cj5_seekget_string(&jres, 0, "mesh", tmpstr, sizeof(tmpstr), ""));
  sx_os_path_unixpath(mesh_filepath, sizeof(mesh_filepath), mesh_filepath);

  vd__v3d_doll *doll = sx_malloc(alloc, sizeof(vd__v3d_doll));
  doll->ozz = ozz_create_instance(0);

  sx_mem_block *skeleton_mem = sx_file_load_bin(alloc, skeleton_filepath);
  ozz_load_skeleton(doll->ozz, skeleton_mem->data, skeleton_mem->size);

  sx_mem_block *animation_mem = sx_file_load_bin(alloc, animation_filepath);
  ozz_load_animation(doll->ozz, animation_mem->data, animation_mem->size);

  sx_mem_block *mesh_mem = sx_file_load_bin(alloc, mesh_filepath);
  ozz_load_skeleton(doll->ozz, mesh_mem->data, mesh_mem->size);

  printf("done preparing doll!\n");
  return (vd__asset_load_data){.obj.ptr = doll};
}

static bool vd__v3d_doll_on_load(vd__asset_load_data *data, const vd__asset_load_params *params,
                                 const sx_mem_block *mem)
{
  printf("inside doll on load!\n");
  return true;
}

static void vd__v3d_doll_on_finalize(vd__asset_load_data *data, const vd__asset_load_params *params,
                                     const sx_mem_block *mem)
{
  printf("inside doll on finalize!\n");
}

static void vd__v3d_doll_on_reload(vd__asset_handle handle, vd__asset_obj prev_obj, const sx_alloc *alloc)
{
}

static void vd__v3d_doll_on_release(vd__asset_obj obj, const sx_alloc *alloc)
{
  printf("releasing doll!\n");
  sx_free(alloc, obj.ptr);
}

static void vd__v3d_cube()
{
  static const float cube_vertices[] = {-1.0, -1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 1.0,  -1.0, -1.0, 1.0, 0.0, 0.0, 1.0,
                                  1.0,  1.0,  -1.0, 1.0, 0.0, 0.0, 1.0, -1.0, 1.0,  -1.0, 1.0, 0.0, 0.0, 1.0,

                                  -1.0, -1.0, 1.0,  0.0, 1.0, 0.0, 1.0, 1.0,  -1.0, 1.0,  0.0, 1.0, 0.0, 1.0,
                                  1.0,  1.0,  1.0,  0.0, 1.0, 0.0, 1.0, -1.0, 1.0,  1.0,  0.0, 1.0, 0.0, 1.0,

                                  -1.0, -1.0, -1.0, 0.0, 0.0, 1.0, 1.0, -1.0, 1.0,  -1.0, 0.0, 0.0, 1.0, 1.0,
                                  -1.0, 1.0,  1.0,  0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0,  0.0, 0.0, 1.0, 1.0,

                                  1.0,  -1.0, -1.0, 1.0, 0.5, 0.0, 1.0, 1.0,  1.0,  -1.0, 1.0, 0.5, 0.0, 1.0,
                                  1.0,  1.0,  1.0,  1.0, 0.5, 0.0, 1.0, 1.0,  -1.0, 1.0,  1.0, 0.5, 0.0, 1.0,

                                  -1.0, -1.0, -1.0, 0.0, 0.5, 1.0, 1.0, -1.0, -1.0, 1.0,  0.0, 0.5, 1.0, 1.0,
                                  1.0,  -1.0, 1.0,  0.0, 0.5, 1.0, 1.0, 1.0,  -1.0, -1.0, 0.0, 0.5, 1.0, 1.0,

                                  -1.0, 1.0,  -1.0, 1.0, 0.0, 0.5, 1.0, -1.0, 1.0,  1.0,  1.0, 0.0, 0.5, 1.0,
                                  1.0,  1.0,  1.0,  1.0, 0.0, 0.5, 1.0, 1.0,  1.0,  -1.0, 1.0, 0.0, 0.5, 1.0};

  static const uint16_t cube_indices[] = {0,  1,  2,  0,  2,  3,  6,  5,  4,  7,  6,  4,  8,  9,  10, 8,  10, 11,
                                 14, 13, 12, 15, 14, 12, 16, 17, 18, 16, 18, 19, 22, 21, 20, 23, 22, 20};
  ecs_entity_t e = ecs_new_id(vd__state.ecs.world);
  ecs_set(vd__state.ecs.world, e, vd__static_mesh,
          {sg_make_buffer(&(sg_buffer_desc){.data = SG_RANGE(cube_vertices)}),
           sg_make_buffer(&(sg_buffer_desc){.type = SG_BUFFERTYPE_INDEXBUFFER, .data = SG_RANGE(cube_indices)}),
           36});
}

static void vd__v3d_render(void)
{
  HMM_Mat4 model = HMM_Scale(HMM_V3(25.0f, 25.0f, 25.0f));
  vd__state.v3d.fwd.display_uniforms.mvp = HMM_MulM4(vd__state.v3d.dbg.vp, model);
  sg_begin_pass(&(sg_pass){
    .action = {
      .colors[0] = {
        .load_action = SG_LOADACTION_CLEAR,
        .clear_value = { 0.13f, 0.13f, 0.13f, 1.0f } },
    },
    .swapchain = sglue_swapchain()
  });
  sg_apply_pipeline(vd__state.v3d.fwd.display.pip);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vd_v3d_display_vs_uniforms, &SG_RANGE(vd__state.v3d.fwd.display_uniforms));

  ecs_iter_t it = ecs_query_iter(vd__state.ecs.world, vd__state.v3d.static_mesh_query);
  while (ecs_query_next(&it))
  {
    vd__static_mesh *mesh = ecs_field(&it, vd__static_mesh, 1);

    printf("drawing cube with %d indices!!!\n", mesh->num_indices);
    
    sg_bindings bind = (sg_bindings) {
      .vertex_buffers[0] = mesh->vbuf,
      .index_buffer = mesh->ibuf
    };
    
    sg_apply_bindings(&bind);
    sg_draw(0, mesh->num_indices, 1);
  }
  vd__dbg_draw();
  sg_end_pass();
  sg_commit();
  // sg_begin_pass(state.shadow.pass, &state.shadow.pass_action);
  // sg_apply_pipeline(state.shadow.pip);
  // sg_apply_bindings(&state.shadow.bind);
  // sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_shadow_params, &SG_RANGE(cube_vs_shadow_params));
  // sg_draw(0, 36, 1);
  // sg_end_pass();

  // sg_begin_pass(vd__state.v3d.fwd.shadow.pass, &vd__state.v3d.fwd.shadow.pass_action);
}

static void vd__v3d_shadow_pass_init(vd__v3d_offscreen_pass *shadow, int size)
{
  /* shadow->pass_action = (sg_pass_action){.colors[0] = {
                                           .load_action = SG_LOADACTION_CLEAR,
                                           .clear_value = {1.0f, 1.0f, 1.0f, 1.0f},
                                         }};

  shadow->color_target = sg_make_image(&(sg_image_desc){
    .render_target = true,
    .width = size,
    .height = size,
    .pixel_format = SG_PIXELFORMAT_RGBA8,
    .sample_count = 1,
    .label = "shadow-map",
  });

  sg_image shadow_depth_img = sg_make_image(&(sg_image_desc){
    .render_target = true,
    .width = size,
    .height = size,
    .pixel_format = SG_PIXELFORMAT_DEPTH,
    .sample_count = 1,
    .label = "shadow-depth-buffer",
  });

  shadow->sampler = sg_make_sampler(&(sg_sampler_desc){
    .min_filter = SG_FILTER_NEAREST,
    .mag_filter = SG_FILTER_NEAREST,
    .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
    .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
    .label = "shadow-sampler",
  });

  shadow->pass = sg_make_pass(&(sg_pass_desc){
    .color_attachments[0].image = shadow->color_target,
    .depth_stencil_attachment.image = shadow_depth_img,
    .label = "shadow-pass",
  });

  shadow->pip = sg_make_pipeline(
    &(sg_pipeline_desc){.layout =
                          {
                            // need to provide vertex stride, because normal component is skipped in shadow pass
                            .buffers[0].stride = 6 * sizeof(float),
                            .attrs =
                              {
                                [ATTR_vs_shadow_pos].format = SG_VERTEXFORMAT_FLOAT3,
                              },
                          },
                        .shader = sg_make_shader(shadow_shader_desc(sg_query_backend())),
                        .index_type = SG_INDEXTYPE_UINT16,
                        // render back-faces in shadow pass to prevent shadow acne on front-faces
                        .cull_mode = SG_CULLMODE_FRONT,
                        .sample_count = 1,
                        .colors[0].pixel_format = SG_PIXELFORMAT_RGBA8,
                        .depth =
                          {
                            .pixel_format = SG_PIXELFORMAT_DEPTH,
                            .compare = SG_COMPAREFUNC_LESS_EQUAL,
                            .write_enabled = true,
                          },
                        .label = "shadow-pipeline"});*/
}

static void vd__v3d_display_pass_init(vd__v3d_display_pass *display)
{
  display->pass_action = (sg_pass_action){
    .colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.13f, 0.13f, 0.13f, 1.0f}},
  };

  display->pip = sg_make_pipeline(&(sg_pipeline_desc){
    .layout = {.attrs =
                 {
                   [ATTR_vd_v3d_display_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
                   [ATTR_vd_v3d_display_vs_color0].format = SG_VERTEXFORMAT_FLOAT4,
                 }},
    .shader = sg_make_shader(vd_v3d_display_shader_desc(sg_query_backend())),
    .index_type = SG_INDEXTYPE_UINT16,
    .cull_mode = SG_CULLMODE_BACK,
    .depth =
      {
        .compare = SG_COMPAREFUNC_LESS_EQUAL,
        .write_enabled = true,
      },
    .label = "display-pipeline",
  });
}

static void vd__v3d_init()
{
  vd__state.v3d.alloc = vd__mem_create_allocator("v3d", VD__MEMOPTION_INHERIT, NULL, vd__core_heap_alloc());

  vd__asset_register_asset_type("skeleton",
                                (vd__asset_callbacks){
                                  .on_prepare = vd__v3d_skeleton_on_prepare,
                                  .on_load = vd__v3d_skeleton_on_load,
                                  .on_finalize = vd__v3d_skeleton_on_finalize,
                                  .on_release = vd__v3d_skeleton_on_release,
                                  .on_reload = vd__v3d_skeleton_on_reload,
                                },
                                "vd__skeleton_load_params", sizeof(vd__skeleton_load_params), (vd__asset_obj){0},
                                (vd__asset_obj){0}, 0);
  vd__asset_register_asset_type("doll",
                                (vd__asset_callbacks){
                                  .on_prepare = vd__v3d_doll_on_prepare,
                                  .on_load = vd__v3d_doll_on_load,
                                  .on_finalize = vd__v3d_doll_on_finalize,
                                  .on_release = vd__v3d_doll_on_release,
                                  .on_reload = vd__v3d_doll_on_reload,
                                },
                                "vd__doll_load_params", sizeof(vd__v3d_doll_load_params), (vd__asset_obj){0},
                                (vd__asset_obj){0}, VD__ASSET_LOAD_FLAG_TEXT_FILE);


  ECS_COMPONENT_DEFINE(vd__state.ecs.world, vd__static_mesh);

  vd__state.v3d.static_mesh_query = ecs_query_new(vd__state.ecs.world, "vd__static_mesh");

  vd__v3d_shadow_pass_init(&vd__state.v3d.fwd.shadow, VD__CONFIG_SHADOW_MAP_SIZE);
  vd__v3d_display_pass_init(&vd__state.v3d.fwd.display);
  vd__v3d_cube();
}

//    ___   ___  ___
//   / _ | / _ \/ _ \
//  / __ |/ ___/ ___/
// /_/ |_/_/  /_/
//
// >>app

int vd__app_width()
{
  return sapp_width();
}

int vd__app_height()
{
  return sapp_height();
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

void vd__app_init()
{
  vd__core_init(&(vd__config){0});
  vd__ecs_init();
  vd__asset_init();
  vd__script_init("game");
  vd__gfx_init();
  vd__v3d_init();
  vd__dbg_draw_init();

  vd__vfs_mount("/assets", "/assets", false);

  Janet ret;
  JanetSignal status =
    vd__app_janet_pcall_keep_env(vd__state.app.mod_init_cb, 0, NULL, &ret, &vd__state.app.main_fiber);
  if (status == JANET_SIGNAL_ERROR)
  {
    printf("error loading game\n");
    janet_stacktrace(vd__state.app.main_fiber, ret);
    sapp_quit();
  }

  vd__asset_load("doll", "/assets/dolls/ozz_skin.doll", &(vd__v3d_doll_load_params){}, 0, NULL, 0);
}

void vd__app_shutdown(void)
{
  vd__gfx_shutdown();
  vd__script_shutdown();
  vd__asset_shutdown();
  vd__vfs_shutdown();
  vd__ecs_shutdown();
  vd__core_shutdown();
  if (sargs_isvalid())
    sargs_shutdown();
  memset(&vd__state, 0, sizeof(vd__state));
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
  // MAIN_THREAD_EM_ASM({
  //   const isDirty = Module.get_signal(Module.signals.scriptDirty);
  //   if (isDirty)
  //   {
  //     const updatedScript = Module.editor.state.doc.toString();
  //     const updatedScriptByteArray = stringToNewUTF8(updatedScript);
  //     const result = Module._vd__script_evaluate(updatedScriptByteArray);
  //     Module.set_signal(Module.signals.scriptDirty, false);

  //     if (!result)
  //     {
  //       // Signal.set(evaluationState, EvaluationState.EvaluationError);
  //       console.error("failed evaluation");
  //     }
  //     else
  //     {
  //       console.log("successfully evaluated script!");
  //     }
  //     // outputChannel.target = null;
  //   }
  // });

  Janet ret;
  JanetSignal status =
    vd__app_janet_pcall_keep_env(vd__state.app.mod_update_cb, 0, NULL, &ret, &vd__state.app.main_fiber);
  if (status == JANET_SIGNAL_ERROR)
  {
    printf("error loading game\n");
    janet_stacktrace(vd__state.app.main_fiber, ret);
    sapp_quit();
  }

  vd__core_update();
}

sapp_desc sokol_main(int argc, char *argv[])
{
  sargs_setup(&(sargs_desc){.argc = argc, .argv = argv});

  const char *mod = NULL, *err = NULL;
  if (sargs_exists("run"))
  {
    mod = sargs_value("run");
  }

  vd__script_init(mod);

  return (sapp_desc){
    .init_cb = vd__app_init,
    .event_cb = vd__app_event,
    .frame_cb = vd__app_update,
    .cleanup_cb = vd__app_shutdown,
    .width = vd__state.app.width,
    .height = vd__state.app.height,
    .sample_count = 4,
    .window_title = "Cube (sokol-app)",
    .icon.sokol_default = true,
    .logger.func = slog_func,
  };
}

static Janet cfun_vd_app_width(int32_t argc, Janet *argv)
{
  return janet_wrap_integer(vd__app_width());
}

static Janet cfun_vd_app_height(int32_t argc, Janet *argv)
{
  return janet_wrap_integer(vd__app_height());
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // VOODOO_IMPL
