#include "global.h"
#include "item.h"
#include "text.h"
#include "util.h"
#include "link.h"
#include "random.h"
#include "pokemon.h"
#include "string_util.h"
#include "field_weather.h"
#include "form_change.h"
#include "event_data.h"
#include "mail.h"
#include "mail_data.h"
#include "party_menu.h"
#include "pokedex.h"
#include "strings.h"
#include "malloc.h"
#include "battle.h"
#include "battle_queued_effects.h"
#include "battle_anim.h"
#include "battle_scripts.h"
#include "battle_message.h"
#include "battle_controllers.h"
#include "battle_ai_script_commands.h"
#include "calculate_base_damage.h"
#include "set_effect.h"
#include "constants/battle_string_ids.h"
#include "constants/battle_anim.h"
#include "constants/battle.h"
#include "constants/form_change.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/weather.h"
#include "constants/abilities.h"
#include "constants/pokemon.h"
#include "constants/hold_effects.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_script_commands.h"

static const u8 sTargetPositions[][2] =
{
	[B_SIDE_PLAYER] = {B_POSITION_OPPONENT_RIGHT, B_POSITION_OPPONENT_LEFT},
	[B_SIDE_OPPONENT] = {B_POSITION_PLAYER_RIGHT, B_POSITION_PLAYER_LEFT},
};

static const bool8 sIgnorableAbilities[ABILITIES_COUNT] =
{
    [ABILITY_BATTLE_ARMOR] = TRUE,
    [ABILITY_CLEAR_BODY] = TRUE,
    [ABILITY_DAMP] = TRUE,
    [ABILITY_DRY_SKIN] = TRUE,
    [ABILITY_FILTER] = TRUE,
    [ABILITY_FLASH_FIRE] = TRUE,
    [ABILITY_FLOWER_GIFT] = TRUE,
    [ABILITY_HEATPROOF] = TRUE,
    [ABILITY_HYPER_CUTTER] = TRUE,
    [ABILITY_IMMUNITY] = TRUE,
    [ABILITY_INNER_FOCUS] = TRUE,
    [ABILITY_INSOMNIA] = TRUE,
    [ABILITY_KEEN_EYE] = TRUE,
    [ABILITY_LEAF_GUARD] = TRUE,
    [ABILITY_LEVITATE] = TRUE,
    [ABILITY_LIGHTNING_ROD] = TRUE,
    [ABILITY_LIMBER] = TRUE,
    [ABILITY_MAGMA_ARMOR] = TRUE,
    [ABILITY_MARVEL_SCALE] = TRUE,
    [ABILITY_MOTOR_DRIVE] = TRUE,
    [ABILITY_OBLIVIOUS] = TRUE,
    [ABILITY_OWN_TEMPO] = TRUE,
    [ABILITY_SAND_VEIL] = TRUE,
    [ABILITY_SHELL_ARMOR] = TRUE,
    [ABILITY_SHIELD_DUST] = TRUE,
    [ABILITY_SIMPLE] = TRUE,
    [ABILITY_SNOW_CLOAK] = TRUE,
    [ABILITY_SOLID_ROCK] = TRUE,
    [ABILITY_SOUNDPROOF] = TRUE,
    [ABILITY_STICKY_HOLD] = TRUE,
    [ABILITY_STORM_DRAIN] = TRUE,
    [ABILITY_STURDY] = TRUE,
    [ABILITY_SUCTION_CUPS] = TRUE,
    [ABILITY_TANGLED_FEET] = TRUE,
    [ABILITY_THICK_FAT] = TRUE,
    [ABILITY_UNAWARE] = TRUE,
    [ABILITY_VITAL_SPIRIT] = TRUE,
    [ABILITY_VOLT_ABSORB] = TRUE,
    [ABILITY_WATER_ABSORB] = TRUE,
    [ABILITY_WATER_VEIL] = TRUE,
    [ABILITY_WHITE_SMOKE] = TRUE,
    [ABILITY_WONDER_GUARD] = TRUE,
    [ABILITY_BIG_PECKS] = TRUE,
    [ABILITY_CONTRARY] = TRUE,
    [ABILITY_FRIEND_GUARD] = TRUE,
    [ABILITY_HEAVY_METAL] = TRUE,
    [ABILITY_LIGHT_METAL] = TRUE,
    [ABILITY_MAGIC_BOUNCE] = TRUE,
    [ABILITY_MULTISCALE] = TRUE,
    [ABILITY_SAP_SIPPER] = TRUE,
    [ABILITY_TELEPATHY] = TRUE,
    [ABILITY_WONDER_SKIN] = TRUE,
    [ABILITY_AURA_BREAK] = TRUE,
    [ABILITY_AROMA_VEIL] = TRUE,
    [ABILITY_BULLETPROOF] = TRUE,
    [ABILITY_FLOWER_VEIL] = TRUE,
    [ABILITY_FUR_COAT] = TRUE,
    [ABILITY_OVERCOAT] = TRUE,
    [ABILITY_SWEET_VEIL] = TRUE,
    [ABILITY_DAZZLING] = TRUE,
    [ABILITY_DISGUISE] = TRUE,
    [ABILITY_FLUFFY] = TRUE,
    [ABILITY_QUEENLY_MAJESTY] = TRUE,
    [ABILITY_WATER_BUBBLE] = TRUE,
    [ABILITY_ICE_SCALES] = TRUE,
    [ABILITY_ICE_FACE] = TRUE,
    [ABILITY_MIRROR_ARMOR] = TRUE,
    [ABILITY_PASTEL_VEIL] = TRUE,
    [ABILITY_PUNK_ROCK] = TRUE,
    [ABILITY_ARMOR_TAIL] = TRUE,
    [ABILITY_EARTH_EATER] = TRUE,
    [ABILITY_GOOD_AS_GOLD] = TRUE,
    [ABILITY_PURIFYING_SALT] = TRUE,
    [ABILITY_WELL_BAKED_BODY] = TRUE,
    [ABILITY_WIND_RIDER] = TRUE,
};

static const u16 sWeatherFlagsInfo[][2] =
{
	[ENUM_WEATHER_NONE]         = {0, 0},
	[ENUM_WEATHER_RAIN] 		= {WEATHER_RAIN_TEMPORARY, WEATHER_RAIN_PERMANENT},
	[ENUM_WEATHER_RAIN_PRIMAL] 	= {WEATHER_RAIN_PRIMAL, WEATHER_RAIN_PRIMAL},
	[ENUM_WEATHER_SUN] 			= {WEATHER_SUN_TEMPORARY, WEATHER_SUN_PERMANENT},
	[ENUM_WEATHER_SUN_PRIMAL] 	= {WEATHER_SUN_PRIMAL, WEATHER_SUN_PRIMAL},
	[ENUM_WEATHER_SANDSTORM] 	= {WEATHER_SANDSTORM_TEMPORARY, WEATHER_SANDSTORM_PERMANENT},
	[ENUM_WEATHER_HAIL] 		= {WEATHER_HAIL_TEMPORARY, WEATHER_HAIL_PERMANENT},
	[ENUM_WEATHER_STRONG_WINDS] = {WEATHER_STRONG_WINDS, WEATHER_STRONG_WINDS},
	[ENUM_WEATHER_FOG] 			= {WEATHER_FOG_TEMPORARY, WEATHER_FOG_PERMANENT},
};

const u16 gInobedientStringIds[] =
{
    [B_MSG_LOAFING_AROUND] = STRINGID_ATKISLOAFINGAROUND,
    STRINGID_ATKWONTOBEY,
    STRINGID_ATKTURNEDAWAY,
    STRINGID_ATKPRETENDNOTNOTICE
};

// Check if attacker can put a status condition on defender
static u8 CanBeStatused(u8 attacker, u8 defender, u32 flags)
{
	// Check already statused
	if (!(flags & STATUS_CHANGE_FLAG_IGNORE_GENERAL_STATUS) && gBattleMons[defender].status1 & STATUS1_ANY)
		return STATUS_CHANGE_FAIL_ALREADY_STATUSED;
	
	// Check defender's abilities
	switch (GetBattlerAbility(defender))
	{
		case ABILITY_LEAF_GUARD:
			if (IsBattlerWeatherAffected(defender, WEATHER_SUN_ANY))
				return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
			break;
		case ABILITY_SHIELDS_DOWN:
		    if (!(gBattleMons[defender].status2 & STATUS2_TRANSFORMED) && gBattleMons[defender].species == SPECIES_MINIOR_METEOR)
				return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
			break;
		case ABILITY_COMATOSE:
		case ABILITY_PURIFYING_SALT:
		    return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
	}
	// Check Safeguard and Flower Veil
	if (!(flags & STATUS_CHANGE_FLAG_IGNORE_SAFEGUARD))
	{
		if (CanSafeguardProtectBattler(attacker, defender))
			return STATUS_CHANGE_FAIL_SAFEGUARD_PROTECTED;
		else if (IsBattlerProtectedByFlowerVeil(defender))
			return STATUS_CHANGE_FAIL_FLOWER_VEIL_ON_SIDE;
	}
	// Check Uproar
	if ((flags & STATUS_CHANGE_FLAG_CHECK_UPROAR) && UproarWakeUpCheck())
		return STATUS_CHANGE_FAIL_UPROAR;
	
	return STATUS_CHANGE_WORKED;
}

// Check if attacker can put defender to sleep
u8 CanBePutToSleep(u8 attacker, u8 defender, u32 flags)
{
	u8 ret, defenderPartner;
	
	// Check already sleeping
	if (gBattleMons[defender].status1 & STATUS1_SLEEP)
		return STATUS_CHANGE_FAIL_SPECIFIC_STATUSED;
	
	// Check general statuses
	ret = CanBeStatused(attacker, defender, flags);
	
	if (ret != STATUS_CHANGE_WORKED)
		return ret;
	
	// Check defender's abilities
	switch (GetBattlerAbility(defender))
	{
		case ABILITY_INSOMNIA:
		case ABILITY_VITAL_SPIRIT:
			return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
		case ABILITY_SWEET_VEIL:
			gBattleScripting.savedBattler = defender;
			return STATUS_CHANGE_FAIL_SWEET_VEIL_ON_SIDE;
	}
	// Check defender's ally abilities
	defenderPartner = BATTLE_PARTNER(defender);
	
	if (IsBattlerAlive(defenderPartner))
	{
		switch (GetBattlerAbility(defenderPartner))
		{
			case ABILITY_SWEET_VEIL:
				gBattleScripting.savedBattler = defenderPartner;
			    return STATUS_CHANGE_FAIL_SWEET_VEIL_ON_SIDE;
		}
	}
	return STATUS_CHANGE_WORKED;
}

static bool8 CanPoisonType(u8 attacker, u8 defender)
{
	if ((GetBattlerAbility(attacker) == ABILITY_CORROSION && IS_MOVE_STATUS(gCurrentMove)) || !(IS_BATTLER_OF_TYPE(defender, TYPE_POISON) || IS_BATTLER_OF_TYPE(defender, TYPE_STEEL)))
		return TRUE;
	return FALSE;
}

// Check if attacker can poison defender
bool8 CanBePoisoned(u8 attacker, u8 defender, u32 flags)
{
	u8 ret, defenderPartner;
	
	// Check already poisoned
	if (gBattleMons[defender].status1 & STATUS1_PSN_ANY)
		return STATUS_CHANGE_FAIL_SPECIFIC_STATUSED;
	
	// Check general statuses
	ret = CanBeStatused(attacker, defender, flags);
	
	if (ret != STATUS_CHANGE_WORKED)
		return ret;
	
	// Check defender's abilities
	switch (GetBattlerAbility(defender))
	{
		case ABILITY_IMMUNITY:
			return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
		case ABILITY_PASTEL_VEIL:
			gBattleScripting.savedBattler = defender;
			return STATUS_CHANGE_FAIL_PASTEL_VEIL_ON_SIDE;
	}
	// Check defender's ally abilities
	defenderPartner = BATTLE_PARTNER(defender);
	
	if (IsBattlerAlive(defenderPartner))
	{
		switch (GetBattlerAbility(defenderPartner))
		{
			case ABILITY_PASTEL_VEIL:
				gBattleScripting.savedBattler = defenderPartner;
			    return STATUS_CHANGE_FAIL_PASTEL_VEIL_ON_SIDE;
		}
	}
	// Check types
	if (!CanPoisonType(attacker, defender))
		return STATUS_CHANGE_FAIL_TYPE_NOT_AFFECTED;
	
	return STATUS_CHANGE_WORKED;
}

// Check if attacker can burn defender
u8 CanBeBurned(u8 attacker, u8 defender, u32 flags)
{
	u8 ret;
	
	// Check already burned
	if (gBattleMons[defender].status1 & STATUS1_BURN)
		return STATUS_CHANGE_FAIL_SPECIFIC_STATUSED;
	
	// Check general statuses
	ret = CanBeStatused(attacker, defender, flags);
	
	if (ret != STATUS_CHANGE_WORKED)
		return ret;
	
	// Check defender's abilities
	switch (GetBattlerAbility(defender))
	{
		case ABILITY_WATER_VEIL:
		case ABILITY_WATER_BUBBLE:
		case ABILITY_THERMAL_EXCHANGE:
			return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
	}
	// Check types
	if (IS_BATTLER_OF_TYPE(defender, TYPE_FIRE))
		return STATUS_CHANGE_FAIL_TYPE_NOT_AFFECTED;
	
	return STATUS_CHANGE_WORKED;
}

// Check if attacker can frozen defender
bool8 CanBeFrozen(u8 attacker, u8 defender, u32 flags)
{
	// Check general statuses
	if (CanBeStatused(attacker, defender, flags) != STATUS_CHANGE_WORKED)
		return FALSE;
	
	// Check defender's abilities
	switch (GetBattlerAbility(defender))
	{
		case ABILITY_MAGMA_ARMOR:
			return FALSE;
	}
	if (IS_BATTLER_OF_TYPE(defender, TYPE_ICE)) // Check types
		return FALSE;
	else if (IsBattlerWeatherAffected(defender, WEATHER_SUN_ANY)) // Check weather
		return FALSE;
	
	return TRUE;
}

// Check if attacker can paralyse defender
u8 CanBeParalyzed(u8 attacker, u8 defender, u32 flags)
{
	u8 ret;
	
	// Check already paralized
	if (gBattleMons[defender].status1 & STATUS1_PARALYSIS)
		return STATUS_CHANGE_FAIL_SPECIFIC_STATUSED;
	
	// Check general statuses
	ret = CanBeStatused(attacker, defender, flags);
	
	if (ret != STATUS_CHANGE_WORKED)
		return ret;
	
	// Check defender's abilities
	switch (GetBattlerAbility(defender))
	{
		case ABILITY_LIMBER:
			return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
	}
	// Check types
	if (IS_BATTLER_OF_TYPE(defender, TYPE_ELECTRIC))
		return STATUS_CHANGE_FAIL_TYPE_NOT_AFFECTED;
	
	return STATUS_CHANGE_WORKED;
}

u8 CanBecameConfused(u8 attacker, u8 defender, u32 flags)
{
	// Check defender's abilities
	if (GetBattlerAbility(defender) == ABILITY_OWN_TEMPO)
		return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
	
	// Check already confused
	if (gBattleMons[defender].status2 & STATUS2_CONFUSION)
		return STATUS_CHANGE_FAIL_SPECIFIC_STATUSED;
	
	// Check Safeguard
	if (!(flags & STATUS_CHANGE_FLAG_IGNORE_SAFEGUARD) && CanSafeguardProtectBattler(attacker, defender))
		return STATUS_CHANGE_FAIL_SAFEGUARD_PROTECTED;
	
	return STATUS_CHANGE_WORKED;
}

bool8 IsAbilityBlockedByNeutralizingGas(u16 ability)
{
	switch (ability)
	{
		case ABILITY_NEUTRALIZING_GAS:
		case ABILITY_MULTITYPE:
		case ABILITY_ZEN_MODE:
		case ABILITY_STANCE_CHANGE:
		case ABILITY_SCHOOLING:
		case ABILITY_COMATOSE:
		case ABILITY_SHIELDS_DOWN:
		case ABILITY_DISGUISE:
		case ABILITY_RKS_SYSTEM:
		case ABILITY_POWER_CONSTRUCT:
		case ABILITY_BATTLE_BOND:
		case ABILITY_ICE_FACE:
		case ABILITY_GULP_MISSILE:
		case ABILITY_AS_ONE_ICE_RIDER:
		case ABILITY_AS_ONE_SHADOW_RIDER:
		case ABILITY_ZERO_TO_HERO:
		    return FALSE;
	}
	return TRUE;
}

bool8 IsRolePlayBannedAbilityAttacker(u16 ability)
{
	switch (ability)
	{
		case ABILITY_COMMANDER:
		case ABILITY_RECEIVER:
		case ABILITY_MULTITYPE:
		case ABILITY_ZEN_MODE:
		case ABILITY_STANCE_CHANGE:
		case ABILITY_SCHOOLING:
		case ABILITY_COMATOSE:
		case ABILITY_SHIELDS_DOWN:
		case ABILITY_DISGUISE:
		case ABILITY_RKS_SYSTEM:
		case ABILITY_POWER_CONSTRUCT:
		case ABILITY_BATTLE_BOND:
		case ABILITY_ICE_FACE:
		case ABILITY_GULP_MISSILE:
		case ABILITY_AS_ONE_ICE_RIDER:
		case ABILITY_AS_ONE_SHADOW_RIDER:
		case ABILITY_ZERO_TO_HERO:
		    return TRUE;
	}
	return FALSE;
}

bool8 IsRolePlayBannedAbilityTarget(u16 ability)
{
	switch (ability)
	{
		case ABILITY_TRACE:
		case ABILITY_FORECAST:
		case ABILITY_WONDER_GUARD:
		case ABILITY_FLOWER_GIFT:
		case ABILITY_ILLUSION:
		case ABILITY_IMPOSTER:
		case ABILITY_POWER_OF_ALCHEMY:
		case ABILITY_RECEIVER:
		case ABILITY_COMMANDER:
		case ABILITY_NEUTRALIZING_GAS:
		case ABILITY_MULTITYPE:
		case ABILITY_ZEN_MODE:
		case ABILITY_STANCE_CHANGE:
		case ABILITY_SCHOOLING:
		case ABILITY_COMATOSE:
		case ABILITY_SHIELDS_DOWN:
		case ABILITY_DISGUISE:
		case ABILITY_RKS_SYSTEM:
		case ABILITY_POWER_CONSTRUCT:
		case ABILITY_BATTLE_BOND:
		case ABILITY_ICE_FACE:
		case ABILITY_GULP_MISSILE:
		case ABILITY_AS_ONE_ICE_RIDER:
		case ABILITY_AS_ONE_SHADOW_RIDER:
		case ABILITY_ZERO_TO_HERO:
		    return TRUE;
	}
	return FALSE;
}

static bool8 IsBattlerAbilityCopyableByTrace(u8 battlerId)
{
	u16 ability = gBattleMons[battlerId].ability;
	
	switch (ability)
	{
		case ABILITY_TRACE:
		case ABILITY_FLOWER_GIFT:
		case ABILITY_FORECAST:
		case ABILITY_HUNGER_SWITCH:
		case ABILITY_ILLUSION:
		case ABILITY_IMPOSTER:
		case ABILITY_NEUTRALIZING_GAS:
		case ABILITY_POWER_OF_ALCHEMY:
		case ABILITY_RECEIVER:
		case ABILITY_COMMANDER:
		case ABILITY_MULTITYPE:
		case ABILITY_ZEN_MODE:
		case ABILITY_STANCE_CHANGE:
		case ABILITY_SCHOOLING:
		case ABILITY_COMATOSE:
		case ABILITY_SHIELDS_DOWN:
		case ABILITY_DISGUISE:
		case ABILITY_RKS_SYSTEM:
		case ABILITY_POWER_CONSTRUCT:
		case ABILITY_BATTLE_BOND:
		case ABILITY_ICE_FACE:
		case ABILITY_GULP_MISSILE:
		case ABILITY_AS_ONE_ICE_RIDER:
		case ABILITY_AS_ONE_SHADOW_RIDER:
		case ABILITY_ZERO_TO_HERO:
		    return FALSE;
	}
	return TRUE;
}

static bool8 IsAbilityChangeableByWanderingSpirit(u16 ability)
{
	switch (ability)
	{
		case ABILITY_COMMANDER:
		case ABILITY_FLOWER_GIFT:
		case ABILITY_FORECAST:
		case ABILITY_HUNGER_SWITCH:
		case ABILITY_ILLUSION:
		case ABILITY_IMPOSTER:
		case ABILITY_NEUTRALIZING_GAS:
		case ABILITY_POWER_OF_ALCHEMY:
		case ABILITY_RECEIVER:
		case ABILITY_WONDER_GUARD:
		case ABILITY_MULTITYPE:
		case ABILITY_ZEN_MODE:
		case ABILITY_STANCE_CHANGE:
		case ABILITY_SCHOOLING:
		case ABILITY_COMATOSE:
		case ABILITY_SHIELDS_DOWN:
		case ABILITY_DISGUISE:
		case ABILITY_RKS_SYSTEM:
		case ABILITY_POWER_CONSTRUCT:
		case ABILITY_BATTLE_BOND:
		case ABILITY_ICE_FACE:
		case ABILITY_GULP_MISSILE:
		case ABILITY_AS_ONE_ICE_RIDER:
		case ABILITY_AS_ONE_SHADOW_RIDER:
		case ABILITY_ZERO_TO_HERO:
		    return FALSE;
	}
	return TRUE;
}

static bool8 CanAbilityBeReplacedByMummy(u16 ability)
{
	switch (ability)
	{
		case ABILITY_COMMANDER:
		case ABILITY_LINGERING_AROMA:
		case ABILITY_MUMMY:
		case ABILITY_MULTITYPE:
		case ABILITY_ZEN_MODE:
		case ABILITY_STANCE_CHANGE:
		case ABILITY_SCHOOLING:
		case ABILITY_COMATOSE:
		case ABILITY_SHIELDS_DOWN:
		case ABILITY_DISGUISE:
		case ABILITY_RKS_SYSTEM:
		case ABILITY_POWER_CONSTRUCT:
		case ABILITY_BATTLE_BOND:
		case ABILITY_ICE_FACE:
		case ABILITY_GULP_MISSILE:
		case ABILITY_AS_ONE_ICE_RIDER:
		case ABILITY_AS_ONE_SHADOW_RIDER:
		case ABILITY_ZERO_TO_HERO:
		    return FALSE;
	}
	return TRUE;
}

bool8 IsAbilityCopyableByReceiver(u16 ability)
{
	switch (ability)
	{
		case ABILITY_RECEIVER:
		case ABILITY_POWER_OF_ALCHEMY:
		case ABILITY_TRACE:
		case ABILITY_FORECAST:
		case ABILITY_FLOWER_GIFT:
		case ABILITY_ILLUSION:
		case ABILITY_WONDER_GUARD:
		case ABILITY_IMPOSTER:
		case ABILITY_WANDERING_SPIRIT:
		case ABILITY_COMMANDER:
		case ABILITY_MULTITYPE:
		case ABILITY_ZEN_MODE:
		case ABILITY_STANCE_CHANGE:
		case ABILITY_SCHOOLING:
		case ABILITY_COMATOSE:
		case ABILITY_SHIELDS_DOWN:
		case ABILITY_DISGUISE:
		case ABILITY_RKS_SYSTEM:
		case ABILITY_POWER_CONSTRUCT:
		case ABILITY_BATTLE_BOND:
		case ABILITY_ICE_FACE:
		case ABILITY_GULP_MISSILE:
		case ABILITY_AS_ONE_ICE_RIDER:
		case ABILITY_AS_ONE_SHADOW_RIDER:
		case ABILITY_ZERO_TO_HERO:
		    return FALSE;
	}
	return TRUE;
}

bool8 IsSkillSwapBannedAbility(u16 ability)
{
	switch (ability)
	{
		case ABILITY_WONDER_GUARD:
		case ABILITY_ILLUSION:
		case ABILITY_NEUTRALIZING_GAS:
		case ABILITY_COMMANDER:
		case ABILITY_MULTITYPE:
		case ABILITY_STANCE_CHANGE:
		case ABILITY_SCHOOLING:
		case ABILITY_COMATOSE:
		case ABILITY_SHIELDS_DOWN:
		case ABILITY_DISGUISE:
		case ABILITY_RKS_SYSTEM:
		case ABILITY_POWER_CONSTRUCT:
		case ABILITY_BATTLE_BOND:
		case ABILITY_ICE_FACE:
		case ABILITY_GULP_MISSILE:
		case ABILITY_ZERO_TO_HERO:
		case ABILITY_PROTOSYNTHESIS:
		case ABILITY_QUARK_DRIVE:
		case ABILITY_ORICHALCUM_PULSE:
		case ABILITY_HADRON_ENGINE:
		    return TRUE;
	}
	return FALSE;
}

u8 GetBattlerForBattleScript(u8 caseId)
{
    u32 ret = 0;

    switch (caseId)
    {
    case BS_TARGET:
        ret = gBattlerTarget;
        break;
    case BS_ATTACKER:
        ret = gBattlerAttacker;
        break;
    case BS_EFFECT_BATTLER:
        ret = gEffectBattler;
        break;
    case BS_SCRIPTING:
        ret = gBattleScripting.battler;
        break;
    case BS_FAINTED:
        ret = gBattlerFainted;
        break;
    case 5:
        ret = gBattlerFainted;
        break;
    case BS_PLAYER1:
        ret = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        break;
    case BS_OPPONENT1:
        ret = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        break;
	case BS_SCRIPTING_PARTNER:
	    ret = BATTLE_PARTNER(gBattleScripting.battler);
		break;
    }
    return ret;
}

void PressurePPLose(u8 target, u8 attacker, u16 move)
{
    s32 i;

    if (GetBattlerAbility(target) == ABILITY_PRESSURE)
    {
        for (i = 0; i < MAX_MON_MOVES && gBattleMons[attacker].moves[i] != move; ++i);
        if (i != MAX_MON_MOVES)
        {
            if (gBattleMons[attacker].pp[i])
                --gBattleMons[attacker].pp[i];
            if (!(gBattleMons[attacker].status2 & STATUS2_TRANSFORMED)
             && !(gDisableStructs[attacker].mimickedMoves & gBitTable[i]))
            {
                BtlController_EmitSetMonData(attacker, BUFFER_A, REQUEST_PPMOVE1_BATTLE + i, 0, 1, &gBattleMons[attacker].pp[i]);
                MarkBattlerForControllerExec(attacker);
            }
        }
    }
}

void PressurePPLoseOnUsingImprison(u8 attacker)
{
    s32 i, j;
    s32 imprisonPos = 4;
    u8 atkSide = GetBattlerSide(attacker);

    for (i = 0; i < gBattlersCount; ++i)
    {
        if (atkSide != GetBattlerSide(i) && GetBattlerAbility(i) == ABILITY_PRESSURE)
        {
            for (j = 0; j < MAX_MON_MOVES && gBattleMons[attacker].moves[j] != MOVE_IMPRISON; ++j);
			
            if (j != MAX_MON_MOVES)
            {
                imprisonPos = j;
                if (gBattleMons[attacker].pp[j])
                    --gBattleMons[attacker].pp[j];
            }
        }
    }
    if (imprisonPos != MAX_MON_MOVES && !(gBattleMons[attacker].status2 & STATUS2_TRANSFORMED) && !(gDisableStructs[attacker].mimickedMoves & gBitTable[imprisonPos]))
    {
        BtlController_EmitSetMonData(attacker, BUFFER_A, REQUEST_PPMOVE1_BATTLE + imprisonPos, 0, 1, &gBattleMons[attacker].pp[imprisonPos]);
        MarkBattlerForControllerExec(attacker);
    }
}

