#pragma once

#include "AEEngine.h"
#include "map.hpp"
#include "utils.hpp"
#include "gameConstants.hpp"

struct EditorState {
    int currentTileType;    
    s32 mouseX;
    s32 mouseY;            
    bool editorPaintReady;  
    CameraPos camera;        
};

EditorState InitEditor(void);

void UpdateAndRenderEditor(EditorState& editor,
    std::vector<std::vector<int>>& gameMap,
    AEGfxVertexList* squareMesh);

// Input handling
void HandleEditorInput(EditorState& editor,
    std::vector<std::vector<int>>& gameMap);

// UI rendering
void RenderEditorUI(const EditorState& editor,
    AEGfxVertexList* squareMesh);

void DrawEditorHover(const EditorState& editor,
    std::vector<std::vector<int>>& gameMap,
    AEGfxVertexList* squareMesh);

void SaveCustomMap(std::vector<std::vector<int>>& gameMap);
void LoadCustomMap(std::vector<std::vector<int>>& gameMap);
void SaveInitialEditorMap(std::vector<std::vector<int>>& gameMap);
void ResetToInitialEditorMap(std::vector<std::vector<int>>& gameMap);
void Quit_Editor(EditorState& editor);