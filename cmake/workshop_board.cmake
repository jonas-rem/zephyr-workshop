# SPDX-License-Identifier: Apache-2.0
#
# Workshop-wide board configuration.
# Include this BEFORE find_package(Zephyr) in your CMakeLists.txt to
# automatically pick up shared board overlays and conf files from the
# repository root boards/ directory.

get_filename_component(WORKSHOP_ROOT ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)

# BOARD may include qualifiers (e.g. "native_sim/native"), strip them.
string(REPLACE "/" ";" _board_parts "${BOARD}")
list(GET _board_parts 0 _board_name)

set(_ws_overlay ${WORKSHOP_ROOT}/boards/${_board_name}.overlay)
set(_ws_conf ${WORKSHOP_ROOT}/boards/${_board_name}.conf)

if(EXISTS ${_ws_overlay})
  set(EXTRA_DTC_OVERLAY_FILE "${_ws_overlay}")
endif()

if(EXISTS ${_ws_conf})
  set(EXTRA_CONF_FILE "${_ws_conf}")
endif()
