#include "../include/main.h"
#include "../include/evt.h"

#define MAX_SCRIPTS 128
/// Progammatically converts EVT_FLOAT --> f32
#define EVT_FIXED_TO_FLOAT(x) ({f32 var = (x) + EVT_FIXED_OFFSET; var /= 1024.0f; var;})

s32 gNumScripts;
s32 gScriptListCount;
s32 gStaticScriptCounter = 1;
f32 gGlobalTimeSpace = 1.0f;
s32 gIsUpdatingScripts = 0;
s32 gScriptIndexList[MAX_SCRIPTS];
s32 gScriptIdList[MAX_SCRIPTS];
typedef Evt* ScriptList[MAX_SCRIPTS];
ScriptList* gCurrentScriptListPtr;

u32* gMapFlags;
s32* gMapVars;

SaveData gCurrentSaveFile;

f32 func_800E5348(void) {
    f32 deltaYaw = get_clamped_angle_diff(gCameras[gCurrentCameraID].currentYaw, gPlayerStatus.currentYaw);

    if (deltaYaw < -5.0f && deltaYaw > -175.0f) {
        deltaYaw = 0.0f;
    } else if (deltaYaw > 5.0f && deltaYaw < 175.0f) {
        deltaYaw = 180.0f;
    } else {
        deltaYaw = PrevPlayerCamRelativeYaw;
    }
    return clamp_angle(deltaYaw - 90.0f + gCameras[gCurrentCameraID].currentYaw);
}

s32 set_global_flag(s32 index) {
    SaveData* saveFile;
    s32 wordIdx;
    s32 bitIdx;
    s32 flag;

    if (index <= EVT_GAME_FLAG_CUTOFF) {
        index = EVT_INDEX_OF_GAME_FLAG(index);
    }

    wordIdx = index / 32;
    bitIdx = index % 32;

    saveFile = &gCurrentSaveFile;
    flag = saveFile->globalFlags[wordIdx] & (1 << bitIdx);

    if (flag != 0) {
        flag = 1;
    }

    saveFile->globalFlags[wordIdx] |= (1 << bitIdx);
    return flag;
}

s32 get_global_flag(s32 index) {
    s32 wordIdx;
    s32 bitIdx;
    s32 flag;

    if (index <= EVT_GAME_FLAG_CUTOFF) {
        index = EVT_INDEX_OF_GAME_FLAG(index);
    }

    wordIdx = index / 32;
    bitIdx = index % 32;
    flag = gCurrentSaveFile.globalFlags[wordIdx] & (1 << bitIdx);

    if (flag != 0) {
        flag = 1;
    }
    return flag;
}

s8 set_global_byte(s32 index, s32 value) {
    SaveData* saveFile = &gCurrentSaveFile;
    s32 ret = saveFile->globalBytes[index];

    saveFile->globalBytes[index] = value;
    return ret;
}

s32 get_global_byte(s32 index) {
    return gCurrentSaveFile.globalBytes[index];
}

f32 evt_fixed_var_to_float(Bytecode scriptVar) {
    if (scriptVar <= EVT_FIXED_CUTOFF) {
        return EVT_FIXED_TO_FLOAT(scriptVar);
    } else {
        return scriptVar;
    }
}

