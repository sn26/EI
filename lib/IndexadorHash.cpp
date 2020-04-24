#include "IndexadorHash.h"
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <dirent.h>


//Ser? privado porque no se permitir? al usuario usarlo
IndexadorHash::IndexadorHash(){
    this->pregunta=""; 
    this->tok = Tokenizador();         
    this->tok.CasosEspeciales(false);
    this->tok.DelimitadoresPalabra(" "); 
    this->ficheroStopWords="";
    this->directorioIndice="./";//Lo guardaremos en el mismo directorio que donde tenemos creadas las cosas 
    this->tipoStemmer= 0; 
    this->almacenarEnDisco=false; 
    this->almacenarPosTerm=false;
    this->indice.clear(); 
    this->indiceDocs.clear(); 
    this->indicePregunta.clear(); 
}

//M?TODO PARA ESCRIBIR EN EL FICHERO
bool IndexadorHash::WriteStopWords() const {
    std::string buffer;
    buffer.clear();
    //CON ESTO COGEMOS CADA UNA DE LAS PALABRAS
    for ( auto it = this->stopWords.begin(); it != this->stopWords.end(); ++it ){
        buffer.append((*it));
        buffer.append("\n");
    }
    //Ahora lo escribimos en el fichero de salida
    std::ofstream i(this->directorioIndice + "/" + this->ficheroStopWords);
    if(!i) {
        cerr << "ERROR: No se ha podido escribir en el directorio " <<this->directorioIndice<<" el archivo de guardado de las palabras de parada" << endl;
        return false;
    }
    i.write(buffer.data(), buffer.size());
    buffer.clear();
    i.close();
    return true;
    
}

//M?todo para leer las palabras del fichero
void IndexadorHash::ReadStopWords(){
    ifstream file;
    string cadena;
    file.open(this->ficheroStopWords.c_str());
    if(!file) {
        cerr << "ERROR: No existe el archivo: " << this->ficheroStopWords << endl;
    }
    std::string buffer;
    std::ifstream f(this->ficheroStopWords);
    std::string to;
    while(std::getline(f,to,'\n')){
        if(to.length() != 0) this->stopWords.insert(to);      
    }
    f.close();
    file.close();
}

IndexadorHash::~IndexadorHash(){ 
    this->indice.clear(); this->indiceDocs.clear(); this->indicePregunta.clear(); 
    this->informacionColeccionDocs.~InfColeccionDocs(); 
    this->pregunta.clear(); this->pregunta= ""; 
    this->infPregunta.~InformacionPregunta();
    this->stopWords.clear(); 
    this->ficheroStopWords.clear(); this->ficheroStopWords="";
    this->directorioIndice.clear(); this->directorioIndice="";
    this->tok.~Tokenizador(); 
    this->tipoStemmer = 0;
    this->almacenarEnDisco = false ; 
    this->almacenarPosTerm= false; ;
}

bool IndexadorHash::RecuperarIndexacion (const string& directorioIndexacion){
    (*this) = IndexadorHash(directorioIndexacion); //Llamaremos al constructor, que ya es el que de por s? hace todo lo que deber?a hacer esto
    return true;
}


IndexadorHash & IndexadorHash::operator=(const IndexadorHash& copia ){
    this->indice = copia.indice;
    this->indiceDocs = copia.indiceDocs; 
    this->informacionColeccionDocs = copia.informacionColeccionDocs; 
    this->pregunta = copia.pregunta;
    this->indicePregunta = copia.indicePregunta; 
    this->infPregunta = copia.infPregunta; 
    this->stopWords = copia.stopWords; 
    this->ficheroStopWords= copia.ficheroStopWords; 
    this->directorioIndice = copia.directorioIndice; 
    this->tok = copia.tok; 
    this->tipoStemmer = copia.tipoStemmer;
    this->almacenarEnDisco = copia.almacenarEnDisco; 
    this->almacenarPosTerm = copia.almacenarPosTerm;
    return *this;
}

//CONSTRUCTOR DE COPIA
IndexadorHash::IndexadorHash(const IndexadorHash& copia ){
    (*this) = copia;
}

/**
 * Constructor de la clase
 * */ 
IndexadorHash::IndexadorHash(const string& fichStopWords, const string& delimitadores, const bool& detectComp, const bool& minuscSinAcentos, 
const string& dirIndice, const int& tStemmer, const bool& almEnDisco, const bool& almPosTerm){
    this->ficheroStopWords = fichStopWords;
    ReadStopWords(); //Primero leeremos lo que haya en el fichero de StopWords. 
    this->tok = Tokenizador(); 
    this->tok.DelimitadoresPalabra(delimitadores); 
    this->tok.CasosEspeciales(detectComp); 
    this->tok.PasarAminuscSinAcentos(minuscSinAcentos); 
    //VEMOS EN QU? DIRECTORIO VAMOS A ALMACENAR LAS COSAS
    if(dirIndice.length() == 0 ) this->directorioIndice = "./"; 
    else this->directorioIndice = dirIndice; 
    this->tipoStemmer = tStemmer; //TIPO DE STEMMER QUE SE VA A UTILIZAR (0 (NO SE APLICA ),1 (ESPA?OL) ,2(INGL?S))
    this->almacenarEnDisco = almEnDisco; 
    this->almacenarPosTerm = almPosTerm; 
    this->indice.clear(); 
    this->indiceDocs.clear(); 
    this-> indicePregunta.clear(); 
    this->informacionColeccionDocs =  InfColeccionDocs(); 
    this->infPregunta = InformacionPregunta(); 
    this->pregunta = "";
}





//Aqu? ser? donde nos guardaremos la indexaci?n
/*
        FICHEROS: 
            ficheroStopWords ->DONDE GUARDAREMOS LAS PALABRAS DE PARADA
            PrivValues1.txt -> GUARDAREMOS LOS VALORES DE LAS VARIABLES PRIVADAS QUE NO SEAN MUY LARGOS DE GUARDAR 
            PrivValuesMaps.txt -> GUARDAREMOS LOS MAPAS Y COSAS DEL TIPO, QUE SER?N M?S LARGOS DE GUARDAR
    */ 
