Testing
#######

Zephyr has an extensive testing infrastructure. The CI that tests every pull
request in the GitHub repository uses this test infrastructure to run build
tests and unit tests. The main framework for that is the Zephyr Test Framework
(Ztest_). Twister_ is a test runner that collects all tests and platforms,
composes them into test runs and generates reports after a completed test run.

The workshop shows how to test the `app`_ application with Ztest. Ztest can be
used to test on several levels (unit-, integration- and E2E-tests). The main
test platform for all those levels is ``native_sim``. Since there is no hardware
dependency and ``native_sim`` runs on any Linux host it can easily be used to
create a CI pipeline.

For tests that must run on hardware, frameworks like ``pytest`` or ``Robot`` can
be configured to work with Twister_ and Zephyr's Shell Subsystem.

This workshop demonstrates **Component Testing** with Ztest, based on zbus
messages. This allows testing components in an isolated fashion, and validates
their interface to other components (zbus messages).

Ztest also supports E2E-testing in a simulated environment by utilizing
emulators. This workshop showcases emulators in sample `05_sensor`_ where a TI
HDC1010 Temperature sensor is emulated. Ztest can connect with those emulators
(button, led, sensor) and validate the behavior of the whole application.

All of those levels have their place and the ``native_sim`` environment in
Zephyr with its many tools like test frameworks, test runner, emulators and
simulators (BSim) can greatly accelerate product development.

Additional Resources
********************

**Zephyr Documentation:**

- Twister_
- Ztest_
- Pytest_

**Links to source code from the zephyr-workshop repo**

- :doc:`Application README <../samples/simulation/README>` - Integration testing details
- `05_sensor`_ - Sensor emulator sample with E2E testing

.. _Twister: https://docs.zephyrproject.org/latest/develop/test/twister.html
.. _Ztest: https://docs.zephyrproject.org/latest/develop/test/ztest.html
.. _Pytest: https://docs.zephyrproject.org/latest/develop/test/pytest.html
.. _app: https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/simulation

.. _LED blinking patterns: https://github.com/jonas-rem/zephyr-workshop/blob/main/samples/simulation/src/components/led/led.c
.. _05_sensor: https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/basics/05_sensor
