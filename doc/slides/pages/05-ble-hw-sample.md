---
layout: section
level: 1
---

# Bluetooth Low Energy Demo

---

## nRF54L15 DK

<div class="grid grid-cols-2 gap-4">

<div>

Features
- nRF54L15 wireless SoC
- 2.4 GHz and NFC antennas
- J-Link programmer / debugger
- LEDs and buttons
- 8 MB external flash

</div>

<div class="flex flex-col items-center justify-center">
  <img src="../public/images/nrf54l15dk.jpg" class="h-70 object-contain" />
</div>

</div>

---

## Compiling

```shell
west build -b nrf54l15dk/nrf54l15/cpuapp samples/01_hello_world/ -p
```

---

## Flashing with `west`

```shell
west flash
```

In case of an error like 

```shell
ERROR: The operation attempted is unavailable due to readback protection in
ERROR: your device. Please use --recover to unlock the device.
```

The readback protection is enabeled. Disable it by recovering the device with:

```shell
west flash --recover
```

---

## Flashing with nRF Connect for Desktop (backup)

In case you use the Codespace, you can use Nordic's nRF Connect for Desktop application to access and flash your device.

- Download it from <a href="https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-desktop/download" about="_target">https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-desktop/download</a>

---

## nRF Connect

<div class="flex flex-col items-center justify-center">
  <img src="../public/images/nrf_connect_desktop.png" class="h-100 object-contain" />
Install and open the Programmer.
</div>

---

## nRF Connect - Programmer

<div class="flex flex-col items-center justify-center">
  <img src="../public/images/nrf_connect_desktop_programmer.png" class="h-100 object-contain" />
</div>

---

## Download the Binary

<div class="flex flex-col items-center justify-center">
  <img src="../public/images/codespace_download.png" class="h-100 object-contain" />
</div>

---

## GPIO - Buttons and LEDs

---
