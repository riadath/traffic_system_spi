# STM32F4 SPI Device Drivers

This repository contains device drivers for SPI (Serial Peripheral Interface) communication on STM32F4 microcontrollers with ARM Cortex-M4 processors. The drivers enable you to configure and use the SPI interface for various applications.

## Features

- SPI initialization and configuration.
- Support for both master and slave modes.
- Flexible baud rate and data format settings.
- Data transmission and reception.
- Interrupt-based communication for the slave mode.

## Files

- `SPI.h`: Header file for SPI driver functions.
- `SPI.c`: Source file containing SPI driver implementation.
- `main.c`: Example usage of the SPI driver.

## Usage

1. Include the necessary header files in your project.
2. Initialize and configure SPI using `SPI1_Config(ifMaster)`, where `ifMaster` determines whether the device is configured as a master or a slave.
3. Use functions like `SPI1_Send` for transmitting data and `SPI1_Receive` for receiving data.
4. Configure baud rate, data format, and other settings as needed.
5. Handle interrupts for slave mode communication.

## Example

You can refer to the `main.c` file for an example of how to use the SPI driver for both master and slave modes.

## License

This project is open-source and provided under the [license](LICENSE) included in this repository.
