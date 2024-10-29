Rush Shell Program

Overview:
  Rush is a simple Unix shell that supports basic command execution, parallel commands using the & operator, and output
  redirection using the > symbol. It includes built-in commands to navigate and manage the shell environment, such as exit,
  cd, and path. The shell searches for commands in specified paths and executes them using child processes.

Features:
  * Basic Command Execution: Supports executing commands with arguments.
  * Parallel Command Execution: Run multiple commands in parallel using the & operator.
  * Output Redirection: Redirect command output to a file using the > symbol.
  * Built-in Commands:
    * exit to terminate the shell.
    * cd to change the current working directory.
    * path to set executable search paths.

File structure:
  - rush.c          # Source code file containing the implementation of the shell
  - Makefile        # Makefile for building the shell program
  - README.md       # Documentation file (this file)

Prerequisites:
  * GCC Compiler: The program requires GCC to compile the source code. Make sure that GCC is installed on your system.
  * Unix-based Operating System: The shell program is designed to run on Unix-based operating systems like Linux or macOS.

Building the Program:
  To build the program, follow these steps:

  1) Open a terminal and navigate to the directory containing rush.c and the Makefile.
  2) Run the following command to build the program using the Makefile:
     make
  3) If the build is successful, an executable named rush will be generated in the same directory.

Running the Shell:
  To start the shell, execute the following command in the terminal:
    ./rush
  The shell will display a prompt (rush>) indicating that it is ready to accept commands.

Usage:
  * Built-in Commands
    * exit: Terminate the shell.
    * cd: Change the current working directory.
    * path: Set the search paths for executable commands. Each path should be separated by a space.
  * Parallel Commands:
    * Use the & operator to execute multiple commands in parallel. For example:
        ls & pwd & whoami
  * Output Redirection:
    * Redirect the output of a command to a file using the > operator. For example:
        ls > output.txt
  * Error Handling:
    * The shell will print an error message if a command fails to execute or if invalid input is provided. Common reasons for
      errors include incorrect command syntax, missing arguments, or an unavailable file path.
  * Cleaning Up:
    * To clean up the build files, run:
        make clean

Known Limitations:  
  * The shell does not support complex command pipelines or conditional statements.
  * It is designed to run on Unix-based systems only.
  * Memory management for dynamically allocated paths could be improved.

Author
Fariz Zeynalov
