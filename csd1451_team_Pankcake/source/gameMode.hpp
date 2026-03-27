/*!************************************************************************
  \file gameMode.hpp
  \author
  Yeo Kah Boon Joshua (Player Gameplay )






  \brief
  This source file contains declarations of the stuff related to the main level gameplay.

**************************************************************************/

#pragma once
#include "utils.hpp"
#include "enemies.hpp"
#include "player.hpp"
#include "damageNumbers.hpp"
#include "AI.hpp"
#include "weapon.hpp"
#include "events.hpp"
#include "map.hpp"

void DrawControlsHint(s8 font);

void UpdateAndRenderGameplay(
    Player& player,
    float dt,
    std::vector<std::vector<int>>* gameMap,
    CameraPos& camera,
    std::vector<Bullet>& playerBullets,
    std::vector<Bullet>& enemyBullets,
    std::vector<Grunt>& grunts,
    EventManager& events);