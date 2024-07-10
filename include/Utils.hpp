#pragma once

#include <sys/stat.h>
#include <sys/statfs.h>
#include <fcntl.h>
#include <csignal>
#include <dirent.h>
#include <fstream>
#include <regex>
#include <chrono>
#include <iostream>
#include <thread>

namespace cm {
	constexpr char lut[] = "0123456789ABCDEF";
	constexpr char digits[] = "9876543210123456789";
	inline const char *zero = digits + 9;

	template<typename T>
	size_t convert(char buf[], T value) {
		T i = value;
		char *p = buf;
		do {
			const int lsd = static_cast<int>(i % 10);
			i /= 10;
			*p++ = zero[lsd];
		} while (i != 0);

		if (value < 0) {
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return p - buf;
	}

	/**
	 * 获取比特位是0或1
	 * @tparam T
	 * @param num
	 * @param i
	 * @return
	 */
	template<typename T>
	T get_bit(const T num, const int i) {
		return (1 << (i - 1) & num) >> (i - 1);
	}

	/**
	 * 设置比特位为1
	 * @tparam T
	 * @param num
	 * @param i
	 * @return
	 */
	template<typename T>
	T set_bit(T num, const int i) {
		num |= 1 << (i - 1);
		return num;
	}

	/**
	 * 清除比特位为0
	 * @tparam T
	 * @param num
	 * @param i
	 * @return
	 */
	template<typename T>
	T clr_bit(T num, const int i) {
		num &= ~(1 << (i - 1));
		return num;
	}

	/**
	 * 获取文件大小
	 * @param path
	 * @return
	 */
	inline long int get_file_size(const std::string &path) {
		if (open(path.c_str(), O_RDONLY) < 0) {
			std::cerr << "file open fail!" << std::endl;
			return -1;
		}
		struct stat info{};
		stat(path.c_str(), &info);
		return info.st_size;
	}

	/**
	 * 获取路径可用空间
	 * @param path
	 * @return
	 */
	inline size_t get_disk_available_space(const std::string &path) {
		if (access(path.c_str(), F_OK) != 0) {
			std::cerr << " file path open fail!" << std::endl;
			return -1;
		}
		struct statfs diskInfo{};
		statfs(path.c_str(), &diskInfo);
		return diskInfo.f_bavail * diskInfo.f_bsize;
	}

	/**
	 * 获取文件夹内所有文件
	 * @param path
	 * @param files
	 */
	inline bool get_dir_file_names(const std::string &path, std::vector<std::string> &files) {
		DIR *pDir = opendir(path.c_str());
		if (!pDir) return false;
		dirent *ptr = nullptr;
		while ((ptr = readdir(pDir)) != nullptr) {
			if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
				files.emplace_back(ptr->d_name);
			}
		}
		closedir(pDir);
		return true;
	}

	/**
	 * 查找文件夹内是否有指定文件
	 * @param path
	 * @param name
	 * @return
	 */
	inline bool find_dir_file_exists(const std::string &path, const std::string &name) {
		std::vector<std::string> files_name;
		get_dir_file_names(path, files_name);
		return std::any_of(files_name.begin(), files_name.end(), [&](const std::string &it) { return name == it; });
	}

	/**
	 * 读取文件并发送
	 * @param fd
	 * @param path
	 * @return
	 */
	inline ssize_t write_file_all(const int fd, const std::string &path) {
		std::fstream fs(path.c_str(), std::fstream::in | std::fstream::binary);
		char buf[BUFSIZ] = {0};
		ssize_t size = 0;
		while (!fs.eof()) {
			fs.read(buf, sizeof(buf));
			size += write(fd, buf, fs.gcount());
		}
		fs.close();
		return size;
	}

	/**
	 * 读取并写入文件
	 * @param fd
	 * @param path
	 * @return
	 */
	inline ssize_t read_file_all(const int fd, const std::string &path) {
		std::fstream fs(path.c_str(), std::fstream::out | std::fstream::binary);
		char buf[BUFSIZ] = {0};
		ssize_t size = 0;
		ssize_t len = 0;
		while (true) {
			len = ::read(fd, buf, sizeof(buf));
			if (len <= 0) break;
			fs.write(buf, len);
			size += len;
		}
		fs.close();
		return size;
	}

	/**
	 * 获取剩余空间的大小
	 * @param path
	 * @return
	 */
	inline unsigned long long get_available_Disk(const std::string &path) {
		struct statfs diskInfo{};
		statfs(path.c_str(), &diskInfo);
		return (diskInfo.f_bavail * diskInfo.f_bsize) >> 30;
	}

	/**
	 * 字符串切片
	 * @param str_v
	 * @param del_ims
	 * @return
	 */
	inline std::vector<std::string_view> string_view_split(std::string_view str_v, std::string_view del_ims = " ") {
		std::vector<std::string_view> output;
		size_t first = 0;
		while (first < str_v.size()) {
			const auto second = str_v.find_first_of(del_ims, first);
			if (first != second) {
				output.emplace_back(str_v.substr(first, second - first));
			}
			if (second == std::string_view::npos) {
				break;
			}
			first = second + 1;
		}
		return output;
	}

