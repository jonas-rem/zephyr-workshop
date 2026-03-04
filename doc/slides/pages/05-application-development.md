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
  <img src="/images/zbus_zephyr.svg" class="h-60 object-contain" />
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
- Switch between system states (active, sleep)

**LED**
- Indicate system state
- Run in own thread for smooth animations

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/zbus_application.png" class="h-60 object-contain" />
  <div class="text-xs text-center mt-2">Minimal modular application with zbus</div>
</div>

</div>

---

## Modular Development

<div class="grid grid-cols-2 gap-4">

<div>

- **General:** Code reuse, maintainability, readability
- **IPC:** Communication via Zbus
- **Context:** Each component can be controlled independently
- **Testing:** Components can be tested separately

</div>

<div>

```text
app/
├── CMakeLists.txt
├── Kconfig
├── prj.conf
└── src
    ├── common
    │   ├── CMakeLists.txt
    │   ├── message_channel.c
    │   └── message_channel.h
    ├── main.c
    └── components
        ├── button
        │   ├── button.c
        │   ├── CMakeLists.txt
        │   ├── Kconfig.button
        │   └── tests/
        └── led
            ├── led.c
            ├── CMakeLists.txt
            └── Kconfig.led
```

</div>

</div>

---

## Starting Components via System Initialization (SYS_INIT)

<div class="grid grid-cols-2 gap-4">

<div>

**Automatic Initialization**

- Runs after drivers/ZBus, before `main()`
- Configurable priority

**Boot Sequence:**
```text
Kernel
  ↓
drivers/ZBus
  ↓
SYS_INIT functions via priority
  ↓
Component threads
  ↓
main()
```
<v-click>

<br>

**Benefits:**

- Testability via Decoupled modules

</v-click>

</div>

<div>

<v-click>

**button.c:**
```c
SYS_INIT(init, APPLICATION,
         CONFIG_BUTTON_MODULE_INIT_PRIORITY);
```

**Boot Log:**
```text
*** Booting Zephyr OS build v4.3.0 ***
<inf> button_module: Set up button at gpio_emul pin 1
<inf> button_module: Button module started
<inf> sys_ctrl: System control started
<inf> led_module: LED module started
<inf> app: Main thread going to sleep.
```

</v-click>

</div>

</div>

---

## Testing Components in Isolation

<div class="grid grid-cols-2 gap-4">

<div>

**Isolated testing of components**
- Zephyr Test framework (`Ztest`)
- represents an integration test
- co-located with components
- Interfaces abstracted via Zbus
- Hardware emulation via `native_sim`

**Test Structure:**
```text
app/src/components/button/tests/
├── CMakeLists.txt
├── prj.conf
├── testcase.yaml
└── src/
    └── test_button.c
```

</div>

<div>

**test_button.c**: direct access to zbus events and emulated hardware (button, sensor, led).

<br>

**Running Component Tests:**
```bash
# Single component test
west twister -T app/src/components/button/tests \
  -v --integration -p native_sim

# All component tests
west twister -T app/src/components/ --integration

# Use existing build artifacts for faster testing
west build -b native_sim app/src/components/button/tests
west build -t run
  *** Booting Zephyr OS build v4.3.0 ***
  START - test_button_press_sleep_to_standby
  [00:00:00.060,000] <inf> sys_ctrl: System state sleep
   PASS - test_button_press_sleep_to_standby in 0.000 s
  [..]
```

</div>

</div>

---

## Testing Components in Isolation - Results

<div class="grid grid-cols-2 gap-4 items-start">

<div>

**Run Button Test**

```bash
west twister -T app/src/components/button/tests \
  --integration -p native_sim
```

**Key Artifacts**
```text
twister-out/
├── twister_report.xml          # JUnit XML report
└── native_sim_native/
    └── host/zephyr-workshop/
        └── app/src/components/
            └── button/tests/
                └── component.button/
                    ├── handler.log       # Test output
                    └── build.log         # Build output
```

</div>

<div>

**Sample Output** (handler.log)
```text
Running TESTSUITE button_test_suite
START - test_button_module_initialized
 PASS - test_button_module_initialized in 0.000 seconds
START - test_button_press_creates_event
 PASS - test_button_press_creates_event in 0.190 seconds
...
TESTSUITE button_test_suite succeeded
SUITE PASS - 100.00% [button_test_suite]:
  pass = 4, fail = 0, skip = 0, total = 4
```

**Generate HTML Report**
```bash
pip install junit2html
junit2html twister-out/twister_report.xml report.html
```

</div>

</div>

---

## Automated Testing with Twister

<div class="grid grid-cols-2 gap-4">

<div>

**Zephyr Test Runner (Twister)**
- `west twister` - Automates building and running tests
- Supports multiple platforms (real HW or simulation)
- Component tests via `testcase.yaml`

</div>

<div>

**Run Integration Tests:**
```shell
west twister -T app/ --integration
```

<br>

Run all tests found in the **app/** dir. This can be build-only, unit-,
integration- or e2e tests. With or without hardware.

<br>


**Output:**
```text
Total complete:   14/  14  100%
    built (not run):    8,
    failed:             0,
    error:              0
Run completed
```

</div>

</div>

---

## Hands-on 4: Extend the Application

<div class="grid grid-cols-2 gap-4">

<div>

Currently: **standby**, **sleep**.
**Task:** Add a third state: **active**.
Cycle via button: `standby` -> `sleep` -> `active`

- **Sleep:** LED off
- **Standby:** LED blinking
- **Active:** LED on

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/zbus_application.png" class="h-60" />
</div>

</div>

---
---

## Hands-on 5: BLE Sensor Improvements

<div class="grid grid-cols-2 gap-4">

<div>

`06_ble` measures the temp with the sensor in the nRF52840 die. A higher
precision is possible by using the TI HDC1010 sensor.

**Change the sample accordingly:**

- Confirm that the sensor is working (Sensor Shell)
- Enable the Sensor API
- Change the sensor device and get HDC1010 from DTS
- `SENSOR_CHAN_DIE_TEMP` → `SENSOR_CHAN_AMBIENT_TEMP`

**Can you confirm a faster reaction time?**

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/reel_board_hdc1010.jpg" class="h-60 object-contain" />
  <div class="text-xs text-center mt-2">TI HDC1010 Temperature and Humidity Sensor</div>
</div>

</div>

<Footnotes y="col">
  <Footnote :number="1">Hint: Check the sensor sample `05_sensor` for reference. Only a few lines need to be changed.</Footnote>
</Footnotes>
