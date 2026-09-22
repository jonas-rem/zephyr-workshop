---
layout: section
level: 1
---

# IoT: From Sensor to Dashboard

---

## What We Are Building

<div class="grid grid-cols-2 gap-4">

<div>

- Everyone flashes **their own node**: it reads the on-board sensor, joins a
  Thread mesh and publishes its readings
- One **border router** on the trainer's laptop bridges the mesh to MQTT
- One **shared dashboard** for the room -- find your board by the name you
  gave it
- Only the node is a Zephyr application. Everything to the right of the
  border router is off-the-shelf

</div>

<div class="flex flex-col items-center justify-center">

```text
┌──────┐
│ node │──┐
└──────┘  │   CoAP/UDP    ┌────────┐
┌──────┐  │   ff03::1     │ border │
│ node │──┼─────────────▶ │ router │
└──────┘  │   802.15.4    └───┬────┘
┌──────┐  │   mesh            │
│ node │──┘                   ▼
└──────┘                  ┌────────┐
                          │ bridge │
 nodes relay for          └───┬────┘
 each other (FTD)             │ MQTT
                              ▼
                        ┌───────────┐
                        │ dashboard │
                        └───────────┘
```

<div class="text-xs text-center mt-2">One node per participant, one collector for the room</div>

</div>

</div>

---

## The Hardware

<div class="grid grid-cols-2 gap-4">

<div>

**TiaC CoffeeCaller (nRF52840)** -- your node

- SHT4x temperature and humidity sensor over I²C
- 4x WS2812 RGB LEDs, one button (SW0), one status LED (LD1)
- 802.15.4 radio, USB CDC-ACM console
- UF2 bootloader: drag and drop a file, no debugger needed

**nRF52840 DK** -- the trainer's border router

- Runs `ot-rcp`: a radio only, no Thread stack, no workshop code
- The Thread stack itself runs on the laptop

</div>

<div>

```shell
west build -b coffeecaller_nrf52/nrf52840 \
  samples/iot/thread_telemetry -p -- -DFILE_SUFFIX=mesh
# then drag build/zephyr/zephyr.uf2 onto the board
```

<br>

```text
samples/iot/thread_telemetry/
├── CMakeLists.txt
├── Kconfig
├── prj.conf          # skeleton, no stage
├── prj_{sensor,display,mesh}.conf  # the stages
├── boards/
└── src/
    ├── main.c
    ├── app_shell.c
    └── <one directory per module>
```

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Early prototype boards need the revision spelled out: <code>-b coffeecaller_nrf52@0.9.0/nrf52840</code>.</Footnote>
</Footnotes>

---

## Thread in One Slide

<div class="grid grid-cols-2 gap-4">

<div>

- **IEEE 802.15.4** radio, **6LoWPAN**, and then plain **IPv6 all the way to
  the node** -- every board has a real address and answers `ping`
- **Self-healing mesh:** nodes relay for each other, no single hop to a gateway
- **Roles are elected, not configured:** leader, router, child
- **FTD** routes for others; an **MTD/SED** sleeps instead -- what a
  battery-powered sensor would be
- A **border router** joins the mesh to the rest of the network

</div>

<div>

```console
uart:~$ telemetry mesh
Role      : child (attached)
Network   : 'ZephyrWorkshop', PAN 0x1234, channel 15
ML-EID    : fdde:ad00:beef:0:9c2b:1f7a:4e1d:8b03
RLOC16    : 0x4c01
Partition : 0x2f8a1c40, led by router 3
Parent    : 0x4c00, link quality in/out 3/3, RSSI -41 dBm
Neighbors : 0 children, 0 routers
```

<div class="text-xs text-center mt-2">The node's own view of the mesh</div>

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1><a href="https://openthread.io/guides/thread-primer">openthread.io/guides/thread-primer</a></Footnote>
</Footnotes>

---

## Why CoAP on the Node

<div class="grid grid-cols-2 gap-4">

<div>

**CoAP** ✓ — one UDP datagram per reading, nothing to keep open

**MQTT on the node** — one TCP session per board, over a lossy mesh

**MQTT-SN** — needs a gateway, and Zephyr's library is experimental