void PressurePPLoseOnUsingPerishSong(u8 attacker)
{
    s32 i, j;
    s32 perishSongPos = 4;

    for (i = 0; i < gBattlersCount; ++i)
    {
        if (GetBattlerAbility(i) == ABILITY_PRESSURE && i != attacker)
        {
            for (j = 0; j < MAX_MON_MOVES && gBattleMons[attacker].moves[j] != MOVE_PERISH_SONG; ++j);
			
            if (j != MAX_MON_MOVES)
            {
                perishSongPos = j;
                if (gBattleMons[attacker].pp[j])
                    --gBattleMons[attacker].pp[j];
            }
        }
    }
    if (perishSongPos != MAX_MON_MOVES && !(gBattleMons[attacker].status2 & STATUS2_TRANSFORMED) && !(gDisableStructs[attacker].mimickedMoves & gBitTable[perishSongPos]))
    {
        BtlController_EmitSetMonData(attacker, BUFFER_A, REQUEST_PPMOVE1_BATTLE + perishSongPos, 0, 1, &gBattleMons[attacker].pp[perishSongPos]);
        MarkBattlerForControllerExec(attacker);
    }
}

void MarkAllBattlersForControllerExec(void)
{
    s32 i;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        for (i = 0; i < gBattlersCount; ++i)
            gBattleControllerExecFlags |= gBitTable[i] << 0x1C;
    else
        for (i = 0; i < gBattlersCount; ++i)
            gBattleControllerExecFlags |= gBitTable[i];
}

void MarkBattlerForControllerExec(u8 battlerId)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        gBattleControllerExecFlags |= gBitTable[battlerId] << 0x1C;
    else
        gBattleControllerExecFlags |= gBitTable[battlerId];
}

void MarkBattlerReceivedLinkData(u8 battlerId)
{
    s32 i;

    for (i = 0; i < GetLinkPlayerCount(); ++i)
        gBattleControllerExecFlags |= gBitTable[battlerId] << (i << 2);

    gBattleControllerExecFlags &= ~(0x10000000 << battlerId);
}

void CancelMultiTurnMoves(u8 battler)
{
    gBattleMons[battler].status2 &= ~(STATUS2_MULTIPLETURNS | STATUS2_LOCK_CONFUSE | STATUS2_UPROAR | STATUS2_BIDE);
    gStatuses3[battler] &= ~(STATUS3_SEMI_INVULNERABLE);
    gDisableStructs[battler].rolloutTimer = 0;
    gDisableStructs[battler].furyCutterCounter = 0;
}

bool8 WasUnableToUseMove(u8 battler)
{
    if (gProtectStructs[battler].prlzImmobility
     || gProtectStructs[battler].targetNotAffected
     || gProtectStructs[battler].usedImprisonedMove
     || gProtectStructs[battler].loveImmobility
     || gProtectStructs[battler].usedDisabledMove
     || gProtectStructs[battler].usedTauntedMove
     || gProtectStructs[battler].flinchImmobility
     || gProtectStructs[battler].confusionSelfDmg)
        return TRUE;
    else
        return FALSE;
}

// Used internaly to check if the battler's ability has't be suppressed
static u16 GetBattlerSupressedAbility(u8 battler)
{
	u16 ability = gBattleMons[battler].ability;
	
	return ability;
}

// Check Neutralizing Gas on field blocking the ability. Separated from GetBattlerSupressedAbility to prevent loop using it, bc it was called here too
static bool8 IsNeutralizingGasOnField(u16 ability)
{
	u8 i;
	bool8 foundOnField = FALSE;
	
	for (i = 0; i < gBattlersCount; i++)
	{
		if (IsBattlerAlive(i) && GetBattlerSupressedAbility(i) == ABILITY_NEUTRALIZING_GAS)
			foundOnField = TRUE;
	}
	if (foundOnField)
		foundOnField = IsAbilityBlockedByNeutralizingGas(ability);
	
	return foundOnField;
}

// Used to get the battler's ability. Passing throug all effects that can suppress/block the ability effect, like Gastro Acid and Mycelium Might
u16 GetBattlerAbility(u8 battler)
{
	u16 attackerAbility = GetBattlerSupressedAbility(gBattlerAttacker), ability = GetBattlerSupressedAbility(battler);
	
	if (IsNeutralizingGasOnField(ability))
		ability = ABILITY_NONE;
	else if (gBattlerByTurnOrder[gCurrentTurnActionNumber] == gBattlerAttacker && gActionsByTurnOrder[gBattlerByTurnOrder[gBattlerAttacker]] == B_ACTION_USE_MOVE
		&& gCurrentTurnActionNumber < gBattlersCount && sIgnorableAbilities[ability] == TRUE) // Check if gBattlerAttacker is using a move
	{
		if (attackerAbility == ABILITY_MOLD_BREAKER || attackerAbility == ABILITY_TERAVOLT || attackerAbility == ABILITY_TURBOBLAZE
		|| (attackerAbility == ABILITY_MYCELIUM_MIGHT && gProtectStructs[gBattlerAttacker].myceliumMightElevated))
			ability = ABILITY_NONE;
	}
	return ability;
}

// Used internaly by ability change/suppression to clear vars related to it (e.g suppressing or removing Neutralizing Gas will cause it's effect to wears off)
static void TryEndAbilityEffectOnSuppression(u8 battlerId, u16 oldAbility, u16 newAbility)
{
	if (oldAbility == ABILITY_NEUTRALIZING_GAS && newAbility != ABILITY_NEUTRALIZING_GAS)
		gSpecialStatuses[battlerId].removedNeutralizingGas = TRUE; // for display the message and activate switch in abilities
	else if ((oldAbility == ABILITY_FORECAST && newAbility != ABILITY_FORECAST) || (oldAbility == ABILITY_FLOWER_GIFT && newAbility != ABILITY_FLOWER_GIFT))
		gSpecialStatuses[battlerId].removedWeatherChangeAbility = TRUE; // for transform the mon back
}

// Used in abilities change to clear vars related to it (e.g removing Unburden from a battler will cause it to lose it's boost)
// Returns the old ability of the pokemon
u16 SetBattlerAbility(u8 battlerId, u16 newAbility)
{
	u16 oldAbility = gBattleMons[battlerId].ability;
	
	gBattleMons[battlerId].ability = newAbility;
	
	// Separated of other abilities bc it only loses it's boost when replaced. If suppressed and then regained the boost is restored.
	if (newAbility != ABILITY_UNBURDEN) // Don't need check the old ability as well
		gBattleResources->flags->flags[battlerId] &= ~(RESOURCE_FLAG_UNBURDEN_BOOST);
	
	TryEndAbilityEffectOnSuppression(battlerId, oldAbility, newAbility);
	
	return oldAbility;
}

// Used in abilities suppression. Does nothing to suppress the ability itself, it's only used to clear vars related to it
// Returns the ability suppressed of the pokemon
u16 SuppressBattlerAbility(u8 battlerId)
{
	u16 suppressedAbility = gBattleMons[battlerId].ability;
	
	TryEndAbilityEffectOnSuppression(battlerId, suppressedAbility, ABILITY_NONE); // The suppressed ability has no effect now, so passes it as ABILITY_NONE
	
	return suppressedAbility;
}

void CheckSetBattlerUnburden(u8 battler)
{
	if (GetBattlerAbility(battler) == ABILITY_UNBURDEN)
		gBattleResources->flags->flags[battler] |= RESOURCE_FLAG_UNBURDEN_BOOST;
}

u8 GetBattlerItemHoldEffect(u8 battler, bool8 checkNegating)
{
	u8 holdEffect = ItemId_GetHoldEffect(gBattleMons[battler].item);

	if (checkNegating)
	{
		if (GetBattlerAbility(battler) == ABILITY_KLUTZ)
			holdEffect = HOLD_EFFECT_NONE;
	}
	return holdEffect;
}

static void TryActivateDefiant(u16 stringId)
{
	if (stringId == STRINGID_PKMNSSTATCHANGED4 && gSpecialStatuses[gBattlerTarget].changedStatsBattlerId != BATTLE_PARTNER(gBattlerTarget)
	&& gSpecialStatuses[gBattlerTarget].changedStatsBattlerId != gBattlerTarget)
	{
		switch (GetBattlerAbility(gBattlerTarget))
		{
			case ABILITY_DEFIANT:
			    if (CompareStat(gBattlerTarget, STAT_ATK, MAX_STAT_STAGES, CMP_LESS_THAN))
				{
					SET_STATCHANGER(STAT_ATK, 2, FALSE);
					BattleScriptPushCursor();
					gBattlescriptCurrInstr = BattleScript_DefiantCompetitive;
				}
			    break;
			case ABILITY_COMPETITIVE:
			    if (CompareStat(gBattlerTarget, STAT_SPATK, MAX_STAT_STAGES, CMP_LESS_THAN))
				{
					SET_STATCHANGER(STAT_SPATK, 2, FALSE);
					BattleScriptPushCursor();
					gBattlescriptCurrInstr = BattleScript_DefiantCompetitive;
				}
			    break;
		}
	}
}

void PrepareStringBattle(u16 stringId, u8 battler)
{
    BtlController_EmitPrintString(battler, BUFFER_A, stringId);
    MarkBattlerForControllerExec(battler);
	TryActivateDefiant(stringId);
}

void ResetSentPokesToOpponentValue(void)
{
    s32 i;
    u32 bits = 0;

    gSentPokesToOpponent[0] = 0;
    gSentPokesToOpponent[1] = 0;
    for (i = 0; i < gBattlersCount; i += 2)
        bits |= gBitTable[gBattlerPartyIndexes[i]];
    for (i = 1; i < gBattlersCount; i += 2)
        gSentPokesToOpponent[(i & BIT_FLANK) >> 1] = bits;
}

void OpponentSwitchInResetSentPokesToOpponentValue(u8 battler)
{
    s32 i = 0;
    u32 bits = 0;

    if (GetBattlerSide(battler) == B_SIDE_OPPONENT)
    {
        u8 flank = ((battler & BIT_FLANK) >> 1);
        gSentPokesToOpponent[flank] = 0;

        for (i = 0; i < gBattlersCount; i += 2)
            if (!(gAbsentBattlerFlags & gBitTable[i]))
                bits |= gBitTable[gBattlerPartyIndexes[i]];
        gSentPokesToOpponent[flank] = bits;
    }
}

void UpdateSentPokesToOpponentValue(u8 battler)
{
    if (GetBattlerSide(battler) == B_SIDE_OPPONENT)
    {
        OpponentSwitchInResetSentPokesToOpponentValue(battler);
    }
    else
    {
        s32 i;
        
        for (i = 1; i < gBattlersCount; ++i)
            gSentPokesToOpponent[(i & BIT_FLANK) >> 1] |= gBitTable[gBattlerPartyIndexes[battler]];
    }
}

void BattleScriptPush(const u8 *bsPtr)
{
    gBattleResources->battleScriptsStack->ptr[gBattleResources->battleScriptsStack->size++] = bsPtr;
}

void BattleScriptPushCursor(void)
{
    gBattleResources->battleScriptsStack->ptr[gBattleResources->battleScriptsStack->size++] = gBattlescriptCurrInstr;
}

void BattleScriptPop(void)
{
    gBattlescriptCurrInstr = gBattleResources->battleScriptsStack->ptr[--gBattleResources->battleScriptsStack->size];
}

static u8 GetImprisonedMovesCount(u8 battlerId, u16 move)
{
    u8 i, j, imprisonedMoves = 0, battlerSide = GetBattlerSide(battlerId);

    for (i = 0; i < gBattlersCount; ++i)
    {
        if ((gStatuses3[i] & STATUS3_IMPRISONED_OTHERS) && battlerSide != GetBattlerSide(i))
        {
            for (j = 0; j < MAX_MON_MOVES && move != gBattleMons[i].moves[j]; ++j);
			
            if (j < MAX_MON_MOVES)
                ++imprisonedMoves;
        }
    }
    return imprisonedMoves;
}

bool8 TrySetCantSelectMoveBattleScript(u8 battlerId)
{
	bool8 cantSelect = FALSE;
	u8 holdEffect, movePos = gBattleBufferB[battlerId][2];
	u16 move = gBattleMons[battlerId].moves[movePos];

    if (move && gDisableStructs[battlerId].disabledMove == move)
    {
        gSelectionBattleScripts[battlerId] = BattleScript_SelectingDisabledMove;
        cantSelect = TRUE;
    }
    if ((gBattleMons[battlerId].status2 & STATUS2_TORMENT) && move != MOVE_STRUGGLE && move == gLastMoves[battlerId])
    {
        CancelMultiTurnMoves(battlerId);
        gSelectionBattleScripts[battlerId] = BattleScript_SelectingTormentedMove;
        cantSelect = TRUE;
    }
    if (gDisableStructs[battlerId].tauntTimer && IS_MOVE_STATUS(move))
    {
        gSelectionBattleScripts[battlerId] = BattleScript_SelectingNotAllowedMoveTaunt;
        cantSelect = TRUE;
    }
    if (GetImprisonedMovesCount(battlerId, move))
    {
        gSelectionBattleScripts[battlerId] = BattleScript_SelectingImprisonedMove;
        cantSelect = TRUE;
    }
	holdEffect = GetBattlerItemHoldEffect(battlerId, TRUE);
	
    if (holdEffect == HOLD_EFFECT_CHOICE_ITEM && gBattleStruct->choicedMove[battlerId] && gBattleStruct->choicedMove[battlerId] != MOVE_UNVAILABLE
	&& gBattleStruct->choicedMove[battlerId] != move)
    {
		move = gBattleStruct->choicedMove[battlerId];
        gLastUsedItem = gBattleMons[battlerId].item;
        gSelectionBattleScripts[battlerId] = BattleScript_SelectingNotAllowedMoveChoiceItem;
        cantSelect = TRUE;
    }
    if (!gBattleMons[battlerId].pp[movePos])
    {
        gSelectionBattleScripts[battlerId] = BattleScript_SelectingMoveWithNoPP;
        cantSelect = TRUE;
    }
	
	if (cantSelect)
		gCurrentMove = move;
	
    return cantSelect;
}

u8 CheckMoveLimitations(u8 battlerId, u32 flags)
{
    u8 i, unusableMoves = 0, holdEffect = GetBattlerItemHoldEffect(battlerId, TRUE);
    u16 move;

    for (i = 0; i < MAX_MON_MOVES; ++i)
    {
		move = gBattleMons[battlerId].moves[i];
		
        if (!move)
            unusableMoves |= gBitTable[i];
		
        if (!(flags & MOVE_LIMITATION_IGNORE_NO_PP) && !gBattleMons[battlerId].pp[i])
            unusableMoves |= gBitTable[i];
		
		if (!(flags & MOVE_LIMITATION_IGNORE_IMPRISON) && GetImprisonedMovesCount(battlerId, move))
            unusableMoves |= gBitTable[i];
		
        if (move == gDisableStructs[battlerId].disabledMove)
            unusableMoves |= gBitTable[i];
		
        if ((gBattleMons[battlerId].status2 & STATUS2_TORMENT) && move == gLastMoves[battlerId])
            unusableMoves |= gBitTable[i];
		
        if (gDisableStructs[battlerId].tauntTimer && IS_MOVE_STATUS(move))
            unusableMoves |= gBitTable[i];
		
        if (gDisableStructs[battlerId].encoreTimer && gDisableStructs[battlerId].encoredMove != move)
            unusableMoves |= gBitTable[i];
		
        if (holdEffect == HOLD_EFFECT_CHOICE_ITEM && gBattleStruct->choicedMove[battlerId] && gBattleStruct->choicedMove[battlerId] != MOVE_UNVAILABLE
		&& gBattleStruct->choicedMove[battlerId] != move)
            unusableMoves |= gBitTable[i];
    }
    return unusableMoves;
}

bool8 AreAllMovesUnusable(u8 battlerId)
{
    u8 unusable = CheckMoveLimitations(battlerId, 0);

    if (unusable == MOVE_LIMITATION_ALL_MOVES_MASK) // All moves are unusable.
    {
        gProtectStructs[battlerId].noValidMoves = TRUE;
        gSelectionBattleScripts[battlerId] = BattleScript_NoMovesLeft;
		
        if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            gBattleBufferB[battlerId][3] = GetBattlerAtPosition((BATTLE_OPPOSITE(GetBattlerPosition(battlerId))) | (Random() & 2));
        else 
            gBattleBufferB[battlerId][3] = GetBattlerAtPosition(BATTLE_OPPOSITE(GetBattlerPosition(battlerId)));
    }
    else
        gProtectStructs[battlerId].noValidMoves = FALSE;

    return (unusable == MOVE_LIMITATION_ALL_MOVES_MASK);
}

static bool8 TryActivateEmergencyExit(u8 battler)
{
	u16 halfHp = gBattleMons[battler].maxHP / 2;
	
	if (gBattleStruct->hpBefore[battler] > halfHp && gBattleMons[battler].hp < halfHp && ((!(gBattleTypeFlags & BATTLE_TYPE_TRAINER)
		&& GetBattlerSide(battler) == B_SIDE_OPPONENT) || (CanBattlerSwitch(battler) && CountUsablePartyMons(battler) > 0)))
        return TRUE;
		
	return FALSE;
}

static bool8 IsImmuneToWeatherDamage(u8 battlerId, u16 ability)
{
	if ((IS_BATTLE_TYPE_GHOST_WITHOUT_SCOPE() && GetBattlerSide(battlerId) == B_SIDE_OPPONENT) || (gStatuses3[battlerId] & (STATUS3_UNDERGROUND | STATUS3_UNDERWATER))
	|| !WEATHER_HAS_EFFECT || ability == ABILITY_OVERCOAT || ability == ABILITY_MAGIC_GUARD)
	    return TRUE;
	return FALSE;
}

static void IncrementBattlerBasedEndTurnEffects(void)
{
	if (++gBattleStruct->turnEffectsBattlerId >= gBattlersCount)
	{
		gBattleStruct->turnEffectsBattlerId = 0;
		++gBattleStruct->turnEffectsTracker;
	}
}

static void IncrementSideBasedEndTurnEffects(void)
{
	if (++gBattleStruct->turnSideTracker >= B_SIDE_COUNT)
	{
		gBattleStruct->turnSideTracker = 0;
		++gBattleStruct->turnEffectsTracker;
	}
}

enum
{
	ENDTURN_ORDER,
	ENDTURN_GENERAL_COUNTERS,
	ENDTURN_WEATHER_ENDS,
	ENDTURN_WEATHER_DAMAGE, // Also weather heal abilities
	ENDTURN_EMERGENCY_EXIT,
	ENDTURN_FUTURE_SIGHT_WISH,
	ENDTURN_SEA_OF_FIRE_DAMAGE, // Also G-Max Cannonade, Vine Lash, Wildfire and Volcalith
	ENDTURN_GRASSY_TERRAIN_HEAL,
	ENDTURN_HEALER_SHED_SKIN,
	ENDTURN_BLACK_SLUDGE_LEFTOVERS,
	ENDTURN_EMERGENCY_EXIT2,
	ENDTURN_AQUA_RING_HEAL,
	ENDTURN_INGRAIN_HEAL,
	ENDTURN_LEECH_SEED_DRAIN, // The battler that get seeded
	ENDTURN_POISON_DAMAGE, // Also Poison Heal
	ENDTURN_BURN_DAMAGE,
	ENDTURN_NIGHTMARE_DAMAGE,
	ENDTURN_CURSE_DAMAGE,
	ENDTURN_BINDING_COUNTER,
	ENDTURN_OCTOLOCK_DROP,
	ENDTURN_TAUNT_ENDS, // From G-Max Meltdown
	ENDTURN_ENCORE_ENDS,
	ENDTURN_DISABLE_ENDS,
	ENDTURN_LOCK_ON_ENDS,
	ENDTURN_LASER_FOCUS_ENDS,
	ENDTURN_ENDURE_ENDS,
	ENDTURN_ION_DELUGE_ENDS,
	ENDTURN_CHARGE_ENDS,
	ENDTURN_MAGNET_RISE_ENDS,
	ENDTURN_TELEKINESIS_ENDS,
	ENDTURN_HEAL_BLOCK_ENDS,
	ENDTURN_EMBARGO_ENDS,
	ENDTURN_YAWN_ASLEEP,
	ENDTURN_PERISH_SONG_COUNTER,
	ENDTURN_ROOST_ENDS,
	ENDTURN_EMERGENCY_EXIT3,
	ENDTURN_REFLECT_ENDS,
	ENDTURN_LIGHT_SCREEN_ENDS,
	ENDTURN_SAFEGUARD_ENDS,
	ENDTURN_MIST_ENDS,
	ENDTURN_TAILWIND_ENDS,
	ENDTURN_LUCKY_CHANT_ENDS,
	ENDTURN_RAINBOW_ENDS,
	ENDTURN_SEA_OF_FIRE_ENDS,
	ENDTURN_SWAMP_ENDS,
	ENDTURN_GMAX_COUNTERS, // G-Max Vine Lash, Wildfire, Cannonade and Volcalith
	ENDTURN_AURORA_VEIL_ENDS,
	ENDTURN_TRICK_ROOM_ENDS,
	ENDTURN_GRAVITY_ENDS,
	ENDTURN_WATER_SPORT_ENDS,
	ENDTURN_MUD_SPORT_ENDS,
	ENDTURN_WONDER_ROOM_ENDS,
	ENDTURN_MAGIC_ROOM_ENDS,
	ENDTURN_TERRAIN_ENDS,
	ENDTURN_UPROAR_ENDS,
	ENDTURN_THRASH_CONFUSES,
	ENDTURN_BAD_DREAMS_HARVEST, // Also Ball Fetch, Moody, Pickup, Slow Start and Speed Boost
	ENDTURN_ORBS, // Also Sticky Barb and White Herb
	ENDTURN_EMERGENCY_EXIT4,
	ENDTURN_FORM_CHANGE, // Power Construct, Schooling, Shields Down, Zen Mode and Hunger Switch
	ENDTURN_EJECT_PACK,
	ENDTURN_COUNT,
};

