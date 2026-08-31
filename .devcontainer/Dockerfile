FROM mcr.microsoft.com/devcontainers/base:debian-13

# Create zephyrproject directory
RUN mkdir -p /workspaces \
    && chown -R vscode:vscode /workspaces

# Install west and basic build dependencies
RUN export DEBIAN_FRONTEND=noninteractive && apt-get update \
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
    python3-jsonschema \
    python3-pip \
    python3-tk \
    python3-venv \
    ripgrep \
    screen \
    tio \
    wget \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Install west
RUN pip3 install --break-system-packages west

USER vscode
WORKDIR /workspaces
