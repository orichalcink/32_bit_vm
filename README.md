# 32bit Virtual Machine

Project by chalcinxx (orichalcink) based off LC-3.
[Videos on topic.](https://www.youtube.com/playlist?list=PLAYMpoWModGOzP_LNhaJDvMbUxX_9OI90)

Resources:
- [LC-3 16bit opcode documentation](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf)
- [Guide for creating LC-3 VM](https://www.jmeiners.com/lc3-vm/)

This virtual machine has memory and a CPU that executes some basic instructions. It reads a file, turns the contents into tokens for parsing, translates the labels into memory addresses and handles includes, parses the tokens into instructions and stores them into memory and executes the instructions one by one. The commands can be found in opcodes.hpp file, where their bit size and functions are documented.