bool8 DoEndTurnEffects(void)
{
	u8 side;
	u32 i, j;
	u16 ability;
	bool8 effect = FALSE;

    gHitMarker |= (HITMARKER_GRUDGE | HITMARKER_SKIP_DMG_TRACK);
	
	do
	{
		gBattlerAttacker = gBattlerTarget = gBattlerByTurnOrder[gBattleStruct->turnEffectsBattlerId];
		ability = GetBattlerAbility(gBattlerAttacker);
		side = gBattleStruct->turnSideTracker;
		
		switch (gBattleStruct->turnEffectsTracker)
		{
			case ENDTURN_ORDER:
			    for (i = 0; i < gBattlersCount; i++)
					gBattlerByTurnOrder[i] = i;
				
				for (i = 0; i < gBattlersCount - 1; i++)
				{
					for (j = i + 1; j < gBattlersCount; j++)
					{
						if (GetWhoStrikesFirst(gBattlerByTurnOrder[i], gBattlerByTurnOrder[j], TRUE) != ATTACKER_STRIKES_FIRST)
							SwapTurnOrder(i, j);
					}
				}
				++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_GENERAL_COUNTERS:
			    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
				{
					if (gDisableStructs[i].destinyBondCounter)
						--gDisableStructs[i].destinyBondCounter;
				}
				++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_WEATHER_ENDS:
			    gBattleCommunication[MULTIUSE_STATE] = FALSE;
				
				if (gBattleWeather & WEATHER_RAIN_ANY)
				{
					if ((gBattleWeather & (WEATHER_RAIN_PERMANENT | WEATHER_RAIN_PRIMAL)) || --gWishFutureKnock.weatherDuration != 0)
					{
						gBattleScripting.animArg1 = B_ANIM_RAIN_CONTINUES;
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_RAIN_CONTINUES;
					}
					else
					{
						gBattleWeather &= ~(WEATHER_RAIN_TEMPORARY);
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_RAIN_STOPPED;
						gBattleCommunication[MULTIUSE_STATE] = TRUE; // Weather ends
					}
					effect = TRUE;
				}
				else if (gBattleWeather & WEATHER_SUN_ANY)
				{
					if ((gBattleWeather & (WEATHER_SUN_PERMANENT | WEATHER_SUN_PRIMAL)) || --gWishFutureKnock.weatherDuration != 0)
					{
						gBattleScripting.animArg1 = B_ANIM_SUN_CONTINUES;
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SUN_CONTINUES;
					}
					else
					{
						gBattleWeather &= ~(WEATHER_SUN_TEMPORARY);
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SUN_STOPPED;
						gBattleCommunication[MULTIUSE_STATE] = TRUE; // Weather ends
					}
					effect = TRUE;
				}
				else if (gBattleWeather & WEATHER_SANDSTORM_ANY)
				{
					if ((gBattleWeather & WEATHER_SANDSTORM_PERMANENT) || --gWishFutureKnock.weatherDuration != 0)
					{
						gBattleScripting.animArg1 = B_ANIM_SANDSTORM_CONTINUES;
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SANDSTORM_CONTINUES;
					}
					else
					{
						gBattleWeather &= ~(WEATHER_SANDSTORM_TEMPORARY);
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SANDSTORM_STOPPED;
						gBattleCommunication[MULTIUSE_STATE] = TRUE; // Weather ends
					}
					effect = TRUE;
				}
				else if (gBattleWeather & WEATHER_HAIL_ANY)
				{
					if ((gBattleWeather & WEATHER_HAIL_PERMANENT) || --gWishFutureKnock.weatherDuration != 0)
					{
						gBattleScripting.animArg1 = B_ANIM_HAIL_CONTINUES;
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_HAIL_CONTINUES;
					}
					else
					{
						gBattleWeather &= ~(WEATHER_HAIL_TEMPORARY);
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_HAIL_STOPPED;
						gBattleCommunication[MULTIUSE_STATE] = TRUE; // Weather ends
					}
					effect = TRUE;
				}
				else if (gBattleWeather & WEATHER_FOG_ANY)
				{
					if ((gBattleWeather & WEATHER_FOG_PERMANENT) || --gWishFutureKnock.weatherDuration != 0)
					{
						gBattleScripting.animArg1 = B_ANIM_FOG_CONTINUES;
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FOG_CONTINUES;
					}
					else
					{
						gBattleWeather &= ~(WEATHER_FOG_TEMPORARY);
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FOG_STOPPED;
						gBattleCommunication[MULTIUSE_STATE] = TRUE; // Weather ends
					}
					effect = TRUE;
				}
				else if (gBattleWeather & WEATHER_STRONG_WINDS)
				{
					gBattleScripting.animArg1 = B_ANIM_STRONG_WINDS_CONTINUE;
					gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STRONG_WINDS_CONTINUES;
					effect = TRUE;
				}
				
				if (effect)
					BattleScriptExecute(BattleScript_WeatherContinuesOrEnds);
				
				++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_WEATHER_DAMAGE:
			    if (IsBattlerAlive(gBattlerAttacker))
				{
					switch (ability)
					{
						case ABILITY_RAIN_DISH:
						case ABILITY_DRY_SKIN:
						case ABILITY_ICE_BODY:
						case ABILITY_SOLAR_POWER:
						    if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, gBattlerAttacker))
								effect = TRUE;
							break;
					}
					
					if (!effect && !IsImmuneToWeatherDamage(gBattlerAttacker, ability))
					{
						if (IsBattlerWeatherAffected(gBattlerAttacker, WEATHER_SANDSTORM_ANY))
						{
							if (!IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_ROCK) && !IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_STEEL)
							&& !IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_GROUND) && ability != ABILITY_SAND_VEIL && ability != ABILITY_SAND_RUSH
						    && ability != ABILITY_SAND_FORCE)
							{
								gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DAMAGED_BY_SANDSTORM;
								effect = TRUE;
							}
						}
						else if (IsBattlerWeatherAffected(gBattlerAttacker, WEATHER_HAIL_ANY))
						{
							if (!IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_ICE) && ability != ABILITY_SNOW_CLOAK && ability != ABILITY_ICE_BODY)
							{
								gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DAMAGED_BY_HAIL;
								effect = TRUE;
							}
						}
						
						if (effect)
						{
							gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 16;
							if (gBattleMoveDamage == 0)
								gBattleMoveDamage = 1;
							
							BattleScriptExecute(BattleScript_WeatherDamage);
						}
					}
				}
				IncrementBattlerBasedEndTurnEffects();
				break;
			// These are checked each time it takes damage
			case ENDTURN_EMERGENCY_EXIT:
			case ENDTURN_EMERGENCY_EXIT2:
			case ENDTURN_EMERGENCY_EXIT3:
			case ENDTURN_EMERGENCY_EXIT4:
			    if (IsBattlerAlive(gBattlerTarget))
				{
					switch (ability)
					{
						case ABILITY_WIMP_OUT:
						case ABILITY_EMERGENCY_EXIT:
						    if (TryActivateEmergencyExit(gBattlerTarget))
							{
								gLastUsedAbility = ability;
								RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
								BattleScriptExecute(BattleScript_EmergencyExitEnd2);
								effect = TRUE;
							}
							break;
					}
				}
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_FUTURE_SIGHT_WISH:
			    // Activate in order were are set
			    if (TryDoQueuedBattleEffectsInList(gWishFutureSightQueuedEffectIds, QueuedEffects_DoWishFutureSight))
					effect = TRUE;
				else // Increment when all are done
					++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_SEA_OF_FIRE_DAMAGE:
			    // Activate in order were are set
				if (TryDoQueuedBattleEffectsInList(gSeaOfFireAndGMaxQueuedEffectIds, QueuedEffects_DoSeaOfFireAndGMaxEffects))
					effect = TRUE;
				else // Increment when all are done
					++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_GRASSY_TERRAIN_HEAL:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_HEALER_SHED_SKIN:
			    if (IsBattlerAlive(gBattlerAttacker))
				{
					switch (ability)
					{
						case ABILITY_HEALER:
						case ABILITY_SHED_SKIN:
						case ABILITY_HYDRATION:
						    if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, gBattlerAttacker))
								effect = TRUE;
							break;
					}
				}
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_BLACK_SLUDGE_LEFTOVERS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_AQUA_RING_HEAL:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_INGRAIN_HEAL:
			    if (IsBattlerAlive(gBattlerAttacker) && (gStatuses3[gBattlerAttacker] & STATUS3_ROOTED) && !BATTLER_MAX_HP(gBattlerAttacker))
                {
                    gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 16;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
					// Sing change handled in battle script
                    BattleScriptExecute(BattleScript_IngrainTurnHeal);
                    effect = TRUE;
                }
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_LEECH_SEED_DRAIN:
			    if (IsBattlerAlive(gBattlerTarget) && (gStatuses3[gBattlerTarget] & STATUS3_LEECHSEED)
				&& IsBattlerAlive(gStatuses3[gBattlerTarget] & STATUS3_LEECHSEED_BATTLER) && ability != ABILITY_MAGIC_GUARD)
                {
					gBattlerAttacker = gStatuses3[gBattlerTarget] & STATUS3_LEECHSEED_BATTLER; // HP receiver
					gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 8;
					if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    gBattleScripting.animArg1 = gBattlerAttacker;
                    gBattleScripting.animArg2 = gBattlerTarget;
                    BattleScriptExecute(BattleScript_LeechSeedTurnDrain);
                    effect = TRUE;
                }
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_POISON_DAMAGE:
			    if (IsBattlerAlive(gBattlerAttacker))
				{
					switch (ability)
					{
						case ABILITY_POISON_HEAL:
						    if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, gBattlerAttacker))
								effect = TRUE;
							break;
					}
					
					if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_POISON))
					{
						if (ability != ABILITY_POISON_HEAL && ability != ABILITY_MAGIC_GUARD)
						{
							gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 8;
							if (gBattleMoveDamage == 0)
								gBattleMoveDamage = 1;
							BattleScriptExecute(BattleScript_PoisonTurnDmg);
							effect = TRUE;
						}
					}
					else if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_TOXIC_POISON))
					{
						if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_TOXIC_COUNTER) != STATUS1_TOXIC_TURN(15))
							gBattleMons[gBattlerAttacker].status1 += STATUS1_TOXIC_TURN(1);
						
						if (ability != ABILITY_POISON_HEAL && ability != ABILITY_MAGIC_GUARD) // These prevent damage, but not the counter increase
						{
							gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 16;
							if (gBattleMoveDamage == 0)
								gBattleMoveDamage = 1;
							gBattleMoveDamage *= (gBattleMons[gBattlerAttacker].status1 & STATUS1_TOXIC_COUNTER) >> 8;
							BattleScriptExecute(BattleScript_PoisonTurnDmg);
							effect = TRUE;
						}
					}
				}
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_BURN_DAMAGE:
			    if (IsBattlerAlive(gBattlerAttacker) && (gBattleMons[gBattlerAttacker].status1 & STATUS1_BURN) && ability != ABILITY_MAGIC_GUARD)
				{
#if BURN_UPDATE
					gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 16;
#else
					gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 8;
#endif

					if (ability == ABILITY_HEATPROOF)
						gBattleMoveDamage /= 2;
					
					if (gBattleMoveDamage == 0)
						gBattleMoveDamage = 1;
					
					BattleScriptExecute(BattleScript_BurnTurnDmg);
					effect = TRUE;
				}
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_NIGHTMARE_DAMAGE:
			    if (IsBattlerAlive(gBattlerAttacker) && (gBattleMons[gBattlerAttacker].status2 & STATUS2_NIGHTMARE) && ability != ABILITY_MAGIC_GUARD
				&& ((gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP) || ability == ABILITY_COMATOSE))
				{
					gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 4;
					if (gBattleMoveDamage == 0)
						gBattleMoveDamage = 1;
					
					BattleScriptExecute(BattleScript_NightmareTurnDmg);
					effect = TRUE;
				}
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_CURSE_DAMAGE:
			    if (IsBattlerAlive(gBattlerAttacker) && (gBattleMons[gBattlerAttacker].status2 & STATUS2_CURSED) && ability != ABILITY_MAGIC_GUARD)
                {
                    gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 4;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
					
                    BattleScriptExecute(BattleScript_CurseTurnDmg);
                    effect = TRUE;
                }
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_BINDING_COUNTER:
			    if (IsBattlerAlive(gBattlerAttacker) && (gBattleMons[gBattlerAttacker].status2 & STATUS2_WRAPPED) && ability != ABILITY_MAGIC_GUARD)
                {
					gBattleMons[gBattlerAttacker].status2 -= STATUS2_WRAPPED_TURN(1);
					PrepareMoveBuffer(gBattleTextBuff1, gBattleStruct->wrappedMove[gBattlerAttacker]);
					
                    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_WRAPPED) // Damaged by wrap
                    {
                        gBattleScripting.animArg1 = gBattleStruct->wrappedMove[gBattlerAttacker];
                        gBattleScripting.animArg2 = gBattleStruct->wrappedMove[gBattlerAttacker] >> 8;
						
                        gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 16;
                        if (gBattleMoveDamage == 0)
                            gBattleMoveDamage = 1;
						
						gBattlescriptCurrInstr = BattleScript_WrapTurnDmg;
                    }
                    else  // Broke free
                        gBattlescriptCurrInstr = BattleScript_WrapEnds;
						
                    BattleScriptExecute(gBattlescriptCurrInstr);
                    effect = TRUE;
                }
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_OCTOLOCK_DROP:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_TAUNT_ENDS:
			    if (gDisableStructs[gBattlerAttacker].tauntTimer)
					--gDisableStructs[gBattlerAttacker].tauntTimer;
				
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_ENCORE_ENDS:
			    if (gDisableStructs[gBattlerAttacker].encoreTimer != 0)
                {
					// Does not have the encored move anymore
                    if (gBattleMons[gBattlerAttacker].moves[gDisableStructs[gBattlerAttacker].encoredMovePos] != gDisableStructs[gBattlerAttacker].encoredMove)
                    {
                        gDisableStructs[gBattlerAttacker].encoredMove = MOVE_NONE;
                        gDisableStructs[gBattlerAttacker].encoreTimer = 0;
                    }
                    else if (--gDisableStructs[gBattlerAttacker].encoreTimer == 0 || gBattleMons[gBattlerAttacker].pp[gDisableStructs[gBattlerAttacker].encoredMovePos] == 0)
                    {
                        gDisableStructs[gBattlerAttacker].encoredMove = MOVE_NONE;
                        gDisableStructs[gBattlerAttacker].encoreTimer = 0;
                        BattleScriptExecute(BattleScript_EncoredNoMore);
                        effect = TRUE;
                    }
                }
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_DISABLE_ENDS:
				if (gDisableStructs[gBattlerAttacker].disableTimer != 0)
                {
                    for (i = 0; i < MAX_MON_MOVES; ++i)
                    {
                        if (gDisableStructs[gBattlerAttacker].disabledMove == gBattleMons[gBattlerAttacker].moves[i])
                            break;
                    }
                    if (i == MAX_MON_MOVES)  // Does not have the disabled move anymore
                    {
                        gDisableStructs[gBattlerAttacker].disabledMove = MOVE_NONE;
                        gDisableStructs[gBattlerAttacker].disableTimer = 0;
                    }
                    else if (--gDisableStructs[gBattlerAttacker].disableTimer == 0) // Disable ends
                    {
                        gDisableStructs[gBattlerAttacker].disabledMove = MOVE_NONE;
                        BattleScriptExecute(BattleScript_DisabledNoMore);
                        effect = TRUE;
                    }
                }
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_LOCK_ON_ENDS:
			    if (gStatuses3[gBattlerAttacker] & STATUS3_ALWAYS_HITS)
                    gStatuses3[gBattlerAttacker] -= STATUS3_ALWAYS_HITS_TURN(1);
				
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_LASER_FOCUS_ENDS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_ENDURE_ENDS:
			    gProtectStructs[gBattlerAttacker].endured = FALSE;
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_ION_DELUGE_ENDS:
			    ++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_CHARGE_ENDS:
			    if (gDisableStructs[gBattlerAttacker].chargeTimer && --gDisableStructs[gBattlerAttacker].chargeTimer == 0)
                    gStatuses3[gBattlerAttacker] &= ~(STATUS3_CHARGED_UP);
				
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_MAGNET_RISE_ENDS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_TELEKINESIS_ENDS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_HEAL_BLOCK_ENDS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_EMBARGO_ENDS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_YAWN_ASLEEP:
			    if (IsBattlerAlive(gBattlerAttacker) && (gStatuses3[gBattlerAttacker] & STATUS3_YAWN))
                {
                    gStatuses3[gBattlerAttacker] -= STATUS3_YAWN_TURN(1);
					
					SetMoveEffect(MOVE_EFFECT_SLEEP, TRUE, TRUE);
					
                    if (!(gStatuses3[gBattlerAttacker] & STATUS3_YAWN) && DoMoveEffect(TRUE, FALSE, 0))
					{
						BattleScriptExecute(BattleScript_YawnMakesAsleep);
						effect = TRUE;
					}
				}
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_PERISH_SONG_COUNTER:
			    if (IsBattlerAlive(gBattlerAttacker) && (gStatuses3[gBattlerAttacker] & STATUS3_PERISH_SONG))
				{
					PrepareByteNumberBuffer(gBattleTextBuff1, 1, gDisableStructs[gBattlerAttacker].perishSongTimer);
					
					if (gDisableStructs[gBattlerAttacker].perishSongTimer == 0)
					{
						gStatuses3[gBattlerAttacker] &= ~(STATUS3_PERISH_SONG);
						gBattleMoveDamage = gBattleMons[gBattlerAttacker].hp;
						gBattlescriptCurrInstr = BattleScript_PerishSongTakesLife;
					}
					else
					{
						--gDisableStructs[gBattlerAttacker].perishSongTimer;
						gBattlescriptCurrInstr = BattleScript_PerishSongCountGoesDown;
					}
					BattleScriptExecute(gBattlescriptCurrInstr);
					effect = TRUE;
				}
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_ROOST_ENDS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_REFLECT_ENDS:
                if ((gSideStatuses[side] & SIDE_STATUS_REFLECT) && --gSideTimers[side].reflectTimer == 0)
                {
					gSideStatuses[side] &= ~(SIDE_STATUS_REFLECT);
					gBattlerAttacker = gSideTimers[side].reflectBattlerId;
					PrepareMoveBuffer(gBattleTextBuff1, MOVE_REFLECT);
					gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SIDE_STATUS_WORE_OFF;
					BattleScriptExecute(BattleScript_SideStatusWoreOff);
					effect = TRUE;
                }
			    IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_LIGHT_SCREEN_ENDS:
                if ((gSideStatuses[side] & SIDE_STATUS_LIGHTSCREEN) && --gSideTimers[side].lightscreenTimer == 0)
                {
					gSideStatuses[side] &= ~(SIDE_STATUS_LIGHTSCREEN);
					gBattlerAttacker = gSideTimers[side].lightscreenBattlerId;
					PrepareMoveBuffer(gBattleTextBuff1, MOVE_LIGHT_SCREEN);
					gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SIDE_STATUS_WORE_OFF;
					BattleScriptExecute(BattleScript_SideStatusWoreOff);
					effect = TRUE;
                }
				IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_SAFEGUARD_ENDS:
                if ((gSideStatuses[side] & SIDE_STATUS_SAFEGUARD) && --gSideTimers[side].safeguardTimer == 0)
                {
					gSideStatuses[side] &= ~(SIDE_STATUS_SAFEGUARD);
					gBattlerAttacker = gSideTimers[side].safeguardBattlerId;
					gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SAFEGUARD_WORE_OFF;
					BattleScriptExecute(BattleScript_SideStatusWoreOff);
					effect = TRUE;
                }
				IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_MIST_ENDS:
                if ((gSideStatuses[side] & SIDE_STATUS_MIST) && --gSideTimers[side].mistTimer == 0)
                {
                    gSideStatuses[side] &= ~(SIDE_STATUS_MIST);
					gBattlerAttacker = gSideTimers[side].mistBattlerId;
					PrepareMoveBuffer(gBattleTextBuff1, MOVE_MIST);
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SIDE_STATUS_WORE_OFF;
					BattleScriptExecute(BattleScript_SideStatusWoreOff);
					effect = TRUE;
                }
				IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_TAILWIND_ENDS:
			    IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_LUCKY_CHANT_ENDS:
			    IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_RAINBOW_ENDS:
			    IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_SEA_OF_FIRE_ENDS:
			    IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_SWAMP_ENDS:
			    IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_GMAX_COUNTERS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_AURORA_VEIL_ENDS:
			    IncrementSideBasedEndTurnEffects();
				break;
			case ENDTURN_TRICK_ROOM_ENDS:
			    ++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_GRAVITY_ENDS:
			    ++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_WATER_SPORT_ENDS:
			    ++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_MUD_SPORT_ENDS:
			    ++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_WONDER_ROOM_ENDS:
			    ++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_MAGIC_ROOM_ENDS:
			    ++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_TERRAIN_ENDS:
			    ++gBattleStruct->turnEffectsTracker;
				break;
			case ENDTURN_UPROAR_ENDS:
			    if (IsBattlerAlive(gBattlerAttacker) && (gBattleMons[gBattlerAttacker].status2 & STATUS2_UPROAR))
                {
					gBattleMons[gBattlerAttacker].status2 -= STATUS2_UPROAR_TURN(1);
					
					if (WasUnableToUseMove(gBattlerAttacker))
					{
						CancelMultiTurnMoves(gBattlerAttacker);
						gBattleCommunication[MULTISTRING_CHOOSER] = 1;
					}
					else if (gBattleMons[gBattlerAttacker].status2 & STATUS2_UPROAR)
					{
						gBattleCommunication[MULTISTRING_CHOOSER] = 0;
						gBattleMons[gBattlerAttacker].status2 |= STATUS2_MULTIPLETURNS;
					}
					else
					{
						CancelMultiTurnMoves(gBattlerAttacker);
						gBattleCommunication[MULTISTRING_CHOOSER] = 1;
					}
					BattleScriptExecute(BattleScript_PrintUproarOverTurns);
					effect = TRUE;
                }
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_THRASH_CONFUSES:
			    if (IsBattlerAlive(gBattlerAttacker) && (gBattleMons[gBattlerAttacker].status2 & STATUS2_LOCK_CONFUSE))
				{
					gBattleMons[gBattlerAttacker].status2 -= STATUS2_LOCK_CONFUSE_TURN(1);
					
                    if (WasUnableToUseMove(gBattlerAttacker))
                        CancelMultiTurnMoves(gBattlerAttacker);
                    else if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_LOCK_CONFUSE) && (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS))
                    {
                        gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_MULTIPLETURNS);
						
						SetMoveEffect(MOVE_EFFECT_CONFUSION, TRUE, FALSE);
						
                        if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_CONFUSION) && DoMoveEffect(TRUE, FALSE, 0))
                        {
							BattleScriptExecute(BattleScript_ThrashConfuses);
							effect = TRUE;
                        }
                    }
				}
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_BAD_DREAMS_HARVEST:
			    if (IsBattlerAlive(gBattlerAttacker))
				{
					switch (ability)
					{
						case ABILITY_TRUANT:
						case ABILITY_BAD_DREAMS:
						case ABILITY_HARVEST:
						case ABILITY_SPEED_BOOST:
						case ABILITY_MOODY:
						case ABILITY_SLOW_START:
						case ABILITY_PICKUP:
						case ABILITY_BALL_FETCH:
						    if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, gBattlerAttacker))
								effect = TRUE;
							break;
					}
				}
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_ORBS:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_FORM_CHANGE:
			    if (IsBattlerAlive(gBattlerAttacker))
				{
					switch (ability)
					{
						case ABILITY_POWER_CONSTRUCT:
						case ABILITY_ZEN_MODE:
						case ABILITY_SCHOOLING:
						case ABILITY_SHIELDS_DOWN:
						case ABILITY_HUNGER_SWITCH:
						    if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, gBattlerAttacker))
								effect = TRUE;
							break;
					}
				}
				IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_EJECT_PACK:
			    IncrementBattlerBasedEndTurnEffects();
				break;
			case ENDTURN_COUNT:
			    effect = TRUE;
				break;
		}
	} while (!effect);
	
	if (gBattleMainFunc != BattleTurnPassed)
		return TRUE;
	else
	{
		gHitMarker &= ~(HITMARKER_GRUDGE | HITMARKER_SKIP_DMG_TRACK);
		return FALSE;
	}
}

#define FAINTED_ACTIONS_MAX_CASE 8

bool8 HandleFaintedMonActions(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
        return FALSE;
    do
    {
        s32 i;
        switch (gBattleStruct->faintedActionsState)
        {
        case 0:
            gBattleStruct->faintedActionsBattlerId = 0;
            ++gBattleStruct->faintedActionsState;
            for (i = 0; i < gBattlersCount; ++i)
            {
                if (gAbsentBattlerFlags & gBitTable[i] && !HasNoMonsToSwitch(i, PARTY_SIZE, PARTY_SIZE))
                    gAbsentBattlerFlags &= ~(gBitTable[i]);
            }
            // fall through
        case 1:
            do
            {
                gBattlerFainted = gBattlerTarget = gBattleStruct->faintedActionsBattlerId;
                if (gBattleMons[gBattleStruct->faintedActionsBattlerId].hp == 0
                 && !(gBattleStruct->givenExpMons & gBitTable[gBattlerPartyIndexes[gBattleStruct->faintedActionsBattlerId]])
                 && !(gAbsentBattlerFlags & gBitTable[gBattleStruct->faintedActionsBattlerId]))
                {
                    BattleScriptExecute(BattleScript_GiveExp);
                    gBattleStruct->faintedActionsState = 2;
                    return TRUE;
                }
            } while (++gBattleStruct->faintedActionsBattlerId != gBattlersCount);
            gBattleStruct->faintedActionsState = 3;
            break;
        case 2:
            OpponentSwitchInResetSentPokesToOpponentValue(gBattlerFainted);
            if (++gBattleStruct->faintedActionsBattlerId == gBattlersCount)
                gBattleStruct->faintedActionsState = 3;
            else
                gBattleStruct->faintedActionsState = 1;
			
			// Don't switch mons until all pokemon performed their actions or the battle's over.
            if (gBattleOutcome == 0 && !NoAliveMonsForEitherParty() && gCurrentTurnActionNumber != gBattlersCount)
            {
                gAbsentBattlerFlags |= gBitTable[gBattlerFainted];
                return FALSE;
            }
            break;
        case 3:
		    // Don't switch mons until all pokemon performed their actions or the battle's over.
            if (gBattleOutcome == 0 && !NoAliveMonsForEitherParty() && gCurrentTurnActionNumber != gBattlersCount)
                return FALSE;
            gBattleStruct->faintedActionsBattlerId = 0;
            ++gBattleStruct->faintedActionsState;
            // fall through
        case 4:
            do
            {
                gBattlerFainted = gBattlerTarget = gBattleStruct->faintedActionsBattlerId;
                if (gBattleMons[gBattleStruct->faintedActionsBattlerId].hp == 0
                 && !(gAbsentBattlerFlags & gBitTable[gBattleStruct->faintedActionsBattlerId]))
                {
                    BattleScriptExecute(BattleScript_HandleFaintedMon);
                    gBattleStruct->faintedActionsState = 5;
                    return TRUE;
                }
            } while (++gBattleStruct->faintedActionsBattlerId != gBattlersCount);
            gBattleStruct->faintedActionsState = 6;
            break;
        case 5:
            if (++gBattleStruct->faintedActionsBattlerId == gBattlersCount)
                gBattleStruct->faintedActionsState = 6;
            else
                gBattleStruct->faintedActionsState = 4;
            break;
        case 6: // All battlers switch-in abilities happen here to prevent them happening against an empty field.
		    for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattleStruct->switchInAbilityPostponed & gBitTable[i])
                {
                    if (DoSwitchInAbilitiesItems(i))
                        return TRUE;
                    gBattleStruct->switchInAbilityPostponed &= ~(gBitTable[i]);
                }
            }
            gBattleStruct->faintedActionsState++;
            break;
		case 7:
		    if (ItemBattleEffects(1, 0, TRUE))
                return TRUE;
            ++gBattleStruct->faintedActionsState;
            break;
        case FAINTED_ACTIONS_MAX_CASE:
            break;
        }
    } while (gBattleStruct->faintedActionsState != FAINTED_ACTIONS_MAX_CASE);
    return FALSE;
}

void TryClearRageStatuses(void)
{
    s32 i;

    for (i = 0; i < gBattlersCount; ++i)
	{
        if ((gBattleMons[i].status2 & STATUS2_RAGE) && gChosenMoveByBattler[i] != MOVE_RAGE)
            gBattleMons[i].status2 &= ~(STATUS2_RAGE);
	}
}

static void SetRandomMultiHitCounter(void)
{
	gMultiHitCounter = Random() % 100;
	
	if (gMultiHitCounter < 35)
		gMultiHitCounter = 2;
	else if (gMultiHitCounter < 35 + 35)
		gMultiHitCounter = 3;
	else if (gMultiHitCounter < 35 + 35 + 15)
		gMultiHitCounter = 4;
	else
		gMultiHitCounter = 5;
}

