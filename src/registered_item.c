#include "global.h"
#include "battle_anim.h"
#include "event_object_lock.h"
#include "event_object_movement.h"
#include "graphics.h"
#include "item.h"
#include "item_menu.h"
#include "item_menu_icons.h"
#include "list_menu.h"
#include "menu_indicators.h"
#include "overworld.h"
#include "registered_item.h"
#include "sprite.h"
#include "script.h"
#include "sound.h"
#include "task.h"
#include "constants/items.h"
#include "constants/songs.h"

/* cursor directions */
enum
{
	CURSORDIRECTION_UP,
	CURSORDIRECTION_RIGT,
	CURSORDIRECTION_DOWN,
	CURSORDIRECTION_LEFT,
	NUM_CURSORDIRECTIONS
};

// number of times that is added X and Y to the box position when starting slot swapping
#define SLIDE_FRAMES_COUNT 10

// this file's functions
static void SpriteCB_WaitAffinAnimsAndInitCursor(struct Sprite *sprite);
static void SpriteCB_WaitAffinAnimsAndSwitchItems(struct Sprite *sprite);
static void SpriteCB_WaitAffinAnimsAndReturnToInput(struct Sprite *sprite);
static void SpriteCB_WaitSlideAnimAndReturnToInput(struct Sprite *sprite);
static void SpriteCB_SelectItem_HandleInput(struct Sprite *sprite);
static void SpriteCB_MoveItem_HandleInput(struct Sprite *sprite);
static void SpriteCB_HandleUseSelectedItem(struct Sprite *sprite);
static void SpriteCB_MoveSpriteToSwitchMode(struct Sprite *sprite);
static u8 DirectionToAnimNum(u8 direction);
static u8 GetItemSpriteIdByDirection(struct Sprite *sprite, u8 direction, bool8 GetOriginal);
static void SetDataToSlideAnim(struct Sprite *sprite, u8 direction);
static void GeneralCursorMovement(struct Sprite *sprite);
static void HandleSelectItemAction(struct Sprite *sprite, u8 direction);
static void InitAffinAnimsAndUseItem(struct Sprite *sprite);

// X and Y positions of each box
static const struct UCoords16 sRegisteredBoxesIconsPositions[REGISTERED_ITEMS_COUNT] =
{
	{ // up box
		.x = 120,
		.y = 30,
	},
	{ // right box
		.x = 170, 
		.y = 80,
	},
	{ // down box
		.x = 120,
		.y = 130,
	},
	{ // left box
		.x = 70,
		.y = 80,
	},
};

// X And Y Positions of each cursor direction
static const struct UCoords16 sCursorPositions[NUM_CURSORDIRECTIONS] =
{
	[CURSORDIRECTION_UP] = {
		.x = 120, 
		.y = 52,
	},
	[CURSORDIRECTION_RIGT] = {
		.x = 147,
		.y = 80,
	},
	[CURSORDIRECTION_DOWN] = {
		.x = 119,
		.y = 107,
	},
	[CURSORDIRECTION_LEFT] = {
		.x = 92,
		.y = 79,
	},
};

static const struct SpriteSheet sBoxSpriteSheet = 
{
	.data = gRegisteredItemBoxSpriteGfx,
	.size = 0x200,
	.tag = BOX_TAG,
};

static const struct SpritePalette sBoxAndCursorSpritePalette = 
{
	.data = gRegisteredItemBoxSpritePal,
	.tag = BOX_TAG,
};

static const struct SpriteSheet sCursorSpriteSheet = 
{
	.data = gRegisteredItemCursorSpriteGfx,
	.size = 0x100,
	.tag = CURSOR_TAG,
};

