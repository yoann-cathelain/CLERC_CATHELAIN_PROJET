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

#include "serveur.h"

void plot(char *data)
{

  // Extraire le compteur et les couleurs RGB
  FILE *p = popen("gnuplot -persist", "w");
  printf("Plot\n");
  int count = 0;
  int n;
  char *saveptr = NULL;
  char *str = data;
  fprintf(p, "set xrange [-15:15]\n");
  fprintf(p, "set yrange [-15:15]\n");
  fprintf(p, "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "set title 'Top 10 colors'\n");
  fprintf(p, "plot '-' with circles lc rgbcolor variable\n");
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
      // Le numéro 36, parceque 360° (cercle) / 10 couleurs = 36
      fprintf(p, "0 0 10 %d %d 0x%s\n", (count - 1) * 36, count * 36, token + 1);
    }
    count++;
  }
  fprintf(p, "e\n");
  printf("Plot: FIN\n");
  pclose(p);
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data)
{
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

/* renvoyer le nom du client contenu dans (*data)
*/
int renvoie_nom_client(int client_socket_fd, char *data)
{
  int data_size = write(client_socket_fd, (void *)data, strlen(data));
  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

/* renvoyer le resultat du calcul contenu dans (*data)
*/
int renvoie_res_calcul(int client_socket_fd, char *data)
{
  int data_size = write(client_socket_fd, (void *)data, strlen(data));
  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd)
{
  struct sockaddr_in client_addr;
  char data[1024];

  unsigned int client_addr_len = sizeof(client_addr);

  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_socket_fd < 0)
  {
    perror("accept");
    return (EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lecture de données envoyées par un client
  int data_size = read(client_socket_fd, (void *)data, sizeof(data));

  if (data_size < 0)
  {
    perror("erreur lecture");
    return (EXIT_FAILURE);
  }

  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Message recu: %s\n", data);
  char code[10];
  sscanf(data, "%s", code);

  // Si le message commence par le mot: 'message:'
  if (strcmp(code, "message:") == 0)
  {
    renvoie_message(client_socket_fd, data);
  }
  else
  {
    plot(data);
  }

  // fermer le socket
  close(socketfd);
  return (EXIT_SUCCESS);
}

int renvoi_res_calcul(int socketfd)
{
  struct sockaddr_in client_addr;
  char data[1024];

  unsigned int client_addr_len = sizeof(client_addr);

  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_socket_fd < 0)
  {
    perror("accept");
    return (EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lecture de données envoyées par un client
  int data_size = read(client_socket_fd, (void *)data, sizeof(data));

  if (data_size < 0)
  {
    perror("erreur lecture");
    return (EXIT_FAILURE);
  }

  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Calcul recu: %s\n", data);
  char code[10];
  char * datacopy = data;
  int operation;
  char * resultat;
  int firstoperand =0 ;
  int convfirstoperand = sscanf((strtok(datacopy, "calcul: ")), "%d", &firstoperand);
  char * operator = strtok(NULL, " ");
  int secondoperand = 0;
  int convsecondoperand = sscanf((strtok(NULL, " ")), "%d", &secondoperand);

  if(strcmp(operator, "+") == 0)
  {
    operation = firstoperand + secondoperand;
    sprintf(resultat, "%d", operation);
  }else if(strcmp(operator, "-") == 0) 
  {
    operation = firstoperand - secondoperand;
    sprintf(resultat, "%d", operation);
  }else if(strcmp(operator, "*") == 0)
  {
    operation = firstoperand * secondoperand;
    sprintf(resultat, "%d", operation);
  }else if(strcmp(operator, "/") == 0)
  {
    operation = firstoperand/secondoperand;
    sprintf(resultat, "%d", operation);
  }else 
  {
    perror("Erreur operateur ou mauvaise saisie");
    return(EXIT_FAILURE);
  }
  sscanf(data, "%s", code);

  // Si le message commence par le mot: 'calcul'
  if (strcmp(code, "calcul:") == 0)
  {
    renvoie_res_calcul(client_socket_fd, resultat);
  }
  else
  {
    plot(data);
  }

  // fermer le socket
  close(socketfd);
  return (EXIT_SUCCESS);
}

int renvoi_nom_client(int socketfd)
{
  struct sockaddr_in client_addr;
  char data[1024];

  unsigned int client_addr_len = sizeof(client_addr);

  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_socket_fd < 0)
  {
    perror("accept");
    return (EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lecture de données envoyées par un client
  int data_size = read(client_socket_fd, (void *)data, sizeof(data));

  if (data_size < 0)
  {
    perror("erreur lecture");
    return (EXIT_FAILURE);
  }

  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Nom recu: %s\n", data);
  char code[10];
  sscanf(data, "%s", code);

  // Si le message commence par le mot: 'nom:'
  if (strcmp(code, "nom:") == 0)
  {
    renvoie_nom_client(client_socket_fd, data);
  }
  else
  {
    plot(data);
  }

  // fermer le socket
  close(socketfd);
  return (EXIT_SUCCESS);
}

int main()
{

  int socketfd;
  int bind_status;

  struct sockaddr_in server_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0)
  {
    perror("bind");
    return (EXIT_FAILURE);
  }

  // Écouter les messages envoyés par le client
  listen(socketfd, 10);

  // Lire et répondre au client
  /*if(strcmp(code, "message") == 0)
  {
    recois_envoie_message(socketfd);
  }else if(strcmp(code, "nom") == 0)
  {
    renvoi_nom_client(socketfd);
  }else if(strcmp(code, "calcul") == 0)
  {
    renvoi_res_calcul(socketfd);
  }*/

  renvoi_res_calcul(socketfd);


  return 0;
}
