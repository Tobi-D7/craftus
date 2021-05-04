#include <misc/CommandLine.h>
#include <world/savegame/SaveManager.h>

#include <3ds.h>

#include <gui/DebugUI.h>
#include <mpack/mpack.h>

#include <stdio.h>
#include <string.h>

void CommandLine_Activate(World* world, Player* player) {
	static SwkbdState swkbd;
	static char textBuffer[64];
	swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, 64);
	swkbdSetHintText(&swkbd, "Enter command");

	int button = swkbdInputText(&swkbd, textBuffer, sizeof(textBuffer));
	if (button == SWKBD_BUTTON_CONFIRM) {
		CommandLine_Execute(world, player, textBuffer);
	}
}

void CommandLine_Execute(World* world, Player* player, const char* text) {
	int length = strlen(text);
	mpack_writer_t writer;
	mpack_error_t err = mpack_writer_destroy(&writer);
	if (length >= 1 && text[0] == '/'&&player->cheats==true) {
		if (length >= 9) {
			float x, y, z;
			if (sscanf(&text[1], "tp %f %f %f", &x, &y, &z) == 3) {
				player->position.x = x;
				player->position.y = y + 1;
				player->position.z = z;
				DebugUI_Log("Teleported to %f, %f %f", x, y, z);
			}
		}
		if (length == 2 && text[1] == 'k') {
			player->hp=0;
			DebugUI_Log("Killed player");
		}
		float hp;
		if ( sscanf(&text[1],"hp %f",&hp)) {
			player->hp=hp;
			DebugUI_Log("Set player hp to %f", hp);
		}
		float x,y,z;
		if (sscanf(&text[1], "ws %f %f %f", &x, &y, &z) == 3) {
			player->spawnx = x;
			player->spawny = y;
			player->spawnz = z;
			mpack_write_cstr(&writer, "sx");
			mpack_write_float(&writer,player->spawnx);
			mpack_write_cstr(&writer, "sy");
			mpack_write_float(&writer,player->spawny);
			mpack_write_cstr(&writer, "sz");
			mpack_write_float(&writer,player->spawnz);
			DebugUI_Log("Set spawn to %f, %f %f", x, y, z);
			if (err != mpack_ok) DebugUI_Log("Mpack error %d while saving world manifest", err);
		}
		int gm;
		if ( sscanf(&text[1],"gm %i",&gm)) {
			player->gamemode=gm;
			DebugUI_Log("Set gamemode to %i", gm);
		}
	}
}