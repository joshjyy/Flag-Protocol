/*!************************************************************************
  \file AI.cpp
  \author
  Ye Min Thwin

  \brief
  This source file contains definitions for the enemy AI system, including
  the Grunt's behavior and decision-making logic.
**************************************************************************/

#include "ai.hpp"
#include "player.hpp"
#include "map.hpp"
#include "gameConstants.hpp"
#include <cmath>

// Returns the straight-line distance between a grunt and the player
float DistanceToPlayer(Grunt& grunt, Player& player) {
    float dx = player.x - grunt.movingX;
    float dy = player.y - grunt.movingY;
    return sqrtf(dx * dx + dy * dy);
}

// Casts a ray from the grunt to the player using DDA (Digital Differential Analysis).
// Returns true if no wall tiles block the path between them.
bool HasLineOfSight(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap) {
    float startX = grunt.movingX;
    float startY = grunt.movingY;
    float endX = player.x;
    float endY = player.y;

    // Convert world positions to tile grid indices
    int tileColStart = static_cast<int>(floorf((startX + 800.0f - 25.0f) / (float)TILE_SIZE));
    int tileRowStart = static_cast<int>(floorf((450.0f - startY - 25.0f) / (float)TILE_SIZE));
    int tileColEnd = static_cast<int>(floorf((endX + 800.0f - 25.0f) / (float)TILE_SIZE));
    int tileRowEnd = static_cast<int>(floorf((450.0f - endY - 25.0f) / (float)TILE_SIZE));

    int col = tileColStart;
    int row = tileRowStart;

    // Which direction to step along each axis (+1 or -1)
    int stepCol = (tileColEnd > tileColStart) ? 1 : -1;
    int stepRow = (tileRowEnd > tileRowStart) ? 1 : -1;

    float dx = fabsf(endX - startX);
    float dy = fabsf(endY - startY);

    // How much t increases when crossing one full tile on each axis.
// If the ray is perfectly axis-aligned, set to a large value to avoid division by zero.
    float tDeltaCol = (dx > 0.0f) ? (float)TILE_SIZE / dx : 1e30f;
    float tDeltaRow = (dy > 0.0f) ? (float)TILE_SIZE / dy : 1e30f;

    float tMaxCol, tMaxRow;

    // World-space position of the nearest tile boundary in each direction
    float tileEdgeX = (stepCol > 0)
        ? ((col + 1) * TILE_SIZE - 800.0f + 25.0f)
        : (col * TILE_SIZE - 800.0f + 25.0f);
    float tileEdgeY = (stepRow > 0)
        ? (450.0f - (row + 1) * TILE_SIZE - 25.0f)
        : (450.0f - row * TILE_SIZE - 25.0f);

    // t value at which the ray first crosses each axis boundary
    tMaxCol = (dx > 0.0f) ? fabsf(tileEdgeX - startX) / dx : 1e30f;
    tMaxRow = (dy > 0.0f) ? fabsf(tileEdgeY - startY) / dy : 1e30f;

    // Cap iterations to the map diagonal to prevent infinite loops
    int maxSteps = static_cast<int>(MAP_WIDTH) + static_cast<int>(MAP_HEIGHT);

    for (int step = 0; step < maxSteps; step++) {
        // Reached the destination tile – line of sight is clear
        if (col == tileColEnd && row == tileRowEnd)
            return true;
        // Ray has left the map bounds – no line of sight
        if (col < 0 || col >= MAP_WIDTH || row < 0 || row >= MAP_HEIGHT)
            return false;
        // Hit a wall tile – line of sight is blocked
        if (gameMap[row][col] != 0)
            return false;

        // Advance to the next tile along whichever axis boundary is closer
        if (tMaxCol < tMaxRow) {
            tMaxCol += tDeltaCol;
            col += stepCol;
        }
        else {
            tMaxRow += tDeltaRow;
            row += stepRow;
        }
    }
    return true;
}

// Simple range-only visibility check (no wall test)
bool IsPlayerVisible(Grunt& grunt, Player& player) {
    return DistanceToPlayer(grunt, player) < grunt.detectionRange;
}

bool IsPlayerInAttackRange(Grunt& grunt, Player& player) {
    return DistanceToPlayer(grunt, player) < grunt.attackRange;
}

bool ShouldRetreat(Grunt& grunt) {
    float healthPercent = (grunt.health / 100.0f) * 100.0f;
    return healthPercent < grunt.retreatHealthThreshold;
}

bool IsPlayerTooClose(Grunt& grunt, Player& player) {
    return DistanceToPlayer(grunt, player) < AI_TOO_CLOSE_RANGE;
}

bool HasAmmo(Grunt& grunt) {
    return grunt.pistol.currentAmmo > 0;
}

bool IsReloading(Grunt& grunt) {
    return grunt.pistol.inReload;
}

