#include "global.h"
#include "gflib.h"
#include "battle_anim.h"
#include "data.h"
#include "decompress.h"
#include "pokemon_icon.h"
#include "task.h"
#include "trig.h"
#include "util.h"
#include "constants/battle_anim.h"

static u8 GetBattlerSpriteFinal_Y(u8 battlerId, u16 species, bool8 a3);
static void PlayerThrowBall_RunLinearTranslation_ThenceSetCBtoStoredInData6(struct Sprite *sprite);
static void SpriteCB_RunAnimFastLinearTranslation(struct Sprite *sprite);
static void AnimTask_AlphaFadeIn_Step(u8 taskId);
static void AnimTask_BlendMonInAndOutSetup(struct Task *task);
static void AnimTask_BlendMonInAndOutStep(u8 taskId);
static u16 GetBattlerYDeltaFromSpriteId(u8 spriteId);
static void AnimTask_AttackerPunchWithTrace_Step(u8 taskId);
static void PunchAnim_CreateTraceSprite(struct Task *task, u8 taskId);
static void SpriteCB_PunchTrace(struct Sprite *sprite);
static void SpriteCB_WeatherBallUp_Step(struct Sprite *sprite);

static EWRAM_DATA union AffineAnimCmd *sAnimTaskAffineAnim = NULL;

static const struct UCoords8 sBattlerCoords[][MAX_BATTLERS_COUNT] =
{
    {
        { 72, 80 },
        { 176, 40 },
        { 48, 40 },
        { 112, 80 },
    },
    {
        { 32, 80 },
        { 200, 40 },
        { 90, 88 },
        { 152, 32 },
    },
};

static const struct SpriteTemplate sSpriteTemplate_AdditionalForAnim =
{
	.tileTag = 55125,
	.paletteTag = 55125,
	.oam = &gOamData_AffineNormal_ObjNormal_64x64,
	.anims = gDummySpriteAnimTable,
	.images = NULL,
	.affineAnims = gDummySpriteAffineAnimTable,
	.callback = SpriteCallbackDummy,
};

static const struct SpriteSheet sMoveAnimAdtlSprSheet[] =
{
    { gMiscBlank_Gfx, 0x800, 55125 },
};

u8 GetBattlerSpriteCoord(u8 battlerId, u8 coordType)
{
    u8 retVal;
    u16 species;
    struct BattleSpriteInfo *spriteInfo;

    switch (coordType)
    {
    case BATTLER_COORD_X:
    case BATTLER_COORD_X_2:
        retVal = sBattlerCoords[gBattleTypeFlags & BATTLE_TYPE_DOUBLE][GetBattlerPosition(battlerId)].x;
        break;
    case BATTLER_COORD_Y:
        retVal = sBattlerCoords[gBattleTypeFlags & BATTLE_TYPE_DOUBLE][GetBattlerPosition(battlerId)].y;
        break;
    case BATTLER_COORD_Y_PIC_OFFSET:
    case BATTLER_COORD_Y_PIC_OFFSET_DEFAULT:
    default:
        spriteInfo = gBattleSpritesDataPtr->battlerData;
        species = spriteInfo[battlerId].transformSpecies == SPECIES_NONE ? GetMonData(GetBattlerIllusionPartyIndexPtr(battlerId), MON_DATA_SPECIES)
		: spriteInfo[battlerId].transformSpecies;
		retVal = GetBattlerSpriteFinal_Y(battlerId, species, (coordType == BATTLER_COORD_Y_PIC_OFFSET));
        break;
    }
    return retVal;
}

static u8 GetBattlerYDelta(u8 battlerId, u16 species)
{
	species = SanitizeSpeciesId(species);
    return GetBattlerSide(battlerId) == B_SIDE_PLAYER ? gMonBackPicCoords[species].y_offset : gMonFrontPicCoords[species].y_offset;
}

static u8 GetBattlerElevation(u8 battlerId, u16 species)
{
	return GetBattlerSide(battlerId) == B_SIDE_OPPONENT ? gEnemyMonElevation[SanitizeSpeciesId(species)] : 0;
}

static u8 GetBattlerSpriteFinal_Y(u8 battlerId, u16 species, bool8 a3)
{
	u8 y, battlerSide = GetBattlerSide(battlerId);
	u16 offset = GetBattlerYDelta(battlerId, species);

    if (battlerSide != B_SIDE_PLAYER)
		offset -= GetBattlerElevation(battlerId, species);
	
    y = offset + sBattlerCoords[gBattleTypeFlags & BATTLE_TYPE_DOUBLE][GetBattlerPosition(battlerId)].y;
    if (a3)
    {
        if (battlerSide == B_SIDE_PLAYER)
            y += 8;
        if (y > 104)
            y = 104;
    }
    return y;
}

u8 GetBattlerSpriteCoord2(u8 battlerId, u8 coordType)
{
    u16 species;
    struct BattleSpriteInfo *spriteInfo;

    if (coordType == BATTLER_COORD_Y_PIC_OFFSET || coordType == BATTLER_COORD_Y_PIC_OFFSET_DEFAULT)
    {
        spriteInfo = gBattleSpritesDataPtr->battlerData;
		species = spriteInfo[battlerId].transformSpecies == SPECIES_NONE ? gAnimBattlerSpecies[battlerId] : spriteInfo[battlerId].transformSpecies;
		return GetBattlerSpriteFinal_Y(battlerId, species, coordType == BATTLER_COORD_Y_PIC_OFFSET);
    }
    else
    {
        return GetBattlerSpriteCoord(battlerId, coordType);
    }
}

u8 GetBattlerSpriteDefault_Y(u8 battlerId)
{
    return GetBattlerSpriteCoord(battlerId, BATTLER_COORD_Y_PIC_OFFSET_DEFAULT);
}

u8 GetSubstituteSpriteDefault_Y(u8 battlerId)
{
    u16 y;

    if (GetBattlerSide(battlerId) != B_SIDE_PLAYER)
        y = GetBattlerSpriteCoord(battlerId, BATTLER_COORD_Y) + 16;
    else
        y = GetBattlerSpriteCoord(battlerId, BATTLER_COORD_Y) + 17;
	
    return y;
}

u8 GetGhostSpriteDefault_Y(u8 battlerId)
{
	return GetBattlerSpriteCoord(battlerId, GetBattlerSide(battlerId) != B_SIDE_OPPONENT ? BATTLER_COORD_Y_PIC_OFFSET_DEFAULT : BATTLER_COORD_Y);
}

u8 GetBattlerYCoordWithElevation(u8 battlerId)
{
	u16 transformSpecies = gBattleSpritesDataPtr->battlerData[battlerId].transformSpecies;
    u16 species = transformSpecies == SPECIES_NONE ? GetMonData(GetBattlerPartyIndexPtr(battlerId), MON_DATA_SPECIES) : transformSpecies;
    u8 y = GetBattlerSpriteCoord(battlerId, BATTLER_COORD_Y);
	
    if (GetBattlerSide(battlerId) != B_SIDE_PLAYER)
        y -= GetBattlerElevation(battlerId, species);
	
    return y;
}

