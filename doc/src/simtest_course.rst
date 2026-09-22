Course
######

This self-study walks through the workshop application ``samples/simulation``
and the tools that test it without hardware. There is no task to solve. Explore
the code, run it, and form an opinion for the discussion afterwards. All
commands are also in :doc:`simtest_cheatsheet`. Run them from
``zephyr-workshop``.

Keep these questions in mind:

#. What share of firmware validation can live on a host PC, and what strictly
   requires hardware?
#. How must firmware be structured so that components can be tested in
   isolation?
#. Where does simulation fall short in real-world production?


Execution Targets
*****************

``native_sim`` compiles the application into a fast, reproducible Linux
executable but models no real CPU or memory map, QEMU emulates the target CPU
at the cost of speed and determinism, and an instruction set simulator adds
cycle accuracy but runs far slower than real time.

.. only:: html

   .. figure:: /_static/images/simulation_options_comparison.svg
      :align: center
      :width: 100%
      :alt: Comparison of simulation and hardware testing options by speed,
            hardware accuracy, and debugging visibility
      :target: https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options

      Comparison of Zephyr execution targets. Source: `Zephyr POSIX architecture
      documentation <https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options>`_.

.. only:: latex

   .. figure:: /_static/images/simulation_options_comparison.pdf
      :align: center
      :width: 100%
      :alt: Comparison of simulation and hardware testing options by speed,
            hardware accuracy, and debugging visibility
      :target: https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options

      Comparison of Zephyr execution targets. Source: `Zephyr POSIX architecture
      documentation <https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options>`_.


native_sim
**********

``native_sim`` builds Zephyr and the application into a single Linux
executable, ``build/zephyr/zephyr.exe``. Useful features:

Deterministic
   Every run behaves the same
Time control
   Run faster than real time (``-no-rt``), stop after a simulated time
   (``-stop_at``), set the RTC start value (``-rtc-offset``), e.g. to test
   hours of timer behaviour in seconds
Debugging
   ``gdb``
Sanitizers
   ``CONFIG_ASAN``, ``CONFIG_UBSAN``
Coverage
   ``CONFIG_COVERAGE``
Networking
   Host sockets (offloaded) or full Zephyr stack via TAP, e.g. to test MQTT or
   LwM2M connections
Storage
   Flash and EEPROM backed by a host file, e.g. to test settings, NVS or
   firmware updates across reboots
CAN
   Loopback controller or Linux SocketCAN, e.g. to exchange frames with host
   tools like ``candump``
Bluetooth
   Host Bluetooth controller via HCI user channel, or `BabbleSim
   <https://docs.zephyrproject.org/latest/develop/test/bsim.html>`_ to run a
   central and several peripherals entirely in simulation
Display and input
   SDL window for displays and touch, Linux evdev for input devices, e.g. to
   develop LVGL user interfaces
Audio
   Microphone (DMIC) and I2S backed by host files, e.g. to feed recorded
   samples into audio processing
Peripherals
   Emulators and host resources, e.g. UART as PTY
nRF hardware models
   Register-level nRF peripheral models, so the real nRF drivers and Bluetooth
   controller run unmodified. Note: these are separate ``nrf*_bsim`` boards
   that run on the same native simulator, not ``native_sim`` itself.

See the `native_sim documentation
<https://docs.zephyrproject.org/latest/boards/native/native_sim/doc/index.html>`_
for details.


1. The Modular Application
**************************

The application consists of three modules that only communicate through ZBus
channels. No module calls another module directly.

::

   zephyr-workshop/
   ├── boards/native_sim.overlay     # sensor, button and LEDs for native_sim
   ├── emulators/sensor/ti_hdc_emul/ # sensor emulator
   └── samples/simulation/
       ├── prj.conf
       ├── tests.yaml                # boot and end-to-end tests
       ├── pytest/                   # end-to-end test script
       └── src/
           ├── main.c
           ├── common/               # ZBus channels and message types
           └── modules/
               ├── button/tests/
               ├── led/tests/
               └── sensor/tests/

- ``src/modules/button``: reads a GPIO and publishes on ``button_event_ch``.
- ``src/modules/sensor``: reads the temperature sensor on a button event and
  publishes the result on ``sensor_event_ch``.
