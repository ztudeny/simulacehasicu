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
	stanice.push_back(new Stanice(0, 0, "Stanice 5"));
	stanice.push_back(new Stanice(60000, 60000, "Stanice 6"));
	stanice.push_back(new Stanice(0, 60000, "Stanice 7"));
	stanice.push_back(new Stanice(60000, 0, "Stanice 8"));
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
	if (Length() < 1)
		return false;
	Pozar *temp = ((Pozar*)(*begin()));
	if (temp == NULL)
		return false;

	// Cas horeni je aktualni cas minus cas vzniku pozaru
	int burnTime = (int)(Time - temp->StartTime);
	// Uchovani puvodni intenzity
	int intenzita = temp->Intenzita;
	// 1 bod intenzity == 1 minuta
	temp->Intenzita += burnTime;
	temp->Skoda += temp->VypocetSkod(burnTime, intenzita);
	// Vypocitana dosavadni skoda, je tedy treba nastavit aktualni cas pro pripadny dalsi vypocet
	temp->StartTime = Time;
	// Pokud byl cas horeni nad 100 minut (nebo se intenzita prehoupla do dalsi stovky), z pozaru se stane o stupen horsi pozar
	if (burnTime >= 100 || (temp->Intenzita % 100) <= (intenzita % 100)) {
		// Priorita je doplnek stovky (0 je nejvetsi, 100 nejmensi priorita) modula aktualni intenzity
		temp->Priority = 100 - (temp->Intenzita % 100);
		return true;
	}
	// Pokud pozar zustava ve stejne kategorii intenzity, snizi se priorita
	else {
		temp->Priority -= burnTime;
		return false;
	}
		
}

int Pozar::VypocetSkod(int burnTime, int pocatecniIntenzita) {
	int skoda = 0;
	int autoCount = 0;	
	do {

		// Usek, pro ktery se pocitaji skody. Pokud je to usek, ve kterem se prehoupne intenzita do vyssi kategorie, usek se zkrati presne na hranici prechodu do vyssi kategorie.
		int temp = (burnTime >= 100 || (Intenzita % 100) <= (pocatecniIntenzita % 100)) ? (100 - (pocatecniIntenzita % 100)) : (Intenzita - pocatecniIntenzita);
		std::cout << "----------------------------------------------------\n";
		std::cout << "aktualni cas: " << Time << " StartTime: " << StartTime << " temp: " << temp << " pocatecniIntenzita: " << pocatecniIntenzita << " Intenzita: " << Intenzita << "\n";

		for (int i = 0; i < Auta.size(); i++) {
			std::cout << "je tu auto - AutoStartTime: " << Auta[i]->AutoStartTime << " AutoArrivalTime: " << Auta[i]->AutoArrivalTime << "\n";

			// Pokud je cas prijezdu auta vetsi, nez pocatecni cas pozaru, zasahuje do vypoctu. Pokud je mensi nebo roven, auto uz pro pocitany usek dorazilo
			if (Auta[i]->AutoStartTime + Auta[i]->AutoArrivalTime > StartTime) {				
				// Pokud auto dorazi v prubehu pocitaneho useku, je tento usek treba zkratit na cas prijezdu auta, aby se mohly vypocitat skody pro usek, kdy auto jeste nedorazilo
				if (Auta[i]->AutoStartTime + Auta[i]->AutoArrivalTime < StartTime + temp) {
					std::cout << "je treba snizit temp na " << (int)Auta[i]->AutoArrivalTime << std::endl;
					temp = (int)Auta[i]->AutoArrivalTime;
				}
			}
			else {
				autoCount++;
				std::cout << "pocita se s " << autoCount << " auty\n";
			}
				
		}
		std::cout << "auta vyresena\n";
		// Prumer intenzity, se kterou pocitany usek horel
		double average = (pocatecniIntenzita + (pocatecniIntenzita + temp))/2;
		// Koeficient, ktery udava rust skod
		double koefi = 0;
		// Pokud jsou u pozaru jiz vsechna potrebna auta
		if (pocatecniIntenzita - (autoCount * 100) <= 100)
			koefi = 1;
		// Pokud je u pozaru o jedno auto mene
		else if (pocatecniIntenzita - (autoCount * 100) <= 200)
			koefi = 4;
		// Pokud je u pozaru o dve auta mene
		else
			koefi = 16;
		// Skoda += prumerna intenzita v danem useku * koeficient * cas v tomto vpyocetnim useku
		skoda += average*koefi*temp;
		// Posun pocatecni intenzity o vypocteny usek - chceme pocitat dalsi usek
		pocatecniIntenzita += temp;
		// Posun casu pozaru o vypocteny usek - aktualni je jiz vypocteny, StartTime se tedy posune na dalsi nevypocitany usek
		StartTime += temp;
		burnTime -= temp;
	// Cyklus pokracuje, pokud jeste nejsou vypocteny skody od zacatku pozaru (resp. posledniho vypocitaneho useku) az po aktualni stav pozaru
	} while (pocatecniIntenzita != Intenzita);
	return skoda;
}

