#include "global.h"
#include "task.h"
#include "main.h"
#include "dns.h"
#include "fieldmap.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex_screen.h"
#include "pokemon_icon.h"
#include "dexnav.h"
#include "battle_main.h"
#include "battle_interface.h"
#include "registered_item.h"
#include "item_menu_icons.h"
#include "constants/map_types.h"

#define DNS_PAL_EXCEPTION FALSE
#define DNS_PAL_ACTIVE    TRUE

/*******************************************************/
/*********    Day and Night Configuration     **********/
/********************************************************
 * You can customize the DNS by editing the following   *
 * timelapses and the filters used to change the        *
 * palette colours.                                     *
 * In addition to that, you can also configure which    *
 * palettes are affected by the system, as well as      *
 * establishing sprite palettes exceptions by its TAG.  *
 * 
 * It is highly recommended to read the following config*
 * options, to understand how the dns works.            *
 * ******************************************************/

/* Timelapses */
enum
{
    TIME_MIDNIGHT,
    TIME_DAWN,
    TIME_DAY,
    TIME_SUNSET,
    TIME_NIGHTFALL,
    TIME_NIGHT
};

/* This array contains the colours used for the windows or other    *
 * tiles that have to be illuminated at night.                      *
 * You can add or remove light slots as you whish, each entry       *
 * requires the paletteNum and the colourNum of each colour slot,   *
 * as well as the RGB 15 bit colour that's gonna be used as         *
 * "light colour".                                                  */
static const struct lightingColour sLightingColours[] =
{
    {
        .paletteNum = 0,
        .colourNum = 1,
        .lightColour = RGB2(30, 30, 5),
    },
    {
        .paletteNum = 0,
        .colourNum = 2,
        .lightColour = RGB2(26, 25, 4),
    },
    {
        .paletteNum = 0,
        .colourNum = 3,
        .lightColour = RGB2(22, 21, 3),
    },
    {
        .paletteNum = 1,
        .colourNum = 1,
        .lightColour = RGB2(30, 30, 5),
    },
    {
        .paletteNum = 1,
        .colourNum = 2,
        .lightColour = RGB2(26, 25, 4),
    },
    {
        .paletteNum = 6,
        .colourNum = 1,
        .lightColour = RGB2(30, 30, 5),
    },
    {
        .paletteNum = 6,
        .colourNum = 2,
        .lightColour = RGB2(26, 25, 4),
    },
    {
        .paletteNum = 6,
        .colourNum = 3,
        .lightColour = RGB2(22, 21, 3),
    },
};

/* Maptypes that are not affected by DNS */
static const u8 sDNSMapExceptions[] =
{
	MAP_TYPE_NONE,
	MAP_TYPE_INDOOR,
	MAP_TYPE_UNDERGROUND,
	MAP_TYPE_SECRET_BASE,
};

/* Configure each palette slot to be affected or not by DNS *
 * while you are in the overworld.                          *
 * 0-15 background, 16-31 sprites.                          */
static const struct DNSPalExceptions sOWPalExceptions = 
{
    .pal = {
        DNS_PAL_ACTIVE,    //0
        DNS_PAL_ACTIVE,    //1
        DNS_PAL_ACTIVE,    //2
        DNS_PAL_ACTIVE,    //3
        DNS_PAL_ACTIVE,    //4
        DNS_PAL_ACTIVE,    //5
        DNS_PAL_ACTIVE,    //6
        DNS_PAL_ACTIVE,    //7
        DNS_PAL_ACTIVE,    //8
        DNS_PAL_ACTIVE,    //9
        DNS_PAL_ACTIVE,    //10
        DNS_PAL_ACTIVE,    //11
        DNS_PAL_ACTIVE,    //12
        DNS_PAL_EXCEPTION, //13
        DNS_PAL_EXCEPTION, //14
        DNS_PAL_EXCEPTION, //15
        DNS_PAL_ACTIVE,    //16
        DNS_PAL_ACTIVE,    //17
        DNS_PAL_ACTIVE,    //18
        DNS_PAL_ACTIVE,    //19
        DNS_PAL_ACTIVE,    //20
        DNS_PAL_ACTIVE,    //21
        DNS_PAL_ACTIVE,    //22
        DNS_PAL_ACTIVE,    //23
        DNS_PAL_ACTIVE,    //24
        DNS_PAL_ACTIVE,    //25
        DNS_PAL_ACTIVE,    //26
        DNS_PAL_ACTIVE,    //27
        DNS_PAL_ACTIVE,    //28
        DNS_PAL_ACTIVE,    //29
        DNS_PAL_ACTIVE,    //30
        DNS_PAL_ACTIVE,    //31
    }
};

