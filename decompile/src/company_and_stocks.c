/*
 * company_and_stocks.c - Oil Baron Companies & Stock Market Subsystem
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - 7 Oil Companies categorized into 3 pricing tiers:
 *       * Low Tier  (Base $200): Tower, Royal (Colored Cube)
 *       * Mid Tier  (Base $300): American Natural Gas, Oil Arabia
 *       * High Tier (Base $400): Imperial, SupraCo, Weem Oil Refining
 *   - Stock Pricing Curve (FUN_1008_88d9 / FUN_1008_8826):
 *       Share price increases with company territory tile count.
 *   - Shareholder Payouts (FUN_1008_50e3 / FUN_1008_7a91):
 *       * Majority Bonus: 10x current share price
 *       * Minority Bonus:  5x current share price
 *   - Company Formation & Founding (FUN_1008_4c3f, FUN_1008_51fd):
 *       Founder receives 1 free bonus share when establishing a company.
 *   - Stock Trading & Limits (FUN_1008_38c8, FUN_1008_71fc):
 *       Players can purchase up to 3 shares per turn; enforce share limits.
 */

/* Function Index:
 *   FUN_1008_0002        @ 1008:0002
 *   FUN_1008_0057        @ 1008:0057
 *   FUN_1008_0084        @ 1008:0084
 *   FUN_1008_0ed2        @ 1008:0ed2
 *   FUN_1008_1936        @ 1008:1936
 *   FUN_1008_19f6        @ 1008:19f6
 *   FUN_1008_1aa7        @ 1008:1aa7
 *   FUN_1008_1e95        @ 1008:1e95
 *   FUN_1008_1fa6        @ 1008:1fa6
 *   FUN_1008_1fc6        @ 1008:1fc6
 *   FUN_1008_229d        @ 1008:229d
 *   FUN_1008_3423        @ 1008:3423
 *   FUN_1008_35ac        @ 1008:35ac
 *   FUN_1008_38a0        @ 1008:38a0
 *   FUN_1008_38c8        @ 1008:38c8
 *   FUN_1008_3d8d        @ 1008:3d8d
 *   FUN_1008_3e0e        @ 1008:3e0e
 *   FUN_1008_3f81        @ 1008:3f81
 *   FUN_1008_3fa3        @ 1008:3fa3
 *   FUN_1008_422f        @ 1008:422f
 *   FUN_1008_4c3f        @ 1008:4c3f
 *   FUN_1008_50e3        @ 1008:50e3
 *   FUN_1008_51fd        @ 1008:51fd
 *   FUN_1008_5484        @ 1008:5484
 *   FUN_1008_57a7        @ 1008:57a7
 *   FUN_1008_5d6b        @ 1008:5d6b
 *   FUN_1008_5dc4        @ 1008:5dc4
 *   FUN_1008_5e10        @ 1008:5e10
 *   FUN_1008_5e91        @ 1008:5e91
 *   FUN_1008_5ebb        @ 1008:5ebb
 *   FUN_1008_61d8        @ 1008:61d8
 *   FUN_1008_65bd        @ 1008:65bd
 *   FUN_1008_65f9        @ 1008:65f9
 *   FUN_1008_67c5        @ 1008:67c5
 *   FUN_1008_687f        @ 1008:687f
 *   FUN_1008_6a95        @ 1008:6a95
 *   FUN_1008_6cc8        @ 1008:6cc8
 *   FUN_1008_6e18        @ 1008:6e18
 *   FUN_1008_6e82        @ 1008:6e82
 *   FUN_1008_6ead        @ 1008:6ead
 *   FUN_1008_6f2e        @ 1008:6f2e
 *   FUN_1008_6f68        @ 1008:6f68
 *   FUN_1008_6f8f        @ 1008:6f8f
 *   FUN_1008_7000        @ 1008:7000
 *   FUN_1008_7101        @ 1008:7101
 *   FUN_1008_7122        @ 1008:7122
 *   FUN_1008_719a        @ 1008:719a
 *   FUN_1008_71fc        @ 1008:71fc
 *   FUN_1008_7591        @ 1008:7591
 *   FUN_1008_79a2        @ 1008:79a2
 *   FUN_1008_79c7        @ 1008:79c7
 *   FUN_1008_79ec        @ 1008:79ec
 *   FUN_1008_7a28        @ 1008:7a28
 *   FUN_1008_7a49        @ 1008:7a49
 *   FUN_1008_7a91        @ 1008:7a91
 *   FUN_1008_7b09        @ 1008:7b09
 *   FUN_1008_7bd8        @ 1008:7bd8
 *   FUN_1008_7e1e        @ 1008:7e1e
 *   FUN_1008_7f62        @ 1008:7f62
 *   FUN_1008_7f83        @ 1008:7f83
 *   FUN_1008_7fa7        @ 1008:7fa7
 *   FUN_1008_7fde        @ 1008:7fde
 *   FUN_1008_807d        @ 1008:807d
 *   FUN_1008_8826        @ 1008:8826
 *   FUN_1008_88d9        @ 1008:88d9
 *   FUN_1008_9121        @ 1008:9121
 *   FUN_1008_91bd        @ 1008:91bd
 *   FUN_1008_963a        @ 1008:963a
 *   FUN_1008_a1da        @ 1008:a1da
 *   FUN_1068_0090        @ 1068:0090
 *   FUN_1068_0144        @ 1068:0144
 *   FUN_1068_020c        @ 1068:020c
 */

/* ======================================================== */
/* Function: FUN_1008_0002 @ 1008:0002 */
/* ======================================================== */

void FUN_1008_0002(void)

