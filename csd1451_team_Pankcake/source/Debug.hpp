/*!************************************************************************
  \file weapon.hpp
  \author
 




  \brief
  This source file contains declarations of the stuff related to the weapon system.

**************************************************************************/
#pragma once
struct debug {
	bool g_debug_collision = false;
};

extern debug DebugMode;
extern bool showDebug;
enum class DebugDrawType { PLAYER, BULLET, ENEMY };
void DebugDrawAABB(float worldX, float worldY, float halfW, float halfH, CameraPos camera, DebugDrawType type);