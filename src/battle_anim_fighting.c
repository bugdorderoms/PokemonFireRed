#include "global.h"
#include "gflib.h"
#include "battle_anim.h"
#include "random.h"
#include "task.h"
#include "trig.h"

static void AnimSlideHandOrFootToTarget(struct Sprite *sprite);
static void AnimBasicFistOrFoot(struct Sprite *sprite);
static void AnimKarateChopHand(struct Sprite *sprite);
static void AnimFistOrFootRandomPos(struct Sprite *sprite);
static void AnimCrossChopHand(struct Sprite *sprite);
static void AnimSlidingKick(struct Sprite *sprite);
static void AnimSpinningKickOrPunch(struct Sprite *sprite);
static void AnimStompFoot(struct Sprite *sprite);
static void AnimDizzyPunchDuck(struct Sprite *sprite);
static void AnimBrickBreakWall(struct Sprite *sprite);
static void AnimBrickBreakWallShard(struct Sprite *sprite);
static void AnimSuperpowerOrb(struct Sprite *sprite);
static void AnimSuperpowerRock(struct Sprite *sprite);
static void AnimSuperpowerFireball(struct Sprite *sprite);
static void AnimArmThrustHit(struct Sprite *sprite);
static void AnimRevengeScratch(struct Sprite *sprite);
static void AnimFocusPunchFist(struct Sprite *sprite);
static void sub_80B0B2C(struct Sprite *sprite);
static void sub_80B0BD8(struct Sprite *sprite);
static void sub_80B0CB4(struct Sprite *sprite);
static void AnimSpinningKickOrPunchFinish(struct Sprite *sprite);
static void AnimStompFootStep(struct Sprite *sprite);
static void AnimStompFootEnd(struct Sprite *sprite);
static void sub_80B0EF0(struct Sprite *sprite);
static void sub_80B1050(struct Sprite *sprite);
static void sub_80B111C(struct Sprite *sprite);
static void sub_80B11E4(struct Sprite *sprite);
static void sub_80B12A4(struct Sprite *sprite);
static void AnimSlapSwipesHand(struct Sprite *sprite);

