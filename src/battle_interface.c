#include "global.h"
#include "gflib.h"
#include "battle_anim.h"
#include "battle_interface.h"
#include "battle_message.h"
#include "decompress.h"
#include "graphics.h"
#include "menu.h"
#include "strings.h"
#include "pokedex.h"
#include "pokemon_summary_screen.h"
#include "safari_zone.h"
#include "constants/songs.h"

#define GetStringRightAlignXOffset(fontId, string, destWidth) ({ \
    s32 w = GetStringWidth(fontId, string, 0);                   \
    destWidth - w;                                               \
})

#undef abs
#define abs(a) ((a) < 0 ? -(a) : (a))

enum
{   // Corresponds to gHealthboxElementsGfxTable (and the tables after it) in graphics.c
    // These are indexes into the tables, which are filled with 8x8 square pixel data.
    HEALTHBOX_GFX_0, //hp bar [black section]
    HEALTHBOX_GFX_1, //hp bar "H"
    HEALTHBOX_GFX_2, //hp bar "P"
    HEALTHBOX_GFX_HP_BAR_GREEN, //hp bar [0 pixels]
    HEALTHBOX_GFX_4,  //hp bar [1 pixels]
    HEALTHBOX_GFX_5,  //hp bar [2 pixels]
    HEALTHBOX_GFX_6,  //hp bar [3 pixels]
    HEALTHBOX_GFX_7,  //hp bar [4 pixels]
    HEALTHBOX_GFX_8,  //hp bar [5 pixels]
    HEALTHBOX_GFX_9,  //hp bar [6 pixels]
    HEALTHBOX_GFX_10, //hp bar [7 pixels]
    HEALTHBOX_GFX_11, //hp bar [8 pixels]
    HEALTHBOX_GFX_12, //exp bar [0 pixels]
    HEALTHBOX_GFX_13, //exp bar [1 pixels]
    HEALTHBOX_GFX_14, //exp bar [2 pixels]
    HEALTHBOX_GFX_15, //exp bar [3 pixels]
    HEALTHBOX_GFX_16, //exp bar [4 pixels]
    HEALTHBOX_GFX_17, //exp bar [5 pixels]
    HEALTHBOX_GFX_18, //exp bar [6 pixels]
    HEALTHBOX_GFX_19, //exp bar [7 pixels]
    HEALTHBOX_GFX_20, //exp bar [8 pixels]
    HEALTHBOX_GFX_STATUS_PSN_BATTLER0,  //status psn "(P"
    HEALTHBOX_GFX_22,                   //status psn "SN"
    HEALTHBOX_GFX_23,                   //status psn "|)""
    HEALTHBOX_GFX_STATUS_PRZ_BATTLER0,  //status prz
    HEALTHBOX_GFX_25,
    HEALTHBOX_GFX_26,
    HEALTHBOX_GFX_STATUS_SLP_BATTLER0,  //status slp
    HEALTHBOX_GFX_28,
    HEALTHBOX_GFX_29,
    HEALTHBOX_GFX_STATUS_FRZ_BATTLER0,  //status frz
    HEALTHBOX_GFX_31,
    HEALTHBOX_GFX_32,
    HEALTHBOX_GFX_STATUS_BRN_BATTLER0,  //status brn
    HEALTHBOX_GFX_34,
    HEALTHBOX_GFX_35,
    HEALTHBOX_GFX_36, //misc [Black section]
    HEALTHBOX_GFX_37, //misc [Black section]
    HEALTHBOX_GFX_38, //misc [Black section]
    HEALTHBOX_GFX_39, //misc [Blank Health Window?]
    HEALTHBOX_GFX_40, //misc [Blank Health Window?]
    HEALTHBOX_GFX_41, //misc [Blank Health Window?]
    HEALTHBOX_GFX_42, //misc [Blank Health Window?]
    HEALTHBOX_GFX_43, //misc [Top of Health Window?]
    HEALTHBOX_GFX_44, //misc [Top of Health Window?]
    HEALTHBOX_GFX_45, //misc [Top of Health Window?]
    HEALTHBOX_GFX_46, //misc [Blank Health Window?]
    HEALTHBOX_GFX_HP_BAR_YELLOW, //hp bar yellow [0 pixels]
    HEALTHBOX_GFX_48, //hp bar yellow [1 pixels]
    HEALTHBOX_GFX_49, //hp bar yellow [2 pixels]
    HEALTHBOX_GFX_50, //hp bar yellow [3 pixels]
    HEALTHBOX_GFX_51, //hp bar yellow [4 pixels]
    HEALTHBOX_GFX_52, //hp bar yellow [5 pixels]
    HEALTHBOX_GFX_53, //hp bar yellow [6 pixels]
    HEALTHBOX_GFX_54, //hp bar yellow [7 pixels]
    HEALTHBOX_GFX_55, //hp bar yellow [8 pixels]
    HEALTHBOX_GFX_HP_BAR_RED,  //hp bar red [0 pixels]
    HEALTHBOX_GFX_57, //hp bar red [1 pixels]
    HEALTHBOX_GFX_58, //hp bar red [2 pixels]
    HEALTHBOX_GFX_59, //hp bar red [3 pixels]
    HEALTHBOX_GFX_60, //hp bar red [4 pixels]
    HEALTHBOX_GFX_61, //hp bar red [5 pixels]
    HEALTHBOX_GFX_62, //hp bar red [6 pixels]
    HEALTHBOX_GFX_63, //hp bar red [7 pixels]
    HEALTHBOX_GFX_64, //hp bar red [8 pixels]
    HEALTHBOX_GFX_65, //hp bar frame end
    HEALTHBOX_GFX_66, //status ball [full]
    HEALTHBOX_GFX_67, //status ball [empty]
    HEALTHBOX_GFX_68, //status ball [fainted]
    HEALTHBOX_GFX_69, //status ball [statused]
    HEALTHBOX_GFX_70, //status ball [unused extra]
    HEALTHBOX_GFX_STATUS_PSN_BATTLER1, //status2 "PSN"
    HEALTHBOX_GFX_72,
    HEALTHBOX_GFX_73,
    HEALTHBOX_GFX_STATUS_PRZ_BATTLER1, //status2 "PRZ"
    HEALTHBOX_GFX_75,
    HEALTHBOX_GFX_76,
    HEALTHBOX_GFX_STATUS_SLP_BATTLER1, //status2 "SLP"
    HEALTHBOX_GFX_78,
    HEALTHBOX_GFX_79,
    HEALTHBOX_GFX_STATUS_FRZ_BATTLER1, //status2 "FRZ"
    HEALTHBOX_GFX_81,
    HEALTHBOX_GFX_82,
    HEALTHBOX_GFX_STATUS_BRN_BATTLER1, //status2 "BRN"
    HEALTHBOX_GFX_84,
    HEALTHBOX_GFX_85,
    HEALTHBOX_GFX_STATUS_PSN_BATTLER2, //status3 "PSN"
    HEALTHBOX_GFX_87,
    HEALTHBOX_GFX_88,
    HEALTHBOX_GFX_STATUS_PRZ_BATTLER2, //status3 "PRZ"
    HEALTHBOX_GFX_90,
    HEALTHBOX_GFX_91,
    HEALTHBOX_GFX_STATUS_SLP_BATTLER2, //status3 "SLP"
    HEALTHBOX_GFX_93,
    HEALTHBOX_GFX_94,
    HEALTHBOX_GFX_STATUS_FRZ_BATTLER2, //status3 "FRZ"
    HEALTHBOX_GFX_96,
    HEALTHBOX_GFX_97,
    HEALTHBOX_GFX_STATUS_BRN_BATTLER2, //status3 "BRN"
    HEALTHBOX_GFX_99,
    HEALTHBOX_GFX_100,
    HEALTHBOX_GFX_STATUS_PSN_BATTLER3, //status4 "PSN"
    HEALTHBOX_GFX_102,
    HEALTHBOX_GFX_103,
    HEALTHBOX_GFX_STATUS_PRZ_BATTLER3, //status4 "PRZ"
    HEALTHBOX_GFX_105,
    HEALTHBOX_GFX_106,
    HEALTHBOX_GFX_STATUS_SLP_BATTLER3, //status4 "SLP"
    HEALTHBOX_GFX_108,
    HEALTHBOX_GFX_109,
    HEALTHBOX_GFX_STATUS_FRZ_BATTLER3, //status4 "FRZ"
    HEALTHBOX_GFX_111,
    HEALTHBOX_GFX_112,
    HEALTHBOX_GFX_STATUS_BRN_BATTLER3, //status4 "BRN"
    HEALTHBOX_GFX_114,
    HEALTHBOX_GFX_115,
    HEALTHBOX_GFX_116, //unknown_D12FEC
    HEALTHBOX_GFX_117, //unknown_D1300C
};

static void SpriteCB_HealthBoxOther(struct Sprite * sprite);
static void SpriteCB_HealthBar(struct Sprite * sprite);
static const u8 *GetHealthboxElementGfxPtr(u8 which);
static void UpdateHpTextInHealthboxInDoubles(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent);
static void sub_8049388(u8 taskId);
static void sub_80493E4(u8 taskId);
static void sub_8049568(struct Sprite * sprite);
static void sub_8049630(struct Sprite * sprite);
static void sub_804948C(u8 taskId);
static void SpriteCB_StatusSummaryBallsOnSwitchout(struct Sprite * sprite);
static void UpdateStatusIconInHealthbox(u8 spriteId);
static void SpriteCB_StatusSummaryBar(struct Sprite * sprite);
static void SpriteCB_StatusSummaryBallsOnBattleStart(struct Sprite * sprite);
static u8 GetStatusIconForBattlerId(u8 statusElementId, u8 battlerId);
static void MoveBattleBarGraphically(u8 battlerId, u8 whichBar);
static u8 GetScaledExpFraction(s32 oldValue, s32 receivedValue, s32 maxValue, u8 scale);
static u8 CalcBarFilledPixels(s32 maxValue, s32 oldValue, s32 receivedValue, s32 *currValue, u8 *arg4, u8 scale);
static s32 CalcNewBarValue(s32 maxValue, s32 currValue, s32 receivedValue, s32 *arg3, u8 arg4, u16 arg5);
static void SafariTextIntoHealthboxObject(void *dest, u8 *windowTileData, u32 windowWidth);
static u8 *AddTextPrinterAndCreateWindowOnHealthbox(const u8 *str, u32 x, u32 y, u32 *windowId);
static void TextIntoHealthboxObject(void *dest, u8 *windowTileData, s32 windowWidth);
static void SpriteCB_HealthboxSlideInDelayed(struct Sprite *sprite);
static void SpriteCB_HealthboxSlideIn(struct Sprite *sprite);
static void SpriteCB_WeatherAnimIcon(struct Sprite *sprite);

