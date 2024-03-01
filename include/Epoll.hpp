#pragma once

#include <sys/epoll.h>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <cassert>

namespace cm {
    constexpr size_t kMAX_EVENT_NUM = 1024;
    constexpr size_t kBUFFER_SIZE = 1024;

    class Epoll {
    public:
        Epoll() : epoll_fd_(epoll_create(EPOLL_CLOEXEC)), events_(kMAX_EVENT_NUM) {
            assert(epoll_fd_ >= 0 && !events_.empty());
        }

        ~Epoll() {
            close(epoll_fd_);
        }

        static int set_no_blocking(const int fd) {
            const int old_option = fcntl(fd, F_GETFL);
            fcntl(fd, F_SETFL, old_option | O_NONBLOCK);
            return old_option;
        }

        [[nodiscard]] bool insert(const int fd, const uint32_t events, const bool ones_hot) const {
            if (fd < 0)return false;
            epoll_event event{};
            event.data.fd = fd;
            event.events = events;

            if (ones_hot) {
                event.events |= EPOLLONESHOT;
            }
            const int res = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
            set_no_blocking(fd);
            return res == 0;
        }

        [[nodiscard]] bool update(const int fd, const uint32_t events) const {
            if (fd < 0) return false;
            epoll_event event{};
            event.data.fd = fd;
            event.events = events;
            return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);
        }

        [[nodiscard]] bool remove(const int fd) const {
            if (fd < 0)return false;
            epoll_event event = {0};
            return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event);
        }

        int wait(const int timeout) {
            return epoll_wait(epoll_fd_, &events_[0], static_cast<int>(events_.size()), timeout);
        }

        [[nodiscard]] int get_event_fd(const int i) const {
            assert(i >= 0 && i < events_.size());
            return events_[i].data.fd;
        }

        [[nodiscard]] uint32_t get_event(const int i) const {
            assert(i >= 0 && i < events_.size());
            return events_[i].events;
        }

    private:
        int epoll_fd_;
        std::vector<epoll_event> events_;
    };
}
