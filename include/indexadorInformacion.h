#ifndef INDEXADORINFORMACION_H_
#define INDEXADORINFORMACION_H_
#include <iostream>
#include <string>
#include "stemmer.h"
#include "tokenizador.h"
#include <ctime>
#include <unordered_map>
#include <unordered_set>

using namespace std;


class InfTermDoc {
    friend ostream& operator<<(ostream& s, const InfTermDoc& p){
        s << "ft: " << p.ft;
        // A continuaci?n se mostrar?an todos los elementos de p.posTerm (?posicion
        //TAB posicion TAB ... posicion, es decir nunca finalizar? en un TAB?): s <<
        //?\t? << posicion; 
        InfTermDoc p2 = p; 
        for (std::list<int>::iterator it=p2.posTerm.begin(); it != p2.posTerm.end(); ++it){
            s <<"\t" << *it;
        }
    
        return s;
    }
    public:
        InfTermDoc (const InfTermDoc &);
        InfTermDoc (); // Inicializa ft = 0
        ~InfTermDoc (); // Pone ft = 0
        InfTermDoc & operator= (const InfTermDoc &);
        //M?todos para gestionar la parte privada
        const list<int>& getPosTerm();
		void setPosTerm(const list<int>&);
        const int getFt(); 
        void setFt(const int & );
    private:
        int ft; // Frecuencia del t?rmino en el documento
        list<int> posTerm;
        
};
/*ostream& operator<<(ostream& s, const InfTermDoc& p){
    
} */


class InformacionTermino {
    friend ostream& operator<<(ostream& s, const InformacionTermino& p){
        s << "Frecuencia total: " << p.ftc << "\tfd: " << p.l_docs.size();
        // A continuaci?n se mostrar?an todos los elementos de p.l_docs: s <<?\tId.Doc: ? << idDoc << ?\t? << InfTermDoc;   
        for ( auto it = p.l_docs.begin(); it != p.l_docs.end(); ++it ){
            s <<"\tId.Doc: " << it->first << "\t" << it->second;  
        }
        return s;
    }
    public:
        InformacionTermino (const InformacionTermino &);
        InformacionTermino(const int& fft, const int& ffd);
        InformacionTermino (); // Inicializa ftc = 0
        ~InformacionTermino (); // Pone ftc = 0 y vac?a l_docs
        InformacionTermino & operator= (const InformacionTermino &);
        //M?TODOS PARA MANEJAR LA PARTE PRIVADA DE LA CLASE
        const int getFtc(); 
        void setFtc(const int&); 
        const int getFt(); 
        void setFt( const int &); 
        const int getFd(); 
        void setFd(const int &);    
        void setL_docs(const unordered_map<long int, InfTermDoc> & l_docs ) ; 
        unordered_map<long int , InfTermDoc>& getL_docs();             
    private:
        int ft;
        int ftc; // Frecuencia total del t?rmino en la colecci?n
        int fd; 
        unordered_map<long int, InfTermDoc> l_docs;
// Tabla Hash que se acceder? por el id del documento, devolviendo unobjeto de la clase InfTermDoc que contiene toda la informaci?n de  aparici?n del t?rmino en el documento
};

/*ostream& operator<<(ostream& s, const InformacionTermino& p) {

    
} */

class Fecha{
    //friend ostream& operator<<(ostream &s, const Fecha& p);
	public:
		Fecha(const Fecha& fecha);
		Fecha(const int& dia, const int &mes,const int &anyo, const int &hora,const int & min, const int &seg);
		Fecha();//Inicializa la fecha a la actual del sistema
        Fecha(tm * ltm );
        Fecha& operator= (const Fecha &);
        bool operator==(const Fecha & ); //Para comparar si dos fechas son iguales
        bool operator>(const Fecha & ); //Para comprobar si nuestra fecha es más actual 
        bool operator<(const Fecha &); //Para comprobar si nuestra fecha es más antigua
        void setFecha(const int & dia , const int &  mes ,const int &  anyo , const int &  hora, const int &  min ,const int &  seg);
        const int getMes(); 
        const int getDia();
        const int getAnyo(); 
        const int getHora(); 
        const int getMin(); 
        const int getSeg(); 
	private:
        bool comprobadorFecha(const int &  dia , const int &  mes , const int &  anyo , const int &  hora , const int &  min , const int & seg); //Para comprobar si una fecha es correcta
		int mes;
        int dia; 
		int anyo;
		int hora;
		int min;
		int seg;
};
/*ostream& operator <<(ostream& s, const Fecha& p){
	//Muestra la fecha
	s << "Fecha: " << p.dia << "/"<< p.mes << "/" <<p.anyo <<" ";
	//Muestra la hora
	if (p.hora<10){
		s << "0" << p.hora <<":";
	}else{s << p.hora << ":";}
	//Muestra los minutos
	if (p.min<10){
		s << "0" << p.min <<":";
	}else{s << p.min << ":";}
	//Muestra los segundos
	if (p.seg<10){
		s << "0" << p.seg;
	}else{s << p.seg;}

	return s;
}*/


