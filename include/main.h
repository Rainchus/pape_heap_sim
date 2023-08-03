#ifndef _MAIN_H
#define _MAIN_H

#include "evt.h"

typedef struct Vec2f {
    /* 0x00 */ f32 x;
    /* 0x04 */ f32 y;
} Vec2f; // size = 0x08

typedef struct VecXZf {
    /* 0x00 */ f32 x;
    /* 0x04 */ f32 z;
} VecXZf; // size = 0x08

typedef struct Vec3f {
    /* 0x00 */ f32 x;
    /* 0x04 */ f32 y;
    /* 0x08 */ f32 z;
} Vec3f; // size = 0x0C

typedef struct Vec3s {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ s16 z;
} Vec3s; // size = 0x06

typedef struct HeapNode {
    /* 0x00 */ struct HeapNode* next;
    /* 0x04 */ u32 length;
    /* 0x08 */ u16 allocated;
    /* 0x0A */ u16 entryID;
    /* 0x0C */ u32 capacity;
} HeapNode; // size = 0x10

typedef struct PartnerData {
    /* 0x00 */ u8 enabled;
    /* 0x01 */ s8 level;
    /* 0x02 */ s16 unk_02[3];
} PartnerData; // size = 0x08

/// Ring buffer of an NPC's position over the past 20 frames.
typedef struct NpcMotionBlur {
    /* 0x00 */ s8 unk_00;
    /* 0x01 */ s8 index; ///< Current blur ring buffer index
    /* 0x02 */ char unk_02[2]; // padding?
    /* 0x04 */ f32 x[20];
    /* 0x54 */ f32 y[20];
    /* 0xA4 */ f32 z[20];
} NpcMotionBlur; // size = 0xF4

typedef struct NpcChompBlur {
    /* 0x00 */ struct Npc* npc;
    /* 0x04 */ Vec3f offset;
} NpcChompBlur; // size = 0x10;

typedef struct NpcQuizmoBlur {
    /* 0x00 */ s32 flags;
    /* 0x04 */ char unk_04[0x4];
} NpcQuizmoBlur; // size = 0x8;

typedef struct NpcHistoryPoint {
    /* 0x00 */ s8 isAirborne;
    /* 0x01 */ char unk_01[0x3];
    /* 0x04 */ Vec3f pos;
} NpcHistoryPoint; // size = 0x10

typedef struct FollowAnims {
    /* 0x00 */ AnimID walk;
    /* 0x04 */ AnimID jump;
    /* 0x08 */ AnimID fall;
    /* 0x0C */ AnimID land;
    /* 0x10 */ AnimID idle;
    /* 0x14 */ AnimID run;
} FollowAnims; // size = 0x18

typedef struct NpcFollowData {
    /* 0x000 */ NpcHistoryPoint moveHistory[40];
    /* 0x280 */ s32 lastPointIdx;
    /* 0x284 */ s32 targetPointIdx;
    /* 0x288 */ s32 followState;
    /* 0x28C */ s32 targetNpcID;
    /* 0x290 */ FollowAnims* anims;
    /* 0x294 */ f32 walkSpeed;
    /* 0x298 */ f32 runSpeed;
    /* 0x29C */ f32 idleRadius;
    /* 0x2A0 */ f32 walkRadius;
} NpcFollowData; // size = 0x2A4

typedef struct SlideParams {
    f32 heading;
    f32 maxDescendAccel;
    f32 launchVelocity;
    f32 maxDescendVelocity;
    f32 integrator[4];
} SlideParams;

