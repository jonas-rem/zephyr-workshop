#!/bin/bash
set -e

cd /zephyrproject

if [ ! -f .west/config ]; then
    west init -l zephyr-workshop
fi

west update --narrow -o=--depth=1
pip install -r zephyr/scripts/requirements.txt --break-system-packages
west zephyr-export
west sdk install --toolchains arm-zephyr-eabi x86_64-zephyr-elf
