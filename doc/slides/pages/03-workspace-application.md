---
layout: section
level: 1
---

# Workspace Application and Hardware Abstraction

---

## Workspace Application

<div class="grid grid-cols-2 gap-4">

<div>

**Workspace application** or **Out of tree** build<sup>1</sup>

Separate application from Zephyr repository

- Independent version control for app
- Different Licences for app?
- Maintain references to Zephyr, Modules etc. in app repo
- Update Zephyr version independently from app

</div>

<div class="flex flex-col items-center justify-center">

```text
zephyrproject
├── modules
│   └── hal
├── zephyr
│   ├── samples
│   ├── west.yml
└── zephyr-workshop
    ├── west.yml
    └── samples
        └── 01_hello_world
            ├── CMakeLists.txt
            ├── prj.conf
            ├── README.rst
            ├── sample.yaml
            └── src
```
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1><a href="https://docs.zephyrproject.org/latest/develop/application/index.html">docs.zephyrproject.org/latest/develop/application</a></Footnote>
  <Footnote :number=2><a href="https://github.com/zephyrproject-rtos/example-application">github.com/zephyrproject-rtos/example-application</a></Footnote>
  <Footnote :number=3><a href="https://github.com/jonas-rem/zephyr-workshop">github.com/jonas-rem/zephyr-workshop</a></Footnote>
</Footnotes>

---

## Enabled by _west_ and Manifest files

Manifests references repositories and modules<sup>1</sup>

```yaml {lineNumbers:true}
manifest:
  remotes:
    - name: zephyrproject-rtos
      url-base: https://github.com/zephyrproject-rtos

  projects:
    - name: zephyr
      remote: zephyrproject-rtos
      revision: v3.6.0
      import:
        name-allowlist:
          - cmsis
          - hal_nordic
          - hal_nxp
          - [..]
```
<div class="text-xs text-center mt-2">
	west.yaml manifest file in the zephyr-workshop repository
</div>

<Footnotes y="col">
  <Footnote :number=1><a href="https://docs.zephyrproject.org/latest/develop/west/manifest.html">docs.zephyrproject.org/latest/develop/west/manifest.html</a></Footnote>
</Footnotes>

---

## Understanding and Using _west_

<div class="grid grid-cols-2 gap-4">

<div>

`west` repository management tool, developed by the Zephyr community

- Inspired by Google's Repo tool and git submodules
- Cloning Zephyr repo, dependencies, modules
- Keeping project repositories synchronized
- In addition building, flashing, and debugging support

</div>

<div class="flex flex-col items-center justify-center">


```shell
# Navigate to the project root
$ cd zephyrproject

# Update all repositories
$ west update

=== updating zephyr (zephyr):
HEAD is now at 468eb56cf24 [..]
=== updating cmsis (modules/hal/cmsis):
HEAD is now at 4b96cbb [..]
=== updating hal_atmel (modules/hal/atmel):
HEAD is now at aad79bf [..]
```

</div>

</div>

---

## Application Structure - Use Case I

<div class="grid grid-cols-2 gap-4">

<div>

- **Who:** User creates one application for testing
- **What:** One variant of an application
- **Solution:** Make changes inside the Zephyr tree for simplicity<sup>1</sup>

</div>

<div class="h-full flex flex-col items-center justify-center">
  <img src="/images/application_topologies_1.svg" class="h-80 object-contain" />
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Not recommended for production, use an out-of-tree build
		instead. This makes it easier to upgrade to more recent Zephyr
		versions.</Footnote>
</Footnotes>

---

## Application Structure - Use Case II

<div class="grid grid-cols-2 gap-4">

<div>

<v-clicks>

- **Who:** One company developing one product
- **What:** Two variants of the product
- Different sensors, pin assignments, but similar application
- **Solution:** Devicetrees for each variant
  - board_a.dts: `west build -b board_a app`
  - board_b.dts: `west build -b board_b app`

</v-clicks>

</div>

<div class="h-full flex flex-col items-center justify-center">
  <img src="/images/application_topologies_2.svg" class="h-80 object-contain" />
</div>

</div>

---
---

## Application Structure - Use Case III

<div class="grid grid-cols-2 gap-4">

<div>

<v-clicks>

- **Who:** One company developing multiple products
- **What:** Different applications
- **Solution:** Separate applications
- Use same Zephyr version for all applications if you can

