#pragma once

#ifdef _DEBUG
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#endif

#include <WS2tcpip.h>

#include <bcrypt.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#pragma execution_character_set("utf-8")

#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include <sqlite3.h>
