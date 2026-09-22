05 Sensor API
#############

**Link to Source:** `samples/basics/05_sensor <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/basics/05_sensor>`_

Overview
********

This sample demonstrates the use of Zephyr's Sensor API to read temperature
and humidity from a TI HDC1010 (or compatible) sensor via I2C.

This sample is useful for:

- Understanding the Sensor API
- Learning how to fetch and read sensor data
- Working with I2C sensors

Requirements
************

This sample requires:

- A board with I2C support (e.g., reel_board@2, nucleo_l496zg)
- A TI HDC1010 or HDC1080 temperature/humidity sensor

Supported Boards:

- reel_board@2
- nucleo_l496zg (with external HDC sensor)

Building
********

For reel_board@2 (has onboard sensor):

.. code-block:: console

   host:~$ west build -b reel_board@2 samples/basics/05_sensor -p
   host:~$ west flash

For other boards with external sensor:

.. code-block:: console

   host:~$ west build -b nucleo_l496zg samples/basics/05_sensor -p
   host:~$ west flash

Changing Emulated Values
************************

On ``native_sim``, run the application with the shell on ``uart1``:

.. code-block:: console

   host:~$ west build -b native_sim samples/basics/05_sensor -p
   host:~$ ./build/zephyr/zephyr.exe -uart_1_attach_uart_cmd='ln -sf %s /tmp/zephyr_shell'

Connect to the shell from another terminal and set the sensor channels:

.. code-block:: console

   host:~$ screen /tmp/zephyr_shell
   uart:~$ sensor_emul set temp 25.5
   uart:~$ sensor_emul set humidity 62.5

The application reads these values through the TI HDC driver and Sensor API.
Setting a channel disables random values for that channel until restart.

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
   Running on native_sim!
   Dev 0x80525e0 name ti_hdc@43 is ready!
   Fetching...
   Temp = 4.059753 C, RH = 40.344238 %
   Fetching...
   Temp = 3.677062 C, RH = 45.675659 %
   Fetching...
   Temp = 4.165496 C, RH = 58.164978 %
   Fetching...

Resources
*********

- `Zephyr Sensor API Documentation <https://docs.zephyrproject.org/latest/hardware/peripherals/sensor.html>`_
