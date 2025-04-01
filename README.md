# Carbon Trinity-Audio Interfaces
Single source of truth for audio interfaces shared between Carbon Trinity and Carbon Audio systems. This repository centralizes interface definitions to prevent API inconsistencies and maintain compatibility between systems.

# Current Interfaces

- `include/ITr2AudEmitter.h` - Interface for audio emitters
- `include/ITr2Audio.h` - Interface for audio functionality on assets
- `include/IAudioInputMgr.h` - Interface for audio input management

# Usage
To use this library in your project, add the following to your CMakeLists.txt:

```cmake
find_package(CarbonTrinityAudioAPI REQUIRED)
target_link_libraries(YourTarget PRIVATE CarbonTrinityAudioAPI)
```

This will automatically set up all necessary include paths and dependencies.

# Available Build Presets

- windows_vs - Visual Studio 2022 with v141 toolset
- windows_nmc - Windows using Ninja Multi-Config
- osx - MacOS using Ninja Multi-Config