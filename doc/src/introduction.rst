Introduction
============

Goals of the Workshop
---------------------

This Workshop shows methods that enable anyone to work with Zephyr efficiently.
Applying fundamental concepts like devicetree or KConfig can lead to a good
abstraction of complexity and superior maintainability.

The official `Zephyr Project Documentation
<https://docs.zephyrproject.org/latest/>`_ is extensive and e.g. already offers
a `Getting Started Guide
<https://docs.zephyrproject.org/latest/develop/getting_started/index.html>`_.
Topics that are covered in the official documentation will not be explained
here, but linked or summarized.

This repository hosts a reference architecture of a Zephyr application. This
workshop references it to explain the core principles of a modern and
maintainable firmware application.

How is Zephyr different?
------------------------

Zephyr is more than an RTOS, it is an infrastructure with many components such
as RF and networking stacks (TCP/IP, BLE ..), Shell-, Sensor- or Modem subsystem
and many more. It offers generic abstractions that enable to develop code
independently on the SoC and even CPU architecture. Traditional RTOS often lock
developers into vendor-specific ecosystems with proprietary APIs and limited
hardware support. Zephyr provides a unified, open-source foundation with
consistent APIs that enable code portability for multiple architectures and
reducing vendor lock-in.

Similarity to GNU/Linux
-----------------------

Linux is widely used in the Embedded industry. Linux unified this fragmented
landscape by providing a common kernel, standardized interfaces, and portable
abstractions across diverse SoCs and architectures. The key concepts that
resulted in this achievment have been adopted by Zephyr.

Key Concepts from the Linux Kernel adopted in Zephyr:

- Devicetree
- KConfig
- Unified driver model with standardized APIs

Other similarities:

- coding style
- Modular architecture

Those similarities make it easy to adopt Zephyr, if developers already have a
Linux background. However, if a project can afford to use Linux (Applications
processors with an MMU) it should. Eventhough Zephyr runs on Application
processors as well, the main focus area are SoCs with microcontrollers.
