#include "SocketOps.hpp"

#include <netinet/tcp.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/uio.h>

int cm::net::sockets::createNonblockingOrDie(const sa_family_t family) {
	const int fd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (fd < 0) {
		std::cerr << "sockets::createNonblockingOrDie" << std::endl;;
	}
	return fd;
}

int cm::net::sockets::connect(const int fd, const sockaddr *addr) {
	return ::connect(fd, addr, static_cast<socklen_t>(sizeof(sockaddr_in)));
}

void cm::net::sockets::fromIpPort(const char *ip, uint16_t port, sockaddr_in *addr) {
	addr->sin_family = AF_INET;
	addr->sin_port = htobe16(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
		std::cerr << "sockets::fromIpPort" << std::endl;
	}
}

void cm::net::sockets::close(const int fd) {
	if (::close(fd) < 0) {
		std::cerr << "sockets::close" << std::endl;;
	}
}

void cm::net::sockets::bindOrDie(const int fd, const sockaddr *addr) {
	if (::bind(fd, addr, sizeof(sockaddr_in)) < 0) {
		std::cerr << "sockets::bindOrDie" << std::endl;;
	}
}

void cm::net::sockets::listenOrDie(const int fd) {
	if (::listen(fd, SOMAXCONN) < 0) {
		std::cerr << "sockets::listen" << std::endl;;
	}
}

int cm::net::sockets::accept(const int fd, sockaddr_in *addr) {
	auto len = static_cast<socklen_t>(sizeof(*addr));
	const int connFd = ::accept4(fd, (sockaddr *) &addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (fd < 0) {
		const int savedErrno = errno;
		switch (savedErrno) {
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO:
			case EPERM:
			case EMFILE:
				errno = savedErrno;
				break;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				std::cerr << "unexpected error of ::accept %d" << savedErrno << std::endl;
				break;
			default:
				std::cerr << "unknown error of ::accept %d" << savedErrno << std::endl;;
				break;
		}
	}
	return connFd;
}

void cm::net::sockets::shutdownWrite(const int fd) {
	if (::shutdown(fd, SHUT_WR) < 0) {
		std::cerr << "sockets::shutdownWrite" << std::endl;
	}
}

ssize_t cm::net::sockets::read(const int fd, void *buf, const size_t count) {
	return ::read(fd, buf, count);
}

ssize_t cm::net::sockets::readv(const int fd, const iovec *iov, const int count) {
	return ::readv(fd, iov, count);
}

ssize_t cm::net::sockets::write(const int fd, const void *buf, const size_t count) {
	return ::write(fd, buf, count);
}

void cm::net::sockets::setTcpNoDelay(const int fd, const bool on) {
	const int opt = on ? 1 : 0;
	::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void cm::net::sockets::setReuseAddr(const int fd, const bool on) {
	const int opt = on ? 1 : 0;
	::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
}

void cm::net::sockets::setReusePort(const int fd, const bool on) {
	const int opt = on ? 1 : 0;
	const int ret = ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));
	if (ret < 0 && on) {
		std::cerr << "SO_REUSEPORT failed." << std::endl;
	}
}

void cm::net::sockets::setKeepAlive(const int fd, const bool on) {
	const int opt = on ? 1 : 0;
	::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof(opt)));
}

sockaddr_in cm::net::sockets::getLocalAddr(const int fd) {
	sockaddr_in localAddr{};
	memset(&localAddr, 0, sizeof(localAddr));
	auto addrLen = static_cast<socklen_t>(sizeof(localAddr));
	if (::getsockname(fd, (sockaddr *) &localAddr, &addrLen) < 0) {
		std::cerr << "sockets::getPeerAddr" << std::endl;
	}
	return localAddr;
}

sockaddr_in cm::net::sockets::getPeerAddr(const int fd) {
	sockaddr_in peerAddr{};
	memset(&peerAddr, 0, sizeof peerAddr);
	auto addrLen = static_cast<socklen_t>(sizeof peerAddr);
	if (::getpeername(fd, (sockaddr *) &peerAddr, &addrLen) < 0) {
		std::cerr << "sockets::getPeerAddr" << std::endl;
	}
	return peerAddr;
}

int cm::net::sockets::getSocketError(const int fd) {
	int optVal = 0;
	auto optLen = static_cast<socklen_t>(sizeof(optVal));
	if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &optVal, &optLen) < 0) {
		return errno;
	}
	return optVal;
}

bool cm::net::sockets::isSelfConnect(const int fd) {
	const sockaddr_in localAddr = getLocalAddr(fd);
	const sockaddr_in peerAddr = getPeerAddr(fd);
	if (localAddr.sin_family == AF_INET) {
		const sockaddr_in *lAddr = (&localAddr);
		const sockaddr_in *rAddr = (&peerAddr);
		return lAddr->sin_port == rAddr->sin_port && lAddr->sin_addr.s_addr == rAddr->sin_addr.s_addr;
	}
	return false;
}
