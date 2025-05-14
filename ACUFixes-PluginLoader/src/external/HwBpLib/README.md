Library taken from https://github.com/biocomp/hw_break/tree/d3f5df5c5799db110201492a2712ca1298325ad2

Thank you Artem Tokmakov/biocomp




# Library to programmatically set hardware breakpoints
Include `HwBpLib\inc\HwBpLib.h`.

Use `HwBp::Set(pointer, size, condition)` to set a breakpoint on Write/ReadWrite or Execution of a memory at a certain address, with specified size.

Use `HwBp::Remove()` to remove breakpoint.

This library was inspired by https://www.codeproject.com/Articles/28071/Toggle-hardware-data-read-execute-breakpoints-prog. 
It only sets breakpoints on current thread, but does it faster (as it doesn't spin up threads to do that).

## Building tests
The library is header-only, but you can build and run test file by building HwBp.sln.
