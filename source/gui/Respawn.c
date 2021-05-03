#include <entity/Player.h>

void RespawnUI(Player* player){
    if(player->spawnx!=0&&player->spawny!=0) {
		player->position.x=player->spawnx;
		player->position.y=player->spawny;
		player->position.z=player->spawnz;
		DebugUI_Log("Lol u ded");
		player->hp=20;
	} else {
		DebugUI_Log("No spawn, lol u ded");
		player->position.x=0;
		player->position.y=17;
		player->position.z=0;
		player->hp=20;
	}
}