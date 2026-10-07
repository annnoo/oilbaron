/*
 * main_winproc.c - Application Lifecycle, Main WindowProc & Game State Machine
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - Win16 Entry Point (FUN_1000_0002 / FUN_1000_002f):
 *       Initializes task, registers TurboWindow window class, creates main frame.
 *   - Message Loop & Command Dispatch (Segment 1078, FUN_1078_0c59):
 *       Dispatches WM_COMMAND for menus (MAIN_MENU, COMPUTER, POPUP_MENU).
 *       Handles accelerator shortcuts (Alt+E, Alt+N, Alt+S, Alt+B, Alt+I, Alt+D, Alt+P).
 *   - Turn Loop State Machine (FUN_1008_0084, FUN_1008_0ed2):
 *       Phase 1: Place or Discard tile (Must place before purchasing)
 *       Phase 2: Resolve Company Formation or Merger
 *       Phase 3: Buy / Sell Stocks (up to 3 shares)
 *       Phase 4: Draw replacement tile
 *       Phase 5: Advance to next player ("Press <Space> For next Player")
 *   - End Game Evaluation (FUN_1008_21eb, FUN_1008_229d):
 *       Triggered when all active companies >= 11 safe tiles or any company >= 41 tiles.
 *       Displays Players' Wealth window with final liquidation totals.
 */

/* Function Index:
 *   FUN_1000_0002        @ 1000:0002
 *   FUN_1000_002f        @ 1000:002f
 *   FUN_1008_2168        @ 1008:2168
 *   FUN_1008_21eb        @ 1008:21eb
 *   FUN_1008_23d1        @ 1008:23d1
 *   FUN_1008_6e62        @ 1008:6e62
 *   FUN_1010_0002        @ 1010:0002
 *   FUN_1010_006a        @ 1010:006a
 *   FUN_1010_00c0        @ 1010:00c0
 *   FUN_1010_00fd        @ 1010:00fd
 *   FUN_1010_01a2        @ 1010:01a2
 *   FUN_1010_0312        @ 1010:0312
 *   FUN_1010_03dd        @ 1010:03dd
 *   FUN_1010_0425        @ 1010:0425
 *   FUN_1010_0498        @ 1010:0498
 *   FUN_1010_051c        @ 1010:051c
 *   FUN_1010_0554        @ 1010:0554
 *   FUN_1010_0593        @ 1010:0593
 *   FUN_1018_0002        @ 1018:0002
 *   FUN_1020_0002        @ 1020:0002
 *   FUN_1020_009f        @ 1020:009f
 *   FUN_1020_00c8        @ 1020:00c8
 *   FUN_1020_00ed        @ 1020:00ed
 *   FUN_1020_0137        @ 1020:0137
 *   FUN_1020_0157        @ 1020:0157
 *   FUN_1020_017c        @ 1020:017c
 *   FUN_1020_01d1        @ 1020:01d1
 *   FUN_1020_05ca        @ 1020:05ca
 *   FUN_1028_0002        @ 1028:0002
 *   FUN_1028_0175        @ 1028:0175
 *   FUN_1028_01a9        @ 1028:01a9
 *   FUN_1028_021b        @ 1028:021b
 *   FUN_1028_0244        @ 1028:0244
 *   FUN_1028_0334        @ 1028:0334
 *   FUN_1028_0386        @ 1028:0386
 *   FUN_1028_03b5        @ 1028:03b5
 *   FUN_1028_042c        @ 1028:042c
 *   FUN_1028_04b3        @ 1028:04b3
 *   FUN_1028_0502        @ 1028:0502
 *   FUN_1028_0551        @ 1028:0551
 *   FUN_1028_057e        @ 1028:057e
 *   FUN_1028_059e        @ 1028:059e
 *   FUN_1028_05df        @ 1028:05df
 *   FUN_1028_0610        @ 1028:0610
 *   FUN_1028_0ddf        @ 1028:0ddf
 *   FUN_1030_0002        @ 1030:0002
 *   FUN_1030_00a5        @ 1030:00a5
 *   FUN_1030_016c        @ 1030:016c
 *   FUN_1030_01fa        @ 1030:01fa
 *   FUN_1030_03e2        @ 1030:03e2
 *   FUN_1030_0536        @ 1030:0536
 *   FUN_1030_0557        @ 1030:0557
 *   FUN_1030_0578        @ 1030:0578
 *   FUN_1030_0599        @ 1030:0599
 *   FUN_1030_0790        @ 1030:0790
 *   FUN_1030_081e        @ 1030:081e
 *   FUN_1030_08c0        @ 1030:08c0
 *   FUN_1038_0002        @ 1038:0002
 *   FUN_1040_0002        @ 1040:0002
 *   FUN_1040_0043        @ 1040:0043
 *   FUN_1040_00f0        @ 1040:00f0
 *   FUN_1040_016f        @ 1040:016f
 *   FUN_1040_01bc        @ 1040:01bc
 *   FUN_1040_0256        @ 1040:0256
 *   FUN_1040_02b8        @ 1040:02b8
 *   FUN_1040_06af        @ 1040:06af
 *   FUN_1040_0865        @ 1040:0865
 *   FUN_1040_0a98        @ 1040:0a98
 *   FUN_1040_0c2f        @ 1040:0c2f
 *   FUN_1040_0c60        @ 1040:0c60
 *   FUN_1078_0002        @ 1078:0002
 *   FUN_1078_0045        @ 1078:0045
 *   FUN_1078_0071        @ 1078:0071
 *   FUN_1078_0097        @ 1078:0097
 *   FUN_1078_0103        @ 1078:0103
 *   FUN_1078_0133        @ 1078:0133
 *   FUN_1078_018b        @ 1078:018b
 *   FUN_1078_01dc        @ 1078:01dc
 *   FUN_1078_02fa        @ 1078:02fa
 *   FUN_1078_0341        @ 1078:0341
 *   FUN_1078_03e9        @ 1078:03e9
 *   FUN_1078_03ff        @ 1078:03ff
 *   FUN_1078_0466        @ 1078:0466
 *   FUN_1078_049e        @ 1078:049e
 *   FUN_1078_0524        @ 1078:0524
 *   FUN_1078_055c        @ 1078:055c
 *   FUN_1078_05dc        @ 1078:05dc
 *   FUN_1078_05f3        @ 1078:05f3
 *   FUN_1078_060d        @ 1078:060d
 *   FUN_1078_0627        @ 1078:0627
 *   FUN_1078_0641        @ 1078:0641
 *   FUN_1078_0673        @ 1078:0673
 *   FUN_1078_069a        @ 1078:069a
 *   FUN_1078_071c        @ 1078:071c
 *   FUN_1078_080d        @ 1078:080d
 *   FUN_1078_085a        @ 1078:085a
 *   FUN_1078_08ac        @ 1078:08ac
 *   FUN_1078_08c3        @ 1078:08c3
 *   FUN_1078_08f0        @ 1078:08f0
 *   FUN_1078_0a85        @ 1078:0a85
 *   FUN_1078_0b25        @ 1078:0b25
 *   FUN_1078_0b6c        @ 1078:0b6c
 *   FUN_1078_0bb3        @ 1078:0bb3
 *   FUN_1078_0c3d        @ 1078:0c3d
 *   FUN_1078_0c59        @ 1078:0c59
 *   FUN_1078_0cc7        @ 1078:0cc7
 *   FUN_1078_0ce7        @ 1078:0ce7
 *   FUN_1078_0d6a        @ 1078:0d6a
 *   FUN_1078_0d8a        @ 1078:0d8a
 *   FUN_1078_0d9b        @ 1078:0d9b
 *   FUN_1078_0dcd        @ 1078:0dcd
 *   FUN_1078_0e0a        @ 1078:0e0a
 *   FUN_1078_0e46        @ 1078:0e46
 *   FUN_1078_0e9e        @ 1078:0e9e
 *   FUN_1078_0ec0        @ 1078:0ec0
 *   FUN_1078_0ef4        @ 1078:0ef4
 *   FUN_1078_0f22        @ 1078:0f22
 *   FUN_1078_0f38        @ 1078:0f38
 *   FUN_1078_0f8d        @ 1078:0f8d
 *   FUN_1078_0fcc        @ 1078:0fcc
 *   FUN_1078_0ffd        @ 1078:0ffd
 *   FUN_1078_1054        @ 1078:1054
 *   FUN_1078_10aa        @ 1078:10aa
 *   FUN_1078_10e9        @ 1078:10e9
 *   FUN_1078_1106        @ 1078:1106
 *   FUN_1078_121b        @ 1078:121b
 *   FUN_1078_1274        @ 1078:1274
 *   FUN_1078_12f5        @ 1078:12f5
 *   FUN_1078_1311        @ 1078:1311
 *   FUN_1078_134f        @ 1078:134f
 *   FUN_1078_1547        @ 1078:1547
 *   FUN_1078_1586        @ 1078:1586
 *   FUN_1078_15c0        @ 1078:15c0
 *   FUN_1078_169d        @ 1078:169d
 *   FUN_1078_16eb        @ 1078:16eb
 *   FUN_1078_170c        @ 1078:170c
 *   FUN_1078_1761        @ 1078:1761
 *   FUN_1078_1788        @ 1078:1788
 *   FUN_1078_17f8        @ 1078:17f8
 *   FUN_1078_1868        @ 1078:1868
 *   FUN_1078_18e9        @ 1078:18e9
 *   FUN_1078_18f5        @ 1078:18f5
 *   FUN_1078_196c        @ 1078:196c
 *   FUN_1078_1992        @ 1078:1992
 *   FUN_1078_1a1a        @ 1078:1a1a
 *   FUN_1078_1a6d        @ 1078:1a6d
 *   FUN_1078_1b14        @ 1078:1b14
 *   FUN_1078_1b41        @ 1078:1b41
 *   FUN_1078_1b57        @ 1078:1b57
 *   FUN_1078_1b63        @ 1078:1b63
 *   FUN_1078_1bbc        @ 1078:1bbc
 *   FUN_1078_1beb        @ 1078:1beb
 *   FUN_1078_1c08        @ 1078:1c08
 *   FUN_1078_1c8b        @ 1078:1c8b
 *   FUN_1078_1ce2        @ 1078:1ce2
 *   FUN_1078_1d2e        @ 1078:1d2e
 *   FUN_1078_1d70        @ 1078:1d70
 *   FUN_1078_1dbd        @ 1078:1dbd
 *   FUN_1078_1e45        @ 1078:1e45
 *   FUN_1078_1ebc        @ 1078:1ebc
 *   FUN_1078_1f20        @ 1078:1f20
 *   FUN_1078_1f62        @ 1078:1f62
 *   FUN_1088_0002        @ 1088:0002
 *   FUN_1088_0014        @ 1088:0014
 *   FUN_1088_002e        @ 1088:002e
 *   FUN_1088_0048        @ 1088:0048
 *   FUN_1090_0002        @ 1090:0002
 *   FUN_1090_0021        @ 1090:0021
 *   FUN_1090_0044        @ 1090:0044
 *   FUN_1090_0098        @ 1090:0098
 *   FUN_1098_0002        @ 1098:0002
 *   FUN_1098_0019        @ 1098:0019
 *   FUN_1098_0030        @ 1098:0030
 *   FUN_1098_0055        @ 1098:0055
 *   FUN_1098_0077        @ 1098:0077
 *   FUN_1098_009f        @ 1098:009f
 *   FUN_1098_00bd        @ 1098:00bd
 *   FUN_1098_00e0        @ 1098:00e0
 *   FUN_1098_010e        @ 1098:010e
 *   FUN_1098_0136        @ 1098:0136
 *   FUN_1098_015d        @ 1098:015d
 *   FUN_1098_01ca        @ 1098:01ca
 */

/* ======================================================== */
/* Function: FUN_1000_0002 @ 1000:0002 */
/* ======================================================== */

void __stdcall16far FUN_1000_0002(undefined4 param_1)

{
  undefined2 uVar1;
  undefined4 uVar2;
  
  uVar2 = FUN_1008_6a95(0,0,0x17e,0x58,(char *)s_RandomShrs_10a8_109f + 9,0,0);
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  *(undefined2 *)((int)param_1 + 8) = (int)uVar2;
  *(undefined2 *)((int)param_1 + 10) = (int)((ulong)uVar2 >> 0x10);
  return;
}

/* ======================================================== */
/* Function: FUN_1000_002f @ 1000:002f */
/* ======================================================== */

void __stdcall16far FUN_1000_002f(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  FUN_1078_1b63((int)param_1,uVar2);
  uVar1 = LOADACCELERATORS((char *)s_Blanks_10a8_1072 + 6,0x62,(char *)s_RandomShrs_10a8_109f + 9);
  *(undefined2 *)((int)param_1 + 0xc) = uVar1;
  return;
}

/* ======================================================== */
/* Function: FUN_1008_2168 @ 1008:2168 */
/* ======================================================== */

void __stdcall16far FUN_1008_2168(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  undefined2 uVar3;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  uVar3 = *(undefined2 *)(iVar1 + 4);
  SETMENU(unaff_CS,*(undefined2 *)(iVar1 + 0x41));
  DESTROYMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar1 + 0x43),uVar3);
  DESTROYMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar1 + 0x45));
  uVar3 = *(undefined2 *)(iVar1 + 4);
  WINHELP((char *)s__Show_Graph_10a8_1226 + 10,0,0,2,0x3a2,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1078_121b(iVar1,uVar2,0);
  FUN_1048_13db(uVar3);
  DELETEOBJECT((char *)s_StartCash_10a8_1043 + 5,DAT_10a8_378e);
  DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_39a0);
  FUN_1048_1bdc((undefined2 *)&DAT_10a8_22aa,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_10a0_0439();
  return;
}

/* ======================================================== */
/* Function: FUN_1008_21eb @ 1008:21eb */
/* ======================================================== */

void FUN_1008_21eb(undefined4 param_1)

