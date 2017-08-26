

.section "GSELib" free


.define GSEScrolltableHeader $8000
.define GSElibRAM $D000

.define GSEVDPNametableAddress $7800



.define GSE_MetatileBoundary    $C0


.enum GSEScrolltableHeader  asc
                            H_ScrolltableLoc      dw
                            H_MetatileLoc         dw ; not needed
                            H_VerticalLUTLoc      dw
                            H_WidthInMetatiles    dw
                            H_HeightInMetatiles   dw
                            H_WidthInPixels       dw
                            H_HeightInPixel       dw
                            H_HorizontalAddition  dw
                            H_VerticalAddition    dw
                            H_OptionByte          db
                            H_Scrolltable         dw
    
.ende

.enum GSElibRAM asc
                            ScrolltableLoc      dw
                            MetatileLoc         dw
                            VerticalLUTLoc      dw
                            WidthInMetatiles    dw
                            HeightInMetatiles   dw
                            WidthInPixels       dw
                            HeightInPixel       dw
                            HorizontalAddition  dw
                            VerticalAddition    dw
                            OptionByte          db
                            ScrolltablePointer  dw
                            NametablePointer    dw
                            XCurrent            dw
                            YCurrent            dw
                            XUpdateStatus       db
                            YUpdateStatus       db
                            YScroll224          db
                            XUpdate             db
                            YUpdate             db
                            GSE_NATUpdateColumn     dw
                            GSE_NATUpdateRow        dw
                            GSE_NATColumnBuffer     dsb 66
                            GSE_NATRowBuffer        dsb 66  
                            
                            
                            

.ende

.define XXUpdateStatus 27
.define YYUpdateStatus 29



GSEPositionWindow:          ; == Store (x, y) in ram
                            ld (YCurrent), hl
                            ex de, hl
                            ld (XCurrent), hl
                            ex de, hl
                            
                            
                            ; == Get Scrolltable location based on (x, y) and store as pointer
                            call GSEMetatileLookup
                            ld (ScrolltablePointer), hl
                            
                            
                            ; == Get value prepresenting top right of nametable in vram
                            ld hl, (YCurrent)
                            srl h                   ; divide y by 8 (get tile row)
                            rr l
                            srl h
                            rr l
                            srl h
                            rr l
                            
                            ld e, 28                ; mod 28
                            ld b,8
                        -:  adc hl,hl
                            ld a,h
                            jr c,+
                            cp e
                            jr c,++
                        +:  sub e
                            ld h,a
                            or a
                        ++: djnz -
                            ld a,l
                            rla
                            cpl

                            ld l, h                 ; multiply by 64 - length of data in nametable row.
                            xor a
                            ld d, a
                            ld h, a
                            add hl, hl              
                            add hl, hl
                            add hl, hl
                            add hl, hl
                            add hl, hl
                            add hl, hl
                            
                            
                            ld a, (XCurrent)
                            and %00011111
                            rla
                            ld e, a
                            add hl, de
                            
                            ld de, GSEVDPNametableAddress       ; add vdp base address of nametable 
                            add hl, de
                            
                            ld (NametablePointer), hl        ; store result.
                            
                            ret
                            
                           
GSERefreshWindow:                   

/*
 * for each row
 * {
 *      for each metatile
 *      {
 *          resolve id
 *          
 *      }
 * }
 */                    
                            

/* == GSEInitalizeMap
 * Initalises Map Settings.
 */
GSEInitalizeMap:    
_GenerateLUT:               ; == Copy Scrolltable header to RAM
                            CopyToRAM GSElibRAM, GSEScrolltableHeader, 19

                            ; == Generate VerticalLUT if Required.
                            CartRam0Enable
                            ld a, (OptionByte)
                            bit 0, a
                            jr z, +
                            ld ix, (VerticalLUTLoc)
                            ld hl, H_Scrolltable
                            ld bc, (HeightInMetatiles)
                            ld de, (WidthInMetatiles)
-:                          ld (ix+0), h
                            inc ix
                            ld (ix+0), l
                            inc ix
                            add hl, de
                            dec bc
                            ld a, b
                            or c
                            jr nz, -
                            
+:                          ret



ret



