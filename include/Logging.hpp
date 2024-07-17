#pragma once
#include <LogStream.hpp>
#include <memory>

namespace cm {
	class Logger {
	public:
		enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };

		Logger(const std::string &, int);

		Logger(const std::string &, int, LogLevel);

		Logger(const std::string &, int, LogLevel, const char *);

		Logger(const std::string &, int, bool);

		~Logger();

		[[nodiscard]] LogStream &stream() const;

		[[nodiscard]] LogLevel logLevel() const;

		void setLogLevel(LogLevel) const;

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	};

#define LOG_WARN cm::Logger(__FILE__, __LINE__, cm::Logger::LogLevel::WARN).stream()
#define LOG_ERROR cm::Logger(__FILE__, __LINE__, cm::Logger::LogLevel::ERROR).stream()
#define LOG_FATAL cm::Logger(__FILE__, __LINE__, cm::Logger::LogLevel::FATAL).stream()
#define LOG_SYSERR cm::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL cm::Logger(__FILE__, __LINE__, true).stream()
}
