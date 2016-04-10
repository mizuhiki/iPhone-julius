//
//  JuliusSampleViewController.m
//  JuliusSample
//
//  Created by Watanabe Toshinori on 11/01/15.
//  Copyright 2011 FLCL.jp. All rights reserved.
//

#import "JuliusSampleViewController.h"

@implementation JuliusSampleViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    
    self.julius = [[Julius alloc] init];
    self.julius.delegate = self;
    [self.julius startContinuousRecognizing];
}

#pragma mark -
#pragma mark Julius delegate

- (void)callBackResult:(NSArray *)results {
    // Show results.
    __weak JuliusSampleViewController *weak_self = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        weak_self.textView.text = [results componentsJoinedByString:@""];
    });
}

@end
