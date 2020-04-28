#ifndef INDEXADORHASH_H_
#define INDEXADORHASH_H_

#include "indexadorInformacion.h"
using namespace std; 

class IndexadorHash {
    friend ostream& operator<<(ostream& s, const IndexadorHash& p) {
        s << "Fichero con el listado de palabras de parada: " << p.ficheroStopWords << endl;
        s << "Tokenizador: " << p.tok << endl;
        s << "Directorio donde se almacenara el indice generado: " << p.directorioIndice << endl;
        s << "Stemmer utilizado: " << p.tipoStemmer << endl;
        s << "Informacion de la coleccion indexada: " << p.informacionColeccionDocs << endl;
        s << "Se almacenara parte del indice en disco duro: " <<p.almacenarEnDisco << endl;
        s << "Se almacenaran las posiciones de los terminos: "<<
        p.almacenarPosTerm;
        return s;
    }

    public: 
        IndexadorHash(const string& fichStopWords, const string& delimitadores, const bool& detectComp, const bool& minuscSinAcentos, 
        const string& dirIndice, const int& tStemmer, const bool& almEnDisco, const bool& almPosTerm); //HECHO
        IndexadorHash(const string& directorioIndexacion);  //HECHO
        IndexadorHash(const IndexadorHash&); //HECHO 
        ~IndexadorHash(); //HECHO
        IndexadorHash& operator= (const IndexadorHash&); //HECHO
        bool Indexar(const string& ficheroDocumentos); //M?todo con el que vamos a coger los documentos y a indexarlos HECHO
        bool IndexarDirectorio(const string& dirAIndexar); //HECHO
        bool GuardarIndexacion() const; //HECHO
        bool RecuperarIndexacion (const string& directorioIndexacion); //HECHO
        void ImprimirIndexacion() const {
            cout << "Terminos indexados: " << endl;
            //HABR? QUE RECORRER CADA UNO DE LOS T?RMINOS
            for(auto iterator = this->indice.begin() ; iterator!= indice.end() ; iterator ++){
                cout<<iterator->first<<'\t'<<iterator->second<<endl; 
            }
            //cout << termino << '\t' << InformacionTermino << endl;
            cout << "Documentos indexados: " << endl;
            for(auto iterator = this->indiceDocs.begin() ; iterator!=indiceDocs.end() ; iterator++){
                cout<<iterator->first<<'\t'<<iterator->second<<endl; 
            }
            //cout << nomDoc << '\t' << InfDoc << endl;
        }
        bool IndexarPregunta(const string& preg); //HECHO
        bool DevuelvePregunta(string& preg) const; //HECHO
        bool DevuelvePregunta(const string& word, InformacionTerminoPregunta& inf) const; //HECHO
        bool DevuelvePregunta(InformacionPregunta& inf) const; //HECHO
        void ImprimirIndexacionPregunta() {
            cout << "Pregunta indexada: "<< pregunta << endl;
            cout << "Terminos indexados en la pregunta: " << endl; 
            for(auto iterator = this->indicePregunta.begin() ; iterator!= indicePregunta.end() ; iterator++){
                cout<<iterator->first<<'\t'<<iterator->second<<endl ; 
            }
            //cout << termino << '\t' << InformacionTerminoPregunta << endl;
            cout << "Informacion de la pregunta: " << infPregunta << endl;
        
        }

