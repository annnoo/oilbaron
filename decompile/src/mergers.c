/*
 * mergers.c - Oil Baron Company Merger Resolution Subsystem
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - Trigger: Placement of a tile connecting 2 or more active companies.
 *   - Survivor Determination (FUN_1008_2448):
 *       * Company with strictly greater tile count acquires the smaller.
 *       * In case of a tie in size, player or tie-breaking priority decides.
 *   - Shareholder Bonus Distribution:
 *       * Majority shareholder bonus: 10 * share price
 *       * Minority shareholder bonus:  5 * share price
 *       * Tied holdings divide bonuses equally among tied shareholders.
 *   - Share Disposition Options (FUN_1060_02a7..0397):
 *       Each holder of defunct company shares chooses in turn:
 *       1. KEEP:  Hold shares in anticipation of company rebirth.
 *       2. SELL:  Liquidate at pre-merger share price.
 *       3. TRADE: Exchange 2 defunct shares for 1 acquirer share (if stock remains).
 *   - UI Dialogs:
 *       * MERGER STATISTICS dialog (FUN_1060_0002 / FUN_1060_0045)
 *       * ShareOptionsDialog (FUN_1060_02a7 / FUN_1060_02f8)
 */

/* Function Index:
 *   FUN_1008_2448        @ 1008:2448
 *   FUN_1060_0002        @ 1060:0002
 *   FUN_1060_0045        @ 1060:0045
 *   FUN_1060_0219        @ 1060:0219
 *   FUN_1060_02a7        @ 1060:02a7
 *   FUN_1060_02f8        @ 1060:02f8
 *   FUN_1060_035b        @ 1060:035b
 *   FUN_1060_0379        @ 1060:0379
 *   FUN_1060_0397        @ 1060:0397
 */

/* ======================================================== */
/* Function: FUN_1008_2448 @ 1008:2448 */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x100824a8) */
/* WARNING: Removing unreachable block (ram,0x10082602) */
/* WARNING: Removing unreachable block (ram,0x10082c9a) */

undefined2 __stdcall16far
FUN_1008_2448(undefined4 param_1,char param_2,uint param_3,int param_4,uint param_5,int param_6)

