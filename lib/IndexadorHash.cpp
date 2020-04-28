#include "IndexadorHash.h"
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <dirent.h>


//Ser? privado porque no se permitir? al usuario usarlo y lo implementaremos a nuestra manera
IndexadorHash::IndexadorHash(){
    this->pregunta=""; 
    this->tok = Tokenizador();     
    this->tok.PasarAminuscSinAcentos(true);    
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
    //this->informacionColeccionDocs.~InfColeccionDocs(); 
    this->pregunta.clear(); this->pregunta= ""; 
    //this->infPregunta.~InformacionPregunta();
    this->stopWords.clear(); 
    this->ficheroStopWords.clear(); this->ficheroStopWords="";
    this->directorioIndice.clear(); this->directorioIndice="";
    //this->tok.~Tokenizador(); 
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
    //(*this) = copia;
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
    if(dirIndice.length() == 0 ) this->directorioIndice = "."; 
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



/**
 * Método para guardar en disco cuando almDicso==true
 * 
 * */
/*
bool IndexadorHash::guardarEnDisco(){
     char * ficheroSAL = new char(); 
    string concat = "/PrivValuesIND.txt";
    ficheroSAL = const_cast<char*>((this->directorioIndice + concat).c_str());
    FILE* fp = fopen( ficheroSAL,"wb");
    auto t0 = this->indice.size(); 
    //cout<<"ENTRANDO"<<endl;
    fwrite(&t0 , sizeof(t0) , sizeof(t0), fp ); 
    //cout<<"ENTRANDO"<<endl;
    for ( auto it = this->indice.begin(); it != this->indice.end(); it++ ){ 
        fwrite(&it->first , sizeof(string ) , it->first.length()+1 , fp  ); 
        //Recorreremos todo lo del segundo iterador
        auto t = it->second.getFd(); 
        fwrite(&t ,sizeof(it->second.getFd()) ,1 , fp );
        t = it->second.getFt();
        fwrite(&t , sizeof(t) ,1 , fp );
        t= it->second.getFtc();
        fwrite(&t , sizeof(t) , 1 , fp);
        t= it->second.getL_docs().size(); //Para saber cuántos documentos lo contienen
        fwrite(&t , sizeof(t) ,1 , fp );
        
        //Ahora guardaremos las pos term y los id del doc
        for(auto it2 = it->second.getL_docs().begin() ; it2 != it->second.getL_docs().end() ; it2++){
            //POR CADA UNO DE LOS TÉRMINOS, PONDREMOS: NUMTOTALDOCS(PARA AL LEER PODER ITERAR)  - ID.DOC -  FT - ALMACENARPOSTERM - NUMTOTALPOS(PARA AL LEER PODER ITERAR) - POS 
            auto t2 = it2->first; 
            fwrite(&t2 , sizeof(t2) , 1 , fp); 
            t2 = it2->second.getFt(); 
            fwrite(&t2 , sizeof(t2) , 1 , fp); 
            t2 = std::stoi(to_string(this->almacenarPosTerm)); 
            fwrite(&t2 , sizeof(t2) , 1 , fp); 
            //Ahora almacenaremos las pos si las hay 
            if(this->almacenarPosTerm ){
                t2= it2->second.getPosTerm().size(); 
                fwrite(&t2 , sizeof(t2) , 1 , fp); 
                for(auto it3 = it2->second.getPosTerm().begin() ; it3 != it2->second.getPosTerm().end() ; it3 ++){
                    t2= *it3;
                    fwrite(&t2 , sizeof(t2)  , 1 , fp);
                }
            }


        }
        
    }
    fclose(fp);
    /*fp = fopen(this->directorioIndice.c_str() + '/PrivValuesDOCS.txt',"wb");
    for(auto it = this->indiceDocs.begin() ; it!= this->indiceDocs.end() ; it++){
        //Tendremos que meter las cosas en otro documento
        fwrite(&it->first , sizeof(string ) , it->first.length()+1 , fp  ); 
        auto t = it->second.getIdDoc(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 
        Fecha* f = new Fecha();
        *f = it->second.getFechaModificacion(); 
        t = f->getAnyo(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 
        t = f->getDia(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 
        t=  f->getHora(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 
        t=  f->getMes();
        fwrite(&t , sizeof(t) , 1 , fp );
        t=  f->getMin();
        fwrite(&t , sizeof(t) , 1 , fp );  
        t=  f->getSeg(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 
        t = it->second.getNumPal(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 
        t= it->second.getNumPalDiferentes(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 
        t = it->second.getNumPalSinParada(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 
        t = it->second.getTamBytes(); 
        fwrite(&t , sizeof(t) , 1 , fp ); 

    }
    
    return true;
}*/
/**
 * Hacemos la lectura del disco
 * 
 * */ 
/*
bool IndexadorHash::leerDeDisco(){
    char * ficheroSAL = new char(); 
    string concat = "/PrivValuesIND.txt";
    ficheroSAL = const_cast<char*>((this->directorioIndice + concat).c_str());
    
    
    FILE* fp = fopen( ficheroSAL,"rb");
    size_t size;
    int read_bytes = 0;     
    fread(&size, sizeof(size), 1, fp);
    void* buf = malloc(size);
    fread(buf, size, 1, fp);
    fclose(fp);
    int tam = *((int * )buf ); //Con esto leemos el tamaño del índice
    for(int i = 0 ; i<tam ; i++){
        InformacionTermino * actual = new InformacionTermino();
        string * nombre = new string(); 
        *nombre = *((string*)buf);  // might break if you wrote your file on OS with different endingness
        read_bytes +=sizeof(*nombre);
        int fd = *((int * ) buf+read_bytes);
        actual->setFd(fd);
        read_bytes += sizeof(fd);
        int ft = *((int * ) buf + read_bytes);
        actual->setFt(ft); 
        read_bytes+=sizeof(fd);
        int ftc =*((int *) buf + read_bytes);
        actual->setFtc(ftc);
        read_bytes += sizeof(ftc);
        int listDocsTotal = *((int *) buf + read_bytes ); 
        unordered_map <long int , InfTermDoc>cj;
        for(int j = 0 ; j<listDocsTotal ; j++){
            //Sacamos el id 
            
            long int idDoc =*((long int * ) buf + read_bytes); 
            read_bytes = read_bytes + sizeof(idDoc ); 
            int ftDoc =*((int * ) buf + read_bytes);  
            read_bytes += sizeof(ftDoc);
            //Ahora miraremos si tenemos que leer términos
            int leerPosiciones = *((int*) buf + read_bytes ); 
            read_bytes += sizeof(leerPosiciones );
            InfTermDoc *term = new InfTermDoc();
            if(leerPosiciones == 1 ) {
                this->almacenarPosTerm = true;
                //Sacaremos el tamaño
                int tam = *((int *) buf + read_bytes);
                read_bytes += sizeof(tam);
                list<int> *posL = new list<int>();
                for(int i = 0; i <tam ; i ++){ //ft y PosTerm
                    int pos = *((int *) buf + read_bytes); 
                    read_bytes += sizeof(pos);
                    posL->push_back(pos);
                } 
                //Montaremos el término que irá dentro del unordered_map
                
                term->setPosTerm(*posL); 
            }
            term->setFt(ftDoc);
            //Lo meteremos al conjunto 
            std::pair <long int , InfTermDoc > pareja1 (idDoc, *term); 
            cj.insert(pareja1);
            delete term;

        }
        actual->setL_docs(cj);
        std::pair<string , InformacionTermino> tCompleto (*nombre , *actual);
        this->indice.insert(tCompleto);
        delete nombre; delete actual;

    }

}*/

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
        std::string *buffer = new string();
        buffer->clear();
        //AHORA COGEREMOS LOS VALORES DE LAS VARIABLES PRIVADAS QUE IR?N AL PRIMER ARCHIVO    
        buffer->append("PreguntaIndexada" + this->pregunta);     
        buffer->append("\n");
        buffer->assign("FicheroStopWords" + this->ficheroStopWords);    
        buffer->append("\n");
        buffer->append("tipoStemmer"); buffer->append(to_string(this->tipoStemmer));   
        buffer->append("\n");
        buffer->append("Almacena en disco"); buffer->append(to_string( this->almacenarEnDisco));    
        buffer->append("\n");
        buffer->append("Almacena posTerm"); buffer->append(to_string(this->almacenarPosTerm));   
        buffer->append("\n");
        buffer->append("TOKDelimiters"); buffer->append(this->tok.DelimitadoresPalabra());  
        buffer->append("\n");
        buffer->append("TOKPasarAMinSin");buffer->append(to_string(this->tok.PasarAminuscSinAcentos()));  
        buffer->append("\n");
        buffer->append("TOKCasosEspeciales"); buffer->append(to_string(this->tok.CasosEspeciales()));  
        buffer->append("\n");
        buffer->append("ICDDNumDocs");  buffer->append(to_string(informacionColeccionDocs.getNumDocs()));
        buffer->append("\n");
        buffer->append("ICDDNumTotalPal");  buffer->append(to_string(informacionColeccionDocs.getNumTotalPal()));
        buffer->append("\n"); 
        buffer->append("ICDDNumSinParada"); buffer->append(to_string(informacionColeccionDocs.getNumTotalPalSinParada())); 
        buffer->append("\n");
        buffer->append("ICDDNumDiferentes"); buffer->append(to_string(informacionColeccionDocs.getNumTotalPalDiferentes()));
        buffer->append("\n");
        buffer->append("ICDDTamBytes"); buffer->append(to_string(informacionColeccionDocs.getTamBytes()));
        buffer->append("\n");
        buffer->append("INFPREGNumTotalPal"); buffer->append(to_string(infPregunta.getNumTotalPal())); 
        buffer->append("\n");
        buffer->append("INFPREGDiferentes"); buffer->append(to_string( infPregunta.getNumTotalPalDiferentes())); 
        buffer->append("\n"); 
        buffer->append("INFPREGSinParada"); buffer->append( to_string(infPregunta.getNumTotalPalSinParada())); 
        std::ofstream i(this->directorioIndice + "/PrivValues1.txt" );
        if(!i) {
            cerr << "ERROR: No se ha podido escribir en el directorio " <<this->directorioIndice<< " el archivo de guardado del disco: PrivValues1.txt" << endl;
            if( remove( this->ficheroStopWords.c_str() ) != 0 ) cerr<< "Error deleting file" ;
            return false;
        }
        i.write(buffer->data(), buffer->size());
        i.close();
        buffer->clear(); 
        std::stringstream o; 
        //Ahora guardaremos el resto en PrivValuesMaps.txt
        o<<"INDICE\n";
        for ( auto it = this->indice.begin(); it != this->indice.end(); it++ ){ o<<it->first<<"\t "<<it->second<<endl; }
        o<<"INDICEDOCS\n";
        for ( auto it = this->indiceDocs.begin(); it != this->indiceDocs.end(); it++ ){ o<<it->first<<"\t "<<it->second<<endl;}
        o<<"INDICEPREGUNTA\n";
        for ( auto it = this->indicePregunta.begin(); it != this->indicePregunta.end(); it++ ){ o<<it->first<<"\t "<<it->second<<"\tPREG:"<<"\t "<<this->pregunta<<endl;'\n'; }
        *buffer= *buffer + o.str();
        i.open(this->directorioIndice + "/PrivValuesMaps.txt");
        if(!i) {
            cerr << "ERROR: No se ha podido escribir en el directorio "<<this->directorioIndice<< " el archivo de guardado del disco: PrivValuesMaps.txt" << endl;
            if( remove( this->ficheroStopWords.c_str() ) != 0 ) cerr<< "Error deleting file" ;
            if( remove( "PrivValues1.txt" ) != 0 ) cerr<< "Error deleting file" ;
            return false;
        }
        i.write(buffer->data() , buffer->size()); 
        buffer->clear();
        i.close(); 
        return true;
    }else {
        std::string buffer;
        buffer.clear();
        std::stringstream o; 
        //o.clear();
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
    string *copiaDel  = new string();
    *copiaDel= this->tok.DelimitadoresPalabra();
    bool *copiaEspeciales = new bool();
    *copiaEspeciales= this->tok.CasosEspeciales();
    bool *pasarAMinSin = new bool();
    *pasarAMinSin = this->tok.PasarAminuscSinAcentos();
    tok.DelimitadoresPalabra("\t ");//Para tokenizar los indices
	tok.CasosEspeciales(false);
	tok.PasarAminuscSinAcentos(true);
    list<string> res; 
    while(std::getline(f,to,'\n')){
        res.clear();
        //cout<<"CADENA:"<<to<<endl;
        if(to.find("INDICE")==string::npos && to.find("INDICEDOCS")==string::npos && to.find("INDICEPREGUNTA")==string::npos){//SI NO ESTAMOS CAMBIANDO ENTRE UNO Y OTRO
            if(p=="INDICE"){//CUANDO ESTAMOS EN EL PRIMERO
                //string it1 = ""; //Primera parte del conjunto (string)
                //string it2 = ""; //Segunda parte del  conjunto (<<InformacionTermino)
                /*if(to.find("\t")!=string::npos){ //STRING 
                    it1 = to.substr(0, to.find("\t"));
                    cout<<"MI IT1 ES "<<it1<<endl;
                    string p = "\t";
                    it2 = to.substr(to.find("\t") + p.length());
                }*/ 
               // cout<<"ENTRANDO"<<endl;
                if(to.find("\t")!=string::npos){   
                    string it1="";   
                    //cout<<"LA CADENA QUE VAMOS A INDEXAR PARA "<<it1<<" es "<<it2<<endl;
                    InformacionTermino *actualTerm = new InformacionTermino(); 
                    list<int> InfTermDocLista; 
                    list<int> ftInfTermDocs; 
                    list<long int> idsDocs;
                    unordered_map<long int , InfTermDoc> entero; 
                    unordered_map<long int , std::pair<int , list<int>>> listaCompleta;
                    tok.Tokenizar(to,res);//AS? SACAREMOS CADA UNA DE LAS PARTES QUE NECESITAMOS
                    bool nombre = false;
			        for(list<string>::iterator iter= res.begin();iter!= res.end();++iter){
                        //cout<<"MIRAMOS EL "<<*iter<<endl;
                        
                        if(*iter!="ft:" && *iter!="frecuencia" && *iter!="total:" && *iter!="fd:" && *iter!="id.doc:" && this->almacenarPosTerm==true && nombre == true ){ //CUANDO NOS ENCONTRAMOS N?MEROS (ESTAMOS AL FINAL)
                            for( auto iter2= iter ; iter2!=res.end() ; iter2++){
                               iter=iter2;
                               if(*iter2!="ft:" && *iter2!="frecuencia" && *iter2!="total:" && *iter2!="fd:" && *iter2!="id.doc:"){
                                    InfTermDocLista.push_back(std::stoi(*iter2));
                               }else break;
                           } 
                            
                        }
                        if( nombre == false ){
                            //cout<<"ME METO ULT CON "<<*iter<<endl;
                            auto iter2 = iter; 
                            iter2++;
                            //cout<<"MI ITER 2 ES "<<*iter2<<endl;
                            if(*iter2=="frecuencia"){
                                nombre = true;
                                //cout<<"Me he metido a mirar "<<*iter<<endl;
                                it1=*iter;
                                //iter++;
                            }
                            
                        }
                        if(idsDocs.size() >=1 && this->almacenarPosTerm==true && InfTermDocLista.size()>=1 ){
                            auto iterator = idsDocs.begin(); 
                            auto iteratorFt= ftInfTermDocs.begin();
                            //if(InfTermDocLista.size()>=1 && this->almacenarPosTerm== true ){
                            std::pair< int , list<int>> copia (*iteratorFt, InfTermDocLista); 
                            std::pair<long int , std::pair<int , list<int>>> entero (*iterator , copia ); //<id , ft , list<Int> posTerm>
                            listaCompleta.insert(entero);
                            InfTermDocLista.clear();
                            //}
                        }
                        if(*iter=="ft:" ){//Cuando entramos al final de la cadena
                            //cout<<"Entrando antes del desastre "<<endl;
                            iter++; 
                            if(almacenarPosTerm==true) ftInfTermDocs.clear();
                            ftInfTermDocs.push_back(std::stol(*iter));    
                        }
                        if(*iter == "total:"){//Sacaremos la frecuencia del doc
                            iter++;
                            actualTerm->setFtc(std::stoi(*iter));
                            actualTerm->setFt(std::stoi(*iter));
                        }
                        if(*iter=="fd:"){//Sacamaos la frecuencia del documento
                            iter++; 
                            actualTerm->setFd(std::stoi(*iter));
                        }
                        if(*iter == "id.doc:"){//Sacamos el id del documento que va en el unordered map
                            iter++; 
                            if(almacenarPosTerm==true)idsDocs.clear();
                            idsDocs.push_back(std::stol(*iter)); 
                        }
                    }
                    unordered_map <long int , InfTermDoc > l_docs; 
                    //Una vez fuera del bucle, crearemos el término 
                    if(almacenarPosTerm==false ){
                        listaCompleta.clear();
                        for(auto iterator = idsDocs.begin() ; iterator != idsDocs.end() ; iterator ++){
                            InfTermDoc *act = new InfTermDoc(); 
                            act->setFt(*ftInfTermDocs.begin()); 
                            ftInfTermDocs.erase(ftInfTermDocs.begin());
                             std::pair<long int , InfTermDoc> parejaAct (*iterator,*act);
                            l_docs.insert(parejaAct); 
                            delete act;
                        }
                    }
                    for(auto iterator = listaCompleta.begin() ; iterator!= listaCompleta.end() ; iterator ++){
                        InfTermDoc *act = new InfTermDoc(); 
                        act->setFt(iterator->second.first); 
                        act->setPosTerm(iterator->second.second);
                        std::pair<long int , InfTermDoc> parejaAct (iterator->first ,*act);
                        l_docs.insert(parejaAct); 
                        delete act;
                    }
                    
                    //Una vez tengamos el l_docs
                    
                    actualTerm->setL_docs(l_docs);
                    //Lo meteremos en el índice
                    std::pair<string , InformacionTermino> term (it1 , *actualTerm);
                    this->indice.insert(term);
                    delete actualTerm;
                }
            }
            if(p=="INDICEDOCS"){//CUANDO ESTAMOS EN EL PRIMERO
                string it1 = ""; //Primera parte del conjunto (string)
                string it2 = ""; //Segunda parte del  conjunto (<<InformacionTermino)
                if(to.find("\t")!=string::npos){ //STRING 
                    it1 = to.substr(0, to.find("\t"));
                    string p = "\t";
                    it2 = to.substr(to.find("\t") + p.length());
                    //cout<<"Mi it2 de INDICEDOCS es "<<it2<<endl;
                } 
                if(it2!=""){
                    InfDoc itCopia = InfDoc();         
                    tok.Tokenizar(it2,res);//AS? SACAREMOS CADA UNA DE LAS PARTES QUE NECESITAMOS
                
			        for(list<string>::iterator iter= res.begin();iter!= res.end();++iter){
                        if(*iter=="iddoc:"){ iter++;  itCopia.setIdDoc(std::stol(*iter));    }
                        if(*iter == "numpal:"){ iter++; itCopia.setNumPal(std::stoi(*iter));}
                        if(*iter=="numalsinparada:"){ iter++; itCopia.setNumPalSinParada(std::stoi(*iter));}
                        if(*iter == "numpaldiferentes:"){ iter++; itCopia.setNumPalDiferentes(std::stoi(*iter)); }
                        if(*iter=="tambytes:"){ iter++; itCopia.setTamBytes(std::stoi(*iter)); }
                    }
                    std::pair<string , InfDoc> parejaFinal (it1 , itCopia);
                    //Le metemos en el total
                    this->indiceDocs.insert(parejaFinal);
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
                        if(*iter!="ft:" && this->almacenarPosTerm == true){posTerms.push_back(std::stoi(*iter)); }//CUANDO NOS ENCONTRAMOS N?MEROS (ESTAMOS AL FINAL)
                        if(*iter=="ft:"){
                            iter++; 
                            itCopia.setFt(std::stoi(*iter));
                        }
                        if(*iter=="preg:"){
                            iter++;
                            this->pregunta=*iter;
                        }
                    }
                    if(this->almacenarPosTerm==true)itCopia.setPosTerm(posTerms);
                    std::pair<string , InformacionTerminoPregunta> parejaFinal (it1 , itCopia);
                    //Le metemos en el total
                    this->indicePregunta.insert(parejaFinal);
                }
            }
        }
        //MIRAREMOS SI ESTAMOS EN EL SIGUIETE
        if(to.find("INDICE")!=string::npos) p = "INDICE"; 
        if(to.find("INDICEDOCS")!=string::npos) p = "INDICEDOCS";
        if(to.find("INDICEPREGUNTA")!=string::npos) p = "INDICEPREGUNTA";
    }
    this->tok.DelimitadoresPalabra(*copiaDel);
    this->tok.PasarAminuscSinAcentos(*pasarAMinSin);
    this->tok.CasosEspeciales(*copiaEspeciales);
    delete copiaDel; delete copiaEspeciales; delete pasarAMinSin;
    file.close(); 
    f.close();
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
        //cerr << "ERROR: No existe el archivo: " << fichero << " En el directorio "<<this->directorioIndice<< endl;
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
        if(to.find("ICDDNumSinParada") != string::npos) { 
            p = "ICDDNumSinParada"; 
            this->informacionColeccionDocs.setNumTotalPalSinParada(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("ICDDNumDiferentes") != string::npos) { 
            p = "ICDDNumDiferentes"; 
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
        if(to.find("INFPREGDiferentes") != string::npos) { 
            p = "INFPREGDiferentes"; 
            this->infPregunta.setNumTotalPalDiferentes(std::stol(to.substr(to.find(p)+p.length() )));
        }
        if(to.find("INFPREGSinParada") != string::npos) { 
            p = "INFPREGSinParada"; 
            this->infPregunta.setNumTotalPalSinParada(std::stol(to.substr(to.find(p)+p.length() )));
        }
        p.clear();      
    }

    f.close();
    file.close();
    return true;
}

