#ifndef test_dump_objc_computed_h_
#define test_dump_objc_computed_h_

#include <libcss/computed.h>
#include <libcss/properties.h>
#include <libcss/types.h>

static NSString* dump_objc_css_fixed(css_fixed f)
{
	uint32_t uintpart = FIXTOINT(ABS(f));
	/* + 500 to ensure round to nearest (division will truncate) */
	uint32_t fracpart = ((ABS(f) & 0x3ff) * 1000 + 500) / (1 << 10);

	size_t flen = 0;
	char tmp[20];
	size_t tlen = 0;
	NSMutableString *buf = [NSMutableString new];
	
	if (f < 0) {
		[buf appendFormat:@"%c", '-'];
	}
	
	do {
		tmp[tlen] = "0123456789"[uintpart % 10];
		tlen++;
		
		uintpart /= 10;
	} while (tlen < 20 && uintpart != 0);
	
	while (tlen > 0) {
		[buf appendFormat:@"%c", tmp[--tlen]];
	}
	
	[buf appendFormat:@"%c", '.'];
	
	do {
		tmp[tlen] = "0123456789"[fracpart % 10];
		tlen++;
		
		fracpart /= 10;
	} while (tlen < 20 && fracpart != 0);
	
	while (tlen > 0) {
		[buf appendFormat:@"%c", tmp[--tlen]];
		flen++;
	}
	
	while (flen < 3) {
		[buf appendFormat:@"%c", '0'];
		flen++;
	}
	
	return buf;
}

static NSString * dump_objc_css_number(css_fixed val)
{
	if (INTTOFIX(FIXTOINT(val)) == val)
		return [NSString stringWithFormat:@"%d", FIXTOINT(val)];
	else
		return dump_objc_css_fixed(val);
}

static NSString *dump_objc_css_unit(css_fixed val, css_unit unit)
{
	NSMutableString *buf = [NSMutableString new];
	NSString *numberStr = dump_objc_css_number(val);
	[buf appendString:numberStr];
	
	switch (unit) {
		case CSS_UNIT_PX:
			[buf appendString:@"px"];
			break;
		case CSS_UNIT_EX:
			[buf appendString:@"ex"];
			break;
		case CSS_UNIT_EM:
			[buf appendString:@"em"];
			break;
		case CSS_UNIT_IN:
			[buf appendString:@"in"];
			break;
		case CSS_UNIT_CM:
			[buf appendString:@"cm"];
			break;
		case CSS_UNIT_MM:
			[buf appendString:@"mm"];
			break;
		case CSS_UNIT_PT:
			[buf appendString:@"pt"];
			break;
		case CSS_UNIT_PC:
			[buf appendString:@"pc"];
			break;
		case CSS_UNIT_CAP:
			[buf appendString:@"cap"];
			break;
		case CSS_UNIT_CH:
			[buf appendString:@"ch"];
			break;
		case CSS_UNIT_IC:
			[buf appendString:@"ic"];
			break;
		case CSS_UNIT_REM:
			[buf appendString:@"rem"];
			break;
		case CSS_UNIT_LH:
			[buf appendString:@"lh"];
			break;
		case CSS_UNIT_RLH:
			[buf appendString:@"rlh"];
			break;
		case CSS_UNIT_VH:
			[buf appendString:@"vh"];
			break;
		case CSS_UNIT_VW:
			[buf appendString:@"vw"];
			break;
		case CSS_UNIT_VI:
			[buf appendString:@"vi"];
			break;
		case CSS_UNIT_VB:
			[buf appendString:@"vb"];
			break;
		case CSS_UNIT_VMIN:
			[buf appendString:@"vmin"];
			break;
		case CSS_UNIT_VMAX:
			[buf appendString:@"vmax"];
			break;
		case CSS_UNIT_Q:
			[buf appendString:@"q"];
			break;
		case CSS_UNIT_PCT:
			[buf appendString:@"%%"];
			break;
		case CSS_UNIT_DEG:
			[buf appendString:@"deg"];
			break;
		case CSS_UNIT_GRAD:
			[buf appendString:@"grad"];
			break;
		case CSS_UNIT_RAD:
			[buf appendString:@"rad"];
			break;
		case CSS_UNIT_MS:
			[buf appendString:@"ms"];
			break;
		case CSS_UNIT_S:
			[buf appendString:@"s"];
			break;
		case CSS_UNIT_HZ:
			[buf appendString:@"Hz"];
			break;
		case CSS_UNIT_KHZ:
			[buf appendString:@"kHz"];
			break;
	}
	
	return buf;
}

