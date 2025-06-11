#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

#include "memory/memory.h"
#include "offsets/offsets.h"





int main()
{
	//declare memory instance
	memory mem;

	//get processID
	HWND hwnd = FindWindowA(0, "AssaultCube");
	DWORD pID;
	GetWindowThreadProcessId(hwnd, &pID);



	//get baseAddress
	DWORD baseAddr;
	baseAddr = mem.GetModuleBaseAddress(pID, L"ac_client.exe");

	std::cout << baseAddr << "\n\n";


	//get HANDLE with ALL_ACCESS
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, pID);



	uintptr_t locPlayer = mem.readMemory(baseAddr + offsets::localPlayer, handle);
	uintptr_t locHealth = mem.readMemory(locPlayer + offsets::health, handle);
	std::cout << "health: " << locHealth << "\n\n";
	

	uintptr_t entityList = mem.readMemory(baseAddr + offsets::entityList, handle);
	uintptr_t currentEntity;
	char name[32];
	int entHealth;
	
	//loop entList
	for (int i = 1; i < 8; i++)
	{
		currentEntity = mem.readMemory(entityList + 0x4 * i, handle);
		entHealth = mem.readMemory(currentEntity + offsets::health, handle);
		ReadProcessMemory(handle, (LPCVOID)(currentEntity + offsets::name), &name, sizeof(name), nullptr);


		std::cout << "Name: " << name << "		| Health: " << entHealth << "\n";

	}



}