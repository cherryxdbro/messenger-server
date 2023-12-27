#include "pch.h"

#include "WinsockInitializer.h"

int WinsockInitializer::WSAResult;

WinsockInitializer::WinsockInitializer()
{
    WSADATA wsaData;
    WSAResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
}

WinsockInitializer::~WinsockInitializer()
{
    if (WSAResult != 0)
    {
        WSACleanup();
    }
}