- ``src/modules/led``: switches an LED on for a button event and off for a
  sensor result.

::

   button_event_ch                            sensor_event_ch
              │                                      │
              │             ┌──────────┐             │
              │◀─── event ──│  Button  │             │
              │             └──────────┘             │
              │             ┌──────────┐             │
              │─── event ──▶│  Sensor  │─── event ──▶│
              │             └──────────┘             │
              │             ┌──────────┐             │
              │─── event ──▶│   LED    │◀── event ───│
              │             └──────────┘             │

The channels and message types live in ``src/common``. Each module is a Kconfig
option (``CONFIG_APP_SIM_BUTTON``, ...), see ``prj.conf`` and
``CMakeLists.txt``. ``src/main.c`` only prints a boot message.

Consider: which module could you test alone, and what would stand in for its
neighbours?


2. Build and Run
****************

.. code-block:: console

   $ west build -b native_sim samples/simulation -p
   $ west build -t run
   uart connected to pseudotty: /dev/pts/4

The shell runs on a pseudo terminal. Attach from a second terminal with the
number from the boot output, quit with ``Ctrl+a Ctrl+x``:

.. code-block:: console

   $ picocom /dev/pts/4

Press the emulated button, then read the result:

.. code-block:: console

   uart:~$ button emulate
   uart:~$ sensor latest
   uart:~$ sensor_emul set temp 30
   uart:~$ button emulate
   uart:~$ sensor latest

Watch the log lines in the first terminal: button, sensor and LED react to each
other through ZBus.


3. Sensor Emulation
*******************

An emulator replaces a chip at register level: it answers the bus transfers of
the real driver as the hardware would. Driver, application and tests stay
unchanged, and the test can set the values the chip reports. It can also
inject faults such as bus errors or implausible values to test that the
application handles a faulty sensor.

The sensor module uses the real ``ti_hdc`` driver. Only the chip behind the I2C
bus is emulated. Follow the chain:

#. ``boards/native_sim.overlay`` places a ``ti,hdc`` node on ``i2c0``. On
   ``native_sim``, ``i2c0`` is a ``zephyr,i2c-emul-controller``.
#. ``zephyr/drivers/sensor/ti/ti_hdc/ti_hdc.c`` is the unmodified driver. It
   issues I2C transfers as on hardware.
#. ``emulators/sensor/ti_hdc_emul/emul_ti_hdc.c`` answers these transfers with
   register values. ``ti_hdc_emul_transfer()`` is the I2C side,
   ``ti_hdc_emul_set_channel()`` the backend API that sets a value in physical
   units.
#. ``emulators/sensor/ti_hdc_emul/ti_hdc_emul_shell.c`` exposes the backend API
   as the ``sensor_emul`` shell command.

Build the sensor module alone, without the button and LED modules:

.. code-block:: console

   $ west build -b native_sim samples/simulation -p -- -DCONFIG_APP_SIM_BUTTON=n -DCONFIG_APP_SIM_LED=n
   $ west build -t run

As in step 2, attach ``picocom`` in a second terminal to the pseudo terminal
from the boot output. Quit it with ``Ctrl+a Ctrl+x``.

Set the temperature in the emulator and read it back through the real driver:

.. code-block:: console

   uart:~$ sensor_emul set temp 30
   Temperature set to 30
   uart:~$ sensor read
   uart:~$ sensor latest
   Sensor state: SUCCESS
   Temperature: 29.999 C
   Humidity: 55.703 %

Zephyr ships emulators for many device classes, typically:

Buses
   I2C and SPI controllers to attach emulated chips
Sensors
   Accelerometers, magnetometers, pressure and temperature sensors
Power
   Fuel gauges, chargers, USB BC1.2 charge detection
GPIO, ADC, DAC
   Set inputs and read outputs from the test
Memory
   Flash, EEPROM, battery-backed RAM, OTP
Others
   UART, DMA, RTC, GNSS, video

See `emulators
<https://docs.zephyrproject.org/latest/hardware/emulator/index.html>`_ and `bus
emulators
<https://docs.zephyrproject.org/latest/hardware/emulator/bus_emulators.html>`_
for the full list.


