#pragma once

#include <arpa/inet.h>

namespace cm::net::sockets {
	int createNonblockingOrDie(sa_family_t);

	int connect(int, const sockaddr *);

	void fromIpPort(const char *ip, uint16_t, sockaddr_in *);

	void close(int);

	void bindOrDie(int, const sockaddr *);

	void listenOrDie(int);

	int accept(int, sockaddr_in *);

	void shutdownWrite(int);

	ssize_t read(int, void *, size_t);

	ssize_t readv(int, const iovec *, int);

	ssize_t write(int, const void *, size_t);

	void setTcpNoDelay(int, bool);

	void setReuseAddr(int, bool);

	void setReusePort(int, bool);

	void setKeepAlive(int, bool);

	sockaddr_in getLocalAddr(int);

	sockaddr_in getPeerAddr(int);

	int getSocketError(int);

	bool isSelfConnect(int);
}


