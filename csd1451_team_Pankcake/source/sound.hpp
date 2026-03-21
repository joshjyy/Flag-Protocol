#include "AEEngine.h" 

extern AEAudio gDashSound;
extern AEAudio gSprintSound;
extern AEAudio gDeathSound;
extern AEAudio gShoot1Sound;
extern AEAudio StaticSound;
extern AEAudio GameBGM1;
extern AEAudio MainMenuBGM;

extern AEAudioGroup gSFXGroup;
extern AEAudioGroup GameBGMGroup;
extern AEAudioGroup gSprintGroup;
extern AEAudioGroup StaticBGMGroup;
extern AEAudioGroup MainBGMGroup;

void SoundInit();
void SoundFree();