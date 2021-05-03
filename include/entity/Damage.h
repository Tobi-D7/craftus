#pragma once

#include <entity/Player.h>

typedef struct {
    float BlocksFallen;
    float ProjectileDamage;
    float OverTimeDamage;
    float NoArmourDamage;
    float FallDamage;
    float ArmourDamage;
} Damage;

void FallDamage();
void Respawn();