#pragma once

#include <arpa/inet.h>

namespace cm::socketOps {
	int createNonblockingSocket(sa_family_t family) {
		int fd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
		if (fd < 0) {
			fprintf(stderr, "Sockets::CreateNonblockingSocket\n");
		}
		return fd;
	}

	void bind(int fd, const sockaddr *addr) {
		if (::bind(fd, addr, sizeof(sockaddr_in)) < 0) {
			fprintf(stderr, "Sockets::Bind\n");
		}
	}

	void listen(int fd) {
		if (::listen(fd, SOMAXCONN) < 0) {
			fprintf(stderr, "Sockets::Listen\n");
		}
	}

	int accept(int fd, sockaddr_in *addr) {
		auto len = static_cast<socklen_t>(sizeof(*addr));
		int connFd = ::accept4(fd, (sockaddr *) addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
		if (connFd < 0) {
			fprintf(stderr, "Sockets::Accept\n");
		}
		return connFd;
	}

	void setSockOps(int fd) {
		int opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *) &opt, sizeof(opt));
	}

	void close(int fd) {
		if (::close(fd) < 0) {
			fprintf(stderr, "Sockets::Close\n");
		}
	}

	void shutdownWrite(int fd) {
		if (::shutdown(fd, SHUT_WR) < 0) {
			fprintf(stderr, "sockets::shutdownWrite\n");
		}
	}

	ssize_t read(int fd, void *buf, size_t count) {
		return ::read(fd, buf, count);
	}

	ssize_t write(int fd, const void *buf, size_t count) {
		return ::write(fd, buf, count);
	}
}