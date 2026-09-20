---
layout: section
level: 1
---

# Code Examples and Subsystems

---

## Samples in Zephyr

**Samples**
- Zephyr provides a wide range of samples
- Samples are located in __zephyr/samples/__
- Isolated functionality or feature

**Tests**
- Tests are located in __zephyr/tests/__
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
├── tests.yaml
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
- `devicetree_generated.h` (devicetree header)
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
        [..]
        printk("Hello World! %s\n", CONFIG_BOARD);

        LOG_ERR("error string");
        [..]

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
[..]
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
*0x1243a0 shell_uart
	options: 0x0, priority: 14 timeout: 0
	state: queued, entry: 0x109399
	Total execution cycles: 20502688 (0 %)
	stack size 2048, unused 872, usage 1176 / 2048 (57 %)

 0x12dea0 sysworkq
	options: 0x1, priority: -1 timeout: 0
	state: pending, entry: 0x11204a
	Total execution cycles: 624459 (0 %)
	stack size 1024, unused 648, usage 376 / 1024 (36 %)
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
layout: center
class: text-center
---

## Questions?

<div class="text-sm text-gray-500 mt-12">

Slides and workshop material remain available at<br>
<a href="https://github.com/jonas-rem/zephyr-workshop">github.com/jonas-rem/zephyr-workshop</a>

</div>
