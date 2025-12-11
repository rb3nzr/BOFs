#include <windows.h>
#include "beacon.h"

#define ANY_SIZE 1
#define MAXLEN_PHYSADDR 8
#define MAXLEN_IFDESCR 256
#define MAX_INTERFACE_NAME_LEN 256
#define IF_TYPE_SOFTWARE_LOOPBACK 24

// interface types (non-exhaustive)
// https://learn.microsoft.com/en-us/windows-hardware/drivers/network/ndis-interface-types
#define IF_TYPE_OTHER                   1   
#define IF_TYPE_ETHERNET_CSMACD         6  
#define IF_TYPE_PPP                     23  
#define IF_TYPE_SOFTWARE_LOOPBACK       24  
#define IF_TYPE_PROP_VIRTUAL            53 
#define IF_TYPE_IEEE80211               71  
#define IF_TYPE_TUNNEL                  131 
#define IF_TYPE_L2VLAN                  135 
#define IF_TYPE_IEEE8023AD_LAG          161 
#define IF_TYPE_BRIDGE                  209
#define IF_TYPE_WWANPP                  243
#define IF_TYPE_WWANPP2                 244 

// operational status for GetIfTable (MIB_IFROW)
#define MIB_IF_OPER_STATUS_NON_OPERATIONAL  0
#define MIB_IF_OPER_STATUS_UNREACHABLE      1
#define MIB_IF_OPER_STATUS_DISCONNECTED     2
#define MIB_IF_OPER_STATUS_CONNECTING       3
#define MIB_IF_OPER_STATUS_CONNECTED        4
#define MIB_IF_OPER_STATUS_OPERATIONAL      5

// administrative status for GetIfTable (MIB_IFROW)
#define MIB_IF_ADMIN_STATUS_UP              1
#define MIB_IF_ADMIN_STATUS_DOWN            2
#define MIB_IF_ADMIN_STATUS_TESTING         3

typedef DWORD IFTYPE;

typedef struct _MIB_IFROW {
    WCHAR wszName[MAX_INTERFACE_NAME_LEN];
    DWORD dwIndex;
    DWORD dwType;
    DWORD dwMtu;
    DWORD dwSpeed;
    DWORD dwPhysAddrLen;
    BYTE  bPhysAddr[MAXLEN_PHYSADDR];
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
    BYTE  bDescr[MAXLEN_IFDESCR];
} MIB_IFROW, *PMIB_IFROW;

// Management Information Base (MIB) interface table
// contains table of interface entries
typedef struct _MIB_IFTABLE {
    DWORD dwNumEntries;
    MIB_IFROW table[ANY_SIZE];
} MIB_IFTABLE, *PMIB_IFTABLE;

DECLSPEC_IMPORT DWORD WINAPI IPHLPAPI$GetIfTable(PMIB_IFTABLE, PDWORD, BOOL);
DECLSPEC_IMPORT HLOCAL WINAPI KERNEL32$LocalFree(HLOCAL);
DECLSPEC_IMPORT HLOCAL WINAPI KERNEL32$LocalAlloc(UINT, SIZE_T);
DECLSPEC_IMPORT int MSVCRT$sprintf_s(char*, size_t, const char*, ...);
WINBASEAPI void *__cdecl MSVCRT$memcpy(void * __restrict__ _Dst,const void * __restrict__ _Src,size_t _MaxCount);
WINBASEAPI int WINAPI KERNEL32$WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);

VOID MAC2String(const BYTE *mac, DWORD len, char *out, size_t outsz) {
    if (!len) {
        out[0] = '\0'; 
        return;
    }

    size_t pos = 0;
    for (DWORD i = 0; i < len  && pos + 3 < outsz; i++) {
        int written = MSVCRT$sprintf_s(out + pos, outsz - pos, i ? "-%02X":"%02X", mac[i]);
        if (written > 0) pos += written;
    }
}

VOID Speed2String(DWORD bps, char *out, size_t outsz) {
    if (bps >= 1000000000) {
        MSVCRT$sprintf_s(out, outsz, "%u Gbps", bps / 1000000000);
    } else if (bps >= 1000000) {
        MSVCRT$sprintf_s(out, outsz, "%u Mbps", bps / 1000000);
    } else if (bps >= 1000) {
        MSVCRT$sprintf_s(out, outsz, "%u Kbps", bps / 1000);
    } else {
        MSVCRT$sprintf_s(out, outsz, "%u bps", bps);
    }
}

