// ---------------------------------------------------------------------------
// includes

#include "AEEngine.h"
#include "utils.hpp"  
#include "player.hpp"
#include "gunTypes.hpp"
#include "enemies.hpp"
#include "weapon.hpp"
#include <vector>
#include "map.hpp"
#include "events.hpp"


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
	InitPlayerParticles(&player , meshes.square);

	// ---------------------------------------------------------------------------
	// MAP
	// ---------------------------------------------------------------------------

	// 2D array to store the game map data (18 rows x 32 columns)
	std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT> gameMap;
	//Camera variables(in main, with other game variables)
	float cameraX = 0.0f;
	float cameraY = 0.0f;

	// In game loop - UPDATE section:
	// Update camera to follow player
	
	// Load the map from file into the gameMap array
	if (!Load_Game_Level("Assets/Level_1_Map.txt", gameMap)) {
		printf("Failed to load map! Exiting...\n");
		gGameRunning = 0;
	}
	else {
		printf("Map loaded successfully!\n");
	}

	// ---------------------------------------------------------------------------
	// GAME LOOP
	// ---------------------------------------------------------------------------
	// Set background color (gray)
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	// Set render mode for colored meshes
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	// Enable transparency
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	while (gGameRunning)
	{
		// Frame start
		AESysFrameStart();
		// Get frame time
		float dt = (float)AEFrameRateControllerGetFrameTime();

		// ---------------------------------------------------------------------------
		// INPUT HANDLING
		// ---------------------------------------------------------------------------

		//Check what weapon the player is using once per frame
		WeaponStats weapon = GetCurrentWeapon(player);

		// ---------------------------------------------------------------------------
		// MAP AND CAMERA
		// ---------------------------------------------------------------------------
		//Camera follows the player
		cameraX = player.x;
		cameraY = player.y;
		Load_Map(gameMap, meshes.square, cameraX, cameraY);

		// Exit game
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;
		


		// Update Timers CD
		UpdatePlayer(&player, dt, gameMap);


		// Weapon controls
		ResetShootCooldown(player, dt);
		WeaponSwitchCheck(player);
		ShootingCheck(player, playerBullets, weapon, cameraX, cameraY);
		ReloadBullets(weapon, dt);

		// ---------------------------------------------------------------------------
		// UPDATE ENEMIES
		// ---------------------------------------------------------------------------

		//Update Grunt to aim at player
		UpdateGrunts(grunts, player, enemyBullets, dt);

		// ---------------------------------------------------------------------------
		// UPDATE BULLETS
		// ---------------------------------------------------------------------------

		UpdateBullets(playerBullets, dt, gameMap);
		UpdateBullets(enemyBullets, dt, gameMap);

		if (AEInputCheckTriggered(AEVK_K)) {
			player.isDead = true;
		}
	
		// ---------------------------------------------------------------------------
		// Collision Checks
		// ---------------------------------------------------------------------------
		// CHECK COLLISIONS 
		CheckBulletGruntCollisions(playerBullets, grunts, player);
		CheckEnemyBulletPlayerCollision(enemyBullets, player);




		// ---------------------------------------------------------------------------
		// RENDERING
		// ---------------------------------------------------------------------------
		// In your update function
		

		// Debug: Emit particles at mouse position when left-clicking
		//if (AEInputCheckTriggered(AEVK_LBUTTON)) {
		//	s32 mx, my;
		//	AEInputGetCursorPosition(&mx, &my);

		//	float wx = (float)mx - 1600/ 2.0f + cameraX;
		//	float wy = -(float)my + 800/ 2.0f + cameraY;

		//	ParticleEmitterEmit(player.trailEmitter, wx, wy, 20);
		//}
	

		//Enemy Circle (draw first - bottom layer)
		RenderGrunts(grunts, meshes.circle, cameraX, cameraY);

		//Player Circle (draw on top)
		DrawPlayer(player, meshes.circle, cameraX, cameraY, dt);

		// Draw bullets (use correct overload with camera)
		RenderBullets(playerBullets, meshes.circle, cameraX, cameraY);
		RenderBullets(enemyBullets, meshes.circle, cameraX, cameraY);

		//Draw Health Bar UI
		DrawPlayerUI(player, weapon, meshes.square, meshes.circle, cameraX, cameraY);
		// *** CHECK WIN CONDITION ***
		CheckWinCondition(gameEvents, player);
		// ---------------------------------------------------------------------------
		//DEBUGGING
		// ---------------------------------------------------------------------------

		// Frame end
		AESysFrameEnd();
	}

	// ---------------------------------------------------------------------------
	// CLEANUP
	// ---------------------------------------------------------------------------

	ClearBullets(playerBullets);
	ClearBullets(enemyBullets);
	AEGfxDestroyFont(fontID);
	ParticleEmitterFree(&player.trailEmitter);
	ParticleEmitterFree(&player.deathEmitter);
	FreeCommonMeshes();


	AESysExit();

	return 0;
}