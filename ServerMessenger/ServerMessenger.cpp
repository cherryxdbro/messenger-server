#include "pch.h"

#include "Server.h"
#include <conio.h>
static std::wstring incomingMessage;
int wmain(int, wchar_t* [], wchar_t* [])
{
    SetConsoleOutputCP(CP_UTF8);
    static CONSOLE_SCREEN_BUFFER_INFO csbi;
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("main", spdlog::sinks_init_list
        {
            std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg&)
                {
                    printf_s("%s", "\x1b[2K\r");
                    static CONSOLE_SCREEN_BUFFER_INFO csbi2;
                    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi2);
                    for (short i = 0; i < csbi2.dwCursorPosition.Y - csbi.dwCursorPosition.Y; i++)
                    {
                        printf_s("%s", "\x1b[A\r");
                        printf_s("%s", "\x1b[2K\r");
                    }
                }),
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(L"logs/log.txt", 1024Ui64 * 1024Ui64 * 16Ui64, 10Ui64),
            std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg&)
                {
                    printf_s("%s", "> ");
                    std::string buf;
                    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
                    spdlog::details::os::wstr_to_utf8buf(incomingMessage, buf);
                    printf_s("%s", buf.c_str());
                })
        }));
    sqlite3* database;
    int nRes = sqlite3_open_v2("server.sqlite3", &database, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
    if (nRes != SQLITE_OK)
    {
        return SQLITE_ERROR;
    }
    char* err;
    nRes = sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS USERS (ID TEXT NOT NULL PRIMARY KEY, KYBER_PK BLOB NOT NULL, DILITHIUM_PK BLOB NOT NULL);", nullptr, nullptr, &err);
    if (nRes != SQLITE_OK)
    {
        return nRes;
    }
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value value;
    Capsulator::KyberKeyPair kyberKeyPair = Capsulator::MakeKyberKeyPair();
    value.SetString(reinterpret_cast<const char*>(kyberKeyPair.PublicKey.data()), kyberKeyPair.PublicKey.size());
    document.AddMember("kyber_pk", value, document.GetAllocator());
    Signer::DilithiumKeyPair dilithiumKeyPair = Signer::MakeDilithiumKeyPair();
    value.SetString(reinterpret_cast<const char*>(dilithiumKeyPair.PublicKey.data()), dilithiumKeyPair.PublicKey.size());
    document.AddMember("dilithium_pk", value, document.GetAllocator());
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    const std::string& str = buffer.GetString();
    rapidjson::Document document2;
    document2.Parse(str.c_str());
    sqlite3_stmt* stmW;
    nRes = sqlite3_prepare_v2(database, "INSERT OR IGNORE INTO USERS VALUES(?, ?, ?)", -1, &stmW, 0);
    if (nRes != SQLITE_OK) { return nRes; }
    nRes = sqlite3_bind_text(stmW, 1, "hello", 5, 0);
    nRes = sqlite3_bind_blob(stmW, 2, document2["kyber_pk"].GetString(), Capsulator::PublicBytes, 0);
    nRes = sqlite3_bind_blob(stmW, 3, document2["dilithium_pk"].GetString(), Signer::PublicBytes, 0);
    if (nRes != SQLITE_OK) { return nRes; }
    nRes = sqlite3_step(stmW);
    if (nRes != SQLITE_DONE) { return nRes; }
    nRes = sqlite3_reset(stmW);
    if (nRes != SQLITE_OK) { return nRes; }
    nRes = sqlite3_close_v2(database);
    std::vector<uint8_t> kyber_pk(Capsulator::PublicBytes);
    memcpy_s(kyber_pk.data(), Capsulator::PublicBytes, document2["kyber_pk"].GetString(), Capsulator::PublicBytes);
    std::vector<uint8_t> dilithium_pk(Signer::PublicBytes);
    memcpy_s(dilithium_pk.data(), Signer::PublicBytes, document2["dilithium_pk"].GetString(), Signer::PublicBytes);
    std::thread([]() {
        while (true)
        {
            spdlog::info(L"хай");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }}).detach();
    while (true)
    {
        std::wstring wbuf;
        std::string buf;
        wbuf.push_back(_getwch());
        spdlog::details::os::wstr_to_utf8buf(wbuf, buf);
        switch (wbuf[0])
        {
        case '\0':
        case '\n':
            break;
        case '\b':
            if (!incomingMessage.empty())
            {
                printf_s("%s", "\b \b");
                incomingMessage.pop_back();
            }
            break;
        case '\r':
            if (!incomingMessage.empty())
            {
                printf_s("%s", "\n> ");
                incomingMessage.clear();
            }
            break;
        default:
            printf_s("%s", buf.c_str());
            incomingMessage.append(wbuf);
            break;
        }
    }
    std::shared_ptr<Server> server = std::make_shared<Server>();
    server->Start();
}
