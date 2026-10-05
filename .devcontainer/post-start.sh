#!/bin/bash
# Runs on every container start. A failed step only warns, so the container
# still starts without network access.

cd /workspaces

warn() {
    echo "post-start: $1 failed, continuing" >&2
}

if git -C zephyr-workshop rev-parse --abbrev-ref --symbolic-full-name '@{u}' >/dev/null 2>&1; then
    git -C zephyr-workshop pull --ff-only || warn "workshop update"
fi

# Revert the patches first, so west update can move Zephyr to the latest main.
if [ -f zephyr-workshop/zephyr/patches.yml ]; then
    west patch clean || warn "west patch clean"
fi

west update --narrow -o=--depth=1 || warn "west update"
pip install -q -r zephyr/scripts/requirements.txt --break-system-packages || warn "pip install"

if [ -f zephyr-workshop/zephyr/patches.yml ]; then
    west patch apply || warn "west patch apply"
fi
