#ifndef test_dump_h_
#define test_dump_h_

#include <libcss/types.h>

extern const char *opcode_names[];
void dump_sheet(css_stylesheet *sheet, char *buf, size_t *len);

#endif
