#include <algorithm>
#include <iostream>
#include "general.h"
#include "simulation.h"
#include "visuals.h"

using std::cout;
using std::endl;

namespace visuals
{
	SDL_Window *win = NULL;
	SDL_Renderer *ren = NULL;
	SDL_Texture *bgTex = NULL;
	SDL_Rect bgRect;
	SDL_Rect blobRect;
	double transMult;
	Uint32 blobColors[blobColors_size] = {
		colors::red, colors::green,
		colors::blue, colors::redorange,
		colors::orange, colors::yelloworange,
		colors::yellowgreen, colors::bluegreen,
		colors::blueviolet, colors::violet,
		colors::redviolet, colors::cyan
	};

	int init(void)
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			cout << "SDL_Init Error: " << SDL_GetError() << endl;
			return 1;
		}

		win = SDL_CreateWindow("Test", 100, 100, win_w, win_h,
				       SDL_WINDOW_SHOWN );
		if (win == nullptr) {
			cout << "SDL_CreateWindow Error: " <<
				SDL_GetError() << endl;
			quit();
			return 1;
		}

		ren = SDL_CreateRenderer(win, -1,
					 SDL_RENDERER_ACCELERATED |
					 SDL_RENDERER_PRESENTVSYNC );
		if (ren == nullptr) {
			cout << "SDL_CreateRenderer Error: " <<
				SDL_GetError() << endl;
			quit();
			return 1;
		}

		bgRect = {0, 0, win_w, win_h};
		transMult = std::min((double)(win_w-padding*2)/sim::bounds.x,
				     (double)(win_h-padding*2)/sim::bounds.y );
		blobRect.w = (int)(sim::bounds.x*transMult);
		blobRect.h = (int)(sim::bounds.y*transMult);
		blobRect.x = (win_w-blobRect.w) / 2;
		blobRect.y = (win_h-blobRect.h) / 2;

		SDL_Surface *bgSurf =
			SDL_CreateRGBSurface(0, win_w, win_h, 32,
			                     rmask, gmask, bmask, amask );
		if (bgSurf == NULL) {
			cout << "SDL_CreateRGBSurface Error: " <<
				SDL_GetError() << endl;
			quit();
			return 1;
		}
		if (SDL_FillRect(bgSurf, &bgRect, 0xffffffff) != 0) {
			cout << "SDL_FillRect Error: " <<
				SDL_GetError() << endl;
			quit();
			return 1;
		}

		Uint32 format = SDL_GetWindowPixelFormat(win);
		if (format == SDL_PIXELFORMAT_UNKNOWN) {
			cout << "SDL_GetWindowPixelFormat Error: " <<
				SDL_GetError() << endl;
			quit();
			return 1;
		}
		bgTex = SDL_CreateTextureFromSurface(ren, bgSurf);
		SDL_FreeSurface(bgSurf);
		if (bgTex == NULL) {
			cout << "SDL_CreateTexture Error: " <<
				SDL_GetError() << endl;
		        quit();
			return 1;
		}
	}

	void quit(void)
	{
		if (!SDL_WasInit(SDL_INIT_VIDEO)) return;
		if (bgTex != NULL) SDL_DestroyTexture(bgTex);
		if (ren != NULL) SDL_DestroyRenderer(ren);
		if (win != NULL) SDL_DestroyWindow(win);
	        SDL_Quit();
	}

	Uint32 getRandBlobColor(void)
	{
		return blobColors[getRand(blobColors_size)];
	}

	void draw(void)
	{
		SDL_RenderClear(ren);

		/* Draw background */
		SDL_RenderCopy(ren, bgTex, NULL, &bgRect);

		/* Draw borders */
		SDL_RenderDrawLine(ren,
				   blobRect.x-1,
				   blobRect.y-1,
				   blobRect.x-1,
				   blobRect.y+blobRect.h-1 );
		SDL_RenderDrawLine(ren,
				   blobRect.x+blobRect.w-1,
				   blobRect.y-1,
				   blobRect.x+blobRect.w-1,
				   blobRect.y+blobRect.h-1 );
		SDL_RenderDrawLine(ren,
				   blobRect.x-1,
				   blobRect.y-1,
				   blobRect.x+blobRect.w-1,
				   blobRect.y-1 );
		SDL_RenderDrawLine(ren,
				   blobRect.x-1,
				   blobRect.y+blobRect.h-1,
				   blobRect.x+blobRect.w-1,
				   blobRect.y+blobRect.h-1 );

		/* Draw blobs and food */
		SDL_Surface *blobSurf =
			SDL_CreateRGBSurface(0, blobRect.w, blobRect.h, 32,
					     rmask, gmask, bmask, amask );
		if (blobSurf == NULL) {
			cout << "SDL_CreateRGBSurface Error: " <<
				SDL_GetError() << endl;
		} else {
			SDL_Rect curRect;
			/* Draw each food pellet to the surface individually */
			for (std::vector<Food>::iterator it = sim::food.begin();
			     it != sim::food.end(); ++it) {
				curRect.x = (int)(it->pos.x * transMult);
				curRect.y = (int)(it->pos.y * transMult);
				curRect.w = std::max(1, (int)(it->size*transMult));
				curRect.h = std::max(1, (int)(it->size*transMult));
				SDL_FillRect(blobSurf, &curRect, colors::red);
			}
			/* Draw each blob to the surface individually */
			for (std::vector<Blob>::iterator it = sim::pop.begin();
			     it != sim::pop.end(); ++it) {
				curRect.x = (int)(it->pos.x * transMult);
				curRect.y = (int)(it->pos.y * transMult);
				curRect.w = std::max(1, (int)(it->size*transMult));
				curRect.h = std::max(1, (int)(it->size*transMult));
				SDL_FillRect(blobSurf, &curRect, it->color);
			}
			SDL_Texture *blobTex =
				SDL_CreateTextureFromSurface(ren, blobSurf);
			SDL_FreeSurface(blobSurf);
			if (blobTex == NULL) {
				cout << "SDL_CreateTextureFromSurface Error: " <<
					SDL_GetError() << endl;
			} else {
				SDL_RenderCopy(ren, blobTex, NULL, &blobRect);
				SDL_DestroyTexture(blobTex);
			}
		}

		/* Update screen */
		SDL_RenderPresent(ren);
	}
}

