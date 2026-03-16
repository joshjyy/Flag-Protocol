#pragma once

// Mathematical constants
const float pi = 3.14159265359f;

// Player constants
const float PLAYER_RADIUS = 50.0f;

// Bullet constants
const float BULLET_RADIUS = 25.0f;

// Enemy constants
const float GRUNT_RADIUS = 50.0f;

// Movement constants
const float PLAYER_WALK_SPEED = 300.0f;
const float PLAYER_DASH_SPEED = 2000.0f;
const float DASH_DURATION = 0.1f;
const float DASH_COOLDOWN = 0.5f;
const float ACCELERATION = 10000.0f;
const float FRICTION = 10.0f;

// AI Consts
const float AI_DETECTION_RANGE = 400.0f;
const float AI_ATTACK_RANGE = 300.0f;
const float AI_RETREAT_HEALTH = 30.0f;
const float AI_TOO_CLOSE_RANGE = 150.0f;
const float AI_PATROL_SPEED = 50.0f;
const float AI_CHASE_SPEED = 150.0f;
const float AI_RETREAT_SPEED = 200.0f;
const float AI_STRAFE_SPEED = 80.0f;

//Screen Width and Height
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;


//Tile Types
enum TileType {
    EMPTY = 0,
    WALL,
    GOAL,
    ENEMY_SPAWN,
    FLAG,
    SPAWN,
    VISITED = -1
};

struct CameraPos {
    float x = 0.0f;
    float y = 0.0f;
    float lerpSpeed = 8.0f;
};