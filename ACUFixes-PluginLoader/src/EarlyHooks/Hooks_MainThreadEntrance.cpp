#include "pch.h"

/*


Most of my features don't require super-early hooks.
("Early" here means code that runs before the game's window opens,
before it is safe to apply most of the code patches.)
But I found a need of an early hook with the Asset Overrides.
You can read further below about why that feature needs earlier hooks than most.[*1]

Placing very early hooks into ACU is problematic.
- I cannot start the game under a debugger.
- The game process is started twice, actually, presumably due to some kind of protection handshake.
  The Steam version of ACU does, at least.
  This makes CreateProcess(CREATE_SUSPENDED) useless (for now at least).
- The Main Integrity Check (the one that needs to be killed before one can freely modify the game code)
  only starts just before the game window becomes visible,
  but other integrity checks run earlier than that, and will cause a crash if they detect
  code modifications.
- If the mod is injected very early at the start, then most of the game's code
  isn't even present yet, and is filled with zeroes instead.
  For the most part there isn't anything to hook yet.
- Using Hardware Breakpoints is still an option that bypasses the integrity checks,
  but they are cumbersome, you can only have 4 per thread,
  and I'm unable to set them before a thread starts.
  That is, setting them during DLL_THREAD_ATTACH is too soon, because the thread hasn't fully started
  (hasn't reached its Start Address), so the Thread Context including the Debug Registers
  gets reset after DLL_THREAD_DETACH, so the Hardware Breakpoints don't last.
- There's an issue of the Loader Lock. There's a race to place the very early hooks,
  but I'm limited by the LL while I'm within DLLMain(). So I need to start a new thread
  to get out of the LL. And preferably to suspend the other threads if I'm not able to control
  these other threads yet.
  But this introduces the risk of an occasional deadlock at startup,
  and that makes this no "Fixes" anymore.

Long story shorter: I use the Proxy DLL to get injected _very_ early,
before the MainThread has even started.
The mod's initialization is done in parts, jumping from one hook to another,
setting up a next hook and cleaning up the previous one,
in a bit of a spaghetti fashion.

Some specifics:
1. The Proxy DLL (version.dll) loads the PluginLoader DLL.
2. The PluginLoader's DLLMain(DLL_PROCESS_ATTACH) is run in the context of the game's MainThread.
3. I patch the MainThread's Start Address to jump into my code.
...DLLMain(DLL_PROCESS_ATTACH) exits...
...Game's Statically linked DLLs finish getting loaded...
...MainThread reaches its StartAddress...
4. MainThread jumps into my code.
5. I restore the original code at the MainThread's StartAddress.
   I'm now running in the MainThread free of the Loader Lock
   and also able to place Hardware Breakpoints.
6. I place a Hardward Breakpoint at CreateThread() on the MainThread and install a Vectored Exception Handler (VEH) to catch my breakpoint.
7. I jump back to MainThread's Start Address.
...MainThread now runs its original code as it was meant to...
...Splash Screen with 4 assassins appears...
...Some integrity checks are done (not the main one), but they don't find any code modifications...
...Some of the .forge files get loaded, some of the game objects get loaded from them...
8. MainThread runs CreateThread(MainIntegrityCheck).
9. My VEH catches CreateThread() at the first instruction and replaces MainIntegrityCheck with an EmptyThread.
   (Thank you to lnx00 (https://next.nexusmods.com/profile/lnx00?gameId=2619)
   for this idea from his "Disable Mouse Smoothing" mod
   https://www.nexusmods.com/assassinscreedunity/mods/357
   https://github.com/lnx00/acu-patches/blob/d288464804536baf7b8ce156949f7f4bc516985e/src/game/integrity.rs#L164
   )
10. The MainIntegrityCheck is now disabled, I'm free to apply the rest of the code patches and load the plugins.

Some notes:
- The MainThread is not the thread that calls Present().
- I'm keeping the "heartbeat" thread, the one that just calls Sleep() in a loop
  and waits for Base::Data::Detached. It is still useful to have it,
  at least to repeatedly clear the BeingDebugged flag.
- You can see in the "..." steps above that I'm still unable to place arbitrary early hooks.
  I think the best option I have if I require control over that code
  is to use the hardware breakpoints
  and a Vectored Exception Handler to place hooks at key points
  during the game's initialization.
  And hopefully I won't need more than 4 of these at a time.










[*1]About why the Asset Overrides want to have early hooks:
Using a proxy DLL and a Sleep() loop to wait until the main integrity check
can be disabled is enough for most of the mod's features.
But the main integrity check, the one that needs to be terminated to allow normal code patches,
doesn't start until just before the game's window opens, which is quite a bit after the game's
.forge files started to open.
Most mods for ACU are outfit replacers, and for those it's fine to start patching the code late -
they don't get loaded until you load into Paris (or another World).
The exception is, for example, the datapack that contains the player's AtomGraph:
    DataAI_GroupBehavior_Couple_TakeAway.data
and the one that among other things contains all the meshes for all the weapons:
    Game Bootstrap Settings.data
Why does the game need to load all the weapon meshes before even starting the integrity check?
Because Unity of course.
This means that replacing outfits at runtime is easy while replacing weapons is a pain.
To be able to override _all_ datapacks I need a way to hook the game code
very early.
Problem 1:
    I don't think I can _edit_ the code before the main integrity check is started.
    This would cause a crash, and I haven't been able to bypass whatever it is VMProtect does to protect
    the code. (By the way, before the thread that runs the Main Integrity Check even starts,
    the MIC _code_ is already running in a different thread.)
Problem 2:
    When the PluginLoader is loaded, most of the game code and some of VMProtect code is actually mostly zeroes.
    The game code then gets unpacked from the VM.
    What this means is that at the moment the PluginLoader starts, there isn't anything to even hook yet.
    Of the 50+ threads that the game is normally running, only like 8 have started, and
    there's an ocean of virtualized code to execute before the Main Integrity Check can be terminated.

What seems to me the best approach right now (until there's a solution to bypassing
or deobfuscating the VMProtect) is to utilize the technique of the Cheat Engine's VEH debugger
and use the 4 hardware breakpoints plus a Vectored Exception Handler to place hooks at key points
during the game's initialization.
*/

