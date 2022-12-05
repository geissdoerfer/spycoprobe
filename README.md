# Spycoprobe

Spycoprobe is a USB programmer for TI MSP430FRxxxx devices. It receives commands from a host application via USB and interacts with an attached target device by bit-banging TI's Spy-Bi-Wire (SBW) protocol. The Spycoprobe firmware runs on Raspberry Pi's RP2040 microcontroller that is found on the Raspberry Pi Pico board.

Spycoprobe should work with all MSP430X devices. It has been tested with MSP430FR5994 and MSP430FR2433.

## Configuring the firmware

You can change the pins that are used for SBW programming on top of `src/main.c`. There are four relevant pins:
 - `PIN_SBW_TCK` is the SBW clock signal
 - `PIN_SBW_TDIO` is the SBW data signal
 - `PIN_SBW_DIR` is an **optional** signal specifying the direction of the `SBW_TDIO` signal. This can be used to switch directions of a level converter. Signal is high when data is sent from the probe to the target.
 - `PIN_TARGET_POWER` is an **optional** signal to enable/disable a power supply to the target. Signal is high when programmer is active.

You should replace the dummy USB vendor ID and product ID in `src/usb_descriptors.c` before using Spycoprobe.

## Building the firmware

Follow the [official instructions](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) to install and setup the Pico SDK.

Clone this repository:

```
git clone git@github.com:geissdoerfer/spycoprobe.git
```

Change to the `build` directory and configure cmake and build:

```
cd spycoprobe/firmware/build
cmake ..
make -j4
```

You should now find `spycoprobe.hex` and `spycoprobe.uf2` files in the `build` directory.

## Flashing the firmware

If you have a Raspberry Pico, disconnect it from your PC and keep pressing the `BOOTSEL` button while re-connecting the USB cable to your PC. A drive should appear on your PC. Copy the `spycoprobe.uf2` from the `build` dir onto this drive. The drive should disappear, while the bootloader flashes the new firmware. A new USB CDC ACM device should appear.

Alternatively, you can use any suitable SWD debug probe to upload the hex file to the RP2040.

## Usage

WIP

## References

 - Spycoprobe is related to [Picoprobe](https://github.com/raspberrypi/picoprobe), a Rasperry Pico based SWD debug probe
 - Some inspiration for the USB protocol comes from [Mspdebug](https://dlbeer.co.nz/mspdebug/) and [Goodfet](https://goodfet.sourceforge.net/)
 - The SBW routines in this project are based on TI's [SLAU320AJ](https://www.ti.com/lit/ug/slau320aj/slau320aj.pdf)
