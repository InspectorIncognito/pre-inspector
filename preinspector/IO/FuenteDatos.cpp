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
#include "FuenteDatos.h"

FuenteDatos::FuenteDatos(void)
{
	///Creacion de objeto para manejo de tiempo
	tsh = TimeStampHandler();
}

FuenteDatos::~FuenteDatos(void)
{
}

FuenteDatos::FuenteDatos(const char *nombreArchivoParametros)
{
	///Creacion de objeto para manejo de tiempo
	tsh = TimeStampHandler();

	///Lectura de Parametros
	this->parametros = new Parametros(nombreArchivoParametros);

    system(string("mkdir "+parametros->outputPath+"/"+parametros->version).c_str());
    
	outParameters.open(string(parametros->outputPath+"/"+parametros->version + "/PhoneParameters.csv").c_str());
	outParameters.precision(10);
	outParameters << "key;value" << endl;

    
	///Lectura de diccionario de codigos servicio-sentido
// 	leeDiccionarioServicios();

	///Lectura de las rutas
	leeRutas();

	///Lectura de la red de paraderos
	leeRedDeParadas();
 
    ///Instancia de lectura de secuencia de paraderos
	//leeSecuenciaDeParadas();

	///
	leeSecuenciaDeParadasDTPM();
    
 

}

