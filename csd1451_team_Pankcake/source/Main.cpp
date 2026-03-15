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
#include "tutorial.hpp"
#include "editor.hpp"
#include "damageNumbers.hpp"


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

	// Editor state variables
	int currentTileType = 1;  // 1 = Walls (Green)
	s32 mouseX = 0;
	s32 mouseY = 0;

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
	// Camera
	// ---------------------------------------------------------------------------
	bool editorPaintReady = false;
	std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>* gameMap = new std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>;
	//Camera variables(in main, with other game variables)
	float cameraX = 0.0f;
	float cameraY = 0.0f;
	CameraPos camera;	

	// Cutscene variables
	CutsceneData cutscene;

	

	
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
	static GameState previousState = STATE_MENU;
	
	while (gGameRunning)
	{
		AESysFrameStart();
		float dt = (float)AEFrameRateControllerGetFrameTime();

		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;

		if (currentState != previousState) {
			CurrGamestate(currentState, gGameRunning, *gameMap);
			previousState = currentState;

			// Force spawn when entering tutorial temporary fix
			if (currentState == STATE_TUTORIAL) {
				player.x = 15 * TILE_SIZE - 800.0f + 25.0f;
				player.y = 450.0f - 31 * TILE_SIZE - 25.0f;
			}

			if (currentState == STATE_CUTSCENE) {
				cutscene = CutsceneData{};  // reset
			}

			if (currentState == STATE_EDITOR) {
				editorPaintReady = false;
			}
		}

		if (currentState == STATE_QUIT) {
			gGameRunning = 0;
		}

		// ---------------------------------------------------------------------------
		// STATE_MENU
		// ---------------------------------------------------------------------------
		if (currentState == STATE_MENU) {
			currentState = UpdateAndRenderMenu(meshes.square, fontID, dt);
			if (AEInputCheckTriggered(AEVK_Q))
				gGameRunning = 0;
		}

		// ---------------------------------------------------------------------------
		// STATE_TUTORIAL
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_TUTORIAL) {
			ShowCursor(false);
			Load_Map(*gameMap, cameraX, cameraY);

			Tutorial_RenderHints(fontID);
			Tutorial_RenderDebug(fontID, player);
				
			Range_Hacks(player);
		
			//Check what weapon the player is using once per frame
			WeaponStats weapon = GetCurrentWeapon(player);

			// ---------------------------------------------------------------------------
			// MAP AND CAMERA
			// ---------------------------------------------------------------------------
			//Camera follows the player
			cameraX = player.x;
			cameraY = player.y;

			// Update Timers CD
			UpdatePlayer(&player, dt, *gameMap);
			// Weapon controls
			WeaponSwitchCheck(player);
			ShootingCheck(player, playerBullets, weapon, cameraX, cameraY);
			ReloadBullets(weapon, dt);
	
			// ---------------------------------------------------------------------------
			// UPDATE BULLETS
			// ---------------------------------------------------------------------------
			UpdateBullets(playerBullets, dt, *gameMap);
		
			// ---------------------------------------------------------------------------
			// RENDERING
			// ---------------------------------------------------------------------------
			//Player Circle (draw on top)
			DrawPlayer(player, meshes.circle, cameraX, cameraY, dt);
			// Draw bullets (use correct overload with camera)
			RenderBullets(playerBullets, meshes.circle, cameraX, cameraY);

			//Draw Health Bar UI
			DrawPlayerUI(player, weapon, meshes.square, meshes.circle, cameraX, cameraY);
			DrawCrosshair(meshes.square);

			CheckWinCondition(gameEvents, player);
			if (AEInputCheckTriggered(AEVK_Q))
				currentState = STATE_MENU;
				
		}

		// ---------------------------------------------------------------------------
		// STATE_LOAD
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_LOAD) {
			Load_Map(*gameMap, cameraX, cameraY);
			if (AEInputCheckTriggered(AEVK_Q))
				currentState = STATE_MENU;
				
		}

		else if (currentState == STATE_CUTSCENE)
		{
			Cutscene_Draw(cutscene, fontID, meshes.square);
			if (Cutscene_Update(cutscene, dt))
				currentState = STATE_GAME;
		}   
	
		// ---------------------------------------------------------------------------
		// STATE_CUTSCENE
		// ---------------------------------------------------------------------------

	
		else if (currentState == STATE_CUTSCENE)
		{
			Cutscene_Draw(cutscene, fontID, meshes.square);
			if (Cutscene_Update(cutscene, dt))
				currentState = STATE_GAME;
		}
		// ---------------------------------------------------------------------------
		// GAME STATE
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_GAME)
		{
			//Check for pause input
			if (AEInputCheckTriggered(AEVK_P) || AEInputCheckTriggered(AEVK_ESCAPE)) {
				currentState = STATE_PAUSE;
				printf("Pause triggered!\n");
			}

			//Check what weapon the player is using once per frame
			WeaponStats weapon = GetCurrentWeapon(player);
			
			// ---------------------------------------------------------------------------
			// MAP AND CAMERA
			// ---------------------------------------------------------------------------
			//Camera follows the player
			cameraX = player.x;
			cameraY = player.y;
			Load_Map(*gameMap, cameraX, cameraY);
			
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
			// UPDATE DAMAGE NUMBERS
			// ---------------------------------------------------------------------------
			UpdateDamageNumbers(dt);
	
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
			RenderDamageNumbers(cameraX, cameraY, fontID);
			// *** CHECK WIN CONDITION ***
			RemoveDeadGrunts(grunts);
			CheckWinCondition(gameEvents, player);

			if (AEInputCheckTriggered(AEVK_Q))
				currentState = STATE_MENU;
			DrawScoreUI(player, meshes.square , dt);

		}

		// ---------------------------------------------------------------------------
		// PAUSED STATE
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_PAUSE)
		{
			ShowCursor(true);

			cameraX = player.x;
			cameraY = player.y;

			// Render the map
			Load_Map(*gameMap, cameraX, cameraY);

			// Render enemies
			RenderGrunts(grunts, meshes.circle, cameraX, cameraY);

			// Render player
			DrawPlayer(player, meshes.circle, cameraX, cameraY, 0.0f);

			// Render bullets
			RenderBullets(playerBullets, meshes.circle, cameraX, cameraY);
			RenderBullets(enemyBullets, meshes.circle, cameraX, cameraY);

			// Render UI
			WeaponStats weapon = GetCurrentWeapon(player);
			DrawPlayerUI(player, weapon, meshes.square, meshes.circle, cameraX, cameraY);

			// Pause menu on top
			currentState = UpdateAndRenderPause(meshes.square, fontID, dt);

			}

		// ---------------------------------------------------------------------------
		// EDITOR STATE
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_EDITOR)
		{
			static EditorState editor = InitEditor();
			static bool editorLoaded = false;

			if (!editorLoaded) {
				GameStateLoadEditor(gGameRunning, *gameMap);
				SaveInitialEditorMap(*gameMap); 
				editorLoaded = true;
			}

			UpdateAndRenderEditor(editor, *gameMap, meshes.square, fontID, gGameRunning);

			if (AEInputCheckTriggered(AEVK_Q)) {
				currentState = STATE_MENU;
				editorLoaded = false;
			}
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