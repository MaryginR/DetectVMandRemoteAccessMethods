#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#ifdef _WIN32
#include <stdio.h>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>
#include <Psapi.h>
#include <algorithm>

#pragma comment(lib, "iphlpapi.lib")
#else
#include <cstring>
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <sys/socket.h>
#include <iterator>
#endif