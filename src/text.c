#include "global.h"
#include "gflib.h"
#include "event_data.h"
#include "strings.h"
#include "m4a.h"
#include "graphics.h"
#include "dynamic_placeholder_text_util.h"
#include "constants/songs.h"

extern const struct OamData gOamData_AffineOff_ObjNormal_16x16;

static void DecompressGlyphFont3(u16 glyphId, bool32 isJapanese);
static void DecompressGlyphFont4(u16 glyphId, bool32 isJapanese);

TextFlags gTextFlags;

static const u8 sDownArrowTiles[]         = INCBIN_U8("graphics/fonts/down_arrow.4bpp");
static const u8 sDarkDownArrowTiles[]     = INCBIN_U8("graphics/fonts/down_arrow_RS.4bpp");
static const u8 sTinyArrowTiles[]         = INCBIN_U8("graphics/fonts/down_arrow_2.4bpp");
static const u8 sTinyDarkDownArrowTiles[] = INCBIN_U8("graphics/fonts/down_arrow_RS_2.4bpp");
static const u8 sDoubleArrowTiles1[]       = INCBIN_U8("graphics/fonts/down_arrow_3.4bpp");
static const u8 sDoubleArrowTiles2[]       = INCBIN_U8("graphics/fonts/down_arrow_4.4bpp");

static const u8 sDownArrowYCoords[]           = { 0x0, 0x10, 0x20, 0x10 };
static const u8 sWindowVerticalScrollSpeeds[] = { 0x1, 0x2 , 0x4 , 0x0 };

static const struct GlyphWidthFunc sGlyphWidthFuncs[] = {
    { 0x0, GetGlyphWidthFont0 },
    { 0x1, GetGlyphWidthFont1 },
    { 0x2, GetGlyphWidthFont2 },
    { 0x3, GetGlyphWidthFont3 },
    { 0x4, GetGlyphWidthFont4 },
    { 0x5, GetGlyphWidthFont5 },
    { 0x6, GetGlyphWidthFont6 }
};

static const struct SpriteSheet sUnknown_81EA68C[] =
{
    {sDoubleArrowTiles1, sizeof(sDoubleArrowTiles1), 0x8000},
    {sDoubleArrowTiles2, sizeof(sDoubleArrowTiles2), 0x8000},
    {NULL}
};

static const struct SpritePalette sUnknown_81EA6A4[] =
{
    {gTMCaseMainWindowPalette, 0x8000},
    {NULL}
};

static const struct SpriteTemplate sUnknown_81EA6B4 =
{
    .tileTag = 0x8000,
    .paletteTag = 0x8000,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80062B0,
};

static const struct KeypadIcon sKeypadIcons[] = {
    {  0x0,  0x8, 0xC },
    {  0x1,  0x8, 0xC },
    {  0x2, 0x10, 0xC },
    {  0x4, 0x10, 0xC },
    {  0x6, 0x18, 0xC },
    {  0x9, 0x18, 0xC },
    {  0xC,  0x8, 0xC },
    {  0xD,  0x8, 0xC },
    {  0xE,  0x8, 0xC },
    {  0xF,  0x8, 0xC },
    { 0x20,  0x8, 0xC },
    { 0x21,  0x8, 0xC },
    { 0x22,  0x8, 0xC },
};

const u8 gKeypadIconTiles[] = INCBIN_U8("graphics/fonts/keypad_icons.4bpp");

// Font 0
static const u16 sFont0LatinGlyphs[] = INCBIN_U16("graphics/fonts/font0_latin.latfont");
static const u8 sFont0LatinGlyphWidths[] = 
{
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04, 0x05, 0x04, 0x04, 0x05, 
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x04, 0x05, 0x04, 0x04, 0x05, 0x05, 0x05, 0x06, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x08, 0x07, 0x08, 0x05, 0x05, 0x05, 0x05, 0x05, 0x08, 0x08, 0x07, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x07, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x08, 0x08, 0x08, 0x08, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x07, 0x07, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x08, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x08, 0x05, 0x08, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04, 0x05, 0x05, 0x05,
    0x05, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x08, 0x07, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x05
};
static const u16 sFont0JapaneseGlyphs[] = INCBIN_U16("graphics/fonts/font0_jap.fwjpnfont");

// Font 1
static const u16 sFont1LatinGlyphs[] = INCBIN_U16("graphics/fonts/font1_latin.latfont");
static const u8 sFont1LatinGlyphWidths[] =
{
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x09, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0A, 0x08, 0x05, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x06, 0x08, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x0C, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x05, 0x06, 0x06, 0x06, 0x03, 0x03, 0x06,
    0x06, 0x08, 0x05, 0x09, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x04, 0x06, 0x05,
    0x05, 0x06, 0x05, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x08, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x0C, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x06
};
static const u16 sFont1JapaneseGlyphs[] = INCBIN_U16("graphics/fonts/font1_jap.fwjpnfont");

// Font 2
static const u16 sFont2LatinGlyphs[] = INCBIN_U16("graphics/fonts/font2_latin.latfont");
static const u8 sFont2LatinGlyphWidths[] =
{
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x09, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0A, 0x08, 0x05, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x06, 0x08, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x0C, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x05, 0x06, 0x06, 0x06, 0x03, 0x03, 0x06,
    0x06, 0x08, 0x05, 0x09, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x04, 0x06, 0x05,
    0x05, 0x06, 0x05, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x08, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x0C, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x06
};
static const u16 sFont2JapaneseGlyphs[] = INCBIN_U16("graphics/fonts/font2_jap.fwjpnfont");
static const u8 sFont2JapaneseGlyphWidths[] =
{
    0x00, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x08, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x05, 0x09, 0x0A, 0x0A, 0x0A, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x08,
    0x08, 0x08, 0x0A, 0x0A, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x02, 0x04, 0x06,
    0x03, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x00
};

// Font 4
static const u16 sFont4LatinGlyphs[] = INCBIN_U16("graphics/fonts/font4_latin.latfont");
static const u8 sFont4LatinGlyphWidths[] =
{
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x09, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0A, 0x08, 0x05, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x06, 0x08, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x0C, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x05, 0x06, 0x06, 0x06, 0x03, 0x03, 0x06,
    0x06, 0x08, 0x05, 0x09, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x04, 0x06, 0x05,
    0x05, 0x06, 0x05, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x08, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x0C, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x06
};
static const u16 sFont4JapaneseGlyphs[] = INCBIN_U16("graphics/fonts/font4_jap.fwjpnfont");
static const u8 sFont4JapaneseGlyphWidths[] = 
{
    0x00, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x08, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x05, 0x09, 0x0A, 0x0A, 0x0A, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x08,
    0x08, 0x08, 0x0A, 0x0A, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x02, 0x04, 0x06,
    0x03, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x00
};

