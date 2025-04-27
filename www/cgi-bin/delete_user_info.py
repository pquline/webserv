#!/usr/bin/env python3

import os
import sys
import cgi

def delete_user_info():
    try:
        # Get the server root directory from environment
        server_root = os.environ.get('DOCUMENT_ROOT', '')
        if not server_root:
            server_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

        user_info_path = os.path.join(server_root, "usrInfo.txt")

        # Debug output
        print("Content-Type: text/plain")
        print("")
        print(f"Server root: {server_root}")
        print(f"Attempting to delete: {user_info_path}")

        # Check if file exists and is writable
        if os.path.exists(user_info_path):
            if os.access(user_info_path, os.W_OK):
                os.remove(user_info_path)
                print("File deleted successfully")
            else:
                print(f"Permission denied: {user_info_path}")
                print(f"File permissions: {oct(os.stat(user_info_path).st_mode)[-3:]}")
        else:
            print("File not found")

        # Send success response
        print("HTTP/1.1 200 OK")
        print("Content-Type: text/plain")
        print("Content-Length: 2")
        print("")
        print("OK")
    except Exception as e:
        # Send error response
        print("HTTP/1.1 500 Internal Server Error")
        print("Content-Type: text/plain")
        error_msg = str(e)
        print(f"Content-Length: {len(error_msg)}")
        print("")
        print(error_msg)
        sys.exit(1)

if __name__ == "__main__":
    delete_user_info()
