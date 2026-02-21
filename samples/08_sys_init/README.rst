08 System Initialization
########################

**Link to Source:** `samples/08_sys_init <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/08_sys_init>`_

Overview
********

This sample demonstrates the use of Zephyr's ``SYS_INIT()`` macro for
automatic module initialization during system startup.

The ``SYS_INIT()`` macro allows you to register functions that will be
called automatically during the kernel initialization sequence, before
``main()`` is executed. This is useful for:

- Hardware driver initialization
- Subsystem setup
- Early application configuration

Initialization Levels (in order of execution):

1. **EARLY** - Very early initialization
2. **KERNEL** - Kernel services initialization
3. **APPLICATION** - Application-level initialization (used in this sample)

Within each level, priority values determine the order (lower numbers run first).

This sample includes two modules:

- **Module 1**: Priority APPLICATION, level 0 (runs first)
- **Module 2**: Priority APPLICATION, level 999 (runs second)

Building
********

This application can be built and executed on native_sim as follows:

.. code-block:: console

   host:~$ west build -b native_sim samples/08_sys_init -p


Check Init Levels
*****************

The Initialization levels can be checked and validated after compilation. Zephyr
shows a list of registered functions for all init levels. The functions are
listed according to their execution order. The execution order is determined
according to the init level as well as the priority (0 - 999).

The ``main()`` function is called after all the init hooks from APPLICATION have
been executed. Therefore the execution order of the example should be:

1. module_1_init()
1. module_2_init()
3. main()

.. code-block:: console

   host:~$ west build -t initlevels
   -- west build: running target initlevels
   [2/3] cd
   /home/jonas/git/zephyrproject/zephyr-workshop/buil...roject/zephyr-workshop/build/zephyr/zephyr.exe
   --initlevels
   EARLY
   PRE_KERNEL_1
     __init_posix_arch_console_init: posix_arch_console_init(NULL)
   PRE_KERNEL_2
     __init_sys_clock_driver_init: sys_clock_driver_init(NULL)
   POST_KERNEL
   APPLICATION
     __init_module_1_init: module_1_init(NULL)
     __init_module_2_init: module_2_init(NULL)
   SMP

Sample Output
*************

.. code-block:: console

   host:~$ west build -t run

   *** Booting Zephyr OS build v4.3.0 ***
   Module 1 initialized (priority: APPLICATION, level: 0)
   Module 2 initialized (priority: APPLICATION, level: 999)
   Hello World! native_sim/native

Exit native_sim by pressing :kbd:`CTRL+C`.
