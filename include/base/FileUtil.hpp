#pragma once
#include <cstdio>
#include <string>

#include "NonCopyable.hpp"

namespace cm::FileUtil {
	class AppendFile : NonCopyable {
	public:
		explicit AppendFile(const std::string &);

		~AppendFile();

		void append(const char *, size_t);

		void flush() const;

		[[nodiscard]] off_t writtenBytes() const { return writtenBytes_; }

	private:
		size_t write(const char *, size_t) const;

		FILE *fp_;
		char buffer_[64 * 1024]{};
		off_t writtenBytes_{};
	};
}
