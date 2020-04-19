#include "indexadorInformacion.h"

/**
 * CLASE INFTERMDOC
 * 
 * */
//Constructor de copia de la calse
InfTermDoc::InfTermDoc (const InfTermDoc & itd){
    this->ft = itd.ft; 
    this->posTerm = itd.posTerm;
}
// Inicializa ft = 0
InfTermDoc::InfTermDoc (){this->ft= 0;} 
InfTermDoc::~InfTermDoc(){
    this->ft= 0; 
    this->posTerm.clear();
}

InfTermDoc& InfTermDoc::operator =(const InfTermDoc &itd){
    this->ft = itd.ft; 
    this->posTerm =itd.posTerm; 
    return *this;
}
const list<int>& InfTermDoc::getPosTerm(){return this->posTerm;}
void InfTermDoc::setPosTerm(const list<int>& lista){this->posTerm = lista; }
const int InfTermDoc::getFt(){return this->ft;}
void InfTermDoc::setFt(const int& ft){this->ft = ft; }
  

/**
 * CLASE INFORMACION TERMINo
 * 
 * */
InformacionTermino::InformacionTermino(const int& fft, const int& ffd)
{
    this->ft = fft;
    this->fd = ffd;
}

//Constructor de copia de la clase
InformacionTermino::InformacionTermino(const InformacionTermino& it){
    this->ftc= it.ftc; 
    this->ft= it.ft;
    this->fd = it.fd; 
}

InformacionTermino::InformacionTermino(){ft = fd =ftc = 0;}

//Destructor de la clase
InformacionTermino::~InformacionTermino(){
    this->ft=0; 
    this->ftc= 0; 
    this-> fd = 0; 
    l_docs.clear(); 
}

const int InformacionTermino::getFtc() {return this->ftc; }
void InformacionTermino::setFtc(const int & ftc){this->ftc = ftc; }
const int InformacionTermino::getFt(){return this->ft;  }
void InformacionTermino::setFt(const int & ft){ this->ft = ft;}
const int InformacionTermino::getFd(){ return this->fd;}
void InformacionTermino::setFd(const int & fd){ this->fd = fd;}
void InformacionTermino::setL_docs(const unordered_map<long int, InfTermDoc> & l_docs ){
    this->l_docs = l_docs;
}
unordered_map<long int , InfTermDoc>& InformacionTermino::getL_docs(){
    return this->l_docs;
}      
/**
 * CLASE FECHA
 * 
 * */
Fecha& Fecha::operator =(const Fecha &fecha){
    this->anyo = fecha.anyo; 
    this->dia = fecha.dia; 
    this->mes = fecha.mes; 
    this->hora= fecha.hora; 
    this->min = fecha.min; 
    this->seg= fecha.seg;
    return *this;
}

Fecha::Fecha(const Fecha& fecha){ *this = fecha;  }

Fecha::Fecha(const int& dia , const int &mes , const int &anyo, const int &hota , const int & min , const int & seg ){
    this->anyo = anyo; 
    this->dia = dia; 
    this->mes = mes; 
    this->hora= hora; 
    this->min = min; 
    this->seg= seg;
}

Fecha::Fecha() {
    this->anyo = 1975; 
    this->mes = 1; 
    this->dia= 1; 
    this->hora = 00; 
    this->min = 0; 
    this->seg= 0; 
}

Fecha::Fecha(tm * ltm ){
    this->anyo = 1900 + ltm->tm_year; 
    this->mes = 1 + ltm->tm_mon; 
    this->dia = ltm->tm_mday;
    this->hora = 1 + ltm->tm_hour; 
    this->min = 1 + ltm->tm_min; 
    this->seg = 1+ ltm->tm_sec; 
}

void Fecha::setFecha(const int& dia , const int &mes , const int &anyo, const int &hota , const int & min , const int & seg){
    this->anyo = anyo; 
    this->dia = dia; 
    this->mes = mes; 
    this->hora= hora; 
    this->min = min; 
    this->seg= seg;
}
const int Fecha::getMes(){ return this->mes;  }
const int Fecha::getAnyo(){ return this->anyo;  }
const int Fecha::getDia(){ return this->dia; }
const int Fecha::getMin(){ return this->min; }
const int Fecha::getSeg(){ return this->seg; }
const int Fecha::getHora(){ return this->hora;}

/**
 * CLASE INFDOC
 * 
 * */
InfDoc& InfDoc::operator=(const InfDoc & id){
    this->fechaModificacion = id.fechaModificacion; 
    this->idDoc = id.idDoc; 
    this->numPal = id.numPal; 
    this->numPalDiferentes = id.numPalDiferentes; 
    this->numPalSinParada = id.numPalSinParada; 
    this->tamBytes = id.tamBytes;
    return *this;
}
InfDoc::InfDoc(const InfDoc &id){
    (*this) = id; 
}

InfDoc::InfDoc(){
    time_t now =time(0); 
    tm *ltm = localtime(&now);
    this->fechaModificacion = Fecha(ltm);
    this->idDoc =  this->numPal = this->numPalDiferentes = this->numPalSinParada = 0; 
}

InfDoc::~InfDoc(){
    this->idDoc =  this->numPal = this->numPalDiferentes = this->numPalSinParada = 0; 
} 

