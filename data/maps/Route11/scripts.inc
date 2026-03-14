Route11_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route11_OnTransition
	.byte 0

Route11_OnTransition::
	specialvar VAR_RESULT, ValidateGiovanniInteractionOverlayForCurrentMap
	goto_if_eq VAR_RESULT, FALSE, EventScript_Return
	end

Route11_EventScript_DiglettsCaveSign::
	msgbox Route11_Text_DiglettsCave, MSGBOX_SIGN
	end

Route11_EventScript_MemoryRouteCheckpoint::
	goto_if_unset FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE, EventScript_Nothing
	goto_if_ne VAR_CHAPTER_ID, 1, EventScript_Nothing
	setflag FLAG_GIO_MEM_CH1_DUGTRIO_BOSS_DEFEATED
	setvar VAR_GIO_ACT, 2
	setvar VAR_GIO_CHECKPOINT_ID, 1
	end