static const u8 sWeatherIconRainGfx[] = INCBIN_U8("graphics/battle_interface/weather_icon_rain.4bpp");
static const u8 sWeatherIconSunGfx[] = INCBIN_U8("graphics/battle_interface/weather_icon_sun.4bpp");
static const u8 sWeatherIconHailGfx[] = INCBIN_U8("graphics/battle_interface/weather_icon_hail.4bpp");
static const u8 sWeatherIconSandstormGfx[] = INCBIN_U8("graphics/battle_interface/weather_icon_sandstorm.4bpp");
static const u8 sWeatherIconFogGfx[] = INCBIN_U8("graphics/battle_interface/weather_icon_fog.4bpp");
static const u8 sWeatherIconStrongWinds[] = INCBIN_U8("graphics/battle_interface/weather_icon_strong_winds.4bpp");

static const u16 sWeatherIconRainPal[] = INCBIN_U16("graphics/battle_interface/weather_icon_rain.gbapal");
static const u16 sWeatherIconSunPal[] = INCBIN_U16("graphics/battle_interface/weather_icon_sun.gbapal");
static const u16 sWeatherIconHailPal[] = INCBIN_U16("graphics/battle_interface/weather_icon_hail.gbapal");
static const u16 sWeatherIconSandstormPal[] = INCBIN_U16("graphics/battle_interface/weather_icon_sandstorm.gbapal");
static const u16 sWeatherIconFogPal[] = INCBIN_U16("graphics/battle_interface/weather_icon_fog.gbapal");

static const struct OamData gOamData_8260270 = {
    .shape = SPRITE_SHAPE(64x32),
    .size = SPRITE_SIZE(64x32),
    .priority = 1
};

static const struct SpriteTemplate sHealthboxPlayerSpriteTemplates[] = {
    {
        .tileTag = TAG_HEALTHBOX_PLAYER1_TILE,
        .paletteTag = TAG_HEALTHBOX_PAL,
        .oam = &gOamData_8260270,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy
    }, {
        .tileTag = TAG_HEALTHBOX_PLAYER2_TILE,
        .paletteTag = TAG_HEALTHBOX_PAL,
        .oam = &gOamData_8260270,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy
    }
};

static const struct SpriteTemplate sHealthboxOpponentSpriteTemplates[] = {
    {
        .tileTag = TAG_HEALTHBOX_OPPONENT1_TILE,
        .paletteTag = TAG_HEALTHBOX_PAL,
        .oam = &gOamData_8260270,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy
    }, {
        .tileTag = TAG_HEALTHBOX_OPPONENT2_TILE,
        .paletteTag = TAG_HEALTHBOX_PAL,
        .oam = &gOamData_8260270,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy
    }
};

static const struct SpriteTemplate sHealthboxSafariSpriteTemplate =
{
    .tileTag = TAG_HEALTHBOX_SAFARI_TILE,
    .paletteTag = TAG_HEALTHBOX_PAL,
    .oam = &gOamData_8260270,
    .anims = gDummySpriteAnimTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct OamData gUnknown_82602F0 = {
    .shape = SPRITE_SHAPE(32x8),
    .size = SPRITE_SIZE(32x8),
    .priority = 1
};

static const struct SpriteTemplate gUnknown_82602F8[] = {
    {
        .tileTag = TAG_HEALTHBAR_PLAYER1_TILE,
        .paletteTag = TAG_HEALTHBAR_PAL,
        .oam = &gUnknown_82602F0,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_HealthBar
    }, {
        .tileTag = TAG_HEALTHBAR_OPPONENT1_TILE,
        .paletteTag = TAG_HEALTHBAR_PAL,
        .oam = &gUnknown_82602F0,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_HealthBar
    }, {
        .tileTag = TAG_HEALTHBAR_PLAYER2_TILE,
        .paletteTag = TAG_HEALTHBAR_PAL,
        .oam = &gUnknown_82602F0,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_HealthBar
    }, {
        .tileTag = TAG_HEALTHBAR_OPPONENT2_TILE,
        .paletteTag = TAG_HEALTHBAR_PAL,
        .oam = &gUnknown_82602F0,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_HealthBar
    }
};

static const struct Subsprite gUnknown_8260390[] = {
    { 240, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0000, 1 },
    { 16, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0004, 1 }
};

static const struct Subsprite gUnknown_8260398[] = {
    { 240, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0000, 1 },
    { 16, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0004, 1 },
    { 224, 0, SPRITE_SHAPE(8x8), SPRITE_SIZE(8x8), 0x0008, 1 }
};

static const struct SubspriteTable gUnknown_82603C4[] = {
    {NELEMS(gUnknown_8260390), gUnknown_8260390},
    {NELEMS(gUnknown_8260398), gUnknown_8260398}
};

static const struct Subsprite gUnknown_82603D4[] = {
    { 160, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0000, 1 },
    { 192, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0004, 1 },
    { 224, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0008, 1 },
    { 0, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x000c, 1 }
};

static const struct Subsprite gUnknown_82603E4[] = {
    { 160, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0000, 1 },
    { 192, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0004, 1 },
    { 224, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0008, 1 },
    { 0, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0008, 1 },
    { 32, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x0008, 1 },
    { 64, 0, SPRITE_SHAPE(32x8), SPRITE_SIZE(32x8), 0x000c, 1 }
};

static const struct SubspriteTable sStatusSummaryBar_SubspriteTable[] =
{
    {NELEMS(gUnknown_82603D4), gUnknown_82603D4}
};

static const struct SubspriteTable gUnknown_8260404[] = {
    {NELEMS(gUnknown_82603E4), gUnknown_82603E4}
};

static const struct CompressedSpriteSheet sStatusSummaryBarSpriteSheets[] = {
    {gFile_graphics_battle_interface_ball_status_bar_sheet, 0x0200, TAG_PARTY_SUMMARY_BAR_PLAYER_TILE},
    {gFile_graphics_battle_interface_ball_status_bar_sheet, 0x0200, TAG_PARTY_SUMMARY_BAR_OPPONENT_TILE}
};

static const struct SpritePalette sStatusSummaryBarSpritePals[] = {
    {gBattleInterface_BallStatusBarPal, TAG_PARTY_SUMMARY_BAR_PLAYER_PAL},
    {gBattleInterface_BallStatusBarPal, TAG_PARTY_SUMMARY_BAR_OPPONENT_PAL}
};

static const struct SpritePalette sStatusSummaryBallsSpritePals[] = {
    {gBattleInterface_BallDisplayPal, TAG_PARTY_SUMMARY_BALL_PLAYER_PAL},
    {gBattleInterface_BallDisplayPal, TAG_PARTY_SUMMARY_BALL_OPPONENT_PAL}
};

static const struct SpriteSheet sStatusSummaryBallsSpriteSheets[] = {
    {gBattleInterface_SummaryBallDisplayGfx, 0x0080, TAG_PARTY_SUMMARY_BALL_PLAYER_TILE},
    {gBattleInterface_SummaryBallDisplayGfx, 0x0080, TAG_PARTY_SUMMARY_BALL_OPPONENT_TILE}
};

static const struct OamData gUnknown_82604B4 = {
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 1
};

static const struct SpriteTemplate sStatusSummaryBarSpriteTemplates[] = {
    {
        .tileTag = TAG_PARTY_SUMMARY_BAR_PLAYER_TILE,
        .paletteTag = TAG_PARTY_SUMMARY_BAR_PLAYER_PAL,
        .oam = &gOamData_8260270,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_StatusSummaryBar
    }, {
        .tileTag = TAG_PARTY_SUMMARY_BAR_OPPONENT_TILE,
        .paletteTag = TAG_PARTY_SUMMARY_BAR_OPPONENT_PAL,
        .oam = &gOamData_8260270,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_StatusSummaryBar
    }
};

static const struct SpriteTemplate sStatusSummaryBallsSpriteTemplates[] = {
    {
        .tileTag = TAG_PARTY_SUMMARY_BALL_PLAYER_TILE,
        .paletteTag = TAG_PARTY_SUMMARY_BALL_PLAYER_PAL,
        .oam = &gUnknown_82604B4,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_StatusSummaryBallsOnBattleStart
    }, {
        .tileTag = TAG_PARTY_SUMMARY_BALL_OPPONENT_TILE,
        .paletteTag = TAG_PARTY_SUMMARY_BALL_OPPONENT_PAL,
        .oam = &gUnknown_82604B4,
        .anims = gDummySpriteAnimTable,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_StatusSummaryBallsOnBattleStart
    }
};

static const struct SpriteSheet sSpriteSheet_WeatherStatusIcons[] =
{
	[ENUM_WEATHER_RAIN - 1] = {sWeatherIconRainGfx, sizeof(sWeatherIconRainGfx), TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_SUN - 1] = {sWeatherIconSunGfx, sizeof(sWeatherIconSunGfx), TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_SANDSTORM - 1] = {sWeatherIconSandstormGfx, sizeof(sWeatherIconSandstormGfx), TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_HAIL - 1] = {sWeatherIconHailGfx, sizeof(sWeatherIconHailGfx), TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_FOG - 1] = {sWeatherIconFogGfx, sizeof(sWeatherIconFogGfx), TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_STRONG_WINDS - 1] = {sWeatherIconStrongWinds, sizeof(sWeatherIconStrongWinds), TAG_WEATHER_ICON_GFX}
};

static const struct SpritePalette sSpritePalette_WeatherStatusIcons[] =
{
	[ENUM_WEATHER_RAIN - 1] = {sWeatherIconRainPal, TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_SUN - 1] = {sWeatherIconSunPal, TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_SANDSTORM - 1] = {sWeatherIconSandstormPal, TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_HAIL - 1] = {sWeatherIconHailPal, TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_FOG - 1] = {sWeatherIconFogPal, TAG_WEATHER_ICON_GFX},
	[ENUM_WEATHER_STRONG_WINDS - 1] = {sWeatherIconFogPal, TAG_WEATHER_ICON_GFX}, // same palette as fog
};

static const union AnimCmd sAnim_WeatherIcon[] =
{
    ANIMCMD_FRAME(0, 40),
	ANIMCMD_FRAME(16, 40),
	ANIMCMD_FRAME(32, 40),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd * const sAnims_WeatherIcon[] =
{
	sAnim_WeatherIcon,
};

static const struct OamData sOam_WeatherIcon =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .size = SPRITE_SIZE(32x16),
};

static const struct SpriteTemplate sSpriteTemplate_WeatherIcon =
{
	.tileTag = TAG_WEATHER_ICON_GFX,
    .paletteTag = TAG_WEATHER_ICON_GFX,
    .oam = &sOam_WeatherIcon,
    .anims = sAnims_WeatherIcon,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_WeatherAnimIcon
};

enum
{
    HEALTHBAR_TYPE_PLAYER_SINGLE,
    HEALTHBAR_TYPE_PLAYER_DOUBLE,
    HEALTHBAR_TYPE_OPPONENT,
};

// Because the healthbox is too large to fit into one sprite, it is divided into two sprites.
// healthboxLeft  or healthboxMain  is the left part that is used as the 'main' sprite.
// healthboxRight or healthboxOther is the right part of the healthbox.
// There's also the third sprite under name of healthbarSprite that refers to the healthbar visible on the healtbox.

u8 CreateBattlerHealthboxSprites(u8 a)
{
    u8 healthboxLeftSpriteId, healthboxRightSpriteId, healthbarSpriteId, healthbarType = HEALTHBAR_TYPE_PLAYER_SINGLE;
    struct Sprite *sprite;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        if (GetBattlerSide(a) == B_SIDE_PLAYER)
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxPlayerSpriteTemplates[0], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxPlayerSpriteTemplates[0], 240, 160, 1);

            gSprites[healthboxLeftSpriteId].oam.shape = SPRITE_SHAPE(64x64);
            gSprites[healthboxRightSpriteId].oam.shape = SPRITE_SHAPE(64x64);
            gSprites[healthboxRightSpriteId].oam.tileNum += 64;
        }
        else
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxOpponentSpriteTemplates[0], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxOpponentSpriteTemplates[0], 240, 160, 1);

            gSprites[healthboxRightSpriteId].oam.tileNum += 32;
            healthbarType = HEALTHBAR_TYPE_OPPONENT;
        }
        gSprites[healthboxLeftSpriteId].hMain_HealthBoxOtherSpriteId = healthboxRightSpriteId;
        gSprites[healthboxRightSpriteId].hOther_HealthBoxSpriteId = healthboxLeftSpriteId;
        gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
    }
    else
    {
        if (GetBattlerSide(a) == B_SIDE_PLAYER)
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxPlayerSpriteTemplates[GetBattlerPosition(a) / 2], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxPlayerSpriteTemplates[GetBattlerPosition(a) / 2], 240, 160, 1);
            healthbarType = HEALTHBAR_TYPE_PLAYER_DOUBLE;
        }
        else
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxOpponentSpriteTemplates[GetBattlerPosition(a) / 2], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxOpponentSpriteTemplates[GetBattlerPosition(a) / 2], 240, 160, 1);
            healthbarType = HEALTHBAR_TYPE_OPPONENT;
        }
		gSprites[healthboxLeftSpriteId].hMain_HealthBoxOtherSpriteId = healthboxRightSpriteId;
		gSprites[healthboxRightSpriteId].hOther_HealthBoxSpriteId = healthboxLeftSpriteId;
		gSprites[healthboxRightSpriteId].oam.tileNum += 32;
		gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
    }
    healthbarSpriteId = CreateSpriteAtEnd(&gUnknown_82602F8[gBattlerPositions[a]], 140, 60, 0);
    sprite = &gSprites[healthbarSpriteId];
    SetSubspriteTables(sprite, &gUnknown_82603C4[GetBattlerSide(a)]);
    sprite->subspriteMode = SUBSPRITES_IGNORE_PRIORITY;
    sprite->oam.priority = 1;
    CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_1), OBJ_VRAM0 + sprite->oam.tileNum * 32, 64);

    gSprites[healthboxLeftSpriteId].hMain_HealthBarSpriteId = healthbarSpriteId;
    gSprites[healthboxLeftSpriteId].hMain_Battler = a;
    gSprites[healthboxLeftSpriteId].invisible = TRUE;
    gSprites[healthboxRightSpriteId].invisible = TRUE;
    sprite->hBar_HealthBoxSpriteId = healthboxLeftSpriteId;
    sprite->hBar_Type = healthbarType;
    sprite->invisible = TRUE;

    return healthboxLeftSpriteId;
}

