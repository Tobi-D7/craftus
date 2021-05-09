#pragma once

#include <entity/Player.h>

typedef struct {
    int cause;
    int time;
}Damage;


void FallDamage();
void Respawn();
void ProjectileDamage();
void OvertimeDamage(cause, time);
void NoArmourDamage();
void ArmourDamage();