{
  int iVar1;
  undefined2 unaff_SS;
  bool bVar2;
  char local_10;
  int local_f;
  undefined2 local_d;
  undefined1 local_b;
  undefined1 local_a;
  
  if (DAT_10a8_1d32 != 0 || DAT_10a8_1d34 != 0) {
    bVar2 = false;
    if ((DAT_10a8_22c6 == '\0') || (iVar1 = FUN_10a0_18f1(0x50), iVar1 != 2)) {
      do {
        local_10 = FUN_10a0_18f1(DAT_10a8_22b2);
        local_10 = local_10 + 'A';
        local_f = FUN_10a0_18f1(DAT_10a8_22b6);
        local_f = local_f + 1;
      } while (*(char *)(local_10 * 0x1a + local_f + 0x169b) != '\x01');
    }
    else {
      local_10 = 'A';
      local_f = -1;
      bVar2 = true;
    }
    local_b = 1;
    local_a = 0;
    local_d = 0;
    if (!bVar2) {
      *(undefined1 *)(local_10 * 0x1a + local_f + 0x169b) = 0;
    }
    bVar2 = DAT_10a8_1d32 == 0;
    DAT_10a8_1d32 = DAT_10a8_1d32 + -1;
    DAT_10a8_1d34 = DAT_10a8_1d34 - (uint)bVar2;
    FUN_10a0_0b92(0xd,(int)param_1,(int)((ulong)param_1 >> 0x10),&local_10,unaff_SS);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_23d1 @ 1008:23d1 */
/* ======================================================== */

void FUN_1008_23d1(int param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int iVar4;
  undefined1 local_3;
  
  local_3 = '\x01';
  while( true ) {
    iVar4 = local_3 * 0x24;
    uVar3 = *(undefined2 *)(iVar4 + 0x376e);
    uVar2 = *(undefined2 *)(param_1 * 0xfc + local_3 * 4 + 0x3184);
    FUN_10a0_1458(*(undefined2 *)(iVar4 + 0x376c),uVar3,*(undefined2 *)(iVar4 + 0x3770));
    FUN_10a0_1444();
    uVar1 = FUN_10a0_1432();
    iVar4 = param_1 * 0xfc;
    *(undefined2 *)(iVar4 + 0x30ca) = uVar1;
    *(undefined2 *)(iVar4 + 0x30cc) = uVar3;
    *(undefined2 *)(iVar4 + 0x30ce) = uVar2;
    if (local_3 == '\b') break;
    local_3 = local_3 + '\x01';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6e62 @ 1008:6e62 */
/* ======================================================== */

char * __stdcall16far FUN_1008_6e62(void)

{
  return s_Oil_Baron_10a8_05f4;
}

/* ======================================================== */
/* Function: FUN_1010_0002 @ 1010:0002 */
/* ======================================================== */

undefined4 __stdcall16far FUN_1010_0002(int param_1,undefined2 param_2)

{
  long lVar1;
  int local_c;
  undefined2 local_a;
  int local_8;
  undefined2 local_6;
  
  lVar1 = FUN_1098_0136(0x5c,param_1,param_2);
  if (lVar1 == 0) {
    lVar1 = FUN_1098_0136(0x3a,param_1,param_2);
  }
  local_a = (undefined2)((ulong)lVar1 >> 0x10);
  local_c = (int)lVar1;
  if (lVar1 == 0) {
    local_8 = param_1;
    local_6 = param_2;
  }
  else {
    local_8 = local_c + 1;
    local_6 = local_a;
  }
  return CONCAT22(local_6,local_8);
}

/* ======================================================== */
/* Function: FUN_1010_006a @ 1010:006a */
/* ======================================================== */

long __stdcall16far FUN_1010_006a(undefined2 param_1,undefined2 param_2)

{
  undefined4 uVar1;
  long lVar2;
  
  uVar1 = FUN_1010_0002(param_1,param_2);
  lVar2 = FUN_1098_010e(0x2e,uVar1);
  if (lVar2 == 0) {
    lVar2 = FUN_1098_0019(param_1,param_2);
  }
  return lVar2;
}

/* ======================================================== */
/* Function: FUN_1010_00c0 @ 1010:00c0 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1010_00c0(undefined2 param_1,undefined2 param_2)

{
  undefined1 uVar1;
  uint uVar2;
  undefined1 uVar3;
  undefined4 uVar4;
  
  uVar4 = FUN_1098_010e(0x2a,param_1,param_2);
  uVar2 = (uint)uVar4 | (uint)((ulong)uVar4 >> 0x10);
  if (uVar2 == 0) {
    uVar4 = FUN_1098_010e(0x3f,param_1,param_2);
    uVar2 = (uint)uVar4 | (uint)((ulong)uVar4 >> 0x10);
    if (uVar2 == 0) {
      uVar1 = 0;
      uVar3 = 0;
      goto LAB_1010_00f0;
    }
  }
  uVar3 = (undefined1)(uVar2 >> 8);
  uVar1 = 1;
LAB_1010_00f0:
  return CONCAT11(uVar3,uVar1);
}

/* ======================================================== */
/* Function: FUN_1010_00fd @ 1010:00fd */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1010_00fd(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             int param_5,int param_6,undefined2 param_7,undefined2 param_8)

{
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  bool bVar3;
  
  bVar3 = true;
  FUN_10a0_03ef((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (!bVar3) {
    if ((param_6 == 0) && (param_5 == 0x7fff)) {
      if (DAT_10a8_1b4a == '\0') {
        param_5 = 0x7f00;
        param_6 = 0;
      }
      else {
        param_5 = 0x7f03;
        param_6 = 0;
      }
    }
    if ((param_6 == 0) && (param_5 == 0x7ffe)) {
      if (DAT_10a8_1b4a == '\0') {
        param_5 = 0x7f01;
        param_6 = 0;
      }
      else {
        param_5 = 0x7f04;
        param_6 = 0;
      }
    }
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    iVar1 = (int)param_1;
    FUN_1080_0002(iVar1,uVar2,0,param_5,param_6,param_7,param_8);
    *(undefined2 *)(iVar1 + 0x26) = 0;
    *(undefined2 *)(iVar1 + 0x28) = 0;
    *(undefined2 *)(iVar1 + 0x2a) = param_3;
    *(undefined2 *)(iVar1 + 0x2c) = param_4;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1010_01a2 @ 1010:01a2 */
/* ======================================================== */

undefined1 __stdcall16far FUN_1010_01a2(undefined4 param_1)

{
  char cVar1;
  int iVar2;
  undefined2 uVar3;
  char *pcVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_CS;
  undefined4 uVar7;
  char *pcVar8;
  char *pcVar9;
  undefined2 uVar10;
  
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  iVar5 = (int)param_1;
  GETDLGITEMTEXT(unaff_CS,0x50,iVar5 + 0x2e,uVar6,100);
  FUN_1018_0002(iVar5 + 0x2e,uVar6,iVar5 + 0x2e,uVar6);
  iVar2 = FUN_1098_0002(iVar5 + 0x2e,uVar6);
  if ((*(char *)(iVar5 + iVar2 + 0x2d) != '\\') &&
     (cVar1 = FUN_1010_00c0(iVar5 + 0x2e,uVar6), cVar1 == '\0')) {
    uVar10 = *(undefined2 *)(iVar5 + 4);
    uVar3 = GETDLGITEM((char *)s_Oil_Baron_10a8_100a + 6,0x67);
    pcVar9 = (char *)s__Show_Graph_10a8_1226 + 10;
    pcVar4 = (char *)GETFOCUS((char *)s__Show_Graph_10a8_1226 + 10,uVar3,uVar10);
    if (pcVar4 != pcVar9) {
      uVar7 = FUN_1098_00e0(0x4f,0x7e6,(char *)s_RandomShrs_10a8_109f + 9,iVar5 + 0x2e,uVar6);
      FUN_1098_00e0(0x4f,iVar5 + 0x83,uVar6,uVar7);
      cVar1 = FUN_1010_0593(iVar5,uVar6);
      if (cVar1 != '\0') {
        return 0;
      }
      *(undefined1 *)(iVar5 + iVar2 + 0x2e) = 0;
      pcVar8 = (char *)FUN_1010_006a(iVar5 + 0x2e,uVar6);
      if (*pcVar8 == '\0') {
        FUN_1098_00e0(0x4f,iVar5 + 0x7e,uVar6,iVar5 + 0x2e,uVar6);
      }
      uVar6 = FUN_1098_0055(iVar5 + 0x2e,uVar6,*(undefined2 *)(iVar5 + 0x2a),
                            *(undefined2 *)(iVar5 + 0x2c));
      ANSILOWER((char *)s_RandomDivs_10a8_1094 + 4,uVar6);
      return 1;
    }
  }
  if (*(char *)(iVar5 + iVar2 + 0x2d) == '\\') {
    FUN_1098_00e0(0x4f,iVar5 + 0x83,uVar6,iVar5 + 0x2e,uVar6);
  }
  cVar1 = FUN_1010_0593(iVar5,uVar6);
  if (cVar1 == '\0') {
    MESSAGEBEEP((char *)s_Oil_Baron_10a8_100a + 6,0);
    FUN_1010_051c(iVar5,uVar6);
  }
  return 0;
}

/* ======================================================== */
/* Function: FUN_1010_0312 @ 1010:0312 */
/* ======================================================== */

void __stdcall16far FUN_1010_0312(undefined4 param_1)

{
  char cVar1;
  int unaff_BP;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_CS;
  undefined4 uVar5;
  undefined2 uVar6;
  char *pcVar7;
  
  iVar2 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  uVar6 = *(undefined2 *)(iVar3 + 4);
  SENDDLGITEMMESSAGE(unaff_CS,0,0,0x4f,0x415,100);
  if (*(int *)(iVar3 + 0x26) != 0 || *(int *)(iVar3 + 0x28) != 0) {
    SETWINDOWTEXT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar3 + 0x26),
                  *(undefined2 *)(iVar3 + 0x28),*(undefined2 *)(iVar3 + 4),uVar6,pcVar7,iVar2);
  }
  FUN_1098_0077(0x4f,*(undefined2 *)(iVar3 + 0x2a),*(undefined2 *)(iVar3 + 0x2c),iVar3 + 0x2e,uVar4)
  ;
  iVar2 = iVar3 + 0x7e;
  uVar6 = uVar4;
  uVar5 = FUN_1010_006a(iVar3 + 0x2e,uVar4);
  FUN_1098_0077(4,uVar5,iVar2,uVar6);
  cVar1 = FUN_1010_00c0(iVar3 + 0x7e,uVar4);
  if (cVar1 != '\0') {
    *(undefined1 *)(iVar3 + 0x7e) = 0;
  }
  cVar1 = FUN_1010_0593(iVar3,uVar4);
  if (cVar1 == '\0') {
    FUN_1098_0055(0x7e8,(char *)s_RandomShrs_10a8_109f + 9,iVar3 + 0x2e,uVar4);
    FUN_1010_0593(iVar3,uVar4);
  }
  FUN_1010_051c(iVar3,uVar4);
  return;
}

/* ======================================================== */
/* Function: FUN_1010_03dd @ 1010:03dd */
/* ======================================================== */

void __stdcall16far FUN_1010_03dd(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  undefined2 unaff_CS;
  long lVar1;
  
  if (*(int *)((int)param_3 + 8) == 0x300) {
    GETDLGITEM(unaff_CS,1);
    lVar1 = SENDMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,0,0,0,0xe);
    ENABLEWINDOW((char *)s__Show_Graph_10a8_1226 + 10,lVar1 != 0);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1010_0425 @ 1010:0425 */
/* ======================================================== */

void __stdcall16far FUN_1010_0425(undefined4 param_1,undefined4 param_2)

{
  int unaff_BP;
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_CS;
  undefined2 uVar6;
  char *pcVar7;
  
  iVar1 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = (int)param_2;
  iVar3 = *(int *)(iVar2 + 8);
  if ((iVar3 == 1) || (iVar3 == 2)) {
    uVar5 = (undefined2)((ulong)param_1 >> 0x10);
    iVar3 = (int)param_1;
    uVar6 = *(undefined2 *)(iVar3 + 4);
    DLGDIRSELECT(unaff_CS,0x66,iVar3 + 0x2e,uVar5);
    FUN_1010_0554(iVar3,uVar5);
    if (*(int *)(iVar2 + 8) == 2) {
      FUN_10a0_1e88((char *)s_Oil_Baron_10a8_100a + 6,iVar3,uVar5,iVar2,uVar4,uVar6,pcVar7,iVar1);
    }
  }
  else if (iVar3 == 5) {
    SENDMESSAGE(unaff_CS,0,0,0xffff,0x407);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1010_0498 @ 1010:0498 */
/* ======================================================== */

void __stdcall16far FUN_1010_0498(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  
  uVar2 = (undefined2)((ulong)param_2 >> 0x10);
  iVar1 = *(int *)((int)param_2 + 8);
  if ((iVar1 == 1) || (iVar1 == 2)) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    iVar1 = (int)param_1;
    DLGDIRSELECT(unaff_CS,0x67,iVar1 + 0x2e,uVar3);
    FUN_1098_00bd(iVar1 + 0x83,uVar3,iVar1 + 0x2e,uVar3);
    if (*(int *)((int)param_2 + 8) == 2) {
      FUN_1010_0593(iVar1,uVar3);
    }
    else {
      FUN_1010_0554(iVar1,uVar3);
    }
  }
  else if (iVar1 == 5) {
    SENDMESSAGE(unaff_CS,0,0,0xffff,0x407);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1010_051c @ 1010:051c */
/* ======================================================== */

void __stdcall16far FUN_1010_051c(undefined4 param_1)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined2 uVar4;
  char *pcVar5;
  
  iVar2 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  uVar4 = *(undefined2 *)((int)param_1 + 4);
  SENDDLGITEMMESSAGE(unaff_CS,0,0x7fff,0,0x401,100);
  uVar3 = *(undefined2 *)((int)param_1 + 4);
  uVar1 = GETDLGITEM((char *)s__Show_Graph_10a8_1226 + 10,100);
  SETFOCUS((char *)s__Show_Graph_10a8_1226 + 10,uVar1,uVar3,uVar4,pcVar5,iVar2);
  return;
}

/* ======================================================== */
/* Function: FUN_1010_0554 @ 1010:0554 */
/* ======================================================== */

void __stdcall16far FUN_1010_0554(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  undefined4 uVar2;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  uVar2 = ANSILOWER(unaff_CS,(int)param_1 + 0x2e);
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,uVar2,uVar1);
  SENDDLGITEMMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,0,0x7fff,0,0x401,100);
  return;
}

/* ======================================================== */
/* Function: FUN_1010_0593 @ 1010:0593 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1010_0593(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined1 uVar3;
  undefined1 extraout_AH;
  int unaff_BP;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined4 uVar7;
  undefined2 uVar8;
  undefined1 *puVar9;
  undefined2 uVar10;
  undefined1 local_58 [80];
  int local_8;
  undefined1 local_5;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  local_5 = 0;
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  iVar1 = GETDLGITEM(unaff_CS,0x66);
  if (iVar1 == 0) {
    puVar9 = local_58;
    iVar5 = iVar4 + 0x2e;
    iVar1 = iVar4 + 0x2e;
    uVar8 = uVar6;
    uVar10 = unaff_SS;
    iVar2 = FUN_1010_0002(iVar4 + 0x2e,uVar6);
    FUN_1098_0077(iVar2 - iVar1,iVar5,uVar8,puVar9,uVar10);
    FUN_1098_00e0(0x4f,0x7ec,(char *)s_RandomShrs_10a8_109f + 9,local_58,unaff_SS);
    iVar1 = DLGDIRLIST((char *)s_RandomDivs_10a8_1094 + 4,0xc010,0x65,0x67,local_58,unaff_SS);
    local_8 = iVar1;
  }
  else {
    FUN_1098_0055(iVar4 + 0x2e,uVar6,local_58,unaff_SS);
    iVar1 = DLGDIRLIST((char *)s_RandomDivs_10a8_1094 + 4,0,0x65,0x66,local_58,unaff_SS);
    local_8 = iVar1;
    if (iVar1 != 0) {
      iVar1 = DLGDIRLIST((char *)s__Show_Graph_10a8_1226 + 10,0xc010,0,0x67,0x7ec,
                         (char *)s_RandomShrs_10a8_109f + 9);
    }
  }
  uVar3 = (undefined1)((uint)iVar1 >> 8);
  if (local_8 != 0) {
    iVar1 = iVar4 + 0x83;
    uVar8 = uVar6;
    uVar7 = FUN_1010_0002(iVar4 + 0x2e,uVar6);
    FUN_1098_0077(0xc,uVar7,iVar1,uVar8);
    FUN_1098_0055(iVar4 + 0x83,uVar6,iVar4 + 0x2e,uVar6);
    FUN_1010_0554(iVar4,uVar6);
    local_5 = 1;
    uVar3 = extraout_AH;
  }
  return CONCAT11(uVar3,local_5);
}

/* ======================================================== */
/* Function: FUN_1018_0002 @ 1018:0002 */
/* ======================================================== */

undefined4 __stdcall16far FUN_1018_0002(char *param_1,undefined1 *param_2)

{
  undefined2 *puVar1;
  char *pcVar2;
  undefined1 *puVar3;
  code *pcVar4;
  byte bVar5;
  char cVar6;
  int iVar7;
  undefined1 uVar9;
  uint uVar8;
  char *pcVar10;
  undefined2 *puVar11;
  char *pcVar12;
  char *pcVar13;
  undefined1 *puVar14;
  undefined1 *puVar15;
  undefined2 uVar16;
  char *unaff_CS;
  undefined2 unaff_SS;
  undefined2 uVar17;
  bool bVar18;
  undefined2 local_a2;
  char local_a0;
  char acStack_9f [157];
  
  pcVar13 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar17 = (undefined2)((ulong)param_1 >> 0x10);
  pcVar10 = (char *)param_1;
  pcVar12 = pcVar10 + 2;
  local_a2 = *(undefined2 *)param_1;
  bVar5 = (byte)local_a2;
  if ((bVar5 == 0) || ((char)((uint)local_a2 >> 8) != ':')) {
    pcVar4 = (code *)swi(0x21);
    cVar6 = (*pcVar4)();
    local_a2 = CONCAT11(0x3a,cVar6 + 'A');
  }
  else {
    pcVar10 = pcVar12;
    if ((0x60 < bVar5) && (bVar5 < 0x7b)) {
      local_a2 = CONCAT11(0x3a,bVar5 - 0x20);
    }
  }
  pcVar12 = &local_a0;
  uVar16 = unaff_SS;
  if (*pcVar10 != '\\') {
    bVar18 = (byte)local_a2 < 0x40;
    pcVar12 = acStack_9f;
    local_a0 = '\\';
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)(uVar17,pcVar13);
    if ((!bVar18) && (acStack_9f[0] != '\0')) {
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      Ordinal_6();
      iVar7 = -1;
      do {
        if (iVar7 == 0) break;
        iVar7 = iVar7 + -1;
        pcVar2 = pcVar12;
        pcVar12 = pcVar12 + 1;
      } while (*pcVar2 != '\0');
      pcVar12[-1] = '\\';
    }
  }
  iVar7 = 8;
  while( true ) {
    pcVar2 = pcVar10;
    pcVar10 = pcVar10 + 1;
    cVar6 = *pcVar2;
    uVar9 = (undefined1)((uint)iVar7 >> 8);
    pcVar13 = pcVar12;
    if ((cVar6 == '\0') || (cVar6 == '\\')) break;
    if (cVar6 == '.') {
      iVar7 = CONCAT11(uVar9,3);
      goto LAB_1018_0080;
    }
    if (iVar7 != 0) {
      iVar7 = iVar7 + -1;
LAB_1018_0080:
      pcVar12 = pcVar13 + 1;
      *pcVar13 = cVar6;
    }
  }
  if (*(int *)(pcVar12 + -2) == 0x2e5c) {
    pcVar13 = pcVar12 + -2;
  }
  else if (((*(int *)(pcVar12 + -2) == 0x2e2e) && (pcVar12[-3] == '\\')) &&
          (pcVar13 = pcVar12 + -3, pcVar12[-4] != ':')) {
    do {
      pcVar13 = pcVar13 + -1;
    } while (*pcVar13 != '\\');
  }
  iVar7 = CONCAT11(uVar9,8);
  if (cVar6 == '\0') {
    if (pcVar13[-1] == ':') {
      pcVar2 = pcVar13;
      pcVar13 = pcVar13 + 1;
      *pcVar2 = '\\';
    }
    puVar11 = &local_a2;
    uVar8 = (int)pcVar13 - (int)puVar11;
    if (0x4f < uVar8) {
      uVar8 = 0x4f;
    }
    uVar17 = (undefined2)((ulong)param_2 >> 0x10);
    puVar14 = (undefined1 *)param_2;
    puVar15 = puVar14;
    for (; uVar8 != 0; uVar8 = uVar8 - 1) {
      puVar3 = puVar15;
      puVar15 = puVar15 + 1;
      puVar1 = puVar11;
      puVar11 = (undefined2 *)((int)puVar11 + 1);
      *puVar3 = *(undefined1 *)puVar1;
    }
    *puVar15 = 0;
    ANSIUPPER(unaff_CS,puVar14);
    return CONCAT22(puVar14,uVar17);
  }
  goto LAB_1018_0080;
}

/* ======================================================== */
/* Function: FUN_1020_0002 @ 1020:0002 */
/* ======================================================== */

undefined4 __stdcall16far FUN_1020_0002(undefined4 param_1)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  bool bVar5;
  undefined2 in_stack_0000000e;
  undefined2 in_stack_00000010;
  
  bVar5 = true;
  FUN_10a0_03ef();
  if (!bVar5) {
    uVar4 = (undefined2)((ulong)param_1 >> 0x10);
    iVar3 = (int)param_1;
    FUN_1078_1106(iVar3,uVar4,0,0x84e,(char *)s_RandomShrs_10a8_109f + 9,in_stack_0000000e,
                  in_stack_00000010);
    *(undefined2 *)(iVar3 + 0x21) = 0;
    *(undefined2 *)(iVar3 + 0x23) = 0x90cc;
    uVar1 = GETDC((char *)s_Blanks_10a8_1072 + 6,0);
    iVar2 = GETDEVICECAPS((char *)s__Show_Graph_10a8_1226 + 10,8);
    *(int *)(iVar3 + 0x2d) = iVar2 / 4;
    iVar2 = GETDEVICECAPS((char *)s__Show_Graph_10a8_1226 + 10,10);
    *(int *)(iVar3 + 0x2f) = iVar2 / 6;
    RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
    *(undefined2 *)(iVar3 + 0x29) = 0x32;
    *(undefined2 *)(iVar3 + 0x2b) = 0x32;
    DAT_10a8_1d24 = 0;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1020_009f @ 1020:009f */
/* ======================================================== */

void __stdcall16far FUN_1020_009f(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 4) == 9) {
    SETFOCUS();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1020_00c8 @ 1020:00c8 */
/* ======================================================== */

void __stdcall16far FUN_1020_00c8(undefined4 param_1)

{
  int unaff_BP;
  int iVar1;
  char *pcVar2;
  
  iVar1 = unaff_BP + 1;
  pcVar2 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_1078_121b((int)param_1,(int)((ulong)param_1 >> 0x10),0);
  DAT_10a8_1d24 = 1;
  FUN_10a0_0439(pcVar2,iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1020_00ed @ 1020:00ed */
/* ======================================================== */

void __stdcall16far FUN_1020_00ed(undefined2 param_1,undefined2 param_2,int param_3,int param_4)

{
  undefined2 unaff_CS;
  
  MOVEWINDOW(unaff_CS,1,param_3 / 4,param_4 / 3,(param_3 * 3) / 4,(param_4 << 1) / 3);
  return;
}

/* ======================================================== */
/* Function: FUN_1020_0137 @ 1020:0137 */
/* ======================================================== */

char * __stdcall16far FUN_1020_0137(void)

{
  return s_Wealth_Window_10a8_085e;
}

/* ======================================================== */
/* Function: FUN_1020_0157 @ 1020:0157 */
/* ======================================================== */

void __stdcall16far FUN_1020_0157(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_2 >> 0x10);
  FUN_1078_1274((int)param_1,(int)((ulong)param_1 >> 0x10),(int)param_2,uVar1);
  *(undefined2 *)((int)param_2 + 0x10) = DAT_10a8_39a0;
  return;
}

/* ======================================================== */
/* Function: FUN_1020_017c @ 1020:017c */
/* ======================================================== */

void __stdcall16far FUN_1020_017c(undefined4 param_1,undefined2 param_2,undefined1 *param_3)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  undefined1 local_e2 [222];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = local_e2;
  puVar4 = (undefined1 *)param_3;
  for (iVar3 = 0xde; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  FUN_10a0_0b92(0xde,iVar3 + 0x43,uVar6,local_e2,unaff_SS);
  *(undefined2 *)(iVar3 + 0x41) = param_2;
  INVALIDATERECT((char *)s_RandomShrs_10a8_109f + 1,1,0,0,*(undefined2 *)(iVar3 + 4));
  return;
}

/* ======================================================== */
/* Function: FUN_1020_01d1 @ 1020:01d1 */
/* ======================================================== */

void __stdcall16far
FUN_1020_01d1(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int unaff_BP;
  uint uVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_CS;
  char *pcVar9;
  undefined2 unaff_SS;
  bool bVar10;
  bool bVar11;
  undefined4 uVar12;
  undefined1 local_38 [32];
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  undefined2 local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar8 = (undefined2)((ulong)param_1 >> 0x10);
  uVar6 = (uint)param_1;
  if (*(int *)(uVar6 + 0x41) != 0) {
    SETMAPMODE(unaff_CS,8);
    uVar4 = *(undefined2 *)(uVar6 + 4);
    GETCLIENTRECT((char *)s__Show_Graph_10a8_1226 + 10,&local_e,unaff_SS);
    uVar2 = param_4;
    SETWINDOWEXT((char *)s__Show_Graph_10a8_1226 + 10,1000,1000);
    uVar5 = param_4;
    SETVIEWPORTEXT((char *)s__Show_Graph_10a8_1226 + 10,-local_c - local_8,local_a - local_e);
    uVar3 = param_4;
    SETVIEWPORTORG((char *)s__Show_Graph_10a8_1226 + 10,local_8 - local_c,0);
    FUN_10a0_1458(uVar3,uVar5,uVar2,uVar4);
    uVar4 = 0;
    FUN_10a0_144a();
    uVar2 = FUN_10a0_145c();
    local_6 = CREATEFONT((char *)s_RandomShrs_10a8_109f + 1,0,0,0x20,0,0,0,0,0,0,0,300,0,0,0x14);
    uVar5 = param_4;
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_6);
    pcVar9 = (char *)s__Show_Graph_10a8_1226 + 10;
    uVar3 = param_4;
    SETBKMODE((char *)s__Show_Graph_10a8_1226 + 10,1);
    local_16 = 0;
    local_14 = 0;
    local_12 = 0;
    iVar1 = *(int *)(uVar6 + 0x41);
    if (0 < iVar1) {
      local_10 = 1;
      while( true ) {
        bVar10 = CARRY2(uVar6,local_10 * 0x25);
        iVar7 = uVar6 + local_10 * 0x25;
        bVar11 = iVar7 == 0;
        uVar4 = *(undefined2 *)(iVar7 + 0x3f);
        pcVar9 = (char *)s_RandomShrs_10a8_109f + 1;
        FUN_10a0_1454(uVar3,uVar5,uVar2);
        if (!bVar10 && !bVar11) {
          iVar7 = uVar6 + local_10 * 0x25;
          local_16 = *(undefined2 *)(iVar7 + 0x3d);
          uVar4 = *(undefined2 *)(iVar7 + 0x3f);
          local_12 = *(undefined2 *)(iVar7 + 0x41);
          local_14 = uVar4;
        }
        if (local_10 == iVar1) break;
        local_10 = local_10 + 1;
      }
    }
    iVar1 = *(int *)(uVar6 + 0x41);
    if (0 < iVar1) {
      local_10 = 1;
      while( true ) {
        uVar3 = param_4;
        uVar5 = param_4;
        uVar12 = GETNEARESTCOLOR(pcVar9,*(undefined2 *)(local_10 * 4 + 0x868),
                                 *(undefined2 *)(local_10 * 4 + 0x86a));
        SETTEXTCOLOR((char *)s__Show_Graph_10a8_1226 + 10,uVar12);
        local_e = 0x14;
        local_a = 200;
        iVar7 = local_10 + -1 >> 0xf;
        uVar3 = FUN_10a0_1458(uVar3,uVar5);
        FUN_10a0_1458(uVar3,uVar4,iVar7);
        FUN_10a0_144a();
        FUN_10a0_1444();
        FUN_10a0_1438();
        local_c = FUN_10a0_145c();
        FUN_10a0_1458();
        uVar5 = 0;
        uVar3 = 0x4800;
        FUN_10a0_144a();
        uVar4 = FUN_10a0_1444();
        FUN_10a0_1458(uVar4,uVar5,uVar3);
        FUN_10a0_1438();
        local_8 = FUN_10a0_145c();
        DRAWTEXT((char *)s_RandomShrs_10a8_109f + 1,0x20,&local_e,unaff_SS,0xffff,
                 uVar6 + local_10 * 0x25 + 0x1e,uVar8);
        local_e = 0x2a8;
        local_a = 0x3ca;
        iVar7 = uVar6 + local_10 * 0x25;
        FUN_10a0_0af5(0x1e,local_38,unaff_SS,2,7,*(undefined2 *)(iVar7 + 0x3d),
                      *(undefined2 *)(iVar7 + 0x3f),*(undefined2 *)(iVar7 + 0x41));
        SETTEXTCOLOR((char *)s_RandomShrs_10a8_109f + 1,0,0);
        DRAWTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x22,&local_e,unaff_SS,0xffff,local_38,
                 unaff_SS);
        uVar4 = GETNEARESTCOLOR((char *)s__Show_Graph_10a8_1226 + 10,
                                *(undefined2 *)(local_10 * 4 + 0x868),
                                *(undefined2 *)(local_10 * 4 + 0x86a));
        local_18 = CREATESOLIDBRUSH((char *)s__Show_Graph_10a8_1226 + 10,uVar4);
        SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_18);
        local_e = 0xd2;
        iVar7 = local_10 + -1 >> 0xf;
        uVar4 = FUN_10a0_1458();
        FUN_10a0_1458(uVar4,uVar5,iVar7);
        FUN_10a0_144a();
        FUN_10a0_1444();
        FUN_10a0_1438();
        local_c = FUN_10a0_145c();
        iVar7 = uVar6 + local_10 * 0x25;
        uVar4 = *(undefined2 *)(iVar7 + 0x3f);
        uVar3 = *(undefined2 *)(iVar7 + 0x41);
        FUN_10a0_1444();
        uVar5 = FUN_10a0_144a();
        FUN_10a0_1458(uVar5,uVar4,uVar3);
        FUN_10a0_1432();
        local_a = FUN_10a0_145c();
        FUN_10a0_1458();
        uVar4 = 0;
        uVar5 = 0x4800;
        FUN_10a0_144a();
        uVar3 = FUN_10a0_1444();
        FUN_10a0_1458(uVar3,uVar4,uVar5);
        FUN_10a0_1438();
        local_8 = FUN_10a0_145c();
        RECTANGLE((char *)s_RandomShrs_10a8_109f + 1,local_8,local_a,local_c,local_e);
        uVar3 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,5,param_4);
        SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar3);
        pcVar9 = (char *)s__Show_Graph_10a8_1226 + 10;
        DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_18);
        if (local_10 == iVar1) break;
        local_10 = local_10 + 1;
      }
    }
    uVar8 = GETSTOCKOBJECT(pcVar9,0xb,param_4);
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar8);
    DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_6);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1020_05ca @ 1020:05ca */
/* ======================================================== */

void __cdecl16far FUN_1020_05ca(void)

{
  DAT_10a8_1d24 = 1;
  return;
}

/* ======================================================== */
/* Function: FUN_1028_0002 @ 1028:0002 */
/* ======================================================== */
/* Failed to decompile function: FUN_1028_0002 @ 1028:0002: 
Low-level Error: Forced merge caused intersection */

/* ======================================================== */
/* Function: FUN_1028_0175 @ 1028:0175 */
/* ======================================================== */

void __cdecl16far FUN_1028_0175(void)

{
  if (((int)DAT_10a8_1d28 != 0 || DAT_10a8_1d28._2_2_ != 0) && (DAT_10a8_1d26 == '\0')) {
    FUN_1078_0f38((int)DAT_10a8_1d28,DAT_10a8_1d28._2_2_);
    DAT_10a8_1d28 = 0;
    DAT_10a8_1d26 = '\x01';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1028_01a9 @ 1028:01a9 */
/* ======================================================== */

void __stdcall16far
FUN_1028_01a9(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6,undefined2 param_7,undefined2 param_8,
             undefined2 param_9)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  undefined2 unaff_CS;
  undefined2 uVar3;
  char *pcVar4;
  
  iVar2 = unaff_BP + 1;
  pcVar4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar1 = param_9;
  MOVETO(unaff_CS,param_1,param_7);
  uVar1 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,6,param_9,uVar1,pcVar4,iVar2);
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
  uVar3 = param_9;
  LINETO((char *)s__Show_Graph_10a8_1226 + 10,param_5,param_7);
  uVar1 = param_9;
  LINETO((char *)s__Show_Graph_10a8_1226 + 10,param_5,param_3);
  uVar1 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,7,param_9,uVar1,uVar3);
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
  LINETO((char *)s__Show_Graph_10a8_1226 + 10,param_1,param_3);
  LINETO((char *)s__Show_Graph_10a8_1226 + 10,param_1,param_7);
  return;
}

/* ======================================================== */
/* Function: FUN_1028_021b @ 1028:021b */
/* ======================================================== */

void __stdcall16far FUN_1028_021b(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 4) == 9) {
    SETFOCUS();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1028_0244 @ 1028:0244 */
/* ======================================================== */

void __stdcall16far FUN_1028_0244(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 in_BX;
  int unaff_BP;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined1 local_1a [4];
  int local_16;
  int local_14;
  int local_12;
  undefined2 local_10;
  undefined2 local_c;
  undefined2 local_a;
  undefined2 local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  uVar1 = *(undefined2 *)(iVar4 + 4);
  GETCLIENTRECT(unaff_CS,local_1a,unaff_SS);
  iVar2 = local_16 >> 0xf;
  uVar1 = FUN_10a0_1458(uVar1);
  uVar3 = 0;
  FUN_10a0_1458(uVar1,in_BX,iVar2);
  local_a = FUN_10a0_144a();
  iVar2 = local_14 >> 0xf;
  local_6 = uVar3;
  uVar1 = FUN_10a0_1458();
  uVar3 = 0;
  FUN_10a0_1458(uVar1,in_BX,iVar2);
  local_10 = FUN_10a0_144a();
  local_c = uVar3;
  FUN_10a0_1438();
  FUN_10a0_144a();
  iVar2 = FUN_10a0_1464();
  iVar2 = iVar2 + 1;
  if (iVar2 < 1) {
    local_12 = 1;
  }
  else {
    local_12 = iVar2;
    if (8 < iVar2) {
      local_12 = 8;
    }
  }
  *(undefined1 *)(iVar4 + 0x41) = (undefined1)local_12;
  uVar1 = (undefined2)((ulong)param_2 >> 0x10);
  FUN_1030_0002(CONCAT11((char)((uint)iVar2 >> 8),*(undefined1 *)(iVar4 + 0x41)),
                *(undefined2 *)((int)param_2 + 8),*(undefined2 *)((int)param_2 + 6),0x3790,
                (char *)s_RandomShrs_10a8_109f + 9,iVar4 + 0x48,uVar5,iVar4,uVar5);
  return;
}

/* ======================================================== */
/* Function: FUN_1028_0334 @ 1028:0334 */
/* ======================================================== */

void __stdcall16far FUN_1028_0334(int param_1,int param_2)

{
  undefined2 unaff_CS;
  
  SHOWWINDOW(unaff_CS,9);
  MOVEWINDOW((char *)s__Show_Graph_10a8_1226 + 10,1,(param_1 * 3) / 4,param_2 / 3,0,
             (param_2 << 1) / 3);
  return;
}

/* ======================================================== */
/* Function: FUN_1028_0386 @ 1028:0386 */
/* ======================================================== */

void __cdecl16far FUN_1028_0386(void)

{
  if (DAT_10a8_1d26 == '\0') {
    SETFOCUS();
    SHOWWINDOW((char *)s__Show_Graph_10a8_1226 + 10,5);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1028_03b5 @ 1028:03b5 */
/* ======================================================== */

void __stdcall16far FUN_1028_03b5(undefined4 param_1)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  char *pcVar5;
  
  iVar2 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  FUN_1078_170c(iVar3,uVar4);
  if (DAT_10a8_1d27 == '\0') {
    uVar1 = GETMENU((char *)s_Blanks_10a8_1072 + 6,*(undefined2 *)(iVar3 + 4),pcVar5,iVar2);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0x6a);
    GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar3 + 4),uVar1);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x69);
  }
  else {
    uVar1 = GETMENU((char *)s_Blanks_10a8_1072 + 6,*(undefined2 *)(iVar3 + 4),pcVar5,iVar2);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x6a);
    GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar3 + 4),uVar1);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0x69);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1028_042c @ 1028:042c */
