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
#pragma once

#include <iostream>
#include <string>
#include <map>

using namespace std;

/**
* Title: Paradero
* Description:	Clase que modela un parada de bus
* Copyright:  Pragxis (c) 2016
* Date: 18-06-2016
* @author Mauricio Zuñiga G. <mzuniga@pragxis.cl>
* @version 0.0
*/
class Servicio
{
public:
	/**
	* Constructor por defecto
	*/
	Servicio(void);
	/**
	* Constructor
	*/
	Servicio(string nombre, string origen, string destino, string color);
	/**
	* Destructor
	*/
	~Servicio(void);

	string nombre;
	string origen;
	string destino;
	string color;
    string direccion;

	string horarioI;
	string horarioR;
    
    string servicioSentidoVariante;
    
    map< string , string > variantes;
};
