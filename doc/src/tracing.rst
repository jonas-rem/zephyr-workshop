Instrumentation & Tracing
=========================

Zephyr provides two complementary subsystems for analyzing runtime behavior.
While both extract data from a running system, they operate at different
levels and answer different questions.

Comparison
----------

.. table:: Tracing vs Instrumentation
   :widths: auto

   +---------------+--------------------------------------+------------------------------------------+
   | Feature       | Tracing                              | Instrumentation                          |
   +===============+======================================+==========================================+
   | Core Question | *When* did it happen? (Sequence &    | *What functions executed?* (Call graph & |
   |               | Timing)                              | flow)                                    |
   +---------------+--------------------------------------+------------------------------------------+
   | Level         | RTOS-aware (high-level events)       | Compiler-level (all functions)           |
   +---------------+--------------------------------------+------------------------------------------+
   | Data Type     | Discrete Events (Context switch, IRQ | Function entry/exit events with          |
   |               | entry, Semaphore take)               | timestamps                               |
   +---------------+--------------------------------------+------------------------------------------+
   | Visual Output | Timeline / Gantt Chart (e.g.,        | Function call tree / Perfetto            |
   |               | TraceCompass, Perfetto)              |                                          |
   +---------------+--------------------------------------+------------------------------------------+
   | Manual Setup  | Minimal (Kconfig only)               | None (automatic compiler insertion)      |
   | Required      |                                      |                                          |
   +---------------+--------------------------------------+------------------------------------------+
   | Overhead      | Low to Medium                        | Higher (every function call)             |
   +---------------+--------------------------------------+------------------------------------------+

When to Use What
----------------

Use the **Tracing** subsystem (`subsys/tracing`_) when you need RTOS-aware event
tracing with structured event APIs. Best for tracking kernel events like thread
switches, semaphore operations, and IRQ handling with minimal overhead. Supports
backends like CTF, SysView, and Percepio Tracealyzer.

Use the **Instrumentation** subsystem (`subsys/instrumentation`_) when you need a
detailed view of function-level execution flow without adding manual trace
points. The compiler automatically instruments function entry/exit using GCC's
``-finstrument-functions``, making this ideal for capturing complete call graphs
and understanding code flow at the function level.

Tracing
-------

Tracing records the exact sequence of executable events as they occur on the
timeline. It preserves the temporal relationship between different parts of the
system.

**Useful for:**

* **Debugging Concurrency:** Visualizing race conditions, deadlocks, and
  priority inversions between threads.
* **Latency Analysis:** Measuring the exact duration an Interrupt Service
  Routine (ISR) blocks critical threads.
* **Flow Verification:** Confirming that the sequence of hardware interactions
  (e.g., "SPI transaction starts" -> "GPIO toggles") happens in the correct
  order.

Instrumentation
---------------

Instrumentation captures function entry and exit events automatically through
compiler instrumentation. Unlike tracing which focuses on RTOS events, instrument-
ation records virtually every function call without code changes.

**Useful for:**

* **Complete Call Graphs:** Reconstructing the full function call tree to
  understand complex execution flows.
* **Automated Analysis:** Capturing function traces without manually adding
  trace points throughout the codebase.
* **Detailed Flow Analysis:** Understanding the exact path taken through the
  code at the function level.

**Important Considerations:**

* Higher overhead than tracing (instruments every function)
* Requires GCC with ``-finstrument-functions``
* Increases code size and stack usage
* Use trigger/stopper functions to limit recording to specific code regions
* Exclude performance-critical functions via Kconfig to reduce overhead

References
----------

.. _subsys/tracing: https://docs.zephyrproject.org/latest/services/tracing/index.html
.. _subsys/instrumentation: https://docs.zephyrproject.org/latest/services/instrumentation/index.html

- `Tracing subsystem documentation <subsys/tracing_>`_
- `Instrumentation subsystem documentation <subsys/instrumentation_>`_
