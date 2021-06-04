# WorkArea synchronisation for Uniface 10

This Work Area Support utility integrates the Uniface IDE with file-based version control systems by synchronising Uniface's repository with a directory structure (WorkArea) of exports. The utility will detect changes to the export files (like checkout of git branches) and replay them within Uniface's repository, whilst also protecting against orphan definitions.
The utility also keeps track of the developer's changes, allowing them to accept the updates or revert to the export file.

**For more information, instructions and a prebuilt version, please visit - <https://community.uniface.com/display/DOW/Work+Area+Support+Utility>**

## Source structure

The WASListener utility is split into to parts,

* **The IDE plugin (IdePlugin subfolder)<./IdePlugin>**
* **The Listener utility (Listener subfolder)<./listener>**

The Listener utility requires the IDE plugin to start and run and the IDE plugin can be used in isolation.
Build instructions are available in each subfolder
