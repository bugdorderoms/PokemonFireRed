#ifndef GUARD_SAFARI_ZONE_H
#define GUARD_SAFARI_ZONE_H

#include "global.h"

#define MAX_SAFARI_BALLS 30
#define MAX_SAFARI_STEPS 600

extern u8 gNumSafariBalls;
extern u16 gSafariZoneStepCounter;

bool32 GetSafariZoneFlag(void);
void ResetSafariZoneFlag(void);

void EnterSafariMode(void);
void ExitSafariMode(void);

bool8 SafariZoneTakeStep(void);
void SafariZoneRetirePrompt(void);

void CB2_EndSafariBattle(void);

#endif // GUARD_SAFARI_ZONE_H
