#ifndef GUARD_FIELD_SPECIALS_H
#define GUARD_FIELD_SPECIALS_H

#include "global.h"

enum CorpseRunFieldBlockReason
{
    CORPSE_RUN_FIELD_BLOCK_NONE,
    CORPSE_RUN_FIELD_BLOCK_MART,
    CORPSE_RUN_FIELD_BLOCK_HEAL,
    CORPSE_RUN_FIELD_BLOCK_PC,
    CORPSE_RUN_FIELD_BLOCK_GYM_BATTLE,
    CORPSE_RUN_FIELD_BLOCK_DAYCARE_TUTOR,
};

u8 GetLeadMonIndex(void);
s32 CountDigits(s32 number);
void TV_PrintIntToStringVar(u8 varidx, s32 number);
u16 GetStarterSpecies(void);
void StopPokemonLeagueLightingEffectTask(void);
void FieldCB_ShowPortholeView(void);
u8 ContextNpcGetTextColor(void);
void SetPCBoxToSendMon(u8);
u16 GetPCBoxToSendMon(void);
bool8 IsDestinationBoxFull(void);
bool8 ShouldShowBoxWasFullMessage(void);
u16 GetHiddenItemAttr(u32 hiddenItem, u8 attr);
u8 GetUnlockedSeviiAreas(void);
u32 GetPlayerTrainerId(void);
bool8 CutMoveRuinValleyCheck(void);
void CutMoveOpenDottedHoleDoor(void);
void RunMassageCooldownStepCounter(void);
void IncrementResortGorgeousStepCounter(void);
void IncrementBirthIslandRockStepCount(void);
void ResetCyclingRoadChallengeData(void);
void ResetFieldTasksArgs(void);
bool8 UsedPokemonCenterWarp(void);
void QuestLog_CheckDepartingIndoorsMap(void);
void QuestLog_TryRecordDepartedLocation(void);
u8 GetCorpseRunFieldBlockReason(void);
u16 GetCorpseRunFieldBlockTextId(void);
u16 StartGiovanniMemoryMode(void);
u16 SetGiovanniMemoryModeChapter3Complete(void);
u16 AbortGiovanniMemoryMode(void);
u16 RestoreGiovanniMemoryModeSnapshot(void);
u16 IsGiovanniMemoryModeReadyForBattle(void);
bool8 IsGiovanniMemoryModeCaptureLocked(void);
u16 ClearGiovanniMemoryModeCaptureLock(void);
u16 ReconcileGiovanniMemoryModeOutcome(void);
u16 IsGiovanniNarrativeBeatReadySpecial(void);
u16 GetGiovanniNarrativeBeatFallbackChapterSpecial(void);
u16 DebugForceGiovanniMemoryModeChapterState(void);
u16 ValidateGiovanniMemoryModeRocketFlags(void);
u16 ValidateGiovanniInteractionOverlayForCurrentMap(void);
u16 SyncGiovanniMemoryModeNpcState(void);
u16 Special_RocketOps_OpenTerminal(void);
u16 Special_RocketOps_ValidateCommandContext(void);
u16 Special_RocketOps_WritebackState(void);
bool8 HandleGiovanniMemoryModeWhiteout(void);
bool8 HandleGiovanniMemoryModeBootstrapOnLoad(void);

#endif // GUARD_FIELD_SPECIALS_H
