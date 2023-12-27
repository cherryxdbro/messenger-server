#pragma once

template <class T>
class Message
{
public:

    Message(T&& data) :
        m_data(data)
    {

    }

    T& GetData()
    {
        return m_data;
    }
    void SetData(const T& data)
    {
        m_data = data;
    }

    virtual std::unique_ptr<std::vector<char>> Serialize() const
    {
        std::unique_ptr<std::vector<char>> buffer = std::make_unique<std::vector<char>>(sizeof(T));
        memcpy_s(buffer->data(), sizeof(T), &m_data, sizeof(T));
        return buffer;
    }
    static Message<T> Deserialize(const std::vector<char>& buffer)
    {
        if (buffer.size() != sizeof(T))
        {
            return Message<T>(T());
        }
        T data{};
        memcpy_s(&data, sizeof(T), buffer.data(), sizeof(T));
        return Message<T>(std::move(data));
    }

protected:

    T m_data;

};
