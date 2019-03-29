#ifndef test_dump_objc_h_
#define test_dump_objc_h_

#include <string.h>

#include <libcss/types.h>

#include "stylesheet.h"
#include "bytecode/bytecode.h"
#include "bytecode/opcodes.h"
#include "select/font_face.h"

#include "testutils.h"
#include "dump_computed_objc.h"
#include "dump.h"

static NSDictionary *dump_objc_sheet(css_stylesheet *sheet);
static void dump_objc_rule_selector(css_rule_selector *s, NSMutableString *buf, uint32_t depth);
static void dump_objc_rule_charset(css_rule_charset *s, NSMutableDictionary *buf);
static void dump_objc_rule_import(css_rule_import *s, NSMutableDictionary *buf);
static void dump_objc_rule_media(css_rule_media *s, NSMutableDictionary *buf);
static void dump_objc_rule_page(css_rule_page *s, NSMutableDictionary *buf);
static void dump_objc_rule_font_face(css_rule_font_face *s,
								NSMutableDictionary *buf);
static void dump_objc_selector_list(css_selector *list, NSMutableString* ptr);
static void dump_objc_selector(css_selector *selector, NSMutableString* ptr);
static void dump_objc_selector_detail(css_selector_detail *detail, NSMutableString* ptr);
void dump_objc_bytecode(css_style *style, NSMutableDictionary *resultDict, uint32_t depth);
static void dump_objc_string(lwc_string *string, NSMutableString *ptr);
//static void dump_objc_font_face(css_font_face *font_face, NSMutableDictionary *ptr);

static NSDictionary *dump_objc_sheet(css_stylesheet *sheet)
{
	NSMutableDictionary *result = [NSMutableDictionary new];
	
	css_rule *rule;
	
	for (rule = sheet->rule_list; rule != NULL; rule = rule->next) {
		NSMutableString *selector = [NSMutableString new];
		NSMutableDictionary *ruleDict = [NSMutableDictionary new];
		switch (rule->type) {
			case CSS_RULE_SELECTOR:
			{
				css_rule_selector *s = (css_rule_selector *) rule;
				dump_objc_rule_selector(s, selector, 1);
				if (s->style != NULL)
					dump_objc_bytecode(s->style, ruleDict, 2);
			}
				break;
			case CSS_RULE_CHARSET:
				dump_objc_rule_charset((css_rule_charset *) rule, ruleDict);
				break;
			case CSS_RULE_IMPORT:
				dump_objc_rule_import((css_rule_import *) rule, ruleDict);
				break;
			case CSS_RULE_MEDIA:
				dump_objc_rule_media((css_rule_media *) rule, ruleDict);
				break;
			case CSS_RULE_PAGE:
				dump_objc_rule_page((css_rule_page *) rule, ruleDict);
				break;
			case CSS_RULE_FONT_FACE:
				dump_objc_rule_font_face((css_rule_font_face *) rule, ruleDict);
				break;
			default:
			{
//				[ruleStr appendFormat:@"Unhandled rule type %d", rule->type];
			}
				break;
		}
		
		[result setObject:ruleDict forKey:[selector stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]]];
	}
	
	return result;
}

void dump_objc_rule_selector(css_rule_selector *s, NSMutableString *buf,
						uint32_t depth)
{
	uint32_t i;

	for (i = 0; i < depth; i++)
		 [buf appendString:@" "];
	
	/* Build selector string */
	for (i = 0; i < s->base.items; i++) {
		dump_objc_selector_list(s->selectors[i], buf);
		if (i != (uint32_t) (s->base.items - 1)) {
			[buf appendString:@", "];
		}
	}
}

void dump_objc_rule_charset(css_rule_charset *s, NSMutableDictionary *buf)
{
//	[buf appendString:@"| @charset("];
//	dump_objc_string(s->encoding, buf);
//	[buf appendString:@")"];
}

void dump_objc_rule_import(css_rule_import *s, NSMutableDictionary *buf)
{
//	[buf appendFormat:@"| @import url(\"%.*s\")",
//	 (int) lwc_string_length(s->url), lwc_string_data(s->url)];
}

void dump_objc_rule_media(css_rule_media *s, NSMutableDictionary *buf)
{
//	css_rule *rule;
	
//	[buf appendString:@"| @media "];
	
	/* \todo media list */
	
//	for (rule = s->first_child; rule != NULL; rule = rule->next) {
//		css_rule_selector *s = (css_rule_selector *) rule;
//		dump_objc_rule_selector(s, buf, 2);
//		if (s->style != NULL)
//			dump_objc_bytecode(s->style, buf, 3);
//	}
}

void dump_objc_rule_page(css_rule_page *s, NSMutableDictionary *buf)
{
//	[buf appendString:@"| @page "];
//
//	if (s->selector != NULL)
//		dump_objc_selector_list(s->selector, buf);
//
//	if (s->style != NULL)
//		dump_objc_bytecode(s->style, buf, 2);
}

void dump_objc_rule_font_face(css_rule_font_face *s, NSMutableDictionary *buf)
{
//	[buf appendString:@"| @font-face "];
//
//	if (s->font_face != NULL) {
//		dump_objc_font_face(s->font_face, buf);
//	}
}

void dump_objc_selector_list(css_selector *list, NSMutableString *ptr)
{
	if (list->combinator != NULL) {
		dump_objc_selector_list(list->combinator, ptr);
	}
	
	switch (list->data.comb) {
		case CSS_COMBINATOR_NONE:
			break;
		case CSS_COMBINATOR_ANCESTOR:
			[ptr appendString:@" "];
			break;
		case CSS_COMBINATOR_PARENT:
			[ptr appendString:@" > "];
			break;
		case CSS_COMBINATOR_SIBLING:
			[ptr appendString:@" + "];
			break;
		case CSS_COMBINATOR_GENERIC_SIBLING:
			[ptr appendString:@" + "];
			break;
	}
	
	dump_objc_selector(list, ptr);
}

void dump_objc_selector(css_selector *selector, NSMutableString *ptr)
{
	css_selector_detail *d = &selector->data;
	
	while (true) {
		dump_objc_selector_detail(d, ptr);
		
		if (d->next == 0)
			break;
		
		d++;
	}
}

void dump_objc_selector_detail(css_selector_detail *detail, NSMutableString *ptr)
{
	if (detail->negate)
		[ptr appendString:@":not("];
	
	switch (detail->type) {
		case CSS_SELECTOR_ELEMENT:
			if (lwc_string_length(detail->qname.name) == 1 &&
				lwc_string_data(detail->qname.name)[0] == '*' &&
				detail->next == 0) {
				dump_objc_string(detail->qname.name, ptr);
			} else if (lwc_string_length(detail->qname.name) != 1 ||
					   lwc_string_data(detail->qname.name)[0] != '*') {
				dump_objc_string(detail->qname.name, ptr);
			}
			break;
		case CSS_SELECTOR_CLASS:
			[ptr appendString:@"."];
			dump_objc_string(detail->qname.name, ptr);
			break;
		case CSS_SELECTOR_ID:
			[ptr appendString:@"#"];
			dump_objc_string(detail->qname.name, ptr);
			break;
		case CSS_SELECTOR_PSEUDO_CLASS:
		case CSS_SELECTOR_PSEUDO_ELEMENT:
			[ptr appendString:@":"];
			dump_objc_string(detail->qname.name, ptr);
			if (detail->value_type == CSS_SELECTOR_DETAIL_VALUE_STRING) {
				if (detail->value.string != NULL) {
					[ptr appendString:@"("];
					dump_objc_string(detail->value.string, ptr);
					[ptr appendString:@")"];
				}
			} else {
				[ptr appendFormat:@"(%dn+%d)",
								detail->value.nth.a,
								detail->value.nth.b];
			}
			break;
		case CSS_SELECTOR_ATTRIBUTE:
			[ptr appendString:@"["];
			dump_objc_string(detail->qname.name, ptr);
			[ptr appendString:@"]"];
			break;
		case CSS_SELECTOR_ATTRIBUTE_EQUAL:
			[ptr appendString:@"["];
			dump_objc_string(detail->qname.name, ptr);
			[ptr appendString:@"=\""];
			dump_objc_string(detail->value.string, ptr);
			[ptr appendString:@"\"]"];
			break;
		case CSS_SELECTOR_ATTRIBUTE_DASHMATCH:
			[ptr appendString:@"["];
			dump_objc_string(detail->qname.name, ptr);
			[ptr appendString:@"|=\""];
			dump_objc_string(detail->value.string, ptr);
			[ptr appendString:@"\"]"];
			break;
		case CSS_SELECTOR_ATTRIBUTE_INCLUDES:
			[ptr appendString:@"["];
			dump_objc_string(detail->qname.name, ptr);
			[ptr appendString:@"~=\""];
			dump_objc_string(detail->value.string, ptr);
			[ptr appendString:@"\"]"];
			break;
		case CSS_SELECTOR_ATTRIBUTE_PREFIX:
			[ptr appendString:@"["];
			dump_objc_string(detail->qname.name, ptr);
			[ptr appendString:@"^=\""];
			dump_objc_string(detail->value.string, ptr);
			[ptr appendString:@"\"]"];
			break;
		case CSS_SELECTOR_ATTRIBUTE_SUFFIX:
			[ptr appendString:@"["];
			dump_objc_string(detail->qname.name, ptr);
			[ptr appendString:@"$=\""];
			dump_objc_string(detail->value.string, ptr);
			[ptr appendString:@"\"]"];
			break;
		case CSS_SELECTOR_ATTRIBUTE_SUBSTRING:
			[ptr appendString:@"["];
			dump_objc_string(detail->qname.name, ptr);
			[ptr appendString:@"*=\""];
			dump_objc_string(detail->value.string, ptr);
			[ptr appendString:@"\"]"];
			break;
	}
	
	if (detail->negate)
		[ptr appendString:@")"];
}

