# SIMPLEX Architecture

The SIMPLEX architecture is a simple stack-based machine used to demonstrate the implementation of assemblers and emulators.

Each instruction is encoded as a **32-bit word**.

* Lower **8 bits** → Opcode
* Upper **24 bits** → Operand

---

## Registers

The SIMPLEX machine contains four 32-bit registers:

| Register | Description                        |
| -------- | ---------------------------------- |
| A        | Accumulator                        |
| B        | Secondary register (used as stack) |
| PC       | Program Counter                    |
| SP       | Stack Pointer                      |

Registers **A and B operate as a small internal stack**.

---

## Instruction Format

```
| Operand (24 bits) | Opcode (8 bits) |
```

Example encoding:

```
000000000000000000000101 00000000
          operand            opcode
```

---

## Memory Model

* Memory stores **32-bit words**
* Code starts at **address 0**
* Instructions and data share the same memory

---

## Instruction Set

| Mnemonic | Opcode | Description                    |
| -------- | ------ | ------------------------------ |
| data     | -      | Reserve memory location        |
| ldc      | 0      | Load constant into accumulator |
| adc      | 1      | Add constant to accumulator    |
| ldl      | 2      | Load value from stack offset   |
| stl      | 3      | Store value to stack offset    |
| ldnl     | 4      | Load non-local                 |
| stnl     | 5      | Store non-local                |
| add      | 6      | Addition                       |
| sub      | 7      | Subtraction                    |
| shl      | 8      | Shift left                     |
| shr      | 9      | Shift right                    |
| adj      | 10     | Adjust stack pointer           |
| a2sp     | 11     | Transfer A to SP               |
| sp2a     | 12     | Transfer SP to A               |
| call     | 13     | Procedure call                 |
| return   | 14     | Return from procedure          |
| brz      | 15     | Branch if zero                 |
| brlz     | 16     | Branch if less than zero       |
| br       | 17     | Unconditional branch           |
| HALT     | 18     | Stop execution                 |

---

## Assembly Language Rules

* One instruction per line
* Comments begin with `;`
* Labels end with `:`
* Labels may be used as operands

Example:

```
start:
    ldc 5
    adc 3
    brz end
    br start
end:
    HALT
```

---

## Assembler Design

The assembler follows a **two-pass design**.

### Pass 1

* Read source file
* Identify labels
* Build **symbol table**
* Assign addresses

### Pass 2

* Translate instructions
* Resolve label references
* Generate machine code
* Produce listing file

---

## Emulator Execution Flow

```
Load object file
       ↓
Initialize registers
       ↓
Fetch instruction
       ↓
Decode opcode
       ↓
Execute instruction
       ↓
Update PC
       ↓
Repeat until HALT
```

---

## Example Execution

Assembly:

```
ldc 5
adc 3
HALT
```

Execution:

1. A ← 5
2. A ← A + 3
3. Program stops

Final value in accumulator:

```
A = 8
```

---

## Purpose of the Architecture

The SIMPLEX architecture is designed to help understand:

* Instruction encoding
* Stack-based computation
* Branching and procedure calls
* Assembler implementation
* Emulator design
