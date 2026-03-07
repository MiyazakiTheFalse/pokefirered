Route5_MapScripts::
	.byte 0

Route5_EventScript_UnderpassGuard::
	lock
	faceplayer
	msgbox Route5_Text_UnderpassGuard
	release
	end

Route5_EventScript_UndergroundPathSign::
	goto_if_set FLAG_HIDE_ROUTE5_ROUTE6_UNDERPASS_GUARDS, Route5_EventScript_UndergroundPathSignOpen
	msgbox Route5_Text_UndergroundPathSign, MSGBOX_SIGN
	end

Route5_EventScript_UndergroundPathSignOpen::
	msgbox Route5_Text_UndergroundPathSignOpen, MSGBOX_SIGN
	end
