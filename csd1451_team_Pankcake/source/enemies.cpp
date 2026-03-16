#include "enemies.hpp"
#include "AI.hpp"
#include "gameConstants.hpp"
#include "AEEngine.h"
#include "map.hpp"
#include "utils.hpp"
#include "damageNumbers.hpp"
#include <cstdlib> 

GruntSpawner gruntSpawner;

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
		if (!grunt.isDead) {
			UpdateGruntAI(grunt, player, enemyBullets, dt, gameMap);
		}
	}
}

//Initialise the spawner
void InitGruntSpawner() {
	gruntSpawner.waveTimer = 0.0f;
	gruntSpawner.waveInterval = 5.0f;
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

void UpdateGruntSpawner(std::vector<Grunt>& grunts,
	std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap,
	float dt)
{
	gruntSpawner.waveTimer += dt;
	if (gruntSpawner.waveTimer < gruntSpawner.waveInterval)
		return;

	gruntSpawner.waveTimer = 0.0f;
	// Don't spawn if at max capacity of 25.
	if ((int)grunts.size() >= 25)
		return;

	// Find all spawner positions
	struct SpawnTile { int row, col; };
	std::vector<SpawnTile> spawnTiles;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (gameMap[i][j] == 3) {
				spawnTiles.push_back({ i, j });
			}
		}
	}

	if (spawnTiles.empty()) return;

	// Random count between 1 and 3
	int spawnCount = 1 + (rand() % 3);

	// Clamp to not exceed max
	int maxNew = 25 - (int)grunts.size();
	if (spawnCount > maxNew)
		spawnCount = maxNew;

	float radius = 3.0f * (float)TILE_SIZE;  // 3-tile radius in world units

	for (int n = 0; n < spawnCount; n++) {
		// Pick a random spawn tile
		int idx = rand() % (int)spawnTiles.size();
		int row = spawnTiles[idx].row;
		int col = spawnTiles[idx].col;

		// Convert tile to world position
		float tileWorldX = (col * TILE_SIZE) - 800.0f + 25.0f;
		float tileWorldY = 450.0f - (row * TILE_SIZE) - 25.0f;

		float radius = 3.0f * (float)TILE_SIZE;

		// Try up to 5 times to find a valid position
		bool validSpawn = false;
		float spawnX = tileWorldX;
		float spawnY = tileWorldY;

		for (int attempt = 0; attempt < 5; attempt++) {
			float angle = ((float)(rand() % 360)) * 3.14159f / 180.0f;
			float dist = ((float)(rand() % (int)radius));
			float tryX = tileWorldX + dist * cosf(angle);
			float tryY = tileWorldY + dist * sinf(angle);

			// Convert back to tile indices
			int tileCol = (int)((tryX + 800.0f - 25.0f) / TILE_SIZE);
			int tileRow = (int)((450.0f - tryY - 25.0f) / TILE_SIZE);

			// Bounds check
			if (tileCol < 0 || tileCol >= MAP_WIDTH || tileRow < 0 || tileRow >= MAP_HEIGHT)
				continue;

			// Only spawn on empty tiles (0)
			if (gameMap[tileRow][tileCol] == 0) {
				spawnX = tryX;
				spawnY = tryY;
				validSpawn = true;
				break;
			}
		}

		if (validSpawn) {
			grunts.push_back(SpawnGrunt(spawnX, spawnY));
		}
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

				// Spawn damage number at grunt position
				SpawnDamageNumber(grunt.movingX, grunt.movingY, playerBullets[i].damage, false);

				printf("Grunt hit! Health: %.1f\n", grunt.health);

				if (grunt.health <= 0.0f && !grunt.isDead){
					playerInfo.gruntsKilled++;
					// Add ammo, capped at maxMagazine
					printf("Before: pistol=%d, maxAmmo=%d\n", playerInfo.pistol.currentAmmo, playerInfo.pistol.maxAmmo);

					playerInfo.pistol.currentAmmo += 5;
					if (playerInfo.pistol.currentAmmo > playerInfo.pistol.maxAmmo)
						playerInfo.pistol.currentAmmo = playerInfo.pistol.maxAmmo;

					playerInfo.rifle.currentAmmo += 10;
					if (playerInfo.rifle.currentAmmo > playerInfo.rifle.maxAmmo)
						playerInfo.rifle.currentAmmo = playerInfo.rifle.maxAmmo;
					printf("After: pistol=%d\n", playerInfo.pistol.currentAmmo);

					playerInfo.score += 10;  
					if (playerInfo.health < 96.0f) playerInfo.health += 15.0f;
					else {
						playerInfo.health = 100.0f;
						
					}
					grunt.isDead = true;
					grunt.currentState = AI_DEAD;
					printf("Grunt killed! Total kills: %d\n", playerInfo.gruntsKilled);
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