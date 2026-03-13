@ Shared precedence for warp tiles that are used by both normal traversal and
@ Giovanni memory chapters.
@
@ Precedence contract:
@ 1) Verify Giovanni mode + chapter.
@ 2) If valid, caller uses chapter destination / locked message handling.
@ 3) Otherwise, caller falls back to normal route or chapter-desync hub warp.

	.set GIO_SHARED_WARP_RESULT_NORMAL_ROUTE, 0
	.set GIO_SHARED_WARP_RESULT_CHAPTER_ROUTE, 1
	.set GIO_SHARED_WARP_RESULT_LOCKED_FALLBACK, 2

EventScript_GiovanniSharedWarp_ResolvePrecedence::
	setvar VAR_RESULT, GIO_SHARED_WARP_RESULT_NORMAL_ROUTE
	goto_if_unset FLAG_SYS_GIOVANNI_MEMORY_MODE_ACTIVE, EventScript_GiovanniSharedWarp_Return
	goto_if_eq VAR_MODE_GIOVANNI_MEMORY, FALSE, EventScript_GiovanniSharedWarp_SetLockedFallback
	goto_if_set FLAG_GIO_MEM_CH3_ESCORT_SEGMENT_ACTIVE, EventScript_GiovanniSharedWarp_SetLockedFallback
	compare VAR_CHAPTER_ID, VAR_0x8004
	goto_if_ne EventScript_GiovanniSharedWarp_SetLockedFallback
	setvar VAR_RESULT, GIO_SHARED_WARP_RESULT_CHAPTER_ROUTE
	return

EventScript_GiovanniSharedWarp_SetLockedFallback::
	setvar VAR_RESULT, GIO_SHARED_WARP_RESULT_LOCKED_FALLBACK

EventScript_GiovanniSharedWarp_Return::
	return