static const union AffineAnimCmd sAffineAnim_RegisteredItemLoad[] = {
	AFFINEANIMCMD_FRAME(16, 16, 0, 0),
	AFFINEANIMCMD_FRAME(16, 16, 0, 15),
	AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_RegisteredItemUnload[] = {
	AFFINEANIMCMD_FRAME(-16, -16, 0, 15),
	AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_RegisteredItemCursorUp[] = {
	AFFINEANIMCMD_FRAME(0, 0, 0x00, 1),
	AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_RegisteredItemCursorDown[] = {
	AFFINEANIMCMD_FRAME(0, 0, 0x80, 1),
	AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_RegisteredItemCursorRight[] = {
	AFFINEANIMCMD_FRAME(0, 0, 0xC0, 1),
	AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_RegisteredItemCursorLeft[] = {
	AFFINEANIMCMD_FRAME(0, 0, 0x40, 1),
	AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnimTable_RegisteredItem[] = {
	sAffineAnim_RegisteredItemLoad,
	sAffineAnim_RegisteredItemUnload
};

static const union AffineAnimCmd *const sAffineAnimTable_RegisteredItemCursor[] = {
	sAffineAnim_RegisteredItemCursorUp,
	sAffineAnim_RegisteredItemCursorDown,
	sAffineAnim_RegisteredItemCursorRight,
	sAffineAnim_RegisteredItemCursorLeft
};

static const struct OamData sOamData_RegisteredItem =
{
	.y = 0,
	.affineMode = ST_OAM_AFFINE_NORMAL,
	.objMode = ST_OAM_OBJ_NORMAL,
	.bpp = ST_OAM_4BPP,
	.shape = SPRITE_SHAPE(32x32),
	.x = 0,
	.size = SPRITE_SIZE(32x32),
	.tileNum = 0,
	.priority = 0,
	.paletteNum = 0,
};

static const struct OamData sOamData_RegisteredItemCursor =
{
	.y = 0,
	.affineMode = ST_OAM_AFFINE_NORMAL,
	.objMode = ST_OAM_OBJ_NORMAL,
	.bpp = ST_OAM_4BPP,
	.shape = SPRITE_SHAPE(16x16),
	.x = 0,
	.size = SPRITE_SIZE(16x16),
	.tileNum = 0,
	.priority = 0,
	.paletteNum = 0,
};

static const struct SpriteTemplate sSelectItem_Cursor =
{
	.tileTag = CURSOR_TAG,
	.paletteTag = BOX_TAG,
	.oam = &sOamData_RegisteredItemCursor,
	.anims = gDummySpriteAnimTable,
	.images = NULL,
	.affineAnims = sAffineAnimTable_RegisteredItemCursor,
	.callback = SpriteCB_WaitAffinAnimsAndInitCursor
};
	
static const struct SpriteTemplate sBoxTemplate = 
{
	.tileTag = BOX_TAG,
	.paletteTag = BOX_TAG,
	.oam = &sOamData_RegisteredItem,
	.anims = gDummySpriteAnimTable,
	.images = NULL,
	.affineAnims = sAffineAnimTable_RegisteredItem,
	.callback = SpriteCallbackDummy
};

// functions that makes the game load more than 1 registered item
bool8 IsItemRegistered(u16 itemId)
{
	u8 i;
	
	for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
	{
		if (gSaveBlock1Ptr->registeredItem[i] == itemId)
			return TRUE;
	}
	return FALSE;
}

bool8 IsAllRegisteredItemSlotsFree(void)
{
	u8 i;

	TryRemoveRegisteredItems();
	
	for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
	{
		if (gSaveBlock1Ptr->registeredItem[i])
			return FALSE;
	}
	return TRUE;
}

u8 FindFirstFreeSlot(void)
{
	u8 i;
	
	for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
	{
		if (!gSaveBlock1Ptr->registeredItem[i])
			return i;
	}
	return REGISTERED_ITEMS_COUNT;
}

u8 FindRegisteredItemSlot(u16 itemId)
{
	u8 i;
	
	for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
	{
		if (gSaveBlock1Ptr->registeredItem[i] == itemId)
			return i;
	}
	return REGISTERED_ITEMS_COUNT;
}

void TryRemoveRegisteredItems(void)
{
	u8 i;
	
	for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
	{
		if (gSaveBlock1Ptr->registeredItem[i] && !CheckBagHasItem(gSaveBlock1Ptr->registeredItem[i], 1))
			gSaveBlock1Ptr->registeredItem[i] = ITEM_NONE;
	}
}

// box and item defines
#define sAddToX                  data[0]
#define sAddToY                  data[1]
#define sNumFramesToAdd          data[2]
// useds only by the boxes
#define sItemSpriteId            data[3]
#define sOriginalItemSpriteId    data[4]

// cursor defines
#define sCursorDirection         data[0]
#define sCursorToSwitch          data[1]
#define sBoxSpriteIds(direction) data[direction + 2]

void InitRegisteredItemsToChoose(void)
{
	struct SpriteTemplate itemTemplate = {
		.oam = &sOamData_RegisteredItem,
		.anims = gDummySpriteAnimTable,
		.images = NULL,
		.affineAnims = sAffineAnimTable_RegisteredItem,
		.callback = SpriteCallbackDummy
	};
	
	u8 i, spriteId, spriteId2, cursorSpriteId, itemSpriteIds[REGISTERED_ITEMS_COUNT] = {0}, boxSpriteIds[REGISTERED_ITEMS_COUNT] = {0};
	u16 tag;
	
	LoadSpriteSheet(&sBoxSpriteSheet);
	LoadSpritePalette(&sBoxAndCursorSpritePalette);
	
	for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
	{
		tag = ITEMICON_INITIAL_TAG + i;
		itemTemplate.tileTag = tag;
		itemTemplate.paletteTag = tag;
		
		boxSpriteIds[i] = spriteId2 = CreateSprite(&sBoxTemplate, sRegisteredBoxesIconsPositions[i].x, sRegisteredBoxesIconsPositions[i].y, 0);
		itemSpriteIds[i] = MAX_SPRITES;
		
		if (gSaveBlock1Ptr->registeredItem[i])
		{
			itemSpriteIds[i] = spriteId = AddItemIconObjectWithCustomObjectTemplate(&itemTemplate, tag, tag, gSaveBlock1Ptr->registeredItem[i]);
			// icon X and Y positions = box position + 4 (center of the box image)
			gSprites[spriteId].x = sRegisteredBoxesIconsPositions[i].x + 4;
			gSprites[spriteId].y = sRegisteredBoxesIconsPositions[i].y + 4;
		}
		gSprites[spriteId2].sItemSpriteId = itemSpriteIds[i];
		gSprites[spriteId2].sOriginalItemSpriteId = itemSpriteIds[i];
	}
	PlaySE(SE_EXIT);
	
	LoadSpriteSheet(&sCursorSpriteSheet);
	
	cursorSpriteId = CreateSprite(&sSelectItem_Cursor, sCursorPositions[CURSORDIRECTION_UP].x, sCursorPositions[CURSORDIRECTION_UP].y, 0);
	
	gSprites[cursorSpriteId].invisible = TRUE;
	gSprites[cursorSpriteId].sCursorDirection = CURSORDIRECTION_UP;
	
	for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
		gSprites[cursorSpriteId].sBoxSpriteIds(i) = boxSpriteIds[i];
}

static void SpriteCB_WaitAffinAnimsAndInitCursor(struct Sprite *sprite)
{
	if (gSprites[sprite->sBoxSpriteIds(0)].affineAnimEnded)
	{
		sprite->invisible = FALSE;
		sprite->callback = SpriteCB_SelectItem_HandleInput;
	}
}

static void SpriteCB_SelectItem_HandleInput(struct Sprite *sprite)
{
	u8 direction = sprite->sCursorDirection;
	
	if (JOY_NEW(A_BUTTON))
	{
		// when press A use the selected item
		if (gSaveBlock1Ptr->registeredItem[direction])
		{
			PlaySE(SE_SELECT);
			gSpecialVar_ItemId = gSaveBlock1Ptr->registeredItem[direction];
			InitAffinAnimsAndUseItem(sprite);
			return;
		}
		PlaySE(SE_FAILURE);
	}
	else if (JOY_NEW(B_BUTTON | SELECT_BUTTON))
	{
		// when press B or SELECT close the system
		PlaySE(SE_EXIT);
		gSpecialVar_ItemId = ITEM_NONE;
		InitAffinAnimsAndUseItem(sprite);
	}
	else if (JOY_NEW(START_BUTTON))
	{
		// when press START init slot swap input
		SetDataToSlideAnim(&gSprites[sprite->sBoxSpriteIds(direction)], direction);
		sprite->sCursorToSwitch = direction;
		HandleSelectItemAction(sprite, direction ^ 2); // ^ 2 inverts the cursor direction for quickly acess the opposite slot
		sprite->callback = SpriteCB_MoveItem_HandleInput;
	}
	else // general cursor movement
		GeneralCursorMovement(sprite);
}

static void SpriteCB_MoveItem_HandleInput(struct Sprite *sprite)
{
	u8 firstItem, secondItem, direction = sprite->sCursorToSwitch;
	
	if (!gSprites[sprite->sBoxSpriteIds(direction)].sNumFramesToAdd)
	{
		firstItem = GetItemSpriteIdByDirection(sprite, direction, FALSE);
		secondItem = GetItemSpriteIdByDirection(sprite, sprite->sCursorDirection, FALSE);
		
		if (JOY_NEW(A_BUTTON))
		{
			// when press A init the swap action
			SetDataToSlideAnim(&gSprites[sprite->sBoxSpriteIds(direction)], direction ^ 2); // inverts the cursor direction for the box moving anim
			
			// fails if choose the same slot or two free slots
			if (firstItem == secondItem || (firstItem == MAX_SPRITES && secondItem == MAX_SPRITES))
			{
				PlaySE(SE_FAILURE);
				sprite->callback = SpriteCB_WaitSlideAnimAndReturnToInput;
			}
			else
			{
				if (firstItem != MAX_SPRITES)
				{
					StartSpriteAffineAnim(&gSprites[firstItem], 1);
				}
				if (secondItem != MAX_SPRITES)
				{
					StartSpriteAffineAnim(&gSprites[secondItem], 1);
				}
				sprite->callback = SpriteCB_WaitAffinAnimsAndSwitchItems;
			}
		}
		else if (JOY_NEW(B_BUTTON))
		{
			// when press B calcel slot swap action
			SetDataToSlideAnim(&gSprites[sprite->sBoxSpriteIds(direction)], direction ^ 2); // inverts the cursor direction for the box return for it's original position
			HandleSelectItemAction(sprite, sprite->sCursorToSwitch);
			sprite->callback = SpriteCB_WaitSlideAnimAndReturnToInput;
		}
		else // general cursor movement
		    GeneralCursorMovement(sprite);
	}
}

static void SpriteCB_HandleUseSelectedItem(struct Sprite *sprite)
{
	u8 i, spriteId;
	
	if (gSprites[sprite->sBoxSpriteIds(0)].affineAnimEnded)
	{
		for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
		{
			spriteId = GetItemSpriteIdByDirection(sprite, i, TRUE);
			
			if (spriteId != MAX_SPRITES)
			{
				FreeSpriteOamMatrix(&gSprites[spriteId]);
				DestroySprite(&gSprites[spriteId]);
				FreeSpriteTilesByTag(ITEMICON_INITIAL_TAG + i);
				FreeSpritePaletteByTag(ITEMICON_INITIAL_TAG + i);
			}
			FreeSpriteOamMatrix(&gSprites[sprite->sBoxSpriteIds(i)]);
			DestroySprite(&gSprites[sprite->sBoxSpriteIds(i)]);
			FreeSpriteTilesByTag(BOX_TAG);
			FreeSpritePaletteByTag(BOX_TAG);
		}
		FreeSpriteOamMatrix(sprite);
		DestroySprite(sprite);
		FreeSpriteTilesByTag(CURSOR_TAG);
		FreeSpritePaletteByTag(BOX_TAG);
		
		if (!gSpecialVar_ItemId)
		{ // close the system
			ClearPlayerHeldMovementAndUnfreezeObjectEvents();
			ScriptContext2_Disable();
		}
		else
		{ // close the system and use the item
			ScriptContext2_Enable();
			gTasks[CreateTask(ItemId_GetFieldFunc(gSpecialVar_ItemId), 8)].data[3] = 1;
		}
	}
}

static void SpriteCB_WaitAffinAnimsAndSwitchItems(struct Sprite *sprite)
{
	u8 spriteId, firstItem = GetItemSpriteIdByDirection(sprite, sprite->sCursorDirection, FALSE), secondItem = GetItemSpriteIdByDirection(sprite, sprite->sCursorToSwitch, FALSE);
	u16 temp;
	
	if ((firstItem != MAX_SPRITES && gSprites[firstItem].affineAnimEnded) || (secondItem != MAX_SPRITES && gSprites[secondItem].affineAnimEnded)) // wait anim ends to continue
	{
		SWAP(gSaveBlock1Ptr->registeredItem[sprite->sCursorDirection], gSaveBlock1Ptr->registeredItem[sprite->sCursorToSwitch], temp);
			
		gSprites[sprite->sBoxSpriteIds(sprite->sCursorDirection)].sItemSpriteId = secondItem;
		gSprites[sprite->sBoxSpriteIds(sprite->sCursorToSwitch)].sItemSpriteId = firstItem;
		
		PlaySE(SE_SWITCH);
		
		if (firstItem != MAX_SPRITES && secondItem != MAX_SPRITES)
		{
			SWAP(gSprites[firstItem].x, gSprites[secondItem].x, temp);
			SWAP(gSprites[firstItem].y, gSprites[secondItem].y, temp);
		}
		else if (firstItem != MAX_SPRITES)
		{
			gSprites[firstItem].x = sRegisteredBoxesIconsPositions[sprite->sCursorToSwitch].x + 4;
			gSprites[firstItem].y = sRegisteredBoxesIconsPositions[sprite->sCursorToSwitch].y + 4;
		}
		else
		{
			gSprites[secondItem].x = sRegisteredBoxesIconsPositions[sprite->sCursorDirection].x + 4;
			gSprites[secondItem].y = sRegisteredBoxesIconsPositions[sprite->sCursorDirection].y + 4;
		}
		
		if (firstItem != MAX_SPRITES)
		{
			StartSpriteAffineAnim(&gSprites[firstItem], 0);
		}
		if (secondItem != MAX_SPRITES)
		{
			StartSpriteAffineAnim(&gSprites[secondItem], 0);
		}
		sprite->callback = SpriteCB_WaitAffinAnimsAndReturnToInput;
	}
}

static void SpriteCB_WaitAffinAnimsAndReturnToInput(struct Sprite *sprite)
{
	u8 firstItem = GetItemSpriteIdByDirection(sprite, sprite->sCursorDirection, FALSE), secondItem = GetItemSpriteIdByDirection(sprite, sprite->sCursorToSwitch, FALSE);
	
	if ((firstItem != MAX_SPRITES && gSprites[firstItem].affineAnimEnded) || (secondItem != MAX_SPRITES && gSprites[secondItem].affineAnimEnded)) // wait anim ends to continue
		sprite->callback = SpriteCB_SelectItem_HandleInput;
}

static void SpriteCB_WaitSlideAnimAndReturnToInput(struct Sprite *sprite)
{
	if (!gSprites[sprite->sBoxSpriteIds(sprite->sCursorToSwitch)].sNumFramesToAdd)
		sprite->callback = SpriteCB_SelectItem_HandleInput;
}

static void SpriteCB_MoveSpriteToSwitchMode(struct Sprite *sprite)
{
	if (--sprite->sNumFramesToAdd != 0)
	{
		sprite->x += sprite->sAddToX;
		sprite->y += sprite->sAddToY;
		return;
	}
	sprite->callback = SpriteCallbackDummy;
}

static void SetDataToSlideAnim(struct Sprite *sprite, u8 direction)
{
	u8 itemSpriteId;
	s16 addX = 0, addY = 0;
	u16 numFramesToAdd = SLIDE_FRAMES_COUNT;
	
	switch (direction)
	{
		case CURSORDIRECTION_UP:
		    addY = -1;
	            break;
		case CURSORDIRECTION_RIGT:
		    addX = 1;
		    break;
		case CURSORDIRECTION_DOWN:
		    addY = 1;
		    break;
		case CURSORDIRECTION_LEFT:
		    addX = -1;
		    break;
	}
	sprite->sAddToX = addX;
	sprite->sAddToY = addY;
	sprite->sNumFramesToAdd = numFramesToAdd;
	
	itemSpriteId = sprite->sItemSpriteId;
	
	if (itemSpriteId != MAX_SPRITES)
	{
		gSprites[itemSpriteId].sAddToX = addX;
		gSprites[itemSpriteId].sAddToY = addY;
		gSprites[itemSpriteId].sNumFramesToAdd = numFramesToAdd;
		gSprites[itemSpriteId].callback = SpriteCB_MoveSpriteToSwitchMode;
	}
	sprite->callback = SpriteCB_MoveSpriteToSwitchMode;
}

static void InitAffinAnimsAndUseItem(struct Sprite *sprite)
{
	u8 i, spriteId;
	
	for (i = 0; i < REGISTERED_ITEMS_COUNT; i++)
	{
		spriteId = GetItemSpriteIdByDirection(sprite, i, FALSE);
		
		if (spriteId != MAX_SPRITES)
			StartSpriteAffineAnim(&gSprites[spriteId], 1);
		StartSpriteAffineAnim(&gSprites[sprite->sBoxSpriteIds(i)], 1);
	}
	sprite->invisible = TRUE;
	sprite->callback = SpriteCB_HandleUseSelectedItem;
}

static void GeneralCursorMovement(struct Sprite *sprite)
{
	if (JOY_NEW(DPAD_UP))
	{
		HandleSelectItemAction(sprite, CURSORDIRECTION_UP);
	}
	else if (JOY_NEW(DPAD_DOWN))
	{
		HandleSelectItemAction(sprite, CURSORDIRECTION_DOWN);
	}
	else if (JOY_NEW(DPAD_RIGHT))
	{
		HandleSelectItemAction(sprite, CURSORDIRECTION_RIGT);
	}
	else if (JOY_NEW(DPAD_LEFT))
	{
		HandleSelectItemAction(sprite, CURSORDIRECTION_LEFT);
	}
}

static void HandleSelectItemAction(struct Sprite *sprite, u8 direction)
{
	PlaySE(SE_SELECT);
	sprite->sCursorDirection = direction;
	sprite->x = sCursorPositions[direction].x;
	sprite->y = sCursorPositions[direction].y;
	StartSpriteAffineAnim(sprite, DirectionToAnimNum(direction));
}

static u8 GetItemSpriteIdByDirection(struct Sprite *sprite, u8 direction, bool8 GetOriginal)
{
	struct Sprite *boxSprite = &gSprites[sprite->sBoxSpriteIds(direction)];
	
	if (GetOriginal)
		return boxSprite->sOriginalItemSpriteId;
	else
		return boxSprite->sItemSpriteId;
}

static u8 DirectionToAnimNum(u8 direction)
{
	if (direction == CURSORDIRECTION_RIGT || direction == CURSORDIRECTION_DOWN)
		direction ^= 3;
	
	return direction;
}
