//
//  DTHTMLElementBR.m
//  DTCoreText
//
//  Created by Oliver Drobnik on 26.12.12.
//  Copyright (c) 2012 Drobnik.com. All rights reserved.
//

#import "DTBreakHTMLElement.h"

@implementation DTBreakHTMLElement

- (NSAttributedString *)attributedString
{
	@synchronized(self)
	{
		NSDictionary *attributes = [self attributesForAttributedStringRepresentation];
        
        // 将 UNICODE_LINE_FEED 替换为 \n，因为 iOS 识别不了 UNICODE_LINE_FEED，不会自动显示为换行
		return [[NSAttributedString alloc] initWithString:@"\n" attributes:attributes];
	}
}

@end
