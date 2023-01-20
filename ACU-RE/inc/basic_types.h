#pragma once

typedef unsigned char		byte;		// 8 bits
typedef unsigned short		word;		// 16 bits
typedef unsigned int		dword;		// 32 bits
typedef unsigned int		uint;
typedef unsigned long		ulong;

typedef signed char			int8;
typedef unsigned char		uint8;
typedef short int			int16;
typedef unsigned short int	uint16;
typedef int					int32;
typedef unsigned int		uint32;
typedef long long			int64;
typedef unsigned long long	uint64;

template<bool> struct compile_time_assert_failed;
template<> struct compile_time_assert_failed<true> {};
template<int x> struct compile_time_assert_test {};
#define compile_time_assert_join2( a, b )	a##b
#define compile_time_assert_join( a, b )	compile_time_assert_join2(a,b)
#define compile_time_assert( x )			typedef compile_time_assert_test<sizeof(compile_time_assert_failed<(bool)(x)>)> compile_time_assert_join(compile_time_assert_typedef_, __LINE__)
#define assert_sizeof( type, size )						compile_time_assert( sizeof( type ) == size )
#define assert_offsetof( type, field, offset )			compile_time_assert( offsetof( type, field ) == offset )

// The C/C++ standard guarantees the size of an unsigned type is the same as the signed type.
// The exact size in bytes of several types is guaranteed here.
assert_sizeof(bool, 1);
assert_sizeof(char, 1);
assert_sizeof(short, 2);
assert_sizeof(int, 4);
assert_sizeof(float, 4);
assert_sizeof(byte, 1);
assert_sizeof(int8, 1);
assert_sizeof(uint8, 1);
assert_sizeof(int16, 2);
assert_sizeof(uint16, 2);
assert_sizeof(int32, 4);
assert_sizeof(uint32, 4);
assert_sizeof(int64, 8);
assert_sizeof(uint64, 8);
