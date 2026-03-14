CeladonCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CeladonCity_OnTransition
	.byte 0

CeladonCity_OnTransition::
	specialvar VAR_RESULT, ValidateGiovanniInteractionOverlayForCurrentMap
	goto_if_eq VAR_RESULT, FALSE, EventScript_Return
	setworldmapflag FLAG_WORLD_MAP_CELADON_CITY
	call_if_set FLAG_ROCKET_DISBANDED, CeladonCity_EventScript_HideRocketsAfterDisband
	call_if_unset FLAG_ROCKET_DISBANDED, CeladonCity_EventScript_ShowRocketsBeforeDisband
	end

CeladonCity_EventScript_HideRocketsAfterDisband::
	setflag FLAG_HIDE_CELADON_ROCKETS
	return

CeladonCity_EventScript_ShowRocketsBeforeDisband::
	clearflag FLAG_HIDE_CELADON_ROCKETS
	return

CeladonCity_EventScript_LittleGirl::
	msgbox CeladonCity_Text_GotMyKoffingInCinnabar, MSGBOX_NPC
	end

CeladonCity_EventScript_OldMan1::
	lock
	msgbox CeladonCity_Text_GymIsGreatFullOfWomen
	release
	end

CeladonCity_EventScript_Woman::
	msgbox CeladonCity_Text_GameCornerIsBadForCitysImage, MSGBOX_NPC
	end

CeladonCity_EventScript_OldMan2::
	msgbox CeladonCity_Text_BlewItAllAtSlots, MSGBOX_NPC
	end

CeladonCity_EventScript_SoftboiledTutor::
	goto EventScript_SoftboiledTutor
	end

CeladonCity_EventScript_FatMan::
	lock
	faceplayer
	msgbox CeladonCity_Text_MyTrustedPalPoliwrath
	closemessage
	applymovement LOCALID_CELADON_FAT_MAN, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

CeladonCity_EventScript_Poliwrath::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_POLIWRATH, CRY_MODE_NORMAL
	msgbox CeladonCity_Text_Poliwrath
	closemessage
	waitmoncry
	applymovement LOCALID_CELADON_POLIWRATH, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

CeladonCity_EventScript_RocketGrunt1::
	goto_if_set FLAG_ROCKET_DISBANDED, CeladonCity_EventScript_RocketGrunt1_PostDisband
	msgbox CeladonCity_Text_KeepOutOfTeamRocketsWay, MSGBOX_NPC
	end

CeladonCity_EventScript_RocketGrunt1_PostDisband::
	msgbox CeladonCity_Text_FormerRocketGrunt1, MSGBOX_NPC
	end

CeladonCity_EventScript_RocketGrunt2::
	goto_if_set FLAG_ROCKET_DISBANDED, CeladonCity_EventScript_RocketGrunt2_PostDisband
	msgbox CeladonCity_Text_GetLostOrIllPunchYou, MSGBOX_NPC
	end

CeladonCity_EventScript_RocketGrunt2_PostDisband::
	msgbox CeladonCity_Text_FormerRocketGrunt2, MSGBOX_NPC
	end

CeladonCity_EventScript_Boy::
	msgbox CeladonCity_Text_ScaldedTongueOnTea, MSGBOX_NPC
	end

CeladonCity_EventScript_SilphCoScientist::
	goto_if_set FLAG_ROCKET_DISBANDED, CeladonCity_EventScript_SilphCoScientist_PostDisband
	msgbox CeladonCity_Text_SomeoneStoleSilphScope, MSGBOX_NPC
	end

CeladonCity_EventScript_SilphCoScientist_PostDisband::
	msgbox CeladonCity_Text_SilphScopeRecovered, MSGBOX_NPC
	end

CeladonCity_EventScript_TrainerTips1::
	msgbox CeladonCity_Text_ExplainXAccuracyDireHit, MSGBOX_SIGN
	end

CeladonCity_EventScript_CitySign::
	msgbox CeladonCity_Text_CitySign, MSGBOX_SIGN
	end

CeladonCity_EventScript_GymSign::
	lockall
	famechecker FAMECHECKER_ERIKA, 0
	msgbox CeladonCity_Text_GymSign
	releaseall
	end

CeladonCity_EventScript_MansionSign::
	msgbox CeladonCity_Text_MansionSign, MSGBOX_SIGN
	end

CeladonCity_EventScript_DeptStoreSign::
	msgbox CeladonCity_Text_DeptStoreSign, MSGBOX_SIGN
	end

CeladonCity_EventScript_TrainerTips2::
	msgbox CeladonCity_Text_GuardSpecProtectsFromStatus, MSGBOX_SIGN
	end

CeladonCity_EventScript_PrizeExchangeSign::
	msgbox CeladonCity_Text_PrizeExchangeSign, MSGBOX_SIGN
	end

CeladonCity_EventScript_GameCornerSign::
	msgbox CeladonCity_Text_GameCornerSign, MSGBOX_SIGN
	end

CeladonCity_EventScript_MemoryRouteCheckpoint::
	goto_if_unset FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE, EventScript_Nothing
	goto_if_ne VAR_CHAPTER_ID, 2, EventScript_Nothing
	setflag FLAG_GIO_MEM_CH2_STARTED
	setvar VAR_GIO_CHAPTER, 2
	setvar VAR_GIO_ACT, 1
	setvar VAR_GIO_CHECKPOINT_ID, 0
	end
