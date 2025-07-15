# ACUFixes
Addon/fixes for Assassin's Creed Unity 1.5.0\
A small mod that aims to fix some of the annoying things about Assassin's Creed Unity.\
Now with a Plugin Loader.

> <a href="//imgur.com/a/RpFYGXX">Demo GIFs</a>

## Features:
- Enter windows by pressing R like in Assassin's Creed Syndicate (when climbing walls)
- More reliable "Breakfall" (Hold ParkourDown/Interact (E) to Catch Ledge when in uncontrolled freefall)
- Can drop bombs and perform Quickshot in more situations (while hanging on a wall, during a jump etc.)
- No more imaginary bomb throws
- Wider FOV when aiming bombs and the Guillotine Gun, zoom can be adjusted by pressing Right Mouse Button
- Select Bombs Equipment Type by scrolling the Mouse Wheel
- Arno is not forced to pull out the melee weapon while Disguise is active
- Autowalk and Slow Walk
- Less "sticky" Take Cover/Leave Cover
- Easier turn on horizontal swings
- Can aim bombs while sitting on perches
- Slightly more responsive Crouch button
- Unequip Pistol in the Pistols menu
- FOV slider (in the "Personal requests" section of the mod menu)
- Some other minor things

#### Some cheats:
- Weather Controls
- Use the Head of Saint Denis outside missions (Thanks Jinouga on YouTube for the idea https://www.youtube.com/@jinouga-assassinscreedmodd3155)
- Unbreakable Disguise

## Requirements
Assassin's Creed Unity 1.5.0.

## How to use
To install:\
Place the files directly into the "Assassin's Creed Unity" folder (where the "ACU.exe" file is).\
Game's folder structure will look like this:
```
    Assassin's Creed Unity/
    ├── ACUFixes/
    │   ├── plugins/
    │   │   ├── NewAnimations/
    │   │   └── ACUFixes.dll
    │   └── ACUFixes-PluginLoader.dll
    ├── ACU.exe
    └── version.dll
```
Start the game, press `Insert` to open the menu.

To uninstall:
1. Go to the game's folder.
2. Delete the `version.dll`. The mod will not be loaded automatically anymore.
3. Delete the `ACUFixes/` folder.


## For plugin developers: the Plugin Loader
The `ACUFixes-PluginLoader.dll` can load DLL plugins from the `Assassin's Creed Unity/ACUFixes/plugins` folder.\
If you want to develop code-patching mods for AC Unity, consider building them as a plugin. You get:
- **Automatic injection** when the game's window opens.
- **An ImGui graphical interface immediately compatible with ReShade** (and other plugins).
- **An in-game ImGui Console** for logging (toggled by hotkey, default is "Keyboard OEM3"/"Tilde" (\~)/"Backtick" (\`))
- A small **CrashLog**, which helps during development because this kind of modding does entail crashes, and ACU likes to make them silent.
- A small C++ library containing some reverse engineered classes for AC Unity (the "ACU-RE" project in the source code).
- You can use the `ACUFixes-PluginLoader.dll` without the `ACUFixes.dll` plugin, if you want to.
- The Plugin Loader will disable the **Main Integrity Check** for you.
- The Plugin Loader allows you to attach the **Visual Studio debugger** (after the game's window is opened, at least).

Example plugins: Asset Overrides ([source code](//github.com/NameTaken3125/AssetOverrides-ACUnity)), Halzoid98CPP ([source code](//github.com/NameTaken3125/Halzoid98CPP)) and the `ACUFixes.dll` plugin itself.

## Some more modding resources
### About Animation Modding
See some notes [here](ACUFixes/src/AnimationTools/README.md) about the player's animation graph and how it was extended to implement the Removable Hood feature.

## Warning
Consider backing up your savegame.
