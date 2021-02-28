#include "genAlgo.h"

/// FILE POINTERS

static FILE *fEvo, *fPrice, *fMut, *fPriceAvrg, *fPop;
bool adaptiveMutation = true;
static int numOfChromos = 0;
static float mutationProb;
static float avrgPrice, priceGrad;
static int pairsArray[NUM_OF_INDIV][2];
static bool gaInitOK = false;
static float priceTreshold = 0;
static GA_ChromosomeRange chromosomeRange[MAX_CHROMOSOMES] = {0.0, 1.0, false};
static int generation = 0;
static initSOC = 100;
static GA_individual bestIndividual;
static GA_individual individualArray[NUM_OF_INDIV];
static float (*fitnesFunc)(float*);
// float searchTable[MAX_TABLE_LEN][3];

// TODO ged rid of all magic numbers


static void GA_calcParameters(){
    float temp1, temp2, temp3;
    for(int i = 0; i<NUM_OF_INDIV; i++){
        for(int c = 0; c<numOfChromos; c++){
            // calculate parameters value representet by chromosome
            temp1 = (float)individualArray[i].chromosome[c];
            temp2 = temp1 / (float)(((1<<BITS_IN_CHROMOSOME)-1)) * (chromosomeRange[c].up - chromosomeRange[c].low);
            temp3 = temp2 + chromosomeRange[c].low;
            if(temp3>chromosomeRange[c].up){
                printf("!!! over %d\n",c);
                temp3 = chromosomeRange[c].up;
            }
            if(temp3<chromosomeRange[c].low){
                temp3 = chromosomeRange[c].low;
                printf("!!! lower%d\n",c);
            }
            individualArray[i].parameters[c] = temp3;
        }
    }
}



///* ==================================================================== */
///* ============================  FUNCTIONS  =========================== */
///* ==================================================================== */

void GA_init(float(*fitnesFunction)(float*),float priceTr){
    fitnesFunc = fitnesFunction;
    gaInitOK = true;
    priceTreshold = priceTr;
    bestIndividual.price = FLT_MAX;
    GA_IOFileInit();
    printf("NUMBER OF CHROMOSOMES = %d\n", numOfChromos);
    printf("GA initialization succesfull!\n\n");
}


void GA_compute(){
    if(!gaInitOK){
        printf("Ga init not ok!\n");
        return;
    }
    time_t t;
    srand((unsigned) time(&t));
    GA_initPopulation();
    printf("GA initial population generated \n");

    for(int i = 0; i<MAX_GENERATIONS; i++){
        printf("\nGENERATION %d\n",generation);

        GA_ratePopulation();

        GA_selection();
        printf("best price = %f\n", bestIndividual.price);

        if(bestIndividual.price<priceTreshold){
            printf("Price treshold %f < %f reached!", bestIndividual.price, priceTreshold);
            break;
        }

        GA_crossover();

        GA_mutation();

        GA_outputMutation();
        generation++;
    }
    GA_IOfileClose();
}

void GA_initPopulation(){
    for(int i = 0; i<NUM_OF_INDIV; i++){
        for(int c = 0; c<numOfChromos; c++){
            individualArray[i].chromosome[c] = rand()%(1<<BITS_IN_CHROMOSOME);
        }
    }
    bestIndividual.price = FLT_MAX;
}


void GA_ratePopulation(){
    GA_calcParameters();
    for(int i = 0; i < NUM_OF_INDIV; i++){
        individualArray[i].price = (*fitnesFunc)(&individualArray[i].parameters);
    }
}

static int GA_comparator(const void* p, const void* q)
{
    const struct GA_individual *elem1 = p;
    const struct GA_individual *elem2 = q;
    return (fabs(elem1->price - elem2->price) < (float)0.0000001f)? 0:
                 elem1->price - elem2->price > 0? 1: -1;
}


void GA_selection(){
    qsort(individualArray, NUM_OF_INDIV, sizeof(struct GA_individual), GA_comparator);
    int quarter = NUM_OF_INDIV /4;
    for(int i = 0; i<quarter; i++){
        individualArray[NUM_OF_INDIV-i-1] =individualArray[i];
    }
    if(individualArray[0].price < bestIndividual.price){
        //printf("New best %f < %f\n",popul[0].price,bestIndividual.price);
        bestIndividual = individualArray[0];
    }
    else{
        //printf("FAILED To make better individual\n");
    }
}


