#include "player.hpp"
#include "AEEngine.h"
#include "utils.hpp"
#include "gunTypes.hpp"
#include "weapon.hpp"
#include "gameConstants.hpp"



Player InitPlayer(void) {
    Player player = {
        0,
        1000.0f,                  // health
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
		false,                     // inDash
        false,                    // isDead

		 0.0f,                    // velocityX
         0.0f,                    // velocityY
         0.0f,                    // prevX
         0.0f,                    // prevY
		nullptr,                   // particleMesh
		nullptr,                   // trailEmitter
		nullptr,                   // deathEmitter
		false                      // deathExplosionSpawned

     };

    return player;
}

void InitPlayerParticles(Player* player, void* mesh) {


    player->trailEmitter = ParticleEmitterCreate();
    if (!player->trailEmitter) {
        printf("ERROR: Failed to create particle emitter!\n");
        return;
    }
	//Init emitter properties 
    ParticleEmitterSetMesh(player->trailEmitter, (AEGfxVertexList*)mesh);
    ParticleEmitterSetLifeTime(player->trailEmitter, 0.7f, 1.0f);  // Longer life
    ParticleEmitterSetVelocity(player->trailEmitter, -30.0f, 30.0f, -30.0f, 30.0f); // tight spread
    ParticleEmitterSetScale(player->trailEmitter, 1.f, 5.f);  // More size variation
    ParticleEmitterSetColor(player->trailEmitter, 
        0.0f, 1.0f, 0.392f, 1.0f   // start color
    );


    player->deathEmitter = ParticleEmitterCreate();
    ParticleEmitterSetMesh(player->deathEmitter, (AEGfxVertexList*)mesh);
    ParticleEmitterSetLifeTime(player->deathEmitter, 0.4f, 0.8f);
	ParticleEmitterSetVelocity(player->deathEmitter, -300.0f, 300.0f, -300.0f, 300.0f); //AOE explosion, so wide spread
    ParticleEmitterSetScale(player->deathEmitter, 5.0f, 5.0f);
    ParticleEmitterSetColor(player->deathEmitter,
        0.0f, 1.0f, 0.392f, 1.0f     // start color
    );
    ParticleEmitterSetEmissionRate(player->deathEmitter, 0.0f);
    player->deathExplosionSpawned = false;
}
// math (on god im not sure whats happening here)
void NormalizeVector(float* x, float* y) {
    float length = sqrtf((*x) * (*x) + (*y) * (*y));
    if (length > 0.0f) {
        *x /= length;
        *y /= length;
    }
}


void DrawPlayer(Player& player, void* mesh, float cameraX, float cameraY, float dt) {
    float screenX = player.x - cameraX;
    float screenY = player.y - cameraY;    
    // Draw particles (they're in world space, need camera offset)
    if (player.trailEmitter && !player.isDead) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);

        // Temporarily offset particles for camera
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (player.trailEmitter->particles[i].active) {
                player.trailEmitter->particles[i].x -= cameraX;
                player.trailEmitter->particles[i].y -= cameraY;
            }
        }

        ParticleEmitterDraw(player.trailEmitter);

        // Restore world positions
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (player.trailEmitter->particles[i].active) {
                player.trailEmitter->particles[i].x += cameraX;
                player.trailEmitter->particles[i].y += cameraY;
            }
        }
    }

    // Draw death explosion
    if (player.isDead && player.deathEmitter) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (player.deathEmitter->particles[i].active) {
                player.deathEmitter->particles[i].x -= cameraX;
                player.deathEmitter->particles[i].y -= cameraY;
            }
        }
        ParticleEmitterDraw(player.deathEmitter);
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (player.deathEmitter->particles[i].active) {
                player.deathEmitter->particles[i].x += cameraX;
                player.deathEmitter->particles[i].y += cameraY;
            }
        }
        return; // skip drawing player body if ded
    }

    // Pulsing effect
    static float pulseTime = 0.0f;
    pulseTime += dt;
    float pulse = 1.0f + 0.15f * sinf(pulseTime * 5.0f);  // Size multiplier

    // Outer glow (neon green) - PULSING SIZE
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(0.3f);
    AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);  // Pure green
    AEGfxSetColorToAdd(0, 0, 0, 0);
    DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, PLAYER_RADIUS * 1.0f * pulse, PLAYER_RADIUS * 1.0f * pulse, 0.0f);

    // Inner core (bright neon green)
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(0.2f, 1.0f, 0.2f, 1.0f);  // Bright neon green
    DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, PLAYER_RADIUS * 0.7f, PLAYER_RADIUS * 0.7f, 0.0f);



}

