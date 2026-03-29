/*!************************************************************************
  \file AI.hpp
  \author 
  Ye Min Thwin





  \brief
  This source file contains declarations for the enemy AI system, including the Grunt's behavior and decision-making logic.

**************************************************************************/

#pragma once


#include "enemies.hpp"
#include "player.hpp"
#include "utils.hpp"        // For AreCirclesIntersecting
#include "map.hpp"
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
void UpdateGruntAI(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt,
    std::vector<std::vector<int>>& gameMap);

//Decision Tree
GruntAIState EvaluateDecisionTree(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap);

// Decision Tree Checks
bool IsPlayerVisible(Grunt& grunt, Player& player);
bool IsPlayerInAttackRange(Grunt& grunt, Player& player);
bool ShouldRetreat(Grunt& grunt);
bool IsPlayerTooClose(Grunt& grunt, Player& player);
bool HasAmmo(Grunt& grunt);
bool IsReloading(Grunt& grunt);
bool IsPlayerVisible(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap);
bool HasLineOfSight(Grunt& grunt, Player& player, std::vector<std::vector<int>>& gameMap);

//Normal Behaviours
void AI_Patrol(Grunt& grunt, float dt, std::vector<std::vector<int>>& gameMap);
void AI_Chase(Grunt& grunt, Player& player, float dt, std::vector<std::vector<int>>& gameMap);
void AI_Attack(Grunt& grunt, Player& player, std::vector<Bullet>& enemyBullets, float dt, std::vector<std::vector<int>>& gameMap);
void AI_Retreat(Grunt& grunt, Player& player, float dt, std::vector<std::vector<int>>& gameMap);

// Helper functions
float DistanceToPlayer(Grunt& grunt, Player& player);

