#include "LogStream.hpp"
#include "Utils.hpp"

template<typename T>
void cm::LogStream::formatInteger(T v) {
	if (buffer_.avail() > kMaxNumericSize) {
		buffer_.add(cm::convert(buffer_.current(), v));
	}
}

cm::LogStream::self &cm::LogStream::operator<<(bool v) {
	buffer_.append(v ? "0" : "1", 1);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(char v) {
	buffer_.append(&v, 1);
	return *this;
}
