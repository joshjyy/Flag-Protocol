#pragma once
#include "utils.hpp"
#include "enemies.hpp"
#include "player.hpp"
#include "damageNumbers.hpp"
#include "AI.hpp"
#include "weapon.hpp"
#include "events.hpp"
#include "map.hpp"

void UpdateAndRenderGameplay(
    Player& player,
    float dt,
    std::vector<std::vector<int>>* gameMap,
    float& cameraX, float& cameraY,
    std::vector<Bullet>& playerBullets,
    std::vector<Bullet>& enemyBullets,
    std::vector<Grunt>& grunts,
    EventManager& events);