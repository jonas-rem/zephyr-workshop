01 Hello World
##############

**Link to Source:** `samples/01_hello_world <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/01_hello_world>`_

Overview
********

This is a simple sample that demonstrates the basic structure of a Zephyr
application. It prints "Hello World" along with the board name to the console.

This sample is useful for:

- Testing the build setup
- Understanding the structure of a Zephyr application
- Verifying the toolchain installation

Building
********

This application can be built and executed on native_sim as follows:

.. code-block:: console

   host:~$ west build -b native_sim samples/01_hello_world -p
   host:~$ west build -t run

To build for a real board (e.g., reel_board@2), use:

.. code-block:: console

   host:~$ west build -b reel_board@2 samples/01_hello_world -p
   host:~$ west flash

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
   Hello World! native_sim/native

Exit native_sim by pressing :kbd:`CTRL+C`.
