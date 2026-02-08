Future Requirements to Software Development
===========================================

Developments in AI, specifically LLMs, Coding Assistants, Agentic workflows, etc.
are progressing very fast. Large amounts of code can be produced in a short
amount of time. The quality still largely depends on the person that operates
the tools that produce code.

One could argue that the critical path for the development of great products is
not code anymore (it maybe never was?). Let's assume for a moment that high
quality code can be produced for free in an instant. What becomes one of the
critical paths is the validation of the produced code that meets the
requirements. The reliable operation of an embedded device over years without
failure has to be proven. For most embedded devices a failure can have
significant consequences ranging from rendering a device inoperable to causing
injury to humans. Two things become critical.

1. A human has to understand at least the architecture of the code to be able to
   review a large codebase efficiently.
2. Code has to be well-covered by tests in different hierarchies
   (unit/integration/e2e). For a long time this practice is industry standard
   e.g. in web development. Achieving this in an embedded product is equally
   possible, but can be more difficult.

How Zephyr addresses complexity
-------------------------------

Coming from bare-metal, Zephyr is more complex and the developer has to understand
new concepts. However, this complexity is there for a reason. If teams
follow a few paradigms during development, powerful workflows are enabled that
lead to better, and more maintainable products in a shorter amount of time:

- loosely coupled modules that can run independently: `zbus`.
- Integration of the `shell` subsystem into each firmware module gives the
  possibility to trigger specific events and validate the result.
- With `native_sim` it is possible to run application code on the developer
  machine at much faster execution speeds. This enables unit-tests that AI tools
  can invoke to validate the code they write.
- Out of those modules, a subset of the final application can be configured to
  form integration tests that can run with `native_sim`.
- E2E tests are still required to validate the application generally runs on the
  target hardware in the target conditions. However, since the logic of the
  firmware has already been validated in simulation End 2 End tests can be light.
  E2E tests require the most amount of time and are the most complex to maintain.

This creates a very fast feedback loop for iteration. the Shell subsystem acts
an introspection tool into the code.

TODO / Ideas to expand
----------------------

- Specification-driven development: Human writes executable specs (Gherkin, contracts),
  agent implements to satisfy them. Specs are harder than coding but compound value.
  Can be iterative: sketch → implement → validate → refine.

- Architecture matters MORE with agents, not less. Good architecture enables:
  * Observable state for human validation (via shell/zbus)
  * Fast simulation cycles for agent iteration
  * Explicit behavior that serves as living specification
  * Linear progress instead of compounding technical debt

- Human becomes product validator and specification author, not code reviewer.
  Shift from "debugging opaque code" to "clarifying intent."

- Hybrid workflow: humans explore via shell/REPL, capture insights as specs,
  agents implement. Specs are the durable artifact, not the code.
