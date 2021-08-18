<<<<<<< HEAD
/*
 * Fast, threaded Opus audio streaming example using libopusfile
 * for libctru on Nintendo 3DS
 * 
 *     Originally written by Lauren Kelly (thejsa) with lots of help
 * from mtheall, who re-architected the decoding and buffer logic to be
 * much more efficient as well as overall making the code half decent :)
 * 
 *     Thanks also to David Gow for his example code, which is in the
 * public domain & explains in excellent detail how to use libopusfile:
 * https://davidgow.net/hacks/opusal.html
 * 
 * Last update: 2020-05-16
 */

=======
>>>>>>> 78cccca472e6b954d249446dd6bd13192daa3846
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#include <opus/opusfile.h>
#include <3ds.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// ---- DEFINITIONS ----

static const char *PATH = "romfs:/assets/sound/entity/player/hit.opus";  // Path to Opus file to play

static const int SAMPLE_RATE = 48000;            // Opus is fixed at 48kHz
static const int SAMPLES_PER_BUF = SAMPLE_RATE * 120 / 1000;  // 120ms buffer
static const int CHANNELS_PER_SAMPLE = 2;        // We ask libopusfile for
                                                 // stereo output; it will down
                                                 // -mix for us as necessary.

static const int THREAD_AFFINITY = -1;           // Execute thread on any core
static const int THREAD_STACK_SZ = 32 * 1024;    // 32kB stack for audio thread

static const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE
    * sizeof(int16_t);                           // Size of NDSP wavebufs

int totalSamples=0;
// ---- END DEFINITIONS ----

<<<<<<< HEAD
ndspWaveBuf s_waveBufs[3];
int16_t *s_audioBuffer = NULL;

LightEvent s_event;
volatile bool s_quit = false;  // Quit flag

// ---- HELPER FUNCTIONS ----
=======
ndspWaveBuf s_waveBufs0[3];
ndspWaveBuf s_waveBufs1[3];

int16_t *s_audioBuffer0 = NULL;
int16_t *s_audioBuffer1 = NULL;

LightEvent s_event, s_event1;
volatile bool s_quit0 = false, s_quit1 = false;
>>>>>>> 78cccca472e6b954d249446dd6bd13192daa3846

// Retrieve strings for libopusfile errors
// Sourced from David Gow's example code: https://davidgow.net/files/opusal.cpp
const char *opusStrError(int error)
{
<<<<<<< HEAD
    switch(error) {
        case OP_FALSE:
            return "OP_FALSE: A request did not succeed.";
        case OP_HOLE:
            return "OP_HOLE: There was a hole in the page sequence numbers.";
        case OP_EREAD:
            return "OP_EREAD: An underlying read, seek or tell operation "
                   "failed.";
        case OP_EFAULT:
            return "OP_EFAULT: A NULL pointer was passed where none was "
                   "expected, or an internal library error was encountered.";
        case OP_EIMPL:
            return "OP_EIMPL: The stream used a feature which is not "
                   "implemented.";
        case OP_EINVAL:
            return "OP_EINVAL: One or more parameters to a function were "
                   "invalid.";
        case OP_ENOTFORMAT:
            return "OP_ENOTFORMAT: This is not a valid Ogg Opus stream.";
        case OP_EBADHEADER:
            return "OP_EBADHEADER: A required header packet was not properly "
                   "formatted.";
        case OP_EVERSION:
            return "OP_EVERSION: The ID header contained an unrecognised "
                   "version number.";
        case OP_EBADPACKET:
            return "OP_EBADPACKET: An audio packet failed to decode properly.";
        case OP_EBADLINK:
            return "OP_EBADLINK: We failed to find data we had seen before or "
                   "the stream was sufficiently corrupt that seeking is "
                   "impossible.";
        case OP_ENOSEEK:
            return "OP_ENOSEEK: An operation that requires seeking was "
                   "requested on an unseekable stream.";
        case OP_EBADTIMESTAMP:
            return "OP_EBADTIMESTAMP: The first or last granule position of a "
                   "link failed basic validity checks.";
        default:
            return "Unknown error.";
    }
}

// Pause until user presses a button
void waitForInput(void) {
    printf("Press any button to exit...\n");
    while(aptMainLoop())
    {
        gspWaitForVBlank();
        gfxSwapBuffers();
        hidScanInput();

        if(hidKeysDown())
            break;
    }
}

