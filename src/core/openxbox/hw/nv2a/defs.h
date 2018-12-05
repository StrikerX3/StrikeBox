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
    unsigned int dma_class = 0;
    unsigned int dma_target = 0;
    uint32_t address = 0;
    uint32_t limit = 0;
} DMAObject;

typedef struct GraphicsObject {
    uint8_t graphics_class = 0;
    union {
        ContextSurfaces2DState context_surfaces_2d;

        ImageBlitState image_blit;

        KelvinState kelvin;
    } data;
} GraphicsObject;

typedef struct GraphicsSubchannel {
    uint32_t object_instance = 0;
    GraphicsObject object;
    uint32_t object_cache[5] = { 0 };
} GraphicsSubchannel;

typedef struct GraphicsContext {
    bool channel_3d = false;
    unsigned int subchannel = 0;
} GraphicsContext;

typedef struct RAMHTEntry {
    uint32_t handle = 0;
    uint32_t instance = 0;
    enum FIFOEngine engine = ENGINE_SOFTWARE;
    unsigned int channel_id : 5;
    bool valid = false;
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
    uint8_t cr_index = 0;
    uint8_t cr[256] = { 0 }; /* CRT registers */
} NV2APRMCIO;

typedef struct {
    uint32_t pendingInterrupts = 0;
    uint32_t enabledInterrupts = 0;
} NV2APMC;

typedef struct Surface {
    bool draw_dirty = false;
    bool buffer_dirty = false;
    bool write_enabled_cache = false;
    unsigned int pitch = 0;

    uint32_t offset = 0;
} Surface;

typedef struct SurfaceShape {
    unsigned int z_format = 0;
    unsigned int color_format = 0;
    unsigned int zeta_format = 0;
    unsigned int log_width = 0, log_height = 0;
    unsigned int clip_x = 0, clip_y = 0;
    unsigned int clip_width = 0, clip_height = 0;
    unsigned int anti_aliasing = 0;
} SurfaceShape;

typedef struct TextureShape {
    bool cubemap = false;
    unsigned int dimensionality = 0;
    unsigned int color_format = 0;
    unsigned int levels = 0;
    unsigned int width = 0, height = 0, depth = 0;

    unsigned int min_mipmap_level = 0;
    unsigned int max_mipmap_level = 0;
    unsigned int pitch = 0;
} TextureShape;

typedef struct TextureKey {
    TextureShape state;
    uint64_t data_hash = 0;
    uint8_t* texture_data = nullptr;
    uint8_t* palette_data = nullptr;
} TextureKey;

typedef struct TextureBinding {
    // FIXME: no OpenGL allowed here
    //GLenum gl_target;
    //GLuint gl_texture;
    unsigned int refcnt = 0;
} TextureBinding;

typedef struct NV2APGRAPH {
    std::mutex mutex;

    uint32_t pending_interrupts = 0;
    uint32_t enabled_interrupts = 0;
    std::condition_variable interrupt_cond;

    uint32_t context_table = 0;
    uint32_t context_address = 0;


    unsigned int trapped_method = 0;
    unsigned int trapped_subchannel = 0;
    unsigned int trapped_channel_id = 0;
    uint32_t trapped_data[2] = { 0 };
    uint32_t notify_source = 0;

    bool fifo_access = false;
    std::condition_variable fifo_access_cond;
    std::condition_variable flip_3d;

    unsigned int channel_id = 0;
    bool channel_valid = false;
    GraphicsContext context[NV2A_NUM_CHANNELS];

    uint32_t dma_color = 0;
    uint32_t dma_zeta = 0;
    Surface surface_color;
    Surface surface_zeta;
    unsigned int surface_type = 0;
    SurfaceShape surface_shape;
    SurfaceShape last_surface_shape;

    uint32_t dma_a = 0;
    uint32_t dma_b = 0;
    //GLruCache *texture_cache;
    bool texture_dirty[NV2A_MAX_TEXTURES] = { false };
    TextureBinding *texture_binding[NV2A_MAX_TEXTURES] = { nullptr };

    //GHashTable *shader_cache;
    //ShaderBinding *shader_binding;

    bool texture_matrix_enable[NV2A_MAX_TEXTURES] = { false };

    /* FIXME: Move to NV_PGRAPH_BUMPMAT... */
    float bump_env_matrix[NV2A_MAX_TEXTURES - 1][4]; /* 3 allowed stages with 2x2 matrix each */

    // FIXME: no OpenGL allowed here
    ////wglContext *gl_context;
    //GLuint gl_framebuffer;
    //GLuint gl_color_buffer, gl_zeta_buffer;
    GraphicsSubchannel subchannel_data[NV2A_NUM_SUBCHANNELS];

    uint32_t dma_report = 0;
    uint32_t report_offset = 0;
    bool zpass_pixel_count_enable = 0;
    unsigned int zpass_pixel_count_result = 0;
    unsigned int gl_zpass_pixel_count_query_count = 0;
    // FIXME: no OpenGL allowed here
    //GLuint* gl_zpass_pixel_count_queries;

    uint32_t dma_vertex_a = 0;
    uint32_t dma_vertex_b = 0;

    unsigned int primitive_mode = 0;

    bool enable_vertex_program_write = false;

    //uint32_t program_data[NV2A_MAX_TRANSFORM_PROGRAM_LENGTH][VSH_TOKEN_SIZE];

    uint32_t vsh_constants[NV2A_VERTEXSHADER_CONSTANTS][4] = { { 0 } };
    bool vsh_constants_dirty[NV2A_VERTEXSHADER_CONSTANTS] = { 0 };

    /* lighting constant arrays */
    uint32_t ltctxa[NV2A_LTCTXA_COUNT][4] = { { 0 } };
    bool ltctxa_dirty[NV2A_LTCTXA_COUNT] = { false };
    uint32_t ltctxb[NV2A_LTCTXB_COUNT][4] = { { 0 } };
    bool ltctxb_dirty[NV2A_LTCTXB_COUNT] = { false };
    uint32_t ltc1[NV2A_LTC1_COUNT][4] = { { 0 } };
    bool ltc1_dirty[NV2A_LTC1_COUNT] = { false };

    // should figure out where these are in lighting context
    float light_infinite_half_vector[NV2A_MAX_LIGHTS][3] = { { 0 } };
    float light_infinite_direction[NV2A_MAX_LIGHTS][3] = { { 0 } };
    float light_local_position[NV2A_MAX_LIGHTS][3] = { { 0 } };
    float light_local_attenuation[NV2A_MAX_LIGHTS][3] = { { 0 } };

    //VertexAttribute vertex_attributes[NV2A_VERTEXSHADER_ATTRIBUTES];

    unsigned int inline_array_length = 0;
    uint32_t inline_array[NV2A_MAX_BATCH_LENGTH] = { 0 };
    // FIXME: no OpenGL allowed here
    //GLuint gl_inline_array_buffer;

    unsigned int inline_elements_length = 0;
    uint32_t inline_elements[NV2A_MAX_BATCH_LENGTH] = { 0 };

    unsigned int inline_buffer_length = 0;

    unsigned int draw_arrays_length = 0;
    unsigned int draw_arrays_max_count = 0;

    // FIXME: no OpenGL allowed here
    /* FIXME: Unknown size, possibly endless, 1000 will do for now */
    //GLint gl_draw_arrays_start[1000];
    //GLsizei gl_draw_arrays_count[1000];

    //GLuint gl_element_buffer;
    //GLuint gl_memory_buffer;
    //GLuint gl_vertex_array;

    uint32_t regs[NV_PGRAPH_SIZE] = { 0 }; // TODO : union
} NV2APGRAPH;

