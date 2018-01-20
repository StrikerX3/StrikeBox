#include <stdio.h>
#include <assert.h>
#include <SDL.h>
#include <GL/glew.h>
#include <dev.h>
#include <log.h>
#include "video.h"
#include <util.h>
#include "alloc.h"

#define PCRTC_START 0x00600800

const char *vert_shader_src = "\
#version 150 core                                                            \n\
in vec2 in_Position;                                                         \n\
in vec2 in_Texcoord;                                                         \n\
out vec2 Texcoord;                                                           \n\
void main()                                                                  \n\
{                                                                            \n\
    Texcoord = in_Texcoord;                                                  \n\
    gl_Position = vec4(in_Position, 0.0, 1.0);                               \n\
}                                                                            \n\
";

const char *frag_shader_src = "\
#version 150 core                                                            \n\
in vec2 Texcoord;                                                            \n\
out vec4 out_Color;                                                          \n\
uniform sampler2D tex;                                                       \n\
void main()                                                                  \n\
{                                                                            \n\
    out_Color = texture(tex, Texcoord);                                      \n\
}                                                                            \n\
";

const GLfloat verts[6][4] = {
    //  x      y      s      t
    { -1.0f, -1.0f,  0.0f,  1.0f }, // BL
    { -1.0f,  1.0f,  0.0f,  0.0f }, // TL
    {  1.0f,  1.0f,  1.0f,  0.0f }, // TR
    {  1.0f, -1.0f,  1.0f,  1.0f }, // BR
};

const GLint indicies[] = {
    0, 1, 2, 0, 2, 3
};

Video::Video(MemoryRegion *mem, MemoryRegion *ram, Scheduler *sched)
{
    m_mem   = mem;
    m_ram   = ram;
    m_sched = sched;
}

Video::~Video()
{
    delete m_nv2a;
}

