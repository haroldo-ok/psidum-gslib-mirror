

.section "GSELib" free


; == METATILES
; - Metatiles has header in slot 0
; - Must be on 2k boundary!


; == SCROLLTABLE
; -  header prior to actual data




.define GSE_GENERAL_RAM                 $D000       ; location of general purpose ram
.define GSE_METATILE_TABLE              $8000       ; location of metatile table aligned to 2k boundary
.define GSE_METATILE_TABLE_HIGH_BYTE    $80
.define GSE_HORIZONTAL_ADDITION         $0F         ; addition value required to push scrolltable pointer to right size of screen 
.define GSE_NAMETABLE_BASE_ADDRESS      $7800       ; base address of nametable in vram including write bit set (14)   

.enum GSE_GENERAL_RAM asc
        
        GSE_ScrolltableLoc              dw     
        GSE_MetatileLUTLoc              dw          ; note: must be on 2k boundary!
        
        ; == Following is stored in scrolltable header.
        GSE_ScrolltableSize             dw
        GSE_WidthInMetatiles            dw          
        GSE_HeightInMetatiles           dw
        GSE_WidthInPixels               dw
        GSE_HeightInPixels              dw          
        GSE_VerticalAddition            dw          ; addition value required to push scrolltable point to bottom left.
        GSE_OptionByte                  db
        
        ; == Following is internal working ram
        GSE_ScrolltablePointer          dw
        GSE_NametablePointer            dw
        GSE_MetatileLookupBuffer        dw
        GSE_X                           dw          ; current x point in scrolltable
        GSE_Y                           dw          ; current y point in scroll table
        GSE_XUpdateRequest              db          ; requested x adjustment (signed)
        GSE_YUpdateRequest              db          ; requested y adjustment (signed)
        GSE_XUpdateStatus               db          ; represents required updates if any
        GSE_YUpdateStatus               db          ; ^^
        GSE_Y224                        db          ; stores Y%224. Needed to speed things up.
        GSE_NametableUpdateColumn       dw          ; first entry in nametable for column update
        GSE_NametableUpdateRow          dw          ; first entry in nametable for row update
        GSE_NATColumnBuffer             dsb 100
        GSE_NATRowBuffer                dsb 100  
        
.ende


/*
 * == Initializes GSElib for rendering of new map
 * 
 * @in hl: Scrolltable Location
 */
GSE_InitaliseMap:           ; == Copy Scrolltable Header to Ram (its structure is identical to out enum)
                            ld de, GSE_GENERAL_RAM + 4
                            ld bc, 13
                            ldir
                            
                            
                            ; == Store GSE_ScrolltableLoc (ldir will have pushed past header to actual table)
                            ld (GSE_ScrolltableLoc), hl
                            
                            
                            ; == Store GSE_MetatileLUTLoc
                            ld de, (GSE_ScrolltableSize)
                            add hl, de
                            ld (GSE_MetatileLUTLoc), hl
                            
                            ld a, (GSE_OptionByte)              ; bit 0 set = Metatile LUT Required.
                            bit 0, a                            ; proceed only if set
                            jr z, +                             
                            
                            ld ix, (GSE_MetatileLUTLoc)
                            ld hl, (GSE_ScrolltableLoc)
                            ld bc, (GSE_HeightInMetatiles)
                            ld de, (GSE_WidthInMetatiles)
                            
-:                          ld (ix+0), h                        ; == psudo 
                            inc ix                              ; lut_entry = Scrolltable Location
                            ld (ix+0), l                        ; loop height in metatiles
                            inc ix                              ; { 
                            add hl, de                          ;   write lut_entry
                            dec bc                              ;   lut_entry += width in metatiles
                            ld a, b                             ; }
                            or c
                            jr nz, -
                            
+:                          ret




/* == Retreives from Scrolltable, Metatile ID and address
 * 
 * - Stores address in buffer GSE_MetatileLookupBuffer.
 * - Requires GSE_MetatileLUT to have been created.
 * - GSE_MetatileLUT has Scrolltable base address baked in so no need do add this!
 * 
 * @in hl: Y
 * @in de: X
 * 
 * @out a:  metatile id
 * @out hl: address of metatile in scrolltable
 */
GSE_MetatileLookup:         ; == Convert Y to LUT Entry and retrieve value
                            ld a, %11110000
                            and l
                            srl h
                            rra
                            srl h
                            rra
                            srl h
                            rra
                            ld l, a
                            
                            ld bc, (GSE_MetatileLUTLoc)
                            add hl, bc
                            ld a, (hl)                      ; indirection      
                            inc hl                       
                            ld l, (hl)                   
                            ld h, a    


                            ; == Divide X by 16 then add to above address
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
                            
                            ; == Put Metatile ID in a and store address in buffer.
                            ld a, (hl)
                            ld (GSE_MetatileLookupBuffer), hl
                            
                            ret
                            
                            
                            
                            
/* == Retreives from Scrolltable, Nametable Entry
 * 
 * - Stores address of surrounding metatile in buffer GSE_MetatileLookupBuffer.
 * - Requires GSE_MetatileLUT to have been created.
 * - GSE_MetatileLUT has Scrolltable base address baked in so no need do add this!
 * 
 * @in hl: Y
 * @in de: X
 * 
 * @out hl: Nametable entry
 */                      