// Font 5
static const u16 sFont5LatinGlyphs[] = INCBIN_U16("graphics/fonts/font5_latin.latfont");
static const u8 sFont5LatinGlyphWidths[] =
{
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x09, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0A, 0x08, 0x05, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x06, 0x08, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x0C, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x05, 0x06, 0x06, 0x06, 0x03, 0x03, 0x06,
    0x06, 0x08, 0x05, 0x09, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x04, 0x06, 0x05,
    0x05, 0x06, 0x05, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x08, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x0C, 0x0C, 0x0C, 0x0C, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x06
};
static const u16 sFont5JapaneseGlyphs[] = INCBIN_U16("graphics/fonts/font5_jap.fwjpnfont");
static const u8 sFont5JapaneseGlyphWidths[] =
{
    0x00, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x08, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x05, 0x09, 0x0A, 0x0A, 0x0A, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x08,
    0x08, 0x08, 0x0A, 0x0A, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x02, 0x04, 0x06,
    0x03, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x00
};

// Font 9
static const u16 sFont9JapaneseGlyphs[] = INCBIN_U16("graphics/fonts/font9_jap.fwjpnfont");

u16 Font0Func(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;

    if (subStruct->hasGlyphIdBeenSet == 0)
    {
        textPrinter->subUnion.sub.glyphId = 0;
        subStruct->hasGlyphIdBeenSet = 1;
    }
    return RenderText(textPrinter);
}

u16 Font1Func(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;

    if (subStruct->hasGlyphIdBeenSet == 0)
    {
        textPrinter->subUnion.sub.glyphId = 1;
        subStruct->hasGlyphIdBeenSet = 1;
    }
    return RenderText(textPrinter);
}

u16 Font2Func(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;

    if (subStruct->hasGlyphIdBeenSet == 0)
    {
        textPrinter->subUnion.sub.glyphId = 2;
        subStruct->hasGlyphIdBeenSet = 1;
    }
    return RenderText(textPrinter);
}

u16 Font3Func(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;

    if (subStruct->hasGlyphIdBeenSet == 0)
    {
        textPrinter->subUnion.sub.glyphId = 3;
        subStruct->hasGlyphIdBeenSet = 1;
    }
    return RenderText(textPrinter);
}

u16 Font4Func(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;

    if (subStruct->hasGlyphIdBeenSet == 0)
    {
        textPrinter->subUnion.sub.glyphId = 4;
        subStruct->hasGlyphIdBeenSet = 1;
    }
    return RenderText(textPrinter);
}

u16 Font5Func(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;

    if (subStruct->hasGlyphIdBeenSet == 0)
    {
        textPrinter->subUnion.sub.glyphId = 5;
        subStruct->hasGlyphIdBeenSet = 1;
    }
    return RenderText(textPrinter);
}

void TextPrinterInitDownArrowCounters(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;

    if (gTextFlags.autoScroll == 1)
        subStruct->autoScrollDelay = 0;
    else
    {
        subStruct->downArrowYPosIdx = 0;
        subStruct->downArrowDelay = 0;
    }
}

void TextPrinterDrawDownArrow(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;
    const u8 *arrowTiles;

    if (gTextFlags.autoScroll == 0)
    {
        if (subStruct->downArrowDelay != 0)
        {
            subStruct->downArrowDelay = ((*(u32*)&textPrinter->subUnion.sub) << 19 >> 27) - 1;    // convoluted way of getting field_1, necessary to match
        }
        else
        {
            FillWindowPixelRect(
                textPrinter->printerTemplate.windowId,
                textPrinter->printerTemplate.bgColor << 4 | textPrinter->printerTemplate.bgColor,
                textPrinter->printerTemplate.currentX,
                textPrinter->printerTemplate.currentY,
                10,
                12);

            switch (gTextFlags.useAlternateDownArrow)
            {
                case 0:
                default:
                    arrowTiles = sDownArrowTiles;
                    break;
                case 1:
                    arrowTiles = sDarkDownArrowTiles;
                    break;
            }

            BlitBitmapRectToWindow(
                textPrinter->printerTemplate.windowId,
                arrowTiles,
                sDownArrowYCoords[subStruct->downArrowYPosIdx],
                0,
                0x80,
                0x10,
                textPrinter->printerTemplate.currentX,
                textPrinter->printerTemplate.currentY,
                10,
                12);
            CopyWindowToVram(textPrinter->printerTemplate.windowId, 0x2);

            subStruct->downArrowDelay = 0x8;
            subStruct->downArrowYPosIdx = (*(u32*)subStruct << 17 >> 30) + 1;
        }
    }
}

void TextPrinterClearDownArrow(struct TextPrinter *textPrinter)
{
    FillWindowPixelRect(
        textPrinter->printerTemplate.windowId,
        textPrinter->printerTemplate.bgColor << 4 | textPrinter->printerTemplate.bgColor,
        textPrinter->printerTemplate.currentX,
        textPrinter->printerTemplate.currentY,
        10,
        12);
    CopyWindowToVram(textPrinter->printerTemplate.windowId, 0x2);
}

bool8 TextPrinterWaitAutoMode(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;
	
    if (subStruct->autoScrollDelay == 120)
    {
        return TRUE;
    }
    else
    {
        subStruct->autoScrollDelay++;
        return FALSE;
    }
}

bool16 TextPrinterWaitWithDownArrow(struct TextPrinter *textPrinter)
{
    bool8 result = FALSE;
    if (gTextFlags.autoScroll != 0)
    {
        result = TextPrinterWaitAutoMode(textPrinter);
    }
    else
    {
        TextPrinterDrawDownArrow(textPrinter);
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            result = TRUE;
            PlaySE(SE_SELECT);
        }
    }
    return result;
}

bool16 TextPrinterWait(struct TextPrinter *textPrinter)
{
    bool16 result = FALSE;
    if (gTextFlags.autoScroll != 0)
    {
        result = TextPrinterWaitAutoMode(textPrinter);
    }
    else
    {
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            result = TRUE;
            PlaySE(SE_SELECT);
        }
    }
    return result;
}

void DrawDownArrow(u8 windowId, u16 x, u16 y, u8 bgColor, bool8 drawArrow, u8 *counter, u8 *yCoordIndex)
{
    const u8 *arrowTiles;

    if (*counter != 0)
    {
        --*counter;
    }
    else
    {
        FillWindowPixelRect(windowId, (bgColor << 4) | bgColor, x, y, 10, 12);
        if (drawArrow == 0)
        {
            switch (gTextFlags.useAlternateDownArrow)
            {
                case 0:
                default:
                    arrowTiles = sDownArrowTiles;
                    break;
                case 1:
                    arrowTiles = sDarkDownArrowTiles;
                    break;
            }

            BlitBitmapRectToWindow(
                windowId,
                arrowTiles,
                sDownArrowYCoords[*yCoordIndex & 3],
                0,
                0x80,
                0x10,
                x,
                y,
                10,
                12);
            CopyWindowToVram(windowId, 0x2);
            *counter = 8;
            ++*yCoordIndex;
        }
    }
}

