#ifndef GUARD_POKEMON_SUMMARY_SCREEN_H
#define GUARD_POKEMON_SUMMARY_SCREEN_H

#include "main.h"

void ShowSelectMovePokemonSummaryScreen(struct Pokemon *, u8, u8, MainCallback, u16);
u8 GetMoveSlotToReplace(void);
u8 GetLastViewedMonIndex(void);
void ShowPokemonSummaryScreen(struct Pokemon * party, u8 cursorPos, u8 lastIdx, void (*callback)(void), u8 a4);
void SetPokemonSummaryScreenMode(u8);

// The Pokemon Summary Screen can operate in different modes. Certain features,
// such as move re-ordering, are available in the different modes.
enum PokemonSummaryScreenMode
{
    PSS_MODE_NORMAL,
    PSS_MODE_UNK1,
    PSS_MODE_SELECT_MOVE,
    PSS_MODE_FORGET_MOVE,
    PSS_MODE_TRADE,
    PSS_MODE_BOX,
};

enum PokemonSummaryScreenPage
{
    PSS_PAGE_INFO,
    PSS_PAGE_SKILLS,
    PSS_PAGE_MOVES,
    PSS_PAGE_MOVES_INFO,
    PSS_PAGE_UNK4,
    PSS_PAGE_MOVE_DELETER,
};

enum PokemonSummaryScreenState3270
{
    PSS_STATE3270_FADEIN,
    PSS_STATE3270_PLAYCRY,
    PSS_STATE3270_HANDLEINPUT,
    PSS_STATE3270_FLIPPAGES,
    PSS_STATE3270_ATEXIT_FADEOUT,
    PSS_STATE3270_ATEXIT_WAITLINKDELAY,
    PSS_STATE3270_ATEXIT_WAITFADE
};

#define TAG_PSS_UNK_6E 0x6E
#define TAG_PSS_UNK_78 0x78
#define TAG_PSS_UNK_82 0x82
#define TAG_PSS_UNK_8C 0x8C
#define TAG_PSS_UNK_96 0x96
#define TAG_PSS_UNK_A0 0xA0
#define TAG_PSS_UNK_A1 0xA1
#define TAG_PSS_UNK_A2 0xA2
#define TAG_PSS_UNK_A3 0xA3

#endif // GUARD_POKEMON_SUMMARY_SCREEN_H
