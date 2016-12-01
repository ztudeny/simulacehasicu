#include "hasici.h"

/*Store Kotvy("Sklad kotev", 40);
Stanice stanice[POCET_STANIC];

Histogram dobaCesty("Doba stravena lyzarem u vleku", 0, 1, 15);
Histogram pocetPokusu("Pocet pokusu nastoupit", 1, 1, 10);

Stat	cekaniZavodniku("Doba cekani zavodniku");*/

void GenerovatStanice() {
	stanice.push_back(new Stanice(10000, 10000, "Stanice 1"));
	stanice.push_back(new Stanice(50000, 10000, "Stanice 2"));
	stanice.push_back(new Stanice(50000, 50000, "Stanice 3"));
	stanice.push_back(new Stanice(10000, 50000, "Stanice 4"));	
}

void DeleteStanice()
{
	Stanice *temp = NULL;
	while (!stanice.empty()) {
		temp = stanice.back();
		delete temp;
		stanice.pop_back();
	}
}

void SeraditStanice(Point pozar) {
	std::sort(stanice.begin(),
		stanice.end(),
		[pozar](Stanice* lhs, Stanice* rhs){ return Vzdalenost(pozar, lhs->Poloha) < Vzdalenost(pozar, rhs->Poloha); });
}

double Vzdalenost(Point pozar, Point stanice) {	
	double dist = pow(pozar.X - stanice.X,2)+pow(pozar.Y - stanice.Y,2);
	dist = sqrt(dist);
    return dist;
}

void OrderQueue() {
	while (Q1.IntensityOverflow())
	{
		Q1.GetFirst()->Into(Q2);
	}
	while (Q2.IntensityOverflow())
	{
		Q2.GetFirst()->Into(Q3);
	}
}

/* Metody definujici chovani tridy Stanice */

Stanice::Stanice(int x, int y, const char* name) : Facility(name) {
	Poloha.X = x;
	Poloha.Y = y;
}

bool QueuePozar::IntensityOverflow() {
	return (Length() > 0 && ((Pozar*)(*begin()))->StartTime - Time >= 60) ? true : false;		
}

Pozar::Pozar(int intenzita) : Process(intenzita) {
	Intenzita = intenzita;
}

void Pozar::Behavior() {
	StartTime = Time;	
	Poloha.X = Random()*X_MAX;
	Poloha.Y = Random()*Y_MAX;
	Stanice *current = NULL;
	zpet:
	SeraditStanice(Poloha);
	for (int i = 0; i < stanice.size(); i++) {
		if (!stanice[i]->Busy()) {
			current = stanice[i];
			break;
		}
	}
	if (current == NULL) {
		switch (Intenzita) {
			case 0 ... 199:
				Into(Q1);
				break;
			case 200 ... 299:
				Into(Q2);
				break;			
			default:				
				Into(Q3);
				break;
		}
		Passivate();
		goto zpet;
	}		
	Seize(*current);
	current->Auto = Poloha;
	double vzdalenost = Vzdalenost(Poloha, current->Poloha);
	// TODO - výpočet času, za který auto dojede
	if (Intenzita < 200)
		Wait(Exponential(120));
	else if (Intenzita < 300)
		Wait(Exponential(320));
	else
		Wait(Exponential(500));
	// TODO - zajisteni dynamickeho pocitani skod, proces haseni

	Release(*current);
	OrderQueue();
	// konec haseni, primarne vytahnout pozar s intenzitou 3
	if (Q3.Length()>0)
		(Q3.GetFirst())->Activate();
	else if (Q2.Length()>0)
		(Q2.GetFirst())->Activate();
	else if (Q1.Length()>0)
		(Q1.GetFirst())->Activate();
	
}


Generator::Generator() : Event() {
}
	
void Generator::Behavior() {
	double k = Random();
	// TODO - vypocet intenzity, dosazeni spravnych hodnot pravdepodobnostniho rozlozeni
	if (k < 0.083)
		(new Pozar(300 + Random()*99))->Activate();		
	else if (k < 0.363)		
		(new Pozar(200 + Random()*99))->Activate();
	else
		(new Pozar(100 + Random()*99))->Activate();
	Activate(Time+Exponential(MINUTES_TO_FIRE));
}

int	main()
{
	GenerovatStanice();
	SetOutput("pozar.dat");
	// 1 rok v minutach
	Init(0, 525600);
	RandomSeed(time(NULL));
	(new Generator())->Activate();
	Run();	
	stanice[0]->Output();
	stanice[1]->Output();
	stanice[2]->Output();
	stanice[3]->Output();
	Q1.Output();
	DeleteStanice();
}