**Matter** — its own commissioning, data model and certificates

Translating to MQTT **once**, at the border router, keeps the node small.

</div>

<div>

**One reading, a non-confirmable POST:**

```json
{"node":"bench-window","temp_c":22.41,
 "hum_pct":47.83,"uptime_s":315}
```

**Republished by the bridge as:**

```text
workshop/telemetry/bench-window
```

<br>

Telemetry is periodic, so a lost sample costs less than the retransmissions an
acknowledged message would put on the mesh.

</div>

</div>

---

## Three Stages, Three Images

<div class="grid grid-cols-2 gap-4">

<div>

```text
Stage  The node is                     Flash
  -    skeleton: shell and sensor       ~92 KB
  1    sensor, console, name, shell    ~100 KB
  2    stage 1 plus the LED strip      ~110 KB
  3    the whole node, on the mesh     ~340 KB
```

A module that is switched off is **not compiled into the image at all**, so
the difference shows up in the memory report and not only in the behaviour.

Forget the suffix and you get the skeleton, not the finished node.

</div>

<div>

```shell
# Stage 1
west build -b coffeecaller_nrf52/nrf52840 \
  samples/iot/thread_telemetry -p -- -DFILE_SUFFIX=sensor

# Stage 2
west build -b coffeecaller_nrf52/nrf52840 \
  samples/iot/thread_telemetry -p -- -DFILE_SUFFIX=display

# Stage 3
west build -b coffeecaller_nrf52/nrf52840 \
  samples/iot/thread_telemetry -p -- -DFILE_SUFFIX=mesh
```

<br>

Each stage is a complete, self-contained `prj_<stage>.conf`, used *instead of*
`prj.conf` -- not a runtime flag.

</div>

</div>

---

## Getting the Build onto the Board

<div class="grid grid-cols-2 gap-4">

<div>

<div class="your-turn">
<div class="yt-head">Your turn <span class="yt-time">~10 min</span></div>

<div class="yt-step"><span class="yt-num">1</span> Build blinky for your board</div>

```shell
west build -b coffeecaller_nrf52/nrf52840 \
  zephyr/samples/basic/blinky -p
```

<div class="yt-step"><span class="yt-num">2</span> Download the <code>.uf2</code> from the file tree</div>

<div class="yt-step"><span class="yt-num">3</span> Double-tap RESET -- a USB drive appears</div>

<div class="yt-step"><span class="yt-num">4</span> Drag the file onto that drive</div>

<div class="yt-step"><span class="yt-num">5</span> LD1 blinks -- you flashed a board</div>

</div>

**There is no `west flash` here.** The build runs in the cloud and your board
is on your desk, so the file has to make that trip by hand -- once now, and
after every rebuild for the rest of the session.

</div>

<div>

<svg class="diagram" viewBox="0 0 880 380" role="img" aria-label="In Codespaces, right-click zephyr.uf2 in the file tree and choose Download">
  <rect class="dg-panel" x="1" y="1" width="568" height="330" rx="10" />
  <text class="dg-sub" x="28" y="44" style="letter-spacing:0.12em">EXPLORER</text>
  <path d="M 1 62 H 569" stroke="#e4eaec" stroke-width="2" />

  <text class="dg-row dg-mono" x="28" y="112">&#9662; zephyr-workshop</text>
  <text class="dg-row dg-mono" x="60" y="164">&#9662; build</text>
  <text class="dg-row dg-mono" x="92" y="216">&#9662; zephyr</text>

  <rect class="dg-hit" x="10" y="240" width="548" height="44" rx="6" />
  <text class="dg-row dg-mono" x="124" y="270">zephyr.uf2</text>

  <!-- the right-click menu -->
  <rect class="dg-panel" x="600" y="180" width="272" height="170" rx="8" />
  <text class="dg-row" x="626" y="224">Open</text>
  <rect class="dg-hit" x="610" y="250" width="252" height="44" rx="5" />
  <text class="dg-row" x="626" y="280">Download&#8230;</text>
  <text class="dg-row" x="626" y="332">Copy Path</text>

  <!-- pointer -->
  <path d="M 566 238 L 566 286 L 579 274 L 589 294 L 599 289 L 589 269 L 605 268 Z"
        fill="#23373b" stroke="#ffffff" stroke-width="2" />

  <text class="dg-note" x="1" y="366">In Codespaces: right-click what the build wrote.</text>
