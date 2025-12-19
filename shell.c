#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 100

// Funzione per eseguire un comando senza pipe
void execute_command(char *cmd) {
    char *args[MAX_ARGUMENTS];
    char *token;
    int i = 0;

    // Tokenizzare il comando
    token = strtok(cmd, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Creazione del processo figlio per eseguire il comando
    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);  // Esegui il comando
        perror("execvp failed");
        exit(1);  // Se exec fallisce, esci con errore
    } else if (pid > 0) {
        wait(NULL);  // Aspetta la terminazione del processo figlio
    } else {
        perror("fork failed");
    }
}

// Funzione per gestire il comando con pipe
void execute_with_pipe(char *cmd1, char *cmd2) {
    int pipefd[2];
    pid_t pid1, pid2;

    // Creazione del pipe
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    // Primo processo (cmd1)
    pid1 = fork();
    if (pid1 == 0) {
        // Chiudiamo il lato di lettura del pipe
        close(pipefd[0]);
        // Rerouting dell'output standard (stdout) al lato di scrittura del pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execute_command(cmd1);  // Esegui il primo comando
    } else if (pid1 > 0) {
        // Secondo processo (cmd2)
        pid2 = fork();
        if (pid2 == 0) {
            // Chiudiamo il lato di scrittura del pipe
            close(pipefd[1]);
            // Rerouting dell'input standard (stdin) al lato di lettura del pipe
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            execute_command(cmd2);  // Esegui il secondo comando
        } else if (pid2 > 0) {
            // Aspettiamo entrambi i processi figli
            close(pipefd[0]);
            close(pipefd[1]);
            wait(NULL);
            wait(NULL);
        } else {
            perror("fork failed");
        }
    } else {
        perror("fork failed");
    }
}

int main() {
    char input[MAX_COMMAND_LENGTH];
    char *cmd1, *cmd2;

    while (1) {
        // Stampa il prompt
        printf("shell> ");
        fflush(stdout);

        // Leggi l'input
        if (fgets(input, MAX_COMMAND_LENGTH, stdin) == NULL) {
            break;
        }

        // Rimuovi il carattere di newline
        input[strcspn(input, "\n")] = 0;

        // Se la linea contiene una pipe, separa i comandi
        if (strchr(input, '|') != NULL) {
            cmd1 = strtok(input, "|");
            cmd2 = strtok(NULL, "|");

            // Esegui i comandi con pipe
            execute_with_pipe(cmd1, cmd2);
        } else {
            // Esegui un singolo comando
            execute_command(input);
        }
    }

    return 0;
}
