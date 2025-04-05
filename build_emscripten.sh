#!/bin/bash

# Try to find emsdk in common locations
find_emsdk() {
    # Check each possible path
    if [ -d "C:/emsdk" ] && [ -f "C:/emsdk/emsdk_env.sh" ]; then
        echo "C:/emsdk"
        return 0
    fi
    
    if [ -d "C:/Users/$USERNAME/emsdk" ] && [ -f "C:/Users/$USERNAME/emsdk/emsdk_env.sh" ]; then
        echo "C:/Users/$USERNAME/emsdk"
        return 0
    fi
    
    if [ -d "C:/adi/emsdk" ] && [ -f "C:/adi/emsdk/emsdk_env.sh" ]; then
        echo "C:/adi/emsdk"
        return 0
    fi
    
    if [ -d "C:/Users/lenovo/src/emsdk" ] && [ -f "C:/Users/lenovo/src/emsdk/emsdk_env.sh" ]; then
        echo "C:/Users/lenovo/src/emsdk"
        return 0
    fi
    
    return 1
}

# Check if emcc is available
if ! command -v emcc &> /dev/null; then
    echo "Emscripten compiler (emcc) not found in PATH"
    
    # Try to find and source emsdk
    EMSDK_PATH=$(find_emsdk)
    if [ -n "$EMSDK_PATH" ]; then
        echo "Found Emscripten at: $EMSDK_PATH"
        echo "Sourcing environment..."
        source "$EMSDK_PATH/emsdk_env.sh"
    else
        # If not found through search, try direct path
        EMSDK_PATH="C:/Users/lenovo/src/emsdk"
        if [ -d "$EMSDK_PATH" ] && [ -f "$EMSDK_PATH/emsdk_env.sh" ]; then
            echo "Found Emscripten at: $EMSDK_PATH"
            echo "Sourcing environment..."
            source "$EMSDK_PATH/emsdk_env.sh"
        else
            echo "Error: Could not find Emscripten installation!"
            echo "Please ensure Emscripten is installed and activated:"
            echo "1. Install Emscripten: git clone https://github.com/emscripten-core/emsdk.git"
            echo "2. cd emsdk"
            echo "3. ./emsdk install latest"
            echo "4. ./emsdk activate latest"
            echo "5. source ./emsdk_env.sh"
            exit 1
        fi
    fi
fi

# If emcc is still not found, try to add it directly to PATH
if ! command -v emcc &> /dev/null; then
    EMC_PATH="C:/Users/lenovo/src/emsdk/upstream/emscripten"
    if [ -f "$EMC_PATH/emcc" ]; then
        echo "Adding Emscripten to PATH directly..."
        export PATH="$EMC_PATH:$PATH"
    fi
fi

# Final verification of emcc
if ! command -v emcc &> /dev/null; then
    echo "Error: Still cannot find emcc after all attempts!"
    echo "Please ensure Emscripten is properly installed and activated."
    echo "You may need to run this from a fresh w64devkit terminal after sourcing emsdk_env.sh"
    exit 1
fi

echo "Building Space Invaders for Emscripten..."
make PLATFORM=PLATFORM_WEB CFLAGS="-DEMSCRIPTEN_BUILD -Wall -std=c++14 -D_DEFAULT_SOURCE -Wno-missing-braces -s -O1" all

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build complete!" 