# VädretUnderBordet

A C-based weather client-server application for WeatherTech Solutions. The project implements a distributed weather data system with TCP/HTTP communication layers.

## 🏗️ Project Overview

VädretUnderBordet consists of a lightweight HTTP weather server and a corresponding client application that communicate over TCP sockets. The server provides weather data in JSON format, while the client makes HTTP requests to retrieve this information.

## 📁 Project Structure

```
VadretUnderBordet/
├── README.md                 # Project documentation
├── client/                   # Weather client application
│   ├── main.c               # Client implementation
│   └── Makefile             # Client build configuration
├── server/                   # Weather server application
│   ├── main.c               # Server implementation
│   └── Makefile             # Server build configuration
└── libs/                     # Shared networking libraries
    ├── tcp.c/.h             # TCP socket operations
    ├── http.c/.h            # HTTP protocol implementation
    └── utils.h              # Common utilities and macros
```

## 🛠️ Dependencies

### System Requirements
- **Operating System**: Linux/WSL (Windows Subsystem for Linux)
- **Compiler**: GCC (GNU Compiler Collection)
- **Build Tool**: Make

### Installation on Ubuntu/WSL
```bash
# Update package list
sudo apt update

# Install build essentials (includes GCC and Make)
sudo apt install build-essential

# Verify installation
gcc --version
make --version
```

### Standard C Libraries Used
- `stdio.h` - Standard input/output operations
- `string.h` - String manipulation functions
- `stdlib.h` - Standard library functions (atoi, etc.)
- `unistd.h` - POSIX operating system API (read, write, close)
- `sys/types.h` - System data types
- `sys/socket.h` - Socket programming (Linux)
- `netinet/in.h` - Internet address family
- `arpa/inet.h` - Definitions for internet operations

## 🚀 Building the Project

### Build Server
```bash
# Navigate to server directory
cd server/

# Compile server
make

# Clean build files (optional)
make clean
```

### Build Client
```bash
# Navigate to client directory  
cd client/

# Compile client
make

# Clean build files (optional)
make clean
```

## 🏃‍♂️ Running the Applications

### Start the Server
```bash
# Navigate to server directory
cd server/

# Run server on default port (8080)
./weather_server

# Run server on custom port
./weather_server 9000
```

**Expected Output:**
```
Listening on 8080
```

### Run the Client
```bash
# Navigate to client directory (in a new terminal)
cd client/

# Connect to server on localhost:8080
./weather_client

# Connect to custom host and port
./weather_client 192.168.1.100 9000
```

## 🧪 Testing the Server

### Method 1: Using the Client Application
```bash
# Terminal 1 - Start server
cd server && ./weather_server

# Terminal 2 - Run client
cd client && ./weather_client
```

### Method 2: Using curl
```bash
# Basic request
curl http://localhost:8080

# Verbose output (see full HTTP exchange)
curl -v http://localhost:8080

# Test different endpoints
curl http://localhost:8080/weather
curl -X POST http://localhost:8080
```

### Method 3: Using Web Browser
Navigate to: `http://localhost:8080`

## 📡 API Documentation

### Server Endpoints

#### GET / (Any path)
**Description:** Returns current weather data in JSON format

**Request Example:**
```http
GET /weather HTTP/1.1
Host: localhost
Connection: close
```

**Successful Response (200 OK):**
```json
{
  "weather": "sunny",
  "temp": 20
}
```

**Error Response (400 Bad Request):**
```json
{
  "error": "bad request"
}
```

### Network Configuration
- **Default Server Address:** `127.0.0.1` (localhost)
- **Default Port:** `8080`
- **Protocol:** HTTP/1.1 over TCP
- **Data Format:** JSON
- **Connection Type:** Close after each request

## 🏛️ Architecture

### Communication Flow
```
Client Application → HTTP Layer → TCP Layer → Network → Server
```

### Core Components

#### TCP Layer (`libs/tcp.c/h`)
- **`tcp_listen(port, backlog)`** - Creates listening socket
- **`tcp_accept(listen_fd)`** - Accepts incoming connections  
- **`tcp_connect(host, port)`** - Connects to remote server
- **`tcp_read_all(fd, buf, maxlen)`** - Reads data from socket
- **`tcp_write_all(fd, buf, len)`** - Writes data to socket

#### HTTP Layer (`libs/http.c/h`)
- **`http_parse_request()`** - Parses incoming HTTP requests
- **`http_build_response()`** - Constructs HTTP responses
- **`http_request_t`** - Structure for HTTP request data

#### Utilities (`libs/utils.h`)
- **`ARRAY_LEN(x)`** - Macro for calculating array length

## 🔧 Development Guidelines

### Adding New Features
1. Define data structures in `libs/utils.h`
2. Implement protocol handling in `libs/http.c`
3. Add client functionality in `client/main.c`
4. Add server handling in `server/main.c`

### Code Standards
- Use standard C (C99 compatible)
- Include guards in all header files: `#ifndef HEADER_H`
- Handle memory allocation/deallocation consistently
- Implement proper error handling for socket operations

### Network Programming Best Practices
- Check return values for all socket operations
- Handle network byte order (endianness)
- Implement graceful connection shutdown
- Use appropriate buffer sizes (4KB-8KB for HTTP)

## 🐛 Troubleshooting

### Common Issues

#### "Address already in use" Error
```bash
# Kill process using port 8080
sudo lsof -ti:8080 | xargs kill -9

# Or wait ~2 minutes for socket cleanup
```

#### "Connection refused" Error  
- Ensure server is running: `./weather_server`
- Check server is listening: `netstat -ln | grep 8080`
- Verify firewall settings allow port 8080

#### Compilation Errors
```bash
# Install missing dependencies
sudo apt install build-essential

# Check GCC installation
gcc --version
```

#### WSL-Specific Issues
```bash
# Ensure you're in the correct directory
pwd
# Should show: /mnt/c/Academy/VadretUnderBordet/server (or client)

# Use correct path format in WSL
cd /mnt/c/Academy/VadretUnderBordet/server
```

### Testing Commands
```bash
# Check if server is running
ps aux | grep weather_server

# Monitor network connections  
netstat -tlnp | grep 8080

# Test server responsiveness
timeout 5 curl http://localhost:8080
```

## 🚦 Build Status & Warnings

The project compiles successfully with minor warnings:
- `warning: ignoring return value of 'write'` - Non-critical, program functions correctly

To suppress warnings, modify code to handle return values:
```c
ssize_t result = write(cfd, out, strlen(out));
if (result < 0) {
    perror("write failed");
}
```

## 📊 Performance Notes

- **Concurrent Connections:** Server handles one connection at a time (single-threaded)
- **Buffer Sizes:** 4KB input buffer, 8KB response buffer
- **Memory Usage:** Minimal (~17KB executable size)
- **Network Latency:** Low latency for localhost connections

## 👥 Development Team

**WeatherTech Solutions** - VädretUnderBordet Development Team

## 📄 License

Internal project for WeatherTech Solutions.
