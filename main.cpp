#include "SDL2/SDL.h"
#include "fmt/format.h"

constexpr auto WND_TITLE = "SoftGL Demo";
constexpr auto WND_WIDTH = 640;
constexpr auto WND_HEIGHT = 480;

SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
SDL_Texture *gTarget = nullptr;

void initialize() {

}

void shutdown() {

}

void onEvent(const SDL_Event &event) {

}

void update(float dt) {

}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

    gWindow = SDL_CreateWindow(WND_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WND_WIDTH, WND_HEIGHT, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    gTarget = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WND_WIDTH, WND_HEIGHT);

    initialize();

    uint64_t perfCounterFreq = SDL_GetPerformanceFrequency();
    uint64_t frameTimer = SDL_GetPerformanceCounter();
    uint32_t fpsTimer = SDL_GetTicks();
    uint32_t fps = 0;
    double frameTime = 0;

    bool isExit = false;
    while (!isExit) {
        frameTime = ((double)(SDL_GetPerformanceCounter() - frameTimer)) / perfCounterFreq;
        frameTimer = SDL_GetPerformanceCounter();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isExit = true;
            }

            onEvent(event);
        }

        update(frameTime);

        SDL_RenderCopy(gRenderer, gTarget, nullptr, nullptr);
        SDL_RenderPresent(gRenderer);

        if (SDL_GetTicks() - fpsTimer > 1000) {
            uint32_t fps = (uint32_t)(1.0 / (frameTime > 0.0 ? frameTime : 1.0));
            SDL_SetWindowTitle(gWindow, fmt::format("{} | FPS: {} | FrameTime: {}", WND_TITLE, fps, frameTime*1000.0).data());
            fpsTimer = SDL_GetTicks();
        }
    }

    shutdown();

    SDL_DestroyTexture(gTarget);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
}