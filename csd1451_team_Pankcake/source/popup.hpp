#pragma once
#include "AEEngine.h"

struct Popup {
    bool active = false;
    float timer = 0.0f;
    float duration = 3.0f;
    float r = 1.0f, g = 1.0f, b = 1.0f;  // ADD
    char message[64] = {};
};

void Popup_Show(Popup& p, const char* msg, float duration = 3.0f, float r = 1.0f, float g = 0.0f, float b = 0.0f);
void Popup_Update(Popup& p, float dt);
void Popup_Draw(Popup& p, AEGfxVertexList* mesh, s8 font);