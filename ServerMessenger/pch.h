#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <iphlpapi.h>

#include <fcntl.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <io.h>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <sstream>
