# Electronic Dice (Zar electronic)

A single-button electronic die built around a **Microchip PIC16LF1937**.
Holding the button "rolls" the die: nine LEDs arranged as a 3×3 pip grid flash
through the faces while a 7‑segment display shows the numeric value. When the
button is released the animation decelerates like a real die and settles on a
random face.

The project is captured as a **Proteus Design Suite 8** project (`dice.pdsprj`)
containing the schematic, the PCB layout, the manufacturing (CADCAM/Gerber)
output and the C firmware that runs on the simulated / real MCU.

---

## 1. Behaviour

| Button state | What happens |
|---|---|
| **Pressed / held** | The frame counter advances every ~8 ms — the pips and the digit cycle rapidly through the faces. |
| **Released** | The frame period grows by 3 ms after every frame (8 → 11 → 14 … ms). Once it passes 220 ms the die is considered "settled" and the display freezes on the current face. |
| **Idle** | The last face stays lit until the button is pressed again. |

Because the moment of release is not correlated with the internal counter, the
resting face is effectively random (1–6).

The animation walks through **12 frames** (`c = 0…11`). Faces 1, 4, 5 and 6 use
one pip pattern each; faces 2 and 3 have two alternative pip arrangements so the
roll looks livelier:

| Frame `c` | Face shown | 7‑seg digit |
|:--:|:--:|:--:|
| 0, 6 | 1 | `1` |
| 1 | 2 (pips D1, D9) | `2` |
| 7 | 2 (pips D3, D7) | `2` |
| 2 | 3 | `3` |
| 8 | 3 (alt.) | `3` |
| 3, 9 | 4 | `4` |
| 4, 10 | 5 | `5` |
| 5, 11 | 6 | `6` |

---

## 2. Repository layout

```
dice.pdsprj                          Proteus 8 project (schematic + PCB + firmware link)
dice.PDF                             Rendered schematic / documentation sheet
dice.EDF                             Proteus electrical design (netlist) export
dice.DO                              Autorouter command script

dice - CADCAM *.GBR                  Gerber artwork (see §6)
dice - CADCAM Drill TOP-BOT Plated.GBR   NC drill data
dice - CADCAM Netlist.IPC            IPC‑356 netlist for bare-board test
dice - CADCAM READ-ME.TXT            Labcenter tool-info file: aperture list & layer stackup

_firmware_extras/
  FIRMWARE.XML                       Firmware family / compiler descriptor
  FIRMWARE/
    PIC16LF1937.XML                  VSM Studio sub-project (build options)
    PIC16LF1937/
      main.c                         >>> the firmware source <<<
      Debug/                         XC8 build output (git-ignored, regenerated)
```

Auto-generated Proteus files (`*.pdsbak`, `Project Backups/`, `*.workspace`,
`*.sts`, `bestsave.rte`, print spools) are excluded via `.gitignore`.

---

## 3. Hardware

### 3.1 Bill of materials

| Ref | Qty | Part | Notes |
|---|:--:|---|---|
| U1 | 1 | PIC16LF1937, 40‑pin PDIP | 8‑bit MCU, internal 16 MHz oscillator |
| D1–D9 | 9 | LED (pip) | 3×3 grid, anode fed from the MCU through a series resistor |
| DISPLAY | 1 | 7‑segment display, **common anode** | segment order on this board is `a b c d e g f dp` (f and g swapped vs. textbook) |
| BUTTON | 1 | Tactile push-button | to GND, needs an external pull-up (see 3.4) |
| R1–R9 | 9 | LED series resistors | one per pip |
| R10–R17 | 8 | 7‑segment series resistors | one per segment (a…g + dp) |
| PROGRAM | 1 | 5‑pin ICSP header | MCLR/VPP, VDD, VSS, PGD, PGC |
| ALIMENTARE | 1 | Power / UART connector | VDD, GND and RC6/RC7 (TX/RX) |

*(Reference designators and counts are taken from `dice - CADCAM Netlist.IPC`;
resistor roles are inferred from the schematic net names.)*

