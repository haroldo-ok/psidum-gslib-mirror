

; == Common Routines.
; == Some copied from Maxim (if it ain't broke...)
; ===================================================================================================

.bank 0 slot 0
.section "Generic Routines" free


; == Sets the VDP address
; Parameters: hl = address

; Affects: 	  a, hl
SetVDPAddress:				ld 	a, l
							out (VDP_CONTROL_PORT),a
							ld	a, h
							out (VDP_CONTROL_PORT),a
						        
							ret



; == Copies data to the VDP
; Parameters: hl = data address
;			  bc = data length

; Affects: 	  a, hl, bc
CopyToVDP:					ld a, (hl)    ; Get data byte
							out (VDP_DATA_PORT), a
							inc hl       ; Point to next letter
							dec bc
							ld a, b
							or c
							jr nz, CopyToVDP
							    
							ret
							
							
							
; == Clears user specified location and length of vdp vram
; Parameters: hl = vram address
;			  bc = size to clear (in bytes)

; Affects: 	  a, hl, bc
ClearVRAM:					call SetVDPAddress
-:							xor a
							out (VDP_DATA_PORT), a
							dec bc
							ld a, b
							or c
							jr nz, -
    
							ret	



; Clears user specified area of ram (can be $0000 - $FFFF)
; Parameters: hl = ram address
;			  bc - size to clear (in bytes)

; Affects: 	  a, hl, bc
ClearRAM:					xor a
							ld (hl), a
							dec bc
							ld a, b
							or c
							jr nz, ClearRAM
							
							ret
							
							
; Outs ascii text to vdp. Copied and modified from Maxim
; $FF = end of text.
; Parameters: hl = text address 	

; Affects:	  a, hl
WriteTextToVDP:				ld a,(hl)
						    cp $ff
						    ret z
						    out (VDP_DATA_PORT),a
						    xor a
						    out (VDP_DATA_PORT),a
						    inc hl
						    jr WriteTextToVDP
						    
						
						
CallHL:                     jp (hl)





/* ===================================================================================================
 * ** 256 Long Block of outi
 * - Call as follows; call OutBlock - n (n being number of outi to complete)
 */                   
                            .repeat 256
                            outi
                            .endr
OutBlock:                   ret
                            



genericPushInterrupt:         
                            in a, (VDP_CONTROL_PORT)
                            inc sp   ; << event handler
                            inc sp
                            ei 
                            reti
genericPushInterruptEnd:



;
;; takes three scanlines to sync z80 to vdp.
;; not perfect, best currently known.
;; written by...  calindro + psidum
;vdpSyncLineInterrupt:       ; call vdpSyncLineInterrupt     ; 17
;                            in a, (V_COUNTER_PORT)          ; 11
;                            ld e, a                         ; 4
;                            ld a, LINE_INTERRUPT_OFF        ; 7
;                            out (VDP_CONTROL_PORT), a       ; 11
;                            ld a, $80                       ; 7
;                            out (VDP_CONTROL_PORT), a       ; 11
;                            call PadCycles_150              ; 164 (215)
;                            
;                            ; 215
;                            in a, (V_COUNTER_PORT)          ; 11
;                            cp e                            ; 4
;                            jp z, _syncD7                   ; 10 ... out for d7 (240)
;                            ld e, a                         ; 4
;                            call PadCycles_198              ; 198
;                            
;                            ; 214
;                            in a, (V_COUNTER_PORT)          ; 11
;                            cp e                            ; 4
;                            jp z, _syncD6                   ; 10 ... out for d6 (240)
;                            ld e, a                         ; 4
;                            call PadCycles_198              ; 198
;
;                            ; 213
;                            in a, (V_COUNTER_PORT)          ; 11
;                            cp e                            ; 4
;                            jp z, _syncD5                   ; 10 ... out for d5 (240)
;
;_syncD4:                    call PadCycles_192              ; fall through.
;                            ret   
;
;_syncD7:                    call PadCycles_228
;_syncD6:                    call PadCycles_228         
;_syncD5:                    call PadCycles_193
;                            ret
;
;
;vdpSyncSpecific:            in a, (V_COUNTER_PORT)
;                            nop
;                            cp e
;                            jr nz, vdpSyncSpecific
;                            
;                            call PadCycles_186
;                            
;                            ; == Poll VCounter
;_div1:                      in a, (V_COUNTER_PORT)          ; VCounter Equality?
;                            cp e                            
;                            jp nz, +                        
;                            
;                            ; == Value Is Same +8
;                            inc a
;                            ld e, a                   
;                            call PadCycles_193              
;                            jp _div2 
;                            
;                            ; == Value Is Different -8
;+:                          ld e, a
;                            call PadCycles_191       
;                            
;                            
;                            
;                            ; == Poll VCounter
;_div2:                      in a, (V_COUNTER_PORT)          ; VCounter Equality?
;                            cp e                            
;                            jp nz, +                        
;                            
;                            ; == Value Is Same +4
;                            inc a
;                            ld e, a                   
;                            call PadCycles_189              
;                            jp _div3 
;                            
;                            ; == Value Is Different -4
;+:                          ld e, a
;                            call PadCycles_195       
;                            
;                            
;                            
;                            ; == Poll VCounter
;_div3:                      in a, (V_COUNTER_PORT)          ; VCounter Equality?
;                            cp e                            
;                            jp nz, +                        
;                            
;                            ; == Value Is Same +2
;                            inc a
;                            ld e, a                   
;                            call PadCycles_187              
;                            jp _div4 
;                            
;                            ; == Value Is Different -2
;+:                          ld e, a
;                            call PadCycles_197    
;
;
;
;                            ; == Poll VCounter
;_div4:                      in a, (V_COUNTER_PORT)          ; VCounter Equality?
;                            cp e                            
;                            jp nz, +                        
;                            
;                            ; == Value Is Same +1
;                            inc a
;                            ld e, a                   
;                            call PadCycles_186            
;                            jp _div5 
;                            
;                            ; == Value Is Different -1
;+:                          ld e, a
;                            call PadCycles_198   
;                            
;                            
;                            ; == Poll VCounter
;_div5:                      in a, (V_COUNTER_PORT)          ; VCounter Equality?
;                            cp e                            
;                            jp nz, +                        
;                            
;                            ; == Sync is + 2
;                            call PadCycles_193
;                            ret
;                            
;                            ; == Sync is +1
;+:                          call PadCycles_192
;                            ret                                 
;                            
;
;

;
;
;
;; == Padding Routines
;; == Minimum cycle cost of instruction is 4. 4 sections of code with offsets off 4 cycles to create
;; == unique call costs from 45 cycles to 233 cycles.
;; ===================================================================================================
;
;PadCycles_278:              nop   
;PadCycles_274:              nop  
;PadCycles_270:              nop  
;PadCycles_266:              nop  
;PadCycles_262:              nop  
;PadCycles_258:              nop 
;PadCycles_254:              nop 
;PadCycles_250:              nop  
;PadCycles_246:              nop   
;PadCycles_242:              nop   
;PadCycles_238:              nop   
;PadCycles_234:              nop   
;PadCycles_230: 				nop   ; -52 | 230
;PadCycles_226: 				nop   ; -51 | 226
;PadCycles_222: 				nop   ; -50 | 222
;PadCycles_218: 				nop   ; -49 | 218
;PadCycles_214: 				nop   ; -48 | 214
;PadCycles_210: 				nop   ; -47 | 210
;PadCycles_206: 				nop   ; -46 | 206
;PadCycles_202: 				nop   ; -45 | 202
;PadCycles_198: 				nop   ; -44 | 198
;PadCycles_194: 				nop   ; -43 | 194
;PadCycles_190: 				nop   ; -42 | 190
;PadCycles_186: 				nop   ; -41 | 186
;PadCycles_182: 				nop   ; -40 | 182
;PadCycles_178: 				nop   ; -39 | 178
;PadCycles_174: 				nop   ; -38 | 174
;PadCycles_170: 				nop   ; -37 | 170
;PadCycles_166: 				nop   ; -36 | 166
;PadCycles_162: 				nop   ; -35 | 162
;PadCycles_158: 				nop   ; -34 | 158
;PadCycles_154: 				nop   ; -33 | 154
;PadCycles_150: 				nop   ; -32 | 150
;PadCycles_146: 				nop   ; -31 | 146
;PadCycles_142: 				nop   ; -30 | 142
;PadCycles_138: 				nop   ; -29 | 138
;PadCycles_134: 				nop   ; -28 | 134
;PadCycles_130: 				nop   ; -27 | 130
;PadCycles_126: 				nop   ; -26 | 126
;PadCycles_122: 				nop   ; -25 | 122
;PadCycles_118: 				nop   ; -24 | 118
;PadCycles_114: 				nop   ; -23 | 114
;PadCycles_110: 				nop   ; -22 | 110
;PadCycles_106: 				nop   ; -21 | 106
;PadCycles_102: 				nop   ; -20 | 102
;PadCycles_98:  				nop   ; -19 | 98
;PadCycles_94:  				nop   ; -18 | 94
;PadCycles_90:  				nop   ; -17 | 90
;PadCycles_86:  				nop   ; -16 | 86
;PadCycles_82:  				nop   ;- 15 | 82
;PadCycles_78:  				nop   ; -14 | 78
;PadCycles_74:  				nop   ; -13 | 74
;PadCycles_70:  				nop   ; -12 | 70
;PadCycles_66:  				nop   ; -11 | 66
;PadCycles_62:  				nop   ; -10 | 62
;PadCycles_58:  				nop   ; -9  | 58
;PadCycles_54:  				nop   ; -8  | 54
;PadCycles_50:  				nop   ; -7  | 50
;PadCycles_46:  				nop   ; -6  | 46
;PadCycles_42:  				nop   ; -5  | 42
;PadCycles_38:  				nop   ; -4  | 38
;PadCycles_34:  				cp 1  ;     | 34
;PadCycles_27:  				ret   ;     | 27
;
;	
;	
;	
;	
;PadCycles_275:              nop
;PadCycles_271:              nop
;PadCycles_267:              nop
;PadCycles_263:              nop
;PadCycles_259:              nop
;PadCycles_255:              nop
;PadCycles_251:              nop
;PadCycles_247:              nop
;PadCycles_243:              nop
;PadCycles_239:              nop
;PadCycles_235:              nop
;PadCycles_231: 				nop
;PadCycles_227: 				nop
;PadCycles_223: 				nop
;PadCycles_219: 				nop
;PadCycles_215: 				nop
;PadCycles_211: 				nop
;PadCycles_207: 				nop
;PadCycles_203: 				nop
;PadCycles_199: 				nop
;PadCycles_195: 				nop
;PadCycles_191: 				nop
;PadCycles_187: 				nop
;PadCycles_183: 				nop
;PadCycles_179: 				nop
;PadCycles_175: 				nop
;PadCycles_171: 				nop
;PadCycles_167: 				nop
;PadCycles_163: 				nop
;PadCycles_159: 				nop
;PadCycles_155: 				nop
;PadCycles_151: 				nop
;PadCycles_147: 				nop
;PadCycles_143: 				nop
;PadCycles_139: 				nop
;PadCycles_135: 				nop
;PadCycles_131: 				nop
;PadCycles_127: 				nop
;PadCycles_123: 				nop
;PadCycles_119: 				nop
;PadCycles_115: 				nop
;PadCycles_111: 				nop
;PadCycles_107: 				nop
;PadCycles_103: 				nop
;PadCycles_99:  				nop
;PadCycles_95:  				nop
;PadCycles_91:  				nop
;PadCycles_87:  				nop
;PadCycles_83:  				nop
;PadCycles_79:  				nop
;PadCycles_75:  				nop
;PadCycles_71:  				nop
;PadCycles_67:  				nop
;PadCycles_63:  				nop
;PadCycles_59:  				nop
;PadCycles_55:  				nop
;PadCycles_51:  				nop
;PadCycles_47:  				nop
;PadCycles_43:  				nop
;PadCycles_39:  				nop
;PadCycles_35:  				nop
;PadCycles_31:  				nop
;			   				ret
;
;
;
;PadCycles_276:              nop
;PadCycles_272:              nop
;PadCycles_268:              nop
;PadCycles_264:              nop
;PadCycles_260:              nop
;PadCycles_256:              nop
;PadCycles_252:              nop
;PadCycles_248:              nop
;PadCycles_244:              nop
;PadCycles_240:              nop
;PadCycles_236:              nop
;PadCycles_232: 				nop
;PadCycles_228: 				nop
;PadCycles_224: 				nop
;PadCycles_220: 				nop
;PadCycles_216: 				nop
;PadCycles_212: 				nop
;PadCycles_208: 				nop
;PadCycles_204: 				nop
;PadCycles_200: 				nop
;PadCycles_196: 				nop
;PadCycles_192: 				nop
;PadCycles_188: 				nop
;PadCycles_184: 				nop
;PadCycles_180: 				nop
;PadCycles_176: 				nop
;PadCycles_172: 				nop
;PadCycles_168: 				nop
;PadCycles_164: 				nop
;PadCycles_160: 				nop
;PadCycles_156: 				nop
;PadCycles_152: 				nop
;PadCycles_148: 				nop
;PadCycles_144: 				nop
;PadCycles_140: 				nop
;PadCycles_136: 				nop
;PadCycles_132: 				nop
;PadCycles_128: 				nop
;PadCycles_124: 				nop
;PadCycles_120: 				nop
;PadCycles_116: 				nop
;PadCycles_112: 				nop
;PadCycles_108: 				nop
;PadCycles_104: 				nop
;PadCycles_100: 				nop
;PadCycles_96:  				nop
;PadCycles_92:  				nop
;PadCycles_88:  				nop
;PadCycles_84:  				nop
;PadCycles_80:  				nop
;PadCycles_76:  				nop
;PadCycles_72:  				nop
;PadCycles_68:  				nop
;PadCycles_64:  				nop
;PadCycles_60:  				nop
;PadCycles_56:  				nop
;PadCycles_52:  				nop
;PadCycles_48:  				nop
;PadCycles_44:  				nop
;PadCycles_40:  				nop
;PadCycles_36:  				scf
;			   				ret nc
;			   				ret
;
;
;PadCycles_277:              nop
;PadCycles_273:              nop
;PadCycles_269:              nop
;PadCycles_265:              nop
;PadCycles_261:              nop
;PadCycles_257:              nop
;PadCycles_253:              nop
;PadCycles_249:              nop
;PadCycles_245:              nop
;PadCycles_241:              nop
;PadCycles_237:              nop
;PadCycles_233: 				nop
;PadCycles_229: 				nop
;PadCycles_225: 				nop
;PadCycles_221: 				nop
;PadCycles_217: 				nop
;PadCycles_213: 				nop
;PadCycles_209: 				nop
;PadCycles_205: 				nop
;PadCycles_201: 				nop
;PadCycles_197: 				nop
;PadCycles_193: 				nop
;PadCycles_189: 				nop
;PadCycles_185: 				nop
;PadCycles_181: 				nop
;PadCycles_177: 				nop
;PadCycles_173: 				nop
;PadCycles_169: 				nop
;PadCycles_165: 				nop
;PadCycles_161: 				nop
;PadCycles_157: 				nop
;PadCycles_153: 				nop
;PadCycles_149: 				nop
;PadCycles_145: 				nop
;PadCycles_141: 				nop
;PadCycles_137: 				nop
;PadCycles_133: 				nop
;PadCycles_129: 				nop
;PadCycles_125: 				nop
;PadCycles_121: 				nop
;PadCycles_117: 				nop
;PadCycles_113: 				nop
;PadCycles_109: 				nop
;PadCycles_105: 				nop
;PadCycles_101: 				nop
;PadCycles_97:  				nop
;PadCycles_93:  				nop
;PadCycles_89:  				nop
;PadCycles_85:  				nop
;PadCycles_81:  				nop
;PadCycles_77:  				nop
;PadCycles_73:  				nop
;PadCycles_69:  				nop
;PadCycles_65:  				nop
;PadCycles_61:  				nop
;PadCycles_57:  				nop
;PadCycles_53:  				nop
;PadCycles_49:  				nop
;PadCycles_45:  				nop
;			   				cp 1
;			   				cp 1
;			   				ret

.ends