u8 GetAnimBattlerSpriteId(u8 animBattler)
{
    u8 *sprites;

    if (animBattler == ANIM_ATTACKER)
    {
        if (IsBattlerSpritePresent(gBattleAnimAttacker))
        {
            sprites = gBattlerSpriteIds;
            return sprites[gBattleAnimAttacker];
        }
        else
        {
            return 0xFF;
        }
    }
    else if (animBattler == ANIM_TARGET)
    {
        if (IsBattlerSpritePresent(gBattleAnimTarget))
        {
            sprites = gBattlerSpriteIds;
            return sprites[gBattleAnimTarget];
        }
        else
        {
            return 0xFF;
        }
    }
    else if (animBattler == ANIM_ATK_PARTNER)
    {
        if (!IsBattlerSpriteVisible(BATTLE_PARTNER(gBattleAnimAttacker)))
            return 0xFF;
        else
            return gBattlerSpriteIds[BATTLE_PARTNER(gBattleAnimAttacker)];
    }
    else
    {
        if (IsBattlerSpriteVisible(BATTLE_PARTNER(gBattleAnimTarget)))
            return gBattlerSpriteIds[BATTLE_PARTNER(gBattleAnimTarget)];
        else
            return 0xFF;
    }
}

void StoreSpriteCallbackInData6(struct Sprite *sprite, SpriteCallback callback)
{
    sprite->data[6] = (u32)(callback) & 0xFFFF;
    sprite->data[7] = (u32)(callback) >> 16;
}

static void SetCallbackToStoredInData6(struct Sprite *sprite)
{
    u32 callback = (u16)sprite->data[6] | (sprite->data[7] << 16);
    
    sprite->callback = (SpriteCallback)callback;
}

// x = a * sin(theta0 + dtheta * t)
// y = a * cos(theta0 + dtheta * t)
void TranslateSpriteInCircleOverDuration(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = Sin(sprite->data[0], sprite->data[1]);
        sprite->y2 = Cos(sprite->data[0], sprite->data[1]);
        sprite->data[0] += sprite->data[2];
        if (sprite->data[0] >= 0x100)
            sprite->data[0] -= 0x100;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 0x100;
        --sprite->data[3];
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

// x = (a0 + da * t) * sin(theta0 + dtheta * t)
// y = (a0 + da * t) * cos(theta0 + dtheta * t)
void TranslateSpriteInGrowingCircleOverDuration(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = Sin(sprite->data[0], (sprite->data[5] >> 8) + sprite->data[1]);
        sprite->y2 = Cos(sprite->data[0], (sprite->data[5] >> 8) + sprite->data[1]);
        sprite->data[0] += sprite->data[2];
        sprite->data[5] += sprite->data[4];
        if (sprite->data[0] >= 0x100)
            sprite->data[0] -= 0x100;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 0x100;
        --sprite->data[3];
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

// not used
// x = alpl * sin(alpha0 + dalpha * t)
// y = ampl * cos(beta0 + dbeta * t)
static void TranslateSpriteInLissajousCurveOverDuration(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = Sin(sprite->data[0], sprite->data[1]);
        sprite->y2 = Cos(sprite->data[4], sprite->data[1]);
        sprite->data[0] += sprite->data[2];
        sprite->data[4] += sprite->data[5];
        if (sprite->data[0] >= 0x100)
            sprite->data[0] -= 0x100;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 0x100;
        if (sprite->data[4] >= 0x100)
            sprite->data[4] -= 0x100;
        else if (sprite->data[4] < 0)
            sprite->data[4] += 0x100;
        --sprite->data[3];
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

// x = a * sin(theta0 + dtheta * t)
// y = b * cos(theta0 + dtheta * t)
void TranslateSpriteInEllipseOverDuration(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->x2 = Sin(sprite->data[0], sprite->data[1]);
        sprite->y2 = Cos(sprite->data[0], sprite->data[4]);
        sprite->data[0] += sprite->data[2];
        if (sprite->data[0] >= 0x100)
            sprite->data[0] -= 0x100;
        else if (sprite->data[0] < 0)
            sprite->data[0] += 0x100;
        --sprite->data[3];
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

// Simply waits until the sprite's data[0] hits zero.
// This is used to let sprite anims or affine anims to run for a designated
// duration.
void WaitAnimForDuration(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
        --sprite->data[0];
    else
        SetCallbackToStoredInData6(sprite);
}

static void SetupAndStartSpriteLinearTranslation(struct Sprite *sprite)
{
    SetupLinearTranslationWithFixedDuration(sprite);
    sprite->callback = TranslateSpriteLinear;
    sprite->callback(sprite);
}

void SetupLinearTranslationWithFixedDuration(struct Sprite *sprite)
{
    s16 old;
    s32 xDiff;

    if (sprite->data[1] > sprite->data[2])
        sprite->data[0] = -sprite->data[0];
    xDiff = sprite->data[2] - sprite->data[1];
    old = sprite->data[0];
    sprite->data[0] = abs(xDiff / sprite->data[0]);
    sprite->data[2] = (sprite->data[4] - sprite->data[3]) / sprite->data[0];
    sprite->data[1] = old;
}

void TranslateSpriteLinear(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        --sprite->data[0];
        sprite->x2 += sprite->data[1];
        sprite->y2 += sprite->data[2];
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void TranslateSpriteLinearFixedPoint(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        --sprite->data[0];
        sprite->data[3] += sprite->data[1];
        sprite->data[4] += sprite->data[2];
        sprite->x2 = sprite->data[3] >> 8;
        sprite->y2 = sprite->data[4] >> 8;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

static void TranslateSpriteLinearFixedPointIconFrame(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        --sprite->data[0];
        sprite->data[3] += sprite->data[1];
        sprite->data[4] += sprite->data[2];
        sprite->x2 = sprite->data[3] >> 8;
        sprite->y2 = sprite->data[4] >> 8;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }

    UpdateMonIconFrame(sprite);
}

void TranslateMonSpriteLinear(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        --sprite->data[0];
        gSprites[sprite->data[3]].x2 += sprite->data[1];
        gSprites[sprite->data[3]].y2 += sprite->data[2];
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void TranslateMonSpriteLinearFixedPoint(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        --sprite->data[0];
        sprite->data[3] += sprite->data[1];
        sprite->data[4] += sprite->data[2];
        gSprites[sprite->data[5]].x2 = sprite->data[3] >> 8;
        gSprites[sprite->data[5]].y2 = sprite->data[4] >> 8;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void TranslateSpriteLinearAndFlicker(struct Sprite *sprite)
{
    if (sprite->data[0] > 0)
    {
        --sprite->data[0];
        sprite->x2 = sprite->data[2] >> 8;
        sprite->data[2] += sprite->data[1];
        sprite->y2 = sprite->data[4] >> 8;
        sprite->data[4] += sprite->data[3];
		
        if (sprite->data[0] % sprite->data[5] == 0 && sprite->data[5])
			sprite->invisible ^= 1;
    }
    else
    {
        SetCallbackToStoredInData6(sprite);
    }
}

void DestroySpriteAndMatrix(struct Sprite *sprite)
{
    FreeSpriteOamMatrix(sprite);
    DestroyAnimSprite(sprite);
}

void RunStoredCallbackWhenAffineAnimEnds(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
        SetCallbackToStoredInData6(sprite);
}

void RunStoredCallbackWhenAnimEnds(struct Sprite *sprite)
{
    if (sprite->animEnded)
        SetCallbackToStoredInData6(sprite);
}

void DestroyAnimSpriteAndDisableBlend(struct Sprite *sprite)
{
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    DestroyAnimSprite(sprite);
}

void DestroyAnimVisualTaskAndDisableBlend(u8 taskId)
{
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    DestroyAnimVisualTask(taskId);
}

void SetSpriteCoordsToAnimAttackerCoords(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
}

// Sets the initial x offset of the anim sprite depending on the horizontal orientation
// of the two involved mons.
void SetAnimSpriteInitialXOffset(struct Sprite *sprite, s16 xOffset)
{
    u16 attackerX = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X);
    u16 targetX = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X);

    if (attackerX > targetX)
    {
        sprite->x -= xOffset;
    }
    else if (attackerX < targetX)
    {
        sprite->x += xOffset;
    }
    else
    {
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
            sprite->x -= xOffset;
        else
            sprite->x += xOffset;
    }
}

void InitAnimArcTranslation(struct Sprite *sprite)
{
    sprite->sTransl_InitX = sprite->x;
    sprite->sTransl_InitY = sprite->y;
    InitAnimLinearTranslation(sprite);
    sprite->data[6] = 0x8000 / sprite->sTransl_Speed;
    sprite->data[7] = 0;
}

bool8 TranslateAnimHorizontalArc(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
        return TRUE;
    sprite->data[7] += sprite->data[6];
    sprite->y2 += Sin((u8)(sprite->data[7] >> 8), sprite->sTransl_ArcAmpl);
    return FALSE;
}

bool8 TranslateAnimVerticalArc(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
        return TRUE;
    sprite->data[7] += sprite->data[6];
    sprite->x2 += Sin((u8)(sprite->data[7] >> 8), sprite->sTransl_ArcAmpl);
    return FALSE;
}

void SetSpritePrimaryCoordsFromSecondaryCoords(struct Sprite *sprite)
{
    sprite->x += sprite->x2;
    sprite->y += sprite->y2;
    sprite->x2 = 0;
    sprite->y2 = 0;
}

void InitSpritePosToAnimTarget(struct Sprite *sprite, bool8 respectMonPicOffsets)
{
    // Battle anim sprites are automatically created at the anim target's center, which
    // is why there is no else clause for the "respectMonPicOffsets" check.
    if (!respectMonPicOffsets)
    {
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_X);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_Y);
    }
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}

void InitSpritePosToAnimAttacker(struct Sprite *sprite, bool8 respectMonPicOffsets)
{
    if (!respectMonPicOffsets)
    {
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_X);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_Y);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_X_2);
        sprite->y = GetBattlerSpriteCoord2(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    }
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}

u8 GetBattlerAtPosition(u8 position)
{
    u8 i;

    for (i = 0; i < gBattlersCount; ++i)
	{
        if (gBattlerPositions[i] == position)
            break;
	}
    return i;
}

bool8 IsBattlerSpritePresent(u8 battlerId)
{
    if (gBattlerPositions[battlerId] == 0xFF)
        return FALSE;
	
    if (!gBattleStruct->spriteIgnore0Hp)
    {
		if (!GetMonData(GetBattlerPartyIndexPtr(battlerId), MON_DATA_HP))
			return FALSE;
    }
    return TRUE;
}

void GetBattleAnimBgData(struct BattleAnimBgData *animBgData, u32 bgId)
{
    if (bgId == 1)
    {
        animBgData->paletteId = 8;
		animBgData->bgId = 1;
		animBgData->tilesOffset = 0x200;
    }
    else
    {
        animBgData->paletteId = 9;
        animBgData->bgId = 2;
        animBgData->tilesOffset = 0x300;
    }
	animBgData->bgTiles = gBattleAnimMons_BgTilesBuffer;
	animBgData->bgTilemap = (u16 *)gBattleAnimMons_BgTilemapBuffer;
}

void GetBattleAnimBgDataByPriorityRank(struct BattleAnimBgData *animBgData)
{
    animBgData->bgTiles = gBattleAnimMons_BgTilesBuffer;
    animBgData->bgTilemap = (u16 *)gBattleAnimMons_BgTilemapBuffer;
    if (GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) == 1)
    {
        animBgData->paletteId = 8;
        animBgData->bgId = 1;
        animBgData->tilesOffset = 0x200;
    }
    else
    {
        animBgData->paletteId = 9;
        animBgData->bgId = 2;
        animBgData->tilesOffset = 0x300;
    }
}

