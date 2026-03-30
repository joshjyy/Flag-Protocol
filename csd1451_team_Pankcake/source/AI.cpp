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

float DistanceToPlayer(Grunt& grunt, Player& player) {
    float dx = player.x - grunt.movingX;
    float dy = player.y - grunt.movingY;
    return sqrtf(dx * dx + dy * dy);
}

bool HasLineOfSight(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap) {
    float startX = grunt.movingX;
    float startY = grunt.movingY;
    float endX = player.x;
    float endY = player.y;

    int tileColStart = static_cast<int>(floorf((startX + 800.0f - 25.0f) / (float)TILE_SIZE));
    int tileRowStart = static_cast<int>(floorf((450.0f - startY - 25.0f) / (float)TILE_SIZE));
    int tileColEnd = static_cast<int>(floorf((endX + 800.0f - 25.0f) / (float)TILE_SIZE));
    int tileRowEnd = static_cast<int>(floorf((450.0f - endY - 25.0f) / (float)TILE_SIZE));

    int col = tileColStart;
    int row = tileRowStart;

    int stepCol = (tileColEnd > tileColStart) ? 1 : -1;
    int stepRow = (tileRowEnd > tileRowStart) ? 1 : -1;

    float dx = fabsf(endX - startX);
    float dy = fabsf(endY - startY);

    float tDeltaCol = (dx > 0.0f) ? (float)TILE_SIZE / dx : 1e30f;
    float tDeltaRow = (dy > 0.0f) ? (float)TILE_SIZE / dy : 1e30f;

    float tMaxCol, tMaxRow;

    float tileEdgeX = (stepCol > 0)
        ? ((col + 1) * TILE_SIZE - 800.0f + 25.0f)
        : (col * TILE_SIZE - 800.0f + 25.0f);
    float tileEdgeY = (stepRow > 0)
        ? (450.0f - (row + 1) * TILE_SIZE - 25.0f)
        : (450.0f - row * TILE_SIZE - 25.0f);

    tMaxCol = (dx > 0.0f) ? fabsf(tileEdgeX - startX) / dx : 1e30f;
    tMaxRow = (dy > 0.0f) ? fabsf(tileEdgeY - startY) / dy : 1e30f;

    int maxSteps = static_cast<int>(MAP_WIDTH) + static_cast<int>(MAP_HEIGHT);

    for (int step = 0; step < maxSteps; step++) {
        if (col == tileColEnd && row == tileRowEnd)
            return true;
        if (col < 0 || col >= MAP_WIDTH || row < 0 || row >= MAP_HEIGHT)
            return false;
        if (gameMap[row][col] != 0)
            return false;

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

bool IsPlayerVisible(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap) {
    float distance = DistanceToPlayer(grunt, player);
    return (distance < grunt.detectionRange) && HasLineOfSight(grunt, player, gameMap);
}

void AI_Patrol(Grunt& grunt, float dt, std::vector<std::vector<int>>& gameMap) {
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    float dx = grunt.lastSeenPlayerX - grunt.movingX;
    float dy = grunt.lastSeenPlayerY - grunt.movingY;
    float distToLastSeen = sqrtf(dx * dx + dy * dy);

    if (distToLastSeen > AI_INVESTIGATE_ARRIVE_DIST &&
        (grunt.lastSeenPlayerX != 0.0f || grunt.lastSeenPlayerY != 0.0f) &&
        grunt.investigateTimer < AI_INVESTIGATE_MAX_TIME)
    {
        grunt.investigateTimer += dt;

        NormalizeVector(&dx, &dy);
        grunt.movingX += dx * AI_INVESTIGATE_SPEED * dt;
        grunt.movingY += dy * AI_INVESTIGATE_SPEED * dt;

        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            grunt.movingX = oldX;
            grunt.movingY = oldY;

            float perpX = -dy;
            float perpY = dx;
            grunt.movingX = oldX + perpX * AI_INVESTIGATE_SPEED * AI_INVESTIGATE_PERP_MULT * dt;
            grunt.movingY = oldY + perpY * AI_INVESTIGATE_SPEED * AI_INVESTIGATE_PERP_MULT * dt;

            if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
                grunt.movingX = oldX - perpX * AI_INVESTIGATE_SPEED * AI_INVESTIGATE_PERP_MULT * dt;
                grunt.movingY = oldY - perpY * AI_INVESTIGATE_SPEED * AI_INVESTIGATE_PERP_MULT * dt;

                if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
                    grunt.movingX = oldX;
                    grunt.movingY = oldY;
                }
            }
        }
    }
    else {
        grunt.lastSeenPlayerX = 0.0f;
        grunt.lastSeenPlayerY = 0.0f;
        grunt.investigateTimer = 0.0f;

        grunt.movingX += cosf(grunt.patrolAngle) * AI_WANDER_SPEED * dt;
        grunt.movingY += sinf(grunt.patrolAngle) * AI_WANDER_SPEED * dt;
        grunt.patrolAngle += AI_WANDER_ANGLE_RATE * dt;

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

    grunt.investigateTimer = 0.0f;

    float dx = player.x - grunt.movingX;
    float dy = player.y - grunt.movingY;
    float distance = DistanceToPlayer(grunt, player);

    if (distance > AI_CHASE_ARRIVE_DIST) {
        NormalizeVector(&dx, &dy);
        grunt.movingX += dx * AI_CHASE_SPEED * dt;
        grunt.movingY += dy * AI_CHASE_SPEED * dt;
    }

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

    grunt.lastSeenPlayerX = player.x;
    grunt.lastSeenPlayerY = player.y;
}

