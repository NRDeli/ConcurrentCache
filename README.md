# ConcurrentCache

### High-Performance Multi-Threaded In-Memory Cache Server (C++)

ConcurrentCache is a **production-style, multi-threaded in-memory cache
server written in modern C++20**.\
It implements a simplified Redis/Memcached-like architecture featuring
**event-driven networking, lock-striped concurrency, TTL expiration, LRU
eviction, metrics instrumentation, and benchmarking**.

The project demonstrates **systems engineering fundamentals** relevant
to backend infrastructure, distributed systems, and high-performance
server design.

------------------------------------------------------------------------

# 🧠 Architecture Overview

The server follows a **multi-threaded reactor architecture**:

Client Connections\
↓\
TCP Acceptor\
↓\
Worker Thread Pool (Event Loops)\
↓\
Sharded Concurrent Cache Engine\
↓\
LRU + TTL + Metrics

Key design properties:

• Event-driven networking via **Reactor pattern**\
• **Lock-striped sharded cache** for high concurrency\
• **Background expiration thread** for proactive TTL cleanup\
• **LRU eviction policy** under memory pressure\
• **Atomic metrics counters** for observability\
• **Benchmark client** for throughput measurement

------------------------------------------------------------------------

# ⚙️ Core Features

## 🌐 Event-Driven TCP Server

The server implements a **non-blocking TCP architecture** where worker
threads manage multiple connections simultaneously.

Key components:

  Component      Responsibility
  -------------- -----------------------------------
  `TcpServer`    Accepts incoming connections
  `Worker`       Processes client I/O and commands
  `Reactor`      Handles socket readiness events
  `Connection`   Maintains per-client buffers

Properties:

• Non-blocking sockets\
• Worker thread pool\
• Efficient command parsing\
• High concurrency handling

------------------------------------------------------------------------

## 🧵 Multi-Threaded Worker Architecture

Each worker runs an independent event loop:

    while(running):
        wait_for_events()
        read_requests()
        parse_commands()
        execute_cache_operations()
        write_responses()

Advantages:

• Parallel request processing\
• No global networking lock\
• Efficient CPU utilization

------------------------------------------------------------------------

## 🧱 Sharded Concurrent Cache

To reduce lock contention the cache uses **lock striping**.

    Cache
     ├─ Shard 0 (mutex)
     ├─ Shard 1 (mutex)
     ├─ Shard 2 (mutex)
     └─ Shard N (mutex)

Key mapping:

    hash(key) % shard_count

Benefits:

• Multiple threads access different shards simultaneously\
• Eliminates global locking bottleneck\
• Improves throughput under heavy load

------------------------------------------------------------------------

## ⏳ TTL Expiration

Cache entries support **time-to-live semantics**.

Each entry stores:

    value
    expire_at_ms

Expiration occurs via:

### Lazy Expiration

During GET operations:

    if current_time > expire_at:
        delete entry

### Active Expiration

A background thread periodically scans shards to remove expired entries.

------------------------------------------------------------------------

## 🔄 Background Expiration Thread

A maintenance thread periodically runs:

    while running:
        sleep(200ms)
        for each shard:
            scan_expired_entries()

Goals:

• Prevent memory accumulation from expired keys\
• Avoid full-cache scans\
• Maintain stable latency

------------------------------------------------------------------------

## 📉 LRU Eviction Policy

ConcurrentCache uses **Least Recently Used eviction** when memory limits
are exceeded.

Each shard maintains:

    unordered_map<string, Entry>
    list<string> lru

Entry structure:

    value
    expire_at_ms
    lru_iterator
    bytes

Operations:

  Operation   Effect
  ----------- --------------------------
  GET         Move key to front of LRU
  SET         Insert at front
  Eviction    Remove from back

Complexity:

    GET  : O(1)
    SET  : O(1)
    DEL  : O(1)
    LRU  : O(1)

------------------------------------------------------------------------

## 🧮 Memory Accounting

Each entry tracks approximate memory usage:

    bytes = key.size() + value.size()

