
.memorymap
defaultslot 0
slotsize $4000
slot 0 $0000
slot 1 $4000
slot 2 $8000
slot 3 $c000
.endme
 

.rombankmap
bankstotal 4
banksize $4000
banks 4
.endro


.sdsctag 0.1,"GSE", "Generic Scroll Engine","Psidum" 



; == RAM SETUP
; =====================================

; stack pointer @ $dff0 < - space given for 60 16 bit entries.
; interrupt handler @ de7f > - 256 bytes given.
.define InteruptHandler     $DE80
.define CoreSupport         $DE7F
.define ScratchPad          $DE00 ; used as generic ram space for various routines! 64 bytes
.define RAMJump             $DDFD
.define EffectRAM           $DDFC ; effects will use memory $DDFC descending

.enum CoreSupport desc      ; general hardware
    pageBuffer1 db          ; paging (everdrive bug on slot 0. only 1 & 2)
    pageBuffer2 db
    stackSwapBufffer dw     ; used to store stack location when using stack for other things.
.ende


; == INCLUDES GENERIC
; =====================================
.include "core/defines.inc"
.include "core/interupts.inc"
.include "core/generic_routines.inc"
.include "core/macros.inc"
.include "libs/GSElib.inc"
.include "libs/VDPlib.inc"


; == ASCII SETUP
; =====================================
.asciitable
map " " to "~" = 0
.enda


; == START UP
; =====================================
.bank 0 slot 0
.section "Initialize SMS" free

InitializeSMS:              ld sp, $DFF0                       ; set up stack pointer

                            VDPRegisterWrite 1, DISPLAY_OFF
                            InitaliseVDPRegisters
                            InitaliseGeneral
                            
                            ClearVRAM $4000, $4000
                            ClearVRAM $C000, $0032
                            
                            
                            
                            CartRam0Enable
                            ClearRAM $8000, $4000
                            CopyToRAM $8000, Metatiles, MetatilesEnd - Metatiles
                            
                            
                            ld hl, ($8000)
                            ld de, $8000
                            add hl, de
                            push hl
                            ex de, hl
                            
                            ld hl, Scrolltable
                            ld bc, ScrolltableEnd - Scrolltable
                            ldir
                            
                            
                            WriteToVDP $4000, tiles, tilesEnd - tiles
                            WriteToVDP $C000, palette, paletteEnd - palette
                            
                            pop hl
                            call GSE_InitaliseMap
                            
                            ld hl, 0
                            ld de, 0
                            call GSE_PositionWindow
                            
                            
                            xor a
                            ld (GSE_XUpdateRequest), a
                            ld (GSE_YUpdateRequest), a
                            
                            ; == write interrupt handler to ram
                            LoadInterruptHandler genericPushInterrupt, genericPushInterruptEnd - genericPushInterrupt  
                            
                            call GSE_RefreshScreen
                            
--:                         ld hl, _INT
                            push hl
                            
                            

                            call GSE_ActiveDisplayRoutine
                            
                            
                             
;                            ld hl, 24
;                            ld de, 24
;                            call GSE_MetatileLookup
;                            
;                            ex af, af'
;                            ld a, $38
;                            ld (hl), a
;                            ;inc a
;                            ex af, af'
;                            call GSE_MetatileUpdate
;                            
;                            
;                            ld hl, 65  
;                            ld de, 65
;                            call GSE_MetatileLookup
;                            
;                            ex af, af'
;                            ld a, $38
;                            ld (hl), a
;                            ;inc a
;                            ex af, af'
;                            call GSE_MetatileUpdate
                            
                            
                            
                            
                            
                            
                            
                            
                            VDPRegisterWrite 0, SCROLL_MASK_ON
                            VDPRegisterWrite 1, DISPLAY_ON | FRAME_INTERRUPT_ON
                            in a, (VDP_CONTROL_PORT)
                            ei
                            -: halt
                            jr -
                            
_INT:                       call GSE_VBlankRoutine
                            di
                            
                            ld a, (GSE_X)
                            neg
                            out (VDP_CONTROL_PORT), a
                            ld a, $88
                            out (VDP_CONTROL_PORT), a
                            
                            ld a, (GSE_Y224)
                            out (VDP_CONTROL_PORT), a
                            ld a, $89
                            out (VDP_CONTROL_PORT), a 
                            
                                                                ; == process user input
                            in a, ($dc)                         ; get joypad 1 input      
                            ld d, a                      
                            
_joypad_test_b2:            ; == Test input for reset (button 2)
                            bit 5, a                            ; condition ::  button 2 pressed?
                            jp z, $0000                         ; yes :: reset console to menu
                            
                            
_joypad_test_b1:            ; == Test input for button 1 (while pressing 1 direction input effects x scroll)
                            ;bit 4, a                            ; condition ::  button 2 pressed?
                            ;jp nz, _joypad_test_left
                            xor a
                            ld (GSE_XUpdateRequest), a
                            ld (GSE_YUpdateRequest), a
                            
                            
                            ; == Button 1 IS pressed, input effects scrolling.
                            ;ld a, i
