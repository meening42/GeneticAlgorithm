#include <stdio.h>
#include <stdlib.h>
#include "genAlgo.h"


const int tableLen = 21;
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


float fitnesQuadratic(float parameters[2]);


int main()
{


    GA_setChromosomeRange(0, -10.0, 10.0);
    GA_setChromosomeRange(1, -10.0, 10.0);

    if(tableLen){
        GA_init(fitnesQuadratic, 0.0000001);
        GA_compute();
    }
    GA_printBest();
    return 0;
}


float fitnesQuadratic(float parameters[2]){
    float aprox, diff, price;
    price = 0;
    for(int i= 0; i<tableLen; i++){
        aprox = parameters[0]*(float)inputTable[i][0]*(float)inputTable[i][0] + parameters[1];
        diff = inputTable[i][1]-aprox;
        price += diff*diff;
    }
    price = price/tableLen;
    return price;
}





