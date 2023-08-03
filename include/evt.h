#ifndef _EVT_H
#define _EVT_H

#include "types.h"

struct Evt;

/* Return type of script API functions */
#define ApiStatus_BLOCK  0   /* Call again next frame */
#define ApiStatus_DONE1  1   /* Unconditional. Probably only exists to return a bool from functions */
#define ApiStatus_DONE2  2   /* Conditional on Evt->disableScripts */
#define ApiStatus_REPEAT 3   /* Call again immediately */
#define ApiStatus_FINISH 255 /* Corresponds to EVT_FINISH */

enum EventPriority {
    EVT_PRIORITY_0  = 0x00, // map main script
    EVT_PRIORITY_1  = 0x01,
    EVT_PRIORITY_A  = 0x0A,
    EVT_PRIORITY_14 = 0x14,
};

/// Decimal constant.
/// Despite the name, "floats" are actually stored as fixed-point values.

#define EVT_LOCAL_VAR_CUTOFF     -20000000
#define EVT_LOCAL_VAR_OFFSET      30000000
#define EVT_MAP_VAR_CUTOFF       -40000000
#define EVT_MAP_VAR_OFFSET        50000000
#define EVT_LOCAL_FLAG_CUTOFF    -60000000
#define EVT_LOCAL_FLAG_OFFSET     70000000
#define EVT_MAP_FLAG_CUTOFF      -80000000
#define EVT_MAP_FLAG_OFFSET       90000000
#define EVT_AREA_FLAG_CUTOFF    -100000000
#define EVT_AREA_FLAG_OFFSET     110000000
#define EVT_GAME_FLAG_CUTOFF    -120000000
#define EVT_GAME_FLAG_OFFSET     130000000
#define EVT_AREA_BYTE_CUTOFF    -140000000
#define EVT_AREA_BYTE_OFFSET     150000000
#define EVT_GAME_BYTE_CUTOFF    -160000000
#define EVT_GAME_BYTE_OFFSET     170000000
#define EVT_ARRAY_VAR_CUTOFF    -180000000
#define EVT_ARRAY_VAR_OFFSET     190000000
#define EVT_ARRAY_FLAG_CUTOFF   -200000000
#define EVT_ARRAY_FLAG_OFFSET    210000000
#define EVT_FIXED_CUTOFF        -220000000
#define EVT_FIXED_OFFSET         230000000
#define EVT_IGNORE_ARG          -250000000 // used by a couple functions to selectively ignore args
#define EVT_LIMIT               -270000000 // TODO better name

enum EventGroupFlags {
    EVT_GROUP_00    = 0x00,
    EVT_GROUP_0A    = 0x0A, // 8 | 2
    EVT_GROUP_0B    = 0x0B, // 8 | 4 | 1
    EVT_GROUP_1B    = 0x1B, // 10 | 8 | 4 | 1
    EVT_GROUP_EF    = 0xEF, // ~10
    EVT_GROUP_01    = 0x01,
    EVT_GROUP_02    = 0x02,
    EVT_GROUP_04    = 0x04,
    EVT_GROUP_08    = 0x08,
    EVT_GROUP_10    = 0x10,
};

