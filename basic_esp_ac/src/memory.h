#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>




class memory
{
	
public:
	DWORD GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
	uintptr_t readMemory(DWORD addy, HANDLE handle);

};

