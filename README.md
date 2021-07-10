# Genetic Algorithm

C library with genetic (evolutionary) algorithm. Implementations and features: 
- generation of population (array of individduals)
- each individual has set number of chromosomes
- chromosome is represented with intiger number
- each chromosome can be automaticaly maped to a float type with desired range (use function GA_setParameterRange)
- selection
- crosover
- mutation    

## Usage

Include:
```c
#include "genAlgo.h"
```

GA will feed the parameters to fitnes function. Fitnes function evaluates the parameters and returns fitnes.
GA is minimizing fitnes so write fitnes function so that the best individual has the smallest fitnes.

Define your fitnes function example:

```c
float fitnesFunciton(float parameters[]){
    /*CALCULATE FITNES OF PARAMETERS HERE*/
    /*AND RETURN THE FITNES*/
    return fitnes;
}
```
GA needs to know what fitnes function to call. This is done with Ga_Init function.

Initialize GA:
```c
GA_init(fitnessFunction, 0.0000001);
```

Each individual has a set number of chromosomes. Chromosome is a number in Intiger form. 
You can set the number of bit representing a chromosome with BITS_IN_CHROMOSOME. If you want to use
floating ponint number you neet to call function GA_setParameterRange. This function will map each cromosome to a set  range.
```c
GA_setParameterRange(0, -10.0, 10.0);
```

Start genetic algorithm:
```c
GA_compute();
```
Print out best individual:
```c
GA_printBest();
```

## Examples
The library comes with example sketches. See Examples file 

## License

This code is released under the MIT License.