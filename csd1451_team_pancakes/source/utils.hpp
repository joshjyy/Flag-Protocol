#pragma once
#include "AEEngine.h"
#include "map.hpp"



// Collision detection
int AreCirclesIntersecting(float circle_center1_x, float circle_center1_y, float radius_1,
    float circle_center2_x, float circle_center2_y, float radius_2);

int AreBoxesIntersecting(float box1_center_x, float box1_center_y, float box1_width, float box1_height,
    float box2_center_x, float box2_center_y, float box2_width, float box2_height);

void DrawSprite(AEGfxVertexList* mesh, float x, float y, float scaleX, float scaleY, float rotation);

int checkCollision(float playerX, float playerY, float radius,
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);

// Common mesh management
typedef struct {
    AEGfxVertexList* square;
    AEGfxVertexList* circle;
} CommonMeshes;

extern CommonMeshes meshes;  // Global mesh storage

void InitCommonMeshes(void);
void FreeCommonMeshes(void);

