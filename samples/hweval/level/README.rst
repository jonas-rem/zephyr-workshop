Level
#####

A digital spirit level on an RGB LED matrix, driven through the Display
driver API and fed from an accelerometer.

**Link to Source:** `samples/hweval/level <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/hweval/level>`_

Overview
********

This sample turns the board into a digital spirit level. The on-board
accelerometer measures which way is down, and a bubble drawn on the RGB LED
matrix runs towards whichever edge of the board is raised, exactly like the
bubble in a vial. Hold the board flat and the bubble returns to the middle of
the panel.

The panel is driven through Zephyr's Display driver API and the sensor through
Zephyr's Sensor driver API, so the same binary runs on panels of different
geometry, wiring order and pixel format without any board specific code. The
panel is taken from the ``chosen { zephyr,display = ...; };`` node, which on
the supported board resolves to a ``led-strip-matrix`` device on top of a
WS2812 compatible LED strip. The sensor is taken from the ``accel0`` alias;
only the accelerometer is used, since a level needs the drift-free reference
that gravity provides and a gyroscope does not.

Techniques worth knowing about:

* The acceleration vector is normalised against its own length, which turns the
  in-plane components into plain sines of the tilt angle, and is smoothed by a
  first order low pass filter before use.
* The bubble is rendered with sub-pixel accuracy: instead of snapping to the
  nearest pixel, its light is spread bilinearly over the up to four pixels it
  straddles.
* Since position runs out of resolution near the centre, the bubble also changes
  colour over a much tighter angle than it moves over, and latches to the near
  colour, with hysteresis, once the board is level.

Requirements
************

An RGB LED matrix assigned to the ``zephyr,display`` chosen node, with a
pixel format of either ``PIXEL_FORMAT_RGB_888`` or ``PIXEL_FORMAT_ARGB_8888``,
**and** a 3-axis accelerometer assigned to the ``accel0`` alias. Either one
of the following development boards with an on-board matrix:

* `RP2350-Matrix`_ (Waveshare RP2350-Matrix), an 8×8 panel

The sample supports the following platforms (located in `tests.yaml <./tests.yaml>`_):

:RP2350-Matrix_:
   | ``waveshare_rp2350_matrix/rp2350a/m33``
   | ``waveshare_rp2350_matrix/rp2350a/hazard3``

Configuration options
*********************

The following sample-specific Kconfig options are used in this sample
(located in `Kconfig <./Kconfig>`_):

:``LEVEL_COLOR_RANGE_TILT``:

   | **Tilt angle over which the bubble changes colour [tenths of a degree]**
   | *range is:* 1 600
   | *default is:* 10
   |

   At this angle and beyond, the bubble is fully the far colour; at
   dead level it is fully the near colour; in between it is mixed.
   Should be far tighter than ``LEVEL_FULL_SCALE_TILT``.

:``LEVEL_LOCK_TILT``:

   | **Tilt angle below which the board counts as level [tenths of a degree]**
   | *range is:* 0 600
   | *default is:* 3
   |

   Below this tilt the bubble latches fully to the near colour.
   Keep well inside ``LEVEL_COLOR_RANGE_TILT``. Set to 0 to disable
   the latch.

:``LEVEL_LOCK_HYSTERESIS``:

   | **Extra tilt needed to break the level latch [tenths of a degree]**
   | *range is:* 0 600
   | *default is:* 2
   |

   The latch engages at ``LEVEL_LOCK_TILT`` and releases at
   ``LEVEL_LOCK_TILT`` plus this, so the two form a band rather than
   a single edge that would flicker on noise.

:``LEVEL_FULL_SCALE_TILT``:

   | **Tilt angle at which the bubble reaches the panel edge [degrees]**
   | *range is:* 1 60
   | *default is:* 10
   |

   Tilt that moves the bubble from the centre of the panel all the
   way to the rim; sets the sensitivity of the instrument.

:``LEVEL_FILTER_TIME_CONSTANT``:

   | **Low pass filter time constant [milliseconds]**
   | *range is:* 0 5000
   | *default is:* 100
   |

   Time constant of the first order low pass filter on the
   acceleration reading. Set to 0 to disable filtering. Quoted as
   a time rather than a per sample weight, so the smoothing is
   independent of ``LEVEL_FRAME_INTERVAL``.

