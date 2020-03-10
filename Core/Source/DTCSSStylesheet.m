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


static lwc_string *lwc_string_for_nsstring(NSString *objcStr);
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

static css_error resolve_url(void *pw,
                             const char *base, lwc_string *rel, lwc_string **abs)
{
    /* About as useless as possible */
    const char *rel_data = lwc_string_data(rel);
    NSString *relStr = [NSString stringWithUTF8String:rel_data];
    NSString *baseStr = [NSString stringWithUTF8String:base];
    NSString *path = [baseStr stringByAppendingPathComponent:relStr];
    *abs = lwc_string_for_nsstring(path);
    
    return CSS_OK;
}

@interface DTStylesheetCreator : NSObject

+ (instancetype)sharedInstance;
- (css_stylesheet *)createStyleSheetWithStyleBlock:(NSString *)css inlineStyle:(BOOL)inlineStyle lockFirst:(BOOL)lockFirst baseURL:(NSURL *)baseURL;

@property (nonatomic, strong) dispatch_semaphore_t syncLock;

@end

static DTStylesheetCreator *_shareSheetCreator;

#define MyStyleCreator ([DTStylesheetCreator sharedInstance])

@implementation DTStylesheetCreator

+ (instancetype)sharedInstance
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _shareSheetCreator = [DTStylesheetCreator new];
    });
    
    return _shareSheetCreator;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.syncLock = dispatch_semaphore_create(1);
    }
    
    return self;
}

- (css_stylesheet_params)createEmptyStylesheetParams
{
    css_stylesheet_params params;
    
    params.params_version = CSS_STYLESHEET_PARAMS_VERSION_1;
    params.level = CSS_LEVEL_DEFAULT;
    params.charset = NULL;
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
    
    return params;
}

- (css_stylesheet *)createStyleSheetWithStyleBlock:(NSString *)css inlineStyle:(BOOL)inlineStyle lockFirst:(BOOL)lockFirst baseURL:(NSURL *)baseURL
{
    if (css.length == 0) {
        return nil;
    }
    
    css_stylesheet_params params = [self createEmptyStylesheetParams];
    params.url = [baseURL.absoluteString UTF8String] ?: "";
    params.inline_style = inlineStyle;
    
    if (lockFirst) {
        dispatch_semaphore_wait(self.syncLock, DISPATCH_TIME_FOREVER);
    }
    
    css_stylesheet *sheet;
    assert(css_stylesheet_create(&params, &sheet) == CSS_OK);
    
    css_error error = css_stylesheet_append_data(sheet, (const uint8_t *)css.UTF8String, css.length);
    assert(error == CSS_OK || error == CSS_NEEDDATA);
    
    [self loadChildCSS:sheet withBaseURL:baseURL];
    
    assert(css_stylesheet_data_done(sheet) == CSS_OK);
    
    if (lockFirst) {
        dispatch_semaphore_signal(self.syncLock);
    }
    
#if DEBUG
    
    //    NSDictionary *styles = dump_objc_sheet(sheet);
    //    NSLog(@"%@", styles);
    
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

/**
 * 如果css中包含@import语句，我们需要将import指定的css文件导入。
 */
- (void)loadChildCSS:(css_stylesheet *)parent withBaseURL:(NSURL *)baseURL
{
    do {
        lwc_string *url;
        uint64_t media;
        css_error error = css_stylesheet_next_pending_import(parent, &url, &media);
        if (error == CSS_INVALID) {
            break;
        }
        
        const char *url_data = lwc_string_data(url);
        NSString *urlPath = [NSString stringWithUTF8String:url_data];
        NSString *cssStr = [NSString stringWithContentsOfFile:urlPath encoding:NSUTF8StringEncoding error:nil];
        
        BOOL registeredChild = NO;
        if (cssStr.length) {
            css_stylesheet *childSheet = [self createStyleSheetWithStyleBlock:cssStr inlineStyle:NO lockFirst:NO baseURL:baseURL];
            if (childSheet != NULL) {
                css_stylesheet_register_import(parent, childSheet);
                registeredChild = YES;
            }
        }
        
        if (!registeredChild) {
            css_stylesheet *emptySheet;
            css_stylesheet_params params = [self createEmptyStylesheetParams];
            assert(css_stylesheet_create(&params, &emptySheet) == CSS_OK);
            css_stylesheet_register_import(parent, emptySheet);
        }
    } while (YES);
}

@end

@interface DTSheetContext : NSObject

@property (nonatomic, assign, readonly) css_stylesheet *sheet;
@property (nonatomic, assign, readonly) css_origin origin;
@property (nonatomic, assign, readonly) uint64_t media;

@end

@implementation DTSheetContext

- (instancetype)initWithCSSString:(NSString *)cssString origin:(css_origin)origin media:(uint64_t)media baseURL:(NSURL *)baseURL
{
    self = [super init];
    if (self) {
        _sheet = [MyStyleCreator createStyleSheetWithStyleBlock:cssString inlineStyle:NO lockFirst:YES baseURL:baseURL];
        _origin = origin;
        _media = media;
    }
    
    return self;
}

- (void)dealloc
{
    if (self.sheet) {
        dispatch_semaphore_wait(MyStyleCreator.syncLock, DISPATCH_TIME_FOREVER);
        css_stylesheet_destroy(self.sheet);
        _sheet = nil;
        dispatch_semaphore_signal(MyStyleCreator.syncLock);
    }
}

@end

@implementation DTCSSStylesheet
{
	NSMutableDictionary *_styles;
	
	NSMutableArray<DTSheetContext *> *_sheets;
    css_select_ctx *_select;
	
	css_origin _defaultOrigin;
	uint64_t _defaultMedia;
    NSURL *_baseURL;
}

#pragma mark Creating Stylesheets

+ (DTCSSStylesheet *)defaultStyleSheet
{
	static DTCSSStylesheet *defaultDTCSSStylesheet = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSBundle *bundle = [NSBundle bundleForClass:self];
        NSString *path = [bundle pathForResource:@"default" ofType:@"css"];
        // Cocoapods uses a separate Resources bundle to include default.css
        if (!path)
        {
            NSString *resourcesBundlePath = [bundle pathForResource:@"Resources" ofType:@"bundle"];
            NSBundle *resourcesBundle = [NSBundle bundleWithPath:resourcesBundlePath];
            path = [resourcesBundle pathForResource:@"default" ofType:@"css"];
        }
        NSString *cssString = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
        
        defaultDTCSSStylesheet = [[DTCSSStylesheet alloc] initWithStyleBlock:cssString origin:CSS_ORIGIN_UA media:CSS_MEDIA_ALL baseURL:nil];
    });
    
	return defaultDTCSSStylesheet;
}

