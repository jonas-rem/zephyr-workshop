05 Sensor API
#############

**Link to Source:** `samples/05_sensor <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/05_sensor>`_

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

   host:~$ west build -b reel_board@2 samples/05_sensor -p
   host:~$ west flash

For other boards with external sensor:

.. code-block:: console

   host:~$ west build -b nucleo_l496zg samples/05_sensor -p
   host:~$ west flash

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
   Running on arm!
   Dev 0x801c name ti_hdc@43 is ready!
   Fetching...
   Temp = 22.852966 C, RH = 38.793945 %
   Fetching...
   Temp = 22.842895 C, RH = 38.897705 %
   Fetching...

Resources
*********

- `Zephyr Sensor API Documentation <https://docs.zephyrproject.org/latest/hardware/peripherals/sensor.html>`_