:``LEVEL_FRAME_INTERVAL``:

   | **Delay between two frames [milliseconds]**
   | *range is:* 1 10000
   | *default is:* 40
   |

   Also the rate at which the inertial sensor is polled.

:``LEVEL_SENSOR_LOG_INTERVAL``:

   | **Delay between two logged sensor readings [milliseconds]**
   | *range is:* 0 60000
   | *default is:* 1000
   |

   Throttles the log output only; sampling always runs at
   ``LEVEL_FRAME_INTERVAL``. Set to 0 to log every sample.

:``LEVEL_TEST_IMAGE_MS``:

   | **Show the corner test image at startup [milliseconds]**
   | *range is:* 0 10000
   | *default is:* 2000
   |

   Lights the four corner pixels in red, green, blue and white to
   show where the origin of the display physically sits. Set to 0
   to skip the test image.

:``LEVEL_AXIS_SWAP_XY``:

   | **Swap the sensor X and Y axes**
   | *default is:* y *if* ``BOARD_WAVESHARE_RP2350_MATRIX``
   | *default is:* n *else*
   |

   Feed the sensor Y axis into the panel X axis and vice versa.
   Applied before the inversions.

:``LEVEL_AXIS_INVERT_X``:

   | **Invert the panel X axis**
   | *default is:* y *if* ``BOARD_WAVESHARE_RP2350_MATRIX``
   | *default is:* n *else*
   |

   Negate the horizontal component, after any swap

:``LEVEL_AXIS_INVERT_Y``:

   | **Invert the panel Y axis**
   |

   Negate the vertical component, after any swap.

:``LEVEL_BRIGHTNESS``:

   | **Brightness of the bubble and the test image**
   | *range is:* 1 255
   | *default is:* 32
   |

   The channel level a fully lit primary colour is drawn at. This
   is also the number of brightness steps the sub-pixel
   positioning has to work with.

      [!IMPORTANT]

      Keep ``CONFIG_LEVEL_BRIGHTNESS`` low. A WS2812B pixel draws up to 60 ㎃
      at full white according to the `WS2812B datasheet`_, so **an 8×8 panel
      alone can draw more than 3.5 A**, far beyond what a typical board's
      USB supply can deliver.

Building and Running
********************

* On RP2350-Matrix_ board, on **ARM Cortex-M33**:

  .. code-block:: console

     host:~$ west build --board waveshare_rp2350_matrix/rp2350a/m33 samples/hweval/level -p
     host:~$ west flash

* On RP2350-Matrix_ board, on **Hazard3 RISC-V (RV32IMAC+)**:

  .. code-block:: console

     host:~$ west build --board waveshare_rp2350_matrix/rp2350a/hazard3 samples/hweval/level -p
     host:~$ west flash

Sample output
=============

.. rubric:: On RP2350-Matrix_

The following output is logged on the USB console, here for the 8×8
panel of the RP2350-Matrix_ board:

.. code-block:: console

   *** Booting Zephyr OS build v4.5.0 ***
   [00:00:00.017,000] <inf> level: Level on a 8x8 RGB matrix, 4 bytes per pixel, 40 ms per frame
   [00:00:00.017,000] <inf> level: Inertial sensor is qmi8658a@6b, full scale tilt is 10 deg
   [00:00:00.017,000] <inf> level: Axis map: swap-xy yes, invert-x yes, invert-y no
   [00:00:00.017,000] <inf> level: Filter time constant is 100 ms, giving a weight of 0.330 per sample
   [00:00:00.017,000] <inf> level: Colour ramps below 1.0 deg, latches level below 0.3 deg, releases at 0.5 deg
   [00:00:02.020,000] <inf> level: accel  -0.995  -0.153  -9.735 m/s^2, |a|  9.787
   [00:00:02.020,000] <inf> level: tilt  x +0.016 y -0.102, angle  5.91 deg
   [00:00:02.020,000] <inf> level: dot    3.81,  1.45, proximity 0.00
   [00:00:03.035,000] <inf> level: accel  -0.991  -0.153  -9.723 m/s^2, |a|  9.775
   [00:00:03.035,000] <inf> level: tilt  x +0.016 y -0.101, angle  5.89 deg
   [00:00:03.035,000] <inf> level: dot    3.82,  1.46, proximity 0.00

Axis mapping
============

Nothing in the devicetree says how the sensor is oriented relative to the
panel, and a bubble floats towards the *raised* edge, the opposite of
where the gravity vector points. Both are folded into the
``CONFIG_LEVEL_AXIS_*`` options, defaulted per board:

.. list-table::
   :header-rows: 1

   * - Board
     - ``SWAP_XY``
     - ``INVERT_X``
     - ``INVERT_Y``
   * - RP2350-Matrix_
     - yes
     - yes
     - no
   * - anything else
     - no
     - no
     - no

To bring up a new board:

#. Lay the board flat, LEDs up. Note where the corner test image puts its red
   pixel: that is the origin of the panel.
#. Build and flash with all three switches off. Raise the edge **closest
   to you** and watch the bubble:

   * It moves towards you: correct, leave the switches alone.
   * It moves away from you: set ``CONFIG_LEVEL_AXIS_INVERT_Y=y``.
   * It moves left or right instead: set ``CONFIG_LEVEL_AXIS_SWAP_XY=y``
     and repeat this step.

#. Raise the **left** edge. If the bubble moves right instead of left,
   set ``CONFIG_LEVEL_AXIS_INVERT_X=y``.
#. Record the result as a per board default in `Kconfig <./Kconfig>`_.

The console prints the mapping in use at startup. Zephyr's ``sensor-axis-align``
devicetree convention would be the proper home for the hardware half of this
mapping, but the ``qst,qmi8658a`` binding does not support it yet.

Troubleshooting
===============

The panel stays dark
   Check that the board assigns a matrix to the ``zephyr,display`` chosen
   node. The build fails when no such node exists, but a board that chooses
   a different kind of display reports an unsupported pixel format at run
   time instead.

The corner colours are wrong or the image is mirrored
   The panel is not wired the way the matrix node describes. Review the
   ``circulative``, ``serpentine`` and ``color-mapping`` properties.

The dot runs the wrong way
   The sensor is not oriented the way the panel is. Work through section
   *Axis mapping* (see above).

The dot never leaves the middle, or pins to the rim at the slightest tilt
   ``CONFIG_LEVEL_FULL_SCALE_TILT`` is too large or too small for the way
   you are holding the board. The console reports the measured angle in
   degrees, so compare that against the configured full scale.

The bubble will not sit still, or lags behind your hand
   Adjust ``CONFIG_LEVEL_FILTER_TIME_CONSTANT``: raise it for
   steadiness, lower it for responsiveness. Set it to ``0`` to see the
   raw, unfiltered reading.

The colour flickers while the board is nearly level
   Raise ``CONFIG_LEVEL_LOCK_HYSTERESIS``. Set it to ``0`` once to see
   the flicker it is there to prevent.

Twister test suites
*******************

To run all test with twister, use the following command:

.. code-block:: console

   host:~$ west twister -T samples/hweval/level

.. rubric:: Sample Output

.. code-block:: console

   INFO - Total complete:    2/   2  100%  built (not run):    2, filtered:   51, failed:    0, error:    0
   INFO - 1 test scenarios (53 configurations) selected, 51 configurations filtered (51 by static filter, 0 at runtime).
   INFO - 0 of 2 executed test configurations passed (0.00%), 2 built (not run), 0 failed, 0 errored, with no warnings in 33.93 seconds.
   INFO - 0 of 0 executed test cases passed (0.00%) on 0 out of total 1721 platforms (0.00%).
   INFO - 2 selected test cases not executed: 2 not run (built only).
   INFO - 0 test configurations executed on platforms, 2 test configurations were only built.

Dependencies
************

This sample uses the following Zephyr libraries:

* `Display`_:

  * ``include/zephyr/drivers/display.h``

* `Sensors`_:

  * ``include/zephyr/drivers/sensor.h``

* `Light-Emitting Diode (LED)`_, by way of the LED strip matrix display driver
* `Kernel Services`_:

  * ``include/zephyr/kernel.h``

References
**********

.. target-notes::

.. _WS2812B datasheet: https://www.world-semi.com/ws2812-family/

.. _RP2350-Matrix: /boards/waveshare/rp2350_matrix

.. _Display: https://docs.zephyrproject.org/latest/hardware/peripherals/display/index.html#display-api
.. _Sensors: https://docs.zephyrproject.org/latest/hardware/peripherals/sensor/index.html#sensor
.. _Light-Emitting Diode (LED): https://docs.zephyrproject.org/latest/hardware/peripherals/led.html#led-api
.. _Kernel Services: https://docs.zephyrproject.org/latest/kernel/services/index.html#kernel-api
