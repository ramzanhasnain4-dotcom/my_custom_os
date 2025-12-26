# 32-bit Preemptive Multitasking Kernel with MLFQ Scheduler

**Student Name:** Muhmmad Hasnain
**Course:** Operating Systems
**Date:** December 2025

![Status](https://img.shields.io/badge/Status-Completed-success)
![Platform](https://img.shields.io/badge/Platform-x86-blue)
![Language](https://img.shields.io/badge/Language-C%20%7C%20Assembly-orange)

## 📖 Project Overview
This project is a custom **32-bit Operating System Kernel** built from scratch. It boots into Protected Mode and features a **Preemptive Multitasking** system powered by a **Multilevel Feedback Queue (MLFQ)** scheduler.

Unlike basic cooperative schedulers, this kernel utilizes hardware interrupts (PIT) to forcibly context switch between tasks, ensuring fair CPU distribution and responsiveness. It successfully handles dynamic process creation, priority-based demotion, and safe task termination.

---

## ⚡ Key Features

### 1. Preemptive Multitasking
- **Hardware-Driven:** Uses the Programmable Interval Timer (PIT) to trigger interrupts at ~50Hz.
- **Context Switching:** Implemented in pure Assembly (`switch.s`) to save/restore general-purpose registers and CPU flags (EFLAGS) for seamless task swapping.

### 2. Advanced Scheduling (MLFQ)
The scheduler prioritizes tasks based on three queue levels:
- **Priority 0 (High/Green):** Short time slice (100 ticks). For new/interactive tasks.
- **Priority 1 (Medium/Yellow):** Medium time slice (300 ticks).
- **Priority 2 (Low/Red):** Long time slice (600 ticks). For CPU-heavy background tasks.
- **Demotion Logic:** Tasks that exhaust their time slice are automatically penalized and moved to a lower priority queue.

### 3. System Architecture
- **Multiboot Compliant:** Boots via GRUB or QEMU using a custom `boot.s` loader.
- **Flat Memory Model:** Custom Global Descriptor Table (GDT) mapping 4GB of memory.
- **Interrupt Management:** Remapped PIC (Programmable Interrupt Controller) to avoid conflict with CPU exceptions.

---

## 📂 File Structure

The project is organized into the core kernel and hardware-specific CPU drivers.

### Root Directory (`/my_os`)
| File | Description |
| :--- | :--- |
| `kernel.c` | Main C entry point. Handles VGA text output. |
| `boot.s` | Assembly bootloader. Sets up the stack and jumps to C. |
| `linker.ld` | Defines memory layout (1MB offset) for the linker. |
| `Makefile` | Build script to compile and link the OS image. |

### CPU Subsystem (`/my_os/cpu`)
| File | Description |
| :--- | :--- |
| `scheduler.c` | **The Core Logic.** Implements MLFQ, `create_task`, and `exit_task`. |
| `switch.s` | **Context Switcher.** Assembly magic to swap ESP registers. |
| `timer.c` | PIT driver. Fires the heartbeat interrupt for the scheduler. |
| `idt.c` | Interrupt Descriptor Table. Handles hardware interrupts (IRQs). |
| `gdt.c` | Global Descriptor Table. Sets up Protected Mode segments. |
| `interrupt.s` | Assembly stubs for catching ISRs and IRQs. |
| `ports.c` | I/O port helpers (`inb`, `outb`) to talk to hardware. |

---

## 🚀 How to Build & Run

### Prerequisites
You need a Linux environment (Ubuntu/WSL) with the following tools:
* `qemu-system-i386` (Emulator)
* `gcc` (Cross-compiler recommended)
* `nasm` (Assembler)
* `make`

### Commands
To perform a "nuclear clean" (delete all old binaries) and run the kernel immediately:

```bash
make fresh
