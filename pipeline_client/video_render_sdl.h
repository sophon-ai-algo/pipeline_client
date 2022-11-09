#pragma once

#include <SDL2/SDL.h>
#include "video_render.h"
#include <deque>

namespace fdrtsp {
    class video_render_sdl : public video_render_interface
    {
        int m_nWidth{ 0 };
        int m_nHeight{ 0 };
        SDL_Rect m_nWindowRect;

        std::deque<SDL_Rect> m_queRects;

        SDL_Window *m_sdl_window{ nullptr };
        SDL_Renderer *m_sdl_render{ nullptr };
        SDL_Texture *m_sdl_texture;
        SDL_Texture *m_sdl_osd;

        void *m_hwnd{ 0 };

        int init_sdl();

    public:
        video_render_sdl();
        virtual ~video_render_sdl();
        int set_hwnd(void* hwnd) override;

        int draw_yuv420(const uint8_t* const data[], int const linesize[], int w, int h) override;
        int draw_rect(int x, int y, int w, int h) override;

        int image_width() override { return m_nWidth; }
        int image_height() override { return m_nHeight; }
        int resize(int x, int y, int w, int h) override;
    };
}