// Full visibility check: player must be in range AND have an unobstructed line of sight
bool IsPlayerVisible(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap) {
    float distance = DistanceToPlayer(grunt, player);
    return (distance < grunt.detectionRange) && HasLineOfSight(grunt, player, gameMap);
}

// Patrol: move toward the last-known player position if one exists.
// Once the grunt arrives or the timer expires, fall back to idle wandering.
void AI_Patrol(Grunt& grunt, float dt, std::vector<std::vector<int>>& gameMap) {
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    float dx = grunt.lastSeenPlayerX - grunt.movingX;
    float dy = grunt.lastSeenPlayerY - grunt.movingY;
    float distToLastSeen = sqrtf(dx * dx + dy * dy);

    // Investigate if a valid last-seen position exists and the timer has not expired
    if (distToLastSeen > AI_INVESTIGATE_ARRIVE_DIST &&
        (grunt.lastSeenPlayerX != 0.0f || grunt.lastSeenPlayerY != 0.0f) &&
        grunt.investigateTimer < AI_INVESTIGATE_MAX_TIME)
    {
        grunt.investigateTimer += dt;

        NormalizeVector(&dx, &dy);
        grunt.movingX += dx * AI_INVESTIGATE_SPEED * dt;
        grunt.movingY += dy * AI_INVESTIGATE_SPEED * dt;

        // Wall hit: revert and try sliding along the wall perpendicular to our direction
        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            grunt.movingX = oldX;
            grunt.movingY = oldY;

            float perpX = -dy;
            float perpY = dx;
            grunt.movingX = oldX + perpX * AI_INVESTIGATE_SPEED * AI_INVESTIGATE_PERP_MULT * dt;
            grunt.movingY = oldY + perpY * AI_INVESTIGATE_SPEED * AI_INVESTIGATE_PERP_MULT * dt;

            if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
                // Try the opposite perpendicular direction
                grunt.movingX = oldX - perpX * AI_INVESTIGATE_SPEED * AI_INVESTIGATE_PERP_MULT * dt;
                grunt.movingY = oldY - perpY * AI_INVESTIGATE_SPEED * AI_INVESTIGATE_PERP_MULT * dt;

                if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
                    // Completely stuck – stay in place this frame
                    grunt.movingX = oldX;
                    grunt.movingY = oldY;
                }
            }
        }
    }
    else {
        // No valid target – clear last-seen data and wander idly
        grunt.lastSeenPlayerX = 0.0f;
        grunt.lastSeenPlayerY = 0.0f;
        grunt.investigateTimer = 0.0f;

        // Move in the direction of patrolAngle, slowly rotating it each frame
        grunt.movingX += cosf(grunt.patrolAngle) * AI_WANDER_SPEED * dt;
        grunt.movingY += sinf(grunt.patrolAngle) * AI_WANDER_SPEED * dt;
        grunt.patrolAngle += AI_WANDER_ANGLE_RATE * dt;

        // Wall hit during wander: revert and rotate sharply to pick a new direction
        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            grunt.movingX = oldX;
            grunt.movingY = oldY;
            grunt.patrolAngle += AI_WANDER_WALL_BOUNCE_ANGLE;
        }
    }
}

void AI_Chase(Grunt& grunt, Player& player, float dt, std::vector<std::vector<int>>& gameMap) {
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    // Actively chasing means we have a confirmed position
    // reset the investigate timer
    grunt.investigateTimer = 0.0f;

    float dx = player.x - grunt.movingX;
    float dy = player.y - grunt.movingY;
    float distance = DistanceToPlayer(grunt, player);

    // Move toward the player only if not already overlapping
    if (distance > AI_CHASE_ARRIVE_DIST) {
        NormalizeVector(&dx, &dy);
        grunt.movingX += dx * AI_CHASE_SPEED * dt;
        grunt.movingY += dy * AI_CHASE_SPEED * dt;
    }

    // Wall hit: revert and try sliding perpendicular to navigate around the obstacle
    if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
        grunt.movingX = oldX;
        grunt.movingY = oldY;

        float perpX = -dy;
        float perpY = dx;
        grunt.movingX += perpX * AI_CHASE_SPEED * AI_CHASE_PERP_MULT * dt;
        grunt.movingY += perpY * AI_CHASE_SPEED * AI_CHASE_PERP_MULT * dt;

        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            grunt.movingX = oldX;
            grunt.movingY = oldY;
        }
    }

    // Keep the last-seen position up to date while actively chasing
    grunt.lastSeenPlayerX = player.x;
    grunt.lastSeenPlayerY = player.y;
}

