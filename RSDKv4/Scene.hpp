#ifndef SCENE_H
#define SCENE_H

#define LAYER_COUNT    (9)
#define DEFORM_STORE   (256)
#define DEFORM_SIZE    (320)
#define DEFORM_COUNT   (DEFORM_STORE + DEFORM_SIZE)
#define PARALLAX_COUNT (0x100)

#if RETRO_VANILLA_LIKE
#define TILE_COUNT    (0x400)
#else
#define TILE_COUNT    (0x1000)
#endif
#define TILE_SIZE     (0x10)
#define CHUNK_SIZE    (0x80)
#define TILE_DATASIZE (TILE_SIZE * TILE_SIZE)
#define TILESET_SIZE  (TILE_COUNT * TILE_DATASIZE)

#define TILELAYER_CHUNK_W          (0x100)
#define TILELAYER_CHUNK_H          (0x100)
#define TILELAYER_CHUNK_COUNT      (TILELAYER_CHUNK_W * TILELAYER_CHUNK_H)
#define TILELAYER_LINESCROLL_COUNT (TILELAYER_CHUNK_H * CHUNK_SIZE)

#if RETRO_VANILLA_LIKE
#define CHUNKTILE_COUNT (0x200 * (8 * 8))
#else
#define CHUNKTILE_COUNT (0x400 * (8 * 8))
#endif

#define CPATH_COUNT (2)

#define DEFAULT_CAMERA_COUNT (DEFAULT_LOCAL_PLAYER_COUNT)

enum StageListNames {
    STAGELIST_PRESENTATION,
    STAGELIST_REGULAR,
    STAGELIST_BONUS,
    STAGELIST_SPECIAL,
    STAGELIST_MAX, // StageList size
};

enum TileLayerTypes {
    LAYER_NOSCROLL,
    LAYER_HSCROLL,
    LAYER_VSCROLL,
    LAYER_3DFLOOR,
    LAYER_3DSKY,
};

enum StageModes {
    STAGEMODE_LOAD,
    STAGEMODE_NORMAL,
    STAGEMODE_PAUSED,
    STAGEMODE_FROZEN,
#if !RETRO_REV00
    STAGEMODE_2P,
#endif

    STAGEMODE_NORMAL_STEP,
    STAGEMODE_PAUSED_STEP,
    STAGEMODE_FROZEN_STEP,
    STAGEMODE_2P_STEP,

    // put custom stagemodes here
#if RETRO_REV00
    STAGEMODE_2P, // just to not break rev00 bytecode using STAGEMODE_STEPs yknow
#endif
};

enum TileInfo {
    TILEINFO_INDEX,
    TILEINFO_DIRECTION,
    TILEINFO_VISUALPLANE,
    TILEINFO_SOLIDITYA,
    TILEINFO_SOLIDITYB,
    TILEINFO_FLAGSA,
    TILEINFO_ANGLEA,
    TILEINFO_FLAGSB,
    TILEINFO_ANGLEB,
};

enum DeformationModes {
    DEFORM_FG,
    DEFORM_FG_WATER,
    DEFORM_BG,
    DEFORM_BG_WATER,
};

enum CameraStyles {
    CAMERASTYLE_FOLLOW,
    CAMERASTYLE_EXTENDED,
    CAMERASTYLE_EXTENDED_OFFSET_L,
    CAMERASTYLE_EXTENDED_OFFSET_R,
    CAMERASTYLE_HLOCKED,
};

struct SceneInfo {
    char name[0x40];
    char folder[0x40];
    char id[0x40];
    bool highlighted;
};

struct CollisionMasks {
    sbyte floorMasks[TILE_COUNT * TILE_SIZE];
    sbyte lWallMasks[TILE_COUNT * TILE_SIZE];
    sbyte rWallMasks[TILE_COUNT * TILE_SIZE];
    sbyte roofMasks[TILE_COUNT * TILE_SIZE];
    uint angles[TILE_COUNT];
    byte flags[TILE_COUNT];
};

struct TileLayer {
    ushort tiles[TILELAYER_CHUNK_COUNT];
    byte lineScroll[TILELAYER_LINESCROLL_COUNT];
    int parallaxFactor;
    int scrollSpeed;
    int scrollPos;
    int angle;
    int xpos;
    int ypos;
    int zpos;
    int deformationOffset;
    int deformationOffsetW;
    byte type;
    byte xsize;
    byte ysize;
};

struct LineScroll {
    int parallaxFactor[PARALLAX_COUNT];
    int scrollSpeed[PARALLAX_COUNT];
    int scrollPos[PARALLAX_COUNT];
    int linePos[PARALLAX_COUNT];
    int deform[PARALLAX_COUNT];
    byte entryCount;
};

struct Tiles128x128 {
    int gfxDataPos[CHUNKTILE_COUNT];
    ushort tileIndex[CHUNKTILE_COUNT];
    byte direction[CHUNKTILE_COUNT];
    byte visualPlane[CHUNKTILE_COUNT];
    byte collisionFlags[CPATH_COUNT][CHUNKTILE_COUNT];
};

