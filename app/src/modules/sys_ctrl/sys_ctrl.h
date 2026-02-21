#ifndef _SYS_CTRL_H_
#define _SYS_CTRL_H_

#include <zephyr/zbus/zbus.h>
#include "message_channel.h"

/**
 * @brief Handle a button press event.
 */
void sys_ctrl_handle_button_press(void);

/**
 * @brief Get the current system state.
 *
 * @return Current system state.
 */
enum sys_states sys_ctrl_get_state(void);

/**
 * @brief Register shell commands for system controller.
 */
void sys_ctrl_register_shell_cmds(void);

#endif /* _SYS_CTRL_H_ */