bool IndexadorHash::GuardarIndexacion() const{
    //PRIMERO DE TODO, COMPROBAREMOS EL DIRECTORIO, Y EN EL CASO DE QUE NO EXISTA, PASAREMOS A CREARLO
    struct stat dir;
    if(stat(this->directorioIndice.c_str(), &dir) == -1 || !S_ISDIR(dir.st_mode) ){
        if (mkdir(const_cast<char*>(this->directorioIndice.c_str()), 0777) == -1) {
            cerr << "Error :  " << strerror(errno) << endl; 
            return false;
        }
    }
    if(almacenarEnDisco == false ) { //SIGNIFICA QUE HAREMOS UN ALMACENADO COMPLETO EN DISCO
        if(!WriteStopWords()) {//ALMACENAMOS LAS PALABRAS DE PARADA
            return false;
        } 
        std::string buffer;
        buffer.clear();
        //AHORA COGEREMOS LOS VALORES DE LAS VARIABLES PRIVADAS QUE IR?N AL PRIMER ARCHIVO    
        buffer.append("PreguntaIndexada" + this->pregunta);     
        buffer.append("\n");
        buffer.append("FicheroStopWords" + this->ficheroStopWords);    
        buffer.append("\n");
        buffer.append("tipoStemmer" + this->tipoStemmer);   
        buffer.append("\n");
        buffer.append("Almacena en disco" + this->almacenarEnDisco);    
        buffer.append("\n");
        buffer.append("Almacena posTerm" + this->almacenarPosTerm);   
        buffer.append("\n");
        buffer.append("TOKDelimiters" + this->tok.DelimitadoresPalabra());  
        buffer.append("\n");
        buffer.append("TOKPasarAMinSin" + this->tok.PasarAminuscSinAcentos());  
        buffer.append("\n");
        buffer.append("TOKCasosEspeciales" + this->tok.CasosEspeciales());  
        buffer.append("\n");
        buffer.append("ICDDNumDocs" + informacionColeccionDocs.getNumDocs());
        buffer.append("\n");
        buffer.append("ICDDNumTotalPal"+ informacionColeccionDocs.getNumTotalPal());
        buffer.append("\n"); 
        buffer.append("ICDDNumTotalPalSinParada" + informacionColeccionDocs.getNumTotalPalSinParada()); 
        buffer.append("\n");
        buffer.append("ICDDNumTotalPalDiferentes" + informacionColeccionDocs.getNumTotalPalDiferentes());
        buffer.append("\n");
        buffer.append("ICDDTamBytes" + informacionColeccionDocs.getTamBytes());
        buffer.append("\n");
        buffer.append("INFPREGNumTotalPal" + infPregunta.getNumTotalPal()); 
        buffer.append("\n");
        buffer.append("INFPREGTotalPalDiferentes" + infPregunta.getNumTotalPalDiferentes()); 
        buffer.append("\n"); 
        buffer.append("INFPREGTotalPalSinParada" + infPregunta.getNumTotalPalSinParada()); 
        std::ofstream i(this->directorioIndice + "/PrivValues1.txt" );
        if(!i) {
            cerr << "ERROR: No se ha podido escribir en el directorio " <<this->directorioIndice<< " el archivo de guardado del disco: PrivValues1.txt" << endl;
            if( remove( this->ficheroStopWords.c_str() ) != 0 ) cerr<< "Error deleting file" ;
            return false;
        }
        i.write(buffer.data(), buffer.size());
        i.close();
        buffer.clear(); 
        std::stringstream o; 
        o.clear();
        //Ahora guardaremos el resto en PrivValuesMaps.txt
        o<<"INDICE\n";
        for ( auto it = this->indice.begin(); it != this->indice.end(); ++it ){ o<<it->first<<"\t"<<it->second<<'\n'; }
        o<<"INDICEDOCS\n";
        buffer= o.str();
        o.clear(); 
        for ( auto it = this->indiceDocs.begin(); it != this->indiceDocs.end(); ++it ){ o<<it->first<<"\t"<<it->second<<'\n';}
        o<<"INDICEPREGUNTA\n";
        buffer= buffer + o.str();
        o.clear(); 
        for ( auto it = this->indicePregunta.begin(); it != this->indicePregunta.end(); ++it ){ o<<it->first<<"\t"<<it->second<<'\n'; }
        buffer= buffer + o.str();
        o.clear(); 
        i.open(this->directorioIndice + "/PrivValuesMaps.txt");
        if(!i) {
            cerr << "ERROR: No se ha podido escribir en el directorio "<<this->directorioIndice<< " el archivo de guardado del disco: PrivValuesMaps.txt" << endl;
            if( remove( this->ficheroStopWords.c_str() ) != 0 ) cerr<< "Error deleting file" ;
            if( remove( "PrivValues1.txt" ) != 0 ) cerr<< "Error deleting file" ;
            return false;
        }
        i.write(buffer.data() , buffer.size()); 
        buffer.clear();
        i.close(); 
        return true;
    }else {
        std::string buffer;
        buffer.clear();
        std::stringstream o; 
        o.clear();
        o<<"INDICE\n";
        for ( auto it = this->indice.begin(); it != this->indice.end(); ++it ){ o<<it->first<<"\t"<<it->second<<'\n'; }
        buffer = o.str(); 
        std::ofstream i(this->directorioIndice + "/PrivValuesMaps.txt" );
        if(!i) {
            cerr << "ERROR: No se ha podido escribir en el directorio "<< this->directorioIndice <<" el archivo de guardado del disco: PrivValuesMaps.txt" << endl;
            return false;
        }
        i.write(buffer.data(), buffer.size());
        i.close();
        buffer.clear(); 
        return true; 
    }
    

}