_joypad_test_left1:         bit 2, d                            ; condition :: is left pressed?
                            jr nz, _joypad_test_right1                    
                            
                                ld a, -1
                                ld (GSE_XUpdateRequest), a
                                jp _joypad_test_up1
              
              
_joypad_test_right1:        bit 3, d                            ; condition :: is right pressed?
                            jr nz, _joypad_test_up1
                            
                                ld a, 1
                                ld (GSE_XUpdateRequest), a
                                ;jp _joypad_test_end
                                    
                                    
_joypad_test_up1:           bit 0, d                            ; condition :: is up pressed?
                            jr nz, _joypad_test_down1                    
                            
                                ld a, -1
                                ld (GSE_YUpdateRequest), a
                                jp _joypad_test_end
                                
                                
_joypad_test_down1:           bit 1, d                            ; condition :: is up pressed?
                                jr nz, _joypad_test_end                    
                            
                                ld a, 1
                                ld (GSE_YUpdateRequest), a
                                ;jp _joypad_test_end                                


_joypad_test_end:            
                            
                            
                            jp --
                                
                            
                            -: halt
                            jr -

.ends

.bank 1 slot 1
.section "data" free
Scrolltable:
.dw $0900
.dw $0040
.dw $0024
.dw $0400
.dw $0240
.dw $0340
.db %00000001