void FuenteDatos::leeRutas()
{
    int nTimeStart = Cronometro::GetMilliCount();

	///Archivo de entrada Principal
	ifstream archivoRutas;
	archivoRutas.open(parametros->nombreArchivoRutas);

	///Chequeo de archivo 
	if (!archivoRutas.good())
		cout << "Error : No se encuentra el archivo " << parametros->nombreArchivoRutas << "!" << endl;
	else
		cout << "Cargando datos de Rutas (" << parametros->nombreArchivoRutas << ")... ";

	///Vector contenedor de la linea actual del archivo
	vector<string> cur;

	int nlineas = 0;

	///Lectura del header
	cur = StringFunctions::ExplodeF(';', &archivoRutas);

	string antServicio = string("-");
	bool unavez = true;
	map< string, Ruta >::iterator iserv_ant;
	Vector3D nodo_ant;
	int corr_ant;

	double x, y;
	char UTMZone[5];

	///Lectura archivo primario
	while (archivoRutas.good())
	{
		nlineas++;

		///Lectura de linea del archivo
		cur = StringFunctions::ExplodeF(';', &archivoRutas);

		///Condicion de salida, a veces no es suficiente solo la condicion del ciclo
		if (cur.size() == 0 || cur[0].compare("") == 0)
			continue;


        string code = cur.at(1);
        
        dicSS.servicios_2_usuario[cur.at(1)] = cur.at(2);


        map<string, Servicio>::iterator iit = serviciosBase.find(cur[5]);
        if(iit == serviciosBase.end())
        {
            Servicio servicio;
            servicio.nombre = cur[5];
            servicio.variantes[cur[5]] = cur[1];
            servicio.direccion = string("-");
            
            serviciosBase[cur[5]] = servicio;
        }
        else
        {
            int antes = cur[8].size();
            //string direccion = StringFunctions::translateCharacters(cur[8]);
            string direccion = cur[8];

            bool cambio=false;
            if((int)direccion.size() != antes)
                cambio=true;
            
//             direccion = std::regex_replace (direccion,regex("pealolen"),"penalolen");
//             direccion = std::regex_replace (direccion,regex("uble"),"nuble");
//             direccion = std::regex_replace (direccion,regex("maana"),"manana");
//             direccion = std::regex_replace (direccion,regex("uoa"),"nunoa");
//             direccion = std::regex_replace (direccion,regex("egaa"),"egana");
//             direccion = std::regex_replace (direccion,regex("espaa"),"espana");
//             direccion = std::regex_replace (direccion,regex("mamias"),"maminas");
            
            if(cambio)
                palabrasTraducidas[cur[8]] = direccion;
            
             (*iit).second.variantes[cur[1]] = direccion;
             
             
        }
        
        
		dicSS.colores[cur[1]] = cur[9];

        int x = atoi(cur.at(6).c_str());
        int y = atoi(cur.at(7).c_str());
		
		map< string, Ruta >::iterator iserv = rutas.mapeo->find(code);

		if (iserv == rutas.mapeo->end())
		{
			Ruta ruta = Ruta(code);

			bool ok = ruta.AgregarNodo(Vector3D(x,y,0.0),atoi(cur[0].c_str()));

			rutas.mapeo->insert(pair<string, Ruta >(code, ruta));
		}
		else
		{
			bool ok = (*iserv).second.AgregarNodo(Vector3D(x,y,0.0),atoi(cur[0].c_str()));
		}

		///Parche para usar el ultimo nodo (Nodo Fin)
		if (unavez)
		{
			antServicio = code;
			iserv_ant = rutas.mapeo->find(code);
			unavez = false;
		}

		if (antServicio.compare(code) != 0)
		{
			antServicio = code;
			bool ok = (*iserv_ant).second.AgregarNodo(nodo_ant, corr_ant + 1);
			iserv_ant = rutas.mapeo->find(code);
		}

		nodo_ant = Vector3D(x, y, 0.0);
		corr_ant = atoi(cur[0].c_str());
	}

	//output->SimplificaRutas();
	rutas.CalculaLargosRutas();
	rutas.CalculaTramificado(parametros->distanciaTramado);
	rutas.SimplificaRutas();

    ///Detecto direccion de servicios_base
    for(map<string, Servicio>::iterator iit = serviciosBase.begin();iit != serviciosBase.end();iit++)
    {
        for(map<string,string>::iterator iiit = (*iit).second.variantes.begin();iiit != (*iit).second.variantes.end();iiit++)
        {
            if( (*iit).first.compare((*iiit).first) == 0)
            {
                (*iit).second.direccion = (*iiit).second;
                
                std::locale loc;
                std::string str = (*iit).second.direccion;
                for (std::string::size_type i = 0; i < (*iit).second.direccion.length(); ++i)
                    str[i] = std::toupper((*iit).second.direccion[i], loc);
                
                vector<string> od = StringFunctions::Explode(str, '-');
                
                if ((int)od.size()==2)
                {
                    (*iit).second.destino = od[1].substr(1, od[1].length());
                    (*iit).second.origen = od[0].substr(0,od[0].length()-1);
                }
                else
                {
                    (*iit).second.destino = str;
                    (*iit).second.origen = str;
                }
            }
        }
    }
    
    ///DEBUG
    ofstream debug;
    debug.open("servicios_base" + parametros->version + ".txt");
    for(map<string, Servicio>::iterator iit = serviciosBase.begin();iit != serviciosBase.end();iit++)
    {
        debug << (*iit).first << " || " << (*iit).second.direccion << " || " << (*iit).second.destino << endl;
        debug << "-----------------------------------------------" << endl;
        for(map<string,string>::iterator iiit = (*iit).second.variantes.begin();iiit != (*iit).second.variantes.end();iiit++)
        {
            debug << "  " << (*iiit).first << " | " << (*iiit).second << endl;
        }
        debug << endl;
    }
    debug.close();
    
    ///DEBUG
    ofstream debug1;
    debug1.open("palabrasTraducidas" + parametros->version + ".txt");
    for(map<string, string>::iterator iit = palabrasTraducidas.begin();iit != palabrasTraducidas.end();iit++)
    {
        debug1 << (*iit).first << " | " << (*iit).second << endl;
    }
    debug1.close();
        
	cout << Cronometro::GetMilliSpan(nTimeStart) / 60000.0 << "(min)" << endl;

}