/* ======================================================== */

undefined4 __stdcall16far FUN_1028_042c(undefined4 param_1)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  undefined2 uVar3;
  bool bVar4;
  undefined2 in_stack_0000000e;
  undefined2 in_stack_00000010;
  
  bVar4 = true;
  FUN_10a0_03ef((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (!bVar4) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    iVar2 = (int)param_1;
    FUN_1078_1106(iVar2,uVar3,0,0x966,(char *)s_RandomShrs_10a8_109f + 9,in_stack_0000000e,
                  in_stack_00000010);
    *(undefined2 *)(iVar2 + 0x21) = 0;
    *(undefined2 *)(iVar2 + 0x23) = 0x80cf;
    *(undefined2 *)(iVar2 + 0x29) = 0x32;
    *(undefined2 *)(iVar2 + 0x2b) = 0x32;
    *(undefined2 *)(iVar2 + 0x2f) = 0x177;
    *(undefined2 *)(iVar2 + 0x2d) = 500;
    uVar1 = LOADMENU((char *)s_Blanks_10a8_1072 + 6,0x97e,(char *)s_RandomShrs_10a8_109f + 9);
    *(undefined2 *)(iVar2 + 0x35) = uVar1;
    uVar1 = CREATEPEN((char *)s__Show_Graph_10a8_1226 + 10,0,0,2);
    *(undefined2 *)(iVar2 + 0x44) = uVar1;
    DAT_10a8_1d26 = 0;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1028_04b3 @ 1028:04b3 */
/* ======================================================== */

void __stdcall16far FUN_1028_04b3(undefined4 param_1)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_CS;
  char *pcVar5;
  
  iVar2 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  DAT_10a8_1d27 = 0;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  INVALIDATERECT(unaff_CS,1,0,0,*(undefined2 *)(iVar3 + 4));
  uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar3 + 4),pcVar5,iVar2);
  CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0x6a);
  GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar3 + 4),uVar1);
  CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x69);
  return;
}

/* ======================================================== */
/* Function: FUN_1028_0502 @ 1028:0502 */
/* ======================================================== */

void __stdcall16far FUN_1028_0502(undefined4 param_1)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_CS;
  char *pcVar5;
  
  iVar2 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  DAT_10a8_1d27 = 1;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  INVALIDATERECT(unaff_CS,1,0,0,*(undefined2 *)(iVar3 + 4));
  uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar3 + 4),pcVar5,iVar2);
  CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x6a);
  GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar3 + 4),uVar1);
  CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0x69);
  return;
}

/* ======================================================== */
/* Function: FUN_1028_0551 @ 1028:0551 */
/* ======================================================== */

void __stdcall16far FUN_1028_0551(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  INVALIDATERECT(unaff_CS,1,0,0,*(undefined2 *)((int)param_1 + 4));
  FUN_1078_18f5((int)param_1,uVar1,(int)param_2,(int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1028_057e @ 1028:057e */
/* ======================================================== */

char * __stdcall16far FUN_1028_057e(void)

{
  return s_OilGraph_10a8_0988;
}

/* ======================================================== */
/* Function: FUN_1028_059e @ 1028:059e */
/* ======================================================== */

void __stdcall16far FUN_1028_059e(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = (int)param_2;
  FUN_1078_1274((int)param_1,(int)((ulong)param_1 >> 0x10),iVar2,uVar3);
  uVar1 = LOADICON((char *)s_Blanks_10a8_1072 + 6,0x992,(char *)s_RandomShrs_10a8_109f + 9);
  *(undefined2 *)(iVar2 + 0xc) = uVar1;
  uVar1 = CREATEBRUSHINDIRECT((char *)s__Show_Graph_10a8_1226 + 10,0x19be);
  *(undefined2 *)(iVar2 + 0x10) = uVar1;
  return;
}

/* ======================================================== */
/* Function: FUN_1028_05df @ 1028:05df */
/* ======================================================== */

void __stdcall16far FUN_1028_05df(undefined4 param_1)

{
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  char *pcVar4;
  
  iVar1 = unaff_BP + 1;
  pcVar4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  uVar3 = *(undefined2 *)((int)param_1 + 0x44);
  DELETEOBJECT();
  DAT_10a8_1d26 = 1;
  FUN_1078_121b((int)param_1,uVar2,0);
  FUN_10a0_0439(uVar3,pcVar4,iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1028_0610 @ 1028:0610 */
/* ======================================================== */

void __stdcall16far FUN_1028_0610(undefined4 param_1,undefined4 param_2,undefined2 param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  uint uVar8;
  int unaff_BP;
  int iVar9;
  undefined2 uVar10;
  char *pcVar11;
  undefined2 unaff_SS;
  bool bVar12;
  undefined4 uVar13;
  undefined2 uVar14;
  undefined2 uVar15;
  undefined2 uVar16;
  uint local_1a2 [18];
  char local_17d;
  undefined2 local_17c;
  int local_17a;
  uint local_178;
  int local_176;
  uint local_174;
  uint local_172;
  uint local_170;
  uint local_16e;
  int local_16c;
  int local_16a;
  int local_168;
  char *local_166;
  undefined1 local_15c [184];
  undefined1 local_a4 [160];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar10 = (undefined2)((ulong)param_1 >> 0x10);
  iVar9 = (int)param_1;
  FUN_1078_18e9(iVar9,uVar10,(int)param_2,(int)((ulong)param_2 >> 0x10),param_3);
  local_170 = 0;
  local_16e = 0;
  local_17d = '\x01';
  while( true ) {
    iVar4 = (int)local_17d;
    local_1a2[iVar4 * 2] = 0;
    local_1a2[iVar4 * 2 + 1] = 0;
    uVar1 = *(uint *)(iVar9 + 0x42);
    local_1a2[0] = (int)uVar1 >> 0xf;
    if ((local_1a2[0] < 0x8000) && ((0 < (int)local_1a2[0] || (uVar1 != 0)))) {
      local_174 = 1;
      local_172 = 0;
      while( true ) {
        iVar4 = iVar9 + local_174 * 0xfc + local_17d * 4;
        uVar8 = *(uint *)(iVar4 + 0x24);
        iVar4 = *(int *)(iVar4 + 0x26);
        uVar2 = local_1a2[local_17d * 2];
        uVar3 = local_1a2[local_17d * 2 + 1];
        iVar5 = (int)local_17d;
        local_1a2[iVar5 * 2] = uVar2 + uVar8;
        local_1a2[iVar5 * 2 + 1] = uVar3 + iVar4 + (uint)CARRY2(uVar2,uVar8);
        if ((local_172 == local_1a2[0]) && (local_174 == uVar1)) break;
        bVar12 = 0xfffe < local_174;
        local_174 = local_174 + 1;
        local_172 = local_172 + bVar12;
      }
    }
    if (((int)local_16e < (int)local_1a2[local_17d * 2 + 1]) ||
       (((int)local_16e <= (int)local_1a2[local_17d * 2 + 1] &&
        (local_170 < local_1a2[local_17d * 2])))) {
      local_170 = local_1a2[local_17d * 2];
      local_16e = local_1a2[local_17d * 2 + 1];
    }
    if (local_17d == '\b') break;
    local_17d = local_17d + '\x01';
  }
  SETMAPMODE((char *)s_Blanks_10a8_1072 + 6,8);
  GETCLIENTRECT((char *)s__Show_Graph_10a8_1226 + 10,&local_16c,unaff_SS);
  SETWINDOWEXT((char *)s__Show_Graph_10a8_1226 + 10,5000,5000);
  SETVIEWPORTEXT((char *)s__Show_Graph_10a8_1226 + 10,-local_16a - (int)local_166,
                 local_168 - local_16c);
  SETVIEWPORTORG((char *)s__Show_Graph_10a8_1226 + 10,(int)local_166 - local_16a,0);
  uVar6 = CREATEFONT((char *)s__Show_Graph_10a8_1226 + 10,0,0,0x20,0,0,0,0,0,0,0,300,0,0,0x2d);
  *(undefined2 *)(iVar9 + 0x46) = uVar6;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar9 + 0x46));
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar9 + 0x44));
  GETTEXTMETRICS((char *)s__Show_Graph_10a8_1226 + 10,local_15c,unaff_SS);
  MOVETO((char *)s__Show_Graph_10a8_1226 + 10,0x128e,0x1c2);
  LINETO((char *)s__Show_Graph_10a8_1226 + 10,0x128e,0x1194);
  MOVETO((char *)s__Show_Graph_10a8_1226 + 10,0xb3b,0x1c2);
  LINETO((char *)s__Show_Graph_10a8_1226 + 10,0xb3b,0x1194);
  MOVETO((char *)s__Show_Graph_10a8_1226 + 10,0x128e,0x1c2);
  uVar7 = param_3;
  LINETO((char *)s__Show_Graph_10a8_1226 + 10,1000,0x1c2);
  uVar16 = param_3;
  LINETO((char *)s__Show_Graph_10a8_1226 + 10,1000,0x1194);
  uVar6 = param_3;
  SETBKMODE((char *)s__Show_Graph_10a8_1226 + 10,1);
  FUN_1028_01a9(100,0,0x12f2,0,500,0,0x96,0,param_3);
  FUN_10a0_1458(uVar6,uVar16,uVar7);
  iVar5 = 0xca0;
  FUN_10a0_144a();
  local_17c = FUN_10a0_145c();
  iVar4 = *(int *)(iVar9 + 0x42) >> 0xf;
  pcVar11 = (char *)s_RandomShrs_10a8_109f + 1;
  local_17a = iVar5;
  local_17c = FUN_10a0_0be7();
  uVar1 = *(uint *)(iVar9 + 0x42);
  local_1a2[0] = (int)uVar1 >> 0xf;
  local_17a = iVar5;
  if ((-1 < (int)local_1a2[0]) && ((0 < (int)local_1a2[0] || (uVar1 != 0)))) {
    local_174 = 1;
    local_172 = 0;
    while( true ) {
      uVar6 = param_3;
      uVar16 = param_3;
      uVar13 = GETNEARESTCOLOR(pcVar11,*(undefined2 *)(local_174 * 4 + 0x868),
                               *(undefined2 *)(local_174 * 4 + 0x86a));
      SETTEXTCOLOR((char *)s__Show_Graph_10a8_1226 + 10,uVar13);
      FUN_10a0_0baa(uVar6,uVar16);
      FUN_10a0_0baa();
      FUN_10a0_1458();
      FUN_10a0_1432();
      local_16c = FUN_10a0_145c();
      iVar4 = local_17a;
      FUN_10a0_0baa();
      FUN_10a0_0baa();
      FUN_10a0_1458();
      FUN_10a0_1432();
      local_168 = FUN_10a0_145c();
      local_166 = (char *)0x96;
      local_16a = 0x1c2;
      local_a4[0] = 0;
      FUN_1098_00bd(0x9a4,(char *)s_RandomShrs_10a8_109f + 9,local_a4,unaff_SS);
      FUN_1098_00bd(iVar9 + local_174 * 0xfc + -0xb4,uVar10,local_a4,unaff_SS);
      pcVar11 = (char *)s__Show_Graph_10a8_1226 + 10;
      DRAWTEXT((char *)s_RandomDivs_10a8_1094 + 4,1,&local_16c,unaff_SS,0xffff,local_a4,unaff_SS);
      if ((local_172 == local_1a2[0]) && (local_174 == uVar1)) break;
      bVar12 = 0xfffe < local_174;
      local_174 = local_174 + 1;
      local_172 = local_172 + bVar12;
    }
  }
  uVar6 = 0;
  uVar16 = param_3;
  SETTEXTCOLOR(pcVar11,0,0);
  if (local_170 != 0 || local_16e != 0) {
    local_16a = 0x128e;
    local_16c = 0x32;
    local_166 = (char *)s_RandomDivs_10a8_1094 + 6;
    local_168 = 0x1c2;
    if (DAT_10a8_1d27 == '\0') {
      pcVar11 = (char *)s_RandomShrs_10a8_109f + 1;
      FUN_10a0_0a72(0x96,local_a4,unaff_SS,0,local_170,local_16e);
    }
    else {
      local_a4[0] = 0;
      pcVar11 = (char *)s_RandomDivs_10a8_1094 + 4;
      FUN_1098_00bd(DAT_10a8_099a,DAT_10a8_099c,local_a4,unaff_SS);
    }
    uVar6 = param_3;
    DRAWTEXT(pcVar11,0,&local_16c,unaff_SS,0xffff,local_a4,unaff_SS);
    local_17d = '\x01';
    while( true ) {
      local_178 = 0;
      local_176 = 0;
      if (DAT_10a8_1d27 != '\0') {
        local_170 = 0;
        local_16e = 0;
        uVar1 = *(uint *)(iVar9 + 0x42);
        local_1a2[0] = (int)uVar1 >> 0xf;
        if ((local_1a2[0] < 0x8000) && ((0 < (int)local_1a2[0] || (uVar1 != 0)))) {
          local_174 = 1;
          local_172 = 0;
          while( true ) {
            iVar5 = iVar9 + local_174 * 0xfc + local_17d * 4;
            uVar8 = *(uint *)(iVar5 + 0x24);
            bVar12 = CARRY2(uVar8,local_170);
            local_170 = uVar8 + local_170;
            local_16e = *(int *)(iVar5 + 0x26) + local_16e + (uint)bVar12;
            if ((local_172 == local_1a2[0]) && (local_174 == uVar1)) break;
            bVar12 = 0xfffe < local_174;
            local_174 = local_174 + 1;
            local_172 = local_172 + bVar12;
          }
        }
      }
      local_166 = (char *)0x64;
      local_16a = 0x3b6;
      FUN_10a0_1458(uVar6);
      FUN_10a0_1444();
      FUN_10a0_1432();
      local_16c = FUN_10a0_145c();
      FUN_10a0_1458();
      FUN_10a0_1432();
      FUN_10a0_1438();
      local_168 = FUN_10a0_145c();
      DRAWTEXT((char *)s_RandomShrs_10a8_109f + 1,1,&local_16c,unaff_SS,0xffff,
               *(undefined2 *)(local_17d * 4 + 0x880),*(undefined2 *)(local_17d * 4 + 0x882));
      uVar1 = *(uint *)(iVar9 + 0x42);
      local_1a2[0] = (int)uVar1 >> 0xf;
      if ((-1 < (int)local_1a2[0]) && ((0 < (int)local_1a2[0] || (uVar1 != 0)))) {
        local_174 = 1;
        local_172 = 0;
        while( true ) {
          if (local_170 != 0 || local_16e != 0) {
            uVar15 = param_3;
            uVar13 = GETNEARESTCOLOR((char *)s__Show_Graph_10a8_1226 + 10,
                                     *(undefined2 *)(local_174 * 4 + 0x868),
                                     *(undefined2 *)(local_174 * 4 + 0x86a));
            uVar14 = (undefined2)((ulong)uVar13 >> 0x10);
            local_1a2[1] = CREATESOLIDBRUSH((char *)s__Show_Graph_10a8_1226 + 10,(int)uVar13);
            uVar7 = param_3;
            SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_1a2[1]);
            uVar8 = local_16e;
            uVar7 = FUN_10a0_1458(uVar7,uVar14,uVar15);
            FUN_10a0_1458(uVar7,iVar4,uVar8);
            FUN_10a0_1444();
            FUN_10a0_1444();
            FUN_10a0_144a();
            FUN_10a0_1432();
            local_166 = (char *)FUN_10a0_145c();
            uVar8 = local_16e;
            uVar7 = FUN_10a0_1458();
            FUN_10a0_1458(uVar7,iVar4,uVar8);
            FUN_10a0_144a();
            FUN_10a0_1444();
            FUN_10a0_1432();
            local_16a = FUN_10a0_145c();
            FUN_10a0_1458();
            FUN_10a0_1444();
            FUN_10a0_1432();
            local_16c = FUN_10a0_145c();
            FUN_10a0_1458();
            FUN_10a0_1432();
            FUN_10a0_1438();
            local_168 = FUN_10a0_145c();
            RECTANGLE((char *)s_RandomShrs_10a8_109f + 1,local_166,local_168,local_16a,local_16c);
            uVar7 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,5,param_3);
            SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar7);
            DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_1a2[1]);
            iVar5 = iVar9 + local_174 * 0xfc + local_17d * 4;
            uVar8 = *(uint *)(iVar5 + 0x24);
            bVar12 = CARRY2(uVar8,local_178);
            local_178 = uVar8 + local_178;
            local_176 = *(int *)(iVar5 + 0x26) + local_176 + (uint)bVar12;
          }
          if ((local_172 == local_1a2[0]) && (local_174 == uVar1)) break;
          bVar12 = 0xfffe < local_174;
          local_174 = local_174 + 1;
          local_172 = local_172 + bVar12;
        }
      }
      if (local_17d == '\b') break;
      local_17d = local_17d + '\x01';
    }
  }
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,0xb);
  DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar9 + 0x46),param_3,uVar6,
               uVar16);
  return;
}

/* ======================================================== */
/* Function: FUN_1028_0ddf @ 1028:0ddf */
/* ======================================================== */

void __cdecl16far FUN_1028_0ddf(void)

