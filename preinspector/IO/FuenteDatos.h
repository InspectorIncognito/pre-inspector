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
#include "Parametros.h"
#include "../Utilidades/TimeStampHandler.h"
#include "../Utilidades/StringFunctions.h"
#include "../Utilidades/ConvertCoordinate.h"
#include "../Utilidades/Cronometro.h"
#include "../Transporte/Servicios/Servicio.h"
#include "../Transporte/Paraderos/SecuenciaParaderos.h"
#include "../Transporte/Paraderos/RedParadas.h"
#include "../Transporte/Rutas/Rutas.h"
#include "../Transporte/Grid/Grid.h"
// #include "../Transporte/CargaBip/PuntoBip.h"
#include "../Transporte/Diccionarios/DiccionarioServicios.h"


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
class FuenteDatos{
  public:
	/**
	*	Constructor de la clase
	*	@param file nombre del archivo de parametros externos 
	*/
	FuenteDatos(const char *file);
	/**
	 * Constructor por defecto
	 */
	FuenteDatos(void);
	/**
	 * Destructor por defecto
	 */
	~FuenteDatos(void);
    
	///Parametros externos, nombres de archivo, indices de columnas parametros para el proceso...
	Parametros* parametros;

	///Estructura de diccionario de codigos servicio-sentido
	DiccionarioServicios dicSS;

	///Estructura  de las rutas
	Rutas rutas;

	///Estructura con la red de paraderos
	RedParadas redParaderos;

	///Estructura de secuencia de paraderos
	SecuenciaParaderos secParaderosTODOS;
    
    struct SecParadas {
		string servicio;
		string sentido;
		string nombre;
		string color;
		string secuencia;
        string routeCode;
        vector<string> secuenciav;
	};

	map<string, SecParadas> secuenciaDTPM;

    ///Manejo de variantes
    map<string, Servicio> serviciosBase;

	///Grilla para ordenar informacion geograficamente
	Grid grid;

	///Archivo para parametros variados de salida
	ofstream outParameters;

	///Instancia de clase para manejo de tiempo
	TimeStampHandler tsh;

    ///debug, tabla con palabras que perdieron caracteres en la traduccion
    map<string,string> palabrasTraducidas;
    
	void leeDiccionarioServicios();

	void leeRutas();

	void leeRedDeParadas();
	
    void CorrigeParadasMismaPosicion();

	void leeSecuenciaDeParadas();

    void leeSecuenciaDeParadasDTPM();

	string toCamelCase(string in);
};
