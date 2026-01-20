#include "AEEngine.h"
#include "player.hpp"
#include "gunTypes.hpp"
#include "weapon.hpp"


void WeaponSwitchCheck(Player &player) {
	if (AEInputCheckTriggered(AEVK_1)) {
		player.currentWeaponIndex = 0;  // Switch to Pistol
		printf("Pistol Switched! %d\n", player.currentWeaponIndex);

	}
	if (AEInputCheckTriggered(AEVK_2)) {
		player.currentWeaponIndex = 1;  // Switch to Assault Rifle
		printf("Rifle Switched! %d\n", player.currentWeaponIndex);
	}
}

void ResetShootCooldown(Player &player, float dt) {
	if (player.shootCooldown > 0.0f) {
		player.shootCooldown -= dt;
	}
}

void ShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon,
	float cameraX, float cameraY) {  // ADD THESE
	if (AEInputCheckCurr(AEVK_LBUTTON) && player.shootCooldown <= 0.0f) {
		if (*weapon.currentAmmo > 0) {
			// Get mouse position in SCREEN coordinates
			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);

			// Convert screen to centered screen coords
			float screenX = (float)mouseX - 800.0f;
			float screenY = 450.0f - (float)mouseY;

			// *** ADD THIS - Convert screen to world coordinates ***
			float worldMouseX = screenX + cameraX;
			float worldMouseY = screenY + cameraY;

			// Create bullet with current weapon stats
			Bullet newBullet = CreateBullet(player.x, player.y, worldMouseX, worldMouseY, weapon.bulletSpeed, weapon.bulletDamage);
			type.push_back(newBullet);

			// Decrease ammo
			(*weapon.currentAmmo)--;
			printf("Ammo: %d/%d\n", *weapon.currentAmmo, *weapon.maxAmmo);

			// Reset cooldown with current weapon's fire rate
			player.shootCooldown = weapon.fireRate;
		}
	}
}

void ReloadBullets(WeaponStats weapon, float &reloadTime, float dt) {
	// // Basically counts the CD down to 0 >> the time taken to reload
	if (reloadTime > 0.f) {
		printf("Reloading!\n");
		reloadTime -= dt;
		//Once the reload is completed, change current ammo count to max ammo count
		if (reloadTime <= 0.f) {
			*weapon.currentAmmo = *weapon.maxAmmo;
			printf("Reloaded!\n");
		}
	}

	if (AEInputCheckTriggered(AEVK_R) && reloadTime <= 0.f && *weapon.currentAmmo < *weapon.maxAmmo) {
		reloadTime = weapon.reloadSpeed;
	}
}