	.include "MPlayDef.s"

	.equ	mus_vs_duo_brock_misty_grp, voicegroup155
	.equ	mus_vs_duo_brock_misty_pri, 0
	.equ	mus_vs_duo_brock_misty_rev, reverb_set+50
	.equ	mus_vs_duo_brock_misty_mvl, 90
	.equ	mus_vs_duo_brock_misty_key, 0
	.equ	mus_vs_duo_brock_misty_tbs, 1
	.equ	mus_vs_duo_brock_misty_exg, 1
	.equ	mus_vs_duo_brock_misty_cmp, 1

	.section .rodata
	.global	mus_vs_duo_brock_misty
	.align	2

@**************** Track 1 (Midi-Chn.1) ****************@

mus_vs_duo_brock_misty_1:
	.byte		VOL   , 127*mus_vs_duo_brock_misty_mvl/mxv
	.byte	KEYSH , mus_vs_duo_brock_misty_key+0
@ 000   ----------------------------------------
@ 001   ----------------------------------------
	.byte	TEMPO , 152*mus_vs_duo_brock_misty_tbs/2
	.byte		VOICE , 29
	.byte		N12   , En3 , v100
	.byte	W12
	.byte		        Gn3 
	.byte	W12
	.byte		N24   , Bn3 , v096
	.byte	W36
	.byte		N12   , An3 , v092
	.byte	W12
	.byte		        Gn3 
	.byte	W12
	.byte		N24   , En3 , v096
	.byte	W12
@ 002   ----------------------------------------
	.byte	W12
	.byte		N12   , Cn4 , v104
	.byte	W12
	.byte		        Dn4 , v096
	.byte	W12
	.byte		N24   , En4 
	.byte	W36
	.byte		N12   , Dn4 , v092
	.byte	W12
	.byte		        Cn4 
	.byte	W12
@ 003   ----------------------------------------
	.byte		N24   , Bn3 , v096
	.byte	W24
	.byte	FINE

@**************** Track 2 (Midi-Chn.2) ****************@

mus_vs_duo_brock_misty_2:
	.byte		VOL   , 127*mus_vs_duo_brock_misty_mvl/mxv
	.byte	KEYSH , mus_vs_duo_brock_misty_key+0
@ 000   ----------------------------------------
	.byte		VOICE , 34
	.byte		N24   , En1 , v088
	.byte	W24
	.byte		N24   
	.byte	W24
	.byte		        Gn1 
	.byte	W24
	.byte		        Bn1 
	.byte	W24
@ 001   ----------------------------------------
	.byte		        En1 
	.byte	W24
	.byte		N24   
	.byte	W24
	.byte		        Dn1 
	.byte	W24
	.byte		        Bn0 
	.byte	W24
@ 002   ----------------------------------------
	.byte		        Bn1 
	.byte	W24
	.byte		N24   
	.byte	W24
	.byte		        An1 
	.byte	W24
	.byte		        Gn1 
	.byte	W24
@ 003   ----------------------------------------
	.byte		        En1 
	.byte	W24
	.byte		N24   
	.byte	W24
	.byte		        Gn1 
	.byte	W24
	.byte		        Bn1 
	.byte	W24
@ 004   ----------------------------------------
	.byte	FINE

@**************** Track 3 (Midi-Chn.3) ****************@

mus_vs_duo_brock_misty_3:
	.byte		VOL   , 127*mus_vs_duo_brock_misty_mvl/mxv
	.byte	KEYSH , mus_vs_duo_brock_misty_key+0
@ 000   ----------------------------------------
	.byte		VOICE , 48
	.byte		N12   , En3 , v072
	.byte	W12
	.byte		        Gn3 
	.byte	W12
	.byte		        Bn3 
	.byte	W12
	.byte		        Dn3 
	.byte	W12
	.byte		        Fs3 
	.byte	W12
	.byte		        An3 
	.byte	W12
	.byte		        Bn2 
	.byte	W12
	.byte		        En3 
	.byte	W12
@ 001   ----------------------------------------
	.byte		        Gn3 
	.byte	W12
	.byte		        Cn3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        An3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Gn3 
	.byte	W12
	.byte		        Bn3 
	.byte	W12
	.byte		        Dn3 
	.byte	W12
@ 002   ----------------------------------------
	.byte		        Fs3 
	.byte	W12
	.byte		        An3 
	.byte	W12
	.byte		        Bn2 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Gn3 
	.byte	W12
	.byte		        An2 
	.byte	W12
	.byte		        Cn3 
	.byte	W12
	.byte		        En3 
	.byte	W12
@ 003   ----------------------------------------
	.byte		        Gn3 
	.byte	W12
	.byte		        Bn3 
	.byte	W12
	.byte		        Dn4 
	.byte	W12
	.byte		        Fs3 
	.byte	W12
	.byte		        An3 
	.byte	W12
	.byte		        Dn4 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Gn3 
	.byte	W12
@ 004   ----------------------------------------
	.byte		        Bn3 
	.byte	W12
	.byte		        Dn3 
	.byte	W12
	.byte		        Fs3 
	.byte	W12
	.byte		        An3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Gn3 
	.byte	W12
	.byte		        Bn3 
	.byte	W12
	.byte		        Dn3 
	.byte	W12
@ 005   ----------------------------------------
	.byte		        Fs3 
	.byte	W12
	.byte		        An3 
	.byte	W12
	.byte		        Bn2 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Gn3 
	.byte	W12
	.byte		        Cn3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        An3 
	.byte	W12
@ 006   ----------------------------------------
	.byte	FINE

@**************** Track 4 (Midi-Chn.10) ****************@

mus_vs_duo_brock_misty_4:
	.byte		VOL   , 127*mus_vs_duo_brock_misty_mvl/mxv
	.byte	KEYSH , mus_vs_duo_brock_misty_key+0
@ 000   ----------------------------------------
	.byte		N12   , Fs1 , v056
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
@ 001   ----------------------------------------
	.byte		N01   , Cn1 , v096
	.byte		N01   
	.byte		N01   
	.byte		N01   
	.byte		N01   , Dn1 , v088
	.byte		N01   
	.byte		N12   , Fs1 , v056
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
	.byte		N12   
	.byte	W12
@ 002   ----------------------------------------
	.byte		N01   , Cn1 , v096
	.byte		N01   
	.byte		N01   
	.byte		N01   
	.byte		N01   , Dn1 , v088
	.byte		N01   
	.byte	FINE

@******************************************************@
	.align	2

mus_vs_duo_brock_misty:
	.byte	4	@ NumTrks
	.byte	0	@ NumBlks
	.byte	mus_vs_duo_brock_misty_pri	@ Priority
	.byte	mus_vs_duo_brock_misty_rev	@ Reverb.

	.word	mus_vs_duo_brock_misty_grp

	.word	mus_vs_duo_brock_misty_1
	.word	mus_vs_duo_brock_misty_2
	.word	mus_vs_duo_brock_misty_3
	.word	mus_vs_duo_brock_misty_4

	.end
