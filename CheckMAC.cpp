﻿#include "pch.h"

struct MacVirtualMachinePair {
    const char* mac;
    const char* vm;
};

//Структура с mac-адресами и вир. машинами
MacVirtualMachinePair vmPairs[] = {
        { "00:05:69", "VMware" },
        { "00:0C:29", "VMware" },
        { "00:1C:14", "VMware" },

        { "08:00:27", "VirtualBox" },

        { "00:15:5D", "HyperV" },
        { "00:03:FF", "HyperV" },

        { "00:16:3E", "XenSource" },

        { "00:ca:fe", "Xen" },

        { "52:54:00", "KVM" }
};

// Функция, которая сравнивает MAC-адрес с массивом MacVirtualMachinePair и возвращает соответствующее название виртуальной машины
std::string check_mac(std::string macAd) {

    const char* mac = macAd.c_str();

    int numPairs = sizeof(vmPairs) / sizeof(vmPairs[0]);

    for (int i = 0; i < numPairs; i++) {
        if (strncmp(mac, vmPairs[i].mac, 8) == 0) {
            return vmPairs[i].vm;
        }
    }

    return "host";
}

#ifdef _WIN32

std::string getVMonMAC() {
    IP_ADAPTER_INFO* AdapterInfo = nullptr;
    DWORD dwBufLen = 0;

    if (GetAdaptersInfo(nullptr, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
        AdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new char[dwBufLen]);
        if (AdapterInfo == nullptr) {
            return "ERR";
        }

        if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
            IP_ADAPTER_INFO* pAdapterInfo = AdapterInfo;

            while (pAdapterInfo != nullptr) {
                if (pAdapterInfo->AddressLength > 0 && pAdapterInfo->Address != nullptr) {
                    std::stringstream ss;
                    for (unsigned int i = 0; i < pAdapterInfo->AddressLength; ++i) {
                        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(pAdapterInfo->Address[i]);
                        if (i < pAdapterInfo->AddressLength - 1) {
                            ss << ":";
                        }
                    }
                    std::string mac_addr = ss.str();

                    std::string check_result = check_mac(mac_addr);
                    if (check_result != "host") {
                        delete[] reinterpret_cast<char*>(AdapterInfo);
                        return check_result;
                    }
                }

                pAdapterInfo = pAdapterInfo->Next;
            }
        }

        delete[] reinterpret_cast<char*>(AdapterInfo);
    }

    return "host";
}


#else


std::string getVMonMAC() {
    struct ifaddrs* ifaddr, * ifa;
    char mac_addr[18];

    if (getifaddrs(&ifaddr) != 0) {
        return "ERR";
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != nullptr && ifa->ifa_addr->sa_family == AF_LINK) {
            struct sockaddr_dl* sdl = (struct sockaddr_dl*)ifa->ifa_addr;
            const unsigned char* macBytes = (unsigned char*)LLADDR(sdl);
            std::stringstream ss;
            for (int i = 0; i < sdl->sdl_alen; ++i) {
                ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(macBytes[i]);
                if (i < sdl->sdl_alen - 1) {
                    ss << ":";
                }
            }
            std::strcpy(mac_addr, ss.str().c_str());

            std::string check_result = check_mac(mac_addr);
            if (check_result != "host") {
                freeifaddrs(ifaddr);
                return check_result;
            }
        }
    }

    freeifaddrs(ifaddr);
    return "host";
}

#endif