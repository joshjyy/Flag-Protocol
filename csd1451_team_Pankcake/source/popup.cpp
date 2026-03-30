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

void Popup_Draw(Popup& p, s8 font) {
    if (!p.active) return;

    float alpha = p.timer / p.duration;

    char buf[256];
    strcpy_s(buf, p.message);

    char* next = nullptr;
    char* line = strtok_s(buf, "\n", &next);
    float lineY = POPUP_TEXT_Y;

    while (line != nullptr) {
        float tw, th;
        AEGfxGetPrintSize(font, line, 1.0f, &tw, &th);
        AEGfxPrint(font, line, -tw / 2, lineY, POPUP_TEXT_SCALE, p.r, p.g, p.b, alpha);
        lineY -= th + 0.02f; // move down each line
        line = strtok_s(nullptr, "\n", &next);
    }
}