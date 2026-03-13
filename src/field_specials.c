#include "global.h"
#include "gflib.h"
#include "quest_log.h"
#include "list_menu.h"
#include "diploma.h"
#include "script.h"
#include "field_player_avatar.h"
#include "overworld.h"
#include "field_message_box.h"
#include "event_data.h"
#include "strings.h"
#include "battle.h"
#include "fieldmap.h"
#include "field_specials.h"
#include "region_map.h"
#include "task.h"
#include "battle_tower.h"
#include "field_camera.h"
#include "field_effect.h"
#include "event_object_movement.h"
#include "menu_indicators.h"
#include "random.h"
#include "mail_data.h"
#include "help_system.h"
#include "pokemon_storage_system.h"
#include "script_menu.h"
#include "data.h"
#include "pokedex.h"
#include "text_window.h"
#include "menu.h"
#include "mystery_gift.h"
#include "naming_screen.h"
#include "party_menu.h"
#include "pokemon.h"
#include "dynamic_placeholder_text_util.h"
#include "new_menu_helpers.h"
#include "constants/songs.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "constants/moves.h"
#include "constants/menu.h"
#include "constants/event_objects.h"
#include "constants/vars.h"
#include "constants/species.h"
#include "constants/metatile_labels.h"
#include "corpse_run.h"

static EWRAM_DATA u8 sElevatorCurrentFloorWindowId = 0;
static EWRAM_DATA u16 sElevatorScroll = 0;
static EWRAM_DATA u16 sElevatorCursorPos = 0;
static EWRAM_DATA struct ListMenuItem * sListMenuItems = NULL;
static EWRAM_DATA u16 sListMenuLastScrollPosition = 0;
static EWRAM_DATA u8 sPCBoxToSendMon = 0;
static EWRAM_DATA u8 sBrailleTextCursorSpriteID = 0;

static struct GiovanniMemoryModeSnapshot *GetGiovanniMemoryModeSnapshot(void)
{
    return &gSaveBlock1Ptr->giovanniMemorySnapshot;
}


struct GiovanniNpcStateReset
{
    u16 map;
    const u8 *localIds;
    u8 localIdCount;
};

struct GiovanniPartyTemplateMon
{
    u16 species;
    u8 level;
};

struct GiovanniPartyTemplate
{
    u8 chapterId;
    const struct GiovanniPartyTemplateMon *mons;
    u8 monCount;
};

static const struct GiovanniPartyTemplateMon sGiovanniMemoryParty_Chapter1[] =
{
    {SPECIES_ONIX, 25},
    {SPECIES_RHYHORN, 24},
    {SPECIES_KANGASKHAN, 29},
};

static const struct GiovanniPartyTemplateMon sGiovanniMemoryParty_Chapter2[] =
{
    {SPECIES_NIDORINO, 37},
    {SPECIES_KANGASKHAN, 35},
    {SPECIES_RHYHORN, 37},
    {SPECIES_NIDOQUEEN, 41},
};

static const struct GiovanniPartyTemplateMon sGiovanniMemoryParty_Chapter3[] =
{
    {SPECIES_RHYHORN, 45},
    {SPECIES_DUGTRIO, 42},
    {SPECIES_NIDOQUEEN, 44},
    {SPECIES_NIDOKING, 45},
    {SPECIES_RHYHORN, 50},
};

static const struct GiovanniPartyTemplate sGiovanniMemoryPartyTemplates[] =
{
    {
        .chapterId = 1,
        .mons = sGiovanniMemoryParty_Chapter1,
        .monCount = ARRAY_COUNT(sGiovanniMemoryParty_Chapter1),
    },
    {
        .chapterId = 2,
        .mons = sGiovanniMemoryParty_Chapter2,
        .monCount = ARRAY_COUNT(sGiovanniMemoryParty_Chapter2),
    },
    {
        .chapterId = 3,
        .mons = sGiovanniMemoryParty_Chapter3,
        .monCount = ARRAY_COUNT(sGiovanniMemoryParty_Chapter3),
    },
};

static const u8 sGiovanniNpcLocalIds_CeladonGameCorner[] =
{
    LOCALID_GAME_CORNER_GRUNT,
};

static const u8 sGiovanniNpcLocalIds_SilphCo11F[] =
{
    LOCALID_SILPH_CO_GIOVANNI,
    LOCALID_SILPH_CO_11F_GRUNT1,
    LOCALID_SILPH_CO_11F_GRUNT2,
};

static const u8 sGiovanniNpcLocalIds_ViridianGym[] =
{
    LOCALID_VIRIDIAN_GIOVANNI,
};

static const struct GiovanniNpcStateReset sGiovanniNpcStateResets[] =
{
    {
        .map = MAP_CELADON_CITY_GAME_CORNER,
        .localIds = sGiovanniNpcLocalIds_CeladonGameCorner,
        .localIdCount = ARRAY_COUNT(sGiovanniNpcLocalIds_CeladonGameCorner),
    },
    {
        .map = MAP_SILPH_CO_11F,
        .localIds = sGiovanniNpcLocalIds_SilphCo11F,
        .localIdCount = ARRAY_COUNT(sGiovanniNpcLocalIds_SilphCo11F),
    },
    {
        .map = MAP_VIRIDIAN_CITY_GYM,
        .localIds = sGiovanniNpcLocalIds_ViridianGym,
        .localIdCount = ARRAY_COUNT(sGiovanniNpcLocalIds_ViridianGym),
    },
};
struct GiovanniMapOverlay
{
    u16 map;
    u8 chapterId;
    u8 allowedWarpMask;
    u16 enabledTriggerMask;
    u8 npcSetId;
    u8 guaranteedProgressWarpId;
    u16 safeReturnMap;
    u8 safeReturnX;
    u8 safeReturnY;
    u8 recoveryFallbackX;
    u8 recoveryFallbackY;
};

enum GiovanniNarrativeBeatId
{
    GIO_BEAT_CH1_ENTRY,
    GIO_BEAT_CH1_LOCAL_DIALOGUE,
    GIO_BEAT_CH2_ENTRY,
    GIO_BEAT_CH2_LOCAL_DIALOGUE,
    GIO_BEAT_CH3_ENTRY,
    GIO_BEAT_CH3_LOCAL_DIALOGUE,
    GIO_BEAT_COUNT,
};

enum GiovanniBeatFallback
{
    GIO_BEAT_FALLBACK_ABORT,
    GIO_BEAT_FALLBACK_CH1,
    GIO_BEAT_FALLBACK_CH2,
    GIO_BEAT_FALLBACK_CH3,
};


enum RocketOpsCommandId
{
    ROCKETOPS_COMMAND_SECURE_ROUTE,
    ROCKETOPS_COMMAND_DEPLOY_AGENT,
    ROCKETOPS_COMMAND_DESTROY_DATA,
    ROCKETOPS_COMMAND_EXTRACT_STAFF,
    ROCKETOPS_COMMAND_COUNT,
};

enum RocketOpsTriggerType
{
    ROCKETOPS_TRIGGER_NONE,
    ROCKETOPS_TRIGGER_CONTROL_NODE,
    ROCKETOPS_TRIGGER_SURVEILLANCE_NODE,
    ROCKETOPS_TRIGGER_ARCHIVE_NODE,
    ROCKETOPS_TRIGGER_EXTRACTION_NODE,
};

struct GiovanniBeatFlagGate
{
    u16 flag;
    bool8 expectedSet;
};

struct GiovanniBeatVarGate
{
    u16 var;
    u16 expectedValue;
};

struct GiovanniBeatGate
{
    const struct GiovanniBeatFlagGate *required;
    u8 requiredCount;
    const struct GiovanniBeatFlagGate *prohibited;
    u8 prohibitedCount;
    const struct GiovanniBeatVarGate *requiredVars;
    u8 requiredVarCount;
    u8 fallbackBehavior;
};

static const struct GiovanniBeatFlagGate sGiovanniBeatRequired_Ch1Entry[] =
{
    {FLAG_DEFEATED_BROCK, TRUE},
};

static const struct GiovanniBeatFlagGate sGiovanniBeatProhibited_Ch1Entry[] =
{
    {FLAG_HIDE_CELADON_ROCKETS, TRUE},
    {FLAG_HIDE_SAFFRON_ROCKETS, TRUE},
    {FLAG_DEFEATED_LEADER_GIOVANNI, TRUE},
};

static const struct GiovanniBeatFlagGate sGiovanniBeatRequired_Ch2Entry[] =
{
    {FLAG_HIDE_CELADON_ROCKETS, TRUE},
};

static const struct GiovanniBeatFlagGate sGiovanniBeatProhibited_Ch2Entry[] =
{
    {FLAG_HIDE_SAFFRON_ROCKETS, TRUE},
    {FLAG_DEFEATED_LEADER_GIOVANNI, TRUE},
};

static const struct GiovanniBeatFlagGate sGiovanniBeatRequired_Ch3Entry[] =
{
    {FLAG_HIDE_CELADON_ROCKETS, TRUE},
    {FLAG_HIDE_SAFFRON_ROCKETS, TRUE},
    {FLAG_HIDE_SAFFRON_CIVILIANS, FALSE},
};

static const struct GiovanniBeatFlagGate sGiovanniBeatProhibited_Ch3Entry[] =
{
    {FLAG_DEFEATED_LEADER_GIOVANNI, TRUE},
};

static const struct GiovanniBeatVarGate sGiovanniBeatRequiredVars_Ch1LocalDialogue[] =
{
    {VAR_MODE_GIOVANNI_MEMORY, TRUE},
    {VAR_CHAPTER_ID, 1},
};

static const struct GiovanniBeatVarGate sGiovanniBeatRequiredVars_Ch2LocalDialogue[] =
{
    {VAR_MODE_GIOVANNI_MEMORY, TRUE},
    {VAR_CHAPTER_ID, 2},
};

static const struct GiovanniBeatVarGate sGiovanniBeatRequiredVars_Ch3LocalDialogue[] =
{
    {VAR_MODE_GIOVANNI_MEMORY, TRUE},
    {VAR_CHAPTER_ID, 3},
};

static const struct GiovanniBeatGate sGiovanniBeatGates[GIO_BEAT_COUNT] =
{
    [GIO_BEAT_CH1_ENTRY] =
    {
        .required = sGiovanniBeatRequired_Ch1Entry,
        .requiredCount = ARRAY_COUNT(sGiovanniBeatRequired_Ch1Entry),
        .prohibited = sGiovanniBeatProhibited_Ch1Entry,
        .prohibitedCount = ARRAY_COUNT(sGiovanniBeatProhibited_Ch1Entry),
        .requiredVars = NULL,
        .requiredVarCount = 0,
        .fallbackBehavior = GIO_BEAT_FALLBACK_ABORT,
    },
    [GIO_BEAT_CH1_LOCAL_DIALOGUE] =
    {
        .required = sGiovanniBeatRequired_Ch1Entry,
        .requiredCount = ARRAY_COUNT(sGiovanniBeatRequired_Ch1Entry),
        .prohibited = sGiovanniBeatProhibited_Ch1Entry,
        .prohibitedCount = ARRAY_COUNT(sGiovanniBeatProhibited_Ch1Entry),
        .requiredVars = sGiovanniBeatRequiredVars_Ch1LocalDialogue,
        .requiredVarCount = ARRAY_COUNT(sGiovanniBeatRequiredVars_Ch1LocalDialogue),
        .fallbackBehavior = GIO_BEAT_FALLBACK_CH1,
    },
    [GIO_BEAT_CH2_ENTRY] =
    {
        .required = sGiovanniBeatRequired_Ch2Entry,
        .requiredCount = ARRAY_COUNT(sGiovanniBeatRequired_Ch2Entry),
        .prohibited = sGiovanniBeatProhibited_Ch2Entry,
        .prohibitedCount = ARRAY_COUNT(sGiovanniBeatProhibited_Ch2Entry),
        .requiredVars = NULL,
        .requiredVarCount = 0,
        .fallbackBehavior = GIO_BEAT_FALLBACK_CH1,
    },
    [GIO_BEAT_CH2_LOCAL_DIALOGUE] =
    {
        .required = sGiovanniBeatRequired_Ch2Entry,
        .requiredCount = ARRAY_COUNT(sGiovanniBeatRequired_Ch2Entry),
        .prohibited = sGiovanniBeatProhibited_Ch2Entry,
        .prohibitedCount = ARRAY_COUNT(sGiovanniBeatProhibited_Ch2Entry),
        .requiredVars = sGiovanniBeatRequiredVars_Ch2LocalDialogue,
        .requiredVarCount = ARRAY_COUNT(sGiovanniBeatRequiredVars_Ch2LocalDialogue),
        .fallbackBehavior = GIO_BEAT_FALLBACK_CH2,
    },
    [GIO_BEAT_CH3_ENTRY] =
    {
        .required = sGiovanniBeatRequired_Ch3Entry,
        .requiredCount = ARRAY_COUNT(sGiovanniBeatRequired_Ch3Entry),
        .prohibited = sGiovanniBeatProhibited_Ch3Entry,
        .prohibitedCount = ARRAY_COUNT(sGiovanniBeatProhibited_Ch3Entry),
        .requiredVars = NULL,
        .requiredVarCount = 0,
        .fallbackBehavior = GIO_BEAT_FALLBACK_CH2,
    },
    [GIO_BEAT_CH3_LOCAL_DIALOGUE] =
    {
        .required = sGiovanniBeatRequired_Ch3Entry,
        .requiredCount = ARRAY_COUNT(sGiovanniBeatRequired_Ch3Entry),
        .prohibited = sGiovanniBeatProhibited_Ch3Entry,
        .prohibitedCount = ARRAY_COUNT(sGiovanniBeatProhibited_Ch3Entry),
        .requiredVars = sGiovanniBeatRequiredVars_Ch3LocalDialogue,
        .requiredVarCount = ARRAY_COUNT(sGiovanniBeatRequiredVars_Ch3LocalDialogue),
        .fallbackBehavior = GIO_BEAT_FALLBACK_CH3,
    },
};

enum
{
    GIOVANNI_NPC_SET_NONE,
    GIOVANNI_NPC_SET_CELADON_GAME_CORNER,
    GIOVANNI_NPC_SET_SILPH_CO,
    GIOVANNI_NPC_SET_VIRIDIAN_GYM,
};

static const struct GiovanniMapOverlay sGiovanniMapOverlays[] =
{
    {
        .map = MAP_DIGLETTS_CAVE_NORTH_ENTRANCE,
        .chapterId = 1,
        .allowedWarpMask = (1 << 0) | (1 << 1),
        .enabledTriggerMask = (1 << 0),
        .npcSetId = GIOVANNI_NPC_SET_NONE,
        .guaranteedProgressWarpId = 0,
        .safeReturnMap = MAP_ROCKET_HIDEOUT_B4F,
        .safeReturnX = 19,
        .safeReturnY = 6,
        .recoveryFallbackX = 3,
        .recoveryFallbackY = 4,
    },
    {
        .map = MAP_ROCKET_HIDEOUT_B4F,
        .chapterId = 1,
        .allowedWarpMask = (1 << 0),
        .enabledTriggerMask = (1 << 0) | (1 << 1) | (1 << 2),
        .npcSetId = GIOVANNI_NPC_SET_NONE,
        .guaranteedProgressWarpId = 0,
        .safeReturnMap = MAP_ROCKET_HIDEOUT_B4F,
        .safeReturnX = 19,
        .safeReturnY = 6,
        .recoveryFallbackX = 19,
        .recoveryFallbackY = 6,
    },
    {
        .map = MAP_VIRIDIAN_CITY,
        .chapterId = 3,
        .allowedWarpMask = (1 << 2),
        .enabledTriggerMask = 0,
        .npcSetId = GIOVANNI_NPC_SET_NONE,
        .guaranteedProgressWarpId = 2,
        .safeReturnMap = MAP_VIRIDIAN_CITY_GYM,
        .safeReturnX = 17,
        .safeReturnY = 21,
        .recoveryFallbackX = 17,
        .recoveryFallbackY = 21,
    },
    {
        .map = MAP_CELADON_CITY_GAME_CORNER,
        .chapterId = 1,
        .allowedWarpMask = (1 << 3),
        .enabledTriggerMask = (1 << 0) | (1 << 1) | (1 << 2),
        .npcSetId = GIOVANNI_NPC_SET_CELADON_GAME_CORNER,
        .guaranteedProgressWarpId = 3,
        .safeReturnMap = MAP_ROCKET_HIDEOUT_B4F,
        .safeReturnX = 19,
        .safeReturnY = 6,
        .recoveryFallbackX = 19,
        .recoveryFallbackY = 6,
    },
    {
        .map = MAP_UNDERGROUND_PATH_NORTH_SOUTH_TUNNEL,
        .chapterId = 2,
        .allowedWarpMask = (1 << 0) | (1 << 1),
        .enabledTriggerMask = (1 << 0) | (1 << 1),
        .npcSetId = GIOVANNI_NPC_SET_NONE,
        .guaranteedProgressWarpId = 0,
        .safeReturnMap = MAP_SILPH_CO_11F,
        .safeReturnX = 6,
        .safeReturnY = 14,
        .recoveryFallbackX = 6,
        .recoveryFallbackY = 14,
    },
    {
        .map = MAP_DIGLETTS_CAVE_SOUTH_ENTRANCE,
        .chapterId = 2,
        .allowedWarpMask = (1 << 0) | (1 << 1),
        .enabledTriggerMask = (1 << 0),
        .npcSetId = GIOVANNI_NPC_SET_NONE,
        .guaranteedProgressWarpId = 1,
        .safeReturnMap = MAP_SILPH_CO_11F,
        .safeReturnX = 6,
        .safeReturnY = 14,
        .recoveryFallbackX = 82,
        .recoveryFallbackY = 70,
    },
    {
        .map = MAP_SILPH_CO_11F,
        .chapterId = 2,
        .allowedWarpMask = 0x07,
        .enabledTriggerMask = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
        .npcSetId = GIOVANNI_NPC_SET_SILPH_CO,
        .guaranteedProgressWarpId = 1,
        .safeReturnMap = MAP_SILPH_CO_11F,
        .safeReturnX = 6,
        .safeReturnY = 14,
        .recoveryFallbackX = 6,
        .recoveryFallbackY = 14,
    },
    {
        .map = MAP_VIRIDIAN_CITY_GYM,
        .chapterId = 3,
        .allowedWarpMask = 0x07,
        .enabledTriggerMask = (1 << 0) | (1 << 1) | (1 << 2),
        .npcSetId = GIOVANNI_NPC_SET_VIRIDIAN_GYM,
        .guaranteedProgressWarpId = 1,
        .safeReturnMap = MAP_VIRIDIAN_CITY_GYM,
        .safeReturnX = 17,
        .safeReturnY = 21,
        .recoveryFallbackX = 17,
        .recoveryFallbackY = 21,
    },
};

static const struct GiovanniMapOverlay *GetGiovanniOverlayForMap(u16 map)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sGiovanniMapOverlays); i++)
    {
        if (sGiovanniMapOverlays[i].map == map)
            return &sGiovanniMapOverlays[i];
    }

    return NULL;
}

static void WarpPlayerToGiovanniOverlaySafeReturn(const struct GiovanniMapOverlay *overlay)
{
    SetWarpDestination(MAP_GROUP(overlay->safeReturnMap), MAP_NUM(overlay->safeReturnMap), WARP_ID_NONE, overlay->safeReturnX, overlay->safeReturnY);
    WarpIntoMap();
    SetMainCallback2(CB2_LoadMap);
}

