// ---------------------------------------------------------------------------
// includes
#include <crtdbg.h>
#include "AEEngine.h"
#include "utils.hpp"  
#include "player.hpp"
#include <cstdint>
#include "gunTypes.hpp"

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
	float playerX = 0.0f;
	float playerY = 0.0f;

	float healthbar = 1000.0f;


	// Bullet management
	Bullet* bulletHead = nullptr;  // Head of bullet linked list

	//float shootCooldown = 0.0f;
	//const float SHOOT_DELAY = 0.05f;  
	//const float BULLET_SPEED = 1000.0f;

	//Initialise player starting pistol
	Pistol playerPistol;
	float shootCooldown = 0.0f;


	// Initialize engine (1600x900 window, 60 FPS)
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);
	AESysSetWindowTitle("Game Test!");
	AESysReset();

	// Create square mesh (a quad made of 2 triangles)
	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
	);

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
	);

	AEGfxVertexList* squareMesh = AEGfxMeshEnd();

	// Create circle mesh (a circle made of triangles, reckon around 30 segments)
	AEGfxMeshStart();

	int segments = 30; 
	//Create circle using triangle fan method ( like a pizza ) 10 slices
	for (int i = 0; i < segments; i++) {
		float angle1 = (i * 360.0f / segments) * PI / 180.0f;
		float angle2 = ((i + 1) * 360.0f / segments) * PI / 180.0f;

		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f,           // Center point
			cosf(angle1) * 0.5f, sinf(angle1) * 0.5f, 0xFFFFFFFF, 0.0f, 0.0f,  // Edge point 1
			cosf(angle2) * 0.5f, sinf(angle2) * 0.5f, 0xFFFFFFFF, 0.0f, 0.0f   // Edge point 2
		);
	}

	AEGfxVertexList* circleMesh = AEGfxMeshEnd();





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

		// Exit game
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;
		
		// Movement
			// Movement
		if (AEInputCheckCurr(AEVK_W)) {
			playerY += 500.0f * dt;
			if (AEInputCheckPrev(AEVK_LSHIFT)) {
				playerY += 1500.0f * dt;
			}

		}

		if (AEInputCheckCurr(AEVK_A)) {
			playerX -= 500.0f * dt;
			if (AEInputCheckPrev(AEVK_LSHIFT)) {
				playerX -= 1500.0f * dt;
			}
		}

		if (AEInputCheckCurr(AEVK_S)) {
			playerY -= 500.0f * dt;
			if (AEInputCheckPrev(AEVK_LSHIFT)) {
				playerY -= 1500.0f * dt;
			}
		}

		if (AEInputCheckCurr(AEVK_D)) {
			playerX += 500.0f * dt;
			if (AEInputCheckPrev(AEVK_LSHIFT)) {
				playerX += 1500.0f * dt;
			}
		}
	
		

		// Bullet shooting cooldown
		if (shootCooldown > 0.0f) {
			shootCooldown -= dt;
		}
		// Shooting Check + Ammo Check (Use a playerStruct later so the ammo check is not hardcoded to the starter pistol)
		if (AEInputCheckCurr(AEVK_LBUTTON) && shootCooldown <= 0.0f && playerPistol.currentAmmo > 0) {
			// Get mouse position in world coordinates 
			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);

			/*  
				Screen(0, 0)      →  World(-800, +450)[top - left corner]
				Screen(800, 450)  →  World(0, 0)[center of screen]
				Screen(1600, 900) →  World(+800, -450)[bottom - right corner]
				Screen(1600, 0)   →  World(+800, +450)[top - right corner]
			*/

			// Convert screen coordinates to world coordinates
			float worldMouseX = (float)mouseX - 800.0f;  
			float worldMouseY = 450.0f - (float)mouseY;  

			// Create bullet at player aimed at mouse x and y
			Bullet* newBullet = CreateBullet(playerX, playerY, worldMouseX, worldMouseY,
											 playerPistol.bulletSpeed, playerPistol.bulletDamage);

			// Add to front of linked list
			newBullet->next = bulletHead;
			bulletHead = newBullet;

			//Ammo of the gun decreases as the player shoots
			playerPistol.currentAmmo--;

			// Reset cooldown
			shootCooldown = playerPistol.fireRate;
		}

		// ---------------------------------------------------------------------------
		// UPDATE BULLETS
		// ---------------------------------------------------------------------------

		UpdateBullets(&bulletHead, dt);

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
		DrawSprite(circleMesh, playerX, playerY, 100.0f, 100.0f, 0.0f);

		// Draw bullets
		RenderBullets(bulletHead, circleMesh);

		
		//Draw Background Health Bar
		AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f);
		DrawSprite(squareMesh, 0.0f, 350.0f, 1000.0f, 50.0f, 0.0f);
		//Draw Current Health
		AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
		float healthbarOffset = (1000.0f - healthbar) / 2.0f;
		DrawSprite(squareMesh, 0.0f - healthbarOffset, 350.0f, healthbar, 50.0f, 0.0f);



	




		// ---------------------------------------------------------------------------
		//DEBUGGING
		// ---------------------------------------------------------------------------
		int bulletCount = GetBulletCount(bulletHead);
		printf("Active bullets: %d\n", bulletCount);
	 
		// Frame end
		AESysFrameEnd();
	}

	// ---------------------------------------------------------------------------
	// CLEANUP
	// ---------------------------------------------------------------------------
	FreeBullets(&bulletHead);


	AEGfxMeshFree(squareMesh);
	AEGfxMeshFree(circleMesh);
	AESysExit();

	return 0;
}















