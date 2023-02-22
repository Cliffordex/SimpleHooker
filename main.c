#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>

const wchar_t* procName = NULL; // L"Silver.exe";
const char* dllPath = NULL; // HookingDebugConsole.dll

void HandleArgs(int argc, char* argv[]) {
	procName = malloc(sizeof(wchar_t) * strlen(argv[1]));
	mbstowcs(procName, argv[1], strlen(argv[1]));

	// Don't need to copy, argv never goes out of scope.
	dllPath = argv[2];
}

int PrintHelp() {
	printf("SimpleHooker Usage:\n");
	printf("SimpleHooker.exe <process_name> <dll_path>\n");

	return 1;
}

DWORD GetProcId(const WCHAR* procName) {
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		ZeroMemory(&procEntry, sizeof(procEntry));
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) {
			do {
				if (!_wcsicmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);
	return procId;
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("Invalid number of arguments.\n");
		return PrintHelp();
	}

	HandleArgs(argc, argv);

	if (procName == NULL)
		return PrintHelp();
	if (dllPath == NULL)
		return PrintHelp();

	DWORD procId = 0;

	printf("Waiting for process... ");
	while (!procId)
	{
		procId = GetProcId(procName);
		Sleep(50);
	}
	printf("Process found.\n");

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

	if (hProc && hProc != INVALID_HANDLE_VALUE) {
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (loc) {
			WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, NULL);
			HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
			if (hThread) CloseHandle(hThread);
		}
		else {
			printf("VirtualAllocEx failed.\n");
		}
	}

	if (hProc)
		CloseHandle(hProc);

	return 0;
}

