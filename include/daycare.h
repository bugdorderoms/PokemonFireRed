#ifndef GUARD_DAYCARE_H
#define GUARD_DAYCARE_H

#include "global.h"

void StoreSelectedPokemonInDaycare(void);
u16 TakePokemonFromDaycare(void);
void GetDaycareCost(void);
u8 GetNumLevelsGainedFromDaycare(void);
void RejectEggFromDayCare(void);
void CreateEgg(struct Pokemon *mon, u16 species, bool8 setHotSpringsLocation);
void GiveEggFromDaycare(void);
void GetDaycareMonNicknames(void);
u8 GetDaycareState(void);
void SetDaycareCompatibilityString(void);
bool8 NameHasGenderSymbol(const u8 *name, u8 genderRatio);
void ShowDaycareLevelMenu(void);
void ChooseSendDaycareMon(void);
u8 GetEggMoves(struct Pokemon *pokemon, u16 *eggMoves);
void EggHatch(void);
bool8 ShouldEggHatch(void);

#endif // GUARD_DAYCARE_H