void InitBattleAnimBg(u32 bgId)
{
    struct BattleAnimBgData animBgData;

    GetBattleAnimBgData(&animBgData, bgId);
    CpuFill32(0, animBgData.bgTiles, 0x2000);
    LoadBgTiles(bgId, animBgData.bgTiles, 0x2000, animBgData.tilesOffset);
    FillBgTilemapBufferRect(bgId, 0, 0, 0, 32, 64, 17);
    CopyBgTilemapBufferToVram(bgId);
}

void AnimLoadCompressedBgGfx(u32 bgId, const u32 *src, u32 tilesOffset)
{
    CpuFill32(0, gBattleAnimMons_BgTilesBuffer, 0x2000);
    LZDecompressWram(src, gBattleAnimMons_BgTilesBuffer);
    LoadBgTiles(bgId, gBattleAnimMons_BgTilesBuffer, 0x2000, tilesOffset);
}

void InitAnimBgTilemapBuffer(u32 bgId, const void *src)
{
    FillBgTilemapBufferRect(bgId, 0, 0, 0, 32, 64, 17);
    CopyToBgTilemapBuffer(bgId, src, 0, 0);
}

void AnimLoadCompressedBgTilemap(u32 bgId, const u32 *src)
{
    InitAnimBgTilemapBuffer(bgId, src);
    CopyBgTilemapBufferToVram(bgId);
}

void ToggleBg3Mode(bool8 arg0)
{
    if (!arg0)
    {
        SetAnimBgAttribute(3, BG_ANIM_SCREEN_SIZE, 0);
        SetAnimBgAttribute(3, BG_ANIM_AREA_OVERFLOW_MODE, 1);
    }
    else
    {
        SetAnimBgAttribute(3, BG_ANIM_SCREEN_SIZE, 1);
        SetAnimBgAttribute(3, BG_ANIM_AREA_OVERFLOW_MODE, 0);
    }
}

void StartSpriteLinearTranslationFromCurrentPos(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitSpriteDataForLinearTranslation(sprite);
    sprite->callback = TranslateSpriteLinearFixedPointIconFrame;
    sprite->callback(sprite);
}

void InitSpriteDataForLinearTranslation(struct Sprite *sprite)
{
    s16 x = (sprite->data[2] - sprite->data[1]) << 8;
    s16 y = (sprite->data[4] - sprite->data[3]) << 8;

    sprite->data[1] = x / sprite->data[0];
    sprite->data[2] = y / sprite->data[0];
    sprite->data[4] = 0;
    sprite->data[3] = 0;
}

void InitAnimLinearTranslation(struct Sprite *sprite)
{
    s32 x = sprite->sTransl_DestX - sprite->sTransl_InitX;
    s32 y = sprite->sTransl_DestY - sprite->sTransl_InitY;
    bool8 movingLeft = x < 0;
    bool8 movingUp = y < 0;
    u16 xDelta = abs(x) << 8;
    u16 yDelta = abs(y) << 8;

    xDelta = xDelta / sprite->sTransl_Speed;
    yDelta = yDelta / sprite->sTransl_Speed;

    if (movingLeft)
        xDelta |= 1;
    else
        xDelta &= ~1;

    if (movingUp)
        yDelta |= 1;
    else
        yDelta &= ~1;

    sprite->data[1] = xDelta;
    sprite->data[2] = yDelta;
    sprite->data[4] = 0;
    sprite->data[3] = 0;
}