4. Component Test with Ztest
****************************

::

   samples/simulation/src/modules/sensor/
   ├── sensor.c
   └── tests/
       ├── CMakeLists.txt    # builds sensor.c and src/common only
       ├── prj.conf
       ├── tests.yaml        # Twister test definition
       └── src/test_sensor.c

``src/modules/sensor/tests`` builds the sensor module alone, without the button
and LED modules. The test in ``src/test_sensor.c`` replaces the neighbours:

- It publishes a ``button_event`` on ZBus instead of pressing a button.
- It sets temperature and humidity through the emulator backend API.
- A ZBus listener catches the ``sensor_event`` and the test asserts its values.

.. literalinclude:: ../samples/simulation/src/modules/sensor/tests/src/test_sensor.c
   :language: c
   :start-at: ZTEST(sensor_test
   :end-before: ZTEST_SUITE

.. code-block:: console

   $ west build -b native_sim samples/simulation/src/modules/sensor/tests -p
   $ west build -t run

Change an expected value in the test and run ``west build -t run`` again to see
a failure. The button and LED tests follow the same pattern with the GPIO
emulator.


5. Twister and End-to-End Tests
*******************************

Twister finds all ``tests.yaml`` files, builds and runs each test and collects
the results in ``twister-out``.

.. code-block:: console

   $ west twister -T samples/simulation --integration

``samples/simulation/tests.yaml`` defines an end-to-end test. The pytest script
``pytest/test_sensor_shell.py`` drives the application through the shell, the
same way you did in step 2. The identical script runs on ``native_sim`` and,
with ``app_simulation.e2e.hil``, on a real ``reel_board``.

.. literalinclude:: ../samples/simulation/pytest/test_sensor_shell.py
   :language: python
   :pyobject: test_sensor_read_updates_latest_sample_and_led

Replay the test by hand in your open console and compare the output with the
asserts:

.. code-block:: console

   uart:~$ button emulate
   uart:~$ sensor latest

Run only this test with Twister:

.. code-block:: console

   $ west twister -T samples/simulation -s app_simulation.e2e.native_sim --integration
   ...
   INFO    - 1 of 1 executed test configurations passed (100.00%), 0 built (not run), 0 failed, 0 errored, with no warnings in 22.96 seconds.

The test log shows the shell session the script drove:

.. code-block:: console

   $ cat twister-out/native_sim_native/host_gnu/zephyr-workshop/samples/simulation/app_simulation.e2e.native_sim/twister_harness.log

   samples/simulation/pytest/test_sensor_shell.py::test_sensor_read_updates_latest_sample_and_led
   DEBUG: #: uart:~$ button emulate
   DEBUG: #: Emulated button press started
   DEBUG: #: <dbg> app_simulation_button.publish_event: Button pressed, published event 1
   DEBUG: #: <dbg> app_simulation_sensor.perform_read: Reading sensor
   DEBUG: #: <dbg> app_simulation_led.set_output: LED ON
   DEBUG: #: <dbg> app_simulation_sensor.perform_read: Read 1: 4059 mC, 40344 m%
   DEBUG: #: <dbg> app_simulation_led.set_output: LED OFF
   DEBUG: #: sensor latest
   DEBUG: #: Sensor state: SUCCESS
   DEBUG: #: Temperature: 4.059 C
   DEBUG: #: Humidity: 40.344 %
   PASSED


References
**********

- `Zephyr testing overview <https://docs.zephyrproject.org/latest/develop/test/index.html>`_
- `Ztest framework <https://docs.zephyrproject.org/latest/develop/test/ztest.html>`_
- `Twister test runner <https://docs.zephyrproject.org/latest/develop/twister/index.html>`_
- `Pytest harness <https://docs.zephyrproject.org/latest/develop/test/pytest.html>`_
- `native_sim <https://docs.zephyrproject.org/latest/boards/native/native_sim/doc/index.html>`_
- `Emulators <https://docs.zephyrproject.org/latest/hardware/emulator/index.html>`_
- `ZBus <https://docs.zephyrproject.org/latest/services/zbus/index.html>`_
- `BabbleSim <https://docs.zephyrproject.org/latest/develop/test/bsim.html>`_
- :doc:`testing`
