#ifndef enemies_H
#define enemies_H

#include "gunTypes.hpp"
#include "player.hpp"
#include <vector>


struct Grunt {
	float health;
	float armour;
	// Starting Position
	float startX;
	float startY;

	// Real Time Position
	float movingX;
	float movingY;

	Pistol pistol;
	float shootCooldown;

	Grunt(float x, float y) {
		health = 100.0f;
		armour = 0.0f;
		startX = x;
		startY = y;
		movingX = x;
		movingY = y;
		pistol = Pistol();
		shootCooldown = 0.0f;
	}
};

// Function declarations
Grunt SpawnGrunt(float startX, float startY);
void UpdateGrunts(std::vector<Grunt>& grunts, Player& player, std::vector<Bullet>& enemyBullets, float dt);
void RenderGrunts(std::vector<Grunt>& grunts, void* mesh);

#endif