int Video::Initialize()
{
    // Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        log_error("Failed to initialize SDL video\n");
        return 1;
    }

    // Create main window
    m_window = SDL_CreateWindow(
        "OpenXBOX",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_OPENGL);
    if (m_window == NULL) {
        log_error("Failed to create main window\n");
        SDL_Quit();
        return 1;
    }

    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == NULL) {
        log_error("Failed to create GL context\n");
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // Use VSYNC

    // Initialize GL Extension Wrangler (GLEW)
    GLenum err;
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    err = glewInit();
    if (err != GLEW_OK) {
        log_error("Failed to init GLEW\n");
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    InitShaders();
    InitGeometry();
    InitTextures();

    m_nv2a = new Nv2aDevice(m_mem, m_ram, m_sched);
    assert(m_nv2a != NULL);

    return 0;
}

int Video::InitShaders()
{
    GLint status;
    char err_buf[512];

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Compile vertex shader
    m_vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vert_shader, 1, &vert_shader_src, NULL);
    glCompileShader(m_vert_shader);
    glGetShaderiv(m_vert_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(m_vert_shader, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        log_error("Vertex shader compilation failed: %s\n", err_buf);
        return 1;
    }

    // Compile fragment shader
    m_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_frag_shader, 1, &frag_shader_src, NULL);
    glCompileShader(m_frag_shader);
    glGetShaderiv(m_frag_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(m_frag_shader, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        log_error("Fragment shader compilation failed: %s\n", err_buf);
        return 1;
    }

    // Link vertex and fragment shaders
    m_shader_prog = glCreateProgram();
    glAttachShader(m_shader_prog, m_vert_shader);
    glAttachShader(m_shader_prog, m_frag_shader);
    glBindFragDataLocation(m_shader_prog, 0, "out_Color");
    glLinkProgram(m_shader_prog);
    glUseProgram(m_shader_prog);

    return 0;
}

int Video::InitGeometry()
{
    // Populate vertex buffer
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Populate element buffer
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    // Bind vertex position attribute
    GLint pos_attr_loc = glGetAttribLocation(m_shader_prog, "in_Position");
    glVertexAttribPointer(pos_attr_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(pos_attr_loc);

    // Bind vertex texture coordinate attribute
    GLint tex_attr_loc = glGetAttribLocation(m_shader_prog, "in_Texcoord");
    glVertexAttribPointer(tex_attr_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
    glEnableVertexAttribArray(tex_attr_loc);

    return 0;
}

int Video::InitTextures()
{
    glGenTextures(1, &m_tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glUniform1i(glGetUniformLocation(m_shader_prog, "tex"), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return 0;
}

int Video::Cleanup()
{
    glUseProgram(0);
    glDisableVertexAttribArray(0); // FIXME
    glDetachShader(m_shader_prog, m_vert_shader);
    glDetachShader(m_shader_prog, m_frag_shader);
    glDeleteProgram(m_shader_prog);
    glDeleteShader(m_vert_shader);
    glDeleteShader(m_frag_shader);
    glDeleteTextures(1, &m_tex);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    return 0;
}

int Video::Update()
{
    void *fb = m_nv2a->GetFramebuffer();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 640, 480, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, fb);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    SDL_GL_SwapWindow(m_window);
    return 0;
}

Nv2aDevice::Nv2aDevice(MemoryRegion *mem, MemoryRegion *ram, Scheduler *sched)
: Device::Device(mem)
{
    m_sched = sched;

    // Create MMIO region -- 16 MiB of address space beginning at 0xFD000000
    // which contains the control registers and some buffers for the NV2A.
    uint32_t mmio_base = 0xFD000000;
    uint32_t mmio_size = 0x1000000;
    void *mmio_data;

    mmio_data = valloc(mmio_size);
    assert(mmio_data != NULL);
    memset(mmio_data, 0, mmio_size);

    m_mmio = new MemoryRegion(MEM_REGION_MMIO, mmio_base, mmio_size, mmio_data);
    assert( m_mmio != NULL);
    // m_mmio->SetEventHandler(Nv2aDevice::EventHandler, this);
    mem->AddSubRegion(m_mmio);

    // Create VRAM region -- NV2A shares system memory with CPU. Memory can be
    // accessed normally from CPU through the associated physical address, and
    // it can also be accessed via the NV2A through a MMIO window which begins
    // at the address specified by the second BAR of the NV2A PCI device. For
    // example, the memory at physical address 0x100000 can also be accessed
    // at address 0xF0100000. For performance, Xbox system software configures
    // the caching policy of this aliased region to be write-combining.
    uint32_t vram_base = 0xF0000000;
    uint32_t vram_size = ram->m_size;
    void *vram_data = ram->m_data;

    m_vram = new MemoryRegion(MEM_REGION_MMIO, vram_base, vram_size, vram_data);
    assert(mmio_data != NULL && m_vram != NULL);
    // m_vram->SetEventHandler(Nv2aDevice::EventHandler, this);
    mem->AddSubRegion(m_vram);

    *(uint32_t*)((uint8_t*)m_mmio->m_data + PCRTC_START) = XBOX_FRAMEBUFFER_BASE;
}

Nv2aDevice::~Nv2aDevice()
{
    free(m_mmio->m_data);
    // Don't need to free vram (it's allocated as system memory!)
    delete m_mmio;
    delete m_vram;
}

int Nv2aDevice::EventHandler(MemoryRegion *region, struct MemoryRegionEvent *event, void *user_data)
{
    #if 0
    Nv2aDevice *inst = (Nv2aDevice *)user_data;
    uint32_t offset = event->addr - inst->m_mmio_base;
    
    log_debug("Nv2aDevice::EventHandler! %08x\n", event->addr);
    if (offset == PCRTC_START) {
        log_debug("Read to PCRTC_START\n");
    }
    #endif

    return 0;
}

void *Nv2aDevice::GetFramebuffer()
{
    uint32_t offset = *(uint32_t*)((uint8_t*)m_mmio->m_data + PCRTC_START);
    return (char*)m_vram->m_data + offset;
}
