Introduction
############

This chapter is the Simulation & Testing track reference. Commands are in
:doc:`simtest_cheatsheet`.

The session maps software-in-the-loop (SiL) test methods to Zephyr primitives.
Participants with testing experience can share and discuss strategies that work.

The workshop application ``samples/simulation`` runs component and E2E tests
on ``native_sim``.

The session covers the architecture for hardware-free testing, not the test
code itself. AI tools generate test suites and mocks. The architect needs to
know the Zephyr primitives (native_sim, Ztest, ZBus, driver emulators) to
choose the design and to instruct those tools.


Framing Questions
*****************

#. What percentage of our firmware validation can live entirely on a host PC,
   and what strictly requires HIL hardware?
#. How must firmware logic be structured so that components can be tested in
   isolation?
#. Where does pure software simulation fall short or break down in real-world
   production?


Different Emulation and Simulation Methods
******************************************

.. only:: html

   .. figure:: /_static/images/simulation_options_comparison.svg
      :align: center
      :width: 100%
      :alt: Comparison of simulation and hardware testing options by speed,
            hardware accuracy, and debugging visibility
      :target: https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options

      Comparison of Zephyr execution targets. Source: `Zephyr POSIX architecture
      documentation <https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options>`_.

.. only:: latex

   .. figure:: /_static/images/simulation_options_comparison.pdf
      :align: center
      :width: 100%
      :alt: Comparison of simulation and hardware testing options by speed,
            hardware accuracy, and debugging visibility
      :target: https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options

      Comparison of Zephyr execution targets. Source: `Zephyr POSIX architecture
      documentation <https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options>`_.


Whiteboard Templates
********************

Copy these onto the board. Participants add items. Leave empty cells empty.

What you already do
===================

::

    Practice                   |  Zephyr
  -----------------------------|------------------
    unit test                  |
    test runner                |
                               |
                               |
                               |
                               |

Host or hardware
================

::

    Lives on a host PC         |  Needs a board
  -----------------------------|------------------
                               |
                               |
                               |
                               |

native_sim extras
=================

Names on the left are given. Fill what each one stands in for.

::

    Tool                       |  Stands in for
  -----------------------------|------------------
    GPIO emulator              |
    sensor emulator            |
    UART / PTY                 |
    BabbleSim                  |
                               |

Testable or not
===============

The list starts empty. Add architecture choices, not tools.

::

    Architectural Enablers to support SiL
  ------------------------------------------

References
**********

- `Zephyr testing overview <https://docs.zephyrproject.org/latest/develop/test/index.html>`_
- `Ztest framework <https://docs.zephyrproject.org/latest/develop/test/ztest.html>`_
- `Twister test runner <https://docs.zephyrproject.org/latest/develop/twister/index.html>`_
- `Pytest harness <https://docs.zephyrproject.org/latest/develop/test/pytest.html>`_
- `Coverage reports <https://docs.zephyrproject.org/latest/develop/test/coverage.html>`_
- `native_sim <https://docs.zephyrproject.org/latest/boards/native/native_sim/doc/index.html>`_
- `Emulators <https://docs.zephyrproject.org/latest/hardware/emulator/index.html>`_
- `ZBus <https://docs.zephyrproject.org/latest/services/zbus/index.html>`_
- `Fake Function Framework <https://github.com/meekrosoft/fff>`_
- `BabbleSim <https://docs.zephyrproject.org/latest/develop/test/bsim.html>`_
- `Simulation options compared <https://docs.zephyrproject.org/latest/boards/native/doc/arch_soc.html#comparison-with-other-options>`_
- :doc:`testing`
