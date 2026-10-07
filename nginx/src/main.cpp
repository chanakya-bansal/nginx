#include "server.hpp"

#include <cstdlib>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

void spawnWorker(uint16_t port) {
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "fork() failed\n";
        return;
    }

    if (pid == 0) {
        // Child: create its OWN socket+epoll, run forever.
        Server worker(port);
        if (!worker.init()) {
            std::cerr << "Worker " << getpid() << " failed to init\n";
            _exit(1);
        }
        std::cout << "Worker " << getpid() << " ready\n";
        worker.runWorker();
        _exit(0); // only reached if runWorker() somehow returns
    }
    // Parent just returns — pid is the new worker's PID, caller tracks it if needed.
}  

int main(int argc, char* argv[]) {
    uint16_t port = 8080;
    if (argc > 1) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    //unsigned int num_workers = std::thread::hardware_concurrency();
    unsigned int num_workers =20;
    if (num_workers == 0) num_workers = 2;
    std::cout << "Spawning " << num_workers << " worker processes (SO_REUSEPORT)\n";

    for (unsigned int i = 0; i < num_workers; ++i) {
        spawnWorker(port);
    }

    // Master: monitor workers, restart any that crash.
    while (true) {
        int status;
        pid_t dead_pid = wait(&status);

        if (dead_pid == -1) {
            if (errno == EINTR) continue;
            break;
        }

        std::cerr << "Worker " << dead_pid << " died, restarting...\n";
        spawnWorker(port);
    }

    return 0;
}