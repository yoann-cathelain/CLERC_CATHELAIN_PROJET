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
#include "json.h"

void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

json_object json_decode(char* json_string) 
{
    json_object json_res;
    char* json_string_copy;
    char* code;
    char* value;
    char* data;

    json_string_copy = malloc(sizeof(char)*1024);
    data = malloc(sizeof(char)*1024);
    code = malloc(sizeof(char)*1024);
    value = malloc(sizeof(char)*1024);
    json_res.code = malloc(sizeof(char)*1024);
    json_res.valeurs = malloc(sizeof(char)*1024);

    json_string_copy = strcpy(json_string_copy,json_string);
    data = strtok(json_string_copy, ",");
    code = data;
    while(data != NULL){
        if(data == NULL) {
            break;
        }else {
            value = data;
            data = strtok(NULL, ",");

        }

    }
    free(data);
    code = strtok(code,": ");
    code = strtok(NULL,": \"");

    for(unsigned int i =0 ; i < strlen(code); i++){
        if(code[i] == '"'){
            code[i] = code[i+1];
        }
    }
    value = strtok(value, ": [");
    value = strtok(NULL, ": [");

    for(unsigned int i = 0; i <= strlen(value); i++){
        if(value[i] == '"' || value[i] == ']'){
            removeChar(value, value[i]);
        }
        if(value[i] == '}'){
            removeChar(value, value[i]);
        }
    }
    
    json_res.code = code;
    json_res.valeurs = value;

    return json_res;
}

char* json_encode(json_object *str, char delim){

    char *json_string = malloc(sizeof(char) * 1024);
    char* datatmp;
    char* data;
    datatmp = malloc(sizeof(char)*1024);
    data = malloc(sizeof(char) * 1024);
    sprintf(json_string, "{\"code\": \"%s\",\"valeur\": [", str->code);
    if(delim == '\x32'){
        datatmp = strtok(str->valeurs, " ");
        sprintf(data, "\"%s\"", datatmp);
        strcat(json_string,data);
        while(datatmp != NULL){
            datatmp = strtok(NULL, " ");
            if(datatmp == NULL){
            }else {
                printf("%s", datatmp);        
                sprintf(data,",\"%s\"",datatmp);
                strcat(json_string,data);
            }
        }
        strcat(json_string, "]}");
    }else if(delim == '\x44'){
        datatmp = strtok(str->valeurs, ",");
        sprintf(data, "\"%s\"", datatmp);
        strcat(json_string,data);
        while(datatmp != NULL){
            datatmp = strtok(NULL, ",");
            if(datatmp == NULL){

            }else {
                sprintf(data,",\"%s\"", datatmp);
                strcat(json_string, data);
            }
        }
        strcat(json_string, "]}");
    }
    free(data);
    return json_string;

}

