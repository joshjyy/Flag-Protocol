#include "popup.hpp"
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
    AEGfxSetTransparency(alpha * 0.8f);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

    AEMtx33 scale, translate, transform;
    AEMtx33Scale(&scale, 400.0f, 60.0f);
    AEMtx33Trans(&translate, 0.0f, -300.0f);
    AEMtx33Concat(&transform, &translate, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    // red warning text
    float tw, th;
    AEGfxGetPrintSize(font, p.message, 1.0f, &tw, &th);
    AEGfxPrint(font, p.message, -tw / 2, -0.7f, 1.0f, p.r, p.g, p.b, alpha);
}