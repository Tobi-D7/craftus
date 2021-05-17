#include <world/worldgen/SuperFlatGen.h>

#include <misc/NumberUtils.h>
#include <blocks/BlockDefs.h>

void SuperFlatGen_Init(SuperFlatGen* gen, World* world) {}

void SuperFlatGen_Generate(WorkQueue* queue, WorkerItem item, void* this) {
	for (int y = 0; y < 17; y++) {
		Block block = BlockDef_Air;
		switch (y) {  // TODO: Mit einem Lookup Table ersetzen, Superflach Gen konfigurierbar machen
			case 0:
				block = BlockDef_Bedrock;
				break;
			case 1 ... 10:
				block = BlockDef_Stone;
				break;
			case 11 ... 15:
				block = BlockDef_Dirt;
				break;
			case 16:
				block = BlockDef_Grass;
				break;
			default:
				block = BlockDef_Air;
				break;
		}
		for (int x = 0; x < CHUNK_SIZE; x++)
			for (int z = 0; z < CHUNK_SIZE; z++) Chunk_SetBlock(item.chunk, x, y, z, block);
	}
}