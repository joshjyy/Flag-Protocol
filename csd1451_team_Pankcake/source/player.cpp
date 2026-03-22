#include "player.hpp"
#include "AEEngine.h"
#include "utils.hpp"
#include "gunTypes.hpp"
#include "weapon.hpp"
#include "gameConstants.hpp"
#include "enemies.hpp"
#include "sound.hpp"
#include <fstream>



Player InitPlayer(void) {
    Player player = {
		0,                          //gruntsKilled
		0,                        // score
		0,						// flagCaptures
		0,						// scoreMilestone
		0.0f,					// milestoneTimer
        100.0f,                  // health
        0.0f,                     // x
        0.0f,                     // y
        0.0f,                     // dashTimer
        0.0f,                     // dashCooldownTimer
        0.0f,                     // dashDirX
        0.0f,                     // dashDirY
        false,                    //canShoot
        Pistol(),                 // pistol constructor
        AssaultRifle(),           // rifle constructor
		Shotgun(),               // shotgun constructor
        0,                        // currentWeaponIndex (start with pistol)
        0.0f,                    // shootCooldown
		false,                     // inDash
        false,                    // isDead
        false,                      //holdingFlag
		false,                      // wasSprinting
		false,                      // lowHealthWarningPlayed

		 0.0f,                    // velocityX
         0.0f,                    // velocityY
         0.0f,                    // prevX
         0.0f,                    // prevY
		nullptr,                   // particleMesh
		nullptr,                   // trailEmitter
		nullptr,                   // deathEmitter
		nullptr,                   // dashTrailEmitter
		false                // deathExplosionSpawned

        

     };

    return player;
}

void InitPlayerParticles(Player* player, void* mesh, void*mesh2) {

	// Trail emitter setup
    player->trailEmitter = ParticleEmitterCreate();
	//Init emitter properties 
    ParticleEmitterSetMesh(player->trailEmitter, (AEGfxVertexList*)mesh);
    ParticleEmitterSetLifeTime(player->trailEmitter, 0.7f, 1.0f);  // Longer life
    ParticleEmitterSetVelocity(player->trailEmitter, -30.0f, 30.0f, -40.0f, 40.0f); // tight spread
    ParticleEmitterSetScale(player->trailEmitter, 1.f, 5.f);  // More size variation
    ParticleEmitterSetColor(player->trailEmitter, 
        0.0f, 1.0f, 0.392f, 1.0f   // start color
    );

	// Death explosion emitter setup
    player->deathEmitter = ParticleEmitterCreate();
    ParticleEmitterSetMesh(player->deathEmitter, (AEGfxVertexList*)mesh);
    ParticleEmitterSetLifeTime(player->deathEmitter, 0.4f, 0.8f);
	ParticleEmitterSetVelocity(player->deathEmitter, -300.0f, 300.0f, -300.0f, 300.0f); //AOE explosion, so wide spread
    ParticleEmitterSetScale(player->deathEmitter, 5.0f, 5.0f);
    ParticleEmitterSetColor(player->deathEmitter,
        0.0f, 1.0f, 0.392f, 1.0f     // start color
    );

    ParticleEmitterSetEmissionRate(player->deathEmitter, 0.0f);
    // Dash trail emitter setup
    player->dashTrailEmitter = ParticleEmitterCreate();
    ParticleEmitterSetMesh(player->dashTrailEmitter, (AEGfxVertexList*)mesh2);
    ParticleEmitterSetLifeTime(player->dashTrailEmitter, 0.1f, 0.25f);
    ParticleEmitterSetVelocity(player->dashTrailEmitter, -10.f, 10.f, -10.f, 10.f);
    ParticleEmitterSetScale(player->dashTrailEmitter, PLAYER_RADIUS * 0.7f, PLAYER_RADIUS * 0.7f);
    ParticleEmitterSetColor(player->dashTrailEmitter, 0.0f, 1.0f, 0.0f, 0.4f);
    ParticleEmitterSetEmissionRate(player->dashTrailEmitter, 30.0f);



}



