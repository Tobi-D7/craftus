#include <entity/Player.h>
#include <entity/Damage.h>

void OvertimeDamage(cause,time){
	Player* player;
	if (cause=="fire"){
		while(time<0){
			player->hp-1;
			sleep(1);
			time-1;
		}
		cause==NULL;
		time==NULL;
	}
	/*if(cause==poison){
		while(time<0)
			player->hp-1;
			sleep(1);
			time-1;
	}*/
}
void Respawn(cause){
	Player* player;
	if (player->hp<=0&&player->gamemode!=1/*&&player->totem==true*/){
		if (player->difficulty!=3) { 
			if(player->spawnx!=NAN&&player->spawny!=NAN&&player->spawnz!=NAN) {
				DebugUI_Log("Died by %s",cause);
				player->position.x=player->spawnx;
				player->position.y=player->spawny+0.6;
				player->position.z=player->spawnz;
				player->hp=20;
			} else {
				DebugUI_Log("No spawn, lol u ded");
				player->position.x=0.0;
				player->position.y=player->spawny2;
				player->position.z=0.0;
				player->hp=20;
			}
		} else {
			DebugUI_Log("lol ur world is gone");
			/*char buffer[512];
			sprintf(buffer, "sdmc:/craftus_redesigned/saves/%s", worlds.data[selectedWorld].path);
			delete_folder(buffer);*/
		}
	}
}