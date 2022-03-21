#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define SIZE_BLOCK 4
#define SIZE_GROUP 4
#define SIZE_CELL 8
#define NUM_CELL 4
#define BIT_REPLACEMENT 3
#define BIT_LABEL 4
#define BIT_ADDRESS 7
#define SIZE_CACHE 8
#define SIZE_MAIN_MEMORY 128
#define NUM_GROUP 2

int write_hit = 0;
int write_miss = 0;
int read_hit = 0;
int read_miss = 0;

typedef struct{
    char data[SIZE_CELL];
} Cell;

typedef struct{
    char replacement_count[BIT_REPLACEMENT];
    char label[BIT_LABEL];
    int validate;
    Cell *cells;
} Cache_Line;

typedef struct{
    Cache_Line lines[SIZE_CACHE];
} Cache_Memory;

typedef struct {
    Cell data[SIZE_MAIN_MEMORY];
} Main_Memory;

void strCopy(char * destiny, char * origin, int size){
    for (int i = 0; i < size; i++)
    {
        destiny[i] = origin[i];
    }
    return;
}

int binaryToDecimal(char * binary, int size){
    int decimal = 0;
    int exp = 0;
    for(int i = size-1; i >=0 ; i--,exp++){
        int bit = (int) binary[i]-48;
        decimal+= bit * pow(2,exp);
    }
    return decimal;
}

