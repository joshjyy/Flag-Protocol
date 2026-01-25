#include "pch.hpp"
#include "enemies.hpp"
#include "AEEngine.h"
#include "utils.hpp"


//spawning grunt at whatever x and y
Grunt SpawnGrunt(float startX, float startY) {
	return Grunt(startX, startY);
}

// Update all grunts - shooting logic
void UpdateGrunts(std::vector<Grunt>& grunts, Player& player, std::vector<Bullet>& enemyBullets, float dt) {
	for (auto& grunt : grunts) {
		// Update shoot cooldown
		if (grunt.shootCooldown > 0.0f) {
			grunt.shootCooldown -= dt;
		}

		// Shoot at player every 2 seconds
		if (grunt.shootCooldown <= 0.0f && grunt.pistol.currentAmmo > 0) {
			// Create bullet aimed at player
			Bullet newBullet = CreateBullet(
				grunt.movingX,
				grunt.movingY,
				player.x,
				player.y,
				grunt.pistol.bulletSpeed,
				grunt.pistol.bulletDamage
			);

			enemyBullets.push_back(newBullet);

			// Decrease ammo
			grunt.pistol.currentAmmo--;

			// Reset cooldown to 2 seconds
			grunt.shootCooldown = 2.0f;

			printf("Grunt fired! Ammo: %d/%d\n", grunt.pistol.currentAmmo, grunt.pistol.maxAmmo);
		}

		// Auto-reload when out of ammo
		if (grunt.pistol.currentAmmo <= 0 && grunt.pistol.reloadTimer <= 0.0f) {
			grunt.pistol.reloadTimer = grunt.pistol.reloadSpeed;
			grunt.pistol.inReload = true;
		}

		// Handle reload countdown
		if (grunt.pistol.reloadTimer > 0.0f) {
			grunt.pistol.reloadTimer -= dt;
			if (grunt.pistol.reloadTimer <= 0.0f) {
				grunt.pistol.currentAmmo = grunt.pistol.maxAmmo;
				grunt.pistol.inReload = false;
			}
		}
	}
}

// Render all grunts
void RenderGrunts(std::vector<Grunt>& grunts, void* mesh) {
	AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);  // Red color for enemies

	for (const auto& grunt : grunts) {
		DrawSprite((AEGfxVertexList*)mesh, grunt.movingX, grunt.movingY, 50.0f, 50.0f, 0.0f);
	}
}