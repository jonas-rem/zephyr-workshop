---
layout: section
level: 1
hideInToc: true
---

# Backup Slides

---
layout: default
---

## Debugging: git bisect

<div class="grid grid-cols-2 gap-4">

<div>

- Something worked in the past, but does not work anymore
- **Example:** The lvgl sample worked on the reel_board in v4.0.0, but not with the current main (7fc9c26fb0d)

</div>

<div>

```shell {1-3|4-8|10-11|12-19}
git bisect start
git bisect good v4.0.0
git bisect bad 7fc9c26fb0d
west update && \
west build -b reel_board@2 samples/subsys/display/lvgl/ -p && \
west flash
# -> After flashing, test console and display to see if the sample works
git bisect good|bad

  ... (repeat binary search for log2(N) steps)
  # log2(10000) ≈ 13.3 -> max 14 steps for 10k commits!

git bisect good
1ea35e is the first bad commit
commit 1ea35e237cc0aa26b8a3517144528e9781a56781
Author: ***
Date:   Thu Jan 25 11:09:06 2024 +0100

    west.yml: Update lvgl module to v9.2.0
```

<p class="text-xs text-center mt-2">Find the commit that caused a bug with <i>git bisect</i></p>

</div>

</div>
