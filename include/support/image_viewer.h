
#include "SDL.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

SDL_PixelFormatEnum convertSLIMCODEToSDLPixelFormat(SLIMCODE code) {
    switch (code) {
        case CODE_RGB:  return SDL_PIXELFORMAT_RGB24;
        case CODE_RGBA: return SDL_PIXELFORMAT_RGBA32;
        case CODE_NONE: 
        default:        return SDL_PIXELFORMAT_UNKNOWN;
    }
}

class ImageViewer {
public:
    ImageViewer(unsigned char* data, int w, int h, SLIMCODE code): width(w), height(h), codes(code) 
    {
        if (!data || w <= 0 || h <= 0) {
            std::cerr << "Invalid image data\n";
            valid = false;
            return;
        }
        if(codes==SLIMCODE::CODE_MAP){
            dataimg = (unsigned char*)SLIM_MALLOC(w * h * 3);
            grayToViridis(data, dataimg, w, h);
            codes=SLIMCODE::CODE_RGB;
            ch = CodeToChannel(codes);
            valid = true;
        }else{
            ch = CodeToChannel(codes);
            dataimg = data;
            valid = true;
        }

    }

    void show(const std::string& title = "Image Viewer") {
        
        if (!valid) {
            std::cerr << "Cannot show invalid image\n";
            return;
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
            return;
        }

        const std::string& namef = getFilename(title);

        SDL_Window* window = SDL_CreateWindow(
            namef.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            std::min(std::max(width,800), 1280), std::min(std::max(height,600), 720),
            SDL_WINDOW_RESIZABLE
        );

        if (!window) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
            SDL_Quit();
            return;
        }

        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }

        SDL_PixelFormatEnum format = convertSLIMCODEToSDLPixelFormat(codes);

        if(format == SDL_PIXELFORMAT_UNKNOWN){ 
            std::cerr << "Not correct pixel format" << "\n";
            return;
        }

        SDL_Texture* texture = SDL_CreateTexture(
            renderer,
            format,
            SDL_TEXTUREACCESS_STATIC,
            width, height
        );

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_UpdateTexture(texture, nullptr, dataimg, width * ch);

        float zoom = 1.0f;
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        const float MIN_ZOOM = 0.05f;
        const float MAX_ZOOM = 20.0f;

        auto centerImage = [&](int winW, int winH) {
            float scaleX = (float)winW / width;
            float scaleY = (float)winH / height;
            zoom = std::min(scaleX, scaleY);
            offsetX = (winW - width * zoom) / 2.0f;
            offsetY = (winH - height * zoom) / 2.0f;
        };

        int winW, winH;
        SDL_GetWindowSize(window, &winW, &winH);
        centerImage(winW, winH);

        bool running = true;
        bool panning = false;
        auto lastClickTime = std::chrono::steady_clock::now();
        int clickCount = 0;

        while (running) {
            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                switch (ev.type) {
                    case SDL_QUIT:
                        running = false;
                        break;

                    case SDL_WINDOWEVENT:
                        if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                            SDL_GetWindowSize(window, &winW, &winH);
                        }
                        break;

                    case SDL_MOUSEWHEEL: {
                        int mx, my;
                        SDL_GetMouseState(&mx, &my);

                        float zoomFactor = (ev.wheel.y > 0) ? 1.1f : 0.9f;
                        float newZoom = zoom * zoomFactor;
                        if (newZoom < MIN_ZOOM) zoomFactor = MIN_ZOOM / zoom;
                        if (newZoom > MAX_ZOOM) zoomFactor = MAX_ZOOM / zoom;

                        float worldX = (mx - offsetX) / zoom;
                        float worldY = (my - offsetY) / zoom;

                        zoom *= zoomFactor;
                        offsetX = mx - worldX * zoom;
                        offsetY = my - worldY * zoom;
                        break;
                    }

                    case SDL_MOUSEBUTTONDOWN:
                        if (ev.button.button == SDL_BUTTON_LEFT) {
                            panning = true;
                        }
                        if (ev.button.button == SDL_BUTTON_LEFT) {
                            auto now = std::chrono::steady_clock::now();
                            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime).count();
                            if (ms < 400) clickCount++; else clickCount = 1;
                            lastClickTime = now;

                            if (clickCount >= 2) {
                                SDL_GetWindowSize(window, &winW, &winH);
                                centerImage(winW, winH);
                            }
                        }
                        break;

                    case SDL_MOUSEBUTTONUP:
                        if (ev.button.button == SDL_BUTTON_LEFT){panning = false;}
                        break;

                    case SDL_MOUSEMOTION:
                        if (panning) {
                            offsetX += ev.motion.xrel;
                            offsetY += ev.motion.yrel;
                        }
                        break;

                    case SDL_KEYDOWN:
                        if (ev.key.keysym.sym == SDLK_ESCAPE) running = false;
                        if (ev.key.keysym.sym == SDLK_f) {
                            SDL_GetWindowSize(window, &winW, &winH);
                            centerImage(winW, winH);
                        }
                        break;
                }
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_Rect dstRect = {
                static_cast<int>(offsetX),
                static_cast<int>(offsetY),
                static_cast<int>(width * zoom),
                static_cast<int>(height * zoom)
            };

            SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
            SDL_RenderPresent(renderer);
        }

        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

private:
    int width = 0;
    int height = 0;
    int ch = 0;
    SLIMCODE codes;
    bool valid = false;
    unsigned char* dataimg;

    std::string getFilename(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos){
            return path;
        }
        return path.substr(pos + 1);
    }
};