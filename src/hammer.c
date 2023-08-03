#include "../include/main.h"
#include "../include/evt.h"

/*
{
  Int size: 00000168
  Heap Ptr: 803150B0
}
*/

typedef struct HammerHitData {
    /* 0x00 */ Vec3f hitPos;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 hitID;
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 timer;
    /* 0x1C */ s32 unk_1C;
} HammerHitData;

HammerHitData D_802B6E90;
HammerHitData* HammerHit = &D_802B6E90;

EvtScript ShakeCamX = {
    EVT_SET_GROUP(EVT_GROUP_00)
    EVT_CALL(ShakeCam, LVar0, LVar1, LVar2, LVar3)
    EVT_RETURN
    EVT_END
};

void exec_ShakeCamX(s32 camID, s32 mode, s32 duration, f32 magnitude) {
    Evt* script;

    script = start_script_in_group(&ShakeCamX, EVT_PRIORITY_1, 0, EVT_GROUP_04); //allocates Evt struct to heap
    evt_set_variable(script, LVar0, camID);
    evt_set_variable(script, LVar1, mode);
    evt_set_variable(script, LVar2, duration);
    evt_set_float_variable(script, LVar3, magnitude);
}

void action_hammer_play_hit_fx(s32 hitID) {
    PlayerStatus* playerStatus = &gPlayerStatus;
    f32 shakeAmt;
    s32 time;
    s32 radius;
    s32 soundID;
    f32 theta, sinTheta, cosTheta;
    s32 numParticles;
    f32 x, y, z;

    if (gPlayerData.hammerLevel == 2) {
        shakeAmt = 1.2f;
        time = 1;
        radius = 28;
    } else if (gPlayerData.hammerLevel == 1) {
        shakeAmt = 0.8f;
        time = 1;
        radius = 16;
    } else {
        shakeAmt = 0.4f;
        time = 1;
        radius = 4;
    }

    theta = DEG_TO_RAD(func_800E5348());
    sinTheta = sin_rad(theta) * 25.0f;
    cosTheta = cos_rad(theta) * -25.0f;

    if (hitID < 0) {
        numParticles = 6;
        x = playerStatus->position.x + sinTheta;
        y = playerStatus->position.y;
        z = playerStatus->position.z + cosTheta;
    } else {
        numParticles = 3;
        x = HammerHit->hitPos.x + sinTheta;
        y = HammerHit->hitPos.y + playerStatus->colliderHeight - 5.0f;
        z = HammerHit->hitPos.z + cosTheta;
        radius = 1;
    }

    exec_ShakeCamX(0, 2, time, shakeAmt);
    fx_smoke_impact(0, x, y, z, radius, numParticles, 0, (time + 3) * 3);

    start_rumble(256, 50);
}