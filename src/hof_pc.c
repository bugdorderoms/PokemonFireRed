#include "global.h"
#include "gflib.h"
#include "hall_of_fame.h"
#include "overworld.h"
#include "script.h"
#include "script_menu.h"
#include "task.h"

static void Task_WaitFadeAndSetCallback(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FreeAllWindowBuffers();
        ResetBgsAndClearDma3BusyFlags(0);
        DestroyTask(taskId);
        SetMainCallback2(CB2_InitHofPC);
    }
}

void HallOfFamePCBeginFade(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, RGB_BLACK);
    ScriptContext2_Enable();
    CreateTask(Task_WaitFadeAndSetCallback, 0);
}

static void Task_WaitForPaletteFade(u8 taskId)
{
	if (!gPaletteFade.active)
        DestroyTask(taskId);
}

static void ReshowPCMenuAfterHallOfFamePC(void)
{
    ScriptContext2_Enable();
    Overworld_PlaySpecialMapMusic();
    CreatePCMenu();
    ScriptMenu_DisplayPCStartupPrompt();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0x10, 0, RGB_BLACK);
    CreateTask(Task_WaitForPaletteFade, 10);
}

void ReturnFromHallOfFamePC(void)
{
	SetMainCallback2(CB2_ReturnToField);
    gFieldCallback = ReshowPCMenuAfterHallOfFamePC;
}