</svg>

<svg class="diagram" viewBox="0 0 880 366" role="img" aria-label="On your own machine, drag zephyr.uf2 from Downloads onto the USB drive that appeared">
  <rect class="dg-panel" x="1" y="1" width="878" height="322" rx="10" />
  <text class="dg-row" x="28" y="40">Downloads</text>
  <path d="M 1 58 H 879" stroke="#e4eaec" stroke-width="2" />
  <path d="M 268 58 V 323" stroke="#e4eaec" stroke-width="2" />

  <!-- sidebar: where the drive turns up once you double-tap RESET -->
  <text class="dg-row dg-muted" x="30" y="114">Home</text>
  <rect class="dg-sel" x="12" y="132" width="244" height="48" rx="6" />
  <text class="dg-row" x="30" y="164">Downloads</text>

  <rect class="dg-drop" x="12" y="198" width="244" height="56" rx="8" />
  <g class="dg-glyph">
    <rect x="30" y="216" width="28" height="18" rx="3" />
    <rect x="58" y="220" width="10" height="10" />
  </g>
  <text class="dg-row" x="78" y="233">USB drive</text>

  <!-- the file, and the drag -->
  <path class="dg-glyph" d="M 300 88 H 320 L 328 96 V 124 H 300 Z" />
  <text class="dg-row dg-mono" x="344" y="114">zephyr.uf2</text>

  <path class="dg-path" d="M 348 134 C 398 180 348 218 286 226" />
  <path class="dg-act-head" d="M 272 228 L 288 220 V 236 Z" />

  <!-- the file riding the cursor, over the drive -->
  <rect class="dg-ghost" x="238" y="258" width="200" height="46" rx="8" opacity="0.95" />
  <text class="dg-row dg-mono" x="338" y="288" text-anchor="middle">zephyr.uf2</text>
  <path d="M 226 222 L 226 270 L 239 258 L 249 278 L 259 273 L 249 253 L 265 252 Z"
        fill="#23373b" stroke="#ffffff" stroke-width="2" />

  <text class="dg-note" x="1" y="354">On your machine: drop it on the drive that appeared.</text>
</svg>

</div>

</div>

---

## Hands-on 5: The Sensor Node (Stage 1)

<div class="grid grid-cols-2 gap-4">

<div>

<div class="your-turn">
<div class="yt-head">Your turn <span class="yt-time">~10 min</span></div>

<div class="yt-step"><span class="yt-num">1</span> Build it</div>

```shell
west build -b coffeecaller_nrf52/nrf52840 \
  samples/iot/thread_telemetry -p \
  -- -DFILE_SUFFIX=sensor
```

<div class="yt-step"><span class="yt-num">2</span> Flash it the same way as blinky</div>

<div class="yt-step"><span class="yt-num">3</span> Open the console in your browser</div>

<div class="yt-link"><a href="https://terminal.vavrin.eu/">terminal.vavrin.eu</a> <span class="yt-hint">&rarr; Chrome or Edge</span></div>

```shell
uart:~$ telemetry name <your-name>
uart:~$ telemetry status
```

</div>

**Remember that name.** `telemetry --help` lists what this image can do.

</div>

<div>

```console
[00:00:05.012,000] <inf> sensor_console: 22.41 C, 47.83 %rH

uart:~$ telemetry status
Sensor    : 22.41 C, 47.83 %rH
Node name : bench-window
```

<br>

- The name is stored in **NVS**: it survives a reboot *and* a re-flash
- Without a name the node calls itself `node-a1b2c3`, derived from the device
  ID, so two boards never collide
- Only two status lines so far -- each one comes from the module it describes

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>The board is a USB device on <em>your</em> machine, not in the Codespace, so the console runs there. The baud rate does not matter on CDC-ACM.</Footnote>
</Footnotes>

---

## Hands-on 6: Make It Visible (Stage 2)