u8 CreateSafariPlayerHealthboxSprites(void)
{
    u8 healthboxLeftSpriteId = CreateSprite(&sHealthboxSafariSpriteTemplate, 240, 160, 1);
    u8 healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxSafariSpriteTemplate, 240, 160, 1);

    gSprites[healthboxLeftSpriteId].oam.shape = SPRITE_SHAPE(64x64);
    gSprites[healthboxRightSpriteId].oam.shape = SPRITE_SHAPE(64x64);
    gSprites[healthboxRightSpriteId].oam.tileNum += 0x40;
    gSprites[healthboxLeftSpriteId].hMain_HealthBoxOtherSpriteId = healthboxRightSpriteId;
    gSprites[healthboxRightSpriteId].hOther_HealthBoxSpriteId = healthboxLeftSpriteId;
    gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
    return healthboxLeftSpriteId;
}

static const u8 *GetHealthboxElementGfxPtr(u8 elementId)
{
    return gHealthboxElementsGfxTable[elementId];
}

#define HEALTHBOX_SLIDE_SPEED 5

void StartHealthboxSlideIn(u8 battlerId)
{
    struct Sprite *healthboxSprite = &gSprites[gHealthboxSpriteIds[battlerId]];
	
	healthboxSprite->x2 = 0x73;
	healthboxSprite->y2 = 0;
	healthboxSprite->callback = SpriteCB_HealthboxSlideIn;
	
	if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
	{
		if (GetBattlerPosition(battlerId) == B_POSITION_PLAYER_RIGHT)
		{
			healthboxSprite->hMain_HealthBoxSlideSpeedX = 0; // Temporary used as a delay counter
			healthboxSprite->callback = SpriteCB_HealthboxSlideInDelayed;
		}
		else
			healthboxSprite->hMain_HealthBoxSlideSpeedX = HEALTHBOX_SLIDE_SPEED;
	}
	else
	{
		healthboxSprite->hMain_HealthBoxSlideSpeedX = -HEALTHBOX_SLIDE_SPEED;
		healthboxSprite->x2 = -healthboxSprite->x2;
		healthboxSprite->y2 = -healthboxSprite->y2;
	}
	// Update healthbar coords
	gSprites[healthboxSprite->hMain_HealthBarSpriteId].callback(&gSprites[healthboxSprite->hMain_HealthBarSpriteId]);
}

static void SpriteCB_HealthboxSlideInDelayed(struct Sprite *sprite)
{
    if (++sprite->hMain_HealthBoxSlideSpeedX == 20)
    {
        sprite->hMain_HealthBoxSlideSpeedX = HEALTHBOX_SLIDE_SPEED;
        sprite->callback = SpriteCB_HealthboxSlideIn;
    }
}

static void SpriteCB_HealthboxSlideIn(struct Sprite *sprite)
{
    sprite->x2 -= sprite->hMain_HealthBoxSlideSpeedX;
	
    if (sprite->x2 == 0)
        sprite->callback = SpriteCallbackDummy;
}

// Syncs the position of healthbar accordingly with the healthbox.
static void SpriteCB_HealthBar(struct Sprite *sprite)
{
    u8 healthboxLeftSpriteId = sprite->hBar_HealthBoxSpriteId;

    switch (sprite->hBar_Type)
    {
		case HEALTHBAR_TYPE_OPPONENT:
			sprite->x = gSprites[healthboxLeftSpriteId].x + 8;
			break;
		default:
			sprite->x = gSprites[healthboxLeftSpriteId].x + 16;
			break;
    }
	sprite->y = gSprites[healthboxLeftSpriteId].y;
    sprite->x2 = gSprites[healthboxLeftSpriteId].x2;
    sprite->y2 = gSprites[healthboxLeftSpriteId].y2;
}

static void SpriteCB_HealthBoxOther(struct Sprite *sprite)
{
    u8 healthboxMainSpriteId = sprite->hOther_HealthBoxSpriteId;

    sprite->x = gSprites[healthboxMainSpriteId].x + 64;
    sprite->y = gSprites[healthboxMainSpriteId].y;
    sprite->x2 = gSprites[healthboxMainSpriteId].x2;
    sprite->y2 = gSprites[healthboxMainSpriteId].y2;
}

void SetBattleBarStruct(u8 battlerId, s32 maxVal, s32 oldVal, s32 receivedValue)
{
    gBattleSpritesDataPtr->battleBars[battlerId].healthboxSpriteId = gHealthboxSpriteIds[battlerId];
    gBattleSpritesDataPtr->battleBars[battlerId].maxValue = maxVal;
    gBattleSpritesDataPtr->battleBars[battlerId].oldValue = oldVal;
    gBattleSpritesDataPtr->battleBars[battlerId].receivedValue = receivedValue;
    gBattleSpritesDataPtr->battleBars[battlerId].currValue = -32768;
}

void SetHealthboxSpriteInvisible(u8 healthboxSpriteId)
{
    gSprites[healthboxSpriteId].invisible = TRUE;
    gSprites[gSprites[healthboxSpriteId].hMain_HealthBarSpriteId].invisible = TRUE;
    gSprites[gSprites[healthboxSpriteId].hMain_HealthBoxOtherSpriteId].invisible = TRUE;
}

void SetHealthboxSpriteVisible(u8 healthboxSpriteId)
{
    gSprites[healthboxSpriteId].invisible = FALSE;
    gSprites[gSprites[healthboxSpriteId].hMain_HealthBarSpriteId].invisible = FALSE;
    gSprites[gSprites[healthboxSpriteId].hMain_HealthBoxOtherSpriteId].invisible = FALSE;
}

void DestoryHealthboxSprite(u8 healthboxSpriteId)
{
    DestroySprite(&gSprites[gSprites[healthboxSpriteId].hMain_HealthBoxOtherSpriteId]);
    DestroySprite(&gSprites[gSprites[healthboxSpriteId].hMain_HealthBarSpriteId]);
    DestroySprite(&gSprites[healthboxSpriteId]);
}

