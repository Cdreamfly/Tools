#include "Logging.hpp"

#include <cassert>
#include <Timestamp.hpp>

class T {
public:
	T(const char *str, const unsigned len): str_(str), len_(len) {
		assert(strlen(str) == len);
	}

	const char *str_;
	const unsigned len_;
};

inline cm::LogStream::self &operator<<(cm::LogStream &s, const T v) {
	s.append(v.str_, v.len_);
	return s;
}

inline cm::LogStream::self &operator<<(cm::LogStream &s, const cm::Logger::SourceFile &v) {
	s.append(v.data_, v.size_);
	return s;
}

class cm::Logger::Impl {
public:
	Impl(const LogLevel level, const int savedErrno, const SourceFile &file, const int line): time_(Timestamp::now()),
		stream_(), level_(level), line_(line), name_(file) {
	}

	void formatTime() const {
		const int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
		const int microseconds = static_cast<int>(microSecondsSinceEpoch % 1000000);
		const Fmt us(".%06dZ", microseconds);
		assert(us.length() == 9);
	}

	void finish();

	Timestamp time_;
	LogStream stream_;
	LogLevel level_;
	int line_;
	SourceFile name_;
};

cm::Logger::Logger(const SourceFile file, const int line) : impl_(
	std::make_unique<Impl>(LogLevel::INFO, 0, file, line)) {
}

cm::Logger::Logger(const SourceFile file, const int line, const bool toAbort) : impl_(
	std::make_unique<Impl>(toAbort ? LogLevel::FATAL : LogLevel::ERROR,errno, file, line)) {
}

cm::Logger::Logger(const SourceFile file, const int line, const LogLevel level) : impl_(
	std::make_unique<Impl>(level, 0, file, line)) {
}

cm::Logger::Logger(const SourceFile file, const int line, const LogLevel level, const char *func) : impl_(
	std::make_unique<Impl>(level, 0, file, line)) {
	impl_->stream_ << func << ' ';
}

cm::Logger::~Logger() {
	impl_->finish();
	if (impl_->level_ == LogLevel::FATAL) {
		abort();
	}
}


cm::Logger::LogLevel cm::Logger::logLevel() const {
	return impl_->level_;
}

cm::LogStream &cm::Logger::stream() const {
	return impl_->stream_;
}

void cm::Logger::setLogLevel(const LogLevel level) const {
	impl_->level_ = level;
}

void cm::Logger::Impl::finish() {
	stream_ << " - " << name_ << ':' << line_ << '\n';
}