static void GA_pairIndividuals(){
    int indx = 0;
    int j;
    for(int i = 0; i<NUM_OF_INDIV/2; i++){
        do{
            indx =  rand()%NUM_OF_INDIV;
            if(!individualArray[indx].paired){
                pairsArray[i][0] = indx;
                individualArray[indx].paired  = true;
                break;
            }
        }
        while(individualArray[indx].paired);
    }
    j = 0;
    for(int i = 0; i<NUM_OF_INDIV; i++){
        if(!individualArray[i].paired){
            pairsArray[j][1] = i;
            individualArray[i].paired  = true;
            j++;
        }
    }
}

static void GA_printPairedTable(){
    for(int i = 0; i<NUM_OF_INDIV/2+1; i++){
        printf("pair %d: = %d, %d" , i , pairsArray[i][0], pairsArray[i][1] );
        if(abs(pairsArray[i][0] - pairsArray[i][1]) == 75){
            printf("same !!!\n");
        }
        else{
            printf("\n");
        }
    }
}


static void GA_resetPaired(){
    for(int i = 0; i<NUM_OF_INDIV; i++){
        individualArray[i].paired  = false;
    }
}


static uint16_t max(uint16_t num1, uint16_t num2){
    return (num1 > num2 ) ? num1 : num2;
}


void GA_crossover(){
    uint16_t crossBit, indx1, indx2;
    // check The Knuth algorithm
    GA_pairIndividuals();
    uint16_t leftMask, rightMask;
    for(int i = 0;i<NUM_OF_INDIV/2;i++){
        indx1 = pairsArray[i][0];
        indx2 = pairsArray[i][1];
        for(int j = 0; j < numOfChromos; j++){
            uint16_t temp = 0;// max(individualArray[indx1].gaIndv.chromosome[j],individualArray[indx2].gaIndv.chromosome[j]);
            //printf("%d %x   ",temp,temp);
            crossBit = rand()%BITS_IN_CHROMOSOME;
            //printf("%d\n", crossBit);
            rightMask = (1<<crossBit)-1; // for crossBit 3 rightMask = 00000111
            leftMask = ~rightMask;       // for crossBit 3 leftMask  = 11111000
            temp = individualArray[indx1].chromosome[j];
            individualArray[indx1].chromosome[j] = (leftMask & temp) | (rightMask & individualArray[indx2].chromosome[j]);
            individualArray[indx2].chromosome[j] = (leftMask & individualArray[indx2].chromosome[j]) | (rightMask & temp);
        }
    }
    GA_resetPaired();
}


void GA_mutation(){
    int nBits = numOfChromos * NUM_OF_INDIV * BITS_IN_CHROMOSOME;
    int nMutations;
    if(adaptiveMutation && (generation>= PRICE_AVRG_WINDOW)){
        //printf("price grad = %f\n",priceGrad);
        if(fabs(priceGrad) < PRICE_MUTATION_TRESHOLD){
            mutationProb = mutationProb + MUTATION_STEP;
        }
        else{
            mutationProb = MUTATION_INIT_PROB;
        }
    }
    else{
        mutationProb = MUTATION_INIT_PROB;
    }
    if(mutationProb>MAX_MUTATION_PROB){
        mutationProb = MAX_MUTATION_PROB;
    }
    nMutations =  (float)nBits * mutationProb * 0.01f;
    for(int i = 0; i<nMutations; i++){
        int indxIndv = (rand()%(NUM_OF_INDIV - ELITES)) + ELITES;
        //printf("mutated ind = %d\n", indxIndv);
        int indxChrom = rand()%numOfChromos;
        int indxBit = rand()%(BITS_IN_CHROMOSOME);
        uint16_t mask = 1<<indxBit;
        individualArray[indxIndv].chromosome[indxChrom] ^= mask;
    }
}

void GA_setChromosomeRange(int indx, float inLow, float inUp){
    chromosomeRange[indx].low = inLow;
    chromosomeRange[indx].up = inUp;
    chromosomeRange[indx].used = true;
    // count number of used chromosomes
    numOfChromos = 0;
    for(int i = 0; i<MAX_CHROMOSOMES;i++){
        if(chromosomeRange[i].used){
            numOfChromos++;
        }
    }
    return;
}





///* ==================================================================== */
/// PRINT FUNCTIONNS
///* ==================================================================== */