GSEMetatileLookup:          ; Convert Y to LUT Entry
                            ld a, %11110000
                            and l
                            srl h
                            rra
                            srl h
                            rra
                            srl h
                            rra
                            ld l, a
                            
                            ld bc, (VerticalLUTLoc)
                            add hl, bc
                            ld a, (hl)                      ; indirection      
                            inc hl                       
                            ld l, (hl)                   
                            ld h, a    


                            ; Convert and add X
                            ld a, e
                            srl d
                            rra
                            srl d
                            rra
                            srl d
                            rra
                            srl d
                            rra
                            ld e, a
                            add hl, de
                            
                            ld a, (hl)
                            ret
                            
GSETileLookup:              ld a, %00000001
                            and l
                            rla
                            rla
                            ld c, a
                            ld a, %00000001
                            and e
                            rla
                            or c
                            push af
                            
                            call GSEMetatileLookup
                            ld l, a
                            xor a
                            ld h, a
                            add hl, hl
                            add hl, hl
                            add hl, hl

                            pop af
                            add a, l
                            ld l, a
                            ld de, (MetatileLoc)
                            add hl, de
                            
                            ld a, (hl)                      ; indirection      
                            inc hl
                            ld h, (hl)
                            ld l, a
                            
                            ret
                            
                            
                            
                            
                            
GSEPreProcessUpdate:        ; == Initialize
                            ld iy, GSElibRAM
                            xor a
                            ld (XUpdateStatus), a
                            ld (YUpdateStatus), a
                            
                            
_xScroll:                   ; == Check Left / Right Scroll... 
                            ld a, (XUpdate)              
                            and a                               
                            jp z, _yScroll                      ; no value = no horizontal scroll
                            jp m, _leftScroll                   ; signed = left scroll.
                                  
                                     
_rightScroll:               ; == Update currentXScroll Value.
                            ld hl, (XCurrent)             ; add to current x value
                            ld b, l                             
                            add a, l
                            jr nc, +
                            inc h
+:                          ld l, a
                            ld (XCurrent), hl   
                            xor b
                            ld i, a                             ; store low byte different pre / post scroll (for boundary tests)
+:                                    

_rightNameTable:            ; == Check for NameTable Boundary Cross
                            and %11111000
                            jp z, _yScroll
                            
                            ; == NameTable Bounary Crossed... Process
                            ld (iy + XXUpdateStatus), 1       ; Flag boundary cross
                            
                            ld a, (NametablePointer)            ; Update NameTable Pointer
                            ld c, a                             ; (addition bound within low 6 bits!!!)
                            and %11000000                       
                            ld b, a
                            ld a, c
                            add a, 2
                            and %111111
                            or b
                            ld (NametablePointer), a

_rightBufferTable:          ; == Check for BufferTable Boundary Cross
                            ld a, i                                 
                            and %11110000                       ; TODO !SMC metatile width bitmask
                            jp z, _yScroll
                            
                            ; == BufferTable Boundary Crossed... Process
                            ;ld (iy + XUpdateStatus), 3
                            
                            ld hl, (ScrolltablePointer)          ;  Update buffer table pointer.
                            inc hl
                            ld (ScrolltablePointer), hl

                            jp _yScroll

                                                    
_leftScroll:                ; == Update currentXScroll Value.
                            ld hl, (XCurrent)                   ; add to current x value
                            ld b, l
                            add a, l
                            jr c, +
                            dec h
+:                          ld l, a
                            xor b
                            ld i, a                             ; store low byte different pre / post scroll (for boundary tests)
+:                          ld (XCurrent), hl             
                            
_leftNameTable:             ; == Check for NameTable Boundary Cross
                            ld a, i
                            and %11111000
                            jp z, _yScroll
                            
                            ; == NameTable Bounary Crossed... Process
                            ld (iy + XXUpdateStatus), 129
                            
                            ld a, (NametablePointer)                   ; Update name table pointer.
                            ld c, a                             ; (subtraction bound within low 6 bits!!!)
                            and %11000000                       
                            ld b, a
                            ld a, c
                            sub 2
                            and %111111
                            or b
                            ld (NametablePointer), a
                            
_leftBufferTable:           ; == Check for BufferTable Boundary Cross
                            ld a, i                                 
                            and %11111000                       ; TODO !SMC metatile width bitmask
                            jp z, _yScroll
                            
                            ; == BufferTable Boundary Crossed... Process
                            ;ld (iy + XUpdateStatus), 131
                            
                            ld hl, (ScrolltablePointer)          ;  Update buffer table pointer.
                            dec hl
                            ld (ScrolltablePointer), hl
                                

