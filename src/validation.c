
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


/*
 * Fonction qui supprime les espaces d'une chaine de caractère
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "json.h"


/*
 *  Fonction de validation de format JSON
 *  Renvoi 0 si la chaîne de caractère JSON n'est pas au bon format ou 1 si le format est correct
 */
int validate_json (char *json_string)
{
  int validate;
  int validate_array;
  int validate_res = 0;
  unsigned int i = 2;
  unsigned int j = 0;
  int index = 0;
  if (json_string[0] == '{')
    {
      if (json_string[1] == '"')
	{
	  validate = 1;
	  while (json_string[i] != '[' && validate == 1)
	    {
	      if (json_string[i] == '"' && json_string[i + 1] == ':')
		{
		  validate = 1;
		  validate_res = 1;
		}
	      else if (json_string[i] == '"'
		       && (json_string[i + 1] >= 'a'
			   && json_string[i + 1] <= 'z'))
		{
		  validate = 1;
		  validate_res = 1;
		}
	      else if (json_string[i] == '"'
		       && (json_string[i - 1] >= 'a'
			   && json_string[i - 1] <= 'z')
		       && json_string[i + 1] == ',')
		{
		  validate = 1;
		  validate_res = 1;
		}
	      else if (json_string[i] == ':'
		       && json_string[i + 1] == '[')
		{
		  validate = 1;
		  validate_res = 1;
		}
	      else
		{
		  validate_res = 0;
		  i++;
		  continue;
		}
	      i++;
	    }
	}
      if (validate_res == 1)
	{
	  if (json_string[i] == '[')
	    {
	      char *values = strchr (json_string, '[');
	      if (values)
		{
		  index = values - json_string;
		  j = index;
		}
	    }
	  else
	    {
	      validate_array = 0;
	      validate_res = 0;
	    }
	  while (j < strlen (json_string) && validate_array == 1)
	    {

	      if (json_string[j] == '"'
		  && ((json_string[j + 1] >= 'a' && json_string[j + 1] <= 'z') 
		  || (json_string[j+1] >= '0' && json_string[j+1] <= '9') || json_string[j + 1] == '#'))
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else if (json_string[j] >= '0' && json_string[j] <= '9'
		       && json_string[j + 1] == ',')
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else if (json_string[j] == '"'
		       && (json_string[j - 1] >= 'a'
			   && json_string[j - 1] <= 'z')
		       && json_string[j + 1] == ',')
		{
		  validate_array = 1;
		  validate_res = 1;
		}else if (json_string[j] == '"'
		       && (json_string[j - 1] >= '0'
			   && json_string[j - 1] <= '9')
		       && json_string[j + 1] == ',')
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else if (json_string[j] == '"' && json_string[j - 1] == ','
		       && (json_string[j + 1] >= 'a'
			   && json_string[j + 1] <= 'z'))
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else if (json_string[j] == '"' && json_string[j - 1] == ','
		       && (json_string[j + 1] >= '0'
			   && json_string[j + 1] <= '9'))
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else if (json_string[j] == '"'
		       && (json_string[j - 1] >= 'a'
			   && json_string[j - 1] <= 'z')
		       && json_string[j + 1] == ']')
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else if (json_string[j] == '"'
		       && (json_string[j - 1] >= '0'
			   && json_string[j - 1] <= '9')
		       && json_string[j + 1] == ']')
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else if (json_string[j] == ']')
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else if (json_string[j] == '}')
		{
		  validate_array = 1;
		  validate_res = 1;
		}
	      else
		{
		  validate_array = 0;
		  validate_res = 0;
		}
	      j++;
	    }

	}
      else
	{
	  validate_res = 0;
	}
    }
  else
    {
      validate_res = 0;
    }
  return validate_res;
}

int validate_json_nom(char* json_string){
    json_object json_nom;
    json_nom.code = malloc(sizeof(char)*1024);
    json_nom.valeurs = malloc(sizeof(char)*1024);

    json_nom = json_decode(json_string);
    char hostname[1024];
    gethostname(hostname, 1023);
    int validate_res;
    if(strcmp(json_nom.code, "nom") == 0){
        if(strcmp(json_nom.valeurs, hostname) == 0){
            validate_res = 1;
        }else {
            validate_res = 0;
        }
    }else {
        validate_res = 0;
    }

    return validate_res;
}

