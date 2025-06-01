#include "pch.h"

#include "ACU/LoadingThread.h"
#include "external/hwbrk/hwbrk.h"

#include "VEH.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(SOOLog, "[SOOverride]");


// The function of interest has at least 3 call sites.
// Hooking the function at the prologue technically allows to catch more deserializations,
// but I'm not sure that I need them. Everything I tried to override
// was successfully caught from the callsite at 0x1426F0DC2.
void* g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Prologue =   (void*)0x14270B510;
void* g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Callsite0 =  (void*)0x1426F0DC2;
void* g_CodeAddr_DeserializeSingleManagedObject_Full =                  (void*)0x1426F0CF7;
void* g_CodeAddr_WhenGameForgesJustOpened =                             (void*)0x14272F794;
std::optional<VEHandler> g_EarlyHooks_SingleObjectOverride_VEHandler;

HANDLE g_HWBPHandle_DeserializeSingleObject = nullptr;
HANDLE g_HWBPHandle_DeserializeSingleObjectFull = nullptr;
HANDLE g_HWBPHandle_NewForgeAdded = nullptr;
void AssetOverrides_EarlyHooks_HWBP_SetBP()
{
    HANDLE loadingThreadHandle = OpenThread(THREAD_ALL_ACCESS, 0, LoadingThread::GetSingleton()->threadID);
    //g_HWBPHandle_DeserializeSingleObject = SetHardwareBreakpoint(loadingThreadHandle, HWBRK_TYPE_CODE, HWBRK_SIZE_1, g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Callsite0);
    g_HWBPHandle_DeserializeSingleObjectFull = SetHardwareBreakpoint(loadingThreadHandle, HWBRK_TYPE_CODE, HWBRK_SIZE_1, g_CodeAddr_DeserializeSingleManagedObject_Full);
    g_HWBPHandle_NewForgeAdded = SetHardwareBreakpoint(loadingThreadHandle, HWBRK_TYPE_CODE, HWBRK_SIZE_1, g_CodeAddr_WhenGameForgesJustOpened);
}
void AssetOverrides_EarlyHooks_HWBP_ClearBP()
{
    if (g_HWBPHandle_DeserializeSingleObject)
    {
        RemoveHardwareBreakpoint(g_HWBPHandle_DeserializeSingleObject);
        g_HWBPHandle_DeserializeSingleObject = nullptr;
    }
    if (g_HWBPHandle_DeserializeSingleObjectFull)
    {
        RemoveHardwareBreakpoint(g_HWBPHandle_DeserializeSingleObjectFull);
        g_HWBPHandle_DeserializeSingleObjectFull = nullptr;
    }
    if (g_HWBPHandle_NewForgeAdded)
    {
        RemoveHardwareBreakpoint(g_HWBPHandle_NewForgeAdded);
        g_HWBPHandle_NewForgeAdded = nullptr;
    }
}
class ManagedObject;
class DeserializationStream;
void DeserializeManagedObject_FullReplacement(DeserializationStream* deserState, ManagedObject** pManObj);
LONG HWBP_SingleObjectOverride_HandleException(::_EXCEPTION_POINTERS* ExceptionInfo)
{
    void* ExceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;
    if (ExceptionAddress == g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Callsite0)
    {
        CONTEXT* ContextRecord = ExceptionInfo->ContextRecord;
        ContextRecord->EFlags |= EFLAGS_MASK_RF;

        DeserializeManagedObject_FullReplacement(
            (DeserializationStream*)ContextRecord->Rcx,
            (ManagedObject**)ContextRecord->Rdx
        );
        ContextRecord->Rip += 5;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else if (ExceptionAddress == g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Prologue)
    {
        ExceptionInfo->ContextRecord->EFlags |= EFLAGS_MASK_RF;
        ExceptionInfo->ContextRecord->Rip = (DWORD64)&DeserializeManagedObject_FullReplacement;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else if (ExceptionAddress == g_CodeAddr_DeserializeSingleManagedObject_Full)
    {
        ExceptionInfo->ContextRecord->EFlags |= EFLAGS_MASK_RF;
        CONTEXT* ContextRecord = ExceptionInfo->ContextRecord;
        void WhenStartStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(uint64 rax, uint64 rbx, uint64 rsi, uint64 rbp, uint64 rsp, uint64 r13);
        WhenStartStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(
            ContextRecord->Rax,
            ContextRecord->Rbx,
            ContextRecord->Rsi,
            ContextRecord->Rbp,
            ContextRecord->Rsp,
            ContextRecord->R13
        );
        ExceptionInfo->ContextRecord->Rip = 0x1426F0DC7;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else if (ExceptionAddress == g_CodeAddr_WhenGameForgesJustOpened)
    {
        ExceptionInfo->ContextRecord->EFlags |= EFLAGS_MASK_RF;
        void AssetOverrides_PutForgesInCorrectOrder(); AssetOverrides_PutForgesInCorrectOrder();
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

#include "AssetOverridesConfig.h"
void AssetOverrides_EarlyHooks_Start()
{
    if (!g_AssetOverridesConfig.enableOnStartup) return;
    g_EarlyHooks_SingleObjectOverride_VEHandler.emplace(1, HWBP_SingleObjectOverride_HandleException);
    AssetOverrides_EarlyHooks_HWBP_SetBP();
}
void AssetOverrides_EarlyHooks_End()
{
    AssetOverrides_EarlyHooks_HWBP_ClearBP();
    g_EarlyHooks_SingleObjectOverride_VEHandler.reset();
}