Global limit:

    max_bytes

When exceeded:

    evict_until(memory < max_bytes)

------------------------------------------------------------------------

## 📊 Metrics & Observability

Atomic counters provide runtime insight:

  Metric      Description
  ----------- -------------------------------------
  requests    Total commands processed
  hits        Cache hits
  misses      Cache misses
  evictions   Keys removed due to memory pressure

Command:

    STATS

Example output:

    requests 10000
    hits 7000
    misses 3000
    evictions 12

------------------------------------------------------------------------

# 💬 Supported Commands

  Command              Description
  -------------------- ---------------------
  PING                 Health check
  SET key value        Store value
  GET key              Retrieve value
  DEL key              Delete key
  EXPIRE key seconds   Set TTL
  TTL key              Query remaining TTL
  STATS                Metrics output

Example session:

    SET a hello
    +OK

    GET a
    $5
    hello

    EXPIRE a 5
    :1

    TTL a
    :4

------------------------------------------------------------------------

# 🧪 Quick Functional Test

Start the server:

    ./concurrent-cache

Open another terminal and run:

    printf "PING\nSET a hello\nGET a\nDEL a\nSTATS\n" | nc localhost 7379

Example output:

    +PONG
    +OK
    $5
    hello
    :1
    requests 5
    hits 1
    misses 0
    evictions 0

------------------------------------------------------------------------

# ⏱️ TTL Test

    printf "SET temp value\nEXPIRE temp 5\nTTL temp\n" | nc localhost 7379

Example output:

    +OK
    :1
    :5

After \~5 seconds:

    printf "GET temp\n" | nc localhost 7379

Output:

    $-1

------------------------------------------------------------------------

# 📈 Benchmark Results

Local benchmark using **8 concurrent client threads**:

    Total requests: 400000
    Time: 2.947 sec
    Throughput: ~135,000 ops/sec

Environment:

MacBook Pro\
Apple Clang 17\
C++20\
Localhost networking

Throughput scales with:

• CPU cores\
• worker thread count\
• shard count

------------------------------------------------------------------------

# 📂 Project Structure

    ConcurrentCache/
    │
    ├── src/
    │   ├── server/
    │   │   ├── TcpServer
    │   │   ├── Worker
    │   │   ├── Reactor
    │   │   └── Connection
    │   │
    │   ├── cache/
    │   │   ├── ConcurrentCache
    │   │   ├── Shard
    │   │   └── Entry
    │   │
    │   └── util/
    │       ├── Logger
    │       ├── Clock
    │       └── Metrics
    │
    ├── tools/
    │   └── bench.cpp
    │
    ├── CMakeLists.txt
    └── README.md

------------------------------------------------------------------------

# ⚠️ Limitations

This implementation focuses on **systems architecture and concurrency
concepts** rather than full production completeness.

Current limitations:

• In-memory only (no persistence)\
• No replication or clustering\
• No authentication/security layer\
• Simplified network protocol\
• Approximate memory accounting\
• No pipelining support

------------------------------------------------------------------------

# 🧑‍💻 Resume Value

This project demonstrates several backend infrastructure competencies:

• Event-driven server architecture\
• Multi-threaded concurrency design\
• Lock-striped data structures\
• Cache eviction algorithms\
• Systems performance benchmarking

Example resume bullet:

**ConcurrentCache -- High-Performance In-Memory Cache (C++)**\
• Built a multi-threaded TCP cache server implementing sharded
concurrency, TTL expiration, and O(1) LRU eviction.\
• Designed lock-striped cache architecture with background expiration
and atomic metrics instrumentation.\
• Achieved \~135k ops/sec throughput under multi-threaded benchmark
load.

------------------------------------------------------------------------

# 🛠️ Build Instructions

Clone repository:

    git clone <repo>
    cd ConcurrentCache

Build:

    mkdir build
    cd build
    cmake ..
    make

Run server:

    ./concurrent-cache

Run benchmark:

    ./bench
