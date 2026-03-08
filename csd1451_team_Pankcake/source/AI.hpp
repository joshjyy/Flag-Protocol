#ifndef AI_HPP
#define AI_HPP

#include "enemies.hpp"
#include "player.hpp"
#include "utils.hpp"        // For AreCirclesIntersecting
#include "gameConstants.hpp" // For constants
#include <vector>

// AI States
enum GruntAIState {
    AI_IDLE,
    AI_PATROL,
    AI_CHASE,
    AI_ATTACK,
    AI_RETREAT,
    AI_SEEK_COVER,
    AI_DEAD
};

//Update AI
void UpdateGruntAI(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt);

//Decision Tree
GruntAIState EvaluateDecisionTree(Grunt& grunt, Player& player);

// Decision Tree Checks
bool IsPlayerVisible(Grunt& grunt, Player& player);
bool IsPlayerInAttackRange(Grunt& grunt, Player& player);
bool ShouldRetreat(Grunt& grunt, Player& player);
bool IsPlayerTooClose(Grunt& grunt, Player& player);
bool HasAmmo(Grunt& grunt);
bool IsReloading(Grunt& grunt);

//Normal Behaviours
void AI_Patrol(Grunt& grunt, float dt);
void AI_Chase(Grunt& grunt, Player& player, float dt);
void AI_Attack(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt);
void AI_Retreat(Grunt& grunt, Player& player, float dt);

// Helper functions
float DistanceToPlayer(Grunt& grunt, Player& player);

#endif