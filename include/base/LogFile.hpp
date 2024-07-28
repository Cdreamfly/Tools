#pragma once
#include <memory>
#include <mutex>
#include <string>

#include "NonCopyable.hpp"

namespace cm {
	namespace FileUtil {
		class AppendFile;
	}

	class LogFile : NonCopyable {
	public:
		LogFile(const std::string &, off_t rollSize,
		        bool threadSafe = true, int flushInterval = 3, int checkEverN = 1024);

		~LogFile();

		void append(const char *, int);

		void flush();

		void rollFile();

	private:
		void append_unlocked(const char *, int);

		static std::string getLogFileName(const std::string &, time_t *);

		const std::string basename_;
		const off_t rollSize_;
		const int flushInterval_;
		const int checkEverN_;
		const int threadSafe_;

		int count_;
		std::mutex mutex_;
		time_t startOfPeriod_;
		time_t lastRoll_;
		time_t lastFlush_;

		std::unique_ptr<FileUtil::AppendFile> file_;
		constexpr static int kRollPerSeconds_ = 60 * 60 * 24;
	};
}
