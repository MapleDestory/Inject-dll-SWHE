#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>

#include <Windows.h>

#include <stdio.h>
#include <tchar.h>

namespace fs = std::experimental::filesystem;

int main(int, char**)
{
	HMODULE		hModule		= nullptr;
	HOOKPROC	hAddress	= nullptr;
	HHOOK		hHandle		= nullptr;
	DWORD		dwTid		= 0;
	DWORD		dwPid		= 0;
	HWND		hParent		= nullptr;
	bool		bStatus		= false;

	const auto& dllpath = fs::path(L"D:\\Projects\\Visual C++\\Games\\Point Blank\\Release\\image.dll");
	//const auto& dllpath = fs::path(L"D:\\Program Files\\Fraps\\fraps32.dll");

	hParent = FindWindowA("PBApp", nullptr);
	if (!hParent)
	{
		bStatus = true;
		std::cout << "[!] Could not find target window." << std::endl;
		goto FreeExit;
	}

	dwTid = GetWindowThreadProcessId(hParent, &dwPid);
	if (!dwTid)
	{
		bStatus = true;
		std::cout << "[!] Could not get thread ID of the target window." << std::endl;
		goto FreeExit;
	}

	if (!fs::exists(dllpath))
	{
		bStatus = true;
		std::cout << "[!] Files not exists." << std::endl;
		goto FreeExit;
	}

	hModule = LoadLibraryExW(dllpath.generic_wstring().c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
	if (!hModule)
	{
		bStatus = true;
		std::cout << "[!] The dll could not be found." << std::endl;
		goto FreeExit;
	}

	if (!hAddress)
		hAddress = reinterpret_cast<HOOKPROC>(GetProcAddress(hModule, "FrapsProcCALLWND"));

	if (!hAddress)
	{
		bStatus = true;
		std::cout << "[!] The function was not found." << std::endl;
		goto FreeExit;
	}

	hHandle = SetWindowsHookEx(WH_GETMESSAGE, hAddress, hModule, dwTid);
	if (!hHandle)
	{
		bStatus = true;
		std::cout << "[!] Couldn't set the hook with SetWindowsHookEx." << std::endl;
		goto FreeExit;
	}

	PostThreadMessage(dwTid, WM_NULL, NULL, NULL);

	std::cout << "[+] Inject dll successfully." << std::endl;
	std::cout << "[!] Press any key to unhook (This will unload the DLL)." << std::endl;

	system("pause > nul");

	if (!UnhookWindowsHookEx(hHandle))
	{
		bStatus = true;
		std::cout << "Could not remove the hook." << std::endl;
		goto FreeExit;
	}

FreeExit:

	if (bStatus)
		std::cin.get();

	return -1;
}