	/**
		* 字符串切片
		* @param str_v
		* @param del_ims
		* @return
	*/
	inline std::vector<std::string> string_split(const std::string &str_v, const std::string &del_ims = " ") {
		std::vector<std::string> output;
		size_t first = 0;
		while (first < str_v.size()) {
			const auto second = str_v.find_first_of(del_ims, first);
			if (first != second) {
				output.emplace_back(str_v.substr(first, second - first));
			}
			if (second == std::string::npos) {
				break;
			}
			first = second + 1;
		}
		return output;
	}

	/**
	 * 获取执行cmd命令返回内容
	 * @param cmd
	 * @return
	 */
	inline std::string get_addr_by_cmd(const std::string &cmd) {
		char buf[BUFSIZ] = {0};
		std::string res;
		if (FILE *fp = nullptr; (fp = popen(cmd.c_str(), "r")) != nullptr) {
			while (fgets(buf, sizeof(buf), fp) != nullptr) {
				res += buf;
			}
			pclose(fp);
			fp = nullptr;
		}
		return res;
	}

	/**
	 * 判断字符串是不是全是数字
	 * @param str
	 * @return
	 */
	inline bool is_all_number_string(const std::string &str) {
		return std::all_of(str.begin(), str.end(), [&](const char c) {
			if (std::isdigit(c) == 0) {
				return false;
			}
			return true;
		});
	}

	/**
	 * 10进制转任意进制
	 * @param n
	 * @param radix
	 * @return
	 */
	inline std::string ito_str(int n, const int radix) {
		std::string ans;
		do {
			int t = n % radix;
			if (t >= 0 && t <= 9)ans += (t + '0');
			else ans += (t - 10 + 'a');
			n /= radix;
		} while (n != 0);
		reverse(ans.begin(), ans.end());
		return ans;
	}

	/**
	 * 任意进制转10进制
	 * @param s
	 * @param radix
	 * @return
	 */
	inline int str_toi(const std::string &s, const int radix) {
		int ans = 0;
		for (const char t: s) {
			if (t >= '0' && t <= '9') ans = ans * radix + t - '0';
			else ans = ans * radix + t - 'a' + 10;
		}
		return ans;
	}

	/**
	 * 16进制转文字
	 * @param str
	 * @return
	 */
	inline std::string hex_to_string(const std::string &str) {
		std::string result;
		//十六进制两个字符为原始字符一个字符
		for (size_t i = 0; i < str.length(); i += 2) {
			//每次切两个字符
			std::string byte = str.substr(i, 2);
			//将十六进制的string转成long再强转成int再转成char
			result.push_back(
				static_cast<char>(static_cast<int>(std::strtol(byte.c_str(), nullptr, 16)))); //将处理完的字符压入result中
		}
		return result;
	}

	inline std::string string_to_hex(const std::string &str) {
		const size_t len = str.length();
		std::string hex;
		hex.reserve(len * 2);
		for (size_t i = 0; i < len; ++i) {
			const unsigned char c = str[i];
			hex.push_back(lut[c >> 4]);
			hex.push_back(lut[c & 15]);
		}
		return hex;
	}


	/**
	 * 异步定时任务
	 * @tparam callable
	 * @tparam arguments
	 * @param count 执行次数, 0 循环执行
	 * @param after 定时执行, 单位秒，过after秒后执行 0 立即执行
	 * @param interval 间隔时间, 单位秒, 每次执行间隔多长  0 不间隔
	 * @param f
	 * @param args
	 */
	template<typename callable, class... arguments>
	void async_wait(const size_t count, const size_t after, const size_t interval, callable &&f, arguments &&... args) {
		std::function<typename std::result_of<callable(arguments...)>::type()> task
				(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
		std::thread([after, task, count, interval]() {
			if (count == 0) {
				std::this_thread::sleep_for(std::chrono::seconds(after));
				while (true) {
					task();
					std::this_thread::sleep_for(std::chrono::seconds(interval));
				}
			} else {
				std::this_thread::sleep_for(std::chrono::seconds(after));
				for (size_t i = 0; i < count; ++i) {
					task();
					std::this_thread::sleep_for(std::chrono::seconds(interval));
				}
			}
		}).detach();
	}

	/**
	* Name:    CRC-16/MODBUS       x16+x15+x2+1
	* Poly:    0x8005
	* Init:    0xFFFF
	* Refine:   TrueA
	* Ref out:  True
	* Xor out:  0x0000
	* Note:
	*/
	inline unsigned short int crc16_modbus(const unsigned char *data, unsigned short int length) {
		unsigned short int crc = 0xffff;
		while (length--) {
			crc ^= *data++;
			for (unsigned char i = 0; i < 8; ++i) {
				if (crc & 1) {
					crc = (crc >> 1) ^ 0xA001;
				} else {
					crc = (crc >> 1);
				}
			}
		}
		return crc;
	}

	inline std::string ustr_to_hex(const unsigned char ch[], const int len) {
		std::stringstream ss;
		for (int i = 0; i < len; ++i) {
			ss << lut[ch[i] >> 4] << lut[ch[i] & 0xf];
		}
		return ss.str();
	}
}
