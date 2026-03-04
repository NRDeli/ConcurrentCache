#include <arpa/inet.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

void worker(int requests)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7379);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr *)&addr, sizeof(addr));

    std::string cmd = "SET key value\n";

    char buffer[64];

    for (int i = 0; i < requests; i++)
    {
        write(sock, cmd.data(), cmd.size());

        // read response to prevent socket buffer overflow
        read(sock, buffer, sizeof(buffer));
    }

    close(sock);
}

int main()
{
    int threads = 8;
    int per_thread = 50000;

    int total = threads * per_thread;

    std::vector<std::thread> workers;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < threads; i++)
        workers.emplace_back(worker, per_thread);

    for (auto &t : workers)
        t.join();

    auto end = std::chrono::high_resolution_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    double ops = total / seconds;

    std::cout << "Total requests: " << total << "\n";
    std::cout << "Time: " << seconds << " sec\n";
    std::cout << "Throughput: " << ops << " ops/sec\n";
}