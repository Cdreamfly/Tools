#pragma once
#include <cstring>
#include <LogStream.hpp>
#include <memory>

namespace cm {
	class Logger {
	public:
		enum class LogLevel { RACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };

		class SourceFile {
		public:
			template<int N>
			SourceFile(const char (&arr)[N]): data_(arr), size_(N - 1) {
				const char *slash = strrchr(data_, '/');
				if (slash) {
					data_ = slash + 1;
					size_ -= data_ - arr;
				}
			}

			explicit SourceFile(const char *name): data_(name) {
				const char *slash = strrchr(data_, '/');
				if (slash) {
					data_ = slash + 1;
				}
				size_ = static_cast<int>(strlen(data_));
			}

			const char *data_;
			int size_{};
		};

		Logger(SourceFile, int);

		Logger(SourceFile, int, LogLevel);

		Logger(SourceFile, int, LogLevel, const char *);

		Logger(SourceFile, int, bool);

		~Logger();

		[[nodiscard]] LogStream &stream() const;

		[[nodiscard]] LogLevel logLevel() const;

		void setLogLevel(LogLevel) const;

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	};
}
