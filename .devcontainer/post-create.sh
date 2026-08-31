#!/bin/bash
set -e

cd /workspaces

if [ ! -f .west/config ]; then
    west init -l zephyr-workshop
fi

west update --narrow -o=--depth=1
pip install -r zephyr/scripts/requirements.txt --break-system-packages
west zephyr-export

# make clangd file visible for the plugin
if [ ! -f .clangd ]; then
    ln -s zephyr-workshop/.devcontainer/.clangd
fi

# make the VS Code launch/debug configuration visible at the workspace root
if [ ! -e .vscode ]; then
    ln -s zephyr-workshop/.vscode
fi

west sdk install --gnu-toolchains arm-zephyr-eabi x86_64-zephyr-elf
