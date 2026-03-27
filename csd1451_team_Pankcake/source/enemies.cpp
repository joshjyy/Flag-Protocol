/*!************************************************************************
  \file enemies.hpp
  \author






  \brief
  This source file contains definitions for the enemies.

**************************************************************************/

#include "enemies.hpp"
#include "AI.hpp"
#include "gameConstants.hpp"
#include "AEEngine.h"
#include "map.hpp"
#include "utils.hpp"
#include "damageNumbers.hpp"
#include <cstdlib> 
#include "sound.hpp"

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
	timeAlive(0.f),
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
	investigateTimer(0.0f),
	deathExplosionSpawned(false),
	deathEmitter(nullptr)
{

}

//SPAWNING
//spawning grunt at whatever x and y
Grunt SpawnGrunt(float startX, float startY) {
	Grunt g(startX, startY);
	InitGruntParticles(&g, (AEGfxVertexList*)meshes.square);
	return g;
}

//UPDATING GRUNT
// Update all grunts - shooting logic
void UpdateGrunts(std::vector<Grunt>& grunts, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::vector<std::vector<int>>& gameMap) {
	for (auto& grunt : grunts) {
		if (grunt.isDead) {  
			// Spawn death explosion particles if not already spawned
			if (!grunt.deathExplosionSpawned) {
				ParticleEmitterEmit(grunt.deathEmitter, grunt.movingX, grunt.movingY, 64);
				grunt.deathExplosionSpawned = true;
			}
			ParticleEmitterUpdate(grunt.deathEmitter, dt);
			continue;
		}
		grunt.timeAlive += dt;

		// Cull inactive grunts that haven't engaged the player after 30 seconds
		if (grunt.timeAlive >= 30.0f &&
			grunt.currentState == AI_PATROL &&
			!grunt.isDead)
		{
			grunt.isDead = true;
			grunt.currentState = AI_DEAD;
			continue;
		}

		UpdateGruntAI(grunt, player, enemyBullets, dt, gameMap);
	}
}

//Initialise the spawner
void InitGruntSpawner() {
	gruntSpawner.waveTimer = 0.0f;
	gruntSpawner.waveInterval = 5.0f;
}

//RENDERING
// Render all grunts
void RenderGrunts(std::vector<Grunt>& grunts, void* mesh, CameraPos& camera) {
	//Lamda  to offset particles for camera movement
	auto offsetParticles = [&](ParticleEmitter* emitter, float dx, float dy) {
		for (int i = 0; i < MAX_PARTICLES; i++) {
			auto& p = emitter->particles[i];
			if (p.active) {
				p.x += dx;
				p.y += dy;
			}
		}
		};
	auto drawEmitter = [&](ParticleEmitter* emitter) {
		if (!emitter) return;
		offsetParticles(emitter, -camera.x, -camera.y);
		ParticleEmitterDraw(emitter);
		offsetParticles(emitter, camera.x, camera.y);
		};

	AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);

	for (const auto& grunt : grunts) {
		if (grunt.isDead) {
			drawEmitter(grunt.deathEmitter);
			continue;
		}

		// Convert world position to screen position
		float screenX = grunt.movingX - camera.x;
		float screenY = grunt.movingY - camera.y;

		DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, GRUNT_RADIUS, GRUNT_RADIUS, 0.0f);
	}
}

