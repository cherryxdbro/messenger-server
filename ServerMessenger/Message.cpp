#include "pch.h"

#include "Message.h"

Message::Message() noexcept :
	Data()
{

}

Message::Message(std::vector<uint8_t>&& data) noexcept :
	Data(std::move(data))
{

}

Message::Message(Message&& other) noexcept :
	Data(std::move(other.Data))
{

}
