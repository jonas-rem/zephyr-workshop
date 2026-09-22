---
layout: section
level: 1
---

# Application Development

---
---

## IPC Mechanisms and Zephyr bus (Zbus)

<div class="grid grid-cols-2 gap-4">

<div>

**Classic**
- Mutexes, Semaphores
- Conditional Variables, Message Queues
- Polling API to wait for any out of multiple conditions

**Zbus**
- Comparable to D-Bus in Linux
- Many-to-many communication
- Simplifies thread synchronization

</div>

<div class="flex flex-col items-center justify-center">
  <img src="../public/images/zbus_zephyr.svg" class="h-60 object-contain" />
  <div class="text-xs text-center mt-2">Zbus overview</div>
</div>

</div>

<Footnotes y="col">
  <Footnote :number="1"><a href="https://docs.zephyrproject.org/latest/services/zbus/index.html">docs.zephyrproject.org/latest/services/zbus</a></Footnote>
</Footnotes>

---
---

## Example Application

<div class="grid grid-cols-2 gap-4">

<div>

**Button**
- Debounced `sw0` press
- Publishes `button_event_ch`

**Sensor**
- Reads the HDC on `BUTTON_EVENT_0_PRESSED`
- Publishes lifecycle events on `sensor_event_ch`

**LED**
- On while a read is in progress

</div>

<div>

<div class="text-xs">

```text
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
```

</div>

<div class="text-xs text-center mt-2">Button, sensor, and LED on typed channels</div>

</div>

</div>

---

## Modular Development

<div class="grid grid-cols-2 gap-4">

<div>

- **General:** Code reuse, maintainability, readability
- **IPC:** Communication via Zbus
- **Context:** Each module is Kconfig-gated
- **Testing:** Each module has its own test

</div>

<div>

```text
samples/simulation/
├── CMakeLists.txt
├── Kconfig
├── prj.conf
├── tests.yaml
└── src
    ├── main.c
    ├── common
    │   ├── message_channel.h
    │   └── message_types.h
    └── modules
        ├── button/
        ├── led/
        └── sensor/
```

</div>

</div>

---

## Starting the Modules

<div class="grid grid-cols-2 gap-4">

<div>

**Button:** `SYS_INIT`, before `main()`

**Sensor and LED:** static threads, init inside the thread

**Boot sequence:**
```text
Kernel
  ↓
drivers / ZBus channels
  ↓
SYS_INIT (button)
  ↓
static threads (sensor, LED) and main()
```

</div>

<div>

**button.c:**
```c
SYS_INIT(button_init, APPLICATION,
         CONFIG_APP_SIM_BUTTON_INIT_PRIORITY);
```

**Boot log:**
```text
*** Booting Zephyr OS build v4.4.0 ***
<inf> app_simulation_button: Button initialized
<inf> app_simulation: App simulation booted
<inf> app_simulation_led: LED initialized
<inf> app_simulation_sensor: Sensor initialized
```

</div>

</div>

---

## Testing Modules in Isolation

<div class="grid grid-cols-2 gap-4">

<div>

**Component test per module**
- Zephyr Test framework (`Ztest`)
- Co-located with the module
- Contract is the Zbus channel
- Hardware via `native_sim` emulators

**Test structure:**
```text
samples/simulation/src/modules/button/tests/
├── CMakeLists.txt
├── prj.conf
├── tests.yaml
└── src/
    └── test_button.c
```

</div>

<div>

**test_button.c:** press the emulated GPIO, observe `button_event_ch`.

<br>

**Running component tests:**
```bash
# One module
west twister -T samples/simulation/src/modules/button/tests \
  -v --integration -p native_sim

# All modules
west twister -T samples/simulation/src/modules --integration

# Log on the console
west build -b native_sim samples/simulation/src/modules/button/tests
west build -t run
```

</div>

</div>

---

## Testing Modules in Isolation - Results

<div class="grid grid-cols-2 gap-4 items-start">

<div>

**Run button test**

```bash
west twister -T samples/simulation/src/modules/button/tests \
  --integration -p native_sim
```

**Key artifacts**
```text
twister-out/
└── native_sim/
    └── .../button/tests/
        └── app_simulation.component.button/
            ├── handler.log
            └── build.log
```

</div>

<div>

**Sample output** (handler.log)
```text
Running TESTSUITE button_test
START - test_press_publishes_request
 PASS - test_press_publishes_request
START - test_bounce_publishes_one_request
 PASS - test_bounce_publishes_one_request
START - test_repeated_presses_publish_repeated_requests
 PASS - test_repeated_presses_publish_repeated_requests
TESTSUITE button_test succeeded
SUITE PASS - 100.00% [button_test]:
  pass = 3, fail = 0, skip = 0, total = 3
```

</div>

</div>

---

## Automated Testing with Twister

<div class="grid grid-cols-2 gap-4">

<div>

**Zephyr Test Runner (Twister)**
- `west twister` builds and runs tests
- Host (`native_sim`) or hardware
- Scenarios come from `tests.yaml`

</div>

<div>

**Integration run:**
```shell
west twister -T samples/simulation --integration
```

<br>

Boot, component, and `native_sim` end-to-end tests under `samples/simulation/`:

```text
app_simulation.basic
app_simulation.component.button
app_simulation.component.led
app_simulation.component.sensor
app_simulation.e2e.native_sim
```

`app_simulation.e2e.hil` is the same shell test on `reel_board`. It is not in the integration set.

</div>

</div>
