#ifndef enemies_H
#define enemies_H

#include "gunTypes.hpp"
#include "player.hpp"
#include "map.hpp"
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

// Function declarations
Grunt SpawnGrunt(float startX, float startY);
void UpdateGrunts(std::vector<Grunt>& grunts, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void RenderGrunts(std::vector<Grunt>& grunts, void* mesh, float cameraX, float cameraY);
void RemoveDeadGrunts(std::vector<Grunt>& grunts);
void CheckBulletGruntCollisions(std::vector<Bullet>& playerBullets, std::vector<Grunt>& grunts, Player& player);

#endif