GSE_TileLookup:             ; == Create offset for within Metatile (which nametable entry does x,y point to)
                            ld a, %00000001
                            and l
                            rla
                            rla
                            ld c, a
                            ld a, %00000001
                            and e
                            rla
                            or c
                            push af
                            
                            ; == Retrieve Metatile ID
                            call GSE_MetatileLookup
                            
                            ; == Resolve in to Metatile Address - ((ld << 3) + Offset + GSE_METATILE_TABLE)
                            ld l, a
                            xor a
                            ld h, a
                            add hl, hl
                            add hl, hl
                            add hl, hl
                            pop af
                            add a, l
                            ld l, a
                            ld de, (GSE_METATILE_TABLE)
                            add hl, de
                            
                            ; == Get Nametable entry using resolved address
                            ld a, (hl)                      ; indirection      
                            inc hl
                            ld h, (hl)
                            ld l, a
                            
                            ret
                            
                            
                            
                            
GSE_PositionWindow:         ; **** Generate GSE_ScrolltablePointer
                            ; == Store (x, y) in ram
                            ld (GSE_Y), hl
                            ex de, hl
                            ld (GSE_X), hl
                            ex de, hl
                            
                            ; == Get Scrolltable location based on (x, y) and store as pointer
                            call GSE_MetatileLookup
                            ld (GSE_ScrolltablePointer), hl
                            
                            
                            ; **** Generate GSE_NametablePointer (vram pointer representing top left of window)
                            ; == Divide Y by 8
                            ld hl, (GSE_Y)
                            srl h                   ; divide y by 8 (get tile row)
                            rr l
                            srl h
                            rr l
                            srl h
                            rr l
                            
                            ; == Modulo 28 (height of nametable in tiles)
                            ld e, 28                
                            ld b, 8
-:                          adc hl, hl
                            ld a, h
                            jr c, +
                            cp e
                            jr c, ++
+:                          sub e
                            ld h, a
                            or a
++:                         djnz -
                            ld a, l
                            rla
                            cpl
                            
                            ; == Multiply by 64 (length of data in nametable row)
                            ld l, h                 
                            xor a
                            ld d, a                     ; this is for next step!
                            ld h, a
                            add hl, hl              
                            add hl, hl
                            add hl, hl
                            add hl, hl
                            add hl, hl
                            add hl, hl
                            
                            ; == Add X worth of nametable entries.
                            ; Divide by 8 then << 1 (below is optimization of this)
                            ld a, (GSE_X)
                            and %11111000
                            rra
                            rra
                            ld e, a
                            add hl, de
                            
                            ; == Add vdp base address of nametable 
                            ld de, GSE_NAMETABLE_BASE_ADDRESS       
                            add hl, de
                            
                            ; == Store GSE_NametablePointer
                            inc hl
                            inc hl
                            ld (GSE_NametablePointer), hl     
                            
                            
                            ; **** Generate GSE_Y224
                            ; == Y Modulo 224 (height of window in pixels)
                            ld hl, (GSE_Y)
                            ld e, 224             
                            ld b, 8
-:                          adc hl, hl
                            ld a, h
                            jr c, +
                            cp e
                            jr c, ++
+:                          sub e
                            ld h, a
                            or a
++:                         djnz -
                            ld a, l
                            rla
                            cpl
                            
                            ; == Store GSE_Y224
                            ld a, h
                            ld (GSE_Y224), a
                            
                            ret
                            
                            
                            
                            
GSEPreProcessUpdate:        ; == Initialize
                            ld iy, GSE_GENERAL_RAM
                            xor a
                            ld (GSE_XUpdateStatus), a
                            ld (GSE_YUpdateStatus), a
                            
                            
_xScroll:                   ; == Check Left / Right Scroll... 
                            ld a, (GSE_XUpdateRequest)              
                            and a                               
                            jp z, _yScroll                      ; no value = no horizontal scroll
                            jp m, _leftScroll                   ; signed = left scroll.
                                  
                                     
_rightScroll:               ; == Update currentXScroll Value.
                            ld hl, (GSE_X)                      ; add to current x value
                            ld b, l
                            add a, l
                            jr nc, +
                            inc h
+:                          ld l, a
                            xor b
                            ld i, a                             ; store low byte different pre / post scroll (for boundary tests)
+:                          ld (GSE_X), hl      
                                   

_rightNameTable:            ; == Check for NameTable Boundary Cross
                            and %11111000
                            jp z, _yScroll
                            
                            ; == NameTable Bounary Crossed... Process
                            ld (iy + GSE_XUpdateStatus - GSE_GENERAL_RAM), 1       ; Flag boundary cross
                            
                            ld a, (GSE_NametablePointer)            ; Update NameTable Pointer
                            ld c, a                             ; (addition bound within low 6 bits!!!)
                            and %11000000                       
                            ld b, a
                            ld a, c
                            add a, 2
                            and %111111
                            or b
                            ld (GSE_NametablePointer), a


_rightBufferTable:          ; == Check for BufferTable Boundary Cross
                            ld a, i                                 
                            and %11110000                    
                            jp z, _yScroll
                            
                            ; == BufferTable Boundary Crossed... Process
                            ld hl, (GSE_ScrolltablePointer)          ;  Update buffer table pointer.
                            inc hl
                            ld (GSE_ScrolltablePointer), hl

                            jp _yScroll

                                                    