IndexadorHash::IndexadorHash(const string& directorioIndexacion){
    //LECTURA DEL DIRECTORIO+PRIMERO MIRAMOS QUE EXISTA
    this->directorioIndice = directorioIndexacion; 
    struct stat dir;
    if(stat(this->directorioIndice.c_str(), &dir) == -1 || !S_ISDIR(dir.st_mode) ){
        cerr << "Error:  " << strerror(errno) << endl; 
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
            this->tok.PasarAminuscSinAcentos(true);
            this->tok.DelimitadoresPalabra(" "); 
            this->ficheroStopWords="";
            this->tipoStemmer= 0; 
            this->almacenarEnDisco=true; 
            this->almacenarPosTerm=false;
        }
    }catch(...){
        cerr<<"ERROR: Los datos del archivo PrivValues1 no son correctos"<<endl;
        return; 
    }
    try{
        if(this->ficheroStopWords!="") ReadStopWords(); 
    }catch(...){
        cerr<<"ERROR: No se ha podido leer el ficheroStopWords"<<endl;
        return; 
    }
    try{
        if(!ReadPrivValuesMaps() ){
            cerr<<"ERROR: al leer el archivo con los índices"<<endl;
            return;
        }
    }catch(...){
        cerr<<"ERROR: Los datos del archivo PrivValuesMaps no son correctos"<<endl;
        return; 
    } 
    
}

