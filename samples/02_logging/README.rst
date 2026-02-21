02 Logging
##########

**Link to Source:** `samples/02_logging <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/02_logging>`_

Overview
********

This sample demonstrates the use of Zephyr's logging subsystem. It shows how to
use different log levels and print various data types to the console.

This sample is useful for:

- Understanding log levels (ERR, WRN, INF, DBG)
- Setting log level via Kconfig
- Learning about logging backends
- Observing that DBG messages include function names

Key Features:

- Demonstrates printk() vs logging macros
- Shows different data types (integers, strings, pointers)
- Includes hexdump functionality

Building
********

This application can be built and executed on native_sim as follows:

.. code-block:: console

   host:~$ west build -b native_sim samples/02_logging -p
   host:~$ west build -t run

To build for a real board (e.g., reel_board@2), use:

.. code-block:: console

   host:~$ west build -b reel_board@2 samples/02_logging -p
   host:~$ west flash

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
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

Exit native_sim by pressing :kbd:`CTRL+C`.
