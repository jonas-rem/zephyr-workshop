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
  <img src="/images/zbus_application.svg" class="h-60 object-contain" />
  <div class="text-xs text-center mt-2">Minimal modular application with zbus</div>
</div>

</div>

---

## Modular Development

<div class="grid grid-cols-2 gap-4">

<div>

- **General:** Code reuse, maintainability, readability
- **IPC:** Communication e.g. via Zbus
- **Context:** Each module can be controlled independently
- **Testing:** Modules can be tested separately

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
    └── modules
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

## Testing Modules

<div class="grid grid-cols-2 gap-4">

<div>

**Isolated testing of modules**
- Add subsystems like `shell`, `ztest` for test cases
- Tests reference modules via CMake
- Interfaces abstracted via Zbus

**Example Test Structure:**
```text
test/
├── button
│   ├── CMakeLists.txt
│   ├── prj.conf
│   └── src/main.c
└── led
    ├── CMakeLists.txt
    └── src/main.c
```

</div>

<div>

```cmake
# test/button/CMakeLists.txt
target_sources(app PRIVATE src/main.c)
add_subdirectory(../../app/src/common common)
add_subdirectory(../../app/src/modules/button button)
```

```c
// test/button/src/main.c
void button_test_msg_cb(const struct zbus_channel *chan) {
    const enum sys_msg *msg = zbus_chan_const_msg(chan);
    if (*msg == SYS_BUTTON_PRESSED) {
        LOG_INF("Button pressed!");
    }
}
ZBUS_LISTENER_DEFINE(button_test, button_test_msg_cb);
ZBUS_CHAN_ADD_OBS(button_ch, button_test, 1);
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
  - reel_board
  - frdm_k64f
  - nrf52840dk/nrf52840
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
  <img src="/images/zbus_application.svg" class="h-60" />
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
