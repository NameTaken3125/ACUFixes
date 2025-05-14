﻿// HwBpLib.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#pragma warning(push)
#include <Windows.h>
#include <cstddef>
#include <algorithm>
#include <array>
#include <cassert>
#include <bitset>
#pragma warning(pop)

namespace HwBp
{
    enum class Result
    {
        Success,
        CantGetThreadContext,
        CantSetThreadContext,
        NoAvailableRegisters,
        BadWhen, // Unsupported value of When passed
        BadSize // Size can only be 1, 2, 4, 8
    };

    enum class When
    {
        ReadOrWritten,
        Written,
        Executed
    };

    struct Breakpoint
    {
        static constexpr Breakpoint MakeFailed(Result result)
        {
            return {
                0,
                result
            };
        }

        const std::uint8_t m_registerIndex;
        const Result m_error;
    };

    namespace Detail
    {
        template <typename TAction, typename TFailure>
        auto UpdateThreadContext(TAction action, TFailure failure)
        {
            CONTEXT ctx{0};
            ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
            if (::GetThreadContext(::GetCurrentThread(), &ctx) == FALSE)
            {
                return failure(Result::CantGetThreadContext);
            }

            std::array<bool, 4> busyDebugRegister{{false, false, false, false}};
            auto checkBusyRegister = [&](std::size_t index, DWORD64 mask)
            {
                if (ctx.Dr7 & mask)
                    busyDebugRegister[index] = true;
            };

            checkBusyRegister(0, 1);
            checkBusyRegister(1, 4);
            checkBusyRegister(2, 16);
            checkBusyRegister(3, 64);

            const auto actionResult = action(ctx, busyDebugRegister);

            if (::SetThreadContext(::GetCurrentThread(), &ctx) == FALSE)
            {
                return failure(Result::CantSetThreadContext);
            }

            return actionResult;
        }
    }

    inline Breakpoint Set(const void* onPointer, std::uint8_t size, When when)
    {
        return Detail::UpdateThreadContext(
            [&](CONTEXT& ctx, const std::array<bool, 4> & busyDebugRegister) -> Breakpoint
            {
                const auto found = std::find(begin(busyDebugRegister), end(busyDebugRegister), false);
                if (found == end(busyDebugRegister))
                {
                    return Breakpoint::MakeFailed(Result::NoAvailableRegisters);
                }

                const auto registerIndex = static_cast<std::uint16_t>(std::distance(begin(busyDebugRegister), found));

                switch (registerIndex)
                {
                case 0:
                    ctx.Dr0 = reinterpret_cast<DWORD_PTR>(const_cast<void*>(onPointer));
                    break;
                case 1:
                    ctx.Dr1 = reinterpret_cast<DWORD_PTR>(const_cast<void*>(onPointer));
                    break;
                case 2:
                    ctx.Dr2 = reinterpret_cast<DWORD_PTR>(const_cast<void*>(onPointer));
                    break;
                case 3:
                    ctx.Dr3 = reinterpret_cast<DWORD_PTR>(const_cast<void*>(onPointer));
                    break;
                default:
                    assert(!"Impossible happened - searching in array of 4 got index < 0 or > 3");
                    std::exit(EXIT_FAILURE);
                }

                std::bitset<sizeof(ctx.Dr7) * 8> dr7;
                std::memcpy(&dr7, &ctx.Dr7, sizeof(ctx.Dr7));

                dr7.set(registerIndex * 2); // Flag to enable 'local' debugging for each of 4 registers. Second bit is for global debugging, not working.
                
                switch (when)
                {
                case When::ReadOrWritten:
                    dr7.set(16 + registerIndex * 4 + 1, true);
                    dr7.set(16 + registerIndex * 4, true);
                    break;

                case When::Written:
                    dr7.set(16 + registerIndex * 4 + 1, false);
                    dr7.set(16 + registerIndex * 4, true);
                    break;

                case When::Executed:
                    dr7.set(16 + registerIndex * 4 + 1, false);
                    dr7.set(16 + registerIndex * 4, false);
                    break;

                default:
                    return Breakpoint::MakeFailed(Result::BadWhen);
                }

                switch (size)
                {
                case 1:
                    dr7.set(16 + registerIndex * 4 + 3, false);
                    dr7.set(16 + registerIndex * 4 + 2, false);
                    break;

                case 2:
                    dr7.set(16 + registerIndex * 4 + 3, false);
                    dr7.set(16 + registerIndex * 4 + 2, true);
                    break;

                case 8:
                    dr7.set(16 + registerIndex * 4 + 3, true);
                    dr7.set(16 + registerIndex * 4 + 2, false);
                    break;

                case 4:
                    dr7.set(16 + registerIndex * 4 + 3, true);
                    dr7.set(16 + registerIndex * 4 + 2, true);
                    break;

                default:
                    return Breakpoint::MakeFailed(Result::BadSize);
                }

                std::memcpy(&ctx.Dr7, &dr7, sizeof(ctx.Dr7));

                return Breakpoint{ static_cast<std::uint8_t>(registerIndex), Result::Success };
            },
            [](auto failureCode)
            {
                return Breakpoint::MakeFailed(failureCode);
            }
        );
    }

    inline void Remove(const Breakpoint& bp)
    {
        if (bp.m_error != Result::Success)
        {
            return;
        }

        Detail::UpdateThreadContext(
            [&](CONTEXT& ctx, const std::array<bool, 4>&) -> Breakpoint
            {
                std::bitset<sizeof(ctx.Dr7) * 8> dr7;
                std::memcpy(&dr7, &ctx.Dr7, sizeof(ctx.Dr7));

                dr7.set(bp.m_registerIndex * 2, false); // Flag to enable 'local' debugging for each of 4 registers. Second bit is for global debugging, not working.
                
                std::memcpy(&ctx.Dr7, &dr7, sizeof(ctx.Dr7));

                return Breakpoint{};
            },
            [](auto failureCode)
            {
                return Breakpoint::MakeFailed(failureCode);
            }
        );
    }
}