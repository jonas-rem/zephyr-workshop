Color wheel
###########

Animate a circular HSV color space projection that spins around the centre of an
RGB LED matrix, using the Display driver API.

**Link to Source:** `samples/hweval/colorwheel <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/hweval/colorwheel>`_

Overview
********

This sample paints a color wheel onto an RGB LED matrix: the hue of each pixel
follows its polar angle around the centre, so a full rainbow wraps once around
the middle, while saturation rises with the radius. A bright white core in the
centre blooms into fully saturated color towards the corners, and the whole
wheel slowly spins. It drives the panel through Zephyr's Display driver API
rather than the LED strip API of Zephyr's LED subsystem API, so the same
binary runs on panels of different geometry, wiring order and pixel format
without any board specific code.

The panel is taken from the ``chosen { zephyr,display = ...; };`` node, which
on all supported boards resolves to a ``led-strip-matrix`` device on top of a
WS2812 compatible LED strip. That driver maps a rectangular frame buffer onto
the serpentine or circulative pixel order of the physical strip.

Color model
===========

A color wheel is much easier to express in `HSL and HSV`_ than in RGB: hue is
the angle, saturation is the radius, and value sets the brightness of the whole
panel. The sample computes every pixel in HSV and converts it to RGB in
`src/hsv.c <./src/hsv.c>`_.

Each pixel is placed in a polar coordinate system centered on the panel, and the
HSV values computed as follows:

   .. code-block:: none

      hue(x, y)        = atan2(y - cy, x - cx) + phase
      saturation(x, y) = 255 * dist((x, y), center) / (corner_radius * scale)
      value(x, y)      = CONFIG_COLORWHEEL_MAX_BRIGHTNESS

… where ``(cx, cy)`` is the center of the panel and the radius is normalised
against a fraction of the corner distance, in percent, set by
``CONFIG_COLORWHEEL_SAT_SCALE``, at which the ramp reaches full saturation.

Requirements
************

An RGB LED matrix assigned to the ``zephyr,display`` chosen node, with a
pixel format of either ``PIXEL_FORMAT_RGB_888`` or ``PIXEL_FORMAT_ARGB_8888``.
Either one of the following development boards with an on-board matrix:

* `RP2350-Matrix`_ (Waveshare RP2350-Matrix), an 8×8 panel
* `RP2040-Matrix`_ (Waveshare RP2040-Matrix), a 5×5 panel

or a Raspberry Pi Pico compatible board carrying the following shield from
the Bridle public downstream project (**not possible in this workshop**):

* `Waveshare Pico RGB LED`_ (Waveshare Pico-RGB-LED), a 16×10 panel

The sample supports the following platforms (located in `tests.yaml <./tests.yaml>`_):

:RP2350-Matrix_:
   | ``waveshare_rp2350_matrix/rp2350a/m33``
   | ``waveshare_rp2350_matrix/rp2350a/hazard3``

:RP2040-Matrix_:
   | ``rp2040_matrix/rp2040``

Configuration options
*********************

The following sample-specific Kconfig options are used in this sample
(located in `Kconfig <./Kconfig>`_):

:``COLORWHEEL_HUE_STEP``:

   | **Hue rotation per frame [degrees]**
   | *range is:* 0 360
   | *default is:* 1
   |

   How far the color wheel rotates around the centre of the
   panel between two consecutive frames. Set to 0 to render a
   static wheel.

:``COLORWHEEL_FRAME_INTERVAL``:

   | **Delay between two frames [milliseconds]**
   | *range is:* 1 10000
   | *default is:* 20
   |

   Time to sleep between two consecutive frames. Together with
   ``COLORWHEEL_HUE_STEP`` this defines the angular speed of the
   wheel. The default of 40 ms gives 25 frames per second.

:``COLORWHEEL_SAT_SCALE``:

   | **Radius of full saturation [% of corner distance]**
   | *range is:* 1 100
   | *default is:* 50
   |

   Radius at which the saturation ramp reaches full saturation,
   as a percentage of the distance from the centre to a corner.
   Beyond this radius the color stays fully saturated all the
   way into the corners.

:``COLORWHEEL_MAX_BRIGHTNESS``:

   | **Brightness ceiling**
   | *range is:* 1 255
   | *default is:* 8
   |

   The brightness every pixel is driven at. The radial ramp is
   carried by saturation alone, so this option sets the brightness
   of the whole panel directly.

      [!IMPORTANT]

      Keep ``CONFIG_COLORWHEEL_MAX_BRIGHTNESS`` low. A WS2812B pixel
      draws up to 60 ㎃ at full white according to the `WS2812B datasheet`_,
      so **an 8×8 panel alone can draw more than 3.5 A**, far beyond what a
      typical board's USB supply can deliver.

Building and Running
********************