u16 RenderText(struct TextPrinter *textPrinter)
{
    struct TextPrinterSubStruct *subStruct = &textPrinter->subUnion.sub;
    u16 currChar;
    s32 width;
    s32 widthHelper;

    switch (textPrinter->state)
    {
    case 0:
        if (JOY_HELD(A_BUTTON | B_BUTTON) && subStruct->hasPrintBeenSpedUp)
            textPrinter->delayCounter = 0;

        if (textPrinter->delayCounter && textPrinter->textSpeed)
        {
            textPrinter->delayCounter--;
            if (gTextFlags.canABSpeedUpPrint && JOY_NEW(A_BUTTON | B_BUTTON))
            {
                subStruct->hasPrintBeenSpedUp = TRUE;
                textPrinter->delayCounter = 0;
            }
            return 3;
        }

        if (gTextFlags.autoScroll)
            textPrinter->delayCounter = 1;
        else
            textPrinter->delayCounter = textPrinter->textSpeed;

        currChar = *textPrinter->printerTemplate.currentChar;
        textPrinter->printerTemplate.currentChar++;

        switch (currChar)
        {
        case CHAR_NEWLINE:
            textPrinter->printerTemplate.currentX = textPrinter->printerTemplate.x;
            textPrinter->printerTemplate.currentY += gFonts[textPrinter->printerTemplate.fontId].maxLetterHeight + textPrinter->printerTemplate.lineSpacing;
            return 2;
        case PLACEHOLDER_BEGIN:
            textPrinter->printerTemplate.currentChar++;
            return 2;
        case EXT_CTRL_CODE_BEGIN:
            currChar = *textPrinter->printerTemplate.currentChar;
            textPrinter->printerTemplate.currentChar++;
            switch (currChar)
            {
            case EXT_CTRL_CODE_COLOR:
                textPrinter->printerTemplate.fgColor = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                GenerateFontHalfRowLookupTable(textPrinter->printerTemplate.fgColor, textPrinter->printerTemplate.bgColor, textPrinter->printerTemplate.shadowColor);
                return 2;
            case EXT_CTRL_CODE_HIGHLIGHT:
                textPrinter->printerTemplate.bgColor = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                GenerateFontHalfRowLookupTable(textPrinter->printerTemplate.fgColor, textPrinter->printerTemplate.bgColor, textPrinter->printerTemplate.shadowColor);
                return 2;
            case EXT_CTRL_CODE_SHADOW:
                textPrinter->printerTemplate.shadowColor = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                GenerateFontHalfRowLookupTable(textPrinter->printerTemplate.fgColor, textPrinter->printerTemplate.bgColor, textPrinter->printerTemplate.shadowColor);
                return 2;
            case EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW:
                textPrinter->printerTemplate.fgColor = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                textPrinter->printerTemplate.bgColor = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                textPrinter->printerTemplate.shadowColor = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                GenerateFontHalfRowLookupTable(textPrinter->printerTemplate.fgColor, textPrinter->printerTemplate.bgColor, textPrinter->printerTemplate.shadowColor);
                return 2;
            case EXT_CTRL_CODE_PALETTE:
                textPrinter->printerTemplate.currentChar++;
                return 2;
            case EXT_CTRL_CODE_FONT:
                subStruct->glyphId = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                return 2;
            case EXT_CTRL_CODE_RESET_FONT:
                return 2;
            case EXT_CTRL_CODE_PAUSE:
                textPrinter->delayCounter = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                textPrinter->state = 6;
                return 2;
            case EXT_CTRL_CODE_WAIT_BUTTON:
                textPrinter->state = 1;
                if (gTextFlags.autoScroll)
                    subStruct->autoScrollDelay = 0;
                return 3;
            case EXT_CTRL_CODE_WAIT_SE:
                textPrinter->state = 5;
                return 3;
            case EXT_CTRL_CODE_PLAY_BGM:
                currChar = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                currChar |= *textPrinter->printerTemplate.currentChar << 8;
                textPrinter->printerTemplate.currentChar++;
				PlayBGM(currChar);
                return 2;
            case EXT_CTRL_CODE_PLAY_SE:
                currChar = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                currChar |= (*textPrinter->printerTemplate.currentChar << 8);
                textPrinter->printerTemplate.currentChar++;
                PlaySE(currChar);
                return 2;
            case EXT_CTRL_CODE_ESCAPE:
                textPrinter->printerTemplate.currentChar++;
                currChar = *textPrinter->printerTemplate.currentChar;
                break;
            case EXT_CTRL_CODE_SHIFT_RIGHT:
                textPrinter->printerTemplate.currentX = textPrinter->printerTemplate.x + *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                return 2;
            case EXT_CTRL_CODE_SHIFT_DOWN:
                textPrinter->printerTemplate.currentY = textPrinter->printerTemplate.y + *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                return 2;
            case EXT_CTRL_CODE_FILL_WINDOW:
                FillWindowPixelBuffer(textPrinter->printerTemplate.windowId, PIXEL_FILL(textPrinter->printerTemplate.bgColor));
                return 2;
            case EXT_CTRL_CODE_STOP_BGM:
                m4aMPlayStop(&gMPlayInfo_BGM);
                return 2;
            case EXT_CTRL_CODE_RESUME_BGM:
                m4aMPlayContinue(&gMPlayInfo_BGM);
                return 2;
            case EXT_CTRL_CODE_CLEAR:
                width = *textPrinter->printerTemplate.currentChar;
                textPrinter->printerTemplate.currentChar++;
                if (width > 0)
                {
                    ClearTextSpan(textPrinter, width);
                    textPrinter->printerTemplate.currentX += width;
                    return 0;
                }
                return 2;
            case EXT_CTRL_CODE_SKIP:
                textPrinter->printerTemplate.currentX = *textPrinter->printerTemplate.currentChar + textPrinter->printerTemplate.x;
                textPrinter->printerTemplate.currentChar++;
                return 2;
            case EXT_CTRL_CODE_CLEAR_TO:
                {
                    widthHelper = *textPrinter->printerTemplate.currentChar;
                    widthHelper += textPrinter->printerTemplate.x;
                    textPrinter->printerTemplate.currentChar++;
                    width = widthHelper - textPrinter->printerTemplate.currentX;
                    if (width > 0)
                    {
                        ClearTextSpan(textPrinter, width);
                        textPrinter->printerTemplate.currentX += width;
                        return 0;
                    }
                }
                return 2;
            case EXT_CTRL_CODE_MIN_LETTER_SPACING:
                textPrinter->minLetterSpacing = *textPrinter->printerTemplate.currentChar++;
                return 2;
            case EXT_CTRL_CODE_JPN:
                textPrinter->japanese = 1;
                return 2;
            case EXT_CTRL_CODE_ENG:
                textPrinter->japanese = 0;
                return 2;
            }
            break;
        case CHAR_PROMPT_CLEAR:
            textPrinter->state = 2;
            TextPrinterInitDownArrowCounters(textPrinter);
            return 3;
        case CHAR_PROMPT_SCROLL:
            textPrinter->state = 3;
            TextPrinterInitDownArrowCounters(textPrinter);
            return 3;
        case CHAR_EXTRA_EMOJI:
            currChar = *textPrinter->printerTemplate.currentChar | 0x100;
            textPrinter->printerTemplate.currentChar++;
            break;
        case CHAR_KEYPAD_ICON:
            currChar = *textPrinter->printerTemplate.currentChar++;
            gGlyphInfo.width = DrawKeypadIcon(textPrinter->printerTemplate.windowId, currChar, textPrinter->printerTemplate.currentX, textPrinter->printerTemplate.currentY);
            textPrinter->printerTemplate.currentX += gGlyphInfo.width + textPrinter->printerTemplate.letterSpacing;
            return 0;
        case EOS:
            return 1;
        }

        switch (subStruct->glyphId)
        {
        case 0:
            DecompressGlyphFont0(currChar, textPrinter->japanese);
            break;
        case 1:
            DecompressGlyphFont1(currChar, textPrinter->japanese);
            break;
        case 2:
            DecompressGlyphFont2(currChar, textPrinter->japanese);
            break;
        case 3:
            DecompressGlyphFont3(currChar, textPrinter->japanese);
            break;
        case 4:
            DecompressGlyphFont4(currChar, textPrinter->japanese);
            break;
        case 5:
            DecompressGlyphFont5(currChar, textPrinter->japanese);
        }

        CopyGlyphToWindow(textPrinter);

        if (textPrinter->minLetterSpacing)
        {
            textPrinter->printerTemplate.currentX += gGlyphInfo.width;
            width = textPrinter->minLetterSpacing - gGlyphInfo.width;
            if (width > 0)
            {
                ClearTextSpan(textPrinter, width);
                textPrinter->printerTemplate.currentX += width;
            }
        }
        else
        {
            if (textPrinter->japanese)
                textPrinter->printerTemplate.currentX += (gGlyphInfo.width + textPrinter->printerTemplate.letterSpacing);
            else
                textPrinter->printerTemplate.currentX += gGlyphInfo.width;
        }
        return 0;
    case 1:
        if (TextPrinterWait(textPrinter))
            textPrinter->state = 0;
        return 3;
    case 2:
        if (TextPrinterWaitWithDownArrow(textPrinter))
        {
            FillWindowPixelBuffer(textPrinter->printerTemplate.windowId, PIXEL_FILL(textPrinter->printerTemplate.bgColor));
            textPrinter->printerTemplate.currentX = textPrinter->printerTemplate.x;
            textPrinter->printerTemplate.currentY = textPrinter->printerTemplate.y;
            textPrinter->state = 0;
        }
        return 3;
    case 3:
        if (TextPrinterWaitWithDownArrow(textPrinter))
        {
            TextPrinterClearDownArrow(textPrinter);
            textPrinter->scrollDistance = gFonts[textPrinter->printerTemplate.fontId].maxLetterHeight + textPrinter->printerTemplate.lineSpacing;
            textPrinter->printerTemplate.currentX = textPrinter->printerTemplate.x;
            textPrinter->state = 4;
        }
        return 3;
    case 4:
        if (textPrinter->scrollDistance)
        {
    
            if (textPrinter->scrollDistance < sWindowVerticalScrollSpeeds[gSaveBlock2Ptr->optionsTextSpeed])
            {
                ScrollWindow(textPrinter->printerTemplate.windowId, 0, textPrinter->scrollDistance, PIXEL_FILL(textPrinter->printerTemplate.bgColor));
                textPrinter->scrollDistance = 0;
            }
            else
            {
                ScrollWindow(textPrinter->printerTemplate.windowId, 0, sWindowVerticalScrollSpeeds[gSaveBlock2Ptr->optionsTextSpeed], PIXEL_FILL(textPrinter->printerTemplate.bgColor));
                textPrinter->scrollDistance -= sWindowVerticalScrollSpeeds[gSaveBlock2Ptr->optionsTextSpeed];
            }
            CopyWindowToVram(textPrinter->printerTemplate.windowId, COPYWIN_GFX);
        }
        else
        {
            textPrinter->state = 0;
        }
        return 3;
    case 5:
        if (!IsSEPlaying())
            textPrinter->state = 0;
        return 3;
    case 6:
        if (textPrinter->delayCounter != 0)
            textPrinter->delayCounter--;
        else
            textPrinter->state = 0;
        return 3;
    }

    return 1;
}

