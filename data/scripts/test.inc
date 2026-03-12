Test_EventScript_NPC::
	msgbox Test_Text_WelcomeToWorldOfPokemon, MSGBOX_NPC
	end

EventScript_TestSignpostMsg::
	msgbox Test_Text_ThisIsASignpost, MSGBOX_SIGN
	end

Test_EventScript_CoordEvent::
	msgbox Test_Text_ThisIsACoordEvent, MSGBOX_SIGN
	end

#ifndef NDEBUG
Test_EventScript_DebugSetRoute12BossCompletionFlag::
	setflag FLAG_DEFEATED_ROUTE12_KOGA_SABRINA
	end

Test_EventScript_DebugClearRoute12BossCompletionFlag::
	clearflag FLAG_DEFEATED_ROUTE12_KOGA_SABRINA
	end

Test_EventScript_DebugSetRoute12ShortcutUnlockedFlag::
	setflag FLAG_ROUTE12_BRIDGE_SHORTCUT_CLEARED
	end

Test_EventScript_DebugClearRoute12ShortcutUnlockedFlag::
	clearflag FLAG_ROUTE12_BRIDGE_SHORTCUT_CLEARED
	end

Test_EventScript_DebugSetRoute12EventActiveFlag::
	setflag FLAG_ROUTE12_BRIDGE_MIDPOINT_BLOCKERS
	end

Test_EventScript_DebugClearRoute12EventActiveFlag::
	clearflag FLAG_ROUTE12_BRIDGE_MIDPOINT_BLOCKERS
	end
Test_EventScript_DebugSetGiovanniChapter0::
	setvar VAR_0x8004, 0
	special DebugForceGiovanniMemoryModeChapterState
	end

Test_EventScript_DebugSetGiovanniChapter1::
	setvar VAR_0x8004, 1
	special DebugForceGiovanniMemoryModeChapterState
	end

Test_EventScript_DebugSetGiovanniChapter2::
	setvar VAR_0x8004, 2
	special DebugForceGiovanniMemoryModeChapterState
	end

Test_EventScript_DebugSetGiovanniChapter3::
	setvar VAR_0x8004, 3
	special DebugForceGiovanniMemoryModeChapterState
	end

#endif

Test_Text_WelcomeToWorldOfPokemon::
	.string "テストよう　メッセージです！\n"
	.string "ポケモンの　せかいへ　ようこそ！$"

Test_Text_ThisIsASignpost::
	.string "テストよう　メッセージです！\n"
	.string "かんばん　です$"

Test_Text_ThisIsACoordEvent::
	.string "テストよう　メッセージです！\n"
	.string "ざひょう　チェックの　イベントです$"

	.align 2
Test_Text_Empty::
	.string "$"
