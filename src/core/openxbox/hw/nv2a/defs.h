#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include "nv2a_int.h"

namespace openxbox {

#define NV2A_SIZE 0x01000000

#define NV_PMC_ADDR      0x00000000
#define NV_PMC_SIZE      0x001000
#define NV_PBUS_ADDR     0x00001000
#define NV_PBUS_SIZE     0x001000
#define NV_PFIFO_ADDR    0x00002000
#define NV_PFIFO_SIZE    0x002000
#define NV_PRMA_ADDR     0x00007000
#define NV_PRMA_SIZE     0x001000
#define NV_PVIDEO_ADDR   0x00008000
#define NV_PVIDEO_SIZE   0x001000
#define NV_PTIMER_ADDR   0x00009000
#define NV_PTIMER_SIZE   0x001000
#define NV_PCOUNTER_ADDR 0x0000A000
#define NV_PCOUNTER_SIZE 0x001000
#define NV_PVPE_ADDR     0x0000B000
#define NV_PVPE_SIZE     0x001000
#define NV_PTV_ADDR      0x0000D000
#define NV_PTV_SIZE      0x001000
#define NV_PRMFB_ADDR    0x000A0000
#define NV_PRMFB_SIZE    0x020000
#define NV_PRMVIO_ADDR   0x000C0000
#define NV_PRMVIO_SIZE   0x001000
#define NV_PFB_ADDR      0x00100000
#define NV_PFB_SIZE      0x001000
#define NV_PSTRAPS_ADDR  0x00101000
#define NV_PSTRAPS_SIZE  0x001000
#define NV_PGRAPH_ADDR   0x00400000
#define NV_PGRAPH_SIZE   0x002000
#define NV_PCRTC_ADDR    0x00600000
#define NV_PCRTC_SIZE    0x001000
#define NV_PRMCIO_ADDR   0x00601000
#define NV_PRMCIO_SIZE   0x001000
#define NV_PRAMDAC_ADDR  0x00680000
#define NV_PRAMDAC_SIZE  0x001000
#define NV_PRMDIO_ADDR   0x00681000
#define NV_PRMDIO_SIZE   0x001000
#define NV_PRAMIN_ADDR   0x00700000
#define NV_PRAMIN_SIZE   0x100000
#define NV_USER_ADDR     0x00800000
#define NV_USER_SIZE     0x800000

class NV2ADevice;

enum FifoMode {
    FIFO_PIO = 0,
    FIFO_DMA = 1,
};

enum FIFOEngine {
    ENGINE_SOFTWARE = 0,
    ENGINE_GRAPHICS = 1,
    ENGINE_DVD = 2,
};

typedef struct VGACommonState {
    uint8_t st00; // Status Register 0
    uint8_t st01; // Status Register 1
} VGACommonState;

typedef struct ImageBlitState {
    uint32_t context_surfaces;
    unsigned int operation;
    unsigned int in_x, in_y;
    unsigned int out_x, out_y;
    unsigned int width, height;
} ImageBlitState;

typedef struct KelvinState {
    uint32_t dma_notifies;
    uint32_t dma_state;
    uint32_t dma_semaphore;
    unsigned int semaphore_offset;
} KelvinState;

typedef struct ContextSurfaces2DState {
    uint32_t dma_image_source;
    uint32_t dma_image_dest;
    unsigned int color_format;
    unsigned int source_pitch, dest_pitch;
    uint32_t source_offset, dest_offset;
} ContextSurfaces2DState;

typedef struct DMAObject {
    unsigned int dma_class;
    unsigned int dma_target;
    uint32_t address;
    uint32_t limit;
} DMAObject;

typedef struct GraphicsObject {
    uint8_t graphics_class;
    union {
        ContextSurfaces2DState context_surfaces_2d;

        ImageBlitState image_blit;

        KelvinState kelvin;
    } data;
} GraphicsObject;

typedef struct GraphicsSubchannel {
    uint32_t object_instance;
    GraphicsObject object;
    uint32_t object_cache[5];
} GraphicsSubchannel;

typedef struct GraphicsContext {
    bool channel_3d;
    unsigned int subchannel;
} GraphicsContext;

typedef struct RAMHTEntry {
    uint32_t handle;
    uint32_t instance;
    enum FIFOEngine engine;
    unsigned int channel_id : 5;
    bool valid;
} RAMHTEntry;

typedef struct {
    uint32_t offset;
    uint32_t size;
    void (*read)(NV2ADevice* nv2a, uint32_t addr, uint32_t *value, uint8_t size);
    void (*write)(NV2ADevice* nv2a, uint32_t addr, uint32_t value, uint8_t size);
} NV2ABlockInfo;

typedef struct {
    uint32_t regs[NV_PVIDEO_SIZE];
} NV2APVIDEO;

typedef struct {
    uint8_t cr_index;
    uint8_t cr[256]; /* CRT registers */
} NV2APRMCIO;

typedef struct {
    uint32_t pendingInterrupts;
    uint32_t enabledInterrupts;
} NV2APMC;

typedef struct Surface {
    bool draw_dirty;
    bool buffer_dirty;
    bool write_enabled_cache;
    unsigned int pitch;

    uint32_t offset;
} Surface;

typedef struct SurfaceShape {
    unsigned int z_format;
    unsigned int color_format;
    unsigned int zeta_format;
    unsigned int log_width, log_height;
    unsigned int clip_x, clip_y;
    unsigned int clip_width, clip_height;
    unsigned int anti_aliasing;
} SurfaceShape;

typedef struct TextureShape {
    bool cubemap;
    unsigned int dimensionality;
    unsigned int color_format;
    unsigned int levels;
    unsigned int width, height, depth;

    unsigned int min_mipmap_level, max_mipmap_level;
    unsigned int pitch;
} TextureShape;

typedef struct TextureKey {
    TextureShape state;
    uint64_t data_hash;
    uint8_t* texture_data;
    uint8_t* palette_data;
} TextureKey;

typedef struct TextureBinding {
    // FIXME: no OpenGL allowed here
    //GLenum gl_target;
    //GLuint gl_texture;
    unsigned int refcnt;
} TextureBinding;

typedef struct {
    std::mutex mutex;

    uint32_t pending_interrupts;
    uint32_t enabled_interrupts;
    std::condition_variable interrupt_cond;

    uint32_t context_table;
    uint32_t context_address;


    unsigned int trapped_method;
    unsigned int trapped_subchannel;
    unsigned int trapped_channel_id;
    uint32_t trapped_data[2];
    uint32_t notify_source;

    bool fifo_access;
    std::condition_variable fifo_access_cond;
    std::condition_variable flip_3d;

    unsigned int channel_id;
    bool channel_valid;
    GraphicsContext context[NV2A_NUM_CHANNELS];

    uint32_t dma_color, dma_zeta;
    Surface surface_color, surface_zeta;
    unsigned int surface_type;
    SurfaceShape surface_shape;
    SurfaceShape last_surface_shape;

    uint32_t dma_a, dma_b;
    //GLruCache *texture_cache;
    bool texture_dirty[NV2A_MAX_TEXTURES];
    TextureBinding *texture_binding[NV2A_MAX_TEXTURES];

    //GHashTable *shader_cache;
    //ShaderBinding *shader_binding;

    bool texture_matrix_enable[NV2A_MAX_TEXTURES];

    /* FIXME: Move to NV_PGRAPH_BUMPMAT... */
    float bump_env_matrix[NV2A_MAX_TEXTURES - 1][4]; /* 3 allowed stages with 2x2 matrix each */

    // FIXME: no OpenGL allowed here
    ////wglContext *gl_context;
    //GLuint gl_framebuffer;
    //GLuint gl_color_buffer, gl_zeta_buffer;
    GraphicsSubchannel subchannel_data[NV2A_NUM_SUBCHANNELS];

    uint32_t dma_report;
    uint32_t report_offset;
    bool zpass_pixel_count_enable;
    unsigned int zpass_pixel_count_result;
    unsigned int gl_zpass_pixel_count_query_count;
    // FIXME: no OpenGL allowed here
    //GLuint* gl_zpass_pixel_count_queries;

    uint32_t dma_vertex_a, dma_vertex_b;

    unsigned int primitive_mode;

    bool enable_vertex_program_write;

    //uint32_t program_data[NV2A_MAX_TRANSFORM_PROGRAM_LENGTH][VSH_TOKEN_SIZE];

    uint32_t vsh_constants[NV2A_VERTEXSHADER_CONSTANTS][4];
    bool vsh_constants_dirty[NV2A_VERTEXSHADER_CONSTANTS];

    /* lighting constant arrays */
    uint32_t ltctxa[NV2A_LTCTXA_COUNT][4];
    bool ltctxa_dirty[NV2A_LTCTXA_COUNT];
    uint32_t ltctxb[NV2A_LTCTXB_COUNT][4];
    bool ltctxb_dirty[NV2A_LTCTXB_COUNT];
    uint32_t ltc1[NV2A_LTC1_COUNT][4];
    bool ltc1_dirty[NV2A_LTC1_COUNT];

    // should figure out where these are in lighting context
    float light_infinite_half_vector[NV2A_MAX_LIGHTS][3];
    float light_infinite_direction[NV2A_MAX_LIGHTS][3];
    float light_local_position[NV2A_MAX_LIGHTS][3];
    float light_local_attenuation[NV2A_MAX_LIGHTS][3];

    //VertexAttribute vertex_attributes[NV2A_VERTEXSHADER_ATTRIBUTES];

    unsigned int inline_array_length;
    uint32_t inline_array[NV2A_MAX_BATCH_LENGTH];
    // FIXME: no OpenGL allowed here
    //GLuint gl_inline_array_buffer;

    unsigned int inline_elements_length;
    uint32_t inline_elements[NV2A_MAX_BATCH_LENGTH];

    unsigned int inline_buffer_length;

    unsigned int draw_arrays_length;
    unsigned int draw_arrays_max_count;

    // FIXME: no OpenGL allowed here
    /* FIXME: Unknown size, possibly endless, 1000 will do for now */
    //GLint gl_draw_arrays_start[1000];
    //GLsizei gl_draw_arrays_count[1000];

    //GLuint gl_element_buffer;
    //GLuint gl_memory_buffer;
    //GLuint gl_vertex_array;

    uint32_t regs[NV_PGRAPH_SIZE]; // TODO : union
} NV2APGRAPH;

typedef struct {
    uint32_t pendingInterrupts;
    uint32_t enabledInterrupts;
    uint32_t start;
    uint32_t regs[NV_PCRTC_SIZE]; // TODO : union
} NV2APCRTC;

typedef struct {
    uint32_t pending_interrupts;
    uint32_t enabled_interrupts;
    uint32_t numerator;
    uint32_t denominator;
    uint32_t alarm_time;
    uint32_t regs[NV_PTIMER_SIZE];
} NV2APTIMER;

typedef struct {
    uint32_t core_clock_coeff;
    uint64_t core_clock_freq;
    uint32_t memory_clock_coeff;
    uint32_t video_clock_coeff;
    uint32_t regs[NV_PRAMDAC_SIZE];
} NV2APRAMDAC;

typedef struct CacheEntry {
    unsigned int method : 14;
    unsigned int subchannel : 3;
    bool nonincreasing;
    uint32_t parameter;
} CacheEntry;

typedef struct Cache1State {
    unsigned int channel_id;
    FifoMode mode;

    /* Pusher state */
    bool push_enabled;
    bool dma_push_enabled;
    bool dma_push_suspended;
    uint32_t dma_instance;

    bool method_nonincreasing;
    unsigned int method : 14;
    unsigned int subchannel : 3;
    unsigned int method_count : 24;
    uint32_t dcount;

    bool subroutine_active;
    uint32_t subroutine_return;
    uint32_t get_jmp_shadow;
    uint32_t rsvd_shadow;
    uint32_t data_shadow;
    uint32_t error;

    bool pull_enabled;
    enum FIFOEngine bound_engines[NV2A_NUM_SUBCHANNELS];
    enum FIFOEngine last_engine;

    /* The actual command queue */
    std::mutex mutex;
    std::condition_variable cache_cond;
    std::queue<CacheEntry*> cache;
    std::queue<CacheEntry*> working_cache;
} Cache1State;

typedef struct {
    uint32_t pending_interrupts;
    uint32_t enabled_interrupts;
    Cache1State cache1;
    uint32_t regs[NV_PFIFO_SIZE];
    std::thread puller_thread;
} NV2APFIFO;

typedef struct {
    uint32_t registers[NV_PFB_SIZE];
} NV2APFB;

typedef struct ChannelControl {
    uint32_t dma_put;
    uint32_t dma_get;
    uint32_t ref;
} ChannelControl;

typedef struct {
    ChannelControl channel_control[NV2A_NUM_CHANNELS];
} NV2AUSER;

}
