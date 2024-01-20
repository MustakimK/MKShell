/* Standard header files for system calls, file operations, and basic C functionalities */
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

/* Function declarations for shell commands */
int mkshell_cd(char **args);         // Change directory command
int mkshell_help(char **args);       // Display help information
int mkshell_ls();                    // List directory contents
int mkshell_history(char **args);    // Show command history
int mkshell_exit(char **args);       // Exit the shell
int mkshell_cat(char **args);        // Concatenate and display files
int mkshell_mkdir(char **args);      // Make a directory
int mkshell_rmdir(char **args);      // Remove a directory
int mkshell_touch(char **args);      // Change file timestamps
int mkshell_rm(char **args);         // Remove files or directories
int mkshell_clear_history(char **args); // Clear command history
int mkshell_cp(char **args);         // Copy files and directories
int mkshell_path(char **args);       // Set or get the system path

/* Global definitions and variables */
#define MKSHELL_HISTSIZE 10          // Define the size of the history array
char *history[MKSHELL_HISTSIZE];     // Array to store history of commands
int history_count = 0;           // Counter for the number of commands in history

/* Additional function declarations */
int mkshell_num_builtins();          // Get the number of built-in functions
int mkshell_launch(char **args);     // Launch a program
int mkshell_execute(char **args);    // Execute a shell command
char *mkshell_read_line(void);       // Read a line of input from the user
char **mkshell_split_line(char *line); // Split a line into tokens
void mkshell_loop(void);             // Start the main loop of the shell
void sigint_handler(int sig);    // Signal handler for interrupt signal


/* Built-in command names and corresponding function pointers. */
char *builtin_str[] = {
    "cd", "help", "exit", "ls", "history", 
    "cat", "mkdir", "rmdir", "touch", "rm", 
    "clear_history", "cp", "path"
};

int (*builtin_func[]) (char **) = {
    &mkshell_cd, &mkshell_help, &mkshell_exit, &mkshell_ls, &mkshell_history, 
    &mkshell_cat, &mkshell_mkdir, &mkshell_rmdir, &mkshell_touch, &mkshell_rm, 
    &mkshell_clear_history, &mkshell_cp, &mkshell_path
};

/* Returns count of built-in commands. */
int mkshell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/* 'cd' command: change shell's current directory. */
int mkshell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkshell: expected argument to \"cd\"\n");  // Error if no argument
    } else {
        if (chdir(args[1]) != 0) {
            perror("mkshell");  // Print error if directory change fails
        }
    }
    return 1;
}

/* 'ls' command: list contents of current directory. */
int mkshell_ls() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");  // Open current directory
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);  // Print each directory entry
        }
        closedir(d);
    }
    return 1;
}

/* 'history' command: display command history. */
int mkshell_history(char **args) {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i, history[i]);  // Print history item
    }
    return 1;
}

/* 'clear_history' command: clear the command history. */
int mkshell_clear_history(char **args) {
    for (int i = 0; i < history_count; i++) {
        free(history[i]);  // Free each history string
    }
    history_count = 0;
    return 1;
}

/* 'cat' command: concatenate and display file contents. */
int mkshell_cat(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkshell: expected argument to \"cat\"\n");  // Check for argument
    } else {
        FILE *file = fopen(args[1], "r");  // Open the file for reading
        char line[1024];  // Buffer to store file lines
        if (file == NULL) {
            perror("mkshell");  // Handle file opening errors
            return 1;
        }
        while (fgets(line, sizeof(line), file) != NULL) {
            printf("%s", line);  // Print each line of the file
        }
        fclose(file);  // Close the file
    }
    return 1;
}

/* 'mkdir' command: create a new directory. */
int mkshell_mkdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkshell: expected argument to \"mkdir\"\n");  // Check for argument
    } else {
        if (mkdir(args[1], 0777) != 0) {
            perror("mkshell");  // Handle errors in directory creation
        }
    }
    return 1;
}

/* 'rmdir' command: remove a directory. */
int mkshell_rmdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkshell: expected argument to \"rmdir\"\n");  // Check for argument
    } else {
        if (rmdir(args[1]) != 0) {
            perror("mkshell");  // Handle errors in removing directory
        }
    }
    return 1;
}

/* 'touch' command: update the access and modification times of a file. */
int mkshell_touch(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkshell: expected argument to \"touch\"\n");  // Check for argument
    } else {
        FILE *file = fopen(args[1], "ab+");  // Open or create file for updating
        if (file == NULL) {
            perror("mkshell");  // Handle file opening errors
        } else {
            fclose(file);  // Close the file
        }
    }
    return 1;
}

/* 'rm' command: remove a file or directory. */
int mkshell_rm(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkshell: expected argument to \"rm\"\n");  // Check for argument
    } else {
        if (remove(args[1]) != 0) {
            perror("mkshell");  // Handle errors in removing file/directory
        }
    }
    return 1;
}

