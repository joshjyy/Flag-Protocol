/*!************************************************************************
  \file popup.cpp
  \author Zhang Yida
  \brief
  This source file implements a lightweight popup notification system for
  the map editor. It provides functions to activate a popup with a custom
  message, colour, and display duration; tick it down each frame; and render
  it centred on screen with per-line text layout and fade-out alpha based on
  remaining time.
**************************************************************************/

#include "popup.hpp"
#include "gameConstants.hpp"
#include <string.h>

/*!************************************************************************
  \brief
    Activates a popup by setting its message, display duration, colour, and
    marking it as active. Any previously showing popup is immediately
    replaced.

  \param p
    The Popup instance to activate.

  \param msg
    Null-terminated string to display. Supports newline characters for
    multi-line messages.

  \param duration
    Total time in seconds the popup remains visible before fading out.

  \param r
    Red channel of the text colour (0.0 – 1.0).

  \param g
    Green channel of the text colour (0.0 – 1.0).

  \param b
    Blue channel of the text colour (0.0 – 1.0).
**************************************************************************/
void Popup_Show(Popup& p, const char* msg, float duration, float r, float g, float b) {
    p.active = true;
    p.timer = duration;
    p.duration = duration;
    p.r = r; p.g = g; p.b = b;
    strcpy_s(p.message, msg);
}

/*!************************************************************************
  \brief
    Advances the popup timer by one frame. When the timer reaches zero the
    popup is deactivated. Has no effect if the popup is already inactive.

  \param p
    The Popup instance to update.

  \param dt
    Delta time in seconds for the current frame.
**************************************************************************/
void Popup_Update(Popup& p, float dt) {
    if (!p.active) return;
    p.timer -= dt;
    if (p.timer <= 0.0f)
        p.active = false;
}

/*!************************************************************************
  \brief
    Renders the popup message centred on screen. The message is split on
    newline characters and each line is printed separately, advancing the
    Y position downward after every line. Alpha is derived from the ratio
    of remaining time to total duration, producing a smooth fade-out effect.
    Has no effect if the popup is inactive.

  \param p
    The Popup instance to draw.

  \param font
    Font handle passed to AEGfxPrint for rendering the text.
**************************************************************************/
void Popup_Draw(Popup& p, s8 font) {
    if (!p.active) return;

    /* Compute alpha as a 0–1 fade based on remaining time. */
    float alpha = p.timer / p.duration;

    /* Work on a local copy so strtok_s does not mutate the stored message. */
    char buf[256];
    strcpy_s(buf, p.message);

    char* next = nullptr;
    char* line = strtok_s(buf, "\n", &next);
    float lineY = POPUP_TEXT_Y;

    /* Print each newline-delimited line, centred horizontally. */
    while (line != nullptr) {
        float tw, th;
        AEGfxGetPrintSize(font, line, 1.0f, &tw, &th);
        AEGfxPrint(font, line, -tw / 2, lineY, POPUP_TEXT_SCALE, p.r, p.g, p.b, alpha);
        lineY -= th + 0.02f; /* Advance downward by line height plus a small gap. */
        line = strtok_s(nullptr, "\n", &next);
    }
}