# Oil Baron Reverse Engineering & Logic Extraction Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Establish the project repository, extract all binary assets and dialogs, execute headless Ghidra decompilation on the 16-bit `OILBARON.EXE` binary, and synthesize the extracted game rules and mathematical formulas into a definitive rulebook.

**Architecture:** A reverse engineering pipeline using native Linux tools (`wrestool`) for asset extraction and Ghidra Headless Analyzer (`analyzeHeadless`) with an automated Java export post-script (`ExportDecompiledC.java`) that disassembles, analyzes, and decompiles all 16-bit x86 protected mode functions into categorized C pseudocode and symbol references, culminating in an authoritative `docs/game_rules.md`.

**Tech Stack:** Git, Bash, wrestool/icoutils, Ghidra 11.x (Java 21 OpenJDK), Ghidra Decompiler API (`DecompInterface`).

**Spec:** [docs/superpowers/specs/2026-10-07-oilbaron-reverse-engineering-design.md](file:///home/anno/Code/gamedev/oilbaron/docs/superpowers/specs/2026-10-07-oilbaron-reverse-engineering-design.md)

## Global Constraints
- Target binary: `game/OILBARON.EXE` (16-bit Windows 3.1 NE format).
- Must NEVER commit `game/` folder or binaries to git (strict `.gitignore`).
- Decompiled output must be structured and saved in `decompile/src/`, `decompile/resources/`, and `decompile/symbols/`.
- All extracted rules and formulas must be documented clearly in `docs/game_rules.md`.

---

### Task 1: Git Repository Initialization & Configuration

**Files:**
- Create: `.gitignore`
- Modify: None
- Test: Shell verification via `git status`

**Interfaces:**
- Consumes: Workspace root `/home/anno/Code/gamedev/oilbaron`
- Produces: Active git repository with clean ignore rules

- [ ] **Step 1: Create `.gitignore`**

```gitignore
# Game binaries and proprietary original files
game/

# Ghidra project files and lock databases
*.gpr
*.rep/
*.lock*

# OS and Editor metadata
.DS_Store
.idea/
.vscode/

# Web dependencies and build artifacts (for future webgame)
node_modules/
dist/
build/
```

- [ ] **Step 2: Initialize git repository**

Run:
```bash
git init
git add .gitignore docs/
git status
```
Expected: `game/` is untracked and ignored; `.gitignore` and `docs/` staged.

- [ ] **Step 3: Initial commit**

Run:
```bash
git commit -m "chore: initialize repository and add reverse engineering spec"
```
Expected: Commit created on main branch.

---

### Task 2: Binary Asset and Resource Extraction

**Files:**
- Create: `scripts/extract_resources.sh`
- Output: `decompile/resources/bitmaps/*.bmp`
- Output: `decompile/resources/icons/*.ico`
- Output: `decompile/resources/dialogs.rc`
- Output: `decompile/resources/help_text.txt`
- Test: Shell verification of extracted BMPs and dialog templates

**Interfaces:**
- Consumes: `game/OILBARON.EXE`, `game/OILBARON.HLP`, `game/OILBARON.TXT`
- Produces: Raw visual assets and dialog templates in `decompile/resources/`

- [ ] **Step 1: Write resource extraction script**

Create `scripts/extract_resources.sh`:
```bash
#!/usr/bin/env bash
set -euo pipefail

mkdir -p decompile/resources/bitmaps
mkdir -p decompile/resources/icons

echo "Extracting bitmaps from OILBARON.EXE..."
wrestool -x -t 2 game/OILBARON.EXE -o decompile/resources/bitmaps/

echo "Extracting icons from OILBARON.EXE..."
wrestool -x -t 14 game/OILBARON.EXE -o decompile/resources/icons/ || true
wrestool -x -t 3 game/OILBARON.EXE -o decompile/resources/icons/ || true

echo "Extracting dialogs and menus..."
wrestool -x -t 5 game/OILBARON.EXE > decompile/resources/dialogs_raw.bin || true
wrestool -l -v game/OILBARON.EXE > decompile/resources/resources_list.txt

echo "Extracting strings and help text..."
strings -n 4 game/OILBARON.HLP > decompile/resources/help_text.txt || true
strings -n 4 game/OILBARON.EXE > decompile/resources/exe_strings.txt

echo "Resource extraction complete."
```

- [ ] **Step 2: Execute resource extraction script**

Run:
```bash
chmod +x scripts/extract_resources.sh
./scripts/extract_resources.sh
```
Expected: Success exit code 0; extracted bitmap files present.

- [ ] **Step 3: Verify extracted resources**

Run:
```bash
ls -la decompile/resources/bitmaps/
grep -iE "dialog|menu" decompile/resources/resources_list.txt | head -n 15
```
Expected: Company bitmaps (`AMERICAN`, `ARABIAN`, `IMPERIAL`, `ROYAL`, `SUPRA`, `TOWER`, `WEEMOCO`) extracted; dialogs listed.

- [ ] **Step 4: Commit extracted assets**

Run:
```bash
git add scripts/ decompile/resources/
git commit -m "feat(decompile): extract embedded bitmaps, dialogs, and text strings"
```

---

### Task 3: Ghidra Headless Decompilation Pipeline

**Files:**
- Create: `scripts/ExportDecompiledC.java`
- Output: `decompile/src/*.c`
- Output: `decompile/symbols/symbols.json`
- Test: Shell verification of generated C sources and symbol mappings

**Interfaces:**
- Consumes: `/opt/ghidra/support/analyzeHeadless`, `game/OILBARON.EXE`
- Produces: Decompiled C functions in `decompile/src/` and symbol table in `decompile/symbols/`

- [ ] **Step 1: Write Ghidra Headless Export Java Script**

Create `scripts/ExportDecompiledC.java`:
```java
// Ghidra headless post-analysis script to decompile all functions into categorized C files
// @author Antigravity
// @category Decompiler

import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.*;

public class ExportDecompiledC extends GhidraScript {

    @Override
    public void run() throws Exception {
        File outDir = new File(getScriptArgs().length > 0 ? getScriptArgs()[0] : "decompile/src");
        File symDir = new File("decompile/symbols");
        outDir.mkdirs();
        symDir.mkdirs();

        println("Setting up DecompInterface...");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);

        println("Exporting symbols and functions...");
        File symFile = new File(symDir, "symbols.json");
        PrintWriter symWriter = new PrintWriter(new FileWriter(symFile));
        symWriter.println("{");
        symWriter.println("  \"functions\": [");

        File allCodeFile = new File(outDir, "all_decompiled.c");
        PrintWriter codeWriter = new PrintWriter(new FileWriter(allCodeFile));

        codeWriter.println("/* Decompiled code for OILBARON.EXE (16-bit Windows 3.1 NE) */");
        codeWriter.println("/* Generated by Ghidra Headless Analyzer */\n");

        FunctionIterator funcs = currentProgram.getFunctionManager().getFunctions(true);
        boolean firstSym = true;
        int count = 0;

        while (funcs.hasNext() && !monitor.isCancelled()) {
            Function f = funcs.next();
            count++;
            String fName = f.getName();
            String entry = f.getEntryPoint().toString();

            if (!firstSym) symWriter.println(",");
            firstSym = false;
            symWriter.print(String.format("    {\"name\": \"%s\", \"address\": \"%s\", \"size\": %d}", 
                fName, entry, f.getBody().getNumAddresses()));

            // Decompile
            DecompileResults res = decomp.decompileFunction(f, 30, monitor);
            if (res != null && res.decompileCompleted() && res.getDecompiledFunction() != null) {
                String cCode = res.getDecompiledFunction().getC();
                codeWriter.println("/* ======================================================== */");
                codeWriter.println("/* Function: " + fName + " @ " + entry + " */");
                codeWriter.println("/* ======================================================== */");
                codeWriter.println(cCode);
                codeWriter.println();
            } else {
                codeWriter.println("/* Failed to decompile function: " + fName + " @ " + entry + " */\n");
            }
        }

        symWriter.println("\n  ]");
        symWriter.println("}");
        symWriter.close();
        codeWriter.close();

        println(String.format("Decompilation complete. Processed %d functions into %s", count, allCodeFile.getAbsolutePath()));
    }
}
```

- [ ] **Step 2: Run Ghidra headless analysis on `game/OILBARON.EXE`**

Run:
```bash
mkdir -p decompile/src decompile/symbols
/opt/ghidra/support/analyzeHeadless /tmp/ghidra_proj OilBaronProj \
    -import game/OILBARON.EXE \
    -scriptPath scripts \
    -postScript ExportDecompiledC.java decompile/src \
    -deleteProject \
    -overwrite
```
Expected: Ghidra completes auto-analysis and runs `ExportDecompiledC.java`; writes `decompile/src/all_decompiled.c` and `decompile/symbols/symbols.json`.

- [ ] **Step 3: Organize and split functions into module C files**

Run a helper splitting script (`scripts/split_modules.py`) to parse `all_decompiled.c` into logical subsystems (`board.c`, `stocks.c`, `mergers.c`, `ai.c`, `dialogs.c`, `main.c`) based on function references (e.g. references to grid offsets, company names, dialog IDs).

- [ ] **Step 4: Verify C source files and symbols**

Run:
```bash
wc -l decompile/src/*.c
head -n 25 decompile/symbols/symbols.json
```
Expected: Multiple thousands of lines of decompiled C pseudocode; valid JSON symbols list.

- [ ] **Step 5: Commit decompiled C sources and symbols**

Run:
```bash
git add decompile/src/ decompile/symbols/ scripts/
git commit -m "feat(decompile): generate decompiled C pseudocode and symbol registry"
```

---

### Task 4: Reverse Engineering Analysis & Game Rules Documentation

**Files:**
- Create: `docs/game_rules.md`
- Inspect: `decompile/src/*.c`, `decompile/resources/help_text.txt`, `decompile/resources/resources_list.txt`
- Test: Review and verify formulas against decompiled C logic

**Interfaces:**
- Consumes: Decompiled C code and resource files
- Produces: `docs/game_rules.md` with complete mathematical formulas and rule engine specifications

- [ ] **Step 1: Trace board and grid data structures**
Inspect functions interacting with board tiles and coordinates:
- Grid size (rows, columns).
- Cell encoding: 0 = empty, company ID = 1..7, dead/unplayable markers.
- Hand size per player (e.g., 6 tiles).

- [ ] **Step 2: Trace company profiles and stock price curve**
Inspect company tables and stock pricing functions:
- 7 companies: American, Arabian, Imperial, Royal, Supra, Tower, Weemoco.
- Identify tier grouping (e.g., Low tier vs Mid tier vs High tier price baselines).
- Price lookup curve based on company tile size.
- Total stock shares per company (e.g., 25 shares) and founder bonus.

- [ ] **Step 3: Trace merger mechanics and shareholder bonus formulas**
Inspect merger resolution code:
- Determining survivor company (larger size). Tie-breaking logic.
- Majority shareholder bonus formula: e.g., $10 \times \text{share price}$ or similar formula.
- Minority shareholder bonus formula: e.g., $5 \times \text{share price}$.
- Share handling: keep, sell at market, or 2-for-1 trade.

- [ ] **Step 4: Trace AI decision heuristics**
Inspect computer player turn routines:
- How the AI ranks tile placement (maximizing immediate company growth vs founding vs holding).
- Share purchase priority (buying dominant stock vs growing stocks).

- [ ] **Step 5: Write comprehensive `docs/game_rules.md`**
Synthesize all extracted findings into `docs/game_rules.md` with:
- Grid dimensions & coordinate system.
- Company tier matrix & stock price formula table.
- Merger payout and tie-breaking algorithms.
- Full turn state machine.
- AI logic flowchart / decision weights.

- [ ] **Step 6: Commit `docs/game_rules.md`**

Run:
```bash
git add docs/game_rules.md
git commit -m "docs: finalize extracted game rules and mathematical model"
```