_leftScroll:                ; == Update currentXScroll Value.
                            ld hl, (GSE_X)                      ; add to current x value
                            ld b, l
                            add a, l
                            jr c, +
                            dec h
+:                          ld l, a
                            xor b
                            ld i, a                             ; store low byte different pre / post scroll (for boundary tests)
+:                          ld (GSE_X), hl             
                            
_leftNameTable:             ; == Check for NameTable Boundary Cross
                            and %11111000
                            jp z, _yScroll
                            
                            ; == NameTable Bounary Crossed... Process
                            ld (iy + GSE_XUpdateStatus - GSE_GENERAL_RAM), 129
                            
                            ld a, (GSE_NametablePointer)        ; Update name table pointer.
                            ld c, a                             ; (subtraction bound within low 6 bits!!!)
                            and %11000000                       
                            ld b, a
                            ld a, c
                            sub 2
                            and %111111
                            or b
                            ld (GSE_NametablePointer), a
                            
                            
_leftBufferTable:           ; == Check for BufferTable Boundary Cross
                            ld a, i                                 
                            and %11110000                       
                            jp z, _yScroll
                            
                            ; == BufferTable Boundary Crossed... Process
                            ld hl, (GSE_ScrolltablePointer)          ;  Update buffer table pointer.
                            dec hl
                            ld (GSE_ScrolltablePointer), hl
                                

_yScroll:                   ; == Check Up / Down Scroll... 
                            ld a, (GSE_YUpdateRequest)
                            and a                           
                            ret z          ; no value = no vertical scroll
                            jp m, _upScroll                     ; signed = left scroll.


_downScroll:                ; == Update dummyYScroll Value (wraps around 224 NameTable height!)
                            ld b, a
                            ld a, (GSE_Y224)
                            add a, b
                            cp 224
                            jp c, +
                            sub 224
+:                          ld (GSE_Y224), a
                            ld a, b
                            
                            ; == Update currentYScroll Value.
                            ld hl, (GSE_Y)             ; add to current y value
                            ld b, l
                            add a, l
                            jr nc, +
                            inc h
+:                          ld l, a
                            xor b
                            ld i, a                             ; store low byte different pre / post scroll (for boundary tests)
+:                          ld (GSE_Y), hl             
                            
                            
_downNameTable:             ; == Check for NameTable Boundary Cross
                            and %11111000
                            ret z 
                            
                            ; == NameTable Bounary Crossed... Process
                            ld (iy + GSE_YUpdateStatus - GSE_GENERAL_RAM), 1 ; ????
                                                
                            ld hl, (GSE_NametablePointer)                  ; Update name table pointer.
                            ld de, 64                           
                            add hl, de
                            ld a, $7F                                       ; TODO shift this to .defines for variable nat positioning
                            cp h
                            jp nz, +
                            ld h, $78
+:                          ld (GSE_NametablePointer), hl

_downBufferTable:           ; == Check for BufferTable Boundary Cross
                            ld a, i                                 
                            and %11110000                       ; !SMC metatile width bitmask
                            ret z
                            
                            ; == BufferTable Boundary Crossed... Process
                            
                            ld hl, (GSE_ScrolltablePointer)
                            ld de, (GSE_WidthInMetatiles)
                            add hl, de
                            ld (GSE_ScrolltablePointer), hl

                            ret
                              
                              
                                
_upScroll:                  ; == Update dummyYScroll Value (wraps around 224 NameTable height!)
                            ld b, a
                            ld a, (GSE_Y224)
                            add a, b
                            jp c, +
                            sub 32
+:                          ld (GSE_Y224), a
                            ld a, b
                            
                            ; == Update currentYScroll Value.
                            ld hl, (GSE_Y)             ; add to current y value
                            ld b, l
                            add a, l
                            jr c, +
                            dec h
+:                          ld l, a
                            xor b
                            ld i, a                             ; store low byte different pre / post scroll (for boundary tests)
+:                          ld (GSE_Y), hl             
                            
_upNameTable:               ; == Check for NameTable Boundary Cross
                            ld a, i
                            and %11111000
                            ret z

                            ; == NameTable Bounary Crossed... Process
                            ld (iy + GSE_YUpdateStatus - GSE_GENERAL_RAM), 129
                            
                            ld hl, (GSE_NametablePointer)                  ; Update name table pointer.
                            ld de, 64
                            or a
                            sbc hl, de
                            ld a, $77
                            cp h
                            jp nz, +
                            ld h, $7E
+:                          ld (GSE_NametablePointer), hl

_upBufferTable:             ; == Check for BufferTable Boundary Cross
                            ld a, i                                 
                            and %11110000                       ; !SMC metatile width bitmask
                            ret z
                            
                            ; == BufferTable Boundary Crossed... Process
                            
                            ld hl, (GSE_ScrolltablePointer)
                            ld de, (GSE_WidthInMetatiles)
                            xor a
                            sbc hl, de
                            ld (GSE_ScrolltablePointer), hl

                            ret
                            
                            
                            
                            
                            
processScrollUpdates:       
_px:                        ; == Left / Right Scroll Occur?
                            ld a, (GSE_XUpdateStatus)
                            or a
                            jp z, _py
                            jp m, _p1_name
                            
