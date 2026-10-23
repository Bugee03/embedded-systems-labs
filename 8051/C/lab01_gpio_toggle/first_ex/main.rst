                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ISO C Compiler
                                      3 ; Version 4.5.0 #15242 (Mac OS X ppc)
                                      4 ;--------------------------------------------------------
                                      5 	.module main
                                      6 	
                                      7 	.optsdcc -mmcs51 --model-small
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _main
                                     12 	.globl _CY
                                     13 	.globl _AC
                                     14 	.globl _F0
                                     15 	.globl _RS1
                                     16 	.globl _RS0
                                     17 	.globl _OV
                                     18 	.globl _F1
                                     19 	.globl _P
                                     20 	.globl _PS
                                     21 	.globl _PT1
                                     22 	.globl _PX1
                                     23 	.globl _PT0
                                     24 	.globl _PX0
                                     25 	.globl _RD
                                     26 	.globl _WR
                                     27 	.globl _T1
                                     28 	.globl _T0
                                     29 	.globl _INT1
                                     30 	.globl _INT0
                                     31 	.globl _TXD
                                     32 	.globl _RXD
                                     33 	.globl _P3_7
                                     34 	.globl _P3_6
                                     35 	.globl _P3_5
                                     36 	.globl _P3_4
                                     37 	.globl _P3_3
                                     38 	.globl _P3_2
                                     39 	.globl _P3_1
                                     40 	.globl _P3_0
                                     41 	.globl _EA
                                     42 	.globl _ES
                                     43 	.globl _ET1
                                     44 	.globl _EX1
                                     45 	.globl _ET0
                                     46 	.globl _EX0
                                     47 	.globl _P2_7
                                     48 	.globl _P2_6
                                     49 	.globl _P2_5
                                     50 	.globl _P2_4
                                     51 	.globl _P2_3
                                     52 	.globl _P2_2
                                     53 	.globl _P2_1
                                     54 	.globl _P2_0
                                     55 	.globl _SM0
                                     56 	.globl _SM1
                                     57 	.globl _SM2
                                     58 	.globl _REN
                                     59 	.globl _TB8
                                     60 	.globl _RB8
                                     61 	.globl _TI
                                     62 	.globl _RI
                                     63 	.globl _P1_7
                                     64 	.globl _P1_6
                                     65 	.globl _P1_5
                                     66 	.globl _P1_4
                                     67 	.globl _P1_3
                                     68 	.globl _P1_2
                                     69 	.globl _P1_1
                                     70 	.globl _P1_0
                                     71 	.globl _TF1
                                     72 	.globl _TR1
                                     73 	.globl _TF0
                                     74 	.globl _TR0
                                     75 	.globl _IE1
                                     76 	.globl _IT1
                                     77 	.globl _IE0
                                     78 	.globl _IT0
                                     79 	.globl _P0_7
                                     80 	.globl _P0_6
                                     81 	.globl _P0_5
                                     82 	.globl _P0_4
                                     83 	.globl _P0_3
                                     84 	.globl _P0_2
                                     85 	.globl _P0_1
                                     86 	.globl _P0_0
                                     87 	.globl _B
                                     88 	.globl _ACC
                                     89 	.globl _PSW
                                     90 	.globl _IP
                                     91 	.globl _P3
                                     92 	.globl _IE
                                     93 	.globl _P2
                                     94 	.globl _SBUF
                                     95 	.globl _SCON
                                     96 	.globl _P1
                                     97 	.globl _TH1
                                     98 	.globl _TH0
                                     99 	.globl _TL1
                                    100 	.globl _TL0
                                    101 	.globl _TMOD
                                    102 	.globl _TCON
                                    103 	.globl _PCON
                                    104 	.globl _DPH
                                    105 	.globl _DPL
                                    106 	.globl _SP
                                    107 	.globl _P0
                                    108 ;--------------------------------------------------------
                                    109 ; special function registers
                                    110 ;--------------------------------------------------------
                                    111 	.area RSEG    (ABS,DATA)
      000000                        112 	.org 0x0000
                           000080   113 _P0	=	0x0080
                           000081   114 _SP	=	0x0081
                           000082   115 _DPL	=	0x0082
                           000083   116 _DPH	=	0x0083
                           000087   117 _PCON	=	0x0087
                           000088   118 _TCON	=	0x0088
                           000089   119 _TMOD	=	0x0089
                           00008A   120 _TL0	=	0x008a
                           00008B   121 _TL1	=	0x008b
                           00008C   122 _TH0	=	0x008c
                           00008D   123 _TH1	=	0x008d
                           000090   124 _P1	=	0x0090
                           000098   125 _SCON	=	0x0098
                           000099   126 _SBUF	=	0x0099
                           0000A0   127 _P2	=	0x00a0
                           0000A8   128 _IE	=	0x00a8
                           0000B0   129 _P3	=	0x00b0
                           0000B8   130 _IP	=	0x00b8
                           0000D0   131 _PSW	=	0x00d0
                           0000E0   132 _ACC	=	0x00e0
                           0000F0   133 _B	=	0x00f0
                                    134 ;--------------------------------------------------------
                                    135 ; special function bits
                                    136 ;--------------------------------------------------------
                                    137 	.area RSEG    (ABS,DATA)
      000000                        138 	.org 0x0000
                           000080   139 _P0_0	=	0x0080
                           000081   140 _P0_1	=	0x0081
                           000082   141 _P0_2	=	0x0082
                           000083   142 _P0_3	=	0x0083
                           000084   143 _P0_4	=	0x0084
                           000085   144 _P0_5	=	0x0085
                           000086   145 _P0_6	=	0x0086
                           000087   146 _P0_7	=	0x0087
                           000088   147 _IT0	=	0x0088
                           000089   148 _IE0	=	0x0089
                           00008A   149 _IT1	=	0x008a
                           00008B   150 _IE1	=	0x008b
                           00008C   151 _TR0	=	0x008c
                           00008D   152 _TF0	=	0x008d
                           00008E   153 _TR1	=	0x008e
                           00008F   154 _TF1	=	0x008f
                           000090   155 _P1_0	=	0x0090
                           000091   156 _P1_1	=	0x0091
                           000092   157 _P1_2	=	0x0092
                           000093   158 _P1_3	=	0x0093
                           000094   159 _P1_4	=	0x0094
                           000095   160 _P1_5	=	0x0095
                           000096   161 _P1_6	=	0x0096
                           000097   162 _P1_7	=	0x0097
                           000098   163 _RI	=	0x0098
                           000099   164 _TI	=	0x0099
                           00009A   165 _RB8	=	0x009a
                           00009B   166 _TB8	=	0x009b
                           00009C   167 _REN	=	0x009c
                           00009D   168 _SM2	=	0x009d
                           00009E   169 _SM1	=	0x009e
                           00009F   170 _SM0	=	0x009f
                           0000A0   171 _P2_0	=	0x00a0
                           0000A1   172 _P2_1	=	0x00a1
                           0000A2   173 _P2_2	=	0x00a2
                           0000A3   174 _P2_3	=	0x00a3
                           0000A4   175 _P2_4	=	0x00a4
                           0000A5   176 _P2_5	=	0x00a5
                           0000A6   177 _P2_6	=	0x00a6
                           0000A7   178 _P2_7	=	0x00a7
                           0000A8   179 _EX0	=	0x00a8
                           0000A9   180 _ET0	=	0x00a9
                           0000AA   181 _EX1	=	0x00aa
                           0000AB   182 _ET1	=	0x00ab
                           0000AC   183 _ES	=	0x00ac
                           0000AF   184 _EA	=	0x00af
                           0000B0   185 _P3_0	=	0x00b0
                           0000B1   186 _P3_1	=	0x00b1
                           0000B2   187 _P3_2	=	0x00b2
                           0000B3   188 _P3_3	=	0x00b3
                           0000B4   189 _P3_4	=	0x00b4
                           0000B5   190 _P3_5	=	0x00b5
                           0000B6   191 _P3_6	=	0x00b6
                           0000B7   192 _P3_7	=	0x00b7
                           0000B0   193 _RXD	=	0x00b0
                           0000B1   194 _TXD	=	0x00b1
                           0000B2   195 _INT0	=	0x00b2
                           0000B3   196 _INT1	=	0x00b3
                           0000B4   197 _T0	=	0x00b4
                           0000B5   198 _T1	=	0x00b5
                           0000B6   199 _WR	=	0x00b6
                           0000B7   200 _RD	=	0x00b7
                           0000B8   201 _PX0	=	0x00b8
                           0000B9   202 _PT0	=	0x00b9
                           0000BA   203 _PX1	=	0x00ba
                           0000BB   204 _PT1	=	0x00bb
                           0000BC   205 _PS	=	0x00bc
                           0000D0   206 _P	=	0x00d0
                           0000D1   207 _F1	=	0x00d1
                           0000D2   208 _OV	=	0x00d2
                           0000D3   209 _RS0	=	0x00d3
                           0000D4   210 _RS1	=	0x00d4
                           0000D5   211 _F0	=	0x00d5
                           0000D6   212 _AC	=	0x00d6
                           0000D7   213 _CY	=	0x00d7
                                    214 ;--------------------------------------------------------
                                    215 ; overlayable register banks
                                    216 ;--------------------------------------------------------
                                    217 	.area REG_BANK_0	(REL,OVR,DATA)
      000000                        218 	.ds 8
                                    219 ;--------------------------------------------------------
                                    220 ; internal ram data
                                    221 ;--------------------------------------------------------
                                    222 	.area DSEG    (DATA)
                                    223 ;--------------------------------------------------------
                                    224 ; overlayable items in internal ram
                                    225 ;--------------------------------------------------------
                                    226 	.area	OSEG    (OVR,DATA)
      000008                        227 _main_mysymb_10000_2:
      000008                        228 	.ds 11
                                    229 ;--------------------------------------------------------
                                    230 ; Stack segment in internal ram
                                    231 ;--------------------------------------------------------
                                    232 	.area SSEG
      000013                        233 __start__stack:
      000013                        234 	.ds	1
                                    235 
                                    236 ;--------------------------------------------------------
                                    237 ; indirectly addressable internal ram data
                                    238 ;--------------------------------------------------------
                                    239 	.area ISEG    (DATA)
                                    240 ;--------------------------------------------------------
                                    241 ; absolute internal ram data
                                    242 ;--------------------------------------------------------
                                    243 	.area IABS    (ABS,DATA)
                                    244 	.area IABS    (ABS,DATA)
                                    245 ;--------------------------------------------------------
                                    246 ; bit data
                                    247 ;--------------------------------------------------------
                                    248 	.area BSEG    (BIT)
                                    249 ;--------------------------------------------------------
                                    250 ; paged external ram data
                                    251 ;--------------------------------------------------------
                                    252 	.area PSEG    (PAG,XDATA)
                                    253 ;--------------------------------------------------------
                                    254 ; uninitialized external ram data
                                    255 ;--------------------------------------------------------
                                    256 	.area XSEG    (XDATA)
                                    257 ;--------------------------------------------------------
                                    258 ; absolute external ram data
                                    259 ;--------------------------------------------------------
                                    260 	.area XABS    (ABS,XDATA)
                                    261 ;--------------------------------------------------------
                                    262 ; initialized external ram data
                                    263 ;--------------------------------------------------------
                                    264 	.area XISEG   (XDATA)
                                    265 	.area HOME    (CODE)
                                    266 	.area GSINIT0 (CODE)
                                    267 	.area GSINIT1 (CODE)
                                    268 	.area GSINIT2 (CODE)
                                    269 	.area GSINIT3 (CODE)
                                    270 	.area GSINIT4 (CODE)
                                    271 	.area GSINIT5 (CODE)
                                    272 	.area GSINIT  (CODE)
                                    273 	.area GSFINAL (CODE)
                                    274 	.area CSEG    (CODE)
                                    275 ;--------------------------------------------------------
                                    276 ; interrupt vector
                                    277 ;--------------------------------------------------------
                                    278 	.area HOME    (CODE)
      000000                        279 __interrupt_vect:
      000000 02 00 4C         [24]  280 	ljmp	__sdcc_gsinit_startup
                                    281 ; restartable atomic support routines
      000003                        282 	.ds	5
      000008                        283 sdcc_atomic_exchange_rollback_start::
      000008 00               [12]  284 	nop
      000009 00               [12]  285 	nop
      00000A                        286 sdcc_atomic_exchange_pdata_impl:
      00000A E2               [24]  287 	movx	a, @r0
      00000B FB               [12]  288 	mov	r3, a
      00000C EA               [12]  289 	mov	a, r2
      00000D F2               [24]  290 	movx	@r0, a
      00000E 80 2C            [24]  291 	sjmp	sdcc_atomic_exchange_exit
      000010 00               [12]  292 	nop
      000011 00               [12]  293 	nop
      000012                        294 sdcc_atomic_exchange_xdata_impl:
      000012 E0               [24]  295 	movx	a, @dptr
      000013 FB               [12]  296 	mov	r3, a
      000014 EA               [12]  297 	mov	a, r2
      000015 F0               [24]  298 	movx	@dptr, a
      000016 80 24            [24]  299 	sjmp	sdcc_atomic_exchange_exit
      000018                        300 sdcc_atomic_compare_exchange_idata_impl:
      000018 E6               [12]  301 	mov	a, @r0
      000019 B5 02 02         [24]  302 	cjne	a, ar2, .+#5
      00001C EB               [12]  303 	mov	a, r3
      00001D F6               [12]  304 	mov	@r0, a
      00001E 22               [24]  305 	ret
      00001F 00               [12]  306 	nop
      000020                        307 sdcc_atomic_compare_exchange_pdata_impl:
      000020 E2               [24]  308 	movx	a, @r0
      000021 B5 02 02         [24]  309 	cjne	a, ar2, .+#5
      000024 EB               [12]  310 	mov	a, r3
      000025 F2               [24]  311 	movx	@r0, a
      000026 22               [24]  312 	ret
      000027 00               [12]  313 	nop
      000028                        314 sdcc_atomic_compare_exchange_xdata_impl:
      000028 E0               [24]  315 	movx	a, @dptr
      000029 B5 02 02         [24]  316 	cjne	a, ar2, .+#5
      00002C EB               [12]  317 	mov	a, r3
      00002D F0               [24]  318 	movx	@dptr, a
      00002E 22               [24]  319 	ret
      00002F                        320 sdcc_atomic_exchange_rollback_end::
                                    321 
      00002F                        322 sdcc_atomic_exchange_gptr_impl::
      00002F 30 F6 E0         [24]  323 	jnb	b.6, sdcc_atomic_exchange_xdata_impl
      000032 A8 82            [24]  324 	mov	r0, dpl
      000034 20 F5 D3         [24]  325 	jb	b.5, sdcc_atomic_exchange_pdata_impl
      000037                        326 sdcc_atomic_exchange_idata_impl:
      000037 EA               [12]  327 	mov	a, r2
      000038 C6               [12]  328 	xch	a, @r0
      000039 F5 82            [12]  329 	mov	dpl, a
      00003B 22               [24]  330 	ret
      00003C                        331 sdcc_atomic_exchange_exit:
      00003C 8B 82            [24]  332 	mov	dpl, r3
      00003E 22               [24]  333 	ret
      00003F                        334 sdcc_atomic_compare_exchange_gptr_impl::
      00003F 30 F6 E6         [24]  335 	jnb	b.6, sdcc_atomic_compare_exchange_xdata_impl
      000042 A8 82            [24]  336 	mov	r0, dpl
      000044 20 F5 D9         [24]  337 	jb	b.5, sdcc_atomic_compare_exchange_pdata_impl
      000047 80 CF            [24]  338 	sjmp	sdcc_atomic_compare_exchange_idata_impl
                                    339 ;--------------------------------------------------------
                                    340 ; global & static initialisations
                                    341 ;--------------------------------------------------------
                                    342 	.area HOME    (CODE)
                                    343 	.area GSINIT  (CODE)
                                    344 	.area GSFINAL (CODE)
                                    345 	.area GSINIT  (CODE)
                                    346 	.globl __sdcc_gsinit_startup
                                    347 	.globl __sdcc_program_startup
                                    348 	.globl __start__stack
                                    349 	.globl __mcs51_genXINIT
                                    350 	.globl __mcs51_genXRAMCLEAR
                                    351 	.globl __mcs51_genRAMCLEAR
                                    352 	.area GSFINAL (CODE)
      0000A5 02 00 49         [24]  353 	ljmp	__sdcc_program_startup
                                    354 ;--------------------------------------------------------
                                    355 ; Home
                                    356 ;--------------------------------------------------------
                                    357 	.area HOME    (CODE)
                                    358 	.area HOME    (CODE)
      000049                        359 __sdcc_program_startup:
      000049 02 00 A8         [24]  360 	ljmp	_main
                                    361 ;	return from main will return to caller
                                    362 ;--------------------------------------------------------
                                    363 ; code
                                    364 ;--------------------------------------------------------
                                    365 	.area CSEG    (CODE)
                                    366 ;------------------------------------------------------------
                                    367 ;Allocation info for local variables in function 'main'
                                    368 ;------------------------------------------------------------
                                    369 ;mysymb        Allocated with name '_main_mysymb_10000_2'
                                    370 ;z             Allocated to registers 
                                    371 ;------------------------------------------------------------
                                    372 ;	main.c:3: void main(void){
                                    373 ;	-----------------------------------------
                                    374 ;	 function main
                                    375 ;	-----------------------------------------
      0000A8                        376 _main:
                           000007   377 	ar7 = 0x07
                           000006   378 	ar6 = 0x06
                           000005   379 	ar5 = 0x05
                           000004   380 	ar4 = 0x04
                           000003   381 	ar3 = 0x03
                           000002   382 	ar2 = 0x02
                           000001   383 	ar1 = 0x01
                           000000   384 	ar0 = 0x00
                                    385 ;	main.c:4: unsigned char mysymb[] = "012345ABCD";
      0000A8 75 08 30         [24]  386 	mov	_main_mysymb_10000_2,#0x30
      0000AB 75 09 31         [24]  387 	mov	(_main_mysymb_10000_2 + 0x0001),#0x31
      0000AE 75 0A 32         [24]  388 	mov	(_main_mysymb_10000_2 + 0x0002),#0x32
      0000B1 75 0B 33         [24]  389 	mov	(_main_mysymb_10000_2 + 0x0003),#0x33
      0000B4 75 0C 34         [24]  390 	mov	(_main_mysymb_10000_2 + 0x0004),#0x34
      0000B7 75 0D 35         [24]  391 	mov	(_main_mysymb_10000_2 + 0x0005),#0x35
      0000BA 75 0E 41         [24]  392 	mov	(_main_mysymb_10000_2 + 0x0006),#0x41
      0000BD 75 0F 42         [24]  393 	mov	(_main_mysymb_10000_2 + 0x0007),#0x42
      0000C0 75 10 43         [24]  394 	mov	(_main_mysymb_10000_2 + 0x0008),#0x43
      0000C3 75 11 44         [24]  395 	mov	(_main_mysymb_10000_2 + 0x0009),#0x44
      0000C6 75 12 00         [24]  396 	mov	(_main_mysymb_10000_2 + 0x000a),#0x00
                                    397 ;	main.c:6: for(z = 0;z <= 255;z++){
      0000C9 7F 00            [12]  398 	mov	r7,#0x00
      0000CB                        399 00102$:
                                    400 ;	main.c:7: P1 = mysymb[z];
      0000CB EF               [12]  401 	mov	a,r7
      0000CC 24 08            [12]  402 	add	a, #_main_mysymb_10000_2
      0000CE F9               [12]  403 	mov	r1,a
      0000CF 87 90            [24]  404 	mov	_P1,@r1
                                    405 ;	main.c:6: for(z = 0;z <= 255;z++){
      0000D1 0F               [12]  406 	inc	r7
                                    407 ;	main.c:9: }
      0000D2 80 F7            [24]  408 	sjmp	00102$
                                    409 	.area CSEG    (CODE)
                                    410 	.area CONST   (CODE)
                                    411 	.area XINIT   (CODE)
                                    412 	.area CABS    (ABS,CODE)