/* Configure each palette slot to be affected or not by DNS *
 * while in combat.                                         *
 * 0-15 background, 16-31 sprites.                          */
static const struct DNSPalExceptions sCombatPalExceptions =  
{
    .pal = {
        DNS_PAL_EXCEPTION,  //0
        DNS_PAL_EXCEPTION,  //1
        DNS_PAL_ACTIVE,     //2
        DNS_PAL_ACTIVE,     //3
        DNS_PAL_ACTIVE,     //4
        DNS_PAL_EXCEPTION,  //5
        DNS_PAL_ACTIVE,     //6
        DNS_PAL_ACTIVE,     //7
        DNS_PAL_EXCEPTION,  //8
        DNS_PAL_EXCEPTION,  //9
        DNS_PAL_ACTIVE,     //10
        DNS_PAL_ACTIVE,     //11
        DNS_PAL_ACTIVE,     //12
        DNS_PAL_ACTIVE,     //13
        DNS_PAL_ACTIVE,     //14
        DNS_PAL_ACTIVE,     //15
        DNS_PAL_EXCEPTION,  //16
        DNS_PAL_EXCEPTION,  //17
        DNS_PAL_EXCEPTION,  //18
        DNS_PAL_EXCEPTION,  //19
        DNS_PAL_EXCEPTION,  //20
        DNS_PAL_EXCEPTION,  //21
        DNS_PAL_EXCEPTION,  //22
        DNS_PAL_EXCEPTION,  //23
        DNS_PAL_EXCEPTION,  //24
        DNS_PAL_EXCEPTION,  //25
        DNS_PAL_EXCEPTION,  //26
        DNS_PAL_EXCEPTION,  //27
        DNS_PAL_EXCEPTION,  //28
        DNS_PAL_EXCEPTION,  //29
        DNS_PAL_EXCEPTION,  //30
        DNS_PAL_EXCEPTION,  //31
    }
};

/*******************************************************/
/*************    DNS Colour Filters     ***************/
/*******************************************************/
/* DNS filters are actual 15bit RGB colours.            *
 * This colours R - G - B channels are substracted from *
 * the level of each channel porportionally.            *
 *                                                      *
 *  [BUFFER] -> (Value - Filter) -> [PAL_RAM]           *
 *                                                      *
 * This means that you shouln't use too high values for *
 * RGB channels in the filters. Otherwie, the channels  *
 * will easily reach 0, giving you plain colours.       *
 * I Suggest to not use channels with a value above 16. *
 *                                                      *
 * Feel free to experiment with your own filters.       *
 * ******************************************************

/* Filters used at midnight.                    *
 * From 00:00 to 00:59 filters are cycled every *
 * 8 minutes.                                   *
 * From 01:00 to 03:59 the last filter is used. */
static const u16 sMidnightFilters[] =
{
    RGB2(14, 14, 6),    //CE19
    RGB2(14, 14, 7),    //CE1D
    RGB2(14, 14, 8),    //CE21
    RGB2(15, 15, 8),    //EF21
    RGB2(15, 15, 9),    //EF25
    RGB2(15, 15, 9),    //EF25
    RGB2(16, 16, 9),    //1026
    RGB2(16, 16, 10),   //102A
};

/* Filters used at dawn. (30 filters).          *
 * From 04:00 to 05:59 filters are cycled every *
 * 2 minutes.                                   */
static const u16 sDawnFilters[] =
{
    RGB2(15, 15, 10),
    RGB2(15, 15, 10),   //1
    RGB2(14, 14, 10),   //2
    RGB2(13, 13, 10),   //3
    RGB2(12, 12, 10),   //4
    RGB2(11, 11, 10),   //5
    RGB2(10, 10, 10),   //6
    RGB2(9, 9, 10),     //7
    RGB2(8, 8, 10),     //8
    RGB2(8, 8, 11),     //9
    RGB2(7, 7, 11),     //10
    RGB2(6, 6, 11),     //11
    RGB2(5, 5, 11),     //12
    RGB2(4, 4, 11),     //13
    RGB2(3, 3, 11),     //14
    RGB2(2, 2, 11),     //15
    RGB2(1, 1, 11),     //16
    RGB2(0, 0, 11),     //17
    RGB2(0, 0, 10),     //18
    RGB2(0, 0, 9),      //19
    RGB2(0, 0, 8),      //20
    RGB2(0, 0, 7),      //21
    RGB2(0, 0, 6),      //22
    RGB2(0, 0, 5),      //23
    RGB2(0, 0, 4),      //24
    RGB2(0, 0, 3),      //0003
    RGB2(0, 0, 2),      //0002
    RGB2(0, 0, 1),      //0001
    RGB2(0, 0, 0),      //0000
    RGB2(0, 0, 0),      //0000
};