_pr_name:                   ; == Create Adjusted btPointer for _unrollColumnForNTUpdate call
;                            ld de, (GSE_ScrolltablePointer)         ; We need to pass _unrollColumnForNTUpdate a buffertable pointer
;                            ld a, 16
;                            add a, e
;                            jr nc, +
;                            inc d
;+:                          ld e, a
                            ld iy, (GSE_ScrolltablePointer)
                            ld de, 16
                            add iy, de

                            ; == Create X offset for _unrollColumnForNTUpdate call
                            ld a, (GSE_X)                    ; get x offset for meta tile. Writing column so need to adjust for this.
                            and %00001000
                            rrca
                            rrca
                            ld ixl, a
                            
                            ; == Unroll MetaTiles and get NameTable data for update
                            call _unrollColumnForNTUpdate   
                            
                            ; == Store Adjusted NameTable Address for Update.
                            ld a, (GSE_NametablePointer)
                            ld c, a
                            and %11000000                       ; update name table pointer.
                            ld b, a
                            ld a, c
                            sub 2
                            and %111111
                            or b
                            ld (GSE_NametableUpdateColumn), a
                            ld a, (GSE_NametablePointer + 1)
                            ld (GSE_NametableUpdateColumn+ 1), a    
                            
                            
                            jp _py
;                            
;                     
_p1_name:                   ; == NameTable Update Required?
                            rra
                            jp nc, _py
                            

                            ; == Create Adjusted btPointer for _unrollColumnForNTUpdate call
;                            ld de, (GSE_ScrolltablePointer)  
;                            ld a, (GSE_X) 
;                            add a, 8
;                            and %00001000
;                            jp nz, +
;                            inc de
;                            +:
                            
                            ld iy, (GSE_ScrolltablePointer)
                            ld a, (GSE_X) 
                            add a, 8
                            and %00001000
                            jp nz, +
                            inc iy
                            +:
                            
                            ; == Create X offset for _unrollColumnForNTUpdate call
                            ;ld a, (GSE_X)              ; get x offset for meta tile. Writing column so need to adjust for this.
                            ;add a, 8
                            ;and %00001000
                            rrca
                            rrca
                            ld ixl, a

                            ; == Unroll MetaTiles and get NameTable data for update                        
                            call _unrollColumnForNTUpdate
                                 
                            ; == Store NameTable Address for Update (no adjustment required)
                            ld hl, (GSE_NametablePointer)
                            ld (GSE_NametableUpdateColumn), hl

                           
_py:                        ; == Left / Right Scroll Occur?
                            ld a, (GSE_YUpdateStatus)
                            or a
                            ret z
                            jp m, _pu_name
                           


_pd_name:                   ; == NameTable Update Required?
                            rra
                            ret nc

                            ; == Create Adjusted btPointer for _unrollColumnForNTUpdate call
                            ld iy, (GSE_ScrolltablePointer)
                            ld de, (GSE_VerticalAddition)
                            add iy, de
                            
                            ld de, (GSE_WidthInMetatiles)
                            ld a, (GSE_Y)              ; get x offset for meta tile. Writing column so need to adjust for this.
                            sub 8
                            and %00001000
                            jp nz, +
                            add iy, de
                            +:
                            
                            rrca
                            ld ixl, a
                                        
                            ; == Unroll MetaTiles and get NameTable data for update                             
                            call _unrollRowForNTUpdate          
                            
                            
                            ; == Store Adjusted NameTable Address for Update.
                            ld hl, (GSE_NametablePointer)
                            ld de, 64
                            or a
                            sbc hl, de
                            ld a, $77
                            cp h
                            jp nz, +
                            ld h, $7E
+:                          ld (GSE_NametableUpdateRow), hl
                            
                            ret





_pu_name:                   ; == NameTable Update Required?
                            rrca
                            ret nc
                            
                            ; == Get btPointer for _unrollColumnForNTUpdate call
                            ;ld bc, (GSE_ScrolltablePointer)    
                            ld iy, (GSE_ScrolltablePointer)
                            
                            ; == Create Y offset for _unrollColumnForNTUpdate call
                            ld a, (GSE_Y)              ; get x offset for meta tile. Writing column so need to adjust for this.
                            and %00001000
                            rrca
                            ld ixl, a
                            call _unrollRowForNTUpdate          ; Unroll buffer table column for nametable updates!
                            
                            ; == Store NameTable Address for Update (no adjustment required)
                            ld hl, (GSE_NametablePointer)
+:                          ld (GSE_NametableUpdateRow), hl
                            
                            ret




/*
 * == Populates Buffer for VBlank Updates
 * 
 * - Extracts row of nametable entries from scrolltable
 * - Stored in Buffer for Vblank routine
 * - Metatile ids in scrolltable are pre processed to speed the resolving of their address (which is very time consuming)
 *      * (metatile_id << 3) & 248) + ((metatile_id >> 5) & 7)
 * 
 * @in iy: scrolltable pointer pointing to first metatile in update
 * @in ixl: X offset, needed to identify which row in metatile to extract.
 */  
_unrollRowForNTUpdate:      ld de, GSE_NATRowBuffer         
                            ld c, 17*4   
                            