static NSString * dump_objc_counter(lwc_string *name, uint32_t value)
{
	NSMutableString *result = [NSMutableString new];
	[result appendFormat:@"counter(%.*s",
					(int) lwc_string_length(name), lwc_string_data(name)];
	
	value >>= CONTENT_COUNTER_STYLE_SHIFT;
	
	switch (value) {
		case LIST_STYLE_TYPE_DISC:
			[result appendString:@", disc"];
			break;
		case LIST_STYLE_TYPE_CIRCLE:
			[result appendString:@", circle"];
			break;
		case LIST_STYLE_TYPE_SQUARE:
			[result appendString:@", square"];
			break;
		case LIST_STYLE_TYPE_DECIMAL:
			break;
		case LIST_STYLE_TYPE_DECIMAL_LEADING_ZERO:
			[result appendString:@", decimal-leading-zero"];
			break;
		case LIST_STYLE_TYPE_LOWER_ROMAN:
			[result appendString:@", lower-roman"];
			break;
		case LIST_STYLE_TYPE_UPPER_ROMAN:
			[result appendString:@", upper-roman"];
			break;
		case LIST_STYLE_TYPE_LOWER_GREEK:
			[result appendString:@", lower-greek"];
			break;
		case LIST_STYLE_TYPE_LOWER_LATIN:
			[result appendString:@", lower-latin"];
			break;
		case LIST_STYLE_TYPE_UPPER_LATIN:
			[result appendString:@", upper-latin"];
			break;
		case LIST_STYLE_TYPE_ARMENIAN:
			[result appendString:@", armenian"];
			break;
		case LIST_STYLE_TYPE_GEORGIAN:
			[result appendString:@", georgian"];
			break;
		case LIST_STYLE_TYPE_LOWER_ALPHA:
			[result appendString:@", lower-alpha"];
			break;
		case LIST_STYLE_TYPE_UPPER_ALPHA:
			[result appendString:@", upper-alpha"];
			break;
		case LIST_STYLE_TYPE_NONE:
			[result appendString:@", none"];
			break;
	}
	
	[result appendString:@")"];
	
	return result;
}

static NSString * dump_objc_counters(lwc_string *name, lwc_string *separator,
						  uint32_t value)
{
	NSMutableString *result = [NSMutableString new];
	[result appendFormat:@"counter(%.*s, %.*s",
					(int) lwc_string_length(name),
					lwc_string_data(name),
					(int) lwc_string_length(separator),
					lwc_string_data(separator)];
	
	value >>= CONTENT_COUNTER_STYLE_SHIFT;
	
	switch (value) {
		case LIST_STYLE_TYPE_DISC:
			[result appendString:@", disc"];
			break;
		case LIST_STYLE_TYPE_CIRCLE:
			[result appendString:@", circle"];
			break;
		case LIST_STYLE_TYPE_SQUARE:
			[result appendString:@", square"];
			break;
		case LIST_STYLE_TYPE_DECIMAL:
			break;
		case LIST_STYLE_TYPE_DECIMAL_LEADING_ZERO:
			[result appendString:@", decimal-leading-zero"];
			break;
		case LIST_STYLE_TYPE_LOWER_ROMAN:
			[result appendString:@", lower-roman"];
			break;
		case LIST_STYLE_TYPE_UPPER_ROMAN:
			[result appendString:@", upper-roman"];
			break;
		case LIST_STYLE_TYPE_LOWER_GREEK:
			[result appendString:@", lower-greek"];
			break;
		case LIST_STYLE_TYPE_LOWER_LATIN:
			[result appendString:@", lower-latin"];
			break;
		case LIST_STYLE_TYPE_UPPER_LATIN:
			[result appendString:@", upper-latin"];
			break;
		case LIST_STYLE_TYPE_ARMENIAN:
			[result appendString:@", armenian"];
			break;
		case LIST_STYLE_TYPE_GEORGIAN:
			[result appendString:@", georgian"];
			break;
		case LIST_STYLE_TYPE_LOWER_ALPHA:
			[result appendString:@", lower-alpha"];
			break;
		case LIST_STYLE_TYPE_UPPER_ALPHA:
			[result appendString:@", upper-alpha"];
			break;
		case LIST_STYLE_TYPE_NONE:
			[result appendString:@", none"];
			break;
	}
	
	[result appendString:@")"];
	
	return result;
}

