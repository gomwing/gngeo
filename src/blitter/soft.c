
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>

#include "SDL.h"
#include "../emu.h"
#include "../screen.h"
#include "../video.h"
#include "../effect.h"
#include "../conf.h"
#include "../gnutil.h"
#ifdef GP2X
#include "../gp2x.h"

#define SC_STATUS     0x1802>>1
#define SC_DISP_FIELD (1<<8)

#endif

SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
unsigned short* texbuf = NULL;

/*
static SDL_Rect buf_rect	 =	{16, 16, 304, 224};
*/
#ifdef DEVKIT8000
static SDL_Rect screen_rect =	{ 0,  0, 304, 240};
#else
static SDL_Rect screen_rect =	{ 0,  0, 304, 224};
#endif
//static SDL_Surface *offscreen;
static int vsync;

extern SDL_Window* sdl_window;

int blitter_soft_init()
{
	Uint32 width = visible_area.w;
	Uint32 height = visible_area.h;



#ifdef GP2X
	Uint32 screen_w;
	int hw_surface=SDL_HWSURFACE/*|SDL_FULLSCREEN|SDL_DOUBLEBUF*/;
	int *tvoffset = CF_ARRAY(cf_get_item_by_name("tv_offset"));
	// TVTEST
	//int hw_surface=SDL_SWSURFACE;
#else
	int hw_surface=(CF_BOOL(cf_get_item_by_name("hwsurface"))?SDL_HWSURFACE:SDL_SWSURFACE);
#endif
	//int screen_size=CF_BOOL(cf_get_item_by_name("screen320"));
#ifdef DEVKIT8000
	Uint32 sdl_flags = hw_surface|(fullscreen?SDL_FULLSCREEN:0)|SDL_DOUBLEBUF;

	screen_rect.w=visible_area.w;
	screen_rect.h=240;
	height=240;
#else
	Uint32 sdl_flags = hw_surface | (fullscreen ? SDL_FULLSCREEN : 0);

	vsync = CF_BOOL(cf_get_item_by_name("vsync"));
	sdl_flags |= (vsync?SDL_DOUBLEBUF:0);

	if (vsync) {
		height=240;
		screen_rect.y = 8;

	} else {
		height=visible_area.h;
		screen_rect.y = 0;
		yscreenpadding=0;
	}

	screen_rect.w=visible_area.w;
	screen_rect.h=visible_area.h;


#endif
	if (neffect!=0)	scale =1;
	if (scale == 1) {
	    width *=effect[neffect].x_ratio;
	    height*=effect[neffect].y_ratio;
	} else {
	    if (scale > 3) scale=3;
	    width *=scale;
	    height *=scale;
	}
	
#ifdef PANDORA
		
	if (CF_BOOL(cf_get_item_by_name("wide"))) {
		setenv("SDL_OMAP_LAYER_SIZE","800x480",1);
	} else {
		setenv("SDL_OMAP_LAYER_SIZE","640x480",1);
	}
	
#endif
	
#ifdef GP2X
	//screen = SDL_SetVideoMode(width, height, 16, sdl_flags);

	//gp2x_video_RGB_setscaling(320, 240);
	//screen = SDL_SetVideoMode(320, 240, 16, 
	if ((screen_w=gp2x_is_tvout_on())==0) {
		screen = SDL_SetVideoMode(width, height, 16, 
					  sdl_flags|
					  (CF_BOOL(cf_get_item_by_name("vsync"))?SDL_DOUBLEBUF:0));

		if (CF_BOOL(cf_get_item_by_name("vsync"))) {
			set_LCD_custom_rate(LCDR_60);
		}
		
		if (width!=320) {
			//screen_rect.x=8;
			SDL_GP2X_MiniDisplay(8,8);
		} else {
			//screen_rect.y=8;
			SDL_GP2X_MiniDisplay(0,8);
		}
	} else {
		if (screen_w==240) /* ntsc */
			screen = SDL_SetVideoMode(360, 240, 16, 
						  sdl_flags|
						  (CF_BOOL(cf_get_item_by_name("vsync"))?SDL_DOUBLEBUF:0));
		else /* pal */
			screen = SDL_SetVideoMode(360, 288, 16, 
						  sdl_flags|
						  (CF_BOOL(cf_get_item_by_name("vsync"))?SDL_DOUBLEBUF:0));
		if (width!=320) {
			screen_rect.x=8;
		}
		screen_rect.y=8;
		/* tvout pseudo offset fix */
		screen_rect.y=(signed)screen_rect.y+tvoffset[1];
		screen_rect.x=(signed)screen_rect.x+tvoffset[0];
		if (screen_rect.x<0) {visible_area.x-=screen_rect.x;screen_rect.x=0;}
		if (screen_rect.y<0) {visible_area.y-=screen_rect.y;screen_rect.y=0;}
	}


#else

#ifdef SDL1
	screen = SDL_SetVideoMode(width, height, 16, sdl_flags);
	if (!screen) return GN_FALSE;
	// TODO: if (vsync) yscreenpadding = screen_rect.y * screen->pitch; 
	//offscreen = SDL_CreateRGBSurface(SDL_HWSURFACE, 304, 224, 16, 0xF800, 0x7E0, 0x1F, 0);

	return GN_TRUE;

#else
	screen = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 16, 0xF800, 0x7E0, 0x1F, 0);

	/*SDL_Renderer**/ ren = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == NULL) {
			_dbg("SDL_CreateRenderer Error: %d", SDL_GetError());
			ren = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE);
			if (ren == NULL) {
				_dbg("SDL_CreateRenderer unrecoverble Error: %d", SDL_GetError());

				_msg("SDL_CreateRenderer unrecoverble Error");
				//MessageBoxA(NULL, (LPCSTR)"SDL_CreateRenderer unrecoverble Error", (LPCSTR)"WC warning", MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2);
				SDL_DestroyWindow(sdl_window);
				return GN_FALSE;
			}
		}
		if (tex == NULL) {

			tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, width, height);
		}
		if (texbuf == NULL)
		{
			texbuf = malloc(width * height * 2);
		}
		//SDL_UpdateTexture(tex, NULL, texbuf, height * sizeof(short));
		return GN_TRUE;

