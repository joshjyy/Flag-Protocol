/*!************************************************************************
  \file utils.hpp
  \author
  Yeo Kah Boon Joshua 





  \brief
  This source file contains declarations of the universal functions.

**************************************************************************/

#pragma once
#include "AEEngine.h"
#include "map.hpp"
#include "player.hpp"   


extern s8 fontID;

int AreCirclesIntersecting(float circle_center1_x, float circle_center1_y, float radius_1,
    float circle_center2_x, float circle_center2_y, float radius_2);

int AreBoxesIntersecting(float box1_center_x, float box1_center_y, float box1_width, float box1_height,
    float box2_center_x, float box2_center_y, float box2_width, float box2_height);

void DrawSprite(AEGfxVertexList* mesh, float x, float y, float scaleX, float scaleY, float rotation);

int checkCollision(float playerX, float playerY, float radius,
    std::vector<std::vector<int>>& gameMap);
//Overload for Player Flag System
int checkCollision(Player& player, float radius,
    std::vector<std::vector<int>>& gameMap);

void GetNormalizedTextPosition(float pixelX, float pixelY, float* outNormalizedX, float* outNormalizedY);
void NormalizeVector(float* x, float* y);
// Common mesh management
typedef struct {
    AEGfxVertexList* square;
    AEGfxVertexList* circle;
	AEGfxVertexList* arrow;
    AEGfxVertexList* debug_quad;
} CommonMeshes;

extern CommonMeshes meshes;  // Global mesh storage

void InitCommonMeshes(void);
void FreeCommonMeshes(void);

void ForceShowCursor(bool show);