/**
 * M?todo par apasar a min?sculas sin acentos
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
 * M?todo para comprobar si tenemos indexada una palabra 
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
    if(this->tipoStemmer == 1 ) { //Habr? que usar el stemmer en espa?ol
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
                for(std::pair<std::string, InfDoc > it2: this->indiceDocs){ //Miramos si el documento est? dentro Y COINCIDE CON EL DOCUMENTO QUE QUEREMOS
                    if(it2.first == nomDoc){ //Miramos si coinciden los nombres (del documento iterado y del documento que me han pasado de entrada)
                        //Miraremos para cada uno de los documentos, si su id coincide con los documentos que hay indexados en el ?ndice del t?rmino, de manera que si coinciden, ya sabremos que es el bueno
                        //cogeremos cada uno de los documentos
                        for(auto it3 = iterator.second.getL_docs().begin() ; it3!=iterator.second.getL_docs().end() ; it3 ++) { //Con esto vamos sacando cada una de las iteraciones que est?n detntro del t?rmino
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
                for(std::pair<std::string, InfDoc > it2: this->indiceDocs){ //Miramos si el documento est? dentro Y COINCIDE CON EL DOCUMENTO QUE QUEREMOS
                    if(it2.first == nomDoc){ //Miramos si coinciden los nombres (del documento iterado y del documento que me han pasado de entrada)
                        for(auto it3 = iterator.second.getL_docs().begin() ; it3!=iterator.second.getL_docs().end() ; it3 ++) { //Con esto vamos sacando cada una de las iteraciones que est?n detntro del t?rmino
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
                for(std::pair<std::string, InfDoc > it2: this->indiceDocs){ //Miramos si el documento est? dentro Y COINCIDE CON EL DOCUMENTO QUE QUEREMOS
                    if(it2.first == nomDoc){ //Miramos si coinciden los nombres (del documento iterado y del documento que me han pasado de entrada)
                        for(auto it3 = iterator.second.getL_docs().begin() ; it3!=iterator.second.getL_docs().end() ; it3 ++) { //Con esto vamos sacando cada una de las iteraciones que est?n detntro del t?rmino
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
 * Cuando devolvemos el ?ndice
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
 * M?todo para actualizar
 * 
 * */ 
