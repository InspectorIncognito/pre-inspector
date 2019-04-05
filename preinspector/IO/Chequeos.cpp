/**
* Project: "proceGTFS".
* Package:
* Class:
* Created by: Mauricio Zuñiga G. <mzuniga@pragxis.cl>
* Terms: These coded instructions, statements, and computer programs are
* produced as part of a software project. They contain unpublished
* propietary information and are protected by copyright law. They may
* not be disclosed to third parties or copied or duplicated in any form,
* in whole or in part, without the prior written consent of Pragxis SpA.
* Copyright:  Pragxis (c) 2016
* Last modified : Mauricio Zuñiga 18-06-2016
*/
#include "Chequeos.h"

Chequeos::Chequeos(void)
{
}

Chequeos::~Chequeos(void)
{
}

void Chequeos::chequeaParaderos(FuenteDatos* fd)
{
	int nTimeStart = Cronometro::GetMilliCount();


    for(map< string, Paradero >::iterator ired = fd->redParaderos.red.begin();ired != fd->redParaderos.red.end();ired++)
    {
        bool esta=false;
        for(map<string, FuenteDatos::SecParadas>::iterator isecp = fd->secuenciaDTPM.begin();isecp != fd->secuenciaDTPM.end();isecp++)
        {
            for(vector<string>::iterator it = (*isecp).second.secuenciav.begin();it != (*isecp).second.secuenciav.end(); it++)
            {
                if((*it).compare((*ired).first)==0)
                    esta=true;
            }
        }
        
        if(!esta)
        {
            cout << "Paradero de red no encontrado en secuencia : " << (*ired).first << endl;
        }
    }
    
    
    for(map<string, FuenteDatos::SecParadas>::iterator isecp = fd->secuenciaDTPM.begin();isecp != fd->secuenciaDTPM.end();isecp++)
    {
        bool esta=false;
        for(vector<string>::iterator it = (*isecp).second.secuenciav.begin();it != (*isecp).second.secuenciav.end(); it++)
        {
            for(map< string, Paradero >::iterator ired = fd->redParaderos.red.begin();ired != fd->redParaderos.red.end();ired++)
            {

                if((*it).compare((*ired).first)==0)
                    esta=true;
            }
            
            if(!esta)
            {
                cout << "Paradero de secuencia no encontrado en red : " << (*isecp).first << "|" << (*it) << endl;
            }
        }
    }
    
    
	cout << "chequeo paradas : " << Cronometro::GetMilliSpan(nTimeStart) / 60000.0 << "(min)" << endl;

}

