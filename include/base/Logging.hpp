#pragma once

#include <functional>

#include "base/LogStream.hpp"

#include <memory>

namespace cm {
	class Logger {
	public:
		enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };

		using OutPutCallback = std::function<void(const char *, int)>;
		using FlushCallback = std::function<void()>;

		Logger(const std::string &, int);

		Logger(const std::string &, int, LogLevel);

		Logger(const std::string &, int, LogLevel, const char *);

		Logger(const std::string &, int, bool);

		~Logger();

		[[nodiscard]] LogStream &stream() const;

		static LogLevel logLevel();

		static void setLogLevel(LogLevel);

		static void setOutPutCallback(const OutPutCallback &cb) {
			outPutCallback_ = cb;
		}

		static void setFlushCallback(const FlushCallback &cb) {
			flushCallback_ = cb;
		}

	private:
		static OutPutCallback outPutCallback_;
		static FlushCallback flushCallback_;
		static LogLevel logLevel_;
		class Impl;
		std::unique_ptr<Impl> impl_;
	};

#define LOG_TRACE if (cm::Logger::logLevel() <= cm::Logger::LogLevel::TRACE) \
						cm::Logger(__FILE__, __LINE__, cm::Logger::LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG if (cm::Logger::logLevel() <= cm::Logger::LogLevel::DEBUG) \
						cm::Logger(__FILE__, __LINE__, cm::Logger::LogLevel::DEBUG, __func__).stream()
#define LOG_INFO if (cm::Logger::logLevel() <=cm::Logger::LogLevel::INFO) \
						cm::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN cm::Logger(__FILE__, __LINE__, cm::Logger::LogLevel::WARN).stream()
#define LOG_ERROR cm::Logger(__FILE__, __LINE__, cm::Logger::LogLevel::ERROR).stream()
#define LOG_FATAL cm::Logger(__FILE__, __LINE__, cm::Logger::LogLevel::FATAL).stream()
#define LOG_SYSERR cm::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL cm::Logger(__FILE__, __LINE__, true).stream()
}