static void DecompressAndRenderGlyph(u8 fontId, u16 glyph, struct Bitmap *srcBlit, struct Bitmap *destBlit, u8 *destBuffer, u8 x, u8 y, u8 width, u8 height)
{
    if (fontId == 0)
        DecompressGlyphFont0(glyph, FALSE);
    else if (fontId == 5)
        DecompressGlyphFont5(glyph, FALSE);
    else
        DecompressGlyphFont2(glyph, FALSE);
    srcBlit->pixels = gGlyphInfo.pixels;
    srcBlit->width = 16;
    srcBlit->height = 16;
    destBlit->pixels = destBuffer;
    destBlit->width = width * 8;
    destBlit->height = height * 8;
    BlitBitmapRect4Bit(srcBlit, destBlit, 0, 0, x, y, gGlyphInfo.width, gGlyphInfo.height, 0);
}

void RenderSaveFailedScreenText(u8 fontId, u8 * dest, const u8 * src, u8 x, u8 y, u8 width, u8 height)
{
	// fontId -> sp+24
    // dest -> sp+28
    // src -> r9
    // x -> sp+34
    // y -> r10
    // width -> sp+2C
    // height -> sp+30
    struct Bitmap srcBlit;
    struct Bitmap destBlit;
    u8 orig_x = x;
    u8 i = 0;
    s32 clearPixels = 0;

    while (1)
    {
        u16 curChar = *src;
        src++;
        switch (curChar)
        {
        case EOS:
            return;
        case CHAR_NEWLINE:
            x = orig_x;
            y += gGlyphInfo.height + 1;
            break;
        case PLACEHOLDER_BEGIN:
            curChar = *src;
            src++;
            if (curChar == PLACEHOLDER_ID_PLAYER)
            {
                for (i = 0; i < 10; i++)
                {
                    if (gSaveBlock2Ptr->playerName[i] == EOS)
                    {
                        break;
                    }
                    DecompressAndRenderGlyph(fontId, gSaveBlock2Ptr->playerName[i], &srcBlit, &destBlit, dest, x, y, width, height);
					x += gGlyphInfo.width;
                }
            }
            else if (curChar == PLACEHOLDER_ID_STRING_VAR_1)
            {
                for (i = 0; ; i++)
                {
                    if (FlagGet(FLAG_SYS_NOT_SOMEONES_PC) == TRUE)
                    {
                        if (gString_Bill[i] == EOS)
                        {
                            break;
                        }
                        DecompressAndRenderGlyph(fontId, gString_Bill[i], &srcBlit, &destBlit, dest, x, y, width, height);
                    }
                    else
                    {
                        if (gString_Someone[i] == EOS)
                        {
                            break;
                        }
                        DecompressAndRenderGlyph(fontId, gString_Someone[i], &srcBlit, &destBlit, dest, x, y, width, height);
                    }
					x += gGlyphInfo.width;
                }
            }
            break;
        case CHAR_PROMPT_SCROLL:
        case CHAR_PROMPT_CLEAR:
            x = orig_x;
            y += gGlyphInfo.height + 1;
            break;
        case EXT_CTRL_CODE_BEGIN:
            curChar = *src;
            src++;
            switch (curChar)
            {
            case EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW:
                src++;
                //fallthrough
            case EXT_CTRL_CODE_PLAY_BGM:
            case EXT_CTRL_CODE_PLAY_SE:
                src++;
                //fallthrough
            case EXT_CTRL_CODE_COLOR:
            case EXT_CTRL_CODE_HIGHLIGHT:
            case EXT_CTRL_CODE_SHADOW:
            case EXT_CTRL_CODE_PALETTE:
            case EXT_CTRL_CODE_FONT:
            case EXT_CTRL_CODE_PAUSE:
            case EXT_CTRL_CODE_ESCAPE:
            case EXT_CTRL_CODE_SHIFT_RIGHT:
            case EXT_CTRL_CODE_SHIFT_DOWN:
                src++;
            case EXT_CTRL_CODE_RESET_FONT:
            case EXT_CTRL_CODE_WAIT_BUTTON:
            case EXT_CTRL_CODE_WAIT_SE:
            case EXT_CTRL_CODE_FILL_WINDOW:
                break;
            case EXT_CTRL_CODE_CLEAR:
            case EXT_CTRL_CODE_SKIP:
                src++;
                break;
            case EXT_CTRL_CODE_CLEAR_TO:
            {
                clearPixels = *src + orig_x - x;

                if (clearPixels > 0)
                {
                    destBlit.pixels = dest;
                    destBlit.width = width * 8;
                    destBlit.height = height * 8;
                    FillBitmapRect4Bit(&destBlit, x, y, clearPixels, GetFontAttribute(fontId, FONTATTR_MAX_LETTER_HEIGHT), 0);
                    x += clearPixels;
                }
                src++;
                break;
            }
            case EXT_CTRL_CODE_MIN_LETTER_SPACING:
                src++;
                break;
            case EXT_CTRL_CODE_JPN:
            case EXT_CTRL_CODE_ENG:
                break;
            }
            break;
        case CHAR_KEYPAD_ICON:
            curChar = *src;
            src++;
            srcBlit.pixels = (u8 *)&gKeypadIconTiles[0x20 * GetKeypadIconTileOffset(curChar)];
            srcBlit.width = 0x80;
            srcBlit.height = 0x80;
            destBlit.pixels = dest;
            destBlit.width = width * 8;
            destBlit.height = height * 8;
            BlitBitmapRect4Bit(&srcBlit, &destBlit, 0, 0, x, y, GetKeypadIconWidth(curChar), GetKeypadIconHeight(curChar), 0);
            x += GetKeypadIconWidth(curChar);
            break;
        case CHAR_EXTRA_EMOJI:
            curChar = *src + 0x100;
            src++;
            //fallthrough
        default:
            if (curChar == CHAR_SPACE)
            {
                if (fontId == 0)
                {
                    x += 5;
                }
                else
                {
                    x += 4;
                }
            }
            else
            {
                DecompressAndRenderGlyph(fontId, curChar, &srcBlit, &destBlit, dest, x, y, width, height);
				x += gGlyphInfo.width;
            }
            break;
        }
    }
}

