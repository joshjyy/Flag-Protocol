#include "debris.hpp"
#include "AEEngine.h"


#define ASTEROID_COUNT 100
extern float MAP_WIDTH;
extern float MAP_HEIGHT;
extern float TILE_SIZE;

static AEGfxVertexList* asteroidMesh = nullptr;
static Asteroid asteroids[ASTEROID_COUNT];

void Background_Load() {
    AEGfxMeshStart();
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFF00FF80, 0.0f, 0.0f,
        0.5f, 0.5f, 0xFF00FF80, 0.0f, 0.0f,
        -0.5f, 0.5f, 0xFF00FF80, 0.0f, 0.0f);
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFF00FF80, 0.0f, 0.0f,
        0.5f, -0.5f, 0xFF00FF80, 0.0f, 0.0f,
        0.5f, 0.5f, 0xFF00FF80, 0.0f, 0.0f);
    asteroidMesh = AEGfxMeshEnd();
}

void Background_Init() {
    for (int i = 0; i < ASTEROID_COUNT; i++) {
        asteroids[i].pos.x = AERandFloat() * MAP_WIDTH * TILE_SIZE;
        asteroids[i].pos.y = -(AERandFloat() * MAP_HEIGHT * TILE_SIZE);
        asteroids[i].vel.x = (AERandFloat() - 0.5f) * 100.0f;
        asteroids[i].vel.y = (AERandFloat() - 0.5f) * 100.0f;
        asteroids[i].scale = 80.0f + AERandFloat() * 120.0f;
    }
}

void Background_Update(float g_dt) {
    for (int i = 0; i < ASTEROID_COUNT; i++) {
        asteroids[i].pos.x += asteroids[i].vel.x * g_dt;
        asteroids[i].pos.y += asteroids[i].vel.y * g_dt;

        // wrap around full map
        asteroids[i].pos.x = AEWrap(asteroids[i].pos.x, 0.0f, MAP_WIDTH * TILE_SIZE);
        asteroids[i].pos.y = AEWrap(asteroids[i].pos.y, -(MAP_HEIGHT * TILE_SIZE), 0.0f);
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