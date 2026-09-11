# SPDX-FileCopyrightText: Copyright (c) 2026 Navimatix GmbH
# SPDX-FileCopyrightText: Copyright (c) 2021-2025 TiaC Systems
# SPDX-FileCopyrightText: Copyright (c) 2023 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

import logging
import time

from twister_harness import Shell

logger = logging.getLogger(__name__)


def test_myboard_sensor(shell: Shell):
    logger.info('send "sensor get qmi8658a@6b" command')
    lines = shell.exec_command('sensor get qmi8658a@6b')
    assert any(['type=0(accel_x)' in line for line in lines]), 'expected response not found'
    assert any(['type=1(accel_y)' in line for line in lines]), 'expected response not found'
    assert any(['type=2(accel_z)' in line for line in lines]), 'expected response not found'
    assert any(['type=3(accel_xyz)' in line for line in lines]), 'expected response not found'
    assert any(['type=4(gyro_x)' in line for line in lines]), 'expected response not found'
    assert any(['type=5(gyro_y)' in line for line in lines]), 'expected response not found'
    assert any(['type=6(gyro_z)' in line for line in lines]), 'expected response not found'
    assert any(['type=7(gyro_xyz)' in line for line in lines]), 'expected response not found'
    assert any(['type=12(die_temp)' in line for line in lines]), 'expected response not found'
    logger.info('response is valid')


def test_myboard_led_strip(shell: Shell):
    logger.info('send "led_strip get_length led_strip" command')
    lines = shell.exec_command('led_strip get_length led_strip')
    assert any(['ws2812-8x8' in line for line in lines]), 'expected response not found'
    assert any(['has 64 pixel' in line for line in lines]), 'expected response not found'

    logger.info('send "led_strip fill led_strip 000000 65" command')
    lines = shell.exec_command('led_strip fill led_strip 000000 65')
    assert any(['Invalid number of pixels' in line for line in lines]), 'expected response not found'
    assert any(['65 (max 64)' in line for line in lines]), 'expected response not found'

    # BLACK
    logger.info('send "led_strip fill led_strip 000000 64" command')
    lines = shell.exec_command('led_strip fill led_strip 000000 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'

    # RED
    logger.info('send "led_strip fill led_strip 070000 64" command')
    lines = shell.exec_command('led_strip fill led_strip 070000 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'
    time.sleep(1)

    # YELLOW
    logger.info('send "led_strip fill led_strip 070700 64" command')
    lines = shell.exec_command('led_strip fill led_strip 070700 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'
    time.sleep(1)

    # GREEN
    logger.info('send "led_strip fill led_strip 000700 64" command')
    lines = shell.exec_command('led_strip fill led_strip 000700 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'
    time.sleep(1)

    # CYAN
    logger.info('send "led_strip fill led_strip 000707 64" command')
    lines = shell.exec_command('led_strip fill led_strip 000707 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'
    time.sleep(1)

    # BLUE
    logger.info('send "led_strip fill led_strip 000007 64" command')
    lines = shell.exec_command('led_strip fill led_strip 000007 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'
    time.sleep(1)

    # MAGENTA
    logger.info('send "led_strip fill led_strip 070007 64" command')
    lines = shell.exec_command('led_strip fill led_strip 070007 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'
    time.sleep(1)

    # WHITE
    logger.info('send "led_strip fill led_strip 070707 64" command')
    lines = shell.exec_command('led_strip fill led_strip 070707 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'
    time.sleep(1)

    # BLACK
    logger.info('send "led_strip fill led_strip 000000 64" command')
    lines = shell.exec_command('led_strip fill led_strip 000000 64')
    assert not any(['Invalid number of pixels' in line for line in lines]), 'expected empty response not found'

    logger.info('response is valid')
