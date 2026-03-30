
/*!************************************************************************
  \file weapon.cpp
  \author
  Yeo Kah Boon Joshua (Shooting Code )





  \brief
  This source file contains definitions of the stuff related to the weapon system.

**************************************************************************/

#include "AEEngine.h"
#include "player.hpp"
#include "gunTypes.hpp"
#include "weapon.hpp"
#include "sound.hpp"


void WeaponSwitchCheck(Player& player) {
	if (AEInputCheckTriggered(AEVK_1)) {
		player.currentWeaponIndex = 0;
		//printf("%s Switched!\n", GetCurrentWeapon(player).weaponName);
	}
	if (AEInputCheckTriggered(AEVK_2)) {
		player.currentWeaponIndex = 1;
		//printf("%s Switched!\n", GetCurrentWeapon(player).weaponName);
	}
	if (AEInputCheckTriggered(AEVK_3)) {
		player.currentWeaponIndex = 2;
		//printf("%s Switched!\n", GetCurrentWeapon(player).weaponName);
	}

	s32 wheelDelta = 0;
	AEInputMouseWheelDelta(&wheelDelta);

	if (wheelDelta > 0) {
		player.currentWeaponIndex = (player.currentWeaponIndex + 1) % 3;
		//printf("%s Switched!\n", GetCurrentWeapon(player).weaponName);
	}
	if (wheelDelta < 0) {
		player.currentWeaponIndex = (player.currentWeaponIndex + 2) % 3;
		//printf("%s Switched!\n", GetCurrentWeapon(player).weaponName);
	}
}

void ResetShootCooldown(Player &player, float dt) {
	if (player.shootCooldown > 0.0f) {
		player.shootCooldown -= dt;
	}
}

void ShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon, CameraPos& camera) {
	if (CanShoot(player)){
		if (AEInputCheckCurr(AEVK_LBUTTON) && player.shootCooldown <= 0.0f &&
			player.inDash == false && !(*weapon.inReload) && !player.isDead) {

			if (*weapon.currentAmmo > 0) {
				AEAudioPlay(gShoot1Sound, gSFXGroup, 0.6f, 1.0f, 0);

				s32 mouseX, mouseY;
				AEInputGetCursorPosition(&mouseX, &mouseY);

				float screenX = (float)mouseX - 800.0f;
				float screenY = 450.0f - (float)mouseY;

				float worldMouseX = screenX + camera.x;
				float worldMouseY = screenY + camera.y;

				Bullet newBullet = CreateBullet(player.x, player.y, worldMouseX, worldMouseY,
					weapon.bulletSpeed, weapon.bulletDamage);
				type.push_back(newBullet);

				(*weapon.currentAmmo)--;
				//printf("Ammo: %d/%d\n", *weapon.currentAmmo, *weapon.maxAmmo);

				player.shootCooldown = weapon.fireRate;
			}
		}
	}
}

void ReloadBullets(WeaponStats weapon, float dt) {
	// Reload countdown
	if (*weapon.reloadTimer > 0.f) {
		*weapon.reloadTimer -= dt;
		// Once the reload is completed, change current ammo count to max ammo count
		if (*weapon.reloadTimer <= 0.f) {
			*weapon.currentAmmo = *weapon.maxAmmo;
			*weapon.inReload = false;
			//printf("%s Reloaded!\n", weapon.weaponName);
		}
	}

	// Start reload (manual)
	if (AEInputCheckTriggered(AEVK_R) && *weapon.reloadTimer <= 0.f && *weapon.currentAmmo < *weapon.maxAmmo) {
		//printf("%s Reloading...\n", weapon.weaponName);
		*weapon.reloadTimer = weapon.reloadSpeed;
		*weapon.inReload = true;
	}

	// Auto-reload when clip is empty
	if (*weapon.currentAmmo <= 0 && *weapon.reloadTimer <= 0.f && !*weapon.inReload) {
		//printf("%s Auto-reloading...\n", weapon.weaponName);
		*weapon.reloadTimer = weapon.reloadSpeed;
		*weapon.inReload = true;
	}
}


// Spread shooting for shotgun, creates multiple bullets with a spread angle
void SpreadShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon,
	CameraPos& camera, int pellets, float spreadAngle) {

	if (AEInputCheckCurr(AEVK_LBUTTON) && player.shootCooldown <= 0.0f &&
		player.inDash == false && !(*weapon.inReload) && !player.isDead) {

		if (*weapon.currentAmmo > 0) {
			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);

			float screenX = (float)mouseX - 800.0f;
			float screenY = 450.0f - (float)mouseY;
			// Convert screen coordinates to world coordinates
			float worldMouseX = screenX + camera.x;
			float worldMouseY = screenY + camera.y;

			// Base angle to mouse
			float baseAngle = atan2f(worldMouseY - player.y, worldMouseX - player.x);

			// Convert spread to radians
			float halfSpread = AEDegToRad(spreadAngle / 2.0f);
			float angleStep = (pellets > 1) ? (AEDegToRad(spreadAngle) / (pellets - 1)) : 0.0f;

			for (int i = 0; i < pellets; i++) {
				AEAudioPlay(gShoot1Sound, gSFXGroup, 0.6f, 1.0f, 0);
				// Calculate angle for each pellet
				float angle = baseAngle - halfSpread + angleStep * i;
				// Calculate target X position for each pellet based on angle
				float targetX = player.x + AECos(angle) * 100.0f;
				// Calculate target Y position for each pellet based on angle
				float targetY = player.y + AESin(angle) * 100.0f;

				Bullet newBullet = CreateBullet(player.x, player.y, targetX, targetY,
					weapon.bulletSpeed, weapon.bulletDamage);
				type.push_back(newBullet);
			}

			(*weapon.currentAmmo)--;
			player.shootCooldown = weapon.fireRate;
		}
	}
}

// Sets the bullet color based on the current weapon index
void SetWeaponColor(int weaponIndex) {
	if (weaponIndex == 0) {
		AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow - Pistol
	}
	else if (weaponIndex == 1) {
		AEGfxSetColorToMultiply(1.0f, 0.0f, 1.0f, 1.0f);  // Purple - Rifle
	}
	else {
		AEGfxSetColorToMultiply(0.0f, 1.0f, 1.0f, 1.0f);  // Cyan - Shotgun
	}
}

// Gets the RGB color values for the current weapon index, used for bullet coloring
void GetWeaponColor(int weaponIndex, float& r, float& g, float& b) {
	if (weaponIndex == 0) { r = 1.f; g = 1.f; b = 0.f; } // Yellow
	else if (weaponIndex == 1) { r = 1.f; g = 0.f; b = 1.f; } // Purple
	else { r = 0.f; g = 1.f; b = 1.f; } // Cyan
}