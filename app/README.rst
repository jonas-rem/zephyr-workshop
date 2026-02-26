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

Tracing with CTF
****************

This application supports tracing using CTF (Common Trace Format). CTF is a
binary trace format that allows you to visualize thread scheduling, interrupts,
synchronization primitives, and more using tools like Trace Compass.

Two configuration files are provided for different backends:

- ``prj_native_ctf.conf``: For native_sim (outputs to file)
- ``prj_usb_ctf.conf``: For hardware boards via USB

Prerequisites
=============

Install Trace Compass for viewing traces:

.. code-block:: console

   host:~$ sudo snap install trace-compass   # Ubuntu
   # OR download from https://eclipse.dev/tracecompass/

For USB capture, install pyusb:

.. code-block:: console

   host:~$ sudo apt install python3-usb

Option 1: native_sim (POSIX Backend)
====================================

Step 1: Build the application with CTF tracing
------------------------------------------------

.. code-block:: console

   host:~$ west build -b native_sim app -p -- \
               -DEXTRA_CONF_FILE=prj_native_ctf.conf

Step 2: Create trace output directory
--------------------------------------

.. code-block:: console

   host:~$ mkdir -p traces

Step 3: Run the application and capture trace data
---------------------------------------------------

.. code-block:: console

   host:~$ ./build/zephyr/zephyr.exe -trace-file=traces/channel0_0

Let the application run for 10-30 seconds, then press :kbd:`CTRL+C` to stop.

Step 4: Verify trace files
---------------------------

Your traces directory should now contain:

.. code-block:: none

   traces/
   ├── channel0_0    # Binary trace data (generated during run)
   └── metadata      # CTF format description (from zephyr/subsys/tracing/ctf/tsdl/)

Option 2: Hardware Board via USB (e.g., nrf52840dk)
===================================================

Step 1: Build the application with USB CTF tracing
---------------------------------------------------

.. code-block:: console

   host:~$ west build -b nrf52840dk/nrf52840 app -p -- \
               -DEXTRA_CONF_FILE=prj_usb_ctf.conf

Step 2: Flash the firmware to the board
-----------------------------------------

.. code-block:: console

   host:~$ west flash

Step 3: Connect USB cable
--------------------------

Connect a USB cable from the nrf52840dk's **USB port** (not the J-Link USB)
to your host computer.

Step 4: Verify USB device is detected
--------------------------------------

The board should enumerate as a USB device:

.. code-block:: console

   host:~$ lsusb | grep Nordic
   Bus 003 Device 082: ID 2fe3:0001 NordicSemiconductor USBD sample

**Understanding VID and PID:**

The output shows ``ID 2fe3:0001`` where:

- **VID** (Vendor ID) = ``0x2FE3`` - Identifies the manufacturer (Nordic Semiconductor)
- **PID** (Product ID) = ``0x0001`` - Identifies the specific device (Zephyr USB device)

To see all USB devices without filtering:

.. code-block:: console

   host:~$ lsusb

Look for your board in the output. The format is always ``ID VID:PID Description``.

**Common VIDs:**

- ``0x2FE3``: Nordic Semiconductor (nrf52840dk, nrf5340dk, etc.)
- ``0x1366``: SEGGER (J-Link debuggers)
- ``0x0483``: STMicroelectronics (ST-Link, STM32 boards)

Step 5: Create trace directory
-------------------------------

.. code-block:: console

   host:~$ mkdir -p traces

Step 6: Capture trace data
---------------------------

Run the capture script for 10-20 seconds, then press :kbd:`CTRL+C`:

.. code-block:: console

   host:~$ python3 zephyr/scripts/tracing/trace_capture_usb.py \
               -v 0x2FE3 -p 0x0001 -o traces/channel0_0

The capture script parameters are:

- ``-v 0x2FE3``: Vendor ID (from Step 4)
- ``-p 0x0001``: Product ID (from Step 4)
- ``-o traces/channel0_0``: Output file path

Replace the VID and PID values with those you found in Step 4.

Step 7: Verify trace files
---------------------------

.. code-block:: console

   host:~$ ls -lh traces/
   total 8.1M
   -rw-r--r-- 1 root root 8.1M Feb 26 18:05 channel0_0
   -rw-rw-r-- 1 user user  29K Feb 26 18:06 metadata

Viewing Traces in Trace Compass
================================

Step 1: Launch Trace Compass
-----------------------------

.. code-block:: console

   host:~$ trace-compass &

