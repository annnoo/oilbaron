/*
 * ai.c - Oil Baron Computer Player Artificial Intelligence Subsystem
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - AI Turn Controller (FUN_1008_ba6a):
 *       Executes when current player is flagged as Computer (DAT_10a8_39a4).
 *       Configurable thinking delay (s_SlowComputer).
 *   - Move Evaluation & Scoring (FUN_1008_a397):
 *       Evaluates each tile in the AI hand against candidate grid positions:
 *       * Forming a new company: evaluates available high-tier companies.
 *       * Expanding existing company: weighs immediate share value gain.
 *       * Triggering mergers: calculates majority/minority bonus payouts
 *         and net equity change before initiating a takeover.
 *       * Defending majority holdings.
 *   - Stock Purchase Decision (FUN_1008_bbb8):
 *       Evaluates cash reserves, target company growth rates, and majority
 *       contest status to purchase up to 3 shares per turn.
 */

/* Function Index:
 *   FUN_1008_9f94        @ 1008:9f94
 *   FUN_1008_a009        @ 1008:a009
 *   FUN_1008_a13c        @ 1008:a13c
 *   FUN_1008_a397        @ 1008:a397
 *   FUN_1008_ba6a        @ 1008:ba6a
 *   FUN_1008_bbb8        @ 1008:bbb8
 *   FUN_1008_bd1a        @ 1008:bd1a
 *   FUN_1048_1db2        @ 1048:1db2
 */

/* ======================================================== */
/* Function: FUN_1008_9f94 @ 1008:9f94 */
/* ======================================================== */

undefined1 FUN_1008_9f94(undefined2 param_1,char *param_2,char *param_3)

{
  char *pcVar1;
  char *pcVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  undefined2 unaff_SS;
  char local_8;
  int local_7;
  undefined1 local_3;
  
  pcVar7 = &local_8;
  pcVar6 = (char *)param_3;
  for (iVar5 = 5; iVar5 != 0; iVar5 = iVar5 + -1) {
    pcVar2 = pcVar7;
    pcVar7 = pcVar7 + 1;
    pcVar1 = pcVar6;
    pcVar6 = pcVar6 + 1;
    *pcVar2 = *pcVar1;
  }
  uVar4 = (int)local_8 - 0x41;
  cVar3 = FUN_1008_1abd(0,local_7,((int)(local_7 - 1U) >> 0xf) + (uint)(0xfffe < local_7 - 1U),
                        local_8 + -0x40,((int)uVar4 >> 0xf) + (uint)(0xfffe < uVar4));
  *param_2 = cVar3;
  if ((*param_2 == '\t') || (*param_2 == '\0')) {
    local_3 = 0;
  }
  else {
    local_3 = 1;
  }
  return local_3;
}

/* ======================================================== */
/* Function: FUN_1008_a009 @ 1008:a009 */
/* ======================================================== */

undefined2 FUN_1008_a009(undefined2 param_1,char param_2)

