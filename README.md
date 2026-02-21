# Zephyr Workshop

Welcome to this Zephyr Workshop! This workshop is designed for everyone who is
new to Zephyr. This repository includes slides firmware samples and a modular
application. It links to existing references on how to get started and explains
basic RTOS concepts with examples.

The slides will guide you through the content and the hands-on parts of the
workshop. Please check out the pdf version of the slides at the [releases
section](https://github.com/jonas-rem/zephyr-workshop/releases)

Resources to quickly get started with Zephyr:

- [Zephyr Documentation](https://docs.zephyrproject.org/latest/)
- [Supported Boards](https://docs.zephyrproject.org/latest/boards/)
- [Samples and Demos](https://docs.zephyrproject.org/latest/samples/)
- [Zephyr GitHub Repository](https://github.com/zephyrproject-rtos/zephyr)

## Documentation and Slides 📖

The workshop includes both a web-based documentation (Sphinx) and a slide
presentation (Slidev). The slides are integrated into the documentation.

### Building the Documentation

The documentation is located in the `doc` folder. To build the full
documentation including the integrated slides, use `tox`:

```shell
cd doc
# Build integrated HTML documentation (Sphinx + Slides)
tox -e html
# Or for live preview with autobuild
tox -e docs
```

The output will be available in `doc/_build/html/index.html`.

> [!NOTE]
> Slidev uses ES Modules and cannot be opened directly via the `file://`
> protocol. You must serve the documentation via a web server
> (e.g., using `tox -e docs` for local preview).

### Slides 📊

The presentation slides are located in `doc/slides` and are written with
[Slidev](https://sli.dev/). They are automatically built and included when
building the documentation, but they can also be built standalone:

```shell
cd doc
tox -e slides
```

The standalone build output is located in
`doc/slides_dist/zephyr-workshop_slides`.

#### Live Preview (Development)
For the best experience when editing slides, use the Slidev development server which provides instant hot-reloading:

```shell
cd doc/slides
npm install
npm run dev
```

### Presentation License

The content in the `doc/slides` directory is licensed under the Creative
Commons Attribution-ShareAlike 4.0 International License (CC BY-SA 4.0). A full
copy of the license text is available in the `LICENSE_PRESENTATION` file at the
root of this repository.

## Firmware 🛠

### Samples

There are several samples in the folder `samples`. These samples are mostly
adapted from the [Zephyr GitHub
Repository](https://github.com/zephyrproject-rtos/zephyr) and referenced here
for a better overview. All samples can be built as Zephyr workspace examples.

### Modular Application
The modular application is a simple example of a modular application that
consists of multiple modules. Different modules communicate with each oter via
Zephyr's Zbus. The application is located at ´app´, related tests are in the
´tests´ folder.

The application should run on most supported boards as it only uses a button
and an led.

To run and explore integration tests with twister, you can build the tests,
application and samples for multiple boards:
```shell
west twister -T app/ -T test/ -T samples/ --integration
```

For a partial run (e.g., after changing a single file), you can filter for specific
tests or platforms while still running as integration test:
```shell
# Run only a specific test scenario (still as integration test)
west twister -T samples/ -s sample.basic.helloworld --integration

# Run tests only for a specific platform (still as integration test)
west twister -T samples/ -p native_sim --integration

# Run a specific sample directory only (still as integration test)
west twister -T samples/01_hello_world/ --integration
```

Twister will automatically reuse existing build directories unless they are
incompatible with the changes. Use `--pristine` or `-p always` to force a clean
rebuild when needed.

## Github Codespaces Environment
Just click on the button `<> Code` above to create a Github Codespaces
environment for the workshop. For further instructions, please refer to the
slides.

## Local Environment

### Setup the with existing Zephyr Project Workspace

After completing the Zephyr Getting Started guide, you should already have a
project folder setup. In this case, you can just clone this repository into the
existing `zephyrproject` folder:

```shell
# clone this repository in the existing zephyrproject folder
cd zephyrproject
git clone https://github.com/jonas-rem/zephyr-workshop
# Change west config manifest file location to the zephyr-workshop repository
west config manifest.path zephyr-workshop
cd zephyr-workshop
# update Zephyr modules
west update
```

### Setup from Scratch

To set up a new Zephyr workspace directly from this repository, west can be
used with the following options:

```shell
# Initialize a workspace named zephyrproject from this repository.
west init -m https://github.com/jonas-rem/zephyr-workshop --mr main zephyrproject
# update Zephyr modules
cd zephyrproject/zephyr-workshop
west update
```

### Building and running of the Firmware Samples

```shell
# e.g. for reel_board
west build -b reel_board samples/01_hello_world -p

# Or for native execution on the host system
west build -b native_sim samples/01_hello_world -p
```

Once the sample is build, run the following command to flash/run it:

```shell
# on a board
west flash

# native_sim execution
west build -t run
```

### Building and Running the Application on native_sim

The application can be built and run on `native_sim` for testing without hw:

```shell
# Build the application
west build -b native_sim app -p

# Run with shell on separate UART (creates predictable symlink)
./build/zephyr/zephyr.exe -uart_1_attach_uart_cmd='ln -sf %s /tmp/zephyr_shell'
```

This creates two separate consoles:
- **Logs** - Appear in the terminal output (uart0)
- **Shell** - Accessible via `/tmp/zephyr_shell` symlink (uart1)

Interact with the shell:

```shell
# Send commands to the application
echo "my_app button button_press" > /tmp/zephyr_shell
echo "help" > /tmp/zephyr_shell

# Or use screen for interactive session
tio /tmp/zephyr_shell
```