void UpdateOamPriorityInAllHealthboxes(u8 priority, bool8 hideHpBoxes)
{
    s32 i;

    for (i = 0; i < gBattlersCount; i++)
    {
        u8 healthboxLeftSpriteId = gHealthboxSpriteIds[i];
		
        gSprites[healthboxLeftSpriteId].oam.priority = priority;
        gSprites[gSprites[healthboxLeftSpriteId].hMain_HealthBoxOtherSpriteId].oam.priority = priority;
        gSprites[gSprites[healthboxLeftSpriteId].hMain_HealthBarSpriteId].oam.priority = priority;

		if (IsBattlerAlive(i))
		{
			if (hideHpBoxes)
				SetHealthboxSpriteInvisible(healthboxLeftSpriteId);
			else
				SetHealthboxSpriteVisible(healthboxLeftSpriteId);
		}
    }
}

void InitBattlerHealthboxCoords(u8 battler)
{
    s16 x = 0, y = 0;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        if (GetBattlerSide(battler) != B_SIDE_PLAYER)
            x = 44, y = 30;
        else
            x = 158, y = 88;
    }
    else
    {
        switch (GetBattlerPosition(battler))
        {
        case B_POSITION_PLAYER_LEFT:
            x = 159, y = 75;
            break;
        case B_POSITION_PLAYER_RIGHT:
            x = 171, y = 100;
            break;
        case B_POSITION_OPPONENT_LEFT:
            x = 44, y = 19;
            break;
        case B_POSITION_OPPONENT_RIGHT:
            x = 32, y = 44;
            break;
        }
    }
	gSprites[gHealthboxSpriteIds[battler]].x = x;
    gSprites[gHealthboxSpriteIds[battler]].y = y;
}

static void UpdateLvlInHealthbox(u8 healthboxSpriteId, u8 lvl)
{
    u32 windowId, spriteTileNum, xPos;
	u8 battlerId = gSprites[healthboxSpriteId].hMain_Battler, text[16] = _("{LV_2}");
    u8 *windowTileData, *objVram = ConvertIntToDecimalStringN(text + 2, lvl, STR_CONV_MODE_LEFT_ALIGN, 3);

    xPos = 5 * (3 - (objVram - (text + 2)));

    windowTileData = AddTextPrinterAndCreateWindowOnHealthbox(text, xPos, 3, &windowId);
    spriteTileNum = gSprites[healthboxSpriteId].oam.tileNum * TILE_SIZE_4BPP;

    if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
    {
        objVram = (void*)(OBJ_VRAM0);
        if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            objVram += spriteTileNum + 0x820;
        else
            objVram += spriteTileNum + 0x420;
    }
    else
    {
        objVram = (void*)(OBJ_VRAM0);
        objVram += spriteTileNum + 0x400;
    }
    TextIntoHealthboxObject(objVram, windowTileData, 3);
    RemoveWindow(windowId);
}

void UpdateHpTextInHealthbox(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent)
{
    u32 windowId, spriteTileNum;
    u8 *windowTileData;
    u8 *strptr;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        u8 text[8];
		
        if (maxOrCurrent != HP_CURRENT) // singles, max
        {
            ConvertIntToDecimalStringN(text, value, STR_CONV_MODE_RIGHT_ALIGN, 3);
            windowTileData = AddTextPrinterAndCreateWindowOnHealthbox(text, 0, 5, &windowId);
            spriteTileNum = gSprites[healthboxSpriteId].oam.tileNum;
            TextIntoHealthboxObject( (void*)(OBJ_VRAM0) + spriteTileNum * TILE_SIZE_4BPP + 0xA40, windowTileData, 2);
            RemoveWindow(windowId);
        }
        else // singles, current
        {
            strptr = ConvertIntToDecimalStringN(text, value, STR_CONV_MODE_RIGHT_ALIGN, 3);
            *strptr++ = CHAR_SLASH;
            *strptr++ = EOS;
            windowTileData = AddTextPrinterAndCreateWindowOnHealthbox(text, 4, 5, &windowId);
            spriteTileNum = gSprites[healthboxSpriteId].oam.tileNum;
            TextIntoHealthboxObject((void *)(OBJ_VRAM0) + spriteTileNum * TILE_SIZE_4BPP + 0x2E0, windowTileData, 1);
            TextIntoHealthboxObject((void *)(OBJ_VRAM0) + spriteTileNum * TILE_SIZE_4BPP + 0xA00, windowTileData + 0x20, 2);
            RemoveWindow(windowId);
        }
    }
    else
		UpdateHpTextInHealthboxInDoubles(healthboxSpriteId, value, maxOrCurrent);
}

static void UpdateHpTextInHealthboxInDoubles(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent)
{
    if (gBattleSpritesDataPtr->battlerData[gSprites[healthboxSpriteId].hMain_Battler].hpNumbersNoBars) // don't print text if only bars are visible
    {
		u8 text[20] = __("{COLOR 01}{HIGHLIGHT 00}");
        u8 var = maxOrCurrent == HP_CURRENT ? 0 : 4;
        u8 healthbarSpriteId = gSprites[healthboxSpriteId].hMain_HealthBarSpriteId;
        u8 *txtPtr = ConvertIntToDecimalStringN(text + 6, value, STR_CONV_MODE_RIGHT_ALIGN, 3);
        u8 i;

        if (!maxOrCurrent)
            StringCopy(txtPtr, gText_Slash);
        RenderTextFont9(gMonSpritesGfxPtr->barFontGfx, 0, text, 0, 0, 0, 0, 0);

        for (i = var; i < var + 3; i++)
        {
            if (i < 3)
            {
                CpuCopy32(&gMonSpritesGfxPtr->barFontGfx[((i - var) * 64) + 32],
                          (void*)((OBJ_VRAM0) + 32 * (1 + gSprites[healthbarSpriteId].oam.tileNum + i)),
                          0x20);
            }
            else
            {
                CpuCopy32(&gMonSpritesGfxPtr->barFontGfx[((i - var) * 64) + 32],
                          (void*)((OBJ_VRAM0 + 0x20) + 32 * (i + gSprites[healthbarSpriteId].oam.tileNum)),
                          0x20);
            }
        }
        if (maxOrCurrent == HP_CURRENT)
        {
            CpuCopy32(&gMonSpritesGfxPtr->barFontGfx[224], (void*)((OBJ_VRAM0) + ((gSprites[healthbarSpriteId].oam.tileNum + 4) * TILE_SIZE_4BPP)), 0x20);
            CpuFill32(0, (void*)((OBJ_VRAM0) + (gSprites[healthbarSpriteId].oam.tileNum * TILE_SIZE_4BPP)), 0x20);
        }
        else
			CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_116), (void*)(OBJ_VRAM0) + ((gSprites[healthboxSpriteId].oam.tileNum + 52) * TILE_SIZE_4BPP), 0x20);
    }
}

void SwapHpBarsWithHpText(void)
{
    s32 i;
    u8 healthboxSpriteId;
	struct Pokemon *mon;
	struct Sprite *healthboxSprite;
	
	if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE) && !(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
	{
		for (i = 0; i < gBattlersCount; i++)
		{
			healthboxSpriteId = gHealthboxSpriteIds[i];
			healthboxSprite = &gSprites[healthboxSpriteId];
			
			if (healthboxSprite->callback == SpriteCallbackDummy && GetBattlerSide(i) == B_SIDE_PLAYER)
			{
				mon = &gPlayerParty[gBattlerPartyIndexes[i]];
				
				gBattleSpritesDataPtr->battlerData[i].hpNumbersNoBars ^= TRUE;
				
				if (gBattleSpritesDataPtr->battlerData[i].hpNumbersNoBars) // bars to text
				{
					CpuFill32(0, (void*)(OBJ_VRAM0 + gSprites[healthboxSprite->hMain_HealthBarSpriteId].oam.tileNum * TILE_SIZE_4BPP), 0x100);
                    UpdateHpTextInHealthboxInDoubles(healthboxSpriteId, GetMonData(mon, MON_DATA_HP), HP_CURRENT);
                    UpdateHpTextInHealthboxInDoubles(healthboxSpriteId, GetMonData(mon, MON_DATA_MAX_HP), HP_MAX);
				}
				else // text to bars
				{
					UpdateStatusIconInHealthbox(healthboxSpriteId);
                    UpdateHealthboxAttribute(i, HEALTHBOX_HEALTH_BAR);
                    CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_117), (void*)(OBJ_VRAM0 + 0x680 + healthboxSprite->oam.tileNum * TILE_SIZE_4BPP), 32);
				}
			}
		}
	}
}

#define tBattler                data[0]
#define tSummaryBarSpriteId     data[1]
#define tBallIconSpriteId(n)    data[3 + n]
#define tIsBattleStart          data[10]
#define tData15                 data[15]

