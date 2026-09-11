My Shell for Hardware Evaluation
################################

**Link to Source:** `samples/hweval/myshell <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/hweval/myshell>`_

Overview
********

This sample demonstrates the use of Zephyr's shell subsystem, providing a
command-line interface for interacting with the system. The main focus here
is on providing a wide range of existing shell commands as an interface to
various Zephyr APIs or subsystems in order to simplify putting hardware
components into operation.

This sample is useful for:

- Device listing and kernel internal information.
- Raw access to basic functions like GPIO, PWM, or ADC.
- Working with more complex functions like LEDs or sensors.
- Ready for operation with yet more complex subsystems (Networking).

Building
********

This sample uses ``qemu_x86`` because the shell subsystem requires a UART
backend for interactive use, which QEMU provides.

This application can be built and executed on QEMU x86 as follows:

.. code-block:: console

   host:~$ west build -b qemu_x86 samples/hweval/myshell -p
   host:~$ west build -t run

To exit QEMU, press :kbd:`CTRL+a` followed by :kbd:`x`.

To build for a real board (e.g., reel_board@2), use:

.. code-block:: console

   host:~$ west build -b reel_board@2 samples/hweval/myshell -p
   host:~$ west flash

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build v4.4.0 ***
   Hardware Evaluation! I'm THE My Shell from qemu_x86
   uart:~$ <Tab>
     adc        clear      dac        date       device     devmem     eeprom
     flash      gpio       help       history    hwinfo     i2c        kernel
     led        log        pwm        regulator  rem        resize     retval
     rtc        sensor     shell      stats      timer

List devices (that were enabled on build time) and their status:

.. code-block:: console

   uart:~$ device list
   devices:
   - ioapic@fec00000 (READY)
     DT node labels: intc
   - loapic@fee00000 (READY)
     DT node labels: intc_loapic
   - uart@2f8 (READY)
     DT node labels: uart1
   - uart@3f8 (READY)
     DT node labels: uart0
   - sim_flash (READY)
     DT node labels: sim_flash
   - eeprom0 (READY)
     DT node labels: eeprom0
   - eeprom1 (READY)
     DT node labels: eeprom1

Exit QEMU by pressing :kbd:`CTRL+a` followed by :kbd:`x`.

Testing
*******

To run the test with Twister on emulated or dummy hardware,
use the following command (integration test):

.. code-block:: console

   west twister -vv -G -X fixture_console -T samples/hweval/myshell

.. rubric:: Sample Output

.. code-block:: console

   INFO - 1/7 qemu_x86/atom           sample.myshell.build   NOT RUN (build <zephyr/gnu>)
   INFO - 2/7 qemu_x86/atom           sample.myshell.boot    PASSED (qemu 1.718s <zephyr/gnu>)
   INFO - 3/7 reel_board@1/nrf52840   sample.myshell.boot    NOT RUN (build <zephyr/gnu>)
   INFO - 4/7 reel_board@1/nrf52840   sample.myshell.build   NOT RUN (build <zephyr/gnu>)
   INFO - 5/7 qemu_x86/atom           sample.myshell.cmd     PASSED (qemu 2.481s <zephyr/gnu>)
   INFO -                                sample.myshell.cmd.test_myshell_print_help             PASSED
   INFO -                                sample.myshell.cmd.test_myshell_print_kernel_version   PASSED
   INFO - 6/7 waveshare_rp2350_matrix/rp2350a/hazard3 sample.myboard.hil   NOT RUN (build <zephyr/gnu>)
   INFO - 7/7 waveshare_rp2350_matrix/rp2350a/m33 sample.myboard.hil       NOT RUN (build <zephyr/gnu>)

HiL test on Waveshare RP2350-Matrix
===================================

This board presents a challenging but solvable problem. It does not have
access to a JTAG/SWD interface, nor does it include a debug / programming
adapter on the board itself. Only the UF2-compatible on-chip ROM bootloader
can be used.

The only reliable way to activate this UF2 bootloader is by holding down
the **BOOT** button while pressing the **RESET** button. This purely manual
process cannot be easily automated by Twister.

As a result, Twister can only build and run a single, specific test scenario
on a single platform. Before each invocation of Twister, the UF2 bootloader
must be activated manually. Only then can ``west twister …`` be executed.

To run a single test scenario with Twister on real hardware (HiL),
use the following command:

.. code-block:: console

   west twister -v -G -X fixture_product -T samples/hweval/myshell \
                --scenario sample.myboard.hil \
                --platform waveshare_rp2350_matrix/rp2350a/m33 \
                --flash-before \
                --west-runner uf2 \
                --device-testing \
                --device-flash-timeout 60 \
                --device-serial /dev/ttyACM0 \
                --device-serial-baud 115200

.. rubric:: Sample Output

.. code-block:: console

   Device testing on:

   | Platform                            | ID   | Serial device   |
   |-------------------------------------|------|-----------------|
   | waveshare_rp2350_matrix/rp2350a/m33 |      | /dev/ttyACM0    |

   INFO - 1/1 waveshare_rp2350_matrix/rp2350a/m33   sample.myboard.hil   PASSED (device: None, 12.869s <zephyr/gnu>)
