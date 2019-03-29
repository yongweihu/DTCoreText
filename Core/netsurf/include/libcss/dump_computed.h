#ifndef test_dump_computed_h_
#define test_dump_computed_h_

#include <libcss/computed.h>
#include <libcss/properties.h>
#include <libcss/types.h>

void dump_computed_style(const css_computed_style *style, char *buf,
                         size_t *len);
#endif