enum {
    EVT_OP_INTERNAL_FETCH,
    EVT_OP_END,
    EVT_OP_RETURN,
    EVT_OP_LABEL, ///< Args: index
    EVT_OP_GOTO, ///< Args: index
    EVT_OP_LOOP, ///< Args: number of repeats (0 = infinite)
    EVT_OP_END_LOOP,
    EVT_OP_BREAK_LOOP,
    EVT_OP_WAIT_FRAMES,
    EVT_OP_WAIT_SECS,
    EVT_OP_IF_EQ, ///< Args: a, b
    EVT_OP_IF_NE, ///< Args: a, b
    EVT_OP_IF_LT, ///< Args: a, b
    EVT_OP_IF_GT, ///< Args: a, b
    EVT_OP_IF_LE, ///< Args: a, b
    EVT_OP_IF_GE, ///< Args: a, b
    EVT_OP_IF_FLAG, ///< Args: a, b
    EVT_OP_IF_NOT_FLAG, ///< Args: a, b
    EVT_OP_ELSE,
    EVT_OP_END_IF,
    EVT_OP_SWITCH, ///< Args: expression to test against
    EVT_OP_SWITCH_CONST, ///< Args: value to test against
    EVT_OP_CASE_EQ, ///< Args: expression to test for
    EVT_OP_CASE_NE, ///< Args: expression to test for
    EVT_OP_CASE_LT, ///< Args: expression to test for
    EVT_OP_CASE_GT, ///< Args: expression to test for
    EVT_OP_CASE_LE, ///< Args: expression to test for
    EVT_OP_CASE_GE, ///< Args: expression to test for
    EVT_OP_CASE_DEFAULT,
    EVT_OP_CASE_OR_EQ, ///< Args: expression to test for
    EVT_OP_CASE_AND_EQ, ///< Args: expression to test for
    EVT_OP_CASE_FLAG, ///< Args: expression to test for
    EVT_OP_END_CASE_GROUP, ///< Ends the case block of EVT_OP_CASE_OR_EQ condition(s).
    EVT_OP_CASE_RANGE, ///< Args: from, to
    EVT_OP_BREAK_SWITCH,
    EVT_OP_END_SWITCH,
    EVT_OP_SET, ///< Args: container, expression
    EVT_OP_SET_CONST, ///< Args: container, value
    EVT_OP_SETF, ///< Args: container, expression
    EVT_OP_ADD, ///< Args: container, expression to increment by
    EVT_OP_SUB, ///< Args: container, expression to decrement by
    EVT_OP_MUL, ///< Args: container, expression to multiply by
    EVT_OP_DIV, ///< Integer division. Args: container, expression to divide by
    EVT_OP_MOD, ///< Args: container, expression to divide by
    EVT_OP_ADDF, ///< Args: container, expression to increment by
    EVT_OP_SUBF, ///< Args: container, expression to decrement by
    EVT_OP_MULF, ///< Args: container, expression to multiply by
    EVT_OP_DIVF, ///< Args: container, expression to divide by
    EVT_OP_USE_BUF, ///< Args: s32*
    EVT_OP_BUF_READ1, /// Args: container
    EVT_OP_BUF_READ2, /// Args: container, container
    EVT_OP_BUF_READ3, /// Args: container, container, container
    EVT_OP_BUF_READ4, /// Args: container, container, container, container
    EVT_OP_BUF_PEEK, ///< Args: index, container
    EVT_OP_USE_FBUF, ///< Identical to USE_BUFFER. Args: f32*
    EVT_OP_FBUF_READ1, /// Args: container
    EVT_OP_FBUF_READ2, /// Args: container, container
    EVT_OP_FBUF_READ3, /// Args: container, container, container
    EVT_OP_FBUF_READ4, /// Args: container, container, container, container
    EVT_OP_FBUF_PEEK, ///< Args: index, container
    EVT_OP_USE_ARRAY, ///< Args: *s32
    EVT_OP_USE_FLAGS, ///< Args: *s32
    EVT_OP_MALLOC_ARRAY, ///< Allocates a new array. Args: length, s32*
    EVT_OP_BITWISE_AND, ///< Args: container, expression to bitwise AND with
    EVT_OP_BITWISE_AND_CONST, ///< Args: container, value to bitwise AND with
    EVT_OP_BITWISE_OR, ///< Args: container, expression to bitwise OR with
    EVT_OP_BITWISE_OR_CONST, ///< Args: container, value to bitwise OR with
    EVT_OP_CALL, ///< Args: *function, ...
    EVT_OP_EXEC, ///< Args: EvtScript*
    EVT_OP_EXEC_GET_TID, ///< Args: EvtScript*, container
    EVT_OP_EXEC_WAIT, ///< Spawns a script and waits for it to return before continuing. Args: EvtScript*
    EVT_OP_BIND_TRIGGER, ///< Args: EvtScript*, trigger flags, s32 target, 1, Trigger*
    EVT_OP_UNBIND, ///< Unbinds any triggers bound to this script.
    EVT_OP_KILL_THREAD, ///< Args: ScriptID
    EVT_OP_JUMP, ///< Args: EvtScript*
    EVT_OP_SET_PRIORITY, ///< Args: priority
    EVT_OP_SET_TIMESCALE, ///< Args: timescale
    EVT_OP_SET_GROUP, ///< Args: group
    EVT_OP_BIND_PADLOCK, ///< Args: EvtScript*, trigger flags, s32 target, ItemList*, 0, 1
    EVT_OP_SUSPEND_GROUP, ///< Args: group
    EVT_OP_RESUME_GROUP, ///< Args: group
    EVT_OP_SUSPEND_OTHERS, ///< Args: group
    EVT_OP_RESUME_OTHERS, ///< Args: group
    EVT_OP_SUSPEND_THREAD, ///< Args: ScriptID
    EVT_OP_RESUME_THREAD, ///< Args: ScriptID
    EVT_OP_IS_THREAD_RUNNING, ///< Args: ScriptID, container
    EVT_OP_THREAD,
    EVT_OP_END_THREAD,
    EVT_OP_CHILD_THREAD, ///< Parallel threads are killed as soon as the parent script returns.
    EVT_OP_END_CHILD_THREAD,
    EVT_OP_DEBUG_LOG,
    EVT_OP_DEBUG_PRINT_VAR, ///< Args: expression
    EVT_OP_92,
    EVT_OP_93,
    EVT_OP_94,
};

