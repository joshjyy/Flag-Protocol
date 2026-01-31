#include "events.hpp"
#include "AEEngine.h"
#include "utils.hpp"

EventManager InitEventManager(void) {
    EventManager events = {
        false  // winTriggered
    };
    return events;
}

void CheckWinCondition(EventManager& events, Player& player) {
    // Check if player killed 3 grunts AND hasn't already won
    if (player.gruntsKilled >= 3 && !events.winTriggered) {
        events.winTriggered = true;  // Mark as triggered
        printf("=== YOU WIN! ===\n");
    }

    // If win condition met, display "YOU WIN" on screen
    if (events.winTriggered) {
        float textX, textY;
		//Print Win Text
        GetNormalizedTextPosition(-100.0f, -150.0f, &textX, &textY);
        AEGfxPrint(fontID, "YOU WIN!", textX, textY, 3.0f, 0.0f, 1.0f, 0.0f, 1.0f);

        // Score Print
        char scoreText[64];
        sprintf_s(scoreText, 64, "Grunts Killed: %d", player.gruntsKilled);
        GetNormalizedTextPosition(-75.0f, -200.0f, &textX, &textY);
        AEGfxPrint(fontID, scoreText, textX, textY, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
}