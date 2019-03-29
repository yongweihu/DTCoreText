#ifndef test_dump_objc_computed_h_
#define test_dump_objc_computed_h_

#include <libcss/computed.h>
#include <libcss/properties.h>
#include <libcss/types.h>
#import <Foundation/Foundation.h>

NSDictionary * dump_objc_computed_style(const css_computed_style *style);
NSString *dump_objc_css_unit(css_fixed val, css_unit unit);
NSString * dump_objc_css_number(css_fixed val);
NSString* dump_objc_css_fixed(css_fixed f);

#endif

