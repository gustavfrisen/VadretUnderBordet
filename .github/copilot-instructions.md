#VädretUnderBordet

## Project Overview
VädretUnderBordet is a C-based weather client-server application for WeatherTech Solutions. The project implements a distributed weather data system with TCP/HTTP communication layers.

## Architecture

### Core Components
- **`client/`** - Weather data client application
- **`server/`** - Weather data server application  
- **`libs/`** - Shared networking and utility libraries
  - `TCP.c/h` - Low-level TCP socket operations
  - `HTTP.c/h` - HTTP protocol implementation layer
  - `utils.h` - Common utilities and data structures

### Communication Flow
Client communicates with server through the layered networking stack: Application → HTTP layer → TCP layer → Network

## Development Guidelines

### File Organization
- Keep networking protocols in separate abstraction layers (`TCP.c` for sockets, `HTTP.c` for protocol)
- Place shared functionality in `libs/` directory
- Separate client and server executables but share common libraries

### C Programming Conventions
- Use standard C (compatible with common compilers)
- Include guards in all header files
- Forward declare structs and functions appropriately
- Handle memory allocation/deallocation consistently

### Building and Testing
This project uses manual compilation - no build system is currently configured. When adding build instructions:
- Consider Makefile for cross-platform compatibility
- Account for Windows/Linux socket differences (`winsock2.h` vs `sys/socket.h`)
- Link appropriate network libraries (`-lws2_32` on Windows, standard on Linux)

### Network Programming Patterns
- Implement proper error handling for socket operations
- Use non-blocking I/O where appropriate for server scalability  
- Handle endianness for network byte order
- Implement graceful connection shutdown

### Key Integration Points
- TCP layer should provide socket abstraction for HTTP layer
- HTTP layer should handle protocol formatting/parsing
- Client and server should use identical message formats
- Weather data structures should be defined in shared headers

## Common Tasks

### Adding New Features
1. Define data structures in `libs/utils.h`
2. Implement protocol handling in `libs/HTTP.c`
3. Add client-side functionality in `client/main.c`
4. Add server-side handling in `server/main.c`

### Debugging Network Issues
- Check socket creation and binding in TCP layer
- Verify message formatting in HTTP layer
- Test with localhost before remote connections
- Use network monitoring tools (netstat, tcpdump)