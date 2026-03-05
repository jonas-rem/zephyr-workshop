04 Shell
########

**Link to Source:** `samples/04_shell <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/04_shell>`_

Overview
********

This sample demonstrates the use of Zephyr's shell subsystem, providing a
command-line interface for interacting with the system.

This sample is useful for:

- Understanding the shell subsystem
- Learning about command history, completion, and help
- Hardware validation, testing, and debugging
- Exploring kernel and device information at runtime

Key Features:

- Interactive command-line interface
- Tab completion and command history
- Built-in commands for kernel, device, and sensor information
- Support for I2C scanning and sensor reading
- Custom command registration examples

Building
********

This sample uses ``qemu_x86`` because the shell subsystem requires a UART
backend for interactive use, which QEMU provides.

This application can be built and executed on QEMU x86 as follows:

.. code-block:: console

   host:~$ west build -b qemu_x86 samples/04_shell -p
   host:~$ west build -t run

To exit QEMU, press ``CTRL+a, x``.

To build for a real board (e.g., reel_board@2), use:

.. code-block:: console

   host:~$ west build -b reel_board@2 samples/04_shell -p
   host:~$ west flash

Sample Output
*************

After connecting to the shell, you can use various commands:

.. code-block:: console

   uart:~$
     bypass              clear               date
     demo                device              devmem
     dynamic             help                history
     kernel              log                 log_test
     rem                 resize              retval
     section_cmd         shell               shell_uart_release
     stats               version

List kernel threads:

.. code-block:: console

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

Exit QEMU by pressing ``CTRL+a, x``.