bool IndexadorHash::Actualiza(const string& word, const InformacionTermino& inf){
    //Miraremos si el t?rmino est? indexado
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
            //Tendremos que coger su id y borrarlo de todos los ?ndices que lo contengan 
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
    //cout<<"Estoy entrando "<<indiceDocs.size()<<endl;
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
 * M?tdo para devolver el infPregunta de una pregunta que tengamos almacenada
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
 * M?todo para mirar si hay una pregunta indexada y devolverla 
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
 * M?todo para borrar una palabra del ?ndice con todas las palabras indexadas
 * 
 * */ 
bool IndexadorHash::Borra(const string& word){
    //Miraremos si esa palabra despu?s de aplicarle el stemming est? indexada
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
 * M?todo para borrar documentos
 * 
 * */
bool IndexadorHash::BorraDoc(const string& nomDoc){
    //Primero miraremos si tenemos el documento indexado 
    for(auto it1 = this->indiceDocs.begin() ; it1 != this->indiceDocs.end() ;  it1++) {
        if(it1->first == nomDoc){//Si es el mismo documento
            //Tendremos que coger su id y borrarlo de todos los ?ndices que lo contengan 
            for(auto it2 = this->indice.begin() ; it2!= this->indice.end() ;it2++ ){
                //Tendremos que mirar si contienen el id 
                //cout<<"VOY A MIRAR EL TERM  "<<it2->first<<endl;
                for(auto it3 = it2->second.getL_docs().begin() ; it3!= it2->second.getL_docs().end() ; it3++) {
                    if(it3->first == it1->second.getIdDoc()){ // Si coinciden los ids del documento
                        //Tendremos que actualizar las frecuencias
                        it2->second.setFd(it2->second.getFd() -1 ); 
                        it2->second.setFtc(it2->second.getFtc() - it3->second.getFt());
                        it2->second.setFt(it2->second.getFt() - it3->second.getFt());
                        //Miraremos si conten?a m?s documentos
                        unordered_map<long int , InfTermDoc > copia = it2->second.getL_docs();
                        copia.erase(it3->first);
                        it2->second.setL_docs(copia); 
                    
                        if(it2->second.getL_docs().size()==0){
                            //cout<<"ESTOY ENTRANDO Y MI TAM DEL INDICE ES "<<this->indice.size()<<"Y VOY A BORRAR "<<it2->first<<endl;
                            this->indice.erase(it2->first);
                            it2 = this->indice.begin(); 
                            //cout<<"ESTOY SALIENDO Y MI TAM DEL INDICE ES "<<this->indice.size()<<endl;
                        }
                        break; 
                    }

                } 
                
                
            }
            //Ahora borraremos el documento
            
	        this->informacionColeccionDocs.setTamBytes(informacionColeccionDocs.getTamBytes()-it1->second.getTamBytes());
		    this->informacionColeccionDocs.setNumTotalPal(informacionColeccionDocs.getNumTotalPal()-it1->second.getNumPal());
		    this->informacionColeccionDocs.setNumTotalPalSinParada(informacionColeccionDocs.getNumTotalPalSinParada()-it1->second.getNumPalSinParada());
		    this->informacionColeccionDocs.setNumTotalPalDiferentes(indice.size()); //ALGO NO FUNCIONA 
            this->informacionColeccionDocs.setNumDocs(informacionColeccionDocs.getNumDocs()-1);
            this->indiceDocs.erase(it1->first);
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
 * M?todo que usaremos para indexar documentos
 * 
 * */
bool IndexadorHash::IndexarUnDocu(const char * fichero , InfDoc & actual , list<string> * palabras){
    struct stat properties;  //Lo usaremos para poder sacar los valores del tama?o en bytes
    int rc = stat(fichero, &properties);
    //cout<<"FICHERO: "<<fichero<<endl;
    /*if( S_ISDIR(properties.st_mode) ){
        cerr<<"Error: El fichero: "<<fichero<<" es un directorio!"<<endl;
        return true;
    }*/
    ifstream file;     
    file.open(fichero);
    if(!file){
        cerr<<"Error: El fichero: "<<fichero<<" No se ha podido abrir "<<endl;
        return true; //No mostramos mensaje de error porque se supone que el archivo lo acabamos de crear  
    }
    std::ifstream f(fichero );
    std::string to; 
    actual.setTamBytes( properties.st_size); //Ponemos el tama?o de los bytes
    actual.setNumPal(0); actual.setNumPalSinParada(0); actual.setNumPalDiferentes(0); 
    
    //Iremos por cada token creado en el archivo
    list<string> *tokens= new list<string>(); 
    list<string> *conjunto = new list<string >();
    while(std::getline(f,to,'\n')){
        this->tok.Tokenizar(to, *tokens);
        for( auto its = tokens->begin(); its!=tokens->end() ; its++){
            conjunto->push_back(*its);
        }
        tokens->clear();
    }
    delete tokens;
    for(auto to = conjunto->begin() ; to != conjunto->end() ; to ++){
        //cout<<*to<<endl;
        
        palabras->push_back(*to); //Pos si falla, saber en qué término nos hemos quedado 
        bool eraPalabraParada= false;
        string * copia = new string(); 
        *copia = *to;
        if(tipoStemmer == 1 ){
            stemmerPorter s = stemmerPorter(); 
            s.stemmer(*to , 1); 
        }
        if(tipoStemmer == 2 ){
            stemmerPorter s = stemmerPorter(); 
            s.stemmer(*to , 2); 
        }
        if(*to=="" ||*to ==" " || *to=="\n" || *to=="\r" || *to=="\n" || *to=="\t") break;
        for(auto iterator = this->stopWords.begin() ; iterator!= stopWords.end() ; iterator ++){
            if(*iterator == *to ||*copia == *iterator){ //Hemos encontrado una stopWord
                //Sumaremos +1 al n?mero de palabras que nos hemos encontrado en el documento
                actual.setNumPal(actual.getNumPal() + 1);  
                eraPalabraParada = true;
                break;
            }
        }
        delete copia;
        if(!eraPalabraParada){ //Como no era una palabra de parada, pues miramos si estaba ya o no indexada
            //Miramos si estaba ya indexada (Como quiero hacerlo eficiente, no utilizo la funci?n Existe para mirar esto)
           
            bool encontradoEnIndice = false; 
            for( auto iterator = this->indice.begin() ; iterator!= this->indice.end() ; iterator ++){
                if(iterator->first == *to ){ //NOS HEMOS ENCONTRADO CON LA CADENA YA PREVIAMENTE INDEXADA POR OTRO DOCUMENTO 
                    bool existeYaId = false;
                    encontradoEnIndice = true; 
                    iterator->second.setFtc(iterator->second.getFtc() +1 );
                    iterator->second.setFt(iterator->second.getFt() + 1 );
                    for(auto iterator2 = iterator->second.getL_docs().begin() ; iterator2 != iterator->second.getL_docs().end() ; iterator2 ++){
                        if(iterator2->first == actual.getIdDoc() ){
                            if(this->almacenarPosTerm==true ){
                                list<int> copia = iterator2->second.getPosTerm(); 
                                copia.push_back(actual.getNumPal());
                                iterator2->second.setPosTerm(copia);
                            }
                            iterator2->second.setFt(iterator2->second.getFt() + 1 );
                            existeYaId = true; 
                            break; 
                        }      
                    }if(existeYaId)break;
                    if(!existeYaId){
                       // cout<<"ENTRANDO"<<endl;
                        InfTermDoc *elNuevo;
                        elNuevo = new InfTermDoc();
                        elNuevo->setFt( 1 );
                        if(this->almacenarPosTerm == true ){
                            list<int> copia;//= elNuevo->getPosTerm();
                            copia.push_back(actual.getNumPal());
                            elNuevo->setPosTerm(copia);
                        }
                        //Ahora, meteremos el par dentro del iterador
                        std::pair<long int , InfTermDoc > added (actual.getIdDoc() , *elNuevo); 
                        unordered_map<long int , InfTermDoc> copia2 = iterator->second.getL_docs(); 
                        copia2.insert(added);
                        iterator->second.setL_docs(copia2);
                        delete elNuevo;
                        break;
                    }
                    break; 
                }
            }
           
            if(!encontradoEnIndice){ //Cuando no estaba en el ?ndice
                InformacionTermino nuevoTermino = InformacionTermino(); 
                nuevoTermino.setFt(1); 
                nuevoTermino.setFtc(1);
                InfTermDoc *elNuevo;
                elNuevo = new InfTermDoc();
                elNuevo->setFt( 1 );
                if(almacenarPosTerm == true){
                    list<int> copia;
                    copia.push_back(actual.getNumPal());//Metemos el n?mero de palabras que nos hemos encontrado
                    elNuevo->setPosTerm(copia);
                }
                //Crearemos un par para a?adir al ?ndice
                std::pair<long int , InfTermDoc > added (actual.getIdDoc() , *elNuevo); 
                unordered_map<long int , InfTermDoc> listaNueva;
                listaNueva.insert(added);
                nuevoTermino.setL_docs(listaNueva);
                std::pair<std::string , InformacionTermino> pareja (*to , nuevoTermino);
                this->indice.insert(pareja);
                delete elNuevo; 
            }
            actual.setNumPalSinParada(actual.getNumPalSinParada() + 1 );
            actual.setNumPal(actual.getNumPal() + 1 );
        }
        
    }
		//Tendremos que buscar las palabras que son diferentes del documento 
    long int counter = 0; 
	unordered_map<long int,InfTermDoc> copiaLista;
    //cout<<"ENTRANDO A CONTAR "<<endl;
	for(auto iterator=indice.begin();iterator!=indice.end();iterator++){ //Por cada t?rmino indexado, buscaremos aquel que contenga el documento que estamos buscando
	    for( std::pair< long int , InfTermDoc>it2 : iterator->second.getL_docs() ) {
            //cout<<"Hey que tenemos estos ids "<<it2.first<<" Y QUE LA PALABRA ES "<<iterator->first<<endl;
            if(it2.first == actual.getIdDoc()) {
                counter++;
            }
        }
	}
    actual.setNumPalDiferentes(counter); 
    std::pair<string , InfDoc> result (fichero , actual); 
    this->indiceDocs.insert(result);
    //ACTUALIZAMOS EL RESTO
    informacionColeccionDocs.setNumDocs(informacionColeccionDocs.getNumDocs()+1);
    informacionColeccionDocs.setNumTotalPalDiferentes(indice.size());
    informacionColeccionDocs.setNumTotalPal(informacionColeccionDocs.getNumTotalPal()+actual.getNumPal());
    informacionColeccionDocs.setTamBytes(informacionColeccionDocs.getTamBytes()+actual.getTamBytes());
    informacionColeccionDocs.setNumTotalPalSinParada(informacionColeccionDocs.getNumTotalPalSinParada()+actual.getNumPalSinParada());
    file.close();
    delete conjunto;
    
    return true; 
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
         * 1. Mirar si ese documento ya est? indexado, de manera que si lo est?, no tendremos que indexarlo nuevamente, solamente tendremos que modificar su frecuencia
         * */
        bool estabaIndexado = false;
        for(auto iterator= indiceDocs.begin() ; iterator!=indiceDocs.end() ; iterator++){
            if(iterator->first == to ){ //Si ya est? indexado, haremos sus respectivas modificaciones
                //Compararemos las fechas de los documentos
                estabaIndexado=true;
                Fecha copiaFechaIterator = iterator->second.getFechaModificacion(); 
                Fecha f =Fecha();  //Creamos una nueva fecha con la hora local del sistema
                if( f == copiaFechaIterator || f<copiaFechaIterator) break; //No haremos nada 
                if(f > copiaFechaIterator ){ //Cuando f es m?s actual que la fecha que hab?a metida
                    //Borraremos e indexaremos nuevamente el documento
                    auto copia = iterator; 
                    this->BorraDoc(to);
                    list<string> *palabras= new list<string>;
                    try{
                        struct stat properties;  //Lo usaremos para poder sacar los valores del tama?o en bytes
                        int rc = stat(copia->first.c_str(), &properties);
                        if( S_ISDIR(properties.st_mode) ){
                            cerr<<"Error: El fichero: "<<copia->first<<" es un directorio!"<<endl;
                        }else { IndexarUnDocu(const_cast<char *>(copia->first.c_str())  , copia->second , palabras);}; 
                        delete palabras;
                    }catch(...){
                        cerr << "ERROR: indexación no finalizada."<<endl;
                        cerr<<"Nos hemos quedado en el term: "<<*palabras->end()<<"Y en el documento "<<copia->first.c_str()<<endl;
                        return false; 
                    }
                    break; 
                }
            } 
        }
        if(!estabaIndexado){
            list<string> *palabras = new list<string>;
            try{
                InfDoc actual = InfDoc(); 
                actual.setIdDoc(indiceDocs.size() + 1 );
                struct stat properties;  //Lo usaremos para poder sacar los valores del tama?o en bytes
                int rc = stat(to.c_str(), &properties);
                if( S_ISDIR(properties.st_mode) ){
                    cerr<<"Error: El fichero: "<<to<<" es un directorio!"<<endl;
                }else IndexarUnDocu(const_cast<char*>(to.c_str()), actual , palabras);
                delete palabras;
            }catch(...){
                cerr << "ERROR: indexación no finalizada."<<endl;
                cerr<<"Nos hemos quedado en el term: "<<*palabras->end()<<"Y en el documento "<<to.c_str()<<endl;
                return false; 
            }

        } 
    
    }
    f.close(); 
    file.close();
    to.clear();
    return true; 
}

/**
 * M?todo para indexar un directorio 
 * HAREMOS USO DEL INDEXADOR DE UN FICHERO CON NOMBRES DE DOCUMENTOS 
 * */ 
bool IndexadorHash::IndexarDirectorio(const string& dirAIndexar){
    struct stat properties; 
    if(stat(dirAIndexar.c_str(), &properties) == -1 || !S_ISDIR(properties.st_mode) ) return false;
    string cmd="find "+dirAIndexar+" -follow |sort > FicheroConNombreDocus.txt"; //Extrae todas las rutas de los ficheros a un archivo .lista_fich
	system(cmd.c_str()); //Ejecuta el comando cmd en el tablero de comandos
	//Recorreremos el archivo y quitaremos los directorios que hayan, para que el indexar no de fallo 
    ifstream file;
    string cadena;
    file.open("./FicheroConNombreDocus.txt");
    if(!file) {
        cerr << "ERROR: No existe el archivo: " << this->ficheroStopWords << endl;
        return false;
    }
    std::string *buffer = new string(); 
    std::ifstream f("./FicheroConNombreDocus.txt");
    std::string to;
    while(std::getline(f,to,'\n')){
        //cout<<"No encontramos con "<<to<<endl;
        stat(to.c_str() , &properties); 
        if(!S_ISDIR(properties.st_mode)){
            //Lo dejaremos
            buffer->append(to); 
            buffer->append("\n");
        } 
    }
    //cout<<"LO QUE NOS ENCONTRAREMOS ES "<<*buffer<<endl;
    f.close();
    file.close();
    std::ofstream i("./FicheroConNombreDocus.txt");
    if(!i) {
        cerr << "ERROR: No se ha podido escribir en el directorio " << endl;
        return false;
    }
    i.write(buffer->data(), buffer->size());
    delete buffer;
    i.close(); 
    return this->Indexar("./FicheroConNombreDocus.txt");
    
    
}






bool IndexadorHash::IndexarUnTermPregunta(const string & pregCopia , InformacionTerminoPregunta & term ){
    //cout<<"La pregunta que estoy indexando es "<<pregCopia <<"Y SU TERM ES "<<term<<endl;
    if( this->almacenarPosTerm ==true ) { //Si tenemos que almacenar las posiciones
        list<int> copia = term.getPosTerm();
        copia.push_back(this->infPregunta.getNumTotalPal());//Metemos el n?mero de palabras que nos hemos encontrado
        term.setPosTerm(copia);                
    }
    term.setFt(term.getFt() +1 ); 
    //Crearemos el par y lo indexaremos
    std::pair<string , InformacionTerminoPregunta> copia (pregCopia , term ); 
    this->indicePregunta.insert(copia);// Metemos la nueva pregunta indexada
    this->infPregunta.setNumTotalPalSinParada(this->infPregunta.getNumTotalPalSinParada() + 1 ); 
    this->infPregunta.setNumTotalPal(this->infPregunta.getNumTotalPal() + 1 ); 
    return true; 

}

/**
 * M?todo para indexar una pregunta
 * 
 * */
bool IndexadorHash::IndexarPregunta(const string& preg){
    //HAREMOS UN BUCLE PAR A IR MIRANDO CADA UNA DE LAS PALABRAS QUE CONFORMAN LA PREGUNTA(DESPU?S DE HABERLAS TOKENIZADO)
    //cout<<"La pregunta que vamos a indexar es "<<preg<<endl;
    bool *pasarAMinSin= new bool(); 
    *pasarAMinSin= this->tok.PasarAminuscSinAcentos(); 
    bool *casosEspeciales = new bool(); 
    *casosEspeciales = this->tok.CasosEspeciales(); 
    this->tok.PasarAminuscSinAcentos(true ); 
    this->tok.CasosEspeciales(false); 
    list<string > tokens; //PALABRAS RESULTANTES
    this->tok.Tokenizar(preg, tokens ); 
    this->pregunta.clear();
    this->pregunta = preg;
    this->infPregunta.~InformacionPregunta(); // Borramos todo lo que hab?a de la informaci?n de la pregunta
    this->indicePregunta.clear(); 
    //Ahora recorreremos la lista y miraremos si estaba ya indexada y dem?s 
     for (std::list<string>::iterator iterPalabraPregunta=tokens.begin(); iterPalabraPregunta != tokens.end(); ++iterPalabraPregunta){
        //Lo primero que haremos ser? aplicar el stemming, dado que las palabras que tengamos indexadas vendr?n ya con el stemming aplicado
        //APLICAREMOS EL TIPO DE STEMMING? 
        string * copia = new string(); 
        *copia = *iterPalabraPregunta;
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
            if(iterator->first == *iterPalabraPregunta ){ //Si ya est? indexado, haremos sus respectivas modificaciones
                //cout<<"LA PREGUNTA QUE ESTOY INDEXANDO ES " <<iterator->first<<"Y SU TERM ES "<<iterator->second<<endl;
                estabaIndexado=true;
                InformacionTerminoPregunta copia = iterator->second; 
                this->indicePregunta.erase(iterator->first);
                try{
                    IndexarUnTermPregunta( *iterPalabraPregunta  , copia); 
                    
                }catch(...){ //CUANDO NO TENEMOS ESPACIO EN MEMORIA
                    cerr << "ERROR: No hay espacio suficiente en memoria para la indexación"<<endl;
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
                if(*iterator == *iterPalabraPregunta ||*copia == *iterator ){
                    //Si es una palabra de parada, simplemente sumaremos al total 
                    this->infPregunta.setNumTotalPal(this->infPregunta.getNumTotalPal() + 1 ); 
                    esPalabraParada = true; 
                    break; 
                }
            }
            delete copia;
            if(!esPalabraParada){
                InformacionTerminoPregunta p = InformacionTerminoPregunta(); 
                try{
                    IndexarUnTermPregunta(*iterPalabraPregunta, p );
                }catch(...){
                    cerr << "ERROR: No hay espacio suficiente en memoria para la indexación"<<endl;
                    return false; 
                }
            }

        } 
    
    }
    this->infPregunta.setNumTotalPalDiferentes(this->indicePregunta.size() ); 
    this->tok.CasosEspeciales(*casosEspeciales);
    this->tok.PasarAminuscSinAcentos(*pasarAMinSin);
    delete casosEspeciales; 
    delete pasarAMinSin; 
    return true;


}










