	.include "MPlayDef.s"

	.equ	mus_vs_duo_koga_sabrina_grp, voicegroup155
	.equ	mus_vs_duo_koga_sabrina_pri, 0
	.equ	mus_vs_duo_koga_sabrina_rev, reverb_set+50
	.equ	mus_vs_duo_koga_sabrina_mvl, 90
	.equ	mus_vs_duo_koga_sabrina_key, 0
	.equ	mus_vs_duo_koga_sabrina_tbs, 1
	.equ	mus_vs_duo_koga_sabrina_exg, 1
	.equ	mus_vs_duo_koga_sabrina_cmp, 1

	.section .rodata
	.global	mus_vs_duo_koga_sabrina
	.align	2

@**************** Track 1 (Midi-Chn.1) ****************@

mus_vs_duo_koga_sabrina_1:
	.byte		VOL   , 127*mus_vs_duo_koga_sabrina_mvl/mxv
	.byte	KEYSH , mus_vs_duo_koga_sabrina_key+0
@ 000   ----------------------------------------
@ 001   ----------------------------------------
	.byte	TEMPO , 150*mus_vs_duo_koga_sabrina_tbs/2
	.byte		VOICE , 81
	.byte		N12   , Cs4 , v100
	.byte	W12
	.byte		        En4 , v096
	.byte	W12
	.byte		N24   , Fs4 
	.byte	W36
	.byte		N12   , En4 , v092
	.byte	W12
	.byte		        Dn4 
	.byte	W12
	.byte		N24   , Cs4 , v096
	.byte	W12
@ 002   ----------------------------------------
	.byte	W12
	.byte		N12   , Gs4 , v100
	.byte	W12
	.byte		        An4 , v096
	.byte	W12
	.byte		N24   , Bn4 
	.byte	W36
	.byte		N12   , An4 , v092
	.byte	W12
	.byte		        Gs4 
	.byte	W12
@ 003   ----------------------------------------
	.byte		N24   , Fs4 , v096
	.byte	W24
	.byte	FINE

@**************** Track 2 (Midi-Chn.2) ****************@

mus_vs_duo_koga_sabrina_2:
	.byte		VOL   , 127*mus_vs_duo_koga_sabrina_mvl/mxv
	.byte	KEYSH , mus_vs_duo_koga_sabrina_key+0
@ 000   ----------------------------------------
	.byte		VOICE , 39
	.byte		N24   , Cs1 , v088
	.byte	W24
	.byte		N24   
	.byte	W24
	.byte		        En1 
	.byte	W24
	.byte		        Gs1 
	.byte	W24
@ 001   ----------------------------------------
	.byte		        Cs1 
	.byte	W24
	.byte		N24   
	.byte	W24
	.byte		        Bn0 
	.byte	W24
	.byte		        Gs0 
	.byte	W24
@ 002   ----------------------------------------
	.byte		        Gs1 
	.byte	W24
	.byte		N24   
	.byte	W24
	.byte		        Fs1 
	.byte	W24
	.byte		        En1 
	.byte	W24
@ 003   ----------------------------------------
	.byte		        Cs1 
	.byte	W24
	.byte		N24   
	.byte	W24
	.byte		        En1 
	.byte	W24
	.byte		        Gs1 
	.byte	W24
@ 004   ----------------------------------------
	.byte	FINE

@**************** Track 3 (Midi-Chn.3) ****************@

mus_vs_duo_koga_sabrina_3:
	.byte		VOL   , 127*mus_vs_duo_koga_sabrina_mvl/mxv
	.byte	KEYSH , mus_vs_duo_koga_sabrina_key+0
@ 000   ----------------------------------------
	.byte		VOICE , 52
	.byte		N12   , Cs3 , v072
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Gs3 
	.byte	W12
	.byte		        Bn2 
	.byte	W12
	.byte		        Ds3 
	.byte	W12
	.byte		        Fs3 
	.byte	W12
	.byte		        Gs2 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
@ 001   ----------------------------------------
	.byte		        En3 
	.byte	W12
	.byte		        An2 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Gs3 
	.byte	W12
	.byte		        Bn2 
	.byte	W12
@ 002   ----------------------------------------
	.byte		        Ds3 
	.byte	W12
	.byte		        Fs3 
	.byte	W12
	.byte		        Gs2 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Fs2 
	.byte	W12
	.byte		        An2 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
@ 003   ----------------------------------------
	.byte		        En3 
	.byte	W12
	.byte		        Gs3 
	.byte	W12
	.byte		        Bn3 
	.byte	W12
	.byte		        Ds3 
	.byte	W12
	.byte		        Fs3 
	.byte	W12
	.byte		        Bn3 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
	.byte		        En3 
	.byte	W12
@ 004   ----------------------------------------
	.byte		        Gs3 
	.byte	W12
	.byte		        Bn2 
	.byte	W12
	.byte		        Ds3 
	.byte	W12
	.byte		        Fs3 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        Gs3 
	.byte	W12
	.byte		        Bn2 
	.byte	W12
@ 005   ----------------------------------------
	.byte		        Ds3 
	.byte	W12
	.byte		        Fs3 
	.byte	W12
	.byte		        Gs2 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
	.byte		        En3 
	.byte	W12
	.byte		        An2 
	.byte	W12
	.byte		        Cs3 
	.byte	W12
	.byte		        En3 
	.byte	W12
@ 006   ----------------------------------------
	.byte	FINE

@**************** Track 4 (Midi-Chn.10) ****************@

mus_vs_duo_koga_sabrina_4:
	.byte		VOL   , 127*mus_vs_duo_koga_sabrina_mvl/mxv
	.byte	KEYSH , mus_vs_duo_koga_sabrina_key+0
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

mus_vs_duo_koga_sabrina:
	.byte	4	@ NumTrks
	.byte	0	@ NumBlks
	.byte	mus_vs_duo_koga_sabrina_pri	@ Priority
	.byte	mus_vs_duo_koga_sabrina_rev	@ Reverb.

	.word	mus_vs_duo_koga_sabrina_grp

	.word	mus_vs_duo_koga_sabrina_1
	.word	mus_vs_duo_koga_sabrina_2
	.word	mus_vs_duo_koga_sabrina_3
	.word	mus_vs_duo_koga_sabrina_4

	.end