{
  DAT_10a8_1d26 = 1;
  DAT_10a8_1d27 = 0;
  DAT_10a8_1d28._0_2_ = 0;
  DAT_10a8_1d28._2_2_ = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1030_0002 @ 1030:0002 */
/* ======================================================== */

void __stdcall16far
FUN_1030_0002(undefined1 param_1,undefined2 param_2,undefined2 param_3,undefined1 *param_4,
             undefined1 *param_5,undefined2 param_6,undefined2 param_7)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 unaff_SS;
  int local_e44 [19];
  undefined2 local_e1e;
  undefined2 local_e1c;
  undefined1 local_e16 [1512];
  undefined1 local_82e [288];
  undefined1 local_70e;
  undefined1 local_70c [288];
  undefined1 local_5ec [1512];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = local_5ec;
  puVar4 = (undefined1 *)param_5;
  for (iVar3 = 0x5e8; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  puVar5 = local_70c;
  puVar4 = (undefined1 *)param_4;
  for (iVar3 = 0x120; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  local_e1e = param_3;
  local_e1c = param_2;
  local_70e = param_1;
  FUN_10a0_0b92(0x120,local_82e,unaff_SS,local_70c,unaff_SS);
  FUN_10a0_0b92(0x5e8,local_e16,unaff_SS,local_5ec,unaff_SS);
  FUN_1080_0002(local_e44,unaff_SS,0xa84,0xad8,(char *)s_RandomShrs_10a8_109f + 9,param_6,param_7);
  (*(code *)*(undefined2 *)(local_e44[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_e44);
  (*(code *)*(undefined2 *)(local_e44[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_e44);
  return;
}

/* ======================================================== */
/* Function: FUN_1030_00a5 @ 1030:00a5 */
/* ======================================================== */

void __stdcall16far FUN_1030_00a5(undefined1 *param_1,undefined2 param_2,undefined2 param_3)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 unaff_SS;
  int local_70 [19];
  undefined2 local_4a;
  undefined2 local_48;
  undefined2 local_46;
  undefined2 local_44;
  undefined2 local_42;
  undefined2 local_40;
  undefined2 local_3e;
  undefined2 local_3c;
  undefined2 local_3a;
  undefined2 local_38;
  undefined2 local_36;
  undefined2 local_34;
  undefined1 local_32;
  undefined1 local_31;
  undefined1 local_30;
  undefined1 local_2f;
  undefined1 local_2e;
  undefined1 local_2d;
  undefined1 local_2c;
  undefined1 local_2a [4];
  undefined2 local_26;
  undefined2 local_24;
  undefined2 local_22;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  undefined2 local_1a;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_12;
  undefined2 local_10;
  undefined1 local_e;
  undefined1 local_d;
  undefined1 local_b;
  undefined1 local_a;
  undefined1 local_9;
  undefined1 local_8;
  undefined1 local_7;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = local_2a;
  puVar4 = (undefined1 *)param_1;
  for (iVar3 = 0x26; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  local_4a = local_26;
  local_48 = local_24;
  local_46 = local_22;
  local_44 = local_20;
  local_42 = local_1e;
  local_40 = local_1c;
  local_3a = local_16;
  local_38 = local_14;
  local_3e = local_1a;
  local_3c = local_18;
  local_32 = local_e;
  local_31 = local_d;
  local_30 = local_b;
  local_2f = local_a;
  local_2e = local_9;
  local_2d = local_8;
  local_36 = local_12;
  local_34 = local_10;
  local_2c = local_7;
  FUN_1080_0002(local_70,unaff_SS,0x9a8,0xae4,(char *)s_RandomShrs_10a8_109f + 9,param_2,param_3);
  (*(code *)*(undefined2 *)(local_70[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_70);
  (*(code *)*(undefined2 *)(local_70[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_70);
  return;
}

/* ======================================================== */
/* Function: FUN_1030_016c @ 1030:016c */
/* ======================================================== */

void __stdcall16far
FUN_1030_016c(undefined2 param_1,undefined2 param_2,undefined2 param_3,int *param_4)

{
  int unaff_BP;
  int *piVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined4 uVar3;
  undefined1 local_26 [2];
  undefined1 local_24 [32];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar2 = (undefined2)((ulong)param_4 >> 0x10);
  piVar1 = (int *)param_4;
  if ((piVar1[1] < 1) && (piVar1[1] < 0)) {
    if ((piVar1[1] == -1) && (*param_4 == -1)) {
      GETDLGITEMTEXT(unaff_CS,10,local_24,unaff_SS,param_3);
      uVar3 = FUN_10a0_0abf(local_26,unaff_SS,local_24,unaff_SS);
      *param_4 = (int)uVar3;
      piVar1[1] = (int)((ulong)uVar3 >> 0x10);
    }
  }
  else {
    FUN_10a0_0a72(0x1e,local_24,unaff_SS,0,*param_4,piVar1[1]);
    SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_24,unaff_SS,param_3);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1030_01fa @ 1030:01fa */
/* ======================================================== */

void __stdcall16far FUN_1030_01fa(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  char *pcVar3;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  SETDLGITEMINT(unaff_CS,0,*(undefined2 *)(iVar1 + 0x26),0x65);
  SETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,*(undefined2 *)(iVar1 + 0x2a),0x69);
  SETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,*(undefined2 *)(iVar1 + 0x2e),0x67);
  SETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,*(undefined2 *)(iVar1 + 0x32),0x6b);
  if ((*(int *)(iVar1 + 0x38) < 1) &&
     ((*(int *)(iVar1 + 0x38) < 0 || (*(int *)(iVar1 + 0x36) == 0)))) {
    pcVar3 = (char *)s__Show_Graph_10a8_1226 + 10;
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xaf4,(char *)s_RandomShrs_10a8_109f + 9,
                   0x6c);
  }
  else {
    pcVar3 = (char *)s_NumChits_10a8_102d + 3;
    FUN_1030_016c(iVar1,uVar2,0x6c,iVar1 + 0x36,uVar2);
  }
  if (*(char *)(iVar1 + 0x3e) == '\0') {
    SETDLGITEMTEXT(pcVar3,0xafd,(char *)s_RandomShrs_10a8_109f + 9,0x66);
  }
  else {
    SETDLGITEMTEXT(pcVar3,0xaf9,(char *)s_RandomShrs_10a8_109f + 9,0x66);
  }
  if (*(char *)(iVar1 + 0x3f) == '\0') {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xafd,(char *)s_RandomShrs_10a8_109f + 9,
                   0x68);
  }
  else {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xaf9,(char *)s_RandomShrs_10a8_109f + 9,
                   0x68);
  }
  if (*(char *)(iVar1 + 0x40) == '\0') {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xafd,(char *)s_RandomShrs_10a8_109f + 9,
                   0x6a);
  }
  else {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xaf9,(char *)s_RandomShrs_10a8_109f + 9,
                   0x6a);
  }
  if (*(char *)(iVar1 + 0x41) == '\0') {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xafd,(char *)s_RandomShrs_10a8_109f + 9,
                   0x6d);
  }
  else {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xaf9,(char *)s_RandomShrs_10a8_109f + 9,
                   0x6d);
  }
  if (*(char *)(iVar1 + 0x42) == '\0') {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xafd,(char *)s_RandomShrs_10a8_109f + 9,
                   0x6e);
  }
  else {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xaf9,(char *)s_RandomShrs_10a8_109f + 9,
                   0x6e);
  }
  if (*(char *)(iVar1 + 0x44) == '\0') {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xafd,(char *)s_RandomShrs_10a8_109f + 9,
                   0x82);
  }
  else {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xaf9,(char *)s_RandomShrs_10a8_109f + 9,
                   0x82);
  }
  if (*(char *)(iVar1 + 0x43) == '\0') {
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xaf4,(char *)s_RandomShrs_10a8_109f + 9,300
                  );
  }
  else {
    FUN_1030_016c(iVar1,uVar2,300,iVar1 + 0x3a,uVar2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1030_03e2 @ 1030:03e2 */
/* ======================================================== */

void __stdcall16far FUN_1030_03e2(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int unaff_BP;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  int local_2a;
  int local_28;
  byte local_25;
  char local_24 [32];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  GETDLGITEMTEXT(unaff_CS,6,local_24,unaff_SS,0x65);
  local_28 = 0;
  do {
    local_25 = local_24[local_28];
    local_28 = local_28 + 1;
    if (local_25 != ' ') break;
  } while (local_28 < 4);
  if (local_28 < 4) {
    local_25 = FUN_10a0_1e71(CONCAT11((char)((uint)local_28 >> 8),local_25));
    if (local_24[local_28] == ',') {
      local_24[local_28] = ' ';
    }
    if (-1 < local_28 + -1) {
      for (local_2a = 0; local_24[local_2a] = ' ', local_2a != local_28 + -1;
          local_2a = local_2a + 1) {
      }
    }
    iVar1 = FUN_10a0_0abf(&local_28,unaff_SS,local_24,unaff_SS);
    if ((local_25 < 0x41) || (*(byte *)(iVar2 + 0x28) < local_25)) {
      MESSAGEBOX((char *)s_RandomShrs_10a8_109f + 1,0x30,0xb1a,(char *)s_RandomShrs_10a8_109f + 9,
                 0xb00,(char *)s_RandomShrs_10a8_109f + 9);
    }
    else if ((iVar1 < 1) || (*(int *)(iVar2 + 0x2b) < iVar1)) {
      MESSAGEBOX((char *)s_RandomShrs_10a8_109f + 1,0x30,0xb1a,(char *)s_RandomShrs_10a8_109f + 9,
                 0xb29,(char *)s_RandomShrs_10a8_109f + 9);
    }
    else {
      *(int *)(iVar2 + 0x29) = iVar1;
      *(byte *)(iVar2 + 0x27) = local_25;
      if (*(char *)(iVar2 + *(char *)(iVar2 + 0x27) * 0x1a + *(int *)(iVar2 + 0x29) + -0x3c6) ==
          '\x01') {
        *(undefined1 *)(iVar2 + 0x26) = 0;
        FUN_1080_0382(iVar2,uVar3,(int)param_2,(int)((ulong)param_2 >> 0x10));
      }
      else {
        MESSAGEBOX((char *)s_RandomShrs_10a8_109f + 1,0x30,0xb1a,(char *)s_RandomShrs_10a8_109f + 9,
                   0xb3d,(char *)s_RandomShrs_10a8_109f + 9);
      }
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1030_0536 @ 1030:0536 */
/* ======================================================== */

void __stdcall16far FUN_1030_0536(void)

{
  FUN_10a0_1e88();
  return;
}

/* ======================================================== */
/* Function: FUN_1030_0557 @ 1030:0557 */
/* ======================================================== */

void __stdcall16far FUN_1030_0557(void)

{
  FUN_10a0_1e88();
  return;
}

/* ======================================================== */
/* Function: FUN_1030_0578 @ 1030:0578 */
/* ======================================================== */

void __stdcall16far FUN_1030_0578(void)

{
  FUN_10a0_1e88();
  return;
}

/* ======================================================== */
/* Function: FUN_1030_0599 @ 1030:0599 */
/* ======================================================== */

void __stdcall16far FUN_1030_0599(undefined4 param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int unaff_BP;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  char *pcVar8;
  undefined2 unaff_SS;
  undefined2 uVar9;
  undefined2 uVar10;
  uint local_18;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  iVar5 = (int)param_1;
  FUN_1078_0d9b(iVar5,uVar7);
  uVar10 = *(undefined2 *)((int)*(undefined4 *)(iVar5 + 6) + 4);
  GETWINDOWRECT((char *)s_Blanks_10a8_1072 + 6,&local_c,unaff_SS);
  uVar9 = *(undefined2 *)(iVar5 + 4);
  GETWINDOWRECT((char *)s__Show_Graph_10a8_1226 + 10,&local_14,unaff_SS);
  *(int *)(iVar5 + 0x2a) = local_10 - local_14;
  *(int *)(iVar5 + 0x2c) = local_e - local_12;
  iVar4 = *(int *)(iVar5 + 0x2c) >> 0xf;
  FUN_10a0_1458(uVar9,uVar10);
  FUN_10a0_1444();
  iVar2 = FUN_10a0_145c();
  FUN_10a0_0baa(iVar2,iVar4);
  iVar4 = FUN_10a0_0be7();
  *(int *)(iVar5 + 0x2c) = iVar4 + iVar2;
  local_18 = GETSYSTEMMETRICS((char *)s_RandomShrs_10a8_109f + 1,0x20);
  iVar2 = GETSYSTEMMETRICS((char *)s__Show_Graph_10a8_1226 + 10,4);
  iVar4 = GETSYSTEMMETRICS((char *)s__Show_Graph_10a8_1226 + 10,0xf);
  iVar3 = GETSYSTEMMETRICS((char *)s__Show_Graph_10a8_1226 + 10,0x21);
  iVar6 = *(char *)(iVar5 + 0x736) * 4;
  pcVar8 = (char *)s__Show_Graph_10a8_1226 + 10;
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,
                 *(undefined2 *)((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 + iVar6),
                 *(undefined2 *)((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + iVar6),0x6b
                );
  uVar1 = DAT_10a8_22aa;
  if (0 < (int)DAT_10a8_22aa) {
    local_18 = 1;
    while( true ) {
      SETDLGITEMTEXT(pcVar8,iVar5 + local_18 * 0xfc + -0xce,uVar7,local_18 + 0x77);
      pcVar8 = (char *)s_NumChits_10a8_102d + 3;
      FUN_1030_0790(iVar5,uVar7,local_18 + 0x81,
                    iVar5 + local_18 * 0xfc + *(char *)(iVar5 + 0x736) * 4 + 10,uVar7);
      if (local_18 == uVar1) break;
      local_18 = local_18 + 1;
    }
  }
  if (((DAT_10a8_22ac < 0) || ((DAT_10a8_22ac < 1 && (DAT_10a8_22aa < 6)))) &&
     (uVar1 = DAT_10a8_22aa + 1, (int)(DAT_10a8_22aa + 1) < 7)) {
    while( true ) {
      local_18 = uVar1;
      SETDLGITEMTEXT(pcVar8,0xb52,(char *)s_RandomShrs_10a8_109f + 9,local_18 + 0x77);
      pcVar8 = (char *)s__Show_Graph_10a8_1226 + 10;
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0xb52,(char *)s_RandomShrs_10a8_109f + 9,
                     local_18 + 0x81);
      if (local_18 == 6) break;
      uVar1 = local_18 + 1;
    }
  }
  MOVEWINDOW(pcVar8,0,*(undefined2 *)(iVar5 + 0x2c),*(undefined2 *)(iVar5 + 0x2a),
             local_a + *(int *)(iVar5 + 0x28) + iVar3 + iVar4 + iVar2 + -3,
             local_c + *(int *)(iVar5 + 0x26) + local_18 + -3);
  return;
}

/* ======================================================== */
/* Function: FUN_1030_0790 @ 1030:0790 */
/* ======================================================== */

void __stdcall16far
FUN_1030_0790(undefined2 param_1,undefined2 param_2,undefined2 param_3,int *param_4)

{
  int unaff_BP;
  int *piVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined4 uVar3;
  undefined1 local_26 [2];
  undefined1 local_24 [32];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar2 = (undefined2)((ulong)param_4 >> 0x10);
  piVar1 = (int *)param_4;
  if ((piVar1[1] < 1) && (piVar1[1] < 0)) {
    if ((piVar1[1] == -1) && (*param_4 == -1)) {
      GETDLGITEMTEXT(unaff_CS,10,local_24,unaff_SS,param_3);
      uVar3 = FUN_10a0_0abf(local_26,unaff_SS,local_24,unaff_SS);
      *param_4 = (int)uVar3;
      piVar1[1] = (int)((ulong)uVar3 >> 0x10);
    }
  }
  else {
    FUN_10a0_0a72(0x1e,local_24,unaff_SS,0,*param_4,piVar1[1]);
    SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_24,unaff_SS,param_3);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1030_081e @ 1030:081e */
/* ======================================================== */

void __stdcall16far FUN_1030_081e(undefined4 param_1,undefined4 param_2)

{
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  undefined2 uVar4;
  undefined2 uVar5;
  int local_14;
  int local_12;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  uVar5 = *(undefined2 *)(iVar1 + 4);
  SETCAPTURE();
  uVar4 = *(undefined2 *)(iVar1 + 4);
  GETWINDOWRECT((char *)s__Show_Graph_10a8_1226 + 10,&local_14,unaff_SS);
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  *(int *)(iVar1 + 0x26) = local_14 + *(int *)((int)param_2 + 6);
  *(int *)(iVar1 + 0x28) = local_12 + *(int *)((int)param_2 + 8);
  GETSYSTEMMETRICS((char *)s__Show_Graph_10a8_1226 + 10,0x20,uVar4,uVar5);
  GETSYSTEMMETRICS((char *)s__Show_Graph_10a8_1226 + 10,4);
  GETSYSTEMMETRICS((char *)s__Show_Graph_10a8_1226 + 10,0xf);
  GETSYSTEMMETRICS((char *)s__Show_Graph_10a8_1226 + 10,0x21);
  MOVEWINDOW((char *)s__Show_Graph_10a8_1226 + 10,1,*(int *)(iVar1 + 0x2c) + -3,
             *(int *)(iVar1 + 0x2a) + -3,*(undefined2 *)(iVar1 + 0x28),*(undefined2 *)(iVar1 + 0x26)
            );
  return;
}

/* ======================================================== */
/* Function: FUN_1030_08c0 @ 1030:08c0 */
/* ======================================================== */

void __stdcall16far FUN_1030_08c0(undefined4 param_1,undefined4 param_2)

{
  RELEASECAPTURE();
  FUN_1080_0382((int)param_1,(int)((ulong)param_1 >> 0x10),(int)param_2,
                (int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1038_0002 @ 1038:0002 */
/* ======================================================== */

void __stdcall16far FUN_1038_0002(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  char *unaff_CS;
  int local_6;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  iVar1 = *(int *)(iVar2 + 0x26);
  if (0 < iVar1) {
    local_6 = 1;
    while( true ) {
      SETDLGITEMTEXT(unaff_CS,iVar2 + local_6 * 0xfc + 0x16,uVar3,local_6 + 100);
      SETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,
                    *(undefined2 *)(iVar2 + local_6 * 2 + 0x26),local_6 + 0x6d);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,iVar2 + local_6 * 0x1f + 0x15,uVar3,
                     local_6 + 0x77);
      if (local_6 == iVar1) break;
      local_6 = local_6 + 1;
    }
  }
  local_6 = *(int *)(iVar2 + 0x26) + 1;
  if (local_6 < 7) {
    while( true ) {
      SETDLGITEMTEXT(unaff_CS,0,0,local_6 + 100);
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0,0,local_6 + 0x6d);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0,0,local_6 + 0x77);
      if (local_6 == 6) break;
      local_6 = local_6 + 1;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1040_0002 @ 1040:0002 */
/* ======================================================== */

void __stdcall16far FUN_1040_0002(undefined2 param_1,undefined2 param_2)

{
  int unaff_BP;
  undefined2 unaff_SS;
  int local_2a [19];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_1080_0002(local_2a,unaff_SS,0x1bd2,0xc78,(char *)s_RandomShrs_10a8_109f + 9,param_1,param_2);
  (*(code *)*(undefined2 *)(local_2a[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_2a);
  (*(code *)*(undefined2 *)(local_2a[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_2a);
  return;
}

/* ======================================================== */
/* Function: FUN_1040_0043 @ 1040:0043 */
/* ======================================================== */

void __stdcall16far
FUN_1040_0043(undefined1 param_1,undefined1 *param_2,undefined2 param_3,undefined2 param_4,
             undefined1 *param_5,undefined2 param_6,undefined2 param_7)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 unaff_SS;
  int local_e40 [19];
  undefined2 local_e1a;
  undefined2 local_e18;
  undefined1 local_e16;
  undefined1 local_e15 [1512];
  undefined1 local_82d [289];
  undefined1 local_70c [288];
  undefined1 local_5ec [1512];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = local_5ec;
  puVar4 = (undefined1 *)param_5;
  for (iVar3 = 0x5e8; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  puVar5 = local_70c;
  puVar4 = (undefined1 *)param_2;
  for (iVar3 = 0x120; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  DAT_10a8_1d2c = 1;
  local_e1a = param_3;
  local_e18 = param_4;
  FUN_10a0_0b92(0x5e8,local_e15,unaff_SS,local_5ec,unaff_SS);
  FUN_10a0_0b92(0x120,local_82d,unaff_SS,local_70c,unaff_SS);
  local_e16 = param_1;
  FUN_1080_0002(local_e40,unaff_SS,0xbbc,0xc86,(char *)s_RandomShrs_10a8_109f + 9,param_6,param_7);
  (*(code *)*(undefined2 *)(local_e40[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_e40);
  (*(code *)*(undefined2 *)(local_e40[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_e40);
  DAT_10a8_1d2c = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1040_00f0 @ 1040:00f0 */
/* ======================================================== */

void __stdcall16far
FUN_1040_00f0(undefined1 param_1,undefined1 *param_2,undefined2 param_3,undefined2 param_4)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 unaff_SS;
  int local_276 [19];
  undefined1 local_250 [288];
  undefined1 local_130;
  undefined1 local_124 [288];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = local_124;
  puVar4 = (undefined1 *)param_2;
  for (iVar3 = 0x120; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  DAT_10a8_1d2d = 1;
  FUN_10a0_0b92(0x120,local_250,unaff_SS,local_124,unaff_SS);
  local_130 = param_1;
  FUN_1080_0002(local_276,unaff_SS,0xc24,0xc92,(char *)s_RandomShrs_10a8_109f + 9,param_3,param_4);
  (*(code *)*(undefined2 *)(local_276[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_276);
  (*(code *)*(undefined2 *)(local_276[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_276);
  DAT_10a8_1d2d = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1040_016f @ 1040:016f */
/* ======================================================== */

void __stdcall16far
FUN_1040_016f(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 *param_4)

{
  int iVar1;
  int unaff_BP;
  undefined2 uVar2;
  undefined2 unaff_SS;
  undefined1 local_24 [32];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar2 = (undefined2)((ulong)param_4 >> 0x10);
  iVar1 = ((undefined2 *)param_4)[1];
  if ((0 < iVar1) || (-1 < iVar1)) {
    FUN_10a0_0a72(0x1e,local_24,unaff_SS,0,*param_4,((undefined2 *)param_4)[1]);
    SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_24,unaff_SS,param_3);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1040_01bc @ 1040:01bc */
/* ======================================================== */

void __stdcall16far
FUN_1040_01bc(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6)

{
  int unaff_BP;
  undefined2 unaff_SS;
  bool bVar1;
  undefined1 local_26 [2];
  undefined1 local_24 [32];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  bVar1 = false;
  FUN_10a0_1454();
  if (bVar1) {
    bVar1 = true;
    FUN_10a0_1454();
    if (bVar1) {
      GETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,10,local_24,unaff_SS,param_3);
      FUN_10a0_0b5a(local_26,unaff_SS,local_24,unaff_SS);
    }
  }
  else {
    FUN_10a0_0af5(0x1e,local_24,unaff_SS,2,8,param_4,param_5,param_6);
    SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_24,unaff_SS,param_3);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1040_0256 @ 1040:0256 */
/* ======================================================== */

void __stdcall16far FUN_1040_0256(undefined4 param_1)

{
  undefined1 extraout_AH;
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  bool bVar3;
  bool bVar4;
  
  bVar3 = false;
  bVar4 = true;
  FUN_10a0_1454((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  iVar1 = (int)param_1;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if (bVar3 || bVar4) {
    MESSAGEBOX((char *)s_RandomShrs_10a8_109f + 1,0x30,0xcd6,(char *)s_RandomShrs_10a8_109f + 9,
               0xca0,(char *)s_RandomShrs_10a8_109f + 9);
  }
  else {
    FUN_1040_00f0(CONCAT11(extraout_AH,*(undefined1 *)(iVar1 + 0x2a)),iVar1 + 0x613,uVar2,iVar1,
                  uVar2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1040_02b8 @ 1040:02b8 */
/* ======================================================== */

void __stdcall16far FUN_1040_02b8(undefined4 param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  int unaff_BP;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  char *pcVar11;
  undefined2 unaff_SS;
  bool bVar12;
  uint local_2e;
  int local_2c;
  undefined1 local_2a [22];
  undefined2 local_14;
  undefined2 local_12;
  undefined2 local_10;
  char local_d;
  int local_c;
  int local_a;
  uint local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar10 = (undefined2)((ulong)param_1 >> 0x10);
  iVar7 = (int)param_1;
  FUN_1078_0d9b(iVar7,uVar10);
  local_8 = 1;
  local_6 = 0;
  pcVar11 = (char *)s_Blanks_10a8_1072 + 6;
  while( true ) {
    SENDDLGITEMMESSAGE(pcVar11,*(undefined2 *)
                                ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 + local_8 * 4
                                ),
                       *(undefined2 *)
                        ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + local_8 * 4),0,
                       0x401,0x65);
    if ((local_6 == 0) && (local_8 == 8)) break;
    bVar12 = 0xfffe < local_8;
    local_8 = local_8 + 1;
    local_6 = local_6 + (uint)bVar12;
    pcVar11 = (char *)s__Show_Graph_10a8_1226 + 10;
  }
  local_8 = 1;
  local_6 = 0;
  while( true ) {
    if ((local_6 < *(int *)(iVar7 + 0x28)) ||
       ((local_6 <= *(int *)(iVar7 + 0x28) && (local_8 <= *(uint *)(iVar7 + 0x26))))) {
      uVar6 = *(undefined2 *)(iVar7 + 4);
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,iVar7 + local_8 * 0xfc + -0xd1,uVar10,
                     local_8 + 0x65);
      local_2a[0] = 0;
      iVar8 = iVar7 + local_8 * 0xfc;
      FUN_10a0_0af5(0x14,local_2a,unaff_SS,2,7,*(undefined2 *)(iVar8 + -0xb1),
                    *(undefined2 *)(iVar8 + -0xaf),*(undefined2 *)(iVar8 + -0xad));
      uVar5 = *(undefined2 *)(iVar7 + 4);
      SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_2a,unaff_SS,local_8 + 0x81);
      iVar8 = iVar7 + local_8 * 0xfc;
      local_14 = *(undefined2 *)(iVar8 + -0xb1);
      local_d = '\x01';
      local_12 = *(undefined2 *)(iVar8 + -0xaf);
      local_10 = *(undefined2 *)(iVar8 + -0xad);
      while( true ) {
        iVar8 = iVar7 + local_d * 0x24;
        uVar3 = *(undefined2 *)(iVar8 + 0x5f7);
        uVar4 = *(undefined2 *)(iVar7 + local_8 * 0xfc + local_d * 4 + 9);
        FUN_10a0_1458(*(undefined2 *)(iVar8 + 0x5f5),uVar3,*(undefined2 *)(iVar8 + 0x5f9),uVar5,
                      uVar6);
        FUN_10a0_1444();
        local_14 = FUN_10a0_1432();
        local_12 = uVar3;
        local_10 = uVar4;
        if (local_d == '\b') break;
        local_d = local_d + '\x01';
      }
      FUN_10a0_0af5(0x14,local_2a,unaff_SS,2,7,local_14,uVar3,uVar4);
      SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_2a,unaff_SS,local_8 + 0x8b);
    }
    else {
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0,0,local_8 + 0x65);
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0,0,local_8 + 0x77);
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0,0,local_8 + 0x8b);
    }
    if ((local_6 == 0) && (local_8 == 6)) break;
    bVar12 = 0xfffe < local_8;
    local_8 = local_8 + 1;
    local_6 = local_6 + (uint)bVar12;
  }
  local_2e = 0;
  local_2c = 0;
  uVar1 = *(uint *)(iVar7 + 0x26);
  iVar8 = *(int *)(iVar7 + 0x28);
  if ((-1 < iVar8) && ((0 < iVar8 || (uVar1 != 0)))) {
    local_8 = 1;
    local_6 = 0;
    while( true ) {
      FUN_1040_016f(iVar7,uVar10,local_8 + 0x6d,
                    iVar7 + local_8 * 0xfc + *(char *)(iVar7 + 0x2a) * 4 + 7,uVar10);
      iVar9 = iVar7 + *(char *)(iVar7 + 0x2a) * 0x24;
      uVar6 = *(undefined2 *)(iVar9 + 0x5f7);
      uVar5 = *(undefined2 *)(iVar7 + local_8 * 0xfc + *(char *)(iVar7 + 0x2a) * 4 + 9);
      FUN_10a0_1458(*(undefined2 *)(iVar9 + 0x5f5),uVar6,*(undefined2 *)(iVar9 + 0x5f9));
      uVar3 = FUN_10a0_1444();
      FUN_10a0_0af5(0x14,local_2a,unaff_SS,2,7,uVar3,uVar6,uVar5);
      SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_2a,unaff_SS,local_8 + 0x77);
      iVar9 = iVar7 + local_8 * 0xfc + *(char *)(iVar7 + 0x2a) * 4;
      uVar2 = *(uint *)(iVar9 + 7);
      bVar12 = CARRY2(uVar2,local_2e);
      local_2e = uVar2 + local_2e;
      local_2c = *(int *)(iVar9 + 9) + local_2c + (uint)bVar12;
      if ((local_6 == iVar8) && (local_8 == uVar1)) break;
      bVar12 = 0xfffe < local_8;
      local_8 = local_8 + 1;
      local_6 = local_6 + (uint)bVar12;
    }
  }
  FUN_1040_016f(iVar7,uVar10,0x132,&local_2e,unaff_SS);
  local_c = DAT_10a8_22be - local_2e;
  local_a = (DAT_10a8_22c0 - local_2c) - (uint)(DAT_10a8_22be < local_2e);
  FUN_1040_016f(iVar7,uVar10,0x133,&local_c,unaff_SS);
  SENDDLGITEMMESSAGE(0x1040,0,0,*(char *)(iVar7 + 0x2a) + -1,0x407,0x65);
  if (DAT_10a8_1d2d != '\0') {
    GETDLGITEM((char *)s__Show_Graph_10a8_1226 + 10,0x131);
    SHOWWINDOW((char *)s__Show_Graph_10a8_1226 + 10,0);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1040_06af @ 1040:06af */
/* ======================================================== */

void __stdcall16far FUN_1040_06af(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  int unaff_BP;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  bool bVar10;
  undefined2 uVar11;
  undefined1 local_2e [22];
  int local_18;
  int local_16;
  uint local_14;
  int local_12;
  uint local_c;
  int local_a;
  long local_8;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  if (*(int *)((int)param_2 + 8) == 1) {
    uVar9 = (undefined2)((ulong)param_1 >> 0x10);
    iVar7 = (int)param_1;
    uVar11 = *(undefined2 *)(iVar7 + 4);
    local_8 = SENDDLGITEMMESSAGE(unaff_CS,0,0,0,0x409,0x65);
    local_8 = local_8 + 1;
    local_14 = 0;
    local_12 = 0;
    uVar1 = *(uint *)(iVar7 + 0x26);
    iVar2 = *(int *)(iVar7 + 0x28);
    if ((-1 < iVar2) && ((0 < iVar2 || (uVar1 != 0)))) {
      local_c = 1;
      local_a = 0;
      while( true ) {
        FUN_1040_016f(iVar7,uVar9,local_c + 0x6d,iVar7 + local_c * 0xfc + (char)local_8 * 4 + 7,
                      uVar9);
        iVar8 = iVar7 + (char)local_8 * 0x24;
        uVar6 = *(undefined2 *)(iVar8 + 0x5f7);
        uVar5 = *(undefined2 *)(iVar7 + local_c * 0xfc + (char)local_8 * 4 + 9);
        FUN_10a0_1458(*(undefined2 *)(iVar8 + 0x5f5),uVar6,*(undefined2 *)(iVar8 + 0x5f9),uVar11);
        uVar4 = FUN_10a0_1444();
        FUN_10a0_0af5(0x14,local_2e,unaff_SS,2,7,uVar4,uVar6,uVar5);
        SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_2e,unaff_SS,local_c + 0x77);
        iVar8 = iVar7 + local_c * 0xfc + (char)local_8 * 4;
        uVar3 = *(uint *)(iVar8 + 7);
        bVar10 = CARRY2(uVar3,local_14);
        local_14 = uVar3 + local_14;
        local_12 = *(int *)(iVar8 + 9) + local_12 + (uint)bVar10;
        if ((local_a == iVar2) && (local_c == uVar1)) break;
        bVar10 = 0xfffe < local_c;
        local_c = local_c + 1;
        local_a = local_a + (uint)bVar10;
      }
    }
    FUN_1040_016f(iVar7,uVar9,0x132,&local_14,unaff_SS);
    local_18 = DAT_10a8_22be - local_14;
    local_16 = (DAT_10a8_22c0 - local_12) - (uint)(DAT_10a8_22be < local_14);
    FUN_1040_016f(iVar7,uVar9,0x133,&local_18,unaff_SS);
    *(undefined1 *)(iVar7 + 0x2a) = (char)local_8;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1040_0865 @ 1040:0865 */
/* ======================================================== */

void __stdcall16far FUN_1040_0865(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  char *pcVar7;
  bool bVar8;
  bool bVar9;
  int local_8;
  char local_5;
  
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  FUN_1078_0d9b(iVar4,uVar6);
  local_8 = 1;
  local_5 = '\x01';
  while( true ) {
    bVar8 = false;
    bVar9 = true;
    pcVar7 = (char *)s_RandomShrs_10a8_109f + 1;
    FUN_10a0_1454();
    iVar5 = local_8;
    if (!bVar8 && !bVar9) {
      if (*(char *)(iVar4 + 0x146) == '\t') {
        *(char *)(iVar4 + 0x146) = local_5;
      }
      *(char *)(iVar4 + local_8 + 0x146) = local_5;
      pcVar7 = (char *)s__Show_Graph_10a8_1226 + 10;
      SENDDLGITEMMESSAGE((char *)s_RandomShrs_10a8_109f + 1,
                         *(undefined2 *)
                          ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 + local_5 * 4),
                         *(undefined2 *)
                          ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + local_5 * 4),0,
                         0x403,0x65);
      iVar5 = local_8 + 1;
      if (local_5 == *(char *)(iVar4 + 0x146)) {
        pcVar7 = (char *)s__Show_Graph_10a8_1226 + 10;
        SENDDLGITEMMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,0,0,local_8 + -1,0x40e,0x65);
      }
    }
    local_8 = iVar5;
    if (local_5 == '\b') break;
    local_5 = local_5 + '\x01';
  }
  if (DAT_10a8_1d2c != '\0') {
    GETDLGITEM(pcVar7,0x6c);
    SHOWWINDOW((char *)s__Show_Graph_10a8_1226 + 10,0);
  }
  iVar5 = iVar4 + *(char *)(iVar4 + 0x146) * 0x24;
  FUN_1040_01bc(iVar4,uVar6,0x66,*(undefined2 *)(iVar5 + 8),*(undefined2 *)(iVar5 + 10),
                *(undefined2 *)(iVar5 + 0xc));
  iVar5 = iVar4 + *(char *)(iVar4 + 0x146) * 0x24;
  FUN_1040_01bc(iVar4,uVar6,0x67,*(undefined2 *)(iVar5 + 2),*(undefined2 *)(iVar5 + 4),
                *(undefined2 *)(iVar5 + 6));
  iVar5 = iVar4 + *(char *)(iVar4 + 0x146) * 0x24;
  uVar3 = *(undefined2 *)(iVar5 + 4);
  uVar2 = *(undefined2 *)(iVar5 + 6);
  FUN_10a0_144a();
  uVar1 = FUN_10a0_1444();
  FUN_1040_01bc(iVar4,uVar6,0x68,uVar1,uVar3,uVar2);
  iVar5 = iVar4 + *(char *)(iVar4 + 0x146) * 0x24;
  FUN_1040_01bc(iVar4,uVar6,0x69,*(undefined2 *)(iVar5 + 0xe),*(undefined2 *)(iVar5 + 0x10),
                *(undefined2 *)(iVar5 + 0x12));
  iVar5 = iVar4 + *(char *)(iVar4 + 0x146) * 0x24;
  FUN_1040_01bc(iVar4,uVar6,0x6a,*(undefined2 *)(iVar5 + 0x20),*(undefined2 *)(iVar5 + 0x22),
                *(undefined2 *)(iVar5 + 0x24));
  iVar5 = iVar4 + *(char *)(iVar4 + 0x146) * 0x24;
  uVar3 = *(undefined2 *)(iVar5 + 0x22);
  uVar2 = *(undefined2 *)(iVar5 + 0x24);
  FUN_10a0_144a();
  uVar1 = FUN_10a0_1444();
  FUN_1040_01bc(iVar4,uVar6,0x6b,uVar1,uVar3,uVar2);
  return;
}

/* ======================================================== */
/* Function: FUN_1040_0a98 @ 1040:0a98 */
/* ======================================================== */

void __stdcall16far FUN_1040_0a98(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_CS;
  
  if (*(int *)((int)param_2 + 8) == 1) {
    uVar6 = (undefined2)((ulong)param_1 >> 0x10);
    iVar5 = (int)param_1;
    uVar2 = *(undefined2 *)(iVar5 + 4);
    iVar1 = SENDDLGITEMMESSAGE(unaff_CS,0,0,0,0x407,0x65);
    *(undefined1 *)(iVar5 + 0x146) = *(undefined1 *)(iVar5 + iVar1 + 0x147);
    iVar1 = iVar5 + *(char *)(iVar5 + 0x146) * 0x24;
    FUN_1040_01bc(iVar5,uVar6,0x66,*(undefined2 *)(iVar1 + 8),*(undefined2 *)(iVar1 + 10),
                  *(undefined2 *)(iVar1 + 0xc));
    iVar1 = iVar5 + *(char *)(iVar5 + 0x146) * 0x24;
    FUN_1040_01bc(iVar5,uVar6,0x67,*(undefined2 *)(iVar1 + 2),*(undefined2 *)(iVar1 + 4),
                  *(undefined2 *)(iVar1 + 6));
    iVar1 = iVar5 + *(char *)(iVar5 + 0x146) * 0x24;
    uVar4 = *(undefined2 *)(iVar1 + 4);
    uVar3 = *(undefined2 *)(iVar1 + 6);
    FUN_10a0_144a(uVar2);
    uVar2 = FUN_10a0_1444();
    FUN_1040_01bc(iVar5,uVar6,0x68,uVar2,uVar4,uVar3);
    iVar1 = iVar5 + *(char *)(iVar5 + 0x146) * 0x24;
    FUN_1040_01bc(iVar5,uVar6,0x69,*(undefined2 *)(iVar1 + 0xe),*(undefined2 *)(iVar1 + 0x10),
                  *(undefined2 *)(iVar1 + 0x12));
    iVar1 = iVar5 + *(char *)(iVar5 + 0x146) * 0x24;
    FUN_1040_01bc(iVar5,uVar6,0x6a,*(undefined2 *)(iVar1 + 0x20),*(undefined2 *)(iVar1 + 0x22),
                  *(undefined2 *)(iVar1 + 0x24));
    iVar1 = iVar5 + *(char *)(iVar5 + 0x146) * 0x24;
    uVar2 = *(undefined2 *)(iVar1 + 0x22);
    uVar4 = *(undefined2 *)(iVar1 + 0x24);
    FUN_10a0_144a();
    uVar3 = FUN_10a0_1444();
    FUN_1040_01bc(iVar5,uVar6,0x6b,uVar3,uVar2,uVar4);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1040_0c2f @ 1040:0c2f */
/* ======================================================== */

void __stdcall16far FUN_1040_0c2f(undefined4 param_1)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  FUN_1040_0043(*(undefined1 *)((int)param_1 + 0x146),0x3790,(char *)s_RandomShrs_10a8_109f + 9,
                DAT_10a8_22aa,DAT_10a8_22ac,0x31a6,(char *)s_RandomShrs_10a8_109f + 9,(int)param_1,
                uVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1040_0c60 @ 1040:0c60 */
/* ======================================================== */

void __cdecl16far FUN_1040_0c60(void)

{
  DAT_10a8_1d2c = 0;
  DAT_10a8_1d2d = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0002 @ 1078:0002 */
/* ======================================================== */

void __cdecl16near FUN_1078_0002(void)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int in_AX;
  int iVar5;
  int in_BX;
  int *piVar6;
  bool bVar7;
  
  piVar2 = (int *)*(int *)(in_BX + 4);
  if (piVar2 != (int *)0x0) {
    bVar7 = in_AX == piVar2[1];
    while (!bVar7) {
      iVar5 = piVar2[3];
      piVar6 = piVar2 + 4;
      do {
        if (iVar5 == 0) break;
        iVar5 = iVar5 + -1;
        piVar1 = piVar6;
        piVar6 = piVar6 + 1;
        bVar7 = in_AX == *piVar1;
      } while (!bVar7);
      if (bVar7) {
        iVar3 = piVar2[3];
        iVar4 = *(int *)(in_BX + 4);
        *(int *)(iVar4 + 2) = in_AX;
        *(undefined2 *)(iVar4 + 4) = piVar6 + ((iVar3 + -1) * 2 - iVar5);
        return;
      }
      piVar2 = (int *)*piVar2;
      bVar7 = piVar2 == (int *)0x0;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0045 @ 1078:0045 */
/* ======================================================== */

void __stdcall16far FUN_1078_0045(undefined2 param_1,undefined2 param_2)

{
  undefined2 unaff_CS;
  
  SETPROP(unaff_CS,param_2,0x1b5a,(char *)s_RandomShrs_10a8_109f + 9);
  SETPROP((char *)s__Show_Graph_10a8_1226 + 10,param_1,0x1b5e,(char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0071 @ 1078:0071 */
/* ======================================================== */

void __stdcall16far FUN_1078_0071(void)

{
  undefined2 unaff_CS;
  
  REMOVEPROP(unaff_CS,0x1b5a,(char *)s_RandomShrs_10a8_109f + 9);
  REMOVEPROP((char *)s__Show_Graph_10a8_1226 + 10,0x1b5e,(char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0097 @ 1078:0097 */
/* ======================================================== */

undefined4 __stdcall16far FUN_1078_0097(int param_1)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  undefined2 uVar4;
  char *pcVar5;
  
  iVar1 = ISWINDOW();
  iVar3 = iVar1 >> 0xf;
  if (iVar1 != 0) {
    pcVar5 = (char *)GETWINDOWLONG((char *)s__Show_Graph_10a8_1226 + 10,0xfffc);
    uVar4 = (undefined2)((ulong)pcVar5 >> 0x10);
    pcVar2 = (char *)pcVar5;
    if (((*pcVar5 == -0x18) && (-1 - (int)pcVar2 == *(int *)(pcVar2 + 1))) &&
       (*(int *)0x2 == 0x2e5b)) {
      iVar1 = *(int *)(pcVar2 + 3);
      iVar3 = *(int *)(pcVar2 + 5);
    }
    else {
      GETPROP((char *)s__Show_Graph_10a8_1226 + 10,0x1b5a,(char *)s_RandomShrs_10a8_109f + 9);
      iVar3 = param_1;
      iVar1 = GETPROP((char *)s__Show_Graph_10a8_1226 + 10,0x1b5e,(char *)s_RandomShrs_10a8_109f + 9
                     );
    }
  }
  return CONCAT22(iVar3,iVar1);
}

/* ======================================================== */
/* Function: FUN_1078_0103 @ 1078:0103 */
/* ======================================================== */

void __cdecl16near FUN_1078_0103(void)

{
  (*DAT_10a8_1b62)();
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0133 @ 1078:0133 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1078_0133(undefined2 param_1,undefined2 param_2,undefined2 param_3,int param_4,int param_5)

{
  int *in_BX;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  
  in_BX[2] = param_5;
  if (param_4 < 0) {
    unaff_DI = (undefined2 *)(*in_BX + 0xc);
  }
  else {
    FUN_1078_0002();
  }
  if (*(int *)&DAT_10a8_1b64 != 0) {
    FUN_1078_0103();
  }
  (*(code *)*unaff_DI)();
  return CONCAT22(param_3,param_4);
}

/* ======================================================== */
/* Function: FUN_1078_018b @ 1078:018b */
/* ======================================================== */

void __stdcall16far
FUN_1078_018b(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             undefined2 param_5)

{
  undefined2 *puVar1;
  undefined4 uVar2;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 uVar3;
  
  uVar2 = *(undefined4 *)((int)*(undefined4 *)&DAT_10a8_1b56 + 0x12);
  uVar3 = param_5;
  SETWINDOWLONG(unaff_CS,(int)uVar2,(int)((ulong)uVar2 >> 0x10),0xfffc);
  FUN_1078_0045((int)*(undefined4 *)&DAT_10a8_1b56,
                (int)((ulong)*(undefined4 *)&DAT_10a8_1b56 >> 0x10),param_5);
  puVar1 = (undefined2 *)((int)*(undefined4 *)&DAT_10a8_1b56 + 0x12);
  (*(code *)*puVar1)((char *)s_Blanks_10a8_1072 + 6,param_1,param_2,param_3,param_4,param_5,uVar3);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_01dc @ 1078:01dc */
/* ======================================================== */

undefined1 * __stdcall16far FUN_1078_01dc(undefined2 param_1,undefined2 param_2)

{
  undefined1 *puVar1;
  undefined2 *puVar2;
  undefined2 uVar3;
  int iVar4;
  char *unaff_CS;
  undefined2 *puVar5;
  undefined2 uVar6;
  undefined4 local_10;
  
  if (DAT_10a8_1b4e == (undefined1 *)0x0 && DAT_10a8_1b50 == 0) {
    uVar6 = 0;
    uVar3 = GLOBALALLOC(unaff_CS,0x100,0);
    puVar5 = (undefined2 *)GLOBALLOCK((char *)s__Show_Graph_10a8_1226 + 10,uVar3,uVar6);
    uVar6 = (undefined2)((ulong)puVar5 >> 0x10);
    puVar2 = (undefined2 *)puVar5;
    *puVar5 = DAT_10a8_1b4c;
    FUN_10a0_1e39(5,puVar2 + 1,uVar6,0x1b66,(char *)s_RandomShrs_10a8_109f + 9);
    uVar3 = DAT_10a8_1b54;
    *(undefined2 *)((int)puVar2 + 7) = DAT_10a8_1b52;
    *(undefined2 *)((int)puVar2 + 9) = uVar3;
    local_10 = (undefined1 *)CONCAT22(uVar6,(undefined1 *)((int)puVar2 + 0xb));
    do {
      *local_10 = 0xe8;
      iVar4 = (int)((ulong)local_10 >> 0x10);
      *(int *)((undefined1 *)local_10 + 1) = -1 - (int)(undefined1 *)local_10;
      *(undefined1 **)((undefined1 *)local_10 + 3) = DAT_10a8_1b4e;
      *(int *)((undefined1 *)local_10 + 5) = DAT_10a8_1b50;
      DAT_10a8_1b4e = (undefined1 *)local_10;
      local_10._0_2_ = (undefined1 *)local_10 + 7;
      local_10 = (undefined1 *)CONCAT22(iVar4,(undefined1 *)local_10);
      DAT_10a8_1b50 = iVar4;
    } while ((undefined1 *)local_10 != (undefined1 *)0x100);
    unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
    DAT_10a8_1b4c = uVar6;
    PRESTOCHANGOSELECTOR((char *)s_RandomShrs_10a8_109f + 1,uVar6);
  }
  iVar4 = DAT_10a8_1b50;
  puVar1 = DAT_10a8_1b4e;
  uVar3 = ALLOCCSTODSALIAS(unaff_CS,DAT_10a8_1b50);
  DAT_10a8_1b4e = *(undefined1 **)(puVar1 + 3);
  DAT_10a8_1b50 = *(int *)(puVar1 + 5);
  *(undefined2 *)(puVar1 + 3) = param_1;
  *(undefined2 *)(puVar1 + 5) = param_2;
  FREESELECTOR((char *)s__Show_Graph_10a8_1226 + 10,uVar3);
  return (undefined1 *)CONCAT22(iVar4,puVar1);
}

/* ======================================================== */
/* Function: FUN_1078_02fa @ 1078:02fa */
/* ======================================================== */

void __stdcall16far FUN_1078_02fa(int param_1,undefined2 param_2)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int unaff_BP;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  char *pcVar6;
  
  iVar3 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = param_2;
  iVar5 = param_1;
  uVar2 = ALLOCCSTODSALIAS();
  uVar1 = DAT_10a8_1b50;
  *(undefined2 *)(iVar5 + 3) = DAT_10a8_1b4e;
  *(undefined2 *)(iVar5 + 5) = uVar1;
  FREESELECTOR((char *)s__Show_Graph_10a8_1226 + 10,uVar2,uVar4,iVar5,uVar2,pcVar6,iVar3);
  DAT_10a8_1b4e = param_1;
  DAT_10a8_1b50 = param_2;
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0341 @ 1078:0341 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1078_0341(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4)

{
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  bool bVar3;
  undefined4 uVar4;
  
  bVar3 = true;
  FUN_10a0_03ef((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (!bVar3) {
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    iVar1 = (int)param_1;
    FUN_1088_0014(iVar1,uVar2,0);
    *(undefined2 *)(iVar1 + 2) = 0;
    *(undefined2 *)(iVar1 + 4) = 0;
    *(undefined2 *)(iVar1 + 0x17) = 0;
    *(undefined2 *)(iVar1 + 6) = param_3;
    *(undefined2 *)(iVar1 + 8) = param_4;
    if (*(int *)(iVar1 + 6) == 0 && *(int *)(iVar1 + 8) == 0) {
      *(undefined2 *)(iVar1 + 0x19) = 0;
      *(undefined2 *)(iVar1 + 0x1b) = 0;
    }
    else {
      FUN_1078_069a((int)*(undefined4 *)(iVar1 + 6),(int)((ulong)*(undefined4 *)(iVar1 + 6) >> 0x10)
                    ,iVar1,uVar2);
    }
    *(undefined2 *)(iVar1 + 10) = 0;
    *(undefined2 *)(iVar1 + 0xc) = 0;
    *(undefined2 *)(iVar1 + 0xe) = 0;
    *(undefined2 *)(iVar1 + 0x10) = 0;
    uVar4 = FUN_1078_01dc(iVar1,uVar2);
    *(undefined2 *)(iVar1 + 0x12) = (int)uVar4;
    *(undefined2 *)(iVar1 + 0x14) = (int)((ulong)uVar4 >> 0x10);
    *(undefined1 *)(iVar1 + 0x16) = 0;
    FUN_1078_060d(iVar1,uVar2);
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1078_03e9 @ 1078:03e9 */
/* ======================================================== */

void __stdcall16far FUN_1078_03e9(undefined2 param_1,undefined4 param_2)

{
  FUN_1088_002e((int)param_2,(int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1078_03ff @ 1078:03ff */
/* ======================================================== */

void __stdcall16far FUN_1078_03ff(int *param_1)

{
  int *piVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  piVar1 = (int *)param_1;
  (*(code *)*(undefined2 *)(*param_1 + 0x24))();
  FUN_1078_085a(piVar1,uVar2,0x3e9,(char *)s_Blanks_10a8_1072 + 6);
  if (piVar1[3] != 0 || piVar1[4] != 0) {
    FUN_1078_071c((int)*(undefined4 *)(piVar1 + 3),(int)((ulong)*(undefined4 *)(piVar1 + 3) >> 0x10)
                  ,piVar1,uVar2);
  }
  FUN_1078_02fa(piVar1[9],piVar1[10]);
  FUN_1088_0048(piVar1,uVar2,0);
  FUN_10a0_0439();
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0466 @ 1078:0466 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1078_0466(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 uVar2;
  undefined2 unaff_SS;
  
  iVar1 = *(int *)((int)param_2 + 0x17);
  if ((*(int *)(param_1 + -6) < 0) || (iVar1 != *(int *)(param_1 + -6))) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return CONCAT11((char)((uint)iVar1 >> 8),uVar2);
}

/* ======================================================== */
/* Function: FUN_1078_049e @ 1078:049e */
/* ======================================================== */

uint __stdcall16far FUN_1078_049e(undefined2 param_1,int *param_2)

{
  uint uVar1;
  int unaff_BP;
  int *piVar2;
  undefined2 uVar3;
  undefined1 local_58 [82];
  char local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  piVar2 = (int *)param_2;
  uVar1 = FUN_1078_0673(piVar2,uVar3,4);
  if ((char)uVar1 != '\0') {
    uVar1 = (*(code *)*(undefined2 *)(*param_2 + 0x20))((char *)s_Blanks_10a8_1072 + 6,piVar2,uVar3)
    ;
    if ((char)uVar1 == '\0') {
      uVar1 = uVar1 & 0xff00;
      goto LAB_1078_04d1;
    }
  }
  uVar1 = CONCAT11((char)(uVar1 >> 8),1);
LAB_1078_04d1:
  local_6 = (char)uVar1;
  if (local_6 != '\0') {
    uVar1 = ISICONIC((char *)s_Blanks_10a8_1072 + 6,piVar2[2]);
    if (uVar1 != 0) {
      GETWINDOWTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x51,local_58);
      uVar1 = SETWINDOWTEXT((char *)s__Show_Graph_10a8_1226 + 10,local_58);
    }
  }
  uVar1 = uVar1 & 0xff00;
  if (local_6 == '\0') {
    uVar1 = uVar1 + 1;
  }
  return uVar1;
}

/* ======================================================== */
/* Function: FUN_1078_0524 @ 1078:0524 */
/* ======================================================== */

undefined1 __stdcall16far FUN_1078_0524(undefined2 param_1,undefined4 param_2)

{
  char cVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_2 >> 0x10);
  if ((*(int *)((int)param_2 + 0x17) == 0) &&
     (cVar1 = FUN_1078_049e(param_1,(int)param_2,uVar2), cVar1 != '\0')) {
    return 1;
  }
  return 0;
}

/* ======================================================== */
/* Function: FUN_1078_055c @ 1078:055c */
/* ======================================================== */

uint __stdcall16far FUN_1078_055c(undefined4 param_1)

{
  uint uVar1;
  uint uVar2;
  int unaff_BP;
  undefined2 uVar3;
  undefined4 uVar4;
  char local_d;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  local_d = '\0';
  do {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    uVar4 = FUN_1078_080d((int)param_1,uVar3,0x466,(char *)s_Blanks_10a8_1072 + 6);
    uVar2 = (uint)((ulong)uVar4 >> 0x10);
    uVar1 = (uint)uVar4 | uVar2;
    if (uVar1 != 0) {
      uVar1 = FUN_1078_049e(&iStack_2,uVar4);
      local_d = (char)uVar1;
    }
  } while ((local_d == '\0') && (uVar1 = (uint)uVar4 | uVar2, uVar1 != 0));
  if (local_d == '\0') {
    uVar4 = FUN_1078_080d((int)param_1,uVar3,0x524,(char *)s_Blanks_10a8_1072 + 6);
    uVar1 = (uint)uVar4 | (uint)((ulong)uVar4 >> 0x10);
    if (uVar1 == 0) {
      return 1;
    }
  }
  return uVar1 & 0xff00;
}

/* ======================================================== */
/* Function: FUN_1078_05dc @ 1078:05dc */
/* ======================================================== */

undefined2 __stdcall16far FUN_1078_05dc(void)

{
  return 0;
}

/* ======================================================== */
/* Function: FUN_1078_05f3 @ 1078:05f3 */
/* ======================================================== */

void __stdcall16far FUN_1078_05f3(undefined4 param_1)

{
  FUN_1078_0641((int)param_1,(int)((ulong)param_1 >> 0x10),1,1);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_060d @ 1078:060d */
/* ======================================================== */

void __stdcall16far FUN_1078_060d(undefined4 param_1)

{
  FUN_1078_0641((int)param_1,(int)((ulong)param_1 >> 0x10),1,4);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0627 @ 1078:0627 */
/* ======================================================== */

void __stdcall16far FUN_1078_0627(undefined4 param_1)

{
  FUN_1078_0641((int)param_1,(int)((ulong)param_1 >> 0x10),0,4);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0641 @ 1078:0641 */
/* ======================================================== */

void __stdcall16far FUN_1078_0641(undefined4 param_1,char param_2,byte param_3)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = (int)param_1;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if (param_2 == '\0') {
    *(byte *)(iVar1 + 0x16) = ~param_3 & *(byte *)(iVar1 + 0x16);
  }
  else {
    *(byte *)(iVar1 + 0x16) = *(byte *)(iVar1 + 0x16) | param_3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0673 @ 1078:0673 */
/* ======================================================== */

uint __stdcall16far FUN_1078_0673(undefined4 param_1,byte param_2)

{
  uint in_AX;
  uint uVar1;
  
  uVar1 = in_AX & 0xff00;
  if ((*(byte *)((int)param_1 + 0x16) & param_2) == param_2) {
    uVar1 = uVar1 + 1;
  }
  return uVar1;
}

/* ======================================================== */
/* Function: FUN_1078_069a @ 1078:069a */
/* ======================================================== */

void __stdcall16far FUN_1078_069a(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  if ((int)param_2 != 0 || param_2._2_2_ != 0) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    iVar1 = (int)param_1;
    if (*(int *)(iVar1 + 10) == 0 && *(int *)(iVar1 + 0xc) == 0) {
      *(int *)(iVar1 + 10) = (int)param_2;
      *(int *)(iVar1 + 0xc) = param_2._2_2_;
      *(int *)((int)param_2 + 0x19) = (int)param_2;
      *(int *)((int)param_2 + 0x1b) = param_2._2_2_;
    }
    else {
      uVar4 = (undefined2)((ulong)*(undefined4 *)(iVar1 + 10) >> 0x10);
      iVar2 = (int)*(undefined4 *)(iVar1 + 10);
      uVar5 = *(undefined2 *)(iVar2 + 0x1b);
      *(undefined2 *)((int)param_2 + 0x19) = *(undefined2 *)(iVar2 + 0x19);
      *(undefined2 *)((int)param_2 + 0x1b) = uVar5;
      uVar5 = (undefined2)((ulong)*(undefined4 *)(iVar1 + 10) >> 0x10);
      iVar2 = (int)*(undefined4 *)(iVar1 + 10);
      *(int *)(iVar2 + 0x19) = (int)param_2;
      *(int *)(iVar2 + 0x1b) = param_2._2_2_;
      *(int *)(iVar1 + 10) = (int)param_2;
      *(int *)(iVar1 + 0xc) = param_2._2_2_;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_071c @ 1078:071c */
/* ======================================================== */

void __stdcall16far FUN_1078_071c(undefined4 param_1,int param_2,int param_3)

{
  int *piVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  int local_c;
  int local_a;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  if (*(int *)(iVar2 + 10) != 0 || *(int *)(iVar2 + 0xc) != 0) {
    local_c = *(int *)(iVar2 + 10);
    local_a = *(int *)(iVar2 + 0xc);
    while (((*(int *)(local_c + 0x1b) != *(int *)(iVar2 + 0xc) ||
            (*(int *)(local_c + 0x19) != *(int *)(iVar2 + 10))) &&
           ((*(int *)(local_c + 0x1b) != param_3 || (*(int *)(local_c + 0x19) != param_2))))) {
      piVar1 = (int *)(local_c + 0x19);
      local_a = *(int *)(local_c + 0x1b);
      local_c = *piVar1;
    }
    if ((*(int *)(local_c + 0x1b) == param_3) && (*(int *)(local_c + 0x19) == param_2)) {
      if ((*(int *)(local_c + 0x1b) == local_a) && (*(int *)(local_c + 0x19) == local_c)) {
        *(undefined2 *)(iVar2 + 10) = 0;
        *(undefined2 *)(iVar2 + 0xc) = 0;
      }
      else {
        if ((*(int *)(local_c + 0x1b) == *(int *)(iVar2 + 0xc)) &&
           (*(int *)(local_c + 0x19) == *(int *)(iVar2 + 10))) {
          *(int *)(iVar2 + 10) = local_c;
          *(int *)(iVar2 + 0xc) = local_a;
        }
        uVar4 = (undefined2)((ulong)*(undefined4 *)(local_c + 0x19) >> 0x10);
        iVar2 = (int)*(undefined4 *)(local_c + 0x19);
        uVar3 = *(undefined2 *)(iVar2 + 0x1b);
        *(undefined2 *)(local_c + 0x19) = *(undefined2 *)(iVar2 + 0x19);
        *(undefined2 *)(local_c + 0x1b) = uVar3;
      }
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_080d @ 1078:080d */
/* ======================================================== */

undefined4 __stdcall16far FUN_1078_080d(undefined4 param_1,code *param_2)

{
  undefined4 uVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  uVar1 = *(undefined4 *)((int)param_1 + 10);
  iVar5 = (int)((ulong)uVar1 >> 0x10);
  iVar3 = (int)uVar1;
  iVar4 = iVar3;
  iVar6 = iVar5;
  iVar7 = iVar5;
  if (iVar5 != 0 || iVar3 != 0) {
    do {
      iVar6 = (int)((ulong)*(undefined4 *)(iVar4 + 0x19) >> 0x10);
      iVar4 = (int)*(undefined4 *)(iVar4 + 0x19);
      cVar2 = (*param_2)();
      if (cVar2 != '\0') goto LAB_1078_084f;
      iVar7 = iVar6;
    } while ((iVar4 != iVar3) || (iVar6 != iVar5));
    iVar4 = 0;
    iVar6 = 0;
  }
LAB_1078_084f:
  return CONCAT22(iVar6,iVar4);
}

/* ======================================================== */
/* Function: FUN_1078_085a @ 1078:085a */
/* ======================================================== */

void __stdcall16far FUN_1078_085a(undefined4 param_1,code *param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  uVar2 = *(undefined4 *)((int)param_1 + 10);
  iVar5 = (int)((ulong)uVar2 >> 0x10);
  iVar3 = (int)uVar2;
  if (iVar5 != 0 || iVar3 != 0) {
    iVar4 = (int)*(undefined4 *)(iVar3 + 0x19);
    iVar6 = (int)((ulong)*(undefined4 *)(iVar3 + 0x19) >> 0x10);
    while ((iVar4 != iVar3 || (iVar6 != iVar5))) {
      iVar1 = *(int *)(iVar4 + 0x1b);
      iVar4 = *(int *)(iVar4 + 0x19);
      (*param_2)();
      iVar6 = iVar1;
    }
    (*param_2)();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_08ac @ 1078:08ac */
/* ======================================================== */

undefined2 __stdcall16far FUN_1078_08ac(void)

{
  return 0xffff;
}

/* ======================================================== */
/* Function: FUN_1078_08c3 @ 1078:08c3 */
/* ======================================================== */

void __stdcall16far
FUN_1078_08c3(undefined2 param_1,undefined2 param_2,undefined2 *param_3,undefined4 param_4)

{
  undefined2 *puVar1;
  int unaff_BP;
  
  FUN_1078_0002((int)param_4,(int)((ulong)param_4 >> 0x10),(undefined2 *)param_3,
                (int)((ulong)param_3 >> 0x10),(char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (DAT_10a8_1b64 != 0) {
    FUN_1078_0103();
  }
  puVar1 = (undefined2 *)param_3;
  (*(code *)*puVar1)();
  return;
}

/* ======================================================== */
/* Function: FUN_1078_08f0 @ 1078:08f0 */
/* ======================================================== */

void __stdcall16far FUN_1078_08f0(int *param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined2 uVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  char *pcVar9;
  int *piVar10;
  long lVar11;
  int iVar12;
  int iVar13;
  int local_6;
  
  iVar13 = 1;
  iVar7 = (int)((ulong)param_1 >> 0x10);
  piVar5 = (int *)param_1;
  pcVar9 = (char *)s_Blanks_10a8_1072 + 6;
  iVar12 = iVar7;
  cVar1 = FUN_1078_0673(param_1,1);
  iVar6 = (int)param_2;
  uVar8 = (undefined2)((ulong)param_2 >> 0x10);
  if ((cVar1 != '\0') && (*(int *)(iVar6 + 6) == 0 && *(int *)(iVar6 + 8) == 0)) {
    iVar13 = piVar5[2];
    iVar12 = *(int *)(iVar6 + 4);
    pcVar9 = (char *)s__Show_Graph_10a8_1226 + 10;
    iVar2 = GETDLGITEM((char *)s_Blanks_10a8_1072 + 6,iVar12);
    if (iVar2 != 0) {
      pcVar9 = (char *)s__Show_Graph_10a8_1226 + 10;
      iVar12 = iVar2;
      uVar3 = SENDMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,0,0,0,0x87);
      if ((uVar3 & 0x30) != 0) {
        *(int *)(iVar6 + 6) = iVar2;
        *(undefined2 *)(iVar6 + 8) = 0;
      }
    }
  }
  if (*(int *)(iVar6 + 6) == 0) {
    if (*(uint *)(iVar6 + 4) < 0x6000) {
      local_6 = GETFOCUS(pcVar9);
      piVar10 = (int *)FUN_1078_0097(local_6);
      while (((piVar10 == (int *)0x0 && (local_6 != 0)) && (local_6 != piVar5[2]))) {
        local_6 = GETPARENT((char *)s_Blanks_10a8_1072 + 6,local_6);
        piVar10 = (int *)FUN_1078_0097(local_6);
      }
      if (piVar10 == (int *)0x0) {
        piVar10 = param_1;
      }
      FUN_1078_08c3(0x10,*(int *)(iVar6 + 4) + -0x6000,iVar6,uVar8,piVar10);
    }
    else {
      (*(code *)*(undefined2 *)(*param_1 + 0xc))(pcVar9,param_1,iVar6,uVar8,iVar12,iVar13);
    }
  }
  else {
    iVar13 = piVar5[2];
    uVar4 = GETDLGITEM(pcVar9,*(undefined2 *)(iVar6 + 4));
    lVar11 = FUN_1078_0097(uVar4);
    if ((lVar11 == 0) || (0xfff < *(uint *)(iVar6 + 8))) {
      if (*(uint *)(iVar6 + 4) < 0x1000) {
        FUN_1078_08c3(0x14,*(int *)(iVar6 + 4) + -0x8000,iVar6,uVar8,param_1);
      }
      else {
        (*(code *)*(undefined2 *)(*param_1 + 0x14))
                  ((char *)s_Blanks_10a8_1072 + 6,param_1,iVar6,uVar8,iVar13);
      }
    }
    else {
      FUN_1078_08c3(0x18,*(int *)(iVar6 + 8) + -0x7000,iVar6,uVar8,lVar11);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0a85 @ 1078:0a85 */
/* ======================================================== */

void __stdcall16far FUN_1078_0a85(int *param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  long lVar5;
  undefined2 uVar6;
  
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = (int)param_2;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  if (*(int *)(iVar2 + 8) == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0xc))();
  }
  else {
    lVar5 = FUN_1078_0097(*(undefined2 *)(iVar2 + 8));
    if (lVar5 == 0) {
      uVar6 = *(undefined2 *)(iVar2 + 8);
      uVar1 = GETWINDOWWORD((char *)s_Blanks_10a8_1072 + 6,0xfff4);
      if (uVar1 < 0x1000) {
        FUN_1078_08c3(0x14,uVar1 + 0x8000,iVar2,uVar3,(int *)param_1,uVar4);
      }
      else {
        (*(code *)*(undefined2 *)(*param_1 + 0x14))
                  ((char *)s__Show_Graph_10a8_1226 + 10,(int *)param_1,uVar4,iVar2,uVar3,uVar6);
      }
    }
    else {
      FUN_1078_08c3(0x18,*(int *)(iVar2 + 4) + -0x7000,iVar2,uVar3,lVar5);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0b25 @ 1078:0b25 */
/* ======================================================== */

void __stdcall16far FUN_1078_0b25(int *param_1,undefined4 param_2)

{
  uint extraout_DX;
  int unaff_BP;
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_CS;
  int iVar5;
  char *pcVar6;
  
  iVar1 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  iVar5 = piVar2[2];
  GETWINDOWLONG(unaff_CS,0xfff0);
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  if ((extraout_DX & 0x20) == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0x48))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,(int)param_2,uVar4);
  }
  else {
    (*(code *)*(undefined2 *)(*param_1 + 0xc))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,(int)param_2,uVar4,iVar5,pcVar6,
               iVar1);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0b6c @ 1078:0b6c */
/* ======================================================== */

void __stdcall16far FUN_1078_0b6c(int *param_1,undefined4 param_2)

{
  uint extraout_DX;
  int unaff_BP;
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_CS;
  int iVar5;
  char *pcVar6;
  
  iVar1 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  iVar5 = piVar2[2];
  GETWINDOWLONG(unaff_CS,0xfff0);
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  if ((extraout_DX & 0x10) == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0x48))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,(int)param_2,uVar4);
  }
  else {
    (*(code *)*(undefined2 *)(*param_1 + 0xc))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,(int)param_2,uVar4,iVar5,pcVar6,
               iVar1);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0bb3 @ 1078:0bb3 */
/* ======================================================== */

void __stdcall16far FUN_1078_0bb3(int *param_1,int *param_2)

{
  int unaff_BP;
  int iVar1;
  int *piVar2;
  int *piVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  char *unaff_CS;
  long lVar6;
  char *pcVar7;
  
  iVar1 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  if (*param_2 == piVar2[2]) {
    lVar6 = 0;
  }
  else if (piVar2[3] == 0 && piVar2[4] == 0) {
    unaff_CS = (char *)s_Blanks_10a8_1072 + 6;
    lVar6 = FUN_1078_0097(*param_2);
  }
  else {
    lVar6 = CONCAT22(piVar2[4],piVar2[3]);
  }
  piVar3 = (int *)param_2;
  uVar5 = (undefined2)((ulong)param_2 >> 0x10);
  if (lVar6 == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0xc))(unaff_CS,piVar2,uVar4,piVar3,uVar5,0,pcVar7,iVar1);
  }
  else {
    FUN_1078_08c3(0x10,piVar3[2] + -0x6000,piVar3,uVar5,lVar6);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0c3d @ 1078:0c3d */
/* ======================================================== */

void __stdcall16far FUN_1078_0c3d(int *param_1)

{
  (*(code *)*(undefined2 *)(*param_1 + 0xc))();
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0c59 @ 1078:0c59 */
/* ======================================================== */

void __stdcall16far FUN_1078_0c59(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_CS;
  undefined2 uVar5;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  if (*(int *)(iVar1 + 6) != 0 || *(int *)(iVar1 + 8) != 0) {
    uVar4 = (undefined2)((ulong)param_2 >> 0x10);
    iVar2 = (int)param_2;
    if (*(int *)(iVar2 + 2) == 0x111) {
      FUN_1078_08c3(0x14,*(int *)(iVar2 + 4) + -0x8000,iVar2,uVar4,*(undefined2 *)(iVar1 + 6),
                    *(undefined2 *)(iVar1 + 8));
    }
    else {
      uVar5 = *(undefined2 *)(iVar1 + 6);
      uVar3 = *(undefined2 *)(iVar1 + 4);
      iVar1 = GETWINDOWWORD(unaff_CS,0xfff4);
      FUN_1078_08c3(0x14,iVar1 + -0x8000,uVar3,iVar2,uVar4,uVar5);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0cc7 @ 1078:0cc7 */
/* ======================================================== */

void __stdcall16far FUN_1078_0cc7(undefined2 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_2 >> 0x10);
  if (*(int *)((int)param_2 + 4) != 0) {
    FUN_1078_060d((int)param_2,uVar1);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0ce7 @ 1078:0ce7 */
/* ======================================================== */

void __stdcall16far FUN_1078_0ce7(undefined4 param_1)

{
  int *piVar1;
  char cVar2;
  int unaff_BP;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  long lVar6;
  char *pcVar7;
  
  iVar3 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  if (*(int *)(iVar4 + 4) != 0) {
    FUN_1078_085a(iVar4,uVar5,0xcc7,(char *)s_Blanks_10a8_1072 + 6);
    cVar2 = FUN_1078_0673(iVar4,uVar5,8);
    if (cVar2 != '\0') {
      piVar1 = (int *)*(undefined4 *)(iVar4 + 6);
      lVar6 = (*(code *)*(undefined2 *)(*piVar1 + 0x30))
                        ((char *)s_Blanks_10a8_1072 + 6,(int *)piVar1,(int)((ulong)piVar1 >> 0x10),
                         pcVar7,iVar3);
      if (lVar6 != 0) {
        piVar1 = (int *)*(undefined4 *)(iVar4 + 6);
        (*(code *)*(undefined2 *)(*piVar1 + 0x30))
                  ((char *)s_Blanks_10a8_1072 + 6,(int *)piVar1,(int)((ulong)piVar1 >> 0x10));
        SENDMESSAGE((char *)s_Blanks_10a8_1072 + 6,0,0,*(undefined2 *)(iVar4 + 4),0x221);
        return;
      }
    }
    DESTROYWINDOW((char *)s_Blanks_10a8_1072 + 6,*(undefined2 *)(iVar4 + 4));
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0d6a @ 1078:0d6a */
/* ======================================================== */

char * __stdcall16far FUN_1078_0d6a(void)

{
  return s_TurboWindow_10a8_1b6c;
}

/* ======================================================== */
/* Function: FUN_1078_0d8a @ 1078:0d8a */
/* ======================================================== */

void __stdcall16far FUN_1078_0d8a(void)

{
  int unaff_BP;
  
  FUN_1088_0002((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0d9b @ 1078:0d9b */
/* ======================================================== */

void __stdcall16far FUN_1078_0d9b(int *param_1)

{
  char cVar1;
  int unaff_BP;
  int iVar2;
  int *piVar3;
  undefined2 uVar4;
  char *pcVar5;
  
  iVar2 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  piVar3 = (int *)param_1;
  cVar1 = FUN_1078_055c(piVar3,uVar4);
  if (cVar1 == '\0') {
    piVar3[1] = -4;
  }
  else {
    (*(code *)*(undefined2 *)(*param_1 + 0x44))
              ((char *)s_Blanks_10a8_1072 + 6,piVar3,uVar4,2,pcVar5,iVar2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0dcd @ 1078:0dcd */
/* ======================================================== */

void __stdcall16far FUN_1078_0dcd(int param_1,int *param_2)

{
  char cVar1;
  int unaff_BP;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  char *pcVar4;
  
  iVar2 = unaff_BP + 1;
  pcVar4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  cVar1 = FUN_1078_0673((int *)param_2,uVar3,0x10);
  if (cVar1 != '\0') {
    iVar2 = (*(code *)*(undefined2 *)(*param_2 + 0x40))
                      ((char *)s_Blanks_10a8_1072 + 6,(int *)param_2,uVar3,
                       *(undefined2 *)(param_1 + 10),*(undefined2 *)(param_1 + -6),
                       *(undefined2 *)(param_1 + -4),pcVar4,iVar2);
    *(int *)(param_1 + -6) = *(int *)(param_1 + -6) + iVar2;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0e0a @ 1078:0e0a */
/* ======================================================== */

void __stdcall16far FUN_1078_0e0a(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  if (*(int *)(iVar1 + 0xe) != 0 || *(int *)(iVar1 + 0x10) != 0) {
    FUN_1078_085a(iVar1,uVar2,0xdcd,(char *)s_Blanks_10a8_1072 + 6);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0e46 @ 1078:0e46 */
/* ======================================================== */

undefined1 __stdcall16far FUN_1078_0e46(int *param_1)

{
  int iVar1;
  int unaff_BP;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined4 uVar2;
  undefined1 local_20 [27];
  undefined1 local_5;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  local_5 = 1;
  uVar2 = (*(code *)*(undefined2 *)(*param_1 + 0x2c))();
  iVar1 = GETCLASSINFO(unaff_CS,local_20,unaff_SS,uVar2);
  if (iVar1 == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0x34))
              ((char *)s__Show_Graph_10a8_1226 + 10,(int *)param_1,(int)((ulong)param_1 >> 0x10),
               local_20);
    iVar1 = REGISTERCLASS((char *)s__Show_Graph_10a8_1226 + 10,local_20);
    local_5 = iVar1 != 0;
  }
  return local_5;
}

/* ======================================================== */
/* Function: FUN_1078_0e9e @ 1078:0e9e */
/* ======================================================== */

void __stdcall16far FUN_1078_0e9e(undefined4 param_1,undefined2 param_2)

{
  undefined2 unaff_CS;
  
  if (*(int *)((int)param_1 + 4) != 0) {
    SHOWWINDOW(unaff_CS,param_2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0ec0 @ 1078:0ec0 */
/* ======================================================== */

undefined1 __stdcall16far FUN_1078_0ec0(undefined2 param_1,int *param_2)

{
  char cVar1;
  
  if ((((int *)param_2)[2] != 0) &&
     (cVar1 = (*(code *)*(undefined2 *)(*param_2 + 0x3c))(), cVar1 == '\0')) {
    return 1;
  }
  return 0;
}

/* ======================================================== */
/* Function: FUN_1078_0ef4 @ 1078:0ef4 */
/* ======================================================== */

uint __stdcall16far FUN_1078_0ef4(undefined4 param_1)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  
  uVar3 = FUN_1078_080d((int)param_1,(int)((ulong)param_1 >> 0x10),0xec0,
                        (char *)s_Blanks_10a8_1072 + 6);
  uVar1 = (uint)uVar3 | (uint)((ulong)uVar3 >> 0x10);
  uVar2 = uVar1 & 0xff00;
  if (uVar1 == 0) {
    uVar2 = uVar2 + 1;
  }
  return uVar2;
}

/* ======================================================== */
/* Function: FUN_1078_0f22 @ 1078:0f22 */
/* ======================================================== */

void __stdcall16far FUN_1078_0f22(undefined4 param_1)

{
  FUN_1078_0f38((int)param_1,(int)((ulong)param_1 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0f38 @ 1078:0f38 */
/* ======================================================== */

void __stdcall16far FUN_1078_0f38(int *param_1)

{
  int iVar1;
  undefined2 uVar2;
  char local_5;
  
  iVar1 = (int)((ulong)param_1 >> 0x10);
  uVar2 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  if ((iVar1 == ((int *)DAT_10a8_1b2e)[5]) && ((int *)param_1 == (int *)((int *)DAT_10a8_1b2e)[4]))
  {
    local_5 = (*(code *)*(undefined2 *)(*DAT_10a8_1b2e + 0x44))();
  }
  else {
    local_5 = (*(code *)*(undefined2 *)(*param_1 + 0x3c))();
  }
  if (local_5 != '\0') {
    FUN_1088_002e((int *)param_1,iVar1);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0f8d @ 1078:0f8d */
/* ======================================================== */

void __stdcall16far FUN_1078_0f8d(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  char *unaff_CS;
  
  iVar1 = (int)((ulong)param_1 >> 0x10);
  uVar2 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  if ((iVar1 == *(int *)((int)DAT_10a8_1b2e + 10)) &&
     ((int *)param_1 == (int *)*(int *)((int)DAT_10a8_1b2e + 8))) {
    unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
    POSTQUITMESSAGE();
  }
  (*(code *)*(undefined2 *)(*param_1 + 0xc))
            (unaff_CS,(int *)param_1,iVar1,(int)param_2,(int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0fcc @ 1078:0fcc */
/* ======================================================== */

void __stdcall16far FUN_1078_0fcc(int *param_1,undefined4 param_2)

{
  int unaff_BP;
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  char *pcVar4;
  
  iVar1 = unaff_BP + 1;
  pcVar4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  FUN_1078_0071(piVar2[2]);
  (*(code *)*(undefined2 *)(*param_1 + 0xc))
            ((char *)s_Blanks_10a8_1072 + 6,piVar2,uVar3,(int)param_2,(int)((ulong)param_2 >> 0x10),
             pcVar4,iVar1);
  piVar2[2] = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1078_0ffd @ 1078:0ffd */
/* ======================================================== */

void __stdcall16far FUN_1078_0ffd(int *param_1,undefined4 param_2)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  (*(code *)*(undefined2 *)(*param_1 + 0xc))();
  if (*(int *)((int)param_2 + 4) != 0) {
    cVar1 = FUN_1078_0673((int *)param_1,uVar2,1);
    uVar3 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
    if (cVar1 == '\0') {
      FUN_1078_1beb((int)DAT_10a8_1b2e,uVar3,0,0);
    }
    else {
      FUN_1078_1beb((int)DAT_10a8_1b2e,uVar3,(int *)param_1,uVar2);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1054 @ 1078:1054 */
/* ======================================================== */

void __stdcall16far FUN_1078_1054(int *param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  
  uVar3 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  iVar2 = (int)param_2;
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  if (((int)((ulong)param_1 >> 0x10) == ((int *)DAT_10a8_1b2e)[5]) &&
     ((int *)param_1 == (int *)((int *)DAT_10a8_1b2e)[4])) {
    cVar1 = (*(code *)*(undefined2 *)(*DAT_10a8_1b2e + 0x44))();
    *(int *)(iVar2 + 10) = (int)cVar1;
    *(int *)(iVar2 + 0xc) = (int)cVar1 >> 0xf;
  }
  else {
    cVar1 = (*(code *)*(undefined2 *)(*param_1 + 0x3c))();
    *(int *)(iVar2 + 10) = (int)cVar1;
    *(int *)(iVar2 + 0xc) = (int)cVar1 >> 0xf;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_10aa @ 1078:10aa */
/* ======================================================== */

void __stdcall16far FUN_1078_10aa(int *param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = (int)((ulong)param_1 >> 0x10);
  uVar2 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  if ((iVar1 == *(int *)((int)DAT_10a8_1b2e + 10)) &&
     ((int *)param_1 == (int *)*(int *)((int)DAT_10a8_1b2e + 8))) {
    FUN_1078_0f38((int *)param_1,iVar1);
  }
  else {
    (*(code *)*(undefined2 *)(*param_1 + 0x10))();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_10e9 @ 1078:10e9 */
/* ======================================================== */

undefined4 __stdcall16far FUN_1078_10e9(void)

{
  return 0;
}

/* ======================================================== */
/* Function: FUN_1078_1106 @ 1078:1106 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1078_1106(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             int *param_5)

{
  int unaff_BP;
  int iVar1;
  int iVar2;
  undefined2 *puVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  bool bVar6;
  undefined4 uVar7;
  long lVar8;
  undefined2 *puVar9;
  char *pcVar10;
  
  iVar1 = unaff_BP + 1;
  pcVar10 = (char *)s_RandomShrs_10a8_109f + 9;
  bVar6 = true;
  FUN_10a0_03ef();
  if (!bVar6) {
    uVar4 = (undefined2)((ulong)param_1 >> 0x10);
    iVar2 = (int)param_1;
    FUN_1078_0341(iVar2,uVar4,0,(int *)param_5,param_5._2_2_);
    puVar9 = (undefined2 *)CONCAT22(uVar4,(undefined2 *)(iVar2 + 0x1d));
    uVar7 = FUN_1098_015d(param_3,param_4);
    uVar5 = (undefined2)((ulong)puVar9 >> 0x10);
    puVar3 = (undefined2 *)puVar9;
    *puVar9 = (int)uVar7;
    puVar3[1] = (int)((ulong)uVar7 >> 0x10);
    *(undefined2 *)(iVar2 + 0x37) = 0x178;
    *(undefined2 *)(iVar2 + 0x39) = (char *)s__Show_Graph_10a8_1226 + 10;
    if ((int *)param_5 == (int *)0x0 && param_5._2_2_ == 0) {
      puVar3[2] = 0;
      puVar3[3] = 0xcf;
    }
    else {
      lVar8 = (*(code *)*(undefined2 *)(*param_5 + 0x30))
                        ((char *)s_RandomDivs_10a8_1094 + 4,(int *)param_5,param_5._2_2_,puVar9,
                         pcVar10,iVar1);
      if (lVar8 == 0) {
        uVar5 = (undefined2)((ulong)puVar9 >> 0x10);
        ((undefined2 *)puVar9)[2] = 0;
        ((undefined2 *)puVar9)[3] = 0x1000;
      }
      else {
        FUN_1078_0641(iVar2,uVar4,1,8);
        *(undefined2 *)(iVar2 + 0x37) = 0x208;
        *(undefined2 *)(iVar2 + 0x39) = (char *)s__Show_Graph_10a8_1226 + 10;
        uVar5 = (undefined2)((ulong)puVar9 >> 0x10);
        ((undefined2 *)puVar9)[2] = 0;
        ((undefined2 *)puVar9)[3] = 0x400;
      }
    }
    uVar5 = (undefined2)((ulong)puVar9 >> 0x10);
    puVar3 = (undefined2 *)puVar9;
    puVar3[4] = 0;
    puVar3[5] = 0;
    puVar3[6] = 0x8000;
    puVar3[7] = 0;
    puVar3[8] = 0x8000;
    puVar3[9] = 0;
    puVar3[10] = 0;
    puVar3[0xb] = 0;
    puVar3[0xc] = 0;
    *(undefined2 *)(iVar2 + 0x3b) = 0;
    *(undefined2 *)(iVar2 + 0x3d) = 0;
    *(undefined2 *)(iVar2 + 0x3f) = 0;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1078_121b @ 1078:121b */
/* ======================================================== */

void __stdcall16far FUN_1078_121b(undefined4 param_1)

{
  int *piVar1;
  uint uVar2;
  int unaff_BP;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  char *pcVar6;
  
  iVar3 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  FUN_1098_01ca(*(undefined2 *)(iVar4 + 0x1d),*(undefined2 *)(iVar4 + 0x1f));
  uVar2 = *(uint *)(iVar4 + 0x3b) | *(uint *)(iVar4 + 0x3d);
  if (uVar2 != 0) {
    piVar1 = (int *)*(undefined4 *)(iVar4 + 0x3b);
    (*(code *)*(undefined2 *)(*piVar1 + 8))
              ((char *)s_RandomDivs_10a8_1094 + 4,(int *)piVar1,(int)((ulong)piVar1 >> 0x10),
               CONCAT11((char)(uVar2 >> 8),1),pcVar6,iVar3);
    *(undefined2 *)(iVar4 + 0x3b) = 0;
    *(undefined2 *)(iVar4 + 0x3d) = 0;
  }
  FUN_1078_03ff(iVar4,uVar5,0);
  FUN_10a0_0439();
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1274 @ 1078:1274 */
/* ======================================================== */

void __stdcall16far FUN_1078_1274(int *param_1,undefined2 *param_2)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  undefined2 *puVar3;
  undefined2 uVar4;
  undefined2 unaff_CS;
  undefined4 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  char *pcVar8;
  
  iVar2 = unaff_BP + 1;
  pcVar8 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  puVar3 = (undefined2 *)param_2;
  puVar3[3] = 0;
  puVar3[4] = 0;
  puVar3[5] = DAT_10a8_1c9e;
  uVar7 = 0;
  uVar1 = LOADICON(unaff_CS,0x7f00,0);
  puVar3[6] = uVar1;
  uVar6 = 0;
  uVar1 = LOADCURSOR((char *)s__Show_Graph_10a8_1226 + 10,0x7f00,0);
  puVar3[7] = uVar1;
  puVar3[8] = 6;
  puVar3[9] = 0;
  puVar3[10] = 0;
  uVar5 = (*(code *)*(undefined2 *)(*param_1 + 0x2c))
                    ((char *)s__Show_Graph_10a8_1226 + 10,(int *)param_1,
                     (int)((ulong)param_1 >> 0x10),uVar6,uVar7,pcVar8,iVar2);
  puVar3[0xb] = (int)uVar5;
  puVar3[0xc] = (int)((ulong)uVar5 >> 0x10);
  *param_2 = 3;
  puVar3[1] = 0x18b;
  puVar3[2] = (char *)s_Blanks_10a8_1072 + 6;
  return;
}

/* ======================================================== */
/* Function: FUN_1078_12f5 @ 1078:12f5 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1078_12f5(undefined4 param_1)

{
  return *(undefined2 *)((int)param_1 + 0x35);
}

/* ======================================================== */
/* Function: FUN_1078_1311 @ 1078:1311 */
/* ======================================================== */

void __stdcall16far FUN_1078_1311(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined4 uVar4;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  iVar1 = (int)param_2;
  uVar4 = CALLWINDOWPROC(unaff_CS,*(undefined2 *)(iVar1 + 6),*(undefined2 *)(iVar1 + 8),
                         *(undefined2 *)(iVar1 + 4),*(undefined2 *)(iVar1 + 2),
                         *(undefined2 *)((int)param_1 + 4),*(undefined2 *)((int)param_1 + 0x37));
  *(undefined2 *)(iVar1 + 10) = (int)uVar4;
  *(undefined2 *)(iVar1 + 0xc) = (int)((ulong)uVar4 >> 0x10);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_134f @ 1078:134f */
/* ======================================================== */

uint __stdcall16far FUN_1078_134f(int *param_1)

{
  int *piVar1;
  char cVar2;
  uint in_AX;
  uint uVar3;
  int unaff_BP;
  int *piVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  undefined4 uVar6;
  undefined2 uVar7;
  int iVar8;
  undefined4 local_2a;
  undefined4 local_26;
  undefined2 local_22;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  undefined2 local_1a;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_12;
  undefined4 local_c;
  undefined2 local_8;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  piVar4 = (int *)param_1;
  if (piVar4[1] == 0) {
    FUN_1078_0627(piVar4,uVar5);
    if (piVar4[3] == 0 && piVar4[4] == 0) {
      local_8 = 0;
    }
    else {
      local_8 = *(undefined2 *)((int)*(undefined4 *)(piVar4 + 3) + 4);
    }
    cVar2 = FUN_1078_0673(piVar4,uVar5,2);
    if (cVar2 == '\0') {
      in_AX = (*(code *)*(undefined2 *)(*param_1 + 0x1c))
                        ((char *)s_Blanks_10a8_1072 + 6,piVar4,uVar5);
      if ((char)in_AX != '\0') {
        DAT_10a8_1b56 = piVar4;
        DAT_10a8_1b58 = uVar5;
        cVar2 = FUN_1078_0673(piVar4,uVar5,8);
        if (cVar2 == '\0') {
          local_2a = (undefined2 *)CONCAT22(uVar5,(undefined2 *)((int)piVar4 + 0x1d));
          uVar7 = *(undefined2 *)((int)piVar4 + 0x27);
          uVar6 = (*(code *)*(undefined2 *)(*param_1 + 0x2c))
                            ((char *)s_Blanks_10a8_1072 + 6,piVar4,uVar5,
                             *(undefined2 *)((int)piVar4 + 0x25),uVar7);
          in_AX = CREATEWINDOWEX((char *)s_Blanks_10a8_1072 + 6,*(undefined2 *)((int)piVar4 + 0x31),
                                 *(undefined2 *)((int)piVar4 + 0x33),DAT_10a8_1c9e,
                                 *(undefined2 *)((int)piVar4 + 0x35),local_8,
                                 *(undefined2 *)((int)piVar4 + 0x2f),
                                 *(undefined2 *)((int)piVar4 + 0x2d),
                                 *(undefined2 *)((int)piVar4 + 0x2b),
                                 *(undefined2 *)((int)piVar4 + 0x29),
                                 *(undefined2 *)((int)piVar4 + 0x21),
                                 *(undefined2 *)((int)piVar4 + 0x23),*local_2a,
                                 *(undefined2 *)((int)piVar4 + 0x1f),uVar6,uVar7);
          piVar4[2] = in_AX;
        }
        else {
          local_26 = (*(code *)*(undefined2 *)(*param_1 + 0x2c))
                               ((char *)s_Blanks_10a8_1072 + 6,piVar4,uVar5);
          local_22 = *(undefined2 *)((int)piVar4 + 0x1d);
          local_20 = *(undefined2 *)((int)piVar4 + 0x1f);
          local_1e = DAT_10a8_1c9e;
          local_1c = *(undefined2 *)((int)piVar4 + 0x29);
          local_1a = *(undefined2 *)((int)piVar4 + 0x2b);
          local_18 = *(undefined2 *)((int)piVar4 + 0x2d);
          local_16 = *(undefined2 *)((int)piVar4 + 0x2f);
          local_14 = *(undefined2 *)((int)piVar4 + 0x21);
          local_12 = *(undefined2 *)((int)piVar4 + 0x23);
          piVar1 = *(int **)(piVar4 + 3);
          local_c = (*(code *)*(undefined2 *)(*piVar1 + 0x30))
                              ((char *)s_Blanks_10a8_1072 + 6,(int *)piVar1,
                               (int)((ulong)piVar1 >> 0x10));
          in_AX = (uint)local_c | (uint)((ulong)local_c >> 0x10);
          if (in_AX != 0) {
            in_AX = SENDMESSAGE((char *)s_Blanks_10a8_1072 + 6,&local_26,unaff_SS,0,0x220);
            piVar4[2] = in_AX;
          }
        }
      }
    }
    else {
      in_AX = GETDLGITEM((char *)s_Blanks_10a8_1072 + 6,*(undefined2 *)((int)piVar4 + 0x35));
      piVar4[2] = in_AX;
    }
    if (piVar4[2] == 0) {
      piVar4[1] = -1;
    }
    else {
      uVar6 = FUN_1078_0097(piVar4[2]);
      in_AX = (uint)uVar6 | (uint)((ulong)uVar6 >> 0x10);
      if (in_AX == 0) {
        FUN_1078_0045(piVar4,uVar5,piVar4[2]);
        iVar8 = piVar4[2];
        uVar6 = SETWINDOWLONG((char *)s_Blanks_10a8_1072 + 6,piVar4[9],piVar4[10],0xfffc);
        *(undefined2 *)((int)piVar4 + 0x37) = (int)uVar6;
        *(undefined2 *)((int)piVar4 + 0x39) = (int)((ulong)uVar6 >> 0x10);
        in_AX = (*(code *)*(undefined2 *)(*param_1 + 0x38))
                          ((char *)s__Show_Graph_10a8_1226 + 10,piVar4,uVar5,iVar8);
      }
    }
  }
  uVar3 = in_AX & 0xff00;
  if (piVar4[1] == 0) {
    uVar3 = uVar3 + 1;
  }
  return uVar3;
}

/* ======================================================== */
/* Function: FUN_1078_1547 @ 1078:1547 */
/* ======================================================== */

void __stdcall16far FUN_1078_1547(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  if (*(int *)(iVar2 + 0x3f) != 0) {
    iVar1 = ISWINDOW();
    if (iVar1 != 0) {
      iVar1 = ISICONIC((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar2 + 4));
      if (iVar1 == 0) {
        SETFOCUS((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar2 + 0x3f));
      }
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1586 @ 1078:1586 */
/* ======================================================== */

void __stdcall16far FUN_1078_1586(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  iVar1 = GETFOCUS();
  if (iVar1 != 0) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    iVar2 = ISCHILD((char *)s__Show_Graph_10a8_1226 + 10,iVar1,*(undefined2 *)((int)param_1 + 4));
    if (iVar2 != 0) {
      *(int *)((int)param_1 + 0x3f) = iVar1;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_15c0 @ 1078:15c0 */
/* ======================================================== */

void __stdcall16far FUN_1078_15c0(undefined4 param_1)

{
  int *piVar1;
  char cVar2;
  int iVar3;
  int unaff_BP;
  int iVar4;
  undefined2 uVar5;
  char *pcVar6;
  undefined2 unaff_SS;
  long lVar7;
  undefined2 uVar8;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  iVar3 = ISICONIC();
  if (iVar3 != 0) {
    return;
  }
  iVar3 = ISZOOMED((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4));
  if (iVar3 != 0) {
    return;
  }
  uVar8 = *(undefined2 *)(iVar4 + 4);
  pcVar6 = (char *)s__Show_Graph_10a8_1226 + 10;
  GETWINDOWRECT((char *)s__Show_Graph_10a8_1226 + 10,&local_c,unaff_SS);
  *(int *)(iVar4 + 0x2d) = local_8 - local_c;
  *(int *)(iVar4 + 0x2f) = local_6 - local_a;
  if (*(int *)(iVar4 + 6) != 0 || *(int *)(iVar4 + 8) != 0) {
    piVar1 = (int *)*(undefined4 *)(iVar4 + 6);
    lVar7 = (*(code *)*(undefined2 *)(*piVar1 + 0x30))
                      ((char *)s__Show_Graph_10a8_1226 + 10,(int *)piVar1,
                       (int)((ulong)piVar1 >> 0x10),uVar8);
    if (lVar7 != 0) {
      pcVar6 = (char *)s_Blanks_10a8_1072 + 6;
      cVar2 = FUN_1078_0673(iVar4,uVar5,8);
      if (cVar2 != '\0') {
        SCREENTOCLIENT((char *)s_Blanks_10a8_1072 + 6,&local_c,unaff_SS);
        goto LAB_1078_1685;
      }
    }
    if ((*(uint *)(iVar4 + 0x23) & 0x4000) != 0) {
      SCREENTOCLIENT(pcVar6,&local_c,unaff_SS);
    }
  }
LAB_1078_1685:
  *(int *)(iVar4 + 0x29) = local_c;
  *(int *)(iVar4 + 0x2b) = local_a;
  return;
}

/* ======================================================== */
/* Function: FUN_1078_169d @ 1078:169d */
/* ======================================================== */

void __stdcall16far FUN_1078_169d(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  uVar2 = (undefined2)param_1;
  FUN_1078_0ffd(uVar2,uVar4,(int)param_2,uVar3);
  cVar1 = FUN_1078_0673(uVar2,uVar4,1);
  if (cVar1 != '\0') {
    if (*(int *)((int)param_2 + 4) == 0) {
      FUN_1078_1586(uVar2,uVar4);
    }
    else {
      FUN_1078_1547(uVar2,uVar4);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_16eb @ 1078:16eb */
/* ======================================================== */

void __stdcall16far FUN_1078_16eb(void)

{
  FUN_10a0_1e88();
  return;
}

/* ======================================================== */
/* Function: FUN_1078_170c @ 1078:170c */
/* ======================================================== */

void __stdcall16far FUN_1078_170c(undefined4 param_1)

{
  int *piVar1;
  char cVar2;
  int unaff_BP;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  char *pcVar6;
  char *pcVar7;
  
  iVar3 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  FUN_1078_0d9b(iVar4,uVar5);
  pcVar6 = (char *)s_Blanks_10a8_1072 + 6;
  cVar2 = FUN_1078_0673(iVar4,uVar5,8);
  if (cVar2 != '\0') {
    pcVar6 = (char *)s__Show_Graph_10a8_1226 + 10;
    SETFOCUS((char *)s_Blanks_10a8_1072 + 6,*(undefined2 *)(iVar4 + 4),pcVar7,iVar3);
  }
  if (*(int *)(iVar4 + 0x3b) != 0 || *(int *)(iVar4 + 0x3d) != 0) {
    piVar1 = (int *)*(undefined4 *)(iVar4 + 0x3b);
    (*(code *)*(undefined2 *)(*piVar1 + 0x10))(pcVar6,(int *)piVar1,(int)((ulong)piVar1 >> 0x10));
  }
  FUN_1078_15c0(iVar4,uVar5);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1761 @ 1078:1761 */
/* ======================================================== */

void __stdcall16far FUN_1078_1761(int *param_1)

{
  (*(code *)*(undefined2 *)(*param_1 + 0x38))();
  (*(code *)*(undefined2 *)(*param_1 + 0xc))();
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1788 @ 1078:1788 */
/* ======================================================== */

void __stdcall16far FUN_1078_1788(int *param_1,undefined4 param_2)

{
  int *piVar1;
  uint extraout_DX;
  int unaff_BP;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 unaff_CS;
  int iVar7;
  char *pcVar8;
  
  iVar2 = unaff_BP + 1;
  pcVar8 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  piVar3 = (int *)param_1;
  iVar7 = piVar3[2];
  GETWINDOWLONG(unaff_CS,0xfff0);
  iVar4 = (int)param_2;
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  if ((extraout_DX & 0x10) == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0x48))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar3,uVar5,iVar4,uVar6);
  }
  else if (*(int *)((int)piVar3 + 0x3b) == 0 && *(int *)((int)piVar3 + 0x3d) == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0xc))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar3,uVar5,iVar4,uVar6,iVar7,pcVar8,iVar2);
  }
  else {
    piVar1 = (int *)*(undefined4 *)((int)piVar3 + 0x3b);
    (*(code *)*(undefined2 *)(*piVar1 + 0x20))
              ((char *)s__Show_Graph_10a8_1226 + 10,(int *)piVar1,(int)((ulong)piVar1 >> 0x10),
               *(undefined2 *)(iVar4 + 6),*(undefined2 *)(iVar4 + 4));
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_17f8 @ 1078:17f8 */
/* ======================================================== */

void __stdcall16far FUN_1078_17f8(int *param_1,undefined4 param_2)

{
  int *piVar1;
  uint extraout_DX;
  int unaff_BP;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 unaff_CS;
  int iVar7;
  char *pcVar8;
  
  iVar2 = unaff_BP + 1;
  pcVar8 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  piVar3 = (int *)param_1;
  iVar7 = piVar3[2];
  GETWINDOWLONG(unaff_CS,0xfff0);
  iVar4 = (int)param_2;
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  if ((extraout_DX & 0x20) == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0x48))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar3,uVar5,iVar4,uVar6);
  }
  else if (*(int *)((int)piVar3 + 0x3b) == 0 && *(int *)((int)piVar3 + 0x3d) == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0xc))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar3,uVar5,iVar4,uVar6,iVar7,pcVar8,iVar2);
  }
  else {
    piVar1 = (int *)*(undefined4 *)((int)piVar3 + 0x3b);
    (*(code *)*(undefined2 *)(*piVar1 + 0x1c))
              ((char *)s__Show_Graph_10a8_1226 + 10,(int *)piVar1,(int)((ulong)piVar1 >> 0x10),
               *(undefined2 *)(iVar4 + 6),*(undefined2 *)(iVar4 + 4));
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1868 @ 1078:1868 */
/* ======================================================== */

void __stdcall16far FUN_1078_1868(int *param_1)

{
  int *piVar1;
  int unaff_BP;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined1 local_24 [32];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  BEGINPAINT(unaff_CS,local_24,unaff_SS,piVar2[2]);
  if (*(int *)((int)piVar2 + 0x3b) != 0 || *(int *)((int)piVar2 + 0x3d) != 0) {
    piVar1 = (int *)*(undefined4 *)((int)piVar2 + 0x3b);
    (*(code *)*(undefined2 *)(*piVar1 + 0x14))
              ((char *)s__Show_Graph_10a8_1226 + 10,(int *)piVar1,(int)((ulong)piVar1 >> 0x10),
               local_24);
  }
  (*(code *)*(undefined2 *)(*param_1 + 0x4c))
            ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,local_24);
  if (*(int *)((int)piVar2 + 0x3b) != 0 || *(int *)((int)piVar2 + 0x3d) != 0) {
    piVar1 = (int *)*(undefined4 *)((int)piVar2 + 0x3b);
    (*(code *)*(undefined2 *)(*piVar1 + 0x18))
              ((char *)s__Show_Graph_10a8_1226 + 10,(int *)piVar1,(int)((ulong)piVar1 >> 0x10));
  }
  ENDPAINT((char *)s__Show_Graph_10a8_1226 + 10,local_24,unaff_SS);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_18e9 @ 1078:18e9 */
/* ======================================================== */

void __stdcall16far FUN_1078_18e9(void)

{
  return;
}

/* ======================================================== */
/* Function: FUN_1078_18f5 @ 1078:18f5 */
/* ======================================================== */

void __stdcall16far FUN_1078_18f5(int *param_1,undefined4 param_2)

{
  undefined2 *puVar1;
  int unaff_BP;
  int *piVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  char *unaff_CS;
  char *pcVar6;
  undefined2 unaff_SS;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  iVar3 = (int)param_2;
  uVar5 = (undefined2)((ulong)param_2 >> 0x10);
  if ((*(int *)((int)piVar2 + 0x3b) != 0 || *(int *)((int)piVar2 + 0x3d) != 0) &&
     (*(int *)(iVar3 + 4) != 1)) {
    puVar1 = (undefined2 *)(*(int *)*(undefined4 *)((int)piVar2 + 0x3b) + 0xc);
    (*(code *)*puVar1)();
  }
  pcVar6 = unaff_CS;
  if (*(int *)(iVar3 + 4) == 0) {
    pcVar6 = (char *)s__Show_Graph_10a8_1226 + 10;
    GETWINDOWRECT(unaff_CS,&local_c,unaff_SS);
    *(int *)((int)piVar2 + 0x2f) = local_6 - local_a;
    *(int *)((int)piVar2 + 0x2d) = local_8 - local_c;
  }
  (*(code *)*(undefined2 *)(*param_1 + 0xc))(pcVar6,piVar2,uVar4,iVar3,uVar5);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_196c @ 1078:196c */
/* ======================================================== */

void __stdcall16far FUN_1078_196c(int *param_1,undefined4 param_2)

{
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  char *pcVar3;
  
  iVar1 = unaff_BP + 1;
  pcVar3 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  FUN_1078_15c0((int *)param_1,uVar2);
  (*(code *)*(undefined2 *)(*param_1 + 0xc))
            ((char *)s_Blanks_10a8_1072 + 6,(int *)param_1,uVar2,(int)param_2,
             (int)((ulong)param_2 >> 0x10),pcVar3,iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1992 @ 1078:1992 */
/* ======================================================== */

void __stdcall16far FUN_1078_1992(int *param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  int unaff_BP;
  int *piVar3;
  undefined2 uVar4;
  char *unaff_CS;
  undefined2 unaff_SS;
  int iVar5;
  undefined1 local_16 [2];
  int local_14;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  piVar3 = (int *)param_1;
  if ((*(int *)((int)piVar3 + 0x3b) != 0 || *(int *)((int)piVar3 + 0x3d) != 0) &&
     (*(char *)((int)*(undefined4 *)((int)piVar3 + 0x3b) + 0x22) != '\0')) {
    iVar5 = piVar3[2];
    SETCAPTURE();
    do {
      iVar2 = PEEKMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,1,0,0,0,local_16,unaff_SS);
      if (iVar2 != 0) {
        TRANSLATEMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,local_16);
        DISPATCHMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,local_16);
      }
      piVar1 = (int *)*(undefined4 *)((int)piVar3 + 0x3b);
      (*(code *)*(undefined2 *)(*piVar1 + 0x24))
                ((char *)s__Show_Graph_10a8_1226 + 10,(int *)piVar1,(int)((ulong)piVar1 >> 0x10),
                 iVar5);
    } while (local_14 != 0x202);
    unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
    RELEASECAPTURE((char *)s__Show_Graph_10a8_1226 + 10);
  }
  (*(code *)*(undefined2 *)(*param_1 + 0xc))
            (unaff_CS,piVar3,uVar4,(int)param_2,(int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1a1a @ 1078:1a1a */
/* ======================================================== */

void __stdcall16far FUN_1078_1a1a(int *param_1,undefined4 param_2)

{
  int iVar1;
  char cVar2;
  int unaff_BP;
  int iVar3;
  int *piVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  char *pcVar7;
  
  iVar3 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  piVar4 = (int *)param_1;
  cVar2 = FUN_1078_0673(piVar4,uVar5,1);
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  if (cVar2 != '\0') {
    iVar1 = *(int *)((int)param_2 + 4);
    if (iVar1 == -0xfe0) {
      FUN_1078_1586(piVar4,uVar5);
    }
    else if (iVar1 == -0xee0) {
      FUN_1078_1547(piVar4,uVar5);
    }
  }
  (*(code *)*(undefined2 *)(*param_1 + 0xc))
            ((char *)s_Blanks_10a8_1072 + 6,piVar4,uVar5,(int)param_2,uVar6,pcVar7,iVar3);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1a6d @ 1078:1a6d */
/* ======================================================== */

int * __stdcall16far FUN_1078_1a6d(int *param_1,undefined2 param_2,int param_3,int param_4)

{
  int unaff_BP;
  int *piVar1;
  undefined2 uVar2;
  bool bVar3;
  undefined4 uVar4;
  char *pcVar5;
  
  bVar3 = true;
  FUN_10a0_03ef((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (!bVar3) {
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    piVar1 = (int *)param_1;
    FUN_1088_0014(piVar1,uVar2,0);
    piVar1[2] = param_3;
    piVar1[3] = param_4;
    DAT_10a8_1b2e._0_2_ = piVar1;
    DAT_10a8_1b2e._2_2_ = uVar2;
    piVar1[6] = 0;
    piVar1[1] = 0;
    piVar1[4] = 0;
    piVar1[5] = 0;
    piVar1[7] = 0;
    piVar1[8] = 0;
    pcVar5 = (char *)s_Blanks_10a8_1072 + 6;
    uVar4 = MAKEPROCINSTANCE((char *)s_Cancel_10a8_1086 + 2,DAT_10a8_1c9e,0x133);
    DAT_10a8_1b54 = (undefined2)((ulong)uVar4 >> 0x10);
    DAT_10a8_1b52 = (undefined2)uVar4;
    FUN_1090_0098(pcVar5);
    if (DAT_10a8_1c9c == 0) {
      (*(code *)*(undefined2 *)(*param_1 + 0x10))(0x1090,piVar1,uVar2);
    }
    if (piVar1[1] == 0) {
      (*(code *)*(undefined2 *)(*param_1 + 0x14))(0x1090,piVar1,uVar2);
    }
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1078_1b14 @ 1078:1b14 */
/* ======================================================== */

void __stdcall16far FUN_1078_1b14(undefined4 param_1)

{
  int unaff_BP;
  int iVar1;
  undefined2 unaff_CS;
  undefined2 uVar2;
  char *pcVar3;
  
  iVar1 = unaff_BP + 1;
  pcVar3 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar2 = DAT_10a8_1b54;
  FREEPROCINSTANCE(unaff_CS,DAT_10a8_1b52);
  FUN_1088_0048((int)param_1,(int)((ulong)param_1 >> 0x10),0);
  FUN_10a0_0439(uVar2,pcVar3,iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1b41 @ 1078:1b41 */
/* ======================================================== */

undefined1 __stdcall16far FUN_1078_1b41(void)

{
  return 0;
}

/* ======================================================== */
/* Function: FUN_1078_1b57 @ 1078:1b57 */
/* ======================================================== */

void __stdcall16far FUN_1078_1b57(void)

{
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1b63 @ 1078:1b63 */
/* ======================================================== */

void __stdcall16far FUN_1078_1b63(int *param_1)

{
  int *piVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  piVar1 = (int *)param_1;
  (*(code *)*(undefined2 *)(*param_1 + 0x18))();
  uVar3 = (*(code *)*(undefined2 *)(*param_1 + 0x34))();
  piVar1[4] = (int)uVar3;
  piVar1[5] = (int)((ulong)uVar3 >> 0x10);
  if (piVar1[4] == 0 && piVar1[5] == 0) {
    piVar1[1] = -5;
  }
  else {
    FUN_1078_0e9e((int)*(undefined4 *)(piVar1 + 4),(int)((ulong)*(undefined4 *)(piVar1 + 4) >> 0x10)
                  ,DAT_10a8_1ca0);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1bbc @ 1078:1bbc */
/* ======================================================== */

void __stdcall16far FUN_1078_1bbc(int *param_1)

{
  if (((int *)param_1)[1] == 0) {
    (*(code *)*(undefined2 *)(*param_1 + 0x20))();
  }
  else {
    (*(code *)*(undefined2 *)(*param_1 + 0x40))();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1beb @ 1078:1beb */
/* ======================================================== */

void __stdcall16far FUN_1078_1beb(undefined4 param_1,undefined2 param_2,undefined2 param_3)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  *(undefined2 *)((int)param_1 + 0xe) = param_2;
  *(undefined2 *)((int)param_1 + 0x10) = param_3;
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1c08 @ 1078:1c08 */
/* ======================================================== */

void __stdcall16far FUN_1078_1c08(int *param_1)

{
  char cVar1;
  char cVar2;
  int iVar3;
  int unaff_BP;
  int *piVar4;
  undefined2 uVar5;
  char *unaff_CS;
  undefined2 unaff_SS;
  undefined1 local_16 [2];
  int local_14;
  int local_12;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  cVar1 = '\0';
  do {
    iVar3 = PEEKMESSAGE(unaff_CS,1,0,0,0,local_16,unaff_SS);
    piVar4 = (int *)param_1;
    uVar5 = (undefined2)((ulong)param_1 >> 0x10);
    if (iVar3 == 0) {
      cVar2 = (*(code *)*(undefined2 *)(*param_1 + 0xc))
                        ((char *)s__Show_Graph_10a8_1226 + 10,piVar4,uVar5);
      if (cVar2 == '\0') {
        cVar1 = '\x12';
        WAITMESSAGE();
      }
    }
    else if (local_14 == 0x12) {
      cVar1 = '\x01';
    }
    else {
      cVar2 = (*(code *)*(undefined2 *)(*param_1 + 0x24))
                        ((char *)s__Show_Graph_10a8_1226 + 10,piVar4,uVar5,local_16);
      if (cVar2 == '\0') {
        TRANSLATEMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,local_16);
        DISPATCHMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,local_16);
      }
    }
    unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
  } while (cVar1 == '\0');
  piVar4[1] = local_12;
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1c8b @ 1078:1c8b */
/* ======================================================== */

undefined1 __stdcall16far FUN_1078_1c8b(int *param_1)

{
  char cVar1;
  
  cVar1 = (*(code *)*(undefined2 *)(*param_1 + 0x28))();
  if (((cVar1 == '\0') && (cVar1 = (*(code *)*(undefined2 *)(*param_1 + 0x30))(), cVar1 == '\0')) &&
     (cVar1 = (*(code *)*(undefined2 *)(*param_1 + 0x2c))(), cVar1 == '\0')) {
    return 0;
  }
  return 1;
}

/* ======================================================== */
/* Function: FUN_1078_1ce2 @ 1078:1ce2 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1078_1ce2(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  undefined1 local_5;
  
  local_5 = false;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  uVar1 = *(uint *)(iVar2 + 0xe) | *(uint *)(iVar2 + 0x10);
  if ((uVar1 != 0) && (*(int *)((int)*(undefined4 *)(iVar2 + 0xe) + 4) != 0)) {
    iVar2 = ISDIALOGMESSAGE(unaff_CS,(int)param_2,(int)((ulong)param_2 >> 0x10));
    local_5 = iVar2 != 0;
    uVar1 = CONCAT11((char)((uint)-iVar2 >> 8),local_5);
  }
  return CONCAT11((char)(uVar1 >> 8),local_5);
}

/* ======================================================== */
/* Function: FUN_1078_1d2e @ 1078:1d2e */
/* ======================================================== */

undefined2 __stdcall16far FUN_1078_1d2e(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if ((*(int *)((int)param_1 + 0xc) != 0) &&
     (iVar1 = TRANSLATEACCELERATOR
                        (unaff_CS,(int)param_2,(int)((ulong)param_2 >> 0x10),
                         *(undefined2 *)((int)param_1 + 0xc)), iVar1 != 0)) {
    return 1;
  }
  return 0;
}

/* ======================================================== */
/* Function: FUN_1078_1d70 @ 1078:1d70 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1078_1d70(undefined4 param_1,undefined4 param_2)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 unaff_CS;
  long lVar3;
  
  puVar1 = (undefined2 *)(*(int *)*(undefined4 *)((int)param_1 + 8) + 0x30);
  lVar3 = (*(code *)*puVar1)();
  if ((lVar3 != 0) &&
     (iVar2 = TRANSLATEMDISYSACCEL(unaff_CS,(int)param_2,(int)((ulong)param_2 >> 0x10)), iVar2 != 0)
     ) {
    return 1;
  }
  return 0;
}

/* ======================================================== */
/* Function: FUN_1078_1dbd @ 1078:1dbd */
/* ======================================================== */

int * __stdcall16far FUN_1078_1dbd(int *param_1,int *param_2)

{
  char cVar1;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  int unaff_BP;
  undefined2 uVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  
  piVar3 = (int *)0x0;
  iVar5 = 0;
  piVar4 = piVar3;
  iVar6 = iVar5;
  if ((int *)param_2 != (int *)0x0 || param_2._2_2_ != 0) {
    cVar1 = FUN_1090_0002(0,0,(char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    if (cVar1 == '\0') {
      piVar4 = (int *)param_2;
      iVar6 = param_2._2_2_;
      if (((int *)param_2)[1] != 0) {
        (*(code *)*(undefined2 *)(*param_1 + 0x40))(0x1090,(int *)param_1,uVar2,((int *)param_2)[1])
        ;
        (*(code *)*(undefined2 *)(*param_2 + 8))
                  (0x1090,(int *)param_2,param_2._2_2_,CONCAT11(extraout_AH_00,1));
        piVar4 = piVar3;
        iVar6 = iVar5;
      }
    }
    else {
      (*(code *)*(undefined2 *)(*param_1 + 0x40))(0x1090,(int *)param_1,uVar2,0xfffe);
      (*(code *)*(undefined2 *)(*param_2 + 8))
                (0x1090,(int *)param_2,param_2._2_2_,CONCAT11(extraout_AH,1));
      FUN_1090_0021();
      piVar4 = piVar3;
      iVar6 = iVar5;
    }
  }
  return (int *)CONCAT22(iVar6,piVar4);
}

/* ======================================================== */
/* Function: FUN_1078_1e45 @ 1078:1e45 */
/* ======================================================== */

int * __stdcall16far FUN_1078_1e45(int *param_1,int *param_2)

{
  char cVar1;
  long lVar2;
  int *local_8;
  int local_6;
  
  local_8 = (int *)0x0;
  local_6 = 0;
  if ((int *)param_2 != (int *)0x0 || param_2._2_2_ != 0) {
    lVar2 = (*(code *)*(undefined2 *)(*param_1 + 0x3c))();
    if (lVar2 != 0) {
      cVar1 = (*(code *)*(undefined2 *)(*param_2 + 0x20))();
      if (cVar1 == '\0') {
        (*(code *)*(undefined2 *)(*param_1 + 0x40))();
        (*(code *)*(undefined2 *)(*param_2 + 8))();
      }
      else {
        local_8 = (int *)param_2;
        local_6 = param_2._2_2_;
      }
    }
  }
  return (int *)CONCAT22(local_6,local_8);
}

/* ======================================================== */
/* Function: FUN_1078_1ebc @ 1078:1ebc */
/* ======================================================== */

int __stdcall16far FUN_1078_1ebc(int *param_1,int *param_2)

{
  int iVar1;
  long lVar2;
  int local_6;
  
  local_6 = 2;
  lVar2 = (*(code *)*(undefined2 *)(*param_1 + 0x3c))();
  if (lVar2 != 0) {
    iVar1 = (*(code *)*(undefined2 *)(*param_2 + 0x4c))();
    if (iVar1 < 0) {
      (*(code *)*(undefined2 *)(*param_1 + 0x40))();
      iVar1 = local_6;
    }
    local_6 = iVar1;
    (*(code *)*(undefined2 *)(*param_2 + 8))();
  }
  return local_6;
}

/* ======================================================== */
/* Function: FUN_1078_1f20 @ 1078:1f20 */
/* ======================================================== */

void __stdcall16far FUN_1078_1f20(void)

{
  int iVar1;
  int unaff_BP;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined1 local_24 [32];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  WVSPRINTF(unaff_CS,&stack0x0008,unaff_SS,(char *)s_Error_code____d__Continue__10a8_1b78,
            (char *)s_RandomShrs_10a8_109f + 9,local_24);
  iVar1 = (*DAT_10a8_1b46)((char *)s__Show_Graph_10a8_1226 + 10,0x14,
                           (char *)s_Application_Error_10a8_1b93,(char *)s_RandomShrs_10a8_109f + 9,
                           local_24);
  if (iVar1 == 7) {
    FUN_10a0_0061((char *)s__Show_Graph_10a8_1226 + 10);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1078_1f62 @ 1078:1f62 */
/* ======================================================== */

undefined1 __stdcall16far FUN_1078_1f62(undefined4 param_1)

{
  undefined2 *puVar1;
  undefined1 uVar2;
  
  puVar1 = (undefined2 *)(*(int *)*(undefined4 *)((int)param_1 + 8) + 0x3c);
  uVar2 = (*(code *)*puVar1)();
  return uVar2;
}

/* ======================================================== */
/* Function: FUN_1088_0002 @ 1088:0002 */
/* ======================================================== */

void __cdecl16far FUN_1088_0002(void)

{
  FUN_10a0_005d();
  return;
}

/* ======================================================== */
/* Function: FUN_1088_0014 @ 1088:0014 */
/* ======================================================== */

undefined4 __stdcall16far FUN_1088_0014(undefined4 param_1)

{
  int unaff_BP;
  
  FUN_10a0_03ef((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1088_002e @ 1088:002e */
/* ======================================================== */

void __stdcall16far FUN_1088_002e(int *param_1)

{
  (*(code *)*(undefined2 *)(*param_1 + 8))();
  return;
}

/* ======================================================== */
/* Function: FUN_1088_0048 @ 1088:0048 */
/* ======================================================== */

void __stdcall16far FUN_1088_0048(void)

{
  int unaff_BP;
  
  FUN_10a0_0439((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  return;
}

/* ======================================================== */
/* Function: FUN_1090_0002 @ 1090:0002 */
/* ======================================================== */

uint __cdecl16far FUN_1090_0002(void)

{
  uint uVar1;
  
  uVar1 = (DAT_10a8_1c96 | DAT_10a8_1c98) & 0xff00;
  if ((DAT_10a8_1c96 | DAT_10a8_1c98) == 0) {
    uVar1 = uVar1 + 1;
  }
  return uVar1;
}

/* ======================================================== */
/* Function: FUN_1090_0021 @ 1090:0021 */
/* ======================================================== */

void __cdecl16far FUN_1090_0021(void)

{
  char cVar1;
  int unaff_BP;
  undefined4 uVar2;
  
  cVar1 = FUN_1090_0002((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  uVar2 = CONCAT22(DAT_10a8_1c98,DAT_10a8_1c96);
  if (cVar1 != '\0') {
    uVar2 = FUN_10a0_012d(DAT_10a8_1c94);
  }
  DAT_10a8_1c98 = (undefined2)((ulong)uVar2 >> 0x10);
  DAT_10a8_1c96 = (undefined2)uVar2;
  return;
}

/* ======================================================== */
/* Function: FUN_1090_0044 @ 1090:0044 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1090_0044(int param_1)

{
  char cVar1;
  undefined2 local_6;
  
  if (param_1 != 0) {
    if (DAT_10a8_1c9a == '\0') {
      cVar1 = FUN_1090_0002();
      if (cVar1 == '\0') {
        FUN_10a0_0147(DAT_10a8_1c94,DAT_10a8_1c96,DAT_10a8_1c98);
        DAT_10a8_1c96 = 0;
        DAT_10a8_1c98 = 0;
        local_6 = 2;
      }
      else {
        local_6 = 0;
      }
    }
    else {
      local_6 = 1;
    }
  }
  return local_6;
}

/* ======================================================== */
/* Function: FUN_1090_0098 @ 1090:0098 */
/* ======================================================== */

void __cdecl16far FUN_1090_0098(void)

{
  int unaff_BP;
  
  FUN_1090_0021((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  DAT_10a8_1cae = 0x44;
  DAT_10a8_1cb0 = 0x1090;
  return;
}

/* ======================================================== */
/* Function: FUN_1098_0002 @ 1098:0002 */
/* ======================================================== */

int __stdcall16far FUN_1098_0002(char *param_1)

{
  char *pcVar1;
  int iVar2;
  char *pcVar3;
  
  pcVar3 = (char *)param_1;
  iVar2 = -1;
  do {
    if (iVar2 == 0) break;
    iVar2 = iVar2 + -1;
    pcVar1 = pcVar3;
    pcVar3 = pcVar3 + 1;
  } while (*pcVar1 != '\0');
  return -2 - iVar2;
}

/* ======================================================== */
/* Function: FUN_1098_0019 @ 1098:0019 */
/* ======================================================== */

char * __stdcall16far FUN_1098_0019(char *param_1)

{
  char *pcVar1;
  int iVar2;
  char *pcVar3;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  pcVar3 = (char *)param_1;
  iVar2 = -1;
  do {
    if (iVar2 == 0) break;
    iVar2 = iVar2 + -1;
    pcVar1 = pcVar3;
    pcVar3 = pcVar3 + 1;
  } while (*pcVar1 != '\0');
  return (char *)CONCAT22(uVar4,pcVar3 + -1);
}

/* ======================================================== */
/* Function: FUN_1098_0030 @ 1098:0030 */
/* ======================================================== */

undefined1 * __stdcall16far FUN_1098_0030(int param_1,undefined1 *param_2,undefined1 *param_3)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  byte bVar5;
  
  bVar5 = 0;
  puVar3 = (undefined1 *)param_2;
  puVar4 = (undefined1 *)param_3;
  if (puVar3 < puVar4) {
    bVar5 = 1;
    puVar3 = puVar3 + param_1 + -1;
    puVar4 = puVar4 + param_1 + -1;
  }
  for (; param_1 != 0; param_1 = param_1 + -1) {
    puVar2 = puVar4;
    puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
    puVar1 = puVar3;
    puVar3 = puVar3 + (uint)bVar5 * -2 + 1;
    *puVar2 = *puVar1;
  }
  return param_3;
}

/* ======================================================== */
/* Function: FUN_1098_0055 @ 1098:0055 */
/* ======================================================== */

char * __stdcall16far FUN_1098_0055(char *param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  uint uVar3;
  char *pcVar4;
  char *pcVar5;
  undefined2 uVar6;
  
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  pcVar4 = (char *)param_1;
  uVar3 = 0xffff;
  pcVar5 = pcVar4;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar1 = pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (*pcVar1 != '\0');
  pcVar5 = (char *)param_2;
  for (uVar3 = ~uVar3; uVar3 != 0; uVar3 = uVar3 - 1) {
    pcVar2 = pcVar5;
    pcVar5 = pcVar5 + 1;
    pcVar1 = pcVar4;
    pcVar4 = pcVar4 + 1;
    *pcVar2 = *pcVar1;
  }
  return param_2;
}

/* ======================================================== */
/* Function: FUN_1098_0077 @ 1098:0077 */
/* ======================================================== */

char * __stdcall16far FUN_1098_0077(int param_1,char *param_2,char *param_3)

{
  char *pcVar1;
  char *pcVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  pcVar4 = (char *)param_2;
  iVar3 = param_1;
  pcVar5 = pcVar4;
  do {
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar1 = pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (*pcVar1 != '\0');
  uVar7 = (undefined2)((ulong)param_3 >> 0x10);
  pcVar5 = (char *)param_3;
  for (param_1 = param_1 - iVar3; param_1 != 0; param_1 = param_1 + -1) {
    pcVar2 = pcVar5;
    pcVar5 = pcVar5 + 1;
    pcVar1 = pcVar4;
    pcVar4 = pcVar4 + 1;
    *pcVar2 = *pcVar1;
  }
  *pcVar5 = '\0';
  return param_3;
}

/* ======================================================== */
/* Function: FUN_1098_009f @ 1098:009f */
/* ======================================================== */

byte * __stdcall16far FUN_1098_009f(byte *param_1,byte *param_2)

{
  byte *pbVar1;
  uint uVar2;
  byte *pbVar3;
  byte *pbVar4;
  undefined2 uVar5;
  
  pbVar3 = (byte *)param_1;
  uVar5 = (undefined2)((ulong)param_2 >> 0x10);
  pbVar4 = (byte *)param_2;
  for (uVar2 = (uint)*param_1; pbVar3 = pbVar3 + 1, uVar2 != 0; uVar2 = uVar2 - 1) {
    pbVar1 = pbVar4;
    pbVar4 = pbVar4 + 1;
    *pbVar1 = *pbVar3;
  }
  *pbVar4 = 0;
  return param_2;
}

/* ======================================================== */
/* Function: FUN_1098_00bd @ 1098:00bd */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1098_00bd(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4)

{
  undefined4 uVar1;
  
  uVar1 = FUN_1098_0019(param_3,param_4);
  FUN_1098_0055(param_1,param_2,uVar1);
  return CONCAT22(param_4,param_3);
}

/* ======================================================== */
/* Function: FUN_1098_00e0 @ 1098:00e0 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1098_00e0(int param_1,undefined2 param_2,undefined2 param_3,int param_4,undefined2 param_5)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = FUN_1098_0019(param_4,param_5);
  iVar1 = (param_4 + param_1) - (uint)uVar2;
  if ((uint)uVar2 <= (uint)(param_4 + param_1) && iVar1 != 0) {
    FUN_1098_0077(iVar1,param_2,param_3,uVar2);
  }
  return CONCAT22(param_5,param_4);
}

/* ======================================================== */
/* Function: FUN_1098_010e @ 1098:010e */
/* ======================================================== */

char * __stdcall16far FUN_1098_010e(char param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  uint uVar3;
  undefined2 uVar4;
  char *pcVar5;
  undefined2 uVar6;
  bool bVar7;
  
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  pcVar5 = (char *)param_2;
  uVar3 = 0xffff;
  bVar7 = true;
  pcVar2 = pcVar5;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar1 = pcVar2;
    pcVar2 = pcVar2 + 1;
    bVar7 = *pcVar1 == '\0';
  } while (!bVar7);
  uVar3 = ~uVar3;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar1 = pcVar5;
    pcVar5 = pcVar5 + 1;
    bVar7 = param_1 == *pcVar1;
  } while (!bVar7);
  pcVar2 = (char *)0x0;
  uVar4 = 0;
  if (bVar7) {
    pcVar2 = pcVar5 + -1;
    uVar4 = uVar6;
  }
  return (char *)CONCAT22(uVar4,pcVar2);
}

/* ======================================================== */
/* Function: FUN_1098_0136 @ 1098:0136 */
/* ======================================================== */

char * __stdcall16far FUN_1098_0136(char param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  uint uVar3;
  undefined2 uVar4;
  char *pcVar5;
  undefined2 uVar6;
  bool bVar7;
  
  uVar6 = (undefined2)((ulong)param_2 >> 0x10);
  pcVar5 = (char *)param_2;
  uVar3 = 0xffff;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar1 = pcVar5;
    pcVar5 = pcVar5 + 1;
  } while (*pcVar1 != '\0');
  uVar3 = ~uVar3;
  pcVar5 = pcVar5 + -1;
  bVar7 = pcVar5 == (char *)0x0;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar1 = pcVar5;
    pcVar5 = pcVar5 + -1;
    bVar7 = param_1 == *pcVar1;
  } while (!bVar7);
  pcVar2 = (char *)0x0;
  uVar4 = 0;
  if (bVar7) {
    pcVar2 = pcVar5 + 1;
    uVar4 = uVar6;
  }
  return (char *)CONCAT22(uVar4,pcVar2);
}

/* ======================================================== */
/* Function: FUN_1098_015d @ 1098:015d */
/* ======================================================== */

undefined4 __stdcall16far FUN_1098_015d(char *param_1)

{
  int iVar1;
  long lVar2;
  undefined4 uVar3;
  
  uVar3 = 0;
  if (((char *)param_1 != (char *)0x0 || param_1._2_2_ != 0) && (uVar3 = 0, *param_1 != '\0')) {
    iVar1 = FUN_1098_0002((char *)param_1,param_1._2_2_);
    lVar2 = FUN_10a0_012d(iVar1 + 1);
    uVar3 = 0;
    if (lVar2 != 0) {
      uVar3 = FUN_1098_0030(iVar1 + 1,(char *)param_1,param_1._2_2_,lVar2);
    }
  }
  return uVar3;
}

/* ======================================================== */
/* Function: FUN_1098_01ca @ 1098:01ca */
/* ======================================================== */

void __stdcall16far FUN_1098_01ca(int param_1,int param_2)

{
  int iVar1;
  
  if (param_1 != 0 || param_2 != 0) {
    iVar1 = FUN_1098_0002(param_1,param_2);
    FUN_10a0_0147(iVar1 + 1,param_1,param_2);
  }
  return;
}

