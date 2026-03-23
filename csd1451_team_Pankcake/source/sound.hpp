#include "AEEngine.h" 

extern AEAudio gDashSound;
extern AEAudio gSprintSound;
extern AEAudio gDeathSound;
extern AEAudio gShoot1Sound;
extern AEAudio gPickUpSound;
extern AEAudio gDropSound;
extern AEAudio gLowHealthBeepSound;
extern AEAudio gHealSound;

extern AEAudio EDeathSound;

extern AEAudio LoseOST;
extern AEAudio WinOST;
extern AEAudio StaticSound;
extern AEAudio GameBGM1;
extern AEAudio MainMenuBGM;
extern AEAudio EditorBGM;

extern AEAudioGroup gSFXGroup;
extern AEAudioGroup GameBGMGroup;
extern AEAudioGroup gSprintGroup;
extern AEAudioGroup StaticBGMGroup;
extern AEAudioGroup MainBGMGroup;
extern AEAudioGroup EditorBGMGroup;
extern AEAudioGroup LoseBGMGroup;
extern AEAudioGroup WinBGMGroup;


void SoundInit();
void SoundFree();