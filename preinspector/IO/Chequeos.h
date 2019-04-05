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
#include <fstream>
#include <vector>
#include <string>
#include <locale>
#include <algorithm>
#include <regex>
#include "FuenteDatos.h"
#include "../Utilidades/TimeStampHandler.h"
#include "../Utilidades/StringFunctions.h"
#include "../Utilidades/ConvertCoordinate.h"
#include "../Utilidades/Cronometro.h"


using namespace std;

/**
* Title: FuenteDatos
* Description:	Clase contenedora de los datos de entrada, tiene estructuras y los metodos 
* 		de lectura de estos especificos de Transantiago.
* Copyright: Pragxis (c) 2016
* Date: 18-06-2016
* @author Mauricio Zuñiga
* @version 0.0
*/
class Chequeos{
  public:
	/**
	 * Constructor por defecto
	 */
	Chequeos(void);
	/**
	 * Destructor por defecto
	 */
	~Chequeos(void);

    void chequeaParaderos(FuenteDatos* fd);
};
