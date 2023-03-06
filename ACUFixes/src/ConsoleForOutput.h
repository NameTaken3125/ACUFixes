#pragma once

#include <corecrt_wstdio.h>

class ConsoleForOutput
{
public:
    ConsoleForOutput();
    ~ConsoleForOutput();
};
class RedirectSTDOUTToConsole
{
public:
    RedirectSTDOUTToConsole();
    ~RedirectSTDOUTToConsole();
private:
    FILE* f;
};