void DrawPlayer(Player& player, void* mesh, CameraPos &camera, float dt)
{

	// Lamda  to offset particles for camera movement 
    //returns particles to world space for correct position, then back to screen space after drawing
	//note - [&] captures all local variables by reference, allowing access to cameraX and cameraY
    auto offsetParticles = [&](ParticleEmitter* emitter, float dx, float dy)
        {
			// Loop through all particles and offset their positions based on camera
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                
                auto& p = emitter->particles[i];
                if (p.active) { 
					// Offset position by adding dx and dy which is camera
                    p.x += dx;
                    p.y += dy;
                }
            }
        };
	// Helper to draw a single emitter based on camera screen space, handles null checks and offsetting
    auto drawEmitter = [&](ParticleEmitter* emitter)
        {
			// If emitter is null, just skip drawing
            if (!emitter) return;
			// offset particles to screen space for correct rendering, then offset back
            offsetParticles(emitter, -camera.x, -camera.y);
            ParticleEmitterDraw(emitter);
            offsetParticles(emitter, camera.x, camera.y);
        };

	// Set render mode to color for player drawing
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    float r, g, b;
    GetWeaponColor(player.currentWeaponIndex, r, g, b);
    player.trailEmitter->startR = r;
    player.trailEmitter->startG = g;
    player.trailEmitter->startB = b;

    player.dashTrailEmitter->startR = r;
    player.dashTrailEmitter->startG = g;
    player.dashTrailEmitter->startB = b;

	player.deathEmitter->startR = r;
	player.deathEmitter->startG = g;
	player.deathEmitter->startB = b;
    // --- Death state: draw explosion and skip player body ---
    if (player.isDead)
    {
        drawEmitter(player.deathEmitter);
        return;
    }

    float screenX = player.x - camera.x;
    float screenY = player.y - camera.y;



    // --- Trail & dash particles ---
    drawEmitter(player.trailEmitter);
    drawEmitter(player.dashTrailEmitter);

    // Dash overlay on player body
    if (player.inDash)
    {
        AEGfxSetColorToMultiply(0.0f, 1.0f, 1.0f, 0.7f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY,
            PLAYER_RADIUS * 1.2f, PLAYER_RADIUS * 1.2f, 0.0f);
    }

    // --- Player body ---


    // Halo effect (health/flag indicator)
    static float haloTime = 0.0f;
    haloTime += dt;
    float halo = 1.0f + 0.5f * sinf(haloTime * 4.0f);

    AEGfxSetTransparency(0.4f);

    // Determine color: Flag > Low Health > Full Health
    if (player.holdingFlag) {
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f); // Gold
    }
    else if (player.health < 40.0f) {
        AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f); // Red
    }
    else {
        AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f); // Green
    }

    DrawSprite((AEGfxVertexList*)mesh, screenX, screenY,
        PLAYER_RADIUS * 1.1f * halo, PLAYER_RADIUS * 1.1f * halo, 0.0f);

    // Inner core - weapon color

    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(r, g, b, 1.0f);
    DrawSprite((AEGfxVertexList*)mesh, screenX, screenY,
        PLAYER_RADIUS * 0.7f, PLAYER_RADIUS * 0.7f, 0.0f);



}

void DrawPlayerUI(Player& player, WeaponStats& weapon, void* mesh, void* mesh2,
    CameraPos camera) {
    float uiX = -600.0f;
    float uiY = -380.0f;
    float barWidth = 300.0f;
    float barHeight = 30.0f;

    //health bar
    AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f);
    DrawSprite((AEGfxVertexList*)mesh, uiX, uiY, barWidth, barHeight, 0.0f);

    AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
    float currentHealthWidth = (player.health / 100.0f) * barWidth;
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
    PlayerDashStaminaBar(player, meshes.square, camera);
}

