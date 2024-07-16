#pragma once
#include <cstring>

namespace cm
{
    class Logger
    {
    public:
        enum class LogLevel { RACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };

        class SourceFile
        {
        public:
            template <int N>
            SourceFile(const char (&arr)[N]): data_(arr), size_(N - 1)
            {
                const char* slash = strrchr(data_, '/');
                if (slash)
                {
                    data_ = slash + 1;
                    size_ -= data_ - arr;
                }
            }

            explicit SourceFile(const char* name): data_(name)
            {
                const char* slash = strrchr(data_, '/');
                if (slash)
                {
                    data_ = slash + 1;
                }
                size_ = static_cast<int>(strlen(data_));
            }

            const char* data_;
            int size_{};
        };

    private:
    };
}
