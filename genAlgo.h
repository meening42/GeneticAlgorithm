
/***************************************************************
* @file GA.h
* @brief GENETIC ALGORITHM
* @author Copyright (C) 2020  Marko A.
* @version v0.0.1
***************************************************************/

#ifndef GA_H_INCUDED
#define GA_H_INCUDED

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <float.h>

/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */
//#define NUM_OF_INDIVIDUALS 100
//#define NUM_OF_GENERATIONS 100
#define MAX_TABLE_LEN 1000
#define MAX_TABLE_WITDH 5
#define MAX_CHROMOSOMES 10
#define MAX_GENERATIONS 100

#define NUM_OF_INDIV 100
#define MAX_MUTATION_PROB 6
#define MUTATION_STEP 0.5
#define MUTATION_INIT_PROB 0.1
#define PRICE_MUTATION_TRESHOLD 0.0005f
#define ELITES 2                   // TODO Check elites smaller than num of individuas
#define BITS_IN_CHROMOSOME 15     // TODO check min 2 max 15 !!!
#define PRICE_AVRG_WINDOW 5




/* ==================================================================== */
/* ============================ STRUCTURES ============================ */
/* ==================================================================== */

typedef struct GA_individual{
    uint16_t chromosome[MAX_CHROMOSOMES];
    float parameters[MAX_CHROMOSOMES];
    float price;
    bool paired;
}GA_individual;

typedef struct GA_ChromosomeRange{
    float low;
    float up;
    bool used;
}GA_ChromosomeRange;




/* ==================================================================== */
/* ==================== GA function prototypes ======================== */
/* ==================================================================== */
void GA_init(float (*fitnesFunction)(), float priceTr);
void GA_compute(float table[][5]);
void GA_initPopulation();
void GA_ratePopulation(float table[][5]);
void GA_selection();
void GA_crossover();
void GA_mutation();
void GA_setChromosomeRange(int indx, float inLow, float inUp);

/// PRINT
void GA_printPopulation(int numIndv);
void GA_printIndividual();
void GA_printBest();
void GA_printSearchTable(int tLength,float table[][5]);

/// INPUT / OUTPUT
void GA_IOFileInit();
void GA_IOfileClose();
void GA_readTable(char* fileName, int tColumns, int tLength);
void GA_clearFiles();
void GA_outputPrice();
void GA_outputPriceAvrg();
void GA_outputPopulation();
void GA_outputMutation();
void GA_outputBestParameters();

#endif /* GA_H_INCUDED */
