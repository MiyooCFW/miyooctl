# Miyoo Control v2

A command line program for Miyoo devices (bittboy, pocket go v1, powkiddy v90/q90) for talking to miyoo-specific linux kernel modules. It can set "versions", volume, and turn rumble on/off.

## Usage

`miyooctl [-h] [-i] [-v] [-V volume(0-10)] [-k keypad_ver(1-6)] [-l layout_ver(1-3)] [-m rumble_ver(1-3)] [-M rumble_mode(0-1)] [-s screen_ver(1-4)] [-f fpbp_hexbyte]`

* `-h` prints the above help line
* `-i` prints the information that the modules report (currently the volume, keypad version & layout)
* `-v` verbose: print what is supposed to be happening
* `-V` sets the volume; range 0 to 10
* `-k` sets the keypad version; range 1 to 6. Note this affect how your keypad works - an incorrect version can make your keypad stop working until you shut down the device.
* `-l` sets the keypad layout; range 1 to 3. Note this affect how your face buttons work
* `-m` sets the vibration/rumble motor version; range 1-3. (Not sure that this actually works.)
* `-M` sets the vibration/rumble motor on/off; 0=on, 1=off
* `-s` sets the screen version; range 1 to 4. Note that an incorrect version will effectively disable your screen until you shut down the device.
* `-f` sets FP/BP for the screen driver

**Note**: In the latest Miyoo cfw builds you can use it in the `normalboot.custom.sh` bash file located on `boot/variants/<CONSOLE_VARIANT>`

## Compiling

A toolchain for cross-compiling is required.

Inspect the `Makefile` and adjust to your setup (e.g. `PREFIX`). It is also possible to specify these on command line, e.g. `make PREFIX=arm-linux-gnueabi-`.

Run `make`.

## FP/BP

This is "front porch" and "back porch" setting for the LCD controller. The expected format is a single byte in hex format, so '00' to 'FF'. The upper four bits are FP value, the lower four bits are BP value. So, for example, `A9` will set FP to 10 and BP to 9.

For example, in the screen "version 2", this is what page 263 of [ST7789V datasheet](http://www.lcdwiki.com/res/MSP1141/ST7789VW_datasheet.pdf) refers to.

## Credits

* Original version by James O'Farrell
* Rewrite (v2) by flabbergast
