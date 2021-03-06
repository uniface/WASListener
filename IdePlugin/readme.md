# Uniface IDE integration plugin

## Prerequisites for Uniface

- [Uniface 10](https://uniface.com/get-started "Register for Uniface 10 CE")

## Building

1. Create a shortcut for the development environment
`{Uniface's Install folder}\common\bin\ide.exe /dir={projectFolder} /adm={Uniface's Install folder}\uniface\adm ?`
2. Import all sources contained in the WorkArea folder and all its subfolders.
3. Compile the project by opening the project "VERSIONCONTROL" within the IDE and click Compile
Note: Compiling the project means that descriptors for the Uniface dictionary tables will not be built. As the resulting resources folder or uar does not contain descriptors for the Uniface dictinoary tables they can be included anywhere within the list of resources.

At this point, all components have been built. As the sources have not been imported using the utility, all the sources will be marked as new. To fix this, you will need to export all objects into the WorkArea.

1. Uncomment the  line `;IDE_DEFINE_USERMENUS=VC_UPDATED` in ide.asn
2. Start the IDE and select "WrokArea Export/Revert" from the burger menu
3. Click Export All
