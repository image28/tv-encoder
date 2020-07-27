
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include <SDL2/SDL.h>
#include <png.h>
#include <SDL2/SDL_image.h>

#define WIDTH 800
#define HEIGHT 600

SDL_Event event; 

const int FRAMES_PER_SECOND = 30;

int main(int argc, char *argv[])
{
   int start=0;
   int frame=0;
   int done = 0;
   int ticks=0;
   int pos=0;
   int padding=0;
   u_int64_t filesize=0;
   u_int64_t segments=0;
   u_int64_t remainder=0;

   char outfile[32768];
   FILE *fp;
   Uint32 *pixels;
   SDL_Window *sdlWindow;
   SDL_Renderer *sdlRenderer;
   SDL_Surface *screen;
   SDL_Surface *surface;
   SDL_Texture *sdlTexture;
   union {
      int8_t *a;
      u_int64_t *b;
   }map;
   
   u_int64_t *fspointer;
   u_int64_t *segpointer;
   int i=0;
   u_int64_t segsize=(WIDTH*HEIGHT*3/8);

   fp=fopen(argv[1],"a+");
   //filesize=ftell(fp);

   SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &sdlWindow, &sdlRenderer);
   sprintf(outfile, "output-%d.png",frame);

      sdlTexture = SDL_CreateTexture(sdlRenderer,
                               SDL_PIXELFORMAT_RGB888,
                               SDL_TEXTUREACCESS_STREAMING,
                               WIDTH, HEIGHT);

   //map = malloc(WIDTH*HEIGHT*3);
   struct stat buffer;   
  
   while ( stat (outfile, &buffer) == 0 )
   {
      surface=IMG_Load(outfile);

      map.a=surface->pixels;

      filesize=*(map.b+(segsize-1));
      segments=*(map.b+(segsize-2));
      printf("%llu %llu\n",filesize,segments);

		if( SDL_MUSTLOCK( surface ) ) 
		{ //Lock the surface 
			SDL_LockSurface( surface ); 
		} 
      start=SDL_GetTicks();

      SDL_RenderClear(sdlRenderer);
      SDL_UpdateTexture(sdlTexture, NULL, surface->pixels, surface->pitch);
      SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
      SDL_RenderPresent(sdlRenderer);

      while (SDL_PollEvent(&event))
      {
         switch(event.type)
         {
            case SDL_KEYDOWN: done =1; break;
            case SDL_QUIT: done = 1; break;
         }
      }

      pos=pos+WIDTH*HEIGHT*3;
      
      if ( segments-frame == 0 )
         fwrite((Uint8 *)map.a,1,filesize-(segments*((segsize-2)*8)),fp);
      else
         fwrite((Uint8 *)map.a,1,(segsize-2)*8,fp);
      //fseek(fp,0L,SEEK_END);

	   if( SDL_MUSTLOCK( surface ) ) 
		{ //Lock the surface 
			SDL_UnlockSurface( surface ); 
		} 

      SDL_FreeSurface(surface);

      frame++;

      sprintf(outfile, "output-%d.png",frame);
      printf("%s %llu\n", outfile, fseek(fp,0L,SEEK_CUR));

      if ( ( (SDL_GetTicks()-start) < 1000 / FRAMES_PER_SECOND ) )
      {
         //Sleep the remaining frame time
         SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - (SDL_GetTicks()-start) );
      }
   }

   // Return screen to text mode.
  
   fclose(fp);
   
   SDL_Quit();
   
   return EXIT_SUCCESS;
}

