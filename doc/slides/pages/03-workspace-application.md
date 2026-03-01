---
layout: section
level: 1
---

# Workspace Application and Hardware Abstraction

---

## Workspace Application & Topologies

<div class="grid grid-cols-2 gap-4">

<div>

**Workspace application** or **Out of tree** build<sup>1</sup>

Separate application from Zephyr repository

- Independent version control for app
- Different Licences for app?
- Maintain references to Zephyr, Modules etc. in app repo
- Update Zephyr version independently from app
- West supports T1, T2, and T3 topologies<sup>2</sup>

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
  <Footnote :number=2><a href="https://docs.zephyrproject.org/latest/develop/west/workspaces.html">docs.zephyrproject.org/latest/develop/west/workspaces</a></Footnote>
  <Footnote :number=3><a href="https://github.com/zephyrproject-rtos/example-application">github.com/zephyrproject-rtos/example-application</a></Footnote>
  <Footnote :number=4><a href="https://github.com/jonas-rem/zephyr-workshop">github.com/jonas-rem/zephyr-workshop</a></Footnote>
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
      revision: main
      import:
        name-allowlist:
          - cmsis
          - cmsis_6
          - mbedtls
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

## T1: Star Topology, Zephyr is the Manifest Repository

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
  <Footnote :number=1><a href="https://docs.zephyrproject.org/latest/develop/west/workspaces.html#t1-star-topology-zephyr-is-the-manifest-repository">Zephyr Docs: T1 Topology</a></Footnote>
</Footnotes>

---

## T2: Star Topology, Application is the Manifest Repository

<div class="grid grid-cols-2 gap-4">

<div>

- **Who:** One company developing one product
- **What:** Application-focused development with one or more board variants
- **Solution:** Application repository acts as the central manifest repository

</div>

<div class="h-full flex flex-col items-center justify-center">
  <img src="/images/application_topologies_2.svg" class="h-80 object-contain" />
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1><a href="https://docs.zephyrproject.org/latest/develop/west/workspaces.html#t2-star-topology-application-is-the-manifest-repository">Zephyr Docs: T2 Topology</a></Footnote>
</Footnotes>

---

## T2: Star Topology, Applied to multiple Projects

<div class="grid grid-cols-2 gap-4">

<div>

- **Who:** Service provider developing different products for multiple companies
- **What:** Development states and lifecycles for products differ significantly
- **Solution:** Same Star topology with dedicated manifest repository
- **Note:** Switching between projects with west update (via changed config)

</div>

<div class="h-full flex flex-col items-center justify-center">
  <img src="/images/application_topologies_4.svg" class="h-80 object-contain" />
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1><a href="https://docs.zephyrproject.org/latest/develop/west/workspaces.html#t2-star-topology-application-is-the-manifest-repository">Zephyr Docs: T2 Topology</a></Footnote>
</Footnotes>

---

## T3: Forest Topology

<div class="grid grid-cols-2 gap-4">

<div>

- **Who:** One company developing multiple independent products, or service providers
- **What:** Multiple applications at the same level, potentially with different lifecycles
- **Solution:** Dedicated manifest repository containing no source code

</div>

<div class="h-full flex flex-col items-center justify-center">
  <img src="/images/application_topologies_3.svg" class="h-80 object-contain" />
</div>

</div>

<Footnotes y="col">
  <Footnote :number=1><a href="https://docs.zephyrproject.org/latest/develop/west/workspaces.html#t3-forest-topology">Zephyr Docs: T3 Topology</a></Footnote>
</Footnotes>

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

```c
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
};
```

<div class="text-xs text-center mt-2">devicetree node for the reel board: `boards/phytec/reel_board/dts/reel_board.dtsi`</div>

---

## Zephyr Hardware Abstraction - Header generation at compile time

```bash
zephyr$ west build -b reel_board samples/subsys/shell/shell_module -p
[..]
-- Board: reel_board, Revision: 1, qualifiers: nrf52840
-- Found Dtc: /home/j.remmert/zephyr-sdk-0.17.4/sysroots/x86_64-pokysdk-linux/usr/bin/dtc (found
suitable version "1.7.0", minimum required is "1.4.6")
-- Found BOARD.dts: /home/j.remmert/git/zephyrproject/zephyr/boards/phytec/reel_board/reel_board.dts
-- Generated zephyr.dts: /home/j.remmert/git/zephyrproject/zephyr/build/zephyr/zephyr.dts
-- Generated pickled edt: /home/j.remmert/git/zephyrproject/zephyr/build/zephyr/edt.pickle
-- Generated devicetree_generated.h:
[..]/zephyr/build/zephyr/include/generated/zephyr/devicetree_generated.h
```
<div class="text-xs text-center mt-2">Build log shows how devicetree headers are generated at
    `build/zephyr/include/generated/zephyr/devicetree_generated.h`</div>

---

## Zephyr Hardware Abstraction - Generated header

```c
/* Generated header: devicetree_generated.h (excerpt) */

/* Line 15205: Alias used by application */
#define DT_N_ALIAS_accel0         DT_N_S_soc_S_i2c_40003000_S_mma8652fc_1d

/* Line 15206-15208: Instance macros & nodelabel */
#define DT_N_INST_0_nxp_fxos8700  DT_N_S_soc_S_i2c_40003000_S_mma8652fc_1d
#define DT_N_INST_0_nxp_mma8652fc DT_N_S_soc_S_i2c_40003000_S_mma8652fc_1d
#define DT_N_NODELABEL_mma8642fc  DT_N_S_soc_S_i2c_40003000_S_mma8652fc_1d

/* Line 15156: Full devicetree path */
#define DT_N_S_soc_S_i2c_40003000_S_mma8652fc_1d_PATH "/soc/i2c@40003000/mma8652fc@1d"
```

<div class="text-xs text-center mt-2">Generated identifiers in devicetree_generated.h</div>

---

## Zephyr Hardware Abstraction - Practical Usage in Code

```c
const struct device *const dev = DEVICE_DT_GET(DT_ALIAS(accel0));

if (!device_is_ready(dev)) {
    printf("Device %s is not ready\n", dev->name);
    return 0;
}
```

<div class="text-xs text-center mt-1">Application: `samples/sensor/accel_trig/src/main.c`</div>
<br>

```c
#define DT_DRV_COMPAT nxp_fxos8700

static const struct fxos8700_config fxos8700_config_##inst = {
    .bus_cfg.i2c = I2C_DT_SPEC_INST_GET(inst),
    .range = DT_INST_PROP(inst, range),
    ...
};

SENSOR_DEVICE_DT_INST_DEFINE(inst, ...);
DT_INST_FOREACH_STATUS_OKAY(FXOS8700_INIT)
```

<div class="text-xs text-center mt-1">Driver: `drivers/sensor/nxp/fxos8700/fxos8700.c`</div>

---

## Hands-on 2: Run Zephyr on native_sim!

<div class="grid grid-cols-5 gap-4">

<div class="col-span-3">

Build the __blinky__ sample for native_sim:

```shell
west build -b native_sim samples/basic/blinky -p
```

And run it:

```shell
west build -t run
```
<br>

Watch the LED state toggle in the console. Match the devicetree definition
(__led0__) with the sample code and generated header:

<div class="text-xxs">

```shell
boards/native/native_sim/native_sim.dts
samples/basic/blinky/src/main.c
build/zephyr/include/generated/zephyr/devicetree_generated.h
```

</div>

</div>

<div class="col-span-2 flex flex-col items-center justify-center">
  <img src="/images/native_sim_blinky.jpg" class="h-60 object-contain rounded-lg shadow-lg" />
</div>

</div>
