#include "player.hpp"
#include "AEEngine.h"
#include "utils.hpp"
#include "gunTypes.hpp"
#include <cmath>


// math (on god im not sure whats happening here)
void NormalizeVector(float* x, float* y) {
    float length = sqrtf((*x) * (*x) + (*y) * (*y));
    if (length > 0.0f) {
        *x /= length;
        *y /= length;
    }
}
//Create a new bullet and allocate memory
//Parameters: starting position (x, y), target position (targetX, targetY), speed
Bullet* CreateBullet(float x, float y, float targetX, float targetY, float speed, float damage) {
    Bullet* newBullet = new Bullet;
    newBullet->x = x;
    newBullet->y = y;
    newBullet->damage = damage;

    // Calculate direction vector
    float dirX = targetX - x;
    float dirY = targetY - y;

    // Normalize the direction
    NormalizeVector(&dirX, &dirY);


    // Set velocity of bullet
    newBullet->velocityX = dirX * speed;
    newBullet->velocityY = dirY * speed;


    newBullet->active = true;
	//Initialize next pointer to nullptr so it doesnt point to garbage value
    newBullet->next = nullptr;

    return newBullet;
}

// Update all bullets and remove off-screen ones
void UpdateBullets(Bullet** bulletHead, float dt) {
    Bullet* current = *bulletHead;

	// Create a previous variable to store the previous bullet in the linked list
    Bullet* previous = nullptr;

    while (current != nullptr) {
        // Update bullet position
        current->x += current->velocityX * dt;
        current->y += current->velocityY * dt;

        // Check if bullet is off screen 
        if (current->y > 500.0f || current->y < -500.0f ||
            current->x > 900.0f || current->x < -900.0f) {

			// Prepare to remove bullet
            Bullet* toDelete = current;

			// Update linked list pointers
            if (previous == nullptr) {
				// Move bulletHead to next bullet
                *bulletHead = current->next;
                current = *bulletHead;
            }
            else {
				// Removing middle/end of list in case bullet collidies with something before going off screen
                previous->next = current->next;
                current = current->next;
            }

            // Free memory for bullet pointed to by this address
            delete toDelete;  
        }








        else {
            // Move to next bullet
            previous = current;
            current = current->next;
        }
    }
}

// Render all bullets
void RenderBullets(Bullet* bulletHead, void* mesh) {
    // Set bullet color (Yellow)
    AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f);

    Bullet* current = bulletHead;

    //Draw bullet if current bullet is active
    while (current != nullptr) {
        // Outer glow (bigger, more transparent)
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 0.3f);  // Yellow, 30% opacity
        DrawSprite((AEGfxVertexList*)mesh, current->x, current->y, 40.0f, 40.0f, 0.0f);

        // Middle glow
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 0.6f);  // Yellow, 60% opacity
        DrawSprite((AEGfxVertexList*)mesh, current->x, current->y, 30.0f, 30.0f, 0.0f);

        // Core (solid)
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);  // White center
        DrawSprite((AEGfxVertexList*)mesh, current->x, current->y, 20.0f, 20.0f, 0.0f);

        current = current->next;
    }
}

// Free all bullets in the list and clear the list
void FreeBullets(Bullet** bulletHead) {
    while (*bulletHead != nullptr) {
        Bullet* toDelete = *bulletHead;
        *bulletHead = (*bulletHead)->next;
        delete toDelete;
    }
}

// Count how many bullets exist (for debugging)
int GetBulletCount(Bullet* bulletHead) {
    int count = 0;
    Bullet* current = bulletHead;
    while (current != nullptr) {
        count++;
        current = current->next;
    }
    return count;
}
//using references because weapon ammo are stored in player struct
WeaponStats GetCurrentWeapon(Player& player) {
    if (player.currentWeaponIndex == 0) {
        // Pistol
        return {
            &player.pistol.currentAmmo,
            &player.pistol.maxAmmo,
            player.pistol.bulletSpeed,
            player.pistol.bulletDamage,
            player.pistol.fireRate
        };
    }

    else if (player.currentWeaponIndex == 1) {
		// Assault Rifle
        return {
          &player.rifle.currentAmmo,
          &player.rifle.maxAmmo,
          player.rifle.bulletSpeed,
          player.rifle.bulletDamage,
          player.rifle.fireRate
        };
    }
    else {

    }
}