bool IndexadorHash::ReadPrivValuesMaps() {
     ifstream file;
    string cadena;
    string fichero = this->directorioIndice+ "/PrivValuesMaps.txt"; 
    file.open(fichero.c_str());
    if(!file) {
        cerr << "ERROR: No existe el archivo: " << this->ficheroStopWords << " En el directorio "<<this->directorioIndice<< endl;
        return false; 
    }
    std::string buffer;
    std::ifstream f(fichero.c_str());
    std::string to; 
    std::string p; 
    p.clear();
    p= "";
    //vamos a configurar el tokenizador para sperar cada una de las dos cosas que componen el mapa
    tok.DelimitadoresPalabra("\t ");//Para tokenizar los indices
	tok.CasosEspeciales(false);
	tok.PasarAminuscSinAcentos(true);
    unordered_map<string , InformacionTermino> totalIndice;
    unordered_map<string , InfDoc> indiceDocsCopia; 
    unordered_map<string , InformacionTerminoPregunta> totalIndicePregunta;
    totalIndice.clear();indiceDocsCopia.clear(); totalIndicePregunta.clear();
    list<string> res; 
    while(std::getline(f,to,'\n')){
        if(to.find("INDICE")==string::npos && to.find("INDICEDOCS")==string::npos && to.find("INDICEPREGUNTA")==string::npos){//SI NO ESTAMOS CAMBIANDO ENTRE UNO Y OTRO
            if(p=="INDICE"){//CUANDO ESTAMOS EN EL PRIMERO
                string it1 = ""; //Primera parte del conjunto (string)
                string it2 = ""; //Segunda parte del  conjunto (<<InformacionTermino)
                if(to.find("\t")!=string::npos){ //STRING 
                    it1 = to.substr(0, to.find("\t"));
                    string p = "\t";
                    it2 = to.substr(to.find("\t") + p.length());
                } 
                if(it2!=""){
                    InformacionTermino itCopia = InformacionTermino();         
                    vector<list<int>> InfTermDocLista; 
                    list<int> ftInfTermDocs; 
                    int iteration=-1; //No hay ninguna iteraci?n al principio disponible
                    list<long int> idsDocs; 
                    tok.Tokenizar(it2,res);//AS? SACAREMOS CADA UNA DE LAS PARTES QUE NECESITAMOS
			        for(list<string>::iterator iter= res.begin();iter!= res.end();++iter){
                        
                        if(*iter!="ft:" && *iter!="Frecuencia total:" && *iter!="fd:" && *iter!="Id.Doc:"){ InfTermDocLista[iteration].push_back(std::stoi(*iter)); }//CUANDO NOS ENCONTRAMOS N?MEROS (ESTAMOS AL FINAL)
                        if(*iter=="ft:"){//Cuando entramos al final de la cadena
                            iter++; 
                            ftInfTermDocs.push_back(std::stol(*iter));    
                        }
                        if(*iter == "Frecuencia total:"){//Sacaremos la frecuencia del doc
                            iter++;
                            itCopia.setFtc(std::stoi(*iter));
                        }
                        if(*iter=="fd:"){//Sacamaos la frecuencia del documento
                            iter++; 
                            itCopia.setFd(std::stoi(*iter));
                        }
                        if(*iter == "Id.Doc:"){//Sacamos el id del documento que va en el unordered map
                            iter++; 
                            iteration++; 
                            idsDocs.push_back(std::stol(*iter)); 
                        }
                    }
                    //Cuando ya hemos salido del bucle, podremos pasar a crear los objetos que nos faltan
                    iteration = -1; 
                    std::list<long int>::iterator it = idsDocs.begin();
                    unordered_map<long int , InfTermDoc> aMeterMap; 
                    std::list<int>::iterator iftDs = ftInfTermDocs.begin();
                    for (; it != idsDocs.end() && iftDs != ftInfTermDocs.end() ; ++it , ++iftDs){//Haremos un bucle que recorra cada idDoc, que coincidir? con el n?mero de fts
                        InfTermDoc infTermCopia = InfTermDoc(); 
                        infTermCopia.setFt(*iftDs);
                        iteration++;
                        //Vamos a llenar la lista del postTerm y a meterla 
                        list<int> PosTerms;
                        for( std::list<int>::iterator itPosTerm =InfTermDocLista[iteration].begin(); itPosTerm!= InfTermDocLista[iteration].end() ; ++itPosTerm) PosTerms.push_back(*itPosTerm);
                        //Metemos la lista en la copia 
                        infTermCopia.setPosTerm(PosTerms);
                        std::pair<long int ,InfTermDoc> c1(*it , infTermCopia );
                        aMeterMap.insert(c1);
                    }   
                    //Una vez que tenemos el unordered_map, ya lo podemos meter en el InformacionTermino
                    itCopia.setL_docs(aMeterMap);
                    //Creamos el para y lo metemos en el ?ndice
                    std::pair<string , InformacionTermino> parejaFinal (it1 , itCopia);
                    //Le metemos en el total
                    totalIndice.insert(parejaFinal);
                }
            }
            if(p=="INDICEDOCS"){//CUANDO ESTAMOS EN EL PRIMERO
                string it1 = ""; //Primera parte del conjunto (string)
                string it2 = ""; //Segunda parte del  conjunto (<<InformacionTermino)
                if(to.find("\t")!=string::npos){ //STRING 
                    it1 = to.substr(0, to.find("\t"));
                    string p = "\t";
                    it2 = to.substr(to.find("\t") + p.length());
                } 
                if(it2!=""){
                    InfDoc itCopia = InfDoc();         
                    tok.Tokenizar(it2,res);//AS? SACAREMOS CADA UNA DE LAS PARTES QUE NECESITAMOS
			        for(list<string>::iterator iter= res.begin();iter!= res.end();++iter){
                        if(*iter=="idDoc:"){ iter++;  itCopia.setIdDoc(std::stol(*iter));    }
                        if(*iter == "numPal:"){ iter++; itCopia.setNumPal(std::stoi(*iter));}
                        if(*iter=="numPalSinParada:"){ iter++; itCopia.setNumPalSinParada(std::stoi(*iter));}
                        if(*iter == "numPalDiferentes:"){ iter++; itCopia.setNumPalDiferentes(std::stoi(*iter)); }
                        if(*iter=="tamBytes:"){ iter++; itCopia.setTamBytes(std::stoi(*iter)); }
                    }
                    std::pair<string , InfDoc> parejaFinal (it1 , itCopia);
                    //Le metemos en el total
                    indiceDocsCopia.insert(parejaFinal);
                }
            }
            if(p=="INDICEPREGUNTA"){
                string it1 = ""; //Primera parte del conjunto (string)
                string it2 = ""; //Segunda parte del  conjunto (<<InformacionTermino)
                if(to.find("\t")!=string::npos){ //STRING 
                    it1 = to.substr(0, to.find("\t"));
                    string p = "\t";
                    it2 = to.substr(to.find("\t") + p.length());
                } 
                if(it2!=""){

                    InformacionTerminoPregunta itCopia = InformacionTerminoPregunta();         
                    list<int> posTerms; 
                    tok.Tokenizar(it2,res);//AS? SACAREMOS CADA UNA DE LAS PARTES QUE NECESITAMOS
			        for(list<string>::iterator iter= res.begin();iter!= res.end();++iter){
                        if(*iter!="ft:" ){posTerms.push_back(std::stoi(*iter)); }//CUANDO NOS ENCONTRAMOS N?MEROS (ESTAMOS AL FINAL)
                        if(*iter=="ft:"){
                            iter++; 
                            itCopia.setFt(std::stoi(*iter));
                        }
                    }
                    itCopia.setPosTerm(posTerms);
                    std::pair<string , InformacionTerminoPregunta> parejaFinal (it1 , itCopia);
                    //Le metemos en el total
                    totalIndicePregunta.insert(parejaFinal);
                }
            }
        }
        //MIRAREMOS SI ESTAMOS EN EL SIGUIETE
        if(to.find("INDICE")!=string::npos) p = "INDICE"; 
        if(to.find("INDICEDOCS")!=string::npos) p = "INDICEDOCS";
        if(to.find("INDICEPREGUNTA")!=string::npos) p = "INDICEPREGUNTA";
    }
    //AHORA METEREMOS LOS ?NDICES
    this->indice = totalIndice;
    this->indiceDocs = indiceDocsCopia;
    this->indicePregunta = totalIndicePregunta;
    file.close(); 
    f.close();
    totalIndice.clear(); 
    indiceDocsCopia.clear(); 
    totalIndicePregunta.clear();
    p.clear();

    return true;
    
}

