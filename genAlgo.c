#include "genAlgo.h"

/// FILE POINTERS

static FILE *fEvo, *fPrice, *fMut, *fPriceAvrg, *fPop;
bool adaptiveMutation = true;
static int numOfIndivid, tableLength, numOfChromos;
static float mutationProb;
static float avrgPrice, priceGrad;
static int pairsArray[NUM_OF_INDIV][2];
static bool gaInitOK = false;
static float priceTreshold = 0;
static GA_ChromosomeRange chromosomeRange[MAX_CHROMOSOMES] = {0.0, 1.0, false};
static int generation = 0;
static initSOC = 100;
static GA_individual bestIndividual;
// float searchTable[MAX_TABLE_LEN][3];


// TODO ged rid of all magic numbers
//



///* ==================================================================== */
///* ============================  FUNCTIONS  =========================== */
///* ==================================================================== */

void GA_init(float (*priceFun)(),float priceTr){
    gaInitOK = true;
    priceTreshold = priceTr;
    bestIndividual.price = FLT_MAX;
    GA_IOFileInit();
    printf("GA initialization succesfull!\n");
}


void GA_compute(float table[][5]){
    if(!gaInitOK){
        printf("Ga init not ok!\n");
        return;
    }
    printf("GA COMPUTE \n");
    time_t t;
    srand((unsigned) time(&t));
    GA_initPopulation();
    printf("GA initial population generated \n");
    //GA_printPopulation(numOfIndivid);
    for(int i = 0; i<MAX_GENERATIONS; i++){
        //printf("\n\nGENERATION %d\n",generation);
        //printf("RATE: \n");
        GA_ratePopulation(table);
        GA_selection();
        GA_outputPrice();
        GA_outputPriceAvrg();
        if(bestIndividual.gaIndv.price<priceTreshold){
            printf("Price treshold %f < %f reached!", bestIndividual.gaIndv.price, priceTreshold);
            break;
        }
        //GA_printPopulation(numOfIndivid);
       // printf("SELECTION: \n");


        //GA_printPopulation(numOfIndivid);

        //printf("CROSOVER: \n");
        GA_crossover();
        //GA_printPopulation(numOfIndivid);
        //printf("MUTATION: \n");
        GA_mutation();
        //GA_printPopulation(numOfIndivid);
        GA_outputMutation();
        generation++;
    }
    GA_printPopulation(2);
    GA_printBest();
    GA_outputBestParameters();
    GA_IOfileClose();
}

void GA_initPopulation(){
    for(int i = 0; i<numOfIndivid; i++){
        ESC_initCell(&populEsc[i]);
        for(int c = 0; c<numOfChromos; c++){
            populEsc[i].gaIndv.chromosome[c] = rand()%(1<<BITS_IN_CHROMOSOME);
        }
    }
    bestIndividual.gaIndv.price = 99999.9;
}


void GA_ratePopulation(float table[][5]){
    for(int i = 0; i<numOfIndivid; i++){
        ESC_initCell(&populEsc[i]);
    }
    GA_calcEscParameters();
    float price;
    for(int i = 0; i<numOfIndivid; i++){
        price = 0.0f;
        for(int j = 0; j<tableLength; j++){
            float aprox;
            //aprox = polyFunc(ind, table, i,j);
            //aprox = furierFunc(searchTable,i,j);
            aprox = escVbatAprox(table, i, j);
            price = price + fabs((aprox - table[j][2])); //* (aprox - table[j][2]));
            //printf("   price %f\n",price);
        }
        //printf("\n");
        populEsc[i].gaIndv.price = price;
    }
}


void GA_selection(){
    qsort(populEsc, numOfIndivid, sizeof(struct Cell), GA_comparator);
    int quarter = numOfIndivid /4;
    for(int i = 0; i<quarter; i++){
        populEsc[numOfIndivid-i-1] =populEsc[i];
    }
    if(populEsc[0].gaIndv.price < bestIndividual.gaIndv.price){
        //printf("New best %f < %f\n",popul[0].price,bestIndividual.price);
        bestIndividual = populEsc[0];
    }
    else{
        //printf("FAILED To make better individual\n");
    }
    GA_priceMonitor();
}