static const union AnimCmd sAnim_HandOrFoot[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_SlidingKick_0[] =
{
    ANIMCMD_FRAME(16, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_SlidingKick_1[] =
{
    ANIMCMD_FRAME(32, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CrossChopHand_0[] =
{
    ANIMCMD_FRAME(48, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_CrossChopHand_1[] =
{
    ANIMCMD_FRAME(48, 1, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_HandOrFoot[] =
{
    sAnim_HandOrFoot,
};

static const union AnimCmd *const sAnims_SlidingKick[] =
{
    sAnim_SlidingKick_0,
    sAnim_SlidingKick_1,
};

static const union AnimCmd *const sAnims_CrossChopHand[] =
{
    sAnim_CrossChopHand_0,
    sAnim_CrossChopHand_1,
};

const struct SpriteTemplate gJumpKickSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_HandOrFoot,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSlideHandOrFootToTarget,
};

const struct SpriteTemplate gFistFootSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_HandOrFoot,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBasicFistOrFoot,
};

const struct SpriteTemplate gFistFootRandomPosSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_HandOrFoot,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFistOrFootRandomPos,
};

const struct SpriteTemplate gCrossChopHandSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_CrossChopHand,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimCrossChopHand,
};

const struct SpriteTemplate gSlidingKickSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_SlidingKick,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSlidingKick,
};

static const union AnimCmd sAnim_KarateHandLeft[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_KarateHandRight[] =
{
    ANIMCMD_FRAME(0, 1, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_KarateHand[] =
{
	sAnim_KarateHandLeft,
    sAnim_KarateHandRight
};

const struct SpriteTemplate gKarateHandSpriteTemplate =
{
    .tileTag = ANIM_TAG_KARATE_HAND,
    .paletteTag = ANIM_TAG_KARATE_HAND,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_KarateHand,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimKarateChopHand,
};

static const union AffineAnimCmd sAffineAnim_SpinningHandOrFoot[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(-0x8, -0x8, 20, 1),
    AFFINEANIMCMD_JUMP(1),
};

static const union AffineAnimCmd *const sAffineAnims_SpinningHandOrFoot[] =
{
    sAffineAnim_SpinningHandOrFoot,
};

const struct SpriteTemplate gSpinningHandOrFootSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineDouble_ObjNormal_32x32,
    .anims = sAnims_HandOrFoot,
    .images = NULL,
    .affineAnims = sAffineAnims_SpinningHandOrFoot,
    .callback = AnimSpinningKickOrPunch,
};

static const union AffineAnimCmd sAffineAnim_MegaPunchKick[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(-0x4, -0x4, 20, 1),
    AFFINEANIMCMD_JUMP(1),
};

static const union AffineAnimCmd *const sAffineAnims_MegaPunchKick[] =
{
    sAffineAnim_MegaPunchKick,
};

const struct SpriteTemplate gMegaPunchKickSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineDouble_ObjNormal_32x32,
    .anims = sAnims_HandOrFoot,
    .images = NULL,
    .affineAnims = sAffineAnims_MegaPunchKick,
    .callback = AnimSpinningKickOrPunch,
};

const struct SpriteTemplate gStompFootSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_SlidingKick,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimStompFoot,
};

const struct SpriteTemplate gDizzyPunchDuckSpriteTemplate =
{
    .tileTag = ANIM_TAG_DUCK,
    .paletteTag = ANIM_TAG_DUCK,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimDizzyPunchDuck,
};

const struct SpriteTemplate gBrickBreakWallSpriteTemplate =
{
    .tileTag = ANIM_TAG_BLUE_LIGHT_WALL,
    .paletteTag = ANIM_TAG_BLUE_LIGHT_WALL,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBrickBreakWall,
};

const struct SpriteTemplate gBrickBreakWallShardSpriteTemplate =
{
    .tileTag = ANIM_TAG_TORN_METAL,
    .paletteTag = ANIM_TAG_TORN_METAL,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBrickBreakWallShard,
};

static const union AffineAnimCmd sAffineAnim_SuperpowerOrb[] =
{
    AFFINEANIMCMD_FRAME(0x20, 0x20, 0, 0),
    AFFINEANIMCMD_FRAME(0x4, 0x4, 0, 64),
    AFFINEANIMCMD_FRAME(-0x6, -0x6, 0, 8),
    AFFINEANIMCMD_FRAME(0x6, 0x6, 0, 8),
    AFFINEANIMCMD_JUMP(2),
};

static const union AffineAnimCmd *const sAffineAnims_SuperpowerOrb[] =
{
    sAffineAnim_SuperpowerOrb,
};

const struct SpriteTemplate gSuperpowerOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_CIRCLE_OF_LIGHT,
    .paletteTag = ANIM_TAG_CIRCLE_OF_LIGHT,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_SuperpowerOrb,
    .callback = AnimSuperpowerOrb,
};

const struct SpriteTemplate gSuperpowerRockSpriteTemplate =
{
    .tileTag = ANIM_TAG_FLAT_ROCK,
    .paletteTag = ANIM_TAG_FLAT_ROCK,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSuperpowerRock,
};

const struct SpriteTemplate gSuperpowerFireballSpriteTemplate =
{
    .tileTag = ANIM_TAG_METEOR,
    .paletteTag = ANIM_TAG_METEOR,
    .oam = &gOamData_AffineOff_ObjNormal_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSuperpowerFireball,
};

const struct SpriteTemplate gArmThrustHandSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_HandOrFoot,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimArmThrustHit,
};

static const union AnimCmd sAnim_RevengeSmallScratch_0[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RevengeSmallScratch_1[] =
{
    ANIMCMD_FRAME(0, 4, .vFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .vFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .vFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RevengeSmallScratch_2[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_RevengeSmallScratch[] =
{
    sAnim_RevengeSmallScratch_0,
    sAnim_RevengeSmallScratch_1,
    sAnim_RevengeSmallScratch_2,
};

const struct SpriteTemplate gRevengeSmallScratchSpriteTemplate =
{
    .tileTag = ANIM_TAG_PURPLE_SCRATCH,
    .paletteTag = ANIM_TAG_PURPLE_SCRATCH,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_RevengeSmallScratch,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimRevengeScratch,
};

static const union AnimCmd sAnim_RevengeBigScratch_0[] =
{
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(64, 6),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RevengeBigScratch_1[] =
{
    ANIMCMD_FRAME(0, 6, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(64, 6, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_RevengeBigScratch_2[] =
{
    ANIMCMD_FRAME(0, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(64, 6, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_RevengeBigScratch[] =
{
    sAnim_RevengeBigScratch_0,
    sAnim_RevengeBigScratch_1,
    sAnim_RevengeBigScratch_2,
};

const struct SpriteTemplate gRevengeBigScratchSpriteTemplate =
{
    .tileTag = ANIM_TAG_PURPLE_SWIPE,
    .paletteTag = ANIM_TAG_PURPLE_SWIPE,
    .oam = &gOamData_AffineOff_ObjNormal_64x64,
    .anims = sAnims_RevengeBigScratch,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimRevengeScratch,
};

static const union AffineAnimCmd sAffineAnim_FocusPunchFist[] =
{
    AFFINEANIMCMD_FRAME(0x200, 0x200, 0, 0),
    AFFINEANIMCMD_FRAME(-0x20, -0x20, 0, 8),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_FocusPunchFist[] =
{
    sAffineAnim_FocusPunchFist,
};

const struct SpriteTemplate gFocusPunchFistSpriteTemplate =
{
    .tileTag = ANIM_TAG_HANDS_AND_FEET,
    .paletteTag = ANIM_TAG_HANDS_AND_FEET,
    .oam = &gOamData_AffineDouble_ObjNormal_32x32,
    .anims = sAnims_HandOrFoot,
    .images = NULL,
    .affineAnims = sAffineAnims_FocusPunchFist,
    .callback = AnimFocusPunchFist,
};

static const union AffineAnimCmd sAffineAnim_DoubleSlapSwipe[] =
{
	AFFINEANIMCMD_FRAME(0, 0, 0, 3), // Pause for 3 frames
	AFFINEANIMCMD_FRAME(-28, 0, 0, 8), // Flatten vertically (on its side)
	AFFINEANIMCMD_FRAME(0, 0, 0, 11), // Pause for 11 frames
	AFFINEANIMCMD_FRAME(-288, 0, 0, 1), // Unflatten in other direction
	AFFINEANIMCMD_FRAME(0, 0, 0, 2), // Pause for 2 frames
	AFFINEANIMCMD_FRAME(28, 0, 0, 8), // Flatten vertically (on its side)
	AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_SlapSwipes[] =
{
    sAffineAnim_DoubleSlapSwipe,
};

const struct SpriteTemplate gDoubleSlapSpriteTemplate =
{
	.tileTag = ANIM_TAG_KARATE_HAND,
    .paletteTag = ANIM_TAG_KARATE_HAND,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_SlapSwipes,
    .callback = AnimSlapSwipesHand,
};

static void AnimSlideHandOrFootToTarget(struct Sprite *sprite)
{
    if (gBattleAnimArgs[7] == 1 && GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        gBattleAnimArgs[1] = -gBattleAnimArgs[1];
        gBattleAnimArgs[3] = -gBattleAnimArgs[3];
    }
    StartSpriteAnim(sprite, gBattleAnimArgs[6]);
    gBattleAnimArgs[6] = 0;
    AnimSnoreZ(sprite);
}

// Displays a basic fist or foot sprite for a given duration.
// Used by many fighting moves (and elemental "punch" moves).
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: duration
// arg 3: anim battler
// arg 4: anim num
static void AnimBasicFistOrFoot(struct Sprite *sprite)
{
    StartSpriteAnim(sprite, gBattleAnimArgs[4]);
	InitSpritePosToAnimBattler(sprite, gBattleAnimArgs[3], TRUE);
	sprite->data[0] = gBattleAnimArgs[2];
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// Creadits: Blackuser
// Animates a karate hand that slides downward.
// It flips horizontally depending on what side and poke are using it.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: initial wait duration
// arg 3: final y pixel slide
static void AnimKarateChopHand(struct Sprite *sprite)
{
	bool8 hFlip;
	u8 defSide = GetBattlerSide(gBattleAnimTarget);
	
	if (GetBattlerSide(gBattleAnimAttacker) != defSide)
		hFlip = (defSide != B_SIDE_PLAYER);
	else
		hFlip = ((GetBattlerPosition(gBattleAnimTarget) & BIT_FLANK) != B_FLANK_LEFT);
	
	StartSpriteAnim(sprite, hFlip);
	AnimStompFoot(sprite);
}

static void AnimFistOrFootRandomPos(struct Sprite *sprite)
{
    u8 battler = gBattleAnimArgs[0] == 0 ? gBattleAnimAttacker : gBattleAnimTarget;
    s16 x, y;

    if (gBattleAnimArgs[2] < 0)
        gBattleAnimArgs[2] = RandomMax(5);
    StartSpriteAnim(sprite, gBattleAnimArgs[2]);
    sprite->x = GetBattlerSpriteCoord(battler, 2);
    sprite->y = GetBattlerSpriteCoord(battler, 3);

    x = RandomMax(GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_WIDTH) / 2);
    y = RandomMax(GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_HEIGHT) / 4);
	
    if (RandomMax(2))
        x *= -1;
    if (RandomMax(2))
        y *= -1;
	
    if ((gBattlerPositions[battler] & BIT_SIDE) == B_SIDE_PLAYER)
        y += 0xFFF0;
	
    sprite->x += x;
    sprite->y += y;
	
    sprite->data[0] = gBattleAnimArgs[1];
    sprite->data[7] = CreateSprite(&gBasicHitSplatSpriteTemplate, sprite->x, sprite->y, sprite->subpriority + 1);
	
    if (sprite->data[7] != 64)
    {
        StartSpriteAffineAnim(&gSprites[sprite->data[7]], 0);
        gSprites[sprite->data[7]].callback = SpriteCallbackDummy;
    }
    sprite->callback = sub_80B0B2C;
}

static void sub_80B0B2C(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        if (sprite->data[7] != 64)
        {
            FreeOamMatrix(gSprites[sprite->data[7]].oam.matrixNum);
            DestroySprite(&gSprites[sprite->data[7]]);
        }
        DestroyAnimSprite(sprite);
    }
    else
        --sprite->data[0];
}

static void AnimCrossChopHand(struct Sprite *sprite)
{
    InitSpritePosToAnimTarget(sprite, TRUE);
    sprite->data[0] = 30;
    if (gBattleAnimArgs[2] == 0)
    {
        sprite->data[2] = sprite->x - 20;
    }
    else
    {
        sprite->data[2] = sprite->x + 20;
        sprite->hFlip = TRUE;
    }
    sprite->data[4] = sprite->y - 20;
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, sub_80B0BD8);
}

static void sub_80B0BD8(struct Sprite *sprite)
{
    if (++sprite->data[5] == 11)
    {
        sprite->data[2] = sprite->x - sprite->x2;
        sprite->data[4] = sprite->y - sprite->y2;
        sprite->data[0] = 8;
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->y2 = 0;
        sprite->x2 = 0;
        sprite->callback = StartAnimLinearTranslation;
        StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    }
}

static void AnimSlidingKick(struct Sprite *sprite)
{
    if (BATTLE_PARTNER(gBattleAnimAttacker) == gBattleAnimTarget && GetBattlerPosition(gBattleAnimTarget) < B_POSITION_PLAYER_RIGHT)
        gBattleAnimArgs[0] *= -1;
    InitSpritePosToAnimTarget(sprite, TRUE);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    sprite->data[0] = gBattleAnimArgs[3];
    sprite->data[1] = sprite->x;
    sprite->data[2] = sprite->x + gBattleAnimArgs[2];
    sprite->data[3] = sprite->y;
    sprite->data[4] = sprite->y;
    InitAnimLinearTranslation(sprite);
    sprite->data[5] = gBattleAnimArgs[5];
    sprite->data[6] = gBattleAnimArgs[4];
    sprite->data[7] = 0;
    sprite->callback = sub_80B0CB4;
}

static void sub_80B0CB4(struct Sprite *sprite)
{
    if (!AnimTranslateLinear(sprite))
    {
        sprite->y2 += Sin(sprite->data[7] >> 8, sprite->data[5]);
        sprite->data[7] += sprite->data[6];
    }
    else
    {
        DestroyAnimSprite(sprite);
    }
}

// Animates the spinning, shrinking kick or punch, which then reappears at full size.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: anim num
// arg 3: spin duration
static void AnimSpinningKickOrPunch(struct Sprite *sprite)
{
    InitSpritePosToAnimTarget(sprite, TRUE);
    StartSpriteAnim(sprite, gBattleAnimArgs[2]);
    sprite->data[0] = gBattleAnimArgs[3];
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, AnimSpinningKickOrPunchFinish);
}

static void AnimSpinningKickOrPunchFinish(struct Sprite *sprite)
{
    StartSpriteAffineAnim(sprite, 0);
    sprite->affineAnimPaused = TRUE;
    sprite->data[0] = 20;
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// Animates MOVE_STOMP's foot that slides downward.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: initial wait duration
// arg 3: final y pixel slide
static void AnimStompFoot(struct Sprite *sprite)
{
    InitSpritePosToAnimTarget(sprite, TRUE);
    sprite->data[0] = gBattleAnimArgs[2];
	sprite->data[4] = gBattleAnimArgs[3];
    sprite->callback = AnimStompFootStep;
}

static void AnimStompFootStep(struct Sprite *sprite)
{
    if (--sprite->data[0] == -1)
    {
        sprite->data[0] = 6;
        sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
        sprite->data[4] += GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET);
        sprite->callback = StartAnimLinearTranslation;
		StoreSpriteCallbackInData6(sprite, AnimStompFootEnd);
    }
}

static void AnimStompFootEnd(struct Sprite *sprite)
{
    sprite->data[0] = 15;
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

static void AnimDizzyPunchDuck(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        InitSpritePosToAnimTarget(sprite, TRUE);
        sprite->data[1] = gBattleAnimArgs[2];
        sprite->data[2] = gBattleAnimArgs[3];
        ++sprite->data[0];
    }
    else
    {
        sprite->data[4] += sprite->data[1];
        sprite->x2 = sprite->data[4] >> 8;
        sprite->y2 = Sin(sprite->data[3], sprite->data[2]);
        sprite->data[3] = (sprite->data[3] + 3) & 0xFF;
        if (sprite->data[3] > 100)
            sprite->invisible = sprite->data[3] % 2;
        if (sprite->data[3] > 120)
            DestroyAnimSprite(sprite);
    }
}

static void AnimBrickBreakWall(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 0);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 1);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 0);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 1);
    }
    sprite->x += gBattleAnimArgs[1];
    sprite->y += gBattleAnimArgs[2];
    sprite->data[0] = 0;
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[4];
    sprite->data[3] = 0;
    sprite->callback = sub_80B0EF0;
}

static void sub_80B0EF0(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        if (--sprite->data[1] == 0)
        {
            if (sprite->data[2] == 0)
                DestroyAnimSprite(sprite);
            else
                ++sprite->data[0];
        }
        break;
    case 1:
        if (++sprite->data[1] > 1)
        {
            sprite->data[1] = 0;
            ++sprite->data[3];
			sprite->x2 = (sprite->data[3] & 1) ? 2 : -2;
        }
        if (--sprite->data[2] == 0)
            DestroyAnimSprite(sprite);
        break;
    }
}

static void AnimBrickBreakWallShard(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 0) + gBattleAnimArgs[2];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 1) + gBattleAnimArgs[3];
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 0) + gBattleAnimArgs[2];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 1) + gBattleAnimArgs[3];
    }
    sprite->oam.tileNum += gBattleAnimArgs[1] * 16;
    sprite->data[0] = 0;
    switch (gBattleAnimArgs[1])
    {
    case 0:
        sprite->data[6] = -3;
        sprite->data[7] = -3;
        break;
    case 1:
        sprite->data[6] = 3;
        sprite->data[7] = -3;
        break;
    case 2:
        sprite->data[6] = -3;
        sprite->data[7] = 3;
        break;
    case 3:
        sprite->data[6] = 3;
        sprite->data[7] = 3;
        break;
    default:
        DestroyAnimSprite(sprite);
        return;
    }
    sprite->callback = sub_80B1050;
}

