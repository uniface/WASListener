

# WorkArea synchronisation for Uniface 10

**A prebuilt version can be downloaded from - <https://community.uniface.com/display/DOW/Work+Area+Support+Utility>**

- [WorkArea synchronisation for Uniface 10](#workarea-synchronisation-for-uniface-10)
  - [WASListener.exe](#waslistenerexe)
    - [Prerequisites for 3gl](#prerequisites-for-3gl)
    - [Building](#building)
  - [Uniface integration](#uniface-integration)
    - [Prerequisites for Uniface](#prerequisites-for-uniface)
    - [Building](#building-1)

## WASListener.exe

### Prerequisites for 3gl

- [CMake](https://cmake.org/ "Get CMake")
- [Boost (1.74)](https://www.boost.org/users/history/version_1_74_0.html "Get Boost")
- [Visual Studio 2019](https://visualstudio.microsoft.com/ "Get Visual Studio")
- [Uniface 10](https://uniface.com/get-started "Register for Uniface 10 CE")

### Building

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

## Uniface integration

### Prerequisites for Uniface

- [Uniface 10](https://uniface.com/get-started "Register for Uniface 10 CE")

### Building

1. Create a shortcut for the development environment
`{Uniface's Install folder}\common\bin\ide.exe /dir={projectFolder} /adm={Uniface's Install folder}\uniface\adm ?`
2. Import all sources contained in the WorkArea folder and all its subfolders.
3. Compile the project by entering `/all` in the "Optional Command Line Parameters" field when starting the IDE and clicking "Continue"

At this point, all components have been built. As the sources have not been imported using the utility, all the sources will be marked as new. To fix this, you will need to export all objects into the WorkArea.

1. Uncomment the  line ;IDE_DEFINE_USERMENUS=VC_UPDATED in ide.asn
2. Start the IDE and select "WrokArea Export/Revert" from the burger menu
3. Click Export All