#include "base.h"
#include "ACU/basic_types.h"
#include "ACU/Threads/KnownThreads.h"
#include "EarlyHooks/VEH.h"

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"
#include "external/HwBpLib/inc/HwBpLib.h"

#include "MyLog.h"



using RunImmediatelyAtTheStartOfMainThread_fnt = void(*)(void);
struct MainEntryPointHook_Trampoline : AutoAssemblerCodeHolder_Base
{
    SymbolWithAnAddress* m_Trampoline = nullptr;
    MainEntryPointHook_Trampoline(RunImmediatelyAtTheStartOfMainThread_fnt FunctionToRunImmediatelyAfterGamesMainThreadStarted);
};

MainEntryPointHook_Trampoline::MainEntryPointHook_Trampoline(RunImmediatelyAtTheStartOfMainThread_fnt FunctionToRunImmediatelyAfterGamesMainThreadStarted)
{
    DEFINE_ADDR(mainThreadEntryPoint__originalAddr, ACU::Threads::ThreadStartAddr_MainThread);
    ALLOC(mainThreadEntryPoint__cave, 0x1000, ACU::Threads::ThreadStartAddr_MainThread);
    m_Trampoline = &mainThreadEntryPoint__cave;
    mainThreadEntryPoint__cave = {
        "48 83 EC 08   "                                    // - sub rsp,08 { 8 }
        "FF 15 09000000"                                    // - call qword ptr [myExternalFunc]
        "48 83 C4 08   "                                    // - add rsp,08 { 8 }
        "E9", RIP(mainThreadEntryPoint__originalAddr),		// - jmp ACU.exe+7544B1D
        // myExternalFunc:
        dq((uint64)FunctionToRunImmediatelyAfterGamesMainThreadStarted),
    };
}

struct MainEntryPointHook_TheJmpToTrampoline : AutoAssemblerCodeHolder_Base
{
    MainEntryPointHook_Trampoline* m_Trampoline;
    MainEntryPointHook_TheJmpToTrampoline(MainEntryPointHook_Trampoline& trampoline);
};

MainEntryPointHook_TheJmpToTrampoline::MainEntryPointHook_TheJmpToTrampoline(MainEntryPointHook_Trampoline& trampoline)
    : m_Trampoline(&trampoline)
{
    DEFINE_ADDR(mainThreadEntryPoint__originalAddr, ACU::Threads::ThreadStartAddr_MainThread);
    uintptr_t resolvedTrampolineAddress = *m_Trampoline->m_Trampoline->m_ResolvedAddr;
    DEFINE_ADDR(mainThreadEntryPoint__trampolineLocation, resolvedTrampolineAddress);
    mainThreadEntryPoint__originalAddr = {
        db(0xE9), RIP(mainThreadEntryPoint__trampolineLocation)
    };
}