static void sub_80B1050(struct Sprite *sprite)
{
    sprite->x += sprite->data[6];
    sprite->y += sprite->data[7];
    if (++sprite->data[0] > 40)
        DestroyAnimSprite(sprite);
}

static void AnimSuperpowerOrb(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattlerAttacker, 2);
        sprite->y = GetBattlerSpriteCoord(gBattlerAttacker, 3);
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimAttacker);
        sprite->data[7] = gBattleAnimTarget;
    }
    else
    {
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimTarget);
        sprite->data[7] = gBattleAnimAttacker;
    }
    sprite->data[0] = 0;
    sprite->data[1] = 12;
    sprite->data[2] = 8;
    sprite->callback = sub_80B111C;
}

static void sub_80B111C(struct Sprite *sprite)
{
    if (++sprite->data[0] == 180)
    {
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        sprite->data[0] = 16;
        sprite->data[1] = sprite->x;
        sprite->data[2] = GetBattlerSpriteCoord(sprite->data[7], 2);
        sprite->data[3] = sprite->y;
        sprite->data[4] = GetBattlerSpriteCoord(sprite->data[7], 3);
        InitAnimLinearTranslation(sprite);
        StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
        sprite->callback = RunLinearTranslation_ThenceSetCBtoStoredInData6;
    }
}

