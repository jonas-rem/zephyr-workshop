/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_STATUS_H
#define APP_STATUS_H

#include <zephyr/shell/shell.h>
#include <zephyr/sys/iterable_sections.h>

/**
 * @brief One line of the "telemetry status" output.
 *
 * The command keeps no list of modules to ask. The linker collects one of
 * these from every module that was built into the image, and the command
 * walks whatever it finds -- so a module that is switched off simply has no
 * line, with no conditional compilation anywhere.
 */
struct app_status_entry {
	/** Print this module's line, with @ref APP_STATUS_PRINT. */
	void (*print)(const struct shell *sh);
};

/**
 * @brief Contribute one line to "telemetry status".
 *
 * @param _name Unique name for the entry. The lines come out in alphabetical
 *              order of this name, which is why every module prefixes it with
 *              a letter.
 * @param _fn   Function printing the line.
 */
#define APP_STATUS_ENTRY_DEFINE(_name, _fn)                                                        \
	static const STRUCT_SECTION_ITERABLE(app_status_entry, _CONCAT(app_status_, _name)) = {     \
		.print = (_fn),                                                                    \
	}

/**
 * @brief Print one "<label> : <value>" line, aligned across all modules.
 *
 * Keep the label to nine characters, which is what the columns are padded to.
 */
#define APP_STATUS_PRINT(_sh, _label, _fmt, ...)                                                   \
	shell_print(_sh, "%-9s : " _fmt, _label, ##__VA_ARGS__)

#endif /* APP_STATUS_H */
