

.bank 1 slot 1
.org $0000
.section "data" force


/*
 * Scrolltable is array of metatile id entries. left to right, top to bottom.
 * 
 * NOTE: Metatile id's are stored in processed form for speed on...
 * (metatile_id << 3) & 248) + ((metatile_id >> 5) & 7)
 * **************************************************************************
 */
Scrolltable:

.dw $0900
.dw $0040
.dw $0024
.dw $0400
.dw $0240
.dw $0340
.db $01


.db $28, $A8, $A8, $28, $28, $28, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $28, $28, $28, $28, $28, $A8, $A8, $A8, $A8, $A8, $E8, $28, $A8, $A8, $A8, $A8, $A8, $28
.db $70, $70, $70, $70, $08, $08, $08, $98, $08, $98, $08, $08, $08, $98, $70, $70, $70, $70, $70, $70, $08, $08, $08, $98, $08, $98, $08, $08, $08, $70, $70, $70
.db $28, $A8, $A8, $28, $28, $28, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $80, $80, $80, $80, $80, $A8, $A8, $A8, $A8, $A8, $E8, $28, $A8, $A8, $A8, $A8, $A8, $28
.db $70, $70, $70, $68, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $A0, $70, $70, $70, $70, $68, $39, $39, $39, $39, $39, $39, $39, $39, $39, $A0, $70, $70
.db $28, $A8, $A8, $28, $28, $28, $A8, $A8, $A8, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $28, $E8, $80, $A8, $A8, $A8, $A8, $A8, $28
.db $70, $70, $68, $39, $39, $39, $39, $F0, $39, $F0, $39, $39, $39, $39, $39, $A0, $08, $08, $68, $39, $39, $39, $39, $88, $39, $88, $39, $39, $39, $39, $A0, $70
.db $28, $A8, $A8, $80, $80, $80, $A8, $A8, $A8, $D8, $D8, $D8, $D8, $D8, $D8, $D8, $D8, $D8, $D8, $D8, $D8, $B0, $D8, $D8, $E8, $A8, $A8, $A8, $A8, $A8, $A8, $28
.db $70, $78, $39, $39, $39, $39, $39, $11, $39, $11, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $11, $39, $11, $39, $39, $39, $39, $39, $38
.db $28, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $E8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $28
.db $70, $78, $39, $39, $39, $39, $F0, $39, $19, $39, $F0, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $88, $39, $19, $39, $88, $39, $39, $39, $39, $38
.db $28, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $E8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $28
.db $70, $78, $39, $39, $39, $39, $11, $39, $39, $39, $11, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $11, $39, $39, $39, $11, $39, $39, $39, $39, $70
.db $28, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $E8, $A8, $A8, $A8, $A8, $28, $28, $28, $A8, $28, $28, $A8, $28, $28, $A8, $28, $28, $A8, $19, $28, $A8, $A8, $28
.db $70, $70, $20, $39, $39, $39, $39, $F0, $39, $F0, $39, $39, $39, $39, $90, $60, $60, $60, $20, $39, $39, $39, $39, $88, $39, $88, $39, $39, $39, $39, $90, $70
.db $28, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $E8, $A8, $A8, $A8, $A8, $80, $80, $80, $A8, $80, $80, $A8, $80, $80, $A8, $80, $80, $A8, $A8, $80, $A8, $A8, $28
.db $70, $70, $70, $20, $39, $39, $39, $11, $39, $11, $39, $39, $39, $90, $70, $70, $70, $70, $70, $20, $39, $39, $39, $11, $39, $11, $39, $39, $39, $90, $70, $70
.db $28, $A8, $A8, $A8, $A8, $A8, $A8, $28, $28, $E8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $28
.db $70, $70, $70, $70, $60, $60, $59, $39, $21, $60, $60, $60, $60, $70, $70, $70, $70, $70, $70, $70, $20, $90, $60, $60, $20, $90, $60, $60, $60, $70, $70, $70
.db $28, $A8, $A8, $A8, $A8, $A8, $28, $28, $28, $E8, $A8, $28, $28, $28, $A8, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
.db $70, $70, $70, $70, $70, $70, $70, $C0, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70
.db $28, $A8, $A8, $A8, $A8, $A8, $28, $28, $28, $E8, $A8, $28, $28, $28, $A8, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
.db $70, $70, $70, $70, $70, $70, $70, $C0, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70, $70
.db $28, $A8, $A8, $A8, $A8, $A8, $28, $28, $80, $E8, $A8, $80, $80, $80, $A8, $80, $80, $80, $80, $28, $28, $80, $80, $80, $80, $80, $80, $80, $28, $28, $28, $28
.db $70, $08, $08, $68, $A0, $98, $98, $51, $08, $08, $98, $98, $98, $08, $08, $08, $98, $08, $98, $08, $98, $08, $98, $98, $08, $98, $08, $98, $B8, $98, $08, $70
.db $28, $A8, $A8, $A8, $A8, $A8, $28, $28, $A8, $E8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $28, $28, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $28, $28, $28, $28
.db $68, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $88, $39, $88, $39, $A0
.db $28, $A8, $A8, $A8, $A8, $A8, $80, $80, $A8, $E8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $80, $80, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $80, $80, $80, $80
.db $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $88, $88, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $88, $88, $88, $39, $39
.db $28, $50, $50, $50, $50, $50, $50, $50, $50, $E8, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50, $50
.db $30, $30, $30, $30, $30, $39, $39, $39, $39, $88, $39, $88, $39, $39, $39, $88, $88, $39, $39, $88, $88, $39, $39, $39, $39, $39, $39, $11, $11, $11, $39, $39
.db $E0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $09, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0, $D0
.db $D0, $D0, $D0, $D0, $F8, $88, $39, $39, $39, $11, $39, $11, $39, $39, $39, $88, $88, $39, $39, $11, $88, $39, $88, $39, $88, $39, $39, $39, $39, $39, $39, $39
.db $E0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $31, $19, $A8, $A8, $29, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $F8, $39, $39, $39, $88, $39, $88, $39, $39, $39, $11, $11, $39, $39, $39, $11, $39, $11, $39, $11, $39, $39, $39, $39, $39, $39, $39
.db $E0, $E0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $31, $A8, $A8, $A8, $29, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $39, $39, $39, $11, $39, $11, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39
.db $E0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $31, $50, $50, $50, $29, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $88, $39, $39, $39, $39, $39, $39, $39, $39, $88, $88, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39, $39
.db $E0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $D0, $B0, $D0, $09, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $88, $39, $39, $39, $88, $39, $39, $39, $39, $88, $88, $88, $88, $88, $88, $88, $88, $88, $88, $88, $88, $88, $88, $88, $88, $88
.db $E0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $B0, $09, $09, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $28, $A8, $A8, $A8, $28, $A8, $A8, $A8, $A8, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
.db $E0, $09, $09, $09, $09, $E0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $31, $28, $58, $28, $29, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $80, $A8, $A8, $A8, $80, $A8, $A8, $A8, $A8, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
.db $E0, $09, $09, $31, $28, $28, $28, $29, $09, $09, $09, $09, $09, $09, $09, $09, $09, $31, $28, $28, $61, $28, $58, $28, $61, $A8, $58, $58, $58, $28, $29, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $10, $10, $10, $10, $10, $10, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
.db $E0, $09, $09, $31, $28, $19, $80, $29, $09, $09, $09, $09, $09, $09, $09, $09, $09, $31, $80, $28, $61, $28, $58, $28, $49, $A8, $58, $58, $58, $58, $29, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $40, $40, $40, $40, $40, $40, $80, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
.db $E0, $09, $09, $31, $80, $A8, $A8, $29, $09, $09, $09, $09, $09, $09, $09, $09, $09, $31, $A8, $80, $61, $28, $58, $58, $18, $50, $50, $A8, $58, $58, $29, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $80, $80, $80, $80, $80, $80, $A8, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
.db $E0, $09, $09, $C8, $50, $50, $50, $29, $09, $31, $28, $28, $28, $28, $29, $09, $09, $31, $A8, $A8, $29, $F8, $50, $50, $29, $D0, $F8, $50, $50, $50, $29, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80
.db $E0, $09, $09, $09, $D0, $D0, $D0, $09, $09, $31, $28, $28, $80, $80, $49, $49, $49, $49, $28, $A8, $29, $09, $D0, $D0, $09, $09, $09, $D0, $49, $D0, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $10, $10, $10, $10, $10, $10, $A8, $28, $28, $28, $28, $28, $28, $A8, $A8, $28, $28, $A8, $28, $A8, $28, $28, $28, $28
.db $E0, $E0, $09, $09, $09, $09, $09, $09, $09, $31, $28, $80, $A8, $A8, $01, $48, $48, $41, $50, $50, $61, $28, $A8, $A8, $29, $09, $31, $A8, $A8, $50, $29, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $40, $40, $40, $40, $40, $40, $A8, $28, $28, $28, $28, $28, $28, $28, $A8, $28, $28, $28, $28, $A8, $28, $28, $28, $28
.db $E0, $E0, $E0, $09, $09, $09, $09, $09, $09, $31, $28, $50, $A8, $50, $29, $09, $09, $09, $D0, $D0, $31, $50, $50, $50, $49, $49, $49, $50, $50, $C8, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $80, $80, $80, $80, $80, $80, $A8, $28, $28, $28, $28, $28, $28, $28, $A8, $28, $28, $28, $28, $A8, $28, $28, $28, $28
.db $E0, $E0, $E0, $09, $09, $09, $09, $09, $09, $09, $D0, $D0, $B0, $D0, $09, $09, $09, $09, $09, $09, $09, $D0, $D0, $D0, $48, $48, $48, $D0, $D0, $48, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $28, $28, $28, $28, $28, $28, $28, $A8, $28, $28, $28, $28, $A8, $28, $28, $28, $28
.db $E0, $E0, $E0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $B0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $28, $28, $28, $28, $28, $28, $28, $A8, $28, $28, $28, $28, $A8, $28, $28, $28, $28
.db $E0, $E0, $09, $09, $31, $A8, $10, $A8, $A8, $29, $09, $09, $B0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $10, $10, $10, $10, $10, $10, $A8, $28, $28, $28, $80, $28, $28, $28, $A8, $28, $28, $28, $28, $A8, $28, $28, $28, $28
.db $E0, $09, $E0, $09, $31, $A8, $40, $A8, $A8, $29, $09, $09, $B0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $40, $40, $40, $40, $40, $40, $A8, $80, $80, $80, $A8, $80, $80, $80, $A8, $80, $28, $28, $28, $A8, $80, $80, $80, $80
.db $E0, $09, $09, $09, $31, $50, $80, $50, $50, $29, $09, $09, $B0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $80, $80, $80, $80, $80, $80, $A8, $28, $28, $28, $28, $28, $A8, $28, $28, $A8, $28, $28, $28, $A8, $A8, $A8, $A8, $A8
.db $E0, $E0, $09, $09, $09, $D0, $B0, $D0, $C8, $09, $09, $09, $B0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $10, $10, $10, $10, $10, $10, $A8, $28, $28, $28, $28, $28, $28, $28, $28, $A8, $28, $28, $80, $28, $28, $28, $28, $28
.db $E0, $09, $E0, $09, $09, $09, $B0, $09, $09, $09, $09, $09, $B0, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09, $09
.db $09, $09, $09, $09, $09, $31, $A8, $A8, $40, $40, $40, $40, $40, $40, $A8, $28, $28, $28, $28, $28, $28, $28, $28, $A8, $80, $80, $A8, $80, $80, $80, $28, $28

