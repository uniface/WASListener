# Build Instructions

## Prerequisites

- [Uniface 10](https://uniface.com/get-started "Register for Uniface 10 CE")
- [cmake](https://cmake.org/download/ "Download cmake")
- [Visual Studio](https://visualstudio.microsoft.com/ "Get Visual Studio")
  
## Get the source code and prepare the the package manager

```powershell
git clone --recurse git@github.com:uniface/WASListener.git
cd WASListener\listener
vcpkg\bootstrap-vcpkg.bat

```

## Compile x64

### Prepare build (x64)

```powershell
cmake -DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET="x64-windows-static" -DUNIFACE_3GL_FOLDER="C:\Program Files\Uniface\Uniface 10 10.4.02\uniface\3gl" -A="x64" -B="build\x64"
```

### Build Release (x64)

```powershell
cmake --build build\x64 --config Release
```

### Biild Debug (x64)

```powershell
cmake --build build\x64 --config Debug
```

## Compile x86

### Prepare build (x86)

```powershell
cmake -DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET="x86-windows-static" -DUNIFACE_3GL_FOLDER="C:\Program Files (x86)\Uniface\Uniface 10 10.4.02\uniface\3gl" -A="Win32" -B="build\x86"
```

### Build Release (x86)

```powershell
cmake --build build\x86 --config Release
```

### Build Debug (x86)

```powershell
cmake --build build\x86 --config Debug
```
