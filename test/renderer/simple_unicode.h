#ifndef __BISMITE_EXT_SIMPLE_UNICODE_H__
#define __BISMITE_EXT_SIMPLE_UNICODE_H__

#include <stdint.h>
#include <stdlib.h>

extern uint32_t utf8_getch_as_codepoint(const char **txt, size_t *len);

#endif
