/*!************************************************************************
  \file editor.hpp
  \author






  \brief
  This source file contains declarations for the editor game mode.

**************************************************************************/

#pragma once

#include "AEEngine.h"
#include "map.hpp"
#include "utils.hpp"
#include "gameConstants.hpp"
#include "popup.hpp"
extern Popup popup;
struct EditorState {
    int currentTileType     = 1;
    s32 mouseX              = 0;
    s32 mouseY              = 0;
    bool editorPaintReady   = false;
    int flag_count          = 0;
    int enemy_spawner_count = 0;
    int player_spawn_count  = 0;
    int goal_count          = 0;
    CameraPos camera        = {0.0f, 0.0f};
};

EditorState InitEditor(void);

void UpdateAndRenderEditor(EditorState& editor,
    std::vector<std::vector<int>>& gameMap,
    AEGfxVertexList* squareMesh);

void ClearTileType(std::vector<std::vector<int>>& gameMap, int tileType);
// Input handling
void HandleEditorInput(EditorState& editor,
    std::vector<std::vector<int>>& gameMap);

// UI rendering
void RenderEditorUI(const EditorState& editor,
    AEGfxVertexList* squareMesh);

void DrawEditorHover(const EditorState& editor,
    std::vector<std::vector<int>>& gameMap,
    AEGfxVertexList* squareMesh);

void SaveCustomMap(std::vector<std::vector<int>>& gameMap, EditorState& editor);
void LoadCustomMap(std::vector<std::vector<int>>& gameMap);
void SaveInitialEditorMap(std::vector<std::vector<int>>& gameMap);
void ResetToInitialEditorMap(std::vector<std::vector<int>>& gameMap);
void Quit_Editor(EditorState& editor);