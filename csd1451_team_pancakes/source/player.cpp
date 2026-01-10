#include "player.hpp"
#include "AEEngine.h"
#include "utils.hpp"
#include <cmath>

// Add this function to Bullet.cpp (put it before CreateBullet)
void NormalizeVector(float* x, float* y) {
    float length = sqrtf((*x) * (*x) + (*y) * (*y));
    if (length > 0.0f) {
        *x /= length;
        *y /= length;
    }
}
//Create a new bullet and allocate memory
//Parameters: starting position (x, y), target position (targetX, targetY), speed
Bullet* CreateBullet(float x, float y, float targetX, float targetY, float speed) {
    Bullet* newBullet = new Bullet;
    newBullet->x = x;
    newBullet->y = y;


    // Calculate direction vector
    float dirX = targetX - x;
    float dirY = targetY - y;

    // Normalize the direction
    NormalizeVector(&dirX, &dirY);


    // Set velocity
    newBullet->velocityX = dirX * speed;
    newBullet->velocityY = dirY * speed;
    newBullet->active = true;
    newBullet->next = nullptr;

    return newBullet;
}

// Update all bullets and remove off-screen ones
void UpdateBullets(Bullet** bulletHead, float dt) {
    Bullet* current = *bulletHead;
    Bullet* previous = nullptr;

    while (current != nullptr) {
        // Update bullet position
        current->x += current->velocityX * dt;
        current->y += current->velocityY * dt;

        // Check if bullet is off screen 
        if (current->y > 500.0f || current->y < -500.0f ||
            current->x > 900.0f || current->x < -900.0f) {

            // Remove bullet from list and free memory
            Bullet* toDelete = current;

            if (previous == nullptr) {
                // Removing head of list
                *bulletHead = current->next;
                current = *bulletHead;
            }
            else {
                // Removing middle/end of list
                previous->next = current->next;
                current = current->next;
            }

            delete toDelete;  // Free the memory
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
    while (current != nullptr) {
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

