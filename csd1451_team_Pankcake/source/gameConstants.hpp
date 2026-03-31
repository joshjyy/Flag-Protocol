/*!************************************************************************
  \file gameConstants.hpp
  \author
  Yeo Kah Boon Joshua (Player Gameplay)
  Ye Min Thwin (Enemies, AI, Settings, Damage Numbers)

  \brief
  This header contains extern declarations for all game constants.
  Definitions and default values are in gameConstants.cpp.
**************************************************************************/

#pragma once

void LoadGameConstants(const char* filename);

// Compile-time array sizes (must stay constexpr)
constexpr int MAX_PARTICLES = 30;
constexpr int ASTEROID_COUNT = 2000;

// Array sizes used in screens.cpp
constexpr int MENU_BG_BLOCK_COUNT = 15;
constexpr int CREDITS_P1_NAME_COUNT = 4;
constexpr int CREDITS_P2_INSTRUCTOR_COUNT = 3;
constexpr int CREDITS_P3_THANKS_COUNT = 3;

enum TileType {
    EMPTY = 0,
    WALL,
    GOAL,
    ENEMY_SPAWN,
    FLAG,
    FLAG_SPAWN,
    PLAYER_SPAWN,
    VISITED = -1
};

struct CameraPos {
    float x = 0.0f;
    float y = 0.0f;
    float lerpSpeed = 8.0f;
};

// Math
extern float pi;

// Player
extern float PLAYER_RADIUS;
extern float PLAYER_COLLISION_RADIUS;
extern float PLAYER_MAX_HEALTH;
extern float PLAYER_LOW_HEALTH_THRESHOLD;
extern float PLAYER_HEALTH_RECOVER_THRESHOLD;
extern float PLAYER_HEAL_THRESHOLD;
extern float PLAYER_HEAL_AMOUNT;
extern float PLAYER_SPRINT_SPEED;

// Bullet / Enemy
extern float BULLET_RADIUS;
extern float GRUNT_RADIUS;

// Movement
extern float PLAYER_WALK_SPEED;
extern float PLAYER_DASH_SPEED;
extern float DASH_DURATION;
extern float DASH_COOLDOWN;
extern float DASH_DAMAGE;
extern float DASH_HIT_RADIUS_BONUS;
extern float ACCELERATION;
extern float FRICTION;

// Shotgun
extern int   SHOTGUN_PELLET_COUNT;
extern float SHOTGUN_SPREAD_ANGLE;

// Kill rewards
extern int   KILL_SCORE;
extern float KILL_HEAL_AMOUNT;
extern float KILL_HEAL_FULL_THRESHOLD;
extern int   KILL_AMMO_PISTOL;
extern int   KILL_AMMO_RIFLE;
extern int   KILL_AMMO_SHOTGUN;

// Damage numbers
extern float DMG_NUM_LIFETIME;
extern float DMG_NUM_VELOCITY_NORMAL;
extern float DMG_NUM_VELOCITY_CRIT;
extern float DMG_NUM_TEXT_OFFSET_X;
extern float DMG_NUM_SCALE_NORMAL;
extern float DMG_NUM_SCALE_CRIT;

// Pistol lifesteal
extern float PISTOL_LIFESTEAL_AMOUNT;
extern float PISTOL_LIFESTEAL_THRESHOLD;

// Flag indicator
extern float FLAG_INDICATOR_RADIUS_MULT;
extern float FLAG_INDICATOR_SIZE_MULT;
extern float FLAG_INDICATOR_SIZE_H_MULT;
extern float FLAG_INDICATOR_PULSE_SPEED;
extern float FLAG_INDICATOR_PULSE_BASE;
extern float FLAG_INDICATOR_PULSE_AMP;
extern float FLAG_INDICATOR_TRANSPARENCY;

// Screen
extern int   SCREEN_WIDTH;
extern int   SCREEN_HEIGHT;

// AI
extern float AI_DETECTION_RANGE;
extern float AI_ATTACK_RANGE;
extern float AI_RETREAT_HEALTH;
extern float AI_TOO_CLOSE_RANGE;
extern float AI_PATROL_SPEED;
extern float AI_CHASE_SPEED;
extern float AI_RETREAT_SPEED;
extern float AI_STRAFE_SPEED;
extern float AI_INVESTIGATE_ARRIVE_DIST;
extern float AI_INVESTIGATE_SPEED;
extern float AI_INVESTIGATE_PERP_MULT;
extern float AI_INVESTIGATE_MAX_TIME;
extern float AI_WANDER_SPEED;
extern float AI_WANDER_ANGLE_RATE;
extern float AI_WANDER_WALL_BOUNCE_ANGLE;
extern float AI_CHASE_ARRIVE_DIST;
extern float AI_CHASE_PERP_MULT;
extern float AI_SHOOT_COOLDOWN;
extern float AI_STRAFE_OSCILLATE_SPEED;
extern float AI_RETREAT_PERP_MULT;

