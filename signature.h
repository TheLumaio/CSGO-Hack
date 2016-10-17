#ifndef SIGNATURE_H
#define SIGNATURE_H
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <tlhelp32.h>

bool Compare(const BYTE* pData, const BYTE* bMask, const char* szMask);
DWORD Pattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask);

DWORD FindPattern(char* pattern, char* mask, DWORD start, DWORD end, DWORD offset);

DWORD GetModuleSize(char* moduleName);

#endif /* end of include guard: SIGNATURE_H */