void GA_printIndividual(GA_individual  * ind){
    for(int i = 0; i<numOfChromos; i++){
        printf("\nparamter %d = %f",ind->parameters[i]);
    }
}

void GA_printBest(){
    printf("BEST INDIVIDUAL:\n");
    printf("chromosomes = ");
    for(int c = 0; c<numOfChromos;c++){
        printf("%d  ",bestIndividual.chromosome[c] );
    }
    printf("\nparameters  =");
    for(int c = 0; c<numOfChromos;c++){
        printf("%f  ",bestIndividual.parameters[c] );
    }
    printf("\nprice: %f\n\n", bestIndividual.price);
}


void GA_printPopulation(int numIndv){
    static int gen = 0;
    //printf("GEN %d\n",gen);
    for(int i = 0; i<numIndv; i++){
        printf(" INDIV %d ",i);
        printf("  Parameters: ");
        for(int c = 0; c<numOfChromos;c++){
            printf("%.3f  ",individualArray[i].parameters[c]);
        }
        printf("\n");
        printf("           Chromosome: ");
        for(int c = 0; c<numOfChromos;c++){
            printf("%d,  ",individualArray[i].chromosome[c]);
        }
        printf("\n");
//        printf("    R0  =%.5f\n",individualArray[i].R0);
//        printf("    eta =%.5f\n",individualArray[i].eta);
        printf("  PRICE: %f\n", individualArray[i].price);
    }
    printf("\n\n\n\n");
    gen++;
}

///* ==================================================================== */
/// INPUT OUTPUT FUNCTIONS
///* ==================================================================== */

void GA_IOFileInit(){
    /// CLEAR FILES
    remove("evolution.txt");
    fEvo = fopen("evolution.txt", "w");
    fclose(fEvo);

    remove("price.txt");
    fPrice = fopen("price.txt", "w");
    fclose(fPrice);

    remove("mutation.txt");
    fMut = fopen("mutation.txt", "w");
    fclose(fMut);

    remove("priceAvrg.txt");
    fPriceAvrg = fopen("priceAvrg.txt", "w");
    fclose(fPriceAvrg);

    fEvo = fopen("evolution.txt", "a");
    fPrice = fopen("price.txt", "a");
    fPriceAvrg = fopen("priceAvrg.txt", "a");
    fMut = fopen("mutation.txt", "a");
}

void GA_IOfileClose(){
    fclose(fEvo);
    fclose(fPrice);
    fclose(fMut);
    fclose(fPriceAvrg);
}

//void GA_readTable(char* fileName, int tColumns, int tLenght){
//    FILE * fTable;
//    fTable = fopen(fileName, "r" );
//    if(searchTable == 0){
//        printf("Can't open file\n");
//        return;
//    }
//    int i = 0;
//    if(tColumns == 2){
//        while (fscanf(fTable,"%f %f",&searchTable[i][0], &searchTable[i][1]) ==2){i++;}
//    }
//    if(tColumns == 3){
//        while (fscanf(fTable,"%f %f %f",&searchTable[i][0], &searchTable[i][1], &searchTable[i][2]) ==3){i++;}
//    }
//    fclose(fTable);
//    // GA_printSearchTable(tLenght);  // optional print of tabel
//    return;
//}

void GA_outputPopulation(){
    for(int i = 0; i<NUM_OF_INDIV; i++){
        for(int j = 0;j<numOfChromos;j++){
             fprintf(fPop, "%f ", individualArray[i].parameters[j]);
        }
        fprintf(fPop,"\n");
    }
}

void GA_outputBestParameters(){
    printf("Write best parameters to file...\n");
    FILE *fPar;
    fPar = fopen("parameters.txt", "w");
    if(fPar == 0){
        printf("Can't open file parameters.txt\n");
    }
    else{
        for(int i = 0; i< numOfChromos; i++){
            //printf("%f \n", bestIndividual.parameters[i]);
            fprintf(fPar, "%f \n", bestIndividual.parameters[i]);
        }
    }
    fclose(fPar);
}


void GA_outputPrice(){
    for(int i = 0; i<1;i++){
        fprintf(fPrice, "%e ", individualArray[i].price);
    }
    fprintf(fPrice,"\n");
}

void GA_outputPriceAvrg(){
    fprintf(fPriceAvrg, "%f \n", avrgPrice);
}

void GA_outputMutation(){
    fprintf(fMut, "%f \n",mutationProb);
}


