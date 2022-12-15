#include<iostream>
#include <windows.h>
#include<winbase.h>
#include <tlhelp32.h>
#include <iomanip>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

typedef unsigned long long llong;

struct process {
    const DWORD ID;
    const std::wstring NAME;

    process(DWORD id, std::wstring name) : ID(id), NAME(name) {};
};

class ProcessHandler {
    const LPCSTR killer_path = "D:\\OS\\KillerUser\\Killer\\Debug\\Killer.exe";
public:
    std::vector<process> getActiveProcesses() {
        std::vector<process> processes;
        HANDLE snShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pEntry;
        pEntry.dwSize = sizeof(PROCESSENTRY32);
        auto hRes = Process32First(snShot, &pEntry);
        while (hRes) {
            processes.push_back(process(pEntry.th32ProcessID, std::wstring(pEntry.szExeFile)));
            hRes = Process32Next(snShot, &pEntry);
        }
        CloseHandle(snShot);
        return processes;
    }

    void killerProcess(std::string command) {
        std::string str_ = command;
        char *proc = (char *) command.c_str();
        STARTUPINFOA startUpInf = {0};
        PROCESS_INFORMATION procInf = {0};
        if (CreateProcessA(killer_path, proc, NULL, NULL, FALSE, 0, NULL, NULL, &startUpInf, &procInf)) {
            WaitForSingleObject(procInf.hProcess, INFINITE);
            CloseHandle(procInf.hProcess);
            CloseHandle(procInf.hThread);
        }
    }
};

void printProcesses(std::vector<process> vec) {
    for (auto x : vec) {
        std::wcout << "\nID:  " << x.ID << "  NAME:  " << x.NAME;
    }
}


int main() {
    ProcessHandler handler;
    SetEnvironmentVariableA("PROC_TO_KILL", "Telegram,notepad++");
    auto currentActiveProcesses = handler.getActiveProcesses();
    for (auto x : currentActiveProcesses) {
        if (x.NAME == L"Telegram.exe" || x.NAME == L"notepad++.exe") {
            std::wcout << '\n' << x.NAME << " process is active";
        }
    }
    handler.killerProcess("");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    _putenv("PROC_TO_KILL");
    bool killed = true;
    auto check = handler.getActiveProcesses();
    for (auto x : check) {
        if (x.NAME == L"Telegram.exe" || x.NAME == L"notepad++.exe") {
            std::wcout << '\n' << x.NAME << " process was not terminated";
            killed = false;
        }
    }
    if (killed) {
        std::cout << "\nProcesses are successfuly terminated\n";
    }

    std::string proc;
    std::cin >> proc;
    if (proc == "end") return 0;
    handler.killerProcess(proc);

    return 0;
}