#include <iostream>
#include "memory.h"




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



	uintptr_t locPlayer = mem.readMemory(baseAddr + 0x17E0A8, handle);

	uintptr_t locHealth = mem.readMemory(locPlayer + 0xEC, handle);

	std::cout << "health: " << locHealth << "\n";
	




}