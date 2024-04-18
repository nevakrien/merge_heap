#!/bin/bash

# Define the base directory for the compiled executables
base_directory="precompiled"

# Compile function for different targets
compile() {
    local os=$1
    local arch=$2
    local target=$3
    local ext=$4

    # Create the target directory if it does not exist
    local target_directory="${base_directory}/${os}/${arch}"
    mkdir -p "${target_directory}"

    # Set the file extension for Windows executables
    local executable="cli${ext}"

    # Compilation command
    zig cc -target "${target}" -g2 cli.c -o "${target_directory}/${executable}"
    echo "Compiled cli for ${os}-${arch} in ${target_directory}"
}

# Compile for x86_64 and ARM on Windows, Linux, and macOS
compile "windows" "x86_64" "x86_64-windows" ".exe"
compile "windows" "arm64"   "aarch64-windows" ".exe"
compile "linux"   "x86_64" "x86_64-linux" ""
compile "linux"   "arm64"   "aarch64-linux" ""
compile "macos"   "x86_64" "x86_64-macos" ""
compile "macos"   "arm64"   "aarch64-macos" ""

echo "Compilation complete for all specified platforms and architectures."
