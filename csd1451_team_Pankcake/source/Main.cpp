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
	InitPlayerParticles(&player , meshes.square);

	// Editor variables
	int currentTileType = 1;  // 1 = green wall, 2 = cyan wall, 0 = eraser
	bool showEditorGrid = true;

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
		if (currentState == STATE_MENU)
		{
			if (IsQuitClicked()) gGameRunning = 0;
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

			// Get mouse position for tile painting
			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);

			// Convert to world coordinates
			float worldX = (float)mouseX - 800.0f;
			float worldY = 450.0f - (float)mouseY;

			// Convert to grid coordinates
			int gridX = (int)((worldX + 800.0f) / 50.0f);
			int gridY = (int)((450.0f - worldY) / 50.0f);

			// Check if within grid bounds
			if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
				// Left click to paint
				if (AEInputCheckCurr(AEVK_LBUTTON)) {
					gameMap[gridY][gridX] = currentTileType;
				}
				// Right click to erase
				if (AEInputCheckCurr(AEVK_RBUTTON)) {
					gameMap[gridY][gridX] = 0;
				}
			}

			// Number keys to change tile type
			if (AEInputCheckTriggered(AEVK_0)) currentTileType = 0;  // Eraser
			if (AEInputCheckTriggered(AEVK_1)) currentTileType = 1;  // Red, wall/obstacle
			if (AEInputCheckTriggered(AEVK_2)) currentTileType = 2;  // Blue, Water
			if (AEInputCheckTriggered(AEVK_3)) currentTileType = 3;  // Green, Grass
			if (AEInputCheckTriggered(AEVK_4)) currentTileType = 4;  // Yellow, Sand
			if (AEInputCheckTriggered(AEVK_5)) currentTileType = 5;  // Purple, Special

			// Save map (S key)
			if (AEInputCheckTriggered(AEVK_S)) {
				Save_Game_Level("Assets/My_Custom_Map.txt", gameMap);
				printf("Map saved!\n");
			}

			// Load map (L key)
			if (AEInputCheckTriggered(AEVK_L)) {
				Load_Game_Level("Assets/Level_1_Map.txt", gameMap);
				printf("Map loaded!\n");
			}

			// Clear map (C key)
			if (AEInputCheckTriggered(AEVK_C)) {
				for (int i = 0; i < MAP_HEIGHT; i++) {
					for (int j = 0; j < MAP_WIDTH; j++) {
						gameMap[i][j] = 0;
					}
				}
				printf("Map cleared!\n");
			}

			// Render the map with centered camera
			cameraX = 0.0f;  
			cameraY = 0.0f;
			Load_Map(gameMap, meshes.square, cameraX, cameraY);

			// Draw UI instructions
			float uiY = 350.0f;
			float textX, textY;

			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "EDITOR MODE", textX, textY, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f);

			// Tiles
			uiY -= 40.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "1: Green", textX, textY, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f);  

			uiY -= 30.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "2: Cyan", textX, textY, 0.6f, 0.0f, 1.0f, 1.0f, 1.0f);  

			uiY -= 30.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "3: Lime", textX, textY, 0.6f, 0.5f, 1.0f, 0.0f, 1.0f);  

			uiY -= 30.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "4: Yellow", textX, textY, 0.6f, 1.0f, 0.8f, 0.0f, 1.0f); 

			uiY -= 30.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "5: Purple", textX, textY, 0.6f, 1.0f, 0.0f, 1.0f, 1.0f); 

			uiY -= 30.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "0: Eraser", textX, textY, 0.6f, 1.0f, 1.0f, 1.0f, 1.0f);

			uiY -= 50.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "S: Save", textX, textY, 0.6f, 1.0f, 1.0f, 1.0f, 1.0f);

			uiY -= 30.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "L: Load", textX, textY, 0.6f, 1.0f, 1.0f, 1.0f, 1.0f);

			uiY -= 30.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			AEGfxPrint(fontID, "C: Clear", textX, textY, 0.6f, 1.0f, 1.0f, 1.0f, 1.0f);

			// Show current tile
			uiY -= 50.0f;
			GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
			char tileText[32];

			const char* tileName = "";
			if (currentTileType == 0) tileName = "Eraser";
			else if (currentTileType == 1) tileName = "Green";
			else if (currentTileType == 2) tileName = "Cyan";
			else if (currentTileType == 3) tileName = "Lime";
			else if (currentTileType == 4) tileName = "Yellow";
			else if (currentTileType == 5) tileName = "Purple";

			sprintf_s(tileText, 32, "Current: %s", tileName);

			// Colour of tile
			if (currentTileType == 0)
				AEGfxPrint(fontID, tileText, textX, textY, 0.6f, 1.0f, 1.0f, 1.0f, 1.0f);
			else if (currentTileType == 1)
				AEGfxPrint(fontID, tileText, textX, textY, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f);
			else if (currentTileType == 2)
				AEGfxPrint(fontID, tileText, textX, textY, 0.6f, 0.0f, 1.0f, 1.0f, 1.0f);
			else if (currentTileType == 3)
				AEGfxPrint(fontID, tileText, textX, textY, 0.6f, 0.5f, 1.0f, 0.0f, 1.0f);
			else if (currentTileType == 4)
				AEGfxPrint(fontID, tileText, textX, textY, 0.6f, 1.0f, 0.8f, 0.0f, 1.0f);
			else if (currentTileType == 5)
				AEGfxPrint(fontID, tileText, textX, textY, 0.6f, 1.0f, 0.0f, 1.0f, 1.0f);

			// Back to menu
			if (AEInputCheckTriggered(AEVK_BACK) || AEInputCheckTriggered(AEVK_ESCAPE)) {
				currentState = STATE_MENU;
			}
		}		
		
		// ---------------------------------------------------------------------------
		// INPUT HANDLING
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_GAME)
		{
			AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

			//For the pause screen mid-game
			if (AEInputCheckTriggered(AEVK_P) || AEInputCheckTriggered(AEVK_ESCAPE)) {
				currentState = STATE_PAUSE;
			}

			//Check what weapon the player is using once per frame
			WeaponStats weapon = GetCurrentWeapon(player);

			// ---------------------------------------------------------------------------
			// MAP AND CAMERA
			// ---------------------------------------------------------------------------
			//Camera follows the player
			cameraX = player.x;
			cameraY = player.y;
			Load_Map(gameMap, meshes.square, cameraX, cameraY);





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
		}

			// ---------------------------------------------------------------------------
			// PAUSED STATE
			// ---------------------------------------------------------------------------
			else if (currentState == STATE_PAUSE)
			{
				Load_Map(gameMap, meshes.square, cameraX, cameraY);

				RenderGrunts(grunts, meshes.circle, cameraX, cameraY);
				DrawPlayer(player, meshes.circle, cameraX, cameraY, dt);
				RenderBullets(playerBullets, meshes.circle, cameraX, cameraY);
				RenderBullets(enemyBullets, meshes.circle, cameraX, cameraY);

				WeaponStats weapon = GetCurrentWeapon(player);
				DrawPlayerUI(player, weapon, meshes.square, meshes.circle, cameraX, cameraY);

				currentState = UpdateAndRenderPause(meshes.square, fontID, dt);
				}

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