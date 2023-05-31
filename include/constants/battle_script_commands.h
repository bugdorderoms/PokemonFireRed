#ifndef GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H
#define GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H

// Battle Scripting addresses
#define sPAINSPLIT_HP gBattleScripting + 0x0
#define sBIDE_DMG gBattleScripting + 0x4
#define sSAVED_DMG gBattleScripting + 0x8
#define sMULTIHIT_STRING gBattleScripting + 0x0C
#define sDMG_MULTIPLIER gBattleScripting + 0x12
#define sTWOTURN_STRINGID gBattleScripting + 0x13
#define sB_ANIM_ARG1 gBattleScripting + 0x14
#define sB_ANIM_ARG2 gBattleScripting + 0x15
#define sTRIPLE_KICK_POWER gBattleScripting + 0x16
#define sMOVEEND_STATE gBattleScripting + 0x17
#define sBATTLER gBattleScripting + 0x19
#define sB_ANIM_TURN gBattleScripting + 0x1A
#define sB_ANIM_TARGETS_HIT gBattleScripting + 0x1B
#define sSTATCHANGER gBattleScripting + 0x1C
#define sSTAT_ANIM_PLAYED gBattleScripting + 0x1D
#define sGIVEEXP_STATE gBattleScripting + 0x1E
#define sBATTLE_STYLE gBattleScripting + 0x1F
#define sLVLBOX_STATE gBattleScripting + 0x20
#define sLEARNMOVE_STATE gBattleScripting + 0x21
#define sILLUSION_NICK_HACK gBattleScripting + 0x25
#define sFIXED_ABILITY_POPUP gBattleScripting + 0x26
#define sBYPASS_ABILITY_POP_UP gBattleScripting + 0x27

// array entries for battle communication
#define MULTIUSE_STATE                      0x0
#define CURSOR_POSITION                     0x1
#define TASK_ID                             0x1 // task Id and cursor position share the same field
#define SPRITES_INIT_STATE1                 0x1 // shares the Id as well
#define SPRITES_INIT_STATE2                 0x2
#define ACTIONS_CONFIRMED_COUNT             0x3
#define MULTISTRING_CHOOSER                 0x4
#define MSG_DISPLAY                         0x5
#define MISS_TYPE                           0x6
#define BATTLE_COMMUNICATION_ENTRIES_COUNT  0x7

// BattleCommunication addresses
#define sMULTIUSE_STATE gBattleCommunication + MULTIUSE_STATE
#define sCURSOR_POSITION gBattleCommunication + CURSOR_POSITION
#define sMULTISTRING_CHOOSER gBattleCommunication + MULTISTRING_CHOOSER
#define sMISS_TYPE gBattleCommunication + MISS_TYPE

// Battle Script defines for getting the wanted battler
#define BS_TARGET                   0
#define BS_ATTACKER                 1
#define BS_EFFECT_BATTLER           2
#define BS_FAINTED                  3
#define BS_ATTACKER_WITH_PARTNER    4 // for atk98_updatestatusicon
#define BS_UNKNOWN_5                5 // for openpartyscreen
#define BS_UNKNOWN_6                6 // for openpartyscreen
#define BS_BATTLER_0                7
#define BS_ATTACKER_SIDE            8 // for atk1E_jumpifability
#define BS_NOT_ATTACKER_SIDE        9 // for atk1E_jumpifability
#define BS_SCRIPTING                10
#define BS_PLAYER1                  11
#define BS_OPPONENT1                12
#define BS_PLAYER2                  13 // for atk98_updatestatusicon
#define BS_OPPONENT2                14
#define BS_SCRIPTING_PARTNER        15
#define BS_ACTIVE                   16

// Atk01, accuracy calc
#define NO_ACC_CALC_CHECK_LOCK_ON 0
#define ACC_CURR_MOVE             1

// Atk08, Parental Bond counter states
#define PARENTAL_BOND_1ST_HIT 2
#define PARENTAL_BOND_2ND_HIT 1
#define PARENTAL_BOND_OFF     0

// Compare operands
#define CMP_EQUAL               0x0
#define CMP_NOT_EQUAL           0x1
#define CMP_GREATER_THAN        0x2
#define CMP_LESS_THAN           0x3
#define CMP_COMMON_BITS         0x4
#define CMP_NO_COMMON_BITS      0x5

// Atk4F, a flag used for the jumpifcantswitch command
#define ATK4F_DONT_CHECK_STATUSES   0x80

// Atk50, a flag used for the openpartyscreen command
#define OPEN_PARTY_ALLOW_CANCEL     0x80

// Statchange defines
#define STAT_CHANGE_BS_PTR                  0x1
#define STAT_CHANGE_NOT_PROTECT_AFFECTED    0x20

// Atk48
#define ATK48_STAT_NEGATIVE         0x1
#define ATK48_STAT_BY_TWO           0x2
#define ATK48_ONLY_MULTIPLE         0x4
#define ATK48_DONT_CHECK_LOWER      0x8

// Atk49, moveend states and cases
#define ATK49_MOVEEND_ALL  0 // loop through all cases
#define ATK49_MOVEEND_CASE 1 // do only the specified case
#define ATK49_MOVEEND_TO   2 // loop from first to specified cases