static NSDictionary * dump_objc_computed_style(const css_computed_style *style)
{
	NSMutableDictionary *result = [NSMutableDictionary new];
	
	uint8_t val;
	css_color color = 0;
	lwc_string *url = NULL;
	css_fixed len1 = 0, len2 = 0;
	css_unit unit1 = CSS_UNIT_PX, unit2 = CSS_UNIT_PX;
	css_computed_clip_rect rect = { 0, 0, 0, 0, CSS_UNIT_PX, CSS_UNIT_PX,
		CSS_UNIT_PX, CSS_UNIT_PX, true, true,
		true, true };
	const css_computed_content_item *content = NULL;
	const css_computed_counter *counter = NULL;
	lwc_string **string_list = NULL;
	int32_t integer = 0;
	
	/* align-content */
	val = css_computed_align_content(style);
	switch (val) {
		case CSS_ALIGN_CONTENT_INHERIT:
			result[@"align-content"] = @"inherit";
			break;
		case CSS_ALIGN_CONTENT_STRETCH:
			result[@"align-content"] = @"stretch";
			break;
		case CSS_ALIGN_CONTENT_FLEX_START:
			result[@"align-content"] = @"flex-start";
			break;
		case CSS_ALIGN_CONTENT_FLEX_END:
			result[@"align-content"] = @"flex-end";
			break;
		case CSS_ALIGN_CONTENT_CENTER:
			result[@"align-content"] = @"center";
			break;
		case CSS_ALIGN_CONTENT_SPACE_BETWEEN:
			result[@"align-content"] = @"space-between";
			break;
		case CSS_ALIGN_CONTENT_SPACE_AROUND:
			result[@"align-content"] = @"space-around";
			break;
		case CSS_ALIGN_CONTENT_SPACE_EVENLY:
			result[@"align-content"] = @"space-evenly";
			break;
		default:
			break;
	}
	
	/* align-items */
	val = css_computed_align_items(style);
	switch (val) {
		case CSS_ALIGN_ITEMS_INHERIT:
			result[@"align-items"] = @"inherit";
			break;
		case CSS_ALIGN_ITEMS_STRETCH:
			result[@"align-items"] = @"stretch";
			break;
		case CSS_ALIGN_ITEMS_FLEX_START:
			result[@"align-items"] = @"flex-start";
			break;
		case CSS_ALIGN_ITEMS_FLEX_END:
			result[@"align-items"] = @"flex-end";
			break;
		case CSS_ALIGN_ITEMS_CENTER:
			result[@"align-items"] = @"center";
			break;
		case CSS_ALIGN_ITEMS_BASELINE:
			result[@"align-items"] = @"baseline";
			break;
		default:
			break;
	}
	
	/* align-self */
	val = css_computed_align_self(style);
	switch (val) {
		case CSS_ALIGN_SELF_INHERIT:
			result[@"align-self"] = @"inherit";
			break;
		case CSS_ALIGN_SELF_STRETCH:
			result[@"align-self"] = @"stretch";
			break;
		case CSS_ALIGN_SELF_FLEX_START:
			result[@"align-self"] = @"flex-start";
			break;
		case CSS_ALIGN_SELF_FLEX_END:
			result[@"align-self"] = @"flex-end";
			break;
		case CSS_ALIGN_SELF_CENTER:
			result[@"align-self"] = @"center";
			break;
		case CSS_ALIGN_SELF_BASELINE:
			result[@"align-self"] = @"baseline";
			break;
		case CSS_ALIGN_SELF_AUTO:
			result[@"align-self"] = @"auto";
			break;
		default:
			break;
	}
	
	/* background-attachment */
	val = css_computed_background_attachment(style);
	switch (val) {
		case CSS_BACKGROUND_ATTACHMENT_INHERIT:
			result[@"background-attachment"] = @"inherit";
			break;
		case CSS_BACKGROUND_ATTACHMENT_FIXED:
			result[@"background-attachment"] = @"fixed";
			break;
		case CSS_BACKGROUND_ATTACHMENT_SCROLL:
			result[@"background-attachment"] = @"scroll";
			break;
		default:
			break;
	}
	
	/* background-color */
	val = css_computed_background_color(style, &color);
	switch (val) {
		case CSS_BACKGROUND_COLOR_INHERIT:
			result[@"background-color"] = @"inherit";
			break;
		case CSS_BACKGROUND_COLOR_COLOR:
			result[@"background-color"] = [NSString stringWithFormat:@"#%08x", color];
			break;
		default:
			break;
	}
	
	/* background-image */
	val = css_computed_background_image(style, &url);
	if (val == CSS_BACKGROUND_IMAGE_INHERIT) {
		result[@"background-image"] = @"inherit";
	} else if (val == CSS_BACKGROUND_IMAGE_IMAGE && url != NULL) {
		result[@"background-image"] = [NSString stringWithFormat:@"url('%.*s')", (int) lwc_string_length(url), lwc_string_data(url)];
	} else if (val == CSS_BACKGROUND_IMAGE_NONE) {
		result[@"background-image"] = @"none";
	}
	
	/* background-position */
	val = css_computed_background_position(style, &len1, &unit1,
										   &len2, &unit2);
	if (val == CSS_BACKGROUND_POSITION_INHERIT) {
		result[@"background-position"] = @"inherit";
	} else if (val == CSS_BACKGROUND_POSITION_SET) {
		result[@"background-position"] = [NSString stringWithFormat:@"%@ %@", dump_objc_css_unit(len1, unit1), dump_objc_css_unit(len2, unit2)];
	}
	
	/* background-repeat */
	val = css_computed_background_repeat(style);
	switch (val) {
		case CSS_BACKGROUND_REPEAT_INHERIT:
			result[@"background-repeat"] = @"inherit";
			break;
		case CSS_BACKGROUND_REPEAT_REPEAT_X:
			result[@"background-repeat"] = @"repeat-x";
			break;
		case CSS_BACKGROUND_REPEAT_REPEAT_Y:
			result[@"background-repeat"] = @"repeat-y";
			break;
		case CSS_BACKGROUND_REPEAT_REPEAT:
			result[@"background-repeat"] = @"repeat";
			break;
		case CSS_BACKGROUND_REPEAT_NO_REPEAT:
			result[@"background-repeat"] = @"no-repeat";
			break;
		default:
			break;
	}
	
	/* border-collapse */
	val = css_computed_border_collapse(style);
	switch (val) {
		case CSS_BORDER_COLLAPSE_INHERIT:
			result[@"border-collapse"] = @"inherit";
			break;
		case CSS_BORDER_COLLAPSE_SEPARATE:
			result[@"border-collapse"] = @"separate";
			break;
		case CSS_BORDER_COLLAPSE_COLLAPSE:
			result[@"border-collapse"] = @"collapse";
			break;
		default:
			break;
	}
	
	/* border-spacing */
	val = css_computed_border_spacing(style, &len1, &unit1, &len2, &unit2);
	if (val == CSS_BORDER_SPACING_INHERIT) {
		result[@"border-spacing"] = @"inherit";
	} else if (val == CSS_BORDER_SPACING_SET) {
		result[@"border-spacing"] = [NSString stringWithFormat:@"%@ %@", dump_objc_css_unit(len1, unit1), dump_objc_css_unit(len2, unit2)];
	}
	
	/* border-top-color */
	val = css_computed_border_top_color(style, &color);
	switch (val) {
		case CSS_BORDER_COLOR_INHERIT:
			result[@"border-top-color"] = @"inherit";
			break;
		case CSS_BORDER_COLOR_CURRENT_COLOR:
			result[@"border-top-color"] = @"currentColor";
			break;
		case CSS_BORDER_COLOR_COLOR:
			result[@"border-top-color"] = [NSString stringWithFormat:@"#%08x", color];
			break;
		default:
			break;
	}
	
	/* border-right-color */
	val = css_computed_border_right_color(style, &color);
	switch (val) {
		case CSS_BORDER_COLOR_INHERIT:
			result[@"border-right-color"] = @"inherit";
			break;
		case CSS_BORDER_COLOR_CURRENT_COLOR:
			result[@"border-right-color"] = @"currentColor";
			break;
		case CSS_BORDER_COLOR_COLOR:
			result[@"border-right-color"] = [NSString stringWithFormat:@"#%08x", color];
			break;
		default:
			break;
	}
	
	/* border-bottom-color */
	val = css_computed_border_bottom_color(style, &color);
	switch (val) {
		case CSS_BORDER_COLOR_INHERIT:
			result[@"border-bottom-color"] = @"inherit";
			break;
		case CSS_BORDER_COLOR_CURRENT_COLOR:
			result[@"border-bottom-color"] = @"currentColor";
			break;
		case CSS_BORDER_COLOR_COLOR:
			result[@"border-bottom-color"] = [NSString stringWithFormat:@"#%08x", color];
			break;
		default:
			break;
	}
	
	/* border-left-color */
	val = css_computed_border_left_color(style, &color);
	switch (val) {
		case CSS_BORDER_COLOR_INHERIT:
			result[@"border-left-color"] = @"inherit";
			break;
		case CSS_BORDER_COLOR_CURRENT_COLOR:
			result[@"border-left-color"] = @"currentColor";
			break;
		case CSS_BORDER_COLOR_COLOR:
			result[@"border-left-color"] = [NSString stringWithFormat:@"#%08x", color];
			break;
		default:
			break;
	}
	
	/* border-top-style */
	val = css_computed_border_top_style(style);
	switch (val) {
		case CSS_BORDER_STYLE_INHERIT:
			result[@"border-top-style"] = @"inherit";
			break;
		case CSS_BORDER_STYLE_NONE:
			result[@"border-top-style"] = @"none";
			break;
		case CSS_BORDER_STYLE_HIDDEN:
			result[@"border-top-style"] = @"hidden";
			break;
		case CSS_BORDER_STYLE_DOTTED:
			result[@"border-top-style"] = @"dotted";
			break;
		case CSS_BORDER_STYLE_DASHED:
			result[@"border-top-style"] = @"dashed";
			break;
		case CSS_BORDER_STYLE_SOLID:
			result[@"border-top-style"] = @"solid";
			break;
		case CSS_BORDER_STYLE_DOUBLE:
			result[@"border-top-style"] = @"double";
			break;
		case CSS_BORDER_STYLE_GROOVE:
			result[@"border-top-style"] = @"groove";
			break;
		case CSS_BORDER_STYLE_RIDGE:
			result[@"border-top-style"] = @"ridge";
			break;
		case CSS_BORDER_STYLE_INSET:
			result[@"border-top-style"] = @"inset";
			break;
		case CSS_BORDER_STYLE_OUTSET:
			result[@"border-top-style"] = @"outset";
			break;
		default:
			break;
	}
	
	/* border-right-style */
	val = css_computed_border_right_style(style);
	switch (val) {
		case CSS_BORDER_STYLE_INHERIT:
			result[@"border-right-style"] = @"inherit";
			break;
		case CSS_BORDER_STYLE_NONE:
			result[@"border-right-style"] = @"none";
			break;
		case CSS_BORDER_STYLE_HIDDEN:
			result[@"border-right-style"] = @"hidden";
			break;
		case CSS_BORDER_STYLE_DOTTED:
			result[@"border-right-style"] = @"dotted";
			break;
		case CSS_BORDER_STYLE_DASHED:
			result[@"border-right-style"] = @"dashed";
			break;
		case CSS_BORDER_STYLE_SOLID:
			result[@"border-right-style"] = @"solid";
			break;
		case CSS_BORDER_STYLE_DOUBLE:
			result[@"border-right-style"] = @"double";
			break;
		case CSS_BORDER_STYLE_GROOVE:
			result[@"border-right-style"] = @"groove";
			break;
		case CSS_BORDER_STYLE_RIDGE:
			result[@"border-right-style"] = @"ridge";
			break;
		case CSS_BORDER_STYLE_INSET:
			result[@"border-right-style"] = @"inset";
			break;
		case CSS_BORDER_STYLE_OUTSET:
			result[@"border-right-style"] = @"outset";
			break;
		default:
			break;
	}
	
	/* border-bottom-style */
	val = css_computed_border_bottom_style(style);
	switch (val) {
		case CSS_BORDER_STYLE_INHERIT:
			result[@"border-bottom-style"] = @"inherit";
			break;
		case CSS_BORDER_STYLE_NONE:
			result[@"border-bottom-style"] = @"none";
			break;
		case CSS_BORDER_STYLE_HIDDEN:
			result[@"border-bottom-style"] = @"hidden";
			break;
		case CSS_BORDER_STYLE_DOTTED:
			result[@"border-bottom-style"] = @"dotted";
			break;
		case CSS_BORDER_STYLE_DASHED:
			result[@"border-bottom-style"] = @"dashed";
			break;
		case CSS_BORDER_STYLE_SOLID:
			result[@"border-bottom-style"] = @"solid";
			break;
		case CSS_BORDER_STYLE_DOUBLE:
			result[@"border-bottom-style"] = @"double";
			break;
		case CSS_BORDER_STYLE_GROOVE:
			result[@"border-bottom-style"] = @"groove";
			break;
		case CSS_BORDER_STYLE_RIDGE:
			result[@"border-bottom-style"] = @"ridge";
			break;
		case CSS_BORDER_STYLE_INSET:
			result[@"border-bottom-style"] = @"inset";
			break;
		case CSS_BORDER_STYLE_OUTSET:
			result[@"border-bottom-style"] = @"outset";
			break;
		default:
			break;
	}
	
	/* border-left-style */
	val = css_computed_border_left_style(style);
	switch (val) {
		case CSS_BORDER_STYLE_INHERIT:
			result[@"border-left-style"] = @"inherit";
			break;
		case CSS_BORDER_STYLE_NONE:
			result[@"border-left-style"] = @"none";
			break;
		case CSS_BORDER_STYLE_HIDDEN:
			result[@"border-left-style"] = @"hidden";
			break;
		case CSS_BORDER_STYLE_DOTTED:
			result[@"border-left-style"] = @"dotted";
			break;
		case CSS_BORDER_STYLE_DASHED:
			result[@"border-left-style"] = @"dashed";
			break;
		case CSS_BORDER_STYLE_SOLID:
			result[@"border-left-style"] = @"solid";
			break;
		case CSS_BORDER_STYLE_DOUBLE:
			result[@"border-left-style"] = @"double";
			break;
		case CSS_BORDER_STYLE_GROOVE:
			result[@"border-left-style"] = @"groove";
			break;
		case CSS_BORDER_STYLE_RIDGE:
			result[@"border-left-style"] = @"ridge";
			break;
		case CSS_BORDER_STYLE_INSET:
			result[@"border-left-style"] = @"inset";
			break;
		case CSS_BORDER_STYLE_OUTSET:
			result[@"border-left-style"] = @"outset";
			break;
		default:
			break;
	}
	
	/* border-top-width */
	val = css_computed_border_top_width(style, &len1, &unit1);
	switch (val) {
		case CSS_BORDER_WIDTH_INHERIT:
			result[@"border-top-width"] = @"inherit";
			break;
		case CSS_BORDER_WIDTH_THIN:
			result[@"border-top-width"] = @"thin";
			break;
		case CSS_BORDER_WIDTH_MEDIUM:
			result[@"border-top-width"] = @"medium";
			break;
		case CSS_BORDER_WIDTH_THICK:
			result[@"border-top-width"] = @"thick";
			break;
		case CSS_BORDER_WIDTH_WIDTH:
			result[@"border-top-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* border-right-width */
	val = css_computed_border_right_width(style, &len1, &unit1);
	switch (val) {
		case CSS_BORDER_WIDTH_INHERIT:
			result[@"border-right-width"] = @"inherit";
			break;
		case CSS_BORDER_WIDTH_THIN:
			result[@"border-right-width"] = @"thin";
			break;
		case CSS_BORDER_WIDTH_MEDIUM:
			result[@"border-right-width"] = @"medium";
			break;
		case CSS_BORDER_WIDTH_THICK:
			result[@"border-right-width"] = @"thick";
			break;
		case CSS_BORDER_WIDTH_WIDTH:
			result[@"border-right-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* border-bottom-width */
	val = css_computed_border_bottom_width(style, &len1, &unit1);
	switch (val) {
		case CSS_BORDER_WIDTH_INHERIT:
			result[@"border-bottom-width"] = @"inherit";
			break;
		case CSS_BORDER_WIDTH_THIN:
			result[@"border-bottom-width"] = @"thin";
			break;
		case CSS_BORDER_WIDTH_MEDIUM:
			result[@"border-bottom-width"] = @"medium";
			break;
		case CSS_BORDER_WIDTH_THICK:
			result[@"border-bottom-width"] = @"thick";
			break;
		case CSS_BORDER_WIDTH_WIDTH:
			result[@"border-bottom-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* border-left-width */
	val = css_computed_border_left_width(style, &len1, &unit1);
	switch (val) {
		case CSS_BORDER_WIDTH_INHERIT:
			result[@"border-left-width"] = @"inherit";
			break;
		case CSS_BORDER_WIDTH_THIN:
			result[@"border-left-width"] = @"thin";
			break;
		case CSS_BORDER_WIDTH_MEDIUM:
			result[@"border-left-width"] = @"medium";
			break;
		case CSS_BORDER_WIDTH_THICK:
			result[@"border-left-width"] = @"thick";
			break;
		case CSS_BORDER_WIDTH_WIDTH:
			result[@"border-left-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* bottom */
	val = css_computed_bottom(style, &len1, &unit1);
	switch (val) {
		case CSS_BOTTOM_INHERIT:
			result[@"bottom"] = @"inherit";
			break;
		case CSS_BOTTOM_AUTO:
			result[@"bottom"] = @"auto";
			break;
		case CSS_BOTTOM_SET:
			result[@"bottom"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* box-sizing */
	val = css_computed_box_sizing(style);
	switch (val) {
		case CSS_BOX_SIZING_INHERIT:
			result[@"box-sizing"] = @"inherit";
			break;
		case CSS_BOX_SIZING_CONTENT_BOX:
			result[@"box-sizing"] = @"content-box";
			break;
		case CSS_BOX_SIZING_BORDER_BOX:
			result[@"box-sizing"] = @"border-box";
			break;
		default:
			printf("DISASTER!\n");
			assert(0);
			break;
	}
	
	/* break-after */
	val = css_computed_break_after(style);
	switch (val) {
		case CSS_BREAK_AFTER_INHERIT:
			result[@"break-after"] = @"inherit";
			break;
		case CSS_BREAK_AFTER_AUTO:
			result[@"break-after"] = @"auto";
			break;
		case CSS_BREAK_AFTER_ALWAYS:
			result[@"break-after"] = @"always";
			break;
		case CSS_BREAK_AFTER_AVOID:
			result[@"break-after"] = @"avoid";
			break;
		case CSS_BREAK_AFTER_LEFT:
			result[@"break-after"] = @"left";
			break;
		case CSS_BREAK_AFTER_RIGHT:
			result[@"break-after"] = @"right";
			break;
		case CSS_BREAK_AFTER_PAGE:
			result[@"break-after"] = @"page";
			break;
		case CSS_BREAK_AFTER_COLUMN:
			result[@"break-after"] = @"column";
			break;
		case CSS_BREAK_AFTER_AVOID_PAGE:
			result[@"break-after"] = @"avoid-page";
			break;
		case CSS_BREAK_AFTER_AVOID_COLUMN:
			result[@"break-after"] = @"avoid-column";
			break;
		default:
			break;
	}
	
	/* break-before */
	val = css_computed_break_before(style);
	switch (val) {
		case CSS_BREAK_BEFORE_INHERIT:
			result[@"break-before"] = @"inherit";
			break;
		case CSS_BREAK_BEFORE_AUTO:
			result[@"break-before"] = @"auto";
			break;
		case CSS_BREAK_BEFORE_ALWAYS:
			result[@"break-before"] = @"always";
			break;
		case CSS_BREAK_BEFORE_AVOID:
			result[@"break-before"] = @"avoid";
			break;
		case CSS_BREAK_BEFORE_LEFT:
			result[@"break-before"] = @"left";
			break;
		case CSS_BREAK_BEFORE_RIGHT:
			result[@"break-before"] = @"right";
			break;
		case CSS_BREAK_BEFORE_PAGE:
			result[@"break-before"] = @"page";
			break;
		case CSS_BREAK_BEFORE_COLUMN:
			result[@"break-before"] = @"column";
			break;
		case CSS_BREAK_BEFORE_AVOID_PAGE:
			result[@"break-before"] = @"avoid-page";
			break;
		case CSS_BREAK_BEFORE_AVOID_COLUMN:
			result[@"break-before"] = @"avoid-column";
			break;
		default:
			break;
	}
	
	/* break-inside */
	val = css_computed_break_inside(style);
	switch (val) {
		case CSS_BREAK_INSIDE_INHERIT:
			result[@"break-inside"] = @"inherit";
			break;
		case CSS_BREAK_INSIDE_AUTO:
			result[@"break-inside"] = @"auto";
			break;
		case CSS_BREAK_INSIDE_AVOID:
			result[@"break-inside"] = @"avoid";
			break;
		case CSS_BREAK_INSIDE_AVOID_PAGE:
			result[@"break-inside"] = @"avoid-page";
			break;
		case CSS_BREAK_INSIDE_AVOID_COLUMN:
			result[@"break-inside"] = @"avoid-column";
			break;
		default:
			break;
	}
	
	/* caption-side */
	val = css_computed_caption_side(style);
	switch (val) {
		case CSS_CAPTION_SIDE_INHERIT:
			result[@"caption-side"] = @"inherit";
			break;
		case CSS_CAPTION_SIDE_TOP:
			result[@"caption-side"] = @"top";
			break;
		case CSS_CAPTION_SIDE_BOTTOM:
			result[@"caption-side"] = @"bottom";
			break;
		default:
			break;
	}
	
	/* clear */
	val = css_computed_clear(style);
	switch (val) {
		case CSS_CLEAR_INHERIT:
			result[@"clear"] = @"inherit";
			break;
		case CSS_CLEAR_NONE:
			result[@"clear"] = @"none";
			break;
		case CSS_CLEAR_LEFT:
			result[@"clear"] = @"left";
			break;
		case CSS_CLEAR_RIGHT:
			result[@"clear"] = @"right";
			break;
		case CSS_CLEAR_BOTH:
			result[@"clear"] = @"both";
			break;
		default:
			break;
	}
	
	/* clip */
	val = css_computed_clip(style, &rect);
	switch (val) {
		case CSS_CLIP_INHERIT:
			result[@"clip"] = @"inherit";
			break;
		case CSS_CLIP_AUTO:
			result[@"clip"] = @"auto";
			break;
		case CSS_CLIP_RECT:
		{
			NSMutableString *ptr = [NSMutableString new];
			
			[ptr appendString:@"clip: rect( "];
			if (rect.top_auto)
				[ptr appendString:@"auto"];
			else
				[ptr appendString:dump_objc_css_unit(rect.top, rect.tunit)];
			
			[ptr appendString:@", "];
			
			if (rect.right_auto)
				[ptr appendString:@"auto"];
			else
				[ptr appendString:dump_objc_css_unit(rect.right, rect.runit)];
			
			[ptr appendString:@", "];
			
			if (rect.bottom_auto)
				[ptr appendString:@"auto"];
			else
				[ptr appendString:dump_objc_css_unit(rect.bottom, rect.bunit)];
			
			[ptr appendString:@", "];
			
			if (rect.left_auto)
				[ptr appendString:@"auto"];
			else
				[ptr appendString:dump_objc_css_unit(rect.left, rect.lunit)];
			
			[ptr appendString:@")"];
			
			result[@"clip"] = ptr;
		}
			break;
		default:
			break;
	}
	
	/* color */
	val = css_computed_color(style, &color);
	if (val == CSS_COLOR_INHERIT) {
		result[@"color"] = @"inherit";
	} else if (val == CSS_COLOR_COLOR) {
		result[@"color"] = [NSString stringWithFormat:@"#%08x", color];
	}
	
	/* column-count */
	val = css_computed_column_count(style, &integer);
	switch (val) {
		case CSS_COLUMN_COUNT_INHERIT:
			result[@"column-count"] = @"inherit";
			break;
		case CSS_COLUMN_COUNT_AUTO:
			result[@"column-count"] = @"auto";
			break;
		case CSS_COLUMN_COUNT_SET:
			result[@"column-count"] = [NSString stringWithFormat:@"%d", integer];
			break;
		default:
			break;
	}
	
	/* column-fill */
	val = css_computed_column_fill(style);
	switch (val) {
		case CSS_COLUMN_FILL_INHERIT:
			result[@"column-fill"] = @"inherit";
			break;
		case CSS_COLUMN_FILL_AUTO:
			result[@"column-fill"] = @"auto";
			break;
		case CSS_COLUMN_FILL_BALANCE:
			result[@"column-fill"] = @"balance";
			break;
		default:
			break;
	}
	
	/* column-gap */
	val = css_computed_column_gap(style, &len1, &unit1);
	switch (val) {
		case CSS_COLUMN_GAP_INHERIT:
			result[@"column-gap"] = @"inherit";
			break;
		case CSS_COLUMN_GAP_NORMAL:
			result[@"column-gap"] = @"normal";
			break;
		case CSS_COLUMN_GAP_SET:
			result[@"column-gap"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* column-rule-color */
	val = css_computed_column_rule_color(style, &color);
	switch (val) {
		case CSS_COLUMN_RULE_COLOR_INHERIT:
			result[@"column-rule-color"] = @"inherit";
			break;
		case CSS_COLUMN_RULE_COLOR_CURRENT_COLOR:
			result[@"column-rule-color"] = @"currentColor";
			break;
		case CSS_COLUMN_RULE_COLOR_COLOR:
			result[@"column-rule-color"] = [NSString stringWithFormat:@"#%08x", color];
			break;
	}
	
	/* column-rule-style */
	val = css_computed_column_rule_style(style);
	switch (val) {
		case CSS_COLUMN_RULE_STYLE_INHERIT:
			result[@"column-rule-style"] = @"inherit";
			break;
		case CSS_COLUMN_RULE_STYLE_NONE:
			result[@"column-rule-style"] = @"none";
			break;
		case CSS_COLUMN_RULE_STYLE_HIDDEN:
			result[@"column-rule-style"] = @"hidden";
			break;
		case CSS_COLUMN_RULE_STYLE_DOTTED:
			result[@"column-rule-style"] = @"dotted";
			break;
		case CSS_COLUMN_RULE_STYLE_DASHED:
			result[@"column-rule-style"] = @"dashed";
			break;
		case CSS_COLUMN_RULE_STYLE_SOLID:
			result[@"column-rule-style"] = @"solid";
			break;
		case CSS_COLUMN_RULE_STYLE_DOUBLE:
			result[@"column-rule-style"] = @"double";
			break;
		case CSS_COLUMN_RULE_STYLE_GROOVE:
			result[@"column-rule-style"] = @"groove";
			break;
		case CSS_COLUMN_RULE_STYLE_RIDGE:
			result[@"column-rule-style"] = @"ridge";
			break;
		case CSS_COLUMN_RULE_STYLE_INSET:
			result[@"column-rule-style"] = @"inset";
			break;
		case CSS_COLUMN_RULE_STYLE_OUTSET:
			result[@"column-rule-style"] = @"outset";
			break;
		default:
			break;
	}
	
	/* column-rule-width */
	val = css_computed_column_rule_width(style, &len1, &unit1);
	switch (val) {
		case CSS_COLUMN_RULE_WIDTH_INHERIT:
			result[@"column-rule-width"] = @"inherit";
			break;
		case CSS_COLUMN_RULE_WIDTH_THIN:
			result[@"column-rule-width"] = @"thin";
			break;
		case CSS_COLUMN_RULE_WIDTH_MEDIUM:
			result[@"column-rule-width"] = @"medium";
			break;
		case CSS_COLUMN_RULE_WIDTH_THICK:
			result[@"column-rule-width"] = @"thick";
			break;
		case CSS_COLUMN_RULE_WIDTH_WIDTH:
			result[@"column-rule-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* column-span */
	val = css_computed_column_span(style);
	switch (val) {
		case CSS_COLUMN_SPAN_INHERIT:
			result[@"column-span"] = @"inherit";
			break;
		case CSS_COLUMN_SPAN_NONE:
			result[@"column-span"] = @"none";
			break;
		case CSS_COLUMN_SPAN_ALL:
			result[@"column-span"] = @"all";
			break;
		default:
			break;
	}
	
	/* column-width */
	val = css_computed_column_width(style, &len1, &unit1);
	switch (val) {
		case CSS_COLUMN_WIDTH_INHERIT:
			result[@"column-width"] = @"inherit";
			break;
		case CSS_COLUMN_WIDTH_AUTO:
			result[@"column-width"] = @"auto";
			break;
		case CSS_COLUMN_WIDTH_SET:
			result[@"column-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* content */
	val = css_computed_content(style, &content);
	switch (val) {
		case CSS_CONTENT_INHERIT:
			result[@"content"] = @"inherit";
			break;
		case CSS_CONTENT_NONE:
			result[@"content"] = @"none";
			break;
		case CSS_CONTENT_NORMAL:
			result[@"content"] = @"normal";
			break;
		case CSS_CONTENT_SET:
		{
			NSMutableString *ptr = [NSMutableString new];
			result[@"content"] = ptr;
			
			while (content->type != CSS_COMPUTED_CONTENT_NONE) {
				[ptr appendString:@" "];
				
				switch (content->type) {
					case CSS_COMPUTED_CONTENT_STRING:
						[ptr appendFormat:@"\"%.*s\"",
						 (int) lwc_string_length(content->data.string),
						 lwc_string_data(content->data.string)];
						break;
					case CSS_COMPUTED_CONTENT_URI:
						[ptr appendFormat:@"uri(\"%.*s\")",
						 (int) lwc_string_length(content->data.uri),
						 lwc_string_data(content->data.uri)];
						break;
					case CSS_COMPUTED_CONTENT_COUNTER:
						[ptr appendFormat:@"counter(%.*s)",
						 (int) lwc_string_length(content->data.counter.name),
						 lwc_string_data(content->data.counter.name)];
						break;
					case CSS_COMPUTED_CONTENT_COUNTERS:
						[ptr appendFormat:@"counters(%.*s, "
						 "\"%.*s\")",
						 (int) lwc_string_length(content->data.counters.name),
						 lwc_string_data(content->data.counters.name),
						 (int) lwc_string_length(content->data.counters.sep),
						 lwc_string_data(content->data.counters.sep)];
						break;
					case CSS_COMPUTED_CONTENT_ATTR:
						[ptr appendFormat:@"attr(%.*s)",
						 (int) lwc_string_length(content->data.attr),
						 lwc_string_data(content->data.attr)];
						break;
					case CSS_COMPUTED_CONTENT_OPEN_QUOTE:
						[ptr appendString:@"open-quote"];
						break;
					case CSS_COMPUTED_CONTENT_CLOSE_QUOTE:
						[ptr appendString:@"close-quote"];
						break;
					case CSS_COMPUTED_CONTENT_NO_OPEN_QUOTE:
						[ptr appendString:@"no-open-quote"];
						break;
					case CSS_COMPUTED_CONTENT_NO_CLOSE_QUOTE:
						[ptr appendString:@"no-close-quote"];
						break;
				}
				
				content++;
			}
		}
			break;
		default:
			break;
	}
	
	/* counter-increment */
	val = css_computed_counter_increment(style, &counter);
	if (val == CSS_COUNTER_INCREMENT_INHERIT) {
		result[@"counter-increment"] = @"inherit";
	} else if (counter == NULL) {
		result[@"counter-increment"] = @"none";
	} else {
		NSMutableString *ptr = [NSMutableString new];
		result[@"counter-increment"] = ptr;
		
		while (counter->name != NULL) {
			[ptr appendFormat:@" %.*s ", (int) lwc_string_length(counter->name),
			 lwc_string_data(counter->name)];
			[ptr appendString: dump_objc_css_fixed(counter->value)];
			counter++;
		}
	}
	
	/* counter-reset */
	val = css_computed_counter_reset(style, &counter);
	if (val == CSS_COUNTER_RESET_INHERIT) {
		result[@"counter-reset"] = @"inherit";
	} else if (counter == NULL) {
		result[@"counter-reset"] = @"none";
	} else {
		NSMutableString *ptr = [NSMutableString new];
		result[@"counter-reset"] = ptr;
		
		while (counter->name != NULL) {
			[ptr appendFormat:@" %.*s ", (int) lwc_string_length(counter->name),
			 lwc_string_data(counter->name)];
			[ptr appendString: dump_objc_css_fixed(counter->value)];
			
			counter++;
		}
	}
	
	/* cursor */
	val = css_computed_cursor(style, &string_list);
	
	NSMutableString *ptr = [NSMutableString new];
	result[@"cursor"] = ptr;
	
	if (string_list != NULL) {
		while (*string_list != NULL) {
			[ptr appendFormat:@" url('%.*s')",
			 (int) lwc_string_length(*string_list),
			 lwc_string_data(*string_list)];
			
			string_list++;
		}
	}
	
	switch (val) {
		case CSS_CURSOR_INHERIT:
			[ptr appendString:@" inherit"];
			break;
		case CSS_CURSOR_AUTO:
			[ptr appendString:@" auto"];
			break;
		case CSS_CURSOR_CROSSHAIR:
			[ptr appendString:@" crosshair"];
			break;
		case CSS_CURSOR_DEFAULT:
			[ptr appendString:@" default"];
			break;
		case CSS_CURSOR_POINTER:
			[ptr appendString:@" pointer"];
			break;
		case CSS_CURSOR_MOVE:
			[ptr appendString:@" move"];
			break;
		case CSS_CURSOR_E_RESIZE:
			[ptr appendString:@" e-resize"];
			break;
		case CSS_CURSOR_NE_RESIZE:
			[ptr appendString:@" ne-resize"];
			break;
		case CSS_CURSOR_NW_RESIZE:
			[ptr appendString:@" nw-resize"];
			break;
		case CSS_CURSOR_N_RESIZE:
			[ptr appendString:@" n-resize"];
			break;
		case CSS_CURSOR_SE_RESIZE:
			[ptr appendString:@" se-resize"];
			break;
		case CSS_CURSOR_SW_RESIZE:
			[ptr appendString:@" sw-resize"];
			break;
		case CSS_CURSOR_S_RESIZE:
			[ptr appendString:@" s-resize"];
			break;
		case CSS_CURSOR_W_RESIZE:
			[ptr appendString:@" w-resize"];
			break;
		case CSS_CURSOR_TEXT:
			[ptr appendString:@" text"];
			break;
		case CSS_CURSOR_WAIT:
			[ptr appendString:@" wait"];
			break;
		case CSS_CURSOR_HELP:
			[ptr appendString:@" help"];
			break;
		case CSS_CURSOR_PROGRESS:
			[ptr appendString:@" progress"];
			break;
		default:
			break;
	}
	
	/* direction */
	val = css_computed_direction(style);
	switch (val) {
		case CSS_DIRECTION_INHERIT:
			result[@"direction"] = @"inherit";
			break;
		case CSS_DIRECTION_LTR:
			result[@"direction"] = @"ltr";
			break;
		case CSS_DIRECTION_RTL:
			result[@"direction"] = @"rtl";
			break;
		default:
			break;
	}
	
	/* display */
	val = css_computed_display_static(style);
	switch (val) {
		case CSS_DISPLAY_INHERIT:
			result[@"display"] = @"inherit";
			break;
		case CSS_DISPLAY_INLINE:
			result[@"display"] = @"inline";
			break;
		case CSS_DISPLAY_BLOCK:
			result[@"display"] = @"block";
			break;
		case CSS_DISPLAY_LIST_ITEM:
			result[@"display"] = @"list-item";
			break;
		case CSS_DISPLAY_RUN_IN:
			result[@"display"] = @"run-in";
			break;
		case CSS_DISPLAY_INLINE_BLOCK:
			result[@"display"] = @"inline-block";
			break;
		case CSS_DISPLAY_TABLE:
			result[@"display"] = @"table";
			break;
		case CSS_DISPLAY_INLINE_TABLE:
			result[@"display"] = @"inline-table";
			break;
		case CSS_DISPLAY_TABLE_ROW_GROUP:
			result[@"display"] = @"table-row-group";
			break;
		case CSS_DISPLAY_TABLE_HEADER_GROUP:
			result[@"display"] = @"table-header-group";
			break;
		case CSS_DISPLAY_TABLE_FOOTER_GROUP:
			result[@"display"] = @"table-footer-group";
			break;
		case CSS_DISPLAY_TABLE_ROW:
			result[@"display"] = @"table-row";
			break;
		case CSS_DISPLAY_TABLE_COLUMN_GROUP:
			result[@"display"] = @"table-column-group";
			break;
		case CSS_DISPLAY_TABLE_COLUMN:
			result[@"display"] = @"table-column";
			break;
		case CSS_DISPLAY_TABLE_CELL:
			result[@"display"] = @"table-cell";
			break;
		case CSS_DISPLAY_TABLE_CAPTION:
			result[@"display"] = @"table-caption";
			break;
		case CSS_DISPLAY_NONE:
			result[@"display"] = @"none";
			break;
		case CSS_DISPLAY_FLEX:
			result[@"display"] = @"flex";
			break;
		case CSS_DISPLAY_INLINE_FLEX:
			result[@"display"] = @"inline-flex";
			break;
		default:
			break;
	}
	
	/* empty-cells */
	val = css_computed_empty_cells(style);
	switch (val) {
		case CSS_EMPTY_CELLS_INHERIT:
			result[@"empty-cells"] = @"inherit";
			break;
		case CSS_EMPTY_CELLS_SHOW:
			result[@"empty-cells"] = @"show";
			break;
		case CSS_EMPTY_CELLS_HIDE:
			result[@"empty-cells"] = @"hide";
			break;
		default:
			break;
	}
	
	/* flex-basis */
	val = css_computed_flex_basis(style, &len1, &unit1);
	switch (val) {
		case CSS_FLEX_BASIS_INHERIT:
			result[@"flex-basis"] = @"inherit";
			break;
		case CSS_FLEX_BASIS_AUTO:
			result[@"flex-basis"] = @"auto";
			break;
		case CSS_FLEX_BASIS_CONTENT:
			result[@"flex-basis"] = @"content";
			break;
		case CSS_FLEX_BASIS_SET:
			result[@"flex-basis"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* flex-direction */
	val = css_computed_flex_direction(style);
	switch (val) {
		case CSS_FLEX_DIRECTION_INHERIT:
			result[@"flex-direction"] = @"inherit";
			break;
		case CSS_FLEX_DIRECTION_ROW:
			result[@"flex-direction"] = @"row";
			break;
		case CSS_FLEX_DIRECTION_ROW_REVERSE:
			result[@"flex-direction"] = @"row-reverse";
			break;
		case CSS_FLEX_DIRECTION_COLUMN:
			result[@"flex-direction"] = @"column";
			break;
		case CSS_FLEX_DIRECTION_COLUMN_REVERSE:
			result[@"flex-direction"] = @"column-reverse";
			break;
		default:
			break;
	}
	
	/* flex-grow */
	val = css_computed_flex_grow(style, &len1);
	switch (val) {
		case CSS_FLEX_GROW_INHERIT:
			result[@"flex-grow"] = @"inherit";
			break;
		case CSS_FLEX_GROW_SET:
			result[@"flex-grow"] = dump_objc_css_fixed(len1);
			break;
		default:
			break;
	}
	
	/* flex-shrink */
	val = css_computed_flex_shrink(style, &len1);
	switch (val) {
		case CSS_FLEX_SHRINK_INHERIT:
			result[@"flex-shrink"] = @"inherit";
			break;
		case CSS_FLEX_SHRINK_SET:
			result[@"flex-shrink"] = dump_objc_css_fixed(len1);
			break;
		default:
			break;
	}
	
	/* flex-wrap */
	val = css_computed_flex_wrap(style);
	switch (val) {
		case CSS_FLEX_WRAP_INHERIT:
			result[@"flex-wrap"] = @"inherit";
			break;
		case CSS_FLEX_WRAP_NOWRAP:
			result[@"flex-wrap"] = @"nowrap";
			break;
		case CSS_FLEX_WRAP_WRAP:
			result[@"flex-wrap"] = @"wrap";
			break;
		case CSS_FLEX_WRAP_WRAP_REVERSE:
			result[@"flex-wrap"] = @"wrap-reverse";
			break;
		default:
			break;
	}
	
	/* float */
	val = css_computed_float(style);
	switch (val) {
		case CSS_FLOAT_INHERIT:
			result[@"float"] = @"inherit";
			break;
		case CSS_FLOAT_LEFT:
			result[@"float"] = @"left";
			break;
		case CSS_FLOAT_RIGHT:
			result[@"float"] = @"right";
			break;
		case CSS_FLOAT_NONE:
			result[@"float"] = @"none";
			break;
		default:
			break;
	}
	
	/* font-family */
	val = css_computed_font_family(style, &string_list);
	if (val == CSS_FONT_FAMILY_INHERIT) {
		result[@"font-family"] = @"inherit";
	} else {
		NSMutableString *ptr = [NSMutableString new];
		result[@"font-family"] = ptr;
		
		if (string_list != NULL) {
			while (*string_list != NULL) {
				[ptr appendFormat:@"\"%.*s\",",
				 (int) lwc_string_length(*string_list),
				 lwc_string_data(*string_list)];
				
				string_list++;
			}
		}
		switch (val) {
			case CSS_FONT_FAMILY_SERIF:
				[ptr appendString:@" serif"];
				break;
			case CSS_FONT_FAMILY_SANS_SERIF:
				[ptr appendString:@" sans-serif"];
				break;
			case CSS_FONT_FAMILY_CURSIVE:
				[ptr appendString:@" cursive"];
				break;
			case CSS_FONT_FAMILY_FANTASY:
				[ptr appendString:@" fantasy"];
				break;
			case CSS_FONT_FAMILY_MONOSPACE:
				[ptr appendString:@" monospace"];
				break;
		}
	}
	
	/* font-size */
	val = css_computed_font_size(style, &len1, &unit1);
	switch (val) {
		case CSS_FONT_SIZE_INHERIT:
			result[@"font-size"] = @"inherit";
			break;
		case CSS_FONT_SIZE_XX_SMALL:
			result[@"font-size"] = @"xx-small";
			break;
		case CSS_FONT_SIZE_X_SMALL:
			result[@"font-size"] = @"x-small";
			break;
		case CSS_FONT_SIZE_SMALL:
			result[@"font-size"] = @"small";
			break;
		case CSS_FONT_SIZE_MEDIUM:
			result[@"font-size"] = @"medium";
			break;
		case CSS_FONT_SIZE_LARGE:
			result[@"font-size"] = @"large";
			break;
		case CSS_FONT_SIZE_X_LARGE:
			result[@"font-size"] = @"x-large";
			break;
		case CSS_FONT_SIZE_XX_LARGE:
			result[@"font-size"] = @"xx-large";
			break;
		case CSS_FONT_SIZE_LARGER:
			result[@"font-size"] = @"larger";
			break;
		case CSS_FONT_SIZE_SMALLER:
			result[@"font-size"] = @"smaller";
			break;
		case CSS_FONT_SIZE_DIMENSION:
			result[@"font-size"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* font-style */
	val = css_computed_font_style(style);
	switch (val) {
		case CSS_FONT_STYLE_INHERIT:
			result[@"font-style"] = @"inherit";
			break;
		case CSS_FONT_STYLE_NORMAL:
			result[@"font-style"] = @"normal";
			break;
		case CSS_FONT_STYLE_ITALIC:
			result[@"font-style"] = @"italic";
			break;
		case CSS_FONT_STYLE_OBLIQUE:
			result[@"font-style"] = @"oblique";
			break;
		default:
			break;
	}
	
	/* font-variant */
	val = css_computed_font_variant(style);
	switch (val) {
		case CSS_FONT_VARIANT_INHERIT:
			result[@"font-variant"] = @"inherit";
			break;
		case CSS_FONT_VARIANT_NORMAL:
			result[@"font-variant"] = @"normal";
			break;
		case CSS_FONT_VARIANT_SMALL_CAPS:
			result[@"font-variant"] = @"small-caps";
			break;
		default:
			break;
	}
	
	/* font-weight */
	val = css_computed_font_weight(style);
	switch (val) {
		case CSS_FONT_WEIGHT_INHERIT:
			result[@"font-weight"] = @"inherit";
			break;
		case CSS_FONT_WEIGHT_NORMAL:
			result[@"font-weight"] = @"normal";
			break;
		case CSS_FONT_WEIGHT_BOLD:
			result[@"font-weight"] = @"bold";
			break;
		case CSS_FONT_WEIGHT_BOLDER:
			result[@"font-weight"] = @"bolder";
			break;
		case CSS_FONT_WEIGHT_LIGHTER:
			result[@"font-weight"] = @"lighter";
			break;
		case CSS_FONT_WEIGHT_100:
			result[@"font-weight"] = @"100";
			break;
		case CSS_FONT_WEIGHT_200:
			result[@"font-weight"] = @"200";
			break;
		case CSS_FONT_WEIGHT_300:
			result[@"font-weight"] = @"300";
			break;
		case CSS_FONT_WEIGHT_400:
			result[@"font-weight"] = @"400";
			break;
		case CSS_FONT_WEIGHT_500:
			result[@"font-weight"] = @"500";
			break;
		case CSS_FONT_WEIGHT_600:
			result[@"font-weight"] = @"600";
			break;
		case CSS_FONT_WEIGHT_700:
			result[@"font-weight"] = @"700";
			break;
		case CSS_FONT_WEIGHT_800:
			result[@"font-weight"] = @"800";
			break;
		case CSS_FONT_WEIGHT_900:
			result[@"font-weight"] = @"900";
			break;
		default:
			break;
	}
	
	/* height */
	val = css_computed_height(style, &len1, &unit1);
	switch (val) {
		case CSS_HEIGHT_INHERIT:
			result[@"height"] = @"inherit";
			break;
		case CSS_HEIGHT_AUTO:
			result[@"height"] = @"auto";
			break;
		case CSS_HEIGHT_SET:
			result[@"height"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* justify-content */
	val = css_computed_justify_content(style);
	switch (val) {
		case CSS_JUSTIFY_CONTENT_INHERIT:
			result[@"justify-content"] = @"inherit";
			break;
		case CSS_JUSTIFY_CONTENT_FLEX_START:
			result[@"justify-content"] = @"flex-start";
			break;
		case CSS_JUSTIFY_CONTENT_FLEX_END:
			result[@"justify-content"] = @"flex-end";
			break;
		case CSS_JUSTIFY_CONTENT_CENTER:
			result[@"justify-content"] = @"center";
			break;
		case CSS_JUSTIFY_CONTENT_SPACE_BETWEEN:
			result[@"justify-content"] = @"space-between";
			break;
		case CSS_JUSTIFY_CONTENT_SPACE_AROUND:
			result[@"justify-content"] = @"space-around";
			break;
		case CSS_JUSTIFY_CONTENT_SPACE_EVENLY:
			result[@"justify-content"] = @"space-evenly";
			break;
		default:
			break;
	}
	
	/* left */
	val = css_computed_left(style, &len1, &unit1);
	switch (val) {
		case CSS_LEFT_INHERIT:
			result[@"left"] = @"inherit";
			break;
		case CSS_LEFT_AUTO:
			result[@"left"] = @"auto";
			break;
		case CSS_LEFT_SET:
			result[@"left"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* letter-spacing */
	val = css_computed_letter_spacing(style, &len1, &unit1);
	switch (val) {
		case CSS_LETTER_SPACING_INHERIT:
			result[@"letter-spacing"] = @"inherit";
			break;
		case CSS_LETTER_SPACING_NORMAL:
			result[@"letter-spacing"] = @"normal";
			break;
		case CSS_LETTER_SPACING_SET:
			result[@"letter-spacing"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* line-height */
	val = css_computed_line_height(style, &len1, &unit1);
	switch (val) {
		case CSS_LINE_HEIGHT_INHERIT:
			result[@"line-height"] = @"inherit";
			break;
		case CSS_LINE_HEIGHT_NORMAL:
			result[@"line-height"] = @"normal";
			break;
		case CSS_LINE_HEIGHT_NUMBER:
			result[@"line-height"] = dump_objc_css_fixed(len1);
			break;
		case CSS_LINE_HEIGHT_DIMENSION:
			result[@"line-height"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* list-style-image */
	val = css_computed_list_style_image(style, &url);
	if (val == CSS_LIST_STYLE_IMAGE_INHERIT) {
		result[@"list-style-image"] = @"inherit";
	} else if (url != NULL) {
		result[@"list-style-image"] =
		[NSString stringWithFormat:@"url('%.*s')",
		 (int) lwc_string_length(url),
		 lwc_string_data(url)];
	} else if (val == CSS_LIST_STYLE_IMAGE_NONE) {
		result[@"list-style-image"] = @"none";
	}
	
	/* list-style-position */
	val = css_computed_list_style_position(style);
	switch (val) {
		case CSS_LIST_STYLE_POSITION_INHERIT:
			result[@"list-style-position"] = @"inherit";
			break;
		case CSS_LIST_STYLE_POSITION_INSIDE:
			result[@"list-style-position"] = @"inside";
			break;
		case CSS_LIST_STYLE_POSITION_OUTSIDE:
			result[@"list-style-position"] = @"outside";
			break;
		default:
			break;
	}
	
	/* list-style-type */
	val = css_computed_list_style_type(style);
	switch (val) {
		case CSS_LIST_STYLE_TYPE_INHERIT:
			result[@"list-style-type"] = @"inherit";
			break;
		case CSS_LIST_STYLE_TYPE_DISC:
			result[@"list-style-type"] = @"disc";
			break;
		case CSS_LIST_STYLE_TYPE_CIRCLE:
			result[@"list-style-type"] = @"circle";
			break;
		case CSS_LIST_STYLE_TYPE_SQUARE:
			result[@"list-style-type"] = @"square";
			break;
		case CSS_LIST_STYLE_TYPE_DECIMAL:
			result[@"list-style-type"] = @"decimal";
			break;
		case CSS_LIST_STYLE_TYPE_DECIMAL_LEADING_ZERO:
			result[@"list-style-type"] = @"decimal-leading-zero";
			break;
		case CSS_LIST_STYLE_TYPE_LOWER_ROMAN:
			result[@"list-style-type"] = @"lower-roman";
			break;
		case CSS_LIST_STYLE_TYPE_UPPER_ROMAN:
			result[@"list-style-type"] = @"upper-roman";
			break;
		case CSS_LIST_STYLE_TYPE_LOWER_GREEK:
			result[@"list-style-type"] = @"lower-greek";
			break;
		case CSS_LIST_STYLE_TYPE_LOWER_LATIN:
			result[@"list-style-type"] = @"lower-latin";
			break;
		case CSS_LIST_STYLE_TYPE_UPPER_LATIN:
			result[@"list-style-type"] = @"upper-latin";
			break;
		case CSS_LIST_STYLE_TYPE_ARMENIAN:
			result[@"list-style-type"] = @"armenian";
			break;
		case CSS_LIST_STYLE_TYPE_GEORGIAN:
			result[@"list-style-type"] = @"georgian";
			break;
		case CSS_LIST_STYLE_TYPE_LOWER_ALPHA:
			result[@"list-style-type"] = @"lower-alpha";
			break;
		case CSS_LIST_STYLE_TYPE_UPPER_ALPHA:
			result[@"list-style-type"] = @"upper-alpha";
			break;
		case CSS_LIST_STYLE_TYPE_NONE:
			result[@"list-style-type"] = @"none";
			break;
		default:
			break;
	}
	
	/* margin-top */
	val = css_computed_margin_top(style, &len1, &unit1);
	switch (val) {
		case CSS_MARGIN_INHERIT:
			result[@"margin-top"] = @"inherit";
			break;
		case CSS_MARGIN_AUTO:
			result[@"margin-top"] = @"auto";
			break;
		case CSS_MARGIN_SET:
			result[@"margin-top"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* margin-right */
	val = css_computed_margin_right(style, &len1, &unit1);
	switch (val) {
		case CSS_MARGIN_INHERIT:
			result[@"margin-right"] = @"inherit";
			break;
		case CSS_MARGIN_AUTO:
			result[@"margin-right"] = @"auto";
			break;
		case CSS_MARGIN_SET:
			result[@"margin-right"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* margin-bottom */
	val = css_computed_margin_bottom(style, &len1, &unit1);
	switch (val) {
		case CSS_MARGIN_INHERIT:
			result[@"margin-bottom"] = @"inherit";
			break;
		case CSS_MARGIN_AUTO:
			result[@"margin-bottom"] = @"auto";
			break;
		case CSS_MARGIN_SET:
			result[@"margin-bottom"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* margin-left */
	val = css_computed_margin_left(style, &len1, &unit1);
	switch (val) {
		case CSS_MARGIN_INHERIT:
			result[@"margin-left"] = @"inherit";
			break;
		case CSS_MARGIN_AUTO:
			result[@"margin-left"] = @"auto";
			break;
		case CSS_MARGIN_SET:
			result[@"margin-left"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* max-height */
	val = css_computed_max_height(style, &len1, &unit1);
	switch (val) {
		case CSS_MAX_HEIGHT_INHERIT:
			result[@"max-height"] = @"inherit";
			break;
		case CSS_MAX_HEIGHT_NONE:
			result[@"max-height"] = @"none";
			break;
		case CSS_MAX_HEIGHT_SET:
			result[@"max-height"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* max-width */
	val = css_computed_max_width(style, &len1, &unit1);
	switch (val) {
		case CSS_MAX_WIDTH_INHERIT:
			result[@"max-width"] = @"inherit";
			break;
		case CSS_MAX_WIDTH_NONE:
			result[@"max-width"] = @"none";
			break;
		case CSS_MAX_WIDTH_SET:
			result[@"max-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* min-height */
	val = css_computed_min_height(style, &len1, &unit1);
	switch (val) {
		case CSS_MIN_HEIGHT_INHERIT:
			result[@"min-height"] = @"inherit";
			break;
		case CSS_MIN_HEIGHT_AUTO:
			result[@"min-height"] = @"auto";
			break;
		case CSS_MIN_HEIGHT_SET:
			result[@"min-height"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* min-width */
	val = css_computed_min_width(style, &len1, &unit1);
	switch (val) {
		case CSS_MIN_WIDTH_INHERIT:
			result[@"min-width"] = @"inherit";
			break;
		case CSS_MIN_WIDTH_AUTO:
			result[@"min-width"] = @"auto";
			break;
		case CSS_MIN_WIDTH_SET:
			result[@"min-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* opacity */
	val = css_computed_opacity(style, &len1);
	switch (val) {
		case CSS_OPACITY_INHERIT:
			result[@"opacity"] = @"inherit";
			break;
		case CSS_OPACITY_SET:
			result[@"opacity"] = dump_objc_css_fixed(len1);
			break;
		default:
			break;
	}
	
	/* order */
	val = css_computed_order(style, &integer);
	switch (val) {
		case CSS_ORDER_INHERIT:
			result[@"order"] = @"inherit";
			break;
		case CSS_ORDER_SET:
			result[@"order"] = [NSString stringWithFormat:@"%d", integer];
			break;
		default:
			break;
	}
	
	/* outline-color */
	val = css_computed_outline_color(style, &color);
	switch (val) {
		case CSS_OUTLINE_COLOR_INHERIT:
			result[@"outline-color"] = @"inherit";
			break;
		case CSS_OUTLINE_COLOR_INVERT:
			result[@"outline-color"] = @"invert";
			break;
		case CSS_OUTLINE_COLOR_COLOR:
			result[@"outline-color"] = [NSString stringWithFormat:@"#%08x", color];
			break;
		default:
			break;
	}
	
	/* outline-style */
	val = css_computed_outline_style(style);
	switch (val) {
		case CSS_OUTLINE_STYLE_INHERIT:
			result[@"outline-style"] = @"inherit";
			break;
		case CSS_OUTLINE_STYLE_NONE:
			result[@"outline-style"] = @"none";
			break;
		case CSS_OUTLINE_STYLE_DOTTED:
			result[@"outline-style"] = @"dotted";
			break;
		case CSS_OUTLINE_STYLE_DASHED:
			result[@"outline-style"] = @"dashed";
			break;
		case CSS_OUTLINE_STYLE_SOLID:
			result[@"outline-style"] = @"solid";
			break;
		case CSS_OUTLINE_STYLE_DOUBLE:
			result[@"outline-style"] = @"double";
			break;
		case CSS_OUTLINE_STYLE_GROOVE:
			result[@"outline-style"] = @"groove";
			break;
		case CSS_OUTLINE_STYLE_RIDGE:
			result[@"outline-style"] = @"ridge";
			break;
		case CSS_OUTLINE_STYLE_INSET:
			result[@"outline-style"] = @"inset";
			break;
		case CSS_OUTLINE_STYLE_OUTSET:
			result[@"outline-style"] = @"outset";
			break;
		default:
			break;
	}
	
	/* outline-width */
	val = css_computed_outline_width(style, &len1, &unit1);
	switch (val) {
		case CSS_OUTLINE_WIDTH_INHERIT:
			result[@"outline-width"] = @"inherit";
			break;
		case CSS_OUTLINE_WIDTH_THIN:
			result[@"outline-width"] = @"thin";
			break;
		case CSS_OUTLINE_WIDTH_MEDIUM:
			result[@"outline-width"] = @"medium";
			break;
		case CSS_OUTLINE_WIDTH_THICK:
			result[@"outline-width"] = @"thick";
			break;
		case CSS_OUTLINE_WIDTH_WIDTH:
			result[@"outline-width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* overflow-x */
	val = css_computed_overflow_x(style);
	switch (val) {
		case CSS_OVERFLOW_INHERIT:
			result[@"overflow-x"] = @"inherit";
			break;
		case CSS_OVERFLOW_VISIBLE:
			result[@"overflow-x"] = @"visible";
			break;
		case CSS_OVERFLOW_HIDDEN:
			result[@"overflow-x"] = @"hidden";
			break;
		case CSS_OVERFLOW_SCROLL:
			result[@"overflow-x"] = @"scroll";
			break;
		case CSS_OVERFLOW_AUTO:
			result[@"overflow-x"] = @"auto";
			break;
		default:
			break;
	}
	
	/* overflow-y */
	val = css_computed_overflow_y(style);
	switch (val) {
		case CSS_OVERFLOW_INHERIT:
			result[@"overflow-y"] = @"inherit";
			break;
		case CSS_OVERFLOW_VISIBLE:
			result[@"overflow-y"] = @"visible";
			break;
		case CSS_OVERFLOW_HIDDEN:
			result[@"overflow-y"] = @"hidden";
			break;
		case CSS_OVERFLOW_SCROLL:
			result[@"overflow-y"] = @"scroll";
			break;
		case CSS_OVERFLOW_AUTO:
			result[@"overflow-y"] = @"auto";
			break;
		default:
			break;
	}
	
	/* padding-top */
	val = css_computed_padding_top(style, &len1, &unit1);
	switch (val) {
		case CSS_PADDING_INHERIT:
			result[@"padding-top"] = @"inherit";
			break;
		case CSS_PADDING_SET:
			result[@"padding-top"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* padding-right */
	val = css_computed_padding_right(style, &len1, &unit1);
	switch (val) {
		case CSS_PADDING_INHERIT:
			result[@"padding-right"] = @"inherit";
			break;
		case CSS_PADDING_SET:
			result[@"padding-right"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* padding-bottom */
	val = css_computed_padding_bottom(style, &len1, &unit1);
	switch (val) {
		case CSS_PADDING_INHERIT:
			result[@"padding-bottom"] = @"inherit";
			break;
		case CSS_PADDING_SET:
			result[@"padding-bottom"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* padding-left */
	val = css_computed_padding_left(style, &len1, &unit1);
	switch (val) {
		case CSS_PADDING_INHERIT:
			result[@"padding-left"] = @"inherit";
			break;
		case CSS_PADDING_SET:
			result[@"padding-left"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* position */
	val = css_computed_position(style);
	switch (val) {
		case CSS_POSITION_INHERIT:
			result[@"position"] = @"inherit";
			break;
		case CSS_POSITION_STATIC:
			result[@"position"] = @"static";
			break;
		case CSS_POSITION_RELATIVE:
			result[@"position"] = @"relative";
			break;
		case CSS_POSITION_ABSOLUTE:
			result[@"position"] = @"absolute";
			break;
		case CSS_POSITION_FIXED:
			result[@"position"] = @"fixed";
			break;
		default:
			break;
	}
	
	/* quotes */
	val = css_computed_quotes(style, &string_list);
	if (val == CSS_QUOTES_STRING && string_list != NULL) {
		NSMutableString *ptr = [NSMutableString new];
		result[@"quotes"] = ptr;
		
		while (*string_list != NULL) {
			[ptr appendFormat:@" \"%.*s\"",
							 (int) lwc_string_length(*string_list),
							 lwc_string_data(*string_list)];
			
			string_list++;
		}
	} else {
		switch (val) {
			case CSS_QUOTES_INHERIT:
				result[@"quotes"] = @"inherit";
				break;
			case CSS_QUOTES_NONE:
				result[@"quotes"] = @"none";
				break;
			default:
				break;
		}
	}
	
	/* right */
	val = css_computed_right(style, &len1, &unit1);
	switch (val) {
		case CSS_RIGHT_INHERIT:
			result[@"right"] = @"inherit";
			break;
		case CSS_RIGHT_AUTO:
			result[@"right"] = @"auto";
			break;
		case CSS_RIGHT_SET:
			result[@"right"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* table-layout */
	val = css_computed_table_layout(style);
	switch (val) {
		case CSS_TABLE_LAYOUT_INHERIT:
			result[@"table-layout"] = @"inherit";
			break;
		case CSS_TABLE_LAYOUT_AUTO:
			result[@"table-layout"] = @"auto";
			break;
		case CSS_TABLE_LAYOUT_FIXED:
			result[@"table-layout"] = @"fixed";
			break;
		default:
			break;
	}
	
	/* text-align */
	val = css_computed_text_align(style);
	switch (val) {
		case CSS_TEXT_ALIGN_INHERIT:
			result[@"text-align"] = @"inherit";
			break;
		case CSS_TEXT_ALIGN_LEFT:
			result[@"text-align"] = @"left";
			break;
		case CSS_TEXT_ALIGN_RIGHT:
			result[@"text-align"] = @"right";
			break;
		case CSS_TEXT_ALIGN_CENTER:
			result[@"text-align"] = @"center";
			break;
		case CSS_TEXT_ALIGN_JUSTIFY:
			result[@"text-align"] = @"justify";
			break;
		case CSS_TEXT_ALIGN_DEFAULT:
			result[@"text-align"] = @"default";
			break;
		case CSS_TEXT_ALIGN_LIBCSS_LEFT:
			result[@"text-align"] = @"-libcss-left";
			break;
		case CSS_TEXT_ALIGN_LIBCSS_CENTER:
			result[@"text-align"] = @"-libcss-center";
			break;
		case CSS_TEXT_ALIGN_LIBCSS_RIGHT:
			result[@"text-align"] = @"-libcss-right";
			break;
		default:
			break;
	}
	
	/* text-decoration */
	val = css_computed_text_decoration(style);
	if (val == CSS_TEXT_DECORATION_INHERIT) {
		result[@"text-decoration"] = @"inherit";
	} else if (val == CSS_TEXT_DECORATION_NONE) {
		result[@"text-decoration"] = @"none";
	} else {
		if (val & CSS_TEXT_DECORATION_BLINK) {
			result[@"text-decoration"] = @"blink";
		}
		if (val & CSS_TEXT_DECORATION_LINE_THROUGH) {
			result[@"text-decoration"] = @"line-through";
		}
		if (val & CSS_TEXT_DECORATION_OVERLINE) {
			result[@"text-decoration"] = @"overline";
		}
		if (val & CSS_TEXT_DECORATION_UNDERLINE) {
			result[@"text-decoration"] = @"underline";
		}
	}
	
	/* text-indent */
	val = css_computed_text_indent(style, &len1, &unit1);
	switch (val) {
		case CSS_TEXT_INDENT_INHERIT:
			result[@"text-indent"] = @"inherit";
			break;
		case CSS_TEXT_INDENT_SET:
			result[@"text-indent"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* text-transform */
	val = css_computed_text_transform(style);
	switch (val) {
		case CSS_TEXT_TRANSFORM_INHERIT:
			result[@"text-transform"] = @"inherit";
			break;
		case CSS_TEXT_TRANSFORM_CAPITALIZE:
			result[@"text-transform"] = @"capitalize";
			break;
		case CSS_TEXT_TRANSFORM_UPPERCASE:
			result[@"text-transform"] = @"uppercase";
			break;
		case CSS_TEXT_TRANSFORM_LOWERCASE:
			result[@"text-transform"] = @"lowercase";
			break;
		case CSS_TEXT_TRANSFORM_NONE:
			result[@"text-transform"] = @"none";
			break;
		default:
			break;
	}
	
	/* top */
	val = css_computed_top(style, &len1, &unit1);
	switch (val) {
		case CSS_TOP_INHERIT:
			result[@"top"] = @"inherit";
			break;
		case CSS_TOP_AUTO:
			result[@"top"] = @"auto";
			break;
		case CSS_TOP_SET:
			result[@"top"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* unicode-bidi */
	val = css_computed_unicode_bidi(style);
	switch (val) {
		case CSS_UNICODE_BIDI_INHERIT:
			result[@"unicode-bidi"] = @"inherit";
			break;
		case CSS_UNICODE_BIDI_NORMAL:
			result[@"unicode-bidi"] = @"normal";
			break;
		case CSS_UNICODE_BIDI_EMBED:
			result[@"unicode-bidi"] = @"embed";
			break;
		case CSS_UNICODE_BIDI_BIDI_OVERRIDE:
			result[@"unicode-bidi"] = @"bidi-override";
			break;
		default:
			break;
	}
	
	/* vertical-align */
	val = css_computed_vertical_align(style, &len1, &unit1);
	switch (val) {
		case CSS_VERTICAL_ALIGN_INHERIT:
			result[@"vertical-align"] = @"inherit";
			break;
		case CSS_VERTICAL_ALIGN_BASELINE:
			result[@"vertical-align"] = @"baseline";
			break;
		case CSS_VERTICAL_ALIGN_SUB:
			result[@"vertical-align"] = @"sub";
			break;
		case CSS_VERTICAL_ALIGN_SUPER:
			result[@"vertical-align"] = @"super";
			break;
		case CSS_VERTICAL_ALIGN_TOP:
			result[@"vertical-align"] = @"top";
			break;
		case CSS_VERTICAL_ALIGN_TEXT_TOP:
			result[@"vertical-align"] = @"text-top";
			break;
		case CSS_VERTICAL_ALIGN_MIDDLE:
			result[@"vertical-align"] = @"middle";
			break;
		case CSS_VERTICAL_ALIGN_BOTTOM:
			result[@"vertical-align"] = @"bottom";
			break;
		case CSS_VERTICAL_ALIGN_TEXT_BOTTOM:
			result[@"vertical-align"] = @"text-bottom";
			break;
		case CSS_VERTICAL_ALIGN_SET:
			result[@"vertical-align"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* visibility */
	val = css_computed_visibility(style);
	switch (val) {
		case CSS_VISIBILITY_INHERIT:
			result[@"visibility"] = @"inherit";
			break;
		case CSS_VISIBILITY_VISIBLE:
			result[@"visibility"] = @"visible";
			break;
		case CSS_VISIBILITY_HIDDEN:
			result[@"visibility"] = @"hidden";
			break;
		case CSS_VISIBILITY_COLLAPSE:
			result[@"visibility"] = @"collapse";
			break;
		default:
			break;
	}
	
	/* white-space */
	val = css_computed_white_space(style);
	switch (val) {
		case CSS_WHITE_SPACE_INHERIT:
			result[@"white-space"] = @"inherit";
			break;
		case CSS_WHITE_SPACE_NORMAL:
			result[@"white-space"] = @"normal";
			break;
		case CSS_WHITE_SPACE_PRE:
			result[@"white-space"] = @"pre";
			break;
		case CSS_WHITE_SPACE_NOWRAP:
			result[@"white-space"] = @"nowrap";
			break;
		case CSS_WHITE_SPACE_PRE_WRAP:
			result[@"white-space"] = @"pre-wrap";
			break;
		case CSS_WHITE_SPACE_PRE_LINE:
			result[@"white-space"] = @"pre-line";
			break;
		default:
			break;
	}
	
	/* width */
	val = css_computed_width(style, &len1, &unit1);
	switch (val) {
		case CSS_WIDTH_INHERIT:
			result[@"width"] = @"inherit";
			break;
		case CSS_WIDTH_AUTO:
			result[@"width"] = @"auto";
			break;
		case CSS_WIDTH_SET:
			result[@"width"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* word-spacing */
	val = css_computed_word_spacing(style, &len1, &unit1);
	switch (val) {
		case CSS_WORD_SPACING_INHERIT:
			result[@"word-spacing"] = @"inherit";
			break;
		case CSS_WORD_SPACING_NORMAL:
			result[@"word-spacing"] = @"normal";
			break;
		case CSS_WORD_SPACING_SET:
			result[@"word-spacing"] = dump_objc_css_unit(len1, unit1);
			break;
		default:
			break;
	}
	
	/* writing-mode */
	val = css_computed_writing_mode(style);
	switch (val) {
		case CSS_WRITING_MODE_INHERIT:
			result[@"writing-mode"] = @"inherit";
			break;
		case CSS_WRITING_MODE_HORIZONTAL_TB:
			result[@"writing-mode"] = @"horizontal-tb";
			break;
		case CSS_WRITING_MODE_VERTICAL_RL:
			result[@"writing-mode"] = @"vertical-rl";
			break;
		case CSS_WRITING_MODE_VERTICAL_LR:
			result[@"writing-mode"] = @"vertical-lr";
			break;
		default:
			break;
	}
	
	/* z-index */
	val = css_computed_z_index(style, &integer);
	switch (val) {
		case CSS_Z_INDEX_INHERIT:
			result[@"z-index"] = @"inherit";
			break;
		case CSS_Z_INDEX_AUTO:
			result[@"z-index"] = @"auto";
			break;
		case CSS_Z_INDEX_SET:
			result[@"z-index"] = [NSString stringWithFormat:@"%d", integer];
			break;
		default:
			break;
	}
	
	return result;
}

#endif

