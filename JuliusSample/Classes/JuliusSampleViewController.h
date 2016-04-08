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
#import <MBProgressHUD.h>
#import "Julius.h"

@interface JuliusSampleViewController : UIViewController<AVAudioRecorderDelegate, JuliusDelegate> {
	
	// UI
	UIButton *recordButton;
	UITextView *textView;
	MBProgressHUD *HUD;

	AVAudioRecorder *recorder;
	Julius *julius;
	NSString *filePath;
	BOOL processing;
}

@property (nonatomic, strong) IBOutlet UIButton *recordButton;
@property (nonatomic, strong) IBOutlet UITextView *textView;
@property (nonatomic, strong) MBProgressHUD *HUD;
@property (nonatomic, strong) AVAudioRecorder *recorder;
@property (nonatomic, strong) Julius *julius;
@property (nonatomic, strong) NSString *filePath;
@property (nonatomic, assign) BOOL processing;

- (IBAction)startOrStopRecording:(id)sender;

@end

