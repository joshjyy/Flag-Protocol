#pragma once

// Mathematical constants
const float pi = 3.14159265359f;

// Player constants
const float PLAYER_RADIUS = 50.0f;
const float PLAYER_COLLISION_RADIUS = 25.0f;
const float PLAYER_MAX_HEALTH = 100.0f;
const float PLAYER_LOW_HEALTH_THRESHOLD = 40.0f;
const float PLAYER_HEALTH_RECOVER_THRESHOLD = 90.0f;
const float PLAYER_HEAL_THRESHOLD = 80.0f;
const float PLAYER_HEAL_AMOUNT = 20.0f;
const float PLAYER_SPRINT_SPEED = 600.0f;

// Bullet constants
const float BULLET_RADIUS = 25.0f;

// Enemy constants
const float GRUNT_RADIUS = 50.0f;

// Movement constants
const float PLAYER_WALK_SPEED = 300.0f;
const float PLAYER_DASH_SPEED = 1500.0f;
const float DASH_DURATION = 0.1f;
const float DASH_COOLDOWN = 0.5f;
const float DASH_DAMAGE = 50.0f;
const float DASH_HIT_RADIUS_BONUS = 20.0f;
const float ACCELERATION = 10000.0f;
const float FRICTION = 10.0f;

// Shotgun constants
const int   SHOTGUN_PELLET_COUNT = 5;
const float SHOTGUN_SPREAD_ANGLE = 30.0f;

//Flag indicator constants
const float FLAG_INDICATOR_RADIUS_MULT = 1.8f;
const float FLAG_INDICATOR_SIZE_MULT = 0.5f;
const float FLAG_INDICATOR_SIZE_H_MULT = 0.5f;
const float FLAG_INDICATOR_PULSE_SPEED = 5.0f;
const float FLAG_INDICATOR_PULSE_BASE = 0.8f;
const float FLAG_INDICATOR_PULSE_AMP = 0.2f;
const float FLAG_INDICATOR_TRANSPARENCY = 0.9f;



//Screen Width and Height
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;


enum TileType {
    EMPTY = 0,
    WALL,
    GOAL,
    ENEMY_SPAWN,
    FLAG,
    FLAG_SPAWN,   // rename this to FLAG_SPAWN (was implicitly 5)
    PLAYER_SPAWN, // now = 6
    VISITED = -1
};

struct CameraPos {
    float x = 0.0f;
    float y = 0.0f;
    float lerpSpeed = 8.0f;
};



// AI Consts
const float AI_DETECTION_RANGE = 400.0f;
const float AI_ATTACK_RANGE = 300.0f;
const float AI_RETREAT_HEALTH = 30.0f;
const float AI_TOO_CLOSE_RANGE = 150.0f;
const float AI_PATROL_SPEED = 50.0f;
const float AI_CHASE_SPEED = 150.0f;
const float AI_RETREAT_SPEED = 200.0f;
const float AI_STRAFE_SPEED = 80.0f;


// Tutorial hint UI
const float TUTORIAL_HINT_START_Y = 390.0f;
const float TUTORIAL_HINT_STEP_Y = 30.0f;
const float TUTORIAL_HINT_X = -780.0f;
const float TUTORIAL_HINT_SCALE = 0.8f;
const float TUTORIAL_HINT_SCALE_SMALL = 0.7f;
const float TUTORIAL_HINT_EXTRA_OFFSET = 20.0f;
const int   TUTORIAL_HINT_KEYBIND_COUNT = 4;

// Tutorial debug UI
const float TUTORIAL_DEBUG_X = 600.0f;
const float TUTORIAL_DEBUG_START_Y = 390.0f;
const float TUTORIAL_DEBUG_STEP_Y = 28.0f;


