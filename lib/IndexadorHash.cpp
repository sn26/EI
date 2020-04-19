#include "IndexadorHash.h"
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 

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
	tok.PasarAminuscSinAcentos(false);
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
        cerr<<"Los par?metros que se han introducido no son correctos y no se ha podido generar el objeto de indexaci?n"<<endl;
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
            cerr<<"Error al leer el archivo con los ?ndices"<<endl;
            return;
        }
    }catch(...){
        cerr<<"Los par?metros que se han introducido no son correctos y no se ha podido generar el objeto de indexaci?n"<<endl;
        return; 
    } 
    
}