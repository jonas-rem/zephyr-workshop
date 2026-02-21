---
layout: section
level: 1
---

# Development Setup

---
layout: figure
figureCaption: Zephyr Getting Started Guide
figureUrl: /images/Zephyr_getting_started.png
figureFootnoteNumber: 1
---

## Getting Started Guide

<Footnotes y="col">
  <Footnote :number=1><a href="https://docs.zephyrproject.org/latest/getting_started/index.html">docs.zephyrproject.org/latest/getting_started/index.html</a></Footnote>
</Footnotes>

---

## Setting up a Local Development Environment

Linux, macOS and Windows<sup>1</sup> are supported!

<div class="grid grid-cols-2 gap-4">

<div>

**Overview of Components**

Packages
- git
- Cmake
- Python ..

Zephyr SDK
- Toolchains for different architectures

Python Tools
- west, requirements.txt

</div>

<div>

IDE
- Text editor, IDE (e.g. VSCode)

Repositories
- Zephyr
- Modules via west

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Windows is supported, but will complicate development for Embedded Systems (e.g. no onboard package manager, often inconsistent Python environment setups).</Footnote>
</Footnotes>

---

## Testing the Local Development Environment

<div class="grid grid-cols-2 gap-4">

<div>

Build and flash the application with `west`

```shell
cd ~/zephyrproject/zephyr
west build -b reel_board@2 samples/basic/blinky -p
west flash
```

```shell
cd ~/zephyrproject/zephyr
west build -b native_sim samples/hello_world/ -p
west build -t run

*** Booting Zephyr OS build v4.1.0 ***
Hello World! native_sim/native
```

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/zephyr_blinky.png" class="h-60 object-contain" />
  <div class="text-xs text-center mt-2">reel board with blinking LED</div>
</div>

</div>

---

## Starting a Cloud Development Environment

<div class="grid grid-cols-2 gap-4">

<div>

GitHub Codespaces<sup>1</sup>

- Cloud hosted development environment based on devcontainers
- Visual Studio Code integration
- Runs on Microsoft Azure cloud
- Configuration individual for each repository

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/codespaces_how_to_start.png" class="h-40 object-contain" />
  <div class="text-xs text-center mt-2">Create a new Codespace</div>
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Open Source Alternatives: Gitpod, Eclipse Theia and many more</Footnote>
</Footnotes>

---

## Active Instance of GitHub Codespace

<div class="grid grid-cols-2 gap-4">

<div>

<div class="flex flex-col gap-4">
  <div>
    <img src="/images/codespaces_setting_up.png" class="h-50" />
    <div class="text-xs">Codespaces starting..</div>
  </div>
</div>

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/codespaces_open.png" class="h-100 object-contain" />
  <div class="text-xs text-center mt-2">Codespaces in a Browser Window</div>
</div>

</div>

---

## Recommendations from Experience

Virtual Machines in combination with embedded hardware can bring their own problems.

**Prioritize a local environment over a cloud environment**
- Hardware is better accessible
- Better integration of your own tools
- Check vendor tools that can enhance your Zephyr Dev Environment

---

## Hands-on 1 - Codespaces Setup

<div class="grid grid-cols-2 gap-4">

<div>

Start your own Codespaces Instance now!

[github.com/jonas-rem/zephyr-workshop](https://github.com/jonas-rem/zephyr-workshop)

Setup will take a few minutes..

**Test your setup with the Hello World example:**

```shell
west build -b native_sim zephyr/samples/hello_world -p
west build -t run
```

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/codespaces_setting_up_class.png" class="h-60 object-contain" />
  <div class="text-xs text-center mt-2">Setup new Instance</div>
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1><strong>Recommendation:</strong> Use a 4-core setup instead of the 2-core default.</Footnote>
  <Footnote :number=2><strong>Note:</strong> You should have 120 core-hours per month free.</Footnote>
</Footnotes>
