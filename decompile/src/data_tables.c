/*
 * data_tables.c - Oil Baron Global Data Tables, Memory Layout & Runtime Stubs
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - DGROUP (Segment 10a8) memory layout and data structures.
 *   - Company definitions, baseline tiers, pricing curves.
 *   - Board grid layout constants and coordinate conversions.
 *   - Player structure fields (Cash, Stock Portfolio, Hand tiles).
 *   - Borland Pascal runtime stubs (Segment 10a0) and Windows API import stubs (Segment 1230).
 */

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================= */
/* OIL BARON REVERSE-ENGINEERED CONSTANTS & DATA DEFINITIONS                 */
/* ========================================================================= */

/* Company Identifiers */
typedef enum {
    COMPANY_NONE     = 0,
    COMPANY_IMPERIAL = 1,  /* High tier: Imperial Oil and Gas                */
    COMPANY_ROYAL    = 2,  /* Low tier:  Colored Cube Oil Co.                */
    COMPANY_AMERICAN = 3,  /* Mid tier:  American Natural Gas Co.            */
    COMPANY_ARABIAN  = 4,  /* Mid tier:  Oil Arabia, Inc.                    */
    COMPANY_TOWER    = 5,  /* Low tier:  Pyramid Oil Co.                     */
    COMPANY_SUPRA    = 6,  /* High tier: SupraCo Fuel Resources              */
    COMPANY_WEEMOCO  = 7,  /* High tier: Weem Oil Refining                   */
    TILE_PURPLE      = 8,  /* Single placed chit (unformed company)          */
    TILE_BLANK       = 9,  /* Unplayable dead tile                           */
    TILE_DISCARD     = 10  /* Discard square marker                          */
} CompanyId;

/* Company Pricing Tiers */
typedef enum {
    TIER_LOW  = 0,  /* Tower, Royal ($200 base price)                        */
    TIER_MID  = 1,  /* American, Arabian ($300 base price)                   */
    TIER_HIGH = 2   /* Imperial, Supra, Weemoco ($400 base price)            */
} CompanyTier;

/* Company Static Descriptor (36 bytes = 0x24 in DGROUP) */
typedef struct {
    char        name[32];          /* Full company name                      */
    CompanyTier tier;              /* Low, Mid, High                         */
    uint16_t    base_price;        /* Starting price when formed             */
    uint16_t    total_shares;      /* Total share limit (default 25)         */
    uint16_t    safe_size;         /* Immune from takeover threshold (11)    */
} CompanyInfo;

static const CompanyInfo OIL_COMPANIES[8] = {
    { "None",                     TIER_LOW,    0,    0,  0 },
    { "Imperial Oil and Gas",     TIER_HIGH, 400,   25, 11 },
    { "Colored Cube Oil Co.",     TIER_LOW,  200,   25, 11 },
    { "American Natural Gas Co.", TIER_MID,  300,   25, 11 },
    { "Oil Arabia, Inc.",         TIER_MID,  300,   25, 11 },
    { "Pyramid Oil Co.",          TIER_LOW,  200,   25, 11 },
    { "SupraCo Fuel Resources",   TIER_HIGH, 400,   25, 11 },
    { "Weem Oil Refining",        TIER_HIGH, 400,   25, 11 },
};

/* Board Dimensions & Memory Geometry */
#define BOARD_MIN_ROWS       12
#define BOARD_MIN_COLS       12
#define BOARD_DEFAULT_ROWS   12
#define BOARD_DEFAULT_COLS   12
#define BOARD_ROW_STRIDE     0x8c  /* 140 bytes per row in memory             */
#define BOARD_CELL_SIZE      5     /* 5 bytes per board grid square           */
#define BOARD_MAX_HAND_TILES 14    /* Max hand chits (default 6)              */

/* Player Struct Layout (252 bytes = 0xfc in DGROUP at DAT_10a8_30aa) */
typedef struct {
    char     name[32];             /* Player name (e.g. "Player1")           */
    bool     is_computer;          /* 0 = Human, 1 = Computer AI             */
    int32_t  cash;                 /* Player cash balance                    */
    uint16_t shares[8];            /* Shares held per company (1..7)         */
    uint16_t hand_tiles[14];       /* Chits currently in player hand         */
    uint16_t hand_count;           /* Active tiles in hand                   */
} PlayerData;

/* Stock Pricing Curve Matrix: Price($) based on Territory Tile Count */
/* [Tier 0: Low, Tier 1: Mid, Tier 2: High] */
typedef struct {
    uint16_t min_tiles;
    uint16_t max_tiles;
    uint16_t price_low;
    uint16_t price_mid;
    uint16_t price_high;
} StockPriceBracket;

static const StockPriceBracket STOCK_PRICE_CURVE[] = {
    {  2,  2, 200, 300, 400 },
    {  3,  3, 300, 400, 500 },
    {  4,  4, 400, 500, 600 },
    {  5,  5, 500, 600, 700 },
    {  6, 10, 600, 700, 800 },
    { 11, 20, 700, 800, 900 },
    { 21, 30, 800, 900, 1000 },
    { 31, 40, 900, 1000, 1100 },
    { 41, 999, 1000, 1100, 1200 }
};

/* End Game Conditions:
 * 1. All active companies have size >= 11 (Safe)
 * 2. Any single active company reaches size >= 41
 */
#define SAFE_COMPANY_THRESHOLD 11
#define END_GAME_TILE_COUNT    41

/* ========================================================================= */
/* DECOMPILED RUNTIME AND IMPORT STUBS                                      */
/* ========================================================================= */

/* Function Index:
 *   FUN_1008_45ab        @ 1008:45ab
 *   FUN_10a0_0002        @ 10a0:0002
 *   FUN_10a0_005d        @ 10a0:005d
 *   FUN_10a0_0061        @ 10a0:0061
 *   FUN_10a0_00d2        @ 10a0:00d2
 *   FUN_10a0_00f0        @ 10a0:00f0
 *   FUN_10a0_012d        @ 10a0:012d
 *   FUN_10a0_0147        @ 10a0:0147
 *   FUN_10a0_01ca        @ 10a0:01ca
 *   FUN_10a0_0222        @ 10a0:0222
 *   FUN_10a0_023c        @ 10a0:023c
 *   FUN_10a0_026a        @ 10a0:026a
 *   FUN_10a0_02a6        @ 10a0:02a6
 *   FUN_10a0_02d7        @ 10a0:02d7
 *   FUN_10a0_0327        @ 10a0:0327
 *   FUN_10a0_0388        @ 10a0:0388
 *   FUN_10a0_03ef        @ 10a0:03ef
 *   FUN_10a0_0439        @ 10a0:0439
 *   FUN_10a0_049f        @ 10a0:049f
 *   FUN_10a0_0527        @ 10a0:0527
 *   FUN_10a0_052c        @ 10a0:052c
 *   FUN_10a0_0534        @ 10a0:0534
 *   FUN_10a0_0586        @ 10a0:0586
 *   FUN_10a0_05c7        @ 10a0:05c7
 *   FUN_10a0_05d8        @ 10a0:05d8
 *   FUN_10a0_0608        @ 10a0:0608
 *   FUN_10a0_062d        @ 10a0:062d
 *   FUN_10a0_064d        @ 10a0:064d
 *   FUN_10a0_0666        @ 10a0:0666
 *   FUN_10a0_06c4        @ 10a0:06c4
 *   FUN_10a0_0713        @ 10a0:0713
 *   FUN_10a0_076a        @ 10a0:076a
 *   FUN_10a0_07af        @ 10a0:07af
 *   FUN_10a0_07f5        @ 10a0:07f5
 *   FUN_10a0_0803        @ 10a0:0803
 *   FUN_10a0_0811        @ 10a0:0811
 *   FUN_10a0_085c        @ 10a0:085c
 *   FUN_10a0_08ac        @ 10a0:08ac
 *   FUN_10a0_08e9        @ 10a0:08e9
 *   FUN_10a0_092d        @ 10a0:092d
 *   FUN_10a0_098c        @ 10a0:098c
 *   FUN_10a0_09c3        @ 10a0:09c3
 *   FUN_10a0_0a24        @ 10a0:0a24
 *   FUN_10a0_0a72        @ 10a0:0a72
 *   FUN_10a0_0abf        @ 10a0:0abf
 *   FUN_10a0_0af5        @ 10a0:0af5
 *   FUN_10a0_0b5a        @ 10a0:0b5a
 *   FUN_10a0_0b92        @ 10a0:0b92
 *   FUN_10a0_0baa        @ 10a0:0baa
 *   FUN_10a0_0be7        @ 10a0:0be7
 *   FUN_10a0_0cbf        @ 10a0:0cbf
 *   FUN_10a0_0cd9        @ 10a0:0cd9
 *   FUN_10a0_0cfd        @ 10a0:0cfd
 *   FUN_10a0_0d3e        @ 10a0:0d3e
 *   FUN_10a0_0d6a        @ 10a0:0d6a
 *   FUN_10a0_0e08        @ 10a0:0e08
 *   FUN_10a0_0e67        @ 10a0:0e67
 *   FUN_10a0_0f00        @ 10a0:0f00
 *   FUN_10a0_0f16        @ 10a0:0f16
 *   FUN_10a0_0f8b        @ 10a0:0f8b
 *   FUN_10a0_1036        @ 10a0:1036
 *   FUN_10a0_10c3        @ 10a0:10c3
 *   FUN_10a0_111d        @ 10a0:111d
 *   FUN_10a0_1121        @ 10a0:1121
 *   FUN_10a0_11e4        @ 10a0:11e4
 *   FUN_10a0_12e0        @ 10a0:12e0
 *   FUN_10a0_12e7        @ 10a0:12e7
 *   FUN_10a0_135e        @ 10a0:135e
 *   FUN_10a0_1375        @ 10a0:1375
 *   FUN_10a0_1388        @ 10a0:1388
 *   FUN_10a0_13c4        @ 10a0:13c4
 *   FUN_10a0_1432        @ 10a0:1432
 *   FUN_10a0_1438        @ 10a0:1438
 *   FUN_10a0_1444        @ 10a0:1444
 *   FUN_10a0_144a        @ 10a0:144a
 *   FUN_10a0_1454        @ 10a0:1454
 *   FUN_10a0_1458        @ 10a0:1458
 *   FUN_10a0_145c        @ 10a0:145c
 *   FUN_10a0_1464        @ 10a0:1464
 *   FUN_10a0_147e        @ 10a0:147e
 *   FUN_10a0_149c        @ 10a0:149c
 *   FUN_10a0_150b        @ 10a0:150b
 *   FUN_10a0_16b1        @ 10a0:16b1
 *   FUN_10a0_18a2        @ 10a0:18a2
 *   FUN_10a0_18f1        @ 10a0:18f1
 *   FUN_10a0_194e        @ 10a0:194e
 *   FUN_10a0_1986        @ 10a0:1986
 *   FUN_10a0_1993        @ 10a0:1993
 *   FUN_10a0_19af        @ 10a0:19af
 *   FUN_10a0_19cf        @ 10a0:19cf
 *   FUN_10a0_1a76        @ 10a0:1a76
 *   FUN_10a0_1b62        @ 10a0:1b62
 *   FUN_10a0_1b6e        @ 10a0:1b6e
 *   FUN_10a0_1bf9        @ 10a0:1bf9
 *   FUN_10a0_1ca5        @ 10a0:1ca5
 *   FUN_10a0_1ce3        @ 10a0:1ce3
 *   FUN_10a0_1d6f        @ 10a0:1d6f
 *   FUN_10a0_1dbd        @ 10a0:1dbd
 *   FUN_10a0_1e08        @ 10a0:1e08
 *   FUN_10a0_1e39        @ 10a0:1e39
 *   FUN_10a0_1e71        @ 10a0:1e71
 *   FUN_10a0_1e88        @ 10a0:1e88
 *   FUN_10a0_1e8d        @ 10a0:1e8d
 *   GLOBALALLOC          @ 1230:0000
 *   GLOBALFREE           @ 1230:0004
 *   GLOBALLOCK           @ 1230:0008
 *   GLOBALUNLOCK         @ 1230:000c
 *   GLOBALHANDLE         @ 1230:0010
 *   GLOBALCOMPACT        @ 1230:0014
 *   WAITEVENT            @ 1230:0018
 *   MAKEPROCINSTANCE     @ 1230:001c
 *   FREEPROCINSTANCE     @ 1230:0020
 *   GETPROFILEINT        @ 1230:0024
 *   GETPROFILESTRING     @ 1230:0028
 *   WRITEPROFILESTRING   @ 1230:002c
 *   OPENFILE             @ 1230:0030
 *   INITTASK             @ 1230:0034
 *   __AHINCR             @ 1230:0038
 *   GETPRIVATEPROFILEINT @ 1230:003c
 *   GETPRIVATEPROFILESTRING @ 1230:0040
 *   WRITEPRIVATEPROFILESTRING @ 1230:0044
 *   GETWINFLAGS          @ 1230:0048
 *   GETFREESPACE         @ 1230:004c
 *   ALLOCCSTODSALIAS     @ 1230:0050
 *   FREESELECTOR         @ 1230:0054
 *   PRESTOCHANGOSELECTOR @ 1230:0058
 *   SETBKCOLOR           @ 1230:005c
 *   SETBKMODE            @ 1230:0060
 *   SETMAPMODE           @ 1230:0064
 *   SETSTRETCHBLTMODE    @ 1230:0068
 *   SETTEXTCOLOR         @ 1230:006c
 *   SETWINDOWEXT         @ 1230:0070
 *   SETVIEWPORTORG       @ 1230:0074
 *   SETVIEWPORTEXT       @ 1230:0078
 *   LINETO               @ 1230:007c
 *   MOVETO               @ 1230:0080
 *   RECTANGLE            @ 1230:0084
 *   BITBLT               @ 1230:0088
 *   STRETCHBLT           @ 1230:008c
 *   SELECTOBJECT         @ 1230:0090
 *   CREATEBRUSHINDIRECT  @ 1230:0094
 *   CREATECOMPATIBLEBITMAP @ 1230:0098
 *   CREATECOMPATIBLEDC   @ 1230:009c
 *   CREATEFONT           @ 1230:00a0
 *   CREATEPEN            @ 1230:00a4
 *   CREATESOLIDBRUSH     @ 1230:00a8
 *   DELETEDC             @ 1230:00ac
 *   DELETEOBJECT         @ 1230:00b0
 *   GETDEVICECAPS        @ 1230:00b4
 *   GETOBJECT            @ 1230:00b8
 *   GETSTOCKOBJECT       @ 1230:00bc
 *   GETTEXTMETRICS       @ 1230:00c0
 *   GETNEARESTCOLOR      @ 1230:00c4
 *   MESSAGEBOX           @ 1230:00c8
 *   INITAPP              @ 1230:00cc
 *   POSTQUITMESSAGE      @ 1230:00d0
 *   GETCURSORPOS         @ 1230:00d4
 *   SETCAPTURE           @ 1230:00d8
 *   RELEASECAPTURE       @ 1230:00dc
 *   SETFOCUS             @ 1230:00e0
 *   GETFOCUS             @ 1230:00e4
 *   REMOVEPROP           @ 1230:00e8
 *   GETPROP              @ 1230:00ec
 *   SETPROP              @ 1230:00f0
 *   SCREENTOCLIENT       @ 1230:00f4
 *   ISICONIC             @ 1230:00f8
 *   GETWINDOWRECT        @ 1230:00fc
 *   GETCLIENTRECT        @ 1230:0100
 *   ENABLEWINDOW         @ 1230:0104
 *   GETWINDOWTEXT        @ 1230:0108
 *   SETWINDOWTEXT        @ 1230:010c
 *   BEGINPAINT           @ 1230:0110
 *   ENDPAINT             @ 1230:0114
 *   SHOWWINDOW           @ 1230:0118
 *   GETPARENT            @ 1230:011c
 *   ISWINDOW             @ 1230:0120
 *   ISCHILD              @ 1230:0124
 *   DESTROYWINDOW        @ 1230:0128
 *   MOVEWINDOW           @ 1230:012c
 *   REGISTERCLASS        @ 1230:0130
 *   GETDC                @ 1230:0134
 *   RELEASEDC            @ 1230:0138
 *   SETCURSORPOS         @ 1230:013c
 *   DRAWTEXT             @ 1230:0140
 *   ENDDIALOG            @ 1230:0144
 *   ISDIALOGMESSAGE      @ 1230:0148
 *   GETDLGITEM           @ 1230:014c
 *   SETDLGITEMTEXT       @ 1230:0150
 *   GETDLGITEMTEXT       @ 1230:0154
 *   SETDLGITEMINT        @ 1230:0158
 *   GETDLGITEMINT        @ 1230:015c
 *   CHECKRADIOBUTTON     @ 1230:0160
 *   CHECKDLGBUTTON       @ 1230:0164
 *   DLGDIRSELECT         @ 1230:0168
 *   DLGDIRLIST           @ 1230:016c
 *   SENDDLGITEMMESSAGE   @ 1230:0170
 *   MESSAGEBEEP          @ 1230:0174
 *   DEFWINDOWPROC        @ 1230:0178
 *   PEEKMESSAGE          @ 1230:017c
 *   POSTMESSAGE          @ 1230:0180
 *   SENDMESSAGE          @ 1230:0184
 *   WAITMESSAGE          @ 1230:0188
 *   TRANSLATEMESSAGE     @ 1230:018c
 *   DISPATCHMESSAGE      @ 1230:0190
 *   CALLWINDOWPROC       @ 1230:0194
 *   INVALIDATERECT       @ 1230:0198
 *   GETWINDOWWORD        @ 1230:019c
 *   GETWINDOWLONG        @ 1230:01a0
 *   SETWINDOWLONG        @ 1230:01a4
 *   LOADMENU             @ 1230:01a8
 *   DESTROYMENU          @ 1230:01ac
 *   CHECKMENUITEM        @ 1230:01b0
 *   ENABLEMENUITEM       @ 1230:01b4
 *   GETMENU              @ 1230:01b8
 *   SETMENU              @ 1230:01bc
 *   GETSUBMENU           @ 1230:01c0
 *   WINHELP              @ 1230:01c4
 *   LOADCURSOR           @ 1230:01c8
 *   LOADICON             @ 1230:01cc
 *   LOADBITMAP           @ 1230:01d0
 *   LOADACCELERATORS     @ 1230:01d4
 *   TRANSLATEACCELERATOR @ 1230:01d8
 *   GETSYSTEMMETRICS     @ 1230:01dc
 *   DIALOGBOXPARAM       @ 1230:01e0
 *   CREATEDIALOGPARAM    @ 1230:01e4
 *   ISZOOMED             @ 1230:01e8
 *   DEFDLGPROC           @ 1230:01ec
 *   GETCLASSINFO         @ 1230:01f0
 *   MODIFYMENU           @ 1230:01f4
 *   TRACKPOPUPMENU       @ 1230:01f8
 *   WVSPRINTF            @ 1230:01fc
 *   ANSIUPPER            @ 1230:0200
 *   ANSILOWER            @ 1230:0204
 *   DEFMDICHILDPROC      @ 1230:0208
 *   TRANSLATEMDISYSACCEL @ 1230:020c
 *   CREATEWINDOWEX       @ 1230:0210
 *   Ordinal_5            @ 1230:0214
 *   Ordinal_6            @ 1230:0218
 *   GLOBALALLOC          @ 1230:021c
 *   GLOBALFREE           @ 1230:0220
 *   GLOBALLOCK           @ 1230:0224
 *   GLOBALUNLOCK         @ 1230:0228
 *   GLOBALHANDLE         @ 1230:022c
 *   GLOBALCOMPACT        @ 1230:0230
 *   WAITEVENT            @ 1230:0234
 *   MAKEPROCINSTANCE     @ 1230:0238
 *   FREEPROCINSTANCE     @ 1230:023c
 *   GETPROFILEINT        @ 1230:0240
 *   GETPROFILESTRING     @ 1230:0244
 *   WRITEPROFILESTRING   @ 1230:0248
 *   OPENFILE             @ 1230:024c
 *   INITTASK             @ 1230:0250
 *   __AHINCR             @ 1230:0254
 *   GETPRIVATEPROFILEINT @ 1230:0258
 *   GETPRIVATEPROFILESTRING @ 1230:025c
 *   WRITEPRIVATEPROFILESTRING @ 1230:0260
 *   GETWINFLAGS          @ 1230:0264
 *   GETFREESPACE         @ 1230:0268
 *   ALLOCCSTODSALIAS     @ 1230:026c
 *   FREESELECTOR         @ 1230:0270
 *   PRESTOCHANGOSELECTOR @ 1230:0274
 *   MESSAGEBOX           @ 1230:0278
 *   INITAPP              @ 1230:027c
 *   POSTQUITMESSAGE      @ 1230:0280
 *   GETCURSORPOS         @ 1230:0284
 *   SETCAPTURE           @ 1230:0288
 *   RELEASECAPTURE       @ 1230:028c
 *   SETFOCUS             @ 1230:0290
 *   GETFOCUS             @ 1230:0294
 *   REMOVEPROP           @ 1230:0298
 *   GETPROP              @ 1230:029c
 *   SETPROP              @ 1230:02a0
 *   SCREENTOCLIENT       @ 1230:02a4
 *   ISICONIC             @ 1230:02a8
 *   GETWINDOWRECT        @ 1230:02ac
 *   GETCLIENTRECT        @ 1230:02b0
 *   ENABLEWINDOW         @ 1230:02b4
 *   GETWINDOWTEXT        @ 1230:02b8
 *   SETWINDOWTEXT        @ 1230:02bc
 *   BEGINPAINT           @ 1230:02c0
 *   ENDPAINT             @ 1230:02c4
 *   SHOWWINDOW           @ 1230:02c8
 *   GETPARENT            @ 1230:02cc
 *   ISWINDOW             @ 1230:02d0
 *   ISCHILD              @ 1230:02d4
 *   DESTROYWINDOW        @ 1230:02d8
 *   MOVEWINDOW           @ 1230:02dc
 *   REGISTERCLASS        @ 1230:02e0
 *   GETDC                @ 1230:02e4
 *   RELEASEDC            @ 1230:02e8
 *   SETCURSORPOS         @ 1230:02ec
 *   DRAWTEXT             @ 1230:02f0
 *   ENDDIALOG            @ 1230:02f4
 *   ISDIALOGMESSAGE      @ 1230:02f8
 *   GETDLGITEM           @ 1230:02fc
 *   SETDLGITEMTEXT       @ 1230:0300
 *   GETDLGITEMTEXT       @ 1230:0304
 *   SETDLGITEMINT        @ 1230:0308
 *   GETDLGITEMINT        @ 1230:030c
 *   CHECKRADIOBUTTON     @ 1230:0310
 *   CHECKDLGBUTTON       @ 1230:0314
 *   DLGDIRSELECT         @ 1230:0318
 *   DLGDIRLIST           @ 1230:031c
 *   SENDDLGITEMMESSAGE   @ 1230:0320
 *   MESSAGEBEEP          @ 1230:0324
 *   DEFWINDOWPROC        @ 1230:0328
 *   PEEKMESSAGE          @ 1230:032c
 *   POSTMESSAGE          @ 1230:0330
 *   SENDMESSAGE          @ 1230:0334
 *   WAITMESSAGE          @ 1230:0338
 *   TRANSLATEMESSAGE     @ 1230:033c
 *   DISPATCHMESSAGE      @ 1230:0340
 *   CALLWINDOWPROC       @ 1230:0344
 *   INVALIDATERECT       @ 1230:0348
 *   GETWINDOWWORD        @ 1230:034c
 *   GETWINDOWLONG        @ 1230:0350
 *   SETWINDOWLONG        @ 1230:0354
 *   LOADMENU             @ 1230:0358
 *   DESTROYMENU          @ 1230:035c
 *   CHECKMENUITEM        @ 1230:0360
 *   ENABLEMENUITEM       @ 1230:0364
 *   GETMENU              @ 1230:0368
 *   SETMENU              @ 1230:036c
 *   GETSUBMENU           @ 1230:0370
 *   WINHELP              @ 1230:0374
 *   LOADCURSOR           @ 1230:0378
 *   LOADICON             @ 1230:037c
 *   LOADBITMAP           @ 1230:0380
 *   LOADACCELERATORS     @ 1230:0384
 *   TRANSLATEACCELERATOR @ 1230:0388
 *   GETSYSTEMMETRICS     @ 1230:038c
 *   DIALOGBOXPARAM       @ 1230:0390
 *   CREATEDIALOGPARAM    @ 1230:0394
 *   ISZOOMED             @ 1230:0398
 *   DEFDLGPROC           @ 1230:039c
 *   GETCLASSINFO         @ 1230:03a0
 *   MODIFYMENU           @ 1230:03a4
 *   TRACKPOPUPMENU       @ 1230:03a8
 *   WVSPRINTF            @ 1230:03ac
 *   ANSIUPPER            @ 1230:03b0
 *   ANSILOWER            @ 1230:03b4
 *   DEFMDICHILDPROC      @ 1230:03b8
 *   TRANSLATEMDISYSACCEL @ 1230:03bc
 *   CREATEWINDOWEX       @ 1230:03c0
 *   Ordinal_5            @ 1230:03c4
 *   Ordinal_6            @ 1230:03c8
 */

