#ifndef __CODEPOINT_H__
#define __CODEPOINT_H__

extern uint32_t* codepoint_from_face(FT_Face face, uint32_t *size);
extern uint32_t* codepoint_file_read(const char *filename, uint32_t *codepoint_num);

#endif
