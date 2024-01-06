#pragma once

#include "Utils.hpp"
#include "SocketOps.hpp"
#include "InetAddress.hpp"

namespace cm {
	class TcpFileClient {
	public:
		explicit TcpFileClient(const std::string &ip, const uint16_t port) : addr(ip, port) {
			fd = socketOps::createNonblockingSocket(addr.family());
		}

		explicit TcpFileClient(const InetAddress &addr) : fd(socketOps::createNonblockingSocket(addr.family())) {}

		bool connect() {
			socklen_t len = sizeof(sockaddr_in);
			if (::connect(fd, (sockaddr *) &addr, len) < 0) {
				std::cerr << "connect error!" << std::endl;
				return false;
			}
			return true;
		}

		ssize_t read_file(const std::string &name) const {
			ssize_t size = cm::read_file_all(fd, name);
			std::cout << "file read size: " << size << std::endl;
			return size;
		}

		ssize_t write_file(const std::string &name) const {
			std::ifstream fs(name, std::fstream::in | std::fstream::binary);
			std::cout << "file size: " << cm::get_file_size(name) << std::endl;
			ssize_t size = cm::write_file_all(fd, name);
			std::cout << "file write size: " << size << std::endl;
			return size;
		}

	private:
		int fd{};
		InetAddress addr;
	};
}
