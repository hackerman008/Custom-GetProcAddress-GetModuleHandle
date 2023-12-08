#include <stdio.h>
#include <stdlib.h>
#include "Custom.h"

//void* GetProcAddress__(void*, char*, size_t );
//void* GetDllBase__(wchar_t*, size_t);

int main(){

    wchar_t* dllName = L"kernel32.dll";
    size_t nameLength = wcslen(dllName);
    void* dllBase;
    dllBase = GetDllBase__(dllName, nameLength);
	
	char* apiName = "EnumCalendarInfoExW";
	size_t apiNameLength = strlen(apiName);
	void* apiBase;
	apiBase = GetProcAddress__(dllBase, apiName, apiNameLength);

	apiName = "EnumCalendarInfoExA";
	apiNameLength = strlen(apiName);
	apiBase = GetProcAddress__(dllBase, apiName, apiNameLength);
	
	apiName = "VirtualAlloc";
	apiNameLength = strlen(apiName);
	apiBase = GetProcAddress__(dllBase, apiName, apiNameLength);

	apiName = "VirtualAllocEx";
	apiNameLength = strlen(apiName);
	apiBase = GetProcAddress__(dllBase, apiName, apiNameLength);

	
    dllName = L"ntdll.dll";
    nameLength = wcslen(dllName);
    dllBase = GetDllBase__(dllName, nameLength);

	apiName = "EtwEventWrite";
	apiNameLength = strlen(apiName);
	apiBase = GetProcAddress__(dllBase, apiName, apiNameLength);
	
	apiName = "EtwEventWriteEx";
	apiNameLength = strlen(apiName);
	apiBase = GetProcAddress__(dllBase, apiName, apiNameLength);
	
	

    return 0;
}