// Grunt spawner
extern int   GRUNT_MAX_COUNT;
extern float GRUNT_WAVE_INTERVAL;
extern int   GRUNT_SPAWN_SKIP_CHANCE;
extern int   GRUNT_MAX_PER_SPAWNER;
extern int   GRUNT_SPAWN_ATTEMPTS;
extern float GRUNT_SPAWN_RADIUS_TILES;
extern float GRUNT_CULL_TIME;

// Wave timer UI
extern float WAVE_TIMER_POS_X;
extern float WAVE_TIMER_POS_Y;
extern float WAVE_TIMER_SCALE;
extern float WAVE_TIMER_PANEL_W;
extern float WAVE_TIMER_PANEL_H;
extern float WAVE_TIMER_PANEL_ALPHA;
extern float WAVE_TIMER_YELLOW_THRESHOLD;
extern float WAVE_TIMER_RED_THRESHOLD;
extern float WAVE_TIMER_RED_G_VALUE;

// Tutorial hint UI
extern float TUTORIAL_HINT_START_Y;
extern float TUTORIAL_HINT_STEP_Y;
extern float TUTORIAL_HINT_X;
extern float TUTORIAL_HINT_SCALE;
extern float TUTORIAL_HINT_SCALE_SMALL;
extern float TUTORIAL_HINT_EXTRA_OFFSET;
extern int   TUTORIAL_HINT_KEYBIND_COUNT;

// Tutorial debug UI
extern float TUTORIAL_DEBUG_X;
extern float TUTORIAL_DEBUG_START_Y;
extern float TUTORIAL_DEBUG_STEP_Y;

// Settings slider
extern float SETTINGS_SLIDER_TRACK_H;
extern float SETTINGS_SLIDER_KNOB_R;
extern float SETTINGS_SLIDER_GAP;
extern float SETTINGS_SLIDER_TRACK_W;
extern float SETTINGS_SLIDER_TRACK_X;
extern float SETTINGS_SLIDER_Y_MASTER;
extern float SETTINGS_SLIDER_Y_MUSIC;
extern float SETTINGS_SLIDER_Y_SFX;

// Settings panel
extern float SETTINGS_PANEL_W;
extern float SETTINGS_PANEL_H;
extern float SETTINGS_ACCENT_Y;
extern float SETTINGS_TITLE_Y;
extern float SETTINGS_TITLE_SCALE;
extern float SETTINGS_ACCENT_H;
extern float SETTINGS_DIVIDER_H;
extern float SETTINGS_DIVIDER_Y;
extern float SETTINGS_DIVIDER_W;
extern float SETTINGS_BACK_BTN_Y;
extern float SETTINGS_BACK_BTN_W;
extern float SETTINGS_BACK_BTN_H;
extern float SETTINGS_DEFAULT_MASTER;
extern float SETTINGS_DEFAULT_MUSIC;
extern float SETTINGS_DEFAULT_SFX;

// Splash screen
extern float SPLASH_FADE_IN_TIME;
extern float SPLASH_VISIBLE_TIME;
extern float SPLASH_FADE_OUT_TIME;
extern float SPLASH_LOGO_W;
extern float SPLASH_LOGO_H;

// Menu
extern float MENU_BLOCK_MIN_SIZE;
extern int   MENU_BLOCK_SIZE_RAND;
extern float MENU_BLOCK_MIN_SPEED;
extern int   MENU_BLOCK_SPEED_RAND;
extern float MENU_BLOCK_START_X_OFFSET;
extern float MENU_PULSE_BASE;
extern float MENU_PULSE_AMP;
extern float MENU_PULSE_FREQ;

// Scoreboard
extern float SCOREBOARD_BTN_X;
extern float SCOREBOARD_BTN_Y;
extern float SCOREBOARD_BTN_W;
extern float SCOREBOARD_BTN_H;
extern float SCOREBOARD_POP_X;
extern float SCOREBOARD_POP_Y;
extern float SCOREBOARD_POP_W;
extern float SCOREBOARD_POP_H;
extern int   SCOREBOARD_ENTRY_COUNT;
extern float SCOREBOARD_ENTRY_STEP_Y;

// Confirm window
extern float CONFIRM_OVERLAY_ALPHA;
extern float CONFIRM_SHADOW_ALPHA;
extern float CONFIRM_SHADOW_OFFSET;
extern float CONFIRM_SHADOW_W;
extern float CONFIRM_SHADOW_H;
extern float CONFIRM_PANEL_ALPHA;
extern float CONFIRM_PANEL_W;
extern float CONFIRM_PANEL_H;
extern float CONFIRM_HEADER_Y;
extern float CONFIRM_HEADER_W;
extern float CONFIRM_HEADER_H;
extern float CONFIRM_TITLE_SCALE;
extern float CONFIRM_DESC_SCALE;
extern float CONFIRM_DESC_RENDER_SCALE;
extern float CONFIRM_BTN_YES_X;
extern float CONFIRM_BTN_NO_X;
extern float CONFIRM_BTN_Y;
extern float CONFIRM_BTN_W;
extern float CONFIRM_BTN_H;

// Pause
extern float PAUSE_OVERLAY_ALPHA;
extern float PAUSE_TITLE_SCALE;
extern float PAUSE_TITLE_Y;