//M?todo para leer y copiar en las variables lo que hemos guardado en e archivo /PrivValues1.txt
bool IndexadorHash::ReadPrivValues1() {
    ifstream file;
    string cadena;
    string fichero = this->directorioIndice+ "/PrivValues1.txt"; 
    file.open(fichero.c_str());
    if(!file) {
        cerr << "ERROR: No existe el archivo: " << fichero << " En el directorio "<<this->directorioIndice<< endl;
        return false; 
    }
    std::ifstream f(fichero.c_str());
    std::string to; 
    std::string p; 
    p.clear(); 
    while(std::getline(f,to,'\n')){
        if(to.find("PreguntaIndexada") != string::npos) { //Estaremos en el primer caso
            p = "PreguntaIndexada"; 
            this->pregunta.clear();
            this->pregunta = to.substr(to.find(p)+p.length() );
        }
        if(to.find("FicheroStopWords") != string::npos) {
            p = "FicheroStopWords"; 
            this->ficheroStopWords.clear();
            this->ficheroStopWords= to.substr(to.find(p)+p.length() ); 
        }
        if(to.find("tipoStemmer") != string::npos) { 
            p = "tipoStemmer"; 
            this->tipoStemmer= std::stoi(to.substr(to.find(p)+p.length() ));
        }
        if(to.find("Almacena en disco") != string::npos) {
            p = "Almacena en disco"; 
            if( to.substr(to.find(p)+p.length() ) == "true") {this->almacenarEnDisco= true; }
            if(to.substr(to.find(p) +p.length()) == "false"){this->almacenarEnDisco = false;}
            if(to.substr(to.find(p) + p.length())== "1"){this->almacenarEnDisco = true;  }
            if(to.substr(to.find(p)+p.length()) == "0"){this->almacenarEnDisco = false; } 
        }
        if(to.find("Almacena posTerm") != string::npos){
            p= "Almacena posTerm"; 
            if( to.substr(to.find(p)+p.length() ) == "true") {this->almacenarPosTerm= true; }
            if(to.substr(to.find(p) +p.length()) == "false"){this->almacenarPosTerm =  false;}
            if(to.substr(to.find(p) + p.length())== "1"){this->almacenarPosTerm = true;  }
            if(to.substr(to.find(p)+p.length()) == "0"){this->almacenarPosTerm = false; } 
        }
        if(to.find("TOKDelimiters") != string::npos) {
            p = "TOKDelimiters"; 
            this->tok.DelimitadoresPalabra( to.substr(to.find(p)+p.length() ));
        }
        if(to.find("TOKPasarAMinSin") != string::npos) {
            p = "TOKPasarAMinSin"; 
            if( to.substr(to.find(p)+p.length() ) == "true") {this->tok.PasarAminuscSinAcentos(true); }
            if(to.substr(to.find(p) +p.length()) == "false"){this->tok.PasarAminuscSinAcentos(false); }
            if(to.substr(to.find(p) + p.length())== "1"){this->tok.PasarAminuscSinAcentos(true);  }
            if(to.substr(to.find(p)+p.length()) == "0"){this->tok.PasarAminuscSinAcentos(false);  } 
        }
        if(to.find("TOKCasosEspeciales") != string::npos) {
            p = "TOKCasosEspeciales" ;
            if( to.substr(to.find(p)+p.length() ) == "true") {this->tok.CasosEspeciales(true); }
            if(to.substr(to.find(p) +p.length()) == "false"){this->tok.CasosEspeciales(false); }
            if(to.substr(to.find(p) + p.length())== "1"){this->tok.CasosEspeciales(true);  }
            if(to.substr(to.find(p)+p.length()) == "0"){this->tok.CasosEspeciales(false);  } 
        }
        if(to.find("ICDDNumDocs") != string::npos) { 
            p = "ICDDNumDocs"; 
            this->informacionColeccionDocs.setNumDocs(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("ICDDNumTotalPal") != string::npos) { 
            p = "ICDDNumTotalPal"; 
            this->informacionColeccionDocs.setNumTotalPal(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("ICDDNumTotalPalSinParada") != string::npos) { 
            p = "ICDDNumTotalPalSinParada"; 
            this->informacionColeccionDocs.setNumTotalPalSinParada(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("ICDDNumTotalPalDiferentes") != string::npos) { 
            p = "ICDDNumTotalPalDiferentes"; 
            this->informacionColeccionDocs.setNumTotalPalDiferentes(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("ICDDTamBytes") != string::npos) { 
            p = "ICDDTamBytes"; 
            this->informacionColeccionDocs.setTamBytes(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("INFPREGNumTotalPal") != string::npos) { 
            p = "INFPREGNumTotalPal"; 
            this->infPregunta.setNumTotalPal(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("INFPREGTotalPalDiferentes") != string::npos) { 
            p = "INFPREGTotalPalDiferentes"; 
            this->infPregunta.setNumTotalPalDiferentes(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("INFPREGTotalPalSinParada") != string::npos) { 
            p = "INFPREGTotalPalSinParada"; 
            this->infPregunta.setNumTotalPalSinParada(std::stol(to.substr(to.find(p)+p.length() )));
        }
        p.clear();      
    }
    f.close();
    file.close();
}

IndexadorHash::IndexadorHash(const string& directorioIndexacion){
    //LECTURA DEL DIRECTORIO+PRIMERO MIRAMOS QUE EXISTA
    this->directorioIndice = directorioIndexacion; 
    struct stat dir;
    if(stat(this->directorioIndice.c_str(), &dir) == -1 || !S_ISDIR(dir.st_mode) ){
        cerr << "Error :  " << strerror(errno) << endl; 
        return; 
    }
    this->tok = Tokenizador(); //Inicializamos el tokenizador
    this->indice.clear(); 
    this->indiceDocs.clear(); 
    this-> indicePregunta.clear(); 
    this->informacionColeccionDocs =  InfColeccionDocs(); 
    this->infPregunta = InformacionPregunta();  
    try{
        if(!ReadPrivValues1() ){
            this->pregunta=""; 
            this->tok = Tokenizador(); 
            this->tok.CasosEspeciales(false);
            this->tok.DelimitadoresPalabra(" "); 
            this->ficheroStopWords="";
            this->tipoStemmer= 0; 
            this->almacenarEnDisco=true; 
            this->almacenarPosTerm=false;
        }
    }catch(...){
        cerr<<"Los par�metros que se han introducido no son correctos y no se ha podido generar el objeto de indexaci�n"<<endl;
        return; 
    }
    try{
        if(this->ficheroStopWords!="") ReadStopWords(); 
    }catch(...){
        cerr<<"No se ha podido leer el ficheroStopWords"<<endl;
        return; 
    }
    try{
        if(!ReadPrivValuesMaps() ){
            cerr<<"Error al leer el archivo con los �ndices"<<endl;
            return;
        }
    }catch(...){
        cerr<<"Los par?metros que se han introducido no son correctos y no se ha podido generar el objeto de indexaci?n"<<endl;
        return; 
    } 
    
}

/**
 * M�todo par apasar a min�sculas sin acentos
 * 
 * */
char * IndexadorHash::pasarAMinSin(char * cadena) const{
    int caracter= 0;
    for(int i=0 ; i< strlen(cadena) ; i++){
        //Cambiaremos cada uno de los car?cteres a entero
        caracter = (int) cadena[i];
        if( caracter <0 ) { 
            //Si es <0, entonces lo que haremos ser? compararlo con cada uno de los casos
            caracter = caracter+ 256;
            if(caracter==224 || caracter == 225 || caracter== 192 || caracter== 193) cadena[i] = (char)97; //a
            if(caracter==232 || caracter==233 || caracter==201 || caracter==200)  cadena[i] =(char)101; //e
            if(caracter==237 || caracter == 236 || caracter==204 || caracter==205) cadena[i]= (char)105; //i
            if(caracter== 242 ||caracter == 243 || caracter == 210 || caracter == 211) cadena[i] = (char)111; //o
            if(caracter==250 || caracter == 249 || caracter==217 || caracter==218) cadena[i] = (char)117; //u
            if( caracter==209 ) cadena[i] = (char)241; //?
        }else{
            cadena[i] =std::tolower(cadena[i]);
        }
    }
    return cadena;
}



/**
 * M�todo para comprobar si tenemos indexada una palabra 
 * 
 * */
bool IndexadorHash::Existe(const string& word) const{//Tendremos que aplicar el stemmer a la plabra y luego buscarla 
    if(this->tipoStemmer == 0){
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str())); 
        for(auto iterator = indice.begin() ; iterator!=indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) return true; 
        }
        return false; 
    } 
    if(this->tipoStemmer == 1 ) { //Habr� que usar el stemmer en espa�ol
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 1); 
        for(auto iterator = indice.begin() ; iterator!=indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) return true; 
        }
        return false; 
    }
    
    if(this->tipoStemmer==2){
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str())); 
        s.stemmer(copia , 2); 
        for(auto iterator = indice.begin() ; iterator!=indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) return true; 
        }
        return false; 
    }
    return false;

}