static void ApplyGiovanniOverlayForCurrentMap(const struct GiovanniMapOverlay *overlay)
{
    u16 map = overlay->map;

    if (map == MAP_VIRIDIAN_CITY)
    {
        if ((overlay->enabledTriggerMask & 1) == 0)
            VarSet(VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, 2);
        VarSet(VAR_MAP_SCENE_VIRIDIAN_CITY_GYM_DOOR, 1);
    }
    else if (map == MAP_SILPH_CO_11F)
    {
        FlagClear(FLAG_HIDE_SILPH_ROCKETS);
        VarSet(VAR_MAP_SCENE_SILPH_CO_11F, 0);
    }
    else if (map == MAP_VIRIDIAN_CITY_GYM)
    {
        FlagClear(FLAG_HIDE_VIRIDIAN_GIOVANNI);
    }
}

static void ApplyGiovanniOverlayNpcSet(const struct GiovanniMapOverlay *overlay)
{
    switch (overlay->npcSetId)
    {
    case GIOVANNI_NPC_SET_CELADON_GAME_CORNER:
        FlagClear(FLAG_HIDE_GAME_CORNER_ROCKET);
        FlagSet(FLAG_OPENED_ROCKET_HIDEOUT);
        break;
    case GIOVANNI_NPC_SET_SILPH_CO:
        FlagClear(FLAG_HIDE_SILPH_ROCKETS);
        FlagClear(FLAG_HIDE_SAFFRON_ROCKETS);
        break;
    case GIOVANNI_NPC_SET_VIRIDIAN_GYM:
        FlagClear(FLAG_HIDE_VIRIDIAN_GIOVANNI);
        break;
    case GIOVANNI_NPC_SET_NONE:
    default:
        break;
    }
}

static bool8 IsOverlayWarpAllowed(const struct GiovanniMapOverlay *overlay, u8 warpId)
{
    if (warpId >= 8)
        return FALSE;

    return (overlay->allowedWarpMask & (1 << warpId)) != 0;
}

static bool8 DoesGiovanniBeatGatePass(const struct GiovanniBeatGate *gate)
{
    u8 i;

    for (i = 0; i < gate->requiredCount; i++)
    {
        if (FlagGet(gate->required[i].flag) != gate->required[i].expectedSet)
            return FALSE;
    }

    for (i = 0; i < gate->prohibitedCount; i++)
    {
        if (FlagGet(gate->prohibited[i].flag) == gate->prohibited[i].expectedSet)
            return FALSE;
    }

    for (i = 0; i < gate->requiredVarCount; i++)
    {
        if (VarGet(gate->requiredVars[i].var) != gate->requiredVars[i].expectedValue)
            return FALSE;
    }

    return TRUE;
}

static u8 GetGiovanniBeatFallbackChapter(u8 fallbackBehavior)
{
    switch (fallbackBehavior)
    {
    case GIO_BEAT_FALLBACK_CH1:
        return 1;
    case GIO_BEAT_FALLBACK_CH2:
        return 2;
    case GIO_BEAT_FALLBACK_CH3:
        return 3;
    case GIO_BEAT_FALLBACK_ABORT:
    default:
        return 0;
    }
}

static u8 ResolveGiovanniBeatFallbackChapter(u8 beatId)
{
    if (beatId >= GIO_BEAT_COUNT)
        return 0;

    return GetGiovanniBeatFallbackChapter(sGiovanniBeatGates[beatId].fallbackBehavior);
}


static bool8 IsGiovanniNarrativeBeatReady(u8 beatId)
{
    if (beatId >= GIO_BEAT_COUNT)
        return FALSE;

    return DoesGiovanniBeatGatePass(&sGiovanniBeatGates[beatId]);
}



static void ApplyGiovanniMemoryModeNpcFlags(u8 chapterId)
{
    if (chapterId == 0)
    {
        FlagClear(FLAG_GIO_MEM_HIDE_CELADON_ROCKETS);
        FlagClear(FLAG_GIO_MEM_HIDE_SAFFRON_ROCKETS);
        FlagClear(FLAG_GIO_MEM_HIDE_SAFFRON_CIVILIANS);
    }
    else if (chapterId == 1)
    {
        FlagClear(FLAG_GIO_MEM_HIDE_CELADON_ROCKETS);
        FlagClear(FLAG_GIO_MEM_HIDE_SAFFRON_ROCKETS);
        FlagSet(FLAG_GIO_MEM_HIDE_SAFFRON_CIVILIANS);
    }
    else if (chapterId == 2)
    {
        FlagSet(FLAG_GIO_MEM_HIDE_CELADON_ROCKETS);
        FlagClear(FLAG_GIO_MEM_HIDE_SAFFRON_ROCKETS);
        FlagSet(FLAG_GIO_MEM_HIDE_SAFFRON_CIVILIANS);
    }
    else
    {
        FlagSet(FLAG_GIO_MEM_HIDE_CELADON_ROCKETS);
        FlagSet(FLAG_GIO_MEM_HIDE_SAFFRON_ROCKETS);
        FlagClear(FLAG_GIO_MEM_HIDE_SAFFRON_CIVILIANS);
    }

    if (FlagGet(FLAG_GIO_MEM_HIDE_CELADON_ROCKETS))
        FlagSet(FLAG_HIDE_CELADON_ROCKETS);
    else
        FlagClear(FLAG_HIDE_CELADON_ROCKETS);

    if (FlagGet(FLAG_GIO_MEM_HIDE_SAFFRON_ROCKETS))
        FlagSet(FLAG_HIDE_SAFFRON_ROCKETS);
    else
        FlagClear(FLAG_HIDE_SAFFRON_ROCKETS);

    if (FlagGet(FLAG_GIO_MEM_HIDE_SAFFRON_CIVILIANS))
        FlagSet(FLAG_HIDE_SAFFRON_CIVILIANS);
    else
        FlagClear(FLAG_HIDE_SAFFRON_CIVILIANS);

    if (chapterId == 0)
        FlagSet(FLAG_HIDE_VIRIDIAN_GIOVANNI);
    else
        FlagClear(FLAG_HIDE_VIRIDIAN_GIOVANNI);
}

static void ReloadGiovanniMemoryModeNpcObjects(void)
{
    u8 i;
    u8 j;

    for (i = 0; i < ARRAY_COUNT(sGiovanniNpcStateResets); i++)
    {
        u8 mapNum = MAP_NUM(sGiovanniNpcStateResets[i].map);
        u8 mapGroup = MAP_GROUP(sGiovanniNpcStateResets[i].map);

        for (j = 0; j < sGiovanniNpcStateResets[i].localIdCount; j++)
        {
            u8 localId = sGiovanniNpcStateResets[i].localIds[j];
            RemoveObjectEventByLocalIdAndMap(localId, mapNum, mapGroup);
            TrySpawnObjectEvent(localId, mapNum, mapGroup);
        }
    }
}

static void RunGiovanniMemoryModeResetHooks(u8 chapterId)
{
    VarSet(VAR_MODE_GIOVANNI_MEMORY, chapterId != 0);
    VarSet(VAR_CHAPTER_ID, chapterId);
    ApplyGiovanniMemoryModeNpcFlags(chapterId);
    ReloadGiovanniMemoryModeNpcObjects();
}

static u8 GetGiovanniMemoryModeChapterId(void)
{
    if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return 0;
    if (!FlagGet(FLAG_GIO_MEM_CH1_COMPLETE))
        return 1;
    if (!FlagGet(FLAG_GIO_MEM_CH2_COMPLETE))
        return 2;
    return 3;
}

static void SnapshotGiovanniRocketProgressFlags(struct GiovanniMemoryModeFlagSnapshot *flags)
{
    flags->hideMiscKantoRockets = FlagGet(FLAG_HIDE_MISC_KANTO_ROCKETS);
    flags->defeatedLeaderGiovanni = FlagGet(FLAG_DEFEATED_LEADER_GIOVANNI);
    flags->gotTm26FromGiovanni = FlagGet(FLAG_GOT_TM26_FROM_GIOVANNI);
    flags->hideViridianGiovanni = FlagGet(FLAG_HIDE_VIRIDIAN_GIOVANNI);
    flags->hideHideoutGiovanni = FlagGet(FLAG_HIDE_HIDEOUT_GIOVANNI);
    flags->hideCeruleanRocket = FlagGet(FLAG_HIDE_CERULEAN_ROCKET);
    flags->hideSaffronRockets = FlagGet(FLAG_HIDE_SAFFRON_ROCKETS);
    flags->hideCeladonRockets = FlagGet(FLAG_HIDE_CELADON_ROCKETS);
    flags->hideSaffronCivilians = FlagGet(FLAG_HIDE_SAFFRON_CIVILIANS);
    flags->hideTowerRocket1 = FlagGet(FLAG_HIDE_TOWER_ROCKET_1);
    flags->hideTowerRocket2 = FlagGet(FLAG_HIDE_TOWER_ROCKET_2);
    flags->hideTowerRocket3 = FlagGet(FLAG_HIDE_TOWER_ROCKET_3);
    flags->rescuedMrFuji = FlagGet(FLAG_RESCUED_MR_FUJI);
}

static void RestoreGiovanniRocketProgressFlags(const struct GiovanniMemoryModeFlagSnapshot *flags)
{
    if (flags->hideMiscKantoRockets)
        FlagSet(FLAG_HIDE_MISC_KANTO_ROCKETS);
    else
        FlagClear(FLAG_HIDE_MISC_KANTO_ROCKETS);

    if (flags->defeatedLeaderGiovanni)
        FlagSet(FLAG_DEFEATED_LEADER_GIOVANNI);
    else
        FlagClear(FLAG_DEFEATED_LEADER_GIOVANNI);

    if (flags->gotTm26FromGiovanni)
        FlagSet(FLAG_GOT_TM26_FROM_GIOVANNI);
    else
        FlagClear(FLAG_GOT_TM26_FROM_GIOVANNI);

    if (flags->hideViridianGiovanni)
        FlagSet(FLAG_HIDE_VIRIDIAN_GIOVANNI);
    else
        FlagClear(FLAG_HIDE_VIRIDIAN_GIOVANNI);

    if (flags->hideHideoutGiovanni)
        FlagSet(FLAG_HIDE_HIDEOUT_GIOVANNI);
    else
        FlagClear(FLAG_HIDE_HIDEOUT_GIOVANNI);

    if (flags->hideCeruleanRocket)
        FlagSet(FLAG_HIDE_CERULEAN_ROCKET);
    else
        FlagClear(FLAG_HIDE_CERULEAN_ROCKET);

    if (flags->hideSaffronRockets)
        FlagSet(FLAG_HIDE_SAFFRON_ROCKETS);
    else
        FlagClear(FLAG_HIDE_SAFFRON_ROCKETS);

    if (flags->hideCeladonRockets)
        FlagSet(FLAG_HIDE_CELADON_ROCKETS);
    else
        FlagClear(FLAG_HIDE_CELADON_ROCKETS);

    if (flags->hideSaffronCivilians)
        FlagSet(FLAG_HIDE_SAFFRON_CIVILIANS);
    else
        FlagClear(FLAG_HIDE_SAFFRON_CIVILIANS);

    if (flags->hideTowerRocket1)
        FlagSet(FLAG_HIDE_TOWER_ROCKET_1);
    else
        FlagClear(FLAG_HIDE_TOWER_ROCKET_1);

    if (flags->hideTowerRocket2)
        FlagSet(FLAG_HIDE_TOWER_ROCKET_2);
    else
        FlagClear(FLAG_HIDE_TOWER_ROCKET_2);

    if (flags->hideTowerRocket3)
        FlagSet(FLAG_HIDE_TOWER_ROCKET_3);
    else
        FlagClear(FLAG_HIDE_TOWER_ROCKET_3);

    if (flags->rescuedMrFuji)
        FlagSet(FLAG_RESCUED_MR_FUJI);
    else
        FlagClear(FLAG_RESCUED_MR_FUJI);
}

COMMON_DATA struct ListMenuTemplate sFieldSpecialsListMenuTemplate = {0};
COMMON_DATA u16 sFieldSpecialsListMenuScrollBuffer = 0;

static void Task_AnimatePcTurnOn(u8 taskId);
static void PcTurnOnUpdateMetatileId(bool16 flag);
static void Task_ShakeScreen(u8 taskId);
static void Task_EndScreenShake(u8 taskId);
static u16 SampleResortGorgeousMon(void);
static u16 SampleResortGorgeousReward(void);
static void Task_ElevatorShake(u8 taskId);
static void AnimateElevatorWindowView(u16 nfloors, bool8 direction);
static void Task_AnimateElevatorWindowView(u8 taskId);
static void Task_CreateScriptListMenu(u8 taskId);
static void CreateScriptListMenu(void);
static void ScriptListMenuMoveCursorFunction(s32 nothing, bool8 is, struct ListMenu * used);
static void Task_ListMenuHandleInput(u8 taskId);
static void Task_DestroyListMenu(u8 taskId);
static void Task_SuspendListMenu(u8 taskId);
static void Task_RedrawScrollArrowsAndWaitInput(u8 taskId);
static void Task_CreateMenuRemoveScrollIndicatorArrowPair(u8 taskId);
static void Task_ListMenuRemoveScrollIndicatorArrowPair(u8 taskId);
static u16 GetStarterSpeciesById(u16 starterIdx);
static void ChangeBoxPokemonNickname_CB(void);
static void ChangePokemonNickname_CB(void);
static void Task_RunPokemonLeagueLightingEffect(u8 taskId);
static void Task_CancelPokemonLeagueLightingEffect(u8 taskId);
static void Task_DoDeoxysTriangleInteraction(u8 taskId);
static void MoveDeoxysObject(u8 num);
static void Task_WaitDeoxysFieldEffect(u8 taskId);
static void Task_WingFlapSound(u8 taskId);

static u8 *const sStringVarPtrs[] = {
    gStringVar1,
    gStringVar2,
    gStringVar3
};

void ShowDiploma(void)
{
    QuestLog_CutRecording();
    SetMainCallback2(CB2_ShowDiploma);
    LockPlayerFieldControls();
}

void ForcePlayerOntoBike(void)
{
    if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_ON_FOOT)
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_MACH_BIKE);
    Overworld_SetSavedMusic(MUS_CYCLING);
    Overworld_ChangeMusicTo(MUS_CYCLING);
}

void ResetCyclingRoadChallengeData(void)
{

}

u8 GetPlayerAvatarBike(void)
{
    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_ACRO_BIKE))
        return 1;
    else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE))
        return 2;
    else
        return 0;
}

void ShowFieldMessageStringVar4(void)
{
    ShowFieldMessage(gStringVar4);
}



u8 GetCorpseRunFieldBlockReason(void)
{
    bool8 corpseRunBlocked = CorpseRun_IsActive();
    bool8 giovanniMemoryFacilityBlocked = FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE)
        && VarGet(VAR_MODE_GIOVANNI_MEMORY) == TRUE;

    if (!corpseRunBlocked && !giovanniMemoryFacilityBlocked)
        return CORPSE_RUN_FIELD_BLOCK_NONE;

    switch (gSpecialVar_0x8004)
    {
    case CORPSE_RUN_FIELD_BLOCK_MART:
    case CORPSE_RUN_FIELD_BLOCK_HEAL:
    case CORPSE_RUN_FIELD_BLOCK_PC:
        return gSpecialVar_0x8004;
    case CORPSE_RUN_FIELD_BLOCK_GYM_BATTLE:
    case CORPSE_RUN_FIELD_BLOCK_DAYCARE_TUTOR:
        return corpseRunBlocked ? gSpecialVar_0x8004 : CORPSE_RUN_FIELD_BLOCK_NONE;
    default:
        return CORPSE_RUN_FIELD_BLOCK_NONE;
    }
}

u16 GetCorpseRunFieldBlockTextId(void)
{
    switch (GetCorpseRunFieldBlockReason())
    {
    case CORPSE_RUN_FIELD_BLOCK_MART:
        return 1;
    case CORPSE_RUN_FIELD_BLOCK_HEAL:
        return 2;
    case CORPSE_RUN_FIELD_BLOCK_PC:
        return 3;
    case CORPSE_RUN_FIELD_BLOCK_GYM_BATTLE:
        return 4;
    case CORPSE_RUN_FIELD_BLOCK_DAYCARE_TUTOR:
        return 5;
    default:
        return 0;
    }
}
void GetPlayerXY(void)
{
    gSpecialVar_0x8004 = gSaveBlock1Ptr->pos.x;
    gSpecialVar_0x8005 = gSaveBlock1Ptr->pos.y;
}

u8 GetPlayerTrainerIdOnesDigit(void)
{
    u16 playerId = (gSaveBlock2Ptr->playerTrainerId[1] << 8) | gSaveBlock2Ptr->playerTrainerId[0];
    return playerId % 10;
}

void BufferBigGuyOrBigGirlString(void)
{
    if (gSaveBlock2Ptr->playerGender == MALE)
        StringCopy(gStringVar1, gText_BigGuy);
    else
        StringCopy(gStringVar1, gText_BigGirl);
}

void BufferSonOrDaughterString(void)
{
    if (gSaveBlock2Ptr->playerGender == MALE)
        StringCopy(gStringVar1, gText_Daughter);
    else
        StringCopy(gStringVar1, gText_Son);
}

u8 GetBattleOutcome(void)
{
    return gBattleOutcome;
}

void SetHiddenItemFlag(void)
{
    FlagSet(gSpecialVar_0x8004);
}

u8 GetLeadMonFriendship(void)
{
    struct Pokemon * pokemon = &gPlayerParty[GetLeadMonIndex()];
    if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) == 255)
        return 6;
    else if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 200)
        return 5;
    else if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 150)
        return 4;
    else if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 100)
        return 3;
    else if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 50)
        return 2;
    else if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) > 0)
        return 1;
    else
        return 0;
}

