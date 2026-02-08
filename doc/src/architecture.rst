Architecture
============

Patterns
--------

Software architecture can be described using patterns. Common patterns can
often be implemented in Zephyr applications using specific mechanisms and
subsystems.

The table below lists some patterns and their typical implementation methods in
Zephyr.

.. table:: Common Architectural Patterns
   :widths: auto

   +-----------------+-----------------------------------+----------------------------------------------+
   | Pattern         | Implementation Method             | Notes                                        |
   +=================+===================================+==============================================+
   | Feature Toggles | KConfig (compile time)            | En-/disable specific code                    |
   |                 |                                   | from the source tree                         |
   +-----------------+-----------------------------------+----------------------------------------------+
   | Strategy        | Devicetree                        | Switch between HW implementations via API    |
   +-----------------+-----------------------------------+----------------------------------------------+
   | Adapter         | Device Driver Model               | Abstract HW behind unified driver API        |
   +-----------------+-----------------------------------+----------------------------------------------+
   | Observer        | ZBus                              | Decouple modules via msg passing (Pub-/Sub)  |
   +-----------------+-----------------------------------+----------------------------------------------+
   | State           | SMF (State Machine Framework)     | Handle complex system states and transitions |
   +-----------------+-----------------------------------+----------------------------------------------+
   | Factory         | DEVICE_DEFINE / DT_INST_FOREACH_* | Auto-instantiate drivers from Devicetree     |
   +-----------------+-----------------------------------+----------------------------------------------+

.. note::

   Zephyr transforms traditional runtime design patterns into compile-time
   configurations, using the build system to "bake" logic into the binary for
   maximum speed and minimal memory footprint.
