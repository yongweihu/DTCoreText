//
//  DTCSSStylesheet.m
//  DTCoreText
//
//  Created by Oliver Drobnik on 9/5/11.
//  Copyright (c) 2011 Drobnik.com. All rights reserved.
//

#import "DTCSSStylesheet.h"
#import "DTCSSListStyle.h"

#import "DTHTMLElement.h"
#import "NSScanner+HTML.h"
#import "NSString+CSS.h"
#import "NSString+HTML.h"

#import <libcss/libcss.h>
#import <libcss/dump_computed_objc.h>
#import <libcss/dump_objc.h>

// external symbols generated via custom build rule and xxd
extern unsigned char default_css[];
extern unsigned int default_css_len;

static css_error node_name(void *pw, void *node,
						   css_qname *qname);
static css_error node_classes(void *pw, void *n,
							  lwc_string ***classes, uint32_t *n_classes);
static css_error node_id(void *pw, void *node,
						 lwc_string **id);
static css_error named_ancestor_node(void *pw, void *node,
									 const css_qname *qname,
									 void **ancestor);
static css_error named_parent_node(void *pw, void *node,
								   const css_qname *qname,
								   void **parent);
static css_error named_sibling_node(void *pw, void *node,
									const css_qname *qname,
									void **sibling);
static css_error named_generic_sibling_node(void *pw, void *node,
											const css_qname *qname,
											void **sibling);
static css_error parent_node(void *pw, void *node, void **parent);
static css_error sibling_node(void *pw, void *node, void **sibling);
static css_error node_has_name(void *pw, void *node,
							   const css_qname *qname,
							   bool *match);
static css_error node_has_class(void *pw, void *node,
								lwc_string *name,
								bool *match);
static css_error node_has_id(void *pw, void *node,
							 lwc_string *name,
							 bool *match);
static css_error node_has_attribute(void *pw, void *node,
									const css_qname *qname,
									bool *match);
static css_error node_has_attribute_equal(void *pw, void *node,
										  const css_qname *qname,
										  lwc_string *value,
										  bool *match);
static css_error node_has_attribute_dashmatch(void *pw, void *node,
											  const css_qname *qname,
											  lwc_string *value,
											  bool *match);
static css_error node_has_attribute_includes(void *pw, void *node,
											 const css_qname *qname,
											 lwc_string *value,
											 bool *match);
static css_error node_has_attribute_prefix(void *pw, void *node,
										   const css_qname *qname,
										   lwc_string *value,
										   bool *match);
static css_error node_has_attribute_suffix(void *pw, void *node,
										   const css_qname *qname,
										   lwc_string *value,
										   bool *match);
static css_error node_has_attribute_substring(void *pw, void *node,
											  const css_qname *qname,
											  lwc_string *value,
											  bool *match);
static css_error node_is_root(void *pw, void *node, bool *match);
static css_error node_count_siblings(void *pw, void *node,
									 bool same_name, bool after, int32_t *count);
static css_error node_is_empty(void *pw, void *node, bool *match);
static css_error node_is_link(void *pw, void *node, bool *match);
static css_error node_is_visited(void *pw, void *node, bool *match);
static css_error node_is_hover(void *pw, void *node, bool *match);
static css_error node_is_active(void *pw, void *node, bool *match);
static css_error node_is_focus(void *pw, void *node, bool *match);
static css_error node_is_enabled(void *pw, void *node, bool *match);
static css_error node_is_disabled(void *pw, void *node, bool *match);
static css_error node_is_checked(void *pw, void *node, bool *match);
static css_error node_is_target(void *pw, void *node, bool *match);
static css_error node_is_lang(void *pw, void *node,
							  lwc_string *lang, bool *match);
static css_error node_presentational_hint(void *pw, void *node,
										  uint32_t *nhints, css_hint **hints);
static css_error ua_default_for_property(void *pw, uint32_t property,
										 css_hint *hints);
static css_error compute_font_size(void *pw, const css_hint *parent,
								   css_hint *size);
static css_error set_libcss_node_data(void *pw, void *n,
									  void *libcss_node_data);
static css_error get_libcss_node_data(void *pw, void *n,
									  void **libcss_node_data);

