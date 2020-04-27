#include "../include/tokenizador.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib> 
#include<string.h>
//#include <sstream>
ostream& operator<<(ostream& os , const Tokenizador& tokenizer){
    os <<"DELIMITADORES: " << tokenizer.delimiters << " TRATA CASOS ESPECIALES: "
    << tokenizer.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << tokenizer.pasarAminuscSinAcentos;
    return os; 
}

//Inicializaremos los delimitadores, los casos especiales, que los pondremos a true y pasarAminus a false
Tokenizador::Tokenizador(){
  

    delimiters=",;:.-/+*\\ '\"{}[]()<>?!??&#=\t\n\r@";
    casosEspeciales= true; 
    pasarAminuscSinAcentos=false; 
}


Tokenizador::Tokenizador(const Tokenizador& tokenizer){
    this->delimiters= tokenizer.delimiters; 
    this->casosEspeciales = tokenizer.casosEspeciales; 
    this->pasarAminuscSinAcentos = tokenizer.pasarAminuscSinAcentos; 
}
Tokenizador::~Tokenizador() {
    this->delimiters=""; 
}


//Recorreremos el string y sacaremos los que hay repetidos 
//filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo
//caso se eliminar?an los que hayan sido repetidos por la derecha);
void Tokenizador::filtrarRepetidos(const string& delimitadoresPalabra ){
    //Para filtrar los repetidos lo que vamos a hacer es recorrer cada una de las casillas del string
    for(int i = 0 ; i<delimitadoresPalabra.length() ; i++){
       if(this->delimiters.find(delimitadoresPalabra[i]) == string::npos){
           this->delimiters= this->delimiters + delimitadoresPalabra[i]; 
       }
        
    }
    
}



void Tokenizador:: DelimitadoresPalabra(const string& nuevoDelimiters) {
    this->delimiters="";
    this->filtrarRepetidos(nuevoDelimiters);
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters){
    this->filtrarRepetidos(nuevoDelimiters);
}


//Sobrecarga del operador = 
Tokenizador& Tokenizador::operator= (const Tokenizador& tokenizer) {
    this->delimiters = tokenizer.delimiters; 
    this->casosEspeciales = tokenizer.casosEspeciales; 
    this->pasarAminuscSinAcentos = tokenizer.pasarAminuscSinAcentos;  
    return *this; 
}