/* 'cp' command: copy a file from source to destination. */
int mkshell_cp(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "mkshell: expected arguments to \"cp\"\n");  // Check for arguments
    } else {
        FILE *src = fopen(args[1], "rb");  // Open source file for reading in binary mode
        if (src == NULL) {
            perror("mkshell");
            return 1;
        }
        FILE *dest = fopen(args[2], "wb");  // Open destination file for writing in binary mode
        if (dest == NULL) {
            perror("mkshell");
            fclose(src);
            return 1;
        }

        char buffer[1024];  // Buffer for file content
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
            fwrite(buffer, 1, bytesRead, dest);  // Write to destination file
        }

        fclose(src);
        fclose(dest);
    }
    return 1;
}

/* 'path' command: display the current working directory. */
int mkshell_path(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);  // Print current working directory
    } else {
        perror("mkshell");  // Handle errors in getting current directory
    }
    return 1;
}

/* 'help' command: display information about built-in commands. */
int mkshell_help(char **args) {
    printf("Welcome to Mustakim Kazi's MKSHELL\n");
    printf("Enter program names and their arguments, then press enter.\n");
    printf("Available built-in commands:\n");

    for (int i = 0; i < mkshell_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);  // Display each built-in command
    }

    printf("For more information on external programs, use the 'man' command.\n");
    return 1;
}


/* 'exit' command: terminate the shell. */
int mkshell_exit(char **args) {
    return 0;  // Return 0 to signal shell to terminate
}

/* Launches a program and waits for it to terminate. */
int mkshell_launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();  // Create a new process
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("mkshell");  // If execution fails
        }
        exit(EXIT_FAILURE);  // Exit child process on failure
    } else if (pid < 0) {
        // Error forking
        perror("mkshell");
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);  // Wait for the child process to exit
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

/* Execute shell built-in or launch program. */
int mkshell_execute(char **args) {
    if (args[0] == NULL) {
        // An empty command was entered
        return 1;
    }

    // Check if the command is a built-in function
    for (int i = 0; i < mkshell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);  // Execute the built-in function
        }
    }

    return mkshell_launch(args);  // Launch the program if not a built-in function
}

/* Read a line of input from stdin. */
char *mkshell_read_line(void) {
#ifdef MKSHELL_USE_STD_GETLINE
    char *line = NULL;
    ssize_t bufsize = 0;
    // Use getline if available
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);  // Exit on EOF
        } else  {
            perror("mkshell: getline\n");
            exit(EXIT_FAILURE);
        }
    }
    return line;
#else
    // Manual implementation if getline is not available
#define MKSHELL_RL_BUFSIZE 1024
    int bufsize = MKSHELL_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "mkshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();  // Read a character

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;  // Return the buffer if end of line or file
        } else {
            buffer[position] = c;  // Store the character
        }
        position++;

        // Increase the buffer size if needed
        if (position >= bufsize) {
            bufsize += MKSHELL_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "mkshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
#endif
}

// Define the initial buffer size for tokenizing the input line
#define MKSHELL_TOK_BUFSIZE 64

// Define the delimiters used to tokenize the input line.
#define MKSHELL_TOK_DELIM " \t\r\n\a"

/* Split a line into tokens using defined delimiters. */
char **mkshell_split_line(char *line) {
    int bufsize = MKSHELL_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;

    if (!tokens) {
        fprintf(stderr, "mkshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, MKSHELL_TOK_DELIM);  // Tokenize the line
    while (token != NULL) {
        tokens[position] = token;
        position++;

        // Resize tokens array if necessary
        if (position >= bufsize) {
            bufsize += MKSHELL_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "mkshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, MKSHELL_TOK_DELIM);  // Continue tokenizing
    }
    tokens[position] = NULL;
    return tokens;
}

/* Signal handler for SIGINT (Ctrl+C). */
void sigint_handler(int sig) {
    char *message = "\nShell has been closed.\n";
    write(STDOUT_FILENO, message, strlen(message));
    exit(EXIT_SUCCESS);
}

/* Main loop of the shell. */
void mkshell_loop(void) {
    char *line;
    char **args;
    int status;
    char cwd[1024];
    char shell_prompt[100];

    signal(SIGINT, sigint_handler);  // Set up the signal handler

    do {
        // Create shell prompt with current user and directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            snprintf(shell_prompt, sizeof(shell_prompt), "%s@%s > ", getenv("USER"), cwd);
        } else {
            perror("getcwd() error");
            return;
        }

        printf("%s", shell_prompt);  // Print the prompt
        line = mkshell_read_line();      // Read input from user
        args = mkshell_split_line(line); // Split input into arguments

        // Handle command history
        if (history_count < MKSHELL_HISTSIZE) {
            history[history_count++] = strdup(line);
        } else {
            free(history[0]);
            memmove(history, history + 1, (MKSHELL_HISTSIZE - 1) * sizeof(char*));
            history[MKSHELL_HISTSIZE - 1] = strdup(line);
        }
        
        status = mkshell_execute(args);  // Execute the command

        free(line);  // Free memory
        free(args);
    } while (status);
}

/* Main entry point of the shell. */
int main(int argc, char **argv) {
    mkshell_loop();  // Start the shell loop

    return EXIT_SUCCESS;
}