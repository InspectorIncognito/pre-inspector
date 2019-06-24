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
#include "TablaRutas.h"


TablaRutas::TablaRutas(FuenteDatos *fdd)
{
	this->fdd_ = fdd;

	Crear();
}

TablaRutas::~TablaRutas()
{

}

void TablaRutas::Crear()
{
	int nTimeStart = Cronometro::GetMilliCount();

	cout << "Proceso X : Creando e imprimiendo tabla rutas... " ;

	///Impresion de la tabla
	ofstream fileout;
    fileout.open(string(fdd_->parametros->outputPath+"/"+fdd_->parametros->version + "/ServerShapes.csv").c_str());
	fileout << "servicio;";
	fileout << "latitud;";
	fileout << "longitud;";
	fileout << "correlativo";
	fileout << endl;

	map<string, Ruta>::iterator iRuta;
	map<string, string>::iterator iDicServ;
	for (map<string, Ruta>::iterator iruta = fdd_->rutas.mapeo->begin(); iruta != fdd_->rutas.mapeo->end(); iruta++)
	{
			for (map<int, Vector3D>::iterator inodo = (*iruta).second.nodosSimplificados->begin(); inodo != (*iruta).second.nodosSimplificados->end(); inodo++)
			{
				double lat, lon;

				ConvertCoordinate::UTMtoLL(23, (*inodo).second.y, (*inodo).second.x, UTMZONE, lat, lon);

				string strlat = StringFunctions::Double2String(lat, 10);
				string strlon = StringFunctions::Double2String(lon, 10);

				fileout << (*iruta).first << ";";
				fileout << strlat << ";";
				fileout << strlon << ";";
				fileout << (*inodo).first << endl;
			}
	}

	fileout.close();
	cout << Cronometro::GetMilliSpan( nTimeStart )/60000.0 << "(min)" << endl;
}
