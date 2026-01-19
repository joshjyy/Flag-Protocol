
#include "pch.hpp"




void HandleWeaponSwitching(Player* player) {
    // Using pointer because we need to CHANGE the player's weapon

    if (AEInputCheckTriggered(AEVK_1)) {
        player->currentWeaponIndex = 0;  // Switch to Pistol
        printf("Pistol Switched! %d\n", player->currentWeaponIndex);
    }

    if (AEInputCheckTriggered(AEVK_2)) {
        player->currentWeaponIndex = 1;  // Switch to Assault Rifle
        printf("Rifle Switched! %d\n", player->currentWeaponIndex);
    }
}

void UpdateShootCooldown(Player* player, float dt) {
    // Only count down if cooldown is active (greater than 0)
    if (player->shootCooldown > 0.0f) {
        player->shootCooldown -= dt;  // Subtract frame time
    }
}

void HandleShooting(Player* player, WeaponStats weapon, std::vector<Bullet>* playerBullets) {
    if (AEInputCheckCurr(AEVK_LBUTTON) && player->shootCooldown <= 0.0f) {

        // Check: Do we have ammo?
        if (*weapon.currentAmmo > 0) {

            s32 mouseX, mouseY;
            AEInputGetCursorPosition(&mouseX, &mouseY);

            // Step 2: Convert screen coords to world coords
            // Why: Your game world uses different coordinates than the screen
            float worldMouseX = (float)mouseX - 800.0f;
            float worldMouseY = 450.0f - (float)mouseY;

            // Step 3: Create the bullet
            Bullet newBullet = CreateBullet(
                player->x, player->y,           
                worldMouseX, worldMouseY,       
                weapon.bulletSpeed,            
                weapon.bulletDamage             
            );

            // Step 4: Add bullet to the active bullets list
            playerBullets->push_back(newBullet);

            // Step 5: Use up one bullet
            (*weapon.currentAmmo)--;

            // Step 6: Show remaining ammo (for debugging)
            printf("Ammo: %d/%d\n", *weapon.currentAmmo, *weapon.maxAmmo);

            // Step 7: Start cooldown timer
            // Why: Prevents shooting too fast - each weapon has different fire rate
            player->shootCooldown = weapon.fireRate;
        }
    }
}


void UpdateReloadTimer(WeaponStats weapon, float* reloadTime, float dt) {

    // Is reload in progress?
    if (*reloadTime > 0.f) {
        *reloadTime -= dt;  // Count down

        // Did reload just finish?
        if (*reloadTime <= 0.f) {
            // Refill ammo to max
            *weapon.currentAmmo = *weapon.maxAmmo;
            printf("Reloaded!\n");
        }
    }
}


void TryStartReload(WeaponStats weapon, float* reloadTime) {
    // Three conditions must ALL be true:
    // 1. Player pressed R
    // 2. Not currently reloading
    // 3. Magazine isn't already full

    if (AEInputCheckTriggered(AEVK_R) &&
        *reloadTime <= 0.f &&
        *weapon.currentAmmo < *weapon.maxAmmo) {

        // Start reload timer
        *reloadTime = weapon.reloadSpeed;
        printf("Reloading...\n");  
    }
}

