#include "pch.hpp"

void ResetShootCooldown(Player& player, float dt);
void ShootingCheck(Player& player, std::vector<Bullet> &bullets, WeaponStats weapon);
void WeaponSwitchCheck(Player& player);
void ReloadBullets(WeaponStats weapon, float dt);
