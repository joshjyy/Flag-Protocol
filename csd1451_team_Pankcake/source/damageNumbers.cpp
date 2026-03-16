#include "damageNumbers.hpp"
#include "AEEngine.h"

std::vector<DamageNumber> damageNumbers;

void SpawnDamageNumber(float x, float y, float damage, bool isCritical) {
    DamageNumber dmg{};
    dmg.x = x;
    dmg.y = y;
    dmg.damage = damage;
    dmg.lifetime = 1.0f;  // 1 second lifetime
    dmg.velocityY = isCritical ? 100.0f : 60.0f;  // Crits fly faster
    dmg.active = true;
    dmg.isCritical = isCritical;

    damageNumbers.push_back(dmg);
}

void UpdateDamageNumbers(float dt) {
    for (auto& dmg : damageNumbers) {
        if (!dmg.active) continue;

        dmg.lifetime -= dt;
        dmg.y += dmg.velocityY * dt;  // Float upward

        if (dmg.lifetime <= 0.0f) {
            dmg.active = false;
        }
    }

    // Remove inactive numbers
    damageNumbers.erase(
        std::remove_if(damageNumbers.begin(), damageNumbers.end(),
            [](const DamageNumber& d) { return !d.active; }),
        damageNumbers.end()
    );
}

void RenderDamageNumbers(float cameraX, float cameraY) {
    for (const auto& dmg : damageNumbers) {
        if (!dmg.active) continue;

        // Convert to screen space
        float screenX = dmg.x - cameraX;
        float screenY = dmg.y - cameraY;

        // Convert to normalized coordinates for AEGfxPrint
        float textX, textY;
        GetNormalizedTextPosition(screenX - 20.0f, screenY, &textX, &textY);

        // Format text
        char text[16];
        sprintf_s(text, "%.0f", dmg.damage);

        // Fade out based on lifetime
        float alpha = dmg.lifetime;  // 1.0 to 0.0

        // Color based on critical hit
        if (dmg.isCritical) {
            // Critical hits are yellow and larger
            AEGfxPrint(fontID, text, textX, textY, 2.0f, 1.0f, 1.0f, 0.0f, alpha);
        }
        else {
            // Normal hits are white
            AEGfxPrint(fontID, text, textX, textY, 1.5f, 1.0f, 1.0f, 1.0f, alpha);
        }
    }
}

void ClearDamageNumbers() {
    damageNumbers.clear();
}