u8 AtkCanceller_UnableToUseMove(void)
{
    u8 effect = 0;

    do
    {
        switch (gBattleStruct->atkCancellerTracker)
        {
			case CANCELLER_FLAGS: // flags clear
			    gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_DESTINY_BOND);
				gStatuses3[gBattlerAttacker] &= ~(STATUS3_GRUDGE);
				gBattleStruct->targetsDone[gBattlerAttacker] = 0;
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_ASLEEP: // check being asleep
			    if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP))
				{
					if (UproarWakeUpCheck())
					{
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_UPROAR_WOKEUP;
						effect = 1; // Wake up
					}
					else
					{
						u8 toSub = STATUS1_SLEEP_TURN(GetBattlerAbility(gBattlerAttacker) == ABILITY_EARLY_BIRD ? 2 : 1);
						
						if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP) <= toSub)
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WOKEUP;
							effect = 1; // Wake up
						}
						else
						{
							gBattleMons[gBattlerAttacker].status1 -= toSub;
							
							if (gBattleMoves[gCurrentMove].effect != EFFECT_SNORE && gBattleMoves[gCurrentMove].effect != EFFECT_SLEEP_TALK)
							{
								gBattlescriptCurrInstr = BattleScript_MoveUsedIsAsleep;
								gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
								effect = 2; // Asleep
							}
						}
					}
					
					if (effect == 1)
					{
						ClearBattlerStatus(gBattlerAttacker);
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_MoveUsedWokeUp;
					}
                }
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_FROZEN: // check being frozen
			    // unfreezing via a move thawing happens in CANCELLER_THAW
			    if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_FREEZE) && !gBattleMoves[gCurrentMove].flags.thawUser)
				{
					if (Random() % 5)
					{
						gBattlescriptCurrInstr = BattleScript_MoveUsedIsFrozen;
						gHitMarker |= HITMARKER_NO_ATTACKSTRING;
					}
					else // unfreeze
					{
						ClearBattlerStatus(gBattlerAttacker);
						PrepareMonNickWithPrefixBuffer(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker]);
						gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DEFROST;
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_MoveUsedUnfroze;
					}
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_TRUANT: // truant
			    if (GetBattlerAbility(gBattlerAttacker) == ABILITY_TRUANT && gDisableStructs[gBattlerAttacker].truantCounter)
				{
					gMoveResultFlags |= MOVE_RESULT_MISSED;
					gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_LOAFING_AROUND;
					gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
					CancelMultiTurnMoves(gBattlerAttacker);
					gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_RECHARGE: // recharge
			    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_RECHARGE)
				{
					gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_RECHARGE);
					gDisableStructs[gBattlerAttacker].rechargeTimer = 0;
					gBattlescriptCurrInstr = BattleScript_MoveUsedMustRecharge;
					CancelMultiTurnMoves(gBattlerAttacker);
					gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_FLINCH: // flinch
			    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_FLINCHED)
				{
					gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_FLINCHED);
					gProtectStructs[gBattlerAttacker].flinchImmobility = 1;
					gBattlescriptCurrInstr = BattleScript_MoveUsedFlinched;
					CancelMultiTurnMoves(gBattlerAttacker);
					gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_DISABLED: // disabled move
			    if (gDisableStructs[gBattlerAttacker].disabledMove && gDisableStructs[gBattlerAttacker].disabledMove == gCurrentMove)
				{
					gProtectStructs[gBattlerAttacker].usedDisabledMove = 1;
					gBattlescriptCurrInstr = BattleScript_MoveUsedIsDisabled;
					CancelMultiTurnMoves(gBattlerAttacker);
					gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_TAUNTED: // taunt
			    if (gDisableStructs[gBattlerAttacker].tauntTimer && IS_MOVE_STATUS(gCurrentMove))
				{
					gProtectStructs[gBattlerAttacker].usedTauntedMove = 1;
					gBattlescriptCurrInstr = BattleScript_MoveUsedIsTaunted;
					CancelMultiTurnMoves(gBattlerAttacker);
					gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_IMPRISONED: // imprisoned
			    if (GetImprisonedMovesCount(gBattlerAttacker, gCurrentMove))
				{
					gProtectStructs[gBattlerAttacker].usedImprisonedMove = 1;
					gBattlescriptCurrInstr = BattleScript_MoveUsedIsImprisoned;
					CancelMultiTurnMoves(gBattlerAttacker);
					gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_CONFUSED: // confusion
			    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_CONFUSION)
				{
					gBattleMons[gBattlerAttacker].status2 -= STATUS2_CONFUSION_TURN(1);
					
					if (gBattleMons[gBattlerAttacker].status2 & STATUS2_CONFUSION)
					{
#if CONFUSION_UPDATE
                        if (Random() % 3)
#else
		                if (Random() & 1)
#endif
                        {
							BattleScriptPushCursor();
							gBattlescriptCurrInstr = BattleScript_MoveUsedIsConfused;
						}
						else // confusion dmg
						{
							gBattleMoveDamage = CalculateConfusionDamage();
							CancelMultiTurnMoves(gBattlerAttacker);
							gProtectStructs[gBattlerAttacker].confusionSelfDmg = 1;
							gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
							gBattlescriptCurrInstr = BattleScript_MoveUsedIsConfusedSefHit;
						}
					}
					else // snapped out
					{
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_MoveUsedIsConfusedNoMore;
					}
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_PARALYSED: // paralysis
			    if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_PARALYSIS) && (Random() % 4) == 0)
				{
					gProtectStructs[gBattlerAttacker].prlzImmobility = 1;
					gBattlescriptCurrInstr = BattleScript_MoveUsedIsParalyzed;
					CancelMultiTurnMoves(gBattlerAttacker);
					gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_GHOST: // GHOST in pokemon tower
                if (IS_BATTLE_TYPE_GHOST_WITHOUT_SCOPE())
				{
					gBattlescriptCurrInstr = GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER ? BattleScript_TooScaredToMove : BattleScript_GhostGetOutGetOut;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_IN_LOVE: // infatuation
			    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_INFATUATION)
				{
					gBattleScripting.battler = gDisableStructs[gBattlerAttacker].infatuatedWith;
					
					if (Random() & 1)
						BattleScriptPushCursor();
					else
					{
						BattleScriptPush(BattleScript_MoveUsedIsInLoveCantAttack);
						gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
						gProtectStructs[gBattlerAttacker].loveImmobility = 1;
						CancelMultiTurnMoves(gBattlerAttacker);
					}
					gBattlescriptCurrInstr = BattleScript_MoveUsedIsInLove;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_BIDE: // bide
			    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_BIDE)
				{
					gBattleMons[gBattlerAttacker].status2 -= STATUS2_BIDE_TURN(1);
					
					if (gBattleMons[gBattlerAttacker].status2 & STATUS2_BIDE)
						gBattlescriptCurrInstr = BattleScript_BideStoringEnergy;
					else
					{
						if (gTakenDmg[gBattlerAttacker])
						{
							gCurrentMove = MOVE_BIDE;
							gBattleScripting.bideDmg = gTakenDmg[gBattlerAttacker] * 2;
							gBattlerTarget = gTakenDmgByBattler[gBattlerAttacker];
							
							if (!IsBattlerAlive(gBattlerTarget))
								gBattlerTarget = GetMoveTarget(gCurrentMove, 1);
							
							gBattlescriptCurrInstr = BattleScript_BideAttack;
						}
						else
							gBattlescriptCurrInstr = BattleScript_BideNoEnergyToAttack;
					}
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_THAW: // move thawing
			    if (gBattleMons[gBattlerAttacker].status1 & STATUS1_FREEZE && gBattleMoves[gCurrentMove].flags.thawUser)
				{
					ClearBattlerStatus(gBattlerAttacker);
					BattleScriptPushCursor();
					gBattlescriptCurrInstr = BattleScript_MoveUsedUnfroze;
					gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DEFROST_BY_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_PRIMAL_WEATHER: // primal weather
			    if (!IS_MOVE_STATUS(gCurrentMove))
				{
					switch (gBattleStruct->dynamicMoveType)
					{
						case TYPE_FIRE:
							if (IsBattlerWeatherAffected(gBattlerAttacker, WEATHER_RAIN_PRIMAL))
							{
								gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FIZZLES_ATTACK;
								++effect;
							}
							break;
						case TYPE_WATER:
							if (IsBattlerWeatherAffected(gBattlerAttacker, WEATHER_SUN_PRIMAL))
							{
								gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_EVAPORATE_ATTACK;
								++effect;
							}
							break;
					}
					if (effect)
					{
						PrepareTypeBuffer(gBattleTextBuff1, gBattleStruct->dynamicMoveType);
						gBattlescriptCurrInstr = BattleScript_PrimalWeatherBlocksMove;
						CancelMultiTurnMoves(gBattlerAttacker);
						gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
						gMoveResultFlags = MOVE_RESULT_FAILED;
					}
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_SPORE_MOVE:
			    if (gBattlerAttacker != gBattlerTarget && gBattleMoves[gCurrentMove].flags.powderMove && !IsBattlerAffectedBySpore(gBattlerTarget))
				{
					if (GetBattlerAbility(gBattlerTarget) != ABILITY_OVERCOAT)
						gBattleScripting.bypassAbilityPopUp = TRUE;
					
					CancelMultiTurnMoves(gBattlerAttacker);
					gBattlescriptCurrInstr = BattleScript_PowderMoveNoEffect;
					gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
					++effect;
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_DAMP:
			    if (gBattleMoves[gCurrentMove].effect == EFFECT_EXPLOSION)
				{
					u8 i;
					
					for (i = 0; i < gBattlersCount; i++)
					{
						if (GetBattlerAbility(i) == ABILITY_DAMP)
							break;
					}
					if (i != gBattlersCount)
					{
						gBattleScripting.battler = i;
						gLastUsedAbility = GetBattlerAbility(i);
						RecordAbilityBattle(i, gLastUsedAbility);
						gBattlescriptCurrInstr = BattleScript_DampStopsExplosion;
						gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
						++effect;
					}
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_MULTIHIT_MOVE:
			    if (!gSpecialStatuses[gBattlerAttacker].multiHitOn) // For don't stay in an infinite multi-hit loop
				{
					if (gBattleMoves[gCurrentMove].effect == EFFECT_MULTI_HIT)
					{
						if (GetBattlerAbility(gBattlerAttacker) == ABILITY_SKILL_LINK)
							gMultiHitCounter = 5;
						else
							SetRandomMultiHitCounter();
						
						PrepareByteNumberBuffer(gBattleScripting.multihitString, 1, 0);
					}
					else if (gBattleMoves[gCurrentMove].effect == EFFECT_BEAT_UP)
					{
						u8 i;
						struct Pokemon *party = GetBattlerParty(gBattlerAttacker);
						
						for (i = 0; i < PARTY_SIZE; i++)
						{
							if (MonCanBattle(&party[i]) && !GetMonData(&party[i], MON_DATA_STATUS))
							    ++gMultiHitCounter;
						}
						gBattleCommunication[MULTIUSE_STATE] = 0; // For later
						PrepareByteNumberBuffer(gBattleScripting.multihitString, 1, 0);
					}
					else if (gBattleMoves[gCurrentMove].flags.strikeCount > 1)
					{
						gBattleScripting.tripleKickPower = 0;
						gMultiHitCounter = gBattleMoves[gCurrentMove].flags.strikeCount;
						PrepareByteNumberBuffer(gBattleScripting.multihitString, 1, 0);
					}
				}
				++gBattleStruct->atkCancellerTracker;
				break;
			case CANCELLER_END:
			    break;
        }

    }
	while (gBattleStruct->atkCancellerTracker != CANCELLER_END && !effect);
	
    return effect;
}

bool8 HasNoMonsToSwitch(u8 battler, u8 partyIdBattlerOn1, u8 partyIdBattlerOn2)
{
    u8 playerId, flankId;
    struct Pokemon *party;
    s32 i;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        return FALSE;
    }
    else
    {
		party = GetBattlerParty(battler);
		
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        {
            playerId = GetBattlerMultiplayerId(battler);
            flankId = GetLinkTrainerFlankId(playerId);
			
            for (i = flankId * 3; i < flankId * 3 + 3; ++i)
            {
                if (MonCanBattle(&party[i]))
                    break;
            }
            return (i == flankId * 3 + 3);
        }
        else
        {
            if (GetBattlerSide(battler) == B_SIDE_OPPONENT)
            {
                playerId = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
                flankId = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            }
            else
            {
                playerId = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
                flankId = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            }
            if (partyIdBattlerOn1 == PARTY_SIZE)
                partyIdBattlerOn1 = gBattlerPartyIndexes[playerId];
            if (partyIdBattlerOn2 == PARTY_SIZE)
                partyIdBattlerOn2 = gBattlerPartyIndexes[flankId];
            for (i = 0; i < PARTY_SIZE; ++i)
            {
                if (MonCanBattle(&party[i]) && i != partyIdBattlerOn1 && i != partyIdBattlerOn2 && i != gBattleStruct->monToSwitchIntoId[flankId]
                 && i != playerId[gBattleStruct->monToSwitchIntoId])
                    break;
            }
            return (i == PARTY_SIZE);
        }
    }
}

void CopyStatusStringToBattleBuffer1(u32 status)
{
	if (status & STATUS1_PSN_ANY)
		StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
	else if (status & STATUS1_PARALYSIS)
		StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
	else if (status & STATUS1_BURN)
		StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
	else if (status & STATUS1_FREEZE)
		StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
	else if (status & STATUS1_SLEEP)
		StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
}

void ClearBattlerStatus(u8 battler)
{
	CopyStatusStringToBattleBuffer1(gBattleMons[battler].status1);
	gBattleMons[battler].status1 = 0;
	gBattleMons[battler].status2 &= ~(STATUS2_NIGHTMARE); // fix nightmare glitch
	gBattleScripting.battler = battler;
	BtlController_EmitSetMonData(battler, BUFFER_A, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[battler].status1);
	MarkBattlerForControllerExec(battler);
}

static bool8 IsAbilityBlockedByGhostBattle(u16 ability)
{
	if (IS_BATTLE_TYPE_GHOST_WITHOUT_SCOPE())
	{
		if (ability == ABILITY_INTIMIDATE || ability == ABILITY_TRACE || ability == ABILITY_ANTICIPATION || ability == ABILITY_DOWNLOAD
		|| ability == ABILITY_FOREWARN || ability == ABILITY_FRISK || ability == ABILITY_UNNERVE || ability == ABILITY_IMPOSTER)
			return TRUE;
	}
	return FALSE;
}

static bool8 MoveHasFlinchChance(u16 move)
{
	switch (gBattleMoves[move].effect)
	{
		case EFFECT_TWO_TURNS_ATTACK:
		    if (gBattleMoves[move].argument != MOVE_EFFECT_FLINCH)
				break;
		// fallthrough
		case EFFECT_FLINCH_HIT:
		case EFFECT_FLINCH_STATUS:
		case EFFECT_FAKE_OUT:
		case EFFECT_FLING:
		case EFFECT_SECRET_POWER:
		case EFFECT_SNORE:
		    return TRUE;
	}
	return FALSE;
}

#define IS_MULTIHIT_FINAL_STRIKE (gMultiHitCounter <= 1)

u8 AbilityBattleEffects(u8 caseId, u8 battler)
{
    u8 moveType, effect = 0, moveTarget;
	s32 i;

    if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
	{
		gLastUsedAbility = GetBattlerAbility(battler);
		
		if (gBattlerAttacker >= gBattlersCount)
			gBattlerAttacker = battler;
		
		if (gBattlerTarget >= gBattlersCount)
			gBattlerTarget = battler;
		
		moveType = gBattleStruct->dynamicMoveType;
		
		switch (caseId)
		{
			case ABILITYEFFECT_SWITCH_IN_WEATHER: // not an actual ability effect, but handled here.
			    switch (GetCurrentWeather())
				{
					case WEATHER_RAIN:
					case WEATHER_RAIN_THUNDERSTORM:
					case WEATHER_DOWNPOUR:
					    if (!(gBattleWeather & WEATHER_RAIN_ANY))
						{
							gBattleWeather = (WEATHER_RAIN_TEMPORARY | WEATHER_RAIN_PERMANENT);
							gBattleScripting.animArg1 = B_ANIM_RAIN_CONTINUES;
							++effect;
						}
						break;
					case WEATHER_SANDSTORM:
					    if (!(gBattleWeather & WEATHER_SANDSTORM_ANY))
						{
							gBattleWeather = (WEATHER_SANDSTORM_PERMANENT | WEATHER_SANDSTORM_TEMPORARY);
							gBattleScripting.animArg1 = B_ANIM_SANDSTORM_CONTINUES;
							++effect;
						}
						break;
					case WEATHER_SNOW:
					    if (!(gBattleWeather & WEATHER_HAIL_ANY))
						{
							gBattleWeather = (WEATHER_HAIL_PERMANENT | WEATHER_HAIL_TEMPORARY);
							gBattleScripting.animArg1 = B_ANIM_HAIL_CONTINUES;
							++effect;
						}
						break;
#if FOG_IN_BATTLE
					case WEATHER_FOG_HORIZONTAL:
					case WEATHER_FOG_DIAGONAL:
				        if (!(gBattleWeather & WEATHER_FOG_ANY))
						{
							gBattleWeather = (WEATHER_FOG_PERMANENT | WEATHER_FOG_TEMPORARY);
							gBattleScripting.animArg1 = B_ANIM_FOG_CONTINUES;
							++effect;
						}
						break;
#endif
				}
				if (effect)
				{
					gBattleScripting.battler = battler;
					gBattleCommunication[MULTISTRING_CHOOSER] = GetCurrentWeather();
                    BattleScriptPushCursorAndCallback(BattleScript_OverworldWeatherStarts);
				}
				break;
			case ABILITYEFFECT_ON_SWITCHIN:
			    if (IsAbilityBlockedByGhostBattle(gLastUsedAbility))
					return effect;
			
			    gBattleScripting.battler = battler;
			
			    switch (gLastUsedAbility)
				{
					case ABILITY_DRIZZLE:
					    if (TryChangeBattleWeather(battler, ENUM_WEATHER_RAIN))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 0;
							gBattleScripting.animArg1 = B_ANIM_RAIN_CONTINUES;
							BattleScriptPushCursorAndCallback(BattleScript_SwitchInWeatherAbilityActivates);
							++effect;
						}
						break;
					case ABILITY_SAND_STREAM:
					    if (TryChangeBattleWeather(battler, ENUM_WEATHER_SANDSTORM))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 1;
							gBattleScripting.animArg1 = B_ANIM_SANDSTORM_CONTINUES;
							BattleScriptPushCursorAndCallback(BattleScript_SwitchInWeatherAbilityActivates);
							++effect;
						}
						break;
					case ABILITY_DROUGHT:
					    if (TryChangeBattleWeather(battler, ENUM_WEATHER_SUN))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 2;
							gBattleScripting.animArg1 = B_ANIM_SUN_CONTINUES;
							BattleScriptPushCursorAndCallback(BattleScript_SwitchInWeatherAbilityActivates);
							++effect;
						}
						break;
					case ABILITY_SNOW_WARNING:
					    if (TryChangeBattleWeather(battler, ENUM_WEATHER_HAIL))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 3;
							gBattleScripting.animArg1 = B_ANIM_HAIL_CONTINUES;
							BattleScriptPushCursorAndCallback(BattleScript_SwitchInWeatherAbilityActivates);
							++effect;
						}
						break;
					case ABILITY_PRIMORDIAL_SEA:
					    if (TryChangeBattleWeather(battler, ENUM_WEATHER_RAIN_PRIMAL))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 4;
							gBattleScripting.animArg1 = B_ANIM_RAIN_CONTINUES;
							BattleScriptPushCursorAndCallback(BattleScript_SwitchInWeatherAbilityActivates);
							++effect;
						}
						break;
					case ABILITY_DESOLATE_LAND:
					    if (TryChangeBattleWeather(battler, ENUM_WEATHER_SUN_PRIMAL))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 5;
							gBattleScripting.animArg1 = B_ANIM_SUN_CONTINUES;
							BattleScriptPushCursorAndCallback(BattleScript_SwitchInWeatherAbilityActivates);
							++effect;
						}
						break;
					case ABILITY_DELTA_STREAM:
					    if (TryChangeBattleWeather(battler, ENUM_WEATHER_STRONG_WINDS))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 6;
							gBattleScripting.animArg1 = B_ANIM_STRONG_WINDS_CONTINUE;
							BattleScriptPushCursorAndCallback(BattleScript_SwitchInWeatherAbilityActivates);
							++effect;
						}
						break;
					case ABILITY_INTIMIDATE:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattlerAttacker = battler;
							BattleScriptPushCursorAndCallback(BattleScript_IntimidateActivates);
							++effect;
						}
						break;
					case ABILITY_TRACE:
					    if (!gSpecialStatuses[battler].traced)
						{
							gSpecialStatuses[battler].traced = TRUE;
							gBattleResources->flags->flags[battler] |= RESOURCE_FLAG_TRACED;
						}
						break;
					case ABILITY_CLOUD_NINE:
					case ABILITY_AIR_LOCK:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							BattleScriptPushCursorAndCallback(BattleScript_AirLock);
							++effect;
						}
					    break;
					case ABILITY_MOLD_BREAKER:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 0;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_SLOW_START:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gDisableStructs[battler].slowStartTimer = 5;
							gBattleCommunication[MULTISTRING_CHOOSER] = 1;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_IMPOSTER:
						if (IsBattlerAlive(BATTLE_OPPOSITE(battler)) && CanTransformIntoBattler(battler, BATTLE_OPPOSITE(battler)) && !gDisableStructs[battler].imposterActivated)
						{
							gDisableStructs[battler].imposterActivated = TRUE;
							gBattlerAttacker = battler;
							gBattlerTarget = BATTLE_OPPOSITE(battler);
							BattleScriptPushCursorAndCallback(BattleScript_ImposterActivates);
							++effect;
						}
						break;
					case ABILITY_TURBOBLAZE:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 3;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_TERAVOLT:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 4;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_DARK_AURA:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 5;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_FAIRY_AURA:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 6;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_AURA_BREAK:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 7;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_COMATOSE:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 8;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_PRESSURE:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 16;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_VESSEL_OF_RUIN:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							PrepareStatBuffer(gBattleTextBuff1, STAT_SPATK);
							gBattleCommunication[MULTISTRING_CHOOSER] = 18;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_SWORD_OF_RUIN:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							PrepareStatBuffer(gBattleTextBuff1, STAT_DEF);
							gBattleCommunication[MULTISTRING_CHOOSER] = 18;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_TABLETS_OF_RUIN:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							PrepareStatBuffer(gBattleTextBuff1, STAT_ATK);
							gBattleCommunication[MULTISTRING_CHOOSER] = 18;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_BEADS_OF_RUIN:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							PrepareStatBuffer(gBattleTextBuff1, STAT_SPDEF);
							gBattleCommunication[MULTISTRING_CHOOSER] = 18;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_ZERO_TO_HERO:
					    if (gBattleStruct->zeroToHeroActivated[GetBattlerSide(battler)] & gBitTable[gBattlerPartyIndexes[battler]])
						{
							gBattleStruct->zeroToHeroActivated[GetBattlerSide(battler)] &= ~(gBitTable[gBattlerPartyIndexes[battler]]);
							gBattleCommunication[MULTISTRING_CHOOSER] = 9;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_SCHOOLING:
					    if (gBattleMons[battler].level >= 20)
						{
							u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_HP);
							
							if (newSpecies)
							{
								gBattleCommunication[MULTISTRING_CHOOSER] = 0;
								DoBattleFormChange(battler, newSpecies, FALSE, TRUE, FALSE);
								BattleScriptPushCursorAndCallback(BattleScript_SchoolingActivatesPause);
								++effect;
							}
						}
						break;
					case ABILITY_ZEN_MODE:
					    {
							u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_HP);
							
							if (newSpecies)
							{
								gBattleCommunication[MULTISTRING_CHOOSER] = 0;
								DoBattleFormChange(battler, newSpecies, TRUE, TRUE, FALSE);
								BattleScriptPushCursorAndCallback(BattleScript_ZenModeActivatesPause);
								++effect;
							}
					    }
						break;
					case ABILITY_SHIELDS_DOWN:
					   {
						   u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_HP);
						   
						   if (newSpecies)
						   {
							   if (newSpecies == SPECIES_MINIOR_METEOR)
								   gBattleCommunication[MULTISTRING_CHOOSER] = 5;
							   else
								   gBattleCommunication[MULTISTRING_CHOOSER] = 4;
							   DoBattleFormChange(battler, newSpecies, FALSE, TRUE, FALSE);
							   BattleScriptPushCursorAndCallback(BattleScript_ZenModeActivatesPause);
							   ++effect;
						   }
					   }
					   break;
					case ABILITY_ANTICIPATION:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							u8 j;
							u16 flags, move;
							
							for (i = 0; i < MAX_BATTLERS_COUNT; i++)
							{
								if (IsBattlerAlive(i) && GetBattlerSide(i) != GetBattlerSide(battler))
								{
									for (j = 0; j < MAX_MON_MOVES; j++)
									{
										move = gBattleMons[i].moves[j];
										
										if (gBattleMoves[move].effect == EFFECT_OHKO)
											++effect;
										else if (move && !IS_MOVE_STATUS(move))
										{
											if (gBattleMoves[move].effect == EFFECT_HIDDEN_POWER)
												moveType = GetHiddenPowerType(GetBattlerPartyIndexPtr(i));
											else
												moveType = gBattleMoves[move].type;
											
											if (CalcTypeEffectivenessMultiplier(move, moveType, i, battler, FALSE, &flags) == TYPE_MUL_SUPER_EFFECTIVE)
												++effect;
										}
									}
								}
							}
							if (effect)
							{
								gBattleCommunication[MULTISTRING_CHOOSER] = 10;
								BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
								gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							}
						}
						break;
					case ABILITY_DOWNLOAD:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							u8 statId, opposingBattler = BATTLE_OPPOSITE(battler);
							u32 def = 0, spDef = 0, var;
							
							for (i = 0; i < (MAX_BATTLERS_COUNT / 2); i++, opposingBattler = BATTLE_PARTNER(opposingBattler))
							{
								if (IsBattlerAlive(opposingBattler))
								{
									APPLY_STAT_MOD(var, &gBattleMons[opposingBattler], gBattleMons[opposingBattler].defense, STAT_DEF);
									def += var;
									
									APPLY_STAT_MOD(var, &gBattleMons[opposingBattler], gBattleMons[opposingBattler].spDefense, STAT_SPDEF);
									spDef += var;
								}
							}
							if (def < spDef)
								statId = STAT_ATK;
							else
								statId = STAT_SPATK;
							
							if (CompareStat(battler, statId, MAX_STAT_STAGES, CMP_LESS_THAN))
							{
								SET_STATCHANGER(statId, 1, FALSE);
								gBattlerAttacker = battler;
								PrepareStatBuffer(gBattleTextBuff1, statId);
								BattleScriptPushCursorAndCallback(BattleScript_AttackerAbilityStatRaiseEnd3);
								gSpecialStatuses[battler].switchInAbilityDone = TRUE;
								++effect;
							}
						}
						break;
					case ABILITY_FOREWARN:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							struct Forewarn
							{
								u8 battlerId;
								u8 power;
								u16 moveId;
							};
							u8 j, bestId, count;
							struct Forewarn *data = Alloc(sizeof(struct Forewarn) * MAX_BATTLERS_COUNT * MAX_MON_MOVES);
							
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							
							for (i = 0, count = 0; i < MAX_BATTLERS_COUNT; i++)
							{
								if (IsBattlerAlive(i) && GetBattlerSide(i) != GetBattlerSide(battler))
								{
									for (j = 0; j < MAX_MON_MOVES; j++)
									{
										if (gBattleMons[i].moves[j])
										{
											data[count].moveId = gBattleMons[i].moves[j];
											data[count].battlerId = i;
											
											switch (gBattleMoves[data[count].moveId].effect)
											{
												case EFFECT_OHKO:
												    data[count].power = 150;
													break;
												case EFFECT_COUNTER:
												case EFFECT_MIRROR_COAT:
												case EFFECT_METAL_BURST:
												    data[count].power = 120;
													break;
												default:
												    if (gBattleMoves[data[count].moveId].power == 1)
														data[count].power = 80;
													else
														data[count].power = gBattleMoves[data[count].moveId].power;
													break;
											}
											++count;
										}
									}
								}
							}
							for (i = 1, bestId = 0; i < count; i++)
							{
								if (data[i].power > data[bestId].power || (data[i].power == data[bestId].power && Random() & 1))
									bestId = i;
							}
							gBattlerTarget = data[bestId].battlerId;
							PrepareMoveBuffer(gBattleTextBuff1, data[bestId].moveId);
							Free(data);
							gBattleCommunication[MULTISTRING_CHOOSER] = 11;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_FRISK:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattlerAttacker = battler;
							BattleScriptPushCursorAndCallback(BattleScript_Frisk);
							++effect;
						}
						return effect; // don't record ability if don't activate
					case ABILITY_SCREEN_CLEANER:
					    if (!gSpecialStatuses[battler].switchInAbilityDone && TryRemoveScreens(battler, TRUE))
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleCommunication[MULTISTRING_CHOOSER] = 12;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_CURIOUS_MEDICINE:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gBattlerTarget = BATTLE_PARTNER(battler);
							
							if (IsBattlerAlive(gBattlerTarget) && TryResetBattlerStatChanges(gBattlerTarget))
							{
								gBattleCommunication[MULTISTRING_CHOOSER] = 13;
								BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
								gSpecialStatuses[battler].switchInAbilityDone = TRUE;
								++effect;
							}
						}
						break;
					case ABILITY_COSTAR:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gBattlerTarget = BATTLE_PARTNER(battler);
							
							if (IsBattlerAlive(gBattlerTarget))
							{
								if (CountBattlerStatIncreases(gBattlerTarget, FALSE)) // Does it copies accuracy and evasion or negative stat stages ?
								{
									CopyBattlerStatChanges(battler, gBattlerTarget);
									++effect;
								}
								
								if (CopyBattlerCritModifier(battler, gBattlerTarget)) // It can also copy Focus Energy status
									++effect;

								if (effect)
								{
									gBattlerAttacker = battler;
									gBattleCommunication[MULTISTRING_CHOOSER] = 14;
									BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
									gSpecialStatuses[battler].switchInAbilityDone = TRUE;
								}
							}
						}
						break;
					case ABILITY_SUPREME_OVERLORD:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattleStruct->supremeOverlordBoosts[battler] = min(5, gBattleStruct->faintCounter[GetBattlerSide(battler)]);
							gBattleCommunication[MULTISTRING_CHOOSER] = 17;
							BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
							++effect;
						}
						break;
					case ABILITY_LIMBER:
					    if (!gSpecialStatuses[battler].switchInAbilityDone && gBattleMons[battler].status1 & STATUS1_PARALYSIS)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattlerTarget = battler;
							ClearBattlerStatus(gBattlerTarget);
							BattleScriptPushCursorAndCallback(BattleScript_PastelVeilActivates);
							++effect;
						}
						break;
					case ABILITY_INSOMNIA:
					case ABILITY_VITAL_SPIRIT:
					    if (!gSpecialStatuses[battler].switchInAbilityDone && gBattleMons[battler].status1 & STATUS1_SLEEP)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattlerTarget = battler;
							ClearBattlerStatus(gBattlerTarget);
							BattleScriptPushCursorAndCallback(BattleScript_PastelVeilActivates);
							++effect;
						}
						break;
					case ABILITY_IMMUNITY:
					    if (!gSpecialStatuses[battler].switchInAbilityDone && gBattleMons[battler].status1 & STATUS1_PSN_ANY)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattlerTarget = battler;
							ClearBattlerStatus(gBattlerTarget);
							BattleScriptPushCursorAndCallback(BattleScript_PastelVeilActivates);
							++effect;
						}
						break;
					case ABILITY_MAGMA_ARMOR:
					    if (!gSpecialStatuses[battler].switchInAbilityDone && gBattleMons[battler].status1 & STATUS1_FREEZE)
						{
							gSpecialStatuses[battler].switchInAbilityDone = TRUE;
							gBattlerTarget = battler;
							ClearBattlerStatus(gBattlerTarget);
							BattleScriptPushCursorAndCallback(BattleScript_PastelVeilActivates);
							++effect;
						}
						break;
					case ABILITY_PASTEL_VEIL:
					    if (!gSpecialStatuses[battler].switchInAbilityDone)
						{
							gBattlerTarget = BATTLE_PARTNER(battler);
							
							if (IsBattlerAlive(gBattlerTarget) && gBattleMons[gBattlerTarget].status1 & STATUS1_PSN_ANY)
							{
								ClearBattlerStatus(gBattlerTarget);
								BattleScriptPushCursorAndCallback(BattleScript_PastelVeilActivates);
								gSpecialStatuses[battler].switchInAbilityDone = TRUE;
								++effect;
							}
						}
						break;
					case ABILITY_INTREPID_SWORD:
					    if (!(gBattleStruct->intrepidSwordActivated[GetBattlerSide(battler)] & gBitTable[gBattlerPartyIndexes[battler]])
							&& CompareStat(battler, STAT_ATK, MAX_STAT_STAGES, CMP_LESS_THAN))
						{
							gBattleStruct->intrepidSwordActivated[GetBattlerSide(battler)] |= gBitTable[gBattlerPartyIndexes[battler]];
							SET_STATCHANGER(STAT_ATK, 1, FALSE);
							gBattlerAttacker = battler;
							BattleScriptPushCursorAndCallback(BattleScript_BattlerAbilityStatRaiseOnSwitchIn);
							++effect;
						}
						break;
					case ABILITY_DAUNTLESS_SHIELD:
					    if (!(gBattleStruct->dauntlessShieldActivated[GetBattlerSide(battler)] & gBitTable[gBattlerPartyIndexes[battler]])
							&& CompareStat(battler, STAT_DEF, MAX_STAT_STAGES, CMP_LESS_THAN))
						{
							gBattleStruct->dauntlessShieldActivated[GetBattlerSide(battler)] |= gBitTable[gBattlerPartyIndexes[battler]];
							SET_STATCHANGER(STAT_DEF, 1, FALSE);
							gBattlerAttacker = battler;
							BattleScriptPushCursorAndCallback(BattleScript_BattlerAbilityStatRaiseOnSwitchIn);
							++effect;
						}
						break;
				}
				break;
			case ABILITYEFFECT_ENDTURN:
			    switch (gLastUsedAbility)
				{
					case ABILITY_SOLAR_POWER:
					    SOLAR_POWER_SUN_CHECK:
						if (IsBattlerWeatherAffected(battler, WEATHER_SUN_ANY))
						{
							BattleScriptPushCursorAndCallback(BattleScript_DrySkinSunActivates);
							gBattleMoveDamage = gBattleMons[battler].maxHP / 8;
							if (gBattleMoveDamage == 0)
								gBattleMoveDamage = 1;
							++effect;
						}
						break;
					case ABILITY_RAIN_DISH:
					    if (IsBattlerWeatherAffected(battler, WEATHER_RAIN_ANY) && !BATTLER_MAX_HP(battler))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 0;
							gBattleMoveDamage = gBattleMons[battler].maxHP / 16;
							if (gBattleMoveDamage == 0)
								gBattleMoveDamage = 1;
							gBattleMoveDamage *= -1;
							BattleScriptPushCursorAndCallback(BattleScript_RainDishActivates);
							++effect;
						}
						break;
					case ABILITY_ICE_BODY:
					    if (IsBattlerWeatherAffected(battler, WEATHER_HAIL_ANY) && !BATTLER_MAX_HP(battler))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 0;
							gBattleMoveDamage = gBattleMons[battler].maxHP / 16;
							if (gBattleMoveDamage == 0)
								gBattleMoveDamage = 1;
							gBattleMoveDamage *= -1;
							BattleScriptPushCursorAndCallback(BattleScript_RainDishActivates);
							++effect;
						}
						break;
					case ABILITY_POISON_HEAL:
					    if (gBattleMons[battler].status1 & STATUS1_PSN_ANY && !BATTLER_MAX_HP(battler))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 0;
							gBattleMoveDamage = gBattleMons[battler].maxHP / 8;
							if (gBattleMoveDamage == 0)
								gBattleMoveDamage = 1;
							gBattleMoveDamage *= -1;
							BattleScriptPushCursorAndCallback(BattleScript_RainDishActivates);
							++effect;
						}
						break;
					case ABILITY_DRY_SKIN:
					    if (IsBattlerWeatherAffected(battler, WEATHER_RAIN_ANY) && !BATTLER_MAX_HP(battler))
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 1;
							gBattleMoveDamage = gBattleMons[battler].maxHP / 8;
							if (gBattleMoveDamage == 0)
								gBattleMoveDamage = 1;
							gBattleMoveDamage *= -1;
							BattleScriptPushCursorAndCallback(BattleScript_RainDishActivates);
							++effect;
						}
						else
							goto SOLAR_POWER_SUN_CHECK;
						break;
					case ABILITY_SHED_SKIN:
					    if (gBattleMons[battler].status1 & STATUS1_ANY && (Random() % 3) == 0)
						{
							ClearBattlerStatus(battler);
							BattleScriptPushCursorAndCallback(BattleScript_ShedSkinActivates);
							++effect;
						}
						break;
					case ABILITY_HYDRATION:
					    if (gBattleMons[battler].status1 & STATUS1_ANY && IsBattlerWeatherAffected(battler, WEATHER_RAIN_ANY))
						{
							ClearBattlerStatus(battler);
							BattleScriptPushCursorAndCallback(BattleScript_ShedSkinActivates);
							++effect;
						}
						break;
					case ABILITY_HEALER:
					    if (IsBattlerAlive(BATTLE_PARTNER(battler)) && gBattleMons[BATTLE_PARTNER(battler)].status1 & STATUS1_ANY && (Random() % 3) == 0)
						{
							gEffectBattler = BATTLE_PARTNER(battler);
							ClearBattlerStatus(gEffectBattler);
							BattleScriptPushCursorAndCallback(BattleScript_HealerActivates);
							++effect;
						}
						break;
					case ABILITY_SPEED_BOOST:
					    if (CompareStat(battler, STAT_SPEED, MAX_STAT_STAGES, CMP_LESS_THAN) && gDisableStructs[battler].isFirstTurn != 2
						// in gen5 onwards Speed Boost fails to activate if the user failed to run from a wild battle
						&& gActionsByTurnOrder[gBattlerByTurnOrder[battler]] != B_ACTION_RUN) // tried to run. If the game reach in this part is because it failed to do it.
						{
							SET_STATCHANGER(STAT_SPEED, 1, FALSE);
							BattleScriptPushCursorAndCallback(BattleScript_SpeedBoostActivates);
							++effect;
						}
						break;
					case ABILITY_TRUANT:
					    if (!(gBattleMons[battler].status1 & STATUS1_SLEEP))
							gDisableStructs[battler].truantCounter ^= TRUE;
						else
							gDisableStructs[battler].truantCounter = 0; // being asleep resets the Truant counter
						break; // don't record ability
					case ABILITY_SLOW_START:
					    if (gDisableStructs[battler].slowStartTimer && --gDisableStructs[battler].slowStartTimer == 0)
						{
							BattleScriptPushCursorAndCallback(BattleScript_SlowStartEnd);
							++effect;
						}
						break;
					case ABILITY_HARVEST:
					    if ((IsBattlerWeatherAffected(battler, WEATHER_SUN_ANY) || (Random() % 2) == 0) && !gBattleMons[battler].item
						&& !gBattleStruct->changedItems[battler] && ItemId_GetPocket(*GetUsedHeldItemPtr(battler)) == POCKET_BERRY_POUCH)
						{
							gLastUsedItem = *GetUsedHeldItemPtr(battler);
							BattleScriptPushCursorAndCallback(BattleScript_HarvestActivates);
							++effect;
						}
						break;
					case ABILITY_MOODY:
					    if (gDisableStructs[battler].isFirstTurn != 2)
						{
							u32 validToRaise = 0, validToLower = 0;
							
							for (i = STAT_ATK; i < NUM_STATS; i++)
							{
								if (CompareStat(battler, i, MIN_STAT_STAGES, CMP_GREATER_THAN))
									validToLower |= gBitTable[i];
								
								if (CompareStat(battler, i, MAX_STAT_STAGES, CMP_LESS_THAN))
									validToRaise |= gBitTable[i];
							}
							
							if (validToRaise || validToLower)
							{
								gBattleCommunication[MULTIUSE_STATE] = gBattleScripting.statChanger = 0;
								
								if (validToLower)
								{
									do
									{
										i = (Random() % NUM_STATS) + STAT_ATK;
									} while (!(validToLower & gBitTable[i]));
									
									SET_STATCHANGER(i, 1, TRUE);
									gBattleCommunication[MULTIUSE_STATE] = gBattleScripting.statChanger; // save it for use later
									validToRaise &= ~(gBitTable[i]); // cannot raise the same stat
								}
								if (validToRaise)
								{
									do
									{
										i = (Random() % NUM_STATS) + STAT_ATK;
									} while (!(validToRaise & gBitTable[i]));
									
									SET_STATCHANGER(i, 2, FALSE);
								}
								BattleScriptPushCursorAndCallback(BattleScript_MoodyActivates);
								++effect;
							}
						}
						break;
					case ABILITY_SCHOOLING:
					    if (gBattleMons[battler].level >= 20)
						{
							u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_HP);
							
							if (newSpecies)
							{
								if (newSpecies == SPECIES_WISHIWASHI)
									gBattleCommunication[MULTISTRING_CHOOSER] = 1;
								else
									gBattleCommunication[MULTISTRING_CHOOSER] = 0;
								DoBattleFormChange(battler, newSpecies, FALSE, TRUE, FALSE);
								BattleScriptPushCursorAndCallback(BattleScript_SchoolingActivates);
								++effect;
							}
						}
						break;
					case ABILITY_ZEN_MODE:
					{
						u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_HP);
						
						if (newSpecies)
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 0;
							DoBattleFormChange(battler, newSpecies, TRUE, TRUE, FALSE);
							BattleScriptPushCursorAndCallback(BattleScript_ZenModeActivates);
							gBattleScripting.battler = battler;
							++effect;
						}
						break;
					}
					case ABILITY_SHIELDS_DOWN:
					{
						u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_HP);
						
						if (newSpecies)
						{
							if (newSpecies == SPECIES_MINIOR_METEOR)
								gBattleCommunication[MULTISTRING_CHOOSER] = 5;
							else
								gBattleCommunication[MULTISTRING_CHOOSER] = 4;
							DoBattleFormChange(battler, newSpecies, FALSE, TRUE, FALSE);
							BattleScriptPushCursorAndCallback(BattleScript_ZenModeActivates);
							gBattleScripting.battler = battler;
							++effect;
						}
						break;
					}
					case ABILITY_HUNGER_SWITCH:
					{
						u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_ENDTURN);
						
						if (newSpecies)
						{
							gBattleCommunication[MULTISTRING_CHOOSER] = 1;
							DoBattleFormChange(battler, newSpecies, FALSE, FALSE, FALSE);
							BattleScriptPushCursorAndCallback(BattleScript_ZenModeActivates);
							gBattleScripting.battler = battler;
							++effect;
						}
						break;
					}
					case ABILITY_PICKUP:
					{
						u8 pickupBattler = GetBattlerOnTopOfPickupStack(battler);
						
						if (pickupBattler != 0xFF && TryRecycleBattlerItem(battler, pickupBattler))
						{
							BattleScriptPushCursorAndCallback(BattleScript_Pickup);
							++effect;
						}
						break;
					}
					case ABILITY_BAD_DREAMS:
						BattleScriptPushCursorAndCallback(BattleScript_BadDreamsActivates);
						return ++effect; // don't record ability if don't activate
				}
				break;
			case ABILITYEFFECT_TRACE:
			    for (i = 0; i < gBattlersCount; i++)
				{
					if (GetBattlerAbility(i) == ABILITY_TRACE && gBattleResources->flags->flags[i] & RESOURCE_FLAG_TRACED)
					{
						u8 opposingSide = BATTLE_OPPOSITE(GetBattlerPosition(battler)) & BIT_SIDE;
						u8 target1 = GetBattlerAtPosition(opposingSide);
						u8 target2 = GetBattlerAtPosition(opposingSide + BIT_FLANK);
						
						if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
						{
							if (IsBattlerAlive(target1) && IsBattlerAlive(target2) && IsBattlerAbilityCopyableByTrace(target1)
								&& IsBattlerAbilityCopyableByTrace(target2))
							{
								gBattlerTarget = GetBattlerAtPosition(((Random() & 1) * 2) | opposingSide);
								++effect;
							}
							else if (IsBattlerAlive(target1) && IsBattlerAbilityCopyableByTrace(target1))
							{
								gBattlerTarget = target1;
								++effect;
							}
							else if (IsBattlerAlive(target2) && IsBattlerAbilityCopyableByTrace(target2))
							{
								gBattlerTarget = target2;
								++effect;
							}
						}
						else
						{
							if (IsBattlerAlive(target1) && IsBattlerAbilityCopyableByTrace(target1))
							{
								gBattlerTarget = target1;
								++effect;
							}
						}
						if (effect)
						{
							gBattlerAttacker = i;
							gBattleResources->flags->flags[i] &= ~(RESOURCE_FLAG_TRACED);
							RecordAbilityBattle(i, gBattleMons[gBattlerTarget].ability);
							BattleScriptPushCursorAndCallback(BattleScript_TraceActivates);
							break;
						}
					}
				}
				break;
			case ABILITYEFFECT_NEUTRALIZING_GAS:
			    // Prints message only. separate from ABILITYEFFECT_ON_SWITCHIN bc activates before entry hazards
			    for (i = 0; i < gBattlersCount; i++)
				{
					if (IsBattlerAlive(i) && GetBattlerAbility(i) == ABILITY_NEUTRALIZING_GAS && !(gBattleResources->flags->flags[i] & RESOURCE_FLAG_NEUTRALIZING_GAS))
					{
						gBattleResources->flags->flags[i] |= RESOURCE_FLAG_NEUTRALIZING_GAS;
						gBattleScripting.battler = battler = i;
						gLastUsedAbility = GetBattlerAbility(i);
						BattleScriptPushCursorAndCallback(BattleScript_NeutralizingGasActivates);
						++effect;
						break;
					}
				}
				break;
			case ABILITYEFFECT_UNNERVE:
			    // Prints message only. separate from ABILITYEFFECT_ON_SWITCHIN bc activates before entry hazards
				for (i = 0; i < gBattlersCount; i++)
				{
					if (IsBattlerAlive(i) && (GetBattlerAbility(i) == ABILITY_UNNERVE || GetBattlerAbility(i) == ABILITY_AS_ONE_ICE_RIDER
					|| GetBattlerAbility(i) == ABILITY_AS_ONE_SHADOW_RIDER) && !gSpecialStatuses[i].switchInAbilityDone)
					{
						gSpecialStatuses[battler].switchInAbilityDone = TRUE;
						if (GetBattlerAbility(i) == ABILITY_UNNERVE)
							gBattleCommunication[MULTISTRING_CHOOSER] = 2; // Unnerve message
						else
							gBattleCommunication[MULTISTRING_CHOOSER] = 15; // As One message
						gBattleScripting.battler = battler = i;
						PrepareMonTeamPrefixBuffer(gBattleTextBuff1, BATTLE_OPPOSITE(battler));
						gLastUsedAbility = GetBattlerAbility(i);
						BattleScriptPushCursorAndCallback(BattleScript_DisplaySwitchInMsg);
						++effect;
						break;
					}
				}
				break;
			case ABILITYEFFECT_MOVES_BLOCK:
			    moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove);
			
			    switch (gLastUsedAbility)
				{
					case ABILITY_SOUNDPROOF:
					    if (gBattleMoves[gCurrentMove].flags.soundMove && moveTarget != MOVE_TARGET_USER && moveTarget != MOVE_TARGET_ALL_BATTLERS)
							effect = 1;
						break;
					case ABILITY_BULLETPROOF:
					    if (gBattleMoves[gCurrentMove].flags.ballisticMove)
							effect = 1;
						break;
					case ABILITY_TELEPATHY:
					    if (IsBattlerAlive(BATTLE_PARTNER(battler)) && gBattlerAttacker == BATTLE_PARTNER(battler) && !IS_MOVE_STATUS(gCurrentMove))
							effect = 1;
						break;
					case ABILITY_QUEENLY_MAJESTY:
					case ABILITY_DAZZLING:
					case ABILITY_ARMOR_TAIL:
					    if (GetChosenMovePriority(gBattlerAttacker) > 0 && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(battler)
							&& moveTarget != MOVE_TARGET_OPPONENTS_FIELD && (moveTarget != MOVE_TARGET_ALL_BATTLERS || gCurrentMove == MOVE_PERISH_SONG
						    || gCurrentMove == MOVE_FLOWER_SHIELD || gCurrentMove == MOVE_ROTOTILLER))
							effect = 2;
						break;
				}
				if (effect)
				{
					if (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS)
						gHitMarker |= HITMARKER_NO_PPDEDUCT;
					
					if (effect == 1)
						gBattlescriptCurrInstr = BattleScript_SoundproofProtected;
					else
						gBattlescriptCurrInstr = BattleScript_DazzlingProtected;
				}
				break;
			case ABILITYEFFECT_ABSORBING:
			    if (gCurrentMove)
				{
					u8 statId, statAmount = 1;
					
					moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove);
					
					switch (gLastUsedAbility)
					{
						case ABILITY_VOLT_ABSORB:
						    if (moveType == TYPE_ELECTRIC)
								effect = 1;
							break;
						case ABILITY_WATER_ABSORB:
						case ABILITY_DRY_SKIN:
						    if (moveType == TYPE_WATER)
								effect = 1;
							break;
						case ABILITY_EARTH_EATER: // Don't absorb Spikes
						    if (moveType == TYPE_GROUND && moveTarget != MOVE_TARGET_OPPONENTS_FIELD)
								effect = 1;
							break;
						case ABILITY_MOTOR_DRIVE:
						    if (moveType == TYPE_ELECTRIC)
							{
								statId = STAT_SPEED;
								effect = 2;
							}
							break;
						case ABILITY_LIGHTNING_ROD:
						    if (moveType == TYPE_ELECTRIC)
							{
								statId = STAT_SPATK;
								effect = 2;
							}
							break;
						case ABILITY_SAP_SIPPER:
						    if (moveType == TYPE_GRASS)
							{
								statId = STAT_ATK;
								effect = 2;
							}
							break;
						case ABILITY_WIND_RIDER: // Don't affects moves that target the user or all battlers, like Tailwind (will be handled separated)
						    // Sandstorm is a wind move, but can't trigger Wind Rider as well. Why?...
						    if (gBattleMoves[gCurrentMove].flags.windMove && moveTarget != MOVE_TARGET_USER && moveTarget != MOVE_TARGET_ALL_BATTLERS)
							{
								statId = STAT_ATK;
								effect = 2;
							}
							break;
						case ABILITY_WELL_BAKED_BODY:
						    if (moveType == TYPE_FIRE)
							{
								statId = STAT_DEF;
								statAmount = 2;
								effect = 2;
							}
							break;
						case ABILITY_FLASH_FIRE:
						    if (moveType == TYPE_FIRE)
								effect = 3;
							break;
					}
					
					switch (effect)
					{
						case 1:
						    if (gProtectStructs[gBattlerAttacker].notFirstStrike)
								gBattlescriptCurrInstr = BattleScript_MoveHPDrain;
							else
								gBattlescriptCurrInstr = BattleScript_MoveHPDrain_PPLoss;
							
							if (BATTLER_MAX_HP(battler))
							{
								if (gProtectStructs[gBattlerAttacker].notFirstStrike)
									gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless;
								else
									gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless_PPLoss;
							}
							else
							{
								gBattleMoveDamage = gBattleMons[battler].maxHP / 4;
								if (gBattleMoveDamage == 0)
									gBattleMoveDamage = 1;
								gBattleMoveDamage *= -1;
							}
							break;
						case 2:
						    if (CompareStat(battler, statId, MAX_STAT_STAGES, CMP_LESS_THAN))
							{
								SET_STATCHANGER(statId, statAmount, FALSE);
								
								if (gProtectStructs[gBattlerAttacker].notFirstStrike)
									gBattlescriptCurrInstr = BattleScript_MoveStatRaise;
								else
									gBattlescriptCurrInstr = BattleScript_MoveStatRaise_PPLoss;
							}
							else
							{
								if ((gProtectStructs[gBattlerAttacker].notFirstStrike))
									gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless;
								else
									gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless_PPLoss;
							}
							break;
						case 3:
						    if (gProtectStructs[gBattlerAttacker].notFirstStrike)
								gBattlescriptCurrInstr = BattleScript_FlashFireBoost;
							else
								gBattlescriptCurrInstr = BattleScript_FlashFireBoost_PPLoss;
							
							if (!(gBattleResources->flags->flags[battler] & RESOURCE_FLAG_FLASH_FIRE))
							{
								gBattleResources->flags->flags[battler] |= RESOURCE_FLAG_FLASH_FIRE;
								PrepareTypeBuffer(gBattleTextBuff1, moveType);
								gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FLASH_FIRE_BOOST;
							}
							else
								gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FLASH_FIRE_DONT_AFFECT;
							break;
					}
				}
				break;
			case ABILITYEFFECT_MOVE_END_ATTACKER: // attacker abilities that activates after makes contact with the target
			    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(gBattlerTarget) && !gProtectStructs[battler].confusionSelfDmg
				&& !SubsBlockMove(battler, gBattlerTarget, gCurrentMove))
				{
					switch (gLastUsedAbility)
					{
						case ABILITY_STENCH: // Stench check is taken care of in King's Rock check
						    BattleScriptPushCursor(); // Backup the current script
							SetMoveEffect(MOVE_EFFECT_FLINCH, FALSE, FALSE);
							
						    if (Random() % 100 < 10 && !MoveHasFlinchChance(gCurrentMove) && DoMoveEffect(FALSE, FALSE, STATUS_CHANGE_FLAG_IGNORE_SUBSTITUTE)) // Substitute already checked
								++effect;

							BattleScriptPop(); // Restore current script
							break;
						case ABILITY_POISON_TOUCH:
						    BattleScriptPushCursor(); // Backup the current script
							SetMoveEffect(MOVE_EFFECT_POISON, FALSE, FALSE);
							
						    if (DoMoveEffect(FALSE, FALSE, STATUS_CHANGE_FLAG_IGNORE_SUBSTITUTE)) // Substitute already checked
							{
								PrepareAbilityBuffer(gBattleTextBuff1, gLastUsedAbility);
								BattleScriptPop(); // Restore current script
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_PoisonTouchActivation;
								++effect;
							}
							else
								BattleScriptPop(); // Restore current script
							break;
					}
				}
				break;
			case ABILITYEFFECT_MOVE_END_TARGET: // target abilities that activates after makes contact with it
			    if (!ReceiveSheerForceBoost(gBattlerAttacker, gCurrentMove) || gLastUsedAbility == ABILITY_ILLUSION) // all these abilities are negated by Sheer Force, except Illusion
				{
					switch (gLastUsedAbility)
					{
						case ABILITY_ROUGH_SKIN:
						case ABILITY_IRON_BARBS:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(gBattlerAttacker) && BATTLER_DAMAGED(battler)
							&& !gProtectStructs[gBattlerAttacker].confusionSelfDmg && IsMoveMakingContact(gBattlerAttacker, gCurrentMove)
							&& GetBattlerAbility(gBattlerAttacker) != ABILITY_MAGIC_GUARD)
							{
								gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 8;
								if (gBattleMoveDamage == 0)
									gBattleMoveDamage = 1;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_RoughSkinActivates;
								++effect;
							}
							break;
						case ABILITY_AFTERMATH:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsBattlerAlive(gBattlerAttacker)
							&& GetBattlerAbility(gBattlerAttacker) != ABILITY_MAGIC_GUARD && IsMoveMakingContact(gBattlerAttacker, gCurrentMove)
							&& !IsBattlerAlive(battler) && !ABILITY_ON_FIELD(ABILITY_DAMP))
							{
								gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 4;
								if (gBattleMoveDamage == 0)
									gBattleMoveDamage = 1;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_RoughSkinActivates;
								++effect;
							}
							break;
						case ABILITY_INNARDS_OUT:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && !IsBattlerAlive(battler) && IsBattlerAlive(gBattlerAttacker))
							{
								gBattleMoveDamage = gSpecialStatuses[battler].dmg;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_RoughSkinActivates;
								++effect;
							}
							break;
						case ABILITY_COLOR_CHANGE:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && gCurrentMove != MOVE_STRUGGLE && !IS_MOVE_STATUS(gCurrentMove)
							&& BATTLER_DAMAGED(battler) && !IS_BATTLER_OF_TYPE(battler, moveType) && IsBattlerAlive(battler)
							&& !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove) && IS_MULTIHIT_FINAL_STRIKE) // Only occours on final strike of multi-hit moves
							{
								SetBattlerType(battler, moveType);
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_ColorChangeActivates;
								++effect;
							}
							break;
						case ABILITY_EFFECT_SPORE:
						    if (IsBattlerAffectedBySpore(gBattlerAttacker))
							{
								i = Random() % 3;
								
								if (i == 0)
									goto POISON_POINT;
								if (i == 1)
									goto STATIC;
								// sleep
								if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(gBattlerAttacker) && BATTLER_DAMAGED(battler)
								&& !gProtectStructs[gBattlerAttacker].confusionSelfDmg && IsMoveMakingContact(gBattlerAttacker, gCurrentMove) && (Random() % 3) == 0
								&& !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove) && CanBePutToSleep(battler, gBattlerAttacker, STATUS_CHANGE_FLAG_CHECK_UPROAR) == STATUS_CHANGE_WORKED)
								{
									SetMoveEffect(MOVE_EFFECT_SLEEP, TRUE, FALSE);
									BattleScriptPushCursor();
									gBattlescriptCurrInstr = BattleScript_ApplySecondaryEffect;
									++effect;
								}
							}
							break;
						case ABILITY_POISON_POINT:
						    POISON_POINT:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(gBattlerAttacker) && BATTLER_DAMAGED(battler)
							&& !gProtectStructs[gBattlerAttacker].confusionSelfDmg && IsMoveMakingContact(gBattlerAttacker, gCurrentMove) && (Random() % 3) == 0
							&& !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove) && CanBePoisoned(battler, gBattlerAttacker, 0) == STATUS_CHANGE_WORKED)
							{
								SetMoveEffect(MOVE_EFFECT_POISON, TRUE, FALSE);
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_ApplySecondaryEffect;
								++effect;
							}
							break;
						case ABILITY_STATIC:
						    STATIC:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(gBattlerAttacker) && BATTLER_DAMAGED(battler)
							&& !gProtectStructs[gBattlerAttacker].confusionSelfDmg && IsMoveMakingContact(gBattlerAttacker, gCurrentMove) && (Random() % 3) == 0
							&& !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove) && CanBeParalyzed(battler, gBattlerAttacker, 0) == STATUS_CHANGE_WORKED)
							{
								SetMoveEffect(MOVE_EFFECT_PARALYSIS, TRUE, FALSE);
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_ApplySecondaryEffect;
								++effect;
							}
							break;
						case ABILITY_FLAME_BODY:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(gBattlerAttacker) && BATTLER_DAMAGED(battler)
							&& !gProtectStructs[gBattlerAttacker].confusionSelfDmg && IsMoveMakingContact(gBattlerAttacker, gCurrentMove) && (Random() % 3) == 0
							&& !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove) && CanBeBurned(battler, gBattlerAttacker, 0) == STATUS_CHANGE_WORKED)
							{
								SetMoveEffect(MOVE_EFFECT_BURN, TRUE, FALSE);
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_ApplySecondaryEffect;
								++effect;
							}
							break;
						case ABILITY_CUTE_CHARM:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(gBattlerAttacker) && BATTLER_DAMAGED(battler)
							&& !gProtectStructs[gBattlerAttacker].confusionSelfDmg && IsMoveMakingContact(gBattlerAttacker, gCurrentMove)
							&& IsBattlerAlive(gBattlerTarget) && (Random() % 3) == 0 && CanBeInfatuatedBy(gBattlerAttacker, battler) == STATUS_CHANGE_WORKED)
							{
								gBattleMons[gBattlerAttacker].status2 |= STATUS2_INFATUATION;
								gDisableStructs[gBattlerAttacker].infatuatedWith = battler;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_CuteCharmActivates;
								++effect;
							}
							break;
						case ABILITY_ANGER_POINT:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && gIsCriticalHit && IsBattlerAlive(gBattlerTarget)
								&& !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove) && CompareStat(battler, STAT_ATK, MAX_STAT_STAGES, CMP_LESS_THAN))
							{
								gBattleMons[battler].statStages[STAT_ATK] = MAX_STAT_STAGES;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_AngerPointActivation;
								++effect;
							}
							break;
						case ABILITY_CURSED_BODY:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsBattlerAlive(gBattlerAttacker) && (Random() % 3) == 0
							&& !gProtectStructs[gBattlerAttacker].confusionSelfDmg && !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove)
							&& CanDisableMove(gBattlerAttacker, gCurrMovePos, gCurrentMove))
							{
								gDisableStructs[gBattlerAttacker].disabledMove = gCurrentMove;
								gDisableStructs[gBattlerAttacker].disableTimer = 4;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_CursedBodyActivation;
								++effect;
							}
							break;
						case ABILITY_WEAK_ARMOR:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(battler) && IS_MOVE_PHYSICAL(gCurrentMove) && BATTLER_DAMAGED(battler)
							&& (CompareStat(battler, STAT_SPEED, MAX_STAT_STAGES, CMP_LESS_THAN) || CompareStat(battler, STAT_DEF, MIN_STAT_STAGES, CMP_GREATER_THAN)))
							{
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_WeakArmorActivation;
								++effect;
							}
							break;
						case ABILITY_ILLUSION:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && TryRemoveIllusion(battler))
								++effect;
							break;
						case ABILITY_MUMMY:
						case ABILITY_LINGERING_AROMA:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(gBattlerAttacker) && BATTLER_DAMAGED(battler) && IsMoveMakingContact(gBattlerAttacker, gCurrentMove)
								&& CanAbilityBeReplacedByMummy(gBattleMons[gBattlerAttacker].ability))
							{
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_MummyActivates;
								// for recording the correct ability
								RecordAbilityBattle(gBattlerAttacker, gBattleMons[battler].ability); // same as target ability
								RecordAbilityBattle(battler, gBattleMons[battler].ability);
								return ++effect;
							}
							break;
						case ABILITY_THERMAL_EXCHANGE:
						    if (moveType == TYPE_FIRE && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsBattlerAlive(battler)
							&& CompareStat(battler, STAT_ATK, MAX_STAT_STAGES, CMP_LESS_THAN) && !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove))
							{
								SET_STATCHANGER(STAT_ATK, 1, FALSE);
								gEffectBattler = battler;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_TargetAbilityStatRaiseRet;
								++effect;
							}
							break;
						case ABILITY_JUSTIFIED:
						    if (moveType == TYPE_DARK && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsBattlerAlive(battler)
							&& CompareStat(battler, STAT_ATK, MAX_STAT_STAGES, CMP_LESS_THAN) && !SubsBlockMove(gBattlerAttacker, battler, gCurrentMove))
							{
								SET_STATCHANGER(STAT_ATK, 1, FALSE);
								gEffectBattler = battler;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_TargetAbilityStatRaiseRet;
								++effect;
							}
							break;
						case ABILITY_RATTLED:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && CompareStat(battler, STAT_SPEED, MAX_STAT_STAGES, CMP_LESS_THAN)
							&& IsBattlerAlive(battler) && (moveType == TYPE_BUG || moveType == TYPE_DARK || moveType == TYPE_GHOST))
							{
								SET_STATCHANGER(STAT_SPEED, 1, FALSE);
								gEffectBattler = battler;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_TargetAbilityStatRaiseRet;
								++effect;
							}
							break;
						case ABILITY_STAMINA:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && CompareStat(battler, STAT_DEF, MAX_STAT_STAGES, CMP_LESS_THAN)
							&& IsBattlerAlive(battler))
							{
								SET_STATCHANGER(STAT_DEF, 1, FALSE);
								gEffectBattler = battler;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_TargetAbilityStatRaiseRet;
								++effect;
							}
							break;
						case ABILITY_WATER_COMPACTION:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsBattlerAlive(battler) && moveType == TYPE_WATER
							&& CompareStat(battler, STAT_DEF, MAX_STAT_STAGES, CMP_LESS_THAN))
							{
								SET_STATCHANGER(STAT_DEF, 2, FALSE);
								gEffectBattler = battler;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_TargetAbilityStatRaiseRet;
								++effect;
							}
							break;
						case ABILITY_BERSERK:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsBattlerAlive(battler) && gBattleStruct->hpBefore[battler] > gBattleMons[battler].maxHP / 2
							&& gBattleMons[battler].hp < gBattleMons[battler].maxHP / 2 && CompareStat(battler, STAT_SPATK, MAX_STAT_STAGES, CMP_LESS_THAN) && IS_MULTIHIT_FINAL_STRIKE)
							{
								SET_STATCHANGER(STAT_SPATK, 1, FALSE);
								gEffectBattler = battler;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_TargetAbilityStatRaiseRet;
								++effect;
							}
							break;
						case ABILITY_WIMP_OUT:
						case ABILITY_EMERGENCY_EXIT:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
							&& IsBattlerAlive(battler) && TryActivateEmergencyExit(battler))
								gBattleResources->flags->flags[battler] |= RESOURCE_FLAG_EMERGENCY_EXIT;
							break; // don't record ability
						case ABILITY_GOOEY:
						case ABILITY_TANGLING_HAIR:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsBattlerAlive(gBattlerAttacker) && IsMoveMakingContact(gBattlerAttacker, gCurrentMove)
							&& CompareStat(gBattlerAttacker, STAT_SPEED, MIN_STAT_STAGES, CMP_GREATER_THAN))
							{
								SetMoveEffect(MOVE_EFFECT_SPD_MINUS_1, TRUE, FALSE);
								PrepareAbilityBuffer(gBattleTextBuff1, gLastUsedAbility);
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_GooeyActivates;
								++effect;
							}
							break;
						case ABILITY_SAND_SPIT:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
							&& TryChangeBattleWeather(battler, ENUM_WEATHER_SANDSTORM))
							{
								gBattleScripting.battler = battler;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_SandSpitActivated;
								++effect;
							}
							break;
						case ABILITY_PERISH_BODY:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsMoveMakingContact(gBattlerAttacker, gCurrentMove)
							&& !gProtectStructs[gBattlerAttacker].confusionSelfDmg && !(gStatuses3[gBattlerAttacker] & STATUS3_PERISH_SONG)
							&& IsBattlerAlive(battler))
							{
								if (!(gStatuses3[battler] & STATUS3_PERISH_SONG))
								{
									gStatuses3[battler] |= STATUS3_PERISH_SONG;
									gDisableStructs[battler].perishSongTimer = 3;
								}
								gStatuses3[gBattlerAttacker] |= STATUS3_PERISH_SONG;
								gDisableStructs[gBattlerAttacker].perishSongTimer = 3;
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_PerishBodyActivates;
								++effect;
							}
							break;
						case ABILITY_WANDERING_SPIRIT:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && IsMoveMakingContact(gBattlerAttacker, gCurrentMove)
							&& IsBattlerAlive(gBattlerAttacker) && IsAbilityChangeableByWanderingSpirit(gBattleMons[gBattlerAttacker].ability))
							{
								// for recording the correct ability
								RecordAbilityBattle(gBattlerAttacker, gLastUsedAbility);
								RecordAbilityBattle(battler, gBattleMons[gBattlerAttacker].ability);
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_WanderingSpiritActivates;
								return ++effect;
							}
							break;
						case ABILITY_WIND_POWER:
						    if (!gBattleMoves[gCurrentMove].flags.windMove)
								break;
							// fallthrough
						case ABILITY_ELECTROMORPHOSIS:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && !gProtectStructs[gBattlerAttacker].confusionSelfDmg)
							{
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_WindPowerActivates;
								++effect;
							}
							break;
						case ABILITY_COTTON_DOWN:
						    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(battler) && !gProtectStructs[gBattlerAttacker].confusionSelfDmg)
							{
								BattleScriptPushCursor();
								gBattlescriptCurrInstr = BattleScript_CottonDownActivates;
								++effect;
							}
							break;
					}
				}
				break;
			case ABILITYEFFECT_IMMUNITY:
			    for (battler = 0; battler < gBattlersCount; battler++)
				{
					switch (GetBattlerAbility(battler))
					{
						case ABILITY_IMMUNITY:
						case ABILITY_PASTEL_VEIL:
						    if (gBattleMons[battler].status1 & STATUS1_PSN_ANY)
								effect = 1;
							break;
						case ABILITY_LIMBER:
						    if (gBattleMons[battler].status1 & STATUS1_PARALYSIS)
								effect = 1;
							break;
						case ABILITY_INSOMNIA:
						case ABILITY_VITAL_SPIRIT:
						    if (gBattleMons[battler].status1 & STATUS1_SLEEP)
								effect = 1;
							break;
						case ABILITY_WATER_VEIL:
						case ABILITY_WATER_BUBBLE:
						case ABILITY_THERMAL_EXCHANGE:
						    if (gBattleMons[battler].status1 & STATUS1_BURN)
								effect = 1;
							break;
						case ABILITY_MAGMA_ARMOR:
						    if (gBattleMons[battler].status1 & STATUS1_FREEZE)
								effect = 1;
							break;
						case ABILITY_OBLIVIOUS:
						    if (gBattleMons[battler].status2 & STATUS2_INFATUATION || gDisableStructs[battler].tauntTimer)
								effect = 2;
							break;
						case ABILITY_OWN_TEMPO:
						    if (gBattleMons[battler].status2 & STATUS2_CONFUSION)
								effect = 3;
							break;
					}
					if (effect)
					{
						gBattleScripting.battler = battler;
						gLastUsedAbility = GetBattlerAbility(battler);
						BattleScriptPushCursor();
						gBattlescriptCurrInstr = BattleScript_AbilityCuredStatus;
						
						switch (effect)
						{
							case 1:
							    ClearBattlerStatus(battler);
								break;
							case 2:
							    gBattleMons[battler].status2 &= ~(STATUS2_INFATUATION);
								gDisableStructs[battler].tauntTimer = 0;
								StringCopy(gBattleTextBuff1, gStatusConditionString_MentalState);
								break;
							case 3:
							    gBattleMons[battler].status2 &= ~(STATUS2_CONFUSION);
								StringCopy(gBattleTextBuff1, gStatusConditionString_ConfusionJpn);
								break;
						}
					}
				}
				break;
			case ABILITYEFFECT_ON_WEATHER: // abilities that activate when the weather changes
			    gBattleScripting.battler = battler;
			
			    switch (gLastUsedAbility)
				{
					case ABILITY_ICE_FACE:
					    if (gBattleStruct->allowedToChangeFormInWeather[GetBattlerSide(battler)] & gBitTable[gBattlerPartyIndexes[battler]])
						{
							u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_WEATHER);
							
							if (newSpecies)
							{
								gBattleStruct->allowedToChangeFormInWeather[GetBattlerSide(battler)] &= ~(gBitTable[gBattlerPartyIndexes[battler]]);
								DoBattleFormChange(battler, newSpecies, FALSE, TRUE, FALSE);
								BattleScriptPushCursorAndCallback(BattleScript_CastformChange);
								++effect;
							}
						}
						break;
				}
				// Not only related to abilities
				// Reverts Castform and Cherrim when ability changed or suppressed
				if (!effect)
				{
					bool8 reloadTypes = FALSE, reloadStats = FALSE;
					u16 newSpecies = TryDoBattleFormChange(battler, FORM_CHANGE_WEATHER);
					
					if (newSpecies)
					{
						switch (SpeciesToNationalPokedexNum(newSpecies))
						{
							case NATIONAL_DEX_CASTFORM:
							    reloadTypes = TRUE;
								break;
							case NATIONAL_DEX_CHERRIM:
							    reloadStats = TRUE;
								break;
						}
						DoBattleFormChange(battler, newSpecies, reloadTypes, reloadStats, FALSE);
						BattleScriptPushCursorAndCallback(BattleScript_CastformChange);
						++effect;
					}
				}
				gSpecialStatuses[battler].removedWeatherChangeAbility = FALSE;
				break;
			case ABILITYEFFECT_SYNCHRONIZE:
			    if (gLastUsedAbility == ABILITY_SYNCHRONIZE && gHitMarker & HITMARKER_SYNCHRONISE_EFFECT)
				{
					gHitMarker &= ~(HITMARKER_SYNCHRONISE_EFFECT);
					SetMoveEffect(gBattleStruct->synchronizeMoveEffect, FALSE, FALSE);
					gBattleScripting.battler = battler;
					BattleScriptPushCursor();
					gBattlescriptCurrInstr = BattleScript_SynchronizeActivates;
					++effect;
				}
				break;
			case ABILITYEFFECT_ON_TERRAIN: // abilities that activate when the terrain changes
			    gBattleScripting.battler = battler;
				break;
		}
		if (effect && caseId != ABILITYEFFECT_SWITCH_IN_WEATHER)
			RecordAbilityBattle(battler, gLastUsedAbility);
	}
	return effect;
}

