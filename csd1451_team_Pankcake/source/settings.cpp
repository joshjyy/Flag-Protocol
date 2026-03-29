#include "screens.hpp"
#include "settings.hpp"
#include "utils.hpp"
#include "sound.hpp"

// ---------------------------------------------------------------------------
// Volume globals
// ---------------------------------------------------------------------------
float gMasterVolume = 1.0f;
float gMusicVolume = 1.0f;
float gSFXVolume = 1.0f;

// ---------------------------------------------------------------------------
// ApplyVolumes
// ---------------------------------------------------------------------------
void ApplyVolumes()
{
    float music = gMasterVolume * gMusicVolume;
    AEAudioSetGroupVolume(GameBGMGroup, music);
    AEAudioSetGroupVolume(MainBGMGroup, music);
    AEAudioSetGroupVolume(EditorBGMGroup, music);
    AEAudioSetGroupVolume(StaticBGMGroup, music);
    AEAudioSetGroupVolume(WinBGMGroup, music);
    AEAudioSetGroupVolume(LoseBGMGroup, music);

    float sfx = gMasterVolume * gSFXVolume;
    AEAudioSetGroupVolume(gSFXGroup, sfx);
    AEAudioSetGroupVolume(gSprintGroup, sfx);
}

// ---------------------------------------------------------------------------
// SaveSettings / LoadSettings
// ---------------------------------------------------------------------------
void SaveSettings()
{
    FILE* f;
    fopen_s(&f, "Assets/settings.txt", "w");
    if (!f) return;
    fprintf(f, "%f %f %f\n", gMasterVolume, gMusicVolume, gSFXVolume);
    fclose(f);
}

void LoadSettings()
{
    FILE* f;
    fopen_s(&f, "Assets/settings.txt", "r");
    if (!f)
    {
        // No settings file yet – keep the 1.0f defaults and apply them
        ApplyVolumes();
        return;
    }
    fscanf_s(f, "%f %f %f", &gMasterVolume, &gMusicVolume, &gSFXVolume);
    fclose(f);

    // Clamp in case the file is corrupted or from a bad testing session
    auto Clamp01 = [](float v) { return v < 0.0f ? 0.0f : v > 1.0f ? 1.0f : v; };
    gMasterVolume = Clamp01(gMasterVolume);
    gMusicVolume = Clamp01(gMusicVolume);
    gSFXVolume = Clamp01(gSFXVolume);

    ApplyVolumes();
}

// ---------------------------------------------------------------------------
// DrawSlider (file-local helper)
//
// sliderID    – unique int per slider (0, 1, 2 …)
// activeSlider – which slider currently owns the mouse drag (-1 = none)
// ---------------------------------------------------------------------------
static float DrawSlider(AEGfxVertexList* mesh, s8 font,
    const char* label,
    float trackCX, float trackCY, float trackW,
    float value,
    int sliderID, int& activeSlider)
{
    const float TRACK_H = 8.0f;
    const float KNOB_R = 13.0f;
    const float GAP = 18.0f;

    // track background
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(0.08f, 0.10f, 0.08f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    DrawSprite(mesh, trackCX, trackCY, trackW, TRACK_H, 0.0f);

    // filled portion
    float filledW = trackW * value;
    float filledX = (trackCX - trackW * 0.5f) + filledW * 0.5f;
    AEGfxSetColorToMultiply(0.0f, 0.85f, 0.25f, 1.0f);
    DrawSprite(mesh, filledX, trackCY, filledW, TRACK_H, 0.0f);

    // knob
    float knobX = (trackCX - trackW * 0.5f) + trackW * value;
    AEGfxSetColorToMultiply(0.85f, 1.0f, 0.85f, 1.0f);
    DrawSprite(mesh, knobX, trackCY, KNOB_R * 2.0f, KNOB_R * 2.0f, 0.0f);

    // label (left of track)
    float tw, th;
    AEGfxGetPrintSize(font, label, 1.0f, &tw, &th);
    float lx = (trackCX - trackW * 0.5f - GAP) / 800.0f - tw;
    float ly = trackCY / 450.0f - th * 0.5f;
    AEGfxPrint(font, const_cast<char*>(label),
        lx, ly, 1.0f, 0.75f, 1.0f, 0.75f, 1.0f);

    // percentage (right of track)
    char pct[8];
    sprintf_s(pct, "%3d%%", (int)(value * 100.0f + 0.5f));
    AEGfxGetPrintSize(font, pct, 1.0f, &tw, &th);
    float px = (trackCX + trackW * 0.5f + GAP) / 800.0f;
    float py = trackCY / 450.0f - th * 0.5f;
    AEGfxPrint(font, pct, px, py, 1.0f, 0.55f, 1.0f, 0.55f, 1.0f);

    // mouse drag – locked to the slider that was clicked first
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);
    float cx = (float)mx - 800.0f;
    float cy = 450.0f - (float)my;

    bool overTrack = (cx >= trackCX - trackW * 0.5f - KNOB_R) &&
        (cx <= trackCX + trackW * 0.5f + KNOB_R) &&
        (cy >= trackCY - KNOB_R * 2.0f) &&
        (cy <= trackCY + KNOB_R * 2.0f);

    // claim ownership on the frame the button is first pressed
    if (overTrack && AEInputCheckTriggered(AEVK_LBUTTON))
        activeSlider = sliderID;

    // release ownership when button is released
    if (!AEInputCheckCurr(AEVK_LBUTTON))
        activeSlider = -1;

    // only move if we own the drag
    if (activeSlider == sliderID && AEInputCheckCurr(AEVK_LBUTTON))
    {
        float raw = (cx - (trackCX - trackW * 0.5f)) / trackW;
        if (raw < 0.0f) raw = 0.0f;
        if (raw > 1.0f) raw = 1.0f;
        return raw;
    }
    return value;
}

