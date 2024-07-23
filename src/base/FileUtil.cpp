#include "base/FileUtil.hpp"

#include <cassert>
#include <cstring>

cm::FileUtil::AppendFile::AppendFile(const std::string &file) : fp_(fopen(file.c_str(), "ae")), writtenBytes_(0) {
	assert(fp_);
	::setbuffer(fp_, buffer_, sizeof(buffer_));
}

cm::FileUtil::AppendFile::~AppendFile() {
	::fclose(fp_);
}

void cm::FileUtil::AppendFile::flush() const {
	::fflush(fp_);
}

void cm::FileUtil::AppendFile::append(const char *logLine, const size_t len) {
	size_t n = write(logLine, len);
	size_t remain = len - n;
	while (remain > 0) {
		const size_t x = write(logLine + n, remain);
		if (x == 0) {
			const int err = ferror(fp_);
			if (err) {
				fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
			}
			break;
		}
		n += x;
		remain = len - n;
	}
	writtenBytes_ += len;
}

size_t cm::FileUtil::AppendFile::write(const char *logLine, const size_t len) const {
	return ::fwrite_unlocked(logLine, 1, len, fp_);
}
