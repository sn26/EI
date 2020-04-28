/* 
        1_DNI: 48536200J
        1_NOMBRE: PEREZ PONSODA, JORGE AMIL
*/

#include <iostream>
#include <string>
#include <list>
#include <sys/resource.h>
#include "tokenizador.h"
#include "indexadorHash.h"
#include "indexadorInformacion.h"

using namespace std;

double getcputime (void) {
	struct timeval tim;
	struct rusage ru;
	getrusage (RUSAGE_SELF, &ru);
	tim = ru.ru_utime;
	double t = (double) tim.tv_sec + (double) tim.tv_usec / 1000000.0;
	tim = ru.ru_stime;
	t += (double) tim.tv_sec + (double) tim.tv_usec / 1000000.0;
	return t;
}

main() {
	long double aa = getcputime();

	IndexadorHash b("./StopWordsEspanyol.txt", ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@", false, false, "./indicePrueba", 0, false, true);
	b.Indexar ("listaFicheros_corto1.txt");
    b.GuardarIndexacion();
	
	//cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
}