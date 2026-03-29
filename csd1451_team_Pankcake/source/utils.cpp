/*!************************************************************************
  \file utils.cpp
  \author
  Yeo Kah Boon Joshua





  \brief
  This source file contains definitions of the universal functions.

**************************************************************************/

#include "utils.hpp"
#include "AEEngine.h"
#include <array>
#include "map.hpp"
#include "player.hpp"
#include "gameConstants.hpp"

int AreCirclesIntersecting(float circle_center1_x, float circle_center1_y, float radius_1, float circle_center2_x, float circle_center2_y, float radius_2) {

    float dist_between_pt = sqrtf((circle_center2_x - circle_center1_x) * (circle_center2_x - circle_center1_x) +
        (circle_center2_y - circle_center1_y) * (circle_center2_y - circle_center1_y)); // distance between circle centers

    float sum_of_radii = radius_1 + radius_2; //since both circles have same diameter, radius1 + radius2 = diameter

    float diff_of_radii = radius_1 - radius_2; //difference of radii is zero since both circles have same diameter


    if (dist_between_pt <= diff_of_radii) {
        return 2; //one circle is inside the other
    }

    else if (dist_between_pt <= sum_of_radii) {
        return 1; //circles are colliding
    }

    else {
        return 0; //circles are not colliding
    }

}

int AreBoxesIntersecting(float box1_center_x, float box1_center_y, float box1_width, float box1_height,
    float box2_center_x, float box2_center_y, float box2_width, float box2_height) {
    float halfW1 = box1_width / 2.0f;
    float halfH1 = box1_height / 2.0f;
    float halfW2 = box2_width / 2.0f;
    float halfH2 = box2_height / 2.0f;

    int overlapX = (box1_center_x - halfW1 < box2_center_x + halfW2) &&
        (box1_center_x + halfW1 > box2_center_x - halfW2);

    int overlapY = (box1_center_y - halfH1 < box2_center_y + halfH2) &&
        (box1_center_y + halfH1 > box2_center_y - halfH2);

    return (overlapX && overlapY) ? 1 : 0;
}

void DrawSprite(AEGfxVertexList* mesh, float x, float y, float scaleX, float scaleY, float rotation) {
    AEMtx33 scale = { 0 };
    AEMtx33Scale(&scale, scaleX, scaleY);

    AEMtx33 rotate = { 0 };
    AEMtx33Rot(&rotate, rotation);

    AEMtx33 translate = { 0 };
    AEMtx33Trans(&translate, x, y);

    AEMtx33 transform = { 0 };
    AEMtx33Concat(&transform, &rotate, &scale);
    AEMtx33Concat(&transform, &translate, &transform);

    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}



//Mesh -----------
CommonMeshes meshes = { NULL };
void InitCommonMeshes(void) {
    // Create square mesh
    AEGfxMeshStart();
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
    );
    AEGfxTriAdd(
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
    );
    meshes.square = AEGfxMeshEnd();

    // Create circle mesh
    AEGfxMeshStart();
    int segments = 360;
    for (int i = 0; i < segments; i++) {
        float angle1 = (i * 360.0f / segments) * PI / 180.0f;
        float angle2 = ((i + 1) * 360.0f / segments) * PI / 180.0f;
        AEGfxTriAdd(
            0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f,
            cosf(angle1) * 0.5f, sinf(angle1) * 0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
            cosf(angle2) * 0.5f, sinf(angle2) * 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
        );
    }
    meshes.circle = AEGfxMeshEnd();

    
        AEGfxMeshStart();
        AEGfxTriAdd(
            0.5f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f,   // tip
            -0.5f, 0.3f, 0xFFFFFFFF, 0.0f, 0.0f,   // top-left
            -0.5f, -0.3f, 0xFFFFFFFF, 0.0f, 0.0f    // bottom-left
        );

      meshes.arrow = AEGfxMeshEnd();
    





}

void FreeCommonMeshes(void) {
    if (meshes.square) {
        AEGfxMeshFree(meshes.square);
        meshes.square = NULL;
    }
    if (meshes.circle) {
        AEGfxMeshFree(meshes.circle);
        meshes.circle = NULL;
    }
    if (meshes.arrow) 
    { AEGfxMeshFree(meshes.arrow);
        meshes.arrow = NULL;
    }

}

