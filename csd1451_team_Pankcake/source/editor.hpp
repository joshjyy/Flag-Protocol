#pragma once

#include "AEEngine.h"
#include "map.hpp"
#include "utils.hpp"

struct EditorState {
    int currentTileType;    
    s32 mouseX;
    s32 mouseY;            
    bool editorPaintReady;  
    CameraPos camera;        
};

EditorState InitEditor(void);

void UpdateAndRenderEditor(EditorState& editor,
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap,
    AEGfxVertexList* squareMesh);

// Input handling
void HandleEditorInput(EditorState& editor,
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);

// UI rendering
void RenderEditorUI(const EditorState& editor,
    AEGfxVertexList* squareMesh);

void DrawEditorHover(const EditorState& editor,
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap,
    AEGfxVertexList* squareMesh);

void SaveCustomMap(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void LoadCustomMap(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void ReloadInitialMap(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void SaveInitialEditorMap(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void ResetToInitialEditorMap(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