Step 2: Import the Zephyr parser (first time only)
---------------------------------------------------

1. Clone the parser repository:

   .. code-block:: console

      host:~$ git clone https://github.com/ostrodivski/zephyr-tracecompass-parser.git

2. In Trace Compass: Window → Preferences → Tracing → Custom Parsers

3. Click "Import" and select the XML files from the cloned repository

Step 3: Open the trace
-----------------------

1. File → Open Trace
2. Browse to your ``traces/`` directory
3. Select the folder (not individual files)
4. Click "Open"

Step 4: Explore the trace
--------------------------

Trace Compass will display:

- **Kernel Overview**: Thread states and CPU usage
- **Thread Scheduling**: Timeline showing when each thread runs
- **Interrupts**: ISR entry and exit events
- **Synchronization**: Semaphore and mutex operations
- **Custom Events**: Any application-specific trace points

Configuration Files Explained
==============================

prj_native_ctf.conf
--------------------

.. code-block:: cfg

   # Enable tracing subsystem
   CONFIG_TRACING=y
   CONFIG_TRACING_CTF=y
   CONFIG_TRACING_SYNC=y
   CONFIG_TRACING_BACKEND_POSIX=y
   CONFIG_TRACING_PACKET_MAX_SIZE=64

   # Trace events to capture
   CONFIG_TRACING_THREAD=y    # Thread create/exit/switch
   CONFIG_TRACING_ISR=y       # Interrupt entry/exit
   CONFIG_TRACING_SEMAPHORE=y # Semaphore operations
   CONFIG_TRACING_MUTEX=y     # Mutex operations
   CONFIG_TRACING_WORK=y      # Work queue operations
   CONFIG_TRACING_SYSCALL=y   # System calls

   # Enable thread names in traces
   CONFIG_THREAD_NAME=y
   CONFIG_DEBUG_THREAD_INFO=y

prj_usb_ctf.conf
-----------------

.. code-block:: cfg

   # USB device stack
   CONFIG_GPIO=y
   CONFIG_USB_DEVICE_STACK_NEXT=y
   CONFIG_CDC_ACM_SERIAL_INITIALIZE_AT_BOOT=n

   # Enable tracing subsystem with USB backend
   CONFIG_TRACING=y
   CONFIG_TRACING_CTF=y
   CONFIG_TRACING_ASYNC=y
   CONFIG_TRACING_BACKEND_USB=y
   CONFIG_TRACING_HANDLE_HOST_CMD=y
   CONFIG_TRACING_BUFFER_SIZE=4096

   # Trace events (same as native_sim)
   CONFIG_TRACING_THREAD=y
   CONFIG_TRACING_ISR=y
   CONFIG_TRACING_SEMAPHORE=y
   CONFIG_TRACING_MUTEX=y
   CONFIG_TRACING_WORK=y
   CONFIG_TRACING_SYSCALL=y

   # Thread names
   CONFIG_THREAD_NAME=y
   CONFIG_DEBUG_THREAD_INFO=y

Troubleshooting
===============

USB device not detected
------------------------

- Check physical USB connection (use the board's USB port, not J-Link)
- Verify the board is powered
- Try a different USB cable
- Check ``lsusb`` output

Permission denied on USB capture
---------------------------------

The capture script requires root privileges to access USB devices:

.. code-block:: console

   host:~$ python3 zephyr/scripts/tracing/trace_capture_usb.py ...

Trace Compass shows "No events"
--------------------------------

- Verify both ``channel0_0`` and ``metadata`` files are in the same directory
- Check that the metadata file is not empty (should be ~29KB)
- Ensure the Zephyr parser is imported in Trace Compass

Capture script missing dependency
----------------------------------

If you see ``ModuleNotFoundError: No module named 'usb'``:

.. code-block:: console

   host:~$ sudo apt install python3-usb

Trace file is empty
--------------------

- Ensure the application is actually running (check console output)
- For USB: verify the device is enumerated (check ``lsusb``)
- For native_sim: ensure the trace file path is correct

Resources
*********

- `Zephyr ZBus Documentation <https://docs.zephyrproject.org/latest/services/zbus/index.html>`_
- `native_sim Board Documentation <https://docs.zephyrproject.org/latest/boards/native/native_sim/doc/index.html>`_
- `Zephyr Testing with Twister <https://docs.zephyrproject.org/latest/develop/test/twister.html>`_
- `Zephyr Tracing Documentation <https://docs.zephyrproject.org/latest/services/tracing/index.html>`_
