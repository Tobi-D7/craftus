#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <opus/opusfile.h>
#include <3ds.h>

#include <GameStates.h>
#include <entity/Player.h>
#include <entity/PlayerController.h>
#include <entity/Damage.h>
#include <gui/DebugUI.h>
#include <gui/Gui.h>
#include <gui/WorldSelect.h>
#include <rendering/PolyGen.h>
#include <rendering/Renderer.h>
#include <world/ChunkWorker.h>
#include <world/World.h>
#include <world/savegame/SaveManager.h>
#include <world/savegame/SuperChunk.h>
#include <world/worldgen/SmeaGen.h>
#include <world/worldgen/SuperFlatGen.h>

#include <sino/sino.h>
#include <citro3d.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

//Yeah i copied most of the code from the opus example, what are you going to do about it?
static const char *PATH = "romfs:/sample.opus";

static const int SAMPLE_RATE = 48000;
static const int SAMPLES_PER_BUF = SAMPLE_RATE * 120 / 1000;
static const int CHANNELS_PER_SAMPLE = 2;

static const int THREAD_AFFINITY = -1;
static const int THREAD_STACK_SZ = 32 * 1024;

static const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE
    * sizeof(int16_t);

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
        if(samples <= 0) {
            if(samples == 0) break;

            printf("something fucked up");
            break;
        }
        totalSamples += samples;
    }
    waveBuf_->nsamples = totalSamples;
    ndspChnWaveBufAdd(0, waveBuf_);
    DSP_FlushDataCache(waveBuf_->data_pcm16,
        totalSamples * CHANNELS_PER_SAMPLE * sizeof(int16_t));

    return true;
}

void audioCallback(void *const nul_) {
    (void)nul_;

    if(s_quit) {
        return;
    }
    
    LightEvent_Signal(&s_event);
}

bool showDebugInfo = false;