{
  bool bVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  char cVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  int iVar9;
  undefined2 unaff_SS;
  bool bVar10;
  undefined1 uVar11;
  uint local_a;
  int local_8;
  undefined1 local_3;
  
  iVar4 = DAT_10a8_22b0;
  uVar3 = DAT_10a8_22ae;
  bVar1 = false;
  bVar2 = false;
  local_a = 0;
  if ((-1 < DAT_10a8_22b0) && ((0 < DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_a = 1;
    local_8 = 0;
    while( true ) {
      iVar9 = DAT_10a8_319a * 0xfc + local_a * 0xd;
      if (*(char *)(iVar9 + 0x30c8) != '\0') {
        cVar5 = FUN_1008_9f94(param_1,&param_2,unaff_SS,iVar9 + 0x30c3,
                              (char *)s_RandomShrs_10a8_109f + 9);
        if (cVar5 == '\0') {
          if ((param_2 != '\t') && (param_2 != '\0')) {
            uVar8 = 0;
            uVar7 = DAT_10a8_22c0;
            FUN_10a0_0baa();
            FUN_10a0_1458();
            uVar11 = 0;
            uVar6 = FUN_10a0_144a();
            FUN_1048_0fac(param_2);
            FUN_10a0_1458(uVar6,uVar8,uVar7);
            FUN_10a0_1454();
            if ((bool)uVar11) {
              FUN_10a0_1444();
              bVar10 = (long)(int)((long)DAT_10a8_319a * 0xfc) != (long)DAT_10a8_319a * 0xfc;
              FUN_10a0_1454();
              if (bVar10) {
                bVar2 = true;
              }
            }
          }
        }
        else {
          bVar1 = true;
        }
      }
      if ((local_8 == iVar4) && (local_a == uVar3)) break;
      bVar10 = 0xfffe < local_a;
      local_a = local_a + 1;
      local_8 = local_8 + (uint)bVar10;
    }
  }
  if ((!bVar1) || (bVar2)) {
    local_3 = 0;
  }
  else {
    local_3 = 1;
  }
  return CONCAT11((char)(local_a >> 8),local_3);
}

/* ======================================================== */
/* Function: FUN_1008_a13c @ 1008:a13c */
/* ======================================================== */

undefined4 FUN_1008_a13c(undefined2 param_1,char param_2)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  undefined2 local_e;
  undefined2 local_c;
  undefined2 local_a;
  undefined2 local_8;
  
  local_a = 0;
  local_8 = 0;
  if ((DAT_10a8_22ac < 0x8000) && ((0 < (int)DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
    local_e = 1;
    local_c = 0;
    while( true ) {
      iVar1 = local_e * 0xfc + param_2 * 4;
      iVar2 = *(int *)(iVar1 + 0x3184);
      if ((local_8 < iVar2) || ((local_8 <= iVar2 && (local_a < *(uint *)(iVar1 + 0x3182))))) {
        iVar2 = local_e * 0xfc + param_2 * 4;
        local_a = *(uint *)(iVar2 + 0x3182);
        local_8 = *(int *)(iVar2 + 0x3184);
      }
      if ((local_c == DAT_10a8_22ac) && (local_e == DAT_10a8_22aa)) break;
      bVar3 = 0xfffe < local_e;
      local_e = local_e + 1;
      local_c = local_c + bVar3;
    }
  }
  return CONCAT22(local_8,local_a);
}

/* ======================================================== */
/* Function: FUN_1008_a397 @ 1008:a397 */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x1008a763) */
/* WARNING: Removing unreachable block (ram,0x1008adf3) */
/* WARNING: Removing unreachable block (ram,0x1008ae26) */
/* WARNING: Type propagation algorithm not settling */

void FUN_1008_a397(undefined2 param_1)

{
  char cVar1;
  int iVar2;
  undefined2 uVar3;
  undefined1 extraout_AH;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  char *pcVar11;
  uint uVar12;
  undefined2 extraout_DX;
  undefined2 uVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  undefined2 unaff_SS;
  bool bVar18;
  undefined1 uVar19;
  bool bVar20;
  undefined1 uVar21;
  long lVar22;
  undefined4 uVar23;
  undefined4 local_20a;
  uint local_1fe [18];
  undefined2 local_1da;
  uint local_1d8;
  uint local_1d6;
  char local_1d3;
  uint local_1d2;
  uint local_1d0;
  undefined4 local_1ce;
  uint local_1ca;
  int local_1c8;
  uint local_1c6;
  uint local_1c4;
  uint local_1c2;
  uint local_1c0;
  uint local_100 [23];
  char local_d1;
  undefined2 local_d0 [32];
  undefined2 local_90 [69];
  char local_6;
  char local_5;
  char local_4;
  char local_3;
  
  iVar14 = DAT_10a8_319a * 0xfc;
  iVar15 = iVar14 + 0x30aa;
  local_3 = '\b';
  while( true ) {
    iVar2 = (int)local_3;
    local_100[iVar2 * 3] = 0x7f;
    local_100[iVar2 * 3 + 1] = 0;
    local_100[iVar2 * 3 + 2] = 0;
    bVar18 = false;
    bVar20 = true;
    FUN_10a0_1454();
    if (!bVar18 && !bVar20) {
      FUN_1008_826e(param_1,1,&local_4,unaff_SS,&local_3,unaff_SS);
      bVar18 = false;
      uVar19 = 1;
      FUN_10a0_1454();
      if (bVar18) {
        iVar2 = local_4 * 0x24;
        uVar8 = *(undefined2 *)(iVar2 + 0x378a);
        uVar10 = *(undefined2 *)(iVar2 + 0x378c);
        uVar9 = ((undefined2 *)&DAT_10a8_378e)[local_4 * 0x12];
        FUN_1008_80b8(param_1,CONCAT11((char)((uint)uVar8 >> 8),local_3));
        FUN_10a0_1458(uVar8,uVar10,uVar9);
        FUN_10a0_1444();
        uVar8 = *(undefined2 *)(local_4 * 0x24 + 0x3774);
        uVar10 = *(undefined2 *)(local_4 * 0x24 + 0x3776);
        uVar9 = FUN_10a0_1432();
        uVar19 = 0;
        FUN_10a0_1444(uVar9,uVar8,uVar10);
        FUN_10a0_1454();
        if ((bool)uVar19) {
          uVar6 = local_100[local_3 * 3 + 1];
          uVar7 = local_100[local_3 * 3 + 2];
          uVar4 = FUN_10a0_1432();
          iVar2 = (int)local_3;
          uVar19 = iVar2 * 6 == 0;
          local_100[iVar2 * 3] = uVar4;
          local_100[iVar2 * 3 + 1] = uVar6;
          local_100[iVar2 * 3 + 2] = uVar7;
        }
        else {
          uVar6 = local_100[local_3 * 3 + 1];
          uVar7 = local_100[local_3 * 3 + 2];
          uVar4 = FUN_10a0_1432();
          iVar2 = (int)local_3;
          uVar19 = iVar2 * 6 == 0;
          local_100[iVar2 * 3] = uVar4;
          local_100[iVar2 * 3 + 1] = uVar6;
          local_100[iVar2 * 3 + 2] = uVar7;
        }
      }
      uVar13 = 0x9999;
      pcVar11 = (char *)s_Weem_Oil_Refining_10a8_198f + 10;
      uVar3 = FUN_10a0_1444();
      iVar2 = (int)((long)(int)local_3 * 0x24);
      uVar21 = (long)iVar2 != (long)(int)local_3 * 0x24;
      uVar8 = *(undefined2 *)(iVar2 + 0x3772);
      uVar10 = *(undefined2 *)(iVar2 + 0x3774);
      uVar9 = *(undefined2 *)(iVar2 + 0x3776);
      FUN_1008_a13c(param_1,CONCAT11((char)((uint)uVar8 >> 8),local_3));
      FUN_10a0_1458(uVar8,uVar10,uVar9,uVar3,uVar13,pcVar11);
      FUN_10a0_1444();
      FUN_10a0_1454();
      if (!(bool)uVar21 && !(bool)uVar19) {
        uVar6 = local_100[local_3 * 3 + 1];
        uVar7 = local_100[local_3 * 3 + 2];
        uVar4 = FUN_10a0_1438();
        iVar2 = (int)local_3;
        local_100[iVar2 * 3] = uVar4;
        local_100[iVar2 * 3 + 1] = uVar6;
        local_100[iVar2 * 3 + 2] = uVar7;
      }
      iVar2 = local_3 * 0x24;
      uVar8 = *(undefined2 *)(iVar14 + 0x30cc);
      uVar10 = *(undefined2 *)(iVar14 + 0x30ce);
      FUN_10a0_144a(*(undefined2 *)(iVar2 + 0x3772),*(undefined2 *)(iVar2 + 0x3774),
                    *(undefined2 *)(iVar2 + 0x3776));
      uVar9 = FUN_10a0_144a();
      iVar2 = iVar15 + local_3 * 4;
      uVar6 = *(uint *)(iVar2 + 0xda);
      uVar7 = *(uint *)(iVar2 + 0xd8);
      uVar23 = FUN_1008_a13c(param_1,CONCAT11((char)((uint)(local_3 * 4) >> 8),local_3));
      uVar4 = (uint)((ulong)uVar23 >> 0x10);
      uVar19 = uVar4 < uVar6 || uVar4 - uVar6 < (uint)((uint)uVar23 < uVar7);
      FUN_10a0_1458(uVar9,uVar8,uVar10);
      FUN_10a0_1454();
      if ((bool)uVar19) {
        uVar6 = local_100[local_3 * 3 + 1];
        uVar7 = local_100[local_3 * 3 + 2];
        uVar4 = FUN_10a0_1432();
        iVar2 = (int)local_3;
        local_100[iVar2 * 3] = uVar4;
        local_100[iVar2 * 3 + 1] = uVar6;
        local_100[iVar2 * 3 + 2] = uVar7;
      }
      cVar1 = FUN_1008_a1da(&stack0xfffe,local_3);
      if (cVar1 != '\0') {
        uVar6 = local_100[local_3 * 3 + 1];
        uVar7 = local_100[local_3 * 3 + 2];
        uVar4 = FUN_10a0_1432();
        iVar2 = (int)local_3;
        local_100[iVar2 * 3] = uVar4;
        local_100[iVar2 * 3 + 1] = uVar6;
        local_100[iVar2 * 3 + 2] = uVar7;
      }
      iVar2 = FUN_1008_80b8(param_1,local_3);
      if (iVar2 == 2) {
        local_1c2 = 5;
      }
      else if (iVar2 == 3) {
        local_1c2 = 7;
      }
      else if (iVar2 == 4) {
        local_1c2 = 8;
      }
      else if ((iVar2 < 5) || (7 < iVar2)) {
        if ((iVar2 < 7) || (10 < iVar2)) {
          local_1c2 = 0;
        }
        else {
          local_1c2 = 0xf;
        }
      }
      else {
        local_1c2 = 10;
      }
      local_1c0 = 0;
      if (local_1c2 != 0) {
        uVar6 = FUN_1008_813e(param_1,local_3);
        if (((int)uVar6 >> 0xf < (int)local_1c0) ||
           (((int)uVar6 >> 0xf <= (int)local_1c0 && (uVar6 < local_1c2)))) {
          uVar6 = local_100[local_3 * 3 + 1];
          uVar7 = local_100[local_3 * 3 + 2];
          uVar4 = FUN_10a0_1438();
          iVar2 = (int)local_3;
          local_100[iVar2 * 3] = uVar4;
          local_100[iVar2 * 3 + 1] = uVar6;
          local_100[iVar2 * 3 + 2] = uVar7;
        }
        uVar6 = local_1c0;
        FUN_10a0_0baa();
        uVar7 = FUN_10a0_0be7();
        uVar4 = FUN_1008_813e(param_1,local_3);
        if (((int)uVar4 >> 0xf < (int)uVar6) ||
           (((int)uVar4 >> 0xf <= (int)uVar6 && (uVar4 < uVar7)))) {
          uVar6 = local_100[local_3 * 3 + 1];
          uVar7 = local_100[local_3 * 3 + 2];
          uVar4 = FUN_10a0_1438();
          iVar2 = (int)local_3;
          local_100[iVar2 * 3] = uVar4;
          local_100[iVar2 * 3 + 1] = uVar6;
          local_100[iVar2 * 3 + 2] = uVar7;
        }
      }
    }
    uVar7 = DAT_10a8_22b0;
    uVar6 = DAT_10a8_22ae;
    if (local_3 == '\x01') break;
    local_3 = local_3 + -1;
  }
  if ((DAT_10a8_22b0 < 0x8000) && ((0 < (int)DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_1c2 = 1;
    local_1c0 = 0;
    while( true ) {
      local_90[local_1c2 * 3] = 0x81;
      local_90[local_1c2 * 3 + 1] = 0;
      local_90[local_1c2 * 3 + 2] = 0;
      if ((local_1c0 == DAT_10a8_22b0) && (local_1c2 == DAT_10a8_22ae)) break;
      bVar18 = 0xfffe < local_1c2;
      local_1c2 = local_1c2 + 1;
      local_1c0 = local_1c0 + bVar18;
    }
  }
  local_1fe[0] = 0;
  local_1fe[1] = 0;
  local_6 = '\0';
  if ((-1 < (int)DAT_10a8_22b0) && ((0 < (int)DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_1c2 = 1;
    local_1c0 = 0;
    while( true ) {
      if ((*(char *)(iVar15 + local_1c2 * 0xd + 0x1e) != '\0') &&
         (cVar1 = FUN_1008_9c3e(param_1,&local_3,unaff_SS,iVar15 + local_1c2 * 0xd + 0x19,
                                (char *)s_RandomShrs_10a8_109f + 9), cVar1 != '\0')) {
        FUN_1008_826e(param_1,1,&local_6,unaff_SS,&local_3,unaff_SS);
        uVar4 = local_100[local_3 * 3 + 1];
        uVar12 = local_100[local_3 * 3 + 2];
        uVar5 = FUN_10a0_1432();
        iVar2 = (int)local_3;
        local_100[iVar2 * 3] = uVar5;
        local_100[iVar2 * 3 + 1] = uVar4;
        local_100[iVar2 * 3 + 2] = uVar12;
        uVar4 = local_100[local_6 * 3 + 1];
        uVar12 = local_100[local_6 * 3 + 2];
        uVar5 = FUN_10a0_1432();
        iVar2 = (int)local_6;
        local_100[iVar2 * 3] = uVar5;
        local_100[iVar2 * 3 + 1] = uVar4;
        local_100[iVar2 * 3 + 2] = uVar12;
        FUN_1008_88d9(param_1,local_6,local_3,DAT_10a8_319a,DAT_10a8_319c);
        uVar19 = false;
        uVar21 = true;
        FUN_10a0_1454();
        if (!(bool)uVar19 && !(bool)uVar21) {
          uVar4 = local_100[local_3 * 3 + 1];
          uVar12 = local_100[local_3 * 3 + 2];
          uVar5 = FUN_10a0_1432();
          iVar2 = (int)local_3;
          uVar19 = CARRY2(iVar2 * 4,iVar2 * 2);
          uVar21 = iVar2 * 6 == 0;
          local_100[iVar2 * 3] = uVar5;
          local_100[iVar2 * 3 + 1] = uVar4;
          local_100[iVar2 * 3 + 2] = uVar12;
        }
        FUN_1008_88d9(param_1,local_6,local_3,DAT_10a8_319a,DAT_10a8_319c);
        FUN_10a0_1454();
        if (!(bool)uVar19 && !(bool)uVar21) {
          FUN_1008_88d9(param_1,CONCAT11(extraout_AH,local_6),CONCAT11(extraout_AH,local_3),
                        DAT_10a8_319a,DAT_10a8_319c);
          local_1fe[0] = uVar4;
          local_1fe[1] = extraout_DX;
        }
      }
      if ((local_1c0 == uVar7) && (local_1c2 == uVar6)) break;
      bVar18 = 0xfffe < local_1c2;
      local_1c2 = local_1c2 + 1;
      local_1c0 = local_1c0 + bVar18;
    }
  }
  bVar18 = false;
  bVar20 = true;
  FUN_10a0_1454();
  if (!bVar18 && !bVar20) {
    uVar6 = local_100[local_3 * 3 + 1];
    uVar7 = local_100[local_3 * 3 + 2];
    uVar4 = FUN_10a0_1432();
    iVar2 = (int)local_3;
    local_100[iVar2 * 3] = uVar4;
    local_100[iVar2 * 3 + 1] = uVar6;
    local_100[iVar2 * 3 + 2] = uVar7;
  }
  uVar7 = DAT_10a8_22b0;
  uVar6 = DAT_10a8_22ae;
  if ((-1 < (int)DAT_10a8_22b0) && ((0 < (int)DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_1c2 = 1;
    local_1c0 = 0;
    while( true ) {
      if ((*(char *)(iVar15 + local_1c2 * 0xd + 0x1e) != '\0') &&
         (cVar1 = FUN_1008_9f94(param_1,&local_3,unaff_SS,iVar15 + local_1c2 * 0xd + 0x19,
                                (char *)s_RandomShrs_10a8_109f + 9), cVar1 != '\0')) {
        FUN_10a0_1444();
        uVar4 = local_100[local_3 * 3 + 1];
        uVar12 = local_100[local_3 * 3 + 2];
        uVar5 = FUN_10a0_1432();
        iVar2 = (int)local_3;
        local_100[iVar2 * 3] = uVar5;
        local_100[iVar2 * 3 + 1] = uVar4;
        local_100[iVar2 * 3 + 2] = uVar12;
        local_90[local_1c2 * 3] = 0x80;
        local_90[local_1c2 * 3 + 1] = 0;
        local_90[local_1c2 * 3 + 2] = 0;
      }
      if ((local_1c0 == uVar7) && (local_1c2 == uVar6)) break;
      bVar18 = 0xfffe < local_1c2;
      local_1c2 = local_1c2 + 1;
      local_1c0 = local_1c0 + bVar18;
    }
  }
  local_1ce._0_2_ = 1;
  local_1ce._2_2_ = 0;
  while( true ) {
    iVar2 = (uint)local_1ce * 7;
    (&local_d1)[iVar2] = '\x01';
    *(undefined2 *)((int)local_d0 + iVar2) = 0x8a;
    *(undefined2 *)((int)local_d0 + iVar2 + 2U) = 0;
    *(undefined2 *)((int)local_d0 + iVar2 + 4U) = 0x7a00;
    if ((local_1ce._2_2_ == 0) && ((uint)local_1ce == 8)) break;
    bVar18 = 0xfffe < (uint)local_1ce;
    local_1ce._0_2_ = (uint)local_1ce + 1;
    local_1ce._2_2_ = local_1ce._2_2_ + (uint)bVar18;
  }
  local_3 = '\x01';
  while( true ) {
    bVar18 = false;
    bVar20 = true;
    FUN_10a0_1454();
    if (!bVar18 && !bVar20) {
      local_1ce._0_2_ = 1;
      local_1ce._2_2_ = 0;
      do {
        local_1d3 = '\0';
        iVar2 = (uint)local_1ce * 7;
        local_20a = (char *)CONCAT22(unaff_SS,&local_d1 + iVar2);
        bVar18 = (long)(int)((long)(int)local_3 * 0x24) != (long)(int)local_3 * 0x24;
        FUN_10a0_1454();
        if (bVar18) {
          uVar6 = (uint)local_1ce + 1;
          uVar7 = local_1ce._2_2_ + (uint)(0xfffe < (uint)local_1ce);
          if (((int)uVar7 < 1) && ((0x7fff < uVar7 || (uVar6 < 9)))) {
            local_1d2 = 8;
            local_1d0 = 0;
            while ((FUN_10a0_0b92(7,&local_d1 + local_1d2 * 7,unaff_SS,
                                  &local_d1 + (local_1d2 - 1) * 7,unaff_SS), local_1d0 != uVar7 ||
                   (local_1d2 != uVar6))) {
              bVar18 = local_1d2 == 0;
              local_1d2 = local_1d2 - 1;
              local_1d0 = local_1d0 - bVar18;
            }
          }
          iVar16 = local_3 * 0x24;
          uVar8 = *(undefined2 *)(iVar16 + 0x3774);
          uVar10 = *(undefined2 *)(iVar16 + 0x3776);
          *(undefined2 *)((int)local_d0 + iVar2) = *(undefined2 *)(iVar16 + 0x3772);
          *(undefined2 *)((int)local_d0 + iVar2 + 2U) = uVar8;
          *(undefined2 *)((int)local_d0 + iVar2 + 4U) = uVar10;
          *local_20a = local_3;
          local_1d3 = '\x01';
        }
        bVar18 = 0xfffe < (uint)local_1ce;
        local_1ce._0_2_ = (uint)local_1ce + 1;
        local_1ce._2_2_ = local_1ce._2_2_ + (uint)bVar18;
      } while (local_1d3 == '\0');
    }
    if (local_3 == '\b') break;
    local_3 = local_3 + '\x01';
  }
  lVar22 = FUN_1048_1035(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
  if (3 < lVar22) {
    lVar22 = 3;
  }
  local_1ce = lVar22;
  if (0 < lVar22) {
    local_1d2 = 1;
    local_1d0 = 0;
    while( true ) {
      bVar18 = true;
      FUN_10a0_1454();
      if (!bVar18) {
        FUN_10a0_144a();
        uVar6 = local_100[(&local_d1)[local_1d2 * 7] * 3 + 1];
        uVar7 = local_100[(&local_d1)[local_1d2 * 7] * 3 + 2];
        uVar4 = FUN_10a0_1432();
        iVar2 = (int)(&local_d1)[local_1d2 * 7];
        local_100[iVar2 * 3] = uVar4;
        local_100[iVar2 * 3 + 1] = uVar6;
        local_100[iVar2 * 3 + 2] = uVar7;
        uVar6 = local_100[(&local_d1)[local_1d2 * 7] * 3 + 1];
        uVar7 = local_100[(&local_d1)[local_1d2 * 7] * 3 + 2];
        uVar4 = FUN_10a0_1432();
        iVar2 = (int)(&local_d1)[local_1d2 * 7];
        local_100[iVar2 * 3] = uVar4;
        local_100[iVar2 * 3 + 1] = uVar6;
        local_100[iVar2 * 3 + 2] = uVar7;
      }
      cVar1 = FUN_1008_a1da(&stack0xfffe,local_3);
      if (cVar1 != '\0') {
        uVar6 = local_100[local_3 * 3 + 1];
        uVar7 = local_100[local_3 * 3 + 2];
        uVar4 = FUN_10a0_1432();
        iVar2 = (int)local_3;
        local_100[iVar2 * 3] = uVar4;
        local_100[iVar2 * 3 + 1] = uVar6;
        local_100[iVar2 * 3 + 2] = uVar7;
      }
      bVar18 = true;
      FUN_10a0_1454();
      if (bVar18) {
        iVar2 = (int)(&local_d1)[local_1d2 * 7];
        local_100[iVar2 * 3] = 0x9a7e;
        local_100[iVar2 * 3 + 1] = 0x9999;
        local_100[iVar2 * 3 + 2] = (uint)((char *)s_Weem_Oil_Refining_10a8_198f + 10);
      }
      if (lVar22 == CONCAT22(local_1d0,local_1d2)) break;
      bVar18 = 0xfffe < local_1d2;
      local_1d2 = local_1d2 + 1;
      local_1d0 = local_1d0 + bVar18;
    }
  }
  local_3 = '\x01';
  while( true ) {
    cVar1 = FUN_1008_a307(&stack0xfffe,local_3);
    if (cVar1 != '\0') {
      iVar2 = (int)local_3;
      local_100[iVar2 * 3] = 0;
      local_100[iVar2 * 3 + 1] = 0;
      local_100[iVar2 * 3 + 2] = 0;
    }
    if (local_3 == '\b') break;
    local_3 = local_3 + '\x01';
  }
  local_3 = '\x01';
  while( true ) {
    bVar18 = false;
    bVar20 = true;
    FUN_10a0_1454();
    if (!bVar18 && !bVar20) {
      iVar16 = iVar15 + local_3 * 4;
      uVar6 = *(uint *)(iVar16 + 0xda);
      iVar2 = (int)local_3;
      local_1fe[iVar2 * 2] = *(uint *)(iVar16 + 0xd8);
      local_1fe[iVar2 * 2 + 1] = uVar6;
      iVar2 = iVar15 + local_3 * 4;
      uVar6 = *(uint *)(iVar2 + 0xd8);
      iVar2 = *(int *)(iVar2 + 0xda);
      uVar7 = ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2];
      iVar16 = ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2];
      ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2] = uVar7 + uVar6;
      ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2] = iVar16 + iVar2 + (uint)CARRY2(uVar7,uVar6);
      uVar23 = FUN_1048_0fac((int)local_3);
      uVar7 = DAT_10a8_22be - (uint)uVar23;
      iVar2 = iVar15 + local_3 * 4;
      uVar6 = *(uint *)(iVar2 + 0xd8);
      iVar2 = *(int *)(iVar2 + 0xda) +
              ((DAT_10a8_22c0 - (int)((ulong)uVar23 >> 0x10)) - (uint)(DAT_10a8_22be < (uint)uVar23)
              ) + (uint)CARRY2(uVar6,uVar7);
      if (((int)local_1fe[local_3 * 2 + 1] < iVar2) ||
         (((int)local_1fe[local_3 * 2 + 1] <= iVar2 && (local_1fe[local_3 * 2] < uVar6 + uVar7)))) {
        uVar23 = FUN_1048_0fac(CONCAT11((char)(local_1fe[local_3 * 2] >> 8),local_3));
        uVar7 = DAT_10a8_22be - (uint)uVar23;
        iVar2 = iVar15 + local_3 * 4;
        uVar6 = *(uint *)(iVar2 + 0xd8);
        iVar2 = *(int *)(iVar2 + 0xda);
        iVar16 = (int)local_3;
        local_1fe[iVar16 * 2] = uVar6 + uVar7;
        local_1fe[iVar16 * 2 + 1] =
             iVar2 + ((DAT_10a8_22c0 - (int)((ulong)uVar23 >> 0x10)) -
                     (uint)(DAT_10a8_22be < (uint)uVar23)) + (uint)CARRY2(uVar6,uVar7);
      }
      if (DAT_10a8_22cc != '\0') {
        if (((int)((undefined2 *)&DAT_10a8_3adc)[local_3 * 2] < (int)local_1fe[local_3 * 2 + 1]) ||
           (((int)((undefined2 *)&DAT_10a8_3adc)[local_3 * 2] <= (int)local_1fe[local_3 * 2 + 1] &&
            ((uint)((undefined2 *)&DAT_10a8_3ada)[local_3 * 2] < local_1fe[local_3 * 2])))) {
          uVar6 = ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2];
          iVar2 = (int)local_3;
          local_1fe[iVar2 * 2] = ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2];
          local_1fe[iVar2 * 2 + 1] = uVar6;
        }
        if ((int)local_1fe[local_3 * 2 + 1] < 0) {
          iVar2 = (int)local_3;
          local_1fe[iVar2 * 2] = 0;
          local_1fe[iVar2 * 2 + 1] = 0;
        }
      }
      iVar2 = local_3 * 0x24;
      uVar8 = *(undefined2 *)(iVar2 + 0x3774);
      uVar10 = *(undefined2 *)(iVar15 + local_3 * 4 + 0xda);
      FUN_10a0_1458(*(undefined2 *)(iVar2 + 0x3772),uVar8,*(undefined2 *)(iVar2 + 0x3776));
      FUN_10a0_1444();
      uVar9 = FUN_10a0_1432();
      *(undefined2 *)(iVar14 + 0x30ca) = uVar9;
      *(undefined2 *)(iVar14 + 0x30cc) = uVar8;
      *(undefined2 *)(iVar14 + 0x30ce) = uVar10;
      iVar2 = iVar15 + local_3 * 4;
      *(undefined2 *)(iVar2 + 0xd8) = 0;
      *(undefined2 *)(iVar2 + 0xda) = 0;
    }
    if (local_3 == '\b') break;
    local_3 = local_3 + '\x01';
  }
  local_1da = 0;
  local_1d8 = 0;
  local_1d6 = 0;
  if (*(char *)((undefined2 *)&DAT_10a8_38bc + DAT_10a8_319a * 0x10) == '\x02') {
    local_5 = '\x01';
    while( true ) {
      bVar18 = false;
      bVar20 = true;
      FUN_10a0_1454();
      if (!bVar18 && !bVar20) {
        FUN_10a0_18f1(100);
        FUN_10a0_1458();
        FUN_10a0_144a();
        FUN_10a0_1432();
        uVar6 = local_100[local_5 * 3 + 1];
        uVar7 = local_100[local_5 * 3 + 2];
        uVar4 = FUN_10a0_1444();
        iVar2 = (int)local_5;
        local_100[iVar2 * 3] = uVar4;
        local_100[iVar2 * 3 + 1] = uVar6;
        local_100[iVar2 * 3 + 2] = uVar7;
      }
      if (local_5 == '\b') break;
      local_5 = local_5 + '\x01';
    }
  }
  local_3 = '\x01';
  while( true ) {
    bVar18 = false;
    FUN_10a0_1454();
    if (bVar18) {
      iVar2 = (int)local_3;
      local_100[iVar2 * 3] = 0;
      local_100[iVar2 * 3 + 1] = 0;
      local_100[iVar2 * 3 + 2] = 0;
    }
    if (local_3 == '\b') break;
    local_3 = local_3 + '\x01';
  }
  if (*(char *)((undefined2 *)&DAT_10a8_38bc + DAT_10a8_319a * 0x10) == '\x03') {
    local_3 = '\x01';
    while( true ) {
      local_1ca = 1;
      local_1c8 = 0;
      while( true ) {
        uVar6 = local_100[local_3 * 3 + 1];
        uVar7 = local_100[local_3 * 3 + 2];
        uVar4 = FUN_10a0_16b1();
        iVar2 = (int)local_3;
        local_100[iVar2 * 3] = uVar4;
        local_100[iVar2 * 3 + 1] = uVar6;
        local_100[iVar2 * 3 + 2] = uVar7;
        if ((local_1c8 == 0) && (local_1ca == 2)) break;
        bVar18 = 0xfffe < local_1ca;
        local_1ca = local_1ca + 1;
        local_1c8 = local_1c8 + (uint)bVar18;
      }
      if (local_3 == '\b') break;
      local_3 = local_3 + '\x01';
    }
  }
  local_3 = '\x01';
  while( true ) {
    uVar6 = local_100[local_3 * 3 + 1];
    uVar7 = local_100[local_3 * 3 + 2];
    local_1da = FUN_10a0_1432();
    local_1d8 = uVar6;
    local_1d6 = uVar7;
    if (local_3 == '\b') break;
    local_3 = local_3 + '\x01';
  }
  bVar18 = true;
  FUN_10a0_1454();
  if (bVar18) {
    local_1da = 0x81;
    local_1d8 = 0;
    local_1d6 = 0;
  }
  local_3 = '\x01';
  while( true ) {
    uVar6 = local_100[local_3 * 3 + 1];
    uVar7 = local_100[local_3 * 3 + 2];
    uVar4 = FUN_10a0_144a();
    iVar2 = (int)local_3;
    local_100[iVar2 * 3] = uVar4;
    local_100[iVar2 * 3 + 1] = uVar6;
    local_100[iVar2 * 3 + 2] = uVar7;
    if (local_3 == '\b') break;
    local_3 = local_3 + '\x01';
  }
  local_1ca = 0;
  local_1c8 = 0;
  do {
    local_3 = '\b';
    while( true ) {
      iVar2 = DAT_10a8_319a * 0xfc;
      iVar16 = iVar2 + 0x30aa;
      bVar18 = false;
      bVar20 = true;
      FUN_10a0_1454();
      if (!bVar18 && !bVar20) {
        iVar17 = local_3 * 0x24;
        uVar7 = local_100[local_3 * 3 + 1];
        uVar6 = local_100[local_3 * 3 + 2];
        FUN_10a0_1444(*(undefined2 *)(iVar17 + 0x3772),*(undefined2 *)(iVar17 + 0x3774),
                      *(undefined2 *)(iVar17 + 0x3776));
        FUN_10a0_144a();
        local_1c6 = FUN_10a0_145c();
        local_1c4 = uVar6;
        cVar1 = FUN_1008_a009(param_1,local_3);
        uVar19 = 0;
        if (cVar1 != '\0') {
          uVar6 = DAT_10a8_22c0;
          FUN_10a0_1458();
          uVar8 = FUN_10a0_1444();
          FUN_10a0_1458(uVar8,uVar7,uVar6);
          FUN_10a0_1454();
          if ((bool)uVar19) {
            uVar6 = DAT_10a8_22c0;
            uVar8 = FUN_10a0_1458();
            FUN_10a0_1444(uVar8,uVar7,uVar6);
            FUN_10a0_144a();
            FUN_10a0_1444();
            uVar7 = *(uint *)(iVar2 + 0x30cc);
            FUN_10a0_1438();
            uVar19 = false;
            bVar18 = true;
            FUN_10a0_1454();
            if (!(bool)uVar19 && !bVar18) {
              uVar6 = DAT_10a8_22c0;
              FUN_10a0_1458();
              FUN_10a0_1444();
              local_1c6 = FUN_10a0_145c();
              local_1c4 = uVar6;
            }
          }
          uVar6 = DAT_10a8_22c0;
          FUN_10a0_1458();
          uVar8 = FUN_10a0_1444();
          FUN_10a0_1458(uVar8,uVar7,uVar6);
          FUN_10a0_1454();
          if ((bool)uVar19) {
            uVar6 = DAT_10a8_22c0;
            uVar8 = FUN_10a0_1458();
            FUN_10a0_1444(uVar8,uVar7,uVar6);
            FUN_10a0_144a();
            FUN_10a0_1444();
            FUN_10a0_1438();
            bVar18 = false;
            bVar20 = true;
            FUN_10a0_1454();
            if (!bVar18 && !bVar20) {
              uVar6 = DAT_10a8_22c0;
              FUN_10a0_1458();
              FUN_10a0_1444();
              local_1c6 = FUN_10a0_145c();
              local_1c4 = uVar6;
            }
          }
        }
        iVar17 = iVar16 + local_3 * 4;
        uVar6 = *(uint *)(iVar17 + 0xd8);
        iVar17 = *(int *)(iVar17 + 0xda) + local_1c4 + (uint)CARRY2(uVar6,local_1c6);
        if (((int)local_1fe[local_3 * 2 + 1] < iVar17) ||
           (((int)local_1fe[local_3 * 2 + 1] <= iVar17 &&
            (local_1fe[local_3 * 2] < uVar6 + local_1c6)))) {
          iVar17 = iVar16 + local_3 * 4;
          uVar6 = *(uint *)(iVar17 + 0xd8);
          local_1c6 = local_1fe[local_3 * 2] - uVar6;
          local_1c4 = (local_1fe[local_3 * 2 + 1] - *(int *)(iVar17 + 0xda)) -
                      (uint)(local_1fe[local_3 * 2] < uVar6);
        }
        if ((int)local_1c4 < 0) {
          local_1c6 = 0;
          local_1c4 = 0;
        }
        FUN_10a0_1458();
        FUN_10a0_1444();
        uVar8 = *(undefined2 *)(iVar2 + 0x30cc);
        uVar10 = *(undefined2 *)(iVar2 + 0x30ce);
        uVar9 = FUN_10a0_1438();
        *(undefined2 *)(iVar2 + 0x30ca) = uVar9;
        *(undefined2 *)(iVar2 + 0x30cc) = uVar8;
        *(undefined2 *)(iVar2 + 0x30ce) = uVar10;
        iVar2 = iVar16 + local_3 * 4;
        uVar6 = *(uint *)(iVar2 + 0xd8);
        iVar2 = *(int *)(iVar2 + 0xda);
        iVar16 = iVar16 + local_3 * 4;
        *(int *)(iVar16 + 0xd8) = uVar6 + local_1c6;
        *(int *)(iVar16 + 0xda) = iVar2 + local_1c4 + (uint)CARRY2(uVar6,local_1c6);
        uVar6 = ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2];
        iVar2 = ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2];
        ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2] = uVar6 - local_1c6;
        ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2] =
             (iVar2 - local_1c4) - (uint)(uVar6 < local_1c6);
      }
      if (local_3 == '\x01') break;
      local_3 = local_3 + -1;
    }
    bVar18 = 0xfffe < local_1ca;
    local_1ca = local_1ca + 1;
    local_1c8 = local_1c8 + (uint)bVar18;
    bVar18 = false;
    FUN_10a0_1454();
  } while (((!bVar18) && (local_1c8 < 1)) && ((local_1c8 < 0 || (local_1ca < 3))));
  local_3 = '\x01';
  while( true ) {
    bVar18 = false;
    bVar20 = true;
    FUN_10a0_1454();
    if (!bVar18 && !bVar20) {
      uVar6 = *(uint *)(iVar14 + 0x30ce);
      FUN_10a0_144a();
      local_1c6 = FUN_10a0_145c();
      local_1c4 = uVar6;
      iVar2 = iVar15 + local_3 * 4;
      uVar7 = *(uint *)(iVar2 + 0xd8);
      iVar2 = *(int *)(iVar2 + 0xda) + uVar6 + (uint)CARRY2(uVar7,local_1c6);
      if (((int)local_1fe[local_3 * 2 + 1] < iVar2) ||
         (((int)local_1fe[local_3 * 2 + 1] <= iVar2 && (local_1fe[local_3 * 2] < uVar7 + local_1c6))
         )) {
        iVar2 = iVar15 + local_3 * 4;
        uVar6 = *(uint *)(iVar2 + 0xd8);
        local_1c6 = local_1fe[local_3 * 2] - uVar6;
        local_1c4 = (local_1fe[local_3 * 2 + 1] - *(int *)(iVar2 + 0xda)) -
                    (uint)(local_1fe[local_3 * 2] < uVar6);
      }
      if ((int)local_1c4 < 0) {
        local_1c6 = 0;
        local_1c4 = 0;
      }
      iVar2 = iVar15 + local_3 * 4;
      uVar6 = *(uint *)(iVar2 + 0xd8);
      iVar2 = *(int *)(iVar2 + 0xda);
      iVar16 = iVar15 + local_3 * 4;
      *(int *)(iVar16 + 0xd8) = uVar6 + local_1c6;
      *(int *)(iVar16 + 0xda) = iVar2 + local_1c4 + (uint)CARRY2(uVar6,local_1c6);
      uVar6 = ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2];
      iVar2 = ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2];
      ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2] = uVar6 - local_1c6;
      ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2] = (iVar2 - local_1c4) - (uint)(uVar6 < local_1c6);
      FUN_10a0_1458();
      FUN_10a0_1444();
      uVar8 = *(undefined2 *)(iVar14 + 0x30cc);
      uVar10 = *(undefined2 *)(iVar14 + 0x30ce);
      uVar9 = FUN_10a0_1438();
      *(undefined2 *)(iVar14 + 0x30ca) = uVar9;
      *(undefined2 *)(iVar14 + 0x30cc) = uVar8;
      *(undefined2 *)(iVar14 + 0x30ce) = uVar10;
    }
    if (local_3 == '\b') break;
    local_3 = local_3 + '\x01';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_ba6a @ 1008:ba6a */
