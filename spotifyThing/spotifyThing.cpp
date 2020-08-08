#define CURL_STATICLIB
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <sstream>
#include <string>

#include "curl/curl.h"

using namespace std;

bool process_exists(const wchar_t* name, uint32_t& pid)
{
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot == INVALID_HANDLE_VALUE)
		return false;

	auto entry = PROCESSENTRY32{ sizeof(PROCESSENTRY32) };

	if (Process32First(snapshot, &entry)) {
		do {
			if (!wcscmp(entry.szExeFile, name)) {
				pid = entry.th32ProcessID;
				CloseHandle(snapshot);
				return true;
			}
		} while (Process32Next(snapshot, &entry));
	}
	CloseHandle(snapshot);
	return false;
}

size_t writeFunction(void* ptr, size_t size, size_t nmemb, string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

bool finder_function(string& str) {
	string desired = "ad\"";
	int ind = str.find(desired);
	if (ind != -1)
		return true;
	return false;
}

int main()
{
    bool running = true;
	const wchar_t* target = L"Spotify.exe";
	unsigned id = 0;
	STARTUPINFO start_info = { 0 };
	PROCESS_INFORMATION proc_info= { 0 };
	cout << "Started Process . . . " << endl;
		
    while (running) {
		if (!process_exists(target, id)) {				
			CreateProcess(TEXT("C:\\Users\\athannnnnn\\AppData\\Roaming\\Spotify\\spotify.exe"), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &start_info, &proc_info);

			Sleep(3000);
				
			// play song after reopening
			INPUT inp = { 0 };
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_MEDIA_PLAY_PAUSE;
			SendInput(1, &inp, sizeof(inp));

			ZeroMemory(&inp, sizeof(inp));

			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &inp, sizeof(inp));
			
			Sleep(3000);
		}
		else {
			string result;
			CURL* curl;
			CURLcode res;
			curl_global_init(CURL_GLOBAL_DEFAULT);

			curl = curl_easy_init();
			if (curl) {

				curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player/currently-playing");
				curl_slist* headers = NULL;
				headers = curl_slist_append(headers, "Accept: application/json");
				headers = curl_slist_append(headers, "Content-Type: application/json");
				headers = curl_slist_append(headers, "Authorization: Bearer BQDm_c_e9odiFud4ZHUQs-yvi1XEOaAAOYEWqR1FxgwoG7l3yKgK_E3BPl2VcG4dgNjUieb4os9513Pao6LgX3AF6Iq-_V4nRz9NQZtXWkEXcV82MTa74n8LZ8TS8JE5EmC2wKTgXLtamdp1tLLw07ICJ4eW");
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
				//curl_easy_perform(curl);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
				//curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

				//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

				res = curl_easy_perform(curl);

				curl_easy_cleanup(curl);
				if (CURLE_OK != res) {
					cerr << "CURL error: " << res << endl;
				}
			}
			curl_global_cleanup();
			bool should_close = finder_function(result);
			if (should_close) {
				PROCESSENTRY32 entry = PROCESSENTRY32{ sizeof(PROCESSENTRY32) };
				HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

				bool handleFound = Process32First(snapshot, &entry);
				while (handleFound) {
					if (!wcscmp(entry.szExeFile, L"Spotify.exe"))
					{
						HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

						if (hProcess) {
							TerminateProcess(hProcess, 9);

							CloseHandle(hProcess);
							break;
						}
					}
					handleFound = Process32Next(snapshot, &entry);
				}
				CloseHandle(snapshot);
			}

			if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) {
				running = false;
				cout << "Stopped running" << endl;
			}
		}

        Sleep(1);
    }	
    return 0;
}

