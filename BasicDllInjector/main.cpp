#include <iostream>
#include <string>
using namespace std;

#include <Windows.h>
//#include <memoryapi.h>
//#include <processthreadsapi.h>
//#include <libloaderapi.h>

bool SetPrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable) {
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid);
	tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	return ((GetLastError() == ERROR_SUCCESS));
}

int main(int argc, char** argv) {
	string injection_file_name;

#ifdef NDEBUG
	if (argc >= 2) {
		injection_file_name = string(argv[1]);
	}
	else {
		cout << "Not enough startup arguments.\n";
		return 0;
	}
#else
	injection_file_name = "C:\\boot.dll";
#endif // DEBUG

	cout << "PID: ";

	int process_id;
	cin >> process_id;

	HANDLE hToken;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		cout << "Could not open process token\n";
		return 0;
	}
	else {
		if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE)) {
			cout << "Unable to set privilege for current process\n";
			return 0;
		}
	}

	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, process_id);
	LPVOID base_address = VirtualAllocEx(process_handle, 0, injection_file_name.size() + 1, 4096, 4);

	SIZE_T successful_bytes_written = -1;
	WriteProcessMemory(process_handle, base_address, injection_file_name.c_str(), injection_file_name.size() + 1, &successful_bytes_written);

	FARPROC proc_address = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	DWORD created_thread_id = -1;
	HANDLE thread_handle = CreateRemoteThread(process_handle, 0, 0, (LPTHREAD_START_ROUTINE)proc_address, base_address, 0, &created_thread_id);

	cout << "Successful bytes written: " << successful_bytes_written << "\n";
	cout << "Thread handle: " << thread_handle << "\n";
	cout << "Thread id: " << created_thread_id << "\n";

	WaitForSingleObject(thread_handle, INFINITE);

	CloseHandle(thread_handle);
	CloseHandle(process_handle);

	return 0;
}