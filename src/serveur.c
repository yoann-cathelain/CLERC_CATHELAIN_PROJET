/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Le code côté serveur. L'objectif principal est de recevoir des messages des clients,
 * de traiter ces messages et de répondre aux clients.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "serveur.h"
#include "json.h"

// Pour le threading
#include <arpa/inet.h>
#include <netdb.h>

// Pour les tests
#include "testes.h"
/*
 *
 * Créé un plot de couleur
 */
void plot(char *data){

  // Extraire le compteur et les couleurs RGB
  FILE *p = popen("gnuplot -persist", "w");
  printf("Plot\n");
  int count = 0;
  json_object json_image = json_decode(data);
  int n;
  int intNbCoul;
  char *saveptr = NULL;
  char *str = json_image.valeurs;
  
  // Récupération du nombre de couleurs
  char *copy = malloc(sizeof(char)*1024);
  strcpy(copy, str);
  char* nbCoul = strtok(copy, "image: ");
  nbCoul = strtok(nbCoul, ",");
  
  printf("%s\n", data);
  sscanf(nbCoul, "%d", &intNbCoul);

  // Construction de la chaîne de titre
  char titre[1024] = "set title 'Top ";
  strcat(titre, nbCoul);
  strcat(titre, " colors'\n");

  // Construction de l'échelle
  int scaleInt=1.5*intNbCoul;
  char* scale = malloc(sizeof(char*)*2);
  sprintf(scale, "%d", scaleInt);
  char xrange[1024] = "set xrange [-";
  strcat(xrange, scale);
  strcat(xrange, ":");
  strcat(xrange, scale);
  strcat(xrange, "]\n");

  char yrange[1024] = "set yrange [-";
  strcat(yrange, scale);
  strcat(yrange, ":");
  strcat(yrange, scale);
  strcat(yrange, "]\n");


  // Présentation
  fprintf(p, "%s", xrange);
  fprintf(p, "%s", yrange);
  fprintf(p, "%s", "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "%s", titre);
  fprintf(p, "%s", "plot '-' with circles lc rgbcolor variable\n");
  while (1)
  {
    char *token = strtok_r(str, ",", &saveptr);
    if (token == NULL)
    {
      break;
    }
    str = NULL;
    printf("%d: %s\n", count, token);
    if (count == 1)
    {
      n = atoi(token);
      printf("n = %d\n", n);
    }
    else
    {
      // On prend numero tel que 360° (cercle) / nbCoul = 36
      int numero = 360 / intNbCoul;
      fprintf(p, "0 0 %d %d %d 0x%s\n", intNbCoul, (count-1) * numero, count * numero, token + 1);
    }
    count++;
  }

  // Fin du plot
  fprintf(p, "e\n");
  printf("Plot: FIN\n");
  pclose(p);

  // Libération de la mémoire
  free(copy);
  free(scale);
}



/* Renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data){

  // Envoi
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  // Erreurs
  if (data_size < 0){
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}



/* Renvoyer le nom du client contenu dans (*data)
*/
int renvoie_nom_client(int client_socket_fd, char *data)
{

  // Envoi
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  // Erreurs
  if (data_size < 0){
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}



/* Renvoyer le resultat du calcul contenu dans (*data)
*/
int renvoi_res_calcul(int client_socket_fd, char *data)
{
  // Init
  float result = 0;
  float operand = 0;
  json_object json_resultat;
  json_resultat.code = malloc(sizeof(char)*1024);
  json_resultat.valeurs = malloc(sizeof(char)*1024);

  // Récupération des données JSON
  json_object json_data = json_decode(data);
  char* valeurs = json_data.valeurs;
  
  // Récupération du code d'opération
  char* token = strtok(valeurs, ",");

  // Gestion du code d'opération
   // Addition
  if(strcmp(token, "+") == 0){

    token = strtok(NULL, ",");
    
    while(token != NULL) 
    {
	    sscanf(token,"%f", &operand);
	    result += operand;
	    token = strtok(NULL, ",");
    }
  }
  // Soustraction
  else if(strcmp(token, "-") == 0){

  token = strtok(NULL, ",");
  sscanf(token,"%f", &result);

  result -= operand;

  while(token != NULL){
	    token = strtok(NULL, ",");

      // Sécurité nombre d'opérandes impaire
      if(token != NULL){
        sscanf(token,"%f", &operand);
        result -= operand;
      }
      
  }
  }
  // Multiplication
  else if(strcmp(token, "*") == 0){
    result = 1;
    token = strtok(NULL, ",");
    
    while(token != NULL) 
    {
	    sscanf(token,"%f", &operand);
	    result *= operand;
	    token = strtok(NULL, ",");
    }
  }
  // Division
  else if(strcmp(token, "/") == 0){

    token = strtok(NULL, ",");
    sscanf(token,"%f", &result);

    // Sécurité division par 0
    operand = 1;

    result /= operand;

    while(token != NULL){
        token = strtok(NULL, ",");

        // Sécurité nombre d'opérandes impaire
        if(token != NULL){
          sscanf(token,"%f", &operand);
          result /= operand;
        }
    }
  }
    // Minimum
  else if(strcmp(token, "min") == 0){

    token = strtok(NULL, ",");
    sscanf(token,"%f", &result);

    while(token != NULL){
        token = strtok(NULL, ",");
        // Sécurité nombre d'opérandes impaire
        if(token != NULL){
          sscanf(token,"%f", &operand);

          if(operand < result){
            result = operand;
          }
        }
    }
  }
  // Maximum
  else if(strcmp(token, "max") == 0){

    token = strtok(NULL, ",");
    sscanf(token,"%f", &result);

    while(token != NULL){
        token = strtok(NULL, ",");
        // Sécurité nombre d'opérandes impaire
        if(token != NULL){
          sscanf(token,"%f", &operand);

          if(operand > result){
            result = operand;
          }
        }
    }
  }
  // Moyenne
  else if(strcmp(token, "avg") == 0){
    
    token = strtok(NULL, ",");
    int i = 0;

    while(token != NULL) 
    {
	    sscanf(token,"%f", &operand);
	    result += operand;
	    token = strtok(NULL, ",");
      i++;
    }    

    result /= i;
  }
  // Ecart-type: sqrt(e(x^2)-e(x)^2)
  else if(strcmp(token, "ecart") == 0){

    token = strtok(NULL, ",");
    int i = 0;
    float squaredOperand = 0;
    float squaredResult = 0;

    while(token != NULL) 
    {
	    sscanf(token,"%f", &operand);

      // Construction de e(x)
	    result += operand;

      // Construction de e(x^2)
      squaredOperand = operand * operand;
      squaredResult += squaredOperand;

	    token = strtok(NULL, ",");
      i++;
    }    

    squaredResult /= i;
    result /= i;

    result = sqrt(squaredResult - (result * result));
    
  }
  // Erreur de saisie
  else {
    perror("Erreur operateur ou mauvaise saisie");
    return(EXIT_FAILURE);
  }

  // Renvoi des données
  char* resultat = malloc(sizeof(char)*1024);
  sprintf(resultat, "%f", result);

  json_resultat.code = "Resultat";
  json_resultat.valeurs = resultat;
  int data_size = write(client_socket_fd, json_encode(&json_resultat, '\x32'), 1024);
  
  if (data_size < 0){
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  
  // Libération de la mémoire
  free(resultat);

  return (EXIT_SUCCESS);
}



/* Accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd, char* data){

  // Init
  json_object json_message;
  int client_socket_fd = socketfd;

  /*
   * Extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Message recu: %s\n", data);
  json_message = json_decode(data);
  
  // Si le message commence par le mot: 'message' ou un test de protocole
  if (strcmp(json_message.code, "message") == 0){
	renvoie_message(client_socket_fd, data);
  }
  // Si le message commence par le mot: 'calcul:'
  else if(strcmp(json_message.code, "calcul") == 0){
	renvoi_res_calcul(client_socket_fd, data);

  }
  // Si le message commence par le mot: 'nom:'
  else if(strcmp (json_message.code, "nom") == 0){
	renvoie_nom_client(client_socket_fd, data);

  }
  // Si le message commence par le mot: 'couleurs:'
  else if(strcmp(json_message.code, "couleurs") == 0){
 
	// Init
	json_object json_res;
	json_res.code = malloc(sizeof(char)*1024);
	json_res.valeurs = malloc(sizeof(char)*1024);
  	char *filename = "couleurs.txt";
  	FILE *fp = fopen(filename, "w");

	// Erreur d'ouverture
  	if (fp == NULL){
     		perror("Error opening your file");
     		return(EXIT_FAILURE);
  	}

	// Ecriture du fichier
	json_res.code = json_message.code;
	json_res.valeurs = "enregistre";
  	fprintf(fp, "%s" ,json_message.valeurs);
  	fclose(fp);
  
	// Renvoi du message
	renvoie_message(client_socket_fd, json_encode(&json_res, '\x32'));
  }
  // Si le message commence par le mot: 'balises:'
  else if (strcmp(json_message.code, "balises") == 0){
    
	  // Init
	  json_object json_res;
	  json_res.code = malloc(sizeof(char)*1024);
	  json_res.valeurs = malloc(sizeof(char)*1024);
  	char *filename = "balises.txt";
  	FILE *fp = fopen(filename, "w");

	  // Erreur d'ouverture
  	if (fp == NULL){
     		perror("Error opening your file");
     		return(EXIT_FAILURE);
  	}

	  // Ecriture du fichier
	  json_res.code = json_message.code;
	  json_res.valeurs = "enregistre";
  	fprintf(fp, "%s" ,json_message.valeurs);
  	fclose(fp);

	  // Renvoi du message
  	renvoie_message(client_socket_fd, json_encode(&json_res, '\x32'));
  } 
  // Si le messsage est un test de JSON
  else if(strcmp(json_message.code, "testJSON") == 0){
      // Init
    json_object json_test;

    // Décodage du JSON
    json_test = json_decode(data);

    // Test de la compréhension du JSON côté client
    testJson("serveur", json_test.code, json_test.valeurs);
  }
  // Si le message est un test de protocole
  else if(strcmp(json_message.code, "testProt") == 0){

    // Init
    char* socketNo = malloc(sizeof(char)*2);
    sprintf(socketNo, "%d", client_socket_fd);

    // Renvoi du message
    write(client_socket_fd, data, 1024);

    // Renvoi du numéro de socket
    send(client_socket_fd, socketNo, 2, 0);
  }
  else{
    plot(data);
  }

  return (EXIT_SUCCESS);
}


 /* Fonction principale => créé le socket, écoute les messages et renvoie les réponses adéquates 
 */
int main(){

  // Init
  int option=1, 
  socketfd, 
  addrlen, 
  new_socket, 
  client_socket[30], 
  max_clients = 30, 
  activity, 
  valread, 
  sd,
  bind_status,
  max_sd;
  struct sockaddr_in server_addr;
  char* buffer = malloc(sizeof(char)*1024);
  char* hostname = malloc(sizeof(char)*256);
  fd_set readfds;
  FILE* file;
  char* filename = malloc(sizeof(char)*16);

  // Initialiser le tableau de sockets à 0 pour dire que le socket n'est pas utilisé
  for (int i = 0; i < max_clients; i++){
    client_socket[i] = 0;
  }

  // Création du socket principal
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0){
    perror("Unable to open a socket");
    return (EXIT_FAILURE);
  }

  // Permet de réutiliser le port
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  
  // Détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse au socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0){
    perror("bind");
    return (EXIT_FAILURE);
  } 

  // Ecoute des messages
  listen(socketfd, 10);

  // Accepter les connexions
  addrlen = sizeof(server_addr);
  puts("En attente de connexions...");

  while(1){

    // Initialiser le set de sockets
    FD_ZERO(&readfds);

    // Ajouter le socket principal au set
    FD_SET(socketfd, &readfds);
    max_sd = socketfd;

    // Ajouter les sockets des clients au set
    for (int i=0; i<max_clients; i++){
      
      // Socket descriptor
      sd = client_socket[i];

      // Si le socket est valide, on l'ajoute au set
      if(sd > 0){
        FD_SET(sd, &readfds);
      }

      // On garde le plus grand socket descriptor
      if(sd > max_sd){
        max_sd = sd;
      }
    }

    // On attend une activité sur un des sockets
    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

    if ((activity < 0)){
      perror("select");
      return (EXIT_FAILURE);
    }

    // Si une activité est détectée sur le socket principal, c'est une nouvelle connexion
    if (FD_ISSET(socketfd, &readfds)){

      new_socket = accept(socketfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen);

      if (new_socket < 0){
        perror("accept");
        return (EXIT_FAILURE);
      }

      // On reçoit le nom du client
      read(new_socket, hostname, 256);


      // On affiche le nom du client et on l'enregistre dans un fichier au nom du numéro de socket
      printf("Nouvelle connexion de %s\n", hostname);
      sprintf(filename, "%d", new_socket);
      file = fopen(filename, "w");
      // Erreur d'ouverture
      if (file == NULL) {
        perror("Error opening file");
        exit(1);
      }
      // Écriture de l'hostname dans le fichier
      fprintf(file, "%s\n", hostname);
      // Fermeture du fichier
      fclose(file);

      // On ajoute le nouveau socket à la liste des sockets
      for(int i=0; i<max_clients; i++){
        if(client_socket[i] == 0){

          client_socket[i] = new_socket;
          break;
        }
      }
    }
    
    // Sinon, c'est une activité sur un socket client
    for (int i = 0; i<max_clients; i++){

      sd = client_socket[i];

      if(FD_ISSET(sd, &readfds)){

        // Vérifie si le client a fermé la connexion
        valread = read(sd, buffer, 1024);
        if(valread == 0){

          // On récupère le nom du client
          sprintf(filename, "%d", sd);
          file = fopen(filename, "r");
          if (file == NULL) {
            perror("Error opening file");
            exit(1);
          }

          // Lecture de l'hostname depuis le fichier
          if (fgets(hostname, sizeof(hostname), file) == NULL) {
            perror("Error reading hostname");
            exit(1);
          }

          fclose(file);

          getpeername(sd, (struct sockaddr*)&server_addr, (socklen_t*)&addrlen);
          printf("Déconnexion de %s\n", hostname);

          // Supprimer le socket du tableau et marquer l'index comme libre
          close(sd);
          client_socket[i] = 0;
        }
        // Sinon c'est un messsage du client
        else{
          recois_envoie_message(sd, buffer);
          memset(buffer, 0, 1024);
        }
      }
    }
  }

  // On libère la mémoire
  free(buffer);
  free(hostname);
  free(filename);

  return 0;
}
