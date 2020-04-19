//Sirvent Navarro,Alvaro
#include <iostream>
#include <list>
#include <fstream> 




#ifndef TOKENIZADOR_H_
#define TOKENIZADOR_H_

using namespace std;

class Tokenizador {
	friend ostream& operator<<(ostream&, const Tokenizador&);

	public:
		Tokenizador(const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos); //HECHO

		Tokenizador(const Tokenizador&); //HECHO
		Tokenizador(); //HECHO
	
		~Tokenizador(); //HECHO
		Tokenizador& operator= (const Tokenizador&); //HECHO
		void Tokenizar(const string& str, list<string>& tokens) const; //A MEJORAR
		// Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciar? antes de almacenar el resultado de la tokenizaci?n.
		bool Tokenizar(const string& i, const string& f) const;
		// Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada l?nea del fichero).Devolver? true si se realiza latokenizaci?n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente(p.ej.que no exista el archivo i)
		bool Tokenizar(const string& i) const; 
		bool TokenizarListaFicheros(const string& i) const;
		bool TokenizarDirectorio(const string& i) const;
	
		void DelimitadoresPalabra(const string& nuevoDelimiters); //HECHO
		void AnyadirDelimitadoresPalabra(const string& nuevoDelimiters); //HECHO
		
		string DelimitadoresPalabra() const; //HECH0
		// Devuelve ?delimiters?
		void CasosEspeciales(const bool& nuevoCasosEspeciales); //HECHO
		// Cambia la variable privada ?casosEspeciales?
		bool CasosEspeciales() const ; //HECHO
		// Devuelve el contenido de la variable privada ?casosEspeciales?
		void PasarAminuscSinAcentos(const bool& nuevoPasarAminuscSinAcentos); //HECHO

		bool PasarAminuscSinAcentos() const ; //HECHO
	
	private:
		bool email(string::size_type&, const string & , list<string>&tokens, string::size_type & , string::size_type &) const; //M�todo para tokenizar los casos especiales de los emails
		void normal(  const string& , list<string>&tokens, string::size_type & , string::size_type &) const; //M�todo para cuando no se ha realizado ning�n caso especial
		void acronimo(const string& , const string & , list<string>&tokens, string::size_type & , string::size_type &) const; //M�todo para tokenizar casos especiales acr�nimos		
		void guiones(const string& , const string & , list<string>&tokens, string::size_type & , string::size_type &) const; //M�todo para tokenizar cassos especiales guines
		int numero(const string &,const string&, list<string>& tokens ,  string::size_type &  ,  string::size_type & ) const; //M�todo para tokenizar casos especiales n�meros
		//void numeroSeguidoX(const string& cadena, list<string>& tokens ,string::size_type & lastPos  , string::size_type & pos ) const;
		//void puntoNumero(const string& cadena, list<string>& tokens ,string::size_type & lastPos  , string::size_type & pos ) const;
		//M�todo para gestionar los casos especiales de la url
		bool url(const string&, list<string>& tokens ,  string::size_type &  ,  string::size_type &  ) const; 
		//M�todo para filtrar los string repetidos
		void filtrarRepetidos(const string&) ; //HECHO
		//PASAR A MIN�SCULAS M�TODO
		string pasarMinSin( string& cadena) const;  

		//string cadenaEspeciales(string &cadena , const string &delimitadoresEspeciales, bool &primero  , string::size_type &lastPos , string::size_type &pos) const;
		string delimiters; // Delimitadores de t?rminos. Aunque se
		
		bool casosEspeciales;
	
		bool pasarAminuscSinAcentos;
	
};

#endif