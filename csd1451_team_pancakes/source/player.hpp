#include "gunTypes.hpp"

#ifndef BULLET_H
#define BULLET_H
// Bullet structure
struct Bullet {
    float x;
    float y;
    float velocityX;
    float velocityY;
    float damage;
    bool active; 
    Bullet* next;  // Pointer to next bullet in linked list
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


};

Bullet* CreateBullet(float x, float y, float targetX, float targetY, float speed, float damage);

Player InitPlayer(void);
void NormalizeVector(float* x, float* y);
void UpdateBullets(Bullet** bulletHead, float dt);
void RenderBullets(Bullet* bulletHead, void* mesh);
void FreeBullets(Bullet** bulletHead);
int GetBulletCount(Bullet* bulletHead);
WeaponStats GetCurrentWeapon(Player& player);
void UpdatePlayerMovement(Player* player, float dt); 
void UpdatePlayerTimers(Player* player, float dt);

#endif 