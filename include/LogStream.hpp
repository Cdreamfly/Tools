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

			void clear() const { memset(data_, 0, SIZE); }

			void add(const std::size_t len) { cur_ += len; }

			[[nodiscard]] std::size_t avail() const { return end() - cur_; }

			void append(const char *str, const std::size_t len) {
				if (avail() > len) {
					memcpy(cur_, str, len);
					cur_ += len;
				}
			}

			[[nodiscard]] std::string toString() const { return std::string(data_, length()); }

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

		void append(const char*data,const std::size_t len) {buffer_.append(data,len);}
		[[nodiscard]] const Buffer& buffer()const{return buffer_;}

	private:
		template<typename T>
		void formatInteger(T);

	private:
		Buffer buffer_;
		static constexpr int kMaxNumericSize = 48;
	};
}
