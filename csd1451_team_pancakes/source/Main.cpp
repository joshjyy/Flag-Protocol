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

	// Bullet management
	Bullet* bulletHead = nullptr;  // Head of bullet linked list

	//float shootCooldown = 0.0f;
	//const float SHOOT_DELAY = 0.05f;  
	//const float BULLET_SPEED = 1000.0f;


	Player player = {
		1000.0f,  // health
		0.0f,  // x
		0.0f,  // y
		0.0f,  // dashTimer
		0.0f,  // dashCooldownTimer
		0.0f,  // dashDirX
		0.0f,   // dashDirY
		Pistol(), // pistol constructor
		AssaultRifle(), // rifle constructor
		0,        // currentWeaponIndex (start with pistol)
		0.0f      // shootCooldown
	};


	//Reloading variables
	//To add CD to reload so Spamming R does not feel like a cheat
	float reloadCD = 0.f;

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
		
		//Check what weapon the player is using once per frame
		WeaponStats weapon = GetCurrentWeapon(player);
		// Exit game
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;
		
		/// Update timers
		if (player.dashTimer > 0.0f) player.dashTimer -= dt;
		if (player.dashCooldownTimer > 0.0f) player.dashCooldownTimer -= dt;

		if (AEInputCheckTriggered(AEVK_SPACE) && player.dashCooldownTimer <= 0.0f) {
			// Get current movement direction from WASD
			float inputX = 0.0f;
			float inputY = 0.0f;
			if (AEInputCheckCurr(AEVK_W)) inputY += 1.0f;
			if (AEInputCheckCurr(AEVK_S)) inputY -= 1.0f;
			if (AEInputCheckCurr(AEVK_D)) inputX += 1.0f;
			if (AEInputCheckCurr(AEVK_A)) inputX -= 1.0f;

			// Normalize direction
			float length = sqrtf(inputX * inputX + inputY * inputY);
			if (length > 0.0f) {
				player.dashDirX = inputX / length;
				player.dashDirY = inputY / length;
				player.dashTimer = Player::dashDuration;
				player.dashCooldownTimer = Player::dashCooldownMax;
			}
		}

		// Apply movement
		if (player.dashTimer > 0.0f) {
			// During dash - locked into dash direction
			player.x += player.dashDirX * Player::dashSpeed * dt;
			player.y += player.dashDirY * Player::dashSpeed * dt;
		}
		else {
			// Normal WASD movement
			if (AEInputCheckCurr(AEVK_W)) {
				player.y += Player::walkSpeed * dt;
			}
			if (AEInputCheckCurr(AEVK_A)) {
				player.x -= Player::walkSpeed * dt;
			}
			if (AEInputCheckCurr(AEVK_S)) {
				player.y -= Player::walkSpeed * dt;
			}
			if (AEInputCheckCurr(AEVK_D)) {
				player.x += Player::walkSpeed * dt;
			}
		}

		//Weapon Switching ( 1 = Pistol, 2 = Assault Rifle)
		// JUST FOR TESTING PURPOSES, WILL IMPLEMENT A BETTER SYSTEM LATER
	
		if (AEInputCheckTriggered(AEVK_1)) {
			player.currentWeaponIndex = 0;  // Switch to Pistol
			printf("Pistol Switched! %d\n", player.currentWeaponIndex);

		}
		if (AEInputCheckTriggered(AEVK_2)) {
			player.currentWeaponIndex = 1;  // Switch to Assault Rifle
			printf("Rifle Switched! %d\n", player.currentWeaponIndex);
		}



		// Update shoot cooldown
		if (player.shootCooldown > 0.0f) {
			player.shootCooldown -= dt;
		}

		// Shooting
		if (AEInputCheckCurr(AEVK_LBUTTON) && player.shootCooldown <= 0.0f ) {
			

			if (*weapon.currentAmmo > 0) {

				// Get mouse position in world coordinates
				s32 mouseX, mouseY;
				AEInputGetCursorPosition(&mouseX, &mouseY);

				float worldMouseX = (float)mouseX - 800.0f;
				float worldMouseY = 450.0f - (float)mouseY;

				// Create bullet with current weapon stats
				Bullet* newBullet = CreateBullet(player.x, player.y, worldMouseX, worldMouseY,
					weapon.bulletSpeed, weapon.bulletDamage);

				// Add to linked list
				newBullet->next = bulletHead;
				bulletHead = newBullet;

				// Decrease ammo
				(*weapon.currentAmmo)--;

				// Reset cooldown with current weapon's fire rate
				player.shootCooldown = weapon.fireRate;
			}
		}

		// // Basically counts the CD down to 0 >> the time taken to reload
		if (reloadCD > 0.f) {
			reloadCD -= dt;
			//Once the reload is completed, change current ammo count to max ammo count
			if (reloadCD <= 0.f) {
				*weapon.currentAmmo = *weapon.maxAmmo;
			}

		}

		if (AEInputCheckTriggered(AEVK_R) && reloadCD <= 0.f && *weapon.currentAmmo < *weapon.maxAmmo) {
			reloadCD = weapon.reloadSpeed;
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
		DrawSprite(circleMesh, player.x, player.y, 100.0f, 100.0f, 0.0f);

		// Draw bullets
		RenderBullets(bulletHead, circleMesh);

		
		//Draw Background Health Bar
		AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f);
		DrawSprite(squareMesh, 0.0f, 350.0f, 1000.0f, 50.0f, 0.0f);
		//Draw Current Health
		AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
		float healthbarOffset = (1000.0f - player.health) / 2.0f;
		DrawSprite(squareMesh, 0.0f - healthbarOffset, 350.0f, player.health, 50.0f, 0.0f);



	




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
	FreeBullets(&bulletHead);


	AEGfxMeshFree(squareMesh);
	AEGfxMeshFree(circleMesh);
	AESysExit();

	return 0;
}















