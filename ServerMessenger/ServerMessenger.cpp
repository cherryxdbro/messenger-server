#include "pch.h"

#include "Server.h"

int wmain(int, wchar_t* [], wchar_t* [])
{
    SetConsoleOutputCP(CP_UTF8);
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("main", spdlog::sinks_init_list
        {
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(L"logs/log.txt", 1024Ui64 * 1024Ui64 * 16Ui64, 10Ui64)
        }));
    Server server = Server();
    server.Start(5678);
}
