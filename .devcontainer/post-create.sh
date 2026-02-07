#!/bin/bash
set -e

sudo chown "$(id -u):$(id -g)" /zephyrproject
cd /zephyrproject
west init -l zephyr-workshop
west update
pip install -r zephyr/scripts/requirements.txt --break-system-packages
west zephyr-export
west sdk install --toolchains arm-zephyr-eabi x86_64-zephyr-elf