u8 CreatePartyStatusSummarySprites(u8 battlerId, struct HpAndStatus *partyInfo, bool8 isSwitchingMons, bool8 isBattleStart)
{
    bool8 isOpponent;
    s8 nValidMons;
    s16 bar_X, bar_Y, bar_pos2_X, bar_data0;
    s32 i;
    u8 summaryBarSpriteId;
    u8 ballIconSpritesIds[PARTY_SIZE];
    u8 taskId;

    if (!isSwitchingMons || GetBattlerPosition(battlerId) != B_POSITION_OPPONENT_RIGHT)
    {
        if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
        {
            isOpponent = FALSE;
            bar_X = 136, bar_Y = 96;
            bar_pos2_X = 100;
            bar_data0 = -5;
        }
        else
        {
            isOpponent = TRUE;

            if (!isSwitchingMons || !(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
                bar_X = 104, bar_Y = 40;
            else
                bar_X = 104, bar_Y = 16;

            bar_pos2_X = -100;
            bar_data0 = 5;
        }
    }
    else
    {
        isOpponent = TRUE;
        bar_X = 104, bar_Y = 40;
        bar_pos2_X = -100;
        bar_data0 = 5;
    }

    for (i = 0, nValidMons = 0; i < PARTY_SIZE; i++)
    {
        if (partyInfo[i].hp != 0xFFFF)
            nValidMons++;
    }

    LoadCompressedSpriteSheetUsingHeap(&sStatusSummaryBarSpriteSheets[isOpponent]);
    LoadSpriteSheet(&sStatusSummaryBallsSpriteSheets[isOpponent]);
    LoadSpritePalette(&sStatusSummaryBarSpritePals[isOpponent]);
    LoadSpritePalette(&sStatusSummaryBallsSpritePals[isOpponent]);

    summaryBarSpriteId = CreateSprite(&sStatusSummaryBarSpriteTemplates[isOpponent], bar_X, bar_Y, 10);
    SetSubspriteTables(&gSprites[summaryBarSpriteId], sStatusSummaryBar_SubspriteTable);
    gSprites[summaryBarSpriteId].x2 = bar_pos2_X;
    gSprites[summaryBarSpriteId].data[0] = bar_data0;

    if (isOpponent)
    {
        gSprites[summaryBarSpriteId].x -= 96;
        gSprites[summaryBarSpriteId].oam.matrixNum = ST_OAM_HFLIP;
    }
    else
    {
        gSprites[summaryBarSpriteId].x += 96;
    }

    for (i = 0; i < PARTY_SIZE; i++)
    {
        ballIconSpritesIds[i] = CreateSpriteAtEnd(&sStatusSummaryBallsSpriteTemplates[isOpponent], bar_X, bar_Y - 4, 9);

        if (!isBattleStart)
            gSprites[ballIconSpritesIds[i]].callback = SpriteCB_StatusSummaryBallsOnSwitchout;

        if (!isOpponent)
        {
            gSprites[ballIconSpritesIds[i]].x2 = 0;
            gSprites[ballIconSpritesIds[i]].y2 = 0;
        }

        gSprites[ballIconSpritesIds[i]].data[0] = summaryBarSpriteId;

        if (!isOpponent)
        {
            gSprites[ballIconSpritesIds[i]].x += 10 * i + 24;
            gSprites[ballIconSpritesIds[i]].data[1] = i * 7 + 10;
            gSprites[ballIconSpritesIds[i]].x2 = 120;
        }
        else
        {
            gSprites[ballIconSpritesIds[i]].x -= 10 * (5 - i) + 24;
            gSprites[ballIconSpritesIds[i]].data[1] = (6 - i) * 7 + 10;
            gSprites[ballIconSpritesIds[i]].x2 = -120;
        }

        gSprites[ballIconSpritesIds[i]].data[2] = isOpponent;
    }

    if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
    {
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                if (partyInfo[i].hp == 0xFFFF) // empty slot or an egg
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 1;
                    gSprites[ballIconSpritesIds[i]].data[7] = 1;
                }
                else if (partyInfo[i].hp == 0) // fainted mon
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 3;
                }
                else if (partyInfo[i].status != 0) // mon with major status
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 2;
                }
            }
            else
            {
                if (i >= nValidMons) // empty slot or an egg
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 1;
                    gSprites[ballIconSpritesIds[i]].data[7] = 1;
                }
                else if (partyInfo[i].hp == 0) // fainted mon
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 3;
                }
                else if (partyInfo[i].status != 0) // mon with major status
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 2;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                if (partyInfo[i].hp == 0xFFFF) // empty slot or an egg
                {
                    gSprites[ballIconSpritesIds[5 - i]].oam.tileNum += 1;
                    gSprites[ballIconSpritesIds[5 - i]].data[7] = 1;
                }
                else if (partyInfo[i].hp == 0) // fainted mon
                {
                    gSprites[ballIconSpritesIds[5 - i]].oam.tileNum += 3;
                }
                else if (partyInfo[i].status != 0) // mon with major status
                {
                    gSprites[ballIconSpritesIds[5 - i]].oam.tileNum += 2;
                }
            }
            else
            {
                ballIconSpritesIds[5 - i] += 0;
                if (i >= nValidMons) // empty slot or an egg
                {
                    gSprites[ballIconSpritesIds[5 - i]].oam.tileNum += 1;
                    gSprites[ballIconSpritesIds[5 - i]].data[7] = 1;
                }
                else if (partyInfo[i].hp == 0) // fainted mon
                {
                    gSprites[ballIconSpritesIds[5 - i]].oam.tileNum += 3;
                }
                else if (partyInfo[i].status != 0) // mon with major status
                {
                    do
                    {
                        gSprites[ballIconSpritesIds[5 - i]].oam.tileNum += 2;
                    } while (0);
                }
            }
            ballIconSpritesIds[5 - ++i] += 0; i--;
        }
    }

    taskId = CreateTask(TaskDummy, 5);
    gTasks[taskId].tBattler = battlerId;
    gTasks[taskId].tSummaryBarSpriteId = summaryBarSpriteId;

    for (i = 0; i < PARTY_SIZE; i++)
        gTasks[taskId].tBallIconSpriteId(i) = ballIconSpritesIds[i];

    gTasks[taskId].tIsBattleStart = isBattleStart;
    PlaySE12WithPanning(SE_BALL_TRAY_ENTER, 0);
    return taskId;
}

void Task_HidePartyStatusSummary(u8 taskId)
{
    u8 ballIconSpriteIds[PARTY_SIZE];
    bool8 isBattleStart;
    u8 summaryBarSpriteId;
    u8 battlerId;
    s32 i;

    isBattleStart = gTasks[taskId].tIsBattleStart;
    summaryBarSpriteId = gTasks[taskId].tSummaryBarSpriteId;
    battlerId = gTasks[taskId].tBattler;

    for (i = 0; i < PARTY_SIZE; i++)
        ballIconSpriteIds[i] = gTasks[taskId].tBallIconSpriteId(i);

    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT2_ALL | BLDCNT_EFFECT_BLEND);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));

    gTasks[taskId].tData15 = 16;

    for (i = 0; i < PARTY_SIZE; i++)
        gSprites[ballIconSpriteIds[i]].oam.objMode = ST_OAM_OBJ_BLEND;

    gSprites[summaryBarSpriteId].oam.objMode = ST_OAM_OBJ_BLEND;

    if (isBattleStart)
    {
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (GetBattlerSide(battlerId) != B_SIDE_PLAYER)
            {
                gSprites[ballIconSpriteIds[5 - i]].data[1] = 7 * i;
                gSprites[ballIconSpriteIds[5 - i]].data[3] = 0;
                gSprites[ballIconSpriteIds[5 - i]].data[4] = 0;
                gSprites[ballIconSpriteIds[5 - i]].callback = sub_8049630;
            }
            else
            {
                gSprites[ballIconSpriteIds[i]].data[1] = 7 * i;
                gSprites[ballIconSpriteIds[i]].data[3] = 0;
                gSprites[ballIconSpriteIds[i]].data[4] = 0;
                gSprites[ballIconSpriteIds[i]].callback = sub_8049630;
            }
        }
        gSprites[summaryBarSpriteId].data[0] /= 2;
        gSprites[summaryBarSpriteId].data[1] = 0;
        gSprites[summaryBarSpriteId].callback = sub_8049568;
        SetSubspriteTables(&gSprites[summaryBarSpriteId], gUnknown_8260404);
        gTasks[taskId].func = sub_8049388;
    }
    else
    {
        gTasks[taskId].func = sub_804948C;
    }
}

static void sub_8049388(u8 taskId)
{
    if ((gTasks[taskId].data[11]++ % 2) == 0)
    {
        if (--gTasks[taskId].tData15 < 0)
            return;

        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(gTasks[taskId].data[15], 16 - gTasks[taskId].data[15]));
    }
    if (gTasks[taskId].tData15 == 0)
        gTasks[taskId].func = sub_80493E4;
}

static void sub_80493E4(u8 taskId)
{
    u8 ballIconSpriteIds[PARTY_SIZE];
    s32 i;

    u8 battlerId = gTasks[taskId].tBattler;
    if (--gTasks[taskId].tData15 == -1)
    {
        u8 summaryBarSpriteId = gTasks[taskId].tSummaryBarSpriteId;

        for (i = 0; i < PARTY_SIZE; i++)
            ballIconSpriteIds[i] = gTasks[taskId].tBallIconSpriteId(i);

        DestroySpriteAndFreeResources(&gSprites[summaryBarSpriteId]);
        DestroySpriteAndFreeResources(&gSprites[ballIconSpriteIds[0]]);

        for (i = 1; i < PARTY_SIZE; i++)
            DestroySprite(&gSprites[ballIconSpriteIds[i]]);
    }
    else if (gTasks[taskId].tData15 == -3)
    {
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        DestroyTask(taskId);
    }
}

static void sub_804948C(u8 taskId)
{
    u8 ballIconSpriteIds[PARTY_SIZE];
    s32 i;
    u8 battlerId = gTasks[taskId].tBattler;

    if (--gTasks[taskId].tData15 >= 0)
    {
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(gTasks[taskId].data[15], 16 - gTasks[taskId].data[15]));
    }
    else if (gTasks[taskId].tData15 == -1)
    {
        u8 summaryBarSpriteId = gTasks[taskId].tSummaryBarSpriteId;

        for (i = 0; i < PARTY_SIZE; i++)
            ballIconSpriteIds[i] = gTasks[taskId].tBallIconSpriteId(i);

        DestroySpriteAndFreeResources(&gSprites[summaryBarSpriteId]);
        DestroySpriteAndFreeResources(&gSprites[ballIconSpriteIds[0]]);

        for (i = 1; i < PARTY_SIZE; i++)
            DestroySprite(&gSprites[ballIconSpriteIds[i]]);
    }
    else if (gTasks[taskId].tData15 == -3)
    {
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        DestroyTask(taskId);
    }
}

#undef tBattler
#undef tSummaryBarSpriteId
#undef tBallIconSpriteId
#undef tIsBattleStart
#undef tData15

static void SpriteCB_StatusSummaryBar(struct Sprite *sprite)
{
    if (sprite->x2 != 0)
        sprite->x2 += sprite->data[0];
}

static void sub_8049568(struct Sprite *sprite)
{
    sprite->data[1] += 32;
    if (sprite->data[0] > 0)
        sprite->x2 += sprite->data[1] >> 4;
    else
        sprite->x2 -= sprite->data[1] >> 4;
    sprite->data[1] &= 0xF;
}

static void SpriteCB_StatusSummaryBallsOnBattleStart(struct Sprite *sprite)
{
    u8 var1;
    u16 var2;
    s8 pan;

    if (sprite->data[1] > 0)
    {
        sprite->data[1]--;
        return;
    }

    var1 = sprite->data[2];
    var2 = sprite->data[3];
    var2 += 56;
    sprite->data[3] = var2 & 0xFFF0;

    if (var1 != 0)
    {
        sprite->x2 += var2 >> 4;
        if (sprite->x2 > 0)
            sprite->x2 = 0;
    }
    else
    {
        sprite->x2 -= var2 >> 4;
        if (sprite->x2 < 0)
            sprite->x2 = 0;
    }

    if (sprite->x2 == 0)
    {
        pan = SOUND_PAN_TARGET;
        if (var1 != 0)
            pan = SOUND_PAN_ATTACKER;

        if (sprite->data[7] != 0)
            PlaySE2WithPanning(SE_BALL_TRAY_EXIT, pan);
        else
            PlaySE1WithPanning(SE_BALL_TRAY_BALL, pan);

        sprite->callback = SpriteCallbackDummy;
    }
}

