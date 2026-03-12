#!/usr/bin/env bash
set -euo pipefail

# Updates 
sudo apt-get update
sudo apt-get upgrade -y

# Build and test dependencies.
sudo apt-get install -y build-essential gdb valgrind libboost-all-dev python3-pip

# Linting tool.
pip3 install --no-cache-dir cpplint