static void AnimSuperpowerRock(struct Sprite *sprite)
{
    sprite->x = gBattleAnimArgs[0];
    sprite->y = 120;
    sprite->data[0] = gBattleAnimArgs[3];
    StorePointerInVars(&sprite->data[4], &sprite->data[5], (void *)(sprite->y << 8));
    sprite->data[6] = gBattleAnimArgs[1];
    sprite->oam.tileNum += gBattleAnimArgs[2] * 4;
    sprite->callback = sub_80B11E4;
}

static void sub_80B11E4(struct Sprite *sprite)
{
    void *var0;

    if (sprite->data[0] != 0)
    {
        var0 = LoadPointerFromVars(sprite->data[4], sprite->data[5]);
        var0 -= sprite->data[6];
        StorePointerInVars(&sprite->data[4], &sprite->data[5], var0);
        var0 = (void *)(((intptr_t)var0) >> 8);
        sprite->y = (intptr_t)var0;
        if (sprite->y < -8)
            DestroyAnimSprite(sprite);
        else
            --sprite->data[0];
    }
    else
    {
        sprite->data[0] = GetBattlerSpriteCoord(gBattleAnimTarget, 2) - GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
        sprite->data[1] = GetBattlerSpriteCoord(gBattleAnimTarget, 3) - GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
        sprite->data[2] = sprite->x << 4;
        sprite->data[3] = sprite->y << 4;
        sprite->callback = sub_80B12A4;
    }
}