//M?TODO PARA PASAR A MIN?SCULAS
string Tokenizador::pasarMinSin( string& cadena ) const{
    int caracter= 0;
    for(int i=0 ; i<cadena.length() ; i++){
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





//Mejorada la eficiencia del m?todo
/*string::size_type find_first_not_of(string delimiters , string::size_type pos ){


}*/

//MODIFICADO PARA HACERLO M?S EFICIENTE PARA LA SEGUNDA PR?CTICA
void Tokenizador::Tokenizar (const string& str, list<string>& tokens) const{
    
    string cadena= str; 
    if(this->pasarAminuscSinAcentos) { //Miramos si se pide pasar a min?sculas
        cadena = this->pasarMinSin(cadena);    
    }
    if(tokens.size()!= 0 ) tokens.clear(); //Borramos si la lista no est? vac?a
   /* string::size_type lastPos = cadena.find_first_not_of(this->delimiters,0);
    string::size_type pos = cadena.find_first_of(this->delimiters,lastPos);
    while(string::npos != pos || string::npos != lastPos)
    {   
        if(string::npos == pos && string::npos!= lastPos)  pos = str.length();
        tokens.push_back(cadena.substr(lastPos, pos - lastPos));
        lastPos = cadena.find_first_not_of(delimiters, pos);
        pos = cadena.find_first_of(delimiters, lastPos);
        
    } */

    char*cstr = strtok(const_cast<char*>(cadena.c_str()),const_cast<char*>(this->delimiters.c_str()));    // 5) Make use of i to be explicit 
    while(cstr!=NULL)                    
    {
      tokens.push_back(cstr);
      cstr=strtok(NULL,const_cast<char*>(this->delimiters.c_str()));
    }
    delete cstr;
   
}



//Haremos los setters de casos especiales y pasar a min?sculas
void Tokenizador::CasosEspeciales(const bool& nuevoCasosEspeciales){
    this->casosEspeciales = nuevoCasosEspeciales;
}

void Tokenizador::PasarAminuscSinAcentos(const bool& nuevoPasarAminuscSinAcentos){
    this->pasarAminuscSinAcentos = nuevoPasarAminuscSinAcentos;
}



bool Tokenizador::CasosEspeciales() const {
    return this->casosEspeciales;
}

string Tokenizador::DelimitadoresPalabra() const{
    return this->delimiters;
}

bool Tokenizador::PasarAminuscSinAcentos() const{
    return this->pasarAminuscSinAcentos; 
}

bool Tokenizador::Tokenizar(const string& NomFichEntr, const string& NomFichSal) const {   
    ifstream i;
    ofstream f;
    string cadena;
    list<string> tokens;
    string copia;
    i.open(NomFichEntr.c_str());
    if(!i) {
        cerr << "ERROR: No existe el archivo: " << NomFichEntr << endl;
        return false;
    
    }else{
        //HACEMOS LA LECTURA M?S R?PIDA DEL ARCHIVO
        std::string buffer;

        std::ifstream f(NomFichEntr);
        std::string to;
        list<string>::iterator its;
        buffer.clear(); 
        //if (copia != NULL)
        //{
        while(std::getline(f,to,'\n')){
            Tokenizar(to , tokens);
                
            for( its = tokens.begin(); its!=tokens.end() ; its++){
                buffer.append((*its));
                buffer.append("\n");
            }
        }
        //}
        std::ofstream i(NomFichSal);
        i.write(buffer.data(), buffer.size());


        //Tokenizar(buffer , tokens);
        
        /*buffer.clear();
        for( its = tokens.begin(); its!=tokens.end() ; its++){
            buffer.append((*its));
            //buffer.append("\n");
        }*/
        //ESCRIBIREMOS EN NUESTRO ARCHIVO DE DESTINO
        

    }
    //if(copia.length()!=0) Tokenizar(copia , tokens );
    i.close();
    f.close();
    return true; 

}


bool Tokenizador::TokenizarListaFicheros (const string& sources) const {
    ifstream file;
    //ofstream f;
    string cadena;
    list<string> tokens;
    file.open(sources.c_str());
    if(!file) {
        cerr << "ERROR: No existe el archivo: " << sources << endl;
        return false;
    }/*else{
        while(!file.eof()){
            cadena="";
            getline(file, cadena);
            if(cadena.length()!=0)
            {
                if(!Tokenizar(cadena , cadena+".tk")){
                    return false; 
                }
                 
            
            } 
            
        } 
    }
    file.close();*/
    std::string buffer;

    std::ifstream f(sources);
    /*f.seekg(0, std::ios::end);
    buffer.resize(f.tellg());
    f.seekg(0);
    f.read(const_cast<char*> (buffer.data()), buffer.size());
    //UNA VEZ LE?DO, SE LO PASAREMOS A NUESTRO TOKENIZADOR
    char * copia = const_cast<char*>(buffer.data());
    auto ss = std::stringstream(copia);*/
    std::string to;
    list<string>::iterator its;
    buffer.clear(); 
    //if (copia != NULL)
    //{
    while(std::getline(f,to,'\n')){
        Tokenizar(to , to+".tk");
                
    }
    /*std::ofstream i(NomFichSal);
    i.write(buffer.data(), buffer.size());
    i.close();*/
    f.close();
    return true;
    file.close();
   

}
    
//Tokenizaremos el fichero de salida  
bool Tokenizador::Tokenizar (const string& i) const {
    return Tokenizar(i , i+".tk"); 
    
}


//M?todo para tokenizar directorios
bool Tokenizador::TokenizarDirectorio(const string & i ) const {
    struct stat dir; // Compruebo la existencia del directorio  
    int err=stat(i.c_str(), &dir); 
      if(err==-1 || !S_ISDIR(dir.st_mode))
       return false; 
       else { 
            // Hago una lista en un fichero con find>fich
            string cmd="find "+i+" -follow |sort > .lista_fich";
            system(cmd.c_str()); 
            return TokenizarListaFicheros(".lista_fich"); 
        }
}
// Inicializa delimiters a filtraRepetidos(delimitadoresPalabra)
//casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a
//minuscSinAcentos 
Tokenizador::Tokenizador(const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){
    filtrarRepetidos(delimitadoresPalabra);
    this->casosEspeciales = kcasosEspeciales; 
    this->pasarAminuscSinAcentos = minuscSinAcentos; 
}



