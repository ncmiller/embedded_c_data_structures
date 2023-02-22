#!/usr/bin/env bash

set -Eeuo pipefail

# Build
mkdir -p build
cd build
cmake ..
make -j8

# Run tests
ctest \
    --output-on-failure \
    --timeout 2 \
    $@
