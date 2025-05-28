# STM32F407 I2C1 + DMA Communication Driver

This repository contains a bare-metal implementation of I2C1 communication using DMA on the STM32F407 microcontroller. It is designed to work without HAL/LL libraries and can be easily integrated into custom embedded applications. The I2C interface is used to communicate with slave devices efficiently using DMA1 Stream5 (RX) and Stream6 (TX).

---

## 🛠️ Features

- I2C1 initialization with direct register access
- DMA1 Stream5 (RX) and Stream6 (TX) configuration
- Support for:
  - I2C memory write
  - I2C memory read
- Repeated START condition for read operations
- NVIC interrupt handling for DMA completion
- STM32F407 specific configuration

---

## 🧰 Hardware Configuration

| Peripheral | Pin | Function        |
|-----------:|-----|-----------------|
| I2C1       | PB6 | I2C1_SCL (AF4)  |
| I2C1       | PB7 | I2C1_SDA (AF4)  |
| DMA1       |     | Channel 1       |
| DMA1       | Stream5 | I2C1_RX    |
| DMA1       | Stream6 | I2C1_TX    |

> Refer to the STM32F407 **datasheet** and **reference manual** for AF and DMA mappings.

---
