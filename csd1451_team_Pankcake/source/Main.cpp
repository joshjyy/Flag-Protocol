// ---------------------------------------------------------------------------
// includes

#include "AEEngine.h"
#include "utils.hpp"  
#include "player.hpp"
#include "gunTypes.hpp"
#include "enemies.hpp"
#include "AI.hpp"  
#include "weapon.hpp"
#include <vector>
#include "map.hpp"
#include "events.hpp"
#include "screens.hpp"


//Legend if needed later
/*
SIGNED (can be negative)
s8   = tiny whole number     (-128 to 127)
s16  = small whole number    (-32,768 to 32,767)
s32  = normal whole number   (-2 billion to 2 billion)
s64  = huge whole number     (-9 quintillion to 9 quintillion)

UNSIGNED (only positive)
u8   = tiny positive number  (0 to 255)
u16  = small positive number (0 to 65,535)
u32  = normal positive number(0 to 4 billion)
u64  = huge positive number  (0 to 18 quintillion)

DECIMALS
f32  = normal decimal        (float)
f64  = precise decimal       (double)
*/

//DONT IMPORT STD NAMESPACE
// ---------------------------------------------------------------------------




 s8 fontID;
 EventManager gameEvents;


// ---------------------------------------------------------------------------
// Main entry point
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// Memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ---------------------------------------------------------------------------
	// INITIALIZATION
	// ---------------------------------------------------------------------------


	// Game variables
	int gGameRunning = 1;

	// Bullet management
	std::vector<Bullet> playerBullets;
	std::vector<Bullet> enemyBullets;

	//Init Grunt Enemy type
	std::vector<Grunt> grunts;

	//Init Event Manager
	gameEvents = InitEventManager();

	//Spawning of Grunts
	grunts.push_back(SpawnGrunt(300.0f, 200.0f));
	grunts.push_back(SpawnGrunt(-300.0f, -200.0f));
	grunts.push_back(SpawnGrunt(0.0f, 300.0f));

	//Init Player
	Player player = InitPlayer();
	// Initialize engine (1600x900 window, 60 FPS)
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);
	AESysSetWindowTitle("Game Test!");
	AESysReset();

	//load font
	fontID = AEGfxCreateFont("Assets/Fonts/Jersey10-Regular.ttf", 24); 

	//load meshes
	InitCommonMeshes();
	InitPlayerParticles(&player , meshes.square, meshes.circle);

	// ---------------------------------------------------------------------------
	// MAP
	// ---------------------------------------------------------------------------

	// 2D array to store the game map data (18 rows x 32 columns)
	auto gameMap =  new std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>;
	//Camera variables(in main, with other game variables)
	float cameraX = 0.0f;
	float cameraY = 0.0f;

	// In game loop - UPDATE section:
	// Update camera to follow player
	
	// Load the map from file into the gameMap array
	if (!Load_Game_Level("Assets/Level_1_Map.txt", *gameMap)) {
		printf("Failed to load map! Exiting...\n");
		gGameRunning = 0;
	}
	else {
		printf("Map loaded successfully!\n");
	}
	Spawn_Flag(*gameMap);
	// ---------------------------------------------------------------------------
	// GAME LOOP
	// ---------------------------------------------------------------------------
	// Set background color (gray)
	AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
	// Set render mode for colored meshes
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	// Enable transparency
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);
	// Screen state
	GameState currentState = STATE_MENU;
	while (gGameRunning)
	{
		// Frame start
		AESysFrameStart();
		// Get frame time
		float dt = (float)AEFrameRateControllerGetFrameTime();
		// Exit game
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;
		// ---------------------------------------------------------------------------
		// MENU STATE
		// ---------------------------------------------------------------------------

		if (currentState == STATE_QUIT) {
			gGameRunning = 0;
		}

		if (currentState == STATE_MENU)
		{

			currentState = UpdateAndRenderMenu(meshes.square, fontID, dt);

			if (AEInputCheckTriggered(AEVK_ESCAPE))
				gGameRunning = 0;
		}
		// ---------------------------------------------------------------------------
		// EDITOR STATE
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_EDITOR)
		{
			AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);


			if (AEInputCheckTriggered(AEVK_BACK) || AEInputCheckTriggered(AEVK_ESCAPE))
				currentState = STATE_MENU;
		}
		// ---------------------------------------------------------------------------
		// INPUT HANDLING
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_GAME)
		{

			//Check what weapon the player is using once per frame
			WeaponStats weapon = GetCurrentWeapon(player);
			// ---------------------------------------------------------------------------
			// MAP AND CAMERA
			// ---------------------------------------------------------------------------
			//Camera follows the player
			cameraX = player.x;
			cameraY = player.y;
			Load_Map(*gameMap, meshes.square, cameraX, cameraY);
			
			// Update Timers CD
			UpdatePlayer(&player, dt, *gameMap);
			// Weapon controls
			WeaponSwitchCheck(player);
			ShootingCheck(player, playerBullets, weapon, cameraX, cameraY);
			ReloadBullets(weapon, dt);
			// ---------------------------------------------------------------------------
			// UPDATE ENEMIES
			// ---------------------------------------------------------------------------
			//Update Grunt to aim at player
			UpdateGrunts(grunts, player, enemyBullets, dt, *gameMap);

			// ---------------------------------------------------------------------------
			// UPDATE BULLETS
			// ---------------------------------------------------------------------------
			UpdateBullets(playerBullets, dt, *gameMap);
			UpdateBullets(enemyBullets, dt, *gameMap);
	
			// ---------------------------------------------------------------------------
			// Collision Checks
			// ---------------------------------------------------------------------------
			// CHECK COLLISIONS 
			CheckBulletGruntCollisions(playerBullets, grunts, player);
			CheckEnemyBulletPlayerCollision(enemyBullets, player);
			CheckPlayerGruntBodyCollision(player,grunts);

			// ---------------------------------------------------------------------------
			// RENDERING
			// ---------------------------------------------------------------------------
			//Enemy Circle (draw first - bottom layer)
			RenderGrunts(grunts, meshes.circle, cameraX, cameraY);
			//Player Circle (draw on top)
			DrawPlayer(player, meshes.circle, cameraX, cameraY, dt);
			// Draw bullets (use correct overload with camera)
			RenderBullets(playerBullets, meshes.circle, cameraX, cameraY);
			RenderBullets(enemyBullets, meshes.circle, cameraX, cameraY);
			//Draw Health Bar UI
			DrawPlayerUI(player, weapon, meshes.square, meshes.circle, cameraX, cameraY);
			DrawCrosshair(meshes.square);
			// *** CHECK WIN CONDITION ***
			RemoveDeadGrunts(grunts);
			CheckWinCondition(gameEvents, player);
		}
		else if (currentState == STATE_TUTORIAL) {


			printf("Tutorial State - Press BACKSPACE or ESCAPE to return to Menu\n");




			 if (AEInputCheckTriggered(AEVK_BACK) || AEInputCheckTriggered(AEVK_ESCAPE))
				 currentState = STATE_MENU;

		}
		// ---------------------------------------------------------------------------
		//DEBUGGING
		// ---------------------------------------------------------------------------
		//Debugging Things--------------------------------------------------------------
		killPlayer(player);

		// Frame end
		AESysFrameEnd();
	}

	// ---------------------------------------------------------------------------
	// CLEANUP
	// ---------------------------------------------------------------------------
	delete gameMap;
	ClearBullets(playerBullets);
	ClearBullets(enemyBullets);
	AEGfxDestroyFont(fontID);
	ParticleEmitterFree(&player.trailEmitter);
	ParticleEmitterFree(&player.deathEmitter);
	ParticleEmitterFree(&player.dashTrailEmitter);

	FreeCommonMeshes();


	AESysExit();

	return 0;
}