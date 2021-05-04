#include <entity/Player.h>
#include <entity/Damage.h>

void FallDamage(Player* player){
	if (player->velocity.y<=-26.0&&player->grounded==false){
		player->hp=0;
	}
}
