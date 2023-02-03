#ifndef GUARD_BATTLE_SCRIPT_COMMANDS_H
#define GUARD_BATTLE_SCRIPT_COMMANDS_H

#include "global.h"

#define STAT_CHANGE_WORKED      0
#define STAT_CHANGE_DIDNT_WORK  1

#define WINDOW_CLEAR            0x1
#define WINDOW_x80              0x80

void SetMoveEffect(bool8 primary, u8 certain);
bool8 UproarWakeUpCheck(u8 battlerId);
u8 GetTypeModifier(u8 atkType, u8 defType);
u16 AI_TypeCalc(u16 move, u16 targetSpecies, u16 targetAbility);
u8 TypeCalc(u16 move, u8 moveType, u8 attacker, u8 defender, bool8 recordAbilities, bool8 resetFlags, u16 *flags);
void AI_CalcDmg(u8 attacker, u8 defender, u16 move);
bool32 IsMonGettingExpSentOut(void);
void BattleCreateYesNoCursorAt(void);
void BattleDestroyYesNoCursorAt(void);
void HandleBattleWindow(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u8 flags);
u8 GetBattlerTurnOrderNum(u8 battlerId);
void BufferMoveToLearnIntoBattleTextBuff2(void);

//callasm functions declaration
void TryDoAnticipationShudder(void);
void TryBadDreamsSecondDamage(void);
void GetStatRaiseDownload(void);
void GetStrongestMoveForewarn(void);
void TryFriskFirstTarget(void);
void TryFriskSecondTarget(void);

extern const u16 gSheerForceBoostedMoves[];

extern void (* const gBattleScriptingCommandsTable[])(void);

#endif // GUARD_BATTLE_SCRIPT_COMMANDS_H
