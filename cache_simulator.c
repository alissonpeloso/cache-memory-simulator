#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

typedef struct{
    char data[8];
} Cell;

typedef struct{
    Cell cells[4];
    int validate;
    char replacement_count[3];
    char label[4];
} Cache_Line;

typedef struct{
    Cache_Line lines[8];
} Cache_Memory;

typedef struct {
    Cell data[128];
} Main_Memory;

int binaryToDecimal(char * binary, int size){
    int decimal = 0;
    int exp = 0;
    for(int i = size-1; i >=0 ; i--,exp++){
        int bit = (int) binary[i]-48;
        decimal+= bit * pow(2,exp);
    }
    return decimal;
}

char *decimalToBinary(int decimal){
    char *binary = malloc(sizeof(char)*8);
    for(int i = sizeof(binary)-1; i >= 0; i--){
        if(decimal > 0){
            int bit = decimal % 2;
            decimal = decimal / 2;
            binary[i] = bit+'0';
        }
        else{
            binary[i] = '0';
        }
    }
    return binary;
}

Main_Memory *fillMainMemory(Main_Memory *main_memory){
    main_memory = malloc(sizeof(Main_Memory));
    srand(time(NULL));
    for(int i = 0; i < 128; i++){
        strcpy(main_memory->data[i].data, decimalToBinary(rand()%256));

    }

    return main_memory;
}

Cache_Memory *fillCacheMemory(Cache_Memory *cache_memory){
    cache_memory = malloc(sizeof(Cache_Memory));

    for (int i = 0; i < 8; i++){
        cache_memory->lines[i].validate = 0;
        strcpy(cache_memory->lines[i].replacement_count, "000");
        strcpy(cache_memory->lines[i].label, "0000");
        strcpy(cache_memory->lines[i].cells[0].data, "00000000");
        strcpy(cache_memory->lines[i].cells[1].data, "00000000");
        strcpy(cache_memory->lines[i].cells[2].data, "00000000");
        strcpy(cache_memory->lines[i].cells[3].data, "00000000");
    }
    return cache_memory;
}

void printMainMemory(Main_Memory *main_memory){
    printf("=============================================================================|\n");
    printf("                              Memória Principal                              |\n");
    printf("=============================================================================|\n");
    printf("   Blocos   |\t\t\t    Endereços/Dados\t\t\t     |\n");
    for (int i = 0; i < 128; i+=4){
        int nBlock = (i/4);
        printf("=============================================================================|\n");
        printf("            |  %.*s  |  %.*s  |  %.*s  |  %.*s  |  Endereço  |\n",8,decimalToBinary(i),8,decimalToBinary(i+1),8,decimalToBinary(i+2),8,decimalToBinary(i+3));
        printf("  %.*s  |------------+------------+------------+------------+------------|\n",8,decimalToBinary(nBlock));
        printf("            |  %.*s  |  %.*s  |  %.*s  |  %.*s  |    Dado    |\n",8,main_memory->data[i].data,8,main_memory->data[i+1].data,8,main_memory->data[i+2].data,8,main_memory->data[i+3].data);
    }
}

void printCacheMemory(Cache_Memory *cache_memory){
    printf("===========================================================================================|\n");
    printf("                                       Memória Cache                                       |\n");
    printf("===========================================================================================|\n");
    printf("  Endereço  |  Sub  |  Val  |  Rótulo  |                      Células                      |\n");
    for (int i = 0; i < 8; i++){
        printf("------------+-------+-------+----------+---------------------------------------------------|\n");
        printf("  %.*s  |  %.*s  |   %d   |   %s   |  %.*s  |  %.*s  |  %.*s  |  %.*s  |\n",8,decimalToBinary(i),3,cache_memory->lines[i].replacement_count,cache_memory->lines[i].validate,cache_memory->lines[i].label,8,cache_memory->lines[i].cells[0].data,8,cache_memory->lines[i].cells[1].data,8,cache_memory->lines[i].cells[2].data,8,cache_memory->lines[i].cells[3].data);
    }
}

int menu(){
    int opt;
    printf("================================================================|\n");
    printf("                              Menu                              |\n");
    printf("================================================================|\n");
    printf("1 - Ler da Memória;\n");
    printf("2 - Escrever na Memória;\n");
    printf("3 - Sair;\n");
    printf("Selecione uma opção: ");
    scanf("%i", &opt);

    return opt;
}

int main(){
    Main_Memory *main_memory = fillMainMemory(main_memory);
    Cache_Memory *cache_memory = fillCacheMemory(cache_memory);

    while (1){
        printMainMemory(main_memory);
        printCacheMemory(cache_memory);
        switch (menu())
        {
        case 1:
            /* code */
            break;
        case 2:
            /* code */
            break;
        case 3:
            /* code */
            break;
        default:
            printf("ALERTA: Opção inválida\n");
            break;
        }
    }    
}


// 128 c -> MP
// 4 -> bloco

//32 blocos na MP

// 8 l -> cache
// 8 bits -> cada célula
// 4 -> cada linha da cache -> 2 conjuntos de 4 linhas na cache

// tamanho do conjunto -> 4

// rótulos MP -> 7 bits

// 0000000 -> 4 bit rótulo / 1 bit pro conjunto / 2 bits pro deslocamento