// ---------------------------------------------------------------------------
// UpdateAndRenderSettings
// ---------------------------------------------------------------------------
GameState UpdateAndRenderSettings(AEGfxVertexList* mesh, s8 font, float dt)
{
    (void)dt;

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

    // translucent panel
    AEGfxSetTransparency(0.5f);
    AEGfxSetColorToMultiply(0.04f, 0.06f, 0.04f, 1.0f);
    DrawSprite(mesh, 0.0f, 0.0f, 680.0f, 600.0f, 0.0f);
    AEGfxSetTransparency(1.0f);

    // green accent line at top of panel
    AEGfxSetColorToMultiply(0.0f, 0.8f, 0.2f, 1.0f);
    DrawSprite(mesh, 0.0f, 275.0f, 680.0f, 4.0f, 0.0f);

    // title
    float tw, th;
    AEGfxGetPrintSize(font, "SETTINGS", 2.0f, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("SETTINGS"),
        -tw / 2.0f, 0.55f, 2.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    // sliders – centred on screen (TRACK_X = 0 means track centre is at x=0)
    const float TRACK_W = 400.0f;
    const float TRACK_X = 0.0f;
    static int activeSlider = -1;

    float newMaster = DrawSlider(mesh, font, "MASTER", TRACK_X, 100.0f, TRACK_W, gMasterVolume, 0, activeSlider);
    if (newMaster != gMasterVolume) { gMasterVolume = newMaster; ApplyVolumes(); }

    float newMusic = DrawSlider(mesh, font, "MUSIC", TRACK_X, -30.0f, TRACK_W, gMusicVolume, 1, activeSlider);
    if (newMusic != gMusicVolume) { gMusicVolume = newMusic;  ApplyVolumes(); }

    float newSFX = DrawSlider(mesh, font, "SFX", TRACK_X, -160.0f, TRACK_W, gSFXVolume, 2, activeSlider);
    if (newSFX != gSFXVolume) { gSFXVolume = newSFX;    ApplyVolumes(); }

    // divider
    AEGfxSetTransparency(0.3f);
    AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);
    DrawSprite(mesh, 0.0f, -220.0f, 600.0f, 2.0f, 0.0f);
    AEGfxSetTransparency(1.0f);

    // single BACK button (auto-saves)
    Button backBtn = { 0.0f, -270.0f, 200.0f, 50.0f, "BACK" };
    DrawButton(backBtn, mesh, font);

    if (IsButtonClicked(backBtn) || AEInputCheckTriggered(AEVK_ESCAPE))
    {
        SaveSettings();
        return STATE_MENU;
    }

    return STATE_SETTINGS;
}