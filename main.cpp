#include <iostream>
#include "include/TcpFileClient.hpp"

int main(const int argc, char **argv) {
	if (argc != 5) {
		std::cout << "argv = ip port fileName read/write (1/2)" <<std::endl;
		return 0;
	}
	const InetAddress addr(argv[1], std::strtol(argv[2], nullptr, 10));
	cm::TcpFileClient client(addr);
	if (client.connect()) {
		std::cout << "connect " << std::endl;
	} else {
		return 0;
	}
	if (std::strtol(argv[4], nullptr, 10) == 1) {
		std::cout << "file read size: " << client.read_file(argv[3]) << std::endl;
	} else if (std::strtol(argv[4], nullptr, 10) == 2) {
		std::cout << "file write size: " << client.write_file(argv[3]) << std::endl;
	} else {
		std::cout << "no such option" << std::endl;
	}
	return 0;
}