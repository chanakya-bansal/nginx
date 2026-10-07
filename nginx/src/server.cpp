#include "server.hpp"
#include "http_parser.hpp"
#include "http_response.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(uint16_t port, int backlog)
    : port_(port), backlog_(backlog) {}

Server::~Server() {
    if (listen_fd_ != -1) close(listen_fd_);
    if (epoll_fd_ != -1) close(epoll_fd_);
}

bool Server::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1;
}

bool Server::init() {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ == -1) {
        std::cerr << "socket() failed: " << strerror(errno) << "\n";
        return false;
    }

    int opt = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // NEW: lets multiple independent sockets (one per worker) all
    // bind to the SAME port. The kernel load-balances incoming
    // connections across them internally.
    if (setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << "\n";
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        std::cerr << "bind() failed: " << strerror(errno) << "\n";
        return false;
    }

    if (listen(listen_fd_, backlog_) == -1) {
        std::cerr << "listen() failed: " << strerror(errno) << "\n";
        return false;
    }

    if (!setNonBlocking(listen_fd_)) {
        std::cerr << "failed to set listen socket non-blocking\n";
        return false;
    }

    return true;
}

void Server::acceptNewConnection() {
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            if (errno == EINTR) continue; // interrupted, just retry
            std::cerr << "accept() failed: " << strerror(errno) << "\n";
            break;
        }

        setNonBlocking(client_fd);

        epoll_event ev{};
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev);
    }
}

void Server::handleClientReadable(int client_fd) {
    char buf[4096];
    ssize_t n = read(client_fd, buf, sizeof(buf) - 1);

    if (n > 0) {
        std::string raw(buf, n);
        HttpRequest req = parseRequest(raw);
        std::string response = buildResponse(req);
        write(client_fd, response.c_str(), response.size());

        close(client_fd);
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr);
    } else if (n == 0) {
        close(client_fd);
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr);
    } else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            close(client_fd);
            epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr);
        }
    }
}

void Server::runWorker() {
    // Each worker process creates its OWN epoll instance —
    // this cannot be shared across processes like listen_fd_ can.
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        std::cerr << "epoll_create1() failed: " << strerror(errno) << "\n";
        return;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd_;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_fd_, &ev) == -1) {
        std::cerr << "epoll_ctl() ADD listen_fd failed: " << strerror(errno) << "\n";
        return;
    }

    std::cout << "Worker " << getpid() << " ready\n";

    constexpr int MAX_EVENTS = 64;
    epoll_event events[MAX_EVENTS];

    while (true) {
        int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
        if (n == -1) {
            if (errno == EINTR) continue;
            std::cerr << "epoll_wait() failed: " << strerror(errno) << "\n";
            break;
        }

        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == listen_fd_) {
                acceptNewConnection();
            } else {
                handleClientReadable(events[i].data.fd);
            }
        }
    }
}