/* Filters used at day. (no filter actually lul)*/
static const u16 sDayFilter = RGB2(0, 0, 0);   //0000

/* Filters used at sunset. (30 filters).        *
 * From 17:00 to 18:59 filters are cycled every *
 * 2 minutes.                                   */
static const u16 sSunsetFilters[] = 
{
    RGB2(0, 0, 1),      //0004
    RGB2(0, 1, 1),      //2004
    RGB2(0, 1, 2),      //2008
    RGB2(0, 1, 3),      //200C
    RGB2(0, 2, 3),      //400C
    RGB2(0, 2, 4),      //4010
    RGB2(0, 2, 5),      //4014
    RGB2(0, 3, 5),      //6014
    RGB2(0, 3, 6),      //6018
    RGB2(0, 3, 7),      //601C
    RGB2(0, 4, 7),      //801C
    RGB2(0, 4, 8),      //8020
    RGB2(0, 4, 9),      //8024
    RGB2(0, 5, 9),      //A024
    RGB2(0, 5, 10),     //A028
    RGB2(0, 5, 11),     //A02C
    RGB2(0, 6, 11),     //C02C
    RGB2(0, 6, 12),     //C030
    RGB2(0, 6, 13),     //C034
    RGB2(0, 7, 13),     //E034
    RGB2(0, 7, 14),     //E038
    RGB2(0, 7, 14),     //E038
    RGB2(0, 8, 14),     //0039
    RGB2(0, 9, 14),     //2039
    RGB2(0, 10, 14),    //4039
    RGB2(0, 11, 14),    //6039
    RGB2(0, 12, 14),    //8039
    RGB2(0, 13, 14),    //A039
    RGB2(0, 14, 14),    //C039
    RGB2(0, 14, 14),    //C039
};

/* Filters used at nightfall. (30 filters).     *
 * From 19:00 to 21:59 filters are cycled every *
 * 2 minutes.                                   */
static const u16 sNightfallFilters[] = 
{
    RGB2(0, 14, 14),    //39C0
    RGB2(0, 14, 14),    //39C0
    RGB2(0, 14, 13),    //35C0
    RGB2(0, 14, 12),    //31C0
    RGB2(0, 14, 11),    //2DC0
    RGB2(0, 14, 10),    //29C0
    RGB2(1, 14, 10),    //29C1
    RGB2(1, 14, 9),     //25C1
    RGB2(0, 14, 8),     //21C0
    RGB2(1, 14, 7),     //1DC1
    RGB2(1, 14, 6),     //19C1
    RGB2(2, 14, 6),     //19C2
    RGB2(2, 14, 5),     //15C2
    RGB2(2, 14, 4),     //11C2
    RGB2(2, 14, 3),     //0DC2
    RGB2(2, 14, 2),     //09C2
    RGB2(2, 14, 2),     //09C2
    RGB2(3, 14, 3),     //0DC3
    RGB2(4, 14, 4),     //11C4
    RGB2(5, 14, 5),     //15C5
    RGB2(6, 14, 6),     //19C6
    RGB2(7, 14, 6),     //19C7
    RGB2(8, 14, 6),     //19C8
    RGB2(9, 14, 6),     //19C9
    RGB2(10, 14, 6),    //19CA
    RGB2(11, 14, 6),    //19CB
    RGB2(12, 14, 6),    //19CC
    RGB2(13, 14, 6),    //19CD
    RGB2(14, 14, 6),    //19CE
    RGB2(14, 14, 6),    //19CE
};

/* Filter used at night. From 22:00 to 24:59 */
static const u16 sNightFilter = RGB2(14, 14, 6);   //19CE

/*************   SpritePalette DNS exceptions by TAG   **************
 * If you are using any dynamic sprite palette allocation system,   *
 * you will most likely want to use this system to avoid certain    *
 * palette tags to be "banned" from dns, as the palettes may get    *
 * loaded in different slots each time.                             */