void BattleScriptExecute(const u8 *BS_ptr)
{
    gBattlescriptCurrInstr = BS_ptr;
    gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size++] = gBattleMainFunc;
    gBattleMainFunc = RunBattleScriptCommands_PopCallbacksStack;
    gCurrentActionFuncId = 0;
}

void BattleScriptPushCursorAndCallback(const u8 *BS_ptr)
{
    BattleScriptPushCursor();
    gBattlescriptCurrInstr = BS_ptr;
    gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size++] = gBattleMainFunc;
    gBattleMainFunc = RunBattleScriptCommands;
}

enum
{
    ITEM_NO_EFFECT,
    ITEM_STATUS_CHANGE,
    ITEM_EFFECT_OTHER,
    ITEM_PP_CHANGE,
    ITEM_HP_CHANGE,
    ITEM_STATS_CHANGE,
};

static u8 ConfusionBerries(u8 battlerId, u8 flavor, bool8 moveTurn)
{
    u8 effect = 0;
    
    if (CheckPinchBerryActivate(battlerId, gLastUsedItem) && !moveTurn)
    {
        PrepareFlavorBuffer(gBattleTextBuff1, flavor);
		
		gBattleMoveDamage = gBattleMons[battlerId].maxHP / 3;
		if (gBattleMoveDamage == 0)
			gBattleMoveDamage = 1;
		
		if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
			gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
		gBattleMoveDamage *= -1;
		
		if (GetMonFlavorRelation(GetBattlerPartyIndexPtr(battlerId), flavor) < 0)
			BattleScriptExecute(BattleScript_BerryConfuseHealEnd2);
		else
			BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
		
		effect = ITEM_HP_CHANGE;
    }
    return effect;
}