int validate_json_calcul(char* json_string){
    json_object json_calcul;
    json_calcul.code = malloc(sizeof(char)*1024);
    json_calcul.valeurs = malloc(sizeof(char)*1024);

    json_calcul = json_decode(json_string);
	unsigned int i = 0;
    int validate_res;
    if(strcmp(json_calcul.code, "calcul") == 0){
		while(i < strlen(json_calcul.valeurs)){
        	if(json_calcul.valeurs[i] == '+' || json_calcul.valeurs[i] == '-' || json_calcul.valeurs[i] == '*' || json_calcul.valeurs[i] == '/'){
				validate_res = 1;
			}else if(json_calcul.valeurs[i] >= '0' && json_calcul.valeurs[i] <= '9'){
				validate_res = 1;
			}else if(json_calcul.valeurs[i] == ','){
				i++;
				continue;
			}else {
				break;
			}
			i++;
		}

    }else {
        validate_res = 0;
    }
    return validate_res;
}

int validate_json_message(char* json_string){
    json_object json_message;
    json_message.code = malloc(sizeof(char)*1024);
    json_message.valeurs = malloc(sizeof(char)*1024);

    json_message = json_decode(json_string);
	unsigned int i = 0;
    int validate_res;
    if(strcmp(json_message.code, "message") == 0){
		while(i < strlen(json_message.valeurs)){
			json_message.valeurs[i] = tolower(json_message.valeurs[i]);
        	if(json_message.valeurs[i] >= 'a' && json_message.valeurs[i] <= 'z'){
				validate_res = 1;
			}else if(json_message.valeurs[i] == ','){
				i++;
				continue;
			}else {
				break;
			}
			i++;
		}

    }else {
        validate_res = 0;
    }

    return validate_res;
}

int validate_json_couleurs(char* json_string){
    json_object json_couleurs;
    json_couleurs.code = malloc(sizeof(char)*1024);
    json_couleurs.valeurs = malloc(sizeof(char)*1024);

    json_couleurs = json_decode(json_string);
	unsigned int i = 0;
    int validate_res;
    if(strcmp(json_couleurs.code, "couleurs") == 0){
		while(i < strlen(json_couleurs.valeurs)){
			json_couleurs.valeurs[i] = tolower(json_couleurs.valeurs[i]);
        	if((json_couleurs.valeurs[i] >= 'a' && json_couleurs.valeurs[i] <= 'z') || json_couleurs.valeurs[i] == '#' || (json_couleurs.valeurs[i] >= '0' && json_couleurs.valeurs[i] <= '9')){
				validate_res = 1;
			}else if(json_couleurs.valeurs[i] == ','){
				i++;
				continue;
			}else {
				break;
			}
			i++;
		}

    }else {
        validate_res = 0;
    }

    return validate_res;
}

int validate_json_balises(char* json_string){
    json_object json_balises;
    json_balises.code = malloc(sizeof(char)*1024);
    json_balises.valeurs = malloc(sizeof(char)*1024);

    json_balises = json_decode(json_string);
	unsigned int i = 0;
    int validate_res;
    if(strcmp(json_balises.code, "balises") == 0){
		while(i < strlen(json_balises.valeurs)){
			json_balises.valeurs[i] = tolower(json_balises.valeurs[i]);
        	if((json_balises.valeurs[i] >= 'a' && json_balises.valeurs[i] <= 'z') || json_balises.valeurs[i] == '#' || (json_balises.valeurs[i] >= '0' && json_balises.valeurs[i] <= '9')){
				validate_res = 1;
			}else if(json_balises.valeurs[i] == ','){
				i++;
				continue;
			}else {
				break;
			}
			i++;
		}

    }else {
        validate_res = 0;
    }

    return validate_res;
}



/* 
 * Le but principal de ce code est de s'assurer que les messages envoyés par le client sont
 * valides, c'est-à-dire que les messages respectent le format JSON et
 * respectent le protocole spécifié par le côté serveur.
 */
