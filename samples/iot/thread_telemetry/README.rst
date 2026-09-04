.. _iot_thread_telemetry:

Thread telemetry node
#####################

Overview
********

A minimal IoT node: it joins a pre-shared OpenThread mesh, reads the on-board
SHT4x temperature and humidity sensor every 15 seconds and POSTs the reading
as JSON over CoAP. Every participant gives their board a name, so their data
is easy to spot on the shared dashboard.

The node is deliberately the *only* thing in this sample. Collecting the data
and displaying it happens outside the board, in :ref:`iot_collector`:

.. code-block:: none

   ┌──────────┐   CoAP/UDP    ┌────────────────┐        ┌──────────┐   ┌───────────┐
   │  node    │──────────────▶│ Thread border  │───────▶│ CoAP→MQTT│──▶│  MQTT     │
   │ (SHT4x)  │   over the    │    router      │        │  bridge  │   │  broker   │
   └──────────┘  802.15.4     └────────────────┘        └──────────┘   └───────────┘
                    mesh                                                     │
   ┌──────────┐        ▲                                                     ▼
   │  node    │────────┘                                                ┌───────────┐
   └──────────┘   nodes relay for                                       │ dashboard │
                  each other (FTD)                                      └───────────┘

CoAP was chosen over speaking MQTT on the node itself. MQTT needs a TCP
connection per node, which is a poor fit for a lossy 802.15.4 mesh with a
roomful of boards; CoAP is a single UDP datagram per reading. Translating to
MQTT once, at the border router, keeps the node tiny and still gives the
dashboard a plain MQTT topic to subscribe to.

Why not Matter? Matter would bring its own commissioning, data model and
certificate handling -- far more stack than a temperature reading justifies.

.. warning::

   **This sample is not a production configuration.**

   The Thread network key is hard-coded in :file:`prj.conf` and therefore
   identical on every board and published in this repository. Anyone in radio
   range can join the mesh, decrypt all traffic and inject their own. That is
   an acceptable trade for a workshop -- it removes commissioning from the
   exercise -- but a real deployment must commission each device instead, for
   example with Thread's in-band joiner flow
   (``CONFIG_OPENTHREAD_JOINER`` plus a per-device PSKd) or by provisioning the
   operational dataset out of band.

   The telemetry itself is also unauthenticated: the collector cannot tell a
   genuine node from a spoofed one, and node names are self-declared.

Requirements
************

* A TiaC CoffeeCaller nRF52 board (revision 0.9.0 or 1.0.x).
* A Thread border router on the same channel, PAN ID and network key, with a
  CoAP-to-MQTT bridge listening on the ``telemetry`` resource.
  :ref:`iot_collector` is both: an nRF52840 DK as a Radio Co-Processor and a
  Docker Compose stack that reads the credentials straight out of this
  sample's :file:`prj.conf`.

The ``openthread`` module must be present in the workspace. It is in the
manifest allowlist, so a plain ``west update`` is enough:

.. code-block:: console

   west update

Building and running
********************

.. code-block:: console

   west build -b coffeecaller_nrf52/nrf52840 samples/iot/thread_telemetry
   west flash

For the first prototype run of the board, select the revision explicitly:

.. code-block:: console

   west build -b coffeecaller_nrf52@0.9.0/nrf52840 samples/iot/thread_telemetry

Setting the node name
*********************

Open the USB CDC-ACM console and give the board a name. It is stored in NVS
and survives a reboot and a re-flash:

.. code-block:: console

   uart:~$ telemetry name bench-window
   Node name is now 'bench-window' and survives a reboot

   uart:~$ telemetry status
   Node name : bench-window
   Collector : coap://[ff03::1]/telemetry
   Interval  : 15 s
   Mesh role : child

Without a stored name the node falls back to one derived from its EUI-64, such
as ``node-a1b2c3``, so two unconfigured boards never collide on the dashboard.

Other commands:

.. code-block:: console

   uart:~$ telemetry publish              # send one sample immediately
   uart:~$ telemetry collector fd11:22::1 # change the destination (not persisted)
   uart:~$ telemetry display humidity     # switch the LED strip without SW0
   uart:~$ ot state                       # OpenThread shell: child, router, leader...
   uart:~$ ot router table                # show the mesh topology

Each of these is contributed by the module that implements it, so
``telemetry --help`` lists exactly the commands that were built into the
image. See `Shell bindings`_.