static u8 StatRaiseBerries(u8 battlerId, u8 statId, bool8 moveTurn)
{
    u8 effect = 0;

    if (CheckPinchBerryActivate(battlerId, gLastUsedItem) && !moveTurn && CompareStat(battlerId, statId, MAX_STAT_STAGES, CMP_LESS_THAN))
    {
        PrepareStatBuffer(gBattleTextBuff1, statId);
		PrepareStringBuffer(gBattleTextBuff2, STRINGID_STATROSE);
		gEffectBattler = battlerId;
		SET_STATCHANGER(statId, 1, FALSE);
		gBattleScripting.animArg1 = 0xE + statId;
		gBattleScripting.animArg2 = 0;
		BattleScriptExecute(BattleScript_BerryStatRaiseEnd2);
		effect = ITEM_STATS_CHANGE;
    }
    return effect;
}

u8 ItemBattleEffects(u8 caseID, u8 battlerId, bool8 moveTurn)
{
    int i = 0;
    u8 effect = ITEM_NO_EFFECT;
    u8 changedPP = 0;
    u8 battlerHoldEffect = GetBattlerItemHoldEffect(battlerId, TRUE), defHoldEffect;
    u8 battlerHoldEffectParam, defHoldEffectParam;
    u16 defItem;

    gLastUsedItem = gBattleMons[battlerId].item;
	
    if (IsUnnerveOnOpposingField(battlerId) && ItemId_GetPocket(gLastUsedItem) == POCKET_BERRY_POUCH)
    {
	    battlerHoldEffect = 0;
	    battlerHoldEffectParam = 0;
    }
	else
		battlerHoldEffectParam = ItemId_GetHoldEffectParam(gLastUsedItem);
	
    switch (caseID)
    {
    case ITEMEFFECT_ON_SWITCH_IN:
        if (GetBattlerItemHoldEffect(battlerId, FALSE) == HOLD_EFFECT_DOUBLE_PRIZE)
            gBattleStruct->moneyMultiplier = 2;
        else if (battlerHoldEffect == HOLD_EFFECT_RESTORE_STATS)
	{
		for (i = 0; i < NUM_BATTLE_STATS; ++i)
		{
			if (gBattleMons[battlerId].statStages[i] < DEFAULT_STAT_STAGES)
			{
				gBattleMons[battlerId].statStages[i] = DEFAULT_STAT_STAGES;
				effect = ITEM_STATS_CHANGE;
			}
		}
	}
        if (effect)
            {
                gBattleScripting.battler = battlerId;
                gBattlerAttacker = battlerId;
                BattleScriptExecute(BattleScript_WhiteHerbEnd2);
            }
            break;
    case 1:
        if (IsBattlerAlive(battlerId))
        {
            switch (battlerHoldEffect)
            {
            case HOLD_EFFECT_RESTORE_HP:
                if (gBattleMons[battlerId].hp <= gBattleMons[battlerId].maxHP / 2 && !moveTurn)
                {
                    gBattleMoveDamage = battlerHoldEffectParam;
                    if (gBattleMons[battlerId].hp + battlerHoldEffectParam > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = 4;
                }
                break;
			case HOLD_EFFECT_RESTORE_HP_PERCENTAGE:
			    if (gBattleMons[battlerId].hp <= gBattleMons[battlerId].maxHP / 2 && !moveTurn)
				{
					gBattleMoveDamage = (gBattleMons[battlerId].maxHP * battlerHoldEffectParam) / 100;
					gBattleMoveDamage *= -1;
                    BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = 4;
				}
				break;
            case HOLD_EFFECT_RESTORE_PP:
                if (!moveTurn)
                {
                    struct Pokemon *mon = GetBattlerPartyIndexPtr(battlerId);
                    u8 ppBonuses;
                    u16 move;

                    for (i = 0; i < MAX_MON_MOVES; ++i)
                    {
                        move = GetMonData(mon, MON_DATA_MOVE1 + i);
                        changedPP = GetMonData(mon, MON_DATA_PP1 + i);
                        ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
                        if (move && changedPP == 0)
                            break;
                    }
                    if (i != MAX_MON_MOVES)
                    {
                        u8 maxPP = CalculatePPWithBonus(move, ppBonuses, i);
                        if (changedPP + battlerHoldEffectParam > maxPP)
                            changedPP = maxPP;
                        else
                            changedPP = changedPP + battlerHoldEffectParam;
                        PrepareMoveBuffer(gBattleTextBuff1, move);
                        BattleScriptExecute(BattleScript_BerryPPHealEnd2);
                        BtlController_EmitSetMonData(battlerId, BUFFER_A, i + REQUEST_PPMOVE1_BATTLE, 0, 1, &changedPP);
                        MarkBattlerForControllerExec(battlerId);
                        effect = ITEM_PP_CHANGE;
                    }
                }
                break;
            case HOLD_EFFECT_RESTORE_STATS:
                for (i = 0; i < NUM_BATTLE_STATS; ++i)
                {
                    if (gBattleMons[battlerId].statStages[i] < DEFAULT_STAT_STAGES)
                    {
                        gBattleMons[battlerId].statStages[i] = DEFAULT_STAT_STAGES;
                        effect = ITEM_STATS_CHANGE;
                    }
                }
                if (effect)
                {
                    gBattleScripting.battler = battlerId;
                    gBattlerAttacker = battlerId;
                    BattleScriptExecute(BattleScript_WhiteHerbEnd2);
                }
                break;
            case HOLD_EFFECT_LEFTOVERS:
                if (!BATTLER_MAX_HP(battlerId) && !moveTurn)
                {
                    gBattleMoveDamage = gBattleMons[battlerId].maxHP / battlerHoldEffectParam;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    BattleScriptExecute(BattleScript_ItemHealHP_End2);
                    effect = ITEM_HP_CHANGE;
                    RecordItemEffectBattle(battlerId, battlerHoldEffect);
                }
                break;
            case HOLD_EFFECT_CONFUSE_FLAVOR:
			    effect = ConfusionBerries(battlerId, battlerHoldEffectParam, moveTurn);
				break;
            case HOLD_EFFECT_STAT_UP:
	            effect = StatRaiseBerries(battlerId, battlerHoldEffectParam, moveTurn);
				break;
            case HOLD_EFFECT_CRITICAL_UP:
                if (CheckPinchBerryActivate(battlerId, gLastUsedItem) && !moveTurn && !(gBattleMons[battlerId].status2 & STATUS2_FOCUS_ENERGY))
                {
                    gBattleMons[battlerId].status2 |= STATUS2_FOCUS_ENERGY;
                    BattleScriptExecute(BattleScript_BerryFocusEnergyEnd2);
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_RANDOM_STAT_UP:
                if (!moveTurn && CheckPinchBerryActivate(battlerId, gLastUsedItem))
                {
                    for (i = 0; i < 5 && !CompareStat(battlerId, STAT_ATK + i, MAX_STAT_STAGES, CMP_LESS_THAN); ++i);
                    if (i != 5)
                    {
                        do
                            i = Random() % 5;
                        while (!CompareStat(battlerId, STAT_ATK + i, MAX_STAT_STAGES, CMP_LESS_THAN));
                        PrepareStatBuffer(gBattleTextBuff1, i + 1);
                        gBattleTextBuff2[0] = B_BUFF_PLACEHOLDER_BEGIN;
                        gBattleTextBuff2[1] = B_BUFF_STRING;
                        gBattleTextBuff2[2] = STRINGID_STATSHARPLY;
                        gBattleTextBuff2[3] = STRINGID_STATSHARPLY >> 8;
                        gBattleTextBuff2[4] = B_BUFF_STRING;
                        gBattleTextBuff2[5] = STRINGID_STATROSE;
                        gBattleTextBuff2[6] = STRINGID_STATROSE >> 8;
                        gBattleTextBuff2[7] = EOS;
                        gEffectBattler = battlerId;
                        SET_STATCHANGER(i + 1, 2, FALSE);
                        gBattleScripting.animArg1 = 0x21 + i + 6;
                        gBattleScripting.animArg2 = 0;
                        BattleScriptExecute(BattleScript_BerryStatRaiseEnd2);
                        effect = ITEM_STATS_CHANGE;
                    }
                }
                break;
            case HOLD_EFFECT_CURE_PAR:
                if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_PARALYSIS);
                    BattleScriptExecute(BattleScript_BerryCurePrlzEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_PSN:
                if (gBattleMons[battlerId].status1 & STATUS1_PSN_ANY)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER);
                    BattleScriptExecute(BattleScript_BerryCurePsnEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_BRN:
                if (gBattleMons[battlerId].status1 & STATUS1_BURN)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_BURN);
                    BattleScriptExecute(BattleScript_BerryCureBrnEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_FRZ:
                if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_FREEZE);
                    BattleScriptExecute(BattleScript_BerryCureFrzEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_SLP:
                if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_SLEEP);
                    gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                    BattleScriptExecute(BattleScript_BerryCureSlpEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_CONFUSION:
                if (gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                {
                    gBattleMons[battlerId].status2 &= ~(STATUS2_CONFUSION);
                    BattleScriptExecute(BattleScript_BerryCureConfusionEnd2);
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_CURE_STATUS:
                if (gBattleMons[battlerId].status1 & STATUS1_ANY || gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                {
                    i = 0;
                    if (gBattleMons[battlerId].status1 & STATUS1_PSN_ANY)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
                        ++i;
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                    {
                        gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                        StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
                        ++i;
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
                        ++i;
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_BURN)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
                        ++i;
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
                        ++i;
                    }
                    if (gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ConfusionJpn);
                        ++i;
                    }
                    if (!(i > 1))
                        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                    else
                        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                    gBattleMons[battlerId].status1 = 0;
                    gBattleMons[battlerId].status2 &= ~(STATUS2_CONFUSION);
                    BattleScriptExecute(BattleScript_BerryCureChosenStatusEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_MENTAL_HERB:
                if (gBattleMons[battlerId].status2 & STATUS2_INFATUATION)
                {
                    gBattleMons[battlerId].status2 &= ~(STATUS2_INFATUATION);
                    StringCopy(gBattleTextBuff1, gStatusConditionString_LoveJpn);
                    BattleScriptExecute(BattleScript_BerryCureChosenStatusEnd2);
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            }
            if (effect)
            {
                gBattleScripting.battler = battlerId;
                gBattlerAttacker = battlerId;
                switch (effect)
                {
                case ITEM_STATUS_CHANGE:
                    BtlController_EmitSetMonData(battlerId, BUFFER_A, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[battlerId].status1);
                    MarkBattlerForControllerExec(battlerId);
                    break;
                case ITEM_PP_CHANGE:
                    if (!(gBattleMons[battlerId].status2 & STATUS2_TRANSFORMED) && !(gDisableStructs[battlerId].mimickedMoves & gBitTable[i]))
                        gBattleMons[battlerId].pp[i] = changedPP;
                    break;
                }
            }
        }
        break;
    case 2:
        break;
    case ITEMEFFECT_MOVE_END:
        for (battlerId = 0; battlerId < gBattlersCount; ++battlerId)
        {
            gLastUsedItem = gBattleMons[battlerId].item;
			battlerHoldEffect = GetBattlerItemHoldEffect(battlerId, TRUE);
            battlerHoldEffectParam = ItemId_GetHoldEffectParam(gLastUsedItem);
			
            switch (battlerHoldEffect)
            {
            case HOLD_EFFECT_CURE_PAR:
                if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_PARALYSIS);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureParRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_PSN:
                if (gBattleMons[battlerId].status1 & STATUS1_PSN_ANY)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCurePsnRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_BRN:
                if (gBattleMons[battlerId].status1 & STATUS1_BURN)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_BURN);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureBrnRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_FRZ:
                if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_FREEZE);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureFrzRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_SLP:
                if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_SLEEP);
                    gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureSlpRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_CONFUSION:
                if (gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                {
                    gBattleMons[battlerId].status2 &= ~(STATUS2_CONFUSION);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureConfusionRet;
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_MENTAL_HERB:
                if (gBattleMons[battlerId].status2 & STATUS2_INFATUATION)
                {
                    gBattleMons[battlerId].status2 &= ~(STATUS2_INFATUATION);
                    StringCopy(gBattleTextBuff1, gStatusConditionString_LoveJpn);
                    BattleScriptPushCursor();
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                    gBattlescriptCurrInstr = BattleScript_BerryCureChosenStatusRet;
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_CURE_STATUS:
                if (gBattleMons[battlerId].status1 & STATUS1_ANY || gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                {
                    if (gBattleMons[battlerId].status1 & STATUS1_PSN_ANY)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                    {
                        gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                        StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_BURN)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
                    }
                    if (gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ConfusionJpn);
                    }
                    gBattleMons[battlerId].status1 = 0;
                    gBattleMons[battlerId].status2 &= ~(STATUS2_CONFUSION);
                    BattleScriptPushCursor();
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                    gBattlescriptCurrInstr = BattleScript_BerryCureChosenStatusRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_RESTORE_STATS:
                for (i = 0; i < NUM_BATTLE_STATS; ++i)
                {
                    if (gBattleMons[battlerId].statStages[i] < DEFAULT_STAT_STAGES)
                    {
                        gBattleMons[battlerId].statStages[i] = DEFAULT_STAT_STAGES;
                        effect = ITEM_STATS_CHANGE;
                    }
                }
                if (effect)
                {
                    gBattleScripting.battler = battlerId;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_WhiteHerbRet;
                    return effect;
                }
                break;
            }
            if (effect)
            {
                gBattleScripting.battler = battlerId;
                BtlController_EmitSetMonData(battlerId, BUFFER_A, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[battlerId].status1);
                MarkBattlerForControllerExec(battlerId);
                break;
            }
        }
        break;
    case ITEMEFFECT_KINGSROCK_SHELLBELL:
        if (gBattleMoveDamage && !ReceiveSheerForceBoost(battlerId, gCurrentMove))
        {
            switch (battlerHoldEffect)
            {
            case HOLD_EFFECT_FLINCH:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && BATTLER_DAMAGED(gBattlerTarget)
                 && gBattleMoves[gCurrentMove].flags.kingsRockAffected
                 && IsBattlerAlive(gBattlerTarget)
				 && GetBattlerAbility(battlerId) != ABILITY_STENCH
				 && (Random() % 100) < GetEffectChanceIncreases(battlerId, battlerHoldEffectParam))
                {
					BattleScriptPushCursor();
					SetMoveEffect(MOVE_EFFECT_FLINCH, FALSE, FALSE);
					DoMoveEffect(FALSE, FALSE, 0);
					BattleScriptPop();
                }
                break;
            case HOLD_EFFECT_SHELL_BELL:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && gSpecialStatuses[gBattlerTarget].dmg != 0
                 && gSpecialStatuses[gBattlerTarget].dmg != 0xFFFF
                 && battlerId != gBattlerTarget
                 && !BATTLER_MAX_HP(battlerId)
                 && IsBattlerAlive(battlerId))
                {
                    gBattleScripting.battler = battlerId;
                    gBattleMoveDamage = (gSpecialStatuses[gBattlerTarget].dmg / battlerHoldEffectParam) * -1;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = -1;
                    gSpecialStatuses[gBattlerTarget].dmg = 0;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_ItemHealHP_Ret;
                    ++effect;
                }
                break;
            }
        }
        break;
    }
    return effect;
}

bool8 CheckPinchBerryActivate(u8 battler, u16 item)
{
    if (IsUnnerveOnOpposingField(battler) && ItemId_GetPocket(item) == POCKET_BERRY_POUCH)
		return FALSE;
    if (gBattleMons[battler].hp <= gBattleMons[battler].maxHP / 4 || (GetBattlerAbility(battler) == ABILITY_GLUTTONY && gBattleMons[battler].hp <= gBattleMons[battler].maxHP / 2))
        return TRUE;
    return FALSE;
}

void ClearFuryCutterDestinyBondGrudge(u8 battlerId)
{
    gDisableStructs[battlerId].furyCutterCounter = 0;
    gBattleMons[battlerId].status2 &= ~(STATUS2_DESTINY_BOND);
    gStatuses3[battlerId] &= ~(STATUS3_GRUDGE);
}

void HandleAction_RunBattleScript(void) // identical to RunBattleScriptCommands
{
    if (!gBattleControllerExecFlags)
        gBattleScriptingCommandsTable[*gBattlescriptCurrInstr]();
}

u8 GetMoveSplit(u16 move)
{
	u8 split = gBattleMoves[move].split;
	
	// add moves that can change its split.
	
	return split;
}

u8 GetDefaultMoveTarget(u8 battlerId)
{
    u8 opposing = BATTLE_OPPOSITE(GetBattlerPosition(battlerId) & BIT_SIDE);

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        return GetBattlerAtPosition(opposing);
    else if (CountAliveMonsInBattle(battlerId, BATTLE_ALIVE_EXCEPT_BATTLER) > 1)
        return GetBattlerAtPosition((Random() & 1) == 0 ? BATTLE_PARTNER(opposing) : opposing);
    else
		return GetBattlerAtPosition((gAbsentBattlerFlags & gBitTable[opposing]) ? BATTLE_PARTNER(opposing) : opposing);
}

u8 GetRandomTarget(u8 battlerId)
{
	return GetBattlerAtPosition(sTargetPositions[GetBattlerSide(battlerId)][Random() & 1]);
}

// Get move base target type
u8 GetBattlerMoveTargetType(u8 battlerId, u16 move)
{
	u8 target = gBattleMoves[move].target;
	
	// Add moves that can change target's type in the future, like Expanding Force.
	
	return target;
}

// Get move target and apply all redirection effects
u8 GetMoveTarget(u16 move, u8 setTarget)
{
    u8 side, moveTarget, targetBattler = 0, battlerOpposite = BATTLE_OPPOSITE(gBattlerAttacker);
    
    if (setTarget)
        moveTarget = setTarget - 1;
    else
        moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, move);
	
    switch (moveTarget)
    {
		case MOVE_TARGET_SELECTED:
            side = GetBattlerSide(battlerOpposite);
			
			if (IsBattlerAffectedByFollowMe(gBattlerAttacker, side, move))
				targetBattler = gSideTimers[side].followmeTarget;
			else
			{
				do
				{
					targetBattler = Random() % gBattlersCount;
				} while (GetBattlerSide(gBattlerAttacker) == GetBattlerSide(targetBattler) || !IsBattlerAlive(targetBattler));
				
				if (gBattleMoves[move].type == TYPE_ELECTRIC && ABILITY_ON_OPPOSING_SIDE(gBattlerAttacker, ABILITY_LIGHTNING_ROD)
				&& GetBattlerAbility(targetBattler) != ABILITY_LIGHTNING_ROD)
				{
					targetBattler = BATTLE_PARTNER(targetBattler);
					RecordAbilityBattle(targetBattler, gBattleMons[targetBattler].ability);
					gSpecialStatuses[targetBattler].lightningRodRedirected = 1;
				}
				else if (gBattleMoves[move].type == TYPE_WATER && ABILITY_ON_OPPOSING_SIDE(gBattlerAttacker, ABILITY_STORM_DRAIN)
				&& GetBattlerAbility(targetBattler) != ABILITY_STORM_DRAIN)
				{
					targetBattler = BATTLE_PARTNER(targetBattler);
					RecordAbilityBattle(targetBattler, gBattleMons[targetBattler].ability);
					gSpecialStatuses[targetBattler].stormDrainRedirected = 1;
				}
			}
			break;
		case MOVE_TARGET_DEPENDS:
		case MOVE_TARGET_BOTH:
		case MOVE_TARGET_FOES_AND_ALLY:
		case MOVE_TARGET_OPPONENTS_FIELD:
			targetBattler = battlerOpposite;
			
			if (!IsBattlerAlive(targetBattler))
				targetBattler = BATTLE_PARTNER(targetBattler);
			break;
		case MOVE_TARGET_RANDOM:
			side = GetBattlerSide(battlerOpposite);
			
			if (IsBattlerAffectedByFollowMe(gBattlerAttacker, side, move))
				targetBattler = gSideTimers[side].followmeTarget;
			else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
			{
				targetBattler = GetRandomTarget(gBattlerAttacker);
				
				if (!IsBattlerAlive(targetBattler))
					targetBattler = BATTLE_PARTNER(targetBattler);
			}
			else
				targetBattler = battlerOpposite;
			break;
		case MOVE_TARGET_ALLY:
		    targetBattler = BATTLE_PARTNER(gBattlerAttacker);
			
			if (IsBattlerAlive(targetBattler))
				break;
			// Fallthrough
		default:
			targetBattler = gBattlerAttacker;
			break;
    }
    gBattleStruct->moveTarget[gBattlerAttacker] = targetBattler;
	
    return targetBattler;
}

// This don't account for moves that can change target
void CopyMoveTargetName(u8 *dest, u16 move)
{
	const u8 *str;
	
	switch (gBattleMoves[move].target)
	{
		case MOVE_TARGET_SELECTED:
		    str = gText_MoveTargetSelected;
			break;
		case MOVE_TARGET_DEPENDS:
		    str = gText_MoveTargetDepends;
			break;
		case MOVE_TARGET_USER_OR_SELECTED:
		    str = gText_MoveTargetUserOrSelected;
			break;
		case MOVE_TARGET_RANDOM:
		    str = gText_MoveTargetRandom;
			break;
		case MOVE_TARGET_OPPONENTS_FIELD:
		    str = gText_MoveTargetOpponentsField;
			break;
		case MOVE_TARGET_BOTH:
		    str = gText_MoveTargetBoth;
			break;
		case MOVE_TARGET_USER:
		    str = gText_MoveTargetUser;
			break;
		case MOVE_TARGET_ALLY:
		    str = gText_MoveTargetAlly;
			break;
		case MOVE_TARGET_FOES_AND_ALLY:
		    str = gText_MoveTargetFoesAndAlly;
			break;
		case MOVE_TARGET_USER_OR_ALLY:
			str = gText_MoveTargetUserOrAlly;
			break;
		case MOVE_TARGET_ALL_BATTLERS:
		    str = gText_MoveTargetAllBattlers;
			break;
	}
	StringCopyN(dest, str, 12);
}

u8 IsMonDisobedient(void)
{
    s32 calc;
    u8 levelCapLevel = GetCurrentLevelCapLevel();

    if (levelCapLevel == MAX_LEVEL || (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_POKEDUDE)) || GetBattlerSide(gBattlerAttacker) == B_SIDE_OPPONENT
	|| !IsOtherTrainer(gBattleMons[gBattlerAttacker].otId, gBattleMons[gBattlerAttacker].otName) || gBattleMons[gBattlerAttacker].level <= levelCapLevel)
        return 0;

    calc = (gBattleMons[gBattlerAttacker].level + levelCapLevel) * (Random() & 255) >> 8;
    if (calc < levelCapLevel)
        return 0;
	
    // is not obedient
    if (gBattleMoves[gCurrentMove].effect == EFFECT_RAGE)
        gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_RAGE);
	
    if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP) && (gBattleMoves[gCurrentMove].effect == EFFECT_SNORE
	|| gBattleMoves[gCurrentMove].effect == EFFECT_SLEEP_TALK))
    {
        gBattlescriptCurrInstr = BattleScript_IgnoresWhileAsleep;
        return 1;
    }
    calc = (gBattleMons[gBattlerAttacker].level + levelCapLevel) * (Random() & 255) >> 8;
	
    if (calc < levelCapLevel && gBattleMoves[gCurrentMove].effect != EFFECT_FOCUS_PUNCH) // Additional check for focus punch in FR
    {
        calc = gBitTable[gCurrMovePos] | CheckMoveLimitations(gBattlerAttacker, 0);
		
        if (calc == MOVE_LIMITATION_ALL_MOVES_MASK) // all moves cannot be used
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = Random() % ARRAY_COUNT(gInobedientStringIds); // Choose a random string to print
            gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
            return 1;
        }
        else // use a random move
        {
            do
                gCurrMovePos = gChosenMovePos = Random() % MAX_MON_MOVES;
            while (gBitTable[gCurrMovePos] & calc);
			
            gCalledMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
            gBattlescriptCurrInstr = BattleScript_IgnoresAndUsesRandomMove;
            return 2;
        }
    }
    else
    {
        levelCapLevel = gBattleMons[gBattlerAttacker].level - levelCapLevel;
        calc = (Random() & 255);
		
        if (calc < levelCapLevel && CanBePutToSleep(gBattlerAttacker, gBattlerAttacker, STATUS_CHANGE_FLAG_IGNORE_SAFEGUARD) == STATUS_CHANGE_WORKED
		&& IsUproarActive() == gBattlersCount)
        {
			gBattlescriptCurrInstr = BattleScript_IgnoresAndFallsAsleep;
			return 1;
        }
        calc -= levelCapLevel;
		
        if (calc < levelCapLevel)
        {
            gBattleMoveDamage = CalculateConfusionDamage();
            gBattlescriptCurrInstr = BattleScript_IgnoresAndHitsItself;
            gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
            return 2;
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = Random() % ARRAY_COUNT(gInobedientStringIds); // Choose a random string to print
            gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
            return 1;
        }
    }
}