void ShowTownMap(void)
{
    QuestLog_CutRecording();
    InitRegionMapWithExitCB(REGIONMAP_TYPE_WALL, CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

bool8 PlayerHasGrassPokemonInParty(void)
{
    u8 i;
    struct Pokemon * pokemon;
    u16 species;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        pokemon = &gPlayerParty[i];
        if (GetMonData(pokemon, MON_DATA_SANITY_HAS_SPECIES)
         && !GetMonData(pokemon, MON_DATA_IS_EGG)
        )
        {
            species = GetMonData(pokemon, MON_DATA_SPECIES);
            if (gSpeciesInfo[species].types[0] == TYPE_GRASS || gSpeciesInfo[species].types[1] == TYPE_GRASS)
                return TRUE;
        }
    }
    return FALSE;
}

#define tState data[0]
#define tTimer data[1]

void AnimatePcTurnOn(void)
{
    u8 taskId;

    if (FuncIsActiveTask(Task_AnimatePcTurnOn) != TRUE)
    {
        taskId = CreateTask(Task_AnimatePcTurnOn, 8);
        gTasks[taskId].tState = 0;
        gTasks[taskId].tTimer = 0;
    }
}

// PC flickers on and off while turning on
static void Task_AnimatePcTurnOn(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (tTimer == 6)
    {
        PcTurnOnUpdateMetatileId(tState & 1);
        DrawWholeMapView();
        tTimer = 0;
        tState++;
        if (tState == 5)
            DestroyTask(taskId);
    }
    tTimer++;
}

#undef tState
#undef tTimer

static void PcTurnOnUpdateMetatileId(bool16 flickerOff)
{
    u16 metatileId = 0;
    s8 deltaX = 0;
    s8 deltaY = 0;
    u8 direction = GetPlayerFacingDirection();

    switch (direction)
    {
    case DIR_NORTH:
        deltaX = 0;
        deltaY = -1;
        break;
    case DIR_WEST:
        deltaX = -1;
        deltaY = -1;
        break;
    case DIR_EAST:
        deltaX = 1;
        deltaY = -1;
        break;
    }
    if (flickerOff)
    {
        if (gSpecialVar_0x8004 == 0)
            metatileId = METATILE_Building_PCOff;
        else if (gSpecialVar_0x8004 == 1)
            metatileId = METATILE_GenericBuilding1_PlayersPCOff;
        else if (gSpecialVar_0x8004 == 2)
            metatileId = METATILE_GenericBuilding1_PlayersPCOff;
    }
    else
    {
        if (gSpecialVar_0x8004 == 0)
            metatileId = METATILE_Building_PCOn;
        else if (gSpecialVar_0x8004 == 1)
            metatileId = METATILE_GenericBuilding1_PlayersPCOn;
        else if (gSpecialVar_0x8004 == 2)
            metatileId = METATILE_GenericBuilding1_PlayersPCOn;
    }
    MapGridSetMetatileIdAt(gSaveBlock1Ptr->pos.x + deltaX + MAP_OFFSET, gSaveBlock1Ptr->pos.y + deltaY + MAP_OFFSET, metatileId | MAPGRID_COLLISION_MASK);
}

void AnimatePcTurnOff()
{
    u16 metatileId = 0;
    s8 deltaX = 0;
    s8 deltaY = 0;
    u8 direction = GetPlayerFacingDirection();

    switch (direction)
    {
    case DIR_NORTH:
        deltaX = 0;
        deltaY = -1;
        break;
    case DIR_WEST:
        deltaX = -1;
        deltaY = -1;
        break;
    case DIR_EAST:
        deltaX = 1;
        deltaY = -1;
        break;
    }
    if (gSpecialVar_0x8004 == 0)
        metatileId = METATILE_Building_PCOff;
    else if (gSpecialVar_0x8004 == 1)
        metatileId = METATILE_GenericBuilding1_PlayersPCOff;
    else if (gSpecialVar_0x8004 == 2)
        metatileId = METATILE_GenericBuilding1_PlayersPCOff;
    MapGridSetMetatileIdAt(gSaveBlock1Ptr->pos.x + deltaX + MAP_OFFSET, gSaveBlock1Ptr->pos.y + deltaY + MAP_OFFSET, metatileId | MAPGRID_COLLISION_MASK);
    DrawWholeMapView();
}

void SpawnCameraObject(void)
{
    u8 objectEventId = SpawnSpecialObjectEventParameterized(OBJ_EVENT_GFX_YOUNGSTER, 8, LOCALID_CAMERA, gSaveBlock1Ptr->pos.x + MAP_OFFSET, gSaveBlock1Ptr->pos.y + MAP_OFFSET, 3);
    gObjectEvents[objectEventId].invisible = TRUE;
    CameraObjectSetFollowedObjectId(gObjectEvents[objectEventId].spriteId);
}

void RemoveCameraObject(void)
{
    CameraObjectSetFollowedObjectId(GetPlayerAvatarObjectId());
    RemoveObjectEventByLocalIdAndMap(LOCALID_CAMERA, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
}

void BufferEReaderTrainerName(void)
{
    CopyEReaderTrainerName5(gStringVar1);
}

// Unused
static const u8 sSlotMachineRandomSeeds[] = {
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    12,
    2,
    4,
    5,
    1,
    8,
    7,
    11,
    3,
    10,
    9,
    6
};

static const u8 sSlotMachineIndices[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    2,
    2,
    2,
    3,
    3,
    3,
    4,
    4,
    5
};

u8 GetRandomSlotMachineId(void)
{
    u16 rval = Random() % NELEMS(sSlotMachineIndices);
    return sSlotMachineIndices[rval];
}

bool8 LeadMonHasEffortRibbon(void)
{
    u8 leadMonIdx = GetLeadMonIndex();
    return GetMonData(&gPlayerParty[leadMonIdx], MON_DATA_EFFORT_RIBBON, NULL);
}

void GiveLeadMonEffortRibbon(void)
{
    u8 leadMonIdx;
    bool8 param;
    IncrementGameStat(GAME_STAT_RECEIVED_RIBBONS);
    FlagSet(FLAG_SYS_RIBBON_GET);
    param = TRUE;
    leadMonIdx = GetLeadMonIndex();
    SetMonData(&gPlayerParty[leadMonIdx], MON_DATA_EFFORT_RIBBON, &param);
}

bool8 AreLeadMonEVsMaxedOut(void)
{
    u8 leadMonIndex = GetLeadMonIndex();
    if (GetMonEVCount(&gPlayerParty[leadMonIndex]) >= 510)
        return TRUE;
    else
        return FALSE;
}

bool8 IsStarterFirstStageInParty(void)
{
    u16 species = GetStarterSpeciesById(VarGet(VAR_STARTER_MON));
    u8 partyCount = CalculatePlayerPartyCount();
    u8 i;
    for (i = 0; i < partyCount; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL) == species)
            return TRUE;
    }
    return FALSE;
}

bool8 IsThereRoomInAnyBoxForMorePokemon(void)
{
    u16 i;
    u16 j;
    for (i = 0; i < TOTAL_BOXES_COUNT; i++)
    {
        for (j = 0; j < IN_BOX_COUNT; j++)
        {
            if (GetBoxMonDataAt(i, j, MON_DATA_SPECIES) == SPECIES_NONE)
                return TRUE;
        }
    }
    return FALSE;
}

bool8 IsPokerusInParty(void)
{
    if (!CheckPartyPokerus(gPlayerParty, 0x3F))
        return FALSE;
    else
        return TRUE;
}

#define tXtrans   data[0]
#define tTimer    data[1]
#define tNremain  data[2]
#define tDuration data[3]
#define tYtrans   data[4]

void ShakeScreen(void)
{
    /*
     * 0x8004 = x translation
     * 0x8005 = y translation
     * 0x8006 = num interations
     * 0x8007 = duration of an iteration
     */
    u8 taskId = CreateTask(Task_ShakeScreen, 9);
    gTasks[taskId].tXtrans = gSpecialVar_0x8005;
    gTasks[taskId].tTimer = 0;
    gTasks[taskId].tNremain = gSpecialVar_0x8006;
    gTasks[taskId].tDuration = gSpecialVar_0x8007;
    gTasks[taskId].tYtrans = gSpecialVar_0x8004;
    SetCameraPanningCallback(NULL);
    PlaySE(SE_M_STRENGTH);
}

static void Task_ShakeScreen(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    tTimer++;
    if (tTimer % tDuration == 0)
    {
        tTimer = 0;
        tNremain--;
        tXtrans = -tXtrans;
        tYtrans = -tYtrans;
        SetCameraPanning(tXtrans, tYtrans);
        if (tNremain == 0)
        {
            Task_EndScreenShake(taskId);
            InstallCameraPanAheadCallback();
        }
    }
}

static void Task_EndScreenShake(u8 taskId)
{
    DestroyTask(taskId);
    ScriptContext_Enable();
}

#undef tYtrans
#undef tDuration
#undef tNremain
#undef tTimer
#undef tXtrans

u8 GetLeadMonIndex(void)
{
    u8 partyCount = CalculatePlayerPartyCount();
    u8 i;
    struct Pokemon * pokemon;
    for (i = 0; i < partyCount; i++)
    {
        pokemon = &gPlayerParty[i];
        if (GetMonData(pokemon, MON_DATA_SPECIES_OR_EGG, NULL) != SPECIES_EGG && GetMonData(pokemon, MON_DATA_SPECIES_OR_EGG, NULL) != SPECIES_NONE)
            return i;
    }
    return 0;
}

u16 GetPartyMonSpecies(void)
{
    return GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES_OR_EGG, NULL);
}

bool8 IsMonOTNameNotPlayers(void)
{
    GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_OT_NAME, gStringVar1);
    
    if (!StringCompare(gSaveBlock2Ptr->playerName, gStringVar1))
        return FALSE;
    else
        return TRUE;
}

// Used to nop all the unused specials from RS
void NullFieldSpecial(void)
{

}

void DoPicboxCancel(void)
{
    u8 t = EOS;
    AddTextPrinterParameterized(0, FONT_NORMAL, &t, 0, 1, 0, NULL);
    PicboxCancel();
}

void SetVermilionTrashCans(void)
{
    u16 idx = (Random() % 15) + 1;
    gSpecialVar_0x8004 = idx;
    gSpecialVar_0x8005 = idx;
    switch (gSpecialVar_0x8004)
    {
    case 1:
        idx = Random() % 2;
        if (idx == 0)
            gSpecialVar_0x8005 += 1;
        else
            gSpecialVar_0x8005 += 5;
        break;
    case 2:
    case 3:
    case 4:
        idx = Random() % 3;
        if (idx == 0)
            gSpecialVar_0x8005 += 1;
        else if (idx == 1)
            gSpecialVar_0x8005 += 5;
        else
            gSpecialVar_0x8005 -= 1;
        break;
    case 5:
        idx = Random() % 2;
        if (idx == 0)
            gSpecialVar_0x8005 += 5;
        else
            gSpecialVar_0x8005 -= 1;
        break;
    case 6:
        idx = Random() % 3;
        if (idx == 0)
            gSpecialVar_0x8005 -= 5;
        else if (idx == 1)
            gSpecialVar_0x8005 += 1;
        else
            gSpecialVar_0x8005 += 5;
        break;
    case 7:
    case 8:
    case 9:
        idx = Random() % 4;
        if (idx == 0)
            gSpecialVar_0x8005 -= 5;
        else if (idx == 1)
            gSpecialVar_0x8005 += 1;
        else if (idx == 2)
            gSpecialVar_0x8005 += 5;
        else
            gSpecialVar_0x8005 -= 1;
        break;
    case 10:
        idx = Random() % 3;
        if (idx == 0)
            gSpecialVar_0x8005 -= 5;
        else if (idx == 1)
            gSpecialVar_0x8005 += 5;
        else
            gSpecialVar_0x8005 -= 1;
        break;
    case 11:
        idx = Random() % 2;
        if (idx == 0)
            gSpecialVar_0x8005 -= 5;
        else
            gSpecialVar_0x8005 += 1;
        break;
    case 12:
    case 13:
    case 14:
        idx = Random() % 3;
        if (idx == 0)
            gSpecialVar_0x8005 -= 5;
        else if (idx == 1)
            gSpecialVar_0x8005 += 1;
        else
            gSpecialVar_0x8005 -= 1;
        break;
    case 15:
        idx = Random() % 2;
        if (idx == 0)
            gSpecialVar_0x8005 -= 5;
        else
            gSpecialVar_0x8005 -= 1;
        break;
    }
    if (gSpecialVar_0x8005 > 15)
    {
        if (gSpecialVar_0x8004 % 5 == 1)
            gSpecialVar_0x8005 = gSpecialVar_0x8004 + 1;
        else if (gSpecialVar_0x8004 % 5 == 0)
            gSpecialVar_0x8005 = gSpecialVar_0x8004 - 1;
        else
            gSpecialVar_0x8005 = gSpecialVar_0x8004 + 1;
    }
}

static const u16 sResortGorgeousDeluxeRewards[] = {
    ITEM_BIG_PEARL,
    ITEM_PEARL,
    ITEM_STARDUST,
    ITEM_STAR_PIECE,
    ITEM_NUGGET,
    ITEM_RARE_CANDY
};

void IncrementResortGorgeousStepCounter(void)
{
    u16 var4035 = VarGet(VAR_RESORT_GOREGEOUS_STEP_COUNTER);
    if (VarGet(VAR_RESORT_GORGEOUS_REQUESTED_MON) != SPECIES_NONE)
    {
        var4035++;
        if (var4035 >= 250)
        {
            VarSet(VAR_RESORT_GORGEOUS_REQUESTED_MON, 0xFFFF);
            VarSet(VAR_RESORT_GOREGEOUS_STEP_COUNTER, 0);
        }
        else
        {
            VarSet(VAR_RESORT_GOREGEOUS_STEP_COUNTER, var4035);
        }
    }
}

void SampleResortGorgeousMonAndReward(void)
{
    u16 requestedSpecies = VarGet(VAR_RESORT_GORGEOUS_REQUESTED_MON);
    if (requestedSpecies == SPECIES_NONE || requestedSpecies == 0xFFFF)
    {
        VarSet(VAR_RESORT_GORGEOUS_REQUESTED_MON, SampleResortGorgeousMon());
        VarSet(VAR_RESORT_GORGEOUS_REWARD, SampleResortGorgeousReward());
        VarSet(VAR_RESORT_GOREGEOUS_STEP_COUNTER, 0);
    }
    StringCopy(gStringVar1, gSpeciesNames[VarGet(VAR_RESORT_GORGEOUS_REQUESTED_MON)]);
}

static u16 SampleResortGorgeousMon(void)
{
    u16 i;
    u16 species;
    for (i = 0; i < 100; i++)
    {
        species = (Random() % (NUM_SPECIES - 1)) + 1;
        if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), 0) == TRUE)
            return species;
    }
    while (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), 0) != TRUE)
    {
        if (species == SPECIES_BULBASAUR)
            species = NUM_SPECIES - 1;
        else
            species--;
    }
    return species;
}

static u16 SampleResortGorgeousReward(void)
{
    if ((Random() % 100) >= 30)
        return ITEM_LUXURY_BALL;
    else
        return sResortGorgeousDeluxeRewards[Random() % NELEMS(sResortGorgeousDeluxeRewards)];
}

bool8 CheckAddCoins(void)
{
    if (gSpecialVar_Result + gSpecialVar_0x8006 > 9999)
        return FALSE;
    else
        return TRUE;
}

static const struct WindowTemplate sElevatorCurrentFloorWindowTemplate = {
    .bg = 0,
    .tilemapLeft = 22,
    .tilemapTop = 1,
    .width = 7,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x008
};

static const u8 *const sFloorNamePointers[] = {
    gText_B4F,
    gText_B3F,
    gText_B2F,
    gText_B1F,
    gText_1F,
    gText_2F,
    gText_3F,
    gText_4F,
    gText_5F,
    gText_6F,
    gText_7F,
    gText_8F,
    gText_9F,
    gText_10F,
    gText_11F,
    gText_Rooftop
};

// Unused
static const u8 sFloorNameWidthPadding[] = {
    26,
    26,
    26,
    26,
    30,
    30,
    30,
    30,
    30,
    30,
    30,
    30,
    30,
    26,
    26,
    18
};

static const u16 sElevatorWindowMetatilesGoingUp[][3] = {
    {
        METATILE_SilphCo_ElevatorWindow_Top0, 
        METATILE_SilphCo_ElevatorWindow_Top1, 
        METATILE_SilphCo_ElevatorWindow_Top2
    },
    {
        METATILE_SilphCo_ElevatorWindow_Mid0, 
        METATILE_SilphCo_ElevatorWindow_Mid1, 
        METATILE_SilphCo_ElevatorWindow_Mid2
    },
    {
        METATILE_SilphCo_ElevatorWindow_Bottom0, 
        METATILE_SilphCo_ElevatorWindow_Bottom1, 
        METATILE_SilphCo_ElevatorWindow_Bottom2
    }
};

static const u16 sElevatorWindowMetatilesGoingDown[][3] = {
    {
        METATILE_SilphCo_ElevatorWindow_Top0, 
        METATILE_SilphCo_ElevatorWindow_Top2, 
        METATILE_SilphCo_ElevatorWindow_Top1
    },
    {
        METATILE_SilphCo_ElevatorWindow_Mid0, 
        METATILE_SilphCo_ElevatorWindow_Mid2, 
        METATILE_SilphCo_ElevatorWindow_Mid1
    },
    {
        METATILE_SilphCo_ElevatorWindow_Bottom0, 
        METATILE_SilphCo_ElevatorWindow_Bottom2, 
        METATILE_SilphCo_ElevatorWindow_Bottom1
    }
};

static const u8 sElevatorAnimationDuration[] = {
    8,
    16,
    24,
    32,
    38,
    46,
    53,
    56,
    57
};

static const u8 sElevatorWindowAnimDuration[] = {
    3,
    6,
    9,
    12,
    15,
    18,
    21,
    24,
    27
};

void GetElevatorFloor(void)
{
    u16 floor = 4;
    if (gSaveBlock1Ptr->dynamicWarp.mapGroup == MAP_GROUP(MAP_ROCKET_HIDEOUT_B1F))
    {
        switch (gSaveBlock1Ptr->dynamicWarp.mapNum)
        {
        case MAP_NUM(MAP_SILPH_CO_1F):
            floor = 4;
            break;
        case MAP_NUM(MAP_SILPH_CO_2F):
            floor = 5;
            break;
        case MAP_NUM(MAP_SILPH_CO_3F):
            floor = 6;
            break;
        case MAP_NUM(MAP_SILPH_CO_4F):
            floor = 7;
            break;
        case MAP_NUM(MAP_SILPH_CO_5F):
            floor = 8;
            break;
        case MAP_NUM(MAP_SILPH_CO_6F):
            floor = 9;
            break;
        case MAP_NUM(MAP_SILPH_CO_7F):
            floor = 10;
            break;
        case MAP_NUM(MAP_SILPH_CO_8F):
            floor = 11;
            break;
        case MAP_NUM(MAP_SILPH_CO_9F):
            floor = 12;
            break;
        case MAP_NUM(MAP_SILPH_CO_10F):
            floor = 13;
            break;
        case MAP_NUM(MAP_SILPH_CO_11F):
            floor = 14;
            break;
        case MAP_NUM(MAP_ROCKET_HIDEOUT_B1F):
            floor = 3;
            break;
        case MAP_NUM(MAP_ROCKET_HIDEOUT_B2F):
            floor = 2;
            break;
        case MAP_NUM(MAP_ROCKET_HIDEOUT_B4F):
            floor = 0;
            break;
        }
    }
    if (gSaveBlock1Ptr->dynamicWarp.mapGroup == MAP_GROUP(MAP_CELADON_CITY_DEPARTMENT_STORE_1F))
    {
        switch (gSaveBlock1Ptr->dynamicWarp.mapNum)
        {
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_1F):
            floor = 4;
            break;
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_2F):
            floor = 5;
            break;
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_3F):
            floor = 6;
            break;
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_4F):
            floor = 7;
            break;
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_5F):
            floor = 8;
            break;
        }
    }
    if (gSaveBlock1Ptr->dynamicWarp.mapGroup == MAP_GROUP(MAP_TRAINER_TOWER_1F))
    {
        switch (gSaveBlock1Ptr->dynamicWarp.mapNum)
        {
        case MAP_NUM(MAP_TRAINER_TOWER_1F):
        case MAP_NUM(MAP_TRAINER_TOWER_2F):
        case MAP_NUM(MAP_TRAINER_TOWER_3F):
        case MAP_NUM(MAP_TRAINER_TOWER_4F):
        case MAP_NUM(MAP_TRAINER_TOWER_5F):
        case MAP_NUM(MAP_TRAINER_TOWER_6F):
        case MAP_NUM(MAP_TRAINER_TOWER_7F):
        case MAP_NUM(MAP_TRAINER_TOWER_8F):
        case MAP_NUM(MAP_TRAINER_TOWER_ROOF):
            floor = 15;
            break;
        case MAP_NUM(MAP_TRAINER_TOWER_LOBBY):
            floor = 3;
            break;
        }
    }
    VarSet(VAR_ELEVATOR_FLOOR, floor);
}

