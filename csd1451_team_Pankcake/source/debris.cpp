#include "debris.hpp"
#include "AEEngine.h"


#define ASTEROID_COUNT 10

static AEGfxVertexList* asteroidMesh = nullptr;
static Asteroid asteroids[ASTEROID_COUNT];

void Background_Load() {
    AEGfxMeshStart();
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFF808080, 0.0f, 0.0f,
        0.5f, 0.5f, 0xFF808080, 0.0f, 0.0f,
        -0.5f, 0.5f, 0xFF808080, 0.0f, 0.0f);
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFF808080, 0.0f, 0.0f,
        0.5f, -0.5f, 0xFF808080, 0.0f, 0.0f,
        0.5f, 0.5f, 0xFF808080, 0.0f, 0.0f);
    asteroidMesh = AEGfxMeshEnd();
}

void Background_Init() {
    for (int i = 0; i < ASTEROID_COUNT; i++) {
        asteroids[i].pos.x = AERandFloat() * (AEGfxGetWinMaxX() - AEGfxGetWinMinX()) + AEGfxGetWinMinX();
        asteroids[i].pos.y = AERandFloat() * (AEGfxGetWinMaxY() - AEGfxGetWinMinY()) + AEGfxGetWinMinY();
        asteroids[i].vel.x = (AERandFloat() - 0.5f) * 100.0f;
        asteroids[i].vel.y = (AERandFloat() - 0.5f) * 100.0f;
        asteroids[i].scale = 80.0f + AERandFloat() * 5.0f;    
    }
}

void Background_Update(float g_dt   ) {
    for (int i = 0; i < ASTEROID_COUNT; i++) {
        asteroids[i].pos.x += asteroids[i].vel.x * (float)g_dt;
        asteroids[i].pos.y += asteroids[i].vel.y * (float)g_dt;

        // wrap around screen
        asteroids[i].pos.x = AEWrap(asteroids[i].pos.x, AEGfxGetWinMinX() - 50.0f, AEGfxGetWinMaxX() + 50.0f);
        asteroids[i].pos.y = AEWrap(asteroids[i].pos.y, AEGfxGetWinMinY() - 50.0f, AEGfxGetWinMaxY() + 50.0f);
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