Testing Strategies
##################

Zephyr has an extensive testing infrastructure. The CI that tests every
Pull-Request in the GitHub repository uses this test infrastructure to run build
tests and unit-tests. The main framework for that is the Zephyr Test Framework
(ZTest). Twister is a test runner that collects all tests and platforms,
composes them into test runs and generates reports after a compoeted test run.

The workshop shows how to test the ``app`` application with ZTest. ZTest can be
used to test on several levels (unit-, integration- and E2E-tests). The main
test platform for all those levels is ``native_sim``. Since there is no hardware
dependency and ``native_sim`` runs on any Linux Host it can easily be used to
create a CI pipeline.

For test that must run on hardware, Frameworks like ``pytest`` or ``Robot`` can
be configured to work with Twister and Zephyrs Shell Subsystem.

This workshop demonstrates **Component Testing** with ZTest, based on ZBus
messages. This allows to test components in an isolated fashion, and validates
their interface to other components (ZBus messages).

ZTest also supports E2E-testing in a simulated environment by utilizing
emulators. This workshop showcases emulators in the sample ``05_sensor`` where a
TI HDC1010 Temperature sensor is emulated. ZTest can connect with those
emulators (button, led, sensor) and validate The behavior of the whole
application.

All of those levels have their place and the ``native_sim`` environment in
Zephyr with its many tools like test frameworks, test runner, emulators and
simulators (BSim) can greatly accelerate product development.

Overview
********

Zephyr provides multiple testing frameworks to ensure application correctness
across different levels of abstraction:

**Integration Tests (Pytest + Twister)**
  Test complete modules running on ``native_sim`` through their public
  interfaces (shell commands, ZBus messages, log output). These verify that
  the module behaves correctly as a black box.

**Unit Tests (Ztest)**
  Test individual functions in isolation with mocked dependencies. These
  verify internal logic without requiring the full Zephyr runtime or hardware.

**When to use which?**

+------------------+-----------------------+------------------------+
| Aspect           | Integration Tests     | Unit Tests             |
+==================+=======================+========================+
| **Scope**        | Complete module       | Individual functions   |
+------------------+-----------------------+------------------------+
| **Dependencies** | Full Zephyr stack     | Minimal/mocked         |
+------------------+-----------------------+------------------------+
| **Execution**    | Runs on native_sim    | Runs on native_sim     |
+------------------+-----------------------+------------------------+
| **Best for**     | Hardware drivers,     | Algorithms, state      |
|                  | module behavior       | machines, utilities    |
+------------------+-----------------------+------------------------+
| **Example**      | LED blinking patterns | State transition logic |
+------------------+-----------------------+------------------------+

For comprehensive testing, use both approaches: unit tests for complex logic
and integration tests for module-level behavior.

Additional Resources
********************

**Zephyr Documentation:**

- `Twister Test Runner <https://docs.zephyrproject.org/latest/develop/test/twister.html>`_
- `Ztest Framework <https://docs.zephyrproject.org/latest/develop/test/ztest.html>`_
- `Pytest Harness <https://docs.zephyrproject.org/latest/develop/test/pytest.html>`_
- `Test Configuration (sample.yaml) <https://docs.zephyrproject.org/latest/develop/test/twister.html#test-configuration>`_

**Related Workshop Content:**

- :ref:`Example Application README <example-app>` - Integration testing details
- :doc:`slides` - Workshop slides

**Source Files Referenced:**

*Integration Tests:*

- `app/pytest/test_led_module.py <https://github.com/jonas-rem/zephyr-workshop/blob/main/app/pytest/test_led_module.py>`_ - Pytest integration tests
- `app/sample.yaml <https://github.com/jonas-rem/zephyr-workshop/blob/main/app/sample.yaml>`_ - Twister configuration
- `app/src/modules/led/led.c <https://github.com/jonas-rem/zephyr-workshop/blob/main/app/src/modules/led/led.c>`_ - LED module under test

*Unit Tests:*

- `test/sys_ctrl/src/test_sys_ctrl.c <https://github.com/jonas-rem/zephyr-workshop/blob/main/test/sys_ctrl/src/test_sys_ctrl.c>`_ - Ztest unit tests
- `test/sys_ctrl/src/sys_ctrl.c <https://github.com/jonas-rem/zephyr-workshop/blob/main/test/sys_ctrl/src/sys_ctrl.c>`_ - Testable sys_ctrl implementation
- `test/sys_ctrl/testcase.yaml <https://github.com/jonas-rem/zephyr-workshop/blob/main/test/sys_ctrl/testcase.yaml>`_ - Unit test configuration
