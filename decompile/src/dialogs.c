/*
 * dialogs.c - Oil Baron User Interface & Dialog Window Procedures
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - Win16 / Borland ObjectWindows (OWL) dialog implementations:
 *       * OPTIONS dialog (FUN_1070_0002): grid size, start cash, share limits
 *       * NEW GAME dialog (FUN_1070_02d3): player names, human/computer types
 *       * ABOUT dialog (FUN_1070_0bb2): copyright, version, author credits
 *       * REGISTRATION dialog: shareware registration code validation
 *       * CHOOSE COMPANY dialog: company selection on tie/formation
 *       * DISCARD PIECE dialog: dead/unplayable tile discard
 *       * QUICK PEEK / GRAPH dialogs: portfolio and wealth graphs
 *   - Segments 1080 / 1068 / 1058: Dialog event handlers and widget controls
 */

/* Function Index:
 *   FUN_1058_0002        @ 1058:0002
 *   FUN_1058_0123        @ 1058:0123
 *   FUN_1058_0152        @ 1058:0152
 *   FUN_1058_0177        @ 1058:0177
 *   FUN_1058_019c        @ 1058:019c
 *   FUN_1058_01c1        @ 1058:01c1
 *   FUN_1058_01e6        @ 1058:01e6
 *   FUN_1058_020b        @ 1058:020b
 *   FUN_1058_022b        @ 1058:022b
 *   FUN_1058_026e        @ 1058:026e
 *   FUN_1058_04ee        @ 1058:04ee
 *   FUN_1058_052e        @ 1058:052e
 *   FUN_1068_0002        @ 1068:0002
 *   FUN_1068_0164        @ 1068:0164
 *   FUN_1068_01b8        @ 1068:01b8
 *   FUN_1068_02e5        @ 1068:02e5
 *   FUN_1068_037b        @ 1068:037b
 *   FUN_1068_04d6        @ 1068:04d6
 *   FUN_1068_064d        @ 1068:064d
 *   FUN_1068_0761        @ 1068:0761
 *   FUN_1068_07dd        @ 1068:07dd
 *   FUN_1068_0860        @ 1068:0860
 *   FUN_1068_0880        @ 1068:0880
 *   FUN_1068_08a8        @ 1068:08a8
 *   FUN_1070_0002        @ 1070:0002
 *   FUN_1070_02d3        @ 1070:02d3
 *   FUN_1070_0355        @ 1070:0355
 *   FUN_1070_03f3        @ 1070:03f3
 *   FUN_1070_0517        @ 1070:0517
 *   FUN_1070_05cb        @ 1070:05cb
 *   FUN_1070_05ee        @ 1070:05ee
 *   FUN_1070_0611        @ 1070:0611
 *   FUN_1070_0634        @ 1070:0634
 *   FUN_1070_0657        @ 1070:0657
 *   FUN_1070_067a        @ 1070:067a
 *   FUN_1070_069d        @ 1070:069d
 *   FUN_1070_06c0        @ 1070:06c0
 *   FUN_1070_0732        @ 1070:0732
 *   FUN_1070_075a        @ 1070:075a
 *   FUN_1070_0902        @ 1070:0902
 *   FUN_1070_092a        @ 1070:092a
 *   FUN_1070_0945        @ 1070:0945
 *   FUN_1070_0960        @ 1070:0960
 *   FUN_1070_097b        @ 1070:097b
 *   FUN_1070_0996        @ 1070:0996
 *   FUN_1070_09b1        @ 1070:09b1
 *   FUN_1070_09cc        @ 1070:09cc
 *   FUN_1070_09e7        @ 1070:09e7
 *   FUN_1070_0a02        @ 1070:0a02
 *   FUN_1070_0a1d        @ 1070:0a1d
 *   FUN_1070_0a38        @ 1070:0a38
 *   FUN_1070_0a53        @ 1070:0a53
 *   FUN_1070_0a6e        @ 1070:0a6e
 *   FUN_1070_0a89        @ 1070:0a89
 *   FUN_1070_0aa4        @ 1070:0aa4
 *   FUN_1070_0abf        @ 1070:0abf
 *   FUN_1070_0ada        @ 1070:0ada
 *   FUN_1070_0af5        @ 1070:0af5
 *   FUN_1070_0b10        @ 1070:0b10
 *   FUN_1070_0b2b        @ 1070:0b2b
 *   FUN_1070_0b46        @ 1070:0b46
 *   FUN_1070_0b61        @ 1070:0b61
 *   FUN_1070_0b7c        @ 1070:0b7c
 *   FUN_1070_0b97        @ 1070:0b97
 *   FUN_1070_0bb2        @ 1070:0bb2
 *   FUN_1070_0c30        @ 1070:0c30
 *   FUN_1070_0c9c        @ 1070:0c9c
 *   FUN_1080_0002        @ 1080:0002
 *   FUN_1080_007a        @ 1080:007a
 *   FUN_1080_00b1        @ 1080:00b1
 *   FUN_1080_014f        @ 1080:014f
 *   FUN_1080_021d        @ 1080:021d
 *   FUN_1080_023d        @ 1080:023d
 *   FUN_1080_0274        @ 1080:0274
 *   FUN_1080_028b        @ 1080:028b
 *   FUN_1080_02ef        @ 1080:02ef
 *   FUN_1080_0345        @ 1080:0345
 *   FUN_1080_0369        @ 1080:0369
 *   FUN_1080_0382        @ 1080:0382
 *   FUN_1080_03ca        @ 1080:03ca
 *   FUN_1080_03f6        @ 1080:03f6
 *   FUN_1080_0417        @ 1080:0417
 *   FUN_1080_047c        @ 1080:047c
 *   FUN_1080_0492        @ 1080:0492
 *   FUN_1080_04ae        @ 1080:04ae
 *   FUN_1080_052c        @ 1080:052c
 */

/* ======================================================== */
/* Function: FUN_1058_0002 @ 1058:0002 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1058_0002(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6)

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
    FUN_1078_1106(iVar1,uVar2,0,param_3,param_4,param_5,param_6);
    *(undefined2 *)(iVar1 + 0x21) = 0;
    *(undefined2 *)(iVar1 + 0x23) = 0x5400;
    uVar4 = FUN_1080_04ae(0,0,0x1c34,0,0x19,0x19,1,1,0x1204,(char *)s_RandomShrs_10a8_109f + 9,0x7d,
                          iVar1,uVar2);
    *(undefined2 *)(iVar1 + 0x41) = (int)uVar4;
    *(undefined2 *)(iVar1 + 0x43) = (int)((ulong)uVar4 >> 0x10);
    uVar4 = FUN_1080_04ae(0,0,0x1c34,0,0x19,0x19,1,100,0x1209,(char *)s_RandomShrs_10a8_109f + 9,
                          0x7e,iVar1,uVar2);
    *(undefined2 *)(iVar1 + 0x45) = (int)uVar4;
    *(undefined2 *)(iVar1 + 0x47) = (int)((ulong)uVar4 >> 0x10);
    uVar4 = FUN_1080_04ae(0,0,0x1c34,0,0x19,0x19,1,0x96,(char *)s__Buy_Sell_10a8_1210,
                          (char *)s_RandomShrs_10a8_109f + 9,0x7f,iVar1,uVar2);
    *(undefined2 *)(iVar1 + 0x49) = (int)uVar4;
    *(undefined2 *)(iVar1 + 0x4b) = (int)((ulong)uVar4 >> 0x10);
    uVar4 = FUN_1080_04ae(0,0,0x1c34,0,0x19,0x19,1,0xaf,(char *)s_H_ide_Chits_10a8_121a,
                          (char *)s_RandomShrs_10a8_109f + 9,0x80,iVar1,uVar2);
    *(undefined2 *)(iVar1 + 0x4d) = (int)uVar4;
    *(undefined2 *)(iVar1 + 0x4f) = (int)((ulong)uVar4 >> 0x10);
    uVar4 = FUN_1080_04ae(0,0,0x1c34,0,0x19,0x19,1,200,(char *)s__Show_Graph_10a8_1226,
                          (char *)s_RandomShrs_10a8_109f + 9,0xf8,iVar1,uVar2);
    *(undefined2 *)(iVar1 + 0x51) = (int)uVar4;
    *(undefined2 *)(iVar1 + 0x53) = (int)((ulong)uVar4 >> 0x10);
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1058_0123 @ 1058:0123 */
/* ======================================================== */

