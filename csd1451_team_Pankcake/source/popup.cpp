/*!************************************************************************
  \file popup.cpp
  \author






  \brief
  This source file contains definitions of the stuff related to the popups in the editor.

**************************************************************************/

#include "popup.hpp"
#include "gameConstants.hpp"
#include <string.h>

void Popup_Show(Popup& p, const char* msg, float duration, float r, float g, float b) {
    p.active = true;
    p.timer = duration;
    p.duration = duration;
    p.r = r; p.g = g; p.b = b;
    strcpy_s(p.message, msg);
}

void Popup_Update(Popup& p, float dt) {
    if (!p.active) return;
    p.timer -= dt;
    if (p.timer <= 0.0f)
        p.active = false;
}

void Popup_Draw(Popup& p, AEGfxVertexList* mesh, s8 font) {
    if (!p.active) return;

    // fade out as timer runs out
    float alpha = p.timer / p.duration;

    // dark background box
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(alpha * POPUP_BOX_ALPHA_MULT);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

    AEMtx33 scale, translate, transform;
    AEMtx33Scale(&scale, POPUP_BOX_W, POPUP_BOX_H);
    AEMtx33Trans(&translate, 0.0f, POPUP_BOX_Y);
    AEMtx33Concat(&transform, &translate, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    // red warning text
    float tw, th;
    AEGfxGetPrintSize(font, p.message, 1.0f, &tw, &th);
    AEGfxPrint(font, p.message, -tw / 2, POPUP_TEXT_Y, POPUP_TEXT_SCALE, p.r, p.g, p.b, alpha);
}