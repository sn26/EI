#include <iostream> 
#include <string>
#include <list> 
#include "tokenizador.h"

using namespace std;

///////// Comprobaci�n de que vac�e la lista resultado

void imprimirListaSTL(const list<string>& cadena)
{
        list<string>::const_iterator itCadena;
        for(itCadena=cadena.begin();itCadena!=cadena.end();itCadena++)
        {
                cout <<"TOKEN: " <<(*itCadena) <<endl;
        }
        cout << endl;
}

int
main(void)
{
	bool kCasosEspeciales = true, kpasarAminusculas = false;

	list<string> lt1, lt2;

Tokenizador a(".,-%&//)(", true, true); 
list<string> tokens; 

a.Tokenizar("http: a.b..d.a.d..f.d.a..s...S.D...D...D....D...nd . . . .,.,.d,.,.,d.d,.,.d...d..,.,.,.,.,d%$Jkmfkkfkfkfk.56%$%$%$%%$%...-ms-dos--dsd-�ds--dd-s--b-di https://hola.com ....https://web.whatsapp.com/ ghh ..?.!http:.?&//hj.com/ hassd/// chsjf/.dfsdshjk .ftp://fdsjh.com/ ..23  ..?!https:.?&//hj.com/?= 10.0.0.0..57 .fds.53..6.4.5.23.6...532 0.2123 .4.2.3.4..5,3  ,,23.05 ,,8.6.6.5.7,,5,322,34,2..3.22..4.1,7,53,,,32,432,321..432,,32.432,4.323,132,, ,21,4,21.432,4..321,4.32,432..432,432.54,654.7,43.65,322.,42,543.,432,..423,4.322,42 ! 10..10.a.20.10 0.86.54.3.543.4.322.432.5.34.,5,432,.5433,5.3,5.4,3.5,2.,.24,3.25,34.5,.34,5.43...,543.,5.34,5.4.3.,43.,5.32,5.654.7.8.6759..0.54.3.5.5.435,34.5,4.7%4.6%.3$ 7..6.5.4hola.5 8..6.7.5.5.4&%%$$/http:ghfdssjjf.com.764322 hola.76..4.3.4.", tokens);
	imprimirListaSTL(tokens);




}
