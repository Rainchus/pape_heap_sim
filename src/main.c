#include "../include/main.h"
#include "../include/evt.h"

PlayerData gPlayerData;
PlayerStatus gPlayerStatus;
extern u16 heap_nextMallocID;

ApiStatus ShakeCam(Evt* script, s32 isInitialCall) {
    Bytecode* args = script->ptrReadPos;
    s32 camIndex = evt_get_variable(script, *args++);
    s32 shakeMode = evt_get_variable(script, *args++);
    s32 duration = evt_get_variable(script, *args++);
    f32 magnitude = 2.0f * evt_get_float_variable(script, *args++);
    //Camera* camera = &gCameras[camIndex];
    f32 scale;
    f32 rumbleMagnitude;
    s32 rumbleDuration;

    if (isInitialCall) {
        switch (shakeMode){
            case CAM_SHAKE_CONSTANT_VERTICAL:
            case CAM_SHAKE_ANGULAR_HORIZONTAL:
                break;
            case CAM_SHAKE_DECAYING_VERTICAL:
                duration *= 4;
                break;
        }

        *(f32*)&script->functionTemp[3] = 1.0f; //TODO functionTempF ?
        script->functionTemp[1] = duration;

        // if (!gGameStatusPtr->isBattle) {
        //     if (magnitude > 10.0f) {
        //         magnitude = 10.0f;
        //     }

        //     rumbleMagnitude = magnitude;
        //     if (rumbleMagnitude > 6.0f) {
        //         rumbleMagnitude = 6.0f;
        //     }
        //     rumbleMagnitude = rumbleMagnitude * 32.0f + 64.0f;

        //     rumbleDuration = duration;

        //     if (rumbleDuration < 5) {
        //         rumbleDuration = 5;
        //     }
        //     start_rumble(rumbleMagnitude, (rumbleDuration & 0xFFFF) * 2);
        // }
    }

    //camera->flags |= CAMERA_FLAG_SHAKING;
    scale = script->functionTempF[3];
    switch (shakeMode) {
        case CAM_SHAKE_CONSTANT_VERTICAL:
            //guTranslateF(camera->viewMtxShaking, 0.0f, -scale * magnitude, 0.0f);
            script->functionTempF[3] = -script->functionTempF[3];
            break;
        case CAM_SHAKE_ANGULAR_HORIZONTAL:
            //guRotateF(camera->viewMtxShaking, scale * magnitude, 0.0f, 0.0f, 1.0f);
            script->functionTempF[3] = -script->functionTempF[3];
            break;
        case CAM_SHAKE_DECAYING_VERTICAL:
            //guTranslateF(camera->viewMtxShaking, 0.0f, -scale * magnitude, 0.0f);
            if ((script->functionTemp[1] < (duration * 2)) && (duration < script->functionTemp[1])) {
                script->functionTempF[3] = script->functionTempF[3] * -0.8;
            } else {
                script->functionTempF[3] = -script->functionTempF[3];
            }
            break;
    }

    if (script->functionTemp[1] == 0) {
        //camera->flags &= ~CAMERA_FLAG_SHAKING;
        return ApiStatus_DONE2;
    }
    script->functionTemp[1]--;
    return ApiStatus_BLOCK;
}

int main(void) {
    return 0;
}