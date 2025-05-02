#!/usr/bin/env python3

import time
import sys

while True:
    sys.stdout.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}]</br>")
    sys.stdout.flush()
    time.sleep(1)
