// TCap v0.1
// (C) Thomas Hargrove
// http://toonarchive.com/tcap

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
union{
	int8_t *a;
	u_int64_t *b;
}map;

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Surface *screen;
SDL_Surface *offscreen;
SDL_Texture *sdlTexture;
SDL_Event event; 

const int FRAMES_PER_SECOND = 12;

int png_save_surface(char *filename, SDL_Surface *surf);
void png_user_error(png_structp ctx, png_const_charp str);
void png_user_warn(png_structp ctx, png_const_charp str);
static int png_colortype_from_surface(SDL_Surface *surface);

/* Copies map to screen, then updates the screen */
void copytoscreen(char* tmap) 
{
	offscreen = SDL_CreateRGBSurfaceFrom((void *) tmap, WIDTH, HEIGHT, 24, WIDTH*3, 0xFF0000, 0x00FF00, 0x0000FF, 0x000000);
   sdlTexture = SDL_CreateTexture(sdlRenderer,
                               SDL_PIXELFORMAT_RGB888,
                               SDL_TEXTUREACCESS_STREAMING,
                               WIDTH, HEIGHT);

   SDL_RenderClear(sdlRenderer);
   SDL_UpdateTexture(sdlTexture, NULL, tmap, WIDTH*3);
   SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
   SDL_RenderPresent(sdlRenderer);
}


int main(int argc, char *argv[])
{
   int start=0;
   int frame=0;
   int done = 0;
   int ticks=0;

   typedef union conv {
   	u_int64_t a;
	u_int8_t b[8];
   };

   	union conv segments;
	union conv filesize;


   int pos=0;
   FILE *fp;
   char outfile[32768];
	
   if ((fp=fopen(argv[1],"r")) == NULL)  printf("File open failed"); exit;	 
   fseek(fp,0L,SEEK_END);
   filesize.a=ftell(fp);
   fseek(fp,0L,SEEK_SET);
   segments.a=filesize.a/(WIDTH*HEIGHT*3+16);


   SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &sdlWindow, &sdlRenderer);

   map.a = malloc(WIDTH*HEIGHT*3);
   
   printf("%llu %llu\n", filesize.a, segments.a);
   
   while ( pos < filesize.a )
   {
      start=SDL_GetTicks();

      fread(map.a, WIDTH*HEIGHT*3-16, 1, fp);
	  memmove((map.a+(WIDTH*HEIGHT*3-8)),&filesize,8);
	  memmove((map.a+(WIDTH*HEIGHT*3-16)),&segments,8);;
	  copytoscreen(map.a);
  	  printf("%llu %llu\n",(u_int64_t)*(map.b+(WIDTH*HEIGHT*3/8-1)),(u_int64_t)*(map.b+(WIDTH*HEIGHT*3/8-2)));
    

      while (SDL_PollEvent(&event))
      {
         switch(event.type)
         {
            case SDL_KEYDOWN: done =1; break;
            case SDL_QUIT: done = 1; break;
         }
      }

      pos=pos+(WIDTH*HEIGHT*3-16);
      
      sprintf(outfile, "output-%d.png",frame);
		//strcat(outfile,"-2.png");
      if( png_save_surface(outfile, offscreen) < 0 ) exit(-1);

      frame++;
   
	  //free(map);
	  //map='\0';
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

static int png_colortype_from_surface(SDL_Surface *surface)
{
	int colortype = PNG_COLOR_MASK_COLOR; /* grayscale not supported */

	if (surface->format->palette)
		colortype |= PNG_COLOR_MASK_PALETTE;
	else if (surface->format->Amask)
		colortype |= PNG_COLOR_MASK_ALPHA;
		
	return colortype;
}


void png_user_warn(png_structp ctx, png_const_charp str)
{
	fprintf(stderr, "libpng: warning: %s\n", str);
}


void png_user_error(png_structp ctx, png_const_charp str)
{
	fprintf(stderr, "libpng: error: %s\n", str);
}


int png_save_surface(char *filename, SDL_Surface *surf)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	int i, colortype;
	png_bytep *row_pointers;

	/* Opening output file */
	fp = fopen(filename, "wb");
	if (fp == NULL) {
		perror("fopen error");
		return -1;
	}

	/* Initializing png structures and callbacks */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 
		NULL, png_user_error, png_user_warn);
	if (png_ptr == NULL) {
		printf("png_create_write_struct error!\n");
		return -1;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		printf("png_create_info_struct error!\n");
		exit(-1);
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		exit(-1);
	}

	png_init_io(png_ptr, fp);

	colortype = png_colortype_from_surface(surf);
	png_set_IHDR(png_ptr, info_ptr, surf->w, surf->h, 8, colortype,	PNG_INTERLACE_NONE, 
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	/* Writing the image */
	png_write_info(png_ptr, info_ptr);
	png_set_packing(png_ptr);

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*surf->h);
	for (i = 0; i < surf->h; i++)
		row_pointers[i] = (png_bytep)(Uint8 *)surf->pixels + i*surf->pitch;
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, info_ptr);

	/* Cleaning out... */
	free(row_pointers);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);

	return 0;
}

