---
layout: section
level: 1
---

# Code Examples and Subsystems

---

## Samples in Zephyr

**Samples**
- Zephyr provides a wide range of samples
- Samples are located in `zephyr/samples/`
- Isolated functionality or feature

**Tests**
- Tests are located in `zephyr/tests/`
- Isolated test cases for a feature or hardware
- Useful to test e.g. a device driver

**Applications**
- Application Example:
[github.com/zephyrproject-rtos/example-application](https://github.com/zephyrproject-rtos/example-application)
- ZSWatch - Open Source Smart Watch:
[github.com/jakkra/ZSWatch](https://github.com/jakkra/ZSWatch)

---

## 01_hello_world

<div class="grid grid-cols-2 gap-4">

<div>

**Description:**
- Simple Hello World program<sup>1</sup>

**Learn:**
- Test setup
- Structure of a Zephyr application

**Build and run:**

```shell
west build -b native_sim samples/01_hello_world -p
west build -t run
```

</div>

<div class="flex flex-col items-center justify-center">

```text
samples/01_hello_world/
├── CMakeLists.txt
├── prj.conf
├── README.rst
├── sample.yaml
├── 01_hello_world
└── src
    └── main.c
```

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Equivalent in the Zephyr main Repository: zephyr/samples/hello_world.</Footnote>
</Footnotes>

---

## 01_hello_world Configuring the Build System

```cmake
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)

target_sources(app PRIVATE src/main.c)
```

<div class="text-xs text-center mt-2">`samples/01_hello_world/CMakeLists.txt`</div>

---

## 01_hello_world Application Source Code

```c
/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
	return 0;
}
```

<div class="text-xs text-center mt-2">`samples/01_hello_world/src/main.c`</div>

---

## 01_hello_world Application Build Output

```shell
west build -b native_sim samples/01_hello_world/ -p
-- Found host-tools: zephyr 0.17.0 (/home/jonas/zephyr-sdk-0.17.0)
-- Found toolchain: zephyr 0.17.0 (/home/jonas/zephyr-sdk-0.17.0)
[..]
Parsing /home/jonas/git/zephyrproject/zephyr/Kconfig
-- The C compiler identification is GNU 12.2.0
-- The CXX compiler identification is GNU 12.2.0
-- The ASM compiler identification is GNU
-- Found assembler: /home/jonas/zephyr-sdk-0.17.0/arm-zephyr-eabi/bin/...
-- Configuring done (3.0s)
-- Generating done (0.1s)
-- Build files have been written to: .../zephyr-workshop/build
-- west build: building application
[1/117] Preparing syscall dependency handling

[2/117] Generating include/generated/zephyr/version.h
-- Zephyr version: 4.1.0 (/home/jonas/git/zephyrproject/zephyr), build: v4.1.0
[117/117] Linking C executable zephyr/zephyr.elf
Memory region         Used Size  Region Size  %age Used
           FLASH:        8094 B       256 KB      3.09%
             RAM:          4 KB        64 KB      6.25%
        IDT_LIST:          0 GB        32 KB      0.00%
```

---

## 01_hello_world Build Artifacts

<div class="grid grid-cols-2 gap-4">

<div>

**Build Location:**
- `build/`

**Executable Location:**
- `build/zephyr/`

**Artifacts:**
- `zephyr.elf|hex|bin`
- `zephyr.map`
- `autoconf.h` (Kconfig options)
- `devicetree_generated.h` (Generated devicetree header)
- `Kconfig.dts` (devicetree conf)

</div>

<div class="flex flex-col items-center justify-center">

```text
build/
├── app
│   └── libapp.a
├── Kconfig
│   └── Kconfig.dts
└── zephyr
    ├── include
    │   └── generated
    │      └── zephyr
    │         ├── autoconf.h
    │         ├── devicetree_generated.h
    ├── zephyr.dts
    ├── zephyr.elf|bin|hex
    ├── zephyr_final.map
```

</div>

</div>

---

## 01_hello_world Sample - Console Output

```shell
*** Booting Zephyr OS build v4.1.0 ***
Hello World! native_sim
```

---

## 02_logging Sample

<div class="grid grid-cols-2 gap-4">

<div>

**Description:**
- Logging subsystem example<sup>1</sup>

**Learn:**
- Log levels
- Set loglevel via Kconfig
- Logging backends, e.g. filesystem, BLE
- Log messages printed in own thread when system is idle

**Sample:**
- Demonstrates logging output

</div>

<div class="flex flex-col items-center justify-center">

```ini
CONFIG_LOG=y
```
<div class="text-xs text-center mt-2">samples/02_logging/prj.conf</div><br><br>

```c
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(hello_world, LOG_LEVEL_DBG);

int main(void)
{
	LOG_INF("info string");

	return 0;
}
```
<div class="text-xs text-center mt-2">samples/02_logging/src/main.c</div>

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Equivalent in the Zephyr main Repository: zephyr/samples/subsys/logging/logger.</Footnote>
</Footnotes>

---

## 02_logging Sample - Console Output

```
*** Booting Zephyr OS build v4.1.0 ***
Hello World! native_sim
[00:00:00.001,691] <err> hello_world: error string
[00:00:00.001,843] <dbg> hello_world: main: debug string
[00:00:00.001,859] <inf> hello_world: info string
[00:00:00.001,874] <dbg> hello_world: main: int8_t 1, uint8_t 2
[00:00:00.001,887] <dbg> hello_world: main: int16_t 16, uint16_t 17
[00:00:00.001,899] <dbg> hello_world: main: int32_t 32, uint32_t 33
[00:00:00.001,921] <dbg> hello_world: main: int64_t 64, uint64_t 65
[00:00:00.001,956] <dbg> hello_world: main: char !
[00:00:00.002,383] <dbg> hello_world: main: s str static str c str
[00:00:00.002,567] <dbg> hello_world: main: d str dynamic str
[00:00:00.002,607] <dbg> hello_world: main: mixed str dynamic str --- ...
[00:00:00.002,640] <dbg> hello_world: main: mixed c/s ! static str ...
[00:00:00.002,653] <dbg> hello_world: main: pointer 0x5c3e
[00:00:00.002,674] <dbg> hello_world: main: HeXdUmP!
                                      48 45 58 44 55 4d 50 21  20 23 40  |HEXDUMP!  #@
```

---

## 03_workqueues Sample

**Description:**
- Workqueue and Timer Example

**Learn:**
- Workqueue, Timers, Runtime Contexts (IRQ, Thread)
- Queue of work items
- Work items are executed in a thread context
- Timer is used to schedule work items
- System workqueue is enabled by default

**Sample:**
- Executes a function in different contexts

---

## 03_workqueues Sample - Console Output

```shell
*** Booting Zephyr OS build v4.1.0 ***
Work Item Executed - runtime context:
 Thread Name: main
 Thread Priority: 0

Work Item Executed - runtime context:
 Thread Name: sysworkq
 Thread Priority: -1

Work Item Executed - runtime context:
 Thread Name: my_work_q_thread
 Thread Priority: 5

Timer Expired!!
Work Item Executed - runtime context:
 ISR Context!

Work Item Executed - runtime context:
 Thread Name: sysworkq
 Thread Priority: -1
```

---

## 04_shell Sample

<div class="grid grid-cols-2 gap-4">

<div>

**Description:**
- Shell subsystem example<sup>1</sup>

**Learn:**
- Command line interface
- Command history, completion, help
- Great for hardware validation, testing and debugging

**Sample:**
- Provides a basic shell

</div>

<div class="flex flex-col items-center justify-center">

```ini
CONFIG_SHELL=y

# Optional features
CONFIG_THREAD_STACK_INFO=y
CONFIG_KERNEL_SHELL=y
CONFIG_THREAD_MONITOR=y
CONFIG_BOOT_BANNER=n
CONFIG_THREAD_NAME=y
CONFIG_DEVICE_SHELL=y
CONFIG_POSIX_CLOCK=y
CONFIG_DATE_SHELL=y
CONFIG_THREAD_RUNTIME_STATS=y
CONFIG_THREAD_RUNTIME_STATS_USE_TIMING_FUNCTIONS=y
CONFIG_STATS=y
CONFIG_STATS_SHELL=y
CONFIG_SENSOR=y
CONFIG_SENSOR_SHELL=y
CONFIG_SENSOR_INFO=y
CONFIG_I2C_SHELL=y
```

<div class="text-xs text-center mt-2">samples/04_shell/prj.conf</div>

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Equivalent in the Zephyr main Repository: zephyr/samples/subsys/shell/shell_module.</Footnote>
</Footnotes>

---

## 04_shell Sample - Console Output

```shell {1-8|9-21}
uart:~$
  bypass              clear               date
  demo                device              devmem
  dynamic             help                history
  kernel              log                 log_test
  rem                 resize              retval
  section_cmd         shell               shell_uart_release
  stats               version
uart:~$ kernel thread list
Threads:
*0x20000720 shell_uart
	options: 0x0, priority: 14 timeout: 0
	state: queued, entry: 0x3ed9
	Total execution cycles: 22360 (0 %)
	stack size 2048, unused 932, usage 1116 / 2048 (54 %)

 0x20001288 sysworkq
	options: 0x1, priority: -1 timeout: 0
	state: pending, entry: 0x7169
	Total execution cycles: 163 (0 %)
	stack size 1024, unused 808, usage 216 / 1024 (21 %)
...
```

---

## 04_shell Sample - I2C Shell

```shell {1|2-15|17-24}
uart:~$ i2c -h
i2c - I2C commands
Subcommands:
  scan         : Scan I2C devices
                 Usage: scan <device>
  recover      : Recover I2C bus
                 Usage: recover <device>
  read         : Read bytes from an I2C device
                 Usage: read <device> <addr> <reg> [<bytes>]
  read_byte    : Read a byte from an I2C device
                 Usage: read_byte <device> <addr> <reg>
  write        : Write bytes to an I2C device
                 Usage: write <device> <addr> <reg> [<byte1>, ...]
  write_byte   : Write a byte to an I2C device
                 Usage: write_byte <device> <addr> <reg> <value>

uart:~$ i2c scan i2c@40003000
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:             -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- 1d -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- 39 -- -- -- -- -- --
40: -- -- -- 43 -- -- -- -- -- -- -- -- -- -- -- --
3 devices found on i2c@40003000
```

---

## 04_shell Sample - Sensor Shell

```shell {1-5|7-11}
uart:~$ sensor info
device name: apds9960@39, vendor: Avago Technologies, model: apds9960, ...
device name: mma8652fc@1d, vendor: NXP Semiconductors, model: fxos8700, ...
device name: ti_hdc@43, vendor: Texas Instruments, model: hdc, ...
device name: temp@4000c000, vendor: Nordic Semiconductor, model: nrf-temp, ...

uart:~$ sensor get ti_hdc@43
channel type=13(ambient_temp) index=0 shift=6 num_samples=1
  value=132211120605ns (26.427000)
channel type=16(humidity) index=0 shift=6 num_samples=1
  value=132211120605ns (36.364745)
```

---

## 05_sensor Sample

<div class="grid grid-cols-2 gap-4">

<div>

**Description:**
- TI HDC1010: I2C Temperature and Humidity Sensor<sup>1</sup>

**Learn:**
- Get Temperature and Humidity e.g. on the reel board via Sensor API

**Sample:**
- Demonstrates Sensor API

</div>

<div class="flex flex-col items-center justify-center">

```c
sensor_sample_fetch(dev);
sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP,
	           &temp);
sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY,
	           &humidity);

/* print the result */
printk("Temp = %d.%06d C, RH = %d.%06d %%\n",
       temp.val1, temp.val2,
       humidity.val1, humidity.val2);
```

<div class="text-xs text-center mt-2">samples/05_sensor/src/main.c</div>

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Equivalent in the Zephyr main Repository: zephyr/samples/sensor/ti_hdc.</Footnote>
</Footnotes>

---

## 05_sensor Sample - Console Output

```shell
*** Booting Zephyr OS build v4.1.0 ***
Running on arm!
Dev 0x801c name ti_hdc@43 is ready!
Fetching...
Temp = 22.852966 C, RH = 38.793945 %
Fetching...
Temp = 22.842895 C, RH = 38.897705 %
Fetching...
```

---

## 06_ble Sample

<div class="grid grid-cols-2 gap-4">

<div>

**Description:**
- BLE Peripheral device, temperature monitor<sup>1</sup>

**Learn:**
- BLE peripheral role and advertising
- Health Thermometer Service (HTS)

**Sample:**
- App to connect: nRF Connect for Mobile (Android, iOS)

</div>

<div class="flex flex-col items-center justify-center">

```ini
CONFIG_BT=y
CONFIG_LOG=y
CONFIG_BT_SMP=y
CONFIG_BT_PERIPHERAL=y
CONFIG_BT_DIS=y
CONFIG_BT_DIS_PNP=n
CONFIG_BT_BAS=y
CONFIG_BT_DEVICE_NAME="Zephyr Health Thermometer"
CONFIG_BT_DEVICE_APPEARANCE=768
CONFIG_CBPRINTF_FP_SUPPORT=y
CONFIG_SENSOR_SHELL=y
CONFIG_SENSOR_INFO=y
CONFIG_I2C_SHELL=y
```

<div class="text-xs text-center mt-2">samples/06_ble/prj.conf</div>

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Equivalent in the Zephyr main Repository: zephyr/samples/bluetooth/peripheral_ht.</Footnote>
</Footnotes>

---

## 06_ble Sample - Console Output

```shell
*** Booting Zephyr OS build v4.1.0 ***
[00:00:00.380,645] <inf> bt_hci_core: HW Platform: Nordic Semiconductor (0x0002)
[00:00:00.380,676] <inf> bt_hci_core: HW Variant: nRF52x (0x0002)
[00:00:00.380,706] <inf> bt_hci_core: Firmware: Standard Bluetooth controller ...
[00:00:00.381,347] <inf> bt_hci_core: Identity: D0:6F:6B:78:0C:E8 (random)
[00:00:00.381,378] <inf> bt_hci_core: HCI: version 5.4 (0x0d) revision 0x0000, ...
[00:00:00.381,408] <inf> bt_hci_core: LMP: version 5.4 (0x0d) subver 0xffff
Bluetooth initialized
temp device is 0x28b5c, name is temp@4000c000
Advertising successfully started
Connected
temperature is 24C
temperature is 23.75C
Indication success
Indication complete
```

---

## 06_ble Sample - Connect with a Smartphone

<div class="grid grid-cols-2 gap-8">

<div class="flex flex-col items-center">
  <img src="/images/nrf_connect_scan.png" class="h-72" />
  <div class="text-xs text-center mt-2">Scanning for BLE devices</div>
</div>

<div class="flex flex-col items-center">
  <img src="/images/nrf_connect_ble_connected.png" class="h-72" />
  <div class="text-xs text-center mt-2">Connected with nRF Connect App</div>
</div>

</div>

---
---

## 07_display_cfb Sample

<div class="grid grid-cols-2 gap-4">

<div>

**Control the passive Display** <br> <br>

- **Description:** Character Framebuffer Sample<sup>1</sup>
- **Sample:** Writes text to the display

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/reel_board_passive_display.jpg" class="h-60 object-contain" />
  <div class="text-xs text-center mt-2">Updated display on the reel board</div>
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Equivalent in the Zephyr main Repository: zephyr/samples/subsys/display/cfb.</Footnote>
</Footnotes>

---

## Hands-on 3

### Test the Samples

<div class="grid grid-cols-2 gap-4">

<div>

Build and run the samples<sup>1</sup>:

```shell
west build -b native_sim samples/02_logging -p
west build -t run
```

**or**

```shell
west build -b reel_board@2 samples/02_logging -p
west flash
```

**Task:** Update the displayed name on the passive display

</div>

<div class="flex flex-col items-center justify-center">

```text
samples
├── 01_hello_world
├── 02_logging
├── 03_workqueues
├── 04_shell
├── 05_sensor
├── 06_ble
└── 07_display_cfb
```

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Note: The sensor and ble samples require a board to run.</Footnote>
</Footnotes>
