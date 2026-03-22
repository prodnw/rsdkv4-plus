#include "RetroEngine.hpp"

int stageListCount[STAGELIST_MAX];
char stageListNames[STAGELIST_MAX][0x20] = {
    "Presentation Stages",
    "Regular Stages",
    "Bonus Stages",
    "Special Stages",
};
SceneInfo stageList[STAGELIST_MAX][0x100];

int stageMode = STAGEMODE_LOAD;

Camera camera[DEFAULT_CAMERA_COUNT];
int currentCamera = 0;
int cameraCount   = 1;
int &curCam       = currentCamera; // shorter name (if needed)

int SCREEN_SCROLL_LEFT  = SCREEN_CENTERX - 8;
int SCREEN_SCROLL_RIGHT = SCREEN_CENTERX + 8;

int lastYSize = -1;
int lastXSize = -1;

bool pauseEnabled     = true;
bool timeEnabled      = true;
bool debugMode        = false;
int frameCounter      = 0;
int stageMilliseconds = 0;
int stageSeconds      = 0;
int stageMinutes      = 0;

// Category and Scene IDs
int activeStageList   = 0;
int stageListPosition = 0;
char currentStageFolder[0x100];
int actID = 0;

char titleCardText[0x100];
byte titleCardWord2 = 0;

byte activeTileLayers[4];
byte tLayerMidPoint;
TileLayer stageLayouts[LAYER_COUNT];

int bgDeformationData0[DEFORM_COUNT];
int bgDeformationData1[DEFORM_COUNT];
int bgDeformationData2[DEFORM_COUNT];
int bgDeformationData3[DEFORM_COUNT];

LineScroll hParallax;
LineScroll vParallax;

Tiles128x128 tiles128x128;
CollisionMasks collisionMasks[2];

byte tilesetGFXData[TILESET_SIZE];

ushort tile3DFloorBuffer[0x100 * 0x100];
bool drawStageGFXHQ = false;

#if RETRO_USE_MOD_LOADER
bool loadGlobalScripts = false; // stored here so I can use it later
int globalObjCount     = 0;
#endif

void InitFirstStage(void)
{
    for (int c = 0; c < DEFAULT_CAMERA_COUNT; ++c) {
        camera[c].xScrollOffset = 0;
        camera[c].yScrollOffset = 0;
    }
    StopMusic(true);
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode = 0;
    ClearGraphicsData();
    ClearAnimationData();
    activePalette     = fullPalette[0];
    activePalette32   = fullPalette32[0];
    stageMode         = STAGEMODE_LOAD;
    Engine.gameMode   = ENGINE_MAINGAME;
    activeStageList   = 0;
    stageListPosition = 0;
}

void InitStartingStage(int list, int stage, int player)
{
    for (int c = 0; c < DEFAULT_CAMERA_COUNT; ++c) {
        camera[c].xScrollOffset = 0;
        camera[c].yScrollOffset = 0;
    }
    StopMusic(true);
    StopAllSfx();
    ReleaseStageSfx();
    fadeMode      = 0;
    playerListPos = player;
    ClearGraphicsData();
    ClearAnimationData();
    ResetCurrentStageFolder();
    activeStageList   = list;
    activePalette     = fullPalette[0];
    activePalette32   = fullPalette32[0];
    stageMode         = STAGEMODE_LOAD;
    Engine.gameMode   = ENGINE_MAINGAME;
    stageListPosition = stage;
}

