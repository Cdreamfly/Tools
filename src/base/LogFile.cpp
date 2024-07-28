#include <cassert>
#include <memory>
#include <thread>

#include <base/LogFile.hpp>
#include <base/FileUtil.hpp>
#include <base/Utils.hpp>

cm::LogFile::LogFile(const std::string &basename_, const off_t rollSize, const bool threadSafe, const int flushInterval,
                     const int checkEverN) : basename_(basename_), rollSize_(rollSize), flushInterval_(flushInterval),
                                             checkEverN_(checkEverN), threadSafe_(threadSafe), count_(0),
                                             startOfPeriod_(0), lastRoll_(0), lastFlush_(0) {
	assert(basename_.find('/') == std::string::npos);
	rollFile();
}

cm::LogFile::~LogFile() = default;

void cm::LogFile::rollFile() {
	time_t now = time(nullptr);
	std::string filename = getLogFileName(basename_, &now);
	const time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;
	if (now > lastRoll_) {
		lastRoll_ = now;
		lastFlush_ = now;
		startOfPeriod_ = start;
		file_ = std::make_unique<FileUtil::AppendFile>(filename);
	}
}

std::string cm::LogFile::getLogFileName(const std::string &basename, time_t *const now) {
	std::string filename;
	filename.reserve(basename.size() + 64);
	filename = basename;

	char timebuf[32] = {0};
	*now = time(nullptr);
	const std::tm *tm = localtime(now);
	strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", tm);
	filename += timebuf;

	filename += hostname();

	char pidbuf[32] = {0};
	snprintf(pidbuf, sizeof pidbuf, ".%d", ::getpid());
	filename += pidbuf;

	filename += ".log";

	return filename;
}


void cm::LogFile::append(const char *logLine, const int len) {
	if (threadSafe_) {
		std::lock_guard<std::mutex> lg{mutex_};
		append_unlocked(logLine, len);
	} else {
		append_unlocked(logLine, len);
	}
}

void cm::LogFile::append_unlocked(const char *logLine, const int len) {
	file_->append(logLine, len);
	if (file_->writtenBytes() > rollSize_) {
		rollFile();
	} else {
		if (++count_ > checkEverN_) {
			count_ = 0;
			const time_t now = time(nullptr);
			const time_t thisPeriod = now / kRollPerSeconds_ * kRollPerSeconds_;
			if (thisPeriod != startOfPeriod_) {
				rollFile();
			} else if (now - lastFlush_ > flushInterval_) {
				lastFlush_ = now;
				file_->flush();
			}
		}
	}
}

void cm::LogFile::flush() {
	if (threadSafe_) {
		std::lock_guard<std::mutex> lg{mutex_};
		file_->flush();
	} else {
		file_->flush();
	}
}
