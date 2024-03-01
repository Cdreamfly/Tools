#pragma once

#include "SocketOps.hpp"
#include "InetAddress.hpp"
#include <netinet/tcp.h>

namespace cm {
    class Socket : private cm::noncopyable {
    public:
        explicit Socket(const int fd) : _fd(fd) {}

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
            const int fd = socketOps::accept(_fd, &addr);
            if (fd >= 0) {
                peerAddr.setSockAddr(addr);
            }
            return fd;
        }

        void setTcpNoDelay(const bool on) const {
            const int opt_val = on ? 1 : 0;
            setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
        }

        void setReuseAddr(const bool on) const {
            const int opt_val = on ? 1 : 0;
            setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
        }

        void setReusePort(const bool on) const {
            const int opt_val = on ? 1 : 0;
            if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &opt_val, static_cast<socklen_t>(sizeof(opt_val))) < 0 && on) {
                fprintf(stderr, "setReusePort error!\n");
            }
        }

        void setKeepAlive(const bool on) const {
            const int opt_val = on ? 1 : 0;
            setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &opt_val, static_cast<socklen_t>(sizeof(opt_val)));
        }

        void setNonblocking() const {
            fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
        }

    private:
        int _fd;
    };
}
