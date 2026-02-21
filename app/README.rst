Example APP
###########

**Link to Source:** `app <https://github.com/jonas-rem/zephyr-workshop/tree/main/app>`_

Overview
********

This application demonstrates a simple state machine controlled by a button
press. When the button is pressed, the system toggles between two states:
**Sleep** and **Standby**:

- **Sleep**: LED is off, system waits for events (blocking)
- **Standby**: LED blinks continuously (50ms on, 500ms off)

The application showcases modular design with inter-module communication via
ZBus, allowing the Button module to notify the LED module of state changes
without direct coupling. This decoupled architecture enables testing individual
modules in isolation.

This application is useful for:

- Understanding modular architecture in Zephyr
- Learning about ZBus for inter-module communication
- Using SYS_INIT for application initialization
- Testing with native_sim and twister

Key Features:

- **Button Module**: Handles physical button presses via GPIO interrupts
- **LED Module**: Controls LED state based on system state (runs in own thread)
- **Main Application (sys_ctrl)**: Manages system state machine and coordinates modules

Architecture
************

The application follows a modular design where functionality is organized into
reusable components. The source code is located in the ``src/`` directory, with
a clear separation between common infrastructure and feature-specific modules.

The ``common/`` directory contains shared components like message channels that
enable inter-module communication, while the ``modules/`` directory houses the
individual functional units. Each module is self-contained with its own source
files, build configuration, and Kconfig options, promoting reusability and
testability.

.. code-block:: text

   app
   ├── boards
   │   └── native_sim.overlay
   ├── CMakeLists.txt
   ├── Kconfig
   ├── prj.conf
   ├── sample.yaml
   └── src
       ├── common
       │   ├── CMakeLists.txt
       │   ├── message_channel.c
       │   └── message_channel.h
       ├── main.c
       └── modules
           ├── button
           │   ├── button.c
           │   ├── CMakeLists.txt
           │   └── Kconfig.button
           ├── button_mock
           │   ├── button_mock.c
           │   ├── CMakeLists.txt
           │   └── Kconfig.button_mock
           └── led
               ├── CMakeLists.txt
               ├── Kconfig.led
               └── led.c

Initialization using SYS_INIT
=============================

In true Zephyr fashion, the application's central logic (``sys_ctrl``) is
initialized independently of the ``main()`` function. Zephyr provides the
``SYS_INIT`` macro, which allows modules to inject their initialization routines
into the OS boot sequence automatically.

By using ``SYS_INIT(sys_ctrl_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY)``,
the system controller registers itself at the ``APPLICATION`` initialization level.
This ensures that drivers and ZBus are fully operational before the state machine
begins listening to button presses and broadcasting LED states. Consequently,
``main.c`` is completely decoupled from the application logic and is merely a
lightweight placeholder.

System States
=============

The application implements two system states:

- **Sleep** (``SYS_SLEEP``): LED is off, system waits for events
- **Standby** (``SYS_STANDBY``): LED blinks, system is active

Button presses toggle between these states via ZBus messages.

Module Communication
====================

Modules communicate via ZBus channels:

- ``button_ch``: Button press events (publisher: Button module, subscriber: Main app)
- ``led_ch``: LED state changes (publisher: Main app, subscriber: LED module)

Hardware Abstraction
====================

On native_sim, the application uses:

- **GPIO Emulator** (zephyr,gpio-emul) for button and LED
- **PTY UART** for shell on separate console
- **Devicetree Overlay** to define button alias and UART configuration

The overlay file (``app/boards/native_sim.overlay``) defines:

- ``sw0`` alias for the button
- ``uart1`` enabled for shell
- Shell UART mapped to ``zephyr,shell-uart``

Building
********

The ``native_sim`` board allows Zephyr applications to be compiled as native
Linux executables, enabling development and testing without physical hardware.

Build the application for native_sim:

.. code-block:: console

   host:~$ west build -b native_sim app -p

To build for a real board (e.g., reel_board@2), use:

.. code-block:: console

   host:~$ west build -b reel_board@2 app -p
   host:~$ west flash

Running with Separate Shell Console
===================================

Run the application with shell on a separate UART/console:

.. code-block:: console

   host:~$ ./build/zephyr/zephyr.exe -uart_1_attach_uart_cmd='ln -sf %s /tmp/zephyr_shell'

When you run the application, it creates two pseudo-terminals (PTYs):

- **uart** (/dev/pts/X) - Main console (logs and shell if not redirected)
- **uart_1** (/dev/pts/Y) - Shell console when using ``-uart_1_attach_uart_cmd``

With the ``-uart_1_attach_uart_cmd`` option, the shell UART is redirected to a
separate PTY and linked to ``/tmp/zephyr_shell``. The main UART output (logs)
remains on the terminal where you started the application.

Connect to the shell using:

.. code-block:: console

   host:~$ tio /tmp/zephyr_shell

Sample Output
*************

Without UART redirection:

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
   [00:00:00.100,000] <inf> sys_ctrl: System initialized in SLEEP state
   [00:00:00.100,000] <inf> led: LED module initialized
   [00:00:00.100,000] <inf> button: Button module initialized

With shell on separate console (logs in main terminal):

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
   [00:00:00.100,000] <inf> sys_ctrl: System initialized in SLEEP state
   [00:00:00.100,000] <inf> led: LED module initialized
   [00:00:00.100,000] <inf> button: Button module initialized

Exit native_sim by pressing :kbd:`CTRL+C`.

Shell Commands
**************

Two methods are available to trigger button events:

**1. GPIO Level (Hardware Simulation)**

Uses the GPIO emulator to simulate physical button press:

.. code-block:: console

   uart:~$ mock_button

This triggers the actual GPIO interrupt handler with debouncing.

**2. Application Level (ZBus)**

Sends button event directly via ZBus:

.. code-block:: console

   uart:~$ my_app button button_press

This bypasses the GPIO layer and sends the event directly to the application.

Testing
*******

The application uses a modular testing approach where individual modules can be
tested in isolation.

Run all integration tests:

.. code-block:: console

   host:~$ west twister -T app/ -T test/ -T samples/ --integration

Run tests for a specific module:

.. code-block:: console

   host:~$ west twister -T test/button --integration

Run a single test by its scenario name:

.. code-block:: console

   host:~$ west twister -s sample.basic.button
   host:~$ west twister -s sample.basic.led

Resources
*********

- `Zephyr ZBus Documentation <https://docs.zephyrproject.org/latest/services/zbus/index.html>`_
- `native_sim Board Documentation <https://docs.zephyrproject.org/latest/boards/native/native_sim/doc/index.html>`_
- `Zephyr Testing with Twister <https://docs.zephyrproject.org/latest/develop/test/twister.html>`_
