# 🚀 WebServ

A lightning-fast ⚡ custom HTTP server implementation in C++98, designed to handle HTTP requests and serve web content with style! 🎨

## ✨ Features

- 🎯 Multi-server configuration support
- 🛠️ Configurable server blocks with multiple locations
- 🔄 Support for GET, POST, and DELETE methods
- 🐍 CGI script execution
- 📂 Auto-indexing for directories
- ❌ Custom error pages
- 📏 Client body size limits
- 🌐 Server name and alias support
- 🔀 Redirection support

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

## ⚙️ Configuration

The server is configured using a configuration file (`webserv.conf`). Here's an example configuration:

```nginx
server {
    listen 8080;
    client_max_body_size 80000000;
    server_name example.com;
    root ./www/;
    autoindex on;

    location / {
        index index.html;
        methods GET POST DELETE;
    }
}
```

### 🔧 Configuration Options

- 🎧 `listen`: Port number(s) to listen on
- 📦 `client_max_body_size`: Maximum size of client request body
- 🌐 `server_name`: Server name and optional aliases
- 📁 `root`: Root directory for serving files
- 📂 `autoindex`: Enable/disable directory listing
- 🔄 `methods`: Allowed HTTP methods
- ❌ `error_page`: Custom error pages
- 🔀 `redirection`: URL redirections

## 📁 Project Structure

```
webserv/
├── 📂 includes/     # Header files
├── 📂 srcs/         # Source files
├── 🌐 www/          # Web root directory
├── 🔧 Makefile      # Build configuration
└── ⚙️ webserv.conf  # Server configuration
```

## 🚀 Usage

1. ⚙️ Configure your server in `webserv.conf`
2. 🏗️ Build the project using `make`
3. 🚀 Run the server: `./webserv`

