# WorkArea synchronisation for Uniface 10

This Work Area Support utility integrates the Uniface IDE with file-based version control systems by synchronising Uniface's repository with a directory structure (WorkArea) of exports. The utility will detect changes to the export files (like checkout of git branches) and replay them within Uniface's repository whilst also protecting against orphan definitions.
The utility also keeps track of the developer's changes, allowing them to accept the updates or revert to the export file.

**For more information, [configuration instructions](https://community.rocketsoftware.com/viewdocument/work-area-support-utility?CommunityKey=8a320023-2402-4fd6-b71d-6224c43259e8) and a prebuilt version, please visit [Utilities, Addons and Extras](https://community.rocketsoftware.com/forums/unifacesupport/unifacesamples?attachments=&folder=ebc94f86-7df8-42bc-8804-2c2d2a52b06b&libraryentry=4ad8f36f-f820-4c0a-9bc6-44934da24b95&pageindex=0&pagesize=12&search=&sort=most_viewed&viewtype=card) in the Rocket Uniface forum**

## Source structure

The WASListener utility is split into to parts,

* [The IDE plugin](./IdePlugin)
* [The Listener utility](./listener)

Specific build instructions are available in each subfolder