s32 GetStringWidthFixedWidthFont(const u8 *str, u8 fontId, u8 letterSpacing)
{
    int i;
    u8 width;
    int temp;
    int temp2;
    u8 line;
    int strPos;
    u8 lineWidths[8];
    const u8 *strLocal;

    for (i = 0; i < 8; i++)
    {
        lineWidths[i] = 0;
    }

    width = 0;
    line = 0;
    strLocal = str;
    strPos = 0;

    do
    {
        temp = strLocal[strPos++];
        switch (temp)
        {
        case CHAR_NEWLINE:
        case EOS:
            lineWidths[line] = width;
            width = 0;
            line++;
            break;
        case EXT_CTRL_CODE_BEGIN:
            temp2 = strLocal[strPos++];
            switch (temp2)
            {
            case EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW:
                ++strPos;
            case EXT_CTRL_CODE_PLAY_BGM:
            case EXT_CTRL_CODE_PLAY_SE:
                ++strPos;
            case EXT_CTRL_CODE_COLOR:
            case EXT_CTRL_CODE_HIGHLIGHT:
            case EXT_CTRL_CODE_SHADOW:
            case EXT_CTRL_CODE_PALETTE:
            case EXT_CTRL_CODE_FONT:
            case EXT_CTRL_CODE_PAUSE:
            case EXT_CTRL_CODE_ESCAPE:
            case EXT_CTRL_CODE_SHIFT_RIGHT:
            case EXT_CTRL_CODE_SHIFT_DOWN:
            case EXT_CTRL_CODE_CLEAR:
            case EXT_CTRL_CODE_SKIP:
            case EXT_CTRL_CODE_CLEAR_TO:
            case EXT_CTRL_CODE_MIN_LETTER_SPACING:
                ++strPos;
                break;
            case EXT_CTRL_CODE_RESET_FONT:
            case EXT_CTRL_CODE_WAIT_BUTTON:
            case EXT_CTRL_CODE_WAIT_SE:
            case EXT_CTRL_CODE_FILL_WINDOW:
            case EXT_CTRL_CODE_JPN:
            case EXT_CTRL_CODE_ENG:
            default:
                break;
            }
            break;
        case CHAR_DYNAMIC_PLACEHOLDER:
        case PLACEHOLDER_BEGIN:
            ++strPos;
            break;
        case CHAR_PROMPT_SCROLL:
        case CHAR_PROMPT_CLEAR:
            break;
        case CHAR_KEYPAD_ICON:
        case CHAR_EXTRA_EMOJI:
            ++strPos;
        default:
            ++width;
            break;
        }
    } while (temp != EOS);

    for (width = 0, strPos = 0; strPos < 8; ++strPos)
    {
        if (width < lineWidths[strPos])
            width = lineWidths[strPos];
    }

    return (u8)(GetFontAttribute(fontId, 0) + letterSpacing) * width;
}

s32 (*GetFontWidthFunc(u8 glyphId))(u16 _glyphId, bool32 _isJapanese)
{
    u32 i;

    for (i = 0; i < 7; ++i)
    {
        if (glyphId == sGlyphWidthFuncs[i].fontId)
            return *sGlyphWidthFuncs[i].func;
    }

    return NULL;
}

