/*!************************************************************************
  \file enemies.cpp
  \author
  Ye Min Thwin

  \brief
  This source file contains definitions for the enemies.
**************************************************************************/

#include "gameConstants.hpp"
#include "enemies.hpp"
#include "AI.hpp"
#include "AEEngine.h"
#include "map.hpp"
#include "utils.hpp"    // DrawSprite, GetNormalizedTextPosition, fontID, meshes
#include "damageNumbers.hpp"
#include <cstdlib>
#include "sound.hpp"
#include "debug.hpp"

GruntSpawner gruntSpawner;

Grunt::Grunt(float x, float y)
    : health(100.0f),
    armour(0.0f),
    startX(x),
    startY(y),
    movingX(x), // current position starts at spawn point
    movingY(y),
    isDead(false),
    timeAlive(0.f),
    pistol(),
    shootCooldown(0.0f),
    currentState(AI_PATROL),    // grunts begin in patrol state
    detectionRange(AI_DETECTION_RANGE),
    attackRange(AI_ATTACK_RANGE),
    retreatHealthThreshold(AI_RETREAT_HEALTH),
    lastSeenPlayerX(0.0f),  // zero means no last-seen position yet
    lastSeenPlayerY(0.0f),
    stateTimer(0.0f),
    patrolRadius(100.0f),
    patrolAngle(0.0f),
    patrolSpeed(AI_PATROL_SPEED),
    investigateTimer(0.0f),
    deathExplosionSpawned(false),
    deathEmitter(nullptr)
{}

// Create a grunt at the given position and set up its particle emitter
Grunt SpawnGrunt(float startX, float startY) {
    Grunt g(startX, startY);
    InitGruntParticles(&g, (AEGfxVertexList*)meshes.square);
    return g;
}

