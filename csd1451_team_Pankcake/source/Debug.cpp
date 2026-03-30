
/*!************************************************************************
  \file Debug.cpp
  \author






  \brief
  This source file contains definitions of the stuff related to the weapon system.

**************************************************************************/

#include "AEEngine.h"
#include "utils.hpp"
#include "player.hpp"
#include "debug.hpp"

#include "gameConstants.hpp"

debug DebugMode;

void DebugDrawAABB(float worldX, float worldY, float halfW, float halfH, CameraPos camera, DebugDrawType type) {
    if (!DebugMode.g_debug_collision) return;

    float screenX = worldX - camera.x;
    float screenY = worldY - camera.y;

    float L = screenX - (halfW - DEBUG_EPSILON);
    float R = screenX + (halfW - DEBUG_EPSILON);
    float T = screenY + (halfH - DEBUG_EPSILON);
    float B = screenY - (halfH - DEBUG_EPSILON);

    float thick = DEBUG_THICKNESS;
    float w = R - L;
    float h = T - B;
    float cx = (L + R) / 2.0f;
    float cy = (T + B) / 2.0f;

    switch (type) {
    case DebugDrawType::PLAYER: AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f); break;
    case DebugDrawType::BULLET: AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f); break;
    case DebugDrawType::ENEMY: AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f); break;
    }

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0, 0);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    DrawSprite(meshes.debug_quad, cx, T, w, thick, 0.0f);
    DrawSprite(meshes.debug_quad, cx, B, w, thick, 0.0f);
    DrawSprite(meshes.debug_quad, L, cy, thick, h, 0.0f);
    DrawSprite(meshes.debug_quad, R, cy, thick, h, 0.0f);

}