{
  DAT_10a8_1d2f = 1;
  FUN_1048_1432();
  FUN_1048_10c1(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1008_6752();
  FUN_1008_65f9();
  FUN_10a0_0b92(0x26,(undefined2 *)&DAT_10a8_22aa,(char *)s_RandomShrs_10a8_109f + 9,0x1cfe,
                (char *)s_RandomShrs_10a8_109f + 9);
  MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0x10);
  MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x10,0x203,(char *)s_RandomShrs_10a8_109f + 9,
             0x1de,(char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_0057 @ 1008:0057 */
/* ======================================================== */

undefined1 FUN_1008_0057(undefined2 param_1,int param_2)

{
  undefined1 local_3;
  
  if ((param_2 < 0) || (1 < param_2)) {
    DAT_10a8_1d2f = 1;
    FUN_1008_0002(param_1);
    local_3 = 0;
  }
  else {
    local_3 = 1;
  }
  return local_3;
}

/* ======================================================== */
/* Function: FUN_1008_0084 @ 1008:0084 */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x1008038f) */
/* WARNING: Removing unreachable block (ram,0x100803d0) */
/* WARNING: Removing unreachable block (ram,0x1008075a) */
/* WARNING: Removing unreachable block (ram,0x1008079b) */

void __stdcall16far FUN_1008_0084(int *param_1)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  int extraout_DX;
  int extraout_DX_00;
  int extraout_DX_01;
  undefined2 extraout_DX_02;
  undefined2 uVar8;
  undefined2 extraout_DX_03;
  int extraout_DX_04;
  int iVar9;
  int extraout_DX_05;
  int extraout_DX_06;
  undefined2 extraout_DX_07;
  int extraout_DX_08;
  int iVar10;
  undefined2 extraout_DX_09;
  undefined2 extraout_DX_10;
  undefined2 extraout_DX_11;
  undefined2 extraout_DX_12;
  undefined2 extraout_DX_13;
  undefined2 extraout_DX_14;
  undefined2 extraout_DX_15;
  int in_BX;
  int unaff_BP;
  int *piVar11;
  int iVar12;
  int iVar13;
  undefined2 uVar14;
  undefined2 unaff_SS;
  bool bVar15;
  undefined4 uVar16;
  undefined1 *puVar17;
  undefined2 uVar18;
  undefined2 uVar19;
  int *piVar20;
  char local_180;
  char local_17f;
  undefined1 local_17e;
  char local_17a;
  uint local_176;
  int local_174;
  uint local_172;
  int local_170;
  undefined1 local_16a [256];
  undefined1 local_6a [102];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  piVar11 = (int *)param_1;
  uVar14 = (undefined2)((ulong)param_1 >> 0x10);
  local_174 = in_BX;
  if (DAT_10a8_1d2f == '\0') {
    MESSAGEBEEP();
    iVar3 = MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x24,0x237,
                       (char *)s_RandomShrs_10a8_109f + 9,0x20e,(char *)s_RandomShrs_10a8_109f + 9);
    local_174 = in_BX;
    if (iVar3 == 7) {
      return;
    }
  }
  FUN_10a0_0b92(0x26,0x1cfe,(char *)s_RandomShrs_10a8_109f + 9,(undefined2 *)&DAT_10a8_22aa,
                (char *)s_RandomShrs_10a8_109f + 9);
  uVar19 = 0;
  uVar18 = 0x7fff;
  piVar20 = piVar11;
  uVar8 = uVar14;
  uVar16 = FUN_1098_0055(0x243,(char *)s_RandomShrs_10a8_109f + 9,local_6a,unaff_SS);
  uVar16 = FUN_1010_00fd(0,0,0x792,uVar16,uVar18,uVar19,piVar20,uVar8);
  iVar3 = (*(code *)*(undefined2 *)(*DAT_10a8_1b2e + 0x38))
                    ((char *)s_Oil_Baron_10a8_100a + 6,(int *)DAT_10a8_1b2e,
                     (int)((ulong)DAT_10a8_1b2e >> 0x10),uVar16);
  if (iVar3 != 1) {
    return;
  }
  FUN_10a0_049f(local_6a,unaff_SS,local_16a,unaff_SS);
  FUN_10a0_0527((char *)s_RandomShrs_10a8_109f + 1,local_16a);
  cVar1 = FUN_1008_19f6(piVar11,uVar14);
  if (cVar1 != '\0') {
    FUN_10a0_0586(local_16a,unaff_SS);
    return;
  }
  puVar17 = local_16a;
  iVar3 = extraout_DX;
  uVar8 = unaff_SS;
  uVar4 = FUN_10a0_092d(puVar17);
  FUN_10a0_076a(puVar17,uVar8);
  if ((byte)((char)uVar4 + 0x75U) < 3) {
    DAT_10a8_22cb = 0;
    DAT_10a8_22ca = 0;
    FUN_1048_10c1(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
    puVar17 = local_16a;
    iVar9 = extraout_DX_00;
    uVar8 = unaff_SS;
    DAT_10a8_22aa = FUN_10a0_092d(puVar17);
    DAT_10a8_22ac = iVar9;
    FUN_10a0_076a(puVar17,uVar8);
    if ((DAT_10a8_22ac < 1) && ((DAT_10a8_22ac < 0 || (DAT_10a8_22aa < 7)))) {
      puVar17 = local_16a;
      iVar9 = extraout_DX_01;
      uVar8 = unaff_SS;
      DAT_10a8_319a = FUN_10a0_092d(puVar17);
      DAT_10a8_319c = iVar9;
      FUN_10a0_076a(puVar17,uVar8);
      if ((DAT_10a8_319c < 1) &&
         ((((DAT_10a8_319c < 0 || (DAT_10a8_319a < 7)) && (-1 < DAT_10a8_319c)) &&
          ((0 < DAT_10a8_319c || (DAT_10a8_319a != 0)))))) {
        puVar17 = local_16a;
        uVar8 = extraout_DX_02;
        uVar18 = unaff_SS;
        DAT_10a8_319e = FUN_10a0_092d(puVar17);
        DAT_10a8_31a0 = uVar8;
        FUN_10a0_076a(puVar17,uVar18);
        puVar17 = local_16a;
        uVar8 = extraout_DX_03;
        uVar18 = unaff_SS;
        DAT_10a8_31a2 = FUN_10a0_092d(puVar17);
        DAT_10a8_31a4 = uVar8;
        FUN_10a0_076a(puVar17,uVar18);
        puVar17 = local_16a;
        iVar9 = extraout_DX_04;
        uVar8 = unaff_SS;
        DAT_10a8_22b2 = FUN_10a0_092d(puVar17);
        DAT_10a8_22b4 = iVar9;
        FUN_10a0_076a(puVar17,uVar8);
        if ((DAT_10a8_22b4 < 1) && ((DAT_10a8_22b4 < 0 || (DAT_10a8_22b2 < 0x1b)))) {
          DAT_10a8_38b8 = DAT_10a8_22b2 + 3;
          iVar9 = DAT_10a8_22b4 + (uint)(0xfffc < DAT_10a8_22b2);
          puVar17 = local_16a;
          DAT_10a8_38ba = iVar9;
          uVar8 = unaff_SS;
          DAT_10a8_22b6 = FUN_10a0_092d(puVar17);
          DAT_10a8_22b8 = iVar9;
          FUN_10a0_076a(puVar17,uVar8);
          if ((DAT_10a8_22b8 < 1) && ((DAT_10a8_22b8 < 0 || (DAT_10a8_22b6 < 0x1b)))) {
            DAT_10a8_38bc = DAT_10a8_22b6 + 2;
            iVar9 = DAT_10a8_22b8 + (uint)(0xfffd < DAT_10a8_22b6);
            puVar17 = local_16a;
            DAT_10a8_38be = iVar9;
            uVar8 = unaff_SS;
            DAT_10a8_22ae = FUN_10a0_092d(puVar17);
            DAT_10a8_22b0 = iVar9;
            FUN_10a0_076a(puVar17,uVar8);
            if (((DAT_10a8_22b0 < 0) ||
                (((DAT_10a8_22b0 < 1 && (DAT_10a8_22ae < 3)) || (0 < DAT_10a8_22b0)))) ||
               ((-1 < DAT_10a8_22b0 && (0xe < DAT_10a8_22ae)))) {
              FUN_1008_0002(&iStack_2);
            }
            else {
              puVar17 = local_16a;
              iVar9 = extraout_DX_05;
              uVar8 = unaff_SS;
              DAT_10a8_22ba = FUN_10a0_092d(puVar17);
              DAT_10a8_22bc = iVar9;
              FUN_10a0_076a(puVar17,uVar8);
              if (DAT_10a8_22bc < 0) {
                FUN_1008_0002(&iStack_2);
              }
              else {
                puVar17 = local_16a;
                iVar9 = extraout_DX_06;
                uVar8 = unaff_SS;
                DAT_10a8_22be = FUN_10a0_092d(puVar17);
                DAT_10a8_22c0 = iVar9;
                FUN_10a0_076a(puVar17,uVar8);
                if ((DAT_10a8_22c0 < 0) || ((DAT_10a8_22c0 < 1 && (DAT_10a8_22be < 0xfa)))) {
                  FUN_1008_0002(&iStack_2);
                }
                else {
                  uVar5 = DAT_10a8_22b2 - 1;
                  iVar9 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
                  if (-1 < iVar9) {
                    local_172 = 0;
                    local_170 = 0;
                    iVar10 = local_174;
                    while( true ) {
                      uVar7 = DAT_10a8_22b6 - 1;
                      iVar12 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
                      if (-1 < iVar12) {
                        local_176 = 0;
                        local_174 = 0;
                        iVar13 = 0;
                        while( true ) {
                          puVar17 = local_16a;
                          uVar8 = unaff_SS;
                          uVar6 = FUN_10a0_092d(puVar17);
                          FUN_10a0_076a(puVar17,uVar8);
                          if (((iVar13 < 0) || (0 < iVar13)) || ((-1 < iVar13 && (9 < uVar6)))) {
                            FUN_1008_0002(&iStack_2);
                            goto LAB_1008_0e76;
                          }
                          local_17a = (char)uVar6;
                          *(undefined1 *)(local_172 * 0x8c + local_176 * 5 + 0x22d2) = local_17a;
                          if ((local_174 == iVar12) && (local_176 == uVar7)) break;
                          bVar15 = 0xfffe < local_176;
                          local_176 = local_176 + 1;
                          iVar13 = local_174;
                          local_174 = local_174 + (uint)bVar15;
                        }
                      }
                      FUN_10a0_076a(local_16a,unaff_SS);
                      if ((local_170 == iVar9) && (local_174 = iVar10, local_172 == uVar5)) break;
                      bVar15 = 0xfffe < local_172;
                      local_172 = local_172 + 1;
                      local_170 = local_170 + (uint)bVar15;
                    }
                  }
                  iVar9 = DAT_10a8_22ac;
                  uVar5 = DAT_10a8_22aa;
                  if ((DAT_10a8_22ac < 0) || ((DAT_10a8_22ac < 1 && (DAT_10a8_22aa == 0)))) {
LAB_1008_0730:
                    uVar5 = DAT_10a8_22b2 - 1;
                    iVar9 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
                    local_170 = 0;
                    if (-1 < iVar9) {
                      local_172 = 0;
                      local_170 = 0;
                      while( true ) {
                        uVar7 = DAT_10a8_22b6 - 1;
                        iVar10 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
                        if (-1 < iVar10) {
                          local_176 = 0;
                          local_174 = 0;
                          iVar12 = 0;
                          while( true ) {
                            puVar17 = local_16a;
                            uVar8 = unaff_SS;
                            uVar6 = FUN_10a0_092d(puVar17);
                            FUN_10a0_076a(puVar17,uVar8);
                            puVar17 = local_16a;
                            uVar8 = unaff_SS;
                            uVar18 = FUN_10a0_092d(puVar17);
                            FUN_10a0_076a(puVar17,uVar8);
                            cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                            if (cVar1 == '\0') goto LAB_1008_0e76;
                            if (((iVar12 < 0) || (0 < iVar12)) || ((-1 < iVar12 && (2 < uVar6)))) {
                              FUN_1008_0002(&iStack_2);
                              goto LAB_1008_0e76;
                            }
                            local_17a = (char)uVar6;
                            ((char *)s_American_Natural_Gas_Co__10a8_193e + 2)
                            [(uint)(byte)((char)local_172 + 0x41) * 0x1a + local_176] = local_17a;
                            local_17e = (undefined1)uVar18;
                            *(undefined1 *)
                             ((uint)(byte)((char)local_172 + 0x41) * 0x1a + local_176 + 0x169c) =
                                 local_17e;
                            if ((local_174 == iVar10) && (local_176 == uVar7)) break;
                            bVar15 = 0xfffe < local_176;
                            local_176 = local_176 + 1;
                            iVar12 = local_174;
                            local_174 = local_174 + (uint)bVar15;
                          }
                        }
                        if ((local_170 == iVar9) && (local_172 == uVar5)) break;
                        bVar15 = 0xfffe < local_172;
                        local_172 = local_172 + 1;
                        local_170 = local_170 + (uint)bVar15;
                      }
                    }
                    puVar17 = local_16a;
                    uVar8 = unaff_SS;
                    DAT_10a8_1d32 = FUN_10a0_092d(puVar17);
                    DAT_10a8_1d34 = local_170;
                    FUN_10a0_076a(puVar17,uVar8);
                    if (((DAT_10a8_1d34 < 0) ||
                        (iVar10 = DAT_10a8_22b4, iVar12 = DAT_10a8_22b8, uVar7 = FUN_10a0_0baa(),
                        iVar9 = DAT_10a8_22ac, uVar5 = DAT_10a8_22aa, iVar10 < DAT_10a8_1d34)) ||
                       ((iVar10 <= DAT_10a8_1d34 && (uVar7 < DAT_10a8_1d32)))) {
                      FUN_1008_0002(&iStack_2);
                    }
                    else if ((DAT_10a8_22ac < 0) || ((DAT_10a8_22ac < 1 && (DAT_10a8_22aa == 0)))) {
LAB_1008_09ba:
                      FUN_10a0_076a(local_16a,unaff_SS);
                      if ((iVar3 == 0) && (uVar4 == 0x568b)) {
                        puVar17 = local_16a;
                        uVar8 = unaff_SS;
                        uVar18 = FUN_10a0_092d(puVar17);
                        FUN_10a0_076a(puVar17,uVar8);
                        cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                        if (cVar1 == '\0') goto LAB_1008_0e76;
                        local_17a = (char)uVar18;
                        if (local_17a == '\0') {
                          DAT_10a8_22be = 0x9680;
                          DAT_10a8_22c0 = 0x98;
                        }
                      }
                      else if ((0x98 < DAT_10a8_22c0) ||
                              ((0x97 < DAT_10a8_22c0 && (0x9680 < DAT_10a8_22be)))) {
                        FUN_1008_0002(&iStack_2);
                        goto LAB_1008_0e76;
                      }
                      puVar17 = local_16a;
                      uVar8 = unaff_SS;
                      uVar18 = FUN_10a0_092d(puVar17);
                      FUN_10a0_076a(puVar17,uVar8);
                      cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                      if (cVar1 != '\0') {
                        local_17a = (char)uVar18;
                        DAT_10a8_22c6 = local_17a;
                        puVar17 = local_16a;
                        uVar8 = unaff_SS;
                        uVar18 = FUN_10a0_092d(puVar17);
                        FUN_10a0_076a(puVar17,uVar8);
                        cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                        if (cVar1 != '\0') {
                          local_17a = (char)uVar18;
                          DAT_10a8_22c7 = local_17a;
                          puVar17 = local_16a;
                          uVar8 = unaff_SS;
                          uVar18 = FUN_10a0_092d(puVar17);
                          FUN_10a0_076a(puVar17,uVar8);
                          cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                          if (cVar1 != '\0') {
                            local_17a = (char)uVar18;
                            DAT_10a8_1d2f = local_17a;
                            puVar17 = local_16a;
                            uVar8 = unaff_SS;
                            uVar18 = FUN_10a0_092d(puVar17);
                            FUN_10a0_076a(puVar17,uVar8);
                            cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                            if (cVar1 != '\0') {
                              local_17a = (char)uVar18;
                              DAT_10a8_1d30 = local_17a;
                              puVar17 = local_16a;
                              uVar8 = unaff_SS;
                              uVar18 = FUN_10a0_092d(puVar17);
                              FUN_10a0_076a(puVar17,uVar8);
                              cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                              if (cVar1 != '\0') {
                                local_17a = (char)uVar18;
                                *(char *)(piVar11 + 0x24) = local_17a;
                                puVar17 = local_16a;
                                uVar8 = unaff_SS;
                                uVar18 = FUN_10a0_092d(puVar17);
                                FUN_10a0_076a(puVar17,uVar8);
                                cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                if (cVar1 != '\0') {
                                  puVar17 = local_16a;
                                  uVar8 = unaff_SS;
                                  uVar18 = FUN_10a0_092d(puVar17);
                                  FUN_10a0_076a(puVar17,uVar8);
                                  cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                  if (cVar1 != '\0') {
                                    local_17a = (char)uVar18;
                                    DAT_10a8_1d31 = local_17a;
                                    puVar17 = local_16a;
                                    uVar8 = unaff_SS;
                                    uVar18 = FUN_10a0_092d(puVar17);
                                    FUN_10a0_076a(puVar17,uVar8);
                                    cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                    if (cVar1 != '\0') {
                                      local_17a = (char)uVar18;
                                      DAT_10a8_22c9 = local_17a;
                                      puVar17 = local_16a;
                                      uVar8 = unaff_SS;
                                      uVar18 = FUN_10a0_092d(puVar17);
                                      FUN_10a0_076a(puVar17,uVar8);
                                      cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                      if (cVar1 != '\0') {
                                        local_17a = (char)uVar18;
                                        DAT_10a8_1d2e = local_17a;
                                        puVar17 = local_16a;
                                        uVar8 = unaff_SS;
                                        uVar18 = FUN_10a0_092d(puVar17);
                                        FUN_10a0_076a(puVar17,uVar8);
                                        cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                        if (cVar1 != '\0') {
                                          local_17a = (char)uVar18;
                                          DAT_10a8_22cf = local_17a;
                                          puVar17 = local_16a;
                                          uVar8 = unaff_SS;
                                          uVar18 = FUN_10a0_092d(puVar17);
                                          FUN_10a0_076a(puVar17,uVar8);
                                          cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                          if (cVar1 != '\0') {
                                            local_17a = (char)uVar18;
                                            DAT_10a8_39a2 = local_17a;
                                            puVar17 = local_16a;
                                            uVar8 = unaff_SS;
                                            uVar18 = FUN_10a0_092d(puVar17);
                                            FUN_10a0_076a(puVar17,uVar8);
                                            cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                            if (cVar1 != '\0') {
                                              local_17a = (char)uVar18;
                                              DAT_10a8_39a8 = local_17a;
                                              if ((iVar3 < 1) && ((iVar3 < 0 || (uVar4 < 0x568c))))
                                              {
                                                DAT_10a8_22cb = 0;
                                                DAT_10a8_22ca = 0;
                                              }
                                              else {
                                                puVar17 = local_16a;
                                                uVar8 = unaff_SS;
                                                uVar18 = FUN_10a0_092d(puVar17);
                                                FUN_10a0_076a(puVar17,uVar8);
                                                cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                                if (cVar1 == '\0') goto LAB_1008_0e76;
                                                local_17a = (char)uVar18;
                                                DAT_10a8_22cb = local_17a;
                                                puVar17 = local_16a;
                                                uVar8 = unaff_SS;
                                                uVar18 = FUN_10a0_092d(puVar17);
                                                FUN_10a0_076a(puVar17,uVar8);
                                                cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                                if (cVar1 == '\0') goto LAB_1008_0e76;
                                                local_17a = (char)uVar18;
                                                DAT_10a8_22ca = local_17a;
                                                local_180 = '\x01';
                                                uVar8 = extraout_DX_09;
                                                while( true ) {
                                                  iVar9 = local_180 * 0x24;
                                                  puVar17 = local_16a;
                                                  uVar18 = unaff_SS;
                                                  uVar19 = FUN_10a0_09c3(puVar17);
                                                  *(undefined2 *)(iVar9 + 0x3778) = uVar19;
                                                  *(int *)(iVar9 + 0x377a) = iVar12;
                                                  *(undefined2 *)(iVar9 + 0x377c) = uVar8;
                                                  FUN_10a0_076a(puVar17,uVar18);
                                                  puVar17 = local_16a;
                                                  uVar8 = extraout_DX_10;
                                                  uVar18 = unaff_SS;
                                                  uVar19 = FUN_10a0_09c3(puVar17);
                                                  *(undefined2 *)(iVar9 + 0x377e) = uVar19;
                                                  *(int *)(iVar9 + 0x3780) = iVar12;
                                                  *(undefined2 *)(iVar9 + 0x3782) = uVar8;
                                                  FUN_10a0_076a(puVar17,uVar18);
                                                  puVar17 = local_16a;
                                                  uVar8 = extraout_DX_11;
                                                  uVar18 = unaff_SS;
                                                  uVar19 = FUN_10a0_09c3(puVar17);
                                                  *(undefined2 *)(iVar9 + 0x3784) = uVar19;
                                                  *(int *)(iVar9 + 0x3786) = iVar12;
                                                  *(undefined2 *)(iVar9 + 0x3788) = uVar8;
                                                  FUN_10a0_076a(puVar17,uVar18);
                                                  puVar17 = local_16a;
                                                  uVar8 = extraout_DX_12;
                                                  uVar18 = unaff_SS;
                                                  uVar19 = FUN_10a0_09c3(puVar17);
                                                  *(undefined2 *)(iVar9 + 0x378a) = uVar19;
                                                  *(int *)(iVar9 + 0x378c) = iVar12;
                                                  ((undefined2 *)&DAT_10a8_378e)[local_180 * 0x12] =
                                                       uVar8;
                                                  FUN_10a0_076a(puVar17,uVar18);
                                                  if (local_180 == '\b') break;
                                                  local_180 = local_180 + '\x01';
                                                  uVar8 = extraout_DX_13;
                                                }
                                              }
                                              if ((iVar3 == 0) && (uVar4 == 0x568d)) {
                                                puVar17 = local_16a;
                                                uVar8 = unaff_SS;
                                                uVar18 = FUN_10a0_092d(puVar17);
                                                FUN_10a0_076a(puVar17,uVar8);
                                                cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                                if (cVar1 == '\0') goto LAB_1008_0e76;
                                                local_17a = (char)uVar18;
                                                DAT_10a8_22cc = local_17a;
                                                puVar17 = local_16a;
                                                uVar8 = unaff_SS;
                                                uVar18 = FUN_10a0_092d(puVar17);
                                                FUN_10a0_076a(puVar17,uVar8);
                                                cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                                                if (cVar1 == '\0') goto LAB_1008_0e76;
                                                local_17a = (char)uVar18;
                                                DAT_10a8_22cd = local_17a;
                                                local_17f = '\x01';
                                                uVar8 = extraout_DX_14;
                                                while( true ) {
                                                  puVar17 = local_16a;
                                                  uVar18 = unaff_SS;
                                                  uVar19 = FUN_10a0_092d(puVar17);
                                                  ((undefined2 *)&DAT_10a8_3ada)[local_17f * 2] =
                                                       uVar19;
                                                  ((undefined2 *)&DAT_10a8_3adc)[local_17f * 2] =
                                                       uVar8;
                                                  FUN_10a0_076a(puVar17,uVar18);
                                                  if (local_17f == '\b') break;
                                                  local_17f = local_17f + '\x01';
                                                  uVar8 = extraout_DX_15;
                                                }
                                                puVar17 = local_16a;
                                                uVar8 = extraout_DX_15;
                                                uVar18 = unaff_SS;
                                                DAT_10a8_22c2 = FUN_10a0_092d(puVar17);
                                                DAT_10a8_22c4 = uVar8;
                                                FUN_10a0_076a(puVar17,uVar18);
                                              }
                                              else {
                                                DAT_10a8_22cc = 0;
                                                DAT_10a8_22cd = 0;
                                              }
                                              cVar1 = FUN_1008_19f6(piVar11,uVar14);
                                              if (cVar1 == '\0') {
                                                FUN_10a0_0b92(0x26,0x1cfe,
                                                              (char *)s_RandomShrs_10a8_109f + 9,
                                                              (undefined2 *)&DAT_10a8_22aa,
                                                              (char *)s_RandomShrs_10a8_109f + 9);
                                              }
                                              else {
                                                DAT_10a8_1d2f = '\x01';
                                                FUN_1008_0002(&iStack_2);
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                    else {
                      local_172 = 1;
                      local_170 = 0;
                      iVar10 = 0;
                      while( true ) {
                        puVar17 = local_16a;
                        uVar8 = unaff_SS;
                        uVar7 = FUN_10a0_092d(puVar17);
                        FUN_10a0_076a(puVar17,uVar8);
                        if (((iVar10 < 0) || (0 < iVar10)) || ((-1 < iVar10 && (3 < uVar7)))) break;
                        local_17a = (char)uVar7;
                        *(char *)((undefined2 *)&DAT_10a8_38bc + local_172 * 0x10) = local_17a;
                        FUN_10a0_0b92(0x1f,local_172 * 0x20 + 0x38bd,
                                      (char *)s_RandomShrs_10a8_109f + 9,local_172 * 0xfc + 0x30aa,
                                      (char *)s_RandomShrs_10a8_109f + 9);
                        if ((local_170 == iVar9) && (local_172 == uVar5)) goto LAB_1008_09ba;
                        bVar15 = 0xfffe < local_172;
                        local_172 = local_172 + 1;
                        iVar10 = local_170;
                        local_170 = local_170 + (uint)bVar15;
                      }
                      FUN_1008_0002(&iStack_2);
                    }
                  }
                  else {
                    local_172 = 1;
                    local_170 = 0;
                    while( true ) {
                      iVar10 = local_172 * 0xfc;
                      iVar12 = iVar10 + 0x30aa;
                      puVar17 = local_16a;
                      uVar8 = unaff_SS;
                      FUN_10a0_08ac(0x1e,iVar12,(char *)s_RandomShrs_10a8_109f + 9);
                      FUN_10a0_076a(puVar17,uVar8);
                      puVar17 = local_16a;
                      uVar8 = unaff_SS;
                      uVar18 = FUN_10a0_092d(puVar17);
                      FUN_10a0_076a(puVar17,uVar8);
                      cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                      if (cVar1 == '\0') break;
                      local_17a = (char)uVar18;
                      *(undefined1 *)(iVar10 + 0x30c9) = local_17a;
                      puVar17 = local_16a;
                      uVar8 = extraout_DX_07;
                      uVar18 = unaff_SS;
                      uVar19 = FUN_10a0_09c3(puVar17);
                      *(undefined2 *)(iVar10 + 0x30ca) = uVar19;
                      *(int *)(iVar10 + 0x30cc) = local_174;
                      *(undefined2 *)(iVar10 + 0x30ce) = uVar8;
                      FUN_10a0_076a(puVar17,uVar18);
                      iVar10 = DAT_10a8_22b0;
                      uVar7 = DAT_10a8_22ae;
                      local_174 = 0;
                      if ((-1 < DAT_10a8_22b0) && ((0 < DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
                        local_176 = 1;
                        local_174 = 0;
                        while( true ) {
                          puVar17 = local_16a;
                          uVar8 = unaff_SS;
                          uVar2 = FUN_10a0_0811(puVar17);
                          *(undefined1 *)(iVar12 + local_176 * 0xd + 0x19) = uVar2;
                          FUN_10a0_076a(puVar17,uVar8);
                          puVar17 = local_16a;
                          uVar8 = unaff_SS;
                          uVar18 = FUN_10a0_092d(puVar17);
                          *(undefined2 *)(iVar12 + local_176 * 0xd + 0x1a) = uVar18;
                          FUN_10a0_076a(puVar17,uVar8);
                          *(undefined2 *)(iVar12 + local_176 * 0xd + 0x1c) = 2;
                          puVar17 = local_16a;
                          uVar8 = unaff_SS;
                          uVar18 = FUN_10a0_092d(puVar17);
                          FUN_10a0_076a(puVar17,uVar8);
                          cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                          if (cVar1 == '\0') goto LAB_1008_0e76;
                          local_17a = (char)uVar18;
                          *(undefined1 *)(iVar12 + local_176 * 0xd + 0x1e) = local_17a;
                          puVar17 = local_16a;
                          uVar8 = unaff_SS;
                          uVar18 = FUN_10a0_092d(puVar17);
                          FUN_10a0_076a(puVar17,uVar8);
                          cVar1 = FUN_1008_0057(&iStack_2,uVar18);
                          if (cVar1 == '\0') goto LAB_1008_0e76;
                          local_17a = (char)uVar18;
                          *(undefined1 *)(iVar12 + local_176 * 0xd + 0x1f) = local_17a;
                          if ((local_174 == iVar10) && (local_176 == uVar7)) break;
                          bVar15 = 0xfffe < local_176;
                          local_176 = local_176 + 1;
                          local_174 = local_174 + (uint)bVar15;
                        }
                      }
                      local_17f = '\x01';
                      while( true ) {
                        puVar17 = local_16a;
                        uVar8 = unaff_SS;
                        uVar18 = FUN_10a0_092d(puVar17);
                        iVar10 = iVar12 + local_17f * 4;
                        *(undefined2 *)(iVar10 + 0xd8) = uVar18;
                        *(int *)(iVar10 + 0xda) = local_174;
                        FUN_10a0_076a(puVar17,uVar8);
                        iVar13 = iVar12 + local_17f * 4;
                        iVar10 = *(int *)(iVar13 + 0xda);
                        if ((0x98 < iVar10) ||
                           ((0x97 < iVar10 && (0x9680 < *(uint *)(iVar13 + 0xd8))))) {
                          iVar3 = piVar11[2];
                          MESSAGEBOX((char *)s_RandomShrs_10a8_109f + 1,0,0x2b3,
                                     (char *)s_RandomShrs_10a8_109f + 9,0x278,
                                     (char *)s_RandomShrs_10a8_109f + 9);
                          FUN_1008_0002(&iStack_2,iVar3);
                          goto LAB_1008_0e76;
                        }
                        if (local_17f == '\b') break;
                        local_17f = local_17f + '\x01';
                        local_174 = extraout_DX_08;
                      }
                      local_17f = '\x01';
                      while( true ) {
                        if (*(int *)(iVar12 + local_17f * 4 + 0xda) < 0) {
                          FUN_1008_0002(&iStack_2);
                          goto LAB_1008_0e76;
                        }
                        if (local_17f == '\b') break;
                        local_17f = local_17f + '\x01';
                      }
                      FUN_10a0_076a(local_16a,unaff_SS);
                      if ((local_170 == iVar9) && (local_172 == uVar5)) goto LAB_1008_0730;
                      bVar15 = 0xfffe < local_172;
                      local_172 = local_172 + 1;
                      local_170 = local_170 + (uint)bVar15;
                    }
                  }
                }
              }
            }
          }
          else {
            FUN_1008_0002(&iStack_2);
          }
        }
        else {
          FUN_1008_0002(&iStack_2);
        }
      }
      else {
        FUN_1008_0002(&iStack_2);
      }
    }
    else {
      FUN_1008_0002(&iStack_2);
    }
  }
  else {
    MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0x30);
    MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x268,(char *)s_RandomShrs_10a8_109f + 9,
               0x249,(char *)s_RandomShrs_10a8_109f + 9);
    FUN_10a0_0586(local_16a,unaff_SS);
  }
LAB_1008_0e76:
  FUN_10a0_0586(local_16a,unaff_SS);
  FUN_1008_4c3f(piVar11,uVar14);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s_FALSE_10a8_1003 + 5,piVar11,uVar14,CONCAT11(extraout_AH,DAT_10a8_39a8));
  (*(code *)*(undefined2 *)(*param_1 + 0x58))((char *)s_FALSE_10a8_1003 + 5,piVar11,uVar14);
  INVALIDATERECT((char *)s_FALSE_10a8_1003 + 5,1,0,0,piVar11[2]);
  FUN_1028_0002(CONCAT11(extraout_AH_00,DAT_10a8_39a7),1,DAT_10a8_22aa,0x31a6,
                (char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_0ed2 @ 1008:0ed2 */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10081402) */
/* WARNING: Removing unreachable block (ram,0x100810b5) */
/* WARNING: Removing unreachable block (ram,0x100813c1) */

void __stdcall16far FUN_1008_0ed2(undefined4 param_1)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int unaff_BP;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 unaff_SS;
  bool bVar10;
  undefined4 uVar11;
  undefined1 *puVar12;
  undefined2 uVar13;
  undefined2 uVar14;
  undefined2 uVar15;
  undefined1 local_204 [2];
  int local_202;
  char local_17b;
  int local_17a;
  int local_178;
  int local_176;
  int local_174;
  uint local_172;
  uint local_170;
  uint local_16e;
  uint local_16c;
  undefined1 local_16a [256];
  undefined1 local_6a [102];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar9 = (undefined2)((ulong)param_1 >> 0x10);
  iVar7 = (int)param_1;
  uVar15 = 0;
  uVar14 = 0x7ffe;
  iVar2 = iVar7;
  uVar13 = uVar9;
  uVar11 = FUN_1098_0055(0x2c2,(char *)s_RandomShrs_10a8_109f + 9,local_6a,unaff_SS);
  uVar11 = FUN_1010_00fd(0,0,0x792,uVar11,uVar14,uVar15,iVar2,uVar13);
  iVar2 = (*(code *)*(undefined2 *)(*DAT_10a8_1b2e + 0x38))
                    ((char *)s_Oil_Baron_10a8_100a + 6,(int *)DAT_10a8_1b2e,
                     (int)((ulong)DAT_10a8_1b2e >> 0x10),uVar11);
  if ((iVar2 == 1) &&
     ((OPENFILE((char *)s_Oil_Baron_10a8_100a + 6,0x4000,local_204,unaff_SS,local_6a),
      local_202 != 0 ||
      (iVar2 = MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x34,0x2e1,
                          (char *)s_RandomShrs_10a8_109f + 9,0x2c8,
                          (char *)s_RandomShrs_10a8_109f + 9), iVar2 == 6)))) {
    FUN_10a0_049f(local_6a,unaff_SS,local_16a,unaff_SS);
    FUN_10a0_052c((char *)s_RandomShrs_10a8_109f + 1,local_16a);
    cVar1 = FUN_1008_19f6(iVar7,uVar9);
    if (cVar1 == '\0') {
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,0x568d,0);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_22aa,DAT_10a8_22ac);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_319a,DAT_10a8_319c);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_319e,DAT_10a8_31a0);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_31a2,DAT_10a8_31a4);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_22b2,DAT_10a8_22b4);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_22b6,DAT_10a8_22b8);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_22ae,DAT_10a8_22b0);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_22ba,DAT_10a8_22bc);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_22be,DAT_10a8_22c0);
      FUN_10a0_07af(puVar12,uVar13);
      uVar3 = DAT_10a8_22b2 - 1;
      uVar5 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
      if (-1 < (int)uVar5) {
        local_16e = 0;
        local_16c = 0;
        while( true ) {
          uVar4 = DAT_10a8_22b6 - 1;
          uVar6 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
          if (uVar6 < 0x8000) {
            local_172 = 0;
            local_170 = 0;
            while( true ) {
              local_176 = (int)*(char *)(local_16e * 0x8c + local_172 * 5 + 0x22d2);
              local_174 = local_176 >> 0xf;
              puVar12 = local_16a;
              uVar13 = unaff_SS;
              FUN_10a0_098c(0,local_176,local_174);
              FUN_10a0_07af(puVar12,uVar13);
              if ((local_170 == uVar6) && (local_172 == uVar4)) break;
              bVar10 = 0xfffe < local_172;
              local_172 = local_172 + 1;
              local_170 = local_170 + bVar10;
            }
          }
          FUN_10a0_07af(local_16a,unaff_SS);
          if ((local_16c == uVar5) && (local_16e == uVar3)) break;
          bVar10 = 0xfffe < local_16e;
          local_16e = local_16e + 1;
          local_16c = local_16c + bVar10;
        }
      }
      uVar5 = DAT_10a8_22ac;
      uVar3 = DAT_10a8_22aa;
      if ((-1 < (int)DAT_10a8_22ac) && ((0 < (int)DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
        local_16e = 1;
        local_16c = 0;
        while( true ) {
          iVar2 = local_16e * 0xfc;
          iVar8 = iVar2 + 0x30aa;
          puVar12 = local_16a;
          uVar13 = unaff_SS;
          FUN_10a0_08e9(0,iVar8,(char *)s_RandomShrs_10a8_109f + 9);
          FUN_10a0_07af(puVar12,uVar13);
          local_176 = (int)*(char *)(iVar2 + 0x30c9);
          local_174 = local_176 >> 0xf;
          puVar12 = local_16a;
          uVar13 = unaff_SS;
          FUN_10a0_098c(0,local_176,local_174);
          FUN_10a0_07af(puVar12,uVar13);
          puVar12 = local_16a;
          uVar13 = unaff_SS;
          FUN_10a0_0a24(0xffff,0x11,*(undefined2 *)(iVar2 + 0x30ca),*(undefined2 *)(iVar2 + 0x30cc),
                        *(undefined2 *)(iVar2 + 0x30ce));
          FUN_10a0_07af(puVar12,uVar13);
          uVar6 = DAT_10a8_22b0;
          uVar4 = DAT_10a8_22ae;
          if ((-1 < (int)DAT_10a8_22b0) && ((0 < (int)DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
            local_172 = 1;
            local_170 = 0;
            while( true ) {
              puVar12 = local_16a;
              uVar13 = unaff_SS;
              FUN_10a0_085c(0,CONCAT11((char)(local_172 * 0xd >> 8),
                                       *(undefined1 *)(iVar8 + local_172 * 0xd + 0x19)));
              FUN_10a0_07af(puVar12,uVar13);
              puVar12 = local_16a;
              iVar2 = *(int *)(iVar8 + local_172 * 0xd + 0x1a);
              uVar13 = unaff_SS;
              FUN_10a0_098c(0,iVar2,iVar2 >> 0xf);
              FUN_10a0_07af(puVar12,uVar13);
              local_176 = (int)*(char *)(iVar8 + local_172 * 0xd + 0x1e);
              local_174 = local_176 >> 0xf;
              puVar12 = local_16a;
              uVar13 = unaff_SS;
              FUN_10a0_098c(0,local_176,local_174);
              FUN_10a0_07af(puVar12,uVar13);
              local_176 = (int)*(char *)(iVar8 + local_172 * 0xd + 0x1f);
              local_174 = local_176 >> 0xf;
              puVar12 = local_16a;
              uVar13 = unaff_SS;
              FUN_10a0_098c(0,local_176,local_174);
              FUN_10a0_07af(puVar12,uVar13);
              if ((local_170 == uVar6) && (local_172 == uVar4)) break;
              bVar10 = 0xfffe < local_172;
              local_172 = local_172 + 1;
              local_170 = local_170 + bVar10;
            }
          }
          local_17b = '\x01';
          while( true ) {
            puVar12 = local_16a;
            iVar2 = iVar8 + local_17b * 4;
            uVar13 = unaff_SS;
            FUN_10a0_098c(0,*(undefined2 *)(iVar2 + 0xd8),*(undefined2 *)(iVar2 + 0xda));
            FUN_10a0_07af(puVar12,uVar13);
            if (local_17b == '\b') break;
            local_17b = local_17b + '\x01';
          }
          FUN_10a0_07af(local_16a,unaff_SS);
          if ((local_16c == uVar5) && (local_16e == uVar3)) break;
          bVar10 = 0xfffe < local_16e;
          local_16e = local_16e + 1;
          local_16c = local_16c + bVar10;
        }
      }
      uVar3 = DAT_10a8_22b2 - 1;
      uVar5 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
      if (-1 < (int)uVar5) {
        local_16e = 0;
        local_16c = 0;
        while( true ) {
          uVar4 = DAT_10a8_22b6 - 1;
          uVar6 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
          if (-1 < (int)uVar6) {
            local_172 = 0;
            local_170 = 0;
            while( true ) {
              local_176 = (int)((char *)s_American_Natural_Gas_Co__10a8_193e + 2)
                               [(uint)(byte)((char)local_16e + 0x41) * 0x1a + local_172];
              local_174 = local_176 >> 0xf;
              local_17a = (int)*(char *)((uint)(byte)((char)local_16e + 0x41) * 0x1a + local_172 +
                                        0x169c);
              local_178 = local_17a >> 0xf;
              puVar12 = local_16a;
              uVar13 = unaff_SS;
              FUN_10a0_098c(0,local_176,local_174);
              FUN_10a0_07af(puVar12,uVar13);
              puVar12 = local_16a;
              uVar13 = unaff_SS;
              FUN_10a0_098c(0,local_17a,local_178);
              FUN_10a0_07af(puVar12,uVar13);
              if ((local_170 == uVar6) && (local_172 == uVar4)) break;
              bVar10 = 0xfffe < local_172;
              local_172 = local_172 + 1;
              local_170 = local_170 + bVar10;
            }
          }
          if ((local_16c == uVar5) && (local_16e == uVar3)) break;
          bVar10 = 0xfffe < local_16e;
          local_16e = local_16e + 1;
          local_16c = local_16c + bVar10;
        }
      }
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_1d32,DAT_10a8_1d34);
      FUN_10a0_07af(puVar12,uVar13);
      uVar5 = DAT_10a8_22ac;
      uVar3 = DAT_10a8_22aa;
      if ((DAT_10a8_22ac < 0x8000) && ((0 < (int)DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
        local_16e = 1;
        local_16c = 0;
        while( true ) {
          local_176 = (int)*(char *)((undefined2 *)&DAT_10a8_38bc + local_16e * 0x10);
          local_174 = local_176 >> 0xf;
          puVar12 = local_16a;
          uVar13 = unaff_SS;
          FUN_10a0_098c(0,local_176,local_174);
          FUN_10a0_07af(puVar12,uVar13);
          if ((local_16c == uVar5) && (local_16e == uVar3)) break;
          bVar10 = 0xfffe < local_16e;
          local_16e = local_16e + 1;
          local_16c = local_16c + bVar10;
        }
      }
      FUN_10a0_07af(local_16a,unaff_SS);
      local_176 = (int)DAT_10a8_22c6;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_22c7;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_1d2f;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_1d30;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)*(char *)(iVar7 + 0x48);
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_1d31;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_22c9;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_1d2e;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_22cf;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_39a2;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_39a8;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_22cb;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_22ca;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_17b = '\x01';
      while( true ) {
        iVar2 = (int)local_17b;
        iVar8 = iVar2 * 0x24;
        puVar12 = local_16a;
        uVar13 = unaff_SS;
        FUN_10a0_0a24(0xffff,0x11,*(undefined2 *)(iVar8 + 0x3778),*(undefined2 *)(iVar8 + 0x377a),
                      *(undefined2 *)(iVar8 + 0x377c));
        FUN_10a0_07af(puVar12,uVar13);
        puVar12 = local_16a;
        uVar13 = unaff_SS;
        FUN_10a0_0a24(0xffff,0x11,*(undefined2 *)(iVar8 + 0x377e),*(undefined2 *)(iVar8 + 0x3780),
                      *(undefined2 *)(iVar8 + 0x3782));
        FUN_10a0_07af(puVar12,uVar13);
        puVar12 = local_16a;
        uVar13 = unaff_SS;
        FUN_10a0_0a24(0xffff,0x11,*(undefined2 *)(iVar8 + 0x3784),*(undefined2 *)(iVar8 + 0x3786),
                      *(undefined2 *)(iVar8 + 0x3788));
        FUN_10a0_07af(puVar12,uVar13);
        puVar12 = local_16a;
        uVar13 = unaff_SS;
        FUN_10a0_0a24(0xffff,0x11,*(undefined2 *)(iVar8 + 0x378a),*(undefined2 *)(iVar8 + 0x378c),
                      ((undefined2 *)&DAT_10a8_378e)[iVar2 * 0x12]);
        FUN_10a0_07af(puVar12,uVar13);
        if (local_17b == '\b') break;
        local_17b = local_17b + '\x01';
      }
      local_176 = (int)DAT_10a8_22cc;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_176 = (int)DAT_10a8_22cd;
      local_174 = local_176 >> 0xf;
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,local_176,local_174);
      FUN_10a0_07af(puVar12,uVar13);
      local_17b = '\x01';
      while( true ) {
        puVar12 = local_16a;
        uVar13 = unaff_SS;
        FUN_10a0_098c(0,((undefined2 *)&DAT_10a8_3ada)[local_17b * 2],
                      ((undefined2 *)&DAT_10a8_3adc)[local_17b * 2]);
        FUN_10a0_07af(puVar12,uVar13);
        if (local_17b == '\b') break;
        local_17b = local_17b + '\x01';
      }
      puVar12 = local_16a;
      uVar13 = unaff_SS;
      FUN_10a0_098c(0,DAT_10a8_22c2,DAT_10a8_22c4);
      FUN_10a0_07af(puVar12,uVar13);
      FUN_10a0_0586(local_16a,unaff_SS);
      cVar1 = FUN_1008_19f6(iVar7,uVar9);
      if (cVar1 != '\0') {
        MESSAGEBEEP((char *)s_FALSE_10a8_1003 + 5,0x30);
        MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x303,
                   (char *)s_RandomShrs_10a8_109f + 9,0x2f1,(char *)s_RandomShrs_10a8_109f + 9);
      }
    }
    else {
      FUN_10a0_0586(local_16a,unaff_SS);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_1936 @ 1008:1936 */
/* ======================================================== */

bool __stdcall16far FUN_1008_1936(int *param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  bool local_5;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  if ((*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\0') || (DAT_10a8_1d2f != '\0')) {
    if ((DAT_10a8_1d2f == '\0') && (DAT_10a8_39a4 == '\0')) {
      MESSAGEBEEP();
      iVar2 = MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x24,0x360,
                         (char *)s_RandomShrs_10a8_109f + 9,0x33d,(char *)s_RandomShrs_10a8_109f + 9
                        );
      local_5 = iVar2 == 6;
    }
    else if (DAT_10a8_39a4 == '\0') {
      local_5 = true;
    }
    else {
      local_5 = false;
    }
  }
  else {
    uVar4 = 0x20;
    MESSAGEBEEP();
    iVar2 = ((int *)param_1)[2];
    iVar1 = MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x24,0x32b,
                       (char *)s_RandomShrs_10a8_109f + 9,0x30e,(char *)s_RandomShrs_10a8_109f + 9);
    if (iVar1 == 6) {
      DAT_10a8_1d2f = '\x01';
    }
    local_5 = false;
    (*(code *)*(undefined2 *)(*param_1 + 0x54))
              ((char *)s__Show_Graph_10a8_1226 + 10,(int *)param_1,uVar3,DAT_10a8_39a8,iVar2,uVar4);
  }
  return local_5;
}

/* ======================================================== */
/* Function: FUN_1008_19f6 @ 1008:19f6 */
/* ======================================================== */

undefined1 __stdcall16far FUN_1008_19f6(void)

{
  int unaff_BP;
  undefined2 unaff_SS;
  undefined1 *puVar1;
  undefined2 uVar2;
  undefined1 *puVar3;
  undefined2 uVar4;
  undefined1 local_1d8 [256];
  int local_d8;
  int local_d6;
  int local_d4;
  int local_d2;
  undefined1 *local_cc;
  undefined1 local_c8 [104];
  undefined1 local_60 [91];
  undefined1 local_5;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  local_d8 = FUN_10a0_0388();
  local_d6 = local_d8 >> 0xf;
  local_5 = 0;
  local_cc = local_60;
  local_d4 = local_d8;
  local_d2 = local_d6;
  FUN_10a0_1dbd(10,local_c8,unaff_SS,0,local_d8,local_d6);
  puVar1 = local_1d8;
  uVar2 = unaff_SS;
  puVar3 = local_cc;
  uVar4 = unaff_SS;
  FUN_10a0_0cbf(0x19ed,(char *)s_RandomShrs_10a8_109f + 1);
  FUN_10a0_0d3e(local_c8,unaff_SS);
  FUN_1098_009f(puVar1,uVar2,puVar3,uVar4);
  if (local_d8 != 0 || local_d6 != 0) {
    MESSAGEBEEP((char *)s_RandomDivs_10a8_1094 + 4,0x30);
    MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x372,(char *)s_RandomShrs_10a8_109f + 9,
               local_cc,unaff_SS);
    local_5 = 1;
  }
  return local_5;
}

/* ======================================================== */
/* Function: FUN_1008_1aa7 @ 1008:1aa7 */
/* ======================================================== */

void __cdecl16near FUN_1008_1aa7(void)

{
  SETFOCUS();
  return;
}

/* ======================================================== */
/* Function: FUN_1008_1e95 @ 1008:1e95 */
/* ======================================================== */

void FUN_1008_1e95(undefined4 param_1)

{
  uint uVar1;
  int iVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  bool bVar7;
  undefined4 local_e;
  uint local_6;
  int local_4;
  
  FUN_10a0_1986();
  iVar2 = DAT_10a8_22b0;
  uVar1 = DAT_10a8_22ae;
  if (((DAT_10a8_1d32 != 0 || DAT_10a8_1d34 != 0) && (-1 < DAT_10a8_22b0)) &&
     ((0 < DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_6 = 1;
    local_4 = 0;
    while( true ) {
      uVar6 = (undefined2)((ulong)param_1 >> 0x10);
      iVar5 = (int)param_1 + local_6 * 0xd;
      local_e = (char *)CONCAT22(uVar6,(char *)(iVar5 + -0xd));
      if ((DAT_10a8_22c6 == '\0') || (iVar4 = FUN_10a0_18f1(0x50), iVar4 != 2)) {
        do {
          cVar3 = FUN_10a0_18f1(DAT_10a8_22b2);
          *local_e = cVar3 + 'A';
          iVar4 = FUN_10a0_18f1(DAT_10a8_22b6);
          *(int *)(iVar5 + -0xc) = iVar4 + 1;
          *(undefined2 *)(iVar5 + -10) = 1;
        } while (*(char *)(*local_e * 0x1a + *(int *)(iVar5 + -0xc) + 0x169b) != '\x01');
        *(undefined1 *)(*local_e * 0x1a + *(int *)(iVar5 + -0xc) + 0x169b) = 0;
      }
      else {
        *local_e = 'A';
        *(undefined2 *)(iVar5 + -0xc) = 0xffff;
        *(undefined2 *)(iVar5 + -10) = 0;
      }
      *(undefined1 *)(iVar5 + -8) = 1;
      *(undefined1 *)(iVar5 + -7) = 0;
      bVar7 = DAT_10a8_1d32 == 0;
      DAT_10a8_1d32 = DAT_10a8_1d32 + -1;
      DAT_10a8_1d34 = DAT_10a8_1d34 - (uint)bVar7;
      if ((local_4 == iVar2) && (local_6 == uVar1)) break;
      bVar7 = 0xfffe < local_6;
      local_6 = local_6 + 1;
      local_4 = local_4 + (uint)bVar7;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_1fa6 @ 1008:1fa6 */
/* ======================================================== */

void __stdcall16far FUN_1008_1fa6(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x10);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_1fc6 @ 1008:1fc6 */
/* ======================================================== */

undefined2 __stdcall16far
FUN_1008_1fc6(undefined2 param_1,undefined2 param_2,char param_3,undefined1 *param_4,
             undefined2 param_5)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined2 uVar4;
  int iVar5;
  int unaff_BP;
  undefined1 *puVar6;
  undefined1 *puVar7;
  char *unaff_CS;
  undefined2 unaff_SS;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 local_56;
  undefined2 local_54;
  undefined2 local_52;
  undefined2 local_50;
  undefined1 local_4e;
  char local_4d;
  char local_4c;
  undefined1 local_4b;
  undefined2 local_22;
  undefined2 local_20;
  undefined1 local_1e [2];
  undefined2 local_1c;
  undefined2 local_1a;
  undefined2 local_10;
  undefined2 local_e;
  undefined1 local_c;
  int local_b;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar7 = &local_c;
  puVar6 = (undefined1 *)param_4;
  for (iVar5 = 5; iVar5 != 0; iVar5 = iVar5 + -1) {
    puVar2 = puVar7;
    puVar7 = puVar7 + 1;
    puVar1 = puVar6;
    puVar6 = puVar6 + 1;
    *puVar2 = *puVar1;
  }
  local_4c = '\0';
  local_4e = local_c;
  cVar3 = (char)(local_b % 10) + '0';
  local_4d = cVar3;
  if ((-1 < local_b) && (9 < local_b)) {
    local_4b = 0;
    local_4d = (char)(local_b / 10) + '0';
    local_4c = cVar3;
  }
  if (local_b == -1) {
    local_4e = 0;
    unaff_CS = (char *)s_RandomDivs_10a8_1094 + 4;
    FUN_1098_00bd(0x38c,(char *)s_RandomShrs_10a8_109f + 9,&local_4e,unaff_SS);
  }
  uVar8 = DAT_10a8_1c9e;
  if (param_3 == '\0') {
    local_20 = LOADBITMAP(unaff_CS,0x395,(char *)s_RandomShrs_10a8_109f + 9);
  }
  else {
    local_20 = LOADBITMAP(unaff_CS,0x390,(char *)s_RandomShrs_10a8_109f + 9);
  }
  local_e = CREATECOMPATIBLEDC((char *)s__Show_Graph_10a8_1226 + 10,param_5,uVar8);
  local_10 = CREATECOMPATIBLEDC((char *)s__Show_Graph_10a8_1226 + 10,param_5);
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_20);
  GETOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_1e,unaff_SS,0xe);
  local_22 = CREATECOMPATIBLEBITMAP
                       ((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_38b4,DAT_10a8_38b0);
  uVar10 = local_10;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_22);
  uVar4 = local_10;
  STRETCHBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,local_1a,local_1c,0,0,local_e,
             DAT_10a8_38b4,DAT_10a8_38b0,0,0);
  uVar9 = local_10;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_378e);
  uVar8 = local_10;
  SETBKMODE((char *)s__Show_Graph_10a8_1226 + 10,1);
  FUN_10a0_1458(uVar8,uVar9,uVar4,uVar10);
  FUN_10a0_1444();
  local_54 = FUN_10a0_145c();
  local_56 = 0;
  local_50 = DAT_10a8_38b4;
  local_52 = DAT_10a8_38b0;
  puVar7 = &local_4e;
  uVar8 = unaff_SS;
  uVar9 = local_10;
  uVar4 = FUN_1098_0002(&local_4e,unaff_SS);
  DRAWTEXT((char *)s_RandomDivs_10a8_1094 + 4,5,&local_56,unaff_SS,uVar4,puVar7,uVar8);
  DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_e,uVar9);
  DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_10);
  DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_20);
  return local_22;
}

/* ======================================================== */
/* Function: FUN_1008_229d @ 1008:229d */
/* ======================================================== */

void FUN_1008_229d(char *param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  char *pcVar5;
  int iVar6;
  undefined2 uVar7;
  bool bVar8;
  uint local_a;
  int local_8;
  uint local_6;
  int local_4;
  
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  pcVar5 = (char *)param_1;
  *(undefined1 *)(*param_1 * 0x1a + *(int *)(pcVar5 + 1) + 0x169b) = 0;
  ((char *)s_American_Natural_Gas_Co__10a8_193e + 1)[*param_1 * 0x1a + *(int *)(pcVar5 + 1)] = '\0';
  iVar2 = DAT_10a8_22ac;
  uVar1 = DAT_10a8_22aa;
  if ((-1 < DAT_10a8_22ac) && ((0 < DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
    local_6 = 1;
    local_4 = 0;
    while( true ) {
      iVar4 = DAT_10a8_22b0;
      uVar3 = DAT_10a8_22ae;
      iVar6 = local_6 * 0xfc + 0x30aa;
      if ((-1 < DAT_10a8_22b0) && ((0 < DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
        local_a = 1;
        local_8 = 0;
        while( true ) {
          if (((*(char *)(iVar6 + local_a * 0xd + 0x19) == *param_1) &&
              (*(int *)(iVar6 + local_a * 0xd + 0x1a) == *(int *)(pcVar5 + 1))) &&
             ((*(undefined1 *)(iVar6 + local_a * 0xd + 0x1e) = 0, local_4 != DAT_10a8_319c ||
              (local_6 != DAT_10a8_319a)))) {
            FUN_1008_21eb(iVar6 + local_a * 0xd + 0x19,(char *)s_RandomShrs_10a8_109f + 9);
          }
          if ((local_8 == iVar4) && (local_a == uVar3)) break;
          bVar8 = 0xfffe < local_a;
          local_a = local_a + 1;
          local_8 = local_8 + (uint)bVar8;
        }
      }
      if ((local_4 == iVar2) && (local_6 == uVar1)) break;
      bVar8 = 0xfffe < local_6;
      local_6 = local_6 + 1;
      local_4 = local_4 + (uint)bVar8;
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_3423 @ 1008:3423 */
/* ======================================================== */

void __stdcall16far FUN_1008_3423(int *param_1,undefined4 param_2)

{
  undefined2 *puVar1;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  undefined1 uVar3;
  undefined2 uVar2;
  int *piVar4;
  int iVar5;
  int *piVar6;
  char *pcVar7;
  long lVar8;
  
  if (DAT_10a8_1d2f == '\0') {
    if (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\0') {
      piVar4 = (int *)param_1;
      uVar2 = (undefined2)((ulong)param_1 >> 0x10);
      if (DAT_10a8_39a8 == '\0') {
        lVar8 = FUN_1050_0002(1,DAT_10a8_319a * 0xfc + 0x30d0,(char *)s_RandomShrs_10a8_109f + 9,
                              DAT_10a8_22ae,DAT_10a8_22b0,piVar4,uVar2);
        iVar5 = (int)lVar8;
        if (lVar8 != 0) {
          *(undefined1 *)(DAT_10a8_319a * 0xfc + iVar5 * 0xd + 0x30c8) = 0;
          *(undefined1 *)(DAT_10a8_319a * 0xfc + iVar5 * 0xd + 0x30c9) = 0;
          ((char *)s_American_Natural_Gas_Co__10a8_193e + 1)
          [*(char *)(DAT_10a8_319a * 0xfc + iVar5 * 0xd + 0x30c3) * 0x1a +
           *(int *)(DAT_10a8_319a * 0xfc + iVar5 * 0xd + 0x30c4)] = '\x02';
          pcVar7 = (char *)s_FALSE_10a8_1003 + 5;
          FUN_1008_5e10(piVar4,uVar2,lVar8);
          uVar3 = extraout_AH;
          if (DAT_10a8_22c9 != '\0') {
            pcVar7 = (char *)s_RandomShrs_10a8_109f + 1;
            FUN_10a0_1e88((char *)s_FALSE_10a8_1003 + 5,piVar4,uVar2,(int)param_2,
                          (int)((ulong)param_2 >> 0x10));
            uVar3 = extraout_AH_00;
          }
          DAT_10a8_39a8 = '\x01';
          (*(code *)*(undefined2 *)(*param_1 + 0x54))(pcVar7,piVar4,uVar2,CONCAT11(uVar3,1));
          if (DAT_10a8_22cf != '\0') {
            iVar5 = DAT_10a8_319a * 0xfc + iVar5 * 0xd;
            piVar6 = (int *)((*(char *)(iVar5 + 0x30c3) + -0x41) * 0x8c +
                             (*(int *)(iVar5 + 0x30c4) + -1) * 5 + 0x22d0);
            puVar1 = (undefined2 *)(*piVar6 + 0x10);
            (*(code *)*puVar1)(pcVar7,piVar6,(char *)s_RandomShrs_10a8_109f + 9,0);
          }
          uVar2 = GETDC(pcVar7,piVar4[2]);
          FUN_1048_14c9(uVar2);
          RELEASEDC((char *)s_StartCash_10a8_1043 + 5,uVar2);
        }
      }
      else {
        MESSAGEBEEP();
        MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x3f6,
                   (char *)s_RandomShrs_10a8_109f + 9,0x3da,(char *)s_RandomShrs_10a8_109f + 9);
      }
    }
    else {
      FUN_1048_08f5();
    }
  }
  else if (DAT_10a8_1d2f != '\0') {
    FUN_1048_13ad();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_35ac @ 1008:35ac */
/* ======================================================== */

void __stdcall16far FUN_1008_35ac(int *param_1)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  undefined1 extraout_AH;
  undefined2 uVar4;
  int unaff_BP;
  int iVar5;
  int *piVar6;
  undefined2 uVar7;
  undefined2 unaff_SS;
  bool bVar8;
  bool bVar9;
  undefined1 local_732 [32];
  undefined2 local_712;
  undefined2 local_710;
  undefined2 local_70e;
  char local_70b;
  uint local_70a;
  int local_708;
  uint local_706;
  int local_704;
  uint local_702;
  int local_700;
  int local_6fe [10];
  undefined1 local_6e9 [17];
  uint local_6d8 [97];
  undefined2 local_616 [21];
  undefined1 local_5ec [1512];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  local_6d8[0] = DAT_10a8_22aa;
  FUN_10a0_0b92(0x5e8,local_5ec,unaff_SS,0x31a6,(char *)s_RandomShrs_10a8_109f + 9);
  iVar3 = DAT_10a8_22ac;
  uVar1 = DAT_10a8_22aa;
  if ((-1 < DAT_10a8_22ac) && ((0 < DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
    local_702 = 1;
    local_700 = 0;
    while( true ) {
      iVar5 = local_702 * 0xfc;
      local_712 = *(undefined2 *)(iVar5 + 0x30ca);
      local_70b = '\x01';
      local_710 = *(undefined2 *)(iVar5 + 0x30cc);
      local_70e = *(undefined2 *)(iVar5 + 0x30ce);
      while( true ) {
        FUN_10a0_1458();
        uVar7 = *(undefined2 *)(local_70b * 0x24 + 0x3774);
        uVar4 = *(undefined2 *)(local_70b * 0x24 + 0x3776);
        FUN_10a0_1444();
        local_712 = FUN_10a0_1432();
        local_710 = uVar7;
        local_70e = uVar4;
        if (local_70b == '\b') break;
        local_70b = local_70b + '\x01';
      }
      local_616[local_702 * 3] = local_712;
      local_616[local_702 * 3 + 1] = uVar7;
      local_616[local_702 * 3 + 2] = uVar4;
      FUN_10a0_0af5(0x1e,local_732,unaff_SS,2,7,local_712,uVar7,uVar4);
      FUN_10a0_0b92(0x1f,local_6e9 + local_702 * 0x1f,unaff_SS,local_732,unaff_SS);
      if ((local_700 == iVar3) && (local_702 == uVar1)) break;
      bVar8 = 0xfffe < local_702;
      local_702 = local_702 + 1;
      local_700 = local_700 + (uint)bVar8;
    }
  }
  iVar3 = DAT_10a8_22ac;
  uVar1 = DAT_10a8_22aa;
  if ((-1 < DAT_10a8_22ac) && ((0 < DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
    local_702 = 1;
    local_700 = 0;
    while( true ) {
      iVar5 = DAT_10a8_22ac;
      uVar2 = DAT_10a8_22aa;
      local_706 = 1;
      local_704 = 0;
      if ((-1 < DAT_10a8_22ac) && ((0 < DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
        local_70a = 1;
        local_708 = 0;
        while( true ) {
          if ((local_700 != local_708) || (local_702 != local_70a)) {
            bVar8 = CARRY2(local_702 * 4,local_702 * 2);
            bVar9 = local_702 * 6 == 0;
            FUN_10a0_1454();
            if (!bVar8 && !bVar9) {
              bVar8 = 0xfffe < local_706;
              local_706 = local_706 + 1;
              local_704 = local_704 + (uint)bVar8;
            }
          }
          if ((local_708 == iVar5) && (local_70a == uVar2)) break;
          bVar8 = 0xfffe < local_70a;
          local_70a = local_70a + 1;
          local_708 = local_708 + (uint)bVar8;
        }
      }
      local_6d8[local_702] = local_706;
      if ((local_700 == iVar3) && (local_702 == uVar1)) break;
      bVar8 = 0xfffe < local_702;
      local_702 = local_702 + 1;
      local_700 = local_700 + (uint)bVar8;
    }
  }
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  piVar6 = (int *)param_1;
  FUN_1080_0002(local_6fe,unaff_SS,0xb54,0x418,(char *)s_RandomShrs_10a8_109f + 9,piVar6,uVar7);
  (*(code *)*(undefined2 *)(local_6fe[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_6fe);
  (*(code *)*(undefined2 *)(local_6fe[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_6fe);
  DAT_10a8_1d2f = 1;
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s_PayDividends_10a8_1079 + 7,piVar6,uVar7,CONCAT11(extraout_AH,DAT_10a8_39a8));
  uVar7 = GETDC((char *)s_PayDividends_10a8_1079 + 7,piVar6[2]);
  FUN_1048_14c9(uVar7);
  RELEASEDC((char *)s_StartCash_10a8_1043 + 5,uVar7);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_38a0 @ 1008:38a0 */
/* ======================================================== */

void __stdcall16far FUN_1008_38a0(int *param_1)

{
  DAT_10a8_22ce = DAT_10a8_22ce == '\0';
  (*(code *)*(undefined2 *)(*param_1 + 0x54))();
  return;
}

/* ======================================================== */
/* Function: FUN_1008_38c8 @ 1008:38c8 */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10083b74) */

void __stdcall16far FUN_1008_38c8(int *param_1)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 uVar4;
  uint uVar5;
  int iVar6;
  undefined1 uVar7;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  undefined1 extraout_AH_01;
  uint uVar8;
  int unaff_BP;
  int *piVar9;
  char *pcVar10;
  char *pcVar11;
  char *pcVar12;
  undefined2 unaff_SS;
  bool bVar13;
  bool bVar14;
  char *pcVar15;
  char local_14;
  uint local_12;
  uint local_10;
  undefined2 local_c;
  undefined2 local_a;
  undefined2 local_8;
  undefined2 local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  if (DAT_10a8_22ce != '\0') {
    FUN_1008_1aa7();
  }
  uVar5 = DAT_10a8_22b0;
  uVar8 = DAT_10a8_22ae;
  if (DAT_10a8_1d2f != '\0') {
    FUN_1048_13ad();
    return;
  }
  if ((*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) != '\0') && (DAT_10a8_39a4 != '\0')) {
    FUN_1048_08f5();
    return;
  }
  piVar9 = (int *)param_1;
  pcVar10 = (char *)((ulong)param_1 >> 0x10);
  if (DAT_10a8_39a8 == '\0') {
    MESSAGEBEEP();
    MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x444,(char *)s_RandomShrs_10a8_109f + 9,
               0x422,(char *)s_RandomShrs_10a8_109f + 9);
    return;
  }
  if ((DAT_10a8_22b0 < 0x8000) && ((0 < (int)DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_12 = 1;
    local_10 = 0;
    while( true ) {
      if (*(char *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c9) != '\0') {
        *(undefined1 *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c9) = 0;
        *(undefined1 *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c8) = 1;
      }
      if ((local_10 == uVar5) && (local_12 == uVar8)) break;
      bVar13 = 0xfffe < local_12;
      local_12 = local_12 + 1;
      local_10 = local_10 + bVar13;
    }
  }
  DAT_10a8_39a8 = 0;
  local_c = 0;
  local_8 = FUN_10a0_0baa();
  local_a = FUN_10a0_0baa();
  local_6 = FUN_10a0_0baa();
  DAT_10a8_1d31 = 0;
  INVALIDATERECT((char *)s_RandomShrs_10a8_109f + 1,0,&local_c,unaff_SS,piVar9[2]);
  if (DAT_10a8_39a3 != '\0') {
    uVar8 = DAT_10a8_22aa;
    iVar6 = DAT_10a8_22ac;
    FUN_10a0_0be7();
    if ((iVar6 + (uint)(0xfffe < uVar8) == DAT_10a8_38c6) && (uVar8 + 1 == DAT_10a8_38c4)) {
      *(undefined1 *)(DAT_10a8_38c4 * 0xfc + 0x30c9) = 0;
      *(undefined1 *)((undefined2 *)&DAT_10a8_38bc + DAT_10a8_38c4 * 0x10) = 1;
      DAT_10a8_39a3 = '\0';
    }
  }
  uVar8 = DAT_10a8_22aa;
  iVar6 = DAT_10a8_22ac;
  FUN_10a0_0be7();
  DAT_10a8_319a = uVar8 + 1;
  DAT_10a8_319c = iVar6 + (uint)(0xfffe < uVar8);
  if (DAT_10a8_22c7 != '\0') {
    FUN_1008_23d1(DAT_10a8_319a,DAT_10a8_319c);
  }
  uVar2 = GETDC((char *)s_RandomShrs_10a8_109f + 1,piVar9[2]);
  FUN_1048_14c9(uVar2);
  iVar6 = piVar9[2];
  RELEASEDC((char *)s_StartCash_10a8_1043 + 5,uVar2);
  uVar5 = DAT_10a8_22b0;
  uVar8 = DAT_10a8_22ae;
  if ((DAT_10a8_22b0 < 0x8000) && ((0 < (int)DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_12 = 1;
    local_10 = 0;
    while( true ) {
      if (*(char *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c8) == '\0') {
        FUN_1008_21eb(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c3,
                      (char *)s_RandomShrs_10a8_109f + 9);
      }
      *(int *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c6) =
           *(int *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c6) + 1;
      if ((local_10 == uVar5) && (local_12 == uVar8)) break;
      bVar13 = 0xfffe < local_12;
      local_12 = local_12 + 1;
      local_10 = local_10 + bVar13;
    }
  }
  bVar13 = false;
  local_14 = '\x01';
  while( true ) {
    uVar2 = *(undefined2 *)(local_14 * 0x24 + 0x3774);
    bVar14 = true;
    FUN_10a0_1454(iVar6);
    uVar5 = DAT_10a8_22b0;
    uVar8 = DAT_10a8_22ae;
    if (bVar14) {
      bVar13 = true;
    }
    if (local_14 == '\b') break;
    local_14 = local_14 + '\x01';
  }
  if (((!bVar13) && (-1 < (int)DAT_10a8_22b0)) && ((0 < (int)DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))
     ) {
    local_12 = 1;
    local_10 = 0;
    while( true ) {
      iVar3 = *(char *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c3) + -0x40;
      iVar6 = *(int *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c4);
      cVar1 = FUN_1008_1abd(0,iVar6,iVar6 >> 0xf,iVar3,iVar3 >> 0xf);
      if (((cVar1 == '\t') &&
          (iVar6 = *(char *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c3) + -0x41,
          iVar3 = *(int *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c4) + -1,
          cVar1 = FUN_1008_51fd(piVar9,pcVar10,0,iVar3,iVar3 >> 0xf,iVar6,iVar6 >> 0xf),
          cVar1 != '\0')) && (*(char *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c8) != '\0')) {
        *(undefined1 *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c8) = 0;
        *(undefined1 *)(DAT_10a8_319a * 0xfc + local_12 * 0xd + 0x30c9) = 1;
      }
      if ((local_10 == uVar5) && (local_12 == uVar8)) break;
      bVar13 = 0xfffe < local_12;
      local_12 = local_12 + 1;
      local_10 = local_10 + bVar13;
    }
  }
  if (DAT_10a8_1d32 == 0 && DAT_10a8_1d34 == 0) {
    bVar13 = 0xfffe < DAT_10a8_31a2;
    DAT_10a8_31a2 = DAT_10a8_31a2 + 1;
    DAT_10a8_31a4 = DAT_10a8_31a4 + bVar13;
  }
  cVar1 = FUN_1008_7591(piVar9,pcVar10);
  if (cVar1 == '\0') {
    iVar6 = DAT_10a8_22ac;
    uVar4 = FUN_10a0_1458();
    uVar8 = DAT_10a8_22b0;
    FUN_10a0_1458(uVar4,uVar2,iVar6);
    FUN_10a0_1444();
    FUN_10a0_1444();
    pcVar15 = (char *)s_RandomShrs_10a8_109f + 1;
    pcVar11 = (char *)s_RandomShrs_10a8_109f + 1;
    uVar5 = FUN_10a0_145c();
    if ((uVar8 != DAT_10a8_31a4) || (uVar5 != DAT_10a8_31a2)) goto LAB_1008_3cee;
  }
  pcVar11 = (char *)s_FALSE_10a8_1003 + 5;
  pcVar15 = pcVar10;
  FUN_1008_35ac(piVar9,pcVar10);
LAB_1008_3cee:
  iVar6 = DAT_10a8_319e + 1;
  DAT_10a8_31a0 = DAT_10a8_31a0 + (uint)(0xfffe < DAT_10a8_319e);
  DAT_10a8_319e = iVar6;
  if (DAT_10a8_22cc != '\0') {
    local_14 = '\x01';
    while( true ) {
      uVar2 = DAT_10a8_22c4;
      iVar6 = (int)local_14;
      ((undefined2 *)&DAT_10a8_3ada)[iVar6 * 2] = DAT_10a8_22c2;
      ((undefined2 *)&DAT_10a8_3adc)[iVar6 * 2] = uVar2;
      if (local_14 == '\b') break;
      local_14 = local_14 + '\x01';
    }
  }
  uVar7 = (undefined1)((uint)iVar6 >> 8);
  pcVar12 = pcVar11;
  if ((*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) != '\0') && (DAT_10a8_1d2f == '\0')) {
    pcVar15 = (char *)piVar9[2];
    pcVar12 = (char *)s__Show_Graph_10a8_1226 + 10;
    POSTMESSAGE(pcVar11,0,0,0,0x4ef);
    uVar7 = extraout_AH;
  }
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            (pcVar12,piVar9,pcVar10,CONCAT11(uVar7,DAT_10a8_39a8),pcVar15);
  FUN_1058_04ee(CONCAT11(extraout_AH_00,DAT_10a8_1d31));
  FUN_1028_0002(CONCAT11(extraout_AH_01,DAT_10a8_39a7),1,DAT_10a8_22aa,0x31a6,
                (char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_3d8d @ 1008:3d8d */
/* ======================================================== */

void __stdcall16far FUN_1008_3d8d(void)

{
  undefined2 uVar1;
  
  if (DAT_10a8_378e != 0) {
    DELETEOBJECT();
  }
  FUN_10a0_1458();
  FUN_10a0_144a();
  uVar1 = FUN_10a0_145c();
  FUN_10a0_1458(uVar1);
  FUN_10a0_144a();
  uVar1 = FUN_10a0_145c();
  DAT_10a8_378e =
       CREATEFONT((char *)s_RandomShrs_10a8_109f + 1,0,0,0x20,2,0,0,0,0,0,0,500,0,0,uVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_3e0e @ 1008:3e0e */
/* ======================================================== */

void __stdcall16far
FUN_1008_3e0e(undefined4 param_1,undefined1 param_2,int param_3,int param_4,undefined1 *param_5,
             undefined2 param_6)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined2 uVar3;
  uint uVar4;
  undefined2 uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int unaff_BP;
  undefined1 *puVar9;
  undefined1 *puVar10;
  undefined2 unaff_SS;
  undefined1 local_1c [14];
  undefined2 local_e;
  undefined2 local_c;
  undefined1 local_a [6];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  puVar10 = local_a;
  puVar9 = (undefined1 *)param_5;
  for (iVar6 = 5; iVar6 != 0; iVar6 = iVar6 + -1) {
    puVar2 = puVar10;
    puVar10 = puVar10 + 1;
    puVar1 = puVar9;
    puVar9 = puVar9 + 1;
    *puVar2 = *puVar1;
  }
  local_c = FUN_1008_1fc6((int)param_1,(int)((ulong)param_1 >> 0x10),param_2,local_a,unaff_SS,
                          param_6);
  uVar3 = CREATECOMPATIBLEDC((char *)s_FALSE_10a8_1003 + 5,param_6);
  local_e = uVar3;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_c);
  uVar5 = local_c;
  GETOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_1c,unaff_SS,0xe);
  uVar7 = 2;
  iVar8 = 0;
  FUN_10a0_0be7(uVar5,uVar3);
  iVar6 = DAT_10a8_22b0;
  uVar4 = FUN_10a0_0be7(uVar7,iVar8);
  iVar8 = iVar6 + iVar8 + (uint)CARRY2(uVar4,uVar7);
  FUN_10a0_0be7();
  iVar6 = iVar8;
  uVar5 = FUN_10a0_1458();
  FUN_10a0_1458(uVar5,iVar6,iVar8);
  FUN_10a0_1444();
  FUN_10a0_1444();
  iVar6 = FUN_10a0_145c();
  param_4 = param_4 - (uint)(param_3 == 0);
  FUN_10a0_0be7();
  uVar5 = FUN_10a0_0baa();
  FUN_10a0_0baa(uVar5,param_4);
  FUN_10a0_1458();
  uVar5 = FUN_10a0_145c();
  BITBLT((char *)s_RandomShrs_10a8_109f + 1,0x20,0xcc,0,0,local_e,DAT_10a8_38b4,DAT_10a8_38b0,uVar5,
         iVar6 + 5);
  DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_e,param_6);
  DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_c);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_3f81 @ 1008:3f81 */
/* ======================================================== */

void __stdcall16far FUN_1008_3f81(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar1 = *(undefined4 *)((int)param_3 + 6);
  uVar3 = (undefined2)((ulong)uVar1 >> 0x10);
  iVar2 = (int)uVar1;
  *(undefined2 *)(iVar2 + 0xe) = 300;
  *(undefined2 *)(iVar2 + 0xc) = 0xfa;
  return;
}

/* ======================================================== */
/* Function: FUN_1008_3fa3 @ 1008:3fa3 */
/* ======================================================== */

void __stdcall16far FUN_1008_3fa3(undefined4 param_1,undefined4 param_2,undefined2 param_3)

{
  undefined2 *puVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int *piVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  bool bVar9;
  undefined2 uVar10;
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
  uint local_8;
  uint local_6;
  
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  FUN_1008_3d8d((int)param_1,uVar7);
  uVar10 = param_3;
  SELECTOBJECT((char *)s_FALSE_10a8_1003 + 5,DAT_10a8_378e);
  uVar8 = (undefined2)((ulong)param_2 >> 0x10);
  iVar5 = (int)param_2;
  local_e = *(int *)(iVar5 + 4) >> 0xf;
  iVar3 = FUN_10a0_0be7(uVar10);
  local_10 = iVar3 - 1;
  local_e = local_e - (uint)(iVar3 == 0);
  if (local_e < 0) {
    local_10 = 0;
    local_e = 0;
  }
  local_12 = *(int *)(iVar5 + 6) >> 0xf;
  iVar3 = FUN_10a0_0be7(uVar10);
  local_14 = iVar3 - 1;
  local_12 = local_12 - (uint)(iVar3 == 0);
  if (local_12 < 0) {
    local_14 = 0;
    local_12 = 0;
  }
  local_16 = *(int *)(iVar5 + 8) >> 0xf;
  uVar4 = FUN_10a0_0be7(uVar10);
  local_18 = uVar4 + 1;
  local_16 = local_16 + (uint)(0xfffe < uVar4);
  local_1a = *(int *)(iVar5 + 10) >> 0xf;
  uVar4 = FUN_10a0_0be7();
  local_1c = uVar4 + 1;
  local_1a = local_1a + (uint)(0xfffe < uVar4);
  iVar3 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
  if ((iVar3 < local_16) || ((iVar3 <= local_16 && (DAT_10a8_22b2 - 1U < local_18)))) {
    local_18 = DAT_10a8_22b2 - 1;
    local_16 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
  }
  iVar3 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
  if ((iVar3 < local_1a) || ((iVar3 <= local_1a && (DAT_10a8_22b6 - 1U < local_1c)))) {
    local_1c = DAT_10a8_22b6 - 1;
    local_1a = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
  }
  if ((local_e <= local_16) && ((local_e < local_16 || (local_10 <= local_18)))) {
    local_8 = local_10;
    local_6 = local_e;
    while( true ) {
      if ((local_12 <= local_1a) && ((local_12 < local_1a || (local_14 <= local_1c)))) {
        local_a = local_12;
        for (local_c = local_14;
            (piVar6 = (int *)(local_8 * 0x8c + local_c * 5 + 0x22d0),
            puVar1 = (undefined2 *)(*piVar6 + 0xc),
            (*(code *)*puVar1)((char *)s_RandomShrs_10a8_109f + 1,piVar6,
                               (char *)s_RandomShrs_10a8_109f + 9,param_3), local_a != local_1a ||
            (local_c != local_1c)); local_c = local_c + 1) {
          local_a = local_a + (uint)(0xfffe < local_c);
        }
      }
      if ((local_6 == local_16) && (local_8 == local_18)) break;
      bVar9 = 0xfffe < local_8;
      local_8 = local_8 + 1;
      local_6 = local_6 + bVar9;
    }
  }
  uVar2 = DAT_10a8_22b0;
  uVar4 = DAT_10a8_22ae;
  if ((((DAT_10a8_1d31 == '\0') && (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\0')) &&
      (DAT_10a8_22b0 < 0x8000)) && ((0 < (int)DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_6 = 0;
    for (local_8 = 1;
        (FUN_1008_3e0e((int)param_1,uVar7,
                       CONCAT11((char)(local_8 * 0xd >> 8),
                                *(undefined1 *)(DAT_10a8_319a * 0xfc + local_8 * 0xd + 0x30c8)),
                       local_8,local_6,DAT_10a8_319a * 0xfc + local_8 * 0xd + 0x30c3,
                       (char *)s_RandomShrs_10a8_109f + 9,param_3), local_6 != uVar2 ||
        (local_8 != uVar4)); local_8 = local_8 + 1) {
      local_6 = local_6 + (0xfffe < local_8);
    }
  }
  iVar3 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
  if ((iVar3 < local_16) || ((iVar3 <= local_16 && (DAT_10a8_22b2 - 1U <= local_18)))) {
    FUN_1048_17cd(param_3);
  }
  FUN_1048_14c9(param_3);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_422f @ 1008:422f */
/* ======================================================== */

void FUN_1008_422f(int param_1,undefined2 param_2,char param_3)

{
  uint uVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined1 extraout_AH;
  undefined2 uVar6;
  int iVar7;
  undefined2 unaff_SS;
  bool bVar8;
  bool bVar9;
  undefined4 uVar10;
  undefined1 auStack_58 [2];
  undefined1 auStack_56 [2];
  undefined1 local_54 [3];
  char local_51 [73];
  uint local_8;
  int local_6;
  char local_3;
  
  uVar10 = *(undefined4 *)((int)DAT_10a8_1b2e + 8);
  auStack_56 = SUB42((ulong)uVar10 >> 0x10,0);
  auStack_58 = SUB42(uVar10,0);
  if (DAT_10a8_39a8 == '\0') {
    MESSAGEBEEP();
    MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x475,(char *)s_RandomShrs_10a8_109f + 9,
               0x454,(char *)s_RandomShrs_10a8_109f + 9);
  }
  else if (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\0') {
    local_51[0] = '\0';
    local_3 = '\x01';
    while( true ) {
      bVar8 = false;
      bVar9 = true;
      FUN_10a0_1454();
      if (bVar8 || bVar9) {
        iVar4 = (int)local_3;
        *(undefined2 *)(auStack_56 + iVar4 * 9 + 1) = 0;
        *(undefined2 *)(local_54 + iVar4 * 9 + 1) = 0;
      }
      else {
        uVar6 = *(undefined2 *)(DAT_10a8_319a * 0xfc + 0x30ce);
        FUN_10a0_144a();
        uVar3 = FUN_10a0_145c();
        cVar2 = local_3;
        iVar4 = (int)local_3;
        *(undefined2 *)(auStack_56 + iVar4 * 9 + 1) = uVar3;
        *(undefined2 *)(local_54 + iVar4 * 9 + 1) = uVar6;
        uVar10 = FUN_1048_0fac(CONCAT11(cVar2 >> 7,local_3));
        iVar4 = (DAT_10a8_22c0 - (int)((ulong)uVar10 >> 0x10)) -
                (uint)(DAT_10a8_22be < (uint)uVar10);
        if ((iVar4 < *(int *)(local_54 + local_3 * 9 + 1)) ||
           ((iVar4 <= *(int *)(local_54 + local_3 * 9 + 1) &&
            (DAT_10a8_22be - (uint)uVar10 < *(uint *)(auStack_56 + local_3 * 9 + 1))))) {
          uVar10 = FUN_1048_0fac(CONCAT11((char)(*(uint *)(auStack_56 + local_3 * 9 + 1) >> 8),
                                          local_3));
          iVar4 = (int)local_3;
          *(uint *)(auStack_56 + iVar4 * 9 + 1) = DAT_10a8_22be - (uint)uVar10;
          *(uint *)(local_54 + iVar4 * 9 + 1) =
               (DAT_10a8_22c0 - (int)((ulong)uVar10 >> 0x10)) - (uint)(DAT_10a8_22be < (uint)uVar10)
          ;
        }
        if (DAT_10a8_22cc != '\0') {
          if (((int)((undefined2 *)&DAT_10a8_3adc)[local_3 * 2] <
               *(int *)(local_54 + local_3 * 9 + 1)) ||
             (((int)((undefined2 *)&DAT_10a8_3adc)[local_3 * 2] <=
               *(int *)(local_54 + local_3 * 9 + 1) &&
              ((uint)((undefined2 *)&DAT_10a8_3ada)[local_3 * 2] <
               *(uint *)(auStack_56 + local_3 * 9 + 1))))) {
            uVar6 = ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2];
            iVar4 = (int)local_3;
            *(undefined2 *)(auStack_56 + iVar4 * 9 + 1) =
                 ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2];
            *(undefined2 *)(local_54 + iVar4 * 9 + 1) = uVar6;
          }
        }
        if (*(int *)(local_54 + local_3 * 9 + 1) < 0) {
          iVar4 = (int)local_3;
          *(undefined2 *)(auStack_56 + iVar4 * 9 + 1) = 0;
          *(undefined2 *)(local_54 + iVar4 * 9 + 1) = 0;
        }
      }
      iVar7 = DAT_10a8_319a * 0xfc + local_3 * 4;
      uVar6 = *(undefined2 *)(iVar7 + 0x3184);
      iVar4 = (int)local_3;
      *(undefined2 *)(local_51 + iVar4 * 9 + -8) = *(undefined2 *)(iVar7 + 0x3182);
      *(undefined2 *)(auStack_58 + iVar4 * 9 + 1) = uVar6;
      bVar8 = false;
      bVar9 = true;
      FUN_10a0_1454();
      if (bVar8 || bVar9) {
        local_51[local_3 * 9] = '\0';
      }
      else {
        local_51[local_3 * 9] = '\x01';
        local_51[0] = '\x01';
      }
      if (local_3 == '\b') break;
      local_3 = local_3 + '\x01';
    }
    local_3 = *(char *)((param_3 + -0x41) * 0x8c + (param_1 + -1) * 5 + 0x22d2);
    if (local_51[0] == '\0') {
      MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0);
      MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x4a8,(char *)s_RandomShrs_10a8_109f + 9,
                 0x486,(char *)s_RandomShrs_10a8_109f + 9);
    }
    else {
      FUN_1068_0090(local_51 + 1,unaff_SS,&local_8,unaff_SS,&local_3,unaff_SS);
      iVar4 = DAT_10a8_319a * 0xfc + local_3 * 4;
      uVar1 = *(uint *)(iVar4 + 0x3182);
      iVar4 = *(int *)(iVar4 + 0x3184);
      iVar7 = DAT_10a8_319a * 0xfc + local_3 * 4;
      *(int *)(iVar7 + 0x3182) = uVar1 + local_8;
      *(int *)(iVar7 + 0x3184) = iVar4 + local_6 + (uint)CARRY2(uVar1,local_8);
      uVar1 = ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2];
      iVar4 = ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2];
      ((undefined2 *)&DAT_10a8_3ada)[local_3 * 2] = uVar1 - local_8;
      ((undefined2 *)&DAT_10a8_3adc)[local_3 * 2] = (iVar4 - local_6) - (uint)(uVar1 < local_8);
      iVar4 = local_3 * 0x24;
      FUN_10a0_1458(*(undefined2 *)(iVar4 + 0x3772),*(undefined2 *)(iVar4 + 0x3774),
                    *(undefined2 *)(iVar4 + 0x3776));
      FUN_10a0_1444();
      uVar6 = *(undefined2 *)(DAT_10a8_319a * 0xfc + 0x30cc);
      uVar3 = *(undefined2 *)(DAT_10a8_319a * 0xfc + 0x30ce);
      uVar5 = FUN_10a0_1438();
      iVar4 = DAT_10a8_319a * 0xfc;
      *(undefined2 *)(iVar4 + 0x30ca) = uVar5;
      *(undefined2 *)(iVar4 + 0x30cc) = uVar6;
      *(undefined2 *)(iVar4 + 0x30ce) = uVar3;
      uVar10 = *(undefined4 *)((int)DAT_10a8_1b2e + 8);
      local_54._0_2_ = GETDC((char *)s_RandomShrs_10a8_109f + 1,*(undefined2 *)((int)uVar10 + 4));
      FUN_1048_14c9(local_54._0_2_);
      RELEASEDC((char *)s_StartCash_10a8_1043 + 5,local_54._0_2_);
      FUN_1028_0002(CONCAT11(extraout_AH,DAT_10a8_39a7),1,DAT_10a8_22aa,0x31a6,
                    (char *)s_RandomShrs_10a8_109f + 9);
    }
  }
  else {
    FUN_1048_08f5();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_4c3f @ 1008:4c3f */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10084cc6) */
/* WARNING: Removing unreachable block (ram,0x10084c8d) */

void __stdcall16far FUN_1008_4c3f(int *param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int iVar6;
  int in_BX;
  int unaff_BP;
  char *unaff_CS;
  undefined2 unaff_SS;
  bool bVar7;
  undefined4 local_34;
  uint local_30;
  char cStack_2d;
  uint local_c;
  int local_a;
  uint local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  cStack_2d = '\x01';
  while( true ) {
    iVar1 = (int)cStack_2d;
    (&local_30)[iVar1 * 2] = 0;
    *(undefined2 *)(&stack0xffd2 + iVar1 * 4) = 0;
    if (cStack_2d == '\b') break;
    cStack_2d = cStack_2d + '\x01';
  }
  iVar1 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
  if (-1 < iVar1) {
    local_8 = 0;
    local_6 = 0;
    while( true ) {
      iVar6 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
      if (-1 < iVar6) {
        local_c = 0;
        local_a = 0;
        while( true ) {
          if ((*(char *)(local_8 * 0x8c + local_c * 5 + 0x22d2) != '\0') &&
             (*(char *)(local_8 * 0x8c + local_c * 5 + 0x22d2) != '\t')) {
            iVar2 = (int)*(char *)(local_8 * 0x8c + local_c * 5 + 0x22d2);
            if ((*(int *)(&stack0xffd2 + iVar2 * 4) < 0) ||
               ((*(int *)(&stack0xffd2 + iVar2 * 4) < 1 && ((&local_30)[iVar2 * 2] < 0x2d)))) {
              iVar2 = (int)*(char *)(local_8 * 0x8c + local_c * 5 + 0x22d2);
              in_BX = *(int *)(&stack0xffd2 + iVar2 * 4) + (uint)(0xfffe < (&local_30)[iVar2 * 2]);
              iVar3 = (int)*(char *)(local_8 * 0x8c + local_c * 5 + 0x22d2);
              (&local_30)[iVar3 * 2] = (&local_30)[iVar2 * 2] + 1;
              *(int *)(&stack0xffd2 + iVar3 * 4) = in_BX;
            }
          }
          if ((local_a == iVar6) && (local_c == DAT_10a8_22b6 - 1U)) break;
          bVar7 = 0xfffe < local_c;
          local_c = local_c + 1;
          local_a = local_a + (uint)bVar7;
        }
      }
      if ((local_6 == iVar1) && (local_8 == DAT_10a8_22b2 - 1U)) break;
      bVar7 = 0xfffe < local_8;
      local_8 = local_8 + 1;
      local_6 = local_6 + (uint)bVar7;
    }
  }
  cStack_2d = '\x01';
  while( true ) {
    iVar1 = cStack_2d * 0x24;
    local_34 = (undefined2 *)
               CONCAT22((char *)s_RandomShrs_10a8_109f + 9,(undefined2 *)(iVar1 + 0x376c));
    if ((*(int *)(&stack0xffd2 + cStack_2d * 4) < 1) &&
       ((*(int *)(&stack0xffd2 + cStack_2d * 4) < 0 || ((&local_30)[cStack_2d * 2] == 0)))) {
      *(undefined2 *)(iVar1 + 0x3772) = 0;
      *(undefined2 *)(iVar1 + 0x3774) = 0;
      *(undefined2 *)(iVar1 + 0x3776) = 0;
      *local_34 = 0;
      *(undefined2 *)(iVar1 + 0x376e) = 0;
      *(undefined2 *)(iVar1 + 0x3770) = 0;
    }
    else {
      iVar6 = *(int *)(&stack0xffd2 + cStack_2d * 4) - (uint)((&local_30)[cStack_2d * 2] < 2);
      local_c = (&local_30)[cStack_2d * 2] - 2;
      local_a = iVar6;
      if ((cStack_2d == '\x01') || (cStack_2d == '\x02')) {
        FUN_10a0_1458();
        FUN_10a0_1444();
        uVar4 = FUN_10a0_1432();
        *(undefined2 *)(iVar1 + 0x3772) = uVar4;
        *(int *)(iVar1 + 0x3774) = in_BX;
        *(int *)(iVar1 + 0x3776) = iVar6;
        iVar6 = local_a;
        FUN_10a0_1458();
        FUN_10a0_1444();
        FUN_10a0_1432();
        unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
        uVar4 = FUN_10a0_1444();
        *local_34 = uVar4;
        *(int *)(iVar1 + 0x376e) = in_BX;
        *(int *)(iVar1 + 0x3770) = iVar6;
      }
      else if (((cStack_2d == '\x03') || (cStack_2d == '\x04')) || (cStack_2d == '\x05')) {
        FUN_10a0_1458();
        FUN_10a0_1444();
        uVar4 = FUN_10a0_1432();
        *(undefined2 *)(iVar1 + 0x3772) = uVar4;
        *(int *)(iVar1 + 0x3774) = in_BX;
        *(int *)(iVar1 + 0x3776) = iVar6;
        iVar6 = local_a;
        FUN_10a0_1458();
        FUN_10a0_1444();
        FUN_10a0_1432();
        unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
        uVar4 = FUN_10a0_1444();
        *local_34 = uVar4;
        *(int *)(iVar1 + 0x376e) = in_BX;
        *(int *)(iVar1 + 0x3770) = iVar6;
      }
      else if (((cStack_2d == '\b') || (cStack_2d == '\a')) || (cStack_2d == '\x06')) {
        FUN_10a0_1458();
        FUN_10a0_1444();
        uVar4 = FUN_10a0_1432();
        *(undefined2 *)(iVar1 + 0x3772) = uVar4;
        *(int *)(iVar1 + 0x3774) = in_BX;
        *(int *)(iVar1 + 0x3776) = iVar6;
        iVar6 = local_a;
        FUN_10a0_1458();
        FUN_10a0_1444();
        FUN_10a0_1432();
        unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
        uVar4 = FUN_10a0_1444();
        *local_34 = uVar4;
        *(int *)(iVar1 + 0x376e) = in_BX;
        *(int *)(iVar1 + 0x3770) = iVar6;
      }
      if (DAT_10a8_22ca != '\0') {
        FUN_10a0_1458();
        FUN_10a0_1444();
        in_BX = *(int *)(cStack_2d * 0x24 + 0x3780);
        uVar4 = *(undefined2 *)(cStack_2d * 0x24 + 0x3782);
        unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
        uVar5 = FUN_10a0_1432();
        iVar1 = cStack_2d * 0x24;
        *(undefined2 *)(iVar1 + 0x376c) = uVar5;
        *(int *)(iVar1 + 0x376e) = in_BX;
        *(undefined2 *)(iVar1 + 0x3770) = uVar4;
      }
    }
    if (cStack_2d == '\b') break;
    cStack_2d = cStack_2d + '\x01';
  }
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  if (DAT_10a8_1d30 != '\0') {
    local_30 = GETDC(unaff_CS,((int *)param_1)[2]);
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_378e);
    SETBKCOLOR((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_19ba,DAT_10a8_19bc);
    FUN_1048_17cd(local_30);
    unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
    RELEASEDC((char *)s_StartCash_10a8_1043 + 5,local_30);
  }
  DAT_10a8_1d30 = 0;
  (*(code *)*(undefined2 *)(*param_1 + 0x5c))(unaff_CS,(int *)param_1,uVar4);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_50e3 @ 1008:50e3 */
/* ======================================================== */

void __stdcall16far FUN_1008_50e3(undefined4 param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int unaff_BP;
  int iVar4;
  int iVar5;
  char *unaff_CS;
  undefined2 unaff_SS;
  undefined1 local_10b [21];
  undefined2 uStack_f6;
  char *pcStack_f4;
  undefined2 uStack_f2;
  undefined1 *puStack_f0;
  undefined1 *local_ec;
  int local_e8;
  undefined1 local_e6 [223];
  char local_7;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  local_e8 = DAT_10a8_22aa;
  if (0 < DAT_10a8_22aa) {
    local_6 = 1;
    while( true ) {
      iVar4 = local_6 * 0xfc;
      uVar3 = *(undefined2 *)(iVar4 + 0x30cc);
      uVar2 = *(undefined2 *)(iVar4 + 0x30ce);
      iVar5 = local_6 * 0x25;
      *(undefined2 *)((int)&local_ec + iVar5) = *(undefined2 *)(iVar4 + 0x30ca);
      *(undefined2 *)(local_e6 + iVar5 + -4) = uVar3;
      *(undefined2 *)(local_e6 + iVar5 + -2) = uVar2;
      local_ec = (undefined1 *)(local_6 * 0xfc + 0x30aa);
      puStack_f0 = local_10b + local_6 * 0x25;
      uStack_f2 = 0x1f;
      uStack_f6 = 0x5144;
      pcStack_f4 = unaff_CS;
      FUN_10a0_0b92();
      local_7 = '\x01';
      while( true ) {
        local_ec = (undefined1 *)0x5168;
        FUN_10a0_1458();
        uVar3 = *(undefined2 *)(local_7 * 0x24 + 0x3774);
        uVar2 = *(undefined2 *)(local_7 * 0x24 + 0x3776);
        local_ec = (undefined1 *)0x5186;
        FUN_10a0_1444();
        unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
        local_ec = (undefined1 *)0x519b;
        uVar1 = FUN_10a0_1432();
        iVar4 = local_6 * 0x25;
        *(undefined2 *)((int)&local_ec + iVar4) = uVar1;
        *(undefined2 *)(local_e6 + iVar4 + -4) = uVar3;
        *(undefined2 *)(local_e6 + iVar4 + -2) = uVar2;
        if (local_7 == '\b') break;
        local_7 = local_7 + '\x01';
      }
      if (local_6 == local_e8) break;
      local_6 = local_6 + 1;
    }
  }
  iVar4 = (int)param_1;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  if (DAT_10a8_1d24 != '\0') {
    *(undefined2 *)(iVar4 + 0x57) = 0;
    *(undefined2 *)(iVar4 + 0x59) = 0;
  }
  if (*(int *)(iVar4 + 0x57) != 0 || *(int *)(iVar4 + 0x59) != 0) {
    local_ec = local_e6;
    puStack_f0 = (undefined1 *)((ulong)*(undefined4 *)(iVar4 + 0x57) >> 0x10);
    uStack_f2 = (undefined2)*(undefined4 *)(iVar4 + 0x57);
    uStack_f6 = 0x51f6;
    pcStack_f4 = unaff_CS;
    FUN_1020_017c();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_51fd @ 1008:51fd */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x1008529e) */
/* WARNING: Removing unreachable block (ram,0x100852b2) */

undefined1 __stdcall16far
FUN_1008_51fd(undefined4 param_1,char param_2,uint param_3,int param_4,uint param_5,int param_6)

{
  uint uVar1;
  undefined2 *puVar2;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  char *unaff_CS;
  char *pcVar8;
  bool bVar9;
  char local_f;
  uint local_e;
  int local_c;
  uint local_a;
  int local_8;
  undefined1 local_5;
  
  local_f = '\0';
  local_5 = 0;
  iVar3 = param_6 + (uint)(0xfffe < param_5);
  local_a = param_5 - 1;
  local_8 = param_6 - (uint)(param_5 == 0);
  if ((local_8 <= iVar3) && ((local_8 < iVar3 || (local_a <= param_5 + 1)))) {
    while( true ) {
      iVar4 = param_4 + (uint)(0xfffe < param_3);
      local_e = param_3 - 1;
      local_c = param_4 - (uint)(param_3 == 0);
      if ((local_c <= iVar4) && ((local_c < iVar4 || (local_e <= param_3 + 1)))) {
        while( true ) {
          if ((((-1 < local_8) && ((-1 < local_c && (local_8 <= DAT_10a8_22b4)))) &&
              ((local_8 < DAT_10a8_22b4 || (local_a < DAT_10a8_22b2)))) &&
             (((local_c <= DAT_10a8_22b8 && ((local_c < DAT_10a8_22b8 || (local_e < DAT_10a8_22b6)))
               ) && ((((local_8 == param_6 && (local_a == param_5)) ||
                      ((local_c == param_4 && (local_e == param_3)))) &&
                     (((local_8 != param_6 ||
                       (((local_a != param_5 || (local_c != param_4)) || (local_e != param_3)))) &&
                      (*(char *)(local_a * 0x8c + local_e * 5 + 0x22d2) == '\t')))))))) {
            if (local_f == '\0') {
              if (param_2 != '\0') {
                if (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\x01') {
                  pcVar8 = (char *)s_StartCash_10a8_1043 + 5;
                  local_f = FUN_1048_0988();
                }
                else {
                  pcVar8 = (char *)s_PurchaseLimitNumber_10a8_105e + 10;
                  local_f = FUN_1068_0761(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
                }
                iVar5 = DAT_10a8_319a * 0xfc + local_f * 4;
                uVar1 = *(uint *)(iVar5 + 0x3182);
                iVar5 = *(int *)(iVar5 + 0x3184);
                iVar6 = DAT_10a8_319a * 0xfc + local_f * 4;
                *(int *)(iVar6 + 0x3182) = uVar1 + 10;
                *(int *)(iVar6 + 0x3184) = iVar5 + (uint)(0xfff5 < uVar1);
                piVar7 = (int *)(local_a * 0x8c + local_e * 5 + 0x22d0);
                puVar2 = (undefined2 *)(*piVar7 + 0x10);
                (*(code *)*puVar2)(pcVar8,piVar7,(char *)s_RandomShrs_10a8_109f + 9,
                                   CONCAT11((char)((uint)(local_f * 4) >> 8),local_f));
                piVar7 = (int *)(param_5 * 0x8c + param_3 * 5 + 0x22d0);
                puVar2 = (undefined2 *)(*piVar7 + 0x10);
                (*(code *)*puVar2)(pcVar8,piVar7,(char *)s_RandomShrs_10a8_109f + 9,
                                   CONCAT11(extraout_AH,local_f));
                DAT_10a8_1d30 = 1;
                unaff_CS = (char *)s_NumPlayers_10a8_1022 + 6;
                FUN_1028_0002(CONCAT11(extraout_AH_00,DAT_10a8_39a7),1,DAT_10a8_22aa,0x31a6,
                              (char *)s_RandomShrs_10a8_109f + 9);
              }
              local_5 = 1;
            }
            else {
              piVar7 = (int *)(local_a * 0x8c + local_e * 5 + 0x22d0);
              puVar2 = (undefined2 *)(*piVar7 + 0x10);
              (*(code *)*puVar2)(unaff_CS,piVar7,(char *)s_RandomShrs_10a8_109f + 9,
                                 CONCAT11((char)(local_e * 5 >> 8),local_f));
            }
          }
          if ((local_c == iVar4) && (local_e == param_3 + 1)) break;
          bVar9 = 0xfffe < local_e;
          local_e = local_e + 1;
          local_c = local_c + (uint)bVar9;
        }
      }
      if ((local_8 == iVar3) && (local_a == param_5 + 1)) break;
      bVar9 = 0xfffe < local_a;
      local_a = local_a + 1;
      local_8 = local_8 + (uint)bVar9;
    }
  }
  FUN_1008_4c3f((int)param_1,(int)((ulong)param_1 >> 0x10));
  return local_5;
}

/* ======================================================== */
/* Function: FUN_1008_5484 @ 1008:5484 */
/* ======================================================== */

void __stdcall16far FUN_1008_5484(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int unaff_BP;
  int iVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 unaff_SS;
  undefined1 uVar11;
  undefined1 uVar12;
  undefined2 uVar13;
  undefined2 local_12;
  undefined2 local_10;
  undefined2 local_e;
  undefined1 local_c [8];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  if (DAT_10a8_39a8 != '\0') {
    return;
  }
  if (DAT_10a8_1d2f == '\0') {
    if (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) != '\0') {
      return;
    }
    uVar9 = (undefined2)((ulong)param_2 >> 0x10);
    iVar1 = (int)param_2;
    uVar10 = (undefined2)((ulong)param_1 >> 0x10);
    iVar8 = (int)param_1;
    uVar4 = *(undefined2 *)(iVar8 + 4);
    SETCAPTURE();
    *(undefined1 *)(iVar8 + 0x47) = 1;
    *(undefined1 *)(iVar8 + 0x48) = 0;
    uVar13 = *(undefined2 *)(iVar8 + 4);
    GETCLIENTRECT((char *)s__Show_Graph_10a8_1226 + 10,local_c,unaff_SS);
    uVar3 = DAT_10a8_22b8;
    uVar2 = FUN_10a0_0baa(uVar13,uVar4);
    if (((int)uVar3 < 1) && ((0x7fff < uVar3 || (uVar2 <= *(uint *)(iVar1 + 8))))) {
      uVar2 = 2;
      iVar7 = 0;
      FUN_10a0_0be7();
      iVar6 = DAT_10a8_22b0;
      uVar3 = FUN_10a0_0be7(uVar2,iVar7);
      iVar5 = iVar6 + iVar7 + (uint)CARRY2(uVar3,uVar2);
      uVar4 = FUN_10a0_1458();
      iVar6 = DAT_10a8_38b2;
      FUN_10a0_1458(uVar4,iVar7,iVar5);
      FUN_10a0_1444();
      uVar4 = FUN_10a0_1444();
      uVar11 = 0;
      uVar12 = 1;
      FUN_10a0_1458(uVar4,iVar7,iVar6);
      FUN_10a0_1454();
      if ((bool)uVar11 || (bool)uVar12) {
        if (DAT_10a8_1d31 != '\0') {
          RELEASECAPTURE((char *)s_RandomShrs_10a8_109f + 1);
          *(undefined1 *)(iVar8 + 0x47) = 0;
          return;
        }
        iVar6 = DAT_10a8_38b2;
        FUN_10a0_1458();
        FUN_10a0_1444();
        uVar3 = FUN_10a0_145c();
        FUN_10a0_0be7();
        if ((iVar6 < DAT_10a8_38b2) || ((iVar6 <= DAT_10a8_38b2 && (uVar3 <= DAT_10a8_38b0)))) {
          uVar2 = 2;
          iVar7 = 0;
          FUN_10a0_0be7();
          iVar6 = DAT_10a8_22b0;
          uVar3 = FUN_10a0_0be7(uVar2,iVar7);
          iVar5 = DAT_10a8_38b6;
          uVar3 = FUN_10a0_0baa(uVar3 + uVar2,iVar6 + iVar7 + (uint)CARRY2(uVar3,uVar2));
          iVar5 = -(uint)(*(uint *)(iVar1 + 8) < uVar3) - iVar5;
          FUN_10a0_0be7();
          uVar3 = FUN_10a0_0baa();
          FUN_10a0_1458();
          FUN_10a0_1444();
          FUN_10a0_145c();
          iVar6 = 0;
          uVar2 = FUN_10a0_0be7();
          DAT_10a8_38c8 = uVar2 + uVar3 + 1;
          DAT_10a8_38ca = iVar6 + iVar5 + (uint)CARRY2(uVar2,uVar3) + (uint)(0xfffe < uVar2 + uVar3)
          ;
          if ((DAT_10a8_22b0 < DAT_10a8_38ca) ||
             ((DAT_10a8_22b0 <= DAT_10a8_38ca && (DAT_10a8_22ae < DAT_10a8_38c8)))) {
            RELEASECAPTURE((char *)s_RandomShrs_10a8_109f + 1);
            *(undefined1 *)(iVar8 + 0x47) = 0;
            return;
          }
          if (*(char *)(DAT_10a8_319a * 0xfc + DAT_10a8_38c8 * 0xd + 0x30c8) == '\0') {
            MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0);
            RELEASECAPTURE((char *)s__Show_Graph_10a8_1226 + 10);
            *(undefined1 *)(iVar8 + 0x47) = 0;
            return;
          }
          local_e = GETDC((char *)s_RandomShrs_10a8_109f + 1,*(undefined2 *)(iVar8 + 4));
          uVar4 = FUN_1008_1fc6(iVar8,uVar10,1,DAT_10a8_319a * 0xfc + DAT_10a8_38c8 * 0xd + 0x30c3,
                                (char *)s_RandomShrs_10a8_109f + 9,local_e);
          *(undefined2 *)(iVar8 + 0x49) = uVar4;
          FUN_10a0_0b92(5,(undefined1 *)&DAT_10a8_3194,(char *)s_RandomShrs_10a8_109f + 9,
                        DAT_10a8_319a * 0xfc + DAT_10a8_38c8 * 0xd + 0x30c3,
                        (char *)s_RandomShrs_10a8_109f + 9);
          uVar4 = CREATECOMPATIBLEBITMAP
                            ((char *)s_RandomShrs_10a8_109f + 1,DAT_10a8_38b4,DAT_10a8_38b0);
          *(undefined2 *)(iVar8 + 0x4b) = uVar4;
          RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,local_e);
        }
        else {
          RELEASECAPTURE((char *)s_RandomShrs_10a8_109f + 1);
          *(undefined1 *)(iVar8 + 0x47) = 0;
        }
        GETCURSORPOS((char *)s__Show_Graph_10a8_1226 + 10,&local_12);
        SETCURSORPOS((char *)s__Show_Graph_10a8_1226 + 10,local_10,local_12);
        FUN_1078_1992(iVar8,uVar10,iVar1,uVar9);
        return;
      }
    }
    RELEASECAPTURE((char *)s_RandomShrs_10a8_109f + 1);
    *(undefined1 *)(iVar8 + 0x47) = 0;
    return;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_57a7 @ 1008:57a7 */
/* ======================================================== */

void __stdcall16far FUN_1008_57a7(int *param_1,undefined4 param_2)

{
  undefined2 *puVar1;
  char cVar2;
  undefined2 uVar3;
  uint uVar4;
  uint uVar5;
  undefined1 uVar6;
  undefined1 extraout_AH;
  int iVar7;
  int unaff_BP;
  int *piVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  undefined2 uVar12;
  char *pcVar13;
  undefined2 unaff_SS;
  int iVar14;
  char *pcVar15;
  undefined2 local_30;
  int local_2e;
  undefined2 local_2c;
  int local_2a;
  undefined2 local_1a;
  undefined2 local_18;
  undefined1 local_15;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  iVar11 = (int)((ulong)param_1 >> 0x10);
  piVar8 = (int *)param_1;
  if (*(char *)((int)piVar8 + 0x47) == '\0') {
    return;
  }
  if ((char)piVar8[0x24] == '\0') {
    RELEASECAPTURE();
    DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)((int)piVar8 + 0x49));
    DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)((int)piVar8 + 0x4b));
    *(undefined1 *)((int)piVar8 + 0x47) = 0;
    return;
  }
  RELEASECAPTURE();
  local_18 = GETDC((char *)s__Show_Graph_10a8_1226 + 10,piVar8[2]);
  uVar3 = CREATECOMPATIBLEDC((char *)s__Show_Graph_10a8_1226 + 10,local_18);
  local_1a = uVar3;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)((int)piVar8 + 0x4b));
  uVar12 = local_18;
  BITBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,0,0,local_1a,DAT_10a8_38b4,DAT_10a8_38b0,
         *(undefined2 *)((int)piVar8 + 0x53),*(undefined2 *)((int)piVar8 + 0x4f));
  iVar14 = piVar8[2];
  RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,local_18);
  DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_1a,iVar14,uVar12,uVar3);
  *(undefined1 *)((int)piVar8 + 0x47) = 0;
  pcVar15 = (char *)s__Show_Graph_10a8_1226 + 10;
  pcVar13 = (char *)s_RandomShrs_10a8_109f + 1;
  iVar14 = 0x5876;
  uVar5 = DAT_10a8_22b4;
  uVar4 = FUN_10a0_0baa();
  uVar12 = (undefined2)((ulong)param_2 >> 0x10);
  iVar9 = (int)param_2;
  if ((uVar5 < 0x8000) && ((0 < (int)uVar5 || (*(uint *)(iVar9 + 6) <= uVar4)))) {
    pcVar15 = (char *)s_RandomShrs_10a8_109f + 1;
    pcVar13 = (char *)s_RandomShrs_10a8_109f + 1;
    iVar14 = 0x58b1;
    uVar5 = DAT_10a8_22b8;
    uVar4 = FUN_10a0_0baa();
    if ((uVar5 < 0x8000) && ((0 < (int)uVar5 || (*(uint *)(iVar9 + 8) <= uVar4)))) {
      iVar14 = 0;
      uVar5 = FUN_10a0_0be7();
      local_8 = uVar5 + 1;
      local_6 = iVar14 + (uint)(0xfffe < uVar5);
      iVar7 = 0;
      pcVar15 = (char *)s_RandomShrs_10a8_109f + 1;
      pcVar13 = (char *)s_RandomShrs_10a8_109f + 1;
      iVar14 = 0x5910;
      uVar5 = FUN_10a0_0be7();
      local_c = uVar5 + 1;
      local_a = iVar7 + (uint)(0xfffe < uVar5);
      iVar7 = local_8 + 0x40;
      if ((((char)(local_8 + 0x40) == DAT_10a8_3194) &&
          ((iVar7 = DAT_10a8_3195, DAT_10a8_3195 >> 0xf == local_a && (DAT_10a8_3195 == local_c))))
         || (DAT_10a8_3195 == -1)) {
        uVar6 = (undefined1)((uint)iVar7 >> 8);
        if (DAT_10a8_3195 == -1) {
          if (((char *)s_American_Natural_Gas_Co__10a8_193e + 1)
              [(char)((char)local_8 + '@') * 0x1a + local_c] == '\0') {
            pcVar15 = (char *)0x0;
            MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0);
            iVar14 = piVar8[2];
            pcVar13 = (char *)s__Show_Graph_10a8_1226 + 10;
            MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x507,
                       (char *)s_RandomShrs_10a8_109f + 9,0x4ee,(char *)s_RandomShrs_10a8_109f + 9);
            goto LAB_1008_5b01;
          }
          DAT_10a8_3194 = (char)local_8 + '@';
          DAT_10a8_3195 = local_c;
          *(undefined1 *)(DAT_10a8_319a * 0xfc + DAT_10a8_38c8 * 0xd + 0x30c8) = 0;
          local_2e = FUN_10a0_0baa();
          local_2a = FUN_10a0_0baa();
          local_2a = local_2e + local_2a;
          local_30 = 0;
          local_2c = FUN_10a0_0baa();
          pcVar13 = (char *)s__Show_Graph_10a8_1226 + 10;
          INVALIDATERECT((char *)s_RandomShrs_10a8_109f + 1,0,&local_30,unaff_SS,piVar8[2]);
          uVar6 = extraout_AH;
        }
        DAT_10a8_39a8 = 1;
        pcVar15 = (char *)CONCAT11(uVar6,1);
        iVar14 = iVar11;
        (*(code *)*(undefined2 *)(*param_1 + 0x54))(pcVar13,piVar8,iVar11,pcVar15);
        uVar3 = FUN_1008_1abd(1,local_c,local_a,local_8,local_6);
        local_15 = (undefined1)uVar3;
        piVar10 = (int *)((DAT_10a8_3194 + -0x41) * 0x8c + (DAT_10a8_3195 + -1) * 5 + 0x22d0);
        puVar1 = (undefined2 *)(*piVar10 + 0x10);
        (*(code *)*puVar1)(pcVar13,piVar10,(char *)s_RandomShrs_10a8_109f + 9,uVar3);
        FUN_1008_229d((undefined1 *)&DAT_10a8_3194,(char *)s_RandomShrs_10a8_109f + 9);
        FUN_1008_5e10(piVar8,iVar11,DAT_10a8_38c8,DAT_10a8_38ca);
        if ((DAT_10a8_1d31 == '\0') && (DAT_10a8_22c9 != '\0')) {
          FUN_10a0_1e88((char *)s_FALSE_10a8_1003 + 5,piVar8,iVar11,iVar9,uVar12);
        }
        pcVar13 = (char *)s_FALSE_10a8_1003 + 5;
        cVar2 = FUN_1008_2448(piVar8,iVar11,1,local_c + -1,local_a - (uint)(local_c == 0),
                              local_8 + -1,local_6 - (uint)(local_8 == 0));
        if (cVar2 == '\0') {
          pcVar13 = (char *)s_FALSE_10a8_1003 + 5;
          FUN_1008_51fd(piVar8,iVar11,1,local_c + -1,local_a - (uint)(local_c == 0),local_8 + -1,
                        local_6 - (uint)(local_8 == 0));
        }
      }
    }
  }
LAB_1008_5b01:
  DELETEOBJECT(pcVar13,*(undefined2 *)((int)piVar8 + 0x49),iVar14,pcVar15);
  DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)((int)piVar8 + 0x4b));
  return;
}

/* ======================================================== */
/* Function: FUN_1008_5d6b @ 1008:5d6b */
/* ======================================================== */

void __stdcall16far FUN_1008_5d6b(undefined4 param_1)

{
  uint uVar1;
  undefined4 uVar2;
  
  uVar2 = FUN_1048_1035(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
  uVar1 = (uint)uVar2 | (uint)((ulong)uVar2 >> 0x10);
  if (uVar1 == 0) {
    MESSAGEBOX((char *)s_StartCash_10a8_1043 + 5,0x30,0x543,(char *)s_RandomShrs_10a8_109f + 9,0x520
               ,(char *)s_RandomShrs_10a8_109f + 9);
  }
  else {
    FUN_1040_0043(CONCAT11((char)(uVar1 >> 8),DAT_10a8_1cfc),0x3790,
                  (char *)s_RandomShrs_10a8_109f + 9,DAT_10a8_22aa,DAT_10a8_22ac,0x31a6,
                  (char *)s_RandomShrs_10a8_109f + 9,(int)param_1,(int)((ulong)param_1 >> 0x10));
    DAT_10a8_1cfc = 9;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_5dc4 @ 1008:5dc4 */
/* ======================================================== */

void __stdcall16far FUN_1008_5dc4(undefined4 param_1)

{
  uint uVar1;
  undefined4 uVar2;
  
  uVar2 = FUN_1048_1035(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
  uVar1 = (uint)uVar2 | (uint)((ulong)uVar2 >> 0x10);
  if (uVar1 == 0) {
    MESSAGEBOX((char *)s_StartCash_10a8_1043 + 5,0x30,0x579,(char *)s_RandomShrs_10a8_109f + 9,0x556
               ,(char *)s_RandomShrs_10a8_109f + 9);
  }
  else {
    FUN_1040_00f0(CONCAT11((char)(uVar1 >> 8),DAT_10a8_1cfc),0x3790,
                  (char *)s_RandomShrs_10a8_109f + 9,(int)param_1,(int)((ulong)param_1 >> 0x10));
    DAT_10a8_1cfc = 9;
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_5e10 @ 1008:5e10 */
/* ======================================================== */

void __stdcall16far FUN_1008_5e10(undefined4 param_1,int param_2,undefined2 param_3)

{
  undefined2 uVar1;
  char *pcVar2;
  
  pcVar2 = (char *)s__Show_Graph_10a8_1226 + 10;
  uVar1 = GETDC();
  if ((DAT_10a8_1d31 == '\0') && (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\0')) {
    pcVar2 = (char *)s_FALSE_10a8_1003 + 5;
    FUN_1008_3e0e((int)param_1,(int)((ulong)param_1 >> 0x10),
                  CONCAT11((char)((uint)(param_2 * 0xd) >> 8),
                           *(undefined1 *)(DAT_10a8_319a * 0xfc + param_2 * 0xd + 0x30c8)),param_2,
                  param_3,DAT_10a8_319a * 0xfc + param_2 * 0xd + 0x30c3,
                  (char *)s_RandomShrs_10a8_109f + 9,uVar1);
  }
  SETBKMODE(pcVar2,1);
  RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_5e91 @ 1008:5e91 */
/* ======================================================== */

void __stdcall16far FUN_1008_5e91(undefined4 param_1)

{
  FUN_1040_0043(1,0x3790,(char *)s_RandomShrs_10a8_109f + 9,DAT_10a8_22aa,DAT_10a8_22ac,0x31a6,
                (char *)s_RandomShrs_10a8_109f + 9,(int)param_1,(int)((ulong)param_1 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1008_5ebb @ 1008:5ebb */
/* ======================================================== */

void __stdcall16far FUN_1008_5ebb(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  uint local_22;
  int local_20;
  uint local_1e;
  int local_1c;
  
  uVar8 = (undefined2)((ulong)param_1 >> 0x10);
  iVar7 = (int)param_1;
  if (*(char *)(iVar7 + 0x47) != '\0') {
    uVar9 = (undefined2)((ulong)param_2 >> 0x10);
    local_1e = *(uint *)((int)param_2 + 6);
    local_1c = 0;
    local_22 = *(uint *)((int)param_2 + 8);
    local_20 = 0;
    iVar6 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
    uVar1 = FUN_10a0_0baa();
    if (((iVar6 < 0) || ((iVar6 < 1 && (uVar1 < local_1e)))) && (local_1e < 10000)) {
      local_1c = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
      local_1e = FUN_10a0_0baa();
    }
    if (((local_1c < 0) || (0 < local_1c)) || ((-1 < local_1c && (10000 < local_1e)))) {
      local_1e = 0;
      local_1c = 0;
    }
    iVar6 = DAT_10a8_38be - (uint)(DAT_10a8_38bc == 0);
    uVar1 = FUN_10a0_0baa();
    if (((iVar6 < 0) || ((iVar6 < 1 && (uVar1 < local_22)))) && (local_22 < 10000)) {
      local_20 = DAT_10a8_38be - (uint)(DAT_10a8_38bc == 0);
      local_22 = FUN_10a0_0baa();
    }
    if (((local_20 < 0) || (0 < local_20)) || ((-1 < local_20 && (10000 < local_22)))) {
      local_22 = 0;
      local_20 = 0;
    }
    uVar9 = GETDC((char *)s_RandomShrs_10a8_109f + 1,*(undefined2 *)(iVar7 + 4));
    uVar2 = CREATECOMPATIBLEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar9);
    uVar3 = CREATECOMPATIBLEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar9);
    uVar4 = CREATECOMPATIBLEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar9);
    uVar5 = CREATECOMPATIBLEBITMAP((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_38b4,DAT_10a8_38b0)
    ;
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar7 + 0x4b));
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar7 + 0x49));
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar5);
    if (*(char *)(iVar7 + 0x48) == '\0') {
      uVar11 = uVar3;
      BITBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,local_22,local_1e,uVar9,DAT_10a8_38b4,
             DAT_10a8_38b0,0,0);
    }
    else {
      uVar11 = uVar4;
      BITBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,0,0,uVar3,DAT_10a8_38b4,DAT_10a8_38b0,0,
             0);
      BITBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,0,0,uVar2,DAT_10a8_38b4,DAT_10a8_38b0,
             local_22 - *(int *)(iVar7 + 0x53),local_1e - *(int *)(iVar7 + 0x4f));
      BITBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,0,0,uVar3,DAT_10a8_38b4,DAT_10a8_38b0,
             *(undefined2 *)(iVar7 + 0x53),*(undefined2 *)(iVar7 + 0x4f));
      BITBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,local_22,local_1e,uVar9,DAT_10a8_38b4,
             DAT_10a8_38b0,0,0);
      BITBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,0,0,uVar4,DAT_10a8_38b4,DAT_10a8_38b0,
             *(int *)(iVar7 + 0x53) - local_22,*(int *)(iVar7 + 0x4f) - local_1e);
    }
    uVar10 = uVar9;
    BITBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,0,0,uVar2,DAT_10a8_38b4,DAT_10a8_38b0,
           local_22,local_1e);
    *(uint *)(iVar7 + 0x4f) = local_1e;
    *(int *)(iVar7 + 0x51) = local_1c;
    *(uint *)(iVar7 + 0x53) = local_22;
    *(int *)(iVar7 + 0x55) = local_20;
    *(undefined1 *)(iVar7 + 0x48) = 1;
    uVar8 = *(undefined2 *)(iVar7 + 4);
    RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar9);
    DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar2,uVar8,uVar10,uVar11);
    DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar3);
    DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar4);
    DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar5);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_61d8 @ 1008:61d8 */
/* ======================================================== */

void __stdcall16far FUN_1008_61d8(undefined4 param_1)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  int iVar9;
  int unaff_BP;
  int iVar10;
  undefined2 uVar11;
  undefined2 unaff_SS;
  undefined2 uVar12;
  undefined2 uVar13;
  undefined2 uVar14;
  int local_5a [16];
  int local_3a;
  int local_38;
  int local_36;
  int local_34;
  undefined2 local_32;
  undefined2 local_30;
  undefined2 local_2e;
  char local_2b;
  undefined1 local_2a [2];
  int local_28;
  int local_26;
  int local_1c;
  int local_1a;
  int local_18;
  uint local_16;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar11 = (undefined2)((ulong)param_1 >> 0x10);
  iVar10 = (int)param_1;
  uVar3 = *(undefined2 *)(iVar10 + 4);
  local_30 = GETDC();
  local_2e = CREATECOMPATIBLEDC((char *)s__Show_Graph_10a8_1226 + 10,local_30,uVar3);
  local_32 = CREATECOMPATIBLEDC((char *)s__Show_Graph_10a8_1226 + 10,local_30);
  uVar3 = *(undefined2 *)(iVar10 + 4);
  GETCLIENTRECT((char *)s__Show_Graph_10a8_1226 + 10,&local_1c,unaff_SS);
  uVar8 = local_32;
  SETSTRETCHBLTMODE((char *)s__Show_Graph_10a8_1226 + 10,3);
  DAT_10a8_399c = (int)(local_16 - local_1a) / 0x12;
  DAT_10a8_399e = (int)DAT_10a8_399c >> 0xf;
  if ((DAT_10a8_399e < 0) || ((DAT_10a8_399e < 1 && (DAT_10a8_399c < 0x1e)))) {
    DAT_10a8_399c = 0x1e;
    DAT_10a8_399e = 0;
  }
  DAT_10a8_38b8 = DAT_10a8_22b2 + 3;
  DAT_10a8_38ba = DAT_10a8_22b4 + (uint)(0xfffc < DAT_10a8_22b2);
  iVar6 = local_18 - local_1c >> 0xf;
  DAT_10a8_38b0 = FUN_10a0_0be7(uVar8,uVar3);
  DAT_10a8_38bc = DAT_10a8_22b6 + 2;
  DAT_10a8_38be = DAT_10a8_22b8 + (uint)(0xfffd < DAT_10a8_22b6);
  iVar7 = (((int)(local_16 - local_1a) >> 0xf) - DAT_10a8_399e) -
          (uint)(local_16 - local_1a < DAT_10a8_399c);
  DAT_10a8_38b2 = iVar6;
  DAT_10a8_38b4 = FUN_10a0_0be7();
  DAT_10a8_38b6 = iVar7;
  FUN_1008_3d8d(iVar10,uVar11);
  uVar8 = local_30;
  SELECTOBJECT((char *)s_FALSE_10a8_1003 + 5,DAT_10a8_378e);
  uVar3 = local_30;
  GETTEXTMETRICS((char *)s__Show_Graph_10a8_1226 + 10,local_5a,unaff_SS);
  DAT_10a8_38c2 = local_5a[0] >> 0xf;
  DAT_10a8_38c0 = local_5a[0];
  local_2b = '\0';
  while( true ) {
    DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(local_2b * 2 + 0x2296),uVar3,
                 uVar8);
    GETOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_2a,unaff_SS,0xe);
    local_34 = local_28 >> 0xf;
    local_36 = local_28;
    local_38 = local_26 >> 0xf;
    local_3a = local_26;
    uVar14 = local_30;
    uVar1 = CREATECOMPATIBLEBITMAP((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_38b4,DAT_10a8_38b0)
    ;
    *(undefined2 *)(local_2b * 2 + 0x2296) = uVar1;
    uVar13 = local_32;
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(local_2b * 2 + 0x2296));
    uVar12 = local_2e;
    SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(local_2b * 2 + 0x2282));
    uVar1 = local_32;
    STRETCHBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,local_3a,local_36,0,0,local_2e,
               DAT_10a8_38b4,DAT_10a8_38b0,0,0);
    if (local_2b == '\t') break;
    local_2b = local_2b + '\x01';
  }
  DELETEOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_2280,uVar1,uVar12,uVar13,uVar14);
  GETOBJECT((char *)s__Show_Graph_10a8_1226 + 10,local_2a,unaff_SS,0xe);
  local_34 = local_28 >> 0xf;
  local_36 = local_28;
  local_38 = local_26 >> 0xf;
  local_3a = local_26;
  DAT_10a8_2280 =
       CREATECOMPATIBLEBITMAP((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_38b4,DAT_10a8_38b0);
  uVar12 = local_32;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_2280);
  uVar1 = local_2e;
  SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,DAT_10a8_227e);
  uVar8 = local_32;
  STRETCHBLT((char *)s__Show_Graph_10a8_1226 + 10,0x20,0xcc,local_3a,local_36,0,0,local_2e,
             DAT_10a8_38b4,DAT_10a8_38b0,0,0);
  uVar3 = *(undefined2 *)(iVar10 + 4);
  RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,local_30);
  DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_2e,uVar3,uVar8,uVar1,uVar12);
  DELETEDC((char *)s__Show_Graph_10a8_1226 + 10,local_32);
  uVar8 = 0;
  iVar6 = DAT_10a8_38b2;
  iVar2 = FUN_10a0_0baa();
  iVar7 = DAT_10a8_38b2;
  uVar3 = FUN_10a0_1458(iVar2,iVar6);
  uVar5 = 2;
  iVar9 = 0;
  FUN_10a0_0be7(uVar3,uVar8,iVar7);
  iVar6 = DAT_10a8_22b0;
  uVar4 = FUN_10a0_0be7(uVar5,iVar9);
  iVar6 = iVar6 + iVar9 + (uint)CARRY2(uVar4,uVar5);
  uVar3 = FUN_10a0_1458();
  FUN_10a0_1458(uVar3,iVar9,iVar6);
  FUN_10a0_1444();
  FUN_10a0_1444();
  FUN_10a0_1432();
  iVar6 = FUN_10a0_145c();
  DAT_10a8_38cc = iVar6 + iVar2;
  DAT_10a8_38ce = FUN_10a0_0baa();
  iVar6 = FUN_10a0_0baa();
  DAT_10a8_38d0 = iVar6 + -2;
  iVar6 = FUN_10a0_0baa();
  DAT_10a8_38d2 = ((local_16 - local_1a) + DAT_10a8_38ce) - iVar6;
  DAT_10a8_38d4 = FUN_10a0_0baa();
  DAT_10a8_38d6 = 0;
  iVar6 = FUN_10a0_0baa();
  DAT_10a8_38d8 = DAT_10a8_38d4 + iVar6;
  iVar6 = FUN_10a0_0baa();
  DAT_10a8_38da = DAT_10a8_38d6 + iVar6;
  FUN_1058_026e(DAT_10a8_399c,DAT_10a8_399e,local_18,local_18 >> 0xf,local_16 - DAT_10a8_399c,
                (((int)local_16 >> 0xf) - DAT_10a8_399e) - (uint)(local_16 < DAT_10a8_399c),0,0,
                iVar10,uVar11);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_65bd @ 1008:65bd */