u8 GetHiddenPowerType(struct Pokemon *mon)
{
	u8 type;
	s32 typeBits = ((GetMonData(mon, MON_DATA_HP_IV) & 1) << 0)
              | ((GetMonData(mon, MON_DATA_ATK_IV) & 1) << 1)
              | ((GetMonData(mon, MON_DATA_DEF_IV) & 1) << 2)
              | ((GetMonData(mon, MON_DATA_SPEED_IV) & 1) << 3)
              | ((GetMonData(mon, MON_DATA_SPATK_IV) & 1) << 4)
              | ((GetMonData(mon, MON_DATA_SPDEF_IV) & 1) << 5);
	
	type = (15 * typeBits) / 63 + 1;
	if (type >= TYPE_MYSTERY)
		++type;
	return type;
}

u8 GetPartyMonIdForIllusion(u8 battler, struct Pokemon *party, u8 partyCount, struct Pokemon *illusionMon)
{
	struct Pokemon *partnerMon;
	s8 i, id;
	
	if (GetMonAbility(illusionMon) == ABILITY_ILLUSION)
	{
		if (&party[partyCount - 1] != illusionMon)
		{
			if (IsBattlerAlive(BATTLE_PARTNER(battler)))
				partnerMon = &party[gBattlerPartyIndexes[BATTLE_PARTNER(battler)]];
			else
				partnerMon = illusionMon;
			
			// find last alive non-egg pokemon
			for (i = PARTY_SIZE - 1; i >= 0; i--)
			{
				id = i;
				
				if (MonCanBattle(&party[id]) && &party[id] != illusionMon && &party[id] != partnerMon)
				    return id;
			}
		}
	}
	return PARTY_SIZE;
}

static void SetIllusionMon(u8 battler)
{
	struct Pokemon *party = GetBattlerParty(battler);
	u8 id;
	
	gBattleStruct->illusion[battler].set = TRUE;

	id = GetPartyMonIdForIllusion(battler, party, GetBattlerSide(battler) == B_SIDE_PLAYER ? gPlayerPartyCount : gEnemyPartyCount, &party[gBattlerPartyIndexes[battler]]);
	
	if (id != PARTY_SIZE)
	{
		gBattleStruct->illusion[battler].on = TRUE;
		gBattleStruct->illusion[battler].broken = FALSE;
		gBattleStruct->illusion[battler].partyId = id;
		gBattleStruct->illusion[battler].mon = &party[id];
	}
}

static struct Pokemon *GetIllusionMonPtr(u8 battler)
{
	if (gBattleStruct->illusion[battler].broken)
		return NULL;
	
	if (!gBattleStruct->illusion[battler].set)
		SetIllusionMon(battler);
	
	if (!gBattleStruct->illusion[battler].on)
		return NULL;
	
	return gBattleStruct->illusion[battler].mon;
}

void ClearIllusionMon(u8 battler)
{
	memset(&gBattleStruct->illusion[battler], 0, sizeof(gBattleStruct->illusion[battler]));
}

bool8 TryRemoveIllusion(u8 battler)
{
	if (GetIllusionMonPtr(battler) != NULL)
	{
		gBattleScripting.battler = battler;
		BattleScriptPushCursor();
		gBattlescriptCurrInstr = BattleScript_IllusionOff;
		return TRUE;
	}
	return FALSE;
}

bool8 ReceiveSheerForceBoost(u8 battler, u16 move)
{
	return (GetBattlerAbility(battler) == ABILITY_SHEER_FORCE && gBattleMoves[move].secondaryEffectChance);
}

bool8 IsUnnerveOnOpposingField(u8 battler)
{
	if (ABILITY_ON_OPPOSING_SIDE(battler, ABILITY_UNNERVE) || ABILITY_ON_OPPOSING_SIDE(battler, ABILITY_AS_ONE_ICE_RIDER) || ABILITY_ON_OPPOSING_SIDE(battler, ABILITY_AS_ONE_SHADOW_RIDER))
		return TRUE;
	return FALSE;
}

u16 *GetUsedHeldItemPtr(u8 battler)
{
	return &gBattleStruct->usedHeldItems[gBattlerPartyIndexes[battler]][GetBattlerSide(battler)];
}

bool8 NoAliveMonsForParty(struct Pokemon *party)
{
	u8 i;
	u32 HP_count = 0;
	
	for (i = 0; i < PARTY_SIZE; i++)
	{
		if (IsMonValidSpecies(&party[i]))
			HP_count += GetMonData(&party[i], MON_DATA_HP);
	}
	return (HP_count == 0);
}

bool8 NoAliveMonsForEitherParty(void)
{
	return (NoAliveMonsForParty(gPlayerParty) || NoAliveMonsForParty(gEnemyParty));
}

bool8 IsBattlerAlive(u8 battlerId)
{
	if (battlerId >= gBattlersCount || gBattleMons[battlerId].hp == 0 || gAbsentBattlerFlags & gBitTable[battlerId])
		return FALSE;
	return TRUE;
}

bool8 IsBattlerWeatherAffected(u8 battlerId, u16 weatherFlags)
{
	if (WEATHER_HAS_EFFECT && gBattleWeather & weatherFlags)
	{
		return TRUE;
	}
	return FALSE;
}

static void ShouldChangeFormOnWeatherStart(u8 battlerId)
{
	u8 i, side = GetBattlerSide(battlerId);
	struct Pokemon *party = GetBattlerParty(battlerId);
	
	for (i = 0; i < PARTY_SIZE; i++)
	{
		if (GetMonData(&party[i], MON_DATA_SPECIES) == SPECIES_EISCUE_NOICE_FACE)
			gBattleStruct->allowedToChangeFormInWeather[side] |= gBitTable[i];
		else
			gBattleStruct->allowedToChangeFormInWeather[side] &= ~(gBitTable[i]);
	}
}

bool8 TryChangeBattleWeather(u8 battlerId, u8 weatherEnumId)
{
	u16 ability = GetBattlerAbility(battlerId);
	
	if (gBattleWeather & WEATHER_PRIMAL_ANY && ability != ABILITY_PRIMORDIAL_SEA && ability != ABILITY_DESOLATE_LAND && ability != ABILITY_DELTA_STREAM)
		return FALSE;
	else if (!(gBattleWeather & (sWeatherFlagsInfo[weatherEnumId][0] | sWeatherFlagsInfo[weatherEnumId][1])))
	{
		gBattleWeather = sWeatherFlagsInfo[weatherEnumId][0];
		gWishFutureKnock.weatherDuration = 5;
		ShouldChangeFormOnWeatherStart(battlerId);
		return TRUE;
	}
	return FALSE;
}

bool8 IsMoveMakingContact(u8 battler, u16 move)
{
	if (GetBattlerAbility(battler) != ABILITY_LONG_REACH && gBattleMoves[move].flags.makesContact)
		return TRUE;
	return FALSE;
}

// Check if defender is protected from attacker's move
bool8 IsBattlerProtected(u8 attacker, u8 defender, u16 move)
{
	if (GetBattlerAbility(attacker) == ABILITY_UNSEEN_FIST && IsMoveMakingContact(attacker, move))
		return FALSE;
	else if (gProtectStructs[defender].protected && !gBattleMoves[move].flags.forbiddenProtect)
		return TRUE;
	return FALSE;
}

u8 CheckAbilityInBattle(u8 mode, u8 battlerId, u16 abilityId)
{
	u8 i, ret = 0;
	
	switch (mode)
	{
		case CHECK_ABILITY_ON_FIELD:
		case CHECK_ABILITY_ON_FIELD_EXCEPT_BATTLER:
		    for (i = 0; i < gBattlersCount; i++)
			{
				if (mode == CHECK_ABILITY_ON_FIELD_EXCEPT_BATTLER && i == battlerId) // Skip the exception battler
					continue;
				if (IsBattlerAlive(i) && GetBattlerAbility(i) == abilityId)
				{
					gLastUsedAbility = abilityId;
					ret = i + 1;
				}
			}
			break;
		case CHECK_ABILITY_ON_SIDE:
		    for (i = 0; i < gBattlersCount; i++)
			{
				if (IsBattlerAlive(i) && GetBattlerSide(i) == GetBattlerSide(battlerId) && GetBattlerAbility(i) == abilityId)
				{
					gLastUsedAbility = abilityId;
					ret = i + 1;
				}
			}
			break;
	}
	return ret;
}

// Don't call Neutralizing Gas and Unnerve here bc at the point this function is called the two abilities already has ben called before
bool8 DoSwitchInAbilitiesItems(u8 battlerId)
{
	if (AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, battlerId) || AbilityBattleEffects(ABILITYEFFECT_ON_WEATHER, battlerId)
		|| AbilityBattleEffects(ABILITYEFFECT_ON_TERRAIN, battlerId) || AbilityBattleEffects(ABILITYEFFECT_TRACE, 0)
	    || ItemBattleEffects(ITEMEFFECT_ON_SWITCH_IN, battlerId, FALSE))
		return TRUE;
	return FALSE;
}

u8 CountBattlerStatIncreases(u8 battlerId, bool8 countEvasionAccuracy)
{
	u8 i, count;
	
	for (i = 0, count = 0; i < NUM_BATTLE_STATS; i++)
	{
		if (!countEvasionAccuracy && (i == STAT_ACC || i == STAT_EVASION))
			continue;
		if (gBattleMons[battlerId].statStages[i] > DEFAULT_STAT_STAGES)
			count += gBattleMons[battlerId].statStages[i] - DEFAULT_STAT_STAGES;
	}
	return count;
}

bool8 IsBattlerGrounded(u8 battlerId)
{
	if (gStatuses3[battlerId] & STATUS3_ROOTED)
		return TRUE;
	else if (GetBattlerAbility(battlerId) == ABILITY_LEVITATE || IS_BATTLER_OF_TYPE(battlerId, TYPE_FLYING))
		return FALSE;
	return TRUE;
}

static bool8 CanBattlerGetOrLoseItem(u8 battlerId, u16 itemId)
{
	u16 ability = GetBattlerAbility(battlerId), species = gBattleMons[battlerId].species;
	u32 personality = gBattleMons[battlerId].personality;
	
	if (GetBattlerItemHoldEffect(battlerId, FALSE) == HOLD_EFFECT_Z_CRYSTAL || ItemIsMail(itemId)
	|| GetSpeciesFormChange(FORM_CHANGE_HOLD_ITEM, species, personality, ability, itemId, 0, FALSE)
	|| GetSpeciesFormChange(FORM_CHANGE_MEGA_EVO, species, personality, ability, itemId, 0, FALSE)
	|| GetSpeciesFormChange(FORM_CHANGE_PRIMAL, species, personality, ability, itemId, 0, FALSE)
	|| GetSpeciesFormChange(FORM_CHANGE_ULTRA_BURST, species, personality, ability, itemId, 0, FALSE))
	    return FALSE;
	return TRUE;
}

bool8 CanStealItem(u8 battlerAtk, u8 battlerDef, u16 itemId)
{
	u8 battlerAtkSide = GetBattlerSide(battlerAtk);
	bool8 battleTypeBlocksItemSteal = !((gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_LINK))
	&& gTrainerBattleOpponent_A != 0x400);
	
	if (((battlerAtkSide == B_SIDE_OPPONENT || gWishFutureKnock.knockedOffMons[battlerAtkSide] & gBitTable[gBattlerPartyIndexes[battlerAtk]])
		&& battleTypeBlocksItemSteal) || !CanBattlerGetOrLoseItem(battlerDef, itemId) || !CanBattlerGetOrLoseItem(battlerAtk, itemId))
		return FALSE;
		
	return TRUE;
}

void SortBattlersBySpeed(u8 *battlers, bool8 slowToFast)
{
	s8 j;
	u8 i, battler;
	u32 currSpeed, speeds[MAX_BATTLERS_COUNT] = {0};
	
	for (i = 0; i < gBattlersCount; i++)
		speeds[i] = GetBattlerTotalSpeed(battlers[i]);
	
	for (i = 1; i < gBattlersCount; i++)
	{
		battler = battlers[i];
		currSpeed = speeds[i];
		j = i - 1;
		
		if (slowToFast)
		{
			while (j >= 0 && speeds[j] > currSpeed)
			{
				battlers[j + 1] = battlers[j];
				speeds[j + 1] = speeds[j];
				j -= 1;
			}
		}
		else
		{
			while (j >= 0 && speeds[j] < currSpeed)
			{
				battlers[j + 1] = battlers[j];
				speeds[j + 1] = speeds[j];
				j -= 1;
			}
		}
		battlers[j + 1] = battler;
		speeds[j + 1] = currSpeed;
	}
}

u8 CountUsablePartyMons(u8 battlerId)
{
	u8 i, ret, battlerOnField2, battlerOnField1 = gBattlerPartyIndexes[battlerId];
	struct Pokemon *party = GetBattlerParty(battlerId);
	
	if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
		battlerOnField2 = gBattlerPartyIndexes[BATTLE_PARTNER(battlerId)];
	else // In singles there's only one battlerId by side
		battlerOnField2 = battlerOnField1;
		
	for (i = 0, ret = 0; i < PARTY_SIZE; i++)
	{
		if (i != battlerOnField1 && i != battlerOnField2 && MonCanBattle(&party[i]))
			++ret;
	}
	return ret;
}

bool8 CanBattlerEscape(u8 battlerId, bool8 checkIngrain)
{
	if (IS_BATTLER_OF_TYPE(battlerId, TYPE_GHOST))
		return TRUE;
	else if ((gBattleMons[battlerId].status2 & (STATUS2_ESCAPE_PREVENTION | STATUS2_WRAPPED)) || (checkIngrain && gStatuses3[battlerId] & STATUS3_ROOTED))
		return FALSE;
	else
		return TRUE;
}