struct Camera {
    int target = -1;
    int style;
    int enabled;
    int adjustY;
    int xScrollOffset;
    int yScrollOffset;
    int xpos;
    int ypos;
    int shakeX;
    int shakeY;
    // drawx/drawy should both be the position of the top left corner of the camera, like rects
    int drawx; // xpos of the camera in the renderer
    int drawy;
    int xscale; // to make the camera smaller
    int yscale;
    
    // engine-only stuff
    int shift;	// extended camera shenanigans
    int lockedY;// Camera Y trailing i think
    
    // maybe someday
//    int adjustX;
	
	// additions for multiplayer
	int curXBoundary1;
	int curYBoundary1;
	int newXBoundary1;
	int newYBoundary1;
	int curXBoundary2;
	int curYBoundary2;
	int newXBoundary2;
	int newYBoundary2;
	
	// walter level stuff
	int waterLevel;   // not really used but what the hey
	int waterDrawPos; // same here
	
	// mirror mode shenanigans
	byte layerDir[DRAWLAYER_COUNT];
	byte direction;
};

extern int stageListCount[STAGELIST_MAX];
extern char stageListNames[STAGELIST_MAX][0x20];
extern SceneInfo stageList[STAGELIST_MAX][0x100];

extern int stageMode;

extern Camera camera[DEFAULT_CAMERA_COUNT];
extern int currentCamera;
extern int cameraCount;
extern int &curCam;
// these 2 were literally never used LMAO
//extern int cameraLag;
//extern int cameraLagStyle;

extern int SCREEN_SCROLL_LEFT;
extern int SCREEN_SCROLL_RIGHT;
#define SCREEN_SCROLL_UP   ((SCREEN_YSIZE / 2) - 16)
#define SCREEN_SCROLL_DOWN ((SCREEN_YSIZE / 2) + 16)

extern int lastXSize;
extern int lastYSize;

extern bool pauseEnabled;
extern bool timeEnabled;
extern bool debugMode;
extern int frameCounter;
extern int stageMilliseconds;
extern int stageSeconds;
extern int stageMinutes;

// Category and Scene IDs
extern int activeStageList;
extern int stageListPosition;
extern char currentStageFolder[0x100];
extern int actID;

extern char titleCardText[0x100];
extern byte titleCardWord2;

extern byte activeTileLayers[4];
extern byte tLayerMidPoint;
extern TileLayer stageLayouts[LAYER_COUNT];

extern int bgDeformationData0[DEFORM_COUNT];
extern int bgDeformationData1[DEFORM_COUNT];
extern int bgDeformationData2[DEFORM_COUNT];
extern int bgDeformationData3[DEFORM_COUNT];

extern LineScroll hParallax;
extern LineScroll vParallax;

extern Tiles128x128 tiles128x128;
extern CollisionMasks collisionMasks[2];

extern byte tilesetGFXData[TILESET_SIZE];

extern ushort tile3DFloorBuffer[0x100 * 0x100];
extern bool drawStageGFXHQ;

void InitFirstStage();
void InitStartingStage(int list, int stage, int player);
void ProcessStage();

void ProcessParallaxAutoScroll();

void ResetBackgroundSettings();
inline void ResetCurrentStageFolder() { strcpy(currentStageFolder, ""); }
inline bool CheckCurrentStageFolder(int stage)
{
    if (strcmp(currentStageFolder, stageList[activeStageList][stage].folder) == 0) {
        return true;
    }
    else {
        strcpy(currentStageFolder, stageList[activeStageList][stage].folder);
        return false;
    }
}

void LoadStageFiles();
int LoadActFile(const char *ext, int stageID, FileInfo *info);
int LoadStageFile(const char *filePath, int stageID, FileInfo *info);

void LoadActLayout();
void LoadStageBackground();
void LoadStageChunks();
void LoadStageCollisions();
void LoadStageGIFFile(int stageID);

inline void Init3DFloorBuffer(int layerID)
{
    for (int y = 0; y < TILELAYER_CHUNK_H; ++y) {
        for (int x = 0; x < TILELAYER_CHUNK_W; ++x) {
            int c                           = stageLayouts[layerID].tiles[(x >> 3) + (y >> 3 << 8)] << 6;
            int tx                          = x & 7;
            tile3DFloorBuffer[x + (y << 8)] = c + tx + ((y & 7) << 3);
        }
    }
}

inline void Copy16x16Tile(ushort dest, ushort src)
{
    byte *destPtr = &tilesetGFXData[TILELAYER_CHUNK_W * dest];
    byte *srcPtr  = &tilesetGFXData[TILELAYER_CHUNK_W * src];
    int cnt       = TILE_DATASIZE;
    while (cnt--) *destPtr++ = *srcPtr++;
}

void SetLayerDeformation(int selectedDef, int waveLength, int waveWidth, int waveType, int YPos, int waveSize);

void SetPlayerScreenPosition(Entity *target);
void SetPlayerScreenPositionCDStyle(Entity *target);
void SetPlayerHLockedScreenPosition(Entity *target);
void SetPlayerLockedScreenPosition(Entity *target);
void SetPlayerScreenPositionFixed(Entity *target);

#endif // !SCENE_H