static void sub_80B12A4(struct Sprite *sprite)
{
    sprite->data[2] += sprite->data[0];
    sprite->data[3] += sprite->data[1];
    sprite->x = sprite->data[2] >> 4;
    sprite->y = sprite->data[3] >> 4;

    if ((u16)sprite->x + 8 > 256 || sprite->y < -8 || sprite->y > 120)
        DestroyAnimSprite(sprite);
}

static void AnimSuperpowerFireball(struct Sprite *sprite)
{
    u8 battler;

    if (gBattleAnimArgs[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattlerAttacker, 2);
        sprite->y = GetBattlerSpriteCoord(gBattlerAttacker, 3);
        battler = gBattleAnimTarget;
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimAttacker);
    }
    else
    {
        battler = gBattleAnimAttacker;
        sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimTarget);
    }
    if (GetBattlerSide(battler) == B_SIDE_PLAYER)
        sprite->oam.matrixNum |= (ST_OAM_HFLIP | ST_OAM_VFLIP);
    sprite->data[0] = 16;
    sprite->data[1] = sprite->x;
    sprite->data[2] = GetBattlerSpriteCoord(battler, 2);
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(battler, 3);
    InitAnimLinearTranslation(sprite);
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = RunLinearTranslation_ThenceSetCBtoStoredInData6;
}