/* ======================================================== */

void __stdcall16far FUN_1008_ba6a(int *param_1)

{
  undefined2 uVar1;
  int unaff_BP;
  int *piVar2;
  undefined2 uVar3;
  char *unaff_CS;
  int *piVar4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  if (DAT_10a8_1d2f == '\0') {
    DAT_10a8_39a4 = 1;
    FUN_1008_963a(&iStack_2);
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    piVar2 = (int *)param_1;
    piVar4 = piVar2;
    uVar1 = uVar3;
    (*(code *)*(undefined2 *)(*param_1 + 0x50))();
    if (DAT_10a8_1d2f == '\0') {
      FUN_1008_a397(&iStack_2,piVar4,uVar1);
      if (DAT_10a8_1d2f == '\0') {
        piVar4 = piVar2;
        uVar1 = uVar3;
        (*(code *)*(undefined2 *)(*param_1 + 0x50))();
        if (DAT_10a8_1d2f == '\0') {
          if (DAT_10a8_22ce != '\0') {
            DAT_10a8_39a9 = '\x01';
            FUN_1008_1aa7(piVar4,uVar1);
            uVar1 = GETDC();
            FUN_1048_14c9(uVar1);
            unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
            RELEASEDC((char *)s_StartCash_10a8_1043 + 5,uVar1);
            do {
              (*(code *)*(undefined2 *)(*param_1 + 0x50))
                        ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3);
              if (DAT_10a8_39a9 == '\0') break;
            } while (DAT_10a8_1d2f == '\0');
            DAT_10a8_39a9 = 0;
          }
          if (DAT_10a8_1d2f == '\0') {
            POSTMESSAGE(unaff_CS,0,0,0,0x47d);
          }
          DAT_10a8_39a4 = 0;
        }
        else {
          uVar1 = GETDC();
          FUN_1048_14c9(uVar1);
          RELEASEDC((char *)s_StartCash_10a8_1043 + 5,uVar1);
          DAT_10a8_39a4 = 0;
        }
      }
      else {
        uVar1 = GETDC();
        FUN_1048_14c9(uVar1);
        RELEASEDC((char *)s_StartCash_10a8_1043 + 5,uVar1);
        DAT_10a8_39a4 = 0;
      }
    }
    else {
      uVar1 = GETDC();
      FUN_1048_14c9(uVar1);
      RELEASEDC((char *)s_StartCash_10a8_1043 + 5,uVar1);
      DAT_10a8_39a4 = 0;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_bbb8 @ 1008:bbb8 */
/* ======================================================== */

void __stdcall16far FUN_1008_bbb8(undefined4 param_1)

{
  uint uVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined1 extraout_AH;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  bool bVar10;
  bool bVar11;
  char local_5;
  
  local_5 = '\x01';
  while( true ) {
    bVar10 = false;
    bVar11 = true;
    FUN_10a0_1454();
    if (!bVar10 && !bVar11) {
      iVar6 = DAT_10a8_319a * 0xfc;
      iVar7 = iVar6 + 0x30aa;
      iVar8 = iVar7 + local_5 * 4;
      iVar9 = *(int *)(iVar8 + 0xda);
      if ((0 < iVar9) || ((-1 < iVar9 && (*(int *)(iVar8 + 0xd8) != 0)))) {
        iVar9 = iVar7 + local_5 * 4;
        uVar1 = *(uint *)(iVar9 + 0xd8);
        iVar9 = *(int *)(iVar9 + 0xda);
        uVar2 = ((undefined2 *)&DAT_10a8_3ada)[local_5 * 2];
        iVar8 = ((undefined2 *)&DAT_10a8_3adc)[local_5 * 2];
        ((undefined2 *)&DAT_10a8_3ada)[local_5 * 2] = uVar2 + uVar1;
        ((undefined2 *)&DAT_10a8_3adc)[local_5 * 2] = iVar8 + iVar9 + (uint)CARRY2(uVar2,uVar1);
        iVar9 = local_5 * 0x24;
        uVar4 = *(undefined2 *)(iVar9 + 0x3774);
        uVar5 = *(undefined2 *)(iVar7 + local_5 * 4 + 0xda);
        FUN_10a0_1458(*(undefined2 *)(iVar9 + 0x3772),uVar4,*(undefined2 *)(iVar9 + 0x3776));
        FUN_10a0_1444();
        uVar3 = FUN_10a0_1432();
        *(undefined2 *)(iVar6 + 0x30ca) = uVar3;
        *(undefined2 *)(iVar6 + 0x30cc) = uVar4;
        *(undefined2 *)(iVar6 + 0x30ce) = uVar5;
        iVar7 = iVar7 + local_5 * 4;
        *(undefined2 *)(iVar7 + 0xd8) = 0;
        *(undefined2 *)(iVar7 + 0xda) = 0;
      }
    }
    if (local_5 == '\b') break;
    local_5 = local_5 + '\x01';
  }
  uVar4 = GETDC((char *)s_RandomShrs_10a8_109f + 1,*(undefined2 *)((int)param_1 + 4));
  FUN_1048_14c9(uVar4);
  RELEASEDC((char *)s_StartCash_10a8_1043 + 5,uVar4);
  FUN_1028_0002(CONCAT11(extraout_AH,DAT_10a8_39a7),1,DAT_10a8_22aa,0x31a6,
                (char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_bd1a @ 1008:bd1a */
/* ======================================================== */

void __cdecl16far FUN_1008_bd1a(void)

{
  DAT_10a8_399c = 0x1e;
  DAT_10a8_399e = 0;
  DAT_10a8_39a5 = 0;
  DAT_10a8_1cfd = 0;
  DAT_10a8_1cfc = 9;
  return;
}

/* ======================================================== */
/* Function: FUN_1048_1db2 @ 1048:1db2 */
/* ======================================================== */

void __stdcall16far FUN_1048_1db2(undefined4 param_1)

{
  int iVar1;
  int unaff_BP;
  undefined2 uVar2;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  FUN_1048_1cea(&iStack_2,iVar1,uVar2,(char *)s_NumPlayers_10a8_1022,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1cea(&iStack_2,iVar1 + 4,uVar2,(char *)s_NumChits_10a8_102d,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1cea(&iStack_2,iVar1 + 8,uVar2,(char *)s_Columns_10a8_1036,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1cea(&iStack_2,iVar1 + 0xc,uVar2,0x103e,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1cea(&iStack_2,iVar1 + 0x10,uVar2,(char *)s_StartCash_10a8_1043,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1cea(&iStack_2,iVar1 + 0x14,uVar2,(char *)s_ShareLimitNumber_10a8_104d,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1cea(&iStack_2,iVar1 + 0x18,uVar2,(char *)s_PurchaseLimitNumber_10a8_105e,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,iVar1 + 0x1c,uVar2,(char *)s_Blanks_10a8_1072,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,iVar1 + 0x1d,uVar2,(char *)s_PayDividends_10a8_1079,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,iVar1 + 0x1e,uVar2,(char *)s_Cancel_10a8_1086,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,iVar1 + 0x1f,uVar2,0x108d,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,iVar1 + 0x20,uVar2,(char *)s_RandomDivs_10a8_1094,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,iVar1 + 0x21,uVar2,(char *)s_RandomShrs_10a8_109f,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,iVar1 + 0x22,uVar2,(char *)s_Purchaselimit_10a8_10aa,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,iVar1 + 0x23,uVar2,(char *)s_HardShare_10a8_10b8,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,(undefined1 *)&DAT_10a8_22ce,(char *)s_RandomShrs_10a8_109f + 9,
                (char *)s_SlowComputer_10a8_10c2,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1048_1d68(&iStack_2,(undefined1 *)&DAT_10a8_22cf,(char *)s_RandomShrs_10a8_109f + 9,
                (char *)s_ShowDiscard_10a8_10cf,(char *)s_RandomShrs_10a8_109f + 9);
  return;
}