Pozar::Pozar(int intenzita) : Process(intenzita) {
	Intenzita = intenzita;
	Priority = 100 - (intenzita % 100);
}

void Pozar::Behavior() {
	std::cout << "NOVY POZAR, INTENZITA " << Intenzita << std::endl; 
	StartTime = Time;
	int waitTime = 0;
	Poloha.X = Random()*X_MAX;
	Poloha.Y = Random()*Y_MAX;
	int auta = 0;	
	zpet:
	Stanice *current = NULL;
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
		std::cout << "Proces aktivovan - Intenzita " << Intenzita << ", cas " << Time << std::endl;
		goto zpet;
	}		
	Seize(*current);
	current->AutoStartTime = Time;
	current->Auto = Poloha;
	// 80 km/h == 1333 m/min
	current->AutoArrivalTime = Vzdalenost(Poloha, current->Poloha)/1333;
	Auta.push_back(current);
	std::cout << "Intenzita " << Intenzita << " sila aut " << Auta.size()*100 << "\n";
	if (Intenzita - (Auta.size()*100) >= 100)
		goto zpet;	
	waitTime = (current->AutoArrivalTime + current->AutoStartTime) - Time;
	if (current->AutoArrivalTime + current->AutoStartTime > Time)
	{
		std::cout << "Je treba pockat na auto...\n";
		Wait(waitTime);
		Intenzita += waitTime;
	}
	Skoda += VypocetSkod(waitTime, Intenzita - waitTime);
	// TODO - výpočet času, za který auto dojede
	std::cout << "HASIM, cas " << Time << " skoda " << Skoda << "\n";
	if (Intenzita < 200)
		Wait(Exponential(120));
	else if (Intenzita < 300)
		Wait(Exponential(320));
	else
		Wait(Exponential(500));
	// TODO - zajisteni dynamickeho pocitani skod, proces haseni
	for (int i = 0; i < Auta.size(); i++) {
		Release(*Auta[i]);
		Auta[i]->AutoStartTime = 0;
		Auta[i]->AutoArrivalTime = 0;
	}
	//Release(*current);
	
	OrderQueue();
	// konec haseni, primarne vytahnout pozar s intenzitou 3
	if (Q3.Length()>0) {
		std::cout << "Q3 length " << Q3.Length() << "\n";
		(Q3.GetFirst())->Activate();
	}		
	else if (Q2.Length()>0) {
		std::cout << "Q2 length " << Q3.Length() << "\n";
		(Q2.GetFirst())->Activate();
	}		
	else if (Q1.Length()>0) {
		std::cout << "Q1 length " << Q3.Length() << "\n";
		(Q1.GetFirst())->Activate();

	}
		
	else
		std::cout << "NIC VE FRONTE\n";
	
}


Generator::Generator() : Event() {
}
	
void Generator::Behavior() {
	double k = Random();
	// TODO - vypocet intenzity, dosazeni spravnych hodnot pravdepodobnostniho rozlozeni
	if (k < 0.083)
		(new Pozar(300 + Random()*50))->Activate();		
	else if (k < 0.363)		
		(new Pozar(200 + Random()*50))->Activate();
	else
		(new Pozar(100 + Random()*50))->Activate();
	Activate(Time+Exponential(MINUTES_TO_FIRE));
}

int	main()
{
	GenerovatStanice();
	SetOutput("pozar.dat");
	// 1 rok v minutach
	Init(0, 3000);
	RandomSeed(time(NULL));
	(new Generator())->Activate();
	Run();	
	stanice[0]->Output();
	stanice[1]->Output();
	stanice[2]->Output();
	stanice[3]->Output();
	stanice[4]->Output();
	stanice[5]->Output();
	stanice[6]->Output();
	stanice[7]->Output();
	Q1.Output();
	Q2.Output();
	Q3.Output();
	DeleteStanice();
}