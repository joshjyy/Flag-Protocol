/*!************************************************************************
  \file gameConstants.hpp
  \author
  Yeo Kah Boon Joshua (Player Gameplay )
  Ye Min Thwin (Enemies, AI, Settings, Damage Numbers)





  \brief
  This source file contains declarations for the game variables.

**************************************************************************/

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

// Kill rewards  
const int   KILL_SCORE = 10;     
const float KILL_HEAL_AMOUNT = 15.0f;  
const float KILL_HEAL_FULL_THRESHOLD = 85.0f;  
const int   KILL_AMMO_PISTOL = 5;      
const int   KILL_AMMO_RIFLE = 10;     
const int   KILL_AMMO_SHOTGUN = 2;      

// Damage numbers
const float DMG_NUM_LIFETIME = 1.0f;    // seconds before fading out
const float DMG_NUM_VELOCITY_NORMAL = 60.0f;   // float-up speed for normal hits
const float DMG_NUM_VELOCITY_CRIT = 100.0f;  // float-up speed for critical hits
const float DMG_NUM_TEXT_OFFSET_X = -20.0f;  // horizontal nudge from world position
const float DMG_NUM_SCALE_NORMAL = 1.5f;    // text scale for normal hits
const float DMG_NUM_SCALE_CRIT = 2.0f;    // text scale for critical hits

// Pistol lifesteal on hit (weapon index 0)
const float PISTOL_LIFESTEAL_AMOUNT = 20.0f; // HP gained per hit
const float PISTOL_LIFESTEAL_THRESHOLD = 80.0f; // if above this, heal to full instead

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
const float AI_INVESTIGATE_ARRIVE_DIST = 30.0f;   // how close counts as "reached" last-seen pos
const float AI_INVESTIGATE_SPEED = 100.0f;  // move speed while investigating
const float AI_INVESTIGATE_PERP_MULT = 0.8f;
const float AI_INVESTIGATE_MAX_TIME = 5.0f;    // give up investigating after this many seconds
const float AI_WANDER_SPEED = 30.0f;   // idle wander speed
const float AI_WANDER_ANGLE_RATE = 0.5f;
const float AI_WANDER_WALL_BOUNCE_ANGLE = 1.5707963f;
const float AI_CHASE_ARRIVE_DIST = 10.0f;  // stop chasing closer than this
const float AI_CHASE_PERP_MULT = 0.5f;
const float AI_SHOOT_COOLDOWN = 2.0f;   // seconds between shots
const float AI_STRAFE_OSCILLATE_SPEED = 2.0f;
const float AI_RETREAT_PERP_MULT = 0.7f;   // sideways speed multiplier when hitting wall


// Grunt constants
const int   GRUNT_MAX_COUNT = 25;     // max live grunts at once
const float GRUNT_WAVE_INTERVAL = 5.0f;   // seconds between spawn waves
const int   GRUNT_SPAWN_SKIP_CHANCE = 3;      // 1/x chance to skip a spawner tile each wave
const int   GRUNT_MAX_PER_SPAWNER = 3;      // max new grunts per spawner tile per wave
const int   GRUNT_SPAWN_ATTEMPTS = 10;     // max random position attempts before giving up
const float GRUNT_SPAWN_RADIUS_TILES = 3.0f;   // spawn search radius in tiles
const float GRUNT_CULL_TIME = 30.0f;  // seconds before an unengaged patrolling grunt is culled

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

//Settings Constants
const float SETTINGS_SLIDER_TRACK_H = 8.0f;    // height of the track bar
const float SETTINGS_SLIDER_KNOB_R = 13.0f;   // half-size of the knob square
const float SETTINGS_SLIDER_GAP = 18.0f;   // gap between label/pct text and track ends
const float SETTINGS_SLIDER_TRACK_W = 400.0f;  // total track width in pixels
const float SETTINGS_SLIDER_TRACK_X = 0.0f;    // track centre X (0 = screen centre)

// Slider centre Y positions for each channel
const float SETTINGS_SLIDER_Y_MASTER = 100.0f;
const float SETTINGS_SLIDER_Y_MUSIC = -30.0f;
const float SETTINGS_SLIDER_Y_SFX = -160.0f;

