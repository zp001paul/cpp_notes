#!/usr/bin/env python3
"""Module daemonstrating python file IO"""
import os


def main():
    """main function"""
    with open("/home/zp001/hello1.txt", "w", encoding="utf-8") as fh:
        fh.write("hello\n")
        fh.flush()  # flush() doesn't call fsync()
        # os.fsync(fh)


if __name__ == "__main__":
    main()