u16 InitElevatorFloorSelectMenuPos(void)
{
    sElevatorScroll = 0;
    sElevatorCursorPos = 0;

    if (gSaveBlock1Ptr->dynamicWarp.mapGroup == MAP_GROUP(MAP_ROCKET_HIDEOUT_B1F))
    {
        switch (gSaveBlock1Ptr->dynamicWarp.mapNum)
        {
        case MAP_NUM(MAP_SILPH_CO_11F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 0;
            break;
        case MAP_NUM(MAP_SILPH_CO_10F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 1;
            break;
        case MAP_NUM(MAP_SILPH_CO_9F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 2;
            break;
        case MAP_NUM(MAP_SILPH_CO_8F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 3;
            break;
        case MAP_NUM(MAP_SILPH_CO_7F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 4;
            break;
        case MAP_NUM(MAP_SILPH_CO_6F):
            sElevatorScroll = 1;
            sElevatorCursorPos = 4;
            break;
        case MAP_NUM(MAP_SILPH_CO_5F):
            sElevatorScroll = 2;
            sElevatorCursorPos = 4;
            break;
        case MAP_NUM(MAP_SILPH_CO_4F):
            sElevatorScroll = 3;
            sElevatorCursorPos = 4;
            break;
        case MAP_NUM(MAP_SILPH_CO_3F):
            sElevatorScroll = 4;
            sElevatorCursorPos = 4;
            break;
        case MAP_NUM(MAP_SILPH_CO_2F):
            sElevatorScroll = 5;
            sElevatorCursorPos = 4;
            break;
        case MAP_NUM(MAP_SILPH_CO_1F):
            sElevatorScroll = 5;
            sElevatorCursorPos = 5;
            break;
        case MAP_NUM(MAP_ROCKET_HIDEOUT_B1F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 0;
            break;
        case MAP_NUM(MAP_ROCKET_HIDEOUT_B2F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 1;
            break;
        case MAP_NUM(MAP_ROCKET_HIDEOUT_B4F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 2;
            break;
        }
    }
    if (gSaveBlock1Ptr->dynamicWarp.mapGroup == MAP_GROUP(MAP_CELADON_CITY_DEPARTMENT_STORE_1F))
    {
        switch (gSaveBlock1Ptr->dynamicWarp.mapNum)
        {
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_5F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 0;
            break;
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_4F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 1;
            break;
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_3F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 2;
            break;
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_2F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 3;
            break;
        case MAP_NUM(MAP_CELADON_CITY_DEPARTMENT_STORE_1F):
            sElevatorScroll = 0;
            sElevatorCursorPos = 4;
            break;
        }
    }
    if (gSaveBlock1Ptr->dynamicWarp.mapGroup == MAP_GROUP(MAP_TRAINER_TOWER_1F))
    {
        switch (gSaveBlock1Ptr->dynamicWarp.mapNum)
        {
        case MAP_NUM(MAP_TRAINER_TOWER_1F):
        case MAP_NUM(MAP_TRAINER_TOWER_2F):
        case MAP_NUM(MAP_TRAINER_TOWER_3F):
        case MAP_NUM(MAP_TRAINER_TOWER_4F):
        case MAP_NUM(MAP_TRAINER_TOWER_5F):
        case MAP_NUM(MAP_TRAINER_TOWER_6F):
        case MAP_NUM(MAP_TRAINER_TOWER_7F):
        case MAP_NUM(MAP_TRAINER_TOWER_8F):
        case MAP_NUM(MAP_TRAINER_TOWER_ROOF):
            sElevatorScroll = 0;
            sElevatorCursorPos = 0;
            break;
        case MAP_NUM(MAP_TRAINER_TOWER_LOBBY):
            sElevatorScroll = 0;
            sElevatorCursorPos = 1;
            break;
        }
    }
    return sElevatorCursorPos;
}

void AnimateElevator(void)
{
    u16 nfloors;
    s16 *data = gTasks[CreateTask(Task_ElevatorShake, 9)].data;
    data[1] = 0;
    data[2] = 0;
    data[4] = 1;
    if (gSpecialVar_0x8005 > gSpecialVar_0x8006)
    {
        nfloors = gSpecialVar_0x8005 - gSpecialVar_0x8006;
        data[6] = 1;
    }
    else
    {
        nfloors = gSpecialVar_0x8006 - gSpecialVar_0x8005;
        data[6] = 0;
    }
    if (nfloors > 8)
        nfloors = 8;
    data[5] = sElevatorAnimationDuration[nfloors];
    SetCameraPanningCallback(NULL);
    AnimateElevatorWindowView(nfloors, data[6]);
    PlaySE(SE_ELEVATOR);
}

static void Task_ElevatorShake(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    data[1]++;
    if ((data[1] % 3) == 0)
    {
        data[1] = 0;
        data[2]++;
        data[4] = -data[4];
        SetCameraPanning(0, data[4]);
        if (data[2] == data[5])
        {
            PlaySE(SE_DING_DONG);
            DestroyTask(taskId);
            ScriptContext_Enable();
            InstallCameraPanAheadCallback();
        }
    }
}

void DrawElevatorCurrentFloorWindow(void)
{
    const u8 *floorname;
    u32 strwidth;

    if (QL_AvoidDisplay(QL_DestroyAbortedDisplay) == TRUE)
        return;

    sElevatorCurrentFloorWindowId = AddWindow(&sElevatorCurrentFloorWindowTemplate);
    LoadStdWindowGfx(sElevatorCurrentFloorWindowId, 0x21D, BG_PLTT_ID(13));
    DrawStdFrameWithCustomTileAndPalette(sElevatorCurrentFloorWindowId, FALSE, 0x21D, 13);
    AddTextPrinterParameterized(sElevatorCurrentFloorWindowId, FONT_NORMAL, gText_NowOn, 0, 2, 0xFF, NULL);
    floorname = sFloorNamePointers[gSpecialVar_0x8005];
    strwidth = GetStringWidth(FONT_NORMAL, floorname, 0);
    AddTextPrinterParameterized(sElevatorCurrentFloorWindowId, FONT_NORMAL, floorname, 56 - strwidth, 16, 0xFF, NULL);
    PutWindowTilemap(sElevatorCurrentFloorWindowId);
    CopyWindowToVram(sElevatorCurrentFloorWindowId, COPYWIN_FULL);
}

void CloseElevatorCurrentFloorWindow(void)
{
    ClearStdWindowAndFrameToTransparent(sElevatorCurrentFloorWindowId, TRUE);
    RemoveWindow(sElevatorCurrentFloorWindowId);
}

static void AnimateElevatorWindowView(u16 nfloors, u8 direction)
{
    u8 taskId;
    if (FuncIsActiveTask(Task_AnimateElevatorWindowView) != TRUE)
    {
        taskId = CreateTask(Task_AnimateElevatorWindowView, 8);
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].data[2] = direction;
        gTasks[taskId].data[3] = sElevatorWindowAnimDuration[nfloors];
    }
}

static void Task_AnimateElevatorWindowView(u8 taskId)
{
    u8 i;
    u8 j;
    s16 *data = gTasks[taskId].data;
    if (data[1] == 6)
    {
        data[0]++;
        if (data[2] == 0)
        {
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                    MapGridSetMetatileIdAt(j + 1 + MAP_OFFSET, i + MAP_OFFSET, sElevatorWindowMetatilesGoingUp[i][data[0] % 3] | MAPGRID_COLLISION_MASK);
            }
        }
        else
        {
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                    MapGridSetMetatileIdAt(j + 1 + MAP_OFFSET, i + MAP_OFFSET, sElevatorWindowMetatilesGoingDown[i][data[0] % 3] | MAPGRID_COLLISION_MASK);
            }
        }
        DrawWholeMapView();
        data[1] = 0;
        if (data[0] == data[3])
            DestroyTask(taskId);
    }
    data[1]++;
}

void ListMenu(void)
{
    u8 taskId;
    struct Task *task;

    if (QL_AvoidDisplay(QL_DestroyAbortedDisplay) == TRUE)
        return;
        
    taskId = CreateTask(Task_CreateScriptListMenu, 8);
    task = &gTasks[taskId];
    switch (gSpecialVar_0x8004)
    {
    case LISTMENU_BADGES:
        task->data[0] = 4;
        task->data[1] = 9;
        task->data[2] = 1;
        task->data[3] = 1;
        task->data[4] = 12;
        task->data[5] = 7;
        task->data[6] = 1;
        task->data[15] = taskId;
        break;
    case LISTMENU_SILPHCO_FLOORS:
        task->data[0] = 7;
        task->data[1] = 12;
        task->data[2] = 1;
        task->data[3] = 1;
        task->data[4] = 8;
        task->data[5] = 12;
        task->data[6] = 0;
        task->data[15] = taskId;
        task->data[7] = sElevatorScroll;
        task->data[8] = sElevatorCursorPos;
        break;
    case LISTMENU_ROCKET_HIDEOUT_FLOORS: // Multichoice used instead
        task->data[0] = 4;
        task->data[1] = 4;
        task->data[2] = 1;
        task->data[3] = 1;
        task->data[4] = 8;
        task->data[5] = 8;
        task->data[6] = 0;
        task->data[15] = taskId;
        break;
    case LISTMENU_DEPT_STORE_FLOORS: // Multichoice used instead
        task->data[0] = 4;
        task->data[1] = 6;
        task->data[2] = 1;
        task->data[3] = 1;
        task->data[4] = 8;
        task->data[5] = 8;
        task->data[6] = 0;
        task->data[15] = taskId;
        break;
    case LISTMENU_WIRELESS_LECTURE_HEADERS: // Multichoice used instead
        task->data[0] = 4;
        task->data[1] = 4;
        task->data[2] = 1;
        task->data[3] = 1;
        task->data[4] = 17;
        task->data[5] = 8;
        task->data[6] = 1;
        task->data[15] = taskId;
        break;
    case LISTMENU_BERRY_POWDER:
        task->data[0] = 7;
        task->data[1] = 12;
        task->data[2] = 16;
        task->data[3] = 1;
        task->data[4] = 17;
        task->data[5] = 12;
        task->data[6] = 0;
        task->data[15] = taskId;
        break;
    case LISTMENU_TRAINER_TOWER_FLOORS: // Mulitchoice used instead
        task->data[0] = 3;
        task->data[1] = 3;
        task->data[2] = 1;
        task->data[3] = 1;
        task->data[4] = 8;
        task->data[5] = 6;
        task->data[6] = 0;
        task->data[15] = taskId;
        break;
    case 99:
        break;
    default:
        gSpecialVar_Result = 0x7F;
        DestroyTask(taskId);
        break;
    }
}

static const u8 *const sListMenuLabels[][12] = {
    [LISTMENU_BADGES] = 
    {
        gText_BoulderBadge,
        gText_CascadeBadge,
        gText_ThunderBadge,
        gText_RainbowBadge,
        gText_SoulBadge,
        gText_MarshBadge,
        gText_VolcanoBadge,
        gText_EarthBadge,
        gOtherText_Exit,
    }, 
    [LISTMENU_SILPHCO_FLOORS] = 
    {
        gText_11F,
        gText_10F,
        gText_9F,
        gText_8F,
        gText_7F,
        gText_6F,
        gText_5F,
        gText_4F,
        gText_3F,
        gText_2F,
        gText_1F,
        gOtherText_Exit,
    }, 
    [LISTMENU_ROCKET_HIDEOUT_FLOORS] = // Unncessary, MULTICHOICE_ROCKET_HIDEOUT_ELEVATOR is used instead
    {
        gText_B1F,
        gText_B2F,
        gText_B4F,
        gOtherText_Exit,
    }, 
    [LISTMENU_DEPT_STORE_FLOORS] = // Unncessary, MULTICHOICE_DEPT_STORE_ELEVATOR is used instead
    {
        gText_5F,
        gText_4F,
        gText_3F,
        gText_2F,
        gText_1F,
        gOtherText_Exit,
    }, 
    [LISTMENU_WIRELESS_LECTURE_HEADERS] = // Unnecessary, MULTICHOICE_LINKED_DIRECT_UNION is used instead
    {
        gText_LinkedGamePlay,
        gText_DirectCorner,
        gText_UnionRoom,
        gOtherText_Quit,
    }, 
    [LISTMENU_BERRY_POWDER] = 
    {
        gText_Energypowder_50,
        gText_EnergyRoot_80,
        gText_HealPowder_50,
        gText_RevivalHerb_300,
        gText_Protein_1000,
        gText_Iron_1000,
        gText_Carbos_1000,
        gText_Calcium_1000,
        gText_Zinc_1000,
        gText_HpUp_1000,
        gText_PpUp_3000,
        gOtherText_Exit,
    }, 
    [LISTMENU_TRAINER_TOWER_FLOORS] = // Unnecessary, MULTICHOICE_ROOFTOP_B1F is used instead
    {
        gText_Rooftop,
        gText_B1F,
        gOtherText_Exit,
    }
};

static void Task_CreateScriptListMenu(u8 taskId)
{
    struct WindowTemplate template;
    u8 i;
    s32 width;
    s32 mwidth;
    struct Task *task = &gTasks[taskId];
    u8 windowId;
    LockPlayerFieldControls();
    if (gSpecialVar_0x8004 == LISTMENU_SILPHCO_FLOORS)
        sListMenuLastScrollPosition = sElevatorScroll;
    else
        sListMenuLastScrollPosition = 0;
    sListMenuItems = AllocZeroed(task->data[1] * sizeof(struct ListMenuItem));
    CreateScriptListMenu();
    mwidth = 0;
    for (i = 0; i < task->data[1]; i++)
    {
        sListMenuItems[i].label = sListMenuLabels[gSpecialVar_0x8004][i];
        sListMenuItems[i].index = i;
        width = GetStringWidth(FONT_NORMAL, sListMenuItems[i].label, 0);
        if (width > mwidth)
            mwidth = width;
    }
    task->data[4] = (mwidth + 9) / 8 + 1;
    if (task->data[2] + task->data[4] > 29)
        task->data[2] = 29 - task->data[4];
    template = SetWindowTemplateFields(0, task->data[2], task->data[3], task->data[4], task->data[5], 15, 0x038);
    task->data[13] = windowId = AddWindow(&template);
    SetStdWindowBorderStyle(task->data[13], 0);
    sFieldSpecialsListMenuTemplate.totalItems = task->data[1];
    sFieldSpecialsListMenuTemplate.maxShowed = task->data[0];
    sFieldSpecialsListMenuTemplate.windowId = task->data[13];
    Task_CreateMenuRemoveScrollIndicatorArrowPair(taskId);
    task->data[14] = ListMenuInit(&sFieldSpecialsListMenuTemplate, task->data[7], task->data[8]);
    PutWindowTilemap(task->data[13]);
    CopyWindowToVram(task->data[13], COPYWIN_FULL);
    gTasks[taskId].func = Task_ListMenuHandleInput;
}

static void CreateScriptListMenu(void)
{
    sFieldSpecialsListMenuTemplate.items = sListMenuItems;
    sFieldSpecialsListMenuTemplate.moveCursorFunc = ScriptListMenuMoveCursorFunction;
    sFieldSpecialsListMenuTemplate.itemPrintFunc = NULL;
    sFieldSpecialsListMenuTemplate.totalItems = 1;
    sFieldSpecialsListMenuTemplate.maxShowed = 1;
    sFieldSpecialsListMenuTemplate.windowId = 0;
    sFieldSpecialsListMenuTemplate.header_X = 0;
    sFieldSpecialsListMenuTemplate.item_X = 8;
    sFieldSpecialsListMenuTemplate.cursor_X = 0;
    sFieldSpecialsListMenuTemplate.upText_Y = 0;
    sFieldSpecialsListMenuTemplate.cursorPal = 2;
    sFieldSpecialsListMenuTemplate.fillValue = 1;
    sFieldSpecialsListMenuTemplate.cursorShadowPal = 3;
    sFieldSpecialsListMenuTemplate.lettersSpacing = 1;
    sFieldSpecialsListMenuTemplate.itemVerticalPadding = 0;
    sFieldSpecialsListMenuTemplate.scrollMultiple = 0;
    sFieldSpecialsListMenuTemplate.fontId = FONT_NORMAL;
    sFieldSpecialsListMenuTemplate.cursorKind = 0;
}

static void ScriptListMenuMoveCursorFunction(s32 nothing, bool8 is, struct ListMenu * used)
{
    u8 taskId;
    struct Task *task;
    PlaySE(SE_SELECT);
    taskId = FindTaskIdByFunc(Task_ListMenuHandleInput);
    if (taskId != 0xFF)
    {
        task = &gTasks[taskId];
        ListMenuGetScrollAndRow(task->data[14], &sFieldSpecialsListMenuScrollBuffer, NULL);
        sListMenuLastScrollPosition = sFieldSpecialsListMenuScrollBuffer;
    }
}

static void Task_ListMenuHandleInput(u8 taskId)
{
    s32 input;
    struct Task *task;

    task = &gTasks[taskId];
    task++;task--;
    input = ListMenu_ProcessInput(task->data[14]);
    switch (input)
    {
    case -1:
        break;
    case -2:
        gSpecialVar_Result = 0x7F;
        PlaySE(SE_SELECT);
        Task_DestroyListMenu(taskId);
        break;
    default:
        gSpecialVar_Result = input;
        PlaySE(SE_SELECT);
        if (task->data[6] == 0 || input == task->data[1] - 1)
        {
            Task_DestroyListMenu(taskId);
        }
        else
        {
            Task_ListMenuRemoveScrollIndicatorArrowPair(taskId);
            task->func = Task_SuspendListMenu;
            ScriptContext_Enable();
        }
        break;
    }
}

static void Task_DestroyListMenu(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    Task_ListMenuRemoveScrollIndicatorArrowPair(taskId);
    DestroyListMenuTask(task->data[14], NULL, NULL);
    Free(sListMenuItems);
    ClearStdWindowAndFrameToTransparent(task->data[13], TRUE);
    FillWindowPixelBuffer(task->data[13], PIXEL_FILL(0));
    ClearWindowTilemap(task->data[13]);
    CopyWindowToVram(task->data[13], COPYWIN_GFX);
    RemoveWindow(task->data[13]);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_SuspendListMenu(u8 taskId)
{
    switch (gTasks[taskId].data[6])
    {
    case 1:
        break;
    case 2:
        gTasks[taskId].data[6] = 1;
        gTasks[taskId].func = Task_RedrawScrollArrowsAndWaitInput;
        break;
    }
}

void ReturnToListMenu(void)
{
    u8 taskId = FindTaskIdByFunc(Task_SuspendListMenu);
    if (taskId == 0xFF)
        ScriptContext_Enable();
    else
        gTasks[taskId].data[6]++;
}

static void Task_RedrawScrollArrowsAndWaitInput(u8 taskId)
{
    LockPlayerFieldControls();
    Task_CreateMenuRemoveScrollIndicatorArrowPair(taskId);
    gTasks[taskId].func = Task_ListMenuHandleInput;
}

static void Task_CreateMenuRemoveScrollIndicatorArrowPair(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    struct ScrollArrowsTemplate template = {
        .firstArrowType = 2,
        .secondArrowType = 3,
        .tileTag = 2000,
        .palTag = 100
    };
    if (task->data[0] != task->data[1])
    {
        template.firstX = 4 * task->data[4] + 8 * task->data[2];
        template.firstY = 8;
        template.secondX = 4 * task->data[4] + 8 * task->data[2];
        template.secondY = 8 * task->data[5] + 10;
        template.fullyUpThreshold = 0;
        template.fullyDownThreshold = task->data[1] - task->data[0];
        task->data[12] = AddScrollIndicatorArrowPair(&template, &sListMenuLastScrollPosition);
    }
}

static void Task_ListMenuRemoveScrollIndicatorArrowPair(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    if (task->data[0] != task->data[1])
        RemoveScrollIndicatorArrowPair(task->data[12]);
}

void ForcePlayerToStartSurfing(void)
{
    SetHelpContext(HELPCONTEXT_SURFING);
    SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_SURFING);
}

static const u16 sStarterSpecies[] = {
    SPECIES_BULBASAUR,
    SPECIES_SQUIRTLE,
    SPECIES_CHARMANDER
};

static u16 GetStarterSpeciesById(u16 idx)
{
    if (idx >= NELEMS(sStarterSpecies))
        idx = 0;
    return sStarterSpecies[idx];
}

u16 GetStarterSpecies(void)
{
    return GetStarterSpeciesById(VarGet(VAR_STARTER_MON));
}

void SetSeenMon(void)
{
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(gSpecialVar_0x8004), 2);
}

void ResetContextNpcTextColor(void)
{
    gSelectedObjectEvent = 0;
    gSpecialVar_TextColor = NPC_TEXT_COLOR_DEFAULT;
}

u8 ContextNpcGetTextColor(void)
{
    u8 gfxId;
    if (gSpecialVar_TextColor != NPC_TEXT_COLOR_DEFAULT)
    {
        // A text color has been specified, use that
        return gSpecialVar_TextColor;
    }
    else if (gSelectedObjectEvent == 0)
    {
        // No text color specified and no object selected, use neutral
        return NPC_TEXT_COLOR_NEUTRAL;
    }
    else
    {
        // An object is selected and no color has been specified.
        // Use the text color normally associated with this object's sprite.
        gfxId = gObjectEvents[gSelectedObjectEvent].graphicsId;
        if (gfxId >= OBJ_EVENT_GFX_VAR_0)
            gfxId = VarGetObjectEventGraphicsId(gfxId - OBJ_EVENT_GFX_VAR_0);
        return GetColorFromTextColorTable(gfxId);
    }
}

static bool8 HasMonBeenRenamed(u8 idx)
{
    struct Pokemon * pokemon = &gPlayerParty[idx];
    u8 language;
    GetMonData(pokemon, MON_DATA_NICKNAME, gStringVar1);
    language = GetMonData(pokemon, MON_DATA_LANGUAGE, &language);
    if (language != LANGUAGE_ENGLISH)
        return TRUE;
    else if (StringCompare(gSpeciesNames[GetMonData(pokemon, MON_DATA_SPECIES, NULL)], gStringVar1) != 0)
        return TRUE;
    else
        return FALSE;
}

bool8 HasLeadMonBeenRenamed(void)
{
    return HasMonBeenRenamed(GetLeadMonIndex());
}

void TV_PrintIntToStringVar(u8 varidx, s32 number)
{
    s32 n = CountDigits(number);
    ConvertIntToDecimalStringN(sStringVarPtrs[varidx], number, STR_CONV_MODE_LEFT_ALIGN, n);
}

s32 CountDigits(s32 number)
{
    if (number / 10 == 0)
        return 1;
    else if (number / 100 == 0)
        return 2;
    else if (number / 1000 == 0)
        return 3;
    else if (number / 10000 == 0)
        return 4;
    else if (number / 100000 == 0)
        return 5;
    else if (number / 1000000 == 0)
        return 6;
    else if (number / 10000000 == 0)
        return 7;
    else if (number / 100000000 == 0)
        return 8;
    else
        return 1;
}

bool8 NameRaterWasNicknameChanged(void)
{
    struct Pokemon * pokemon = &gPlayerParty[gSpecialVar_0x8004];
    GetMonData(pokemon, MON_DATA_NICKNAME, gStringVar1);
    if (StringCompare(gStringVar3, gStringVar1) == 0)
        return FALSE;
    else
        return TRUE;
}

void ChangeBoxPokemonNickname(void)
{
    struct BoxPokemon * pokemon = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    u16 species;
    u8 gender;
    u32 personality;


    GetBoxMonData(pokemon, MON_DATA_NICKNAME, gStringVar3);
    GetBoxMonData(pokemon, MON_DATA_NICKNAME, gStringVar2);
    species = GetBoxMonData(pokemon, MON_DATA_SPECIES, NULL);
    gender = GetBoxMonGender(pokemon);
    personality = GetBoxMonData(pokemon, MON_DATA_PERSONALITY, NULL);
    DoNamingScreen(NAMING_SCREEN_NICKNAME, gStringVar2, species, gender, personality, ChangeBoxPokemonNickname_CB);
}

static void ChangeBoxPokemonNickname_CB(void)
{
    SetBoxMonNickAt(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos, gStringVar2);
    CB2_ReturnToFieldContinueScriptPlayMapMusic();
}

void ChangePokemonNickname(void)
{
    u16 species;
    u8 gender;
    u32 personality;

    GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_NICKNAME, gStringVar3);
    GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_NICKNAME, gStringVar2);
    species = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES, NULL);
    gender = GetMonGender(&gPlayerParty[gSpecialVar_0x8004]);
    personality = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_PERSONALITY, NULL);
    DoNamingScreen(NAMING_SCREEN_NICKNAME, gStringVar2, species, gender, personality, ChangePokemonNickname_CB);
}