// Settings panel layout
const float SETTINGS_PANEL_W = 680.0f;
const float SETTINGS_PANEL_H = 600.0f;
const float SETTINGS_ACCENT_Y = 275.0f;  // Y of the green accent line at top
const float SETTINGS_DIVIDER_Y = -220.0f;
const float SETTINGS_DIVIDER_W = 600.0f;
const float SETTINGS_BACK_BTN_Y = -270.0f;
const float SETTINGS_BACK_BTN_W = 200.0f;
const float SETTINGS_BACK_BTN_H = 50.0f;

// Default volume levels (all 1.0 = 100%)
const float SETTINGS_DEFAULT_MASTER = 1.0f;
const float SETTINGS_DEFAULT_MUSIC = 1.0f;
const float SETTINGS_DEFAULT_SFX = 1.0f;





//Screens Constants

// Splash screen
const float SPLASH_FADE_IN_TIME = 0.5f;
const float SPLASH_VISIBLE_TIME = 2.0f;
const float SPLASH_FADE_OUT_TIME = 0.5f;
const float SPLASH_LOGO_W = 900.0f;
const float SPLASH_LOGO_H = 260.0f;

// Menu screen background blocks
const int   MENU_BG_BLOCK_COUNT = 15;
const float MENU_BLOCK_MIN_SIZE = 18.0f;
const int   MENU_BLOCK_SIZE_RAND = 45;
const float MENU_BLOCK_MIN_SPEED = 450.0f;
const int   MENU_BLOCK_SPEED_RAND = 200;
const float MENU_BLOCK_START_X_OFFSET = 120.0f;

// Menu title pulse
const float MENU_PULSE_BASE = 2.5f;
const float MENU_PULSE_AMP = 0.2f;
const float MENU_PULSE_FREQ = 4.0f;

// Scoreboard popup
const float SCOREBOARD_BTN_X = 650.0f;
const float SCOREBOARD_BTN_Y = 380.0f;
const float SCOREBOARD_BTN_W = 160.0f;
const float SCOREBOARD_BTN_H = 45.0f;
const float SCOREBOARD_POP_X = 650.0f;
const float SCOREBOARD_POP_Y = 150.0f;
const float SCOREBOARD_POP_W = 200.0f;
const float SCOREBOARD_POP_H = 320.0f;
const int   SCOREBOARD_ENTRY_COUNT = 3;
const float SCOREBOARD_ENTRY_STEP_Y = 60.0f;

// Confirm window
const float CONFIRM_OVERLAY_ALPHA = 0.88f;
const float CONFIRM_SHADOW_ALPHA = 0.55f;
const float CONFIRM_SHADOW_OFFSET = 8.0f;
const float CONFIRM_SHADOW_W = 500.0f;
const float CONFIRM_SHADOW_H = 210.0f;
const float CONFIRM_PANEL_ALPHA = 0.96f;
const float CONFIRM_PANEL_W = 560.0f;
const float CONFIRM_PANEL_H = 250.0f;
const float CONFIRM_HEADER_Y = 78.0f;
const float CONFIRM_HEADER_W = 560.0f;
const float CONFIRM_HEADER_H = 34.0f;
const float CONFIRM_TITLE_SCALE = 1.3f;
const float CONFIRM_DESC_SCALE = 0.82f;
const float CONFIRM_DESC_RENDER_SCALE = 0.98f;
const float CONFIRM_BTN_YES_X = -110.0f;
const float CONFIRM_BTN_NO_X = 110.0f;
const float CONFIRM_BTN_Y = -55.0f;
const float CONFIRM_BTN_W = 140.0f;
const float CONFIRM_BTN_H = 45.0f;

// Pause screen
const float PAUSE_OVERLAY_ALPHA = 0.6f;
const float PAUSE_TITLE_SCALE = 2.0f;
const float PAUSE_TITLE_Y = 0.65f;



// Win/Lose shared end screen
const float ENDSCREEN_TITLE_SCALE = 2.5f;
const float ENDSCREEN_TITLE_Y = 0.75f;
const float ENDSCREEN_STATS_SCALE = 1.2f;
const float ENDSCREEN_STAT_SCORE_Y = 0.35f;
const float ENDSCREEN_STAT_GRUNTS_Y = 0.15f;
const float ENDSCREEN_STAT_FLAGS_Y = -0.05f;
const float ENDSCREEN_BTN_W = 200.0f;
const float ENDSCREEN_BTN_H = 50.0f;