static css_select_handler select_handler = {
	CSS_SELECT_HANDLER_VERSION_1,
	
	node_name,
	node_classes,
	node_id,
	named_ancestor_node,
	named_parent_node,
	named_sibling_node,
	named_generic_sibling_node,
	parent_node,
	sibling_node,
	node_has_name,
	node_has_class,
	node_has_id,
	node_has_attribute,
	node_has_attribute_equal,
	node_has_attribute_dashmatch,
	node_has_attribute_includes,
	node_has_attribute_prefix,
	node_has_attribute_suffix,
	node_has_attribute_substring,
	node_is_root,
	node_count_siblings,
	node_is_empty,
	node_is_link,
	node_is_visited,
	node_is_hover,
	node_is_active,
	node_is_focus,
	node_is_enabled,
	node_is_disabled,
	node_is_checked,
	node_is_target,
	node_is_lang,
	node_presentational_hint,
	ua_default_for_property,
	compute_font_size,
	set_libcss_node_data,
	get_libcss_node_data
};

typedef struct sheet_ctx {
	css_stylesheet *sheet;
	css_origin origin;
	uint64_t media;
    void *cssString;
} sheet_ctx;

static css_error resolve_url(void *pw,
							 const char *base, lwc_string *rel, lwc_string **abs)
{
	/* About as useless as possible */
	*abs = lwc_string_ref(rel);
	
	return CSS_OK;
}

@implementation DTCSSStylesheet
{
	NSMutableDictionary *_styles;
	
	uint32_t _n_sheets;
	sheet_ctx *_sheets;
	
	css_origin _defaultOrigin;
	uint64_t _defaultMedia;
}

#pragma mark Creating Stylesheets

+ (DTCSSStylesheet *)defaultStyleSheet
{
	static DTCSSStylesheet *defaultDTCSSStylesheet = nil;
	if (defaultDTCSSStylesheet)
	{
		return defaultDTCSSStylesheet;
	}
	
	@synchronized(self)
	{
		if (!defaultDTCSSStylesheet)
		{
			// get the data from the external symbol
			NSData *data = [NSData dataWithBytes:default_css length:default_css_len];
			NSString *cssString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
			
			defaultDTCSSStylesheet = [[DTCSSStylesheet alloc] initWithStyleBlock:cssString origin:CSS_ORIGIN_UA media:CSS_MEDIA_ALL];
		}
	}
	return defaultDTCSSStylesheet;
}

- (id)initWithStyleBlock:(NSString *)css
{
	return [self initWithStyleBlock:css origin:CSS_ORIGIN_AUTHOR media:CSS_MEDIA_ALL];
}

- (id)initWithStyleBlock:(NSString *)css origin:(css_origin)origin media:(uint64_t)media
{
	self = [super init];
	
	if (self)
	{
		_defaultOrigin = origin;
		_defaultMedia = media;
		_styles	= [[NSMutableDictionary alloc] init];
		
		[self parseStyleBlock:css];
	}
	
	return self;
}

- (id)initWithStylesheet:(DTCSSStylesheet *)stylesheet
{
	self = [super init];
	
	if (self)
	{
		_defaultOrigin = CSS_ORIGIN_AUTHOR;
		_defaultMedia = CSS_MEDIA_ALL;
		_styles	= [[NSMutableDictionary alloc] init];
        
		[self mergeStylesheet:stylesheet];
	}
	
	return self;
}

#ifndef COVERAGE

- (NSString *)description
{
	return [_styles description];
}

#endif

#pragma mark Working with Style Blocks

- (css_stylesheet *)createStyleSheetWithStyleBlock:(NSString *)css
{
	css_stylesheet_params params;
	
	params.params_version = CSS_STYLESHEET_PARAMS_VERSION_1;
	params.level = CSS_LEVEL_21;
	params.charset = "UTF-8";
	params.url = "";
	params.title = NULL;
	params.allow_quirks = false;
	params.inline_style = false;
	params.resolve = resolve_url;
	params.resolve_pw = NULL;
	params.import = NULL;
	params.import_pw = NULL;
	params.color = NULL;
	params.color_pw = NULL;
	params.font = NULL;
	params.font_pw = NULL;
	
	css_stylesheet *sheet;
	assert(css_stylesheet_create(&params, &sheet) == CSS_OK);
	
	css_error error = css_stylesheet_append_data(sheet, (const uint8_t *)css.UTF8String, css.length);
	assert(error == CSS_OK || error == CSS_NEEDDATA);
	assert(css_stylesheet_data_done(sheet) == CSS_OK);
    
#if DEBUG
    
    NSDictionary *styles = dump_objc_sheet(sheet);
    NSLog(@"%@", styles);
    
    /*
    size_t explen = css.length;
    char *buf = malloc(2 * explen);
    if (buf == NULL) {
        assert(0 && "No memory for result data");
    }

    size_t buflen = 2 * explen;

    dump_sheet(_sheet, buf, &buflen);
     */
#endif
    
    return sheet;

}

