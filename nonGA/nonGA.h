//******************************************************************************
// Team 7: Adam Guy, Nabir Dinani, Jonathan Kocmoud, Nicholas Warner
// Date			: 26 March 2015
// Subject		: CSCE 315-504
// Assignment	: Project 3
// Description	: Finds a circuit using Non-GA, which corresponds to the output
//******************************************************************************

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <math.h> 
#include <bitset>

//including header file to display the program graphically 
#include "FLTK_Hist.h"
using namespace std;
 
//struct for a gate, with required functionalities     
struct Gate
{
	int outputLine;
	string gate;		// NONE, NOT, AND, OR
	int x;				// first input for not, and, or and none
	int y;				// second input for and, or | -1 for NONE or NOT gate
	bitset<8> output;// the outputs starting from eg. 000, 001, 010, ... 111
};

//struct for a circuit, with required functionalities     
struct Circuit
{
	int id;
	int numNotGates;
	vector<Gate> gates;
};

//function that checks if string is a number
bool checkStringIsNumber(string value) {
    for (int i = 0; i < value.length(); ++i) {
        if(!isdigit(value[i])) {
            return false;
        }
    }
    return true;
}

//function to get the circuit output(in binary)
bitset<8> getCircuitOutput(Circuit circuit) {
	return circuit.gates[circuit.gates.size()-1].output;
}

//function that just prints the circuit 
void printCircuit(Circuit c) {
	//cout << "CIRCUIT ID: " << c.id << "\n";
	for ( int i = 0; i < c.gates.size(); ++i ) {
		if (i != 0) {
			cout << "\n";
		}
		if( c.gates[ i ].gate == "NONE" ) { //print out all the none gates 
			cout << c.gates[ i ].outputLine << " " << c.gates[ i ].gate << " " << c.gates[ i ].x;
			//cout << "\t" << c.gates[ i ].output;
			//cout << "\n";
		}
		else if ( c.gates[ i ].gate == "NOT" ) { //prints out all the not gates
			cout << c.gates[ i ].outputLine << " " << c.gates[ i ].gate << " " << c.gates[ i ].x;
			//cout << "\t" << c.gates[ i ].output;
			//cout << "\n";
		}
		else if ( c.gates[ i ].gate == "AND" ) { //prints out all the and gates
			cout << c.gates[ i ].outputLine << " " << c.gates[ i ].gate << " " << c.gates[ i ].x << " " << c.gates[ i ].y;
			//cout << "\t" << c.gates[ i ].output;
			//cout << "\n";
		}
		else if ( c.gates[ i ].gate == "OR" ) { //prints out all the or gatess
			cout << c.gates[ i ].outputLine << " " << c.gates[ i ].gate << " " << c.gates[ i ].x << " " << c.gates[ i ].y;
			//cout << "\t" << c.gates[ i ].output;
			//cout << "\n";
		}
		else {
			cout << "Error in circuit read\n";
			break;
		}	
	}
	//cout << "\n";
}

//Returns whether or not output is a correct output
//  if multiple correct, then deletes found one
//  returns true, when all outputs have been found
bool isCorrectOutput( bitset<8> output, vector < bitset<8> >& correctOutputs ) {
	//For each unique output
	for (int j = 0; j < correctOutputs.size(); ++j) {
		//If complete solution, remove Correct Output
		if ( output == correctOutputs[j] ) {
			correctOutputs.erase(correctOutputs.begin()+j);
			//cout << "Found an output! Remaining: " << correctOutputs.size() << "\n";
			//If no more solutions are needed
			if ( correctOutputs.size() == 0 ) {
				return true;
			}
		}
	}
	return false;
}

//Returns whether or not output is unique
//   Based on the vector of outputs
bool isUniqueOutput( bitset<8> output, vector<bitset<8>> uniqueOutputs ) {
	//For each unique output
	for (int j = 0; j < uniqueOutputs.size(); ++j) {
		//if not unique
		if ( output == uniqueOutputs[j] ) {
			return false;
		}
	}
	return true;
}

//function to graphically display the circuit 
void graphCircuit(Circuit circuit) {
	int orGates = 0;
	int andGates = 0;
	int notGates = 0;
	for (int i = circuit.gates.size()-1; i >= 0; --i) {
		Gate gate = circuit.gates[i];
		if (gate.gate == "AND") {
			++andGates;
		}
		else if (gate.gate == "OR") {
			++orGates;
		}
		else if (gate.gate == "NOT") {
			++notGates;
		}
	}

	int num = andGates;
	write(pipefd, &num, sizeof(num)); //writing to pipe which sends values to graphical output process
	//cout << "Writing " << num <<endl;
	num = orGates;
	write(pipefd, &num, sizeof(num)); //writing to pipe which sends values to graphical output process
	num = notGates;
	write(pipefd, &num, sizeof(num)); //writing to pipe which sends values to graphical output process
}