static void sub_8049630(struct Sprite *sprite)
{
    u8 var1;
    u16 var2;

    if (sprite->data[1] > 0)
    {
        sprite->data[1]--;
        return;
    }
    var1 = sprite->data[2];
    var2 = sprite->data[3];
    var2 += 56;
    sprite->data[3] = var2 & 0xFFF0;
    if (var1 != 0)
        sprite->x2 += var2 >> 4;
    else
        sprite->x2 -= var2 >> 4;
    if (sprite->x2 + sprite->x > 248
        || sprite->x2 + sprite->x < -8)
    {
        sprite->invisible = TRUE;
        sprite->callback = SpriteCallbackDummy;
    }
}

static void SpriteCB_StatusSummaryBallsOnSwitchout(struct Sprite *sprite)
{
    u8 barSpriteId = sprite->data[0];

    sprite->x2 = gSprites[barSpriteId].x2;
    sprite->y2 = gSprites[barSpriteId].y2;
}

static bool8 CheckBattleTypeGhost(struct Pokemon *mon, u8 battlerId)
{
	u8 buffer[POKEMON_NAME_LENGTH + 1];

    if (gBattleTypeFlags & BATTLE_TYPE_GHOST && GetBattlerSide(battlerId) != B_SIDE_PLAYER)
    {
        GetMonData(mon, MON_DATA_NICKNAME, buffer);
        StringGet_Nickname(buffer);
        if (!StringCompare(buffer, gText_Ghost))
            return TRUE;
    }
    return FALSE;
}

void TryAddPokeballIconToHealthbox(u8 healthboxSpriteId, bool8 noStatus)
{
    u8 healthBarSpriteId, battlerId = gSprites[healthboxSpriteId].hMain_Battler;
	struct Pokemon *mon = GetBattlerPartyIndexPtr(battlerId);
	
	if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(GetMonData(mon, MON_DATA_SPECIES)), FLAG_GET_CAUGHT)
		&& !(gBattleTypeFlags & (BATTLE_TYPE_FIRST_BATTLE | BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_TRAINER))
		&& GetBattlerSide(battlerId) != B_SIDE_PLAYER && !CheckBattleTypeGhost(mon, battlerId))
	{
		healthBarSpriteId = gSprites[healthboxSpriteId].hMain_HealthBarSpriteId;
		
		if (noStatus)
			CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_70), (void*)(OBJ_VRAM0 + (gSprites[healthBarSpriteId].oam.tileNum + 8) * TILE_SIZE_4BPP), 32);
		else
			CpuFill32(0, (void*)(OBJ_VRAM0 + (gSprites[healthBarSpriteId].oam.tileNum + 8) * TILE_SIZE_4BPP), 32);
	}
}

enum
{
    PAL_STATUS_PSN,
    PAL_STATUS_PAR,
    PAL_STATUS_SLP,
    PAL_STATUS_FRZ,
    PAL_STATUS_BRN
};

static const u16 sStatusIconColors[] = {
    [PAL_STATUS_PSN] = RGB(24, 12, 24),
    [PAL_STATUS_PAR] = RGB(23, 23, 3),
    [PAL_STATUS_SLP] = RGB(20, 20, 17),
    [PAL_STATUS_FRZ] = RGB(17, 22, 28),
    [PAL_STATUS_BRN] = RGB(28, 14, 10)
};

static void UpdateStatusIconInHealthbox(u8 healthboxSpriteId)
{
    s32 i;
    u8 battlerId = gSprites[healthboxSpriteId].hMain_Battler, healthBarSpriteId = gSprites[healthboxSpriteId].hMain_HealthBarSpriteId;
    u32 status, pltAdder;
    const u8 *statusGfxPtr;
    s16 tileNumAdder;
    u8 statusPalId;

    if (GetBattlerSide(battlerId) != B_SIDE_PLAYER)
		tileNumAdder = 0x11;
    else
	{
		if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            tileNumAdder = 0x1A;
        else
            tileNumAdder = 0x12;
	}
	status = GetMonData(GetBattlerPartyIndexPtr(battlerId), MON_DATA_STATUS);
	
    if (status & STATUS1_SLEEP)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(HEALTHBOX_GFX_STATUS_SLP_BATTLER0, battlerId));
        statusPalId = PAL_STATUS_SLP;
    }
    else if (status & STATUS1_PSN_ANY)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(HEALTHBOX_GFX_STATUS_PSN_BATTLER0, battlerId));
        statusPalId = PAL_STATUS_PSN;
    }
    else if (status & STATUS1_BURN)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(HEALTHBOX_GFX_STATUS_BRN_BATTLER0, battlerId));
        statusPalId = PAL_STATUS_BRN;
    }
    else if (status & STATUS1_FREEZE)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(HEALTHBOX_GFX_STATUS_FRZ_BATTLER0, battlerId));
        statusPalId = PAL_STATUS_FRZ;
    }
    else if (status & STATUS1_PARALYSIS)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(HEALTHBOX_GFX_STATUS_PRZ_BATTLER0, battlerId));
        statusPalId = PAL_STATUS_PAR;
    }
    else
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(HEALTHBOX_GFX_39);

        for (i = 0; i < 3; i++)
            CpuCopy32(statusGfxPtr, (void*)(OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + tileNumAdder + i) * TILE_SIZE_4BPP), 32);

        if (!gBattleSpritesDataPtr->battlerData[battlerId].hpNumbersNoBars)
            CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_1), (void *)(OBJ_VRAM0 + gSprites[healthBarSpriteId].oam.tileNum * TILE_SIZE_4BPP), 64);

        TryAddPokeballIconToHealthbox(healthboxSpriteId, TRUE);
        return;
    }
    pltAdder = gSprites[healthboxSpriteId].oam.paletteNum * 16;
    pltAdder += battlerId + 12;

    FillPalette(sStatusIconColors[statusPalId], pltAdder + 0x100, 2);
    CpuCopy16(gPlttBufferUnfaded + 0x100 + pltAdder, (void*)(OBJ_PLTT + pltAdder * 2), 2);
    CpuCopy32(statusGfxPtr, (void*)(OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + tileNumAdder) * TILE_SIZE_4BPP), 96);
    
	if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE || GetBattlerSide(battlerId) == B_SIDE_OPPONENT) && !gBattleSpritesDataPtr->battlerData[battlerId].hpNumbersNoBars)
    {
		CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_0), (void*)(OBJ_VRAM0 + gSprites[healthBarSpriteId].oam.tileNum * TILE_SIZE_4BPP), 32);
		CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_65), (void*)(OBJ_VRAM0 + (gSprites[healthBarSpriteId].oam.tileNum + 1) * TILE_SIZE_4BPP), 32);
    }
    TryAddPokeballIconToHealthbox(healthboxSpriteId, FALSE);
}

static u8 GetStatusIconForBattlerId(u8 statusElementId, u8 battlerId)
{
    switch (statusElementId)
    {
    case HEALTHBOX_GFX_STATUS_PSN_BATTLER0:
	    switch (battlerId)
		{
			case 0:
			    statusElementId = HEALTHBOX_GFX_STATUS_PSN_BATTLER0;
				break;
			case 1:
			    statusElementId = HEALTHBOX_GFX_STATUS_PSN_BATTLER1;
				break;
			case 2:
				statusElementId = HEALTHBOX_GFX_STATUS_PSN_BATTLER2;
				break;
			case 3:
				statusElementId = HEALTHBOX_GFX_STATUS_PSN_BATTLER3;
				break;
		}
        break;
    case HEALTHBOX_GFX_STATUS_PRZ_BATTLER0:
		switch (battlerId)
		{
			case 0:
				statusElementId = HEALTHBOX_GFX_STATUS_PRZ_BATTLER0;
				break;
			case 1:
				statusElementId = HEALTHBOX_GFX_STATUS_PRZ_BATTLER1;
				break;
			case 2:
				statusElementId = HEALTHBOX_GFX_STATUS_PRZ_BATTLER2;
				break;
			case 3:
				statusElementId = HEALTHBOX_GFX_STATUS_PRZ_BATTLER3;
				break;
		}
        break;
    case HEALTHBOX_GFX_STATUS_SLP_BATTLER0:
	    switch (battlerId)
		{
			case 0:
				statusElementId = HEALTHBOX_GFX_STATUS_SLP_BATTLER0;
				break;
			case 1:
				statusElementId = HEALTHBOX_GFX_STATUS_SLP_BATTLER1;
				break;
			case 2:
				statusElementId = HEALTHBOX_GFX_STATUS_SLP_BATTLER2;
				break;
			case 3:
				statusElementId = HEALTHBOX_GFX_STATUS_SLP_BATTLER3;
				break;
		}
        break;
    case HEALTHBOX_GFX_STATUS_FRZ_BATTLER0:
		switch (battlerId)
		{
			case 0:
				statusElementId = HEALTHBOX_GFX_STATUS_FRZ_BATTLER0;
				break;
			case 1:
				statusElementId = HEALTHBOX_GFX_STATUS_FRZ_BATTLER1;
				break;
			case 2:
				statusElementId = HEALTHBOX_GFX_STATUS_FRZ_BATTLER2;
				break;
			case 3:
				statusElementId = HEALTHBOX_GFX_STATUS_FRZ_BATTLER3;
				break;
		}
        break;
    case HEALTHBOX_GFX_STATUS_BRN_BATTLER0:
		switch (battlerId)
		{
			case 0:
				statusElementId = HEALTHBOX_GFX_STATUS_BRN_BATTLER0;
				break;
			case 1:
				statusElementId = HEALTHBOX_GFX_STATUS_BRN_BATTLER1;
				break;
			case 2:
				statusElementId = HEALTHBOX_GFX_STATUS_BRN_BATTLER2;
				break;
			case 3:
				statusElementId = HEALTHBOX_GFX_STATUS_BRN_BATTLER3;
				break;
		}
        break;
    }
    return statusElementId;
}

static void UpdateSafariBallsTextOnHealthbox(u8 healthboxSpriteId)
{
    u32 windowId, spriteTileNum;
    u8 *windowTileData = AddTextPrinterAndCreateWindowOnHealthbox(gText_SafariBalls, 0, 3, &windowId);

    spriteTileNum = gSprites[healthboxSpriteId].oam.tileNum * TILE_SIZE_4BPP;
    TextIntoHealthboxObject((void*)(OBJ_VRAM0 + 0x40) + spriteTileNum, windowTileData, 6);
    TextIntoHealthboxObject((void*)(OBJ_VRAM0 + 0x800) + spriteTileNum, windowTileData + 0xC0, 2);
    RemoveWindow(windowId);
}

