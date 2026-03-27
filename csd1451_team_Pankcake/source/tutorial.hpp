/*!************************************************************************
  \file tutorial.hpp
  \author
  Yeo Kah Boon Joshua (Tutorial UI for Player)





  \brief
  This source file contains declarations of the stuff related to the tutorial level.

**************************************************************************/

#pragma once
#include "AEEngine.h"
#include "utils.hpp"
#include "player.hpp"
#include "enemies.hpp"
#include "events.hpp"

void Tutorial_RenderHints(s8 font);
void Range_Hacks(Player& player);	
void Tutorial_RenderDebug(s8 font, const Player& player);

void UpdateAndRenderTutorial(
    Player& player,
    float dt,
    std::vector<std::vector<int>>* gameMap,
    CameraPos& camera, 
    std::vector<Bullet>& playerBullets,
    std::vector<Bullet>& enemyBullets,
    std::vector<Grunt>& grunts,
    EventManager& events);