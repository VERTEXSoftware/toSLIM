#include <iostream>
#include <string>
#include <algorithm>
#include "SLIM/SLIM.h"
#include "support/support.h"

#ifndef STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#endif

#include "../external/stb/stb_image_resize2.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

void ImageConsoleViewer(unsigned char* data, int w, int h, SLIMCODE code) {
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);

    int ch = CodeToChannel(code);  // количество каналов: 1, 2, 3 или 4

    int term_width = 120;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        term_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize wline{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &wline) == 0)
        term_width = wline.ws_col;
#endif

    int new_w = (std::max)(40, term_width - 4);
    int new_h = (static_cast<long long>(h) * new_w) / w;
    if (new_h % 2 == 1) ++new_h;

    unsigned char* resized = (unsigned char*)SLIM_MALLOC(new_w * new_h * 3);


    if (ch == 4) {
        unsigned char* temp = (unsigned char*)SLIM_MALLOC(w * h * 3);
        for (int i = 0; i < w * h; ++i) {
            int src = i * 4;
            int dst = i * 3;
            if(data[src + 3]>1){
                temp[dst]     = data[src];
                temp[dst + 1] = data[src + 1];
                temp[dst + 2] = data[src + 2];
            }else{
                temp[dst]     = 0;
                temp[dst + 1] = 0;
                temp[dst + 2] = 0;
            }
        }

        stbir_resize_uint8_linear(temp, w, h, 0,resized, new_w, new_h, 0,STBIR_RGB);
        SLIM_Free(temp);
    }else if (ch == 3) {
        stbir_resize_uint8_linear(data, w, h, 0,resized, new_w, new_h, 0,STBIR_RGB);
    }else if (ch == 3) {
        unsigned char* temp = (unsigned char*)SLIM_MALLOC(w * h * 3);
        for (int i = 0; i < w * h; ++i) {
            unsigned char val = temp[i * ch];
            int idx = i * 3;
            resized[idx]     = val;
            resized[idx + 1] = val;
            resized[idx + 2] = val;
        }

        stbir_resize_uint8_linear(temp, w, h, 0,resized, new_w, new_h, 0,STBIR_RGB);
        SLIM_Free(temp);
    }


    std::string line_buffer;
    line_buffer.reserve(static_cast<size_t>(new_w) * 40);

    for (int y = 0; y < new_h; y += 2) {
        line_buffer.clear();

        for (int x = 0; x < new_w; ++x) {
            int i1 = (y * new_w + x) * 3;
            int r1 = resized[i1], g1 = resized[i1 + 1], b1 = resized[i1 + 2];

            int r2 = 0, g2 = 0, b2 = 0;
            if (y + 1 < new_h) {
                int i2 = ((y + 1) * new_w + x) * 3;
                r2 = resized[i2];
                g2 = resized[i2 + 1];
                b2 = resized[i2 + 2];
            }

            line_buffer += "\033[38;2;";
            line_buffer += std::to_string(r1); line_buffer += ';';
            line_buffer += std::to_string(g1); line_buffer += ';';
            line_buffer += std::to_string(b1); line_buffer += "m";

            line_buffer += "\033[48;2;";
            line_buffer += std::to_string(r2); line_buffer += ';';
            line_buffer += std::to_string(g2); line_buffer += ';';
            line_buffer += std::to_string(b2); line_buffer += "m";

            line_buffer += "▀";
        }
        line_buffer += "\033[0m\n";
        std::cout << line_buffer;
    }

    SLIM_Free(resized);
}