### 3.2 MCU pin map

| Port pin | Direction | Function |
|---|---|---|
| RA0 | out | `LED1` (pip D1) |
| RA1 | out | `LED2` (pip D2) |
| RA2 | out | `LED3` (pip D3) |
| RA3 | out | `LED4` (pip D4) |
| RA4 | out | `LED5` (pip D5 – centre) |
| RA5 | out | `LED6` (pip D6) |
| RA6 | out | `LED7` (pip D7) |
| RA7 | **in** | Button (active-low, external 10 k pull-up to VDD) |
| RB0 | out | `LED8` (pip D8) |
| RB1 | out | `LED9` (pip D9) |
| RC0–RC5 | out | 7‑segment segments (6 of the 8 bits) |
| RC6 / RC7 | — | reserved for the ALIMENTARE / UART connector — **firmware must not touch them** |
| RD6 / RD7 | out | 7‑segment segments (remaining 2 bits) |

All LED pins are driven **HIGH = LED on** (pin → resistor → LED anode,
cathode → GND).

### 3.3 Pip (LED) layout

```
   D1   D2   D3
   D4   D5   D6
   D7   D8   D9
```

| Face | Pips lit |
|:--:|---|
| 1 | D5 |
| 2 | D1 D9  (or D3 D7) |
| 3 | D1 D5 D9  (or D3 D5 D7) |
| 4 | D1 D3 D7 D9 |
| 5 | D1 D3 D5 D7 D9 |
| 6 | D1 D3 D4 D6 D7 D9 |

### 3.4 Button

`RA7 → button → GND`. PORTA on the PIC16F1937 has **no internal weak
pull-ups**, so an **external 10 kΩ resistor from RA7 to VDD is required**.
The firmware reads the button as `#define BUTTON (!PORTAbits.RA7)` — pressed =
line pulled low = `BUTTON` true.

### 3.5 7‑segment display

Common-anode; the two common-anode pins (CA1, CA2) tie to VDD. Segments are
sunk through R10–R17 by the MCU, so a segment bit of `0` lights that segment.
On this board the physical bit order is `a, b, c, d, e, g, f, dp` (note **f and
g are swapped**). The `show_digit()` routine writes the low six segment bits to
`RC0–RC5` and the top two to `RD6–RD7` without disturbing `RC6/RC7`.

### 3.6 ICSP programming header (PROGRAM)

Standard 5-pin Microchip in-circuit serial programming layout: `VPP/MCLR`,
`VDD`, `VSS`, `PGD` (ICSPDAT), `PGC` (ICSPCLK). Compatible with PICkit / ICD.

---

## 4. Firmware

Source: [`_firmware_extras/FIRMWARE/PIC16LF1937/main.c`](_firmware_extras/FIRMWARE/PIC16LF1937/main.c)

### 4.1 Toolchain

* **Compiler:** Microchip MPLAB XC8 (invoked in C90 / `--pass1` mode by Proteus VSM Studio)
* **Target:** `PIC16LF1937`
* **Build configurations:** `Debug` (produces `.cof` for Proteus simulation) and `Release` (produces Intel HEX). Options are stored in `_firmware_extras/FIRMWARE/PIC16LF1937.XML`.

### 4.2 Configuration words

Set directly in `main.c`:

```c
__PROG_CONFIG(1, 0x3FE4);   // INTOSC, WDT off, MCLR enabled, CLKOUT off, PWRTE, BOR
__PROG_CONFIG(2, 0x1EFF);   // no LVP quirks, no PLL, etc.
#define _XTAL_FREQ 16000000  // 16 MHz, used by __delay_ms()
```

`init()` then selects the 16 MHz HF internal oscillator (`OSCCON = 0x7B`),
makes RA7 the only input on PORTA, turns every analog function off
(`ANSELA/ANSELB = 0`), and drives all LED/segment ports low.

### 4.3 Program flow

