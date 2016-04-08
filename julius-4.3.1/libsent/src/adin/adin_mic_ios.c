/**
 * @file   adin_mic_linux.c
 *
 * <JA>
 * @brief  マイク入力 (Linux) - デフォルトデバイス
 *
 * マイク入力のための低レベル関数です．
 * インタフェースを明示指定しない (-input mic) 場合に呼ばれます．
 * ALSA, PulesAudio, OSS, ESD の順で最初に見つかったものが使用されます．
 * それぞれの API を明示的に指定したい場合は "-input" にそれぞれ
 * "alsa", "oss", "pulseaudio", "esd" を指定してください。
 * </JA>
 * <EN>
 * @brief  Microphone input on Linux - default device
 *
 * Low level I/O functions for microphone input on Linux.
 * This will be called when no device was explicitly specified ("-input mic").
 * ALSA, PulseAudio, OSS, ESD will be chosen in this order at compilation time.
 * "-input alsa", "-input oss", "-input pulseaudio" or "-input esd" to
 * specify which API to use.
 * </EN>
 *
 * @author Akinobu LEE
 * @date   Sun Feb 13 16:18:26 2005
 *
 * $Revision: 1.10 $
 * 
 */
/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <sent/stddefs.h>
#include <sent/adin.h>

#include <AudioToolbox/AudioToolbox.h>

static OSStatus OutputCallback(void *inRefCon,
                               AudioUnitRenderActionFlags *ioActionFlags,
                               const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber,
                               UInt32 inNumberFrames,
                               AudioBufferList *ioData)
{
    return noErr;
}

/** 
 * Device initialization: check device capability and open for recording.
 * 
 * @param sfreq [in] required sampling frequency.
 * @param dummy [in] a dummy data
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
adin_mic_standby(int sfreq, void *dummy)
{
    OSStatus status;
    
    AudioComponentDescription cd = { 0 };
    cd.componentType         = kAudioUnitType_Output;
    cd.componentSubType      = kAudioUnitSubType_RemoteIO;
    cd.componentManufacturer = kAudioUnitManufacturer_Apple;
    

    AudioUnit outputUnit;
    AudioComponent comp = AudioComponentFindNext(NULL, &cd);
    status = AudioComponentInstanceNew(comp, &outputUnit);
    
    AURenderCallbackStruct callback = { 0 };
    callback.inputProc = OutputCallback;
    status = AudioUnitSetProperty(outputUnit,
                                  kAudioUnitProperty_SetRenderCallback,
                                  kAudioUnitScope_Global, 0,
                                  &callback, sizeof(callback));
    
    AudioStreamBasicDescription outputFormat;
    outputFormat.mSampleRate = 16000;
    outputFormat.mFormatID = kAudioFormatLinearPCM;
    outputFormat.mFormatFlags = kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
    outputFormat.mBitsPerChannel = 16;
    outputFormat.mChannelsPerFrame = 2;
    outputFormat.mFramesPerPacket = 1;
    outputFormat.mBytesPerFrame = outputFormat.mBitsPerChannel / 8 * outputFormat.mChannelsPerFrame;
    outputFormat.mBytesPerPacket = outputFormat.mBytesPerFrame * outputFormat.mFramesPerPacket;
    status = AudioUnitSetProperty(outputUnit, kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input, 0, &outputFormat, sizeof(AudioStreamBasicDescription));

    status = AudioOutputUnitStart(outputUnit);
    
    return TRUE;
}

/** 
 * Start recording.
 * 
 * @param pathname [in] path name to open or NULL for default
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
adin_mic_begin(char *pathname)
{
    printf("adin_mic_begin()\n");
    return TRUE;
}

/** 
 * Stop recording.
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
adin_mic_end()
{
    printf("adin_mic_begin()\n");
    return TRUE;
}

/**
 * @brief  Read samples from device
 * 
 * Try to read @a sampnum samples and returns actual number of recorded
 * samples currently available.  This function will block until
 * at least one sample can be obtained.
 * 
 * @param buf [out] samples obtained in this function
 * @param sampnum [in] wanted number of samples to be read
 * 
 * @return actural number of read samples, -2 if an error occured.
 */
int
adin_mic_read(SP16 *buf, int sampnum)
{
    printf("adin_mic_read()\n");
    return 0;
}

/** 
 * Function to pause audio input (wait for buffer flush)
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
adin_mic_pause()
{
    printf("adin_mic_pause()\n");
    return TRUE;
}

/** 
 * Function to terminate audio input (disgard buffer)
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
adin_mic_terminate()
{
    printf("adin_mic_terminate()\n");
    return TRUE;
}
/** 
 * Function to resume the paused / terminated audio input
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
adin_mic_resume()
{
    printf("adin_mic_resume()\n");
    return TRUE;
}

/** 
 * 
 * Function to return current input source device name
 * 
 * @return string of current input device name.
 * 
 */
char *
adin_mic_input_name()
{
    return "ios_coreaudio";
}