- (void)parseStyleBlock:(NSString*)css
{
	sheet_ctx sheetCtx;
    
    sheetCtx.cssString = (__bridge_retained void *)(css);
	sheetCtx.origin = _defaultOrigin;
	sheetCtx.media = _defaultMedia;
	
	[self mergeCSSStylesheet:sheetCtx];
}

- (void)mergeCSSStylesheet:(sheet_ctx)sheetCtx
{
	/* Extend array of sheets and append new sheet to it */
	sheet_ctx *temp = realloc(_sheets,
							  (_n_sheets + 1) * sizeof(sheet_ctx));
	assert(temp != NULL);
	
	_sheets = temp;
	
    _sheets[_n_sheets].cssString = sheetCtx.cssString;
    _sheets[_n_sheets].sheet = [self createStyleSheetWithStyleBlock:(__bridge NSString *)(sheetCtx.cssString)];
	_sheets[_n_sheets].origin = sheetCtx.origin;
	_sheets[_n_sheets].media = sheetCtx.media;
	
	_n_sheets++;
}

- (void)mergeStylesheet:(DTCSSStylesheet *)stylesheet
{
	for (int i = 0; i < stylesheet->_n_sheets; i++) {
		[self mergeCSSStylesheet:stylesheet->_sheets[i]];
	}
}

#pragma mark Accessing Style Information

- (NSDictionary *)mergedStyleDictionaryForElement:(DTHTMLElement *)element matchedSelectors:(NSSet * __autoreleasing*)matchedSelectors ignoreInlineStyle:(BOOL)ignoreInlineStyle
{
	css_select_results *results;
	
	css_select_ctx *select;
	assert(css_select_ctx_create(&select) == CSS_OK);
	
    for (int i = 0; i < _n_sheets; i++) {
        assert(css_select_ctx_append_sheet(select,
                                           _sheets[i].sheet,
                                           _sheets[i].origin,
                                           _sheets[i].media) == CSS_OK);
    }
	
	assert(css_select_style(select, (__bridge void *)(element), CSS_MEDIA_ALL, NULL, &select_handler, NULL, &results) == CSS_OK);
    
	css_computed_style *styles = results->styles[CSS_PSEUDO_ELEMENT_NONE];
	NSDictionary *stylesDict = dump_objc_computed_style(styles);
    
	css_select_results_destroy(results);
    css_select_ctx_destroy(select);
	
	return stylesDict;
}

- (NSDictionary *)styles
{
	return _styles;
}

- (NSArray *)orderedSelectors
{
	return nil;
}

#pragma mark NSCopying

- (id)copyWithZone:(NSZone *)zone
{
	DTCSSStylesheet *newStylesheet = [[DTCSSStylesheet allocWithZone:zone] initWithStylesheet:self];
	
	return newStylesheet;
}

- (void)dealloc
{
    for (int i = 0; i < _n_sheets; i++) {
        _sheets[i].cssString = NULL;
        css_stylesheet_destroy(_sheets[i].sheet);
    }
    
    _n_sheets = 0;
    free(_sheets);
    _sheets = NULL;
}

@end

// MARK: -

static bool objc_string_is_equal_lwc_string(NSString *objcStr, lwc_string *lwc_str)
{
	bool match = false;
	
	lwc_string *objc_str;
	assert(lwc_intern_string(objcStr.UTF8String, objcStr.length, &objc_str) == lwc_error_ok);
	assert(lwc_string_caseless_isequal(
									   objc_str, lwc_str, &match) ==
		   lwc_error_ok);
	lwc_string_unref(objc_str);
	
	return match;
}