void DrawPlayerUI(Player& player, WeaponStats& weapon, void* mesh, void* mesh2,
    float cameraX, float cameraY) {
    float uiX = -600.0f;
    float uiY = -380.0f;
    float barWidth = 300.0f;
    float barHeight = 30.0f;

    //health bar
    AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f);
    DrawSprite((AEGfxVertexList*)mesh, uiX, uiY, barWidth, barHeight, 0.0f);

    AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
    float currentHealthWidth = (player.health / 1000.0f) * barWidth;
    float healthOffset = (barWidth - currentHealthWidth) / 2.0f;
    DrawSprite((AEGfxVertexList*)mesh, uiX - healthOffset, uiY, currentHealthWidth, barHeight, 0.0f);

    //ammo Bar
    float ammoY = uiY - 40.0f;
    AEGfxSetColorToMultiply(0.2f, 0.2f, 0.2f, 1.0f);
    DrawSprite((AEGfxVertexList*)mesh, uiX, ammoY, barWidth, barHeight, 0.0f);

    SetWeaponColor(player.currentWeaponIndex);

    // Draw ammo bar with weapon color
    float currentAmmoWidth = ((float)*weapon.currentAmmo / (float)*weapon.maxAmmo) * barWidth;
    float ammoOffset = (barWidth - currentAmmoWidth) / 2.0f;
    DrawSprite((AEGfxVertexList*)mesh, uiX - ammoOffset, ammoY, currentAmmoWidth, barHeight, 0.0f);

    // Draw ammo count text
    char ammoText[32];
    float textOffsetX = 0.0f;

    // CHANGE: Use weapon.reloadTimer instead of reloadTime
    if (*weapon.reloadTimer > 0.0f) {
        sprintf_s(ammoText, 32, "Reloading...");
        textOffsetX = -30.0f;
    }
    else {
        sprintf_s(ammoText, 32, "%d / %d", *weapon.currentAmmo, *weapon.maxAmmo);
    }

    float textX, textY;
    GetNormalizedTextPosition(uiX - 30.0f + textOffsetX, ammoY - 8.0f, &textX, &textY);
    AEGfxPrint(fontID, ammoText, textX, textY, 1.5, 0.0, 0.0, 0.0, 1.0);

    // Draw weapon indicator circle 
    float circleX = uiX + barWidth / 2.0f + 30.0f;
    float circleSize = 25.0f;
    DrawSprite((AEGfxVertexList*)mesh2, circleX, ammoY, circleSize, circleSize, 0.0f);

    PlayerDashStaminaBar(player, meshes.square, cameraX, cameraY);
}

