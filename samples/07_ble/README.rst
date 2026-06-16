06 BLE
######

**Link to Source:** `samples/06_ble <https://github.com/jonas-rem/zephyr-workshop/tree/main/samples/06_ble>`_

Overview
********

This sample demonstrates a Bluetooth Low Energy (BLE) peripheral device that
exposes the Health Thermometer Service (HTS). It simulates temperature
measurements and sends them to connected devices.

This sample is useful for:

- Understanding BLE peripheral role and advertising
- Learning about GATT services (Health Thermometer Service)
- Working with BLE connection callbacks
- Simulating sensor data over BLE

Key Features:

- BLE peripheral device with advertising
- Health Thermometer Service (HTS) for temperature data
- Battery Service (BAS) for battery level simulation
- Device Information Service (DIS)
- Support for nRF Connect for Mobile app (Android/iOS)

Requirements
************

- A board with Bluetooth LE support (e.g., reel_board@2, nrf52dk_nrf52832)
- nRF Connect for Mobile app (Android or iOS)

Supported Boards:

- reel_board@2
- nrf52dk_nrf52832

Building
********

.. code-block:: console

   host:~$ west build -b reel_board@2 samples/06_ble -p
   host:~$ west flash

Or for nRF52 DK:

.. code-block:: console

   host:~$ west build -b nrf52dk_nrf52832 samples/06_ble -p
   host:~$ west flash

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build v4.3.0 ***
   [00:00:00.380,645] <inf> bt_hci_core: HW Platform: Nordic Semiconductor (0x0002)
   [00:00:00.380,676] <inf> bt_hci_core: HW Variant: nRF52x (0x0002)
   [00:00:00.380,706] <inf> bt_hci_core: Firmware: Standard Bluetooth controller ...
   [00:00:00.381,347] <inf> bt_hci_core: Identity: D0:6F:6B:78:0C:E8 (random)
   [00:00:00.381,378] <inf> bt_hci_core: HCI: version 5.4 (0x0d) revision 0x0000, ...
   [00:00:00.381,408] <inf> bt_hci_core: LMP: version 5.4 (0x0d) subver 0xffff
   Bluetooth initialized
   temp device is 0x28b5c, name is temp@4000c000
   Advertising successfully started
   Connected
   temperature is 24C
   temperature is 23.75C
   Indication success
   Indication complete

Connecting with a Smartphone
****************************

1. Install nRF Connect for Mobile app (Android/iOS)
2. Scan for BLE devices
3. Look for "Zephyr Health Thermometer"
4. Connect and view temperature notifications

Resources
*********

- `Zephyr Bluetooth Stack Documentation <https://docs.zephyrproject.org/latest/connectivity/bluetooth/index.html>`_
- `nRF Connect for Mobile <https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile>`_
