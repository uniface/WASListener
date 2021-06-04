# WASListener.exe

## Prerequisites for 3gl

- [VCPKG](https://vcpkg.io/en/getting-started.html)
  - git clone https://github.com/Microsoft/vcpkg.git
  - .\vcpkg\bootstrap-vcpkg.bat
  - .\vcpkg\vcpkg integrate install
- [Visual Studio 2019](https://visualstudio.microsoft.com/ "Get Visual Studio")
- [Uniface 10](https://uniface.com/get-started "Register for Uniface 10 CE")

## Building

- Make sure vcpkg is installed and integrated
- In visual studio open the listener folder
- Create your x86 Configuration - Project -> CMake setting for WASListener
  - Configuration name: **x86-Release**
  - Configuration type: **Release**
  - Toolset: **msvc_x86_x64**
  - Build toot: **${projectDir}\out\build\${name}**
  - CMake command arguments: **-DUNIFACE_3GL_FOLDER="{Uniface install Folder}/uniface/3gl"**
    - **-DVCPKG_TARGET_TRIPLET=x86-windows-static** can also be added to statically link boost

The new profile should now be able to build the WASListener project.
 