_yScroll:                   ; == Check Up / Down Scroll... 
                            ld a, (YUpdate)
                            and a                           
                            ret z                               ; no value = no vertical scroll
                            jp m, _upScroll                     ; signed = left scroll.


_downScroll:                ; == Update dummyYScroll Value (wraps around 224 NameTable height!)
                            ld b, a
                            ld a, (YScroll224)
                            add a, b
                            cp 224
                            jp c, +
                            sub 224
+:                          ld (YScroll224), a
                            ld a, b
                            
                            ; == Update currentYScroll Value.
                            ld hl, (YCurrent)             ; add to current y value
                            ld b, l
                            add a, l
                            jr nc, +
                            inc h
+:                          ld l, a
                            xor b
                            ld i, a                             ; store low byte different pre / post scroll (for boundary tests)
+:                          ld (YCurrent), hl             
                            
                            
_downNameTable:             ; == Check for NameTable Boundary Cross
                            ld a, i
                            and %11111000
                            ret z    
                            
                            ; == NameTable Bounary Crossed... Process
                            ld (iy + YYUpdateStatus), 2
                                                
                            ld hl, (NametablePointer)                  ; Update name table pointer.
                            ld de, 64                           
                            add hl, de
                            ld a, $7F                                       ; TODO shift this to .defines for variable nat positioning
                            cp h
                            jp nz, +
                            ld h, $78
+:                          ld (NametablePointer), hl

_downBufferTable:           ; == Check for BufferTable Boundary Cross
                            ld a, i                                 
                            and %11110000                       ; !SMC metatile width bitmask
                            ret z
                            
                            ; == BufferTable Boundary Crossed... Process
                            ld (iy + YYUpdateStatus), 3
                            
                            ld hl, (ScrolltablePointer)
                            ld de, (WidthInMetatiles)
                            add hl, de
                            ld (ScrolltablePointer), hl

                            ret
                              
                              
                                
_upScroll:                  ; == Update dummyYScroll Value (wraps around 224 NameTable height!)
                            ld b, a
                            ld a, (YScroll224)
                            add a, b
                            jp c, +
                            sub 32
+:                          ld (YScroll224), a
                            ld a, b
                            
                            ; == Update currentYScroll Value.
                            ld hl, (YCurrent)             ; add to current y value
                            ld b, l
                            add a, l
                            jr c, +
                            dec h
+:                          ld l, a
                            xor b
                            ld i, a                             ; store low byte different pre / post scroll (for boundary tests)
+:                          ld (YCurrent), hl             
                            
_upNameTable:               ; == Check for NameTable Boundary Cross
                            ld a, i
                            and %11111000
                            ret z

                            ; == NameTable Bounary Crossed... Process
                            ld (iy + YYUpdateStatus), 130
                            
                            ld hl, (NametablePointer)                  ; Update name table pointer.
                            ld de, 64
                            or a
                            sbc hl, de
                            ld a, $77
                            cp h
                            jp nz, +
                            ld h, $7E
+:                          ld (NametablePointer), hl

_upBufferTable:             ; == Check for BufferTable Boundary Cross
                            ld a, i                                 
                            and %11100000                       ; !SMC metatile width bitmask
                            ret z
                            
                            ; == BufferTable Boundary Crossed... Process
                            ld (iy + YYUpdateStatus), 131
                            
                            ld hl, (ScrolltablePointer)
                            ld de, (WidthInMetatiles)
                            xor a
                            sbc hl, de
                            ld (ScrolltablePointer), hl

                            ret
                            
                            
                            
                            
                            
processScrollUpdates:       
_px:                        ; == Left / Right Scroll Occur?
                            ld a, (XUpdateStatus)
                            or a
                            ret z ;jp z, _py
                            ;jp m, _pl_buffer
                            
                            

                            


_pr_name:                   ; == Create Adjusted btPointer for _unrollColumnForNTUpdate call
                            ld de, (ScrolltablePointer)         ; We need to pass _unrollColumnForNTUpdate a buffertable pointer
                            ld a, 15
                            add a, e
                            jr nc, +
                            inc d