ScrolltableEnd:



/*
 * Metatiles are 4x4 nametable entries left to right, top to bottom.
 * *****************************************************************
 */
Metatiles:
.dw $0168
.db $00, $00
.dw $0000
.dw $0000
.dw $0004, $0005, $000A, $000B, $0002, $0002, $0043, $0243, $004A, $024A, $063E, $003E, $0026, $0027, $0008, $0009
.dw $0000, $0001, $0006, $0007, $0010, $0010, $0036, $0036, $001F, $0005, $0021, $0009, $0045, $0046, $0048, $0049
.dw $004D, $004D, $003A, $003A, $0002, $0002, $0017, $0017, $003F, $0040, $0041, $0042, $0026, $0029, $0008, $0009
.dw $0004, $000F, $0012, $0013, $0004, $0005, $0008, $0009, $0004, $001D, $0008, $0020, $000E, $020E, $0002, $0002
.dw $0016, $0216, $0019, $001A, $0028, $0029, $002A, $0009, $0004, $0005, $000C, $000D, $0011, $0005, $0014, $000B
.dw $0002, $0002, $0002, $0002, $001C, $021C, $001C, $021C, $0033, $0033, $0033, $0033, $002F, $0030, $0031, $0032
.dw $004B, $024B, $004C, $024C, $0039, $0039, $003A, $003A, $001B, $001B, $0003, $0003, $0037, $0038, $003C, $003D
.dw $0003, $0003, $0003, $0003, $0015, $0215, $0018, $0218, $0039, $003B, $003A, $003E, $004A, $004D, $063E, $003A
.dw $003A, $003A, $003A, $003A, $001E, $021E, $0010, $0010, $0022, $0023, $0024, $0025, $0010, $002C, $002E, $0009
.dw $063E, $003A, $063E, $003A, $003A, $003E, $003A, $003E, $0010, $0010, $0010, $0010, $004D, $024A, $003A, $003E
.dw $0044, $0044, $0047, $0047, $002F, $0030, $0034, $0035, $002B, $0210, $0008, $002D, $063E, $003E, $063E, $003E


MetatilesEnd:


.ends