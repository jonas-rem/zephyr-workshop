# Zephyr Workshop

This repository contains slides and firmware examples for a Zephyr workshop.
The workshop targets students and beginners. It lists existing references on
how to get started and explains basic RTOS concepts with examples.

## Slides

The presentation slides are written with Latex Beamer and the
[mtheme](https://github.com/matze/mtheme/tree/master). To build the
presentation and generate a pdf file execute `make`.

## Firmware Samples

There are several samples in the folder `samples`. The samples are mostly taken
from the samples in the [Zephyr GitHub
Repository](https://github.com/zephyrproject-rtos/zephyr). All samples can be
build as an out-of-tree example.

### Initialization of the Zephyr Repository

To setup a new Zephyr workspace, west can be used with the following options:

```shell
# Initialize a workspace named zephyrproject from this repository.
west init -m https://github.com/zephyrproject-rtos/example-application --mr main zephyrproject
# update Zephyr modules
cd zephyrproject
west update
```

### Building and running of the Firmware Samples

```shell
# e.g. for reel_board
west build -b reel_board samples/01_hello_world -p

# Or for a simulation that can run on the host system
west build -b native_posix samples/01_hello_world -p
```

Once the sample is build, run the following command to flash/run it:

```shell
# on a board
west flash

# native_posix
./build/zephyr/zephyr.exe
```
