

#ifndef WEAPON_SYSTEM_H  
#define WEAPON_SYSTEM_H  


#include "pch.hpp"       





void HandleWeaponSwitching(Player* player);

void UpdateShootCooldown(Player* player, float dt);

void HandleShooting(Player* player, WeaponStats weapon, std::vector<Bullet>* playerBullets);

void UpdateReloadTimer(WeaponStats weapon, float* reloadTime, float dt);

void TryStartReload(WeaponStats weapon, float* reloadTime);

#endif 