bool IndexadorHash::Devuelve(const string& word, const string& nomDoc, InfTermDoc& InfDocSalida) const{
    //Miraremos el tipo de stemming 
    if(this->tipoStemmer == 0){
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str())); 
        for(std::pair<std::string, InformacionTermino> iterator: this->indice ){
            if(iterator.first == word|| iterator.first == copia){
                //Si hemos encontrado la palabra indexada, buscaremos que ese mismo contenga el nombre del documento
                for(std::pair<std::string, InfDoc > it2: this->indiceDocs){ //Miramos si el documento est� dentro Y COINCIDE CON EL DOCUMENTO QUE QUEREMOS
                    if(it2.first == nomDoc){ //Miramos si coinciden los nombres (del documento iterado y del documento que me han pasado de entrada)
                        //Miraremos para cada uno de los documentos, si su id coincide con los documentos que hay indexados en el �ndice del t�rmino, de manera que si coinciden, ya sabremos que es el bueno
                        //cogeremos cada uno de los documentos
                        for(auto it3 = iterator.second.getL_docs().begin() ; it3!=iterator.second.getL_docs().end() ; it3 ++) { //Con esto vamos sacando cada una de las iteraciones que est�n detntro del t�rmino
                            if ( it3->first == it2.second.getIdDoc() ) {
                                InfDocSalida = it3->second; 
                                return true; 
                            }
                        }
                    }
                }
            } 
        }
        return false; 
    }
    if( this->tipoStemmer == 1) {
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 1); 
        for(std::pair<std::string, InformacionTermino> iterator: this->indice ){
            if(iterator.first == word|| iterator.first == copia){
                //Si hemos encontrado la palabra indexada, buscaremos que ese mismo contenga el nombre del documento
                for(std::pair<std::string, InfDoc > it2: this->indiceDocs){ //Miramos si el documento est� dentro Y COINCIDE CON EL DOCUMENTO QUE QUEREMOS
                    if(it2.first == nomDoc){ //Miramos si coinciden los nombres (del documento iterado y del documento que me han pasado de entrada)
                        for(auto it3 = iterator.second.getL_docs().begin() ; it3!=iterator.second.getL_docs().end() ; it3 ++) { //Con esto vamos sacando cada una de las iteraciones que est�n detntro del t�rmino
                            if ( it3->first == it2.second.getIdDoc() ) { //Comprobamos que coincidan los ids(para saber si es el mismo documento)
                                InfDocSalida = it3->second; 
                                return true; 
                            }
                        }
                    }
                }
            } 
        }
        return false;  
    }
    if(this->tipoStemmer == 2 ){
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 2); 
        for(std::pair<std::string, InformacionTermino> iterator: this->indice ){
            if(iterator.first == word|| iterator.first == copia){
                //Si hemos encontrado la palabra indexada, buscaremos que ese mismo contenga el nombre del documento
                for(std::pair<std::string, InfDoc > it2: this->indiceDocs){ //Miramos si el documento est� dentro Y COINCIDE CON EL DOCUMENTO QUE QUEREMOS
                    if(it2.first == nomDoc){ //Miramos si coinciden los nombres (del documento iterado y del documento que me han pasado de entrada)
                        for(auto it3 = iterator.second.getL_docs().begin() ; it3!=iterator.second.getL_docs().end() ; it3 ++) { //Con esto vamos sacando cada una de las iteraciones que est�n detntro del t�rmino
                            if ( it3->first == it2.second.getIdDoc() ) { //Comprobamos que coincidan los ids(para saber si es el mismo documento)
                                InfDocSalida = it3->second; 
                                return true; 
                            }
                        }
                    }
                }
            } 
        }
        return false; 
    }
    return false;
}

/**
 * Cuando devolvemos el �ndice
 * */ 
bool IndexadorHash::Devuelve(const string& word, InformacionTermino& inf) const{
    if(this->tipoStemmer == 0){
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str())); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia){
                inf = iterator->second;
                return true;
            } 
        }
        return false; 
    }
    if( this->tipoStemmer == 1) {
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 1); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                inf= iterator->second; 
                return true; 
            }
        }
        return false; 
    }
    if(this->tipoStemmer == 2 ){
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 2); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                inf= iterator->second; 
                return true; 
            }
        }
        return false; 
    }
}

bool IndexadorHash::Inserta(const string& word, const InformacionTermino& inf){
    if(this->tipoStemmer == 0){
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str())); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia){
               return false; 
            } 
        }
        std::pair<string, InformacionTermino> pareja (copia , inf);
        this->indice.insert(pareja);
        return true; 
    }
    if( this->tipoStemmer == 1) {
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 1); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                return false; 
            }
        }
        std::pair<string, InformacionTermino> pareja (copia , inf);
        this->indice.insert(pareja);
        return true; 
    }
    if(this->tipoStemmer == 2 ){
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 2); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                return false; 
            }
        }
        std::pair<string, InformacionTermino> pareja (copia , inf);
        this->indice.insert(pareja);
        return true; 
    }
}

/**
 * M�todo para actualizar
 * 
 * */ 
bool IndexadorHash::Actualiza(const string& word, const InformacionTermino& inf){
    //Miraremos si el t�rmino est� indexado
    if(this->tipoStemmer == 0){
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str())); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia){
                iterator->second = inf; 
                return true;
            } 
        }
        return false; 
    }
    if( this->tipoStemmer == 1) {
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 1); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                iterator->second = inf; 
                return true; 
            }
        }
        return false; 
    }
    if(this->tipoStemmer == 2 ){
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 2); 
        for(auto iterator = this->indice.begin() ; iterator!=this->indice.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                iterator->second = inf; 
                return true; 
            }
        }
        return false; 
    }
}

void IndexadorHash::ListarPalParada() const{
    //Recorreremos la lista y la imprimiremo por pantalla 
    for(auto iterator = this->stopWords.begin(); iterator != this->stopWords.end() ; iterator++){
        cout<<*iterator<<endl;
    }
}
int IndexadorHash::NumPalIndexadas() const{
    return this->indice.size();
}
string IndexadorHash::DevolverFichPalParada () const{
    return this->ficheroStopWords;
}
int IndexadorHash::DevolverTipoStemming () const{ 
    return this->tipoStemmer;
}
int IndexadorHash::NumPalParada() const{
    return this->stopWords.size(); 
}
string IndexadorHash::DevolverDelimitadores () const{
    return this->tok.DelimitadoresPalabra();
}
bool IndexadorHash::DevolverCasosEspeciales () const{
    return this->tok.CasosEspeciales();
}
bool IndexadorHash::DevolverPasarAminuscSinAcentos () const{
    return this->tok.PasarAminuscSinAcentos();
}
bool IndexadorHash::DevolverAlmacenarPosTerm () const{
    return this->almacenarPosTerm;
}
string IndexadorHash::DevolverDirIndice () const{
    return this->directorioIndice;
}
bool IndexadorHash::DevolverAlmEnDisco () const{
    return this->almacenarEnDisco;
}
void IndexadorHash::ListarInfColeccDocs() const{
    cout<<this->informacionColeccionDocs<<endl;
}
void IndexadorHash::ListarTerminos() const{
    for(std::pair<std::string , InformacionTermino> iterator : this->indice){
        cout<<iterator.first<<'\t'<<iterator.second<<endl;
    }
}
bool IndexadorHash::ListarTerminos(const string& nomDoc) const{
    for(std::pair<std::string, InfDoc > it: this->indiceDocs) {
        if(it.first == nomDoc){//Si es el mismo documento
            //Tendremos que coger su id y borrarlo de todos los �ndices que lo contengan 
            for(std::pair<std::string , InformacionTermino> it2 : this->indice){
                //Tendremos que mirar si contienen el id 
                for(std::pair<long int , InfTermDoc > it3 : it2.second.getL_docs()) {
                    if(it3.first == it.second.getIdDoc()){ // Si coinciden los ids del documento
                        cout << it3.first << '\t' << it3.second << endl;
                    }
                } 
            }
            return true; 
        }
    }
    return false; 
}
void IndexadorHash::ListarDocs()const {
    for(std::pair<std::string , InfDoc> it : this->indiceDocs){
        cout << it.first << '\t' <<it.second<< endl; 
    }
}
bool IndexadorHash::ListarDocs(const string& nomDoc) const{
    for(std::pair<std::string , InfDoc> it : this->indiceDocs){
        if(it.first == nomDoc){
            cout << it.first << '\t' <<it.second<< endl; 
            return true; 
        }
    }
    return false;
}
/***
 * Miraremos si hay alguna pregunta indexada y la devolveremos en preg 
 * 
 * */ 
