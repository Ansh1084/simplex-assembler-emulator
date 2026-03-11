# SIMPLEX Assembler and Emulator

This project implements a **two-pass assembler and emulator for the SIMPLEX instruction set architecture**.

The assembler reads SIMPLEX assembly programs, resolves labels, converts instructions into machine code, and generates both an **object file** and a **human-readable listing file**.
The emulator loads the generated object code and executes it by simulating the SIMPLEX machine.

This project was developed as part of the **CS2206 Computer Architecture Mini Project**.

---

## Features

### Two-Pass Assembler

* Reads assembly source files
* Builds a **symbol table for labels**
* Resolves forward and backward references
* Converts assembly instructions to **machine code**
* Detects common assembly errors:

  * Duplicate labels
  * Undefined labels
  * Invalid mnemonics
  * Invalid operands

### Object File Generation

* Produces a **binary object file**
* Code starts from memory address `0`
* Each instruction encoded as **32-bit machine code**

### Listing File Generation

Produces a readable listing showing:

* Memory address
* Machine instruction
* Corresponding mnemonic

Example:

```
00000000 00000111 br start
00000001 00005AB4 data 0x5ab4
00000002          start:
00000002 00006500 ldc 0x65
```

### Emulator

The emulator simulates the SIMPLEX machine:

* Loads object files
* Executes instructions
* Maintains registers
* Handles branching and procedure calls
* Stops execution on `HALT`

### Debug Utilities

* Memory inspection using **hex dump**
* Test cases for correct and incorrect programs

---

## Project Structure

```
simplex-assembler-emulator
│
├── src
│   ├── assembler.cpp
│   ├── emulator.cpp
│   └── hexdump.cpp
│
├── tests
│   ├── successful_tests
│   └── failure_tests
│
├── scripts
│   └── run_tests.bat
│
├── Makefile
├── README.md
├── ARCHITECTURE.md
└── .gitignore
```

---

## Build Instructions

Compile using the Makefile:

```
make
```

This produces:

```
assembler
emulator
```

---

## Running the Assembler

```
./assembler program.asm
```

Output files generated:

* `program.obj` → machine code
* `program.lst` → listing file

---

## Running the Emulator

```
./emulator program.obj
```

The emulator loads the object file and executes the program until `HALT`.

---

## Example SIMPLEX Program

```
ldc 5
adc 3
HALT
```

Expected result:

* Loads constant 5
* Adds 3
* Program halts

---

## Testing

The project contains automated test cases.

```
tests/
    successful_tests/
    failure_tests/
```

Successful tests verify correct assembly and execution.
Failure tests verify error detection such as:

* Invalid instruction
* Undefined label
* Duplicate label

Tests can be executed using:

```
scripts/run_tests.bat
```

---

## Technologies Used

* C / C++
* File I/O
* Data structures (maps / symbol tables)
* Two-pass assembler design
* Instruction decoding
* Memory simulation

---

## Learning Outcomes

This project demonstrates:

* Assembler design
* Instruction encoding
* Label resolution
* Symbol table construction
* Emulator implementation
* Low-level architecture concepts

---

## Author

Ansh Goyal
Computer Science Engineering
