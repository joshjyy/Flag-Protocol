#ifndef DAMAGE_NUMBERS_HPP
#define DAMAGE_NUMBERS_HPP

#include "utils.hpp"
#include <vector>

struct DamageNumber {
    float x;
    float y;
    float damage;
    float lifetime;
    float velocityY;
    bool active;
    bool isCritical;  // For headshots/crits
};

// Damage number management
void SpawnDamageNumber(float x, float y, float damage, bool isCritical = false);
void UpdateDamageNumbers(float dt);
void RenderDamageNumbers(CameraPos&  camera);
void ClearDamageNumbers();

// Global damage number storage
extern std::vector<DamageNumber> damageNumbers;

#endif