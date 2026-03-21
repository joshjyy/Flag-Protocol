#pragma once
#include "pch.hpp"

void ResetShootCooldown(Player& player, float dt);
void ShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon,
    CameraPos& camera);
void WeaponSwitchCheck(Player& player);
void ReloadBullets(WeaponStats weapon, float dt);
void SetWeaponColor(int weaponIndex);
void GetWeaponColor(int weaponIndex, float& r, float& g, float& b);
void SpreadShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon,
    CameraPos& camera, int pellets, float spreadAngle);