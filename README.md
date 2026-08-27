# Flight Controller

A flight controller operating system built on FreeRTOS for the STM32F446RE.

## Purpose

This serves as a learning platform for me to understand the full extent of MCU programming, and as the source code for the flight controller in the STOL Design Team at Virginia Tech. I created my own HAL for GPIO and USART, and will program the MCU without using STM code generation. I will use assistive tools for Makefile production.

## Hardware

The microcontroller is the STM32F446RE dev board. It has enough peripheral pins for the entire project, as well as a debugger to allow for a rapid deploy-and-fix cycle. Here is the full spec list:

| Spec | Detail |
|------|--------|
| MCU | STM32F446RE (ARM Cortex-M4, 180 MHz, hardware FPU) |
| Board | ST Nucleo-64 (MB1136) |
| Flash | 512 KB |
| SRAM | 128 KB |
| Debugger | On-board ST-Link V2-1 (SWD) |

## Project Structure

```
Flight Controller/
├── app/
│   └── main.c                    # Application entry point and FreeRTOS task definitions
├── hal/
│   ├── gpio.c                    # GPIO driver (init, read, write, toggle, deinit)
│   ├── gpio.h                    # GPIO types, pin definitions, and function prototypes
│   ├── uart.c                    # UART driver with full init, pin muxing, and RCC clocks
│   └── uart.h                    # UART types, pin configurations, and TX/RX prototypes
├── config/
│   └── FreeRTOSConfig.h          # FreeRTOS kernel configuration (tick rate, heap, priorities)
├── cmsis/
│   ├── stm32f446xx.h             # CMSIS device header with register definitions for the MCU
│   ├── core_cm4.h                # CMSIS Cortex-M4 core peripheral access
│   ├── system_stm32f4xx.c        # System clock initialization (SystemInit, SystemCoreClock)
│   ├── startup_stm32f446xx.s     # Startup assembly with vector table and Reset_Handler
│   └── ...                       # Additional CMSIS compiler/version headers
├── freertos/
│   ├── src/                      # FreeRTOS kernel sources (tasks, queues, timers, etc.)
│   ├── port/                     # Cortex-M4 port layer (port.c, portmacro.h)
│   ├── heap/                     # FreeRTOS memory allocator (heap_4.c)
│   └── include/                  # FreeRTOS public headers
├── linker/
│   └── link.ld                   # Linker script defining memory layout for flash and SRAM
├── docs/
│   ├── NucleoPinOut.jpg           # Board pinout reference image
│   ├── rm0390-...pdf              # STM32F446xx Reference Manual
│   ├── stm32f446mc.pdf            # MCU Datasheet
│   └── um1724-...pdf              # Nucleo-64 User Manual
├── build/                         # Build output directory (gitignored)
└── Makefile                       # Build system with compile, link, flash, and clean targets
```

## Hardware Abstraction Layer

The core focus currently is building the custom Hardware Abstraction Layer (HAL) to be compatible with future modules like a motor controller, as well as an IMU. Sensor fusion will be made easier by these decisions. Most of the HAL is written from scratch in C using the reference manual.

### GPIO Driver

There are multiple GPIO functions that allow each pin to work:

```c
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_PinConfig_t *Config);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t Pin, GPIO_STATE_t State);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t Pin);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t Pin);
```

### UART Driver

This driver builds upon the GPIO Driver, containing these functions:

```c
// Initialization functions
void UART_Init(UART_HandleTypeDef *huart);
void UART_DeInit(UART_HandleTypeDef *huart);

// TX functions
void Transmit_Poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size,
                   uint32_t timeout);
void Transmit_Interrupt(UART_HandleTypeDef *huart, uint8_t *pData,
                        uint16_t Size);
void Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

// RX functions
uint8_t Recieve_Poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size,
                     uint32_t timeout);
uint8_t Recieve_Interrupt(UART_HandleTypeDef *huart, uint8_t *pData,
                          uint16_t Size);
uint8_t Recieve_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
```

## RTOS Configuration

This project uses the FreeRTOS kernel, which is included as source instead of a library, allowing further exploration of the kernel internals.

