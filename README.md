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

## Set up Raspberry Pi

1. Write the Raspbian Image to the Raspberry Pi, i.e.

    ```
    dd bs=4M if=2017-08-16-raspbian-stretch-lite.img of=/dev/sdX oflag=sync status=progress
    partprobe /dev/sdX
    ```

1. Mount the `/boot` partition of the SD card.

    1. Create an empty `ssh` file in the `/boot` partition, i.e. (in the mounted `/boot` partition directory)

        ```
        sudo touch ssh
        ```

    1. Open `/boot/cmdline.txt` and:

        * Remove the console entry that refers to `serial0`, i.e. `console=serial0,115200`.
        * Append `net.ifnames=0` to the end.

    1. Open `/boot/config.txt`, and add the following line to the end of the file:

        ```
        dtoverlay=pi3-disable-bt
        ```
 
1. Mount the root partition of the SD card.

    1. Open `/etc/dhcpcd.conf`, and look for the following similar lines and modify it such that it looks like:

        ```
        # Example static IP configuration:
        interface eth0
        static ip_address=192.168.3.2/24
        ```

        where `192.168.3.2` is your desired static IP.