void ProcessStage(void)
{
#if !RETRO_USE_ORIGINAL_CODE
    debugHitboxCount = 0;
#endif

#if RETRO_USE_DISCORD_SDK
    API_Discord_Update();
#endif

    switch (stageMode) {
        case STAGEMODE_LOAD: // Startup
			ClearGraphicsData();
			ClearAnimationData();
            SetActivePalette(0, 0, 256);
            gameMenu[0].visibleRowOffset = 0;
            gameMenu[1].alignment        = 0;
            gameMenu[1].selectionCount   = 0;
            fadeMode                     = 0;
            
            for (int c = 0; c < DEFAULT_CAMERA_COUNT; ++c) {
                camera[c].target         = -1;
                camera[c].style          = CAMERASTYLE_FOLLOW;
                camera[c].enabled        = true;
                camera[c].adjustY        = 0;
                camera[c].xScrollOffset  = 0;
                camera[c].yScrollOffset  = 0;
                camera[c].xpos           = 0;
                camera[c].ypos           = 0;
                camera[c].shakeX         = 0;
                camera[c].shakeY         = 0;
                camera[c].shift          = 0;
                camera[c].lockedY        = 0;
            }
            
            currentCamera                = 0;
            vertexCount                  = 0;
            faceCount                    = 0;
            frameCounter                 = 0;
            pauseEnabled                 = false;
            timeEnabled                  = false;
            stageMilliseconds            = 0;
            stageSeconds                 = 0;
            stageMinutes                 = 0;
            stageMode                    = STAGEMODE_NORMAL;

#if RSDK_AUTOBUILD // Now it works with various games. TODO: update this if it needs to, but it should be fine?
            if (!hasPlusDLC) {
                switch (Engine.gameType) {
                    // Forever and Absolute has their own Amy, so....
                    case GAME_SONIC1:
                    case GAME_SONIC2:
                    case GAME_SONIC3: // prevent players from using Amy without DLC.
                        if (GetGlobalVariableByName("PLAYER_AMY") && playerListPos == GetGlobalVariableByName("PLAYER_AMY"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_AMY_TAILS") && playerListPos == GetGlobalVariableByName("PLAYER_AMY_TAILS"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_AMY") && (GetGlobalVariableByName("stage.player2Enabled")))
                            playerListPos = 0;
                        break;

                    case GAME_SONICCD: // prevent players from using Knuckles or Amy without DLC, like OG Sonic CD would.
                        if (GetGlobalVariableByName("PLAYER_KNUCKLES") && playerListPos == GetGlobalVariableByName("PLAYER_KNUCKLES"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_KNUCKLES_TAILS") && playerListPos == GetGlobalVariableByName("PLAYER_KNUCKLES_TAILS"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_AMY") && playerListPos == GetGlobalVariableByName("PLAYER_AMY"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_AMY_TAILS") && playerListPos == GetGlobalVariableByName("PLAYER_AMY_TAILS"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_KNUCKLES") && playerListPos == GetGlobalVariableByName("PLAYER_KNUCKLES")
                                 && GetGlobalVariableByName("stage.player2Enabled"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_AMY") && playerListPos == GetGlobalVariableByName("PLAYER_AMY")
                                 && GetGlobalVariableByName("stage.player2Enabled"))
                            playerListPos = 0;
                        break;
                        
                    case GAME_SONICCDINFINITE: // prevent players from using Knuckles without DLC
                        if (GetGlobalVariableByName("PLAYER_KNUCKLES") && playerListPos == GetGlobalVariableByName("PLAYER_KNUCKLES"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_KNUCKLES_TAILS") && playerListPos == GetGlobalVariableByName("PLAYER_KNUCKLES_TAILS"))
                            playerListPos = 0;
                        else if (GetGlobalVariableByName("PLAYER_KNUCKLES") && playerListPos == GetGlobalVariableByName("PLAYER_KNUCKLES")
                                 && GetGlobalVariableByName("stage.player2Enabled"))
                            playerListPos = 0;
                        break;
                }
            }
#endif


#if RETRO_USE_MOD_LOADER
            for (int m = 0; m < modList.size(); ++m) ScanModFolder(&modList[m]);
#endif
            ResetBackgroundSettings();
            LoadStageFiles();

#if RETRO_HARDWARE_RENDER
            texBufferMode = 0;
            for (int i = 0; i < LAYER_COUNT; i++) {
                if (stageLayouts[i].type == LAYER_3DSKY)
                    texBufferMode = 1;
            }
            for (int i = 0; i < hParallax.entryCount; i++) {
                if (hParallax.deform[i])
                    texBufferMode = 1;
            }

            if (tilesetGFXData[0x32002] > 0)
                texBufferMode = 0;

            if (texBufferMode) {
                for (int i = 0; i < TILEUV_SIZE; i += 4) {
                    tileUVArray[i + 0] = (i >> 2) % 28 * 18 + 1;
                    tileUVArray[i + 1] = (i >> 2) / 28 * 18 + 1;
                    tileUVArray[i + 2] = tileUVArray[i + 0] + 16;
                    tileUVArray[i + 3] = tileUVArray[i + 1] + 16;
                }
                tileUVArray[TILEUV_SIZE - 4] = 487.0f;
                tileUVArray[TILEUV_SIZE - 3] = 487.0f;
                tileUVArray[TILEUV_SIZE - 2] = 503.0f;
                tileUVArray[TILEUV_SIZE - 1] = 503.0f;
            }
            else {
                for (int i = 0; i < TILEUV_SIZE; i += 4) {
                    tileUVArray[i + 0] = (i >> 2 & 31) * 16;
                    tileUVArray[i + 1] = (i >> 2 >> 5) * 16;
                    tileUVArray[i + 2] = tileUVArray[i + 0] + 16;
                    tileUVArray[i + 3] = tileUVArray[i + 1] + 16;
                }
            }

            UpdateHardwareTextures();
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
#endif
            break;

        case STAGEMODE_NORMAL:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode = 0;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(keyDown);
            CheckKeyPress(keyPress);

            if (timeEnabled) {
                if (++frameCounter == 60) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        ++stageMinutes;
                    }
                }
                stageMilliseconds = 100 * frameCounter / 60;
            }
            else {
                frameCounter = 60 * stageMilliseconds / 100;
            }

            // Update
            currentCamera = 0;
            ProcessObjects();
            ProcessParallaxAutoScroll();

            for (currentCamera = 0; currentCamera < cameraCount; ++currentCamera) {
                if (camera[currentCamera].target > -1) {
                    if (camera[currentCamera].enabled == true) {
                        switch (camera[currentCamera].style) {
                            case CAMERASTYLE_FOLLOW: SetPlayerScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
                            case CAMERASTYLE_EXTENDED:
                            case CAMERASTYLE_EXTENDED_OFFSET_L:
                            case CAMERASTYLE_EXTENDED_OFFSET_R: SetPlayerScreenPositionCDStyle(&objectEntityList[camera[currentCamera].target]); break;
                            case CAMERASTYLE_HLOCKED: SetPlayerHLockedScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
                            default: break;
                        }
                    }
                    else {
                        SetPlayerLockedScreenPosition(&objectEntityList[camera[currentCamera].target]);
                    }
                }
                DrawStageGFX();
            }
            break;

        case STAGEMODE_PAUSED:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode = 0;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(keyDown);
            CheckKeyPress(keyPress);

            // Update
            currentCamera = 0;
            ProcessPausedObjects();

#if RETRO_HARDWARE_RENDER
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
#endif

            for (currentCamera = 0; currentCamera < cameraCount; ++currentCamera) {
                FlipFrameBuffer(camera[currentCamera].direction);
                DrawObjectList(0);
                DrawObjectList(1);
                DrawObjectList(2);
                DrawObjectList(3);
                DrawObjectList(4);
                DrawObjectList(5);
#if RETRO_REV03
#if !RETRO_USE_ORIGINAL_CODE
                // Hacky fix for Tails Object not working properly in special stages on non-Origins bytecode
                if (forceUseScripts || Engine.usingOrigins)
#endif
                    DrawObjectList(7);
#endif
                DrawObjectList(6);
                FlipFrameBuffer(camera[currentCamera].direction);

#if !RETRO_USE_ORIGINAL_CODE
                DrawDebugOverlays();
#endif
            }
            break;

        case STAGEMODE_FROZEN:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode = 0;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(keyDown);
            CheckKeyPress(keyPress);

            // Update
            currentCamera = 0;
            ProcessFrozenObjects();

            for (currentCamera = 0; currentCamera < cameraCount; ++currentCamera) {
//                if (camera[currentCamera].target > -1) {
//                    if (camera[currentCamera].enabled == true) {
//                        switch (camera[currentCamera].style) {
//                            case CAMERASTYLE_FOLLOW: SetPlayerScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
//                            case CAMERASTYLE_EXTENDED:
//                            case CAMERASTYLE_EXTENDED_OFFSET_L:
//                            case CAMERASTYLE_EXTENDED_OFFSET_R: SetPlayerScreenPositionCDStyle(&objectEntityList[camera[currentCamera].target]); break;
//                            case CAMERASTYLE_HLOCKED: SetPlayerHLockedScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
//                            default: break;
//                        }
//                    }
//                    else {
//                        SetPlayerLockedScreenPosition(&objectEntityList[camera[currentCamera].target]);
//                    }
//                }
                DrawStageGFX();
            }
            break;

        case STAGEMODE_2P:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode = 0;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(keyDown);
            CheckKeyPress(keyPress);

            if (timeEnabled) {
                if (++frameCounter == 60) {
                    frameCounter = 0;
                    if (++stageSeconds > 59) {
                        stageSeconds = 0;
                        ++stageMinutes;
                    }
                }
                stageMilliseconds = 100 * frameCounter / 60;
            }
            else {
                frameCounter = 60 * stageMilliseconds / 100;
            }

            // Update
            currentCamera = 0;
            Process2PObjects();
            ProcessParallaxAutoScroll();

            for (currentCamera = 0; currentCamera < cameraCount; ++currentCamera) {
                if (camera[currentCamera].target > -1) {
                    if (camera[currentCamera].enabled == true) {
                        switch (camera[currentCamera].style) {
                            case CAMERASTYLE_FOLLOW: SetPlayerScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
                            case CAMERASTYLE_EXTENDED:
                            case CAMERASTYLE_EXTENDED_OFFSET_L:
                            case CAMERASTYLE_EXTENDED_OFFSET_R: SetPlayerScreenPositionCDStyle(&objectEntityList[camera[currentCamera].target]); break;
                            case CAMERASTYLE_HLOCKED: SetPlayerHLockedScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
                            default: break;
                        }
                    }
                    else {
                        SetPlayerLockedScreenPosition(&objectEntityList[camera[currentCamera].target]);
                    }
                }
                DrawStageGFX();
            }
            break;

        case STAGEMODE_NORMAL_STEP:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode = 0;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(keyDown);
            CheckKeyPress(keyPress);

            if (keyPress[0].C) {
                keyPress[0].C = false;

                if (timeEnabled) {
                    if (++frameCounter == 60) {
                        frameCounter = 0;
                        if (++stageSeconds > 59) {
                            stageSeconds = 0;
                            ++stageMinutes;
                        }
                    }
                    stageMilliseconds = 100 * frameCounter / 60;
                }
                else {
                    frameCounter = 60 * stageMilliseconds / 100;
                }

                // Update
                currentCamera = 0;
                ProcessObjects();
                ProcessParallaxAutoScroll();

                for (currentCamera = 0; currentCamera < cameraCount; ++currentCamera) {
                    if (camera[currentCamera].target > -1) {
                        if (camera[currentCamera].enabled == true) {
                            switch (camera[currentCamera].style) {
                                case CAMERASTYLE_FOLLOW: SetPlayerScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
                                case CAMERASTYLE_EXTENDED:
                                case CAMERASTYLE_EXTENDED_OFFSET_L:
                                case CAMERASTYLE_EXTENDED_OFFSET_R: SetPlayerScreenPositionCDStyle(&objectEntityList[camera[currentCamera].target]); break;
                                case CAMERASTYLE_HLOCKED: SetPlayerHLockedScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
                                default: break;
                            }
                        }
                        else {
                            SetPlayerLockedScreenPosition(&objectEntityList[camera[currentCamera].target]);
                        }
                    }
                    DrawStageGFX();
                }
            }

            if (pauseEnabled && keyPress[0].start) {
                stageMode = STAGEMODE_NORMAL;
                ResumeSound();
            }
            break;

        case STAGEMODE_PAUSED_STEP:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode = 0;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(keyDown);
            CheckKeyPress(keyPress);

            if (keyPress[0].C) {
#if RETRO_HARDWARE_RENDER
                gfxIndexSize        = 0;
                gfxVertexSize       = 0;
                gfxIndexSizeOpaque  = 0;
                gfxVertexSizeOpaque = 0;
#endif

                keyPress[0].C = false;
                currentCamera = 0;
                ProcessPausedObjects();

                for (currentCamera = 0; currentCamera < cameraCount; ++currentCamera) {
                    FlipFrameBuffer(camera[currentCamera].direction);
                    DrawObjectList(0);
                    DrawObjectList(1);
                    DrawObjectList(2);
                    DrawObjectList(3);
                    DrawObjectList(4);
                    DrawObjectList(5);
#if RETRO_REV03
#if !RETRO_USE_ORIGINAL_CODE
                    // Hacky fix for Tails Object not working properly in special stages on non-Origins bytecode
                    if (forceUseScripts || Engine.usingOrigins)
#endif
                        DrawObjectList(7);
#endif
                    DrawObjectList(6);
                    FlipFrameBuffer(camera[currentCamera].direction);

#if !RETRO_USE_ORIGINAL_CODE
                    DrawDebugOverlays();
#endif
                }
            }

            if (pauseEnabled && keyPress[0].start) {
                stageMode = STAGEMODE_PAUSED;
                ResumeSound();
            }
            break;

        case STAGEMODE_FROZEN_STEP:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode = 0;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(keyDown);
            CheckKeyPress(keyPress);

            if (keyPress[0].C) {
                keyPress[0].C = false;

                // Update
                currentCamera = 0;
                ProcessFrozenObjects();
                for (currentCamera = 0; currentCamera < cameraCount; ++currentCamera) {
//                    if (camera[currentCamera].target > -1) {
//                        if (camera[currentCamera].enabled == true) {
//                            switch (camera[currentCamera].style) {
//                                case CAMERASTYLE_FOLLOW: SetPlayerScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
//                                case CAMERASTYLE_EXTENDED:
//                                case CAMERASTYLE_EXTENDED_OFFSET_L:
//                                case CAMERASTYLE_EXTENDED_OFFSET_R: SetPlayerScreenPositionCDStyle(&objectEntityList[camera[currentCamera].target]); break;
//                                case CAMERASTYLE_HLOCKED: SetPlayerHLockedScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
//                                default: break;
//                            }
//                        }
//                        else {
//                            SetPlayerLockedScreenPosition(&objectEntityList[camera[currentCamera].target]);
//                        }
//                    }
                    DrawStageGFX();
                }
            }
            if (pauseEnabled && keyPress[0].start) {
                stageMode = STAGEMODE_FROZEN;
                ResumeSound();
            }
            break;

        case STAGEMODE_2P_STEP:
            drawStageGFXHQ = false;
            if (fadeMode > 0)
                fadeMode = 0;

            lastXSize = -1;
            lastYSize = -1;
            CheckKeyDown(keyDown);
            CheckKeyPress(keyPress);
            if (keyPress[0].C) {
                keyPress[0].C = false;

                if (timeEnabled) {
                    if (++frameCounter == 60) {
                        frameCounter = 0;
                        if (++stageSeconds > 59) {
                            stageSeconds = 0;
                            ++stageMinutes;
                        }
                    }
                    stageMilliseconds = 100 * frameCounter / 60;
                }
                else {
                    frameCounter = 60 * stageMilliseconds / 100;
                }

                // Update
                currentCamera = 0;
                Process2PObjects();
                ProcessParallaxAutoScroll();

                for (currentCamera = 0; currentCamera < cameraCount; ++currentCamera) {
                    if (camera[currentCamera].target > -1) {
                        if (camera[currentCamera].enabled == true) {
                            switch (camera[currentCamera].style) {
                                case CAMERASTYLE_FOLLOW: SetPlayerScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
                                case CAMERASTYLE_EXTENDED:
                                case CAMERASTYLE_EXTENDED_OFFSET_L:
                                case CAMERASTYLE_EXTENDED_OFFSET_R: SetPlayerScreenPositionCDStyle(&objectEntityList[camera[currentCamera].target]); break;
                                case CAMERASTYLE_HLOCKED: SetPlayerHLockedScreenPosition(&objectEntityList[camera[currentCamera].target]); break;
                                default: break;
                            }
                        }
                        else {
                            SetPlayerLockedScreenPosition(&objectEntityList[camera[currentCamera].target]);
                        }
                    }
                    DrawStageGFX();
                }
            }

            if (pauseEnabled && keyPress[0].start) {
                stageMode = STAGEMODE_2P;
                ResumeSound();
            }
            break;
    }
}

void ProcessParallaxAutoScroll()
{
    for (int i = 0; i < hParallax.entryCount; ++i) hParallax.scrollPos[i] += hParallax.scrollSpeed[i];
    for (int i = 0; i < vParallax.entryCount; ++i) vParallax.scrollPos[i] += vParallax.scrollSpeed[i];
}

void LoadStageFiles(void)
{
    FileInfo infoStore;
    FileInfo info;
    byte fileBuffer  = 0;
    byte fileBuffer2 = 0;
    int scriptID     = 1;
    char strBuffer[0x100];

    StopAllSfx();
    //if (!CheckCurrentStageFolder(stageListPosition)) {
	//Nope. When I say LoadStage() you Load the fuckin Stage() ya piece o' shit.
	if (true) {
        PrintLog("Loading Scene %s - %s", stageListNames[activeStageList], stageList[activeStageList][stageListPosition].name);
        PrintLog("Playing Character %s - Player ID %d", playerNames[playerListPos].c_str(), playerListPos);
        ReleaseStageSfx();
        ClearScriptData();
        for (int i = SURFACE_COUNT; i > 0; i--) RemoveGraphicsFile((char *)"", i - 1);

#if RETRO_USE_MOD_LOADER
        loadGlobalScripts = false;
#else
        bool loadGlobalScripts = false;
#endif

        if (LoadStageFile("StageConfig.bin", stageListPosition, &info)) {
            byte buf = 0;
            FileRead(&buf, 1);
            loadGlobalScripts = buf;
            CloseFile();
        }

        if (LoadFile("Data/Game/GameConfig.bin", &info)) {
            byte globalObjectCount = 0;
            
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);
            FileRead(&fileBuffer, 1);
            FileRead(&strBuffer, fileBuffer);

            byte buf[3];
            for (int c = 0; c < 0x60; ++c) {
                FileRead(buf, 3);
                SetPaletteEntry(-1, c, buf[0], buf[1], buf[2]);
            }

			if (loadGlobalScripts) {
				FileRead(&globalObjectCount, 1);
				for (byte i = 0; i < globalObjectCount; ++i) {
					FileRead(&fileBuffer2, 1);
					FileRead(strBuffer, fileBuffer2);
					strBuffer[fileBuffer2] = 0;
					SetObjectTypeName(strBuffer, scriptID + i);
				}

#if RETRO_USE_MOD_LOADER && RETRO_USE_COMPILER
                for (byte i = 0; i < modObjCount && loadGlobalScripts; ++i) {
                    SetObjectTypeName(modTypeNames[i], globalObjectCount + i + 1);
                }
#endif

#if RETRO_USE_COMPILER
#if !RETRO_USE_ORIGINAL_CODE
                bool bytecodeExists = false;
                FileInfo bytecodeInfo;
                GetFileInfo(&bytecodeInfo);
                CloseFile();
                if (LoadFile("Bytecode/GlobalCode.bin", &info)) {
                    bytecodeExists = true;
                    CloseFile();
                }
                SetFileInfo(&bytecodeInfo);
    
                if (bytecodeExists && !forceUseScripts) {
#else
                if (Engine.usingBytecode) {
#endif
                    GetFileInfo(&infoStore);
                    CloseFile();
                    LoadBytecode(4, scriptID);
                    scriptID += globalObjectCount;
                    SetFileInfo(&infoStore);
                }
                else {
                    for (byte i = 0; i < globalObjectCount; ++i) {
                        FileRead(&fileBuffer2, 1);
                        FileRead(strBuffer, fileBuffer2);
                        strBuffer[fileBuffer2] = 0;
                        GetFileInfo(&infoStore);
                        CloseFile();
                        ParseScriptFile(strBuffer, scriptID++);
                        SetFileInfo(&infoStore);
                        if (Engine.gameMode == ENGINE_SCRIPTERROR)
                            return;
                     }
                }
#else
                GetFileInfo(&infoStore);
                CloseFile();
                LoadBytecode(4, scriptID);
                scriptID += globalObjectCount;
                SetFileInfo(&infoStore);
#endif
            }
            CloseFile();

#if RETRO_USE_MOD_LOADER
            Engine.LoadXMLPalettes();
#endif

#if RETRO_USE_MOD_LOADER && RETRO_USE_COMPILER
            globalObjCount = globalObjectCount;
            for (byte i = 0; i < modObjCount && loadGlobalScripts; ++i) {
                SetObjectTypeName(modTypeNames[i], scriptID);

                //SetFileInfo here was causing a crash when XML mods were mixed with non XML mods
                //so GET IT THE FUCK OUTTA HERE
                //GetFileInfo(&infoStore);
                CloseFile();
                ParseScriptFile(modScriptPaths[i], scriptID++);
                //SetFileInfo(&infoStore);
                if (Engine.gameMode == ENGINE_SCRIPTERROR)
                    return;
            }
#endif
        }

        if (LoadStageFile("StageConfig.bin", stageListPosition, &info)) {
            FileRead(&fileBuffer, 1); // Load Globals

            byte clr[3];
            for (int i = 0x60; i < 0x80; ++i) {
                FileRead(&clr, 3);
                SetPaletteEntry(-1, i, clr[0], clr[1], clr[2]);
            }

            FileRead(&fileBuffer, 1);
            stageSFXCount = fileBuffer;
            for (byte i = 0; i < stageSFXCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;

                SetSfxName(strBuffer, i + globalSFXCount);
            }
            for (byte i = 0; i < stageSFXCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                GetFileInfo(&infoStore);
                CloseFile();
                LoadSfx(strBuffer, globalSFXCount + i);
                SetFileInfo(&infoStore);
            }

            byte stageObjectCount = 0;
            FileRead(&stageObjectCount, 1);
            for (byte i = 0; i < stageObjectCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
                SetObjectTypeName(strBuffer, scriptID + i);
            }

#if RETRO_USE_COMPILER
#if !RETRO_USE_ORIGINAL_CODE
            char scriptPath[0x40];
            switch (activeStageList) {
                case STAGELIST_PRESENTATION:
                case STAGELIST_REGULAR:
                case STAGELIST_BONUS:
                case STAGELIST_SPECIAL:
                    StrCopy(scriptPath, "Bytecode/");
                    StrAdd(scriptPath, stageList[activeStageList][stageListPosition].folder);
                    StrAdd(scriptPath, ".bin");
                    break;
                default: break;
            }
            bool bytecodeExists = false;
            FileInfo bytecodeInfo;
            GetFileInfo(&bytecodeInfo);
            CloseFile();
            if (LoadFile(scriptPath, &info)) {
                bytecodeExists = true;
                CloseFile();
            }
            SetFileInfo(&bytecodeInfo);

            if (bytecodeExists && !forceUseScripts) {
#else
            if (Engine.usingBytecode) {
#endif
                for (byte i = 0; i < stageObjectCount; ++i) {
                    FileRead(&fileBuffer2, 1);
                    FileRead(strBuffer, fileBuffer2);
                    strBuffer[fileBuffer2] = 0;
                }
                GetFileInfo(&infoStore);
                CloseFile();
                LoadBytecode(activeStageList, scriptID);
                SetFileInfo(&infoStore);
            }
            else {
                for (byte i = 0; i < stageObjectCount; ++i) {
                    FileRead(&fileBuffer2, 1);
                    FileRead(strBuffer, fileBuffer2);
                    strBuffer[fileBuffer2] = 0;
                    GetFileInfo(&infoStore);
                    CloseFile();
                    ParseScriptFile(strBuffer, scriptID + i);
                    SetFileInfo(&infoStore);
                    if (Engine.gameMode == ENGINE_SCRIPTERROR)
                        return;
                }
            }
#else
            for (byte i = 0; i < stageObjectCount; ++i) {
                FileRead(&fileBuffer2, 1);
                FileRead(strBuffer, fileBuffer2);
                strBuffer[fileBuffer2] = 0;
            }
            GetFileInfo(&infoStore);
            CloseFile();
            LoadBytecode(activeStageList, scriptID);
            SetFileInfo(&infoStore);
#endif
            CloseFile();
        }

        LoadStageGIFFile(stageListPosition);
        LoadStageCollisions();
        LoadStageBackground();
    }
    else {
        PrintLog("Reloading Scene %s - %s", stageListNames[activeStageList], stageList[activeStageList][stageListPosition].name);
    }
    LoadStageChunks();
    for (int i = 0; i < TRACK_COUNT; ++i) SetMusicTrack("", i, false, 0);

    memset(objectEntityList, 0, ENTITY_COUNT * sizeof(Entity));
    for (int i = 0; i < ENTITY_COUNT; ++i) {
        objectEntityList[i].drawOrder          = 3;
        objectEntityList[i].scale              = 512;
        objectEntityList[i].xscale             = 512;
        objectEntityList[i].yscale             = 512;
        objectEntityList[i].objectInteractions = true;
        objectEntityList[i].visible            = true;
        objectEntityList[i].tileCollisions     = true;
    }
    LoadActLayout();
    Init3DFloorBuffer(0);
    currentCamera = 0;
    ProcessStartupObjects();
}

int LoadActFile(const char *ext, int stageID, FileInfo *info)
{
    char dest[0x40];

    StrCopy(dest, "Data/Stages/");
    StrAdd(dest, stageList[activeStageList][stageID].folder);
    StrAdd(dest, "/Act");
    StrAdd(dest, stageList[activeStageList][stageID].id);
    StrAdd(dest, ext);

    ConvertStringToInteger(stageList[activeStageList][stageID].id, &actID);

    return LoadFile(dest, info);
}
int LoadStageFile(const char *filePath, int stageID, FileInfo *info)
{
    char dest[0x40];

    StrCopy(dest, "Data/Stages/");
    StrAdd(dest, stageList[activeStageList][stageID].folder);
    StrAdd(dest, "/");
    StrAdd(dest, filePath);
    return LoadFile(dest, info);
}
void LoadActLayout()
{
    FileInfo info;
    for (int a = 0; a < 4; ++a) activeTileLayers[a] = 9; // disables missing scenes from rendering

    if (LoadActFile(".bin", stageListPosition, &info)) {
        byte fileBuffer[4];

        byte length = 0;
        FileRead(&length, 1);
        titleCardWord2 = (byte)length;
        for (int i = 0; i < length; ++i) {
            FileRead(&titleCardText[i], 1);
            if (titleCardText[i] == '-')
                titleCardWord2 = (byte)(i + 1);
        }
        titleCardText[length] = '\0';

        // READ TILELAYER
        FileRead(activeTileLayers, 4);
        FileRead(&tLayerMidPoint, 1);

        FileRead(&stageLayouts[0].xsize, 1);
        FileRead(fileBuffer, 1); // Unused

        FileRead(&stageLayouts[0].ysize, 1);
        FileRead(fileBuffer, 1); // Unused
        for (int c = 0; c < DEFAULT_CAMERA_COUNT; ++c) {
            camera[c].curXBoundary1 = 0;
            camera[c].newXBoundary1 = 0;
            camera[c].curYBoundary1 = 0;
            camera[c].newYBoundary1 = 0;
            camera[c].curXBoundary2 = stageLayouts[0].xsize << 7;
            camera[c].curYBoundary2 = stageLayouts[0].ysize << 7;
            camera[c].waterLevel    = camera[c].curYBoundary2 + 128;
            camera[c].newXBoundary2 = stageLayouts[0].xsize << 7;
            camera[c].newYBoundary2 = stageLayouts[0].ysize << 7;
        }

        memset(stageLayouts[0].tiles, 0, TILELAYER_CHUNK_COUNT * sizeof(ushort));
        memset(stageLayouts[0].lineScroll, 0, 0x7FFF);

        for (int y = 0; y < stageLayouts[0].ysize; ++y) {
            ushort *tiles = &stageLayouts[0].tiles[(y * TILELAYER_CHUNK_H)];
            for (int x = 0; x < stageLayouts[0].xsize; ++x) {
                FileRead(&fileBuffer[0], 1);
                tiles[x] = fileBuffer[0];
                FileRead(&fileBuffer[0], 1);
                tiles[x] |= fileBuffer[0] << 8;
            }
        }

        // READ OBJECTS
        FileRead(&fileBuffer[0], 2);
        int objectCount = fileBuffer[0] + (fileBuffer[1] << 8);
#if !RETRO_USE_ORIGINAL_CODE
        if (objectCount > 0x400)
            PrintLog("WARNING: object count %d exceeds the object limit", objectCount);
#endif

#if RETRO_USE_MOD_LOADER
        int offsetCount = 0;
        for (int m = 0; m < modObjCount; ++m)
            if (modScriptFlags[m])
                ++offsetCount;
#endif
        Entity *object = &objectEntityList[32];
        for (int i = 0; i < objectCount; ++i) {
            FileRead(fileBuffer, 2);
            ushort attribs = (fileBuffer[1] << 8) + fileBuffer[0];

            FileRead(fileBuffer, 1);
            object->type = fileBuffer[0];

#if RETRO_USE_MOD_LOADER
            if (loadGlobalScripts && offsetCount && object->type > globalObjCount)
                object->type += offsetCount; // offset it by our mod count
#endif

            FileRead(fileBuffer, 1);
            object->propertyValue = fileBuffer[0];

            FileRead(&fileBuffer, 4);
            object->xpos = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];

            FileRead(&fileBuffer, 4);
            object->ypos = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];

            if (attribs & 0x1) {
                FileRead(&fileBuffer, 4);
                object->state = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x2) {
                FileRead(fileBuffer, 1);
                object->direction = fileBuffer[0];
            }
            if (attribs & 0x4) {
                FileRead(&fileBuffer, 4);
                object->scale = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x8) {
                FileRead(&fileBuffer, 4);
                object->rotation = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x10) {
                FileRead(fileBuffer, 1);
                object->drawOrder = fileBuffer[0];
            }
            if (attribs & 0x20) {
                FileRead(fileBuffer, 1);
                object->priority = fileBuffer[0];
            }
            if (attribs & 0x40) {
                FileRead(fileBuffer, 1);
                object->alpha = fileBuffer[0];
            }
            if (attribs & 0x80) {
                FileRead(fileBuffer, 1);
                object->animation = fileBuffer[0];
            }
            if (attribs & 0x100) {
                FileRead(&fileBuffer, 4);
                object->animationSpeed = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x200) {
                FileRead(fileBuffer, 1);
                object->frame = fileBuffer[0];
            }
            if (attribs & 0x400) {
                FileRead(fileBuffer, 1);
                object->inkEffect = fileBuffer[0];
            }
            if (attribs & 0x800) {
                FileRead(&fileBuffer, 4);
                object->values[0] = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x1000) {
                FileRead(&fileBuffer, 4);
                object->values[1] = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x2000) {
                FileRead(&fileBuffer, 4);
                object->values[2] = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }
            if (attribs & 0x4000) {
                FileRead(&fileBuffer, 4);
                object->values[3] = (fileBuffer[3] << 24) + (fileBuffer[2] << 16) + (fileBuffer[1] << 8) + fileBuffer[0];
            }

            ++object;
        }
    }
    stageLayouts[0].type = LAYER_HSCROLL;
    CloseFile();
}
void LoadStageBackground()
{
    for (int i = 0; i < LAYER_COUNT; ++i) {
        stageLayouts[i].type               = LAYER_NOSCROLL;
        stageLayouts[i].deformationOffset  = 0;
        stageLayouts[i].deformationOffsetW = 0;
    }
    for (int i = 0; i < PARALLAX_COUNT; ++i) {
        hParallax.scrollPos[i] = 0;
        vParallax.scrollPos[i] = 0;
    }

    FileInfo info;
    if (LoadStageFile("Backgrounds.bin", stageListPosition, &info)) {
        byte fileBuffer = 0;
        byte layerCount = 0;
        FileRead(&layerCount, 1);
        FileRead(&hParallax.entryCount, 1);
        for (byte i = 0; i < hParallax.entryCount; ++i) {
            FileRead(&fileBuffer, 1);
            hParallax.parallaxFactor[i] = fileBuffer;
            FileRead(&fileBuffer, 1);
            hParallax.parallaxFactor[i] |= fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            hParallax.scrollSpeed[i] = fileBuffer << 10;

            hParallax.scrollPos[i] = 0;

            FileRead(&hParallax.deform[i], 1);
        }

        FileRead(&vParallax.entryCount, 1);
        for (byte i = 0; i < vParallax.entryCount; ++i) {
            FileRead(&fileBuffer, 1);
            vParallax.parallaxFactor[i] = fileBuffer;
            FileRead(&fileBuffer, 1);
            vParallax.parallaxFactor[i] |= fileBuffer << 8;

            FileRead(&fileBuffer, 1);
            vParallax.scrollSpeed[i] = fileBuffer << 10;

            vParallax.scrollPos[i] = 0;

            FileRead(&vParallax.deform[i], 1);
        }

        for (byte i = 1; i < layerCount + 1; ++i) {
            FileRead(&fileBuffer, 1);
            stageLayouts[i].xsize = fileBuffer;
            FileRead(&fileBuffer, 1); // Unused (???)
            FileRead(&fileBuffer, 1);
            stageLayouts[i].ysize = fileBuffer;
            FileRead(&fileBuffer, 1); // Unused (???)
            FileRead(&fileBuffer, 1);
            stageLayouts[i].type = fileBuffer;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].parallaxFactor = fileBuffer;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].parallaxFactor |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            stageLayouts[i].scrollSpeed = fileBuffer << 10;
            stageLayouts[i].scrollPos   = 0;

            memset(stageLayouts[i].tiles, 0, TILELAYER_CHUNK_COUNT * sizeof(ushort));
            byte *lineScrollPtr = stageLayouts[i].lineScroll;
            memset(stageLayouts[i].lineScroll, 0, 0x7FFF);

            // Read Line Scroll
            byte buf[3];
            int pos = 0;
            while (true) {
                FileRead(&buf[0], 1);
                if (buf[0] == 0xFF) {
                    FileRead(&buf[1], 1);
                    if (buf[1] == 0xFF) {
                        break;
                    }
                    else {
                        FileRead(&buf[2], 1);
                        int index = buf[1];
                        int cnt   = buf[2] - 1;
                        for (int c = 0; c < cnt; ++c) *lineScrollPtr++ = index;
                    }
                }
                else {
                    *lineScrollPtr++ = buf[0];
                }
            }

            // Read Layout
            for (int y = 0; y < stageLayouts[i].ysize; ++y) {
                ushort *chunks = &stageLayouts[i].tiles[y * TILELAYER_CHUNK_H];
                for (int x = 0; x < stageLayouts[i].xsize; ++x) {
                    FileRead(&fileBuffer, 1);
                    *chunks = fileBuffer;
                    FileRead(&fileBuffer, 1);
                    *chunks |= fileBuffer << 8;
                    ++chunks;
                }
            }
        }

        CloseFile();
    }
}
void LoadStageChunks()
{
    FileInfo info;
    byte entry[3];

    if (LoadStageFile("128x128Tiles.bin", stageListPosition, &info)) {
        for (int i = 0; i < CHUNKTILE_COUNT; ++i) {
            FileRead(&entry, 3);
            entry[0] -= (byte)((entry[0] >> 6) << 6);

            tiles128x128.visualPlane[i] = (byte)(entry[0] >> 4);
            entry[0] -= 16 * (entry[0] >> 4);

            tiles128x128.direction[i] = (byte)(entry[0] >> 2);
            entry[0] -= 4 * (entry[0] >> 2);

            tiles128x128.tileIndex[i] = entry[1] + (entry[0] << 8);
#if RETRO_SOFTWARE_RENDER
            tiles128x128.gfxDataPos[i] = tiles128x128.tileIndex[i] << 8;
#endif
#if RETRO_HARDWARE_RENDER
            tiles128x128.gfxDataPos[i] = tiles128x128.tileIndex[i] << 2;
#endif

            tiles128x128.collisionFlags[0][i] = entry[2] >> 4;
            tiles128x128.collisionFlags[1][i] = entry[2] - ((entry[2] >> 4) << 4);
        }
        CloseFile();
    }
}
void LoadStageCollisions()
{
    FileInfo info;
    if (LoadStageFile("CollisionMasks.bin", stageListPosition, &info)) {

        byte fileBuffer = 0;
        int tileIndex   = 0;
        for (int t = 0; t < TILE_COUNT; ++t) {
            for (int p = 0; p < CPATH_COUNT; ++p) {
                FileRead(&fileBuffer, 1);
                bool isCeiling             = fileBuffer >> 4;
                collisionMasks[p].flags[t] = fileBuffer & 0xF;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] = fileBuffer;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] |= fileBuffer << 8;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] |= fileBuffer << 16;
                FileRead(&fileBuffer, 1);
                collisionMasks[p].angles[t] |= fileBuffer << 24;

                if (isCeiling) // Ceiling Tile
                {
                    for (int c = 0; c < TILE_SIZE; c += 2) {
                        FileRead(&fileBuffer, 1);
                        collisionMasks[p].roofMasks[c + tileIndex]     = fileBuffer >> 4;
                        collisionMasks[p].roofMasks[c + tileIndex + 1] = fileBuffer & 0xF;
                    }

                    // Has Collision (Pt 1)
                    FileRead(&fileBuffer, 1);
                    int id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) {
                        if (fileBuffer & id) {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex + 8]  = -0x40;
                        }
                        id <<= 1;
                    }

                    // Has Collision (Pt 2)
                    FileRead(&fileBuffer, 1);
                    id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) {
                        if (fileBuffer & id) {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex]  = -0x40;
                        }
                        id <<= 1;
                    }

                    // LWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = 0;
                        while (h > -1) {
                            if (h == TILE_SIZE) {
                                collisionMasks[p].lWallMasks[c + tileIndex] = 0x40;
                                h                                           = -1;
                            }
                            else if (c > collisionMasks[p].roofMasks[h + tileIndex]) {
                                ++h;
                            }
                            else {
                                collisionMasks[p].lWallMasks[c + tileIndex] = h;
                                h                                           = -1;
                            }
                        }
                    }

                    // RWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = TILE_SIZE - 1;
                        while (h < TILE_SIZE) {
                            if (h == -1) {
                                collisionMasks[p].rWallMasks[c + tileIndex] = -0x40;
                                h                                           = TILE_SIZE;
                            }
                            else if (c > collisionMasks[p].roofMasks[h + tileIndex]) {
                                --h;
                            }
                            else {
                                collisionMasks[p].rWallMasks[c + tileIndex] = h;
                                h                                           = TILE_SIZE;
                            }
                        }
                    }
                }
                else // Regular Tile
                {
                    for (int c = 0; c < TILE_SIZE; c += 2) {
                        FileRead(&fileBuffer, 1);
                        collisionMasks[p].floorMasks[c + tileIndex]     = fileBuffer >> 4;
                        collisionMasks[p].floorMasks[c + tileIndex + 1] = fileBuffer & 0xF;
                    }
                    FileRead(&fileBuffer, 1);
                    int id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) // HasCollision
                    {
                        if (fileBuffer & id) {
                            collisionMasks[p].roofMasks[c + tileIndex + 8] = 0xF;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex + 8] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex + 8]  = -0x40;
                        }
                        id <<= 1;
                    }

                    FileRead(&fileBuffer, 1);
                    id = 1;
                    for (int c = 0; c < TILE_SIZE / 2; ++c) // HasCollision (pt 2)
                    {
                        if (fileBuffer & id) {
                            collisionMasks[p].roofMasks[c + tileIndex] = 0xF;
                        }
                        else {
                            collisionMasks[p].floorMasks[c + tileIndex] = 0x40;
                            collisionMasks[p].roofMasks[c + tileIndex]  = -0x40;
                        }
                        id <<= 1;
                    }

                    // LWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = 0;
                        while (h > -1) {
                            if (h == TILE_SIZE) {
                                collisionMasks[p].lWallMasks[c + tileIndex] = 0x40;
                                h                                           = -1;
                            }
                            else if (c < collisionMasks[p].floorMasks[h + tileIndex]) {
                                ++h;
                            }
                            else {
                                collisionMasks[p].lWallMasks[c + tileIndex] = h;
                                h                                           = -1;
                            }
                        }
                    }

                    // RWall rotations
                    for (int c = 0; c < TILE_SIZE; ++c) {
                        int h = TILE_SIZE - 1;
                        while (h < TILE_SIZE) {
                            if (h == -1) {
                                collisionMasks[p].rWallMasks[c + tileIndex] = -0x40;
                                h                                           = TILE_SIZE;
                            }
                            else if (c < collisionMasks[p].floorMasks[h + tileIndex]) {
                                --h;
                            }
                            else {
                                collisionMasks[p].rWallMasks[c + tileIndex] = h;
                                h                                           = TILE_SIZE;
                            }
                        }
                    }
                }
            }
            tileIndex += 16;
        }
        CloseFile();
    }
}
void LoadStageGIFFile(int stageID)
{
    FileInfo info;
    if (LoadStageFile("16x16Tiles.gif", stageID, &info)) {
        byte fileBuffer = 0;

        SetFilePosition(6); // GIF89a
        FileRead(&fileBuffer, 1);
        int width = fileBuffer;
        FileRead(&fileBuffer, 1);
        width |= (fileBuffer << 8);
        FileRead(&fileBuffer, 1);
        int height = fileBuffer;
        FileRead(&fileBuffer, 1);
        height |= (fileBuffer << 8);

        FileRead(&fileBuffer, 1); // Palette Size (thrown away) :/
        FileRead(&fileBuffer, 1); // BG Color index (thrown away)
        FileRead(&fileBuffer, 1); // idk actually (still thrown away)

        byte clr[3];
        for (int c = 0; c < 0x80; ++c) FileRead(clr, 3);
        for (int c = 0x80; c < 0x100; ++c) {
            FileRead(clr, 3);
            SetPaletteEntry(-1, c, clr[0], clr[1], clr[2]);
        }

        FileRead(&fileBuffer, 1);
        while (fileBuffer != ',') FileRead(&fileBuffer, 1); // gif image start identifier

        ushort fileBuffer2 = 0;
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer2, 2);
        FileRead(&fileBuffer, 1);
        bool interlaced = (fileBuffer & 0x40) >> 6;
        if ((unsigned int)fileBuffer >> 7 == 1) {
            int c = 128;
            do {
                ++c;
                FileRead(&fileBuffer, 3);
            } while (c != 256);
        }

        ReadGifPictureData(width, height, interlaced, tilesetGFXData, 0);

        byte transparent = tilesetGFXData[0];
        for (int i = 0; i < TILESET_SIZE; ++i) {
            if (tilesetGFXData[i] == transparent)
                tilesetGFXData[i] = 0;
        }

        CloseFile();
    }
}

