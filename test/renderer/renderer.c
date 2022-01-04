// clang renderer.c simple_unicode.c -o ../../build/renderer `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image

#include <SDL.h>
#include <SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "simple_unicode.h"
#include "glyph_layout.h"

FILE *dat;

int main(int argc, char **argv)
{
  if(argc<3){
    printf("args: image layout text\n");
    return 0;
  }

  char *image_file_name = argv[1];
  char *layout_file_name = argv[2];
  const char *text = argv[3];

  fprintf(stderr, "Font: %s\n", image_file_name );
  fprintf(stderr, "Layout: %s\n", layout_file_name );
  fprintf(stderr, "Text: %s\n", text);

  FILE *layout_file = fopen(layout_file_name,"rb");
  if(layout_file == NULL) {
    fprintf(stderr, "Layout: %s Load Failed.\n", layout_file_name );
    return 1;
  }

  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);

  SDL_Surface *font_atlas = IMG_Load( image_file_name );
  if(font_atlas == NULL){
    printf("open fail\n");
    return 0;
  }else{
    printf("%s loaded\n", image_file_name);
  }

  FontHeader header;
  size_t count = fread(&header,sizeof(FontHeader),1,layout_file);
  if(count<1) {
    printf("ERROR. header not found.\n");
    exit(1);
  }
  printf("Header: version:%d, font_size:%d height:%d ascent:%d descent:%d glyph_count:%d\n",
    header.version,
    header.font_size,
    header.height,
    header.ascent,
    header.descent,
    header.glyph_count
  );

  // XXX: memory fuck
  GlyphLayout** layoutTable = malloc(sizeof(GlyphLayout*)*0x110000);
  memset(layoutTable, 0, sizeof(GlyphLayout*)*0x110000);

  for(uint32_t i=0; i<header.glyph_count; i++){
    GlyphLayout *l = malloc(sizeof(GlyphLayout));
    size_t count = fread(l,sizeof(GlyphLayout),1,layout_file);
    if(count < 1) {
      printf("ERROR. layout shortage.\n");
      exit(-1);
    }
    layoutTable[l->codepoint] = l;
  }

  SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(0, 480, 320, 32, SDL_PIXELFORMAT_RGBA32);
  if (s == NULL) {
      SDL_Log("SDL_CreateRGBSurfaceWithFormat() failed: %s", SDL_GetError());
      exit(1);
  }
  SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 128, 0, 0xFF));
  printf("surface created\n");

  size_t textlen = SDL_strlen(text);

  int x = 0;
  int y = header.font_size;
  while (textlen > 0) {
      // printf("textlen %zu.\n", textlen);

    uint32_t cp = utf8_getch_as_codepoint(&text, &textlen);
    if(cp==10) { // LF
      y += header.font_size;
      x = 0;
      continue;
    }

    GlyphLayout *g = layoutTable[cp];
    if(g==NULL){
      printf("codepoint U+%06x not found.\n", cp);
      continue;
    }

    SDL_Rect src_rect = { g->x, g->y, g->w, g->h };

    // printf("Layout: %d,%d,%d,%d\n", src_rect.x, src_rect.y, src_rect.w, src_rect.h);

    if(src_rect.w==0 || src_rect.h==0) {
      printf("layout not found U+%06x\n", cp);
      continue;
    }

    SDL_Rect dst_rect = {
      x - g->base_x,
      y - g->base_y,
      src_rect.w,
      src_rect.h
    };

    SDL_BlitSurface(font_atlas,&src_rect, s,&dst_rect);

    x += g->advance_x;
  }

  IMG_SavePNG(s, "rendered.png");
  fclose(dat);
}
