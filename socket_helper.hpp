#pragma once

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>

#include <string_view>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <vector>

#include <cpp_helper.hpp>

namespace socket_helper {

using cpp_helper::configure;
using cpp_helper::empty_configurable_class;

enum class communication_style : int {
    stream = SOCK_STREAM,
    datagram = SOCK_DGRAM,
    raw = SOCK_RAW,
};

static
auto communication_style_to_string_map = std::unordered_map<communication_style, std::string_view>({
        {communication_style::stream, "stream"},
        {communication_style::datagram, "datagrm"},
        {communication_style::raw, "raw"},
});

template<typename T>
class add_socket : public T {
public:
    using parent = T;
    add_socket(const configure auto& conf) : parent{conf} {
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            throw std::runtime_error{"socket create fail"};
        }
    }
    ~add_socket() {
        int ret = close(sock);
        if (ret < 0) {
            std::cerr << "close socket fail" << std::endl;
        }
    }

    int get_socket() {
        return sock;
    }
private:
    int sock;
};

template<typename T>
class connect_address_port : public T {
public:
    using parent = T;
    connect_address_port(const configure auto& conf) : parent{conf} {
        auto address = parent::get_address();
        auto port = parent::get_port();
        hostent* hostinfo;
        sockaddr_in name{};
        name.sin_family = AF_INET;
        name.sin_port = htons(port);
        hostinfo = gethostbyname(address);
        if (hostinfo == NULL) {
            throw std::runtime_error{"Unknown host"};
        }
        name.sin_addr = *(in_addr*)hostinfo->h_addr;

        auto socket = parent::get_socket();
        if (0 > connect(socket, (sockaddr*)&name, sizeof(name))) {
            throw std::runtime_error{"connect fail"};
        }
    }
};

template<typename T>
class add_pollfd : public T {
public:
    using parent = T;
    static constexpr int FD_INDEX = parent::FDS_SIZE;
    static constexpr int FDS_SIZE = parent::FDS_SIZE+1;
    void process_events(auto& fds) {
        if (fds[FD_INDEX].revents & POLLIN) {
            std::cout << "socket event process" << std::endl;
            parent::process_socket_event(fds[FD_INDEX]);
        }
    }
    std::vector<pollfd> get_fds() {
        auto fds = parent::get_fds();
        fds.emplace_back(parent::get_fd());
        return fds;
    }
};

} // namespace socket_helper
