# WASListener.exe

## Prerequisites for 3gl

- [CMake](https://cmake.org/ "Get CMake")
- [Boost (1.74)](https://www.boost.org/users/history/version_1_74_0.html "Get Boost")
- [Visual Studio 2019](https://visualstudio.microsoft.com/ "Get Visual Studio")
- [Uniface 10](https://uniface.com/get-started "Register for Uniface 10 CE")

## Building

1. Start the cmake-gui
2. Set the "Where is the source code:" field to be the \3gl folder within this project
3. Set the "Where to build the binaries" - It should not matter where this is set to
4. Click Configure
5. In "Optional platform for generator" select "Win 32"
6. Click Finish
7. Set the "UNIFACE_3GL_FOLDER" to `{Uniface's Install folder}\uniface\3gl` 
8. Click Generate
9. Click Open Project
10. Select the desired configuration and build the solution