void releaseWorld(ChunkWorker* chunkWorker, SaveManager* savemgr, World* world) {
	for (int i = 0; i < CHUNKCACHE_SIZE; i++) {
		for (int j = 0; j < CHUNKCACHE_SIZE; j++) {
			World_UnloadChunk(world, world->chunkCache[i][j]);
		}
	}
	ChunkWorker_Finish(chunkWorker);
	World_Reset(world);

	SaveManager_Unload(savemgr);
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

int main(int argc, char* argv[]) {
	GameState gamestate = GameState_SelectWorld;
	printf("gfxinit");
	gfxInitDefault();

	gfxSet3D(true);
	printf("romfsinit");
	romfsInit();

	ndspInit();

	SuperFlatGen flatGen;
	SmeaGen smeaGen;

	SuperChunk_InitPools();

	SaveManager_InitFileSystem();

	ChunkWorker chunkWorker;
	ChunkWorker_Init(&chunkWorker);
	ChunkWorker_AddHandler(&chunkWorker, WorkerItemType_PolyGen, (WorkerFuncObj){&PolyGen_GeneratePolygons, NULL, true});
	ChunkWorker_AddHandler(&chunkWorker, WorkerItemType_BaseGen, (WorkerFuncObj){&SuperFlatGen_Generate, &flatGen, true});
	ChunkWorker_AddHandler(&chunkWorker, WorkerItemType_BaseGen, (WorkerFuncObj){&SmeaGen_Generate, &smeaGen, true});

	sino_init();

	World* world = (World*)malloc(sizeof(World));
	Player player;
	PlayerController playerCtrl;
	Player_Init(&player, world);
	PlayerController_Init(&playerCtrl, &player);

	SuperFlatGen_Init(&flatGen, world);
	SmeaGen_Init(&smeaGen, world);

	Renderer_Init(world, &player, &chunkWorker.queue, &gamestate);

	DebugUI_Init();

	OggOpusFile *opusFile = op_open_file(PATH, NULL);
	audioInit();
	ndspSetCallback(audioCallback, NULL);
    int32_t priority = 0x30;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    priority -= 1;
    priority = priority < 0x18 ? 0x18 : priority;
    priority = priority > 0x3F ? 0x3F : priority;
    const Thread threadId = threadCreate(audioThread, opusFile,THREAD_STACK_SZ, priority,THREAD_AFFINITY, false);

	WorldSelect_Init();

	World_Init(world, &chunkWorker.queue);

	SaveManager savemgr;
	SaveManager_Init(&savemgr, &player);
	ChunkWorker_AddHandler(&chunkWorker, WorkerItemType_Load, (WorkerFuncObj){&SaveManager_LoadChunk, &savemgr, true});
	ChunkWorker_AddHandler(&chunkWorker, WorkerItemType_Save, (WorkerFuncObj){&SaveManager_SaveChunk, &savemgr, true});

	uint64_t lastTime = svcGetSystemTick();
	float dt = 0.f, timeAccum = 0.f, fpsClock = 0.f;
	int frameCounter = 0, fps = 0;
	while (aptMainLoop()) {
		//DebugUI_Text("%d FPS  Usage: CPU: %5.2f%% GPU: %5.2f%% Buf: %5.2f%% Lin: %d", fps, C3D_GetProcessingTime() * 6.f,
		//C3D_GetDrawingTime() * 6.f, C3D_GetCmdBufUsage() * 100.f, linearSpaceFree());
		DebugUI_Text("X: %f, Y: %f, Z: %f", f3_unpack(player.position));
		DebugUI_Text("HP: %i",player.hp);
		//DebugUI_Text("velocity: %f rndy: %f",player.velocity.y,player.rndy);
		//DebugUI_Text("Time: %i Cause: %c",dmg->time,dmg->cause);
		//DebugUI_Text("SX: %f SY: %f SZ: %f",player->spawnx,player->spawny,player->spawnz);
		DebugUI_Text("Hunger: %i Hungertimer: %i",player.hunger,player.hungertimer);
		DebugUI_Text("Gamemode: %i",player.gamemode);
		DebugUI_Text("quickbar %i",player.quickSelectBarSlot);

		Renderer_Render();

		uint64_t currentTime = svcGetSystemTick();
		dt = ((float)(currentTime / (float)TICKS_PER_MSEC) - (float)(lastTime / (float)TICKS_PER_MSEC)) / 1000.f;
		lastTime = currentTime;
		timeAccum += dt;

		frameCounter++;
		fpsClock += dt;
		if (fpsClock >= 1.f) {
			fps = frameCounter;
			frameCounter = 0;
			fpsClock = 0.f;
		}

		hidScanInput();
		u32 keysheld = hidKeysHeld(), keysdown = hidKeysDown();
		if (keysdown & KEY_START) {
			if (gamestate == GameState_SelectWorld)
				break;
			else if (gamestate == GameState_Playing) {
				releaseWorld(&chunkWorker, &savemgr, world);

				gamestate = GameState_SelectWorld;

				WorldSelect_ScanWorlds();

				lastTime = svcGetSystemTick();
			}
		}

		circlePosition circlePos;
		hidCircleRead(&circlePos);

		circlePosition cstickPos;
		hidCstickRead(&cstickPos);

		touchPosition touchPos;
		hidTouchRead(&touchPos);

		InputData inputData = (InputData){keysheld,    keysdown,    hidKeysUp(),  circlePos.dx, circlePos.dy,
						  touchPos.px, touchPos.py, cstickPos.dx, cstickPos.dy};

		if (gamestate == GameState_Playing) {
			while (timeAccum >= 1.f / 20.f) {
				World_Tick(world);

				timeAccum -= 1.f / 20.f;
			}

			PlayerController_Update(&playerCtrl, inputData, dt);

			World_UpdateChunkCache(world, WorldToChunkCoord(FastFloor(player.position.x)),
					       WorldToChunkCoord(FastFloor(player.position.z)));
		} else if (gamestate == GameState_SelectWorld) {
			char path[256];
			char name[WORLD_NAME_SIZE] = {'\0'};
			WorldGenType worldType;
			bool newWorld = false;
			if (WorldSelect_Update(path, name, &worldType, &newWorld)) {
				strcpy(world->name, name);
				world->genSettings.type = worldType;

				SaveManager_Load(&savemgr, path);

				ChunkWorker_SetHandlerActive(&chunkWorker, WorkerItemType_BaseGen, &flatGen,
							     world->genSettings.type == WorldGen_SuperFlat);
				ChunkWorker_SetHandlerActive(&chunkWorker, WorkerItemType_BaseGen, &smeaGen,
							     world->genSettings.type == WorldGen_Smea);

				world->cacheTranslationX = WorldToChunkCoord(FastFloor(player.position.x));
				world->cacheTranslationZ = WorldToChunkCoord(FastFloor(player.position.z));
				for (int i = 0; i < CHUNKCACHE_SIZE; i++) {
					for (int j = 0; j < CHUNKCACHE_SIZE; j++) {
						world->chunkCache[i][j] =
						    World_LoadChunk(world, i - CHUNKCACHE_SIZE / 2 + world->cacheTranslationX,
								    j - CHUNKCACHE_SIZE / 2 + world->cacheTranslationZ);
					}
				}

				for (int i = 0; i < 3; i++) {
					while (chunkWorker.working || chunkWorker.queue.queue.length > 0) {
						svcSleepThread(50000000);  // 1 Tick
					}
					World_Tick(world);
				}

				if (newWorld) {
					int highestblock = 0;
					for (int x = -1; x < 1; x++) {
						for (int z = -1; z < 1; z++) {
							int height = World_GetHeight(world, x, z);
							if (height > highestblock) highestblock = height;
						}
					}
					player.hunger=20;
					player.hp=20;
					player.position.y = (float)highestblock + 0.2f;
				}
				gamestate = GameState_Playing;
				lastTime = svcGetSystemTick();  // fix timing
			}
		}
		Gui_InputData(inputData);
	}

	if (gamestate == GameState_Playing) releaseWorld(&chunkWorker, &savemgr, world);

	SaveManager_Deinit(&savemgr);

	SuperChunk_DeinitPools();

	free(world);

	sino_exit();

    s_quit = true;
    LightEvent_Signal(&s_event);
    threadJoin(threadId, UINT64_MAX);
    threadFree(threadId);
    audioExit();
    ndspExit();
    op_free(opusFile);

	WorldSelect_Deinit();

	DebugUI_Deinit();

	ChunkWorker_Deinit(&chunkWorker);

	Renderer_Deinit();

	romfsExit();

	gfxExit();
	return 0;
}
