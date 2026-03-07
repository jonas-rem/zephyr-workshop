Extending the App
#################

This exercise extends the existing application with sensor functionality. Three
new components are added, each developed and tested independently by a separate
group.

:ref:`temp_alert <group2_temp_alert>` (Beginner)
   Listen to sensor readings, publish alert on threshold. Pure logic, no
   hardware, no threads.

:ref:`tempsense <group1_tempsense>` (Intermediate)
   Read sensor via Zephyr Sensor API, publish periodically, react to system
   state. Uses ``k_work_delayable`` and sensor driver.

:ref:`sensor_log <group3_sensor_log>` (Advanced)
   Subscribe to two channels, store entries in ring buffer, expose via shell
   commands. Ring buffer and shell registration are provided in the stub.

The Scenario: Cold-Chain Monitoring
***********************************

The application becomes a cold-chain monitoring device for a refrigerated
transport truck. These devices are common in the food and pharmaceutical
industry. They track the temperature inside the cargo area during transport and
raise an alarm if it gets too warm. A tamper-proof log of all readings serves
as proof that the cold chain was never broken — a legal requirement for food
safety.

The existing application already provides the foundation:

- **Button**: Starts and stops a transport trip (toggles SLEEP/ACTIVE)
- **LED**: Shows system state on ``led0`` (blinks in ACTIVE, off in SLEEP),
  flashes ``led1`` on each sensor reading as activity indicator
- **sys_ctrl**: Manages the system state (SLEEP or ACTIVE)

What is missing are the three components that turn this into a monitoring
device.

Architecture
************

Two ZBus channels connect all components:

- ``event_ch`` carries events: button presses, sensor readings, temperature
  alerts. The message is a ``struct event_msg`` with an event type and an
  optional payload (e.g. sensor data).
- ``sys_ctl_ch`` carries the current system state (``enum sys_states``:
  ``SYS_SLEEP`` or ``SYS_ACTIVE``). Published by sys_ctrl whenever the state
  changes.

.. code-block:: text
   :caption: ZBus channel connections between all components

   event_ch                                     sys_ctl_ch
      │               ┌────────────┐                  │
      │◀─── PRESSED ──│  Button    │                  │
      │               └────────────┘                  │
      │               ┌────────────┐                  │
      │──── PRESSED ─▶│  sys_ctrl  │── ACTIVE/SLEEP ─▶│
      │               └────────────┘                  │
      │               ┌────────────┐                  │
      │─ TEMP/ALERT ─▶│    LED     │◀─ ACTIVE/SLEEP ──│
      │               └────────────┘                  │

      │   ----------- New Components ------------     │
      │               ┌────────────┐                  │
      │◀──── TEMP ────│ tempsense  │◀─ ACTIVE/SLEEP ──│
      │               └────────────┘                  │
      │               ┌────────────┐                  │
      │◀─── ALERT ────│ temp_alert │                  │
      │──── TEMP ────▶│            │                  │
      │               └────────────┘                  │
      │               ┌────────────┐                  │
      │─ TEMP/ALERT ─▶│ sensor_log │◀─ ACTIVE/SLEEP ──│
                      └────────────┘

Hardware
========

Two LEDs are available via devicetree aliases:

- ``led0`` — system state indicator (blinks in ACTIVE, off in SLEEP)
- ``led1`` — sensor activity indicator (50 ms flash on each measurement)

Both are driven by the LED component and emulated as GPIOs on ``native_sim``.
Most hardware boards provide at least two LEDs.

Message Definitions
===================

The event channel carries a generic message struct. The event type determines
which fields in the payload are valid:

.. code-block:: c

   enum sys_events {
       SYS_BUTTON_PRESSED,
       SYS_SENSOR_READING,
       SYS_TEMP_ALERT,
   };

   struct sensor_data {
       int32_t temp;       /* Temperature in 0.01 °C */
   };

   struct event_msg {
       enum sys_events event;
       union {
           struct sensor_data sensor;
       };
   };

The state channel carries a simple enum:

.. code-block:: c

   enum sys_states {
       SYS_SLEEP,
       SYS_ACTIVE,
   };

New Components
**************

.. _group1_tempsense:

Group 1: tempsense — The Sensor Driver
=======================================

Reads the temperature from the sensor and publishes readings to ``event_ch``
every 500 ms while the system is in ACTIVE.

During a trip (ACTIVE), the device measures continuously. When idle (SLEEP),
it pauses sampling to save battery — these devices run on battery for days or
weeks.

**What to implement:**

- Use the Zephyr Sensor API (``sensor_sample_fetch``, ``sensor_channel_get``)
  to read the emulated TI HDC sensor
- Use ``k_work_delayable`` for periodic 500 ms measurement
- Subscribe to ``sys_ctl_ch``: start measuring in ACTIVE, stop in SLEEP
- Publish a ``SYS_SENSOR_READING`` event with the temperature to ``event_ch``
  after each measurement

.. _group2_temp_alert:

Group 2: temp_alert — The Alarm
================================

Watches sensor readings on ``event_ch``. When the temperature exceeds 5 °C for
two consecutive readings, it publishes a ``SYS_TEMP_ALERT`` event. After that,
one alert is published for every further reading that still exceeds the
threshold.

In a real device, this would activate a buzzer or send a notification to a
fleet management system. In our application, the alert is recorded by
sensor_log.

**What to implement:**

- Subscribe to ``event_ch`` (ZBus listener), filter for ``SYS_SENSOR_READING``
- Track consecutive readings above the threshold (configurable via
  ``CONFIG_TEMP_ALERT_THRESHOLD``, default 5 °C)
