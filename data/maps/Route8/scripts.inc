Route8_MapScripts::
	.byte 0

Route8_EventScript_UndergroundPathSign::
	goto_if_set FLAG_UNLOCKED_ROUTE7_TO_ROUTE8_SHORTCUT, Route8_EventScript_UndergroundPathSignOpen
	msgbox Route8_Text_UndergroundPathSign, MSGBOX_SIGN
	end

Route8_EventScript_UndergroundPathSignOpen::
	msgbox Route8_Text_UndergroundPathSignOpen, MSGBOX_SIGN
	end

Route8_EventScript_ShortcutBlocker::
	lock
	faceplayer
	msgbox Route8_Text_ShortcutBlocked
	release
	end
