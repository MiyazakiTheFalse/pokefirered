Route2_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route2_OnTransition
	.byte 0

Route2_OnTransition::
	specialvar VAR_RESULT, ValidateGiovanniInteractionOverlayForCurrentMap
	goto_if_eq VAR_RESULT, FALSE, EventScript_Return
	end

Route2_EventScript_RouteSign::
	msgbox Route2_Text_RouteSign, MSGBOX_SIGN
	end

Route2_EventScript_DiglettsCaveSign::
	msgbox Route2_Text_DiglettsCave, MSGBOX_SIGN
	end

Route2_EventScript_MemoryRouteCheckpoint::
	goto_if_unset FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE, EventScript_Nothing
	goto_if_ne VAR_CHAPTER_ID, 1, EventScript_Nothing
	setflag FLAG_GIO_MEM_CH1_STARTED
	setvar VAR_GIO_CHAPTER, 1
	setvar VAR_GIO_ACT, 1
	setvar VAR_GIO_CHECKPOINT_ID, 0
	end