css_error node_name(void *pw, void *n, css_qname *qname)
{
	DTHTMLElement *node = (__bridge DTHTMLElement *)n;
	
	lwc_string *element_name;
	assert(lwc_intern_string(node.name.UTF8String, node.name.length, &element_name) == lwc_error_ok);
	qname->name = element_name;
	
	return CSS_OK;
}

static css_error node_classes(void *pw, void *n,
							  lwc_string ***classes, uint32_t *n_classes)
{
	DTHTMLElement *node = (__bridge DTHTMLElement *)n;
	NSArray *classNames = [node.attributes[@"class"] componentsSeparatedByString:@" "];
	
	*classes = NULL;
	*n_classes = 0;
	if (classNames.count > 0) {
		*classes = realloc(NULL, sizeof(lwc_string **) * classNames.count);
		
		for (int i = 0; i < classNames.count; i++) {
			NSString *className = [classNames objectAtIndex:i];
			
			lwc_string *class_name;
			assert(lwc_intern_string(className.UTF8String, className.length, &class_name) == lwc_error_ok);
			
			(*classes)[i] = class_name;
		}
		
		*n_classes = classNames.count;
	}
	
	return CSS_OK;
	
}

css_error node_id(void *pw, void *n,
				  lwc_string **id)
{
	DTHTMLElement *node = (__bridge DTHTMLElement *)n;
	NSString *idName = node.attributes[@"id"];
	
	*id = NULL;
	if (idName.length) {
		lwc_string *id_name;
		assert(lwc_intern_string(idName.UTF8String, idName.length, &id_name) == lwc_error_ok);
		
		*id = id_name;
	}
	
	return CSS_OK;
}

css_error named_ancestor_node(void *pw, void *n,
							  const css_qname *qname,
							  void **ancestor)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*ancestor = NULL;
	for (node = node.parentNode; node != nil; node = node.parentNode) {
		if (objc_string_is_equal_lwc_string(node.name, qname->name)) {
			*ancestor = (__bridge void *)node;
			break;
		}
	}
	
	return CSS_OK;
}

css_error named_parent_node(void *pw, void *n,
							const css_qname *qname,
							void **parent)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*parent = NULL;
	if (node.parentNode != nil) {
		if (objc_string_is_equal_lwc_string(node.parentNode.name, qname->name)) {
			*parent = (__bridge void *) node.parentNode;
		}
	}
	
	return CSS_OK;
}

css_error named_sibling_node(void *pw, void *n,
							 const css_qname *qname,
							 void **sibling)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*sibling = NULL;
	
	NSInteger nodeIndex = [node.parentNode.childNodes indexOfObject:node];
	NSInteger previousNodeIndex = nodeIndex - 1;
	if (previousNodeIndex >= 0) {
		DTHTMLParserNode *previousNode = [node.parentNode.childNodes objectAtIndex:previousNodeIndex];
		
		if (objc_string_is_equal_lwc_string(previousNode.name, qname->name)) {
			*sibling = (__bridge void *) previousNode;
		}
	}
	
	return CSS_OK;
}

css_error named_generic_sibling_node(void *pw, void *n,
									 const css_qname *qname,
									 void **sibling)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	NSInteger nodeIndex = [node.parentNode.childNodes indexOfObject:node];
	
	*sibling = NULL;
	for (nodeIndex = nodeIndex - 1; nodeIndex >= 0; nodeIndex--) {
		DTHTMLParserNode *previousNode = [node.parentNode.childNodes objectAtIndex:nodeIndex];
		
		if (objc_string_is_equal_lwc_string(previousNode.name, qname->name)) {
			*sibling = (__bridge void *) [node.parentNode.childNodes objectAtIndex:nodeIndex];
			break;
		}
	}
	
	return CSS_OK;
}

css_error parent_node(void *pw, void *n, void **parent)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*parent = NULL;
	if (node.parentNode) {
		*parent = (__bridge void *) node.parentNode;
	}
	
	return CSS_OK;
}

