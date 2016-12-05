#include "hasici.h"

/*Store Kotvy("Sklad kotev", 40);
Stanice stanice[POCET_STANIC];

Histogram dobaCesty("Doba stravena lyzarem u vleku", 0, 1, 15);
Histogram pocetPokusu("Pocet pokusu nastoupit", 1, 1, 10);

Stat	cekaniZavodniku("Doba cekani zavodniku");*/

Histogram skody("Skody", 0, 100000, 100);
Histogram histintenzita("Intenzita", 0, 10, 80);


void GenerovatStanice() {
	stanice.push_back(new Stanice(11250, 5625, "Morkovice-Slížany"));
	stanice.push_back(new Stanice(11250, 22500, "Uherské Hradiště"));
	stanice.push_back(new Stanice(11250, 33750, "Uherský Brod"));
	stanice.push_back(new Stanice(22500, 16875, "Otrokovice"));	
	stanice.push_back(new Stanice(33750, 5625, "Kroměříž"));
	stanice.push_back(new Stanice(45000, 11250, "Holešov"));
	stanice.push_back(new Stanice(56250, 3375, "Bystřice pod Hostýnem"));
	stanice.push_back(new Stanice(73125, 11250, "Valašské Meziříčí"));
	stanice.push_back(new Stanice(39375, 22500, "Zlín"));
	stanice.push_back(new Stanice(33750, 33750, "Luhačovice"));
	stanice.push_back(new Stanice(45000, 39375, "Slavičín"));
	stanice.push_back(new Stanice(56250, 39375, "Valašské Klobouky"));
	stanice.push_back(new Stanice(67500, 33750, "Vsetín"));
	//stanice.push_back(new Stanice(78750, 22500, "Rožnov pod Radhoštěm"));
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

/* Funkce, ktera se zavola po uvolneni jednoho auta od pozaru. Probudi nejprioritnejsi ohen, ktery si tak auto muze vzit pro sebe */
void UvolneniAuta() {
	// Serazeni ohnu dle priority ve frontw
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
}

Point GenerovatOhen()
{
	int x = 1000 * Random();
	Point pozice;
	switch (x) {
		//Luhacovice - 2,9 %
		case 0 ... 29:
			pozice.X = Uniform(28125,45000);
			pozice.Y = Uniform(28125,45000);
			break;
		//Valasske klobouky - 3,15 %
		case 30 ... 61:
			pozice.X = Uniform(45000,67500);
			pozice.Y = Uniform(33750,45000);
			break;
		//Bystrice pod Hostynem - 2,8 %
		case 62 ... 90:
			pozice.X = Uniform(50625,67500);
			pozice.Y = Uniform(0,11250);
			break;
		//Holesov - 3 %
		case 91 ... 121:
			pozice.X = Uniform(41625,50625);
			pozice.Y = Uniform(0,15750);
			break;
		//Vizovice - 2,8 %
		case 122 ... 150:
			pozice.X = Uniform(50625,67500);
			pozice.Y = Uniform(16875,28125);
			break;
		//Roznov pod Radhostem - 5,9 %
		case 151 ... 210:
			pozice.X = Uniform(67500,90000);
			pozice.Y = Uniform(16875,28125);
			break;
		//Valasske Mezirici - 7,68 %
		case 211 ... 288:
			pozice.X = Uniform(67500,90000);
			pozice.Y = Uniform(0,16875);
			break;
		//Otrokovice - 7,57 %
		case 289 ... 365:
			pozice.X = Uniform(16875,33750);
			pozice.Y = Uniform(11250,22500);
			break;
		//Uhersky Brod - 9,2 %
		case 366 ... 458:
			pozice.X = Uniform(0,22500);
			pozice.Y = Uniform(28125,45000);
			break;
		//Kromeriz - 13,05 %
		case 459 ... 589:
			pozice.X = Uniform(0,39375);
			pozice.Y = Uniform(0,14060);
			break;
		//Vsetin - 10,7 %
		case 590 ... 697:
			pozice.X = Uniform(61875,90000);
			pozice.Y = Uniform(28125,45000);
			break;
		//Uherske Hradiste - 15,15 %
		case 698 ... 849:
			pozice.X = Uniform(0,16875);
			pozice.Y = Uniform(16875,28125);
			break;
		//Zlin - 15,2 %
		default:
			pozice.X = Uniform(37500,50625);
			pozice.Y = Uniform(16875,28125);
			break;
	}
	return pozice;
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
	while (Q3.IntensityOverflow());
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
	if (round(temp->StartTime) == round(Time)) {
		std::cout << "Tento fajr je jiz vyresen.\n";
		return false;
	}

	// Cas horeni je aktualni cas minus cas vzniku pozaru
	double burnTime = (Time - temp->StartTime);
	// Uchovani puvodni intenzity
	double intenzita = temp->Intenzita;
	// 1 bod intenzity == 1 minuta
	//temp->Intenzita += burnTime;
	std::cout << "cekal pozar ve fronte, zacal v case " << temp->StartTime << ", ted je " << Time << ", puvodni intenzita " << intenzita << ", aktualni intenzita " << temp->Intenzita << ", burnTime " << burnTime << std::endl;
	temp->Skoda += temp->VypocetSkod(burnTime, intenzita);
	// Vypocitana dosavadni skoda, je tedy treba nastavit aktualni cas pro pripadny dalsi vypocet
	//temp->StartTime = Time;
	// Pokud byl cas horeni nad 100 minut (nebo se intenzita prehoupla do dalsi stovky), z pozaru se stane o stupen horsi pozar
	if ((burnTime >= 100 || fmod(temp->Intenzita, 100)) <= (fmod(intenzita, 100))) {
		// Priorita je doplnek stovky (0 je nejvetsi, 100 nejmensi priorita) modula aktualni intenzity
		temp->Priority = 100 - fmod(temp->Intenzita, 100);
		return true;
	}
	// Pokud pozar zustava ve stejne kategorii intenzity, snizi se priorita
	else {
		temp->Priority -= burnTime;
		return false;
	}
		
}

// Vypocita skody a intenzitu za urcitou dobu horeni (burnTime). Rozdeluje dobu horeni na useky oddelene prijezdy hasicskych aut.
int Pozar::VypocetSkod(double burnTime, double pocatecniIntenzita) {
	int skoda = 0;
	// Narust intenzity - defaultne 1
	double intenzitaGrow = 1;
	do {
		// Pocet pritomnych aut pri danem useku
		int autoCount = 0;
		// Usek, pro ktery se pocitaji skody. Pokud je to usek, ve kterem se potencialne prehoupne intenzita do vyssi kategorie, usek se zkrati presne na hranici prechodu do vyssi kategorie.
		double temp = ((burnTime) >= 100 || fmod(pocatecniIntenzita + (burnTime), 100) <= fmod(pocatecniIntenzita,100)) ? (100 - (fmod(pocatecniIntenzita, 100)))/intenzitaGrow : (burnTime);
		std::cout << "----------------------------------------------------\n";
		//std::cout << "temp == " "if " << burnTime << " >= 100 || " << fmod(pocatecniIntenzita + (burnTime), 100) << " <= " << fmod(pocatecniIntenzita,100) << " then temp = " << (100 - fmod(pocatecniIntenzita, 100))/intenzitaGrow << " else " << burnTime << std::endl;
		std::cout << "aktualni cas: " << Time << " StartTime: " << StartTime << " usek: " << temp << " pocatecniIntenzita: " << pocatecniIntenzita << " burnTime " << burnTime << "\n";

		for (int i = 0; i < Auta.size(); i++) {
			std::cout << "auto ktere prijelo/jede - AutoStartTime: " << Auta[i]->AutoStartTime << " AutoArrivalTime: " << Auta[i]->AutoArrivalTime << "\n";

			// Pokud je cas prijezdu auta vetsi, nez pocatecni cas pozaru, zasahuje do vypoctu. Pokud je mensi nebo roven, auto uz pro pocitany usek dorazilo
			if (round(Auta[i]->AutoStartTime + Auta[i]->AutoArrivalTime) > round(StartTime)) {				
				// Pokud auto dorazi v prubehu pocitaneho useku, je tento usek treba zkratit na cas prijezdu auta, aby se mohly vypocitat skody pro usek, kdy auto jeste nedorazilo
				if (round(Auta[i]->AutoStartTime + Auta[i]->AutoArrivalTime) < round(StartTime + temp)) {
					std::cout << "je treba snizit usek na " << (Auta[i]->AutoStartTime + Auta[i]->AutoArrivalTime) - StartTime << std::endl;
					temp = (Auta[i]->AutoStartTime + Auta[i]->AutoArrivalTime) - StartTime;
				}
			}
			else {
				autoCount++;
				std::cout << "v tomto useku je pritomnych " << autoCount << " aut\n";
			}			
		}
		// Nastavuje rust intenzity a skod podle poctu chybejicich hasicskych aut v pozaru
		double koefi = 0;
		switch ((int)pocatecniIntenzita - (autoCount * 100)) {
			case 0 ... 99:
				koefi = 1;
				intenzitaGrow = 0;
				break;					
			case 100 ... 199:
				intenzitaGrow = 0.3;
				koefi = 2;
				break;
			case 200 ... 299:
				intenzitaGrow = 0.6;
				koefi = 4;
				break;
			default:
				intenzitaGrow = 1;
				koefi = 8;
				break;
		}
		Intenzita += temp * intenzitaGrow;
		std::cout << "pritomna auta vyresena, usek je " << temp << ", intenzita vzrostla na " << Intenzita <<"\n";
		// Prumer intenzity, se kterou pocitany usek horel
		double average = (pocatecniIntenzita + (Intenzita))/2;
		std::cout << "skoda = prumer intenzit v danem useku " << average << " * koefi " << koefi << " * usek " << temp << std::endl;
		skoda += pow((average/10),average/100)*koefi*temp;
		// Posun pocatecni intenzity o vypocteny usek - chceme pocitat dalsi usek
		pocatecniIntenzita += intenzitaGrow * temp;
		// Posun casu pozaru o vypocteny usek - aktualni je jiz vypocteny, StartTime se tedy posune na dalsi nevypocitany usek
		StartTime += temp;
		burnTime -= temp;
		std::cout << "novy BurnTime " << burnTime << std::endl;
	// Cyklus pokracuje, pokud jeste nejsou vypocteny skody od zacatku pozaru (resp. posledniho vypocitaneho useku) az po aktualni stav pozaru
	} while (burnTime > 0);
	return skoda;	
}

Pozar::Pozar(int intenzita) : Process(intenzita) {
	Intenzita = intenzita;
	Priority = 100 - (int)fmod(intenzita, 100);
	Skoda = 0;
}

void Pozar::Behavior() {
	std::cout << "NOVY POZAR, INTENZITA " << Intenzita << std::endl; 
	StartTime = Time;
	double waitTime = 0;
	Poloha = GenerovatOhen();
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
		switch ((int)Intenzita) {
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
	// auto prijede za tolik minut (vzdalenost / rychlost v metrech za minutu) plus vyrazi 1 - 2 minuty od vyvolaneho poplachu
	current->AutoArrivalTime = Vzdalenost(Poloha, current->Poloha)/AVERAGE_CAR_SPEED + FIREMEN_GET_READY;
	Auta.push_back(current);
	std::cout << "Intenzita " << Intenzita << " sila aut " << Auta.size()*100 << "\n";
	if (Intenzita - (Auta.size()*100) >= 100)
		goto zpet;
	waitTime = (current->AutoArrivalTime + current->AutoStartTime) - Time;
	if (current->AutoArrivalTime + current->AutoStartTime > Time)
	{
		std::cout << "Cekani na posledni prijizdejici auto...\n";
		Wait(waitTime);
	}
	Skoda += VypocetSkod(waitTime, Intenzita);
	std::cout << "HORI... SKODA " << Skoda << std::endl;
	// TODO ZDENEK CAS
	int j = Intenzita/100;
	Wait(Exponential(Intenzita*j));
	Skoda += pow((Intenzita/2)/10,(Intenzita/2)/100);
	skody(Skoda);
	histintenzita(Intenzita);
	std::cout << "UHASENO... SKODA " << Skoda << std::endl;
	std::sort(Auta.begin(), Auta.end(), 
		[](Stanice *a, Stanice *b)
		{ 
			return a->AutoArrivalTime < b->AutoArrivalTime; 
		});
	// Autu po prijezdu od pozaru trva 25 - 35 minut doplneni vody, kysliku v pristrojich atd.
	Wait(Uniform(25,35));
	// Simuluje trvani cesty zpet jednotlivych vozu a uvolneni (cekani ve stanici na naplneni aut je simulovano pro vsechny vozy v predchozim kroku)
	waitTime = 0;
	for (int i = 0; i < Auta.size(); i++) {
		waitTime = Auta[i]->AutoArrivalTime - waitTime - FIREMEN_GET_READY;
		Wait(waitTime);		
		Auta[i]->AutoStartTime = 0;
		Auta[i]->AutoArrivalTime = 0;
		Release(*Auta[i]);
		UvolneniAuta();
	}
}


Generator::Generator() : Event() {
}
	
void Generator::Behavior() {
	double k = Random();
	/*(new Pozar(300 + Random()*50))->Activate();	
	(new Pozar(300 + Random()*50))->Activate();	
	(new Pozar(100 + Random()*50))->Activate();*/
	//TODO - vypocet intenzity, dosazeni spravnych hodnot pravdepodobnostniho rozlozeni
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
	// 1 rok v minutach 525600
	Init(0, 525600);
	RandomSeed(time(NULL));
	(new Generator())->Activate();
	Run();
	for (int i = 0; i < stanice.size(); i++)
		stanice[i]->Output();
	Q1.Output();
	Q2.Output();
	Q3.Output();
	skody.Output();
	histintenzita.Output();
	DeleteStanice();
}