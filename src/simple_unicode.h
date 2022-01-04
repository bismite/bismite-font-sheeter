#ifndef __SIMPLE_UNICODE_H__
#define __SIMPLE_UNICODE_H__

#include <stdint.h>
#include <stdlib.h>

extern uint32_t utf8_getch_as_codepoint( char **src, size_t *srclen);
extern void codepoint_to_utf8_string(uint32_t cp, char *dst);

#endif