void StartAnimLinearTranslation(struct Sprite *sprite)
{
    sprite->sTransl_InitX = sprite->x;
    sprite->sTransl_InitY = sprite->y;
    InitAnimLinearTranslation(sprite);
    sprite->callback = RunLinearTranslation_ThenceSetCBtoStoredInData6;
    sprite->callback(sprite);
}

void PlayerThrowBall_StartAnimLinearTranslation(struct Sprite *sprite)
{
    sprite->sTransl_InitX = sprite->x;
    sprite->sTransl_InitY = sprite->y;
    InitAnimLinearTranslation(sprite);
    sprite->callback = PlayerThrowBall_RunLinearTranslation_ThenceSetCBtoStoredInData6;
    sprite->callback(sprite);
}

bool8 AnimTranslateLinear(struct Sprite *sprite)
{
    u16 v1, v2, x, y;

    if (!sprite->data[0])
        return TRUE;
    v1 = sprite->data[1];
    v2 = sprite->data[2];
    x = sprite->data[3];
    y = sprite->data[4];
    x += v1;
    y += v2;
    if (v1 & 1)
        sprite->x2 = -(x >> 8);
    else
        sprite->x2 = x >> 8;

    if (v2 & 1)
        sprite->y2 = -(y >> 8);
    else
        sprite->y2 = y >> 8;
    sprite->data[3] = x;
    sprite->data[4] = y;
    --sprite->data[0];
    return FALSE;
}

void RunLinearTranslation_ThenceSetCBtoStoredInData6(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
        SetCallbackToStoredInData6(sprite);
}

static void PlayerThrowBall_RunLinearTranslation_ThenceSetCBtoStoredInData6(struct Sprite *sprite)
{
    UpdatePlayerPosInThrowAnim(sprite);
    if (AnimTranslateLinear(sprite))
        SetCallbackToStoredInData6(sprite);
}

void BattleAnim_InitLinearTranslationWithDuration(struct Sprite *sprite)
{
    s32 v1 = abs(sprite->sTransl_DestX - sprite->sTransl_InitX) << 8;

    sprite->sTransl_Speed = v1 / sprite->sTransl_Duration;
    InitAnimLinearTranslation(sprite);
}

void BattleAnim_InitAndRunLinearTranslationWithDuration(struct Sprite *sprite)
{
    sprite->sTransl_InitX = sprite->x;
    sprite->sTransl_InitY = sprite->y;
    BattleAnim_InitLinearTranslationWithDuration(sprite);
    sprite->callback = RunLinearTranslation_ThenceSetCBtoStoredInData6;
    sprite->callback(sprite);
}

static void InitAnimFastLinearTranslation(struct Sprite *sprite)
{
    s32 xDiff = sprite->sTransl_DestX - sprite->sTransl_InitX;
    s32 yDiff = sprite->sTransl_DestY - sprite->sTransl_InitY;
    bool8 xSign = xDiff < 0;
    bool8 ySign = yDiff < 0;
    u16 x2 = abs(xDiff) << 4;
    u16 y2 = abs(yDiff) << 4;

    x2 /= sprite->sTransl_Duration;
    y2 /= sprite->sTransl_Duration;
    if (xSign)
        x2 |= 1;
    else
        x2 &= ~1;
    if (ySign)
        y2 |= 1;
    else
        y2 &= ~1;
    sprite->data[1] = x2;
    sprite->data[2] = y2;
    sprite->data[4] = 0;
    sprite->data[3] = 0;
}

void InitAndRunAnimFastLinearTranslation(struct Sprite *sprite)
{
    sprite->sTransl_InitX = sprite->x;
    sprite->sTransl_InitY = sprite->y;
    InitAnimFastLinearTranslation(sprite);
    sprite->callback = SpriteCB_RunAnimFastLinearTranslation;
    sprite->callback(sprite);
}

bool8 AnimFastTranslateLinear(struct Sprite *sprite)
{
    u16 v1, v2, x, y;

    if (!sprite->data[0])
        return TRUE;
    v1 = sprite->data[1];
    v2 = sprite->data[2];
    x = sprite->data[3];
    y = sprite->data[4];
    x += v1;
    y += v2;
    if (v1 & 1)
        sprite->x2 = -(x >> 4);
    else
        sprite->x2 = x >> 4;
    if (v2 & 1)
        sprite->y2 = -(y >> 4);
    else
        sprite->y2 = y >> 4;
    sprite->data[3] = x;
    sprite->data[4] = y;
    --sprite->data[0];
    return FALSE;
}

static void SpriteCB_RunAnimFastLinearTranslation(struct Sprite *sprite)
{
    if (AnimFastTranslateLinear(sprite))
        SetCallbackToStoredInData6(sprite);
}

void InitAnimFastLinearTranslationWithSpeed(struct Sprite *sprite)
{
    s32 xDiff = abs(sprite->data[2] - sprite->data[1]) << 4;

    sprite->data[0] = xDiff / sprite->data[0];
    InitAnimFastLinearTranslation(sprite);
}

void InitAndStartAnimFastLinearTranslationWithSpeed(struct Sprite *sprite)
{
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;
    InitAnimFastLinearTranslationWithSpeed(sprite);
    sprite->callback = SpriteCB_RunAnimFastLinearTranslation;
    sprite->callback(sprite);
}

void SetSpriteRotScale(u8 spriteId, s16 xScale, s16 yScale, u16 rotation)
{
    s32 i;
    struct ObjAffineSrcData src;
    struct OamMatrix matrix;

    src.xScale = xScale;
    src.yScale = yScale;
    src.rotation = rotation;
    i = gSprites[spriteId].oam.matrixNum;
    ObjAffineSet(&src, &matrix, 1, 2);
    gOamMatrices[i].a = matrix.a;
    gOamMatrices[i].b = matrix.b;
    gOamMatrices[i].c = matrix.c;
    gOamMatrices[i].d = matrix.d;
}

void PrepareBattlerSpriteForRotScale(u8 spriteId, u8 objMode)
{
    u8 battlerId = gSprites[spriteId].data[0];

    if (IsBattlerSpriteVisible(battlerId))
        gSprites[spriteId].invisible = FALSE;
    gSprites[spriteId].oam.objMode = objMode;
    gSprites[spriteId].affineAnimPaused = TRUE;
    if (!gSprites[spriteId].oam.affineMode)
        gSprites[spriteId].oam.matrixNum = gBattleSpritesDataPtr->healthBoxesData[battlerId].matrixNum;
    gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_DOUBLE;
    CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
}

void ResetSpriteRotScale(u8 spriteId)
{
    SetSpriteRotScale(spriteId, 0x100, 0x100, 0);
    gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_NORMAL;
    gSprites[spriteId].oam.objMode = 0;
    gSprites[spriteId].affineAnimPaused = FALSE;
    CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
}

// Sets the sprite's y offset equal to the y displacement caused by the
// matrix's rotation.
void SetBattlerSpriteYOffsetFromRotation(u8 spriteId)
{
    // The "c" component of the battler sprite matrix contains the sine of the rotation angle divided by some scale amount.
    s16 c = gOamMatrices[gSprites[spriteId].oam.matrixNum].c;

    if (c < 0)
        c = -c;
    gSprites[spriteId].y2 = c >> 3;
}