void FuenteDatos::leeRedDeParadas()
{
	int nTimeStart = Cronometro::GetMilliCount();

	int iParCodigo = 6;
	int iParCodigoVariante = 4;
	int iParCodigoServicio = 2;
	int iParCodigoSentido = 3;
	int iParX = 12;
	int iParY = 13;
	int iParNombre = 14;
	int iParCodigoUsuario = 7;
	int iParComuna = 8;
	int iParSecuencia = 0;
    
	///Archivo de entrada Principal
	ifstream archivoParaderos;
	archivoParaderos.open(parametros->nombreArchivoConsolidadoDeParadas.c_str());

	///Chequeo de archivo 
	if (!archivoParaderos.good())
		cout << "Error : No se encuentra el archivo " << parametros->nombreArchivoConsolidadoDeParadas << "!" << endl;
	else
		cout << " Cargando red de Paraderos... " << endl;

	///Vector contenedor de la linea actual del archivo
	vector<string> cur;

	///Iterador para buscar los paraderos
	map< string, Paradero >::iterator itPar;

	int nlineas = 0;

	///Lectura del header
	cur = StringFunctions::ExplodeF(';', &archivoParaderos);

    double x, y;
	char *UTMZone = "19H";

	///Lectura archivo primario
	while (archivoParaderos.good())
	{
		nlineas++;

		///Lectura de linea del archivo
		cur = StringFunctions::ExplodeF(';', &archivoParaderos);

		///Condicion de salida, a veces no es suficiente solo la condicion del ciclo
		if (cur.size() == 0 || cur[0].compare("") == 0)
		{
			//mde_->Advertencia("Advertencia : se ha encontrado linea vacia en " + parametros->nombreArchivoRedParadas + "!\n");
			continue;
		}

		if (cur[iParCodigo].compare("POR DEFINIR") == 0 || cur[iParCodigo].compare("POR DEFINIR ") == 0)
			continue;
            
		x = atoi(cur[iParX].c_str());
        y = atoi(cur[iParY].c_str());
        
// 		ConvertCoordinate::LLtoUTM(23, atof(cur[3].c_str()), atof(cur[4].c_str()), y, x, UTMZone);

		//stop_id,stop_code,stop_name,stop_lat,stop_lon
		///estandarizacion del nombre
		//Quitar el código de paradero(ya hecho)
		//Quitar los slash(/ )
		//Quitar el punto de la palabra "Esq." y que la "E" sea minúscula.Resultado: "Esq." -> "esq"
		//Quitar "@"
		string nombre = cur[iParNombre];
		nombre.erase(std::remove(nombre.begin(), nombre.end(), '/'), nombre.end());
		//nombre.erase(std::remove(nombre.begin(), nombre.end(), '.'), nombre.end());
        nombre = std::regex_replace (nombre,regex("\\.")," ");
		nombre.erase(std::remove(nombre.begin(), nombre.end(), '@'), nombre.end());
        
//         cout << nombre << endl;
        
        nombre = StringFunctions::EliminaCadenasBlancos(nombre);
        nombre = std::regex_replace (nombre,regex("Esq"),"/");
        nombre = std::regex_replace (nombre,regex("esq"),"/");
        
        nombre = std::regex_replace (nombre,regex("Parada 1 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 2 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 3 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 4 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 5 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 6 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 7 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 8 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 9 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 10 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 11 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 12 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 13 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 14 "),"");
        nombre = std::regex_replace (nombre,regex("Parada 15 "),"");
        nombre = std::regex_replace (nombre,regex("Parada "),"");

        nombre = std::regex_replace (nombre,regex("Avenida "),"Av ");
        
        nombre = std::regex_replace (nombre,regex("Av "),"");
        
        vector<string> invertir = StringFunctions::Explode(nombre,'/');
        
        if((int)invertir.size() == 2)
            nombre = invertir[1] + " / " + invertir[0];
        
        nombre = StringFunctions::EliminaCadenasBlancos(nombre);
        
        
        
        
//         cout << nombre << endl;
//        size_t index = 0;
//        index = nombre.find("Esq", index); if (index != std::string::npos){nombre.replace(index, 3, "-");}

        double lat, lon;
        ConvertCoordinate::UTMtoLL(23, y, x, UTMZone, lat, lon);

		Paradero par = Paradero(lat, lon, x,  y, cur[iParCodigoUsuario], nombre);

		redParaderos.red[par.codigo] = par;
	}

	CorrigeParadasMismaPosicion();

	///DEBUG
// 	ofstream fout;
// 	fout.open("redParadas.txt");
// 	for(map< string , Paradero >::iterator it=redParaderos.red.begin();it!=redParaderos.red.end();it++)
// 	{
// 		fout << (*it).second.codigo << "|";
// 		fout << (*it).second.nombre << "|";
// 		fout << (*it).second.x << "|";
// 		fout << (*it).second.y << "|";
// 		fout << (*it).second.lat << "|";
// 		fout << (*it).second.lon << "|";
// 		fout << endl;
// 	}
// 	fout.close();

	cout << Cronometro::GetMilliSpan(nTimeStart) / 60000.0 << "(min)" << endl;
}