void __stdcall16far FUN_1058_0123(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_2 >> 0x10);
  FUN_1078_1274((int)param_1,(int)((ulong)param_1 >> 0x10),(int)param_2,uVar2);
  uVar1 = CREATESOLIDBRUSH((char *)s_Blanks_10a8_1072 + 6,DAT_10a8_19ba);
  *(undefined2 *)((int)param_2 + 0x10) = uVar1;
  return;
}

/* ======================================================== */
/* Function: FUN_1058_0152 @ 1058:0152 */
/* ======================================================== */

void __stdcall16far FUN_1058_0152(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x47d);
  return;
}

/* ======================================================== */
/* Function: FUN_1058_0177 @ 1058:0177 */
/* ======================================================== */

void __stdcall16far FUN_1058_0177(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x468);
  return;
}

/* ======================================================== */
/* Function: FUN_1058_019c @ 1058:019c */
/* ======================================================== */

void __stdcall16far FUN_1058_019c(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x47e);
  return;
}

/* ======================================================== */
/* Function: FUN_1058_01c1 @ 1058:01c1 */
/* ======================================================== */

void __stdcall16far FUN_1058_01c1(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x480);
  return;
}

/* ======================================================== */
/* Function: FUN_1058_01e6 @ 1058:01e6 */
/* ======================================================== */

void __stdcall16far FUN_1058_01e6(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x4f0);
  return;
}

/* ======================================================== */
/* Function: FUN_1058_020b @ 1058:020b */
/* ======================================================== */

char * __stdcall16far FUN_1058_020b(void)

{
  return s_RibbonWindowX_10a8_1232;
}

/* ======================================================== */
/* Function: FUN_1058_022b @ 1058:022b */
/* ======================================================== */

void __stdcall16far
FUN_1058_022b(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int unaff_BP;
  int iVar3;
  undefined2 uVar4;
  char *pcVar5;
  
  iVar3 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = 1;
  uVar2 = param_4;
  uVar1 = GETSTOCKOBJECT();
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
  uVar2 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,6,param_4,uVar4,uVar2,pcVar5,iVar3);
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar2);
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  RECTANGLE((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)((int)param_1 + 0x2f),
            *(undefined2 *)((int)param_1 + 0x2d),0,0);
  return;
}

/* ======================================================== */
/* Function: FUN_1058_026e @ 1058:026e */
/* ======================================================== */

void __stdcall16far
FUN_1058_026e(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             int param_5,undefined2 param_6,undefined2 param_7,undefined2 param_8,undefined2 param_9
             ,undefined2 param_10)

