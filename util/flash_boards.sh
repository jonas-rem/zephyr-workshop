#!/bin/bash
#
# Check if an argument is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path_to_hex_file>"
    exit 1
fi

ZEPHYR_PROJECT_DIR="/home/jonas/git/zephyrproject"
ZEPHYR_SDK_DIR="/home/jonas/Downloads/zephyr-sdk-0.16.5-1"
SDK_SCRIPT_DIR="$ZEPHYR_SDK_DIR/sysroots/x86_64-pokysdk-linux/usr/share/openocd/scripts"
CONFIG_FILE="$BOARD_DIR/openocd.cfg"
HEX_FILE="$1"

echo "Select a board to flash:"
echo "1) reel_board"
echo "2) frdm_k64f"
echo "3) nucleo_wb55rg"
echo "4) nucleo_l496zg"
echo "5) mimxrt1010_evk"
echo "6) nucleo_l452re"
echo "7) nrf5340dk/nrf5340/cpuapp"
echo "8) nrf52833dk_nrf52833"
echo "9) atsamr21_xpro"

read -p "Enter your choice (number): " choice

case $choice in
    1)
        TOOL="pyocd"
        TARGET="nrf52840"
        ;;
    2)
        TOOL="pyocd"
        TARGET="k64f"
        ;;
    3)
        TOOL="pyocd"
        TARGET="stm32wb55rgvx"
        ;;
    4)
        TOOL="openocd"
        BOARD_DIR="$ZEPHYR_PROJECT_DIR/zephyr/boards/st/nucleo_l496zg/support"
	TARGET="stm32l496zg"
        ;;
    5)
        TOOL="pyocd"
        TARGET="mimxrt1010"
        ;;
    6)
        TOOL="openocd"
        BOARD_DIR="$ZEPHYR_PROJECT_DIR/zephyr/boards/st/nucleo_l452re/support"
	TARGET="stm32l454re"
        ;;
    7)
        TOOL="jlink"
        TARGET="NRF53"
        ;;
    8)
        TOOL="pyocd"
        TARGET="nrf52833"
        ;;
    9)
        TOOL="openocd"
        BOARD_DIR="$ZEPHYR_PROJECT_DIR/zephyr/boards/atmel/sam0/samr21_xpro/support"
	TARGET="at91samr21"
        ;;
    *)
        echo "Invalid selection. Exiting."
        exit 1
        ;;
esac

if [ "$TOOL" = "pyocd" ]; then
    pyocd flash -t $TARGET $HEX_FILE
elif [ "$TOOL" = "openocd" ]; then
    openocd -s $BOARD_DIR -s $SDK_SCRIPT_DIR \
            -f $CONFIG_FILE \
            '-c init' '-c targets' -c 'reset init' \
            -c "flash write_image erase $HEX_FILE" \
            -c 'reset run' -c shutdown
elif [ "$TOOL" = "jlink" ]; then
    nrfjprog --program $HEX_FILE --sectorerase --verify -f $TARGET --coprocessor CP_APPLICATION
    nrfjprog --pinreset -f NRF53
fi

# Check if the last command was successful
if [ $? -eq 0 ]; then
    echo "Successfully flashed $HEX_FILE to $TARGET using $TOOL"
else
    echo "Failed to flash $HEX_FILE to $TARGET using $TOOL."
    exit 1
fi