void UpdateGruntSpawner(std::vector<Grunt>& grunts,
	std::vector<std::vector<int>>& gameMap,
	float dt)
{
	gruntSpawner.waveTimer += dt;
	if (gruntSpawner.waveTimer < gruntSpawner.waveInterval)
		return;

	gruntSpawner.waveTimer = 0.0f;

	// Don't spawn if at max capacity
	if ((int)grunts.size() >= 25)
		return;

	// Find all spawner tiles
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

	// Each spawner tile independently rolls its own spawn count
	for (const auto& tile : spawnTiles) {
		if ((int)grunts.size() >= 25)
			break;

		// 33% chance to skip this spawner entirely
		if (rand() % 3 == 0)
			continue;

		float tileWorldX = (tile.col * TILE_SIZE) - 800.0f + 25.0f;
		float tileWorldY = 450.0f - (tile.row * TILE_SIZE) - 25.0f;
		float radius = 3.0f * (float)TILE_SIZE;

		// Each tile rolls its own random count between 1 and 3
		int spawnCount = 1 + (rand() % 3);

		// Clamp to not exceed max
		int maxNew = 25 - (int)grunts.size();
		if (spawnCount > maxNew)
			spawnCount = maxNew;

		for (int n = 0; n < spawnCount; n++) {
			if ((int)grunts.size() >= 25)
				break;

			bool validSpawn = false;
			float spawnX = tileWorldX;
			float spawnY = tileWorldY;

			// Try up to 10 times to find a valid position
			for (int attempt = 0; attempt < 10; attempt++) {
				float angle = ((float)(rand() % 360)) * 3.14159f / 180.0f;
				float dist = ((float)(rand() % (int)radius));
				float tryX = tileWorldX + dist * cosf(angle);
				float tryY = tileWorldY + dist * sinf(angle);

				// Convert world pos back to tile indices (matching Load_Map formula exactly)
				// x = (j * TILE_SIZE) - 800 + 25  =>  j = (x + 800 - 25) / TILE_SIZE
				// y = 450 - (i * TILE_SIZE) - 25  =>  i = (450 - y - 25) / TILE_SIZE
				int tileCol = static_cast<int>(floorf((tryX + 800.0f - 25.0f) / (float)TILE_SIZE));
				int tileRow = static_cast<int>(floorf((450.0f - tryY - 25.0f) / (float)TILE_SIZE));

				if (tileCol < 1 || tileCol >= MAP_WIDTH - 1 || tileRow < 1 || tileRow >= MAP_HEIGHT - 1)
					continue;

				// Check 3x3 area so grunt body doesn't clip into adjacent walls
				bool blocked = false;
				for (int dr = -1; dr <= 1 && !blocked; dr++) {
					for (int dc = -1; dc <= 1 && !blocked; dc++) {
						if (gameMap[tileRow + dr][tileCol + dc] != 0)
							blocked = true;
					}
				}

				if (!blocked) {
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
				if (playerInfo.currentWeaponIndex == 0) {
					if (playerInfo.health < 80.0f) playerInfo.health += 20;
					else {
						playerInfo.health = 100.0f;
						AEAudioPlay(gHealSound, gSFXGroup, 0.3f, 1.0f, 0);
					}
				};
				// Spawn damage number at grunt position
				SpawnDamageNumber(grunt.movingX, grunt.movingY, playerBullets[i].damage, false);

				printf("Grunt hit! Health: %.1f\n", grunt.health);

				if (grunt.health <= 0.0f && !grunt.isDead) {
					playerInfo.gruntsKilled++;

					playerInfo.pistol.currentAmmo += 5;
					if (playerInfo.pistol.currentAmmo > playerInfo.pistol.maxAmmo)
						playerInfo.pistol.currentAmmo = playerInfo.pistol.maxAmmo;

					playerInfo.rifle.currentAmmo += 10;
					if (playerInfo.rifle.currentAmmo > playerInfo.rifle.maxAmmo)
						playerInfo.rifle.currentAmmo = playerInfo.rifle.maxAmmo;

					playerInfo.shotgun.currentAmmo += 2;
					if (playerInfo.shotgun.currentAmmo > playerInfo.shotgun.maxAmmo)
						playerInfo.shotgun.currentAmmo = playerInfo.shotgun.maxAmmo;

					playerInfo.score += 10;
					AEAudioPlay(gHealSound, gSFXGroup, 0.3f, 1.0f, 0);
					if (playerInfo.health < 85.0f) playerInfo.health += 15.0f;
					else playerInfo.health = 100.0f;

					grunt.isDead = true;
					grunt.currentState = AI_DEAD;

					// Play death sound
					AEAudioPlay(EDeathSound, gSFXGroup, 1.0f, 1.0f, 0);

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
		if (!grunts[i].isDead) continue;

		// Wait until explosion has been spawned first
		if (!grunts[i].deathExplosionSpawned) continue;

		// Then wait for all particles to finish
		bool allDone = true;
		for (int j = 0; j < MAX_PARTICLES; j++) {
			if (grunts[i].deathEmitter->particles[j].active) {
				allDone = false;
				break;
			}
		}
		//Once explosion is done, free emitter and remove grunt from vector
		if (allDone) {
			ParticleEmitterFree(&grunts[i].deathEmitter);
			grunts.erase(grunts.begin() + i);
		}
	}
}


void InitGruntParticles(Grunt* grunt, void* mesh) {
	grunt->deathEmitter = ParticleEmitterCreate();
	ParticleEmitterSetMesh(grunt->deathEmitter, (AEGfxVertexList*)mesh);
	ParticleEmitterSetColor(grunt->deathEmitter, 1.0f, 0.0f, 0.0f, 1.0f);
	ParticleEmitterSetVelocity(grunt->deathEmitter, -150.0f, 150.0f, -150.0f, 150.0f);
	ParticleEmitterSetLifeTime(grunt->deathEmitter, 0.3f, 0.8f);
	ParticleEmitterSetScale(grunt->deathEmitter, 4.0f, 12.0f);
}

void FreeGrunts(std::vector<Grunt>& grunts) {
	for (auto& grunt : grunts) {
		ParticleEmitterFree(&grunt.deathEmitter);
	}
	grunts.clear();
}