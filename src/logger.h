#ifndef __LOGGER_H__
#define __LOGGER_H__
#include <cstdio>

inline void PrintLog(const char* log) { fprintf(stderr, "%s\n", log); }

#endif