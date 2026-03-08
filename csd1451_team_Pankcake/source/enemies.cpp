#include "enemies.hpp"
#include "AI.hpp"
#include "gameConstants.hpp"
#include "AEEngine.h"
#include "utils.hpp"

//Constructor for Grunts
Grunt::Grunt(float x, float y)
	: health(100.0f),
	armour(0.0f),
	startX(x),
	startY(y),
	movingX(x),
	movingY(y),
	isDead(false),
	pistol(),
	shootCooldown(0.0f),
	currentState(AI_PATROL),
	detectionRange(AI_DETECTION_RANGE),
	attackRange(AI_ATTACK_RANGE),
	retreatHealthThreshold(AI_RETREAT_HEALTH),
	lastSeenPlayerX(0.0f),
	lastSeenPlayerY(0.0f),
	stateTimer(0.0f),
	patrolRadius(100.0f),
	patrolAngle(0.0f),
	patrolSpeed(AI_PATROL_SPEED),
	investigateTimer(0.0f)
{
}

//SPAWNING
//spawning grunt at whatever x and y
Grunt SpawnGrunt(float startX, float startY) {
	return Grunt(startX, startY);
}

//UPDATING GRUNT
// Update all grunts - shooting logic
void UpdateGrunts(std::vector<Grunt>& grunts, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
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

			//printf("Grunt fired! Ammo: %d/%d\n", grunt.pistol.currentAmmo, grunt.pistol.maxAmmo);
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


//RENDERING
// Render all grunts
void RenderGrunts(std::vector<Grunt>& grunts, void* mesh, float cameraX, float cameraY) {
	AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);

	for (const auto& grunt : grunts) {
		if (grunt.isDead) continue;

		// Convert world position to screen position
		float screenX = grunt.movingX - cameraX;
		float screenY = grunt.movingY - cameraY;

		DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, GRUNT_RADIUS, GRUNT_RADIUS, 0.0f);
	}
}

//COLLISION DETECTION
void CheckBulletGruntCollisions(std::vector<Bullet>& playerBullets, std::vector<Grunt>& grunts, Player& playerInfo) {
	if (playerBullets.empty() || grunts.empty()) {
		return;
	}

	// Loop through bullets backwards for safe removal
	for (int i = static_cast<int>(playerBullets.size()) - 1; i >= 0; i--) {
		for (auto& grunt : grunts) {
			if (grunt.isDead) continue;

			int collision = AreCirclesIntersecting(
				playerBullets[i].x,
				playerBullets[i].y,
				BULLET_RADIUS,
				grunt.movingX,
				grunt.movingY,
				GRUNT_RADIUS / 2
			);

			if (collision > 0) {
				grunt.health -= playerBullets[i].damage;
				printf("Grunt hit! Health: %.1f\n", grunt.health);

				if (grunt.health <= 0.0f) {
					playerInfo.gruntsKilled++;
					grunt.isDead = true;
					//printf("Grunt killed! Grunts KillCount: %d\n", playerInfo.gruntsKilled);


				}

				playerBullets.erase(playerBullets.begin() + i);
				break;
			}
		}
	}
}

//REMOVING
void RemoveDeadGrunts(std::vector<Grunt>& grunts) {
	for (int i = static_cast<int>(grunts.size()) - 1; i >= 0; i--) {
		if (grunts[i].isDead) {
			grunts.erase(grunts.begin() + i);
		}
	}
}