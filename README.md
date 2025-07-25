![](header.png?raw=true)

A complete decompilation of Retro Engine v4 and the menus from Sonic 1 and 2 (2013).

This project is a fork of RSDKv4, which was used to develop the mobile remakes of Sonic 1 and 2 (2013) and aims to improve many already existing functions and add a variety of new ones. The engine was formerly called "Vengeance Engine" since I was using it for my fangame, and while I still am I have decided that I wanted to rename it in case anyone else needs to use the engine for their own purposes.

# **LIST OF THE NEW FEATURES & CHANGES**
 * SteamAPI has been implemented. If you own Sonic Origins on Steam, Origins features will be enabled and if you own Sonic Origins Plus you can play as Amy! (can be toggled, see [here](#compiling))
  * Custom Discord RPC has been added (I'll probably make a guide on how it works, no promises though.) You'll have to make your own [Discord Application](https://discord.com/developers/applications) and add your own art. The new functions include:
    * SetPresenceState("yourtexthere")
    * SetPresenceDetails("yourtexthere")
    * SetPresenceLargeImage("yourimagenamehere") - TEXT HAS TO BE THE SAME NAME AS ART ASSET
    * SetPresenceLargeText("yourtexthere")
    * SetPresenceSmallImage("yourimagenamehere") - TEXT HAS TO BE THE SAME NAME AS ART ASSET
    * SetPresenceSmallText("yourtexthere")
    * UpdatePresence()
 * Video Player has been ported over from CD, allowing any video to be played. (Videos must be in .ogv, and placed in "Data/Videos")
 * Compatibility with mods / projects that run on the older syntax.
 * Many original limitations have been hugely buffed (e.g. more chunks, tiles, and objects can be in a stage, music file size can be larger, etc)
 * Objects that use animation files (e.g. players) now support ink effects
 * New fade function - "SetClassicFade" - This fades the screen exactly like SetScreenFade except it is more in-line with the genesis games
 * New FX command - "FX_ALL" - This allows one single drawn sprite to use all effects at once, these include ink effects, alpha, rotation, flipping, and (to be fixed) scaling
 * New temp values - temp8, temp9, temp10
 * New functions (be sure to add these as values in GameConfig.bin!)
    * "CheckUpdates" - Checks whether your game needs an update
    * "LoadWebsite" - Opens up any web link into your browser (please don't abuse this...)
    * "GetModID" - Gets the ID of any mod in in the mod list and stores it in the checkResult (this could be used for having better mod compatibility with other mods)
    * PLEASE MESSAGE ME ON DISCORD IF YOU ARE HAVING ANY TROUBLE WITH GETTING THESE FUNCTIONS WORKING - MY USERNAME IS prodnw

# Fork Credits
 * ProdNW - Code porting, other minor edits
 * Jd - Video Player, SteamAPI, Discord RPC
 * Elspeth - Graphical FX additions, script parsing additions
 * LittlePlanetCD - Code optimizations
 * EggBanana - New functions listed above
 * SDG & Geared - Other misc. stuff

# To-Dos:
- [ ] Fix Extended Camera
- [ ] Backport a huge variety of features from v5/v5u (e.g. DrawLine, Local 2PVS, PNG Images, and more)


# **SUPPORT THE OFFICIAL RELEASE OF SONIC 1 & 2**
+ Without assets from the official releases, this decompilation will not run.

+ You can get official releases of Sonic 1 & Sonic 2 from:
  * Windows
    * Via Steam, from [Sonic Origins](https://store.steampowered.com/app/1794960)
    * Via the Epic Games Store, from [Sonic Origins](https://store.epicgames.com/en-US/p/sonic-origins)
  * iOS
    * [Sonic 1, Via the App Store](https://apps.apple.com/au/app/sonic-the-hedgehog-classic/id316050001)
    * [Sonic 2, Via the App Store](https://apps.apple.com/au/app/sonic-the-hedgehog-2-classic/id347415188)
    * A tutorial for finding the game assets from the iOS versions can be found [here](https://gamebanana.com/tuts/14491).
  * Android
    * [Sonic 1, Via Google Play](https://play.google.com/store/apps/details?id=com.sega.sonic1px)
    * [Sonic 2, Via Google Play](https://play.google.com/store/apps/details?id=com.sega.sonic2.runner)
    * [Sonic 1, Via Amazon](https://www.amazon.com.au/Sega-of-America-Sonic-Hedgehog/dp/B00D74DVKM)
    * [Sonic 2, Via Amazon](https://www.amazon.com.au/Sega-of-America-Sonic-Hedgehog/dp/B00HAPRVWS)
    * A tutorial for finding the game assets from the Android versions can be found [here](https://gamebanana.com/tuts/14492).

Even if your platform isn't supported by the official releases, you **must** buy or officially download it for the assets (you don't need to run the official release, you just need the game assets).

If you want to transfer your save(s) from the official mobile version(s), the **Android pre-forever** file path is `Android/data/com.sega.sonic1 or 2/SGame.bin` (other versions may have different file paths). Copy that file into the decompilation's folder with the name `SData.bin`.

# Additional Tweaks
* Added the built in script compiler from RSDKv5U.
* Added a built in mod loader and API, allowing to easily create and play mods with features such as save file redirection, custom achievements and XML GameConfig data.
* Custom menu and networking system for Sonic 2 multiplayer, allowing anyone to host and join servers and play 2P VS.
  * Servers may be unreliable; this feature is more or less a proof of concept.
* Egg Gauntlet Zone is playable in the Time Attack menu in Sonic 2, if you're using a version of the game that includes it.
* There is now a `settings.ini` file that the game uses to load all settings, similar to Sonic Mania.
* The dev menu can now be accessed from anywhere by pressing the `ESC` key if enabled in the config.
* The `F12` pause, `F11` step over & fast forward debug features from Sonic Mania have all been ported and are enabled if `devMenu` is enabled in the config.
* A number of additional dev menu debug features have been added:
  * `F1` will load the first scene in the Presentation stage list (usually the title screen).
  * `F2` and `F3` will load the previous and next scene in the current stage list.
  * `F5` will reload the current scene, as well as all assets and scripts.
  * `F8` and `F9` will visualize touch screen and object hitboxes.
  * `F10` will activate a palette overlay that shows the game's 8 internal palettes in real time.
* Added the idle screen dimming feature from Sonic Mania Plus, as well as allowing the user to disable it or set how long it takes for the screen to dim.

# How to Build

This project uses [CMake](https://cmake.org/), a versatile building system that supports many different compilers and platforms. You can download CMake [here](https://cmake.org/download/). **(Make sure to enable the feature to add CMake to the system PATH during the installation!)**

## Get the source code

**DO NOT** download the source code ZIP archive from GitHub, as they do not include the submodules required to build the decompilation.

Instead, you will need to clone the repository using Git, which you can get [here](https://git-scm.com/downloads).

Clone the repo **recursively**, using:
`git clone --recursive https://github.com/prodnw/rsdkv4-plus.git`

If you've already cloned the repo, run this command inside of the repository:
```git submodule update --init --recursive```

## Getting dependencies

### Windows
 To handle dependencies, you'll need to install [Visual Studio Community](https://visualstudio.microsoft.com/downloads/) (make sure to install the `Desktop development with C++` package during the installation) and [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd#1---set-up-vcpkg) (You only need to follow `1 - Set up vcpkg`).

After installing those, run the following in Command Prompt (make sure to replace `[vcpkg root]` with the path to the vcpkg installation!):
- `[vcpkg root]\vcpkg.exe install glew sdl2 libogg libtheora libvorbis --triplet=x64-windows-static` (If you're compiling a 32-bit build, replace `x64-windows-static` with `x86-windows-static`.)
Add `curl` before ` --triplet=` to use cURL/the update checker.

Finally, follow the [compilation steps below](#compiling) using `-DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_PREFIX_PATH=[vcpkg root]/installed/x64-windows-static/` as arguments for `cmake -B build`.
  - Make sure to replace each instance of `[vcpkg root]` with the path to the vcpkg installation!
  - If you're compiling a 32-bit build, replace each instance of `x64-windows-static` with `x86-windows-static`.

### Linux
Install the following dependencies: then follow the [compilation steps below](#compiling):
- **pacman (Arch):** `sudo pacman -S base-devel cmake glew sdl2 libogg libtheora libvorbis`
- **apt (Debian/Ubuntu):** `sudo apt install build-essential cmake libglew-dev libglfw3-dev libsdl2-dev libogg-dev libtheora-dev libvorbis-dev`
- **rpm (Fedora):** `sudo dnf install make gcc cmake glew-devel glfw-devel SDL2-devel libogg-devel libtheora-devel  libvorbis-devel zlib-devel`
- **apk (Alpine/PostmarketOS)** `sudo apk add build-base cmake glew-dev glfw-dev sdl2-dev libogg-dev libtheora-dev libvorbis-dev`
- Your favorite package manager here, [make a pull request](https://github.com/RSDKModding/RSDKv4-Decompilation/fork)
- To use cURL/the update checker, you may have to run one of the following commands depending on your system;
  - **pacman (Arch):** `sudo pacman -S curl`
  - **apt (Debian/Ubuntu):** `sudo apt install libcurl4-openssl-dev`
  - **rpm (Fedora):** `sudo dnf install libcurl-devel`
#### TODO - apk
  - **apk (Alpine/PostmarketOS)** `sudo apk add `

### Android
Follow the android build instructions [here.](./dependencies/android/README.md)

## Compiling

Compiling is as simple as typing the following in the root repository directory:
```
cmake -B build
cmake --build build --config release
```

The resulting build will be located somewhere in `build/` depending on your system.

The following cmake arguments are available when compiling:
- Use these by adding `-D[flag-name]=[value]` to the end of the `cmake -B build` command. For example, to build with `RETRO_DISABLE_PLUS` set to on, add `-DRETRO_DISABLE_PLUS=on` to the command.

### RSDKv4 flags
- `RETRO_REVISION`: What revision to compile for. Takes an integer, defaults to `3` (Origins).
- `RETRO_DISABLE_PLUS`: Whether or not to disable the Plus DLC. Takes a boolean (on/off): build with `on` when compiling for distribution. Defaults to `off`.
- `RETRO_FORCE_CASE_INSENSITIVE`: Forces case insensivity when loading files. Takes a boolean, defaults to `off`.
- `RETRO_MOD_LOADER`: Enables or disables the mod loader. Takes a boolean, defaults to `on`.
- `RETRO_NETWORKING`: Enables or disables networking features used for Sonic 2's 2P VS mode. Takes a boolean, defaults to `on`.
- `RETRO_USE_HW_RENDER`: Enables the Hardware Renderer used by the main menu and touch controls UI. Takes a boolean, defaults to `on`.
- `RETRO_ORIGINAL_CODE`: Removes any custom code. *A playable game will not be built with this enabled.* Takes a boolean, defaults to `off`.
- `RETRO_SDL_VERSION`: *Only change this if you know what you're doing.* Switches between using SDL1 or SDL2. Takes an integer of either `1` or `2`, defaults to `2`.
- `RETRO_USE_STEAM`: Enables the use of the SteamAPI, which can detects if you own [Sonic Origins](https://store.steampowered.com/app/1794960/Sonic_Origins/) and enable features specific to Origins. It also detects if you own [Sonic Origins Plus](https://store.steampowered.com/app/2343200/Sonic_Origins__Plus_Expansion_Pack/) and enables those features as well, defaults to `off`.
- `RETRO_USE_DISCORD` : Toggles whether or not Discord RPC is enabled, defaults to `off`.
- `RETRO_USE_CURL` : Adds support for cURL, which is used for loading websites, defaults to `off`.
- `RETRO_ACCEPT_OLD_SYNTAX` : Adds back aliases that were previously defined within the engine and not the script, allowing compatibility for mods that were made with the old syntax, defaults to `off`.

## Unofficial Branches
Follow the installation instructions in the readme of each branch.
* For the **PlayStation Vita**, go to [Xeeynamo's fork](https://github.com/xeeynamo/Sonic-1-2-2013-Decompilation).
* For the **Nintendo Switch**, go to [heyjoeway's fork](https://github.com/heyjoeway/Sonic-1-2-2013-Decompilation).
* For the **Nintendo 3DS**, go to [JeffRuLz's fork](https://github.com/JeffRuLz/Sonic-1-2-2013-Decompilation).
  * A New Nintendo 3DS is required for the games to run smoothly.
* To play it on the web using **Wasm**, go to [mattConn's fork](https://github.com/mattConn/Sonic-Decompilation-WASM).

Because these branches are unofficial, we can't provide support for them and they may not be up-to-date.

## Other Platforms
Currently the only supported platforms are the ones listed above, however the backend uses libogg, libvorbis & SDL2 to power it (as well as tinyxml2 for the mod API and asio for networking), so the codebase is very multiplatform.
If you're able to, you can clone this repo and port it to a platform not on the list.

# Server
The multiplayer server requires Python 3.8 or later. You can download Python [here](https://www.python.org/downloads/).
To use the server, open Command Prompt in the folder [Server.py](./Server/Server.py) is located in, then run the command `py -3 Server.py [local IPv4 address] [port] debug`. You can find your local IPv4 address using the command `ipconfig`.
Note that the C++ server found in the `Server` folder has been deprecated and no longer works. It has been kept in the repo for reference purposes.

# FAQ
You can find the FAQ [here](./FAQ.md).

# Special Thanks
* [st×tic](https://github.com/stxticOVFL) for helping me fix bugs, tweaking up my sometimes sloppy code and generally being really helpful and fun to work with on this project.
* [The Weigman](https://github.com/TheWeigman) for creating the header you see up here along with similar assets.
* Everyone in the [Retro Engine Modding Server](https://dc.railgun.works/retroengine) for being supportive of me and for giving me a place to show off these things that I've found.

# Contact:
Join the [Retro Engine Modding Discord Server](https://dc.railgun.works/retroengine) for any extra questions you may need to know about the decompilation or modding it.
