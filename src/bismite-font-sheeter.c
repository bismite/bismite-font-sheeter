#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <cairo-ft.h>

#include "glyph_layout.h"
#include "simple_unicode.h"
#include "codepoint.h"


typedef struct {
  const char* codepoint_file_name;
  const char* font_file_name;
  int font_size;
  float outline;
  //
  uint32_t codepoint_num;
  uint32_t *codepoints;
  //
  FT_Face ft_face;
  cairo_font_face_t *cairo_face;
} font_data;

struct _GlyphLayoutWrapper;
typedef struct _GlyphLayoutWrapper GlyphLayoutWrapper;

typedef struct {
  const char* dat_file_name;
  int font_num;
  font_data *fonts;
  FILE *fp;
  GlyphLayoutWrapper* layouts[0x200000]; // XXX: 0x200000 * 8bytes = 17MByte
  FontHeader header;
} layout_data_file;


struct _GlyphLayoutWrapper {
  GlyphLayout layout;
  layout_data_file* data;
  font_data *font;
  uint32_t codepoint;
  cairo_text_extents_t extents;
  cairo_glyph_t glyph;
  float outline;
  int outline_margin;
};

struct {
  const char *image_name;
  int texture_size;
  layout_data_file* data_files;
  int data_num;
} settings;

void parse_args(int argc, char** argv);

static int compare_glyph_height(const void* _a, const void* _b)
{
  const GlyphLayoutWrapper* a = *(const GlyphLayoutWrapper**)_a;
  const GlyphLayoutWrapper* b = *(const GlyphLayoutWrapper**)_b;
  return b->layout.h - a->layout.h;
}

void load_fonts( cairo_t *cr, FT_Library library, layout_data_file *d )
{
  printf("---- load_fonts %s ----\n", d->dat_file_name);

  // header
  d->header.version = 2;
  // d->header.font_size = d->font_size;
  // d->header.height = font_extents.height;
  // d->header.ascent = font_extents.ascent;
  // d->header.descent = font_extents.descent;
  d->header.glyph_count = 0;

  // load glyphs
  cairo_font_extents_t font_extents;
  int y_baseline = 0;
  int highest_y_bearing = 0;
  for( int j=0; j < d->font_num; j++ ) {
    font_data* f = &d->fonts[j];
    if( FT_New_Face(library, f->font_file_name, 0, &f->ft_face ) != 0 ){
      printf("error FT_New_Face\n");
      exit(1);
    }

    // load codepoint file
    f->codepoint_num = 0;
    if( strcmp(f->codepoint_file_name,"-")==0 ) {
      f->codepoints = codepoint_from_face(f->ft_face, &f->codepoint_num);
    }else{
      f->codepoints = codepoint_file_read(f->codepoint_file_name, &f->codepoint_num);
    }

    f->cairo_face = cairo_ft_font_face_create_for_ft_face(f->ft_face, FT_LOAD_COLOR );

    cairo_set_font_face(cr, f->cairo_face);
    cairo_set_font_size(cr, f->font_size);
    cairo_font_extents (cr, &font_extents );

    for( int k=0; k < f->codepoint_num; k++ ) {
      uint32_t cp = f->codepoints[k];

      if( cp >= 0x200000 ) {
        printf("invalid codepoint");
        continue;
      }

      if( d->layouts[cp] != NULL ) {
        // printf("codepoint %x already loaded.\n", cp);
        continue;
      }

      uint32_t glyph_index = FT_Get_Char_Index(f->ft_face, cp);
      if( glyph_index == 0 ) {
        // printf("Codepoint: %x not found in %s\n", cp, f->font_file_name);
        continue;
      }

      cairo_text_extents_t extents;
      cairo_glyph_t glyph;
      glyph.index = glyph_index;
      cairo_glyph_extents(cr, &glyph, 1, &extents);

      if( extents.x_advance == 0 &&
          extents.y_advance == 0 &&
          extents.width == 0 &&
          extents.height == 0
      ) {
        printf("empty glyph cp:%x font:%s\n",cp,f->font_file_name);
        continue;
      }

      GlyphLayoutWrapper *l = malloc( sizeof(GlyphLayoutWrapper) );
      d->layouts[cp] = l;
      d->header.glyph_count++;

      // layout
      l->font = f;
      l->data = d;
      l->codepoint = cp;
      l->extents = extents;
      l->glyph = glyph;
      l->outline = f->outline;
      l->outline_margin = ceil(f->outline/2.0);
      l->layout.w = ceil(extents.width+l->outline_margin*2);
      l->layout.h = ceil(extents.height+l->outline_margin*2);
      l->layout.advance_x = ceil(extents.x_advance);
      l->layout.advance_y = ceil(extents.y_advance);
      l->layout.codepoint = cp;
      l->layout.reserved = 0;

      if( -extents.y_bearing > highest_y_bearing ) {
        highest_y_bearing = -extents.y_bearing;
      }

      if( ceil(extents.height + extents.y_bearing) > y_baseline ){
        y_baseline = ceil(extents.height + extents.y_bearing);
      }

      l++;
    }

    printf("Size:%d Font:%s(CP:%d) Height:%.1f Asc:%.1f Desc:%.1f max_x_advance:%.1f max_y_advance:%.1f\n",
      f->font_size,
      f->font_file_name, f->codepoint_num,
      font_extents.height, font_extents.ascent, font_extents.descent,
      font_extents.max_x_advance, font_extents.max_y_advance
    );

    for( int k=0; k < f->codepoint_num; k++ ) {
      uint32_t cp = f->codepoints[k];
      if( d->layouts[cp] == NULL ) continue;
      d->layouts[cp]->layout.base_y = y_baseline;
    }
  }

  d->header.font_size = highest_y_bearing + y_baseline;
  d->header.ascent = d->header.font_size - y_baseline;
  d->header.descent = y_baseline;
  printf("  - total glyphs %d font_size:%d descent:%d\n", d->header.glyph_count, d->header.font_size, d->header.descent);
}