LD1 lights up once the node has attached to the mesh.

LED strip display
*****************

The four WS2812 LEDs visualise the reading that the node is publishing, which
makes the sensor tangible: a hand cupped over the board moves the bar within a
second or two. **SW0 switches between the two quantities.**

**Temperature** is a thermometer bar over a deliberately narrow range, 20 to
40 °C by default. Each LED owns one quarter of that range, so how many LEDs are
lit says which quarter the reading is in. The leading LED then shows how far
through its own quarter the reading has got, by colour rather than brightness:
it runs green, yellow, orange, red, and turns red exactly as that quarter
completes and the next LED starts at green again.

.. code-block:: none

   quarter   1: 20-25 C   2: 25-30 C   3: 30-35 C   4: 35-40 C

   20 C   green    ·        ·        ·
   24 C   orange   ·        ·        ·
   25 C   red      green    ·        ·
   33 C   red      red      orange   ·
   38 C   red      red      red      orange
   40 C   red      red      red      red

Reading it takes one glance: count the lit LEDs for the coarse value, then look
at the colour of the last one for the rest. That gives four LEDs a far finer
resolution than four discrete steps, and every LED stays at full configured
brightness, so the fine detail is still legible across a room.

The range is narrow on purpose. A hand on the board is the only heat source in
the room, and it moves the reading by a few degrees; over a 0-100 °C range that
would be invisible. Readings outside the range clamp to the ends.

**Humidity** uses exactly the same bar, across 20 to 80 % -- a range breathing
on the sensor is enough to cross -- but with a blue-to-purple gradient instead
of the temperature one:

.. code-block:: none

   quarter   1: 20-35 %   2: 35-50 %   3: 50-65 %   4: 65-80 %

   20 %   blue     ·        ·        ·
   32 %   violet   ·        ·        ·
   35 %   purple   blue     ·        ·
   62 %   purple   purple   violet   ·
   80 %   purple   purple   purple   purple

A quarter completes when its LED reaches purple, the way a temperature quarter
completes at red.

The two palettes are deliberately disjoint: every temperature colour has its
blue channel at zero and every humidity colour has its green channel at zero,
so no reading in one mode can ever look like a reading in the other. A glance
at the strip is enough to tell which quantity is on display. The mode is also
reported by ``telemetry display``, which can set it as well, and logged
whenever it changes.

Neither mode ever goes fully dark: the lowest state is a full-brightness first
LED, green for temperature and blue for humidity, so a cold or dry reading is
not mistaken for a dead board.

The whole strip is scaled by ``CONFIG_TELEMETRY_DISPLAY_BRIGHTNESS``, half by
default. At full output the WS2812s are genuinely unpleasant to sit in front of
on a desk. Note that this is a linear duty cycle while perceived brightness is
not, so half still looks roughly three quarters as bright -- go lower if the
room is dim.

The strip refreshes every ``CONFIG_TELEMETRY_DISPLAY_INTERVAL_MS`` (500 ms by
default), far more often than readings are published, so the feedback feels
immediate while the mesh traffic stays at one message per interval.

Set ``CONFIG_TELEMETRY_DISPLAY=n`` to build without it. The whole module drops
out of the build, including its ``telemetry display`` command.

Published data
**************

Each interval the node sends a CoAP ``POST`` to ``/telemetry`` with
``Content-Format: application/json``:

.. code-block:: json

   {"node":"bench-window","temp_c":22.41,"hum_pct":47.83,"uptime_s":315}

The bridge is expected to republish this on a topic derived from the node
name, for example ``workshop/telemetry/bench-window``.

Addressing
**********

By default the POST goes to ``ff03::1``, the realm-local all-nodes multicast
address. This means **no per-node network configuration at all**: the message
reaches the border router wherever it sits, which is what makes the demo work
in a room of boards that were flashed with an identical image.

The cost is that every node in the mesh also receives the datagram. Once the
collector's address is known, point the nodes at it directly, either with
``CONFIG_TELEMETRY_COLLECTOR_ADDRESS`` at build time or with
``telemetry collector <addr>`` at runtime.

Messages are sent non-confirmable, which is also what the multicast default
requires. See `Why non-confirmable POSTs`_.

Configuration
*************

