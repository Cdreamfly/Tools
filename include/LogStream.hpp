#pragma once
#include <NonCopyable.hpp>
#include <cstring>
#include <string>

namespace cm {
	namespace detail {
		constexpr int kSmallBuffer = 4000;

		template<int SIZE>
		class LogBuffer : NonCopyable {
		public:
			LogBuffer(): cur_(data_) {
			}

			~LogBuffer() = default;

			[[nodiscard]] const char *data() const { return data_; }

			[[nodiscard]] char *current() const { return cur_; }

			[[nodiscard]] int length() const { return cur_ - data_; }

			void reset() const { cur_ = data_; }

			void bzero() const { memset(data_, 0, SIZE); }

			void add(const std::size_t len) { cur_ += len; }

			[[nodiscard]] std::size_t avail() const { return end() - cur_; }

			void append(const char *str, const std::size_t len) {
				if (avail() > len) {
					memcpy(cur_, str, len);
					cur_ += len;
				}
			}

			[[nodiscard]] std::string toString() const { return std::string(data_, length()); }

			[[nodiscard]] std::string_view toStringView() const { return std::string_view(data_, length());}

		private:
			[[nodiscard]] const char *end() const { return data_ + sizeof(data_); }

			char data_[SIZE]{};
			char *cur_{};
		};
	}

	class LogStream : NonCopyable {
	public:
		using self = LogStream;
		using Buffer = detail::LogBuffer<detail::kSmallBuffer>;

		self &operator<<(bool);

		self &operator<<(short);

		self &operator<<(unsigned short);

		self &operator<<(int);

		self &operator<<(unsigned int);

		self &operator<<(long);

		self &operator<<(unsigned long);

		self &operator<<(long long);

		self &operator<<(unsigned long long);

		self &operator<<(const void *);

		self &operator<<(float v);

		self &operator<<(double);

		self &operator<<(char v);

		self &operator<<(const char *);

		self &operator<<(const std::string& );

		self &operator<<(const std::string_view&);

		self &operator<<(const Buffer&);

		void append(const char*data, const std::size_t len) { buffer_.append(data,len); }

		[[nodiscard]] const Buffer& buffer() const { return buffer_; }

		void resetBuffer() const { buffer_.reset(); }

	private:
		template<typename T>
		void formatInteger(T);

	private:
		Buffer buffer_;
		static constexpr int kMaxNumericSize = 48;
	};
}