#define ATK49_SUM_DMG                           0
#define ATK49_RAGE                              1
#define ATK49_SYNCHRONIZE_TARGET                2
#define ATK49_MOVE_END_ABILITIES_ATTACKER       3
#define ATK49_MOVE_END_ABILITIES                4
#define ATK49_STATUS_IMMUNITY_ABILITIES         5
#define ATK49_SYNCHRONIZE_ATTACKER              6
#define ATK49_CHOICE_MOVE                       7
#define ATK49_ATTACKER_INVISIBLE                8
#define ATK49_ATTACKER_VISIBLE                  9
#define ATK49_TARGET_VISIBLE                    10
#define ATK49_ITEM_EFFECTS_ALL                  11
#define ATK49_KINGSROCK_SHELLBELL               12 // These item effects will occur each strike of a multi-hit move
#define ATK49_SUBSTITUTE                        13
#define ATK49_UPDATE_LAST_MOVES                 14
#define ATK49_MIRROR_MOVE                       15
#define ATK49_NEXT_TARGET                       16 // Everything up until here is handled for each strike of a multi-hit move
#define ATK49_MULTIHIT_MOVE                     17
#define ATK49_DEFROST                           18
#define ATK49_RECOIL                            19
#define ATK49_MAGICIAN                          20 // Occurs after final multi-hit strike, and after other items/abilities would activate
#define ATK49_CHANGED_ITEMS                     21
#define ATK49_PICKPOCKET                        22
#define ATK49_EMERGENCY_EXIT                    23
#define ATK49_CLEAR_BITS                        24
#define ATK49_COUNT                             25

// TODO: documentation
// Atk76, various cases
#define VARIOUS_CANCEL_MULTI_TURN_MOVES             0
#define VARIOUS_SET_MAGIC_COAT_TARGET               1
#define VARIOUS_IS_RUNNING_IMPOSSIBLE               2
#define VARIOUS_GET_MOVE_TARGET                     3
#define VARIOUS_GET_BATTLER_FAINTED                 4
#define VARIOUS_RESET_INTIMIDATE_TRACE_BITS         5
#define VARIOUS_UPDATE_CHOICE_MOVE_ON_LVL_UP        6
#define VARIOUS_RESET_PLAYER_FAINTED_FLAG           7
#define VARIOUS_GET_BATTLERS_FOR_RECALL             8
#define VARIOUS_RETURN_OPPONENT_MON1                9
#define VARIOUS_RETURN_OPPONENT_MON2                10
#define VARIOUS_CHECK_POKEFLUTE                     11
#define VARIOUS_WAIT_FANFARE                        12
#define VARIOUS_TRY_ACTIVATE_MOXIE                  13
#define VARIOUS_JUMP_IF_TARGET_ALLY                 14
#define VARIOUS_TRAINER_SLIDE_FIRST_MON_DOWN        15
#define VARIOUS_TRAINER_SLIDE_LAST_MON              16
#define VARIOUS_UPDATE_NICK                         17
#define VARIOUS_SET_SPRITEIGNORE0HP                 18
#define VARIOUS_TRY_REMOVE_ILLUSION                 19
#define VARIOUS_PLAY_TRANSFORM_ANIMATION            20
#define VARIOUS_JUMP_IF_BATTLE_END                  21
#define VARIOUS_TRY_REMOVE_PRIMAL_WEATHER           22
#define VARIOUS_JUMP_IF_NOT_ALIVE                   23
#define VARIOUS_TRY_FRISK_TARGET                    24
#define VARIOUS_SWITCHIN_ABILITIES                  25
#define VARIOUS_TRY_END_NEUTRALIZING_GAS            26
#define VARIOUS_SET_TELEPORT_OUTCOME                27
#define VARIOUS_TRY_FAINT_ON_SPIKES_DAMAGE          28
#define VARIOUS_ATKNAME_IN_BUFF1                    29
#define VARIOUS_RESET_SENT_MONS_VALUE               30
#define VARIOUS_SET_ATK_TO_PLAYER0                  31
#define VARIOUS_BUFFER_MOVE_TO_LEARN                32
#define VARIOUS_ACTIVATE_WEATHER_ABILITIES          33
#define VARIOUS_TRY_ACTIVATE_GRIM_NEIGH             34
#define VARIOUS_TRY_ACTIVATE_SOUL_HEART             35
#define VARIOUS_TRY_ACTIVATE_RECEIVER               36
#define VARIOUS_TRY_ACTIVATE_BEAST_BOOST            37
#define VARIOUS_TRY_NEUTRALIZING_GAS_SUPPRESSION    38

// Atk80, dmg manipulation
#define ATK80_DMG_CHANGE_SIGN                               0
#define ATK80_DMG_HALF_BY_TWO_NOT_MORE_THAN_HALF_MAX_HP     1
#define ATK80_DMG_DOUBLED                                   2 // unused case
#define ATK80_DMG_1_8_MAX_HP                                3
#define ATK48_NEGATIVE_DMG_DEALT                            4
#define ATK48_DMG_HALF_TARGET_HP                            5
#define ATK48_DMG_ATK_LEVEL                                 6

// Atk83, trainerslide cases
#define ATK83_TRAINER_SLIDE_CASE_SAVE_SPRITES    0
#define ATK83_TRAINER_SLIDE_CASE_SLIDE_IN        1
#define ATK83_TRAINER_SLIDE_CASE_PRINT_STRING    2
#define ATK83_TRAINER_SLIDE_CASE_SLIDE_OUT       3
#define ATK83_TRAINER_SLIDE_CASE_RESTORE_SPRITES 4

// atkFC, a flag used for the handleabilitypopup command
#define ATKFC_REMOVE_POP_UP 0x80

#define BIT_HP                      0x1
#define BIT_ATK                     0x2
#define BIT_DEF                     0x4
#define BIT_SPEED                   0x8
#define BIT_SPATK                   0x10
#define BIT_SPDEF                   0x20
#define BIT_ACC                     0x40
#define BIT_EVASION                 0x80

#endif // GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H
