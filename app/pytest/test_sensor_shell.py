# Copyright (c) 2026 Jonas Remmert
# SPDX-License-Identifier: Apache-2.0

from twister_harness import Shell

# The LED module has no shell command: it is observed through its log output.
LED_ON = "LED ON"
LED_OFF = "LED OFF"


def output(shell: Shell, command: str) -> list[str]:
    """Command output only, with log lines filtered out."""
    return shell.get_filtered_output(shell.exec_command(command))


def output_with_logs(shell: Shell, command: str, drain: float = 1.0) -> str:
    """Command output plus the log lines that arrive after the shell prompt."""
    return "\n".join(
        shell.exec_command(command, get_full_output=True, full_output_timeout=drain)
    )


def test_sensor_read_updates_latest_sample_and_led(shell: Shell):
    logs = output_with_logs(shell, "button emulate", drain=2.0)

    assert "Emulated button press started" in logs
    assert "Button pressed, published event" in logs

    # The button event switches the LED on; the sensor result switches it off.
    assert LED_ON in logs
    assert LED_OFF in logs

    lines = output(shell, "sensor latest")
    joined = "\n".join(lines)
    assert "Sensor state: SUCCESS" in joined
    assert any(" C" in line for line in lines)
    assert any(" %" in line for line in lines)