void ResetBackgroundSettings()
{
    for (int i = 0; i < LAYER_COUNT; ++i) {
        stageLayouts[i].deformationOffset  = 0;
        stageLayouts[i].deformationOffsetW = 0;
        stageLayouts[i].scrollPos          = 0;
    }

    for (int i = 0; i < PARALLAX_COUNT; ++i) {
        hParallax.scrollPos[i] = 0;
        vParallax.scrollPos[i] = 0;
    }

    for (int i = 0; i < DEFORM_COUNT; ++i) {
        bgDeformationData0[i] = 0;
        bgDeformationData1[i] = 0;
        bgDeformationData2[i] = 0;
        bgDeformationData3[i] = 0;
    }
}

void SetLayerDeformation(int selectedDef, int waveLength, int waveWidth, int waveType, int YPos, int waveSize)
{
    int *deformPtr = nullptr;
    switch (selectedDef) {
        case DEFORM_FG: deformPtr = bgDeformationData0; break;
        case DEFORM_FG_WATER: deformPtr = bgDeformationData1; break;
        case DEFORM_BG: deformPtr = bgDeformationData2; break;
        case DEFORM_BG_WATER: deformPtr = bgDeformationData3; break;
        default: break;
    }

#if RETRO_SOFTWARE_RENDER
    int shift = 9;
#endif

#if RETRO_HARDWARE_RENDER
    int shift = 5;
#endif

    int id = 0;
    if (waveType == 1) {
        id = YPos;
        for (int i = 0; i < waveSize; ++i) {
            deformPtr[id] = waveWidth * sin512LookupTable[(i << 9) / waveLength & 0x1FF] >> shift;
            ++id;
        }
    }
    else {
        for (int i = 0; i < 0x200 * 0x100; i += 0x200) {
            int val       = waveWidth * sin512LookupTable[i / waveLength & 0x1FF] >> shift;
            deformPtr[id] = val;
#if RETRO_SOFTWARE_RENDER
            if (deformPtr[id] >= waveWidth)
                deformPtr[id] = waveWidth - 1;
#endif
            ++id;
        }
    }

    switch (selectedDef) {
        case DEFORM_FG:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData0[i] = bgDeformationData0[i - DEFORM_STORE];
            break;
        case DEFORM_FG_WATER:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData1[i] = bgDeformationData1[i - DEFORM_STORE];
            break;
        case DEFORM_BG:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData2[i] = bgDeformationData2[i - DEFORM_STORE];
            break;
        case DEFORM_BG_WATER:
            for (int i = DEFORM_STORE; i < DEFORM_COUNT; ++i) bgDeformationData3[i] = bgDeformationData3[i - DEFORM_STORE];
            break;
        default: break;
    }
}

