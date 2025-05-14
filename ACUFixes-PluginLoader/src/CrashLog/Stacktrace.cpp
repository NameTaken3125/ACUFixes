#include "pch.h"



#include <DbgHelp.h>
#pragma comment(lib, "Psapi")
#pragma comment(lib, "Dbghelp")

namespace Stacktrace::Using_StackWalk64
{



//https://bugs.winehq.org/show_bug.cgi?id=57628
//https://stackoverflow.com/questions/15650528/does-stackwalk64-work-on-64-bit-windows/15650639#15650639
__declspec(noinline)
void trace(
    CONTEXT& ContextRecord
    , ImGuiTextBuffer& outputBuf
    , const std::function<std::string(unsigned long long)>& makeAddressReadable
) {
    //CONTEXT defaultCtx;
    //if (!ContextRecord)
    //    RtlCaptureContext(&defaultCtx);
    CONTEXT& c = ContextRecord;

    STACKFRAME64 s;
    s.AddrPC.Offset = c.Rip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.Rsp;
    s.AddrStack.Mode = AddrModeFlat;
    s.AddrFrame.Offset = c.Rbp;
    s.AddrFrame.Mode = AddrModeFlat;

#ifdef _M_IX86
    DWORD MachineType = IMAGE_FILE_MACHINE_I386;
#endif
#ifdef _M_X64
    DWORD MachineType = IMAGE_FILE_MACHINE_AMD64;
#endif
#ifdef _M_IA64
    DWORD MachineType = IMAGE_FILE_MACHINE_IA64;
#endif
    int idx = 0;

    outputBuf.append("     AddrStack, AddrFrame, AddrPC:\n");
    for (;;) {
        if (!StackWalk64(MachineType, GetCurrentProcess(), GetCurrentThread(), &s, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
            break;

        std::string readableAddrPC = makeAddressReadable(s.AddrPC.Offset);
        outputBuf.appendf(
            "%2d. %8llX %8llX %16llX %s\n"
            , idx++
            , s.AddrStack.Offset
            , s.AddrFrame.Offset
            , s.AddrPC.Offset
            , makeAddressReadable(s.AddrPC.Offset).c_str()
        );
    }
    int x = 0;
}
void DoStacktrace(
    EXCEPTION_RECORD& ExceptionRecord
    , CONTEXT& ContextRecord
    , ImGuiTextBuffer& outputBuf
    , const std::function<std::string(unsigned long long)>& makeAddressReadable
) {
    outputBuf.append("StackWalk64():\n");
    SymInitialize(GetCurrentProcess(), NULL, TRUE);
    CONTEXT disposableCtx = ContextRecord;
    trace(disposableCtx, outputBuf, makeAddressReadable);
}
}

void CrashLog_TopLevelException_DoStacktrace(
    EXCEPTION_RECORD& ExceptionRecord
    , CONTEXT& ContextRecord
    , ImGuiTextBuffer& outputBuf
    , const std::function<std::string(unsigned long long)>& makeAddressReadable
) {
    Stacktrace::Using_StackWalk64::DoStacktrace(ExceptionRecord, ContextRecord, outputBuf, makeAddressReadable);
}
