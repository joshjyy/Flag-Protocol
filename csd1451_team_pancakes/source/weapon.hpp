#include "pch.hpp"

void ResetShootCooldown(Player& player, float dt);
void ShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon, float cameraX, float cameraY);
void WeaponSwitchCheck(Player& player);
void ReloadBullets(WeaponStats weapon, float &reloadTime, float dt);
