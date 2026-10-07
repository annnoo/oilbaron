/*
 * board_logic.c - Oil Baron Board & Grid Logic Subsystem
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - Grid data structure: array of cells at DAT_10a8_22d2
 *   - Row stride: 0x8c bytes, Cell size: 5 bytes
 *   - Cell values:
 *       0 = NONE (Empty)
 *       1 = IMPERIAL, 2 = ROYAL, 3 = AMERICAN, 4 = ARABIAN,
 *       5 = TOWER,    6 = SUPRA, 7 = WEEMOCO
 *       8 = PURPLE (Unformed placed chit)
 *       9 = BLANK (Unplayable / Dead tile)
 *      10 = DISCARD_SQUARE
 *   - Functions include:
 *       * FUN_1008_80b8: 4-neighbor adjacency & connectivity check
 *       * FUN_1008_813e: Flood-fill chain expansion across adjacent tiles
 *       * FUN_1008_826e: Tile placement validation & candidate scoring
 *       * FUN_1008_8f0c: Board mouse click handling (WM_LBUTTONDOWN)
 *       * FUN_1008_93e6: Company territory tile counting
 *       * FUN_1008_9c3e: Tile placement execution & dispatch
 *       * Segments 1048 / 1050: Bitmapped tile and chit rendering
 */

/* Function Index:
 *   FUN_1008_1abd        @ 1008:1abd
 *   FUN_1008_5b20        @ 1008:5b20
 *   FUN_1008_6752        @ 1008:6752
 *   FUN_1008_7021        @ 1008:7021
 *   FUN_1008_80b8        @ 1008:80b8
 *   FUN_1008_813e        @ 1008:813e
 *   FUN_1008_826e        @ 1008:826e
 *   FUN_1008_8f0c        @ 1008:8f0c
 *   FUN_1008_92fd        @ 1008:92fd
 *   FUN_1008_93e6        @ 1008:93e6
 *   FUN_1008_9c3e        @ 1008:9c3e
 *   FUN_1008_a307        @ 1008:a307
 *   FUN_1048_0002        @ 1048:0002
 *   FUN_1048_0079        @ 1048:0079
 *   FUN_1048_00b6        @ 1048:00b6
 *   FUN_1048_02d2        @ 1048:02d2
 *   FUN_1048_0321        @ 1048:0321
 *   FUN_1048_0393        @ 1048:0393
 *   FUN_1048_045f        @ 1048:045f
 *   FUN_1048_0509        @ 1048:0509
 *   FUN_1048_058a        @ 1048:058a
 *   FUN_1048_05ce        @ 1048:05ce
 *   FUN_1048_064f        @ 1048:064f
 *   FUN_1048_06ec        @ 1048:06ec
 *   FUN_1048_0789        @ 1048:0789
 *   FUN_1048_0826        @ 1048:0826
 *   FUN_1048_08b2        @ 1048:08b2
 *   FUN_1048_08f5        @ 1048:08f5
 *   FUN_1048_0923        @ 1048:0923
 *   FUN_1048_0988        @ 1048:0988
 *   FUN_1048_0b6b        @ 1048:0b6b
 *   FUN_1048_0bd8        @ 1048:0bd8
 *   FUN_1048_0cf2        @ 1048:0cf2
 *   FUN_1048_0fac        @ 1048:0fac
 *   FUN_1048_1035        @ 1048:1035
 *   FUN_1048_10c1        @ 1048:10c1
 *   FUN_1048_13ad        @ 1048:13ad
 *   FUN_1048_13db        @ 1048:13db
 *   FUN_1048_1432        @ 1048:1432
 *   FUN_1048_14c9        @ 1048:14c9
 *   FUN_1048_17cd        @ 1048:17cd
 *   FUN_1048_1b39        @ 1048:1b39
 *   FUN_1048_1b6e        @ 1048:1b6e
 *   FUN_1048_1b99        @ 1048:1b99
 *   FUN_1048_1bdc        @ 1048:1bdc
 *   FUN_1048_1cea        @ 1048:1cea
 *   FUN_1048_1d18        @ 1048:1d18
 *   FUN_1048_1d68        @ 1048:1d68
 *   FUN_1050_0002        @ 1050:0002
 *   FUN_1050_00af        @ 1050:00af
 *   FUN_1050_00d3        @ 1050:00d3
 *   FUN_1050_0323        @ 1050:0323
 *   FUN_1050_0343        @ 1050:0343
 */

/* ======================================================== */
/* Function: FUN_1008_1abd @ 1008:1abd */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10081b89) */
/* WARNING: Removing unreachable block (ram,0x10081b9d) */

undefined2 FUN_1008_1abd(char param_1,uint param_2,int param_3,uint param_4,int param_5)

{
  undefined2 *puVar1;
  long lVar2;
  uint uVar3;
  uint uVar4;
  undefined1 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  char *unaff_CS;
  bool bVar10;
  bool bVar11;
  uint local_12;
  int local_10;
  uint local_e;
  int local_c;
  char local_4;
  
  uVar3 = param_4 - 1;
  param_5 = param_5 - (uint)(param_4 == 0);
  uVar4 = param_2 - 1;
  param_3 = param_3 - (uint)(param_2 == 0);
  local_4 = '\t';
  iVar6 = param_5 + (uint)(0xfffe < uVar3);
  local_e = param_4 - 2;
  local_c = param_5 - (uint)(uVar3 == 0);
  if ((local_c <= iVar6) && ((local_c < iVar6 || (local_e <= param_4)))) {
    while( true ) {
      iVar7 = param_3 + (uint)(0xfffe < uVar4);
      local_12 = param_2 - 2;
      local_10 = param_3 - (uint)(uVar4 == 0);
      if ((local_10 <= iVar7) && ((local_10 < iVar7 || (local_12 <= param_2)))) {
        while( true ) {
          if ((((0 < local_c) || (-1 < local_c)) && ((0 < local_10 || (-1 < local_10)))) &&
             ((((local_c < DAT_10a8_22b4 ||
                ((local_c <= DAT_10a8_22b4 && (local_e < DAT_10a8_22b2)))) &&
               (((local_10 < DAT_10a8_22b8 ||
                 ((local_10 <= DAT_10a8_22b8 && (local_12 < DAT_10a8_22b6)))) &&
                (((local_c == param_5 && (local_e == uVar3)) ||
                 ((local_10 == param_3 && (local_12 == uVar4)))))))) &&
              ((*(char *)(local_e * 0x8c + local_12 * 5 + 0x22d2) != '\0' &&
               (*(char *)(local_e * 0x8c + local_12 * 5 + 0x22d2) != '\t')))))) {
            iVar8 = local_e * 0x8c + local_12 * 5;
            bVar11 = iVar8 == 0;
            lVar2 = (long)(int)*(char *)(iVar8 + 0x22d2) * 0x24;
            bVar10 = (int)lVar2 != lVar2;
            unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
            FUN_10a0_1454();
            if (!bVar10 && !bVar11) {
              local_4 = *(char *)(local_e * 0x8c + local_12 * 5 + 0x22d2);
            }
          }
          if ((local_10 == iVar7) && (local_12 == param_2)) break;
          bVar10 = 0xfffe < local_12;
          local_12 = local_12 + 1;
          local_10 = local_10 + (uint)bVar10;
        }
      }
      if ((local_c == iVar6) && (local_e == param_4)) break;
      bVar10 = 0xfffe < local_e;
      local_e = local_e + 1;
      local_c = local_c + (uint)bVar10;
    }
  }
  uVar5 = (undefined1)(local_e >> 8);
  if ((local_4 == '\t') || (param_1 == '\0')) {
    DAT_10a8_1d30 = 0;
  }
  else {
    DAT_10a8_1d30 = 1;
    iVar6 = param_5 - (uint)(uVar3 == 0);
    if (((0 < iVar6) || (-1 < iVar6)) &&
       (*(char *)((param_4 - 2) * 0x8c + uVar4 * 5 + 0x22d2) == '\t')) {
      piVar9 = (int *)((param_4 - 2) * 0x8c + uVar4 * 5 + 0x22d0);
      puVar1 = (undefined2 *)(*piVar9 + 0x10);
      (*(code *)*puVar1)(unaff_CS,piVar9,(char *)s_RandomShrs_10a8_109f + 9,
                         CONCAT11((char)(param_4 - 2 >> 8),local_4));
    }
    iVar6 = param_3 - (uint)(uVar4 == 0);
    if (((0 < iVar6) || (-1 < iVar6)) &&
       (iVar6 = (param_2 - 2) * 5, *(char *)(uVar3 * 0x8c + iVar6 + 0x22d2) == '\t')) {
      piVar9 = (int *)(uVar3 * 0x8c + (param_2 - 2) * 5 + 0x22d0);
      puVar1 = (undefined2 *)(*piVar9 + 0x10);
      (*(code *)*puVar1)(unaff_CS,piVar9,(char *)s_RandomShrs_10a8_109f + 9,
                         CONCAT11((char)((uint)iVar6 >> 8),local_4));
    }
    param_5 = param_5 + (uint)(0xfffe < uVar3);
    if (((param_5 < DAT_10a8_22b4) || ((param_5 <= DAT_10a8_22b4 && (param_4 < DAT_10a8_22b2)))) &&
       (*(char *)(param_4 * 0x8c + uVar4 * 5 + 0x22d2) == '\t')) {
      piVar9 = (int *)(param_4 * 0x8c + uVar4 * 5 + 0x22d0);
      puVar1 = (undefined2 *)(*piVar9 + 0x10);
      (*(code *)*puVar1)(unaff_CS,piVar9,(char *)s_RandomShrs_10a8_109f + 9,local_4);
    }
    param_3 = param_3 + (uint)(0xfffe < uVar4);
    if (((param_3 < DAT_10a8_22b8) ||
        ((uVar4 = param_2, param_3 <= DAT_10a8_22b8 && (param_2 < DAT_10a8_22b6)))) &&
       (uVar4 = param_2 * 5, *(char *)(uVar3 * 0x8c + uVar4 + 0x22d2) == '\t')) {
      piVar9 = (int *)(uVar3 * 0x8c + param_2 * 5 + 0x22d0);
      puVar1 = (undefined2 *)(*piVar9 + 0x10);
      uVar4 = (*(code *)*puVar1)(unaff_CS,piVar9,(char *)s_RandomShrs_10a8_109f + 9,
                                 CONCAT11((char)(uVar4 >> 8),local_4));
    }
    uVar5 = (undefined1)(uVar4 >> 8);
  }
  return CONCAT11(uVar5,local_4);
}

/* ======================================================== */
/* Function: FUN_1008_5b20 @ 1008:5b20 */
/* ======================================================== */

