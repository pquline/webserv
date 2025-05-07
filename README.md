# 🚀 WebServ

![Webserv](https://github.com/user-attachments/assets/a0bbd1bd-dfb1-4387-9ba9-2e4a6981bda1)

A lightning-fast ⚡ custom HTTP server implementation in C++98, designed to handle HTTP requests and serve web content with style! 🎨

Team: [icr4m](https://github.com/icr4m), [Roychrltt](https://github.com/Roychrltt), [pquline](https://github.com/pquline)

## ✨ Features

- 🎯 Multi-server configuration support with multiple ports
- 🛠️ Configurable server blocks with multiple locations
- 🔄 Support for GET, POST, and DELETE methods
- 🐍 CGI script execution support
- 📂 Auto-indexing for directories
- ❌ Custom error pages
- 📏 Client body size limits
- 🌐 Server name and alias support
- 🔀 URL redirection support
- 📁 Multiple root directories support

## 📋 Requirements

- 🛠️ C++98 compatible compiler
- 🔧 Make
- 🐧 Linux/Unix environment

## 🏗️ Building the Project

```bash
# Build the project
make

# Build with debug flags
make debug

# Clean object files
make clean

# Clean everything and rebuild
make re
```

### 🔧 Configuration Options

- 🎧 `listen`: Port number(s) to listen on (multiple ports supported)
- 📦 `client_max_body_size`: Maximum size of client request body
- 🌐 `server_name`: Server name and optional aliases
- 📁 `root`: Root directory for serving files
- 📂 `autoindex`: Enable/disable directory listing
- 🔄 `methods`: Allowed HTTP methods (GET, POST, DELETE)
- ❌ `error_page`: Custom error pages
- 🔀 `redirection`: URL redirections
- 📑 `index`: Default index files for locations

## 🚀 Usage

1. ⚙️ Configure your server in `webserv.conf`
2. 🏗️ Build the project using `make`
3. 🚀 Run the server: `./webserv`