typedef struct Npc {
    /* 0x000 */ s32 flags;
    /* 0x004 */ void (*onUpdate)(struct Npc*); ///< Run before anything else for this NPC in update_npcs()
    /* 0x008 */ void (*onRender)(struct Npc*); ///< Run after the display list for this NPC is built
    /* 0x00C */ f32 yaw;
    /* 0x010 */ f32 planarFlyDist; /* also used for speech, temp0? */
    /* 0x014 */ f32 jumpScale; /* also used for speech, temp1? */
    /* 0x018 */ f32 moveSpeed;
    /* 0x01C */ f32 jumpVelocity;
    /* 0x020 */ union {
                void* any;
                NpcMotionBlur* motion; ///< Null unless flag 0x100000 is set.
                NpcChompBlur*  chomp;
                NpcQuizmoBlur* quizmo;
                NpcFollowData* followData;
                struct Npc*    keepAwayNpc;
                s32*           keepAwayStarted;
                } blur;
    /* 0x024 */ s32 spriteInstanceID;
    /* 0x028 */ AnimID currentAnim;
    /* 0x02C */ s32 animNotifyValue;
    /* 0x030 */ f32 animationSpeed;
    /* 0x034 */ f32 renderYaw;
    /* 0x038 */ Vec3f pos;
    /* 0x044 */ Vec3f rotation;
    /* 0x050 */ f32 rotationPivotOffsetY;
    /* 0x054 */ Vec3f scale;
    /* 0x060 */ Vec3f moveToPos;
    /* 0x06C */ Vec3f colliderPos; /* used during collision with player */
    /* 0x078 */ s32 shadowIndex;
    /* 0x07C */ f32 shadowScale;
    /* 0x080 */ s32 collisionChannel; /* flags used with collision tracing */
    /* 0x084 */ s16 currentFloor; /* colliderID */
    /* 0x086 */ s16 currentWall; /* colliderID */
    /* 0x088 */ s16 isFacingAway;
    /* 0x08A */ s16 yawCamOffset;
    /* 0x08C */ s16 turnAroundYawAdjustment;
    /* 0x08E */ s16 duration; // TODO: name less vaguely
    /* 0x090 */ Vec3s homePos;
    /* 0x096 */ s16 unk_96;
    /* 0x098 */ s16 imgfxType;
    /* 0x09A */ s16 imgfxArg1;
    /* 0x09C */ s16 imgfxArg2;
    /* 0x09E */ s16 imgfxArg3;
    /* 0x0A0 */ s16 imgfxArg4;
    /* 0x0A2 */ u16 imgfxFlags;
    /* 0x0A4 */ s8 npcID;
    /* 0x0A5 */ char unk_A5;
    /* 0x0A6 */ s16 collisionDiameter;
    /* 0x0A8 */ s16 collisionHeight;
    /* 0x0AA */ s8 renderMode;
    /* 0x0AB */ s8 verticalRenderOffset;
    /* 0x0AC */ u8 alpha;
    /* 0x0AD */ u8 hideAlpha; ///< Used when hiding NPCs; Multiplied with Npc::alpha
    /* 0x0AE */ char unk_AE[2];
    /* 0x0B0 */ AnimID* extraAnimList;
    /* 0x0B4 */ s8 palSwapType; // 0..4 inclusive
    /* 0x0B5 */ s8 palSwapPrevType;
    /* 0x0B6 */ s8 dirtyPalettes;
    /* 0x0B7 */ s8 palSwapState;
    /* 0x0B8 */ char unk_B8[4];
    /* 0x0BC */ s16 palSwapTimer;
    /* 0x0BE */ s16 palSwapLerpAlpha;
    /* 0x0C0 */ s8 unk_C0;
    /* 0x0C1 */ s8 paletteCount;
    /* 0x0C2 */ char unk_C2[2];
    /* 0x0C4 */ PAL_PTR* spritePaletteList;
    /* 0x0C8 */ PAL_BIN localPaletteData[16][16];
    /* 0x2C8 */ PAL_PTR localPalettes[16];
    /* 0x308 */ s16 unk_308;
    /* 0x30A */ s16 unk_30A;
    /* 0x30C */ u16 unk_30C;
    /* 0x30E */ s16 unk_30E;
    /* 0x310 */ s16 unk_310;
    /* 0x312 */ s16 unk_312;
    /* 0x314 */ s16 unk_314;
    /* 0x316 */ s16 unk_316;
    /* 0x318 */ f32 screenSpaceOffset2D[2];
    /* 0x320 */ f32 verticalStretch;
    /* 0x324 */ struct EffectInstance* decorations[2];
    /* 0x32C */ s8 decorationType[2];
    /* 0x32E */ s8 changedDecoration[2];
    /* 0x330 */ s8 decorationInitialised[2];
    /* 0x332 */ s16 decorationUnk[2];
    /* 0x336 */ char unk_336[10];
} Npc; // size = 0x340

