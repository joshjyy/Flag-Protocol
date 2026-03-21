#include "AEEngine.h" 
#include "sound.hpp"

AEAudio gDashSound;
AEAudio gSprintSound;
AEAudio gDeathSound;
AEAudio gShoot1Sound;
AEAudio StaticSound;
AEAudio GameBGM1;
AEAudio MainMenuBGM;
AEAudio EditorBGM;


AEAudioGroup gSFXGroup;
AEAudioGroup gSprintGroup;
AEAudioGroup GameBGMGroup;
AEAudioGroup StaticBGMGroup;
AEAudioGroup MainBGMGroup;
AEAudioGroup EditorBGMGroup;


void SoundInit()
{
    gSFXGroup = AEAudioCreateGroup();
    GameBGMGroup = AEAudioCreateGroup();
	gSprintGroup = AEAudioCreateGroup();
	StaticBGMGroup = AEAudioCreateGroup();
	MainBGMGroup = AEAudioCreateGroup();
    EditorBGMGroup = AEAudioCreateGroup();
    

    //Player Sound
    gDashSound = AEAudioLoadSound("Assets/Sound/dash.wav");
    gSprintSound = AEAudioLoadSound("Assets/Sound/speed_boost.wav");
    gDeathSound = AEAudioLoadSound("Assets/Sound/death.wav");
	gShoot1Sound = AEAudioLoadSound("Assets/Sound/gun1.wav");

	StaticSound = AEAudioLoadMusic("Assets/Sound/static.wav");

    //BGM
	GameBGM1 = AEAudioLoadMusic("Assets/Sound/gameOST.wav");
    MainMenuBGM = AEAudioLoadMusic("Assets/Sound/mainOST.wav");
    EditorBGM = AEAudioLoadMusic("Assets/Sound/editor.wav");
}

void SoundFree()
{
    AEAudioUnloadAudio(gDashSound);
    AEAudioUnloadAudio(gSprintSound);
    AEAudioUnloadAudio(gDeathSound);
	AEAudioUnloadAudio(gShoot1Sound);

	AEAudioUnloadAudio(StaticSound);
	AEAudioUnloadAudio(MainMenuBGM);
    AEAudioUnloadAudio(EditorBGM);

    AEAudioUnloadAudio(GameBGM1);
}


//parameters: sound, group, volume, pitch, loop
// volume: 0.0 - 1.0 | pitch: 1.0 = normal | loop: 0 = no loop
//AEAudioPlay(gDashSound, gSFXGroup, 1.0f, 1.0f, 0);


//Stop sound: AEAudioStopGroup(GameBGM1);