const long int InfDoc::getIdDoc(){ return this->idDoc;  }
void InfDoc::setIdDoc(const long int &idDoc){this->idDoc = idDoc; }
const int InfDoc::getNumPal(){return this->numPal;}
void InfDoc::setNumPal(const int &numPal){this->numPal = numPal;}
const int InfDoc::getNumPalSinParada(){return this->numPalSinParada; }
void InfDoc::setNumPalSinParada(const int & numPalSinParada) {this->numPalSinParada = numPalSinParada;}
const int InfDoc::getNumPalDiferentes(){return this->numPalDiferentes;}
void InfDoc::setNumPalDiferentes(const int & numPalDiferentes ){this->numPalDiferentes = numPalDiferentes; }
const Fecha InfDoc::getFechaModificacion(){return this->fechaModificacion;}
void InfDoc::setFechaModificacion(const Fecha& fechaModificacion){this->fechaModificacion = fechaModificacion;}

/**
 * CLASE INFCOLECCIONDOCS
 * 
 * 
 * */
InfColeccionDocs::InfColeccionDocs(){
    this->numDocs = this->numTotalPal = this->numTotalPalDiferentes = this->numTotalPalSinParada = this->tamBytes = 0 ; 
}

InfColeccionDocs & InfColeccionDocs::operator=(const InfColeccionDocs &icds){
    this->numDocs= icds.numDocs; 
    this->numTotalPal = icds.numTotalPal;
    this->numTotalPalDiferentes = icds.numTotalPalDiferentes; 
    this->numTotalPalSinParada = icds.numTotalPalSinParada; 
    return *this;
}
InfColeccionDocs::InfColeccionDocs(const InfColeccionDocs & icds ){
    (*this) = icds; 

}

InfColeccionDocs::~InfColeccionDocs(){
    this->numDocs = this->numTotalPal = this->numTotalPalDiferentes = this->numTotalPalSinParada = this->tamBytes = 0 ; 
}

long int InfColeccionDocs::getNumDocs() const {return this->numDocs;}
void InfColeccionDocs::setNumDocs(const long int & numDocs) { this->numDocs = numDocs;}
long int InfColeccionDocs::getNumTotalPal() const {return this->numTotalPal;}
void InfColeccionDocs::setNumTotalPal(const long int & numTotalPal) { this->numTotalPal= numTotalPal;}
long int InfColeccionDocs::getNumTotalPalSinParada() const{return this->numTotalPalSinParada;}
void InfColeccionDocs::setNumTotalPalSinParada(const long int & numTotalPalSinParada) { this->numTotalPalSinParada = numTotalPalSinParada;}
long int InfColeccionDocs::getNumTotalPalDiferentes() const{return this->numTotalPalDiferentes;}
void InfColeccionDocs::setNumTotalPalDiferentes(const long int & numTotalPalDiferentes ) { this->numTotalPalDiferentes = numTotalPalDiferentes; }
long int InfColeccionDocs::getTamBytes()const{ return this->tamBytes; }
void InfColeccionDocs::setTamBytes(const long int & tamBytes ) { this->tamBytes = tamBytes; }

/**
 * CLASE INFORMACIONTERMINOPREGUNTA
 * 
 * */

InformacionTerminoPregunta::InformacionTerminoPregunta(){
    this->ft = 0; 
    this->posTerm.clear();
}

InformacionTerminoPregunta & InformacionTerminoPregunta::operator=(const InformacionTerminoPregunta & itp ){
    this->ft = itp.ft; 
    this->posTerm.clear(); 
    this->posTerm = posTerm;
    return *this;


}
InformacionTerminoPregunta:: InformacionTerminoPregunta(const InformacionTerminoPregunta & itp ){
    (*this) = itp; 
}

InformacionTerminoPregunta::~InformacionTerminoPregunta(){
    this->ft= 0; 
    this->posTerm.clear();
}

const int InformacionTerminoPregunta::getFt(){
    return this->ft;
}
void InformacionTerminoPregunta::setFt(const int &ft ){ this->ft = ft; }
const list<int> & InformacionTerminoPregunta::getPosTerm() {return this->posTerm; };
void InformacionTerminoPregunta::setPosTerm(const list<int> & posTerm ){this->posTerm = posTerm; }

/** 
 * CLASE INFORMACIONPREGUNTA
 * 
 * */

InformacionPregunta & InformacionPregunta::operator=(const InformacionPregunta & ip ){
    this->numTotalPal = numTotalPal; 
    this->numTotalPalSinParada = numTotalPalSinParada; 
    this->numTotalPalDiferentes = numTotalPalDiferentes; 
    return *this;
}
InformacionPregunta::InformacionPregunta(const InformacionPregunta & ip ){
    (*this ) = ip; 
}

InformacionPregunta::InformacionPregunta(){
    this->numTotalPal = this->numTotalPalSinParada= this->numTotalPalDiferentes = 0; 
}

InformacionPregunta::~InformacionPregunta(){
    this->numTotalPal= 0; 
    this->numTotalPalSinParada = 0; 
    this->numTotalPalDiferentes = 0; 
}

long int InformacionPregunta::getNumTotalPal()const {    return  this->numTotalPal; }
void InformacionPregunta::setNumTotalPal(const long int & numTotalPal){ this->numTotalPal = numTotalPal; }
long int InformacionPregunta::getNumTotalPalDiferentes() const{
    return this->numTotalPalDiferentes; 
}
void InformacionPregunta::setNumTotalPalDiferentes(const long int & numTotalPalDiferentes){ this->numTotalPalDiferentes =numTotalPalDiferentes;}
long int InformacionPregunta::getNumTotalPalSinParada() const {return this->numTotalPalSinParada; }
void InformacionPregunta::setNumTotalPalSinParada(const long int & numTotalPalSinParada ) { this->numTotalPalSinParada = numTotalPalSinParada; }






