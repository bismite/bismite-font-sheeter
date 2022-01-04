#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "codepoint.h"
#include "simple_unicode.h"

#define USAGE "usage: bismite-codepoint-extractor {--text|--font} <infile> <outfile>\n"

int main(int argc, char **argv)
{
  if(argc!=4){
    printf(USAGE);
    return 1;
  }

  const char* mode = argv[1];
  const char* infilename = argv[2];
  const char* outfilename = argv[3];

  if( strcmp(mode,"--text") == 0 ){
    FILE *infile = fopen(infilename,"rb");
    fseek(infile, 0, SEEK_END);
    size_t text_len = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    char *buffer = malloc(text_len);
    if( fread(buffer, 1, text_len, infile) != text_len ){
      fprintf(stderr, "%s read failed\n",infilename);
      exit(1);
    }
    fclose(infile);
    FILE *outfile = fopen(outfilename,"wb");
    while (text_len > 0) {
      uint32_t cp = utf8_getch_as_codepoint(&buffer, &text_len);
      fwrite(&cp,sizeof(uint32_t),1,outfile);
    }
    fclose(outfile);
  } else if( strcmp(mode,"--font") == 0 ) {
    FT_Library library;
    FT_Face face;
    FT_Init_FreeType(&library);
    FT_New_Face(library, infilename, 0, &face);
    uint32_t size;
    uint32_t *buf;
    buf = codepoint_from_face(face,&size);
    FT_Done_Face(face);
    FT_Done_FreeType(library);

    FILE *outfile = fopen(outfilename,"wb");
    fwrite(buf, sizeof(uint32_t), size, outfile);
    fclose(outfile);
  } else {
    printf(USAGE);
    return 1;
  }

  return 0;
}
