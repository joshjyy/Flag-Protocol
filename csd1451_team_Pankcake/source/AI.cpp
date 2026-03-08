#include "ai.hpp"
#include "player.hpp"
#include "map.hpp"
#include "gameConstants.hpp"
#include <cmath>

//helper functions to do checks
float DistanceToPlayer(Grunt& grunt, Player& player) {
    float dx = player.x - grunt.movingX;
    float dy = player.y - grunt.movingY;
    return sqrtf(dx * dx + dy * dy);
}

bool HasLineOfSight(Grunt& grunt, Player& player, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    // Cast a ray from grunt to player
    float dx = player.x - grunt.movingX;
    float dy = player.y - grunt.movingY;
    float distance = sqrtf(dx * dx + dy * dy);

    // Normalize direction
    if (distance > 0.0f) {
        dx /= distance;
        dy /= distance;
    }

    // Step along the ray and check for walls
    const float STEP_SIZE = 10.0f;  // Check every 10 pixels
    int numSteps = static_cast<int>((distance / STEP_SIZE));

    for (int i = 1; i < numSteps; i++) {
        float checkX = grunt.movingX + dx * STEP_SIZE * i;
        float checkY = grunt.movingY + dy * STEP_SIZE * i;

        // If any point hits a wall, no line of sight
        if (checkCollision(checkX, checkY, 5.0f, gameMap)) {
            return false;
        }
    }

    return true;  // No walls blocking
}


//Decision checks
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

bool IsPlayerVisible(Grunt& grunt, Player& player, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    float distance = DistanceToPlayer(grunt, player);
    return (distance < grunt.detectionRange) && HasLineOfSight(grunt, player, gameMap);
}

//Standard behaviours
void AI_Patrol(Grunt& grunt, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    // Save old position
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    // Check if grunt has a last known player position to investigate
    float dx = grunt.lastSeenPlayerX - grunt.movingX;
    float dy = grunt.lastSeenPlayerY - grunt.movingY;
    float distToLastSeen = sqrtf(dx * dx + dy * dy);

    const float MAX_INVESTIGATE_TIME = 5.0f;  // Give up after 5 seconds

    // If we have a last seen position and haven't timed out
    if (distToLastSeen > 30.0f &&
        (grunt.lastSeenPlayerX != 0.0f || grunt.lastSeenPlayerY != 0.0f) &&
        grunt.investigateTimer < MAX_INVESTIGATE_TIME) {

        // Increment investigate timer
        grunt.investigateTimer += dt;

        // Move toward last seen position
        NormalizeVector(&dx, &dy);
        float investigateSpeed = 100.0f;
        grunt.movingX += dx * investigateSpeed * dt;
        grunt.movingY += dy * investigateSpeed * dt;

        // Check collision
        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            // Hit wall - revert
            grunt.movingX = oldX;
            grunt.movingY = oldY;

            // Try moving perpendicular to navigate around obstacle
            float perpX = -dy;
            float perpY = dx;

            grunt.movingX = oldX + perpX * investigateSpeed * 0.8f * dt;
            grunt.movingY = oldY + perpY * investigateSpeed * 0.8f * dt;

            // Check perpendicular movement
            if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
                // Try opposite perpendicular
                grunt.movingX = oldX - perpX * investigateSpeed * 0.8f * dt;
                grunt.movingY = oldY - perpY * investigateSpeed * 0.8f * dt;

                if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
                    // Completely stuck - just stay here
                    grunt.movingX = oldX;
                    grunt.movingY = oldY;
                }
            }
        }
    }
    else {
        // Timed out, reached position, or no target - reset and wander
        grunt.lastSeenPlayerX = 0.0f;
        grunt.lastSeenPlayerY = 0.0f;
        grunt.investigateTimer = 0.0f;

        // Idle wander
        grunt.movingX += cosf(grunt.patrolAngle) * 30.0f * dt;
        grunt.movingY += sinf(grunt.patrolAngle) * 30.0f * dt;
        grunt.patrolAngle += 0.5f * dt;

        // Check collision during wandering
        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            grunt.movingX = oldX;
            grunt.movingY = oldY;
            grunt.patrolAngle += PI / 2;
        }
    }
}

void AI_Chase(Grunt& grunt, Player& player, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    // Save old position
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    //Reset Timer
    grunt.investigateTimer = 0.0f;

    // Move towards player
    float dx = player.x - grunt.movingX;
    float dy = player.y - grunt.movingY;
    float distance = DistanceToPlayer(grunt, player);

    if (distance > 10.0f) {
        // Normalize direction vector
        NormalizeVector(&dx, &dy);
        grunt.movingX += dx * AI_CHASE_SPEED * dt;
        grunt.movingY += dy * AI_CHASE_SPEED * dt;
    }

    // Check collision with walls
    if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
        // Revert to old position
        grunt.movingX = oldX;
        grunt.movingY = oldY;

        // Try to navigate around obstacle by moving perpendicular
        float perpX = -dy;
        float perpY = dx;
        grunt.movingX += perpX * AI_CHASE_SPEED * 0.5f * dt;
        grunt.movingY += perpY * AI_CHASE_SPEED * 0.5f * dt;

        // Check again
        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            grunt.movingX = oldX;
            grunt.movingY = oldY;
        }
    }

    // Update last seen position
    grunt.lastSeenPlayerX = player.x;
    grunt.lastSeenPlayerY = player.y;
}

