#include "pch.h"

#include "ACU/CLAssassin.h"
#include "ACU/DebugCommandsHolder.h"

DebugCommandsHolder* GetCheatsHolder()
{
    CLAssassin* cla = CLAssassin::GetSingleton();
    if (!cla) { return nullptr; }
    return cla->cheatCodes;
}
void DrawBuiltinDebugCommands()
{
    ImGui::Text(
        "Most don't do anything.\n"
        "WARNING! The \"Nuke enemies\"/\"Nuke allies\" do crash the game."
    );
    DebugCommandsHolder* cheatsHolder = GetCheatsHolder();
    if (!cheatsHolder) { return; }
    static std::string buf;
    buf.reserve(200);
    for (int i = 0; i < cheatsHolder->arrDebugCommands.size; i++)
    {
        DebugCommand* cheat = cheatsHolder->arrDebugCommands[i];
        buf.clear();
        buf.append(std::to_string(i));
        if (cheat->textDesc)
        {
            buf += ": ";
            buf += cheat->textDesc;
        }
        if (ImGui::Button(buf.c_str()))
        {
            constexpr int mostCommandsSeemToRequireTheFirstParameterToBe1 = 1;
            cheat->fnExecute(mostCommandsSeemToRequireTheFirstParameterToBe1, cheatsHolder);
        }
    }
}