void parse_args(int argc, char** argv)
{
  printf("----\n");
  settings.image_name = argv[1];
  settings.texture_size = atoi(argv[2]);
  settings.data_num = atoi(argv[3]);
  settings.data_files = malloc( sizeof(layout_data_file) * settings.data_num );
  printf("Image:%s size:%d data-number:%d\n", settings.image_name, settings.texture_size, settings.data_num);

  int i=4;
  layout_data_file* l = settings.data_files;
  while(i<argc) {
    l->dat_file_name = argv[i+0];
    l->font_num = atoi(argv[i+1]);
    l->fonts = malloc( sizeof(font_data) * l->font_num );
    memset(l->layouts, 0, 0x110000);
    printf("  Dat:%s Fonts:%d\n", l->dat_file_name, l->font_num);

    i += 2;
    font_data* f = l->fonts;
    for( int j=0; j < l->font_num; j++ ) {
      f->codepoint_file_name = argv[i+j*4+0];
      f->font_file_name = argv[i+j*4+1];
      f->font_size = atof(argv[i+j*4+2]);
      f->outline = atof(argv[i+j*4+3]);
      printf("  - CP:%s Font:%s FontSize:%d Outline:%.2f\n",
        f->codepoint_file_name, f->font_file_name, f->font_size, f->outline );

      f++;
    }
    i += l->font_num * 4;
    l++;
  }
}