/* ======================================================== */
/* Function: FUN_1008_45ab @ 1008:45ab */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10084b0f) */

void __stdcall16far FUN_1008_45ab(int *param_1,undefined4 param_2)

{
  undefined2 *puVar1;
  char cVar2;
  undefined2 uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  int iVar8;
  uint uVar9;
  int unaff_BP;
  int *piVar10;
  int *piVar11;
  undefined2 uVar12;
  char *pcVar13;
  undefined2 unaff_SS;
  undefined1 uVar14;
  bool bVar15;
  undefined1 uVar16;
  bool bVar17;
  int local_58a [19];
  char local_564;
  undefined1 local_563;
  char local_562;
  undefined2 local_561;
  uint local_55f;
  undefined1 local_55d [1356];
  char local_11;
  uint local_10;
  int local_e;
  uint local_c;
  uint local_a;
  uint local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar12 = (undefined2)((ulong)param_2 >> 0x10);
  if (DAT_10a8_1d2f == '\0') {
    if (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\0') {
      iVar5 = 0;
      local_8 = FUN_10a0_0be7();
      iVar6 = 0;
      iVar8 = DAT_10a8_38b6;
      local_6 = iVar5;
      local_c = FUN_10a0_0be7();
      iVar5 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
      local_a = iVar6;
      if ((iVar5 < iVar6) || ((iVar5 <= iVar6 && (DAT_10a8_22b6 - 1 < local_c)))) {
        if (DAT_10a8_1d31 == '\0') {
          uVar4 = 2;
          iVar5 = 0;
          FUN_10a0_0be7();
          iVar8 = DAT_10a8_22b0;
          uVar9 = FUN_10a0_0be7(uVar4,iVar5);
          iVar8 = iVar8 + iVar5 + (uint)CARRY2(uVar9,uVar4);
          uVar3 = FUN_10a0_1458();
          uVar7 = DAT_10a8_38b2;
          FUN_10a0_1458(uVar3,iVar5,iVar8);
          FUN_10a0_1444();
          uVar3 = FUN_10a0_1444();
          uVar14 = 0;
          uVar16 = 1;
          FUN_10a0_1458(uVar3,iVar5,uVar7);
          FUN_10a0_1454();
          if ((bool)uVar14 || (bool)uVar16) {
            uVar4 = 2;
            iVar6 = 0;
            FUN_10a0_0be7();
            iVar8 = DAT_10a8_22b0;
            uVar9 = FUN_10a0_0be7(uVar4,iVar6);
            iVar5 = DAT_10a8_38b6;
            uVar9 = FUN_10a0_0baa(uVar9 + uVar4,iVar8 + iVar6 + (uint)CARRY2(uVar9,uVar4));
            iVar5 = -(uint)(*(uint *)((int)param_2 + 8) < uVar9) - iVar5;
            FUN_10a0_0be7();
            uVar9 = FUN_10a0_0baa();
            FUN_10a0_1458();
            FUN_10a0_1444();
            FUN_10a0_145c();
            iVar8 = 0;
            pcVar13 = (char *)s_RandomShrs_10a8_109f + 1;
            uVar4 = FUN_10a0_0be7();
            local_10 = uVar4 + uVar9 + 1;
            local_e = iVar8 + iVar5 + (uint)CARRY2(uVar4,uVar9) + (uint)(0xfffe < uVar4 + uVar9);
            if ((local_e <= DAT_10a8_22b0) &&
               ((local_e < DAT_10a8_22b0 || (local_10 <= DAT_10a8_22ae)))) {
              piVar10 = (int *)param_1;
              uVar7 = (undefined2)((ulong)param_1 >> 0x10);
              if ((DAT_10a8_39a8 == '\0') ||
                 (((int)local_a < DAT_10a8_22b8 ||
                  (((int)local_a <= DAT_10a8_22b8 && (local_c < DAT_10a8_22b6)))))) {
                if (*(char *)(DAT_10a8_319a * 0xfc + local_10 * 0xd + 0x30c8) == '\0') {
                  MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0);
                }
                else {
                  iVar5 = DAT_10a8_319a * 0xfc + 0x30aa;
                  iVar8 = local_10 * 0xd;
                  if (*(int *)(iVar5 + iVar8 + 0x1a) < 0) {
                    FUN_10a0_0b92(0x54c,local_55d,unaff_SS,(undefined2 *)&DAT_10a8_1d32,
                                  (char *)s_RandomShrs_10a8_109f + 9);
                    local_564 = '\x01';
                    local_562 = (char)DAT_10a8_22b2 + '@';
                    local_55f = DAT_10a8_22b6;
                    pcVar13 = (char *)s_PayDividends_10a8_1079 + 7;
                    FUN_1080_0002(local_58a,unaff_SS,0xa0a,0x4e2,(char *)s_RandomShrs_10a8_109f + 9,
                                  piVar10,uVar7);
                    (*(code *)*(undefined2 *)(local_58a[0] + 0x4c))
                              ((char *)s_PayDividends_10a8_1079 + 7,local_58a);
                    (*(code *)*(undefined2 *)(local_58a[0] + 8))
                              ((char *)s_PayDividends_10a8_1079 + 7,local_58a);
                    if (local_564 != '\0') {
                      return;
                    }
                    *(undefined2 *)(iVar5 + local_10 * 0xd + 0x1a) = local_561;
                    iVar8 = local_10 * 0xd;
                    *(undefined1 *)(iVar5 + iVar8 + 0x19) = local_563;
                  }
                  DAT_10a8_39a8 = '\x01';
                  (*(code *)*(undefined2 *)(*param_1 + 0x54))
                            (pcVar13,piVar10,uVar7,CONCAT11((char)((uint)iVar8 >> 8),1));
                  iVar6 = *(char *)(iVar5 + local_10 * 0xd + 0x19) + -0x40;
                  iVar8 = *(int *)(iVar5 + local_10 * 0xd + 0x1a);
                  uVar3 = FUN_1008_1abd(1,iVar8,iVar8 >> 0xf,iVar6,iVar6 >> 0xf);
                  local_11 = (char)uVar3;
                  piVar11 = (int *)((*(char *)(iVar5 + local_10 * 0xd + 0x19) + -0x41) * 0x8c +
                                    (*(int *)(iVar5 + local_10 * 0xd + 0x1a) + -1) * 5 + 0x22d0);
                  puVar1 = (undefined2 *)(*piVar11 + 0x10);
                  (*(code *)*puVar1)(pcVar13,piVar11,(char *)s_RandomShrs_10a8_109f + 9,uVar3);
                  FUN_1008_229d(DAT_10a8_319a * 0xfc + local_10 * 0xd + 0x30c3,
                                (char *)s_RandomShrs_10a8_109f + 9);
                  FUN_1008_5e10(piVar10,uVar7,local_10,local_e);
                  if (DAT_10a8_22c9 != '\0') {
                    FUN_10a0_1e88((char *)s_FALSE_10a8_1003 + 5,piVar10,uVar7,(int)param_2,uVar12);
                  }
                  iVar8 = *(char *)(iVar5 + local_10 * 0xd + 0x19) + -0x41;
                  iVar6 = *(int *)(iVar5 + local_10 * 0xd + 0x1a) + -1;
                  cVar2 = FUN_1008_2448(piVar10,uVar7,1,iVar6,iVar6 >> 0xf,iVar8,iVar8 >> 0xf);
                  if (cVar2 == '\0') {
                    iVar8 = *(char *)(iVar5 + local_10 * 0xd + 0x19) + -0x41;
                    iVar5 = *(int *)(iVar5 + local_10 * 0xd + 0x1a) + -1;
                    FUN_1008_51fd(piVar10,uVar7,1,iVar5,iVar5 >> 0xf,iVar8,iVar8 >> 0xf);
                  }
                }
              }
              else {
                MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0x30);
                MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x4d5,
                           (char *)s_RandomShrs_10a8_109f + 9,0x4b4,
                           (char *)s_RandomShrs_10a8_109f + 9);
              }
            }
          }
        }
      }
      else if ((((iVar6 < DAT_10a8_22b8) || ((iVar6 <= DAT_10a8_22b8 && (local_c < DAT_10a8_22b6))))
               && (local_6 == DAT_10a8_22b4)) && (local_8 == DAT_10a8_22b2)) {
        iVar5 = DAT_10a8_38b6;
        FUN_10a0_1458();
        uVar12 = FUN_10a0_1444();
        iVar6 = 0;
        FUN_10a0_1458(uVar12,iVar8,iVar5);
        FUN_10a0_1438();
        FUN_10a0_1458();
        FUN_10a0_144a();
        FUN_10a0_144a();
        local_10 = FUN_10a0_145c();
        if (((-1 < iVar6) && (iVar6 < 1)) && ((iVar6 < 0 || (local_10 < 8)))) {
          local_11 = (char)local_10 + '\x01';
          bVar15 = false;
          bVar17 = true;
          local_e = iVar6;
          FUN_10a0_1454();
          if (!bVar15 && !bVar17) {
            iVar8 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
            if (-1 < iVar8) {
              local_8 = 0;
              local_6 = 0;
              while( true ) {
                uVar9 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
                if (uVar9 < 0x8000) {
                  local_c = 0;
                  local_a = 0;
                  while( true ) {
                    if (*(char *)(local_8 * 0x8c + local_c * 5 + 0x22d2) == local_11) {
                      FUN_1008_422f(local_c + 1,local_a + (0xfffe < local_c),local_8 + 0x41);
                      return;
                    }
                    if ((local_a == uVar9) && (local_c == DAT_10a8_22b6 - 1)) break;
                    bVar15 = 0xfffe < local_c;
                    local_c = local_c + 1;
                    local_a = local_a + bVar15;
                  }
                }
                if ((local_6 == iVar8) && (local_8 == DAT_10a8_22b2 - 1)) break;
                bVar15 = 0xfffe < local_8;
                local_8 = local_8 + 1;
                local_6 = local_6 + (uint)bVar15;
              }
            }
          }
        }
      }
      else if ((((iVar6 < DAT_10a8_22b8) || ((iVar6 <= DAT_10a8_22b8 && (local_c < DAT_10a8_22b6))))
               && ((local_6 < DAT_10a8_22b4 ||
                   ((local_6 <= DAT_10a8_22b4 && (local_8 < DAT_10a8_22b2)))))) &&
              ((*(char *)(local_8 * 0x8c + local_c * 5 + 0x22d2) != '\0' &&
               (*(char *)(local_8 * 0x8c + local_c * 5 + 0x22d2) != '\t')))) {
        FUN_1008_422f(local_c + 1,iVar6 + (uint)(0xfffe < local_c),local_8 + 0x41);
      }
    }
    else {
      FUN_1048_08f5();
    }
  }
  else {
    FUN_1048_13ad();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0002 @ 10a0:0002 */
/* ======================================================== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_10a0_0002(void)

{
  code *pcVar1;
  int in_AX;
  int iVar2;
  uint uVar3;
  undefined2 in_DX;
  undefined2 in_BX;
  int unaff_BP;
  int iVar4;
  undefined2 unaff_SI;
  undefined2 unaff_DI;
  int unaff_ES;
  char *unaff_CS;
  undefined2 uVar5;
  char *pcVar6;
  
  iVar4 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  if (in_AX != 0) {
    uVar5 = 0;
    DAT_10a8_1c9c = unaff_SI;
    DAT_10a8_1c9e = unaff_DI;
    DAT_10a8_1ca0 = in_DX;
    DAT_10a8_1ca2 = in_BX;
    DAT_10a8_1ca4 = unaff_ES;
    DAT_10a8_1cbc = unaff_ES;
    WAITEVENT();
    unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
    iVar2 = INITAPP((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_1c9e,uVar5);
    if (iVar2 != 0) {
      uVar3 = GETWINFLAGS((char *)s__Show_Graph_10a8_1226 + 10);
      DAT_10a8_1cc6 = 0;
      if (((uVar3 & 0xc0) == 0) && (DAT_10a8_1cc6 = 1, (uVar3 & 2) == 0)) {
        DAT_10a8_1cc6 = 2;
      }
      DAT_10a8_1cc4 = 0x38;
      return;
    }
  }
  pcVar1 = (code *)swi(0x21);
  _DAT_10a8_1cb6 = (*pcVar1)();
  DAT_10a8_1cb8 = pcVar6;
  if ((DAT_10a8_1cb8 != (char *)0x0 || iVar4 != 0) && (iVar4 != -1)) {
    iVar4 = *(int *)0x0;
  }
  DAT_10a8_1cba = iVar4;
  if (DAT_10a8_1cbc != 0) {
    FUN_10a0_00d2();
  }
  if (DAT_10a8_1cb8 != (char *)0x0 || DAT_10a8_1cba != 0) {
    FUN_10a0_00f0();
    FUN_10a0_00f0();
    FUN_10a0_00f0();
    MESSAGEBOX(unaff_CS,(char *)s_Oil_Baron_10a8_100a + 6,0,0,
               (char *)s_Runtime_error_000_at_0000_0000__10a8_1cc8,
               (char *)s_RandomShrs_10a8_109f + 9);
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((int)((ulong)_DAT_10a8_1cb2 >> 0x10) != 0 || (int)_DAT_10a8_1cb2 != 0) {
    _DAT_10a8_1cb2 = 0;
    DAT_10a8_1cbe = 0;
    return;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_005d @ 10a0:005d */
/* ======================================================== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_10a0_005d(int param_1)

{
  code *pcVar1;
  undefined2 in_AX;
  undefined2 unaff_CS;
  int in_stack_00000000;
  
  if ((in_stack_00000000 != 0 || param_1 != 0) && (param_1 != -1)) {
    param_1 = *(int *)0x0;
  }
  _DAT_10a8_1cb6 = in_AX;
  DAT_10a8_1cb8 = in_stack_00000000;
  DAT_10a8_1cba = param_1;
  if (DAT_10a8_1cbc != 0) {
    FUN_10a0_00d2();
  }
  if (DAT_10a8_1cb8 != 0 || DAT_10a8_1cba != 0) {
    FUN_10a0_00f0();
    FUN_10a0_00f0();
    FUN_10a0_00f0();
    MESSAGEBOX(unaff_CS,(char *)s_Oil_Baron_10a8_100a + 6,0,0,
               (char *)s_Runtime_error_000_at_0000_0000__10a8_1cc8);
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((int)((ulong)_DAT_10a8_1cb2 >> 0x10) != 0 || (int)_DAT_10a8_1cb2 != 0) {
    _DAT_10a8_1cb2 = 0;
    DAT_10a8_1cbe = 0;
    return;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0061 @ 10a0:0061 */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10a0006e) */
