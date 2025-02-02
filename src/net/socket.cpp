#include "socket.hpp"
#include <util/net.hpp>

int Socket::send(const std::string_view data) {
    return send(data.data(), data.size());
}

bool Socket::close() {
    return false;
}

Socket::~Socket() {
    this->close();
}