void TrySetSpriteRotScale(struct Sprite *sprite, bool8 recalcCenterVector, s16 xScale, s16 yScale, u16 rotation)
{
    s32 i;
    struct ObjAffineSrcData src;
    struct OamMatrix matrix;

    if (sprite->oam.affineMode & 1)
    {
        sprite->affineAnimPaused = TRUE;
        if (recalcCenterVector)
            CalcCenterToCornerVec(sprite, sprite->oam.shape, sprite->oam.size, sprite->oam.affineMode);
        src.xScale = xScale;
        src.yScale = yScale;
        src.rotation = rotation;
        i = sprite->oam.matrixNum;
        ObjAffineSet(&src, &matrix, 1, 2);
        gOamMatrices[i].a = matrix.a;
        gOamMatrices[i].b = matrix.b;
        gOamMatrices[i].c = matrix.c;
        gOamMatrices[i].d = matrix.d;
    }
}

void TryResetSpriteAffineState(struct Sprite *sprite)
{
    TrySetSpriteRotScale(sprite, TRUE, 0x100, 0x100, 0);
    sprite->affineAnimPaused = FALSE;
    CalcCenterToCornerVec(sprite, sprite->oam.shape, sprite->oam.size, sprite->oam.affineMode);
}

u16 ArcTan2Neg(s16 a, s16 b)
{
    u16 var = ArcTan2(a, b);
    return -var;
}

void SetGreyscaleOrOriginalPalette(u16 paletteNum, bool8 restoreOriginalColor)
{
    s32 i;
    struct PlttData *originalColor;
    struct PlttData *destColor;
    u16 average;

    paletteNum *= 16;

    if (!restoreOriginalColor)
    {
        for (i = 0; i < 16; ++i)
        {
            originalColor = (struct PlttData *)&gPlttBufferUnfaded[paletteNum + i];
            average = originalColor->r + originalColor->g + originalColor->b;
            average /= 3;
            destColor = (struct PlttData *)&gPlttBufferFaded[paletteNum + i];
            destColor->r = average;
            destColor->g = average;
            destColor->b = average;
        }
    }
    else
    {
        CpuCopy32(&gPlttBufferUnfaded[paletteNum], &gPlttBufferFaded[paletteNum], 32);
    }
}

u32 SelectBattleAnimSpriteAndBgPalettes(bool8 battleBackground, bool8 attacker, bool8 target, bool8 attackerPartner, bool8 targetPartner, bool8 a6, bool8 a7)
{
    u32 selectedPalettes = 0;
    u32 shift;

    if (battleBackground)
    {
        selectedPalettes = 0xe;
    }
    if (attacker)
    {
        shift = gBattleAnimAttacker + 16;
        selectedPalettes |= 1 << shift;
    }
    if (target)
    {
        shift = gBattleAnimTarget + 16;
        selectedPalettes |= 1 << shift;
    }
    if (attackerPartner)
    {
        if (IsBattlerSpriteVisible(BATTLE_PARTNER(gBattleAnimAttacker)))
        {
            shift = BATTLE_PARTNER(gBattleAnimAttacker) + 16;
            selectedPalettes |= 1 << shift;
        }
    }
    if (targetPartner)
    {
        if (IsBattlerSpriteVisible(BATTLE_PARTNER(gBattleAnimTarget)))
        {
            shift = BATTLE_PARTNER(gBattleAnimTarget) + 16;
            selectedPalettes |= 1 << shift;
        }
    }
    if (a6)
    {
        selectedPalettes |= 0x100;
    }
    if (a7)
    {
        selectedPalettes |= 0x200;
    }
    return selectedPalettes;
}

u32 SelectBattlerSpritePalettes(bool8 playerLeft, bool8 playerRight, bool8 foeLeft, bool8 foeRight)
{
    u32 var = 0;
    u32 shift;

    if (playerLeft)
    {
        if (IsBattlerSpriteVisible(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
        {
            var |= 1 << (GetBattlerAtPosition(B_POSITION_PLAYER_LEFT) + 16);
        }
    }
    if (playerRight)
    {
        if (IsBattlerSpriteVisible(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)))
        {
            shift = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT) + 16;
            var |= 1 << shift;
        }
    }
    if (foeLeft)
    {
        if (IsBattlerSpriteVisible(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)))
        {
            shift = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT) + 16;
            var |= 1 << shift;
        }
    }
    if (foeRight)
    {
        if (IsBattlerSpriteVisible(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)))
        {
            shift = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT) + 16;
            var |= 1 << shift;
        }
    }
    return var;
}

void AnimSpriteOnMonPos(struct Sprite *sprite)
{
    bool8 var;

    if (!sprite->data[0])
    {
		var = gBattleAnimArgs[3] == 0 ? TRUE : FALSE;
		
        if (!gBattleAnimArgs[2])
            InitSpritePosToAnimAttacker(sprite, var);
        else
            InitSpritePosToAnimTarget(sprite, var);
        ++sprite->data[0];

    }
    else if (sprite->animEnded || sprite->affineAnimEnded)
    {
        DestroySpriteAndMatrix(sprite);
    }
}

// Linearly translates a sprite to a target position on the
// other mon's sprite.
// arg 0: initial x offset
// arg 1: initial y offset
// arg 2: target x offset
// arg 3: target y offset
// arg 4: duration
// arg 5: lower 8 bits = location on attacking mon, upper 8 bits = location on target mon pick to target
void TranslateAnimSpriteToTargetMonLocation(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, (gBattleAnimArgs[5] & 0xFF00) ? FALSE : TRUE);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, (gBattleAnimArgs[5] & 0xFF) ? BATTLER_COORD_Y : BATTLER_COORD_Y_PIC_OFFSET) + gBattleAnimArgs[3];
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void AnimThrowProjectile(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + gBattleAnimArgs[3];
    sprite->data[5] = gBattleAnimArgs[5];
    InitAnimArcTranslation(sprite);
    sprite->callback = AnimThrowProjectile_Step;
}

void AnimThrowProjectile_Step(struct Sprite *sprite)
{
    if (TranslateAnimHorizontalArc(sprite))
        DestroyAnimSprite(sprite);
}

