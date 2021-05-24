//This is all trimmed code from the opus example, i barely have a idea of what the fuck i am doing with sound on the 3ds
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#include </usr/include/opus/opusfile.h>
#include <3ds.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <misc/Sound.h>

static const int SAMPLE_RATE = 48000;
static const int SAMPLES_PER_BUF = SAMPLE_RATE * 120 / 1000;
static const int CHANNELS_PER_SAMPLE = 2;

static const int THREAD_AFFINITY = -1;
static const int THREAD_STACK_SZ = 32 * 1024;

static const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE* sizeof(int16_t);

ndspWaveBuf s_waveBufs[3];
int16_t *s_audioBuffer = NULL;

LightEvent s_event;
volatile bool s_quit = false;

bool audioInit(void) {
    ndspChnReset(0);
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
    ndspChnSetRate(0, SAMPLE_RATE);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

    const size_t bufferSize = WAVEBUF_SIZE * ARRAY_SIZE(s_waveBufs);
    s_audioBuffer = (int16_t *)linearAlloc(bufferSize);
    if(!s_audioBuffer) {
        printf("Failed to allocate audio buffer\n");
        return false;
    }
    memset(&s_waveBufs, 0, sizeof(s_waveBufs));
    int16_t *buffer = s_audioBuffer;

    for(size_t i = 0; i < ARRAY_SIZE(s_waveBufs); ++i) {
        s_waveBufs[i].data_vaddr = buffer;
        s_waveBufs[i].status     = NDSP_WBUF_DONE;

        buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
    }
    return true;
}
void audioExit(void) {
    ndspChnReset(0);
    linearFree(s_audioBuffer);
}
bool fillBuffer(OggOpusFile *opusFile_, ndspWaveBuf *waveBuf_) {
    int totalSamples = 0;
    while(totalSamples < SAMPLES_PER_BUF) {
        int16_t *buffer = waveBuf_->data_pcm16 + (totalSamples *
            CHANNELS_PER_SAMPLE);
        const size_t bufferSize = (SAMPLES_PER_BUF - totalSamples) *
            CHANNELS_PER_SAMPLE;
        const int samples = op_read_stereo(opusFile_, buffer, bufferSize);
        totalSamples += samples;
    }
    waveBuf_->nsamples = totalSamples;
    ndspChnWaveBufAdd(0, waveBuf_);
    DSP_FlushDataCache(waveBuf_->data_pcm16,totalSamples * CHANNELS_PER_SAMPLE * sizeof(int16_t));
    return true;
}
void audioCallback(void *const nul_) {
    (void)nul_;
    if(s_quit) {
        return;
    }
    LightEvent_Signal(&s_event);
}
void audioThread(void *const opusFile_) {
    OggOpusFile *const opusFile = (OggOpusFile *)opusFile_;

    while(!s_quit) {
        for(size_t i = 0; i < ARRAY_SIZE(s_waveBufs); ++i) {
            if(s_waveBufs[i].status != NDSP_WBUF_DONE) {
                continue;
            }
            if(!fillBuffer(opusFile, &s_waveBufs[i])) {
                return;
            }
        }
        LightEvent_Wait(&s_event);
    }
}

void playopus(path){
    LightEvent_Init(&s_event, RESET_ONESHOT);
    audioInit();
    ndspSetCallback(audioCallback, NULL);
    OggOpusFile *opusFile = op_open_file(path, NULL);
    int32_t priority = 0x30;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    priority -= 1;
    priority = priority < 0x18 ? 0x18 : priority;
    priority = priority > 0x3F ? 0x3F : priority;
    const Thread threadId = threadCreate(audioThread, opusFile,THREAD_STACK_SZ, priority,THREAD_AFFINITY, false);
}

void audioDeinit(path){
    s_quit = true;
    LightEvent_Signal(&s_event);
    audioExit();
    ndspExit();
    OggOpusFile *opusFile = op_open_file(path, NULL);
    op_free(opusFile);
}