static void sub_80B13D4(struct Sprite *sprite)
{
    if (sprite->data[0] == sprite->data[4])
        DestroyAnimSprite(sprite);
    ++sprite->data[0];
}

static void AnimArmThrustHit(struct Sprite *sprite)
{
    u8 turn;

    sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[0];
    sprite->data[3] = gBattleAnimArgs[1];
    sprite->data[4] = gBattleAnimArgs[2];
    turn = gAnimMoveTurn;
    if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_PLAYER)
        ++turn;
    if (turn & 1)
    {
        sprite->data[2] = -sprite->data[2];
        ++sprite->data[1];
    }
    StartSpriteAnim(sprite, sprite->data[1]);
    sprite->x2 = sprite->data[2];
    sprite->y2 = sprite->data[3];
    sprite->callback = sub_80B13D4;
}

static void AnimRevengeScratch(struct Sprite *sprite)
{
    if (gBattleAnimArgs[2] == 0)
        InitSpritePosToAnimAttacker(sprite, 0);
    else
        InitSpritePosToAnimTarget(sprite, FALSE);
	
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        StartSpriteAnim(sprite, 1);
    sprite->callback = RunStoredCallbackWhenAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

static void AnimFocusPunchFist(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
    {
        sprite->data[1] = (sprite->data[1] + 40) & 0xFF;
        sprite->x2 = Sin(sprite->data[1], 2);
        if (++sprite->data[0] > 40)
            DestroyAnimSprite(sprite);
    }
}

void AnimTask_MoveSkyUppercutBg(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        ToggleBg3Mode(0);
        task->data[8] = gBattleAnimArgs[0];
        ++task->data[0];
        break;
    case 1:
        if (--task->data[8] == -1)
            ++task->data[0];
        break;
    case 2:
    default:
        task->data[9] += 1280;
        break;
    }
    task->data[10] += 2816;
    if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_PLAYER)
        gBattle_BG3_X += task->data[9] >> 8;
    else
        gBattle_BG3_X -= task->data[9] >> 8;
    gBattle_BG3_Y += task->data[10] >> 8;
    task->data[9] &= 0xFF;
    task->data[10] &= 0xFF;
    if (gBattleAnimArgs[7] == -1)
    {
        gBattle_BG3_X = 0;
        gBattle_BG3_Y = 0;
        ToggleBg3Mode(1);
        DestroyAnimVisualTask(taskId);
    }
}