.db $08, $10, $10, $08, $08, $08, $10, $10, $10, $10, $10, $10, $10, $10, $08, $08, $08, $08, $08, $10, $10, $10, $10, $10, $18, $08, $10, $10, $10, $10, $10, $08
.db $20, $20, $20, $20, $28, $28, $28, $30, $28, $30, $28, $28, $28, $30, $20, $20, $20, $20, $20, $20, $28, $28, $28, $30, $28, $30, $28, $28, $28, $20, $20, $20
.db $08, $10, $10, $08, $08, $08, $10, $10, $10, $10, $10, $10, $10, $10, $38, $38, $38, $38, $38, $10, $10, $10, $10, $10, $18, $08, $10, $10, $10, $10, $10, $08
.db $20, $20, $20, $40, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $50, $20, $20, $20, $20, $40, $48, $48, $48, $48, $48, $48, $48, $48, $48, $50, $20, $20
.db $08, $10, $10, $08, $08, $08, $10, $10, $10, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $08, $18, $38, $10, $10, $10, $10, $10, $08
.db $20, $20, $40, $48, $48, $48, $48, $60, $48, $60, $48, $48, $48, $48, $48, $50, $28, $28, $40, $48, $48, $48, $48, $68, $48, $68, $48, $48, $48, $48, $50, $20
.db $08, $10, $10, $38, $38, $38, $10, $10, $10, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $78, $70, $70, $18, $10, $10, $10, $10, $10, $10, $08
.db $20, $80, $48, $48, $48, $48, $48, $88, $48, $88, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $88, $48, $88, $48, $48, $48, $48, $48, $90
.db $08, $10, $10, $10, $10, $10, $10, $10, $10, $18, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $08
.db $20, $80, $48, $48, $48, $48, $60, $48, $98, $48, $60, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $68, $48, $98, $48, $68, $48, $48, $48, $48, $90
.db $08, $10, $10, $10, $10, $10, $10, $10, $10, $18, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $08
.db $20, $80, $48, $48, $48, $48, $88, $48, $48, $48, $88, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $88, $48, $48, $48, $88, $48, $48, $48, $48, $20
.db $08, $10, $10, $10, $10, $10, $10, $10, $10, $18, $10, $10, $10, $10, $08, $08, $08, $10, $08, $08, $10, $08, $08, $10, $08, $08, $10, $98, $08, $10, $10, $08
.db $20, $20, $A0, $48, $48, $48, $48, $60, $48, $60, $48, $48, $48, $48, $A8, $B0, $B0, $B0, $A0, $48, $48, $48, $48, $68, $48, $68, $48, $48, $48, $48, $A8, $20
.db $08, $10, $10, $10, $10, $10, $10, $10, $10, $18, $10, $10, $10, $10, $38, $38, $38, $10, $38, $38, $10, $38, $38, $10, $38, $38, $10, $10, $38, $10, $10, $08
.db $20, $20, $20, $A0, $48, $48, $48, $88, $48, $88, $48, $48, $48, $A8, $20, $20, $20, $20, $20, $A0, $48, $48, $48, $88, $48, $88, $48, $48, $48, $A8, $20, $20
.db $08, $10, $10, $10, $10, $10, $10, $08, $08, $18, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $08
.db $20, $20, $20, $20, $B0, $B0, $B8, $48, $C0, $B0, $B0, $B0, $B0, $20, $20, $20, $20, $20, $20, $20, $A0, $A8, $B0, $B0, $A0, $A8, $B0, $B0, $B0, $20, $20, $20
.db $08, $10, $10, $10, $10, $10, $08, $08, $08, $18, $10, $08, $08, $08, $10, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08
.db $20, $20, $20, $20, $20, $20, $20, $C8, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20
.db $08, $10, $10, $10, $10, $10, $08, $08, $08, $18, $10, $08, $08, $08, $10, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08
.db $20, $20, $20, $20, $20, $20, $20, $C8, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20
.db $08, $10, $10, $10, $10, $10, $08, $08, $38, $18, $10, $38, $38, $38, $10, $38, $38, $38, $38, $08, $08, $38, $38, $38, $38, $38, $38, $38, $08, $08, $08, $08
.db $20, $28, $28, $40, $50, $30, $30, $D0, $28, $28, $30, $30, $30, $28, $28, $28, $30, $28, $30, $28, $30, $28, $30, $30, $28, $30, $28, $30, $D8, $30, $28, $20
.db $08, $10, $10, $10, $10, $10, $08, $08, $10, $18, $10, $10, $10, $10, $10, $10, $10, $10, $10, $08, $08, $10, $10, $10, $10, $10, $10, $10, $08, $08, $08, $08
.db $40, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $68, $48, $68, $48, $50
.db $08, $10, $10, $10, $10, $10, $38, $38, $10, $18, $10, $10, $10, $10, $10, $10, $10, $10, $10, $38, $38, $10, $10, $10, $10, $10, $10, $10, $38, $38, $38, $38
.db $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $68, $68, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $68, $68, $68, $48, $48
.db $08, $58, $58, $58, $58, $58, $58, $58, $58, $18, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58, $58
.db $E0, $E0, $E0, $E0, $E0, $48, $48, $48, $48, $68, $48, $68, $48, $48, $48, $68, $68, $48, $48, $68, $68, $48, $48, $48, $48, $48, $48, $88, $88, $88, $48, $48
.db $E8, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F8, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0, $F0
.db $F0, $F0, $F0, $F0, $01, $68, $48, $48, $48, $88, $48, $88, $48, $48, $48, $68, $68, $48, $48, $88, $68, $48, $68, $48, $68, $48, $48, $48, $48, $48, $48, $48
.db $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $98, $10, $10, $11, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $01, $48, $48, $48, $68, $48, $68, $48, $48, $48, $88, $88, $48, $48, $48, $88, $48, $88, $48, $88, $48, $48, $48, $48, $48, $48, $48
.db $E8, $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $10, $11, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $48, $48, $48, $88, $48, $88, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48
.db $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $58, $58, $58, $11, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $68, $48, $48, $48, $48, $48, $48, $48, $48, $68, $68, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48, $48
.db $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F0, $78, $F0, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $68, $48, $48, $48, $68, $48, $48, $48, $48, $68, $68, $68, $68, $68, $68, $68, $68, $68, $68, $68, $68, $68, $68, $68, $68, $68
.db $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $78, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $08, $10, $10, $10, $08, $10, $10, $10, $10, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08
.db $E8, $F8, $F8, $F8, $F8, $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $08, $19, $08, $11, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $38, $10, $10, $10, $38, $10, $10, $10, $10, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08
.db $E8, $F8, $F8, $09, $08, $08, $08, $11, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $08, $08, $21, $08, $19, $08, $21, $10, $19, $19, $19, $08, $11, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $29, $29, $29, $29, $29, $29, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08
.db $E8, $F8, $F8, $09, $08, $98, $38, $11, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $38, $08, $21, $08, $19, $08, $31, $10, $19, $19, $19, $19, $11, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $39, $39, $39, $39, $39, $39, $38, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08
.db $E8, $F8, $F8, $09, $38, $10, $10, $11, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $10, $38, $21, $08, $19, $19, $41, $58, $58, $10, $19, $19, $11, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $38, $38, $38, $38, $38, $38, $10, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08, $08
.db $E8, $F8, $F8, $49, $58, $58, $58, $11, $F8, $09, $08, $08, $08, $08, $11, $F8, $F8, $09, $10, $10, $11, $01, $58, $58, $11, $F0, $01, $58, $58, $58, $11, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $10, $10, $10, $10, $10, $10, $10, $38, $38, $38, $38, $38, $38, $38, $38, $38, $38, $38, $38, $38, $38, $38, $38, $38
.db $E8, $F8, $F8, $F8, $F0, $F0, $F0, $F8, $F8, $09, $08, $08, $38, $38, $31, $31, $31, $31, $08, $10, $11, $F8, $F0, $F0, $F8, $F8, $F8, $F0, $31, $F0, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $29, $29, $29, $29, $29, $29, $10, $08, $08, $08, $08, $08, $08, $10, $10, $08, $08, $10, $08, $10, $08, $08, $08, $08
.db $E8, $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $08, $38, $10, $10, $51, $59, $59, $61, $58, $58, $21, $08, $10, $10, $11, $F8, $09, $10, $10, $58, $11, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $39, $39, $39, $39, $39, $39, $10, $08, $08, $08, $08, $08, $08, $08, $10, $08, $08, $08, $08, $10, $08, $08, $08, $08
.db $E8, $E8, $E8, $F8, $F8, $F8, $F8, $F8, $F8, $09, $08, $58, $10, $58, $11, $F8, $F8, $F8, $F0, $F0, $09, $58, $58, $58, $31, $31, $31, $58, $58, $49, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $38, $38, $38, $38, $38, $38, $10, $08, $08, $08, $08, $08, $08, $08, $10, $08, $08, $08, $08, $10, $08, $08, $08, $08
.db $E8, $E8, $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F0, $F0, $78, $F0, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F0, $F0, $F0, $59, $59, $59, $F0, $F0, $59, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $10, $10, $10, $10, $10, $10, $10, $08, $08, $08, $08, $08, $08, $08, $10, $08, $08, $08, $08, $10, $08, $08, $08, $08
.db $E8, $E8, $E8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $78, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $10, $10, $10, $10, $10, $10, $10, $08, $08, $08, $08, $08, $08, $08, $10, $08, $08, $08, $08, $10, $08, $08, $08, $08
.db $E8, $E8, $F8, $F8, $09, $10, $29, $10, $10, $11, $F8, $F8, $78, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $29, $29, $29, $29, $29, $29, $10, $08, $08, $08, $38, $08, $08, $08, $10, $08, $08, $08, $08, $10, $08, $08, $08, $08
.db $E8, $F8, $E8, $F8, $09, $10, $39, $10, $10, $11, $F8, $F8, $78, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $39, $39, $39, $39, $39, $39, $10, $38, $38, $38, $10, $38, $38, $38, $10, $38, $08, $08, $08, $10, $38, $38, $38, $38
.db $E8, $F8, $F8, $F8, $09, $58, $38, $58, $58, $11, $F8, $F8, $78, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $38, $38, $38, $38, $38, $38, $10, $08, $08, $08, $08, $08, $10, $08, $08, $10, $08, $08, $08, $10, $10, $10, $10, $10
.db $E8, $E8, $F8, $F8, $F8, $F0, $78, $F0, $49, $F8, $F8, $F8, $78, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $29, $29, $29, $29, $29, $29, $10, $08, $08, $08, $08, $08, $08, $08, $08, $10, $08, $08, $38, $08, $08, $08, $08, $08
.db $E8, $F8, $E8, $F8, $F8, $F8, $78, $F8, $F8, $F8, $F8, $F8, $78, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8, $F8
.db $F8, $F8, $F8, $F8, $F8, $09, $10, $10, $39, $39, $39, $39, $39, $39, $10, $08, $08, $08, $08, $08, $08, $08, $08, $10, $38, $38, $10, $38, $38, $38, $08, $08
ScrolltableEnd:

Metatiles:
.dw $0168
.dw $0000
.dw $0000
.dw $0000
.dw $0000, $0001, $0006, $0007, $0002, $0002, $0002, $0002, $0003, $0003, $0003, $0003, $0004, $0005, $0008, $0009
.dw $0004, $0005, $000A, $000B, $0004, $0005, $000C, $000D, $000E, $020E, $0002, $0002, $0004, $000F, $0012, $0013
.dw $0010, $0010, $0010, $0010, $0011, $0005, $0014, $000B, $0002, $0002, $0017, $0017, $0015, $0215, $0018, $0218
.dw $0016, $0216, $0019, $001A, $001B, $001B, $0003, $0003, $001C, $021C, $001C, $021C, $0004, $001D, $0008, $0020
.dw $001E, $021E, $0010, $0010, $001F, $0005, $0021, $0009, $0022, $0023, $0024, $0025, $0026, $0027, $0008, $0009
.dw $0028, $0029, $002A, $0009, $0026, $0029, $0008, $0009, $002B, $0210, $0008, $002D, $0010, $002C, $002E, $0009
.dw $002F, $0030, $0031, $0032, $002F, $0030, $0034, $0035, $0033, $0033, $0033, $0033, $0010, $0010, $0036, $0036
.dw $0037, $0038, $003C, $003D, $0039, $0039, $003A, $003A, $003A, $003A, $003A, $003A, $0039, $003B, $003A, $003E
.dw $003A, $003E, $003A, $003E, $063E, $003A, $063E, $003A, $003F, $0040, $0041, $0042, $063E, $003E, $063E, $003E
.dw $0002, $0002, $0043, $0243, $0044, $0044, $0047, $0047, $0045, $0046, $0048, $0049, $004A, $024A, $063E, $003E
.dw $004B, $024B, $004C, $024C, $004A, $004D, $063E, $003A, $004D, $004D, $003A, $003A, $004D, $024A, $003A, $003E
MetatilesEnd:











.ends