void PlayerDashStaminaBar(Player& player, void* mesh, CameraPos& camera) {
    if (player.dashCooldownTimer > 0.0f && player.isDead == false) {
        float dashBarOffsetX = 50.0f;
        float dashBarOffsetY = 0.0f;

        float dashX = player.x - camera.x + dashBarOffsetX;
        float dashY = player.y - camera.y + dashBarOffsetY;

        float dashBarWidth = 10.0f;
        float dashBarHeight = 55.0f;

        AEGfxSetColorToMultiply(0.2f, 0.2f, 0.2f, 1.0f);
        DrawSprite((AEGfxVertexList*)mesh, dashX, dashY, dashBarWidth, dashBarHeight, 0.0f);

        // Calculate dash cooldown fill
        const float MAX_DASH_COOLDOWN = 0.5f;
        float cooldownPercent = 1.0f - (player.dashCooldownTimer / MAX_DASH_COOLDOWN);
		// Clamp between 0 and 1 just in case
        if (cooldownPercent < 0.0f) cooldownPercent = 0.0f;
        if (cooldownPercent > 1.0f) cooldownPercent = 1.0f;

        // Bar fills from bottom
        float currentDashHeight = cooldownPercent * dashBarHeight;
        float dashOffset = (dashBarHeight - currentDashHeight) / 2.0f;

		// Color shifting from red to cyan
        float cyan = cooldownPercent;
		// Invert red so it goes from 1 to 0 as cooldown fills
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
    std::vector<std::vector<int>>& gameMap) {


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
    CameraPos camera) {  
    // Loop through each bullet

    for (const Bullet& bullet : bullets) {
        //  Convert world position to screen position
		// minus camera position to ensure bullets are drawn in the correct place relative to the player
        float screenX = bullet.x - camera.x;
        float screenY = bullet.y - camera.y;

        // Outer glow 
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 0.3f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS, BULLET_RADIUS, 0.0f);

        // Middle glow
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 0.6f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS - 10.0f, BULLET_RADIUS - 10.0f, 0.0f);

        // Core
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS -20.0f, BULLET_RADIUS -20.0f, 0.0f);
    }
}