static void ChangePokemonNickname_CB(void)
{
    SetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_NICKNAME, gStringVar2);
    CB2_ReturnToFieldContinueScriptPlayMapMusic();
}

void BufferMonNickname(void)
{
    GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_NICKNAME, gStringVar1);
    StringGet_Nickname(gStringVar1);
}

void IsMonOTIDNotPlayers(void)
{
    if (GetPlayerTrainerId() == GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_OT_ID, NULL))
        gSpecialVar_Result = FALSE;
    else
        gSpecialVar_Result = TRUE;
}

u32 GetPlayerTrainerId(void)
{
    return (gSaveBlock2Ptr->playerTrainerId[3] << 24) | (gSaveBlock2Ptr->playerTrainerId[2] << 16) | (gSaveBlock2Ptr->playerTrainerId[1] << 8) | gSaveBlock2Ptr->playerTrainerId[0];
}

u8 GetUnlockedSeviiAreas(void)
{
    u8 result = 0;
    if (FlagGet(FLAG_WORLD_MAP_ONE_ISLAND) == TRUE)
        result |= 1 << 0;
    if (FlagGet(FLAG_WORLD_MAP_TWO_ISLAND) == TRUE)
        result |= 1 << 1;
    if (FlagGet(FLAG_WORLD_MAP_THREE_ISLAND) == TRUE)
        result |= 1 << 2;
    if (FlagGet(FLAG_WORLD_MAP_FOUR_ISLAND) == TRUE)
        result |= 1 << 3;
    if (FlagGet(FLAG_WORLD_MAP_FIVE_ISLAND) == TRUE)
        result |= 1 << 4;
    if (FlagGet(FLAG_WORLD_MAP_SIX_ISLAND) == TRUE)
        result |= 1 << 5;
    if (FlagGet(FLAG_WORLD_MAP_SEVEN_ISLAND) == TRUE)
        result |= 1 << 6;
    return result;
}

void UpdateTrainerCardPhotoIcons(void)
{
    u16 species[PARTY_SIZE];
    u32 personality[PARTY_SIZE];
    u8 i;
    u8 partyCount;
    for (i = 0; i < PARTY_SIZE; i++)
        species[i] = SPECIES_NONE;
    partyCount = CalculatePlayerPartyCount();
    for (i = 0; i < partyCount; i++)
    {
        species[i] = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL);
        personality[i] = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY, NULL);
    }
    VarSet(VAR_TRAINER_CARD_MON_ICON_1, SpeciesToMailSpecies(species[0], personality[0]));
    VarSet(VAR_TRAINER_CARD_MON_ICON_2, SpeciesToMailSpecies(species[1], personality[1]));
    VarSet(VAR_TRAINER_CARD_MON_ICON_3, SpeciesToMailSpecies(species[2], personality[2]));
    VarSet(VAR_TRAINER_CARD_MON_ICON_4, SpeciesToMailSpecies(species[3], personality[3]));
    VarSet(VAR_TRAINER_CARD_MON_ICON_5, SpeciesToMailSpecies(species[4], personality[4]));
    VarSet(VAR_TRAINER_CARD_MON_ICON_6, SpeciesToMailSpecies(species[5], personality[5]));
    VarSet(VAR_TRAINER_CARD_MON_ICON_TINT_IDX, gSpecialVar_0x8004);
}

u16 StickerManGetBragFlags(void)
{
    u16 result = 0;
    u32 numEggs;
    gSpecialVar_0x8004 = GetGameStat(GAME_STAT_ENTERED_HOF);
    numEggs = GetGameStat(GAME_STAT_HATCHED_EGGS);
    gSpecialVar_0x8006 = GetGameStat(GAME_STAT_LINK_BATTLE_WINS);
    if (numEggs > 0xFFFF)
        gSpecialVar_0x8005 = 0xFFFF;
    else
        gSpecialVar_0x8005 = numEggs;
    if (gSpecialVar_0x8004 != 0)
        result |= 1 << 0;
    if (gSpecialVar_0x8005 != 0)
        result |= 1 << 1;
    if (gSpecialVar_0x8006 != 0)
        result |= 1 << 2;
    return result;
}

u16 GetHiddenItemAttr(u32 hiddenItem, u8 attr)
{
    if (attr == HIDDEN_ITEM_ITEM)
        return GET_HIDDEN_ITEM_ITEM(hiddenItem);
    else if (attr == HIDDEN_ITEM_FLAG)
        return GET_HIDDEN_ITEM_FLAG(hiddenItem) + FLAG_HIDDEN_ITEMS_START;
    else if (attr == HIDDEN_ITEM_QUANTITY)
        return GET_HIDDEN_ITEM_QUANTITY(hiddenItem);
    else if (attr == HIDDEN_ITEM_UNDERFOOT)
        return GET_HIDDEN_ITEM_UNDERFOOT(hiddenItem);
    else // Invalid
        return 1;
}

bool8 DoesPlayerPartyContainSpecies(void)
{
    u8 partyCount = CalculatePlayerPartyCount();
    u8 i;
    for (i = 0; i < partyCount; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL) == gSpecialVar_0x8004)
            return TRUE;
    }
    return FALSE;
}

static const u8 sMartMaps[][3] = {
    {MAP(MAP_VIRIDIAN_CITY_MART),   1},
    {MAP(MAP_PEWTER_CITY_MART),     3},
    {MAP(MAP_CERULEAN_CITY_MART),   1},
    {MAP(MAP_LAVENDER_TOWN_MART),   1},
    {MAP(MAP_VERMILION_CITY_MART),  1},
    {MAP(MAP_FUCHSIA_CITY_MART),    1},
    {MAP(MAP_CINNABAR_ISLAND_MART), 1},
    {MAP(MAP_SAFFRON_CITY_MART),    1},
    {MAP(MAP_THREE_ISLAND_MART),    1},
    {MAP(MAP_FOUR_ISLAND_MART),     1},
    {MAP(MAP_SEVEN_ISLAND_MART),    1},
    {MAP(MAP_SIX_ISLAND_MART),      1}
};

u8 GetMartClerkObjectId(void)
{
    u8 i;
    for (i = 0; i < NELEMS(sMartMaps); i++)
    {
        if (gSaveBlock1Ptr->location.mapGroup == sMartMaps[i][0] && gSaveBlock1Ptr->location.mapNum == sMartMaps[i][1])
            return sMartMaps[i][2];
    }
    return 1;
}

void SetUsedPkmnCenterQuestLogEvent(void)
{
    SetQuestLogEvent(QL_EVENT_USED_PKMN_CENTER, NULL);
}

static const struct {
    u16 inside_grp;
    u16 inside_num;
    u16 outside_grp;
    u16 outside_num;
} sInsideOutsidePairs[] = {
    [QL_LOCATION_HOME]               = {MAP(MAP_PALLET_TOWN_PLAYERS_HOUSE_1F),          MAP(MAP_PALLET_TOWN)},
    [QL_LOCATION_OAKS_LAB]           = {MAP(MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB),        MAP(MAP_PALLET_TOWN)},
    [QL_LOCATION_VIRIDIAN_GYM]       = {MAP(MAP_VIRIDIAN_CITY_GYM),                     MAP(MAP_VIRIDIAN_CITY)},
    [QL_LOCATION_LEAGUE_GATE_1]      = {MAP(MAP_ROUTE22_NORTH_ENTRANCE),                MAP(MAP_ROUTE22)},
    [QL_LOCATION_LEAGUE_GATE_2]      = {MAP(MAP_ROUTE22_NORTH_ENTRANCE),                MAP(MAP_ROUTE23)},
    [QL_LOCATION_VIRIDIAN_FOREST_1]  = {MAP(MAP_VIRIDIAN_FOREST),                       MAP(MAP_ROUTE2_VIRIDIAN_FOREST_SOUTH_ENTRANCE)},
    [QL_LOCATION_VIRIDIAN_FOREST_2]  = {MAP(MAP_VIRIDIAN_FOREST),                       MAP(MAP_ROUTE2_VIRIDIAN_FOREST_NORTH_ENTRANCE)},
    [QL_LOCATION_PEWTER_MUSEUM]      = {MAP(MAP_PEWTER_CITY_MUSEUM_1F),                 MAP(MAP_PEWTER_CITY)},
    [QL_LOCATION_PEWTER_GYM]         = {MAP(MAP_PEWTER_CITY_GYM),                       MAP(MAP_PEWTER_CITY)},
    [QL_LOCATION_MT_MOON_1]          = {MAP(MAP_MT_MOON_1F),                            MAP(MAP_ROUTE4)},
    [QL_LOCATION_MT_MOON_2]          = {MAP(MAP_MT_MOON_B1F),                           MAP(MAP_ROUTE4)},
    [QL_LOCATION_CERULEAN_GYM]       = {MAP(MAP_CERULEAN_CITY_GYM),                     MAP(MAP_CERULEAN_CITY)},
    [QL_LOCATION_BIKE_SHOP]          = {MAP(MAP_CERULEAN_CITY_BIKE_SHOP),               MAP(MAP_CERULEAN_CITY)},
    [QL_LOCATION_BILLS_HOUSE]        = {MAP(MAP_ROUTE25_SEA_COTTAGE),                   MAP(MAP_ROUTE25)},
    [QL_LOCATION_DAY_CARE]           = {MAP(MAP_ROUTE5_POKEMON_DAY_CARE),               MAP(MAP_ROUTE5)},
    [QL_LOCATION_UNDERGROUND_PATH_1] = {MAP(MAP_UNDERGROUND_PATH_NORTH_ENTRANCE),       MAP(MAP_ROUTE5)},
    [QL_LOCATION_UNDERGROUND_PATH_2] = {MAP(MAP_UNDERGROUND_PATH_SOUTH_ENTRANCE),       MAP(MAP_ROUTE6)},
    [QL_LOCATION_PKMN_FAN_CLUB]      = {MAP(MAP_VERMILION_CITY_POKEMON_FAN_CLUB),       MAP(MAP_VERMILION_CITY)},
    [QL_LOCATION_VERMILION_GYM]      = {MAP(MAP_VERMILION_CITY_GYM),                    MAP(MAP_VERMILION_CITY)},
    [QL_LOCATION_SS_ANNE]            = {MAP(MAP_SSANNE_1F_CORRIDOR),                    MAP(MAP_VERMILION_CITY)},
    [QL_LOCATION_DIGLETTS_CAVE_1]    = {MAP(MAP_DIGLETTS_CAVE_NORTH_ENTRANCE),          MAP(MAP_ROUTE2)},
    [QL_LOCATION_DIGLETTS_CAVE_2]    = {MAP(MAP_DIGLETTS_CAVE_SOUTH_ENTRANCE),          MAP(MAP_ROUTE11)},
    [QL_LOCATION_ROCK_TUNNEL_1]      = {MAP(MAP_ROCK_TUNNEL_1F),                        MAP(MAP_ROUTE10)},
    [QL_LOCATION_ROCK_TUNNEL_2]      = {MAP(MAP_ROCK_TUNNEL_1F),                        MAP(MAP_ROUTE10)},
    [QL_LOCATION_POWER_PLANT]        = {MAP(MAP_POWER_PLANT),                           MAP(MAP_ROUTE10)},
    [QL_LOCATION_PKMN_TOWER]         = {MAP(MAP_POKEMON_TOWER_1F),                      MAP(MAP_LAVENDER_TOWN)},
    [QL_LOCATION_VOLUNTEER_HOUSE]    = {MAP(MAP_LAVENDER_TOWN_VOLUNTEER_POKEMON_HOUSE), MAP(MAP_LAVENDER_TOWN)},
    [QL_LOCATION_NAME_RATERS_HOUSE]  = {MAP(MAP_LAVENDER_TOWN_HOUSE2),                  MAP(MAP_LAVENDER_TOWN)},
    [QL_LOCATION_UNDERGROUND_PATH_3] = {MAP(MAP_UNDERGROUND_PATH_EAST_ENTRANCE),        MAP(MAP_ROUTE8)},
    [QL_LOCATION_UNDERGROUND_PATH_4] = {MAP(MAP_UNDERGROUND_PATH_WEST_ENTRANCE),        MAP(MAP_ROUTE7)},
    [QL_LOCATION_CELADON_DEPT_STORE] = {MAP(MAP_CELADON_CITY_DEPARTMENT_STORE_1F),      MAP(MAP_CELADON_CITY)},
    [QL_LOCATION_CELADON_MANSION]    = {MAP(MAP_CELADON_CITY_CONDOMINIUMS_1F),          MAP(MAP_CELADON_CITY)},
    [QL_LOCATION_GAME_CORNER]        = {MAP(MAP_CELADON_CITY_GAME_CORNER),              MAP(MAP_CELADON_CITY)},
    [QL_LOCATION_CELADON_GYM]        = {MAP(MAP_CELADON_CITY_GYM),                      MAP(MAP_CELADON_CITY)},
    [QL_LOCATION_CELADON_RESTAURANT] = {MAP(MAP_CELADON_CITY_RESTAURANT),               MAP(MAP_CELADON_CITY)},
    [QL_LOCATION_ROCKET_HIDEOUT]     = {MAP(MAP_ROCKET_HIDEOUT_B1F),                    MAP(MAP_CELADON_CITY_GAME_CORNER)},
    [QL_LOCATION_SAFARI_ZONE]        = {MAP(MAP_SAFARI_ZONE_CENTER),                    MAP(MAP_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE)},
    [QL_LOCATION_FUCHSIA_GYM]        = {MAP(MAP_FUCHSIA_CITY_GYM),                      MAP(MAP_FUCHSIA_CITY)},
    [QL_LOCATION_WARDENS_HOME]       = {MAP(MAP_FUCHSIA_CITY_WARDENS_HOUSE),            MAP(MAP_FUCHSIA_CITY)},
    [QL_LOCATION_FIGHTING_DOJO]      = {MAP(MAP_SAFFRON_CITY_DOJO),                     MAP(MAP_SAFFRON_CITY)},
    [QL_LOCATION_SAFFRON_GYM]        = {MAP(MAP_SAFFRON_CITY_GYM),                      MAP(MAP_SAFFRON_CITY)},
    [QL_LOCATION_SILPH_CO]           = {MAP(MAP_SILPH_CO_1F),                           MAP(MAP_SAFFRON_CITY)},
    [QL_LOCATION_SEAFOAM_ISLANDS_1]  = {MAP(MAP_SEAFOAM_ISLANDS_1F),                    MAP(MAP_ROUTE20)},
    [QL_LOCATION_SEAFOAM_ISLANDS_2]  = {MAP(MAP_SEAFOAM_ISLANDS_1F),                    MAP(MAP_ROUTE20)},
    [QL_LOCATION_PKMN_MANSION]       = {MAP(MAP_POKEMON_MANSION_1F),                    MAP(MAP_CINNABAR_ISLAND)},
    [QL_LOCATION_CINNABAR_GYM]       = {MAP(MAP_CINNABAR_ISLAND_GYM),                   MAP(MAP_CINNABAR_ISLAND)},
    [QL_LOCATION_CINNABAR_LAB]       = {MAP(MAP_CINNABAR_ISLAND_POKEMON_LAB_ENTRANCE),  MAP(MAP_CINNABAR_ISLAND)},
    [QL_LOCATION_VICTORY_ROAD_1]     = {MAP(MAP_VICTORY_ROAD_1F),                       MAP(MAP_ROUTE23)},
    [QL_LOCATION_VICTORY_ROAD_2]     = {MAP(MAP_VICTORY_ROAD_2F),                       MAP(MAP_ROUTE23)},
    [QL_LOCATION_PKMN_LEAGUE]        = {MAP(MAP_INDIGO_PLATEAU_POKEMON_CENTER_1F),      MAP(MAP_INDIGO_PLATEAU_EXTERIOR)},
    [QL_LOCATION_CERULEAN_CAVE]      = {MAP(MAP_CERULEAN_CAVE_1F),                      MAP(MAP_CERULEAN_CITY)}
};

