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