- (id)initWithStyleBlock:(NSString *)css baseURL:(NSURL *)baseURL
{
	return [self initWithStyleBlock:css origin:CSS_ORIGIN_AUTHOR media:CSS_MEDIA_ALL baseURL:baseURL];
}

- (id)initWithStyleBlock:(NSString *)css origin:(css_origin)origin media:(uint64_t)media baseURL:(NSURL *)baseURL
{
	self = [super init];
	
	if (self)
	{
		_defaultOrigin = origin;
		_defaultMedia = media;
		_styles	= [[NSMutableDictionary alloc] init];
        _baseURL = baseURL;
        
		[self parseStyleBlock:css baseURL:baseURL];
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
        _defaultOptions = stylesheet.defaultOptions;
        _baseURL = stylesheet->_baseURL;
        
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

- (void)parseStyleBlock:(NSString *)css
{
    [self parseStyleBlock:css baseURL:nil];
}

- (void)parseStyleBlock:(NSString*)css baseURL:(NSURL *)baseURL
{
	DTSheetContext *sheetCtx = [[DTSheetContext alloc] initWithCSSString:css origin:_defaultOrigin media:_defaultMedia baseURL:baseURL];
	[self mergeCSSStylesheet:sheetCtx];
}

- (void)mergeCSSStylesheet:(DTSheetContext *)sheetCtx
{
    if (_sheets == nil) {
        _sheets = [NSMutableArray new];
    }
	
    [_sheets addObject:sheetCtx];
    
    if (_select != NULL) {
        css_select_ctx_destroy(_select);
        _select = NULL; // 保证下次获取元素sytles时会被重建
    }
}

- (void)mergeStylesheet:(DTCSSStylesheet *)stylesheet
{
	for (int i = 0; i < stylesheet->_sheets.count; i++) {
		[self mergeCSSStylesheet:stylesheet->_sheets[i]];
	}
}

#pragma mark Accessing Style Information

- (NSDictionary *)mergedStyleDictionaryForElement:(DTHTMLElement *)element matchedSelectors:(NSSet * __autoreleasing*)matchedSelectors ignoreInlineStyle:(BOOL)ignoreInlineStyle baseURL:(NSURL *)baseURL
{
    dispatch_semaphore_wait(MyStyleCreator.syncLock, DISPATCH_TIME_FOREVER);
    
	css_select_results *results;
    if (_select == NULL) {
        assert(css_select_ctx_create(&_select) == CSS_OK);
        
        for (int i = 0; i < _sheets.count; i++) {
            if (!_sheets[i].sheet) {
                continue;
            }
            
            assert(css_select_ctx_append_sheet(_select,
                                               _sheets[i].sheet,
                                               _sheets[i].origin,
                                               _sheets[i].media) == CSS_OK);
        }
    }
    
    css_stylesheet *inlineSheet = NULL;
    if (!ignoreInlineStyle) {
        // Get tag's local style attribute
        NSString *styleString = [element.attributes objectForKey:@"style"];
        
        if ([styleString length]) {
            inlineSheet = [MyStyleCreator createStyleSheetWithStyleBlock:styleString inlineStyle:YES lockFirst:NO baseURL:baseURL];
        }
    }
    
	assert(css_select_style(_select, (__bridge void *)(element), CSS_MEDIA_ALL, inlineSheet, &select_handler, (__bridge void *)(self), &results) == CSS_OK);
    
	css_computed_style *styles = results->styles[CSS_PSEUDO_ELEMENT_NONE];
	NSDictionary *stylesDict = dump_objc_computed_style(styles);
    
	css_select_results_destroy(results);
    if (inlineSheet) {
        css_stylesheet_destroy(inlineSheet);
    }
    
    dispatch_semaphore_signal(MyStyleCreator.syncLock);
    
	
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
    if (_select != NULL) {
        css_select_ctx_destroy(_select);
    }
}

@end

// MARK: -

static bool objc_string_is_equal_lwc_string(NSString *objcStr, lwc_string *lwc_str)
{
    const char *lwc_data = lwc_string_data(lwc_str);
    NSString *lwcStr = [[NSString alloc] initWithUTF8String:lwc_data];
	
	return [objcStr isEqualToString:lwcStr];
}

lwc_string *lwc_string_for_nsstring(NSString *objcStr)
{
    lwc_string *lwc_str;
    assert(lwc_intern_string(objcStr.UTF8String, strlen(objcStr.UTF8String), &lwc_str) == lwc_error_ok);
    
    return lwc_str;
}

css_error node_name(void *pw, void *n, css_qname *qname)
{
	DTHTMLElement *node = (__bridge DTHTMLElement *)n;
    qname->name = lwc_string_for_nsstring(node.name);
	
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
			(*classes)[i] = lwc_string_ref(lwc_string_for_nsstring(className));
		}
		
		*n_classes = (uint32_t)classNames.count;
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
        *id = lwc_string_for_nsstring(idName);
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
        if (previousNode.name == nil) {
            continue;
        }
        
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
	
    *sibling = NULL;
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
    NSString *matchedKey;
    
    NSArray *attributedNames = node.attributes.allKeys;
    for (int i = 0; i < attributedNames.count; i++) {
        assert(lwc_string_caseless_isequal(
                lwc_string_for_nsstring(attributedNames[i]),
                qname->name, match
               ) == lwc_error_ok);
        if (*match == true) {
            matchedKey = attributedNames[i];
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
    NSString *matchedKey;
    
    NSArray *attributedNames = node.attributes.allKeys;
    for (int i = 0; i < attributedNames.count; i++) {
        assert(lwc_string_caseless_isequal(
                lwc_string_for_nsstring(attributedNames[i]),
                qname->name, match
               ) == lwc_error_ok);
        if (*match == true) {
            matchedKey = attributedNames[i];
            break;
        }
    }

    if (*match == true) {
        assert(lwc_string_caseless_isequal(
                lwc_string_for_nsstring(matchedKey), value, match) ==
                lwc_error_ok);
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
    NSString *matchedKey;
    
    NSArray *attributedNames = node.attributes.allKeys;
    for (int i = 0; i < attributedNames.count; i++) {
        assert(lwc_string_caseless_isequal(
                lwc_string_for_nsstring(attributedNames[i]),
                qname->name, match
               ) == lwc_error_ok);
        if (*match == true) {
            matchedKey = attributedNames[i];
            break;
        }
    }

    if (*match == true) {
        lwc_string *attributeValue = lwc_string_for_nsstring(node.attributes[matchedKey]);
        
        const char *p;
        const char *start = lwc_string_data(attributeValue);
        const char *end = start +
                lwc_string_length(attributeValue);

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
    NSString *matchedKey;
    
    NSArray *attributedNames = node.attributes.allKeys;
    for (int i = 0; i < attributedNames.count; i++) {
        assert(lwc_string_caseless_isequal(
                lwc_string_for_nsstring(attributedNames[i]),
                qname->name, match
               ) == lwc_error_ok);
        if (*match == true) {
            matchedKey = attributedNames[i];
            break;
        }
    }

    if (*match == true && matchedKey.length) {
        lwc_string *attributeValue = lwc_string_for_nsstring(node.attributes[matchedKey]);
        
        const char *p;
        const char *start = lwc_string_data(attributeValue);
        const char *end = start + lwc_string_length(attributeValue);

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
    NSString *matchedKey;
    
    NSArray *attributedNames = node.attributes.allKeys;
    for (int i = 0; i < attributedNames.count; i++) {
        assert(lwc_string_caseless_isequal(
                lwc_string_for_nsstring(attributedNames[i]),
                qname->name, match
               ) == lwc_error_ok);
        if (*match == true) {
            matchedKey = attributedNames[i];
            break;
        }
    }

    if (*match == true && matchedKey.length) {
        lwc_string *attributeValue = lwc_string_for_nsstring(node.attributes[matchedKey]);
        size_t len = lwc_string_length(attributeValue);
        const char *data = lwc_string_data(attributeValue);

        size_t vlen = lwc_string_length(value);
        const char *vdata = lwc_string_data(value);

        if (len < vlen)
            *match = false;
        else
            *match = (strncasecmp(data, vdata, vlen) == 0);
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
    NSString *matchedKey;
    
    NSArray *attributedNames = node.attributes.allKeys;
    for (int i = 0; i < attributedNames.count; i++) {
        assert(lwc_string_caseless_isequal(
                lwc_string_for_nsstring(attributedNames[i]),
                qname->name, match
               ) == lwc_error_ok);
        if (*match == true) {
            matchedKey = attributedNames[i];
            break;
        }
    }

    if (*match == true && matchedKey.length) {
        lwc_string *attributeValue = lwc_string_for_nsstring(node.attributes[matchedKey]);
        size_t len = lwc_string_length(attributeValue);
        const char *data = lwc_string_data(attributeValue);

        size_t vlen = lwc_string_length(value);
        const char *vdata = lwc_string_data(value);

        size_t suffix_start = len - vlen;

        if (len < vlen)
            *match = false;
        else {
            *match = (strncasecmp(data + suffix_start,
                    vdata, vlen) == 0);
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
    NSString *matchedKey;
    
    NSArray *attributedNames = node.attributes.allKeys;
    for (int i = 0; i < attributedNames.count; i++) {
        assert(lwc_string_caseless_isequal(
                lwc_string_for_nsstring(attributedNames[i]),
                qname->name, match
               ) == lwc_error_ok);
        if (*match == true) {
            matchedKey = attributedNames[i];
            break;
        }
    }

    if (*match == true && matchedKey.length) {
        lwc_string *value = lwc_string_for_nsstring(node.attributes[matchedKey]);
        size_t len = lwc_string_length(value);
        const char *data = lwc_string_data(value);

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
				if ([node.name compare:nextNode.name options:NSCaseInsensitiveSearch] == NSOrderedSame) {
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
                if ([node.name compare:prevNode.name options:NSCaseInsensitiveSearch] == NSOrderedSame) {
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
    DTCSSStylesheet *stylesheet = (__bridge DTCSSStylesheet *)pw;
	if (property == CSS_PROP_COLOR) {
        UIColor *defaultColor = stylesheet.defaultOptions[DTDefaultTextColor];
        if (defaultColor != nil) {
            CGFloat red, green, blue, alpha;
            [defaultColor getRed:&red green:&green blue:&blue alpha:&alpha];
            css_color rgb = (int) (alpha * 255.0f)<<24 | (int) (red * 255.0f)<<16 | (int) (green * 255.0f)<<8 | (int) (blue * 255.0f)<<0;
            hint->data.color = rgb;
        } else {
            hint->data.color = 0xff000000;
        }
        
		hint->status = CSS_COLOR_COLOR;
	} else if (property == CSS_PROP_FONT_FAMILY) {
        NSString *fontFamily = stylesheet.defaultOptions[DTDefaultFontFamily];
        if (fontFamily.length) {
            lwc_string **fonts = malloc(2 * sizeof(lwc_string *));
            fonts[0] = lwc_string_for_nsstring(fontFamily);
            fonts[1] = NULL;
            hint->data.strings = fonts;
        } else {
            hint->data.strings = NULL;
        }
        
        hint->status = CSS_FONT_FAMILY_SANS_SERIF;
    } else if (property == CSS_PROP_FONT_SIZE) {
        float fontSize = [stylesheet.defaultOptions[DTDefaultFontSize] floatValue];
        hint->data.length.value = FLTTOFIX(fontSize);
        hint->data.length.unit = CSS_UNIT_PX;
        hint->status = CSS_FONT_SIZE_DIMENSION;
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
