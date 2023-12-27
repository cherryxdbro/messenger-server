#pragma once

struct WinsockInitializer final
{
    WinsockInitializer();
    ~WinsockInitializer();

    static int WSAResult;
};
