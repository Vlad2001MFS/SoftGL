#include "GL.hpp"
#include "SGL.hpp"
#include "SDL2/SDL.h"
#include "fmt/format.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <vector>

constexpr auto WND_TITLE = "SoftGL Demo";
constexpr auto WND_WIDTH = 1024;
constexpr auto WND_HEIGHT = 768;

SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
SDL_Texture *gTarget = nullptr;

void initialize() {
    glShadeModel(GL_SMOOTH);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void shutdown() {

}

void onEvent(const SDL_Event &event) {
    if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int w = event.window.data1;
            int h = event.window.data2;
            glViewport(0, 0, w, h);

            glMatrixMode(GL_PROJECTION);
            glLoadMatrixf(glm::value_ptr(glm::perspective(glm::radians(45.0f), static_cast<float>(w) / h, 0.1f, 100.0f)));
        
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
    }
}

//float gRotTri = 0.0f;
//float gRotQuad = 0.0f;

void update(float dt) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(-1.5f, 0.0f, -6.0f);
    //glRotatef(gRotTri, 0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f( 0.0f, 1.0f, 0.0f);  // Вверх
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f,-1.0f, 0.0f);  // Слева снизу
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f( 1.0f,-1.0f, 0.0f);  // Справа снизу
    glEnd();

    glLoadIdentity();
    glTranslatef(1.5f, 0.0f, -6.0f);
    //glRotatef(gRotQuad, 0.0f, 1.0f, 0.0f);
    glColor3f(0.5f, 0.5f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 0.0f);  // Слева вверху
    glVertex3f( 1.0f, 1.0f, 0.0f);  // Справа вверху
    glVertex3f( 1.0f,-1.0f, 0.0f);  // Справа внизу
    glVertex3f(-1.0f,-1.0f, 0.0f);  // Слева внизу
    glEnd();

    //gRotTri += 0.2f;
    //gRotQuad -= 0.15f;
}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

    gWindow = SDL_CreateWindow(WND_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WND_WIDTH, WND_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
    gTarget = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WND_WIDTH, WND_HEIGHT);

    SDL_Event resizeEvent;
    resizeEvent.type = SDL_WINDOWEVENT;
    resizeEvent.window.event = SDL_WINDOWEVENT_RESIZED;
    resizeEvent.window.data1 = WND_WIDTH;
    resizeEvent.window.data2 = WND_HEIGHT;
    SDL_PushEvent(&resizeEvent);

    initialize();

    uint64_t perfCounterFreq = SDL_GetPerformanceFrequency();
    uint64_t frameTimer = SDL_GetPerformanceCounter();
    uint32_t fpsTimer = SDL_GetTicks();
    uint32_t fps = 0;
    double frameTime = 0;
    double totalFrameTime = 0;
    uint32_t totalFrameTimeCount = 0;

    bool isExit = false;
    while (!isExit) {
        frameTime = ((double)(SDL_GetPerformanceCounter() - frameTimer)) / perfCounterFreq;
        frameTimer = SDL_GetPerformanceCounter();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isExit = true;
            }
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_DestroyTexture(gTarget);
                    gTarget = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, event.window.data1, event.window.data2);

                    sglUpdateBuffers(event.window.data1, event.window.data2);
                }
            }
            else if (event.type == SDL_KEYUP) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    isExit = true;
                }
            }

            onEvent(event);
        }
        
        update(frameTime);

        uint32_t *colorBuffer = nullptr;
        int colorBufferPitch = 0;
        sglGetColorBuffer(&colorBuffer, &colorBufferPitch);
        SDL_UpdateTexture(gTarget, nullptr, colorBuffer, colorBufferPitch);
        SDL_RenderCopy(gRenderer, gTarget, nullptr, nullptr);
        SDL_RenderPresent(gRenderer);

        if (SDL_GetTicks() - fpsTimer > 1000) {
            double time = totalFrameTime / (totalFrameTimeCount > 0 ? totalFrameTimeCount : 1);
            uint32_t fps = (uint32_t)(1.0 / (time > 0.0 ? time : 1.0));
            SDL_SetWindowTitle(gWindow, fmt::format("{} | FPS: {} | FrameTime: {}", WND_TITLE, fps, time*1000.0).data());
            fpsTimer = SDL_GetTicks();
            totalFrameTime = 0.0;
            totalFrameTimeCount = 0;
        }

        totalFrameTime += frameTime;
        totalFrameTimeCount++;
    }

    shutdown();

    SDL_UnlockTexture(gTarget);
    SDL_DestroyTexture(gTarget);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
}