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
- **IPC:** Communication e.g. via Zbus
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
        │   └── Kconfig.button
        └── led
            ├── led.c
            ├── CMakeLists.txt
            └── Kconfig.led
```

</div>

</div>

---

## Testing Components in Isolation - Build

<div class="grid grid-cols-2 gap-4">

<div>

**Isolated testing of components**
- Add subsystems like `shell`, `ztest` for test cases
- Tests reference components via CMake
- Interfaces abstracted via Zbus

**Test are just a config:**
```text
app
├── sample.yaml
│   [..]
└── test_cfg
    ├── button_module.conf
    ├── led_module.conf
    └── sys_ctrl_module.conf
```

</div>

<div>

```shell
west twister -T app/ -s app.test.led --integration
# -> Twister will build and run the app with only the led component
west build -b native_sim app -p -- -DCONFIG=led_module.conf

# Self-test the isolated module via shell commands
  uart:~$ led set sys_sleep
  Setting LED to sleep mode (off)

# And the LED stops blinking
  <dbg> led_module: led_fn: LED on
  <dbg> led_module: led_fn: LED off
  <dbg> led_module: led_fn: LED off
```

</div>

</div>

---

## Testing Components in Isolation - Integration Test

<div class="grid grid-cols-2 gap-4 items-start">

<div>

**Running Tests**

```bash
west twister -T app/ -s app.test.led.pytest -v
```

**Results:**
```text
app/test_led_module.py::test_led_module_boot PASSED
app/test_led_module.py::test_led_set_sleep PASSED
app/test_led_module.py::test_led_set_standby PASSED
app/test_led_module.py::test_led_state_persistence PASSED
============== 4 passed in 1.70s =====================
```

</div>

<div>

**Artifacts:** `twister-out/<platform>/<test>/`

- `handler.log` - Device output & shell
- `twister_harness.log` - Pytest execution
- `report.xml` - JUnit results


**Test Result HTML Report:**
```bash
# Convert JUnit XML to HTML
pip install junit2html
junit2html twister-out/twister_report.xml report.html
```

</div>

</div>

---

## Testing Components in Isolation - Unit Test

<div class="grid grid-cols-2 gap-4 items-start">

<div>

**When to Unit Test**

- Complex state machines with edge cases
- Algorithm implementations
- Pure functions without side effects
- Logic that benefits from fast execution (< 1ms)

**Structure:**
```text
test/sys_ctrl/
├── CMakeLists.txt
├── prj.conf
├── testcase.yaml
└── src/
    ├── sys_ctrl.c      # Testable version
    └── test_sys_ctrl.c # Unit tests
```

</div>

<div>

**Running Unit Tests**

```bash
west twister -T test/sys_ctrl --integration
```

**Test Pattern:**
```c
// Include source to access static functions
#include "sys_ctrl.c"

ZTEST(sys_ctrl_suite, test_transition)
{
    sys_ctrl_set_state(SYS_SLEEP);
    sys_ctrl_handle_button_press();
    zassert_equal(sys_ctrl_get_state(), 
                  SYS_STANDBY);
}
```

**Trade-off:** Requires testable version of component.

</div>

</div>

---

## Automated Testing with Twister

<div class="grid grid-cols-2 gap-4">

<div>

**Zephyr Test Runner (Twister)**
- `west twister` - Automates building and running tests
- Supports multiple platforms (real HW or simulation)
- Integration tests via `sample.yaml`

**Run Integration Tests:**
```shell
west twister -T app/ -T test/ --integration
```

</div>

<div>

**Example `sample.yaml`:**
```yaml
integration_platforms:
  - native_sim
  - reel_board
```

**Output:**
```text
INFO - Total complete: 24/24 100%
INFO - 24 of 24 configurations passed
INFO - Run completed
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