s32 GetStringWidth(u8 fontId, const u8 *str, s16 letterSpacing)
{
    bool8 isJapanese;
    int minGlyphWidth;
    s32 (*func)(u16 glyphId, bool32 isJapanese);
    int localLetterSpacing;
    u32 lineWidth;
    const u8 *bufferPointer;
    int glyphWidth;
    u32 width;

    isJapanese = FALSE;
    minGlyphWidth = 0;

    func = GetFontWidthFunc(fontId);
    if (func == NULL)
        return 0;

    if (letterSpacing == -1)
        localLetterSpacing = GetFontAttribute(fontId, FONTATTR_LETTER_SPACING);
    else
        localLetterSpacing = letterSpacing;

    width = 0;
    lineWidth = 0;
    bufferPointer = NULL;

    while (*str != EOS)
    {
        switch (*str)
        {
        case CHAR_NEWLINE:
            if (lineWidth > width)
                width = lineWidth;
            lineWidth = 0;
            break;
        case PLACEHOLDER_BEGIN:
            switch (*++str)
            {
                case PLACEHOLDER_ID_STRING_VAR_1:
                    bufferPointer = gStringVar1;
                    break;
                case PLACEHOLDER_ID_STRING_VAR_2:
                    bufferPointer = gStringVar2;
                    break;
                case PLACEHOLDER_ID_STRING_VAR_3:
                    bufferPointer = gStringVar3;
                    break;
                default:
                    return 0;
            }
        case CHAR_DYNAMIC_PLACEHOLDER:
            if (bufferPointer == NULL)
                bufferPointer = DynamicPlaceholderTextUtil_GetPlaceholderPtr(*++str);
            while (*bufferPointer != EOS)
            {
                glyphWidth = func(*bufferPointer++, isJapanese);
                if (minGlyphWidth > 0)
                    lineWidth += minGlyphWidth > glyphWidth ? minGlyphWidth : glyphWidth;
                else
                    lineWidth += isJapanese ? glyphWidth + localLetterSpacing : glyphWidth;
            }
            bufferPointer = NULL;
            break;
        case CHAR_PROMPT_SCROLL:
        case CHAR_PROMPT_CLEAR:
            break;
        case EXT_CTRL_CODE_BEGIN:
            switch (*++str)
            {
            case EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW:
                ++str;
            case EXT_CTRL_CODE_PLAY_BGM:
            case EXT_CTRL_CODE_PLAY_SE:
                ++str;
            case EXT_CTRL_CODE_COLOR:
            case EXT_CTRL_CODE_HIGHLIGHT:
            case EXT_CTRL_CODE_SHADOW:
            case EXT_CTRL_CODE_PALETTE:
            case EXT_CTRL_CODE_PAUSE:
            case EXT_CTRL_CODE_ESCAPE:
            case EXT_CTRL_CODE_SHIFT_RIGHT:
            case EXT_CTRL_CODE_SHIFT_DOWN:
                ++str;
            case EXT_CTRL_CODE_RESET_FONT:
            case EXT_CTRL_CODE_WAIT_BUTTON:
            case EXT_CTRL_CODE_WAIT_SE:
            case EXT_CTRL_CODE_FILL_WINDOW:
                break;
            case EXT_CTRL_CODE_FONT:
                func = GetFontWidthFunc(*++str);
                if (func == NULL)
                    return 0;
                if (letterSpacing == -1)
                    localLetterSpacing = GetFontAttribute(*str, FONTATTR_LETTER_SPACING);
                break;
            case EXT_CTRL_CODE_CLEAR:
                glyphWidth = *++str;
                lineWidth += glyphWidth;
                break;
            case EXT_CTRL_CODE_SKIP:
                lineWidth = *++str;
                break;
            case EXT_CTRL_CODE_CLEAR_TO:
                if (*++str > lineWidth)
                    lineWidth = *str;
                break;
            case EXT_CTRL_CODE_MIN_LETTER_SPACING:
                minGlyphWidth = *++str;
                break;
            case EXT_CTRL_CODE_JPN:
                isJapanese = TRUE;
                break;
            case EXT_CTRL_CODE_ENG:
                isJapanese = FALSE;
            default:
                break;
            }
            break;
        case CHAR_KEYPAD_ICON:
        case CHAR_EXTRA_EMOJI:
            if (*str == CHAR_EXTRA_EMOJI)
                glyphWidth = func(*++str | 0x100, isJapanese);
            else
                glyphWidth = GetKeypadIconWidth(*++str);

            if (minGlyphWidth > 0)
            {
                if (glyphWidth < minGlyphWidth)
                    glyphWidth = minGlyphWidth;
            }
            else if (isJapanese)
            {
                glyphWidth += localLetterSpacing;
            }
            lineWidth += glyphWidth;
            break;
        default:
            glyphWidth = func(*str, isJapanese);
            if (minGlyphWidth > 0)
            {
                if (glyphWidth < minGlyphWidth)
                    glyphWidth = minGlyphWidth;
                lineWidth += glyphWidth;
            }
            else
            {
                if (fontId != 6 && isJapanese)
                {
                    glyphWidth += localLetterSpacing;
                }
                lineWidth += glyphWidth;
            }
            break;
        }
        ++str;
    }

    if (lineWidth > width)
        return lineWidth;
    return width;
}

u8 RenderTextFont9(u8 *pixels, u8 fontId, u8 *str, int a3, int a4, int a5, int a6, int a7)
{
    u8 shadowColor;
    u8 *strLocal;
    int strPos;
    int temp;
    int temp2;
    u8 colorBackup[3];
    u8 fgColor;
    u8 bgColor;

    SaveTextColors(&colorBackup[0], &colorBackup[1], &colorBackup[2]);

    fgColor = 1;
    bgColor = 0;
    shadowColor = 3;

    GenerateFontHalfRowLookupTable(1, 0, 3);
    strLocal = str;
    strPos = 0;

    do
    {
        temp = strLocal[strPos++];
        switch (temp)
        {
        case EXT_CTRL_CODE_BEGIN:
            temp2 = strLocal[strPos++];
            switch (temp2)
            {
            case EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW:
                fgColor = strLocal[strPos++];
                bgColor = strLocal[strPos++];
                shadowColor = strLocal[strPos++];
                GenerateFontHalfRowLookupTable(fgColor, bgColor, shadowColor);
                continue;
            case EXT_CTRL_CODE_COLOR:
                fgColor = strLocal[strPos++];
                GenerateFontHalfRowLookupTable(fgColor, bgColor, shadowColor);
                continue;
            case EXT_CTRL_CODE_HIGHLIGHT:
                bgColor = strLocal[strPos++];
                GenerateFontHalfRowLookupTable(fgColor, bgColor, shadowColor);
                continue;
            case EXT_CTRL_CODE_SHADOW:
                shadowColor = strLocal[strPos++];
                GenerateFontHalfRowLookupTable(fgColor, bgColor, shadowColor);
                continue;
            case EXT_CTRL_CODE_FONT:
                fontId = strLocal[strPos++];
                break;
            case EXT_CTRL_CODE_PLAY_BGM:
            case EXT_CTRL_CODE_PLAY_SE:
                ++strPos;
            case EXT_CTRL_CODE_PALETTE:
            case EXT_CTRL_CODE_PAUSE:
            case EXT_CTRL_CODE_ESCAPE:
            case EXT_CTRL_CODE_SHIFT_RIGHT:
            case EXT_CTRL_CODE_SHIFT_DOWN:
            case EXT_CTRL_CODE_CLEAR:
            case EXT_CTRL_CODE_SKIP:
            case EXT_CTRL_CODE_CLEAR_TO:
            case EXT_CTRL_CODE_MIN_LETTER_SPACING:
                ++strPos;
                break;
            case EXT_CTRL_CODE_RESET_FONT:
            case EXT_CTRL_CODE_WAIT_BUTTON:
            case EXT_CTRL_CODE_WAIT_SE:
            case EXT_CTRL_CODE_FILL_WINDOW:
            case EXT_CTRL_CODE_JPN:
            case EXT_CTRL_CODE_ENG:
            default:
                continue;
            }
            break;
        case CHAR_DYNAMIC_PLACEHOLDER:
        case CHAR_KEYPAD_ICON:
        case CHAR_EXTRA_EMOJI:
        case PLACEHOLDER_BEGIN:
            ++strPos;
            break;
        case CHAR_PROMPT_SCROLL:
        case CHAR_PROMPT_CLEAR:
        case CHAR_NEWLINE:
        case EOS:
            break;
        default:
            DecompressGlyphFont9(temp);
            CpuCopy32(gGlyphInfo.pixels, pixels, 0x20);
            CpuCopy32(gGlyphInfo.pixels + 0x40, pixels + 0x20, 0x20);
            pixels += 0x40;
            break;
        }
    }
    while (temp != EOS);

    RestoreTextColors(&colorBackup[0], &colorBackup[1], &colorBackup[2]);
    return 1;
}