typedef struct PlayerData {
    /* 0x000 */ s8 bootsLevel;
    /* 0x001 */ s8 hammerLevel;
    /* 0x002 */ s8 curHP;
    /* 0x003 */ s8 curMaxHP;
    /* 0x004 */ s8 hardMaxHP;
    /* 0x005 */ s8 curFP;
    /* 0x006 */ s8 curMaxFP;
    /* 0x007 */ s8 hardMaxFP;
    /* 0x008 */ s8 maxBP;
    /* 0x009 */ s8 level;
    /* 0x00A */ b8 hasActionCommands;
    /* 0x00B */ char unk_0B;
    /* 0x00C */ s16 coins;
    /* 0x00E */ s8 fortressKeyCount;
    /* 0x00F */ u8 starPieces;
    /* 0x010 */ s8 starPoints;
    /* 0x011 */ char unk_11;
    /* 0x012 */ s8 currentPartner;
    /* 0x013 */ char unk_13;
    /* 0x014 */ struct PartnerData partners[12];
    /* 0x074 */ s16 keyItems[32];
    /* 0x0B4 */ s16 badges[128];
    /* 0x1B4 */ s16 invItems[10];
    /* 0x1C8 */ s16 storedItems[32];
    /* 0x208 */ s16 equippedBadges[64];
    /* 0x288 */ char unk_288;
    /* 0x289 */ s8 merleeSpellType;
    /* 0x28A */ s8 merleeCastsLeft;
    /* 0x28B */ char unk_28B;
    /* 0x28C */ s16 merleeTurnCount;
    /* 0x28E */ s8 maxStarPower;
    /* 0x28F */ char unk_28F;
    /* 0x290 */ s16 specialBarsFilled;
    /* 0x292 */ s8 starBeamLevel;
    /* 0x293 */ char unk_293;
    /* 0x294 */ u16 actionCommandAttempts;
    /* 0x296 */ s16 actionCommandSuccesses;
    /* 0x298 */ u16 hitsTaken;
    /* 0x29A */ u16 hitsBlocked;
    /* 0x29C */ u16 playerFirstStrikes;
    /* 0x29E */ u16 enemyFirstStrikes;
    /* 0x2A0 */ u16 powerBounces;
    /* 0x2A2 */ u16 battlesCount;
    /* 0x2A4 */ u16 battlesWon;
    /* 0x2A6 */ s16 unk_2A6;
    /* 0x2A8 */ s16 battlesFled;
    /* 0x2AA */ u16 trainingsDone;
    /* 0x2AC */ s32 walkingStepsTaken;
    /* 0x2B0 */ s32 runningStepsTaken;
    /* 0x2B4 */ u32 totalCoinsEarned;
    /* 0x2B8 */ s16 idleFrameCounter; /* frames with no inputs, overflows ever ~36 minutes of idling */
    /* 0x2BA */ char unk_2BA[2];
    /* 0x2BC */ u32 frameCounter; /* increases by 2 per frame */
    /* 0x2C0 */ u16 quizzesAnswered;
    /* 0x2C2 */ u16 quizzesCorrect;
    /* 0x2C4 */ s32 partnerUnlockedTime[12];
    /* 0x2F4 */ s32 partnerUsedTime[12];
    /* 0x324 */ s32 tradeEventStartTime;
    /* 0x328 */ s32 droTreeHintTime;
    /* 0x32C */ u16 starPiecesCollected;
    /* 0x32E */ u16 jumpGamePlays;
    /* 0x330 */ u32 jumpGameTotal; /* all-time winnings, max = 99999 */
    /* 0x334 */ u16 jumpGameRecord;
    /* 0x336 */ u16 smashGamePlays;
    /* 0x338 */ u32 smashGameTotal; /* all-time winnings, max = 99999 */
    /* 0x33C */ u16 smashGameRecord;
    /* 0x33E */ char unk_33E[2];
    /* 0x340 */ char unk_340[0xE0];
    /* 0x420 */ s32 starPoints2;
    /* 0x424 */ char unk_464[4];
} PlayerData; // size = 0x428

