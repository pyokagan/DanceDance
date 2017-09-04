# DanceDance

## Arduino sketch

The `arduino/` directory contains the source files for the Arduino sketch that will be run on the Arduino Mega.

You can either compile/upload it with the Arduino IDE or with `make`.

### Usage with Arduino IDE

**NOTE**: You will need to use the latest version of Arduino IDE (>1.8.0). The version provided in the Debian/Ubuntu repositories is very old, go download it from the Arduino website instead.

1. Start the Arduino IDE.
1. Go to `File` -> `Preferences`
1. Set the "Sketchbook location" to this directory (where this `README.md` file resides). Save the preferences by clicking `OK`.
1. Go to `File` -> `Open`. Navigate to the `arduino` directory and open the `arduino.ino` file.
1. The sketch should be opened. You can now edit the source code and verify/upload it as needed.

### Usage with `make`

1. Set the `ARDUINO_DIR` environment variable to the location of your Arduino installation e.g. `/usr/share/arduino`.
1. `cd` to the `arduino/` directory in this project.
1. Run `make` to compile the sketch, or run `make upload` to compile and upload the sketch.

## Raspberry PI software

Clone the git repository into the raspberry pi and run `make` in the relevant directories to compile the relevant software:

* `raspi-monitor`: A simple program which prints all incoming UART communication frames to stdout.
