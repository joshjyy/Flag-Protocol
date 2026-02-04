#ifndef enemies_H
#define enemies_H

#include "gunTypes.hpp"
#include "player.hpp"
#include <vector>


struct Grunt {
    float health;
    float maxHealth;  // For health bars
    float armour;
    float startX;
    float startY;
    float movingX;
    float movingY;
    bool isDead;
    Pistol pistol;
    float shootCooldown;

    // Default grunt
    Grunt(float x, float y)
        : health(100.0f),
        maxHealth(100.0f),
        armour(0.0f),
        startX(x),
        startY(y),
        movingX(x),
        movingY(y),
        isDead(false),
        pistol(),
        shootCooldown(0.0f)
    {
    }
    // Overload for elite enemies
    // Custom stats (for elite grunts, bosses, etc.)
    Grunt(float x, float y, float hp, float armor)
        : health(hp),
        maxHealth(hp),
        armour(armor),
        startX(x),
        startY(y),
        movingX(x),
        movingY(y),
        isDead(false),
        pistol(),
        shootCooldown(0.0f)
    {
    }
};

// Function declarations
Grunt SpawnGrunt(float startX, float startY);
void UpdateGrunts(std::vector<Grunt>& grunts, Player& player, std::vector<Bullet>& enemyBullets, float dt);
void RenderGrunts(std::vector<Grunt>& grunts, void* mesh, float cameraX, float cameraY);
void CheckBulletGruntCollisions(std::vector<Bullet>& playerBullets, std::vector<Grunt>& grunts, Player& playerInfo);
#endif