void FuenteDatos::leeSecuenciaDeParadasDTPM()
{
	int nTimeStart = Cronometro::GetMilliCount();

	map<string, SecParadas>::iterator isec;

	///Archivo de entrada Principal
	ifstream archivoParaderos;
	archivoParaderos.open(parametros->nombreArchivoConsolidadoDeParadas.c_str());

	///Chequeo de archivo
	if (!archivoParaderos.good())
		cout << "No se encuentra el archivo " << parametros->nombreArchivoConsolidadoDeParadas << "!" << endl;
	else
		cout << "Cargando datos de Secuencia de Paraderos (" << parametros->nombreArchivoConsolidadoDeParadas << ")... " << endl;


	///Vector contenedor de la linea actual del archivo
	vector<string> cur;

	int nlineas = 0;

	///Lectura del header
	cur = StringFunctions::ExplodeF(';', &archivoParaderos);
	 
	int iParCodigo = 6;
	int iParCodigoVariante = 4;
	int iParCodigoServicio = 2;
	int iParCodigoSentido = 3;
	int iParX = 12;
	int iParY = 13;
	int iParNombre = 11;
	int iParCodigoUsuario = 7;
	int iParComuna = 8;
	int iParSecuencia = 0;

	///Lectura archivo primario
	while (archivoParaderos.good())
	{
		nlineas++;

		///Lectura de linea del archivo
		cur = StringFunctions::ExplodeF(';', &archivoParaderos);

		///Condicion de salida, a veces no es suficiente solo la condicion del ciclo
		if (cur.size() == 0 || cur[0].compare("") == 0)
			continue;

		if (cur[iParCodigo].compare("POR DEFINIR") == 0 || cur[iParCodigo].compare("POR DEFINIR ") == 0)
			continue;


                
		map < string, Paradero >::iterator ired;
		ired = redParaderos.red.find(cur[iParCodigoUsuario]);
		if (ired == redParaderos.red.end())
		{
			cout << "PARADERO NO ENCONTRADO EN GTFS : " << cur[iParCodigoUsuario] << endl;
		}

        string sentido;
        if(cur[iParCodigoSentido].compare("Ida")==0 || cur[iParCodigoSentido].compare("I")==0 || cur[iParCodigoSentido].compare("ida")==0)
            sentido = "I";
        else
            sentido = "R";
            
		///Generacion del codigo servicio-sentido concatenando las 3 columnas servicio-sentido-variante
		string codigoServicioSentido;
        string codigoServicio = string(cur[iParCodigoServicio] + sentido);

		if (cur[iParCodigoVariante].compare("-") == 0 || cur[iParCodigoVariante].compare("") == 0)
			codigoServicioSentido = string(cur[iParCodigoServicio] + sentido);
		else
			codigoServicioSentido = string(cur[iParCodigoServicio] + sentido + cur[iParCodigoVariante]);

		isec = secuenciaDTPM.find(codigoServicioSentido);

        ////COLOR
		string color;
		map<string, string>::iterator iiit = dicSS.colores.find(codigoServicioSentido);
		if (iiit != dicSS.colores.end())
			color = (*iiit).second;
		else
		{
			cout << "ERROR : No se encontro el servicio " << codigoServicioSentido << " en la tabla de colores." << endl;
			color = "0";
		}

		if (isec == secuenciaDTPM.end())
		{
			SecParadas sec;

			sec.servicio = cur[iParCodigoServicio];
            
			if(cur[iParCodigoSentido].compare("Ida")==0)
				sec.sentido = "I";
			else
				sec.sentido = "R";
            
            sec.routeCode = sec.servicio+sec.sentido;

			sec.color = color;
			sec.secuencia += cur[iParCodigoUsuario];
            sec.secuenciav.push_back(cur[iParCodigoUsuario]);

            
			map< string, Servicio >::iterator iserv = serviciosBase.find(codigoServicioSentido);
			if (iserv != serviciosBase.end())
			{
				sec.nombre = toCamelCase((*iserv).second.destino);
			}
			///Caso que no encuentra el base, busca en las variantes
			else
            {
                map< string, Servicio >::iterator iserv2 = serviciosBase.find(codigoServicio);
                if(iserv2 != serviciosBase.end() )
                {
                    map<string,string>::iterator iiiit = (*iserv2).second.variantes.find(codigoServicioSentido);
                    if(iiiit != (*iserv2).second.variantes.end())
                    {
                        sec.nombre = toCamelCase((*iserv2).second.destino);
                    }
                    ///Caso que no encuentre la variante (Parche, error de dato, se busca en una posible base)
                    else
                    {
                        codigoServicio.erase(std::remove(codigoServicio.begin(), codigoServicio.end(), 'e'), codigoServicio.end());
                        codigoServicio.erase(std::remove(codigoServicio.begin(), codigoServicio.end(), 'v'), codigoServicio.end());
                        codigoServicio.erase(std::remove(codigoServicio.begin(), codigoServicio.end(), 'c'), codigoServicio.end());

                        map< string, Servicio >::iterator iserv3 = serviciosBase.find(codigoServicio);
                        if(iserv3 != serviciosBase.end() )
                        {
                            sec.nombre = toCamelCase((*iserv3).second.destino);
                        }
                        
                    }
                }
                
            }
            
			if (cur[iParCodigoVariante].compare("-") != 0 && cur[iParCodigoVariante].compare("") != 0)
            {
				sec.nombre += " (" + cur[iParCodigoVariante] + ")";
                sec.routeCode += cur[iParCodigoVariante];
            }

			secuenciaDTPM[codigoServicioSentido] = sec;

		}
		else
		{
			(*isec).second.secuencia += "-" + cur[iParCodigoUsuario];
            (*isec).second.secuenciav.push_back(cur[iParCodigoUsuario]);
		}

		///Construccion de secuencia
		///Busco paradero en red de paradas
		Paradero par;
		ired = redParaderos.red.find(cur[iParCodigoUsuario]);
		if (ired != redParaderos.red.end())
		{
			par = (*ired).second;
		}
		else
		{
			cout << "ERROR : paradero " << cur[iParCodigoUsuario] << " de stops_times.txt no se encuentra en stops.txt!" << endl;
			continue;
		}

		map< string, map<int,Paradero> >::iterator iserv;
		iserv = secParaderosTODOS.secuencias.find(codigoServicioSentido);
		if (iserv == secParaderosTODOS.secuencias.end())
		{
			map<int, Paradero> tmp;
			tmp[nlineas] = par;
			secParaderosTODOS.secuencias[codigoServicioSentido] = tmp;
		}
		else
		{
			(*iserv).second[nlineas] = par;
		}
	}

	///TEST
	/*
	for (map<string, Secuencia >::iterator isec = secuencias.begin(); isec != secuencias.end(); isec++)
	{
		cout << (*isec).first << "|";
		cout << (*isec).second.codigo << "|";
		cout << (*isec).second.nombre << endl;
	}
	*/


	///DEBUG
	ofstream fout;
	fout.open(string(parametros->outputPath+"/"+parametros->version + "/PhoneBusStopsSequences_dtpm.csv").c_str());
	fout << "id_ruta;servicio;sentido;color_id;direccion;paradas;color_hex" << endl;
	for (isec = secuenciaDTPM.begin(); isec != secuenciaDTPM.end(); isec++)
	{
        fout << (*isec).second.routeCode << ";";
		fout << (*isec).second.servicio << ";";
		fout << (*isec).second.sentido << ";";
        
        map<string, string>::iterator icolor = parametros->mapeoColores.find((*isec).second.color);
        if(icolor!=parametros->mapeoColores.end())
            fout << (*icolor).first << ";";
        else
        {
            fout << "-" << ";";
            cout << "ERROR : No se encuetra operador en tabla de colores : " << (*isec).second.color << endl;
            cout << (*isec).second.routeCode << ";";
            cout << (*isec).second.servicio << ";";
            cout << (*isec).second.sentido << ";";
            cout << StringFunctions::EliminaCadenasBlancos((*isec).second.nombre) << ";";
            cout << (*isec).second.secuencia << endl;
        }
        
		//fout << toCamelCase((*isec).second.nombre) << ";";
		fout << StringFunctions::EliminaCadenasBlancos((*isec).second.nombre) << ";";
		fout << (*isec).second.secuencia << ";";
        fout << "#" << (*icolor).second  << endl;
		
	}
	fout.close();
    
	ofstream fout1;
	fout1.open("servicios" + parametros->version + ".dbg");
	fout1 << "servicio;sentido;color_id;direccion" << endl;
	for (isec = secuenciaDTPM.begin(); isec != secuenciaDTPM.end(); isec++)
	{
		fout1 << (*isec).second.servicio << ";";
		fout1 << (*isec).second.sentido << ";";
		fout1 << (*isec).second.color << ";";
		fout1 << StringFunctions::EliminaCadenasBlancos((*isec).second.nombre) << endl;
		
	}
	fout1.close();

	cout << Cronometro::GetMilliSpan(nTimeStart) / 60000.0 << endl;
}

