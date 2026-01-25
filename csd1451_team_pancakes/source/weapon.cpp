#include "AEEngine.h"
#include "player.hpp"
#include "gunTypes.hpp"
#include "weapon.hpp"


void WeaponSwitchCheck(Player& player) {
	if (AEInputCheckTriggered(AEVK_1)) {
		player.currentWeaponIndex = 0;
		printf("%s Switched!\n", GetCurrentWeapon(player).weaponName);
	}
	if (AEInputCheckTriggered(AEVK_2)) {
		player.currentWeaponIndex = 1;
		printf("%s Switched!\n", GetCurrentWeapon(player).weaponName);
	}
}

void ResetShootCooldown(Player &player, float dt) {
	if (player.shootCooldown > 0.0f) {
		player.shootCooldown -= dt;
	}
}

void ShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon) {
	if (AEInputCheckCurr(AEVK_LBUTTON) && player.shootCooldown <= 0.0f && !(*weapon.inReload)) {  // reload check as well, cant shoot when reloading
		if (*weapon.currentAmmo > 0) {
			// Get mouse position in world coordinates
			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);
			float worldMouseX = (float)mouseX - 800.0f;
			float worldMouseY = 450.0f - (float)mouseY;
			// Create bullet with current weapon stats
			Bullet newBullet = CreateBullet(player.x, player.y, worldMouseX, worldMouseY, weapon.bulletSpeed, weapon.bulletDamage);
			type.push_back(newBullet);
			// Decrease ammo
			(*weapon.currentAmmo)--;
			// debug stuff
			printf("Ammo: %d/%d\n", *weapon.currentAmmo, *weapon.maxAmmo);
			// Reset cooldown with current weapon's fire rate
			player.shootCooldown = weapon.fireRate;
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
			*weapon.inReload = false;  // Changed to inReload
			printf("%s Reloaded!\n", weapon.weaponName);
		}
	}

	// Start reload
	if (AEInputCheckTriggered(AEVK_R) && *weapon.reloadTimer <= 0.f && *weapon.currentAmmo < *weapon.maxAmmo) {
		printf("%s Reloading...\n", weapon.weaponName);
		*weapon.reloadTimer = weapon.reloadSpeed;
		*weapon.inReload = true; 
	}
}