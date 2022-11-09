// VideoRender.cpp : ??????
//

#include "video_render_sdl.h"
#ifdef _WIN32
#include <Windows.h>
#endif

namespace fdrtsp {

    video_render_sdl::video_render_sdl()
    {
       m_nWindowRect.x = 0;
       m_nWindowRect.y = 0;
       m_nWindowRect.w = 640;
       m_nWindowRect.h = 480;
    }

    video_render_sdl::~video_render_sdl()
    {

    }

    int video_render_sdl::init_sdl()
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        return 0;
    }

    int video_render_sdl::draw_yuv420(const uint8_t* const data[], int const linesize[], int w, int h)
    {
        if (m_hwnd == nullptr) {
            return -1;
        }

        m_nWidth = w;
        m_nHeight = h;

        if (m_sdl_window == nullptr) {
            int ww, hh;
            // Initialize SDL Window

            m_sdl_window = SDL_CreateWindowFrom((void*)m_hwnd);
            SDL_GetWindowSize(m_sdl_window, &ww, &hh);
            SDL_SetWindowResizable(m_sdl_window, SDL_TRUE);

            m_sdl_render = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_SOFTWARE);
            m_sdl_texture = SDL_CreateTexture(m_sdl_render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, w, h);
            m_sdl_osd = SDL_CreateTexture(m_sdl_render, SDL_PIXELFORMAT_RGB_PLANAR888, SDL_TEXTUREACCESS_TARGET, w, h);
            SDL_SetRenderDrawColor(m_sdl_render, 128, 0, 0, 0);
        }



        //OSD Texture
        SDL_SetTextureBlendMode(m_sdl_texture, SDL_BLENDMODE_NONE);
        SDL_SetRenderTarget(m_sdl_render, m_sdl_osd);

        SDL_Rect rc;
        rc.x = 0; rc.y = 0;
        // Texture Size
        rc.w = w, rc.h = h;
        SDL_UpdateYUVTexture(m_sdl_texture, &rc, data[0], linesize[0], data[1], linesize[1], data[2], linesize[2]);

        SDL_RenderClear(m_sdl_render);
        SDL_RenderCopy(m_sdl_render, m_sdl_texture, NULL, NULL);

        while (m_queRects.size() > 0) {
            auto rc = m_queRects.front();
            SDL_RenderDrawRect(m_sdl_render, &rc);
            rc.x++;
            rc.y++;
            rc.w -= 2;
            rc.h -= 2;
            SDL_RenderDrawRect(m_sdl_render, &rc);
            m_queRects.pop_front();
        }

        //Change to default
        SDL_SetRenderTarget(m_sdl_render, NULL);

        SDL_RenderClear(m_sdl_render);
        SDL_RenderCopy(m_sdl_render, m_sdl_osd, NULL, NULL);

        SDL_RenderPresent(m_sdl_render);

        return 0;
    }

    int video_render_sdl::draw_rect(int x, int y, int w, int h)
    {
        if (m_sdl_render) {
            SDL_Rect rc;
            rc.x = x;
            rc.y = y;
            rc.w = w;
            rc.h = h;

            m_queRects.push_back(rc);
        }

        return 0;
    }

    int video_render_sdl::resize(int x, int y, int w, int h)
    {
        if (m_sdl_render) {

            m_nWindowRect.x = 0; m_nWindowRect.y = 0;
            m_nWindowRect.w = w; m_nWindowRect.h = h;
            SDL_SetWindowSize(m_sdl_window, w, h);
            SDL_SetWindowPosition(m_sdl_window, x, y);
        }

        return 0;
    }

#ifdef __linux__
    int video_render_sdl::set_hwnd(void* hwnd)
    {
        m_hwnd = hwnd;
        return 0;
    }
#endif

}