+:                          ld e, a

                            ; == Create X offset for _unrollColumnForNTUpdate call
                            ld a, (XCurrent)                    ; get x offset for meta tile. Writing column so need to adjust for this.
                            and %00001000
                            rrca
                            rrca
                            ld ixl, a
                            
                            ; == Unroll MetaTiles and get NameTable data for update
                            call _unrollColumnForNTUpdate   
                            
                            ; == Store Adjusted NameTable Address for Update.
                            ld a, (NametablePointer)
                            ld c, a
                            and %11000000                       ; update name table pointer.
                            ld b, a
                            ld a, c
                            sub 2
                            and %111111
                            or b
                            ld (GSE_NATColumnBuffer), a
                            ld a, (NametablePointer + 1)
                            ld (GSE_NATColumnBuffer+ 1), a    
                            
                            ret
                            
;                            jp _py
;                            
;
;_pl_buffer:                 ; == BufferTable Update Required?
;                            rra
;                            jp nc, _p1_name
;                            
;                            ; == Update Buffer Table?
;                            ; <INSERT BUFFER TABLE UPDATE CODE HERE>
;                            
;_p1_name:                   ; == NameTable Update Required?
;                            rra
;                            jp nc, _py
;
;                            ; == Create Adjusted btPointer for _unrollColumnForNTUpdate call
;                            ld bc, (btPointer)  
;                            or a
;                            jp p, +
;                            ld a, %11110000                     ; representing column from where tiles are read for nametable updates.
;                            and c                               ; Add screen width (-1) to pointer to get right side of screen updates.
;                            ld d, a
;                            ld a, c
;                            inc a
;                            and %1111
;                            or d
;                            ld c, a                             ; We have buffertable pointer in BC.
;+:
;                            ; == Create X offset for _unrollColumnForNTUpdate call
;                            ld a, (currentXScroll)              ; get x offset for meta tile. Writing column so need to adjust for this.
;                            add a, 8
;                            and %00011000
;                            rrca
;                            rrca
;                            ld ixl, a
;
;                            ; == Unroll MetaTiles and get NameTable data for update                        
;                            call _unrollColumnForNTUpdate
;                                 
;                            ; == Store NameTable Address for Update (no adjustment required)
;                            ld hl, (ntPointer)
;                            ld (ntColumnAddress), hl
;
;
;_py:                        ; == Left / Right Scroll Occur?
;                            ld a, (ntYUpdateFlag)
;                            or a
;                            ret z
;                            jp m, _pu_buffer
;                           
;_pd_buffer:                 ; == BufferTable Update Required?
;                            rra
;                            jp nc, _pd_name
;                            
;                            ; == Update Buffer Table?
;                            ; <INSERT BUFFER TABLE UPDATE CODE HERE>
;
;_pd_name:                   ; == NameTable Update Required?
;                            rra
;                            ret nc
;
;                            ; == Create Adjusted btPointer for _unrollColumnForNTUpdate call
;                            ld bc, (btPointer)                  ; We need to pass _unrollColumnForNTUpdate a buffertable pointer
;                            or a
;                            ld a, $70
;                            jp p, +
;                            ld a, $60
;+:                          add a, c
;                            ld c, a                             ; We have buffertable pointer in BC.
;                             
;                            ; == Create Y offset for _unrollColumnForNTUpdate call
;                            ld a, (currentYScroll)              ; get x offset for meta tile. Writing column so need to adjust for this.
;                            sub 8
;                            and %00011000
;                            ld ixl, a
;                                        
;                            ; == Unroll MetaTiles and get NameTable data for update                             
;                            call _unrollRowForNTUpdate          
;                            
;                            
;                            ; == Store Adjusted NameTable Address for Update.
;                            ld hl, (ntPointer)
;                            ld de, 64
;                            or a
;                            sbc hl, de
;                            ld a, $77
;                            cp h
;                            jp nz, +
;                            ld h, $7E
;+:                          ld (ntRowAddress), hl
;                            
;                            ret
;
;
;_pu_buffer:                 ; == BufferTable Update Required?
;                            rrca
;                            jp nc, _pu_name
;                            
;                            ; == Update Buffer Table?
;                            ; <INSERT BUFFER TABLE UPDATE CODE HERE>
;
;
;_pu_name:                   ; == NameTable Update Required?
;                            rrca
;                            ret nc
;                            
;                            ; == Get btPointer for _unrollColumnForNTUpdate call
;                            ld bc, (btPointer)                  
;                            
;                            ; == Create Y offset for _unrollColumnForNTUpdate call
;                            ld a, (currentYScroll)              ; get x offset for meta tile. Writing column so need to adjust for this.
;                            and %00011000
;                            ld ixl, a
;                            call _unrollRowForNTUpdate          ; Unroll buffer table column for nametable updates!
;                            
;                            ; == Store NameTable Address for Update (no adjustment required)
;                            ld hl, (ntPointer)
;+:                          ld (ntRowAddress), hl
;                            
;                            ret
;
;
; 
; 
; 
; 
ntColumnUpdate:            ; == Column Update Required?
                            ld a, (XUpdateStatus)
                            and a
                            ret z
                            
                            ; == Set up VDP address and control port stuff in shadow registers.
                            exx
                            ld hl, (GSE_NATUpdateColumn)            ; Get vram nametable pointer
                            ld de, 64                           ; Each new column entry is 64 bytes ahead, store here for addition
                            ld c, VDP_CONTROL_PORT
                            exx
                            
                            ld a, (YScroll224)
                            and %11111000
                            rrca
                            ld hl, ntColumnUpdateTable
                            add a, l
                            ld l, a
                            push hl                             ; Story copy on stack
                            
                            ld e, (hl)                          ; Indirection to get starting address in buffer
                            inc hl
                            ld d, (hl)
                            inc hl
                            
                            ld a, (hl)                          ; indirection to get jump value (equals required number of vram writes)
                            ld (smcRAMJump + 1), a
                            inc hl
                            ld a, (hl)
                            ld (smcRAMJump + 2), a
                            ex de, hl
                            ld c, VDP_DATA_PORT
                            call smcRAMJump
                            
                            exx
                            ld a, (GSE_NATUpdateColumn)                  ; Get vram nametable pointer
                            and %00111111
                            ld h, $78
                            ld l, a
