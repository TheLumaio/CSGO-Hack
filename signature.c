#include "signature.h"

bool Compare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)   return 0;
	return (*szMask) == NULL;
}
DWORD Pattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
{
	for (DWORD i = 0; i < dwLen; i++)
		if (Compare((BYTE*)(dwAddress + i), bMask, szMask))  return (DWORD)(dwAddress + i);
	return 0;
}

DWORD FindPattern(char* pattern, char* mask, DWORD start, DWORD end, DWORD offset)
{
	int patternLength = strlen(mask);
	bool found = false;

	//For each byte from start to end
	for (DWORD i = start; i < end - patternLength; i++)
	{
		found = true;
		//For each byte in the pattern
		for (int idx = 0; idx < patternLength; idx++)
		{
			if (mask[idx] == 'x' && pattern[idx] != *(char*)(i + idx))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return i + offset;
		}
	}
	return NULL;
}

DWORD GetModuleSize(char* moduleName)
{
	HANDLE hSnap;
	MODULEENTRY32 xModule;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(GetCurrentProcess()));
	xModule.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnap, &xModule)) {
		while (Module32Next(hSnap, &xModule)) {
			if (!strncmp((char*)xModule.szModule, moduleName, 8)) {
				CloseHandle(hSnap);
				return (DWORD)xModule.modBaseSize;
			}
		}
	}
	CloseHandle(hSnap);
	return 0;
}
