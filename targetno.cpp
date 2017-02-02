#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace std;

#define POP_SIZE 100
#define CHROMO_LENGTH 300
#define CROSSOVER_RATE 0.7
#define MUTATION_RATE 0.001
#define MAX_GENERATIONS 10000

struct chromosome {
    string bits;
    float fitness;
};

string getRandomBits(int length) {
    string result = "";
    for (int i=0; i<length ; i++) {
        result += '0'+(rand()%2);
    }
    return(result);
}

string binToDec(string bin) {
    string dec;
    if (bin.compare("0000")==0) dec="0";
    else if (bin.compare("0001")==0) dec="1";
    else if (bin.compare("0010")==0) dec="2";
    else if (bin.compare("0011")==0) dec="3";
    else if (bin.compare("0100")==0) dec="4";
    else if (bin.compare("0101")==0) dec="5";
    else if (bin.compare("0110")==0) dec="6";
    else if (bin.compare("0111")==0) dec="7";
    else if (bin.compare("1000")==0) dec="8";
    else if (bin.compare("1001")==0) dec="9";
    else if (bin.compare("1010")==0) dec="+";
    else if (bin.compare("1011")==0) dec="-";
    else if (bin.compare("1100")==0) dec="*";
    else if (bin.compare("1101")==0) dec="/";
    else dec="_";   //For 1111 like sequences
    return dec;
}

bool isOperator(string c) {
    return (c.compare("+")==0 | c.compare("-")==0 |
        c.compare("*")==0 | c.compare("/")==0);
}

chromosome findFittest(chromosome* Population) {
    chromosome fittest = Population[0];
    for (int i=0 ; i<POP_SIZE ; i++) {
        if (Population[i].fitness > fittest.fitness) {
            fittest = Population[i];
        }
    }
    return fittest;
}

string decodeBits(string bits) {
    string result = "";
    bool lookingForOp = false;
    for (int i=0 ; i<bits.length() ; i+=4) {
        string c = binToDec(bits.substr(i,4));
        if (lookingForOp && isOperator(c) && c.compare("_")!=0) {
            result += c;
            lookingForOp = false;
        }
        if (!lookingForOp && !isOperator(c) && c.compare("_")!=0) {
            result += c;
            lookingForOp = true;
        }
    }

    for(int j=0 ; j<result.length() ; j++) {
        if (j>0) {
            if (result[j]=='0' && result[j-1]=='/') {
                result[j-1] = '+';
            }
        }
    }
    if (isOperator(string(1, result[result.length()-1]))) {
        result = result.substr(0, result.length()-1);
    }
    return (result);
}

float evalExpression(float target, string decoded) {
    float result;
    if (decoded.length()==0) {
        return(0.0);
    }
    result = (float)(decoded[0]-'0');
    if (decoded.length() > 1) {
        int i = 1;
        while (i < decoded.length()) {
            switch(decoded[i]) {
                case '+':
                    result += (float)(decoded[i+1]-'0');
                    i += 2;
                    break;
                case '-':
                    result -= (float)(decoded[i+1]-'0');
                    i += 2;
                    break;
                case '*':
                    result *= (float)(decoded[i+1]-'0');
                    i += 2;
                    break;
                case '/':
                    result /= (float)(decoded[i+1]-'0');
                    i += 2;
                    break;
                default:
                    break;
            }
        }
    }
    return (result);
}

float assignFitness(float target, string decoded) {
    float result = evalExpression(target, decoded);
    if (result == target) return(1000.0);
    float fitness = 1.0/fabs(target-result);
    return (fitness);
}

void Log(float target, int generationNo, chromosome* Population) {
    cout<<"Generation number: "<<generationNo<<"\n";
    chromosome fittest = findFittest(Population);
    cout<<"Closest solution: "<<evalExpression(target, decodeBits(fittest.bits))<<"\n";
}

chromosome RouletteSelect(float totalFitness, chromosome* Population) {
    float roll = (float)((rand()/(float)RAND_MAX)*totalFitness);
    float temp = 0.0;
    chromosome chromo;
    for (int i=0 ; i<POP_SIZE ; i++) {
        temp += Population[i].fitness;
        if (temp >= roll) {
            return(Population[i]);
        }
    }
    return(chromo);
}

void Crossover(chromosome &parent1, chromosome &parent2) {
    if (rand()/(float)RAND_MAX < CROSSOVER_RATE) {
        for (int i=rand()%CHROMO_LENGTH ; i<CHROMO_LENGTH ; i++) {
            char temp = parent1.bits[i];
            parent1.bits[i] = parent2.bits[i];
            parent2.bits[i] = temp;
        }
    }
}

void Mutate(chromosome &chromo) {
    for (int i=0 ; i<CHROMO_LENGTH ; i++) {
        if (rand()/(float)RAND_MAX < MUTATION_RATE) {
            if (chromo.bits[i] == '0') {
                chromo.bits[i] = '1';
            } else {
                chromo.bits[i] = '0';
            }
        }
    }
}

int main() {
    srand(time(NULL));

    float target;
    cout<<"Enter target number - ";
    cin>>target;

    chromosome POPULATION[POP_SIZE];
    for (int i=0 ; i<POP_SIZE ; i++) {
        POPULATION[i].bits = getRandomBits(CHROMO_LENGTH);
        string decoded = decodeBits(POPULATION[i].bits);
    }

    int numberOfGenerationsRequired = 0;
    bool found = false;

    while (!found) {
        float totalFitness = 0.0;
        for (int i=0 ; i<POP_SIZE ; i++) {
            string decoded = decodeBits(POPULATION[i].bits);
            POPULATION[i].fitness = assignFitness(target, decoded);
            totalFitness += POPULATION[i].fitness;

            //Check for solution
            if (POPULATION[i].fitness == 1000.0) {
                cout<<"Solution found!\n";
                cout<<numberOfGenerationsRequired<<" generations.\n";
                cout<<decoded<<"\n";
                found = true;
                break;
            }
        }

        //Make next generation
        chromosome next_gen[POP_SIZE];
        for (int i=0 ; i<POP_SIZE ; i+=2) {
            chromosome parent1 = RouletteSelect(totalFitness, POPULATION);
            chromosome parent2 = RouletteSelect(totalFitness, POPULATION);
            Crossover(parent1, parent2);
            Mutate(parent1);
            Mutate(parent2);
            next_gen[i] = parent1;
            next_gen[i+1] = parent2;
        }
        Log(target, numberOfGenerationsRequired, POPULATION);
        numberOfGenerationsRequired++;

        //Copy next gen into main array
        for (int i=0 ; i<POP_SIZE ; i++) {
            POPULATION[i] = next_gen[i];
        }

        if (numberOfGenerationsRequired > MAX_GENERATIONS) {
            cout<<"No solutions found.\n";
            found = true;
        }
    }

    return 0;
}