void AI_Attack(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::vector<std::vector<int>>& gameMap) {
    // Fire when cooldown has expired, ammo is available, and not currently reloading
    if (grunt.shootCooldown <= 0.0f && grunt.pistol.currentAmmo > 0 && !grunt.pistol.inReload) {
        Bullet newBullet = CreateBullet(
            grunt.movingX, grunt.movingY,
            player.x, player.y,
            grunt.pistol.bulletSpeed,
            grunt.pistol.bulletDamage
        );
        enemyBullets.push_back(newBullet);
        grunt.pistol.currentAmmo--;
        grunt.shootCooldown = AI_SHOOT_COOLDOWN;
    }

    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    // Strafe sideways using a sine wave so the grunt oscillates left and right
    // rather than moving in a single constant directio
    float strafeDir = sinf(grunt.stateTimer * AI_STRAFE_OSCILLATE_SPEED);
    float perpX = -(player.y - grunt.movingY);
    float perpY = (player.x - grunt.movingX);
    NormalizeVector(&perpX, &perpY);

    grunt.movingX += perpX * AI_STRAFE_SPEED * strafeDir * dt;
    grunt.movingY += perpY * AI_STRAFE_SPEED * strafeDir * dt;

    // Revert if the strafe movement hit a wall
    if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
        grunt.movingX = oldX;
        grunt.movingY = oldY;
    }
}

void AI_Retreat(Grunt& grunt, Player& player, float dt, std::vector<std::vector<int>>& gameMap) {
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    // Direction vector pointing directly away from the player
    float dx = grunt.movingX - player.x;
    float dy = grunt.movingY - player.y;
    NormalizeVector(&dx, &dy);

    grunt.movingX += dx * AI_RETREAT_SPEED * dt;
    grunt.movingY += dy * AI_RETREAT_SPEED * dt;

    // Wall hit: revert and try sliding sideways to get around the obstacle
    if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
        grunt.movingX = oldX;
        grunt.movingY = oldY;

        float perpX = -dy;
        float perpY = dx;
        grunt.movingX += perpX * AI_RETREAT_SPEED * AI_RETREAT_PERP_MULT * dt;
        grunt.movingY += perpY * AI_RETREAT_SPEED * AI_RETREAT_PERP_MULT * dt;

        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            grunt.movingX = oldX;
            grunt.movingY = oldY;
        }
    }
}

// Evaluates the decision tree each frame and returns the highest-priority
// state the grunt should be in given its current situation.
GruntAIState EvaluateDecisionTree(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap) {
    if (grunt.isDead || grunt.health <= 0)
        return AI_DEAD;

    // Low health overrides all combat states except death
    if (ShouldRetreat(grunt))
        return AI_RETREAT;

    if (IsPlayerVisible(grunt, player, gameMap)) {
        // Retreat while reloading so the grunt is not a stationary target
        if (IsReloading(grunt))
            return AI_RETREAT;
        // Out of ammo and reload not yet started – retreat until magazine is refilled
        if (!HasAmmo(grunt))
            return AI_RETREAT;
        if (IsPlayerInAttackRange(grunt, player))
            return AI_ATTACK;
        else
            return AI_CHASE;
    }
    // Player not visible, investigate last known position or wander
    return AI_PATROL;
}

void UpdateGruntAI(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::vector<std::vector<int>>& gameMap) {
    grunt.stateTimer += dt;

    GruntAIState newState = EvaluateDecisionTree(grunt, player, gameMap);

    // On state change reset the timer so behaviour functions know how long
    // the grunt has been in the new state
    if (newState != grunt.currentState) {
        //GruntAIState oldState = grunt.currentState;
        grunt.currentState = newState;
        grunt.stateTimer = 0.0f;

        //const char* stateNames[] = { "IDLE", "PATROL", "CHASE", "ATTACK", "RETREAT", "SEEK_COVER", "DEAD" };
       //printf("Grunt AI: %s -> %s\n", stateNames[oldState], stateNames[newState]);
    }

    // Dispatch to the behaviour function for the current state
    switch (grunt.currentState) {
    case AI_PATROL:  AI_Patrol(grunt, dt, gameMap);                          break;
    case AI_CHASE:   AI_Chase(grunt, player, dt, gameMap);                   break;
    case AI_ATTACK:  AI_Attack(grunt, player, enemyBullets, dt, gameMap);    break;
    case AI_RETREAT: AI_Retreat(grunt, player, dt, gameMap);                 break;
    case AI_DEAD:                                                             break;
    default:                                                                  break;
    }

    // Reloading
    if (grunt.pistol.currentAmmo <= 0 && grunt.pistol.reloadTimer <= 0.0f) {
        grunt.pistol.reloadTimer = grunt.pistol.reloadSpeed;
        grunt.pistol.inReload = true;
    }
    // Count down the reload timer; refill the magazine when it reaches zero
    if (grunt.pistol.reloadTimer > 0.0f) {
        grunt.pistol.reloadTimer -= dt;
        if (grunt.pistol.reloadTimer <= 0.0f) {
            grunt.pistol.currentAmmo = grunt.pistol.maxAmmo;
            grunt.pistol.inReload = false;
        }
    }
    // Tick the shoot cooldown down each frame
    if (grunt.shootCooldown > 0.0f)
        grunt.shootCooldown -= dt;
}