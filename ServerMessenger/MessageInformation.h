#pragma once

class MessageInformation final
{
public:

    enum MessagesTypes : int
    {
        StringMessage
    };

    MessagesTypes GetMessageType();
    void SetMessageType(MessagesTypes messageType);

    int GetMessageSize();
    void SetMessageSize(int messageSize);

private:

    MessagesTypes m_messageType;
    int m_messageSize;

};