typedef struct {
    uint32_t pendingInterrupts = 0;
    uint32_t enabledInterrupts = 0;
    uint32_t start = 0;
    uint32_t regs[NV_PCRTC_SIZE] = { 0 }; // TODO : union
} NV2APCRTC;

typedef struct {
    uint32_t pending_interrupts = 0;
    uint32_t enabled_interrupts = 0;
    uint32_t numerator = 0;
    uint32_t denominator = 0;
    uint32_t alarm_time = 0;
    uint32_t regs[NV_PTIMER_SIZE] = { 0 };
} NV2APTIMER;

typedef struct {
    uint32_t core_clock_coeff = 0;
    uint64_t core_clock_freq = 0;
    uint32_t memory_clock_coeff = 0;
    uint32_t video_clock_coeff = 0;
    uint32_t regs[NV_PRAMDAC_SIZE] = { 0 };
} NV2APRAMDAC;

typedef struct CacheEntry {
    unsigned int method : 14;
    unsigned int subchannel : 3;
    bool nonincreasing = false;
    uint32_t parameter = 0;
} CacheEntry;

typedef struct Cache1State {
    unsigned int channel_id = 0;
    FifoMode mode = FIFO_PIO;

    /* Pusher state */
    bool push_enabled = false;
    bool dma_push_enabled = false;
    bool dma_push_suspended = false;
    uint32_t dma_instance = 0;

    bool method_nonincreasing = false;
    unsigned int method : 14;
    unsigned int subchannel : 3;
    unsigned int method_count : 24;
    uint32_t dcount = 0;

    bool subroutine_active = false;
    uint32_t subroutine_return = 0;
    uint32_t get_jmp_shadow = 0;
    uint32_t rsvd_shadow = 0;
    uint32_t data_shadow = 0;
    uint32_t error = 0;

    bool pull_enabled = false;
    enum FIFOEngine bound_engines[NV2A_NUM_SUBCHANNELS] = { ENGINE_SOFTWARE };
    enum FIFOEngine last_engine = ENGINE_SOFTWARE;

    /* The actual command queue */
    std::mutex mutex;
    std::condition_variable cache_cond;
    std::queue<CacheEntry*> cache;
    std::queue<CacheEntry*> working_cache;
} Cache1State;

typedef struct {
    uint32_t pending_interrupts = 0;
    uint32_t enabled_interrupts = 0;
    Cache1State cache1;
    uint32_t regs[NV_PFIFO_SIZE] = { 0 };
    std::thread puller_thread;
} NV2APFIFO;

typedef struct {
    uint32_t registers[NV_PFB_SIZE] = { 0 };
} NV2APFB;

typedef struct ChannelControl {
    uint32_t dma_put = 0;
    uint32_t dma_get = 0;
    uint32_t ref = 0;
} ChannelControl;

typedef struct {
    ChannelControl channel_control[NV2A_NUM_CHANNELS];
} NV2AUSER;

}