- After 2 consecutive readings above the threshold, publish ``SYS_TEMP_ALERT``
  to ``event_ch``
- Continue publishing one ``SYS_TEMP_ALERT`` per reading as long as
  temperature stays above the threshold
- Reset the counter when temperature drops back below the threshold

.. _group3_sensor_log:

Group 3: sensor_log — The Compliance Record
=============================================

Records all events and state changes with timestamps. Provides shell commands
to inspect the log.

This is the component that makes the device useful for regulatory compliance.
After a transport trip, an inspector connects to the device and retrieves the
temperature log to verify the cold chain was maintained throughout.

**What to implement:**

- Subscribe to ``event_ch`` (record sensor readings and alerts)
- Subscribe to ``sys_ctl_ch`` (record state changes)
- Store entries in a ring buffer with uptime timestamp (``k_uptime_get()``)
- Provide shell commands:

  - ``sensor_log last`` — print the most recent entry
  - ``sensor_log history`` — print all buffered entries with timestamps

- Buffer size configurable via Kconfig (``CONFIG_SENSOR_LOG_BUFFER_SIZE``)

**Example shell output:**

.. code-block:: console

   uart:~$ sensor_log history
   #[00:00:01.000] STATE    ACTIVE
   [00:00:01.500] SENSOR   temp=3.20 °C
   [00:00:02.000] SENSOR   temp=3.25 °C
   [00:00:02.500] SENSOR   temp=5.80 °C
   [00:00:03.000] SENSOR   temp=6.10 °C
   [00:00:03.000] ALERT    temp=6.10 °C (threshold exceeded)
   [00:00:03.500] SENSOR   temp=6.30 °C
   [00:00:03.500] ALERT    temp=6.30 °C (threshold exceeded)
   [00:00:10.000] STATE    SLEEP

How a Trip Works
****************

When all components are active, a typical trip looks like this:

1. The device is idle (SLEEP). Both LEDs are off. No sensor readings.
2. The driver presses the button to start the trip. sys_ctrl transitions to
   ACTIVE and broadcasts the new state on ``sys_ctl_ch``.
3. The **LED** component receives ACTIVE: ``led0`` starts blinking.
4. **tempsense** receives ACTIVE, starts measuring every 500 ms, publishes
   ``SYS_SENSOR_READING`` events to ``event_ch``. The **LED** component
   flashes ``led1`` for 50 ms on each reading as a visual heartbeat.
5. **sensor_log** records every reading with a timestamp.
6. **temp_alert** watches the readings. If someone left the truck door open
   and temperature rises above 5 °C for two consecutive readings, it publishes
   ``SYS_TEMP_ALERT``. sensor_log records the alert.
7. The driver presses the button again. sys_ctrl transitions to SLEEP.
   tempsense stops measuring. Both LEDs go off.
8. An inspector runs ``sensor_log history`` via the shell to verify the cold
   chain. The log shows all readings, alerts, and state transitions with
   timestamps.

What Is Already Provided
*************************

The following is prepared and does not need to be changed:

- ``message_channel.h``: Channel declarations (``event_ch``, ``sys_ctl_ch``),
  ``struct event_msg``, ``struct sensor_data``, event and state enums
- ``message_channel.c``: ``ZBUS_CHAN_DEFINE`` for both channels
- ``native_sim.overlay``: Emulated TI HDC sensor on I2C, two LEDs
  (``led0``, ``led1``), button (``sw0``)
- ``native_sim.conf``: Emulation support enabled
- The existing components (button, sys_ctrl) use the renamed channels but
  their logic is unchanged
- The LED component is extended to also subscribe to ``event_ch`` and drive
  ``led1`` (sensor activity flash). The existing ``led0`` behavior is
  unchanged.

For each new component, a **stub** and a **failing test** are provided:

- The stub has the file structure (``CMakeLists.txt``, ``Kconfig``, source
  file) but the implementation is incomplete
- The test (in ``tests/``) is complete and will **fail** when run against the
  stub

Development Workflow
********************

Each group works independently on their component:

1. **Read** the component stub and the test to understand what is expected
2. **Run the test** — it will fail:

   .. code-block:: console

      host:~$ west twister -T app/src/components/tempsense/tests --integration -p native_sim

3. **Implement** the component to make the tests pass
4. **Run the test again** — it should pass now
5. **Enable the component** in ``prj.conf`` and build the full application
6. **Verify** together with the other groups that everything works end-to-end

This follows a test-driven development (TDD) approach: the tests define the
expected behavior, the implementation makes it real.

**Rule:** Each group may only edit files inside their assigned component
directory (e.g. ``app/src/components/tempsense/``) and add one line to
``app/prj.conf`` to enable their module (``CONFIG_TEMPSENSE_MODULE=y``).
No other files should be modified.

Integration
===========

Once all groups have their tests passing, the results are merged together:

1. Each group opens a pull request against the ``ew25_app_extension`` branch
2. CI runs all component tests automatically
3. If tests are green, the PR is merged

Since each group only touches their own component directory and one line in
``prj.conf``, merge conflicts should be minimal. After all three PRs are
merged, build and run the full application to see all components working
together.

Running Tests
=============

.. code-block:: console

   # Single component test (during development, faster)
   host:~$ west build -b native_sim app/src/components/tempsense/tests -p
   host:~$ west build -t run
   # If only .c/.h files changed, just re-run (incremental build):
   host:~$ west build -t run

   # Single component test via Twister
   host:~$ west twister -T app/src/components/tempsense/tests --integration -p native_sim

   # All component tests
   host:~$ west twister -T app/src/components/ --integration -p native_sim

   # Full application with all components
   host:~$ west build -b native_sim app -p
   host:~$ west build -t run
