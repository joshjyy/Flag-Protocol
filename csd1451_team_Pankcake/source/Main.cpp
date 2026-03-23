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
#include "gameMode.hpp"
#include "gameConstants.hpp"
#include "debris.hpp"
#include "sound.hpp"

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
	//grunts.push_back(SpawnGrunt(300.0f, 200.0f));
	//grunts.push_back(SpawnGrunt(-300.0f, -200.0f));
	//grunts.push_back(SpawnGrunt(0.0f, 300.0f));

	//Init Player
	Player player = InitPlayer();
	//Init spawner
	InitGruntSpawner();
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
	std::vector<std::vector<int>>* gameMap = new std::vector<std::vector<int>>;
	//Camera variables(in main, with other game variables)	

	CameraPos camera;	

	// Cutscene variables
	CutsceneData cutscene;
	Background_Load();
	Background_Init();

	//Sound Load
	SoundInit();
	

	
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
	ShowCursor(true);
	AEAudioPlay(MainMenuBGM, MainBGMGroup, 1.0f, 1.0f, 1);
	
	while (gGameRunning)
	{
		AESysFrameStart();
		float dt = (float)AEFrameRateControllerGetFrameTime();
		
		

		//State Transition Stuff
		if (currentState != previousState) {
			CurrGamestate(currentState, gGameRunning, *gameMap, player, previousState);

			// Stop BGM only when leaving game or pause
			if (previousState == STATE_GAME && currentState != STATE_PAUSE)
				AEAudioStopGroup(GameBGMGroup);

			// Stop menu BGM when leaving menu or nav

			if ((previousState == STATE_MENU || previousState == STATE_NAVIGATION)
				&& currentState != STATE_MENU
				&& currentState != STATE_NAVIGATION)
				AEAudioStopGroup(MainBGMGroup);

			if (previousState == STATE_PAUSE && currentState == STATE_GAME)
				ShowCursor(false);

			if ((previousState == STATE_GAME || previousState == STATE_PAUSE) && currentState == STATE_MENU){
				//Save player score
				SavePlayerScore(player);
			}

			// Force spawn when entering tutorial temporary fix
			if (currentState == STATE_TUTORIAL) {
				player.x = 15 * TILE_SIZE - 800.0f + 25.0f;
				player.y = 450.0f - 31 * TILE_SIZE - 25.0f;
				
				camera.x = player.x;
				camera.y = player.y;
				FreeGrunts(grunts);
				grunts.clear();
				InitGruntSpawner();
			}

			if (currentState == STATE_CUTSCENE) {
				cutscene = CutsceneData{};  // reset
			}

			if (currentState == STATE_EDITOR) {
				AEAudioStopGroup(MainBGMGroup);
				AEAudioPlay(EditorBGM, EditorBGMGroup, 1.0f, 1.0f, 1);
				editorPaintReady = false;
			}
			if (currentState == STATE_GAME) {
				Background_Init();
				FreeGrunts(grunts);
				grunts.clear();
				InitGruntSpawner();
			}
			if (currentState == STATE_CUSTOM) {
				Background_Init();
				FreeGrunts(grunts);
				grunts.clear();
				InitGruntSpawner();
			}

			if (currentState != STATE_PAUSE  && currentState != STATE_LOSE) {
				previousState = currentState;
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
		// STATE_NAVIGATION
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_NAVIGATION)
		{
			currentState = UpdateAndRenderNavigation(meshes.square, fontID, dt);

			if (AEInputCheckTriggered(AEVK_Q))
				currentState = STATE_MENU;
		}

		else if(currentState == STATE_CONTROLS)
		{
			printf("Not Implemented Yet!\n");
		}
		// ---------------------------------------------------------------------------
		// STATE_TUTORIAL
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_TUTORIAL) {


			Update_CameraPosition(camera, player.x, player.y, dt);
			Load_Map(*gameMap, camera);

			Tutorial_RenderHints(fontID);
			Tutorial_RenderDebug(fontID, player);
				
			Range_Hacks(player);
			UpdateAndRenderTutorial(player, dt, gameMap, camera, playerBullets, enemyBullets, grunts, gameEvents);



			if (AEInputCheckTriggered(AEVK_Q))
				currentState = STATE_MENU;
			if (AEInputCheckTriggered(AEVK_P) || AEInputCheckTriggered(AEVK_ESCAPE)) {
				currentState = STATE_PAUSE;
				ShowCursor(true);

				printf("Pause triggered!\n");
			}
		}

		// ---------------------------------------------------------------------------
		// STATE_LOAD
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_CUSTOM) {
			
			
			
			if (AEInputCheckTriggered(AEVK_P) || AEInputCheckTriggered(AEVK_ESCAPE)) {
				currentState = STATE_PAUSE;
				ShowCursor(true);

				printf("Pause triggered!\n");
			}
			Background_Update(dt);
			Background_Draw(camera);
			Update_CameraPosition(camera, player.x, player.y, dt);
			UpdateAndRenderGameplay(player, dt, gameMap, camera, playerBullets, enemyBullets, grunts, gameEvents);

			if (gameEvents.winTriggered)
			{
				currentState = STATE_WIN;
				gameEvents.winTriggered = false; // Reset for potential future use
			}

			if (gameEvents.playerDied)
			{
				currentState = STATE_LOSE;
				gameEvents.playerDied = false; // Reset for potential future use
			}
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
			AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
			//Check for pause input
			if (AEInputCheckTriggered(AEVK_P) || AEInputCheckTriggered(AEVK_ESCAPE)) {
				currentState = STATE_PAUSE;
				ShowCursor(true);

				printf("Pause triggered!\n");
			}
			
			Background_Update(dt);
			Update_CameraPosition(camera, player.x, player.y, dt);
			UpdateAndRenderGameplay(player, dt, gameMap, camera, playerBullets, enemyBullets, grunts, gameEvents);
			
		
			if(gameEvents.winTriggered)
			{
				currentState = STATE_WIN;
				gameEvents.winTriggered = false; // Reset for potential future use
			}

			if (gameEvents.playerDied)
			{
				currentState = STATE_LOSE;
				gameEvents.playerDied = false; // Reset for potential future use
			}
			
		}

		// ---------------------------------------------------------------------------
		// PAUSED STATE
		// ---------------------------------------------------------------------------
		else if (currentState == STATE_PAUSE)
		{

			camera.x = player.x;
			camera.y = player.y;

			// Render the map
			Load_Map(*gameMap, camera);

			// Render enemies
			RenderGrunts(grunts, meshes.circle, camera);

			// Render player
			DrawPlayer(player, meshes.circle, camera, 0.0f);

			// Render bullets
			RenderBullets(playerBullets, meshes.circle, camera);
			RenderBullets(enemyBullets, meshes.circle, camera);

			// Render UI
			WeaponStats weapon = GetCurrentWeapon(player);
			DrawPlayerUI(player, weapon, meshes.square, meshes.circle, camera);

			// Pause menu on top
			currentState = UpdateAndRenderPause(meshes.square, fontID, dt, previousState);
			

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

			UpdateAndRenderEditor(editor, *gameMap, meshes.square);
			Popup_Update(popup, dt); 
			Popup_Draw(popup, meshes.square, fontID);  

			
		}

		else if(currentState == STATE_WIN)
		{
			currentState = UpdateAndRenderWinScreen(meshes.square, fontID, dt, player);
		
		}
		else if(currentState == STATE_LOSE) 
		{
			while (ShowCursor(true) < 0); //TMP fix
			currentState = UpdateAndRenderLoseScreen(meshes.square, fontID, dt, player, previousState);
		}

		else if(currentState == STATE_CREDITS)
		{
			printf("Credits Not Implemented Yet!\n");
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
	ClearDamageNumbers();
	Background_Free();
	AEGfxDestroyFont(fontID);
	ParticleEmitterFree(&player.trailEmitter);
	ParticleEmitterFree(&player.deathEmitter);
	ParticleEmitterFree(&player.dashTrailEmitter);
	FreeCommonMeshes();
	FreeGrunts(grunts);
	SoundFree();

	AESysExit();

	return 0;
}