</v-clicks>

</div>

<div class="h-full flex flex-col items-center justify-center">
  <img src="/images/application_topologies_3.svg" class="h-80 object-contain" />
</div>

</div>

---
---

## Application Structure - Use Case IV

<div class="grid grid-cols-2 gap-4">

<div>

<v-clicks>

- **Who:** Service provider developing different products for multiple companies
- **What:** Development states and lifecycle for products differ
- **Solution:** Individual manifest files for each product
- Create your own modules, share code inbetween projects
- Quickly setup and reference projects with west

</v-clicks>

</div>

<div class="h-full flex flex-col items-center justify-center">
  <img src="/images/application_topologies_4.svg" class="h-80 object-contain" />
</div>

</div>

---

## Zephyr Hardware Abstraction

<div class="grid grid-cols-2 gap-4">

<div>

- **Vendor HALs:** Hardware abstraction available from vendors. Abstracted via Zephyr APIs and drivers
- **Devicetree:** Decouples the application from the hardware
- **Architecture:** ARM, RISC-V, x86, ARC, NIOS II, Tensilica, Xtensa
- **Other:** 600+ boards, 180+ sensors

</div>

<div class="flex flex-col items-center justify-center">

```console
zephyrproject/zephyr:~$ ls arch/
arc    CMakeLists.txt  mips   riscv  xtensa
arm    common          nios2  sparc
arm64  Kconfig         posix  x86

zephyrproject:~$ ls modules/hal/
altera        espressif   nordic      silabs
ambiq         ethos_u     nuvoton     st
atmel         gigadevice  nxp         stm32
cirrus-logic  infineon    openisa     telink
[..]

zephyrproject/zephyr:~$ ls boards/
96boards               firefly       native_sim
actinius               gd            rak
...
```

</div>

</div>

---
---

## Zephyr Hardware Abstraction - devicetree

<div class="grid grid-cols-2 gap-4">

<div>

Describes the available hardware

- Proven concept used in the Linux kernel
- Single source for hardware information
- Drivers and source are hardware independent

<div class="mt-4">

In Zephyr: C header generation at compile time

</div>

</div>

<div class="h-full flex flex-col items-center justify-center">
  <img src="/images/devicetree-logo.svg" class="h-35" />
  <div class="text-xs text-center mt-2">devicetree Logo</div>
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1><a>devicetree.org</a></Footnote>
</Footnotes>

---

## Zephyr Hardware Abstraction - devicetree

```c {1-8|9-15|16-20|}
arduino_i2c: &i2c0 {
        compatible = "nordic,nrf-twim";
        status = "okay";
        clock-frequency = <I2C_BITRATE_FAST>;
        pinctrl-0 = <&i2c0_default>;
        pinctrl-1 = <&i2c0_sleep>;
        pinctrl-names = "default", "sleep";

        mma8642fc: mma8652fc@1d {
                compatible = "nxp,fxos8700","nxp,mma8652fc";
                reg = <0x1d>;
                int1-gpios = <&gpio0 24 GPIO_ACTIVE_LOW>;
                int2-gpios = <&gpio0 25 GPIO_ACTIVE_LOW>;
        };

        ti_hdc@43 {
                compatible = "ti,hdc","ti,hdc1010";
                reg = <0x43>;
                drdy-gpios = <&gpio0 22 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;
        };
};
```

<div class="text-xs text-center mt-2">devicetree node for the reel board: `boards/phytec/reel_board/dts/reel_board.dtsi`</div>

---
---

## Hands-on 2: Let's build Zephyr for the reel board!

<div class="grid grid-cols-2 gap-4">

<div>


Build the `blinky` sample:

```shell
west build -b reel_board@2 \
  ../zephyr/samples/basic/blinky -p
```

**Flash via Drag and Drop:**
- Download the binary from your Codespace at: `build/zephyr/zephyr.hex`
- Drag and drop the hex file to the mounted reel board mass storage device

**Flash via native setup:**
```shell
west flash
```

**Attach to the serial console:**
```shell
minicom -D /dev/ttyACM0 -b 115200
```

</div>

<div class="flex flex-col items-center justify-center">
  <img src="/images/reel_board.jpg" class="h-60 object-contain" />
  <div class="text-xs text-center mt-2">reel board</div>
</div>

</div>