_unrollMetaTileRow:         ld l, (iy+0)                        ; == Resolve metatile address, current metatile pointed to by iy
                            ld a, l                             ; optimized <<3 shift, bits are stored as '43210765' for speed
                            and %111                          
                            or GSE_METATILE_TABLE_HIGH_BYTE     ; add high bits of metatile address (table must be on 2k boundary)
                            ld h, a
                            ld a, l
                            and %11111000
                            or ixl                              ; add X offset (in ixl) to get desired column in meta tile.
                            ld l, a                             ; hl = pointer to metatile nametable entry we need to write to vdp
                            
                            ldi                                 ; == Write 2x nametable entries to buffer.
                            ldi                                 ; metatiles are 2 entries wide so l+=2 to move to second entry.
                            ldi
                            ldi
                            
                            xor a                               ; loop
                            and c
                            ret z
                            
                            inc iy                              ; Update Scrolltable Pointer
                            
                            jp _unrollMetaTileRow
                            
                            
                            
                            
                            
/*
 * == Populates Buffer for VBlank Updates
 * 
 * - Extracts column of nametable entries from scrolltable
 * - Stored in Buffer for Vblank routine
 * - Metatile ids in scrolltable are pre processed to speed the resolving of their address (which is very time consuming)
 *      * (metatile_id << 3) & 248) + ((metatile_id >> 5) & 7)
 * 
 * @in iy: scrolltable pointer pointing to first metatile in update
 * @in ixl: X offset, needed to identify which column in metatile to extract.
 */                   
_unrollColumnForNTUpdate:   ld de, GSE_NATColumnBuffer         
                            ld c, 15*4   
                            
_unrollMetaTileColumn:      ld l, (iy+0)                        ; == Resolve metatile address, current metatile pointed to by iy
                            ld a, l                             ; optimized <<3 shift, bits are stored as '43210765' for speed
                            and %111                            
                            or GSE_METATILE_TABLE_HIGH_BYTE     ; add high bits of metatile address (table must be on 2k boundary)
                            ld h, a
                            ld a, l
                            and %11111000
                            or ixl                              ; add X offset (in ixl) to get desired column in meta tile.
                            ld l, a                             ; hl = pointer to metatile nametable entry we need to write to vdp
                            
                            ldi                                 ; == Write 2x nametable entries to buffer.
                            ldi                                 ; metatiles are 2 entries wide so l+=2 to move to second entry.
                            inc l                   
                            inc l
                            ldi
                            ldi
                            
                            xor a                               ; loop
                            and c
                            ret z
                            
                            ld hl, (GSE_WidthInMetatiles)       ; Update Scrolltable Pointer
                            ex de, hl
                            add iy, de
                            ex de, hl
                            
                            jp _unrollMetaTileColumn                           
                            
                            
                            
                            
                                               
                            
ntRowUpdate:                ld a, (GSE_YUpdateStatus)
                            and a
                            ret z
                            
                            exx
                            ld hl, (GSE_NametableUpdateRow)                  ; Get vram nametable pointer
                            ld c, VDP_CONTROL_PORT
                            out (c), l
                            out (c), h
                            exx
                            ld c, VDP_DATA_PORT

                            ld a, (GSE_X)              ; Construct _ntColumnUpdateTable for current y location.
                            and %11111000
                            rrca
                            ld hl, ntRowUpdateTable1
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
                            call smcRAMJump
                            
                            exx
                            ld a, %11000000
                            and l
                            out (c), a
                            out (c), h
                            exx
                            
                            ld d, h
                            ld e, l
                            
                            pop hl
                            ld a, 128
                            add a, l
                            ld l, a
                            
                            inc hl
                            inc hl
                            
                            ld a, (hl)                          ; indirection to get jump value (equals required number of vram writes)
                            ld (smcRAMJump + 1), a
                            inc hl
                            ld a, (hl)
                            ld (smcRAMJump + 2), a
                            ex de, hl
                            call smcRAMJump
                            
;                            ld a, (ntUpdateFlag)
;                            bit 3, a
;                            jp z, _ntRowUpdateFinalise
                            

                            
_ntRowUpdateFinalise:       ;xor a
                            ;ld (ntUpdateFlag), a
                            ret

                            
                            

                           
                           
                           
                            
ntColumnUpdate:            ; == Column Update Required?
                            ld a, (GSE_XUpdateStatus)
                            and a
                            ret z
                            
                            ; == Set up VDP address and control port stuff in shadow registers.
                            exx
                            ld hl, (GSE_NametableUpdateColumn)            ; Get vram nametable pointer
                            ld de, 64                           ; Each new column entry is 64 bytes ahead, store here for addition
                            ld c, VDP_CONTROL_PORT
                            exx
                            
                            ld a, (GSE_Y224)
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
                            ld a, (GSE_NametableUpdateColumn)                  ; Get vram nametable pointer
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
                            
.ends


.org $0100
.bank 1 slot 1
.section "fff" force

