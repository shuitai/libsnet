# libsnet
Cross-platform asychronous network library

# Overview
libsnet is a multi-platform support library with a focus on asynchronous network library for TCP and UDP.

# Features
1. Cross-platform asychronous network library
2. Support multi-platform (windows, linux, android)
3. Support IOCP in windows, epoll in linux and android.

# Build Instructions
GYP is a meta-build system which can generate MSVS, Makefile. 
It is best used for integration into other projects.

First, Python 2.6 or 2.7 must be installed as it is required by GYP. If python is not in your path, 
set the environment variable PYTHON to its location. For example: set PYTHON=C:\Python27\python.exe

# Windows
To build with Visual Studio, launch a git shell (e.g. Cmd or PowerShell) and run gen_project.bat which will 
generate libsnet.sln as well as related project files.
