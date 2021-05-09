#include <entity/Player.h>
#include <entity/Damage.h>

void FallDamage(Player* player){
	if (player->velocity.y<=-26.0&&player->grounded==false){
		player->hp=0;
	}
}

void OvertimeDamage(cause,time){
	Player* player;
	if (cause==1){
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
