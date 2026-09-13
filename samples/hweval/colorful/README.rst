Colorful
########

Draw an RGB color gradient on a display device.

**Link to Source:** `samples/hweval/colorful <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/hweval/colorful>`_

Overview
********

This sample uses Zephyr's Display driver API to render an RGB color gradient
across the entire display.

With most display drivers, the RGB color gradient runs across the entire
screen, from red through yellow to green, then from green through cyan to
blue, and finally from blue through magenta back to red. This cycle repeats
endlessly and changes the color of all pixels at once. It is not a static
representation of an RGB color gradient; the colors simply cycle through.

:RGB:

   Typical RGB output at 320x240: color gradient from red across
   dark yellow to green, across dark cyan to blue, across dark
   magenta back to red.

:Grayscale:

   On displays using a multi-bit luminance format (for example
   ``PIXEL_FORMAT_L_8``), the entire screen runs over different
   grey level between black and white (color luminance). Other
   multi-bit monochrome formats behave similarly (different greys).

:1 bpp:

   On displays with 1 bit per pixel, the greyscale animation of the entire
   screen will animate as flickering between fore- and background color:

   :``MONO01``: black on a white screen
   :``MONO10``: white on a black screen

:E-paper:

   By querying the display controller's capabilities, it is
   possible to determine if the display is an e-paper display
   by checking if the ``SCREEN_INFO_EPD`` bit is set in the
   ``play_capabilities.screen_info``. For such displays, the
   color of the entire screen will change at a much slower rate
   than on a typical LCD, to align with the typical refresh rate
   of e-ink technologies.

Configuration options
*********************

The following sample-specific Kconfig options are used in this sample
(located in `Kconfig <./Kconfig>`_):

:``SAMPLE_MAX_INTENSITY``:

   | **SAMPLE_MAX_INTENSITY**
   | *range is:* 0 255
   | *default is:* 15 *if* ``LED_STRIP_MATRIX``
   | *default is:* 255 *else*
   |

   Some displays, particularly those based on LED technology,
   require a reduced brightness range per color channel for
   safety reasons. This ensures that LED matrix displays do
   not draw excessive electrical current and that the luminous
   flux they generate does not pose a risk of physical injury
   (e.g., glared by direct eye exposure to LEDs).

:``SAMPLE_INIT_INTENSITY``:

   | **Initial intensity in all color channels**
   | *range is:* 0 0 *if* ``LED_STRIP_MATRIX``
   | *range is:* 255 255 *else*
   | *default is:* 0 *if* ``LED_STRIP_MATRIX``
   | *default is:* 255 *else*
   |

   Some displays, particularly those based on LED technology,
   do not require initialization with a white background. From
   an energy efficiency standpoint and to avoid other well known
   disruptive effects, the background of this type of displays
   should be initialized as black (i.e., with the LEDs turned off).

:``HEAP_MEM_POOL_ADD_SIZE_SAMPLE``:

   | **Sample maximum heap allocated buffer size**
   | *default is:* 4096
   |

   Maximum size of the buffer for rendering one line of 1024 pixels,
   each 4 bytes wide (ARGB-8888).

Building and Running
********************

As this is a generic sample it should work with any display supported by Zephyr.

Below is an example on how to build for a ``nrf52840dk`` board with a
``adafruit_2_8_tft_touch_v2`` shield.

.. code-block:: console

   host:~$ west build -b nrf52840dk/nrf52840 --shield adafruit_2_8_tft_touch_v2 samples/hweval/colorful -p
   host:~$ west flash

For testing purpose without the need of any hardware, the ``native_sim/native/64``
board is also supported and can be built as follows:

.. code-block:: console

   host:~$ west build -b native_sim/native/64 samples/hweval/colorful -p
   host:~$ west build -t run

.. rubric:: Sample Output

.. code-block:: console

   *** Booting Zephyr OS build v4.5.0 ***
   [00:00:00.000,000] <inf> sample: Colorful sample for sdl_dc
   [00:00:00.000,000] <inf> sample: sdl_dc: 320x240, pixel format: 8
   [00:00:00.000,000] <inf> sample: sdl_dc: allocated memory: 1280
   [00:00:00.000,000] <inf> sample: Colorful starts

Exit native_sim by pressing :kbd:`CTRL+C`.

In addition to using a native POSIX runtime environment, emulation in QEMU
with ``qemu_x86`` is also an option. However, this environment lacks specific
instructions for the actual output/display of the emulated framebuffer within
the Display API. The user must add these instructions manually:

.. code-block:: console

   host:~$ west build -b qemu_x86 samples/hweval/colorful -p
   host:~$ west build -t run

.. rubric:: Sample Output

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
   [00:00:00.010,000] <inf> sample: Colorful sample for ramfb
   [00:00:00.010,000] <inf> sample: ramfb: 1024x768, pixel format: 8
   [00:00:00.010,000] <inf> sample: ramfb: allocated memory: 4096
   [00:00:00.120,000] <inf> sample: Colorful starts

To exit QEMU, press :kbd:`CTRL+a` followed by :kbd:`x`.

Twister test suites
*******************

To run the test with twister on emulated or dummy hardware,
use the following command:

.. code-block:: console

   host:~$ west twister -v -X fixture_display -T samples/hweval/colorful \
                        --scenario sample.display.colorful.sdl \
                        --scenario sample.display.colorful.dummy

.. rubric:: Sample Output

.. code-block:: console

   INFO - 1/3 native_sim/native      sample.display.colorful.dummy   PASSED (native 0.232s <host/gnu>)
   INFO - 2/3 native_sim/native/64   sample.display.colorful.sdl     NOT RUN (build <host/gnu>)
   INFO - 3/3 qemu_x86/atom          sample.display.colorful.dummy   PASSED (qemu 1.790s <zephyr/gnu>)

To run all test with twister, use the following command:

.. code-block:: console

   host:~$ west twister -T samples/hweval/colorful

.. rubric:: Sample Output

.. code-block:: console

   INFO - Total complete:   17/  65  26%  built (not run):    0, filtered:   57, failed:    0, error:    0
   … … …
   INFO - Total complete:   37/  65  56%  built (not run):    4, filtered:   73, failed:    0, error:    0
   … … …
   INFO - Total complete:   58/  65  89%  built (not run):   10, filtered:   88, failed:    0, error:    0
   … … …
   INFO - Total complete:   65/  65  100%  built (not run):   15, filtered:   90, failed:    0, error:    0
   INFO - 4 test scenarios (105 configurations) selected, 90 configurations filtered (40 by static filter, 50 at runtime).
   INFO - 0 of 15 executed test configurations passed (0.00%), 15 built (not run), 0 failed, 0 errored, with no warnings in 166.46 seconds.
   INFO - 0 of 0 executed test cases passed (0.00%) on 0 out of total 1721 platforms (0.00%).
   INFO - 15 selected test cases not executed: 15 not run (built only).
   INFO - 0 test configurations executed on platforms, 15 test configurations were only built.
