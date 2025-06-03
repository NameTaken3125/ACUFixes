#pragma once

struct VEHandler
{
    VEHandler(ULONG First, PVECTORED_EXCEPTION_HANDLER Handler)
    {
        m_handle = ::AddVectoredExceptionHandler(First, Handler);
    }

    bool IsValid() const noexcept
    {
        return m_handle != nullptr;
    }

    ~VEHandler()
    {
        if (m_handle)
        {
            ::RemoveVectoredExceptionHandler(m_handle);
        }
    }

    void* m_handle;
};

// When handling my Hardware Breakpoint exception, EXCEPTION_SINGLE_STEP gets reraised
// if I just return EXCEPTION_CONTINUE_EXECUTION.
// Cheat Engine's VEH debugger sets the RF bit ("Resume Flag").
// https://github.com/cheat-engine/cheat-engine/blob/ec45d5f47f92a239ba0bf51ec5d04a7509c3fd37/Cheat%20Engine/VEHDebug/DebugHandler.pas#L186
// This seems to do the right thing and continue execution.
constexpr const DWORD EFLAGS_MASK_RF = 0x10000;
