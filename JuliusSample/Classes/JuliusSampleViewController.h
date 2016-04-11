//
//  JuliusSampleViewController.h
//  JuliusSample
//
//  Created by Watanabe Toshinori on 11/01/15.
//  Copyright 2011 FLCL.jp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreAudio/CoreAudioTypes.h>

#import "Julius.h"

@interface JuliusSampleViewController : UIViewController<AVAudioRecorderDelegate, JuliusDelegate>

@property (nonatomic, retain) IBOutlet UITextView *textView;
@property (nonatomic, retain) Julius *julius;

@end