<div class="grid grid-cols-2 gap-4">

<div>

<div class="your-turn">
<div class="yt-head">Your turn <span class="yt-time">~10 min</span></div>

<div class="yt-step"><span class="yt-num">1</span> Rebuild with the display stage</div>

```shell
west build -b coffeecaller_nrf52/nrf52840 \
  samples/iot/thread_telemetry -p \
  -- -DFILE_SUFFIX=display
```

<div class="yt-step"><span class="yt-num">2</span> Flash it the same way as blinky</div>

<div class="yt-step"><span class="yt-num">3</span> Cup a hand over the board, watch the bar</div>

<div class="yt-step"><span class="yt-num">4</span> Press SW0 to switch to humidity</div>

```shell
uart:~$ telemetry display humidity
```

</div>

**Going further:** change the gradients at the top of `src/display/display.c`,
or narrow the range in `menuconfig`.

</div>

<div>

Four LEDs, each owning one quarter of the range. Count the lit ones for the
coarse value, read the colour of the last one for the rest:

```text
quarter  1:20-25C  2:25-30C  3:30-35C  4:35-40C

20 C     green      ·         ·         ·
24 C     orange     ·         ·         ·
25 C     red      green       ·         ·
33 C     red       red      orange      ·
38 C     red       red       red      orange
```

Humidity uses the same bar with a blue-to-purple gradient. The two palettes
never overlap, so a glance tells you which quantity is on display.

</div>

</div>

---

## Hands-on 7: Onto the Mesh (Stage 3)

<div class="grid grid-cols-2 gap-4">

<div>

<div class="your-turn">
<div class="yt-head">Your turn <span class="yt-time">~15 min</span></div>

<div class="yt-step"><span class="yt-num">1</span> Rebuild with the mesh stage</div>

```shell
west build -b coffeecaller_nrf52/nrf52840 \
  samples/iot/thread_telemetry -p \
  -- -DFILE_SUFFIX=mesh
```

<div class="yt-step"><span class="yt-num">2</span> Flash it, then wait for LD1</div>

<div class="yt-step"><span class="yt-num">3</span> Send one reading now</div>

```shell
uart:~$ telemetry publish
```

<div class="yt-step"><span class="yt-num">4</span> Find your name on the dashboard</div>

</div>

</div>

<div>

```console
uart:~$ telemetry status
Sensor    : 22.41 C, 47.83 %rH
Node name : bench-window
Display   : temperature
Mesh role : child
Collector : coap://[ff03::1]/telemetry
Publish   : every 15 s
```

<br>

The POST goes to `ff03::1`, the realm-local all-nodes multicast address, so a
roomful of **identically flashed boards needs no per-node configuration** at
all. The price is that every node hears every reading.

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Not attached? <code>ot state</code> says whether you are on the mesh, <code>telemetry mesh</code> and <code>ot router table</code> show who you relay through.</Footnote>
</Footnotes>

---

## Modularity at Application Scale

<div class="grid grid-cols-2 gap-4">

<div>

Every concern is one directory, with its implementation, header, `Kconfig` and
`CMakeLists.txt` next to each other:

```text
src/env_sensor/      read the SHT4x
src/sensor_console/  print it        (stage 1)
src/node_name/       the name, in NVS
src/display/         LED strip + SW0 (stage 2)
src/mesh/            the Thread role (stage 3)
src/telemetry/       format and POST (stage 3)
src/status_led/      LD1 follows attachment
```

```cmake
add_subdirectory(src/env_sensor)
add_subdirectory_ifdef(CONFIG_TELEMETRY_DISPLAY
                       src/display)
```

</div>

<div>

**Nothing is started from `main.c`.** Each module brings itself up:

```c
SYS_INIT(telemetry_init, APPLICATION,
         CONFIG_TELEMETRY_INIT_PRIORITY);
```

**And each is one entry in `menuconfig`**, holding its own options, log level
and init priority:

```text
Modules, in the order the workshop switches them on
    Environment sensor  --->
[*] Print the readings on the console  --->
[*] Participant-chosen node name  --->
[*] LED strip display  --->
[*] Thread mesh  --->
[*] CoAP publishing  --->
```