static const u16 sPaletteTagExceptions[] =
{
	OWNED_ICON_TAG, // 0x0066
	ITEMICON_TAG, // 0xD750
	CURSOR_TAG, // 0x1075
	BOX_TAG, // 0x1078
	ITEMICON_INITIAL_TAG + 0, // 0x1088
	ITEMICON_INITIAL_TAG + 1, // 0x1089
	ITEMICON_INITIAL_TAG + 2, // 0x108A
	ITEMICON_INITIAL_TAG + 3, // 0x108B
	POKE_ICON_BASE_PAL_TAG + 0, // 0xDAC0
	POKE_ICON_BASE_PAL_TAG + 1, // 0xDAC1
	POKE_ICON_BASE_PAL_TAG + 2, // 0xDAC2
	// Don't need define battle palette tag since none battle sprite was affected by dns.
};

// The season for each month of the year
static const u8 sSeasonsByMonth[] =
{
	SEASON_SUMMER, // January
	SEASON_SUMMER, // February
	SEASON_SUMMER, // March
	SEASON_AUTUMN, // April
	SEASON_AUTUMN, // May
	SEASON_AUTUMN, // June
	SEASON_WINTER, // July
	SEASON_WINTER, // August
	SEASON_WINTER, // September
	SEASON_SPRING, // October
	SEASON_SPRING, // November
	SEASON_SPRING, // December
};

/***********************************************
 * --------- DNS CONFIGURATION END ----------- *
 * ******************************************* */

// Functions
static bool8 IsMapDNSException(void);
static bool8 IsSpritePaletteTagDNSException(u8 palNum);
static u8 GetDNSTimeLapse(u8 hour);
static u16 GetDNSFilter(void);
static u16 DNSApplyProportionalFilterToColour(u16 colour, u16 filter);
static void DoDNSLightningWindowsEffect(void);

// DNS palette buffer in EWRAM
ALIGNED(4) EWRAM_DATA static u16 sDNSPaletteDmaBuffer[PLTT_BUFFER_SIZE] = {0};

#define IN_OVERWORLD ((gMain.callback2 == CB2_Overworld || gMain.callback2 == CB2_OverworldBasic))
#define IN_BATTLE ((gMain.callback2 == BattleMainCB2 && gMain.vblankCallback != VBlankCB && !(gBattleTypeFlags & BATTLE_TYPE_POKEDUDE)))
#define LIT_UP_TIME ((gRtcLocation.hour < MORNING_OF_DAY_START || gRtcLocation.hour >= NIGHT_OF_DAY_START))

/* **************************************************** *
 * **************** D&N for pokefirered *************** *
 * **************************************************** *
 * Based on Prime Dialga DNS for Pokemon GBA Games.     *
 * Additional credits to Andrea & Eing                  *
 * Author: Xhyz/Samu                                    *
 ****************************************************** */

void DNSTransferPlttBuffer(void *src, void *dest)
{
#if USE_DNS_IN_BATTLE
	if ((IN_OVERWORLD || IN_BATTLE) && !IsMapDNSException())
#else
	if (IN_OVERWORLD && !IsMapDNSException())
#endif
	{
#if LIT_UP_WINDOWS
		if (!gMain.inBattle)
			DoDNSLightningWindowsEffect();
#endif
		DmaCopy16(3, sDNSPaletteDmaBuffer, dest, PLTT_SIZE);
	}
	else
	{
		DmaCopy16(3, src, dest, PLTT_SIZE);
	}
}

static bool8 IsMapDNSException(void)
{
	u8 i, mapType = gMapHeader.mapType;
	
	for (i = 0; i < NELEMS(sDNSMapExceptions); i++)
	{
		if (sDNSMapExceptions[i] == mapType)
			return TRUE;
	}
	return FALSE;
}

static bool8 IsSpritePaletteTagDNSException(u8 palNum)
{
	u8 i;
	u16 tag = GetSpritePaletteTagByPaletteNum(palNum);

	for (i = 0; i < NELEMS(sPaletteTagExceptions); i++)
	{
		if (sPaletteTagExceptions[i] == tag)
			return TRUE;
	}
	return FALSE;
}