        void ImprimirPregunta(){
            cout << "Pregunta indexada: "<< pregunta << endl; 
            cout << "Informacion de la pregunta: " << infPregunta << endl;
        }
        bool Devuelve(const string& word, InformacionTermino& inf) const; //HECHO
        bool Devuelve(const string& word, const string& nomDoc, InfTermDoc& InfDoc) const;  //HECHO
        bool Existe(const string& word) const;  //HECHO
        bool Borra(const string& word); //HEHCO**PERO NO S? SI EST? CORRECTO DEL TODO
        bool BorraDoc(const string& nomDoc); //HECHO 
        void VaciarIndiceDocs();  //HECHO
        void VaciarIndicePreg(); //HECHO
        bool Actualiza(const string& word, const InformacionTermino& inf); //HECHO
        bool Inserta(const string& word, const InformacionTermino& inf);  //HECHO
        int NumPalIndexadas() const; //HECHO
        string DevolverFichPalParada () const;  //HECHO
        void ListarPalParada() const; //HECHO
        int NumPalParada() const; //HECHO; 
        string DevolverDelimitadores () const;// Devuelve los delimitadores utilizados por el tokenizador  HECHO
        bool DevolverCasosEspeciales () const; //HECHO Devuelve si el tokenizador analiza los casos especiales 
        bool DevolverPasarAminuscSinAcentos () const; //HECHO Devuelve si el tokenizador pasa a min?sculas y sin acentos
        bool DevolverAlmacenarPosTerm () const; //HECHO Devuelve el valor de almacenarPosTerm
        string DevolverDirIndice () const;  // HECHO Devuelve ?directorioIndice? (el directorio del disco duro donde se almacenar? el ?ndice)
        int DevolverTipoStemming () const; // HECHO Devolver? el tipo de stemming realizado en la indexaci?n de acuerdo con el valor indicado en la variable privada ?tipoStemmer?
        bool DevolverAlmEnDisco () const; // HECHO Devolver? el valor indicado en la variable privada ?almEnDisco? 
        void ListarInfColeccDocs() const; //HECHO Mostrar por pantalla: cout << informacionColeccionDocs << endl;
        void ListarTerminos() const; // HECHO Mostrar por pantalla el contenido el contenido del campo privado ??ndice?: cout << termino << ?\t? << InformacionTermino << endl;
        bool ListarTerminos(const string& nomDoc) const; //HECHO
        void ListarDocs() const; //HECHO Mostrar por pantalla el contenido el contenido del campo privado
        bool ListarDocs(const string& nomDoc) const; //HECHO

        private: 
            bool IndexarUnTermPregunta( const string & , InformacionTerminoPregunta &); //M?todo que usaremos para indexar t?rminos que componen una pregunta  HECHO
            //string IndexarSubDirectorios(const string & secondDir);  //M?todo que usaremos para indexar los ficheros de los subdirectorios del fichero inicial
            bool IndexarUnDocu(const char * to , InfDoc & , list<string> * palabras); //HECHO M?todo para indexar un documento (Lo usaremos dentro del m?todo de indexar documentos)
            char * pasarAMinSin(char * word) const; //HECHO PARA PASAR A MIN?SCULAS UNA PALABRA
            bool ReadPrivValuesMaps(); //HECHO
            bool ReadPrivValues1(); //HECHO
            bool WriteStopWords() const ; // HECHOM?todo para escribir en el fichero HECHO 
            void ReadStopWords( ); //HECHO M?todo para leer las palabras de parada del fichero HECHO
            //bool guardarEnDisco();
            //bool leerDeDisco();
            IndexadorHash(); //HECHO Este constructor se pone en la parte privada porque no se permitir? crear un indexador sin inicializarlo convenientemente. La inicializaci?n la decidir? el alumno
            unordered_map<string, InformacionTermino> indice; // ?ndice de t?rminos indexados accesible por el t?rmino
            unordered_map<string, InfDoc> indiceDocs; // ?ndice de documentos indexados accesible por el nombre del documento
            InfColeccionDocs informacionColeccionDocs; // Informaci?n recogida de la colecci?n de documentos indexada
            string pregunta; // Pregunta indexada actualmente. Si no hay ninguna indexada, contendr?a el valor ??
            unordered_map<string, InformacionTerminoPregunta> indicePregunta; // ?ndice de t?rminos indexados en una pregunta. Se almacenar? en memoria principal
            InformacionPregunta infPregunta; // Informaci?n recogida de la pregunta indexada. Se almacenar? en memoria principal
            unordered_set<string> stopWords; 
            string ficheroStopWords; // Nombre del fichero que contiene las palabras de parada
            string directorioIndice;
            Tokenizador tok; 
            int tipoStemmer; // 0 = no se aplica stemmer: se indexa el t?rmino tal y como aparece tokenizado
            // Los siguientes valores har?n que los t?rminos a indexar se les aplique el stemmer y se almacene solo dicho stem.
            // 1 = stemmer de Porter para espa?ol
            // 2 = stemmer de Porter para ingl?s  
            bool almacenarEnDisco;
            bool almacenarPosTerm; 
            



};

#endif