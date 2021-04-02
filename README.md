# miyooctl2

A command line program for Miyoo devices (bittboy, pocket go v1, powkiddy v90/q90) for talking to miyoo-specific linux kernel modules. It can set "versions", volume, and turn rumble on/off.

## Usage

`miyooctl2 [-h] [-i] [-v] [-V volume(0-10)] [-k keypad_ver(1-4)] [-m rumble_ver(1-3)] [-M rumble_mode(0-1)] [-s screen_ver(1-4)]`

* `-h` prints the above help line
* `-i` prints the information that the modules report (currently the screen version and volume)
* `-v` verbose: print what is supposed to be happening
* `-V` sets the volume; range 0 to 10
* `-k` sets the keypad version; range 1 to 4. Note this affect how your keypad works - an incorrect version can make your keypad stop working until you shut down the device.
* `-m` sets the vibration/rumble motor version; range 1-3. (Not sure that this actually works.)
* `-M` sets the vibration/rumble motor on/off; 0=on, 1=off
* `-s` sets the screen version; range 1 to 4. Note that an incorrect version will effectively disable your screen until you shut down the device.

## Compiling

A toolchain for cross-compiling is required.

Inspect the `Makefile` and adjust to your setup (e.g. `PREFIX`). It is also possible to specify these on command line, e.g. `make PREFIX=arm-linux-gnueabi-`.

## Credits

* original version by James O'Farrell
* rewrite (v2) by flabbergast
