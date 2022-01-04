#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

uint32_t* codepoint_from_face(FT_Face face, uint32_t *size)
{
  uint32_t *table;
  uint32_t *buffer;

  table = malloc(0x200000 * sizeof(uint32_t));
  memset(table,0,0x200000 * sizeof(uint32_t));

  *size = 0;
  FT_UInt gindex = 0;
  uint32_t cp = 0;
  cp = FT_Get_First_Char( face, &gindex );
  while ( gindex != 0 ) {
    if(cp==0 || cp>=0x200000) {
      printf("index %u charcode %x\n",gindex,cp);
    }
    table[cp] = 1; // exist
    (*size)++;
    cp = FT_Get_Next_Char( face, cp, &gindex );
  }

  buffer = malloc( sizeof(uint32_t) * (*size) );
  uint32_t* p = buffer;
  for(uint32_t i=0;i<0x200000;i++) {
    if(table[i] == 1) {
      *p = i;
      p++;
    }
  }

  free(table);
  return buffer;
}

uint32_t* codepoint_file_read(const char *filename, uint32_t *codepoint_num)
{
  FILE *fp;
  uint32_t *buffer;
  fp = fopen(filename, "rb");
  // get size
  fseek(fp, 0, SEEK_END);
  size_t filesize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  // check
  if( filesize % 4 != 0) {
    printf("codepoint file invalid size\n");
    return NULL;
  }
  // read
  *codepoint_num = filesize / 4;
  buffer = (uint32_t*)malloc(filesize);
  size_t size = fread(buffer, sizeof(uint32_t), *codepoint_num, fp);
  if(size != *codepoint_num ) {
    printf("read error\n");
    return NULL;
  }
  fclose(fp);
  return buffer;
}