css_error sibling_node(void *pw, void *n, void **sibling)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	NSInteger nodeIndex = [node.parentNode.childNodes indexOfObject:node];
	if (--nodeIndex >= 0) {
		*sibling = (__bridge void *) [node.parentNode.childNodes objectAtIndex:nodeIndex];
	}
	
	return CSS_OK;
}

css_error node_has_name(void *pw, void *n,
						const css_qname *qname,
						bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	if (lwc_string_length(qname->name) == 1 &&
		lwc_string_data(qname->name)[0] == '*') {
		*match = true;
	} else {
		*match = objc_string_is_equal_lwc_string(node.name, qname->name);
	}
	
	return CSS_OK;
}

css_error node_has_class(void *pw, void *n,
						 lwc_string *name,
						 bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	NSArray *classNames = [node.attributes[@"class"] componentsSeparatedByString:@" "];
	
	*match = false;
	for (NSString *className in classNames) {
		if (objc_string_is_equal_lwc_string(className, name)) {
			*match = true;
			break;
		}
	}
	
	return CSS_OK;
}

css_error node_has_id(void *pw, void *n,
					  lwc_string *name,
					  bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	*match = objc_string_is_equal_lwc_string(node.attributes[@"id"], name);
	
	return CSS_OK;
}

css_error node_has_attribute(void *pw, void *n,
							 const css_qname *qname,
							 bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*match = false;
	for (NSString *key in node.attributes) {
		if (objc_string_is_equal_lwc_string(key, qname->name)) {
			*match = true;
			break;
		}
	}
	
	return CSS_OK;
}

css_error node_has_attribute_equal(void *pw, void *n,
								   const css_qname *qname,
								   lwc_string *value,
								   bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*match = false;
	for (NSString *key in node.attributes) {
		if (objc_string_is_equal_lwc_string(key, qname->name)) {
			*match = objc_string_is_equal_lwc_string(node.attributes[key], value);
			break;
		}
	}
	
	return CSS_OK;
}

css_error node_has_attribute_includes(void *pw, void *n,
									  const css_qname *qname,
									  lwc_string *value,
									  bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	size_t vlen = lwc_string_length(value);
	*match = false;
	for (NSString *key in node.attributes) {
		if (objc_string_is_equal_lwc_string(key, qname->name)) {
			NSString *valueStr = node.attributes[key];
			
			lwc_string *value_str;
			lwc_intern_string(valueStr.UTF8String, valueStr.length, &value_str);
			
			const char *p;
			const char *start = lwc_string_data(value_str);
			const char *end = start +
			lwc_string_length(value_str);
			
			*match = false;
			
			for (p = start; p < end; p++) {
				if (*p == ' ') {
					if ((size_t) (p - start) == vlen &&
						strncasecmp(start,
									lwc_string_data(value),
									vlen) == 0) {
							*match = true;
							break;
						}
					
					start = p + 1;
				}
			}
			
			lwc_string_destroy(value_str);
			break;
		}
	}
	
	return CSS_OK;
}

css_error node_has_attribute_dashmatch(void *pw, void *n,
									   const css_qname *qname,
									   lwc_string *value,
									   bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	size_t vlen = lwc_string_length(value);
	*match = false;
	
	for (NSString *key in node.attributes) {
		if (objc_string_is_equal_lwc_string(key, qname->name)) {
			NSString *valueStr = node.attributes[key];
			
			lwc_string *value_str;
			lwc_intern_string(valueStr.UTF8String, valueStr.length, &value_str);
			
			const char *p;
			const char *start = lwc_string_data(value_str);
			const char *end = start +
			lwc_string_length(value_str);
			
			*match = false;
			
			for (p = start; p < end; p++) {
				if (*p == '-') {
					if ((size_t) (p - start) == vlen &&
						strncasecmp(start,
									lwc_string_data(value),
									vlen) == 0) {
							*match = true;
							break;
						}
					
					start = p + 1;
				}
			}
			
			lwc_string_destroy(value_str);
			break;
		}
	}
	
	return CSS_OK;
}

