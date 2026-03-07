Route5_MapScripts::
	.byte 0

Route5_EventScript_UnderpassGuard::
	lock
	faceplayer
	msgbox Route5_Text_UnderpassGuard
	release
	end

Route5_EventScript_UndergroundPathSign::
	msgbox Route5_Text_UndergroundPathSign, MSGBOX_SIGN
	end
