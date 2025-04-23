# Space Invaders

A Space Invaders game made using Raylib.

## Building the Game

### Desktop Build

To build the game for desktop platforms, follow these steps:

1. Create a build directory:
```
mkdir build
cd build
```

2. Configure and build the project:
```
cmake ..
cmake --build .
```

The executable will be created in the `build` directory.

### Emscripten Build (Web)

To build the game for web browsers using Emscripten, use one of the following commands:

#### Windows:
```
build_emscripten.bat
```

#### Linux/macOS:
```
./build_emscripten.sh
```

## Controls

- Arrow keys: Move the spaceship
- Space: Shoot
- P: Pause/Resume
- ESC: Exit menu
- Y/N: Confirm/Cancel exit


Big thanks to https://github.com/educ8s (https://www.youtube.com/@programmingwithnick) for making great Raylib tutorials on youtube.