void RenderBullets(Player& player, const std::vector<Bullet>& bullets, void* mesh,
    CameraPos camera) {
    // Loop through each bullet

    for (const Bullet& bullet : bullets) {
        //  Convert world position to screen position
        // minus camera position to ensure bullets are drawn in the correct place relative to the player
        float screenX = bullet.x - camera.x;
        float screenY = bullet.y - camera.y;

        float r, g, b;
        GetWeaponColor(player.currentWeaponIndex, r, g, b);

        // Outer glow
        AEGfxSetColorToMultiply(r, g, b, 0.3f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS, BULLET_RADIUS, 0.0f);
        // Middle glow
        AEGfxSetColorToMultiply(r, g, b, 0.6f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS - 10.0f, BULLET_RADIUS - 10.0f, 0.0f);
        // Core
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        DrawSprite((AEGfxVertexList*)mesh, screenX, screenY, BULLET_RADIUS - 20.0f, BULLET_RADIUS - 20.0f, 0.0f);
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

    else if (player.currentWeaponIndex == 2) {
        return{
        player.shotgun.weaponName,
        &player.shotgun.currentAmmo,
        &player.shotgun.maxMagazine,
        &player.shotgun.maxAmmo,
        player.shotgun.bulletSpeed,
        player.shotgun.bulletDamage,
        player.shotgun.fireRate,
        player.shotgun.reloadSpeed,
        &player.shotgun.reloadTimer,
        &player.shotgun.inReload,
        };
    } else {
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



void UpdatePlayer(Player* player, float dt, std::vector<std::vector<int>>& gameMap) {
   
	if (player->health > 0.0f)
	player->health -= dt * 2.0f;  // Passive health drain over time
    if(player->health <= 0.0f && !player->isDead) {
        player->isDead = true;

	}

	// If player is dead, only update death explosion and skip all movement/input
    if (player->isDead) {
        if (!player->deathExplosionSpawned) {
			// Spawn a burst of particles at the player's position for the death explosion
            ParticleEmitterEmit(player->deathEmitter, player->x, player->y, 64);
            AEAudioPlay(gDeathSound, gSFXGroup, 1.0f, 1.0f, 0);  

            player->deathExplosionSpawned = true;
        }
        ParticleEmitterUpdate(player->deathEmitter, dt);
        return;
    }

    // --- TIMERS ---
	//decrease dash timer if currently dashing, and cooldown timer if on cooldown. 
    if (player->dashTimer > 0.0f)
        player->dashTimer -= dt;

    if (player->dashCooldownTimer > 0.0f)  
        player->dashCooldownTimer -= dt;

    if (player->shootCooldown > 0.0f)     
        player->shootCooldown -= dt;

    // --- MOVEMENT ---
    float radius = 25.0f;
	// Store previous position for velocity calculation and collision 
    float oldPrevX = player->x;
    float oldPrevY = player->y;

    // Get WASD input
    float inputX = 0.0f, inputY = 0.0f;
    if (AEInputCheckCurr(AEVK_W)) inputY += 1.0f;
    if (AEInputCheckCurr(AEVK_S)) inputY -= 1.0f;
    if (AEInputCheckCurr(AEVK_D)) inputX += 1.0f;
    if (AEInputCheckCurr(AEVK_A)) inputX -= 1.0f;

    // Dash input
	// Only trigger if space is pressed, dash is not on cooldown, and there is some movement input (can't dash in place)
    if (AEInputCheckTriggered(AEVK_SPACE) && player->dashCooldownTimer <= 0.0f && (inputX != 0.0f || inputY != 0.0f)) {
		printf("Dash!\n");
        AEAudioPlay(gDashSound, gSFXGroup, 0.7f, 1.0f, 0); 

		// Start dash in input direction (
        player->dashDirX = inputX;
        player->dashDirY = inputY;
        
		//Get direction of input 
        NormalizeVector(&player->dashDirX, &player->dashDirY);

		// Set dash timers
        player->dashTimer = DASH_DURATION;
        player->dashCooldownTimer = DASH_COOLDOWN;
        player->inDash = true;
        if(player->health < 10.0f){
            player->health = 0.0f;
		}
        else player->health -= 10.0f;  // Dash costs health
    }

	//Store old position before moving for collision checks
    float oldX = player->x, oldY = player->y;

    if (player->dashTimer > 0.0f) {
        // Dashing
        player->x += player->dashDirX * PLAYER_DASH_SPEED * dt;
        player->y += player->dashDirY * PLAYER_DASH_SPEED * dt;
    }
    else {
        // --- SPRINT SOUND ---
        //bool isSprinting = AEInputCheckCurr(AEVK_RBUTTON) && (inputX != 0.0f || inputY != 0.0f);
        //if (isSprinting && !player->wasSprinting)
        //    AEAudioPlay(gSprintSound, gSprintGroup, 0.3f, 1.0f, 1);  // loop
        //if (!isSprinting && player->wasSprinting)
        //    AEAudioStopGroup(gSprintGroup);
        //player->wasSprinting = isSprinting;



        // Sprint cap when holding shift, otherwise walk speed
        float currentMaxSpeed = AEInputCheckCurr(AEVK_RBUTTON) ? PLAYER_WALK_SPEED * 2.0f : PLAYER_WALK_SPEED;

        // Apply acceleration in input direction only when sprinting
        if (inputX != 0.0f || inputY != 0.0f) {
			//Get direction of input and apply acceleration if sprinting
            NormalizeVector(&inputX, &inputY);
            float currentAccel = AEInputCheckCurr(AEVK_RBUTTON) ? ACCELERATION : 0.0f;
            player->velocityX += inputX * currentAccel * dt;
            player->velocityY += inputY * currentAccel * dt;

            // Normal walking movement when not sprinting
            if (!AEInputCheckCurr(AEVK_RBUTTON)) {
                player->velocityX = inputX * PLAYER_WALK_SPEED;
                player->velocityY = inputY * PLAYER_WALK_SPEED;
            }
        }

		// Apply friction to gradually slow down when no input or releasing left mouse
        player->velocityX -= player->velocityX * FRICTION * dt;
        player->velocityY -= player->velocityY * FRICTION * dt;

        // Clamp velocity to current max speed
        float speed = sqrtf(player->velocityX * player->velocityX + player->velocityY * player->velocityY);
        if (speed > currentMaxSpeed) {
            player->velocityX = (player->velocityX / speed) * currentMaxSpeed;
            player->velocityY = (player->velocityY / speed) * currentMaxSpeed;
        }

        // Move player based on current velocity
        player->x += player->velocityX * dt;
        player->y += player->velocityY * dt;
    }

    int collision = checkCollision(*player, radius, gameMap);
    if (collision == 1) {
        player->x = oldX;
        player->y = oldY;
        player->dashTimer = 0.0f;
        player->velocityX = 0.0f;
        player->velocityY = 0.0f;
    }
    else if (collision == 4 && player->holdingFlag == false) {
        player->holdingFlag = true;
		AEAudioPlay(gPickUpSound, gSFXGroup, 1.0f, 1.0f, 0);
        if (player->health < 80.0f) {
            player->health += 20.0f; // Heal player for picking up
        }
        else {
			player->health = 100.0f; // Cap health at 100
        }

    }
    else if (collision == 2 && player->holdingFlag == true) {
        player->holdingFlag = false;
        player->score += 100;
		player->flagCaptures += 1;
        Spawn_Flag(gameMap);
        AEAudioPlay(gDropSound, gSFXGroup, 1.0f, 1.0f, 0);

        // Milestone animations
        if (player->score == 300)
            player->scoreMilestone = 1;  
        else if (player->score == 500)
            player->scoreMilestone = 2; 
        else if (player->score == 1000)
            player->scoreMilestone = 3;  

        player->milestoneTimer = 2.0f;  // show for 2 seconds
    }



	// Update dash state (in case dash just ended)
    player->inDash = (player->dashTimer > 0.0f);

    // --- TRAIL PARTICLES ---
    // Calculate velocity based on how much player moved this frame
    player->velocityX = (player->x - oldPrevX) / dt;
    player->velocityY = (player->y - oldPrevY) / dt;


    if (player->trailEmitter) {
		// Emit particles only if player is moving 
        bool isMoving = (player->velocityX > 0.1f || player->velocityX < -0.1f ||
            player->velocityY > 0.1f || player->velocityY < -0.1f);
        if (isMoving)
			// Emit particles at player's position with a count of 2 for a subtle trail effect
            ParticleEmitterEmit(player->trailEmitter, player->x, player->y, 1);
            ParticleEmitterUpdate(player->trailEmitter, dt);
    }

    if (player->inDash) {
        ParticleEmitterEmit(player->dashTrailEmitter, player->x, player->y, 3);
    }
    ParticleEmitterUpdate(player->dashTrailEmitter, dt);


    if (player->health < 40.0f && !player->lowHealthWarningPlayed) {
        AEAudioPlay(gLowHealthBeepSound, gSFXGroup, 0.6f, 1.0f, 0);
        player->lowHealthWarningPlayed = true;
    }

    // Reset if health goes back up 
    if (player->health >= 90.0f) {
        player->lowHealthWarningPlayed = false;
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
            player.health -= enemyBullets[i].damage;



            // Check if player died
            if (player.health <= 0.0f) {
				player.health = 0.0f;
				player.isDead = true;
                //printf("PLAYER DIED!\n");
                // Handle death here
            }

            // Remove the bullet (it hit the player)
            enemyBullets.erase(enemyBullets.begin() + i);

            // Only take damage from ONE bullet per hit
            break;
        }
    }
}

void DrawCrosshair(void* mesh) {
    // Hide the default Windows cursor
    

    // Get mouse position 
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

	// Recenter origin to middle of screen and flip Y for correct orientation
    float cx = (float)mouseX - 800.0f;   // half of 1600
    float cy = -(float)mouseY + 450.0f;  // half of 900, flipped Y



    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f); // green
    AEGfxSetColorToAdd(0, 0, 0, 0);
    float armLength = 15.0f;
    float thickness = 3.0f;
    // Horizontal bar
    DrawSprite((AEGfxVertexList*)mesh, cx, cy, armLength, thickness, 0.0f);
    // Vertical bar
    DrawSprite((AEGfxVertexList*)mesh, cx, cy, thickness, armLength, 0.0f);
}


void CheckPlayerGruntBodyCollision(Player& player, std::vector<Grunt>& grunts) {
    for (auto& grunt : grunts) {
        if (grunt.isDead) continue;

		// Simple circle collision check between player and grunt and push player out if colliding
        float dx = player.x - grunt.movingX;
        float dy = player.y - grunt.movingY;
		// Distance between centers (pyth)
        float dist = sqrtf(dx * dx + dy * dy);
		// Minimum distance to avoid collision 
        float minDist =  GRUNT_RADIUS; 

		//if distance is less than minimum and greater than 0 , then collide and  to push the player out
        if (dist < minDist && dist > 0.0f) {
			//printf("Collision with grunt! Pushing player out.\n");
            // Push player out
			float overlap = minDist - dist; // how much circles are intersecting 

			float nx = dx / dist; // normalize djrection vector to get push direction
            float ny = dy / dist;

			//multiply by overlap to get push amount, and add to player position to move them out of collision
			player.x += nx * overlap; //push player out of collision
            player.y += ny * overlap;
        }

        if (player.inDash == 1) {
            if (dist < minDist + 20.0f) { // slightly larger radius for dash hit
                //printf("Dashed into grunt!\n");
                grunt.health -= 50.0f; // dash damage
                if (grunt.health <= 0.0f) {
                    if(player.health < 80.0f) {
                        player.health += 20.0f; // heal player for 20 health on kill
                    }
					else
					player.health = 100.0f; // heal player for 20 health on kill
                    

                    player.pistol.currentAmmo += 5;
                    if (player.pistol.currentAmmo > player.pistol.maxAmmo)
                        player.pistol.currentAmmo = player.pistol.maxAmmo;

                    player.rifle.currentAmmo += 10;
                    if (player.rifle.currentAmmo > player.rifle.maxAmmo)
                        player.rifle.currentAmmo = player.rifle.maxAmmo;
                    printf("After: pistol=%d\n", player.pistol.currentAmmo);

                    player.score += 10;
                    grunt.isDead = true;
                    player.gruntsKilled++;
                    //printf("Grunt killed by dash! Total kills: %d\n", player.gruntsKilled);
                }
			}
        }
    }
}

void killPlayer(Player& player) {
    if (AEInputCheckTriggered(AEVK_K)) {
        if (!player.isDead) {
            player.isDead = true; 
            
        }
        else {
            // revive only after explosion finishes
            bool explosionDone = true;
            for (int i = 0; i < MAX_PARTICLES; i++) {
                if (player.deathEmitter->particles[i].active) {
                    explosionDone = false;
                    break;
                }
            }

            if (explosionDone) {
                player.isDead = false;
                player.deathExplosionSpawned = false;
				player.health = 100.0f;
				player.dashCooldownTimer = 0.0f;
				player.dashTimer = 0.0f;

            }
        }
    }
}

void LoadPlayer(Player& player) {
    // Initialize player stats
    player.gruntsKilled = 0;
	player.flagCaptures = 0;
    player.score = 0;
    player.health = 100.0f;
    player.x = 0.0f;
    player.y = 0.0f;
    player.velocityX = 0.0f;
    player.velocityY = 0.0f;
    player.prevX = 0.0f;
    player.prevY = 0.0f;
	player.canshoot = false;

    // Dash system
    player.inDash = false;
    player.dashTimer = 0.0f;
    player.dashCooldownTimer = 0.0f;
    player.dashDirX = 0.0f;
    player.dashDirY = 0.0f;

    // Weapon system
    player.currentWeaponIndex = 0;
    player.shootCooldown = 0.0f;
    player.pistol.currentAmmo = player.pistol.maxAmmo;
    player.rifle.currentAmmo = player.rifle.maxAmmo;
    player.shotgun.currentAmmo = player.shotgun.maxAmmo;


    // State flags
    player.isDead = false;
    player.holdingFlag = false;
    player.deathExplosionSpawned = false;
    player.canshoot = false;

}

void FreePlayer(Player& player) {
    ParticleEmitterFree(&player.trailEmitter);
    ParticleEmitterFree(&player.dashTrailEmitter);
    ParticleEmitterFree(&player.deathEmitter);
}

void DrawScoreUI(Player& player, void* mesh , float dt) {
	//Set up UI render state
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(0.6f);
    AEGfxSetColorToMultiply(0.0f, 0.05f, 0.0f, 1.0f);
    AEMtx33 scale, translate, transform;

    //
    AEMtx33Scale(&scale, 220.0f, 140.0f);
    AEMtx33Trans(&translate, 690.0f, 380.0f);
    AEMtx33Concat(&transform, &translate, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw((AEGfxVertexList*)mesh, AE_GFX_MDM_TRIANGLES);

    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(0.0f, 1.0f, 0.2f, 1.0f);
    AEMtx33Scale(&scale, 3.0f, 140.0f);
    AEMtx33Trans(&translate, 582.0f, 380.0f);
    AEMtx33Concat(&transform, &translate, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw((AEGfxVertexList*)mesh, AE_GFX_MDM_TRIANGLES);

    float textX, textY;

    GetNormalizedTextPosition(600.0f, 430.0f, &textX, &textY);
    AEGfxPrint(fontID, "// SCORE //", textX, textY, 1.0f, 0.0f, 1.0f, 0.2f, 1.0f);

    GetNormalizedTextPosition(600.0f, 400.0f, &textX, &textY);
    AEGfxPrint(fontID, "----------------", textX, textY, 0.8f, 0.0f, 0.4f, 0.1f, 1.0f);

    char scoreText[32];
    sprintf_s(scoreText, 32, "PTS  %d", player.score);
    GetNormalizedTextPosition(600.0f, 370.0f, &textX, &textY);
    AEGfxPrint(fontID, scoreText, textX, textY, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f);

    char flagText[32];
    sprintf_s(flagText, 32, "FLAGS  %d", player.flagCaptures);
    GetNormalizedTextPosition(600.0f, 340.0f, &textX, &textY);
    AEGfxPrint(fontID, flagText, textX, textY, 1.0f, 0.0f, 1.0f, 0.8f, 1.0f);

    int captures = player.flagCaptures;
    for (int i = 0; i < 10; i++) {
        float dotX = 610.0f + i * 16.0f;
        AEGfxSetTransparency(1.0f);
        if (i < captures)
            AEGfxSetColorToMultiply(0.0f, 1.0f, 0.8f, 1.0f);
        else
            AEGfxSetColorToMultiply(0.1f, 0.2f, 0.1f, 1.0f);

        AEMtx33Scale(&scale, 10.0f, 10.0f);
        AEMtx33Trans(&translate, dotX, 310.0f);
        AEMtx33Concat(&transform, &translate, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw((AEGfxVertexList*)mesh, AE_GFX_MDM_TRIANGLES);
    }


    // Milestone animation
    if (player.milestoneTimer > 0.0f) {
		//Decrease timer
        player.milestoneTimer -= dt;

        // Pulse scale based on timer
        float pulse = 1.5f + 0.5f * sinf(player.milestoneTimer * 10.0f);
        // Fade out near end
        float alpha = player.milestoneTimer / 2.0f;

        const char* milestoneText[] = { "", "NODES SEIZED", "SIGNAL BREACH", "GRID DOMINANT" };        
        float colors[4][3] = {
            {1,1,1},
            {0.0f, 1.0f, 0.4f},   // green
            {1.0f, 0.5f, 0.0f},   // orange
            {1.0f, 0.8f, 0.0f},   // gold
        };

        GetNormalizedTextPosition(580.0f, 260.0f, &textX, &textY);
        AEGfxPrint(fontID, milestoneText[player.scoreMilestone],
            textX, textY, pulse,
            colors[player.scoreMilestone][0],
            colors[player.scoreMilestone][1],
            colors[player.scoreMilestone][2],
            alpha);

        // Burst particles at player position
        if (player.trailEmitter)
            ParticleEmitterEmit(player.trailEmitter, player.x, player.y, 10);
           }
    AEGfxSetTransparency(1.0f);
}
void SavePlayerScore(Player& player)
{
    std::ofstream file("Score/score.txt");
    if (file.is_open())
    {
        file << "Previous Score: " << player.score << std::endl;
		file << "Previous Viruses Killed: " << player.gruntsKilled << std::endl;
        file.close();
    }
}

bool CanShoot(Player& player){
    if (!AEInputCheckCurr(AEVK_LBUTTON)) {
        player.canshoot = true;
    }
    return player.canshoot;
}
