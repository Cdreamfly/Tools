#pragma once

#include "SocketOps.hpp"
#include "InetAddress.hpp"
#include <netinet/tcp.h>

namespace cm {
	class Socket : private cm::noncopyable {
	public:
		explicit Socket(int fd) : _fd(fd) {}

		Socket() : _fd(0) {}

		~Socket() noexcept {
			socketOps::close(_fd);
		}

		[[nodiscard]] int getFd() const {
			return _fd;
		}

		void bind(const InetAddress &addr) const {
			socketOps::bind(_fd, addr.getSockAddr());
		}

		void listen() const {
			socketOps::listen(_fd);
		}

		void SetSocketOps() const {
			socketOps::setSockOps(_fd);
		}

		void shutdownWrite() const {
			socketOps::shutdownWrite(_fd);
		}

		int accept(InetAddress &peerAddr) const {
			sockaddr_in addr{};
			memset(&addr, 0, sizeof(addr));
			int fd = socketOps::accept(_fd, &addr);
			if (fd >= 0) {
				peerAddr.setSockAddr(addr);
			}
			return fd;
		}

		void setTcpNoDelay(bool on) const {
			int opt_val = on ? 1 : 0;
			setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
		}

		void setReuseAddr(bool on) const {
			int opt_val = on ? 1 : 0;
			setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
		}

		void setReusePort(bool on) const {
			int opt_val = on ? 1 : 0;
			int ret = setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
			if (ret < 0 && on) {
				fprintf(stderr, "setReusePort error!\n");
			}
		}

		void setKeepAlive(bool on) const {
			int opt_val = on ? 1 : 0;
			setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
		}

		void setNonblocking() const {
			fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
		}

	private:
		int _fd;
	};
}