//Finds the possible solutions by adding
//   multiple AND and OR gates to find
//   all possible unique solutions
bool findPossibleOutputs(Circuit& circuit, vector < bitset<8> > correctOutputs, vector<bitset<8>> uniqueOutputs, int numNotGates = 0) {
	//Add And, Or, and Not gates
	int andsChecked = 0;
	int orsChecked = 0;
	while( true ) {
		if (circuit.gates.size() % 10 == 0) {
			graphCircuit(circuit);
		}
		bool uniqueOutput = false;
		//First try to add AND gate
		for (int i = circuit.gates.size()-1; i >= andsChecked && !uniqueOutput; --i) {
			for (int j = 0; j < i-1 && !uniqueOutput; ++j) {
				//Try to create a circuit and add a AND gate
				Circuit cir = circuit;
				Gate andGate;
				andGate.outputLine = circuit.gates.size()+1;
				andGate.gate = "AND";
				andGate.x = i+1;
				andGate.y = j+1;
				//Calculate the new output
				bitset<8> output;
				bitset<8> prevOutput = circuit.gates[i].output;
				bitset<8> prevOutput2 = circuit.gates[j].output;
				output = prevOutput & prevOutput2;
				//Check to see if output is unique
				//   If so, then set as new circuit
				if ( isUniqueOutput(output, uniqueOutputs) ) {
					//Actually add the AND gate
					andGate.output = output;
					cir.gates.push_back(andGate);
					//set as new circuit
					circuit = cir;
					if ( isCorrectOutput( output, correctOutputs ) ) {
						//cout << "found with AND gate\n";
						return true;
					}
					uniqueOutputs.push_back( output );
					uniqueOutput = true;
				}
			}
		}
		//Increase so we don't look every time
		if ( !uniqueOutput ) {
			andsChecked = circuit.gates.size()-2;//we just added a gate
		}
		//If cannot add an AND gate, try an OR gate
		for (int i = circuit.gates.size()-1; i >= orsChecked && !uniqueOutput; --i) {
			for (int j = 0; j < i-1 && !uniqueOutput; ++j) {
				//Try to create a circuit and add a OR gate
				Circuit cir = circuit;
				Gate orGate;
				orGate.outputLine = circuit.gates.size()+1;
				orGate.gate = "OR";
				orGate.x = i+1;
				orGate.y = j+1;
				//Calculate the new output
				bitset<8> output;
				bitset<8> prevOutput = circuit.gates[i].output;
				bitset<8> prevOutput2 = circuit.gates[j].output;
				output = prevOutput | prevOutput2;

				//Check to see if output is unique
				//   If so, then set as new circuit
				if ( isUniqueOutput(output, uniqueOutputs) ) {
					//Actually add the OR gate
					orGate.output = output;
					cir.gates.push_back(orGate);
					//set as new circuit
					circuit = cir;
					if ( isCorrectOutput( output, correctOutputs ) ) {
						//cout << "found with OR gate\n";
						return true;
					}
					uniqueOutputs.push_back( output );
					uniqueOutput = true;
				}
			}
		}
		//checks if the output is unique
		if ( !uniqueOutput ) {
			orsChecked = circuit.gates.size()-2;
		}
		//Lastly, try to add a NOT gate
		//   And create multiple versions that run
		//   Until they find a solution
		for (int i = circuit.gates.size()-1; i >= 0 && !uniqueOutput && numNotGates < 2; --i) {
			//Try to create a circuit and add a NOT gate
			Circuit cir = circuit;
			Gate notGate;
			notGate.outputLine = circuit.gates.size()+1;
			notGate.gate = "NOT";
			notGate.x = i+1;
			notGate.y = -1;
			//Calculate the new output
			bitset<8> output;
			bitset<8> prevOutput = circuit.gates[i].output;
			output = ~(prevOutput);

			//Check to see if output is unique
			//   If so, then set as new circuit
			if ( isUniqueOutput(output, uniqueOutputs) ) {
				//Actually add the NOT gate
				notGate.output = output;
				cir.gates.push_back(notGate);
				vector < bitset<8> > myCorrectOutputs = correctOutputs;
				if ( isCorrectOutput( output, myCorrectOutputs ) ) {
					//cout << "found with NOT gate\n";
					circuit = cir;
					return true;
				}
				vector<bitset<8>> myUniqueOutputs = uniqueOutputs;
				myUniqueOutputs.push_back( output );
				//set as new circuit
				bool found = findPossibleOutputs(cir, myCorrectOutputs, myUniqueOutputs, numNotGates+1);
				if ( found ) {
					//cout << "FOUND!\n";
					circuit = cir;
					return true;
				}
			}
		}
		//If unable to add gate for unique output
		if ( !uniqueOutput ) {
			return false;
		}
	}

	return false;
}

//Assigns outputs to initial NONE gates
Circuit startPossibleOutputs(Circuit circuit, vector < bitset<8> > correctOutputs) {
	//Circuit will have A, B, ... N inputs
	vector<bitset<8>> uniqueOutputs;
	//Find and Add blank output lines
	int numStarterGates = circuit.gates.size();
	//For each gate
	for (int i = 0; i < numStarterGates; ++i) {
		bitset<8> outputOrig = circuit.gates[i].output;
		if ( isCorrectOutput( outputOrig, correctOutputs ) ) {
			//cout << "found with first gate\n";
			return circuit;
		}
		uniqueOutputs.push_back(outputOrig);
	}
	Circuit cir = circuit;
	bool found = findPossibleOutputs(cir, correctOutputs, uniqueOutputs);
	//checks if the circuit is found 
	if ( found ) {
		return cir;
	}
	//cout << "Was not found!\n";
	return circuit;
}