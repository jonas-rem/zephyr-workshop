/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

/* Test observer for led_ch messages */
static enum sys_states last_led_state;
static int led_msg_count;

static void led_ch_observer_cb(const struct zbus_channel *chan)
{
	const enum sys_states *msg = zbus_chan_const_msg(chan);
	last_led_state = *msg;
	led_msg_count++;
}

ZBUS_LISTENER_DEFINE(test_led_listener, led_ch_observer_cb);
ZBUS_CHAN_ADD_OBS(led_ch, test_led_listener, DEFAULT_OBS_PRIO);

/* External function from sys_ctrl component */
extern enum sys_states sys_ctrl_get_state(void);

static void *sys_ctrl_test_setup(void)
{
	/* Wait for SYS_INIT to complete */
	k_sleep(K_MSEC(50));
	return NULL;
}

ZTEST(sys_ctrl_test_suite, test_initial_state)
{
	enum sys_states state = sys_ctrl_get_state();
	
	/* State should be either STANDBY or SLEEP (valid states) */
	zassert_true(state == SYS_STANDBY || state == SYS_SLEEP,
		      "Initial state should be valid (STANDBY or SLEEP), got %d", state);
	
	/* Should have received initial state message during init */
	zassert_true(led_msg_count >= 1,
		      "Should have published initial state (count: %d)", led_msg_count);
}

ZTEST(sys_ctrl_test_suite, test_button_press_standby_to_sleep)
{
	enum sys_events btn_msg = SYS_BUTTON_PRESSED;
	int count_before = led_msg_count;
	enum sys_states initial_state = sys_ctrl_get_state();

	/* First press: toggles state */
	zbus_chan_pub(&button_ch, &btn_msg, K_NO_WAIT);
	k_yield();

	/* Should have published a message */
	zassert_equal(led_msg_count, count_before + 1,
		      "Should publish message");
	
	/* State should have toggled */
	if (initial_state == SYS_STANDBY) {
		zassert_equal(sys_ctrl_get_state(), SYS_SLEEP,
			      "State should toggle to SLEEP");
		zassert_equal(last_led_state, SYS_SLEEP,
			      "LED channel should receive SLEEP");
	} else {
		zassert_equal(sys_ctrl_get_state(), SYS_STANDBY,
			      "State should toggle to STANDBY");
		zassert_equal(last_led_state, SYS_STANDBY,
			      "LED channel should receive STANDBY");
	}
}

ZTEST(sys_ctrl_test_suite, test_button_press_sleep_to_standby)
{
	enum sys_events btn_msg = SYS_BUTTON_PRESSED;
	int count_before = led_msg_count;
	enum sys_states state_before = sys_ctrl_get_state();

	/* Press: toggles state */
	zbus_chan_pub(&button_ch, &btn_msg, K_NO_WAIT);
	k_yield();

	enum sys_states state_after = sys_ctrl_get_state();

	/* State should have toggled */
	zassert_not_equal(state_before, state_after,
			  "State should change after button press");
	zassert_equal(led_msg_count, count_before + 1,
		      "Should publish 1 message");
	zassert_equal(last_led_state, state_after,
		      "LED channel should receive new state");
}

ZTEST(sys_ctrl_test_suite, test_rapid_button_presses)
{
	enum sys_events btn_msg = SYS_BUTTON_PRESSED;
	int count_before = led_msg_count;
	enum sys_states initial_state = sys_ctrl_get_state();

	/* Multiple rapid button presses */
	for (int i = 0; i < 10; i++) {
		zbus_chan_pub(&button_ch, &btn_msg, K_NO_WAIT);
		k_yield();
	}

	/* Should have 10 more messages */
	zassert_equal(led_msg_count, count_before + 10,
		      "All 10 button presses should generate messages");
	
	/* After 10 toggles, should be back to initial state */
	zassert_equal(sys_ctrl_get_state(), initial_state,
		      "State should return to initial after even toggles");
}

ZTEST(sys_ctrl_test_suite, test_state_toggle_consistency)
{
	enum sys_events btn_msg = SYS_BUTTON_PRESSED;
	enum sys_states prev_state = sys_ctrl_get_state();

	/* Toggle multiple times and verify consistency */
	for (int i = 0; i < 6; i++) {
		zbus_chan_pub(&button_ch, &btn_msg, K_NO_WAIT);
		k_yield();

		enum sys_states current_state = sys_ctrl_get_state();
		
		/* State should alternate */
		zassert_not_equal(current_state, prev_state,
				  "State should alternate on each press");
			
		prev_state = current_state;
	}
}

ZTEST_SUITE(sys_ctrl_test_suite, NULL, sys_ctrl_test_setup, NULL, NULL, NULL);
