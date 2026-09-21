# Copyright (c) 2026 Jonas Remmert
# SPDX-License-Identifier: Apache-2.0

from twister_harness import Shell

from test_sensor_shell import LED_OFF, LED_ON, output, output_with_logs


def test_emulated_button_triggers_sensor_read(shell: Shell):
    logs = output_with_logs(shell, "button emulate", drain=2.0)
    assert "Emulated button press started" in logs
    assert "Button pressed, published event" in logs
    assert LED_ON in logs
    assert LED_OFF in logs
    assert "Sensor state: SUCCESS" in "\n".join(output(shell, "sensor latest"))
