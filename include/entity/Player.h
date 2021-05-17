#pragma once

#include <stdbool.h>
#include <world/World.h>

#include <gui/Inventory.h>
#include <inventory/ItemStack.h>
#include <entity/Damage.h>

#include <misc/Raycast.h>
#include <misc/VecMath.h>

#define PLAYER_EYEHEIGHT (1.65f)
#define PLAYER_HEIGHT (1.8f)
#define PLAYER_COLLISIONBOX_SIZE (0.65f)
#define PLAYER_HALFEYEDIFF (0.07f)

#define PLAYER_PLACE_REPLACE_TIMEOUT (0.2f)

typedef struct {
	float3 position;
	float pitch, yaw;
	float bobbing, fovAdd, crouchAdd;
	bool grounded, jumped, sprinting, flying, crouching;
	World* world;

	int fuckofflol;

	float3 view;

	bool autoJumpEnabled;

	float3 velocity;
	float simStepAccum;

	float breakPlaceTimeout;

	int hp;
	int armour;
	int oxygen;
	int hunger;
	int difficulty;
	int falldmg;
	float rndy;

	float spawnx;
	float spawny;
	float spawnz;
	float spawny2;
	int spawnset;

	int gamemode;
	bool cheats;

	ItemStack inventory[54];

	int quickSelectBarSlots;
	int quickSelectBarSlot;
	ItemStack quickSelectBar[INVENTORY_QUICKSELECT_MAXSLOTS];

	Raycast_Result viewRayCast;
	bool blockInSight, blockInActionRange;
} Player;

void Player_Init(Player* player, World* world);

void Player_Update(Player* player,Damage* dmg);

void Player_Move(Player* player, float dt, float3 accl);

void Player_PlaceBlock(Player* player);
void Player_BreakBlock(Player* player);

void Player_Jump(Player* player, float3 accl);