{
  undefined2 *puVar1;
  char cVar2;
  undefined1 extraout_AH;
  uint uVar3;
  uint uVar4;
  undefined1 uVar8;
  undefined2 uVar5;
  undefined2 uVar6;
  uint uVar7;
  undefined1 extraout_AH_00;
  undefined1 extraout_AH_01;
  int iVar9;
  int iVar10;
  char *pcVar11;
  char *pcVar12;
  int unaff_BP;
  int iVar13;
  undefined2 *puVar14;
  int *piVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  undefined2 uVar20;
  undefined2 uVar21;
  undefined2 unaff_SS;
  bool bVar22;
  bool bVar23;
  undefined4 uVar24;
  undefined1 local_106 [102];
  uint local_a0;
  int local_9e;
  uint local_9c;
  int local_9a;
  undefined2 local_98;
  int local_96 [19];
  char local_70;
  undefined4 local_68;
  uint local_64;
  uint local_62;
  int local_60;
  int local_5e [21];
  char local_33;
  undefined2 local_32;
  char *local_30;
  undefined2 local_2e;
  uint local_2c;
  uint local_2a;
  uint local_28;
  int local_26;
  uint local_24;
  int local_22;
  uint local_20;
  int local_1e;
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
  char local_8;
  char local_7;
  char local_6;
  char local_5;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  local_68 = (undefined2 *)FUN_10a0_012d(0x60);
  iVar9 = DAT_10a8_22ac;
  uVar6 = (undefined2)((ulong)local_68 >> 0x10);
  puVar14 = (undefined2 *)local_68;
  puVar14[4] = DAT_10a8_22aa;
  puVar14[5] = iVar9;
  local_6 = *(char *)(param_5 * 0x8c + param_3 * 5 + 0x22d2);
  local_33 = '\0';
  iVar9 = param_4 - (uint)(param_3 == 0);
  if ((((0 < iVar9) || (-1 < iVar9)) &&
      (*(char *)(param_5 * 0x8c + (param_3 - 1) * 5 + 0x22d2) != '\t')) &&
     ((*(char *)(param_5 * 0x8c + (param_3 - 1) * 5 + 0x22d2) != '\0' &&
      (*(char *)(param_5 * 0x8c + (param_3 - 1) * 5 + 0x22d2) != local_6)))) {
    local_33 = '\x01';
    local_7 = *(char *)(param_5 * 0x8c + (param_3 - 1) * 5 + 0x22d2);
  }
  iVar9 = param_4 + (uint)(0xfffe < param_3);
  if (((iVar9 < DAT_10a8_22b8) || ((iVar9 <= DAT_10a8_22b8 && (param_3 + 1 < DAT_10a8_22b6)))) &&
     ((*(char *)(param_5 * 0x8c + (param_3 + 1) * 5 + 0x22d2) != '\t' &&
      ((*(char *)(param_5 * 0x8c + (param_3 + 1) * 5 + 0x22d2) != '\0' &&
       (*(char *)(param_5 * 0x8c + (param_3 + 1) * 5 + 0x22d2) != local_6)))))) {
    local_33 = '\x01';
    local_7 = *(char *)(param_5 * 0x8c + (param_3 + 1) * 5 + 0x22d2);
  }
  iVar9 = param_6 - (uint)(param_5 == 0);
  if ((((0 < iVar9) || (-1 < iVar9)) &&
      (*(char *)((param_5 - 1) * 0x8c + param_3 * 5 + 0x22d2) != '\t')) &&
     ((*(char *)((param_5 - 1) * 0x8c + param_3 * 5 + 0x22d2) != '\0' &&
      (*(char *)((param_5 - 1) * 0x8c + param_3 * 5 + 0x22d2) != local_6)))) {
    local_33 = '\x01';
    local_7 = *(char *)((param_5 - 1) * 0x8c + param_3 * 5 + 0x22d2);
  }
  iVar9 = param_6 + (uint)(0xfffe < param_5);
  if ((((iVar9 < DAT_10a8_22b4) || ((iVar9 <= DAT_10a8_22b4 && (param_5 + 1 < DAT_10a8_22b2)))) &&
      (*(char *)((param_5 + 1) * 0x8c + param_3 * 5 + 0x22d2) != '\t')) &&
     ((*(char *)((param_5 + 1) * 0x8c + param_3 * 5 + 0x22d2) != '\0' &&
      (*(char *)((param_5 + 1) * 0x8c + param_3 * 5 + 0x22d2) != local_6)))) {
    local_33 = '\x01';
    local_7 = *(char *)((param_5 + 1) * 0x8c + param_3 * 5 + 0x22d2);
  }
  if (param_2 == '\0') {
    local_5 = local_33;
    FUN_10a0_0147(0x60,local_68);
    uVar8 = extraout_AH;
  }
  else {
    iVar9 = (int)param_1;
    uVar20 = (undefined2)((ulong)param_1 >> 0x10);
    if (local_33 == '\0') {
      local_5 = '\0';
    }
    else {
      uVar21 = *(undefined2 *)((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + local_6 * 4);
      puVar14[2] = *(undefined2 *)
                    ((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 + local_6 * 4);
      puVar14[3] = uVar21;
      iVar13 = local_7 * 4;
      bVar23 = iVar13 == 0;
      uVar21 = *(undefined2 *)((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + iVar13);
      *local_68 = *(undefined2 *)((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 + iVar13);
      puVar14[1] = uVar21;
      iVar13 = (int)((long)(int)local_6 * 0x24);
      bVar22 = (long)iVar13 != (long)(int)local_6 * 0x24;
      pcVar11 = (char *)*(undefined2 *)(iVar13 + 0x3774);
      FUN_10a0_1454();
      cVar2 = local_6;
      if (!bVar22 && !bVar23) {
        local_8 = local_6;
        local_6 = local_7;
        local_7 = cVar2;
      }
      local_14 = 0;
      local_12 = 0;
      local_18 = 1;
      local_16 = 0;
      local_1c = 0;
      local_1a = 0;
      local_20 = 1;
      local_1e = 0;
      if ((-1 < DAT_10a8_22ac) && ((0 < DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
        local_24 = 1;
        local_22 = 0;
        while( true ) {
          iVar16 = local_24 * 0xfc + 0x30aa;
          iVar10 = iVar16 + local_6 * 4;
          iVar13 = *(int *)(iVar10 + 0xda);
          if ((local_16 < iVar13) || ((local_16 <= iVar13 && (local_18 < *(uint *)(iVar10 + 0xd8))))
             ) {
            iVar16 = iVar16 + local_6 * 4;
            local_18 = *(uint *)(iVar16 + 0xd8);
            local_16 = *(int *)(iVar16 + 0xda);
            local_14 = 1;
            local_12 = 0;
          }
          else {
            iVar16 = iVar16 + local_6 * 4;
            if ((*(int *)(iVar16 + 0xda) == local_16) && (*(uint *)(iVar16 + 0xd8) == local_18)) {
              bVar23 = 0xfffe < local_14;
              local_14 = local_14 + 1;
              local_12 = local_12 + (uint)bVar23;
            }
          }
          if ((local_22 == DAT_10a8_22ac) && (local_24 == DAT_10a8_22aa)) break;
          bVar23 = 0xfffe < local_24;
          local_24 = local_24 + 1;
          local_22 = local_22 + (uint)bVar23;
        }
      }
      uVar3 = 0;
      if ((-1 < DAT_10a8_22ac) && ((0 < DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
        local_24 = 1;
        local_22 = 0;
        do {
          iVar16 = local_24 * 0xfc + 0x30aa;
          iVar10 = iVar16 + local_6 * 4;
          iVar13 = *(int *)(iVar10 + 0xda);
          if ((iVar13 < local_16) || ((iVar13 <= local_16 && (*(uint *)(iVar10 + 0xd8) < local_18)))
             ) {
            iVar10 = iVar16 + local_6 * 4;
            iVar13 = *(int *)(iVar10 + 0xda);
            if ((iVar13 <= local_1e) &&
               ((iVar13 < local_1e || (*(uint *)(iVar10 + 0xd8) <= local_20)))) goto LAB_1008_29ba;
            iVar16 = iVar16 + local_6 * 4;
            local_20 = *(uint *)(iVar16 + 0xd8);
            local_1e = *(int *)(iVar16 + 0xda);
            local_1c = 1;
            local_1a = 0;
          }
          else {
LAB_1008_29ba:
            iVar16 = iVar16 + local_6 * 4;
            if ((*(int *)(iVar16 + 0xda) == local_1e) && (*(uint *)(iVar16 + 0xd8) == local_20)) {
              bVar23 = 0xfffe < local_1c;
              local_1c = local_1c + 1;
              local_1a = local_1a + (uint)bVar23;
            }
          }
          if ((local_22 == DAT_10a8_22ac) && (uVar3 = local_24, local_24 == DAT_10a8_22aa)) break;
          bVar23 = 0xfffe < local_24;
          local_24 = local_24 + 1;
          local_22 = local_22 + (uint)bVar23;
        } while( true );
      }
      pcVar12 = (char *)s_StartCash_10a8_1043 + 5;
      uVar24 = FUN_1048_0bd8(CONCAT11((char)(uVar3 >> 8),local_6));
      iVar13 = DAT_10a8_22ac;
      uVar3 = DAT_10a8_22aa;
      local_30 = pcVar11;
      if ((-1 < DAT_10a8_22ac) && ((0 < DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
        local_24 = 1;
        local_22 = 0;
        while( true ) {
          local_2e = (undefined2)((ulong)uVar24 >> 0x10);
          local_32 = (undefined2)uVar24;
          iVar10 = local_24 * 0xfc;
          iVar17 = iVar10 + 0x30aa;
          uVar21 = (undefined2)((ulong)local_68 >> 0x10);
          puVar14 = (undefined2 *)local_68;
          puVar14[local_24 * 7 + 1] = 0;
          puVar14[local_24 * 7 + 2] = 0;
          puVar14[local_24 * 7 + 3] = 0;
          iVar16 = iVar17 + local_6 * 4;
          uVar6 = *(undefined2 *)(iVar16 + 0xda);
          (puVar14 + local_24 * 7)[-1] = *(undefined2 *)(iVar16 + 0xd8);
          puVar14[local_24 * 7] = uVar6;
          pcVar11 = (char *)s_RandomShrs_10a8_109f + 9;
          puVar14[local_24 * 7 + 4] = iVar17;
          puVar14[local_24 * 7 + 5] = (char *)s_RandomShrs_10a8_109f + 9;
          iVar16 = iVar17 + local_6 * 4;
          if ((*(int *)(iVar16 + 0xda) == local_16) && (*(uint *)(iVar16 + 0xd8) == local_18)) {
            FUN_10a0_1458();
            uVar6 = local_2e;
            pcVar11 = local_30;
            FUN_10a0_144a();
            uVar21 = FUN_10a0_1432();
            *(undefined2 *)(iVar10 + 0x30ca) = uVar21;
            *(undefined2 *)(iVar10 + 0x30cc) = pcVar11;
            *(undefined2 *)(iVar10 + 0x30ce) = uVar6;
            FUN_10a0_1458();
            pcVar12 = (char *)s_RandomShrs_10a8_109f + 1;
            uVar6 = local_2e;
            pcVar11 = local_30;
            uVar21 = FUN_10a0_144a();
            uVar24 = CONCAT22(local_2e,local_32);
            uVar5 = (undefined2)((ulong)local_68 >> 0x10);
            puVar14 = (undefined2 *)local_68;
            puVar14[local_24 * 7 + 1] = uVar21;
            puVar14[local_24 * 7 + 2] = pcVar11;
            puVar14[local_24 * 7 + 3] = uVar6;
          }
          local_2e = (undefined2)((ulong)uVar24 >> 0x10);
          local_32 = (undefined2)uVar24;
          iVar17 = iVar17 + local_6 * 4;
          if ((*(int *)(iVar17 + 0xda) == local_1e) && (*(uint *)(iVar17 + 0xd8) == local_20)) {
            iVar16 = local_1a;
            uVar21 = FUN_10a0_1458();
            uVar6 = local_2e;
            pcVar12 = local_30;
            FUN_10a0_144a(uVar21,pcVar11,iVar16);
            FUN_10a0_144a();
            uVar21 = FUN_10a0_1432();
            *(undefined2 *)(iVar10 + 0x30ca) = uVar21;
            *(undefined2 *)(iVar10 + 0x30cc) = pcVar12;
            *(undefined2 *)(iVar10 + 0x30ce) = uVar6;
            iVar10 = local_1a;
            uVar21 = FUN_10a0_1458();
            uVar6 = local_2e;
            pcVar11 = local_30;
            FUN_10a0_144a(uVar21,pcVar12,iVar10);
            pcVar12 = (char *)s_RandomShrs_10a8_109f + 1;
            uVar21 = FUN_10a0_144a();
            uVar24 = CONCAT22(local_2e,local_32);
            uVar5 = (undefined2)((ulong)local_68 >> 0x10);
            puVar14 = (undefined2 *)local_68;
            puVar14[local_24 * 7 + 1] = uVar21;
            puVar14[local_24 * 7 + 2] = pcVar11;
            puVar14[local_24 * 7 + 3] = uVar6;
          }
          if ((local_22 == iVar13) && (local_24 == uVar3)) break;
          bVar23 = 0xfffe < local_24;
          local_24 = local_24 + 1;
          local_22 = local_22 + (uint)bVar23;
        }
      }
      local_2e = (undefined2)((ulong)uVar24 >> 0x10);
      local_32 = (undefined2)uVar24;
      if (DAT_10a8_39a2 == '\0') {
        pcVar12 = (char *)s_PurchaseLimitNumber_10a8_105e + 2;
        FUN_1060_0002(local_5e,unaff_SS,0x1266,(undefined2 *)local_68,local_68._2_2_,iVar9,uVar20);
        (*(code *)*(undefined2 *)(local_5e[0] + 0x4c))
                  ((char *)s_PurchaseLimitNumber_10a8_105e + 2,local_5e);
        (*(code *)*(undefined2 *)(local_5e[0] + 8))
                  ((char *)s_PurchaseLimitNumber_10a8_105e + 2,local_5e);
        uVar24 = CONCAT22(local_2e,local_32);
      }
      uVar3 = DAT_10a8_22b2 - 1;
      iVar13 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
      if (-1 < iVar13) {
        local_28 = 0;
        local_26 = 0;
        while( true ) {
          uVar4 = DAT_10a8_22b6 - 1;
          uVar7 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
          if (uVar7 < 0x8000) {
            local_2c = 0;
            local_2a = 0;
            while( true ) {
              local_2e = (undefined2)((ulong)uVar24 >> 0x10);
              local_32 = (undefined2)uVar24;
              if (*(char *)(local_28 * 0x8c + local_2c * 5 + 0x22d2) == local_6) {
                piVar15 = (int *)(local_28 * 0x8c + local_2c * 5 + 0x22d0);
                puVar1 = (undefined2 *)(*piVar15 + 0x10);
                (*(code *)*puVar1)(pcVar12,piVar15,(char *)s_RandomShrs_10a8_109f + 9,
                                   CONCAT11((char)(local_2c * 5 >> 8),local_7));
                uVar24 = CONCAT22(local_2e,local_32);
              }
              if ((local_2a == uVar7) && (local_2c == uVar4)) break;
              bVar23 = 0xfffe < local_2c;
              local_2c = local_2c + 1;
              local_2a = local_2a + bVar23;
            }
          }
          if ((local_26 == iVar13) && (local_28 == uVar3)) break;
          bVar23 = 0xfffe < local_28;
          local_28 = local_28 + 1;
          local_26 = local_26 + (uint)bVar23;
        }
      }
      DAT_10a8_1d30 = 1;
      local_60 = 1;
      while( true ) {
        if (local_60 == 1) {
          local_62 = DAT_10a8_319a;
          local_64 = DAT_10a8_22aa;
        }
        else {
          local_62 = 1;
          local_64 = DAT_10a8_319a - 1;
        }
        uVar3 = local_64;
        if (0 < (int)local_64) {
          iVar13 = (int)local_64 >> 0xf;
          iVar10 = (int)local_62 >> 0xf;
          if ((iVar10 <= iVar13) && ((iVar10 < iVar13 || (local_62 <= local_64)))) {
            local_28 = local_62;
            local_26 = iVar10;
            while( true ) {
              local_2e = (undefined2)((ulong)uVar24 >> 0x10);
              local_32 = (undefined2)uVar24;
              iVar16 = local_28 * 0xfc + local_6 * 4;
              iVar10 = *(int *)(iVar16 + 0x3184);
              if ((0 < iVar10) || ((-1 < iVar10 && (*(int *)(iVar16 + 0x3182) != 0)))) {
                uVar8 = (undefined1)((uint)(local_6 * 4) >> 8);
                if (*(char *)(local_28 * 0xfc + 0x30c9) == '\0') {
                  FUN_1060_02a7(local_96,unaff_SS,0x12d4,CONCAT11(uVar8,local_7),
                                CONCAT11(uVar8,local_6),local_28 * 0x20 + 0x38bd,
                                (char *)s_RandomShrs_10a8_109f + 9,local_28,local_26,iVar9,uVar20);
                  (*(code *)*(undefined2 *)(local_96[0] + 0x4c))
                            ((char *)s_PurchaseLimitNumber_10a8_105e + 2,local_96);
                  (*(code *)*(undefined2 *)(local_96[0] + 8))
                            ((char *)s_PurchaseLimitNumber_10a8_105e + 2,local_96);
                }
                else {
                  local_70 = FUN_1048_0b6b(CONCAT11(uVar8,local_6),CONCAT11(uVar8,local_7),local_28,
                                           local_26);
                }
                uVar24 = CONCAT22(local_2e,local_32);
                iVar10 = local_28 * 0xfc;
                iVar16 = iVar10 + 0x30aa;
                if (local_70 == '\0') {
                  iVar17 = local_6 * 0x24;
                  uVar6 = *(undefined2 *)(iVar17 + 0x3774);
                  uVar21 = *(undefined2 *)(iVar16 + local_6 * 4 + 0xda);
                  FUN_10a0_1458(*(undefined2 *)(iVar17 + 0x3772),uVar6,
                                *(undefined2 *)(iVar17 + 0x3776));
                  FUN_10a0_1444();
                  uVar5 = FUN_10a0_1432();
                  uVar24 = CONCAT22(local_2e,local_32);
                  *(undefined2 *)(iVar10 + 0x30ca) = uVar5;
                  *(undefined2 *)(iVar10 + 0x30cc) = uVar6;
                  *(undefined2 *)(iVar10 + 0x30ce) = uVar21;
                  iVar16 = iVar16 + local_6 * 4;
                  *(undefined2 *)(iVar16 + 0xd8) = 0;
                  *(undefined2 *)(iVar16 + 0xda) = 0;
                }
                else if (local_70 == '\x01') {
                  uVar24 = FUN_1048_0fac(local_7);
                  local_9c = DAT_10a8_22be - (uint)uVar24;
                  local_9a = (DAT_10a8_22c0 - (int)((ulong)uVar24 >> 0x10)) -
                             (uint)(DAT_10a8_22be < (uint)uVar24);
                  iVar17 = *(int *)(iVar16 + local_6 * 4 + 0xda);
                  local_a0 = FUN_10a0_0be7();
                  local_9e = iVar17;
                  if ((DAT_10a8_22cd == '\0') ||
                     ((iVar17 <= local_9a && ((iVar17 < local_9a || (local_a0 <= local_9c)))))) {
                    iVar17 = iVar16 + local_6 * 4;
                    *(undefined2 *)(iVar17 + 0xd8) = 0;
                    *(undefined2 *)(iVar17 + 0xda) = 0;
                  }
                  else {
                    iVar19 = local_9a;
                    uVar7 = FUN_10a0_0baa();
                    iVar17 = iVar16 + local_6 * 4;
                    uVar4 = *(uint *)(iVar17 + 0xd8);
                    iVar17 = *(int *)(iVar17 + 0xda);
                    iVar18 = iVar16 + local_6 * 4;
                    *(int *)(iVar18 + 0xd8) = uVar4 - uVar7;
                    *(int *)(iVar18 + 0xda) = (iVar17 - iVar19) - (uint)(uVar4 < uVar7);
                    local_a0 = local_9c;
                    local_9e = local_9a;
                    if (*(char *)(iVar10 + 0x30c9) == '\0') {
                      iVar17 = iVar16 + local_6 * 4;
                      FUN_10a0_0a72(100,local_106,unaff_SS,0,*(undefined2 *)(iVar17 + 0xd8),
                                    *(undefined2 *)(iVar17 + 0xda));
                      FUN_1098_00bd(0x3b0,(char *)s_RandomShrs_10a8_109f + 9,local_106,unaff_SS);
                      MESSAGEBOX((char *)s_RandomDivs_10a8_1094 + 4,0x40,0x3cd,
                                 (char *)s_RandomShrs_10a8_109f + 9,local_106,unaff_SS);
                    }
                  }
                  iVar17 = iVar16 + local_7 * 4;
                  uVar4 = *(uint *)(iVar17 + 0xd8);
                  iVar17 = *(int *)(iVar17 + 0xda);
                  iVar19 = iVar16 + local_7 * 4;
                  *(int *)(iVar19 + 0xd8) = uVar4 + local_a0;
                  *(int *)(iVar19 + 0xda) = iVar17 + local_9e + (uint)CARRY2(uVar4,local_a0);
                  iVar17 = local_6 * 0x24;
                  uVar6 = *(undefined2 *)(iVar17 + 0x3774);
                  uVar21 = *(undefined2 *)(iVar16 + local_6 * 4 + 0xda);
                  FUN_10a0_1458(*(undefined2 *)(iVar17 + 0x3772),uVar6,
                                *(undefined2 *)(iVar17 + 0x3776));
                  FUN_10a0_1444();
                  uVar5 = FUN_10a0_1432();
                  uVar24 = CONCAT22(local_2e,local_32);
                  *(undefined2 *)(iVar10 + 0x30ca) = uVar5;
                  *(undefined2 *)(iVar10 + 0x30cc) = uVar6;
                  *(undefined2 *)(iVar10 + 0x30ce) = uVar21;
                  iVar16 = iVar16 + local_6 * 4;
                  *(undefined2 *)(iVar16 + 0xd8) = 0;
                  *(undefined2 *)(iVar16 + 0xda) = 0;
                }
                else if (local_70 == '\x02') {
                  iVar17 = iVar16 + local_6 * 4;
                  local_c = *(uint *)(iVar17 + 0xd8);
                  local_a = *(int *)(iVar17 + 0xda);
                  FUN_1048_0002(&local_10,unaff_SS,&local_c,unaff_SS);
                  iVar18 = local_6 * 0x24;
                  iVar17 = local_a + local_e + (uint)CARRY2(local_c,local_10);
                  iVar19 = iVar16 + local_6 * 4;
                  iVar19 = (*(int *)(iVar19 + 0xda) - iVar17) -
                           (uint)(*(uint *)(iVar19 + 0xd8) < local_c + local_10);
                  FUN_10a0_1458(*(undefined2 *)(iVar18 + 0x3772),*(undefined2 *)(iVar18 + 0x3774),
                                *(undefined2 *)(iVar18 + 0x3776));
                  FUN_10a0_1444();
                  uVar6 = FUN_10a0_1432();
                  *(undefined2 *)(iVar10 + 0x30ca) = uVar6;
                  *(int *)(iVar10 + 0x30cc) = iVar17;
                  *(int *)(iVar10 + 0x30ce) = iVar19;
                  iVar17 = iVar16 + local_6 * 4;
                  *(uint *)(iVar17 + 0xd8) = local_10;
                  *(int *)(iVar17 + 0xda) = local_e;
                  uVar24 = FUN_1048_0fac(CONCAT11((char)((uint)(local_6 * 4) >> 8),local_7));
                  local_9c = DAT_10a8_22be - (uint)uVar24;
                  local_9a = (DAT_10a8_22c0 - (int)((ulong)uVar24 >> 0x10)) -
                             (uint)(DAT_10a8_22be < (uint)uVar24);
                  iVar17 = *(int *)(iVar16 + local_6 * 4 + 0xda);
                  local_a0 = FUN_10a0_0be7();
                  local_9e = iVar17;
                  if ((DAT_10a8_22cd == '\0') ||
                     ((iVar17 <= local_9a && ((iVar17 < local_9a || (local_a0 <= local_9c)))))) {
                    iVar17 = iVar16 + local_6 * 4;
                    *(undefined2 *)(iVar17 + 0xd8) = 0;
                    *(undefined2 *)(iVar17 + 0xda) = 0;
                  }
                  else {
                    iVar19 = local_9a;
                    uVar7 = FUN_10a0_0baa();
                    iVar17 = iVar16 + local_6 * 4;
                    uVar4 = *(uint *)(iVar17 + 0xd8);
                    iVar17 = *(int *)(iVar17 + 0xda);
                    iVar18 = iVar16 + local_6 * 4;
                    *(int *)(iVar18 + 0xd8) = uVar4 - uVar7;
                    *(int *)(iVar18 + 0xda) = (iVar17 - iVar19) - (uint)(uVar4 < uVar7);
                    local_a0 = local_9c;
                    local_9e = local_9a;
                    if (*(char *)(iVar10 + 0x30c9) == '\0') {
                      iVar17 = iVar16 + local_6 * 4;
                      FUN_10a0_0a72(100,local_106,unaff_SS,0,*(undefined2 *)(iVar17 + 0xd8),
                                    *(undefined2 *)(iVar17 + 0xda));
                      FUN_1098_00bd(0x3b0,(char *)s_RandomShrs_10a8_109f + 9,local_106,unaff_SS);
                      MESSAGEBOX((char *)s_RandomDivs_10a8_1094 + 4,0x40,0x3cd,
                                 (char *)s_RandomShrs_10a8_109f + 9,local_106,unaff_SS);
                    }
                  }
                  iVar17 = iVar16 + local_7 * 4;
                  uVar4 = *(uint *)(iVar17 + 0xd8);
                  iVar17 = *(int *)(iVar17 + 0xda);
                  iVar19 = iVar16 + local_7 * 4;
                  *(int *)(iVar19 + 0xd8) = uVar4 + local_a0;
                  *(int *)(iVar19 + 0xda) = iVar17 + local_9e + (uint)CARRY2(uVar4,local_a0);
                  iVar17 = local_6 * 0x24;
                  uVar6 = *(undefined2 *)(iVar17 + 0x3774);
                  uVar21 = *(undefined2 *)(iVar16 + local_6 * 4 + 0xda);
                  FUN_10a0_1458(*(undefined2 *)(iVar17 + 0x3772),uVar6,
                                *(undefined2 *)(iVar17 + 0x3776));
                  FUN_10a0_1444();
                  uVar5 = FUN_10a0_1432();
                  uVar24 = CONCAT22(local_2e,local_32);
                  *(undefined2 *)(iVar10 + 0x30ca) = uVar5;
                  *(undefined2 *)(iVar10 + 0x30cc) = uVar6;
                  *(undefined2 *)(iVar10 + 0x30ce) = uVar21;
                  iVar16 = iVar16 + local_6 * 4;
                  *(uint *)(iVar16 + 0xd8) = local_c;
                  *(int *)(iVar16 + 0xda) = local_a;
                }
              }
              if ((local_26 == iVar13) && (local_28 == uVar3)) break;
              bVar23 = 0xfffe < local_28;
              local_28 = local_28 + 1;
              local_26 = local_26 + (uint)bVar23;
            }
          }
        }
        local_2e = (undefined2)((ulong)uVar24 >> 0x10);
        local_32 = (undefined2)uVar24;
        if (local_60 == 2) break;
        local_60 = local_60 + 1;
      }
      FUN_1008_4c3f(iVar9,uVar20);
      FUN_1028_0002(CONCAT11(extraout_AH_00,DAT_10a8_39a7),1,DAT_10a8_22aa,0x31a6,
                    (char *)s_RandomShrs_10a8_109f + 9);
      local_5 = '\x01';
      FUN_1008_2448(iVar9,uVar20,1,param_3,param_4,param_5,param_6);
    }
    FUN_10a0_0147(0x60,(undefined2 *)local_68,local_68._2_2_);
    local_98 = GETDC((char *)s_RandomShrs_10a8_109f + 1,*(undefined2 *)(iVar9 + 4));
    FUN_1048_14c9(local_98);
    RELEASEDC((char *)s_StartCash_10a8_1043 + 5,local_98);
    uVar8 = extraout_AH_01;
  }
  return CONCAT11(uVar8,local_5);
}

/* ======================================================== */
/* Function: FUN_1060_0002 @ 1060:0002 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1060_0002(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6)

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
    FUN_1080_0002(iVar1,uVar2,0,(char *)s_MERGER_STATISTICS_10a8_1328,
                  (char *)s_RandomShrs_10a8_109f + 9,param_5,param_6);
    *(undefined2 *)(iVar1 + 0x26) = param_3;
    *(undefined2 *)(iVar1 + 0x28) = param_4;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1060_0045 @ 1060:0045 */
/* ======================================================== */

void __stdcall16far FUN_1060_0045(undefined4 param_1)

{
  undefined2 *puVar1;
  int unaff_BP;
  int iVar2;
  undefined2 *puVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  char *pcVar6;
  undefined2 unaff_SS;
  bool bVar7;
  undefined1 local_3c [52];
  uint local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar2 = (int)param_1;
  pcVar6 = (char *)s_Blanks_10a8_1072 + 6;
  FUN_1078_0d9b(iVar2,uVar4);
  puVar1 = (undefined2 *)*(undefined4 *)(iVar2 + 0x26);
  local_8 = 1;
  local_6 = 0;
  do {
    uVar5 = (undefined2)((ulong)puVar1 >> 0x10);
    puVar3 = (undefined2 *)puVar1;
    if ((local_6 < (int)puVar3[5]) || ((local_6 <= (int)puVar3[5] && (local_8 <= (uint)puVar3[4]))))
    {
      bVar7 = true;
      pcVar6 = (char *)s_RandomShrs_10a8_109f + 1;
      FUN_10a0_1454();
      if (bVar7) goto LAB_1060_0161;
      SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,puVar3[local_8 * 7 + 4],
                     puVar3[local_8 * 7 + 5],local_8 + 0xd1);
      FUN_1060_0219(iVar2,uVar4,local_8 + 0xdb,puVar3 + local_8 * 7 + -1,uVar5);
      FUN_10a0_0af5(0x32,local_3c,unaff_SS,2,5,puVar3[local_8 * 7 + 1],puVar3[local_8 * 7 + 2],
                    puVar3[local_8 * 7 + 3]);
      SETDLGITEMTEXT((char *)s_RandomShrs_10a8_109f + 1,local_3c,unaff_SS,local_8 + 0xe5);
    }
    else {
LAB_1060_0161:
      SETDLGITEMTEXT(pcVar6,0x133a,(char *)s_RandomShrs_10a8_109f + 9,local_8 + 0xd1);
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x133a,(char *)s_RandomShrs_10a8_109f + 9,
                     local_8 + 0xdb);
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,0x133a,(char *)s_RandomShrs_10a8_109f + 9,
                     local_8 + 0xe5);
    }
    pcVar6 = (char *)s__Show_Graph_10a8_1226 + 10;
    if ((local_6 == 0) && (local_8 == 6)) {
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,*puVar1,puVar3[1],400);
      SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,puVar3[2],puVar3[3],0x191);
      return;
    }
    bVar7 = 0xfffe < local_8;
    local_8 = local_8 + 1;
    local_6 = local_6 + (uint)bVar7;
  } while( true );
}

/* ======================================================== */
/* Function: FUN_1060_0219 @ 1060:0219 */
/* ======================================================== */

void __stdcall16far
FUN_1060_0219(undefined2 param_1,undefined2 param_2,undefined2 param_3,int *param_4)

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
/* Function: FUN_1060_02a7 @ 1060:02a7 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1060_02a7(undefined4 param_1,undefined2 param_2,undefined1 param_3,undefined1 param_4,
             undefined2 param_5,undefined2 param_6,undefined2 param_7,undefined2 param_8,
             undefined2 param_9,undefined2 param_10)

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
    FUN_1080_0002(iVar1,uVar2,0,(char *)s_ShareOptionsDialog_10a8_133c,
                  (char *)s_RandomShrs_10a8_109f + 9,param_9,param_10);
    *(undefined2 *)(iVar1 + 0x27) = param_5;
    *(undefined2 *)(iVar1 + 0x29) = param_6;
    *(undefined1 *)(iVar1 + 0x2b) = param_4;
    *(undefined1 *)(iVar1 + 0x2c) = param_3;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1060_02f8 @ 1060:02f8 */
/* ======================================================== */

void __stdcall16far FUN_1060_02f8(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  FUN_1078_0d9b(iVar1,uVar3);
  *(undefined1 *)(iVar1 + 0x26) = 0;
  CHECKDLGBUTTON((char *)s_Blanks_10a8_1072 + 6,1,0x66);
  iVar2 = *(char *)(iVar1 + 0x2b) * 4;
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,
                 *(undefined2 *)((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x17 + iVar2),
                 *(undefined2 *)((char *)s_Thank_you_for_Registering__10a8_18d9 + 0x19 + iVar2),0x9a
                );
  SETDLGITEMTEXT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar1 + 0x27),
                 *(undefined2 *)(iVar1 + 0x29),0x9b);
  return;
}

/* ======================================================== */
/* Function: FUN_1060_035b @ 1060:035b */
/* ======================================================== */

void __stdcall16far FUN_1060_035b(undefined4 param_1,undefined4 param_2)

{
  if (*(int *)((int)param_2 + 8) == 0) {
    *(undefined1 *)((int)param_1 + 0x26) = 1;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1060_0379 @ 1060:0379 */
/* ======================================================== */

void __stdcall16far FUN_1060_0379(undefined4 param_1,undefined4 param_2)

{
  if (*(int *)((int)param_2 + 8) == 0) {
    *(undefined1 *)((int)param_1 + 0x26) = 2;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1060_0397 @ 1060:0397 */
/* ======================================================== */

void __stdcall16far FUN_1060_0397(undefined4 param_1,undefined4 param_2)

{
  if (*(int *)((int)param_2 + 8) == 0) {
    *(undefined1 *)((int)param_1 + 0x26) = 0;
  }
  return;
}

