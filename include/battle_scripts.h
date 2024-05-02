#ifndef GUARD_BATTLE_SCRIPTS_H
#define GUARD_BATTLE_SCRIPTS_H

#include "global.h"

extern const u8 BattleScript_HitFromCritCalc[];
extern const u8 BattleScript_MoveEnd[];
extern const u8 BattleScript_MoveMissedPause[];
extern const u8 BattleScript_ButItFailed[];
extern const u8 BattleScript_StatChange[];
extern const u8 BattleScript_StatChangeString[];
extern const u8 BattleScript_StatCantChangeString[];
extern const u8 BattleScript_AlreadyAtFullHp[];
extern const u8 BattleScript_PresentHealTarget[];
extern const u8 BattleScript_MoveUsedMustRecharge[];
extern const u8 BattleScript_FaintAttacker[];
extern const u8 BattleScript_FaintTarget[];
extern const u8 BattleScript_GiveExp[];
extern const u8 BattleScript_HandleFaintedMon[];
extern const u8 BattleScript_LocalTrainerBattleWon[];
extern const u8 BattleScript_LocalTwoTrainersDefeated[];
extern const u8 BattleScript_LocalBattleWonLoseTexts[];
extern const u8 BattleScript_LocalBattleWonReward[];
extern const u8 BattleScript_PayDayMoneyAndPickUpItems[];
extern const u8 BattleScript_LocalBattleLost[];
extern const u8 BattleScript_LocalBattleLostPrintWhiteOut[];
extern const u8 BattleScript_LocalBattleLostEnd[];
extern const u8 BattleScript_LinkBattleWonOrLost[];
extern const u8 BattleScript_SmokeBallEscape[];
extern const u8 BattleScript_RanAwayUsingMonAbility[];
extern const u8 BattleScript_GotAwaySafely[];
extern const u8 BattleScript_WildMonFled[];
extern const u8 BattleScript_PrintCantRunFromTrainer[];
extern const u8 BattleScript_PrintFailedToRunString[];
extern const u8 BattleScript_PrintCantEscapeFromBattle[];
extern const u8 BattleScript_ActionSwitch[];
extern const u8 BattleScript_Pausex20[];
extern const u8 BattleScript_LevelUp[];
extern const u8 BattleScript_WeatherContinuesOrEnds[];
extern const u8 BattleScript_WeatherDamage[];
extern const u8 BattleScript_OverworldWeatherStarts[];
extern const u8 BattleScript_SideStatusWoreOff[];
extern const u8 BattleScript_FieldStatusWoreOff[];
extern const u8 BattleScript_SafeguardProtected[];
extern const u8 BattleScript_LeechSeedTurnDrain[];
extern const u8 BattleScript_BideStoringEnergy[];
extern const u8 BattleScript_BideAttack[];
extern const u8 BattleScript_BideNoEnergyToAttack[];
extern const u8 BattleScript_SuccessForceOut[];
extern const u8 BattleScript_MistProtected[];
extern const u8 BattleScript_RageIsBuilding[];
extern const u8 BattleScript_MoveUsedIsDisabled[];
extern const u8 BattleScript_SelectingNotAllowedMove[];
extern const u8 BattleScript_DisabledNoMore[];
extern const u8 BattleScript_EncoredNoMore[];
extern const u8 BattleScript_DestinyBondTakesLife[];
extern const u8 BattleScript_DmgHazardsOnTarget[];
extern const u8 BattleScript_DmgHazardsOnAttacker[];
extern const u8 BattleScript_DmgHazardsOnFaintedBattler[];
extern const u8 BattleScript_PerishSongTakesLife[];
extern const u8 BattleScript_PerishSongCountGoesDown[];
extern const u8 BattleScript_AllStatsUp[];
extern const u8 BattleScript_WrapFree[];
extern const u8 BattleScript_LeechSeedFree[];
extern const u8 BattleScript_SpikesFree[];
extern const u8 BattleScript_MonTookFutureAttack[];
extern const u8 BattleScript_NoPPForMove[];
extern const u8 BattleScript_MoveUsedIsTormented[];
extern const u8 BattleScript_MoveUsedIsTaunted[];
extern const u8 BattleScript_WishComesTrue[];
extern const u8 BattleScript_IngrainTurnHeal[];
extern const u8 BattleScript_MoveEffectAtkDefDown[];
extern const u8 BattleScript_KnockedOff[];
extern const u8 BattleScript_MoveUsedIsImprisoned[];
extern const u8 BattleScript_GrudgeTakesPp[];
extern const u8 BattleScript_MagicCoatBounce[];
extern const u8 BattleScript_SnatchedMove[];
extern const u8 BattleScript_EnduredMsg[];
extern const u8 BattleScript_OneHitKOMsg[];
extern const u8 BattleScript_FocusPunchSetUp[];
extern const u8 BattleScript_MoveUsedIsAsleep[];
extern const u8 BattleScript_MoveUsedWokeUp[];
extern const u8 BattleScript_MonWokeUpInUproar[];
extern const u8 BattleScript_PoisonTurnDmg[];
extern const u8 BattleScript_BurnTurnDmg[];
extern const u8 BattleScript_MoveUsedIsFrozen[];
extern const u8 BattleScript_MoveUsedUnfroze[];
extern const u8 BattleScript_DefrostedViaFireMove[];
extern const u8 BattleScript_MoveUsedIsParalyzed[];
extern const u8 BattleScript_MoveUsedFlinched[];
extern const u8 BattleScript_PrintUproarOverTurns[];
extern const u8 BattleScript_ThrashConfuses[];
extern const u8 BattleScript_MoveUsedIsConfused[];
extern const u8 BattleScript_MoveUsedIsConfusedSefHit[];
extern const u8 BattleScript_MoveUsedIsConfusedNoMore[];
extern const u8 BattleScript_PrintPayDayMoneyString[];
extern const u8 BattleScript_WrapTurnDmg[];
extern const u8 BattleScript_WrapEnds[];
extern const u8 BattleScript_MoveUsedIsInLove[];
extern const u8 BattleScript_MoveUsedIsInLoveCantAttack[];
extern const u8 BattleScript_MoveUsedIsParalyzedCantAttack[];
extern const u8 BattleScript_NightmareTurnDmg[];
extern const u8 BattleScript_CurseTurnDmg[];
extern const u8 BattleScript_TargetStatusHeal[];
extern const u8 BattleScript_MoveEffectSleep[];
extern const u8 BattleScript_YawnMakesAsleep[];
extern const u8 BattleScript_MoveEffectPoison[];
extern const u8 BattleScript_MoveEffectBurn[];
extern const u8 BattleScript_MoveEffectFreeze[];
extern const u8 BattleScript_MoveEffectParalysis[];
extern const u8 BattleScript_MoveEffectUproar[];
extern const u8 BattleScript_MoveEffectToxic[];
extern const u8 BattleScript_MoveEffectPayDay[];
extern const u8 BattleScript_MoveEffectWrap[];
extern const u8 BattleScript_MoveEffectConfusion[];
extern const u8 BattleScript_ItemSteal[];
extern const u8 BattleScript_SwitchInWeatherAbilityActivates[];
extern const u8 BattleScript_SpeedBoostActivates[];
extern const u8 BattleScript_TraceActivates[];
extern const u8 BattleScript_RainDishActivates[];
extern const u8 BattleScript_ShedSkinHealerActivates[];
extern const u8 BattleScript_CastformChange[];
extern const u8 BattleScript_TookAttack[];
extern const u8 BattleScript_SturdyPreventsOHKO[];
extern const u8 BattleScript_DampStopsExplosion[];
extern const u8 BattleScript_MoveHPDrain_PPLoss[];
extern const u8 BattleScript_MoveHPDrain[];
extern const u8 BattleScript_MonMadeMoveUseless_PPLoss[];
extern const u8 BattleScript_MonMadeMoveUseless[];
extern const u8 BattleScript_FlashFireBoost_PPLoss[];
extern const u8 BattleScript_FlashFireBoost[];
extern const u8 BattleScript_AbilityNoStatLoss[];
extern const u8 BattleScript_FlinchPrevention[];
extern const u8 BattleScript_OwnTempoPrevents[];
extern const u8 BattleScript_SoundproofProtected[];
extern const u8 BattleScript_AbilityNoSpecificStatLoss[];
extern const u8 BattleScript_StickyHoldActivates[];
extern const u8 BattleScript_ColorChangeActivates[];
extern const u8 BattleScript_RoughSkinActivates[];
extern const u8 BattleScript_CuteCharmActivates[];
extern const u8 BattleScript_ApplySecondaryEffect[];
extern const u8 BattleScript_SynchronizeActivates[];
extern const u8 BattleScript_NoItemSteal[];
extern const u8 BattleScript_AbilityCuredStatusEnd3[];
extern const u8 BattleScript_AbilityCuredStatus[];
extern const u8 BattleScript_IgnoresAndUsesRandomMove[];
extern const u8 BattleScript_MoveUsedLoafingAround[];
extern const u8 BattleScript_IgnoresAndFallsAsleep[];
extern const u8 BattleScript_SubstituteFade[];
extern const u8 BattleScript_BerryCurePrlzEnd2[];
extern const u8 BattleScript_BerryCureParRet[];
extern const u8 BattleScript_BerryCurePsnEnd2[];
extern const u8 BattleScript_BerryCurePsnRet[];
extern const u8 BattleScript_BerryCureBrnEnd2[];
extern const u8 BattleScript_BerryCureBrnRet[];
extern const u8 BattleScript_BerryCureFrzEnd2[];
extern const u8 BattleScript_BerryCureFrzRet[];
extern const u8 BattleScript_BerryCureSlpEnd2[];
extern const u8 BattleScript_BerryCureSlpRet[];
extern const u8 BattleScript_BerryCureConfusionEnd2[];
extern const u8 BattleScript_BerryCureConfusionRet[];
extern const u8 BattleScript_BerryCureChosenStatusEnd2[];
extern const u8 BattleScript_BerryCureChosenStatusRet[];
extern const u8 BattleScript_WhiteHerbEnd2[];
extern const u8 BattleScript_WhiteHerbRet[];
extern const u8 BattleScript_ItemHealHP_RemoveItem[];
extern const u8 BattleScript_BerryPPHealEnd2[];
extern const u8 BattleScript_ItemHealHP_End2[];
extern const u8 BattleScript_ItemHealHP_Ret[];
extern const u8 BattleScript_HangedOnMsg[];
extern const u8 BattleScript_BerryConfuseHealEnd2[];
extern const u8 BattleScript_BerryStatRaiseEnd2[];
extern const u8 BattleScript_BerryFocusEnergyEnd2[];
extern const u8 BattleScript_ActionSelectionItemsCantBeUsed[];
extern const u8 BattleScript_AskIfWantsToForfeitMatch[];
extern const u8 BattleScript_PrintPlayerForfeited[];
extern const u8 BattleScript_PrintPlayerForfeitedLinkBattle[];
extern const u8 BattleScript_SuccessBallThrow[];
extern const u8 BattleScript_ShakeBallThrow[];
extern const u8 BattleScript_TrainerBallBlock[];
extern const u8 BattleScript_GhostGetOutGetOut[];
extern const u8 BattleScript_TooScaredToMove[];
extern const u8 BattleScript_IntimidateActivates[];
extern const u8 BattleScript_IgnoresWhileAsleep[];
extern const u8 BattleScript_IgnoresAndHitsItself[];
extern const u8 BattleScript_MoveEffectRecoil[];
extern const u8 BattleScript_FlushMessageBox[];
extern const u8 BattleScript_GhostBallDodge[];
extern const u8 BattleScript_OldMan_Pokedude_CaughtMessage[];
extern const u8 BattleScript_ItemUnveiledGhost[];
extern const u8 BattleScript_AngerPointActivation[];
extern const u8 BattleScript_BadDreamsActivates[];
extern const u8 BattleScript_DrySkinSunActivates[];
extern const u8 BattleScript_Frisk[];
extern const u8 BattleScript_SwitchInAbilityMsgRet[];
extern const u8 BattleScript_DisplaySwitchInMsg[];
extern const u8 BattleScript_MoveStatRaise[];
extern const u8 BattleScript_MoveStatRaise_PPLoss[];
extern const u8 BattleScript_PickpocketActivation[];
extern const u8 BattleScript_AirLock[];
extern const u8 BattleScript_SturdiedMsg[];
extern const u8 BattleScript_TrainerSlideMsgEnd2[];
extern const u8 BattleScript_TrainerSlideMsg[];
extern const u8 BattleScript_CursedBodyActivation[];
extern const u8 BattleScript_WeakArmorActivation[];
extern const u8 BattleScript_HarvestActivates[];
extern const u8 BattleScript_MoodyActivates[];
extern const u8 BattleScript_PowderMoveNoEffect[];
extern const u8 BattleScript_AlreadyAsleep[];
extern const u8 BattleScript_AlreadyPoisoned[];
extern const u8 BattleScript_AlreadyBurned[];
extern const u8 BattleScript_AlreadyParalyzed[];
extern const u8 BattleScript_WaterVeilPrevents[];
extern const u8 BattleScript_WaterVeilProtectedRet[];
extern const u8 BattleScript_ImmunityProtected[];
extern const u8 BattleScript_ImmunityProtectedRet[];
extern const u8 BattleScript_LimberProtected[];
extern const u8 BattleScript_LimberProtectedRet[];
extern const u8 BattleScript_NotAffected[];
extern const u8 BattleScript_PoisonTouchActivation[];
extern const u8 BattleScript_IllusionOff[];
extern const u8 BattleScript_ImposterActivates[];
extern const u8 BattleScript_MummyActivates[];
extern const u8 BattleScript_RaiseStatOnFaintingTarget[];
extern const u8 BattleScript_TargetAbilityStatRaiseRet[];
extern const u8 BattleScript_ZenModeActivatesPause[];
extern const u8 BattleScript_ZenModeActivates[];
extern const u8 BattleScript_TeamProtectedByFlowerVeil[];
extern const u8 BattleScript_TeamProtectedByFlowerVeilStatChange[];
extern const u8 BattleScript_TeamProtectedByPastelVeil[];
extern const u8 BattleScript_ProteanActivates[];
extern const u8 BattleScript_TeamProtectedBySweetVeil[];
extern const u8 BattleScript_StanceChangeActivation[];
extern const u8 BattleScript_GooeyActivates[];
extern const u8 BattleScript_PrimalWeatherBlocksMove[];
extern const u8 BattleScript_SandSpitActivated[];
extern const u8 BattleScript_SlowStartEnd[];
extern const u8 BattleScript_ButItFailedAtkStringPpReduce[];
extern const u8 BattleScript_PerishBodyActivates[];
extern const u8 BattleScript_WanderingSpiritActivates[];
extern const u8 BattleScript_WindPowerActivates[];
extern const u8 BattleScript_NeutralizingGasExits[];
extern const u8 BattleScript_CottonDownActivates[];
extern const u8 BattleScript_PastelVeilActivates[];
extern const u8 BattleScript_BattlerAbilityStatRaiseOnSwitchIn[];
extern const u8 BattleScript_IceFaceFade[];
extern const u8 BattleScript_DisguiseBusted[];
extern const u8 BattleScript_MultiHitPrintStrings[];
extern const u8 BattleScript_MagicianActivates[];
extern const u8 BattleScript_EmergencyExit[];
extern const u8 BattleScript_EmergencyExitEnd2[];
extern const u8 BattleScript_DarkTypePreventsPrankster[];
extern const u8 BattleScript_Pickup[];
extern const u8 BattleScript_ReceiverActivates[];
extern const u8 BattleScript_NeutralizingGasActivates[];
extern const u8 BattleScript_NeutralizingGasActivatesRet[];
extern const u8 BattleScript_ThrowBall[];
extern const u8 BattleScript_ItemUseMessageEnd[];
extern const u8 BattleScript_AbilityPreventSleep[];
extern const u8 BattleScript_AlreadyConfused[];
extern const u8 BattleScript_AromaVeilProtects[];
extern const u8 BattleScript_ObliviousPrevents[];
extern const u8 BattleScript_HealBellActivateOnBattler[];
extern const u8 BattleScript_HealBellActivateOnParty[];
extern const u8 BattleScript_SoundproofBlocksString[];
extern const u8 BattleScript_SubstituteBlocksHealBell[];
extern const u8 BattleScript_MagnitudeString[];
extern const u8 BattleScript_NaturePowerString[];
extern const u8 BattleScript_AvoidMoveWithAbility[];
extern const u8 BattleScript_CantMakeAsleep[];
extern const u8 BattleScript_WatchesCarefully[];
extern const u8 BattleScript_ThrowRock[];
extern const u8 BattleScript_ThrowBait[];
extern const u8 BattleScript_LeftoverWallyPrepToThrow[];
extern const u8 BattleScript_GulpMissileCatchPrey[];
extern const u8 BattleScript_GulpMissileSpitUpPrey[];
extern const u8 BattleScript_MirrorArmorBounceBack[];
extern const u8 BattleScript_Protected[];
extern const u8 BattleScript_SoulHeartActivates[];
extern const u8 BattleScript_MoveEffectStockpileWoreOff[];
extern const u8 BattleScript_MoveUsedGravityPrevented[];
extern const u8 BattleScript_HealingWishActivates[];
extern const u8 BattleScript_LunarDanceActivates[];
extern const u8 BattleScript_MoveEffectFeint[];
extern const u8 BattleScript_AngerShellActivates[];
extern const u8 BattleScript_MoveEffectDefSpDefDown[];
extern const u8 BattleScript_EmbargoEnds[];
extern const u8 BattleScript_CommanderActivates[];
extern const u8 BattleScript_BattleBondTransform[];
extern const u8 BattleScript_BattleBondBoost[];
extern const u8 BattleScript_HealBlockEnds[];
extern const u8 BattleScript_MoveUsedHealBlockPrevented[];
extern const u8 BattleScript_PrintEntryHazardsDmgString[];
extern const u8 BattleScript_ToxicDebrisActivation[];
extern const u8 BattleScript_PoisonPuppeteerActivation[];
extern const u8 BattleScript_SupersweetSyrupActivates[];
extern const u8 BattleScript_DoPreFaintEffects[];
extern const u8 BattleScript_Hospitality[];

extern const u8 *const gBattleScriptsForMoveEffects[];
extern const u8 *const gBattlescriptsForUsingItem[];

#endif // GUARD_BATTLE_SCRIPTS_H
