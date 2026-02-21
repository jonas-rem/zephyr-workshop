03 Workqueues
#############

**Link to Source:** `samples/03_workqueues <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/03_workqueues>`_

Overview
********

This sample demonstrates the use of workqueues and timers in Zephyr. The
application executes work items in different contexts to show how workqueues
operate.

This sample is useful for:

- Understanding workqueues and how they defer work to thread context
- Learning about different runtime contexts (ISR, Thread)
- Understanding thread priorities (cooperative vs preemptible)
- Using timers to schedule work items

Key Concepts:

- **Workqueues**: Queue of work items executed in a thread context
- **System workqueue**: Enabled by default, runs with priority -1
- **Thread priorities**: Negative values are cooperative (non-interruptible),
  positive values are preemptible (can be interrupted)

Building
********

This application can be built and executed on native_sim as follows:

.. code-block:: console

   host:~$ west build -b native_sim samples/03_workqueues -p
   host:~$ west build -t run

To build for a real board (e.g., reel_board@2), use:

.. code-block:: console

   host:~$ west build -b reel_board@2 samples/03_workqueues -p
   host:~$ west flash

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
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

Exit native_sim by pressing :kbd:`CTRL+C`.

Resources
*********

- `Zephyr Thread Priorities Documentation <https://docs.zephyrproject.org/latest/kernel/services/threads/index.html#thread-priorities>`_
