#pragma once
#include "player.hpp"
#include <vector>

struct EventManager {
	bool winTriggered;     // Demo Win Condition is player.gruntsKilled >= 3
};

//Initialize the event manager
EventManager InitEventManager(void);

// Check if player kill 3 grunts and print win text
void CheckWinCondition(EventManager& events, Player& player);