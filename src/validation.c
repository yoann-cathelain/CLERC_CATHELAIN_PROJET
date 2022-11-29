
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


void remove_space(char *data)
{
    char *output = data;
    do
    {
        while (*output == ' ')
        {
            ++output;
        }
    } while ((*data++ = *output++));
}
/*
 *  Fonction de validation de format JSON
 *  Renvoi 0 si la chaîne de caractère JSON n'est pas au bon format ou 1 si le format est correct
 */
 int validate_json(char* json_string){
    remove_space(json_string);
    int validate_res = 0;
    int index = 0;
    if(json_string[0] == '{'){
        if(json_string[1] == '"'){
            for(unsigned int i = 2; i < strlen(json_string); i++){
                //Traite toute la partie avant le tableau de valeurs
                validate_res = json_string[i] == '"' && (json_string[i-1] >= 'a' && json_string[i-1] <= 'z') && json_string[i+1] == ':' ? 1 : 0;
                validate_res = json_string[i] == '"' && (json_string[i+1] >= 'a' && json_string[i+1] <= 'z') ? 1 : 0;
                validate_res = json_string[i] == '"' && (json_string[i-1] >= 'a' && json_string[i-1] <= 'z') && json_string[i+1] == ',' ? 1 : 0;
                validate_res = json_string[i] == '"' && json_string[i-1] == ',' && (json_string[i+1] >= 'a' && json_string[i+1] <= 'z') ? 1 : 0;
                validate_res = json_string[i] == '"' && (json_string[i-1] >= 'a' && json_string[i-1] <= 'z') && json_string[i+1] == ':' ? 1 : 0;
                validate_res = json_string[i] == '"' && (json_string[i+1] >= 'a' && json_string[i+1] <= 'z') ? 1 : 0;
                validate_res = json_string[i] == '"' && (json_string[i-1] >= 'a' && json_string[i-1] <= 'z') && json_string[i+1] == '[' ? 1 : 0;

                char* values = strchr(json_string, '[');
                if(values){
                    index = values - json_string;
                }
                if(json_string[i] == '['){
                    for(unsigned int j = index; j < strlen(json_string)  - 2; j++){

                        // Gère le premier élément du tableau de valeurs
                        if(json_string[j] == '"' && (json_string[j+1] >= 'a' && json_string[j+1] <= 'z')){
                            validate_res = 1;
                        }else if(json_string[j] >= '0' && json_string[j] <= '9' && json_string[j+1] == ','){
                            validate_res = 1;
                        }else {
                            validate_res = 0;
                        }
                        validate_res = json_string[j] == '"' && (json_string[j-1] >= 'a' && json_string[j-1] <= 'z') && json_string[j+1] == ',' ? 1 : 0;
                        // Fin traitement premier élément pour un nombre ou une chaîne de caractère

                        // Traitement du cas général des élements au milieu du tableau de valeurs
                        if(json_string[j] == '"' && json_string[j-1] == ',' && (json_string[j+1] >= 'a' && json_string[j+1] <= 'z')){
                            validate_res = 1;
                        }else if ((json_string[j] >= '0' && json_string[j] <= '9') && json_string[j+1] == ','){
                            validate_res = 1;
                        }else {
                            validate_res = 0;
                        }
                        if(json_string[j] == '"' && (json_string[j-1] >= 'a' && json_string[j-1] <= 'z') && json_string[j+1] == ',') {
                            validate_res = 1;
                        }else if(json_string[j] == '"' && (json_string[j-1] >= '0' && json_string[j-1] <= '9') && json_string[j+1] == ','){
                            validate_res = 1;
                        }else {
                            validate_res = 0;
                        }
                        // Fin traitement élément au milieu du tableau

                        // Traitement du dernier élément du tableau
                        if(json_string[j] == '"' && json_string[j-1] == ',' && (json_string[j+1] >= 'a' && json_string[j+1] <= 'z')){
                            validate_res = 1;
                        }else if (json_string[j] == '"' && json_string[j-1] == ',' && (json_string[j+1] >= '0' && json_string[j+1] <= '9')){
                            validate_res = 1;
                        }else {
                            validate_res = 0;
                        }
                        if(json_string[j] == '"' && (json_string[j-1] >= 'a' && json_string[j-1] <= 'z') && json_string[j+1] == ']'){
                            validate_res = 1;
                        }else if(json_string[j] == '"' && (json_string[j-1] >= '0' && json_string[j-1] <= '9') && json_string[j+1] == ']'){
                            validate_res = 1;
                        }else {
                            validate_res = 0;
                        }
                        i = j;
                        // Fin du traitement du dernier element   
                    }
                    validate_res = json_string[i] == ']' ? 1 : 0;
                    validate_res = json_string[i] == '}' ? 1 : 0;
                }
            }
        }else {
            validate_res = 0;
        }
    }else {
        validate_res = 0;
    }

    if(validate_res == 1){
        printf("aled\n");
    }else {
        printf("oskur\n");
    }
    return validate_res;
 }

/* 
 * Le but principal de ce code est de s'assurer que les messages envoyés par le client sont
 * valides, c'est-à-dire que les messages respectent le format JSON et
 * respectent le protocole spécifié par le côté serveur.
 */