string FuenteDatos::toCamelCase(string in)
{
	string out;
	std::locale loc;

	for (int i = 0; i < in.size(); i++)
	{
		if (i == 0)
		{
			out.push_back(std::toupper(in.at(i), loc));
		}
		else
		{
			int ant = i - 1;
			if (in.at(ant) == ' ' || in.at(ant) == '(')
				out.push_back(std::toupper(in.at(i), loc));
			else
				out.push_back(std::tolower(in.at(i), loc));

		}
	}

	return out;
}

void FuenteDatos::CorrigeParadasMismaPosicion()
{
	int nTimeStart = Cronometro::GetMilliCount();

	cout << "Proceso X : Correccion de paradas misma posicion... ";

	char *UTMZone = "19H";
	///Ciclo sobre secuencia de paradas
	for (map<string, Paradero>::iterator ipar1 = redParaderos.red.begin(); ipar1 != redParaderos.red.end(); ipar1++)
	{
		for (map<string, Paradero>::iterator ipar2 = redParaderos.red.begin(); ipar2 != redParaderos.red.end(); ipar2++)
		{
			if ((*ipar1).first.compare((*ipar2).first) != 0)
			{
				float dx = (*ipar1).second.x - (*ipar2).second.x;
				float dy = (*ipar1).second.y - (*ipar2).second.y;
				float dist = sqrt(dx*dx + dy*dy);

				if (dist < 5)
				{
					if(abs(dx) <= abs(dy))
					{
						if ((*ipar1).second.x >= (*ipar2).second.x)
							(*ipar1).second.x += 5;
						else
							(*ipar1).second.x -= 5;

						double lat, lon;
						ConvertCoordinate::UTMtoLL(23, (*ipar1).second.y, (*ipar1).second.x, UTMZone, lat, lon);

						(*ipar1).second.lat = lat;
						(*ipar1).second.lon = lon;
					}
					else
					{
						if ((*ipar1).second.y >= (*ipar2).second.y)
							(*ipar1).second.y += 5;
						else
							(*ipar1).second.y -= 5;

						double lat, lon;
						ConvertCoordinate::UTMtoLL(23, (*ipar1).second.y, (*ipar1).second.x, UTMZone, lat, lon);

						(*ipar1).second.lat = lat;
						(*ipar1).second.lon = lon;
					}
					//cout << (*ipar1).first << "|" << (*ipar2).first << "|" << dist << endl;
				}
			}
		}
	}

	cout << Cronometro::GetMilliSpan(nTimeStart) / 60000.0 << "(min)" << endl;
}

