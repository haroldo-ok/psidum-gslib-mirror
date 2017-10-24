# README #

General Scroll Library (GSLib)

### What is this repository for? ###

General purpose 8 way scrolling engine for use in games or demos. Specifications include...

 * 16x16 pixel metatiles. 
 * 8 way scrolling. 
 * Supports tile and metatile lookup given an (x, y). 
 * Supports updating metatile on screen (vdp nametable). Think rings or destructable environment. 
 * Released under creative commons, free to include in all projects including commercial. 


### How do I get set up? ###

In GSLib:

 * Set GSL_GENERAL_RAM to ram location for library (262 bytes by default)
 * Default Location of Nametable (on vdp) is $7800. If another location is needed change GSE_NAMETABLE_BASE_ADDRESS, GSE_NAMETABLE_HIGH_BYTE_START, GSE_NAMETABLE_HIGH_BYTE_END
 * Do not modify remaining defines unless you know what you are doing (you don't!)
 
In Your Code:

 * Include Library In Code.
 * Load tiles and palette to VDP.
 * Call GSL_InitaliseMap with (hl = Scrolltable Data, bc = Metatile Data)
 * Call GSL_PositionWindow with (hl = Y, de = X) 
 * Call GSL_RefreshScreen
 * Create Loop with calls to GSL_ActiveDisplayRoutine and GSL_VBlankRoutine.
 * To Scroll place desired signed values in ram values GSL_YUpdateRequest and GSL_XUpdateRequest (Max Range -8 to 8)


### Contribution guidelines ###

 * Released under Creative Commons. Feel free to copy, modify, etc.
 * If you want to contribute to project head over to http://www.smspower.org/forums/16800-GeneralScrollEngineLibraryGSLib.


### Who do I talk to? ###

 * You can contact me (psidum) on SMS Power Forums.
 * If you have questions please read http://www.smspower.org/forums/16800-GeneralScrollEngineLibraryGSLib before asking.