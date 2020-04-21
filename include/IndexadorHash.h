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
        bool Indexar(const string& ficheroDocumentos); //Método con el que vamos a coger los documentos y a indexarlos
        bool IndexarDirectorio(const string& dirAIndexar); 
        bool GuardarIndexacion() const; //HECHO
        bool RecuperarIndexacion (const string& directorioIndexacion); //HECHO
        void ImprimirIndexacion() const {
            cout << "Terminos indexados: " << endl;
            //HABRÁ QUE RECORRER CADA UNO DE LOS TÉRMINOS
            //cout << termino << '\t' << InformacionTermino << endl;
            cout << "Documentos indexados: " << endl;
            //cout << nomDoc << '\t' << InfDoc << endl;
        }
        bool IndexarPregunta(const string& preg); 
        bool DevuelvePregunta(string& preg) const; 
        bool DevuelvePregunta(const string& word, InformacionTerminoPregunta& inf) const;
        bool DevuelvePregunta(InformacionPregunta& inf) const;
        void ImprimirIndexacionPregunta() {
            cout << "Pregunta indexada: "<< pregunta << endl;
            cout << "Terminos indexados en la pregunta: " << endl; 
            //cout << termino << '\t' << InformacionTerminoPregunta << endl;
            cout << "Informacion de la pregunta: " << infPregunta << endl;
        
        }

        void ImprimirPregunta(){
            cout << "Pregunta indexada: "<< pregunta << endl; 
            cout << "Informacion de la pregunta: " << infPregunta << endl;
        }
        bool Devuelve(const string& word, InformacionTermino& inf) const; //HECHO
        bool Devuelve(const string& word, const string& nomDoc, InfTermDoc& InfDoc) const; 
        bool Existe(const string& word) const;  //HECHO
        bool Borra(const string& word);
        bool BorraDoc(const string& nomDoc);
        void VaciarIndiceDocs(); 
        void VaciarIndicePreg(); 
        bool Actualiza(const string& word, const InformacionTermino& inf); 
        bool Inserta(const string& word, const InformacionTermino& inf); 
        int NumPalIndexadas() const;
        string DevolverFichPalParada () const; 
        void ListarPalParada() const; 
        int NumPalParada() const; 
        string DevolverDelimitadores () const;// Devuelve los delimitadores utilizados por el tokenizador 
        bool DevolverCasosEspeciales () const; // Devuelve si el tokenizador analiza los casos especiales 
        bool DevolverPasarAminuscSinAcentos () const; // Devuelve si el tokenizador pasa a minúsculas y sin acentos
        bool DevolverAlmacenarPosTerm () const; // Devuelve el valor de almacenarPosTerm
        string DevolverDirIndice () const;  // Devuelve ?directorioIndice? (el directorio del disco duro donde se almacenará el índice)
        int DevolverTipoStemming () const; // HECHO Devolverá el tipo de stemming realizado en la indexación de acuerdo con el valor indicado en la variable privada ?tipoStemmer?
        bool DevolverAlmEnDisco () const; // Devolverá el valor indicado en la variable privada ?almEnDisco? 
        void ListarInfColeccDocs() const; // Mostrar por pantalla: cout << informacionColeccionDocs << endl;
        void ListarTerminos() const; // Mostrar por pantalla el contenido el contenido del campo privado ?índice?: cout << termino << ?\t? << InformacionTermino << endl;
        bool ListarTerminos(const string& nomDoc) const;
        void ListarDocs() const; // Mostrar por pantalla el contenido el contenido del campo privado
        bool ListarDocs(const string& nomDoc) const;


        private: 
            bool IndexarUnDocu(const char * to , InfDoc &); //Método para indexar un documento (Lo usaremos dentro del método de indexar documentos)
            char * pasarAMinSin(char * word) const; //PARA PASAR A MINÚSCULAS UNA PALABRA
            bool ReadPrivValuesMaps(); //HECHO
            bool ReadPrivValues1(); //HECHO
            bool WriteStopWords() const ; //Método para escribir en el fichero HECHO 
            void ReadStopWords( ); //Método para leer las palabras de parada del fichero HECHO
            IndexadorHash(); // Este constructor se pone en la parte privada porque no se permitirá crear un indexador sin inicializarlo convenientemente. La inicialización la decidirá el alumno
            unordered_map<string, InformacionTermino> indice; // Índice de términos indexados accesible por el término
            unordered_map<string, InfDoc> indiceDocs; // Índice de documentos indexados accesible por el nombre del documento
            InfColeccionDocs informacionColeccionDocs; // Información recogida de la colección de documentos indexada
            string pregunta; // Pregunta indexada actualmente. Si no hay ninguna indexada, contendría el valor ??
            unordered_map<string, InformacionTerminoPregunta> indicePregunta; // Índice de términos indexados en una pregunta. Se almacenará en memoria principal
            InformacionPregunta infPregunta; // Información recogida de la pregunta indexada. Se almacenará en memoria principal
            unordered_set<string> stopWords; 
            string ficheroStopWords; // Nombre del fichero que contiene las palabras de parada
            string directorioIndice;
            Tokenizador tok; 
            int tipoStemmer; // 0 = no se aplica stemmer: se indexa el término tal y como aparece tokenizado
            // Los siguientes valores harán que los términos a indexar se les aplique el stemmer y se almacene solo dicho stem.
            // 1 = stemmer de Porter para español
            // 2 = stemmer de Porter para inglés  
            bool almacenarEnDisco;
            bool almacenarPosTerm; 
            



};

#endif