bool IndexadorHash::DevuelvePregunta(string& preg) const{
    if(indicePregunta.size() > 0 ){
        preg = this->pregunta; //Cogemos la pregunta que tengamos indexada actualmente.
        return true; 
    }
    return false;
}

/**
 * M�tdo para devolver el infPregunta de una pregunta que tengamos almacenada
 * */
bool IndexadorHash::DevuelvePregunta(const string& word, InformacionTerminoPregunta& inf) const{
    if(this->tipoStemmer == 0){
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str())); 
        for(auto iterator = this->indicePregunta.begin() ; iterator!=this->indicePregunta.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia){
                inf = iterator->second;
                return true;
            } 
        }
        return false; 
    }
    if( this->tipoStemmer == 1) {
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 1); 
        for(auto iterator = this->indicePregunta.begin() ; iterator!=this->indicePregunta.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                inf= iterator->second; 
                return true; 
            }
        }
        return false; 
    }
    if(this->tipoStemmer == 2 ){
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 2); 
        for(auto iterator = this->indicePregunta.begin() ; iterator!=this->indicePregunta.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                inf= iterator->second; 
                return true; 
            }
        }
        return false; 
    }
}

/**
 * M�todo para mirar si hay una pregunta indexada y devolverla 
 *
 * */
bool IndexadorHash::DevuelvePregunta(InformacionPregunta& inf) const{
    if(this->pregunta.length()>0 || this->indicePregunta.size() >0){
        inf = this->infPregunta; 
        return true; 
    }
    return false;

}
/**
 * M�todo para borrar una palabra del �ndice con todas las palabras indexadas
 * 
 * */ 
bool IndexadorHash::Borra(const string& word){
    //Miraremos si esa palabra despu�s de aplicarle el stemming est� indexada
    if(this->tipoStemmer == 0){
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str())); 
        for(auto iterator = this->indicePregunta.begin() ; iterator!=this->indicePregunta.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia){
                this->indice.erase(iterator->first );
            } 
        }
        return false; 
    }
    if( this->tipoStemmer == 1) {
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 1); 
        for(auto iterator = this->indicePregunta.begin() ; iterator!=this->indicePregunta.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                this->indice.erase(iterator->first);
                return true; 
            }
        }
        return false; 
    }
    if(this->tipoStemmer == 2 ){
        stemmerPorter s = stemmerPorter() ;
        char* copia = pasarAMinSin(const_cast<char*>(word.c_str()));  
        s.stemmer(copia , 2); 
        for(auto iterator = this->indicePregunta.begin() ; iterator!=this->indicePregunta.end() ; iterator ++ ){
            if(iterator->first == word|| iterator->first == copia) {
                this->indice.erase(iterator->first);
                return true; 
            }
        }
        return false; 
    } 
}

/**
 * M�todo para borrar documentos
 * 
 * */
bool IndexadorHash::BorraDoc(const string& nomDoc){
    //Primero miraremos si tenemos el documento indexado 
    for(std::pair<std::string, InfDoc > it: this->indiceDocs) {
        if(it.first == nomDoc){//Si es el mismo documento
            //Tendremos que coger su id y borrarlo de todos los �ndices que lo contengan 
            for(std::pair<std::string , InformacionTermino> it2 : this->indice){
                //Tendremos que mirar si contienen el id 
                for(std::pair<long int , InfTermDoc > it3 : it2.second.getL_docs()) {
                    if(it3.first == it.second.getIdDoc()){ // Si coinciden los ids del documento
                        //Tendremos que actualizar las frecuencias
                        it2.second.setFd(it2.second.getFd() -1 ); 
                        it2.second.setFtc(it2.second.getFtc() - it3.second.getFt());
                        it2.second.setFt(it2.second.getFt() - it3.second.getFt());
                        //Miraremos si conten�a m�s documentos
                        unordered_map<long int , InfTermDoc > copia = it2.second.getL_docs();
                        copia.erase(it3.first);
                        it2.second.setL_docs(copia); 
                        if(it2.second.getL_docs().size()==0){
                            this->indice.erase(it2.first);
                        }
                        break; 
                    }

                } 
                
            }
            //Ahora borraremos el documento
            
	        informacionColeccionDocs.setTamBytes(informacionColeccionDocs.getTamBytes()-it.second.getTamBytes());
		    informacionColeccionDocs.setNumTotalPal(informacionColeccionDocs.getNumTotalPal()-it.second.getNumPal());
		    informacionColeccionDocs.setNumTotalPalSinParada(informacionColeccionDocs.getNumTotalPalSinParada()-it.second.getNumPalSinParada());
		    informacionColeccionDocs.setNumTotalPalDiferentes(informacionColeccionDocs.getNumTotalPalDiferentes()-it.second.getNumPalDiferentes());
            this->indiceDocs.erase(it.first);
            informacionColeccionDocs.setNumDocs(informacionColeccionDocs.getNumDocs()-1);
            return true; 
        }
    }
    return false;
    


 }

 void IndexadorHash::VaciarIndiceDocs(){
    this->indiceDocs.clear(); 
 }

 void IndexadorHash::VaciarIndicePreg(){
    this->pregunta.clear(); 
    this->indicePregunta.clear(); 
 }



/**
 * M�todo que usaremos para indexar documentos
 * 
 * */