int checkCollision(float playerX, float playerY, float radius,
    std::vector<std::vector<int>>& gameMap) {

    // Shrink the collision box slightly inward from all sides
    float epsilon = 30.0f;  // Adjust this value as needed
    float epslion_less = 20.0f;
    // Step 1: Calculate edge points - subtract epsilon from radius
    float leftEdge = playerX - (radius - epsilon);
    float rightEdge = playerX + (radius + epslion_less);
    float topEdge = playerY + (radius - epsilon);
    float bottomEdge = playerY - (radius + epslion_less);

    // Step 2: Convert to grid coordinates
    int leftCol = (int)((leftEdge + 800.0f - 25.0f) / 50.0f);
    int rightCol = (int)((rightEdge + 800.0f - 25.0f) / 50.0f);
    int topRow = (int)((450.0f - topEdge - 25.0f) / 50.0f);
    int bottomRow = (int)((450.0f - bottomEdge - 25.0f) / 50.0f);

    // Step 3: Check boundaries
    if (leftCol < 0 || rightCol >= MAP_WIDTH ||
        topRow < 0 || bottomRow >= MAP_HEIGHT) {
        return 1;
    }

    // Step 4: Check all 4 corners
    if (gameMap[topRow][leftCol] == 1) {
        return 1;
    }
    if (gameMap[topRow][rightCol] == 1 ) {
        return 1;
    }
    if (gameMap[bottomRow][leftCol] == 1) {
        return 1;
    }
    if (gameMap[bottomRow][rightCol] == 1) {
        return 1;
    }

    return 0;
}

int checkCollision(Player& player, float radius,
    std::vector<std::vector<int>>& gameMap) {

    float epsilon = 30.0f, epslion_less = 20.0f;
    float leftEdge = player.x - (radius - epsilon);
    float rightEdge = player.x + (radius + epslion_less);
    float topEdge = player.y + (radius - epsilon);
    float bottomEdge = player.y - (radius + epslion_less);

    int leftCol = (int)((leftEdge + 800.0f - 25.0f) / 50.0f);
    int rightCol = (int)((rightEdge + 800.0f - 25.0f) / 50.0f);
    int topRow = (int)((450.0f - topEdge - 25.0f) / 50.0f);
    int bottomRow = (int)((450.0f - bottomEdge - 25.0f) / 50.0f);

    if (leftCol < 0 || rightCol >= MAP_WIDTH || topRow < 0 || bottomRow >= MAP_HEIGHT)
        return 1;

    if (gameMap[topRow][leftCol] == 1) return 1;
    if (gameMap[topRow][rightCol] == 1) return 1;
    if (gameMap[bottomRow][leftCol] == 1) return 1;
    if (gameMap[bottomRow][rightCol] == 1) return 1;

	//Look for flag interactions in the same area. Player center must cross tile center to trigger
    for (int row = topRow; row <= bottomRow; row++) {
        for (int col = leftCol; col <= rightCol; col++) {
            float tileCenterX = col * 50.0f - 800.0f + 25.0f;
            float tileCenterY = 450.0f - row * 50.0f - 25.0f;

            // Player center must cross tile center to trigger
            bool playerCenterInTile = (player.x >= tileCenterX - 25.0f && player.x <= tileCenterX + 25.0f &&
                player.y >= tileCenterY - 25.0f && player.y <= tileCenterY + 25.0f);

            if (gameMap[row][col] == 1) return 1;

            if (gameMap[row][col] == 4 && !player.holdingFlag && playerCenterInTile) {
                gameMap[row][col] = 5;
                printf("Flag picked up!\n");
                return 4;
            }

            if (gameMap[row][col] == 2 && player.holdingFlag && playerCenterInTile) {
                printf("Flag dropped !");
                return 2;
            }
        }
    }

    return 0;
}



void GetNormalizedTextPosition(float pixelX, float pixelY, float* outNormalizedX, float* outNormalizedY) {

    *outNormalizedX = pixelX / (SCREEN_WIDTH / 2.0f);
    *outNormalizedY = pixelY / (SCREEN_HEIGHT / 2.0f);
}

// Normalizes a 2D vector (x, y) in place to have a length of 1, unless it's the zero vector.
void NormalizeVector(float* x, float* y) {
    float length = sqrtf((*x) * (*x) + (*y) * (*y));
    if (length > 0.0f) {
        *x /= length;
        *y /= length;
    }
}

void ForceShowCursor(bool show) {
    if (show) { while (ShowCursor(true) < 0); }
    else { while (ShowCursor(false) >= 0); }
}