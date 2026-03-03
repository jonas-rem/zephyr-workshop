Testing Strategies
##################

Zephyr has an extensive testing infrastructure. The CI that tests every pull
request in the GitHub repository uses this test infrastructure to run build
tests and unit tests. The main framework for that is the Zephyr Test Framework
(ZTest_). Twister_ is a test runner that collects all tests and platforms,
composes them into test runs and generates reports after a completed test run.

The workshop shows how to test the `app`_ application with ZTest. ZTest can be
used to test on several levels (unit-, integration- and E2E-tests). The main
test platform for all those levels is ``native_sim``. Since there is no hardware
dependency and ``native_sim`` runs on any Linux Host it can easily be used to
create a CI pipeline.

For tests that must run on hardware, frameworks like ``pytest`` or ``Robot`` can
be configured to work with Twister_ and Zephyr's Shell Subsystem.

This workshop demonstrates **Component Testing** with ZTest, based on ZBus
messages. This allows testing components in an isolated fashion, and validates
their interface to other components (ZBus messages).

ZTest also supports E2E-testing in a simulated environment by utilizing
emulators. This workshop showcases emulators in sample `05_sensor`_ where a TI
HDC1010 Temperature sensor is emulated. ZTest can connect with those emulators
(button, led, sensor) and validate the behavior of the whole application.

All of those levels have their place and the ``native_sim`` environment in
Zephyr with its many tools like test frameworks, test runner, emulators and
simulators (BSim) can greatly accelerate product development.

Overview
********

Zephyr provides multiple testing frameworks to ensure application correctness
across different levels of abstraction:

**Integration Tests (Pytest_ + Twister_)**
  Test complete components running on ``native_sim`` through their public
  interfaces (shell commands, ZBus messages, log output). These verify that
  the component behaves correctly as a black box. The test configuration is
  defined in `sample.yaml`_.

**Unit Tests (ZTest_)**
  Test individual functions in isolation with mocked dependencies. These
  verify internal logic without requiring the full Zephyr runtime or hardware.

**When to use which?**

+------------------+--------------------------+------------------------+
| Aspect           | Integration Tests        | Unit Tests             |
+==================+==========================+========================+
| **Scope**        | Complete component       | Individual functions   |
+------------------+--------------------------+------------------------+
| **Dependencies** | Full Zephyr stack        | Minimal/mocked         |
+------------------+--------------------------+------------------------+
| **Execution**    | Runs on native_sim       | Runs on native_sim     |
+------------------+--------------------------+------------------------+
| **Best for**     | Hardware drivers,        | Algorithms, state      |
|                  | component behavior       | machines, utilities    |
+------------------+--------------------------+------------------------+
| **Example**      | `LED blinking patterns`_ | State transition logic |
+------------------+--------------------------+------------------------+

For comprehensive testing, use both approaches: unit tests for complex logic
and integration tests for component-level behavior.

Additional Resources
********************

**Zephyr Documentation:**

- Twister_
- ZTest_
- Pytest_
- `Test Configuration (sample.yaml) <https://docs.zephyrproject.org/latest/develop/test/twister.html#test-configuration>`_

**Related Workshop Content:**

- :doc:`Application README <../app/README>` - Integration testing details
- :doc:`slides` - Workshop slides
- `05_sensor`_ - Sensor emulator sample with E2E testing

**Source Files Referenced:**

- `sample.yaml`_ - Twister configuration
- `LED blinking patterns`_ - LED component under test

.. _Twister: https://docs.zephyrproject.org/latest/develop/test/twister.html
.. _ZTest: https://docs.zephyrproject.org/latest/develop/test/ztest.html
.. _Pytest: https://docs.zephyrproject.org/latest/develop/test/pytest.html
.. _app: https://github.com/jonas-rem/zephyr-workshop/tree/main/app

.. _sample.yaml: https://github.com/jonas-rem/zephyr-workshop/blob/main/app/sample.yaml
.. _LED blinking patterns: https://github.com/jonas-rem/zephyr-workshop/blob/main/app/src/components/led/led.c
.. _05_sensor: https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/05_sensor
