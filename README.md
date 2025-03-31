# Carbon Trinity-Audio Interfaces
Single source of truth for audio interfaces shared between Carbon Trinity and Carbon Audio systems. This repository centralizes interface definitions to prevent API inconsistencies and maintain compatibility between systems.

# Current Interfaces

- `include/ITr2AudEmitter.h` - Interface for audio emitters
- `include/ITr2Audio.h` - Interface for audio functionality on assets
- `include/IAudioInputMgr.h` - Interface for audio input management

# Usage
Including in your project, in your CMakeLists.txt:

```cmake
target_include_directories(YourTarget
    PUBLIC
        ${PATH_TO_CARBONAUDIOAPI}/include
)
```

# Available Build Presets

- windows_vs - Visual Studio 2022 with v141 toolset
- windows_nmc - Windows using Ninja Multi-Config
- osx - MacOS using Ninja Multi-Config
