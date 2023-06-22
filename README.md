# ACUFixes
Addon/fixes for Assassin's Creed Unity 1.5.0\
A small mod that aims to fix some of the annoying things about Assassin's Creed Unity.

> <a href="//imgur.com/a/RpFYGXX">Demo GIFs</a>

## Features:
- Enter windows by pressing R like in Assassin's Creed Syndicate (when climbing walls)
- More reliable "Breakfall" (Hold ParkourDown/Interact (E) to Catch Ledge when in uncontrolled freefall)
- Can drop bombs in more situations (while hanging on a wall, during a jump etc.)
- No more imaginary bomb throws
- Wider FOV when aiming bombs and the Guillotine Gun, zoom can be adjusted by pressing Right Mouse Button
- Select Bombs Equipment Type by scrolling the Mouse Wheel
- Arno is not forced to pull out the melee weapon while Disguise is active
- Autowalk and Slow Walk
- Less "sticky" Take Cover/Leave Cover
- Easier turn on horizontal swings
- Can aim bombs while sitting on perches
- Slightly more responsive Crouch button
- Some other minor things

#### Some cheats:
- Use the Head of Saint Denis outside missions (Thanks Jinouga on YouTube for the idea https://www.youtube.com/@jinouga-assassinscreedmodd3155)
- Unbreakable Disguise

## Requirements
Assassin's Creed Unity 1.5.0.
Specifically the Steam version, I think.
Any other version will most likely crash when mod is injected.

## How to use
### TL;DR
Place `ACUFixes.dll` and `ACUFixesLoader.exe` into the same folder.  
Start the `ACUFixesLoader.exe` (*might* need to start the game first).  
### Details
Place `ACUFixes.dll` and `ACUFixesLoader.exe` into the same folder.  
The mod itself is in the `ACUFixes.dll`. To use it, the DLL needs to be injected into the `ACU.exe` process.  
The Loader is supposed to do that, but it is not required (though recommended).  
After injection, a transparent overlay will be visible in the top left corner (it can be disabled in the menu).  
Press `Insert` to open the menu, press `End` to unload the mod.  
There are 2 ways of loading the DLL mod:
#### - Use the new Loader
The included `ACUFixesLoader.exe` can start the `ACU.exe` process (or automatically find the running process) and inject the DLL.
The Loader and the DLL need to be placed in the same folder.
The Loader opens a console window. You can make it go away automatically by setting
```json
"automaticallyCloseConsoleWindow" : true
```
in `acufixesloader-config.json` that will be created next to the `ACUFixesLoader.exe` after the first time it is started.
##### If the game is already running
If the game version is correct, there should be no problems with this method.
- Start the `ACUFixesLoader.exe`.
- Done.
##### If the game process is NOT running
The Loader will try to start the game and inject the DLL. This was only tested on my PC.
- Start the `ACUFixesLoader.exe`.
- The first time this is done, you'll be asked to find the `ACU.exe` file. It will be remembered on subsequent launches.
- The Loader will try to start the game (I'm actually not sure if it will interact with Steam). If it succeeds, the game will start and the mod will be loaded automatically.
- Now the `ACUFixesLoader.exe` can be used to launch the game and load the mod at the same time.
#### - NOT using the new Loader
Use any DLL Injector to inject the `ACUFixes.dll` into a running `ACU.exe` process.

## Warning
Consider backing up your savegame. Also, not tested in multiplayer.
