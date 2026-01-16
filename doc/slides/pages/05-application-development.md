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
