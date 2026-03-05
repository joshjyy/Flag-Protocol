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

void ShootingCheck(Player& player, std::vector<Bullet>& type, WeaponStats weapon, float cameraX, float cameraY) {
	if (AEInputCheckCurr(AEVK_LBUTTON) && player.shootCooldown <= 0.0f &&
		player.inDash == false && !(*weapon.inReload) && !player.isDead) {

		if (*weapon.currentAmmo > 0) {
			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);

			float screenX = (float)mouseX - 800.0f;
			float screenY = 450.0f - (float)mouseY;

			float worldMouseX = screenX + cameraX;
			float worldMouseY = screenY + cameraY;

			Bullet newBullet = CreateBullet(player.x, player.y, worldMouseX, worldMouseY,
				weapon.bulletSpeed, weapon.bulletDamage);
			type.push_back(newBullet);

			(*weapon.currentAmmo)--;
			printf("Ammo: %d/%d\n", *weapon.currentAmmo, *weapon.maxAmmo);

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


void SetWeaponColor(int weaponIndex) {
	if (weaponIndex == 0) {
		AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow - Pistol
	}
	else if (weaponIndex == 1) {
		AEGfxSetColorToMultiply(1.0f, 0.0f, 1.0f, 1.0f);  // Purple - Rifle
	}
	//else if (weaponIndex == 2) {
	//	AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);  // Red - Shotgun
	//}
	else {
		AEGfxSetColorToMultiply(0.0f, 1.0f, 1.0f, 1.0f);  // Cyan - Default
	}
}