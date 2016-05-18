//******************************************************************************
// Team 7: Adam Guy, Nabir Dinani, Jonathan Kocmoud, Nicholas Warner
// Date			: 26 March 2015
// Subject		: CSCE 315-504
// Assignment	: Project 3
// Description	: Finds a circuit using Non-GA, which corresponds to the output
//******************************************************************************

#include "nonGA.h"

int main ( int argc, char const *argv[] )
{
	int numInputs = 3; //INPUT NUMBER!
	vector < bitset<8> > correctOutputs;
	bitset<8> correctOutput;
	bitset<8> correctOutput2;
	bitset<8> correctOutput3;
    numInputs = 3; //INPUT NUMBER!
    //one-bit full added
	//Carry Out
	correctOutput = bitset<8>(string("00010111"));
	correctOutput2 = bitset<8>(string("01101001"));
	correctOutputs.push_back(correctOutput);
	correctOutputs.push_back(correctOutput2);
	vector<Circuit> cBox;
	Circuit X;
	X.id = 1;
	double numInputRows = pow(2, numInputs);
	double sizeOfGroups = numInputRows/2;
	for (int i = 0; i < numInputs; ++i) {
		Gate noneGate;
		noneGate.outputLine = i+1;
		noneGate.gate = "NONE";
		noneGate.x = i;
		noneGate.y = -1;
		bitset< 8 > output;
		int j = 0;
		bool inputValue = true;
		//repeat for 2^numOfInputs
		while (j < numInputRows) {
			for (int k = 0; k < sizeOfGroups && j < numInputRows; ++k) {
				output.set(j, inputValue);
				++j;
			}
			inputValue = !inputValue;
		}
		sizeOfGroups /= 2;
		noneGate.output = output;
		X.gates.push_back( noneGate );
	}
	// create pipe to send values to graphical output process
	int pfd[2];
	if (pipe(pfd) == -1){
		cout << "Failure creating pipe" <<endl;
		exit(EXIT_FAILURE);
	}
	// fork graphical output process
	pid_t childpid = fork();
	if (childpid < 0){
		cout << "FORK() FAILED";
		exit(EXIT_FAILURE);
	}
	else if (childpid == 0) { // child process
		close(pfd[1]); // close unused write pipe (child only reads)
		pipefd = pfd[0];
		graph();
		close(pfd[0]);
	}
	else {  // parent process
		close(pfd[0]); // close unused read end of pipe
		pipefd = pfd[1];
		Circuit solutionCircuit = startPossibleOutputs(X, correctOutputs);
		printCircuit(solutionCircuit);
		close(pfd[1]);
		wait(NULL);  // wait for child (child ends when Xming window is closed
	}
	return 0;
}