// Creadits: Skeli and Blackuser
// Creates a sprite that moves right then then along the target.
// arg 0: swipe distance
// arg 1: speed
// arg 2: num swipes
// arg 3: affine anim num
static void AnimSlapSwipesHand(struct Sprite *sprite)
{
	switch (sprite->data[3])
	{
		case 0:
			StartSpriteAffineAnim(sprite, gBattleAnimArgs[3]);
			sprite->x2 = gBattleAnimArgs[0];
			sprite->data[0] = -gBattleAnimArgs[0];
			sprite->data[1] = gBattleAnimArgs[1];
			sprite->data[2] = gBattleAnimArgs[2];
			++sprite->data[3];
			break;
		case 1: // Right swipes
			sprite->x2 += sprite->data[1];
			
			if (sprite->x2 >= sprite->data[0])
			{
				sprite->data[0] = -sprite->data[0];
				++sprite->data[3];
				--sprite->data[2];
			}
			break;
		case 2: // Left swipes
			sprite->x2 -= sprite->data[1];
			
			if (sprite->x2 <= sprite->data[0])
			{
				sprite->data[0] = -sprite->data[0];
				
				if (sprite->data[2] == 0)
					sprite->data[3] = 5;
				else
					--sprite->data[3];
			}
			break;
		case 5:
			DestroyAnimSprite(sprite);
			break;
	}
}
