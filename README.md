# README #

General Scroll Engine Library (GSELib)

### What is this repository for? ###

General purpose 8 way scrolling engine for use in games or demos. Specifications include...
 * 16x16 pixel metatiles. 
 * 8 way scrolling. 
 * Supports tile and metatile lookup given an (x, y). 
 * Supports updating metatile on screen (vdp nametable). Think rings or destructable environment. 
 * Released under creative commons, free to include in all projects including commercial. 


### How do I get set up? ###

In GSELib:

 * Set GSE_GENERAL_RAM to ram location for library (262 bytes by default)
 * Default location of Metatiles (on rom) is $4000. If another location is needed change GSE_METATILE_TABLE and GSE_METATILE_TABLE_HIGH_BYTE to desired location (must be on 2k boundary)
 * Default Location of Nametable (on vdp) is $7800. If another location is needed change GSE_NAMETABLE_BASE_ADDRESS, GSE_NAMETABLE_HIGH_BYTE_START, GSE_NAMETABLE_HIGH_BYTE_END
 * Do not modify remaining defines unless you know what you are doing (you don't!)
 
In Your Code:

 * Include Library In Code.
 * Load tiles and palette to VDP.
 * Call GSE_InitaliseMap with (hl = Scrolltable Data, de = ram location for lookup table)
 * Call GSE_PositionWindow with (hl = Y, de = X) 
 * Call GSE_RefreshScreen
 * Create Loop with calls to GSE_ActiveDisplayRoutine and GSE_VBlankRoutine.
 * To Scroll place desired signed values in ram values GSE_YUpdateRequest and GSE_XUpdateRequest (Max Range -8 to 8)


### Contribution guidelines ###

 * Released under Creative Commons. Feel free to copy, modify, etc.
 * If you want to contribute to project head over to http://www.smspower.org/forums/16800-GeneralScrollEngineLibraryGSELib.


### Who do I talk to? ###

 * You can contact me (psidum) on SMS Power Forums.
 * If you have questions please read http://www.smspower.org/forums/16800-GeneralScrollEngineLibraryGSELib before asking.