void AnimSnoreZ(struct Sprite *sprite)
{
    bool8 r4;
    u8 battlerId, coordType;

    if (!gBattleAnimArgs[6])
    {
        r4 = TRUE;
        coordType = BATTLER_COORD_Y_PIC_OFFSET;
    }
    else
    {
        r4 = FALSE;
        coordType = BATTLER_COORD_Y;
    }
    if (!gBattleAnimArgs[5])
    {
        InitSpritePosToAnimAttacker(sprite, r4);
        battlerId = gBattleAnimAttacker;
    }
    else
    {
        InitSpritePosToAnimTarget(sprite, r4);
        battlerId = gBattleAnimTarget;
    }
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    InitSpritePosToAnimTarget(sprite, r4);
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(battlerId, BATTLER_COORD_X_2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(battlerId, coordType) + gBattleAnimArgs[3];
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

s16 CloneBattlerSpriteWithBlend(u8 animBattler)
{
    u16 i;
    u8 spriteId = GetAnimBattlerSpriteId(animBattler);

    if (spriteId != 0xFF)
    {
        for (i = 0; i < MAX_SPRITES; ++i)
        {
            if (!gSprites[i].inUse)
            {
                gSprites[i] = gSprites[spriteId];
                gSprites[i].oam.objMode = ST_OAM_OBJ_BLEND;
                gSprites[i].invisible = FALSE;
                return i;
            }
        }
    }
    return -1;
}

void obj_delete_but_dont_free_vram(struct Sprite *sprite)
{
    sprite->usingSheet = TRUE;
    DestroySprite(sprite);
}

void AnimTask_AlphaFadeIn(u8 taskId)
{
    s16 v1 = 0, v2 = 0;

    if (gBattleAnimArgs[2] > gBattleAnimArgs[0])
        v2 = 1;
    if (gBattleAnimArgs[2] < gBattleAnimArgs[0])
        v2 = -1;
    if (gBattleAnimArgs[3] > gBattleAnimArgs[1])
        v1 = 1;
    if (gBattleAnimArgs[3] < gBattleAnimArgs[1])
        v1 = -1;
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = gBattleAnimArgs[4];
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = gBattleAnimArgs[0];
    gTasks[taskId].data[4] = gBattleAnimArgs[1];
    gTasks[taskId].data[5] = v2;
    gTasks[taskId].data[6] = v1;
    gTasks[taskId].data[7] = gBattleAnimArgs[2];
    gTasks[taskId].data[8] = gBattleAnimArgs[3];
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(gBattleAnimArgs[0], gBattleAnimArgs[1]));
    gTasks[taskId].func = AnimTask_AlphaFadeIn_Step;
}

static void AnimTask_AlphaFadeIn_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (++task->data[0] > task->data[1])
    {
        task->data[0] = 0;
        if (++task->data[2] & 1)
        {
            if (task->data[3] != task->data[7])
                task->data[3] += task->data[5];
        }
        else
        {
            if (task->data[4] != task->data[8])
                task->data[4] += task->data[6];
        }
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(task->data[3], task->data[4]));
        if (task->data[3] == task->data[7] && task->data[4] == task->data[8])
        {
            DestroyAnimVisualTask(taskId);
            return;
        }
    }
}

// Linearly blends a mon's sprite colors with a target color with increasing
// strength, and then blends out to the original color.
// arg 0: anim bank
// arg 1: blend color
// arg 2: target blend coefficient
// arg 3: initial delay
// arg 4: number of times to blend in and out
void AnimTask_BlendMonInAndOut(u8 task)
{
    u8 spriteId = GetAnimBattlerSpriteId(gBattleAnimArgs[0]);

    if (spriteId == 0xFF)
    {
        DestroyAnimVisualTask(task);
        return;
    }
    gTasks[task].data[0] = (gSprites[spriteId].oam.paletteNum * 0x10) + 0x101;
    AnimTask_BlendMonInAndOutSetup(&gTasks[task]);
}

static void AnimTask_BlendMonInAndOutSetup(struct Task *task)
{
    task->data[1] = gBattleAnimArgs[1];
    task->data[2] = 0;
    task->data[3] = gBattleAnimArgs[2];
    task->data[4] = 0;
    task->data[5] = gBattleAnimArgs[3];
    task->data[6] = 0;
    task->data[7] = gBattleAnimArgs[4];
    task->func = AnimTask_BlendMonInAndOutStep;
}

static void AnimTask_BlendMonInAndOutStep(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (++task->data[4] >= task->data[5])
    {
        task->data[4] = 0;
        if (!task->data[6])
        {
            ++task->data[2];
            BlendPalette(task->data[0], 15, task->data[2], task->data[1]);
            if (task->data[2] == task->data[3])
                task->data[6] = 1;
        }
        else
        {
            --task->data[2];
            BlendPalette(task->data[0], 15, task->data[2], task->data[1]);
            if (!task->data[2])
            {
                if (--task->data[7])
                {
                    task->data[4] = 0;
                    task->data[6] = 0;
                }
                else
                {
                    DestroyAnimVisualTask(taskId);
                    return;
                }
            }
        }
    }
}

void AnimTask_BlendPalInAndOutByTag(u8 taskId)
{
    u8 palette = IndexOfSpritePaletteTag(gBattleAnimArgs[0]);

    if (palette == 0xFF)
    {
        DestroyAnimVisualTask(taskId);
        return;
    }
    gTasks[taskId].data[0] = (palette * 0x10) + 0x101;
    AnimTask_BlendMonInAndOutSetup(&gTasks[taskId]);
}

void PrepareAffineAnimInTaskData(struct Task *task, u8 spriteId, const union AffineAnimCmd *affineAnimCmds)
{
    task->data[7] = 0;
    task->data[8] = 0;
    task->data[9] = 0;
    task->data[15] = spriteId;
    task->data[10] = 0x100;
    task->data[11] = 0x100;
    task->data[12] = 0;
    StorePointerInVars(&task->data[13], &task->data[14], affineAnimCmds);
    PrepareBattlerSpriteForRotScale(spriteId, ST_OAM_OBJ_NORMAL);
}

bool8 RunAffineAnimFromTaskData(struct Task *task)
{
    sAnimTaskAffineAnim = LoadPointerFromVars(task->data[13], task->data[14]) + (task->data[7] << 3);
    switch (sAnimTaskAffineAnim->type)
    {
    default:
        if (!sAnimTaskAffineAnim->frame.duration)
        {
            task->data[10] = sAnimTaskAffineAnim->frame.xScale;
            task->data[11] = sAnimTaskAffineAnim->frame.yScale;
            task->data[12] = sAnimTaskAffineAnim->frame.rotation;
            ++task->data[7];
            ++sAnimTaskAffineAnim;
        }
        task->data[10] += sAnimTaskAffineAnim->frame.xScale;
        task->data[11] += sAnimTaskAffineAnim->frame.yScale;
        task->data[12] += sAnimTaskAffineAnim->frame.rotation;
        SetSpriteRotScale(task->data[15], task->data[10], task->data[11], task->data[12]);
        SetBattlerSpriteYOffsetFromYScale(task->data[15]);
        if (++task->data[8] >= sAnimTaskAffineAnim->frame.duration)
        {
            task->data[8] = 0;
            ++task->data[7];
        }
        break;
    case AFFINEANIMCMDTYPE_JUMP:
        task->data[7] = sAnimTaskAffineAnim->jump.target;
        break;
    case AFFINEANIMCMDTYPE_LOOP:
        if (sAnimTaskAffineAnim->loop.count)
        {
            if (task->data[9])
            {
                if (!--task->data[9])
                {
                    ++task->data[7];
                    break;
                }
            }
            else
            {
                task->data[9] = sAnimTaskAffineAnim->loop.count;
            }
            if (!task->data[7])
            {
                break;
            }
            while (TRUE)
            {
                --task->data[7];
                --sAnimTaskAffineAnim;
                if (sAnimTaskAffineAnim->type == AFFINEANIMCMDTYPE_LOOP)
                {
                    ++task->data[7];
                    return TRUE;
                }
                if (!task->data[7])
                    return TRUE;
            }
        }
        ++task->data[7];
        break;
    case AFFINEANIMCMDTYPE_END:
        gSprites[task->data[15]].y2 = 0;
        ResetSpriteRotScale(task->data[15]);
        return FALSE;
    }
    return TRUE;
}

// Sets the sprite's y offset equal to the y displacement caused by the
// matrix's scale in the y dimension.
void SetBattlerSpriteYOffsetFromYScale(u8 spriteId)
{
    s32 var = 64 - GetBattlerYDeltaFromSpriteId(spriteId) * 2;
    s32 var2 = (var << 8) / gOamMatrices[gSprites[spriteId].oam.matrixNum].d;

    if (var2 > 128)
        var2 = 128;
    gSprites[spriteId].y2 = (var - var2) / 2;
}

