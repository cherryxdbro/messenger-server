#include "pch.h"

#include "MessageInformation.h"

MessageInformation::MessagesTypes MessageInformation::GetMessageType()
{
	return m_messageType;
}

int MessageInformation::GetMessageSize()
{
	return m_messageSize;
}

void MessageInformation::SetMessageSize(int messageSize)
{
	m_messageSize = messageSize;
}

void MessageInformation::SetMessageType(MessagesTypes messageType)
{
	m_messageType = messageType;
}
