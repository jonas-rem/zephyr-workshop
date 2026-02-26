cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(application)

target_sources(app PRIVATE src/main.c)

# USB device initialization for tracing support
if(CONFIG_USB_DEVICE_STACK_NEXT)
  target_sources(app PRIVATE src/sample_usbd_init.c)
endif()

add_subdirectory(src/common common)
add_subdirectory(src/modules/led led)
add_subdirectory(src/modules/button button)
add_subdirectory(src/modules/sys_ctrl sys_ctrl)

# Button mock for native_sim (simulates GPIO button presses)
if(CONFIG_BOARD_NATIVE_SIM)
  add_subdirectory(src/modules/button_mock button_mock)
endif()