```
main()
 ├─ init()                     ports, oscillator, Timer1 + interrupts
 └─ forever:
      if (BUTTON)              held → roll fast
          c = (c + 1) % 12
          step = 8;  t = 0
          delay 8 ms
      else if (step)           released → decelerate
          delay 1 ms
          if (++t >= step)
              t = 0
              c = (c + 1) % 12
              step += 3
              if (step > 220) step = 0   // die has settled
      render(c)                paint pips + digit for the current frame
```

`render(c)` sets the nine `LEDx` latch bits for the face and calls
`show_digit(v[n])` with the matching digit pattern.

### 4.4 Segment pattern table

```c
unsigned char v[10] = { 0x03, 0x9F, 0x23, 0x0B, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09 };
//                        0     1     2     3     4     5     6     7     8     9
```

Only `v[1]`…`v[6]` are used (digits 1–6). The values already account for the
`a b c d e g f dp` bit order and the common-anode (active-low) wiring of this
specific board.

### 4.5 Timer1 interrupt

`T1CON = 0x11` runs Timer1 from Fosc/4 with a 1:2 prescale; the ISR reloads
`TMR1H:TMR1L = 0x3CAF` for a ~25 ms overflow and increments a counter `a`.
This tick is **currently unused** by the game loop — it is wired up and
available for a future feature (e.g. auto-sleep or a seeded RNG).

### 4.6 Building & flashing

1. Open `dice.pdsprj` in Proteus and use the built-in VSM Studio, **or** create
   an MPLAB X project targeting `PIC16LF1937` and add `main.c`.
2. Build the **Release** configuration to get `dice.hex`.
3. Flash with a PICkit 3/4 (or ICD) through the **PROGRAM** header:
   `VPP, VDD, VSS, PGD, PGC`.
4. First power-up: press and hold the button, release, and the die should settle
   on a face within ~1–2 s.

---

## 5. Simulating in Proteus

1. Open `dice.pdsprj` (Proteus 8.9 or newer — the CADCAM output was generated
   with 8.9 SP0).
2. The schematic already links the firmware project; press **Run** and click the
   on-screen button to roll.
3. If the firmware link is broken, double-click U1 → *Edit Firmware Project* and
   point it at `_firmware_extras/FIRMWARE/PIC16LF1937/main.c`.

---

## 6. PCB & manufacturing

Two-layer FR‑4 board. Gerber set (Proteus CADCAM, X2 ASCII, 4.3, metric,
absolute):

| File | Layer |
|---|---|
| `dice - CADCAM Top Copper.GBR` | Top copper |
| `dice - CADCAM Bottom Copper.GBR` | Bottom copper |
| `dice - CADCAM Top Silk Screen.GBR` | Top silkscreen |
| `dice - CADCAM Top Solder Resist.GBR` | Top solder mask |
| `dice - CADCAM Bottom Solder Resist.GBR` | Bottom solder mask |
| `dice - CADCAM Top Assembly.GBR` | Assembly / component outlines |
| `dice - CADCAM Mechanical 1.GBR` | Board outline |
| `dice - CADCAM Drill TOP-BOT Plated.GBR` | Plated through-hole drill data |
| `dice - CADCAM Netlist.IPC` | IPC‑356 net list (bare-board test) |

**Layer stackup** (from `dice - CADCAM READ-ME.TXT`):

| Layer | Thickness | Material |
|---|---|---|
| Top solder resist | 0.010 mm | Resist |
| Top copper | 0.018 mm | Copper |
| Core | 1.550 mm | FR4 |
| Bottom copper | 0.018 mm | Copper |
| Bottom solder resist | 0.010 mm | Resist |

Send the eight `.GBR` files plus the drill file to any board house, or re-run
**Output → Generate CADCAM Files** in Proteus to regenerate them.

---

## 7. Credits

* Design & firmware: **Ailenei Daniel**
* Captured in Labcenter **Proteus 8.9**, firmware built with **Microchip MPLAB XC8**.
