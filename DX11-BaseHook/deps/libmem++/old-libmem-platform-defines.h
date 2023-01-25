#pragma once

//Operating System

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__) && !defined(linux)) || (defined(MEM_FORCE_WIN) && !defined(MEM_FORCE_LINUX))
#define MEM_WIN
#elif (defined(linux) || defined(__linux__)) || defined(MEM_FORCE_LINUX)
#define MEM_LINUX
#endif

//Architecture

#if (defined(_M_IX86) || defined(__i386__) || __WORDSIZE == 32) || (defined(MEM_FORCE_86) && !defined(MEM_FORCE_64))
#define MEM_86
#elif (defined(_M_X64) || defined(__LP64__) || defined(_LP64) || __WORDSIZE == 64) || defined(MEM_FORCE_64)
#define MEM_64
#endif

//Charset

#if defined(_UNICODE) && defined(MEM_WIN)
#define MEM_UCS
#else
#define MEM_MBCS
#endif