// Win screen
const float WIN_BG_R = 0.05f;
const float WIN_BG_G = 0.05f;
const float WIN_BG_B = 0.1f;
const int   WIN_NAME_INPUT_MAX = 12;
const float WIN_PROMPT_SCALE = 1.0f;
const float WIN_PROMPT_Y = -0.22f;
const float WIN_NAME_DISPLAY_SCALE = 1.2f;
const float WIN_NAME_DISPLAY_Y = -0.36f;
const float WIN_SAVE_BTN_X = 0.0f;
const float WIN_SAVE_BTN_Y = -290.0f;
const float WIN_TOPSCORE_SCALE = 1.0f;
const float WIN_TOPSCORE_Y = -0.22f;
const float WIN_ENTRY_SCALE = 0.9f;
const float WIN_ENTRY_START_Y = -0.34f;
const float WIN_ENTRY_STEP_Y = 0.12f;
const float WIN_NAV_BTN_Y_MAIN = -390.0f;
const float WIN_NAV_BTN_Y_DEFAULT = -250.0f;
const float WIN_NAV_BTN_LEFT_X = -150.0f;
const float WIN_NAV_BTN_RIGHT_X = 150.0f;

// Lose screen
const float LOSE_BG_R = 0.1f;
const float LOSE_BG_G = 0.05f;
const float LOSE_BG_B = 0.05f;
const float LOSE_RETRY_BTN_X = 0.0f;
const float LOSE_RETRY_BTN_Y = -150.0f;
const float LOSE_MENU_BTN_X = 0.0f;
const float LOSE_MENU_BTN_Y = -250.0f;

// Credits screen
const int   CREDITS_TOTAL_PAGES = 4;
const float CREDITS_TITLE_SCALE = 2.2f;
const float CREDITS_TITLE_Y = 0.78f;
const float CREDITS_HEADER_SCALE = 1.3f;
const float CREDITS_BODY_SCALE = 1.1f;
const float CREDITS_BODY_SCALE_MED = 1.05f;
const float CREDITS_BODY_SCALE_SM = 1.0f;
const float CREDITS_PAGE_IND_SCALE = 1.1f;
const float CREDITS_PAGE_IND_Y = -0.62f;
const float CREDITS_PREV_BTN_X = -520.0f;
const float CREDITS_BACK_BTN_X = 0.0f;
const float CREDITS_NEXT_BTN_X = 520.0f;
const float CREDITS_NAV_BTN_Y = -360.0f;
const float CREDITS_PREV_NEXT_BTN_W = 180.0f;
const float CREDITS_BACK_BTN_W = 260.0f;
const float CREDITS_NAV_BTN_H = 60.0f;

// Credits page 0
const float CREDITS_P0_LINE1_Y = 0.12f;
const float CREDITS_P0_LINE2_Y = 0.00f;
const float CREDITS_P0_LINE3_Y = -0.08f;

// Credits page 1
const float CREDITS_P1_HEADER_Y = 0.32f;
const float CREDITS_P1_NAMES_START_Y = 0.18f;
const float CREDITS_P1_NAMES_STEP_Y = 0.13f;
const int   CREDITS_P1_NAME_COUNT = 4;

// Credits page 2
const float CREDITS_P2_HEADER_Y = 0.36f;
const float CREDITS_P2_PRESIDENT_HDR_Y = 0.22f;
const float CREDITS_P2_PRESIDENT_NAME_Y = 0.14f;
const float CREDITS_P2_INSTRUCTOR_HDR_Y = 0.01f;
const float CREDITS_P2_INSTRUCTOR_START_Y = -0.07f;
const float CREDITS_P2_INSTRUCTOR_STEP_Y = 0.12f;
const int   CREDITS_P2_INSTRUCTOR_COUNT = 3;

// Credits page 3
const float CREDITS_P3_HEADER_Y = 0.28f;
const float CREDITS_P3_THANKS_START_Y = 0.14f;
const float CREDITS_P3_THANKS_STEP_Y = 0.13f;
const int   CREDITS_P3_THANKS_COUNT = 3;

// Map
const float MAP_DEFAULT_TILE_SIZE = 50.0f;
const float MAP_WORLD_SIZE_TILES = 64.0f;
const float MAP_TILE_OFFSET = 25.0f;
const float MAP_CAMERA_PADDING_X = 500.0f;
const float MAP_CAMERA_PADDING_Y = 500.0f;
const float MAP_CAMERA_CLAMP_BUFFER = 200.0f;
const float MAP_ZOOM_MAX = 50.0f;
const float MAP_ZOOM_MIN = 14.0f;