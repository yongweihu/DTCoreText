//
//  DTTextAttachmentWraper.m
//  AccountKit
//
//  Created by Yongwei Hu on 2019/8/16.
//

#import "DTImageTextAttachmentWraper.h"

@implementation DTImageTextAttachmentWraper

- (instancetype)initWithDTImageTextAttachment:(DTImageTextAttachment *)attachment
{
    self = [super init];
    if (self) {
        _originalAttachment = attachment;
        self.image = attachment.image;
    }
    
    return self;
}

- (CGRect)attachmentBoundsForTextContainer:(NSTextContainer *)textContainer proposedLineFragment:(CGRect)lineFrag glyphPosition:(CGPoint)position characterIndex:(NSUInteger)charIndex
{
    // 根据lineFrag的大小调整displaySize
    [self.originalAttachment setDisplaySize:CGSizeZero withMaxDisplaySize:CGSizeMake(lineFrag.size.width, 1000000)];
    return CGRectMake(0, 0, self.originalAttachment.displaySize.width, self.originalAttachment.displaySize.height);
}

@end
