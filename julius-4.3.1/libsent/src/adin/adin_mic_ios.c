/**
 * @file   adin_mic_linux.c
 *
 * <JA>
 * @brief  マイク入力 (iOS Core Audio) - デフォルトデバイス
 *
 * マイク入力のための低レベル関数です．
 * インタフェースを明示指定しない (-input mic) 場合に呼ばれます．
 * </JA>
 * <EN>
 * @brief  Microphone input on iOS Core Audio - default device
 *
 * Low level I/O functions for microphone input on iOS Core Audio.
 * This will be called when no device was explicitly specified ("-input mic").
 * </EN>
 *
 * @author Takashi Mizuhiki
 *
 * $Revision: 1.00 $
 * 
 */
/*
 * Copyright (c) 2016 Takashi Mizuhiki
 * All rights reserved
 */

#include <pthread.h>

#include <sent/stddefs.h>
#include <sent/adin.h>

#include <AudioToolbox/AudioToolbox.h>

#define AUDIO_BUFF_SAMPLES 4096

static pthread_mutex_t _mutex_audioin = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t _cond_audioin = PTHREAD_COND_INITIALIZER;

static SP16 audioBuff[AUDIO_BUFF_SAMPLES];
static int audioBuff_wp = 0;
static int audioBuff_rp = 0;

static AudioUnit outputUnit;

static OSStatus OutputCallback(void *inRefCon,
                               AudioUnitRenderActionFlags *ioActionFlags,
                               const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber,
                               UInt32 inNumberFrames,
                               AudioBufferList *ioData)
{
    AudioUnit *outputUnit = inRefCon;
    
    Float32 *outL = ioData->mBuffers[0].mData;
    Float32 *outR = ioData->mBuffers[1].mData;

    OSStatus status;
    status = AudioUnitRender(outputUnit,
                             ioActionFlags,
                             inTimeStamp,
                             1,
                             inNumberFrames,
                             ioData);


    pthread_mutex_lock(&_mutex_audioin);

    for (int i = 0; i < inNumberFrames; i++) {
        // copy to ring buffer with converting from int16_t to float
        Float32 fval = outL[i];
        if (fval > 1.0f) {
            fval = 1.0f;
        }
        
        if (fval < -1.0f) {
            fval = -1.0f;
        }
        
        SP16 val = fval * INT16_MAX;
        
        audioBuff[audioBuff_wp++] = val;
        
        if (audioBuff_wp >= AUDIO_BUFF_SAMPLES) {
            audioBuff_wp = 0;
        }
        
        if (audioBuff_wp == audioBuff_rp) {
            printf("buffer overrun!\n");
        }
    }
    
    pthread_mutex_unlock(&_mutex_audioin);
    
    pthread_cond_signal(&_cond_audioin);
    
    for (int i = 0; i < inNumberFrames; i++) {
        outL[i] = outR[i] = 0.0f;
    }

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
    

    AudioComponent comp = AudioComponentFindNext(NULL, &cd);
    status = AudioComponentInstanceNew(comp, &outputUnit);
    if (status != noErr) {
        printf("adin_mic_standby(): Error in AudioComponentFindNext()\n");
        return FALSE;
    }
    
    AURenderCallbackStruct callback = { 0 };
    callback.inputProc = OutputCallback;
    callback.inputProcRefCon = outputUnit;
    status = AudioUnitSetProperty(outputUnit,
                                  kAudioUnitProperty_SetRenderCallback,
                                  kAudioUnitScope_Global, 0,
                                  &callback, sizeof(callback));
    if (status != noErr) {
        printf("adin_mic_standby(): Error in setting the render callback function\n");
        return FALSE;
    }
    
    UInt32 enable;

    enable = 1;
    status = AudioUnitSetProperty(outputUnit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Input,
                                  1,
                                  &enable, sizeof(enable));
    if (status != noErr) {
        printf("adin_mic_standby(): Error in enabling Remote IO input\n");
        return FALSE;
    }

    enable = 1;
    status = AudioUnitSetProperty(outputUnit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Output,
                                  0,
                                  &enable, sizeof(enable));
    if (status != noErr) {
        printf("adin_mic_standby(): Error in enabling Remote IO output\n");
        return FALSE;
    }

    
    AudioStreamBasicDescription outputFormat = { 0 };
    outputFormat.mSampleRate = sfreq;
    outputFormat.mFormatID = kAudioFormatLinearPCM;
    outputFormat.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved;
    outputFormat.mBitsPerChannel = 32;
    outputFormat.mChannelsPerFrame = 2;
    outputFormat.mFramesPerPacket = 1;
    outputFormat.mBytesPerFrame = outputFormat.mBitsPerChannel / 8;
    outputFormat.mBytesPerPacket = outputFormat.mBytesPerFrame * outputFormat.mFramesPerPacket;

    status = AudioUnitSetProperty(outputUnit, kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Output, 1,
                                  &outputFormat, sizeof(AudioStreamBasicDescription));
    if (status != noErr) {
        printf("adin_mic_standby(): Error in setting output stream format\n");
        return FALSE;
    }

    status = AudioUnitSetProperty(outputUnit, kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input, 0,
                                  &outputFormat, sizeof(AudioStreamBasicDescription));
    if (status != noErr) {
        printf("adin_mic_standby(): Error in setting input stream format\n");
        return FALSE;
    }

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
    OSStatus status = AudioOutputUnitStart(outputUnit);
    if (status != noErr) {
        return FALSE;
    }

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
    OSStatus status = AudioOutputUnitStop(outputUnit);
    if (status != noErr) {
        return FALSE;
    }
    
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
    pthread_mutex_lock(&_mutex_audioin);

    if (audioBuff_wp == audioBuff_rp) {
        // no audio data available
        // wait for new samples coming
        pthread_cond_wait(&_cond_audioin, &_mutex_audioin);
    }
    
    int totalReadLen = 0;
    int copyLen = 0;
    

    // check available audio samples
    int samples = audioBuff_wp - audioBuff_rp;
    if (samples < 0) {
        samples += AUDIO_BUFF_SAMPLES;
    }
    
    if (samples > sampnum) {
        copyLen = sampnum;
    } else {
        copyLen = samples;
    }
    
    if (AUDIO_BUFF_SAMPLES - audioBuff_rp < copyLen) {
        // copy process should be splitted twice
        int len = AUDIO_BUFF_SAMPLES - audioBuff_rp;
        memcpy(buf, &audioBuff[audioBuff_rp], sizeof(SP16) * len);
        totalReadLen += len;
        
        copyLen -= len;
        buf += len;
        memcpy(buf, &audioBuff[0], sizeof(SP16) * copyLen);

        totalReadLen += copyLen;
        audioBuff_rp = copyLen;
    } else {
        // it can copy at once
        memcpy(buf, &audioBuff[audioBuff_rp], sizeof(SP16) * copyLen);
        totalReadLen += copyLen;
        audioBuff_rp += copyLen;
    }
    
    
    pthread_mutex_unlock(&_mutex_audioin);
    
    return totalReadLen;
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