void sub_80062B0(struct Sprite *sprite)
{
    if(sprite->data[0])
    {
        sprite->data[0]--;
    }
    else
    {
        sprite->data[0] = 8;
        switch(sprite->data[1])
        {
            case 0:
                sprite->y2 = 0;
                break;
            case 1:
                sprite->y2 = 1;
                break;
            case 2:
                sprite->y2 = 2;
                break;
            case 3:
                sprite->y2 = 1;
                sprite->data[1] = 0;
                return;
        }
        sprite->data[1]++;
    }
}

u8 CreateTextCursorSpriteForOakSpeech(u8 sheetId, u16 x, u16 y, u8 priority, u8 subpriority)
{
    u8 spriteId;
    LoadSpriteSheet(&sUnknown_81EA68C[sheetId & 1]);
    LoadSpritePalette(sUnknown_81EA6A4);
    spriteId = CreateSprite(&sUnknown_81EA6B4, x + 3, y + 4, subpriority);
    gSprites[spriteId].oam.priority = (priority & 3);
    gSprites[spriteId].oam.matrixNum = 0;
    gSprites[spriteId].data[0] = 8;
    return spriteId;
}

void DestroyTextCursorSprite(u8 spriteId)
{
    DestroySprite(&gSprites[spriteId]);
    FreeSpriteTilesByTag(0x8000);
    FreeSpritePaletteByTag(0x8000);
}

u8 DrawKeypadIcon(u8 windowId, u8 keypadIconId, u16 x, u16 y)
{
    BlitBitmapRectToWindow(
        windowId,
        gKeypadIconTiles + (sKeypadIcons[keypadIconId].tileOffset * 0x20),
        0,
        0,
        0x80,
        0x80,
        x,
        y,
        sKeypadIcons[keypadIconId].width,
        sKeypadIcons[keypadIconId].height);
    return sKeypadIcons[keypadIconId].width;
}

u8 GetKeypadIconTileOffset(u8 keypadIconId)
{
    return sKeypadIcons[keypadIconId].tileOffset;
}

u8 GetKeypadIconWidth(u8 keypadIconId)
{
    return sKeypadIcons[keypadIconId].width;
}

u8 GetKeypadIconHeight(u8 keypadIconId)
{
    return sKeypadIcons[keypadIconId].height;
}

void DecompressGlyphFont0(u16 glyphId, bool32 isJapanese)
{
    const u16* glyphs;

    if (isJapanese == 1)
    {
        glyphs = sFont0JapaneseGlyphs + (0x100 * (glyphId >> 0x4)) + (0x8 * (glyphId & 0xF));
        DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
        DecompressGlyphTile(glyphs + 0x80, (u16 *)(gGlyphInfo.pixels + 0x40));
        gGlyphInfo.width = 8;
        gGlyphInfo.height = 12;
    }
    else
    {
        glyphs = sFont0LatinGlyphs + (0x10 * glyphId);
        DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
        DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x40));
        gGlyphInfo.width = sFont0LatinGlyphWidths[glyphId];
        gGlyphInfo.height = 13;
    }
}

s32 GetGlyphWidthFont0(u16 glyphId, bool32 isJapanese)
{
    if (isJapanese == TRUE)
        return 8;
    else
        return sFont0LatinGlyphWidths[glyphId];
}

void DecompressGlyphFont1(u16 glyphId, bool32 isJapanese)
{
    const u16* glyphs;

    if (isJapanese == TRUE)
    {
        int eff;
        glyphs = sFont1JapaneseGlyphs + (0x100 * (glyphId >> 0x4)) + (0x8 * (glyphId & (eff = 0xF)));  // shh, no questions, only matching now
        DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
        DecompressGlyphTile(glyphs + 0x80, (u16 *)(gGlyphInfo.pixels + 0x40));
        gGlyphInfo.width = 8;
        gGlyphInfo.height = 16;
    }
    else
    {
        glyphs = sFont1LatinGlyphs + (0x20 * glyphId);
        DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
        DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x20));
        DecompressGlyphTile(glyphs + 0x10, (u16 *)(gGlyphInfo.pixels + 0x40));
        DecompressGlyphTile(glyphs + 0x18, (u16 *)(gGlyphInfo.pixels + 0x60));
        gGlyphInfo.width = sFont1LatinGlyphWidths[glyphId];
        gGlyphInfo.height = 14;
    }
}

s32 GetGlyphWidthFont1(u16 glyphId, bool32 isJapanese)
{
    if (isJapanese == TRUE)
        return 8;
    else
        return sFont1LatinGlyphWidths[glyphId];
}

void DecompressGlyphFont2(u16 glyphId, bool32 isJapanese)
{
    const u16* glyphs;
    int i;
    u8 lastColor;

    if(isJapanese == TRUE)
    {
        if(glyphId == 0)
        {
            lastColor = GetLastTextColor(2);

            for(i = 0; i < 0x80; i++)
            {
                gGlyphInfo.pixels[i] = lastColor | lastColor << 4;
                // Game Freak, please. writing the same values over and over...
                gGlyphInfo.width = 10;
                gGlyphInfo.height = 12;
            }
        }
        else
        {
            glyphs = sFont2JapaneseGlyphs + (0x100 * (glyphId >> 0x3)) + (0x10 * (glyphId & 0x7));
            DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
            DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x20));
            DecompressGlyphTile(glyphs + 0x80, (u16 *)(gGlyphInfo.pixels + 0x40));
            DecompressGlyphTile(glyphs + 0x88, (u16 *)(gGlyphInfo.pixels + 0x60));
            gGlyphInfo.width = sFont2JapaneseGlyphWidths[glyphId];
            gGlyphInfo.height = 12;
        }
    }
    else
    {
        if(glyphId == 0)
        {
            lastColor = GetLastTextColor(2);

            for(i = 0; i < 0x80; i++)
            {
                gGlyphInfo.pixels[i] = lastColor | lastColor << 4;
                // but why
                gGlyphInfo.width = sFont2LatinGlyphWidths[0];
                gGlyphInfo.height = 14;
            }
        }
        else
        {
            glyphs = sFont2LatinGlyphs + (0x20 * glyphId);
            DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
            DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x20));
            DecompressGlyphTile(glyphs + 0x10, (u16 *)(gGlyphInfo.pixels + 0x40));
            DecompressGlyphTile(glyphs + 0x18, (u16 *)(gGlyphInfo.pixels + 0x60));
            gGlyphInfo.width = sFont2LatinGlyphWidths[glyphId];
            gGlyphInfo.height = 14;
        }
    }
}

