#include "ai.hpp"
#include "player.hpp"
#include "map.hpp"
#include <cmath>

//helper function to do checks
float DistanceToPlayer(Grunt& grunt, Player& player) {
    float dx = player.x - grunt.movingX;
    float dy = player.y - grunt.movingY;
    return sqrtf(dx * dx + dy * dy);
}


//Decision checks
bool IsPlayerVisible(Grunt& grunt, Player& player) {
    return DistanceToPlayer(grunt, player) < grunt.detectionRange;
}

bool IsPlayerInAttackRange(Grunt& grunt, Player& player) {
    return DistanceToPlayer(grunt, player) < grunt.attackRange;
}

bool ShouldRetreat(Grunt& grunt, Player& player) {
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

//Standard behaviours
void AI_Patrol(Grunt& grunt, float dt) {
    // Circle around spawn point
    grunt.patrolAngle += AI_PATROL_SPEED * dt * 0.01f;
    grunt.movingX = grunt.startX + cosf(grunt.patrolAngle) * grunt.patrolRadius;
    grunt.movingY = grunt.startY + sinf(grunt.patrolAngle) * grunt.patrolRadius;
}

void AI_Chase(Grunt& grunt, Player& player, float dt) {
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

    // Update last seen position
    grunt.lastSeenPlayerX = player.x;
    grunt.lastSeenPlayerY = player.y;
}

void AI_Attack(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt) {
    // Shoot at player
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
        printf("Grunt attacking! Ammo: %d/%d\n", grunt.pistol.currentAmmo, grunt.pistol.maxAmmo);
    }

    // Strafe while attacking 
    float strafeDir = sinf(grunt.stateTimer * 2.0f);  
    float perpX = -(player.y - grunt.movingY);
    float perpY = (player.x - grunt.movingX);

    // Normalize perpendicular vector
    NormalizeVector(&perpX, &perpY);

    grunt.movingX += perpX * AI_STRAFE_SPEED * strafeDir * dt;
    grunt.movingY += perpY * AI_STRAFE_SPEED * strafeDir * dt;
}

void AI_Retreat(Grunt& grunt, Player& player, float dt) {
    // Move away from player
    float dx = grunt.movingX - player.x;
    float dy = grunt.movingY - player.y;

    // Normalize direction vector
    NormalizeVector(&dx, &dy);

    grunt.movingX += dx * AI_RETREAT_SPEED * dt;
    grunt.movingY += dy * AI_RETREAT_SPEED * dt;
}

//Update Decision Tree based on Boolean Flags above
GruntAIState EvaluateDecisionTree(Grunt& grunt, Player& player) {
    // Dead check (highest priority)
    if (grunt.isDead || grunt.health <= 0) {
        return AI_DEAD;
    }

    // Health-based retreat
    if (ShouldRetreat(grunt, player)) {
        return AI_RETREAT;
    }

    // Player detection
    if (IsPlayerVisible(grunt, player)) {
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
void UpdateGruntAI(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt) {
    // Update state timer
    grunt.stateTimer += dt;

    // Evaluate decision tree
    GruntAIState newState = EvaluateDecisionTree(grunt, player);

    // State transition
    if (newState != grunt.currentState) {
        grunt.currentState = newState;
        grunt.stateTimer = 0.0f;

        // Debug output
        const char* stateNames[] = { "IDLE", "PATROL", "CHASE", "ATTACK", "RETREAT", "SEEK_COVER", "DEAD" };
        printf("Grunt AI: %s\n", stateNames[newState]);
    }

    // Execute Behaviour based on Decision Tree
    switch (grunt.currentState) {
    case AI_PATROL:
        AI_Patrol(grunt, dt);
        break;

    case AI_CHASE:
        AI_Chase(grunt, player, dt);
        break;

    case AI_ATTACK:
        AI_Attack(grunt, player, enemyBullets, dt);
        break;

    case AI_RETREAT:
        AI_Retreat(grunt, player, dt);
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