void AI_Attack(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    if (grunt.shootCooldown <= 0.0f && grunt.pistol.currentAmmo > 0 && !grunt.pistol.inReload) {
        Bullet newBullet = CreateBullet(
            grunt.movingX,
            grunt.movingY,
            player.x,
            player.y,
            grunt.pistol.bulletSpeed,
            grunt.pistol.bulletDamage
        );
        enemyBullets.push_back(newBullet);
        grunt.pistol.currentAmmo--;
        grunt.shootCooldown = 2.0f;
        //printf("Grunt attacking! Ammo: %d/%d\n", grunt.pistol.currentAmmo, grunt.pistol.maxAmmo);
    }

    // Save old position
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    // Strafe while attacking (adds challenge)
    float strafeDir = sinf(grunt.stateTimer * 2.0f);  // Oscillate left/right
    float perpX = -(player.y - grunt.movingY);
    float perpY = (player.x - grunt.movingX);

    // Normalize perpendicular vector
    NormalizeVector(&perpX, &perpY);

    grunt.movingX += perpX * AI_STRAFE_SPEED * strafeDir * dt;
    grunt.movingY += perpY * AI_STRAFE_SPEED * strafeDir * dt;

    // Check collision with walls
    if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
        // Revert to old position
        grunt.movingX = oldX;
        grunt.movingY = oldY;
    }
}

void AI_Retreat(Grunt& grunt, Player& player, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    // Save old position
    float oldX = grunt.movingX;
    float oldY = grunt.movingY;

    // Move away from player
    float dx = grunt.movingX - player.x;
    float dy = grunt.movingY - player.y;

    // Normalize direction vector
    NormalizeVector(&dx, &dy);

    grunt.movingX += dx * AI_RETREAT_SPEED * dt;
    grunt.movingY += dy * AI_RETREAT_SPEED * dt;

    // Check collision with walls
    if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
        // Revert to old position
        grunt.movingX = oldX;
        grunt.movingY = oldY;

        // Try moving sideways instead
        float perpX = -dy;
        float perpY = dx;
        grunt.movingX += perpX * AI_RETREAT_SPEED * 0.7f * dt;
        grunt.movingY += perpY * AI_RETREAT_SPEED * 0.7f * dt;

        // Check again
        if (checkCollision(grunt.movingX, grunt.movingY, GRUNT_RADIUS / 2, gameMap)) {
            grunt.movingX = oldX;
            grunt.movingY = oldY;
        }
    }
}

//Update Decision Tree based on Boolean Flags above
GruntAIState EvaluateDecisionTree(Grunt& grunt, Player& player, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    // Dead check (highest priority)
    if (grunt.isDead || grunt.health <= 0) {
        return AI_DEAD;
    }

    // Health-based retreat
    if (ShouldRetreat(grunt)) {
        return AI_RETREAT;
    }

    // Player detection
    if (IsPlayerVisible(grunt, player, gameMap)) {
        // Reloading check
        if (IsReloading(grunt)) {
            return AI_RETREAT;
        }

        // Ammo check
        if (!HasAmmo(grunt)) {
            return AI_RETREAT;
        }

        // Range-based decision
        if (IsPlayerInAttackRange(grunt, player)) {
            return AI_ATTACK;
        }
        else {
            return AI_CHASE;
        }
    }
    else {
        // Default: patrol
        return AI_PATROL;
    }
}

//Main AI Update function for Grunts
void UpdateGruntAI(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    // Update state timer
    grunt.stateTimer += dt;

    // Evaluate decision tree
    GruntAIState newState = EvaluateDecisionTree(grunt, player, gameMap);

    // State transition
    if (newState != grunt.currentState) {
        GruntAIState oldState = grunt.currentState;
        grunt.currentState = newState;
        grunt.stateTimer = 0.0f;

        //Debug output
        const char* stateNames[] = { "IDLE", "PATROL", "CHASE", "ATTACK", "RETREAT", "SEEK_COVER", "DEAD" };
        printf("Grunt AI: %s -> %s\n", stateNames[oldState], stateNames[newState]);
    }

    // Execute behavior
    switch (grunt.currentState) {
    case AI_PATROL:
        AI_Patrol(grunt, dt, gameMap);
        break;

    case AI_CHASE:
        AI_Chase(grunt, player, dt, gameMap);
        break;

    case AI_ATTACK:
        AI_Attack(grunt, player, enemyBullets, dt, gameMap);
        break;

    case AI_RETREAT:
        AI_Retreat(grunt, player, dt, gameMap);
        break;

    case AI_DEAD:
        break;

    default:
        break;
    }

    // Handle reloading
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

    // Update shoot cooldown
    if (grunt.shootCooldown > 0.0f) {
        grunt.shootCooldown -= dt;
    }
}