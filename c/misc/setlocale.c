#include <stdio.h>
#include <time.h>
#include <locale.h>


int main(int argc, char *argv) 
{ 
    time_t now; 
    struct tm *timeinfo; 
    char str[100]; 

    time(&now); 
    timeinfo = localtime(&now); 

    printf("Locale is :%s\n", setlocale(LC_ALL, NULL)); 
    strftime(str, 100, "%c", timeinfo); 
    printf("now is :%s\n", str); 

    setlocale(LC_ALL, ""); 
    printf("Locale is :%s\n", setlocale(LC_ALL, NULL)); 
    strftime(str, 100, "%c", timeinfo); 
    printf("now is :%s\n", str); 
}