.bank 1 slot 1
.section "tiles" free
tiles:
.db $0D, $0D, $12, $E0, $18, $1A, $47, $80, $30, $B5, $8F, $00, $60, $6E, $1F, $00, $00, $17, $BF, $00, $A0, $AA, $5F, $00, $C0, $D5, $3F, $00, $A0, $AA, $5F, $00
.db $B0, $B0, $40, $07, $08, $48, $F2, $01, $04, $05, $F9, $00, $0A, $AA, $F4, $00, $02, $53, $FD, $00, $0A, $8A, $F4, $00, $05, $25, $FA, $00, $05, $05, $FA, $00
.db $7F, $FF, $FF, $00, $FF, $FF, $FF, $00, $FD, $FF, $FF, $00, $FF, $FF, $FF, $00, $FF, $FF, $FF, $00, $BF, $FF, $FF, $00, $FF, $FF, $FF, $00, $FB, $FF, $FF, $00
.db $FF, $6A, $00, $FF, $FF, $94, $00, $FF, $FF, $25, $00, $FF, $FF, $0A, $04, $FF, $FF, $2C, $00, $FF, $FF, $42, $00, $FF, $FF, $12, $00, $FF, $FF, $B5, $02, $FF
.db $0E, $30, $40, $40, $38, $41, $80, $80, $63, $80, $00, $00, $C9, $12, $00, $00, $1B, $20, $00, $00, $36, $C0, $00, $00, $E4, $00, $01, $01, $C0, $11, $02, $02
.db $34, $48, $00, $00, $48, $93, $00, $00, $98, $21, $02, $02, $B1, $02, $04, $04, $2B, $04, $00, $00, $16, $C8, $00, $00, $49, $90, $00, $00, $B3, $00, $00, $00
.db $D0, $D0, $2F, $00, $20, $20, $5F, $00, $54, $54, $2B, $08, $29, $2F, $10, $00, $58, $5B, $84, $00, $2A, $29, $00, $80, $14, $12, $01, $C1, $0E, $01, $00, $80
.db $0B, $2B, $F4, $00, $02, $02, $FC, $00, $46, $46, $B8, $00, $A0, $A0, $0C, $00, $46, $86, $11, $00, $08, $A8, $40, $41, $30, $D0, $00, $03, $F0, $00, $00, $01
.db $11, $26, $00, $00, $13, $24, $40, $40, $34, $40, $80, $80, $68, $81, $02, $02, $61, $82, $04, $04, $C3, $0C, $00, $00, $8F, $10, $00, $00, $17, $08, $00, $00
.db $8A, $00, $00, $00, $31, $00, $00, $00, $62, $84, $08, $08, $C6, $08, $10, $10, $C4, $0A, $10, $10, $8C, $12, $20, $20, $9A, $24, $00, $00, $32, $4C, $00, $00
.db $C9, $16, $20, $20, $9B, $64, $00, $00, $1F, $A0, $40, $40, $37, $C0, $00, $00, $B6, $40, $00, $00, $E4, $00, $00, $00, $C0, $00, $02, $02, $00, $00, $88, $8F
.db $EF, $00, $00, $00, $D3, $0C, $00, $00, $A7, $18, $00, $00, $5F, $20, $00, $00, $FC, $00, $00, $00, $F8, $00, $00, $00, $F0, $00, $01, $01, $60, $00, $00, $07
.db $C9, $16, $20, $20, $9B, $64, $00, $00, $1F, $A0, $40, $40, $37, $C0, $00, $00, $B6, $40, $00, $00, $E4, $00, $00, $00, $C0, $00, $03, $03, $00, $00, $88, $8F
.db $EF, $00, $00, $00, $CF, $00, $00, $00, $A7, $00, $00, $00, $43, $00, $00, $00, $81, $00, $18, $18, $00, $00, $62, $7E, $00, $00, $01, $FF, $00, $00, $00, $FF
.db $80, $80, $FF, $00, $00, $00, $FF, $00, $E4, $E0, $FF, $00, $FF, $FF, $FF, $00, $DF, $FF, $FF, $00, $FF, $FF, $FF, $00, $FF, $FF, $FF, $00, $F7, $FF, $FF, $00
.db $7F, $80, $00, $00, $FD, $02, $00, $00, $FA, $04, $00, $00, $F2, $0C, $00, $00, $E6, $10, $08, $09, $7C, $80, $00, $01, $5C, $80, $00, $01, $D8, $00, $00, $03
.db $00, $80, $80, $7F, $00, $00, $00, $FF, $00, $02, $02, $FD, $00, $00, $00, $FF, $00, $00, $00, $FF, $00, $40, $40, $BF, $00, $00, $00, $FF, $00, $04, $04, $FB
.db $4F, $30, $80, $80, $BA, $41, $00, $00, $F5, $02, $00, $00, $67, $00, $00, $00, $0F, $00, $80, $80, $0D, $00, $20, $E0, $0A, $00, $01, $E1, $04, $03, $10, $F0
.db $C9, $16, $20, $20, $9B, $64, $00, $00, $1F, $A0, $40, $40, $37, $C0, $00, $00, $B6, $40, $00, $00, $E4, $00, $00, $00, $C0, $00, $00, $02, $00, $00, $00, $8F
.db $E0, $00, $00, $07, $A0, $40, $00, $0F, $60, $80, $00, $0F, $C0, $00, $00, $1F, $C0, $00, $00, $1F, $80, $00, $00, $3F, $00, $00, $00, $7F, $00, $00, $00, $FF
.db $01, $06, $00, $E0, $06, $08, $00, $E0, $0D, $00, $00, $E0, $0B, $00, $00, $E0, $06, $00, $10, $F0, $00, $00, $00, $F0, $00, $00, $02, $FE, $00, $00, $01, $FF
.db $E0, $E0, $E0, $00, $87, $80, $9F, $00, $17, $00, $77, $0F, $0F, $20, $6F, $10, $1F, $20, $7F, $0F, $6F, $10, $7F, $0F, $1E, $22, $7D, $1C, $1F, $20, $7F, $1F
.db $0D, $C2, $C0, $20, $57, $08, $47, $82, $2F, $10, $0F, $80, $5F, $20, $0F, $05, $DF, $20, $9F, $8A, $4F, $30, $0F, $01, $9F, $60, $1F, $04, $AF, $50, $2F, $00
.db $7F, $FF, $FF, $00, $FF, $FF, $FF, $00, $FD, $FF, $FF, $00, $BF, $BF, $FF, $00, $FE, $FE, $FF, $00, $A9, $A9, $FF, $00, $56, $56, $B9, $00, $10, $10, $00, $00
.db $19, $29, $76, $10, $1F, $20, $7F, $1F, $16, $20, $7F, $10, $15, $25, $7A, $10, $1F, $20, $7F, $1F, $2F, $10, $7F, $0F, $47, $40, $3F, $00, $00, $00, $00, $00
.db $DB, $24, $0B, $00, $25, $5A, $01, $01, $50, $2D, $02, $02, $2C, $12, $01, $01, $D8, $07, $80, $00, $2A, $81, $80, $00, $14, $82, $81, $41, $0E, $81, $80, $00
.db $FB, $04, $F8, $80, $FA, $04, $F8, $20, $A6, $58, $A0, $00, $00, $AC, $00, $00, $47, $90, $01, $00, $08, $A0, $40, $41, $30, $C3, $03, $00, $F0, $00, $00, $01
.db $00, $00, $00, $00, $00, $00, $00, $00, $F5, $65, $00, $F5, $FF, $9A, $04, $FF, $FF, $2C, $00, $FF, $FF, $02, $00, $FF, $FF, $22, $00, $FF, $FF, $45, $02, $FF
.db $00, $00, $9F, $DF, $40, $00, $60, $FF, $00, $20, $00, $DF, $00, $9F, $40, $40, $00, $00, $9F, $DF, $40, $00, $60, $FF, $00, $20, $00, $DF, $00, $9F, $40, $40
.db $70, $84, $00, $01, $E0, $0A, $04, $04, $E0, $06, $08, $08, $C0, $16, $08, $08, $00, $0C, $10, $11, $04, $48, $90, $91, $48, $90, $00, $03, $88, $30, $00, $03
.db $00, $00, $7F, $FF, $00, $00, $FF, $FF, $00, $00, $1F, $FD, $00, $00, $00, $FF, $00, $20, $20, $DF, $00, $00, $00, $FF, $00, $00, $00, $FF, $00, $08, $08, $F7
.db $0D, $70, $00, $00, $3A, $41, $00, $00, $21, $02, $00, $80, $0B, $10, $00, $C0, $16, $20, $00, $80, $25, $00, $00, $80, $1A, $00, $01, $81, $28, $11, $02, $02
.db $90, $20, $00, $07, $68, $00, $00, $03, $50, $84, $08, $09, $C4, $08, $10, $11, $8C, $30, $00, $01, $90, $28, $00, $03, $28, $50, $00, $03, $58, $A0, $00, $03
.db $11, $66, $00, $00, $17, $20, $40, $40, $28, $40, $00, $00, $18, $01, $02, $82, $31, $02, $04, $84, $03, $0C, $00, $C0, $0F, $10, $00, $C0, $37, $08, $00, $80
.db $FF, $00, $FF, $FF, $80, $40, $BF, $BF, $C0, $3F, $80, $80, $BF, $00, $80, $80, $80, $00, $80, $80, $80, $00, $80, $80, $81, $00, $81, $81, $81, $00, $81, $81
.db $FF, $00, $FF, $FF, $01, $02, $FD, $FD, $03, $FC, $01, $01, $FD, $00, $01, $01, $11, $02, $15, $1D, $11, $02, $15, $1D, $11, $22, $55, $DD, $11, $22, $55, $DD
.db $91, $02, $95, $9D, $91, $02, $95, $9D, $91, $22, $D5, $DD, $91, $22, $D5, $DD, $A2, $44, $88, $99, $82, $4C, $91, $91, $8C, $11, $80, $80, $FF, $00, $FF, $FF
.db $11, $22, $55, $DD, $11, $22, $55, $DD, $11, $22, $55, $DD, $11, $22, $55, $DD, $23, $44, $89, $99, $23, $CC, $11, $11, $D9, $00, $01, $01, $FF, $00, $FF, $FF
.db $00, $00, $00, $1F, $80, $40, $00, $00, $29, $C6, $00, $00, $62, $8C, $00, $00, $ED, $00, $00, $00, $E3, $00, $00, $00, $CE, $01, $00, $00, $AC, $13, $00, $00
.db $00, $00, $00, $FF, $00, $00, $00, $FF, $00, $00, $00, $0F, $40, $20, $10, $17, $98, $40, $20, $27, $18, $A0, $40, $47, $30, $40, $80, $87, $20, $C0, $00, $01
.db $00, $00, $00, $FF, $00, $00, $00, $FC, $01, $02, $00, $F8, $07, $00, $00, $F0, $06, $00, $00, $F0, $0D, $00, $00, $E0, $0A, $01, $00, $E0, $04, $03, $00, $E0
.db $00, $00, $00, $1F, $C0, $00, $00, $08, $B1, $04, $02, $02, $51, $22, $04, $04, $92, $64, $00, $00, $0A, $A4, $40, $40, $2D, $40, $80, $80, $01, $C0, $00, $00
.db $08, $05, $02, $C2, $11, $0A, $04, $C4, $32, $04, $08, $88, $2E, $10, $00, $80, $5D, $00, $00, $00, $51, $00, $00, $00, $6A, $04, $00, $00, $47, $08, $10, $10
.db $00, $01, $01, $7E, $80, $00, $00, $0F, $D0, $20, $00, $07, $80, $50, $20, $27, $80, $30, $40, $47, $90, $20, $40, $43, $2C, $40, $80, $81, $4A, $90, $00, $00
.db $01, $80, $80, $7E, $07, $00, $00, $F8, $0B, $04, $00, $F0, $19, $02, $04, $E4, $11, $0E, $00, $E0, $13, $0C, $00, $E0, $27, $18, $00, $C0, $2F, $10, $00, $C0
.db $00, $01, $01, $7E, $00, $00, $00, $7F, $00, $40, $40, $3F, $00, $00, $00, $7F, $00, $00, $00, $7F, $80, $02, $02, $3D, $C0, $00, $00, $1F, $E0, $00, $00, $03
.db $00, $80, $80, $7F, $00, $00, $00, $FF, $00, $02, $02, $FD, $00, $00, $00, $FF, $00, $00, $00, $FF, $01, $40, $40, $BE, $07, $00, $00, $F8, $7F, $00, $00, $80
.db $0C, $30, $42, $43, $38, $41, $80, $81, $60, $80, $02, $03, $C8, $11, $00, $01, $18, $20, $02, $03, $34, $C1, $00, $01, $E4, $00, $02, $03, $C0, $11, $00, $01
.db $B4, $08, $00, $00, $48, $13, $80, $80, $98, $21, $02, $02, $71, $02, $84, $84, $AB, $04, $00, $00, $56, $08, $80, $80, $89, $10, $00, $00, $73, $00, $80, $80
.db $10, $24, $02, $03, $10, $25, $40, $41, $34, $40, $82, $83, $68, $81, $00, $01, $60, $80, $06, $07, $C0, $0D, $00, $01, $8C, $10, $02, $03, $14, $09, $00, $01
.db $8A, $00, $00, $00, $71, $00, $80, $80, $A2, $04, $08, $08, $46, $08, $90, $90, $84, $0A, $10, $10, $4C, $12, $A0, $A0, $9A, $24, $00, $00, $72, $0C, $80, $80
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
.db $90, $24, $02, $03, $14, $21, $40, $41, $24, $48, $82, $83, $44, $89, $10, $11, $08, $B0, $02, $03, $B0, $01, $00, $01, $00, $00, $02, $3F, $00, $01, $00, $FD
.db $97, $20, $00, $00, $63, $00, $80, $80, $A0, $04, $08, $08, $45, $0A, $90, $90, $8D, $32, $00, $00, $73, $00, $80, $80, $80, $00, $00, $3C, $40, $00, $80, $BF
.db $00, $80, $80, $7F, $00, $00, $00, $FF, $00, $02, $02, $FD, $00, $40, $00, $BF, $00, $01, $00, $FE, $00, $56, $00, $A9, $46, $EF, $00, $10, $10, $10, $00, $00
.db $CB, $F0, $00, $C3, $85, $59, $20, $A1, $87, $48, $B1, $B1, $03, $8D, $60, $71, $04, $9A, $40, $60, $1D, $60, $02, $02, $70, $85, $00, $02, $80, $05, $C2, $C2
.db $FF, $DC, $00, $FF, $FF, $73, $00, $FF, $FF, $47, $00, $FF, $FF, $19, $80, $FF, $FF, $47, $A0, $FF, $FF, $23, $D0, $FF, $FF, $0C, $60, $7F, $FF, $1B, $60, $7F
.db $FF, $00, $DE, $FF, $FF, $00, $FF, $FF, $FF, $04, $5B, $FF, $FF, $19, $80, $FF, $FF, $67, $00, $FF, $FF, $33, $00, $FF, $FF, $CC, $00, $FF, $FF, $BB, $00, $FF
.db $FF, $DC, $00, $FF, $FF, $73, $00, $FF, $FF, $47, $00, $FF, $FF, $19, $00, $FF, $FF, $47, $00, $FF, $FF, $B3, $00, $FF, $FF, $CC, $00, $FF, $FF, $BB, $00, $FF
.db $FC, $10, $ED, $FC, $FE, $00, $FE, $FE, $FE, $00, $FE, $FE, $FF, $10, $8F, $FF, $FF, $E0, $06, $FF, $FE, $CC, $02, $FE, $FE, $30, $06, $FE, $FE, $D0, $0E, $FE
.db $C0, $0B, $64, $64, $C0, $2D, $C0, $C0, $F5, $00, $F5, $F5, $FF, $18, $23, $FF, $FF, $60, $07, $FF, $FF, $30, $0B, $FF, $FF, $CC, $02, $FF, $FF, $BB, $00, $FF
.db $0B, $30, $00, $03, $05, $58, $21, $21, $87, $48, $B1, $B1, $03, $8D, $60, $70, $86, $18, $C0, $E0, $99, $64, $80, $82, $E9, $04, $82, $82, $C7, $81, $00, $C1
.db $FE, $C8, $14, $FE, $FE, $30, $0E, $FE, $FF, $C8, $06, $7F, $FE, $E0, $0E, $FE, $FE, $90, $0C, $FE, $FE, $E2, $04, $FE, $FE, $C0, $0E, $FE, $FF, $31, $0E, $FF
.db $44, $C4, $FB, $00, $10, $10, $E8, $0F, $47, $40, $B7, $1F, $A2, $A0, $DA, $1F, $40, $40, $BF, $0F, $E7, $E7, $18, $00, $B4, $B4, $CB, $00, $11, $01, $D6, $7C
.db $44, $C4, $FB, $00, $32, $32, $CF, $80, $21, $23, $5E, $C0, $08, $00, $EB, $DC, $5C, $40, $9D, $BE, $1D, $41, $DC, $3E, $08, $00, $EB, $3E, $E1, $E1, $1E, $1C
.db $7D, $01, $FE, $FE, $38, $00, $BB, $FE, $01, $01, $C6, $FE, $82, $82, $7D, $7C, $5E, $5E, $A5, $00, $12, $52, $ED, $00, $6A, $6A, $91, $00, $08, $08, $00, $00
.db $FD, $FD, $42, $00, $92, $92, $6D, $00, $25, $25, $DA, $00, $82, $82, $45, $7C, $38, $00, $BB, $FE, $11, $01, $D6, $FE, $02, $02, $FD, $7C, $08, $08, $00, $00
.db $FF, $FF, $FC, $00, $FC, $FC, $FB, $00, $F8, $F9, $F7, $00, $F8, $FB, $E7, $00, $F0, $F7, $EF, $00, $F0, $F7, $CF, $00, $E0, $EF, $DF, $00, $E0, $E7, $9F, $00
.db $99, $18, $81, $FE, $22, $88, $22, $77, $00, $44, $AA, $BB, $00, $00, $00, $66, $00, $88, $11, $77, $00, $88, $11, $77, $00, $88, $11, $77, $00, $88, $11, $77
.db $40, $CF, $BF, $00, $C0, $D7, $3F, $00, $C0, $CA, $3F, $00, $A0, $A5, $5F, $00, $C0, $C2, $3F, $00, $A0, $A8, $5F, $00, $C0, $C2, $3F, $00, $E8, $E8, $17, $00
.db $03, $EB, $FD, $00, $03, $D3, $FC, $00, $01, $E9, $FE, $00, $03, $43, $FC, $00, $05, $15, $FA, $00, $03, $83, $FC, $00, $05, $25, $FA, $00, $03, $03, $FC, $00
.db $00, $88, $11, $77, $00, $88, $11, $77, $00, $88, $11, $77, $00, $88, $11, $77, $22, $88, $33, $77, $00, $44, $AA, $BB, $00, $00, $00, $66, $00, $99, $66, $66
.db $D0, $D0, $2F, $00, $20, $20, $5F, $00, $54, $54, $2B, $08, $29, $2F, $10, $00, $58, $5B, $84, $00, $AA, $A9, $80, $00, $D4, $D2, $C1, $01, $8E, $81, $80, $00
.db $0B, $2B, $F4, $00, $02, $02, $FC, $00, $46, $46, $B8, $00, $A0, $A0, $0C, $00, $46, $86, $11, $00, $09, $A9, $41, $40, $33, $D3, $03, $00, $F1, $01, $01, $00
.db $00, $00, $00, $00, $00, $00, $00, $00, $3F, $07, $00, $3F, $7F, $19, $20, $7F, $7F, $47, $30, $7F, $FF, $13, $60, $FF, $7F, $0C, $70, $7F, $7F, $13, $28, $7F
.db $7F, $00, $00, $00, $80, $19, $44, $66, $80, $19, $44, $66, $80, $19, $44, $66, $80, $5D, $22, $22, $99, $00, $44, $66, $80, $19, $44, $66, $80, $19, $44, $66
.db $80, $19, $44, $66, $80, $5D, $22, $22, $99, $00, $44, $66, $80, $19, $44, $66, $80, $19, $44, $66, $A2, $19, $22, $66, $99, $66, $00, $00, $66, $00, $00, $00
.db $00, $00, $00, $00, $00, $00, $00, $00, $FF, $B3, $00, $FF, $FF, $CC, $00, $FF, $FF, $BB, $00, $FF, $FF, $DC, $00, $FF, $FF, $73, $00, $FF, $FF, $47, $00, $FF
tilesEnd:


palette:
.db $00, $01, $02, $04, $09, $2A, $2D, $0E, $2F, $34, $17, $38, $1B, $3F
paletteEnd:

.ends

