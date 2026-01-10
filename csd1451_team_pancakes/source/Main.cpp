// ---------------------------------------------------------------------------
// includes
#include <crtdbg.h>
#include "AEEngine.h"
#include "utils.hpp"  
#include "player.hpp"
#include <cstdint>

//Legend if needed later
/*
typedef int8_t    s8; signed 8 bit
typedef uint8_t   u8; unsigned 8 but
typedef int16_t   s16;
typedef uint16_t  u16;
typedef int32_t   s32;
typedef uint32_t  u32;
typedef int64_t   s64;
typedef uint64_t  u64;
typedef float     f32;
typedef double    f64;
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
	float shootCooldown = 0.0f;
	const float SHOOT_DELAY = 0.1f;  // Time between shots (seconds)
	const float BULLET_SPEED = 800.0f;


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
			playerY += 1200.0f * dt;
		}

		if (AEInputCheckCurr(AEVK_A)) {
			playerX -= 1200.0f * dt;
		}

		if (AEInputCheckCurr(AEVK_S)) {
			playerY -= 1200.0f * dt;
		}

		if (AEInputCheckCurr(AEVK_D)) {
			playerX += 1200.0f * dt;
		}

		// Bullet shooting cooldown
		if (shootCooldown > 0.0f) {
			shootCooldown -= dt;
		}
		// Shooting (SPACE key)
		if (AEInputCheckCurr(AEVK_SPACE) && shootCooldown <= 0.0f) {
			// Get mouse position in world coordinates AT THIS INSTANT
			s32 mouseX, mouseY;
			AEInputGetCursorPosition(&mouseX, &mouseY);

			// Convert screen coordinates to world coordinates
			float worldMouseX = (float)mouseX - 800.0f;  // 800 = half of 1600 (window width)
			float worldMouseY = 450.0f - (float)mouseY;  // 450 = half of 900 (window height)

			// Create bullet aimed at CURRENT mouse position
			Bullet* newBullet = CreateBullet(playerX, playerY, worldMouseX, worldMouseY, BULLET_SPEED);

			// Add to front of linked list
			newBullet->next = bulletHead;
			bulletHead = newBullet;

			// Reset cooldown
			shootCooldown = SHOOT_DELAY;
		}

		// ---------------------------------------------------------------------------
		// UPDATE BULLETS
		// ---------------------------------------------------------------------------

		UpdateBullets(&bulletHead, dt);


		//Circle collision
		if (AreCirclesIntersecting(playerX, playerY, 50.0f, -400.0f, -100.0f, 150.0f)) {
			// Lose health
			if (healthbar > 0) {
				healthbar -= 600.0f * dt;
			}
		}

		if (AreCirclesIntersecting(playerX, playerY, 50.0f, 400.0f, -100.0f, 150.0f)) {
			// Gain health
			if (healthbar < 1000) {
				healthbar += 600.0f * dt;
			}
		}

		// ---------------------------------------------------------------------------
		// RENDERING
		// ---------------------------------------------------------------------------

		// Set background color (gray)
		AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);

		// Set render mode for textured objects
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);

		// Enable transparency
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);

		//Red
		AEGfxSetColorToMultiply(0.8f, 0.0f, 0.0f, 1.0f);
		DrawSprite(circleMesh, -400.0f, -100.0f, 400.0f, 400.0f, 0.0f);

		//Green
		AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);
		DrawSprite(circleMesh, 400.0f, -100.0f, 400.0f, 400.0f, 0.0f);


		//Blue
		AEGfxSetColorToMultiply(0.0f, 0.0f, 1.0f, 1.0f);
		DrawSprite(circleMesh, playerX, playerY, 100.0f, 100.0f, 0.0f);

		// Render bullets using Bullet.cpp function
		RenderBullets(bulletHead, circleMesh);

		

		AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f);
		DrawSprite(squareMesh, 0.0f, 350.0f, 1000.0f, 50.0f, 0.0f);


		AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
		float healthbarOffset = (1000.0f - healthbar) / 2.0f;
		DrawSprite(squareMesh, 0.0f - healthbarOffset, 350.0f, healthbar, 50.0f, 0.0f);

		// Update bullets again to ensure smooth movement
		UpdateBullets(&bulletHead, dt);

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















