#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/wait.h>

#define N 100 

int main (){
    char cmd[N];
    char *argtok[N]; // L'array dei puntatori per i pezzi del comando

    while(1){ 
        printf("myshell> "); // Un prompt più carino
        
        if (fgets(cmd, N, stdin) == NULL){
            perror("Uscita forzata"); // Gestisce CTRL+D
            break;
        } 
        
        cmd[strcspn(cmd, "\n")] = 0; // Toglie \n

        // --- TOKENIZER (Il Macellaio) ---
        char *token = strtok(cmd, " ");
        int i = 0; 
        
        while (token != NULL){
            argtok[i] = token; 
            i++;
            token = strtok(NULL, " "); 
        }
        argtok[i] = NULL; // Chiudo l'array con NULL

        // [FIX 1] Controllo anti-crash: Se non hai scritto nulla, ricomincia
        if (argtok[0] == NULL) {
            continue; 
        }

        // --- FORK ---
        int pid = fork();
        
        if (pid < 0){
            perror("errore fork");
        }
        else if (pid == 0){
            // === FIGLIO ===
            printf("Son il figlio (PID:%d). Eseguo: %s\n", getpid(), argtok[0]);
            
            // [FIX 2] Ho tolto la riga inutile char *argv[]...
            // Passo direttamente argtok!
            execvp(argtok[0], argtok);
            
            // Se arrivo qui, exec ha fallito
            perror("Comando fallito"); // perror ti dice PERCHÉ ha fallito
            exit(1); 
        }
        else {
            // === PADRE ===
            wait(NULL);
        }
    }
    return 0;
}