| Parameter | Value | Notes |
|-----------|-------|-------|
| `configUSE_PREEMPTION` | `1` | Preemptive scheduling enabled |
| `configCPU_CLOCK_HZ` | `SystemCoreClock` | Derived from system clock setup |
| `configTICK_RATE_HZ` | `1000` | 1 ms tick resolution |
| `configMAX_PRIORITIES` | `5` | Priority levels 0–4 |
| `configMINIMAL_STACK_SIZE` | `130` words | Minimum stack per task |
| `configTOTAL_HEAP_SIZE` | `75 KB` | FreeRTOS heap (heap_4 allocator) |
| `configMAX_TASK_NAME_LEN` | `10` | Characters per task name |
| `configUSE_MUTEXES` | `1` | Mutexes enabled |
| `configUSE_RECURSIVE_MUTEXES` | `1` | Recursive mutexes enabled |
| `configUSE_COUNTING_SEMAPHORES` | `1` | Counting semaphores enabled |
| `configUSE_TIMERS` | `1` | Software timers enabled |
| `configTIMER_TASK_PRIORITY` | `2` | Timer service task priority |
| `configTIMER_TASK_STACK_DEPTH` | `260` words | Timer task stack (2× minimal) |
| `configCHECK_FOR_STACK_OVERFLOW` | `0` | Stack overflow checking disabled |
| `configPRIO_BITS` | `4` | 15 NVIC priority levels (Cortex-M4) |
| `configLIBRARY_LOWEST_INTERRUPT_PRIORITY` | `0xF` | Lowest NVIC priority |
| `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` | `5` | Highest priority safe for FreeRTOS API calls |

## Building and Flashing

The project uses a Makefile-based build system with `arm-none-eabi-gcc` for cross-compilation and `st-flash` for deploying over the on-board ST-Link debugger.

### Prerequisites

- `arm-none-eabi-gcc` — ARM cross-compilation toolchain
- `st-flash` (from [stlink](https://github.com/stlink-org/stlink)) — flashing over the on-board ST-Link debugger
- `make`

### Windows setup

Build this project from the Windows Subsystem for Linux (WSL); native Windows command prompts are not supported. Install WSL with a Linux distribution (Ubuntu is recommended), then open its terminal and clone or access the repository from there.

Install the prerequisites inside WSL and run all `make` commands from the WSL terminal. If you need to flash the board from Windows, ensure its USB ST-Link device is available to WSL (for example, by using USB/IP passthrough) before running `make flash`.

### Compilation

Build the firmware (ELF, BIN, and HEX outputs):

```bash
make          # compile, link, and generate binaries
```

This compiles all sources with `-std=gnu11`, links against the linker script at `linker/link.ld`, and produces:

- `build/flight_controller.elf` — debug-ready ELF
- `build/flight_controller.bin` — raw binary for flashing
- `build/flight_controller.hex` — Intel HEX format

To view the resulting code and data sizes:

```bash
make size
```

### Flashing

Connect the Nucleo board via USB. The on-board ST-Link debugger appears as a USB device.

```bash
make flash    # write build/flight_controller.bin to flash at 0x08000000
```

To perform a full chip erase before flashing:

```bash
make erase    # erase the entire flash
make flash    # then re-flash
```

### Cleaning

Remove all build artifacts:

```bash
make clean
```

## Current Status

Currently developing USART drivers, with parallel development of SPI and I2C drivers.

## Roadmap

- [x] Startup code and linker script (board boots into `main()`)
- [x] GPIO driver with full init, write, toggle, read, and deinit
- [x] UART initialization with pin muxing, RCC clocks, baud/parity/stop config
- [x] FreeRTOS integration (scheduler runs, LED blink task works)
- [ ] UART TX/RX: polled, interrupt, and DMA transmit/receive functions
- [ ] I2C driver for IMU and barometer communication
- [ ] SPI driver for other sensor modules
- [ ] Sensor integration: IMU (accelerometer + gyroscope), magnetometer, barometer
- [ ] PID control loop for attitude stabilization as a real-time task
- [ ] PWM output for motor/ESC control via timer peripherals
- [ ] Assembly and iterative testing

## Reference Documents

Available in the `docs/` folder:

- **RM0390** — STM32F446xx Reference Manual (register maps, peripheral descriptions, clock trees)
- **STM32F446MC Datasheet** — Pinout, electrical characteristics, package information
- **UM1724** — Nucleo-64 User Manual (board layout, ST-Link configuration, jumper settings)
- **NucleoPinOut.jpg** — Quick-reference pinout diagram for the Nucleo board

## License

The FreeRTOS kernel sources included in `freertos/` are licensed under the MIT License by Amazon.com, Inc. CMSIS headers in `cmsis/` are provided by ARM Ltd. under their respective licenses.
