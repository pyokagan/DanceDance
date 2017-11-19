# DanceDance

## Arduino sketch

The `arduino/` directory contains the source files for the Arduino sketch that will be run on the Arduino Mega.

You can compile it by running `make` in the directory.

**NOTE**: You will need to use the latest version of Arduino IDE (>1.8.0). The version provided in the Debian/Ubuntu repositories is very old, go download it from the Arduino website instead.

### Usage with `make`

1. Set the `ARDUINO_DIR` environment variable to the location of your Arduino installation e.g. `/usr/share/arduino`.
1. `cd` to the `arduino/` directory in this project.
1. Run `make` to compile the sketch, or run `make upload` to compile and upload the sketch.

## Raspberry PI software

### Installing systemd daemons

The `raspi-service.py` script will install the necessary systemd unit files to run `raspi-uart`, `raspi-ml` and `raspi-eval` as a service.

Undeterministic things will happen if you run multiple copies of this software at the same time, so when e.g. running the `raspi-uart` program on the command line, make sure its associated service (`raspi-uart.service`) has been stopped first.

### C software

Clone the git repository into the raspberry pi and run `make` in the relevant directories to compile the relevant software:

* `raspi-monitor`: A simple program which prints all incoming UART messages to stdout.
* `raspi-uart`: The UART communication program that is run on the system.

### Python software
Run them directly, but you will need to have the necessary dependencies installed first.

* `raspi-ml.py`: Machine learning process
* `raspi-eval.py`: Evaluation server communication process.

## Libraries

### ucomm

Located at `libraries/ucomm`.

The `ucomm` library is used for UART communication between the Arduino and Raspberry Pi.
It will automatically be compiled and linked against the software (i.e. arduino, raspi-uart) that needs it.

Run `make test` in the directory to compile and run some tests.

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

        There is no need to specify a `routers` or `domain_name_servers`.