++:                            
                            
                            
                            ld de, 64                           ; Each new column entry is 64 bytes ahead, store here for addition
                            ld c, VDP_CONTROL_PORT
                            exx
                            
                            ex de, hl
                            pop hl
                            ld a, 112
                            add a, l
                            ld l, a
                            
                            ;ld e, (hl)                         ; Indirection to get starting address in buffer
                            inc hl
                            ;ld d, (hl)
                            inc hl
                            
                            ld a, (hl)                          ; indirection to get jump value (equals required number of vram writes)
                            ld (smcRAMJump + 1), a
                            inc hl
                            ld a, (hl)
                            ld (smcRAMJump + 2), a
                            ex de, hl
                            call smcRAMJump
                            
;                           ld a, (ntUpdateFlag)
;                           bit 1, a
;                           jp z, _ntColumnUpdateFinalise
                            

                            
_ntColumnUpdateFinalise:    ;xor a
                            ;ld (ntUpdateFlag), a
                              ret
                            

;
;
;_ntRowUpdate:               ld a, (ntYUpdateFlag)
;                            bit 1, a
;                            ret z
;                            
;                            
;                            exx
;                            ld hl, (ntRowAddress)                  ; Get vram nametable pointer
;                            ld c, VDP_CONTROL_PORT
;                            out (c), l
;                            out (c), h
;                            exx
;                            ld c, VDP_DATA_PORT
;
;                            ld a, (currentXScroll)              ; Construct _ntColumnUpdateTable for current y location.
;                            and %11111000
;                            rrca
;                            ld hl, ntRowUpdateTable1
;                            add a, l
;                            ld l, a
;                            push hl                             ; Story copy on stack
;                            
;                            ld e, (hl)                          ; Indirection to get starting address in buffer
;                            inc hl
;                            ld d, (hl)
;                            inc hl
;                            
;                            ld a, (hl)                          ; indirection to get jump value (equals required number of vram writes)
;                            ld (smcRAMJump + 1), a
;                            inc hl
;                            ld a, (hl)
;                            ld (smcRAMJump + 2), a
;                            ex de, hl
;                            call smcRAMJump
;                            
;                            exx
;                            ld a, %11000000
;                            and l
;                            out (c), a
;                            out (c), h
;                            exx
;                            
;                            ld d, h
;                            ld e, l
;                            
;                            pop hl
;                            ld a, 128
;                            add a, l
;                            ld l, a
;                            
;                            ;ld e, (hl)                          ; Indirection to get starting address in buffer
;                            inc hl
;                            ;ld d, (hl)
;                            inc hl
;                            
;                            ld a, (hl)                          ; indirection to get jump value (equals required number of vram writes)
;                            ld (smcRAMJump + 1), a
;                            inc hl
;                            ld a, (hl)
;                            ld (smcRAMJump + 2), a
;                            ex de, hl
;                            call smcRAMJump
;                            
;;                            ld a, (ntUpdateFlag)
;;                            bit 3, a
;;                            jp z, _ntRowUpdateFinalise
;                            

                            
_ntRowUpdateFinalise:       ;xor a
                            ;ld (ntUpdateFlag), a
                            ret





