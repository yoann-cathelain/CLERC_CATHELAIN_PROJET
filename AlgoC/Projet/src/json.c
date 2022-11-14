/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
/*
 * Code pour le traitement des messages au format JSON
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json.h>
#include <fonction.h>

json_object json_decode(char* json_string) 
{
    json_object json_res;
    json_res.code = strtok(json_string, "{\"code\": ");
    json_res.valeurs = strtok(NULL, ",\"valeurs\": ");
    return json_res;
}

char * json_encode(json_object *str, char * delim){
    char * json_string;
    sprintf(json_string, "{\"code\": %s,", str.code);
    return json_string;

}