typedef struct PlayerStatus {
    /* 0x000 */ s32 flags; // PlayerStatusFlags
    /* 0x004 */ u32 animFlags;
    /* 0x008 */ s16 currentStateTime;
    /* 0x00A */ s8 shiverTime;
    /* 0x00B */ char unk_0B;
    /* 0x00C */ s8 peachDisguise;
    /* 0x00D */ s8 availableDisguiseType; ///< set in main map scripts
    /* 0x00E */ u8 alpha1;
    /* 0x00F */ u8 alpha2;
    /* 0x010 */ s16 blinkTimer;
    /* 0x012 */ s16 moveFrames;
    /* 0x014 */ s8 enableCollisionOverlapsCheck;
    /* 0x015 */ s8 inputDisabledCount; /* whether the C-up menu can appear */
    /* 0x016 */ Vec3s lastGoodPosition;
    /* 0x01C */ Vec3f pushVelocity;
    /* 0x028 */ Vec3f position;
    /* 0x034 */ Vec2f groundAnglesXZ; /* angles along X/Z axes of ground beneath player */
    /* 0x03C */ VecXZf jumpFromPos;
    /* 0x044 */ VecXZf landPos;
    /* 0x04C */ f32 jumpFromHeight;
    /* 0x050 */ f32 jumpApexHeight;
    /* 0x054 */ f32 currentSpeed;
    /* 0x058 */ f32 walkSpeed;
    /* 0x05C */ f32 runSpeed;
    /* 0x060 */ s32 unk_60;
    /* 0x064 */ f32 overlapPushAmount;
    /* 0x068 */ f32 groundNormalPitch;
    /* 0x06C */ f32 maxJumpSpeed;
    /* 0x070 */ f32 gravityIntegrator[4];
    /* 0x080 */ f32 targetYaw;
    /* 0x084 */ f32 currentYaw;
    /* 0x088 */ f32 overlapPushYaw;
    /* 0x08C */ f32 pitch;
    /* 0x090 */ f32 flipYaw[4];
    /* 0x0A0 */ f32 heading;
    /* 0x0A4 */ AnimID trueAnimation; ///< Encoding back-facing sprite
    /* 0x0A8 */ f32 spriteFacingAngle; /* angle of sprite, relative to camera, from 0 to 180 */
    /* 0x0AC */ char unk_AC[4];
    /* 0x0B0 */ s16 colliderHeight;
    /* 0x0B2 */ s16 colliderDiameter;
    /* 0x0B4 */ s8 actionState;
    /* 0x0B5 */ s8 prevActionState;
    /* 0x0B6 */ s8 actionSubstate;
    /* 0x0B7 */ char unk_B7;
    /* 0x0B8 */ AnimID anim;
    /* 0x0BC */ u16 animNotifyValue;
    /* 0x0BE */ s8 renderMode;
    /* 0x0BF */ s8 hazardType;
    /* 0x0C0 */ s16 timeInAir;
    /* 0x0C2 */ s16 peakJumpTime; // frame of current jump when player Y velocity went from position to negative
    /* 0x0C4 */ s8 peachItemHeld;
    /* 0x0C5 */ s8 camResetDelay;
    /* 0x0C6 */ s16 interactingWithID;
    /* 0x0C8 */ Npc* encounteredNPC;
    /* 0x0CC */ s32 shadowID;
    /* 0x0D0 */ SlideParams* slideParams;
    /* 0x0D4 */ f32 spinRate;
    /* 0x0D8 */ struct EffectInstance* specialDecorationEffect;
    /* 0x0DC */ s32 currentButtons;
    /* 0x0E0 */ s32 pressedButtons;
    /* 0x0E4 */ s32 heldButtons;
    /* 0x0E8 */ s32 stickAxis[2];
    /* 0x0F0 */ s32 currentButtonsBuffer[10];
    /* 0x118 */ s32 pressedButtonsBuffer[10];
    /* 0x140 */ s32 heldButtonsBuffer[10];
    /* 0x168 */ s32 stickXBuffer[10];
    /* 0x190 */ s32 stickYBuffer[10];
    /* 0x1B8 */ s32 inputBufPos;
    /* 0x1BC */ char unk_1BC[196];
    /* 0x280 */ s8 poundImpactDelay; // governs period of immobility after landing a ground pound
    /* 0x281 */ char unk_281[7];
} PlayerStatus; // size = 0x288

