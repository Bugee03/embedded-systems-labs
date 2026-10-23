# 8051_LAB_ASM

## 8051 (MCS-51) Microcontroller Laboratory Repository

This repository documents my structured learning and laboratory work on the **8051 microcontroller architecture** using both:

- 8051 Assembly (MCS-51 ISA)
- Embedded C (SDCC / Keil compatible)

The goal of this project is to build a strong low-level understanding of microcontroller systems — from instruction-level programming to hardware-level control.

---

# 🎯 Learning Objectives

✔ Understand 8051 architecture (register banks, RAM, SFRs)  
✔ Master addressing modes and instruction set  
✔ Work with stack, calls, interrupts  
✔ Control GPIO and bit-level operations  
✔ Implement timing and delay routines  
✔ Use UART serial communication  
✔ Write embedded C for 8051  
✔ Understand memory layout and compilation outputs  

This repository is part of my embedded systems engineering training.

---

# 🧠 8051 Architecture Focus

- Harvard architecture
- 8-bit CPU
- Internal RAM (128 bytes)
- Special Function Registers (SFR)
- Bit-addressable memory
- Stack operations
- Timer modules
- UART serial interface

---

# 📂 Repository Structure

8051/
├── ASM/ → 6 assembly labs (addressing, arithmetic, jumps & stack, I/O bits, tables, serial)
└── C/   → 4 embedded‑C labs for SDCC (GPIO toggle ×2, delay, UART)



---

# 🔧 ASM Laboratory Modules

| Lab | Topic | Concepts |
|------|--------|------------|
| Lab 01 | Addressing Modes | Immediate, Direct, Indirect |
| Lab 02 | Arithmetic | ADD, SUBB, BCD, Flags |
| Lab 03 | Jumps & Stack | CALL, RET, PUSH, POP |
| Lab 04 | Bit & I/O | Port manipulation, flags |
| Lab 05 | Tables | Lookup tables |
| Lab 06 | Serial | UART basics |

Each lab contains:
- Assembly source files
- Lab README
- Explanation of behavior

---

# 💻 C Laboratory Modules

Embedded C examples demonstrating:

- GPIO control
- Delays
- Memory handling
- Register manipulation
- Basic hardware abstraction

Toolchain:
- SDCC (recommended)
- Keil µVision (optional)

---

# 🚀 Build & Run

## Using SDCC

Example:

```bash
sdcc main.c
packihx main.ihx > main.hex

Flash .hex to:
Real 8051 board
Or simulator
```


📘 What This Repository Demonstrates
This is not just code collection.
It demonstrates:
Structured embedded learning
Low-level hardware control
Understanding of microcontroller internals
Clean lab organization
Professional repository structure
📌 Future Improvements
Add timer interrupt labs
Add ADC interface (external)
Add LCD interface example
Add GitHub Actions CI for SDCC builds
Add schematic diagrams
👨‍💻 Author
Electrical & Electronics Engineering Student
Embedded Systems Focus
Learning path: 8051 → STM32 → MSP430 → Advanced Control Systems