s32 GetGlyphWidthFont2(u16 glyphId, bool32 isJapanese)
{
    if (isJapanese == TRUE)
    {
        if(glyphId == 0)
            return 10;

        return sFont2JapaneseGlyphWidths[glyphId];
    }
    else
    {
        return sFont2LatinGlyphWidths[glyphId];
    }
}

static void DecompressGlyphFont3(u16 glyphId, bool32 isJapanese)
{
    const u16* glyphs;
    int i;
    u8 lastColor;

    if(isJapanese == TRUE)
    {
        if(glyphId == 0)
        {
            lastColor = GetLastTextColor(2);

            for(i = 0; i < 0x80; i++)
            {
                gGlyphInfo.pixels[i] = lastColor | lastColor << 4;
                // Game Freak, please. writing the same values over and over...
                gGlyphInfo.width = 10;
                gGlyphInfo.height = 12;
            }
        }
        else
        {
            glyphs = sFont2JapaneseGlyphs + (0x100 * (glyphId >> 0x3)) + (0x10 * (glyphId & 0x7));
            DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
            DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x20));
            DecompressGlyphTile(glyphs + 0x80, (u16 *)(gGlyphInfo.pixels + 0x40));
            DecompressGlyphTile(glyphs + 0x88, (u16 *)(gGlyphInfo.pixels + 0x60));
            gGlyphInfo.width = 10;
            gGlyphInfo.height = 12;
        }
    }
    else
        DecompressGlyphFont2(glyphId, isJapanese);
}

s32 GetGlyphWidthFont3(u16 glyphId, bool32 isJapanese)
{
    if(isJapanese == TRUE)
        return 10;
    else
        return sFont2LatinGlyphWidths[glyphId];
}

static void DecompressGlyphFont4(u16 glyphId, bool32 isJapanese)
{
    const u16* glyphs;
    int i;
    u8 lastColor;

    if(isJapanese == TRUE)
    {
        if(glyphId == 0)
        {
            lastColor = GetLastTextColor(2);

            for(i = 0; i < 0x80; i++)
            {
                gGlyphInfo.pixels[i] = lastColor | lastColor << 4;
                // Game Freak, please. writing the same values over and over...
                gGlyphInfo.width = 10;
                gGlyphInfo.height = 12;
            }
        }
        else
        {
            glyphs = sFont4JapaneseGlyphs + (0x100 * (glyphId >> 0x3)) + (0x10 * (glyphId & 0x7));
            DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
            DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x20));
            DecompressGlyphTile(glyphs + 0x80, (u16 *)(gGlyphInfo.pixels + 0x40));
            DecompressGlyphTile(glyphs + 0x88, (u16 *)(gGlyphInfo.pixels + 0x60));
            gGlyphInfo.width = sFont4JapaneseGlyphWidths[glyphId];
            gGlyphInfo.height = 12;
        }
    }
    else
    {
        if(glyphId == 0)
        {
            lastColor = GetLastTextColor(2);

            for(i = 0; i < 0x80; i++)
            {
                gGlyphInfo.pixels[i] = lastColor | lastColor << 4;
                // but why
                gGlyphInfo.width = sFont4LatinGlyphWidths[0];
                gGlyphInfo.height = 14;
            }
        }
        else
        {
            glyphs = sFont4LatinGlyphs + (0x20 * glyphId);
            DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
            DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x20));
            DecompressGlyphTile(glyphs + 0x10, (u16 *)(gGlyphInfo.pixels + 0x40));
            DecompressGlyphTile(glyphs + 0x18, (u16 *)(gGlyphInfo.pixels + 0x60));
            gGlyphInfo.width = sFont4LatinGlyphWidths[glyphId];
            gGlyphInfo.height = 14;
        }
    }
}

s32 GetGlyphWidthFont4(u16 glyphId, bool32 isJapanese)
{
    if(isJapanese == TRUE)
    {
        if(glyphId == 0)
            return 10;

        return sFont4JapaneseGlyphWidths[glyphId];
    }
    else
        return sFont4LatinGlyphWidths[glyphId];
}

void DecompressGlyphFont5(u16 glyphId, bool32 isJapanese)
{
    const u16* glyphs;
    int i;
    u8 lastColor;

    if(isJapanese == TRUE)
    {
        if(glyphId == 0)
        {
            lastColor = GetLastTextColor(2);

            for(i = 0; i < 0x80; i++)
            {
                gGlyphInfo.pixels[i] = lastColor | lastColor << 4;
                // Game Freak, please. writing the same values over and over...
                gGlyphInfo.width = 10;
                gGlyphInfo.height = 12;
            }
        }
        else
        {
            glyphs = sFont5JapaneseGlyphs + (0x100 * (glyphId >> 0x3)) + (0x10 * (glyphId & 0x7));
            DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
            DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x20));
            DecompressGlyphTile(glyphs + 0x80, (u16 *)(gGlyphInfo.pixels + 0x40));
            DecompressGlyphTile(glyphs + 0x88, (u16 *)(gGlyphInfo.pixels + 0x60));
            gGlyphInfo.width = sFont5JapaneseGlyphWidths[glyphId];
            gGlyphInfo.height = 12;
        }
    }
    else
    {
        if(glyphId == 0)
        {
            lastColor = GetLastTextColor(2);

            for(i = 0; i < 0x80; i++)
            {
                gGlyphInfo.pixels[i] = lastColor | lastColor << 4;
                // but why
                gGlyphInfo.width = sFont5LatinGlyphWidths[0];
                gGlyphInfo.height = 14;
            }
        }
        else
        {
            glyphs = sFont5LatinGlyphs + (0x20 * glyphId);
            DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
            DecompressGlyphTile(glyphs + 0x8, (u16 *)(gGlyphInfo.pixels + 0x20));
            DecompressGlyphTile(glyphs + 0x10, (u16 *)(gGlyphInfo.pixels + 0x40));
            DecompressGlyphTile(glyphs + 0x18, (u16 *)(gGlyphInfo.pixels + 0x60));
            gGlyphInfo.width = sFont5LatinGlyphWidths[glyphId];
            gGlyphInfo.height = 14;
        }
    }
}

s32 GetGlyphWidthFont5(u16 glyphId, bool32 isJapanese)
{
    if(isJapanese == TRUE)
    {
        if(glyphId == 0)
            return 10;
        
        return sFont5JapaneseGlyphWidths[glyphId];
    }
    else
        return sFont5LatinGlyphWidths[glyphId];
}

void DecompressGlyphFont9(u16 glyphId)
{
    const u16* glyphs = sFont9JapaneseGlyphs + (0x100 * (glyphId >> 0x4)) + (0x8 * (glyphId & 0xF));
    DecompressGlyphTile(glyphs, (u16 *)gGlyphInfo.pixels);
    DecompressGlyphTile(glyphs + 0x80, (u16 *)(gGlyphInfo.pixels + 0x40));
    gGlyphInfo.width = 8;
    gGlyphInfo.height = 12;
}
