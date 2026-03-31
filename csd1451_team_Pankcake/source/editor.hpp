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
    int currentTileType;    
    s32 mouseX;
    s32 mouseY;            
    bool editorPaintReady;  
    int flag_count;
    int enemy_spawner_count;
    int player_spawn_count;
    int goal_count;
    CameraPos camera;        
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