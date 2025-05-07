#!/usr/bin/python3

import os
import sys

print("Content-Type: text/plain")
print()

content_length = int(os.environ.get('CONTENT_LENGTH', 0))

if content_length > 0:
    post_data = sys.stdin.read(content_length)
    print("Received POST data:")
    print(post_data)
else:
    print("No POST data received")
