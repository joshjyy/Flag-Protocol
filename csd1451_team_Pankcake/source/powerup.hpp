#pragma once
#include "AEEngine.h"
#include "player.hpp"
#include "map.hpp"
#include "player.hpp"

struct PowerUps {
    bool firewall = false;
    float firewallTimer = 0.0f;
};

extern PowerUps playerPowerUps;

void Activate_Firewall(PowerUps& powerup, AEGfxVertexList* mesh, float dt, Player& player, CameraPos& camera);