// ---- END HELPER FUNCTIONS ----

// Audio initialisation code
// This sets up NDSP and our primary audio buffer
bool audioInit(void) {
    // Setup NDSP
    ndspChnReset(0);
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
    ndspChnSetRate(0, SAMPLE_RATE);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

    // Allocate audio buffer
    const size_t bufferSize = WAVEBUF_SIZE * ARRAY_SIZE(s_waveBufs);
    s_audioBuffer = (int16_t *)linearAlloc(bufferSize);
    if(!s_audioBuffer) {
        printf("Failed to allocate audio buffer\n");
        return false;
    }

    // Setup waveBufs for NDSP
    memset(&s_waveBufs, 0, sizeof(s_waveBufs));
    int16_t *buffer = s_audioBuffer;

    for(size_t i = 0; i < ARRAY_SIZE(s_waveBufs); ++i) {
        s_waveBufs[i].data_vaddr = buffer;
        s_waveBufs[i].status     = NDSP_WBUF_DONE;

        buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
    }

    return true;
=======
	switch (error) {
	case OP_FALSE:
		return "OP_FALSE: A request did not succeed.";
	case OP_HOLE:
		return "OP_HOLE: There was a hole in the page sequence numbers.";
	case OP_EREAD:
		return "OP_EREAD: An underlying read, seek or tell operation "
			"failed.";
	case OP_EFAULT:
		return "OP_EFAULT: A NULL pointer was passed where none was "
			"expected, or an internal library error was encountered.";
	case OP_EIMPL:
		return "OP_EIMPL: The stream used a feature which is not "
			"implemented.";
	case OP_EINVAL:
		return "OP_EINVAL: One or more parameters to a function were "
			"invalid.";
	case OP_ENOTFORMAT:
		return "OP_ENOTFORMAT: This is not a valid Ogg Opus stream.";
	case OP_EBADHEADER:
		return "OP_EBADHEADER: A required header packet was not properly "
			"formatted.";
	case OP_EVERSION:
		return "OP_EVERSION: The ID header contained an unrecognised "
			"version number.";
	case OP_EBADPACKET:
		return "OP_EBADPACKET: An audio packet failed to decode properly.";
	case OP_EBADLINK:
		return "OP_EBADLINK: We failed to find data we had seen before or "
			"the stream was sufficiently corrupt that seeking is "
			"impossible.";
	case OP_ENOSEEK:
		return "OP_ENOSEEK: An operation that requires seeking was "
			"requested on an unseekable stream.";
	case OP_EBADTIMESTAMP:
		return "OP_EBADTIMESTAMP: The first or last granule position of a "
			"link failed basic validity checks.";
	default:
		return "Unknown error.";
	}
}

void DoQuit(int _channel)
{
	switch (_channel)
	{
	case 0: s_quit0 = true;
		LightEvent_Signal(&s_event);  
		break;
	case 1:	s_quit1 = true;
		LightEvent_Signal(&s_event1);
		break;
	}
}

// Audio initialisation code
// This sets up NDSP and our primary audio buffer
bool audioInit(int _channel) {
	// Setup NDSP
	ndspChnReset(_channel);
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspChnSetInterp(_channel, NDSP_INTERP_POLYPHASE);
	ndspChnSetRate(_channel, SAMPLE_RATE);
	ndspChnSetFormat(_channel, NDSP_FORMAT_STEREO_PCM16);

	// Allocate audio buffer
	const size_t bufferSize = WAVEBUF_SIZE * ARRAY_SIZE(s_waveBufs0);
	if (_channel == 0)
	{
		s_audioBuffer0 = (int16_t *)linearAlloc(bufferSize);
		if (!s_audioBuffer0) {
			printf("Failed to allocate audio buffer\n");
			return false;
		}
	}
	if (_channel == 1)
	{
		s_audioBuffer1 = (int16_t *)linearAlloc(bufferSize);
		if (!s_audioBuffer1) {
			printf("Failed to allocate audio buffer\n");
			return false;
		}
	}

	// Setup waveBufs for NDSP
	if (_channel == 0)
	{
		memset(&s_waveBufs0, 0, sizeof(s_waveBufs0));
		int16_t *buffer = _channel == 0 ? s_audioBuffer0 : s_audioBuffer1;

		for (size_t i = 0; i < ARRAY_SIZE(s_waveBufs0); ++i) {
			s_waveBufs0[i].data_vaddr = buffer;
			s_waveBufs0[i].status = NDSP_WBUF_DONE;

			buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
		}
	}

	if (_channel == 1)
	{
		memset(&s_waveBufs1, 0, sizeof(s_waveBufs1));
		int16_t *buffer = _channel == 0 ? s_audioBuffer0 : s_audioBuffer1;

		for (size_t i = 0; i < ARRAY_SIZE(s_waveBufs1); ++i) {
			s_waveBufs1[i].data_vaddr = buffer;
			s_waveBufs1[i].status = NDSP_WBUF_DONE;

			buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
		}
	}
	return true;
>>>>>>> 78cccca472e6b954d249446dd6bd13192daa3846
}