static void UpdateLeftNoOfBallsTextOnHealthbox(u8 healthboxSpriteId)
{
    u8 text[16];
    u8 *txtPtr = StringCopy(text, gText_HighlightRed_Left);
    u32 windowId, spriteTileNum;
    u8 *windowTileData;

    ConvertIntToDecimalStringN(txtPtr, gNumSafariBalls, STR_CONV_MODE_LEFT_ALIGN, 2);

    windowTileData = AddTextPrinterAndCreateWindowOnHealthbox(text, GetStringRightAlignXOffset(0, text, 0x2F), 3, &windowId);
    spriteTileNum = gSprites[healthboxSpriteId].oam.tileNum * TILE_SIZE_4BPP;
    SafariTextIntoHealthboxObject((void*)(OBJ_VRAM0 + 0x2C0) + spriteTileNum, windowTileData, 2);
    SafariTextIntoHealthboxObject((void*)(OBJ_VRAM0 + 0xA00) + spriteTileNum, windowTileData + 0x40, 4);
    RemoveWindow(windowId);
}

static const u8 gUnknown_8260556[] = _("{HIGHLIGHT 02}");

static void UpdateNickInHealthbox(u8 healthboxSpriteId, struct Pokemon *mon)
{
    u8 nickname[POKEMON_NAME_LENGTH + 1], *windowTileData, *ptr = StringCopy(gDisplayedStringBattle, gUnknown_8260556), gender = GetMonGender(mon);
    u32 windowId, spriteTileNum;
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
	
    GetMonData(mon, MON_DATA_NICKNAME, nickname);
    ptr = StringCopy(ptr, nickname);
    *ptr++ = EXT_CTRL_CODE_BEGIN;
    *ptr++ = EXT_CTRL_CODE_COLOR;

    if (((species == SPECIES_NIDORAN_F || species == SPECIES_NIDORAN_M) && StringCompare(nickname, gSpeciesNames[species]) == 0)
		|| CheckBattleTypeGhost(mon, gSprites[healthboxSpriteId].hMain_Battler))
        gender = MON_GENDERLESS;

    switch (gender)
    {
    case MON_GENDERLESS:
        *ptr++ = TEXT_DYNAMIC_COLOR_2;
        break;
    case MON_MALE:
        *ptr++ = TEXT_DYNAMIC_COLOR_2;
        *ptr++ = CHAR_MALE;
        break;
    case MON_FEMALE:
        *ptr++ = TEXT_DYNAMIC_COLOR_1;
        *ptr++ = CHAR_FEMALE;
        break;
    }
	*ptr++ = EOS;
	
	windowTileData = AddTextPrinterAndCreateWindowOnHealthbox(gDisplayedStringBattle, 0, 3, &windowId);
    spriteTileNum = gSprites[healthboxSpriteId].oam.tileNum * TILE_SIZE_4BPP;

    if (GetBattlerSide(gSprites[healthboxSpriteId].hMain_Battler) == B_SIDE_PLAYER)
    {
        TextIntoHealthboxObject((void*)(OBJ_VRAM0 + 0x40 + spriteTileNum), windowTileData, 6);
        ptr = (void*)(OBJ_VRAM0);
        if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            ptr += spriteTileNum + 0x800;
        else
            ptr += spriteTileNum + 0x400;
        TextIntoHealthboxObject(ptr, windowTileData + 0xC0, 1);
    }
    else
        TextIntoHealthboxObject((void*)(OBJ_VRAM0 + 0x20 + spriteTileNum), windowTileData, 7);

    RemoveWindow(windowId);
}

void UpdateHealthboxAttribute(u8 battlerId, u8 elementId)
{
	bool8 isDoubles;
	u16 species;
	u32 exp, currLevelExp;
    u8 level, healthboxSpriteId = gHealthboxSpriteIds[battlerId];
	struct Pokemon *mon = GetBattlerPartyIndexPtr(battlerId);
	
	// Update for all battlers
	// Lv
	if (elementId == HEALTHBOX_LEVEL || elementId == HEALTHBOX_ALL)
		UpdateLvlInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_LEVEL));
	
	// Nick
	if (elementId == HEALTHBOX_NICK || elementId == HEALTHBOX_ALL)
		UpdateNickInHealthbox(healthboxSpriteId, GetBattlerIllusionPartyIndexPtr(battlerId));
	
	// Health bar
	if (elementId == HEALTHBOX_HEALTH_BAR || elementId == HEALTHBOX_ALL)
	{
		LoadBattleBarGfx();
		SetBattleBarStruct(battlerId, GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HP), 0);
		MoveBattleBar(battlerId, healthboxSpriteId, HEALTH_BAR);
	}
	
	// Status icon
	if (elementId == HEALTHBOX_STATUS_ICON || elementId == HEALTHBOX_ALL)
		UpdateStatusIconInHealthbox(healthboxSpriteId);
	
	// Only updates on player's side
	if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
	{
		// Hp
		if (elementId == HEALTHBOX_CURRENT_HP || elementId == HEALTHBOX_ALL)
			UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_HP), HP_CURRENT);
		
		// Hp max
		if (elementId == HEALTHBOX_MAX_HP || elementId == HEALTHBOX_ALL)
			UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_MAX_HP), HP_MAX);
		
		// Exp bar
		if (elementId == HEALTHBOX_EXP_BAR || elementId == HEALTHBOX_ALL)
		{
			isDoubles = (gBattleTypeFlags & BATTLE_TYPE_DOUBLE);
			
			if (!isDoubles)
			{
				species = GetMonData(mon, MON_DATA_SPECIES);
				level = GetMonData(mon, MON_DATA_LEVEL);
				exp = GetMonData(mon, MON_DATA_EXP);
				currLevelExp = gExperienceTables[gBaseStats[species].growthRate][level];
				
				LoadBattleBarGfx();
				SetBattleBarStruct(battlerId, gExperienceTables[gBaseStats[species].growthRate][level + 1] - currLevelExp, exp - currLevelExp, isDoubles);
				MoveBattleBar(battlerId, healthboxSpriteId, EXP_BAR);
			}
		}
		
		// Safari balls text
		if (elementId == HEALTHBOX_SAFARI_ALL_TEXT)
			UpdateSafariBallsTextOnHealthbox(healthboxSpriteId);
		
		// Num safari balls
		if (elementId == HEALTHBOX_SAFARI_BALLS_TEXT || elementId == HEALTHBOX_SAFARI_ALL_TEXT)
			UpdateLeftNoOfBallsTextOnHealthbox(healthboxSpriteId);
	}
}

#define B_EXPBAR_PIXELS 64
#define B_HEALTHBAR_PIXELS 48
#define HP_DRAIN_SPEED_DIVISOR 32 // the lower this faster the hp bar decreases vs high level enemy instead of always being the same speed

s32 MoveBattleBar(u8 battlerId, u8 healthboxSpriteId, u8 whichBar)
{
    s32 currentBarValue;

    if (whichBar == HEALTH_BAR) // health bar
    {
        currentBarValue = CalcNewBarValue(gBattleSpritesDataPtr->battleBars[battlerId].maxValue,
                                          gBattleSpritesDataPtr->battleBars[battlerId].oldValue,
                                          gBattleSpritesDataPtr->battleBars[battlerId].receivedValue,
                                          &gBattleSpritesDataPtr->battleBars[battlerId].currValue,
                                          B_HEALTHBAR_PIXELS / 8, max(gBattleSpritesDataPtr->battleBars[battlerId].maxValue / HP_DRAIN_SPEED_DIVISOR, 1));
    }
    else // exp bar
    {
        u16 expFraction = GetScaledExpFraction(gBattleSpritesDataPtr->battleBars[battlerId].oldValue,
                                               gBattleSpritesDataPtr->battleBars[battlerId].receivedValue,
                                               gBattleSpritesDataPtr->battleBars[battlerId].maxValue, 8);
        if (expFraction == 0)
            expFraction = 1;
        expFraction = abs(gBattleSpritesDataPtr->battleBars[battlerId].receivedValue / expFraction);

        currentBarValue = CalcNewBarValue(gBattleSpritesDataPtr->battleBars[battlerId].maxValue,
                                          gBattleSpritesDataPtr->battleBars[battlerId].oldValue,
                                          gBattleSpritesDataPtr->battleBars[battlerId].receivedValue,
                                          &gBattleSpritesDataPtr->battleBars[battlerId].currValue,
                                          B_EXPBAR_PIXELS / 8, expFraction);
    }

    if (whichBar == EXP_BAR || (whichBar == HEALTH_BAR && !gBattleSpritesDataPtr->battlerData[battlerId].hpNumbersNoBars))
        MoveBattleBarGraphically(battlerId, whichBar);

    if (currentBarValue == -1)
        gBattleSpritesDataPtr->battleBars[battlerId].currValue = 0;

    return currentBarValue;
}

