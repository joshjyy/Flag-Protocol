/*!************************************************************************
  \file damageNumbers.hpp
  \author
  Ye Min Thwin





  \brief
  This source file contains declarations for the rendering of damage numbers.

**************************************************************************/

#pragma once


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