#endif

	//SDL_ShowCursor(SDL_DISABLE);
#endif

}

void update_double() {
	Uint16 *src, *dst;
	Uint32 s, d;
	Uint8 w, h;
	
	src = (Uint16 *)buffer->pixels + visible_area.x + (buffer->w << 4);// LeftBorder + RowLength * UpperBorder

	dst = (Uint16 *)screen->pixels + yscreenpadding;
	
	for(h = visible_area.h; h > 0; h--)
	{
		for(w = visible_area.w>>1; w > 0; w--)
		{		
			s = *(Uint32 *)src;
#ifdef WORDS_BIGENDIAN
			d = (s & 0xFFFF0000) + ((s & 0xFFFF0000)>>16);
			*(Uint32 *)(dst) = d;
			*(Uint32 *)(dst+(visible_area.w<<1)) = d;
				
			d = (s & 0x0000FFFF) + ((s & 0x0000FFFF)<<16);
			*(Uint32 *)(dst+2) = d;
			*(Uint32 *)(dst+(visible_area.w<<1)+2) = d;
#else
			d = (s & 0x0000FFFF) + ((s & 0x0000FFFF) << 16);
			*(Uint32 *)(dst) = d;
			*(Uint32 *) (dst + (visible_area.w << 1)) = d;

			d = (s & 0xFFFF0000) + ((s & 0xFFFF0000)>>16);
			*(Uint32 *)(dst+2) = d;
			*(Uint32 *)(dst+(visible_area.w<<1)+2) = d;
				

#endif			
			dst += 4;
			src += 2;
		}
		//memcpy(dst,dst-(visible_area.w<<1),(visible_area.w<<2));
		src += (visible_area.x<<1);		
		dst += (visible_area.w<<1);
//		dst += (buffer->pitch);
	}
}

