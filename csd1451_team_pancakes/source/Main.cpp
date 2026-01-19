// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"


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

	//Init Player
	Player player = InitPlayer();




	//Reloading variables
	//To add CD to reload so reloading takes time
	float reloadTime = 0.f;

	// Initialize engine (1600x900 window, 60 FPS)
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);
	AESysSetWindowTitle("Game Test!");
	AESysReset();

	//dont move this up
	InitCommonMeshes();




	// ---------------------------------------------------------------------------
	// GAME LOOP
	// ---------------------------------------------------------------------------

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
		// Exit game
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;
		
		// Update Timers CD
		UpdatePlayerTimers(&player, dt);
		UpdatePlayerMovement(&player, dt);

	

		ResetShootCooldown(player, dt);
		WeaponSwitchCheck(player);
		ShootingCheck(player, playerBullets, weapon);
		ReloadBullets(weapon, reloadTime, dt);

		// ---------------------------------------------------------------------------
		// UPDATE BULLETS
		// ---------------------------------------------------------------------------

		UpdateBullets(playerBullets, dt);

		// ---------------------------------------------------------------------------
		// Collision Checks
		// ---------------------------------------------------------------------------
		






		// ---------------------------------------------------------------------------
		// RENDERING
		// ---------------------------------------------------------------------------

		// Set background color (gray)
		AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);

		// Set render mode for colored meshes
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);

		// Enable transparency
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);


		//Player Circle
		AEGfxSetColorToMultiply(0.0f, 0.0f, 1.0f, 1.0f);
		DrawSprite(meshes.circle, player.x, player.y, 50.0f, 50.0f, 0.0f);

		// Draw bullets
		RenderBullets(playerBullets, meshes.circle);

		
		// Draw Background Health Bar (top left)
		AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f);
		DrawSprite(meshes.square, -250.0f, 400.0f, 500.0f, 50.0f, 0.0f);

		// Draw Current Health (top left)
		AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
		float healthbarOffset = (1000.0f - player.health) / 2.0f;
		DrawSprite(meshes.square, -250.0f - healthbarOffset, 400.0f, player.health, 50.0f, 0.0f);



	




		// ---------------------------------------------------------------------------
		//DEBUGGING
		// ---------------------------------------------------------------------------
	/*	int bulletCount = GetBulletCount(bulletHead);
		printf("Active bullets: %d\n", bulletCount);*/
	 
		// Frame end
		AESysFrameEnd();
	}

	// ---------------------------------------------------------------------------
	// CLEANUP
	// ---------------------------------------------------------------------------

	ClearBullets(playerBullets);
	FreeCommonMeshes();

	AESysExit();

	return 0;
}















