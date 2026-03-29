/*!************************************************************************
  \file weapon.hpp
  \author
  Yeo Kah Boon Joshua (Shooting Code )
  Ye Min Thwin (Reloading)




  \brief
  This source file contains declarations of the stuff related to the weapon system.

**************************************************************************/
#pragma once

void ResetShootCooldown(Player& player, float dt);
void ShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon,
    CameraPos& camera);
void WeaponSwitchCheck(Player& player);
void ReloadBullets(WeaponStats weapon, float dt);
void SetWeaponColor(int weaponIndex);
void GetWeaponColor(int weaponIndex, float& r, float& g, float& b);
void SpreadShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon,
    CameraPos& camera, int pellets, float spreadAngle);