void update_triple() {
	Uint16 *src, *dst;
	Uint32 s, d;
	Uint8 w, h;
	
	src = (Uint16 *)buffer->pixels + visible_area.x + (buffer->w << 4);// LeftBorder + RowLength * UpperBorder
	dst = (Uint16 *)screen->pixels + yscreenpadding;
	
	for(h = visible_area.h; h > 0; h--)
	{
		for(w = visible_area.w>>1; w > 0; w--)
		{		
			s = *(Uint32 *)src;
#ifdef WORDS_BIGENDIAN
			d = (s & 0xFFFF0000) + ((s & 0xFFFF0000)>>16);
			*(Uint32 *)(dst) = d;
			*(Uint32 *)(dst+(visible_area.w*3)) = d;
			*(Uint32 *)(dst+(visible_area.w*6)) = d;
				
			*(Uint32 *)(dst+2) = s;
			*(Uint32 *)(dst+(visible_area.w*3)+2) = s;
			*(Uint32 *)(dst+(visible_area.w*6)+2) = s;

			d = (s & 0x0000FFFF) + ((s & 0x0000FFFF)<<16);
			*(Uint32 *)(dst+4) = d;
			*(Uint32 *)(dst+(visible_area.w*3)+4) = d;
			*(Uint32 *)(dst+(visible_area.w*6)+4) = d;

#else				
			d = (s & 0xFFFF0000) + ((s & 0xFFFF0000)>>16);
			*(Uint32 *)(dst+4) = d;
			*(Uint32 *)(dst+(visible_area.w*3)+4) = d;
			*(Uint32 *)(dst+(visible_area.w*6)+4) = d;

			*(Uint32 *)(dst+2) = s;
			*(Uint32 *)(dst+(visible_area.w*3)+2) = s;
			*(Uint32 *)(dst+(visible_area.w*6)+2) = s;

			d = (s & 0x0000FFFF) + ((s & 0x0000FFFF)<<16);
			*(Uint32 *)(dst) = d;
			*(Uint32 *)(dst+(visible_area.w*3)) = d;
			*(Uint32 *)(dst+(visible_area.w*6)) = d;
#endif			
			dst += 6;
			src += 2;
		}
		src += (visible_area.x<<1);		
		dst += (visible_area.w*6);
	}
}

#ifdef GP2X
int threaded_blit(void *buf)
{
	SDL_Surface *b=(SDL_Surface*)buf;
	SDL_BlitSurface(b, &visible_area, screen, &screen_rect);
	SDL_Flip(screen);
	return 0;
}
#endif

void blitter_soft_update() {
#ifdef GP2X
    SDL_BlitSurface(buffer, &visible_area, screen, &screen_rect);
    SDL_Flip(screen);
#else
#ifdef PANDORA
	if (neffect == 0 || neffect == 1) {
#else
		if (neffect == 0) {
#endif
			switch (scale) {
				case 2: update_double(); break;
				case 3: update_triple(); break;
				default:
					SDL_BlitSurface(buffer, &visible_area, screen, &screen_rect);
					break;
			}
			
		}
#ifdef DEVKIT8000
	SDL_Flip(screen);
#else

#ifdef SDL1
  if (vsync)  SDL_Flip(screen);
  else SDL_UpdateRect(screen, 0, 0, 0, 0);
#else
	//SDL_UpdateTexture(tex, NULL, texbuf, 320 * sizeof(unsigned short));
	SDL_UpdateTexture(tex, NULL, screen->pixels, 320 * sizeof(unsigned short));

	SDL_RenderClear(ren);
	SDL_RenderCopy(ren, tex, NULL, NULL);
	SDL_RenderPresent(ren);
#endif


#endif
//	SDL_Flip(screen);
#endif
 
}

void blitter_soft_close() {
}
	
void blitter_soft_fullscreen() {
#ifdef SDL1
  SDL_WM_ToggleFullScreen(screen);
#else
	SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);
#endif
}
	
