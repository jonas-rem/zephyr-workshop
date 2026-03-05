# Zephyr Workshop

Welcome to this Zephyr Workshop! This workshop gives an introduction about main
concepts of Zephyr as well as architectural insights that are demonstrated with
an example application.

This repository includes slides, firmware samples and a modular application.
It references the existing Zephyr documentation on how to get started and
explains basic RTOS concepts with examples.

Resources to get started with Zephyr:

- [Zephyr Documentation](https://docs.zephyrproject.org/latest/)
- [Supported Boards](https://docs.zephyrproject.org/latest/boards/)
- [Samples and Demos](https://docs.zephyrproject.org/latest/samples/)
- [Zephyr GitHub Repository](https://github.com/zephyrproject-rtos/zephyr)

## Contents of the Repository

- [Workshop Slides]
- [Documentation](https://jonas-rem.github.io/zephyr-workshop/index.html)
- [Application]
- [GitHub Codespaces Setup](https://jonas-rem.github.io/zephyr-workshop/src/setup.html)

### Modular Application

The [Application] is a minimal firmware example, consisting of 3 components.
A button-controlled state machine (Sleep / Standby) with an LED, built using a
modular architecture. Components communicate via ZBus, keeping button and LED
logic decoupled and independently testable.
([source](https://github.com/jonas-rem/zephyr-workshop/tree/main/app))

### Samples

Progressive examples covering core Zephyr subsystems:

| # | Topic | Description |
|---|-------|-------------|
| [01](https://jonas-rem.github.io/zephyr-workshop/samples/01_hello_world/README.html) | Hello World | Basic application structure and board info |
| [02](https://jonas-rem.github.io/zephyr-workshop/samples/02_logging/README.html) | Logging | Log levels and the logging subsystem |
| [03](https://jonas-rem.github.io/zephyr-workshop/samples/03_workqueues/README.html) | Work Queues | Deferring work with timers and workqueues |
| [04](https://jonas-rem.github.io/zephyr-workshop/samples/04_shell/README.html) | Shell | Interactive CLI via the shell subsystem |
| [05](https://jonas-rem.github.io/zephyr-workshop/samples/05_sensor/README.html) | Sensor API | Reading temperature/humidity from a sensor |
| [06](https://jonas-rem.github.io/zephyr-workshop/samples/06_ble/README.html) | BLE | BLE peripheral with Health Thermometer Service |
| [07](https://jonas-rem.github.io/zephyr-workshop/samples/07_display_cfb/README.html) | Display | Text rendering with the CFB display subsystem |
| [08](https://jonas-rem.github.io/zephyr-workshop/samples/08_sys_init/README.html) | SYS_INIT | Automatic component initialization at boot |

## GitHub Codespaces Environment

On GitHub, click the `<> Code` button above to create a Codespaces environment
to quickly get a working setup for experimentation up and running.

## Documentation and Slides

The workshop includes both a web-based documentation (Sphinx) and a slide
presentation (Slidev). The slides are integrated into the documentation. View
the [Workshop Slides].

See [CONTRIBUTING.md](CONTRIBUTING.md) for instructions on building the
documentation and slides locally.

## License

You are welcome to use these materials to give this workshop yourself. Code is
licensed under [Apache 2.0](LICENSE), documentation and slides under [CC BY-SA
4.0](LICENSE_SLIDES).

[Workshop Slides]: https://jonas-rem.github.io/zephyr-workshop/src/slides.html
[Application]: https://jonas-rem.github.io/zephyr-workshop/app/README.html
