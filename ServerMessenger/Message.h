#pragma once

struct Message final
{
	Message() noexcept;
	Message(std::vector<uint8_t>&& data) noexcept;
	Message(Message&& other) noexcept;

	std::vector<uint8_t> Data;
};
