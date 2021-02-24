#include <stdio.h>
#include <stdlib.h>
#include "genAlgo.h"




float inputTable[21][2] = {
    {-10,  204},
    {-9,   166},
    {-8,   132},
    {-7,   102},
    {-6,    76},
    {-5,    54},
    {-4,    36},
    {-3,    22},
    {-2,    12},
    {-1,     6},
    { 0,     4},
    { 1,     6},
    { 2,    12},
    { 3,     22},
    { 4,    36},
    { 5,    54},
    { 6,    76},
    {7,   102},
    {8,   132},
    {9,   166},
    {10,   204}
};
float sTable2[MAX_TABLE_LEN][MAX_TABLE_WITDH];
int readTable(char* fileName);

float fitnesQuadratic(float table[][5],int tableLen, float parameters[3]);

int main()
{
    printf("\nSTART!\n");

    int tableLen = 21;

    GA_setChromosomeRange(1, 0.0, 100.0);

    if(tableLen){
        GA_init(&fitnesQuadratic, 123.0);
        GA_compute(inputTable);
    }

    printf("GA FINISHED\n");
    return 0;
}



float fitnesQuadratic(float table[][5],int tableLen ,float parameters[3]){
    float aprox, diff, price;
    for(int i= 0; i<tableLen; i++){
        aprox = table[i][1]*table[i][1];
        diff = table[i][2]-aprox;
        price += diff*diff;
    }
    price = price/tableLen;
    return price;
}






