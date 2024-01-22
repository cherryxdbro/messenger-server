#include "pch.h"

#include "Cryptor.h"

Message Cryptor::ToMessage(const std::wstring& message) noexcept
{
    size_t messageSize = message.size() * sizeof(wchar_t);
    std::vector<uint8_t> messageData(messageSize);
    memcpy_s(messageData.data(), messageSize, message.c_str(), messageSize);
    return { std::move(messageData) };
}

std::wstring Cryptor::ToWString(const Message& message) noexcept
{
    return { reinterpret_cast<const wchar_t*>(message.Data.data()), message.Data.size() / sizeof(wchar_t) };
}

Message Cryptor::Encrypt(const Message& message, const std::vector<uint8_t>& sharedKey, const std::vector<uint8_t>& initializationVector) noexcept
{
    BCRYPT_ALG_HANDLE algorithmHandle;
    BCRYPT_KEY_HANDLE keyHandle;
    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(&algorithmHandle, BCRYPT_AES_ALGORITHM, nullptr, 0)))
    {
        return {  };
    }
    if (!NT_SUCCESS(BCryptGenerateSymmetricKey(algorithmHandle, &keyHandle, nullptr, 0, const_cast<PUCHAR>(sharedKey.data()), CRYPTO_BYTES, 0)))
    {
        return {  };
    }
    ULONG encryptedMessageSize = 0;
    if (!NT_SUCCESS(BCryptEncrypt(keyHandle, const_cast<PUCHAR>(message.Data.data()), static_cast<ULONG>(message.Data.size()), nullptr, const_cast<PUCHAR>(initializationVector.data()), CRYPTO_BYTES, nullptr, 0, &encryptedMessageSize, BCRYPT_BLOCK_PADDING)))
    {
        return {  };
    };
    std::vector<uint8_t> encryptedMessage(encryptedMessageSize);
    if (!NT_SUCCESS(BCryptEncrypt(keyHandle, const_cast<PUCHAR>(message.Data.data()), static_cast<ULONG>(message.Data.size()), nullptr, const_cast<PUCHAR>(initializationVector.data()), CRYPTO_BYTES, encryptedMessage.data(), encryptedMessageSize, &encryptedMessageSize, BCRYPT_BLOCK_PADDING)))
    {
        return {  };
    };
    return { std::move(encryptedMessage) };
}

Message Cryptor::Decrypt(const Message& encryptedMessage, const std::vector<uint8_t>& sharedKey, const std::vector<uint8_t>& initializationVector) noexcept
{
    BCRYPT_ALG_HANDLE algorithmHandle;
    BCRYPT_KEY_HANDLE keyHandle;
    if (!NT_SUCCESS(BCryptOpenAlgorithmProvider(&algorithmHandle, BCRYPT_AES_ALGORITHM, nullptr, 0)))
    {
        return {  };
    }
    if (!NT_SUCCESS(BCryptGenerateSymmetricKey(algorithmHandle, &keyHandle, nullptr, 0, const_cast<PUCHAR>(sharedKey.data()), CRYPTO_BYTES, 0)))
    {
        return {  };
    }
    ULONG messageSize = 0;
    if (!NT_SUCCESS(BCryptDecrypt(keyHandle, const_cast<PUCHAR>(encryptedMessage.Data.data()), static_cast<ULONG>(encryptedMessage.Data.size()), nullptr, const_cast<PUCHAR>(initializationVector.data()), CRYPTO_BYTES, nullptr, 0, &messageSize, BCRYPT_BLOCK_PADDING)))
    {
        return {  };
    };
    std::vector<uint8_t> message(messageSize);
    if (!NT_SUCCESS(BCryptDecrypt(keyHandle, const_cast<PUCHAR>(encryptedMessage.Data.data()), static_cast<ULONG>(encryptedMessage.Data.size()), nullptr, const_cast<PUCHAR>(initializationVector.data()), CRYPTO_BYTES, message.data(), messageSize, &messageSize, BCRYPT_BLOCK_PADDING)))
    {
        return {  };
    };
    return { std::move(message) };
}
