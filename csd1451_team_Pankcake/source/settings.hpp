/*!************************************************************************
  \file settings.cpp
  \author
  Ye Min Thwin

  \brief
  Implements the settings screen: Master / Music / SFX volume sliders,
  and save/load to Assets/settings.txt.
**************************************************************************/
#pragma once
#include "AEEngine.h"

// ---------------------------------------------------------------------------
// Volume globals  (defined in settings.cpp)
// ---------------------------------------------------------------------------
extern float gMasterVolume;
extern float gMusicVolume;
extern float gSFXVolume;

// ---------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------
void ApplyVolumes();
void SaveSettings();
void LoadSettings();
GameState UpdateAndRenderSettings(AEGfxVertexList* mesh, s8 font, float dt);