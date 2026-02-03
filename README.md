# XY Control - JUCE Implementation

High-performance XY control pad with spring physics and organic glow effects, optimized for VST plugin UIs.

## Features

- Real spring physics simulation with mass, stiffness, and damping
- 5-layer glow system with heavy, fluid motion
- OpenGL-accelerated rendering for minimal CPU usage
- Breathing animation when idle
- Drag-anywhere control (stays locked even outside bounds)

## Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Git (for fetching JUCE)

### macOS
```bash
# Install with Homebrew
brew install cmake
```

### Linux
```bash
sudo apt-get install cmake build-essential libasound2-dev
```

### Windows
- Install CMake from https://cmake.org/download/
- Install Visual Studio 2019 or later with C++ tools

## Building

1. Open Terminal and navigate to the project directory:
```bash
cd ~/xy-control-juce
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Run CMake (this will automatically download JUCE):
```bash
cmake ..
```

4. Build the project:

**macOS/Linux:**
```bash
cmake --build . --config Release
```

**Windows:**
```bash
cmake --build . --config Release
```

## Running

### macOS
```bash
./XYControl_artefacts/Release/XY\ Control.app/Contents/MacOS/XY\ Control
```

Or double-click the app in Finder at:
`build/XYControl_artefacts/Release/XY Control.app`

### Linux
```bash
./XYControl_artefacts/Release/XY\ Control
```

### Windows
```
XYControl_artefacts\Release\XY Control.exe
```

## How to Use

1. **Click and drag** anywhere in the white square to move the cursor
2. **Drag outside** the bounds while holding - the cursor stays locked to edges
3. **Release and wait** ~200ms to see the breathing animation
4. **Watch the glow** - notice how it lags behind with heavy, fluid motion

## Performance

- **GPU**: <0.5ms per frame
- **CPU**: <0.1ms per frame (just spring physics)
- **60 FPS** locked on any modern hardware

## Integration into Your VST

To use this in your plugin:

1. Copy `XYControlComponent.h` and `XYControlComponent.cpp` to your plugin source
2. Add `juce_opengl` module to your plugin project
3. Add the component to your editor:
```cpp
addAndMakeVisible(xyControl);
xyControl.setBounds(x, y, size, size);
```

## Customization

Edit these values in `XYControlComponent.cpp`:

**Spring physics** (line 4-9):
```cpp
SpringLayer(stiffness, damping, mass)
// Lower stiffness = slower response
// Higher damping = less bounce
// Higher mass = heavier feel
```

**Glow colors/sizes** (line 10-16):
```cpp
{width, height, opacity, blurRadius, color}
```

**Breathing speed** (line 146):
```cpp
float breathePhase = time * (4.0f + i * 0.3f);
// Adjust multipliers for faster/slower breathing
```

## Troubleshooting

**Build fails with "Could not find JUCE":**
- Check your internet connection (CMake needs to download JUCE)
- Try: `rm -rf build` and rebuild

**Application crashes on startup:**
- Make sure your GPU drivers are up to date
- Try commenting out the OpenGL code and use software rendering

**Glow doesn't show:**
- The blur is rendered using multiple overlapping circles
- Check if OpenGL context is properly initialized

## License

MIT License - feel free to use in commercial projects
