#ifndef _HASICI_H
#define _HASICI_H
#include "simlib.h"
#include <time.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>

#define MINUTES_TO_FIRE 733
#define POCET_STANIC 5
#define X_MAX 63000
#define Y_MAX 63000

typedef struct Point {
	int X;
	int Y;
} Point;

void GenerovatStanice();
void DeleteStanice();
void SeraditStanice(Point pozar);
void OrderQueue();
double Vzdalenost(Point pozar, Point stanice);

class QueuePozar : public Queue {
public:
	bool IntensityOverflow();
};

class Stanice : public Facility {
public:
	Stanice(int x, int y, const char* name);	
	Point Poloha;
	Point Auto;
	double AutoStartTime;
	double AutoArrivalTime;
	int Vzdalenost;	
};

class Pozar : public Process {
public:
	int Intenzita;
	double StartTime;
	double ExtinguishTime;
	double EndTime;
	int Skoda;
	std::vector<Stanice*> Auta;
	Point Poloha;
	Pozar(int intenzita);	
	void Behavior();
	int VypocetSkod(int burnTime, int pocatecniIntenzita);
};

class Generator : public Event {
public:
	Generator();	
	void Behavior();
};

std::vector<Stanice*> stanice;
QueuePozar Q1;
QueuePozar Q2;
QueuePozar Q3;

#endif // _HASICI_H