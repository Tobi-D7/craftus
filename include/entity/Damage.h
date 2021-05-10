#pragma once

#include <entity/Player.h>

typedef struct {
    char cause;
    int time;
}Damage;

void Respawn(cause);
void ProjectileDamage();
void OvertimeDamage(cause, time);
void NoArmourDamage();
void ArmourDamage();