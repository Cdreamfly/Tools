#pragma once

#include <iostream>

#include "base/Utils.hpp"
#include "net/InetAddress.hpp"
#include "net/SocketOps.hpp"

namespace cm {
	class TcpFileClient {
	public:
		explicit TcpFileClient(const std::string &ip, const uint16_t port) : addr(port, ip) {
			fd = cm::net::sockets::createNonblockingOrDie(addr.family());
		}

		explicit TcpFileClient(const net::InetAddress &addr) : fd(net::sockets::createNonblockingOrDie(addr.family())) {}

		bool connect() {
			if (::connect(fd, (sockaddr *) &addr, sizeof(sockaddr_in)) < 0) {
				std::cerr << "connect error!" << std::endl;
				return false;
			}
			return true;
		}

		[[nodiscard]] ssize_t read_file(const std::string &name) const {
			return cm::read_file_all(fd, name);
		}

		[[nodiscard]] ssize_t write_file(const std::string &name) const {
			return cm::write_file_all(fd, name);
		}

	private:
		int fd{};
		net::InetAddress addr;
	};
}
