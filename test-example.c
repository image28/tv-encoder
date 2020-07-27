   
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#include <SDL2/SDL.h>
#include <png.h>
#include <SDL2/SDL_image.h>


#define WIDTH 800
#define HEIGHT 600


SDL_Event event; 

const int FRAMES_PER_SECOND = 12;

int main(int argc, char *argv[])
{
   int start=0;
   int frame=0;
   int done = 0;
   int ticks=0;
   int filesize=0;
   int pos=0;
   char outfile[32768];
   FILE *fp;
   Uint32 *pixels;
   SDL_Window *sdlWindow;
   SDL_Renderer *sdlRenderer;
   SDL_Surface *screen;
   SDL_Surface *surface;
   SDL_Texture *sdlTexture;
   Uint8 *map;
   int one=1;
	

   
   SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &sdlWindow, &sdlRenderer);
   sprintf(outfile, "output-%d.png",frame);

    sdlTexture = SDL_CreateTexture(sdlRenderer,
                               SDL_PIXELFORMAT_RGB888,
                               SDL_TEXTUREACCESS_STREAMING,
                               WIDTH, HEIGHT);

    surface=IMG_Load(argv[1]);
    map=(Uint8 *)surface->pixels;

   while ( ! done )
   {

        start=SDL_GetTicks();
      
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN: done =1; break;
                case SDL_QUIT: done = 1; break;
            }
        }

        SDL_RenderClear(sdlRenderer);
        SDL_UpdateTexture(sdlTexture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);
        frame++;

        if ( ( (SDL_GetTicks()-start) < 1000 / FRAMES_PER_SECOND ) )
        {
            //Sleep the remaining frame time
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - (SDL_GetTicks()-start) );
        }
   }

   // Return screen to text mode.
  
    SDL_FreeSurface(surface);
   
   SDL_Quit();
   
   return EXIT_SUCCESS;
}

