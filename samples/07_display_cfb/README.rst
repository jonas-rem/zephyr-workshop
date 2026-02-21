07 Display
##########

**Link to Source:** `samples/07_display_cfb <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/07_display_cfb>`_

Overview
********

This sample demonstrates the use of Zephyr's Character Frame Buffer (CFB)
subsystem to display text on a monochrome display.

This sample is useful for:

- Understanding the Character Frame Buffer subsystem
- Learning to write text to displays
- Working with different font sizes
- Display control and initialization

Key Features:

- Multiple font support
- Text positioning on display
- Kerning control for character spacing
- Display parameter querying

Requirements
************

- A board with CFB-supporting display (e.g., reel_board@2)

Supported Boards:

- reel_board@2 (has onboard passive display)

Building
********

.. code-block:: console

   host:~$ west build -b reel_board@2 samples/07_display_cfb -p
   host:~$ west flash

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
   Initialized display@0
   idx: 0, font width 10, font height 16
   idx: 1, font width 15, font height 24
   x_res 128, y_res 296, ppt 2, rows 32, cols 64

Display Output
**************

The display will show:

- "Zephyr Class"
- "EW 2025"
- "<name>" (placeholder for your name)

Customization
*************

Edit ``samples/07_display_cfb/src/main.c`` to change the displayed text:

.. code-block:: c

   cfb_print(dev, "Your Text", x_position, y_position);

Resources
*********

- `Zephyr Display API Documentation <https://docs.zephyrproject.org/latest/hardware/peripherals/display/index.html>`_
- `Character Frame Buffer API <https://docs.zephyrproject.org/latest/hardware/peripherals/display/cfb.html>`_
