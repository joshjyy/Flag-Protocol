#include "player.hpp"
#include "AEEngine.h"
#include "utils.hpp"
#include "gunTypes.hpp"


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
        0.0f                      // shootCooldown
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

    // WRONG - still using player.x, player.y:
    // DrawSprite((AEGfxVertexList*)mesh, player.x, player.y, 50.0f, 50.0f, 0.0f);

    // CORRECT - use screenX, screenY:
    DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, 50.0f, 50.0f, 0.0f);
}

void DrawPlayerHealth(Player& player, void* mesh) {
    // Draw Background Health Bar (top left)
    AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f);
    DrawSprite((AEGfxVertexList*)mesh, 150, 400.0f, 500.0f, 30.0f, 0.0f);

    // Draw Current Health (top left)
    AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f);
    float healthbarOffset = (1000.0f - player.health) / 2.0f;
    DrawSprite((AEGfxVertexList*)mesh, 150 - healthbarOffset, 400.0f, player.health, 30.0f, 0.0f);
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

void UpdateBullets(std::vector<Bullet>& bullets, float dt) {
	// Backward loop to safely remove bullets
    for (int i = (int)bullets.size() - 1; i >= 0; i--) {
        // Update position
        bullets[i].x += bullets[i].velocityX * dt;
        bullets[i].y += bullets[i].velocityY * dt;

        // Check if off screen
        /*if (bullets[i].y > 500.0f || bullets[i].y < -500.0f ||
            bullets[i].x > 900.0f || bullets[i].x < -900.0f) {

            bullets.erase(bullets.begin() + i);  // Remove bullet
        } 
        Not needed beacuse the map is open map
        */ 

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

void UpdatePlayerMovement(Player* player, float dt) {
    // Check for dash input
    if (AEInputCheckTriggered(AEVK_SPACE) && player->dashCooldownTimer <= 0.0f) {
        float inputX = 0.0f;
        float inputY = 0.0f;

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
        // During dash - locked into dash direction
        player->x += player->dashDirX * dashSpeed * dt;  
        player->y += player->dashDirY * dashSpeed * dt;
    }
    else {
        if (AEInputCheckCurr(AEVK_W)) {
            player->y += walkSpeed * dt;   
        }
        if (AEInputCheckCurr(AEVK_A)) {
            player->x -= walkSpeed * dt;  
        }
        if (AEInputCheckCurr(AEVK_S)) {
            player->y -= walkSpeed * dt;   
        }
        if (AEInputCheckCurr(AEVK_D)) {
            player->x += walkSpeed * dt;   
        }
    }
}