static void MoveBattleBarGraphically(u8 battlerId, u8 whichBar)
{
    u8 array[8];
    u8 filledPixelsCount;
    u8 barElementId;
    u8 i;

    switch (whichBar)
    {
    case HEALTH_BAR:
        filledPixelsCount = CalcBarFilledPixels(gBattleSpritesDataPtr->battleBars[battlerId].maxValue,
                                                gBattleSpritesDataPtr->battleBars[battlerId].oldValue,
                                                gBattleSpritesDataPtr->battleBars[battlerId].receivedValue,
                                                &gBattleSpritesDataPtr->battleBars[battlerId].currValue,
                                                array, B_HEALTHBAR_PIXELS / 8);

        if (filledPixelsCount > (B_HEALTHBAR_PIXELS * 50 / 100)) // more than 50 % hp
            barElementId = HEALTHBOX_GFX_HP_BAR_GREEN;
        else if (filledPixelsCount > (B_HEALTHBAR_PIXELS * 20 / 100)) // more than 20% hp
            barElementId = HEALTHBOX_GFX_HP_BAR_YELLOW;
        else
            barElementId = HEALTHBOX_GFX_HP_BAR_RED; // 20 % or less

        for (i = 0; i < 6; i++)
        {
            u8 healthbarSpriteId = gSprites[gBattleSpritesDataPtr->battleBars[battlerId].healthboxSpriteId].hMain_HealthBarSpriteId;
            if (i < 2)
                CpuCopy32(GetHealthboxElementGfxPtr(barElementId) + array[i] * 32,
                          (void*)(OBJ_VRAM0 + (gSprites[healthbarSpriteId].oam.tileNum + 2 + i) * TILE_SIZE_4BPP), 32);
            else
                CpuCopy32(GetHealthboxElementGfxPtr(barElementId) + array[i] * 32,
                          (void*)(OBJ_VRAM0 + 64 + (i + gSprites[healthbarSpriteId].oam.tileNum) * TILE_SIZE_4BPP), 32);
        }
        break;
    case EXP_BAR:
        CalcBarFilledPixels(gBattleSpritesDataPtr->battleBars[battlerId].maxValue,
                            gBattleSpritesDataPtr->battleBars[battlerId].oldValue,
                            gBattleSpritesDataPtr->battleBars[battlerId].receivedValue,
                            &gBattleSpritesDataPtr->battleBars[battlerId].currValue,
                            array, B_EXPBAR_PIXELS / 8);
        if (GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerId]], MON_DATA_LEVEL) == MAX_LEVEL)
        {
            for (i = 0; i < 8; i++)
                array[i] = 0;
        }
        for (i = 0; i < 8; i++)
        {
            if (i < 4)
                CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_12) + array[i] * 32,
                          (void*)(OBJ_VRAM0 + (gSprites[gBattleSpritesDataPtr->battleBars[battlerId].healthboxSpriteId].oam.tileNum + 0x24 + i) * TILE_SIZE_4BPP), 32);
            else
                CpuCopy32(GetHealthboxElementGfxPtr(HEALTHBOX_GFX_12) + array[i] * 32,
                          (void*)(OBJ_VRAM0 + 0xB80 + (i + gSprites[gBattleSpritesDataPtr->battleBars[battlerId].healthboxSpriteId].oam.tileNum) * TILE_SIZE_4BPP), 32);
        }
        break;
    }
}

static s32 CalcNewBarValue(s32 maxValue, s32 oldValue, s32 receivedValue, s32 *currValue, u8 scale, u16 toAdd)
{
    s32 ret, newValue;
	
    scale *= 8;

    if (*currValue == -32768) // first function call
		*currValue = maxValue < scale ? Q_24_8(oldValue) : oldValue;

    newValue = oldValue - receivedValue;
    if (newValue < 0)
        newValue = 0;
    else if (newValue > maxValue)
        newValue = maxValue;

    if (maxValue < scale)
    {
        if (newValue == Q_24_8_TO_INT(*currValue) && (*currValue & 0xFF) == 0)
            return -1;
    }
    else
    {
        if (newValue == *currValue) // we're done, the bar's value has been updated
            return -1;
    }

    if (maxValue < scale) // handle cases of max var having less pixels than the whole bar
    {
        s32 toAdd_ = Q_24_8(maxValue) / scale;

        if (receivedValue < 0) // fill bar right
        {
            *currValue += toAdd_;
            ret = Q_24_8_TO_INT(*currValue);
            if (ret >= newValue)
            {
                *currValue = Q_24_8(newValue);
                ret = newValue;
            }
        }
        else // move bar left
        {
            *currValue -= toAdd_;
            ret = Q_24_8_TO_INT(*currValue);
            // try round up
            if ((*currValue & 0xFF) > 0)
                ret++;
            if (ret <= newValue)
            {
                *currValue = Q_24_8(newValue);
                ret = newValue;
            }
        }
    }
    else
    {
        if (receivedValue < 0) // fill bar right
        {
            *currValue += toAdd;
            if (*currValue > newValue)
                *currValue = newValue;
            ret = *currValue;
        }
        else // move bar left
        {
            *currValue -= toAdd;
            if (*currValue < newValue)
                *currValue = newValue;
            ret = *currValue;
        }
    }

    return ret;
}

static u8 CalcBarFilledPixels(s32 maxValue, s32 oldValue, s32 receivedValue, s32 *currValue, u8 *arg4, u8 scale)
{
    u8 pixels, filledPixels, totalPixels;
    u8 i;
    s32 newValue = oldValue - receivedValue;
	
    if (newValue < 0)
        newValue = 0;
    else if (newValue > maxValue)
        newValue = maxValue;

    totalPixels = scale * 8;

    for (i = 0; i < scale; i++)
        arg4[i] = 0;

    if (maxValue < totalPixels)
        pixels = (*currValue * totalPixels / maxValue) >> 8;
    else
        pixels = *currValue * totalPixels / maxValue;

    filledPixels = pixels;

    if (filledPixels == 0 && newValue > 0)
    {
        arg4[0] = 1;
        filledPixels = 1;
    }
    else
    {
        for (i = 0; i < scale; i++)
        {
            if (pixels >= 8)
            {
                arg4[i] = 8;
            }
            else
            {
                arg4[i] = pixels;
                break;
            }
            pixels -= 8;
        }
    }

    return filledPixels;
}

static u8 GetScaledExpFraction(s32 oldValue, s32 receivedValue, s32 maxValue, u8 scale)
{
    s8 oldToMax, newToMax;
	s32 result, newVal = oldValue - receivedValue;
	
	if (newVal < 0)
        newVal = 0;
    else if (newVal > maxValue)
        newVal = maxValue;

    scale *= 8;

    oldToMax = oldValue * scale / maxValue;
    newToMax = newVal * scale / maxValue;
    result = oldToMax - newToMax;

    return abs(result);
}

u8 GetScaledHPFraction(s16 hp, s16 maxhp, u8 scale)
{
    u8 result = hp * scale / maxhp;

    if (result == 0 && hp > 0)
        return 1;

    return result;
}

u8 GetHPBarLevel(s16 hp, s16 maxhp)
{
    u8 result;

    if (hp == maxhp)
    {
        result = HP_BAR_FULL;
    }
    else
    {
        u8 fraction = GetScaledHPFraction(hp, maxhp, B_HEALTHBAR_PIXELS);
        if (fraction > (B_HEALTHBAR_PIXELS * 50 / 100)) // more than 50 % hp
            result = HP_BAR_GREEN;
        else if (fraction > (B_HEALTHBAR_PIXELS * 20 / 100)) // more than 20% hp
            result = HP_BAR_YELLOW;
        else if (fraction > 0)
            result = HP_BAR_RED;
        else
            result = HP_BAR_EMPTY;
    }

    return result;
}

static const struct WindowTemplate sHealthboxWindowTemplate = {
    .bg = 0,
    .tilemapLeft = 0,
    .tilemapTop = 0,
    .width = 8,
    .height = 2,
    .paletteNum = 0,
    .baseBlock = 0x000
};

static u8* AddTextPrinterAndCreateWindowOnHealthbox(const u8 *str, u32 x, u32 y, u32 *windowId)
{
    u8 color[3] = {2, 1, 3};
    
	*windowId = AddWindow(&sHealthboxWindowTemplate);
    FillWindowPixelBuffer(*windowId, PIXEL_FILL(color[0]));
    AddTextPrinterParameterized4(*windowId, 0, x, y, 0, 0, color, -1, str);

    return (u8*)(GetWindowAttribute(*windowId, WINDOW_TILE_DATA));
}

static void TextIntoHealthboxObject(void *dest, u8 *windowTileData, s32 windowWidth)
{
    CpuCopy32(windowTileData + 256, dest + 256, windowWidth * TILE_SIZE_4BPP);
// + 256 as that prevents the top 4 blank rows of sHealthboxWindowTemplate from being copied
    if (windowWidth > 0)
    {
        do
        {
            CpuCopy32(windowTileData + 20, dest + 20, 12);
            dest += 32, windowTileData += 32;
            windowWidth--;
        } while (windowWidth != 0);
    }
}

static void SafariTextIntoHealthboxObject(void *dest, u8 *windowTileData, u32 windowWidth)
{
    CpuCopy32(windowTileData, dest, windowWidth * TILE_SIZE_4BPP);
    CpuCopy32(windowTileData + 256, dest + 256, windowWidth * TILE_SIZE_4BPP);
}

#define sHide   data[0]
#define sFinalX data[1]

#define WEATHER_ICON_X_POS   256
#define WEATHER_ICON_Y_POS   15
#define WEATHER_ICON_X_SLIDE 32

void TryCreateWeatherAnimIcon(void)
{
	u8 iconAnimId;
	
	if (gBattleWeather & WEATHER_ANY)
	{
		if (GetSpriteTileStartByTag(TAG_WEATHER_ICON_GFX) == 0xFFFF)
		{
			// get current weather enum id
			if (gBattleWeather & WEATHER_RAIN_ANY)
				iconAnimId = ENUM_WEATHER_RAIN;
			else if (gBattleWeather & WEATHER_SUN_ANY)
				iconAnimId = ENUM_WEATHER_SUN;
			else if (gBattleWeather & WEATHER_SANDSTORM_ANY) 
				iconAnimId = ENUM_WEATHER_SANDSTORM;
			else if (gBattleWeather & WEATHER_HAIL_ANY)
				iconAnimId = ENUM_WEATHER_HAIL;
			else if (gBattleWeather & WEATHER_FOG_ANY)
				iconAnimId = ENUM_WEATHER_FOG;
			else
				iconAnimId = ENUM_WEATHER_STRONG_WINDS;
			
			LoadSpriteSheet(&sSpriteSheet_WeatherStatusIcons[iconAnimId - 1]);
			LoadSpritePalette(&sSpritePalette_WeatherStatusIcons[iconAnimId - 1]);
			gBattleStruct->weatherIconSpriteId = CreateSprite(&sSpriteTemplate_WeatherIcon, WEATHER_ICON_X_POS, WEATHER_ICON_Y_POS, 0);
			gSprites[gBattleStruct->weatherIconSpriteId].sFinalX = WEATHER_ICON_X_POS - WEATHER_ICON_X_SLIDE;
		}
		ShowOrHideWeatherAnimIcon(FALSE); // show icon
	}
}

void ShowOrHideWeatherAnimIcon(bool8 hide)
{
	gSprites[gBattleStruct->weatherIconSpriteId].sHide = hide;
}

static void SpriteCB_WeatherAnimIcon(struct Sprite *sprite)
{
	if (sprite->sHide)
	{
		if (sprite->x != WEATHER_ICON_X_POS) // slide out
			++sprite->x;
		else // destroy sprite
		{
			FreeSpriteTilesByTag(TAG_WEATHER_ICON_GFX);
			FreeSpritePaletteByTag(TAG_WEATHER_ICON_GFX);
			DestroySprite(sprite);
		}
	}
	else
	{
		if (sprite->x != sprite->sFinalX) // slide in
			--sprite->x;
	}
}

#undef sHide
#undef sFinalX