void UpdateGrunts(std::vector<Grunt>& grunts, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::vector<std::vector<int>>& gameMap) {
    for (auto& grunt : grunts) {
        if (grunt.isDead) {
            // Trigger the death explosion on the first frame after death
            if (!grunt.deathExplosionSpawned) {
                ParticleEmitterEmit(grunt.deathEmitter, grunt.movingX, grunt.movingY, 64);
                grunt.deathExplosionSpawned = true;
            }
            // Keep updating the particle emitter until all particles fade out
            ParticleEmitterUpdate(grunt.deathEmitter, dt);
            continue;
        }

        grunt.timeAlive += dt;

        // Silently cull grunts that never engaged the player to prevent overcrowding
        if (grunt.timeAlive >= GRUNT_CULL_TIME &&
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

// Reset the spawner so waves begin counting from zero on level start
void InitGruntSpawner() {
    gruntSpawner.waveTimer = 0.0f;
    gruntSpawner.waveInterval = GRUNT_WAVE_INTERVAL;
}

void RenderGrunts(std::vector<Grunt>& grunts, void* mesh, CameraPos& camera) {
    // Lambdas to temporarily offset particle positions into screen space for drawing,
    // then restore them to world space afterwards
    auto offsetParticles = [&](ParticleEmitter* emitter, float dx, float dy) {
        for (int i = 0; i < MAX_PARTICLES; i++) {
            auto& p = emitter->particles[i];
            if (p.active) { p.x += dx; p.y += dy; }
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
            // Dead grunts only render their death particles
            drawEmitter(grunt.deathEmitter);
            continue;
        }
        // Convert world position to screen position before drawing
        float screenX = grunt.movingX - camera.x;
        float screenY = grunt.movingY - camera.y;

        AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f); // enemy red tint
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, GRUNT_RADIUS, GRUNT_RADIUS, 0.0f);
        DebugDrawAABB(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, GRUNT_RADIUS / 2, camera, DebugDrawType::ENEMY);
    }
}

void UpdateGruntSpawner(std::vector<Grunt>& grunts,
    std::vector<std::vector<int>>& gameMap,
    float dt)
{
    gruntSpawner.waveTimer += dt;
    // Wait until the wave interval has elapsed before spawning
    if (gruntSpawner.waveTimer < gruntSpawner.waveInterval)
        return;

    gruntSpawner.waveTimer = 0.0f;

    // Do not spawn if we are already at the grunt cap
    if ((int)grunts.size() >= GRUNT_MAX_COUNT)
        return;

    // Collect all spawner tiles from the map (tile value 3)
    struct SpawnTile { int row, col; };
    std::vector<SpawnTile> spawnTiles;

    for (int i = 0; i < MAP_HEIGHT; i++)
        for (int j = 0; j < MAP_WIDTH; j++)
            if (gameMap[i][j] == 3)
                spawnTiles.push_back({ i, j });

    if (spawnTiles.empty()) return;

    for (const auto& tile : spawnTiles) {
        if ((int)grunts.size() >= GRUNT_MAX_COUNT)
            break;

        // Random chance to skip this spawner tile this wave
        if (rand() % GRUNT_SPAWN_SKIP_CHANCE == 0)
            continue;

        // Convert tile indices to world-space centre position
        float tileWorldX = (tile.col * TILE_SIZE) - 800.0f + 25.0f;
        float tileWorldY = 450.0f - (tile.row * TILE_SIZE) - 25.0f;
        float radius = GRUNT_SPAWN_RADIUS_TILES * (float)TILE_SIZE;

        // Each spawner tile spawns a random number of grunts this wave
        int spawnCount = 1 + (rand() % GRUNT_MAX_PER_SPAWNER);
        int maxNew = GRUNT_MAX_COUNT - (int)grunts.size();
        if (spawnCount > maxNew) spawnCount = maxNew;   // clamp to remaining capacity

        for (int n = 0; n < spawnCount; n++) {
            if ((int)grunts.size() >= GRUNT_MAX_COUNT)
                break;

            bool  validSpawn = false;
            float spawnX = tileWorldX;
            float spawnY = tileWorldY;

            // Try random positions around the spawner tile until we find an open one
            for (int attempt = 0; attempt < GRUNT_SPAWN_ATTEMPTS; attempt++) {
                float angle = ((float)(rand() % 360)) * 3.14159f / 180.0f;
                float dist = ((float)(rand() % (int)radius));
                float tryX = tileWorldX + dist * cosf(angle);
                float tryY = tileWorldY + dist * sinf(angle);

                // Convert candidate world position back to tile indices
                int tileCol = static_cast<int>(floorf((tryX + 800.0f - 25.0f) / (float)TILE_SIZE));
                int tileRow = static_cast<int>(floorf((450.0f - tryY - 25.0f) / (float)TILE_SIZE));

                // Reject positions outside the playable map border
                if (tileCol < 1 || tileCol >= MAP_WIDTH - 1 || tileRow < 1 || tileRow >= MAP_HEIGHT - 1)
                    continue;

                // Check a 3x3 area so the grunt body does not clip into adjacent walls
                bool blocked = false;
                for (int dr = -1; dr <= 1 && !blocked; dr++)
                    for (int dc = -1; dc <= 1 && !blocked; dc++)
                        if (gameMap[(size_t)tileRow + dr][(size_t)tileCol + dc] != 0)
                            blocked = true;

                if (!blocked) {
                    spawnX = tryX; spawnY = tryY;
                    validSpawn = true;
                    break;
                }
            }

            if (validSpawn)
                grunts.push_back(SpawnGrunt(spawnX, spawnY));
        }
    }
}

void CheckBulletGruntCollisions(std::vector<Bullet>& playerBullets, std::vector<Grunt>& grunts, Player& playerInfo) {
    if (playerBullets.empty() || grunts.empty()) return;

    // Iterate bullets backwards so erasing one does not invalidate the loop index
    for (int i = static_cast<int>(playerBullets.size()) - 1; i >= 0; i--) {
        for (auto& grunt : grunts) {
            if (grunt.isDead) continue;

            int collision = AreCirclesIntersecting(
                playerBullets[i].x, playerBullets[i].y, BULLET_RADIUS,
                grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2
            );

            if (collision > 0) {
                grunt.health -= playerBullets[i].damage;

                // Pistol lifesteal on hit
                if (playerInfo.currentWeaponIndex == 0) {
                    if (playerInfo.health < PISTOL_LIFESTEAL_THRESHOLD)
                        playerInfo.health += PISTOL_LIFESTEAL_AMOUNT;
                    else {
                        playerInfo.health = PLAYER_MAX_HEALTH;
                        AEAudioPlay(gHealSound, gSFXGroup, 0.3f, 1.0f, 0);
                    }
                }

                // Show floating damage number at the grunt's position
                SpawnDamageNumber(grunt.movingX, grunt.movingY, playerBullets[i].damage, false);
                //printf("Grunt hit! Health: %.1f\n", grunt.health);

                if (grunt.health <= 0.0f && !grunt.isDead) {
                    playerInfo.gruntsKilled++;

                    // Ammo drops
                    playerInfo.pistol.currentAmmo += KILL_AMMO_PISTOL;
                    if (playerInfo.pistol.currentAmmo > playerInfo.pistol.maxAmmo)
                        playerInfo.pistol.currentAmmo = playerInfo.pistol.maxAmmo;

                    playerInfo.rifle.currentAmmo += KILL_AMMO_RIFLE;
                    if (playerInfo.rifle.currentAmmo > playerInfo.rifle.maxAmmo)
                        playerInfo.rifle.currentAmmo = playerInfo.rifle.maxAmmo;

                    playerInfo.shotgun.currentAmmo += KILL_AMMO_SHOTGUN;
                    if (playerInfo.shotgun.currentAmmo > playerInfo.shotgun.maxAmmo)
                        playerInfo.shotgun.currentAmmo = playerInfo.shotgun.maxAmmo;

                    // Score and heal
                    playerInfo.score += KILL_SCORE;
                    AEAudioPlay(gHealSound, gSFXGroup, 0.3f, 1.0f, 0);

                    if (playerInfo.health < KILL_HEAL_FULL_THRESHOLD)
                        playerInfo.health += KILL_HEAL_AMOUNT;
                    else
                        playerInfo.health = PLAYER_MAX_HEALTH;

                    grunt.isDead = true;
                    grunt.currentState = AI_DEAD;

                    AEAudioPlay(EDeathSound, gSFXGroup, 1.0f, 1.0f, 0);
                    //printf("Grunt killed! Total kills: %d\n", playerInfo.gruntsKilled);
                }
                // Remove the bullet that hit and stop checking other grunts for this bullet
                playerBullets.erase(playerBullets.begin() + i);
                break;
            }
        }
    }
}

void RemoveDeadGrunts(std::vector<Grunt>& grunts) {
    for (int i = static_cast<int>(grunts.size()) - 1; i >= 0; i--) {
        if (!grunts[i].isDead) continue;
        // Wait until the death explosion has been triggered before removing
        if (!grunts[i].deathExplosionSpawned) continue;

        // Wait until every particle from the explosion has finished
        bool allDone = true;
        for (int j = 0; j < MAX_PARTICLES; j++) {
            if (grunts[i].deathEmitter->particles[j].active) {
                allDone = false;
                break;
            }
        }
        // Safe to free and erase once all particles are gone
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

// Free every grunt's particle emitter before clearing the vector
void FreeGrunts(std::vector<Grunt>& grunts) {
    for (auto& grunt : grunts)
        ParticleEmitterFree(&grunt.deathEmitter);
    grunts.clear();
}

// Draws a countdown timer showing seconds until the next enemy wave.
// Colour shifts from green to yellow to red as the wave approaches.
void DrawWaveTimer(s8 font, AEGfxVertexList* mesh)
{
    float timeUntilNext = gruntSpawner.waveInterval - gruntSpawner.waveTimer;
    if (timeUntilNext < 0.0f) timeUntilNext = 0.0f;

    char timerStr[32];
    sprintf_s(timerStr, "NEXT WAVE: %.1fs", timeUntilNext);

    // Position – top-centre of screen in world space
    const float POS_X = 0.0f;
    const float POS_Y = 390.0f;
    const float SCALE = 0.9f;
    const float PANEL_W = 195.0f;
    const float PANEL_H = 32.0f;

    // Dark background panel
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(0.6f);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    DrawSprite(mesh, POS_X, POS_Y, PANEL_W, PANEL_H, 0.0f);
    AEGfxSetTransparency(1.0f);

    // Colour: green > 5s, yellow 3-5s, red < 3s
    float r, g, b;
    if (timeUntilNext > 5.0f) { r = 0.0f; g = 1.0f; b = 0.0f; }
    else if (timeUntilNext > 3.0f) { r = 1.0f; g = 1.0f; b = 0.0f; }
    else { r = 1.0f; g = 0.2f; b = 0.2f; }

    // Centre the text over POS_X
    float tw, th;
    AEGfxGetPrintSize(font, timerStr, SCALE, &tw, &th);
    float nx = POS_X / 800.0f - tw / 2.0f;
    float ny = POS_Y / 450.0f - th / 2.0f;

    AEGfxPrint(font, timerStr, nx, ny, SCALE, r, g, b, 1.0f);
}