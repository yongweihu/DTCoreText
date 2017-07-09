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

        // 换行符按 \n 来处理
		return [[NSAttributedString alloc] initWithString:@"\n" attributes:attributes];
	}
}

@end