Dependencies live in Kconfig, so a module **disappears** when what it needs is
not in the build.

</div>

</div>

---

## The Shell, Assembled by the Linker

<div class="grid grid-cols-2 gap-4">

<div>

No implementation file contains shell code. The root exists once and knows
about nobody:

```c
/* src/app_shell.c */
SHELL_SUBCMD_SET_CREATE(telemetry_cmds, (telemetry));
SHELL_CMD_REGISTER(telemetry, &telemetry_cmds, ...);
```

Each module adds its own subcommand from its own file:

```c
/* src/display/display_shell.c */
SHELL_SUBCMD_ADD((telemetry), display, NULL,
                 "...", cmd_display, 1, 1);
```

</div>

<div>

`telemetry status` has the same problem one level up -- and keeps **no list**
of modules. Each contributes an entry to an iterable section:

```c
APP_STATUS_ENTRY_DEFINE(d_display, display_status);
```

```c
STRUCT_SECTION_FOREACH(app_status_entry, entry) {
        entry->print(sh);
}
```

<br>

The output shrinks with the build, and there is **no `#ifdef` anywhere in the
sources**. This is the same mechanism Zephyr itself uses for shell commands,
settings handlers and devices.

</div>

</div>

---

## The Other Half: The Trainer's Laptop

<div class="grid grid-cols-2 gap-4">

<div>

**Radio Co-Processor, not Network Co-Processor**

- The DK runs only the **radio and MAC**; the whole Thread stack runs on the
  host in `otbr-agent`
- The mesh therefore appears as an ordinary interface, `wpan0`
- So the collector is a **plain UDP socket**, and the laptop can `ping6` a node

The price: the network key lives on the laptop, and a reboot takes the border
router with it. Neither matters for a workshop.

</div>

```text
node ──CoAP/ff03::1──▶  nRF52840 DK     radio only
                          (ot-rcp)
                              │ Spinel / USB
                              ▼
                         otbr-agent     wpan0
                              │
                              ▼
                       coap-collector   CoAP → MQTT
                              │
                              ▼
 grafana ◀─ influxdb ◀─ telegraf ◀─ mosquitto
```

Everything from `otbr-agent` rightwards is `docker compose up -d`.

</div>

---

## Not a Production Configuration

<div class="grid grid-cols-2 gap-4">

<div>

**What we did, deliberately**

- The Thread **network key is hard-coded** in `prj_mesh.conf`, identical on
  every board and published in the repository
- The telemetry is **unauthenticated**: node names are self-declared and the
  collector cannot tell a genuine node from a spoofed one
- Anonymous MQTT broker, committed tokens, a privileged container

This removes commissioning from the exercise and the demo comes up in seconds.

</div>

<div>

**What a real deployment does instead**

- **Commission each device**: Thread's in-band joiner flow
  (`CONFIG_OPENTHREAD_JOINER` plus a per-device PSKd), or provision the
  operational dataset out of band
- The key is then unique per network and never leaves the commissioner
- Authenticate the telemetry itself -- CoAP over DTLS, or OSCORE
- A battery node would be an **MTD/SED**, not an FTD

</div>

</div>

<Footnotes y="col">
  <Footnote :number=1>Anyone in radio range who has read this repository can join the mesh, decrypt every frame and inject their own. Keep it on the bench.</Footnote>
</Footnotes>

---

## IoT Recap

<div class="grid grid-cols-2 gap-4">

<div>
<v-clicks>

**Networking**
- Thread gives you IPv6 to the node, and a mesh you do not configure
- Pick the protocol to fit the link: one datagram, not one session
- Translate once, at the border -- not on every node

**The node stays small**
- Stage 1 to stage 3: 100 KB to 340 KB, same source tree

</v-clicks>
</div>

<div>
<v-clicks>

**Structure**
- One directory, one Kconfig menu, one `SYS_INIT` per module
- Switched off means *not built*, not *built and idle*
- Shell and status are assembled by the linker

**Stuck? Ask the node first**
- `telemetry status`, `telemetry mesh`, `telemetry publish`

</v-clicks>
</div>

</div>