void __stdcall16far FUN_1008_5b20(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  char *pcVar7;
  bool bVar8;
  char *pcVar9;
  char *pcVar10;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  
  iVar1 = FUN_10a0_0be7();
  pcVar10 = (char *)s_RandomShrs_10a8_109f + 1;
  pcVar9 = (char *)0x5b57;
  uVar2 = FUN_10a0_0be7();
  if ((-1 < iVar1) && (-1 < (int)uVar2)) {
    iVar3 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
    if (((int)uVar2 >> 0xf <= iVar3) &&
       (((int)uVar2 >> 0xf < iVar3 || (uVar2 <= DAT_10a8_22b6 - 1U)))) {
      if ((iVar1 >> 0xf == DAT_10a8_22b4) && (iVar1 == DAT_10a8_22b2)) {
        uVar5 = DAT_10a8_38b6;
        FUN_10a0_1458();
        uVar6 = FUN_10a0_1444();
        FUN_10a0_1458(uVar6,iVar3,uVar5);
        FUN_10a0_1438();
        FUN_10a0_1458();
        FUN_10a0_144a();
        FUN_10a0_144a();
        pcVar10 = (char *)s_RandomShrs_10a8_109f + 1;
        pcVar9 = (char *)0x5c22;
        iVar1 = FUN_10a0_145c();
        if ((iVar1 < 0) || (7 < iVar1)) {
          DAT_10a8_1cfc = '\t';
        }
        else {
          DAT_10a8_1cfc = (char)iVar1 + '\x01';
          bVar8 = true;
          pcVar10 = (char *)s_RandomShrs_10a8_109f + 1;
          pcVar9 = (char *)0x5c5d;
          FUN_10a0_1454();
          if (bVar8) {
            DAT_10a8_1cfc = '\t';
          }
        }
      }
      else {
        DAT_10a8_1cfc = *(char *)(iVar1 * 0x8c + uVar2 * 5 + 0x22d2);
      }
      pcVar7 = (char *)s_RandomShrs_10a8_109f + 1;
      if ((DAT_10a8_1cfc == '\0') || (DAT_10a8_1cfc == '\t')) {
        MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0);
      }
      else {
        if (DAT_10a8_1cfd == '\0') {
          pcVar10 = (char *)0x20;
          pcVar9 = (char *)s_RandomShrs_10a8_109f + 1;
          pcVar7 = (char *)s__Show_Graph_10a8_1226 + 10;
          local_6 = GETSYSTEMMETRICS((char *)s_RandomShrs_10a8_109f + 1,0x20);
        }
        else {
          local_6 = 0;
        }
        if (DAT_10a8_1cfd == '\0') {
          local_8 = GETSYSTEMMETRICS(pcVar7,0x21,pcVar9,pcVar10);
          pcVar7 = (char *)s__Show_Graph_10a8_1226 + 10;
        }
        else {
          local_8 = 0;
        }
        iVar1 = GETSYSTEMMETRICS(pcVar7,0xf,pcVar9,pcVar10);
        iVar3 = GETSYSTEMMETRICS((char *)s__Show_Graph_10a8_1226 + 10,4);
        uVar5 = (undefined2)((ulong)param_1 >> 0x10);
        iVar4 = (int)param_1;
        local_a = *(int *)(iVar4 + 0x29);
        local_c = *(int *)(iVar4 + 0x2b);
        if (DAT_10a8_1cfd != '\0') {
          local_a = 0;
          local_c = 0;
        }
        GETSUBMENU((char *)s__Show_Graph_10a8_1226 + 10,0);
        uVar6 = (undefined2)((ulong)param_2 >> 0x10);
        TRACKPOPUPMENU((char *)s__Show_Graph_10a8_1226 + 10,0,0,*(undefined2 *)(iVar4 + 4),0,
                       local_c + *(int *)((int)param_2 + 8) + iVar3 + iVar1 + local_8,
                       local_6 + *(int *)((int)param_2 + 6) + local_a,2);
      }
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6752 @ 1008:6752 */
/* ======================================================== */

void __cdecl16near FUN_1008_6752(void)

{
  char cVar1;
  int iVar2;
  undefined1 local_4;
  undefined1 local_3;
  
  cVar1 = (char)DAT_10a8_22b2;
  local_3 = 0;
  while( true ) {
    iVar2 = DAT_10a8_22b6 + -1;
    for (local_4 = 0; *(undefined1 *)((uint)local_3 * 0x8c + (uint)local_4 * 5 + 0x22d2) = 0,
        local_4 != (byte)iVar2; local_4 = local_4 + 1) {
    }
    if (local_3 == (byte)(cVar1 - 1U)) break;
    local_3 = local_3 + 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7021 @ 1008:7021 */
/* ======================================================== */

void __stdcall16far FUN_1008_7021(void)

{
  char cVar1;
  char local_d;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  
  local_d = '\b';
  local_a = 0;
  local_c = 0;
  if (-1 < DAT_10a8_22b2 + -1) {
    local_6 = 0;
    while( true ) {
      if (-1 < DAT_10a8_22b6 + -1) {
        local_8 = 0;
        while( true ) {
          cVar1 = *(char *)(local_6 * 0x8c + local_8 * 5 + 0x22d2);
          if (((cVar1 != '\0') && (cVar1 != '\t')) &&
             (*(char *)(local_6 * 0x8c + local_8 * 5 + 0x22d2) <= local_d)) {
            local_a = local_6;
            local_c = local_8;
            local_d = *(char *)(local_6 * 0x8c + local_8 * 5 + 0x22d2);
          }
          if (local_8 == DAT_10a8_22b6 + -1) break;
          local_8 = local_8 + 1;
        }
      }
      if (local_6 == DAT_10a8_22b2 + -1) break;
      local_6 = local_6 + 1;
    }
  }
  FUN_1008_422f(local_c + 1,local_c + 1 >> 0xf,local_a + 0x41);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_80b8 @ 1008:80b8 */
/* ======================================================== */

undefined4 FUN_1008_80b8(undefined2 param_1,char param_2)

{
  int iVar1;
  undefined2 local_a;
  undefined2 local_8;
  undefined2 local_6;
  
  local_a = 0;
  iVar1 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
  if (-1 < DAT_10a8_22b2 + -1) {
    local_6 = 0;
    while( true ) {
      iVar1 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
      if (-1 < DAT_10a8_22b6 + -1) {
        local_8 = 0;
        while( true ) {
          if (*(char *)(local_6 * 0x8c + local_8 * 5 + 0x22d2) == param_2) {
            local_a = local_a + 1;
          }
          if (local_8 == DAT_10a8_22b6 + -1) break;
          local_8 = local_8 + 1;
        }
      }
      if (local_6 == DAT_10a8_22b2 + -1) break;
      local_6 = local_6 + 1;
    }
  }
  return CONCAT22(iVar1,local_a);
}

/* ======================================================== */
/* Function: FUN_1008_813e @ 1008:813e */
/* ======================================================== */

undefined4 FUN_1008_813e(undefined2 param_1,char param_2)

{
  int iVar1;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  
  local_e = 0;
  iVar1 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
  if (-1 < (int)(DAT_10a8_22b2 - 1)) {
    local_6 = 0;
    while( true ) {
      iVar1 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
      if (-1 < (int)(DAT_10a8_22b6 - 1)) {
        local_8 = 0;
        while( true ) {
          if (*(char *)(local_6 * 0x8c + local_8 * 5 + 0x22d2) == param_2) {
            local_a = -1;
            while( true ) {
              local_c = -1;
              while( true ) {
                if ((-1 < local_6 + local_a) && (-1 < local_8 + local_c)) {
                  iVar1 = local_6 + local_a >> 0xf;
                  if ((iVar1 < DAT_10a8_22b4) ||
                     ((iVar1 <= DAT_10a8_22b4 && ((uint)(local_6 + local_a) < DAT_10a8_22b2)))) {
                    iVar1 = local_8 + local_c >> 0xf;
                    if (((iVar1 < DAT_10a8_22b8) ||
                        ((iVar1 <= DAT_10a8_22b8 && ((uint)(local_8 + local_c) < DAT_10a8_22b6))))
                       && (((local_a == 0 || ((local_c == 0 && (local_a != 0)))) &&
                           (iVar1 = (local_8 + local_c) * 5,
                           *(char *)((local_6 + local_a) * 0x8c + iVar1 + 0x22d2) == '\0')))) {
                      local_e = local_e + 1;
                    }
                  }
                }
                if (local_c == 1) break;
                local_c = local_c + 1;
              }
              if (local_a == 1) break;
              local_a = local_a + 1;
            }
          }
          if (local_8 == DAT_10a8_22b6 - 1) break;
          local_8 = local_8 + 1;
        }
      }
      if (local_6 == DAT_10a8_22b2 - 1) break;
      local_6 = local_6 + 1;
    }
  }
  return CONCAT22(iVar1,local_e);
}

/* ======================================================== */
/* Function: FUN_1008_826e @ 1008:826e */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x100885a3) */
/* WARNING: Removing unreachable block (ram,0x10088636) */
/* WARNING: Removing unreachable block (ram,0x100883d2) */
/* WARNING: Removing unreachable block (ram,0x10088399) */
/* WARNING: Removing unreachable block (ram,0x1008833f) */
/* WARNING: Removing unreachable block (ram,0x1008856a) */
/* WARNING: Removing unreachable block (ram,0x10088306) */
/* WARNING: Removing unreachable block (ram,0x100885fd) */
/* WARNING: Removing unreachable block (ram,0x100882ab) */

undefined4 FUN_1008_826e(int param_1,char param_2,char *param_3,char *param_4)

{
  int *piVar1;
  byte bVar2;
  undefined1 extraout_AH;
  uint uVar3;
  uint uVar4;
  undefined2 uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  char *pcVar9;
  undefined2 unaff_SS;
  bool bVar10;
  undefined1 uVar11;
  bool bVar12;
  long lVar13;
  undefined1 *puVar14;
  undefined1 local_56 [32];
  uint local_36;
  int local_34;
  uint local_32;
  int local_30;
  uint local_2e;
  int local_2c;
  uint local_2a;
  int local_28;
  char local_25;
  undefined2 local_24;
  uint local_22;
  int local_20;
  undefined2 local_1e;
  uint local_1c;
  int local_1a;
  uint local_18;
  int local_16;
  uint local_14;
  int local_12;
  uint local_10;
  int local_e;
  uint local_c;
  int local_a;
  undefined2 local_8;
  undefined2 local_6;
  int local_4;
  
  local_1e = 0x83;
  local_1c = 0;
  local_1a = 0x6000;
  local_8 = 0x83;
  local_6 = 0;
  local_4 = 0x6000;
  *param_3 = '\x01';
  pcVar9 = (char *)s_StartCash_10a8_1043 + 5;
  lVar13 = FUN_1048_1035(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
  if (lVar13 < 2) {
    if (param_2 == '\x01') {
      *param_3 = *param_4;
    }
    local_8 = local_1e;
    local_4 = local_1a;
  }
  else {
    if (param_2 == '\0') {
      local_2a = DAT_10a8_22b2 - 1;
      local_28 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
      if (-1 < local_28) {
        local_c = 0;
        local_a = 0;
        while( true ) {
          local_2e = DAT_10a8_22b6 - 1;
          local_2c = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
          if (-1 < local_2c) {
            local_14 = 0;
            local_12 = 0;
            while( true ) {
              if (*(char *)(local_c * 0x8c + local_14 * 5 + 0x22d2) == *param_4) {
                local_32 = DAT_10a8_22b2 - 1;
                local_30 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
                if (-1 < local_30) {
                  local_10 = 0;
                  local_e = 0;
                  while( true ) {
                    local_36 = DAT_10a8_22b6 - 1;
                    local_34 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
                    if (-1 < local_34) {
                      local_18 = 0;
                      local_16 = 0;
                      while( true ) {
                        if (*(char *)(local_10 * 0x8c + local_18 * 5 + 0x22d2) == *param_3) {
                          if (DAT_10a8_1d2e == '\0') {
                            piVar1 = (int *)*(undefined4 *)(param_1 + 6);
                            (*(code *)*(undefined2 *)(*piVar1 + 0x50))
                                      (pcVar9,(int *)piVar1,(int)((ulong)piVar1 >> 0x10));
                          }
                          if (DAT_10a8_1d2f != '\0') {
                            uVar5 = GETDC(pcVar9,*(undefined2 *)
                                                  ((int)*(undefined4 *)(param_1 + 6) + 4));
                            *(undefined2 *)(param_1 + -4) = uVar5;
                            FUN_1048_14c9(*(undefined2 *)(param_1 + -4));
                            RELEASEDC((char *)s_StartCash_10a8_1043 + 5,
                                      *(undefined2 *)(param_1 + -4));
                            goto LAB_1008_8819;
                          }
                          uVar7 = (local_16 - local_12) - (uint)(local_18 < local_14);
                          uVar3 = FUN_10a0_0baa();
                          uVar8 = (local_e - local_a) - (uint)(local_10 < local_c);
                          uVar4 = FUN_10a0_0baa();
                          uVar11 = CARRY2(uVar8,uVar7) ||
                                   CARRY2(uVar8 + uVar7,(uint)CARRY2(uVar4,uVar3));
                          iVar6 = uVar8 + uVar7 + (uint)CARRY2(uVar4,uVar3);
                          FUN_10a0_1458();
                          local_24 = FUN_10a0_150b();
                          pcVar9 = (char *)s_RandomShrs_10a8_109f + 1;
                          local_22 = uVar7;
                          local_20 = iVar6;
                          FUN_10a0_1454();
                          if ((bool)uVar11) {
                            local_1e = local_24;
                            local_1c = local_22;
                            local_1a = local_20;
                          }
                        }
                        if ((local_16 == local_34) && (local_18 == local_36)) break;
                        bVar10 = 0xfffe < local_18;
                        local_18 = local_18 + 1;
                        local_16 = local_16 + (uint)bVar10;
                      }
                    }
                    if ((local_e == local_30) && (local_10 == local_32)) break;
                    bVar10 = 0xfffe < local_10;
                    local_10 = local_10 + 1;
                    local_e = local_e + (uint)bVar10;
                  }
                }
              }
              if ((local_12 == local_2c) && (local_14 == local_2e)) break;
              bVar10 = 0xfffe < local_14;
              local_14 = local_14 + 1;
              local_12 = local_12 + (uint)bVar10;
            }
          }
          if ((local_a == local_28) && (local_c == local_2a)) break;
          bVar10 = 0xfffe < local_c;
          local_c = local_c + 1;
          local_a = local_a + (uint)bVar10;
        }
      }
    }
    else {
      local_25 = '\x01';
      local_2a = DAT_10a8_22b2 - 1;
      local_28 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
      if (-1 < local_28) {
        local_c = 0;
        local_a = 0;
        while( true ) {
          local_2e = DAT_10a8_22b6 - 1;
          local_2c = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
          if (-1 < local_2c) {
            local_14 = 0;
            local_12 = 0;
            while( true ) {
              if (*(char *)(local_c * 0x8c + local_14 * 5 + 0x22d2) == *param_4) {
                local_32 = DAT_10a8_22b2 - 1;
                local_30 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
                if (-1 < local_30) {
                  local_10 = 0;
                  local_e = 0;
                  while( true ) {
                    local_36 = DAT_10a8_22b6 - 1;
                    local_34 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
                    if (-1 < local_34) {
                      local_18 = 0;
                      local_16 = 0;
                      while( true ) {
                        iVar6 = 0x20;
                        bVar2 = FUN_10a0_0f8b();
                        puVar14 = local_56;
                        uVar5 = unaff_SS;
                        FUN_10a0_0f00(0x201);
                        FUN_10a0_0f16(CONCAT11(extraout_AH,*param_4));
                        if ((puVar14[iVar6] & bVar2) == 0) {
                          uVar7 = (local_16 - local_12) - (uint)(local_18 < local_14);
                          uVar3 = FUN_10a0_0baa();
                          uVar8 = (local_e - local_a) - (uint)(local_10 < local_c);
                          uVar4 = FUN_10a0_0baa();
                          uVar11 = CARRY2(uVar8,uVar7) ||
                                   CARRY2(uVar8 + uVar7,(uint)CARRY2(uVar4,uVar3));
                          iVar6 = uVar8 + uVar7 + (uint)CARRY2(uVar4,uVar3);
                          FUN_10a0_1458();
                          local_24 = FUN_10a0_150b();
                          local_22 = uVar7;
                          local_20 = iVar6;
                          FUN_10a0_1454();
                          if ((bool)uVar11) {
                            local_1e = local_24;
                            local_1c = local_22;
                            local_1a = local_20;
                            local_25 = *(char *)(local_10 * 0x8c + local_18 * 5 + 0x22d2);
                            if (DAT_10a8_1d2e == '\0') {
                              piVar1 = (int *)*(undefined4 *)(param_1 + 6);
                              (*(code *)*(undefined2 *)(*piVar1 + 0x50))
                                        ((char *)s_RandomShrs_10a8_109f + 1,(int *)piVar1,
                                         (int)((ulong)piVar1 >> 0x10));
                            }
                            if (DAT_10a8_1d2f != '\0') {
                              uVar5 = GETDC((char *)s_RandomShrs_10a8_109f + 1,
                                            *(undefined2 *)((int)*(undefined4 *)(param_1 + 6) + 4));
                              *(undefined2 *)(param_1 + -4) = uVar5;
                              FUN_1048_14c9(*(undefined2 *)(param_1 + -4));
                              RELEASEDC((char *)s_StartCash_10a8_1043 + 5,
                                        *(undefined2 *)(param_1 + -4));
                              goto LAB_1008_8819;
                            }
                          }
                          bVar10 = false;
                          bVar12 = true;
                          FUN_10a0_1454();
                          if (bVar10 || bVar12) goto LAB_1008_87fe;
                        }
                        if ((local_16 == local_34) && (local_18 == local_36)) break;
                        bVar10 = 0xfffe < local_18;
                        local_18 = local_18 + 1;
                        local_16 = local_16 + (uint)bVar10;
                      }
                    }
                    if ((local_e == local_30) && (local_10 == local_32)) break;
                    bVar10 = 0xfffe < local_10;
                    local_10 = local_10 + 1;
                    local_e = local_e + (uint)bVar10;
                  }
                }
              }
              if ((local_12 == local_2c) && (local_14 == local_2e)) break;
              bVar10 = 0xfffe < local_14;
              local_14 = local_14 + 1;
              local_12 = local_12 + (uint)bVar10;
            }
          }
          if ((local_a == local_28) && (local_c == local_2a)) break;
          bVar10 = 0xfffe < local_c;
          local_c = local_c + 1;
          local_a = local_a + (uint)bVar10;
        }
      }
LAB_1008_87fe:
      *param_3 = local_25;
    }
    local_8 = local_1e;
    local_4 = local_1a;
  }
LAB_1008_8819:
  return CONCAT22(local_4,local_8);
}

/* ======================================================== */
/* Function: FUN_1008_8f0c @ 1008:8f0c */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10088fe9) */
/* WARNING: Removing unreachable block (ram,0x10089007) */

undefined4 FUN_1008_8f0c(undefined2 param_1,char *param_2,undefined2 param_3,undefined2 param_4)

{
  char *pcVar1;
  char *pcVar2;
  byte bVar3;
  undefined1 extraout_AH;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  undefined2 unaff_SS;
  bool bVar7;
  undefined4 uVar8;
  undefined1 *puVar9;
  undefined2 uVar10;
  undefined1 local_46 [32];
  uint local_26;
  int local_24;
  uint local_22;
  int local_20;
  uint local_1e;
  int local_1c;
  uint local_1a;
  int local_18;
  undefined1 local_16;
  undefined1 local_15;
  char local_e;
  int local_d;
  
  pcVar6 = &local_e;
  pcVar5 = (char *)param_2;
  for (iVar4 = 5; iVar4 != 0; iVar4 = iVar4 + -1) {
    pcVar2 = pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar1 = pcVar5;
    pcVar5 = pcVar5 + 1;
    *pcVar2 = *pcVar1;
  }
  local_15 = *(undefined1 *)((local_e + -0x41) * 0x8c + (local_d + -1) * 5 + 0x22d2);
  local_1a = (int)local_e - 0x41;
  local_18 = (int)local_1a >> 0xf;
  local_1e = local_d - 1;
  local_1c = (int)local_1e >> 0xf;
  local_22 = 0xffff;
  local_20 = -1;
  while( true ) {
    local_26 = 0xffff;
    local_24 = -1;
    while( true ) {
      if (((((local_20 != local_24) || (local_22 != local_26)) &&
           (local_22 + local_26 != 0 || local_20 + local_24 + (uint)CARRY2(local_22,local_26) != 0))
          && ((iVar4 = local_18 + local_20 + (uint)CARRY2(local_1a,local_22), 0 < iVar4 ||
              (-1 < iVar4)))) &&
         ((iVar4 = local_1c + local_24 + (uint)CARRY2(local_1e,local_26), 0 < iVar4 || (-1 < iVar4))
         )) {
        iVar4 = local_18 + local_20 + (uint)CARRY2(local_1a,local_22);
        if ((iVar4 < DAT_10a8_22b4) ||
           ((iVar4 <= DAT_10a8_22b4 && (local_1a + local_22 < DAT_10a8_22b2)))) {
          iVar4 = local_1c + local_24 + (uint)CARRY2(local_1e,local_26);
          if ((iVar4 < DAT_10a8_22b8) ||
             ((iVar4 <= DAT_10a8_22b8 && (local_1e + local_26 < DAT_10a8_22b6)))) {
            iVar4 = 0x20;
            bVar3 = FUN_10a0_0f8b();
            puVar9 = local_46;
            uVar10 = unaff_SS;
            FUN_10a0_0f00(0x201);
            FUN_10a0_0f16(CONCAT11(extraout_AH,local_15));
            if ((puVar9[iVar4] & bVar3) == 0) {
              local_16 = *(undefined1 *)
                          ((local_1a + local_22) * 0x8c + (local_1e + local_26) * 5 + 0x22d2);
            }
          }
        }
      }
      if ((local_24 == 0) && (local_26 == 1)) break;
      bVar7 = 0xfffe < local_26;
      local_26 = local_26 + 1;
      local_24 = local_24 + (uint)bVar7;
    }
    if ((local_20 == 0) && (local_22 == 1)) break;
    bVar7 = 0xfffe < local_22;
    local_22 = local_22 + 1;
    local_20 = local_20 + (uint)bVar7;
  }
  uVar8 = FUN_1008_88d9(param_1,local_16,local_15,param_3,param_4);
  return uVar8;
}

/* ======================================================== */
/* Function: FUN_1008_92fd @ 1008:92fd */
/* ======================================================== */

undefined2 FUN_1008_92fd(undefined2 param_1,char *param_2,uint param_3,uint param_4)

{
  char *pcVar1;
  char *pcVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  int iVar8;
  int iVar9;
  undefined2 unaff_SS;
  bool bVar10;
  uint local_e;
  uint local_c;
  char local_8;
  int local_7;
  bool local_3;
  
  pcVar7 = &local_8;
  pcVar6 = (char *)param_2;
  for (iVar5 = 5; iVar5 != 0; iVar5 = iVar5 + -1) {
    pcVar2 = pcVar7;
    pcVar7 = pcVar7 + 1;
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
    *pcVar2 = *pcVar1;
  }
  cVar3 = *(char *)((local_8 + -0x41) * 0x8c + (local_7 + -1) * 5 + 0x22d2);
  iVar5 = param_3 * 0xfc + cVar3 * 4;
  local_3 = *(int *)(iVar5 + 0x3182) != 0 || *(int *)(iVar5 + 0x3184) != 0;
  local_e = 0;
  if ((DAT_10a8_22ac < 0x8000) && ((0 < (int)DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
    local_e = 1;
    local_c = 0;
    while( true ) {
      if ((local_c != param_4) || (local_e != param_3)) {
        iVar8 = param_3 * 0xfc + cVar3 * 4;
        iVar5 = *(int *)(iVar8 + 0x3184);
        iVar9 = local_e * 0xfc + cVar3 * 4;
        iVar4 = *(int *)(iVar9 + 0x3184);
        if ((iVar5 < iVar4) ||
           ((iVar5 <= iVar4 && (*(uint *)(iVar8 + 0x3182) <= *(uint *)(iVar9 + 0x3182))))) {
          local_3 = false;
        }
      }
      if ((local_c == DAT_10a8_22ac) && (local_e == DAT_10a8_22aa)) break;
      bVar10 = 0xfffe < local_e;
      local_e = local_e + 1;
      local_c = local_c + bVar10;
    }
  }
  return CONCAT11((char)(local_e >> 8),local_3);
}

/* ======================================================== */
/* Function: FUN_1008_93e6 @ 1008:93e6 */
/* ======================================================== */

undefined4 FUN_1008_93e6(undefined2 param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  undefined2 *puVar3;
  undefined1 uVar4;
  char cVar5;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  undefined1 extraout_AH_01;
  int iVar6;
  undefined2 uVar7;
  undefined2 extraout_DX;
  undefined2 uVar8;
  char *pcVar9;
  char *pcVar10;
  int *piVar11;
  undefined2 unaff_SS;
  bool bVar12;
  bool bVar13;
  char local_1c;
  char local_1b;
  undefined2 local_1a;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_12;
  undefined2 local_10;
  char local_e;
  int local_d;
  
  pcVar10 = &local_e;
  pcVar9 = (char *)param_2;
  for (iVar6 = 5; iVar6 != 0; iVar6 = iVar6 + -1) {
    pcVar2 = pcVar10;
    pcVar10 = pcVar10 + 1;
    pcVar1 = pcVar9;
    pcVar9 = pcVar9 + 1;
    *pcVar2 = *pcVar1;
  }
  local_14 = 0;
  local_12 = 0;
  local_10 = 0;
  cVar5 = FUN_1008_92fd(param_1,&local_e,unaff_SS,DAT_10a8_319a,DAT_10a8_319c);
  if (cVar5 == '\0') {
    local_14 = 0x87;
    local_10 = 0xc800;
  }
  else {
    local_1b = *(char *)((local_e + -0x41) * 0x8c + (local_d + -1) * 5 + 0x22d2);
    iVar6 = local_1b * 0x24;
    local_1a = *(undefined2 *)(iVar6 + 0x378a);
    uVar8 = *(undefined2 *)(iVar6 + 0x378c);
    local_16 = ((undefined2 *)&DAT_10a8_378e)[local_1b * 0x12];
    uVar7 = *(undefined2 *)(DAT_10a8_319a * 0xfc + local_1b * 4 + 0x3184);
    local_18 = uVar8;
    FUN_10a0_1458();
    local_14 = FUN_10a0_1444();
    local_12 = uVar8;
    local_10 = uVar7;
    FUN_1008_826e(param_1,1,&local_1c,unaff_SS,&local_1b,unaff_SS);
    bVar13 = DAT_10a8_1d2f == '\0';
    if (bVar13) {
      bVar12 = (long)(int)((long)(int)local_1b * 0x24) != (long)(int)local_1b * 0x24;
      FUN_10a0_1454();
      if (((!bVar12 && !bVar13) ||
          (cVar5 = FUN_1008_8826(param_1,CONCAT11(extraout_AH,local_1c),DAT_10a8_319a,DAT_10a8_319c)
          , cVar5 != '\0')) && (local_1b != local_1c)) {
        piVar11 = (int *)((local_e + -0x41) * 0x8c + (local_d + -1) * 5 + 0x22d0);
        puVar3 = (undefined2 *)(*piVar11 + 0x10);
        (*(code *)*puVar3)((char *)s_RandomShrs_10a8_109f + 1,piVar11,
                           (char *)s_RandomShrs_10a8_109f + 9,0);
        uVar4 = DAT_10a8_1d2e;
        DAT_10a8_1d2e = 0;
        FUN_1008_826e(param_1,1,&local_1c,unaff_SS,&local_1b,unaff_SS);
        piVar11 = (int *)((local_e + -0x41) * 0x8c + (local_d + -1) * 5 + 0x22d0);
        puVar3 = (undefined2 *)(*piVar11 + 0x10);
        DAT_10a8_1d2e = uVar4;
        (*(code *)*puVar3)((char *)s_RandomShrs_10a8_109f + 1,piVar11,
                           (char *)s_RandomShrs_10a8_109f + 9,CONCAT11(extraout_AH_00,local_1b));
        bVar13 = false;
        FUN_10a0_1454();
        if ((bVar13) && (FUN_10a0_1454(), bVar13)) {
          FUN_1008_88d9(param_1,CONCAT11(extraout_AH_01,local_1c),CONCAT11(extraout_AH_01,local_1b),
                        DAT_10a8_319a,DAT_10a8_319c);
          uVar8 = extraout_DX;
          FUN_10a0_1444();
          local_14 = FUN_10a0_1432();
          local_10 = uVar8;
        }
      }
    }
  }
  return CONCAT22(local_10,local_14);
}

/* ======================================================== */
/* Function: FUN_1008_9c3e @ 1008:9c3e */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10089d6f) */
/* WARNING: Removing unreachable block (ram,0x10089dad) */

undefined1 FUN_1008_9c3e(int param_1,char *param_2,char *param_3)

{
  char *pcVar1;
  char *pcVar2;
  undefined2 *puVar3;
  char cVar4;
  byte bVar5;
  undefined2 uVar6;
  undefined1 extraout_AH;
  int iVar7;
  char *pcVar8;
  char *pcVar9;
  int *piVar10;
  undefined2 unaff_SS;
  bool bVar11;
  bool bVar12;
  undefined1 uVar13;
  undefined1 *puVar14;
  undefined2 uVar15;
  undefined2 uVar16;
  undefined1 local_44 [32];
  char local_24;
  char local_23;
  undefined2 local_22;
  undefined2 local_20;
  undefined2 local_1e;
  undefined1 local_1b;
  char local_1a;
  char local_19;
  uint local_18;
  int local_16;
  uint local_14;
  int local_12;
  uint local_10;
  int local_e;
  uint local_c;
  int local_a;
  char local_8;
  int local_7;
  
  pcVar9 = &local_8;
  pcVar8 = (char *)param_3;
  for (iVar7 = 5; iVar7 != 0; iVar7 = iVar7 + -1) {
    pcVar2 = pcVar9;
    pcVar9 = pcVar9 + 1;
    pcVar1 = pcVar8;
    pcVar8 = pcVar8 + 1;
    *pcVar2 = *pcVar1;
  }
  local_1b = 0;
  local_c = (int)local_8 - 0x41;
  local_a = (int)local_c >> 0xf;
  local_10 = local_7 - 1;
  local_e = (int)local_10 >> 0xf;
  DAT_10a8_1d2e = 1;
  uVar6 = FUN_1008_1abd(0,local_7,local_e + (uint)(0xfffe < local_10),local_8 + -0x40,
                        local_a + (uint)(0xfffe < local_c));
  local_19 = (char)uVar6;
  if (local_19 != '\t') {
    piVar10 = (int *)(local_c * 0x8c + local_10 * 5 + 0x22d0);
    pcVar8 = (char *)s_RandomShrs_10a8_109f + 9;
    (*(code *)*(undefined2 *)(*piVar10 + 0x10))();
    pcVar9 = (char *)s_FALSE_10a8_1003 + 5;
    cVar4 = FUN_1008_2448((int)*(undefined4 *)(param_1 + 6),
                          (int)((ulong)*(undefined4 *)(param_1 + 6) >> 0x10),0,local_10,local_e,
                          local_c,local_a);
    if (cVar4 != '\0') {
      local_1b = 1;
      local_23 = *(char *)(local_c * 0x8c + local_10 * 5 + 0x22d2);
      local_14 = 0xffff;
      local_12 = -1;
      while( true ) {
        local_18 = 0xffff;
        local_16 = -1;
        while( true ) {
          if ((((local_12 != local_16) || (local_14 != local_18)) &&
              (local_14 + local_18 != 0 ||
               local_12 + local_16 + (uint)CARRY2(local_14,local_18) != 0)) &&
             ((iVar7 = local_a + local_12 + (uint)CARRY2(local_c,local_14), 0 < iVar7 ||
              (-1 < iVar7)))) {
            iVar7 = local_a + local_12 + (uint)CARRY2(local_c,local_14);
            if (((iVar7 < DAT_10a8_22b4) ||
                ((iVar7 <= DAT_10a8_22b4 && (local_c + local_14 < DAT_10a8_22b2)))) &&
               ((iVar7 = local_e + local_16 + (uint)CARRY2(local_10,local_18), 0 < iVar7 ||
                (-1 < iVar7)))) {
              iVar7 = local_e + local_16 + (uint)CARRY2(local_10,local_18);
              if ((iVar7 < DAT_10a8_22b8) ||
                 ((iVar7 <= DAT_10a8_22b8 && (local_10 + local_18 < DAT_10a8_22b6)))) {
                iVar7 = 0x20;
                bVar5 = FUN_10a0_0f8b(piVar10,pcVar8,uVar6);
                puVar14 = local_44;
                uVar15 = unaff_SS;
                FUN_10a0_0f00(0x201);
                FUN_10a0_0f16(CONCAT11(extraout_AH,local_23));
                if ((puVar14[iVar7] & bVar5) == 0) {
                  local_24 = *(char *)((local_c + local_14) * 0x8c + (local_10 + local_18) * 5 +
                                      0x22d2);
                }
              }
            }
          }
          if ((local_16 == 0) && (local_18 == 1)) break;
          bVar11 = 0xfffe < local_18;
          local_18 = local_18 + 1;
          local_16 = local_16 + (uint)bVar11;
        }
        if ((local_12 == 0) && (bVar11 = local_14 == 1, bVar11)) break;
        bVar11 = 0xfffe < local_14;
        local_14 = local_14 + 1;
        local_12 = local_12 + (uint)bVar11;
      }
      bVar12 = (long)(int)((long)(int)local_23 * 0x24) != (long)(int)local_23 * 0x24;
      FUN_10a0_1454();
      if (bVar12 || bVar11) {
        *param_2 = local_23;
        local_1a = local_24;
      }
      else {
        *param_2 = local_24;
        local_1a = local_23;
      }
      iVar7 = local_1a * 0x24;
      local_22 = *(undefined2 *)(iVar7 + 0x378a);
      local_20 = *(undefined2 *)(iVar7 + 0x378c);
      local_1e = ((undefined2 *)&DAT_10a8_378e)[local_1a * 0x12];
      iVar7 = *param_2 * 0x24;
      uVar6 = *(undefined2 *)(iVar7 + 0x3772);
      uVar15 = *(undefined2 *)(iVar7 + 0x3774);
      uVar16 = *(undefined2 *)(iVar7 + 0x3776);
      FUN_1008_80b8(param_1,CONCAT11((char)((uint)uVar6 >> 8),*param_2));
      FUN_10a0_1458(uVar6,uVar15,uVar16);
      FUN_10a0_1444();
      FUN_10a0_144a();
      uVar13 = (long)(int)((long)(int)local_1a * 0x24) != (long)(int)local_1a * 0x24;
      FUN_10a0_1432();
      pcVar9 = (char *)s_RandomShrs_10a8_109f + 1;
      FUN_10a0_1454();
      if ((bool)uVar13) {
        *param_2 = local_1a;
      }
    }
    piVar10 = (int *)(local_c * 0x8c + local_10 * 5 + 0x22d0);
    puVar3 = (undefined2 *)(*piVar10 + 0x10);
    (*(code *)*puVar3)(pcVar9,piVar10,(char *)s_RandomShrs_10a8_109f + 9,0);
  }
  DAT_10a8_1d2e = 0;
  return local_1b;
}

/* ======================================================== */
/* Function: FUN_1008_a307 @ 1008:a307 */
/* ======================================================== */

undefined4 FUN_1008_a307(undefined2 param_1,char param_2)

{
  int iVar1;
  int local_a;
  int local_8;
  int local_6;
  
  local_a = 0;
  iVar1 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
  local_6 = 0;
  if (-1 < DAT_10a8_22b2 + -1) {
    local_6 = 0;
    while( true ) {
      iVar1 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
      if (-1 < DAT_10a8_22b6 + -1) {
        local_8 = 0;
        while( true ) {
          if (*(char *)(local_6 * 0x8c + local_8 * 5 + 0x22d2) == param_2) {
            local_a = local_a + 1;
          }
          if (local_8 == DAT_10a8_22b6 + -1) break;
          local_8 = local_8 + 1;
        }
      }
      if (local_6 == DAT_10a8_22b2 + -1) break;
      local_6 = local_6 + 1;
    }
  }
  return CONCAT22(iVar1,CONCAT11((char)((uint)local_6 >> 8),0x2c < local_a));
}

/* ======================================================== */
/* Function: FUN_1048_0002 @ 1048:0002 */
/* ======================================================== */

void __stdcall16far FUN_1048_0002(undefined2 *param_1,undefined2 *param_2)

{
  int unaff_BP;
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  int local_40 [19];
  undefined2 local_1a;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_e;
  undefined2 local_c;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar1 = (undefined2)((ulong)param_2 >> 0x10);
  local_e = *param_2;
  local_c = ((undefined2 *)param_2)[1];
  uVar2 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  FUN_1080_0002(local_40,unaff_SS,0xd2e,0xd8e,(char *)s_RandomShrs_10a8_109f + 9,
                *(undefined2 *)((int)DAT_10a8_1b2e + 8),*(undefined2 *)((int)DAT_10a8_1b2e + 10));
  (*(code *)*(undefined2 *)(local_40[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_40);
  (*(code *)*(undefined2 *)(local_40[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_40);
  *param_2 = local_1a;
  ((undefined2 *)param_2)[1] = local_18;
  *param_1 = local_16;
  ((undefined2 *)param_1)[1] = local_14;
  return;
}

/* ======================================================== */
/* Function: FUN_1048_0079 @ 1048:0079 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1048_0079(undefined4 param_1,undefined2 param_2,undefined1 param_3,undefined1 param_4)

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
    FUN_1088_0014(iVar1,uVar2,0);
    *(undefined1 *)(iVar1 + 3) = param_4;
    *(undefined1 *)(iVar1 + 4) = param_3;
    *(undefined1 *)(iVar1 + 2) = 0;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1048_00b6 @ 1048:00b6 */
/* ======================================================== */

void __stdcall16far FUN_1048_00b6(undefined4 param_1,int param_2)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  int unaff_BP;
  int iVar6;
  undefined2 uVar7;
  undefined2 unaff_SS;
  int iVar8;
  char local_1e;
  char local_1d;
  char local_1c;
  undefined1 local_1b;
  undefined2 local_18;
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  undefined2 local_c;
  int local_a;
  undefined2 local_8;
  undefined2 local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  if (DAT_10a8_1d2e == '\0') {
    uVar7 = (undefined2)((ulong)param_1 >> 0x10);
    iVar6 = (int)param_1;
    if ((((char *)s_American_Natural_Gas_Co__10a8_193e + 2)
         [(char)(*(char *)(iVar6 + 3) + 'A') * 0x1a + (uint)*(byte *)(iVar6 + 4)] == '\x02') &&
       (DAT_10a8_22cf != '\0')) {
      local_18 = DAT_10a8_2280;
    }
    else {
      local_18 = *(undefined2 *)(*(char *)(iVar6 + 2) * 2 + 0x2296);
    }
    iVar3 = param_2;
    uVar1 = CREATECOMPATIBLEDC();
    local_6 = uVar1;
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_18);
    uVar5 = 0;
    local_a = FUN_10a0_0baa(uVar1,iVar3);
    uVar1 = 0;
    local_8 = uVar5;
    iVar2 = FUN_10a0_0baa();
    iVar3 = DAT_10a8_38b4;
    iVar4 = DAT_10a8_38b0;
    iVar8 = param_2;
    local_e = iVar2;
    local_c = uVar1;
    BITBLT((char *)s_RandomShrs_10a8_109f + 1,0x20,0xcc,0,0,local_6,DAT_10a8_38b4,DAT_10a8_38b0,
           iVar2,local_a);
    if (*(char *)(iVar6 + 2) == '\0') {
      uVar1 = DAT_10a8_38c2;
      iVar3 = FUN_10a0_0be7(iVar8);
      iVar4 = FUN_10a0_0be7(iVar3,uVar1);
      local_14 = (iVar4 + local_e) - iVar3;
      uVar1 = DAT_10a8_38c2;
      iVar3 = FUN_10a0_0be7();
      iVar4 = FUN_10a0_0be7(iVar3,uVar1);
      local_10 = iVar4 + local_e + iVar3;
      local_16 = local_a;
      local_12 = local_a + DAT_10a8_38b0;
      local_1c = '\0';
      local_1e = *(char *)(iVar6 + 3) + 'A';
      if (*(byte *)(iVar6 + 4) < 9) {
        local_1d = *(char *)(iVar6 + 4) + '1';
      }
      else {
        local_1d = (char)((*(byte *)(iVar6 + 4) + 1) / 10 + 0x30);
        local_1c = (char)((*(byte *)(iVar6 + 4) + 1) % 10) + '0';
        local_1b = 0;
      }
      SETTEXTCOLOR((char *)s_RandomShrs_10a8_109f + 1,0xffff,0xff);
      iVar4 = 1;
      iVar2 = param_2;
      SETBKMODE((char *)s__Show_Graph_10a8_1226 + 10,1);
      if ((DAT_10a8_22cf == '\0') ||
         (((char *)s_American_Natural_Gas_Co__10a8_193e + 2)
          [(char)(*(char *)(iVar6 + 3) + 'A') * 0x1a + (uint)*(byte *)(iVar6 + 4)] != '\x02')) {
        iVar4 = param_2;
        DRAWTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x101,&local_16,unaff_SS,0xffff,&local_1e,
                 unaff_SS);
      }
      SETTEXTCOLOR((char *)s__Show_Graph_10a8_1226 + 10,0,0);
      iVar3 = param_2;
    }
    DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_6,iVar3,iVar4,iVar2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_02d2 @ 1048:02d2 */
/* ======================================================== */

void __stdcall16far FUN_1048_02d2(int *param_1,undefined1 param_2)

{
  undefined4 uVar1;
  undefined2 uVar2;
  int unaff_BP;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  char *pcVar6;
  
  iVar3 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  *(undefined1 *)((int *)param_1 + 1) = param_2;
  uVar1 = *(undefined4 *)((int)DAT_10a8_1b2e + 8);
  uVar5 = *(undefined2 *)((int)uVar1 + 4);
  uVar2 = GETDC();
  (*(code *)*(undefined2 *)(*param_1 + 0xc))
            ((char *)s__Show_Graph_10a8_1226 + 10,(int *)param_1,uVar4,uVar2,uVar5,uVar2,pcVar6,
             iVar3);
  RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar2);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_0321 @ 1048:0321 */
/* ======================================================== */

void __stdcall16far
FUN_1048_0321(undefined2 param_1,undefined2 param_2,char param_3,undefined2 param_4,
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
/* Function: FUN_1048_0393 @ 1048:0393 */
/* ======================================================== */

void __stdcall16far FUN_1048_0393(int *param_1)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  *(undefined1 *)(piVar2 + 0x1d) = 1;
  iVar1 = piVar2[0x1a];
  piVar2[0x13] = piVar2[0x19];
  piVar2[0x14] = iVar1;
  piVar2[0x15] = 0;
  piVar2[0x16] = 0;
  piVar2[0x17] = 0;
  piVar2[0x18] = 0;
  iVar1 = piVar2[0x14];
  piVar2[0x1b] = piVar2[0x13];
  piVar2[0x1c] = iVar1;
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  *(undefined1 *)(piVar2 + 0x1d) = 0;
  FUN_1078_0d9b(piVar2,uVar3);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_045f @ 1048:045f */
/* ======================================================== */

void __stdcall16far FUN_1048_045f(int *param_1,undefined4 param_2)

{
  uint uVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  uVar1 = piVar2[0x13] + piVar2[0x15];
  if ((((piVar2[0x14] + piVar2[0x16] + (uint)CARRY2(piVar2[0x13],piVar2[0x15]) + piVar2[0x18] +
         (uint)CARRY2(uVar1,piVar2[0x17]) == piVar2[0x1a]) && (uVar1 + piVar2[0x17] == piVar2[0x19])
       ) && (-1 < piVar2[0x14])) && ((-1 < piVar2[0x16] && (-1 < piVar2[0x18])))) {
    FUN_1080_0382(piVar2,uVar3,(int)param_2,(int)((ulong)param_2 >> 0x10));
  }
  else {
    MESSAGEBOX(unaff_CS,0,0xdae,(char *)s_RandomShrs_10a8_109f + 9,0xd9a,
               (char *)s_RandomShrs_10a8_109f + 9);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_0509 @ 1048:0509 */
/* ======================================================== */

void __stdcall16far FUN_1048_0509(int *param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int *piVar8;
  undefined2 uVar9;
  
  if (*(int *)((int)param_2 + 8) == 0) {
    uVar9 = (undefined2)((ulong)param_1 >> 0x10);
    piVar8 = (int *)param_1;
    uVar1 = piVar8[0x13];
    iVar2 = piVar8[0x14];
    uVar3 = piVar8[0x17];
    uVar7 = uVar1 + piVar8[0x17];
    iVar4 = piVar8[0x18];
    uVar5 = piVar8[0x19];
    iVar6 = piVar8[0x1a];
    piVar8[0x15] = uVar5 - uVar7;
    piVar8[0x16] = (iVar6 - (iVar2 + iVar4 + (uint)CARRY2(uVar1,uVar3))) - (uint)(uVar5 < uVar7);
    if (piVar8[0x16] < 0) {
      piVar8[0x15] = 0;
      piVar8[0x16] = 0;
    }
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x58))();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_058a @ 1048:058a */
/* ======================================================== */

void __stdcall16far FUN_1048_058a(int *param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int *piVar8;
  undefined2 uVar9;
  
  uVar9 = (undefined2)((ulong)param_1 >> 0x10);
  piVar8 = (int *)param_1;
  uVar1 = piVar8[0x15];
  iVar2 = piVar8[0x16];
  uVar3 = piVar8[0x17];
  uVar7 = uVar1 + piVar8[0x17];
  iVar4 = piVar8[0x18];
  uVar5 = piVar8[0x13];
  iVar6 = piVar8[0x14];
  piVar8[0x1b] = uVar7 + piVar8[0x13];
  piVar8[0x1c] = iVar2 + iVar4 + (uint)CARRY2(uVar1,uVar3) + iVar6 + (uint)CARRY2(uVar7,uVar5);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  return;
}

/* ======================================================== */
/* Function: FUN_1048_05ce @ 1048:05ce */
/* ======================================================== */

void __stdcall16far FUN_1048_05ce(int *param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int *piVar8;
  undefined2 uVar9;
  
  if (*(int *)((int)param_2 + 8) == 0) {
    uVar9 = (undefined2)((ulong)param_1 >> 0x10);
    piVar8 = (int *)param_1;
    uVar1 = piVar8[0x15];
    iVar2 = piVar8[0x16];
    uVar3 = piVar8[0x13];
    uVar7 = uVar1 + piVar8[0x13];
    iVar4 = piVar8[0x14];
    uVar5 = piVar8[0x19];
    iVar6 = piVar8[0x1a];
    piVar8[0x17] = uVar5 - uVar7;
    piVar8[0x18] = (iVar6 - (iVar2 + iVar4 + (uint)CARRY2(uVar1,uVar3))) - (uint)(uVar5 < uVar7);
    if (piVar8[0x18] < 0) {
      piVar8[0x17] = 0;
      piVar8[0x18] = 0;
    }
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x58))();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_064f @ 1048:064f */
/* ======================================================== */

void __stdcall16far FUN_1048_064f(int *param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  
  if (*(int *)((int)param_2 + 8) == 0) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    piVar2 = (int *)param_1;
    *(undefined1 *)(piVar2 + 0x1d) = 1;
    piVar2[0x17] = 0;
    piVar2[0x18] = 0;
    piVar2[0x15] = 0;
    piVar2[0x16] = 0;
    iVar1 = piVar2[0x1a];
    piVar2[0x13] = piVar2[0x19];
    piVar2[0x14] = iVar1;
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x58))();
    *(undefined1 *)(piVar2 + 0x1d) = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_06ec @ 1048:06ec */
/* ======================================================== */

void __stdcall16far FUN_1048_06ec(int *param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  
  if (*(int *)((int)param_2 + 8) == 0) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    piVar2 = (int *)param_1;
    *(undefined1 *)(piVar2 + 0x1d) = 1;
    iVar1 = piVar2[0x1a];
    piVar2[0x17] = piVar2[0x19];
    piVar2[0x18] = iVar1;
    piVar2[0x15] = 0;
    piVar2[0x16] = 0;
    piVar2[0x13] = 0;
    piVar2[0x14] = 0;
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x58))();
    *(undefined1 *)(piVar2 + 0x1d) = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_0789 @ 1048:0789 */
/* ======================================================== */

void __stdcall16far FUN_1048_0789(int *param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  
  if (*(int *)((int)param_2 + 8) == 0) {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    piVar2 = (int *)param_1;
    *(undefined1 *)(piVar2 + 0x1d) = 1;
    piVar2[0x17] = 0;
    piVar2[0x18] = 0;
    iVar1 = piVar2[0x1a];
    piVar2[0x15] = piVar2[0x19];
    piVar2[0x16] = iVar1;
    piVar2[0x13] = 0;
    piVar2[0x14] = 0;
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    (*(code *)*(undefined2 *)(*param_1 + 0x58))();
    *(undefined1 *)(piVar2 + 0x1d) = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_0826 @ 1048:0826 */
/* ======================================================== */

void __stdcall16far FUN_1048_0826(int *param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int *piVar8;
  undefined2 uVar9;
  
  uVar9 = (undefined2)((ulong)param_1 >> 0x10);
  piVar8 = (int *)param_1;
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  uVar1 = piVar8[0x17];
  iVar2 = piVar8[0x18];
  uVar3 = piVar8[0x13];
  uVar7 = uVar1 + piVar8[0x13];
  iVar4 = piVar8[0x14];
  uVar5 = piVar8[0x15];
  iVar6 = piVar8[0x16];
  piVar8[0x1b] = uVar7 + piVar8[0x15];
  piVar8[0x1c] = iVar2 + iVar4 + (uint)CARRY2(uVar1,uVar3) + iVar6 + (uint)CARRY2(uVar7,uVar5);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  return;
}

/* ======================================================== */
/* Function: FUN_1048_08b2 @ 1048:08b2 */
/* ======================================================== */

void __stdcall16far FUN_1048_08b2(int *param_1,undefined4 param_2)

{
  byte bVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  if ((((char)((int *)param_1)[0x1d] == '\0') && (bVar1 = *(byte *)((int)param_2 + 4), 100 < bVar1))
     && ((bVar1 < 0x67 || (bVar1 == 0x6c)))) {
    (*(code *)*(undefined2 *)(*param_1 + 0x5c))();
  }
  FUN_1078_08f0((int *)param_1,uVar2,(int)param_2,uVar3);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_08f5 @ 1048:08f5 */
/* ======================================================== */

void __cdecl16far FUN_1048_08f5(void)

{
  MESSAGEBEEP();
  MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0xdd0,(char *)s_RandomShrs_10a8_109f + 9,
             0xdba,(char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_0923 @ 1048:0923 */
/* ======================================================== */

undefined4 FUN_1048_0923(undefined2 param_1,char param_2)

{
  undefined2 local_8;
  undefined2 local_4;
  
  if ((param_2 == '\x01') || (param_2 == '\x02')) {
    local_8 = 0x86;
    local_4 = 0x2000;
  }
  else if ((param_2 == '\x03') || ((param_2 == '\x05' || (param_2 == '\x04')))) {
    local_8 = 0x85;
    local_4 = 0x2000;
  }
  else if ((param_2 == '\b') || ((param_2 == '\x06' || (param_2 == '\a')))) {
    local_8 = 0x83;
    local_4 = 0x7000;
  }
  return CONCAT22(local_4,local_8);
}

/* ======================================================== */
/* Function: FUN_1048_0988 @ 1048:0988 */
/* ======================================================== */

undefined2 __cdecl16far FUN_1048_0988(void)

{
  uint uVar1;
  undefined1 extraout_AH;
  uint uVar2;
  undefined2 uVar3;
  undefined1 extraout_AH_00;
  undefined1 extraout_AH_01;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  int unaff_BP;
  int iVar8;
  bool bVar9;
  undefined1 uVar10;
  bool bVar11;
  undefined1 uVar12;
  undefined1 uVar13;
  undefined2 uVar14;
  char local_8;
  char local_7;
  char local_6;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  local_7 = '\x01';
  while( true ) {
    bVar9 = false;
    bVar11 = true;
    FUN_10a0_1454();
    if (bVar9 || bVar11) break;
    local_7 = local_7 + '\x01';
  }
  local_6 = '\x01';
  while( true ) {
    bVar9 = true;
    FUN_10a0_1454();
    uVar10 = extraout_AH;
    if (bVar9) {
      iVar4 = DAT_10a8_22c0;
      uVar2 = FUN_10a0_0be7();
      iVar8 = DAT_10a8_319a * 0xfc + local_6 * 4;
      uVar1 = *(uint *)(iVar8 + 0x3182);
      iVar8 = *(int *)(iVar8 + 0x3184);
      if ((iVar4 < iVar8) || ((iVar4 <= iVar8 && (uVar2 < uVar1)))) {
        uVar10 = (undefined1)(uVar1 >> 8);
        local_7 = local_6;
      }
      else {
        uVar6 = 0;
        iVar8 = DAT_10a8_22c0;
        FUN_10a0_0baa();
        FUN_10a0_1458();
        uVar12 = 1;
        uVar3 = FUN_10a0_144a();
        FUN_1048_0923(&iStack_2,local_6);
        uVar10 = (long)(int)((long)DAT_10a8_319a * 0xfc) != (long)DAT_10a8_319a * 0xfc;
        FUN_10a0_144a(uVar3,uVar6,iVar8);
        FUN_10a0_1454();
        if ((bool)uVar10 || (bool)uVar12) {
          iVar8 = DAT_10a8_319a * 0xfc;
          uVar3 = *(undefined2 *)(iVar8 + 0x30ca);
          local_8 = '\x01';
          uVar6 = *(undefined2 *)(iVar8 + 0x30cc);
          uVar14 = *(undefined2 *)(iVar8 + 0x30ce);
          while( true ) {
            iVar8 = local_8 * 0x24;
            uVar7 = *(undefined2 *)(iVar8 + 0x3774);
            uVar5 = *(undefined2 *)(DAT_10a8_319a * 0xfc + local_8 * 4 + 0x3184);
            FUN_10a0_1458(*(undefined2 *)(iVar8 + 0x3772),uVar7,*(undefined2 *)(iVar8 + 0x3776),
                          uVar3,uVar6,uVar14);
            FUN_10a0_1444();
            uVar3 = FUN_10a0_1432();
            if (local_8 == '\b') break;
            local_8 = local_8 + '\x01';
            uVar6 = uVar7;
            uVar14 = uVar5;
          }
          uVar6 = 0;
          iVar8 = DAT_10a8_22c0;
          FUN_10a0_0baa();
          FUN_10a0_1458();
          uVar12 = 0;
          uVar13 = 1;
          uVar3 = FUN_10a0_144a();
          FUN_1048_0923(&iStack_2,local_6);
          FUN_10a0_144a(uVar3,uVar6,iVar8);
          FUN_10a0_1454();
          uVar10 = extraout_AH_01;
          if (!(bool)uVar12 && !(bool)uVar13) {
            local_7 = local_6;
          }
        }
        else {
          local_7 = local_6;
          uVar10 = extraout_AH_00;
        }
      }
    }
    if (local_6 == '\b') break;
    local_6 = local_6 + '\x01';
  }
  return CONCAT11(uVar10,local_7);
}

/* ======================================================== */
/* Function: FUN_1048_0b6b @ 1048:0b6b */
/* ======================================================== */

undefined1 __stdcall16far FUN_1048_0b6b(char param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int unaff_BP;
  undefined1 uVar4;
  undefined1 uVar5;
  undefined1 uVar6;
  
  uVar6 = 0;
  uVar3 = *(undefined2 *)(param_1 * 0x24 + 0x3774);
  uVar2 = *(undefined2 *)(param_1 * 0x24 + 0x3776);
  uVar1 = FUN_10a0_1444(0,(char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  uVar4 = 0;
  uVar5 = 1;
  FUN_10a0_144a(uVar1,uVar3,uVar2);
  FUN_10a0_1454();
  if (!(bool)uVar4 && !(bool)uVar5) {
    uVar6 = 1;
  }
  return uVar6;
}

/* ======================================================== */
/* Function: FUN_1048_0bd8 @ 1048:0bd8 */
/* ======================================================== */

undefined4 __stdcall16far FUN_1048_0bd8(char param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 local_c;
  
  iVar3 = param_1 * 0x24;
  uVar2 = *(undefined2 *)(iVar3 + 0x3772);
  uVar1 = *(undefined2 *)(iVar3 + 0x3774);
  local_c = *(undefined2 *)(iVar3 + 0x3776);
  if ((param_1 == '\x01') || (param_1 == '\x02')) {
    local_c = 0x4ccc;
    FUN_10a0_1444(uVar2,uVar1);
    FUN_10a0_1444();
    uVar2 = FUN_10a0_1432();
  }
  else if ((param_1 == '\x03') || ((param_1 == '\x04' || (param_1 == '\x05')))) {
    local_c = 0;
    FUN_10a0_1444(uVar2,uVar1);
    FUN_10a0_1444();
    uVar2 = FUN_10a0_1432();
  }
  else if ((param_1 == '\b') || ((param_1 == '\a' || (param_1 == '\x06')))) {
    local_c = 0x6666;
    FUN_10a0_1444(uVar2,uVar1);
    FUN_10a0_1444();
    uVar2 = FUN_10a0_1432();
  }
  return CONCAT22(local_c,uVar2);
}

/* ======================================================== */
/* Function: FUN_1048_0cf2 @ 1048:0cf2 */
/* ======================================================== */

void __cdecl16far FUN_1048_0cf2(void)

{
  char *unaff_CS;
  char local_5;
  
  local_5 = '\0';
  while( true ) {
    if (local_5 == '\0') {
      uRam10a82282 = LOADBITMAP(unaff_CS,0xde0,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a82296 = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xde0,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\x01') {
      uRam10a82284 = LOADBITMAP(unaff_CS,0xde5,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a82298 = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xde5,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\x02') {
      uRam10a82286 = LOADBITMAP(unaff_CS,0xdee,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a8229a = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xdee,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\x03') {
      uRam10a82288 = LOADBITMAP(unaff_CS,0xdf4,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a8229c = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xdf4,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\x04') {
      uRam10a8228a = LOADBITMAP(unaff_CS,0xdfd,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a8229e = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xdfd,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\x05') {
      uRam10a8228c = LOADBITMAP(unaff_CS,0xe05,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a822a0 = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xe05,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\x06') {
      uRam10a8228e = LOADBITMAP(unaff_CS,0xe0b,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a822a2 = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xe0b,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\a') {
      uRam10a82290 = LOADBITMAP(unaff_CS,0xe11,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a822a4 = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xe11,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\b') {
      uRam10a82292 = LOADBITMAP(unaff_CS,0xe19,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a822a6 = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xe19,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    else if (local_5 == '\t') {
      uRam10a82294 = LOADBITMAP(unaff_CS,0xe20,(char *)s_RandomShrs_10a8_109f + 9);
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      uRam10a822a8 = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xe20,
                                (char *)s_RandomShrs_10a8_109f + 9);
    }
    if (local_5 == '\t') break;
    local_5 = local_5 + '\x01';
  }
  DAT_10a8_227e = LOADBITMAP(unaff_CS,0xe26,(char *)s_RandomShrs_10a8_109f + 9);
  DAT_10a8_2280 =
       LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xe26,(char *)s_RandomShrs_10a8_109f + 9);
  DAT_10a8_39a5 = 1;
  return;
}

/* ======================================================== */
/* Function: FUN_1048_0fac @ 1048:0fac */
/* ======================================================== */

undefined4 __stdcall16far FUN_1048_0fac(char param_1)

{
  uint uVar1;
  int iVar2;
  bool bVar3;
  undefined2 local_10;
  undefined2 local_e;
  undefined2 local_c;
  undefined2 local_a;
  
  local_10 = 0;
  local_e = 0;
  if ((DAT_10a8_22ac < 0x8000) && ((0 < (int)DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
    local_c = 1;
    local_a = 0;
    while( true ) {
      iVar2 = local_c * 0xfc + param_1 * 4;
      uVar1 = *(uint *)(iVar2 + 0x3182);
      bVar3 = CARRY2(uVar1,local_10);
      local_10 = uVar1 + local_10;
      local_e = *(int *)(iVar2 + 0x3184) + local_e + (uint)bVar3;
      if ((local_a == DAT_10a8_22ac) && (local_c == DAT_10a8_22aa)) break;
      bVar3 = 0xfffe < local_c;
      local_c = local_c + 1;
      local_a = local_a + bVar3;
    }
  }
  return CONCAT22(local_e,local_10);
}

/* ======================================================== */
/* Function: FUN_1048_1035 @ 1048:1035 */
/* ======================================================== */

undefined4 __stdcall16far FUN_1048_1035(undefined1 *param_1)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 unaff_SS;
  bool bVar6;
  bool bVar7;
  uint local_12e;
  int local_12c;
  char local_129;
  undefined1 local_128 [292];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = local_128;
  puVar4 = (undefined1 *)param_1;
  for (iVar3 = 0x120; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  local_12e = 0;
  local_12c = 0;
  local_129 = '\x01';
  while( true ) {
    bVar6 = false;
    bVar7 = true;
    FUN_10a0_1454();
    if (!bVar6 && !bVar7) {
      bVar6 = 0xfffe < local_12e;
      local_12e = local_12e + 1;
      local_12c = local_12c + (uint)bVar6;
    }
    if (local_129 == '\b') break;
    local_129 = local_129 + '\x01';
  }
  return CONCAT22(local_12c,local_12e);
}

/* ======================================================== */
/* Function: FUN_1048_10c1 @ 1048:10c1 */
/* ======================================================== */

void __stdcall16far FUN_1048_10c1(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 in_BX;
  int iVar3;
  undefined2 uVar4;
  char local_5;
  
  local_5 = '\x01';
  while( true ) {
    uVar4 = (undefined2)((ulong)param_1 >> 0x10);
    iVar3 = (int)param_1 + local_5 * 0x24;
    *(undefined2 *)(iVar3 + -0x24) = 0;
    *(undefined2 *)(iVar3 + -0x22) = 0;
    *(undefined2 *)(iVar3 + -0x20) = 0;
    *(undefined2 *)(iVar3 + -0x1e) = 0;
    *(undefined2 *)(iVar3 + -0x1c) = 0;
    *(undefined2 *)(iVar3 + -0x1a) = 0;
    if ((local_5 == '\x01') || (local_5 == '\x02')) {
      *(undefined2 *)(iVar3 + -0x18) = 0x86;
      *(undefined2 *)(iVar3 + -0x16) = 0;
      *(undefined2 *)(iVar3 + -0x14) = 0x2000;
      if (DAT_10a8_22cb != '\0') {
        FUN_10a0_18f1(0x28);
        uVar2 = 0;
        uVar1 = FUN_10a0_1458();
        *(undefined2 *)(iVar3 + -0x18) = uVar1;
        *(undefined2 *)(iVar3 + -0x16) = in_BX;
        *(undefined2 *)(iVar3 + -0x14) = uVar2;
      }
      FUN_10a0_18f1((undefined2 *)&DAT_10a8_1b58);
      uVar2 = 0;
      FUN_10a0_1458();
      FUN_10a0_144a();
      uVar1 = FUN_10a0_1432();
      *(undefined2 *)(iVar3 + -0x12) = uVar1;
      *(undefined2 *)(iVar3 + -0x10) = in_BX;
      *(undefined2 *)(iVar3 + -0xe) = uVar2;
      in_BX = *(undefined2 *)(iVar3 + -0x10);
      uVar1 = *(undefined2 *)(iVar3 + -0xe);
      FUN_10a0_144a();
      uVar2 = FUN_10a0_144a();
      *(undefined2 *)(iVar3 + -0xc) = uVar2;
      *(undefined2 *)(iVar3 + -10) = in_BX;
      *(undefined2 *)(iVar3 + -8) = uVar1;
      *(undefined2 *)(iVar3 + -6) = 0x82;
      *(undefined2 *)(iVar3 + -4) = 0;
      *(undefined2 *)(iVar3 + -2) = 0;
    }
    else if (((local_5 == '\x03') || (local_5 == '\x05')) || (local_5 == '\x04')) {
      *(undefined2 *)(iVar3 + -0x18) = 0x85;
      *(undefined2 *)(iVar3 + -0x16) = 0;
      *(undefined2 *)(iVar3 + -0x14) = 0x2000;
      if (DAT_10a8_22cb != '\0') {
        FUN_10a0_18f1(0x14);
        uVar2 = 0;
        uVar1 = FUN_10a0_1458();
        *(undefined2 *)(iVar3 + -0x18) = uVar1;
        *(undefined2 *)(iVar3 + -0x16) = in_BX;
        *(undefined2 *)(iVar3 + -0x14) = uVar2;
      }
      FUN_10a0_18f1(2000);
      uVar2 = 0;
      FUN_10a0_1458();
      FUN_10a0_144a();
      uVar1 = FUN_10a0_1432();
      *(undefined2 *)(iVar3 + -0x12) = uVar1;
      *(undefined2 *)(iVar3 + -0x10) = in_BX;
      *(undefined2 *)(iVar3 + -0xe) = uVar2;
      in_BX = *(undefined2 *)(iVar3 + -0x10);
      uVar1 = *(undefined2 *)(iVar3 + -0xe);
      FUN_10a0_144a();
      uVar2 = FUN_10a0_144a();
      *(undefined2 *)(iVar3 + -0xc) = uVar2;
      *(undefined2 *)(iVar3 + -10) = in_BX;
      *(undefined2 *)(iVar3 + -8) = uVar1;
      *(undefined2 *)(iVar3 + -6) = 0x82;
      *(undefined2 *)(iVar3 + -4) = 0;
      *(undefined2 *)(iVar3 + -2) = 0x4000;
    }
    else if (((local_5 == '\b') || (local_5 == '\x06')) || (local_5 == '\a')) {
      *(undefined2 *)(iVar3 + -0x18) = 0x83;
      *(undefined2 *)(iVar3 + -0x16) = 0;
      *(undefined2 *)(iVar3 + -0x14) = 0x7000;
      if (DAT_10a8_22cb != '\0') {
        FUN_10a0_18f1(9);
        uVar2 = 0;
        uVar1 = FUN_10a0_1458();
        *(undefined2 *)(iVar3 + -0x18) = uVar1;
        *(undefined2 *)(iVar3 + -0x16) = in_BX;
        *(undefined2 *)(iVar3 + -0x14) = uVar2;
      }
      FUN_10a0_18f1(1000);
      uVar2 = 0;
      FUN_10a0_1458();
      FUN_10a0_144a();
      uVar1 = FUN_10a0_1432();
      *(undefined2 *)(iVar3 + -0x12) = uVar1;
      *(undefined2 *)(iVar3 + -0x10) = in_BX;
      *(undefined2 *)(iVar3 + -0xe) = uVar2;
      in_BX = *(undefined2 *)(iVar3 + -0x10);
      uVar1 = *(undefined2 *)(iVar3 + -0xe);
      FUN_10a0_144a();
      uVar2 = FUN_10a0_144a();
      *(undefined2 *)(iVar3 + -0xc) = uVar2;
      *(undefined2 *)(iVar3 + -10) = in_BX;
      *(undefined2 *)(iVar3 + -8) = uVar1;
      *(undefined2 *)(iVar3 + -6) = 0xd182;
      *(undefined2 *)(iVar3 + -4) = 0x425e;
      *(undefined2 *)(iVar3 + -2) = 0x497b;
    }
    else {
      *(undefined2 *)(iVar3 + -0x18) = 0x86;
      *(undefined2 *)(iVar3 + -0x16) = 0;
      *(undefined2 *)(iVar3 + -0x14) = 0x4800;
    }
    if (local_5 == '\b') break;
    local_5 = local_5 + '\x01';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_13ad @ 1048:13ad */
/* ======================================================== */

void __cdecl16far FUN_1048_13ad(void)

{
  MESSAGEBEEP();
  MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0xe5e,(char *)s_RandomShrs_10a8_109f + 9,
             0xe36,(char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_13db @ 1048:13db */
/* ======================================================== */

void __cdecl16far FUN_1048_13db(void)

{
  char *unaff_CS;
  char local_5;
  
  if (DAT_10a8_39a5 != '\0') {
    local_5 = '\0';
    while( true ) {
      DELETEOBJECT(unaff_CS,*(undefined2 *)(local_5 * 2 + 0x2282));
      unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
      DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(local_5 * 2 + 0x2296));
      if (local_5 == '\t') break;
      local_5 = local_5 + '\x01';
    }
    DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_227e);
    DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_2280);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_1432 @ 1048:1432 */
/* ======================================================== */

void __cdecl16far FUN_1048_1432(void)

{
  uint uVar1;
  uint uVar2;
  undefined2 uVar3;
  bool bVar4;
  uint local_a;
  uint local_8;
  char local_5;
  
  local_5 = 'A';
  while( true ) {
    uVar2 = DAT_10a8_22b8;
    uVar1 = DAT_10a8_22b6;
    if ((DAT_10a8_22b8 < 0x8000) && ((0 < (int)DAT_10a8_22b8 || (DAT_10a8_22b6 != 0)))) {
      local_a = 1;
      local_8 = 0;
      while( true ) {
        *(undefined1 *)(local_5 * 0x1a + local_a + 0x169b) = 1;
        ((char *)s_American_Natural_Gas_Co__10a8_193e + 1)[local_5 * 0x1a + local_a] = '\x01';
        if ((local_8 == uVar2) && (local_a == uVar1)) break;
        bVar4 = 0xfffe < local_a;
        local_a = local_a + 1;
        local_8 = local_8 + bVar4;
      }
    }
    if (local_5 == 'Z') break;
    local_5 = local_5 + '\x01';
  }
  uVar3 = DAT_10a8_22b4;
  DAT_10a8_1d32 = FUN_10a0_0baa();
  DAT_10a8_1d34 = uVar3;
  return;
}

/* ======================================================== */
/* Function: FUN_1048_14c9 @ 1048:14c9 */
/* ======================================================== */

void __stdcall16far FUN_1048_14c9(undefined2 param_1)

{
  undefined4 uVar1;
  int iVar2;
  int unaff_BP;
  int iVar3;
  char *pcVar4;
  undefined2 unaff_SS;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  int local_cc;
  int local_ca;
  int local_c8;
  int local_c6;
  undefined1 local_c4 [22];
  undefined1 local_ae [152];
  undefined1 local_16 [2];
  undefined2 local_14;
  undefined2 local_12;
  undefined2 local_8;
  undefined2 local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  local_6 = CREATECOMPATIBLEDC();
  uVar8 = DAT_10a8_1c9e;
  local_8 = LOADBITMAP((char *)s__Show_Graph_10a8_1226 + 10,0xe7e,(char *)s_RandomShrs_10a8_109f + 9
                      );
  uVar7 = local_6;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_8);
  uVar6 = local_8;
  GETOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_16,unaff_SS,0xe);
  uVar1 = *(undefined4 *)((int)DAT_10a8_1b2e + 8);
  uVar5 = *(undefined2 *)((int)uVar1 + 4);
  GETCLIENTRECT((char *)s__Show_Graph_10a8_1226 + 10,&local_cc,unaff_SS);
  local_c6 = (local_c6 - DAT_10a8_399c) + -3;
  local_cc = FUN_10a0_0baa(uVar5,uVar6,uVar7,uVar8);
  local_cc = local_cc + 2;
  local_ca = FUN_10a0_0baa();
  local_c8 = FUN_10a0_0baa();
  local_c8 = local_cc + local_c8;
  uVar5 = param_1;
  STRETCHBLT((char *)s_RandomShrs_10a8_109f + 1,0x20,0xcc,local_12,local_14,0,0,local_6,
             local_c6 - local_ca,local_c8 - local_cc,local_ca,local_cc);
  DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_6,uVar5);
  DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_8);
  FUN_1098_0055(DAT_10a8_319a * 0xfc + 0x30aa,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
  FUN_1098_00bd(0xe8a,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
  if (DAT_10a8_39a8 == '\0') {
    FUN_1098_00bd(0xe96,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
  }
  else {
    FUN_1098_00bd(0xe8d,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
  }
  FUN_1098_00bd(0xe8a,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
  FUN_1098_00bd(0xea2,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
  iVar3 = DAT_10a8_319a * 0xfc;
  FUN_10a0_0af5(0x14,local_c4,unaff_SS,2,7,*(undefined2 *)(iVar3 + 0x30ca),
                *(undefined2 *)(iVar3 + 0x30cc),*(undefined2 *)(iVar3 + 0x30ce));
  FUN_1098_00bd(local_c4,unaff_SS,local_ae,unaff_SS);
  uVar6 = param_1;
  SETTEXTCOLOR((char *)s_RandomDivs_10a8_1094 + 4,0xffff,0xff);
  uVar5 = param_1;
  SETBKMODE((char *)s__Show_Graph_10a8_1226 + 10,1);
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_378e);
  FUN_10a0_0baa(param_1,uVar5,uVar6);
  iVar3 = FUN_10a0_0be7();
  iVar3 = local_ca + ((local_c6 - local_ca) / 2 - iVar3);
  FUN_10a0_0baa();
  pcVar4 = (char *)s_RandomShrs_10a8_109f + 1;
  iVar2 = FUN_10a0_0be7();
  local_c6 = local_ca + (local_c6 - local_ca) / 2 + iVar2;
  local_ca = iVar3;
  if (DAT_10a8_1d2f != '\0') {
    local_ae[0] = 0;
    FUN_1098_00bd(DAT_10a8_0e6c,DAT_10a8_0e6e,local_ae,unaff_SS);
    FUN_1098_00bd(0xeaa,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
    pcVar4 = (char *)s_RandomDivs_10a8_1094 + 4;
    FUN_1098_00bd(0xead,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
  }
  if (DAT_10a8_39a9 != '\0') {
    local_ae[0] = 0;
    FUN_1098_00bd(DAT_10a8_319a * 0xfc + 0x30aa,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS
                 );
    FUN_1098_00bd(0xeb5,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
    pcVar4 = (char *)s_RandomDivs_10a8_1094 + 4;
    FUN_1098_00bd(0xec5,(char *)s_RandomShrs_10a8_109f + 9,local_ae,unaff_SS);
  }
  DRAWTEXT(pcVar4,0x101,&local_cc,unaff_SS,0xffff,local_ae,unaff_SS);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_17cd @ 1048:17cd */
/* ======================================================== */

void __stdcall16far FUN_1048_17cd(undefined2 param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  int unaff_BP;
  int iVar6;
  undefined2 unaff_SS;
  bool bVar7;
  bool bVar8;
  undefined4 local_86;
  undefined1 local_82 [52];
  undefined1 local_4e [52];
  int local_1a;
  int local_18;
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  int local_e;
  undefined2 local_c;
  undefined2 local_a;
  undefined2 local_8;
  char local_5;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = param_1;
  local_8 = CREATECOMPATIBLEDC();
  uVar1 = param_1;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_39a0);
  uVar1 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,7,param_1,uVar1,uVar4);
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
  uVar1 = param_1;
  SETBKCOLOR((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_19ba,DAT_10a8_19bc);
  local_1a = FUN_10a0_0baa(uVar1);
  local_18 = 1;
  local_14 = FUN_10a0_0baa();
  local_14 = local_18 + local_14;
  local_16 = FUN_10a0_0baa();
  local_16 = local_1a + local_16;
  uVar4 = param_1;
  DRAWTEXT((char *)s_RandomShrs_10a8_109f + 1,1,&local_1a,unaff_SS,0xffff,0xed6,
           (char *)s_RandomShrs_10a8_109f + 9);
  uVar1 = DAT_10a8_38b2;
  local_e = FUN_10a0_0baa(uVar4);
  local_5 = '\x01';
  local_c = uVar1;
  while( true ) {
    iVar6 = local_5 * 0x24;
    local_86 = (undefined2 *)
               CONCAT22((char *)s_RandomShrs_10a8_109f + 9,(undefined2 *)(iVar6 + 0x376c));
    uVar1 = *(undefined2 *)(iVar6 + 0x3774);
    bVar7 = false;
    bVar8 = true;
    FUN_10a0_1454();
    if (bVar7 || bVar8) {
      uVar4 = DAT_10a8_38b6;
      FUN_10a0_1458();
      uVar2 = FUN_10a0_1444();
      iVar6 = (int)local_5 >> 0xf;
      FUN_10a0_0baa(uVar2,uVar1,uVar4);
      FUN_10a0_1458();
      FUN_10a0_1444();
      FUN_10a0_1438();
      local_12 = FUN_10a0_145c();
      local_10 = iVar6;
      uVar1 = GETSTOCKOBJECT((char *)s_RandomShrs_10a8_109f + 1,8,param_1);
      SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
      iVar6 = local_12;
      iVar5 = local_e;
      iVar3 = FUN_10a0_0baa(local_12,local_e,param_1);
      RECTANGLE((char *)s_RandomShrs_10a8_109f + 1,local_12 + DAT_10a8_38b4 + 1,iVar3 + local_e,
                iVar6,iVar5);
      uVar1 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,7,param_1);
      SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
    }
    else {
      local_a = *(undefined2 *)(local_5 * 2 + 0x2296);
      uVar2 = local_8;
      SELECTOBJECT((char *)s_RandomShrs_10a8_109f + 1,local_a);
      uVar4 = DAT_10a8_38b6;
      FUN_10a0_1458(uVar2);
      uVar2 = FUN_10a0_1444();
      iVar5 = (int)local_5 >> 0xf;
      FUN_10a0_0baa(uVar2,uVar1,uVar4);
      FUN_10a0_1458();
      FUN_10a0_1444();
      FUN_10a0_1438();
      local_12 = FUN_10a0_145c();
      local_10 = iVar5;
      BITBLT((char *)s_RandomShrs_10a8_109f + 1,0x20,0xcc,0,0,local_8,DAT_10a8_38b4,DAT_10a8_38b0,
             local_12,local_e);
      FUN_10a0_0af5(0x32,local_82,unaff_SS,2,5,*(undefined2 *)(iVar6 + 0x3772),
                    *(undefined2 *)(iVar6 + 0x3774),*(undefined2 *)(iVar6 + 0x3776));
      FUN_1098_0055(0xee3,(char *)s_RandomShrs_10a8_109f + 9,local_4e,unaff_SS);
      FUN_1098_00bd(local_82,unaff_SS,local_4e,unaff_SS);
      if (DAT_10a8_22c7 != '\0') {
        FUN_1098_00bd(0xee9,(char *)s_RandomShrs_10a8_109f + 9,local_4e,unaff_SS);
        FUN_10a0_0af5(0x32,local_82,unaff_SS,2,5,*local_86,*(undefined2 *)(iVar6 + 0x376e),
                      *(undefined2 *)(iVar6 + 0x3770));
        FUN_1098_00bd(0xeec,(char *)s_RandomShrs_10a8_109f + 9,local_4e,unaff_SS);
        FUN_1098_00bd(local_82,unaff_SS,local_4e,unaff_SS);
      }
      local_1a = FUN_10a0_0baa();
      iVar6 = FUN_10a0_0baa();
      local_16 = local_1a + iVar6 + 10;
      local_18 = local_12;
      local_14 = local_12 + DAT_10a8_38b4;
      DRAWTEXT((char *)s_RandomShrs_10a8_109f + 1,4,&local_1a,unaff_SS,0xffff,local_4e,unaff_SS);
    }
    if (local_5 == '\b') break;
    local_5 = local_5 + '\x01';
  }
  DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_8);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_1b39 @ 1048:1b39 */
/* ======================================================== */

undefined4 FUN_1048_1b39(int param_1,undefined2 param_2,undefined2 param_3)

{
  undefined2 unaff_SS;
  
  FUN_10a0_0a72(0x14,param_1 + -0x3e,unaff_SS,0,param_2,param_3);
  return CONCAT22(unaff_SS,param_1 + -0x3e);
}

/* ======================================================== */
/* Function: FUN_1048_1b6e @ 1048:1b6e */
/* ======================================================== */

void FUN_1048_1b6e(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
                  undefined2 param_5)

{
  undefined2 unaff_CS;
  undefined4 uVar1;
  undefined2 uVar2;
  
  uVar2 = 0xef2;
  uVar1 = FUN_1048_1b39(param_1,param_2,param_3);
  WRITEPRIVATEPROFILESTRING
            (unaff_CS,0xefc,(char *)s_RandomShrs_10a8_109f + 9,uVar1,param_4,param_5,uVar2);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_1b99 @ 1048:1b99 */
/* ======================================================== */

void FUN_1048_1b99(undefined2 param_1,char param_2,undefined2 param_3,undefined2 param_4)

{
  undefined2 unaff_CS;
  
  if (param_2 == '\0') {
    WRITEPRIVATEPROFILESTRING
              (unaff_CS,0xf19,(char *)s_RandomShrs_10a8_109f + 9,0xf26,
               (char *)s_RandomShrs_10a8_109f + 9,param_3,param_4,0xf0a);
  }
  else {
    WRITEPRIVATEPROFILESTRING
              (unaff_CS,0xf19,(char *)s_RandomShrs_10a8_109f + 9,0xf14,
               (char *)s_RandomShrs_10a8_109f + 9,param_3,param_4,0xf0a);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1048_1bdc @ 1048:1bdc */
/* ======================================================== */

void __stdcall16far FUN_1048_1bdc(undefined1 *param_1)

{
  undefined1 *puVar1;
  undefined2 *puVar2;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  undefined1 extraout_AH_01;
  undefined1 extraout_AH_02;
  undefined1 extraout_AH_03;
  undefined1 extraout_AH_04;
  undefined1 extraout_AH_05;
  undefined1 extraout_AH_06;
  undefined1 extraout_AH_07;
  undefined1 extraout_AH_08;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined2 *puVar5;
  undefined2 unaff_SS;
  undefined2 local_2a;
  undefined2 local_28;
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
  undefined1 local_c;
  undefined1 local_b;
  undefined1 local_a;
  undefined1 local_9;
  undefined1 local_8;
  undefined1 local_7;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = &local_2a;
  puVar4 = (undefined1 *)param_1;
  for (iVar3 = 0x26; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = (undefined2 *)((int)puVar5 + 1);
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *(undefined1 *)puVar2 = *puVar1;
  }
  FUN_1048_1b6e(&iStack_2,local_2a,local_28,0xf2c,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b6e(&iStack_2,local_26,local_24,0xf37,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b6e(&iStack_2,local_22,local_20,0xf40,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b6e(&iStack_2,local_1e,local_1c,0xf48,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b6e(&iStack_2,local_1a,local_18,0xf4d,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b6e(&iStack_2,local_16,local_14,0xf57,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b6e(&iStack_2,local_12,local_10,0xf68,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH,local_e),0xf7c,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_00,local_d),0xf83,(char *)s_RandomShrs_10a8_109f + 9)
  ;
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_01,local_c),0xf90,(char *)s_RandomShrs_10a8_109f + 9)
  ;
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_02,local_b),0xf97,(char *)s_RandomShrs_10a8_109f + 9)
  ;
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_03,local_a),0xf9e,(char *)s_RandomShrs_10a8_109f + 9)
  ;
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_04,local_9),0xfa9,(char *)s_RandomShrs_10a8_109f + 9)
  ;
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_05,local_8),0xfb4,(char *)s_RandomShrs_10a8_109f + 9)
  ;
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_06,local_7),0xfc2,(char *)s_RandomShrs_10a8_109f + 9)
  ;
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_07,DAT_10a8_22ce),0xfcc,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1b99(&iStack_2,CONCAT11(extraout_AH_08,DAT_10a8_22cf),0xfd9,
                (char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1048_1cea @ 1048:1cea */
/* ======================================================== */

void FUN_1048_1cea(undefined2 param_1,undefined2 *param_2,undefined2 param_3,undefined2 param_4)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  
  uVar1 = GETPRIVATEPROFILEINT
                    (unaff_CS,0xff0,(char *)s_RandomShrs_10a8_109f + 9,*param_2,param_3,param_4,
                     0xfe6);
  *param_2 = uVar1;
  ((undefined2 *)param_2)[1] = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1048_1d18 @ 1048:1d18 */
/* ======================================================== */

undefined4 FUN_1048_1d18(int param_1,char param_2)

{
  undefined2 unaff_SS;
  
  *(undefined1 *)(param_1 + -0x18) = 0;
  if (param_2 == '\0') {
    FUN_1098_0055((char *)s_FALSE_10a8_1003,(char *)s_RandomShrs_10a8_109f + 9,param_1 + -0x18,
                  unaff_SS);
  }
  else {
    FUN_1098_0055(0xffe,(char *)s_RandomShrs_10a8_109f + 9,param_1 + -0x18,unaff_SS);
  }
  return CONCAT22(unaff_SS,param_1 + -0x18);
}

/* ======================================================== */
/* Function: FUN_1048_1d68 @ 1048:1d68 */
/* ======================================================== */

void FUN_1048_1d68(undefined2 param_1,undefined1 *param_2,undefined2 param_3,undefined2 param_4)

{
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined4 uVar1;
  char *pcVar2;
  char local_e [12];
  
  pcVar2 = (char *)s_Oil_Baron_10a8_100a;
  uVar1 = FUN_1048_1d18(param_1,*param_2);
  GETPRIVATEPROFILESTRING
            (unaff_CS,(char *)s_OILBARON_INI_10a8_1014,(char *)s_RandomShrs_10a8_109f + 9,0xb,
             local_e,unaff_SS,uVar1,param_3,param_4,pcVar2);
  if (local_e[0] == 'T') {
    *param_2 = 1;
  }
  else {
    *param_2 = 0;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1050_0002 @ 1050:0002 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1050_0002(undefined1 param_1,undefined1 *param_2,undefined2 param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int unaff_BP;
  undefined1 *puVar4;
  undefined1 *puVar5;
  undefined2 unaff_SS;
  int local_1a0 [19];
  undefined2 local_17a;
  undefined2 local_178;
  undefined1 local_176 [182];
  undefined1 local_c0;
  char local_bf;
  undefined1 local_be [182];
  undefined2 local_8;
  undefined2 local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar5 = local_be;
  puVar4 = (undefined1 *)param_2;
  for (iVar3 = 0xb6; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  local_bf = '\0';
  local_17a = param_3;
  local_178 = param_4;
  FUN_10a0_0b92(0xb6,local_176,unaff_SS,local_be,unaff_SS);
  local_c0 = param_1;
  FUN_1080_0002(local_1a0,unaff_SS,0x10f0,(char *)s_DISCARDPIECE_10a8_1144,
                (char *)s_RandomShrs_10a8_109f + 9,param_5,param_6);
  (*(code *)*(undefined2 *)(local_1a0[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_1a0);
  (*(code *)*(undefined2 *)(local_1a0[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_1a0);
  if (local_bf == '\0') {
    local_8 = local_17a;
    local_6 = local_178;
  }
  else {
    local_8 = 0;
    local_6 = 0;
  }
  return CONCAT22(local_6,local_8);
}

/* ======================================================== */
/* Function: FUN_1050_00af @ 1050:00af */
/* ======================================================== */

void __stdcall16far FUN_1050_00af(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  FUN_1080_03ca((int)param_1,uVar1,(int)param_2,(int)((ulong)param_2 >> 0x10));
  *(undefined1 *)((int)param_1 + 0xe1) = 1;
  return;
}

/* ======================================================== */
/* Function: FUN_1050_00d3 @ 1050:00d3 */
/* ======================================================== */

void __stdcall16far FUN_1050_00d3(undefined4 param_1)

{
  char cVar1;
  int iVar2;
  int unaff_BP;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  bool bVar5;
  undefined2 uVar6;
  undefined1 local_14;
  char local_13;
  char local_12;
  undefined1 local_11;
  uint local_10;
  int local_e;
  uint local_c;
  int local_a;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  FUN_1078_0d9b(iVar3,uVar4);
  if (*(char *)(iVar3 + 0xe0) == '\0') {
    uVar6 = *(undefined2 *)(iVar3 + 4);
    SETDLGITEMTEXT((char *)s_Blanks_10a8_1072 + 6,(char *)s_Please_Choose_Chit_To_Play_10a8_1152,
                   (char *)s_RandomShrs_10a8_109f + 9,0x87);
    SETWINDOWTEXT((char *)s__Show_Graph_10a8_1226 + 10,(char *)s_Move_Dialog_10a8_116d,
                  (char *)s_RandomShrs_10a8_109f + 9,*(undefined2 *)(iVar3 + 4),uVar6);
  }
  local_10 = 0x66;
  local_e = 0;
  local_c = 0x66;
  local_a = 0;
  while( true ) {
    iVar2 = *(int *)(iVar3 + 0x28) + (uint)(0xff99 < *(uint *)(iVar3 + 0x26));
    if ((((local_a < iVar2) || ((local_a <= iVar2 && (local_c < *(uint *)(iVar3 + 0x26) + 0x66))))
        && ((*(char *)(iVar3 + 0xe0) != '\0' ||
            (*(char *)(iVar3 + (local_c - 0x65) * 0xd + 0x22) != '\0')))) &&
       ((*(char *)(iVar3 + (local_c - 0x65) * 0xd + 0x22) != '\0' ||
        (*(char *)(iVar3 + (local_c - 0x65) * 0xd + 0x23) != '\0')))) {
      local_12 = '\0';
      local_14 = *(undefined1 *)(iVar3 + (local_c - 0x65) * 0xd + 0x1d);
      cVar1 = (char)(*(int *)(iVar3 + (local_c - 0x65) * 0xd + 0x1e) % 10) + '0';
      local_13 = cVar1;
      if (9 < *(int *)(iVar3 + (local_c - 0x65) * 0xd + 0x1e)) {
        local_11 = 0;
        local_13 = (char)(*(int *)(iVar3 + (local_c - 0x65) * 0xd + 0x1e) / 10) + '0';
        local_12 = cVar1;
      }
      if (*(int *)(iVar3 + (local_c - 0x65) * 0xd + 0x1e) == -1) {
        local_14 = 0;
        FUN_1098_00bd(0x1179,(char *)s_RandomShrs_10a8_109f + 9,&local_14,unaff_SS);
      }
      FUN_1080_0345(iVar3,uVar4,local_10);
      SETWINDOWTEXT((char *)s_PayDividends_10a8_1079 + 7,&local_14);
      bVar5 = 0xfffe < local_10;
      local_10 = local_10 + 1;
      local_e = local_e + (uint)bVar5;
    }
    if ((local_a == 0) && (local_c == 0x73)) break;
    bVar5 = 0xfffe < local_c;
    local_c = local_c + 1;
    local_a = local_a + (uint)bVar5;
  }
  if ((local_e < 1) && ((local_e < 0 || (local_10 < 0x74)))) {
    local_c = local_10;
    local_a = local_e;
    while( true ) {
      FUN_1080_0345(iVar3,uVar4,local_c);
      SHOWWINDOW((char *)s_PayDividends_10a8_1079 + 7,0);
      if ((local_a == 0) && (local_c == 0x73)) break;
      bVar5 = 0xfffe < local_c;
      local_c = local_c + 1;
      local_a = local_a + (uint)bVar5;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1050_0323 @ 1050:0323 */
/* ======================================================== */

char * __stdcall16far FUN_1050_0323(void)

{
  return s_TDiscardDialog_10a8_117e;
}

/* ======================================================== */
/* Function: FUN_1050_0343 @ 1050:0343 */
/* ======================================================== */

void __stdcall16far FUN_1050_0343(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  bool bVar6;
  uint local_10;
  int local_e;
  uint local_c;
  int local_a;
  uint local_8;
  int local_6;
  
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = (int)param_2;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  FUN_1078_08f0(iVar3,uVar5,iVar2,uVar4);
  if ((*(int *)(iVar2 + 8) == 0) && (2 < *(uint *)(iVar2 + 4))) {
    local_8 = 0x66;
    local_6 = 0;
    while( true ) {
      iVar1 = FUN_1080_0345(iVar3,uVar5,local_8);
      if (iVar1 == *(int *)(iVar2 + 6)) {
        *(int *)(iVar3 + 0x26) = local_8 - 0x65;
        *(int *)(iVar3 + 0x28) =
             (local_6 - (uint)(local_8 < 0x66)) + (uint)(0xfffe < local_8 - 0x66);
        FUN_10a0_1e88((char *)s_PayDividends_10a8_1079 + 7,iVar3,uVar5,iVar2,uVar4);
      }
      if ((local_6 == 0) && (local_8 == 0x73)) break;
      bVar6 = 0xfffe < local_8;
      local_8 = local_8 + 1;
      local_6 = local_6 + (uint)bVar6;
    }
    if (*(char *)(iVar3 + 0xe0) == '\0') {
      local_c = 0;
      local_a = 0;
      local_10 = 0;
      local_e = 0;
      do {
        bVar6 = 0xfffe < local_c;
        local_c = local_c + 1;
        local_a = local_a + (uint)bVar6;
        if (*(char *)(iVar3 + local_c * 0xd + 0x22) != '\0') {
          bVar6 = 0xfffe < local_10;
          local_10 = local_10 + 1;
          local_e = local_e + (uint)bVar6;
        }
      } while ((local_e != *(int *)(iVar3 + 0x28)) || (local_10 != *(uint *)(iVar3 + 0x26)));
      *(uint *)(iVar3 + 0x26) = local_c;
      *(int *)(iVar3 + 0x28) = local_a;
    }
    else {
      local_c = 0;
      local_a = 0;
      local_10 = 0;
      local_e = 0;
      do {
        bVar6 = 0xfffe < local_c;
        local_c = local_c + 1;
        local_a = local_a + (uint)bVar6;
        if ((*(char *)(iVar3 + local_c * 0xd + 0x22) != '\0') ||
           (*(char *)(iVar3 + local_c * 0xd + 0x23) != '\0')) {
          bVar6 = 0xfffe < local_10;
          local_10 = local_10 + 1;
          local_e = local_e + (uint)bVar6;
        }
      } while ((local_e != *(int *)(iVar3 + 0x28)) || (local_10 != *(uint *)(iVar3 + 0x26)));
      *(uint *)(iVar3 + 0x26) = local_c;
      *(int *)(iVar3 + 0x28) = local_a;
    }
  }
  return;
}