// void FuenteDatos::leeDiccionarioServicios()
// {
// 	int nTimeStart = Cronometro::GetMilliCount();
// 
// 	///Archivo de entrada Principal
// 	ifstream archivoDiccionario;
// 	archivoDiccionario.open(parametros->nombreCarpetaGTFS + parametros->slash + "routes.txt");
// 
// 	///Chequeo de archivo 
// 	if (!archivoDiccionario.good())
// 		cout << "Error : No se encuentra el archivo " << parametros->nombreCarpetaGTFS + parametros->slash + "routes.txt" << "!" << endl;
// 	else
// 		cout << "Cargando datos de Diccionario (" << parametros->nombreCarpetaGTFS + parametros->slash + "routes.txt" << ")... ";
// 
// 	///Vector contenedor de la linea actual del archivo
// 	vector<string> cur;
// 
// 	int nlineas = 0;
// 
// 	///Lectura del header
// 	cur = StringFunctions::ExplodeF(',', &archivoDiccionario);
// 
// 
// 	///Lectura archivo primario
// 	while (archivoDiccionario.good())
// 	{
// 		nlineas++;
// 
// 		///Lectura de linea del archivo
// 		cur = StringFunctions::ExplodeF(',', &archivoDiccionario);
// 		
// 		///Condicion de salida, a veces no es suficiente solo la condicion del ciclo
// 		if (cur.size() == 0 || cur[0].compare("") == 0)
// 			continue;
// 
// 		std::locale loc;
// 		std::string str = cur[3];
// 		for (std::string::size_type i = 0; i < cur[3].length(); ++i)
// 			str[i] = std::toupper(cur[3][i], loc);
// 
// 		vector<string> od = StringFunctions::Explode(str, '-');
// 
// 		Servicio ser;
// 		if (od.size() == 2)
// 			ser = Servicio(cur[0], od[0].substr(0,od[0].length()-1), od[1].substr(1, od[1].length()), cur[7]);
// 		else if (od.size() == 1)
// 			ser = Servicio(cur[0], od[0].substr(0, od[0].length() - 1), "", cur[7]);
// 		else
// 			cout << "ERROR : Servicio no bien definido en datos de entrada(routes.txt)!" << endl;
// 
// 		servicios[ser.nombre] = ser;
// 
// 		///Insercion en diccionario de servicios-sentidos global
// 		dicSS.servicios[string(cur[0] + "I")] = cur[0];
// 		dicSS.servicios[string(cur[0] + "R")] = cur[0];
// 
// 		///identificacion de colores
// 		map<string, string>::iterator icolor = parametros->mapeoColores.find(cur[7]);
// 
// 		if (icolor != parametros->mapeoColores.end())
// 		{
// 			dicSS.colores[cur[0]] = (*icolor).second;
// 
// 		}
// 	}
// 
// // 	///DEBUG
// // 	ofstream fout;
// // 	fout.open("dicSS.out");
// // 	for (map<string, string>::iterator it = dicSS.servicios.begin(); it != dicSS.servicios.end(); it++)
// // 		fout << (*it).first << ";" << (*it).second << endl;
// // 	fout.close();
// 
// 	cout << Cronometro::GetMilliSpan(nTimeStart) / 60000.0 << "(min)" << endl;
// 
// }