bool IndexadorHash::IndexarUnDocu(const char * fichero , InfDoc & actual){
    struct stat propiedadesDoc; //Lo usaremos para poder sacar los valores del tama�o en bytes
    if( stat(this->directorioIndice.c_str(), &propiedadesDoc) == -1 || !S_ISDIR(propiedadesDoc.st_mode) ) {
        ifstream file; 
        string ext = ".tk";
        string ficheroSalida = fichero + ext;
        
        
        this->tok.Tokenizar(fichero , ficheroSalida); //Los tokens se habr�n guardado en el fichero de salida, y ser� de ah� de donde los cogeremos
        file.open(ficheroSalida);
        if(!file) return false; //No mostramos mensaje de error porque se supone que el archivo lo acabamos de crear  
        std::ifstream f(ficheroSalida);
        std::string to; 
        actual.setTamBytes( propiedadesDoc.st_size); //Ponemos el tama�o de los bytes
        actual.setNumPal(0); actual.setNumPalSinParada(0); actual.setNumPalDiferentes(0); 
        InfTermDoc elNuevo = InfTermDoc(); 
        elNuevo.setFt(0);
        list<string> palabrasEncontradas; 
        palabrasEncontradas.clear();
        //Iremos por cada token creado en el archivo
        while(std::getline(f,to,'\n')){
            std::list<std::string>::iterator iteradorPalabras;
            iteradorPalabras= std::find(palabrasEncontradas.begin(), palabrasEncontradas.end(), to); //Si no estaba (SER� UNA PALABRA DIFERENTE)
            if(iteradorPalabras == palabrasEncontradas.end()){
                palabrasEncontradas.insert(iteradorPalabras, to); //Insertamos al final 
            }
            bool eraPalabraParada= false;
            for(auto iterator = this->stopWords.begin() ; iterator!= stopWords.end() ; iterator ++){
                if(*iterator == to){ //Hemos encontrado una stopWord
                    //Sumaremos +1 al n�mero de palabras que nos hemos encontrado en el documento
                    actual.setNumPal(actual.getNumPal() + 1);
                    
                    eraPalabraParada = true;
                    break;
                }
            }
            if(!eraPalabraParada){ //Como no era una palabra de parada, pues miramos si estaba ya o no indexada
                //Miramos si estaba ya indexada (Como quiero hacerlo eficiente, no utilizo la funci�n Existe para mirar esto)
                if(tipoStemmer == 1 ){
                    stemmerPorter s = stemmerPorter(); 
                    s.stemmer(to , 1); 
                }
                if(tipoStemmer == 2 ){
                    stemmerPorter s = stemmerPorter(); 
                    s.stemmer(to , 2); 
                }
                bool encontradoEnIndice = false; 
                for( std::pair<std::string, InformacionTermino> iterator : this->indice){
                    if(iterator.first == to ){ //NOS HEMOS ENCONTRADO CON EL DOCUMENTO INDEXADO 
                        iterator.second.setFtc(iterator.second.getFtc() +1 );
                        iterator.second.setFt(iterator.second.getFt() + 1 );
                        elNuevo.setFt(elNuevo.getFt() + 1 );
                        if( this->almacenarPosTerm ==true ) { //Si tenemos que almacenar las posiciones
                            list<int> copia = elNuevo.getPosTerm(); 
                            copia.push_back(actual.getNumPal());//Metemos el n�mero de palabras que nos hemos encontrado
                            elNuevo.setPosTerm(copia);
                        }
                        //Ahora, meteremos el par dentro del iterador
                        std::pair<long int , InfTermDoc > added (actual.getIdDoc() , elNuevo); 
                        unordered_map<long int , InfTermDoc> copia = iterator.second.getL_docs(); 
                        copia.insert(added);
                        iterator.second.setL_docs(copia);
                        encontradoEnIndice = true; 
                        break; 
                    }
                }
                if(!encontradoEnIndice){ //Cuando no estaba en el �ndice
                    InformacionTermino nuevoTermino = InformacionTermino(); 
                    nuevoTermino.setFt(1); 
                    nuevoTermino.setFtc(1);
                    elNuevo.setFt(1);
                    if(almacenarPosTerm == true){
                        list<int> copia = elNuevo.getPosTerm(); 
                        copia.push_back(actual.getNumPal());//Metemos el n�mero de palabras que nos hemos encontrado
                        elNuevo.setPosTerm(copia);
                    }
                    //Crearemos un par para a�adir al �ndice
                    std::pair<long int , InfTermDoc > added (actual.getIdDoc() , elNuevo); 
                    unordered_map<long int , InfTermDoc> listaNueva;
                    listaNueva.insert(added);
                    nuevoTermino.setL_docs(listaNueva);
                    std::pair<std::string , InformacionTermino> pareja (to , nuevoTermino);
                    this->indice.insert(pareja);
                    
                }
                actual.setNumPalSinParada(actual.getNumPalSinParada() + 1 );
                actual.setNumPal(actual.getNumPal() + 1 );
            }
        
        }
		//Tendremos que buscar las palabras que son diferentes del documento 
        /*long int counter = 0; 
	    unordered_map<long int,InfTermDoc> copiaLista;
	    for(auto iterator=indice.begin();iterator!=indice.end();++iterator){ //Por cada t�rmino indexado, buscaremos aquel que contenga el documento que estamos buscando
		    copiaLista=iterator->second.getL_docs();
		    for( std::pair< long int , InfTermDoc>it2 : copiaLista ) {
                if(it2.first == actual.getIdDoc()) {
                    counter++;
                }
            }
	    }*/
        actual.setNumPalDiferentes(palabrasEncontradas.size()); 
        std::pair<string , InfDoc> result (to , actual); 
        indiceDocs.insert(result);
        //ACTUALIZAMOS EL RESTO
        informacionColeccionDocs.setNumDocs(informacionColeccionDocs.getNumDocs()+1);
        informacionColeccionDocs.setNumTotalPalDiferentes(indice.size());
        informacionColeccionDocs.setNumTotalPal(informacionColeccionDocs.getNumTotalPal()+actual.getNumPal());
        informacionColeccionDocs.setTamBytes(informacionColeccionDocs.getTamBytes()+actual.getTamBytes());
        informacionColeccionDocs.setNumTotalPalSinParada(informacionColeccionDocs.getNumTotalPalSinParada()+actual.getNumPalSinParada());
        file.close(); 
        palabrasEncontradas.clear(); 
        
        return true; 
    }
    return false; 

}

/**
 * M?todo para indexar documentos
 * 
 * */
bool IndexadorHash::Indexar(const string& ficheroDocumentos){
    //Lo primero que haremos ser? abrir el documentos para poder ir leyendo
    ifstream file;
    string cadena;
    file.open(ficheroDocumentos.c_str());
    if(!file) {
        cerr << "ERROR: No existe el archivo: "<<ficheroDocumentos<< endl; 
        return false; 
    }
    std::ifstream f(ficheroDocumentos.c_str());
    std::string to; 
	tok.CasosEspeciales(false);
	tok.PasarAminuscSinAcentos(true);
    list<string> res; 
    while(std::getline(f,to,'\n')){ //RECORREREMSO CADA UNA DE LAS L?NEAS
        //COSAS A TENER EN CUENTA
        /**
         * 1. Mirar si ese documento ya est� indexado, de manera que si lo est�, no tendremos que indexarlo nuevamente, solamente tendremos que modificar su frecuencia
         * */
        bool estabaIndexado = false;
        for(auto iterator= indiceDocs.begin() ; iterator!=indiceDocs.end() ; iterator++){
            if(iterator->first == to ){ //Si ya est� indexado, haremos sus respectivas modificaciones
                //Compararemos las fechas de los documentos
                estabaIndexado=true;
                Fecha copiaFechaIterator = iterator->second.getFechaModificacion(); 
                Fecha f =Fecha();  //Creamos una nueva fecha con la hora local del sistema
                if( f == copiaFechaIterator || f<copiaFechaIterator) break; //No haremos nada 
                if(f > copiaFechaIterator ){ //Cuando f es m�s actual que la fecha que hab�a metida
                    //Borraremos e indexaremos nuevamente el documento
                    auto copia = iterator; 
                    indiceDocs.erase(iterator);
                    try{
                        if(!IndexarUnDocu(const_cast<char *>(copia->first.c_str())  , copia->second )) return true; 
                    }catch(...){
                        cerr << "ERROR: No hay espacio suficiente en memoria para la indexaci�n"<<endl;
                        return false; 
                    }
                    break; 
                }
                //break;
            } 
        }
        if(!estabaIndexado){
            try{
                InfDoc actual = InfDoc(); 
                actual.setIdDoc(indiceDocs.size() + 1 );
                if(!IndexarUnDocu(const_cast<char*>(to.c_str()), actual)) return true;
            }catch(...){
                cerr << "ERROR: No hay espacio suficiente en memoria para la indexaci�n"<<endl;
                return false; 
            }

        } 
    
    }
    f.close(); 
    file.close();
    to.clear();
    return true; 


}