/* WARNING: Removing unreachable block (ram,0x10a00073) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_10a0_0061(void)

{
  code *pcVar1;
  undefined2 in_AX;
  undefined2 unaff_CS;
  
  DAT_10a8_1cb8 = 0;
  DAT_10a8_1cba = 0;
  _DAT_10a8_1cb6 = in_AX;
  if (DAT_10a8_1cbc != 0) {
    FUN_10a0_00d2();
  }
  if (DAT_10a8_1cb8 != 0 || DAT_10a8_1cba != 0) {
    FUN_10a0_00f0();
    FUN_10a0_00f0();
    FUN_10a0_00f0();
    MESSAGEBOX(unaff_CS,(char *)s_Oil_Baron_10a8_100a + 6,0,0,
               (char *)s_Runtime_error_000_at_0000_0000__10a8_1cc8,
               (char *)s_RandomShrs_10a8_109f + 9);
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((int)((ulong)_DAT_10a8_1cb2 >> 0x10) == 0 && (int)_DAT_10a8_1cb2 == 0) {
    return;
  }
  _DAT_10a8_1cb2 = 0;
  DAT_10a8_1cbe = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_00d2 @ 10a0:00d2 */
/* ======================================================== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16near FUN_10a0_00d2(void)

{
  if ((int)((ulong)_DAT_10a8_1cb2 >> 0x10) != 0 || (int)_DAT_10a8_1cb2 != 0) {
    _DAT_10a8_1cb2 = 0;
    DAT_10a8_1cbe = 0;
    return;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_00f0 @ 10a0:00f0 */
/* ======================================================== */

void __cdecl16near FUN_10a0_00f0(void)