void PlayerDashStaminaBar(Player& player, void* mesh, float cameraX, float cameraY) {
    if (player.dashCooldownTimer > 0.0f) {
        float dashBarOffsetX = 50.0f;
        float dashBarOffsetY = 0.0f;

        float dashX = player.x - cameraX + dashBarOffsetX;
        float dashY = player.y - cameraY + dashBarOffsetY;

        float dashBarWidth = 10.0f;
        float dashBarHeight = 55.0f;

        AEGfxSetColorToMultiply(0.2f, 0.2f, 0.2f, 1.0f);
        DrawSprite((AEGfxVertexList*)mesh, dashX, dashY, dashBarWidth, dashBarHeight, 0.0f);

        // Calculate dash cooldown fill
        const float MAX_DASH_COOLDOWN = 0.5f;
        float cooldownPercent = 1.0f - (player.dashCooldownTimer / MAX_DASH_COOLDOWN);
        if (cooldownPercent < 0.0f) cooldownPercent = 0.0f;
        if (cooldownPercent > 1.0f) cooldownPercent = 1.0f;

        // Bar fills from bottom
        float currentDashHeight = cooldownPercent * dashBarHeight;
        float dashOffset = (dashBarHeight - currentDashHeight) / 2.0f;

		// Color shifting from red to cyan
        float cyan = cooldownPercent;
        float red = 1.0f - cooldownPercent;
        AEGfxSetColorToMultiply(red, cyan, cyan, 1.0f);
        DrawSprite((AEGfxVertexList*)mesh, dashX, dashY - dashOffset, dashBarWidth, currentDashHeight, 0.0f);
    }
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

// Overload for bullets without map collision (for enemies)
void UpdateBullets(std::vector<Bullet>& bullets, float dt) {
    // Backward loop to safely remove bullets
    for (int i = (int)bullets.size() - 1; i >= 0; i--) {
        // Update position
        bullets[i].x += bullets[i].velocityX * dt;
        bullets[i].y += bullets[i].velocityY * dt;

        // Check if off screen (no map collision)
        if (bullets[i].y > 500.0f || bullets[i].y < -500.0f ||
            bullets[i].x > 900.0f || bullets[i].x < -900.0f) {
            bullets.erase(bullets.begin() + i);
        }
    }
}

void RenderBullets(const std::vector<Bullet>& bullets, void* mesh,
    float cameraX, float cameraY) {  
    // Loop through each bullet

    for (const Bullet& bullet : bullets) {
        //  Convert world position to screen position
        float screenX = bullet.x - cameraX;
        float screenY = bullet.y - cameraY;

        // Outer glow - USE screenX, screenY
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 0.3f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS, BULLET_RADIUS, 0.0f);

        // Middle glow - USE screenX, screenY
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 0.6f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS - 10.0f, BULLET_RADIUS - 10.0f, 0.0f);

        // Core - USE screenX, screenY
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS -20.0f, BULLET_RADIUS -20.0f, 0.0f);
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
            player.pistol.weaponName,
            &player.pistol.currentAmmo,
            &player.pistol.maxMagazine,
            &player.pistol.maxAmmo,
            player.pistol.bulletSpeed,
            player.pistol.bulletDamage,
            player.pistol.fireRate,
            player.pistol.reloadSpeed,
            &player.pistol.reloadTimer,
            &player.pistol.inReload
        };
    }

    else if (player.currentWeaponIndex == 1) {
		// Assault Rifle
        return {
          player.rifle.weaponName, 
          &player.rifle.currentAmmo,
          &player.rifle.maxMagazine,
          &player.rifle.maxAmmo,
          player.rifle.bulletSpeed,
          player.rifle.bulletDamage,
          player.rifle.fireRate,
          player.rifle.reloadSpeed,
          &player.rifle.reloadTimer,
          &player.rifle.inReload
        };
    }
    else {
		// Default to pistol if index is invalid
        return {
            player.pistol.weaponName,
            &player.pistol.currentAmmo,
            &player.pistol.maxMagazine,
            &player.pistol.maxAmmo,
            player.pistol.bulletSpeed,
            player.pistol.bulletDamage,
            player.pistol.fireRate,
            player.pistol.reloadSpeed,
            &player.pistol.reloadTimer,
            &player.pistol.inReload
        };
    }
}


//Movement