/*
 * == Unroll Column for Nametable Update
 * Populates GSE_NATColumnBuffer with data for next vblank update.
 * 
 * It intentionally extracts 8 metatile entries aligned to top of first which 1 more than required.
 * Better for peek performance than logic to specific amount. Vblank code isolates correct data for
 * update.
 * 
 * @IN BC: BufferTable Pointer (Adjusted for left / right Scroll).
 * @IN IXL: Offset used to point to correct column within MetaTile when unrolling.
 * 
 * TRASHED: HL, DE, BC, A, IX
 * 
 * ADDITIONAL NOTES:
 * - DE pointers to buffer RAM. Written to as linear stream throughout code (never adjusted, just incremented after each write).
 * - BC BufferTable pointer is adjusted after ever iteration, shifted to entry below current.
 * - HL MetaTile address, resolved from buffertable entry above. Needs to be resolved after every BufferTable adjustment of course.
 * - IXL stores which column in MetaTile to get nametable entries. Adjusted for 2byte width (3rd entry = 6).
 * - IXH loop count for unrolling metatiles. Terminates on ZERO so we set it +1 the iterations we want. 
 */
_unrollColumnForNTUpdate:   ; == Set Up Registers
                            ld bc, GSE_NATColumnBuffer         
                            ld ixh, 16    
 
 
_unrollMetaTileColumn:      ; == Resolve MetaTile Address 
                            ld a, (de)                          ; bc = buffertable pointer  
                            and %111                            ; Following is optimized 16 bit Right Shift 3
                            or GSE_MetatileBoundary
                            ld h, a
                            ld a, (de)
                            and %11111000
                            or ixl                              ; We add X offset (in ixl) to get desired column in meta tile.
                            ld l, a
                            
                            
                            ; == Write Column Entries in Meta Tile to Update Buffer (unrolled code for 4 entries)
                            ld a, (hl)                          
                            ld (bc), a                          ; copy first byte
                            inc l
                            inc e
                            ld a, (hl)                          
                            ld (bc), a                          ; copy second byte
                            inc e    
                            inc l
                            inc l
                            inc l    
                            
                            ld a, (hl)                          
                            ld (bc), a                          ; copy first byte
                            inc l
                            inc e
                            ld a, (hl)                          
                            ld (bc), a                          ; copy second byte
                            
                            
                            ; == Have we unrolled 8x MetaTiles? (EXIT POINT!)
                            dec ixh                             ; Terminates on zero so ixh contains number of entries 
                            ret z                               ; to unroll +1
                            
                            
                            ; == Adjust Pointers for Next Iteration.
                            inc e   
                                           
                            ld hl, (WidthInMetatiles)           ; Update Scrolltable Pointer
                            add hl, de
                            ex de, hl
                            jp _unrollMetaTileColumn




_MetatileColumnUnroll:      ld b, 16
                            ld de, GSE_NATColumnBuffer
                            
                            ; i need to get / keep pointer to, scroll table. then load value in to hl.
                            
                            
                            ld l, (ix+0)
                            add hl, hl
                            add hl, hl
                            add hl, hl
                            ld bc, $0000
                            add hl, bc
                            
                            






; == MOD table to speed identifying VDPNametable loc for metatile updates.
; - Table is %28 but only for even numbers.
; - Each row represents 1x256 height (width does not matter!).
; - Comment / uncomment depending on your maps...
;   * for stages that max 1 screen tall you need 1 row.
;   * for stages that max 4 screens tall you need 4 rows, etc.
       
