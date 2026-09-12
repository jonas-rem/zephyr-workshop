FROM mcr.microsoft.com/devcontainers/base:debian-13

ENV DEBIAN_FRONTEND=noninteractive \
    LC_ALL=C

# Create workspace directories
RUN mkdir -p /workspaces /opt/zephyrproject/manifest /opt/zephyr-sdk \
    && chown -R vscode:vscode /workspaces /opt/zephyrproject /opt/zephyr-sdk

# Install west and basic build dependencies
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    ccache \
    clangd \
    cmake \
    device-tree-compiler \
    file \
    g++-multilib \
    gcc \
    gcc-multilib \
    gdb \
    gperf \
    libmagic1 \
    libsdl2-dev \
    make \
    minicom \
    ninja-build \
    picocom \
    python3-dev \
    python3-pip \
    python3-tk \
    python3-venv \
    ripgrep \
    screen \
    tio \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Install west
RUN pip3 install --no-cache-dir --break-system-packages west

COPY --chown=vscode:vscode west.yml /opt/zephyrproject/manifest/west.yml

USER vscode
WORKDIR /opt/zephyrproject

RUN git -C manifest init \
    && git -C manifest config user.email "devcontainer@localhost" \
    && git -C manifest config user.name "devcontainer" \
    && git -C manifest add west.yml \
    && git -C manifest commit -m "workshop manifest" \
    && west init -l manifest \
    && west update --narrow -o=--depth=1

USER root
RUN pip3 install --no-cache-dir --break-system-packages \
        -r /opt/zephyrproject/zephyr/scripts/requirements-base.txt \
        -r /opt/zephyrproject/zephyr/scripts/requirements-build-test.txt \
        tabulate natsort psutil cbor pyocd

USER vscode
RUN west zephyr-export \
    && west sdk install \
        --install-dir /opt/zephyr-sdk/current \
        --gnu-toolchains \
        arm-zephyr-eabi \
        riscv64-zephyr-elf \
        x86_64-zephyr-elf

ENV ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk/current
WORKDIR /workspaces