class InfDoc {
    friend ostream& operator<<(ostream& s, const InfDoc& p){
        s << "idDoc: " << p.idDoc << "\tnumPal: " << p.numPal <<
        "\tnumPalSinParada: " << p.numPalSinParada << "\tnumPalDiferentes: " <<
        p.numPalDiferentes << "\ttamBytes: " << p.tamBytes;
        return s;
    }
    public:
        InfDoc (const InfDoc &);
        InfDoc ();
        ~InfDoc ();
        InfDoc & operator= (const InfDoc &);
        //M?TODOS PARA GESTIONAR LA CLASE PRIVADA
        const long int getIdDoc();
		void setIdDoc(const long int &) ;
        const int getNumPal();
		void setNumPal(const int &);
        const int getNumPalSinParada();
		void setNumPalSinParada(const int &);
        const int getNumPalDiferentes();
		void setNumPalDiferentes(const int &);
        const int getTamBytes();
		void setTamBytes(const int &);
        const Fecha getFechaModificacion();
		void setFechaModificacion(const Fecha& fechaModificacion);
    private:
        long int idDoc;
        int numPal; // N? total de palabras del documento
        int numPalSinParada; // N? total de palabras sin stop-words del documento
        int numPalDiferentes;
        int tamBytes; // Tama?o en bytes del documento
        Fecha fechaModificacion;
};





class InfColeccionDocs {
    friend ostream& operator<<(ostream& s, const InfColeccionDocs& p){
        s << "numDocs: " << p.numDocs << "\tnumTotalPal: " << p.numTotalPal <<
        "\tnumTotalPalSinParada: " << p.numTotalPalSinParada <<
        "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes << "\ttamBytes: " << p.tamBytes;
        
        return s;
    }
    public:
        InfColeccionDocs (const InfColeccionDocs &);
        InfColeccionDocs ();
        ~InfColeccionDocs ();
        InfColeccionDocs & operator= (const InfColeccionDocs &);

        
        long int getNumDocs() const;
		void setNumDocs(const long int &numDocs);

		long int getNumTotalPal() const ;
		void setNumTotalPal(const long int & numTotalPal);

        long int getNumTotalPalSinParada() const;
		void setNumTotalPalSinParada(const long int & numTotalPalSinParada);

		long int getNumTotalPalDiferentes() const;
		void setNumTotalPalDiferentes(const long int & numTotalPalDiferentes);

		long int getTamBytes() const;
		void setTamBytes(const long int & tamBytes);
    private:
        long int numDocs; // N? total de documentos en la colecci?n
        long int numTotalPal; // N? total de palabras en la colecci?n
        long int numTotalPalSinParada; // N? total de palabras sin stop-words en la colecci?n   
        long int numTotalPalDiferentes; // N? total de palabras diferentes en la colecci?n que no sean stopwords (sin acumular la frecuencia de cada una de ellas)
        long int tamBytes; // Tama?o total en bytes de la colecci?n
};



class InformacionTerminoPregunta {
    friend ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p){
        s << "ft: " << p.ft;
        // A continuaci?n se mostrar?an todos los elementos de p.posTerm (?posicion
        //TAB posicion TAB ... posicion, es decir nunca finalizar? en un TAB?): s <<
        //?\t? << posicion; 
        InformacionTerminoPregunta p2 = p; 
        for (std::list<int>::iterator it=p2.posTerm.begin(); it != p2.posTerm.end(); ++it){
            s <<"\t" << *it;
        }
        return s; 
    }
    public:
        InformacionTerminoPregunta (const InformacionTerminoPregunta &);
        InformacionTerminoPregunta ();
        ~InformacionTerminoPregunta ();
        InformacionTerminoPregunta & operator= (const InformacionTerminoPregunta&);
        //M?TODOS A?ADIDOS PARA GESTIONAR LA CLASE PRIVADA
        const int getFt(); 
        void setFt(const int &);
        const list<int>& getPosTerm();
		void setPosTerm(const list<int>& posTerm);

    private:
        int ft; // Frecuencia total del t?rmino en la pregunta
        list<int> posTerm;// Solo se almacenar? esta informaci?n si el campo privado del indexador almacenarPosTerm == true
};


class InformacionPregunta {
    friend ostream& operator<<(ostream& s, const InformacionPregunta& p){
        s << "numTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: "<<
        p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes;
        return s;
    }
    public:
        InformacionPregunta (const InformacionPregunta &);
        InformacionPregunta ();
        ~InformacionPregunta ();
        InformacionPregunta & operator= (const InformacionPregunta &);
        
        //M?todos de gesti?n parte privadad
        long int getNumTotalPal()const ; 
        void setNumTotalPal(const long int & ); 
        long int getNumTotalPalSinParada() const; 
        void setNumTotalPalSinParada( const long int & );
        long int getNumTotalPalDiferentes() const; 
        void setNumTotalPalDiferentes(const long int &);
    private:
        long int numTotalPal;
        // N? total de palabras en la pregunta
        long int numTotalPalSinParada;
        // N? total de palabras sin stop-words en la pregunta
        long int numTotalPalDiferentes;
        // N? total de palabras diferentes en la pregunta que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
};

#endif




