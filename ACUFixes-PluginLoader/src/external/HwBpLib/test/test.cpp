#include "HwBpLib.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>

#include <Windows.h>

namespace
{
    struct GlobalState
    {
        static GlobalState& Get()
        {
            static GlobalState state;
            return state;
        }

        void Reset()
        {
            ResetBreakpoints();
        }

        void BreakpointHit()
        {
            ++m_breakPointFound;
        }

        int GetBp()
        {
            return m_breakPointFound;
        }

    private:
        GlobalState()
        {
            ResetBreakpoints();
        }

        void ResetBreakpoints()
        {
            m_breakPointFound = 0;
        }
         
        int m_breakPointFound;
    };

    struct Test
    {
        using TestFunc = void (*)();

        TestFunc m_func = nullptr;
        std::vector<std::ostringstream> m_output;
        bool m_success = true;
    };

    std::unordered_map<std::string, Test> g_tests;
    Test* g_currentTest;

    struct Registrar
    {
        Registrar(const char* name, Test::TestFunc func)
        {
            g_tests[name].m_func = func;
        }
    };

    struct ExceptionHandler
    {
        static constexpr ULONG c_callThisHandlerFirst = 1;

        ExceptionHandler(PVECTORED_EXCEPTION_HANDLER handler)
        {
            m_handle = ::AddVectoredExceptionHandler(c_callThisHandlerFirst, handler);
        }

        bool IsValid() const noexcept
        {
            return m_handle != nullptr;
        }

        ~ExceptionHandler()
        {
            if (m_handle)
            {
                ::RemoveVectoredExceptionHandler(m_handle);
            }
        }

        void* m_handle;
    };

    LONG MyHandler(_EXCEPTION_POINTERS* ExceptionInfo)
    {
        if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
        {
            GlobalState::Get().BreakpointHit();
        }

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    void AssertTrue(const char* str, bool result, int line, const char* file)
    {
        if (!result)
        {
            g_currentTest->m_success = false;
            g_currentTest->m_output.emplace_back();
            g_currentTest->m_output.back() << "Assertion failed (" << file << ": " << line << "): " << str;
        }
    }

#define ASSERT(Expr_) \
    AssertTrue(#Expr_, (Expr_), __LINE__, __FILE__);

#define REQUIRE(Expr_) \
    AssertTrue(#Expr_, (Expr_), __LINE__, __FILE__); \
    ReportResults(); \
    std::exit(EXIT_FAILURE);

#define TEST(Name_) \
    void Name_##TestFunc(); \
    Registrar Name_##Registrar{#Name_, Name_##TestFunc}; \
    void Name_##TestFunc() \

    int ReportResults()
    {
        int testsFailed = 0;
        for (auto&& t : g_tests)
        {
            if (!t.second.m_success)
            {
                std::cout << "Test '" << t.first << "' failed:\n";
                for (auto&& error : t.second.m_output)
                {
                    std::cout << "    '" << error.str() << "'\n";
                }
                ++testsFailed;
            }
        }

        if (testsFailed == 0)
        {
            std::cout << "Success!\n";
            return EXIT_SUCCESS;
        }
        else
        {
            std::cout << testsFailed << " tests failed!\n";
            return EXIT_FAILURE;
        }
    }
}

TEST(OneBp)
{
    GlobalState::Get().Reset();

    int var = 42;
    auto bp = HwBp::Set(&var, sizeof(42), HwBp::When::Written);

    {
        ExceptionHandler h{MyHandler};
        ASSERT(h.IsValid());

        var = 33;
        
        ASSERT(GlobalState::Get().GetBp() == 1);
    }

    HwBp::Remove(bp);
}

TEST(FourBps)
{
    GlobalState::Get().Reset();

    int var[4] = {42, 43, 44, 45};
    HwBp::Breakpoint bps[4] = {
        HwBp::Set(&var[0], sizeof(var[0]), HwBp::When::Written),
        HwBp::Set(&var[1], sizeof(var[0]), HwBp::When::Written),
        HwBp::Set(&var[2], sizeof(var[0]), HwBp::When::Written),
        HwBp::Set(&var[3], sizeof(var[0]), HwBp::When::Written)
    };

    {
        ExceptionHandler h { MyHandler };
        
        var[0] = 30;
        ASSERT(bps[0].m_registerIndex == 0);
        ASSERT(bps[0].m_error == HwBp::Result::Success);
        ASSERT(GlobalState::Get().GetBp() == 1);

        var[1] = 31;
        ASSERT(bps[1].m_registerIndex == 1);
        ASSERT(GlobalState::Get().GetBp() == 2);

        var[2] = 32;
        ASSERT(bps[2].m_registerIndex == 2);
        ASSERT(GlobalState::Get().GetBp() == 3);

        var[3] = 33;
        ASSERT(bps[3].m_registerIndex == 3);
        ASSERT(GlobalState::Get().GetBp() == 4);
    }

    for (auto&& bp : bps)
    {
        HwBp::Remove(bp);
    }
}

TEST(FifthBpFails_ButWorksAfter3rdWasReleased)
{
    int var{0};
    const HwBp::Breakpoint bps[5] = {
        HwBp::Set(&var, sizeof(var), HwBp::When::Written),
        HwBp::Set(&var, sizeof(var), HwBp::When::Written),
        HwBp::Set(&var, sizeof(var), HwBp::When::Written),
        HwBp::Set(&var, sizeof(var), HwBp::When::Written),
        HwBp::Set(&var, sizeof(var), HwBp::When::Written)
    };

    ASSERT(bps[4].m_error == HwBp::Result::NoAvailableRegisters);

    HwBp::Remove(bps[2]);

    auto bp5Retry = HwBp::Set(&var, sizeof(var), HwBp::When::Written);
    ASSERT(bp5Retry.m_error == HwBp::Result::Success);
    ASSERT(bp5Retry.m_registerIndex == bps[2].m_registerIndex);

    HwBp::Remove(bps[0]);
    HwBp::Remove(bps[1]);
    HwBp::Remove(bp5Retry);
    HwBp::Remove(bps[3]);
}

int main()
{
    for (auto&& t : g_tests)
    {
        g_currentTest = &t.second;
        t.second.m_func();
        g_currentTest = nullptr;
    }

    return ReportResults();
}