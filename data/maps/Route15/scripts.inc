Route15_MapScripts::
	.byte 0

Route15_EventScript_RouteSign::
	msgbox Route15_Text_RouteSign, MSGBOX_SIGN
	end

Route15_EventScript_HiddenPassSign::
	goto_if_unset FLAG_DEFEATED_ROUTE12_KOGA_SABRINA, Route15_EventScript_HiddenPassSignLocked
	goto_if_set FLAG_ROUTE15_HIDDEN_PASS_DISCOVERED, Route15_EventScript_HiddenPassSignRepeat
	setflag FLAG_ROUTE15_HIDDEN_PASS_DISCOVERED
	msgbox Route15_Text_HiddenPassSignFirst, MSGBOX_SIGN
	end

Route15_EventScript_HiddenPassSignRepeat::
	msgbox Route15_Text_HiddenPassSignRepeat, MSGBOX_SIGN
	end

Route15_EventScript_HiddenPassSignLocked::
	msgbox Route15_Text_RouteSign, MSGBOX_SIGN
	end
