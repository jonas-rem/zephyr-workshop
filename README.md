# Zephyr Workshop

Welcome to this Zephyr Workshop! This workshop is designed for students and
beginners who are new to Zephyr. This repository includes slides and firmware
examples. It links to existing references on how to get started and explains
basic RTOS concepts with examples.

Resources to quickly get started with Zephyr:

- [Zephyr Documentation](https://docs.zephyrproject.org/latest/)
- [Supported Boards](https://docs.zephyrproject.org/latest/boards/)
- [Samples and Demos](https://docs.zephyrproject.org/latest/samples/)
- [Zephyr GitHub Repository](https://github.com/zephyrproject-rtos/zephyr)

## Prerequisites 📚

Before diving into the samples, please make sure you have a working Zephyr
setup. Follow the [Zephyr Getting Started
Guide](https://docs.zephyrproject.org/latest/develop/getting_started/) to get
everything up and running.

## Slides 📊

The presentation slides are written with Latex Beamer and the
[mtheme](https://github.com/matze/mtheme/tree/master). To build the
presentation and generate a PDF file, execute `make`.

## Firmware Samples 🛠

There are several samples in the folder `samples`. These samples are mostly
adapted from the [Zephyr GitHub
Repository](https://github.com/zephyrproject-rtos/zephyr). All samples can be
built as out-of-tree examples.

### Setup the with existing Zephyr Project Workspace

After completing the Zephyr Getting Started guide, you should already have a
project folder setup. In this case, you can just clone this repository into the
existing `zephyrproject` folder:

```shell
# clone this repository in the existing zephyrproject folder
cd zephyrproject
git clone https://github.com/jremmert-phytec-iot/zephyr-workshop
# Change west config manifest file location to the zephyr-workshop repository
west config manifest.path zephyr-workshop
# update Zephyr modules
west update
```

### Setup from Scratch

To set up a new Zephyr workspace directly from this repository, west can be
used with the following options:

```shell
# Initialize a workspace named zephyrproject from this repository.
west init -m https://github.com/jremmert-phytec-iot/zephyr-workshop --mr main zephyrproject
# update Zephyr modules
cd zephyrproject
west update
```

### Building and running of the Firmware Samples

```shell
# e.g. for reel_board
west build -b reel_board samples/01_hello_world -p

# Or for a simulation that can run on the host system
west build -b qemu_cortex_m0 samples/01_hello_world -p
```

Once the sample is build, run the following command to flash/run it:

```shell
# on a board
west flash

# qemu simulation
west build -t run
```
