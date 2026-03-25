#include "AEEngine.h"
#include "powerup.hpp"
#include "utils.hpp"
#include "player.hpp"

PowerUps playerPowerUps;

void Activate_Firewall(PowerUps& powerup, AEGfxVertexList* mesh, float dt, Player& player, CameraPos& camera) {
    if (!powerup.firewall) return;

    float screenX = player.x - camera.x;
    float screenY = player.y - camera.y;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(0.5f);
    AEGfxSetColorToMultiply(0.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);  // zero out add

    AEMtx33 scale, translate, transform;
    AEMtx33Scale(&scale, 100.0f, 100.0f);
    AEMtx33Trans(&translate, screenX, screenY);
    AEMtx33Concat(&transform, &translate, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    // reset
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetTransparency(1.0f);

    powerup.firewallTimer -= dt;
    if (powerup.firewallTimer <= 0.0f) {
        powerup.firewall = false;
        powerup.firewallTimer = 0.0f;
    }
    
}