#!/usr/bin/env python3

import os
import sys


def main():
    with open("/mnt/ext4/hello.txt", "wb") as fh:
        fh.write("hello\n".encode("utf-8"))
        fh.flush()
        # os.fsync(fh)
    return 0


if __name__ == "__main__":
    sys.exit(main())