s32 evt_get_variable(Evt* script, Bytecode var) {
    s32 wordIdx;
    s32 bitIdx;
    s32 temp;

    if (var <= EVT_LIMIT) {
        return var;
    } else if (var <= EVT_IGNORE_ARG) {
        return var;
    } else if (var <= EVT_FIXED_CUTOFF) {
        return evt_fixed_var_to_float(var);
    } else if (var <= EVT_ARRAY_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_ARRAY_FLAG(var);
        wordIdx = var / 32;
        bitIdx = var % 32;
        var = (script->flagArray[wordIdx] & (1 << bitIdx)) != 0;
        return var;
    } else if (var <= EVT_ARRAY_VAR_CUTOFF) {
        var = EVT_INDEX_OF_ARRAY_VAR(var);
        var = script->array[var];
        if (var > EVT_LIMIT) {
            if (var <= EVT_FIXED_CUTOFF){
                var = evt_fixed_var_to_float(var);
            }
        }
    } else if (var <= EVT_GAME_BYTE_CUTOFF) {
        var = EVT_INDEX_OF_GAME_BYTE(var);
        return get_global_byte(var);
    } else if (var <= EVT_AREA_BYTE_CUTOFF) {
        var = EVT_INDEX_OF_AREA_BYTE(var);
        return get_area_byte(var);
    } else if (var <= EVT_GAME_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_GAME_FLAG(var);
        return get_global_flag(var);
    } else if (var <= EVT_AREA_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_AREA_FLAG(var);
        return get_area_flag(var);
    } else if (var <= EVT_MAP_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_MAP_FLAG(var);
        wordIdx = var;
        bitIdx = var % 32;
        var = (gMapFlags[wordIdx / 32] & (1 << bitIdx)) != 0;
        return var;
    } else if (var <= EVT_LOCAL_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_LOCAL_FLAG(var);
        wordIdx = var;
        bitIdx = var % 32;
        var = (script->varFlags[wordIdx / 32] & (1 << bitIdx)) != 0;
        return var;
    } else if (var <= EVT_MAP_VAR_CUTOFF) {
        var = EVT_INDEX_OF_MAP_VAR(var);
        var = gMapVars[var];
        if (var > EVT_LIMIT) {
            temp = EVT_FIXED_CUTOFF;
            if (var <= temp){
                var = evt_fixed_var_to_float(var);
            }
        }
    } else if (var <= EVT_LOCAL_VAR_CUTOFF) {
        var = EVT_INDEX_OF_LOCAL_VAR(var);
        var = script->varTable[var];
        if (var > EVT_LIMIT) {
            temp = EVT_FIXED_CUTOFF;
            if (var <= temp){
                var = evt_fixed_var_to_float(var);
            }
        }
    }
        return var;
}

s32 evt_get_variable_index(Evt* script, s32 var) {
    if (EVT_LIMIT >= var) {
        return var;
    }
    if (EVT_IGNORE_ARG >= var) {
        return var;
    }
    if (EVT_FIXED_CUTOFF >= var) {
        return var;
    }
    if (EVT_ARRAY_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_ARRAY_FLAG(var);
    }
    if (EVT_ARRAY_VAR_CUTOFF >= var) {
        return EVT_INDEX_OF_ARRAY_VAR(var);
    }
    if (EVT_GAME_BYTE_CUTOFF >= var) {
        return EVT_INDEX_OF_GAME_BYTE(var);
    }
    if (EVT_AREA_BYTE_CUTOFF >= var) {
        return EVT_INDEX_OF_AREA_BYTE(var);
    }
    if (EVT_GAME_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_GAME_FLAG(var);
    }
    if (EVT_AREA_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_AREA_FLAG(var);
    }
    if (EVT_MAP_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_MAP_FLAG(var);
    }
    if (EVT_LOCAL_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_LOCAL_FLAG(var);
    }
    if (EVT_MAP_VAR_CUTOFF >= var) {
        return EVT_INDEX_OF_MAP_VAR(var);
    }
    if (EVT_LOCAL_VAR_CUTOFF >= var) {
        return EVT_INDEX_OF_LOCAL_VAR(var);
    }
    return var;
}

s32 evt_get_variable_index_alt(s32 var) {
    if (EVT_LIMIT >= var) {
        return var;
    }
    if (EVT_IGNORE_ARG >= var) {
        return var;
    }
    if (EVT_FIXED_CUTOFF >= var) {
        return var;
    }
    if (EVT_ARRAY_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_ARRAY_FLAG(var);
    }
    if (EVT_ARRAY_VAR_CUTOFF >= var) {
        return EVT_INDEX_OF_ARRAY_VAR(var);
    }
    if (EVT_GAME_BYTE_CUTOFF >= var) {
        return EVT_INDEX_OF_GAME_BYTE(var);
    }
    if (EVT_AREA_BYTE_CUTOFF >= var) {
        return EVT_INDEX_OF_AREA_BYTE(var);
    }
    if (EVT_GAME_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_GAME_FLAG(var);
    }
    if (EVT_AREA_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_AREA_FLAG(var);
    }
    if (EVT_MAP_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_MAP_FLAG(var);
    }
    if (EVT_LOCAL_FLAG_CUTOFF >= var) {
        return EVT_INDEX_OF_LOCAL_FLAG(var);
    }
    if (EVT_MAP_VAR_CUTOFF >= var) {
        return EVT_INDEX_OF_MAP_VAR(var);
    }
    if (EVT_LOCAL_VAR_CUTOFF >= var) {
        return EVT_INDEX_OF_LOCAL_VAR(var);
    }
    return var;
}