void DNSApplyFilters(void)
{
	u8 palNum, colNum;
	u16 colourSlot, rgbFilter = GetDNSFilter();
	struct DNSPalExceptions palExceptionFlags;
#if USE_DNS_IN_BATTLE
	palExceptionFlags = gMain.inBattle ? sCombatPalExceptions : sOWPalExceptions;
#else
	palExceptionFlags = sOWPalExceptions;
#endif
	
	for (palNum = 0; palNum < 32; palNum++)
	{
		if (palExceptionFlags.pal[palNum] && (palNum < 16 || !IsSpritePaletteTagDNSException(palNum - 16)))
		{
			for (colNum = 0; colNum < 16; colNum++)
			{
				colourSlot = palNum * 16 + colNum;
				sDNSPaletteDmaBuffer[colourSlot] = DNSApplyProportionalFilterToColour(gPlttBufferFaded[colourSlot], rgbFilter);
			}
		}
		else
		{
			for (colNum = 0; colNum < 16; colNum++)
			{
				colourSlot = palNum * 16 + colNum;
				sDNSPaletteDmaBuffer[colourSlot] = gPlttBufferFaded[colourSlot];
			}
		}
	}
}

static void DoDNSLightningWindowsEffect(void)
{
	u8 i;
	u16 colourSlot;
	bool8 fadeActive = gPaletteFade.active;
	
	if (LIT_UP_TIME)
	{
		for (i = 0; i < NELEMS(sLightingColours); i++)
		{
			colourSlot = sLightingColours[i].paletteNum * 16 + sLightingColours[i].colourNum;
			
			if (fadeActive || (gPlttBufferUnfaded[colourSlot] != 0x0000 && gPlttBufferFaded[colourSlot] == 0x0000))
			{
				sDNSPaletteDmaBuffer[colourSlot] = gPlttBufferFaded[colourSlot];
				gPlttBufferUnfaded[colourSlot] = sLightingColours[i].lightColour;
			}
			else
				sDNSPaletteDmaBuffer[colourSlot] = sLightingColours[i].lightColour;
		}
		gMain.tilesetPaletteReloaded = FALSE;
	}
	else if (!fadeActive && !gMain.tilesetPaletteReloaded)
	{
		LoadMapTilesetPalettes(gMapHeader.mapLayout);
		gMain.tilesetPaletteReloaded = TRUE;
	}
}

static u16 GetDNSFilter(void)
{
	u8 hour = gRtcLocation.hour, minutes = gRtcLocation.minute;
	
	switch (GetDNSTimeLapse(hour))
	{
		case TIME_MIDNIGHT:
			return sMidnightFilters[hour < 1 ? minutes >> 3 : 7];
		case TIME_DAWN:
		    return sDawnFilters[minutes >> 1];
		case TIME_DAY:
		    return sDayFilter;
		case TIME_SUNSET: 
		    return sSunsetFilters[minutes >> 1];
		case TIME_NIGHTFALL:
		    return sNightfallFilters[minutes >> 1];
		case TIME_NIGHT:
	        return sNightFilter;
	}
}

static u16 DNSApplyProportionalFilterToColour(u16 colour, u16 filter)
{
	u32 red, green, blue;
    
	red = (colour & 0x1F) * (0x1F - (filter & 0x1F)) >> 5;
	green = ((colour & 0x3E0) >> 5) * ((0x3E0 - (filter & 0x3E0)) >> 5) >> 5;
	blue = ((colour & 0x7C00) >> 10) * ((0x7C00 - (filter & 0x7C00)) >> 10) >> 5;
		
	return RGB2(red <= 31 ? red : 0, green <= 31 ? green : 0, blue <= 31 ? blue : 0);
}

static u8 GetDNSTimeLapse(u8 hour)
{
	if (hour < DAWN_OF_DAY_START)
        return TIME_MIDNIGHT;
    else if (hour < MORNING_OF_DAY_START)
        return TIME_DAWN;
    else if (hour < AFTERNOON_OF_DAY_START)
        return TIME_DAY;
    else if (hour < NIGHT_OF_DAY_START)
        return TIME_SUNSET;
    else if (hour < MIDNIGHT_OF_DAY_START)
        return TIME_NIGHTFALL;
    else 
        return TIME_NIGHT;
}

bool8 GetDNSTimeLapseIsNight(void)
{
	switch (GetDNSTimeLapse(gRtcLocation.hour))
	{
		case TIME_MIDNIGHT:
		case TIME_NIGHTFALL:
		case TIME_NIGHT:
		    return TRUE;
		default:
			return FALSE;
	}
}

u8 DNSGetCurrentSeason(void)
{
	return sSeasonsByMonth[gRtcLocation.month];
}
