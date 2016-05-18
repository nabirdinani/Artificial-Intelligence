//*****************************************************************************
// Team 7: Adam Guy, Nabir Dinani, Jonathan Kocmoud, Nicholas Warner 
// Date : 26 March 2015
// Subject : CSCE 315-504
// Assignment : Project 3
// Description : Graphically displays Non-GA circuit search progress
//*****************************************************************************
/*
compile with: g++-4.7 $(fltk-config --use-images --compile) -std=c++11 -o 
BFS bruteForceSearch.cpp FLTK_Hist.h -lfltk -lX11
*/

//Headers for graphical part using FLTK
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

//Headers for accessing necessary methods 
#include <deque>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for fork, pipe, read, write
#include <sys/wait.h> // for wait() sys call
#include <time.h>

using namespace std;

#define BG_COLOR FL_WHITE
#define OR_COLOR FL_GREEN
#define AND_COLOR FL_DARK_MAGENTA
#define NOT_COLOR FL_RED
#define NONE_COLOR FL_BLACK
static int AND_GATES = 0;
static int OR_GATES = 0;
static int NOT_GATES = 0;
static int pipefd; // read file descripter


//histogram class that displays the historgram 
class Histo : public Fl_Box {
    
	//overloading the virtual draw function, and making it do what we want 
    void draw() {
        
        // TELL BASE WIDGET TO DRAW ITS BACKGROUND
        Fl_Box::draw();
		
		int barWidth = 15,
			yAxis = (int) (x() + 50),
			xAxis = (int) (y() + 80),
			barSpacing = 20,
			barMaxH = 500;

		static deque<long> TOTgates, ANDgates, ORgates, NOTgates; //deque to store all the gates 
		long mostGates = 0;
			
		// update gate deques
		TOTgates.push_back(AND_GATES + OR_GATES + NOT_GATES + 2); // add the starting 2 NONE gates
		ANDgates.push_back(AND_GATES); 
		ORgates.push_back(OR_GATES);
		NOTgates.push_back(NOT_GATES); 
		
		// track only the most recent 25 circuits
		if (ANDgates.size() > 25) {
			TOTgates.pop_front();
			ANDgates.pop_front();
			ORgates.pop_front();
			NOTgates.pop_front();
		}

		// display key
		fl_font(FL_HELVETICA, 10);
		fl_color(AND_COLOR);
		fl_draw("AND gates", yAxis, xAxis-50, 200, 15, FL_ALIGN_TOP);
		fl_color(OR_COLOR);
		fl_draw("OR gates", yAxis+100, xAxis-50, 200, 15, FL_ALIGN_TOP);
		fl_color(NOT_COLOR);
		fl_draw("NOT gates", yAxis+200, xAxis-50, 200, 15, FL_ALIGN_TOP);
		fl_color(NONE_COLOR);
		fl_draw("NONE gates", yAxis+300, xAxis-50, 200, 15, FL_ALIGN_TOP);

		// label axises
		fl_color(NONE_COLOR);
		fl_font(FL_HELVETICA, 26);
		fl_draw("CIRCUIT ITERATIONS", yAxis+50, xAxis-30, 200, 15, FL_ALIGN_TOP);
		fl_draw("N U M B E R o f G A T E S", yAxis-30, xAxis+90, 3, 200, FL_ALIGN_WRAP);

		// find largest number of TOTAL gates and graph NONE gates
		for (int i = 0; i < TOTgates.size(); i++) {
			if (TOTgates[i] > mostGates) mostGates = TOTgates[i];
			fl_rectf(yAxis + (barSpacing*i), xAxis, barWidth, 2*barMaxH/mostGates);
		}
		
		// add AND portion of the histogram
		fl_color(AND_COLOR);
		for (int i = 0; i < ANDgates.size(); i++){
			fl_rectf(yAxis + (barSpacing*i), xAxis + (2*barMaxH/mostGates), barWidth, ANDgates[i]*barMaxH/mostGates);
		}

		// add OR bars to the graph
		fl_color(OR_COLOR);
		for (int i = 0; i < ORgates.size(); i++){
			fl_rectf(yAxis + (barSpacing*i), xAxis + (barMaxH *(2+ANDgates[i])/mostGates), barWidth, ORgates[i]*barMaxH/mostGates);
		}

		// add NOT bars to the graph
		fl_color(NOT_COLOR);
		for (int i = 0; i < NOTgates.size(); i++){
			fl_rectf(yAxis + (barSpacing*i), xAxis + (barMaxH *(2+ANDgates[i]+ORgates[i])/mostGates), barWidth, NOTgates[i]*barMaxH/mostGates);
		}
    }

    static void Timer_CB(void *userdata) {
        
        static int readCheck, loopStop=0;
		Histo *o = (Histo*)userdata;

		// read new data in from pipes
		readCheck = 0; // read returns 0 if not data is read
	    readCheck += read(pipefd, &AND_GATES, sizeof(AND_GATES));
		readCheck += read(pipefd, &OR_GATES, sizeof(OR_GATES));
		readCheck += read(pipefd, &NOT_GATES, sizeof(NOT_GATES));
			
		// prevent endless looping
		if ((AND_GATES == 0 && OR_GATES == 0 && NOT_GATES ==0) ||
			(readCheck == 0)){ 
			loopStop++;
		}
		// stop calling timer (stop looping)
		if (loopStop > 2){
			Fl::remove_timeout(Timer_CB, userdata);
		}
		else{
			o->redraw(); //calling the redraw function
			Fl::repeat_timeout(0.01, Timer_CB, userdata); // call Timer_CB again
		}
    }
	
public:
    // CONSTRUCTOR
    Histo(int X,int Y,int W,int H,const char*L=0) : Fl_Box(X,Y,W,H,L) {
        box(FL_FLAT_BOX);
        color(BG_COLOR);
		Fl::add_timeout(0.01, Timer_CB, (void*)this); // call to redraw 
    }
};

Histo *histo;

int graph() {
	 
	 Fl_Double_Window win(600, 625);
     Histo hist(10, 10, win.w()-20, win.h()-20);
	 histo = &hist;
     win.show();
	
     return (Fl::run());
}