// Audio de-initialisation code
// Stops playback and frees the primary audio buffer
<<<<<<< HEAD
void audioExit(void) {
    ndspChnReset(0);
    linearFree(s_audioBuffer);
=======
void audioExit(int _channel) {
	//	ndspChnWaveBufClear(_channel);
	//	printf("ndspChnWaveBufClear done %i\n", _channel);
	ndspChnReset(_channel);
	//printf("ndspChnReset done %i\n", _channel);
	linearFree(_channel == 0 ? s_audioBuffer0 : s_audioBuffer1);
>>>>>>> 78cccca472e6b954d249446dd6bd13192daa3846
}

// Main audio decoding logic
// This function pulls and decodes audio samples from opusFile_ to fill waveBuf_
<<<<<<< HEAD
bool fillBuffer(OggOpusFile *opusFile_, ndspWaveBuf *waveBuf_) {
    #ifdef DEBUG
    // Setup timer for performance stats
    TickCounter timer;
    osTickCounterStart(&timer);
    #endif  // DEBUG

    // Decode samples until our waveBuf is full
    totalSamples = 0;
    while(totalSamples < SAMPLES_PER_BUF) {
        int16_t *buffer = waveBuf_->data_pcm16 + (totalSamples *
            CHANNELS_PER_SAMPLE);
        const size_t bufferSize = (SAMPLES_PER_BUF - totalSamples) *
            CHANNELS_PER_SAMPLE;

        // Decode bufferSize samples from opusFile_ into buffer,
        // storing the number of samples that were decoded (or error)
        const int samples = op_read_stereo(opusFile_, buffer, bufferSize);
        if(samples <= 0) {
            if(samples == 0) break;  // No error here

            printf("op_read_stereo: error %d (%s)", samples,
                   opusStrError(samples));
            break;
        }
        
        totalSamples += samples;
    }

    // If no samples were read in the last decode cycle, we're done
    if(totalSamples == 0) {
        printf("Playback complete, press Start to exit\n");
        return false;
    }

    // Pass samples to NDSP
    waveBuf_->nsamples = totalSamples;
    ndspChnWaveBufAdd(0, waveBuf_);
    DSP_FlushDataCache(waveBuf_->data_pcm16,
        totalSamples * CHANNELS_PER_SAMPLE * sizeof(int16_t));

    #ifdef DEBUG
    // Print timing info
    osTickCounterUpdate(&timer);
    printf("fillBuffer %lfms in %lfms\n", totalSamples * 1000.0 / SAMPLE_RATE,
           osTickCounterRead(&timer));
    #endif  // DEBUG

    return true;
=======
bool fillBuffer(int _channel, OggOpusFile *opusFile_, ndspWaveBuf *waveBuf_) {
#ifdef DEBUG
	// Setup timer for performance stats
	TickCounter timer;
	osTickCounterStart(&timer);
#endif  // DEBUG
	//	printf("fillbuffer started  %i\n", _channel);
	// Decode samples until our waveBuf is full
	int totalSamples = 0;
	while (totalSamples < SAMPLES_PER_BUF) {
		int16_t *buffer = waveBuf_->data_pcm16 + (totalSamples *
			CHANNELS_PER_SAMPLE);
		const size_t bufferSize = (SAMPLES_PER_BUF - totalSamples) *
			CHANNELS_PER_SAMPLE;

		// Decode bufferSize samples from opusFile_ into buffer,
		// storing the number of samples that were decoded (or error)
		const int samples = op_read_stereo(opusFile_, buffer, bufferSize);
		if (samples <= 0) {
			if (samples == 0) break;  // No error here

			printf("op_read_stereo: error %d (%s)", samples,
				opusStrError(samples));
			break;
		}

		totalSamples += samples;
	}

	// If no samples were read in the last decode cycle, we're done
	if (totalSamples == 0) {
		return false;
	}

	// Pass samples to NDSP
	waveBuf_->nsamples = totalSamples;
	ndspChnWaveBufAdd(_channel, waveBuf_);
	DSP_FlushDataCache(waveBuf_->data_pcm16,
		totalSamples * CHANNELS_PER_SAMPLE * sizeof(int16_t));

#ifdef DEBUG
	// Print timing info
	osTickCounterUpdate(&timer);
	printf("fillBuffer %lfms in %lfms\n", totalSamples * 1000.0 / SAMPLE_RATE,
		osTickCounterRead(&timer));
#endif  // DEBUG

	return true;
>>>>>>> 78cccca472e6b954d249446dd6bd13192daa3846
}

// NDSP audio frame callback
// This signals the audioThread to decode more things
// once NDSP has played a sound frame, meaning that there should be
// one or more available waveBufs to fill with more data.
void audioCallback(void *const nul_) {
<<<<<<< HEAD
    (void)nul_;  // Unused

    if(s_quit) { // Quit flag
        return;
    }
    
    LightEvent_Signal(&s_event);
}

// Audio thread
// This handles calling the decoder function to fill NDSP buffers as necessary
void audioThread(void *const opusFile_) {
    OggOpusFile *const opusFile = (OggOpusFile *)opusFile_;

    while(!s_quit) {  // Whilst the quit flag is unset,
                      // search our waveBufs and fill any that aren't currently
                      // queued for playback (i.e, those that are 'done')
        for(size_t i = 0; i < ARRAY_SIZE(s_waveBufs); ++i) {
            if(s_waveBufs[i].status != NDSP_WBUF_DONE) {
                continue;
            }
            
            if(!fillBuffer(opusFile, &s_waveBufs[i])) {   // Playback complete
                return;
            }
        }

        // Wait for a signal that we're needed again before continuing,
        // so that we can yield to other things that want to run
        // (Note that the 3DS uses cooperative threading)
        LightEvent_Wait(&s_event);
    }
}

void playopus() {
    ndspInit();

    // Enable N3DS 804MHz operation, where available
    osSetSpeedupEnable(true);

    // Setup LightEvent for synchronisation of audioThread
    LightEvent_Init(&s_event, RESET_ONESHOT);

    printf("Opus audio streaming example\n"
           "thejsa and mtheall, May 2020\n"
           "Press START to exit\n"
           "\n"
           "Using %d waveBufs, each of length %d bytes\n"
           "    (%d samples; %lf ms @ %d Hz)\n"
           "\n"
           "Loading audio data from path: %s\n"
           "\n",
           ARRAY_SIZE(s_waveBufs), WAVEBUF_SIZE, SAMPLES_PER_BUF,
           SAMPLES_PER_BUF * 1000.0 / SAMPLE_RATE, SAMPLE_RATE,
           PATH);

    // Open the Opus audio file
    int error = 0;
    OggOpusFile *opusFile = op_open_file(PATH, &error);
    if(error) {
        Crash("Failed to open file: error %d (%s)\n", error,
               opusStrError(error));
    }

    // Attempt audioInit
    if(!audioInit()) {
        Crash("Failed to initialise audio\n");
    }

    // Set the ndsp sound frame callback which signals our audioThread
    ndspSetCallback(audioCallback, NULL);

    // Spawn audio thread

    // Set the thread priority to the main thread's priority ...
    int32_t priority = 0x30;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    // ... then subtract 1, as lower number => higher actual priority ...
    priority -= 1;
    // ... finally, clamp it between 0x18 and 0x3F to guarantee that it's valid.
    priority = priority < 0x18 ? 0x18 : priority;
    priority = priority > 0x3F ? 0x3F : priority;

    // Start the thread, passing our opusFile as an argument.
    const Thread threadId = threadCreate(audioThread, opusFile,
                                         THREAD_STACK_SZ, priority,
                                         THREAD_AFFINITY, false);
    printf("Created audio thread %p\n", threadId);

    // Standard main loop
    while(aptMainLoop())
    {
        if(totalSamples==0){
            Crash("test");
        }
    }

    // Signal audio thread to quit
    s_quit = true;
    LightEvent_Signal(&s_event);

    // Free the audio thread
    threadJoin(threadId, UINT64_MAX);
    threadFree(threadId);

    // Cleanup audio things and de-init platform features
    audioExit();
    ndspExit();
    op_free(opusFile);
}
=======
	(void)nul_;  // Unused
	if (s_quit0) { // Quit flag
		return;
	}

	LightEvent_Signal(&s_event);
}

// Audio thread
// This handles calling the decoder function to fill NDSP buffers as necessary
void audioThread0(void *const opusFile_) {
	OggOpusFile *const opusFile = (OggOpusFile *)opusFile_;

	while (!s_quit0) {  // Whilst the quit flag is unset,
					   // search our waveBufs and fill any that aren't currently
					   // queued for playback (i.e, those that are 'done')
		for (size_t i = 0; i < ARRAY_SIZE(s_waveBufs0); ++i) {
			if (s_waveBufs0[i].status != NDSP_WBUF_DONE) {
				continue;
			}

			if (!fillBuffer(0, opusFile, &s_waveBufs0[i])) {   // Playback complete
				return;
			}
		}

		// Wait for a signal that we're needed again before continuing,
		// so that we can yield to other things that want to run
		// (Note that the 3DS uses cooperative threading)
		LightEvent_Wait(&s_event);
	}
}
// Audio thread
// This handles calling the decoder function to fill NDSP buffers as necessary
void audioThread1(void *const opusFile_) {
	OggOpusFile *const opusFile = (OggOpusFile *)opusFile_;

	while (!s_quit1) {  // Whilst the quit flag is unset,
						// search our waveBufs and fill any that aren't currently
						// queued for playback (i.e, those that are 'done')
		for (size_t i = 0; i < ARRAY_SIZE(s_waveBufs1); ++i) {
			if (s_waveBufs1[i].status != NDSP_WBUF_DONE) {
				continue;
			}

			if (!fillBuffer(1, opusFile, &s_waveBufs1[i])) {   // Playback complete
				s_quit1 = true;
				return;
			}
		}

		// Wait for a signal that we're needed again before continuing,
		// so that we can yield to other things that want to run
		// (Note that the 3DS uses cooperative threading)
		LightEvent_Wait(&s_event1);
	}
}

void playopus(Sound *sound) {

	
	

	//kill the  previous (not backgroupd) thread if existing 
	if (sound->background == false && sound->threaid != NULL)
	{		
		DoQuit(1);
		threadJoin(sound->threaid, 50000);
		threadFree(sound->threaid);
		DebugUI_Log("Free thread %p\n", sound->threaid);
		sound->threaid = NULL;	
		if (sound->opusFile != NULL)
		{
			op_free(sound->opusFile);
		}
		audioExit(1);
		sound->threaid = NULL;
		
		
	}
	s_quit1 = false;
	if (sound->background == true)
	{
		LightEvent_Init(&s_event, RESET_ONESHOT);
	}
	else
	{
		LightEvent_Init(&s_event1, RESET_ONESHOT);
	}

	// Open the Opus audio file
	int error = 0;
	OggOpusFile *opusFile = op_open_file(sound->path, &error);
	if (error)
	{
		DebugUI_Log("An Error occured opening file %s \n", sound->path);
		return ;
	}
	if (!audioInit(sound->background == true ? 0 : 1))
	{
		printf("Failed to initialise audio 0\n");
		return ;
	}
	if (sound->background == true)
	{
		ndspSetCallback(audioCallback, NULL);
		
	}
	int32_t priority = 0x30;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
	// ... then subtract 1, as lower number = > higher actual priority ...
	priority -= 1;
	// ... finally, clamp it between 0x18 and 0x3F to guarantee that it's valid.
    priority = priority < 0x18 ? 0x18 : priority;
    priority = priority > 0x3F ? 0x3F : priority;

	Thread threadId = threadCreate(sound->background == true ? audioThread0 : audioThread1, opusFile,THREAD_STACK_SZ, priority,THREAD_AFFINITY, false);
	sound->threaid  = threadId;
	sound->opusFile = opusFile;


	return ;
}

>>>>>>> 78cccca472e6b954d249446dd6bd13192daa3846
