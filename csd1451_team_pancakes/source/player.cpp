#include "player.hpp"
#include "AEEngine.h"
#include "utils.hpp"
#include "gunTypes.hpp"
#include "weapon.hpp"


Player InitPlayer(void) {
    Player player = {
        500.0f,                  // health
        0.0f,                     // x
        0.0f,                     // y
        0.0f,                     // dashTimer
        0.0f,                     // dashCooldownTimer
        0.0f,                     // dashDirX
        0.0f,                     // dashDirY
        Pistol(),                 // pistol constructor
        AssaultRifle(),           // rifle constructor
        0,                        // currentWeaponIndex (start with pistol)
        0.0f,                    // shootCooldown
        false
    };
    return player;
}


// math (on god im not sure whats happening here)
void NormalizeVector(float* x, float* y) {
    float length = sqrtf((*x) * (*x) + (*y) * (*y));
    if (length > 0.0f) {
        *x /= length;
        *y /= length;
    }
}


void DrawPlayer(Player& player, void* mesh, float cameraX, float cameraY) {
    float screenX = player.x - cameraX;
    float screenY = player.y - cameraY;
    AEGfxSetColorToMultiply(0.0f, 0.0f, 1.0f, 1.0f);


    DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, 50.0f, 50.0f, 0.0f);
}

void DrawPlayerUI(Player& player, WeaponStats& weapon, void* mesh, void* mesh2) {
    float uiX = -600.0f;
    float uiY = -380.0f;
    float barWidth = 300.0f;
    float barHeight = 30.0f;

    //health bar
    AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f);
    DrawSprite((AEGfxVertexList*)mesh, uiX, uiY,
        barWidth, barHeight, 0.0f);
    AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
    float currentHealthWidth = (player.health / 1000.0f) * barWidth;
    float healthOffset = (barWidth - currentHealthWidth) / 2.0f;
    DrawSprite((AEGfxVertexList*)mesh, uiX - healthOffset, uiY, currentHealthWidth, barHeight, 0.0f);

    //ammo Bar
    float ammoY = uiY - 40.0f;
    AEGfxSetColorToMultiply(0.2f, 0.2f, 0.2f, 1.0f);
    DrawSprite((AEGfxVertexList*)mesh, uiX, ammoY,
        barWidth, barHeight, 0.0f);

    SetWeaponColor(player.currentWeaponIndex);


    // Draw ammo bar with weapon color
    float currentAmmoWidth = ((float)*weapon.currentAmmo / (float)*weapon.maxAmmo) * barWidth;
    float ammoOffset = (barWidth - currentAmmoWidth) / 2.0f;
    DrawSprite((AEGfxVertexList*)mesh, uiX - ammoOffset, ammoY, currentAmmoWidth, barHeight, 0.0f);

    // Draw weapon indicator circle 
    float circleX = uiX + barWidth / 2.0f + 30.0f;
    float circleSize = 25.0f;
    DrawSprite((AEGfxVertexList*)mesh2, circleX, ammoY, circleSize, circleSize, 0.0f);
}


//Parameters: starting position (x, y), target position (targetX, targetY), speed
Bullet CreateBullet(float x, float y, float targetX, float targetY, float speed, float damage) {
    Bullet newBullet{};
    newBullet.x = x;
    newBullet.y = y;
    newBullet.damage = damage;

    float dirX = targetX - x;
    float dirY = targetY - y;
    NormalizeVector(&dirX, &dirY);

    newBullet.velocityX = dirX * speed;
    newBullet.velocityY = dirY * speed;
    newBullet.active = true;

    return newBullet;
}

void UpdateBullets(std::vector<Bullet>& bullets, float dt,
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {

    const float BULLET_RADIUS = 5.0f;  // Define bullet size (adjust as needed)

    // Backward loop to safely remove bullets
    for (int i = (int)bullets.size() - 1; i >= 0; i--) {
        // Update position
        bullets[i].x += bullets[i].velocityX * dt;
        bullets[i].y += bullets[i].velocityY * dt;

        // Check collision with walls
        if (checkCollision(bullets[i].x, bullets[i].y, BULLET_RADIUS, gameMap)) {
            bullets.erase(bullets.begin() + i);  // Remove bullet when it hits a wall
        }
    }
}

void RenderBullets(const std::vector<Bullet>& bullets, void* mesh,
    float cameraX, float cameraY) {  // ADD camera parameters
    // Loop through each bullet

    for (const Bullet& bullet : bullets) {
        // *** ADD THIS - Convert world position to screen position ***
        float screenX = bullet.x - cameraX;
        float screenY = bullet.y - cameraY;

        // Outer glow - USE screenX, screenY
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 0.3f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, 40.0f, 40.0f, 0.0f);

        // Middle glow - USE screenX, screenY
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 0.6f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, 30.0f, 30.0f, 0.0f);

        // Core - USE screenX, screenY
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, 20.0f, 20.0f, 0.0f);
    }
}

