#!/bin/bash

# Start the devcontainer in interactive mode
# Usage: ./scripts/devcontainer_start_interactive.sh
#
# Note: After entering the container, run these commands to complete setup:
#   west init -l zephyr-workshop
#   west update
#   west zephyr-export
#   west sdk install --toolchains arm-zephyr-eabi x86_64-zephyr-elf
#   pip install -r zephyr/scripts/requirements.txt --break-system-packages

# Build image if needed
docker build -t zephyr-workshop-dev .devcontainer/

# Run container interactively with workspace mounted
docker run -it --rm \
    -v "$(pwd):/zephyrproject/zephyr-workshop" \
    -w /zephyrproject/zephyr-workshop \
    zephyr-workshop-dev \
    bash
