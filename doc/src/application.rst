.. Copyright (c) 2026 Jonas Remmert
.. SPDX-License-Identifier: Apache-2.0

Application
===========

The example application demonstrates a modular Zephyr design with inter-module communication via Zbus.

Overview
--------

The application consists of three main modules:

- **Button Module**: Handles physical button presses via GPIO interrupts
- **LED Module**: Controls LED state based on system state
- **Main Application**: Manages system state machine and coordinates modules

System States
-------------

The application implements two system states:

- **Sleep**: LED is off, system waits for events
- **Standby**: LED blinks, system is active

Button presses toggle between these states via Zbus messages.

Running on native_sim
---------------------

The application can be built and tested on native_sim without hardware:

.. code-block:: bash

   # Build the application
   west build -b native_sim app -p

   # Run with shell on separate console
   ./build/zephyr/zephyr.exe -uart_1_attach_uart_cmd='ln -sf %s /tmp/zephyr_shell'

This creates two separate consoles:

- **Logs** (/dev/pts/X) - Application logs and boot messages
- **Shell** (/tmp/zephyr_shell) - Interactive shell for commands

Shell Commands
--------------

Two methods are available to trigger button events:

**1. GPIO Level (Hardware Simulation)**

Uses the GPIO emulator to simulate physical button press:

.. code-block:: bash

   uart:~$ mock_button

This triggers the actual GPIO interrupt handler with debouncing.

**2. Application Level (Zbus)**

Sends button event directly via Zbus:

.. code-block:: bash

   uart:~$ my_app button button_press

This bypasses the GPIO layer and sends the event directly to the application.

Architecture
------------

**Module Communication**

Modules communicate via Zbus channels:

- ``button_ch``: Button press events
- ``led_ch``: LED state changes

**Hardware Abstraction**

On native_sim, the application uses:

- **GPIO Emulator** (zephyr,gpio-emul) for button and LED
- **PTY UART** for shell on separate console
- **Devicetree Overlay** to define button alias and UART configuration

The overlay file (``app/boards/native_sim.overlay``) defines:

- ``sw0`` alias for the button
- ``uart1`` enabled for shell
- Shell UART mapped to ``zephyr,shell-uart``

Module Structure
----------------

.. code-block:: text

   app/src/modules/
   ├── button/          # Physical button handling
   ├── button_mock/     # GPIO emulator interface (native_sim only)
   └── led/             # LED control

The ``button_mock`` module provides the ``mock_button`` shell command and is only compiled for native_sim boards.