u8 IsAbilityPreventingSwitchOut(u8 battlerId)
{
	u8 ret;
	
	if (!IS_BATTLER_OF_TYPE(battlerId, TYPE_GHOST))
	{
		if (IsBattlerGrounded(battlerId) && (ret = ABILITY_ON_OPPOSING_SIDE(battlerId, ABILITY_ARENA_TRAP)))
			return ret;
		else if (GetBattlerAbility(battlerId) != ABILITY_SHADOW_TAG && (ret = ABILITY_ON_OPPOSING_SIDE(battlerId, ABILITY_SHADOW_TAG)))
			return ret;
		else if (IS_BATTLER_OF_TYPE(battlerId, TYPE_STEEL) && (ret = ABILITY_ON_OPPOSING_SIDE(battlerId, ABILITY_MAGNET_PULL)))
			return ret;
	}
	return 0;
}

bool8 CanBattlerSwitch(u8 battlerId)
{
	u8 i, battlerIn1, battlerIn2, lastMonId;
    struct Pokemon *party = GetBattlerParty(battlerId);
	
    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        if (GetLinkTrainerFlankId(GetBattlerMultiplayerId(battlerId)) == 1)
            i = 3;
		else
			i = 0;
		
        for (lastMonId = i + 3; i < lastMonId; ++i)
        {
            if (MonCanBattle(&party[i]) && gBattlerPartyIndexes[battlerId] != i)
                break;
        }
    }
    else
    {
        if (GetBattlerSide(battlerId) == B_SIDE_OPPONENT)
        {
            battlerIn1 = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
		
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                battlerIn2 = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            else
                battlerIn2 = battlerIn1;
        }
        else
        {
            battlerIn1 = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
		
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                battlerIn2 = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            else
                battlerIn2 = battlerIn1;
        }
        for (i = 0, lastMonId = PARTY_SIZE; i < PARTY_SIZE; ++i)
        {
            if (MonCanBattle(&party[i]) && i != gBattlerPartyIndexes[battlerIn1] && i != gBattlerPartyIndexes[battlerIn2])
                break;
        }
    }
	return (i != lastMonId);
}

u8 GetBattlerTurnOrderNum(u8 battlerId)
{
    u8 i;
    
    for (i = 0; i < gBattlersCount && gBattlerByTurnOrder[i] != battlerId; ++i) {};
	
	return i;
}

u32 GetEffectChanceIncreases(u8 battlerId, u32 secondaryEffectChance)
{
	if (GetBattlerAbility(battlerId) == ABILITY_SERENE_GRACE) // Check Serene Grace
		secondaryEffectChance *= 2;
	
	return secondaryEffectChance;
}

u8 IsUproarActive(void)
{
	u8 i;
	
	for (i = 0; i < gBattlersCount; i++)
	{
		if ((gBattleMons[i].status2 & STATUS2_UPROAR))
			break;
	}
	return i;
}

bool8 UproarWakeUpCheck(void)
{
	u8 ret = IsUproarActive();
	
	if (ret != gBattlersCount)
	{
		gBattleScripting.battler = ret;
		
		if (gBattlerTarget == 0xFF)
			gBattlerTarget = ret;
		else if (gBattlerTarget == ret)
			gBattleCommunication[MULTISTRING_CHOOSER] = 0;
		else
			gBattleCommunication[MULTISTRING_CHOOSER] = 1;
		
		return TRUE;
	}
	return FALSE;
}

bool8 TryResetBattlerStatChanges(u8 battlerId)
{
	u8 i;
	bool8 statReseted = FALSE;
	
	for (i = 0; i < NUM_BATTLE_STATS; i++)
	{
		if (gBattleMons[battlerId].statStages[i] != DEFAULT_STAT_STAGES)
		{
			gBattleMons[battlerId].statStages[i] = DEFAULT_STAT_STAGES;
			statReseted = TRUE; // Return TRUE if any stat was reseted
		}
	}
	return statReseted;
}

void CopyBattlerStatChanges(u8 battler1, u8 battler2) // Battler1 copy battler2's stat changes
{
	u8 i;
	
	for (i = 0; i < NUM_BATTLE_STATS; i++)
		gBattleMons[battler1].statStages[i] = gBattleMons[battler2].statStages[i];
}

u8 GetBattlerGender(u8 battlerId)
{
	return GetGenderFromSpeciesAndPersonality(gBattleMons[battlerId].species, gBattleMons[battlerId].personality);
}

// Check if battlerIdAtk can be infatuated by battlerIdDef
u8 CanBeInfatuatedBy(u8 battlerIdAtk, u8 battlerIdDef)
{
	u8 ret;
	
	// Check attacker's abilities
	if (GetBattlerAbility(battlerIdAtk) == ABILITY_OBLIVIOUS)
		return STATUS_CHANGE_FAIL_ABILITY_PREVENTED;
	
	// Check Aroma Veil
	ret = CheckAbilityInBattle(CHECK_ABILITY_ON_SIDE, battlerIdAtk, ABILITY_AROMA_VEIL);
	
	if (ret)
	{
		gBattleScripting.savedBattler = ret - 1;
		return STATUS_CHANGE_FAIL_AROMA_VEIL_ON_SIDE;
	}
	// Check already infatuated
	if (gBattleMons[battlerIdAtk].status2 & STATUS2_INFATUATION)
		return STATUS_CHANGE_FAIL_ALREADY_STATUSED;
	// Check gender
	else if (GetBattlerGender(battlerIdAtk) == MON_GENDERLESS || GetBattlerGender(battlerIdDef) == MON_GENDERLESS || ARE_BATTLERS_OF_SAME_GENDER(battlerIdAtk, battlerIdDef))
		return STATUS_CHANGE_FAIL_TYPE_NOT_AFFECTED; // Used it instead of add a new return value
	
	return STATUS_CHANGE_WORKED;
}

u32 GetBattlerWeight(u8 battlerId)
{
	u32 weight = GetPokedexHeightWeight(SpeciesToNationalPokedexNum(gBattleMons[battlerId].species), 1);
	
	switch (GetBattlerAbility(battlerId))
	{
		case ABILITY_HEAVY_METAL:
			weight *= 2;
			break;
		case ABILITY_LIGHT_METAL:
			weight /= 2;
			break;
	}
	return weight;
}

bool8 IsBattlerAffectedBySpore(u8 battlerId)
{
	if (IS_BATTLER_OF_TYPE(battlerId, TYPE_GRASS) || GetBattlerAbility(battlerId) == ABILITY_OVERCOAT)
		return FALSE;
	return TRUE;
}

void RemoveOrAddBattlerOnPickupStack(u8 battlerId, bool8 addToStack)
{
	u8 i, j, newStack[MAX_BATTLERS_COUNT];
	
	for (i = 0, j = 0; i < gBattlersCount; i++)
	{
		if (gBattleStruct->pickupStack[i] != 0xFF && gBattleStruct->pickupStack[i] != battlerId)
			newStack[j++] = gBattleStruct->pickupStack[i];
	}
	if (addToStack)
		newStack[j++] = battlerId;
	
	while (j < gBattlersCount)
		newStack[j++] = 0xFF;
	
	for (i = 0; i < gBattlersCount; i++)
		gBattleStruct->pickupStack[i] = newStack[i];
}

u8 GetBattlerOnTopOfPickupStack(u8 battlerId)
{
	u8 i;
	
	for (i = 0; i < gBattlersCount; i++)
	{
		if (gBattleStruct->pickupStack[i] == 0xFF)
			break;
	}
	// Stack is empty or only contains the ignored battlerId
	if (i == 0 || (i == 1 && gBattleStruct->pickupStack[0] == battlerId))
		return 0xFF;
	
	if (gBattleStruct->pickupStack[i - 1] == battlerId)
		return gBattleStruct->pickupStack[i - 2];
	
	return gBattleStruct->pickupStack[i - 1];
}

bool8 TryRecycleBattlerItem(u8 battlerRecycler, u8 battlerItem)
{
	u16 *usedHeldItem = GetUsedHeldItemPtr(battlerItem);
	
	if (*usedHeldItem && !gBattleMons[battlerRecycler].item)
	{
		gLastUsedItem = *usedHeldItem;
        *usedHeldItem = ITEM_NONE;
		
		gBattleMons[battlerRecycler].item = gLastUsedItem;
		
		RemoveOrAddBattlerOnPickupStack(battlerItem, FALSE);
		
		gBattleResources->flags->flags[battlerRecycler] &= ~(RESOURCE_FLAG_UNBURDEN_BOOST);
		
		if (battlerRecycler != battlerItem)
			CheckSetBattlerUnburden(battlerItem);
		
		BtlController_EmitSetMonData(battlerRecycler, BUFFER_A, REQUEST_HELDITEM_BATTLE, 0, 2, &gBattleMons[battlerRecycler].item);
        MarkBattlerForControllerExec(battlerRecycler);
		return TRUE;
	}
	return FALSE;
}

// Protosynthesis count stat stages, but Beast Boost not. Probably if it be available in SV it will be changed to count too, so just count it...
u8 GetBattlerHighestStatId(u8 battlerId)
{
	u8 i, temp2, statIds[NUM_STATS], highestStatId;
	u16 *statVal, temp, stats[NUM_STATS], highestStat;
	
	// Put the stats and ids into the array
	for (i = STAT_ATK; i < NUM_STATS; i++)
	{
		statVal = &gBattleMons[battlerId].attack + ((i - 1) * 2); // -1 bc STAT_ATK is 1, and not 0
		APPLY_STAT_MOD(stats[i], &gBattleMons[battlerId], *statVal, i);
		statIds[i] = i;
	}
	// Put stats in order of Attack, Defense, Sp. Atk, Sp. Def and Speed
	// without this the order will be Attack, Defense, Speed, Sp. Atk and Sp. Def. What this loop does is only move the Speed to the end of the array
	for (i = STAT_SPEED; i < NUM_STATS - 1; i++)
	{
		SWAP(stats[i], stats[i + 1], temp);
		SWAP(statIds[i], statIds[i + 1], temp2);
	}
	// Get the highest stat id
	for (i = highestStatId = STAT_ATK, highestStat = gBattleMons[battlerId].attack; i < NUM_STATS; i++)
	{
		if (highestStat < stats[statIds[i]])
		{
			highestStat = stats[statIds[i]];
			highestStatId = statIds[i];
		}
	}
	return highestStatId;
}

bool8 MoveHasHealingEffect(u16 move)
{
	switch (gBattleMoves[move].effect)
	{
		case EFFECT_RESTORE_HP:
		case EFFECT_ABSORB:
		case EFFECT_DREAM_EATER:
		case EFFECT_REST:
		case EFFECT_MORNING_SUN:
		case EFFECT_WISH:
		// TODO:
		case EFFECT_HEAL_PULSE:
		case EFFECT_HEALING_WISH:
		case EFFECT_SWALLOW:
		case EFFECT_ROOST:
		    return TRUE;
	}
	return FALSE;
}

bool8 IsBattlerAffectedByFollowMe(u8 battlerId, u8 opposingSide, u16 move)
{
	u16 atkAbility = GetBattlerAbility(battlerId);
	
	if (!gSideTimers[opposingSide].followmeSet || !IsBattlerAlive(gSideTimers[opposingSide].followmeTarget) || atkAbility == ABILITY_PROPELLER_TAIL
	|| atkAbility == ABILITY_STALWART || gBattleMoves[move].effect == EFFECT_SKY_DROP || gBattleMoves[move].effect == EFFECT_FUTURE_SIGHT)
	    return FALSE;
		
	return TRUE;
}

// battlerId = the battler using the item
// partyIndex = the party index the item effect get applyed
u8 GetBattleMonForItemUse(u8 battlerId, u8 partyIndex)
{
	u8 battleMonId = MAX_BATTLERS_COUNT;
	
	if (gMain.inBattle)
	{
		if (partyIndex == gBattlerPartyIndexes[battlerId])
			battleMonId = battlerId;
		else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && partyIndex == gBattlerPartyIndexes[BATTLE_PARTNER(battlerId)])
			battleMonId = BATTLE_PARTNER(battlerId);
	}
	return battleMonId;
}

u8 GetItemUseBattler(u8 battlerId)
{
	return GetBattleMonForItemUse(battlerId, gBattleStruct->itemPartyIndex[battlerId]);
}

bool8 IsItemUseBlockedByBattleEffect(void)
{
	bool8 blocked = FALSE;
	u8 playerLeft = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT), playerRight = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
	
	if (gPartyMenu.slotId == 0) // player left mon effects check
	{
		if ((gStatuses3[playerLeft] & STATUS3_EMBARGO))
			blocked = TRUE;
	}
	else if (gPartyMenu.slotId == 1) // player right mon effects check
	{
		if ((gStatuses3[playerRight] & STATUS3_EMBARGO))
			blocked = TRUE;
	}
	return blocked;
}

#define BALL_THROW_SUCCESS                  0
#define BALL_THROW_UNABLE_TWO_MONS          1
#define BALL_THROW_UNABLE_NO_ROOM           2
#define BALL_THROW_UNABLE_SEMI_INVULNERABLE 3

static u8 GetBallThrowableState(void)
{
	if (IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)) && IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)))
		return BALL_THROW_UNABLE_TWO_MONS;
	else if (IsPlayerPartyAndPokemonStorageFull())
		return BALL_THROW_UNABLE_NO_ROOM;
	else if (gStatuses3[GetCatchingBattler()] & STATUS3_SEMI_INVULNERABLE)
		return BALL_THROW_UNABLE_SEMI_INVULNERABLE;
	else
		return BALL_THROW_SUCCESS;
}

const u8 *PokemonUseItemEffectsBattle(u8 battlerId, u16 itemId, bool8 *canUse)
{
	const u8 *failStr = NULL;
	u8 i, holdEffectParam = ItemId_GetHoldEffectParam(itemId);
	
	switch (ItemId_GetBattleUsage(itemId))
	{
		case EFFECT_ITEM_INCREASE_STAT:
		    if (CompareStat(battlerId, holdEffectParam, MAX_STAT_STAGES, CMP_LESS_THAN))
				*canUse = TRUE;
			break;
		case EFFECT_ITEM_SET_FOCUS_ENERGY:
		    if (!(gBattleMons[battlerId].status2 & STATUS2_FOCUS_ENERGY))
				*canUse = TRUE;
			break;
		case EFFECT_ITEM_SET_MIST:
		    if (!(gSideStatuses[GetBattlerSide(battlerId)] & SIDE_STATUS_MIST))
				*canUse = TRUE;
			break;
		case EFFECT_ITEM_ESCAPE:
		    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
				*canUse = TRUE;
			break;
		case EFFECT_ITEM_THROW_BALL:
		    switch (GetBallThrowableState())
			{
				case BALL_THROW_SUCCESS:
					*canUse = TRUE;
					break;
				case BALL_THROW_UNABLE_TWO_MONS:
				    failStr = gText_CantThrowPokeBall_TwoMons;
					break;
				case BALL_THROW_UNABLE_NO_ROOM:
				    failStr = gUnknown_8416631;
					break;
				case BALL_THROW_UNABLE_SEMI_INVULNERABLE:
				    failStr = gText_CantThrowPokeBall_SemiInvulnerable;
					break;
			}
			break;
		case EFFECT_ITEM_INCREASE_ALL_STATS:
		    for (i = STAT_ATK; i < NUM_STATS; i++)
			{
				if (CompareStat(battlerId, i, MAX_STAT_STAGES, CMP_LESS_THAN))
					*canUse = TRUE;
			}
			break;
		case EFFECT_ITEM_POKE_FLUTE:
			*canUse = TRUE; // Always can be used
			break;
	}
	return failStr;
}

bool8 IsBattleAnimationsOn(void)
{
	if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_POKEDUDE)) && gSaveBlock2Ptr->optionsBattleSceneOff)
		return FALSE;
	return TRUE;
}

struct Pokemon *GetBattlerParty(u8 battlerId)
{
	return GetBattlerSide(battlerId) == B_SIDE_PLAYER ? gPlayerParty : gEnemyParty;
}

struct Pokemon *GetBattlerPartyIndexPtr(u8 battlerId)
{
	return &GetBattlerParty(battlerId)[gBattlerPartyIndexes[battlerId]];
}

// Same as above, but checking for Illusion
struct Pokemon *GetBattlerIllusionPartyIndexPtr(u8 battlerId)
{
	struct Pokemon *illusionMon = GetIllusionMonPtr(battlerId);
	return illusionMon != NULL ? illusionMon : GetBattlerPartyIndexPtr(battlerId);
}

u8 CountAliveMonsInBattle(u8 battlerId, u8 caseId)
{
    s32 i;
    u8 retVal = 0;

    switch (caseId)
    {
    case BATTLE_ALIVE_SIDE:
        for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        {
            if (GetBattlerSide(i) == GetBattlerSide(battlerId) && !(gAbsentBattlerFlags & gBitTable[i]))
                retVal++;
        }
        break;
	case BATTLE_ALIVE_EXCEPT_BATTLER:
	    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        {
            if (i != battlerId && !(gAbsentBattlerFlags & gBitTable[i]))
                retVal++;
        }
        break;
    }
    return retVal;
}

void CalculatePayDayMoney(void)
{
	u8 i;
	
	for (i = 0; i < (MAX_BATTLERS_COUNT / 2); i++)
	{
		if (gBattleStruct->payDayLevels[i])
		{
			u16 payDayMoney = gPaydayMoney + (gBattleStruct->payDayLevels[i] * 5);
			
			if (payDayMoney > 0xFFFF)
				gPaydayMoney = 0xFFFF;
			else
				gPaydayMoney = payDayMoney;
		}
		gBattleStruct->payDayLevels[i] = 0;
	}
}

u8 GetTrappingIdByMove(u16 move)
{
	switch (move)
	{
		case MOVE_BIND:
			return TRAP_ID_BIND;
		case MOVE_WRAP:
			return TRAP_ID_WRAP;
		case MOVE_FIRE_SPIN:
			return TRAP_ID_FIRE_SPIN;
		case MOVE_WHIRLPOOL:
			return TRAP_ID_WHIRLPOOL;
		case MOVE_CLAMP:
			return TRAP_ID_CLAMP;
		case MOVE_SAND_TOMB:
			return TRAP_ID_SAND_TOMB;
	}
}

s32 GetDrainedBigRootHp(u8 battlerId, s32 hp)
{
	if (GetBattlerItemHoldEffect(battlerId, TRUE) == HOLD_EFFECT_BIG_ROOT)
		hp = (hp * (100 + ItemId_GetHoldEffectParam(gBattleMons[battlerId].item))) / 100;
	
	if (hp == 0)
		hp = 1;
	
	return hp * -1;
}

void SetTypeBeforeUsingMove(u16 move, u8 battler)
{
	u16 moveEffect;
	
	gBattleStruct->dynamicMoveType = gBattleMoves[move].type;
	
	if (move == MOVE_STRUGGLE)
		return;
	
	moveEffect = gBattleMoves[move].effect;
	
	switch (moveEffect)
	{
		case EFFECT_WEATHER_BALL:
		    if (IsBattlerWeatherAffected(battler, WEATHER_RAIN_ANY))
				gBattleStruct->dynamicMoveType = TYPE_WATER;
			else if (IsBattlerWeatherAffected(battler, WEATHER_SANDSTORM_ANY))
				gBattleStruct->dynamicMoveType = TYPE_ROCK;
			else if (IsBattlerWeatherAffected(battler, WEATHER_SUN_ANY))
				gBattleStruct->dynamicMoveType = TYPE_FIRE;
			else if (IsBattlerWeatherAffected(battler, WEATHER_HAIL_ANY))
				gBattleStruct->dynamicMoveType = TYPE_ICE;
			break;
		case EFFECT_HIDDEN_POWER:
			gBattleStruct->dynamicMoveType = GetHiddenPowerType(GetBattlerPartyIndexPtr(battler));
			break;
	}
	if (moveEffect != EFFECT_WEATHER_BALL && moveEffect != EFFECT_HIDDEN_POWER && moveEffect != EFFECT_NATURAL_GIFT && moveEffect != EFFECT_CHANGE_TYPE_ON_ITEM
	&& moveEffect != EFFECT_TERRAIN_PULSE)
	{
		switch (GetBattlerAbility(battler))
		{
			case ABILITY_NORMALIZE:
			    if (gBattleStruct->dynamicMoveType != TYPE_NORMAL)
					gBattleStruct->dynamicMoveType = TYPE_NORMAL;
				break;
			case ABILITY_REFRIGERATE:
			    if (gBattleStruct->dynamicMoveType == TYPE_NORMAL)
					gBattleStruct->dynamicMoveType = TYPE_ICE;
				break;
			case ABILITY_PIXILATE:
			    if (gBattleStruct->dynamicMoveType == TYPE_NORMAL)
					gBattleStruct->dynamicMoveType = TYPE_FAIRY;
				break;
			case ABILITY_AERILATE:
			    if (gBattleStruct->dynamicMoveType == TYPE_NORMAL)
					gBattleStruct->dynamicMoveType = TYPE_FLYING;
				break;
			case ABILITY_GALVANIZE:
			    if (gBattleStruct->dynamicMoveType == TYPE_NORMAL)
					gBattleStruct->dynamicMoveType = TYPE_ELECTRIC;
				break;
		}
	}
	else if (gBattleMoves[move].flags.soundMove && GetBattlerAbility(battler) == ABILITY_LIQUID_VOICE)
		gBattleStruct->dynamicMoveType = TYPE_WATER;
}

// Check if battler1 can transform into battler2
bool8 CanTransformIntoBattler(u8 battler1, u8 battler2)
{
	if (!(gBattleMons[battler2].status2 & (STATUS2_TRANSFORMED | STATUS2_SUBSTITUTE)) && !(gStatuses3[battler2] & STATUS3_SEMI_INVULNERABLE)
		&& !gBattleStruct->illusion[battler1].on && !gBattleStruct->illusion[battler2].on && !(gBattleMons[battler1].status2 & STATUS2_TRANSFORMED))
		return TRUE;
	return FALSE;
}

bool8 CanDisableMove(u8 battlerId, u8 movePos, u16 move)
{
	if (move != MOVE_STRUGGLE && !gDisableStructs[battlerId].disabledMove && gBattleMons[battlerId].pp[movePos] && !CheckAbilityInBattle(CHECK_ABILITY_ON_SIDE, battlerId, ABILITY_AROMA_VEIL))
		return TRUE;
	return FALSE;
}

// Check if target can be protected by Safeguard
bool8 CanSafeguardProtectBattler(u8 attacker, u8 defender)
{
	if (GetBattlerAbility(attacker) != ABILITY_INFILTRATOR && (gSideStatuses[GetBattlerSide(defender)] & SIDE_STATUS_SAFEGUARD))
		return TRUE;
	return FALSE;
}

bool8 IsBattlerProtectedByFlowerVeil(u8 battlerId)
{
	u8 battlerPartner;
	
	if (GetBattlerAbility(battlerId) == ABILITY_FLOWER_VEIL && IS_BATTLER_OF_TYPE(battlerId, TYPE_GRASS))
	{
		gBattleScripting.savedBattler = battlerId;
		return TRUE;
	}
	battlerPartner = BATTLE_PARTNER(battlerId);
	
	if (IsBattlerAlive(battlerPartner) && GetBattlerAbility(battlerPartner) == ABILITY_FLOWER_VEIL && IS_BATTLER_OF_TYPE(battlerId, TYPE_GRASS))
	{
		gBattleScripting.savedBattler = battlerPartner;
		return TRUE;
	}
	return FALSE;
}

u8 GetFutureAttackStringId(u16 move)
{
	u8 stringId;
	
	switch (move)
	{
		case MOVE_DOOM_DESIRE:
		    stringId = B_MSG_CHOSE_AS_DESTINY;
			break;
		default:
		    stringId = B_MSG_FORESAW_ATTACK;
			break;
	}
	gBattleCommunication[MULTISTRING_CHOOSER] = stringId;
	return stringId;
}

void SaveBattlersHps(void)
{
	u8 i;
	
	for (i = 0; i < MAX_BATTLERS_COUNT; i++)
		gBattleStruct->hpBefore[i] = gBattleMons[i].hp;
}

// TODO: This will check for Tera Type
bool8 IsBattlerOfType(u8 battlerId, u8 type)
{
	return IS_BATTLER_OF_TYPE(battlerId, type);
}

static inline void SetBattlerTypesInternal(u8 battlerId, u8 type1, u8 type2)
{
	gBattleMons[battlerId].type1 = type1;
    gBattleMons[battlerId].type2 = type2;
	gBattleMons[battlerId].type3 = TYPE_MYSTERY;
}

void SetBattlerType(u8 battlerId, u8 type)
{
	gBattleScripting.battler = battlerId;
	SetBattlerTypesInternal(battlerId, type, type);
	PrepareTypeBuffer(gBattleTextBuff1, type);
}

void SetBattlerInitialTypes(u8 battlerId)
{
	u16 species = gBattleMons[battlerId].species;
	SetBattlerTypesInternal(battlerId, gBaseStats[species].type1, gBaseStats[species].type2);
}

// Attacker copy Defender's crit modifiers
bool8 CopyBattlerCritModifier(u8 attacker, u8 defender)
{
	bool8 copied = FALSE;
	
	gBattleMons[attacker].status2 &= ~(STATUS2_FOCUS_ENERGY);
	gBattleMons[attacker].status2 |= (gBattleMons[defender].status2 & STATUS2_FOCUS_ENERGY);
	
	if (gBattleMons[attacker].status2 & STATUS2_FOCUS_ENERGY)
		copied = TRUE;
	
	return copied;
}

bool8 TryRemoveScreens(u8 battler, bool8 fromBothSides)
{
	u8 opposingSide, battlerSide = GetBattlerSide(battler);
	bool8 removed = FALSE;
	
	if ((gSideStatuses[battlerSide] & SIDE_STATUS_SCREENS_ANY))
	{
		gSideStatuses[battlerSide] &= ~(SIDE_STATUS_SCREENS_ANY);
		removed = TRUE;
	}
	
	if (fromBothSides)
	{
		opposingSide = GetBattlerSide(BATTLE_OPPOSITE(battler));
		
		if ((gSideStatuses[opposingSide] & SIDE_STATUS_SCREENS_ANY))
		{
			gSideStatuses[opposingSide] &= ~(SIDE_STATUS_SCREENS_ANY);
			removed = TRUE;
		}
	}
	return removed;
}

bool8 DoesSpreadMoveStrikesOnlyOnce(u8 attacker, u8 defender, u16 move, bool8 checkTargetsDone)
{
	if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
	{
		switch (GetBattlerMoveTargetType(attacker, move))
		{
			case MOVE_TARGET_BOTH:
			    if (IS_WHOLE_SIDE_ALIVE(defender)) // Both foes alive to be attacked
				    return FALSE;
				else if (checkTargetsDone && gBattleStruct->targetsDone[attacker] & gBitTable[BATTLE_PARTNER(defender)]) // Already did damage to one foe
					return FALSE;
				break;
			case MOVE_TARGET_FOES_AND_ALLY:
				if (CountAliveMonsInBattle(attacker, BATTLE_ALIVE_EXCEPT_BATTLER) >= 2) // Both foes or ally alive to be attacked
				    return FALSE;
				else if (checkTargetsDone && ((gBattleStruct->targetsDone[attacker] & gBitTable[BATTLE_PARTNER(defender)])
					|| (gBattleStruct->targetsDone[attacker] & gBitTable[BATTLE_PARTNER(attacker)]))) // Already did damage at least one target
				    return FALSE;
				break;
		}
	}
	return TRUE;
}