void UpdatePlayer(Player* player, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {

    // --- DEATH ---
    if (player->isDead) {
        if (!player->deathExplosionSpawned) {
            ParticleEmitterEmit(player->deathEmitter, player->x, player->y, 64);
            player->deathExplosionSpawned = true;
        }
        ParticleEmitterUpdate(player->deathEmitter, dt);
        return; // skip everything else
    }

    // --- TIMERS ---
    if (player->dashTimer > 0.0f)        player->dashTimer -= dt;
    if (player->dashCooldownTimer > 0.0f) player->dashCooldownTimer -= dt;
    if (player->shootCooldown > 0.0f)    player->shootCooldown -= dt;

    // --- MOVEMENT ---
    float radius = 25.0f;
    float oldPrevX = player->x;
    float oldPrevY = player->y;

    // Dash input
    if (AEInputCheckTriggered(AEVK_SPACE) && player->dashCooldownTimer <= 0.0f) {
        float inputX = 0.0f, inputY = 0.0f;
        player->inDash = true;

        if (AEInputCheckCurr(AEVK_W)) inputY += 1.0f;
        if (AEInputCheckCurr(AEVK_S)) inputY -= 1.0f;
        if (AEInputCheckCurr(AEVK_D)) inputX += 1.0f;
        if (AEInputCheckCurr(AEVK_A)) inputX -= 1.0f;

        float length = sqrtf(inputX * inputX + inputY * inputY);
        if (length > 0.0f) {
            player->dashDirX = inputX / length;
            player->dashDirY = inputY / length;
            player->dashTimer = DASH_DURATION;
            player->dashCooldownTimer = DASH_COOLDOWN;
        }
    }

    // Apply dash or walk
    if (player->dashTimer > 0.0f) {
        float oldX = player->x, oldY = player->y;
        player->x += player->dashDirX * PLAYER_DASH_SPEED * dt;
        player->y += player->dashDirY * PLAYER_DASH_SPEED * dt;

        if (checkCollision(player->x, player->y, radius, gameMap)) {
            player->x = oldX;
            player->y = oldY;
            player->dashTimer = 0.0f;
            player->inDash = false;
        }
    }
    else {
        player->inDash = false;
        float moveX = 0.0f, moveY = 0.0f;

        if (AEInputCheckCurr(AEVK_W)) moveY += 1.0f;
        if (AEInputCheckCurr(AEVK_S)) moveY -= 1.0f;
        if (AEInputCheckCurr(AEVK_D)) moveX += 1.0f;
        if (AEInputCheckCurr(AEVK_A)) moveX -= 1.0f;

        if (moveX != 0.0f || moveY != 0.0f) {
            float length = sqrtf(moveX * moveX + moveY * moveY);
            moveX /= length;
            moveY /= length;

            float oldX = player->x, oldY = player->y;
            player->x += moveX * PLAYER_WALK_SPEED * dt;
            player->y += moveY * PLAYER_WALK_SPEED * dt;

            if (checkCollision(player->x, player->y, radius, gameMap)) {
                player->x = oldX;
                player->y = oldY;
            }
        }
    }

    // --- VELOCITY & TRAIL PARTICLES ---
    player->velocityX = (player->x - oldPrevX) / dt;
    player->velocityY = (player->y - oldPrevY) / dt;

    if (player->trailEmitter) {
        if (fabs(player->velocityX) > 0.1f || fabs(player->velocityY) > 0.1f)
            ParticleEmitterEmit(player->trailEmitter, player->x, player->y, 2);
        ParticleEmitterUpdate(player->trailEmitter, dt);
    }
}

void CheckEnemyBulletPlayerCollision(std::vector<Bullet>& enemyBullets,
    Player& player) {


    // Loop backwards so we can safely remove bullets
    for (int i = static_cast<int>( enemyBullets.size()) - 1; i >= 0; i--) {
        // Check if bullet hit player
        int collision = AreCirclesIntersecting(
            enemyBullets[i].x,        // bullet X
            enemyBullets[i].y,        // bullet Y
            BULLET_RADIUS,            // bullet radius
            player.x,                 // player X
            player.y,                 // player Y
            PLAYER_RADIUS/2             // player radius
        );

        if (collision > 0 && (!player.isDead)) {
            // Player got hit!
            player.health -= enemyBullets[i].damage;

            printf("Player hit by bullet! Health: %.0f\n", player.health);

            // Check if player died
            if (player.health <= 0.0f) {
				player.health = 0.0f;
				player.isDead = true;
                printf("PLAYER DIED!\n");
                // Handle death here
            }

            // Remove the bullet (it hit the player)
            enemyBullets.erase(enemyBullets.begin() + i);

            // Only take damage from ONE bullet per hit
            break;
        }
    }
}