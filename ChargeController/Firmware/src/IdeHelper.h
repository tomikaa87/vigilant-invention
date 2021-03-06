#pragma once

// These macros help Visual Studio's IntelliSense to
// parse GCC's headers correctly.

#ifdef _MSC_VER
#define __asm__(x)
#define __extension__(x)
#define __attribute__(x)
#define __builtin_va_list int
#define __extension__
#define __inline__
#define __builtin_constant_p
#define _Bool bool
#undef __cplusplus
#define __cplusplus 201103L
#endif