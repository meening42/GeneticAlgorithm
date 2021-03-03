# Genetic Algorithm

C library with genetic (evolutionary) algorithm. Implementations and features: 
- generation of population (array of individduals)
- each individual has set number of chromosomes
- chromosome is represented with intiger number
- each chromosome can be automaticaly maped to a float type with desired range (use function GA_setParameterRange)
- selection
- crosover
- mutation    

## Examples
The library comes with example sketches. See Examples file 

## Usage

Include:
```c
#include "genAlgo.h"
```
Initialize:
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

## License

This code is released under the MIT License.