void dump_objc_bytecode(css_style *style, NSMutableDictionary *resultDict, uint32_t depth)
{
	void *bytecode = style->bytecode;
	size_t length = (style->used * sizeof(css_code_t));
	uint32_t offset = 0;
	
#define ADVANCE(n) do {					\
offset += (n);					\
bytecode = ((uint8_t *) bytecode) + (n);	\
} while(0)
	
	while (offset < length) {
		opcode_t op;
		uint32_t value;
		uint32_t opv = *((uint32_t *) bytecode);
		
		ADVANCE(sizeof(opv));
		
		op = getOpcode(opv);
		
		NSString *opName = [NSString stringWithFormat:@"%s", opcode_names[op]];
		NSMutableString *result = [NSMutableString new];
		
		if (isInherit(opv)) {
			[result appendString:@"inherit"];
		} else {
			value = getValue(opv);
			
			switch (op) {
				case CSS_PROP_ALIGN_CONTENT:
					switch (value) {
						case ALIGN_CONTENT_STRETCH:
							[result appendString:@"stretch"];
							break;
						case ALIGN_CONTENT_FLEX_START:
							[result appendString:@"flex-start"];
							break;
						case ALIGN_CONTENT_FLEX_END:
							[result appendString:@"flex-end"];
							break;
						case ALIGN_CONTENT_CENTER:
							[result appendString:@"center"];
							break;
						case ALIGN_CONTENT_SPACE_BETWEEN:
							[result appendString:@"space-between"];
							break;
						case ALIGN_CONTENT_SPACE_AROUND:
							[result appendString:@"space-around"];
							break;
						case ALIGN_CONTENT_SPACE_EVENLY:
							[result appendString:@"space-evenly"];
							break;
					}
					break;
				case CSS_PROP_ALIGN_ITEMS:
					switch (value) {
						case ALIGN_ITEMS_STRETCH:
							[result appendString:@"stretch"];
							break;
						case ALIGN_ITEMS_FLEX_START:
							[result appendString:@"flex-start"];
							break;
						case ALIGN_ITEMS_FLEX_END:
							[result appendString:@"flex-end"];
							break;
						case ALIGN_ITEMS_CENTER:
							[result appendString:@"center"];
							break;
						case ALIGN_ITEMS_BASELINE:
							[result appendString:@"baseline"];
							break;
					}
					break;
				case CSS_PROP_ALIGN_SELF:
					switch (value) {
						case ALIGN_SELF_STRETCH:
							[result appendString:@"stretch"];
							break;
						case ALIGN_SELF_FLEX_START:
							[result appendString:@"flex-start"];
							break;
						case ALIGN_SELF_FLEX_END:
							[result appendString:@"flex-end"];
							break;
						case ALIGN_SELF_CENTER:
							[result appendString:@"center"];
							break;
						case ALIGN_SELF_BASELINE:
							[result appendString:@"baseline"];
							break;
						case ALIGN_SELF_AUTO:
							[result appendString:@"auto"];
							break;
					}
					break;
				case CSS_PROP_AZIMUTH:
					switch (value & ~AZIMUTH_BEHIND) {
						case AZIMUTH_ANGLE:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case AZIMUTH_LEFTWARDS:
							[result appendString:@"leftwards"];
							break;
						case AZIMUTH_RIGHTWARDS:
							[result appendString:@"rightwards"];
							break;
						case AZIMUTH_LEFT_SIDE:
							[result appendString:@"left-side"];
							break;
						case AZIMUTH_FAR_LEFT:
							[result appendString:@"far-left"];
							break;
						case AZIMUTH_LEFT:
							[result appendString:@"left"];
							break;
						case AZIMUTH_CENTER_LEFT:
							[result appendString:@"center-left"];
							break;
						case AZIMUTH_CENTER:
							[result appendString:@"center"];
							break;
						case AZIMUTH_CENTER_RIGHT:
							[result appendString:@"center-right"];
							break;
						case AZIMUTH_RIGHT:
							[result appendString:@"right"];
							break;
						case AZIMUTH_FAR_RIGHT:
							[result appendString:@"far-right"];
							break;
						case AZIMUTH_RIGHT_SIDE:
							[result appendString:@"right-side"];
							break;
					}
					if (value & AZIMUTH_BEHIND)
						[result appendString:@" behind"];
					break;
				case CSS_PROP_BACKGROUND_ATTACHMENT:
					switch (value) {
						case BACKGROUND_ATTACHMENT_FIXED:
							[result appendString:@"fixed"];
							break;
						case BACKGROUND_ATTACHMENT_SCROLL:
							[result appendString:@"scroll"];
							break;
					}
					break;
				case CSS_PROP_BORDER_TOP_COLOR:
				case CSS_PROP_BORDER_RIGHT_COLOR:
				case CSS_PROP_BORDER_BOTTOM_COLOR:
				case CSS_PROP_BORDER_LEFT_COLOR:
				case CSS_PROP_BACKGROUND_COLOR:
				case CSS_PROP_COLUMN_RULE_COLOR:
					assert(BACKGROUND_COLOR_TRANSPARENT ==
						   (enum op_background_color)
						   BORDER_COLOR_TRANSPARENT);
					assert(BACKGROUND_COLOR_CURRENT_COLOR ==
						   (enum op_background_color)
						   BORDER_COLOR_CURRENT_COLOR);
					assert(BACKGROUND_COLOR_SET ==
						   (enum op_background_color)
						   BORDER_COLOR_SET);
					
					switch (value) {
						case BACKGROUND_COLOR_TRANSPARENT:
							[result appendString:@"transparent"];
							break;
						case BACKGROUND_COLOR_CURRENT_COLOR:
							[result appendString:@"currentColor"];
							break;
						case BACKGROUND_COLOR_SET:
						{
							uint32_t colour =
							*((uint32_t *) bytecode);
							ADVANCE(sizeof(colour));
							[result appendFormat:@"#%08x", colour];
						}
							break;
					}
					break;
				case CSS_PROP_BACKGROUND_IMAGE:
				case CSS_PROP_CUE_AFTER:
				case CSS_PROP_CUE_BEFORE:
				case CSS_PROP_LIST_STYLE_IMAGE:
					assert(BACKGROUND_IMAGE_NONE ==
						   (enum op_background_image)
						   CUE_AFTER_NONE);
					assert(BACKGROUND_IMAGE_URI ==
						   (enum op_background_image)
						   CUE_AFTER_URI);
					assert(BACKGROUND_IMAGE_NONE ==
						   (enum op_background_image)
						   CUE_BEFORE_NONE);
					assert(BACKGROUND_IMAGE_URI ==
						   (enum op_background_image)
						   CUE_BEFORE_URI);
					assert(BACKGROUND_IMAGE_NONE ==
						   (enum op_background_image)
						   LIST_STYLE_IMAGE_NONE);
					assert(BACKGROUND_IMAGE_URI ==
						   (enum op_background_image)
						   LIST_STYLE_IMAGE_URI);
					
					switch (value) {
						case BACKGROUND_IMAGE_NONE:
							[result appendString:@"none"];
							break;
						case BACKGROUND_IMAGE_URI:
						{
							uint32_t snum = *((uint32_t *) bytecode);
							lwc_string *he;
							css__stylesheet_string_get(style->sheet,
													   snum,
													   &he);
							ADVANCE(sizeof(snum));
							[result appendFormat:@"url('%.*s')",
											(int) lwc_string_length(he),
											lwc_string_data(he)];
						}
							break;
					}
					break;
				case CSS_PROP_BACKGROUND_POSITION:
					switch (value & 0xf0) {
						case BACKGROUND_POSITION_HORZ_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case BACKGROUND_POSITION_HORZ_CENTER:
							[result appendString:@"center"];
							break;
						case BACKGROUND_POSITION_HORZ_RIGHT:
							[result appendString:@"right"];
							break;
						case BACKGROUND_POSITION_HORZ_LEFT:
							[result appendString:@"left"];
							break;
					}
					[result appendString:@" "];
					switch (value & 0x0f) {
						case BACKGROUND_POSITION_VERT_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case BACKGROUND_POSITION_VERT_CENTER:
							[result appendString:@"center"];
							break;
						case BACKGROUND_POSITION_VERT_BOTTOM:
							[result appendString:@"bottom"];
							break;
						case BACKGROUND_POSITION_VERT_TOP:
							[result appendString:@"top"];
							break;
					}
					break;
				case CSS_PROP_BACKGROUND_REPEAT:
					switch (value) {
						case BACKGROUND_REPEAT_NO_REPEAT:
							[result appendString:@"no-repeat"];
							break;
						case BACKGROUND_REPEAT_REPEAT_X:
							[result appendString:@"repeat-x"];
							break;
						case BACKGROUND_REPEAT_REPEAT_Y:
							[result appendString:@"repeat-y"];
							break;
						case BACKGROUND_REPEAT_REPEAT:
							[result appendString:@"repeat"];
							break;
					}
					break;
				case CSS_PROP_BORDER_COLLAPSE:
					switch (value) {
						case BORDER_COLLAPSE_SEPARATE:
							[result appendString:@"separate"];
							break;
						case BORDER_COLLAPSE_COLLAPSE:
							[result appendString:@"collapse"];
							break;
					}
					break;
				case CSS_PROP_BORDER_SPACING:
					switch (value) {
						case BORDER_SPACING_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
							
							val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
					}
					break;
				case CSS_PROP_BOX_SIZING:
					switch (value) {
						case BOX_SIZING_CONTENT_BOX:
							[result appendString:@"content-box"];
							break;
						case BOX_SIZING_BORDER_BOX:
							[result appendString:@"border-box"];
							break;
					}
					break;
				case CSS_PROP_BORDER_TOP_STYLE:
				case CSS_PROP_BORDER_RIGHT_STYLE:
				case CSS_PROP_BORDER_BOTTOM_STYLE:
				case CSS_PROP_BORDER_LEFT_STYLE:
				case CSS_PROP_COLUMN_RULE_STYLE:
				case CSS_PROP_OUTLINE_STYLE:
					assert(BORDER_STYLE_NONE ==
						   (enum op_border_style)
						   OUTLINE_STYLE_NONE);
					assert(BORDER_STYLE_NONE ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_NONE);
					assert(BORDER_STYLE_HIDDEN ==
						   (enum op_border_style)
						   OUTLINE_STYLE_HIDDEN);
					assert(BORDER_STYLE_HIDDEN ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_HIDDEN);
					assert(BORDER_STYLE_DOTTED ==
						   (enum op_border_style)
						   OUTLINE_STYLE_DOTTED);
					assert(BORDER_STYLE_DOTTED ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_DOTTED);
					assert(BORDER_STYLE_DASHED ==
						   (enum op_border_style)
						   OUTLINE_STYLE_DASHED);
					assert(BORDER_STYLE_DASHED ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_DASHED);
					assert(BORDER_STYLE_SOLID ==
						   (enum op_border_style)
						   OUTLINE_STYLE_SOLID);
					assert(BORDER_STYLE_SOLID ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_SOLID);
					assert(BORDER_STYLE_DOUBLE ==
						   (enum op_border_style)
						   OUTLINE_STYLE_DOUBLE);
					assert(BORDER_STYLE_DOUBLE ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_DOUBLE);
					assert(BORDER_STYLE_GROOVE ==
						   (enum op_border_style)
						   OUTLINE_STYLE_GROOVE);
					assert(BORDER_STYLE_GROOVE ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_GROOVE);
					assert(BORDER_STYLE_RIDGE ==
						   (enum op_border_style)
						   OUTLINE_STYLE_RIDGE);
					assert(BORDER_STYLE_RIDGE ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_RIDGE);
					assert(BORDER_STYLE_INSET ==
						   (enum op_border_style)
						   OUTLINE_STYLE_INSET);
					assert(BORDER_STYLE_INSET ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_INSET);
					assert(BORDER_STYLE_OUTSET ==
						   (enum op_border_style)
						   OUTLINE_STYLE_OUTSET);
					assert(BORDER_STYLE_OUTSET ==
						   (enum op_border_style)
						   COLUMN_RULE_STYLE_OUTSET);
					
					switch (value) {
						case BORDER_STYLE_NONE:
							[result appendString:@"none"];
							break;
						case BORDER_STYLE_HIDDEN:
							[result appendString:@"hidden"];
							break;
						case BORDER_STYLE_DOTTED:
							[result appendString:@"dotted"];
							break;
						case BORDER_STYLE_DASHED:
							[result appendString:@"dashed"];
							break;
						case BORDER_STYLE_SOLID:
							[result appendString:@"solid"];
							break;
						case BORDER_STYLE_DOUBLE:
							[result appendString:@"double"];
							break;
						case BORDER_STYLE_GROOVE:
							[result appendString:@"groove"];
							break;
						case BORDER_STYLE_RIDGE:
							[result appendString:@"ridge"];
							break;
						case BORDER_STYLE_INSET:
							[result appendString:@"inset"];
							break;
						case BORDER_STYLE_OUTSET:
							[result appendString:@"outset"];
							break;
					}
					break;
				case CSS_PROP_BORDER_TOP_WIDTH:
				case CSS_PROP_BORDER_RIGHT_WIDTH:
				case CSS_PROP_BORDER_BOTTOM_WIDTH:
				case CSS_PROP_BORDER_LEFT_WIDTH:
				case CSS_PROP_COLUMN_RULE_WIDTH:
				case CSS_PROP_OUTLINE_WIDTH:
					assert(BORDER_WIDTH_SET ==
						   (enum op_border_width)
						   OUTLINE_WIDTH_SET);
					assert(BORDER_WIDTH_THIN ==
						   (enum op_border_width)
						   OUTLINE_WIDTH_THIN);
					assert(BORDER_WIDTH_MEDIUM ==
						   (enum op_border_width)
						   OUTLINE_WIDTH_MEDIUM);
					assert(BORDER_WIDTH_THICK ==
						   (enum op_border_width)
						   OUTLINE_WIDTH_THICK);
					
					switch (value) {
						case BORDER_WIDTH_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case BORDER_WIDTH_THIN:
							[result appendString:@"thin"];
							break;
						case BORDER_WIDTH_MEDIUM:
							[result appendString:@"medium"];
							break;
						case BORDER_WIDTH_THICK:
							[result appendString:@"thick"];
							break;
					}
					break;
				case CSS_PROP_MARGIN_TOP:
				case CSS_PROP_MARGIN_RIGHT:
				case CSS_PROP_MARGIN_BOTTOM:
				case CSS_PROP_MARGIN_LEFT:
				case CSS_PROP_BOTTOM:
				case CSS_PROP_LEFT:
				case CSS_PROP_RIGHT:
				case CSS_PROP_TOP:
				case CSS_PROP_HEIGHT:
				case CSS_PROP_WIDTH:
				case CSS_PROP_COLUMN_WIDTH:
					assert(BOTTOM_SET ==
						   (enum op_bottom) LEFT_SET);
					assert(BOTTOM_AUTO ==
						   (enum op_bottom) LEFT_AUTO);
					assert(BOTTOM_SET ==
						   (enum op_bottom) RIGHT_SET);
					assert(BOTTOM_AUTO ==
						   (enum op_bottom) RIGHT_AUTO);
					assert(BOTTOM_SET ==
						   (enum op_bottom) TOP_SET);
					assert(BOTTOM_AUTO ==
						   (enum op_bottom) TOP_AUTO);
					assert(BOTTOM_SET ==
						   (enum op_bottom) HEIGHT_SET);
					assert(BOTTOM_AUTO ==
						   (enum op_bottom) HEIGHT_AUTO);
					assert(BOTTOM_SET ==
						   (enum op_bottom) MARGIN_SET);
					assert(BOTTOM_AUTO ==
						   (enum op_bottom) MARGIN_AUTO);
					assert(BOTTOM_SET ==
						   (enum op_bottom) WIDTH_SET);
					assert(BOTTOM_AUTO ==
						   (enum op_bottom) WIDTH_AUTO);
					assert(BOTTOM_SET ==
						   (enum op_bottom)
						   COLUMN_WIDTH_SET);
					assert(BOTTOM_AUTO ==
						   (enum op_bottom)
						   COLUMN_WIDTH_AUTO);
					
					switch (value) {
						case BOTTOM_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case BOTTOM_AUTO:
							[result appendString:@"auto"];
							break;
					}
					break;
				case CSS_PROP_BREAK_AFTER:
				case CSS_PROP_BREAK_BEFORE:
					assert(BREAK_AFTER_AUTO ==
						   (enum op_break_after)
						   BREAK_BEFORE_AUTO);
					assert(BREAK_AFTER_ALWAYS ==
						   (enum op_break_after)
						   BREAK_BEFORE_ALWAYS);
					assert(BREAK_AFTER_AVOID ==
						   (enum op_break_after)
						   BREAK_BEFORE_AVOID);
					assert(BREAK_AFTER_LEFT ==
						   (enum op_break_after)
						   BREAK_BEFORE_LEFT);
					assert(BREAK_AFTER_RIGHT ==
						   (enum op_break_after)
						   BREAK_BEFORE_RIGHT);
					assert(BREAK_AFTER_PAGE ==
						   (enum op_break_after)
						   BREAK_BEFORE_PAGE);
					assert(BREAK_AFTER_COLUMN ==
						   (enum op_break_after)
						   BREAK_BEFORE_COLUMN);
					assert(BREAK_AFTER_AVOID_PAGE ==
						   (enum op_break_after)
						   BREAK_BEFORE_AVOID_PAGE);
					assert(BREAK_AFTER_AVOID_COLUMN ==
						   (enum op_break_after)
						   BREAK_BEFORE_AVOID_COLUMN);
					
					switch (value) {
						case BREAK_AFTER_AUTO:
							[result appendString:@"auto"];
							break;
						case BREAK_AFTER_ALWAYS:
							[result appendString:@"always"];
							break;
						case BREAK_AFTER_AVOID:
							[result appendString:@"avoid"];
							break;
						case BREAK_AFTER_LEFT:
							[result appendString:@"left"];
							break;
						case BREAK_AFTER_RIGHT:
							[result appendString:@"right"];
							break;
						case BREAK_AFTER_PAGE:
							[result appendString:@"page"];
							break;
						case BREAK_AFTER_COLUMN:
							[result appendString:@"column"];
							break;
						case BREAK_AFTER_AVOID_PAGE:
							[result appendString:@"avoid-page"];
							break;
						case BREAK_AFTER_AVOID_COLUMN:
							[result appendString:@"avoid-column"];
							break;
					}
					break;
				case CSS_PROP_BREAK_INSIDE:
					switch (value) {
						case BREAK_INSIDE_AUTO:
							[result appendString:@"auto"];
							break;
						case BREAK_INSIDE_AVOID:
							[result appendString:@"avoid"];
							break;
						case BREAK_INSIDE_AVOID_PAGE:
							[result appendString:@"avoid-page"];
							break;
						case BREAK_INSIDE_AVOID_COLUMN:
							[result appendString:@"avoid-column"];
							break;
					}
					break;
				case CSS_PROP_CAPTION_SIDE:
					switch (value) {
						case CAPTION_SIDE_TOP:
							[result appendString:@"top"];
							break;
						case CAPTION_SIDE_BOTTOM:
							[result appendString:@"bottom"];
							break;
					}
					break;
				case CSS_PROP_CLEAR:
					switch (value) {
						case CLEAR_NONE:
							[result appendString:@"none"];
							break;
						case CLEAR_LEFT:
							[result appendString:@"left"];
							break;
						case CLEAR_RIGHT:
							[result appendString:@"right"];
							break;
						case CLEAR_BOTH:
							[result appendString:@"both"];
							break;
					}
					break;
				case CSS_PROP_CLIP:
					if ((value & CLIP_SHAPE_MASK) ==
						CLIP_SHAPE_RECT) {
						[result appendString:@"rect("];
						if (value & CLIP_RECT_TOP_AUTO) {
							[result appendString:@"auto"];
						} else {
							uint32_t unit;
							css_fixed val =
							*((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
						[result appendString:@", "];
						if (value & CLIP_RECT_RIGHT_AUTO) {
							[result appendString:@"auto"];
						} else {
							uint32_t unit;
							css_fixed val =
							*((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
						[result appendString:@", "];
						if (value & CLIP_RECT_BOTTOM_AUTO) {
							[result appendString:@"auto"];
						} else {
							uint32_t unit;
							css_fixed val =
							*((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
						[result appendString:@", "];
						if (value & CLIP_RECT_LEFT_AUTO) {
							[result appendString:@"auto"];
						} else {
							uint32_t unit;
							css_fixed val =
							*((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
						[result appendString:@")"];
					} else
						[result appendString:@"auto"];
					break;
				case CSS_PROP_COLOR:
					switch (value) {
						case COLOR_TRANSPARENT:
							[result appendString:@"transparent"];
							break;
						case COLOR_CURRENT_COLOR:
							[result appendString:@"currentColor"];
							break;
						case COLOR_SET:
						{
							uint32_t colour =
							*((uint32_t *) bytecode);
							ADVANCE(sizeof(colour));
							[result appendFormat:@"#%08x", colour];
						}
							break;
					}
					break;
				case CSS_PROP_COLUMN_COUNT:
					switch (value) {
						case COLUMN_COUNT_SET:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
						case COLUMN_COUNT_AUTO:
							[result appendString:@"auto"];
							break;
					}
					break;
				case CSS_PROP_COLUMN_FILL:
					switch (value) {
						case COLUMN_FILL_BALANCE:
							[result appendString:@"balance"];
							break;
						case COLUMN_FILL_AUTO:
							[result appendString:@"auto"];
							break;
					}
					break;
				case CSS_PROP_COLUMN_GAP:
					switch (value) {
						case COLUMN_GAP_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case COLUMN_GAP_NORMAL:
							[result appendString:@"normal"];
							break;
					}
					break;
				case CSS_PROP_COLUMN_SPAN:
					switch (value) {
						case COLUMN_SPAN_NONE:
							[result appendString:@"none"];
							break;
						case COLUMN_SPAN_ALL:
							[result appendString:@"all"];
							break;
					}
					break;
				case CSS_PROP_CONTENT:
					if (value == CONTENT_NORMAL) {
						[result appendString:@"normal"];
						break;
					} else if (value == CONTENT_NONE) {
						[result appendString:@"none"];
						break;
					}
					
					while (value != CONTENT_NORMAL) {
						uint32_t snum = *((uint32_t *) bytecode);
						lwc_string *he;
						const char *end = "";
						
						switch (value & 0xff) {
							case CONTENT_COUNTER:
								css__stylesheet_string_get(style->sheet, snum, &he);
								ADVANCE(sizeof(snum));
								[result appendString:dump_objc_counter(he, value)];
								break;
								
							case CONTENT_COUNTERS:
							{
								lwc_string *sep;
								css__stylesheet_string_get(style->sheet, snum, &he);
								ADVANCE(sizeof(snum));
								
								sep = *((lwc_string **) bytecode);
								ADVANCE(sizeof(sep));
								
								[result appendString:dump_objc_counters(he, sep, value)];
							}
								break;
								
							case CONTENT_URI:
							case CONTENT_ATTR:
							case CONTENT_STRING:
								css__stylesheet_string_get(style->sheet, snum, &he);
								if (value == CONTENT_URI)
									[result appendString:@"url("];
								if (value == CONTENT_ATTR)
									[result appendString:@"attr("];
								if (value != CONTENT_STRING)
									end = ")";
								
								ADVANCE(sizeof(snum));
								
								[result appendFormat:@"'%.*s'%s",
												(int) lwc_string_length(he),
												lwc_string_data(he),
												end];
								break;
								
							case CONTENT_OPEN_QUOTE:
								[result appendString:@"open-quote"];
								break;
								
							case CONTENT_CLOSE_QUOTE:
								[result appendString:@"close-quote"];
								break;
								
							case CONTENT_NO_OPEN_QUOTE:
								[result appendString:@"no-open-quote"];
								break;
								
							case CONTENT_NO_CLOSE_QUOTE:
								[result appendString:@"no-close-quote"];
								break;
						}
						
						value = *((uint32_t *) bytecode);
						ADVANCE(sizeof(value));
						
						if (value != CONTENT_NORMAL)
							[result appendString:@" "];
					}
					break;
				case CSS_PROP_COUNTER_INCREMENT:
				case CSS_PROP_COUNTER_RESET:
					assert(COUNTER_INCREMENT_NONE ==
						   (enum op_counter_increment)
						   COUNTER_RESET_NONE);
					assert(COUNTER_INCREMENT_NAMED ==
						   (enum op_counter_increment)
						   COUNTER_RESET_NAMED);
					
					switch (value) {
						case COUNTER_INCREMENT_NAMED:
							while (value != COUNTER_INCREMENT_NONE) {
								css_fixed val;
								uint32_t snum = *((uint32_t *) bytecode);					lwc_string *he;
								css__stylesheet_string_get(style->sheet,
														   snum,
														   &he);
								ADVANCE(sizeof(snum));
								[result appendFormat:@"%.*s ",
												(int)lwc_string_length(he),
												lwc_string_data(he)];
								val = *((css_fixed *) bytecode);
								ADVANCE(sizeof(val));
								[result appendString:dump_objc_css_number(val)];
								
								value = *((uint32_t *) bytecode);
								ADVANCE(sizeof(value));
								
								if (value !=
									COUNTER_INCREMENT_NONE)
									[result appendString:@" "];
							}
							break;
						case COUNTER_INCREMENT_NONE:
							[result appendString:@"none"];
							break;
					}
					break;
				case CSS_PROP_CURSOR:
					while (value == CURSOR_URI) {
						uint32_t snum = *((uint32_t *) bytecode);					lwc_string *he;
						css__stylesheet_string_get(style->sheet,
												   snum,
												   &he);
						ADVANCE(sizeof(snum));
						[result appendFormat:@"url('%.*s'), ",
										(int) lwc_string_length(he),
										lwc_string_data(he)];
						
						value = *((uint32_t *) bytecode);
						ADVANCE(sizeof(value));
					}
					
					switch (value) {
						case CURSOR_AUTO:
							[result appendString:@"auto"];
							break;
						case CURSOR_CROSSHAIR:
							[result appendString:@"crosshair"];
							break;
						case CURSOR_DEFAULT:
							[result appendString:@"default"];
							break;
						case CURSOR_POINTER:
							[result appendString:@"pointer"];
							break;
						case CURSOR_MOVE:
							[result appendString:@"move"];
							break;
						case CURSOR_E_RESIZE:
							[result appendString:@"e-resize"];
							break;
						case CURSOR_NE_RESIZE:
							[result appendString:@"ne-resize"];
							break;
						case CURSOR_NW_RESIZE:
							[result appendString:@"nw-resize"];
							break;
						case CURSOR_N_RESIZE:
							[result appendString:@"n-resize"];
							break;
						case CURSOR_SE_RESIZE:
							[result appendString:@"se-resize"];
							break;
						case CURSOR_SW_RESIZE:
							[result appendString:@"sw-resize"];
							break;
						case CURSOR_S_RESIZE:
							[result appendString:@"s-resize"];
							break;
						case CURSOR_W_RESIZE:
							[result appendString:@"w-resize"];
							break;
						case CURSOR_TEXT:
							[result appendString:@"text"];
							break;
						case CURSOR_WAIT:
							[result appendString:@"wait"];
							break;
						case CURSOR_HELP:
							[result appendString:@"help"];
							break;
						case CURSOR_PROGRESS:
							[result appendString:@"progress"];
							break;
					}
					break;
				case CSS_PROP_DIRECTION:
					switch (value) {
						case DIRECTION_LTR:
							[result appendString:@"ltr"];
							break;
						case DIRECTION_RTL:
							[result appendString:@"rtl"];
							break;
					}
					break;
				case CSS_PROP_DISPLAY:
					switch (value) {
						case DISPLAY_INLINE:
							[result appendString:@"inline"];
							break;
						case DISPLAY_BLOCK:
							[result appendString:@"block"];
							break;
						case DISPLAY_LIST_ITEM:
							[result appendString:@"list-item"];
							break;
						case DISPLAY_RUN_IN:
							[result appendString:@"run-in"];
							break;
						case DISPLAY_INLINE_BLOCK:
							[result appendString:@"inline-block"];
							break;
						case DISPLAY_TABLE:
							[result appendString:@"table"];
							break;
						case DISPLAY_INLINE_TABLE:
							[result appendString:@"inline-table"];
							break;
						case DISPLAY_TABLE_ROW_GROUP:
							[result appendString:@"table-row-group"];
							break;
						case DISPLAY_TABLE_HEADER_GROUP:
							[result appendString:@"table-header-group"];
							break;
						case DISPLAY_TABLE_FOOTER_GROUP:
							[result appendString:@"table-footer-group"];
							break;
						case DISPLAY_TABLE_ROW:
							[result appendString:@"table-row"];
							break;
						case DISPLAY_TABLE_COLUMN_GROUP:
							[result appendString:@"table-column-group"];
							break;
						case DISPLAY_TABLE_COLUMN:
							[result appendString:@"table-column"];
							break;
						case DISPLAY_TABLE_CELL:
							[result appendString:@"table-cell"];
							break;
						case DISPLAY_TABLE_CAPTION:
							[result appendString:@"table-caption"];
							break;
						case DISPLAY_NONE:
							[result appendString:@"none"];
							break;
						case DISPLAY_FLEX:
							[result appendString:@"flex"];
							break;
						case DISPLAY_INLINE_FLEX:
							[result appendString:@"inline-flex"];
							break;
					}
					break;
				case CSS_PROP_ELEVATION:
					switch (value) {
						case ELEVATION_ANGLE:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case ELEVATION_BELOW:
							[result appendString:@"below"];
							break;
						case ELEVATION_LEVEL:
							[result appendString:@"level"];
							break;
						case ELEVATION_ABOVE:
							[result appendString:@"above"];
							break;
						case ELEVATION_HIGHER:
							[result appendString:@"higher"];
							break;
						case ELEVATION_LOWER:
							[result appendString:@"lower"];
							break;
					}
					break;
				case CSS_PROP_EMPTY_CELLS:
					switch (value) {
						case EMPTY_CELLS_SHOW:
							[result appendString:@"show"];
							break;
						case EMPTY_CELLS_HIDE:
							[result appendString:@"hide"];
							break;
					}
					break;
				case CSS_PROP_FLEX_BASIS:
					switch (value) {
						case FLEX_BASIS_AUTO:
							[result appendString:@"auto"];
							break;
						case FLEX_BASIS_CONTENT:
							[result appendString:@"content"];
							break;
						case FLEX_BASIS_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
					}
					break;
				case CSS_PROP_FLEX_DIRECTION:
					switch (value) {
						case FLEX_DIRECTION_ROW:
							[result appendString:@"row"];
							break;
						case FLEX_DIRECTION_COLUMN:
							[result appendString:@"column"];
							break;
						case FLEX_DIRECTION_ROW_REVERSE:
							[result appendString:@"row-reverse"];
							break;
						case FLEX_DIRECTION_COLUMN_REVERSE:
							[result appendString:@"column-reverse"];
							break;
					}
					break;
				case CSS_PROP_FLEX_GROW:
					switch (value) {
						case FLEX_GROW_SET:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
					}
					break;
				case CSS_PROP_FLEX_SHRINK:
					switch (value) {
						case FLEX_SHRINK_SET:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
					}
					break;
				case CSS_PROP_FLEX_WRAP:
					switch (value) {
						case FLEX_WRAP_NOWRAP:
							[result appendString:@"nowrap"];
							break;
						case FLEX_WRAP_WRAP:
							[result appendString:@"wrap"];
							break;
						case FLEX_WRAP_WRAP_REVERSE:
							[result appendString:@"wrap-reverse"];
							break;
					}
					break;
				case CSS_PROP_FLOAT:
					switch (value) {
						case FLOAT_LEFT:
							[result appendString:@"left"];
							break;
						case FLOAT_RIGHT:
							[result appendString:@"right"];
							break;
						case FLOAT_NONE:
							[result appendString:@"none"];
							break;
					}
					break;
				case CSS_PROP_FONT_FAMILY:
					while (value != FONT_FAMILY_END) {
						switch (value) {
							case FONT_FAMILY_STRING:
							case FONT_FAMILY_IDENT_LIST:
							{
								uint32_t snum = *((uint32_t *) bytecode);
								lwc_string *he;
								css__stylesheet_string_get(style->sheet, snum, &he);
								ADVANCE(sizeof(snum));
								[result appendFormat:@"'%.*s'",
												(int) lwc_string_length(he),
												lwc_string_data(he)];
							}
								break;
							case FONT_FAMILY_SERIF:
								[result appendString:@"serif"];
								break;
							case FONT_FAMILY_SANS_SERIF:
								[result appendString:@"sans-serif"];
								break;
							case FONT_FAMILY_CURSIVE:
								[result appendString:@"cursive"];
								break;
							case FONT_FAMILY_FANTASY:
								[result appendString:@"fantasy"];
								break;
							case FONT_FAMILY_MONOSPACE:
								[result appendString:@"monospace"];
								break;
						}
						
						value = *((uint32_t *) bytecode);
						ADVANCE(sizeof(value));
						
						if (value != FONT_FAMILY_END)
							[result appendString:@", "];
					}
					break;
				case CSS_PROP_FONT_SIZE:
					switch (value) {
						case FONT_SIZE_DIMENSION:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case FONT_SIZE_XX_SMALL:
							[result appendString:@"xx-small"];
							break;
						case FONT_SIZE_X_SMALL:
							[result appendString:@"x-small"];
							break;
						case FONT_SIZE_SMALL:
							[result appendString:@"small"];
							break;
						case FONT_SIZE_MEDIUM:
							[result appendString:@"medium"];
							break;
						case FONT_SIZE_LARGE:
							[result appendString:@"large"];
							break;
						case FONT_SIZE_X_LARGE:
							[result appendString:@"x-large"];
							break;
						case FONT_SIZE_XX_LARGE:
							[result appendString:@"xx-large"];
							break;
						case FONT_SIZE_LARGER:
							[result appendString:@"larger"];
							break;
						case FONT_SIZE_SMALLER:
							[result appendString:@"smaller"];
							break;
					}
					break;
				case CSS_PROP_FONT_STYLE:
					switch (value) {
						case FONT_STYLE_NORMAL:
							[result appendString:@"normal"];
							break;
						case FONT_STYLE_ITALIC:
							[result appendString:@"italic"];
							break;
						case FONT_STYLE_OBLIQUE:
							[result appendString:@"oblique"];
							break;
					}
					break;
				case CSS_PROP_FONT_VARIANT:
					switch (value) {
						case FONT_VARIANT_NORMAL:
							[result appendString:@"normal"];
							break;
						case FONT_VARIANT_SMALL_CAPS:
							[result appendString:@"small-caps"];
							break;
					}
					break;
				case CSS_PROP_FONT_WEIGHT:
					switch (value) {
						case FONT_WEIGHT_NORMAL:
							[result appendString:@"normal"];
							break;
						case FONT_WEIGHT_BOLD:
							[result appendString:@"bold"];
							break;
						case FONT_WEIGHT_BOLDER:
							[result appendString:@"bolder"];
							break;
						case FONT_WEIGHT_LIGHTER:
							[result appendString:@"lighter"];
							break;
						case FONT_WEIGHT_100:
							[result appendString:@"100"];
							break;
						case FONT_WEIGHT_200:
							[result appendString:@"200"];
							break;
						case FONT_WEIGHT_300:
							[result appendString:@"300"];
							break;
						case FONT_WEIGHT_400:
							[result appendString:@"400"];
							break;
						case FONT_WEIGHT_500:
							[result appendString:@"500"];
							break;
						case FONT_WEIGHT_600:
							[result appendString:@"600"];
							break;
						case FONT_WEIGHT_700:
							[result appendString:@"700"];
							break;
						case FONT_WEIGHT_800:
							[result appendString:@"800"];
							break;
						case FONT_WEIGHT_900:
							[result appendString:@"900"];
							break;
					}
					break;
				case CSS_PROP_JUSTIFY_CONTENT:
					switch (value) {
						case JUSTIFY_CONTENT_FLEX_START:
							[result appendString:@"flex-start"];
							break;
						case JUSTIFY_CONTENT_FLEX_END:
							[result appendString:@"flex-end"];
							break;
						case JUSTIFY_CONTENT_CENTER:
							[result appendString:@"center"];
							break;
						case JUSTIFY_CONTENT_SPACE_BETWEEN:
							[result appendString:@"space-between"];
							break;
						case JUSTIFY_CONTENT_SPACE_AROUND:
							[result appendString:@"space-around"];
							break;
						case JUSTIFY_CONTENT_SPACE_EVENLY:
							[result appendString:@"space-evenly"];
							break;
					}
					break;
					
				case CSS_PROP_LETTER_SPACING:
				case CSS_PROP_WORD_SPACING:
					assert(LETTER_SPACING_SET ==
						   (enum op_letter_spacing)
						   WORD_SPACING_SET);
					assert(LETTER_SPACING_NORMAL ==
						   (enum op_letter_spacing)
						   WORD_SPACING_NORMAL);
					
					switch (value) {
						case LETTER_SPACING_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case LETTER_SPACING_NORMAL:
							[result appendString:@"normal"];
							break;
					}
					break;
				case CSS_PROP_LINE_HEIGHT:
					switch (value) {
						case LINE_HEIGHT_NUMBER:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
						case LINE_HEIGHT_DIMENSION:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case LINE_HEIGHT_NORMAL:
							[result appendString:@"normal"];
							break;
					}
					break;
				case CSS_PROP_LIST_STYLE_POSITION:
					switch (value) {
						case LIST_STYLE_POSITION_INSIDE:
							[result appendString:@"inside"];
							break;
						case LIST_STYLE_POSITION_OUTSIDE:
							[result appendString:@"outside"];
							break;
					}
					break;
				case CSS_PROP_LIST_STYLE_TYPE:
					switch (value) {
						case LIST_STYLE_TYPE_DISC:
							[result appendString:@"disc"];
							break;
						case LIST_STYLE_TYPE_CIRCLE:
							[result appendString:@"circle"];
							break;
						case LIST_STYLE_TYPE_SQUARE:
							[result appendString:@"square"];
							break;
						case LIST_STYLE_TYPE_DECIMAL:
							[result appendString:@"decimal"];
							break;
						case LIST_STYLE_TYPE_DECIMAL_LEADING_ZERO:
							[result appendString:@"decimal-leading-zero"];
							break;
						case LIST_STYLE_TYPE_LOWER_ROMAN:
							[result appendString:@"lower-roman"];
							break;
						case LIST_STYLE_TYPE_UPPER_ROMAN:
							[result appendString:@"upper-roman"];
							break;
						case LIST_STYLE_TYPE_LOWER_GREEK:
							[result appendString:@"lower-greek"];
							break;
						case LIST_STYLE_TYPE_LOWER_LATIN:
							[result appendString:@"lower-latin"];
							break;
						case LIST_STYLE_TYPE_UPPER_LATIN:
							[result appendString:@"upper-latin"];
							break;
						case LIST_STYLE_TYPE_ARMENIAN:
							[result appendString:@"armenian"];
							break;
						case LIST_STYLE_TYPE_GEORGIAN:
							[result appendString:@"georgian"];
							break;
						case LIST_STYLE_TYPE_LOWER_ALPHA:
							[result appendString:@"lower-alpha"];
							break;
						case LIST_STYLE_TYPE_UPPER_ALPHA:
							[result appendString:@"upper-alpha"];
							break;
						case LIST_STYLE_TYPE_NONE:
							[result appendString:@"none"];
							break;
					}
					break;
				case CSS_PROP_MAX_HEIGHT:
				case CSS_PROP_MAX_WIDTH:
					assert(MAX_HEIGHT_SET ==
						   (enum op_max_height)
						   MAX_WIDTH_SET);
					assert(MAX_HEIGHT_NONE ==
						   (enum op_max_height)
						   MAX_WIDTH_NONE);
					
					switch (value) {
						case MAX_HEIGHT_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case MAX_HEIGHT_NONE:
							[result appendString:@"none"];
							break;
					}
					break;
				case CSS_PROP_MIN_HEIGHT:
				case CSS_PROP_MIN_WIDTH:
					assert(MIN_HEIGHT_SET ==
						   (enum op_min_height)
						   MIN_WIDTH_SET);
					assert(MIN_HEIGHT_AUTO ==
						   (enum op_min_height)
						   MIN_WIDTH_AUTO);
					
					switch (value) {
						case MIN_HEIGHT_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case MIN_HEIGHT_AUTO:
							[result appendString:@"auto"];
							break;
					}
					break;
				case CSS_PROP_OPACITY:
					switch (value) {
						case OPACITY_SET:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
					}
					break;
				case CSS_PROP_ORDER:
					switch (value) {
						case ORDER_SET:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
					}
					break;
				case CSS_PROP_PADDING_TOP:
				case CSS_PROP_PADDING_RIGHT:
				case CSS_PROP_PADDING_BOTTOM:
				case CSS_PROP_PADDING_LEFT:
				case CSS_PROP_PAUSE_AFTER:
				case CSS_PROP_PAUSE_BEFORE:
				case CSS_PROP_TEXT_INDENT:
					assert(TEXT_INDENT_SET ==
						   (enum op_text_indent)
						   PADDING_SET);
					assert(TEXT_INDENT_SET ==
						   (enum op_text_indent)
						   PAUSE_AFTER_SET);
					assert(TEXT_INDENT_SET ==
						   (enum op_text_indent)
						   PAUSE_BEFORE_SET);
					
					switch (value) {
						case TEXT_INDENT_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
					}
					break;
				case CSS_PROP_ORPHANS:
				case CSS_PROP_PITCH_RANGE:
				case CSS_PROP_RICHNESS:
				case CSS_PROP_STRESS:
				case CSS_PROP_WIDOWS:
					assert(ORPHANS_SET ==
						   (enum op_orphans)
						   PITCH_RANGE_SET);
					assert(ORPHANS_SET ==
						   (enum op_orphans)
						   RICHNESS_SET);
					assert(ORPHANS_SET ==
						   (enum op_orphans)
						   STRESS_SET);
					assert(ORPHANS_SET ==
						   (enum op_orphans)
						   WIDOWS_SET);
					
					switch (value) {
						case ORPHANS_SET:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
					}
					break;
				case CSS_PROP_OUTLINE_COLOR:
					switch (value) {
						case OUTLINE_COLOR_TRANSPARENT:
							[result appendString:@"transparent"];
							break;
						case OUTLINE_COLOR_CURRENT_COLOR:
							[result appendString:@"currentColor"];
							break;
						case OUTLINE_COLOR_SET:
						{
							uint32_t colour =
							*((uint32_t *) bytecode);
							ADVANCE(sizeof(colour));
							[result appendFormat:@"#%08x", colour];
						}
							break;
						case OUTLINE_COLOR_INVERT:
							[result appendString:@"invert"];
							break;
					}
					break;
				case CSS_PROP_OVERFLOW_X:
				case CSS_PROP_OVERFLOW_Y:
					switch (value) {
						case OVERFLOW_VISIBLE:
							[result appendString:@"visible"];
							break;
						case OVERFLOW_HIDDEN:
							[result appendString:@"hidden"];
							break;
						case OVERFLOW_SCROLL:
							[result appendString:@"scroll"];
							break;
						case OVERFLOW_AUTO:
							[result appendString:@"auto"];
							break;
					}
					break;
				case CSS_PROP_PAGE_BREAK_AFTER:
				case CSS_PROP_PAGE_BREAK_BEFORE:
					assert(PAGE_BREAK_AFTER_AUTO ==
						   (enum op_page_break_after)
						   PAGE_BREAK_BEFORE_AUTO);
					assert(PAGE_BREAK_AFTER_ALWAYS ==
						   (enum op_page_break_after)
						   PAGE_BREAK_BEFORE_ALWAYS);
					assert(PAGE_BREAK_AFTER_AVOID ==
						   (enum op_page_break_after)
						   PAGE_BREAK_BEFORE_AVOID);
					assert(PAGE_BREAK_AFTER_LEFT ==
						   (enum op_page_break_after)
						   PAGE_BREAK_BEFORE_LEFT);
					assert(PAGE_BREAK_AFTER_RIGHT ==
						   (enum op_page_break_after)
						   PAGE_BREAK_BEFORE_RIGHT);
					
					switch (value) {
						case PAGE_BREAK_AFTER_AUTO:
							[result appendString:@"auto"];
							break;
						case PAGE_BREAK_AFTER_ALWAYS:
							[result appendString:@"always"];
							break;
						case PAGE_BREAK_AFTER_AVOID:
							[result appendString:@"avoid"];
							break;
						case PAGE_BREAK_AFTER_LEFT:
							[result appendString:@"left"];
							break;
						case PAGE_BREAK_AFTER_RIGHT:
							[result appendString:@"right"];
							break;
					}
					break;
				case CSS_PROP_PAGE_BREAK_INSIDE:
					switch (value) {
						case PAGE_BREAK_INSIDE_AUTO:
							[result appendString:@"auto"];
							break;
						case PAGE_BREAK_INSIDE_AVOID:
							[result appendString:@"avoid"];
							break;
					}
					break;
				case CSS_PROP_PITCH:
					switch (value) {
						case PITCH_FREQUENCY:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case PITCH_X_LOW:
							[result appendString:@"x-low"];
							break;
						case PITCH_LOW:
							[result appendString:@"low"];
							break;
						case PITCH_MEDIUM:
							[result appendString:@"medium"];
							break;
						case PITCH_HIGH:
							[result appendString:@"high"];
							break;
						case PITCH_X_HIGH:
							[result appendString:@"x-high"];
							break;
					}
					break;
				case CSS_PROP_PLAY_DURING:
					switch (value) {
						case PLAY_DURING_URI:
						{
							uint32_t snum = *((uint32_t *) bytecode);
							lwc_string *he;
							css__stylesheet_string_get(style->sheet, snum, &he);
							ADVANCE(sizeof(snum));
							[result appendFormat:@"'%.*s'",
											(int) lwc_string_length(he),
											lwc_string_data(he)];
						}
							break;
						case PLAY_DURING_AUTO:
							[result appendString:@"auto"];
							break;
						case PLAY_DURING_NONE:
							[result appendString:@"none"];
							break;
					}
					
					if (value & PLAY_DURING_MIX)
						[result appendString:@" mix"];
					if (value & PLAY_DURING_REPEAT)
						[result appendString:@" repeat"];
					break;
				case CSS_PROP_POSITION:
					switch (value) {
						case POSITION_STATIC:
							[result appendString:@"static"];
							break;
						case POSITION_RELATIVE:
							[result appendString:@"relative"];
							break;
						case POSITION_ABSOLUTE:
							[result appendString:@"absolute"];
							break;
						case POSITION_FIXED:
							[result appendString:@"fixed"];
							break;
					}
					break;
				case CSS_PROP_QUOTES:
					switch (value) {
						case QUOTES_STRING:
							while (value != QUOTES_NONE) {
								uint32_t snum = *((uint32_t *) bytecode);
								lwc_string *he;
								css__stylesheet_string_get(style->sheet, snum, &he);
								ADVANCE(sizeof(snum));
								[result appendFormat:@" '%.*s' ",
												(int) lwc_string_length(he),
												lwc_string_data(he)];
								
								css__stylesheet_string_get(style->sheet, snum, &he);
								ADVANCE(sizeof(he));
								[result appendFormat:@" '%.*s' ",
												(int) lwc_string_length(he),
												lwc_string_data(he)];
								
								value = *((uint32_t *) bytecode);
								ADVANCE(sizeof(value));
							}
							break;
						case QUOTES_NONE:
							[result appendString:@"none"];
							break;
					}
					break;
				case CSS_PROP_SPEAK_HEADER:
					switch (value) {
						case SPEAK_HEADER_ONCE:
							[result appendString:@"once"];
							break;
						case SPEAK_HEADER_ALWAYS:
							[result appendString:@"always"];
							break;
					}
					break;
				case CSS_PROP_SPEAK_NUMERAL:
					switch (value) {
						case SPEAK_NUMERAL_DIGITS:
							[result appendString:@"digits"];
							break;
						case SPEAK_NUMERAL_CONTINUOUS:
							[result appendString:@"continuous"];
							break;
					}
					break;
				case CSS_PROP_SPEAK_PUNCTUATION:
					switch (value) {
						case SPEAK_PUNCTUATION_CODE:
							[result appendString:@"code"];
							break;
						case SPEAK_PUNCTUATION_NONE:
							[result appendString:@"none"];
							break;
					}
					break;
				case CSS_PROP_SPEAK:
					switch (value) {
						case SPEAK_NORMAL:
							[result appendString:@"normal"];
							break;
						case SPEAK_NONE:
							[result appendString:@"none"];
							break;
						case SPEAK_SPELL_OUT:
							[result appendString:@"spell-out"];
							break;
					}
					break;
				case CSS_PROP_SPEECH_RATE:
					switch (value) {
						case SPEECH_RATE_SET:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
						case SPEECH_RATE_X_SLOW:
							[result appendString:@"x-slow"];
							break;
						case SPEECH_RATE_SLOW:
							[result appendString:@"slow"];
							break;
						case SPEECH_RATE_MEDIUM:
							[result appendString:@"medium"];
							break;
						case SPEECH_RATE_FAST:
							[result appendString:@"fast"];
							break;
						case SPEECH_RATE_X_FAST:
							[result appendString:@"x-fast"];
							break;
						case SPEECH_RATE_FASTER:
							[result appendString:@"faster"];
							break;
						case SPEECH_RATE_SLOWER:
							[result appendString:@"slower"];
							break;
					}
					break;
				case CSS_PROP_TABLE_LAYOUT:
					switch (value) {
						case TABLE_LAYOUT_AUTO:
							[result appendString:@"auto"];
							break;
						case TABLE_LAYOUT_FIXED:
							[result appendString:@"fixed"];
							break;
					}
					break;
				case CSS_PROP_TEXT_ALIGN:
					switch (value) {
						case TEXT_ALIGN_LEFT:
							[result appendString:@"left"];
							break;
						case TEXT_ALIGN_RIGHT:
							[result appendString:@"right"];
							break;
						case TEXT_ALIGN_CENTER:
							[result appendString:@"center"];
							break;
						case TEXT_ALIGN_JUSTIFY:
							[result appendString:@"justify"];
							break;
						case TEXT_ALIGN_LIBCSS_LEFT:
							[result appendString:@"-libcss-left"];
							break;
						case TEXT_ALIGN_LIBCSS_CENTER:
							[result appendString:@"-libcss-center"];
							break;
						case TEXT_ALIGN_LIBCSS_RIGHT:
							[result appendString:@"-libcss-right"];
							break;
					}
					break;
				case CSS_PROP_TEXT_DECORATION:
					if (value == TEXT_DECORATION_NONE)
						[result appendString:@"none"];
					
					if (value & TEXT_DECORATION_UNDERLINE)
						[result appendString:@" underline"];
					if (value & TEXT_DECORATION_OVERLINE)
						[result appendString:@" overline"];
					if (value & TEXT_DECORATION_LINE_THROUGH)
						[result appendString:@" line-through"];
					if (value & TEXT_DECORATION_BLINK)
						[result appendString:@" blink"];
					break;
				case CSS_PROP_TEXT_TRANSFORM:
					switch (value) {
						case TEXT_TRANSFORM_CAPITALIZE:
							[result appendString:@"capitalize"];
							break;
						case TEXT_TRANSFORM_UPPERCASE:
							[result appendString:@"uppercase"];
							break;
						case TEXT_TRANSFORM_LOWERCASE:
							[result appendString:@"lowercase"];
							break;
						case TEXT_TRANSFORM_NONE:
							[result appendString:@"none"];
							break;
					}
					break;
				case CSS_PROP_UNICODE_BIDI:
					switch (value) {
						case UNICODE_BIDI_NORMAL:
							[result appendString:@"normal"];
							break;
						case UNICODE_BIDI_EMBED:
							[result appendString:@"embed"];
							break;
						case UNICODE_BIDI_BIDI_OVERRIDE:
							[result appendString:@"bidi-override"];
							break;
					}
					break;
				case CSS_PROP_VERTICAL_ALIGN:
					switch (value) {
						case VERTICAL_ALIGN_SET:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case VERTICAL_ALIGN_BASELINE:
							[result appendString:@"baseline"];
							break;
						case VERTICAL_ALIGN_SUB:
							[result appendString:@"sub"];
							break;
						case VERTICAL_ALIGN_SUPER:
							[result appendString:@"super"];
							break;
						case VERTICAL_ALIGN_TOP:
							[result appendString:@"top"];
							break;
						case VERTICAL_ALIGN_TEXT_TOP:
							[result appendString:@"text-top"];
							break;
						case VERTICAL_ALIGN_MIDDLE:
							[result appendString:@"middle"];
							break;
						case VERTICAL_ALIGN_BOTTOM:
							[result appendString:@"bottom"];
							break;
						case VERTICAL_ALIGN_TEXT_BOTTOM:
							[result appendString:@"text-bottom"];
							break;
					}
					break;
				case CSS_PROP_VISIBILITY:
					switch (value) {
						case VISIBILITY_VISIBLE:
							[result appendString:@"visible"];
							break;
						case VISIBILITY_HIDDEN:
							[result appendString:@"hidden"];
							break;
						case VISIBILITY_COLLAPSE:
							[result appendString:@"collapse"];
							break;
					}
					break;
				case CSS_PROP_VOICE_FAMILY:
					while (value != VOICE_FAMILY_END) {
						switch (value) {
							case VOICE_FAMILY_STRING:
							case VOICE_FAMILY_IDENT_LIST:
							{
								uint32_t snum = *((uint32_t *) bytecode);
								lwc_string *he;
								css__stylesheet_string_get(style->sheet, snum, &he);
								ADVANCE(sizeof(snum));
								[result appendFormat:@"'%.*s'",
												(int) lwc_string_length(he),
												lwc_string_data(he)];
							}
								break;
							case VOICE_FAMILY_MALE:
								[result appendString:@"male"];
								break;
							case VOICE_FAMILY_FEMALE:
								[result appendString:@"female"];
								break;
							case VOICE_FAMILY_CHILD:
								[result appendString:@"child"];
								break;
						}
						
						value = *((uint32_t *) bytecode);
						ADVANCE(sizeof(value));
						
						if (value != VOICE_FAMILY_END)
							[result appendString:@", "];
					}
					break;
				case CSS_PROP_VOLUME:
					switch (value) {
						case VOLUME_NUMBER:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
						case VOLUME_DIMENSION:
						{
							uint32_t unit;
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							unit = *((uint32_t *) bytecode);
							ADVANCE(sizeof(unit));
							[result appendString:dump_objc_css_unit(val, unit)];
						}
							break;
						case VOLUME_SILENT:
							[result appendString:@"silent"];
							break;
						case VOLUME_X_SOFT:
							[result appendString:@"x-soft"];
							break;
						case VOLUME_SOFT:
							[result appendString:@"soft"];
							break;
						case VOLUME_MEDIUM:
							[result appendString:@"medium"];
							break;
						case VOLUME_LOUD:
							[result appendString:@"loud"];
							break;
						case VOLUME_X_LOUD:
							[result appendString:@"x-loud"];
							break;
					}
					break;
				case CSS_PROP_WHITE_SPACE:
					switch (value) {
						case WHITE_SPACE_NORMAL:
							[result appendString:@"normal"];
							break;
						case WHITE_SPACE_PRE:
							[result appendString:@"pre"];
							break;
						case WHITE_SPACE_NOWRAP:
							[result appendString:@"nowrap"];
							break;
						case WHITE_SPACE_PRE_WRAP:
							[result appendString:@"pre-wrap"];
							break;
						case WHITE_SPACE_PRE_LINE:
							[result appendString:@"pre-line"];
							break;
					}
					break;
				case CSS_PROP_WRITING_MODE:
					switch (value) {
						case WRITING_MODE_HORIZONTAL_TB:
							[result appendString:@"horizontal-tb"];
							break;
						case WRITING_MODE_VERTICAL_RL:
							[result appendString:@"vertical-rl"];
							break;
						case WRITING_MODE_VERTICAL_LR:
							[result appendString:@"vertical-lr"];
							break;
					}
					break;
				case CSS_PROP_Z_INDEX:
					switch (value) {
						case Z_INDEX_SET:
						{
							css_fixed val = *((css_fixed *) bytecode);
							ADVANCE(sizeof(val));
							[result appendString:dump_objc_css_number(val)];
						}
							break;
						case Z_INDEX_AUTO:
							[result appendString:@"auto"];
							break;
					}
					break;
				default:
					[result appendFormat:@"Unknown opcode %x", op];
					return;
			}
		}
		
		if (isImportant(opv))
			[result appendString:@" !important"];
		
		[resultDict setObject:result forKey:opName];
	}
	
#undef ADVANCE
}

void dump_objc_string(lwc_string *string, NSMutableString *ptr)
{
	[ptr appendFormat:@"%.*s",
	 (int) lwc_string_length(string),
	 lwc_string_data(string)];
}

/*
void dump_objc_font_face(css_font_face *font_face, NSMutableDictionary *resultDict)
{
	uint8_t style, weight;
	
	if (font_face->font_family != NULL) {
		[resultDict setObject:[NSString stringWithFormat:@"%.*s",
						   (int) lwc_string_length(font_face->font_family),
						   lwc_string_data(font_face->font_family)] forKey:@"font-family"];
	}
	
	NSString *opKey = @"font-style";
	NSMutableString *result = [NSMutableString new];
	style = css_font_face_font_style(font_face);
	switch (style) {
		case CSS_FONT_STYLE_INHERIT:
			[result appendString:@"unspecified"];
			break;
		case CSS_FONT_STYLE_NORMAL:
			[result appendString:@"normal"];
			break;
		case CSS_FONT_STYLE_ITALIC:
			[result appendString:@"italic"];
			break;
		case CSS_FONT_STYLE_OBLIQUE:
			[result appendString:@"oblique"];
			break;
	}
	
	[resultDict setObject:result forKey:opKey];
	
	opKey = @"font-weight";
	result = [NSMutableString new];
	weight = css_font_face_font_weight(font_face);
	switch (weight) {
		case CSS_FONT_WEIGHT_INHERIT:
			[result appendString:@"unspecified"];
			break;
		case CSS_FONT_WEIGHT_NORMAL:
			[result appendString:@"normal"];
			break;
		case CSS_FONT_WEIGHT_BOLD:
			[result appendString:@"bold"];
			break;
		case CSS_FONT_WEIGHT_100:
			[result appendString:@"100"];
			break;
		case CSS_FONT_WEIGHT_200:
			[result appendString:@"200"];
			break;
		case CSS_FONT_WEIGHT_300:
			[result appendString:@"300"];
			break;
		case CSS_FONT_WEIGHT_400:
			[result appendString:@"400"];
			break;
		case CSS_FONT_WEIGHT_500:
			[result appendString:@"500"];
			break;
		case CSS_FONT_WEIGHT_600:
			[result appendString:@"600"];
			break;
		case CSS_FONT_WEIGHT_700:
			[result appendString:@"700"];
			break;
		case CSS_FONT_WEIGHT_800:
			[result appendString:@"800"];
			break;
		case CSS_FONT_WEIGHT_900:
			[result appendString:@"900"];
			break;
		default:
			[result appendFormat:@"Unhandled weight %d\n", (int)weight];
			break;
	}
	
	[resultDict setObject:result forKey:opKey];
	
	if (font_face->srcs != NULL) {
		uint32_t i;
		css_font_face_src *srcs = font_face->srcs;
		for (i = 0; i < font_face->n_srcs; ++i) {
			css_font_face_format format;
			
			opKey = @"format";
			result = [NSMutableString new];
			
			format = css_font_face_src_format(&srcs[i]);
			
			switch (format) {
				case CSS_FONT_FACE_FORMAT_UNSPECIFIED:
					[result appendString:@"unspecified"];
					break;
				case CSS_FONT_FACE_FORMAT_WOFF:
					[result appendString:@"WOFF"];
					break;
				case CSS_FONT_FACE_FORMAT_OPENTYPE:
					[result appendString:@"OTF"];
					break;
				case CSS_FONT_FACE_FORMAT_EMBEDDED_OPENTYPE:
					[result appendString:@"EOTF"];
					break;
				case CSS_FONT_FACE_FORMAT_SVG:
					[result appendString:@"SVG"];
					break;
				case CSS_FONT_FACE_FORMAT_UNKNOWN:
					[result appendString:@"unknown"];
					break;
				default:
					[result appendString:@"UNEXPECTED"];
					break;
			}
			
			[resultDict setObject:result forKey:opKey];
			
			if (srcs[i].location != NULL) {
				opKey = @"location";
				result = [NSMutableString new];
				
				switch (css_font_face_src_location_type(
														&srcs[i])) {
					case CSS_FONT_FACE_LOCATION_TYPE_LOCAL:
						[result appendString:@"local"];
						break;
					case CSS_FONT_FACE_LOCATION_TYPE_URI:
						[result appendString:@"url"];
						break;
					default:
						[result appendString:@"UNKNOWN"];
						break;
				}
				
				[result appendFormat:@"(%.*s)",
								(int) lwc_string_length(
														srcs[i].location),
								lwc_string_data(srcs[i].location)];
				
				[resultDict setObject:result forKey:opKey];
			}
		}
	}
}
 */

#endif