; == Code to generate table...            
; StringBuilder output = new StringBuilder();
; for (int i = 0; i < 2048; i+=2) 
; {
;     int n = i%28;
;     output.append(i%32 == 0 ? System.lineSeparator() + ".db $" + String.format("%02X", n) : ", $" + String.format("%02X", n));
; }                        
GSEModTable: 

.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
;.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
;.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
;.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
;.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
;.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
;.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
;.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
;.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
;.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
;.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
;.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
;.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
;.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
;.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
;.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
;.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
;.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
;.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
;.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
;.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
;.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
;.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
;.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
;.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
;.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
;.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
;.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
;.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
;.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
;.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
;.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
;.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
;.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
;.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
;.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
;.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
;.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
;.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
;.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
;.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
;.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
;.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
;.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
;.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
;.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
;.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
;.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
;.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
;.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02
;.db $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06
;.db $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A
;.db $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E
;.db $10, $12, $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12
;.db $14, $16, $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16
;.db $18, $1A, $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A
;.db $00, $02, $04, $06, $08, $0A, $0C, $0E, $10, $12, $14, $16, $18, $1A, $00, $02

.ends




.org $3B00
.bank 0 slot 0
.section "fff" force

ntColumnUpdateTable:
.dw GSE_NATColumnBuffer + ((00 * 2) & 7), columnNTWritesEnd - (28 * 11)
.dw GSE_NATColumnBuffer + ((01 * 2) & 7), columnNTWritesEnd - (27 * 11)
.dw GSE_NATColumnBuffer + ((02 * 2) & 7), columnNTWritesEnd - (26 * 11)
.dw GSE_NATColumnBuffer + ((03 * 2) & 7), columnNTWritesEnd - (25 * 11)
.dw GSE_NATColumnBuffer + ((04 * 2) & 7), columnNTWritesEnd - (24 * 11)
.dw GSE_NATColumnBuffer + ((05 * 2) & 7), columnNTWritesEnd - (23 * 11)
.dw GSE_NATColumnBuffer + ((06 * 2) & 7), columnNTWritesEnd - (22 * 11)
.dw GSE_NATColumnBuffer + ((07 * 2) & 7), columnNTWritesEnd - (21 * 11)
.dw GSE_NATColumnBuffer + ((08 * 2) & 7), columnNTWritesEnd - (20 * 11)
.dw GSE_NATColumnBuffer + ((09 * 2) & 7), columnNTWritesEnd - (19 * 11)
.dw GSE_NATColumnBuffer + ((10 * 2) & 7), columnNTWritesEnd - (18 * 11)
.dw GSE_NATColumnBuffer + ((11 * 2) & 7), columnNTWritesEnd - (17 * 11)
.dw GSE_NATColumnBuffer + ((12 * 2) & 7), columnNTWritesEnd - (16 * 11)
.dw GSE_NATColumnBuffer + ((13 * 2) & 7), columnNTWritesEnd - (15 * 11)
.dw GSE_NATColumnBuffer + ((14 * 2) & 7), columnNTWritesEnd - (14 * 11)
.dw GSE_NATColumnBuffer + ((15 * 2) & 7), columnNTWritesEnd - (13 * 11)
.dw GSE_NATColumnBuffer + ((16 * 2) & 7), columnNTWritesEnd - (12 * 11)
.dw GSE_NATColumnBuffer + ((17 * 2) & 7), columnNTWritesEnd - (11 * 11)
.dw GSE_NATColumnBuffer + ((18 * 2) & 7), columnNTWritesEnd - (10 * 11)
.dw GSE_NATColumnBuffer + ((19 * 2) & 7), columnNTWritesEnd - (09 * 11)
.dw GSE_NATColumnBuffer + ((20 * 2) & 7), columnNTWritesEnd - (08 * 11)
.dw GSE_NATColumnBuffer + ((21 * 2) & 7), columnNTWritesEnd - (07 * 11)
.dw GSE_NATColumnBuffer + ((22 * 2) & 7), columnNTWritesEnd - (06 * 11)
.dw GSE_NATColumnBuffer + ((23 * 2) & 7), columnNTWritesEnd - (05 * 11)
.dw GSE_NATColumnBuffer + ((24 * 2) & 7), columnNTWritesEnd - (04 * 11)
.dw GSE_NATColumnBuffer + ((25 * 2) & 7), columnNTWritesEnd - (03 * 11)
.dw GSE_NATColumnBuffer + ((26 * 2) & 7), columnNTWritesEnd - (02 * 11)
.dw GSE_NATColumnBuffer + ((27 * 2) & 7), columnNTWritesEnd - (01 * 11)