css_error node_has_attribute_prefix(void *pw, void *n,
									const css_qname *qname,
									lwc_string *value,
									bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*match = false;
	for (NSString *key in node.attributes) {
		if (objc_string_is_equal_lwc_string(key, qname->name)) {
			NSString *valueStr = node.attributes[key];
			
			lwc_string *value_str;
			lwc_intern_string(valueStr.UTF8String, valueStr.length, &value_str);
			
			size_t len = lwc_string_length(value_str);
			const char *data = lwc_string_data(value_str);
			
			size_t vlen = lwc_string_length(value);
			const char *vdata = lwc_string_data(value);
			
			if (len < vlen)
				*match = false;
			else
				*match = (strncasecmp(data, vdata, vlen) == 0);
			
			lwc_string_destroy(value_str);
			break;
		}
	}
	
	return CSS_OK;
}

css_error node_has_attribute_suffix(void *pw, void *n,
									const css_qname *qname,
									lwc_string *value,
									bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;

	*match = false;
	for (NSString *key in node.attributes) {
		if (objc_string_is_equal_lwc_string(key, qname->name)) {
			NSString *valueStr = node.attributes[key];
			
			lwc_string *value_str;
			lwc_intern_string(valueStr.UTF8String, valueStr.length, &value_str);
			
			size_t len = lwc_string_length(value_str);
			const char *data = lwc_string_data(value_str);
			
			size_t vlen = lwc_string_length(value);
			const char *vdata = lwc_string_data(value);
			
			size_t suffix_start = len - vlen;
			
			if (len < vlen)
				*match = false;
			else {
				*match = (strncasecmp(data + suffix_start,
									  vdata, vlen) == 0);
			}
			
			lwc_string_destroy(value_str);
			break;
		}
	}

	return CSS_OK;
}

css_error node_has_attribute_substring(void *pw, void *n,
									   const css_qname *qname,
									   lwc_string *value,
									   bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*match = false;
	for (NSString *key in node.attributes) {
		if (objc_string_is_equal_lwc_string(key, qname->name)) {
			NSString *valueStr = node.attributes[key];
			
			lwc_string *value_str;
			lwc_intern_string(valueStr.UTF8String, valueStr.length, &value_str);
			
			size_t len = lwc_string_length(value_str);
			const char *data = lwc_string_data(value_str);
			
			size_t vlen = lwc_string_length(value);
			const char *vdata = lwc_string_data(value);
			
			const char *last_start = data + len - vlen;
			
			if (len < vlen)
				*match = false;
			else {
				while (data <= last_start) {
					if (strncasecmp(data, vdata, vlen) == 0) {
						*match = true;
						break;
					}
					
					data++;
				}
				
				if (data > last_start)
					*match = false;
			}
			
			lwc_string_destroy(value_str);
			break;
		}
	}
	
	return CSS_OK;
}

css_error node_is_root(void *pw, void *n, bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*match = (node.parentNode == nil);
	
	return CSS_OK;
}

css_error node_count_siblings(void *pw, void *n,
							  bool same_name, bool after, int32_t *count)
{
	int32_t cnt = 0;
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	NSInteger nodeIndex = [node.parentNode.childNodes indexOfObject:node];
	
	if (after) {
		NSInteger nextIndex = nodeIndex + 1;
		while (nextIndex < node.parentNode.childNodes.count) {
			if (same_name) {
				DTHTMLParserNode *nextNode = [node.parentNode.childNodes objectAtIndex:nextIndex];
				if ([node.name isEqualToString:nextNode.name]) {
					cnt++;
				}
			} else {
				cnt++;
			}
			
			nextIndex++;
		}
	} else {
		NSInteger prevIndex = nodeIndex - 1;
		while (prevIndex >= 0) {
			if (same_name) {
				DTHTMLParserNode *prevNode = [node.parentNode.childNodes objectAtIndex:prevIndex];
				if ([node.name isEqualToString:prevNode.name]) {
					cnt++;
				}
			} else {
				cnt++;
			}
			
			prevIndex--;
		}
	}
	
	*count = cnt;
	
	return CSS_OK;
}

css_error node_is_empty(void *pw, void *n, bool *match)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	*match = (node.childNodes.count == 0);
	
	return CSS_OK;
}

