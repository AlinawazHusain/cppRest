# Base image
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        git \
        wget \
        libssl-dev \
        curl \
        ca-certificates \
        lsb-release \
        gnupg \
        && rm -rf /var/lib/apt/lists/*

# Install CMake 3.28+
RUN wget https://github.com/Kitware/CMake/releases/download/v3.28.2/cmake-3.28.2-linux-x86_64.sh && \
    chmod +x cmake-3.28.2-linux-x86_64.sh && \
    ./cmake-3.28.2-linux-x86_64.sh --skip-license --prefix=/usr/local && \
    rm cmake-3.28.2-linux-x86_64.sh

# Set working directory
WORKDIR /app

# Copy project files
COPY . .

# Make scripts executable
RUN chmod +x build.sh run.sh

# Build project
RUN ./build.sh

# Run API
CMD ["bash", "./run.sh"]
