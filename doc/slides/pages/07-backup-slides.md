---
layout: section
level: 1
hideInToc: true
---

# Backup Slides

---
layout: default
---

## Debugging: git bisect

<div class="grid grid-cols-2 gap-4">

<div>

- Something worked in the past, but does not work anymore
- **Example:** The lvgl sample worked on the reel_board in v4.0.0, but not with the current main (7fc9c26fb0d)

</div>

<div>

```shell {1-3|4-8|10-11|12-19}
git bisect start
git bisect good v4.0.0
git bisect bad 7fc9c26fb0d
west update && \
west build -b reel_board@2 samples/subsys/display/lvgl/ -p && \
west flash
# -> After flashing, test console and display to see if the sample works
git bisect good|bad

  ... (repeat binary search for log2(N) steps)
  # log2(10000) ≈ 13.3 -> max 14 steps for 10k commits!

git bisect good
1ea35e is the first bad commit
commit 1ea35e237cc0aa26b8a3517144528e9781a56781
Author: ***
Date:   Thu Jan 25 11:09:06 2024 +0100

    west.yml: Update lvgl module to v9.2.0
```

<p class="text-xs text-center mt-2">Find the commit that caused a bug with <i>git bisect</i></p>

</div>

</div>

---
layout: default
---

## Renode: Technical Overview

<div class="grid grid-cols-2 gap-4">

<div>
<v-clicks>

- Testing of embedded systems in a reproducible virtual environment
- Simulation of unmodified binaries for target hardware
- Simulates entire SoCs, including CPUs, peripherals, and interconnects

</v-clicks>
</div>

<div>
<v-clicks>

- Enables complex network simulations, both wired and wireless
- Allows for interactive debugging, system state inspection, and automated testing
- Open Source, actively developed by Antmicro

</v-clicks>
</div>

</div>

---
layout: default
---

## Hands-On 4: Extension to 3 States - test

```diff
test/led/src/main.c
@@ -29,7 +29,9 @@ static int cmd_led(...)
                led_msg(SYS_STANDBY);
                break;
        /* Add your code here */
+       case 2:
+               led_msg(SYS_ACTIVE);
+               break;
        /* */
        default:
                shell_print(sh, "Invalid argument");
@@ -41,9 +43,9 @@ static int cmd_led(...)
 SHELL_SUBCMD_DICT_SET_CREATE(sub_led_cmds, cmd_led,
        (sys_sleep, 0, "System is sleeping"),
-       (sys_standby, 1, "System is in standby")
+       (sys_standby, 1, "System is in standby"),
        /* Add your code here */
+       (sys_active, 2, "System is active")
        /* */
 );
```

---
layout: default
---

## Hands-On 4: Extension to 3 States - app

<div class="grid grid-cols-2 gap-4">

<div>

```diff
app/src/common/message_channel.h
@@ -20,6 +20,10 @@ enum sys_states {
        /* Add your code here */
+       SYS_ACTIVE,
        /* */
 };
```

```diff
app/src/main.c
@@ -39,11 +39,14 @@ static void button_msg_cb(...)
        case SYS_STANDBY:
+               sys_state = SYS_ACTIVE;
+               LOG_INF("System state active");
               break;
        /* Add your code here */
+       case SYS_ACTIVE:
+               sys_state = SYS_SLEEP;
+               LOG_INF("System state sleep");
+               break;
        /* */
```

</div>

<div>

```diff
app/src/modules/led/led.c
@@ -75,7 +75,12 @@ static void led_fn(void)
        /* Add your code here */
+       case SYS_ACTIVE:
+               gpio_pin_set_dt(&led, 1);
+               printk("LED on\n");
+               ret = zbus_sub_wait(&led_subscriber, &chan, K_FOREVER);
+               break;
        /* */
```

</div>

</div>

---
layout: default
---

## Hands-On 5: Change Sensor to HDC1010

<div class="grid grid-cols-2 gap-4">

<div>

```diff
diff --git a/samples/06_ble/prj.conf b/samples/06_ble/prj.conf
@@ -8,3 +8,4 @@ CONFIG_BT_BAS=y
 CONFIG_BT_DEVICE_NAME="Zephyr Health Thermometer"
 CONFIG_BT_DEVICE_APPEARANCE=768
 CONFIG_CBPRINTF_FP_SUPPORT=y
+CONFIG_SENSOR=y
```

</div>

<div>

```diff
diff --git a/samples/06_ble/src/hts.c b/samples/06_ble/src/hts.c
@@ -25,8 +25,10 @@
 #include <zephyr/bluetooth/uuid.h>
 #include <zephyr/bluetooth/gatt.h>

+#include <zephyr/drivers/sensor.h>
+
 #ifdef CONFIG_TEMP_NRF5
-static const struct device *temp_dev = DEVICE_DT_GET_ANY(nordic_nrf_temp);
+static const struct device *temp_dev = DEVICE_DT_GET_ANY(ti_hdc);
 #else
 static const struct device *temp_dev;
 #endif
@@ -104,7 +106,7 @@ void hts_indicate(void)
-               r = sensor_channel_get(temp_dev, SENSOR_CHAN_DIE_TEMP,
+               r = sensor_channel_get(temp_dev, SENSOR_CHAN_AMBIENT_TEMP,
                                       &temp_value);
```

</div>

</div>