.. list-table::
   :header-rows: 1

   * - Option
     - Default
     - Meaning
   * - ``CONFIG_TELEMETRY_COLLECTOR_ADDRESS``
     - ``ff03::1``
     - Destination of the CoAP POST
   * - ``CONFIG_TELEMETRY_URI_PATH``
     - ``telemetry``
     - CoAP resource on the collector
   * - ``CONFIG_TELEMETRY_INTERVAL_S``
     - ``15``
     - Seconds between publications
   * - ``CONFIG_TELEMETRY_NAME_MAX_LEN``
     - ``24``
     - Longest allowed node name
   * - ``CONFIG_TELEMETRY_DISPLAY``
     - ``y``
     - Visualise the readings on the LED strip
   * - ``CONFIG_TELEMETRY_DISPLAY_INTERVAL_MS``
     - ``500``
     - LED strip refresh interval
   * - ``CONFIG_TELEMETRY_DISPLAY_TEMP_MIN_C`` / ``_MAX_C``
     - ``20`` / ``40``
     - Temperature range shown across the strip
   * - ``CONFIG_TELEMETRY_DISPLAY_HUM_MIN_PCT`` / ``_MAX_PCT``
     - ``20`` / ``80``
     - Humidity range shown as blue intensity
   * - ``CONFIG_TELEMETRY_DISPLAY_BRIGHTNESS``
     - ``500``
     - Overall LED brightness, in permille
   * - ``CONFIG_TELEMETRY_STATUS_LED``
     - ``y``
     - Let LD1 follow the Thread attachment state
   * - ``CONFIG_TELEMETRY_SHELL``
     - ``y``
     - Build the ``telemetry`` command tree

Each module also has a ``CONFIG_TELEMETRY_<module>_LOG_LEVEL`` and a
``CONFIG_TELEMETRY_<module>_INIT_PRIORITY``; see `Module layout`_.

The nodes are built as Full Thread Devices (``CONFIG_OPENTHREAD_FTD``) so they
route for each other and the mesh is actually visible in ``ot router table``.
A battery-powered sensor would normally be a Minimal Thread Device instead
(``CONFIG_OPENTHREAD_MTD`` with ``CONFIG_OPENTHREAD_MTD_SED``).

Implementation notes
********************

Module layout
=============

Each concern lives in its own directory under :file:`src/`, with its
implementation, its header, its :file:`Kconfig.<module>` and its
:file:`CMakeLists.txt` next to each other:

.. code-block:: none

   src/env_sensor/   read the SHT4x, serialised between callers
   src/node_name/    the participant-chosen name, in NVS
   src/mesh/         the Thread role, and the log line when it changes
   src/telemetry/    format the JSON and POST it over CoAP
   src/display/      the LED strip bar and SW0         (optional)
   src/status_led/   LD1 follows the attachment state  (optional)

Nothing is started from :file:`main.c`. Every module registers its own
``SYS_INIT`` hook at the ``APPLICATION`` level and brings itself up:

.. code-block:: c

   SYS_INIT(telemetry_init, APPLICATION, CONFIG_TELEMETRY_INIT_PRIORITY);

That leaves :c:func:`main` with nothing to do but report that the boot
sequence finished, which is the point: a module can be added to or removed
from the node without touching a central bring-up function.

The priority of each hook is a Kconfig option, and that is where the
dependencies between the modules are written down -- the sensor and the node
name come up at 80, the mesh state at 85, and the modules that use them at 90.
Everything at the ``APPLICATION`` level runs after the network stack has been
initialised at ``POST_KERNEL``, so the OpenThread instance already exists by
the time any of these hooks run.

A ``SYS_INIT`` hook has nowhere to return an error to -- the kernel ignores the
return value -- so each module logs its own failure and leaves the rest of the
node running. A board with a dead LED strip still publishes.

The optional modules are left out of the build entirely instead of being
compiled and disabled at runtime, which is also why the sources are listed
per module rather than globbed:

.. code-block:: cmake

   add_subdirectory(src/telemetry)
   add_subdirectory_ifdef(CONFIG_TELEMETRY_DISPLAY src/display)

Each module registers its own log module too, so ``display`` and
``telemetry`` appear under their own names in the log and can be silenced
independently with ``CONFIG_TELEMETRY_<module>_LOG_LEVEL``.

Shell bindings
==============

