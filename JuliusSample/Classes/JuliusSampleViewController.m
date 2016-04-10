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
	self.textView.text = [results componentsJoinedByString:@""];
}

@end