typedef struct SaveMetadata {
    /* 0x00 */ s32 timePlayed;
    /* 0x04 */ u8 spiritsRescued;
    /* 0x05 */ char unk_05[0x1];
    /* 0x06 */ s8 level;
    /* 0x07 */ unsigned char filename[8];
    /* 0x0F */ char unk_0F[0x1];
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
} SaveMetadata; // size = 0x18

typedef struct SaveData {
    /* 0x0000 */ char magicString[16]; /* "Mario Story 006" string */
    /* 0x0010 */ s8 pad[32]; /* always zero */
    /* 0x0030 */ s32 crc1;
    /* 0x0034 */ s32 crc2;
    /* 0x0038 */ s32 saveSlot;
    /* 0x003C */ s32 saveCount;
    /* 0x0040 */ PlayerData player;
    /* 0x0468 */ s16 areaID;
    /* 0x046A */ s16 mapID;
    /* 0x046C */ s16 entryID;
    /* 0x046E */ char unk_46E[2];
    /* 0x0470 */ s32 enemyDefeatFlags[60][12];
    /* 0x0FB0 */ s32 globalFlags[64];
    /* 0x10B0 */ s8 globalBytes[512];
    /* 0x12B0 */ s32 areaFlags[8];
    /* 0x12D0 */ s8 areaBytes[16];
    /* 0x12E0 */ s8 debugEnemyContact;
    /* 0x12E0 */ s8 unk_12E1;
    /* 0x12E0 */ s8 unk_12E2;
    /* 0x12E0 */ s8 musicEnabled;
    /* 0x12E4 */ char unk_12E4[0x2];
    /* 0x12E6 */ Vec3s savePos;
    /* 0x12EC */ SaveMetadata unk_12EC;
    /* 0x1304 */ char unk_1304[0x7C];
} SaveData; // size = 0x1380

enum CamShakeModes {
    CAM_SHAKE_CONSTANT_VERTICAL     = 0,
    CAM_SHAKE_ANGULAR_HORIZONTAL    = 1,
    CAM_SHAKE_DECAYING_VERTICAL     = 2
};

#define PANIC() IS_DEBUG_PANIC("Panic", __FILE__, __LINE__)
#define ASSERT(condition) \
    if (!(condition)) { \
        IS_DEBUG_PANIC("Assertion failed: " #condition, __FILE__, __LINE__); \
    }

ApiStatus ShakeCam(Evt* script, s32 isInitialCall);
Evt* start_script_in_group(EvtScript* source, u8 priority, u8 flags, u8 groupFlags);
s32 evt_set_variable(Evt* script, Bytecode var, s32 value);
f32 evt_set_float_variable(Evt* script, Bytecode var, f32 value);

extern PlayerData gPlayerData;
extern PlayerStatus gPlayerStatus;

#endif