/* ======================================================== */

void __stdcall16far FUN_1008_65bd(int *param_1,undefined4 param_2)

{
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  char *pcVar4;
  
  iVar1 = unaff_BP + 1;
  pcVar4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar2 = (undefined2)((ulong)param_2 >> 0x10);
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  FUN_1078_18f5((int *)param_1,uVar3,(int)param_2,uVar2);
  DAT_10a8_1cfd = *(int *)((int)param_2 + 4) == 2;
  (*(code *)*(undefined2 *)(*param_1 + 0x58))
            ((char *)s_Blanks_10a8_1072 + 6,(int *)param_1,uVar3,pcVar4,iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_65f9 @ 1008:65f9 */
/* ======================================================== */

void __cdecl16near FUN_1008_65f9(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 in_BX;
  int iVar4;
  int iVar5;
  bool bVar6;
  char local_b;
  uint local_6;
  int local_4;
  
  DAT_10a8_22aa = 0;
  DAT_10a8_22ac = 0;
  local_6 = 1;
  local_4 = 0;
  while( true ) {
    if (*(char *)((undefined2 *)&DAT_10a8_38bc + local_6 * 0x10) != '\0') {
      iVar4 = (DAT_10a8_22aa + 1) * 0xfc;
      bVar6 = 0xfffe < DAT_10a8_22aa;
      DAT_10a8_22aa = DAT_10a8_22aa + 1;
      DAT_10a8_22ac = DAT_10a8_22ac + (uint)bVar6;
      FUN_1098_0055(local_6 * 0x20 + 0x38bd,(char *)s_RandomShrs_10a8_109f + 9,iVar4 + 0x30aa,
                    (char *)s_RandomShrs_10a8_109f + 9);
      if ((*(byte *)((undefined2 *)&DAT_10a8_38bc + local_6 * 0x10) < 2) ||
         (3 < *(byte *)((undefined2 *)&DAT_10a8_38bc + local_6 * 0x10))) {
        uVar1 = 0;
      }
      else {
        uVar1 = 1;
      }
      *(undefined1 *)(iVar4 + 0x30c9) = uVar1;
      uVar3 = DAT_10a8_22bc;
      uVar2 = FUN_10a0_1458();
      *(undefined2 *)(iVar4 + 0x30ca) = uVar2;
      *(undefined2 *)(iVar4 + 0x30cc) = in_BX;
      *(undefined2 *)(iVar4 + 0x30ce) = uVar3;
      FUN_1008_1e95(iVar4 + 0x30d0,(char *)s_RandomShrs_10a8_109f + 9);
      local_b = '\x01';
      while( true ) {
        iVar5 = iVar4 + 0x30aa + local_b * 4;
        *(undefined2 *)(iVar5 + 0xd8) = 0;
        *(undefined2 *)(iVar5 + 0xda) = 0;
        if (local_b == '\b') break;
        local_b = local_b + '\x01';
      }
    }
    if ((local_4 == 0) && (local_6 == 6)) break;
    bVar6 = 0xfffe < local_6;
    local_6 = local_6 + 1;
    local_4 = local_4 + (uint)bVar6;
  }
  if ((DAT_10a8_22ac < 0) || ((DAT_10a8_22ac < 1 && (DAT_10a8_22aa == 0)))) {
    DAT_10a8_22aa = 1;
    DAT_10a8_22ac = 0;
    DAT_10a8_38dc = 1;
    uVar3 = DAT_10a8_22bc;
    DAT_10a8_31c6 = FUN_10a0_1458();
    DAT_10a8_31c8 = in_BX;
    DAT_10a8_31ca = uVar3;
    FUN_1008_1e95(0x31cc,(char *)s_RandomShrs_10a8_109f + 9);
    DAT_10a8_31c5 = 0;
    local_b = '\x01';
    while( true ) {
      *(undefined2 *)(local_b * 4 + 0x327e) = 0;
      *(undefined2 *)(local_b * 4 + 0x3280) = 0;
      if (local_b == '\b') break;
      local_b = local_b + '\x01';
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_67c5 @ 1008:67c5 */
/* ======================================================== */

void __stdcall16far FUN_1008_67c5(undefined4 param_1,undefined4 param_2)

{
  int unaff_BP;
  int iVar1;
  undefined2 uVar2;
  undefined2 unaff_SS;
  undefined1 local_42 [30];
  char local_24;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_10a0_0b92(0x26,0x1cfe,(char *)s_RandomShrs_10a8_109f + 9,(undefined2 *)&DAT_10a8_22aa,
                (char *)s_RandomShrs_10a8_109f + 9);
  DAT_10a8_1d2e = 1;
  FUN_10a0_0b92(0x26,local_42,unaff_SS,(undefined2 *)&DAT_10a8_22aa,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1070_0002(local_42,unaff_SS);
  iVar1 = (int)param_1;
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if (local_24 == '\0') {
    FUN_10a0_0b92(0x26,(undefined2 *)&DAT_10a8_22aa,(char *)s_RandomShrs_10a8_109f + 9,local_42,
                  unaff_SS);
    DAT_10a8_38b8 = DAT_10a8_22b2 + 3;
    DAT_10a8_38ba = DAT_10a8_22b4 + (uint)(0xfffc < DAT_10a8_22b2);
    DAT_10a8_38bc = DAT_10a8_22b6 + 2;
    DAT_10a8_38be = DAT_10a8_22b8 + (uint)(0xfffd < DAT_10a8_22b6);
    *(undefined1 *)(iVar1 + 0x47) = 0;
    DAT_10a8_39a8 = 0;
    FUN_10a0_1e88((char *)s_RandomShrs_10a8_109f + 1,iVar1,uVar2,(int)param_2,
                  (int)((ulong)param_2 >> 0x10));
  }
  else {
    DAT_10a8_1d2e = 0;
    INVALIDATERECT((char *)s_PurchaseLimitNumber_10a8_105e + 0x12,1,0,0,*(undefined2 *)(iVar1 + 4));
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_687f @ 1008:687f */
/* ======================================================== */

void __stdcall16far FUN_1008_687f(int *param_1)

{
  char cVar1;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  int *piVar2;
  undefined2 uVar3;
  bool bVar4;
  bool bVar5;
  int iVar6;
  uint local_8;
  uint local_6;
  
  DAT_10a8_1d2e = 1;
  cVar1 = FUN_1070_02d3((undefined1 *)&DAT_10a8_38dc,(char *)s_RandomShrs_10a8_109f + 9);
  piVar2 = (int *)param_1;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  if (cVar1 == '\0') {
    DAT_10a8_1d2e = 0;
    FUN_10a0_0b92(0x26,(undefined2 *)&DAT_10a8_22aa,(char *)s_RandomShrs_10a8_109f + 9,0x1cfe,
                  (char *)s_RandomShrs_10a8_109f + 9);
    INVALIDATERECT((char *)s_RandomShrs_10a8_109f + 1,0,0,0,piVar2[2]);
  }
  else {
    bVar5 = false;
    *(undefined1 *)((int)piVar2 + 0x47) = 0;
    DAT_10a8_1d31 = 0;
    DAT_10a8_39a8 = 0;
    DAT_10a8_319a = 1;
    DAT_10a8_319c = 0;
    DAT_10a8_319e = 0;
    DAT_10a8_31a0 = 0;
    DAT_10a8_31a2 = 0;
    DAT_10a8_31a4 = 0;
    FUN_1048_1432();
    FUN_1048_10c1(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
    FUN_1008_6752();
    FUN_1008_65f9();
    DAT_10a8_1d2f = 0;
    (*(code *)*(undefined2 *)(*param_1 + 0x54))
              ((char *)s_StartCash_10a8_1043 + 5,piVar2,uVar3,CONCAT11(extraout_AH,DAT_10a8_39a8));
    if ((DAT_10a8_22ac < 0x8000) && ((0 < (int)DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
      local_8 = 1;
      local_6 = 0;
      while( true ) {
        if (*(char *)(local_8 * 0xfc + 0x30c9) != '\0') {
          bVar5 = true;
        }
        if ((local_6 == DAT_10a8_22ac) && (local_8 == DAT_10a8_22aa)) break;
        bVar4 = 0xfffe < local_8;
        local_8 = local_8 + 1;
        local_6 = local_6 + bVar4;
      }
    }
    if ((bVar5) && (DAT_10a8_22c6 != '\0')) {
      MESSAGEBEEP((char *)s_StartCash_10a8_1043 + 5,0);
      iVar6 = piVar2[2];
      MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x5b6,(char *)s_RandomShrs_10a8_109f + 9,
                 0x58c,(char *)s_RandomShrs_10a8_109f + 9);
      DAT_10a8_22c6 = '\0';
      FUN_1048_1432(iVar6);
      FUN_1048_10c1(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
      FUN_1008_6752();
      FUN_1008_65f9();
    }
    DAT_10a8_39a2 = 1;
    if ((DAT_10a8_22ac < 0x8000) && ((0 < (int)DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
      local_8 = 1;
      local_6 = 0;
      while( true ) {
        if (*(char *)(local_8 * 0xfc + 0x30c9) == '\0') {
          DAT_10a8_39a2 = 0;
        }
        if ((local_6 == DAT_10a8_22ac) && (local_8 == DAT_10a8_22aa)) break;
        bVar5 = 0xfffe < local_8;
        local_8 = local_8 + 1;
        local_6 = local_6 + bVar5;
      }
    }
    if (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) != '\0') {
      DAT_10a8_1d31 = 1;
    }
    DAT_10a8_1d2e = 0;
    (*(code *)*(undefined2 *)(*param_1 + 0x58))((char *)s_StartCash_10a8_1043 + 5,piVar2,uVar3);
    (*(code *)*(undefined2 *)(*param_1 + 0x5c))((char *)s_StartCash_10a8_1043 + 5,piVar2,uVar3);
    INVALIDATERECT((char *)s_StartCash_10a8_1043 + 5,1,0,0,piVar2[2]);
    FUN_1028_0002(CONCAT11(extraout_AH_00,DAT_10a8_39a7),1,DAT_10a8_22aa,0x31a6,
                  (char *)s_RandomShrs_10a8_109f + 9);
    if (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) != '\0') {
      POSTMESSAGE((char *)s_NumPlayers_10a8_1022 + 6,0,0,0,0x4ef);
    }
    FUN_10a0_0b92(0x26,0x1cfe,(char *)s_RandomShrs_10a8_109f + 9,(undefined2 *)&DAT_10a8_22aa,
                  (char *)s_RandomShrs_10a8_109f + 9);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6a95 @ 1008:6a95 */
/* ======================================================== */

undefined4 __stdcall16far
FUN_1008_6a95(undefined4 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
             undefined2 param_5,undefined2 param_6)

{
  uint uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  int iVar5;
  undefined2 uVar6;
  bool bVar7;
  undefined2 uVar8;
  uint local_c;
  int local_a;
  uint local_8;
  int local_6;
  
  bVar7 = true;
  FUN_10a0_03ef();
  if (!bVar7) {
    uVar6 = (undefined2)((ulong)param_1 >> 0x10);
    iVar5 = (int)param_1;
    FUN_1078_1106(iVar5,uVar6,0,param_3,param_4,param_5,param_6);
    FUN_10a0_0b92(0x26,(undefined2 *)&DAT_10a8_22aa,(char *)s_RandomShrs_10a8_109f + 9,0x19c6,
                  (char *)s_RandomShrs_10a8_109f + 9);
    DAT_10a8_38b8 = 0x12;
    DAT_10a8_38ba = 0;
    DAT_10a8_38bc = 0xe;
    DAT_10a8_38be = 0;
    local_8 = 0;
    local_6 = 0;
    while( true ) {
      local_c = 0;
      local_a = 0;
      while( true ) {
        FUN_1048_0079(local_8 * 0x8c + local_c * 5 + 0x22d0,(char *)s_RandomShrs_10a8_109f + 9,0xce8
                      ,local_c & 0xff,local_8 & 0xff);
        if ((local_a == 0) && (local_c == 0x19)) break;
        bVar7 = 0xfffe < local_c;
        local_c = local_c + 1;
        local_a = local_a + (uint)bVar7;
      }
      if ((local_6 == 0) && (local_8 == 0x19)) break;
      bVar7 = 0xfffe < local_8;
      local_8 = local_8 + 1;
      local_6 = local_6 + (uint)bVar7;
    }
    FUN_1048_0cf2();
    uVar3 = DAT_10a8_38ba;
    uVar2 = FUN_10a0_0be7();
    GETSYSTEMMETRICS((char *)s_RandomShrs_10a8_109f + 1,1,uVar2,uVar3);
    uVar3 = FUN_10a0_0baa();
    *(undefined2 *)(iVar5 + 0x2d) = uVar3;
    GETSYSTEMMETRICS((char *)s_RandomShrs_10a8_109f + 1,1);
    FUN_10a0_1458();
    FUN_10a0_1444();
    uVar3 = FUN_10a0_145c();
    *(undefined2 *)(iVar5 + 0x2f) = uVar3;
    *(undefined2 *)(iVar5 + 0x29) = 0x1e;
    *(undefined2 *)(iVar5 + 0x2b) = 0x1e;
    uVar1 = *(uint *)(iVar5 + 0x23);
    *(undefined2 *)(iVar5 + 0x21) = *(undefined2 *)(iVar5 + 0x21);
    *(uint *)(iVar5 + 0x23) = uVar1 | 0x200;
    uVar8 = DAT_10a8_1c9e;
    uVar3 = LOADMENU((char *)s_RandomShrs_10a8_109f + 1,0x5ce,(char *)s_RandomShrs_10a8_109f + 9);
    *(undefined2 *)(iVar5 + 0x41) = uVar3;
    uVar2 = DAT_10a8_1c9e;
    uVar3 = LOADMENU((char *)s__Show_Graph_10a8_1226 + 10,0x5d8,(char *)s_RandomShrs_10a8_109f + 9);
    *(undefined2 *)(iVar5 + 0x43) = uVar3;
    uVar3 = DAT_10a8_1c9e;
    uVar4 = LOADMENU((char *)s__Show_Graph_10a8_1226 + 10,0x5e1,(char *)s_RandomShrs_10a8_109f + 9);
    *(undefined2 *)(iVar5 + 0x45) = uVar4;
    *(undefined2 *)(iVar5 + 0x35) = *(undefined2 *)(iVar5 + 0x41);
    DAT_10a8_378e = 0;
    *(undefined2 *)(iVar5 + 0x4f) = 0xffff;
    *(undefined2 *)(iVar5 + 0x51) = 0xffff;
    DAT_10a8_39a3 = 0;
    *(undefined1 *)(iVar5 + 0x47) = 0;
    DAT_10a8_1d31 = 1;
    DAT_10a8_1d2e = 0;
    *(undefined1 *)(iVar5 + 0x48) = 0;
    DAT_10a8_39a8 = 0;
    DAT_10a8_38c0 = 10;
    DAT_10a8_38c2 = 0;
    DAT_10a8_39a7 = 0;
    DAT_10a8_319a = 1;
    DAT_10a8_319c = 0;
    DAT_10a8_22cf = 0;
    FUN_1048_1432(uVar3,uVar2,uVar8);
    FUN_10a0_0b92(0xc0,(undefined1 *)&DAT_10a8_38dc,(char *)s_RandomShrs_10a8_109f + 9,0x19ec,
                  (char *)s_RandomShrs_10a8_109f + 9);
    FUN_1008_65f9();
    FUN_1048_10c1(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
    DAT_10a8_1d2f = 1;
    *(undefined2 *)(iVar5 + 0x57) = 0;
    *(undefined2 *)(iVar5 + 0x59) = 0;
    DAT_10a8_39a0 = CREATEBRUSHINDIRECT((char *)s_StartCash_10a8_1043 + 5,0x19be);
    FUN_1048_1db2((undefined2 *)&DAT_10a8_22aa,(char *)s_RandomShrs_10a8_109f + 9);
    DAT_10a8_22aa = 2;
    DAT_10a8_22ac = 0;
  }
  return param_1;
}

/* ======================================================== */
/* Function: FUN_1008_6cc8 @ 1008:6cc8 */
/* ======================================================== */

void __stdcall16far FUN_1008_6cc8(int *param_1)

{
  byte extraout_AH;
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int iVar4;
  int unaff_BP;
  int *piVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  undefined2 uVar7;
  int *piVar8;
  undefined2 local_e;
  int local_c;
  undefined2 local_a;
  int local_8;
  undefined2 local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_1008_1aa7();
  if (DAT_10a8_39a4 == '\0') {
    iVar1 = (uint)extraout_AH * 0x100;
    if (DAT_10a8_1d31 == '\0') {
      iVar1 = iVar1 + 1;
    }
    DAT_10a8_1d31 = (char)iVar1;
    uVar3 = CONCAT11((char)((uint)iVar1 >> 8),DAT_10a8_39a8);
    uVar6 = (undefined2)((ulong)param_1 >> 0x10);
    piVar5 = (int *)param_1;
    piVar8 = piVar5;
    uVar2 = uVar6;
    (*(code *)*(undefined2 *)(*param_1 + 0x54))();
    if (DAT_10a8_1d31 == '\0') {
      local_c = FUN_10a0_0baa();
      local_8 = FUN_10a0_0baa();
      local_8 = local_c + local_8;
      local_e = 0;
      local_a = FUN_10a0_0baa();
      INVALIDATERECT((char *)s_RandomShrs_10a8_109f + 1,1,&local_e,unaff_SS,piVar5[2]);
    }
    else {
      iVar1 = piVar5[2];
      local_6 = GETDC();
      uVar2 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,1,local_6,iVar1,piVar8,uVar2,uVar3
                            );
      SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar2);
      uVar2 = GETSTOCKOBJECT((char *)s__Show_Graph_10a8_1226 + 10,8,local_6);
      SELECTOBJECT((char *)s__Show_Graph_10a8_1226 + 10,uVar2);
      uVar7 = 0;
      uVar3 = FUN_10a0_0baa(0,local_6);
      uVar6 = FUN_10a0_0baa(uVar3);
      uVar2 = DAT_10a8_38b6;
      iVar1 = FUN_10a0_0baa(uVar6);
      iVar4 = FUN_10a0_0baa(iVar1,uVar2);
      RECTANGLE((char *)s_RandomShrs_10a8_109f + 1,iVar4 + iVar1 + 1,uVar6,uVar3,uVar7);
      RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,local_6);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6e18 @ 1008:6e18 */
/* ======================================================== */

void __stdcall16far FUN_1008_6e18(undefined4 param_1,uint *param_2)

{
  uint uVar1;
  uint *puVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  puVar2 = (uint *)param_2;
  FUN_1078_1274((int)param_1,(int)((ulong)param_1 >> 0x10),puVar2,uVar3);
  uVar1 = LOADICON((char *)s_Blanks_10a8_1072 + 6,0x5ec,(char *)s_RandomShrs_10a8_109f + 9);
  puVar2[6] = uVar1;
  uVar1 = CREATEBRUSHINDIRECT((char *)s__Show_Graph_10a8_1226 + 10,0x19be);
  puVar2[8] = uVar1;
  *param_2 = *param_2 | 8;
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6e82 @ 1008:6e82 */
/* ======================================================== */

void __stdcall16far FUN_1008_6e82(undefined4 param_1)

{
  FUN_1078_170c((int)param_1,(int)((ulong)param_1 >> 0x10));
  POSTMESSAGE((char *)s_Blanks_10a8_1072 + 6,0,0,0,0x4e3);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6ead @ 1008:6ead */
/* ======================================================== */

void __cdecl16near FUN_1008_6ead(void)

{
  undefined2 unaff_CS;
  
  GETPROFILESTRING(unaff_CS,0x97,(undefined1 *)&DAT_10a8_39aa,(char *)s_RandomShrs_10a8_109f + 9,
                   0x60d,(char *)s_RandomShrs_10a8_109f + 9,0x608,(char *)s_RandomShrs_10a8_109f + 9
                   ,0x5fe);
  GETPROFILESTRING((char *)s__Show_Graph_10a8_1226 + 10,0x97,(undefined1 *)&DAT_10a8_3a42,
                   (char *)s_RandomShrs_10a8_109f + 9,0x60d,(char *)s_RandomShrs_10a8_109f + 9,0x612
                   ,(char *)s_RandomShrs_10a8_109f + 9,0x5fe);
  DAT_10a8_3ada =
       GETPROFILEINT((char *)s__Show_Graph_10a8_1226 + 10,0,0x61a,(char *)s_RandomShrs_10a8_109f + 9
                     ,0x5fe);
  DAT_10a8_3adc = 0;
  DAT_10a8_39a6 = (uint)DAT_10a8_39aa + (uint)DAT_10a8_3a42 + 0xf == DAT_10a8_3ada;
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6f2e @ 1008:6f2e */
/* ======================================================== */

void __stdcall16far FUN_1008_6f2e(int *param_1)

{
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  undefined1 uVar1;
  int unaff_BP;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_CS;
  
  FUN_1008_6ead((char *)s_RandomShrs_10a8_109f + 9,unaff_BP + 1);
  piVar2 = (int *)param_1;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  uVar1 = extraout_AH;
  if (DAT_10a8_39a6 == '\0') {
    unaff_CS = 0x1040;
    FUN_1040_0002(piVar2,uVar3);
    uVar1 = extraout_AH_00;
  }
  (*(code *)*(undefined2 *)(*param_1 + 0x54))(unaff_CS,piVar2,uVar3,CONCAT11(uVar1,DAT_10a8_39a8));
  (*(code *)*(undefined2 *)(*param_1 + 0x58))(unaff_CS,piVar2,uVar3);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6f68 @ 1008:6f68 */
/* ======================================================== */

void __stdcall16far FUN_1008_6f68(void)

{
  undefined2 unaff_CS;
  
  WINHELP(unaff_CS,0,0,3,0x620,(char *)s_RandomShrs_10a8_109f + 9);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_6f8f @ 1008:6f8f */
/* ======================================================== */

void __stdcall16far FUN_1008_6f8f(undefined4 param_1)

{
  int unaff_BP;
  undefined2 unaff_SS;
  int local_15a [19];
  undefined1 local_134;
  undefined1 local_133 [151];
  undefined1 local_9c [152];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_1080_0002(local_15a,unaff_SS,0x1632,0x62e,(char *)s_RandomShrs_10a8_109f + 9,(int)param_1,
                (int)((ulong)param_1 >> 0x10));
  local_134 = DAT_10a8_39a6;
  FUN_10a0_0b92(0x97,local_133,unaff_SS,(undefined1 *)&DAT_10a8_39aa,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_10a0_0b92(0x97,local_9c,unaff_SS,(undefined1 *)&DAT_10a8_3a42,
                (char *)s_RandomShrs_10a8_109f + 9);
  (*(code *)*(undefined2 *)(local_15a[0] + 0x4c))((char *)s_RandomShrs_10a8_109f + 1,local_15a);
  (*(code *)*(undefined2 *)(local_15a[0] + 8))((char *)s_RandomShrs_10a8_109f + 1,local_15a);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7000 @ 1008:7000 */
/* ======================================================== */

void __stdcall16far FUN_1008_7000(void)

{
  FUN_10a0_1e88();
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7101 @ 1008:7101 */
/* ======================================================== */

void __stdcall16far FUN_1008_7101(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x480);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7122 @ 1008:7122 */
/* ======================================================== */

void __stdcall16far FUN_1008_7122(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  int unaff_BP;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  char *pcVar6;
  
  iVar2 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  DAT_10a8_22c9 = DAT_10a8_22c9 == '\0';
  iVar3 = (int)param_1;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  if ((bool)DAT_10a8_22c9) {
    uVar5 = *(undefined2 *)(iVar3 + 4);
    uVar1 = GETMENU();
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0x6c);
  }
  else {
    uVar5 = *(undefined2 *)(iVar3 + 4);
    uVar1 = GETMENU();
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x6c);
  }
  if (((DAT_10a8_1d31 == '\0') && (DAT_10a8_22c9 != '\0')) && (DAT_10a8_39a8 != '\0')) {
    FUN_10a0_1e88((char *)s__Show_Graph_10a8_1226 + 10,iVar3,uVar4,(int)param_2,
                  (int)((ulong)param_2 >> 0x10),uVar1,uVar5,pcVar6,iVar2);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_719a @ 1008:719a */
/* ======================================================== */

void __stdcall16far FUN_1008_719a(undefined4 param_1)

{
  DAT_10a8_22cf = DAT_10a8_22cf == '\0';
  if ((bool)DAT_10a8_22cf) {
    GETMENU();
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0xdf);
  }
  else {
    GETMENU();
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0xdf);
  }
  INVALIDATERECT((char *)s__Show_Graph_10a8_1226 + 10,0,0,0,*(undefined2 *)((int)param_1 + 4));
  return;
}

/* ======================================================== */
/* Function: FUN_1008_71fc @ 1008:71fc */
/* ======================================================== */

void __stdcall16far FUN_1008_71fc(undefined4 param_1,char param_2)

{
  undefined2 uVar1;
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  undefined1 uVar2;
  int unaff_BP;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_CS;
  char *pcVar6;
  
  iVar3 = unaff_BP + 1;
  pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
  iVar4 = (int)param_1;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  if ((*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\0') || (DAT_10a8_1d2f != '\0')) {
    uVar1 = *(undefined2 *)(iVar4 + 4);
    SETMENU(unaff_CS,*(undefined2 *)(iVar4 + 0x41));
  }
  else {
    uVar1 = *(undefined2 *)(iVar4 + 4);
    SETMENU(unaff_CS,*(undefined2 *)(iVar4 + 0x43));
  }
  if (DAT_10a8_22cf == '\0') {
    uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1,pcVar6,
                    iVar3);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0xdf);
  }
  else {
    uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1,pcVar6,
                    iVar3);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0xdf);
  }
  if (DAT_10a8_39a7 == '\0') {
    uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0xe0);
  }
  else {
    uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0xe0);
  }
  if (DAT_10a8_22ce == '\0') {
    uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0xe2);
    uVar2 = extraout_AH_00;
  }
  else {
    uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
    CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0xe2);
    uVar2 = extraout_AH;
  }
  FUN_1058_04ee(CONCAT11(uVar2,DAT_10a8_1d31));
  if ((DAT_10a8_1d2f != '\0') || (*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) == '\0')) {
    if (DAT_10a8_1d2f == '\0') {
      if (param_2 == '\0') {
        uVar1 = GETMENU((char *)s_ShareLimitNumber_10a8_104d + 0xb,*(undefined2 *)(iVar4 + 4),uVar1)
        ;
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0x6b);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x6a);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0x68);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x6d);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0xc4);
        if (DAT_10a8_22c6 == '\0') {
          uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
          ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0xe1);
        }
        else {
          uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
          ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0xe1);
        }
      }
      else {
        uVar1 = GETMENU((char *)s_ShareLimitNumber_10a8_104d + 0xb,*(undefined2 *)(iVar4 + 4),uVar1)
        ;
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x6b);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0x6a);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x68);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0x6d);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0xc4);
        uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
        ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0xe1);
      }
    }
    else {
      uVar1 = GETMENU((char *)s_ShareLimitNumber_10a8_104d + 0xb,*(undefined2 *)(iVar4 + 4),uVar1);
      ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0x6b);
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0x6a);
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0x68);
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0x6d);
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0xc4);
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0xe1);
    }
    if (DAT_10a8_22c9 == '\0') {
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0x6c);
    }
    else {
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      CHECKMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,8,0x6c);
    }
    if ((DAT_10a8_1d31 == '\0') ||
       ((*(char *)(DAT_10a8_319a * 0xfc + 0x30c9) != '\0' && (DAT_10a8_1d2f == '\0')))) {
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      MODIFYMENU((char *)s__Show_Graph_10a8_1226 + 10,0x646,(char *)s_RandomShrs_10a8_109f + 9,0x69,
                 0,0x69);
    }
    else {
      uVar1 = GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      MODIFYMENU((char *)s__Show_Graph_10a8_1226 + 10,0x634,(char *)s_RandomShrs_10a8_109f + 9,0x69,
                 0,0x69);
    }
    if (DAT_10a8_39a6 == '\0') {
      GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,0,0xb6);
    }
    else {
      GETMENU((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar1);
      ENABLEMENUITEM((char *)s__Show_Graph_10a8_1226 + 10,1,0xb6);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7591 @ 1008:7591 */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10087797) */
/* WARNING: Removing unreachable block (ram,0x1008764d) */
/* WARNING: Removing unreachable block (ram,0x10087779) */
/* WARNING: Removing unreachable block (ram,0x10087614) */

undefined2 __stdcall16far FUN_1008_7591(undefined4 param_1)

{
  undefined2 *puVar1;
  char cVar2;
  uint uVar3;
  uint uVar4;
  undefined1 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  char *pcVar10;
  bool bVar11;
  long lVar12;
  undefined1 local_17;
  uint local_16;
  int local_14;
  uint local_12;
  int local_10;
  uint local_e;
  int local_c;
  uint local_a;
  int local_8;
  
  local_17 = 1;
  lVar12 = FUN_1048_1035(0x3790,(char *)s_RandomShrs_10a8_109f + 9);
  uVar3 = (uint)lVar12;
  if (lVar12 == 1) {
    iVar6 = DAT_10a8_22b4;
    FUN_10a0_0baa();
    pcVar10 = (char *)s_RandomShrs_10a8_109f + 1;
    uVar3 = FUN_10a0_0be7();
    if ((iVar6 < DAT_10a8_31a0) || ((iVar6 <= DAT_10a8_31a0 && (uVar3 < DAT_10a8_319e)))) {
      DAT_10a8_1d2e = 1;
      uVar3 = DAT_10a8_22b2 - 1;
      iVar6 = DAT_10a8_22b4 - (uint)(DAT_10a8_22b2 == 0);
      local_a = 0;
      if (-1 < iVar6) {
        local_a = 0;
        local_8 = 0;
        do {
          uVar4 = DAT_10a8_22b6 - 1;
          iVar7 = DAT_10a8_22b8 - (uint)(DAT_10a8_22b6 == 0);
          if (-1 < iVar7) {
            local_e = 0;
            local_c = 0;
            do {
              if ((*(char *)(local_a * 0x8c + local_e * 5 + 0x22d2) == '\0') &&
                 (((char *)s_American_Natural_Gas_Co__10a8_193e + 2)
                  [(char)((char)local_a + 'A') * 0x1a + local_e] != '\x02')) {
                piVar9 = (int *)(local_a * 0x8c + local_e * 5 + 0x22d0);
                puVar1 = (undefined2 *)(*piVar9 + 0x10);
                (*(code *)*puVar1)(pcVar10,piVar9,(char *)s_RandomShrs_10a8_109f + 9,9);
                cVar2 = FUN_1008_1abd(0,local_e + 1,local_c + (uint)(0xfffe < local_e),local_a + 1,
                                      local_8 + (uint)(0xfffe < local_a));
                if (cVar2 == '\t') {
                  pcVar10 = (char *)s_FALSE_10a8_1003 + 5;
                  cVar2 = FUN_1008_51fd((int)param_1,(int)((ulong)param_1 >> 0x10),0,local_e,local_c
                                        ,local_a,local_8);
                  if (cVar2 == '\0') {
                    local_12 = 0xffff;
                    local_10 = -1;
                    while( true ) {
                      local_16 = 0xffff;
                      local_14 = -1;
                      while( true ) {
                        if (((((local_10 != local_14) || (local_12 != local_16)) &&
                             (local_12 + local_16 != 0 ||
                              local_10 + local_14 + (uint)CARRY2(local_12,local_16) != 0)) &&
                            ((iVar8 = local_8 + local_10 + (uint)CARRY2(local_a,local_12), 0 < iVar8
                             || (-1 < iVar8)))) &&
                           ((iVar8 = local_c + local_14 + (uint)CARRY2(local_e,local_16), 0 < iVar8
                            || (-1 < iVar8)))) {
                          iVar8 = local_8 + local_10 + (uint)CARRY2(local_a,local_12);
                          if ((iVar8 < DAT_10a8_22b4) ||
                             ((iVar8 <= DAT_10a8_22b4 && (local_a + local_12 < DAT_10a8_22b2)))) {
                            iVar8 = local_c + local_14 + (uint)CARRY2(local_e,local_16);
                            if ((((iVar8 < DAT_10a8_22b8) ||
                                 ((iVar8 <= DAT_10a8_22b8 && (local_e + local_16 < DAT_10a8_22b6))))
                                && (*(char *)((local_a + local_12) * 0x8c + (local_e + local_16) * 5
                                             + 0x22d2) == '\0')) &&
                               (((char *)s_American_Natural_Gas_Co__10a8_193e + 2)
                                [(char)((char)local_a + (char)local_12 + 'A') * 0x1a +
                                 local_e + local_16] != '\x02')) {
                              piVar9 = (int *)((local_a + local_12) * 0x8c +
                                               (local_e + local_16) * 5 + 0x22d0);
                              puVar1 = (undefined2 *)(*piVar9 + 0x10);
                              (*(code *)*puVar1)((char *)s_FALSE_10a8_1003 + 5,piVar9,
                                                 (char *)s_RandomShrs_10a8_109f + 9,9);
                              cVar2 = FUN_1008_1abd(0,local_e + local_16 + 1,
                                                    local_c + local_14 +
                                                    (uint)CARRY2(local_e,local_16) +
                                                    (uint)(0xfffe < local_e + local_16),
                                                    local_a + local_12 + 1,
                                                    local_8 + local_10 +
                                                    (uint)CARRY2(local_a,local_12) +
                                                    (uint)(0xfffe < local_a + local_12));
                              if (cVar2 == '\t') {
                                local_17 = 0;
                                piVar9 = (int *)((local_a + local_12) * 0x8c +
                                                 (local_e + local_16) * 5 + 0x22d0);
                                puVar1 = (undefined2 *)(*piVar9 + 0x10);
                                (*(code *)*puVar1)((char *)s_FALSE_10a8_1003 + 5,piVar9,
                                                   (char *)s_RandomShrs_10a8_109f + 9,0);
                                goto LAB_1008_793c;
                              }
                              piVar9 = (int *)((local_a + local_12) * 0x8c +
                                               (local_e + local_16) * 5 + 0x22d0);
                              puVar1 = (undefined2 *)(*piVar9 + 0x10);
                              (*(code *)*puVar1)((char *)s_FALSE_10a8_1003 + 5,piVar9,
                                                 (char *)s_RandomShrs_10a8_109f + 9,0);
                            }
                          }
                        }
                        if ((local_14 == 0) && (local_16 == 1)) break;
                        bVar11 = 0xfffe < local_16;
                        local_16 = local_16 + 1;
                        local_14 = local_14 + (uint)bVar11;
                      }
                      if ((local_10 == 0) && (local_12 == 1)) break;
                      bVar11 = 0xfffe < local_12;
                      local_12 = local_12 + 1;
                      local_10 = local_10 + (uint)bVar11;
                    }
                  }
                  else {
                    local_17 = 0;
                  }
                }
LAB_1008_793c:
                piVar9 = (int *)(local_a * 0x8c + local_e * 5 + 0x22d0);
                puVar1 = (undefined2 *)(*piVar9 + 0x10);
                (*(code *)*puVar1)(pcVar10,piVar9,(char *)s_RandomShrs_10a8_109f + 9,0);
              }
              if ((local_c == iVar7) && (local_e == uVar4)) break;
              bVar11 = 0xfffe < local_e;
              local_e = local_e + 1;
              local_c = local_c + (uint)bVar11;
            } while( true );
          }
          if ((local_8 == iVar6) && (local_a == uVar3)) break;
          bVar11 = 0xfffe < local_a;
          local_a = local_a + 1;
          local_8 = local_8 + (uint)bVar11;
        } while( true );
      }
      uVar5 = (undefined1)(local_a >> 8);
      goto LAB_1008_798d;
    }
  }
  uVar5 = (undefined1)(uVar3 >> 8);
  local_17 = 0;
LAB_1008_798d:
  DAT_10a8_1d2e = 0;
  return CONCAT11(uVar5,local_17);
}

/* ======================================================== */
/* Function: FUN_1008_79a2 @ 1008:79a2 */
/* ======================================================== */

void __stdcall16far FUN_1008_79a2(void)

{
  int unaff_BP;
  int iVar1;
  char *pcVar2;
  
  iVar1 = unaff_BP + 1;
  pcVar2 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_1028_0002(DAT_10a8_39a7,0,DAT_10a8_22aa,0x31a6,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1028_0386(pcVar2,iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_79c7 @ 1008:79c7 */
/* ======================================================== */

void __stdcall16far FUN_1008_79c7(void)

{
  int unaff_BP;
  int iVar1;
  char *pcVar2;
  
  iVar1 = unaff_BP + 1;
  pcVar2 = (char *)s_RandomShrs_10a8_109f + 9;
  FUN_1028_0002(DAT_10a8_39a7,0,DAT_10a8_22aa,0x31a6,(char *)s_RandomShrs_10a8_109f + 9);
  FUN_1028_0386(pcVar2,iVar1);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_79ec @ 1008:79ec */
/* ======================================================== */

void __stdcall16far FUN_1008_79ec(int *param_1)

{
  uint in_AX;
  uint uVar1;
  undefined1 extraout_AH;
  int unaff_BP;
  int iVar2;
  char *pcVar3;
  
  iVar2 = unaff_BP + 1;
  pcVar3 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar1 = in_AX & 0xff00;
  if (DAT_10a8_39a7 == '\0') {
    uVar1 = uVar1 + 1;
  }
  DAT_10a8_39a7 = (char)uVar1;
  FUN_1028_0002(uVar1,1,DAT_10a8_22aa,0x31a6,(char *)s_RandomShrs_10a8_109f + 9);
  (*(code *)*(undefined2 *)(*param_1 + 0x54))
            ((char *)s_NumPlayers_10a8_1022 + 6,(int *)param_1,(int)((ulong)param_1 >> 0x10),
             CONCAT11(extraout_AH,DAT_10a8_39a8),pcVar3,iVar2);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7a28 @ 1008:7a28 */
/* ======================================================== */

void __stdcall16far FUN_1008_7a28(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x47e);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7a49 @ 1008:7a49 */
/* ======================================================== */

void __stdcall16far FUN_1008_7a49(void)

{
  int iVar1;
  
  MESSAGEBEEP();
  iVar1 = MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x24,0x69b,
                     (char *)s_RandomShrs_10a8_109f + 9,0x658,(char *)s_RandomShrs_10a8_109f + 9);
  if (iVar1 == 6) {
    DAT_10a8_39a3 = 1;
    DAT_10a8_38c4 = DAT_10a8_319a;
    DAT_10a8_38c6 = DAT_10a8_319c;
  }
  DAT_10a8_39a2 = 0;
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7a91 @ 1008:7a91 */
/* ======================================================== */

void __stdcall16far FUN_1008_7a91(int *param_1)

{
  undefined1 extraout_AH;
  
  if (DAT_10a8_22c6 == '\0') {
    *(undefined1 *)(DAT_10a8_319a * 0xfc + 0x30c9) = 1;
    *(undefined1 *)((undefined2 *)&DAT_10a8_38bc + DAT_10a8_319a * 0x10) = 3;
    FUN_1058_04ee(DAT_10a8_1d31);
    (*(code *)*(undefined2 *)(*param_1 + 0x54))
              ((char *)s_ShareLimitNumber_10a8_104d + 0xb,(int *)param_1,
               (int)((ulong)param_1 >> 0x10),CONCAT11(extraout_AH,DAT_10a8_39a8));
    POSTMESSAGE((char *)s_ShareLimitNumber_10a8_104d + 0xb,0,0,0,0x4ef);
  }
  else {
    MESSAGEBEEP();
    MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x10,0x6e5,(char *)s_RandomShrs_10a8_109f + 9,
               0x6b2,(char *)s_RandomShrs_10a8_109f + 9);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7b09 @ 1008:7b09 */
/* ======================================================== */

void __stdcall16far FUN_1008_7b09(undefined4 param_1,undefined4 param_2)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined1 extraout_AH;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  uVar1 = GETDC();
  uVar8 = uVar1;
  iVar2 = GETDEVICECAPS((char *)s__Show_Graph_10a8_1226 + 10,8);
  uVar6 = uVar1;
  uVar3 = GETDEVICECAPS((char *)s__Show_Graph_10a8_1226 + 10,10);
  uVar7 = *(undefined2 *)(iVar4 + 4);
  RELEASEDC((char *)s__Show_Graph_10a8_1226 + 10,uVar1);
  FUN_1028_0002(CONCAT11(extraout_AH,DAT_10a8_39a7),0,DAT_10a8_22aa,0x31a6,
                (char *)s_RandomShrs_10a8_109f + 9);
  FUN_1028_0386(uVar7,uVar6,uVar8);
  FUN_1028_0334(uVar3,iVar2);
  FUN_10a0_1e88((char *)s_NumPlayers_10a8_1022 + 6,iVar4,uVar5,(int)param_2,
                (int)((ulong)param_2 >> 0x10));
  FUN_1020_00ed((int)*(undefined4 *)(iVar4 + 0x57),
                (int)((ulong)*(undefined4 *)(iVar4 + 0x57) >> 0x10),uVar3,iVar2);
  uVar7 = *(undefined2 *)(iVar4 + 4);
  SHOWWINDOW((char *)s_OILBARON_INI_10a8_1014 + 0xc,1);
  uVar6 = *(undefined2 *)(iVar4 + 4);
  MOVEWINDOW((char *)s__Show_Graph_10a8_1226 + 10,1,uVar3,(iVar2 << 1) / 3,0,0);
  SETFOCUS((char *)s__Show_Graph_10a8_1226 + 10,*(undefined2 *)(iVar4 + 4),uVar6,uVar7);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7bd8 @ 1008:7bd8 */
/* ======================================================== */

void __stdcall16far FUN_1008_7bd8(int *param_1,undefined4 param_2)

{
  undefined2 *puVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  int unaff_BP;
  int *piVar6;
  int iVar7;
  int *piVar8;
  undefined2 uVar9;
  char *pcVar10;
  undefined2 unaff_SS;
  int local_582 [19];
  char local_55c;
  undefined1 local_55b;
  char local_55a;
  undefined2 local_559;
  undefined2 local_557;
  undefined1 local_555 [1356];
  undefined1 local_9;
  long local_8;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  if (DAT_10a8_39a8 == '\0') {
    if (DAT_10a8_1d2f == '\0') {
      uVar9 = (undefined2)((ulong)param_1 >> 0x10);
      piVar6 = (int *)param_1;
      pcVar10 = (char *)s_ShareLimitNumber_10a8_104d + 3;
      local_8 = FUN_1050_0002(0,DAT_10a8_319a * 0xfc + 0x30d0,(char *)s_RandomShrs_10a8_109f + 9,
                              DAT_10a8_22ae,DAT_10a8_22b0,piVar6,uVar9);
      if (local_8 != 0) {
        iVar7 = DAT_10a8_319a * 0xfc + 0x30aa;
        iVar3 = (int)local_8 * 0xd;
        if (*(int *)(iVar7 + iVar3 + 0x1a) < 0) {
          FUN_10a0_0b92(0x54c,local_555,unaff_SS,(undefined2 *)&DAT_10a8_1d32,
                        (char *)s_RandomShrs_10a8_109f + 9);
          local_55c = '\x01';
          local_55a = (char)DAT_10a8_22b2 + '@';
          local_557 = DAT_10a8_22b6;
          pcVar10 = (char *)s_PayDividends_10a8_1079 + 7;
          FUN_1080_0002(local_582,unaff_SS,0xa0a,0x6f8,(char *)s_RandomShrs_10a8_109f + 9,piVar6,
                        uVar9);
          (*(code *)*(undefined2 *)(local_582[0] + 0x4c))
                    ((char *)s_PayDividends_10a8_1079 + 7,local_582);
          (*(code *)*(undefined2 *)(local_582[0] + 8))
                    ((char *)s_PayDividends_10a8_1079 + 7,local_582);
          if (local_55c != '\0') {
            return;
          }
          *(undefined2 *)(iVar7 + (int)local_8 * 0xd + 0x1a) = local_559;
          iVar3 = (int)local_8 * 0xd;
          *(undefined1 *)(iVar7 + iVar3 + 0x19) = local_55b;
        }
        DAT_10a8_39a8 = '\x01';
        (*(code *)*(undefined2 *)(*param_1 + 0x54))
                  (pcVar10,piVar6,uVar9,CONCAT11((char)((uint)iVar3 >> 8),1));
        iVar4 = *(char *)(iVar7 + (int)local_8 * 0xd + 0x19) + -0x40;
        iVar3 = *(int *)(iVar7 + (int)local_8 * 0xd + 0x1a);
        uVar5 = FUN_1008_1abd(1,iVar3,iVar3 >> 0xf,iVar4,iVar4 >> 0xf);
        local_9 = (undefined1)uVar5;
        piVar8 = (int *)((*(char *)(iVar7 + (int)local_8 * 0xd + 0x19) + -0x41) * 0x8c +
                         (*(int *)(iVar7 + (int)local_8 * 0xd + 0x1a) + -1) * 5 + 0x22d0);
        puVar1 = (undefined2 *)(*piVar8 + 0x10);
        (*(code *)*puVar1)(pcVar10,piVar8,(char *)s_RandomShrs_10a8_109f + 9,uVar5);
        FUN_1008_229d(DAT_10a8_319a * 0xfc + (int)local_8 * 0xd + 0x30c3,
                      (char *)s_RandomShrs_10a8_109f + 9);
        FUN_1008_5e10(piVar6,uVar9,local_8);
        if (DAT_10a8_22c9 != '\0') {
          FUN_10a0_1e88((char *)s_FALSE_10a8_1003 + 5,piVar6,uVar9,(int)param_2,
                        (int)((ulong)param_2 >> 0x10));
        }
        iVar3 = *(char *)(iVar7 + (int)local_8 * 0xd + 0x19) + -0x41;
        iVar4 = *(int *)(iVar7 + (int)local_8 * 0xd + 0x1a) + -1;
        cVar2 = FUN_1008_2448(piVar6,uVar9,1,iVar4,iVar4 >> 0xf,iVar3,iVar3 >> 0xf);
        if (cVar2 == '\0') {
          iVar3 = *(char *)(iVar7 + (int)local_8 * 0xd + 0x19) + -0x41;
          iVar7 = *(int *)(iVar7 + (int)local_8 * 0xd + 0x1a) + -1;
          FUN_1008_51fd(piVar6,uVar9,1,iVar7,iVar7 >> 0xf,iVar3,iVar3 >> 0xf);
        }
      }
    }
    else {
      FUN_1048_13ad();
    }
  }
  else {
    MESSAGEBEEP();
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7e1e @ 1008:7e1e */
/* ======================================================== */

void __stdcall16far FUN_1008_7e1e(int *param_1)

{
  undefined1 extraout_AH;
  undefined1 extraout_AH_00;
  undefined1 uVar1;
  int unaff_BP;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  undefined1 local_1f6 [152];
  int local_15e [19];
  undefined1 local_138 [151];
  undefined1 local_a1 [151];
  int local_a;
  int local_8;
  char local_6;
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  piVar2 = (int *)param_1;
  FUN_1080_0002(local_15e,unaff_SS,0x169a,0x704,(char *)s_RandomShrs_10a8_109f + 9,piVar2,uVar3);
  local_6 = '\0';
  (*(code *)*(undefined2 *)(local_15e[0] + 0x4c))((char *)s_PayDividends_10a8_1079 + 7,local_15e);
  (*(code *)*(undefined2 *)(local_15e[0] + 8))((char *)s_PayDividends_10a8_1079 + 7,local_15e);
  if (local_6 != '\x01') {
    DAT_10a8_3ada = local_a;
    DAT_10a8_3adc = local_8;
    FUN_10a0_0b92(0x97,(undefined1 *)&DAT_10a8_3a42,(char *)s_RandomShrs_10a8_109f + 9,local_a1,
                  unaff_SS);
    FUN_10a0_0b92(0x97,(undefined1 *)&DAT_10a8_39aa,(char *)s_RandomShrs_10a8_109f + 9,local_138,
                  unaff_SS);
    if ((DAT_10a8_3adc == 0) && ((uint)DAT_10a8_3a42 + (uint)DAT_10a8_39aa + 0xf == DAT_10a8_3ada))
    {
      DAT_10a8_39a6 = 1;
      WRITEPROFILESTRING((char *)s_RandomShrs_10a8_109f + 1,(undefined1 *)&DAT_10a8_39aa,
                         (char *)s_RandomShrs_10a8_109f + 9,0x742,(char *)s_RandomShrs_10a8_109f + 9
                         ,0x738);
      pcVar6 = (char *)s_RandomShrs_10a8_109f + 9;
      WRITEPROFILESTRING((char *)s__Show_Graph_10a8_1226 + 10,(undefined1 *)&DAT_10a8_3a42,
                         (char *)s_RandomShrs_10a8_109f + 9,0x747,(char *)s_RandomShrs_10a8_109f + 9
                         ,0x738);
      FUN_10a0_0a72(0x96,local_1f6,unaff_SS,0,DAT_10a8_3ada,DAT_10a8_3adc);
      pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
      WRITEPROFILESTRING((char *)s_RandomShrs_10a8_109f + 1,local_1f6,unaff_SS,0x726,
                         (char *)s_RandomShrs_10a8_109f + 9,0x738);
      iVar4 = piVar2[2];
      MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0,0x72b,(char *)s_RandomShrs_10a8_109f + 9,
                 0x74f,(char *)s_RandomShrs_10a8_109f + 9);
      uVar1 = extraout_AH_00;
    }
    else {
      MESSAGEBEEP((char *)s_RandomShrs_10a8_109f + 1,0x30);
      pcVar6 = (char *)piVar2[2];
      pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
      iVar4 = 0x711;
      MESSAGEBOX((char *)s__Show_Graph_10a8_1226 + 10,0x30,0x72b,(char *)s_RandomShrs_10a8_109f + 9,
                 0x711,(char *)s_RandomShrs_10a8_109f + 9);
      uVar1 = extraout_AH;
    }
    (*(code *)*(undefined2 *)(*param_1 + 0x54))
              ((char *)s__Show_Graph_10a8_1226 + 10,piVar2,uVar3,CONCAT11(uVar1,DAT_10a8_39a8),iVar4
               ,pcVar5,pcVar6);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7f62 @ 1008:7f62 */
/* ======================================================== */

void __stdcall16far FUN_1008_7f62(void)

{
  undefined2 unaff_CS;
  
  POSTMESSAGE(unaff_CS,0,0,0,0x47d);
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7f83 @ 1008:7f83 */
/* ======================================================== */

void __stdcall16far FUN_1008_7f83(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  if ((DAT_10a8_39a9 != '\0') && (*(int *)((int)param_3 + 4) == 0x20)) {
    DAT_10a8_39a9 = '\0';
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7fa7 @ 1008:7fa7 */
/* ======================================================== */

void __stdcall16far FUN_1008_7fa7(undefined4 param_1)

{
  FUN_1030_00a5((undefined2 *)&DAT_10a8_22aa,(char *)s_RandomShrs_10a8_109f + 9,(int)param_1,
                (int)((ulong)param_1 >> 0x10));
  return;
}

/* ======================================================== */
/* Function: FUN_1008_7fde @ 1008:7fde */
/* ======================================================== */

void __stdcall16far FUN_1008_7fde(int *param_1)

{
  int unaff_BP;
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  char *unaff_CS;
  undefined4 uVar4;
  char *pcVar5;
  
  iVar1 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  piVar2 = (int *)param_1;
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  if (DAT_10a8_1d24 != '\0') {
    *(undefined2 *)((int)piVar2 + 0x57) = 0;
    *(undefined2 *)((int)piVar2 + 0x59) = 0;
  }
  if (*(int *)((int)piVar2 + 0x57) == 0 && *(int *)((int)piVar2 + 0x59) == 0) {
    unaff_CS = (char *)s_OILBARON_INI_10a8_1014 + 0xc;
    uVar4 = FUN_1020_0002(0,0,0x7fe,0x76a,(char *)s_RandomShrs_10a8_109f + 9,piVar2,uVar3);
    *(undefined2 *)((int)piVar2 + 0x57) = (int)uVar4;
    *(undefined2 *)((int)piVar2 + 0x59) = (int)((ulong)uVar4 >> 0x10);
    *(undefined2 *)((int)*(undefined4 *)((int)piVar2 + 0x57) + 0x41) = 0;
    uVar4 = (*(code *)*(undefined2 *)(*DAT_10a8_1b2e + 0x34))
                      ((char *)s_OILBARON_INI_10a8_1014 + 0xc,(int *)DAT_10a8_1b2e,
                       (int)((ulong)DAT_10a8_1b2e >> 0x10),*(undefined2 *)((int)piVar2 + 0x57),
                       *(undefined2 *)((int)piVar2 + 0x59));
    *(undefined2 *)((int)piVar2 + 0x57) = (int)uVar4;
    *(undefined2 *)((int)piVar2 + 0x59) = (int)((ulong)uVar4 >> 0x10);
    (*(code *)*(undefined2 *)(*param_1 + 0x5c))((char *)s_OILBARON_INI_10a8_1014 + 0xc,piVar2,uVar3)
    ;
  }
  (*(code *)*(undefined2 *)(*param_1 + 0x54))(unaff_CS,piVar2,uVar3,DAT_10a8_39a8,pcVar5,iVar1);
  SETFOCUS(unaff_CS,*(undefined2 *)((int)*(undefined4 *)((int)piVar2 + 0x57) + 4));
  return;
}

/* ======================================================== */
/* Function: FUN_1008_807d @ 1008:807d */
/* ======================================================== */

void __stdcall16far FUN_1008_807d(void)

{
  int iVar1;
  int unaff_BP;
  char *unaff_CS;
  undefined2 unaff_SS;
  undefined1 local_16 [18];
  char *pcStack_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  pcStack_4 = (char *)s_RandomShrs_10a8_109f + 9;
  while( true ) {
    iVar1 = PEEKMESSAGE(unaff_CS,1,0,0,0,local_16,unaff_SS);
    if (iVar1 == 0) break;
    TRANSLATEMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,local_16);
    unaff_CS = (char *)s__Show_Graph_10a8_1226 + 10;
    DISPATCHMESSAGE((char *)s__Show_Graph_10a8_1226 + 10,local_16);
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_8826 @ 1008:8826 */
/* ======================================================== */

undefined2 FUN_1008_8826(undefined2 param_1,char param_2,uint param_3,uint param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  uint local_8;
  uint local_6;
  bool local_3;
  
  iVar2 = param_3 * 0xfc + param_2 * 4;
  local_3 = *(int *)(iVar2 + 0x3182) != 0 || *(int *)(iVar2 + 0x3184) != 0;
  local_8 = 0;
  if ((DAT_10a8_22ac < 0x8000) && ((0 < (int)DAT_10a8_22ac || (DAT_10a8_22aa != 0)))) {
    local_8 = 1;
    local_6 = 0;
    while( true ) {
      if ((local_6 != param_4) || (local_8 != param_3)) {
        iVar3 = param_3 * 0xfc + param_2 * 4;
        iVar2 = *(int *)(iVar3 + 0x3184);
        iVar4 = local_8 * 0xfc + param_2 * 4;
        iVar1 = *(int *)(iVar4 + 0x3184);
        if ((iVar2 < iVar1) ||
           ((iVar2 <= iVar1 && (*(uint *)(iVar3 + 0x3182) <= *(uint *)(iVar4 + 0x3182))))) {
          local_3 = false;
        }
      }
      if ((local_6 == DAT_10a8_22ac) && (local_8 == DAT_10a8_22aa)) break;
      bVar5 = 0xfffe < local_8;
      local_8 = local_8 + 1;
      local_6 = local_6 + bVar5;
    }
  }
  return CONCAT11((char)(local_8 >> 8),local_3);
}

/* ======================================================== */
/* Function: FUN_1008_88d9 @ 1008:88d9 */
/* ======================================================== */

undefined4 FUN_1008_88d9(undefined2 param_1,char param_2,char param_3,uint param_4,uint param_5)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined2 uVar6;
  uint uVar7;
  uint uVar8;
  undefined2 uVar9;
  int iVar10;
  int iVar11;
  undefined2 unaff_SS;
  bool bVar12;
  undefined1 uVar13;
  undefined1 in_ZF;
  undefined1 uVar14;
  bool bVar15;
  undefined4 uVar16;
  uint local_4e;
  uint local_4c;
  uint local_4a;
  uint local_48;
  int local_44;
  uint local_42;
  uint local_40 [21];
  undefined2 local_16;
  undefined2 local_14;
  int local_12;
  undefined2 local_10;
  undefined2 local_e;
  undefined2 local_c;
  char local_a;
  char local_9;
  uint local_8;
  uint local_4;
  
  bVar12 = (long)(int)((long)(int)param_3 * 0x24) != (long)(int)param_3 * 0x24;
  FUN_10a0_1454();
  if (bVar12 || (bool)in_ZF) {
    local_9 = param_3;
    local_a = param_2;
  }
  else {
    local_9 = param_2;
    local_a = param_3;
  }
  iVar10 = local_a * 0x24;
  local_10 = *(undefined2 *)(iVar10 + 0x378a);
  uVar9 = *(undefined2 *)(iVar10 + 0x378c);
  local_c = ((undefined2 *)&DAT_10a8_378e)[local_a * 0x12];
  local_e = uVar9;
  uVar2 = FUN_1008_80b8(param_1,CONCAT11((char)((uint)local_10 >> 8),local_9));
  local_44 = (int)uVar2 >> 0xf;
  uVar3 = FUN_1008_80b8(param_1,local_a);
  iVar10 = ((int)uVar3 >> 0xf) + local_44 + (uint)CARRY2(uVar3,uVar2);
  if ((0 < iVar10) || ((-1 < iVar10 && (0x2d < uVar3 + uVar2)))) {
    iVar10 = FUN_1008_80b8(param_1,CONCAT11((char)(uVar3 + uVar2 >> 8),local_a));
    local_44 = 0x2d - iVar10 >> 0xf;
  }
  if (local_44 < 0) {
    local_44 = 0;
  }
  FUN_10a0_1458();
  local_16 = FUN_10a0_1444();
  local_14 = uVar9;
  local_12 = local_44;
  uVar16 = FUN_1048_0fac(local_a);
  local_4c = (DAT_10a8_22c0 - (int)((ulong)uVar16 >> 0x10)) - (uint)(DAT_10a8_22be < (uint)uVar16);
  uVar2 = 0;
  local_4e = FUN_10a0_0baa();
  uVar7 = DAT_10a8_22ac;
  uVar3 = DAT_10a8_22aa;
  if (((int)param_5 <= (int)DAT_10a8_22ac) &&
     (((int)param_5 < (int)DAT_10a8_22ac || (param_4 <= DAT_10a8_22aa)))) {
    local_40[1] = param_4;
    local_40[2] = param_5;
    while( true ) {
      uVar8 = local_40[1];
      iVar10 = local_40[1] * 0xfc + local_9 * 4;
      local_4a = *(uint *)(iVar10 + 0x3182);
      local_48 = *(uint *)(iVar10 + 0x3184);
      local_40[local_40[1] * 3] = 0;
      local_40[uVar8 * 3 + 1] = 0;
      local_40[uVar8 * 3 + 2] = 0;
      cVar1 = FUN_1008_8826(param_1,CONCAT11((char)(local_4a >> 8),local_9),local_40[1],local_40[2])
      ;
      if (cVar1 != '\0') {
        uVar16 = FUN_1048_0bd8(local_9);
        uVar8 = local_40[1];
        local_40[local_40[1] * 3] = (uint)uVar16;
        local_40[uVar8 * 3 + 1] = uVar2;
        local_40[uVar8 * 3 + 2] = (uint)((ulong)uVar16 >> 0x10);
      }
      iVar10 = local_9 * 0x24;
      uVar2 = *(uint *)(local_a * 0x24 + 0x3774);
      FUN_10a0_1432(*(undefined2 *)(iVar10 + 0x3772),*(undefined2 *)(iVar10 + 0x3774),
                    *(undefined2 *)(iVar10 + 0x3776));
      uVar13 = 0;
      uVar14 = 1;
      FUN_10a0_144a();
      FUN_10a0_1454();
      if (!(bool)uVar13 && !(bool)uVar14) {
        if (DAT_10a8_22cd != '\0') {
          if (((int)local_4c < (int)local_48) ||
             (((int)local_4c <= (int)local_48 && (local_4e < local_4a)))) {
            local_4a = local_4e;
            local_48 = local_4c;
          }
          bVar12 = local_4e < local_4a;
          local_4e = local_4e - local_4a;
          local_4c = (local_4c - local_48) - (uint)bVar12;
        }
        iVar10 = local_9 * 0x24;
        uVar2 = *(uint *)(local_a * 0x24 + 0x3774);
        uVar9 = *(undefined2 *)(local_a * 0x24 + 0x3776);
        FUN_10a0_1432(*(undefined2 *)(iVar10 + 0x3772),*(undefined2 *)(iVar10 + 0x3774),
                      *(undefined2 *)(iVar10 + 0x3776));
        FUN_10a0_144a();
        uVar6 = FUN_10a0_1438();
        FUN_10a0_1458(uVar6,uVar2,uVar9);
        FUN_10a0_1444();
        uVar4 = FUN_10a0_1432();
        uVar8 = local_40[1];
        local_40[local_40[1] * 3] = uVar4;
        local_40[uVar8 * 3 + 1] = uVar2;
        local_40[uVar8 * 3 + 2] = local_48;
      }
      uVar8 = *(uint *)(local_40[1] * 0xfc + local_a * 4 + 0x3184);
      FUN_10a0_1458();
      FUN_10a0_1444();
      uVar5 = FUN_10a0_1432();
      uVar4 = local_40[1];
      local_40[local_40[1] * 3] = uVar5;
      local_40[uVar4 * 3 + 1] = uVar2;
      local_40[uVar4 * 3 + 2] = uVar8;
      if ((local_40[2] == uVar7) && (local_40[1] == uVar3)) break;
      bVar12 = 0xfffe < local_40[1];
      local_40[1] = local_40[1] + 1;
      local_40[2] = local_40[2] + bVar12;
    }
  }
  if ((param_5 != 0) || (param_4 != 1)) {
    iVar10 = param_5 - (param_4 == 0);
    if ((-1 < iVar10) && ((0 < iVar10 || (param_4 != 1)))) {
      local_40[1] = 1;
      local_40[2] = 0;
      while( true ) {
        uVar3 = local_40[1];
        iVar11 = local_40[1] * 0xfc + local_9 * 4;
        local_4a = *(uint *)(iVar11 + 0x3182);
        local_48 = *(uint *)(iVar11 + 0x3184);
        local_40[local_40[1] * 3] = 0;
        local_40[uVar3 * 3 + 1] = 0;
        local_40[uVar3 * 3 + 2] = 0;
        cVar1 = FUN_1008_8826(param_1,CONCAT11((char)(local_4a >> 8),local_9),local_40[1],
                              local_40[2]);
        if (cVar1 != '\0') {
          uVar16 = FUN_1048_0bd8(local_9);
          uVar3 = local_40[1];
          local_40[local_40[1] * 3] = (uint)uVar16;
          local_40[uVar3 * 3 + 1] = uVar2;
          local_40[uVar3 * 3 + 2] = (uint)((ulong)uVar16 >> 0x10);
        }
        iVar11 = local_9 * 0x24;
        uVar2 = *(uint *)(local_a * 0x24 + 0x3774);
        FUN_10a0_1432(*(undefined2 *)(iVar11 + 0x3772),*(undefined2 *)(iVar11 + 0x3774),
                      *(undefined2 *)(iVar11 + 0x3776));
        uVar13 = 0;
        uVar14 = 1;
        FUN_10a0_144a();
        FUN_10a0_1454();
        if (!(bool)uVar13 && !(bool)uVar14) {
          if (DAT_10a8_22cd != '\0') {
            if (((int)local_4c < (int)local_48) ||
               (((int)local_4c <= (int)local_48 && (local_4e < local_4a)))) {
              local_4a = local_4e;
              local_48 = local_4c;
            }
            bVar12 = local_4e < local_4a;
            local_4e = local_4e - local_4a;
            local_4c = (local_4c - local_48) - (uint)bVar12;
          }
          iVar11 = local_9 * 0x24;
          uVar2 = *(uint *)(local_a * 0x24 + 0x3774);
          uVar9 = *(undefined2 *)(local_a * 0x24 + 0x3776);
          FUN_10a0_1432(*(undefined2 *)(iVar11 + 0x3772),*(undefined2 *)(iVar11 + 0x3774),
                        *(undefined2 *)(iVar11 + 0x3776));
          FUN_10a0_144a();
          uVar6 = FUN_10a0_1438();
          FUN_10a0_1458(uVar6,uVar2,uVar9);
          FUN_10a0_1444();
          uVar7 = FUN_10a0_1432();
          uVar3 = local_40[1];
          local_40[local_40[1] * 3] = uVar7;
          local_40[uVar3 * 3 + 1] = uVar2;
          local_40[uVar3 * 3 + 2] = local_48;
        }
        uVar3 = *(uint *)(local_40[1] * 0xfc + local_a * 4 + 0x3184);
        FUN_10a0_1458();
        FUN_10a0_1444();
        uVar8 = FUN_10a0_1432();
        uVar7 = local_40[1];
        local_40[local_40[1] * 3] = uVar8;
        local_40[uVar7 * 3 + 1] = uVar2;
        local_40[uVar7 * 3 + 2] = uVar3;
        if ((local_40[2] == iVar10) && (local_40[1] == param_4 - 1)) break;
        bVar12 = 0xfffe < local_40[1];
        local_40[1] = local_40[1] + 1;
        local_40[2] = local_40[2] + bVar12;
      }
    }
  }
  uVar3 = DAT_10a8_22ac;
  uVar2 = DAT_10a8_22aa;
  local_42 = 1;
  local_40[0] = 0;
  if ((DAT_10a8_22ac < 0x8000) && ((0 < (int)DAT_10a8_22ac || (1 < DAT_10a8_22aa)))) {
    local_40[1] = 2;
    local_40[2] = 0;
    while( true ) {
      bVar12 = CARRY2(local_42 * 4,local_42 * 2);
      bVar15 = local_42 * 6 == 0;
      FUN_10a0_1454();
      if (!bVar12 && !bVar15) {
        local_42 = local_40[1];
        local_40[0] = local_40[2];
      }
      if ((local_40[2] == uVar3) && (local_40[1] == uVar2)) break;
      bVar12 = 0xfffe < local_40[1];
      local_40[1] = local_40[1] + 1;
      local_40[2] = local_40[2] + bVar12;
    }
  }
  if ((param_5 == local_40[0]) && (param_4 == local_42)) {
    local_8 = local_40[local_42 * 3];
    local_4 = local_40[local_42 * 3 + 2];
  }
  else {
    local_8 = 0x83;
    local_4 = 0xa000;
  }
  return CONCAT22(local_4,local_8);
}

/* ======================================================== */
/* Function: FUN_1008_9121 @ 1008:9121 */
/* ======================================================== */

undefined4 FUN_1008_9121(undefined2 param_1,int param_2)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined1 local_9;
  
  iVar4 = param_2 * 0xfc;
  uVar1 = *(undefined2 *)(iVar4 + 0x30ca);
  local_9 = '\x01';
  uVar5 = *(undefined2 *)(iVar4 + 0x30cc);
  uVar6 = *(undefined2 *)(iVar4 + 0x30ce);
  while( true ) {
    iVar4 = local_9 * 0x24;
    uVar3 = *(undefined2 *)(iVar4 + 0x3774);
    uVar2 = *(undefined2 *)(param_2 * 0xfc + local_9 * 4 + 0x3184);
    FUN_10a0_1458(*(undefined2 *)(iVar4 + 0x3772),uVar3,*(undefined2 *)(iVar4 + 0x3776),uVar1,uVar5,
                  uVar6);
    FUN_10a0_1444();
    uVar1 = FUN_10a0_1432();
    if (local_9 == '\b') break;
    local_9 = local_9 + '\x01';
    uVar5 = uVar3;
    uVar6 = uVar2;
  }
  return CONCAT22(uVar2,uVar1);
}

/* ======================================================== */
/* Function: FUN_1008_91bd @ 1008:91bd */
/* ======================================================== */

/* WARNING: Removing unreachable block (ram,0x10089244) */

undefined4 FUN_1008_91bd(undefined2 param_1,undefined1 *param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  char cVar3;
  undefined2 uVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 extraout_DX;
  undefined1 *puVar7;
  undefined1 *puVar8;
  undefined2 unaff_SS;
  undefined1 uVar9;
  undefined1 uVar10;
  long lVar11;
  undefined2 local_16;
  undefined2 local_12;
  undefined1 local_e [12];
  
  puVar8 = local_e;
  puVar7 = (undefined1 *)param_2;
  for (iVar5 = 5; iVar5 != 0; iVar5 = iVar5 + -1) {
    puVar2 = puVar8;
    puVar8 = puVar8 + 1;
    puVar1 = puVar7;
    puVar7 = puVar7 + 1;
    *puVar2 = *puVar1;
  }
  cVar3 = FUN_1048_0988();
  local_12 = *(undefined2 *)(cVar3 * 0x24 + 0x377c);
  FUN_10a0_1444();
  local_16 = FUN_10a0_1444();
  uVar6 = DAT_10a8_22c0;
  FUN_10a0_0baa();
  uVar4 = FUN_10a0_0be7();
  lVar11 = FUN_1048_0fac(cVar3);
  if (lVar11 < CONCAT22(uVar6,uVar4)) {
    FUN_10a0_1458();
    FUN_10a0_1444();
    FUN_10a0_1444();
    uVar9 = 0;
    uVar10 = 1;
    FUN_10a0_144a();
    FUN_1008_9121(param_1,DAT_10a8_319a,DAT_10a8_319c);
    FUN_10a0_1454();
    if (!(bool)uVar9 && !(bool)uVar10) {
      FUN_1008_9121(param_1,DAT_10a8_319a,DAT_10a8_319c);
      local_12 = extraout_DX;
      FUN_10a0_1444();
      local_16 = FUN_10a0_1432();
    }
  }
  return CONCAT22(local_12,local_16);
}

/* ======================================================== */
/* Function: FUN_1008_963a @ 1008:963a */
/* ======================================================== */

void FUN_1008_963a(int param_1)

{
  undefined2 *puVar1;
  int *piVar2;
  uint uVar3;
  char cVar4;
  char cVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  undefined2 in_BX;
  int iVar9;
  char *pcVar10;
  int iVar11;
  int iVar12;
  int *piVar13;
  char *unaff_CS;
  undefined2 unaff_SS;
  bool bVar14;
  bool bVar15;
  undefined4 uVar16;
  undefined4 local_22;
  undefined4 local_1e;
  uint local_16;
  int local_14;
  uint local_a;
  int local_8;
  uint local_6;
  int local_4;
  
  iVar12 = DAT_10a8_22b0;
  uVar3 = DAT_10a8_22ae;
  iVar9 = DAT_10a8_319a * 0xfc + 0x30aa;
  DAT_10a8_1d2e = 1;
  if ((-1 < DAT_10a8_22b0) && ((0 < DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_6 = 1;
    local_4 = 0;
    do {
      iVar11 = iVar9 + local_6 * 0xd;
      pcVar10 = (char *)(iVar11 + 0x19);
      local_22 = (char *)CONCAT22((char *)s_RandomShrs_10a8_109f + 9,pcVar10);
      if ((*(char *)(iVar11 + 0x1e) != '\0') && (DAT_10a8_1d2f == '\0')) {
        piVar13 = (int *)((*pcVar10 + -0x41) * 0x8c + (*(int *)(iVar11 + 0x1a) + -1) * 5 + 0x22d0);
        puVar1 = (undefined2 *)(*piVar13 + 0x10);
        (*(code *)*puVar1)(unaff_CS,piVar13,(char *)s_RandomShrs_10a8_109f + 9,9);
        uVar7 = FUN_1008_1abd(0,*(int *)(iVar11 + 0x1a),*(int *)(iVar11 + 0x1a) >> 0xf,
                              *local_22 + -0x40,*local_22 + -0x40 >> 0xf);
        cVar5 = (char)uVar7;
        if (cVar5 == '\t') {
LAB_1008_97a7:
          if (cVar5 == '\t') {
            iVar6 = *(int *)(iVar11 + 0x1a) + -1;
            unaff_CS = (char *)s_FALSE_10a8_1003 + 5;
            cVar4 = FUN_1008_51fd((int)*(undefined4 *)(param_1 + 6),
                                  (int)((ulong)*(undefined4 *)(param_1 + 6) >> 0x10),0,iVar6,
                                  iVar6 >> 0xf,*local_22 + -0x41,*local_22 + -0x41 >> 0xf);
            if (cVar4 != '\0') {
              uVar16 = FUN_1008_91bd(param_1,pcVar10,(char *)s_RandomShrs_10a8_109f + 9);
              *(undefined2 *)(iVar11 + 0x20) = (int)uVar16;
              *(undefined2 *)(iVar11 + 0x22) = in_BX;
              *(undefined2 *)(iVar11 + 0x24) = (int)((ulong)uVar16 >> 0x10);
              goto LAB_1008_9854;
            }
          }
          if (cVar5 == '\t') {
            *(undefined2 *)(iVar11 + 0x20) = 0x81;
            *(undefined2 *)(iVar11 + 0x22) = 0;
            *(undefined2 *)(iVar11 + 0x24) = 0;
          }
          else {
            uVar16 = FUN_1008_93e6(param_1,pcVar10,(char *)s_RandomShrs_10a8_109f + 9);
            *(undefined2 *)(iVar11 + 0x20) = (int)uVar16;
            *(undefined2 *)(iVar11 + 0x22) = in_BX;
            *(undefined2 *)(iVar11 + 0x24) = (int)((ulong)uVar16 >> 0x10);
            if (1 < *(int *)(iVar11 + 0x1c)) {
              in_BX = *(undefined2 *)(iVar11 + 0x22);
              bVar14 = false;
              bVar15 = true;
              unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
              FUN_10a0_1454();
              if (!bVar14 && !bVar15) {
                *(undefined2 *)(iVar11 + 0x1c) = 0;
              }
            }
          }
        }
        else {
          piVar13 = (int *)((*local_22 + -0x41) * 0x8c + (*(int *)(iVar11 + 0x1a) + -1) * 5 + 0x22d0
                           );
          puVar1 = (undefined2 *)(*piVar13 + 0x10);
          (*(code *)*puVar1)(unaff_CS,piVar13,(char *)s_RandomShrs_10a8_109f + 9,uVar7);
          iVar6 = *(int *)(iVar11 + 0x1a) + -1;
          unaff_CS = (char *)s_FALSE_10a8_1003 + 5;
          cVar4 = FUN_1008_2448((int)*(undefined4 *)(param_1 + 6),
                                (int)((ulong)*(undefined4 *)(param_1 + 6) >> 0x10),0,iVar6,
                                iVar6 >> 0xf,*local_22 + -0x41,*local_22 + -0x41 >> 0xf);
          if (cVar4 == '\0') goto LAB_1008_97a7;
          uVar16 = FUN_1008_8f0c(param_1,pcVar10,(char *)s_RandomShrs_10a8_109f + 9,DAT_10a8_319a,
                                 DAT_10a8_319c);
          *(undefined2 *)(iVar11 + 0x20) = (int)uVar16;
          *(undefined2 *)(iVar11 + 0x22) = in_BX;
          *(undefined2 *)(iVar11 + 0x24) = (int)((ulong)uVar16 >> 0x10);
          if (1 < *(int *)(iVar11 + 0x1c)) {
            in_BX = *(undefined2 *)(iVar11 + 0x22);
            bVar14 = false;
            bVar15 = true;
            unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
            FUN_10a0_1454();
            if (!bVar14 && !bVar15) {
              *(undefined2 *)(iVar11 + 0x1c) = 0;
            }
          }
        }
LAB_1008_9854:
        piVar13 = (int *)((*local_22 + -0x41) * 0x8c + (*(int *)(iVar11 + 0x1a) + -1) * 5 + 0x22d0);
        puVar1 = (undefined2 *)(*piVar13 + 0x10);
        (*(code *)*puVar1)(unaff_CS,piVar13,(char *)s_RandomShrs_10a8_109f + 9,0);
      }
      if ((local_4 == iVar12) && (local_6 == uVar3)) break;
      bVar14 = 0xfffe < local_6;
      local_6 = local_6 + 1;
      local_4 = local_4 + (uint)bVar14;
    } while( true );
  }
  iVar12 = DAT_10a8_22b0;
  uVar3 = DAT_10a8_22ae;
  DAT_10a8_1d2e = 0;
  local_a = 0;
  local_8 = 0;
  if (((*(char *)((undefined2 *)&DAT_10a8_38bc + DAT_10a8_319a * 0x10) == '\x02') &&
      (-1 < DAT_10a8_22b0)) && ((0 < DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_6 = 1;
    local_4 = 0;
    while( true ) {
      iVar11 = iVar9 + local_6 * 0xd;
      FUN_10a0_18f1(100);
      uVar8 = 0;
      FUN_10a0_1458();
      FUN_10a0_1444();
      FUN_10a0_144a();
      unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
      uVar7 = FUN_10a0_1432();
      *(undefined2 *)(iVar11 + 0x20) = uVar7;
      *(undefined2 *)(iVar11 + 0x22) = in_BX;
      *(undefined2 *)(iVar11 + 0x24) = uVar8;
      if ((local_4 == iVar12) && (local_6 == uVar3)) break;
      bVar14 = 0xfffe < local_6;
      local_6 = local_6 + 1;
      local_4 = local_4 + (uint)bVar14;
    }
  }
  iVar12 = DAT_10a8_22b0;
  uVar3 = DAT_10a8_22ae;
  if ((-1 < DAT_10a8_22b0) && ((0 < DAT_10a8_22b0 || (DAT_10a8_22ae != 0)))) {
    local_6 = 1;
    local_4 = 0;
    while( true ) {
      iVar11 = iVar9 + local_6 * 0xd;
      if (*(char *)(iVar11 + 0x1e) != '\0') {
        iVar11 = *(int *)(iVar11 + 0x1c);
        bVar14 = false;
        bVar15 = iVar11 == 0;
        if (0 < iVar11) {
          unaff_CS = (char *)s_RandomShrs_10a8_109f + 1;
          FUN_10a0_1454();
          if (!bVar14 && !bVar15) {
            local_a = local_6;
            local_8 = local_4;
          }
        }
      }
      if ((local_4 == iVar12) && (local_6 == uVar3)) break;
      bVar14 = 0xfffe < local_6;
      local_6 = local_6 + 1;
      local_4 = local_4 + (uint)bVar14;
    }
  }
  if ((local_8 < 1) && ((local_8 < 0 || (local_a == 0)))) {
    local_14 = 0;
    iVar12 = DAT_10a8_319a * 0xfc + 0x30aa;
    for (local_16 = 1;
        ((*(char *)(iVar12 + local_16 * 0xd + 0x1e) == '\0' &&
         (*(char *)(iVar12 + local_16 * 0xd + 0x1f) == '\0')) &&
        ((local_14 < DAT_10a8_22b0 || ((local_14 <= DAT_10a8_22b0 && (local_16 < DAT_10a8_22ae))))))
        ; local_16 = local_16 + 1) {
      local_14 = local_14 + (uint)(0xfffe < local_16);
    }
    if ((local_14 < DAT_10a8_22b0) || ((local_14 <= DAT_10a8_22b0 && (local_16 <= DAT_10a8_22ae))))
    {
      *(undefined1 *)(DAT_10a8_319a * 0xfc + local_16 * 0xd + 0x30c8) = 0;
      *(undefined1 *)(DAT_10a8_319a * 0xfc + local_16 * 0xd + 0x30c9) = 0;
      DAT_10a8_39a8 = 1;
      piVar2 = (int *)*(undefined4 *)(param_1 + 6);
      (*(code *)*(undefined2 *)(*piVar2 + 0x54))
                (unaff_CS,(int *)piVar2,(int)((ulong)piVar2 >> 0x10),
                 CONCAT11((char)(local_16 * 0xd >> 8),1));
      ((char *)s_American_Natural_Gas_Co__10a8_193e + 1)
      [*(char *)(DAT_10a8_319a * 0xfc + local_16 * 0xd + 0x30c3) * 0x1a +
       *(int *)(DAT_10a8_319a * 0xfc + local_16 * 0xd + 0x30c4)] = '\x02';
      FUN_1008_5e10((int)*(undefined4 *)(param_1 + 6),
                    (int)((ulong)*(undefined4 *)(param_1 + 6) >> 0x10),local_16,local_14);
      if (DAT_10a8_22cf != '\0') {
        iVar12 = DAT_10a8_319a * 0xfc + local_16 * 0xd;
        piVar13 = (int *)((*(char *)(iVar12 + 0x30c3) + -0x41) * 0x8c +
                          (*(int *)(iVar12 + 0x30c4) + -1) * 5 + 0x22d0);
        puVar1 = (undefined2 *)(*piVar13 + 0x10);
        (*(code *)*puVar1)((char *)s_FALSE_10a8_1003 + 5,piVar13,(char *)s_RandomShrs_10a8_109f + 9,
                           0);
      }
    }
  }
  else {
    iVar9 = iVar9 + local_a * 0xd;
    local_1e = (char *)CONCAT22((char *)s_RandomShrs_10a8_109f + 9,(char *)(iVar9 + 0x19));
    DAT_10a8_39a8 = 1;
    piVar2 = (int *)*(undefined4 *)(param_1 + 6);
    (*(code *)*(undefined2 *)(*piVar2 + 0x54))
              (unaff_CS,(int *)piVar2,(int)((ulong)piVar2 >> 0x10),
               CONCAT11((char)(local_a * 0xd >> 8),1));
    uVar7 = FUN_1008_1abd(1,*(int *)(iVar9 + 0x1a),*(int *)(iVar9 + 0x1a) >> 0xf,*local_1e + -0x40,
                          *local_1e + -0x40 >> 0xf);
    piVar13 = (int *)((*local_1e + -0x41) * 0x8c + (*(int *)(iVar9 + 0x1a) + -1) * 5 + 0x22d0);
    puVar1 = (undefined2 *)(*piVar13 + 0x10);
    (*(code *)*puVar1)(unaff_CS,piVar13,(char *)s_RandomShrs_10a8_109f + 9,uVar7);
    FUN_1008_229d((char *)(iVar9 + 0x19),(char *)s_RandomShrs_10a8_109f + 9);
    FUN_1008_5e10((int)*(undefined4 *)(param_1 + 6),
                  (int)((ulong)*(undefined4 *)(param_1 + 6) >> 0x10),local_6,local_4);
    iVar12 = *(int *)(iVar9 + 0x1a) + -1;
    cVar5 = FUN_1008_2448((int)*(undefined4 *)(param_1 + 6),
                          (int)((ulong)*(undefined4 *)(param_1 + 6) >> 0x10),1,iVar12,iVar12 >> 0xf,
                          *local_1e + -0x41,*local_1e + -0x41 >> 0xf);
    if (cVar5 == '\0') {
      iVar12 = *(int *)(iVar9 + 0x1a) + -1;
      FUN_1008_51fd((int)*(undefined4 *)(param_1 + 6),
                    (int)((ulong)*(undefined4 *)(param_1 + 6) >> 0x10),1,iVar12,iVar12 >> 0xf,
                    *local_1e + -0x41,*local_1e + -0x41 >> 0xf);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1008_a1da @ 1008:a1da */
/* ======================================================== */

bool FUN_1008_a1da(int param_1,char param_2)

{
  undefined2 in_BX;
  int iVar1;
  undefined2 unaff_SS;
  bool bVar2;
  undefined1 uVar3;
  undefined1 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
  undefined2 uVar9;
  uint local_10;
  uint local_e;
  uint local_8;
  uint local_6;
  bool local_3;
  
  uVar9 = 0;
  uVar5 = 0;
  local_10 = 1;
  local_e = 0;
  uVar7 = DAT_10a8_22aa;
  uVar8 = DAT_10a8_22ac;
  if ((-1 < (int)DAT_10a8_22ac) && ((0 < (int)DAT_10a8_22ac || (uVar5 = 0, DAT_10a8_22aa != 0)))) {
    local_8 = 1;
    local_6 = 0;
    uVar5 = 0;
    do {
      uVar3 = local_6 < DAT_10a8_319c;
      uVar4 = local_6 == DAT_10a8_319c;
      if ((bool)uVar4) {
        uVar3 = local_8 < DAT_10a8_319a;
        uVar4 = local_8 == DAT_10a8_319a;
        if (!(bool)uVar4) goto LAB_1008_a23a;
      }
      else {
LAB_1008_a23a:
        FUN_1008_9121(*(undefined2 *)(param_1 + 4),local_8,local_6);
        FUN_10a0_1454();
        if (!(bool)uVar3 && !(bool)uVar4) {
          uVar5 = FUN_1008_9121(*(undefined2 *)(param_1 + 4),local_8,local_6);
          local_10 = local_8;
          local_e = local_6;
          uVar9 = in_BX;
        }
      }
      if ((local_6 == uVar8) && (local_8 == uVar7)) break;
      bVar2 = 0xfffe < local_8;
      local_8 = local_8 + 1;
      local_6 = local_6 + bVar2;
    } while( true );
  }
  uVar6 = FUN_1008_9121(*(undefined2 *)(param_1 + 4),local_10,local_e);
  iVar1 = param_2 * 0x24;
  uVar3 = CARRY2(local_10 * 0xfc,param_2 * 4);
  uVar4 = local_10 * 0xfc + param_2 * 4 == 0;
  FUN_10a0_1458(*(undefined2 *)(iVar1 + 0x3772),*(undefined2 *)(iVar1 + 0x3774),
                *(undefined2 *)(iVar1 + 0x3776),(int)uVar6,in_BX,(int)((ulong)uVar6 >> 0x10),uVar7,
                uVar8,(int)uVar5,uVar9,(int)((ulong)uVar5 >> 0x10));
  FUN_10a0_1444();
  FUN_10a0_144a();
  FUN_10a0_1454();
  local_3 = !(bool)uVar3 && !(bool)uVar4;
  return local_3;
}

/* ======================================================== */
/* Function: FUN_1068_0090 @ 1068:0090 */
/* ======================================================== */

void __stdcall16far FUN_1068_0090(undefined4 param_1,int *param_2,undefined1 *param_3)

{
  uint uVar1;
  int unaff_BP;
  int iVar2;
  int *piVar3;
  undefined2 uVar4;
  char *pcVar5;
  
  iVar2 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  DAT_10a8_3ca0 = '\0';
  DAT_10a8_3c50 = *param_3;
  FUN_10a0_0b92(0x48,0x3c52,(char *)s_RandomShrs_10a8_109f + 9,(int)param_1,
                (int)((ulong)param_1 >> 0x10));
  DAT_10a8_3c9c = 0;
  DAT_10a8_3c9e = 0;
  uVar4 = (undefined2)((ulong)DAT_10a8_1b2e >> 0x10);
  FUN_1080_0002((undefined2 *)&DAT_10a8_3b02,(char *)s_RandomShrs_10a8_109f + 9,0x137c,
                (char *)s_SHARES_DIALOG_10a8_1438,(char *)s_RandomShrs_10a8_109f + 9,
                *(undefined2 *)((int)DAT_10a8_1b2e + 8),*(undefined2 *)((int)DAT_10a8_1b2e + 10));
  (*(code *)*(undefined2 *)(DAT_10a8_3b02 + 0x4c))
            ((char *)s_PayDividends_10a8_1079 + 7,(undefined2 *)&DAT_10a8_3b02,
             (char *)s_RandomShrs_10a8_109f + 9,pcVar5,iVar2);
  (*(code *)*(undefined2 *)(DAT_10a8_3b02 + 8))
            ((char *)s_PayDividends_10a8_1079 + 7,(undefined2 *)&DAT_10a8_3b02,
             (char *)s_RandomShrs_10a8_109f + 9,0);
  *param_3 = DAT_10a8_3c50;
  iVar2 = DAT_10a8_3c9e;
  piVar3 = (int *)param_2;
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  if (DAT_10a8_3ca0 == '\0') {
    *param_2 = 0;
    piVar3[1] = 0;
  }
  else {
    *param_2 = DAT_10a8_3c9c;
    piVar3[1] = iVar2;
    if (DAT_10a8_3c9a == '\0') {
      iVar2 = *param_2;
      uVar1 = piVar3[1];
      *param_2 = -iVar2;
      piVar3[1] = (~uVar1 + 1) - (uint)(iVar2 != 0);
    }
  }
  return;
}

/* ======================================================== */
/* Function: FUN_1068_0144 @ 1068:0144 */
/* ======================================================== */

char * __stdcall16far FUN_1068_0144(void)

{
  return s_Buy_or_Sell_10a8_1446;
}

/* ======================================================== */
/* Function: FUN_1068_020c @ 1068:020c */
/* ======================================================== */

void __stdcall16far FUN_1068_020c(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int unaff_BP;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  char *pcVar5;
  
  iVar2 = unaff_BP + 1;
  pcVar5 = (char *)s_RandomShrs_10a8_109f + 9;
  DAT_10a8_3c9c = 0xffff;
  DAT_10a8_3c9e = -1;
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  FUN_1068_0002(iVar3,uVar4,0x6a,(undefined2 *)&DAT_10a8_3c9c,(char *)s_RandomShrs_10a8_109f + 9);
  if (DAT_10a8_3c9a != '\0') {
    iVar1 = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4f);
    if ((iVar1 < DAT_10a8_3c9e) ||
       (((iVar1 <= DAT_10a8_3c9e && (*(uint *)(DAT_10a8_3c50 * 9 + 0x3c4d) < DAT_10a8_3c9c)) ||
        (DAT_10a8_3c9e < 0)))) {
      (*DAT_10a8_1b46)((char *)s_PurchaseLimitNumber_10a8_105e + 10,0,(char *)s_Cannot_Buy_10a8_146e
                       ,(char *)s_RandomShrs_10a8_109f + 9,
                       (char *)s_Too_many_or_too_few_shares__10a8_1452,
                       (char *)s_RandomShrs_10a8_109f + 9,*(undefined2 *)(iVar3 + 4));
      return;
    }
  }
  if (DAT_10a8_3c9a == '\0') {
    iVar1 = *(int *)(DAT_10a8_3c50 * 9 + 0x3c4b);
    if (((iVar1 < DAT_10a8_3c9e) ||
        ((iVar1 <= DAT_10a8_3c9e && (*(uint *)(DAT_10a8_3c50 * 9 + 0x3c49) < DAT_10a8_3c9c)))) ||
       (DAT_10a8_3c9e < 0)) {
      (*DAT_10a8_1b46)((char *)s_PurchaseLimitNumber_10a8_105e + 10,0,
                       (char *)s_Cannot_Sell_10a8_1479,(char *)s_RandomShrs_10a8_109f + 9,
                       (char *)s_Too_many_or_too_few_shares__10a8_1452,
                       (char *)s_RandomShrs_10a8_109f + 9,*(undefined2 *)(iVar3 + 4),pcVar5,iVar2);
      return;
    }
  }
  FUN_1080_0382(iVar3,uVar4,(int)param_2,(int)((ulong)param_2 >> 0x10));
  DAT_10a8_3ca0 = 1;
  return;
}