void QuestLog_CheckDepartingIndoorsMap(void)
{
    u8 i;
    for (i = 0; i < NELEMS(sInsideOutsidePairs); i++)
    {
        if (gSaveBlock1Ptr->location.mapGroup == sInsideOutsidePairs[i].inside_grp && gSaveBlock1Ptr->location.mapNum == sInsideOutsidePairs[i].inside_num)
        {
            if (VarGet(VAR_QL_ENTRANCE) != QL_LOCATION_ROCKET_HIDEOUT || i != QL_LOCATION_GAME_CORNER)
            {
                VarSet(VAR_QL_ENTRANCE, i);
                FlagSet(FLAG_SYS_QL_DEPARTED);
            }
            break;
        }
    }
}

void QuestLog_TryRecordDepartedLocation(void)
{
    s16 x, y;
    struct QuestLogEvent_Departed data;
    u16 locationId = VarGet(VAR_QL_ENTRANCE);
    data.mapSec = 0;
    data.locationId = 0;
    if (FlagGet(FLAG_SYS_QL_DEPARTED))
    {
        if (locationId == QL_LOCATION_VIRIDIAN_FOREST_1)
        {
            if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_ROUTE2_VIRIDIAN_FOREST_SOUTH_ENTRANCE)
              && (gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ROUTE2_VIRIDIAN_FOREST_SOUTH_ENTRANCE)
               || gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ROUTE2_VIRIDIAN_FOREST_NORTH_ENTRANCE)))
            {
                data.mapSec = MAPSEC_ROUTE_2;
                if (gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ROUTE2_VIRIDIAN_FOREST_SOUTH_ENTRANCE))
                    data.locationId = locationId;
                else
                    data.locationId = locationId + 1;
                SetQuestLogEvent(QL_EVENT_DEPARTED, (const u16 *)&data);
                FlagClear(FLAG_SYS_QL_DEPARTED);
                return;
            }
        }
        else if (locationId == QL_LOCATION_LEAGUE_GATE_1)
        {
            if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_ROUTE22) &&
                (gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ROUTE22)
              || gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ROUTE23)))
            {
                data.mapSec = Overworld_GetMapHeaderByGroupAndId(sInsideOutsidePairs[locationId].inside_grp, sInsideOutsidePairs[locationId].inside_num)->regionMapSectionId;
                if (gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ROUTE22))
                    data.locationId = locationId;
                else
                    data.locationId = locationId + 1;
                SetQuestLogEvent(QL_EVENT_DEPARTED, (const u16 *)&data);
                FlagClear(FLAG_SYS_QL_DEPARTED);
                return;
            }
        }
        if (gSaveBlock1Ptr->location.mapGroup == sInsideOutsidePairs[locationId].outside_grp
           && gSaveBlock1Ptr->location.mapNum == sInsideOutsidePairs[locationId].outside_num)
        {
            data.mapSec = Overworld_GetMapHeaderByGroupAndId(sInsideOutsidePairs[locationId].inside_grp, sInsideOutsidePairs[locationId].inside_num)->regionMapSectionId;
            data.locationId = locationId;
            if (locationId == QL_LOCATION_ROCK_TUNNEL_1)
            {
                PlayerGetDestCoords(&x, &y);
                if (x != 15 || y != 26)
                    data.locationId++;
            }
            else if (locationId == QL_LOCATION_SEAFOAM_ISLANDS_1)
            {
                PlayerGetDestCoords(&x, &y);
                if (x != 67 || y != 15)
                    data.locationId++;
            }
            SetQuestLogEvent(QL_EVENT_DEPARTED, (const u16 *)&data);
            FlagClear(FLAG_SYS_QL_DEPARTED);
            if (locationId == QL_LOCATION_ROCKET_HIDEOUT)
            {
                VarSet(VAR_QL_ENTRANCE, QL_LOCATION_GAME_CORNER);
                FlagSet(FLAG_SYS_QL_DEPARTED);
            }
        }
    }
}

u16 GetMysteryGiftCardStat(void)
{
    switch (gSpecialVar_Result)
    {
    case GET_NUM_STAMPS:
        return MysteryGift_GetCardStat(CARD_STAT_NUM_STAMPS);
    case GET_MAX_STAMPS:
        return MysteryGift_GetCardStat(CARD_STAT_MAX_STAMPS);
    case GET_CARD_BATTLES_WON:
        return MysteryGift_GetCardStat(CARD_STAT_BATTLES_WON);
    case GET_CARD_BATTLES_LOST:
        return MysteryGift_GetCardStat(CARD_STAT_BATTLES_LOST);
    case GET_CARD_NUM_TRADES:
        return MysteryGift_GetCardStat(CARD_STAT_NUM_TRADES);
    default:
        AGB_ASSERT_EX(0, ABSPATH("scr_tool.c"), 3873);
        return 0;
    }
}

void SetPCBoxToSendMon(u8 boxId)
{
    sPCBoxToSendMon = boxId;
}

u16 GetPCBoxToSendMon(void)
{
    return sPCBoxToSendMon;
}

bool8 ShouldShowBoxWasFullMessage(void)
{
    if (FlagGet(FLAG_SHOWN_BOX_WAS_FULL_MESSAGE))
        return FALSE;
    if (StorageGetCurrentBox() == VarGet(VAR_PC_BOX_TO_SEND_MON))
        return FALSE;
    FlagSet(FLAG_SHOWN_BOX_WAS_FULL_MESSAGE);
    return TRUE;
}

bool8 IsDestinationBoxFull(void)
{
    s32 i;
    s32 j;
    SetPCBoxToSendMon(VarGet(VAR_PC_BOX_TO_SEND_MON));
    i = StorageGetCurrentBox();
    do
    {
        for (j = 0; j < IN_BOX_COUNT; j++)
        {
            if (GetBoxMonData(GetBoxedMonPtr(i, j), MON_DATA_SPECIES, NULL) == SPECIES_NONE)
            {
                if (GetPCBoxToSendMon() != i)
                    FlagClear(FLAG_SHOWN_BOX_WAS_FULL_MESSAGE);
                VarSet(VAR_PC_BOX_TO_SEND_MON, i);
                return ShouldShowBoxWasFullMessage();
            }
        }
        i++;
        if (i == TOTAL_BOXES_COUNT)
            i = 0;
    } while (i != StorageGetCurrentBox());
    return FALSE;
}

const u16 sPokeCenter1FMaps[] = {
    MAP_VIRIDIAN_CITY_POKEMON_CENTER_1F,
    MAP_PEWTER_CITY_POKEMON_CENTER_1F,
    MAP_CERULEAN_CITY_POKEMON_CENTER_1F,
    MAP_LAVENDER_TOWN_POKEMON_CENTER_1F,
    MAP_VERMILION_CITY_POKEMON_CENTER_1F,
    MAP_CELADON_CITY_POKEMON_CENTER_1F,
    MAP_FUCHSIA_CITY_POKEMON_CENTER_1F,
    MAP_CINNABAR_ISLAND_POKEMON_CENTER_1F,
    MAP_INDIGO_PLATEAU_POKEMON_CENTER_1F,
    MAP_SAFFRON_CITY_POKEMON_CENTER_1F,
    MAP_ROUTE4_POKEMON_CENTER_1F,
    MAP_ROUTE10_POKEMON_CENTER_1F,
    MAP_ONE_ISLAND_POKEMON_CENTER_1F,
    MAP_TWO_ISLAND_POKEMON_CENTER_1F,
    MAP_THREE_ISLAND_POKEMON_CENTER_1F,
    MAP_FOUR_ISLAND_POKEMON_CENTER_1F,
    MAP_FIVE_ISLAND_POKEMON_CENTER_1F,
    MAP_SEVEN_ISLAND_POKEMON_CENTER_1F,
    MAP_SIX_ISLAND_POKEMON_CENTER_1F,
    MAP_UNION_ROOM,
    MAP_UNDEFINED
};

bool8 UsedPokemonCenterWarp(void)
{
    s32 i;
    u16 mapno = (gLastUsedWarp.mapGroup << 8) + gLastUsedWarp.mapNum;
    for (i = 0; sPokeCenter1FMaps[i] != MAP_UNDEFINED; i++)
    {
        if (sPokeCenter1FMaps[i] == mapno)
            return TRUE;
    }
    return FALSE;
}

bool8 BufferTMHMMoveName(void)
{
    // 8004 = item ID
    if (gSpecialVar_0x8004 >= ITEM_TM01 && gSpecialVar_0x8004 <= ITEM_HM08)
    {
        StringCopy(gStringVar1, gMoveNames[ItemIdToBattleMoveId(gSpecialVar_0x8004)]);
        return TRUE;
    }
    else
        return FALSE;
}

void RunMassageCooldownStepCounter(void)
{
    u16 count = VarGet(VAR_MASSAGE_COOLDOWN_STEP_COUNTER);
    if (count < 500)
        VarSet(VAR_MASSAGE_COOLDOWN_STEP_COUNTER, count + 1);
}

void DaisyMassageServices(void)
{
    AdjustFriendship(&gPlayerParty[gSpecialVar_0x8004], FRIENDSHIP_EVENT_MASSAGE);
    VarSet(VAR_MASSAGE_COOLDOWN_STEP_COUNTER, 0);
}

static const u16 sEliteFourLightingPalettes[][16] = {
    INCBIN_U16("graphics/field_specials/elite_four_lighting_0.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_1.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_2.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_3.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_4.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_5.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_6.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_7.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_8.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_9.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_10.gbapal"),
    INCBIN_U16("graphics/field_specials/elite_four_lighting_11.gbapal")
};

static const u16 sChampionRoomLightingPalettes[][16] = {
    INCBIN_U16("graphics/field_specials/champion_room_lighting_0.gbapal"),
    INCBIN_U16("graphics/field_specials/champion_room_lighting_1.gbapal"),
    INCBIN_U16("graphics/field_specials/champion_room_lighting_2.gbapal"),
    INCBIN_U16("graphics/field_specials/champion_room_lighting_3.gbapal"),
    INCBIN_U16("graphics/field_specials/champion_room_lighting_4.gbapal"),
    INCBIN_U16("graphics/field_specials/champion_room_lighting_5.gbapal"),
    INCBIN_U16("graphics/field_specials/champion_room_lighting_6.gbapal"),
    INCBIN_U16("graphics/field_specials/champion_room_lighting_7.gbapal"),
    INCBIN_U16("graphics/field_specials/champion_room_lighting_8.gbapal")
};

static const u8 sEliteFourLightingTimers[] = {
    40,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12,
    12
};

static const u8 sChampionRoomLightingTimers[] = {
    20,
     8,
     8,
     8,
     8,
     8,
     8,
     8
};

void DoPokemonLeagueLightingEffect(void)
{
    u8 taskId = CreateTask(Task_RunPokemonLeagueLightingEffect, 8);
    s16 *data = gTasks[taskId].data;
    if (FlagGet(FLAG_TEMP_3) == TRUE)
    {
        gTasks[taskId].func = Task_CancelPokemonLeagueLightingEffect;
    }
    else
    {
        if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_POKEMON_LEAGUE_CHAMPIONS_ROOM) && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_POKEMON_LEAGUE_CHAMPIONS_ROOM))
        {
            data[0] = sChampionRoomLightingTimers[0];
            data[2] = 8;
            LoadPalette(sChampionRoomLightingPalettes[0], BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        }
        else
        {
            data[0] = sEliteFourLightingTimers[0];
            data[2] = 11;
            LoadPalette(sEliteFourLightingPalettes[0], BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        }
        data[1] = 0;
        ApplyGlobalTintToPaletteSlot(7, 1);
    }
}

static void Task_RunPokemonLeagueLightingEffect(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!gPaletteFade.active
     && FlagGet(FLAG_TEMP_2) != FALSE
     && FlagGet(FLAG_TEMP_5) != TRUE
     && gGlobalFieldTintMode != QL_TINT_BACKUP_GRAYSCALE
     && --data[0] == 0
    )
    {
        if (++data[1] == data[2])
            data[1] = 0;

        if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_POKEMON_LEAGUE_CHAMPIONS_ROOM) && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_POKEMON_LEAGUE_CHAMPIONS_ROOM))
        {
            data[0] = sChampionRoomLightingTimers[data[1]];
            LoadPalette(sChampionRoomLightingPalettes[data[1]], BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        }
        else
        {
            data[0] = sEliteFourLightingTimers[data[1]];
            LoadPalette(sEliteFourLightingPalettes[data[1]], BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        }
        ApplyGlobalTintToPaletteSlot(7, 1);
    }
}

static void Task_CancelPokemonLeagueLightingEffect(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (FlagGet(FLAG_TEMP_4) != FALSE)
    {
        if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_POKEMON_LEAGUE_CHAMPIONS_ROOM) && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_POKEMON_LEAGUE_CHAMPIONS_ROOM))
            LoadPalette(sChampionRoomLightingPalettes[8], BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        else
            LoadPalette(sEliteFourLightingPalettes[11], BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        ApplyGlobalTintToPaletteSlot(7, 1);
        if (gPaletteFade.active)
        {
            BlendPalettes(0x00000080, 16, RGB_BLACK);
        }
        DestroyTask(taskId);
    }
}

void StopPokemonLeagueLightingEffectTask(void)
{
    if (FuncIsActiveTask(Task_RunPokemonLeagueLightingEffect) == TRUE)
    {
        DestroyTask(FindTaskIdByFunc(Task_RunPokemonLeagueLightingEffect));
    }
}

static const u8 sCapeBrinkCompatibleSpecies[] = {
    SPECIES_VENUSAUR,
    SPECIES_CHARIZARD,
    SPECIES_BLASTOISE
};

bool8 CapeBrinkGetMoveToTeachLeadPokemon(void)
{
    // Returns:
    //   8005 = Move tutor index
    //   8006 = Num moves known by lead mon
    //   8007 = Index of lead mon
    //   to specialvar = whether a move can be taught in the first place
    u8 tutorMonId = 0;
    u8 numMovesKnown = 0;
    u8 leadMonSlot = GetLeadMonIndex();
    u8 i;
    gSpecialVar_0x8007 = leadMonSlot;
    for (i = 0; i < NELEMS(sCapeBrinkCompatibleSpecies); i++)
    {
        if (GetMonData(&gPlayerParty[leadMonSlot], MON_DATA_SPECIES_OR_EGG, NULL) == sCapeBrinkCompatibleSpecies[i])
        {
            tutorMonId = i;
            break;
        }
    }
    if (i == NELEMS(sCapeBrinkCompatibleSpecies) || GetMonData(&gPlayerParty[leadMonSlot], MON_DATA_FRIENDSHIP) != 255)
        return FALSE;
    if (tutorMonId == 0)
    {
        StringCopy(gStringVar2, gMoveNames[MOVE_FRENZY_PLANT]);
        gSpecialVar_0x8005 = MOVETUTOR_FRENZY_PLANT;
        if (FlagGet(FLAG_TUTOR_FRENZY_PLANT) == TRUE)
            return FALSE;
    }
    else if (tutorMonId == 1)
    {
        StringCopy(gStringVar2, gMoveNames[MOVE_BLAST_BURN]);
        gSpecialVar_0x8005 = MOVETUTOR_BLAST_BURN;
        if (FlagGet(FLAG_TUTOR_BLAST_BURN) == TRUE)
            return FALSE;
    }
    else
    {
        StringCopy(gStringVar2, gMoveNames[MOVE_HYDRO_CANNON]);
        gSpecialVar_0x8005 = MOVETUTOR_HYDRO_CANNON;
        if (FlagGet(FLAG_TUTOR_HYDRO_CANNON) == TRUE)
            return FALSE;
    }
    if (GetMonData(&gPlayerParty[leadMonSlot], MON_DATA_MOVE1) != MOVE_NONE)
        numMovesKnown++;
    if (GetMonData(&gPlayerParty[leadMonSlot], MON_DATA_MOVE2) != MOVE_NONE)
        numMovesKnown++;
    if (GetMonData(&gPlayerParty[leadMonSlot], MON_DATA_MOVE3) != MOVE_NONE)
        numMovesKnown++;
    if (GetMonData(&gPlayerParty[leadMonSlot], MON_DATA_MOVE4) != MOVE_NONE)
        numMovesKnown++;
    gSpecialVar_0x8006 = numMovesKnown;
    return TRUE;
}

bool8 HasLearnedAllMovesFromCapeBrinkTutor(void)
{
    // 8005 is set by CapeBrinkGetMoveToTeachLeadPokemon
    u8 r4 = 0;
    if (gSpecialVar_0x8005 == MOVETUTOR_FRENZY_PLANT)
        FlagSet(FLAG_TUTOR_FRENZY_PLANT);
    else if (gSpecialVar_0x8005 == MOVETUTOR_BLAST_BURN)
        FlagSet(FLAG_TUTOR_BLAST_BURN);
    else
        FlagSet(FLAG_TUTOR_HYDRO_CANNON);
    if (FlagGet(FLAG_TUTOR_FRENZY_PLANT) == TRUE)
        r4++;
    if (FlagGet(FLAG_TUTOR_BLAST_BURN) == TRUE)
        r4++;
    if (FlagGet(FLAG_TUTOR_HYDRO_CANNON) == TRUE)
        r4++;
    if (r4 == 3)
        return TRUE;
    else
        return FALSE;
}

bool8 CutMoveRuinValleyCheck(void)
{
    if (FlagGet(FLAG_USED_CUT_ON_RUIN_VALLEY_BRAILLE) != TRUE
     && gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_SIX_ISLAND_RUIN_VALLEY)
     && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SIX_ISLAND_RUIN_VALLEY)
     && gSaveBlock1Ptr->pos.x == 24
     && gSaveBlock1Ptr->pos.y == 25
     && GetPlayerFacingDirection() == DIR_NORTH
    )
        return TRUE;
    else
        return FALSE;
}

void CutMoveOpenDottedHoleDoor(void)
{
    MapGridSetMetatileIdAt(31, 31, METATILE_SeviiIslands67_DottedHoleDoor_Open);
    DrawWholeMapView();
    PlaySE(SE_BANG);
    FlagSet(FLAG_USED_CUT_ON_RUIN_VALLEY_BRAILLE);
    UnlockPlayerFieldControls();
}

