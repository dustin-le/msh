# Mav Shell

## Overview
Bash-like shell with basic functionality, such as navigating through the file system, handling signals, and executing previous commands.

## Functionality
* Handles any system commands in the /bin, /usr/bin, /usr/local/bin/, and current working directory.
* Lists process identifiers of last 15 processes spawned by shell.
* Lists the last 50 *valid* commands entered.
* Can execute a command from history.
* Ignores SIGINT and SIGTSTP signals.

## What I Learned
* How a shell works in UNIX systems.
* How to handle multiple processes at a time.
* Signal handling in C.
* More efficient data structure management.
