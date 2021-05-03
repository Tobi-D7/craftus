#include <entity/Player.h>
#include <entity/Damage.h>

void FallDamage(Player* player){
	if (player->velocity.y<=-26.0&&player->grounded==false){
		player->hp=0;
	}
}

void Respawn(Player* player){
    if(player->spawnx!=0&&player->spawnz!=0) {
		DebugUI_Log("Lol u ded");
		player->position.x=player->spawnx;
		player->position.y=player->spawny;
		player->position.z=player->spawnz;
		player->dead=false;
		player->hp=20;
	} else {
		DebugUI_Log("No spawn, lol u ded");
		player->position.x=0;
		player->position.y=17;
		player->position.z=0;
		player->dead=false;
		player->hp=20;
	}
}