enum EventStateFlags {
    EVT_FLAG_ACTIVE             = 0x01,
    EVT_FLAG_SUSPENDED_IN_GROUP = 0x02,
    EVT_FLAG_BLOCKED_BY_CHILD   = 0x10,
    EVT_FLAG_RUN_IMMEDIATELY    = 0x20, ///< don't wait for next `update_scripts` call
    EVT_FLAG_THREAD             = 0x40,
    EVT_FLAG_SUSPENDED          = 0x80, ///< doesn't affect child
};

// inverse macros for obtaining the index of a variable from its encoded form
#define EVT_INDEX_OF_LOCAL_VAR(v)   ((v) + EVT_LOCAL_VAR_OFFSET)
#define EVT_INDEX_OF_LOCAL_FLAG(v)  ((v) + EVT_LOCAL_FLAG_OFFSET)
#define EVT_INDEX_OF_MAP_VAR(v)     ((v) + EVT_MAP_VAR_OFFSET)
#define EVT_INDEX_OF_MAP_FLAG(v)    ((v) + EVT_MAP_FLAG_OFFSET)
#define EVT_INDEX_OF_AREA_FLAG(v)   ((v) + EVT_AREA_FLAG_OFFSET)
#define EVT_INDEX_OF_AREA_BYTE(v)   ((v) + EVT_AREA_BYTE_OFFSET)
#define EVT_INDEX_OF_GAME_FLAG(v)   ((v) + EVT_GAME_FLAG_OFFSET)
#define EVT_INDEX_OF_GAME_BYTE(v)   ((v) + EVT_GAME_BYTE_OFFSET)
#define EVT_INDEX_OF_ARRAY_FLAG(v)  ((v) + EVT_ARRAY_FLAG_OFFSET)
#define EVT_INDEX_OF_ARRAY_VAR(v)   ((v) + EVT_ARRAY_VAR_OFFSET)

#define EVT_CMD(opcode, argv...) \
    opcode, \
    (sizeof((Bytecode[]){argv})/sizeof(Bytecode)), \
    ##argv

/// Sets the current thread's group. Group value meanings are currently not known.
#define EVT_SET_GROUP(GROUP)                    EVT_CMD(EVT_OP_SET_GROUP, GROUP),

/// Signals the end of EVT script data. A script missing this will likely crash on load.
#define EVT_END                                 EVT_CMD(EVT_OP_END),

/// Kills the current EVT thread.
/// A script missing a return will live - but do nothing - forever, or until something else kills it (e.g. leaving the map).
#define EVT_RETURN                              EVT_CMD(EVT_OP_RETURN),

/// The given arguments can be accessed from the API function using `thread->ptrReadPos`.
#define EVT_CALL(FUNC, ARGS...)                     EVT_CMD(EVT_OP_CALL, (Bytecode) FUNC, ##ARGS),