// void FuenteDatos::leeSecuenciaDeParadas()
// {
// 	int nTimeStart = Cronometro::GetMilliCount();
// 
//     int iParCodigo = 6;
// 	int iParCodigoVariante = 4;
// 	int iParCodigoServicio = 2;
// 	int iParCodigoSentido = 3;
// 	int iParX = 12;
// 	int iParY = 13;
// 	int iParNombre = 11;
// 	int iParCodigoUsuario = 7;
// 	int iParComuna = 8;
// 	int iParSecuencia = 0;
// 
//     
// 	///Archivo de entrada Principal
// 	ifstream archivoParaderos;
// 	archivoParaderos.open(parametros->nombreArchivoConsolidadoDeParadas.c_str());
// 
// 	///Chequeo de archivo 
// 	if (!archivoParaderos.good())
// 		cout << "Error : No se encuentra el archivo " << parametros->nombreArchivoConsolidadoDeParadas << "!" << endl;
// 	else
// 		cout << "Cargando datos de Secuencia de Paraderos (" << parametros->nombreArchivoConsolidadoDeParadas << ")... ";
// 
// 	///Vector contenedor de la linea actual del archivo
// 	vector<string> cur;
// 
// 	int nlineas = 0;
// 
// 	///Lectura del header
// 	cur = StringFunctions::ExplodeF(';', &archivoParaderos);
// 
// 	map< string, map<int,Paradero> >::iterator iserv;
// 	map < string, Paradero >::iterator ired;
// 
// 	bool activo = false;
// 	string servicio_ant = string("-");
// 
// 	map<string, Ruta>::iterator iRuta;
// 
// 	///Lectura archivo primario
// 	while (archivoParaderos.good())
// 	{
// 		nlineas++;
// 
// 		///Lectura de linea del archivo
// 		cur = StringFunctions::ExplodeF(';', &archivoParaderos);
// 
// 		///Condicion de salida, a veces no es suficiente solo la condicion del ciclo
// 		if (cur.size() == 0 || cur[0].compare("") == 0)
// 			continue;
// 
//         if (cur[iParCodigo].compare("POR DEFINIR") == 0 || cur[iParCodigo].compare("POR DEFINIR ") == 0)
// 			continue;
//         
//         ///Generacion del codigo servicio-sentido concatenando las 3 columnas servicio-sentido-variante
// 		string codigoServicioSentido;
//         
//         string sentido;
//         if(cur[iParCodigoSentido].compare("Ida")==0)
//             sentido = "I";
//         else
//             sentido = "R";
// 
// 		if (cur[iParCodigoVariante].compare("-") == 0 || cur[iParCodigoVariante].compare("") == 0)
// 			codigoServicioSentido = string(cur[iParCodigoServicio] + sentido);
// 		else
// 			codigoServicioSentido = string(cur[iParCodigoServicio] + sentido + cur[iParCodigoVariante]);
//         
// 		string servicio = codigoServicioSentido;
// 
// 		//if (servicio.compare("D03I") == 0 && cur[3].compare("PD219")==0)
// 		//	cout << "PAR1 : " << cur[3] << endl;
// 
// 		if (servicio.compare(servicio_ant) != 0)
// 			activo = true;
// 
// 		if (servicio.compare(servicio_ant) == 0 && cur[4].compare("1") == 0)
// 			activo = false;
// 
// 		//if (servicio.compare("I08I") == 0)
// 		//	cout << "PAR2 : " << cur[3] << endl;
// 
// 		///Busco paradero en red de paradas
// 		Paradero par;
// 		ired = redParaderos.red.find(cur[iParCodigoUsuario]);
// 		if (ired != redParaderos.red.end())
// 		{
// 			par = (*ired).second;
// 		}
// 		else
// 		{
// 			cout << "ERROR : paradero " << cur[iParCodigoUsuario] << " de secuencia no se encuentra en " << parametros->nombreArchivoConsolidadoDeParadas << endl;
// 			continue;
// 		}
// 
// 		iRuta = rutas.mapeo->find(servicio);
// 		Vector3D p = Vector3D((*ired).second.x, (*ired).second.y, 0.0);
// 		float distance = -1;
// 		if (iRuta != rutas.mapeo->end())
// 		{
// 			distance = (*iRuta).second.GetDistanceOnRoute(&p);
// 			//cout << distance << endl;
// 		}
// 
// 		if (activo)
// 		{
// 			///Construccion de secuencia
// 			iserv = secParaderos.secuencias.find(servicio);
// 			if (iserv == secParaderos.secuencias.end())
// 			{
// 				map<int, Paradero> tmp;
// 				tmp[int(distance)] = par;
// 				secParaderos.secuencias[servicio] = tmp;
// 			}
// 			else
// 			{
// 				(*iserv).second[int(distance)] = par;
// 			}
// 		}
// 
// 		servicio_ant = servicio;
// 	}
// 
// 	///DEBUG
// // 	ofstream fout;
// // 	fout.open("secParadas.txt");
// // 	for (iserv = secParaderos.secuencias.begin(); iserv != secParaderos.secuencias.end(); iserv++)
// // 	{
// // 		for (map<int, Paradero>::iterator ipar = (*iserv).second.begin(); ipar != (*iserv).second.end(); ipar++)
// // 		{
// // 			fout << (*iserv).first << ";";
// // 			fout << (*ipar).first << ";";
// // 			fout << (*ipar).second.codigo << ";";
// // 			fout << (*ipar).second.nombre << endl;
// // 		}
// // 	}
// // 	fout.close();
// 
// 	///Variables para reporte
// 	cout << Cronometro::GetMilliSpan(nTimeStart) / 60000.0 << "(min)" << endl;
// }
