#include "simple_unicode.h"
#include <stdbool.h>
#include <string.h>

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

#define REPLACEMENT_CHARACTER (0xFFFD)

uint32_t utf8_getch_as_codepoint(const char **txt, size_t *len)
{
  const uint8_t *p = *(const uint8_t**)txt;
  uint32_t ch = REPLACEMENT_CHARACTER;

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

  // 4bytes
  if(p[0] >= 0b11110000) {
    // shortage
    if(*len < 4) {
      *len -= 1;
      return REPLACEMENT_CHARACTER;
    }
    // invalid sequence
    if( (p[1]&0b11000000) != 0b10000000 ||
        (p[2]&0b11000000) != 0b10000000 ||
        (p[3]&0b11000000) != 0b10000000
    ) {
      *len -= 1;
      return REPLACEMENT_CHARACTER;
    }
    ch = (p[0]&0b00000111) << 18 | // www
         (p[1]&0b00111111) << 12 | // xxxxxx
         (p[2]&0b00111111) <<  6 | // yyyyyy
         (p[3]&0b00111111);        // zzzzzz
    // invalid range
    if( ch < 0x10000 || 0x10FFFF < ch ) {
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    *len -= 4;
    *txt += 4;
    return ch;
  }

  // 3bytes
  if(p[0] >= 0b11100000) {
    // shortage
    if(*len < 3) {
      *len -= 1;
      return REPLACEMENT_CHARACTER;
    }
    // invalid sequence
    if( (p[1]&0b11000000) != 0b10000000 ||
        (p[2]&0b11000000) != 0b10000000
    ) {
      *len -= 1;
      return REPLACEMENT_CHARACTER;
    }
    ch = (p[0]&0b00001111) << 12 |
         (p[1]&0b00111111) << 6 |
         (p[2]&0b00111111);
    // invalid range
    if( ch < 0x0800 || 0xFFFF < ch ) {
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    *len -= 3;
    *txt += 3;
    return ch;
  }

  // 2bytes
  if(p[0] >= 0b11000000) {
    // shortage
    if(*len < 2) {
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    // invalid sequence
    if( (p[1]&0b11000000) != 0b10000000 ) {
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    ch  = (p[0]&0b00011111) << 6 |
          (p[1]&0b00111111);
    // invalid range.
    if( ch < 0x0080 || 0x07FF < ch ) {
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    *len -= 2;
    *txt += 2;
    return ch;
  }

  *len -= 1;
  *txt += 1;
  return REPLACEMENT_CHARACTER;
}
