#include "pch.h"

#include "StringMessage.h"

StringMessage::StringMessage(std::wstring&& data) : Message<std::wstring>(std::move(data))
{

}

std::unique_ptr<std::vector<char>> StringMessage::Serialize() const
{
    size_t size = m_data.size() * sizeof(wchar_t);
    std::unique_ptr<std::vector<char>> buffer = std::make_unique<std::vector<char>>(size);
    memcpy_s(buffer->data(), size, m_data.data(), size);
    return buffer;
}

StringMessage StringMessage::Deserialize(const std::vector<char>& buffer)
{
    size_t size = buffer.size() / sizeof(wchar_t);
    std::wstring data(reinterpret_cast<const wchar_t*>(buffer.data()), size);
    return StringMessage(std::move(data));
}
