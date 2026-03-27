/*!************************************************************************
  \file debris.cpp
  \author






  \brief
  This source file contains definitions for the background particles.

**************************************************************************/

#include "debris.hpp"
#include "AEEngine.h"
#include <random>

#define ASTEROID_COUNT 2000
extern float MAP_WIDTH;
extern float MAP_HEIGHT;
extern float TILE_SIZE;

static AEGfxVertexList* asteroidMesh = nullptr;
static Asteroid asteroids[ASTEROID_COUNT];
static std::mt19937 rng(std::random_device{}());

void Background_Load() {
    AEGfxMeshStart();
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFF003300, 0.0f, 0.0f,
        0.5f, 0.5f, 0xFF00FF00, 0.0f, 0.0f,
        -0.5f, 0.5f, 0xFF00FF00, 0.0f, 0.0f);
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFF003300, 0.0f, 0.0f,
        0.5f, -0.5f, 0xFF003300, 0.0f, 0.0f,
        0.5f, 0.5f, 0xFF00FF00, 0.0f, 0.0f);
    asteroidMesh = AEGfxMeshEnd();
}

void Background_Init() {
    std::uniform_real_distribution<float> randX(-775.0f, -775.0f + (MAP_WIDTH * TILE_SIZE));
    std::uniform_real_distribution<float> randY(425.0f - (MAP_HEIGHT * TILE_SIZE), 425.0f);
    std::uniform_real_distribution<float> randSpeed(30.0f, 100.0f);
    std::uniform_real_distribution<float> randScale(15.0f, 40.0f);

    for (int i = 0; i < ASTEROID_COUNT; i++) {
        asteroids[i].pos.x = randX(rng);
        asteroids[i].pos.y = randY(rng);
        asteroids[i].vel.x = 0.0f;
        asteroids[i].vel.y = -randSpeed(rng);
        asteroids[i].scale = randScale(rng);
    }
}

void Background_Update(float g_dt) {
    std::uniform_real_distribution<float> randX(-775.0f, -775.0f + (MAP_WIDTH * TILE_SIZE));
    std::uniform_real_distribution<float> randSpeed(30.0f, 100.0f);
    std::uniform_real_distribution<float> randScale(15.0f, 40.0f);

    for (int i = 0; i < ASTEROID_COUNT; i++) {
        asteroids[i].pos.y += asteroids[i].vel.y * g_dt;

        if (asteroids[i].pos.y < 425.0f - (MAP_HEIGHT * TILE_SIZE)) {
            std::uniform_real_distribution<float> randResetY(425.0f, 425.0f + 200.0f);
            asteroids[i].pos.x = randX(rng);
            asteroids[i].pos.y = randResetY(rng);  
            asteroids[i].vel.y = -randSpeed(rng);
            asteroids[i].scale = randScale(rng);
        }
    }
}
void Background_Draw(CameraPos camera) {
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0, 0);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    for (int i = 0; i < ASTEROID_COUNT; i++) {
        int col = (int)((asteroids[i].pos.x + 800.0f - 25.0f) / TILE_SIZE);
        int row = (int)((450.0f - asteroids[i].pos.y - 25.0f) / TILE_SIZE);

        if (row < 0 || col < 0 || row >= (int)MAP_HEIGHT || col >= (int)MAP_WIDTH)
            continue;

        if (newMap[row + 1][col + 1] != -1)
            continue;

        AEMtx33 transform;
        AEMtx33Scale(&transform, asteroids[i].scale, asteroids[i].scale);
        AEMtx33TransApply(&transform, &transform,
            asteroids[i].pos.x - camera.x,
            asteroids[i].pos.y - camera.y);
        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw(asteroidMesh, AE_GFX_MDM_TRIANGLES);
    }
}

void Background_Free() {
    AEGfxMeshFree(asteroidMesh);
}