#pragma once

#include "Utils.hpp"

class InetAddress : public cm::copyable {
public:
	explicit InetAddress() = default;

	explicit InetAddress(const sockaddr_in &addr) : m_addr(addr) {}

	explicit InetAddress(uint16_t port, const std::string &ip = "127.0.0.1") {
		memset(&m_addr, 0, sizeof(m_addr));
		m_addr.sin_family = AF_INET;
		m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
		m_addr.sin_port = htons(port);
	}

	InetAddress(const std::string &ip, uint16_t port) {
		memset(&m_addr, 0, sizeof(m_addr));
		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr);
	}

	[[nodiscard]] const sockaddr *getSockAddr() const {
		return (const sockaddr *) &m_addr;
	}

	void setSockAddr(const sockaddr_in &addr) {
		m_addr = addr;
	}

	[[nodiscard]] sa_family_t family() const {
		return m_addr.sin_family;
	}

	[[nodiscard]] std::string getIP() const {
		char buf[64];
		inet_ntop(AF_INET, &m_addr.sin_addr, buf, static_cast<socklen_t>(sizeof(m_addr)));
		return buf;
	}

	[[nodiscard]] std::string toIpPort() const {
		char buf[64] = {0};
		inet_ntop(AF_INET, &m_addr.sin_addr, buf, static_cast<socklen_t>(sizeof(m_addr)));
		snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ":%u", this->getPort());
		return buf;
	}

	[[nodiscard]] uint16_t getPort() const {
		return be16toh(m_addr.sin_port);
	}

private:
	sockaddr_in m_addr{};
};