static const u16 sDeoxysObjectPals[][16] = {
    INCBIN_U16("graphics/field_specials/deoxys_rock_0.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_1.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_2.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_3.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_4.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_5.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_6.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_7.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_8.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_9.gbapal"),
    INCBIN_U16("graphics/field_specials/deoxys_rock_10.gbapal")
};

static const u8 sDeoxysCoords[][2] = {
    {15, 12},
    {11, 14},
    {15,  8},
    {19, 14},
    {12, 11},
    {18, 11},
    {15, 14},
    {11, 14},
    {19, 14},
    {15, 15},
    {15, 10}
};

static const u8 sDeoxysStepCaps[] = {
    4,
    8,
    8,
    8,
    4,
    4,
    4,
    6,
    3,
    3
};

void DoDeoxysTriangleInteraction(void)
{
    CreateTask(Task_DoDeoxysTriangleInteraction, 8);
}

static void Task_DoDeoxysTriangleInteraction(u8 taskId)
{
    u16 r5;
    u16 r6;
    if (FlagGet(FLAG_SYS_DEOXYS_AWAKENED) == TRUE)
    {
        gSpecialVar_Result = 3;
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
    else
    {
        r5 = VarGet(VAR_DEOXYS_INTERACTION_NUM);
        r6 = VarGet(VAR_DEOXYS_INTERACTION_STEP_COUNTER);
        VarSet(VAR_DEOXYS_INTERACTION_STEP_COUNTER, 0);
        if (r5 != 0 && sDeoxysStepCaps[r5 - 1] < r6)
        {
            MoveDeoxysObject(0);
            VarSet(VAR_DEOXYS_INTERACTION_NUM, 0);
            gSpecialVar_Result = 0;
            DestroyTask(taskId);
        }
        else if (r5 == 10)
        {
            FlagSet(FLAG_SYS_DEOXYS_AWAKENED);
            gSpecialVar_Result = 2;
            ScriptContext_Enable();
            DestroyTask(taskId);
        }
        else
        {
            r5++;
            MoveDeoxysObject(r5);
            VarSet(VAR_DEOXYS_INTERACTION_NUM, r5);
            gSpecialVar_Result = 1;
            DestroyTask(taskId);
        }
    }
}

static void MoveDeoxysObject(u8 num)
{
    u8 mapObjId;
    LoadPalette(sDeoxysObjectPals[num], OBJ_PLTT_ID(10), PLTT_SIZEOF(4));
    ApplyGlobalFieldPaletteTint(10);
    TryGetObjectEventIdByLocalIdAndMap(LOCALID_BIRTH_ISLAND_EXTERIOR_ROCK, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, &mapObjId);
    if (num == 0)
        PlaySE(SE_M_CONFUSE_RAY);
    else
        PlaySE(SE_DEOXYS_MOVE);
    CreateTask(Task_WaitDeoxysFieldEffect, 8);
    gFieldEffectArguments[0] = LOCALID_BIRTH_ISLAND_EXTERIOR_ROCK;
    gFieldEffectArguments[1] =  MAP_NUM(MAP_BIRTH_ISLAND_EXTERIOR);
    gFieldEffectArguments[2] = MAP_GROUP(MAP_BIRTH_ISLAND_EXTERIOR);
    gFieldEffectArguments[3] = sDeoxysCoords[num][0];
    gFieldEffectArguments[4] = sDeoxysCoords[num][1];
    if (num == 0)
        gFieldEffectArguments[5] = 60;
    else
        gFieldEffectArguments[5] = 5;
    FieldEffectStart(FLDEFF_MOVE_DEOXYS_ROCK);
    SetObjEventTemplateCoords(LOCALID_BIRTH_ISLAND_EXTERIOR_ROCK, sDeoxysCoords[num][0], sDeoxysCoords[num][1]);
}

static void Task_WaitDeoxysFieldEffect(u8 taskId)
{
    if (!FieldEffectActiveListContains(FLDEFF_MOVE_DEOXYS_ROCK))
    {
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

void IncrementBirthIslandRockStepCount(void)
{
    u16 count = VarGet(VAR_DEOXYS_INTERACTION_STEP_COUNTER);
    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_BIRTH_ISLAND_EXTERIOR) && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_BIRTH_ISLAND_EXTERIOR))
    {
        count++;
        if (count > 99)
            VarSet(VAR_DEOXYS_INTERACTION_STEP_COUNTER, 0);
        else
            VarSet(VAR_DEOXYS_INTERACTION_STEP_COUNTER, count);
    }
}

void SetDeoxysTrianglePalette(void)
{
    u8 num = VarGet(VAR_DEOXYS_INTERACTION_NUM);
    LoadPalette(sDeoxysObjectPals[num], OBJ_PLTT_ID(10), PLTT_SIZEOF(4));
    ApplyGlobalFieldPaletteTint(10);
}

bool8 IsBadEggInParty(void)
{
    u8 partyCount = CalculatePlayerPartyCount();
    u8 i;
    for (i = 0; i < partyCount; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SANITY_IS_BAD_EGG) == TRUE)
            return TRUE;
    }
    return FALSE;
}

bool8 IsPlayerNotInTrainerTowerLobby(void)
{
    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_TRAINER_TOWER_LOBBY) && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_TRAINER_TOWER_LOBBY))
        return FALSE;
    else
        return TRUE;
}

void BrailleCursorToggle(void)
{
    // 8004 = x - 27
    // 8005 = y
    // 8006 = action (0 = create, 1 = delete)
    u16 x;
    if (gQuestLogState != QL_STATE_PLAYBACK)
    {
        x = gSpecialVar_0x8004 + 27;
        if (gSpecialVar_0x8006 == 0)
            sBrailleTextCursorSpriteID = CreateTextCursorSprite(0, x, gSpecialVar_0x8005, 0, 0);
        else
            DestroyTextCursorSprite(sBrailleTextCursorSpriteID);
    }
}

bool8 PlayerPartyContainsSpeciesWithPlayerID(void)
{
    // 8004 = species
    u8 playerCount = CalculatePlayerPartyCount();
    u8 i;
    for (i = 0; i < playerCount; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL) == gSpecialVar_0x8004 
            && GetPlayerTrainerId() == GetMonData(&gPlayerParty[i], MON_DATA_OT_ID, NULL))
            return TRUE;
    }
    return FALSE;
}

/*
 * Determines which of Lorelei's doll collection to show
 * based on how many times you've entered the Hall of Fame.
 */
void UpdateLoreleiDollCollection(void)
{
    u32 numHofClears = GetGameStat(GAME_STAT_ENTERED_HOF);
    if (numHofClears >= 25)
    {
        FlagClear(FLAG_HIDE_LORELEI_HOUSE_MEOWTH_DOLL);
        if (numHofClears >= 50)
            FlagClear(FLAG_HIDE_LORELEI_HOUSE_CHANSEY_DOLL);
        if (numHofClears >= 75)
            FlagClear(FLAG_HIDE_LORELEIS_HOUSE_NIDORAN_F_DOLL);
        if (numHofClears >= 100)
            FlagClear(FLAG_HIDE_LORELEI_HOUSE_JIGGLYPUFF_DOLL);
        if (numHofClears >= 125)
            FlagClear(FLAG_HIDE_LORELEIS_HOUSE_NIDORAN_M_DOLL);
        if (numHofClears >= 150)
            FlagClear(FLAG_HIDE_LORELEIS_HOUSE_FEAROW_DOLL);
        if (numHofClears >= 175)
            FlagClear(FLAG_HIDE_LORELEIS_HOUSE_PIDGEOT_DOLL);
        if (numHofClears >= 200)
            FlagClear(FLAG_HIDE_LORELEIS_HOUSE_LAPRAS_DOLL);
    }
}

void LoopWingFlapSound(void)
{
    // 8004 = Num flaps
    // 8005 = Frame delay between flaps
    CreateTask(Task_WingFlapSound, 8);
    PlaySE(SE_M_WING_ATTACK);
}

static void Task_WingFlapSound(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    data[1]++;
    if (data[1] == gSpecialVar_0x8005)
    {
        data[0]++;
        data[1] = 0;
        PlaySE(SE_M_WING_ATTACK);
    }
    if (data[0] == gSpecialVar_0x8004 - 1)
        DestroyTask(taskId);
}



static void ResetRocketOpsState(void)
{
    VarSet(VAR_ROCKETOPS_CHAPTER, 0);
    VarSet(VAR_ROCKETOPS_ALERT, 0);
    VarSet(VAR_ROCKETOPS_PROGRESS, 0);
    VarSet(VAR_ROCKETOPS_AGENT_TARGET, 0);
    VarSet(VAR_ROCKETOPS_CHAIN_STATE, 0);
    VarSet(VAR_ROCKETOPS_CH1_STAGE, 0);
    VarSet(VAR_ROCKETOPS_CH2_STAGE, 0);
    VarSet(VAR_ROCKETOPS_CH3_STAGE, 0);
    FlagClear(FLAG_ROCKETOPS_TERMINAL_UNLOCKED);
    FlagClear(FLAG_ROCKETOPS_COMMAND_COOLDOWN);
    FlagClear(FLAG_ROCKETOPS_ROUTE_SECURED);
    FlagClear(FLAG_ROCKETOPS_AGENT_DEPLOYED);
    FlagClear(FLAG_ROCKETOPS_DATA_DESTROYED);
    FlagClear(FLAG_ROCKETOPS_STAFF_EXTRACTED);
    FlagClear(FLAG_GIO_MEM_CH3_EVAC_COMPLETE);
    FlagClear(FLAG_ROCKETOPS_MILESTONE_CH1_LOGGED);
    FlagClear(FLAG_ROCKETOPS_MILESTONE_CH2_LOGGED);
    FlagClear(FLAG_ROCKETOPS_MILESTONE_CH3_LOGGED);
}

static bool8 IsRocketOpsTriggerTypeValidForCommand(u16 commandId, u16 triggerType)
{
    switch (commandId)
    {
    case ROCKETOPS_COMMAND_SECURE_ROUTE:
        return triggerType == ROCKETOPS_TRIGGER_CONTROL_NODE;
    case ROCKETOPS_COMMAND_DEPLOY_AGENT:
        return triggerType == ROCKETOPS_TRIGGER_SURVEILLANCE_NODE;
    case ROCKETOPS_COMMAND_DESTROY_DATA:
        return triggerType == ROCKETOPS_TRIGGER_ARCHIVE_NODE;
    case ROCKETOPS_COMMAND_EXTRACT_STAFF:
        return triggerType == ROCKETOPS_TRIGGER_EXTRACTION_NODE;
    default:
        return FALSE;
    }
}


static const struct GiovanniPartyTemplate *GetGiovanniPartyTemplateByChapter(u8 chapterId)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sGiovanniMemoryPartyTemplates); i++)
    {
        if (sGiovanniMemoryPartyTemplates[i].chapterId == chapterId)
            return &sGiovanniMemoryPartyTemplates[i];
    }

    return NULL;
}

static bool8 LoadGiovanniMemoryPartyTemplate(u8 chapterId)
{
    const struct GiovanniPartyTemplate *template = GetGiovanniPartyTemplateByChapter(chapterId);
    u8 i;

    if (template == NULL)
        return FALSE;

    if (template->monCount == 0 || template->monCount > PARTY_SIZE)
        return FALSE;

    ZeroPlayerPartyMons();
    gPlayerPartyCount = template->monCount;

    for (i = 0; i < template->monCount; i++)
    {
        CreateMon(&gPlayerParty[i], template->mons[i].species, template->mons[i].level, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    }

    for (i = 0; i < PARTY_SIZE; i++)
        CalculateMonStats(&gPlayerParty[i]);

    return TRUE;
}

static bool8 GiovanniRocketProgressFlagsMatchSnapshot(const struct GiovanniMemoryModeFlagSnapshot *flags)
{
    if (flags->hideMiscKantoRockets != FlagGet(FLAG_HIDE_MISC_KANTO_ROCKETS))
        return FALSE;
    if (flags->defeatedLeaderGiovanni != FlagGet(FLAG_DEFEATED_LEADER_GIOVANNI))
        return FALSE;
    if (flags->gotTm26FromGiovanni != FlagGet(FLAG_GOT_TM26_FROM_GIOVANNI))
        return FALSE;
    if (flags->hideViridianGiovanni != FlagGet(FLAG_HIDE_VIRIDIAN_GIOVANNI))
        return FALSE;
    if (flags->hideHideoutGiovanni != FlagGet(FLAG_HIDE_HIDEOUT_GIOVANNI))
        return FALSE;
    if (flags->hideCeruleanRocket != FlagGet(FLAG_HIDE_CERULEAN_ROCKET))
        return FALSE;
    if (flags->hideSaffronRockets != FlagGet(FLAG_HIDE_SAFFRON_ROCKETS))
        return FALSE;
    if (flags->hideCeladonRockets != FlagGet(FLAG_HIDE_CELADON_ROCKETS))
        return FALSE;
    if (flags->hideSaffronCivilians != FlagGet(FLAG_HIDE_SAFFRON_CIVILIANS))
        return FALSE;
    if (flags->hideTowerRocket1 != FlagGet(FLAG_HIDE_TOWER_ROCKET_1))
        return FALSE;
    if (flags->hideTowerRocket2 != FlagGet(FLAG_HIDE_TOWER_ROCKET_2))
        return FALSE;
    if (flags->hideTowerRocket3 != FlagGet(FLAG_HIDE_TOWER_ROCKET_3))
        return FALSE;
    if (flags->rescuedMrFuji != FlagGet(FLAG_RESCUED_MR_FUJI))
        return FALSE;

    return TRUE;
}


u16 StartGiovanniMemoryMode(void)
{
    u8 i;

    if (FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return FALSE;

    if (!IsGiovanniNarrativeBeatReady(GIO_BEAT_CH1_ENTRY))
        return FALSE;

    GetGiovanniMemoryModeSnapshot()->valid = TRUE;
    GetGiovanniMemoryModeSnapshot()->playerPartyCount = gPlayerPartyCount;
    memcpy(GetGiovanniMemoryModeSnapshot()->playerParty, gPlayerParty, sizeof(gPlayerParty));
    GetGiovanniMemoryModeSnapshot()->money = gSaveBlock1Ptr->money;
    GetGiovanniMemoryModeSnapshot()->coins = gSaveBlock1Ptr->coins;
    memcpy(GetGiovanniMemoryModeSnapshot()->bagPocket_Items, gSaveBlock1Ptr->bagPocket_Items, sizeof(gSaveBlock1Ptr->bagPocket_Items));
    memcpy(GetGiovanniMemoryModeSnapshot()->bagPocket_KeyItems, gSaveBlock1Ptr->bagPocket_KeyItems, sizeof(gSaveBlock1Ptr->bagPocket_KeyItems));
    memcpy(GetGiovanniMemoryModeSnapshot()->bagPocket_PokeBalls, gSaveBlock1Ptr->bagPocket_PokeBalls, sizeof(gSaveBlock1Ptr->bagPocket_PokeBalls));
    memcpy(GetGiovanniMemoryModeSnapshot()->bagPocket_TMHM, gSaveBlock1Ptr->bagPocket_TMHM, sizeof(gSaveBlock1Ptr->bagPocket_TMHM));
    memcpy(GetGiovanniMemoryModeSnapshot()->bagPocket_Berries, gSaveBlock1Ptr->bagPocket_Berries, sizeof(gSaveBlock1Ptr->bagPocket_Berries));
    GetGiovanniMemoryModeSnapshot()->location = gSaveBlock1Ptr->location;
    GetGiovanniMemoryModeSnapshot()->continueGameWarp = gSaveBlock1Ptr->continueGameWarp;
    GetGiovanniMemoryModeSnapshot()->dynamicWarp = gSaveBlock1Ptr->dynamicWarp;
    GetGiovanniMemoryModeSnapshot()->lastHealLocation = gSaveBlock1Ptr->lastHealLocation;
    GetGiovanniMemoryModeSnapshot()->escapeWarp = gSaveBlock1Ptr->escapeWarp;
    GetGiovanniMemoryModeSnapshot()->pos = gSaveBlock1Ptr->pos;
    SnapshotGiovanniRocketProgressFlags(&GetGiovanniMemoryModeSnapshot()->flags);

    FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE);
    VarSet(VAR_MODE_GIOVANNI_MEMORY, TRUE);
    VarSet(VAR_CHAPTER_ID, 1);
    FlagSet(FLAG_GIO_MEM_SEQUENCE_VIEWED);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_RESTORED);
    FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER1_STARTED);
    FlagSet(FLAG_GIO_MEM_CH1_STARTED);
    FlagClear(FLAG_GIO_MEM_CH1_COMPLETE);
    FlagClear(FLAG_GIO_MEM_CH2_STARTED);
    FlagClear(FLAG_GIO_MEM_CH2_COMPLETE);
    FlagClear(FLAG_GIO_MEM_CH3_STARTED);
    FlagClear(FLAG_GIO_MEM_CH3_COMPLETE);
    RunGiovanniMemoryModeResetHooks(1);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER3_COMPLETE);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_ABORTED);
    FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_CAPTURE_LOCK);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_VALIDATED);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_VALIDATION_FAILED);
    VarSet(VAR_GIO_CHAPTER, 1);
    SetWarpDestination(MAP_GROUP(MAP_ROCKET_HIDEOUT_B4F), MAP_NUM(MAP_ROCKET_HIDEOUT_B4F), WARP_ID_NONE, 19, 6);
    SetDynamicWarpWithCoords(0, MAP_GROUP(MAP_ROCKET_HIDEOUT_B4F), MAP_NUM(MAP_ROCKET_HIDEOUT_B4F), WARP_ID_NONE, 19, 6);
    ResetRocketOpsState();
    VarSet(VAR_ROCKETOPS_CHAPTER, 1);

    if (!LoadGiovanniMemoryPartyTemplate(1))
        return FALSE;

    gSaveBlock1Ptr->money = 0;
    gSaveBlock1Ptr->coins = 0;
    memset(gSaveBlock1Ptr->bagPocket_Items, 0, sizeof(gSaveBlock1Ptr->bagPocket_Items));
    memset(gSaveBlock1Ptr->bagPocket_KeyItems, 0, sizeof(gSaveBlock1Ptr->bagPocket_KeyItems));
    memset(gSaveBlock1Ptr->bagPocket_PokeBalls, 0, sizeof(gSaveBlock1Ptr->bagPocket_PokeBalls));
    memset(gSaveBlock1Ptr->bagPocket_TMHM, 0, sizeof(gSaveBlock1Ptr->bagPocket_TMHM));
    memset(gSaveBlock1Ptr->bagPocket_Berries, 0, sizeof(gSaveBlock1Ptr->bagPocket_Berries));

    for (i = 0; i < PARTY_SIZE; i++)
        CalculateMonStats(&gPlayerParty[i]);

    return TRUE;
}

u16 SetGiovanniMemoryModeChapter3Complete(void)
{
    if (!FlagGet(FLAG_GIO_MEM_CH3_DATA_DESTROYED)
     || !FlagGet(FLAG_GIO_MEM_CH3_STAFF_EXTRACTED)
     || !FlagGet(FLAG_GIO_MEM_CH3_EVAC_COMPLETE))
        return FALSE;

    VarSet(VAR_GIO_CHAPTER, 3);
    FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER3_COMPLETE);
    FlagSet(FLAG_GIO_MEM_CH3_STARTED);
    FlagSet(FLAG_GIO_MEM_CH3_COMPLETE);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_ABORTED);
    RunGiovanniMemoryModeResetHooks(3);

    if (!LoadGiovanniMemoryPartyTemplate(3))
        return FALSE;

    return TRUE;
}