css_error node_is_link(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_visited(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_hover(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_active(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_focus(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_enabled(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_disabled(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_checked(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_target(void *pw, void *n, bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_is_lang(void *pw, void *n,
					   lwc_string *lang,
					   bool *match)
{
	*match = false;
	
	return CSS_OK;
}

css_error node_presentational_hint(void *pw, void *node,
								   uint32_t *nhints, css_hint **hints)
{
	*nhints = 0;
	*hints = NULL;
	
	return CSS_OK;
}

css_error ua_default_for_property(void *pw, uint32_t property, css_hint *hint)
{
	if (property == CSS_PROP_COLOR) {
		hint->data.color = 0xff000000;
		hint->status = CSS_COLOR_COLOR;
	} else if (property == CSS_PROP_FONT_FAMILY) {
		hint->data.strings = NULL;
		hint->status = CSS_FONT_FAMILY_SANS_SERIF;
	} else if (property == CSS_PROP_QUOTES) {
		/* Not exactly useful :) */
		hint->data.strings = NULL;
		hint->status = CSS_QUOTES_NONE;
	} else if (property == CSS_PROP_VOICE_FAMILY) {
		/** \todo Fix this when we have voice-family done */
		hint->data.strings = NULL;
		hint->status = 0;
	} else {
		return CSS_INVALID;
	}
	
	return CSS_OK;
}

css_error compute_font_size(void *pw, const css_hint *parent, css_hint *size)
{
	static css_hint_length sizes[] = {
		{ FLTTOFIX(6.75), CSS_UNIT_PT },
		{ FLTTOFIX(7.50), CSS_UNIT_PT },
		{ FLTTOFIX(9.75), CSS_UNIT_PT },
		{ FLTTOFIX(12.0), CSS_UNIT_PT },
		{ FLTTOFIX(13.5), CSS_UNIT_PT },
		{ FLTTOFIX(18.0), CSS_UNIT_PT },
		{ FLTTOFIX(24.0), CSS_UNIT_PT }
	};
	const css_hint_length *parent_size;
	
	/* Grab parent size, defaulting to medium if none */
	if (parent == NULL) {
		parent_size = &sizes[CSS_FONT_SIZE_MEDIUM - 1];
	} else {
		assert(parent->status == CSS_FONT_SIZE_DIMENSION);
		assert(parent->data.length.unit != CSS_UNIT_EM);
		assert(parent->data.length.unit != CSS_UNIT_EX);
		parent_size = &parent->data.length;
	}
	
	assert(size->status != CSS_FONT_SIZE_INHERIT);
	
	if (size->status < CSS_FONT_SIZE_LARGER) {
		/* Keyword -- simple */
		size->data.length = sizes[size->status - 1];
	} else if (size->status == CSS_FONT_SIZE_LARGER) {
		/** \todo Step within table, if appropriate */
		size->data.length.value =
		FMUL(parent_size->value, FLTTOFIX(1.2));
		size->data.length.unit = parent_size->unit;
	} else if (size->status == CSS_FONT_SIZE_SMALLER) {
		/** \todo Step within table, if appropriate */
		size->data.length.value =
		FDIV(parent_size->value, FLTTOFIX(1.2));
		size->data.length.unit = parent_size->unit;
	} else if (size->data.length.unit == CSS_UNIT_EM ||
			   size->data.length.unit == CSS_UNIT_EX) {
		size->data.length.value =
		FMUL(size->data.length.value, parent_size->value);
		
		if (size->data.length.unit == CSS_UNIT_EX) {
			size->data.length.value = FMUL(size->data.length.value,
										   FLTTOFIX(0.6));
		}
		
		size->data.length.unit = parent_size->unit;
	} else if (size->data.length.unit == CSS_UNIT_PCT) {
		size->data.length.value = FDIV(FMUL(size->data.length.value,
											parent_size->value), FLTTOFIX(100));
		size->data.length.unit = parent_size->unit;
	}
	
	size->status = CSS_FONT_SIZE_DIMENSION;
	
	return CSS_OK;
}

static css_error set_libcss_node_data(void *pw, void *n,
									  void *libcss_node_data)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	node.libCSSData = libcss_node_data;
	
	return CSS_OK;
}

static css_error get_libcss_node_data(void *pw, void *n,
									  void **libcss_node_data)
{
	DTHTMLParserNode *node = (__bridge DTHTMLParserNode*)n;
	
	/* Pass any node data back to libcss */
	*libcss_node_data = node.libCSSData;
	
	return CSS_OK;
}
