/*!************************************************************************
  \file debris.hpp
  \author






  \brief
  This source file contains declarations for the background particles.

**************************************************************************/

#pragma once
#include "AEEngine.h"
#include "map.hpp"
struct Asteroid {
    AEVec2 pos;
    AEVec2 vel;
    float scale;
};

void Background_Load();
void Background_Init();
void Background_Update(float g_dt);
void Background_Draw(CameraPos camera);
void Background_Free();