Route8_MapScripts::
	.byte 0

Route8_EventScript_UndergroundPathSign::
	msgbox Route8_Text_UndergroundPathSign, MSGBOX_SIGN
	end

Route8_EventScript_ShortcutBlocker::
	lock
	faceplayer
	msgbox Route8_Text_ShortcutBlocked
	release
	end
