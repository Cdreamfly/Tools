#include "LogStream.hpp"
#include "Utils.hpp"

template<typename T>
void cm::LogStream::formatInteger(T v) {
	if (buffer_.avail() > kMaxNumericSize) {
		buffer_.add(cm::convert(buffer_.current(), v));
	}
}

cm::LogStream::self &cm::LogStream::operator<<(const bool v) {
	buffer_.append(v ? "0" : "1", 1);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const char v) {
	buffer_.append(&v, 1);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const float v) {
	*this <<static_cast<double>(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const unsigned short v) {
	*this << static_cast<unsigned int>(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const short v) {
	*this << static_cast<int>(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const int v) {
	formatInteger(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const unsigned int v) {
	formatInteger(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const unsigned long v)
{
	formatInteger(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const long v) {
	formatInteger(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const long long v)
{
	formatInteger(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const unsigned long long v)
{
	formatInteger(v);
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const double v) {
	if (buffer_.avail() >=kMaxNumericSize) {
		buffer_.add(snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v));
	}
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const char *str) {
	if(str) {
		buffer_.append(str,strlen(str));
	} else {
		buffer_.append("(NULL)", 6);
	}
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const void *p) {
	if (buffer_.avail() >= kMaxNumericSize) {
		char* buf = buffer_.current();
		buf[0] = '0';
		buf[1] = 'x';
		buffer_.add(convertHex(buf + 2, reinterpret_cast<uintptr_t>(p)) + 2);
	}
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const std::string &str) {
	buffer_.append(str.c_str(), str.length());
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const std::string_view &str) {
	buffer_.append(str.data(),str.length());
	return *this;
}

cm::LogStream::self &cm::LogStream::operator<<(const Buffer &buf) {
	*this << buf.toStringView();
	return *this;
}


