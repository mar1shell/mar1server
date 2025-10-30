# mar1server

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Version](https://img.shields.io/badge/version-0.1.0-blue)
[![Docker](https://img.shields.io/badge/docker-ready-blue)](https://www.docker.com/)

A lightweight HTTP/1.1 server implementation in C, built from scratch as a learning project to understand low-level network programming, HTTP protocol internals, and systems programming.

> ⚠️ **This is a learning project and NOT production-ready.** It contains known security vulnerabilities and is intended for educational purposes only.

## 🎯 Project Goals

This project aims to understand:

- Socket programming and TCP/IP networking
- HTTP/1.1 protocol implementation
- Process management with `fork()`
- Signal handling for graceful shutdowns
- Memory management in C
- Building robust system software

## ✨ Features (v0.1)

- ✅ HTTP/1.1 GET request handling
- ✅ Static file serving (HTML, CSS, JS, JSON, images)
- ✅ MIME type detection
- ✅ Custom error pages (400, 404, 405, 500, 501)
- ✅ Multi-process architecture (fork per connection)
- ✅ Graceful shutdown with signal handling (Ctrl+C)
- ✅ Path traversal protection
- ✅ Request/response logging
- ✅ Docker support for easy deployment

## 🚀 Quick Start

### Option 1: Docker

The easiest way to run mar1server without installing dependencies:

```bash
# Build the Docker image
docker build -t mar1server .

# Run the container
docker run -d -p 9001:9001 --name mar1server mar1server

# View logs
docker logs -f mar1server

# Stop the server
docker stop mar1server
docker rm mar1server
```

### Option 2: Native Build

#### Prerequisites

- GCC compiler
- Make
- POSIX-compliant system (Linux/macOS)

#### Build & Run

```bash
# Clone the repository
git clone https://github.com/mar1shell/mar1server.git
cd mar1server

# Build the server
make

# Run on default port (9001)
./bin/mar1server

# Run on custom port
./bin/mar1server 8080
```

### Test It Out

```bash
# In another terminal
curl http://localhost:9001/
curl http://localhost:9001/index.html
curl http://localhost:9001/js.json
```

Open your browser and navigate to `http://localhost:9001`

## 📁 Project Structure

```
mar1server/
├── include/                # Header files
│   ├── constants.h        # Configuration & constants
│   ├── controller.h       # Request routing
│   ├── http_status.h      # HTTP status codes
│   ├── requests.h         # HTTP request parsing
│   ├── responses.h        # HTTP response building
│   ├── server.h           # Server initialization
│   ├── signals.h          # Signal handling
│   └── files_utils.h      # File operations
├── src/                    # Source files
│   ├── main.c             # Entry point
│   ├── controller/        # Request handlers
│   ├── http/              # HTTP protocol logic
│   ├── server/            # Socket & server setup
│   └── utils/             # Utility functions
├── www/                    # Static files directory
│   ├── index.html         # Default page
│   ├── 404.html           # Error pages
│   └── ...
├── Dockerfile             # Docker build instructions
├── .dockerignore          # Docker build exclusions
├── makefile               # Build configuration
└── README.md
```

## 🔧 Configuration

Edit `include/constants.h` to customize:

```c
#define PORT 9001                          // Default port
#define BACKLOG 10                         // Connection queue size
#define MAX_REQUEST_SIZE 16384             // 16 KB max request
#define STATIC_FILES_PATH "/app/www"       // Static files (Docker)
// OR
#define STATIC_FILES_PATH "./www"          // Static files (native)
```

## 🛠️ Development

### Build Commands

```bash
make              # Build the project
make clean        # Remove compiled files
make run          # Build and run
```

## 📝 What I Learned

- **Socket Programming**: Creating, binding, listening on TCP sockets
- **HTTP Protocol**: Request parsing, response formatting, status codes
- **Process Management**: Fork-based concurrency, zombie reaping
- **Signal Handling**: Graceful shutdowns with SIGINT/SIGTERM
- **Memory Safety**: Buffer management, avoiding overflows
- **Security Basics**: Path traversal prevention, input validation
- **Docker**: Multi-stage builds, containerization best practices

## 🚧 Roadmap (v0.2 and beyond)

After learning more, the next version will include:

- 🔄 **Thread pool** instead of fork-per-request
- ⚡ **Event-driven I/O** with epoll/kqueue
- 🔐 **HTTPS/TLS** support
- 📊 **Better logging** system with log levels
- 🎛️ **Configuration file** support (YAML/JSON)
- 🛡️ **Rate limiting** and DoS protection
- 📝 **POST/PUT/DELETE** method support
- 🗜️ **Compression** (gzip/brotli)
- 🔌 **Keep-alive** connections
- 🧪 **Unit tests** and CI/CD pipeline
- 📦 **Binary file** serving optimization
- 🌐 **Virtual hosts** support

## 🤝 Contributing

This is a personal learning project, but suggestions and feedback are welcome! Feel free to open issues for discussion.

⭐ If you're also learning C and network programming, feel free to star this repo and follow along!

---

<p align="center">Made with ❤️ by <a href="https://github.com/mar1shell">mar1shell</a></p>
