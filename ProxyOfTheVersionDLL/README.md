Adapted from https://github.com/SubstituteR/DllWrapper
Thanks






Original README:

# DllWrapper

A simple wrapper for version.dll that supports x32 and x64 out of box.

# Instructions

Clone this repo and add your code within `DLL_PROCESS_ATTACH` and simply build. Make sure to change the build architecture to match the architecture of the process you wish to load the DLL in (e.g. x32 for 32-bit processes, x64 for 64-bit processes.)