{
  undefined4 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int unaff_BP;
  int iVar4;
  undefined2 uVar5;
  char *unaff_CS;
  undefined2 uVar6;
  undefined4 uVar7;
  char *pcVar8;
  
  iVar4 = unaff_BP + 1;
  pcVar8 = (char *)s_RandomShrs_10a8_109f + 9;
  if ((int)DAT_10a8_3afe == 0 && DAT_10a8_3afe._2_2_ == 0) {
    unaff_CS = (char *)s_ShareLimitNumber_10a8_104d + 0xb;
    DAT_10a8_3afe =
         FUN_1058_0002(0,0,0x11b4,(char *)s_Ribbon_Window_10a8_1240,
                       (char *)s_RandomShrs_10a8_109f + 9,param_9,param_10);
    DAT_10a8_3afe =
         (*(code *)*(undefined2 *)(*DAT_10a8_1b2e + 0x34))
                   ((char *)s_ShareLimitNumber_10a8_104d + 0xb,(int *)DAT_10a8_1b2e,
                    (int)((ulong)DAT_10a8_1b2e >> 0x10),DAT_10a8_3afe);
  }
  uVar5 = (undefined2)((ulong)DAT_10a8_3afe >> 0x10);
  if (*(int *)((int)DAT_10a8_3afe + 4) != 0) {
    uVar5 = *(undefined2 *)((int)DAT_10a8_3afe + 4);
    uVar7 = DAT_10a8_3afe;
    MOVEWINDOW(unaff_CS,1,param_1,param_3,param_5 + 4,param_7);
    uVar1 = *(undefined4 *)((int)uVar7 + 0x41);
    uVar6 = 0;
    uVar5 = FUN_10a0_0be7(0,*(undefined2 *)((int)uVar1 + 4),uVar5,uVar7,pcVar8,iVar4);
    uVar2 = FUN_10a0_0be7(uVar5);
    FUN_10a0_0baa(uVar2);
    uVar3 = FUN_10a0_0be7();
    MOVEWINDOW((char *)s_RandomShrs_10a8_109f + 1,1,uVar3,uVar2,uVar5,uVar6);
    uVar1 = *(undefined4 *)((int)uVar7 + 0x45);
    FUN_10a0_0baa(*(undefined2 *)((int)uVar1 + 4));
    uVar5 = FUN_10a0_0be7();
    uVar2 = FUN_10a0_0be7(uVar5);
    uVar3 = FUN_10a0_0be7(uVar2);
    FUN_10a0_0baa(uVar3);
    uVar6 = FUN_10a0_0be7();
    MOVEWINDOW((char *)s_RandomShrs_10a8_109f + 1,1,uVar6,uVar3,uVar2,uVar5);
    uVar1 = *(undefined4 *)((int)uVar7 + 0x49);
    FUN_10a0_0baa(*(undefined2 *)((int)uVar1 + 4));
    uVar5 = FUN_10a0_0be7();
    uVar2 = FUN_10a0_0be7(uVar5);
    uVar3 = FUN_10a0_0be7();
    FUN_10a0_0baa();
    uVar6 = FUN_10a0_0be7();
    MOVEWINDOW((char *)s_RandomShrs_10a8_109f + 1,1,uVar6,uVar3,uVar2,uVar5);
    uVar1 = *(undefined4 *)((int)uVar7 + 0x4d);
    FUN_10a0_0baa(*(undefined2 *)((int)uVar1 + 4));
    uVar5 = FUN_10a0_0be7();
    uVar2 = FUN_10a0_0be7();
    uVar3 = FUN_10a0_0be7();
    FUN_10a0_0baa();
    uVar6 = FUN_10a0_0be7();
    MOVEWINDOW((char *)s_RandomShrs_10a8_109f + 1,1,uVar6,uVar3,uVar2,uVar5);
    uVar1 = *(undefined4 *)((int)uVar7 + 0x51);
    FUN_10a0_0baa(*(undefined2 *)((int)uVar1 + 4));
    uVar5 = FUN_10a0_0be7();
    uVar2 = FUN_10a0_0be7();
    uVar3 = FUN_10a0_0be7();
    FUN_10a0_0baa();
    uVar6 = FUN_10a0_0be7();
    MOVEWINDOW((char *)s_RandomShrs_10a8_109f + 1,1,uVar6,uVar3,uVar2,uVar5);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1058_04ee @ 1058:04ee */
/* ======================================================== */

void __stdcall16far FUN_1058_04ee(char param_1)

{
  if (param_1 == '\0') {
    SETWINDOWTEXT();
  }
  else {
    SETWINDOWTEXT();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1058_052e @ 1058:052e */
/* ======================================================== */

void __cdecl16far FUN_1058_052e(void)

{
  DAT_10a8_3afe._0_2_ = 0;
  DAT_10a8_3afe._2_2_ = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1068_0002 @ 1068:0002 */
/* ======================================================== */

void __stdcall16far
FUN_1068_0002(undefined2 param_1,undefined2 param_2,undefined2 param_3,int *param_4)

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
/* Function: FUN_1068_0164 @ 1068:0164 */
/* ======================================================== */

void __stdcall16far FUN_1068_0164(undefined4 param_1,undefined4 param_2)

{
  undefined2 unaff_CS;
  
  if (*(int *)((int)param_2 + 8) == 0) {
    DAT_10a8_3c9a = 1;
    CHECKDLGBUTTON(unaff_CS,1,0x6b);
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x6c);
    FUN_1068_0002((int)param_1,(int)((ulong)param_1 >> 0x10),0x6a,DAT_10a8_3c50 * 9 + 0x3c4d,
                  (char *)s_RandomShrs_10a8_109f + 9);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1068_01b8 @ 1068:01b8 */
/* ======================================================== */

void __stdcall16far FUN_1068_01b8(undefined4 param_1,undefined4 param_2)

{
  undefined2 unaff_CS;
  
  if (*(int *)((int)param_2 + 8) == 0) {
    DAT_10a8_3c9a = 0;
    CHECKDLGBUTTON(unaff_CS,0,0x6b);
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,1,0x6c);
    FUN_1068_0002((int)param_1,(int)((ulong)param_1 >> 0x10),0x6a,DAT_10a8_3c50 * 9 + 0x3c49,
                  (char *)s_RandomShrs_10a8_109f + 9);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1068_02e5 @ 1068:02e5 */
/* ======================================================== */

void __stdcall16far FUN_1068_02e5(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  
  if (*(int *)((int)param_2 + 8) == 1) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    iVar2 = (int)param_1;
    iVar1 = SENDDLGITEMMESSAGE(unaff_CS,0,0,0,0x407,0x68);
    DAT_10a8_3c50 = *(char *)(iVar2 + iVar1 + 0x26);
    if (DAT_10a8_3c9a == '\0') {
      FUN_1068_0002(iVar2,uVar3,0x6a,DAT_10a8_3c50 * 9 + 0x3c49,(char *)s_RandomShrs_10a8_109f + 9);
    }
    else {
      FUN_1068_0002(iVar2,uVar3,0x6a,DAT_10a8_3c50 * 9 + 0x3c4d,(char *)s_RandomShrs_10a8_109f + 9);
    }
    FUN_1068_0002(iVar2,uVar3,0x6f,DAT_10a8_3c50 * 9 + 0x3c49,(char *)s_RandomShrs_10a8_109f + 9);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1068_037b @ 1068:037b */
/* ======================================================== */

void __stdcall16far FUN_1068_037b(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  int unaff_BP;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  int local_c;
  int local_a;
  uint local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  if (*(int *)((int)param_2 + 8) == 0) {
    uVar3 = (undefined2)param_1;
    uVar4 = (undefined2)((ulong)param_1 >> 0x10);
    if (DAT_10a8_3c9a == '\0') {
      local_8 = 0xffff;
      local_6 = -1;
      FUN_1068_0002(uVar3,uVar4,0x6a,&local_8,unaff_SS);
      if ((local_6 < 1) && ((local_6 < 0 || (local_8 == 0)))) {
        local_c = 0;
        local_a = 0;
        FUN_1068_0002(uVar3,uVar4,0x6a,&local_c,unaff_SS);
      }
      else {
        iVar1 = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4b);
        if ((iVar1 < local_6) ||
           ((iVar1 <= local_6 && (*(uint *)(DAT_10a8_3c50 * 9 + 0x3c49) < local_8)))) {
          uVar2 = *(uint *)(DAT_10a8_3c50 * 9 + 0x3c49);
          local_8 = uVar2 + 1;
          local_6 = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4b) + (uint)(0xfffe < uVar2);
        }
        local_c = local_8 - 1;
        local_a = local_6 - (uint)(local_8 == 0);
        FUN_1068_0002(uVar3,uVar4,0x6a,&local_c,unaff_SS);
      }
      SENDDLGITEMMESSAGE((char *)s_PurchaseLimitNumber_10a8_105e + 10,10,0,0,0x401,0x6a);
    }
    else {
      local_8 = 0xffff;
      local_6 = -1;
      FUN_1068_0002(uVar3,uVar4,0x6a,&local_8,unaff_SS);
      if ((local_6 < 1) && ((local_6 < 0 || (local_8 == 0)))) {
        local_c = 0;
        local_a = 0;
        FUN_1068_0002(uVar3,uVar4,0x6a,&local_c,unaff_SS);
      }
      else {
        local_c = local_8 - 1;
        local_a = local_6 - (uint)(local_8 == 0);
        FUN_1068_0002(uVar3,uVar4,0x6a,&local_c,unaff_SS);
      }
      SENDDLGITEMMESSAGE((char *)s_PurchaseLimitNumber_10a8_105e + 10,10,0,0,0x401,0x6a);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1068_04d6 @ 1068:04d6 */
/* ======================================================== */

void __stdcall16far FUN_1068_04d6(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int unaff_BP;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  int local_c;
  int local_a;
  uint local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  if (*(int *)((int)param_2 + 8) == 0) {
    uVar2 = (undefined2)param_1;
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    if (DAT_10a8_3c9a == '\0') {
      local_8 = 0xffff;
      local_6 = -1;
      FUN_1068_0002(uVar2,uVar3,0x6a,&local_8,unaff_SS);
      if ((local_6 < 1) && ((local_6 < 0 || (local_8 == 0)))) {
        local_c = 1;
        local_a = 0;
        FUN_1068_0002(uVar2,uVar3,0x6a,&local_c,unaff_SS);
      }
      else {
        iVar1 = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4b);
        if ((iVar1 < local_6) ||
           ((iVar1 <= local_6 && (*(uint *)(DAT_10a8_3c50 * 9 + 0x3c49) <= local_8)))) {
          iVar1 = *(int *)(DAT_10a8_3c50 * 9 + 0x3c49);
          local_8 = iVar1 - 1;
          local_6 = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4b) - (uint)(iVar1 == 0);
        }
        local_c = local_8 + 1;
        local_a = local_6 + (uint)(0xfffe < local_8);
        FUN_1068_0002(uVar2,uVar3,0x6a,&local_c,unaff_SS);
      }
      SENDDLGITEMMESSAGE((char *)s_PurchaseLimitNumber_10a8_105e + 10,10,0,0,0x401,0x6a);
    }
    else {
      local_8 = 0xffff;
      local_6 = -1;
      FUN_1068_0002(uVar2,uVar3,0x6a,&local_8,unaff_SS);
      iVar1 = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4f);
      if ((local_6 < iVar1) ||
         ((local_6 <= iVar1 && (local_8 < *(uint *)(DAT_10a8_3c50 * 9 + 0x3c4d))))) {
        local_c = local_8 + 1;
        local_a = local_6 + (uint)(0xfffe < local_8);
        FUN_1068_0002(uVar2,uVar3,0x6a,&local_c,unaff_SS);
      }
      else {
        local_c = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4d);
        local_a = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4f);
        FUN_1068_0002(uVar2,uVar3,0x6a,&local_c,unaff_SS);
      }
      SENDDLGITEMMESSAGE((char *)s_PurchaseLimitNumber_10a8_105e + 10,10,0,0,0x401,0x6a);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1068_064d @ 1068:064d */
/* ======================================================== */

void __stdcall16far FUN_1068_064d(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  char *pcVar3;
  char *pcVar4;
  byte local_6;
  char local_5;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  FUN_1078_0d9b(iVar1,uVar2);
  DAT_10a8_3c9a = 1;
  local_6 = 1;
  if ((DAT_10a8_3c50 == '\0') || (DAT_10a8_3c50 == '\t')) {
    DAT_10a8_3c50 = '\x01';
  }
  DAT_10a8_3c9c = *(undefined2 *)(DAT_10a8_3c50 * 9 + 0x3c4d);
  DAT_10a8_3c9e = *(undefined2 *)(DAT_10a8_3c50 * 9 + 0x3c4f);
  CHECKDLGBUTTON((char *)s_Blanks_10a8_1072 + 6,1,0x6b);
  CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x6c);
  FUN_1068_0002(iVar1,uVar2,0x6a,(undefined2 *)&DAT_10a8_3c9c,(char *)s_RandomShrs_10a8_109f + 9);
  local_5 = '\x01';
  pcVar3 = (char *)s_PurchaseLimitNumber_10a8_105e + 10;
  while( true ) {
    pcVar4 = pcVar3;
    if (((undefined1 *)&DAT_10a8_3c51)[local_5 * 9] != '\0') {
      pcVar4 = (char *)s__Show_Graph_10a8_1226 + 10;
      SENDDLGITEMMESSAGE(pcVar3,*(undefined2 *)
                                 ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 +
                                 local_5 * 4),
                         *(undefined2 *)
                          ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + local_5 * 4),0,
                         0x403,0x68);
      *(char *)(iVar1 + (uint)local_6 + 0x25) = local_5;
      local_6 = local_6 + 1;
    }
    if (local_5 == '\b') break;
    local_5 = local_5 + '\x01';
    pcVar3 = pcVar4;
  }
  FUN_1068_0002(iVar1,uVar2,0x6f,DAT_10a8_3c50 * 9 + 0x3c49,(char *)s_RandomShrs_10a8_109f + 9);
  SENDDLGITEMMESSAGE((char *)s_PurchaseLimitNumber_10a8_105e + 10,
                     *(undefined2 *)
                      ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 + DAT_10a8_3c50 * 4),
                     *(undefined2 *)
                      ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + DAT_10a8_3c50 * 4),0,
                     0x40d,0x68);
  return;
}

/* ======================================================== */
/* Function: FUN_1068_0761 @ 1068:0761 */
/* ======================================================== */

undefined1 __stdcall16far FUN_1068_0761(undefined1 *param_1)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_14c [19];
  undefined1 local_126 [290];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = local_126;
  puVar4 = (undefined1 *)param_1;
  for (iVar3 = 0x120; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  FUN_10a0_0b92(0x120,0x3b30,(char *)s_RandomShrs_10a8_109f + 9,local_126,unaff_SS);
  uVar6 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  FUN_1080_0002(local_14c,unaff_SS,0x13e4,(char *)s_CHOOSE_COMPANY_10a8_1486,
                (char *)s_RandomShrs_10a8_109f + 9,*(undefined2 *)((int)DAT_10a8_1b2e + 8),
                *(undefined2 *)((int)DAT_10a8_1b2e + 10));
  (*(code *)*(undefined2 *)(local_14c[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_14c);
  (*(code *)*(undefined2 *)(local_14c[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_14c);
  return DAT_10a8_3c51;
}

/* ======================================================== */
/* Function: FUN_1068_07dd @ 1068:07dd */
/* ======================================================== */

void __stdcall16far FUN_1068_07dd(undefined4 param_1)

{
  char *pcVar1;
  bool bVar2;
  char local_5;
  
  FUN_1078_0d9b((int)param_1,(int)((ulong)param_1 >> 0x10));
  local_5 = '\x01';
  while( true ) {
    bVar2 = true;
    pcVar1 = (char *)s_RandomShrs_10a8_109f + 1;
    FUN_10a0_1454();
    if (bVar2) {
      pcVar1 = (char *)s__Show_Graph_10a8_1226 + 10;
      SENDDLGITEMMESSAGE((char *)s_RandomShrs_10a8_109f + 1,
                         *(undefined2 *)
                          ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 + local_5 * 4),
                         *(undefined2 *)
                          ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + local_5 * 4),0,
                         0x403,0x66);
    }
    if (local_5 == '\b') break;
    local_5 = local_5 + '\x01';
  }
  SENDDLGITEMMESSAGE(pcVar1,0,0,0,0x40e,0x66);
  return;
}

/* ======================================================== */
/* Function: FUN_1068_0860 @ 1068:0860 */
/* ======================================================== */

char * __stdcall16far FUN_1068_0860(void)

{
  return s_Choice_Dialog_10a8_1496;
}

/* ======================================================== */
/* Function: FUN_1068_0880 @ 1068:0880 */
/* ======================================================== */

void __stdcall16far FUN_1068_0880(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 2) {
    FUN_10a0_1e88();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1068_08a8 @ 1068:08a8 */
/* ======================================================== */

void __stdcall16far FUN_1068_08a8(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  bool bVar2;
  long lVar3;
  undefined2 uVar4;
  char local_d;
  uint local_c;
  int local_a;
  
  local_c = 0;
  local_a = 0;
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  uVar4 = *(undefined2 *)((int)param_1 + 4);
  lVar3 = SENDDLGITEMMESSAGE(unaff_CS,0,0,0,0x407,0x66);
  if (lVar3 == -1) {
    lVar3 = 0;
  }
  local_d = '\x01';
  while( true ) {
    bVar2 = true;
    FUN_10a0_1454(uVar4);
    if ((bVar2) && (lVar3 == CONCAT22(local_a,local_c))) {
      DAT_10a8_3c51 = local_d;
    }
    bVar2 = true;
    FUN_10a0_1454(uVar4);
    if (bVar2) {
      bVar2 = 0xfffe < local_c;
      local_c = local_c + 1;
      local_a = local_a + (uint)bVar2;
    }
    if (local_d == '\b') break;
    local_d = local_d + '\x01';
  }
  FUN_1080_0382((int)param_1,uVar1,(int)param_2,(int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0002 @ 1070:0002 */
/* ======================================================== */

void __stdcall16far FUN_1070_0002(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  char *pcVar4;
  char *pcVar5;
  
  DAT_10a8_3daf = '\0';
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  FUN_10a0_0b92(0x26,0x3db0,(char *)s_RandomShrs_10a8_109f + 9,iVar1,uVar2);
  uVar3 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  pcVar4 = (char *)s_PayDividends_10a8_1079 + 7;
  FUN_1080_0002((undefined2 *)&DAT_10a8_3cc8,(char *)s_RandomShrs_10a8_109f + 9,0x15da,
                (char *)s_OPTIONS_10a8_16ee,(char *)s_RandomShrs_10a8_109f + 9,
                *(undefined2 *)((int)DAT_10a8_1b2e + 8),*(undefined2 *)((int)DAT_10a8_1b2e + 10));
  (*(code *)*(undefined2 *)(DAT_10a8_3cc8 + 0x4c))
            ((char *)s_PayDividends_10a8_1079 + 7,(undefined2 *)&DAT_10a8_3cc8,
             (char *)s_RandomShrs_10a8_109f + 9);
  (*(code *)*(undefined2 *)(DAT_10a8_3cc8 + 8))
            ((char *)s_PayDividends_10a8_1079 + 7,(undefined2 *)&DAT_10a8_3cc8,
             (char *)s_RandomShrs_10a8_109f + 9,0);
  if (DAT_10a8_3daf == '\0') {
    *(undefined1 *)(iVar1 + 0x1e) = 1;
  }
  else {
    *(undefined1 *)(iVar1 + 0x1e) = 0;
    if ((DAT_10a8_3dba < 0) ||
       ((((DAT_10a8_3dba < 1 && (DAT_10a8_3db8 < 0xc)) || (DAT_10a8_3dbe < 0)) ||
        ((DAT_10a8_3dbe < 1 && (DAT_10a8_3dbc < 0xc)))))) {
      pcVar4 = (char *)s__Show_Graph_10a8_1226 + 10;
      MESSAGEBOX((char *)s_PayDividends_10a8_1079 + 7,0x30,(char *)s_Options_10a8_171c,
                 (char *)s_RandomShrs_10a8_109f + 9,
                 (char *)s_Rows_or_Columns_must_be_12_or_gr_10a8_16f6,
                 (char *)s_RandomShrs_10a8_109f + 9);
    }
    if ((((0 < DAT_10a8_3dba) || ((-1 < DAT_10a8_3dba && (0x1a < DAT_10a8_3db8)))) ||
        (0 < DAT_10a8_3dbe)) || ((pcVar5 = pcVar4, -1 < DAT_10a8_3dbe && (0x1a < DAT_10a8_3dbc)))) {
      pcVar5 = (char *)s__Show_Graph_10a8_1226 + 10;
      MESSAGEBOX(pcVar4,0x30,(char *)s_Options_10a8_171c,(char *)s_RandomShrs_10a8_109f + 9,
                 (char *)s_Rows_or_Columns_too_large_10a8_1724,(char *)s_RandomShrs_10a8_109f + 9);
    }
    if ((DAT_10a8_3db6 < 0) || ((pcVar4 = pcVar5, DAT_10a8_3db6 < 1 && (DAT_10a8_3db4 == 0)))) {
      pcVar4 = (char *)s__Show_Graph_10a8_1226 + 10;
      MESSAGEBOX(pcVar5,0,(char *)s_Too_Few_Chits_10a8_175c,(char *)s_RandomShrs_10a8_109f + 9,
                 (char *)s_Minimum_Number_of_Chits_is_1__10a8_173e,
                 (char *)s_RandomShrs_10a8_109f + 9);
      DAT_10a8_3db4 = 1;
      DAT_10a8_3db6 = 0;
    }
    if ((0 < DAT_10a8_3db6) || ((pcVar5 = pcVar4, -1 < DAT_10a8_3db6 && (0xe < DAT_10a8_3db4)))) {
      pcVar5 = (char *)s__Show_Graph_10a8_1226 + 10;
      MESSAGEBOX(pcVar4,0,(char *)s_Too_Many_Chits_10a8_1789,(char *)s_RandomShrs_10a8_109f + 9,
                 (char *)s_Maximum_Number_of_Chits_is_14__10a8_176a,
                 (char *)s_RandomShrs_10a8_109f + 9);
      DAT_10a8_3db4 = 0xe;
      DAT_10a8_3db6 = 0;
    }
    if ((DAT_10a8_3dba < 0) || ((DAT_10a8_3dba < 1 && (DAT_10a8_3db8 < 0xc)))) {
      DAT_10a8_3db8 = 0xc;
      DAT_10a8_3dba = 0;
    }
    if ((DAT_10a8_3dbe < 0) || ((DAT_10a8_3dbe < 1 && (DAT_10a8_3dbc < 0xc)))) {
      DAT_10a8_3dbc = 0xc;
      DAT_10a8_3dbe = 0;
    }
    if ((0 < DAT_10a8_3dba) || ((-1 < DAT_10a8_3dba && (0x1a < DAT_10a8_3db8)))) {
      DAT_10a8_3db8 = 0x1a;
      DAT_10a8_3dba = 0;
    }
    if ((0 < DAT_10a8_3dbe) || ((-1 < DAT_10a8_3dbe && (0x1a < DAT_10a8_3dbc)))) {
      DAT_10a8_3dbc = 0x1a;
      DAT_10a8_3dbe = 0;
    }
    if ((DAT_10a8_3dc6 < 0) || ((pcVar4 = pcVar5, DAT_10a8_3dc6 < 1 && (DAT_10a8_3dc4 < 500)))) {
      pcVar4 = (char *)s__Show_Graph_10a8_1226 + 10;
      MESSAGEBOX(pcVar5,0,(char *)s_Small_Share_Limit_10a8_17b9,(char *)s_RandomShrs_10a8_109f + 9,
                 (char *)s_Share_Limit_Must_Be_At_Least_500_10a8_1798,
                 (char *)s_RandomShrs_10a8_109f + 9);
      DAT_10a8_3dc4 = 500;
      DAT_10a8_3dc6 = 0;
    }
    if ((0x98 < DAT_10a8_3dc6) ||
       ((pcVar5 = pcVar4, 0x97 < DAT_10a8_3dc6 && (0x9680 < DAT_10a8_3dc4)))) {
      pcVar5 = (char *)s__Show_Graph_10a8_1226 + 10;
      MESSAGEBOX(pcVar4,0,(char *)s_Large_Share_Limit_10a8_17eb,(char *)s_RandomShrs_10a8_109f + 9,
                 (char *)s_Share_Limit_Number_Is_Too_Large_10a8_17cb,
                 (char *)s_RandomShrs_10a8_109f + 9);
      DAT_10a8_3dc4 = 0x9680;
      DAT_10a8_3dc6 = 0x98;
    }
    if (DAT_10a8_3dd2 != '\0') {
      if ((DAT_10a8_3dca < 0) || ((pcVar4 = pcVar5, DAT_10a8_3dca < 1 && (DAT_10a8_3dc8 < 100)))) {
        pcVar4 = (char *)s__Show_Graph_10a8_1226 + 10;
        MESSAGEBOX(pcVar5,0,(char *)s_Small_Purchase_Limit_10a8_1821,
                   (char *)s_RandomShrs_10a8_109f + 9,
                   (char *)s_Purchase_Limit_Must_Be_At_Least_1_10a8_17fd,
                   (char *)s_RandomShrs_10a8_109f + 9);
        DAT_10a8_3dc8 = 100;
        DAT_10a8_3dca = 0;
      }
      if ((DAT_10a8_3dc6 < DAT_10a8_3dca) ||
         ((pcVar5 = pcVar4, DAT_10a8_3dc6 <= DAT_10a8_3dca && (DAT_10a8_3dc4 < DAT_10a8_3dc8)))) {
        pcVar5 = (char *)s__Show_Graph_10a8_1226 + 10;
        MESSAGEBOX(pcVar4,0,(char *)s_Large_Purchase_Limit_10a8_185d,
                   (char *)s_RandomShrs_10a8_109f + 9,
                   (char *)s_Purchase_Limit_Larger_Than_Share_10a8_1836,
                   (char *)s_RandomShrs_10a8_109f + 9);
        DAT_10a8_3dc8 = DAT_10a8_3dc4;
        DAT_10a8_3dca = DAT_10a8_3dc6;
      }
    }
    if ((DAT_10a8_3dc2 < 0) || ((DAT_10a8_3dc2 < 1 && (DAT_10a8_3dc0 < 0xfa)))) {
      MESSAGEBOX(pcVar5,0x30,(char *)s_Improper_Cash_Entry_10a8_1887,
                 (char *)s_RandomShrs_10a8_109f + 9,(char *)s_Cash_Minimum_of__250_10a8_1872,
                 (char *)s_RandomShrs_10a8_109f + 9);
      DAT_10a8_3dc0 = 0xfa;
      DAT_10a8_3dc2 = 0;
    }
    FUN_10a0_0b92(0x26,iVar1,uVar2,0x3db0,(char *)s_RandomShrs_10a8_109f + 9);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_02d3 @ 1070:02d3 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1070_02d3(undefined4 param_1)

{
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  bool bVar4;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  FUN_10a0_0b92(0xc0,(undefined1 *)&DAT_10a8_3cee,(char *)s_RandomShrs_10a8_109f + 9,(int)param_1,
                uVar2);
  DAT_10a8_3dae = '\0';
  uVar3 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  FUN_1080_0002((undefined2 *)&DAT_10a8_3ca2,(char *)s_RandomShrs_10a8_109f + 9,0x1542,
                (char *)s_NEW_GAME_10a8_189c,(char *)s_RandomShrs_10a8_109f + 9,
                *(undefined2 *)((int)DAT_10a8_1b2e + 8),*(undefined2 *)((int)DAT_10a8_1b2e + 10));
  (*(code *)*(undefined2 *)(DAT_10a8_3ca2 + 0x4c))
            ((char *)s_PayDividends_10a8_1079 + 7,(undefined2 *)&DAT_10a8_3ca2,
             (char *)s_RandomShrs_10a8_109f + 9);
  (*(code *)*(undefined2 *)(DAT_10a8_3ca2 + 8))
            ((char *)s_PayDividends_10a8_1079 + 7,(undefined2 *)&DAT_10a8_3ca2,
             (char *)s_RandomShrs_10a8_109f + 9,0);
  bVar4 = DAT_10a8_3dae != '\0';
  uVar1 = extraout_AH;
  if (bVar4) {
    FUN_10a0_0b92(0xc0,(int)param_1,uVar2,(undefined1 *)&DAT_10a8_3cee,
                  (char *)s_RandomShrs_10a8_109f + 9);
    uVar1 = extraout_AH_00;
  }
  return CONCAT11(uVar1,bVar4);
}

/* ======================================================== */
/* Function: FUN_1070_0355 @ 1070:0355 */
/* ======================================================== */

void __stdcall16far FUN_1070_0355(undefined4 param_1,undefined4 param_2)

{
  undefined2 unaff_CS;
  
  DAT_10a8_3dae = 1;
  GETDLGITEMTEXT(unaff_CS,0x1f,0x3cef,(char *)s_RandomShrs_10a8_109f + 9,0x6b);
  GETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x1f,0x3d0f,(char *)s_RandomShrs_10a8_109f + 9
                 ,0x6c);
  GETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x1f,0x3d2f,(char *)s_RandomShrs_10a8_109f + 9
                 ,0x6d);
  GETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x1f,0x3d4f,(char *)s_RandomShrs_10a8_109f + 9
                 ,0x6e);
  GETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x1f,0x3d6f,(char *)s_RandomShrs_10a8_109f + 9
                 ,0x6f);
  GETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x1f,0x3d8f,(char *)s_RandomShrs_10a8_109f + 9
                 ,0x70);
  FUN_1080_0382((int)param_1,(int)((ulong)param_1 >> 0x10),(int)param_2,
                (int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1070_03f3 @ 1070:03f3 */
/* ======================================================== */

void __stdcall16far FUN_1070_03f3(undefined4 param_1)

{
  undefined2 unaff_CS;
  
  CHECKRADIOBUTTON(unaff_CS,DAT_10a8_3cee + 200,0xcb,200);
  CHECKRADIOBUTTON((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_3d0e + 0xd2,0xd5,0xd2);
  CHECKRADIOBUTTON((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_3d2e + 0xdc,0xdf,0xdc);
  CHECKRADIOBUTTON((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_3d4e + 0xe6,0xe9,0xe6);
  CHECKRADIOBUTTON((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_3d6e + 0xf0,0xf3,0xf0);
  CHECKRADIOBUTTON((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_3d8e + 0xfa,0xfd,0xfa);
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x3cef,(char *)s_RandomShrs_10a8_109f + 9,0x6b
                );
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x3d0f,(char *)s_RandomShrs_10a8_109f + 9,0x6c
                );
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x3d2f,(char *)s_RandomShrs_10a8_109f + 9,0x6d
                );
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x3d4f,(char *)s_RandomShrs_10a8_109f + 9,0x6e
                );
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x3d6f,(char *)s_RandomShrs_10a8_109f + 9,0x6f
                );
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x3d8f,(char *)s_RandomShrs_10a8_109f + 9,0x70
                );
  FUN_1078_0d9b((int)param_1,(int)((ulong)param_1 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0517 @ 1070:0517 */
/* ======================================================== */

void __stdcall16far FUN_1070_0517(int *param_1,undefined4 param_2)

{
  int unaff_BP;
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  int iVar4;
  int iVar5;
  int iVar6;
  char *pcVar7;
  
  iVar1 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  DAT_10a8_3daf = 1;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  iVar6 = piVar2[2];
  DAT_10a8_3db4 = GETDLGITEMINT(unaff_CS,0,0,0,0x68);
  DAT_10a8_3db6 = 0;
  iVar5 = piVar2[2];
  DAT_10a8_3db8 = GETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,0,0,0x6b);
  DAT_10a8_3dba = 0;
  iVar4 = piVar2[2];
  DAT_10a8_3dbc = GETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,0,0,0x6c);
  DAT_10a8_3dbe = 0;
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,0,0x6e,(undefined2 *)&DAT_10a8_3dc0,
             (char *)s_RandomShrs_10a8_109f + 9,iVar4,iVar5,iVar6,pcVar7,iVar1);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,0,0x9d,(undefined2 *)&DAT_10a8_3dc4,
             (char *)s_RandomShrs_10a8_109f + 9);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,0,0x73,(undefined2 *)&DAT_10a8_3dc8,
             (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1080_0382(piVar2,uVar3,(int)param_2,(int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1070_05cb @ 1070:05cb */
/* ======================================================== */

void __stdcall16far FUN_1070_05cb(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3dcc = DAT_10a8_3dcc == '\0';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_05ee @ 1070:05ee */
/* ======================================================== */

void __stdcall16far FUN_1070_05ee(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3dcd = DAT_10a8_3dcd == '\0';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0611 @ 1070:0611 */
/* ======================================================== */

void __stdcall16far FUN_1070_0611(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3dd0 = DAT_10a8_3dd0 == '\0';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0634 @ 1070:0634 */
/* ======================================================== */

void __stdcall16far FUN_1070_0634(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3dd1 = DAT_10a8_3dd1 == '\0';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0657 @ 1070:0657 */
/* ======================================================== */

void __stdcall16far FUN_1070_0657(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3dcf = DAT_10a8_3dcf == '\0';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_067a @ 1070:067a */
/* ======================================================== */

void __stdcall16far FUN_1070_067a(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3dd2 = DAT_10a8_3dd2 == '\0';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_069d @ 1070:069d */
/* ======================================================== */

void __stdcall16far FUN_1070_069d(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3dd3 = DAT_10a8_3dd3 == '\0';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_06c0 @ 1070:06c0 */
/* ======================================================== */

void __stdcall16far
FUN_1070_06c0(undefined2 param_1,undefined2 param_2,char param_3,undefined2 param_4,
             undefined2 *param_5)

{
  int unaff_BP;
  undefined2 uVar1;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined4 uVar2;
  undefined1 local_26 [2];
  undefined1 local_24 [32];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar1 = (undefined2)((ulong)param_5 >> 0x10);
  if (param_3 == '\0') {
    GETDLGITEMTEXT(unaff_CS,10,local_24,unaff_SS,param_4);
    uVar2 = FUN_10a0_0abf(local_26,unaff_SS,local_24,unaff_SS);
    *param_5 = (int)uVar2;
    ((undefined2 *)param_5)[1] = (int)((ulong)uVar2 >> 0x10);
  }
  else {
    FUN_10a0_0a72(0x1e,local_24,unaff_SS,0,*param_5,((undefined2 *)param_5)[1]);
    SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_24,unaff_SS,param_4);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0732 @ 1070:0732 */
/* ======================================================== */

void __stdcall16far FUN_1070_0732(void)

{
  undefined2 unaff_CS;
  
  WINHELP(unaff_CS,DAT_10a8_18a6,DAT_10a8_18a8,0x101,(char *)s_OILBARON_HLP_10a8_18b2,
          (char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1070_075a @ 1070:075a */
/* ======================================================== */

void __stdcall16far FUN_1070_075a(int *param_1)

{
  int unaff_BP;
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  int iVar4;
  int iVar5;
  int iVar6;
  char *pcVar7;
  
  iVar1 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  iVar6 = piVar2[2];
  SETDLGITEMINT(unaff_CS,0,DAT_10a8_3db4,0x68);
  iVar5 = piVar2[2];
  SETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,DAT_10a8_3db8,0x6b);
  iVar4 = piVar2[2];
  SETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,DAT_10a8_3dbc,0x6c);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,1,0x6e,(undefined2 *)&DAT_10a8_3dc0,
             (char *)s_RandomShrs_10a8_109f + 9,iVar4,iVar5,iVar6,pcVar7,iVar1);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,1,0x9d,(undefined2 *)&DAT_10a8_3dc4,
             (char *)s_RandomShrs_10a8_109f + 9);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,1,0x73,(undefined2 *)&DAT_10a8_3dc8,
             (char *)s_RandomShrs_10a8_109f + 9);
  if (DAT_10a8_3dd2 == '\0') {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x72);
  }
  else {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,1,0x72);
  }
  if (DAT_10a8_3dcc == '\0') {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x70);
  }
  else {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,1,0x70);
  }
  if (DAT_10a8_3dcd == '\0') {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x6f);
  }
  else {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,1,0x6f);
  }
  if (DAT_10a8_3dcf == '\0') {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x131);
  }
  else {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,1,0x131);
  }
  if (DAT_10a8_3dd0 == '\0') {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x67);
  }
  else {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,1,0x67);
  }
  if (DAT_10a8_3dd1 == '\0') {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x71);
  }
  else {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,1,0x71);
  }
  if (DAT_10a8_3dd3 == '\0') {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,0,0x82);
  }
  else {
    CHECKDLGBUTTON((char *)s__Show_Graph_10a8_1226 + 10,1,0x82);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0902 @ 1070:0902 */
/* ======================================================== */

void __stdcall16far FUN_1070_0902(int *param_1)

{
  int unaff_BP;
  int iVar1;
  char *pcVar2;
  
  iVar1 = unaff_BP + 1;
  pcVar2 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_10a0_0b92(0x26,0x3db0,(char *)s_RandomShrs_10a8_109f + 9,0x19c6,
                (char *)s_RandomShrs_10a8_109f + 9);
  (*(code *)*(undefined2 *)(*param_1 + 0x38))
            ((char *)s_RandomShrs_10a8_109f + 1,(int *)param_1,(int)((ulong)param_1 >> 0x10),pcVar2,
             iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1070_092a @ 1070:092a */
/* ======================================================== */

void __stdcall16far FUN_1070_092a(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3cee = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0945 @ 1070:0945 */
/* ======================================================== */

void __stdcall16far FUN_1070_0945(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3cee = 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0960 @ 1070:0960 */
/* ======================================================== */

void __stdcall16far FUN_1070_0960(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3cee = 2;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_097b @ 1070:097b */
/* ======================================================== */

void __stdcall16far FUN_1070_097b(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3cee = 3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0996 @ 1070:0996 */
/* ======================================================== */

void __stdcall16far FUN_1070_0996(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d0e = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_09b1 @ 1070:09b1 */
/* ======================================================== */

void __stdcall16far FUN_1070_09b1(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d0e = 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_09cc @ 1070:09cc */
/* ======================================================== */

void __stdcall16far FUN_1070_09cc(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d0e = 2;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_09e7 @ 1070:09e7 */
/* ======================================================== */

void __stdcall16far FUN_1070_09e7(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d0e = 3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0a02 @ 1070:0a02 */
/* ======================================================== */

void __stdcall16far FUN_1070_0a02(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d2e = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0a1d @ 1070:0a1d */
/* ======================================================== */

void __stdcall16far FUN_1070_0a1d(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d2e = 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0a38 @ 1070:0a38 */
/* ======================================================== */

void __stdcall16far FUN_1070_0a38(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d2e = 2;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0a53 @ 1070:0a53 */
/* ======================================================== */

void __stdcall16far FUN_1070_0a53(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d2e = 3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0a6e @ 1070:0a6e */
/* ======================================================== */

void __stdcall16far FUN_1070_0a6e(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d4e = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0a89 @ 1070:0a89 */
/* ======================================================== */

void __stdcall16far FUN_1070_0a89(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d4e = 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0aa4 @ 1070:0aa4 */
/* ======================================================== */

void __stdcall16far FUN_1070_0aa4(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d4e = 2;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0abf @ 1070:0abf */
/* ======================================================== */

void __stdcall16far FUN_1070_0abf(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d4e = 3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0ada @ 1070:0ada */
/* ======================================================== */

void __stdcall16far FUN_1070_0ada(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d6e = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0af5 @ 1070:0af5 */
/* ======================================================== */

void __stdcall16far FUN_1070_0af5(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d6e = 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0b10 @ 1070:0b10 */
/* ======================================================== */

void __stdcall16far FUN_1070_0b10(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d6e = 2;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0b2b @ 1070:0b2b */
/* ======================================================== */

void __stdcall16far FUN_1070_0b2b(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d6e = 3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0b46 @ 1070:0b46 */
/* ======================================================== */

void __stdcall16far FUN_1070_0b46(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d8e = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0b61 @ 1070:0b61 */
/* ======================================================== */

void __stdcall16far FUN_1070_0b61(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d8e = 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0b7c @ 1070:0b7c */
/* ======================================================== */

void __stdcall16far FUN_1070_0b7c(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d8e = 2;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0b97 @ 1070:0b97 */
/* ======================================================== */

void __stdcall16far FUN_1070_0b97(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if (*(int *)((int)param_3 + 8) == 0) {
    DAT_10a8_3d8e = 3;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0bb2 @ 1070:0bb2 */
/* ======================================================== */

void __stdcall16far FUN_1070_0bb2(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  if (*(char *)(iVar1 + 0x26) != '\0') {
    SETDLGITEMTEXT(unaff_CS,(char *)s_This_Copy_Registered_To__10a8_18c0,
                   (char *)s_RandomShrs_10a8_109f + 9,0x69);
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,
                   (char *)s_Thank_you_for_Registering__10a8_18d9,(char *)s_RandomShrs_10a8_109f + 9
                   ,0x6f);
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,iVar1 + 0x27,uVar2,0x6c);
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,iVar1 + 0xbe,uVar2,0x6d);
    SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,
                   (char *)s_This_Copy_Registered_To__10a8_18c0 + 0x18,
                   (char *)s_RandomShrs_10a8_109f + 9,0x6a);
  }
  FUN_1078_0d9b(iVar1,uVar2);
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0c30 @ 1070:0c30 */
/* ======================================================== */

void __stdcall16far FUN_1070_0c30(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  GETDLGITEMTEXT(unaff_CS,0x97,iVar2 + 0x26,uVar3,0x68);
  GETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x97,iVar2 + 0xbd,uVar3,0x69);
  uVar1 = GETDLGITEMINT((char *)s__Show_Graph_10a8_1226 + 10,0,0,0,0x66);
  *(undefined2 *)(iVar2 + 0x154) = uVar1;
  *(undefined2 *)(iVar2 + 0x156) = 0;
  FUN_1080_0382(iVar2,uVar3,(int)param_2,(int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1070_0c9c @ 1070:0c9c */
/* ======================================================== */

void __stdcall16far FUN_1070_0c9c(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  *(undefined1 *)((int)param_1 + 0x158) = 1;
  FUN_1080_03ca((int)param_1,uVar1,(int)param_2,(int)((ulong)param_2 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1080_0002 @ 1080:0002 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1080_0002(undefined4 param_1,undefined2 param_2,undefined2 param_3,int param_4,
             undefined2 param_5,undefined2 param_6)

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
    FUN_1078_0341(iVar1,uVar2,0,param_5,param_6);
    FUN_1078_0627(iVar1,uVar2);
    if (param_4 == 0) {
      *(undefined2 *)(iVar1 + 0x1d) = param_3;
      *(undefined2 *)(iVar1 + 0x1f) = 0;
    }
    else {
      uVar4 = FUN_1098_015d(param_3,param_4);
      *(undefined2 *)(iVar1 + 0x1d) = (int)uVar4;
      *(undefined2 *)(iVar1 + 0x1f) = (int)((ulong)uVar4 >> 0x10);
    }
    *(undefined2 *)(iVar1 + 0x21) = 0;
    *(undefined2 *)(iVar1 + 0x23) = 0;
    *(undefined1 *)(iVar1 + 0x25) = 0;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1080_007a @ 1080:007a */
/* ======================================================== */

void __stdcall16far FUN_1080_007a(undefined4 param_1)

{
  int unaff_BP;
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  char *pcVar4;
  
  iVar1 = unaff_BP + 1;
  pcVar4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  if (*(int *)(iVar2 + 0x1f) != 0) {
    FUN_1098_01ca(*(undefined2 *)(iVar2 + 0x1d),*(undefined2 *)(iVar2 + 0x1f));
  }
  FUN_1078_03ff(iVar2,uVar3,0);
  FUN_10a0_0439(pcVar4,iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1080_00b1 @ 1080:00b1 */
/* ======================================================== */

uint __stdcall16far FUN_1080_00b1(undefined4 param_1)

{
  uint in_AX;
  uint uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 local_8;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  if (*(int *)(iVar2 + 2) == 0) {
    FUN_1078_0627(iVar2,uVar3);
    FUN_1078_05f3(iVar2,uVar3);
    *(undefined1 *)(iVar2 + 0x25) = 0;
    if (*(int *)(iVar2 + 6) == 0 && *(int *)(iVar2 + 8) == 0) {
      local_8 = 0;
    }
    else {
      local_8 = *(undefined2 *)((int)*(undefined4 *)(iVar2 + 6) + 4);
    }
    in_AX = (*DAT_10a8_1b32)((char *)s_Blanks_10a8_1072 + 6,*(undefined2 *)(iVar2 + 0x21),
                             *(undefined2 *)(iVar2 + 0x23),*(undefined2 *)(iVar2 + 0x12),
                             *(undefined2 *)(iVar2 + 0x14),local_8,*(undefined2 *)(iVar2 + 0x1d),
                             *(undefined2 *)(iVar2 + 0x1f),DAT_10a8_1c9e);
    *(uint *)(iVar2 + 4) = in_AX;
    if (*(int *)(iVar2 + 4) == 0) {
      *(undefined2 *)(iVar2 + 2) = 0xffff;
    }
  }
  uVar1 = in_AX & 0xff00;
  if (*(int *)(iVar2 + 2) == 0) {
    uVar1 = uVar1 + 1;
  }
  return uVar1;
}

/* ======================================================== */
/* Function: FUN_1080_014f @ 1080:014f */
/* ======================================================== */

int __stdcall16far FUN_1080_014f(undefined4 param_1)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 local_8;
  int local_6;
  
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  if (*(int *)(iVar2 + 2) == 0) {
    FUN_1078_0627(iVar2,uVar4);
    FUN_1078_05f3(iVar2,uVar4);
    *(undefined1 *)(iVar2 + 0x25) = 1;
    if (*(int *)(iVar2 + 6) == 0 && *(int *)(iVar2 + 8) == 0) {
      local_8 = 0;
    }
    else {
      local_8 = *(undefined2 *)((int)*(undefined4 *)(iVar2 + 6) + 4);
    }
    uVar5 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
    uVar1 = *(undefined2 *)((int)DAT_10a8_1b2e + 0xe);
    uVar5 = *(undefined2 *)((int)DAT_10a8_1b2e + 0x10);
    local_6 = (*DAT_10a8_1b36)((char *)s_Blanks_10a8_1072 + 6,*(undefined2 *)(iVar2 + 0x21),
                               *(undefined2 *)(iVar2 + 0x23),*(undefined2 *)(iVar2 + 0x12),
                               *(undefined2 *)(iVar2 + 0x14),local_8,*(undefined2 *)(iVar2 + 0x1d),
                               *(undefined2 *)(iVar2 + 0x1f),DAT_10a8_1c9e);
    uVar6 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
    iVar3 = (int)DAT_10a8_1b2e;
    *(undefined2 *)(iVar3 + 0xe) = uVar1;
    *(undefined2 *)(iVar3 + 0x10) = uVar5;
    if (local_6 == -1) {
      *(undefined2 *)(iVar2 + 2) = 0xffff;
    }
    *(undefined2 *)(iVar2 + 4) = 0;
  }
  else {
    local_6 = *(int *)(iVar2 + 2);
  }
  return local_6;
}

/* ======================================================== */
/* Function: FUN_1080_021d @ 1080:021d */
/* ======================================================== */

void __stdcall16far FUN_1080_021d(undefined2 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_2 >> 0x10);
  if (*(int *)((int)param_2 + 4) != 0) {
    FUN_1078_060d((int)param_2,uVar1);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1080_023d @ 1080:023d */
/* ======================================================== */

void __stdcall16far FUN_1080_023d(undefined4 param_1,undefined2 param_2)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  if (*(char *)((int)param_1 + 0x25) != '\0') {
    FUN_1078_085a((int)param_1,uVar1,0x21d,(char *)s_PayDividends_10a8_1079 + 7);
    ENDDIALOG((char *)s_Blanks_10a8_1072 + 6,param_2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1080_0274 @ 1080:0274 */
/* ======================================================== */

void __stdcall16far FUN_1080_0274(int *param_1)

{
  (*(code *)*(undefined2 *)(*param_1 + 0x38))();
  return;
}

/* ======================================================== */
/* Function: FUN_1080_028b @ 1080:028b */
/* ======================================================== */

void __stdcall16far FUN_1080_028b(int *param_1,undefined4 param_2)

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
    *(int *)(iVar2 + 10) = (int)(cVar1 == '\0');
    *(undefined2 *)(iVar2 + 0xc) = 0;
  }
  else {
    cVar1 = (*(code *)*(undefined2 *)(*param_1 + 0x3c))();
    *(int *)(iVar2 + 10) = (int)(cVar1 == '\0');
    *(undefined2 *)(iVar2 + 0xc) = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1080_02ef @ 1080:02ef */
/* ======================================================== */

void __stdcall16far FUN_1080_02ef(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  int *piVar1;
  int iVar2;
  int unaff_BP;
  int iVar3;
  undefined2 uVar4;
  long lVar5;
  undefined2 uVar6;
  char *pcVar7;
  
  iVar3 = unaff_BP + 1;
  pcVar7 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_3 >> 0x10);
  uVar6 = *(undefined2 *)((int)param_3 + 4);
  SETFOCUS();
  lVar5 = FUN_1078_0097(*(undefined2 *)((int)param_3 + 4));
  uVar4 = (undefined2)((ulong)lVar5 >> 0x10);
  iVar2 = (int)lVar5;
  if ((lVar5 != 0) && (*(int *)(iVar2 + 0x43) != 0 || *(int *)(iVar2 + 0x45) != 0)) {
    piVar1 = (int *)*(undefined4 *)(iVar2 + 0x43);
    (*(code *)*(undefined2 *)(*piVar1 + 0xc))
              ((char *)s_Blanks_10a8_1072 + 6,(int *)piVar1,(int)((ulong)piVar1 >> 0x10),uVar6,lVar5
               ,pcVar7,iVar3);
  }
  DAT_10a8_1c84 = 1;
  return;
}

/* ======================================================== */
/* Function: FUN_1080_0345 @ 1080:0345 */
/* ======================================================== */

undefined2 __stdcall16far FUN_1080_0345(undefined2 param_1,undefined2 param_2,undefined2 param_3)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  
  uVar1 = GETDLGITEM(unaff_CS,param_3);
  return uVar1;
}

/* ======================================================== */
/* Function: FUN_1080_0369 @ 1080:0369 */
/* ======================================================== */

void __stdcall16far FUN_1080_0369(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_3 >> 0x10);
  *(undefined2 *)((int)param_3 + 10) = 0;
  *(undefined2 *)((int)param_3 + 0xc) = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1080_0382 @ 1080:0382 */
/* ======================================================== */

void __stdcall16far FUN_1080_0382(int *param_1)

{
  char cVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if (*(char *)((int)(int *)param_1 + 0x25) == '\0') {
    FUN_1078_0f38((int *)param_1,uVar2);
  }
  else {
    cVar1 = (*(code *)*(undefined2 *)(*param_1 + 0x3c))();
    if (cVar1 != '\0') {
      (*(code *)*(undefined2 *)(*param_1 + 0x44))();
      (*(code *)*(undefined2 *)(*param_1 + 0x50))();
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1080_03ca @ 1080:03ca */
/* ======================================================== */

void __stdcall16far FUN_1080_03ca(int *param_1)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  if (*(char *)((int)(int *)param_1 + 0x25) == '\0') {
    FUN_1078_0f38((int *)param_1,uVar1);
  }
  else {
    (*(code *)*(undefined2 *)(*param_1 + 0x50))();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1080_03f6 @ 1080:03f6 */
/* ======================================================== */

void __stdcall16far FUN_1080_03f6(void)

{
  FUN_10a0_1e88();
  return;
}

/* ======================================================== */
/* Function: FUN_1080_0417 @ 1080:0417 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1080_0417(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6,undefined2 param_7,undefined2 param_8,
             undefined2 param_9,undefined2 param_10,undefined2 param_11)

{
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  bool bVar3;
  
  bVar3 = true;
  FUN_10a0_03ef((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (!bVar3) {
    uVar2 = (undefined2)((ulong)param_1 >> 0x10);
    iVar1 = (int)param_1;
    FUN_1078_1106(iVar1,uVar2,0,param_7,param_8,param_10,param_11);
    *(undefined2 *)(iVar1 + 0x35) = param_9;
    *(undefined2 *)(iVar1 + 0x29) = param_6;
    *(undefined2 *)(iVar1 + 0x2b) = param_5;
    *(undefined2 *)(iVar1 + 0x2d) = param_4;
    *(undefined2 *)(iVar1 + 0x2f) = param_3;
    *(undefined2 *)(iVar1 + 0x21) = 0;
    *(undefined2 *)(iVar1 + 0x23) = 0x5003;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1080_047c @ 1080:047c */
/* ======================================================== */

undefined1 __stdcall16far FUN_1080_047c(void)

{
  return 1;
}

/* ======================================================== */
/* Function: FUN_1080_0492 @ 1080:0492 */
/* ======================================================== */

void __stdcall16far FUN_1080_0492(int *param_1)

{
  (*(code *)*(undefined2 *)(*param_1 + 0xc))();
  return;
}

/* ======================================================== */
/* Function: FUN_1080_04ae @ 1080:04ae */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1080_04ae(undefined4 param_1,undefined2 param_2,char param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6,undefined2 param_7,undefined2 param_8,
             undefined2 param_9,undefined2 param_10,undefined2 param_11,undefined2 param_12)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  undefined2 uVar3;
  bool bVar4;
  
  bVar4 = true;
  FUN_10a0_03ef((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  if (!bVar4) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    iVar2 = (int)param_1;
    FUN_1080_0417(iVar2,uVar3,0,param_4,param_5,param_6,param_7,param_8,param_9,param_10,param_11,
                  param_12);
    if (param_3 == '\0') {
      uVar1 = *(undefined2 *)(iVar2 + 0x23);
      *(undefined2 *)(iVar2 + 0x21) = *(undefined2 *)(iVar2 + 0x21);
      *(undefined2 *)(iVar2 + 0x23) = uVar1;
    }
    else {
      uVar1 = *(undefined2 *)(iVar2 + 0x23);
      *(uint *)(iVar2 + 0x21) = *(uint *)(iVar2 + 0x21) | 1;
      *(undefined2 *)(iVar2 + 0x23) = uVar1;
    }
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1080_052c @ 1080:052c */
/* ======================================================== */

char * __stdcall16far FUN_1080_052c(void)

{
  char *local_8;
  
  if (DAT_10a8_1b4a == '\0') {
    local_8 = (char *)s_Button_10a8_1c8d;
  }
  else {
    local_8 = (char *)0x1c86;
  }
  return (char *)CONCAT22((char *)s_RandomShrs_10a8_109f + 9,local_8);
}

