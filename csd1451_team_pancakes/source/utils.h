#pragma once
#include "AEEngine.h"


int AreCirclesIntersecting(float circle_center1_x, float circle_center1_y, float radius_1,
    float circle_center2_x, float circle_center2_y, float radius_2);


//int AreBoxesIntersecting(float box1_center_x, float box1_center_y, float box1_width, float box1_height,
//    float box2_center_x, float box2_center_y, float box2_width, float box2_height);


void DrawSprite(AEGfxVertexList* mesh, float x, float y, float scaleX, float scaleY, float rotation);