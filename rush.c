/*
Name: Fariz Zeynalov
Description:
This program implements a simple Unix shell called "rush". It supports basic command execution, parallel commands using '&', and output
redirection with '>'.
Built-in commands include:
 - exit: to terminate the shell
 - cd: to change directories
 - path: to set executable search paths
Commands are executed by forking child processes and searching the specified paths. The shell continues to prompt the user
for input until "exit" is called.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


#define MAX_PATHS 100
#define MAX_ARGS 100
#define MAX_CMD_LEN 255



// Function prototypes
void print_error();
char* trim(char* str);
void execute_command(char **args, int output_fd);
void parse_and_execute(char *input, int is_parallel);
void execute_parallel(char *input);
//void rush_loop();   



// Global variables for paths
char *paths[MAX_PATHS] = {"/bin", NULL};  // Default search path
int path_count = 1;  // Number of paths



// Function to print an error message to stderr
void print_error() {
    char error_message[24] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));  // Write error to stderr
}



// Function to trim leading and trailing whitespace from a string
char* trim(char* str) {
    // Remove leading whitespace
    while (*str && (*str == ' ' || *str == '\t' || *str == '\n')) str++;
    if (*str == 0) return str;

    // Remove trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
    *(end + 1) = 0;
    return str;
}



// Function to execute a command with optional output redirection
void execute_command(char **args, int output_fd) {
    pid_t pid = fork();  // Fork a new process
    
    if (pid < 0) {
        print_error();  // Fork failed
        return;
    }
    
    if (pid == 0) {  // Child process
        // Redirect output if necessary
        if (output_fd != STDOUT_FILENO) {
            if (dup2(output_fd, STDOUT_FILENO) < 0) {  // Redirect stdout to the given file
                print_error();
                exit(1);
            }
            close(output_fd);
        }

        // Iterate over the paths to try executing the command
        for (int i = 0; i < path_count; i++) {
            char cmd[MAX_CMD_LEN];
            snprintf(cmd, sizeof(cmd), "%s/%s", paths[i], args[0]);  // Construct the command with the current path
            execv(cmd, args);  // Attempt to execute the command
        }

        // If execution fails for all paths, print an error and exit
        print_error();
        exit(1);
    }
}



// Function to parse input and execute the appropriate command
void parse_and_execute(char *input, int is_parallel) {
    char *args[MAX_ARGS];  // Array to hold arguments
    int arg_count = 0;

    // Tokenize the input by spaces, tabs, or newlines
    for (char *token = strtok(input, " \t\n"); token != NULL; token = strtok(NULL, " \t\n")) {
        args[arg_count++] = token;
    }
    args[arg_count] = NULL;  // Null-terminate the argument list

    if (arg_count == 0) return;  // No command entered

    // Handle built-in commands: exit, cd, path
    if (strcmp(args[0], "exit") == 0) {
        // Exit command: if extra arguments are given, print error
        if (arg_count != 1) {
            print_error();
        } else {
            exit(0);  // Exit the shell
        }
    } else if (strcmp(args[0], "cd") == 0) {
        // cd command: change directory
        if (arg_count != 2 || chdir(args[1]) != 0) {
            print_error();  // Error if no directory is given or if chdir fails
        }
    } else if (strcmp(args[0], "path") == 0) {
        // path command: set the search paths
        path_count = 0;
        for (int i = 1; i < arg_count; i++) {
            paths[path_count++] = strdup(args[i]);  // Store each path
        }
        paths[path_count] = NULL;  // Null-terminate the paths array
    } else {
        int output_fd = STDOUT_FILENO;  // Default output is standard output
        
        // Handle output redirection
        for (int i = 0; i < arg_count; i++) {
            if (strcmp(args[i], ">") == 0) {
                // If output redirection is found
                if (i == arg_count - 2) {
                    output_fd = open(args[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);  // Open file for redirection
                    if (output_fd < 0) {
                        print_error();  // Error if file can't be opened
                        return;
                    }
                    args[i] = NULL;  // Terminate the arguments before the '>'
                    break;
                } else {
                    print_error();  // Error if redirection syntax is incorrect
                    return;
                }
            }
        }

        // Execute the command
        execute_command(args, output_fd);
        
        // Close the output file descriptor if redirection was used
        if (output_fd != STDOUT_FILENO) {
            close(output_fd);
        }
    }

    // Wait for child processes if not running in parallel mode
    if (!is_parallel) {
        while (wait(NULL) > 0);  // Wait for all children to terminate
    }
}



// Function to execute commands in parallel
void execute_parallel(char *input) {
    char *commands[MAX_ARGS];  // Array to hold commands
    int command_count = 0;

    // Tokenize the input string by '&'
    for (char *command = strtok_r(input, "&", &input); command != NULL; command = strtok_r(NULL, "&", &input)) {
        commands[command_count++] = trim(command);  // Trim and store each command
    }

    // Execute each command in parallel
    for (int i = 0; i < command_count; i++) {
        parse_and_execute(commands[i], 1);  // Execute each command with parallel flag
    }

    // Wait for all child processes to complete
    while (wait(NULL) > 0);  // Wait for all parallel processes to finish
}




int main(int argc, char *argv[]) {
    // Check if the shell was invoked with any arguments
    // If arguments are present, print an error and exit with status 1
    if (argc != 1) {
        print_error();
        exit(1);
    }

    // Main shell loop: runs indefinitely, repeatedly displaying prompt and processing commands
    while (1) {
        printf("rush> ");  // Display the shell prompt
        fflush(stdout);    // Ensure the prompt is displayed immediately

        char *line = NULL;  // Pointer for storing the input line
        size_t len = 0;     // Variable to store the size of the input buffer
        ssize_t read = getline(&line, &len, stdin);  // Read user input

        // If getline fails (e.g., EOF or error), free memory and exit the shell
        if (read == -1) {
            free(line);
            exit(0);
        }

        // Remove the trailing newline character from the input line
        line[strcspn(line, "\n")] = 0;
        char *trimmed_line = trim(line);  // Trim leading/trailing spaces and tabs from input

        // Check if the input contains the '&' symbol for parallel commands
        if (strchr(trimmed_line, '&') != NULL) {
            execute_parallel(trimmed_line);  // Execute parallel commands
        } else {
            parse_and_execute(trimmed_line, 0);  // Execute a single command
        }

        free(line);  // Free memory allocated for the input line
    }

    return 0;  // Return 0, although this line is never reached due to the infinite loop
}


