#include <stdio.h>
#include <stdlib.h>
#include "genAlgo.h"
#include <math.h>


const int tableLen = 19;
float inputTable[19][2] = {
{-9.000000, 19.200000},
{-8.000000, 13.900000},
{-7.000000, 9.600000},
{-6.000000, 6.300000},
{-5.000000, 4.000000},
{-4.000000, 2.700000},
{-3.000000, 2.400000},
{-2.000000, 3.100000},
{-1.000000, 4.800000},
{0.000000, 7.500000},
{1.000000, 11.200000},
{2.000000, 15.900000},
{3.000000, 21.600000},
{4.000000, 28.300000},
{5.000000, 36.000000},
{6.000000, 44.700000},
{7.000000, 54.400000},
{8.000000, 65.100000},
{9.000000, 76.800000}
};
float sTable2[MAX_TABLE_LEN][MAX_TABLE_WITDH];
int readTable(char* fileName);


float fitnesQuadratic(float c[2]);


int main()
{


    GA_setChromMappingRange(0, -10.0, 10.0);
    GA_setChromMappingRange(1, -10.0, 10.0);
    GA_setChromMappingRange(2, -10.0, 10.0);

    GA_init(fitnesQuadratic, 0.0000001);
    GA_compute();

    GA_printBest();

    return 0;
}



float fitnesQuadratic(float c[3]){
    float aprox, diff, price;
    price = 0;
    for(int i= 0; i<tableLen; i++){
        aprox = c[0] * powf(inputTable[i][0], 2) + c[1]*(float)inputTable[i][0]+c[2];
        //printf("aprox at %d is %f", inputTable[i][0],aprox);
        diff = inputTable[i][1]-aprox;
        price += diff*diff;
    }
    price = price/tableLen;
    return price;
}