char *decimalToBinary(int decimal, int size){
    char *binary = malloc(sizeof(char)*size);
    for(int i = size-1; i >= 0; i--){
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
    for(int i = 0; i < SIZE_MAIN_MEMORY; i++){
        strCopy(main_memory->data[i].data, decimalToBinary(rand()%256, SIZE_CELL), SIZE_CELL);
    }

    return main_memory;
}

Cache_Memory *fillCacheMemory(Cache_Memory *cache_memory){
    cache_memory = malloc(sizeof(Cache_Memory));

    for (int i = 0; i < SIZE_CACHE; i++){
        cache_memory->lines[i].validate = 0;
        strCopy(cache_memory->lines[i].replacement_count, decimalToBinary(0,BIT_REPLACEMENT), BIT_REPLACEMENT);
        strCopy(cache_memory->lines[i].label, decimalToBinary(0,BIT_LABEL), BIT_LABEL);
        cache_memory->lines[i].cells = malloc(sizeof(Cell)*NUM_CELL);
        strCopy(cache_memory->lines[i].cells[0].data, decimalToBinary(0,SIZE_CELL), SIZE_CELL);
        strCopy(cache_memory->lines[i].cells[1].data, decimalToBinary(0,SIZE_CELL), SIZE_CELL);
        strCopy(cache_memory->lines[i].cells[2].data, decimalToBinary(0,SIZE_CELL), SIZE_CELL);
        strCopy(cache_memory->lines[i].cells[3].data, decimalToBinary(0,SIZE_CELL), SIZE_CELL);
    }
    return cache_memory;
}

void printMainMemory(Main_Memory *main_memory, int block){
    if(block == -1){
        printf("\n\t-> Imprimindo a Memória Principal...\n");
    }
    printf("\n|=============================================================================|\n");
    printf("|                              Memória Principal                              |\n");
    printf("|=============================================================================|\n");
    printf("|   Blocos   |\t\t\t    Endereços/Dados\t\t\t      |\n");
    if (block == -1)
    {
        for (int i = 0; i < SIZE_MAIN_MEMORY; i+=4){
            int nBlock = (i/SIZE_BLOCK);
            printf("|=============================================================================|\n");
            printf("|            |  %.*s   |",BIT_ADDRESS,decimalToBinary(i,BIT_ADDRESS));
            printf("  %.*s   |", BIT_ADDRESS,decimalToBinary(i+1,BIT_ADDRESS));
            printf("  %.*s   |" ,BIT_ADDRESS,decimalToBinary(i+2,BIT_ADDRESS));
            printf("  %.*s   |" ,BIT_ADDRESS,decimalToBinary(i+3,BIT_ADDRESS));
            printf("  Endereço  |\n");
            printf("|    %.*s    |------------+------------+------------+------------+------------|\n",SIZE_BLOCK,decimalToBinary(nBlock,SIZE_BLOCK));
            printf("|            |  %.*s  |", SIZE_CELL,main_memory->data[i].data);
            printf("  %.*s  |" ,SIZE_CELL,main_memory->data[i+1].data);
            printf("  %.*s  |" ,SIZE_CELL,main_memory->data[i+2].data);
            printf("  %.*s  |", SIZE_CELL,main_memory->data[i+3].data);
            printf("    Dado    |\n");
        }
        printf("|=============================================================================|\n");
        return;
    }
    int nBlock = (block/SIZE_BLOCK);

    printf("|=============================================================================|\n");
    printf("|            |  %.*s   |",BIT_ADDRESS,decimalToBinary(block,BIT_ADDRESS));
    printf("  %.*s   |", BIT_ADDRESS,decimalToBinary(block+1,BIT_ADDRESS));
    printf("  %.*s   |" ,BIT_ADDRESS,decimalToBinary(block+2,BIT_ADDRESS));
    printf("  %.*s   |" ,BIT_ADDRESS,decimalToBinary(block+3,BIT_ADDRESS));
    printf("  Endereço  |\n");
    printf("|    %.*s    |------------+------------+------------+------------+------------|\n",SIZE_BLOCK,decimalToBinary(nBlock,SIZE_BLOCK));
    printf("|            |  %.*s  |", SIZE_CELL,main_memory->data[block].data);
    printf("  %.*s  |" ,SIZE_CELL,main_memory->data[block+1].data);
    printf("  %.*s  |" ,SIZE_CELL,main_memory->data[block+2].data);
    printf("  %.*s  |", SIZE_CELL,main_memory->data[block+3].data);
    printf("    Dado    |\n");
    printf("|=============================================================================|\n");   
}

void printCacheMemory(Cache_Memory *cache_memory, int line){
    if(line == -1){
        printf("\n\t-> Imprimindo a Memória Cache...\n");
    }
    printf("\n|=====================================================================================================|\n");
    printf("|                                             Memória Cache                                           |\n");
    printf("|=====================================================================================================|\n");
    printf("|  Linha  |  Sub  |  Val  |  Rótulo  |                      Células                      |  Conjunto  |\n");

    if(line==-1){
        for (int i = 0; i < SIZE_CACHE; i++){
            printf("|---------+-------+-------+----------+---------------------------------------------------+------------|\n");
            printf("|   %.*s   |", 3,decimalToBinary(i, 3));
            printf("  %.*s  |", BIT_REPLACEMENT,cache_memory->lines[i].replacement_count);
            printf("   %d   |", cache_memory->lines[i].validate);
            printf("   %.*s   |", BIT_LABEL,cache_memory->lines[i].label);
            printf("  %.*s  |", SIZE_CELL, cache_memory->lines[i].cells[0].data);
            printf("  %.*s  |", SIZE_CELL,cache_memory->lines[i].cells[1].data);
            printf("  %.*s  |", SIZE_CELL,cache_memory->lines[i].cells[2].data);
            printf("  %.*s  |", SIZE_CELL,cache_memory->lines[i].cells[3].data);
            printf("      %.*s     |\n",1, decimalToBinary(i/4,1));
        }
        printf("|=====================================================================================================|\n");
    }
    else{
        printf("|---------+-------+-------+----------+---------------------------------------------------+------------|\n");
        printf("|   %.*s   |", 3,decimalToBinary(line, 3));
        printf("  %.*s  |", BIT_REPLACEMENT,cache_memory->lines[line].replacement_count);
        printf("   %d   |", cache_memory->lines[line].validate);
        printf("   %.*s   |", BIT_LABEL,cache_memory->lines[line].label);
        printf("  %.*s  |", SIZE_CELL, cache_memory->lines[line].cells[0].data);
        printf("  %.*s  |", SIZE_CELL,cache_memory->lines[line].cells[1].data);
        printf("  %.*s  |", SIZE_CELL,cache_memory->lines[line].cells[2].data);
        printf("  %.*s  |", SIZE_CELL,cache_memory->lines[line].cells[3].data);
        printf("      %.*s     |\n",1, decimalToBinary(line/4,1));
        printf("|=====================================================================================================|\n");
    }
}

int menu(){
    int opt;
    printf("\n|================================================================|\n");
    printf("|                              Menu                              |\n");
    printf("|================================================================|\n");
    printf("1 - Ler da Memória;\n");
    printf("2 - Escrever na Memória;\n");
    printf("3 - Ver Estatísticas;\n");
    printf("4 - Imprimir Memória Principal;\n");
    printf("5 - Imprimir Memória Cache;\n");
    printf("6 - Sair;\n");
    printf("\nSelecione uma opção: ");
    scanf("%i", &opt);

    return opt;
}

void pull(Main_Memory *main_memory, Cache_Memory *cache_memory, int memory_address){
    printf("\n\t-> Puxando valor da Memória Principal para a Cache...\n");
    int block = memory_address/SIZE_BLOCK;
    int group = block%NUM_GROUP;
    int label = block/NUM_GROUP;

    int smallest = pow(2,BIT_REPLACEMENT);
    int posSmallest;
    for (int i = group*SIZE_GROUP; i < group*SIZE_GROUP+SIZE_GROUP; i++)
    {
        if(binaryToDecimal(cache_memory->lines[i].replacement_count,BIT_REPLACEMENT) < smallest){
            smallest = binaryToDecimal(cache_memory->lines[i].replacement_count,BIT_REPLACEMENT);
            posSmallest = i;
        }
    }
    int j = 0;
    for (int i = block*SIZE_BLOCK; i < block*SIZE_BLOCK+SIZE_BLOCK; i++)
    {
        strCopy(cache_memory->lines[posSmallest].cells[j].data, main_memory->data[i].data, SIZE_CELL);
        j++;
    }
    strCopy(cache_memory->lines[posSmallest].label, decimalToBinary(label,BIT_LABEL), BIT_LABEL);
    cache_memory->lines[posSmallest].validate = 1;

    return;
}

void push(Main_Memory *main_memory, Cache_Memory *cache_memory, int memory_address, int cache_line){
    printf("\n\t-> Empurrando valor da Cache para a Memória Principal...\n");
    int block = memory_address/SIZE_BLOCK;
    
    int j = 0;
    for (int i = block*SIZE_BLOCK; i < block*SIZE_BLOCK+SIZE_BLOCK; i++)
    {
        strCopy(main_memory->data[i].data, cache_memory->lines[cache_line].cells[j].data, SIZE_CELL);
        j++;
    }
    
    return;
}

void read(Main_Memory *main_memory, Cache_Memory *cache_memory, int memory_address){
    int block = memory_address/SIZE_BLOCK;
    int displacement = memory_address%SIZE_BLOCK;
    int group = block%NUM_GROUP;
    int label = block/NUM_GROUP;

    printf("\n\t-> Buscando dado na Memória Cache para leitura...\n");

    for (int i = group*SIZE_GROUP; i < group*SIZE_GROUP+SIZE_GROUP; i++)
    {
        if (cache_memory->lines[i].validate == 1)
        {
            if(binaryToDecimal(cache_memory->lines[i].label, BIT_LABEL) == label){
                read_hit++;
                int rep_count = binaryToDecimal(cache_memory->lines[i].replacement_count, BIT_REPLACEMENT);
                rep_count++;
                if(rep_count > pow(2,BIT_REPLACEMENT)-1){
                    rep_count = pow(2,BIT_REPLACEMENT)-1;
                }
                strCopy(cache_memory->lines[i].replacement_count, decimalToBinary(rep_count, BIT_REPLACEMENT), BIT_REPLACEMENT);

                printf("\n --------------------------------");
                printf("\n| Endereço de Mem.  |  %s   |", decimalToBinary(memory_address,7));
                printf("\n|-------------------+------------|");
                printf("\n|       Bloco       |    %s    |", decimalToBinary(block, 4));
                printf("\n|-------------------+------------|");
                printf("\n|   Deslocamento    |     %s     |", decimalToBinary(displacement, 2));
                printf("\n|-------------------+------------|");
                printf("\n| Conjunto da Cache |     %s      |", decimalToBinary(group,1));
                printf("\n|-------------------+------------|");
                printf("\n|  Linha da Cache   |    %s     |", decimalToBinary(i,3));
                printf("\n|-------------------+------------|");
                printf("\n|       Valor       |  %.8s  |", cache_memory->lines[i].cells[displacement].data);
                printf("\n|-------------------+------------|");
                printf("\n|        Hit        |    TRUE    |");
                printf("\n --------------------------------\n");

                printf("\n\t-> Imprimindo Linha da Cache...\n");
                printCacheMemory(cache_memory,i);

                printf("\n\t-> Imprimindo Bloco da Memória Principal...\n");
                printMainMemory(main_memory,block*SIZE_BLOCK);

                return;
            }
        }
    }
    read_miss++;
    printf("\n --------------------------------");
    printf("\n|        Hit        |    FALSE   |");
    printf("\n --------------------------------\n");
    pull(main_memory,cache_memory,memory_address);
    read(main_memory,cache_memory,memory_address);
    read_hit--;
    return;
}

void write(Main_Memory *main_memory, Cache_Memory *cache_memory, int memory_address, int value){
    int block = memory_address/SIZE_BLOCK;
    int displacement = memory_address%SIZE_BLOCK;
    int group = block%NUM_GROUP;
    int label = block/NUM_GROUP;

    printf("\n\t-> Buscando dado na Memória Cache para escrita...\n");

    for (int i = group*SIZE_GROUP; i < group*SIZE_GROUP+SIZE_GROUP; i++)
    {
        if (cache_memory->lines[i].validate == 1)
        {
            if(binaryToDecimal(cache_memory->lines[i].label, BIT_LABEL) == label){
                printf("\n\t-> Imprimindo Linha da Cache antes da operação...\n");
                printCacheMemory(cache_memory,i);

                write_hit++;

                strCopy(cache_memory->lines[i].replacement_count, decimalToBinary(0,BIT_REPLACEMENT), BIT_REPLACEMENT);
                strCopy(cache_memory->lines[i].cells[displacement].data, decimalToBinary(value,SIZE_CELL), SIZE_CELL);

                printf("\n --------------------------------");
                printf("\n| Endereço de Mem.  |  %s   |", decimalToBinary(memory_address,7));
                printf("\n|-------------------+------------|");
                printf("\n|       Bloco       |    %s    |", decimalToBinary(block, 4));
                printf("\n|-------------------+------------|");
                printf("\n|   Deslocamento    |     %s     |", decimalToBinary(displacement, 2));
                printf("\n|-------------------+------------|");
                printf("\n| Conjunto da Cache |     %s      |", decimalToBinary(group,1));
                printf("\n|-------------------+------------|");
                printf("\n|  Linha da Cache   |    %s     |", decimalToBinary(i,3));
                printf("\n|-------------------+------------|");
                printf("\n|       Valor       |  %.8s  |", cache_memory->lines[i].cells[displacement].data);
                printf("\n|-------------------+------------|");
                printf("\n|        Hit        |    TRUE    |");
                printf("\n --------------------------------\n");

                push(main_memory,cache_memory,memory_address,i);

                printf("\n\t-> Imprimindo Linha da Cache depois da operação...\n");
                printCacheMemory(cache_memory,i);

                printf("\n\t-> Imprimindo Bloco da Memória Principal...\n");
                printMainMemory(main_memory,block*SIZE_BLOCK);
                return;
            }
        }
    }
    write_miss++;
    printf("\n --------------------------------");
    printf("\n|        Hit        |    FALSE   |");
    printf("\n --------------------------------\n");
    pull(main_memory,cache_memory,memory_address);
    write(main_memory, cache_memory, memory_address, value);
    write_hit--;
    return;
}

void printStatistics(){
    int acess_to_read = read_hit+read_miss;
    int acess_to_write = write_hit+write_miss;
    int total_acesses = acess_to_read+acess_to_write;
    int total_hits = read_hit+write_hit;
    int total_misses = read_miss+write_miss;

    printf("\n|===============================================|\n");
    printf("|                  Estatísticas                 |\n");
    printf("|===============================================|\n");
    printf("|           |  Acessos  |\t    %d\t\t|\n", acess_to_read);
    printf("|  Leitura  |   Hits    |\t%d (%.2f %%)\t|\n" ,read_hit, (float) read_hit/acess_to_read*100);
    printf("|           |  Misses   |\t%d (%.2f %%)\t|\n" ,read_miss, (float) read_miss/acess_to_read*100);
    printf("|-----------|-----------|-----------------------|\n");
    printf("|           |  Acessos  |\t    %d\t\t|\n", acess_to_write);
    printf("|  Escrita  |   Hits    |\t%d (%.2f %%)\t|\n" ,write_hit, (float) write_hit/acess_to_write*100);
    printf("|           |  Misses   |\t%d (%.2f %%)\t|\n" ,write_miss, (float) write_miss/acess_to_write*100);
    printf("|-----------|-----------|-----------------------|\n");
    printf("|           |  Acessos  |\t    %d\t\t|\n", total_acesses);
    printf("|   Total   |   Hits    |\t%d (%.2f %%)\t|\n" ,total_hits, (float) total_hits/total_acesses*100);
    printf("|           |  Misses   |\t%d (%.2f %%)\t|\n" ,total_misses, (float) total_misses/total_acesses*100);
    printf("|===============================================|\n");

}

bool binaryValidate(char * binary, int size){
    for (int i = 0; i < size; i++)
    {
        if(binary[i] != '0' && binary[i] != '1'){
            return false;
        }
    }
    return true;
}

int main(){
    Main_Memory *main_memory = fillMainMemory(main_memory);
    Cache_Memory *cache_memory = fillCacheMemory(cache_memory);

    while (1){
        // printMainMemory(main_memory, -1);
        // printCacheMemory(cache_memory, -1);
        switch (menu())
        {
        case 1:;
            char address[BIT_ADDRESS];
            printf("\nInsira a posição de memória a ser lida (valor em binário com 7 bits, sem espaço. ex: 0000000): ");
            scanf("%s", address);
            if(!binaryValidate(address, BIT_ADDRESS)){
                printf("\n\t*** AVISO: Insira um endereço válido! ***\n");
                break;
            }
            read(main_memory,cache_memory, binaryToDecimal(address, BIT_ADDRESS));
            break;
        case 2:;
            address[BIT_ADDRESS];
            char value[SIZE_CELL];
            printf("\nInsira a posição de memória a ser escrita (valor em binário com 7 bits, sem espaço. ex: 0000000): ");
            scanf("%s", address);
            if(!binaryValidate(address, BIT_ADDRESS)){
                printf("\n\t*** AVISO: Insira um endereço válido! ***\n");
                break;
            }
            printf("\nInsira o valor a ser escrito (valor em binário com 8 bits, sem espaço. ex: 00000000): ");
            scanf("%s", value);
            if(!binaryValidate(value, BIT_ADDRESS)){
                printf("\n\t*** AVISO: Insira um valor válido! ***\n");
                break;
            }
            write(main_memory,cache_memory, binaryToDecimal(address, BIT_ADDRESS), binaryToDecimal(value, SIZE_CELL));
            break;
        case 3:;
            printStatistics();
            break;
        case 4:;
            printMainMemory(main_memory, -1);
            break;
        case 5:;
            printCacheMemory(cache_memory, -1);
            break;    
        case 6:;
            exit(1);
            break;
        default:
            printf("ALERTA: Opção inválida\n");
            break;
        }
    }    
}