#!/bin/bash
# Required to launch correctly

# Move to AO's directory
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}"

# Run with correct linker path
chmod +x SpriteChat
LD_LIBRARY_PATH=. ./SpriteChat
