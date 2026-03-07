cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(application)

zephyr_include_directories(src/common)

target_sources(app PRIVATE src/main.c)

add_subdirectory(src/common common)

# Conditionally include components based on Kconfig options
if(CONFIG_LED_COMPONENT)
  add_subdirectory(src/components/led led)
endif()

if(CONFIG_BUTTON_COMPONENT)
  add_subdirectory(src/components/button button)

  # Button mock for native_sim (simulates GPIO button presses)
  if(CONFIG_BOARD_NATIVE_SIM)
    add_subdirectory(src/components/button_mock button_mock)
  endif()

endif()

if(CONFIG_SYS_CTRL_COMPONENT)
  add_subdirectory(src/components/sys_ctrl sys_ctrl)
endif()

if(CONFIG_TEMP_ALERT_COMPONENT)
  add_subdirectory(src/components/temp_alert temp_alert)
endif()

# USB tracing support for hardware boards
if(CONFIG_TRACING_USB_COMPONENT)
  add_subdirectory(src/components/tracing_usb tracing_usb)
endif()

if(CONFIG_TEMPSENSE_COMPONENT)
  add_subdirectory(src/components/tempsense tempsense)
endif()

if(CONFIG_SENSOR_LOG_COMPONENT)
  add_subdirectory(src/components/sensor_log sensor_log)
endif()