{
  uint in_AX;
  uint uVar1;
  uint in_CX;
  byte bVar2;
  byte *in_BX;
  char cVar3;
  
  do {
    uVar1 = in_AX / in_CX;
    cVar3 = (char)(in_AX % in_CX);
    bVar2 = cVar3 + 0x30;
    if (0x39 < bVar2) {
      bVar2 = cVar3 + 0x37;
    }
    in_BX = in_BX + -1;
    *in_BX = bVar2;
    in_AX = uVar1;
  } while (uVar1 != 0);
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_012d @ 10a0:012d */
/* ======================================================== */

void __stdcall16far FUN_10a0_012d(void)

{
  int unaff_BP;
  undefined1 in_CF;
  
  FUN_10a0_01ca((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (!(bool)in_CF) {
    return;
  }
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0147 @ 10a0:0147 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0147(void)

{
  int unaff_BP;
  undefined1 in_CF;
  
  FUN_10a0_02d7((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (!(bool)in_CF) {
    return;
  }
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_01ca @ 10a0:01ca */
/* ======================================================== */

void __cdecl16near FUN_10a0_01ca(void)

{
  uint uVar1;
  uint in_AX;
  uint uVar2;
  undefined1 uVar3;
  
  uVar1 = in_AX;
  if (in_AX != 0) {
    do {
      DAT_10a8_3dd6 = uVar1;
      uVar3 = DAT_10a8_3dd6 < DAT_10a8_1ca8;
      if ((bool)uVar3) {
        FUN_10a0_023c();
        if (!(bool)uVar3) {
          return;
        }
        FUN_10a0_0222();
LAB_10a0_0201:
        if (!(bool)uVar3) {
          return;
        }
      }
      else {
        FUN_10a0_0222();
        if (!(bool)uVar3) {
          return;
        }
        if ((DAT_10a8_1ca8 != 0) &&
           (uVar3 = DAT_10a8_3dd6 < DAT_10a8_1caa - 0xcU, DAT_10a8_3dd6 <= DAT_10a8_1caa - 0xcU)) {
          FUN_10a0_023c();
          goto LAB_10a0_0201;
        }
      }
      uVar2 = (uint)DAT_10a8_1cae | DAT_10a8_1cb0;
      if (uVar2 != 0) {
        uVar2 = (*DAT_10a8_1cae)();
      }
      uVar1 = DAT_10a8_3dd6;
    } while (1 < uVar2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0222 @ 10a0:0222 */
/* ======================================================== */

void __cdecl16near FUN_10a0_0222(void)

{
  undefined2 in_AX;
  int iVar1;
  undefined2 unaff_CS;
  undefined2 uVar2;
  
  uVar2 = DAT_10a8_1cac;
  iVar1 = GLOBALALLOC(unaff_CS,in_AX,0);
  if (iVar1 != 0) {
    GLOBALLOCK((char *)s__Show_Graph_10a8_1226 + 10,iVar1,uVar2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_023c @ 10a0:023c */
/* ======================================================== */

undefined2 __cdecl16near FUN_10a0_023c(void)

{
  uint uVar1;
  undefined2 uVar2;
  undefined2 in_BX;
  uint unaff_ES;
  bool bVar3;
  
  bVar3 = false;
  uVar1 = DAT_10a8_1ca6;
  if (DAT_10a8_1ca6 != 0) {
    do {
      unaff_ES = uVar1;
      FUN_10a0_02a6();
      if (!bVar3) {
        DAT_10a8_1ca6 = unaff_ES;
        return in_BX;
      }
      uVar1 = *(uint *)0xa;
      bVar3 = uVar1 < DAT_10a8_1ca6;
    } while (uVar1 != DAT_10a8_1ca6);
  }
  uVar2 = FUN_10a0_026a();
  if (!bVar3) {
    uVar2 = in_BX;
    FUN_10a0_02a6();
    DAT_10a8_1ca6 = unaff_ES;
  }
  return uVar2;
}

/* ======================================================== */
/* Function: FUN_10a0_026a @ 10a0:026a */
/* ======================================================== */

undefined2 __cdecl16near FUN_10a0_026a(void)

{
  undefined2 in_AX;
  int iVar1;
  undefined2 uVar2;
  undefined2 extraout_DX;
  undefined1 in_CF;
  
  FUN_10a0_0222();
  if (!(bool)in_CF) {
    *(undefined2 *)0x0 = 0x5054;
    *(undefined2 *)0x2 = 0;
    *(undefined2 *)0x4 = 0xc;
    *(undefined2 *)0x6 = 0;
    iVar1 = DAT_10a8_1caa + -0xc;
    *(int *)0x8 = iVar1;
    uVar2 = extraout_DX;
    if (DAT_10a8_1ca6 != 0) {
      LOCK();
      uVar2 = *(undefined2 *)0xa;
      *(undefined2 *)0xa = extraout_DX;
      UNLOCK();
    }
    *(undefined2 *)0xa = uVar2;
    *(undefined2 *)0xc = 0;
    *(int *)0xe = iVar1;
  }
  return in_AX;
}

/* ======================================================== */
/* Function: FUN_10a0_02a6 @ 10a0:02a6 */
/* ======================================================== */

void __cdecl16near FUN_10a0_02a6(void)

{
  int *piVar1;
  uint in_AX;
  int iVar2;
  int *piVar3;
  int *piVar4;
  undefined2 unaff_ES;
  
  piVar1 = (int *)0x4;
  do {
    piVar3 = piVar1;
    piVar1 = (int *)*piVar3;
    if (piVar1 == (int *)0x0) {
      return;
    }
    iVar2 = piVar1[1] - in_AX;
  } while ((uint)piVar1[1] < in_AX);
  piVar4 = (int *)*piVar1;
  if (iVar2 != 0) {
    piVar4 = (int *)((int)piVar1 + in_AX);
    *piVar4 = *piVar1;
    piVar4[1] = iVar2;
  }
  *piVar3 = (int)piVar4;
  *(int *)0x8 = *(int *)0x8 - in_AX;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_02d7 @ 10a0:02d7 */
/* ======================================================== */

void __cdecl16near FUN_10a0_02d7(void)

{
  char *pcVar1;
  int in_AX;
  uint uVar2;
  int iVar3;
  uint *in_CX;
  char *in_BX;
  uint *puVar4;
  uint *puVar5;
  
  if (in_AX == 0) {
    return;
  }
  if (in_CX != (uint *)0x0) {
    uVar2 = in_AX + 3U & 0xfffc;
    if (*(int *)0x0 != 0x5054) {
      return;
    }
    if (((uint)in_CX & 3) != 0) {
      return;
    }
    puVar5 = (uint *)0x4;
    do {
      puVar4 = puVar5;
      puVar5 = (uint *)*puVar4;
      if (puVar5 == (uint *)0x0) goto LAB_10a0_0306;
    } while (puVar5 <= in_CX && in_CX != puVar5);
    if (in_CX == puVar5) {
      return;
    }
LAB_10a0_0306:
    *in_CX = (uint)puVar5;
    in_CX[1] = uVar2;
    iVar3 = uVar2 + *(int *)0x8;
    *(int *)0x8 = iVar3;
    if (iVar3 + 0xc != DAT_10a8_1caa) {
      FUN_10a0_0327();
      *puVar4 = (uint)in_CX;
      puVar5 = (uint *)((int)puVar4 + puVar4[1]);
      if (puVar5 != (uint *)*puVar4) {
        return;
      }
      *puVar4 = *puVar5;
      puVar4[1] = puVar4[1] + puVar5[1];
      return;
    }
    pcVar1 = DAT_10a8_1ca6;
    DAT_10a8_1ca6 = (char *)0x0;
    if (in_BX != (char *)*(int *)0xa) {
      do {
        DAT_10a8_1ca6 = pcVar1;
        pcVar1 = (char *)*(int *)0xa;
      } while ((char *)*(int *)0xa != in_BX);
      *(int *)0xa = *(int *)0xa;
    }
  }
  if ((in_BX != (char *)s_RandomShrs_10a8_109f + 9) && (iVar3 = GLOBALHANDLE(), iVar3 != 0)) {
    GLOBALUNLOCK((char *)s__Show_Graph_10a8_1226 + 10,iVar3,iVar3);
    GLOBALFREE((char *)s__Show_Graph_10a8_1226 + 10);
    return;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0327 @ 10a0:0327 */
/* ======================================================== */

void __cdecl16near FUN_10a0_0327(void)

{
  int *in_BX;
  int *piVar1;
  undefined2 unaff_ES;
  
  piVar1 = (int *)((int)in_BX + in_BX[1]);
  if (piVar1 == (int *)*in_BX) {
    *in_BX = *piVar1;
    in_BX[1] = in_BX[1] + piVar1[1];
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0388 @ 10a0:0388 */
/* ======================================================== */

undefined2 __cdecl16far FUN_10a0_0388(void)

{
  undefined2 uVar1;
  
  uVar1 = DAT_10a8_1cbe;
  LOCK();
  DAT_10a8_1cbe = 0;
  UNLOCK();
  return uVar1;
}

/* ======================================================== */
/* Function: FUN_10a0_03ef @ 10a0:03ef */
/* ======================================================== */

/* WARNING: Unable to track spacebase fully for stack */

void __cdecl16far FUN_10a0_03ef(void)

{
  int iVar1;
  int unaff_BP;
  int unaff_DI;
  undefined2 unaff_SS;
  bool bVar2;
  long lVar3;
  
  iVar1 = *(int *)(unaff_BP + 10);
  if (iVar1 != 0) {
    lVar3 = *(long *)(unaff_BP + 6);
    bVar2 = false;
    if (lVar3 == 0) {
      lVar3 = FUN_10a0_01ca();
      if (bVar2) {
        FUN_10a0_005d();
        return;
      }
      if (lVar3 == 0) {
        return;
      }
      *(undefined2 *)(unaff_BP + 6) = (int)lVar3;
      *(undefined2 *)(unaff_BP + 8) = (int)((ulong)lVar3 >> 0x10);
    }
    else {
      *(undefined2 *)(unaff_BP + 10) = 0;
    }
    *(int *)((int)lVar3 + unaff_DI) = iVar1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0439 @ 10a0:0439 */
/* ======================================================== */

/* WARNING: Unable to track spacebase fully for stack */

void __cdecl16far FUN_10a0_0439(void)

{
  int unaff_BP;
  undefined2 unaff_SS;
  bool bVar1;
  
  bVar1 = false;
  if (*(int *)(unaff_BP + 10) != 0) {
    FUN_10a0_02d7((char *)s_RandomShrs_10a8_109f + 9);
    if (bVar1) {
      FUN_10a0_005d();
      return;
    }
  }
  *(undefined2 *)(unaff_BP + 6) = 0;
  *(undefined2 *)(unaff_BP + 8) = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_049f @ 10a0:049f */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10a004df) */
/* WARNING: Removing unreachable block (ram,0x10a004e4) */

void __stdcall16far FUN_10a0_049f(char *param_1,undefined2 *param_2)

{
  char *pcVar1;
  char *pcVar2;
  int iVar3;
  char *pcVar4;
  undefined2 *puVar5;
  char *pcVar6;
  undefined2 uVar7;
  
  uVar7 = (undefined2)((ulong)param_2 >> 0x10);
  puVar5 = (undefined2 *)param_2;
  pcVar4 = (char *)param_1;
  *param_2 = 0;
  puVar5[1] = 0xd7b0;
  puVar5[2] = 0x80;
  puVar5[3] = 0;
  puVar5[4] = 0;
  puVar5[5] = 0;
  puVar5[6] = puVar5 + 0x40;
  puVar5[7] = uVar7;
  pcVar6 = (char *)(puVar5 + 9);
  puVar5[8] = (char *)s_Columns_10a8_1036;
  *(char **)pcVar6 = (char *)s_RandomShrs_10a8_109f + 1;
  for (iVar3 = 0xe; pcVar6 = pcVar6 + 2, iVar3 != 0; iVar3 = iVar3 + -1) {
    pcVar6[0] = '\0';
    pcVar6[1] = '\0';
  }
  iVar3 = 0x4f;
  do {
    pcVar1 = pcVar4;
    pcVar4 = pcVar4 + 1;
    if (*pcVar1 == '\0') break;
    pcVar2 = pcVar6;
    pcVar6 = pcVar6 + 1;
    *pcVar2 = *pcVar1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  *pcVar6 = '\0';
  Ordinal_5();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0527 @ 10a0:0527 */
/* ======================================================== */

void FUN_10a0_0527(void)

{
  FUN_10a0_0534();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_052c @ 10a0:052c */
/* ======================================================== */

void FUN_10a0_052c(void)

{
  FUN_10a0_0534();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0534 @ 10a0:0534 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0534(undefined4 param_1)

{
  int iVar1;
  undefined2 in_DX;
  int unaff_BP;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  bool bVar5;
  char *pcVar6;
  
  iVar2 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  iVar1 = *(int *)(iVar3 + 2);
  if ((iVar1 == -0x284f) || (iVar1 == -0x284e)) {
    FUN_10a0_0586(iVar3,uVar4);
  }
  else if (iVar1 != -0x2850) {
    DAT_10a8_1cbe = 0x66;
    return;
  }
  bVar5 = true;
  *(undefined2 *)(iVar3 + 2) = in_DX;
  *(undefined2 *)(iVar3 + 8) = 0;
  *(undefined2 *)(iVar3 + 10) = 0;
  FUN_10a0_05c7(pcVar6,iVar2);
  if (!bVar5) {
    *(undefined2 *)(iVar3 + 2) = 0xd7b0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0586 @ 10a0:0586 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0586(undefined4 param_1)

{
  char cVar1;
  int unaff_BP;
  int iVar2;
  undefined2 uVar3;
  
  cVar1 = '\x01';
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  if (*(int *)(iVar2 + 2) != -0x284f) {
    if (*(int *)(iVar2 + 2) != -0x284e) {
      DAT_10a8_1cbe = 0x67;
      return;
    }
    cVar1 = '\x01';
    FUN_10a0_05c7(1,(char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  }
  if (cVar1 != '\0') {
    FUN_10a0_05c7();
    *(undefined2 *)(iVar2 + 2) = 0xd7b0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_05c7 @ 10a0:05c7 */
/* ======================================================== */

void __cdecl16near FUN_10a0_05c7(void)

{
  int iVar1;
  int in_BX;
  int unaff_DI;
  
  iVar1 = (*(code *)*(undefined2 *)(in_BX + unaff_DI))();
  if (iVar1 != 0) {
    DAT_10a8_1cbe = iVar1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_05d8 @ 10a0:05d8 */
/* ======================================================== */

undefined2 __stdcall16far FUN_10a0_05d8(undefined4 param_1)

{
  code *pcVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  undefined1 in_CF;
  
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  pcVar1 = (code *)swi(0x21);
  uVar2 = (*pcVar1)();
  if ((bool)in_CF) {
    *(undefined2 *)(iVar3 + 10) = 0;
  }
  else {
    *(undefined2 *)(iVar3 + 10) = uVar2;
    uVar2 = 0;
  }
  *(undefined2 *)(iVar3 + 8) = 0;
  return uVar2;
}

/* ======================================================== */
/* Function: FUN_10a0_0608 @ 10a0:0608 */
/* ======================================================== */

int __stdcall16far FUN_10a0_0608(undefined4 param_1)

{
  int *piVar1;
  code *pcVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  
  bVar5 = false;
  LOCK();
  piVar1 = (int *)((int)param_1 + 8);
  iVar4 = *piVar1;
  *piVar1 = 0;
  UNLOCK();
  pcVar2 = (code *)swi(0x21);
  iVar3 = (*pcVar2)();
  if ((!bVar5) && (iVar3 = iVar3 - iVar4, iVar3 != 0)) {
    iVar3 = 0x65;
  }
  return iVar3;
}

/* ======================================================== */
/* Function: FUN_10a0_062d @ 10a0:062d */
/* ======================================================== */

undefined2 __stdcall16far FUN_10a0_062d(undefined4 param_1)

{
  code *pcVar1;
  undefined2 uVar2;
  bool bVar3;
  
  bVar3 = false;
  LOCK();
  *(undefined2 *)((int)param_1 + 8) = 0;
  UNLOCK();
  pcVar1 = (code *)swi(0x21);
  uVar2 = (*pcVar1)();
  if (!bVar3) {
    uVar2 = 0;
  }
  return uVar2;
}

/* ======================================================== */
/* Function: FUN_10a0_064d @ 10a0:064d */
/* ======================================================== */

undefined2 __stdcall16far FUN_10a0_064d(uint *param_1)

{
  code *pcVar1;
  undefined2 uVar2;
  bool bVar3;
  
  bVar3 = *param_1 < 4;
  if (4 < *param_1) {
    pcVar1 = (code *)swi(0x21);
    uVar2 = (*pcVar1)();
    if (bVar3) {
      return uVar2;
    }
  }
  return 0;
}

/* ======================================================== */
/* Function: FUN_10a0_0666 @ 10a0:0666 */
/* ======================================================== */

void __cdecl16near FUN_10a0_0666(void)

{
  undefined4 uVar1;
  code *in_AX;
  undefined2 in_CX;
  undefined2 in_DX;
  int in_BX;
  int iVar2;
  undefined2 unaff_DI;
  undefined2 unaff_ES;
  char *pcVar3;
  code *pcVar4;
  
  pcVar3 = (char *)s_RandomShrs_10a8_109f + 9;
  if (DAT_10a8_1cbe == 0) {
    if (*(int *)(in_BX + 2) != -0x284f) {
      DAT_10a8_1cbe = 0x68;
      return;
    }
    iVar2 = *(int *)(in_BX + 8);
    if (iVar2 == *(int *)(in_BX + 10)) goto LAB_10a0_06aa;
    while( true ) {
      uVar1 = *(undefined4 *)(in_BX + 0xc);
      pcVar4 = (code *)(*in_AX)(in_BX,unaff_ES,pcVar3);
      in_AX = (code *)pcVar4;
      *(int *)(in_BX + 8) = (iVar2 + (int)uVar1) - (int)((ulong)pcVar4 >> 0x10);
      if (in_AX == (code *)0x0) break;
LAB_10a0_06aa:
      FUN_10a0_07f5(in_BX,unaff_ES,unaff_DI,in_DX,in_CX);
      iVar2 = *(int *)(in_BX + 8);
      if (iVar2 == *(int *)(in_BX + 10)) {
        return;
      }
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_06c4 @ 10a0:06c4 */
/* ======================================================== */

void __cdecl16near FUN_10a0_06c4(void)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  uint uVar3;
  uint in_DX;
  int in_BX;
  int iVar4;
  undefined1 *puVar5;
  undefined2 unaff_ES;
  bool bVar6;
  
  if (DAT_10a8_1cbe == 0) {
    if (*(int *)(in_BX + 2) != -0x284e) {
      DAT_10a8_1cbe = 0x69;
      return;
    }
    do {
      uVar3 = *(int *)(in_BX + 4) - *(int *)(in_BX + 8);
      bVar6 = in_DX < uVar3;
      in_DX = in_DX - uVar3;
      if (bVar6) {
        uVar3 = uVar3 + in_DX;
        in_DX = 0;
      }
      uVar2 = *(undefined4 *)(in_BX + 0xc);
      iVar4 = (int)uVar2;
      puVar5 = (undefined1 *)(*(int *)(in_BX + 8) + iVar4);
      for (; uVar3 != 0; uVar3 = uVar3 - 1) {
        puVar1 = puVar5;
        puVar5 = puVar5 + 1;
        *puVar1 = 0x20;
      }
      iVar4 = (int)puVar5 - iVar4;
      *(int *)(in_BX + 8) = iVar4;
      if (iVar4 == *(int *)(in_BX + 4)) {
        FUN_10a0_07f5();
      }
    } while (in_DX != 0);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0713 @ 10a0:0713 */
/* ======================================================== */

void __cdecl16near FUN_10a0_0713(void)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  uint in_AX;
  uint uVar4;
  undefined2 in_DX;
  int in_BX;
  int iVar5;
  undefined1 *unaff_SI;
  undefined1 *puVar6;
  undefined2 unaff_ES;
  bool bVar7;
  
  if (DAT_10a8_1cbe == 0) {
    if (*(int *)(in_BX + 2) != -0x284e) {
      DAT_10a8_1cbe = 0x69;
      return;
    }
    do {
      uVar4 = *(int *)(in_BX + 4) - *(int *)(in_BX + 8);
      bVar7 = in_AX < uVar4;
      in_AX = in_AX - uVar4;
      if (bVar7) {
        uVar4 = uVar4 + in_AX;
        in_AX = 0;
      }
      uVar3 = *(undefined4 *)(in_BX + 0xc);
      iVar5 = (int)uVar3;
      puVar6 = (undefined1 *)(*(int *)(in_BX + 8) + iVar5);
      for (; uVar4 != 0; uVar4 = uVar4 - 1) {
        puVar2 = puVar6;
        puVar6 = puVar6 + 1;
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        *puVar2 = *puVar1;
      }
      iVar5 = (int)puVar6 - iVar5;
      *(int *)(in_BX + 8) = iVar5;
      if (iVar5 == *(int *)(in_BX + 4)) {
        FUN_10a0_07f5();
      }
    } while (in_AX != 0);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_076a @ 10a0:076a */
/* ======================================================== */

void __stdcall16far FUN_10a0_076a(undefined4 param_1)

{
  int iVar1;
  int unaff_BP;
  bool bVar2;
  
  iVar1 = (int)param_1;
  bVar2 = true;
  FUN_10a0_0666((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if ((bVar2) && (*(int *)(iVar1 + 0x1a) != 0)) {
    FUN_10a0_0803();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_07af @ 10a0:07af */
/* ======================================================== */

void __stdcall16far FUN_10a0_07af(undefined4 param_1)

{
  int iVar1;
  int unaff_BP;
  bool bVar2;
  
  bVar2 = unaff_BP + 1 == 0;
  iVar1 = (int)param_1;
  FUN_10a0_0713((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if ((bVar2) && (*(int *)(iVar1 + 0x1a) != 0)) {
    FUN_10a0_0803();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_07f5 @ 10a0:07f5 */
/* ======================================================== */

void __cdecl16near FUN_10a0_07f5(void)

{
  int iVar1;
  int in_BX;
  
  iVar1 = (*(code *)*(undefined2 *)(in_BX + 0x14))();
  if (iVar1 != 0) {
    DAT_10a8_1cbe = iVar1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0803 @ 10a0:0803 */
/* ======================================================== */

void __cdecl16near FUN_10a0_0803(void)

{
  int iVar1;
  int in_BX;
  
  iVar1 = (*(code *)*(undefined2 *)(in_BX + 0x18))();
  if (iVar1 != 0) {
    DAT_10a8_1cbe = iVar1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0811 @ 10a0:0811 */
/* ======================================================== */

undefined1 __cdecl16far FUN_10a0_0811(undefined4 param_1)

{
  int iVar1;
  int unaff_BP;
  int iVar2;
  undefined2 uVar3;
  
  if (DAT_10a8_1cbe == 0) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    iVar1 = (int)param_1;
    if (*(int *)(iVar1 + 2) == -0x284f) {
      iVar2 = *(int *)(iVar1 + 8);
      if (iVar2 == *(int *)(iVar1 + 10)) {
        FUN_10a0_07f5((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
        iVar2 = *(int *)(iVar1 + 8);
        if (iVar2 == *(int *)(iVar1 + 10)) {
          return 0x1a;
        }
      }
      *(int *)(iVar1 + 8) = *(int *)(iVar1 + 8) + 1;
      return *(undefined1 *)((int)*(undefined4 *)(iVar1 + 0xc) + iVar2);
    }
    DAT_10a8_1cbe = 0x68;
  }
  return 0x1a;
}

/* ======================================================== */
/* Function: FUN_10a0_085c @ 10a0:085c */
/* ======================================================== */

void __stdcall16far FUN_10a0_085c(int param_1,undefined1 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int unaff_BP;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)param_3 >> 0x10);
  iVar3 = (int)param_3;
  if (1 < param_1) {
    FUN_10a0_06c4((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  }
  if (DAT_10a8_1cbe == 0) {
    if (*(int *)(iVar3 + 2) == -0x284e) {
      *(int *)(iVar3 + 8) = *(int *)(iVar3 + 8) + 1;
      iVar1 = *(int *)(iVar3 + 8);
      iVar2 = *(int *)(iVar3 + 4);
      *(undefined1 *)((int)*(undefined4 *)(iVar3 + 0xc) + iVar1 + -1) = param_2;
      if (iVar1 == iVar2) {
        FUN_10a0_07f5();
      }
    }
    else {
      DAT_10a8_1cbe = 0x69;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_08ac @ 10a0:08ac */
/* ======================================================== */

void __stdcall16far FUN_10a0_08ac(undefined2 param_1,undefined1 *param_2)

{
  undefined2 extraout_DX;
  int unaff_BP;
  
  FUN_10a0_0666((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  *param_2 = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_08e9 @ 10a0:08e9 */
/* ======================================================== */

void __stdcall16far FUN_10a0_08e9(int param_1,char *param_2)

{
  char *pcVar1;
  uint uVar2;
  int iVar3;
  int unaff_BP;
  
  uVar2 = (uint)(char *)param_2 | param_2._2_2_;
  if (uVar2 != 0) {
    iVar3 = -1;
    do {
      if (iVar3 == 0) break;
      iVar3 = iVar3 + -1;
      pcVar1 = (char *)param_2;
      param_2._0_2_ = (char *)param_2 + 1;
    } while (*pcVar1 != '\0');
    uVar2 = -iVar3 - 2;
  }
  if ((-1 < (int)uVar2) && ((int)uVar2 < param_1)) {
    FUN_10a0_06c4(uVar2,(char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  }
  if (uVar2 != 0) {
    FUN_10a0_0713();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_092d @ 10a0:092d */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10a0094f) */
/* WARNING: Removing unreachable block (ram,0x10a00956) */
/* WARNING: Removing unreachable block (ram,0x10a00958) */

undefined2 __cdecl16far FUN_10a0_092d(void)

{
  FUN_10a0_0666();
  return 0;
}

/* ======================================================== */
/* Function: FUN_10a0_098c @ 10a0:098c */
/* ======================================================== */

void __stdcall16far FUN_10a0_098c(int param_1)

{
  int in_CX;
  int unaff_BP;
  char *local_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  local_4 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_10a0_1993();
  if (in_CX < param_1) {
    FUN_10a0_06c4(&local_4,in_CX);
  }
  FUN_10a0_0713();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_09c3 @ 10a0:09c3 */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10a009e5) */
/* WARNING: Removing unreachable block (ram,0x10a009ec) */
/* WARNING: Removing unreachable block (ram,0x10a009ee) */

undefined2 __cdecl16far FUN_10a0_09c3(void)

{
  FUN_10a0_0666();
  return 0;
}

/* ======================================================== */
/* Function: FUN_10a0_0a24 @ 10a0:0a24 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0a24(int param_1,int param_2)

{
  if ((param_1 < 0) && (param_1 = 6 - param_2, -2 < param_1)) {
    param_1 = -2;
  }
  FUN_10a0_1a76();
  if (param_1 < param_2) {
    FUN_10a0_06c4(param_1);
  }
  FUN_10a0_0713();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0a72 @ 10a0:0a72 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0a72(int param_1,undefined1 *param_2,int param_3)

{
  undefined1 *puVar1;
  int in_CX;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  
  FUN_10a0_1993();
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  puVar3 = (undefined1 *)param_2;
  if (param_1 < param_3) {
    param_3 = param_1;
  }
  if (param_1 < in_CX) {
    in_CX = param_1;
  }
  if (param_3 < in_CX) {
    param_3 = in_CX;
  }
  param_3 = param_3 - in_CX;
  puVar2 = &stack0xfffe;
  if (param_3 != 0) {
    for (; puVar2 = &stack0xfffe, param_3 != 0; param_3 = param_3 + -1) {
      puVar1 = puVar3;
      puVar3 = puVar3 + 1;
      *puVar1 = 0x20;
    }
  }
  for (; in_CX != 0; in_CX = in_CX + -1) {
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar1 = *puVar2;
    puVar2 = puVar2 + 1;
  }
  *puVar3 = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0abf @ 10a0:0abf */
/* ======================================================== */

void __stdcall16far FUN_10a0_0abf(undefined2 *param_1,char *param_2)

{
  char *pcVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  char *pcVar5;
  char *pcVar6;
  undefined2 uVar7;
  bool bVar8;
  
  uVar7 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = -1;
  pcVar6 = (char *)param_2;
  do {
    if (iVar2 == 0) break;
    iVar2 = iVar2 + -1;
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (*pcVar1 == ' ');
  pcVar6 = pcVar6 + -1;
  uVar3 = 0xffff;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (*pcVar1 != '\0');
  pcVar4 = (char *)~uVar3;
  bVar8 = pcVar6 < pcVar4;
  pcVar5 = pcVar4 + -1;
  FUN_10a0_19cf();
  if ((bVar8) || (pcVar5 != (char *)0x0)) {
    pcVar5 = pcVar6 + (-(int)(char *)param_2 - (int)pcVar4) + 1;
  }
  *param_1 = pcVar5;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0af5 @ 10a0:0af5 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0af5(int param_1,undefined1 *param_2,int param_3,int param_4)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  undefined1 local_42 [64];
  
  if ((param_3 < 0) && (param_3 = 6 - param_4, -2 < param_3)) {
    param_3 = -2;
  }
  puVar3 = local_42;
  FUN_10a0_1a76();
  uVar5 = (undefined2)((ulong)param_2 >> 0x10);
  puVar4 = (undefined1 *)param_2;
  if (param_1 < param_4) {
    param_4 = param_1;
  }
  if (param_1 < param_3) {
    param_3 = param_1;
  }
  if (param_4 < param_3) {
    param_4 = param_3;
  }
  param_4 = param_4 - param_3;
  if (param_4 != 0) {
    for (; param_4 != 0; param_4 = param_4 + -1) {
      puVar1 = puVar4;
      puVar4 = puVar4 + 1;
      *puVar1 = 0x20;
    }
  }
  for (; param_3 != 0; param_3 = param_3 + -1) {
    puVar2 = puVar4;
    puVar4 = puVar4 + 1;
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar2 = *puVar1;
  }
  *puVar4 = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0b5a @ 10a0:0b5a */
/* ======================================================== */

void __stdcall16far FUN_10a0_0b5a(undefined2 *param_1,char *param_2)

{
  char *pcVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  char *pcVar5;
  char *pcVar6;
  undefined2 uVar7;
  bool bVar8;
  
  uVar7 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = -1;
  pcVar6 = (char *)param_2;
  do {
    if (iVar2 == 0) break;
    iVar2 = iVar2 + -1;
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (*pcVar1 == ' ');
  pcVar6 = pcVar6 + -1;
  uVar3 = 0xffff;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (*pcVar1 != '\0');
  pcVar4 = (char *)~uVar3;
  bVar8 = pcVar6 < pcVar4;
  pcVar5 = pcVar4 + -1;
  FUN_10a0_1bf9();
  if ((bVar8) || (pcVar5 != (char *)0x0)) {
    pcVar5 = pcVar6 + (-(int)(char *)param_2 - (int)pcVar4) + 1;
  }
  *param_1 = pcVar5;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0b92 @ 10a0:0b92 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0b92(int param_1,undefined1 *param_2,undefined1 *param_3)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  
  puVar3 = (undefined1 *)param_3;
  puVar4 = (undefined1 *)param_2;
  for (; param_1 != 0; param_1 = param_1 + -1) {
    puVar2 = puVar4;
    puVar4 = puVar4 + 1;
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar2 = *puVar1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0baa @ 10a0:0baa */
/* ======================================================== */

int __cdecl16far FUN_10a0_0baa(void)

{
  int in_AX;
  int in_CX;
  
  if (1 < DAT_10a8_1cc6) {
    return in_AX * in_CX;
  }
  return in_AX * in_CX;
}

/* ======================================================== */
/* Function: FUN_10a0_0be7 @ 10a0:0be7 */
/* ======================================================== */

int __cdecl16far FUN_10a0_0be7(void)

{
  uint uVar1;
  uint in_AX;
  int iVar2;
  int iVar3;
  uint in_CX;
  uint uVar4;
  uint uVar5;
  uint in_DX;
  uint in_BX;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  bool bVar11;
  bool bVar12;
  ulong uVar6;
  
  if (1 < DAT_10a8_1cc6) {
    if (CONCAT22(in_BX,in_CX) != 0) {
      return (int)(CONCAT22(in_DX,in_AX) / CONCAT22(in_BX,in_CX));
    }
LAB_10a0_0c60:
    iVar3 = FUN_10a0_005d();
    return iVar3;
  }
  uVar6 = (ulong)in_DX;
  bVar12 = (int)in_DX < 0;
  if (bVar12) {
    bVar11 = in_AX != 0;
    in_AX = -in_AX;
    uVar6 = (ulong)-(in_DX + bVar11);
  }
  uVar9 = (uint)bVar12;
  if (in_BX == 0) {
    in_BX = 0;
    if (in_CX == 0) goto LAB_10a0_0c60;
LAB_10a0_0c68:
    iVar3 = (int)((((ulong)in_BX << 0x10 | uVar6) % (ulong)in_CX << 0x10 | (ulong)in_AX) /
                 (ulong)in_CX);
  }
  else {
    if ((int)in_BX < 0) {
      uVar9 = uVar9 + 2;
      bVar11 = in_CX != 0;
      in_CX = -in_CX;
      in_BX = -(in_BX + bVar11);
      if (in_BX == 0) goto LAB_10a0_0c68;
    }
    iVar7 = 0;
    iVar3 = 0;
    iVar10 = 0x10;
    do {
      iVar2 = iVar3 * 2;
      bVar11 = (int)in_AX < 0;
      in_AX = in_AX << 1 | (uint)(iVar3 < 0);
      uVar4 = (int)uVar6 << 1 | (uint)bVar11;
      uVar8 = iVar7 << 1 | (uint)((int)uVar6 < 0);
      uVar5 = uVar4 - in_CX;
      uVar4 = (uint)(uVar4 < in_CX);
      uVar1 = uVar8 - in_BX;
      iVar7 = uVar1 - uVar4;
      iVar3 = iVar2 + 1;
      if (uVar8 < in_BX || uVar1 < uVar4) {
        bVar11 = CARRY2(uVar5,in_CX);
        uVar5 = uVar5 + in_CX;
        iVar7 = iVar7 + in_BX + (uint)bVar11;
        iVar3 = iVar2;
      }
      uVar6 = (ulong)uVar5;
      iVar10 = iVar10 + -1;
    } while (iVar10 != 0);
  }
  uVar9 = uVar9 >> 1;
  if (bVar12) {
    uVar9 = uVar9 + 1;
  }
  if (uVar9 == 1) {
    iVar3 = -iVar3;
  }
  return iVar3;
}

/* ======================================================== */
/* Function: FUN_10a0_0cbf @ 10a0:0cbf */
/* ======================================================== */

void __stdcall16far FUN_10a0_0cbf(byte *param_1,byte *param_2)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  byte *pbVar4;
  
  pbVar4 = (byte *)param_2;
  pbVar3 = (byte *)param_1;
  bVar1 = *param_1;
  *param_2 = bVar1;
  uVar2 = (uint)bVar1;
  while( true ) {
    pbVar4 = pbVar4 + 1;
    pbVar3 = pbVar3 + 1;
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    *pbVar4 = *pbVar3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0cd9 @ 10a0:0cd9 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0cd9(byte param_1,byte *param_2,byte *param_3)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  byte *pbVar4;
  
  pbVar3 = (byte *)param_3;
  pbVar4 = (byte *)param_2;
  bVar1 = *param_3;
  if (param_1 < *param_3) {
    bVar1 = param_1;
  }
  *param_2 = bVar1;
  uVar2 = (uint)bVar1;
  while( true ) {
    pbVar4 = pbVar4 + 1;
    pbVar3 = pbVar3 + 1;
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    *pbVar4 = *pbVar3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0cfd @ 10a0:0cfd */
/* ======================================================== */

void __stdcall16far FUN_10a0_0cfd(uint param_1,uint param_2,byte *param_3,byte *param_4)

{
  byte *pbVar1;
  uint uVar2;
  byte *pbVar3;
  byte *pbVar4;
  
  pbVar4 = (byte *)param_4;
  if ((int)param_2 < 1) {
    param_2 = 1;
  }
  pbVar3 = (byte *)param_3 + param_2;
  if (*param_3 < param_2) {
    uVar2 = 0;
  }
  else {
    uVar2 = (*param_3 - param_2) + 1;
    if ((int)param_1 < 0) {
      param_1 = 0;
    }
    if (param_1 < uVar2) {
      uVar2 = param_1;
    }
  }
  *param_4 = (byte)uVar2;
  for (; pbVar4 = pbVar4 + 1, uVar2 != 0; uVar2 = uVar2 - 1) {
    pbVar1 = pbVar3;
    pbVar3 = pbVar3 + 1;
    *pbVar4 = *pbVar1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0d3e @ 10a0:0d3e */
/* ======================================================== */

void __stdcall16far FUN_10a0_0d3e(byte *param_1,byte *param_2)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  uint uVar4;
  byte *pbVar5;
  byte *pbVar6;
  
  pbVar5 = (byte *)param_1;
  bVar2 = *param_2;
  bVar3 = *param_1;
  bVar1 = *param_2;
  *param_2 = *param_2 + bVar3;
  if (CARRY1(bVar1,bVar3)) {
    *param_2 = 0xff;
    bVar3 = ~bVar2;
  }
  pbVar6 = (byte *)param_2 + bVar2;
  uVar4 = (uint)bVar3;
  while( true ) {
    pbVar6 = pbVar6 + 1;
    pbVar5 = pbVar5 + 1;
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    *pbVar6 = *pbVar5;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0d6a @ 10a0:0d6a */
/* ======================================================== */

byte * __stdcall16far FUN_10a0_0d6a(byte *param_1,byte *param_2)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  byte *pbVar5;
  byte *pbVar6;
  byte *pbVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  bool bVar10;
  
  uVar9 = (undefined2)((ulong)param_2 >> 0x10);
  if (*param_2 != 0) {
    uVar4 = (uint)*param_2;
    uVar8 = (undefined2)((ulong)param_1 >> 0x10);
    if (uVar4 <= *param_1) {
      iVar2 = (*param_1 - uVar4) + 1;
      pbVar7 = (byte *)param_1 + 1;
      pbVar6 = pbVar7;
      do {
        pbVar5 = (byte *)param_2 + 1;
        bVar10 = pbVar6 == (byte *)0x0;
        do {
          if (iVar2 == 0) break;
          iVar2 = iVar2 + -1;
          pbVar1 = pbVar7;
          pbVar7 = pbVar7 + 1;
          bVar10 = *pbVar5 == *pbVar1;
        } while (!bVar10);
        if (!bVar10) {
          return (byte *)0x0;
        }
        iVar3 = uVar4 - 1;
        bVar10 = iVar3 == 0;
        pbVar6 = pbVar7;
        do {
          pbVar5 = pbVar5 + 1;
          if (iVar3 == 0) break;
          iVar3 = iVar3 + -1;
          pbVar1 = pbVar6;
          pbVar6 = pbVar6 + 1;
          bVar10 = *pbVar5 == *pbVar1;
        } while (bVar10);
        if (bVar10) {
          return pbVar7 + (-1 - (int)(byte *)param_1);
        }
        pbVar6 = (byte *)param_2 + 1;
      } while( true );
    }
  }
  return (byte *)0x0;
}

/* ======================================================== */
/* Function: FUN_10a0_0e08 @ 10a0:0e08 */
/* ======================================================== */

void __stdcall16far
FUN_10a0_0e08(int param_1,undefined2 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  undefined1 *puVar3;
  undefined1 *puVar4;
  undefined2 uVar5;
  undefined1 local_202 [256];
  undefined1 local_102 [256];
  
  if (param_1 < 1) {
    param_1 = 1;
  }
  puVar4 = local_102;
  uVar2 = (undefined2)((ulong)param_3 >> 0x10);
  uVar1 = (undefined2)param_3;
  uVar5 = unaff_SS;
  FUN_10a0_0cfd(param_1 + -1,1,uVar1,uVar2);
  FUN_10a0_0d3e((int)param_4,(int)((ulong)param_4 >> 0x10));
  puVar3 = local_202;
  FUN_10a0_0cfd(0xff,param_1,uVar1,uVar2);
  FUN_10a0_0d3e(puVar3,unaff_SS);
  FUN_10a0_0cd9(param_2,uVar1,uVar2,puVar4,uVar5);
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0e67 @ 10a0:0e67 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0e67(int param_1,int param_2,undefined4 param_3)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  undefined1 *puVar3;
  undefined1 *puVar4;
  undefined2 uVar5;
  undefined1 local_202 [256];
  undefined1 local_102 [256];
  
  if (((0 < param_1) && (0 < param_2)) && (param_2 < 0x100)) {
    if (0xff < param_1) {
      param_1 = 0xff;
    }
    puVar4 = local_102;
    uVar2 = (undefined2)((ulong)param_3 >> 0x10);
    uVar1 = (undefined2)param_3;
    uVar5 = unaff_SS;
    FUN_10a0_0cfd(param_2 + -1,1,uVar1,uVar2);
    puVar3 = local_202;
    FUN_10a0_0cfd(0xff,param_2 + param_1,uVar1,uVar2);
    FUN_10a0_0d3e(puVar3,unaff_SS);
    FUN_10a0_0cd9(0xff,uVar1,uVar2,puVar4,uVar5);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0f00 @ 10a0:0f00 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0f00(undefined2 param_1,undefined2 *param_2)

{
  int iVar1;
  undefined2 *puVar2;
  
  puVar2 = (undefined2 *)param_2;
  *param_2 = param_1;
  for (iVar1 = 0xf; puVar2 = puVar2 + 1, iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0f16 @ 10a0:0f16 */
/* ======================================================== */

void __stdcall16far FUN_10a0_0f16(byte param_1,undefined4 param_2)

{
  byte *pbVar1;
  
  pbVar1 = (byte *)((uint)(param_1 >> 3) + (int)param_2);
  *pbVar1 = *pbVar1 | '\x01' << (param_1 & 7);
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_0f8b @ 10a0:0f8b */
/* ======================================================== */

int __cdecl16far FUN_10a0_0f8b(void)

{
  sbyte sVar1;
  uint in_AX;
  byte bVar2;
  undefined2 in_DX;
  byte bVar3;
  
  bVar2 = (byte)in_AX >> 3;
  bVar3 = (byte)((uint)in_DX >> 8);
  if ((bVar3 <= bVar2) && ((byte)(bVar2 - bVar3) < (byte)in_DX)) {
    sVar1 = (sbyte)(in_AX & 0xff07);
    bVar2 = (byte)((in_AX & 0xff07) >> 8);
    return CONCAT11(bVar2,bVar2 << sVar1 | bVar2 >> 8 - sVar1);
  }
  return (int)in_AX >> 0xf;
}

/* ======================================================== */
/* Function: FUN_10a0_1036 @ 10a0:1036 */
/* ======================================================== */

int __stdcall16far FUN_10a0_1036(int *param_1)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  uint extraout_DX;
  char *pcVar4;
  int *piVar5;
  undefined2 uVar6;
  bool bVar7;
  
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  piVar5 = (int *)param_1;
  *param_1 = 0;
  if (piVar5[1] != -0x284f) {
    *param_1 = *param_1 + 1;
  }
  bVar7 = false;
  if ((char)piVar5[0x18] != '\0') {
    pcVar1 = (code *)swi(0x21);
    iVar2 = (*pcVar1)();
    if (bVar7) {
      return iVar2;
    }
    *param_1 = iVar2;
  }
  iVar2 = 0x5d8;
  iVar3 = 0;
  pcVar4 = (char *)0x0;
  if (piVar5[1] != -0x284f) {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    iVar2 = 0x62d;
    iVar3 = 0x62d;
    pcVar4 = (char *)s_RandomShrs_10a8_109f + 1;
    if ((extraout_DX & 0x80) == 0) {
      if (piVar5[1] == -0x284d) {
        FUN_10a0_10c3();
      }
      iVar2 = 0x608;
      iVar3 = 0;
      pcVar4 = (char *)0x0;
    }
    piVar5[1] = -0x284e;
  }
  piVar5[10] = iVar2;
  piVar5[0xb] = (int)((char *)s_RandomShrs_10a8_109f + 1);
  piVar5[0xc] = iVar3;
  piVar5[0xd] = (int)pcVar4;
  piVar5[0xe] = 0x64d;
  piVar5[0xf] = (int)((char *)s_RandomShrs_10a8_109f + 1);
  return 0;
}

/* ======================================================== */
/* Function: FUN_10a0_10c3 @ 10a0:10c3 */
/* ======================================================== */

void __cdecl16near FUN_10a0_10c3(void)

{
  bool bVar1;
  code *pcVar2;
  int iVar3;
  int iVar4;
  int unaff_DI;
  undefined1 uVar5;
  undefined4 uVar6;
  
  pcVar2 = (code *)swi(0x21);
  uVar6 = (*pcVar2)();
  bVar1 = (uint)((ulong)uVar6 >> 0x10) < (uint)((uint)uVar6 < 0x80);
  uVar5 = !bVar1 && bVar1;
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  pcVar2 = (code *)swi(0x21);
  iVar3 = (*pcVar2)();
  if ((bool)uVar5) {
    iVar3 = 0;
  }
  iVar4 = 0;
  while( true ) {
    if (iVar4 == iVar3) {
      return;
    }
    if (*(char *)(iVar4 + unaff_DI + 0x80) == '\x1a') break;
    iVar4 = iVar4 + 1;
  }
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_111d @ 10a0:111d */
/* ======================================================== */

undefined4 FUN_10a0_111d(void)

{
  uint uVar1;
  byte bVar2;
  uint in_AX;
  uint uVar3;
  uint uVar4;
  char cVar5;
  uint in_CX;
  uint uVar6;
  uint in_DX;
  uint uVar7;
  uint in_BX;
  uint uVar8;
  uint uVar9;
  uint unaff_SI;
  uint unaff_DI;
  uint uVar10;
  uint uVar11;
  bool bVar12;
  bool bVar13;
  undefined4 uVar14;
  
  uVar10 = unaff_DI ^ 0x8000;
  if ((byte)in_CX != 0) {
    if ((byte)in_AX == 0) {
LAB_10a0_11a1:
      return CONCAT22(uVar10,in_CX);
    }
    uVar4 = in_AX;
    uVar7 = in_DX;
    uVar8 = in_BX;
    if ((byte)in_CX < (byte)in_AX) {
      uVar4 = in_CX;
      in_CX = in_AX;
      uVar7 = uVar10;
      uVar8 = unaff_SI;
      unaff_SI = in_BX;
      uVar10 = in_DX;
    }
    bVar2 = -((char)uVar4 - (char)in_CX);
    if (0x28 < bVar2) goto LAB_10a0_11a1;
    uVar3 = CONCAT11((char)(uVar7 >> 8),(char)in_CX) & 0x80ff;
    uVar4 = uVar4 & 0xff00;
    uVar7 = uVar7 | 0x8000;
    uVar11 = uVar10 | 0x8000;
    for (; 7 < bVar2; bVar2 = bVar2 - 8) {
      uVar4 = CONCAT11((char)uVar8,(char)(uVar4 >> 8));
      uVar8 = CONCAT11((char)uVar7,(char)(uVar8 >> 8));
      uVar7 = uVar7 >> 8;
    }
    uVar6 = CONCAT11((char)(in_CX >> 8),bVar2);
    while (bVar2 != 0) {
      uVar1 = uVar7 & 1;
      uVar7 = uVar7 >> 1;
      uVar9 = uVar8 & 1;
      uVar8 = uVar8 >> 1 | (uint)(uVar1 != 0) << 0xf;
      uVar4 = uVar4 >> 1 | (uint)(uVar9 != 0) << 0xf;
      bVar2 = (char)uVar6 - 1;
      uVar6 = CONCAT11((char)(uVar6 >> 8),bVar2);
    }
    if ((int)(uVar3 ^ uVar10) < 0) {
      in_AX = uVar4 - uVar6;
      uVar9 = (uVar8 - unaff_SI) - (uint)(uVar4 < uVar6);
      uVar10 = (uint)(uVar8 < unaff_SI || uVar8 - unaff_SI < (uint)(uVar4 < uVar6));
      in_DX = (uVar7 - uVar11) - uVar10;
      if (uVar7 < uVar11 || uVar7 - uVar11 < uVar10) {
        uVar10 = ~uVar9;
        bVar12 = in_AX == 0;
        in_AX = -in_AX;
        uVar9 = uVar10 + bVar12;
        in_DX = ~in_DX + (uint)CARRY2(uVar10,(uint)bVar12);
        uVar3 = uVar3 ^ 0x8000;
      }
      if ((in_DX == 0 && uVar9 == 0) && in_AX == 0) goto LAB_10a0_11a0;
      while (-1 < (int)in_DX) {
        bVar12 = (int)in_AX < 0;
        in_AX = in_AX << 1;
        bVar13 = (int)uVar9 < 0;
        uVar9 = uVar9 << 1 | (uint)bVar12;
        in_DX = in_DX << 1 | (uint)bVar13;
        cVar5 = (char)uVar3 + -1;
        uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
        if (cVar5 == '\0') {
          uVar14 = FUN_10a0_12e0();
          return uVar14;
        }
      }
    }
    else {
      in_AX = uVar4 + uVar6;
      uVar9 = uVar8 + unaff_SI + (uint)CARRY2(uVar4,uVar6);
      uVar10 = (uint)(CARRY2(uVar8,unaff_SI) || CARRY2(uVar8 + unaff_SI,(uint)CARRY2(uVar4,uVar6)));
      uVar4 = uVar7 + uVar11;
      in_DX = uVar4 + uVar10;
      if (CARRY2(uVar7,uVar11) || CARRY2(uVar4,uVar10)) {
        uVar8 = in_DX & 1;
        in_DX = in_DX >> 1 | (uint)(CARRY2(uVar7,uVar11) || CARRY2(uVar4,uVar10)) << 0xf;
        uVar10 = uVar9 & 1;
        uVar9 = uVar9 >> 1 | (uint)(uVar8 != 0) << 0xf;
        in_AX = in_AX >> 1 | (uint)(uVar10 != 0) << 0xf;
        cVar5 = (char)uVar3 + '\x01';
        uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
        if (cVar5 == '\0') goto LAB_10a0_11ae;
      }
    }
    bVar12 = 0xff7f < in_AX;
    in_AX = in_AX + 0x80;
    uVar10 = (uint)CARRY2(uVar9,(uint)bVar12);
    bVar12 = CARRY2(in_DX,uVar10);
    in_DX = in_DX + uVar10;
    if (bVar12) {
      in_DX = in_DX >> 1 | (uint)bVar12 << 0xf;
      cVar5 = (char)uVar3 + '\x01';
      uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
      if (cVar5 == '\0') {
LAB_10a0_11ae:
        return CONCAT22(in_DX,in_AX);
      }
    }
    in_AX = CONCAT11((char)(in_AX >> 8),(char)uVar3);
    in_DX = CONCAT11((byte)((in_DX & 0x7fff) >> 8) | (byte)(uVar3 >> 8),(char)(in_DX & 0x7fff));
  }
LAB_10a0_11a0:
  return CONCAT22(in_DX,in_AX);
}

/* ======================================================== */
/* Function: FUN_10a0_1121 @ 10a0:1121 */
/* ======================================================== */

undefined4 __cdecl16near FUN_10a0_1121(void)

{
  uint uVar1;
  byte bVar2;
  uint in_AX;
  uint uVar3;
  uint uVar4;
  char cVar5;
  uint in_CX;
  uint uVar6;
  uint in_DX;
  uint uVar7;
  uint in_BX;
  uint uVar8;
  uint uVar9;
  uint unaff_SI;
  uint unaff_DI;
  uint uVar10;
  bool bVar11;
  bool bVar12;
  undefined4 uVar13;
  
  if ((byte)in_CX != 0) {
    if ((byte)in_AX == 0) {
LAB_10a0_11a1:
      return CONCAT22(unaff_DI,in_CX);
    }
    uVar4 = in_AX;
    uVar7 = in_DX;
    uVar8 = in_BX;
    if ((byte)in_CX < (byte)in_AX) {
      uVar4 = in_CX;
      in_CX = in_AX;
      uVar7 = unaff_DI;
      uVar8 = unaff_SI;
      unaff_SI = in_BX;
      unaff_DI = in_DX;
    }
    bVar2 = -((char)uVar4 - (char)in_CX);
    if (0x28 < bVar2) goto LAB_10a0_11a1;
    uVar3 = CONCAT11((char)(uVar7 >> 8),(char)in_CX) & 0x80ff;
    uVar4 = uVar4 & 0xff00;
    uVar7 = uVar7 | 0x8000;
    uVar10 = unaff_DI | 0x8000;
    for (; 7 < bVar2; bVar2 = bVar2 - 8) {
      uVar4 = CONCAT11((char)uVar8,(char)(uVar4 >> 8));
      uVar8 = CONCAT11((char)uVar7,(char)(uVar8 >> 8));
      uVar7 = uVar7 >> 8;
    }
    uVar6 = CONCAT11((char)(in_CX >> 8),bVar2);
    while (bVar2 != 0) {
      uVar1 = uVar7 & 1;
      uVar7 = uVar7 >> 1;
      uVar9 = uVar8 & 1;
      uVar8 = uVar8 >> 1 | (uint)(uVar1 != 0) << 0xf;
      uVar4 = uVar4 >> 1 | (uint)(uVar9 != 0) << 0xf;
      bVar2 = (char)uVar6 - 1;
      uVar6 = CONCAT11((char)(uVar6 >> 8),bVar2);
    }
    if ((int)(uVar3 ^ unaff_DI) < 0) {
      in_AX = uVar4 - uVar6;
      uVar9 = (uVar8 - unaff_SI) - (uint)(uVar4 < uVar6);
      uVar4 = (uint)(uVar8 < unaff_SI || uVar8 - unaff_SI < (uint)(uVar4 < uVar6));
      in_DX = (uVar7 - uVar10) - uVar4;
      if (uVar7 < uVar10 || uVar7 - uVar10 < uVar4) {
        uVar7 = ~uVar9;
        bVar11 = in_AX == 0;
        in_AX = -in_AX;
        uVar9 = uVar7 + bVar11;
        in_DX = ~in_DX + (uint)CARRY2(uVar7,(uint)bVar11);
        uVar3 = uVar3 ^ 0x8000;
      }
      if ((in_DX == 0 && uVar9 == 0) && in_AX == 0) goto LAB_10a0_11a0;
      while (-1 < (int)in_DX) {
        bVar11 = (int)in_AX < 0;
        in_AX = in_AX << 1;
        bVar12 = (int)uVar9 < 0;
        uVar9 = uVar9 << 1 | (uint)bVar11;
        in_DX = in_DX << 1 | (uint)bVar12;
        cVar5 = (char)uVar3 + -1;
        uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
        if (cVar5 == '\0') {
          uVar13 = FUN_10a0_12e0();
          return uVar13;
        }
      }
    }
    else {
      in_AX = uVar4 + uVar6;
      uVar9 = uVar8 + unaff_SI + (uint)CARRY2(uVar4,uVar6);
      uVar4 = (uint)(CARRY2(uVar8,unaff_SI) || CARRY2(uVar8 + unaff_SI,(uint)CARRY2(uVar4,uVar6)));
      uVar8 = uVar7 + uVar10;
      in_DX = uVar8 + uVar4;
      if (CARRY2(uVar7,uVar10) || CARRY2(uVar8,uVar4)) {
        uVar6 = in_DX & 1;
        in_DX = in_DX >> 1 | (uint)(CARRY2(uVar7,uVar10) || CARRY2(uVar8,uVar4)) << 0xf;
        uVar7 = uVar9 & 1;
        uVar9 = uVar9 >> 1 | (uint)(uVar6 != 0) << 0xf;
        in_AX = in_AX >> 1 | (uint)(uVar7 != 0) << 0xf;
        cVar5 = (char)uVar3 + '\x01';
        uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
        if (cVar5 == '\0') goto LAB_10a0_11ae;
      }
    }
    bVar11 = 0xff7f < in_AX;
    in_AX = in_AX + 0x80;
    uVar7 = (uint)CARRY2(uVar9,(uint)bVar11);
    bVar11 = CARRY2(in_DX,uVar7);
    in_DX = in_DX + uVar7;
    if (bVar11) {
      in_DX = in_DX >> 1 | (uint)bVar11 << 0xf;
      cVar5 = (char)uVar3 + '\x01';
      uVar3 = CONCAT11((char)(uVar3 >> 8),cVar5);
      if (cVar5 == '\0') {
LAB_10a0_11ae:
        return CONCAT22(in_DX,in_AX);
      }
    }
    in_AX = CONCAT11((char)(in_AX >> 8),(char)uVar3);
    in_DX = CONCAT11((byte)((in_DX & 0x7fff) >> 8) | (byte)(uVar3 >> 8),(char)(in_DX & 0x7fff));
  }
LAB_10a0_11a0:
  return CONCAT22(in_DX,in_AX);
}

/* ======================================================== */
/* Function: FUN_10a0_11e4 @ 10a0:11e4 */
/* ======================================================== */

undefined4 __cdecl16near FUN_10a0_11e4(void)

{
  long lVar1;
  long lVar2;
  ulong uVar3;
  long lVar4;
  byte bVar5;
  undefined2 in_AX;
  byte bVar7;
  uint uVar6;
  byte bVar8;
  uint in_CX;
  int iVar9;
  uint in_DX;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  uint in_BX;
  uint uVar15;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  uint unaff_SI;
  uint unaff_DI;
  uint uVar21;
  uint uVar22;
  uint uVar23;
  bool bVar24;
  undefined4 uVar25;
  
  bVar5 = (byte)in_AX;
  bVar7 = (byte)((uint)in_AX >> 8);
  if ((bVar5 == 0) || (bVar8 = (byte)in_CX, bVar8 == 0)) {
    uVar25 = FUN_10a0_12e0();
    return uVar25;
  }
  iVar9 = CONCAT11((((byte)(in_DX >> 8) ^ (byte)(unaff_DI >> 8)) & 0x80) + CARRY1(bVar5,bVar8),
                   bVar5 + bVar8);
  uVar20 = in_DX | 0x8000;
  uVar21 = unaff_DI | 0x8000;
  if (((bVar7 == 0) &&
      (uVar6 = in_CX & 0xff00, uVar14 = uVar20, uVar10 = unaff_SI, uVar22 = uVar21, in_BX == 0)) ||
     ((bVar5 = (byte)(in_CX >> 8), bVar5 == 0 &&
      (uVar6 = (uint)bVar7 << 8, uVar14 = uVar21, uVar10 = in_BX, uVar22 = uVar20, unaff_SI == 0))))
  {
    uVar3 = (ulong)uVar10 * (ulong)uVar14 + ((ulong)uVar6 * (ulong)uVar14 >> 0x10);
    uVar15 = (uint)uVar3;
    lVar1 = (ulong)uVar22 * (ulong)uVar14 + (uVar3 >> 0x10);
  }
  else {
    lVar1 = (ulong)bVar7 * 0x100 * (ulong)unaff_SI;
    lVar4 = lVar1 + (ulong)((uint)bVar7 * (uint)bVar5);
    uVar22 = (uint)((ulong)lVar4 >> 0x10);
    lVar2 = (ulong)in_BX * (ulong)bVar5 * 0x100;
    uVar10 = (uint)((ulong)lVar2 >> 0x10);
    uVar6 = (uint)CARRY2((uint)lVar4,(uint)lVar2);
    uVar14 = uVar22 + uVar10;
    uVar15 = uVar14 + uVar6;
    uVar16 = (uint)CARRY2((uint)((ulong)lVar1 >> 0x10),
                          (uint)CARRY2((uint)bVar7 * (uint)bVar5,(uint)lVar1)) +
             (uint)(CARRY2(uVar22,uVar10) || CARRY2(uVar14,uVar6));
    lVar1 = (ulong)bVar7 * 0x100 * (ulong)uVar21;
    uVar11 = (uint)((ulong)lVar1 >> 0x10);
    uVar6 = (uint)lVar1;
    uVar23 = uVar15 + uVar6;
    uVar6 = (uint)CARRY2(uVar15,uVar6);
    uVar22 = uVar16 + uVar11;
    uVar17 = uVar22 + uVar6;
    uVar12 = (uint)((ulong)in_BX * (ulong)unaff_SI >> 0x10);
    uVar10 = (uint)((ulong)in_BX * (ulong)unaff_SI);
    uVar14 = (uint)CARRY2(uVar23,uVar10);
    uVar15 = uVar17 + uVar12;
    uVar18 = uVar15 + uVar14;
    lVar1 = (ulong)uVar20 * (ulong)bVar5 * 0x100;
    uVar13 = (uint)((ulong)lVar1 >> 0x10);
    uVar10 = (uint)CARRY2(uVar23 + uVar10,(uint)lVar1);
    uVar23 = uVar18 + uVar13;
    uVar19 = uVar23 + uVar10;
    uVar12 = (uint)(CARRY2(uVar16,uVar11) || CARRY2(uVar22,uVar6)) +
             (uint)(CARRY2(uVar17,uVar12) || CARRY2(uVar15,uVar14)) +
             (uint)(CARRY2(uVar18,uVar13) || CARRY2(uVar23,uVar10));
    uVar23 = (uint)((ulong)in_BX * (ulong)uVar21 >> 0x10);
    uVar6 = (uint)((ulong)in_BX * (ulong)uVar21);
    uVar22 = uVar19 + uVar6;
    uVar6 = (uint)CARRY2(uVar19,uVar6);
    uVar10 = uVar12 + uVar23;
    uVar13 = uVar10 + uVar6;
    uVar11 = (uint)((ulong)uVar20 * (ulong)unaff_SI >> 0x10);
    uVar14 = (uint)((ulong)uVar20 * (ulong)unaff_SI);
    uVar15 = uVar22 + uVar14;
    uVar14 = (uint)CARRY2(uVar22,uVar14);
    uVar22 = uVar13 + uVar11;
    lVar1 = (ulong)uVar20 * (ulong)uVar21 +
            CONCAT22((uint)(CARRY2(uVar12,uVar23) || CARRY2(uVar10,uVar6)) +
                     (uint)(CARRY2(uVar13,uVar11) || CARRY2(uVar22,uVar14)),uVar22 + uVar14);
  }
  if (-1 < lVar1) {
    bVar24 = (int)uVar15 < 0;
    uVar15 = uVar15 << 1;
    lVar1 = CONCAT22((int)((ulong)lVar1 >> 0x10) << 1 | (uint)((int)lVar1 < 0),
                     (int)lVar1 << 1 | (uint)bVar24);
    iVar9 = iVar9 + -1;
  }
  uVar6 = (uint)((ulong)lVar1 >> 0x10);
  uVar21 = iVar9 + 0x7f7f;
  uVar20 = (uint)CARRY2((uint)lVar1,(uint)(0xff7f < uVar15));
  uVar14 = uVar6 + uVar20;
  if (CARRY2(uVar6,uVar20)) {
    uVar14 = uVar14 >> 1 | (uint)CARRY2(uVar6,uVar20) << 0xf;
    uVar21 = iVar9 + 0x7f80;
  }
  if ((uVar21 & 0x4000) != 0) {
    return 0;
  }
  return CONCAT22(CONCAT11((byte)(uVar14 >> 8) ^ (byte)(uVar21 + 1 >> 8),(char)uVar14),
                  CONCAT11((char)(uVar15 + 0x80 >> 8),(char)(uVar21 + 1)));
}

/* ======================================================== */
/* Function: FUN_10a0_12e0 @ 10a0:12e0 */
/* ======================================================== */

void __cdecl16near FUN_10a0_12e0(void)

{
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_12e7 @ 10a0:12e7 */
/* ======================================================== */

undefined4 __cdecl16near FUN_10a0_12e7(void)

{
  byte bVar1;
  undefined1 uVar2;
  char cVar3;
  undefined2 in_AX;
  int iVar4;
  byte bVar5;
  undefined2 in_CX;
  byte bVar7;
  uint uVar6;
  uint in_DX;
  char cVar9;
  uint uVar8;
  uint in_BX;
  uint uVar10;
  uint unaff_BP;
  uint uVar11;
  uint unaff_SI;
  uint unaff_DI;
  uint uVar12;
  bool bVar13;
  bool bVar14;
  int in_stack_00000000;
  
  bVar1 = (byte)in_AX;
  if (bVar1 == 0) {
    return 0;
  }
  uVar12 = unaff_DI | 0x8000;
  uVar11 = in_DX | 0x8000;
  cVar9 = (((byte)(in_DX >> 8) ^ (byte)(unaff_DI >> 8)) & 0x80) - (bVar1 < (byte)in_CX);
  iVar4 = CONCAT11((char)((uint)in_AX >> 8),2);
  uVar8 = 1;
  do {
    bVar13 = uVar11 < uVar12;
    bVar7 = (byte)((uint)in_CX >> 8);
    bVar5 = (byte)((uint)iVar4 >> 8);
    if ((uVar11 == uVar12) && (bVar13 = in_BX < unaff_SI, in_BX == unaff_SI)) {
      bVar13 = bVar5 < bVar7;
    }
    if (!bVar13) {
      iVar4 = CONCAT11(bVar5 - bVar7,(char)iVar4);
      bVar13 = in_BX < unaff_SI;
      uVar10 = in_BX - unaff_SI;
      in_BX = uVar10 - (bVar5 < bVar7);
      uVar10 = (uint)(bVar13 || uVar10 < (bVar5 < bVar7));
      bVar13 = uVar11 < uVar12 || uVar11 - uVar12 < uVar10;
      uVar11 = (uVar11 - uVar12) - uVar10;
    }
    while( true ) {
      bVar14 = (int)uVar8 < 0;
      uVar8 = uVar8 << 1 | (uint)bVar13;
      if (bVar14) {
        cVar3 = (char)iVar4 + -1;
        iVar4 = CONCAT11((char)((uint)iVar4 >> 8),cVar3);
        if (cVar3 < '\0') {
          uVar11 = ~(uVar8 << 6);
          uVar10 = ~CONCAT11(cVar9,bVar1 - (byte)in_CX);
          uVar12 = unaff_BP ^ 0xffff;
          if (-1 < (int)uVar12) {
            uVar11 = uVar11 << 1;
            uVar10 = uVar10 << 1 | (uint)(-1 < (int)(uVar8 << 6));
            uVar12 = uVar12 << 1 | (uint)(-1 < cVar9);
            in_stack_00000000 = in_stack_00000000 + -1;
          }
          uVar6 = in_stack_00000000 + 0x8080;
          uVar8 = (uint)CARRY2(uVar10,(uint)(0xff7f < uVar11));
          uVar10 = uVar12 + uVar8;
          if (CARRY2(uVar12,uVar8)) {
            uVar10 = uVar10 >> 1 | (uint)CARRY2(uVar12,uVar8) << 0xf;
            uVar6 = in_stack_00000000 + 0x8081;
          }
          if ((uVar6 & 0x4000) != 0) {
            return 0;
          }
          return CONCAT22(CONCAT11((byte)(uVar10 >> 8) ^ (byte)(uVar6 + 1 >> 8),(char)uVar10),
                          CONCAT11((char)(uVar11 + 0x80 >> 8),(char)(uVar6 + 1)));
        }
        uVar8 = 1;
        if (cVar3 == '\0') {
          uVar8 = 0x40;
        }
      }
      bVar13 = iVar4 < 0;
      uVar2 = (undefined1)iVar4;
      bVar5 = (char)((uint)iVar4 >> 8) * '\x02';
      iVar4 = CONCAT11(bVar5,uVar2);
      bVar14 = (int)in_BX < 0;
      in_BX = in_BX << 1 | (uint)bVar13;
      bVar13 = -1 < (int)uVar11;
      uVar11 = uVar11 << 1 | (uint)bVar14;
      if (bVar13) break;
      iVar4 = CONCAT11(bVar5 - bVar7,uVar2);
      bVar13 = in_BX < unaff_SI;
      uVar10 = in_BX - unaff_SI;
      in_BX = uVar10 - (bVar5 < bVar7);
      uVar11 = (uVar11 - uVar12) - (uint)(bVar13 || uVar10 < (bVar5 < bVar7));
      bVar13 = false;
    }
  } while( true );
}

/* ======================================================== */
/* Function: FUN_10a0_135e @ 10a0:135e */
/* ======================================================== */

void __cdecl16near FUN_10a0_135e(void)

{
  uint in_DX;
  uint unaff_DI;
  bool bVar1;
  
  if ((int)(in_DX ^ unaff_DI) < 0) {
    return;
  }
  bVar1 = (in_DX & 0x8000) == 0;
  if ((!bVar1) && (FUN_10a0_1375(), !bVar1)) {
    return;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1375 @ 10a0:1375 */
/* ======================================================== */

void __cdecl16near FUN_10a0_1375(void)

{
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1388 @ 10a0:1388 */
/* ======================================================== */

void __cdecl16near FUN_10a0_1388(void)

{
  uint uVar1;
  uint in_AX;
  uint in_DX;
  bool bVar2;
  
  if (in_AX != 0 || in_DX != 0) {
    if ((int)in_DX < 0) {
      bVar2 = in_AX != 0;
      in_AX = -in_AX;
      in_DX = -(uint)bVar2 - in_DX;
    }
    uVar1 = in_AX;
    if ((in_DX == 0) && (uVar1 = 0, in_DX = in_AX, (char)(in_AX >> 8) == '\0')) {
      in_DX = in_AX << 8;
    }
    while (-1 < (int)in_DX) {
      in_DX = in_DX * 2 + (uint)CARRY2(uVar1,uVar1);
      uVar1 = uVar1 * 2;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_13c4 @ 10a0:13c4 */
/* ======================================================== */

void __cdecl16near FUN_10a0_13c4(void)

{
  byte in_AL;
  byte bVar1;
  
  bVar1 = -in_AL + 0xa0;
  if (in_AL < 0xa1) {
    if (bVar1 < 0x20) {
      if (0xf < bVar1) {
        bVar1 = -in_AL + 0x90;
      }
      if (7 < bVar1) {
        bVar1 = bVar1 - 8;
      }
      for (; bVar1 != 0; bVar1 = bVar1 - 1) {
      }
    }
    else if (bVar1 != 0x20) {
      return;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1432 @ 10a0:1432 */
/* ======================================================== */

void __cdecl16far FUN_10a0_1432(void)

{
  undefined1 in_CF;
  
  FUN_10a0_1121();
  if (!(bool)in_CF) {
    return;
  }
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1438 @ 10a0:1438 */
/* ======================================================== */

void __cdecl16far FUN_10a0_1438(void)

{
  undefined1 in_CF;
  
  FUN_10a0_111d();
  if (!(bool)in_CF) {
    return;
  }
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1444 @ 10a0:1444 */
/* ======================================================== */

void __cdecl16far FUN_10a0_1444(void)

{
  undefined1 in_CF;
  
  FUN_10a0_11e4();
  if (!(bool)in_CF) {
    return;
  }
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_144a @ 10a0:144a */
/* ======================================================== */

void __cdecl16far FUN_10a0_144a(void)

{
  char in_CL;
  bool bVar1;
  
  bVar1 = false;
  if (in_CL == '\0') {
    FUN_10a0_005d();
    return;
  }
  FUN_10a0_12e7();
  if (!bVar1) {
    return;
  }
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1454 @ 10a0:1454 */
/* ======================================================== */

void __cdecl16far FUN_10a0_1454(void)

{
  FUN_10a0_135e();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1458 @ 10a0:1458 */
/* ======================================================== */

void __cdecl16far FUN_10a0_1458(void)

{
  FUN_10a0_1388();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_145c @ 10a0:145c */
/* ======================================================== */

void __cdecl16far FUN_10a0_145c(void)

{
  undefined1 in_CF;
  
  FUN_10a0_13c4();
  if (!(bool)in_CF) {
    return;
  }
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1464 @ 10a0:1464 */
/* ======================================================== */

void __cdecl16far FUN_10a0_1464(void)

{
  undefined1 in_CF;
  
  FUN_10a0_13c4();
  if (!(bool)in_CF) {
    return;
  }
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_147e @ 10a0:147e */
/* ======================================================== */

void __cdecl16near FUN_10a0_147e(void)

{
  FUN_10a0_1121();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_149c @ 10a0:149c */
/* ======================================================== */

void __cdecl16near FUN_10a0_149c(void)

{
  FUN_10a0_12e7();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_150b @ 10a0:150b */
/* ======================================================== */

uint __cdecl16far FUN_10a0_150b(void)

{
  char cVar1;
  byte bVar2;
  uint in_AX;
  uint uVar3;
  uint in_DX;
  undefined2 in_BX;
  undefined2 uVar4;
  undefined4 uVar5;
  
  cVar1 = (char)in_AX;
  if (cVar1 != '\0') {
    if ((in_DX & 0x8000) != 0) {
      uVar3 = FUN_10a0_005d();
      return uVar3;
    }
    do {
      uVar4 = in_BX;
      FUN_10a0_149c();
      uVar5 = FUN_10a0_147e();
      in_AX = (uint)(byte)((char)uVar5 - 1);
      bVar2 = FUN_10a0_111d(in_AX,uVar4,(int)((ulong)uVar5 >> 0x10));
    } while ((byte)(((char)(cVar1 + -0x80) >> 1) + 0x6cU) <= bVar2);
  }
  return in_AX;
}

/* ======================================================== */
/* Function: FUN_10a0_16b1 @ 10a0:16b1 */
/* ======================================================== */

char __cdecl16far FUN_10a0_16b1(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  byte bVar4;
  uint in_DX;
  undefined2 in_BX;
  undefined4 uVar5;
  undefined2 uVar6;
  
  uVar5 = FUN_10a0_12e7();
  uVar6 = (undefined2)uVar5;
  if ((byte)uVar5 < 0x88) {
    uVar3 = FUN_10a0_13c4(uVar6,in_BX,(int)((ulong)uVar5 >> 0x10));
    FUN_10a0_1388(uVar6);
    FUN_10a0_111d();
    bVar1 = FUN_10a0_18a2();
    bVar4 = (byte)(uVar3 >> 1);
    if ((uVar3 & 1) != 0) {
      bVar1 = FUN_10a0_11e4();
    }
    cVar2 = bVar1 + bVar4;
    if (!CARRY1(bVar1,bVar4)) {
      if ((in_DX & 0x8000) != 0) {
        cVar2 = FUN_10a0_12e7();
      }
      return cVar2;
    }
  }
  cVar2 = FUN_10a0_005d();
  return cVar2;
}

/* ======================================================== */
/* Function: FUN_10a0_18a2 @ 10a0:18a2 */
/* ======================================================== */

void __cdecl16near FUN_10a0_18a2(void)

{
  int in_CX;
  int unaff_DI;
  undefined2 uStack_a;
  
  uStack_a = in_CX;
  while( true ) {
    FUN_10a0_11e4();
    unaff_DI = unaff_DI + 6;
    uStack_a = uStack_a + -1;
    if (uStack_a == 0) break;
    FUN_10a0_1121(unaff_DI);
  }
  FUN_10a0_1121();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_18f1 @ 10a0:18f1 */
/* ======================================================== */

int __stdcall16far FUN_10a0_18f1(uint param_1)

{
  long lVar1;
  ulong uVar2;
  
  uVar2 = FUN_10a0_194e();
  lVar1 = (uVar2 >> 0x10) * (ulong)param_1;
  return (int)((ulong)lVar1 >> 0x10) +
         (uint)CARRY2((uint)lVar1,(uint)((uVar2 & 0xffff) * (ulong)param_1 >> 0x10));
}

/* ======================================================== */
/* Function: FUN_10a0_194e @ 10a0:194e */
/* ======================================================== */

void __cdecl16near FUN_10a0_194e(void)

{
  uint uVar1;
  char cVar2;
  int iVar3;
  
  uVar1 = (uint)((ulong)DAT_10a8_1cc0 * (ulong)DAT_10a0_1984);
  cVar2 = (char)(DAT_10a8_1cc0 << 3);
  iVar3 = (int)((ulong)DAT_10a8_1cc0 * (ulong)DAT_10a0_1984 >> 0x10) +
          CONCAT11((char)((DAT_10a8_1cc0 << 3) >> 8) + cVar2,cVar2) + DAT_10a8_1cc2 * 5;
  DAT_10a8_1cc0 = uVar1 + 1;
  DAT_10a8_1cc2 =
       CONCAT11((char)((uint)iVar3 >> 8) + (char)DAT_10a8_1cc2 * '\x04' + (char)(DAT_10a8_1cc2 << 7)
                ,(char)iVar3) + (uint)(0xfffe < uVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1986 @ 10a0:1986 */
/* ======================================================== */

void __cdecl16far FUN_10a0_1986(void)

{
  code *pcVar1;
  undefined2 in_CX;
  undefined2 extraout_DX;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  DAT_10a8_1cc0 = in_CX;
  DAT_10a8_1cc2 = extraout_DX;
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1993 @ 10a0:1993 */
/* ======================================================== */

void __cdecl16near FUN_10a0_1993(void)

{
  ulong uVar1;
  uint in_AX;
  byte bVar2;
  uint in_DX;
  byte *unaff_DI;
  undefined2 unaff_ES;
  char cVar3;
  
  if ((int)in_DX < 0) {
    FUN_10a0_19af();
    unaff_DI[-1] = 0x2d;
    return;
  }
  do {
    uVar1 = (ulong)in_DX;
    in_DX = in_DX / 10;
    uVar1 = uVar1 % 10 << 0x10 | (ulong)in_AX;
    in_AX = (uint)(uVar1 / 10);
    cVar3 = (char)(uVar1 % 10);
    bVar2 = cVar3 + 0x30;
    if (0x39 < bVar2) {
      bVar2 = cVar3 + 0x37;
    }
    unaff_DI = unaff_DI + -1;
    *unaff_DI = bVar2;
  } while (in_AX != 0 || in_DX != 0);
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_19af @ 10a0:19af */
/* ======================================================== */

void __cdecl16near FUN_10a0_19af(void)

{
  ulong uVar1;
  uint in_AX;
  byte bVar2;
  uint in_BX;
  uint unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  char cVar3;
  
  do {
    uVar1 = (ulong)in_BX;
    in_BX = in_BX / unaff_SI;
    uVar1 = uVar1 % (ulong)unaff_SI << 0x10 | (ulong)in_AX;
    in_AX = (uint)(uVar1 / unaff_SI);
    cVar3 = (char)(uVar1 % (ulong)unaff_SI);
    bVar2 = cVar3 + 0x30;
    if (0x39 < bVar2) {
      bVar2 = cVar3 + 0x37;
    }
    unaff_DI = unaff_DI + -1;
    *unaff_DI = bVar2;
  } while (in_AX != 0 || in_BX != 0);
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_19cf @ 10a0:19cf */
/* ======================================================== */

undefined4 __cdecl16near FUN_10a0_19cf(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int in_CX;
  uint uVar5;
  uint uVar6;
  byte bVar7;
  byte bVar8;
  uint uVar9;
  char cVar10;
  uint uVar11;
  byte *unaff_DI;
  undefined2 unaff_ES;
  bool bVar12;
  bool bVar13;
  
  iVar1 = 0;
  uVar5 = 0;
  uVar11 = 0;
  if (in_CX == 0) goto LAB_10a0_1a34;
  if (*unaff_DI == 0x2b) {
LAB_10a0_19e4:
    unaff_DI = unaff_DI + 1;
    in_CX = in_CX + -1;
    if (in_CX == 0) goto LAB_10a0_1a34;
  }
  else if (*unaff_DI == 0x2d) {
    uVar11 = 0xffff;
    goto LAB_10a0_19e4;
  }
  if (*unaff_DI != 0x24) {
    do {
      uVar9 = (uint)(byte)(*unaff_DI - 0x30);
      if ((byte)(*unaff_DI - 0x3a) < 0xf6) break;
      if ((uVar5 & 0xf000) != 0) goto LAB_10a0_1a34;
      uVar2 = iVar1 * 2;
      uVar6 = uVar5 << 1 | (uint)(iVar1 < 0);
      iVar3 = iVar1 << 2;
      uVar4 = iVar1 * 8;
      uVar5 = iVar1 * 10;
      iVar1 = uVar5 + uVar9;
      uVar5 = ((uVar6 << 1 | (uint)((int)uVar2 < 0)) << 1 | (uint)(iVar3 < 0)) + uVar6 +
              (uint)CARRY2(uVar4,uVar2) + (uint)CARRY2(uVar5,uVar9);
      unaff_DI = unaff_DI + 1;
      in_CX = in_CX + -1;
    } while (in_CX != 0);
LAB_10a0_1a1e:
    if (iVar1 != 0 || uVar5 != 0) {
      if ((int)uVar11 < 0) {
        bVar12 = iVar1 != 0;
        iVar1 = -iVar1;
        uVar5 = -(uint)bVar12 - uVar5;
      }
      if ((int)(uVar11 ^ uVar5) < 0) goto LAB_10a0_1a34;
    }
    return CONCAT22(uVar5,iVar1);
  }
  iVar3 = in_CX + -1;
  if (iVar3 != 0) {
    do {
      unaff_DI = unaff_DI + 1;
      bVar7 = *unaff_DI;
      if (0x60 < bVar7) {
        bVar7 = bVar7 - 0x20;
      }
      bVar8 = bVar7 - 0x30;
      if ((byte)(bVar7 - 0x3a) < 0xf6) {
        if ((byte)(bVar7 + 0xb9) < 0xfa) goto LAB_10a0_1a1e;
        bVar8 = bVar7 - 0x37;
      }
      cVar10 = '\x04';
      do {
        bVar12 = iVar1 < 0;
        iVar1 = iVar1 << 1;
        bVar13 = (int)uVar5 < 0;
        uVar5 = uVar5 << 1 | (uint)bVar12;
        if (bVar13) goto LAB_10a0_1a34;
        cVar10 = cVar10 + -1;
      } while (cVar10 != '\0');
      iVar1 = CONCAT11((char)((uint)iVar1 >> 8),(byte)iVar1 | bVar8);
      iVar3 = iVar3 + -1;
      if (iVar3 == 0) {
        if ((int)uVar11 < 0) {
          bVar12 = iVar1 != 0;
          iVar1 = -iVar1;
          uVar5 = -(uint)bVar12 - uVar5;
        }
        return CONCAT22(uVar5,iVar1);
      }
    } while( true );
  }
LAB_10a0_1a34:
  return CONCAT22(uVar5,iVar1);
}

/* ======================================================== */
/* Function: FUN_10a0_1a76 @ 10a0:1a76 */
/* ======================================================== */

void __cdecl16near FUN_10a0_1a76(void)

{
  undefined1 *puVar1;
  byte bVar2;
  undefined1 uVar3;
  int in_CX;
  int iVar4;
  undefined2 in_DX;
  int extraout_DX;
  int iVar5;
  int extraout_DX_00;
  int extraout_DX_01;
  int extraout_DX_02;
  uint uVar6;
  undefined1 *unaff_DI;
  undefined1 *puVar7;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  uint local_16 [7];
  int local_8;
  byte local_6;
  int local_4;
  
  if (0xb < in_CX) {
    in_CX = 0xb;
  }
  if (in_CX < -0xb) {
    in_CX = -0xb;
  }
  local_6 = (byte)((uint)in_DX >> 8);
  local_4 = in_CX;
  FUN_10a0_1b6e();
  local_8 = in_CX;
  if (local_4 < 0) {
    uVar6 = -local_4;
  }
  else {
    uVar6 = local_4 + in_CX + 1;
    if ((int)uVar6 < 0) {
      local_16[0] = local_16[0] & 0xff00;
      goto LAB_10a0_1ae3;
    }
  }
  if (0xb < uVar6) {
    uVar6 = 0xb;
  }
  bVar2 = *(byte *)((int)local_16 + uVar6);
  *(undefined1 *)((int)local_16 + uVar6) = 0;
  if (0x34 < bVar2) {
    while (-1 < (int)(uVar6 - 1)) {
      (&stack0xffe9)[uVar6] = (&stack0xffe9)[uVar6] + '\x01';
      if ((byte)(&stack0xffe9)[uVar6] < 0x3a) goto LAB_10a0_1ae3;
      (&stack0xffe9)[uVar6] = 0;
      uVar6 = uVar6 - 1;
    }
    local_16[0] = 0x31;
    local_8 = local_8 + 1;
  }
LAB_10a0_1ae3:
  if (local_4 < 0) {
    uVar3 = 0x20;
    if ((local_6 & 0x80) != 0) {
      uVar3 = 0x2d;
    }
    *unaff_DI = uVar3;
    uVar3 = FUN_10a0_1b62();
    puVar7 = unaff_DI + 2;
    unaff_DI[1] = uVar3;
    if (extraout_DX_01 != -1) {
      puVar1 = puVar7;
      puVar7 = unaff_DI + 3;
      *puVar1 = 0x2e;
      do {
        uVar3 = FUN_10a0_1b62();
        puVar1 = puVar7;
        puVar7 = puVar7 + 1;
        *puVar1 = uVar3;
      } while (extraout_DX_02 != -1);
    }
    *puVar7 = 0x45;
    uVar3 = 0x2b;
    if (local_8 < 0) {
      uVar3 = 0x2d;
      local_8 = -local_8;
    }
    puVar7[1] = uVar3;
    *(int *)(puVar7 + 2) = CONCAT11((char)(local_8 % 10),(char)(local_8 / 10)) + 0x3030;
  }
  else {
    if ((local_6 & 0x80) != 0) {
      puVar1 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      *puVar1 = 0x2d;
    }
    iVar4 = local_8;
    if (local_8 < 0) {
      puVar1 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      *puVar1 = 0x30;
      iVar5 = local_4;
    }
    else {
      do {
        uVar3 = FUN_10a0_1b62();
        puVar1 = unaff_DI;
        unaff_DI = unaff_DI + 1;
        *puVar1 = uVar3;
        iVar4 = iVar4 + -1;
        iVar5 = extraout_DX;
      } while (-1 < iVar4);
    }
    if (iVar5 != 0) {
      puVar7 = unaff_DI + 1;
      *unaff_DI = 0x2e;
      do {
        iVar4 = iVar4 + 1;
        if (iVar4 == 0) break;
        puVar1 = puVar7;
        puVar7 = puVar7 + 1;
        *puVar1 = 0x30;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      while (-1 < iVar5 + -1) {
        uVar3 = FUN_10a0_1b62();
        puVar1 = puVar7;
        puVar7 = puVar7 + 1;
        *puVar1 = uVar3;
        iVar5 = extraout_DX_00;
      }
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1b62 @ 10a0:1b62 */
/* ======================================================== */

char __cdecl16near FUN_10a0_1b62(void)

{
  char cVar1;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  
  cVar1 = *(char *)(unaff_BP + unaff_SI + -0x14);
  if (cVar1 == '\0') {
    cVar1 = '0';
  }
  return cVar1;
}

/* ======================================================== */
/* Function: FUN_10a0_1b6e @ 10a0:1b6e */
/* ======================================================== */

void __cdecl16near FUN_10a0_1b6e(void)

{
  char *pcVar1;
  uint uVar2;
  int iVar3;
  char in_AL;
  uint uVar4;
  char cVar5;
  int iVar6;
  uint uVar7;
  uint in_BX;
  uint uVar8;
  uint uVar9;
  int iVar10;
  char *unaff_DI;
  undefined2 unaff_ES;
  bool bVar11;
  bool bVar12;
  undefined4 uVar13;
  
  if (in_AL != '\0') {
    uVar13 = FUN_10a0_1ce3();
    if ((byte)uVar13 < 0x81) {
      uVar13 = FUN_10a0_1d6f();
    }
    uVar7 = (uint)((ulong)uVar13 >> 0x10) | 0x8000;
    uVar4 = (uint)uVar13 & 0xff00;
    for (cVar5 = -0x7c - (char)uVar13; cVar5 != '\0'; cVar5 = cVar5 + -1) {
      uVar9 = uVar7 & 1;
      uVar7 = uVar7 >> 1;
      uVar2 = in_BX & 1;
      in_BX = in_BX >> 1 | (uint)(uVar9 != 0) << 0xf;
      uVar4 = uVar4 >> 1 | (uint)(uVar2 != 0) << 0xf;
    }
    iVar6 = 0xc;
    do {
      *unaff_DI = (byte)(uVar7 >> 0xc) + 0x30;
      bVar11 = (int)in_BX < 0;
      uVar8 = in_BX << 1 | (uint)((int)uVar4 < 0);
      uVar9 = uVar8 << 1 | (uint)((int)(uVar4 << 1) < 0);
      iVar3 = uVar4 * 5;
      uVar2 = (uint)CARRY2(uVar4 * 4,uVar4);
      bVar12 = CARRY2(uVar9,in_BX);
      uVar9 = uVar9 + in_BX;
      iVar10 = uVar9 + uVar2;
      uVar4 = uVar4 * 10;
      in_BX = iVar10 * 2 | (uint)(iVar3 < 0);
      uVar7 = ((((uVar7 & 0xfff) << 1 | (uint)bVar11) << 1 | (uint)((int)uVar8 < 0)) +
               (uVar7 & 0xfff) + (uint)(bVar12 || CARRY2(uVar9,uVar2))) * 2 | (uint)(iVar10 < 0);
      unaff_DI = unaff_DI + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
    *unaff_DI = '\0';
    return;
  }
  for (iVar6 = 6; iVar6 != 0; iVar6 = iVar6 + -1) {
    pcVar1 = unaff_DI;
    unaff_DI = unaff_DI + 2;
    pcVar1[0] = '0';
    pcVar1[1] = '0';
  }
  *unaff_DI = '\0';
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1bf9 @ 10a0:1bf9 */
/* ======================================================== */

void __cdecl16near FUN_10a0_1bf9(void)

{
  byte bVar1;
  int iVar2;
  int in_CX;
  char cVar3;
  int in_BX;
  byte *unaff_DI;
  undefined2 unaff_ES;
  bool bVar4;
  undefined4 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  
  uVar6 = 0;
  uVar7 = 0;
  uVar8 = 0;
  if (in_CX == 0) {
    return;
  }
  bVar1 = *unaff_DI;
  bVar4 = bVar1 < 0x20;
  if (((bVar1 == 0x20) || (bVar4 = bVar1 < 0x2b, bVar1 == 0x2b)) ||
     (bVar4 = bVar1 < 0x2d, bVar1 == 0x2d)) {
    unaff_DI = unaff_DI + 1;
    in_CX = in_CX + -1;
  }
  FUN_10a0_1ca5(0,0,0);
  if (bVar4) {
    return;
  }
  if (in_BX != 0) {
    in_BX = 0;
    cVar3 = '\0';
    if ((in_CX == 0) || (*unaff_DI != 0x2e)) goto LAB_10a0_1c44;
  }
  cVar3 = (char)in_BX;
  if (in_CX == 0) {
    return;
  }
  bVar4 = *unaff_DI < 0x2e;
  if (*unaff_DI != 0x2e) {
    return;
  }
  unaff_DI = unaff_DI + 1;
  in_CX = in_CX + -1;
  FUN_10a0_1ca5(uVar6,uVar7,uVar8);
  if (bVar4) {
    return;
  }
  cVar3 = -cVar3;
LAB_10a0_1c44:
  if (in_CX != 0) {
    bVar1 = *unaff_DI;
    bVar4 = bVar1 < 0x45;
    if ((bVar1 == 0x45) || (bVar4 = bVar1 < 0x65, bVar1 == 0x65)) {
      unaff_DI = unaff_DI + 1;
      in_CX = in_CX + -1;
      uVar5 = FUN_10a0_19cf();
      iVar2 = (int)uVar5;
      if (bVar4) {
        return;
      }
      cVar3 = cVar3 + (char)uVar5;
      if ((int)((ulong)uVar5 >> 0x10) != iVar2 >> 0xf) {
        return;
      }
      if (0x3f < iVar2) {
        return;
      }
      if (iVar2 < -0x3f) {
        return;
      }
    }
  }
  if (cVar3 < -0x24) {
    FUN_10a0_1ce3(cVar3,unaff_DI,in_CX);
  }
  FUN_10a0_1ce3();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1ca5 @ 10a0:1ca5 */
/* ======================================================== */

void __cdecl16near FUN_10a0_1ca5(void)

{
  int in_CX;
  int iVar1;
  undefined2 uVar2;
  int unaff_BP;
  char *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined1 uVar3;
  undefined4 uVar4;
  
  iVar1 = 0;
  while( true ) {
    if (in_CX == 0) {
      return;
    }
    uVar3 = 0xf5 < (byte)(*unaff_DI - 0x3aU);
    if (!(bool)uVar3) break;
    iVar1 = iVar1 + 1;
    unaff_DI = unaff_DI + 1;
    in_CX = in_CX + -1;
    FUN_10a0_1388(unaff_DI,in_CX,iVar1);
    uVar2 = *(undefined2 *)(unaff_BP + -6);
    FUN_10a0_1d6f();
    if (!(bool)uVar3) {
      uVar4 = FUN_10a0_1121();
      *(undefined2 *)(unaff_BP + -8) = (int)uVar4;
      *(undefined2 *)(unaff_BP + -6) = uVar2;
      *(undefined2 *)(unaff_BP + -4) = (int)((ulong)uVar4 >> 0x10);
    }
    if ((bool)uVar3) {
      return;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1ce3 @ 10a0:1ce3 */
/* ======================================================== */

void __cdecl16near FUN_10a0_1ce3(void)

{
  byte in_CL;
  byte bVar1;
  bool bVar2;
  
  if (((char)in_CL < -0x26) || ('&' < (char)in_CL)) {
    return;
  }
  bVar2 = (char)in_CL < '\0';
  if (bVar2) {
    in_CL = -in_CL;
  }
  for (bVar1 = in_CL & 3; bVar1 != 0; bVar1 = bVar1 - 1) {
    FUN_10a0_1d6f();
  }
  if (bVar2) {
    FUN_10a0_12e7();
    return;
  }
  FUN_10a0_11e4();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1d6f @ 10a0:1d6f */
/* ======================================================== */

uint __cdecl16near FUN_10a0_1d6f(void)

{
  uint uVar1;
  uint in_AX;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  char cVar5;
  uint in_DX;
  uint uVar6;
  uint uVar7;
  uint in_BX;
  uint uVar8;
  uint uVar9;
  bool bVar10;
  
  cVar5 = (char)in_AX;
  if (cVar5 != '\0') {
    uVar6 = in_DX | 0x8000;
    uVar2 = in_AX & 0xff00;
    uVar7 = uVar6 >> 2;
    uVar8 = (in_BX >> 1 | (uint)((in_DX & 1) != 0) << 0xf) >> 1 | (uint)((in_DX & 2) != 0) << 0xf;
    uVar3 = (uVar2 >> 1 | (uint)((in_BX & 1) != 0) << 0xf) >> 1 |
            (uint)((in_BX >> 1 & 1) != 0) << 0xf;
    uVar4 = uVar3 + uVar2;
    uVar2 = (uint)CARRY2(uVar3,uVar2);
    uVar3 = uVar8 + in_BX;
    uVar9 = uVar3 + uVar2;
    uVar2 = (uint)(CARRY2(uVar8,in_BX) || CARRY2(uVar3,uVar2));
    uVar3 = uVar7 + uVar6;
    uVar8 = uVar3 + uVar2;
    if (CARRY2(uVar7,uVar6) || CARRY2(uVar3,uVar2)) {
      uVar1 = uVar8 & 1;
      uVar8 = uVar8 >> 1 | (uint)(CARRY2(uVar7,uVar6) || CARRY2(uVar3,uVar2)) << 0xf;
      uVar2 = uVar9 & 1;
      uVar9 = uVar9 >> 1 | (uint)(uVar1 != 0) << 0xf;
      uVar4 = uVar4 >> 1 | (uint)(uVar2 != 0) << 0xf;
      bVar10 = cVar5 == -1;
      cVar5 = cVar5 + '\x01';
      if (bVar10) {
        return uVar4;
      }
    }
    in_AX = uVar4 + 0x80;
    if ((!CARRY2(uVar8,(uint)CARRY2(uVar9,(uint)(0xff7f < uVar4)))) ||
       (bVar10 = cVar5 != -1, cVar5 = cVar5 + '\x01', bVar10)) {
      in_AX = CONCAT11((char)(in_AX >> 8),cVar5 + '\x03');
    }
  }
  return in_AX;
}

/* ======================================================== */
/* Function: FUN_10a0_1dbd @ 10a0:1dbd */
/* ======================================================== */

void __stdcall16far FUN_10a0_1dbd(int param_1,undefined1 *param_2,int param_3)

{
  undefined1 *puVar1;
  int in_CX;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  
  FUN_10a0_1993();
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  if (param_1 < param_3) {
    param_3 = param_1;
  }
  if (param_1 < in_CX) {
    in_CX = param_1;
  }
  if (param_3 < in_CX) {
    param_3 = in_CX;
  }
  puVar3 = (undefined1 *)param_2 + 1;
  *param_2 = (char)param_3;
  param_3 = param_3 - in_CX;
  puVar2 = &stack0xfffe;
  if (param_3 != 0) {
    for (; puVar2 = &stack0xfffe, param_3 != 0; param_3 = param_3 + -1) {
      puVar1 = puVar3;
      puVar3 = puVar3 + 1;
      *puVar1 = 0x20;
    }
  }
  for (; in_CX != 0; in_CX = in_CX + -1) {
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar1 = *puVar2;
    puVar2 = puVar2 + 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1e08 @ 10a0:1e08 */
/* ======================================================== */

undefined2 __stdcall16far FUN_10a0_1e08(uint *param_1,byte *param_2)

{
  undefined2 uVar1;
  uint uVar2;
  byte *pbVar3;
  bool bVar4;
  
  bVar4 = false;
  uVar2 = (uint)*param_2;
  pbVar3 = (byte *)param_2;
  while ((pbVar3 = pbVar3 + 1, uVar2 != 0 && (bVar4 = *pbVar3 < 0x20, *pbVar3 == 0x20))) {
    uVar2 = uVar2 - 1;
  }
  uVar1 = FUN_10a0_19cf();
  if ((bVar4) || (uVar2 != 0)) {
    uVar2 = (int)pbVar3 - (int)(byte *)param_2;
    uVar1 = 0;
  }
  *param_1 = uVar2;
  return uVar1;
}

/* ======================================================== */
/* Function: FUN_10a0_1e39 @ 10a0:1e39 */
/* ======================================================== */

void __stdcall16far FUN_10a0_1e39(int param_1,undefined1 *param_2,undefined1 *param_3)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  byte bVar5;
  
  puVar3 = (undefined1 *)param_3;
  puVar4 = (undefined1 *)param_2;
  bVar5 = 0;
  if (puVar3 < puVar4) {
    puVar3 = puVar3 + param_1 + -1;
    puVar4 = puVar4 + param_1 + -1;
    bVar5 = 1;
  }
  for (; param_1 != 0; param_1 = param_1 + -1) {
    puVar2 = puVar4;
    puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
    puVar1 = puVar3;
    puVar3 = puVar3 + (uint)bVar5 * -2 + 1;
    *puVar2 = *puVar1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1e71 @ 10a0:1e71 */
/* ======================================================== */

byte __stdcall16far FUN_10a0_1e71(byte param_1)

{
  if ((0x60 < param_1) && (param_1 < 0x7b)) {
    param_1 = param_1 - 0x20;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_10a0_1e88 @ 10a0:1e88 */
/* ======================================================== */

void FUN_10a0_1e88(void)

{
  uint *unaff_DI;
  
  FUN_10a0_1e8d();
                    /* WARNING: Could not recover jumptable at 0x10a01e8b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(ulong)*unaff_DI)();
  return;
}

/* ======================================================== */
/* Function: FUN_10a0_1e8d @ 10a0:1e8d */
/* ======================================================== */

void __cdecl16near FUN_10a0_1e8d(void)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int in_AX;
  int iVar4;
  int *piVar5;
  int unaff_DI;
  int *piVar6;
  bool bVar7;
  
  piVar2 = (int *)*(int *)(unaff_DI + 4);
  bVar7 = in_AX == piVar2[1];
  piVar5 = piVar2;
  if (bVar7) {
    return;
  }
  do {
    iVar3 = piVar5[3];
    piVar6 = piVar5 + 4;
    iVar4 = iVar3;
    do {
      if (iVar4 == 0) break;
      iVar4 = iVar4 + -1;
      piVar1 = piVar6;
      piVar6 = piVar6 + 1;
      bVar7 = in_AX == *piVar1;
    } while (!bVar7);
    if (bVar7) {
      piVar2[1] = in_AX;
      piVar2[2] = (int)(piVar6 + ((iVar3 + -1) * 2 - iVar4));
      return;
    }
    bVar7 = (int *)*piVar5 == (int *)0x0;
    piVar5 = (int *)*piVar5;
    if (bVar7) {
      FUN_10a0_005d();
      return;
    }
  } while( true );
}

/* ======================================================== */
/* Function: GLOBALALLOC @ 1230:0000 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALALLOC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALFREE @ 1230:0004 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALFREE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALLOCK @ 1230:0008 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALLOCK(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALUNLOCK @ 1230:000c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALUNLOCK(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALHANDLE @ 1230:0010 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALHANDLE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALCOMPACT @ 1230:0014 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALCOMPACT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WAITEVENT @ 1230:0018 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WAITEVENT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MAKEPROCINSTANCE @ 1230:001c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MAKEPROCINSTANCE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: FREEPROCINSTANCE @ 1230:0020 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void FREEPROCINSTANCE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPROFILEINT @ 1230:0024 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPROFILEINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPROFILESTRING @ 1230:0028 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPROFILESTRING(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WRITEPROFILESTRING @ 1230:002c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WRITEPROFILESTRING(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: OPENFILE @ 1230:0030 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void OPENFILE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: INITTASK @ 1230:0034 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void INITTASK(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: __AHINCR @ 1230:0038 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void __AHINCR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPRIVATEPROFILEINT @ 1230:003c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPRIVATEPROFILEINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPRIVATEPROFILESTRING @ 1230:0040 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPRIVATEPROFILESTRING(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WRITEPRIVATEPROFILESTRING @ 1230:0044 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WRITEPRIVATEPROFILESTRING(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINFLAGS @ 1230:0048 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINFLAGS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETFREESPACE @ 1230:004c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETFREESPACE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ALLOCCSTODSALIAS @ 1230:0050 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ALLOCCSTODSALIAS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: FREESELECTOR @ 1230:0054 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void FREESELECTOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: PRESTOCHANGOSELECTOR @ 1230:0058 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void PRESTOCHANGOSELECTOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETBKCOLOR @ 1230:005c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETBKCOLOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETBKMODE @ 1230:0060 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETBKMODE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETMAPMODE @ 1230:0064 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETMAPMODE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETSTRETCHBLTMODE @ 1230:0068 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETSTRETCHBLTMODE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETTEXTCOLOR @ 1230:006c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETTEXTCOLOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETWINDOWEXT @ 1230:0070 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETWINDOWEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETVIEWPORTORG @ 1230:0074 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETVIEWPORTORG(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETVIEWPORTEXT @ 1230:0078 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETVIEWPORTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LINETO @ 1230:007c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LINETO(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MOVETO @ 1230:0080 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MOVETO(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: RECTANGLE @ 1230:0084 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void RECTANGLE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: BITBLT @ 1230:0088 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void BITBLT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: STRETCHBLT @ 1230:008c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void STRETCHBLT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SELECTOBJECT @ 1230:0090 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SELECTOBJECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATEBRUSHINDIRECT @ 1230:0094 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATEBRUSHINDIRECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATECOMPATIBLEBITMAP @ 1230:0098 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATECOMPATIBLEBITMAP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATECOMPATIBLEDC @ 1230:009c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATECOMPATIBLEDC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATEFONT @ 1230:00a0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATEFONT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATEPEN @ 1230:00a4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATEPEN(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATESOLIDBRUSH @ 1230:00a8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATESOLIDBRUSH(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DELETEDC @ 1230:00ac */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DELETEDC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DELETEOBJECT @ 1230:00b0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DELETEOBJECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDEVICECAPS @ 1230:00b4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDEVICECAPS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETOBJECT @ 1230:00b8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETOBJECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETSTOCKOBJECT @ 1230:00bc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETSTOCKOBJECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETTEXTMETRICS @ 1230:00c0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETTEXTMETRICS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETNEARESTCOLOR @ 1230:00c4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETNEARESTCOLOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MESSAGEBOX @ 1230:00c8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MESSAGEBOX(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: INITAPP @ 1230:00cc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void INITAPP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: POSTQUITMESSAGE @ 1230:00d0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void POSTQUITMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETCURSORPOS @ 1230:00d4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETCURSORPOS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETCAPTURE @ 1230:00d8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETCAPTURE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: RELEASECAPTURE @ 1230:00dc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void RELEASECAPTURE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETFOCUS @ 1230:00e0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETFOCUS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETFOCUS @ 1230:00e4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETFOCUS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: REMOVEPROP @ 1230:00e8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void REMOVEPROP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPROP @ 1230:00ec */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPROP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETPROP @ 1230:00f0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETPROP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SCREENTOCLIENT @ 1230:00f4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SCREENTOCLIENT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISICONIC @ 1230:00f8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISICONIC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINDOWRECT @ 1230:00fc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINDOWRECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETCLIENTRECT @ 1230:0100 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETCLIENTRECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ENABLEWINDOW @ 1230:0104 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ENABLEWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINDOWTEXT @ 1230:0108 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINDOWTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETWINDOWTEXT @ 1230:010c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETWINDOWTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: BEGINPAINT @ 1230:0110 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void BEGINPAINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ENDPAINT @ 1230:0114 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ENDPAINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SHOWWINDOW @ 1230:0118 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SHOWWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPARENT @ 1230:011c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPARENT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISWINDOW @ 1230:0120 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISCHILD @ 1230:0124 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISCHILD(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DESTROYWINDOW @ 1230:0128 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DESTROYWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MOVEWINDOW @ 1230:012c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MOVEWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: REGISTERCLASS @ 1230:0130 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void REGISTERCLASS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDC @ 1230:0134 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: RELEASEDC @ 1230:0138 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void RELEASEDC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETCURSORPOS @ 1230:013c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETCURSORPOS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DRAWTEXT @ 1230:0140 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DRAWTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ENDDIALOG @ 1230:0144 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ENDDIALOG(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISDIALOGMESSAGE @ 1230:0148 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISDIALOGMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDLGITEM @ 1230:014c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDLGITEM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETDLGITEMTEXT @ 1230:0150 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETDLGITEMTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDLGITEMTEXT @ 1230:0154 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDLGITEMTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETDLGITEMINT @ 1230:0158 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETDLGITEMINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDLGITEMINT @ 1230:015c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDLGITEMINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CHECKRADIOBUTTON @ 1230:0160 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CHECKRADIOBUTTON(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CHECKDLGBUTTON @ 1230:0164 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CHECKDLGBUTTON(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DLGDIRSELECT @ 1230:0168 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DLGDIRSELECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DLGDIRLIST @ 1230:016c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DLGDIRLIST(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SENDDLGITEMMESSAGE @ 1230:0170 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SENDDLGITEMMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MESSAGEBEEP @ 1230:0174 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MESSAGEBEEP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DEFWINDOWPROC @ 1230:0178 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DEFWINDOWPROC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: PEEKMESSAGE @ 1230:017c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void PEEKMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: POSTMESSAGE @ 1230:0180 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void POSTMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SENDMESSAGE @ 1230:0184 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SENDMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WAITMESSAGE @ 1230:0188 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WAITMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: TRANSLATEMESSAGE @ 1230:018c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void TRANSLATEMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DISPATCHMESSAGE @ 1230:0190 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DISPATCHMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CALLWINDOWPROC @ 1230:0194 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CALLWINDOWPROC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: INVALIDATERECT @ 1230:0198 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void INVALIDATERECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINDOWWORD @ 1230:019c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINDOWWORD(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINDOWLONG @ 1230:01a0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINDOWLONG(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETWINDOWLONG @ 1230:01a4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETWINDOWLONG(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADMENU @ 1230:01a8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DESTROYMENU @ 1230:01ac */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DESTROYMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CHECKMENUITEM @ 1230:01b0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CHECKMENUITEM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ENABLEMENUITEM @ 1230:01b4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ENABLEMENUITEM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETMENU @ 1230:01b8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETMENU @ 1230:01bc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETSUBMENU @ 1230:01c0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETSUBMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WINHELP @ 1230:01c4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WINHELP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADCURSOR @ 1230:01c8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADCURSOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADICON @ 1230:01cc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADICON(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADBITMAP @ 1230:01d0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADBITMAP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADACCELERATORS @ 1230:01d4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADACCELERATORS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: TRANSLATEACCELERATOR @ 1230:01d8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void TRANSLATEACCELERATOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETSYSTEMMETRICS @ 1230:01dc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETSYSTEMMETRICS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DIALOGBOXPARAM @ 1230:01e0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DIALOGBOXPARAM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATEDIALOGPARAM @ 1230:01e4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATEDIALOGPARAM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISZOOMED @ 1230:01e8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISZOOMED(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DEFDLGPROC @ 1230:01ec */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DEFDLGPROC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETCLASSINFO @ 1230:01f0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETCLASSINFO(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MODIFYMENU @ 1230:01f4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MODIFYMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: TRACKPOPUPMENU @ 1230:01f8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void TRACKPOPUPMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WVSPRINTF @ 1230:01fc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WVSPRINTF(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ANSIUPPER @ 1230:0200 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ANSIUPPER(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ANSILOWER @ 1230:0204 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ANSILOWER(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DEFMDICHILDPROC @ 1230:0208 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DEFMDICHILDPROC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: TRANSLATEMDISYSACCEL @ 1230:020c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void TRANSLATEMDISYSACCEL(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATEWINDOWEX @ 1230:0210 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATEWINDOWEX(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: Ordinal_5 @ 1230:0214 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void Ordinal_5(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: Ordinal_6 @ 1230:0218 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void Ordinal_6(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALALLOC @ 1230:021c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALALLOC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALFREE @ 1230:0220 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALFREE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALLOCK @ 1230:0224 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALLOCK(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALUNLOCK @ 1230:0228 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALUNLOCK(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALHANDLE @ 1230:022c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALHANDLE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GLOBALCOMPACT @ 1230:0230 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GLOBALCOMPACT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WAITEVENT @ 1230:0234 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WAITEVENT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MAKEPROCINSTANCE @ 1230:0238 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MAKEPROCINSTANCE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: FREEPROCINSTANCE @ 1230:023c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void FREEPROCINSTANCE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPROFILEINT @ 1230:0240 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPROFILEINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPROFILESTRING @ 1230:0244 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPROFILESTRING(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WRITEPROFILESTRING @ 1230:0248 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WRITEPROFILESTRING(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: OPENFILE @ 1230:024c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void OPENFILE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: INITTASK @ 1230:0250 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void INITTASK(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: __AHINCR @ 1230:0254 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void __AHINCR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPRIVATEPROFILEINT @ 1230:0258 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPRIVATEPROFILEINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPRIVATEPROFILESTRING @ 1230:025c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPRIVATEPROFILESTRING(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WRITEPRIVATEPROFILESTRING @ 1230:0260 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WRITEPRIVATEPROFILESTRING(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINFLAGS @ 1230:0264 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINFLAGS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETFREESPACE @ 1230:0268 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETFREESPACE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ALLOCCSTODSALIAS @ 1230:026c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ALLOCCSTODSALIAS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: FREESELECTOR @ 1230:0270 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void FREESELECTOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: PRESTOCHANGOSELECTOR @ 1230:0274 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void PRESTOCHANGOSELECTOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MESSAGEBOX @ 1230:0278 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MESSAGEBOX(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: INITAPP @ 1230:027c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void INITAPP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: POSTQUITMESSAGE @ 1230:0280 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void POSTQUITMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETCURSORPOS @ 1230:0284 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETCURSORPOS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETCAPTURE @ 1230:0288 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETCAPTURE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: RELEASECAPTURE @ 1230:028c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void RELEASECAPTURE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETFOCUS @ 1230:0290 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETFOCUS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETFOCUS @ 1230:0294 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETFOCUS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: REMOVEPROP @ 1230:0298 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void REMOVEPROP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPROP @ 1230:029c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPROP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETPROP @ 1230:02a0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETPROP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SCREENTOCLIENT @ 1230:02a4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SCREENTOCLIENT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISICONIC @ 1230:02a8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISICONIC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINDOWRECT @ 1230:02ac */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINDOWRECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETCLIENTRECT @ 1230:02b0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETCLIENTRECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ENABLEWINDOW @ 1230:02b4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ENABLEWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINDOWTEXT @ 1230:02b8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINDOWTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETWINDOWTEXT @ 1230:02bc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETWINDOWTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: BEGINPAINT @ 1230:02c0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void BEGINPAINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ENDPAINT @ 1230:02c4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ENDPAINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SHOWWINDOW @ 1230:02c8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SHOWWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETPARENT @ 1230:02cc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETPARENT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISWINDOW @ 1230:02d0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISCHILD @ 1230:02d4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISCHILD(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DESTROYWINDOW @ 1230:02d8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DESTROYWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MOVEWINDOW @ 1230:02dc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MOVEWINDOW(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: REGISTERCLASS @ 1230:02e0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void REGISTERCLASS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDC @ 1230:02e4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: RELEASEDC @ 1230:02e8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void RELEASEDC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETCURSORPOS @ 1230:02ec */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETCURSORPOS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DRAWTEXT @ 1230:02f0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DRAWTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ENDDIALOG @ 1230:02f4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ENDDIALOG(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISDIALOGMESSAGE @ 1230:02f8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISDIALOGMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDLGITEM @ 1230:02fc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDLGITEM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETDLGITEMTEXT @ 1230:0300 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETDLGITEMTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDLGITEMTEXT @ 1230:0304 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDLGITEMTEXT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETDLGITEMINT @ 1230:0308 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETDLGITEMINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETDLGITEMINT @ 1230:030c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETDLGITEMINT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CHECKRADIOBUTTON @ 1230:0310 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CHECKRADIOBUTTON(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CHECKDLGBUTTON @ 1230:0314 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CHECKDLGBUTTON(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DLGDIRSELECT @ 1230:0318 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DLGDIRSELECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DLGDIRLIST @ 1230:031c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DLGDIRLIST(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SENDDLGITEMMESSAGE @ 1230:0320 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SENDDLGITEMMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MESSAGEBEEP @ 1230:0324 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MESSAGEBEEP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DEFWINDOWPROC @ 1230:0328 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DEFWINDOWPROC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: PEEKMESSAGE @ 1230:032c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void PEEKMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: POSTMESSAGE @ 1230:0330 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void POSTMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SENDMESSAGE @ 1230:0334 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SENDMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WAITMESSAGE @ 1230:0338 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WAITMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: TRANSLATEMESSAGE @ 1230:033c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void TRANSLATEMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DISPATCHMESSAGE @ 1230:0340 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DISPATCHMESSAGE(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CALLWINDOWPROC @ 1230:0344 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CALLWINDOWPROC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: INVALIDATERECT @ 1230:0348 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void INVALIDATERECT(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINDOWWORD @ 1230:034c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINDOWWORD(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETWINDOWLONG @ 1230:0350 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETWINDOWLONG(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETWINDOWLONG @ 1230:0354 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETWINDOWLONG(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADMENU @ 1230:0358 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DESTROYMENU @ 1230:035c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DESTROYMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CHECKMENUITEM @ 1230:0360 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CHECKMENUITEM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ENABLEMENUITEM @ 1230:0364 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ENABLEMENUITEM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETMENU @ 1230:0368 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: SETMENU @ 1230:036c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void SETMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETSUBMENU @ 1230:0370 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETSUBMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WINHELP @ 1230:0374 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WINHELP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADCURSOR @ 1230:0378 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADCURSOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADICON @ 1230:037c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADICON(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADBITMAP @ 1230:0380 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADBITMAP(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: LOADACCELERATORS @ 1230:0384 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void LOADACCELERATORS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: TRANSLATEACCELERATOR @ 1230:0388 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void TRANSLATEACCELERATOR(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETSYSTEMMETRICS @ 1230:038c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETSYSTEMMETRICS(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DIALOGBOXPARAM @ 1230:0390 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DIALOGBOXPARAM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATEDIALOGPARAM @ 1230:0394 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATEDIALOGPARAM(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ISZOOMED @ 1230:0398 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ISZOOMED(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DEFDLGPROC @ 1230:039c */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DEFDLGPROC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: GETCLASSINFO @ 1230:03a0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void GETCLASSINFO(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: MODIFYMENU @ 1230:03a4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void MODIFYMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: TRACKPOPUPMENU @ 1230:03a8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void TRACKPOPUPMENU(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: WVSPRINTF @ 1230:03ac */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void WVSPRINTF(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ANSIUPPER @ 1230:03b0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ANSIUPPER(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: ANSILOWER @ 1230:03b4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void ANSILOWER(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: DEFMDICHILDPROC @ 1230:03b8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void DEFMDICHILDPROC(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: TRANSLATEMDISYSACCEL @ 1230:03bc */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void TRANSLATEMDISYSACCEL(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: CREATEWINDOWEX @ 1230:03c0 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void CREATEWINDOWEX(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: Ordinal_5 @ 1230:03c4 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void Ordinal_5(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

/* ======================================================== */
/* Function: Ordinal_6 @ 1230:03c8 */
/* ======================================================== */

/* WARNING: Control flow encountered bad instruction data */

void Ordinal_6(void)

{
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

