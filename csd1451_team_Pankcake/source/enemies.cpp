#include "enemies.hpp"
#include "gameConstants.hpp"
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

		// Shoot at player when shooting goes off cooldown
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
			//Change this if you want them to shoot faster or slower
			grunt.shootCooldown = 2.0f;

			printf("Grunt fired! Ammo: %d/%d\n", grunt.pistol.currentAmmo, grunt.pistol.maxAmmo);
		}

		// Auto-reload when out of ammo
		if (grunt.pistol.currentAmmo <= 0 && grunt.pistol.reloadTimer <= 0.0f) {
			grunt.pistol.reloadTimer = grunt.pistol.reloadSpeed;
			grunt.pistol.inReload = true;
		}

		// Handle reload countdown
		// doesnt seem like it works, have to test more
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
void RenderGrunts(std::vector<Grunt>& grunts, void* mesh, float cameraX, float cameraY) {
	AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
	for (const auto& grunt : grunts) {
		// skip dead grunts in case still in list
		if (grunt.isDead) continue;
		// Convert world position to screen position
		float screenX = grunt.movingX - cameraX;
		float screenY = grunt.movingY - cameraY;
		DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, GRUNT_RADIUS, GRUNT_RADIUS, 0.0f);
	}
}


void CheckBulletGruntCollisions(std::vector<Bullet>& playerBullets,std::vector<Grunt>& grunts, Player& playerInfo) {
	if (playerBullets.empty() || grunts.empty()) {
		return;  // Nothing to check, exit early
	}
	//loop through bullets backwards to allow safe removal
	for (int i = static_cast<int>(playerBullets.size()) - 1; i >= 0; i--) {
		for (auto& grunt : grunts) {
			// Skip dead grunts
			if (grunt.isDead) continue;

			int collision = AreCirclesIntersecting(
				playerBullets[i].x,    // bullet center X
				playerBullets[i].y,    // bullet center Y
				5.0f,                 // bullet radius
				grunt.movingX,         // grunt center X
				grunt.movingY,         // grunt center Y
				GRUNT_RADIUS/2          // grunt radius
			);

			if (collision > 0) {  // collision == 1 or 2 means hit
				grunt.health -= playerBullets[i].damage;
				if (grunt.health <= 0.0f) {
					playerInfo.gruntsKilled++;
					grunt.isDead = true;
					printf("Grunt killed! Grunts KillCount: %d\n", playerInfo.gruntsKilled);


				}

				playerBullets.erase(playerBullets.begin() + i);
				break;
			}
		}
	}


}

void RemoveDeadGrunts(std::vector<Grunt>& grunts){
for (int i = static_cast<int>(grunts.size()) - 1; i >= 0; i--) {
		if (grunts[i].isDead) {
			grunts.erase(grunts.begin() + i);
		}
	}
}