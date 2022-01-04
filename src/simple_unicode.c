#include "simple_unicode.h"

/*
|       UNICODE (UCS2/UCS4)           | UTF8 1st | 2nd byte | 3rd byte | 4th byte |    Valid Range    |
|-------------------------------------|----------|----------|----------|----------|-------------------|
|                            0xxxxxxx | 0xxxxxxx |     -    |     -    |     -    |  U+0000 .. U+007F |
|                   00000xxx-xxyyyyyy | 110xxxxx | 10yyyyyy |     -    |     -    |  U+0080 .. U+07FF |
|                   xxxxyyyy-yyzzzzzz | 1110xxxx | 10yyyyyy | 10zzzzzz |     -    |  U+0800 .. U+FFFF |
| 00000000-000wwwxx xxxxyyyy-yyzzzzzz | 11110www | 10xxxxxx | 10yyyyyy | 10zzzzzz | U+10000..U+10FFFF |

 - code point U+D800..U+DFFF are invalid. these are for surrogate pair.
 - code point upper U+110000 are invalid.
 - overlong UTF-8 is invalid. ASCII must 1byte, U+0080..U+07FF must 2bytes.

*/

#define valid_utf8_4byte(a,b,c,d) ( \
  (a & 0b11111000) == 0b11110000 && \
  (b & 0b11000000) == 0b10000000 && \
  (c & 0b11000000) == 0b10000000 && \
  (d & 0b11000000) == 0b10000000 )

#define valid_utf8_3byte(a,b,c) ( \
  (a & 0b11110000) == 0b11100000 && \
  (b & 0b11000000) == 0b10000000 && \
  (c & 0b11000000) == 0b10000000 )

#define valid_utf8_2byte(a,b) ( \
  (a & 0b11100000) == 0b11000000 && \
  (b & 0b11000000) == 0b10000000 )

static uint32_t __utf8_to_codepoint__(uint8_t a,uint8_t b,uint8_t c,uint8_t d)
{
    if(a==0) {
      a = b;
      b = c;
      c = d;
      d = 0;
    }
    if(a==0) {
      a = b;
      b = c;
      c = 0;
      d = 0;
    }
    if(a==0) {
      a = b;
      b = 0;
      c = 0;
      d = 0;
    }

    if( a < 0x80 ) { // ASCII
      return a;
    }

    if( valid_utf8_2byte(a,b) ) {
      uint8_t h = (a & 0b00011111) >> 2;
      uint8_t l = ((a & 0b00011111) << 6) | ( b & 0b00111111 );
      return h<<8 | l;
    }

    if( valid_utf8_3byte(a,b,c) ) {
      uint8_t h = ((a & 0b00001111) << 4 & 0b11110000) | ((b & 0b00111111) >> 2 & 0b00001111);
      uint8_t l = ((b & 0b00111111) << 6 & 0b11000000) | ((c & 0b00111111) >> 0 & 0b00111111);
      return h<<8 | l;
    }

    if( valid_utf8_4byte(a,b,c,d) ) {
      uint8_t h = ((a & 0b00000111) << 2 & 0b00011100) | ((b & 0b00110000) >> 4 & 0b00000011);
      uint8_t m = ((b & 0b00001111) << 4 & 0b11110000) | ((c & 0b00111100) >> 2 & 0b00001111);
      uint8_t l = ((c & 0b00000011) << 6 & 0b11000000) | ((d & 0b00111111) >> 0 & 0b00111111);
      return h<<16 | m<<8 | l;
    }

    return 0xFFFD; // 'REPLACEMENT CHARACTER' (U+FFFD)
}


uint32_t utf8_getch_as_codepoint( char **txt, size_t *len)
{
  static const uint32_t REPLACEMENT_CHARACTER = 0xFFFD;
  const uint8_t *p = *(const uint8_t**)txt;

  // ASCII
  if( p[0] < 0x80 ) {
    *len -= 1;
    *txt += 1;
    return p[0];
  }

  // invalid 5bytes and 6bytes UTF8
  if( p[0] > 0b11111000 ) {
    *len -= 1;
    *txt += 1;
    return REPLACEMENT_CHARACTER;
  }

  // 4bytes UTF8
  if( *len >= 4 && valid_utf8_4byte(p[0],p[1],p[2],p[3]) ) {
    *len -= 4;
    *txt += 4;
    return __utf8_to_codepoint__(p[0],p[1],p[2],p[3]);
  }
  // 3bytes UTF8
  else if( *len >= 3 && valid_utf8_3byte(p[0],p[1],p[2]) ) {
    *len -= 3;
    *txt += 3;
    return __utf8_to_codepoint__(p[0],p[1],p[2],0);
  }
  // 2bytes UTF8
  else if( *len >= 2 && valid_utf8_2byte(p[0],p[1]) ) {
    *len -= 2;
    *txt += 2;
    return __utf8_to_codepoint__(p[0],p[1],0,0);
  }

  *len -= 1;
  *txt += 1;
  return REPLACEMENT_CHARACTER;
}

void codepoint_to_utf8_string(uint32_t cp, char *dst)
{
  if (cp < 0x80) {
    dst[0] = cp & 0b01111111;
    dst[1] = 0;
  } else if (cp < 0x0800) { // 2Bytes
    dst[0] = (cp >> 6 & 0b00011111) | 0b11000000;
    dst[1] = (cp      & 0b00111111) | 0b10000000;
    dst[2] = 0;
  } else if (cp < 0x10000) { // 3Bytes
    dst[0] = (cp >> 12 & 0b00001111) | 0b11100000;
    dst[1] = (cp >>  6 & 0b00111111) | 0b10000000;
    dst[2] = (cp       & 0b00111111) | 0b10000000;
    dst[3] = 0;
  } else if (cp < 0x110000) { // 4Bytes
    dst[0] = (cp >> 18 & 0b00000111) | 0b11110000;
    dst[1] = (cp >> 12 & 0b00111111) | 0b10000000;
    dst[2] = (cp >>  6 & 0b00111111) | 0b10000000;
    dst[3] = (cp       & 0b00111111) | 0b10000000;
    dst[4] = 0;
  } else {
    dst[0] = 0;
  }
}
