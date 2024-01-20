# MKSHELL: Custom UNIX Shell

## Overview
MKSHell is a personal project that I developed to create a custom UNIX shell in C. It serves as a demonstration of my strong system programming skills and understanding of UNIX/Linux environments. The goal of this project was to build a functional shell that incorporates essential shell commands, error handling, and a command history feature.

## Key Features
- **Custom Shell Commands**: Includes commonly used shell commands like cd, ls, cat, mkdir, cp, and more.
- **Command History**: Stores the last ten executed commands, providing a convenient way to recall and reuse previous commands.
- **Signal Handling**: Implements a signal handler for SIGINT (Ctrl+C) to gracefully handle interruptions.
- **Robust Error Handling**: Ensures the shell provides informative error messages when commands encounter issues.
- **Memory Management**: Efficiently manages memory for command history and tokenization.

## What I learned
- **System Programming**: Deepened my understanding of system programming concepts such as process management, inter-process communication, and signal handling.
- **C Programming**: Enhanced my proficiency in C, particularly in areas such as memory management, pointers, and error handling.
- **UNIX/Linux Environments**: Gained a better understanding of UNIX/Linux environments, file systems, and shell scripting.

## Installation and Usage
1. Clone the repository: `git clone https://github.com/MustakimK/mkshell.git`
2. Navigate to the project directory: `cd mkshell`
3. Compile the shell: `gcc -o mkshell mkshell.c`
4. Run the shell: `./mkshell`

For a list of available commands, type "help" within the shell.