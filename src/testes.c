
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * L'objectif principal de ce code est d'effectuer des tests unitaires et
 *  fonctionnels sur les différents composants créés dans ce projet.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "json.h"

// Teste que les fonctionnalités du serveur sont supportées
void testOperations(char* code, char* data){
    if(strcmp(code, data) == 0){
        printf("Test de support des opérations par le %s: validé\n", code);
    }else{
        printf("Test de support des opératios par le %s: non validé\n", code);
    }
}

// Teste que le client et le serveur comprennent les messages JSON
void testJson(char* whoami, char* code, char* valeurs){
    if(strcmp(code, "testJSON") == 0 && strcmp(valeurs, "testJSON") == 0){
        printf("Test d'analyse du JSON par le %s: validé\n", whoami);
    }else{
        printf("Test d'analyse du JSON par le %s: non validé\n", whoami);
    }
}

/* Teste que le client et le serveur arrivent à communiquer en JSON en respectant le protocole mis en place:
*   le client initie la connexion en donnant son hostname et le serveur l'enregistre dans un fichier.
*/
void testProtocole(char* clientSide, char* serverSide, char* socketNo){

    // On vérifie que le serveur a bien enregistré le hostname du client
    char* sentHostname = malloc(sizeof(char) * 1024);
    char* hostname = malloc(sizeof(char) * 1024);
    FILE* file = fopen(socketNo, "r");

    // Récupération de l'hostname
    if (fgets(sentHostname, sizeof(sentHostname), file) == NULL) {
        perror("Error reading hostname");
        exit(1);
    }
    sentHostname[strcspn(sentHostname, "\n")] = '\0';
    
    gethostname(hostname, sizeof(hostname));

    // Test
    if(strcmp(clientSide, serverSide) == 0 && strcmp(hostname, sentHostname) == 0){
        printf("Test de communication en JSON: validé\n");
    }else{
        printf("Test de communication en JSON: non validé\n");
    }

    // Fermeture du fichier
    fclose(file);

    // Libération de la mémoire
    free(hostname);
    free(sentHostname);
}

// Teste que le serveur est capable de gérer plusieurs clients
void testMultiUsers(){

  int result = system("./client message; ./client message");
  if (result < 0) {
    perror("system");
    exit(1);
  }

  // Attente de la fin des processus enfants
  int status;
  waitpid(-1, &status, WNOHANG),

  // Envoi d'un SIGINT aux clients
  result = system("pkill -SIGINT client");
  if (result < 0) {
    perror("SIGINT");
    exit(1);
  }

  if(result >= 0){
        printf("Test de gestion de plusieurs clients: validé\n");
    }
    else{
    printf("Test de gestion de plusieurs clients: non validé\n");
    }

}
