#!/bin/bash
set -e

# remoteEnv sets ZEPHYR_BASE to the baked tree. Unset it so west init
# uses the mounted workspace instead of /opt/zephyrproject.
unset ZEPHYR_BASE

cd /workspaces

# Image-baked trees live outside the Codespaces mount; link them in before
# west init so the imported zephyr manifest is already visible.
for name in zephyr modules; do
    if [ ! -e "$name" ]; then
        ln -sfn "/opt/zephyrproject/$name" "$name"
    fi
done

if [ ! -f .west/config ]; then
    west init -l zephyr-workshop
fi

# make clangd file visible for the plugin
if [ ! -f .clangd ]; then
    ln -s zephyr-workshop/.devcontainer/.clangd
fi

# make the VS Code launch/debug configuration visible at the workspace root
if [ ! -e .vscode ]; then
    ln -s zephyr-workshop/.vscode
fi

# CMake linker snippets need one real Zephyr tree. The workspace
# symlink at /workspaces/zephyr would mix /workspaces and /opt paths.
west config zephyr.base /opt/zephyrproject/zephyr
west config build.dir-fmt /workspaces/build

# Patches live in the mounted repo, the baked Zephyr tree is unpatched.
# The marker keeps a re-run from failing on already-applied patches.
if [ ! -f .west/patches-applied ]; then
    west patch apply
    touch .west/patches-applied
fi