const char* MapStatusIFTable(DWORD admin_status, DWORD oper_status) {
    if (admin_status != MIB_IF_ADMIN_STATUS_UP) {
        switch (admin_status) {
            case MIB_IF_ADMIN_STATUS_DOWN:
                return "Administratively Down";
            case MIB_IF_ADMIN_STATUS_TESTING:
                return "Administratively Testing";
            default:
                return "Administratively Unknown";
        }
    }
    
    switch (oper_status) {
        case MIB_IF_OPER_STATUS_NON_OPERATIONAL:
            return "Non Operational";
        case MIB_IF_OPER_STATUS_UNREACHABLE:
            return "Unreachable";
        case MIB_IF_OPER_STATUS_DISCONNECTED:
            return "Disconnected";
        case MIB_IF_OPER_STATUS_CONNECTING:
            return "Connecting";
        case MIB_IF_OPER_STATUS_CONNECTED:
            return "Connected";
        case MIB_IF_OPER_STATUS_OPERATIONAL:
            return "Operational";
        default:
            return "Unknown Operational Status";
    }
}

const char* MapIfType(IFTYPE type) {
    if (type == IF_TYPE_OTHER)
        return "IF_TYPE_OTHER";
    if (type == IF_TYPE_ETHERNET_CSMACD)
        return "Ethernet";
    if (type == IF_TYPE_PPP)
        return "Point to Point";
    if (type == IF_TYPE_SOFTWARE_LOOPBACK)
        return "Software Loopback";
    if (type == IF_TYPE_PROP_VIRTUAL)
        return "Virtual/Internal";
    if (type == IF_TYPE_IEEE80211)
        return "IEEE 802.11 Wireless";
    if (type == IF_TYPE_TUNNEL)
        return "Tunnel type encapsulation";
    if (type == IF_TYPE_L2VLAN)
        return "L2 VLAN - 802.1Q";
    if (type == IF_TYPE_IEEE8023AD_LAG)
        return "NIC Teaming";
    if (type == IF_TYPE_BRIDGE)
        return "Bridge";
    if (type == IF_TYPE_WWANPP)
        return "Cellular - GSM";
    if (type == IF_TYPE_WWANPP2)
        return "Cellular - CDMA";
    else
        return "Unknown";
}

VOID go (IN PCHAR Buffer, IN ULONG Length) {
    DWORD pdwSize = 0;
    DWORD rc = IPHLPAPI$GetIfTable(NULL, &pdwSize, FALSE);
    if (rc != ERROR_INSUFFICIENT_BUFFER) {
        BeaconPrintf(CALLBACK_ERROR, "[X] GetIfTable probe failed: %lu", rc);
        return;
    }

    PMIB_IFTABLE tbl = (PMIB_IFTABLE)KERNEL32$LocalAlloc(LPTR, pdwSize);
    if (!tbl) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Error allocating mem needed to get table\n");
        return;
    }

    rc = IPHLPAPI$GetIfTable(tbl, &pdwSize, FALSE);
    if (rc != NO_ERROR) {
        BeaconPrintf(CALLBACK_ERROR, "[X] GetIfTable failed: %lu", rc);
        KERNEL32$LocalFree(tbl);
        return;
    }

    BeaconPrintf(CALLBACK_OUTPUT, "%-7s %-25s %-25s %-19s %-12s %s", "ifIndex", "Status", "Type", "MacAddress", "Speed", "Description\n");
    BeaconPrintf(CALLBACK_OUTPUT, "------- ------------------------- ------------------------- ------------------- ------------ ------------------------------\n");

    for (DWORD i = 0; i < tbl->dwNumEntries; i++) {
        PMIB_IFROW row = &tbl->table[i];

        char mac[64] = {0};
        char spd[32] = {0};
        MAC2String(row->bPhysAddr, row->dwPhysAddrLen, mac, sizeof(mac));
        Speed2String(row->dwSpeed, spd, sizeof(spd));

        char name[MAX_INTERFACE_NAME_LEN * 2] = {0};
        KERNEL32$WideCharToMultiByte(CP_UTF8, 0, row->wszName, -1, name, sizeof(name), NULL, NULL);

        char desc[MAXLEN_IFDESCR + 1] = {0};
        MSVCRT$memcpy(desc, row->bDescr, min(row->dwDescrLen, MAXLEN_IFDESCR));
        desc[MAXLEN_IFDESCR] = '\0';

        BeaconPrintf(CALLBACK_OUTPUT, "%-7u %-25s %-25s %-19s %-12s %s\n",
                    row->dwIndex,
                    MapStatusIFTable(row->dwAdminStatus, row->dwOperStatus),
                    MapIfType(row->dwType),
                    mac[0] ? mac: "",
                    spd,
                    desc);
    }

    KERNEL32$LocalFree(tbl);
}