static void GA_pairIndividuals(){
    int indx = 0;
    int j;
    for(int i = 0; i<numOfIndivid/2; i++){
        do{
            indx =  rand()%numOfIndivid;
            if(!populEsc[indx].gaIndv.paired){
                pairsArray[i][0] = indx;
                populEsc[indx].gaIndv.paired  = true;
                break;
            }
        }
        while(populEsc[indx].gaIndv.paired);
    }
    j = 0;
    for(int i = 0; i<numOfIndivid; i++){
        if(!populEsc[i].gaIndv.paired){
            pairsArray[j][1] = i;
            populEsc[i].gaIndv.paired  = true;
            j++;
        }
    }
}

static void GA_printPairedTable(){
    for(int i = 0; i<numOfIndivid/2+1; i++){
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
    for(int i = 0; i<numOfIndivid; i++){
        populEsc[i].gaIndv.paired  = false;
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
    for(int i = 0;i<numOfIndivid/2;i++){
        indx1 = pairsArray[i][0];
        indx2 = pairsArray[i][1];
        for(int j = 0; j < numOfChromos; j++){
            uint16_t temp = 0;// max(populEsc[indx1].gaIndv.chromosome[j],populEsc[indx2].gaIndv.chromosome[j]);
            //printf("%d %x   ",temp,temp);
            crossBit = rand()%BITS_IN_CHROMOSOME;
            //printf("%d\n", crossBit);
            rightMask = (1<<crossBit)-1; // for crossBit 3 rightMask = 00000111
            leftMask = ~rightMask;       // for crossBit 3 leftMask  = 11111000
            temp = populEsc[indx1].gaIndv.chromosome[j];
            populEsc[indx1].gaIndv.chromosome[j] = (leftMask & temp) | (rightMask & populEsc[indx2].gaIndv.chromosome[j]);
            populEsc[indx2].gaIndv.chromosome[j] = (leftMask & populEsc[indx2].gaIndv.chromosome[j]) | (rightMask & temp);
        }
    }
    GA_resetPaired();
}


void GA_mutation(){
    int nBits = numOfChromos * numOfIndivid * BITS_IN_CHROMOSOME;
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
        int indxIndv = (rand()%(numOfIndivid - ELITES)) + ELITES;
        //printf("mutated ind = %d\n", indxIndv);
        int indxChrom = rand()%numOfChromos;
        int indxBit = rand()%(BITS_IN_CHROMOSOME);
        uint16_t mask = 1<<indxBit;
        populEsc[indxIndv].gaIndv.chromosome[indxChrom] ^= mask;
    }
}

void GA_setChromosomeRange(int indx, float inLow, float inUp){
    chromosomeRange[indx].low = inLow;
    chromosomeRange[indx].up = inUp;
    chromosomeRange[indx].used = true;
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

void GA_printIndividual(Cell  * ind){
    printf("\n\nz =    %f\n",ind->z);
    printf("eta =  %f\n",ind->eta);
    printf("Fj =   %f\n",ind->Fj[0]);
    printf("h =    %f\n",ind->h);
    printf("s =    %d\n",ind->s);
    printf("ocv =  %f\n",ind->ocv);
    printf("Vbat = %f\n",ind->vBatt);
}

void GA_printBest(){
    printf("BEST INDIVIDUAL:\n");
    printf("chromosomes = ");
    for(int c = 0; c<numOfChromos;c++){
        printf("%d  ",bestIndividual.gaIndv.chromosome[c] );
    }
    printf("\nparameters  =");
    for(int c = 0; c<numOfChromos;c++){
        printf("%f  ",bestIndividual.gaIndv.parameters[c] );
    }
    printf("\n    R0  =%.5f\n",bestIndividual.gaIndv.parameters[0]);
    printf("    eta =%.5f\n",bestIndividual.gaIndv.parameters[1]);
    printf("    gama=%.5f\n",bestIndividual.gaIndv.parameters[2]);
    printf("    M   =%.5f\n",bestIndividual.gaIndv.parameters[3]);
    printf("    M0  =%.5f\n",bestIndividual.gaIndv.parameters[4]);
    printf("    R1  =%.5f\n",bestIndividual.gaIndv.parameters[5]);
    printf("    C1  =%.5f\n",bestIndividual.gaIndv.parameters[6]);
    printf("\nprice: %f\n\n", bestIndividual.gaIndv.price);
}


void GA_printPopulation(int numIndv){
    static int gen = 0;
    //printf("GEN %d\n",gen);
    for(int i = 0; i<numIndv; i++){
        printf(" INDIV %d ",i);
        printf("  Parameters: ");
        for(int c = 0; c<numOfChromos;c++){
            printf("%.3f  ",populEsc[i].gaIndv.parameters[c]);
        }
        printf("\n");
        printf("           Chromosome: ");
        for(int c = 0; c<numOfChromos;c++){
            printf("%d,  ",populEsc[i].gaIndv.chromosome[c]);
        }
        printf("\n");
//        printf("    R0  =%.5f\n",populEsc[i].R0);
//        printf("    eta =%.5f\n",populEsc[i].eta);
//        printf("    gama=%.5f\n",populEsc[i].gama);
//        printf("    M   =%.5f\n",populEsc[i].M);
//        printf("    M0  =%.5f\n",populEsc[i].M0);
//        printf("    R1  =%.5f\n",populEsc[i].rCoef[0]);
//        printf("    C1  =%.5f\n",populEsc[i].cCoef[0]);
        printf("  PRICE: %f\n", populEsc[i].gaIndv.price);
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
    for(int i = 0; i<numOfIndivid; i++){
        for(int j = 0;j<numOfChromos;j++){
             fprintf(fPop, "%f ", populEsc[i].gaIndv.parameters[j]);
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
            fprintf(fPar, "%f \n", bestIndividual.gaIndv.parameters[i]);
        }
    }
    fclose(fPar);
}


void GA_outputPrice(){
    for(int i = 0; i<1;i++){
        fprintf(fPrice, "%e ", populEsc[i].gaIndv.price);
    }
    fprintf(fPrice,"\n");
}

void GA_outputPriceAvrg(){
    fprintf(fPriceAvrg, "%f \n", avrgPrice);
}

void GA_outputMutation(){
    fprintf(fMut, "%f \n",mutationProb);
}

void GA_outputVbatSoc(float table[][5]){
    FILE *fVbat;
    fVbat = fopen("vbatSocISocV.txt", "w");
    Cell testVbat;
    ESC_initCell(&testVbat);
    testVbat.R0 = bestIndividual.R0;
    testVbat.eta = bestIndividual.eta;
    testVbat.gama = bestIndividual.gama;
    testVbat.M = bestIndividual.M;
    testVbat.M0 = bestIndividual.M0;
    testVbat.rCoef[0] = bestIndividual.rCoef[0];
    testVbat.cCoef[0] = bestIndividual.cCoef[0];
    testVbat.rCoef[1] = bestIndividual.rCoef[1];
    testVbat.cCoef[1] = bestIndividual.cCoef[1];

    for(int i = 0; i<tableLength;i++){
        ESC_recalculateCell(&testVbat, table[i][0],  table[i][1], table[i][3]);
        fprintf(fVbat, "%f %f %f\n",testVbat.vBatt, testVbat.z*100, testVbat.SOC);
    }
     fclose(fVbat);
}

void GA_outputESCtest(float table[][5], int tableLength){
    FILE *fEscTest;
    fEscTest = fopen("escTest.txt", "w");
    if(fEscTest == 0){
        printf("Can't open file escTest.txt\n");
        return;
    }
    Cell testEsc;
    ESC_initCell(&testEsc);
    testEsc.R0          = 0.0082f;
    testEsc.eta         = 0.98f;
    testEsc.gama        = 3600.0f;
    testEsc.M           = 150e-5f;
    testEsc.M0          = 100e-5f;
    testEsc.rCoef[0]    = 0.00158f;
    testEsc.cCoef[0]    = 38000.0f;

    for(int i = 0; i<tableLength;i++){
        ESC_recalculateCell(&testEsc, table[i][0],  table[i][1],  table[i][3]);
        //printf("vBatt = %f\n",testEsc.vBatt);
        fprintf(fEscTest, "%f %f\n",testEsc.vBatt, testEsc.z);
    }
    fclose(fEscTest);
    printf("Finished esc test!!!");
}