.dw GSE_NATColumnBuffer + (00 * 2) + ((00 * 2) & 7), columnNTWritesEnd - (00 * 11)
.dw GSE_NATColumnBuffer + (01 * 2) + ((01 * 2) & 7), columnNTWritesEnd - (01 * 11)
.dw GSE_NATColumnBuffer + (02 * 2) + ((02 * 2) & 7), columnNTWritesEnd - (02 * 11)
.dw GSE_NATColumnBuffer + (03 * 2) + ((03 * 2) & 7), columnNTWritesEnd - (03 * 11)
.dw GSE_NATColumnBuffer + (04 * 2) + ((04 * 2) & 7), columnNTWritesEnd - (04 * 11)
.dw GSE_NATColumnBuffer + (05 * 2) + ((05 * 2) & 7), columnNTWritesEnd - (05 * 11)
.dw GSE_NATColumnBuffer + (06 * 2) + ((06 * 2) & 7), columnNTWritesEnd - (06 * 11)
.dw GSE_NATColumnBuffer + (07 * 2) + ((07 * 2) & 7), columnNTWritesEnd - (07 * 11)
.dw GSE_NATColumnBuffer + (08 * 2) + ((08 * 2) & 7), columnNTWritesEnd - (08 * 11)
.dw GSE_NATColumnBuffer + (09 * 2) + ((09 * 2) & 7), columnNTWritesEnd - (09 * 11)
.dw GSE_NATColumnBuffer + (10 * 2) + ((10 * 2) & 7), columnNTWritesEnd - (10 * 11)
.dw GSE_NATColumnBuffer + (11 * 2) + ((11 * 2) & 7), columnNTWritesEnd - (11 * 11)
.dw GSE_NATColumnBuffer + (12 * 2) + ((12 * 2) & 7), columnNTWritesEnd - (12 * 11)
.dw GSE_NATColumnBuffer + (13 * 2) + ((13 * 2) & 7), columnNTWritesEnd - (13 * 11)
.dw GSE_NATColumnBuffer + (14 * 2) + ((14 * 2) & 7), columnNTWritesEnd - (14 * 11)
.dw GSE_NATColumnBuffer + (15 * 2) + ((15 * 2) & 7), columnNTWritesEnd - (15 * 11)
.dw GSE_NATColumnBuffer + (16 * 2) + ((16 * 2) & 7), columnNTWritesEnd - (16 * 11)
.dw GSE_NATColumnBuffer + (17 * 2) + ((17 * 2) & 7), columnNTWritesEnd - (17 * 11)
.dw GSE_NATColumnBuffer + (18 * 2) + ((18 * 2) & 7), columnNTWritesEnd - (18 * 11)
.dw GSE_NATColumnBuffer + (19 * 2) + ((19 * 2) & 7), columnNTWritesEnd - (19 * 11)
.dw GSE_NATColumnBuffer + (20 * 2) + ((20 * 2) & 7), columnNTWritesEnd - (20 * 11)
.dw GSE_NATColumnBuffer + (21 * 2) + ((21 * 2) & 7), columnNTWritesEnd - (21 * 11)
.dw GSE_NATColumnBuffer + (22 * 2) + ((22 * 2) & 7), columnNTWritesEnd - (22 * 11)
.dw GSE_NATColumnBuffer + (23 * 2) + ((23 * 2) & 7), columnNTWritesEnd - (23 * 11)
.dw GSE_NATColumnBuffer + (24 * 2) + ((24 * 2) & 7), columnNTWritesEnd - (24 * 11)
.dw GSE_NATColumnBuffer + (25 * 2) + ((25 * 2) & 7), columnNTWritesEnd - (25 * 11)
.dw GSE_NATColumnBuffer + (26 * 2) + ((26 * 2) & 7), columnNTWritesEnd - (26 * 11)
.dw GSE_NATColumnBuffer + (27 * 2) + ((27 * 2) & 7), columnNTWritesEnd - (27 * 11)

                            .repeat 28
                            exx
                            out (c), l
                            out (c), h
                            add hl, de
                            exx
                            outi
                            outi
                            .endr
columnNTWritesEnd:          ret

.ends







