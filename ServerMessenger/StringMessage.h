#pragma once

#include "Message.h"

class StringMessage final : public Message<std::wstring>
{
public:

    StringMessage(std::wstring&& data);

    virtual std::unique_ptr<std::vector<char>> Serialize() const override;
    static StringMessage Deserialize(const std::vector<char>& buffer);

};