* On RP2350-Matrix_ board, on **ARM Cortex-M33**:

  .. code-block:: console

     host:~$ west build -b waveshare_rp2350_matrix/rp2350a/m33 samples/hweval/colorwheel -p
     host:~$ west flash

* On RP2350-Matrix_ board, on **Hazard3 RISC-V (RV32IMAC+)**:

  .. code-block:: console

     host:~$ west build -b waveshare_rp2350_matrix/rp2350a/hazard3 samples/hweval/colorwheel -p
     host:~$ west flash

* On RP2040-Matrix_ board, on **ARM Cortex-M33**:

  .. code-block:: console

     host:~$ west build -b rp2040_matrix/rp2040 -S cdc-acm-console samples/hweval/colorwheel -p
     host:~$ west flash -r uf2

Sample output
=============

.. rubric:: On RP2350-Matrix_

The following output is logged on the USB console, here for the 8×8
panel of the RP2350-Matrix_ board:

.. code-block:: console

   *** Booting Zephyr OS build v4.5.0 ***
   [00:00:00.001,000] <inf> colorwheel: Color wheel on a 8x8 RGB matrix, 4 bytes per pixel, 20 ms per frame

.. rubric:: On RP2040-Matrix_

The following output is logged on the USB console, here for the 5×5
panel of the RP2040-Matrix_ board:

.. code-block:: console

   *** Booting Zephyr OS build v4.5.0 ***
   [00:00:00.002,000] <inf> usbd_init: bNumInterfaces 2 wTotalLength 75
   [00:00:00.003,000] <inf> colorwheel: Color wheel on a 5x5 RGB matrix, 3 bytes per pixel, 20 ms per frame
   [00:00:00.402,000] <inf> usbd_core: Actual device speed 1
   [00:00:00.487,000] <inf> usbd_core: Actual device speed 1

Troubleshooting
===============

The panel stays dark
   Check that the board assigns a matrix to the ``zephyr,display`` chosen
   node. The build fails when no such node exists, but a board that chooses
   a different kind of display reports an unsupported pixel format at run
   time instead.

The panel flickers or the board resets
   The LED strip is almost certainly browning out the supply. Lower
   ``CONFIG_COLORWHEEL_MAX_BRIGHTNESS``, or feed the panel from a supply
   that can carry the current.

The centre of the wheel is not white
   The centre should be a bright white core on any panel. If it is not, the
   matrix is most likely cropped by a ``width`` or ``height`` smaller than
   the physical panel, which moves the geometric centre off the true
   middle. Check the ``width`` and ``height`` properties of the matrix node
   against the panel.

Twister test suites
*******************

To run all test with twister, use the following command:

.. code-block:: console

   host:~$ west twister -T samples/hweval/colorwheel

.. rubric:: Sample Output

.. code-block:: console

   INFO - Total complete:    3/   3  100%  built (not run):    3, filtered:   51, failed:    0, error:    0
   INFO - 1 test scenarios (54 configurations) selected, 51 configurations filtered (51 by static filter, 0 at runtime).
   INFO - 0 of 3 executed test configurations passed (0.00%), 3 built (not run), 0 failed, 0 errored, with no warnings in 36.59 seconds.
   INFO - 0 of 0 executed test cases passed (0.00%) on 0 out of total 1721 platforms (0.00%).
   INFO - 3 selected test cases not executed: 3 not run (built only).
   INFO - 0 test configurations executed on platforms, 3 test configurations were only built.

Dependencies
************

This sample uses the following Zephyr libraries:

* `Display`_:

  * ``include/zephyr/drivers/display.h``

* `Light-Emitting Diode (LED)`_, by way of the LED strip matrix display driver
* `Kernel Services`_:

  * ``include/zephyr/kernel.h``

Known issues and limitations
****************************

The sample redraws and rewrites the whole panel on every frame, even when
``CONFIG_COLORWHEEL_HUE_STEP`` is ``0`` and nothing has changed. This keeps
the code simple at the cost of some unnecessary traffic to the LED strip.

References
**********

.. target-notes::

.. _HSL and HSV: https://en.wikipedia.org/wiki/HSL_and_HSV
.. _WS2812B datasheet: https://www.world-semi.com/ws2812-family/

.. _RP2350-Matrix: /boards/waveshare/rp2350_matrix
.. _RP2040-Matrix: https://github.com/zephyrproject-rtos/zephyr/tree/main/boards/waveshare/rp2040_matrix

.. _Waveshare Pico RGB LED: https://github.com/tiacsys/bridle/tree/main/boards/shields/rpi_pico_led

.. _Display: https://docs.zephyrproject.org/latest/hardware/peripherals/display/index.html#display-api
.. _Light-Emitting Diode (LED): https://docs.zephyrproject.org/latest/hardware/peripherals/led.html#led-api
.. _Kernel Services: https://docs.zephyrproject.org/latest/kernel/services/index.html#kernel-api
