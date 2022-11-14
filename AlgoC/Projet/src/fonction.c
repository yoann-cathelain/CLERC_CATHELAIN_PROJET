#include <string.h>
#include <stdlib.h>
#include <stdio.h>



void insertString(char * firsString, char * secondString, int position){
    char * finalString;
    finalString = (char*)malloc(strlen(firstString)+strlen(secondString)+1);

    strncpy(finalString, firstString, position);
    finalString[position] = "\0";

    strcat(finalString, secondString);
    strcat(finalString, firstString + position);
    strcpy(firstString, finalString);
    free(finalString);
}