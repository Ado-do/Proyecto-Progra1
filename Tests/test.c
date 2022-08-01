#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* copyString(char *str) {
    char* text = malloc(strlen(str) * sizeof(char));
    strcpy(text, str);
    return text;
}

int main(int argc, char const *argv[]) {
    
    // int x = 3 / 2;
    // printf("%d\n", x);
    
    char* mytext = copyString("Hola!");
    // char str[] = "Hola, wenas";
    // char* mytext = copyString(str);
    printf("%s\n", mytext);

    return 0;
}