s32 evt_set_variable(Evt* script, Bytecode var, s32 value) {
    s32 flagBitPos;
    s32 oldValue;

    if (var <= EVT_LIMIT) {
        return value;
    } else if (var <= EVT_FIXED_CUTOFF) {
        return evt_fixed_var_to_float(value);
    } else if (var <= EVT_ARRAY_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_ARRAY_FLAG(var);
        flagBitPos = var % 32;
        if (value) {
            script->flagArray[var / 32] |= 1 << flagBitPos;
        } else {
            script->flagArray[var / 32] &= ~(1 << flagBitPos);
        }
        return value;
    } else if (var <= EVT_ARRAY_VAR_CUTOFF) {
        var = EVT_INDEX_OF_ARRAY_VAR(var);
        oldValue = script->array[var];
        script->array[var] = value;
        return oldValue;
    } else if (var <= EVT_GAME_BYTE_CUTOFF) {
        var = EVT_INDEX_OF_GAME_BYTE(var);
        oldValue = get_global_byte(var);
        set_global_byte(var, value);
        return oldValue;
    } else if (var <= EVT_AREA_BYTE_CUTOFF) {
        var = EVT_INDEX_OF_AREA_BYTE(var);
        oldValue = get_area_byte(var);
        set_area_byte(var, value);
        return oldValue;
    } else if (var <= EVT_GAME_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_GAME_FLAG(var);
        oldValue = get_global_flag(var);
        if (value) {
            set_global_flag(var);
        } else {
            clear_global_flag(var);
        }
        return oldValue;
    } else if (var <= EVT_AREA_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_AREA_FLAG(var);
        oldValue = get_area_flag(var);
        if (value) {
            set_area_flag(var);
        } else {
            clear_area_flag(var);
        }
        return oldValue;
    } else if (var <= EVT_MAP_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_MAP_FLAG(var);
        flagBitPos = var % 32;
        if (value) {
            gMapFlags[var / 32] |= 1 << flagBitPos;
        } else {
            gMapFlags[var / 32] &= ~(1 << flagBitPos);
        }
        return value;
    } else if (var <= EVT_LOCAL_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_LOCAL_FLAG(var);
        flagBitPos = var % 32;
        if (value) {
            script->varFlags[var / 32] |= 1 << flagBitPos;
        } else {
            script->varFlags[var / 32] &= ~(1 << flagBitPos);
        }
        return value;
    } else if (var <= EVT_MAP_VAR_CUTOFF) {
        var = EVT_INDEX_OF_MAP_VAR(var);
        oldValue = gMapVars[var];
        gMapVars[var] = value;
        return oldValue;
    } else if (var <= EVT_LOCAL_VAR_CUTOFF) {
        var = EVT_INDEX_OF_LOCAL_VAR(var);
        oldValue = script->varTable[var];
        script->varTable[var] = value;
        return oldValue;
    } else {
        return value;
    }
}

f32 evt_get_float_variable(Evt* script, Bytecode var) {
    s32 temp;

    if (var <= EVT_LIMIT) {
        return var;
    } else if (var <= EVT_IGNORE_ARG) {
        return var;
    } else if (var <= EVT_FIXED_CUTOFF) {
        return evt_fixed_var_to_float(var);
    } else if (var <= EVT_ARRAY_VAR_CUTOFF) {
        var = EVT_INDEX_OF_ARRAY_VAR(var);
        return evt_fixed_var_to_float(script->array[var]);
    } else if (var <= EVT_GAME_BYTE_CUTOFF) {
        var = EVT_INDEX_OF_GAME_BYTE(var);
        return evt_fixed_var_to_float(get_global_byte(var));
    } else if (var <= EVT_AREA_BYTE_CUTOFF) {
        var = EVT_INDEX_OF_AREA_BYTE(var);
        return evt_fixed_var_to_float(get_area_byte(var));
    } else if (var <= EVT_MAP_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_MAP_FLAG(var);
        temp = var % 32;
        var = gMapFlags[var / 32] & (1 << temp);
        if (var) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    } else if (var <= EVT_LOCAL_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_LOCAL_FLAG(var);
        temp = var % 32;
        var = script->varFlags[var / 32] & (1 << temp);
        if (var) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    } else if (var <= EVT_MAP_VAR_CUTOFF) {
        var = EVT_INDEX_OF_MAP_VAR(var);
        return evt_fixed_var_to_float(gMapVars[var]);
    } else if (var <= EVT_LOCAL_VAR_CUTOFF) {
        var = EVT_INDEX_OF_LOCAL_VAR(var);
        return evt_fixed_var_to_float(script->varTable[var]);
    } else {
        return evt_fixed_var_to_float(var);
    }
}