typedef struct Evt {
    /* 0x000 */ u8 stateFlags;
    /* 0x001 */ u8 currentArgc;
    /* 0x002 */ u8 currentOpcode;
    /* 0x003 */ u8 priority;
    /* 0x004 */ u8 groupFlags;
    /* 0x005 */ s8 blocked; /* 1 = blocking */
    /* 0x006 */ s8 loopDepth; /* how many nested loops we are in, >= 8 hangs forever */
    /* 0x007 */ s8 switchDepth; /* how many nested switches we are in, max = 8 */
    /* 0x008 */ Bytecode* ptrNextLine;
    /* 0x00C */ Bytecode* ptrReadPos;
    /* 0x010 */ s8 labelIndices[16];
    /* 0x020 */ UNK_PTR labelPositions[16];
    /* 0x060 */ UNK_PTR userData; /* unknown pointer; allocated on the heap, free'd in kill_script() */
    /* 0x064 */ struct Evt* blockingParent; /* parent? */
    /* 0x068 */ struct Evt* childScript;
    /* 0x06C */ struct Evt* parentScript; /* brother? */
    /* 0x070 */ union {
    /*       */     s32 functionTemp[4];
    /*       */     f32 functionTempF[4];
    /*       */     void* functionTempPtr[4];
    /*       */ };
    /* 0x080 */ ApiFunc callFunction;
    /* 0x084 */ union {
    /*       */     s32 varTable[16];
    /*       */     f32 varTableF[16];
    /*       */     void* varTablePtr[16];
    /*       */ };
    /* 0x0C4 */ s32 varFlags[3];
    /* 0x0D0 */ s32 loopStartTable[8];
    /* 0x0F0 */ s32 loopCounterTable[8];
    /* 0x110 */ s8 switchBlockState[8];
    /* 0x118 */ s32 switchBlockValue[8];
    /* 0x138 */ s32* buffer;
    /* 0x13C */ s32* array;
    /* 0x140 */ s32* flagArray;
    /* 0x144 */ s32 id;
    /* 0x148 */ union {
        s32 enemyID;
        s32 actorID;
        struct Enemy* enemy; ///< For overworld scripts owned by an Npc
        struct Actor* actor; ///< For battle scripts
    } owner1;                ///< Initially -1
    /* 0x14C */ union {
        s32 npcID;
        s32 triggerID;
        struct Npc* npc;            ///< For overworld scripts owned by an Npc
        struct Trigger* trigger;
    } owner2;                       ///< Initially -1
    /* 0x150 */ f32 timeScale;
    /* 0x154 */ f32 frameCounter;
    /* 0x158 */ s32 unk_158;
    /* 0x15C */ Bytecode* ptrFirstLine;
    /* 0x160 */ Bytecode* ptrSavedPos;
    /* 0x164 */ Bytecode* ptrCurrentLine;
} Evt; // size = 0x168

/// Local Word. A variable local to the current thread.
/// LWs are copied to any threads created by this one (EVT_EXEC, EVT_EXEC_WAIT, EVT_THREAD, EVT_CHILD_THREAD).
/// Additionally, EVT_EXEC_WAIT copies LWs back from the spawned thread when it completes.
///
/// Range: `0 <= v < 0x10`
#define LocalVar(INDEX) ((INDEX) - EVT_LOCAL_VAR_OFFSET)

// shorthand names for LocalVar
#define LVar0 LocalVar(0)
#define LVar1 LocalVar(1)
#define LVar2 LocalVar(2)
#define LVar3 LocalVar(3)
#define LVar4 LocalVar(4)
#define LVar5 LocalVar(5)
#define LVar6 LocalVar(6)
#define LVar7 LocalVar(7)
#define LVar8 LocalVar(8)
#define LVar9 LocalVar(9)
#define LVarA LocalVar(10)
#define LVarB LocalVar(11)
#define LVarC LocalVar(12)
#define LVarD LocalVar(13)
#define LVarE LocalVar(14)
#define LVarF LocalVar(15)

#endif