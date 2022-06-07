#pragma once

namespace Engine {

    using EException = std::exception;

    class ESocketException : public EException {
    public:
        enum class EType {
            Unknown,
            Socket,
            Bind,
            Listen,
            Accept,
            Connect,
            Send,
            Receive,
            Close,
            Shutdown,
            GetHostByName,
            GetHostByAddr
        };
    private:
        EString fMessage;
    public:
        ESocketException(EType type, const EString& message = EString());

        const char* what() const noexcept override;
    private:
        static const char* GetTypeString(EType type);
    };
    
}