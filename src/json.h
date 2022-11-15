
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code permettant de stocker différents types de couleurs : format 24 bits (RGB) ou format 32 bits (32). 
 */

 //Fonction permettant de décoder une chaîne de caractère au format JSON
void json_decode(json_object *);

//Fonction qui transforme une chaîne de caractère au format JSON
json_object json_encode(char *);

//Structure de données pour un objet json
typedef struct 
{
    char* data;
    char* code;

}json_object