// Sets the sprite's y offset equal to the y displacement caused by another sprite
// matrix's scale in the y dimension.
void SetBattlerSpriteYOffsetFromOtherYScale(u8 spriteId, u8 otherSpriteId)
{
    s32 var = 64 - GetBattlerYDeltaFromSpriteId(otherSpriteId) * 2;
    s32 var2 = SAFE_DIV((var << 8), gOamMatrices[gSprites[spriteId].oam.matrixNum].d);

    if (var2 > 128)
        var2 = 128;
    gSprites[spriteId].y2 = (var - var2) / 2;
}

static u16 GetBattlerYDeltaFromSpriteId(u8 spriteId)
{
	struct Pokemon *mon;
    struct BattleSpriteInfo *spriteInfo = gBattleSpritesDataPtr->battlerData;
    u8 battlerId = gSprites[spriteId].data[0];
    u16 species;
    u16 i;

    for (i = 0; i < MAX_BATTLERS_COUNT; ++i)
    {
        if (gBattlerSpriteIds[i] == spriteId)
        {
			mon = GetBattlerPartyIndexPtr(i);
			species = spriteInfo[battlerId].transformSpecies == SPECIES_NONE ? GetMonData(mon, MON_DATA_SPECIES) : spriteInfo[battlerId].transformSpecies;
			
			return GetBattlerSide(i) == B_SIDE_PLAYER ? gMonBackPicCoords[species].y_offset : gMonFrontPicCoords[species].y_offset;
        }
    }
    return 64;
}

void StorePointerInVars(s16 *lo, s16 *hi, const void *ptr)
{
    *lo = ((intptr_t)ptr) & 0xffff;
    *hi = (((intptr_t)ptr) >> 16) & 0xffff;
}

void *LoadPointerFromVars(s16 lo, s16 hi)
{
    return (void *)((u16)lo | ((u16)hi << 16));
}

void BattleAnimHelper_SetSpriteSquashParams(struct Task *task, u8 spriteId, s16 xScale0, s16 yScale0, s16 xScale1, s16 yScale1, u16 duration)
{
    task->data[8] = duration;
    task->data[15] = spriteId;
    task->data[9] = xScale0;
    task->data[10] = yScale0;
    task->data[13] = xScale1;
    task->data[14] = yScale1;
    task->data[11] = (xScale1 - xScale0) / duration;
    task->data[12] = (yScale1 - yScale0) / duration;
}

u8 BattleAnimHelper_RunSpriteSquash(struct Task *task)
{
    if (!task->data[8])
        return 0;
    if (--task->data[8] != 0)
    {
        task->data[9] += task->data[11];
        task->data[10] += task->data[12];
    }
    else
    {
        task->data[9] = task->data[13];
        task->data[10] = task->data[14];
    }
    SetSpriteRotScale(task->data[15], task->data[9], task->data[10], 0);
    if (task->data[8])
        SetBattlerSpriteYOffsetFromYScale(task->data[15]);
    else
        gSprites[task->data[15]].y2 = 0;
    return task->data[8];
}

void AnimTask_GetFrustrationPowerLevel(u8 taskId)
{
    u16 powerLevel;

    if (gAnimFriendship <= 30)
        powerLevel = 0;
    else if (gAnimFriendship <= 100)
        powerLevel = 1;
    else if (gAnimFriendship <= 200)
        powerLevel = 2;
    else
        powerLevel = 3;
    gBattleAnimArgs[7] = powerLevel;
    DestroyAnimVisualTask(taskId);
}

void ResetSpritePriorityOfAllVisibleBattlers(void)
{
    s32 i;

    for (i = 0; i < gBattlersCount; ++i)
    {
        if (IsBattlerSpriteVisible(i))
        {
            gSprites[gBattlerSpriteIds[i]].subpriority = GetBattlerSpriteSubpriority(i);
            gSprites[gBattlerSpriteIds[i]].oam.priority = 2;
        }
    }
}

u8 GetBattlerSpriteSubpriority(u8 battlerId)
{
    u8 subpriority;
    u8 position = GetBattlerPosition(battlerId);

    if (position == B_POSITION_PLAYER_LEFT)
        subpriority = 30;
    else if (position == B_POSITION_PLAYER_RIGHT)
        subpriority = 20;
    else if (position == B_POSITION_OPPONENT_LEFT)
        subpriority = 40;
    else
        subpriority = 50;
    return subpriority;
}

u8 GetBattlerSpriteBGPriority(u8 battlerId)
{
	return GetAnimBgAttribute(GetBattlerSpriteBGPriorityRank(battlerId), BG_ANIM_PRIORITY);
}

u8 GetBattlerSpriteBGPriorityRank(u8 battlerId)
{
    u8 position = GetBattlerPosition(battlerId);

    if (position == B_POSITION_PLAYER_LEFT || position == B_POSITION_OPPONENT_RIGHT)
        return 2;
    else
        return 1;
}

u8 CreateAdditionalMonSpriteForMoveAnim(u16 species, bool8 isBackpic, s16 x, s16 y, u8 subpriority, u32 personality, u32 trainerId)
{
    u16 sheet = LoadSpriteSheet(sMoveAnimAdtlSprSheet);
    u16 palette = AllocSpritePalette(sSpriteTemplate_AdditionalForAnim.paletteTag);

    if (gMonSpritesGfxPtr != NULL && gMonSpritesGfxPtr->multiUseBuffer == NULL)
        gMonSpritesGfxPtr->multiUseBuffer = AllocZeroed(0x2000);
	
	LoadCompressedPalette(GetMonSpritePalFromSpeciesAndPersonality(species, trainerId, personality), (palette * 0x10) + 0x100, 0x20);
	LoadSpecialPokePic(isBackpic ? &gMonBackPicTable[species] : &gMonFrontPicTable[species], gMonSpritesGfxPtr->multiUseBuffer, species, personality, isBackpic ^ TRUE);
    RequestDma3Copy(gMonSpritesGfxPtr->multiUseBuffer, (void *)(OBJ_VRAM0 + (sheet * 0x20)), 0x800, 1);
    FREE_AND_SET_NULL(gMonSpritesGfxPtr->multiUseBuffer);
	y += isBackpic ? gMonBackPicCoords[species].y_offset : gMonFrontPicCoords[species].y_offset;

    return CreateSprite(&sSpriteTemplate_AdditionalForAnim, x, y, subpriority);
}

