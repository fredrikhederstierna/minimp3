#ifndef __LIBC_H_INCLUDED__
#define __LIBC_H_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

#define libc_memset  memset
#define libc_memcpy  memcpy
#define libc_memmove memmove

#define libc_frexpf  frexpf

#define libc_assert(a) do { if (!(a)) { printf("ASSERT! FILE: %s LINE: %d\n", __FILE__, __LINE__); while(1); } } while(0)

#define libc_abort() do { printf("ABORT! FILE: %s LINE: %d\n", __FILE__, __LINE__); while(1); } while(0)

#endif//__LIBC_H_INCLUDED__
