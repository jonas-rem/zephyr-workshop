FROM mcr.microsoft.com/devcontainers/base:debian-13

# Create zephyrproject directory
RUN mkdir -p /zephyrproject

# Install west and basic build dependencies
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    cmake \
    ninja-build \
    gperf \
    file \
    ccache \
    device-tree-compiler \
    wget \
    python3-dev \
    python3-pip \
    python3-venv \
    python3-tk \
    python3-jsonschema \
    make \
    gcc \
    gcc-multilib \
    g++-multilib \
    libsdl2-dev \
    libmagic1 \
    ripgrep \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Install west
RUN pip3 install --break-system-packages west
