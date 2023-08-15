#include "global.h"
#include "gflib.h"
#include "string_util.h"
#include "menu.h"
#include "text_window.h"
#include "strings.h"
#include "constants/coins.h"

EWRAM_DATA static u8 sCoinsWindowId = 0;

u16 GetCoins(void)
{
    return gSaveBlock1Ptr->coins ^ gSaveBlock2Ptr->encryptionKey;
}

void SetCoins(u16 coinAmount)
{
    gSaveBlock1Ptr->coins = coinAmount ^ gSaveBlock2Ptr->encryptionKey;
}

bool8 AddCoins(u16 toAdd)
{
    u16 coins = GetCoins();
    if (coins >= MAX_COINS)
        return FALSE;
    // check overflow, can't have less coins than previously
    if (coins <= coins + toAdd)
    {
        coins += toAdd;
        if (coins > MAX_COINS)
            coins = MAX_COINS;
    }
    else
        coins = MAX_COINS;
	
    SetCoins(coins);
    return TRUE;
}

bool8 RemoveCoins(u16 toSub)
{
    u16 coins = GetCoins();
    if (coins >= toSub)
    {
        SetCoins(coins - toSub);
        return TRUE;
    }
    return FALSE;
}

void PrintCoinsString(u32 coinAmount)
{
    ConvertIntToDecimalStringN(gStringVar1, coinAmount, STR_CONV_MODE_RIGHT_ALIGN, 4);
    StringExpandPlaceholders(gStringVar4, gText_Coins);
    AddTextPrinterParameterized(sCoinsWindowId, 0, gStringVar4, 64 - GetStringWidth(0, gStringVar4, 0), 0xC, 0, NULL);
}

void ShowCoinsWindow(u32 coinAmount, u8 x, u8 y)
{
    struct WindowTemplate template = SetWindowTemplateFields(0, x + 1, y + 1, 8, 3, 0xF, 0x20);

    sCoinsWindowId = AddWindow(&template);
    FillWindowPixelBuffer(sCoinsWindowId, 0);
    PutWindowTilemap(sCoinsWindowId);
    TextWindow_SetStdFrame0_WithPal(sCoinsWindowId, 0x21D, 0xD0);
    DrawStdFrameWithCustomTileAndPalette(sCoinsWindowId, FALSE, 0x21D, 0xD);
    AddTextPrinterParameterized(sCoinsWindowId, 2, gText_Coins_2, 0, 0, 0xFF, 0);
    PrintCoinsString(coinAmount);
}

void HideCoinsWindow(void)
{
    ClearWindowTilemap(sCoinsWindowId);
    ClearStdWindowAndFrameToTransparent(sCoinsWindowId, TRUE);
    RemoveWindow(sCoinsWindowId);
}