ntColumnUpdateTable:
.dw GSE_NATColumnBuffer + ((00 * 2) & 3), columnNTWritesEnd - (28 * 11)
.dw GSE_NATColumnBuffer + ((01 * 2) & 3), columnNTWritesEnd - (27 * 11)
.dw GSE_NATColumnBuffer + ((02 * 2) & 3), columnNTWritesEnd - (26 * 11)
.dw GSE_NATColumnBuffer + ((03 * 2) & 3), columnNTWritesEnd - (25 * 11)
.dw GSE_NATColumnBuffer + ((04 * 2) & 3), columnNTWritesEnd - (24 * 11)
.dw GSE_NATColumnBuffer + ((05 * 2) & 3), columnNTWritesEnd - (23 * 11)
.dw GSE_NATColumnBuffer + ((06 * 2) & 3), columnNTWritesEnd - (22 * 11)
.dw GSE_NATColumnBuffer + ((07 * 2) & 3), columnNTWritesEnd - (21 * 11)
.dw GSE_NATColumnBuffer + ((08 * 2) & 3), columnNTWritesEnd - (20 * 11)
.dw GSE_NATColumnBuffer + ((09 * 2) & 3), columnNTWritesEnd - (19 * 11)
.dw GSE_NATColumnBuffer + ((10 * 2) & 3), columnNTWritesEnd - (18 * 11)
.dw GSE_NATColumnBuffer + ((11 * 2) & 3), columnNTWritesEnd - (17 * 11)
.dw GSE_NATColumnBuffer + ((12 * 2) & 3), columnNTWritesEnd - (16 * 11)
.dw GSE_NATColumnBuffer + ((13 * 2) & 3), columnNTWritesEnd - (15 * 11)
.dw GSE_NATColumnBuffer + ((14 * 2) & 3), columnNTWritesEnd - (14 * 11)
.dw GSE_NATColumnBuffer + ((15 * 2) & 3), columnNTWritesEnd - (13 * 11)
.dw GSE_NATColumnBuffer + ((16 * 2) & 3), columnNTWritesEnd - (12 * 11)
.dw GSE_NATColumnBuffer + ((17 * 2) & 3), columnNTWritesEnd - (11 * 11)
.dw GSE_NATColumnBuffer + ((18 * 2) & 3), columnNTWritesEnd - (10 * 11)
.dw GSE_NATColumnBuffer + ((19 * 2) & 3), columnNTWritesEnd - (09 * 11)
.dw GSE_NATColumnBuffer + ((20 * 2) & 3), columnNTWritesEnd - (08 * 11)
.dw GSE_NATColumnBuffer + ((21 * 2) & 3), columnNTWritesEnd - (07 * 11)
.dw GSE_NATColumnBuffer + ((22 * 2) & 3), columnNTWritesEnd - (06 * 11)
.dw GSE_NATColumnBuffer + ((23 * 2) & 3), columnNTWritesEnd - (05 * 11)
.dw GSE_NATColumnBuffer + ((24 * 2) & 3), columnNTWritesEnd - (04 * 11)
.dw GSE_NATColumnBuffer + ((25 * 2) & 3), columnNTWritesEnd - (03 * 11)
.dw GSE_NATColumnBuffer + ((26 * 2) & 3), columnNTWritesEnd - (02 * 11)
.dw GSE_NATColumnBuffer + ((27 * 2) & 3), columnNTWritesEnd - (01 * 11)

.dw GSE_NATColumnBuffer + (00 * 2) + ((00 * 2) & 3), columnNTWritesEnd - (00 * 11)
.dw GSE_NATColumnBuffer + (01 * 2) + ((01 * 2) & 3), columnNTWritesEnd - (01 * 11)
.dw GSE_NATColumnBuffer + (02 * 2) + ((02 * 2) & 3), columnNTWritesEnd - (02 * 11)
.dw GSE_NATColumnBuffer + (03 * 2) + ((03 * 2) & 3), columnNTWritesEnd - (03 * 11)
.dw GSE_NATColumnBuffer + (04 * 2) + ((04 * 2) & 3), columnNTWritesEnd - (04 * 11)
.dw GSE_NATColumnBuffer + (05 * 2) + ((05 * 2) & 3), columnNTWritesEnd - (05 * 11)
.dw GSE_NATColumnBuffer + (06 * 2) + ((06 * 2) & 3), columnNTWritesEnd - (06 * 11)
.dw GSE_NATColumnBuffer + (07 * 2) + ((07 * 2) & 3), columnNTWritesEnd - (07 * 11)
.dw GSE_NATColumnBuffer + (08 * 2) + ((08 * 2) & 3), columnNTWritesEnd - (08 * 11)
.dw GSE_NATColumnBuffer + (09 * 2) + ((09 * 2) & 3), columnNTWritesEnd - (09 * 11)
.dw GSE_NATColumnBuffer + (10 * 2) + ((10 * 2) & 3), columnNTWritesEnd - (10 * 11)
.dw GSE_NATColumnBuffer + (11 * 2) + ((11 * 2) & 3), columnNTWritesEnd - (11 * 11)
.dw GSE_NATColumnBuffer + (12 * 2) + ((12 * 2) & 3), columnNTWritesEnd - (12 * 11)
.dw GSE_NATColumnBuffer + (13 * 2) + ((13 * 2) & 3), columnNTWritesEnd - (13 * 11)
.dw GSE_NATColumnBuffer + (14 * 2) + ((14 * 2) & 3), columnNTWritesEnd - (14 * 11)
.dw GSE_NATColumnBuffer + (15 * 2) + ((15 * 2) & 3), columnNTWritesEnd - (15 * 11)
.dw GSE_NATColumnBuffer + (16 * 2) + ((16 * 2) & 3), columnNTWritesEnd - (16 * 11)
.dw GSE_NATColumnBuffer + (17 * 2) + ((17 * 2) & 3), columnNTWritesEnd - (17 * 11)
.dw GSE_NATColumnBuffer + (18 * 2) + ((18 * 2) & 3), columnNTWritesEnd - (18 * 11)
.dw GSE_NATColumnBuffer + (19 * 2) + ((19 * 2) & 3), columnNTWritesEnd - (19 * 11)
.dw GSE_NATColumnBuffer + (20 * 2) + ((20 * 2) & 3), columnNTWritesEnd - (20 * 11)
.dw GSE_NATColumnBuffer + (21 * 2) + ((21 * 2) & 3), columnNTWritesEnd - (21 * 11)
.dw GSE_NATColumnBuffer + (22 * 2) + ((22 * 2) & 3), columnNTWritesEnd - (22 * 11)
.dw GSE_NATColumnBuffer + (23 * 2) + ((23 * 2) & 3), columnNTWritesEnd - (23 * 11)
.dw GSE_NATColumnBuffer + (24 * 2) + ((24 * 2) & 3), columnNTWritesEnd - (24 * 11)
.dw GSE_NATColumnBuffer + (25 * 2) + ((25 * 2) & 3), columnNTWritesEnd - (25 * 11)
.dw GSE_NATColumnBuffer + (26 * 2) + ((26 * 2) & 3), columnNTWritesEnd - (26 * 11)
.dw GSE_NATColumnBuffer + (27 * 2) + ((27 * 2) & 3), columnNTWritesEnd - (27 * 11)

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