string IndexadorHash::IndexarSubDirectorios(const string & secondDir ){
    DIR* dirp = opendir(secondDir.c_str());
    struct dirent * dp;
    list<char*> ficheros; 
    while ((dp = readdir(dirp)) != NULL) {
        ficheros.push_back(dp->d_name);
    }
    closedir(dirp);
    struct stat dir;
    std::string buffer; 
    buffer.clear(); 
    for (std::list<char*>::iterator it=ficheros.begin(); it != ficheros.end(); ++it){
        //Miraremos si es un fichero, porque en el caso de que lo sea, tendremos que a�adir nuevos documentos
        if( stat( *it,& dir) == 0 ){
            if( dir.st_mode & S_IFDIR ){ //it's a directory
                //Si es un directorio tendreos que indexar nuevamente el directorio, y para ello, pues llamaremos de nuevo a la funci�n de indexar directorio
                buffer.append(IndexarSubDirectorios(secondDir + '/' + *it   )); 
            }
            else if( dir.st_mode & S_IFREG ){//it's a file
                //Si es un archivo, podremos meterlo en un archivo de indexaci�n con el que llamaremos al nuevo Indexar
                buffer.append(secondDir + '/' + *it);
                buffer.append("\n");
            }
        }
    }
    return buffer;
}
/**
 * M�todo para indexar un directorio 
 * HAREMOS USO DEL INDEXADOR DE UN FICHERO CON NOMBRES DE DOCUMENTOS 
 * */ 
bool IndexadorHash::IndexarDirectorio(const string& dirAIndexar){
    

    DIR* dirp = opendir(dirAIndexar.c_str());
    struct dirent * dp;
    list<char*> ficheros; 
    while ((dp = readdir(dirp)) != NULL) {
        ficheros.push_back(dp->d_name);
    }
    closedir(dirp);
    struct stat dir;
    std::string buffer; 
    buffer.clear(); 
    for (std::list<char*>::iterator it=ficheros.begin(); it != ficheros.end(); ++it){
        //Miraremos si es un fichero, porque en el caso de que lo sea, tendremos que a�adir nuevos documentos
        if( stat( *it,& dir) == 0 ){
            if( dir.st_mode & S_IFDIR ){ //it's a directory
                //Si es un directorio tendreos que indexar nuevamente el directorio, y para ello, pues llamaremos de nuevo a la funci�n de indexar directorio
                buffer.append(IndexarSubDirectorios(dirAIndexar + '/' + *it)); 
            }
            else if( dir.st_mode & S_IFREG ){//it's a file
                //Si es un archivo, podremos meterlo en un archivo de indexaci�n con el que llamaremos al nuevo Indexar
                buffer.append(*it);
                buffer.append("\n");
            }
        }
    }
    //Ahora escribiremos en el archivo de destino
    std::ofstream i("FicheroConNombreDocus.txt");
    if(!i) {
        cerr<<"Error, no se ha podido realizar la indexaci�n del directorio"<<endl;
        return false;
    }
    i.write(buffer.data(), buffer.size());
    this->Indexar("FicheroConNombreDocus.txt");
    return true;
    
}






bool IndexadorHash::IndexarUnTermPregunta(const string & preg , InformacionTerminoPregunta & term ){
    
    if( this->almacenarPosTerm ==true ) { //Si tenemos que almacenar las posiciones
        list<int> copia = term.getPosTerm();
        copia.push_back(this->infPregunta.getNumTotalPal());//Metemos el n�mero de palabras que nos hemos encontrado
        term.setPosTerm(copia);                
    }
    term.setFt(term.getFt() +1 ); 
    
    //Crearemos el par y lo indexaremos
    std::pair<string , InformacionTerminoPregunta> copia (preg , term ); 
    this->indicePregunta.insert(copia);// Metemos la nueva pregunta indexada
    return true; 

}

/**
 * M�todo para indexar una pregunta
 * 
 * */
bool IndexadorHash::IndexarPregunta(const string& preg){
    //HAREMOS UN BUCLE PAR A IR MIRANDO CADA UNA DE LAS PALABRAS QUE CONFORMAN LA PREGUNTA(DESPU�S DE HABERLAS TOKENIZADO)
    this->tok.PasarAminuscSinAcentos(true ); 
    this->tok.CasosEspeciales(false); 
    list<string > tokens; //PALABRAS RESULTANTES
    this->tok.Tokenizar(preg, tokens ); 
    this->pregunta.clear();
    this->pregunta = preg;
    this->infPregunta.~InformacionPregunta(); // Borramos todo lo que hab�a de la informaci�n de la pregunta
    this->indicePregunta.clear(); 
    //Ahora recorreremos la lista y miraremos si estaba ya indexada y dem�s 
     for (std::list<string>::iterator iterPalabraPregunta=tokens.begin(); iterPalabraPregunta != tokens.end(); ++iterPalabraPregunta){
        //Lo primero que haremos ser� aplicar el stemming, dado que las palabras que tengamos indexadas vendr�n ya con el stemming aplicado
        //APLICAREMOS EL TIPO DE STEMMING? 
        if(this->tipoStemmer == 1 ){
            stemmerPorter s = stemmerPorter() ;
            s.stemmer(*iterPalabraPregunta, 1); 
        } 
        if(this->tipoStemmer == 2 ){
            stemmerPorter s = stemmerPorter() ;
            s.stemmer(*iterPalabraPregunta, 2);  
        } 
        //Ahora miraremos si ya estaba indexado
        bool estabaIndexado = false;
        for(auto iterator= indicePregunta.begin() ; iterator!=indicePregunta.end() ; iterator++){
            if(iterator->first == *iterPalabraPregunta ){ //Si ya est� indexado, haremos sus respectivas modificaciones
                estabaIndexado=true;
                auto copia = iterator; 
                indicePregunta.erase(iterator);
                try{
                   IndexarUnTermPregunta( *iterPalabraPregunta  , copia->second); 
                }catch(...){ //CUANDO NO TENEMOS ESPACIO EN MEMORIA
                    cerr << "ERROR: No hay espacio suficiente en memoria para la indexaci�n"<<endl;
                    return false; 
                }
                break; 
            }
                //break;
        } 
        if(!estabaIndexado){
            bool esPalabraParada = false; 
            //Miraremos si es una palabra de parada
            for(auto iterator= this->stopWords.begin() ; iterator!= this->stopWords.end() ; iterator++){
                if(*iterator == *iterPalabraPregunta ){
                    //Si es una palabra de parada, simplemente sumaremos al total 
                    this->infPregunta.setNumTotalPal(this->infPregunta.getNumTotalPal() + 1 ); 
                    esPalabraParada = true; 
                    break; 
                }
            }
            if(!esPalabraParada){
                this->infPregunta.setNumTotalPalSinParada(this->infPregunta.getNumTotalPalSinParada() + 1 ); 
                this->infPregunta.setNumTotalPal(this->infPregunta.getNumTotalPal() + 1 ); 
                InformacionTerminoPregunta p = InformacionTerminoPregunta(); 
                try{
                    IndexarUnTermPregunta(*iterPalabraPregunta, p );
                }catch(...){
                    cerr << "ERROR: No hay espacio suficiente en memoria para la indexaci�n"<<endl;
                    return false; 
                }
            }

        } 
    
    }
    this->infPregunta.setNumTotalPalDiferentes(this->indicePregunta.size() ); 
    return true;


}










