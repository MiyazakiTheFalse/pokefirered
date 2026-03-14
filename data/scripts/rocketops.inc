@ Shared RocketOps terminal flow and chapter command dispatch.

EventScript_RocketOps_Terminal::
	lockall
	specialvar VAR_RESULT, Special_RocketOps_OpenTerminal
	goto_if_eq VAR_RESULT, FALSE, EventScript_RocketOps_TerminalOffline
	msgbox Text_RocketOpsTerminalBoot

EventScript_RocketOps_MenuLoop::
	multichoice 0, 0, MULTICHOICE_ROCKETOPS_COMMANDS, FALSE
	switch VAR_RESULT
	case 0, EventScript_RocketOps_SelectSecureRoute
	case 1, EventScript_RocketOps_SelectDeployAgent
	case 2, EventScript_RocketOps_SelectDestroyData
	case 3, EventScript_RocketOps_SelectExtractStaff
	case 4, EventScript_RocketOps_Close

EventScript_RocketOps_SelectSecureRoute::
	setvar VAR_0x8004, 0
	specialvar VAR_RESULT, Special_RocketOps_ValidateCommandContext
	goto_if_eq VAR_RESULT, TRUE, EventScript_RocketOps_Command_SecureRoute
	msgbox Text_RocketOpsFailSecureRoute
	goto EventScript_RocketOps_MenuReturn

EventScript_RocketOps_SelectDeployAgent::
	setvar VAR_0x8004, 1
	specialvar VAR_RESULT, Special_RocketOps_ValidateCommandContext
	goto_if_eq VAR_RESULT, TRUE, EventScript_RocketOps_Command_DeployAgent
	msgbox Text_RocketOpsFailDeployAgent
	goto EventScript_RocketOps_MenuReturn

EventScript_RocketOps_SelectDestroyData::
	setvar VAR_0x8004, 2
	specialvar VAR_RESULT, Special_RocketOps_ValidateCommandContext
	goto_if_eq VAR_RESULT, TRUE, EventScript_RocketOps_Command_DestroyData
	msgbox Text_RocketOpsFailDestroyData
	addvar VAR_ROCKETOPS_ALERT, 1
	goto EventScript_RocketOps_MenuReturn

EventScript_RocketOps_SelectExtractStaff::
	setvar VAR_0x8004, 3
	specialvar VAR_RESULT, Special_RocketOps_ValidateCommandContext
	goto_if_eq VAR_RESULT, TRUE, EventScript_RocketOps_Command_ExtractStaff
	msgbox Text_RocketOpsFailExtractStaff
	addvar VAR_ROCKETOPS_ALERT, 1
	goto EventScript_RocketOps_MenuReturn

EventScript_RocketOps_MenuReturn::
	goto EventScript_RocketOps_MenuLoop

EventScript_RocketOps_Command_SecureRoute::
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 1, EventScript_RocketOps_Command_SecureRoute_C1
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 2, EventScript_RocketOps_Command_SecureRoute_C2
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 3, EventScript_RocketOps_Command_SecureRoute_C3
	goto EventScript_RocketOps_CommandChapterMismatch

EventScript_RocketOps_Command_DeployAgent::
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 1, EventScript_RocketOps_Command_DeployAgent_C1
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 2, EventScript_RocketOps_Command_DeployAgent_C2
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 3, EventScript_RocketOps_Command_DeployAgent_C3
	goto EventScript_RocketOps_CommandChapterMismatch

EventScript_RocketOps_Command_DestroyData::
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 1, EventScript_RocketOps_Command_DestroyData_C1
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 2, EventScript_RocketOps_Command_DestroyData_C2
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 3, EventScript_RocketOps_Command_DestroyData_C3
	goto EventScript_RocketOps_CommandChapterMismatch

EventScript_RocketOps_Command_ExtractStaff::
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 1, EventScript_RocketOps_Command_ExtractStaff_C1
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 2, EventScript_RocketOps_Command_ExtractStaff_C2
	goto_if_eq VAR_ROCKETOPS_CHAPTER, 3, EventScript_RocketOps_Command_ExtractStaff_C3
	goto EventScript_RocketOps_CommandChapterMismatch

EventScript_RocketOps_CommandChapterMismatch::
	msgbox Text_RocketOpsCommandChapterMismatch
	goto EventScript_RocketOps_Close

EventScript_RocketOps_CommandFinalize::
	special Special_RocketOps_WritebackState
	goto EventScript_RocketOps_Close

EventScript_RocketOps_TerminalOffline::
	msgbox Text_RocketOpsTerminalOffline

EventScript_RocketOps_Close::
	releaseall
	end

Text_RocketOpsTerminalBoot::
	.string "[ROCKETOPS] Link established. Select\n"
	.string "an operation command.$"

Text_RocketOpsTerminalOffline::
	.string "The terminal does not respond outside\n"
	.string "Giovanni's active memory operation.$"

Text_RocketOpsCommandChapterMismatch::
	.string "[ROCKETOPS] Chapter context mismatch.\n"
	.string "Command routing denied.$"

Text_RocketOpsFailSecureRoute::
	.string "Route cannot be secured from this\n"
	.string "terminal.$"

Text_RocketOpsFailDeployAgent::
	.string "No valid insertion lane available.$"

Text_RocketOpsFailDestroyData::
	.string "Data core is mirrored; purge denied.$"

Text_RocketOpsFailExtractStaff::
	.string "Extraction window closed. Hostiles\n"
	.string "too close.$"
