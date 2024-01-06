#pragma once

#include <sys/epoll.h>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <cassert>

namespace {
	const size_t kMAX_EVENT_NUM = 1024;
	const size_t kBUFFER_SIZE = 1024;
}

namespace cm {
	class Epoll {
	public:
		Epoll() : epoll_fd_(epoll_create(EPOLL_CLOEXEC)), events_(kMAX_EVENT_NUM) {
			assert(epoll_fd_ >= 0 && !events_.empty());
		}

		~Epoll() {
			close(epoll_fd_);
		}

		static int set_no_blocking(int fd) {
			int old_option = fcntl(fd, F_GETFL);
			int new_option = old_option | O_NONBLOCK;
			fcntl(fd, F_SETFL, new_option);
			return old_option;
		}

		bool insert(int fd, uint32_t events, bool ones_hot) const {
			if (fd < 0)return false;
			epoll_event event{};
			event.data.fd = fd;
			event.events = events;

			if (ones_hot) {
				event.events |= EPOLLONESHOT;
			}
			int res = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
			set_no_blocking(fd);
			return res == 0;
		}

		bool update(int fd, uint32_t events) const {
			if (fd < 0)return false;
			epoll_event event{};
			event.data.fd = fd;
			event.events = events;
			return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);
		}

		bool remove(int fd) const {
			if (fd < 0)return false;
			epoll_event event = {0};
			return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event);
		}

		int wait(int timeout) {
			return epoll_wait(epoll_fd_, &events_[0], static_cast<int>(events_.size()), timeout);
		}

		int get_event_fd(int i) const {
			assert(i >= 0 && i < events_.size());
			return events_[i].data.fd;
		}

		uint32_t get_event(int i) const {
			assert(i >= 0 && i < events_.size());
			return events_[i].events;
		}

	private:
		int epoll_fd_;
		std::vector<epoll_event> events_;
	};
}