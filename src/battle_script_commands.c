#include "global.h"
#include "gflib.h"
#include "item.h"
#include "util.h"
#include "random.h"
#include "pokedex.h"
#include "money.h"
#include "battle_setup.h"
#include "pokemon_icon.h"
#include "mail.h"
#include "mail_data.h"
#include "event_data.h"
#include "strings.h"
#include "wild_encounter.h"
#include "pokemon_special_anim.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "task.h"
#include "naming_screen.h"
#include "dns.h"
#include "overworld.h"
#include "form_change.h"
#include "party_menu.h"
#include "battle_queued_effects.h"
#include "trainer_pokemon_sprites.h"
#include "field_specials.h"
#include "battle.h"
#include "pokemon.h"
#include "battle_message.h"
#include "battle_anim.h"
#include "battle_main.h"
#include "battle_ai_script_commands.h"
#include "calculate_base_damage.h"
#include "battle_scripts.h"
#include "trainer_slide.h"
#include "set_effect.h"
#include "reshow_battle_screen.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "constants/battle_anim.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_script_commands.h"
#include "constants/form_change.h"
#include "constants/items.h"
#include "constants/hold_effects.h"
#include "constants/songs.h"
#include "constants/moves.h"
#include "constants/abilities.h"
#include "constants/pokemon.h"
#include "constants/maps.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_string_ids.h"

struct StatFractions
{
    u8 dividend;
    u8 divisor;
};

struct PickupItem
{
    u16 itemId;
    u8 chance;
};

static void TrySetDestinyBondToHappen(void);
static void InitLvlUpBox(void);
static bool8 SlideInLvlUpBox(void);
static bool8 SlideOutLvlUpBox(void);
static void DrawLevelUpWindow1(void);
static void DrawLevelUpWindow2(void);
static void PutMonIconOnLvlUpBox(void);
static void PutLevelAndGenderOnLvlUpBox(void);
static bool8 HasAttackerFaintedTarget(void);
static bool8 NoTargetPresent(void);
static bool8 AttacksThisTurn(u8 battlerId, u16 move);
static void SpriteCB_MonIconOnLvlUpBox(struct Sprite *sprite);

static void atk00_attackcanceler(void);
static void atk01_accuracycheck(void);
static void atk02_attackstring(void);
static void atk03_ppreduce(void);
static void atk04_critcalc(void);
static void atk05_damagecalc(void);
static void atk06_typecalc(void);
static void atk07_adjustdamage(void);
static void atk08_nop(void);
static void atk09_attackanimation(void);
static void atk0A_nop(void);
static void atk0B_healthbarupdate(void);
static void atk0C_datahpupdate(void);
static void atk0D_critmessage(void);
static void atk0E_effectivenesssound(void);
static void atk0F_resultmessage(void);
static void atk10_printstring(void);
static void atk11_printselectionstring(void);
static void atk12_waitmessage(void);
static void atk13_printfromtable(void);
static void atk14_printselectionstringfromtable(void);
static void atk15_seteffectwithchance(void);
static void atk16_setspecialmoveeffect(void);
static void atk17_nop(void);
static void atk18_clearbattlerstatus(void);
static void atk19_tryfaintmon(void);
static void atk1A_dofaintanimation(void);
static void atk1B_cleareffectsonfaint(void);
static void atk1C_jumpbasedonstatus(void);
static void atk1D_nop(void);
static void atk1E_jumpifability(void);
static void atk1F_jumpifsideaffecting(void);
static void atk20_jumpifstat(void);
static void atk21_nop(void);
static void atk22_jumpbasedontype(void);
static void atk23_getexp(void);
static void atk24_checkteamslot(void);
static void atk25_jumpifabilityonside(void);
static void atk26_setmoveeffect(void);
static void atk27_nop(void);
static void atk28_goto(void);
static void atk29_jumpifbyte(void);
static void atk2A_jumpifhalfword(void);
static void atk2B_jumpifword(void);
static void atk2C_jumpifarrayequal(void);
static void atk2D_jumpifarraynotequal(void);
static void atk2E_setbyte(void);
static void atk2F_addbyte(void);
static void atk30_subbyte(void);
static void atk31_copyarray(void);
static void atk32_sethalfword(void);
static void atk33_orbyte(void);
static void atk34_orhalfword(void);
static void atk35_orword(void);
static void atk36_bicbyte(void);
static void atk37_bichalfword(void);
static void atk38_bicword(void);
static void atk39_pause(void);
static void atk3A_waitstate(void);
static void atk3B_nop(void);
static void atk3C_return(void);
static void atk3D_end(void);
static void atk3E_end2(void);
static void atk3F_end3(void);
static void atk40_nop(void);
static void atk41_call(void);
static void atk42_trysetsleep(void);
static void atk43_trysetconfusion(void);
static void atk44_endselectionscript(void);
static void atk45_playanimation(void);
static void atk46_playanimation2(void);
static void atk47_setgraphicalstatchangevalues(void);
static void atk48_playstatchangeanimation(void);
static void atk49_moveend(void);
static void atk4A_formchange(void);
static void atk4B_returnatktoball(void);
static void atk4C_getswitchedmondata(void);
static void atk4D_switchindataupdate(void);
static void atk4E_switchinanim(void);
static void atk4F_jumpifcantswitch(void);
static void atk50_openpartyscreen(void);
static void atk51_switchhandleorder(void);
static void atk52_switchineffects(void);
static void atk53_nop(void);
static void atk54_playse(void);
static void atk55_fanfare(void);
static void atk56_playfaintcry(void);
static void atk57_endlinkbattle(void);
static void atk58_returntoball(void);
static void atk59_handlelearnnewmove(void);
static void atk5A_yesnoboxlearnmove(void);
static void atk5B_yesnoboxstoplearningmove(void);
static void atk5C_nop(void);
static void atk5D_getmoneyreward(void);
static void atk5E_nop(void);
static void atk5F_swapattackerwithtarget(void);
static void atk60_nop(void);
static void atk61_drawpartystatussummary(void);
static void atk62_hidepartystatussummary(void);
static void atk63_jumptocalledmove(void);
static void atk64_argumenttomoveeffect(void);
static void atk65_argumenttomovedamage(void);
static void atk66_chosenstatusanimation(void);
static void atk67_yesnobox(void);
static void atk68_nop(void);
static void atk69_nop(void);
static void atk6A_removeitem(void);
static void atk6B_nop(void);
static void atk6C_drawlvlupbox(void);
static void atk6D_nop(void);
static void atk6E_nop(void);
static void atk6F_makevisible(void);
static void atk70_recordlastability(void);
static void atk71_nop(void);
static void atk72_jumpifplayerran(void);
static void atk73_hpthresholds(void);
static void atk74_hpthresholds2(void);
static void atk75_nop(void);
static void atk76_various(void);
static void atk77_setprotectlike(void);
static void atk78_instanthpdrop(void);
static void atk79_setatkhptozero(void);
static void atk7A_nop(void);
static void atk7B_tryhealhalfhealth(void);
static void atk7C_trymirrormove(void);
static void atk7D_setrain(void);
static void atk7E_trysetreflect(void);
static void atk7F_setseeded(void);
static void atk80_manipulatedamage(void);
static void atk81_trysetrest(void);
static void atk82_nop(void);
static void atk83_handletrainerslidecase(void);
static void atk84_trysetpoison(void);
static void atk85_stockpile(void);
static void atk86_stockpiletobasedamage(void);
static void atk87_stockpiletohpheal(void);
static void atk88_nop(void);
static void atk89_statbuffchange(void);
static void atk8A_nop(void);
static void atk8B_nop(void);
static void atk8C_nop(void);
static void atk8D_nop(void);
static void atk8E_nop(void);
static void atk8F_forcerandomswitch(void);
static void atk90_nop(void);
static void atk91_givepaydaymoney(void);
static void atk92_trysetlightscreen(void);
static void atk93_tryKO(void);
static void atk94_nop(void);
static void atk95_setsandstorm(void);
static void atk96_nop(void);
static void atk97_tryinfatuating(void);
static void atk98_updatestatusicon(void);
static void atk99_nop(void);
static void atk9A_nop(void);
static void atk9B_transformdataexecution(void);
static void atk9C_setsubstitute(void);
static void atk9D_mimicattackcopy(void);
static void atk9E_nop(void);
static void atk9F_nop(void);
static void atkA0_nop(void);
static void atkA1_trycounterattack(void);
static void atkA2_nop(void);
static void atkA3_disablelastusedattack(void);
static void atkA4_trysetencore(void);
static void atkA5_painsplitdmgcalc(void);
static void atkA6_settypetorandomresistance(void);
static void atkA7_trysetalwayshitflag(void);
static void atkA8_copymovepermanently(void);
static void atkA9_trychoosesleeptalkmove(void);
static void atkAA_nop(void);
static void atkAB_nop(void);
static void atkAC_trysetburn(void);
static void atkAD_tryspiteppreduce(void);
static void atkAE_healpartystatus(void);
static void atkAF_cursetarget(void);
static void atkB0_trysetspikes(void);
static void atkB1_nop(void);
static void atkB2_trysetperishsong(void);
static void atkB3_handlerollout(void);
static void atkB4_jumpifconfusedandstatmaxed(void);
static void atkB5_handlefurycutter(void);
static void atkB6_trysetparalyze(void);
static void atkB7_presentcalc(void);
static void atkB8_trysetsafeguard(void);
static void atkB9_magnitudedamagecalculation(void);
static void atkBA_jumpifnopursuitswitchdmg(void);
static void atkBB_setsunny(void);
static void atkBC_maxattackhalvehp(void);
static void atkBD_nop(void);
static void atkBE_rapidspinfree(void);
static void atkBF_nop(void);
static void atkC0_recoverbasedonweather(void);
static void atkC1_nop(void);
static void atkC2_nop(void);
static void atkC3_trysetfutureattack(void);
static void atkC4_nop(void);
static void atkC5_setsemiinvulnerablebit(void);
static void atkC6_clearsemiinvulnerablebit(void);
static void atkC7_nop(void);
static void atkC8_sethail(void);
static void atkC9_jumpifattackandspecialattackcannotfall(void);
static void atkCA_nop(void);
static void atkCB_nop(void);
static void atkCC_nop(void);
static void atkCD_nop(void);
static void atkCE_settorment(void);
static void atkCF_nop(void);
static void atkD0_settaunt(void);
static void atkD1_trysethelpinghand(void);
static void atkD2_tryswapitems(void);
static void atkD3_copyability(void);
static void atkD4_trywish(void);
static void atkD5_trysetroots(void);
static void atkD6_nop(void);
static void atkD7_setyawn(void);
static void atkD8_nop(void);
static void atkD9_nop(void);
static void atkDA_swapabilities(void);
static void atkDB_tryimprison(void);
static void atkDC_trysetgrudge(void);
static void atkDD_nop(void);
static void atkDE_assistattackselect(void);
static void atkDF_trysetmagiccoat(void);
static void atkE0_trysetsnatch(void);
static void atkE1_nop(void);
static void atkE2_switchoutabilities(void);
static void atkE3_nop(void);
static void atkE4_getsecretpowereffect(void);
static void atkE5_pickup(void);
static void atkE6_nop(void);
static void atkE7_nop(void);
static void atkE8_settypebasedhalvers(void);
static void atkE9_nop(void);
static void atkEA_tryrecycleitem(void);
static void atkEB_settypetoterrain(void);
static void atkEC_nop(void);
static void atkED_snatchsetbattlers(void);
static void atkEE_nop(void);
static void atkEF_handleballthrow(void);
static void atkF0_givecaughtmon(void);
static void atkF1_trysetcaughtmondexflags(void);
static void atkF2_displaydexinfo(void);
static void atkF3_trygivecaughtmonnick(void);
static void atkF4_nop(void);
static void atkF5_nop(void);
static void atkF6_finishaction(void);
static void atkF7_finishturn(void);
static void atkF8_callfunction(void);
static void atkF9_cureprimarystatus(void);
static void atkFA_setword(void);
static void atkFB_jumpifsubstituteblocks(void);
static void atkFC_handleabilitypopup(void);
static void atkFD_nop(void);
static void atkFE_nop(void);
static void atkFF_nop(void);

void (* const gBattleScriptingCommandsTable[])(void) =
{
    atk00_attackcanceler,
    atk01_accuracycheck,
    atk02_attackstring,
    atk03_ppreduce,
    atk04_critcalc,
    atk05_damagecalc,
    atk06_typecalc,
    atk07_adjustdamage,
    atk08_nop,
    atk09_attackanimation,
    atk0A_nop,
    atk0B_healthbarupdate,
    atk0C_datahpupdate,
    atk0D_critmessage,
    atk0E_effectivenesssound,
    atk0F_resultmessage,
    atk10_printstring,
    atk11_printselectionstring,
    atk12_waitmessage,
    atk13_printfromtable,
    atk14_printselectionstringfromtable,
    atk15_seteffectwithchance,
    atk16_setspecialmoveeffect,
    atk17_nop,
    atk18_clearbattlerstatus,
    atk19_tryfaintmon,
    atk1A_dofaintanimation,
    atk1B_cleareffectsonfaint,
    atk1C_jumpbasedonstatus,
    atk1D_nop,
    atk1E_jumpifability,
    atk1F_jumpifsideaffecting,
    atk20_jumpifstat,
    atk21_nop,
    atk22_jumpbasedontype,
    atk23_getexp,
    atk24_checkteamslot,
    atk25_jumpifabilityonside,
    atk26_setmoveeffect,
    atk27_nop,
    atk28_goto,
    atk29_jumpifbyte,
    atk2A_jumpifhalfword,
    atk2B_jumpifword,
    atk2C_jumpifarrayequal,
    atk2D_jumpifarraynotequal,
    atk2E_setbyte,
    atk2F_addbyte,
    atk30_subbyte,
    atk31_copyarray,
    atk32_sethalfword,
    atk33_orbyte,
    atk34_orhalfword,
    atk35_orword,
    atk36_bicbyte,
    atk37_bichalfword,
    atk38_bicword,
    atk39_pause,
    atk3A_waitstate,
    atk3B_nop,
    atk3C_return,
    atk3D_end,
    atk3E_end2,
    atk3F_end3,
    atk40_nop,
    atk41_call,
    atk42_trysetsleep,
    atk43_trysetconfusion,
    atk44_endselectionscript,
    atk45_playanimation,
    atk46_playanimation2,
    atk47_setgraphicalstatchangevalues,
    atk48_playstatchangeanimation,
    atk49_moveend,
    atk4A_formchange,
    atk4B_returnatktoball,
    atk4C_getswitchedmondata,
    atk4D_switchindataupdate,
    atk4E_switchinanim,
    atk4F_jumpifcantswitch,
    atk50_openpartyscreen,
    atk51_switchhandleorder,
    atk52_switchineffects,
    atk53_nop,
    atk54_playse,
    atk55_fanfare,
    atk56_playfaintcry,
    atk57_endlinkbattle,
    atk58_returntoball,
    atk59_handlelearnnewmove,
    atk5A_yesnoboxlearnmove,
    atk5B_yesnoboxstoplearningmove,
    atk5C_nop,
    atk5D_getmoneyreward,
    atk5E_nop,
    atk5F_swapattackerwithtarget,
    atk60_nop,
    atk61_drawpartystatussummary,
    atk62_hidepartystatussummary,
    atk63_jumptocalledmove,
    atk64_argumenttomoveeffect,
    atk65_argumenttomovedamage,
    atk66_chosenstatusanimation,
    atk67_yesnobox,
    atk68_nop,
    atk69_nop,
    atk6A_removeitem,
    atk6B_nop,
    atk6C_drawlvlupbox,
    atk6D_nop,
    atk6E_nop,
    atk6F_makevisible,
    atk70_recordlastability,
    atk71_nop,
    atk72_jumpifplayerran,
    atk73_hpthresholds,
    atk74_hpthresholds2,
    atk75_nop,
    atk76_various,
    atk77_setprotectlike,
    atk78_instanthpdrop,
    atk79_setatkhptozero,
    atk7A_nop,
    atk7B_tryhealhalfhealth,
    atk7C_trymirrormove,
    atk7D_setrain,
    atk7E_trysetreflect,
    atk7F_setseeded,
    atk80_manipulatedamage,
    atk81_trysetrest,
    atk82_nop,
    atk83_handletrainerslidecase,
    atk84_trysetpoison,
    atk85_stockpile,
    atk86_stockpiletobasedamage,
    atk87_stockpiletohpheal,
    atk88_nop,
    atk89_statbuffchange,
    atk8A_nop,
    atk8B_nop,
    atk8C_nop,
    atk8D_nop,
    atk8E_nop,
    atk8F_forcerandomswitch,
    atk90_nop,
    atk91_givepaydaymoney,
    atk92_trysetlightscreen,
    atk93_tryKO,
    atk94_nop,
    atk95_setsandstorm,
    atk96_nop,
    atk97_tryinfatuating,
    atk98_updatestatusicon,
    atk99_nop,
    atk9A_nop,
    atk9B_transformdataexecution,
    atk9C_setsubstitute,
    atk9D_mimicattackcopy,
    atk9E_nop,
    atk9F_nop,
    atkA0_nop,
    atkA1_trycounterattack,
    atkA2_nop,
    atkA3_disablelastusedattack,
    atkA4_trysetencore,
    atkA5_painsplitdmgcalc,
    atkA6_settypetorandomresistance,
    atkA7_trysetalwayshitflag,
    atkA8_copymovepermanently,
    atkA9_trychoosesleeptalkmove,
    atkAA_nop,
    atkAB_nop,
    atkAC_trysetburn,
    atkAD_tryspiteppreduce,
    atkAE_healpartystatus,
    atkAF_cursetarget,
    atkB0_trysetspikes,
    atkB1_nop,
    atkB2_trysetperishsong,
    atkB3_handlerollout,
    atkB4_jumpifconfusedandstatmaxed,
    atkB5_handlefurycutter,
    atkB6_trysetparalyze,
    atkB7_presentcalc,
    atkB8_trysetsafeguard,
    atkB9_magnitudedamagecalculation,
    atkBA_jumpifnopursuitswitchdmg,
    atkBB_setsunny,
    atkBC_maxattackhalvehp,
    atkBD_nop,
    atkBE_rapidspinfree,
    atkBF_nop,
    atkC0_recoverbasedonweather,
    atkC1_nop,
    atkC2_nop,
    atkC3_trysetfutureattack,
    atkC4_nop,
    atkC5_setsemiinvulnerablebit,
    atkC6_clearsemiinvulnerablebit,
    atkC7_nop,
    atkC8_sethail,
    atkC9_jumpifattackandspecialattackcannotfall,
    atkCA_nop,
    atkCB_nop,
    atkCC_nop,
    atkCD_nop,
    atkCE_settorment,
    atkCF_nop,
    atkD0_settaunt,
    atkD1_trysethelpinghand,
    atkD2_tryswapitems,
    atkD3_copyability,
    atkD4_trywish,
    atkD5_trysetroots,
    atkD6_nop,
    atkD7_setyawn,
    atkD8_nop,
    atkD9_nop,
    atkDA_swapabilities,
    atkDB_tryimprison,
    atkDC_trysetgrudge,
    atkDD_nop,
    atkDE_assistattackselect,
    atkDF_trysetmagiccoat,
    atkE0_trysetsnatch,
    atkE1_nop,
    atkE2_switchoutabilities,
    atkE3_nop,
    atkE4_getsecretpowereffect,
    atkE5_pickup,
    atkE6_nop,
    atkE7_nop,
    atkE8_settypebasedhalvers,
    atkE9_nop,
    atkEA_tryrecycleitem,
    atkEB_settypetoterrain,
    atkEC_nop,
    atkED_snatchsetbattlers,
    atkEE_nop,
    atkEF_handleballthrow,
    atkF0_givecaughtmon,
    atkF1_trysetcaughtmondexflags,
    atkF2_displaydexinfo,
    atkF3_trygivecaughtmonnick,
    atkF4_nop,
    atkF5_nop,
    atkF6_finishaction,
    atkF7_finishturn,
    atkF8_callfunction,
    atkF9_cureprimarystatus,
    atkFA_setword,
    atkFB_jumpifsubstituteblocks,
    atkFC_handleabilitypopup,
    atkFD_nop,
    atkFE_nop,
	atkFF_nop,
};

static const struct StatFractions sAccuracyStageRatios[] =
{
    {  33, 100 }, // -6
    {  36, 100 }, // -5
    {  43, 100 }, // -4
    {  50, 100 }, // -3
    {  60, 100 }, // -2
    {  75, 100 }, // -1
    {   1,   1 }, //  0
    { 133, 100 }, // +1
    { 166, 100 }, // +2
    {   2,   1 }, // +3
    { 233, 100 }, // +4
    { 133,  50 }, // +5
    {   3,   1 }, // +6
};

// The chance is 1/N for each stage.
static const u16 sCriticalHitChance[] = { 24, 8, 2, 1, 1 };

static const u16 sUnknownBattleboxPal[] = INCBIN_U16("graphics/battle_interface/unk_battlebox.gbapal");
static const u32 sUnknownBattleboxGfx[] = INCBIN_U32("graphics/battle_interface/unk_battlebox.4bpp.lz");

static const struct OamData sOamData_MonIconOnLvlUpBox =
{
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
};

#define MON_ICON_LVLUP_BOX_TAG 0xD75A

static const struct SpriteTemplate sSpriteTemplate_MonIconOnLvlUpBox =
{
    .tileTag = MON_ICON_LVLUP_BOX_TAG,
    .paletteTag = MON_ICON_LVLUP_BOX_TAG,
    .oam = &sOamData_MonIconOnLvlUpBox,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_MonIconOnLvlUpBox
};

static const u8 sExpBlockLevels[] =
{
    16, //badge 1
    20, //badge 2
    25, //badge 3
    30, //badge 4
    35, //badge 5
    40, //badge 6
    45, //badge 7
    50, //badge 8
};

static const u16 sNaturePowerMoves[] =
{
    [BATTLE_TERRAIN_GRASS]      = MOVE_STUN_SPORE,
    [BATTLE_TERRAIN_LONG_GRASS] = MOVE_RAZOR_LEAF,
    [BATTLE_TERRAIN_SAND]       = MOVE_EARTHQUAKE,
    [BATTLE_TERRAIN_UNDERWATER] = MOVE_HYDRO_PUMP,
    [BATTLE_TERRAIN_WATER]      = MOVE_SURF,
    [BATTLE_TERRAIN_POND]       = MOVE_BUBBLE_BEAM,
    [BATTLE_TERRAIN_MOUNTAIN]   = MOVE_ROCK_SLIDE,
    [BATTLE_TERRAIN_CAVE]       = MOVE_SHADOW_BALL,
    [BATTLE_TERRAIN_BUILDING]   = MOVE_SWIFT,
    [BATTLE_TERRAIN_PLAIN]      = MOVE_SWIFT,
	[BATTLE_TERRAIN_LINK]       = MOVE_SWIFT,
	[BATTLE_TERRAIN_GYM]        = MOVE_SWIFT,
	[BATTLE_TERRAIN_LEADER]     = MOVE_SWIFT,
	[BATTLE_TERRAIN_INDOOR_2]   = MOVE_SWIFT,
	[BATTLE_TERRAIN_INDOOR_1]   = MOVE_SWIFT,
	[BATTLE_TERRAIN_LORELEI]    = MOVE_SWIFT,
	[BATTLE_TERRAIN_BRUNO]      = MOVE_SWIFT,
	[BATTLE_TERRAIN_AGATHA]     = MOVE_SWIFT,
	[BATTLE_TERRAIN_LANCE]      = MOVE_SWIFT,
	[BATTLE_TERRAIN_CHAMPION]   = MOVE_SWIFT,
};

static const struct PickupItem sPickupItems[] =
{
    { ITEM_ORAN_BERRY,   15 },
    { ITEM_CHERI_BERRY,  25 },
    { ITEM_CHESTO_BERRY, 35 },
    { ITEM_PECHA_BERRY,  45 },
    { ITEM_RAWST_BERRY,  55 },
    { ITEM_ASPEAR_BERRY, 65 },
    { ITEM_PERSIM_BERRY, 75 },
    { ITEM_TM10,         80 },
    { ITEM_PP_UP,        85 },
    { ITEM_RARE_CANDY,   90 },
    { ITEM_NUGGET,       95 },
    { ITEM_SPELON_BERRY, 96 },
    { ITEM_PAMTRE_BERRY, 97 },
    { ITEM_WATMEL_BERRY, 98 },
    { ITEM_DURIN_BERRY,  99 },
    { ITEM_BELUE_BERRY,  1  },
};

static const u8 sTerrainToType[] =
{
    TYPE_GRASS,  // tall grass
    TYPE_GRASS,  // long grass
    TYPE_GROUND, // sand
    TYPE_WATER,  // underwater
    TYPE_WATER,  // water
    TYPE_WATER,  // pond water
    TYPE_ROCK,   // rock
    TYPE_ROCK,   // cave
    TYPE_NORMAL, // building
    TYPE_NORMAL, // plain
};

u8 GetCurrentLevelCapLevel(void)
{
	u8 numBadges = GetNumOfBadges();
	return numBadges == NUM_BADGES ? MAX_LEVEL : sExpBlockLevels[numBadges];
}

static void TrySetDestinyBondToHappen(void)
{
    if ((gBattleMons[gBattlerTarget].status2 & STATUS2_DESTINY_BOND) && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget) && !(gHitMarker & HITMARKER_GRUDGE))
        gHitMarker |= HITMARKER_DESTINYBOND;
}

static void MoveValuesCleanUp(void)
{
    gMoveResultFlags = 0;
    gBattleScripting.dmgMultiplier = 1;
    gIsCriticalHit = FALSE;
    gBattleStruct->moveEffect.moveEffectByte = MOVE_EFFECT_NONE;
    gBattleCommunication[MISS_TYPE] = B_MSG_MISSED;
    gHitMarker &= ~(HITMARKER_DESTINYBOND | HITMARKER_SYNCHRONISE_EFFECT);
}

static bool8 HasAttackerFaintedTarget(void)
{
	if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && !IS_MOVE_STATUS(gCurrentMove) && (gLastHitBy[gBattlerTarget] == 0xFF || gLastHitBy[gBattlerTarget] == gBattlerAttacker)
		&& gBattleStruct->moveTarget[gBattlerAttacker] == gBattlerTarget && gBattlerTarget != gBattlerAttacker && gCurrentTurnActionNumber == GetBattlerTurnOrderNum(gBattlerAttacker)
	    && (gChosenMove == gChosenMoveByBattler[gBattlerAttacker] || gChosenMove == gBattleMons[gBattlerAttacker].moves[gChosenMovePos]))
		return TRUE;
	return FALSE;
}

static bool8 NoTargetPresent(void)
{
	if (!IsBattlerAlive(gBattlerTarget))
		gBattlerTarget = GetMoveTarget(gCurrentMove, 0);
	
	switch (GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove))
	{
		case MOVE_TARGET_BOTH:
		    if (IsBattlerAlive(BATTLE_PARTNER(gBattlerTarget)))
				break;
			// Fallthrough
		case MOVE_TARGET_SELECTED:
		case MOVE_TARGET_DEPENDS:
		case MOVE_TARGET_RANDOM:
            if (!IsBattlerAlive(gBattlerTarget))
				return TRUE;
			break;
		case MOVE_TARGET_FOES_AND_ALLY:
            if (CountAliveMonsInBattle(gBattlerAttacker, BATTLE_ALIVE_EXCEPT_BATTLER) == 0)
				return TRUE;
			break;
	}
	return FALSE;
}

static bool8 AttacksThisTurn(u8 battlerId, u16 move)
{
	if (gBattleMoves[move].flags.twoTurnsMove)
	{
		if (gBattleMoves[move].effect == EFFECT_SOLARBEAM && IsBattlerWeatherAffected(battlerId, WEATHER_SUN_ANY))
			return TRUE;
		else if (gHitMarker & HITMARKER_CHARGING)
			return FALSE;
	}
	return TRUE;
}

static void atk00_attackcanceler(void)
{
	u8 i, moveTarget;
	u16 atkAbility;
	
	// Finished action
    if (gBattleOutcome)
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }
	// Attacker fainted
    if (!IsBattlerAlive(gBattlerAttacker) && !(gHitMarker & HITMARKER_NO_ATTACKSTRING))
    {
        gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
        gBattlescriptCurrInstr = BattleScript_MoveEnd;
        return;
    }
	// Check effects like flinch, freeze, etc.
    if (AtkCanceller_UnableToUseMove())
        return;
	
	atkAbility = GetBattlerAbility(gBattlerAttacker);
	
	// Check Parental Bond
	if (gSpecialStatuses[gBattlerAttacker].parentalBondState == PARENTAL_BOND_OFF && atkAbility == ABILITY_PARENTAL_BOND && IsBattlerAlive(gBattlerTarget)
	&& !IS_MOVE_STATUS(gCurrentMove) && gCurrentMove != MOVE_STRUGGLE && !gBattleMoves[gCurrentMove].flags.forbiddenParentalBond
    && !(gBattleMoves[gCurrentMove].flags.strikeCount > 1) && DoesSpreadMoveStrikesOnlyOnce(gBattlerAttacker, gBattlerTarget, gCurrentMove, FALSE))
	{
		gSpecialStatuses[gBattlerAttacker].parentalBondState = PARENTAL_BOND_1ST_HIT;
        gMultiHitCounter = 2;
		PrepareByteNumberBuffer(gBattleScripting.multihitString, 1, 0);
	}
	// Try Activate Protean
	if (gCurrentMove != MOVE_STRUGGLE && !IS_BATTLER_OF_TYPE(gBattlerAttacker, gBattleStruct->dynamicMoveType) && gDisableStructs[gBattlerAttacker].canProteanActivate
	&& (atkAbility == ABILITY_PROTEAN || atkAbility == ABILITY_LIBERO) && !gBattleMoves[gCurrentMove].flags.callOtherMove) // Wait the move call the other move to change the type based on it instead
	{
		gLastUsedAbility = atkAbility;
		gDisableStructs[gBattlerAttacker].canProteanActivate = FALSE;
		SetBattlerType(gBattlerAttacker, gBattleStruct->dynamicMoveType);
		BattleScriptCall(BattleScript_ProteanActivates);
        return;
	}
	// Check abilities that blocks the move
	if (AbilityBattleEffects(ABILITYEFFECT_MOVES_BLOCK, gBattlerTarget))
		return;
	
	// Check no PP for move
    if (!gBattleMons[gBattlerAttacker].pp[gCurrMovePos] && !(gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS) 
	&& gCurrentMove != MOVE_STRUGGLE && !(gHitMarker & (HITMARKER_ALLOW_NO_PP | HITMARKER_NO_ATTACKSTRING)))
    {
        gBattlescriptCurrInstr = BattleScript_NoPPForMove;
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        return;
    }
	gHitMarker &= ~(HITMARKER_ALLOW_NO_PP);
	
	// Try activate Stance Change
	if (atkAbility == ABILITY_STANCE_CHANGE && !gBattleMoves[gCurrentMove].flags.callOtherMove)
	{
		u16 newSpecies = TryDoBattleFormChange(gBattlerAttacker, FORM_CHANGE_MOVE);
		
		if (newSpecies)
			gBattleCommunication[MULTISTRING_CHOOSER] = 3;
		else
		{
			newSpecies = TryDoBattleFormChange(gBattlerAttacker, FORM_CHANGE_MOVE_SPLIT);
			
			if (newSpecies)
				gBattleCommunication[MULTISTRING_CHOOSER] = 2;
		}
		
		if (newSpecies)
		{
			gLastUsedAbility = atkAbility;
			DoBattleFormChange(gBattlerAttacker, newSpecies, FALSE, TRUE, FALSE);
			BattleScriptCall(BattleScript_AttackerFormChange);
			return;
		}
	}
	// Check if obeys
    if (!(gHitMarker & HITMARKER_OBEYS) && !(gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS))
    {
        switch (IsMonDisobedient())
        {
			case 0:
			    break;
			case 2:
			    gHitMarker |= HITMARKER_OBEYS;
				return;
			default:
			    gMoveResultFlags |= MOVE_RESULT_MISSED;
				return;
        }
    }
    gHitMarker |= HITMARKER_OBEYS;
	
	if (NoTargetPresent() && (!gBattleMoves[gCurrentMove].flags.twoTurnsMove || (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS)))
	{
		gBattlescriptCurrInstr = BattleScript_ButItFailedAtkStringPpReduce;
		if (!gBattleMoves[gCurrentMove].flags.twoTurnsMove || (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS))
            CancelMultiTurnMoves(gBattlerAttacker);
        return;
	}
	// Try Bounce the move back
	if (gBattleMoves[gCurrentMove].flags.magicCoatAffected && !gProtectStructs[gBattlerAttacker].usesBouncedMove)
	{
		bool8 effect = FALSE;
		
		if (gProtectStructs[gBattlerTarget].bounceMove)
		{
			gBattleScripting.bypassAbilityPopUp = TRUE;
			PressurePPLose(gBattlerAttacker, gBattlerTarget, MOVE_MAGIC_COAT);
			effect = TRUE;
		}
		else if (GetBattlerAbility(gBattlerTarget) == ABILITY_MAGIC_BOUNCE && !(gStatuses3[gBattlerTarget] & STATUS3_SEMI_INVULNERABLE))
		{
			RecordAbilityBattle(gBattlerTarget, GetBattlerAbility(gBattlerTarget));
			effect = TRUE;
		}
		
		if (effect)
		{
			gProtectStructs[gBattlerTarget].usesBouncedMove = TRUE;
			BattleScriptCall(BattleScript_MagicCoatBounce);
			return;
		}
	}
	moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove);
	
	// Check Prankster
	if (gProtectStructs[gBattlerAttacker].pranksterElevated && IS_BATTLER_OF_TYPE(gBattlerTarget, TYPE_DARK) && AttacksThisTurn(gBattlerAttacker, gCurrentMove)
		&& GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget) && !(moveTarget & MOVE_TARGET_OPPONENTS_FIELD)
	    && (moveTarget != MOVE_TARGET_ALL_BATTLERS || gCurrentMove == MOVE_PERISH_SONG || gCurrentMove == MOVE_ROTOTILLER))
	{
		if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE) || (moveTarget != MOVE_TARGET_BOTH && moveTarget != MOVE_TARGET_FOES_AND_ALLY))
			CancelMultiTurnMoves(gBattlerAttacker);
		gBattlescriptCurrInstr = BattleScript_DarkTypePreventsPrankster;
		return;
	}
	// Try Snatch move
    for (i = 0; i < gBattlersCount; ++i)
    {
        if (gBattleMoves[gCurrentMove].flags.snatchAffected && gProtectStructs[gBattlerByTurnOrder[i]].stealMove)
        {
			gProtectStructs[gBattlerByTurnOrder[i]].stealMove = FALSE;
            PressurePPLose(gBattlerAttacker, gBattlerByTurnOrder[i], MOVE_SNATCH);
            gBattleScripting.battler = gBattlerByTurnOrder[i];
			BattleScriptCall(BattleScript_SnatchedMove);
            return;
        }
    }
	// Check Lightning Rod/Storm Drain and Protect
    if (gSpecialStatuses[gBattlerTarget].lightningRodRedirected)
    {
        gSpecialStatuses[gBattlerTarget].lightningRodRedirected = FALSE;
        gLastUsedAbility = ABILITY_LIGHTNING_ROD;
        RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
		BattleScriptCall(BattleScript_TookAttack);
    }
    else if (gSpecialStatuses[gBattlerTarget].stormDrainRedirected)
    {
		gSpecialStatuses[gBattlerTarget].stormDrainRedirected = FALSE;
		gLastUsedAbility = ABILITY_STORM_DRAIN;
        RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
		BattleScriptCall(BattleScript_TookAttack);
    }
    else if (IsBattlerProtected(gBattlerAttacker, gBattlerTarget, gCurrentMove) && (gCurrentMove != MOVE_CURSE || IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_GHOST))
	&& ((!gBattleMoves[gCurrentMove].flags.twoTurnsMove || (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS))))
    {
        CancelMultiTurnMoves(gBattlerAttacker);
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gLastLandedMoves[gBattlerTarget] = 0;
        gLastHitByType[gBattlerTarget] = 0;
        gBattleCommunication[MISS_TYPE] = B_MSG_PROTECTED;
		
		if (gSpecialStatuses[gBattlerAttacker].parentalBondState == PARENTAL_BOND_1ST_HIT)
		{
			gSpecialStatuses[gBattlerAttacker].parentalBondState = PARENTAL_BOND_OFF; // No second hit if first hit was blocked
            gSpecialStatuses[gBattlerAttacker].multiHitOn = FALSE;
            gMultiHitCounter = 0;
		}
        ++gBattlescriptCurrInstr;
    }
    else
        ++gBattlescriptCurrInstr;
}

static void JumpIfMoveFailed(u8 adder)
{
    const u8 *BS_ptr = gBattlescriptCurrInstr + adder;

    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
    {
        gLastLandedMoves[gBattlerTarget] = 0;
        gLastHitByType[gBattlerTarget] = 0;
        BS_ptr = READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        TrySetDestinyBondToHappen();
		
        if (AbilityBattleEffects(ABILITYEFFECT_ABSORBING, gBattlerTarget))
            return;
    }
    gBattlescriptCurrInstr = BS_ptr;
}

static bool8 JumpIfMoveAffectedByProtect(void)
{
    if (IsBattlerProtected(gBattlerAttacker, gBattlerTarget, gCurrentMove))
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        JumpIfMoveFailed(6);
        gBattleCommunication[MISS_TYPE] = B_MSG_PROTECTED;
        return TRUE;
    }
    return FALSE;
}

static bool8 AccuracyCalcHelper(void)
{
    if (gStatuses3[gBattlerAttacker] & STATUS3_ALWAYS_HITS && gDisableStructs[gBattlerAttacker].battlerWithSureHit == gBattlerTarget) // Check Lock On
        JumpIfMoveFailed(6);
	else if (GetBattlerAbility(gBattlerAttacker) == ABILITY_NO_GUARD || GetBattlerAbility(gBattlerTarget) == ABILITY_NO_GUARD) // Check No Guard
		JumpIfMoveFailed(6);
	else if (IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_POISON) && gBattleMoves[gCurrentMove].effect == EFFECT_TOXIC)
		JumpIfMoveFailed(6);
	else if ((!gBattleMoves[gCurrentMove].flags.hitInAir && !gBattleMoves[gCurrentMove].flags.hitInAirDoubleDmg && gStatuses3[gBattlerTarget] & STATUS3_ON_AIR)
		|| (!gBattleMoves[gCurrentMove].flags.hitUnderground && gStatuses3[gBattlerTarget] & STATUS3_UNDERGROUND)
	    || (!gBattleMoves[gCurrentMove].flags.hitUnderwater && gStatuses3[gBattlerTarget] & STATUS3_UNDERWATER)) // Check if semi-invulnerable
	{
		gMoveResultFlags |= MOVE_RESULT_MISSED;
		JumpIfMoveFailed(6);
	}
	else if (IsBattlerWeatherAffected(gBattlerAttacker, WEATHER_RAIN_ANY) && gCurrentMove == MOVE_THUNDER) // Check Thunder on rain
		JumpIfMoveFailed(6);
	else if (IsBattlerWeatherAffected(gBattlerAttacker, WEATHER_HAIL_ANY) && gCurrentMove == MOVE_BLIZZARD) // Check Blizzard on hail
		JumpIfMoveFailed(6);
	else if (gStatuses3[gBattlerTarget] & STATUS3_MINIMIZED && gBattleMoves[gCurrentMove].flags.dmgMinimize) // Check never misses on minimize 
		JumpIfMoveFailed(6);
	else if (!gBattleMoves[gCurrentMove].accuracy) // Check moves that never misses
	    JumpIfMoveFailed(6);
	else
		return FALSE;
	
	return TRUE;
}

static u32 CalcMoveTotalAccuracy(u16 move, u8 attacker, u8 defender)
{
	u8 holdEffect, holdEffectParam, moveAcc = gBattleMoves[move].accuracy;
	u8 accuracyStatStages = gBattleMons[attacker].statStages[STAT_ACC], evasionStatStages = gBattleMons[defender].statStages[STAT_EVASION];
	u16 atkAbility = GetBattlerAbility(attacker), defAbility = GetBattlerAbility(defender);
	u32 totalAccuracy;
	
	// Check effects that ignore the user's accuracy stat stages
	if (defAbility == ABILITY_UNAWARE)
		accuracyStatStages = DEFAULT_STAT_STAGES; // Set accuracy stages to default
	
	// Check effects that ignore the target's evasion stat stages
	if (atkAbility == ABILITY_KEEN_EYE || atkAbility == ABILITY_UNAWARE || (gBattleMons[defender].status2 & STATUS2_FORESIGHT && evasionStatStages > DEFAULT_STAT_STAGES))
		evasionStatStages = DEFAULT_STAT_STAGES; // Set evasion stages to default
	else
		evasionStatStages = accuracyStatStages + DEFAULT_STAT_STAGES - evasionStatStages;
	
	evasionStatStages = min(MAX_STAT_STAGES, evasionStatStages); // Clamp evasion stages to the max
	
	// Check effects that alter the move's accuracy
	if ((IsBattlerWeatherAffected(attacker, WEATHER_SUN_ANY) && move == MOVE_THUNDER) || (defAbility == ABILITY_WONDER_SKIN
	&& IS_MOVE_STATUS(move) && moveAcc > 50))
		moveAcc = 50;
	
	// Apply evasion stat stages into the move's accuracy
	totalAccuracy = (sAccuracyStageRatios[evasionStatStages].dividend * moveAcc) / sAccuracyStageRatios[evasionStatStages].divisor;
	
	// Check effects that increases/decreases the move's total accuracy
	// Check attacker's ability
	switch (atkAbility)
	{
		case ABILITY_COMPOUND_EYES:
		    totalAccuracy = (totalAccuracy * 130) / 100; // 1.3 boost
			break;
		case ABILITY_VICTORY_STAR:
		    totalAccuracy = (totalAccuracy * 110) / 100; // 1.1 boost
			break;
		case ABILITY_HUSTLE:
		    if (IS_MOVE_PHYSICAL(move))
				totalAccuracy = (totalAccuracy * 80) / 100; // 1.2 loss
			break;
	}
	// Check attacker's ally ability
	if (IsBattlerAlive(BATTLE_PARTNER(attacker)))
	{
		switch (GetBattlerAbility(BATTLE_PARTNER(attacker)))
		{
			case ABILITY_VICTORY_STAR:
			    totalAccuracy = (totalAccuracy * 110) / 100; // 1.1 boost
				break;
		}
	}
	// Check defender's ability
	switch (defAbility)
	{
		case ABILITY_TANGLED_FEET:
		    if (gBattleMons[defender].status2 & STATUS2_CONFUSION)
				totalAccuracy /= 2; // 1/2 loss
			break;
		case ABILITY_SAND_VEIL:
		    if (IsBattlerWeatherAffected(defender, WEATHER_SANDSTORM_ANY))
				totalAccuracy = (totalAccuracy * 80) / 100; // 1.2 loss
			break;
		case ABILITY_SNOW_CLOAK:
		    if (IsBattlerWeatherAffected(defender, WEATHER_HAIL_ANY))
				totalAccuracy = (totalAccuracy * 80) / 100; // 1.2 loss
			break;
	}
	// Check defender's items
	holdEffect = GetBattlerItemHoldEffect(defender, TRUE);
	holdEffectParam = ItemId_GetHoldEffectParam(gBattleMons[defender].item);
	
	switch (holdEffect)
	{
		case HOLD_EFFECT_EVASION_UP:
		    totalAccuracy = (totalAccuracy * (100 - holdEffectParam)) / 100;
			break;
	}
	
	// General effects
	if (IsBattlerWeatherAffected(attacker, WEATHER_FOG_ANY))
		totalAccuracy = (totalAccuracy * 60) / 100; // 0.6 loss
	
	return totalAccuracy;
}

static void atk01_accuracycheck(void)
{
	u16 flags;
	u8 moveTarget;
	
	// Poke Dude and first battle moves never misses in certain states
    if ((gBattleTypeFlags & BATTLE_TYPE_POKEDUDE) || (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE && !BtlCtrl_OakOldMan_TestState2Flag(FIRST_BATTLE_MSG_FLAG_INFLICT_DMG) 
         && !IS_MOVE_STATUS(gCurrentMove) && GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER) || (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE 
         && !BtlCtrl_OakOldMan_TestState2Flag(FIRST_BATTLE_MSG_FLAG_STAT_CHG) && IS_MOVE_STATUS(gCurrentMove) && GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER))
        JumpIfMoveFailed(6);
    else if (gBattlescriptCurrInstr[5] == NO_ACC_CALC_CHECK_LOCK_ON)
    {
        if (gStatuses3[gBattlerAttacker] & STATUS3_ALWAYS_HITS && gDisableStructs[gBattlerAttacker].battlerWithSureHit == gBattlerTarget)
            gBattlescriptCurrInstr += 6; // Only checks for Lock On, no acc calc
        else if (gStatuses3[gBattlerTarget] & STATUS3_SEMI_INVULNERABLE) // Check semi-invulnerable
            gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
        else if (!JumpIfMoveAffectedByProtect()) // Check Protect
            gBattlescriptCurrInstr += 6;
    }
	else if (gSpecialStatuses[gBattlerAttacker].parentalBondState == PARENTAL_BOND_2ND_HIT || (gSpecialStatuses[gBattlerAttacker].multiHitOn
	&& (gBattleMoves[gCurrentMove].effect != EFFECT_TRIPLE_KICK || GetBattlerAbility(gBattlerAttacker) == ABILITY_SKILL_LINK))) // Check Skill Link on multi-hit moves
		gBattlescriptCurrInstr += 6;
	else if (JumpIfMoveAffectedByProtect() || AccuracyCalcHelper()) // Check Protect and effects that cause the move to aways hit
	    return;
    else
    {
		if ((Random() % 100 + 1) > CalcMoveTotalAccuracy(gCurrentMove, gBattlerAttacker, gBattlerTarget)) // final calculation, determines if the move misses
		{
			gMoveResultFlags |= MOVE_RESULT_MISSED;
			
			moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove);
			
			if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && (moveTarget == MOVE_TARGET_BOTH || moveTarget == MOVE_TARGET_FOES_AND_ALLY))
				gBattleCommunication[MISS_TYPE] = B_MSG_AVOIDED_ATK;
			else
				gBattleCommunication[MISS_TYPE] = B_MSG_MISSED;
			
			if (!IS_MOVE_STATUS(gCurrentMove) && CalcTypeEffectivenessMultiplier(gCurrentMove, gBattleStruct->dynamicMoveType, gBattlerAttacker, gBattlerTarget, TRUE, &flags) == TYPE_MUL_NO_EFFECT)
			{
				gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
				gProtectStructs[gBattlerAttacker].targetNotAffected = 1;
			}
		}
		JumpIfMoveFailed(6);
    }
}

static void atk02_attackstring(void)
{
    if (!gBattleControllerExecFlags)
    {
        if (!(gHitMarker & (HITMARKER_NO_ATTACKSTRING | HITMARKER_ATTACKSTRING_PRINTED)))
        {
            PrepareStringBattle(STRINGID_USEDMOVE, gBattlerAttacker);
            gHitMarker |= HITMARKER_ATTACKSTRING_PRINTED;
        }
        gBattleCommunication[MSG_DISPLAY] = FALSE;
		++gBattlescriptCurrInstr;
    }
}

 static void atk03_ppreduce(void)
{
    u8 i, ppToDeduct, atkSide;

    if (!gBattleControllerExecFlags)
    {
		atkSide = GetBattlerSide(gBattlerAttacker);
		ppToDeduct = 1;
		
		// Check Pressure PP usage
        if (!gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure)
        {
            switch (GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove))
            {
				case MOVE_TARGET_OPPONENTS_FIELD:
				    if (gCurrentMove == MOVE_STICKY_WEB) // Can't increases Sticky Web PP usage
						break;
					// fallthrough
				case MOVE_TARGET_BOTH:
				case MOVE_TARGET_FOES_AND_ALLY:
					for (i = 0; i < gBattlersCount; i++)
					{
						if (atkSide != GetBattlerSide(i) && IsBattlerAlive(i)) // Can't affect ally's moves
							ppToDeduct += (GetBattlerAbility(i) == ABILITY_PRESSURE);
					}
					break;
				default:
					if (atkSide != GetBattlerSide(gBattlerTarget) && GetBattlerAbility(gBattlerTarget) == ABILITY_PRESSURE)
						++ppToDeduct;
					break;
            }
        }
		// Decreases the PP
        if (!(gHitMarker & (HITMARKER_NO_PPDEDUCT | HITMARKER_NO_ATTACKSTRING)) && gBattleMons[gBattlerAttacker].pp[gCurrMovePos])
        {
            if (gBattleMons[gBattlerAttacker].pp[gCurrMovePos] > ppToDeduct)
                gBattleMons[gBattlerAttacker].pp[gCurrMovePos] -= ppToDeduct;
            else
                gBattleMons[gBattlerAttacker].pp[gCurrMovePos] = 0;
			
			gProtectStructs[gBattlerAttacker].notFirstStrike = 1;
		
            if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_TRANSFORMED) && !((gDisableStructs[gBattlerAttacker].mimickedMoves) & gBitTable[gCurrMovePos]))
            {
                BtlController_EmitSetMonData(gBattlerAttacker, BUFFER_A, REQUEST_PPMOVE1_BATTLE + gCurrMovePos, 0, 1, &gBattleMons[gBattlerAttacker].pp[gCurrMovePos]);
                MarkBattlerForControllerExec(gBattlerAttacker);
            }
        }
        gHitMarker &= ~(HITMARKER_NO_PPDEDUCT);
        ++gBattlescriptCurrInstr;
    }
}

static void atk04_critcalc(void)
{
	s16 critChance = CalcMoveCritChance(gBattlerAttacker, gBattlerTarget, gCurrentMove);
	
	if ((gBattleTypeFlags & (BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_POKEDUDE))
		|| (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE && !BtlCtrl_OakOldMan_TestState2Flag(FIRST_BATTLE_MSG_FLAG_INFLICT_DMG)) || critChance == -1)
		gIsCriticalHit = FALSE;
	else if (critChance == -2 || !(Random() % sCriticalHitChance[critChance]))
	{
		gIsCriticalHit = TRUE;
		
		// counter for EVO_CRITICAL_HITS
		if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER && gPartyCriticalHits[gBattlerPartyIndexes[gBattlerAttacker]] < 255)
			++gPartyCriticalHits[gBattlerPartyIndexes[gBattlerAttacker]];
		// Signal for trainer slide
		if (GetBattlerSide(gBattlerTarget) == B_SIDE_OPPONENT && gBattleStruct->firstCritcalHitTakenMsgState == 0)
			++gBattleStruct->firstCritcalHitTakenMsgState;
	}
	else
		gIsCriticalHit = FALSE;
	
	++gBattlescriptCurrInstr;
}

s16 CalcMoveCritChance(u8 battlerAtk, u8 battlerDef, u16 move)
{
	s16 critChance;
	u8 holdEffect = GetBattlerItemHoldEffect(battlerAtk, TRUE);
	u16 atkAbility = GetBattlerAbility(battlerAtk), defAbility = GetBattlerAbility(battlerDef);
	
	// Check effects that blocks critical hits
	if (defAbility == ABILITY_BATTLE_ARMOR || defAbility == ABILITY_SHELL_ARMOR)
		critChance = -1;
	else if (atkAbility == ABILITY_MERCILESS && gBattleMons[battlerDef].status1 & STATUS1_PSN_ANY) // Check effects that cause the moves to aways be critical
		critChance = -2;
	else // Calc move crit chance
	{
		critChance = 2 * ((gBattleMons[battlerAtk].status2 & STATUS2_FOCUS_ENERGY) == TRUE) // Focus Energy
		             + (atkAbility == ABILITY_SUPER_LUCK) // Super Luck
                     + (gBattleMoves[move].flags.highCritChance == TRUE) // High Crit chance move
                     + (holdEffect == HOLD_EFFECT_SCOPE_LENS) // Scope Lens
                     + 2 * (holdEffect == HOLD_EFFECT_LUCKY_PUNCH && SpeciesToNationalPokedexNum(gBattleMons[battlerAtk].species) == NATIONAL_DEX_CHANSEY) // Lucky Punch
                     + 2 * (holdEffect == HOLD_EFFECT_STICK && (SpeciesToNationalPokedexNum(gBattleMons[battlerAtk].species) == NATIONAL_DEX_FARFETCHD
					 || SpeciesToNationalPokedexNum(gBattleMons[battlerAtk].species) == NATIONAL_DEX_SIRFETCHD)); // Stick
					 
		if (critChance >= ARRAY_COUNT(sCriticalHitChance))
			critChance = ARRAY_COUNT(sCriticalHitChance) - 1; // Crit chance can't be higer than table's lenght
	}
	return critChance;
}

static void atk05_damagecalc(void)
{
    gBattleMoveDamage = CalculateMoveDamage(gCurrentMove, gBattleStruct->dynamicMoveType, gBattlerAttacker, gBattlerTarget, gIsCriticalHit, TRUE);
    ++gBattlescriptCurrInstr;
}

void AI_CalcDmg(u8 attacker, u8 defender, u16 move)
{
    u8 moveType = gBattleMoves[move].type;
    gBattleScripting.dmgMultiplier = 1;
    gBattleMoveDamage = CalculateMoveDamage(move, moveType, attacker, defender, FALSE, FALSE);
	CalcTypeEffectivenessMultiplier(move, moveType, attacker, defender, FALSE, &gMoveResultFlags);
}

static void atk06_typecalc(void)
{
	// Calculate type effectiveness
	CalcTypeEffectivenessMultiplier(gCurrentMove, gBattleStruct->dynamicMoveType, gBattlerAttacker, gBattlerTarget, TRUE, &gMoveResultFlags);
	
	if (gMoveResultFlags & MOVE_RESULT_MISSED)
	{
		gLastLandedMoves[gBattlerTarget] = 0;
		gLastHitByType[gBattlerTarget] = 0;
	}
	
	if (gMoveResultFlags & MOVE_RESULT_DOESNT_AFFECT_FOE)
		gProtectStructs[gBattlerAttacker].targetNotAffected = 1;
	
    ++gBattlescriptCurrInstr;
}

static void UpdateMoveResults(u8 *multiplier, u8 mod, u16 move, u16 *flags) // Update effectiveness multiplier for the type calc
{
    switch (mod)
    {
		case TYPE_MUL_NO_EFFECT:
		    *multiplier = TYPE_MUL_NO_EFFECT;
		    *flags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
			*flags &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE;
			*flags &= ~MOVE_RESULT_SUPER_EFFECTIVE;
			break;
		case TYPE_MUL_NOT_EFFECTIVE:
		    if (!IS_MOVE_STATUS(move) && !(*flags & MOVE_RESULT_NO_EFFECT))
			{
				if (*flags & MOVE_RESULT_SUPER_EFFECTIVE)
					*flags &= ~MOVE_RESULT_SUPER_EFFECTIVE;
				else
					*flags |= MOVE_RESULT_NOT_VERY_EFFECTIVE;
				
				*multiplier = TYPE_MUL_NOT_EFFECTIVE;
			}
			break;
		case TYPE_MUL_SUPER_EFFECTIVE:
		    if (!IS_MOVE_STATUS(move) && !(*flags & MOVE_RESULT_NO_EFFECT))
			{
				if (*flags & MOVE_RESULT_NOT_VERY_EFFECTIVE)
					*flags &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE;
				else
					*flags |= MOVE_RESULT_SUPER_EFFECTIVE;
				
				*multiplier = TYPE_MUL_SUPER_EFFECTIVE;
			}
			break;
    }
}

u8 GetTypeModifier(u8 atkType, u8 defType)
{
	return gTypeEffectiveness[atkType][defType]; // Get effectiveness betwen the move's type and the defender's type
}

static void MulByTypeEffectiveness(u16 move, u8 moveType, u8 attacker, u8 defender, u8 defenderType, u8 *multiplier, u16 *flags)
{
	u8 mod = GetTypeModifier(moveType, defenderType);
	
	// Check Foresight and Scrappy on Ghost types
	if ((moveType == TYPE_FIGHTING || moveType == TYPE_NORMAL) && defenderType == TYPE_GHOST && mod == TYPE_MUL_NO_EFFECT
	&& (gBattleMons[defender].status2 & STATUS2_FORESIGHT || GetBattlerAbility(attacker) == ABILITY_SCRAPPY || move == MOVE_GLARE))
	    mod = TYPE_MUL_NORMAL;
	
	// Check ground immunities
	if (moveType == TYPE_GROUND && mod == TYPE_MUL_NO_EFFECT && IsBattlerGrounded(defender))
		mod = TYPE_MUL_NORMAL;
	
	// Check strong winds
	if (IsBattlerWeatherAffected(defender, WEATHER_STRONG_WINDS) && defenderType == TYPE_FLYING && mod == TYPE_MUL_SUPER_EFFECTIVE)
		mod = TYPE_MUL_NORMAL;
	
	// Check moves that dont display "super effective" or "not very effective" messages
	if (gBattleMoves[move].flags.noEffectiveness && (mod == TYPE_MUL_SUPER_EFFECTIVE || mod == TYPE_MUL_NOT_EFFECTIVE))
		mod = TYPE_MUL_NORMAL;
	
	UpdateMoveResults(multiplier, mod, move, flags);
}

static u8 CalcTypeEffectivenessMultiplierInternal(u16 move, u8 moveType, u8 attacker, u8 defender, u8 multiplier, bool8 recordAbilities, u16 *flags)
{
	u16 defAbility;
	
	MulByTypeEffectiveness(move, moveType, attacker, defender, gBattleMons[defender].type1, &multiplier, flags);
	
	if (gBattleMons[defender].type1 != gBattleMons[defender].type2)
		MulByTypeEffectiveness(move, moveType, attacker, defender, gBattleMons[defender].type2, &multiplier, flags);
	
	defAbility = GetBattlerAbility(defender);
	
	if (IS_MOVE_STATUS(move) && move != MOVE_THUNDER_WAVE)
		multiplier = TYPE_MUL_NORMAL;
	else if (moveType == TYPE_GROUND && !IsBattlerGrounded(defender) && defAbility == ABILITY_LEVITATE)
	{
		multiplier = TYPE_MUL_NO_EFFECT;
		*flags |= (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE);
		
		if (recordAbilities)
		{
			gLastUsedAbility = defAbility;
			gBattleCommunication[MISS_TYPE] = B_MSG_ABILITY_AVOID;
			RecordAbilityBattle(defender, gLastUsedAbility);
		}
	}
	
	if (!IS_MOVE_STATUS(move) && AttacksThisTurn(attacker, move) && defAbility == ABILITY_WONDER_GUARD && multiplier != TYPE_MUL_SUPER_EFFECTIVE)
	{
		multiplier = TYPE_MUL_NO_EFFECT;
		*flags |= MOVE_RESULT_MISSED;
		
		if (recordAbilities)
		{
			gLastUsedAbility = defAbility;
			gBattleCommunication[MISS_TYPE] = B_MSG_ABILITY_AVOID;
			RecordAbilityBattle(defender, gLastUsedAbility);
		}
	}
	return multiplier;
}

u8 CalcTypeEffectivenessMultiplier(u16 move, u8 moveType, u8 attacker, u8 defender, bool8 recordAbilities, u16 *flags)
{
	u8 multiplier = TYPE_MUL_NORMAL;
	
	if (move != MOVE_STRUGGLE && moveType != TYPE_MYSTERY)
	{
		multiplier = CalcTypeEffectivenessMultiplierInternal(move, moveType, attacker, defender, multiplier, recordAbilities, flags);
	}
	return multiplier;
}

u16 AI_TypeCalc(u16 move, u16 targetSpecies, u16 targetAbility)
{
    u8 moveType = gBattleMoves[move].type, multiplier;
	u16 flags = 0;

    if (move != MOVE_STRUGGLE)
	{
		MulByTypeEffectiveness(move, moveType, 0, 0, gBaseStats[targetSpecies].type1, &multiplier, &flags);
		
		if (gBaseStats[targetSpecies].type1 != gBaseStats[targetSpecies].type2)
			MulByTypeEffectiveness(move, moveType, 0, 0, gBaseStats[targetSpecies].type2, &multiplier, &flags);
		
		if (moveType == TYPE_GROUND && targetAbility == ABILITY_LEVITATE)
			flags |= (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE);
		if (targetAbility == ABILITY_WONDER_GUARD && gBattleMoves[move].power)
			flags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
	}
	return flags;
}

bool8 SubsBlockMove(u8 attacker, u8 defender, u16 move) // Check if substitute can block the move
{
	if ((!(gBattleMons[defender].status2 & STATUS2_SUBSTITUTE) || gBattleMoves[move].flags.soundMove || (GetBattlerAbility(attacker) == ABILITY_INFILTRATOR
	&& gBattleMoves[move].effect != EFFECT_TRANSFORM && gBattleMoves[move].effect != EFFECT_SKY_DROP)))
		return FALSE;
	return TRUE;
}

static void atk07_adjustdamage(void) // Check for effects that prevent the target from faint
{
    u8 holdEffect = GetBattlerItemHoldEffect(gBattlerTarget, TRUE), param = ItemId_GetHoldEffectParam(gBattleMons[gBattlerTarget].item);
	
	// Check Focus Band
    if (holdEffect == HOLD_EFFECT_FOCUS_BAND && (Random() % 100) < param)
    {
        RecordItemEffectBattle(gBattlerTarget, holdEffect);
        gSpecialStatuses[gBattlerTarget].focusBanded = 1;
    }
	// Effect that cause the target to survive with 1 HP
    if (gBattleMons[gBattlerTarget].hp <= gBattleMoveDamage && !SubsBlockMove(gBattlerAttacker, gBattlerTarget, gCurrentMove)
	&& ((gBattleMoves[gCurrentMove].effect == EFFECT_FALSE_SWIPE || gProtectStructs[gBattlerTarget].endured
	|| gSpecialStatuses[gBattlerTarget].focusBanded) || (GetBattlerAbility(gBattlerTarget) == ABILITY_STURDY
	&& BATTLER_MAX_HP(gBattlerTarget))))
    {
        gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - 1; // Survive with 1 HP
	    
        if (gProtectStructs[gBattlerTarget].endured)
            gMoveResultFlags |= MOVE_RESULT_FOE_ENDURED;
        else if (gSpecialStatuses[gBattlerTarget].focusBanded)
        {
            gMoveResultFlags |= MOVE_RESULT_FOE_HUNG_ON;
            gLastUsedItem = gBattleMons[gBattlerTarget].item;
        }
		else if (GetBattlerAbility(gBattlerTarget) == ABILITY_STURDY)
		{
			gLastUsedAbility = ABILITY_STURDY;
			RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
			gSpecialStatuses[gBattlerTarget].sturdied = 1;
			gMoveResultFlags |= MOVE_RESULT_FOE_STURDIED;
		}
    }
    ++gBattlescriptCurrInstr;
}

static void atk08_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk09_attackanimation(void)
{
	u8 moveTarget;
	
    if (!gBattleControllerExecFlags)
    {
        if (!IsBattleAnimationsOn() && (gBattleMoves[gCurrentMove].effect != EFFECT_TRANSFORM && gBattleMoves[gCurrentMove].effect != EFFECT_SUBSTITUTE))
        {
            BattleScriptPush(gBattlescriptCurrInstr + 1);
            gBattlescriptCurrInstr = BattleScript_Pausex20;
            ++gBattleScripting.animTurn;
            ++gBattleScripting.animTargetsHit;
        }
        else
        {
			moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove);
			
            if (((moveTarget == MOVE_TARGET_BOTH || moveTarget == MOVE_TARGET_FOES_AND_ALLY || moveTarget == MOVE_TARGET_DEPENDS)
				&& gBattleScripting.animTargetsHit) || gSpecialStatuses[gBattlerAttacker].parentalBondState == PARENTAL_BOND_2ND_HIT)
            {
                ++gBattlescriptCurrInstr;
                return;
            }
            if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
            {
                BtlController_EmitMoveAnimation(gBattlerAttacker, BUFFER_A, gCurrentMove, gBattleScripting.animTurn, gBattleMovePower, gBattleMoveDamage, gBattleMons[gBattlerAttacker].friendship, &gDisableStructs[gBattlerAttacker]);
                ++gBattleScripting.animTurn;
                ++gBattleScripting.animTargetsHit;
                MarkBattlerForControllerExec(gBattlerAttacker);
                ++gBattlescriptCurrInstr;
            }
            else
            {
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_Pausex20;
            }
        }
    }
}

static void atk0A_nop(void)
{
	++gBattlescriptCurrInstr;
}

static bool8 CanBattlerIceFaceBlockMove(u8 battlerId, u16 move)
{
	if (!(gHitMarker & HITMARKER_IGNORE_SUBSTITUTE) && !(gBattleMons[battlerId].status2 & STATUS2_TRANSFORMED)
		&& gBattleMons[battlerId].species == SPECIES_EISCUE && IS_MOVE_PHYSICAL(move) && GetBattlerAbility(battlerId) == ABILITY_ICE_FACE)
	    return TRUE;
	return FALSE;
}

static bool8 BattlerIsDisguised(u8 battlerId)
{
	if (!(gHitMarker & HITMARKER_IGNORE_DISGUISE) && !(gBattleMons[battlerId].status2 & STATUS2_TRANSFORMED)
		&& gBattleMons[battlerId].species == SPECIES_MIMIKYU && GetBattlerAbility(battlerId) == ABILITY_DISGUISE)
		return TRUE;
	return FALSE;
}

static void atk0B_healthbarupdate(void)
{
	u8 battlerId;
    s16 healthValue;
    s32 currDmg, maxPossibleDmgValue;
	
    if (!gBattleControllerExecFlags)
    {
        if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
        {
            battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
			
			// Check effects that blocks the move
			if (SubsBlockMove(gBattlerAttacker, battlerId, gCurrentMove) && gDisableStructs[battlerId].substituteHP
			&& !(gHitMarker & HITMARKER_IGNORE_SUBSTITUTE))
				PrepareStringBattle(STRINGID_SUBSTITUTEDAMAGED, battlerId); // Hit the substitute
			else if (CanBattlerIceFaceBlockMove(battlerId, gCurrentMove) || BattlerIsDisguised(battlerId)) // Skip the health bar update
			{
				gBattlescriptCurrInstr += 2;
				return;
			}
            else
            {
                currDmg = gBattleMoveDamage;
                maxPossibleDmgValue = 10000; // Not present in R/S, ensures that huge damage values don't change sign
                
				// currDmg can't be higher than maxPossibleDmgValue
                if (currDmg <= maxPossibleDmgValue)
                    healthValue = currDmg;
                else
                    healthValue = maxPossibleDmgValue;
		    
                BtlController_EmitHealthBarUpdate(battlerId, BUFFER_A, healthValue);
                MarkBattlerForControllerExec(battlerId);
            }
        }
        gBattlescriptCurrInstr += 2;
    }
}

static void UpdateDamageVars(u8 battlerId, u8 didDamageBattlerId)
{
	if (IS_MOVE_PHYSICAL(gCurrentMove))
	{
		gProtectStructs[battlerId].physicalDmg = gHpDealt;
		gSpecialStatuses[battlerId].physicalDmg = gHpDealt;
		gProtectStructs[battlerId].physicalBattlerId = didDamageBattlerId;
		gSpecialStatuses[battlerId].physicalBattlerId = didDamageBattlerId;
	}
	else if (IS_MOVE_SPECIAL(gCurrentMove))
	{
		gProtectStructs[battlerId].specialDmg = gHpDealt;
		gSpecialStatuses[battlerId].specialDmg = gHpDealt;
		gProtectStructs[battlerId].specialBattlerId = didDamageBattlerId;
		gSpecialStatuses[battlerId].specialBattlerId = didDamageBattlerId;
	}
}

static void atk0C_datahpupdate(void)
{
    if (!gBattleControllerExecFlags)
    {
		u8 moveSplit, battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	    
        if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
        {
			moveSplit = GetMoveSplit(gCurrentMove);
			
			if (SubsBlockMove(gBattlerAttacker, battlerId, gCurrentMove) && gDisableStructs[battlerId].substituteHP
			&& !(gHitMarker & HITMARKER_IGNORE_SUBSTITUTE)) // Check Substitute
			{
				if (gDisableStructs[battlerId].substituteHP >= gBattleMoveDamage)
				{
					if (gSpecialStatuses[battlerId].dmg == 0)
						gSpecialStatuses[battlerId].dmg = gBattleMoveDamage;
					gDisableStructs[battlerId].substituteHP -= gBattleMoveDamage;
					gHpDealt = gBattleMoveDamage;
				}
				else
				{
					if (gSpecialStatuses[battlerId].dmg == 0)
						gSpecialStatuses[battlerId].dmg = gDisableStructs[battlerId].substituteHP;
					gHpDealt = gDisableStructs[battlerId].substituteHP;
					gDisableStructs[battlerId].substituteHP = 0;
				}
				// Check substitute fading
				if (gDisableStructs[battlerId].substituteHP == 0)
				{
					gBattlescriptCurrInstr += 2;
					BattleScriptCall(BattleScript_SubstituteFade);
					return;
				}
			}
			else if (BattlerIsDisguised(battlerId)) // Check Disguise
			{
				gBattleMoveDamage = gBattleMons[battlerId].maxHP / 8;
				if (gBattleMoveDamage == 0)
					gBattleMoveDamage = 1;
				gHpDealt = gBattleMoveDamage;
				if (gSpecialStatuses[battlerId].dmg == 0)
					gSpecialStatuses[battlerId].dmg = 1;
				gMoveResultFlags = 0;
				
				UpdateDamageVars(battlerId, gBattlerAttacker);
				gBattleScripting.battler = battlerId;
				gLastUsedAbility = ABILITY_DISGUISE;
				DoBattleFormChange(battlerId, SPECIES_MIMIKYU_BUSTED, FALSE, FALSE, FALSE);
				gBattlescriptCurrInstr += 2;
				BattleScriptCall(BattleScript_DisguiseBusted);
				return;
			}
			else if (CanBattlerIceFaceBlockMove(battlerId, gCurrentMove)) // Check Ice Face
			{
				if (gSpecialStatuses[battlerId].dmg == 0)
					gSpecialStatuses[battlerId].dmg = 1;
				gHpDealt = 1;
				gMoveResultFlags = 0;
				UpdateDamageVars(battlerId, gBattlerAttacker);
				gBattleScripting.battler = battlerId;
				gLastUsedAbility = ABILITY_ICE_FACE;
				DoBattleFormChange(battlerId, SPECIES_EISCUE_NOICE_FACE, FALSE, TRUE, FALSE);
				gBattlescriptCurrInstr += 2;
				BattleScriptCall(BattleScript_IceFaceFade);
				return;
			}
            else
            {
                gHitMarker &= ~(HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_IGNORE_DISGUISE);
				
                if (gBattleMoveDamage < 0) // HP goes up
                {
                    gBattleMons[battlerId].hp -= gBattleMoveDamage;
                    if (gBattleMons[battlerId].hp > gBattleMons[battlerId].maxHP)
                        gBattleMons[battlerId].hp = gBattleMons[battlerId].maxHP;
                }
                else // HP goes down
                {
					if (gHitMarker & HITMARKER_SKIP_DMG_TRACK)
                        gHitMarker &= ~(HITMARKER_SKIP_DMG_TRACK);
                    else
                    {
                        gTakenDmg[battlerId] += gBattleMoveDamage;
                        if (gBattlescriptCurrInstr[1] == BS_TARGET)
                            gTakenDmgByBattler[battlerId] = gBattlerAttacker;
                        else
                            gTakenDmgByBattler[battlerId] = gBattlerTarget;
                    }

                    if (gBattleMons[battlerId].hp > gBattleMoveDamage)
                    {
                        gBattleMons[battlerId].hp -= gBattleMoveDamage;
                        gHpDealt = gBattleMoveDamage;
						
						if (gDisableStructs[battlerId].enduredHit && !gHpDealt)
						{
							gDisableStructs[battlerId].enduredHit = FALSE;
							gHpDealt = 1;
						}
                    }
                    else
                    {
                        gHpDealt = gBattleMons[battlerId].hp;
                        gBattleMons[battlerId].hp = 0;
                    }
					
					if (!(gHitMarker & HITMARKER_PASSIVE_DAMAGE))
					{
						if (!gSpecialStatuses[battlerId].dmg)
							gSpecialStatuses[battlerId].dmg = gHpDealt;
						
						UpdateDamageVars(battlerId, gBattlescriptCurrInstr[1] == BS_TARGET ? gBattlerAttacker : gBattlerTarget);
					}
                }
                gHitMarker &= ~(HITMARKER_PASSIVE_DAMAGE);
                BtlController_EmitSetMonData(battlerId, BUFFER_A, REQUEST_HP_BATTLE, 0, 2, &gBattleMons[battlerId].hp);
                MarkBattlerForControllerExec(battlerId);
            }
        }
        else
        {
            if (gSpecialStatuses[battlerId].dmg == 0)
                gSpecialStatuses[battlerId].dmg = 0xFFFF;
        }
        gBattlescriptCurrInstr += 2;
    }
}

static void atk0D_critmessage(void)
{
    if (!gBattleControllerExecFlags)
    {
        if (gIsCriticalHit && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
        {
            PrepareStringBattle(DoesSpreadMoveStrikesOnlyOnce(gBattlerAttacker, gBattlerTarget, gCurrentMove, TRUE) ? STRINGID_CRITICALHIT : STRINGID_CRITICALHITONDEF, gBattlerAttacker);
            gBattleCommunication[MSG_DISPLAY] = TRUE;
        }
        ++gBattlescriptCurrInstr;
    }
}

static void atk0E_effectivenesssound(void)
{
    if (!gBattleControllerExecFlags)
    {
        if (!(gMoveResultFlags & MOVE_RESULT_MISSED))
        {
            switch (gMoveResultFlags & ~(MOVE_RESULT_MISSED))
            {
				case MOVE_RESULT_SUPER_EFFECTIVE:
				    BtlController_EmitPlaySE(gBattlerTarget, BUFFER_A, SE_SUPER_EFFECTIVE);
					MarkBattlerForControllerExec(gBattlerTarget);
					break;
				case MOVE_RESULT_NOT_VERY_EFFECTIVE:
                    BtlController_EmitPlaySE(gBattlerTarget, BUFFER_A, SE_NOT_EFFECTIVE);
					MarkBattlerForControllerExec(gBattlerTarget);
					break;
				case MOVE_RESULT_DOESNT_AFFECT_FOE:
				case MOVE_RESULT_FAILED: // no sound
                    break;
				default:
				    if (gMoveResultFlags & MOVE_RESULT_SUPER_EFFECTIVE)
					{
						BtlController_EmitPlaySE(gBattlerTarget, BUFFER_A, SE_SUPER_EFFECTIVE);
						MarkBattlerForControllerExec(gBattlerTarget);
					}
					else if (gMoveResultFlags & MOVE_RESULT_NOT_VERY_EFFECTIVE)
					{
						BtlController_EmitPlaySE(gBattlerTarget, BUFFER_A, SE_NOT_EFFECTIVE);
						MarkBattlerForControllerExec(gBattlerTarget);
					}
					else if (!(gMoveResultFlags & (MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_FAILED)))
					{
						BtlController_EmitPlaySE(gBattlerTarget, BUFFER_A, SE_EFFECTIVE);
						MarkBattlerForControllerExec(gBattlerTarget);
					}
					break;
            }
        }
        ++gBattlescriptCurrInstr;
    }
}

static void atk0F_resultmessage(void)
{
    u16 stringId;

    if (!gBattleControllerExecFlags)
    {
		stringId = 0;
		
		gBattleCommunication[MSG_DISPLAY] = TRUE;
		
        if (gMoveResultFlags & MOVE_RESULT_MISSED && (!(gMoveResultFlags & MOVE_RESULT_DOESNT_AFFECT_FOE) || gBattleCommunication[MISS_TYPE] > B_MSG_AVOIDED_ATK))
		{
			if (gBattleCommunication[MISS_TYPE] == B_MSG_ABILITY_AVOID) // Levitate/Wonder Guard
			{
				BattleScriptPush(gBattlescriptCurrInstr + 1);
				gBattlescriptCurrInstr = BattleScript_AvoidMoveWithAbility;
				gBattleCommunication[MSG_DISPLAY] = FALSE;
				return;
			}
			else
				stringId = gMissStringIds[gBattleCommunication[MISS_TYPE]];
		}
        else
        {
            switch (gMoveResultFlags & ~(MOVE_RESULT_MISSED))
            {
				case MOVE_RESULT_SUPER_EFFECTIVE:
				    if (!gMultiHitCounter) // Don't print effectiveness on each hit in a multi hit attack
					{
						stringId = DoesSpreadMoveStrikesOnlyOnce(gBattlerAttacker, gBattlerTarget, gCurrentMove, TRUE) ? STRINGID_SUPEREFFECTIVE : STRINGID_SUPEREFFECTIVEONDEF;
						
						// Signal for trainer slide
						if (GetBattlerSide(gBattlerTarget) != B_SIDE_PLAYER && gBattleStruct->firstSuperEffectiveHitTakenMsgState == 0)
							++gBattleStruct->firstSuperEffectiveHitTakenMsgState;
					}
					break;
				case MOVE_RESULT_NOT_VERY_EFFECTIVE:
				    if (!gMultiHitCounter)
						stringId = DoesSpreadMoveStrikesOnlyOnce(gBattlerAttacker, gBattlerTarget, gCurrentMove, TRUE) ? STRINGID_NOTVERYEFFECTIVE : STRINGID_NOTVERYEFFECTIVEONDEF;
					break;
				case MOVE_RESULT_ONE_HIT_KO:
				    stringId = STRINGID_ONEHITKO;
					break;
				case MOVE_RESULT_FOE_ENDURED:
				    gDisableStructs[gBattlerTarget].enduredHit = TRUE; // So effects that require damage can activate
				    stringId = STRINGID_DEFENDUREDHIT;
					break;
				case MOVE_RESULT_FAILED:
				    stringId = STRINGID_BUTITFAILED;
					break;
				case MOVE_RESULT_DOESNT_AFFECT_FOE:
				    stringId = STRINGID_ITDOESNTAFFECT;
					break;
				case MOVE_RESULT_FOE_HUNG_ON:
				    gLastUsedItem = gBattleMons[gBattlerTarget].item;
					gMoveResultFlags &= ~(MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
					BattleScriptCall(BattleScript_HangedOnMsg);
					return;
				default:
				    if (gMoveResultFlags & MOVE_RESULT_DOESNT_AFFECT_FOE)
						stringId = STRINGID_ITDOESNTAFFECT;
					else if (gMoveResultFlags & MOVE_RESULT_ONE_HIT_KO)
					{
						gMoveResultFlags &= ~(MOVE_RESULT_ONE_HIT_KO | MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE);
						BattleScriptCall(BattleScript_OneHitKOMsg);
						return;
					}
					else if (gMoveResultFlags & MOVE_RESULT_FOE_STURDIED)
					{
						gMoveResultFlags &= ~(MOVE_RESULT_FOE_STURDIED | MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
						gSpecialStatuses[gBattlerTarget].sturdied = FALSE;
						BattleScriptCall(BattleScript_SturdiedMsg);
						return;
					}
					else if (gMoveResultFlags & MOVE_RESULT_FOE_ENDURED)
					{
						gDisableStructs[gBattlerTarget].enduredHit = TRUE; // So effects that require damage can activate
						gMoveResultFlags &= ~(MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
						BattleScriptCall(BattleScript_EnduredMsg);
						return;
					}
					else if (gMoveResultFlags & MOVE_RESULT_FOE_HUNG_ON)
					{
						gLastUsedItem = gBattleMons[gBattlerTarget].item;
						gMoveResultFlags &= ~(MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
						BattleScriptCall(BattleScript_HangedOnMsg);
						return;
					}
					else if (gMoveResultFlags & MOVE_RESULT_FAILED)
						stringId = STRINGID_BUTITFAILED;
					else
						gBattleCommunication[MSG_DISPLAY] = FALSE;
            }
        }
        if (stringId)
            PrepareStringBattle(stringId, gBattlerAttacker);
		
        ++gBattlescriptCurrInstr;
    }
}

static void atk10_printstring(void)
{
    if (!gBattleControllerExecFlags)
    {
        u16 var = READ_16(gBattlescriptCurrInstr + 1);

        gBattlescriptCurrInstr += 3;
        PrepareStringBattle(var, gBattlerAttacker);
        gBattleCommunication[MSG_DISPLAY] = TRUE;
    }
}

static void atk11_printselectionstring(void)
{
    BtlController_EmitPrintSelectionString(gBattlerAttacker, BUFFER_A, READ_16(gBattlescriptCurrInstr + 1));
    MarkBattlerForControllerExec(gBattlerAttacker);
    gBattleCommunication[MSG_DISPLAY] = TRUE;
	gBattlescriptCurrInstr += 3;
}

static void atk12_waitmessage(void)
{
    if (!gBattleControllerExecFlags)
    {
        if (!gBattleCommunication[MSG_DISPLAY])
            gBattlescriptCurrInstr += 3;
        else if (++gPauseCounterBattle >= READ_16(gBattlescriptCurrInstr + 1))
        {
			gPauseCounterBattle = 0;
			gBattleCommunication[MSG_DISPLAY] = FALSE;
			gBattlescriptCurrInstr += 3;
        }
    }
}

static void atk13_printfromtable(void)
{
    if (!gBattleControllerExecFlags)
    {
        const u16 *ptr = (const u16 *) READ_PTR(gBattlescriptCurrInstr + 1);
        ptr += gBattleCommunication[MULTISTRING_CHOOSER];
		
		gBattlescriptCurrInstr += 5;
        PrepareStringBattle(*ptr, gBattlerAttacker);
        gBattleCommunication[MSG_DISPLAY] = TRUE;
    }
}

static void atk14_printselectionstringfromtable(void)
{
    if (!gBattleControllerExecFlags)
    {
        const u16 *ptr = (const u16 *) READ_PTR(gBattlescriptCurrInstr + 1);

        ptr += gBattleCommunication[MULTISTRING_CHOOSER];

        BtlController_EmitPrintSelectionString(gBattlerAttacker, BUFFER_A, *ptr);
        MarkBattlerForControllerExec(gBattlerAttacker);
        gBattlescriptCurrInstr += 5;
        gBattleCommunication[MSG_DISPLAY] = TRUE;
    }
}

static void atk15_seteffectwithchance(void)
{
    u32 percentChance;
	
	if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && !ReceiveSheerForceBoost(gBattlerAttacker, gCurrentMove))
	{
		percentChance = GetEffectChanceIncreases(gBattlerAttacker, gBattleMoves[gCurrentMove].secondaryEffectChance);
		
		if (gBattleStruct->moveEffect.moveEffectByte && (gBattleStruct->moveEffect.certain || Random() % 100 <= percentChance))
		{
			if (percentChance >= 100)
				gBattleStruct->moveEffect.certain = TRUE;
			
			DoMoveEffect(FALSE, TRUE, 0);
			gBattleStruct->moveEffect.moveEffectByte = MOVE_EFFECT_NONE;
			return;
		}
	}
	gBattleStruct->moveEffect.moveEffectByte = MOVE_EFFECT_NONE;
	++gBattlescriptCurrInstr;
}

static void atk16_setspecialmoveeffect(void)
{
	bool8 primary = gBattlescriptCurrInstr[1];
	u32 flags = READ_32(gBattlescriptCurrInstr + 2);
	
	gBattlescriptCurrInstr += 5;
	
    DoMoveEffect(primary, TRUE, flags);
}

static void atk17_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk18_clearbattlerstatus(void)
{
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	u32 status = READ_32(gBattlescriptCurrInstr + 3);
	
	switch (gBattlescriptCurrInstr[2])
	{
		case ID_STATUS1:
			gBattleMons[battlerId].status1 &= ~(status);
			break;
		case ID_STATUS2:
		    gBattleMons[battlerId].status2 &= ~(status);
			break;
	}
	gBattlescriptCurrInstr += 7;
}

static void atk19_tryfaintmon(void)
{
    u8 attacker, target, moveIndex;
    const u8* BS_ptr;
	
	if (gBattlescriptCurrInstr[1] == BS_ATTACKER)
	{
		attacker = gBattlerAttacker;
		target = gBattlerTarget;
		BS_ptr = BattleScript_FaintAttacker;
	}
	else
	{
		attacker = gBattlerTarget;
		target = gBattlerAttacker;
		BS_ptr = BattleScript_FaintTarget;
	}
	
	if (!IsBattlerAlive(attacker))
	{
		gHitMarker |= HITMARKER_FAINTED(attacker);
		BattleScriptPush(gBattlescriptCurrInstr + 2);
		gBattlescriptCurrInstr = BS_ptr;
		
		if (GetBattlerSide(attacker) == B_SIDE_PLAYER)
		{
			gHitMarker |= HITMARKER_PLAYER_FAINTED;
			AdjustFriendshipOnBattleFaint(attacker);
		}
		if ((gHitMarker & HITMARKER_DESTINYBOND) && IsBattlerAlive(gBattlerAttacker))
		{
			gHitMarker &= ~(HITMARKER_DESTINYBOND);
			gBattleMoveDamage = gBattleMons[target].hp;
			gBattlescriptCurrInstr = BattleScript_DestinyBondTakesLife;
			BattleScriptCall(BS_ptr);
		}
		if ((gStatuses3[gBattlerTarget] & STATUS3_GRUDGE) && !(gHitMarker & HITMARKER_GRUDGE) && gCurrentMove != MOVE_STRUGGLE
			&& GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget) && IsBattlerAlive(gBattlerAttacker))
		{
			moveIndex = *(gBattleStruct->chosenMovePositions + gBattlerAttacker);
			
			gBattleMons[gBattlerAttacker].pp[moveIndex] = 0;
			BattleScriptPush(gBattlescriptCurrInstr);
			gBattlescriptCurrInstr = BattleScript_GrudgeTakesPp;
			BtlController_EmitSetMonData(gBattlerAttacker, BUFFER_A, moveIndex + REQUEST_PPMOVE1_BATTLE, 0, 1, &gBattleMons[gBattlerAttacker].pp[moveIndex]);
			MarkBattlerForControllerExec(gBattlerAttacker);
			PrepareMoveBuffer(gBattleTextBuff1, gBattleMons[gBattlerAttacker].moves[moveIndex]);
		}
	}
	else
		gBattlescriptCurrInstr += 2;
}

static void atk1A_dofaintanimation(void)
{
    if (!gBattleControllerExecFlags)
    {
        u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
        BtlController_EmitFaintAnimation(battlerId, BUFFER_A);
        MarkBattlerForControllerExec(battlerId);
        gBattlescriptCurrInstr += 2;
    }
}

static void atk1B_cleareffectsonfaint(void)
{
    if (!gBattleControllerExecFlags)
    {
        u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
        gBattleMons[battlerId].status1 = 0;
        BtlController_EmitSetMonData(battlerId, BUFFER_A, REQUEST_STATUS_BATTLE, 0, 0x4, &gBattleMons[battlerId].status1);
        MarkBattlerForControllerExec(battlerId);
        FaintClearSetData(battlerId); // Effects like attractions, trapping, etc.
		DoSpecialFormChange(battlerId, gBattlerPartyIndexes[battlerId], FORM_CHANGE_FAINT);
		// Here bc wild mon teleporting from a wild double battle probably don't increase this counter
		if (gBattleStruct->faintCounter[GetBattlerSide(battlerId)] < 100)
			++gBattleStruct->faintCounter[GetBattlerSide(battlerId)];
		
        gBattlescriptCurrInstr += 2;
    }
}

// unify jumpifbyte, jumpifhalfword and jumpifword into one single function body
static bool8 JumpBasedOnKind(u32 cmpTo, u32 value, u8 cmpKind)
{
	bool8 ret = FALSE;
	
	switch (cmpKind)
	{
		case CMP_EQUAL:
		    if (value == cmpTo)
				ret = TRUE;
			break;
		case CMP_NOT_EQUAL:
		    if (value != cmpTo)
				ret = TRUE;
			break;
		case CMP_GREATER_THAN:
		    if (value > cmpTo)
				ret = TRUE;
			break;
		case CMP_LESS_THAN:
		    if (value < cmpTo)
				ret = TRUE;
			break;
		case CMP_COMMON_BITS:
		    if (value & cmpTo)
				ret = TRUE;
			break;
		case CMP_NO_COMMON_BITS:
		    if (!(value & cmpTo))
				ret = TRUE;
			break;
	}
	return ret;
}

static void atk1C_jumpbasedonstatus(void)
{
	const u8* jumpPtr;
    u8 cmpKind, battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	u32 status;
	
	if (IsBattlerAlive(battlerId))
	{
		cmpKind = gBattlescriptCurrInstr[3];
		status = READ_32(gBattlescriptCurrInstr + 4);
		jumpPtr = READ_PTR(gBattlescriptCurrInstr + 8);
		
		switch (gBattlescriptCurrInstr[2])
		{
			case ID_STATUS1:
			    if (JumpBasedOnKind(status, gBattleMons[battlerId].status1, cmpKind))
				{
					gBattlescriptCurrInstr = jumpPtr;
					return;
				}
				break;
			case ID_STATUS2:
			    if (JumpBasedOnKind(status, gBattleMons[battlerId].status2, cmpKind))
				{
					gBattlescriptCurrInstr = jumpPtr;
					return;
				}
				break;
			case ID_STATUS3:
			    if (JumpBasedOnKind(status, gStatuses3[battlerId], cmpKind))
				{
					gBattlescriptCurrInstr = jumpPtr;
					return;
				}
				break;
		}
	}
	gBattlescriptCurrInstr += 12;
}

static void atk1D_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk1E_jumpifability(void)
{
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    u16 ability = READ_16(gBattlescriptCurrInstr + 2);
	
	if (GetBattlerAbility(battlerId) == ability)
	{
		gLastUsedAbility = ability;
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 4);
		RecordAbilityBattle(battlerId, gLastUsedAbility);
		gBattleScripting.battlerWithAbility = battlerId;
    }
	else
		gBattlescriptCurrInstr += 8;
}

static void atk1F_jumpifsideaffecting(void)
{
    if (gSideStatuses[GetBattlerSide(GetBattlerForBattleScript(gBattlescriptCurrInstr[1]))] & READ_16(gBattlescriptCurrInstr + 2))
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 4);
    else
        gBattlescriptCurrInstr += 8;
}

bool8 CompareStat(u8 battlerId, u8 statId, s8 cmpTo, u8 cmpKind)
{
	if (GetBattlerAbility(battlerId) == ABILITY_CONTRARY)
    {
		cmpTo -= 12;
		cmpTo *= -1;
    }
	return JumpBasedOnKind(cmpTo, gBattleMons[battlerId].statStages[statId], cmpKind);
}

static void atk20_jumpifstat(void)
{
	if (CompareStat(GetBattlerForBattleScript(gBattlescriptCurrInstr[1]), gBattlescriptCurrInstr[3], gBattlescriptCurrInstr[4], gBattlescriptCurrInstr[2]))
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 5);
	else
		gBattlescriptCurrInstr += 9;
}

static void atk21_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk22_jumpbasedontype(void)
{
	u8 battler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	if (!gBattlescriptCurrInstr[3])
	{
		if (IS_BATTLER_OF_TYPE(battler, gBattlescriptCurrInstr[2]))
			gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 4);
		else
			gBattlescriptCurrInstr += 8;
	}
	else
	{
		if (!IS_BATTLER_OF_TYPE(battler, gBattlescriptCurrInstr[2]))
			gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 4);
		else
			gBattlescriptCurrInstr += 8;
	}
}

static void atk23_getexp(void)
{
    u8 battler, holdEffect, viaSentIn, i, viaExpShare, level;
	u16 calculatedExp, stringId;
	s32 sentIn;
	
    gBattlerFainted = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    sentIn = gSentPokesToOpponent[(gBattlerFainted & 2) >> 1];
	
    switch (gBattleScripting.atk23_state)
    {
		case 0: // Check if should receive exp at all
            if (GetBattlerSide(gBattlerFainted) != B_SIDE_OPPONENT
			|| (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_SAFARI | BATTLE_TYPE_EREADER_TRAINER)))
			    gBattleScripting.atk23_state = 6; // Goto last case
			else
			{
				++gBattleScripting.atk23_state;
				gBattleStruct->givenExpMons |= gBitTable[gBattlerPartyIndexes[gBattlerFainted]];
			}
			break;
		case 1: // Calculate experience points to redistribute
            for (viaSentIn = 0, i = 0; i < PARTY_SIZE; ++i)
            {
                if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) && GetMonData(&gPlayerParty[i], MON_DATA_HP))
				{
                    if (gBitTable[i] & sentIn)
                        ++viaSentIn;
                }
            }
            calculatedExp = gBaseStats[gBattleMons[gBattlerFainted].species].expYield * gBattleMons[gBattlerFainted].level / 5;
		
            if (gSaveBlock2Ptr->expShare) // Exp share is turned on
            {
                gBattleStruct->expValue = SAFE_DIV(calculatedExp / 2, viaSentIn);
				
                if (gBattleStruct->expValue == 0)
                    gBattleStruct->expValue = 1;
                viaExpShare = gSaveBlock1Ptr->playerPartyCount;
                gExpShareExp = calculatedExp / 2;
                if (gExpShareExp == 0)
                    gExpShareExp = 1;
            }
            else
            {
#if FULL_EXP_FOR_PARTICIPATING
                gBattleStruct->expValue = calculatedExp;
#else
		        gBattleStruct->expValue = SAFE_DIV(calculatedExp, viaSentIn);
#endif
                if (gBattleStruct->expValue == 0)
                    gBattleStruct->expValue = 1;
                gExpShareExp = 0;
            }
            ++gBattleScripting.atk23_state;
            gBattleStruct->expGetterMonId = 0;
            gBattleStruct->sentInPokes = sentIn;
        // Fall through
		case 2: // Set exp value to the poke in expgetter_id and print message
			if (!gBattleControllerExecFlags)
			{
				u8 level = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL), levelCap = GetCurrentLevelCapLevel();
				holdEffect = ItemId_GetHoldEffect(GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HELD_ITEM));
				
				if (!gSaveBlock2Ptr->expShare && !(gBattleStruct->sentInPokes & 1))
				{
					*(&gBattleStruct->sentInPokes) >>= 1;
					gBattleScripting.atk23_state = 5;
					gBattleMoveDamage = 0; // Used for exp
				}     
				else if (level == MAX_LEVEL)
				{
					*(&gBattleStruct->sentInPokes) >>= 1;
					gBattleScripting.atk23_state = 5;
					gBattleMoveDamage = 0; // Used for exp
				}
#if EXP_BLOCK
                else if (levelCap != MAX_LEVEL && level >= levelCap)
				{
					*(&gBattleStruct->sentInPokes) >>= 1;
					gBattleScripting.atk23_state = 5;
					gBattleMoveDamage = 0; // Used for exp
				}
#endif
                else
				{
					// Music change in wild battle after fainting a poke
					if (!(gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_POKEDUDE)) && (IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT))
					|| (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT))))
					&& !gBattleStruct->wildVictorySong && !IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT))
                    && !IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)))
					{
						BattleStopLowHpSound();
						PlayBGM(MUS_VICTORY_WILD);
						gBattleStruct->wildVictorySong = TRUE;
					}
					if (GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HP) && !GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_IS_EGG))
					{
						if (gBattleStruct->sentInPokes & 1)
							gBattleMoveDamage = gBattleStruct->expValue;
						else
							gBattleMoveDamage = 0;
						
						if (gSaveBlock2Ptr->expShare)
							gBattleMoveDamage += gExpShareExp;
						
						if (holdEffect == HOLD_EFFECT_LUCKY_EGG)
							gBattleMoveDamage = (gBattleMoveDamage * 150) / 100;
						
						if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
							gBattleMoveDamage = (gBattleMoveDamage * 150) / 100;
						
						if (IsTradedMon(&gPlayerParty[gBattleStruct->expGetterMonId]) && !(gBattleTypeFlags & BATTLE_TYPE_POKEDUDE))
						{
							gBattleMoveDamage = (gBattleMoveDamage * 150) / 100;
							stringId = STRINGID_ABOOSTED;
						}
						else
							stringId = STRINGID_EMPTYSTRING;
						// Get exp getter battlerId
						if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
						{
							if (!(gBattlerPartyIndexes[2] != gBattleStruct->expGetterMonId) && !(gAbsentBattlerFlags & gBitTable[2]))
								gBattleStruct->expGetterBattlerId = 2;
							else
							{
								if (!(gAbsentBattlerFlags & gBitTable[0]))
									gBattleStruct->expGetterBattlerId = 0;
								else
									gBattleStruct->expGetterBattlerId = 2;
							}
						}
						else
							gBattleStruct->expGetterBattlerId = 0;
						
						PrepareMonNickWithPrefixBuffer(gBattleTextBuff1, gBattleStruct->expGetterBattlerId, gBattleStruct->expGetterMonId);
						// Buffer 'gained' or 'gained a boosted'
						PrepareStringBuffer(gBattleTextBuff2, stringId);
						PrepareWordNumberBuffer(gBattleTextBuff3, 5, gBattleMoveDamage);
						PrepareStringBattle(STRINGID_PKMNGAINEDEXP, gBattleStruct->expGetterBattlerId);
						MonGainEVs(&gPlayerParty[gBattleStruct->expGetterMonId], gBattleMons[gBattlerFainted].species);
					}
					gBattleStruct->sentInPokes >>= 1;
					++gBattleScripting.atk23_state;
				}
			}
			break;
		case 3: // Set stats and give exp
		    if (!gBattleControllerExecFlags)
			{
				gBattleBufferB[gBattleStruct->expGetterBattlerId][0] = 0;
				
				if (GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HP) && GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL) != MAX_LEVEL)
				{
					gBattleResources->beforeLvlUp->stats[STAT_HP]    = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_MAX_HP);
					gBattleResources->beforeLvlUp->stats[STAT_ATK]   = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_ATK);
					gBattleResources->beforeLvlUp->stats[STAT_DEF]   = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_DEF);
					gBattleResources->beforeLvlUp->stats[STAT_SPEED] = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPEED);
					gBattleResources->beforeLvlUp->stats[STAT_SPATK] = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPATK);
					gBattleResources->beforeLvlUp->stats[STAT_SPDEF] = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPDEF);
					BtlController_EmitExpUpdate(gBattleStruct->expGetterBattlerId, BUFFER_A, gBattleStruct->expGetterMonId, gBattleMoveDamage);
					MarkBattlerForControllerExec(gBattleStruct->expGetterBattlerId);
				}
				++gBattleScripting.atk23_state;
			}
			break;
		case 4: // Lvl up if necessary
		    if (!gBattleControllerExecFlags)
			{
				battler = gBattleStruct->expGetterBattlerId;
				
				if (gBattleBufferB[battler][0] == CONTROLLER_TWORETURNVALUES && gBattleBufferB[battler][1] == RET_VALUE_LEVELED_UP)
				{
					if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && gBattlerPartyIndexes[battler] == gBattleStruct->expGetterMonId)
						HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[battler]], battler);
					
					PrepareMonNickWithPrefixBuffer(gBattleTextBuff1, battler, gBattleStruct->expGetterMonId);
					PrepareByteNumberBuffer(gBattleTextBuff2, 3, GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL));
					BattleScriptCall(BattleScript_LevelUp);
					gLeveledUpInBattle |= gBitTable[gBattleStruct->expGetterMonId];
					gBattleMoveDamage = (gBattleBufferB[battler][2] | (gBattleBufferB[battler][3] << 8));
					AdjustFriendship(&gPlayerParty[gBattleStruct->expGetterMonId], FRIENDSHIP_EVENT_GROW_LEVEL);
					// Update battle mon structure after level up
					if (gBattlerPartyIndexes[0] == gBattleStruct->expGetterMonId && IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
					{
						battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
						
						gBattleMons[battler].level = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL);
						gBattleMons[battler].hp = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HP);
						gBattleMons[battler].maxHP = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_MAX_HP);
						gBattleMons[battler].attack = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_ATK);
						gBattleMons[battler].defense = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_DEF);
						gBattleMons[battler].speed = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPEED);
						gBattleMons[battler].spAttack = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPATK);
						gBattleMons[battler].spDefense = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPDEF);
						
						if (gBattleStruct->payDayLevels[0]) // Update pay day level
							gBattleStruct->payDayLevels[0] = gBattleMons[battler].level;
					}
					else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && gBattlerPartyIndexes[2] == gBattleStruct->expGetterMonId
					&& IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)))
					{
						battler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
						
						gBattleMons[battler].level = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL);
						gBattleMons[battler].hp = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HP);
						gBattleMons[battler].maxHP = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_MAX_HP);
						gBattleMons[battler].attack = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_ATK);
						gBattleMons[battler].defense = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_DEF);
						gBattleMons[battler].speed = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPEED);
						gBattleMons[battler].spAttack = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPATK);
						gBattleMons[battler].spDefense = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPDEF);
						
						if (gBattleStruct->payDayLevels[1]) // Update pay day level
							gBattleStruct->payDayLevels[1] = gBattleMons[battler].level;
					}
					gBattleScripting.atk23_state = 5;
				}
				else
				{
					gBattleMoveDamage = 0;
					gBattleScripting.atk23_state = 5;
				}
			}
			break;
		case 5: // Looper increment
		    if (gBattleMoveDamage) // There is exp to give, goto case 3 that gives exp
			    gBattleScripting.atk23_state = 3;
			else
			{
				++gBattleStruct->expGetterMonId;
				
				if (gBattleStruct->expGetterMonId < PARTY_SIZE)
					gBattleScripting.atk23_state = 2; // Loop again
				else
					gBattleScripting.atk23_state = 6; // We're done
			}
			break;
		case 6: // Increment instruction
            if (!gBattleControllerExecFlags)
			{
				// Not sure why gf clears the item and ability here
				gBattleMons[gBattlerFainted].item = ITEM_NONE;
				gBattleMons[gBattlerFainted].ability = ABILITY_NONE;
				gBattlescriptCurrInstr += 2;
			}
			break;
	}
}

static void atk24_checkteamslot(void)
{
	u8 i, foundPlayer, foundOpponent;

    if (!gBattleControllerExecFlags)
    {
        if (NoAliveMonsForParty(gPlayerParty))
            gBattleOutcome |= B_OUTCOME_LOST;
		
        if (NoAliveMonsForParty(gEnemyParty))
            gBattleOutcome |= B_OUTCOME_WON;
		
        if (gBattleOutcome == 0 && (gBattleTypeFlags & BATTLE_TYPE_LINK))
        {
            for (foundPlayer = 0, i = 0; i < gBattlersCount; i += 2)
			{
				u32 *ptr = &gHitMarker;
				u32 hitMarkerUnk = 0x10000000;
				
				if ((hitMarkerUnk << i) & *ptr && !gSpecialStatuses[i].faintedHasReplacement)
					foundPlayer++;
			}
			
            for (foundOpponent = 0, i = 1; i < gBattlersCount; i += 2)
			{
				u32 *ptr = &gHitMarker;
				u32 hitMarkerUnk = 0x10000000;
				
				if ((hitMarkerUnk << i) & *ptr && !gSpecialStatuses[i].faintedHasReplacement)
					foundOpponent++;
			}
				
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                if (foundOpponent + foundPlayer > 1)
                    gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
                else
                    gBattlescriptCurrInstr += 5;
            }
            else
            {
                if (foundOpponent && foundPlayer)
                    gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
                else
                    gBattlescriptCurrInstr += 5;
            }
        }
        else
            gBattlescriptCurrInstr += 5;
    }
}

static void atk25_jumpifabilityonside(void)
{
	u16 abilityId = READ_16(gBattlescriptCurrInstr + 2);
	u8 ret = CheckAbilityInBattle(CHECK_ABILITY_ON_SIDE, GetBattlerSide(GetBattlerForBattleScript(gBattlescriptCurrInstr[1])), abilityId);
	
	if (ret)
	{
		gBattleScripting.battler = ret - 1;
		gLastUsedAbility = abilityId;
		RecordAbilityBattle(gBattleScripting.battler, gLastUsedAbility);
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 4);
	}
	else
		gBattlescriptCurrInstr += 8;
}

static void atk26_setmoveeffect(void)
{
	SetMoveEffect(gBattlescriptCurrInstr[1], gBattlescriptCurrInstr[2], gBattlescriptCurrInstr[3]);
    gBattlescriptCurrInstr += 4;
}

static void atk27_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk28_goto(void)
{
    gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atk29_jumpifbyte(void)
{
    const u8 *memByte = READ_PTR(gBattlescriptCurrInstr + 2);
	
	if (JumpBasedOnKind(gBattlescriptCurrInstr[6], *memByte, gBattlescriptCurrInstr[1]))
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 7);
	else
		gBattlescriptCurrInstr += 11;
}

static void atk2A_jumpifhalfword(void)
{
    const u16 *memHword = READ_PTR2(gBattlescriptCurrInstr + 2);
	
	if (JumpBasedOnKind(READ_16(gBattlescriptCurrInstr + 6), *memHword, gBattlescriptCurrInstr[1]))
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 8);
	else
		gBattlescriptCurrInstr += 12;
}

static void atk2B_jumpifword(void)
{
    const u32 *memWord = READ_PTR2(gBattlescriptCurrInstr + 2);
	
	if (JumpBasedOnKind(READ_32(gBattlescriptCurrInstr + 6), *memWord, gBattlescriptCurrInstr[1]))
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 10);
	else
		gBattlescriptCurrInstr += 14;
}

static void atk2C_jumpifarrayequal(void)
{
    u8 i;
    const u8 *mem1 = READ_PTR(gBattlescriptCurrInstr + 1);
    const u8 *mem2 = READ_PTR(gBattlescriptCurrInstr + 5);
    const u8 *jumpPtr = READ_PTR(gBattlescriptCurrInstr + 10);
    u32 size = gBattlescriptCurrInstr[9];

    for (i = 0; i < size; ++i, ++mem1, ++mem2)
    {
        if (*mem1 != *mem2)
        {
            gBattlescriptCurrInstr += 14;
            break;
        }
    }
    if (i == size)
        gBattlescriptCurrInstr = jumpPtr;
}

static void atk2D_jumpifarraynotequal(void)
{
    u8 i, equalBytes = 0;
    const u8 *mem1 = READ_PTR(gBattlescriptCurrInstr + 1);
    const u8 *mem2 = READ_PTR(gBattlescriptCurrInstr + 5);
    const u8 *jumpPtr = READ_PTR(gBattlescriptCurrInstr + 10);
    u32 size = gBattlescriptCurrInstr[9];

    for (i = 0; i < size; ++i, ++mem1, ++mem2)
    {
        if (*mem1 == *mem2)
            ++equalBytes;
    }
    if (equalBytes != size)
        gBattlescriptCurrInstr = jumpPtr;
    else
        gBattlescriptCurrInstr += 14;
}

static void atk2E_setbyte(void)
{
    u8 *memByte = READ_PTR(gBattlescriptCurrInstr + 1);

    *memByte = gBattlescriptCurrInstr[5];
    gBattlescriptCurrInstr += 6;
}

static void atk2F_addbyte(void)
{
    u8 *memByte = READ_PTR(gBattlescriptCurrInstr + 1);

    *memByte += gBattlescriptCurrInstr[5];
    gBattlescriptCurrInstr += 6;
}

static void atk30_subbyte(void)
{
    u8 *memByte = READ_PTR(gBattlescriptCurrInstr + 1);

    *memByte -= gBattlescriptCurrInstr[5];
    gBattlescriptCurrInstr += 6;
}

static void atk31_copyarray(void)
{
    u8 *dest = READ_PTR(gBattlescriptCurrInstr + 1);
    const u8 *src = READ_PTR(gBattlescriptCurrInstr + 5);
    s32 i;

    for (i = 0; i < gBattlescriptCurrInstr[9]; ++i)
        dest[i] = src[i];
	
    gBattlescriptCurrInstr += 10;
}

static void atk32_sethalfword(void)
{
    u16 *memHWord = READ_PTR2(gBattlescriptCurrInstr + 1);
	u16 val = READ_16(gBattlescriptCurrInstr + 5);
	
    *memHWord = val;
    gBattlescriptCurrInstr += 7;
}

static void atk33_orbyte(void)
{
    u8 *memByte = READ_PTR(gBattlescriptCurrInstr + 1);
	
    *memByte |= gBattlescriptCurrInstr[5];
    gBattlescriptCurrInstr += 6;
}

static void atk34_orhalfword(void)
{
    u16 *memHword = READ_PTR2(gBattlescriptCurrInstr + 1);
    u16 val = READ_16(gBattlescriptCurrInstr + 5);

    *memHword |= val;
    gBattlescriptCurrInstr += 7;
}

static void atk35_orword(void)
{
    u32 *memWord = READ_PTR2(gBattlescriptCurrInstr + 1);
    u32 val = READ_32(gBattlescriptCurrInstr + 5);

    *memWord |= val;
    gBattlescriptCurrInstr += 9;
}

static void atk36_bicbyte(void)
{
    u8 *memByte = READ_PTR(gBattlescriptCurrInstr + 1);

    *memByte &= ~(gBattlescriptCurrInstr[5]);
    gBattlescriptCurrInstr += 6;
}

static void atk37_bichalfword(void)
{
    u16 *memHword = READ_PTR2(gBattlescriptCurrInstr + 1);
    u16 val = READ_16(gBattlescriptCurrInstr + 5);

    *memHword &= ~(val);
    gBattlescriptCurrInstr += 7;
}

static void atk38_bicword(void)
{
    u32 *memWord = READ_PTR2(gBattlescriptCurrInstr + 1);
    u32 val = READ_32(gBattlescriptCurrInstr + 5);

    *memWord &= ~(val);
    gBattlescriptCurrInstr += 9;
}

static void atk39_pause(void)
{
    if (!gBattleControllerExecFlags)
    {
        if (++gPauseCounterBattle >= READ_16(gBattlescriptCurrInstr + 1))
        {
            gPauseCounterBattle = 0;
            gBattlescriptCurrInstr += 3;
        }
    }
}

static void atk3A_waitstate(void)
{
    if (!gBattleControllerExecFlags)
        ++gBattlescriptCurrInstr;
}

static void atk3B_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk3C_return(void)
{
    BattleScriptPop();
}

static void atk3D_end(void)
{
    gMoveResultFlags = 0;
    gCurrentActionFuncId = B_ACTION_TRY_FINISH;
}

static void atk3E_end2(void)
{
    gCurrentActionFuncId = B_ACTION_TRY_FINISH;
}

static void atk3F_end3(void) // pops the main function stack
{
    BattleScriptPop();
    if (gBattleResources->battleCallbackStack->size != 0)
        gBattleResources->battleCallbackStack->size--;
    gBattleMainFunc = gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size];
}

static void atk40_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk41_call(void)
{
    BattleScriptPush(gBattlescriptCurrInstr + 5);
    gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atk42_trysetsleep(void)
{
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	u32 flags = READ_32(gBattlescriptCurrInstr + 2);
	
	switch (CanBePutToSleep(gBattlerAttacker, battlerId, flags))
	{
		case STATUS_CHANGE_WORKED:
			if (!(flags & STATUS_CHANGE_FLAG_IGNORE_SUBSTITUTE) && SubsBlockMove(gBattlerAttacker, battlerId, gCurrentMove))
				gBattlescriptCurrInstr = BattleScript_ButItFailed;
			else
				gBattlescriptCurrInstr += 6;
			break;
		case STATUS_CHANGE_FAIL_SPECIFIC_STATUSED:
			gBattlescriptCurrInstr = BattleScript_AlreadyAsleep;
			break;
		case STATUS_CHANGE_FAIL_ALREADY_STATUSED:
			gBattlescriptCurrInstr = BattleScript_ButItFailed;
			break;
		case STATUS_CHANGE_FAIL_SAFEGUARD_PROTECTED:
			gBattlescriptCurrInstr = BattleScript_SafeguardProtected;
			break;
		case STATUS_CHANGE_FAIL_UPROAR:
			gBattlescriptCurrInstr = BattleScript_CantMakeAsleep;
			break;
		case STATUS_CHANGE_FAIL_ABILITY_PREVENTED:
			gLastUsedAbility = GetBattlerAbility(battlerId);
			RecordAbilityBattle(battlerId, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_AbilityPreventSleep;
			break;
		case STATUS_CHANGE_FAIL_SWEET_VEIL_ON_SIDE:
			gBattleScripting.battler = gBattleScripting.savedBattler;
			gLastUsedAbility = GetBattlerAbility(gBattleScripting.battler);
			RecordAbilityBattle(gBattleScripting.battler, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_TeamProtectedBySweetVeil;
			break;
		case STATUS_CHANGE_FAIL_FLOWER_VEIL_ON_SIDE:
			gBattleScripting.battler = gBattleScripting.savedBattler;
			gLastUsedAbility = GetBattlerAbility(gBattleScripting.battler);
			RecordAbilityBattle(gBattleScripting.battler, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_TeamProtectedByFlowerVeil;
			break;
	}
}

static void atk43_trysetconfusion(void)
{
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	switch (CanBecameConfused(gBattlerAttacker, battlerId, 0))
	{
		case STATUS_CHANGE_WORKED:
			if (SubsBlockMove(gBattlerAttacker, battlerId, gCurrentMove))
				gBattlescriptCurrInstr = BattleScript_ButItFailed;
			else
				gBattlescriptCurrInstr += 2;
			break;
		case STATUS_CHANGE_FAIL_ABILITY_PREVENTED:
		    gLastUsedAbility = GetBattlerAbility(battlerId);
			RecordAbilityBattle(battlerId, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_OwnTempoPrevents;
			break;
		case STATUS_CHANGE_FAIL_SPECIFIC_STATUSED:
			gBattlescriptCurrInstr = BattleScript_AlreadyConfused;
			break;
		case STATUS_CHANGE_FAIL_SAFEGUARD_PROTECTED:
		    gBattlescriptCurrInstr = BattleScript_SafeguardProtected;
			break;
	}
}

static void atk44_endselectionscript(void)
{
    gBattleStruct->selectionScriptFinished |= gBitTable[gBattlerAttacker];
}

// unify playanimation and playanimation2 into a single function
static void PlayAnimationInternal(u8 animId, u16 argument, u8 addr)
{
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	if (animId == B_ANIM_STATS_CHANGE || animId == B_ANIM_SNATCH_MOVE || animId == B_ANIM_SUBSTITUTE_FADE || animId == B_ANIM_SILPH_SCOPED
	|| animId == B_ANIM_ILLUSION_OFF || animId == B_ANIM_FORM_CHANGE)
    {
        BtlController_EmitBattleAnimation(battlerId, BUFFER_A, animId, argument);
        MarkBattlerForControllerExec(battlerId);
        gBattlescriptCurrInstr += addr;
    }
    else if (!IsBattleAnimationsOn() && animId != B_ANIM_MON_HIT)
    {
		BattleScriptPush(gBattlescriptCurrInstr + addr);
		gBattlescriptCurrInstr = BattleScript_Pausex20;
    }
    else if (animId == B_ANIM_RAIN_CONTINUES || animId == B_ANIM_SUN_CONTINUES || animId == B_ANIM_SANDSTORM_CONTINUES || animId == B_ANIM_HAIL_CONTINUES
	|| animId == B_ANIM_FOG_CONTINUES || animId == B_ANIM_STRONG_WINDS_CONTINUE)
    {
        BtlController_EmitBattleAnimation(battlerId, BUFFER_A, animId, argument);
        MarkBattlerForControllerExec(battlerId);
        gBattlescriptCurrInstr += addr;
    }
    else if (gStatuses3[battlerId] & STATUS3_SEMI_INVULNERABLE)
        gBattlescriptCurrInstr += addr;
    else
    {
        BtlController_EmitBattleAnimation(battlerId, BUFFER_A, animId, argument);
        MarkBattlerForControllerExec(battlerId);
        gBattlescriptCurrInstr += addr;
    }
}

static void atk45_playanimation(void)
{
    const u16 *argumentPtr = READ_PTR2(gBattlescriptCurrInstr + 3);

	PlayAnimationInternal(gBattlescriptCurrInstr[2], *argumentPtr, 7);
}

static void atk46_playanimation2(void) // Animation Id is stored in the first pointer
{
	const u8 *animationIdPtr = READ_PTR(gBattlescriptCurrInstr + 2);
    const u16 *argumentPtr = READ_PTR2(gBattlescriptCurrInstr + 6);

	PlayAnimationInternal(*animationIdPtr, *argumentPtr, 10);
}

static void atk47_setgraphicalstatchangevalues(void)
{
    u8 value = 0;

    switch (GET_STAT_BUFF_VALUE2(gBattleScripting.statChanger))
    {
		case SET_STAT_BUFF_VALUE(1): // +1
            value = STAT_ANIM_PLUS1;
			break;
		case SET_STAT_BUFF_VALUE(2): // +2
            value = STAT_ANIM_PLUS2;
			break;
		case SET_STAT_BUFF_VALUE(1) | STAT_BUFF_NEGATIVE: // -1
            value = STAT_ANIM_MINUS1;
            break;
		case SET_STAT_BUFF_VALUE(2) | STAT_BUFF_NEGATIVE: // -2
            value = STAT_ANIM_MINUS2;
			break;
    }
    gBattleScripting.animArg1 = GET_STAT_BUFF_ID(gBattleScripting.statChanger) + value - 1;
    gBattleScripting.animArg2 = 0;
    ++gBattlescriptCurrInstr;
}

static void atk48_playstatchangeanimation(void)
{
    u8 statsToCheck = 0, flag = gBattlescriptCurrInstr[3];
    u16 statAnimId = 0;
    s16 startingStatAnimId;
    u32 currStat = 0;
    s32 changeableStatsCount = 0;
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
    statsToCheck = gBattlescriptCurrInstr[2];
	
    if (GetBattlerAbility(battlerId) == ABILITY_SIMPLE)
	    flag |= ATK48_STAT_BY_TWO;
	
    if (GetBattlerAbility(battlerId) == ABILITY_CONTRARY)
		flag ^= ATK48_STAT_NEGATIVE;
	
    if (flag & ATK48_STAT_NEGATIVE) // Goes down
    {
        if (flag & ATK48_STAT_BY_TWO)
            startingStatAnimId = STAT_ANIM_MINUS2 - 1;
        else
            startingStatAnimId = STAT_ANIM_MINUS1 - 1;

        while (statsToCheck != 0)
        {
            if (statsToCheck & 1)
            {
                if (flag & ATK48_DONT_CHECK_LOWER)
                {
                    if (gBattleMons[battlerId].statStages[currStat] > MIN_STAT_STAGES)
                    {
                        statAnimId = startingStatAnimId + currStat;
                        ++changeableStatsCount;
                    }
                }
                else if (!(gSideStatuses[GetBattlerSide(battlerId)] & SIDE_STATUS_MIST) && GetBattlerAbility(battlerId) != ABILITY_CLEAR_BODY
			 && GetBattlerAbility(battlerId) != ABILITY_WHITE_SMOKE && GetBattlerAbility(battlerId) != ABILITY_FULL_METAL_BODY
			 && !(GetBattlerAbility(battlerId) == ABILITY_KEEN_EYE && currStat == STAT_ACC)
			 && !(GetBattlerAbility(battlerId) == ABILITY_HYPER_CUTTER && currStat == STAT_ATK)
			 && !(GetBattlerAbility(battlerId) == ABILITY_BIG_PECKS && currStat == STAT_DEF)
			 && !(GetBattlerAbility(battlerId) == ABILITY_FLOWER_VEIL && IS_BATTLER_OF_TYPE(battlerId, TYPE_GRASS))
			 && !(IsBattlerAlive(BATTLE_PARTNER(battlerId)) && GetBattlerAbility(BATTLE_PARTNER(battlerId)) == ABILITY_FLOWER_VEIL && IS_BATTLER_OF_TYPE(battlerId, TYPE_GRASS)))
                {
                    if (gBattleMons[battlerId].statStages[currStat] > MIN_STAT_STAGES)
                    {
                        statAnimId = startingStatAnimId + currStat;
                        ++changeableStatsCount;
                    }
                }
            }
            statsToCheck >>= 1;
            ++currStat;
        }

        if (changeableStatsCount > 1) // More than one stat, so the color is gray
        {
            if (flag & ATK48_STAT_BY_TWO)
                statAnimId = STAT_ANIM_MULTIPLE_MINUS2;
            else
                statAnimId = STAT_ANIM_MULTIPLE_MINUS1;
        }
    }
    else // Goes up
    {
        if (flag & ATK48_STAT_BY_TWO)
            startingStatAnimId = STAT_ANIM_PLUS2 - 1;
        else
            startingStatAnimId = STAT_ANIM_PLUS1 - 1;

        while (statsToCheck != 0)
        {
            if (statsToCheck & 1 && gBattleMons[battlerId].statStages[currStat] < MAX_STAT_STAGES)
            {
                statAnimId = startingStatAnimId + currStat;
                ++changeableStatsCount;
            }
            statsToCheck >>= 1;
            ++currStat;
        }
        if (changeableStatsCount > 1) // More than one stat, so the color is gray
        {
            if (flag & ATK48_STAT_BY_TWO)
                statAnimId = STAT_ANIM_MULTIPLE_PLUS2;
            else
                statAnimId = STAT_ANIM_MULTIPLE_PLUS1;
        }
    }
    if (flag & ATK48_ONLY_MULTIPLE && changeableStatsCount < 2)
        gBattlescriptCurrInstr += 4;
    else if (changeableStatsCount != 0 && !gBattleScripting.statAnimPlayed)
    {
        BtlController_EmitBattleAnimation(battlerId, BUFFER_A, B_ANIM_STATS_CHANGE, statAnimId);
        MarkBattlerForControllerExec(battlerId);
        if (flag & ATK48_ONLY_MULTIPLE && changeableStatsCount > 1)
            gBattleScripting.statAnimPlayed = TRUE;
        gBattlescriptCurrInstr += 4;
    }
    else
        gBattlescriptCurrInstr += 4;
}

static u8 GetNextTarget(u8 moveTarget)
{
    u8 i;
	
    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        if (i != gBattlerAttacker && IsBattlerAlive(i) && !(gBattleStruct->targetsDone[gBattlerAttacker] & gBitTable[i])
            && (GetBattlerSide(i) != GetBattlerSide(gBattlerAttacker) || moveTarget == MOVE_TARGET_FOES_AND_ALLY))
                break;
    }
    return i;
}

static void SetNextTarget(u8 nextTarget)
{
	gBattleStruct->moveTarget[gBattlerAttacker] = gBattlerTarget = nextTarget;
	gHitMarker |= HITMARKER_NO_ATTACKSTRING;
	gBattleScripting.atk49_state = 0;
	MoveValuesCleanUp();
	BattleScriptPush(gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect]);
	gBattlescriptCurrInstr = BattleScript_FlushMessageBox;
}

static void atk49_moveend(void)
{
	u8 state = gBattlescriptCurrInstr[1], lastCase = gBattlescriptCurrInstr[2];
	u16 originallyUsedMove = gChosenMove == MOVE_UNVAILABLE ? MOVE_NONE : gChosenMove;
	s32 i;
	bool8 effect = FALSE;
    
	do
	{
		switch (gBattleScripting.atk49_state)
		{
			case ATK49_SUM_DMG: // Sum and store damage dealt for multi strike recoil
			    if (state != ATK49_MOVEEND_FUTURE_ATTACK)
					gBattleScripting.savedDmg += gHpDealt;
				
				++gBattleScripting.atk49_state;
				break;
			case ATK49_RAGE: // Rage check
			    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(gBattlerTarget) && gBattleMons[gBattlerTarget].status2 & STATUS2_RAGE
				&& IsBattlerAlive(gBattlerTarget) && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget) && !IS_MOVE_STATUS(gCurrentMove)
				&& CompareStat(gBattlerTarget, STAT_ATK, MAX_STAT_STAGES, CMP_LESS_THAN))
				{
					BattleScriptCall(BattleScript_RageIsBuilding);
					effect = TRUE;
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_SYNCHRONIZE_TARGET: // Target's synchronize
			    if (AbilityBattleEffects(ABILITYEFFECT_SYNCHRONIZE, gBattlerTarget))
				{
					gBattleStruct->moveEffect.affectsUser = TRUE;
					effect = TRUE;
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_MOVE_END_ABILITIES_ATTACKER: // Attacker abilities that activates on contact. Poison Touch and Stench.
			    if (state != ATK49_MOVEEND_FUTURE_ATTACK && AbilityBattleEffects(ABILITYEFFECT_MOVE_END_ATTACKER, gBattlerAttacker))
					effect = TRUE;
				++gBattleScripting.atk49_state;
				break;
			case ATK49_MOVE_END_ABILITIES: // same as above, but for target. Rough Skin, Effect Spore etc.
			    if (AbilityBattleEffects(ABILITYEFFECT_MOVE_END_TARGET, gBattlerTarget))
					effect = TRUE;
				++gBattleScripting.atk49_state;
				break;
			case ATK49_STATUS_IMMUNITY_ABILITIES: // Status immunity abilities
			    if (AbilityBattleEffects(ABILITYEFFECT_IMMUNITY, 0))
					effect = TRUE; // It loops through all battlers, so we increment after its done with all battlers
				else
					++gBattleScripting.atk49_state;
				break;
			case ATK49_SYNCHRONIZE_ATTACKER: // Attacker's synchronize
			    if (AbilityBattleEffects(ABILITYEFFECT_SYNCHRONIZE, gBattlerAttacker))
					effect = TRUE;
				++gBattleScripting.atk49_state;
				break;
			case ATK49_CHOICE_MOVE: // update Choice Band move
			    if (state != ATK49_MOVEEND_FUTURE_ATTACK)
				{
					if (gHitMarker & HITMARKER_OBEYS && GetBattlerItemHoldEffect(gBattlerAttacker, TRUE) == HOLD_EFFECT_CHOICE_ITEM
					&& gChosenMove != MOVE_STRUGGLE && (!gBattleStruct->choicedMove[gBattlerAttacker] || gBattleStruct->choicedMove[gBattlerAttacker] == MOVE_UNVAILABLE))
					{
						if (gBattleMoves[gChosenMove].effect == EFFECT_BATON_PASS && !(gMoveResultFlags & MOVE_RESULT_FAILED))
							break;
						gBattleStruct->choicedMove[gBattlerAttacker] = gChosenMove;
					}
					for (i = 0; i < MAX_MON_MOVES; i++)
					{
						if (gBattleMons[gBattlerAttacker].moves[i] == gBattleStruct->choicedMove[gBattlerAttacker])
							break;
					}
					if (i == MAX_MON_MOVES)
						gBattleStruct->choicedMove[gBattlerAttacker] = MOVE_NONE;
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_ATTACKER_INVISIBLE: // Make attacker sprite invisible
			    ++gBattleScripting.atk49_state;
				
			    if (gStatuses3[gBattlerAttacker] & STATUS3_SEMI_INVULNERABLE && !IsBattleAnimationsOn())
				{
					BtlController_EmitSpriteInvisibility(gBattlerAttacker, BUFFER_A, TRUE);
					MarkBattlerForControllerExec(gBattlerAttacker);
					return;
				}
				break;
			case ATK49_ATTACKER_VISIBLE: // Make attacker sprite visible
			    ++gBattleScripting.atk49_state;
				
			    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT || !(gStatuses3[gBattlerAttacker] & STATUS3_SEMI_INVULNERABLE) || WasUnableToUseMove(gBattlerAttacker))
				{
					BtlController_EmitSpriteInvisibility(gBattlerAttacker, BUFFER_A, FALSE);
					MarkBattlerForControllerExec(gBattlerAttacker);
					gStatuses3[gBattlerAttacker] &= ~(STATUS3_SEMI_INVULNERABLE);
					gSpecialStatuses[gBattlerAttacker].restoredBattlerSprite = 1;
					return;
				}
				break;
			case ATK49_TARGET_VISIBLE: // Make target sprite visible
			    ++gBattleScripting.atk49_state;
				
				if (!gSpecialStatuses[gBattlerTarget].restoredBattlerSprite && gBattlerTarget < gBattlersCount && !(gStatuses3[gBattlerTarget] & STATUS3_SEMI_INVULNERABLE))
				{
					BtlController_EmitSpriteInvisibility(gBattlerTarget, BUFFER_A, FALSE);
					MarkBattlerForControllerExec(gBattlerTarget);
					gStatuses3[gBattlerTarget] &= ~(STATUS3_SEMI_INVULNERABLE);
					return;
				}
				break;
			case ATK49_ITEM_EFFECTS_ALL: // Item effects for all battlers
			    if (ItemBattleEffects(ITEMEFFECT_MOVE_END, 0, FALSE))
					effect = TRUE; // It loops through all battlers, so we increment after its done with all battlers
				else
					++gBattleScripting.atk49_state;
				break;
			case ATK49_KINGSROCK_SHELLBELL: // King's Rock and Shell Bell
                if (state != ATK49_MOVEEND_FUTURE_ATTACK && ItemBattleEffects(ITEMEFFECT_KINGSROCK_SHELLBELL, gBattlerAttacker, FALSE))
					effect = TRUE;
				++gBattleScripting.atk49_state;
				break;
			case ATK49_SUBSTITUTE: // Update Substitute
                for (i = 0; i < gBattlersCount; ++i)
				{
					if (gDisableStructs[i].substituteHP == 0)
						gBattleMons[i].status2 &= ~(STATUS2_SUBSTITUTE);
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_UPDATE_LAST_MOVES: // Update vars with the last used move
			    if (state != ATK49_MOVEEND_FUTURE_ATTACK)
				{
					if (gHitMarker & HITMARKER_SWAP_ATTACKER_TARGET)
					{
						gHitMarker &= ~(HITMARKER_SWAP_ATTACKER_TARGET);
						SWAP(gBattlerAttacker, gBattlerTarget, i);
					}
					if (gHitMarker & HITMARKER_ATTACKSTRING_PRINTED)
					{
						gLastPrintedMoves[gBattlerAttacker] = gChosenMove;
						gLastUsedMovesTypes[gBattlerAttacker] = gBattleStruct->dynamicMoveType;
					}
					
					if (!(gAbsentBattlerFlags & gBitTable[gBattlerAttacker]) && !(gBattleStruct->absentBattlerFlags & gBitTable[gBattlerAttacker])
					&& gBattleMoves[originallyUsedMove].effect != EFFECT_BATON_PASS)
					{
						if (gHitMarker & HITMARKER_OBEYS)
						{
							gLastMoves[gBattlerAttacker] = gChosenMove;
							gLastResultingMoves[gBattlerAttacker] = gCurrentMove;
						}
						else
						{
							gLastMoves[gBattlerAttacker] = MOVE_UNVAILABLE;
							gLastResultingMoves[gBattlerAttacker] = MOVE_UNVAILABLE;
						}
						
						if (gHitMarker & HITMARKER_OBEYS && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
						{
							if (gChosenMove == MOVE_UNVAILABLE)
								gLastLandedMoves[gBattlerTarget] = gChosenMove;
							else
							{
								gLastLandedMoves[gBattlerTarget] = gCurrentMove;
								gLastHitByType[gBattlerTarget] = gBattleStruct->dynamicMoveType;
							}
						}
						else
							gLastLandedMoves[gBattlerTarget] = MOVE_UNVAILABLE;
						
						if (!(gHitMarker & HITMARKER_FAINTED(gBattlerTarget)))
							gLastHitBy[gBattlerTarget] = gBattlerAttacker;
					}
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_MIRROR_MOVE: // Mirror Move
			    if (state != ATK49_MOVEEND_FUTURE_ATTACK && !(gAbsentBattlerFlags & gBitTable[gBattlerAttacker])
				&& !(gBattleStruct->absentBattlerFlags & gBitTable[gBattlerAttacker]) && originallyUsedMove && !gBattleMoves[originallyUsedMove].flags.forbiddenMirrorMove
			    && gHitMarker & HITMARKER_OBEYS && gBattlerAttacker != gBattlerTarget && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && !(gHitMarker & HITMARKER_FAINTED(gBattlerTarget)))
				{
					gBattleStruct->lastTakenMove[gBattlerTarget] = gChosenMove;
					gBattleStruct->lastTakenMoveFrom[gBattlerTarget][gBattlerAttacker] = gChosenMove;
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_NEXT_TARGET: // For moves with MOVE_TARGET_BOTH and MOVE_TARGET_FOES_AND_ALLY.
			    gBattleStruct->targetsDone[gBattlerAttacker] |= gBitTable[gBattlerTarget];
				
			    if (!(gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE) && gBattleTypeFlags & BATTLE_TYPE_DOUBLE && !gProtectStructs[gBattlerAttacker].chargingTurn)
				{
					u8 nextTarget, moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove);
					
					if (moveTarget == MOVE_TARGET_BOTH || moveTarget == MOVE_TARGET_FOES_AND_ALLY)
					{
						nextTarget = GetNextTarget(moveTarget);
						
						if (nextTarget != MAX_BATTLERS_COUNT)
						{
							SetNextTarget(nextTarget);
							return;
						}
					}
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_MULTIHIT_MOVE: // For multi-hit moves
			    if (state != ATK49_MOVEEND_FUTURE_ATTACK && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && !(gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE)
				&& gMultiHitCounter && !(gBattleMoves[gCurrentMove].effect == EFFECT_PRESENT && !gBattleStruct->presentBasePower)) // Silly edge case
				{
					++gBattleScripting.multihitString[4];
					
					if (--gMultiHitCounter == 0)
					{
						BattleScriptCall(BattleScript_MultiHitPrintStrings);
						effect = TRUE;
					}
					else
					{
						if (IsBattlerAlive(gBattlerAttacker) && IsBattlerAlive(gBattlerTarget) && (gChosenMove == MOVE_SLEEP_TALK || !(gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP))
						&& !(gBattleMons[gBattlerAttacker].status1 & STATUS1_FREEZE))
						{
							if (gSpecialStatuses[gBattlerAttacker].parentalBondState)
								--gSpecialStatuses[gBattlerAttacker].parentalBondState;
							
							gHitMarker |= (HITMARKER_NO_PPDEDUCT);
							SetNextTarget(gBattlerTarget); // Same target as now, uses the function to set proper variables
							gSpecialStatuses[gBattlerTarget].sturdied = FALSE;
							gSpecialStatuses[gBattlerTarget].focusBanded = FALSE;
							gSpecialStatuses[gBattlerAttacker].multiHitOn = TRUE;
							return;
						}
						else
						{
							BattleScriptCall(BattleScript_MultiHitPrintStrings);
							effect = TRUE;
						}
					}
				}
				gMultiHitCounter = 0;
				gSpecialStatuses[gBattlerAttacker].parentalBondState = PARENTAL_BOND_OFF;
				gSpecialStatuses[gBattlerAttacker].multiHitOn = FALSE;
				++gBattleScripting.atk49_state;
				break;
			case ATK49_DEFROST: // Defrosting check
			    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(gBattlerTarget) && gBattleMons[gBattlerTarget].status1 & STATUS1_FREEZE
				&& IsBattlerAlive(gBattlerTarget) && gBattlerAttacker != gBattlerTarget && gBattleStruct->dynamicMoveType == TYPE_FIRE)
				{
					ClearBattlerStatus(gBattlerTarget);
					PrepareMonNickWithPrefixBuffer(gBattleTextBuff1, gBattlerTarget, gBattlerPartyIndexes[gBattlerTarget]);
					BattleScriptCall(BattleScript_DefrostedViaFireMove);
					effect = TRUE;
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_RECOIL: // Recoil damage from move
			    if (state != ATK49_MOVEEND_FUTURE_ATTACK && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && !(gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE)
				&& IsBattlerAlive(gBattlerAttacker) && gBattleScripting.savedDmg)
				{
					switch (gBattleMoves[gCurrentMove].effect)
					{
						case EFFECT_RECOIL:
						    if (GetBattlerAbility(gBattlerAttacker) != ABILITY_ROCK_HEAD && GetBattlerAbility(gBattlerAttacker) != ABILITY_MAGIC_GUARD)
							{
								gBattleMoveDamage = max(1, gBattleScripting.savedDmg / gBattleMoves[gCurrentMove].flags.recoilDivisor);
								BattleScriptCall(BattleScript_MoveEffectRecoil);
								effect = TRUE;
							}
							break;
						case EFFECT_RECOIL_HP:
							gBattleMoveDamage = max(1, gBattleMons[gBattlerAttacker].maxHP / gBattleMoves[gCurrentMove].flags.recoilDivisor);
							BattleScriptCall(BattleScript_MoveEffectRecoil);
							effect = TRUE;
							break;
					}
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_MAGICIAN: // Check Magician activates
			    if (state != ATK49_MOVEEND_FUTURE_ATTACK && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && BATTLER_DAMAGED(gBattlerTarget)
				&& GetBattlerAbility(gBattlerAttacker) == ABILITY_MAGICIAN && IsBattlerAlive(gBattlerAttacker)
			    && gBattleMoves[gCurrentMove].effect != EFFECT_FLING && gBattleMoves[gCurrentMove].effect != EFFECT_NATURAL_GIFT
				&& !gBattleMons[gBattlerAttacker].item && !(gWishFutureKnock.knockedOffMons[GetBattlerSide(gBattlerTarget)] & gBitTable[gBattlerPartyIndexes[gBattlerTarget]])
				&& gBattleMons[gBattlerTarget].item && !(GetBattlerAbility(gBattlerTarget) == ABILITY_STICKY_HOLD && IsBattlerAlive(gBattlerTarget))
				&& !SubsBlockMove(gBattlerAttacker, gBattlerTarget, gCurrentMove) && CanStealItem(gBattlerAttacker, gBattlerTarget, gBattleMons[gBattlerTarget].item))
				{
					StealTargetItem(gBattlerAttacker, gBattlerTarget);
					BattleScriptCall(BattleScript_MagicianActivates);
					RecordAbilityBattle(gBattlerAttacker, GetBattlerAbility(gBattlerAttacker));
					effect = TRUE;
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_CHANGED_ITEMS: // Changed held items
                for (i = 0; i < gBattlersCount; ++i)
				{
					if (gBattleStruct->changedItems[i])
					{
						gBattleMons[i].item = gBattleStruct->changedItems[i];
						gBattleStruct->changedItems[i] = ITEM_NONE;
					}
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_PICKPOCKET: // Check Pickpocket activates
			    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && IsBattlerAlive(gBattlerAttacker) && gBattleMons[gBattlerAttacker].item
				&& !(gWishFutureKnock.knockedOffMons[GetBattlerSide(gBattlerAttacker)] & gBitTable[gBattlerPartyIndexes[gBattlerAttacker]])
				&& IsMoveMakingContact(gBattlerAttacker, gCurrentMove) && !ReceiveSheerForceBoost(gBattlerAttacker, gCurrentMove)
				&& GetBattlerAbility(gBattlerAttacker) != ABILITY_STICKY_HOLD)
				{
					u8 target, battlers[MAX_BATTLERS_COUNT] = {0, 1, 2, 3};
					
					SortBattlersBySpeed(battlers, FALSE);
					
					for (i = 0; i < gBattlersCount; ++i)
					{
						target = battlers[i];
						
						if (target != gBattlerAttacker && IsBattlerAlive(target) && GetBattlerAbility(target) == ABILITY_PICKPOCKET && BATTLER_DAMAGED(target)
						&& !gBattleMons[target].item && CanStealItem(target, gBattlerAttacker, gBattleMons[gBattlerAttacker].item)
					    && !SubsBlockMove(gBattlerAttacker, target, gCurrentMove))
						{
							gBattlerTarget = target;
							StealTargetItem(target, gBattlerAttacker);
							BattleScriptCall(BattleScript_PickpocketActivation);
							RecordAbilityBattle(target, GetBattlerAbility(target));
							effect = TRUE;
							break;
						}
					}
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_EMERGENCY_EXIT: // Check Emergency Exit
		        for (i = 0; i < gBattlersCount; ++i)
				{
					if (gBattleResources->flags->flags[i] & RESOURCE_FLAG_EMERGENCY_EXIT)
					{
						gBattleResources->flags->flags[i] &= ~(RESOURCE_FLAG_EMERGENCY_EXIT);
						gBattlerTarget = i;
						gLastUsedAbility = GetBattlerAbility(i);
						RecordAbilityBattle(i, gLastUsedAbility);
						BattleScriptCall(BattleScript_EmergencyExit);
						effect = TRUE;
						return;
					}
				}
				++gBattleScripting.atk49_state;
				break;
			case ATK49_CLEAR_BITS: // Clear bits after attack ends
				if (gBattleMoves[gCurrentMove].effect == EFFECT_RAMPAGE && (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
					&& (gBattleMons[gBattlerAttacker].status2 & STATUS2_LOCK_CONFUSE) != STATUS2_LOCK_CONFUSE_TURN(1))
					CancelMultiTurnMoves(gBattlerAttacker);
				gProtectStructs[gBattlerAttacker].usesBouncedMove = FALSE;
				++gBattleScripting.atk49_state;
				break;
		}
		
		if ((state == ATK49_MOVEEND_CASE && !effect) || (state == ATK49_MOVEEND_TO && gBattleScripting.atk49_state == lastCase))
			gBattleScripting.atk49_state = ATK49_COUNT;
		
	} while (gBattleScripting.atk49_state != ATK49_COUNT && !effect);
	
	if (gBattleScripting.atk49_state == ATK49_COUNT && !effect)
        gBattlescriptCurrInstr += 3;
}

static void atk4A_formchange(void)
{
	u8 battlerId;
	
	if (!gBattleControllerExecFlags)
	{
		battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
		
		if (!IsBattlerAlive(battlerId) || gBattleMons[battlerId].species != READ_16(gBattlescriptCurrInstr + 2))
			gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 9);
		else
		{
			DoBattleFormChange(battlerId, READ_16(gBattlescriptCurrInstr + 4), gBattlescriptCurrInstr[6], gBattlescriptCurrInstr[7], gBattlescriptCurrInstr[8]);
			gBattlescriptCurrInstr += 13;
		}
	}
}

static void atk4B_returnatktoball(void)
{
    if (!(gHitMarker & HITMARKER_FAINTED(gBattlerAttacker)))
    {
        BtlController_EmitReturnMonToBall(gBattlerAttacker, BUFFER_A, 0);
        MarkBattlerForControllerExec(gBattlerAttacker);
    }
    ++gBattlescriptCurrInstr;
}

static void atk4C_getswitchedmondata(void)
{
    if (!gBattleControllerExecFlags)
    {
        u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
		
        gBattlerPartyIndexes[battlerId] = gBattleStruct->monToSwitchIntoId[battlerId];
        BtlController_EmitGetMonData(battlerId, BUFFER_A, REQUEST_ALL_BATTLE, gBitTable[gBattlerPartyIndexes[battlerId]]);
        MarkBattlerForControllerExec(battlerId);
        gBattlescriptCurrInstr += 2;
    }
}

static void atk4D_switchindataupdate(void)
{
    struct BattlePokemon oldData;
    u8 i, battlerId;

    if (!gBattleControllerExecFlags)
    {
        battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
        oldData = gBattleMons[battlerId];
		memcpy(&gBattleMons[battlerId], &gBattleBufferB[battlerId][4], sizeof(struct BattlePokemon));
		SetBattlerInitialTypes(battlerId);
        gBattleMons[battlerId].ability = GetAbilityBySpecies(gBattleMons[battlerId].species, gBattleMons[battlerId].abilityNum, gBattleMons[battlerId].abilityHidden);
        // check knocked off item
        if (gWishFutureKnock.knockedOffMons[GetBattlerSide(battlerId)] & gBitTable[gBattlerPartyIndexes[battlerId]])
            gBattleMons[battlerId].item = ITEM_NONE;
		
        if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
        {
            for (i = 0; i < NUM_BATTLE_STATS; ++i)
                gBattleMons[battlerId].statStages[i] = oldData.statStages[i];
			
            gBattleMons[battlerId].status2 = oldData.status2;
        }
        SwitchInClearSetData(battlerId);
        gBattleScripting.battler = battlerId;
        PrepareMonNickBuffer(gBattleTextBuff1, battlerId, gBattlerPartyIndexes[battlerId]);
        gBattlescriptCurrInstr += 2;
    }
}

static void atk4E_switchinanim(void)
{
    if (!gBattleControllerExecFlags)
    {
        u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	    
        if (GetBattlerSide(battlerId) == B_SIDE_OPPONENT && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_OLD_MAN_TUTORIAL
		| BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_GHOST)))
            HandleSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[battlerId].species), FLAG_SET_SEEN, gBattleMons[battlerId].personality);
			
        gAbsentBattlerFlags &= ~(gBitTable[battlerId]);
        BtlController_EmitSwitchInAnim(battlerId, BUFFER_A, gBattlerPartyIndexes[battlerId], gBattlescriptCurrInstr[2]);
        MarkBattlerForControllerExec(battlerId);
        gBattlescriptCurrInstr += 3;
    }
}

static void atk4F_jumpifcantswitch(void)
{
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1] & ~(ATK4F_DONT_CHECK_STATUSES));
	
	if ((!(gBattlescriptCurrInstr[1] & ATK4F_DONT_CHECK_STATUSES) && !CanBattlerEscape(battlerId, TRUE)) || !CanBattlerSwitch(battlerId))
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 2);
	else
		gBattlescriptCurrInstr += 6;
}

// Opens the party screen to choose a new Pokémon to send out.
// slotId is the Pokémon to replace.
// Note that this is not used by the Switch action, only replacing fainted Pokémon or Baton Pass
static void ChooseMonToSendOut(u8 battlerId, u8 slotId)
{
    *(gBattleStruct->battlerPartyIndexes + battlerId) = gBattlerPartyIndexes[battlerId];
    BtlController_EmitChoosePokemon(battlerId, BUFFER_A, PARTY_ACTION_SEND_OUT, slotId, 0, gBattleStruct->battlerPartyOrders[battlerId]);
    MarkBattlerForControllerExec(battlerId);
}

static void atk50_openpartyscreen(void)
{
    u8 i, flag40_0, flag40_1, flag40_2, flag40_3, hitmarkerFaintBits, battlerId;
    const u8 *jumpPtr = READ_PTR(gBattlescriptCurrInstr + 2);
    u32 flags = 0;

    if (gBattlescriptCurrInstr[1] == 5)
    {
        if ((gBattleTypeFlags & (BATTLE_TYPE_DOUBLE | BATTLE_TYPE_MULTI)) != BATTLE_TYPE_DOUBLE)
        {
            for (battlerId = 0; battlerId < gBattlersCount; ++battlerId)
            {
                if (gHitMarker & HITMARKER_FAINTED(battlerId))
                {
                    if (HasNoMonsToSwitch(battlerId, PARTY_SIZE, PARTY_SIZE))
                    {
                        gAbsentBattlerFlags |= gBitTable[battlerId];
                        gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
                        BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                        MarkBattlerForControllerExec(battlerId);
                    }
                    else if (!gSpecialStatuses[battlerId].faintedHasReplacement)
                    {
                        ChooseMonToSendOut(battlerId, PARTY_SIZE);
                        gSpecialStatuses[battlerId].faintedHasReplacement = 1;
                    }
                }
                else
                {
                    BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                    MarkBattlerForControllerExec(battlerId);
                }
            }
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        {
            hitmarkerFaintBits = gHitMarker >> 0x1C;
		
            if (gBitTable[0] & hitmarkerFaintBits)
            {
                battlerId = 0;
		    
                if (HasNoMonsToSwitch(battlerId, PARTY_SIZE, PARTY_SIZE))
                {
                    gAbsentBattlerFlags |= gBitTable[battlerId];
                    gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
                }
                else if (!gSpecialStatuses[battlerId].faintedHasReplacement)
                {
                    ChooseMonToSendOut(battlerId, gBattleStruct->monToSwitchIntoId[2]);
                    gSpecialStatuses[battlerId].faintedHasReplacement = 1;
                }
                else
                {
                    BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                    MarkBattlerForControllerExec(battlerId);
                    flags |= 1;
                }
            }
            if (gBitTable[2] & hitmarkerFaintBits && !(gBitTable[0] & hitmarkerFaintBits))
            {
                battlerId = 2;
		    
                if (HasNoMonsToSwitch(battlerId, PARTY_SIZE, PARTY_SIZE))
                {
                    gAbsentBattlerFlags |= gBitTable[battlerId];
                    gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
                }
                else if (!gSpecialStatuses[battlerId].faintedHasReplacement)
                {
                    ChooseMonToSendOut(battlerId, gBattleStruct->monToSwitchIntoId[0]);
                    gSpecialStatuses[battlerId].faintedHasReplacement = 1;
                }
                else if (!(flags & 1))
                {
                    BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                    MarkBattlerForControllerExec(battlerId);
                }
            }
            if (gBitTable[1] & hitmarkerFaintBits)
            {
                battlerId = 1;
		    
                if (HasNoMonsToSwitch(battlerId, PARTY_SIZE, PARTY_SIZE))
                {
                    gAbsentBattlerFlags |= gBitTable[battlerId];
                    gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
                }
                else if (!gSpecialStatuses[battlerId].faintedHasReplacement)
                {
                    ChooseMonToSendOut(battlerId, gBattleStruct->monToSwitchIntoId[3]);
                    gSpecialStatuses[battlerId].faintedHasReplacement = 1;
                }
                else
                {
                    BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                    MarkBattlerForControllerExec(battlerId);
                    flags |= 2;
                }
            }
            if (gBitTable[3] & hitmarkerFaintBits && !(gBitTable[1] & hitmarkerFaintBits))
            {
                battlerId = 3;
		    
                if (HasNoMonsToSwitch(battlerId, PARTY_SIZE, PARTY_SIZE))
                {
                    gAbsentBattlerFlags |= gBitTable[battlerId];
                    gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
                }
                else if (!gSpecialStatuses[battlerId].faintedHasReplacement)
                {
                    ChooseMonToSendOut(battlerId, gBattleStruct->monToSwitchIntoId[1]);
                    gSpecialStatuses[battlerId].faintedHasReplacement = 1;
                }
                else if (!(flags & 2))
                {
                    BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                    MarkBattlerForControllerExec(battlerId);
                }
            }
            flag40_0 = gSpecialStatuses[0].faintedHasReplacement;
		
            if (!flag40_0)
            {
                flag40_2 = gSpecialStatuses[2].faintedHasReplacement;
		    
                if (!flag40_2 && hitmarkerFaintBits != 0)
                {
                    if (gAbsentBattlerFlags & gBitTable[0])
                        battlerId = 2;
                    else
                        battlerId = 0;

                    BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                    MarkBattlerForControllerExec(battlerId);
                }

            }
            flag40_1 = gSpecialStatuses[1].faintedHasReplacement;
		
            if (!flag40_1)
            {
                flag40_3 = gSpecialStatuses[3].faintedHasReplacement;
		    
                if (!flag40_3 && hitmarkerFaintBits != 0)
                {
                    if (gAbsentBattlerFlags & gBitTable[1])
                        battlerId = 3;
                    else
                        battlerId = 1;
					
                    BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                    MarkBattlerForControllerExec(battlerId);
                }
            }
        }
        gBattlescriptCurrInstr += 6;
    }
    else if (gBattlescriptCurrInstr[1] == 6)
    {
        if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            {
                hitmarkerFaintBits = gHitMarker >> 0x1C;
		    
                if (gBitTable[2] & hitmarkerFaintBits && gBitTable[0] & hitmarkerFaintBits)
                {
                    battlerId = 2;
			
                    if (HasNoMonsToSwitch(battlerId, gBattleBufferB[0][1], PARTY_SIZE))
                    {
                        gAbsentBattlerFlags |= gBitTable[battlerId];
                        gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
                    }
                    else if (!gSpecialStatuses[battlerId].faintedHasReplacement)
                    {
                        ChooseMonToSendOut(battlerId, gBattleStruct->monToSwitchIntoId[0]);
                        gSpecialStatuses[battlerId].faintedHasReplacement = 1;
                    }
                }
                if (gBitTable[3] & hitmarkerFaintBits && hitmarkerFaintBits & gBitTable[1])
                {
                    battlerId = 3;
			
                    if (HasNoMonsToSwitch(battlerId, gBattleBufferB[1][1], 6))
                    {
                        gAbsentBattlerFlags |= gBitTable[battlerId];
                        gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
                    }
                    else if (!gSpecialStatuses[battlerId].faintedHasReplacement)
                    {
                        ChooseMonToSendOut(battlerId, gBattleStruct->monToSwitchIntoId[1]);
                        gSpecialStatuses[battlerId].faintedHasReplacement = 1;
                    }
                }
                gBattlescriptCurrInstr += 6;
            }
            else
                gBattlescriptCurrInstr += 6;
        }
        else
            gBattlescriptCurrInstr += 6;

        hitmarkerFaintBits = gHitMarker >> 0x1C;
        gBattlerFainted = 0;
	    
        while (1)
        {
            if (gBitTable[gBattlerFainted] & hitmarkerFaintBits || gBattlerFainted >= gBattlersCount)
                break;
            ++gBattlerFainted;
        }

        if (gBattlerFainted == gBattlersCount)
            gBattlescriptCurrInstr = jumpPtr;
    }
    else
    {
        if (gBattlescriptCurrInstr[1] & OPEN_PARTY_ALLOW_CANCEL)
            hitmarkerFaintBits = PARTY_ACTION_CHOOSE_MON; // Used here as the caseId for the EmitChoose function.
        else
            hitmarkerFaintBits = PARTY_ACTION_SEND_OUT;
		
        battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1] & ~(OPEN_PARTY_ALLOW_CANCEL));
	    
        if (gSpecialStatuses[battlerId].faintedHasReplacement)
            gBattlescriptCurrInstr += 6;
        else if (HasNoMonsToSwitch(battlerId, PARTY_SIZE, PARTY_SIZE))
        {
            gAbsentBattlerFlags |= gBitTable[battlerId];
            gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
            gBattlescriptCurrInstr = jumpPtr;
        }
        else
        {
            *(gBattleStruct->battlerPartyIndexes + battlerId) = gBattlerPartyIndexes[battlerId];
            BtlController_EmitChoosePokemon(battlerId, BUFFER_A, hitmarkerFaintBits, gBattleStruct->monToSwitchIntoId[BATTLE_PARTNER(battlerId)], 0, gBattleStruct->battlerPartyOrders[battlerId]);
            MarkBattlerForControllerExec(battlerId);
            gBattlescriptCurrInstr += 6;

            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                for (i = 0; i < gBattlersCount; ++i)
                {
                    if (i != battlerId)
                    {
                        BtlController_EmitLinkStandbyMsg(i, BUFFER_A, 2);
                        MarkBattlerForControllerExec(i);
                    }
                }
            }
            else
            {
                battlerId = GetBattlerAtPosition(BATTLE_OPPOSITE(GetBattlerPosition(battlerId)));
		    
                if (gAbsentBattlerFlags & gBitTable[battlerId])
                    battlerId = BATTLE_PARTNER(battlerId);
                BtlController_EmitLinkStandbyMsg(battlerId, BUFFER_A, 2);
                MarkBattlerForControllerExec(battlerId);
            }
        }
    }
}

static void atk51_switchhandleorder(void)
{
    u8 i, battlerId;

    if (!gBattleControllerExecFlags)
    {
        battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	    
        switch (gBattlescriptCurrInstr[2])
        {
			case 0:
                for (i = 0; i < gBattlersCount; ++i)
				{
					if (gBattleBufferB[i][0] == 0x22)
						gBattleStruct->monToSwitchIntoId[i] = gBattleBufferB[i][1];
				}
				break;
			case 1:
                if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
					UpdatePartyOwnerOnSwitch_NonMulti(battlerId);
				break;
			case 2:
                gBattleCommunication[MULTIUSE_STATE] = gBattleBufferB[battlerId][1];
                gBattleStruct->monToSwitchIntoId[battlerId] = gBattleBufferB[battlerId][1];
				
				if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
				{
					*(battlerId * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) &= 0xF;
					*(battlerId * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) |= (gBattleBufferB[battlerId][2] & 0xF0);
					*(battlerId * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 1) = gBattleBufferB[battlerId][3];
					*((BATTLE_PARTNER(battlerId)) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) &= (0xF0);
					*((BATTLE_PARTNER(battlerId)) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 0) |= (gBattleBufferB[battlerId][2] & 0xF0) >> 4;
					*((BATTLE_PARTNER(battlerId)) * 3 + (u8 *)(gBattleStruct->battlerPartyOrders) + 2) = gBattleBufferB[battlerId][3];
				}
				else
					UpdatePartyOwnerOnSwitch_NonMulti(battlerId);
				
				PrepareSpeciesBuffer(gBattleTextBuff1, gBattleMons[gBattlerAttacker].species);
				PrepareMonNickBuffer(gBattleTextBuff2, battlerId, gBattleBufferB[battlerId][1]);
				break;
        }
        gBattlescriptCurrInstr += 3;
    }
}

static bool8 ShouldPostponeSwitchInAbilities(u8 battlerId)
{
	u8 battler1 = BATTLE_OPPOSITE(battlerId), battler2 = BATTLE_PARTNER(BATTLE_OPPOSITE(battlerId));
	bool8 aliveFirstMon = IsBattlerAlive(battler1);
	bool8 aliveSecondMon = IsBattlerAlive(battler2);
	
	// No pokemon on opposing side - postopone.
    if (!aliveFirstMon && !aliveSecondMon)
        return TRUE;
	
	// Checks for double battle, so abilities like Intimidate wait until all battlers are switched-in before activating.
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        if (aliveFirstMon && !aliveSecondMon && !HasNoMonsToSwitch(battler1, PARTY_SIZE, PARTY_SIZE))
            return TRUE;
        if (!aliveFirstMon && aliveSecondMon && !HasNoMonsToSwitch(battler2, PARTY_SIZE, PARTY_SIZE))
            return TRUE;
    }
    return FALSE;
}

static void atk52_switchineffects(void)
{
    u8 spikesDmg, i, battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	u16 battlerAbility = GetBattlerAbility(battlerId);
    u32 hitmarkerFaintBits;
	
    UpdateSentPokesToOpponentValue(battlerId);
    gHitMarker &= ~(HITMARKER_FAINTED(battlerId));
    gSpecialStatuses[battlerId].faintedHasReplacement = 0;
	
	if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
		gBattleStruct->appearedInBattle |= gBitTable[gBattlerPartyIndexes[battlerId]];
	
	// Neutralizing Gas announces itself before hazards
	if (!gSpecialStatuses[battlerId].announceNeutralizingGas && battlerAbility == ABILITY_NEUTRALIZING_GAS)
	{
		gSpecialStatuses[battlerId].announceNeutralizingGas = TRUE;
		gBattleScripting.battler = battlerId;
		BattleScriptCall(BattleScript_NeutralizingGasActivatesRet);
	}
	// Unnerve and As One too
	else if (!gSpecialStatuses[battlerId].announceUnnerve && (battlerAbility == ABILITY_UNNERVE || battlerAbility == ABILITY_AS_ONE_ICE_RIDER
	|| battlerAbility == ABILITY_AS_ONE_SHADOW_RIDER))
	{
		gSpecialStatuses[battlerId].announceUnnerve = TRUE;
		gBattleScripting.battler = battlerId;
		if (battlerAbility == ABILITY_UNNERVE)
			gBattleCommunication[MULTISTRING_CHOOSER] = 2; // Unnerve message
		else
			gBattleCommunication[MULTISTRING_CHOOSER] = 15; // As One message
		PrepareMonTeamPrefixBuffer(gBattleTextBuff1, BATTLE_OPPOSITE(battlerId));
		BattleScriptCall(BattleScript_SwitchInAbilityMsgRet);
	}
	else if (!(gSideStatuses[GetBattlerSide(battlerId)] & SIDE_STATUS_SPIKES_DAMAGED) && (gSideStatuses[GetBattlerSide(battlerId)] & SIDE_STATUS_SPIKES)
	&& IsBattlerGrounded(battlerId) && GetBattlerAbility(battlerId) != ABILITY_MAGIC_GUARD)
    {
        gSideStatuses[GetBattlerSide(battlerId)] |= SIDE_STATUS_SPIKES_DAMAGED;
        spikesDmg = (5 - gSideTimers[GetBattlerSide(battlerId)].spikesAmount) * 2;
        gBattleMoveDamage = gBattleMons[battlerId].maxHP / (spikesDmg);
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattleScripting.battler = battlerId;
        if (gBattlescriptCurrInstr[1] == BS_TARGET)
			BattleScriptCall(BattleScript_SpikesOnTarget);
        else if (gBattlescriptCurrInstr[1] == BS_ATTACKER)
            BattleScriptCall(BattleScript_SpikesOnAttacker);
        else
            BattleScriptCall(BattleScript_SpikesOnFaintedBattler);
    }
    else
    {
        if (battlerAbility == ABILITY_TRUANT)
            gDisableStructs[battlerId].truantCounter = 1;
		
		// Don't activate switch-in abilities if the opposing field is empty.
        // This could happen when a mon uses explosion and causes everyone to faint.
        if (ShouldPostponeSwitchInAbilities(battlerId) || gBattleStruct->switchInAbilityPostponed)
            gBattleStruct->switchInAbilityPostponed |= gBitTable[battlerId];
        else if (DoSwitchInAbilitiesItems(battlerId))
			return;
		
		gSideStatuses[GetBattlerSide(battlerId)] &= ~(SIDE_STATUS_SPIKES_DAMAGED);
		
		for (i = 0; i < gBattlersCount; ++i)
		{
			if (gBattlerByTurnOrder[i] == battlerId)
				gActionsByTurnOrder[i] = B_ACTION_CANCEL_PARTNER;
			
			gBattleStruct->hpOnSwitchout[GetBattlerSide(i)] = gBattleMons[i].hp;
		}
		if (gBattlescriptCurrInstr[1] == 5)
		{
			hitmarkerFaintBits = gHitMarker >> 0x1C;
			++gBattlerFainted;
			
			while (TRUE)
			{
				if ((hitmarkerFaintBits & gBitTable[gBattlerFainted] && !(gAbsentBattlerFlags & gBitTable[gBattlerFainted])) || gBattlerFainted >= gBattlersCount)
					break;
				++gBattlerFainted;
			}
		}
		gBattlescriptCurrInstr += 2;
    }
}

static void atk53_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk54_playse(void)
{
    BtlController_EmitPlaySE(gBattlerAttacker, BUFFER_A, READ_16(gBattlescriptCurrInstr + 1));
    MarkBattlerForControllerExec(gBattlerAttacker);
    gBattlescriptCurrInstr += 3;
}

static void atk55_fanfare(void)
{
    BtlController_EmitPlayFanfare(gBattlerAttacker, BUFFER_A, READ_16(gBattlescriptCurrInstr + 1));
    MarkBattlerForControllerExec(gBattlerAttacker);
    gBattlescriptCurrInstr += 3;
}

static void atk56_playfaintcry(void)
{
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    BtlController_EmitFaintingCry(battlerId, BUFFER_A);
    MarkBattlerForControllerExec(battlerId);
    gBattlescriptCurrInstr += 2;
}

static void atk57_endlinkbattle(void)
{
    u8 battlerId = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    BtlController_EmitEndLinkBattle(battlerId, BUFFER_A, gBattleOutcome);
    MarkBattlerForControllerExec(battlerId);
    ++gBattlescriptCurrInstr;
}

static void atk58_returntoball(void)
{
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	BtlController_EmitReturnMonToBall(battlerId, BUFFER_A, 1);
	MarkBattlerForControllerExec(battlerId);
    gBattlescriptCurrInstr += 2;
}

static void SetBattleMonMoveSlot(struct BattlePokemon *mon, u16 move, u8 slot)
{
    mon->moves[slot] = move;
    mon->pp[slot] = gBattleMoves[move].pp;
}

static void GiveMoveToBattleMon(struct BattlePokemon *mon, u16 move)
{
    u8 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (!mon->moves[i])
        {
			SetBattleMonMoveSlot(mon, move, i);
            return;
        }
    }
}

static void atk59_handlelearnnewmove(void)
{
    u16 ret = MonTryLearningNewMove(&gPlayerParty[gBattleStruct->expGetterMonId], gBattlescriptCurrInstr[9]);
    
    while (ret == 0xFFFE)
        ret = MonTryLearningNewMove(&gPlayerParty[gBattleStruct->expGetterMonId], FALSE);
	
    if (!ret)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 5);
    else if (ret == 0xFFFF)
        gBattlescriptCurrInstr += 10;
    else
    {
        u8 battlerId = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
	    
        if (gBattlerPartyIndexes[battlerId] == gBattleStruct->expGetterMonId && !(gBattleMons[battlerId].status2 & STATUS2_TRANSFORMED))
            GiveMoveToBattleMon(&gBattleMons[battlerId], ret);
		
        if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        {
            battlerId = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
		
            if (gBattlerPartyIndexes[battlerId] == gBattleStruct->expGetterMonId && !(gBattleMons[battlerId].status2 & STATUS2_TRANSFORMED))
                GiveMoveToBattleMon(&gBattleMons[battlerId], ret);
        }
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void RemoveBattleMonPPBonus(struct BattlePokemon *mon, u8 moveIndex)
{
    mon->ppBonuses &= gPPUpSetMask[moveIndex];
}

static void atk5A_yesnoboxlearnmove(void)
{
    u8 movePosition;
	
    switch (gBattleScripting.learnMoveState)
    {
    case 0:
        HandleBattleWindow(0x17, 8, 0x1D, 0xD, 0);
        BattlePutTextOnWindow(gText_BattleYesNoChoice, B_WIN_YESNO);
        gBattleCommunication[CURSOR_POSITION] = 0;
        BattleCreateYesNoCursorAt();
		++gBattleScripting.learnMoveState;
        break;
    case 1:
        if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION])
        {
            PlaySE(SE_SELECT);
            BattleDestroyYesNoCursorAt();
            gBattleCommunication[CURSOR_POSITION] = 0;
            BattleCreateYesNoCursorAt();
        }
        if (JOY_NEW(DPAD_DOWN) && !gBattleCommunication[CURSOR_POSITION])
        {
            PlaySE(SE_SELECT);
            BattleDestroyYesNoCursorAt();
            gBattleCommunication[CURSOR_POSITION] = 1;
            BattleCreateYesNoCursorAt();
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (!gBattleCommunication[CURSOR_POSITION])
            {
                HandleBattleWindow(0x17, 0x8, 0x1D, 0xD, WINDOW_CLEAR);
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
                ++gBattleScripting.learnMoveState;
            }
            else
                gBattleScripting.learnMoveState = 4;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBattleScripting.learnMoveState = 4;
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            FreeAllWindowBuffers();
            ShowSelectMovePokemonSummaryScreen(gPlayerParty, gBattleStruct->expGetterMonId, gPlayerPartyCount - 1, ReshowBattleScreenAfterMenu, gMoveToLearn);
            ++gBattleScripting.learnMoveState;
        }
        break;
    case 3:
        if (!gPaletteFade.active && gMain.callback2 == BattleMainCB2)
        {
            movePosition = GetMoveSlotToReplace();

            if (movePosition == MAX_MON_MOVES)
                gBattleScripting.learnMoveState = 4;
            else
            {
                gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
				PrepareMoveBuffer(gBattleTextBuff2, GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_MOVE1 + movePosition));
				RemoveMonPPBonus(&gPlayerParty[gBattleStruct->expGetterMonId], movePosition);
				SetMonMoveSlot(&gPlayerParty[gBattleStruct->expGetterMonId], gMoveToLearn, movePosition);
				
				if (gBattlerPartyIndexes[0] == gBattleStruct->expGetterMonId && !(gBattleMons[0].status2 & STATUS2_TRANSFORMED)
				&& !(gDisableStructs[0].mimickedMoves & gBitTable[movePosition]))
				{
					RemoveBattleMonPPBonus(&gBattleMons[0], movePosition);
					SetBattleMonMoveSlot(&gBattleMons[0], gMoveToLearn, movePosition);
				}
				if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && gBattlerPartyIndexes[2] == gBattleStruct->expGetterMonId
				&& !(gBattleMons[2].status2 & STATUS2_TRANSFORMED) && !(gDisableStructs[2].mimickedMoves & gBitTable[movePosition]))
				{
					RemoveBattleMonPPBonus(&gBattleMons[2], movePosition);
					SetBattleMonMoveSlot(&gBattleMons[2], gMoveToLearn, movePosition);
				}
            }
        }
        break;
    case 4:
        HandleBattleWindow(0x17, 8, 0x1D, 0xD, WINDOW_CLEAR);
        gBattlescriptCurrInstr += 5;
        break;
    case 5:
        if (!gBattleControllerExecFlags)
            gBattleScripting.learnMoveState = 2;
        break;
    }
}

static void atk5B_yesnoboxstoplearningmove(void)
{
    switch (gBattleScripting.learnMoveState)
    {
    case 0:
        HandleBattleWindow(0x17, 8, 0x1D, 0xD, 0);
        BattlePutTextOnWindow(gText_BattleYesNoChoice, B_WIN_YESNO);
        gBattleCommunication[CURSOR_POSITION] = 0;
        BattleCreateYesNoCursorAt();
		++gBattleScripting.learnMoveState;
        break;
    case 1:
        if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION])
        {
            PlaySE(SE_SELECT);
            BattleDestroyYesNoCursorAt();
            gBattleCommunication[CURSOR_POSITION] = 0;
            BattleCreateYesNoCursorAt();
        }
        if (JOY_NEW(DPAD_DOWN) && !gBattleCommunication[CURSOR_POSITION])
        {
            PlaySE(SE_SELECT);
            BattleDestroyYesNoCursorAt();
            gBattleCommunication[CURSOR_POSITION] = 1;
            BattleCreateYesNoCursorAt();
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);

            if (gBattleCommunication[CURSOR_POSITION])
                gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
            else
                gBattlescriptCurrInstr += 5;
			
            HandleBattleWindow(0x17, 0x8, 0x1D, 0xD, WINDOW_CLEAR);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
            HandleBattleWindow(0x17, 0x8, 0x1D, 0xD, WINDOW_CLEAR);
        }
        break;
    }
}

static void atk5C_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk5D_getmoneyreward(void)
{ 
    u8 lastMonLevel;
    u32 moneyReward, i;

    if (gBattleOutcome == B_OUTCOME_WON)
    {
		for (i = 0; gTrainerMoneyAndBallTable[i].classId != 0xFF; i++)
		{
			if (gTrainerMoneyAndBallTable[i].classId == gTrainers[gTrainerBattleOpponent_A].trainerClass)
				break;
		}
		lastMonLevel = GetTrainerPartyMonLevel(gTrainers[gTrainerBattleOpponent_A].party[gTrainers[gTrainerBattleOpponent_A].partySize - 1]);
		moneyReward = 4 * lastMonLevel * gBattleStruct->moneyMultiplier * (gBattleTypeFlags & BATTLE_TYPE_DOUBLE ? 2 : 1) * gTrainerMoneyAndBallTable[i].value;
        AddMoney(&gSaveBlock1Ptr->money, moneyReward);
    }
    else
        moneyReward = ComputeWhiteOutMoneyLoss();
	
    PrepareWordNumberBuffer(gBattleTextBuff1, 5, moneyReward);
	
    if (moneyReward)
        gBattlescriptCurrInstr += 5;
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atk5E_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk5F_swapattackerwithtarget(void)
{
	u8 i;
	SWAP(gBattlerAttacker, gBattlerTarget, i);
	gHitMarker ^= HITMARKER_SWAP_ATTACKER_TARGET;
    ++gBattlescriptCurrInstr;
}

static void atk60_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk61_drawpartystatussummary(void)
{
    u8 i, battlerId;
    struct Pokemon *party;
    struct HpAndStatus hpStatuses[PARTY_SIZE];

    if (!gBattleControllerExecFlags)
    {
        battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
		party = GetBattlerParty(battlerId);

        for (i = 0; i < PARTY_SIZE; ++i)
        {
            if (!IsMonValidSpecies(&party[i]))
            {
                hpStatuses[i].hp = 0xFFFF;
                hpStatuses[i].status = 0;
            }
            else
            {
                hpStatuses[i].hp = GetMonData(&party[i], MON_DATA_HP);
                hpStatuses[i].status = GetMonData(&party[i], MON_DATA_STATUS);
            }
        }
        BtlController_EmitDrawPartyStatusSummary(battlerId, BUFFER_A, hpStatuses, 1);
        MarkBattlerForControllerExec(battlerId);
        gBattlescriptCurrInstr += 2;
    }
}

static void atk62_hidepartystatussummary(void)
{
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    BtlController_EmitHidePartyStatusSummary(battlerId, BUFFER_A);
    MarkBattlerForControllerExec(battlerId);
    gBattlescriptCurrInstr += 2;
}

static void CallAnotherMove(u16 move)
{
	gCurrentMove = move;
	gBattlerTarget = GetMoveTarget(move, 0);
	SetTypeBeforeUsingMove(move, gBattlerAttacker);
	gBattleStruct->atkCancellerTracker = CANCELLER_RECALL_CASEID;
	gBattleStruct->moveEffect.moveEffectByte = MOVE_EFFECT_NONE;
	gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
	gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[move].effect];
}

static void atk63_jumptocalledmove(void)
{
	if (gBattlescriptCurrInstr[1])
		gChosenMove = gCalledMove;
	
	CallAnotherMove(gCalledMove);
}

static void atk64_argumenttomoveeffect(void)
{
	SetMoveEffect(gBattleMoves[gCurrentMove].argument, FALSE, FALSE);
	++gBattlescriptCurrInstr;
}

static void atk65_argumenttomovedamage(void)
{
	gBattleMoveDamage = gBattleMoves[gCurrentMove].argument;
	++gBattlescriptCurrInstr;
}

static void atk66_chosenstatusanimation(void)
{
    if (!gBattleControllerExecFlags)
    {
        u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	    
        if (!(gStatuses3[battlerId] & STATUS3_SEMI_INVULNERABLE) && gDisableStructs[battlerId].substituteHP == 0 && IsBattleAnimationsOn())
        {
            BtlController_EmitStatusAnimation(battlerId, BUFFER_A, gBattlescriptCurrInstr[2], READ_32(gBattlescriptCurrInstr + 3));
            MarkBattlerForControllerExec(battlerId);
        }
        gBattlescriptCurrInstr += 7;
    }
}

static void atk67_yesnobox(void)
{
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
		case 0:
            HandleBattleWindow(0x17, 8, 0x1D, 0xD, 0);
			BattlePutTextOnWindow(gText_BattleYesNoChoice, B_WIN_YESNO);
			gBattleCommunication[CURSOR_POSITION] = 0;
			BattleCreateYesNoCursorAt();
			++gBattleCommunication[MULTIUSE_STATE];
			break;
		case 1:
            if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION])
			{
				PlaySE(SE_SELECT);
				BattleDestroyYesNoCursorAt();
				gBattleCommunication[CURSOR_POSITION] = 0;
				BattleCreateYesNoCursorAt();
			}
			if (JOY_NEW(DPAD_DOWN) && !gBattleCommunication[CURSOR_POSITION])
			{
				PlaySE(SE_SELECT);
				BattleDestroyYesNoCursorAt();
				gBattleCommunication[CURSOR_POSITION] = 1;
				BattleCreateYesNoCursorAt();
			}
			if (JOY_NEW(B_BUTTON))
			{
				gBattleCommunication[CURSOR_POSITION] = 1;
				PlaySE(SE_SELECT);
				HandleBattleWindow(0x17, 8, 0x1D, 0xD, WINDOW_CLEAR);
				++gBattlescriptCurrInstr;
			}
			else if (JOY_NEW(A_BUTTON))
			{
				PlaySE(SE_SELECT);
				HandleBattleWindow(0x17, 8, 0x1D, 0xD, WINDOW_CLEAR);
				++gBattlescriptCurrInstr;
			}
			break;
    }
}

static void atk68_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk69_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk6A_removeitem(void)
{
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	*GetUsedHeldItemPtr(battlerId) = gBattleMons[battlerId].item;
    gBattleMons[battlerId].item = ITEM_NONE;
	CheckSetBattlerUnburden(battlerId);
	RemoveOrAddBattlerOnPickupStack(battlerId, TRUE);
    BtlController_EmitSetMonData(battlerId, BUFFER_A, REQUEST_HELDITEM_BATTLE, 0, 2, &gBattleMons[battlerId].item);
    MarkBattlerForControllerExec(battlerId);
    gBattlescriptCurrInstr += 2;
}

static void atk6B_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk6C_drawlvlupbox(void)
{
    if (gBattleScripting.atk6C_state == 0)
		gBattleScripting.atk6C_state = IsMonGettingExpSentOut() ? 3 : 1;

    switch (gBattleScripting.atk6C_state)
    {
		case 1:
            gBattle_BG2_Y = 0x60;
			SetBgAttribute(2, BG_ATTR_PRIORITY, 0);
			ShowBg(2);
			InitLvlUpBox();
			gBattleScripting.atk6C_state = 2;
			break;
		case 2:
            if (!SlideInLvlUpBox())
				gBattleScripting.atk6C_state = 3;
			break;
		case 3:
            gBattle_BG1_X = 0;
			gBattle_BG1_Y = 0x100;
			SetBgAttribute(0, BG_ATTR_PRIORITY, 1);
			SetBgAttribute(1, BG_ATTR_PRIORITY, 0);
			ShowBg(0);
			ShowBg(1);
			HandleBattleWindow(18, 7, 0x1D, 0x13, WINDOW_x80);
			gBattleScripting.atk6C_state = 4;
			break;
		case 4:
            DrawLevelUpWindow1();
			PutWindowTilemap(12);
			CopyWindowToVram(12, COPYWIN_BOTH);
			++gBattleScripting.atk6C_state;
			break;
		case 5:
		case 7:
		    if (!IsDma3ManagerBusyWithBgCopy())
			{
				gBattle_BG1_Y = 0;
				++gBattleScripting.atk6C_state;
			}
			break;
		case 6:
            if (gMain.newKeys)
			{
				PlaySE(SE_SELECT);
				DrawLevelUpWindow2();
				CopyWindowToVram(12, COPYWIN_GFX);
				++gBattleScripting.atk6C_state;
			}
			break;
		case 8:
            if (gMain.newKeys)
			{
				PlaySE(SE_SELECT);
				HandleBattleWindow(18, 7, 0x1D, 0x13, WINDOW_x80 | WINDOW_CLEAR);
				++gBattleScripting.atk6C_state;
			}
			break;
		case 9:
            if (!SlideOutLvlUpBox())
			{
				ClearWindowTilemap(13);
				CopyWindowToVram(13, COPYWIN_MAP);
				ClearWindowTilemap(12);
				CopyWindowToVram(12, COPYWIN_MAP);
				SetBgAttribute(2, BG_ATTR_PRIORITY, 2);
				ShowBg(2);
				gBattleScripting.atk6C_state = 10;
			}
			break;
		case 10:
            if (!IsDma3ManagerBusyWithBgCopy())
			{
				SetBgAttribute(0, BG_ATTR_PRIORITY, 0);
				SetBgAttribute(1, BG_ATTR_PRIORITY, 1);
				ShowBg(0);
				ShowBg(1);
				++gBattlescriptCurrInstr;
			}
			break;
    }
}

static void DrawLevelUpWindow1(void)
{
    u16 currStats[NUM_STATS];
    GetMonLevelUpWindowStats(&gPlayerParty[gBattleStruct->expGetterMonId], currStats);
    DrawLevelUpWindowPg1(12, gBattleResources->beforeLvlUp->stats, currStats, 0xE, 0xD, 0xF);
}

static void DrawLevelUpWindow2(void)
{
    u16 currStats[NUM_STATS];
    GetMonLevelUpWindowStats(&gPlayerParty[gBattleStruct->expGetterMonId], currStats);
    DrawLevelUpWindowPg2(12, currStats, 0xE, 0xD, 0xF);
}

static void InitLvlUpBox(void)
{
    gBattle_BG2_Y = 0;
    gBattle_BG2_X = 0x1A0;
    LoadPalette(sUnknownBattleboxPal, 0x60, 0x20);
    CopyToWindowPixelBuffer(13, sUnknownBattleboxGfx, 0, 0);
    PutWindowTilemap(13);
    CopyWindowToVram(13, COPYWIN_BOTH);
    PutMonIconOnLvlUpBox();
}

static bool8 SlideInLvlUpBox(void)
{
    if (IsDma3ManagerBusyWithBgCopy())
        return TRUE;
    else if (gBattle_BG2_X == 0x200)
        return FALSE;
    else if (gBattle_BG2_X == 0x1A0)
		PutLevelAndGenderOnLvlUpBox();
	
    gBattle_BG2_X += 8;
    if (gBattle_BG2_X >= 0x200)
        gBattle_BG2_X = 0x200;
	
    return (gBattle_BG2_X != 0x200);
}

static void PutLevelAndGenderOnLvlUpBox(void)
{
    u8 monGender, *txtPtr, *txtPtr2;
    struct TextPrinterTemplate printerTemplate = {0};

    GetMonNickname(&gPlayerParty[gBattleStruct->expGetterMonId], gStringVar4);
	
    printerTemplate.currentChar = gStringVar4;
    printerTemplate.windowId = 13;
    printerTemplate.x = 32;
    printerTemplate.currentX = 32;
    printerTemplate.fgColor = TEXT_COLOR_WHITE;
    printerTemplate.bgColor = TEXT_COLOR_TRANSPARENT;
    printerTemplate.shadowColor = TEXT_COLOR_DARK_GRAY;
    AddTextPrinter(&printerTemplate, 0xFF, NULL);
    txtPtr = gStringVar4;
    gStringVar4[0] = 0xF9;
    *++txtPtr = 5;
    *++txtPtr = 0;
    txtPtr2 = txtPtr + 1;
    txtPtr = ConvertIntToDecimalStringN(++txtPtr, GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL), STR_CONV_MODE_LEFT_ALIGN, 3);
    txtPtr = StringFill(txtPtr, 0, 5);
    txtPtr = txtPtr2 + 4;
	
	monGender = GetMonGender(&gPlayerParty[gBattleStruct->expGetterMonId]);
	
    if (monGender != MON_GENDERLESS)
    {
        if (monGender == MON_MALE)
        {
            txtPtr = WriteColorChangeControlCode(txtPtr, 0, 0xC);
            txtPtr = WriteColorChangeControlCode(txtPtr, 1, 0xD);
            *(txtPtr++) = CHAR_MALE;
        }
        else
        {
            txtPtr = WriteColorChangeControlCode(txtPtr, 0, 0xE);
            txtPtr = WriteColorChangeControlCode(txtPtr, 1, 0xF);
            *(txtPtr++) = CHAR_FEMALE;
        }
        *(txtPtr++) = EOS;
    }
    printerTemplate.y = 10;
    printerTemplate.currentY = 10;
    AddTextPrinter(&printerTemplate, TEXT_SPEED_FF, NULL);
    CopyWindowToVram(13, COPYWIN_GFX);
}

static bool8 SlideOutLvlUpBox(void)
{
    if (gBattle_BG2_X == 0x1A0)
        return FALSE;
    else if (gBattle_BG2_X - 16 < 0x1A0)
        gBattle_BG2_X = 0x1A0;
    else
        gBattle_BG2_X -= 16;
	
    return (gBattle_BG2_X != 0x1A0);
}

#define sDestroy                    data[0]
#define sSavedLvlUpBoxXPosition     data[1]

static void PutMonIconOnLvlUpBox(void)
{
    u8 spriteId;
    struct SpriteSheet iconSheet;
    struct SpritePalette iconPalSheet;
    u16 species = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPECIES);

    iconSheet.data = GetMonIconPtr(species);
    iconSheet.size = 0x200;
    iconSheet.tag = MON_ICON_LVLUP_BOX_TAG;
	
    iconPalSheet.data = GetValidMonIconPalettePtr(species);
    iconPalSheet.tag = MON_ICON_LVLUP_BOX_TAG;
	
    LoadSpriteSheet(&iconSheet);
    LoadSpritePalette(&iconPalSheet);
	
    spriteId = CreateSprite(&sSpriteTemplate_MonIconOnLvlUpBox, 256, 10, 0);
    gSprites[spriteId].sDestroy = FALSE;
    gSprites[spriteId].sSavedLvlUpBoxXPosition = gBattle_BG2_X;
}

static void SpriteCB_MonIconOnLvlUpBox(struct Sprite* sprite)
{
    sprite->x2 = sprite->sSavedLvlUpBoxXPosition - gBattle_BG2_X;
	
    if (sprite->x2 != 0)
        sprite->sDestroy = TRUE;
    else if (sprite->sDestroy)
    {
        DestroySprite(sprite);
        FreeSpriteTilesByTag(MON_ICON_LVLUP_BOX_TAG);
        FreeSpritePaletteByTag(MON_ICON_LVLUP_BOX_TAG);
    }
}

bool32 IsMonGettingExpSentOut(void)
{
    if (gBattlerPartyIndexes[0] == gBattleStruct->expGetterMonId || (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && gBattlerPartyIndexes[2] == gBattleStruct->expGetterMonId))
        return TRUE;
	return FALSE;
}

static void atk6D_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk6E_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk6F_makevisible(void)
{
    u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    BtlController_EmitSpriteInvisibility(battlerId, BUFFER_A, FALSE);
    MarkBattlerForControllerExec(battlerId);
    gBattlescriptCurrInstr += 2;
}

static void atk70_recordlastability(void)
{
    RecordAbilityBattle(GetBattlerForBattleScript(gBattlescriptCurrInstr[1]), gLastUsedAbility);
    gBattlescriptCurrInstr += 2;
}

static void atk71_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk72_jumpifplayerran(void)
{
    if (TryRunFromBattle(gBattlerFainted) && gProtectStructs[gBattlerFainted].fleeFlag != 2) // it don't count flee using Run Away in gen 5 onwards
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
        gBattlescriptCurrInstr += 5;
}

static void atk73_hpthresholds(void)
{
    u8 opposingBattler;
    s32 result;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        opposingBattler = BATTLE_OPPOSITE(GetBattlerForBattleScript(gBattlescriptCurrInstr[1]));
        result = gBattleMons[opposingBattler].hp * 100 / gBattleMons[opposingBattler].maxHP;
	    
        if (result == 0)
            result = 1;
		
        if (result > 69 || !gBattleMons[opposingBattler].hp)
            gBattleStruct->hpScale = 0;
        else if (result > 39)
            gBattleStruct->hpScale = 1;
        else if (result > 9)
            gBattleStruct->hpScale = 2;
        else
            gBattleStruct->hpScale = 3;
    }
    gBattlescriptCurrInstr += 2;
}

static void atk74_hpthresholds2(void)
{
    u8 hpSwitchout, opposingBattler;
    s32 result;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        opposingBattler = BATTLE_OPPOSITE(GetBattlerForBattleScript(gBattlescriptCurrInstr[1]));
        hpSwitchout = *(gBattleStruct->hpOnSwitchout + GetBattlerSide(opposingBattler));
        result = (hpSwitchout - gBattleMons[opposingBattler].hp) * 100 / hpSwitchout;

        if (gBattleMons[opposingBattler].hp >= hpSwitchout)
            gBattleStruct->hpScale = 0;
        else if (result <= 29)
            gBattleStruct->hpScale = 1;
        else if (result <= 69)
            gBattleStruct->hpScale = 2;
        else
            gBattleStruct->hpScale = 3;
    }
    gBattlescriptCurrInstr += 2;
}

static void atk75_nop(void)
{
    ++gBattlescriptCurrInstr;
}

void BufferMoveToLearnIntoBattleTextBuff2(void)
{
    PrepareMoveBuffer(gBattleTextBuff2, gMoveToLearn);
}

static void atk76_various(void)
{
	s32 i, data;
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);

    switch (gBattlescriptCurrInstr[2])
    {
		case VARIOUS_IS_RUNNING_IMPOSSIBLE:
            gBattleCommunication[MULTIUSE_STATE] = IsRunningFromBattleImpossible(battlerId, gBattlescriptCurrInstr[3]);
			gBattlescriptCurrInstr += 4;
			return;
		case VARIOUS_GET_BATTLER_FAINTED:
		    if (gHitMarker & HITMARKER_FAINTED(battlerId))
				gBattleCommunication[MULTIUSE_STATE] = TRUE;
			else
				gBattleCommunication[MULTIUSE_STATE] = FALSE;
			break;
		case VARIOUS_RESET_INTIMIDATE_TRACE_BITS:
	        gSpecialStatuses[battlerId].traced = FALSE;
			gSpecialStatuses[battlerId].switchInAbilityDone = FALSE;
			break;
		case VARIOUS_UPDATE_CHOICE_MOVE_ON_LVL_UP:
            if (gBattlerPartyIndexes[0] == gBattleStruct->expGetterMonId || gBattlerPartyIndexes[2] == gBattleStruct->expGetterMonId)
			{
				battlerId = gBattlerPartyIndexes[0] == gBattleStruct->expGetterMonId ? 0 : 2;
				
				for (i = 0; i < MAX_MON_MOVES; ++i)
				{
					if (gBattleMons[battlerId].moves[i] == gBattleStruct->choicedMove[battlerId])
						break;
				}
				if (i == MAX_MON_MOVES)
					gBattleStruct->choicedMove[battlerId] = MOVE_NONE;
			}
			break;
		case VARIOUS_RESET_PLAYER_FAINTED_FLAG:
		    if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_DOUBLE)) && gBattleTypeFlags & BATTLE_TYPE_TRAINER
			&& gBattleMons[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].hp && gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp) 
			    gHitMarker &= ~(HITMARKER_PLAYER_FAINTED);
			break;
		case VARIOUS_CHECK_POKEFLUTE:
		{
			u32 status = 0;
			
			gBattleCommunication[MULTIUSE_STATE] = FALSE;
			
			// Cure all battlers
			for (i = 0; i < gBattlersCount; ++i)
			{
				if (GetBattlerAbility(i) != ABILITY_SOUNDPROOF && gBattleMons[i].status1 & STATUS1_SLEEP)
					ClearBattlerStatus(i);
			}
			// Cure player's party
			for (i = 0, data = 0; i < PARTY_SIZE; ++i)
			{
				if (IsMonValidSpecies(&gPlayerParty[i]) && GetMonData(&gPlayerParty[i], MON_DATA_STATUS) & AILMENT_FNT && GetMonAbility(&gPlayerParty[i]) != ABILITY_SOUNDPROOF)
					data |= (1 << i);
			}
			if (data)
			{
				u8 battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
				BtlController_EmitSetMonData(battler, BUFFER_A, REQUEST_STATUS_BATTLE, data, 4, &status);
				MarkBattlerForControllerExec(battler);
				gBattleCommunication[MULTIUSE_STATE] = TRUE;
			}
			// Cure enemy's party
			for (i = 0, data = 0; i < PARTY_SIZE; ++i)
			{
				if (IsMonValidSpecies(&gEnemyParty[i]) && GetMonData(&gEnemyParty[i], MON_DATA_STATUS) & AILMENT_FNT && GetMonAbility(&gEnemyParty[i]) != ABILITY_SOUNDPROOF)
					data |= (1 << i);
			}
			if (data)
			{
				u8 battler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
				BtlController_EmitSetMonData(battler, BUFFER_A, REQUEST_STATUS_BATTLE, data, 4, &status);
				MarkBattlerForControllerExec(battler);
				gBattleCommunication[MULTIUSE_STATE] = TRUE;
			}
			break;
		}
		case VARIOUS_WAIT_FANFARE:
			if (!IsFanfareTaskInactive())
				return;
			break;
		case VARIOUS_TRY_ACTIVATE_MOXIE: // Also activates Chilling Neigh and As One ice rider
			if (HasAttackerFaintedTarget() && !NoAliveMonsForEitherParty() && CompareStat(battlerId, STAT_ATK, MAX_STAT_STAGES, CMP_LESS_THAN)
				&& (GetBattlerAbility(battlerId) == ABILITY_MOXIE || GetBattlerAbility(battlerId) == ABILITY_CHILLING_NEIGH
			    || GetBattlerAbility(battlerId) == ABILITY_AS_ONE_ICE_RIDER))
			{
				SET_STATCHANGER(STAT_ATK, 1, FALSE);
				PrepareStatBuffer(gBattleTextBuff1, STAT_ATK);
				
				if (GetBattlerAbility(battlerId) == ABILITY_AS_ONE_ICE_RIDER) // Print Chilling Neigh if As One activates
					gBattleStruct->abilityOverride[battlerId] = gLastUsedAbility = ABILITY_CHILLING_NEIGH;
				else
					gLastUsedAbility = GetBattlerAbility(battlerId);
				
				gBattleScripting.battler = battlerId;
				BattleScriptPush(gBattlescriptCurrInstr + 3);
				gBattlescriptCurrInstr = BattleScript_RaiseStatOnFaintingTarget;
				return;
			}
			break;
		case VARIOUS_TRY_ACTIVATE_GRIM_NEIGH: // Also activates As One shadow rider
		    if (HasAttackerFaintedTarget() && !NoAliveMonsForEitherParty() && CompareStat(battlerId, STAT_SPATK, MAX_STAT_STAGES, CMP_LESS_THAN)
				&& (GetBattlerAbility(battlerId) == ABILITY_GRIM_NEIGH || GetBattlerAbility(battlerId) == ABILITY_AS_ONE_SHADOW_RIDER))
			{
				SET_STATCHANGER(STAT_SPATK, 1, FALSE);
				PrepareStatBuffer(gBattleTextBuff1, STAT_SPATK);
				
				if (GetBattlerAbility(battlerId) == ABILITY_AS_ONE_SHADOW_RIDER) // Print Grim Neigh if As One activates
					gBattleStruct->abilityOverride[battlerId] = gLastUsedAbility = ABILITY_GRIM_NEIGH;
				else
					gLastUsedAbility = GetBattlerAbility(battlerId);
				
				gBattleScripting.battler = battlerId;
				BattleScriptPush(gBattlescriptCurrInstr + 3);
				gBattlescriptCurrInstr = BattleScript_RaiseStatOnFaintingTarget;
				return;
			}
			break;
		case VARIOUS_TRY_ACTIVATE_SOUL_HEART:
		    while (gBattleStruct->soulHeartBattlerId < gBattlersCount)
			{
				gBattleScripting.battler = gBattleStruct->soulHeartBattlerId++;
				
				if (IsBattlerAlive(gBattleScripting.battler) && GetBattlerAbility(gBattleScripting.battler) == ABILITY_SOUL_HEART
				&& !NoAliveMonsForEitherParty() && CompareStat(gBattleScripting.battler, STAT_SPATK, MAX_STAT_STAGES, CMP_LESS_THAN))
				{
					SET_STATCHANGER(STAT_SPATK, 1, FALSE);
					PrepareStatBuffer(gBattleTextBuff1, STAT_SPATK);
					BattleScriptCall(BattleScript_RaiseStatOnFaintingTarget);
					return;
				}
			}
			gBattleStruct->soulHeartBattlerId = 0;
			break;
		case VARIOUS_TRY_ACTIVATE_RECEIVER:
		    if (IsBattlerAlive(BATTLE_PARTNER(battlerId)) && (GetBattlerAbility(BATTLE_PARTNER(battlerId)) == ABILITY_RECEIVER
			|| GetBattlerAbility(BATTLE_PARTNER(battlerId)) == ABILITY_POWER_OF_ALCHEMY) && IsAbilityCopyableByReceiver(gBattleMons[battlerId].ability))
			{
				gBattleScripting.battler = battlerId;
				BattleScriptPush(gBattlescriptCurrInstr + 3);
				gBattlescriptCurrInstr = BattleScript_ReceiverActivates;
				return;
			}
			break;
		case VARIOUS_TRY_ACTIVATE_BEAST_BOOST:
		    if (HasAttackerFaintedTarget() && !NoAliveMonsForEitherParty() && GetBattlerAbility(battlerId) == ABILITY_BEAST_BOOST)
			{
				u8 highestStatId = GetBattlerHighestStatId(battlerId);
				
				if (CompareStat(battlerId, highestStatId, MAX_STAT_STAGES, CMP_LESS_THAN))
				{
					SET_STATCHANGER(highestStatId, 1, FALSE);
					PrepareStatBuffer(gBattleTextBuff1, highestStatId);
					gLastUsedAbility = GetBattlerAbility(battlerId);
					gBattleScripting.battler = battlerId;
					BattleScriptPush(gBattlescriptCurrInstr + 3);
					gBattlescriptCurrInstr = BattleScript_RaiseStatOnFaintingTarget;
					return;
				}
			}
			break;
		case VARIOUS_JUMP_IF_TARGET_ALLY:
			if (GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
				gBattlescriptCurrInstr += 7;
			else
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			return;
		case VARIOUS_TRAINER_SLIDE_FIRST_MON_DOWN:
			if (ShouldDoTrainerSlide(battlerId, TRAINER_SLIDE_FIRST_MON_DOWN))
			{
				BattleScriptPush(gBattlescriptCurrInstr + 3);
				gBattlescriptCurrInstr = BattleScript_TrainerSlideMsg;
				return;
			}
			break;
		case VARIOUS_TRAINER_SLIDE_LAST_MON:
			if (ShouldDoTrainerSlide(battlerId, TRAINER_SLIDE_LAST_MON_SEND_OUT))
			{
				BattleScriptPush(gBattlescriptCurrInstr + 3);
				gBattlescriptCurrInstr = BattleScript_TrainerSlideMsg;
				return;
			}
			break;
		case VARIOUS_UPDATE_NICK:
			UpdateHealthboxAttribute(battlerId, HEALTHBOX_NICK);
			break;
		case VARIOUS_TRY_REMOVE_ILLUSION:
			gBattlescriptCurrInstr += 3;
			TryRemoveIllusion(battlerId);
			return;
		case VARIOUS_PLAY_MOVE_ANIMATION:
			BtlController_EmitMoveAnimation(battlerId, BUFFER_A, READ_16(gBattlescriptCurrInstr + 3), gBattleScripting.animTurn, 0, 0, gBattleMons[battlerId].friendship, &gDisableStructs[battlerId]);
			MarkBattlerForControllerExec(battlerId);
			gBattlescriptCurrInstr += 5;
			return;
		case VARIOUS_JUMP_IF_BATTLE_END:
			if (NoAliveMonsForEitherParty())
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
				gBattlescriptCurrInstr += 7;
			return;
		case VARIOUS_TRY_REMOVE_PRIMAL_WEATHER:
		{
			bool8 shouldNotClear = FALSE;
		
			for (i = 0; i < gBattlersCount; i++)
			{
				if (IsBattlerAlive(i) && ((gBattleWeather & WEATHER_RAIN_PRIMAL && GetBattlerAbility(i) == ABILITY_PRIMORDIAL_SEA)
				|| (gBattleWeather & WEATHER_SUN_PRIMAL && GetBattlerAbility(i) == ABILITY_DESOLATE_LAND) || (gBattleWeather & WEATHER_STRONG_WINDS
				&& GetBattlerAbility(i) == ABILITY_DELTA_STREAM)))
					shouldNotClear = TRUE;
			}
			if (!shouldNotClear)
			{
				if (gBattleWeather & WEATHER_RAIN_PRIMAL)
				{
					gBattleWeather &= ~(WEATHER_RAIN_PRIMAL);
					PrepareStringBattle(STRINGID_HEAVYRAINLIFTED, battlerId);
					gBattleCommunication[MSG_DISPLAY] = TRUE;
				}
				else if (gBattleWeather & WEATHER_SUN_PRIMAL)
				{
					gBattleWeather &= ~(WEATHER_SUN_PRIMAL);
					PrepareStringBattle(STRINGID_EXTREMESUNLIGHTFADED, battlerId);
					gBattleCommunication[MSG_DISPLAY] = TRUE;
				}
				else if (gBattleWeather & WEATHER_STRONG_WINDS)
				{
					gBattleWeather &= ~(WEATHER_STRONG_WINDS);
					PrepareStringBattle(STRINGID_STRONGWINDSDISSIPATED, battlerId);
					gBattleCommunication[MSG_DISPLAY] = TRUE;
				}
			}
			break;
		}
		case VARIOUS_JUMP_IF_NOT_ALIVE:
			if (!IsBattlerAlive(battlerId))
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
				gBattlescriptCurrInstr += 7;
			return;
		case VARIOUS_TRY_FRISK_TARGET:
			if (gBattleMons[battlerId].item)
			{
				gLastUsedItem = gBattleMons[battlerId].item;
				RecordItemEffectBattle(battlerId, GetBattlerItemHoldEffect(battlerId, FALSE));
				gBattlescriptCurrInstr += 7;
			}
			else
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			return;
		case VARIOUS_SWITCHIN_ABILITIES: // Activates switch in abilities on ability change
			gBattlescriptCurrInstr += 3;
		
			if (IsBattlerAlive(battlerId))
			{
				AbilityBattleEffects(ABILITYEFFECT_NEUTRALIZING_GAS, 0);
				AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, battlerId);
				AbilityBattleEffects(ABILITYEFFECT_TRACE, battlerId);
			}
			return;
		case VARIOUS_TRY_END_NEUTRALIZING_GAS: // Try remove it when ability changed or suppresed
			if (gSpecialStatuses[battlerId].removedNeutralizingGas)
			{
				gSpecialStatuses[battlerId].removedNeutralizingGas = FALSE;
				PrepareAbilityBuffer(gBattleTextBuff1, ABILITY_NEUTRALIZING_GAS);
				gBattleScripting.battler = battlerId;
				BattleScriptPush(gBattlescriptCurrInstr + 3);
				gBattlescriptCurrInstr = BattleScript_NeutralizingGasExits;
				return;
			}
			break;
		case VARIOUS_SET_TELEPORT_OUTCOME:
			if (GetBattlerSide(battlerId) == B_SIDE_OPPONENT)
			{
				// Don't end the battle if one of the wild mons teleported from the wild double battle and its partner is still alive.
				if (IsBattlerAlive(BATTLE_PARTNER(battlerId)))
				{
					gAbsentBattlerFlags |= gBitTable[battlerId];
					gHitMarker |= HITMARKER_FAINTED(battlerId);
					gBattleMons[battlerId].hp = 0;
					SetMonData(&gEnemyParty[gBattlerPartyIndexes[battlerId]], MON_DATA_HP, &gBattleMons[battlerId].hp);
					SetHealthboxSpriteInvisible(gHealthboxSpriteIds[battlerId]);
					FaintClearSetData(battlerId);
				}
				else
					gBattleOutcome = B_OUTCOME_MON_TELEPORTED;
			}
			else
				gBattleOutcome = B_OUTCOME_PLAYER_TELEPORTED;
			break;
		case VARIOUS_TRY_FAINT_ON_SPIKES_DAMAGE:
			if (gHitMarker & HITMARKER_FAINTED(battlerId))
			{
				const u8 *BS_ptr = READ_PTR(gBattlescriptCurrInstr + 3);
				BattleScriptPop();
				gBattlescriptCurrInstr = BS_ptr;
				gSideStatuses[GetBattlerSide(battlerId)] &= ~(SIDE_STATUS_SPIKES_DAMAGED);
			}
			else
				gBattlescriptCurrInstr += 7;
			return;
		case VARIOUS_ACTIVATE_WEATHER_ABILITIES:
			gBattlescriptCurrInstr += 3;
			AbilityBattleEffects(ABILITYEFFECT_ON_WEATHER, battlerId);
			return;
		case VARIOUS_TRY_NEUTRALIZING_GAS_SUPPRESSION:
		    if (IsAbilityBlockedByNeutralizingGas(gBattleMons[battlerId].ability))
			{
				SuppressBattlerAbility(battlerId);
				gBattlescriptCurrInstr += 7;
			}
			else
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			return;
		case VARIOUS_JUMP_IF_BATTLER_REVIVED:
		    if (gBattleStruct->usedReviveItemBattler & gBitTable[battlerId])
			{
				gBattleStruct->usedReviveItemBattler &= ~(gBitTable[battlerId]);
				gAbsentBattlerFlags &= ~(gBitTable[gBattleScripting.battler]);
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			}
			else
				gBattlescriptCurrInstr += 7;
			return;
		case VARIOUS_JUMP_IF_ROAR_FAILS:
			if ((IS_DOUBLE_WILD_BATTLE() && GetBattlerSide(battlerId) == B_SIDE_PLAYER && GetBattlerSide(gBattlerTarget) == B_SIDE_OPPONENT
			&& IS_WHOLE_SIDE_ALIVE(gBattlerTarget)) || (IS_DOUBLE_WILD_BATTLE() && GetBattlerSide(battlerId) == B_SIDE_OPPONENT
			&& GetBattlerSide(gBattlerTarget) == B_SIDE_OPPONENT))
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
				gBattlescriptCurrInstr += 7;
			return;
		case VARIOUS_JUMP_IF_WEATHER_AFFECTED:
			if (IsBattlerWeatherAffected(battlerId, READ_16(gBattlescriptCurrInstr + 3)))
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 5);
			else
				gBattlescriptCurrInstr += 9;
			return;
		case VARIOUS_GET_BATTLER_SIDE:
			gBattleCommunication[MULTIUSE_STATE] = GetBattlerSide(battlerId);
			break;
		case VARIOUS_JUMP_IF_PARENTAL_BOND_COUNTER:
			// Some effects only occours in certain hits of Parental Bond, so a way to check this in scripts are usefull
			if (gSpecialStatuses[battlerId].parentalBondState == gBattlescriptCurrInstr[3] && IsBattlerAlive(gBattlerTarget))
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 4);
			else
				gBattlescriptCurrInstr += 8;
			return;
		case VARIOUS_JUMP_IF_NOT_FIRST_TURN:
			if (gDisableStructs[battlerId].isFirstTurn)
				gBattlescriptCurrInstr += 7;
			else
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			return;
		case VARIOUS_TRY_CONVERSION_TYPE_CHANGE:
		{
			u8 moveType = gBattleMoves[gBattleMons[battlerId].moves[0]].type; // Always first slot move's type
			
			if (IS_BATTLER_OF_TYPE(battlerId, moveType))
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
			{
				SetBattlerType(battlerId, moveType);
				gBattlescriptCurrInstr += 7;
			}
			return;
		}
		case VARIOUS_TRY_SET_DESTINY_BOND:
		    if (gDisableStructs[battlerId].destinyBondCounter) // Fail if was successfully executed on the previous turn
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
			{
				gDisableStructs[battlerId].destinyBondCounter = 2;
				gBattleMons[battlerId].status2 |= STATUS2_DESTINY_BOND;
				gBattlescriptCurrInstr += 7;
			}
			return;
		case VARIOUS_TRY_COPY_ABILITY:
		{
			u16 abilityAttacker = gBattleMons[gBattlerAttacker].ability, abilityTarget = gBattleMons[gBattlerTarget].ability;
			
		    if (abilityAttacker == abilityTarget || IsRolePlayBannedAbilityAttacker(abilityAttacker) || IsRolePlayBannedAbilityTarget(abilityTarget))
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
				gBattlescriptCurrInstr += 7;
			return;
		}
		case VARIOUS_JUMP_IF_CANT_GIVE_NICK:
		    if (gSaveBlock2Ptr->optionsSkipPkmnNickname)
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
				gBattlescriptCurrInstr += 7;
			return;
		case VARIOUS_JUMP_IF_NO_DAMAGE:
		    if (gProtectStructs[gBattlerAttacker].physicalDmg || gProtectStructs[gBattlerAttacker].specialDmg)
				gBattlescriptCurrInstr += 7;
			else
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			return;
		case VARIOUS_JUMP_IF_ENDEAVOR_FAIL:
		    if (gBattleMons[gBattlerAttacker].hp >= gBattleMons[gBattlerTarget].hp)
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
				gBattlescriptCurrInstr += 7;
			return;
		case VARIOUS_TRY_SWAP_ABILITIES:
		    if (IsSkillSwapBannedAbility(gBattleMons[gBattlerAttacker].ability) || IsSkillSwapBannedAbility(gBattleMons[gBattlerTarget].ability))
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 3);
			else
				gBattlescriptCurrInstr += 7;
			return;
    }
    gBattlescriptCurrInstr += 3;
}

static void atk77_setprotectlike(void)
{
	u8 i, divisor = 1;
	
    if (!gBattleMoves[gLastResultingMoves[gBattlerAttacker]].flags.protectionMove)
        gDisableStructs[gBattlerAttacker].protectUses = 0;
	
	if (!gBattleMoves[gCurrentMove].flags.affectsUserSide) // effects that protects the user's side increase the fail rate, but can't fail themselves
	{
		for (i = 0; i < gDisableStructs[gBattlerAttacker].protectUses && i <= 6; ++i)
			divisor *= 3;
	}
	
    if ((0xFFFF / divisor) >= Random() && gCurrentTurnActionNumber != (gBattlersCount - 1))
    {
		switch (gBattleMoves[gCurrentMove].effect)
		{
			case EFFECT_PROTECT:
				gProtectStructs[gBattlerAttacker].protected = TRUE;
				gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_USED_PROTECT;
				break;
			case EFFECT_ENDURE:
				gProtectStructs[gBattlerAttacker].endured = TRUE;
				gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_BRACED_ITSELF;
				break;
		}
		++gDisableStructs[gBattlerAttacker].protectUses;
		++gBattlescriptCurrInstr;
    }
    else
    {
        gDisableStructs[gBattlerAttacker].protectUses = 0;
		gBattlescriptCurrInstr = BattleScript_ButItFailed;
    }
}

static void atk78_instanthpdrop(void)
{
	if (IsBattlerAlive(gBattlerAttacker))
	{
		gBattleMoveDamage = gBattleMons[gBattlerAttacker].hp;
		BtlController_EmitHealthBarUpdate(gBattlerAttacker, BUFFER_A, INSTANT_HP_BAR_DROP);
		MarkBattlerForControllerExec(gBattlerAttacker);
	}
	++gBattlescriptCurrInstr;
}

static void atk79_setatkhptozero(void)
{
    if (!gBattleControllerExecFlags)
    {
		if (IsBattlerAlive(gBattlerAttacker))
		{
			gBattleMons[gBattlerAttacker].hp = 0;
			BtlController_EmitSetMonData(gBattlerAttacker, BUFFER_A, REQUEST_HP_BATTLE, 0, 2, &gBattleMons[gBattlerAttacker].hp);
			MarkBattlerForControllerExec(gBattlerAttacker);
		}
        ++gBattlescriptCurrInstr;
    }
}

static void atk7A_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk7B_tryhealhalfhealth(void)
{
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	if (BATTLER_MAX_HP(battlerId))
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 2);
	else
	{
		gBattleMoveDamage = gBattleMons[battlerId].maxHP / 2;
		if (gBattleMoveDamage == 0)
			gBattleMoveDamage = 1;
		gBattleMoveDamage *= -1;
		gBattlescriptCurrInstr += 6;
	}
}

static void atk7C_trymirrormove(void)
{
	u8 i, validMovesCount;
    u16 move, newMove, movesArray[MAX_MON_MOVES - 1];

    for (i = 0; i < MAX_MON_MOVES - 1; ++i)
        movesArray[i] = MOVE_NONE;
	
    for (i = 0, validMovesCount = 0; i < gBattlersCount; ++i)
    {
        if (i != gBattlerAttacker)
        {
            move = gBattleStruct->lastTakenMoveFrom[gBattlerAttacker][i];
		
            if (move && move != MOVE_UNVAILABLE)
                movesArray[validMovesCount++] = move;
        }
    }
    move = gBattleStruct->lastTakenMove[gBattlerAttacker];
	
    if (move && move != MOVE_UNVAILABLE)
		newMove = move;
    else if (validMovesCount)
        newMove = movesArray[Random() % validMovesCount];
    else // No valid moves find
    {
        gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
        ++gBattlescriptCurrInstr;
		return;
    }
	CallAnotherMove(newMove);
}

static void atk7D_setrain(void)
{
    if (!TryChangeBattleWeather(gBattlerAttacker, ENUM_WEATHER_RAIN))
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEATHER_MOVE_FAIL;
    }
    else
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STARTED_RAIN;
	
    ++gBattlescriptCurrInstr;
}

static void atk7E_trysetreflect(void)
{
    if (gSideStatuses[GetBattlerSide(gBattlerAttacker)] & SIDE_STATUS_REFLECT)
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gSideStatuses[GetBattlerSide(gBattlerAttacker)] |= SIDE_STATUS_REFLECT;
        gSideTimers[GetBattlerSide(gBattlerAttacker)].reflectTimer = 5;
        gSideTimers[GetBattlerSide(gBattlerAttacker)].reflectBattlerId = gBattlerAttacker;
		PrepareMoveBuffer(gBattleTextBuff1, MOVE_REFLECT);
		PrepareStatBuffer(gBattleTextBuff2, STAT_DEF);
		gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_REFLECT_RAISED;
		gBattlescriptCurrInstr += 5;
    }
}

static void atk7F_setseeded(void)
{
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT || gStatuses3[gBattlerTarget] & STATUS3_LEECHSEED)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_EVADED_ATTACK;
    }
    else if (IS_BATTLER_OF_TYPE(gBattlerTarget, TYPE_GRASS))
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_LEECH_SEED_DOEST_AFFECT;
    }
    else
    {
        gStatuses3[gBattlerTarget] |= gBattlerAttacker;
        gStatuses3[gBattlerTarget] |= STATUS3_LEECHSEED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SEEDED;
    }
    ++gBattlescriptCurrInstr;
}

static void atk80_manipulatedamage(void)
{
    switch (gBattlescriptCurrInstr[1])
    {
		case ATK80_DMG_CHANGE_SIGN:
            gBattleMoveDamage *= -1;
            break;
		case ATK80_DMG_HALF_USER_HP:
			gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 2;
			if (gBattleMoveDamage == 0)
				gBattleMoveDamage = 1;
			break;
		case ATK80_DMG_1_8_TARGET_MAX_HP:
			gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 8;
			if (gBattleMoveDamage == 0)
				gBattleMoveDamage = 1;
			break;
		case ATK48_DMG_HALF_TARGET_HP:
		    gBattleMoveDamage = gBattleMons[gBattlerTarget].hp / 2;
			if (gBattleMoveDamage == 0)
				gBattleMoveDamage = 1;
			break;
		case ATK48_DMG_ATK_LEVEL:
		    gBattleMoveDamage = gBattleMons[gBattlerAttacker].level;
			break;
		case ATK80_DMG_DRAINED:
			if (gBattleMoves[gCurrentMove].argument)
				gBattleMoveDamage = (gHpDealt * gBattleMoves[gCurrentMove].argument / 100);
			else
				gBattleMoveDamage = (gHpDealt / 2);
			
			if (gBattleMoveDamage == 0)
				gBattleMoveDamage = 1;
			// Fallthrough
		case ATK48_DMG_BIG_ROOT:
			gBattleMoveDamage = GetDrainedBigRootHp(gBattlerAttacker, gBattleMoveDamage);
			break;
		case ATK80_DMG_HEALTH_DIFFERENCE:
		    gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - gBattleMons[gBattlerAttacker].hp;
			break;
    }
    gBattlescriptCurrInstr += 2;
}

static void atk81_trysetrest(void)
{
    if (BATTLER_MAX_HP(gBattlerTarget))
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
		gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP * (-1);
		
        if (gBattleMons[gBattlerTarget].status1 & ((u8)(~STATUS1_SLEEP)))
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SLEPT_HEALTHY;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WENT_TO_SLEEP;
        gBattleMons[gBattlerTarget].status1 = STATUS1_SLEEP_TURN(3);
        BtlController_EmitSetMonData(gBattlerTarget, BUFFER_A, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[gBattlerTarget].status1);
        MarkBattlerForControllerExec(gBattlerTarget);
        gBattlescriptCurrInstr += 5;
    }
}

static void atk82_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk83_handletrainerslidecase(void)
{
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	switch (gBattlescriptCurrInstr[2])
	{
		case ATK83_TRAINER_SLIDE_CASE_SAVE_SPRITES:
		    // Save sprite Id's, because trainer slide in will overwrite gBattlerSpriteIds variable.
		    gBattleMoveDamage = (gBattlerSpriteIds[battlerId] & 0xFF) | (gBattlerSpriteIds[BATTLE_PARTNER(battlerId)] << 8);
		    HideBattlerShadowSprite(battlerId);
		    break;
		case ATK83_TRAINER_SLIDE_CASE_SLIDE_IN:
		    battlerId = GetBattlerAtPosition(gBattlescriptCurrInstr[1]);
			BtlController_EmitTrainerSlide(battlerId, BUFFER_A);
			MarkBattlerForControllerExec(battlerId);
			break;
		case ATK83_TRAINER_SLIDE_CASE_PRINT_STRING:
		    PrepareStringBattle(STRINGID_TRAINERSLIDE, battlerId);
			gBattleCommunication[MSG_DISPLAY] = TRUE;
		    break;
		case ATK83_TRAINER_SLIDE_CASE_SLIDE_OUT:
		    battlerId = GetBattlerAtPosition(gBattlescriptCurrInstr[1]);
			BtlController_EmitTrainerSlideBack(battlerId, BUFFER_A);
			MarkBattlerForControllerExec(battlerId);
			break;
		case ATK83_TRAINER_SLIDE_CASE_RESTORE_SPRITES:
		    gBattlerSpriteIds[battlerId] = gBattleMoveDamage & 0xFF;
			gBattlerSpriteIds[BATTLE_PARTNER(battlerId)] = gBattleMoveDamage >> 8;
		    
		    if (IsBattlerAlive(battlerId))
		    {
			    SetBattlerShadowSpriteCallback(battlerId, gBattleMons[battlerId].species);
			    BattleLoadMonSpriteGfx(battlerId);
		    }
			if (IsBattlerAlive(BATTLE_PARTNER(battlerId)))
			{
				SetBattlerShadowSpriteCallback(BATTLE_PARTNER(battlerId), gBattleMons[BATTLE_PARTNER(battlerId)].species);
			    BattleLoadMonSpriteGfx(BATTLE_PARTNER(battlerId));
			}
		    break;
	}
	gBattlescriptCurrInstr += 3;
}

static void atk84_trysetpoison(void)
{
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	switch (CanBePoisoned(gBattlerAttacker, battlerId, 0))
	{
		case STATUS_CHANGE_WORKED:
			if (SubsBlockMove(gBattlerAttacker, battlerId, gCurrentMove))
				gBattlescriptCurrInstr = BattleScript_ButItFailed;
			else
				gBattlescriptCurrInstr += 2;
			break;
		case STATUS_CHANGE_FAIL_SPECIFIC_STATUSED:
			gBattlescriptCurrInstr = BattleScript_AlreadyPoisoned;
			break;
		case STATUS_CHANGE_FAIL_ALREADY_STATUSED:
			gBattlescriptCurrInstr = BattleScript_ButItFailed;
			break;
		case STATUS_CHANGE_FAIL_TYPE_NOT_AFFECTED:
			gBattlescriptCurrInstr = BattleScript_NotAffected;
			break;
		case STATUS_CHANGE_FAIL_SAFEGUARD_PROTECTED:
			gBattlescriptCurrInstr = BattleScript_SafeguardProtected;
			break;
		case STATUS_CHANGE_FAIL_ABILITY_PREVENTED:
			gLastUsedAbility = GetBattlerAbility(battlerId);
			RecordAbilityBattle(battlerId, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_ImmunityProtected;
			break;
		case STATUS_CHANGE_FAIL_FLOWER_VEIL_ON_SIDE:
			gBattleScripting.battler = gBattleScripting.savedBattler;
			gLastUsedAbility = GetBattlerAbility(gBattleScripting.battler);
			RecordAbilityBattle(gBattleScripting.battler, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_TeamProtectedByFlowerVeil;
			break;
		case STATUS_CHANGE_FAIL_PASTEL_VEIL_ON_SIDE:
			gBattleScripting.battler = gBattleScripting.savedBattler;
			gLastUsedAbility = GetBattlerAbility(gBattleScripting.battler);
			RecordAbilityBattle(gBattleScripting.battler, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_TeamProtectedByPastelVeil;
			break;
	}
}

static void atk85_stockpile(void)
{
    if (gDisableStructs[gBattlerAttacker].stockpileCounter == 3)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
    }
    else
    {
        ++gDisableStructs[gBattlerAttacker].stockpileCounter;
        PrepareByteNumberBuffer(gBattleTextBuff1, 1, gDisableStructs[gBattlerAttacker].stockpileCounter);
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
    }
    ++gBattlescriptCurrInstr;
}

static void atk86_stockpiletobasedamage(void)
{
    if (!gDisableStructs[gBattlerAttacker].stockpileCounter)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        if (gBattleCommunication[MISS_TYPE] != B_MSG_PROTECTED)
        {
            gBattleMoveDamage = CalculateMoveDamage(gCurrentMove, gBattleStruct->dynamicMoveType, gBattlerAttacker, gBattlerTarget, FALSE, TRUE);
            gBattleScripting.animTurn = gDisableStructs[gBattlerAttacker].stockpileCounter;
        }
		if (!(gSpecialStatuses[gBattlerAttacker].parentalBondState == PARENTAL_BOND_1ST_HIT && IsBattlerAlive(gBattlerTarget)))
			gDisableStructs[gBattlerAttacker].stockpileCounter = 0;
		
        gBattlescriptCurrInstr += 5;
    }
}

static void atk87_stockpiletohpheal(void)
{
    const u8 *jumpPtr = READ_PTR(gBattlescriptCurrInstr + 1);

    if (!gDisableStructs[gBattlerAttacker].stockpileCounter)
    {
        gBattlescriptCurrInstr = jumpPtr;
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
    }
    else if (BATTLER_MAX_HP(gBattlerAttacker))
    {
        gDisableStructs[gBattlerAttacker].stockpileCounter = 0;
        gBattlescriptCurrInstr = jumpPtr;
        gBattlerTarget = gBattlerAttacker;
        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
    }
    else
    {
        gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / (1 << (3 - gDisableStructs[gBattlerAttacker].stockpileCounter));
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattleMoveDamage *= -1;
        gBattleScripting.animTurn = gDisableStructs[gBattlerAttacker].stockpileCounter;
        gDisableStructs[gBattlerAttacker].stockpileCounter = 0;
        gBattlerTarget = gBattlerAttacker;
		gBattlescriptCurrInstr += 5;
    }
}

static void atk88_nop(void)
{
    ++gBattlescriptCurrInstr;
}

u8 ChangeStatBuffs(s8 statValue, u8 statId, u8 flags, const u8 *BS_ptr)
{
    bool8 certain = FALSE;
    bool8 notProtectAffected = FALSE;
    u32 index;
	u8 battler;

    if (flags & MOVE_EFFECT_AFFECTS_USER)
        battler = gBattlerAttacker;
    else
        battler = gBattlerTarget;
	
	gSpecialStatuses[battler].changedStatsBattlerId = gBattlerAttacker;
	
    flags &= ~(MOVE_EFFECT_AFFECTS_USER);
    if (flags & MOVE_EFFECT_CERTAIN)
        ++certain;
    flags &= ~(MOVE_EFFECT_CERTAIN);
    if (flags & STAT_CHANGE_NOT_PROTECT_AFFECTED)
        ++notProtectAffected;
    flags &= ~(STAT_CHANGE_NOT_PROTECT_AFFECTED);
	
    PrepareStatBuffer(gBattleTextBuff1, statId);
	
    if ((statValue >= 0 && GetBattlerAbility(battler) == ABILITY_CONTRARY) || (statValue <= -1 && GetBattlerAbility(battler) != ABILITY_CONTRARY)) 
    {   
	// Stat decrease.
        if ((gSideStatuses[GetBattlerSide(battler)] & SIDE_STATUS_MIST) && !certain && gCurrentMove != MOVE_CURSE && GetBattlerAbility(gBattlerAttacker) != ABILITY_INFILTRATOR)
        {
            if (flags == STAT_CHANGE_BS_PTR)
            {
                if (gSpecialStatuses[battler].statLowered)
                    gBattlescriptCurrInstr = BS_ptr;
                else
                {
                    BattleScriptPush(BS_ptr);
                    gBattleScripting.battler = battler;
                    gBattlescriptCurrInstr = BattleScript_MistProtected;
                    gSpecialStatuses[battler].statLowered = 1;
                }
            }
            return STAT_CHANGE_DIDNT_WORK;
        }
        else if (gCurrentMove != MOVE_CURSE && notProtectAffected != TRUE && JumpIfMoveAffectedByProtect())
        {
            gBattlescriptCurrInstr = BattleScript_ButItFailed;
            return STAT_CHANGE_DIDNT_WORK;
        }
        else if ((GetBattlerAbility(battler) == ABILITY_CLEAR_BODY || GetBattlerAbility(battler) == ABILITY_WHITE_SMOKE
		 || GetBattlerAbility(battler) == ABILITY_FULL_METAL_BODY || (GetBattlerAbility(battler) == ABILITY_FLOWER_VEIL
		 && IS_BATTLER_OF_TYPE(battler, TYPE_GRASS))) && !certain && gCurrentMove != MOVE_CURSE)
        {
            if (flags == STAT_CHANGE_BS_PTR)
            {
                if (gSpecialStatuses[battler].statLowered)
                    gBattlescriptCurrInstr = BS_ptr;
                else
                {
                    BattleScriptPush(BS_ptr);
                    gBattleScripting.battler = battler;
                    gBattlescriptCurrInstr = BattleScript_AbilityNoStatLoss;
                    gLastUsedAbility = GetBattlerAbility(battler);
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    gSpecialStatuses[battler].statLowered = 1;
                }
            }
            return STAT_CHANGE_DIDNT_WORK;
        }
		else if (IsBattlerAlive(BATTLE_PARTNER(battler)) && GetBattlerAbility(BATTLE_PARTNER(battler)) == ABILITY_FLOWER_VEIL
		&& IS_BATTLER_OF_TYPE(battler, TYPE_GRASS) && !certain && gCurrentMove != MOVE_CURSE)
		{
			if (flags == STAT_CHANGE_BS_PTR)
            {
                if (gSpecialStatuses[battler].statLowered)
                    gBattlescriptCurrInstr = BS_ptr;
                else
                {
                    BattleScriptPush(BS_ptr);
                    gBattleScripting.battler = BATTLE_PARTNER(battler);
                    gBattlescriptCurrInstr = BattleScript_AbilityNoStatLoss;
                    gLastUsedAbility = GetBattlerAbility(BATTLE_PARTNER(battler));
                    RecordAbilityBattle(BATTLE_PARTNER(battler), gLastUsedAbility);
                    gSpecialStatuses[battler].statLowered = 1;
                }
            }
            return STAT_CHANGE_DIDNT_WORK;
		}
        else if (!certain && ((GetBattlerAbility(battler) == ABILITY_KEEN_EYE && statId == STAT_ACC)
		|| (GetBattlerAbility(battler) == ABILITY_HYPER_CUTTER && statId == STAT_ATK)
		|| (GetBattlerAbility(battler) == ABILITY_BIG_PECKS && statId == STAT_DEF)))
        {
            if (flags == STAT_CHANGE_BS_PTR)
            {
                BattleScriptPush(BS_ptr);
                gBattleScripting.battler = battler;
                gBattlescriptCurrInstr = BattleScript_AbilityNoSpecificStatLoss;
                gLastUsedAbility = GetBattlerAbility(battler);
                RecordAbilityBattle(battler, gLastUsedAbility);
            }
            return STAT_CHANGE_DIDNT_WORK;
        }
        else if ((GetBattlerAbility(battler) == ABILITY_SHIELD_DUST || ReceiveSheerForceBoost(gBattlerAttacker, gCurrentMove)) && !flags)
            return STAT_CHANGE_DIDNT_WORK;
        else // try to decrease
        {
            statValue = -GET_STAT_BUFF_VALUE(statValue);
		
	    if (GetBattlerAbility(battler) == ABILITY_SIMPLE)
		    statValue *= 2;
		
            gBattleTextBuff2[0] = B_BUFF_PLACEHOLDER_BEGIN;
            index = 1;
            if (statValue == -2 || statValue == -4)
            {
                gBattleTextBuff2[1] = B_BUFF_STRING;
                gBattleTextBuff2[2] = STRINGID_STATHARSHLY;
                gBattleTextBuff2[3] = STRINGID_STATHARSHLY >> 8;
                index = 4;
            }
            gBattleTextBuff2[index++] = B_BUFF_STRING;
            gBattleTextBuff2[index++] = STRINGID_STATFELL;
            gBattleTextBuff2[index++] = STRINGID_STATFELL >> 8;
            gBattleTextBuff2[index] = B_BUFF_EOS;
            if (gBattleMons[battler].statStages[statId] == MIN_STAT_STAGES)
                gBattleCommunication[MULTISTRING_CHOOSER] = 2;
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = (gBattlerTarget == battler);
        }
    }
    else // stat increase
    {
        statValue = GET_STAT_BUFF_VALUE(statValue);
	   
	if (GetBattlerAbility(battler) == ABILITY_SIMPLE)
		statValue *= 2;
	    
        gBattleTextBuff2[0] = B_BUFF_PLACEHOLDER_BEGIN;
        index = 1;
        if (statValue == 2 || statValue == 4)
        {
            gBattleTextBuff2[1] = B_BUFF_STRING;
            gBattleTextBuff2[2] = STRINGID_STATSHARPLY;
            gBattleTextBuff2[3] = STRINGID_STATSHARPLY >> 8;
            index = 4;
        }
        gBattleTextBuff2[index++] = B_BUFF_STRING;
        gBattleTextBuff2[index++] = STRINGID_STATROSE;
        gBattleTextBuff2[index++] = STRINGID_STATROSE >> 8;
        gBattleTextBuff2[index] = B_BUFF_EOS;
        if (gBattleMons[battler].statStages[statId] == MAX_STAT_STAGES)
            gBattleCommunication[MULTISTRING_CHOOSER] = 2;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = (gBattlerTarget == battler);
    }
    gBattleMons[battler].statStages[statId] += statValue;
    if (gBattleMons[battler].statStages[statId] < MIN_STAT_STAGES)
        gBattleMons[battler].statStages[statId] = MIN_STAT_STAGES;
    else if (gBattleMons[battler].statStages[statId] > MAX_STAT_STAGES)
        gBattleMons[battler].statStages[statId] = MAX_STAT_STAGES;
    if (gBattleCommunication[MULTISTRING_CHOOSER] == 2 && flags & STAT_CHANGE_BS_PTR)
        gMoveResultFlags |= MOVE_RESULT_MISSED;
    if (gBattleCommunication[MULTISTRING_CHOOSER] == 2 && !(flags & STAT_CHANGE_BS_PTR))
        return STAT_CHANGE_DIDNT_WORK;
    return STAT_CHANGE_WORKED;
}

static void atk89_statbuffchange(void)
{
    if (ChangeStatBuffs(GET_STAT_BUFF_VALUE2(gBattleScripting.statChanger), GET_STAT_BUFF_ID(gBattleScripting.statChanger), gBattlescriptCurrInstr[1],
	READ_PTR(gBattlescriptCurrInstr + 2)) == STAT_CHANGE_WORKED)
        gBattlescriptCurrInstr += 6;
}

static void atk8A_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk8B_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk8C_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk8D_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk8E_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void ForceSwitchOut(void)
{
	*(gBattleStruct->battlerPartyIndexes + gBattlerTarget) = gBattlerPartyIndexes[gBattlerTarget];
	gBattlescriptCurrInstr = BattleScript_SuccessForceOut;
}

static void atk8F_forcerandomswitch(void)
{
    if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER) || (IS_DOUBLE_WILD_BATTLE() && GetBattlerSide(gBattlerAttacker) == B_SIDE_OPPONENT
	&& GetBattlerSide(gBattlerTarget) == B_SIDE_PLAYER && IS_WHOLE_SIDE_ALIVE(gBattlerTarget)) || (IS_DOUBLE_WILD_BATTLE()
	&& GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER && GetBattlerSide(gBattlerTarget) == B_SIDE_PLAYER))
    {
        u8 i, valid, val;
        struct Pokemon *party = GetBattlerParty(gBattlerTarget);
       
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        {
            valid = 0;
            val = 0;
		
            if (GetLinkTrainerFlankId(GetBattlerMultiplayerId(gBattlerTarget)) == 1)
                val = 3;
			
            for (i = val; i < val + 3; ++i)
            {
                if (MonCanBattle(&party[i]))
                    ++valid;
            }
        }
        else
        {
            valid = 0;
		
            for (i = 0; i < PARTY_SIZE; ++i)
            {
                if (MonCanBattle(&party[i]))
                    ++valid;
            }
        }

        if ((valid < 2 && (gBattleTypeFlags & (BATTLE_TYPE_DOUBLE | BATTLE_TYPE_MULTI)) != BATTLE_TYPE_DOUBLE)
	    || (valid < 3 && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) && !(gBattleTypeFlags & BATTLE_TYPE_MULTI)))
            gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
        else
        {
			ForceSwitchOut();
			
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                do
                {
                    val = Random() % 3;
			
                    if (GetLinkTrainerFlankId(GetBattlerMultiplayerId(gBattlerTarget)) == 1)
                        i = val + 3;
                    else
                        i = val;
                }
                while (i == gBattlerPartyIndexes[gBattlerTarget] || i == gBattlerPartyIndexes[BATTLE_PARTNER(gBattlerTarget)] || !MonCanBattle(&party[i]));
            }
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                {
                    do
                    {
                        i = Random() % PARTY_SIZE;
                    }
                    while (i == gBattlerPartyIndexes[gBattlerTarget] || i == gBattlerPartyIndexes[BATTLE_PARTNER(gBattlerTarget)] || !MonCanBattle(&party[i]));
                }
                else
                {
                    do
                    {
                        i = Random() % PARTY_SIZE;
                    }
                    while (i == gBattlerPartyIndexes[gBattlerTarget] || !MonCanBattle(&party[i]));
                }
            }
            gBattleStruct->monToSwitchIntoId[gBattlerTarget] = i;
            if (!IsMultiBattle())
                UpdatePartyOwnerOnSwitch_NonMulti(gBattlerTarget);
            SwitchPartyOrderLinkMulti(gBattlerTarget, i, 0);
            SwitchPartyOrderLinkMulti(BATTLE_PARTNER(gBattlerTarget), i, 1);
        }
    }
    else
	{
		if (gBattleMons[gBattlerAttacker].level < gBattleMons[gBattlerTarget].level)
			gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1); // Fail if attacker level < Defender level
		else
			ForceSwitchOut(); // Success force switch
	}
}

static void atk90_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk91_givepaydaymoney(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK) && gPaydayMoney)
    {
        u32 bonusMoney = gPaydayMoney * gBattleStruct->moneyMultiplier;

        AddMoney(&gSaveBlock1Ptr->money, bonusMoney);
        PrepareHWordNumberBuffer(gBattleTextBuff1, 5, bonusMoney);
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_PrintPayDayMoneyString;
    }
    else
        ++gBattlescriptCurrInstr;
}

static void atk92_trysetlightscreen(void)
{
    if (gSideStatuses[GetBattlerSide(gBattlerAttacker)] & SIDE_STATUS_LIGHTSCREEN)
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gSideStatuses[GetBattlerSide(gBattlerAttacker)] |= SIDE_STATUS_LIGHTSCREEN;
        gSideTimers[GetBattlerSide(gBattlerAttacker)].lightscreenTimer = 5;
        gSideTimers[GetBattlerSide(gBattlerAttacker)].lightscreenBattlerId = gBattlerAttacker;
		PrepareMoveBuffer(gBattleTextBuff1, MOVE_LIGHT_SCREEN);
		PrepareStatBuffer(gBattleTextBuff2, STAT_SPDEF);
		gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_REFLECT_RAISED;
		gBattlescriptCurrInstr += 5;
    }
}

static bool8 CalcKOChance(void)
{
	u16 chance = gBattleMoves[gCurrentMove].accuracy + (gBattleMons[gBattlerAttacker].level - gBattleMons[gBattlerTarget].level);
	
	if (Random() % 100 + 1 < chance && gBattleMons[gBattlerAttacker].level >= gBattleMons[gBattlerTarget].level)
		return TRUE;
	else
		return FALSE;
}

static void atk93_tryKO(void)
{
    u8 holdEffect = GetBattlerItemHoldEffect(gBattlerTarget, TRUE), param = ItemId_GetHoldEffectParam(gBattleMons[gBattlerTarget].item);
    bool8 chance;
	
    if (holdEffect == HOLD_EFFECT_FOCUS_BAND && (Random() % 100) < param)
    {
        RecordItemEffectBattle(gBattlerTarget, HOLD_EFFECT_FOCUS_BAND);
        gSpecialStatuses[gBattlerTarget].focusBanded = 1;
    }

    if (GetBattlerAbility(gBattlerTarget) == ABILITY_STURDY)
    {
        gMoveResultFlags |= MOVE_RESULT_FOE_STURDIED;
        gLastUsedAbility = ABILITY_STURDY;
        gBattlescriptCurrInstr = BattleScript_SturdyPreventsOHKO;
        RecordAbilityBattle(gBattlerTarget, ABILITY_STURDY);
    }
    else
    {
        if (!(gStatuses3[gBattlerAttacker] & STATUS3_ALWAYS_HITS))
            chance = CalcKOChance();
        else if (gDisableStructs[gBattlerAttacker].battlerWithSureHit == gBattlerTarget && gBattleMons[gBattlerAttacker].level >= gBattleMons[gBattlerTarget].level)
            chance = TRUE;
        else
            chance = CalcKOChance();
		
        if (chance)
        {
            if (gProtectStructs[gBattlerTarget].endured)
            {
                gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - 1;
                gMoveResultFlags |= MOVE_RESULT_FOE_ENDURED;
            }
            else if (gSpecialStatuses[gBattlerTarget].focusBanded)
            {
                gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - 1;
                gMoveResultFlags |= MOVE_RESULT_FOE_HUNG_ON;
                gLastUsedItem = gBattleMons[gBattlerTarget].item;
            }
            else
            {
                gBattleMoveDamage = gBattleMons[gBattlerTarget].hp;
                gMoveResultFlags |= MOVE_RESULT_ONE_HIT_KO;
            }
            gBattlescriptCurrInstr += 5;
        }
        else
        {
            gMoveResultFlags |= MOVE_RESULT_MISSED;
		
            if (gBattleMons[gBattlerAttacker].level >= gBattleMons[gBattlerTarget].level)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_KO_MISSED;
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_UNAFFECTED;
			
            gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
        }
    }
}

static void atk94_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk95_setsandstorm(void)
{
    if (!TryChangeBattleWeather(gBattlerAttacker, ENUM_WEATHER_SANDSTORM))
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEATHER_MOVE_FAIL;
    }
    else
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SANDSTORM_UP;
	
    ++gBattlescriptCurrInstr;
}

static void atk96_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk97_tryinfatuating(void)
{
	u8 battler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	switch (CanBeInfatuatedBy(battler, gBattlerAttacker))
	{
		case STATUS_CHANGE_WORKED:
		    gBattleMons[battler].status2 |= STATUS2_INFATUATION;
			gDisableStructs[battler].infatuatedWith = gBattlerAttacker;
		    gBattlescriptCurrInstr += 2;
			break;
		case STATUS_CHANGE_FAIL_ABILITY_PREVENTED:
		    gLastUsedAbility = GetBattlerAbility(battler);
			RecordAbilityBattle(battler, gLastUsedAbility);
		    gBattlescriptCurrInstr = BattleScript_ObliviousPrevents;
			break;
		case STATUS_CHANGE_FAIL_AROMA_VEIL_ON_SIDE:
		    gBattleScripting.battler = gBattleScripting.savedBattler;
		    RecordAbilityBattle(battler, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_AromaVeilProtects;
			break;
		case STATUS_CHANGE_FAIL_ALREADY_STATUSED:
		case STATUS_CHANGE_FAIL_TYPE_NOT_AFFECTED: // gender fail
		    gBattlescriptCurrInstr = BattleScript_ButItFailed;
			break;
	}
}

static void atk98_updatestatusicon(void)
{
	u8 battlerId;
	
    if (!gBattleControllerExecFlags)
    {
        if (gBattlescriptCurrInstr[1] == BS_PLAYER2)
        {
            for (battlerId = gBattleControllerExecFlags; battlerId < gBattlersCount; ++battlerId)
            {
                if (!(gAbsentBattlerFlags & gBitTable[battlerId]))
                {
                    BtlController_EmitStatusIconUpdate(battlerId, BUFFER_A, gBattleMons[battlerId].status1, gBattleMons[battlerId].status2);
                    MarkBattlerForControllerExec(battlerId);
                }
            }
            gBattlescriptCurrInstr += 2;
        }
        else if (gBattlescriptCurrInstr[1] == BS_ATTACKER_WITH_PARTNER)
        {
            if (!(gAbsentBattlerFlags & gBitTable[gBattlerAttacker]))
            {
                BtlController_EmitStatusIconUpdate(gBattlerAttacker, BUFFER_A, gBattleMons[gBattlerAttacker].status1, gBattleMons[gBattlerAttacker].status2);
                MarkBattlerForControllerExec(gBattlerAttacker);
            }
            if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
            {
                battlerId = GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gBattlerAttacker)));
                if (!(gAbsentBattlerFlags & gBitTable[battlerId]))
                {
                    BtlController_EmitStatusIconUpdate(battlerId, BUFFER_A, gBattleMons[battlerId].status1, gBattleMons[battlerId].status2);
                    MarkBattlerForControllerExec(battlerId);
                }
            }
            gBattlescriptCurrInstr += 2;
        }
        else
        {
            battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
            BtlController_EmitStatusIconUpdate(battlerId, BUFFER_A, gBattleMons[battlerId].status1, gBattleMons[battlerId].status2);
            MarkBattlerForControllerExec(battlerId);
            gBattlescriptCurrInstr += 2;
        }
    }
}

static void atk99_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk9A_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atk9B_transformdataexecution(void)
{
    u8 i, *battleMonAttacker, *battleMonTarget;
	u16 oldAbility;
	
    gChosenMove = MOVE_UNVAILABLE;
	
    if (!CanTransformIntoBattler(gBattlerAttacker, gBattlerTarget))
		gBattlescriptCurrInstr = BattleScript_ButItFailed;
    else
    {
		gBattleMons[gBattlerAttacker].status2 |= STATUS2_TRANSFORMED;
		CopyBattlerCritModifier(gBattlerAttacker, gBattlerTarget); // Copy Focus Energy
        gDisableStructs[gBattlerAttacker].disabledMove = MOVE_NONE;
        gDisableStructs[gBattlerAttacker].disableTimer = 0;
        gDisableStructs[gBattlerAttacker].transformedMonPersonality = gBattleMons[gBattlerTarget].personality;
		gDisableStructs[gBattlerAttacker].transformedMonShynies = GetMonData(GetBattlerPartyIndexPtr(gBattlerTarget), MON_DATA_IS_SHINY);
        gDisableStructs[gBattlerAttacker].mimickedMoves = 0;
		
        PrepareSpeciesBuffer(gBattleTextBuff1, gBattleMons[gBattlerTarget].species);
		
		oldAbility = gBattleMons[gBattlerAttacker].ability; // Save ability
		
		battleMonAttacker = (u8 *)(&gBattleMons[gBattlerAttacker]);
        battleMonTarget = (u8 *)(&gBattleMons[gBattlerTarget]);
		
		for (i = 0; i < offsetof(struct BattlePokemon, pp); i++)
			battleMonAttacker[i] = battleMonTarget[i];
		
		gBattleMons[gBattlerAttacker].ability = oldAbility; // Restore ability, nessesary for SetBattlerAbility
		SetBattlerAbility(gBattlerAttacker, gBattleMons[gBattlerTarget].ability);
		
        for (i = 0; i < MAX_MON_MOVES; ++i)
        {
            if (gBattleMoves[gBattleMons[gBattlerAttacker].moves[i]].pp < 5)
                gBattleMons[gBattlerAttacker].pp[i] = gBattleMoves[gBattleMons[gBattlerAttacker].moves[i]].pp;
            else
                gBattleMons[gBattlerAttacker].pp[i] = 5;
        }
        BtlController_EmitResetActionMoveSelection(gBattlerAttacker, BUFFER_A, RESET_MOVE_SELECTION);
        MarkBattlerForControllerExec(gBattlerAttacker);
		++gBattlescriptCurrInstr;
    }
}

static void atk9C_setsubstitute(void)
{
    u32 hp;

	if (gBattleMons[gBattlerAttacker].status2 & STATUS2_SUBSTITUTE)
		gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_HAS_SUBSTITUTE;
	else
	{
		hp = gBattleMons[gBattlerAttacker].maxHP / 4;
		
		if (hp == 0)
			hp = 1;
		
		if (gBattleMons[gBattlerAttacker].hp <= hp)
			gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_TOO_WEAK_TO_SUBSTITUTE;
		else
		{
			gBattleMons[gBattlerAttacker].status2 |= STATUS2_SUBSTITUTE;
			gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_WRAPPED);
			gDisableStructs[gBattlerAttacker].substituteHP = gBattleMoveDamage = hp;
			gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MADE_SUBSTITUTE;
			gHitMarker |= (HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_IGNORE_DISGUISE);
		}
	}
	++gBattlescriptCurrInstr;
}

static void atk9D_mimicattackcopy(void)
{ 
    u8 i;
	
    gChosenMove = MOVE_UNVAILABLE;
	
    if (!gLastMoves[gBattlerTarget] || gLastMoves[gBattlerTarget] == MOVE_UNVAILABLE || gBattleMoves[gLastMoves[gBattlerTarget]].flags.forbiddenMimic
	|| gBattleMons[gBattlerAttacker].status2 & STATUS2_TRANSFORMED)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        for (i = 0; i < MAX_MON_MOVES; ++i)
        {
            if (gBattleMons[gBattlerAttacker].moves[i] == gLastMoves[gBattlerTarget])
                break;
        }
        if (i == MAX_MON_MOVES)
        {
            gBattleMons[gBattlerAttacker].moves[gCurrMovePos] = gLastMoves[gBattlerTarget];
			gBattleMons[gBattlerAttacker].pp[gCurrMovePos] = gBattleMoves[gLastMoves[gBattlerTarget]].pp;
            PrepareMoveBuffer(gBattleTextBuff1, gLastMoves[gBattlerTarget]);
            gDisableStructs[gBattlerAttacker].mimickedMoves |= gBitTable[gCurrMovePos];
            gBattlescriptCurrInstr += 5;
        }
        else
            gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void atk9E_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atk9F_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkA0_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkA1_trycounterattack(void)
{
	u8 battlerId, sideTarget;
	u32 dmg;
	
	switch (gBattlescriptCurrInstr[1])
	{
		case SPLIT_PHYSICAL:
			battlerId = gProtectStructs[gBattlerAttacker].physicalBattlerId;
			dmg = gProtectStructs[gBattlerAttacker].physicalDmg;
			break;
		case SPLIT_SPECIAL:
			battlerId = gProtectStructs[gBattlerAttacker].specialBattlerId;
			dmg = gProtectStructs[gBattlerAttacker].specialDmg;
			break;
	}
	sideTarget = GetBattlerSide(battlerId);
	
	if (dmg && GetBattlerSide(gBattlerAttacker) != sideTarget && IsBattlerAlive(battlerId))
	{
		gBattleMoveDamage = dmg * 2;
		
		if (IsBattlerAffectedByFollowMe(gBattlerAttacker, sideTarget, gCurrentMove))
			gBattlerTarget = gSideTimers[sideTarget].followmeTarget;
		else
			gBattlerTarget = battlerId;
		
		gBattlescriptCurrInstr += 6;
	}
	else
	{
		gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 2);
	}
}

static void atkA2_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkA3_disablelastusedattack(void)
{
    u8 i, battler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);

    for (i = 0; i < MAX_MON_MOVES; ++i)
    {
        if (gBattleMons[battler].moves[i] == gLastMoves[battler])
            break;
    }
	if (i != MAX_MON_MOVES && CanDisableMove(battler, i, gBattleMons[battler].moves[i]))
    {
        PrepareMoveBuffer(gBattleTextBuff1, gBattleMons[battler].moves[i]);
        gDisableStructs[battler].disabledMove = gBattleMons[battler].moves[i];
        gDisableStructs[battler].disableTimer = 4;
        gBattlescriptCurrInstr += 6;
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 2);
}

static void atkA4_trysetencore(void)
{
    u8 i;
	u16 lastMove = gLastMoves[gBattlerTarget];

    for (i = 0; i < MAX_MON_MOVES; ++i)
	{
        if (gBattleMons[gBattlerTarget].moves[i] == lastMove)
            break;
	}
    if (lastMove == MOVE_STRUGGLE || lastMove == MOVE_ENCORE || lastMove == MOVE_MIRROR_MOVE || lastMove == MOVE_TRANSFORM || lastMove == MOVE_MIMIC
	|| lastMove == MOVE_SKETCH || lastMove == MOVE_DYNAMAX_CANNON || gBattleMoves[lastMove].flags.callOtherMove)
        i = MAX_MON_MOVES;
	
    if (!gDisableStructs[gBattlerTarget].encoredMove && i != MAX_MON_MOVES && gBattleMons[gBattlerTarget].pp[i])
    {
        gDisableStructs[gBattlerTarget].encoredMove = gBattleMons[gBattlerTarget].moves[i];
        gDisableStructs[gBattlerTarget].encoredMovePos = i;
        gDisableStructs[gBattlerTarget].encoreTimer = 3;
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkA5_painsplitdmgcalc(void)
{
    if (!SubsBlockMove(gBattlerAttacker, gBattlerTarget, gCurrentMove))
    {
        s32 hpDiff = (gBattleMons[gBattlerAttacker].hp + gBattleMons[gBattlerTarget].hp) / 2;
		s32 painSplitHp = gBattleMons[gBattlerTarget].hp - hpDiff;
		u8 *storeLoc = (void *)(&gBattleScripting.painSplitHp);

        storeLoc[0] = (painSplitHp);
        storeLoc[1] = (painSplitHp & 0x0000FF00) >> 8;
        storeLoc[2] = (painSplitHp & 0x00FF0000) >> 16;
        storeLoc[3] = (painSplitHp & 0xFF000000) >> 24;
        gBattleMoveDamage = gBattleMons[gBattlerAttacker].hp - hpDiff;
        gSpecialStatuses[gBattlerTarget].dmg = 0xFFFF;
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkA6_settypetorandomresistance(void) // conversion 2
{
	u8 i, lastUsedMoveType;
    u32 resistTypes;
	
	if (!gLastMoves[gBattlerTarget] || gLastMoves[gBattlerTarget] == MOVE_UNVAILABLE || gLastMoves[gBattlerTarget] == MOVE_STRUGGLE)
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
	else
	{
		lastUsedMoveType = gLastUsedMovesTypes[gBattlerTarget];
		
		for (i = 0, resistTypes = 0; i < NUMBER_OF_MON_TYPES; i++)
		{
			switch (GetTypeModifier(lastUsedMoveType, i))
			{
				case TYPE_MUL_NO_EFFECT:
				case TYPE_MUL_NOT_EFFECTIVE:
				    resistTypes |= gBitTable[i];
				    break;
			}
		}
		
		while (resistTypes != 0)
		{
			i = Random() % NUMBER_OF_MON_TYPES;
			
			if (resistTypes & gBitTable[i])
			{
				if (IS_BATTLER_OF_TYPE(gBattlerAttacker, i))
					resistTypes &= ~(gBitTable[i]);
				else
				{
					SetBattlerType(gBattlerAttacker, i);
					gBattlescriptCurrInstr += 5;
					return;
				}
			}
		}
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
	}
}

static void atkA7_trysetalwayshitflag(void)
{
	if ((gStatuses3[gBattlerAttacker] & STATUS3_ALWAYS_HITS) && gDisableStructs[gBattlerAttacker].battlerWithSureHit == gBattlerTarget)
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
	else
	{
		gStatuses3[gBattlerAttacker] &= ~(STATUS3_ALWAYS_HITS);
		gStatuses3[gBattlerAttacker] |= STATUS3_ALWAYS_HITS_TURN(2);
		gDisableStructs[gBattlerAttacker].battlerWithSureHit = gBattlerTarget;
		gBattlescriptCurrInstr += 5;
	}
}

static void atkA8_copymovepermanently(void) // sketch
{
    u8 i;
	
    gChosenMove = MOVE_UNVAILABLE;
	
    if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_TRANSFORMED) && gLastPrintedMoves[gBattlerTarget] && gLastPrintedMoves[gBattlerTarget] != MOVE_UNVAILABLE
	&& gLastPrintedMoves[gBattlerTarget] != MOVE_STRUGGLE && gLastPrintedMoves[gBattlerTarget] != MOVE_CHATTER && gLastPrintedMoves[gBattlerTarget] != gCurrentMove)
    {
        for (i = 0; i < MAX_MON_MOVES; ++i)
		{
            if (gBattleMons[gBattlerAttacker].moves[i] != gCurrentMove && gBattleMons[gBattlerAttacker].moves[i] == gLastPrintedMoves[gBattlerTarget])
                break;
		}
        if (i != MAX_MON_MOVES)
            gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
        else // sketch worked
        {
            struct MovePpInfo movePpData;

            gBattleMons[gBattlerAttacker].moves[gCurrMovePos] = gLastPrintedMoves[gBattlerTarget];
            gBattleMons[gBattlerAttacker].pp[gCurrMovePos] = gBattleMoves[gLastPrintedMoves[gBattlerTarget]].pp;
		
            for (i = 0; i < MAX_MON_MOVES; ++i)
            {
                movePpData.moves[i] = gBattleMons[gBattlerAttacker].moves[i];
                movePpData.pp[i] = gBattleMons[gBattlerAttacker].pp[i];
            }
            movePpData.ppBonuses = gBattleMons[gBattlerAttacker].ppBonuses;
            BtlController_EmitSetMonData(gBattlerAttacker, BUFFER_A, REQUEST_MOVES_PP_BATTLE, 0, sizeof(struct MovePpInfo), &movePpData);
            MarkBattlerForControllerExec(gBattlerAttacker);
            PrepareMoveBuffer(gBattleTextBuff1, gLastPrintedMoves[gBattlerTarget]);
            gBattlescriptCurrInstr += 5;
        }
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkA9_trychoosesleeptalkmove(void)
{
    u8 i, movePosition, unusableMovesBits = 0;

    for (i = 0; i < MAX_MON_MOVES; ++i)
    {
        if (!gBattleMons[gBattlerAttacker].moves[i] || gBattleMoves[gBattleMons[gBattlerAttacker].moves[i]].flags.forbiddenSleepTalk
		|| gBattleMoves[gBattleMons[gBattlerAttacker].moves[i]].flags.twoTurnsMove)
            unusableMovesBits |= gBitTable[i];
    }
    unusableMovesBits |= CheckMoveLimitations(gBattlerAttacker, (MOVE_LIMITATION_IGNORE_NO_PP | MOVE_LIMITATION_IGNORE_IMPRISON));
	
    if (unusableMovesBits == MOVE_LIMITATION_ALL_MOVES_MASK) // all 4 moves cannot be chosen
	    gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else // at least one move can be chosen
    {
        do
            movePosition = Random() % MAX_MON_MOVES;
        while ((gBitTable[movePosition] & unusableMovesBits));
		
		gCurrMovePos = movePosition;
        gCalledMove = gBattleMons[gBattlerAttacker].moves[movePosition];
        gBattlescriptCurrInstr += 5;
    }
}

static void atkAA_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkAB_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkAC_trysetburn(void)
{
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	switch (CanBeBurned(gBattlerAttacker, battlerId, 0))
	{
		case STATUS_CHANGE_WORKED:
			if (SubsBlockMove(gBattlerAttacker, battlerId, gCurrentMove))
				gBattlescriptCurrInstr = BattleScript_ButItFailed;
			else
				gBattlescriptCurrInstr += 2;
			break;
		case STATUS_CHANGE_FAIL_SPECIFIC_STATUSED:
			gBattlescriptCurrInstr = BattleScript_AlreadyBurned;
			break;
		case STATUS_CHANGE_FAIL_ALREADY_STATUSED:
			gBattlescriptCurrInstr = BattleScript_ButItFailed;
			break;
		case STATUS_CHANGE_FAIL_TYPE_NOT_AFFECTED:
			gBattlescriptCurrInstr = BattleScript_NotAffected;
			break;
		case STATUS_CHANGE_FAIL_SAFEGUARD_PROTECTED:
			gBattlescriptCurrInstr = BattleScript_SafeguardProtected;
			break;
		case STATUS_CHANGE_FAIL_ABILITY_PREVENTED:
			gLastUsedAbility = GetBattlerAbility(battlerId);
			gBattlescriptCurrInstr = BattleScript_WaterVeilPrevents;
			break;
		case STATUS_CHANGE_FAIL_FLOWER_VEIL_ON_SIDE:
			gBattleScripting.battler = gBattleScripting.savedBattler;
			gLastUsedAbility = GetBattlerAbility(gBattleScripting.battler);
			RecordAbilityBattle(gBattleScripting.battler, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_TeamProtectedByFlowerVeil;
			break;
	}
}

static void atkAD_tryspiteppreduce(void)
{
    u8 i, ppToDeduct;
	
    if (gLastMoves[gBattlerTarget] && gLastMoves[gBattlerTarget] != MOVE_UNVAILABLE)
    {
        for (i = 0; i < MAX_MON_MOVES; ++i)
		{
            if (gLastMoves[gBattlerTarget] == gBattleMons[gBattlerTarget].moves[i])
                break;
		}
        if (i != MAX_MON_MOVES && gBattleMons[gBattlerTarget].pp[i])
        {
            ppToDeduct = gBattleMoves[gCurrentMove].argument;

            if (gBattleMons[gBattlerTarget].pp[i] < ppToDeduct)
                ppToDeduct = gBattleMons[gBattlerTarget].pp[i];
			
            PrepareMoveBuffer(gBattleTextBuff1, gLastMoves[gBattlerTarget]);
            ConvertIntToDecimalStringN(gBattleTextBuff2, ppToDeduct, STR_CONV_MODE_LEFT_ALIGN, 1);
            PrepareByteNumberBuffer(gBattleTextBuff2, 1, ppToDeduct);
            gBattleMons[gBattlerTarget].pp[i] -= ppToDeduct;
			
            if (!(gDisableStructs[gBattlerTarget].mimickedMoves & gBitTable[i]) && !(gBattleMons[gBattlerTarget].status2 & STATUS2_TRANSFORMED))
            {
                BtlController_EmitSetMonData(gBattlerTarget, BUFFER_A, REQUEST_PPMOVE1_BATTLE + i, 0, 1, &gBattleMons[gBattlerTarget].pp[i]);
                MarkBattlerForControllerExec(gBattlerTarget);
            }
			
			if (!gBattleMons[gBattlerTarget].pp[i])
                CancelMultiTurnMoves(gBattlerTarget);
			
            gBattlescriptCurrInstr += 5;
			return;
        }
    }
	gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkAE_healpartystatus(void)
{
	u8 partyId;
	struct Pokemon *mon;
	
	switch (gBattlescriptCurrInstr[1])
	{
		case 0: // Choose string
			switch (gCurrentMove)
			{
				case MOVE_HEAL_BELL:
				    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_BELL_CHIMED;
					break;
				case MOVE_AROMATHERAPY:
				    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SOOTHING_AROMA;
					break;
			}
			gBattlescriptCurrInstr += 2;
			break;
		case 1: // Heal status
		    while (gBattleCommunication[MULTIUSE_STATE] < PARTY_SIZE)
			{
				partyId = gBattleCommunication[MULTIUSE_STATE]++;
				
				if (partyId == gBattlerPartyIndexes[gBattlerAttacker])
				{
					gBattleScripting.battler = gBattlerAttacker;
					
					if (GetBattlerAbility(gBattleScripting.battler) == ABILITY_SOUNDPROOF && gBattleMoves[gCurrentMove].flags.soundMove)
					{
						BattleScriptCall(BattleScript_SoundproofBlocksHealBell);
						break;
					}
					else if ((gBattleMons[gBattleScripting.battler].status1 & STATUS1_ANY))
					{
						PrepareMonNickBuffer(gBattleTextBuff2, gBattleScripting.battler, partyId);
						BattleScriptCall(BattleScript_HealBellActivateOnBattler);
						break;
					}
				}
				else if (IsBattlerAlive(BATTLE_PARTNER(gBattlerAttacker)) && partyId == gBattlerPartyIndexes[BATTLE_PARTNER(gBattlerAttacker)])
				{
					gBattleScripting.battler = BATTLE_PARTNER(gBattlerAttacker);
					
					if (GetBattlerAbility(gBattleScripting.battler) == ABILITY_SOUNDPROOF && gBattleMoves[gCurrentMove].flags.soundMove)
					{
						BattleScriptCall(BattleScript_SoundproofBlocksHealBell);
						break;
					}
					else if ((gBattleMons[gBattleScripting.battler].status1 & STATUS1_ANY))
					{
						PrepareMonNickBuffer(gBattleTextBuff2, gBattleScripting.battler, partyId);
						BattleScriptCall(BattleScript_HealBellActivateOnBattler);
						break;
					}
				}
				else
				{
					mon = &GetBattlerParty(gBattlerAttacker)[partyId];
					
					if (IsMonValidSpecies(mon) && !HealStatusConditions(mon, STATUS1_ANY, MAX_BATTLERS_COUNT))
					{
						PrepareMonNickBuffer(gBattleTextBuff2, gBattlerAttacker, partyId);
						BattleScriptCall(BattleScript_HealBellActivateOnParty);
						break;
					}
				}
			}
			if (gBattleCommunication[MULTIUSE_STATE] >= PARTY_SIZE)
				gBattlescriptCurrInstr += 2;
			break;
	}
}

static void atkAF_cursetarget(void)
{
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_CURSED)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gBattleMons[gBattlerTarget].status2 |= STATUS2_CURSED;
		
        gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 2;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
		
        gBattlescriptCurrInstr += 5;
    }
}

static void atkB0_trysetspikes(void)
{
    u8 targetSide = GetBattlerSide(BATTLE_OPPOSITE(gBattlerAttacker));

    if (gSideTimers[targetSide].spikesAmount == 3)
    {
        gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        gSideStatuses[targetSide] |= SIDE_STATUS_SPIKES;
        ++gSideTimers[targetSide].spikesAmount;
        gBattlescriptCurrInstr += 5;
    }
}

static void atkB1_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkB2_trysetperishsong(void)
{
    u8 i, notAffectedCount;

    for (i = 0, notAffectedCount = 0; i < gBattlersCount; ++i)
    {
        if (gStatuses3[i] & STATUS3_PERISH_SONG || (GetBattlerAbility(i) == ABILITY_SOUNDPROOF && i != gBattlerAttacker)) // Can't prevent the user
            ++notAffectedCount;
        else
        {
            gStatuses3[i] |= STATUS3_PERISH_SONG;
            gDisableStructs[i].perishSongTimer = 3;
        }
    }
    PressurePPLoseOnUsingPerishSong(gBattlerAttacker);
	
    if (notAffectedCount == gBattlersCount)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
        gBattlescriptCurrInstr += 5;
}

static void atkB3_handlerollout(void)
{
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
    {
        CancelMultiTurnMoves(gBattlerAttacker);
        gBattlescriptCurrInstr = BattleScript_MoveMissedPause;
    }
    else
    {
        if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS)) // first hit
        {
            gDisableStructs[gBattlerAttacker].rolloutTimer = 5;
            gBattleMons[gBattlerAttacker].status2 |= STATUS2_MULTIPLETURNS;
            gLockedMoves[gBattlerAttacker] = gCurrentMove;
        }
        if (--gDisableStructs[gBattlerAttacker].rolloutTimer == 0) // last hit
            gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_MULTIPLETURNS);
		
        ++gBattlescriptCurrInstr;
    }
}

static void atkB4_jumpifconfusedandstatmaxed(void)
{
    if ((gBattleMons[gBattlerTarget].status2 & STATUS2_CONFUSION) && !CompareStat(gBattlerTarget, gBattlescriptCurrInstr[1], MAX_STAT_STAGES, CMP_LESS_THAN))
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 2);
    else
        gBattlescriptCurrInstr += 6;
}

static void atkB5_handlefurycutter(void)
{
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
    {
        gDisableStructs[gBattlerAttacker].furyCutterCounter = 0;
        gBattlescriptCurrInstr = BattleScript_MoveMissedPause;
    }
    else
    {
		if (gLastPrintedMoves[gBattlerAttacker] != gCurrentMove) // reset counter if another move was selected
		    gDisableStructs[gBattlerAttacker].furyCutterCounter = 0;
		
		// Don't increase counter on first hit of Parental Bond
        if (gDisableStructs[gBattlerAttacker].furyCutterCounter != 3 && gSpecialStatuses[gBattlerAttacker].parentalBondState != PARENTAL_BOND_1ST_HIT)
            ++gDisableStructs[gBattlerAttacker].furyCutterCounter;
		
		++gBattlescriptCurrInstr;
    }
}

static void atkB6_trysetparalyze(void)
{
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	switch (CanBeParalyzed(gBattlerAttacker, battlerId, 0))
	{
		case STATUS_CHANGE_WORKED:
		    if (SubsBlockMove(gBattlerAttacker, battlerId, gCurrentMove))
				gBattlescriptCurrInstr = BattleScript_ButItFailed;
			else
				gBattlescriptCurrInstr += 2;
			break;
		case STATUS_CHANGE_FAIL_SPECIFIC_STATUSED:
			gBattlescriptCurrInstr = BattleScript_AlreadyParalyzed;
			break;
		case STATUS_CHANGE_FAIL_ALREADY_STATUSED:
			gBattlescriptCurrInstr = BattleScript_ButItFailed;
			break;
		case STATUS_CHANGE_FAIL_TYPE_NOT_AFFECTED:
			gBattlescriptCurrInstr = BattleScript_NotAffected;
			break;
		case STATUS_CHANGE_FAIL_SAFEGUARD_PROTECTED:
			gBattlescriptCurrInstr = BattleScript_SafeguardProtected;
			break;
		case STATUS_CHANGE_FAIL_ABILITY_PREVENTED:
			gLastUsedAbility = GetBattlerAbility(battlerId);
			RecordAbilityBattle(battlerId, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_LimberProtected;
			break;
		case STATUS_CHANGE_FAIL_FLOWER_VEIL_ON_SIDE:
			gBattleScripting.battler = gBattleScripting.savedBattler;
			gLastUsedAbility = GetBattlerAbility(gBattleScripting.battler);
			RecordAbilityBattle(gBattleScripting.battler, gLastUsedAbility);
			gBattlescriptCurrInstr = BattleScript_TeamProtectedByFlowerVeil;
			break;
	}
}

static void atkB7_presentcalc(void)
{
    s32 rand;
	
	if (gSpecialStatuses[gBattlerAttacker].parentalBondState != PARENTAL_BOND_2ND_HIT)
	{
		rand = Random() & 0xFF;
		
		if (rand < 102)
            gBattleStruct->presentBasePower = 40;
        else if (rand < 178)
            gBattleStruct->presentBasePower = 80;
        else if (rand < 204)
            gBattleStruct->presentBasePower = 120;
        else
        {
            gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 4;
            if (gBattleMoveDamage == 0)
                gBattleMoveDamage = 1;
            gBattleMoveDamage *= -1;
            gBattleStruct->presentBasePower = 0;
        }
	}
	if (gBattleStruct->presentBasePower)
		gBattlescriptCurrInstr = BattleScript_HitFromCritCalc;
	else if (BATTLER_MAX_HP(gBattlerTarget))
		gBattlescriptCurrInstr = BattleScript_AlreadyAtFullHp;
	else
		gBattlescriptCurrInstr = BattleScript_PresentHealTarget;
}

static void atkB8_trysetsafeguard(void)
{
    if (gSideStatuses[GetBattlerSide(gBattlerAttacker)] & SIDE_STATUS_SAFEGUARD)
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gSideStatuses[GetBattlerSide(gBattlerAttacker)] |= SIDE_STATUS_SAFEGUARD;
        gSideTimers[GetBattlerSide(gBattlerAttacker)].safeguardTimer = 5;
        gSideTimers[GetBattlerSide(gBattlerAttacker)].safeguardBattlerId = gBattlerAttacker;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SAFEGUARD_COVERED;
		gBattlescriptCurrInstr += 5;
    }
}

static void atkB9_magnitudedamagecalculation(void)
{
    s32 magnitude;
    u8 power;
	
	if (!gBattleStruct->magnitudeBasePower) // Check power has't been calculated
	{
		magnitude = Random() % 100;
		
		if (magnitude < 5)
		{
			power = 10;
			magnitude = 4;
		}
		else if (magnitude < 15)
		{
			power = 30;
			magnitude = 5;
		}
		else if (magnitude < 35)
		{
			power = 50;
			magnitude = 6;
		}
		else if (magnitude < 65)
		{
			power = 70;
			magnitude = 7;
		}
		else if (magnitude < 85)
		{
			power = 90;
			magnitude = 8;
		}
		else if (magnitude < 95)
		{
			power = 110;
			magnitude = 9;
		}
		else
		{
			power = 150;
			magnitude = 10;
		}
		gBattleStruct->magnitudeBasePower = power;
		PrepareByteNumberBuffer(gBattleTextBuff1, 2, magnitude);
		BattleScriptPush(gBattlescriptCurrInstr + 1);
		gBattlescriptCurrInstr = BattleScript_MagnitudeString;
	}
	else
		++gBattlescriptCurrInstr;
}

static void atkBA_jumpifnopursuitswitchdmg(void)
{
    u8 i;
	
    if (gMultiHitCounter == 1)
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        else
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    }
    else
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        else
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
    }
	
    if (gChosenActionByBattler[gBattlerTarget] == B_ACTION_USE_MOVE && !(gBattleMons[gBattlerTarget].status1 & (STATUS1_SLEEP | STATUS1_FREEZE))
	&& IsBattlerAlive(gBattlerAttacker) && !gDisableStructs[gBattlerTarget].truantCounter && gBattleMoves[gChosenMoveByBattler[gBattlerTarget]].effect == EFFECT_PURSUIT)
    {
        for (i = 0; i < gBattlersCount; ++i)
		{
            if (gBattlerByTurnOrder[i] == gBattlerTarget)
                gActionsByTurnOrder[i] = 11;
		}
        gCurrentMove = gChosenMoveByBattler[gBattlerTarget];
        gCurrMovePos = gChosenMovePos = *(gBattleStruct->chosenMovePositions + gBattlerTarget);
        gBattleScripting.animTurn = 1;
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
		gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkBB_setsunny(void)
{
    if (!TryChangeBattleWeather(gBattlerAttacker, ENUM_WEATHER_SUN))
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEATHER_MOVE_FAIL;
    }
    else
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SUN_TURN_HARSH;
	
    ++gBattlescriptCurrInstr;
}

static void atkBC_maxattackhalvehp(void) // belly drum
{
    u32 halfHp = gBattleMons[gBattlerAttacker].maxHP / 2;

    if (halfHp == 0)
        halfHp = 1;
	
    if (CompareStat(gBattlerAttacker, STAT_ATK, MAX_STAT_STAGES, CMP_LESS_THAN) && gBattleMons[gBattlerAttacker].hp > halfHp)
    {
		gBattleMons[gBattlerAttacker].statStages[STAT_ATK] = GetBattlerAbility(gBattlerAttacker) != ABILITY_CONTRARY ? MAX_STAT_STAGES : MIN_STAT_STAGES;
		gBattleMoveDamage = halfHp;
		gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkBD_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkBE_rapidspinfree(void)
{
    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_WRAPPED)
    {
        gBattleScripting.battler = gBattlerTarget;
        gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_WRAPPED);
        gBattlerTarget = gBattleStruct->wrappedBy[gBattlerAttacker];
		PrepareMoveBuffer(gBattleTextBuff1, gBattleStruct->wrappedMove[gBattlerAttacker]);
		BattleScriptCall(BattleScript_WrapFree);
    }
    else if (gStatuses3[gBattlerAttacker] & STATUS3_LEECHSEED)
    {
        gStatuses3[gBattlerAttacker] &= ~(STATUS3_LEECHSEED | STATUS3_LEECHSEED_BATTLER);
		BattleScriptCall(BattleScript_LeechSeedFree);
    }
    else if (gSideStatuses[GetBattlerSide(gBattlerAttacker)] & SIDE_STATUS_SPIKES)
    {
        gSideStatuses[GetBattlerSide(gBattlerAttacker)] &= ~(SIDE_STATUS_SPIKES);
        gSideTimers[GetBattlerSide(gBattlerAttacker)].spikesAmount = 0;
		BattleScriptCall(BattleScript_SpikesFree);
    }
    else
        ++gBattlescriptCurrInstr;
}

static void atkBF_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkC0_recoverbasedonweather(void)
{
	if (BATTLER_MAX_HP(gBattlerTarget))
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
	else
	{
		if (IsBattlerWeatherAffected(gBattlerTarget, WEATHER_SUN_ANY))
			gBattleMoveDamage = (20 * gBattleMons[gBattlerTarget].maxHP) / 30; // Sun
		else if (IsBattlerWeatherAffected(gBattlerTarget, WEATHER_ANY & ~(WEATHER_STRONG_WINDS)))
			gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 4; // Any other weather, except Strong Winds
		else
			gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 2; // No Weather or Strong Winds
		
		if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattleMoveDamage *= -1;
		
		gBattlescriptCurrInstr += 5;
	}
}

static void atkC1_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkC2_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkC3_trysetfutureattack(void)
{
    if (gWishFutureKnock.futureSightCounter[gBattlerTarget])
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gWishFutureKnock.futureSightMove[gBattlerTarget] = gCurrentMove;
        gWishFutureKnock.futureSightAttacker[gBattlerTarget] = gBattlerAttacker;
        gWishFutureKnock.futureSightCounter[gBattlerTarget] = 3;
		AddBattleEffectToQueueList(gBattlerTarget, B_QUEUED_FUTURE_SIGHT);
		GetFutureAttackStringId(gCurrentMove);
        gBattlescriptCurrInstr += 5;
    }
}

static void atkC4_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkC5_setsemiinvulnerablebit(void)
{
    switch (gCurrentMove)
    {
		case MOVE_DIG:
			gStatuses3[gBattlerAttacker] |= STATUS3_UNDERGROUND;
			break;
		case MOVE_DIVE:
			gStatuses3[gBattlerAttacker] |= STATUS3_UNDERWATER;
			break;
		default:
			gStatuses3[gBattlerAttacker] |= STATUS3_ON_AIR;
			break;
    }
    ++gBattlescriptCurrInstr;
}

static void atkC6_clearsemiinvulnerablebit(void)
{
    switch (gCurrentMove)
    {
		case MOVE_DIG:
			gStatuses3[gBattlerAttacker] &= ~(STATUS3_UNDERGROUND);
			break;
		case MOVE_DIVE:
			gStatuses3[gBattlerAttacker] &= ~(STATUS3_UNDERWATER);
			break;
		default:
			gStatuses3[gBattlerAttacker] &= ~(STATUS3_ON_AIR);
			break;
    }
    ++gBattlescriptCurrInstr;
}

static void atkC7_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkC8_sethail(void)
{
    if (!TryChangeBattleWeather(gBattlerAttacker, ENUM_WEATHER_HAIL))
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEATHER_MOVE_FAIL;
    }
    else
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STARTED_HAIL;
	
    ++gBattlescriptCurrInstr;
}

static void atkC9_jumpifattackandspecialattackcannotfall(void) // memento
{
    if (!CompareStat(gBattlerTarget, STAT_ATK, MIN_STAT_STAGES, CMP_GREATER_THAN) && !CompareStat(gBattlerTarget, STAT_SPATK, MIN_STAT_STAGES, CMP_GREATER_THAN)
		&& gBattleCommunication[MISS_TYPE] != B_MSG_PROTECTED)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
        gBattlescriptCurrInstr += 5;
}

static void atkCA_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkCB_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkCC_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkCD_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkCE_settorment(void)
{
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_TORMENT)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gBattleMons[gBattlerTarget].status2 |= STATUS2_TORMENT;
        gBattlescriptCurrInstr += 5;
    }
}

static void atkCF_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkD0_settaunt(void)
{
    if (!gDisableStructs[gBattlerTarget].tauntTimer)
    {
        gDisableStructs[gBattlerTarget].tauntTimer = (GetBattlerTurnOrderNum(gBattlerTarget) > GetBattlerTurnOrderNum(gBattlerAttacker)) ? 3 : 4;
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkD1_trysethelpinghand(void)
{
	gBattlerTarget = BATTLE_PARTNER(gBattlerAttacker);
	
	if (!IsBattlerAlive(gBattlerTarget))
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
	else
	{
		++gProtectStructs[gBattlerTarget].helpingHandUses;
		gBattlescriptCurrInstr += 5;
	}
}

void StealTargetItem(u8 battlerAtk, u8 battlerDef)
{
	gLastUsedItem = gBattleMons[battlerAtk].item = gBattleMons[battlerDef].item;
	gBattleMons[battlerDef].item = ITEM_NONE;
	
	CheckSetBattlerUnburden(battlerDef);
	gBattleResources->flags->flags[battlerAtk] &= ~(RESOURCE_FLAG_UNBURDEN_BOOST);
	
    BtlController_EmitSetMonData(battlerAtk, BUFFER_A, REQUEST_HELDITEM_BATTLE, 0, 2, &gLastUsedItem); // set attacker item
    MarkBattlerForControllerExec(battlerAtk);
	
    BtlController_EmitSetMonData(battlerDef, BUFFER_A, REQUEST_HELDITEM_BATTLE, 0, 2, &gBattleMons[battlerDef].item);  // remove target item
    MarkBattlerForControllerExec(battlerDef);

    gBattleStruct->choicedMove[battlerDef] = 0;
}

static void atkD2_tryswapitems(void) // trick
{
    u8 sideAttacker = GetBattlerSide(gBattlerAttacker), sideTarget = GetBattlerSide(gBattlerTarget);
    u16 oldItemAtk, *newItemAtk;
	
    // opponent can't swap items with player in regular battles
    if ((GetBattlerSide(gBattlerAttacker) == B_SIDE_OPPONENT && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_EREADER_TRAINER))))
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        // you can't swap items if they were knocked off in regular battles
        if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_EREADER_TRAINER))
			&& (gWishFutureKnock.knockedOffMons[sideAttacker] & gBitTable[gBattlerPartyIndexes[gBattlerAttacker]]
            || gWishFutureKnock.knockedOffMons[sideTarget] & gBitTable[gBattlerPartyIndexes[gBattlerTarget]]))
            gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
        // can't swap if two pokemon don't have an item
        // or if either of them is an enigma berry or a mail
        else if ((gBattleMons[gBattlerAttacker].item == 0 && gBattleMons[gBattlerTarget].item == 0) || ItemIsMail(gBattleMons[gBattlerAttacker].item)
		 || ItemIsMail(gBattleMons[gBattlerTarget].item))
            gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
        // check if ability prevents swapping
        else if (GetBattlerAbility(gBattlerTarget) == ABILITY_STICKY_HOLD)
        {
            gBattlescriptCurrInstr = BattleScript_StickyHoldActivates;
            gLastUsedAbility = GetBattlerAbility(gBattlerTarget);
            RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
        }
        // took a while, but all checks passed and items can be safely swapped
        else
        {
            newItemAtk = &gBattleStruct->changedItems[gBattlerAttacker];
            oldItemAtk = gBattleMons[gBattlerAttacker].item;
            *newItemAtk = gBattleMons[gBattlerTarget].item;
            gBattleMons[gBattlerAttacker].item = 0;
            gBattleMons[gBattlerTarget].item = oldItemAtk;
            BtlController_EmitSetMonData(gBattlerAttacker, BUFFER_A, REQUEST_HELDITEM_BATTLE, 0, 2, newItemAtk);
            MarkBattlerForControllerExec(gBattlerAttacker);
            BtlController_EmitSetMonData(gBattlerTarget, BUFFER_A, REQUEST_HELDITEM_BATTLE, 0, 2, &gBattleMons[gBattlerTarget].item);
            MarkBattlerForControllerExec(gBattlerTarget);
            gBattleStruct->choicedMove[gBattlerTarget] = MOVE_NONE;
            gBattleStruct->choicedMove[gBattlerAttacker] = MOVE_NONE;
            gBattlescriptCurrInstr += 5;
            PrepareItemBuffer(gBattleTextBuff1, *newItemAtk);
            PrepareItemBuffer(gBattleTextBuff2, oldItemAtk);
            if (oldItemAtk != ITEM_NONE && *newItemAtk != ITEM_NONE)
                gBattleCommunication[MULTISTRING_CHOOSER] = 2; // attacker's item -> <- target's item
            else if (oldItemAtk == ITEM_NONE && *newItemAtk != ITEM_NONE)
			{
				if (GetBattlerAbility(gBattlerAttacker) == ABILITY_UNBURDEN && gBattleResources->flags->flags[gBattlerAttacker] & RESOURCE_FLAG_UNBURDEN_BOOST)
					gBattleResources->flags->flags[gBattlerAttacker] &= ~RESOURCE_FLAG_UNBURDEN_BOOST;
				
				gBattleCommunication[MULTISTRING_CHOOSER] = 0; // nothing -> <- target's item
			}
            else
			{
				CheckSetBattlerUnburden(gBattlerAttacker);
                gBattleCommunication[MULTISTRING_CHOOSER] = 1; // attacker's item -> <- nothing
			}
        }
    }
}

static void atkD3_copyability(void) // role play
{
	u8 attacker = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]), defender = GetBattlerForBattleScript(gBattlescriptCurrInstr[2]);
	
	gBattleStruct->abilityOverride[attacker] = SetBattlerAbility(attacker, gBattleMons[defender].ability);
	gLastUsedAbility = gBattleMons[defender].ability;
	gBattlescriptCurrInstr += 3;
}

static void atkD4_trywish(void)
{
    switch (gBattlescriptCurrInstr[1])
    {
		case 0: // use wish
			if (!gWishFutureKnock.wishCounter[gBattlerAttacker])
			{
				gWishFutureKnock.wishCounter[gBattlerAttacker] = 2;
				gWishFutureKnock.wishMonId[gBattlerAttacker] = gBattlerPartyIndexes[gBattlerAttacker];
				AddBattleEffectToQueueList(gBattlerAttacker, B_QUEUED_WISH);
				gBattlescriptCurrInstr += 6;
			}
			else
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 2);
			break;
		case 1: // heal effect
			PrepareMonNickWithPrefixBuffer(gBattleTextBuff1, gBattlerTarget, gWishFutureKnock.wishMonId[gBattlerTarget]);
			
			if (BATTLER_MAX_HP(gBattlerTarget))
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 2);
			else
			{
				gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 2;
				if (gBattleMoveDamage == 0)
					gBattleMoveDamage = 1;
				gBattleMoveDamage *= -1;
				
				gBattlescriptCurrInstr += 6;
			}
			break;
    }
}

static void atkD5_trysetroots(void) // ingrain
{
    if (gStatuses3[gBattlerAttacker] & STATUS3_ROOTED)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gStatuses3[gBattlerAttacker] |= STATUS3_ROOTED;
        gBattlescriptCurrInstr += 5;
    }
}

static void atkD6_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkD7_setyawn(void)
{
    if (gStatuses3[gBattlerTarget] & STATUS3_YAWN)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gStatuses3[gBattlerTarget] |= STATUS3_YAWN_TURN(2);
        gBattlescriptCurrInstr += 5;
    }
}

static void atkD8_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkD9_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkDA_swapabilities(void) // skill swap
{
    gBattleStruct->abilityOverride[gBattlerAttacker] = SetBattlerAbility(gBattlerAttacker, gBattleMons[gBattlerTarget].ability);
	gBattleStruct->abilityOverride[gBattlerTarget] = SetBattlerAbility(gBattlerTarget, gBattleStruct->abilityOverride[gBattlerAttacker]);
    ++gBattlescriptCurrInstr;
}

static void atkDB_tryimprison(void)
{
    if ((gStatuses3[gBattlerAttacker] & STATUS3_IMPRISONED_OTHERS))
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
		gStatuses3[gBattlerAttacker] |= STATUS3_IMPRISONED_OTHERS;
        PressurePPLoseOnUsingImprison(gBattlerAttacker);
		gBattlescriptCurrInstr += 5;
    }
}

static void atkDC_trysetgrudge(void)
{
    if (gStatuses3[gBattlerAttacker] & STATUS3_GRUDGE)
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gStatuses3[gBattlerAttacker] |= STATUS3_GRUDGE;
        gBattlescriptCurrInstr += 5;
    }
}

static void atkDD_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkDE_assistattackselect(void)
{
	u8 monId, moveId, chooseableMovesNo = 0;
    u16 move, *movesArray = Alloc(sizeof(u16) * PARTY_SIZE * MAX_MON_MOVES); // 6 mons, each of them knowing 4 moves
    struct Pokemon *party;

    if (movesArray != NULL)
	{
		party = GetBattlerParty(gBattlerAttacker);
		
		for (monId = 0; monId < PARTY_SIZE; ++monId)
		{
			if (monId != gBattlerPartyIndexes[gBattlerAttacker] && IsMonValidSpecies(&party[monId]))
			{
				for (moveId = 0; moveId < MAX_MON_MOVES; ++moveId)
				{
					move = GetMonData(&party[monId], MON_DATA_MOVE1 + moveId);
					
					if (move && move != MOVE_STRUGGLE && !gBattleMoves[move].flags.forbiddenAssist)
						movesArray[chooseableMovesNo++] = move;
				}
			}
		}
	}
    if (chooseableMovesNo)
    {
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
        gCalledMove = movesArray[Random() % chooseableMovesNo];
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
	
	FREE_IF_NOT_NULL(movesArray);
}

static void atkDF_trysetmagiccoat(void)
{
    gBattlerTarget = gBattlerAttacker;
    gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
	
    if (gCurrentTurnActionNumber == gBattlersCount - 1) // moves last turn
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gProtectStructs[gBattlerAttacker].bounceMove = TRUE;
        gBattlescriptCurrInstr += 5;
    }
}

static void atkE0_trysetsnatch(void) // snatch
{
    gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
	
    if (gCurrentTurnActionNumber == gBattlersCount - 1) // moves last turn
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gProtectStructs[gBattlerAttacker].stealMove = 1;
        gBattlescriptCurrInstr += 5;
    }
}

static void atkE1_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkE2_switchoutabilities(void)
{
	u16 temp;
	u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
	
	if (GetBattlerAbility(battlerId) == ABILITY_NEUTRALIZING_GAS)
	{
		PrepareAbilityBuffer(gBattleTextBuff1, GetBattlerAbility(battlerId));
		gBattleMons[battlerId].ability = ABILITY_NONE; // for don't reactivate and stay in a infinite loop
		BattleScriptPush(gBattlescriptCurrInstr);
		gBattlescriptCurrInstr = BattleScript_NeutralizingGasExits;
	}
	else
	{
		SWAP(gBattlerPartyIndexes[battlerId], *(gBattleStruct->battlerPartyIndexes + battlerId), temp);
		
		switch (GetBattlerAbility(battlerId))
		{
			case ABILITY_NATURAL_CURE:
			    gBattleMons[battlerId].status1 = 0;
				BtlController_EmitSetMonData(battlerId, BUFFER_A, REQUEST_STATUS_BATTLE, gBitTable[gBattlerPartyIndexes[battlerId]], 4, &gBattleMons[battlerId].status1);
				MarkBattlerForControllerExec(battlerId);
				break;
			case ABILITY_REGENERATOR:
			    gBattleMoveDamage = gBattleMons[battlerId].maxHP / 3;
				gBattleMoveDamage += gBattleMons[battlerId].hp;
				if (gBattleMoveDamage > gBattleMons[battlerId].maxHP)
					gBattleMoveDamage = gBattleMons[battlerId].maxHP;
				BtlController_EmitSetMonData(battlerId, BUFFER_A, REQUEST_HP_BATTLE, gBitTable[gBattlerPartyIndexes[battlerId]], 2, &gBattleMoveDamage);
				MarkBattlerForControllerExec(battlerId);
				break;
			case ABILITY_ZERO_TO_HERO:
			    if (gBattleMons[battlerId].species == SPECIES_PALAFIN)
				{
					u16 newSpecies = TryDoBattleFormChange(battlerId, FORM_CHANGE_SWITCH_OUT);
					
					if (newSpecies)
					{
						DoBattleFormChange(battlerId, newSpecies, FALSE, TRUE, FALSE);
						gBattleStruct->zeroToHeroActivated[GetBattlerSide(battlerId)] |= gBitTable[gBattlerPartyIndexes[battlerId]]; // for switch in message activate
					}
				}
				break;
		}
		DoSpecialFormChange(battlerId, gBattlerPartyIndexes[battlerId], FORM_CHANGE_SWITCH_OUT);
		
		SWAP(gBattlerPartyIndexes[battlerId], *(gBattleStruct->battlerPartyIndexes + battlerId), temp);
		
		gBattlescriptCurrInstr += 2;
	}
}

static void atkE3_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkE4_getsecretpowereffect(void)
{
	u8 effect;
	
    switch (gBattleTerrain)
    {
		case BATTLE_TERRAIN_GRASS:
			effect = MOVE_EFFECT_POISON;
			break;
		case BATTLE_TERRAIN_LONG_GRASS:
			effect = MOVE_EFFECT_SLEEP;
			break;
		case BATTLE_TERRAIN_SAND:
			effect = MOVE_EFFECT_ACC_MINUS_1;
			break;
		case BATTLE_TERRAIN_UNDERWATER:
			effect = MOVE_EFFECT_DEF_MINUS_1;
			break;
		case BATTLE_TERRAIN_WATER:
			effect = MOVE_EFFECT_ATK_MINUS_1;
			break;
		case BATTLE_TERRAIN_POND:
			effect = MOVE_EFFECT_SPD_MINUS_1;
			break;
		case BATTLE_TERRAIN_MOUNTAIN:
			effect = MOVE_EFFECT_CONFUSION;
			break;
		case BATTLE_TERRAIN_CAVE:
			effect = MOVE_EFFECT_FLINCH;
			break;
		default:
			effect = MOVE_EFFECT_PARALYSIS;
			break;
    }
	SetMoveEffect(effect, FALSE, FALSE);
    ++gBattlescriptCurrInstr;
}

static void atkE5_pickup(void)
{
    u8 i, level, chance;
    u16 heldItem, ability;
    u32 j;
    s32 random;

    for (i = 0; i < PARTY_SIZE; ++i)
    {
		random = Random() % 100;
        heldItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
	    ability = GetMonAbility(&gPlayerParty[i]);
		level = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
		chance = (level - 1) / 10;
		
		if (chance > 9)
			chance = 9;
        
		if (IsMonValidSpecies(&gPlayerParty[i]) && !heldItem)
		{
			if (ability == ABILITY_PICKUP && !(Random() % 10))
			{
				for (j = 0; j < 15; ++j)
				{
					if (sPickupItems[j].chance > random)
						break;
				}
				SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &sPickupItems[j]);
			}
			else if (ability == ABILITY_HONEY_GATHER)
			{
				if ((chance + 1) * 5 > random)
				{
					heldItem = ITEM_HONEY;
					SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &heldItem);
				}
			}
#if SHUCKLE_MAKES_BERRY_JUICE_CHANCE != 0
			else if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2) == SPECIES_SHUCKLE && ItemId_GetPocket(heldItem) == POCKET_BERRY_POUCH && Random() % 100 < SHUCKLE_MAKES_BERRY_JUICE_CHANCE)
			{
				heldItem = ITEM_BERRY_JUICE;
				SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &heldItem);
			}
#endif
		}
	}
    ++gBattlescriptCurrInstr;
}

static void atkE6_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkE7_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkE8_settypebasedhalvers(void) // water and mud sport
{
    bool8 worked = FALSE;

    if (gBattleMoves[gCurrentMove].effect == EFFECT_MUD_SPORT)
    {
		if (!(gFieldStatus & STATUS_FIELD_MUDSPORT))
		{
			gFieldStatus |= STATUS_FIELD_MUDSPORT;
			gFieldTimers.mudSportTimer = 5;
			gBattleCommunication[MULTISTRING_CHOOSER] = 0;
			worked = TRUE;
		}
    }
    else // water sport
    {
		if (!(gFieldStatus & STATUS_FIELD_WATERSPORT))
		{
			gFieldStatus |= STATUS_FIELD_WATERSPORT;
			gFieldTimers.waterSportTimer = 5;
			gBattleCommunication[MULTISTRING_CHOOSER] = 1;
			worked = TRUE;
		}
	}
    if (worked)
        gBattlescriptCurrInstr += 5;
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkE9_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkEA_tryrecycleitem(void)
{
	if (TryRecycleBattlerItem(gBattlerAttacker, gBattlerAttacker))
		gBattlescriptCurrInstr += 5;
	else
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkEB_settypetoterrain(void)
{
    if (!IS_BATTLER_OF_TYPE(gBattlerAttacker, sTerrainToType[gBattleTerrain]))
    {
		SetBattlerType(gBattlerAttacker, sTerrainToType[gBattleTerrain]);
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkEC_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkED_snatchsetbattlers(void)
{
    gEffectBattler = gBattlerAttacker;
	
    if (gBattlerAttacker == gBattlerTarget)
        gBattlerAttacker = gBattlerTarget = gBattleScripting.battler;
    else
        gBattlerTarget = gBattleScripting.battler;
	
    gBattleScripting.battler = gEffectBattler;
	
    ++gBattlescriptCurrInstr;
}

static void atkEE_nop(void)
{
    ++gBattlescriptCurrInstr;
}

u8 GetCatchingBattler(void)
{
	if (IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)))
		return GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
	else
		return GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
}

static void DoMonCaughtEffects(void)
{
	u8 friendBallFriendship = 150;
	struct Pokemon *mon = GetBattlerPartyIndexPtr(gBattlerTarget);
	
	if (CalculatePlayerPartyCount() == PARTY_SIZE)
		gBattleCommunication[MULTISTRING_CHOOSER] = 0;
	else
		gBattleCommunication[MULTISTRING_CHOOSER] = 1;
	
	SetMonData(mon, MON_DATA_POKEBALL, &gLastUsedItem);
	
	switch (gLastUsedItem)
	{
		case ITEM_HEAL_BALL:
		    MonRestorePP(mon);
			HealStatusConditions(mon, STATUS1_ANY, gBattlerTarget);
			gBattleMons[gBattlerTarget].hp = gBattleMons[gBattlerTarget].maxHP;
			SetMonData(mon, MON_DATA_HP, &gBattleMons[gBattlerTarget].hp);
			break;
		case ITEM_FRIEND_BALL:
		    SetMonData(mon, MON_DATA_FRIENDSHIP, &friendBallFriendship);
			break;
	}
	
	if (gLastUsedItem != ITEM_SAFARI_BALL && GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
		IncrementGameStat(GAME_STAT_POKEMON_CAPTURES);
	
	gBattlescriptCurrInstr = BattleScript_SuccessBallThrow;
}

static void atkEF_handleballthrow(void)
{
    u8 shakes, catchRate;
	s8 ballAdition;
	s32 ballMultiplier;
    u32 i, odds, weight;

    if (!gBattleControllerExecFlags)
    {
        gBattlerTarget = GetCatchingBattler();
	    
        if (gBattleTypeFlags & BATTLE_TYPE_GHOST) // Can't throw ball in ghost battle
        {
            BtlController_EmitBallThrowAnim(gBattlerAttacker, BUFFER_A, BALL_GHOST_DODGE);
            MarkBattlerForControllerExec(gBattlerAttacker);
            gBattlescriptCurrInstr = BattleScript_GhostBallDodge;
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER) // Can't throw ball in trainer battles
        {
#if RESTORE_BALL_TRAINER_BLOCK
			AddBagItem(gLastUsedItem, 1);
#endif
            BtlController_EmitBallThrowAnim(gBattlerAttacker, BUFFER_A, BALL_TRAINER_BLOCK);
            MarkBattlerForControllerExec(gBattlerAttacker);
            gBattlescriptCurrInstr = BattleScript_TrainerBallBlock;
        }
        else if (gBattleTypeFlags & (BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_OLD_MAN_TUTORIAL)) // Poke Dude and Old Man battles aways success the ball throw
        {
            BtlController_EmitBallThrowAnim(gBattlerAttacker, BUFFER_A, BALL_3_SHAKES_SUCCESS);
            MarkBattlerForControllerExec(gBattlerAttacker);
            gBattlescriptCurrInstr = BattleScript_OldMan_Pokedude_CaughtMessage;
        }
        else
        {
			ballAdition = 0;
			
			// Get ball catch Rate
			if (gLastUsedItem == ITEM_SAFARI_BALL)
				catchRate = gBattleStruct->safariCatchFactor * 1275 / 100;
			else
				catchRate = gBaseStats[GetMonData(GetBattlerPartyIndexPtr(gBattlerTarget), MON_DATA_SPECIES)].catchRate; // Don't copy catchRate if transformed
			
			// Get ball catch multiplier
			ballMultiplier = 100; // default multiplier x1.0
			
			switch (gLastUsedItem)
			{
				case ITEM_ULTRA_BALL:
				    ballMultiplier = 200;
					break;
				case ITEM_GREAT_BALL:
				case ITEM_SAFARI_BALL:
				    ballMultiplier = 150;
					break;
				case ITEM_NET_BALL:
				    if (IS_BATTLER_OF_TYPE(gBattlerTarget, TYPE_WATER) || IS_BATTLER_OF_TYPE(gBattlerTarget, TYPE_BUG))
						ballMultiplier = 350;
					break;
				case ITEM_DIVE_BALL:
				    if (GetCurrentMapType() == MAP_TYPE_UNDERWATER || gIsFishingEncounter || gIsSurfingEncounter)
						ballMultiplier = 350;
					break;
				case ITEM_NEST_BALL:
				    if (gBattleMons[gBattlerTarget].level < 30)
						ballMultiplier = 410 - (gBattleMons[gBattlerTarget].level * 10);
					break;
				case ITEM_REPEAT_BALL:
				    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gBattlerTarget].species), FLAG_GET_CAUGHT))
						ballMultiplier = 350;
					break;
				case ITEM_TIMER_BALL:
				    ballMultiplier = (gBattleStruct->battleTurnCounter * 30) + 100;
                    if (ballMultiplier > 400)
                        ballMultiplier = 400;
                    break;
				case ITEM_DUSK_BALL:
				    if (gMapHeader.cave || GetCurrentMapType() == MAP_TYPE_UNDERGROUND || GetDNSTimeLapseIsNight())
						ballMultiplier = 300;
					break;
				case ITEM_QUICK_BALL:
				    if (gBattleStruct->battleTurnCounter == 0)
						ballMultiplier = 500;
					break;
				case ITEM_FAST_BALL:
				    if (gBaseStats[gBattleMons[gBattlerTarget].species].baseSpeed >= 100)
						ballMultiplier = 400;
					break;
				case ITEM_LEVEL_BALL:
				    if (gBattleMons[gBattlerAttacker].level >= 4 * gBattleMons[gBattlerTarget].level)
                        ballMultiplier = 800;
                    else if (gBattleMons[gBattlerAttacker].level > 2 * gBattleMons[gBattlerTarget].level)
                        ballMultiplier = 400;
                    else if (gBattleMons[gBattlerAttacker].level > gBattleMons[gBattlerTarget].level)
                        ballMultiplier = 200;
                    break;
				case ITEM_LURE_BALL:
				    if (gIsFishingEncounter)
						ballMultiplier = 500;
					break;
				case ITEM_HEAVY_BALL:
				    weight = GetPokedexHeightWeight(SpeciesToNationalPokedexNum(gBattleMons[gBattlerTarget].species), 1);
				
				    if (weight < 1000)
						ballAdition = -20;
					else if (weight < 2000)
						ballAdition = 0;
					else if (weight < 3000)
						ballAdition = 20;
					else
						ballAdition = 30;
					break;
				case ITEM_LOVE_BALL:
				    if (gBattleMons[gBattlerAttacker].species == gBattleMons[gBattlerTarget].species)
					{
						if (GetBattlerGender(gBattlerAttacker) != MON_GENDERLESS && GetBattlerGender(gBattlerTarget) != MON_GENDERLESS
						&& !ARE_BATTLERS_OF_SAME_GENDER(gBattlerAttacker, gBattlerTarget))
						    ballMultiplier = 800;
					}
					break;
				case ITEM_MOON_BALL:
				    for (i = 0; i < EVOS_PER_MON; i++)
					{
						if (gEvolutionTable[gBattleMons[gBattlerTarget].species][i].method == EVO_ITEM
						&& gEvolutionTable[gBattleMons[gBattlerTarget].species][i].param == ITEM_MOON_STONE)
						    ballMultiplier = 400;
					}
					break;
				case ITEM_BEAST_BALL:
				    ballMultiplier = 10;
					break;
				case ITEM_DREAM_BALL:
				    if (gBattleMons[gBattlerTarget].status1 & STATUS1_SLEEP || GetBattlerAbility(gBattlerTarget) == ABILITY_COMATOSE)
						ballMultiplier = 40;
					break;
			}
			// Check Ultra Beast
			if (IS_SPECIES_ULTRA_BEAST(gBattleMons[gBattlerTarget].species))
			{
				if (gLastUsedItem == ITEM_BEAST_BALL)
					ballMultiplier = 500;
				else
					ballMultiplier = 10;
			}
			// catchRate is unsigned, which means that it may potentially overflow if sum is applied directly.
			if (catchRate < 21 && ballAdition == -20)
				catchRate = 1;
			else
				catchRate += ballAdition;
			
			odds = (catchRate * ballMultiplier / 100) * (gBattleMons[gBattlerTarget].maxHP * 3 - gBattleMons[gBattlerTarget].hp * 2)
			/ (3 * gBattleMons[gBattlerTarget].maxHP);
			
            if (gBattleMons[gBattlerTarget].status1 & (STATUS1_SLEEP | STATUS1_FREEZE))
                odds *= 2;
            if (gBattleMons[gBattlerTarget].status1 & (STATUS1_PSN_ANY | STATUS1_BURN | STATUS1_PARALYSIS))
                odds = (odds * 15) / 10;
			
            if (odds > 254) // mon caught
			{
				BtlController_EmitBallThrowAnim(gBattlerAttacker, BUFFER_A, BALL_3_SHAKES_SUCCESS);
                MarkBattlerForControllerExec(gBattlerAttacker);
			    DoMonCaughtEffects();
            }
            else // mon may be caught, calculate shakes
            {
                odds = Sqrt(Sqrt(16711680 / odds));
                odds = 1048560 / odds;
				
				if (gLastUsedItem == ITEM_MASTER_BALL)
					shakes = BALL_3_SHAKES_SUCCESS;
				else
					for (shakes = 0; shakes < 4 && Random() < odds; ++shakes);
				
                BtlController_EmitBallThrowAnim(gBattlerAttacker, BUFFER_A, shakes);
                MarkBattlerForControllerExec(gBattlerAttacker);
				
				if (shakes == BALL_3_SHAKES_SUCCESS) // mon caught
					DoMonCaughtEffects();
                else // not caught
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = shakes;
                    gBattlescriptCurrInstr = BattleScript_ShakeBallThrow;
                }
            }
        }
    }
}

static void atkF0_givecaughtmon(void)
{
	bool8 isBillsPc;
	u8 battler = GetCatchingBattler();
	
	DoSpecialFormChange(battler, gBattlerPartyIndexes[battler], FORM_CHANGE_END_BATTLE);
	
    if (GiveMonToPlayer(&gEnemyParty[gBattlerPartyIndexes[battler]]) != MON_GIVEN_TO_PARTY)
    {
		isBillsPc = FlagGet(FLAG_SYS_NOT_SOMEONES_PC);
		
        if (!ShouldShowBoxWasFullMessage())
            gBattleCommunication[MULTISTRING_CHOOSER] = isBillsPc ? B_MSG_TRANSFERRED_TO_BILL_PC : B_MSG_TRANSFERRED_TO_SOMEONE_PC;
        else
        {
			gBattleCommunication[MULTISTRING_CHOOSER] = isBillsPc ? B_MSG_BILL_PC_BOX_FULL : B_MSG_SOMEONE_PC_BOX_FULL;
            StringCopy(gStringVar3, GetBoxNamePtr(GetPCBoxToSendMon())); //box the mon was going to be sent to
        }
		StringCopy(gStringVar1, GetBoxNamePtr(VarGet(VAR_PC_BOX_TO_SEND_MON))); // box the mon was sent to
		GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_NICKNAME, gStringVar2);
    }
    ++gBattlescriptCurrInstr;
}

static void atkF1_trysetcaughtmondexflags(void)
{
	u8 battler = GetCatchingBattler();
    u16 natDexNum = SpeciesToNationalPokedexNum(GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES));

    if (GetSetPokedexFlag(natDexNum, FLAG_GET_CAUGHT))
        gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        HandleSetPokedexFlag(natDexNum, FLAG_SET_CAUGHT, GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_PERSONALITY));
	    
		if (!FlagGet(FLAG_SYS_POKEDEX_GET)) // don't show dex entry if don't have the pokedex
			gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
		else
			gBattlescriptCurrInstr += 5;
    }
}

static void atkF2_displaydexinfo(void)
{
	u8 battler = GetCatchingBattler();
    u16 species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES, NULL);

    switch (gBattleCommunication[MULTIUSE_STATE])
    {
		case 0:
			BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_WHITE);
			++gBattleCommunication[MULTIUSE_STATE];
			break;
		case 1:
			if (!gPaletteFade.active)
			{
				FreeAllWindowBuffers();
				gBattleCommunication[TASK_ID] = DexScreen_RegisterMonToPokedex(species);
				++gBattleCommunication[MULTIUSE_STATE];
			}
			break;
		case 2:
			if (!gPaletteFade.active && gMain.callback2 == BattleMainCB2 && !gTasks[gBattleCommunication[TASK_ID]].isActive)
			{
				CpuFill32(0, (void *)VRAM, VRAM_SIZE);
				SetVBlankCallback(VBlankCB_Battle);
				++gBattleCommunication[MULTIUSE_STATE];
			}
			break;
		case 3:
			InitBattleBgsVideo();
			LoadBattleTextboxAndBackground();
			gBattle_BG3_X = 0x100;
			++gBattleCommunication[MULTIUSE_STATE];
			break;
		case 4:
			if (!IsDma3ManagerBusyWithBgCopy())
			{
				CreateMonPicSprite(species, GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_IS_SHINY), gBattleMons[battler].personality, TRUE, 120, 64, 0, 0xFFFF);
				CpuFill32(0, gPlttBufferFaded, BG_PLTT_SIZE);
				BeginNormalPaletteFade(0x1FFFF, 0, 0x10, 0, RGB_BLACK);
				ShowBg(0);
				ShowBg(3);
				++gBattleCommunication[MULTIUSE_STATE];
			}
			break;
		case 5:
			if (!gPaletteFade.active)
				++gBattlescriptCurrInstr;
			break;
    }
}

void HandleBattleWindow(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u8 flags)
{
    u16 var = 0;
    s32 destY, destX;

    for (destY = yStart; destY <= yEnd; ++destY)
    {
        for (destX = xStart; destX <= xEnd; ++destX)
        {
            if (destY == yStart)
            {
                if (destX == xStart)
                    var = 0x1022;
                else if (destX == xEnd)
                    var = 0x1024;
                else
                    var = 0x1023;
            }
            else if (destY == yEnd)
            {
                if (destX == xStart)
                    var = 0x1028;
                else if (destX == xEnd)
                    var = 0x102A;
                else
                    var = 0x1029;
            }
            else
            {
                if (destX == xStart)
                    var = 0x1025;
                else if (destX == xEnd)
                    var = 0x1027;
                else
                    var = 0x1026;
            }
            if (flags & WINDOW_CLEAR)
                var = 0;
			
            if (flags & WINDOW_x80)
                CopyToBgTilemapBufferRect_ChangePalette(1, &var, destX, destY, 1, 1, 0x11);
            else
                CopyToBgTilemapBufferRect_ChangePalette(0, &var, destX, destY, 1, 1, 0x11);
        }
    }
    CopyBgTilemapBufferToVram(1);
}

void BattleCreateYesNoCursorAt(void)
{
    u16 src[2] = {1, 2};
    CopyToBgTilemapBufferRect_ChangePalette(0, src, 0x18, 9 + (2 * gBattleCommunication[CURSOR_POSITION]), 1, 2, 0x11);
    CopyBgTilemapBufferToVram(0);
}

void BattleDestroyYesNoCursorAt(void)
{
    u16 src[2] = {32, 32};
    CopyToBgTilemapBufferRect_ChangePalette(0, src, 0x18, 9 + (2 * gBattleCommunication[CURSOR_POSITION]), 1, 2, 0x11);
    CopyBgTilemapBufferToVram(0);
}

static void atkF3_trygivecaughtmonnick(void)
{
	u8 battler = GetCatchingBattler();
	
	if (gSaveBlock2Ptr->optionsSkipPkmnNickname)
		gBattleCommunication[MULTIUSE_STATE] = 4;
	
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
		case 0:
			HandleBattleWindow(0x17, 8, 0x1D, 0xD, 0);
			BattlePutTextOnWindow(gText_BattleYesNoChoice, B_WIN_YESNO);
			gBattleCommunication[CURSOR_POSITION] = 0;
			BattleCreateYesNoCursorAt();
			++gBattleCommunication[MULTIUSE_STATE];
			break;
		case 1:
			if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION])
			{
				PlaySE(SE_SELECT);
				BattleDestroyYesNoCursorAt();
				gBattleCommunication[CURSOR_POSITION] = 0;
				BattleCreateYesNoCursorAt();
			}
			if (JOY_NEW(DPAD_DOWN) && gBattleCommunication[CURSOR_POSITION] == 0)
			{
				PlaySE(SE_SELECT);
				BattleDestroyYesNoCursorAt();
				gBattleCommunication[CURSOR_POSITION] = 1;
				BattleCreateYesNoCursorAt();
			}
			if (JOY_NEW(A_BUTTON))
			{
				PlaySE(SE_SELECT);
				
				if (gBattleCommunication[CURSOR_POSITION] == 0)
				{
					++gBattleCommunication[MULTIUSE_STATE];
					BeginFastPaletteFade(FAST_FADE_OUT_TO_BLACK);
				}
				else
					gBattleCommunication[MULTIUSE_STATE] = 4;
			}
			else if (JOY_NEW(B_BUTTON))
			{
				PlaySE(SE_SELECT);
				gBattleCommunication[MULTIUSE_STATE] = 4;
			}
			break;
		case 2:
			if (!gPaletteFade.active)
			{
				GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_NICKNAME, gBattleStruct->caughtMonNick);
				FreeAllWindowBuffers();
				DoNamingScreen(NAMING_SCREEN_CAUGHT_MON, gBattleStruct->caughtMonNick, GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES),
                           GetMonGender(&gEnemyParty[gBattlerPartyIndexes[battler]]), BattleMainCB2);
				++gBattleCommunication[MULTIUSE_STATE];
			}
			break;
		case 3:
			if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
			{
				SetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_NICKNAME, gBattleStruct->caughtMonNick);
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
			}
			break;
		case 4:
			if (CalculatePlayerPartyCount() == PARTY_SIZE)
			{
				BattleScriptPush(gBattlescriptCurrInstr + 5);
				gBattlescriptCurrInstr = BattleScript_Pausex20;
			}
			else
				gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
			break;
    }
}

static void atkF4_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkF5_nop(void)
{
    ++gBattlescriptCurrInstr;
}

static void atkF6_finishaction(void)
{
    gCurrentActionFuncId = B_ACTION_FINISHED;
}

static void atkF7_finishturn(void)
{
    gCurrentActionFuncId = B_ACTION_FINISHED;
    gCurrentTurnActionNumber = gBattlersCount;
}

static void atkF8_callfunction(void)
{
	void (*func )(void) = ((void (*)(void))READ_PTR(gBattlescriptCurrInstr + 1));
	func();
}

static void atkF9_cureprimarystatus(void)
{
	if (!gBattleControllerExecFlags)
	{
		u8 battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
		
		if (gBattleMons[battlerId].status1 & STATUS1_ANY) 
		{
			PrepareMonNickBuffer(gBattleTextBuff2, battlerId, gBattlerPartyIndexes[battlerId]);
			ClearBattlerStatus(battlerId);
			gBattlescriptCurrInstr += 6;
		}
		else
			gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 2);
	}
}

static void atkFA_setword(void)
{
	u32 *dest = READ_PTR2(gBattlescriptCurrInstr + 1);
	u32 word = READ_32(gBattlescriptCurrInstr + 5);
	
	*dest = word;
	gBattlescriptCurrInstr += 9;
}

static void atkFB_jumpifsubstituteblocks(void)
{
	if (SubsBlockMove(gBattlerAttacker, gBattlerTarget, gCurrentMove)) 
		gBattlescriptCurrInstr = READ_PTR(gBattlescriptCurrInstr + 1);
	else
		gBattlescriptCurrInstr += 5;
}

static void atkFC_handleabilitypopup(void)
{
	u8 byte, battlerId;
	u16 animId;
	bool8 removePopUp;
	
	if (!gBattleControllerExecFlags)
	{
		if (!gBattleScripting.bypassAbilityPopUp)
		{
			const u16 *argumentPtr = NULL;
			byte = gBattlescriptCurrInstr[1];
			removePopUp = (byte & ATKFC_REMOVE_POP_UP);
			
			battlerId = GetBattlerForBattleScript(byte & ~(ATKFC_REMOVE_POP_UP));
			
			if (!removePopUp)
			{
				if (gBattleScripting.fixedAbilityPopUp && (gActiveAbilityPopUps & gBitTable[battlerId]))
				{
					gBattlescriptCurrInstr += 2;
					return;
				}
				animId = B_ANIM_LOAD_ABILITY_POP_UP;
			}
			else
				animId = B_ANIM_REMOVE_ABILITY_POP_UP;
			
			BtlController_EmitBattleAnimation(battlerId, BUFFER_A, animId, *argumentPtr);
			MarkBattlerForControllerExec(battlerId);
		}
		gBattlescriptCurrInstr += 2;
	}
}

static void atkFD_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkFE_nop(void)
{
	++gBattlescriptCurrInstr;
}

static void atkFF_nop(void)
{
	++gBattlescriptCurrInstr;
}

// Callfunction funcs
void BS_SetMinimize(void)
{
	if (gHitMarker & HITMARKER_OBEYS)
        gStatuses3[gBattlerAttacker] |= STATUS3_MINIMIZED;
	
	gBattlescriptCurrInstr += 5;
}

void BS_MoveValuesCleanUp(void)
{
	MoveValuesCleanUp();
	gBattlescriptCurrInstr += 5;
}

void BS_ItemIncreaseStat(void)
{
	u8 stages;
	
	switch (gLastUsedItem)
	{
		case ITEM_X_ATTACK:
		case ITEM_X_DEFEND:
		case ITEM_X_SPEED:
		case ITEM_X_ACCURACY:
		case ITEM_X_SPECIAL:
		case ITEM_X_SPECIAL_DEFENSE:
		    stages = 2;
			break;
		default:
			stages = 1;
			break;
	}
	SET_STATCHANGER(ItemId_GetHoldEffectParam(gLastUsedItem), stages, FALSE);
	gBattlescriptCurrInstr += 5;
}

void BS_IncrementGameStat(void)
{
	if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        IncrementGameStat(gBattlescriptCurrInstr[5]);
	gBattlescriptCurrInstr += 6;
}

void BS_GetMoveTarget(void)
{
	gBattlerTarget = GetMoveTarget(gCurrentMove, 0);
	gBattlescriptCurrInstr += 5;
}

void BS_SwitchSpriteIgnore0HpBit(void)
{
	gBattleStruct->spriteIgnore0Hp ^= TRUE;
	gBattlescriptCurrInstr += 5;
}

void BS_AtkNameInBuff1(void)
{
	PrepareMonNickBuffer(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker]);
	gBattlescriptCurrInstr += 5;
}

void BS_ResetSentMonsValue(void)
{
	ResetSentPokesToOpponentValue();
	gBattlescriptCurrInstr += 5;
}

void BS_SetAtkToPlayer0(void)
{
	gBattlerAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
	gBattlescriptCurrInstr += 5;
}

void BS_BufferMoveToLearn(void)
{
	BufferMoveToLearnIntoBattleTextBuff2();
	gBattlescriptCurrInstr += 5;
}

void BS_TrySetDestinyBondToHappen(void)
{
	TrySetDestinyBondToHappen();
	gBattlescriptCurrInstr += 5;
}

void BS_SetDefenseCurl(void)
{
	gBattleMons[gBattlerAttacker].status2 |= STATUS2_DEFENSE_CURL;
	gBattlescriptCurrInstr += 5;
}

void BS_SetNightmare(void)
{
	gBattleMons[gBattlerTarget].status2 |= STATUS2_NIGHTMARE;
	gBattlescriptCurrInstr += 5;
}

void BS_NormaliseAllBuffs(void)
{
	u8 i;

    for (i = 0; i < gBattlersCount; ++i)
		TryResetBattlerStatChanges(i);
	
	gBattlescriptCurrInstr += 5;
}

void BS_SetChargingTurn(void)
{
	gHitMarker |= HITMARKER_CHARGING;
	
	if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS))
	{
		gBattleMons[gBattlerAttacker].status2 |= STATUS2_MULTIPLETURNS;
		gLockedMoves[gBattlerAttacker] = gCurrentMove;
		gProtectStructs[gBattlerAttacker].chargingTurn = 1;
	}
	gBattlescriptCurrInstr += 5;
}

void BS_SetRage(void)
{
	gBattleMons[gBattlerAttacker].status2 |= STATUS2_RAGE;
	gBattlescriptCurrInstr += 5;
}

void BS_SetEscapePrevention(void)
{
	gBattleMons[gBattlerTarget].status2 |= STATUS2_ESCAPE_PREVENTION;
	gDisableStructs[gBattlerTarget].battlerPreventingEscape = gBattlerAttacker;
	gBattlescriptCurrInstr += 5;
}

void BS_SetForesight(void)
{
	gBattleMons[gBattlerTarget].status2 |= STATUS2_FORESIGHT;
	gBattlescriptCurrInstr += 5;
}

void BS_SetForcedTarget(void)
{
	gSideTimers[GetBattlerSide(gBattlerAttacker)].followmeSet = TRUE;
    gSideTimers[GetBattlerSide(gBattlerAttacker)].followmeTarget = gBattlerAttacker;
	gBattlescriptCurrInstr += 5;
}

void BS_CallTerrainAttack(void)
{
	PrepareMoveBuffer(gBattleTextBuff1, gCurrentMove);
	CallAnotherMove(sNaturePowerMoves[gBattleTerrain]);
	BattleScriptCall(BattleScript_NaturePowerString);
}

void BS_GetTwoTurnsMoveString(void)
{
	u8 stringId;
	
	switch (gCurrentMove)
	{
		case MOVE_SKY_ATTACK:
		    stringId = B_MSG_CLOAKED_IN_SUNLIGHT;
			break;
		case MOVE_RAZOR_WIND:
		    stringId = B_MSG_WHIPPED_WHIRLWIND;
			break;
		case MOVE_SOLAR_BEAM:
		    stringId = B_MSG_ABSORBED_LIGHT;
			break;
		case MOVE_FLY:
		    stringId = B_MSG_FLEW_UP_HIGH;
			break;
		case MOVE_DIVE:
		    stringId = B_MSG_HID_UNDERWATER;
			break;
		case MOVE_BOUNCE:
		    stringId = B_MSG_SPRANG_UP;
			break;
		case MOVE_DIG:
		    stringId = B_MSG_BURROWED_IN_GROUND;
			break;
		case MOVE_SKULL_BASH:
		    stringId = B_MSG_TUCKED_HEAD;
			break;
	}
	gBattleCommunication[MULTISTRING_CHOOSER] = stringId;
	gBattlescriptCurrInstr += 5;
}

void BS_SetMagicCoatTarget(void)
{
	u8 side, attacker = gBattlerAttacker;
	
	gBattlerAttacker = gBattlerTarget;
	
	side = GetBattlerSide(BATTLE_OPPOSITE(gBattlerAttacker));
	
	if (IsBattlerAffectedByFollowMe(gBattlerAttacker, side, gCurrentMove))
		gBattlerTarget = gSideTimers[side].followmeTarget;
	else
		gBattlerTarget = attacker;
	
	gBattlescriptCurrInstr += 5;
}

void BS_SetFocusEnergy(void)
{
	gBattleMons[gBattlerAttacker].status2 |= STATUS2_FOCUS_ENERGY;
	gBattlescriptCurrInstr += 5;
}

void BS_CancelAllActions(void)
{
	u8 i;

    for (i = 0; i < gBattlersCount; ++i)
        gActionsByTurnOrder[i] = B_ACTION_CANCEL_PARTNER;
	
	gBattlescriptCurrInstr += 5;
}

void BS_SetBide(void)
{
	gHitMarker |= HITMARKER_CHARGING;
    gBattleMons[gBattlerAttacker].status2 |= (STATUS2_MULTIPLETURNS | STATUS2_BIDE_TURN(2));
    gLockedMoves[gBattlerAttacker] = gCurrentMove;
    gTakenDmg[gBattlerAttacker] = 0;
	gBattlescriptCurrInstr += 5;
}

void BS_SetMist(void)
{
	gSideStatuses[GetBattlerSide(gBattlerAttacker)] |= SIDE_STATUS_MIST;
	gSideTimers[GetBattlerSide(gBattlerAttacker)].mistTimer = 5;
	gSideTimers[GetBattlerSide(gBattlerAttacker)].mistBattlerId = gBattlerAttacker;
	gBattlescriptCurrInstr += 5;
}

void BS_Metronome(void)
{
	u16 move;
	
    while (TRUE)
    {
        move = (Random() % (MOVES_COUNT - 1)) + 1;
	    
        if (!gBattleMoves[move].flags.forbiddenMetronome)
		{
			CallAnotherMove(move);
            return;
        }
    }
}

void BS_PsywaveDamageEffect(void)
{
	u32 rand = (Random() % 101) + 50;
	
	gBattleMoveDamage = (gBattleMons[gBattlerAttacker].level * rand) / 100;
	
    if (gBattleMoveDamage == 0)
		gBattleMoveDamage = 1;
	
	gBattlescriptCurrInstr += 5;
}

void BS_CopyFoeStats(void)
{
	CopyBattlerStatChanges(gBattlerAttacker, gBattlerTarget);
	CopyBattlerCritModifier(gBattlerAttacker, gBattlerTarget);
	gBattlescriptCurrInstr += 5;
}

void BS_SetCharge(void)
{
	gStatuses3[gBattlerTarget] |= STATUS3_CHARGED_UP;
    gDisableStructs[gBattlerTarget].chargeTimer = 2;
	gBattlescriptCurrInstr += 5;
}

void BS_RemoveScreens(void)
{
	if (TryRemoveScreens(gBattlerTarget, FALSE))
    {
        gBattleScripting.animTurn = 1;
        gBattleScripting.animTargetsHit = 1;
    }
    else
    {
        gBattleScripting.animTurn = 0;
        gBattleScripting.animTargetsHit = 0;
    }
	gBattlescriptCurrInstr += 5;
}
