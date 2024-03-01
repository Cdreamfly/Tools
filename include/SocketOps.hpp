#pragma once

#include <arpa/inet.h>

namespace cm::socketOps {
	inline int createNonblockingSocket(const sa_family_t family) {
		const int fd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
		if (fd < 0) {
			fprintf(stderr, "Sockets::CreateNonblockingSocket\n");
		}
		return fd;
	}

	inline void bind(const int fd, const sockaddr *addr) {
		if (::bind(fd, addr, sizeof(sockaddr_in)) < 0) {
			fprintf(stderr, "Sockets::Bind\n");
		}
	}

	inline void listen(const int fd) {
		if (::listen(fd, SOMAXCONN) < 0) {
			fprintf(stderr, "Sockets::Listen\n");
		}
	}

	inline int accept(const int fd, sockaddr_in *addr) {
		auto len = static_cast<socklen_t>(sizeof(*addr));
		const int connFd = ::accept4(fd, (sockaddr *) addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
		if (connFd < 0) {
			fprintf(stderr, "Sockets::Accept\n");
		}
		return connFd;
	}

	inline void setSockOps(const int fd) {
		constexpr int opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *) &opt, sizeof(opt));
	}

	inline void close(const int fd) {
		if (::close(fd) < 0) {
			fprintf(stderr, "Sockets::Close\n");
		}
	}

	inline void shutdownWrite(const int fd) {
		if (::shutdown(fd, SHUT_WR) < 0) {
			fprintf(stderr, "sockets::shutdownWrite\n");
		}
	}

	inline ssize_t read(const int fd, void *buf, const size_t count) {
		return ::read(fd, buf, count);
	}

	inline ssize_t write(const int fd, const void *buf, const size_t count) {
		return ::write(fd, buf, count);
	}
}