No implementation file contains shell code. Each module keeps its commands in
its own :file:`<module>_shell.c`, compiled only when ``CONFIG_TELEMETRY_SHELL``
is set:

.. code-block:: cmake

   target_sources(app PRIVATE display.c)
   target_sources_ifdef(CONFIG_TELEMETRY_SHELL app PRIVATE display_shell.c)

The single ``telemetry`` command that participants type is assembled from
those files at link time. :file:`telemetry/telemetry_shell.c` creates the
command set and registers the root command:

.. code-block:: c

   SHELL_SUBCMD_SET_CREATE(telemetry_cmds, (telemetry));
   SHELL_CMD_REGISTER(telemetry, &telemetry_cmds, "Thread telemetry node commands", NULL);

and every module adds its own subcommands to that set from its own file,
without any module knowing about the others:

.. code-block:: c

   SHELL_SUBCMD_ADD((telemetry), display, NULL, "...", cmd_display, 1, 1);

``telemetry display`` therefore appears in ``telemetry --help`` exactly when
the display module is built, and there is no ``#ifdef`` anywhere in the
sources.

Payload size
============

:c:macro:`TELEMETRY_PAYLOAD_SIZE` is derived from
``CONFIG_TELEMETRY_NAME_MAX_LEN`` rather than being a fixed number, so raising
the name limit cannot silently start truncating publications:

.. list-table::
   :header-rows: 1
   :widths: 60 15

   * - Component
     - Bytes
   * - JSON scaffolding ``{"node":"","temp_c":,"hum_pct":,"uptime_s":}``
     - 44
   * - Node name
     - ``CONFIG_TELEMETRY_NAME_MAX_LEN``
   * - Two readings, ``2 * (CENTI_STR_SIZE - 1)``
     - 30
   * - Uptime in seconds, ``int64_t`` with sign
     - 20
   * - Terminating NUL
     - 1
   * - **Total at the default name limit of 24**
     - **119**

A ``BUILD_ASSERT`` keeps the total under 256 bytes. It is a guard rail, not a
protocol limit: the reading should stay inside a single CoAP datagram, and the
name is retransmitted by every node on every interval, so a generous name limit
costs airtime across the whole mesh.

Formatting the readings
=======================

``centi_str()`` converts a :c:struct:`sensor_value` to two decimals without
floating point, keeping ``CONFIG_CBPRINTF_FP_SUPPORT`` out of the image.

``val1`` and ``val2`` carry the same sign, so combining them into a single
centi-unit integer preserves it. The sign is then printed separately, because
for a value such as -0.5 the whole part is ``0`` and ``%d`` would drop the
minus -- the reading would arrive at the dashboard as ``0.50``.

The JSON is assembled with :c:func:`snprintk` rather than the JSON library:
the payload has four fixed fields, and the readings are already strings.

Why non-confirmable POSTs
=========================

Telemetry is periodic, so a dropped sample costs less than the retransmissions
a confirmable message would put on the mesh -- the next reading is a few
seconds away. It is also what makes the multicast default work at all, as
OpenThread rejects confirmable messages sent to a multicast address.

Because nothing is acknowledged, :c:func:`otCoapSendRequest` is called without
a response handler. Note that it takes ownership of the message only on
success, which is why the error paths free it explicitly.

Cleaning up two things at once -- the message and the OpenThread mutex -- is
what usually turns a function like this into a ladder of ``goto`` labels.
Driver code does that, but there is no need for it here: each of the three
functions owns exactly one thing, so every one of them can just return.
:c:func:`telemetry_send` takes and releases the mutex around a single call,
:c:func:`coap_post` owns the message from allocation to hand-over, and
:c:func:`coap_message_fill` only appends and reports the first failure.
:c:func:`env_sensor_read` is split the same way, around its own mutex.

Driving the LED strip over SPI
==============================

The board wires the WS2812 strip to a plain GPIO and the devicetree describes
it as ``worldsemi,ws2812-gpio``. This sample overrides that in
:file:`boards/coffeecaller_nrf52.overlay`, disabling the GPIO node and
re-describing the same pin (P0.26) as ``worldsemi,ws2812-spi`` on SPI1, at
4 MHz with ``0x70``/``0x40`` bit frames.