// End screen
extern float ENDSCREEN_TITLE_SCALE;
extern float ENDSCREEN_TITLE_Y;
extern float ENDSCREEN_STATS_SCALE;
extern float ENDSCREEN_STAT_SCORE_Y;
extern float ENDSCREEN_STAT_GRUNTS_Y;
extern float ENDSCREEN_STAT_FLAGS_Y;
extern float ENDSCREEN_BTN_W;
extern float ENDSCREEN_BTN_H;

// Win screen
extern float WIN_BG_R;
extern float WIN_BG_G;
extern float WIN_BG_B;
extern int   WIN_NAME_INPUT_MAX;
extern float WIN_PROMPT_SCALE;
extern float WIN_PROMPT_Y;
extern float WIN_NAME_DISPLAY_SCALE;
extern float WIN_NAME_DISPLAY_Y;
extern float WIN_SAVE_BTN_X;
extern float WIN_SAVE_BTN_Y;
extern float WIN_TOPSCORE_SCALE;
extern float WIN_TOPSCORE_Y;
extern float WIN_ENTRY_SCALE;
extern float WIN_ENTRY_START_Y;
extern float WIN_ENTRY_STEP_Y;
extern float WIN_NAV_BTN_Y_MAIN;
extern float WIN_NAV_BTN_Y_DEFAULT;
extern float WIN_NAV_BTN_LEFT_X;
extern float WIN_NAV_BTN_RIGHT_X;

// Lose screen
extern float LOSE_BG_R;
extern float LOSE_BG_G;
extern float LOSE_BG_B;
extern float LOSE_RETRY_BTN_X;
extern float LOSE_RETRY_BTN_Y;
extern float LOSE_MENU_BTN_X;
extern float LOSE_MENU_BTN_Y;

// Credits
extern int   CREDITS_TOTAL_PAGES;
extern float CREDITS_TITLE_SCALE;
extern float CREDITS_TITLE_Y;
extern float CREDITS_HEADER_SCALE;
extern float CREDITS_BODY_SCALE;
extern float CREDITS_BODY_SCALE_MED;
extern float CREDITS_BODY_SCALE_SM;
extern float CREDITS_PAGE_IND_SCALE;
extern float CREDITS_PAGE_IND_Y;
extern float CREDITS_PREV_BTN_X;
extern float CREDITS_BACK_BTN_X;
extern float CREDITS_NEXT_BTN_X;
extern float CREDITS_NAV_BTN_Y;
extern float CREDITS_PREV_NEXT_BTN_W;
extern float CREDITS_BACK_BTN_W;
extern float CREDITS_NAV_BTN_H;
extern float CREDITS_P0_LINE1_Y;
extern float CREDITS_P0_LINE2_Y;
extern float CREDITS_P0_LINE3_Y;
extern float CREDITS_P1_HEADER_Y;
extern float CREDITS_P1_NAMES_START_Y;
extern float CREDITS_P1_NAMES_STEP_Y;
extern float CREDITS_P2_HEADER_Y;
extern float CREDITS_P2_PRESIDENT_HDR_Y;
extern float CREDITS_P2_PRESIDENT_NAME_Y;
extern float CREDITS_P2_INSTRUCTOR_HDR_Y;
extern float CREDITS_P2_INSTRUCTOR_START_Y;
extern float CREDITS_P2_INSTRUCTOR_STEP_Y;
extern float CREDITS_P3_HEADER_Y;
extern float CREDITS_P3_THANKS_START_Y;
extern float CREDITS_P3_THANKS_STEP_Y;

// Map
extern float MAP_DEFAULT_TILE_SIZE;
extern float MAP_WORLD_SIZE_TILES;
extern float MAP_TILE_OFFSET;
extern float MAP_CAMERA_PADDING_X;
extern float MAP_CAMERA_PADDING_Y;
extern float MAP_CAMERA_CLAMP_BUFFER;
extern float MAP_ZOOM_MAX;
extern float MAP_ZOOM_MIN;

// Popup
extern float POPUP_BOX_W;
extern float POPUP_BOX_H;
extern float POPUP_BOX_Y;
extern float POPUP_BOX_ALPHA_MULT;
extern float POPUP_TEXT_Y;
extern float POPUP_TEXT_SCALE;

// Debug
extern float DEBUG_EPSILON;
extern float DEBUG_THICKNESS;

// Debris
extern float ASTEROID_SPEED_MIN;
extern float ASTEROID_SPEED_MAX;
extern float ASTEROID_SCALE_MIN;
extern float ASTEROID_SCALE_MAX;
extern float ASTEROID_WORLD_MIN_X;
extern float ASTEROID_WORLD_MAX_Y;
extern float ASTEROID_RESET_Y_MAX;

// Collision
extern float COLLISION_EPSILON;
extern float COLLISION_EPSILON_LESS;
extern float COLLISION_WORLD_OFFSET_X;
extern float COLLISION_WORLD_OFFSET_Y;
extern float COLLISION_TILE_OFFSET;
extern float COLLISION_TILE_SIZE;

// Circle mesh
extern int   CIRCLE_MESH_SEGMENTS;