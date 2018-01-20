#ifndef VIDEO_H
#define VIDEO_H

#include <dev.h>
#include <mem.h>

#include <SDL.h>
#include <GL/glew.h>

#include <sched.h>

class Nv2aDevice;

class Video {
protected:
    Scheduler     *m_sched;
    MemoryRegion  *m_mem;
    MemoryRegion  *m_ram;
    Nv2aDevice    *m_nv2a;

    SDL_Window    *m_window;
    SDL_GLContext  m_context;
    GLuint         m_vao, m_vbo, m_ebo, m_tex;
    GLuint         m_vert_shader;
    GLuint         m_frag_shader;
    GLuint         m_shader_prog;

public:
    Video(MemoryRegion *mem, MemoryRegion *ram, Scheduler *sched);
    ~Video();
    int Initialize();
    int Update();
    int Cleanup();
    int InitShaders();
    int InitGeometry();
    int InitTextures();
    int UpdateFrameData(char *data);
};

class Nv2aDevice : public Device {
protected:
    Scheduler    *m_sched;
    MemoryRegion *m_mmio;
    MemoryRegion *m_vram;

public:
    Nv2aDevice(MemoryRegion *mem, MemoryRegion *ram, Scheduler *sched);
    ~Nv2aDevice();
    static int EventHandler(MemoryRegion *region, struct MemoryRegionEvent *event, void *user_data);
    void *GetFramebuffer();
};

#endif
