
/*!************************************************************************
  \file Debug.cpp
  \author






  \brief
  This source file contains definitions of the stuff related to the weapon system.

**************************************************************************/

#include "AEEngine.h"
#include "utils.hpp"
#include "player.hpp"
#include "Debug.hpp"
#include "gameConstants.hpp"

debug DebugMode;

void DebugDraw(Player& player, float radius, CameraPos camera) {
    if (!DebugMode.g_debug_collision) return;

    float screenX = player.x - camera.x;
    float screenY = player.y - camera.y;

    float L = screenX - (radius - DEBUG_EPSILON);
    float R = screenX + (radius - DEBUG_EPSILON);
    float T = screenY + (radius - DEBUG_EPSILON);
    float B = screenY - (radius - DEBUG_EPSILON);

    float thick = DEBUG_THICKNESS;
    float w = R - L;
    float h = T - B;
    float cx = (L + R) / 2.0f;
    float cy = (T + B) / 2.0f;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0, 0);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);

    DrawSprite(meshes.debug_quad, cx, T, w, thick, 0.0f);
    DrawSprite(meshes.debug_quad, cx, B, w, thick, 0.0f);
    DrawSprite(meshes.debug_quad, L, cy, thick, h, 0.0f);
    DrawSprite(meshes.debug_quad, R, cy, thick, h, 0.0f);
}