/*
Jmp from MainThread's Start Address into a small trampoline, and from there into C++.
From C++, restore the original code (needed so that integrity checks don't crash the game),
but don't deallocate the trampoline.
From C++ now perform the mod's initialization in the context of the MainThread, at its very start.
Jump back into trampoline, and from there into the original code.
*/
class MainThreadEntryHook_PatchContainer
{
public:
    AutoAssembleWrapper<MainEntryPointHook_Trampoline> mainThreadEntryHook_caveWithTrampoline;
    AutoAssembleWrapper<MainEntryPointHook_TheJmpToTrampoline> mainThreadEntryHook_jmpToTrampoline;
    MainThreadEntryHook_PatchContainer(RunImmediatelyAtTheStartOfMainThread_fnt FunctionToRunImmediatelyAfterGamesMainThreadStarted)
        : mainThreadEntryHook_caveWithTrampoline(FunctionToRunImmediatelyAfterGamesMainThreadStarted)
        , mainThreadEntryHook_jmpToTrampoline(mainThreadEntryHook_caveWithTrampoline.GetCodeHolder())
    {
        mainThreadEntryHook_caveWithTrampoline.Activate();
        mainThreadEntryHook_jmpToTrampoline.Activate();
    }
};
std::optional<MainThreadEntryHook_PatchContainer> g_PatchesToTheVeryStartOfTheGamesMainThread;

void RestoreMainThreadStartAddressOriginalCode()
{
    g_PatchesToTheVeryStartOfTheGamesMainThread->mainThreadEntryHook_jmpToTrampoline.Deactivate();
}

std::optional<VEHandler> g_EarlyHooks_VEHandler;
void RemoveHardwareBreakpointFromThisThreadDuringException(_EXCEPTION_POINTERS* ExceptionInfo, void* address)
{
    if ((void*)ExceptionInfo->ContextRecord->Dr0 == address)
    {
        ExceptionInfo->ContextRecord->Dr0 = 0;
        ExceptionInfo->ContextRecord->Dr7 &= ~1;
    }
    if ((void*)ExceptionInfo->ContextRecord->Dr1 == address)
    {
        ExceptionInfo->ContextRecord->Dr1 = 0;
        ExceptionInfo->ContextRecord->Dr7 &= ~4;
    }
    if ((void*)ExceptionInfo->ContextRecord->Dr2 == address)
    {
        ExceptionInfo->ContextRecord->Dr2 = 0;
        ExceptionInfo->ContextRecord->Dr7 &= ~16;
    }
    if ((void*)ExceptionInfo->ContextRecord->Dr3 == address)
    {
        ExceptionInfo->ContextRecord->Dr3 = 0;
        ExceptionInfo->ContextRecord->Dr7 &= ~64;
    }
}


void MainIntegrityCheckHasJustBeenDisabled();
void MainIntegrityCheckHasJustBeenDisabledWithoutStarting()
{
    LOG_DEBUG(DefaultLogger,
        "[+] Main Integrity Check has just been disabled without starting."
    );
    MainIntegrityCheckHasJustBeenDisabled();
}
static DWORD WINAPI EmptyThread(LPVOID lpThreadParameter) { return TRUE; }
LONG EarlyHooks_HardwareBreakpointsHandler(::_EXCEPTION_POINTERS* ExceptionInfo)
{
    void* ExceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;
    if (ExceptionAddress == CreateThread)
    {
        ExceptionInfo->ContextRecord->EFlags |= EFLAGS_MASK_RF;
        uintptr_t startAddress = ExceptionInfo->ContextRecord->R8;
        if (startAddress == ACU::Threads::ThreadStartAddr_MainIntegrityCheck)
        {
            RemoveHardwareBreakpointFromThisThreadDuringException(ExceptionInfo, CreateThread);
            ExceptionInfo->ContextRecord->R8 = (DWORD64)EmptyThread;
            //MessageBoxF(
            //    "MIC DOA.\n"
            //);
            MainIntegrityCheckHasJustBeenDisabledWithoutStarting();
            g_EarlyHooks_VEHandler.reset();
        }
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}









void InstallCrashLog();
/*
In here, I'm in the Context of the MainThread, free of the DLLMain and Loader Lock,
which means I can use Hardware Breakpoints (on the threads I can access).
I'm not yet able to patch the game code, because it's gonna be a while
until the MainIntegrityCheck starts.
*/
void DoImmediatelyAtTheStartOfGamesMainThread()
{
    RestoreMainThreadStartAddressOriginalCode();
    LOG_DEBUG(DefaultLogger,
        "[+] Running at the very start of game's main thread...\n"
    );
    InstallCrashLog();
    g_EarlyHooks_VEHandler.emplace(1, EarlyHooks_HardwareBreakpointsHandler);
    HwBp::Set(CreateThread, 1, HwBp::When::Executed);
}

void BeforeGameMainThreadStarted_HookTheStartAddress()
{
    g_PatchesToTheVeryStartOfTheGamesMainThread.emplace(DoImmediatelyAtTheStartOfGamesMainThread);
}