/* This is here because it messes up Emacs's
   (my text editor) auto-indentation feature,
   which is extremely annoying.
*/
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
namespace visuals {
	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
	namespace colors {
		Uint32 black        = 0x000000ff;
		Uint32 red          = 0xff0000ff;
		Uint32 green        = 0x00ff00ff;
		Uint32 blue         = 0x0000ffff;
		Uint32 redorange    = 0xff4500ff;
		Uint32 orange       = 0xffa500ff;
		Uint32 yelloworange = 0xffcc00ff;
		Uint32 yellowgreen  = 0xadff2fff;
		Uint32 bluegreen    = 0x34ddddff;
		Uint32 blueviolet   = 0x4c50a9ff;
		Uint32 violet       = 0x551a8bff;
		Uint32 redviolet    = 0xf43e71ff;
		Uint32 cyan         = 0x00ffffff;
	}
}
#else
namespace visuals {
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
	namespace colors {
		Uint32 black        = 0xff000000;
		Uint32 red          = 0xff0000ff;
		Uint32 green        = 0xff00ff00;
		Uint32 blue         = 0xffff0000;
		Uint32 redorange    = 0xff0045ff;
		Uint32 orange       = 0xff00a5ff;
		Uint32 yelloworange = 0xff00ccff;
		Uint32 yellowgreen  = 0xff2fffad;
		Uint32 bluegreen    = 0xffdddd34;
		Uint32 blueviolet   = 0xffa9504c;
		Uint32 violet       = 0xff8b1a55;
		Uint32 redviolet    = 0xff713ef4;
		Uint32 cyan         = 0xffffff00;
	}
}
#endif
