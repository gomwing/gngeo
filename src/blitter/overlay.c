
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>

#include "SDL.h"
#include "../emu.h"
#include "../screen.h"
#include "../video.h"
#include "../conf.h"
#include "../gnutil.h"

static SDL_Rect ov_rect;
//static SDL_Overlay *overlay;
static SDL_Texture* overlay;

extern SDL_Renderer* ren;

int blitter_overlay_init()
{	

    Uint32 width = visible_area.w;
    Uint32 height = visible_area.h;



#ifdef SDL1

    Uint32 sdl_flags = (fullscreen?SDL_FULLSCREEN:0)|SDL_HWSURFACE|SDL_RESIZABLE;
#else
    Uint32 sdl_flags = 0;// (fullscreen ? SDL_FULLSCREEN : 0) | SDL_HWSURFACE | SDL_RESIZABLE;
#endif
    Uint32 i;

    overlay=NULL;

    /* TODO : for now, effect are not handled by the YUV blitter */
    if (neffect != 0) {
	printf("WARNING: Overlay does not support effect.\n");
	return GN_FALSE;
    }
	
#ifdef SDL1 	
	/* YV12 blitter must be double sized */
    if (scale<2) scale=2;
    width *= scale;
    height *= scale;
    conf.res_x = width;
    conf.res_y = height;


        //screen = SDL_SetVideoMode(width, height, 16, sdl_flags);
    screen = SDL_SetVideoMode(width, height, 0, sdl_flags);

    overlay = SDL_CreateYUVOverlay(visible_area.w*2,visible_area.h*2,SDL_YV12_OVERLAY,screen);
    for (i = 0; i < overlay->pitches[1] * overlay->h / 2; i++) {
        overlay->pixels[1][i] = 128;
        overlay->pixels[2][i] = 128;
    }
#else
    overlay = SDL_CreateTexture(ren, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING,
        width *2, height * 2);
#endif	

    
    init_rgb2yuv_table();
    
    ov_rect.x=0;
    ov_rect.y=0;
    ov_rect.w=width;
    ov_rect.h=height;
    return GN_TRUE;
}

int
blitter_overlay_resize(int w,int h)
{
#ifdef SDL1
  Uint32 sdl_flags = SDL_HWSURFACE|SDL_RESIZABLE;
  screen = SDL_SetVideoMode(w, h, 16, sdl_flags);
  ov_rect.x=0;
  ov_rect.y=0;
  ov_rect.w=w;
  ov_rect.h=h;
#endif
  return GN_TRUE;
}

void blitter_overlay_update(void)
{
#ifdef SDL1
    int x,y;
    Uint16 *buf=(Uint16*) buffer->pixels + visible_area.x + visible_area.y*(buffer->pitch>>1);
    Uint16 *bufy=(Uint16 *)overlay->pixels[0];
    Uint16 *nbufy=(Uint16 *)overlay->pixels[0]+(overlay->pitches[0]>>1);
    Uint8  *bufu=(Uint8 *)overlay->pixels[1];
    Uint8  *bufv=(Uint8 *)overlay->pixels[2];
    for(y=0;y<visible_area.h;y++) {
        for(x=0;x<visible_area.w;x++) {
            bufy[x]=rgb2yuv[buf[x]].y;
            nbufy[x]=rgb2yuv[buf[x]].y;
            bufu[x]=rgb2yuv[buf[x]].u;
            bufv[x]=rgb2yuv[buf[x]].v;
        }
        bufy+=(overlay->pitches[0]);
        nbufy+=(overlay->pitches[0]);
        bufu+=(overlay->pitches[1]);
        bufv+=(overlay->pitches[2]);
        buf+=(buffer->pitch>>1);
    }
    SDL_DisplayYUVOverlay(overlay,&ov_rect);
#endif
}

void
blitter_overlay_close() {
#ifdef SDL1
    if (overlay) SDL_FreeYUVOverlay(overlay);
    scale=CF_VAL(cf_get_item_by_name("scale"));
#else
    if (overlay) SDL_DestroyTexture(overlay);
#endif
}
	
extern SDL_Window* sdl_window;

void blitter_overlay_fullscreen() {
#ifdef SDL1
    SDL_WM_ToggleFullScreen(screen);
#else
    SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);
#endif
}
	
