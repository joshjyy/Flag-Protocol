#include "gunTypes.hpp"
#include <vector>
#ifndef BULLET_H
#define BULLET_H
// Bullet structure
struct Bullet {
    float x, y;
    float velocityX, velocityY;
    float damage;
    bool active;
};

struct Player {
    float health;
    // Position
    float x;
    float y;

    // Dash system
    float dashTimer;
    float dashCooldownTimer;
    float dashDirX;
    float dashDirY;

    // Weapon system
    Pistol pistol;
    AssaultRifle rifle;
    int currentWeaponIndex;  // 0 = pistol, 1 = rifle
    float shootCooldown;

    bool inDash;


};

Bullet CreateBullet(float x, float y, float targetX, float targetY, float speed, float damage);
Player InitPlayer(void);
void DrawPlayer(Player& player, void* mesh, float cameraX, float cameraY);
void DrawPlayerHealth(Player& player, void* mesh);
void NormalizeVector(float* x, float* y);
void UpdateBullets(std::vector<Bullet>& bullets, float dt);
void RenderBullets(const std::vector<Bullet>& bullets, void* mesh, float cameraX, float cameraY);
void ClearBullets(std::vector<Bullet>& bullets);  // Optional - replaces FreeBullets
int GetBulletCount(const std::vector<Bullet>& bullets);
WeaponStats GetCurrentWeapon(Player& player);
void UpdatePlayerMovement(Player* player, float dt); 
void UpdatePlayerTimers(Player* player, float dt);

#endif 