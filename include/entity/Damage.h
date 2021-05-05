#pragma once

#include <entity/Player.h>

typedef struct {
    float BlocksFallen;
} Damage;

void FallDamage();
void Respawn();
void ProjectileDamage();
void OvertimeDamage();
void NoArmourDamage();
void ArmourDamage();