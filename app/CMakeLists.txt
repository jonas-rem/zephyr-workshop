cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(application)

zephyr_include_directories(src/common)

target_sources(app PRIVATE src/main.c)

add_subdirectory(src/common common)

# Conditionally include modules based on Kconfig options
if(CONFIG_LED_MODULE)
  add_subdirectory(src/modules/led led)
endif()

if(CONFIG_BUTTON_MODULE)
  add_subdirectory(src/modules/button button)

  # Button mock for native_sim (simulates GPIO button presses)
  if(CONFIG_BOARD_NATIVE_SIM)
    add_subdirectory(src/modules/button_mock button_mock)
  endif()

endif()

if(CONFIG_SYS_CTRL_MODULE)
  add_subdirectory(src/modules/sys_ctrl sys_ctrl)
endif()

# USB tracing support for hardware boards
if(CONFIG_TRACING_USB_MODULE)
  add_subdirectory(src/modules/tracing_usb tracing_usb)
endif()