int main(int argc, char **argv)
{
  const int MARGIN_X = 2;
  const int MARGIN_Y = 2;

  printf("start argc %d\n",argc);

  if( argc<9 ){
    printf( "argv shortage\n");
    return 1;
  }
  if( (argc-4)%2 != 0 ) {
    printf( "argv invlid num\n");
    return 1;
  }

  parse_args(argc, argv);

  FT_Library library;
  FT_Init_FreeType(&library);
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, settings.texture_size, settings.texture_size);
  cairo_t *cr = cairo_create(surface);

  for( int i=0; i < settings.data_num; i++ ) {
    load_fonts(cr, library, &settings.data_files[i] );
  }

  printf("----\n");
  uint32_t total_layouts_num = 0;
  for( int i=0; i < settings.data_num; i++ ) {
    total_layouts_num += settings.data_files[i].header.glyph_count;
  }
  GlyphLayoutWrapper** layouts = malloc( sizeof(GlyphLayoutWrapper*) * total_layouts_num );
  printf("total_layouts_num: %d\n",total_layouts_num);

  // sort
  GlyphLayoutWrapper** tmp = layouts;
  for( int i=0; i < settings.data_num; i++ ) {
    for( uint32_t cp=0; cp < 0x200000; cp++ ) {
      GlyphLayoutWrapper *l = settings.data_files[i].layouts[cp];
      if( l != NULL ) {
        *tmp = l;
        tmp++;
      }
    }
  }
  qsort( layouts, total_layouts_num, sizeof(GlyphLayoutWrapper*), compare_glyph_height );

  //
  // rendering
  //
  int bx = 0;
  int by = 0;
  int line_height = 0;
  for(uint32_t i=0; i<total_layouts_num; i++) {
    GlyphLayout *l = &layouts[i]->layout;
    font_data* font = layouts[i]->font;
    cairo_text_extents_t extents = layouts[i]->extents;
    cairo_glyph_t glyph = layouts[i]->glyph;
    float outline = layouts[i]->outline;
    int outline_margin = layouts[i]->outline_margin;

    cairo_set_font_face(cr, font->cairo_face);
    cairo_set_font_size(cr, font->font_size);

    // overflow, goto next line
    if(bx + l->w > settings.texture_size) {
      bx = 0;
      by += line_height + MARGIN_Y;
      line_height = l->h;
    }

    l->x = bx;
    l->y = by;
    l->base_x = extents.x_bearing - outline_margin;
    l->base_y = - (extents.height + extents.y_bearing);

    //
    // draw glyph
    //
    glyph.x = bx + outline_margin - extents.x_bearing;
    glyph.y = by + outline_margin - extents.y_bearing;
    if( outline > 0 ) {
      cairo_glyph_path(cr, &glyph, 1);
      cairo_set_source_rgba (cr, 1,1,1,1);
      cairo_fill_preserve(cr);
      cairo_set_source_rgba (cr, 0,0,0,1);
      cairo_set_line_width(cr,outline);
      cairo_stroke(cr);
    } else {
      cairo_set_source_rgba (cr, 1,1,1,1);
      cairo_show_glyphs(cr,&glyph,1); // good for color emoji
    }

    char utf8[5] = {0};
    codepoint_to_utf8_string(layouts[i]->layout.codepoint,utf8);
    printf("[%s%s](%04x) %d[pt] wh:(%f,%f) bearing:(%.1f,%.1f) advance:(%.1f,%.1f) base(%d,%d) gxy:(%.1f,%.1f) img:(%d,%d,%d,%d)\n",
      l->codepoint <= 0xFF ? " " : "", utf8, l->codepoint,
      font->font_size,
      extents.width, extents.height,
      extents.x_bearing, extents.y_bearing,
      extents.x_advance, extents.y_advance,
      l->base_x, l->base_y,
      glyph.x, glyph.y,
      l->x,l->y,l->w,l->h
    );

    // advance
    if(l->h > line_height) line_height = l->h;
    bx += l->w + MARGIN_X;
  }

  // clipping and save image
  cairo_surface_t *tmp_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, settings.texture_size, settings.texture_size );
  cairo_t *tmp_cr = cairo_create(tmp_surface);

  cairo_set_source_surface (tmp_cr, surface, 0,0);
  cairo_paint (tmp_cr);

  cairo_status_t status = cairo_surface_write_to_png(tmp_surface, settings.image_name);
  if(status==CAIRO_STATUS_SUCCESS){
    printf("%s save.\n", settings.image_name);
  }else{
    printf("FAILED: %s\n", settings.image_name);
  }

  cairo_destroy(tmp_cr);
  cairo_surface_destroy(tmp_surface);

  // write layout data files
  for(int i=0; i<settings.data_num; i++) {
    layout_data_file *d = &settings.data_files[i];
    d->fp = fopen(d->dat_file_name,"w");
    fwrite(&(d->header),sizeof(FontHeader),1,d->fp);
  }
  for(uint32_t i=0; i<total_layouts_num; i++) {
    fwrite(&(layouts[i]->layout),sizeof(GlyphLayout),1,layouts[i]->data->fp);
  }
  for(int i=0; i<settings.data_num; i++) {
    fclose(settings.data_files[i].fp);
  }

  return(1);
}