s16 GetBattlerSpriteCoordAttr(u8 battlerId, u8 attr)
{
    u16 species;
    u32 personality;
    const struct MonCoords *coords;
	struct Pokemon *mon;
    struct BattleSpriteInfo *spriteInfo = gBattleSpritesDataPtr->battlerData;
	
	if (!spriteInfo[battlerId].transformSpecies)
	{
		mon = GetBattlerPartyIndexPtr(battlerId);
		species = GetMonData(mon, MON_DATA_SPECIES);
		personality = GetMonData(mon, MON_DATA_PERSONALITY);
	}
	else
	{
		species = spriteInfo[battlerId].transformSpecies;
		personality = gTransformedPersonalities[battlerId];
	}
	coords = &gMonBackPicCoords[SanitizeSpeciesId(species)];
	
    switch (attr)
    {
    case BATTLER_COORD_ATTR_HEIGHT:
        return (coords->size & 0xf) * 8;
    case BATTLER_COORD_ATTR_WIDTH:
        return (coords->size >> 4) * 8;
    case BATTLER_COORD_ATTR_LEFT:
        return GetBattlerSpriteCoord(battlerId, BATTLER_COORD_X_2) - ((coords->size >> 4) * 4);
    case BATTLER_COORD_ATTR_RIGHT:
        return GetBattlerSpriteCoord(battlerId, BATTLER_COORD_X_2) + ((coords->size >> 4) * 4);
    case BATTLER_COORD_ATTR_TOP:
        return GetBattlerSpriteCoord(battlerId, BATTLER_COORD_Y_PIC_OFFSET) - ((coords->size & 0xf) * 4);
    case BATTLER_COORD_ATTR_BOTTOM:
        return GetBattlerSpriteCoord(battlerId, BATTLER_COORD_Y_PIC_OFFSET) + ((coords->size & 0xf) * 4);
    case BATTLER_COORD_ATTR_RAW_BOTTOM:
        return (GetBattlerSpriteCoord(battlerId, BATTLER_COORD_Y) + 31) - coords->y_offset;
    default:
        return 0;
    }
}

void SetAverageBattlerPositions(u8 battlerId, bool8 respectMonPicOffsets, s16 *x, s16 *y)
{
    u8 xCoordType, yCoordType;
    s16 battlerX, battlerY;
    s16 partnerX, partnerY;

    if (!respectMonPicOffsets)
    {
        xCoordType = BATTLER_COORD_X;
        yCoordType = BATTLER_COORD_Y;
    }
    else
    {
        xCoordType = BATTLER_COORD_X_2;
        yCoordType = BATTLER_COORD_Y_PIC_OFFSET;
    }
    battlerX = GetBattlerSpriteCoord(battlerId, xCoordType);
    battlerY = GetBattlerSpriteCoord(battlerId, yCoordType);
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        partnerX = GetBattlerSpriteCoord(BATTLE_PARTNER(battlerId), xCoordType);
        partnerY = GetBattlerSpriteCoord(BATTLE_PARTNER(battlerId), yCoordType);
    }
    else
    {
        partnerX = battlerX;
        partnerY = battlerY;
    }
    *x = (battlerX + partnerX) / 2;
    *y = (battlerY + partnerY) / 2;
}

u8 CreateCloneOfSpriteInWindowMode(u8 spriteId, s32 species)
{
    u8 newSpriteId = CreateInvisibleSpriteWithCallback(SpriteCallbackDummy);

    gSprites[newSpriteId] = gSprites[spriteId];
    gSprites[newSpriteId].usingSheet = TRUE;
    gSprites[newSpriteId].oam.priority = 0;
    gSprites[newSpriteId].oam.objMode = ST_OAM_OBJ_WINDOW;
    gSprites[newSpriteId].oam.tileNum = gSprites[spriteId].oam.tileNum;
    gSprites[newSpriteId].callback = SpriteCallbackDummy;
    return newSpriteId;
}

void SpriteCB_TrackOffsetFromAttackerAndWaitAnim(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        sprite->x -= gBattleAnimArgs[0];
    else
        sprite->x += gBattleAnimArgs[0];
    sprite->y += gBattleAnimArgs[1];
    sprite->callback = RunStoredCallbackWhenAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void AnimTask_AttackerPunchWithTrace(u8 taskId)
{
    u16 src;
    u16 dest;
    struct Task *task = &gTasks[taskId];

    task->data[0] = GetAnimBattlerSpriteId(ANIM_ATTACKER);
    task->data[1] = ((GetBattlerSide(gBattleAnimAttacker)) != B_SIDE_PLAYER) ? -8 : 8;
    task->data[2] = 0;
    task->data[3] = 0;
    gSprites[task->data[0]].x2 -= task->data[0];
    task->data[4] = AllocSpritePalette(10097);
    task->data[5] = 0;
    dest = (task->data[4] + 0x10) * 0x10;
    src = (gSprites[task->data[0]].oam.paletteNum + 0x10) * 0x10;
    task->data[6] = GetBattlerSpriteSubpriority(gBattleAnimAttacker);
    if (task->data[6] == 20 || task->data[6] == 40)
        task->data[6] = 2;
    else
        task->data[6] = 3;
    CpuCopy32(&gPlttBufferUnfaded[src], &gPlttBufferFaded[dest], 0x20);
    BlendPalette(dest, 16, gBattleAnimArgs[1], gBattleAnimArgs[0]);
    task->func = AnimTask_AttackerPunchWithTrace_Step;
}

static void AnimTask_AttackerPunchWithTrace_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[2])
    {
    case 0:
        PunchAnim_CreateTraceSprite(task, taskId);
        gSprites[task->data[0]].x2 += task->data[1];
        if (++task->data[3] == 5)
        {
            --task->data[3];
            ++task->data[2];
        }
        break;
    case 1:
        PunchAnim_CreateTraceSprite(task, taskId);
        gSprites[task->data[0]].x2 -= task->data[1];
        if (--task->data[3] == 0)
        {
            gSprites[task->data[0]].x2 = 0;
            ++task->data[2];
        }
        break;
    case 2:
        if (!task->data[5])
        {
            FreeSpritePaletteByTag(ANIM_TAG_BENT_SPOON);
            DestroyAnimVisualTask(taskId);
        }
        break;
    }
}

static void PunchAnim_CreateTraceSprite(struct Task *task, u8 taskId)
{
    s16 spriteId = CloneBattlerSpriteWithBlend(0);

    if (spriteId >= 0)
    {
        gSprites[spriteId].oam.priority = task->data[6];
        gSprites[spriteId].oam.paletteNum = task->data[4];
        gSprites[spriteId].data[0] = 8;
        gSprites[spriteId].data[1] = taskId;
        gSprites[spriteId].data[2] = spriteId;
        gSprites[spriteId].x2 = gSprites[task->data[0]].x2;
        gSprites[spriteId].callback = SpriteCB_PunchTrace;
        ++task->data[5];
    }
}

static void SpriteCB_PunchTrace(struct Sprite *sprite)
{
    if (--sprite->data[0] == 0)
    {
        --gTasks[sprite->data[1]].data[5];
        obj_delete_but_dont_free_vram(sprite);
    }
}

void SpriteCB_WeatherBallUp(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
        sprite->data[0] = 5;
    else
        sprite->data[0] = -10;
    sprite->data[1] = -40;
    sprite->callback = SpriteCB_WeatherBallUp_Step;
}

static void SpriteCB_WeatherBallUp_Step(struct Sprite *sprite)
{
    sprite->data[2] += sprite->data[0];
    sprite->data[3] += sprite->data[1];
    sprite->x2 = sprite->data[2] / 10;
    sprite->y2 = sprite->data[3] / 10;
    if (sprite->data[1] < -20)
        ++sprite->data[1];
    if (sprite->y + sprite->y2 < -32)
        DestroyAnimSprite(sprite);
}

void AnimWeatherBallDown(struct Sprite *sprite)
{
    s32 x;

    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[2] = sprite->x + gBattleAnimArgs[4];
    sprite->data[4] = sprite->y + gBattleAnimArgs[5];
    if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_PLAYER)
    {
        x = (u16)gBattleAnimArgs[4] + 30;
        sprite->x += x;
        sprite->y = gBattleAnimArgs[5] - 20;
    }
    else
    {
        x = (u16)gBattleAnimArgs[4] - 30;
        sprite->x += x;
        sprite->y = gBattleAnimArgs[5] - 80;
    }
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}