.bank 0 slot 0
.org $0100
.section "eeee" force

ntRowUpdateTable1:
.dw GSE_NATRowBuffer + ((00 * 2) & 3) + 2, rowNTWritesEnd - (31 * 4)
.dw GSE_NATRowBuffer + ((01 * 2) & 3) + 2, rowNTWritesEnd - (30 * 4)
.dw GSE_NATRowBuffer + ((02 * 2) & 3) + 2, rowNTWritesEnd - (29 * 4)
.dw GSE_NATRowBuffer + ((03 * 2) & 3) + 2, rowNTWritesEnd - (28 * 4)
.dw GSE_NATRowBuffer + ((04 * 2) & 3) + 2, rowNTWritesEnd - (27 * 4)
.dw GSE_NATRowBuffer + ((05 * 2) & 3) + 2, rowNTWritesEnd - (26 * 4)
.dw GSE_NATRowBuffer + ((06 * 2) & 3) + 2, rowNTWritesEnd - (25 * 4)
.dw GSE_NATRowBuffer + ((07 * 2) & 3) + 2, rowNTWritesEnd - (24 * 4)
.dw GSE_NATRowBuffer + ((08 * 2) & 3) + 2, rowNTWritesEnd - (23 * 4)
.dw GSE_NATRowBuffer + ((09 * 2) & 3) + 2, rowNTWritesEnd - (22 * 4)
.dw GSE_NATRowBuffer + ((10 * 2) & 3) + 2, rowNTWritesEnd - (21 * 4)
.dw GSE_NATRowBuffer + ((11 * 2) & 3) + 2, rowNTWritesEnd - (20 * 4)
.dw GSE_NATRowBuffer + ((12 * 2) & 3) + 2, rowNTWritesEnd - (19 * 4)
.dw GSE_NATRowBuffer + ((13 * 2) & 3) + 2, rowNTWritesEnd - (18 * 4)
.dw GSE_NATRowBuffer + ((14 * 2) & 3) + 2, rowNTWritesEnd - (17 * 4)
.dw GSE_NATRowBuffer + ((15 * 2) & 3) + 2, rowNTWritesEnd - (16 * 4)
.dw GSE_NATRowBuffer + ((16 * 2) & 3) + 2, rowNTWritesEnd - (15 * 4)
.dw GSE_NATRowBuffer + ((17 * 2) & 3) + 2, rowNTWritesEnd - (14 * 4)
.dw GSE_NATRowBuffer + ((18 * 2) & 3) + 2, rowNTWritesEnd - (13 * 4)
.dw GSE_NATRowBuffer + ((19 * 2) & 3) + 2, rowNTWritesEnd - (12 * 4)
.dw GSE_NATRowBuffer + ((20 * 2) & 3) + 2, rowNTWritesEnd - (11 * 4)
.dw GSE_NATRowBuffer + ((21 * 2) & 3) + 2, rowNTWritesEnd - (10 * 4)
.dw GSE_NATRowBuffer + ((22 * 2) & 3) + 2, rowNTWritesEnd - (09 * 4)
.dw GSE_NATRowBuffer + ((23 * 2) & 3) + 2, rowNTWritesEnd - (08 * 4)
.dw GSE_NATRowBuffer + ((24 * 2) & 3) + 2, rowNTWritesEnd - (07 * 4)
.dw GSE_NATRowBuffer + ((25 * 2) & 3) + 2, rowNTWritesEnd - (06 * 4)
.dw GSE_NATRowBuffer + ((26 * 2) & 3) + 2, rowNTWritesEnd - (05 * 4)
.dw GSE_NATRowBuffer + ((27 * 2) & 3) + 2, rowNTWritesEnd - (04 * 4)
.dw GSE_NATRowBuffer + ((28 * 2) & 3) + 2, rowNTWritesEnd - (03 * 4)
.dw GSE_NATRowBuffer + ((29 * 2) & 3) + 2, rowNTWritesEnd - (02 * 4)
.dw GSE_NATRowBuffer + ((30 * 2) & 3) + 2, rowNTWritesEnd - (01 * 4)
.dw GSE_NATRowBuffer + ((31 * 2) & 3) + 2, rowNTWritesEnd - (00 * 4)

