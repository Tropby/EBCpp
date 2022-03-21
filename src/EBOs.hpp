#pragma once

#ifdef __GNUC__

// For Version 3.2.0 is 30200
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// Check for windows compiler
#ifndef __WIN32__
#ifdef _WIN32
#define __WIN32__
#endif
#endif

#endif

#ifdef __WIN32__

// Winsock 2 before windows.h
#include <winsock2.h>

// Windows
#include <windows.h>

#endif