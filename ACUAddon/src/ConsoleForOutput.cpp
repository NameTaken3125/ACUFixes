#include "pch.h"

#include "ConsoleForOutput.h"

ConsoleForOutput::ConsoleForOutput()
{
    AllocConsole();
    // Disables red "X" in the top right corner, as accidentally closing the console will close the whole game process.
    DeleteMenu(GetSystemMenu(GetConsoleWindow(), 0), 0xF060, 0);
}
ConsoleForOutput::~ConsoleForOutput()
{
    FreeConsole();
}
RedirectSTDOUTToConsole::RedirectSTDOUTToConsole()
{
    freopen_s(&f, "CONOUT$", "w", stdout);
}
RedirectSTDOUTToConsole::~RedirectSTDOUTToConsole()
{
    fclose(f);
}