f32 evt_set_float_variable(Evt* script, Bytecode var, f32 value) {
    s32 temp;
    s32 oldValue;

    if (var <= EVT_LIMIT) {
        return value;
    } else if (var <= EVT_FIXED_CUTOFF) {
        return value;
    } else if (var <= EVT_ARRAY_VAR_CUTOFF) {
        var = EVT_INDEX_OF_ARRAY_VAR(var);
        oldValue = script->array[var];
        script->array[var] = evt_float_to_fixed_var(value);
        return evt_fixed_var_to_float(oldValue);
    } else if (var <= EVT_MAP_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_MAP_FLAG(var);
        temp = var % 32;
        if (value) {
            gMapFlags[var / 32] |= 1 << temp;
        } else {
            gMapFlags[var / 32] &= ~(1 << temp);
        }
        return value;
    } else if (var <= EVT_LOCAL_FLAG_CUTOFF) {
        var = EVT_INDEX_OF_LOCAL_FLAG(var);
        temp = var % 32;
        if (value) {
            script->varFlags[var / 32] |= 1 << temp;
        } else {
            script->varFlags[var / 32] &= ~(1 << temp);
        }
        return value;
    } else if (var <= EVT_MAP_VAR_CUTOFF) {
        var = EVT_INDEX_OF_MAP_VAR(var);
        oldValue = gMapVars[var];
        gMapVars[var] = evt_float_to_fixed_var(value);
        return evt_fixed_var_to_float(oldValue);
    } else if (var <= EVT_LOCAL_VAR_CUTOFF) {
        var = EVT_INDEX_OF_LOCAL_VAR(var);
        oldValue = script->varTable[var];
        script->varTable[var] = evt_float_to_fixed_var(value);
        return evt_fixed_var_to_float(oldValue);
    } else {
        return value;
    }
}

Evt* start_script_in_group(EvtScript* source, u8 priority, u8 flags, u8 groupFlags) {
    Evt* newScript;
    s32 scriptListCount;
    s32 i;
    s32 curScriptIndex;
    s32* tempCounter;
    s32* numScripts;

    for (i = 0; i < MAX_SCRIPTS; i++) {
        if ((*gCurrentScriptListPtr)[i] == NULL) {
            break;
        }
    }

    ASSERT(i < MAX_SCRIPTS);
    curScriptIndex = i;

    (*gCurrentScriptListPtr)[curScriptIndex] = newScript = heap_malloc(sizeof(*newScript));
    numScripts = &gNumScripts;
    (*numScripts)++;
    ASSERT(newScript != NULL);

    // Some of this function is surely macros. I think we'll learn more as we do others in this file. -Ethan
    do {
        newScript->stateFlags = flags | EVT_FLAG_ACTIVE;
        newScript->currentOpcode = EVT_OP_INTERNAL_FETCH;
        newScript->priority = priority;
        newScript->id = gStaticScriptCounter++;
        newScript->ptrNextLine = (Bytecode*)source;
        newScript->ptrFirstLine = (Bytecode*)source;
        newScript->ptrCurrentLine = (Bytecode*)source;
        newScript->userData = 0;
        newScript->blockingParent = 0;
        newScript->childScript = 0;
        newScript->parentScript = 0;
        newScript->owner1.actorID = -1;
        newScript->owner2.npcID = -1;
        newScript->loopDepth = -1;
        newScript->switchDepth = -1;
        newScript->groupFlags = groupFlags;
        newScript->ptrSavedPos = 0;
        newScript->frameCounter = 0.0f;
        newScript->unk_158 = 0;
        newScript->timeScale = gGlobalTimeSpace;
        scriptListCount = 0;

        for (i = 0; i < ARRAY_COUNT(newScript->varTable); i++) {
            newScript->varTable[i] = 0;
        }
        for (i = 0; i < ARRAY_COUNT(newScript->varFlags); i++) {
            newScript->varFlags[i] = 0;
        }

        find_script_labels(newScript);

        if (gIsUpdatingScripts && (newScript->stateFlags & EVT_FLAG_RUN_IMMEDIATELY)) {
            scriptListCount = gScriptListCount++;
            gScriptIndexList[scriptListCount] = curScriptIndex;
            gScriptIdList[scriptListCount] = newScript->id;
        }
    } while (0);

    suspend_frozen_scripts(newScript);

    tempCounter = &gStaticScriptCounter;
    if (*tempCounter == 0) {
        *tempCounter = 1;
    }

    return newScript;
}