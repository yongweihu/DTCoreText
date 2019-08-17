//
//  DTTextAttachmentWraper.h
//  AccountKit
//
//  Created by Yongwei Hu on 2019/8/16.
//

#import <UIKit/UIKit.h>

#import "DTImageTextAttachment.h"

NS_ASSUME_NONNULL_BEGIN

@interface DTImageTextAttachmentWraper : NSTextAttachment

@property (nonatomic, strong, readonly) DTImageTextAttachment *originalAttachment;

- (instancetype)initWithDTImageTextAttachment:(DTImageTextAttachment *)attachment;

@end

NS_ASSUME_NONNULL_END