void AI_Attack(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::vector<std::vector<int>>& gameMap) {
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

    float strafeDir = sinf(grunt.stateTimer * AI_STRAFE_OSCILLATE_SPEED);
    float perpX = -(player.y - grunt.movingY);
    float perpY = (player.x - grunt.movingX);
    NormalizeVector(&perpX, &perpY);

    grunt.movingX += perpX * AI_STRAFE_SPEED * strafeDir * dt;
    grunt.movingY += perpY * AI_STRAFE_SPEED * strafeDir * dt;

    if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
        grunt.movingX = oldX;
        grunt.movingY = oldY;
    }
}

void AI_Retreat(Grunt& grunt, Player& player, float dt, std::vector<std::vector<int>>& gameMap) {
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    float dx = grunt.movingX - player.x;
    float dy = grunt.movingY - player.y;
    NormalizeVector(&dx, &dy);

    grunt.movingX += dx * AI_RETREAT_SPEED * dt;
    grunt.movingY += dy * AI_RETREAT_SPEED * dt;

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

GruntAIState EvaluateDecisionTree(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap) {
    if (grunt.isDead || grunt.health <= 0)
        return AI_DEAD;

    if (ShouldRetreat(grunt))
        return AI_RETREAT;

    if (IsPlayerVisible(grunt, player, gameMap)) {
        if (IsReloading(grunt))
            return AI_RETREAT;
        if (!HasAmmo(grunt))
            return AI_RETREAT;
        if (IsPlayerInAttackRange(grunt, player))
            return AI_ATTACK;
        else
            return AI_CHASE;
    }

    return AI_PATROL;
}

void UpdateGruntAI(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::vector<std::vector<int>>& gameMap) {
    grunt.stateTimer += dt;

    GruntAIState newState = EvaluateDecisionTree(grunt, player, gameMap);

    if (newState != grunt.currentState) {
        GruntAIState oldState = grunt.currentState;
        grunt.currentState = newState;
        grunt.stateTimer = 0.0f;

       // const char* stateNames[] = { "IDLE", "PATROL", "CHASE", "ATTACK", "RETREAT", "SEEK_COVER", "DEAD" };
       // printf("Grunt AI: %s -> %s\n", stateNames[oldState], stateNames[newState]);
    }

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
    if (grunt.pistol.reloadTimer > 0.0f) {
        grunt.pistol.reloadTimer -= dt;
        if (grunt.pistol.reloadTimer <= 0.0f) {
            grunt.pistol.currentAmmo = grunt.pistol.maxAmmo;
            grunt.pistol.inReload = false;
        }
    }

    if (grunt.shootCooldown > 0.0f)
        grunt.shootCooldown -= dt;
}