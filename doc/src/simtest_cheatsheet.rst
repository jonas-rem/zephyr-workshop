Cheat Sheet
###########

This page gives raw commands that execute the tests inside the Codespaces
environment. Run them from ``zephyr-workshop``.

Build and Run
*************

.. code-block:: console

   $ west build -b native_sim samples/simulation -p
   $ west build -t run
   uart connected to pseudotty: /dev/pts/4

``native_sim`` maps the shell to a pseudo terminal, logs stay in the terminal
running the app. The number changes between runs, so read it from the boot
output.

Attach from a second terminal, quit ``picocom`` with ``Ctrl+a Ctrl+x``:

.. code-block:: console

   $ picocom /dev/pts/4

The shell accepts these commands:

.. code-block:: console

   uart:~$ sensor read
   uart:~$ sensor latest
   uart:~$ sensor info
   uart:~$ button emulate
   uart:~$ sensor_emul set temp 30
   uart:~$ sensor_emul set humidity 50

Twister
*******

.. code-block:: console

   # All application tests
   $ west twister -T samples/simulation --integration

   # All component tests
   $ west twister -T samples/simulation/src/modules --integration

   # Component tests individually
   $ west twister -T samples/simulation/src/modules/button/tests --integration
   $ west twister -T samples/simulation/src/modules/led/tests --integration
   $ west twister -T samples/simulation/src/modules/sensor/tests --integration

   # End-to-end test on native_sim
   $ west twister -T samples/simulation -s app_simulation.e2e.native_sim --integration

   # End-to-end test on reel_board
   $ west twister -T samples/simulation -s app_simulation.e2e.hil -p reel_board --device-testing --device-serial /dev/ttyACM0 -X workshop_app

   # App boot test
   $ west twister -T samples/simulation -s app_simulation.basic --integration

   # List tests without running them
   $ west twister -T samples/simulation --integration --list-tests

   # Show failure logs inline
   $ west twister -T samples/simulation --integration --inline-logs

Artifacts
*********

.. code-block:: console

   $ less twister-out/twister_report.xml
   $ find twister-out -name handler.log -o -name build.log

Manual Test Execution
*********************

Manual test execution prints the test log directly to the console. After a code
change, ``west build -t run`` performs an incremental build and executes the
test.

.. code-block:: console

   # Button
   $ west build -b native_sim samples/simulation/src/modules/button/tests -p
   $ west build -t run

   # LED
   $ west build -b native_sim samples/simulation/src/modules/led/tests -p
   $ west build -t run

   # Sensor
   $ west build -b native_sim samples/simulation/src/modules/sensor/tests -p
   $ west build -t run