The strip is write-only, so MISO is left unassigned, but **SCK has to be given
a pin even though the strip never sees it**: nRF SPIM documents MOSI and MISO
as optional and SCK as mandatory, and without it the peripheral never clocks
and every transfer ends in ``-ETIMEDOUT``. The overlay spends P0.15 on it,
which is a free pin on the 2x8 header of board revision 1.0.x and is not routed
at all on revision 0.9.0. It carries a 4 MHz burst on every strip refresh.
``WS2812_SCK_PIN`` at the top of the overlay changes it; any otherwise unused
pin works.

There are two reasons.

The immediate one is that ``ws2812_gpio.c`` does not currently build against
Zephyr ``main``. It selects its clock path on ``CONFIG_CLOCK_CONTROL_NRF``,
which no longer exists now that the nRF clock driver has been split into
``CLOCK_CONTROL_NRF_HFCLK`` and ``_LFCLK``; the surviving branch still refers
to a ``drv_data`` variable that was removed, so it fails to compile.

The better reason is that the GPIO driver bit-bangs with interrupts locked for
the whole transfer -- roughly 120 µs for four LEDs, every refresh. This node
also runs an 802.15.4 radio that needs timely interrupt service, so handing the
waveform to SPI hardware avoids putting the mesh and the display in
competition. If the GPIO driver is fixed upstream, the overlay is still the
preferable configuration on a board that is doing radio work.

Sampling from two places
========================

The strip refreshes several times per publication interval, so the display and
the publish path both need readings, and ``telemetry publish`` makes the shell
thread a third caller. The SHT4x driver is not reentrant, and the sensor
belongs to none of the three: :file:`src/env_sensor/` owns the device and
wraps every access in a mutex. All of them go through
:c:func:`env_sensor_read`, so the display does not depend on the publisher.

Both the publish work and the strip refresh run on the system workqueue, which
is why ``CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE`` is raised -- the CoAP path
formats its JSON payload on that stack. A node doing more than this would give
the publisher a work queue of its own, so that a slow sensor read or radio
send cannot delay unrelated system work.

Shared state
============

Two pieces of state change while the node runs, both written by the shell
thread while a work handler reads them: the node name and the collector
address. Each is guarded by a mutex inside its own module, and the getters
copy into a caller-provided buffer instead of returning a pointer into that
storage, so a reader can never see a half-written string.

The displayed quantity is written by both SW0 and the shell and read by the
refresh work, but it is a single value rather than a buffer, so it is an
``atomic_t``.

The collector address is parsed when it is set rather than once per
publication, which is also what makes a malformed
``CONFIG_TELEMETRY_COLLECTOR_ADDRESS`` fail at boot rather than on every
interval.

Publishing and the mesh state
=============================

The periodic work checks :c:func:`mesh_is_attached` first and skips the sensor
read entirely while the node is detached, since the send would only fail down
in the radio driver. The work item reschedules itself either way, so a node
that attaches late starts publishing on its next interval without any extra
bookkeeping.

The role itself lives in :file:`src/mesh/`, which is the only module that asks
OpenThread for it. LD1 does not go through that module's state: OpenThread
keeps a list of state changed callbacks, so :file:`src/status_led/` registers
one of its own and the two concerns stay separate.

The node name falls back to one derived from the last three bytes of the IEEE
EUI-64, so a board that nobody has configured still appears under a unique
name instead of colliding with every other unnamed board.

Resetting a board
*****************

The node name and the Thread dataset live in the storage partition, which sits
outside the application partition. Flashing a different UF2 therefore does
**not** clear them -- a board handed on to the next person keeps its old name.

To wipe it, flash :ref:`iot_factory_reset` and wait for the LEDs, then flash
the application back on top. That sample documents the flash layout and what
survives an erase.

Troubleshooting
***************

**The node never attaches** (``ot state`` stays ``detached`` or ``disabled``).
OpenThread prefers the operational dataset stored in NVS over the values in
:file:`prj.conf`, so a board that was previously flashed with different
credentials keeps the old ones. Clear it and reboot:

.. code-block:: console

   uart:~$ ot dataset clear
   uart:~$ ot factoryreset

**No data arrives, but the node says it is attached.** Confirm the border
router is on the same channel and PAN ID, then check reachability from the
node with ``ot ping <collector address>``. Remember that ``ff03::1`` only
crosses the mesh, not the wider network -- the bridge has to sit on the Thread
side of the border router or subscribe to the forwarded multicast.
