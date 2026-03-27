/*!************************************************************************
  \file sound.cpp
  \author
  Yeo Kah Boon Joshua





  \brief
  This source file contains definitions of the stuff related to the audio.

**************************************************************************/

#include "AEEngine.h" 
#include "sound.hpp"

AEAudio gDashSound;
AEAudio gSprintSound;
AEAudio gDeathSound;
AEAudio gShoot1Sound;
AEAudio gPickUpSound;
AEAudio gDropSound;
AEAudio gLowHealthBeepSound;


AEAudio LoseOST;
AEAudio WinOST;
AEAudio StaticSound;
AEAudio GameBGM1;
AEAudio MainMenuBGM;
AEAudio EditorBGM;
AEAudio gHealSound;

AEAudio EDeathSound;

AEAudioGroup gSFXGroup;
AEAudioGroup gSprintGroup;
AEAudioGroup GameBGMGroup;
AEAudioGroup StaticBGMGroup;
AEAudioGroup MainBGMGroup;
AEAudioGroup EditorBGMGroup;
AEAudioGroup LoseBGMGroup;
AEAudioGroup WinBGMGroup;


void SoundInit()
{
    gSFXGroup = AEAudioCreateGroup();
    GameBGMGroup = AEAudioCreateGroup();
	gSprintGroup = AEAudioCreateGroup();
	StaticBGMGroup = AEAudioCreateGroup();
	MainBGMGroup = AEAudioCreateGroup();
    EditorBGMGroup = AEAudioCreateGroup();
	LoseBGMGroup = AEAudioCreateGroup();
	WinBGMGroup = AEAudioCreateGroup();
    

    //Player Sound
    gDashSound = AEAudioLoadSound("Assets/Sound/dash.wav");
    gSprintSound = AEAudioLoadSound("Assets/Sound/speed_boost.wav");
    gDeathSound = AEAudioLoadSound("Assets/Sound/death.wav");
	gShoot1Sound = AEAudioLoadSound("Assets/Sound/gun1.wav");
	gPickUpSound = AEAudioLoadSound("Assets/Sound/flagpickup.wav");
	gDropSound = AEAudioLoadSound("Assets/Sound/flagdrop.wav");
	gLowHealthBeepSound = AEAudioLoadSound("Assets/Sound/low_health.wav");
	gHealSound = AEAudioLoadSound("Assets/Sound/healup.mp3");

	StaticSound = AEAudioLoadMusic("Assets/Sound/static.wav");

	//Enemy death sound
	EDeathSound = AEAudioLoadMusic("Assets/Sound/EnemyDeath.wav");

    //BGM
	GameBGM1 = AEAudioLoadMusic("Assets/Sound/gameOST.wav");
    MainMenuBGM = AEAudioLoadMusic("Assets/Sound/mainOST.wav");
    EditorBGM = AEAudioLoadMusic("Assets/Sound/editor.wav");
	WinOST = AEAudioLoadMusic("Assets/Sound/winOST.wav");
	LoseOST = AEAudioLoadMusic("Assets/Sound/loseOST.wav");
}

void SoundFree()
{
    AEAudioUnloadAudio(gDashSound);
    AEAudioUnloadAudio(gSprintSound);
    AEAudioUnloadAudio(gDeathSound);
	AEAudioUnloadAudio(gShoot1Sound);
    AEAudioUnloadAudio(gPickUpSound);
	AEAudioUnloadAudio(gDropSound);
	AEAudioUnloadAudio(gLowHealthBeepSound);
	AEAudioUnloadAudio(gHealSound);

	AEAudioUnloadAudio(EDeathSound);

	AEAudioUnloadAudio(StaticSound);
	AEAudioUnloadAudio(MainMenuBGM);
    AEAudioUnloadAudio(EditorBGM);
	AEAudioUnloadAudio(WinOST);
	AEAudioUnloadAudio(LoseOST);

    AEAudioUnloadAudio(GameBGM1);
}


//parameters: sound, group, volume, pitch, loop
// volume: 0.0 - 1.0 | pitch: 1.0 = normal | loop: 0 = no loop
//AEAudioPlay(gDashSound, gSFXGroup, 1.0f, 1.0f, 0);


//Stop sound: AEAudioStopGroup(GameBGM1);