//Manual Clear All Bullets at End Of Level
void ClearBullets(std::vector<Bullet>& bullets) {
    bullets.clear(); 
}
//Fetch Bullet Count
int GetBulletCount(const std::vector<Bullet>& bullets) {
    return (int)bullets.size();
}
//current ammo and max ammo are addressed bc they are being modified outside this function
WeaponStats GetCurrentWeapon(Player& player) {
    if (player.currentWeaponIndex == 0) {
        // Pistol
        return {
            &player.pistol.currentAmmo,
            &player.pistol.maxAmmo,
            player.pistol.bulletSpeed,
            player.pistol.bulletDamage,
            player.pistol.fireRate,
            player.pistol.reloadSpeed

        };
    }

    else if (player.currentWeaponIndex == 1) {
		// Assault Rifle
        return {
          &player.rifle.currentAmmo,
          &player.rifle.maxAmmo,
          player.rifle.bulletSpeed,
          player.rifle.bulletDamage,
          player.rifle.fireRate,
          player.rifle.reloadSpeed
        };
    }
    else {
		// Default to pistol if index is invalid
        return {
            &player.pistol.currentAmmo,
            &player.pistol.maxAmmo,
            player.pistol.bulletSpeed,
            player.pistol.bulletDamage,
            player.pistol.fireRate,
            player.pistol.reloadSpeed
        };
    }
}


//Movement

// Constants for player behavior
const float dashSpeed = 2000.0f;
const float dashDuration = 0.2f;
const float dashCooldownMax = 0.5f;
const float walkSpeed = 300.0f;

void UpdatePlayerTimers(Player* player, float dt) {
    if (player->dashTimer > 0.0f) {
        player->dashTimer -= dt;
    }
    if (player->dashCooldownTimer > 0.0f) {
        player->dashCooldownTimer -= dt;
    }
    if (player->shootCooldown > 0.0f) {
        player->shootCooldown -= dt;
    }
}

void UpdatePlayerMovement(Player* player, float dt,
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    float radius = 25.0f;  // Consistent radius

    // Check for dash input
    if (AEInputCheckTriggered(AEVK_SPACE) && player->dashCooldownTimer <= 0.0f) {
        float inputX = 0.0f;
        float inputY = 0.0f;
        player->inDash = true;

        if (AEInputCheckCurr(AEVK_W)) inputY += 1.0f;
        if (AEInputCheckCurr(AEVK_S)) inputY -= 1.0f;
        if (AEInputCheckCurr(AEVK_D)) inputX += 1.0f;
        if (AEInputCheckCurr(AEVK_A)) inputX -= 1.0f;

        float length = sqrtf(inputX * inputX + inputY * inputY);
        if (length > 0.0f) {
            player->dashDirX = inputX / length;
            player->dashDirY = inputY / length;
            player->dashTimer = dashDuration;
            player->dashCooldownTimer = dashCooldownMax;
        }
    }

    // Apply movement
    if (player->dashTimer > 0.0f) {
        // During dash
        float oldX = player->x;
        float oldY = player->y;

        player->x += player->dashDirX * dashSpeed * dt;
        player->y += player->dashDirY * dashSpeed * dt;

        // Check collision with CONSISTENT radius
        if (checkCollision(player->x, player->y, radius, gameMap)) {
            player->x = oldX;
            player->y = oldY;
            player->dashTimer = 0.0f;
            player->inDash = false;
        }
    }
    else {
        player->inDash = false;

        // Calculate movement direction
        float moveX = 0.0f;
        float moveY = 0.0f;

        if (AEInputCheckCurr(AEVK_W)) moveY += 1.0f;
        if (AEInputCheckCurr(AEVK_S)) moveY -= 1.0f;
        if (AEInputCheckCurr(AEVK_D)) moveX += 1.0f;
        if (AEInputCheckCurr(AEVK_A)) moveX -= 1.0f;

        // Only move if there's input
        if (moveX != 0.0f || moveY != 0.0f) {
            float oldX = player->x;
            float oldY = player->y;

            player->x += moveX * walkSpeed * dt;
            player->y += moveY * walkSpeed * dt;

            // Check collision after movement
            if (checkCollision(player->x, player->y, radius, gameMap)) {
                player->x = oldX;
                player->y = oldY;
            }
        }
    }
}
