/*!************************************************************************
  \file damageNumbers.cpp
  \author
  Ye Min Thwin





  \brief
  This source file contains definitions for the rendering of damage numbers.

**************************************************************************/


#include "damageNumbers.hpp"
#include "gameConstants.hpp"
#include "AEEngine.h"

std::vector<DamageNumber> damageNumbers;

void SpawnDamageNumber(float x, float y, float damage, bool isCritical) {
    DamageNumber dmg{};
    dmg.x = x;
    dmg.y = y;
    dmg.damage = damage;
    dmg.lifetime = DMG_NUM_LIFETIME;
    dmg.velocityY = isCritical ? DMG_NUM_VELOCITY_CRIT : DMG_NUM_VELOCITY_NORMAL;
    dmg.active = true;
    dmg.isCritical = isCritical;

    damageNumbers.push_back(dmg);
}

void UpdateDamageNumbers(float dt) {
    for (auto& dmg : damageNumbers) {
        if (!dmg.active) continue;

        dmg.lifetime -= dt;
        dmg.y += dmg.velocityY * dt;  // float upward

        if (dmg.lifetime <= 0.0f)
            dmg.active = false;
    }

    // Remove inactive numbers
    damageNumbers.erase(
        std::remove_if(damageNumbers.begin(), damageNumbers.end(),
            [](const DamageNumber& d) { return !d.active; }),
        damageNumbers.end()
    );
}

void RenderDamageNumbers(CameraPos& camera) {
    for (const auto& dmg : damageNumbers) {
        if (!dmg.active) continue;

        // Convert to screen space
        float screenX = dmg.x - camera.x;
        float screenY = dmg.y - camera.y;

        // Convert to normalised coordinates for AEGfxPrint
        float textX, textY;
        GetNormalizedTextPosition(screenX + DMG_NUM_TEXT_OFFSET_X, screenY, &textX, &textY);

        // Format text
        char text[16];
        sprintf_s(text, "%.0f", dmg.damage);

        // Alpha fades from 1.0 to 0.0 over the lifetime
        float alpha = dmg.lifetime;

        if (dmg.isCritical)
            AEGfxPrint(fontID, text, textX, textY, DMG_NUM_SCALE_CRIT, 1.0f, 1.0f, 0.0f, alpha); // yellow
        else
            AEGfxPrint(fontID, text, textX, textY, DMG_NUM_SCALE_NORMAL, 1.0f, 1.0f, 1.0f, alpha); // white
    }
}

void ClearDamageNumbers() {
    damageNumbers.clear();
}