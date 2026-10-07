#pragma once

#include <cstdint>

class Server {
public:
    Server(uint16_t port, int backlog = 128);
    ~Server();

    // Creates its OWN socket (with SO_REUSEPORT), binds, listens,
    // sets up epoll, and runs forever. Call this once per worker,
    // AFTER forking — no socket is shared between processes anymore.
    bool init();
    void runWorker();

private:
    uint16_t port_;
    int backlog_;
    int listen_fd_ = -1;
    int epoll_fd_ = -1;

    bool setNonBlocking(int fd);
    void acceptNewConnection();
    void handleClientReadable(int client_fd);
};