u16 SetGiovanniMemoryModePartyForCurrentChapter(void)
{
    if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return FALSE;

    return LoadGiovanniMemoryPartyTemplate(GetGiovanniMemoryModeChapterId());
}

u16 AbortGiovanniMemoryMode(void)
{
    FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ABORTED);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CAPTURE_LOCK);
    ResetRocketOpsState();
    RunGiovanniMemoryModeResetHooks(0);
    return TRUE;
}

u16 RestoreGiovanniMemoryModeSnapshot(void)
{
    if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return FALSE;

    RunGiovanniMemoryModeResetHooks(GetGiovanniMemoryModeChapterId());

    if (FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_RESTORED))
        return TRUE;

    if (!GetGiovanniMemoryModeSnapshot()->valid)
        return FALSE;

    gPlayerPartyCount = GetGiovanniMemoryModeSnapshot()->playerPartyCount;
    memcpy(gPlayerParty, GetGiovanniMemoryModeSnapshot()->playerParty, sizeof(gPlayerParty));
    gSaveBlock1Ptr->money = GetGiovanniMemoryModeSnapshot()->money;
    gSaveBlock1Ptr->coins = GetGiovanniMemoryModeSnapshot()->coins;
    memcpy(gSaveBlock1Ptr->bagPocket_Items, GetGiovanniMemoryModeSnapshot()->bagPocket_Items, sizeof(gSaveBlock1Ptr->bagPocket_Items));
    memcpy(gSaveBlock1Ptr->bagPocket_KeyItems, GetGiovanniMemoryModeSnapshot()->bagPocket_KeyItems, sizeof(gSaveBlock1Ptr->bagPocket_KeyItems));
    memcpy(gSaveBlock1Ptr->bagPocket_PokeBalls, GetGiovanniMemoryModeSnapshot()->bagPocket_PokeBalls, sizeof(gSaveBlock1Ptr->bagPocket_PokeBalls));
    memcpy(gSaveBlock1Ptr->bagPocket_TMHM, GetGiovanniMemoryModeSnapshot()->bagPocket_TMHM, sizeof(gSaveBlock1Ptr->bagPocket_TMHM));
    memcpy(gSaveBlock1Ptr->bagPocket_Berries, GetGiovanniMemoryModeSnapshot()->bagPocket_Berries, sizeof(gSaveBlock1Ptr->bagPocket_Berries));
    gSaveBlock1Ptr->location = GetGiovanniMemoryModeSnapshot()->location;
    gSaveBlock1Ptr->continueGameWarp = GetGiovanniMemoryModeSnapshot()->continueGameWarp;
    gSaveBlock1Ptr->dynamicWarp = GetGiovanniMemoryModeSnapshot()->dynamicWarp;
    gSaveBlock1Ptr->lastHealLocation = GetGiovanniMemoryModeSnapshot()->lastHealLocation;
    gSaveBlock1Ptr->escapeWarp = GetGiovanniMemoryModeSnapshot()->escapeWarp;
    gSaveBlock1Ptr->pos = GetGiovanniMemoryModeSnapshot()->pos;

    if (GiovanniRocketProgressFlagsMatchSnapshot(&GetGiovanniMemoryModeSnapshot()->flags))
    {
        FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_VALIDATED);
        FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_VALIDATION_FAILED);
    }
    else
    {
        FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_VALIDATION_FAILED);
        FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_VALIDATED);
    }

    RestoreGiovanniRocketProgressFlags(&GetGiovanniMemoryModeSnapshot()->flags);

    FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_RESTORED);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CAPTURE_LOCK);
    VarSet(VAR_GIO_CHAPTER, 0);
    ResetRocketOpsState();
    GetGiovanniMemoryModeSnapshot()->valid = FALSE;
    RunGiovanniMemoryModeResetHooks(0);
    return TRUE;
}

bool8 HandleGiovanniMemoryModeWhiteout(void)
{
    if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return FALSE;

    RunGiovanniMemoryModeResetHooks(GetGiovanniMemoryModeChapterId());

    if (!FlagGet(FLAG_GIO_MEM_CH1_COMPLETE))
    {
        VarSet(VAR_GIO_CHAPTER, 1);
        SetWarpDestination(MAP_GROUP(MAP_ROCKET_HIDEOUT_B4F), MAP_NUM(MAP_ROCKET_HIDEOUT_B4F), WARP_ID_NONE, 18, 6);
        return TRUE;
    }

    if (!FlagGet(FLAG_GIO_MEM_CH2_COMPLETE))
    {
        VarSet(VAR_GIO_CHAPTER, 2);
        SetWarpDestination(MAP_GROUP(MAP_SILPH_CO_11F), MAP_NUM(MAP_SILPH_CO_11F), WARP_ID_NONE, 6, 13);
        return TRUE;
    }

    if (FlagGet(FLAG_GIO_MEM_CH3_COMPLETE))
    {
        VarSet(VAR_GIO_CHAPTER, 3);
        if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_RESTORED)
         && RestoreGiovanniMemoryModeSnapshot() == FALSE)
        {
            AbortGiovanniMemoryMode();
            ReconcileGiovanniMemoryModeOutcome();
        }
        SetWarpDestination(MAP_GROUP(MAP_VIRIDIAN_CITY_GYM), MAP_NUM(MAP_VIRIDIAN_CITY_GYM), WARP_ID_NONE, 17, 20);
        return TRUE;
    }

    AbortGiovanniMemoryMode();
    RestoreGiovanniMemoryModeSnapshot();
    ReconcileGiovanniMemoryModeOutcome();
    SetWarpDestination(MAP_GROUP(MAP_VIRIDIAN_CITY_GYM), MAP_NUM(MAP_VIRIDIAN_CITY_GYM), WARP_ID_NONE, 17, 20);
    return TRUE;
}

bool8 HandleGiovanniMemoryModeBootstrapOnLoad(void)
{
    if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return FALSE;

    RunGiovanniMemoryModeResetHooks(GetGiovanniMemoryModeChapterId());

    if (!FlagGet(FLAG_GIO_MEM_CH1_COMPLETE))
    {
        VarSet(VAR_GIO_CHAPTER, 1);
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROCKET_HIDEOUT_B4F);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROCKET_HIDEOUT_B4F);
        gSaveBlock1Ptr->location.warpId = WARP_ID_NONE;
        gSaveBlock1Ptr->location.x = 18;
        gSaveBlock1Ptr->location.y = 6;
    }
    else if (!FlagGet(FLAG_GIO_MEM_CH2_COMPLETE))
    {
        VarSet(VAR_GIO_CHAPTER, 2);
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_SILPH_CO_11F);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_SILPH_CO_11F);
        gSaveBlock1Ptr->location.warpId = WARP_ID_NONE;
        gSaveBlock1Ptr->location.x = 6;
        gSaveBlock1Ptr->location.y = 13;
    }
    else
    {
        VarSet(VAR_GIO_CHAPTER, 3);
        if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_RESTORED)
         && RestoreGiovanniMemoryModeSnapshot() == FALSE)
        {
            AbortGiovanniMemoryMode();
            ReconcileGiovanniMemoryModeOutcome();
        }
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_VIRIDIAN_CITY_GYM);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_VIRIDIAN_CITY_GYM);
        gSaveBlock1Ptr->location.warpId = WARP_ID_NONE;
        gSaveBlock1Ptr->location.x = 17;
        gSaveBlock1Ptr->location.y = 20;
    }

    gSaveBlock1Ptr->pos.x = gSaveBlock1Ptr->location.x;
    gSaveBlock1Ptr->pos.y = gSaveBlock1Ptr->location.y;
    gSaveBlock1Ptr->continueGameWarp = gSaveBlock1Ptr->location;
    return TRUE;
}

u16 IsGiovanniMemoryModeReadyForBattle(void)
{
    if (!IsGiovanniNarrativeBeatReady(GIO_BEAT_CH3_ENTRY))
        return FALSE;

    if (FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER3_COMPLETE)
     && FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_RESTORED)
     && !FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return TRUE;

    return FALSE;
}

bool8 IsGiovanniMemoryModeCaptureLocked(void)
{
    return FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_CAPTURE_LOCK);
}

u16 ClearGiovanniMemoryModeCaptureLock(void)
{
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CAPTURE_LOCK);
    return TRUE;
}

u16 ReconcileGiovanniMemoryModeOutcome(void)
{
    FlagSet(FLAG_GIO_MEM_SEQUENCE_VIEWED);
    FlagClear(FLAG_GIO_MEM_CH1_STARTED);
    FlagClear(FLAG_GIO_MEM_CH1_COMPLETE);
    FlagClear(FLAG_GIO_MEM_CH2_STARTED);
    FlagClear(FLAG_GIO_MEM_CH2_COMPLETE);
    FlagClear(FLAG_GIO_MEM_CH3_STARTED);
    FlagClear(FLAG_GIO_MEM_CH3_COMPLETE);
    RunGiovanniMemoryModeResetHooks(0);
    ResetRocketOpsState();

    return TRUE;
}

u16 SyncGiovanniMemoryModeNpcState(void)
{
    if (FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
    {
        u8 chapterId = GetGiovanniMemoryModeChapterId();
        RunGiovanniMemoryModeResetHooks(chapterId);
    }
    else
    {
        RunGiovanniMemoryModeResetHooks(0);
    }
    return TRUE;
}


u16 Special_RocketOps_OpenTerminal(void)
{
    u16 chapterId;

    if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return FALSE;

    if (VarGet(VAR_MODE_GIOVANNI_MEMORY) != TRUE)
        return FALSE;

    chapterId = VarGet(VAR_CHAPTER_ID);
    if (chapterId < 1 || chapterId > 3)
        return FALSE;

    VarSet(VAR_ROCKETOPS_CHAPTER, chapterId);
    FlagSet(FLAG_ROCKETOPS_TERMINAL_UNLOCKED);
    FlagClear(FLAG_ROCKETOPS_COMMAND_COOLDOWN);
    return TRUE;
}

u16 Special_RocketOps_ValidateCommandContext(void)
{
    u16 chapterId = VarGet(VAR_ROCKETOPS_CHAPTER);
    u16 commandId = gSpecialVar_0x8004;
    u16 triggerType = gSpecialVar_0x8005;
    u16 chapterStageVar;
    u16 chapterStage;

    if (!FlagGet(FLAG_ROCKETOPS_TERMINAL_UNLOCKED))
        return FALSE;
    if (FlagGet(FLAG_ROCKETOPS_COMMAND_COOLDOWN))
        return FALSE;
    if (chapterId < 1 || chapterId > 3)
        return FALSE;
    if (commandId >= ROCKETOPS_COMMAND_COUNT)
        return FALSE;
    if (!IsRocketOpsTriggerTypeValidForCommand(commandId, triggerType))
        return FALSE;

    chapterStageVar = VAR_ROCKETOPS_CH1_STAGE + chapterId - 1;
    chapterStage = VarGet(chapterStageVar);

    if (chapterId == 1)
    {
        if (commandId == ROCKETOPS_COMMAND_SECURE_ROUTE)
            return chapterStage == 0;
        if (commandId == ROCKETOPS_COMMAND_DEPLOY_AGENT)
            return chapterStage == 1;
        if (commandId == ROCKETOPS_COMMAND_EXTRACT_STAFF)
            return chapterStage == 2;
        return FALSE;
    }

    if (chapterId == 2)
    {
        if (commandId == ROCKETOPS_COMMAND_SECURE_ROUTE)
            return chapterStage == 0;
        if (commandId == ROCKETOPS_COMMAND_DESTROY_DATA)
            return chapterStage == 1;
        if (commandId == ROCKETOPS_COMMAND_EXTRACT_STAFF)
            return chapterStage == 2;
        return FALSE;
    }

    if (commandId == ROCKETOPS_COMMAND_DEPLOY_AGENT)
        return chapterStage == 0;
    if (commandId == ROCKETOPS_COMMAND_DESTROY_DATA)
        return chapterStage == 1;
    if (commandId == ROCKETOPS_COMMAND_EXTRACT_STAFF)
        return chapterStage == 2;
    return FALSE;
}

u16 Special_RocketOps_WritebackState(void)
{
    s32 alert;
    u16 chapterId = VarGet(VAR_ROCKETOPS_CHAPTER);
    u16 commandId = gSpecialVar_0x8004;
    u16 chapterStageVar;

    if (commandId >= ROCKETOPS_COMMAND_COUNT)
        return FALSE;

    if (chapterId < 1 || chapterId > 3)
        return FALSE;

    chapterStageVar = VAR_ROCKETOPS_CH1_STAGE + chapterId - 1;

    switch (commandId)
    {
    case ROCKETOPS_COMMAND_SECURE_ROUTE:
        FlagSet(FLAG_ROCKETOPS_ROUTE_SECURED);
        alert = VarGet(VAR_ROCKETOPS_ALERT);
        if (alert > 0)
            VarSet(VAR_ROCKETOPS_ALERT, alert - 1);
        VarSet(VAR_ROCKETOPS_PROGRESS, VarGet(VAR_ROCKETOPS_PROGRESS) + 1);
        if (VarGet(chapterStageVar) == 0)
            VarSet(chapterStageVar, 1);
        break;
    case ROCKETOPS_COMMAND_DEPLOY_AGENT:
        FlagSet(FLAG_ROCKETOPS_AGENT_DEPLOYED);
        VarSet(VAR_ROCKETOPS_AGENT_TARGET, gSpecialVar_0x8005);
        VarSet(VAR_ROCKETOPS_PROGRESS, VarGet(VAR_ROCKETOPS_PROGRESS) + 1);
        if (VarGet(chapterStageVar) == 1)
            VarSet(chapterStageVar, 2);
        break;
    case ROCKETOPS_COMMAND_DESTROY_DATA:
        FlagSet(FLAG_ROCKETOPS_DATA_DESTROYED);
        VarSet(VAR_ROCKETOPS_ALERT, VarGet(VAR_ROCKETOPS_ALERT) + 1);
        VarSet(VAR_ROCKETOPS_PROGRESS, VarGet(VAR_ROCKETOPS_PROGRESS) + 2);
        break;
    case ROCKETOPS_COMMAND_EXTRACT_STAFF:
        FlagSet(FLAG_ROCKETOPS_STAFF_EXTRACTED);
        VarSet(VAR_ROCKETOPS_PROGRESS, VarGet(VAR_ROCKETOPS_PROGRESS) + 2);
        if (VarGet(chapterStageVar) == 2)
        {
            VarSet(chapterStageVar, 3);
            if (chapterId == 1)
                FlagSet(FLAG_ROCKETOPS_MILESTONE_CH1_LOGGED);
            else if (chapterId == 2)
                FlagSet(FLAG_ROCKETOPS_MILESTONE_CH2_LOGGED);
            else
                FlagSet(FLAG_ROCKETOPS_MILESTONE_CH3_LOGGED);
        }
        break;
    }

    VarSet(VAR_ROCKETOPS_CHAIN_STATE, VarGet(chapterStageVar));
    FlagSet(FLAG_ROCKETOPS_COMMAND_COOLDOWN);
    SyncGiovanniMemoryModeNpcState();
    return TRUE;
}

u16 IsGiovanniNarrativeBeatReadySpecial(void)
{
    return IsGiovanniNarrativeBeatReady(gSpecialVar_0x8004);
}

u16 GetGiovanniNarrativeBeatFallbackChapterSpecial(void)
{
    return ResolveGiovanniBeatFallbackChapter(gSpecialVar_0x8004);
}

u16 DebugForceGiovanniMemoryModeChapterState(void)
{
    u8 chapterId = gSpecialVar_0x8004;

    FlagClear(FLAG_GIO_MEM_CH1_STARTED);
    FlagClear(FLAG_GIO_MEM_CH1_COMPLETE);
    FlagClear(FLAG_GIO_MEM_CH2_STARTED);
    FlagClear(FLAG_GIO_MEM_CH2_COMPLETE);
    FlagClear(FLAG_GIO_MEM_CH3_STARTED);
    FlagClear(FLAG_GIO_MEM_CH3_COMPLETE);

    if (chapterId == 0)
    {
        FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE);
        FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_RESTORED);
        FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER1_STARTED);
        FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER3_COMPLETE);
        FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_ABORTED);
        FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CAPTURE_LOCK);
        VarSet(VAR_GIO_CHAPTER, 0);
        RunGiovanniMemoryModeResetHooks(0);
        return TRUE;
    }

    if (chapterId > 3)
        return FALSE;

    FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_RESTORED);
    FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER1_STARTED);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER3_COMPLETE);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_ABORTED);
    FlagClear(FLAG_SYS_GIOVANNI_MEMORY_MODE_CAPTURE_LOCK);
    FlagSet(FLAG_GIO_MEM_SEQUENCE_VIEWED);

    FlagSet(FLAG_GIO_MEM_CH1_STARTED);
    if (chapterId >= 2)
    {
        FlagSet(FLAG_GIO_MEM_CH1_COMPLETE);
        FlagSet(FLAG_GIO_MEM_CH2_STARTED);
    }
    if (chapterId >= 3)
    {
        FlagSet(FLAG_GIO_MEM_CH2_COMPLETE);
        FlagSet(FLAG_GIO_MEM_CH3_STARTED);
        FlagSet(FLAG_GIO_MEM_CH3_COMPLETE);
        FlagSet(FLAG_SYS_GIOVANNI_MEMORY_MODE_CHAPTER3_COMPLETE);
    }

    VarSet(VAR_GIO_CHAPTER, chapterId);
    RunGiovanniMemoryModeResetHooks(chapterId);
    return TRUE;
}

u16 ValidateGiovanniMemoryModeRocketFlags(void)
{
    if (FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_VALIDATION_FAILED))
        return FALSE;

    if (FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_VALIDATED))
        return TRUE;

    return FALSE;
}

u16 ValidateGiovanniInteractionOverlayForCurrentMap(void)
{
    const struct GiovanniMapOverlay *overlay;
    u16 map = (gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum;
    u8 chapterId = GetGiovanniMemoryModeChapterId();

    overlay = GetGiovanniOverlayForMap(map);
    if (overlay == NULL)
        return TRUE;

    if (!FlagGet(FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE))
        return TRUE;

    VarSet(VAR_MODE_GIOVANNI_MEMORY, TRUE);
    VarSet(VAR_CHAPTER_ID, chapterId);

    if (chapterId != overlay->chapterId)
    {
        WarpPlayerToGiovanniOverlaySafeReturn(overlay);
        return FALSE;
    }

    if (overlay->allowedWarpMask == 0
     || (overlay->allowedWarpMask & (1 << overlay->guaranteedProgressWarpId)) == 0)
    {
        SetWarpDestination(MAP_GROUP(overlay->safeReturnMap), MAP_NUM(overlay->safeReturnMap), WARP_ID_NONE, overlay->recoveryFallbackX, overlay->recoveryFallbackY);
        WarpIntoMap();
        SetMainCallback2(CB2_LoadMap);
        return FALSE;
    }

    if (gSaveBlock1Ptr->location.warpId != WARP_ID_NONE
     && !IsOverlayWarpAllowed(overlay, gSaveBlock1Ptr->location.warpId))
    {
        WarpPlayerToGiovanniOverlaySafeReturn(overlay);
        return FALSE;
    }

    ApplyGiovanniOverlayForCurrentMap(overlay);
    ApplyGiovanniOverlayNpcSet(overlay);
    return TRUE;
}