void SetPlayerScreenPosition(Entity *target)
{
    int targetX = target->xpos >> 16;
    int targetY = camera[curCam].adjustY + (target->ypos >> 16);
    if (camera[curCam].newYBoundary1 > camera[curCam].curYBoundary1) {
        if (camera[curCam].newYBoundary1 >= camera[curCam].yScrollOffset)
            camera[curCam].curYBoundary1 = camera[curCam].yScrollOffset;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    if (camera[curCam].newYBoundary1 < camera[curCam].curYBoundary1) {
        if (camera[curCam].curYBoundary1 >= camera[curCam].yScrollOffset)
            --camera[curCam].curYBoundary1;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    if (camera[curCam].newYBoundary2 < camera[curCam].curYBoundary2) {
        if (camera[curCam].curYBoundary2 <= camera[curCam].yScrollOffset + SCREEN_YSIZE || camera[curCam].newYBoundary2 >= camera[curCam].yScrollOffset + SCREEN_YSIZE)
            --camera[curCam].curYBoundary2;
        else
            camera[curCam].curYBoundary2 = camera[curCam].yScrollOffset + SCREEN_YSIZE;
    }
    if (camera[curCam].newYBoundary2 > camera[curCam].curYBoundary2) {
        if (camera[curCam].yScrollOffset + SCREEN_YSIZE >= camera[curCam].curYBoundary2) {
            ++camera[curCam].curYBoundary2;
            if (target->yvel > 0) {
                int buf = camera[curCam].curYBoundary2 + (target->yvel >> 16);
                if (camera[curCam].newYBoundary2 < buf) {
                    camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
                }
                else {
                    camera[curCam].curYBoundary2 += target->yvel >> 16;
                }
            }
        }
        else
            camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
    }
    if (camera[curCam].newXBoundary1 > camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].newXBoundary1)
            camera[curCam].curXBoundary1 = camera[curCam].xScrollOffset;
        else
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
    }
    if (camera[curCam].newXBoundary1 < camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].curXBoundary1) {
            --camera[curCam].curXBoundary1;
            if (target->xvel < 0) {
                camera[curCam].curXBoundary1 += target->xvel >> 16;
                if (camera[curCam].curXBoundary1 < camera[curCam].newXBoundary1)
                    camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
            }
        }
        else {
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
        }
    }
    if (camera[curCam].newXBoundary2 < camera[curCam].curXBoundary2) {
        if (camera[curCam].newXBoundary2 > SCREEN_XSIZE + camera[curCam].xScrollOffset)
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        else
            camera[curCam].curXBoundary2 = SCREEN_XSIZE + camera[curCam].xScrollOffset;
    }
    if (camera[curCam].newXBoundary2 > camera[curCam].curXBoundary2) {
        if (SCREEN_XSIZE + camera[curCam].xScrollOffset >= camera[curCam].curXBoundary2) {
            ++camera[curCam].curXBoundary2;
            if (target->xvel > 0) {
                camera[curCam].curXBoundary2 += target->xvel >> 16;
                if (camera[curCam].curXBoundary2 > camera[curCam].newXBoundary2)
                    camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
            }
        }
        else {
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        }
    }

    int xPosDif = targetX - camera[curCam].xpos;
    if (targetX > camera[curCam].xpos) {
        xPosDif -= 8;
        if (xPosDif >= 0) {
            if (xPosDif >= 17)
                xPosDif = 16;
        }
        else {
            xPosDif = 0;
        }
    }
    else {
        xPosDif += 8;
        if (xPosDif > 0) {
            xPosDif = 0;
        }
        else if (xPosDif <= -17) {
            xPosDif = -16;
        }
    }

    int centeredXBound1 = camera[curCam].xpos + xPosDif;
    camera[curCam].xpos          = centeredXBound1;
    if (centeredXBound1 < SCREEN_CENTERX + camera[curCam].curXBoundary1) {
        camera[curCam].xpos      = SCREEN_CENTERX + camera[curCam].curXBoundary1;
        centeredXBound1 = SCREEN_CENTERX + camera[curCam].curXBoundary1;
    }

    int centeredXBound2 = camera[curCam].curXBoundary2 - SCREEN_CENTERX;
    if (centeredXBound2 < centeredXBound1) {
        camera[curCam].xpos      = centeredXBound2;
        centeredXBound1 = centeredXBound2;
    }

    int yPosDif = 0;
    if (target->scrollTracking) {
        if (targetY <= camera[curCam].ypos) {
            yPosDif = (targetY - camera[curCam].ypos) + 32;
            if (yPosDif <= 0) {
                if (yPosDif <= -17)
                    yPosDif = -16;
            }
            else
                yPosDif = 0;
        }
        else {
            yPosDif = (targetY - camera[curCam].ypos) - 32;
            if (yPosDif >= 0) {
                if (yPosDif >= 17)
                    yPosDif = 16;
            }
            else
                yPosDif = 0;
        }
        camera[curCam].lockedY = false;
    }
    else if (camera[curCam].lockedY) {
        yPosDif    = 0;
        camera[curCam].ypos = targetY;
    }
    else if (targetY <= camera[curCam].ypos) {
        yPosDif = targetY - camera[curCam].ypos;
        if (targetY - camera[curCam].ypos <= 0) {
            if (yPosDif >= -32 && abs(target->yvel) <= 0x60000) {
                if (yPosDif < -6) {
                    yPosDif = -6;
                }
            }
            else if (yPosDif < -16) {
                yPosDif = -16;
            }
        }
        else {
            yPosDif       = 0;
            camera[curCam].lockedY = true;
        }
    }
    else {
        yPosDif = targetY - camera[curCam].ypos;
        if (targetY - camera[curCam].ypos < 0) {
            yPosDif       = 0;
            camera[curCam].lockedY = true;
        }
        else if (yPosDif > 32 || abs(target->yvel) > 0x60000) {
            if (yPosDif > 16) {
                yPosDif = 16;
            }
            else {
                camera[curCam].lockedY = true;
            }
        }
        else {
            if (yPosDif <= 6) {
                camera[curCam].lockedY = true;
            }
            else {
                yPosDif = 6;
            }
        }
    }

    int newCamY = camera[curCam].ypos + yPosDif;
    if (newCamY <= camera[curCam].curYBoundary1 + (SCREEN_SCROLL_UP - 1))
        newCamY = camera[curCam].curYBoundary1 + SCREEN_SCROLL_UP;
    camera[curCam].ypos = newCamY;
    if (camera[curCam].curYBoundary2 - (SCREEN_SCROLL_DOWN - 1) <= newCamY) {
        camera[curCam].ypos = camera[curCam].curYBoundary2 - SCREEN_SCROLL_DOWN;
    }

    camera[curCam].xScrollOffset = camera[curCam].shakeX + centeredXBound1 - SCREEN_CENTERX;

    int pos = camera[curCam].ypos + target->lookPosY - SCREEN_SCROLL_UP;
    if (pos < camera[curCam].curYBoundary1) {
        camera[curCam].yScrollOffset = camera[curCam].curYBoundary1;
    }
    else {
        camera[curCam].yScrollOffset = camera[curCam].ypos + target->lookPosY - SCREEN_SCROLL_UP;
    }

    int y = camera[curCam].curYBoundary2 - SCREEN_YSIZE;
    if (camera[curCam].curYBoundary2 - (SCREEN_YSIZE - 1) > camera[curCam].yScrollOffset)
        y = camera[curCam].yScrollOffset;
    camera[curCam].yScrollOffset = camera[curCam].shakeY + y;

    if (camera[curCam].shakeX) {
        if (camera[curCam].shakeX <= 0) {
            camera[curCam].shakeX = ~camera[curCam].shakeX;
        }
        else {
            camera[curCam].shakeX = -camera[curCam].shakeX;
        }
    }

    if (camera[curCam].shakeY) {
        if (camera[curCam].shakeY <= 0) {
            camera[curCam].shakeY = ~camera[curCam].shakeY;
        }
        else {
            camera[curCam].shakeY = -camera[curCam].shakeY;
        }
    }
}
void SetPlayerScreenPositionCDStyle(Entity *target)
{
    int targetX = target->xpos >> 16;
    int targetY = camera[curCam].adjustY + (target->ypos >> 16);
    if (camera[curCam].newYBoundary1 > camera[curCam].curYBoundary1) {
        if (camera[curCam].newYBoundary1 >= camera[curCam].yScrollOffset)
            camera[curCam].curYBoundary1 = camera[curCam].yScrollOffset;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    if (camera[curCam].newYBoundary1 < camera[curCam].curYBoundary1) {
        if (camera[curCam].curYBoundary1 >= camera[curCam].yScrollOffset)
            --camera[curCam].curYBoundary1;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    if (camera[curCam].newYBoundary2 < camera[curCam].curYBoundary2) {
        if (camera[curCam].curYBoundary2 <= camera[curCam].yScrollOffset + SCREEN_YSIZE || camera[curCam].newYBoundary2 >= camera[curCam].yScrollOffset + SCREEN_YSIZE)
            --camera[curCam].curYBoundary2;
        else
            camera[curCam].curYBoundary2 = camera[curCam].yScrollOffset + SCREEN_YSIZE;
    }
    if (camera[curCam].newYBoundary2 > camera[curCam].curYBoundary2) {
        if (camera[curCam].yScrollOffset + SCREEN_YSIZE >= camera[curCam].curYBoundary2) {
            ++camera[curCam].curYBoundary2;
            if (target->yvel > 0) {
                int buf = camera[curCam].curYBoundary2 + (target->yvel >> 16);
                if (camera[curCam].newYBoundary2 < buf) {
                    camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
                }
                else {
                    camera[curCam].curYBoundary2 += target->yvel >> 16;
                }
            }
        }
        else
            camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
    }
    if (camera[curCam].newXBoundary1 > camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].newXBoundary1)
            camera[curCam].curXBoundary1 = camera[curCam].xScrollOffset;
        else
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
    }
    if (camera[curCam].newXBoundary1 < camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].curXBoundary1) {
            --camera[curCam].curXBoundary1;
            if (target->xvel < 0) {
                camera[curCam].curXBoundary1 += target->xvel >> 16;
                if (camera[curCam].curXBoundary1 < camera[curCam].newXBoundary1)
                    camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
            }
        }
        else {
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
        }
    }
    if (camera[curCam].newXBoundary2 < camera[curCam].curXBoundary2) {
        if (camera[curCam].newXBoundary2 > SCREEN_XSIZE + camera[curCam].xScrollOffset)
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        else
            camera[curCam].curXBoundary2 = SCREEN_XSIZE + camera[curCam].xScrollOffset;
    }
    if (camera[curCam].newXBoundary2 > camera[curCam].curXBoundary2) {
        if (SCREEN_XSIZE + camera[curCam].xScrollOffset >= camera[curCam].curXBoundary2) {
            ++camera[curCam].curXBoundary2;
            if (target->xvel > 0) {
                camera[curCam].curXBoundary2 += target->xvel >> 16;
                if (camera[curCam].curXBoundary2 > camera[curCam].newXBoundary2)
                    camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
            }
        }
        else {
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        }
    }

    if (!target->gravity) {
        if (target->direction) {
            if (camera[curCam].style == CAMERASTYLE_EXTENDED_OFFSET_R || target->speed < -0x5F5C2) {
                camera[curCam].shift = 2;
                if (target->lookPosX <= 63) {
                    target->lookPosX += 2;
                }
            }
            else {
                camera[curCam].shift = 0;
                if (target->lookPosX < 0) {
                    target->lookPosX += 2;
                }

                if (target->lookPosX > 0) {
                    target->lookPosX -= 2;
                }
            }
        }
        else {
            if (camera[curCam].style == CAMERASTYLE_EXTENDED_OFFSET_L || target->speed > 0x5F5C2) {
                camera[curCam].shift = 1;
                if (target->lookPosX >= -63) {
                    target->lookPosX -= 2;
                }
            }
            else {
                camera[curCam].shift = 0;
                if (target->lookPosX < 0) {
                    target->lookPosX += 2;
                }

                if (target->lookPosX > 0) {
                    target->lookPosX -= 2;
                }
            }
        }
    }
    else {
        if (camera[curCam].shift == 1) {
            if (target->lookPosX >= -63) {
                target->lookPosX -= 2;
            }
        }
        else if (camera[curCam].shift < 1) {
            if (target->lookPosX < 0) {
                target->lookPosX += 2;
            }
            if (target->lookPosX > 0) {
                target->lookPosX -= 2;
            }
        }
        else if (camera[curCam].shift == 2) {
            if (target->lookPosX <= 63) {
                target->lookPosX += 2;
            }
        }
    }
    camera[curCam].xpos = targetX - target->lookPosX;

    int yPosDif = 0;
    if (target->scrollTracking) {
        if (targetY <= camera[curCam].ypos) {
            yPosDif = (targetY - camera[curCam].ypos) + 32;
            if (yPosDif <= 0) {
                if (yPosDif <= -17)
                    yPosDif = -16;
            }
            else
                yPosDif = 0;
        }
        else {
            yPosDif = (targetY - camera[curCam].ypos) - 32;
            if (yPosDif >= 0) {
                if (yPosDif >= 17)
                    yPosDif = 16;
            }
            else
                yPosDif = 0;
        }
        camera[curCam].lockedY = false;
    }
    else if (camera[curCam].lockedY) {
        yPosDif    = 0;
        camera[curCam].ypos = targetY;
    }
    else if (targetY <= camera[curCam].ypos) {
        yPosDif = targetY - camera[curCam].ypos;
        if (targetY - camera[curCam].ypos <= 0) {
            if (yPosDif >= -32 && abs(target->yvel) <= 0x60000) {
                if (yPosDif < -6) {
                    yPosDif = -6;
                }
            }
            else if (yPosDif < -16) {
                yPosDif = -16;
            }
        }
        else {
            yPosDif       = 0;
            camera[curCam].lockedY = true;
        }
    }
    else {
        yPosDif = targetY - camera[curCam].ypos;
        if (targetY - camera[curCam].ypos < 0) {
            yPosDif       = 0;
            camera[curCam].lockedY = true;
        }
        else if (yPosDif > 32 || abs(target->yvel) > 0x60000) {
            if (yPosDif > 16) {
                yPosDif = 16;
            }
            else {
                camera[curCam].lockedY = true;
            }
        }
        else {
            if (yPosDif <= 6) {
                camera[curCam].lockedY = true;
            }
            else {
                yPosDif = 6;
            }
        }
    }

    int newCamY = camera[curCam].ypos + yPosDif;
    if (newCamY <= camera[curCam].curYBoundary1 + (SCREEN_SCROLL_UP - 1))
        newCamY = camera[curCam].curYBoundary1 + SCREEN_SCROLL_UP;
    camera[curCam].ypos = newCamY;
    if (camera[curCam].curYBoundary2 - (SCREEN_SCROLL_DOWN - 1) <= newCamY) {
        camera[curCam].ypos = camera[curCam].curYBoundary2 - SCREEN_SCROLL_DOWN;
    }

    camera[curCam].xScrollOffset = camera[curCam].xpos - SCREEN_CENTERX;
    camera[curCam].yScrollOffset = target->lookPosY + camera[curCam].ypos - SCREEN_SCROLL_UP;

    int x = camera[curCam].curXBoundary1;
    if (x <= camera[curCam].xScrollOffset)
        x = camera[curCam].xScrollOffset;
    else
        camera[curCam].xScrollOffset = x;

    if (x > camera[curCam].curXBoundary2 - SCREEN_XSIZE) {
        x             = camera[curCam].curXBoundary2 - SCREEN_XSIZE;
        camera[curCam].xScrollOffset = camera[curCam].curXBoundary2 - SCREEN_XSIZE;
    }

    int y = camera[curCam].curYBoundary1;
    if (camera[curCam].yScrollOffset >= y)
        y = camera[curCam].yScrollOffset;
    else
        camera[curCam].yScrollOffset = y;

    if (camera[curCam].curYBoundary2 - SCREEN_YSIZE - 1 <= y)
        y = camera[curCam].curYBoundary2 - SCREEN_YSIZE;

    camera[curCam].xScrollOffset = camera[curCam].shakeX + x;
    camera[curCam].yScrollOffset = camera[curCam].shakeY + y;

    if (camera[curCam].shakeX) {
        if (camera[curCam].shakeX <= 0) {
            camera[curCam].shakeX = ~camera[curCam].shakeX;
        }
        else {
            camera[curCam].shakeX = -camera[curCam].shakeX;
        }
    }

    if (camera[curCam].shakeY) {
        if (camera[curCam].shakeY <= 0) {
            camera[curCam].shakeY = ~camera[curCam].shakeY;
        }
        else {
            camera[curCam].shakeY = -camera[curCam].shakeY;
        }
    }
}
void SetPlayerHLockedScreenPosition(Entity *target)
{
    int targetY = camera[curCam].adjustY + (target->ypos >> 16);

    if (camera[curCam].newYBoundary1 <= camera[curCam].curYBoundary1) {
        if (camera[curCam].curYBoundary1 > camera[curCam].yScrollOffset)
            --camera[curCam].curYBoundary1;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    else {
        if (camera[curCam].newYBoundary1 >= camera[curCam].yScrollOffset)
            camera[curCam].curYBoundary1 = camera[curCam].yScrollOffset;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }

    if (camera[curCam].newYBoundary2 < camera[curCam].curYBoundary2) {
        if (camera[curCam].curYBoundary2 <= camera[curCam].yScrollOffset + SCREEN_YSIZE || camera[curCam].newYBoundary2 >= camera[curCam].yScrollOffset + SCREEN_YSIZE)
            --camera[curCam].curYBoundary2;
        else
            camera[curCam].curYBoundary2 = camera[curCam].yScrollOffset + SCREEN_YSIZE;
    }
    if (camera[curCam].newYBoundary2 > camera[curCam].curYBoundary2) {
        if (camera[curCam].yScrollOffset + SCREEN_YSIZE >= camera[curCam].curYBoundary2) {
            ++camera[curCam].curYBoundary2;
            if (target->yvel > 0) {
                if (camera[curCam].newYBoundary2 < camera[curCam].curYBoundary2 + (target->yvel >> 16)) {
                    camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
                }
                else {
                    camera[curCam].curYBoundary2 += target->yvel >> 16;
                }
            }
        }
        else
            camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
    }

    if (camera[curCam].newXBoundary1 > camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].newXBoundary1)
            camera[curCam].curXBoundary1 = camera[curCam].xScrollOffset;
        else
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
    }
    if (camera[curCam].newXBoundary1 < camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].curXBoundary1) {
            --camera[curCam].curXBoundary1;
            if (target->xvel < 0) {
                camera[curCam].curXBoundary1 += target->xvel >> 16;
                if (camera[curCam].curXBoundary1 < camera[curCam].newXBoundary1)
                    camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
            }
        }
        else {
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
        }
    }
    if (camera[curCam].newXBoundary2 < camera[curCam].curXBoundary2) {
        if (camera[curCam].newXBoundary2 > SCREEN_XSIZE + camera[curCam].xScrollOffset)
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        else
            camera[curCam].curXBoundary2 = SCREEN_XSIZE + camera[curCam].xScrollOffset;
    }
    if (camera[curCam].newXBoundary2 > camera[curCam].curXBoundary2) {
        if (SCREEN_XSIZE + camera[curCam].xScrollOffset >= camera[curCam].curXBoundary2) {
            ++camera[curCam].curXBoundary2;
            if (target->xvel > 0) {
                camera[curCam].curXBoundary2 += target->xvel >> 16;
                if (camera[curCam].curXBoundary2 > camera[curCam].newXBoundary2)
                    camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
            }
        }
        else {
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        }
    }

    int camScroll = 0;
    if (target->scrollTracking) {
        if (targetY <= camera[curCam].ypos) {
            camScroll = targetY - camera[curCam].ypos + 32;
            if (camScroll <= 0) {
                if (camScroll <= -17)
                    camScroll = -16;
            }
            else
                camScroll = 0;
        }
        else {
            camScroll = targetY - camera[curCam].ypos - 32;
            if (camScroll >= 0) {
                if (camScroll >= 17)
                    camScroll = 16;
            }
            else
                camScroll = 0;
        }
        camera[curCam].lockedY = false;
    }
    else if (camera[curCam].lockedY) {
        camScroll = 0;
        camera[curCam].ypos = targetY;
    }
    else if (targetY > camera[curCam].ypos) {
        camScroll = targetY - camera[curCam].ypos;
        if (camScroll >= 0) {
            if (camScroll > 32 || abs(target->yvel) > 0x60000) {
                if (camScroll > 16) {
                    camScroll = 16;
                }
                else {
                    camera[curCam].lockedY = true;
                }
            }
            else if (camScroll > 6) {
                camScroll = 6;
            }
            else {
                camera[curCam].lockedY = true;
            }
        }
        else {
            camScroll     = 0;
            camera[curCam].lockedY = true;
        }
    }
    else {
        camScroll = targetY - camera[curCam].ypos;
        if (camScroll > 0) {
            camScroll     = 0;
            camera[curCam].lockedY = true;
        }
        else if (camScroll < -32 || abs(target->yvel) > 0x60000) {
            if (camScroll < -16) {
                camScroll = -16;
            }
            else {
                camera[curCam].lockedY = true;
            }
        }
        else {
            if (camScroll >= -6)
                camera[curCam].lockedY = true;
            else
                camScroll = -6;
        }
    }

    int newCamY = camera[curCam].ypos + camScroll;
    if (newCamY <= camera[curCam].curYBoundary1 + (SCREEN_SCROLL_UP - 1))
        newCamY = camera[curCam].curYBoundary1 + SCREEN_SCROLL_UP;
    camera[curCam].ypos = newCamY;
    if (camera[curCam].curYBoundary2 - (SCREEN_SCROLL_DOWN - 1) <= newCamY) {
        newCamY    = camera[curCam].curYBoundary2 - SCREEN_SCROLL_DOWN;
        camera[curCam].ypos = camera[curCam].curYBoundary2 - SCREEN_SCROLL_DOWN;
    }

    camera[curCam].xScrollOffset = camera[curCam].shakeX + camera[curCam].xpos - SCREEN_CENTERX;

    int pos = newCamY + target->lookPosY - SCREEN_SCROLL_UP;
    if (pos < camera[curCam].curYBoundary1) {
        camera[curCam].yScrollOffset = camera[curCam].curYBoundary1;
    }
    else {
        camera[curCam].yScrollOffset = newCamY + target->lookPosY - SCREEN_SCROLL_UP;
    }
    int y1 = camera[curCam].curYBoundary2 - (SCREEN_YSIZE - 1);
    int y2 = camera[curCam].curYBoundary2 - SCREEN_YSIZE;
    if (y1 > camera[curCam].yScrollOffset)
        y2 = camera[curCam].yScrollOffset;
    camera[curCam].yScrollOffset = camera[curCam].shakeY + y2;

    if (camera[curCam].shakeX) {
        if (camera[curCam].shakeX <= 0) {
            camera[curCam].shakeX = ~camera[curCam].shakeX;
        }
        else {
            camera[curCam].shakeX = -camera[curCam].shakeX;
        }
    }

    if (camera[curCam].shakeY) {
        if (camera[curCam].shakeY <= 0) {
            camera[curCam].shakeY = ~camera[curCam].shakeY;
        }
        else {
            camera[curCam].shakeY = -camera[curCam].shakeY;
        }
    }
}
void SetPlayerLockedScreenPosition(Entity *target)
{
    if (camera[curCam].newYBoundary1 > camera[curCam].curYBoundary1) {
        if (camera[curCam].yScrollOffset <= camera[curCam].newYBoundary1)
            camera[curCam].curYBoundary1 = camera[curCam].yScrollOffset;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    if (camera[curCam].newYBoundary1 < camera[curCam].curYBoundary1) {
        if (camera[curCam].yScrollOffset <= camera[curCam].curYBoundary1)
            --camera[curCam].curYBoundary1;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    if (camera[curCam].newYBoundary2 < camera[curCam].curYBoundary2) {
        if (camera[curCam].curYBoundary2 <= camera[curCam].yScrollOffset + SCREEN_YSIZE || camera[curCam].newYBoundary2 >= camera[curCam].yScrollOffset + SCREEN_YSIZE)
            --camera[curCam].curYBoundary2;
        else
            camera[curCam].curYBoundary2 = camera[curCam].yScrollOffset + SCREEN_YSIZE;
    }
    if (camera[curCam].newYBoundary2 > camera[curCam].curYBoundary2) {
        if (camera[curCam].yScrollOffset + SCREEN_YSIZE >= camera[curCam].curYBoundary2)
            ++camera[curCam].curYBoundary2;
        else
            camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
    }
    if (camera[curCam].newXBoundary1 > camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].newXBoundary1)
            camera[curCam].curXBoundary1 = camera[curCam].xScrollOffset;
        else
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
    }
    if (camera[curCam].newXBoundary1 < camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].curXBoundary1) {
            --camera[curCam].curXBoundary1;
            if (target->xvel < 0) {
                camera[curCam].curXBoundary1 += target->xvel >> 16;
                if (camera[curCam].curXBoundary1 < camera[curCam].newXBoundary1)
                    camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
            }
        }
        else {
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
        }
    }
    if (camera[curCam].newXBoundary2 < camera[curCam].curXBoundary2) {
        if (camera[curCam].newXBoundary2 > SCREEN_XSIZE + camera[curCam].xScrollOffset)
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        else
            camera[curCam].curXBoundary2 = SCREEN_XSIZE + camera[curCam].xScrollOffset;
    }
    if (camera[curCam].newXBoundary2 > camera[curCam].curXBoundary2) {
        if (SCREEN_XSIZE + camera[curCam].xScrollOffset >= camera[curCam].curXBoundary2) {
            ++camera[curCam].curXBoundary2;
            if (target->xvel > 0) {
                camera[curCam].curXBoundary2 += target->xvel >> 16;
                if (camera[curCam].curXBoundary2 > camera[curCam].newXBoundary2)
                    camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
            }
        }
        else {
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        }
    }

    if (camera[curCam].shakeX) {
        if (camera[curCam].shakeX <= 0) {
            camera[curCam].shakeX = ~camera[curCam].shakeX;
        }
        else {
            camera[curCam].shakeX = -camera[curCam].shakeX;
        }
    }

    if (camera[curCam].shakeY) {
        if (camera[curCam].shakeY <= 0) {
            camera[curCam].shakeY = ~camera[curCam].shakeY;
        }
        else {
            camera[curCam].shakeY = -camera[curCam].shakeY;
        }
    }
}
// Completely unused in both Sonic 1 & Sonic 2
void SetPlayerScreenPositionFixed(Entity *target)
{
    int targetX = target->xpos >> 16;
    int targetY = camera[curCam].adjustY + (target->ypos >> 16);
    if (camera[curCam].newYBoundary1 > camera[curCam].curYBoundary1) {
        if (camera[curCam].newYBoundary1 >= camera[curCam].yScrollOffset)
            camera[curCam].curYBoundary1 = camera[curCam].yScrollOffset;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    if (camera[curCam].newYBoundary1 < camera[curCam].curYBoundary1) {
        if (camera[curCam].curYBoundary1 >= camera[curCam].yScrollOffset)
            --camera[curCam].curYBoundary1;
        else
            camera[curCam].curYBoundary1 = camera[curCam].newYBoundary1;
    }
    if (camera[curCam].newYBoundary2 < camera[curCam].curYBoundary2) {
        if (camera[curCam].curYBoundary2 <= camera[curCam].yScrollOffset + SCREEN_YSIZE || camera[curCam].newYBoundary2 >= camera[curCam].yScrollOffset + SCREEN_YSIZE)
            --camera[curCam].curYBoundary2;
        else
            camera[curCam].curYBoundary2 = camera[curCam].yScrollOffset + SCREEN_YSIZE;
    }
    if (camera[curCam].newYBoundary2 > camera[curCam].curYBoundary2) {
        if (camera[curCam].yScrollOffset + SCREEN_YSIZE >= camera[curCam].curYBoundary2) {
            ++camera[curCam].curYBoundary2;
            if (target->yvel > 0) {
                int buf = camera[curCam].curYBoundary2 + (target->yvel >> 16);
                if (camera[curCam].newYBoundary2 < buf) {
                    camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
                }
                else {
                    camera[curCam].curYBoundary2 += target->yvel >> 16;
                }
            }
        }
        else
            camera[curCam].curYBoundary2 = camera[curCam].newYBoundary2;
    }
    if (camera[curCam].newXBoundary1 > camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].newXBoundary1)
            camera[curCam].curXBoundary1 = camera[curCam].xScrollOffset;
        else
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
    }
    if (camera[curCam].newXBoundary1 < camera[curCam].curXBoundary1) {
        if (camera[curCam].xScrollOffset <= camera[curCam].curXBoundary1) {
            --camera[curCam].curXBoundary1;
            if (target->xvel < 0) {
                camera[curCam].curXBoundary1 += target->xvel >> 16;
                if (camera[curCam].curXBoundary1 < camera[curCam].newXBoundary1)
                    camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
            }
        }
        else {
            camera[curCam].curXBoundary1 = camera[curCam].newXBoundary1;
        }
    }
    if (camera[curCam].newXBoundary2 < camera[curCam].curXBoundary2) {
        if (camera[curCam].newXBoundary2 > SCREEN_XSIZE + camera[curCam].xScrollOffset)
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        else
            camera[curCam].curXBoundary2 = SCREEN_XSIZE + camera[curCam].xScrollOffset;
    }
    if (camera[curCam].newXBoundary2 > camera[curCam].curXBoundary2) {
        if (SCREEN_XSIZE + camera[curCam].xScrollOffset >= camera[curCam].curXBoundary2) {
            ++camera[curCam].curXBoundary2;
            if (target->xvel > 0) {
                camera[curCam].curXBoundary2 += target->xvel >> 16;
                if (camera[curCam].curXBoundary2 > camera[curCam].newXBoundary2)
                    camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
            }
        }
        else {
            camera[curCam].curXBoundary2 = camera[curCam].newXBoundary2;
        }
    }

    camera[curCam].xpos = targetX;
    if (targetX < SCREEN_CENTERX + camera[curCam].curXBoundary1) {
        targetX    = SCREEN_CENTERX + camera[curCam].curXBoundary1;
        camera[curCam].xpos = SCREEN_CENTERX + camera[curCam].curXBoundary1;
    }
    int boundX2 = camera[curCam].curXBoundary2 - SCREEN_CENTERX;
    if (boundX2 < targetX) {
        targetX    = boundX2;
        camera[curCam].xpos = boundX2;
    }

    if (targetY <= camera[curCam].curYBoundary1 + 119) {
        targetY    = camera[curCam].curYBoundary1 + 120;
        camera[curCam].ypos = camera[curCam].curYBoundary1 + 120;
    }
    else {
        camera[curCam].ypos = targetY;
    }
    if (camera[curCam].curYBoundary2 - 119 <= targetY) {
        targetY    = camera[curCam].curYBoundary2 - 120;
        camera[curCam].ypos = camera[curCam].curYBoundary2 - 120;
    }

    camera[curCam].xScrollOffset = camera[curCam].shakeX + targetX - SCREEN_CENTERX;
    int camY      = targetY + target->lookPosY - SCREEN_CENTERY;
    if (camera[curCam].curYBoundary1 > camY) {
        camera[curCam].yScrollOffset = camera[curCam].curYBoundary1;
    }
    else {
        camera[curCam].yScrollOffset = targetY + target->lookPosY - SCREEN_CENTERY;
    }

    int newCamY = camera[curCam].curYBoundary2 - SCREEN_YSIZE;
    if (camera[curCam].curYBoundary2 - (SCREEN_YSIZE - 1) > camera[curCam].yScrollOffset)
        newCamY = camera[curCam].yScrollOffset;
    camera[curCam].yScrollOffset = camera[curCam].shakeY + newCamY;

    if (camera[curCam].shakeX) {
        if (camera[curCam].shakeX <= 0) {
            camera[curCam].shakeX = ~camera[curCam].shakeX;
        }
        else {
            camera[curCam].shakeX = -camera[curCam].shakeX;
        }
    }

    if (camera[curCam].shakeY) {
        if (camera[curCam].shakeY <= 0) {
            camera[curCam].shakeY = ~camera[curCam].shakeY;
        }
        else {
            camera[curCam].shakeY = -camera[curCam].shakeY;
        }
    }
}
