#include "base/Logging.hpp"
#include "base/Timestamp.hpp"

#include <cassert>
#include <ctime>
#include <iomanip>

const char *LogLevelName[] = {
	"TRACE ",
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL ",
};

cm::Logger::LogLevel initLogLevel() {
	if (::getenv("CM_LOG_TRACE")) return cm::Logger::LogLevel::TRACE;
	if (::getenv("CM_LOG_DEBUG")) return cm::Logger::LogLevel::DEBUG;
	return cm::Logger::LogLevel::INFO;
}

class cm::Logger::Impl {
public:
	Impl(const LogLevel level, const int savedErrno, const std::string &file, const int line): time_(Timestamp::now()),
		stream_(), line_(line) {
		const size_t end = file.find_last_of('/');
		if (end != std::string::npos) {
			name_ = file.substr(end + 1);
		}
		formatTime();
		stream_ << LogLevelName[static_cast<int>(level)];
		level_ = level;
	}

	void formatTime() {
		const int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
		const int microseconds = static_cast<int>(microSecondsSinceEpoch % 1000000);
		const std::time_t t = std::time(nullptr);
		const std::tm *lt = localtime(&t);
		const Fmt us(".%06d ", microseconds);
		assert(us.length() == 8);
		std::stringstream ss;
		ss << std::put_time(lt, "%Y-%m-%d %H:%M:%S");
		stream_ << ss.str() << us;
	}

	void finish();

	Timestamp time_;
	LogStream stream_;
	static LogLevel level_;
	int line_;
	std::string name_;
};

cm::Logger::LogLevel cm::Logger::Impl::level_ = initLogLevel();

cm::Logger::Logger(const std::string &file, const int line) : impl_(
	std::make_unique<Impl>(LogLevel::INFO, 0, file, line)) {
}

cm::Logger::Logger(const std::string &file, const int line, const bool toAbort) : impl_(
	std::make_unique<Impl>(toAbort ? LogLevel::FATAL : LogLevel::ERROR,errno, file, line)) {
}

cm::Logger::Logger(const std::string &file, const int line, const LogLevel level) : impl_(
	std::make_unique<Impl>(level, 0, file, line)) {
}

cm::Logger::Logger(const std::string &file, const int line, const LogLevel level, const char *func) : impl_(
	std::make_unique<Impl>(level, 0, file, line)) {
	impl_->stream_ << func << ' ';
}

cm::Logger::~Logger() {
	impl_->finish();
	const LogStream::Buffer &buf(stream().buffer());
	fwrite(buf.data(), 1, buf.length(), stdout);
	if (impl_->level_ == LogLevel::FATAL) {
		fflush(stdout);
		abort();
	}
}

cm::Logger::LogLevel cm::Logger::logLevel() {
	return Impl::level_;
}

cm::LogStream &cm::Logger::stream() const {
	return impl_->stream_;
}

void cm::Logger::setLogLevel(const LogLevel level) {
	Impl::level_ = level;
}

void cm::Logger::Impl::finish() {
	stream_ << " - " << name_ << ':' << line_ << '\n';
}