; NOTE I DIDNT CHANGE FIRST ENTRY SINCE I NO LONGER USE IT!!!
.dw GSE_NATRowBuffer + (00 * 2) + ((00 * 2) & 3), rowNTWritesEnd - (01 * 4)
.dw GSE_NATRowBuffer + (01 * 2) + ((01 * 2) & 3), rowNTWritesEnd - (02 * 4)
.dw GSE_NATRowBuffer + (02 * 2) + ((02 * 2) & 3), rowNTWritesEnd - (03 * 4)
.dw GSE_NATRowBuffer + (03 * 2) + ((03 * 2) & 3), rowNTWritesEnd - (04 * 4)
.dw GSE_NATRowBuffer + (04 * 2) + ((04 * 2) & 3), rowNTWritesEnd - (05 * 4)
.dw GSE_NATRowBuffer + (05 * 2) + ((05 * 2) & 3), rowNTWritesEnd - (06 * 4)
.dw GSE_NATRowBuffer + (06 * 2) + ((06 * 2) & 3), rowNTWritesEnd - (07 * 4)
.dw GSE_NATRowBuffer + (07 * 2) + ((07 * 2) & 3), rowNTWritesEnd - (08 * 4)
.dw GSE_NATRowBuffer + (08 * 2) + ((08 * 2) & 3), rowNTWritesEnd - (09 * 4)
.dw GSE_NATRowBuffer + (09 * 2) + ((09 * 2) & 3), rowNTWritesEnd - (10 * 4)
.dw GSE_NATRowBuffer + (10 * 2) + ((10 * 2) & 3), rowNTWritesEnd - (11 * 4)
.dw GSE_NATRowBuffer + (11 * 2) + ((11 * 2) & 3), rowNTWritesEnd - (12 * 4)
.dw GSE_NATRowBuffer + (12 * 2) + ((12 * 2) & 3), rowNTWritesEnd - (13 * 4)
.dw GSE_NATRowBuffer + (13 * 2) + ((13 * 2) & 3), rowNTWritesEnd - (14 * 4)
.dw GSE_NATRowBuffer + (14 * 2) + ((14 * 2) & 3), rowNTWritesEnd - (15 * 4)
.dw GSE_NATRowBuffer + (15 * 2) + ((15 * 2) & 3), rowNTWritesEnd - (16 * 4)
.dw GSE_NATRowBuffer + (16 * 2) + ((16 * 2) & 3), rowNTWritesEnd - (17 * 4)
.dw GSE_NATRowBuffer + (17 * 2) + ((17 * 2) & 3), rowNTWritesEnd - (18 * 4)
.dw GSE_NATRowBuffer + (18 * 2) + ((18 * 2) & 3), rowNTWritesEnd - (19 * 4)
.dw GSE_NATRowBuffer + (19 * 2) + ((19 * 2) & 3), rowNTWritesEnd - (20 * 4)
.dw GSE_NATRowBuffer + (20 * 2) + ((20 * 2) & 3), rowNTWritesEnd - (21 * 4)
.dw GSE_NATRowBuffer + (21 * 2) + ((21 * 2) & 3), rowNTWritesEnd - (22 * 4)
.dw GSE_NATRowBuffer + (22 * 2) + ((22 * 2) & 3), rowNTWritesEnd - (23 * 4)
.dw GSE_NATRowBuffer + (23 * 2) + ((23 * 2) & 3), rowNTWritesEnd - (24 * 4)
.dw GSE_NATRowBuffer + (24 * 2) + ((24 * 2) & 3), rowNTWritesEnd - (25 * 4)
.dw GSE_NATRowBuffer + (25 * 2) + ((25 * 2) & 3), rowNTWritesEnd - (26 * 4)
.dw GSE_NATRowBuffer + (26 * 2) + ((26 * 2) & 3), rowNTWritesEnd - (27 * 4)
.dw GSE_NATRowBuffer + (27 * 2) + ((27 * 2) & 3), rowNTWritesEnd - (28 * 4)
.dw GSE_NATRowBuffer + (28 * 2) + ((28 * 2) & 3), rowNTWritesEnd - (29 * 4)
.dw GSE_NATRowBuffer + (29 * 2) + ((29 * 2) & 3), rowNTWritesEnd - (30 * 4)
.dw GSE_NATRowBuffer + (30 * 2) + ((30 * 2) & 3), rowNTWritesEnd - (31 * 4)
.dw GSE_NATRowBuffer + (31 * 2) + ((31 * 2) & 3), rowNTWritesEnd - (32 * 4)


                            .repeat 32
                            outi
                            outi
                            .endr
rowNTWritesEnd:            ret

.ends
