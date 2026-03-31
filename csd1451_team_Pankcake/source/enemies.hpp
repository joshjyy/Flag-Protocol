/*!************************************************************************
  \file enemies.hpp
  \author
  Ye Min Thwin





  \brief
  This source file contains declarations for the enemies.

**************************************************************************/

#pragma once

#include "gameConstants.hpp"
#include "gunTypes.hpp"
#include "player.hpp"
#include "map.hpp"
#include "particle.hpp"
#include "utils.hpp"
#include <vector>

enum GruntAIState :int;

struct Grunt {
    // Stats
    float health;
    float armour;

    // Position
    float startX;
    float startY;
    float movingX;
    float movingY;

    // State
    bool isDead;
    float timeAlive;

    // Weapon
    Pistol pistol;
    float shootCooldown;

    // AI data
    GruntAIState currentState;
    float detectionRange;
    float attackRange;
    float retreatHealthThreshold;
    float lastSeenPlayerX;
    float lastSeenPlayerY;
    float stateTimer;

    // Patrol
    float patrolRadius;
    float patrolAngle;
    float patrolSpeed;
    float investigateTimer;

    ParticleEmitter* deathEmitter;
    bool deathExplosionSpawned;
    // Constructor with member initializer list
    Grunt(float x, float y);

    // Overload for elite enemies
    //// Custom stats (for elite grunts, bosses, etc.)
    //Grunt(float x, float y, float hp, float armor)
    //    : health(hp),
    //    maxHealth(hp),
    //    armour(armor),
    //    startX(x),
    //    startY(y),
    //    movingX(x),
    //    movingY(y),
    //    isDead(false),
    //    pistol(),
    //    shootCooldown(0.0f)
    //{
    //}
};

// Spawner state
struct GruntSpawner {
    float waveTimer;
    float waveInterval;  // 5 seconds
};

extern GruntSpawner gruntSpawner;

// Function declarations
void InitGruntSpawner();
Grunt SpawnGrunt(float startX, float startY);
void UpdateGrunts(std::vector<Grunt>& grunts, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::vector<std::vector<int>>& gameMap);
void RenderGrunts(std::vector<Grunt>& grunts, void* mesh, CameraPos& camera);
void UpdateGruntSpawner(std::vector<Grunt>& grunts,
    std::vector<std::vector<int>>& gameMap,
    float dt);
void RemoveDeadGrunts(std::vector<Grunt>& grunts);
void CheckBulletGruntCollisions(std::vector<Bullet>& playerBullets, std::vector<Grunt>& grunts, Player& player);
void FreeGrunts(std::vector<Grunt>& grunts);
void InitGruntParticles(Grunt* grunt, void* mesh);
void DrawWaveTimer(s8 font, AEGfxVertexList* mesh);
