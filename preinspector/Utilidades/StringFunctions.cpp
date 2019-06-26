/**
* Project: "pvm".
* Package: Utilidades
* Class: StringFunctions
* Created by: Mauricio Zuñiga G. <mzuniga@pragxis.cl>
* Terms: These coded instructions, statements, and computer programs are
* produced as part of a software project. They contain unpublished
* propietary information and are protected by copyright law. They may
* not be disclosed to third parties or copied or duplicated in any form,
* in whole or in part, without the prior written consent of Pragxis SpA.
* Copyright:  Pragxis (c) 2013
* Last modified : Mauricio Zuñiga 15-04-2013
*/
#include "StringFunctions.h"

StringFunctions::StringFunctions()
{
}


StringFunctions::~StringFunctions()
{
}

// string StringFunctions::translateCharacters(string word)
// {
//      std::locale loc;
//      std::string str = word;
//      for (std::string::size_type i = 0; i < word.length(); ++i)
//          str[i] = std::tolower(word[i], loc);
// 
//     std::string output;
//     for (std::string::size_type i=0; i < str.length(); i++)
//     {
//         //cout << str[i] << "|" << (int)str[i] << endl;
//         
//         if( (str[i] >= 'A' && str[i] <= 'Z') 
//         ||  (str[i] >= 'a' && str[i] <= 'z') 
//         ||  (str[i] >= '0' && str[i] <= '9') 
//         ||  str[i] == '(' || str[i] == ')' || str[i] == '-' || str[i] == ' ' || str[i] == '/' || str[i] == '.'
//         )
//         {
//             output.push_back(str[i]);
//         }
//         //else{
//         //    cout << "WTF1 : " << (int)str[i] << endl;
//        // }
//     }
//     
//     return output;
// }

bool StringFunctions::utf8_check_is_valid(const string& string)
{
    int c,i,ix,n,j;
    for (i=0, ix=string.length(); i < ix; i++)
    {
        c = (unsigned char) string[i];
        if (0x00 <= c && c <= 0x7f) n=0; // 0bbbbbbb
        else if ((c & 0xE0) == 0xC0) n=1; // 110bbbbb
        else if ( c==0xed && i<(ix-1) && ((unsigned char)string[i+1] & 0xa0)==0xa0) return false; //U+d800 to U+dfff
        else if ((c & 0xF0) == 0xE0) n=2; // 1110bbbb
        else if ((c & 0xF8) == 0xF0) n=3; // 11110bbb
        //else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
        //else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else return false;
        for (j=0; j<n && i<ix; j++) { // n bytes matching 10bbbbbb follow ?
            if ((++i == ix) || (( (unsigned char)string[i] & 0xC0) != 0x80))
                return false;
        }
    }
    return true;
}

string StringFunctions::EliminaCadenasBlancos(string in)
{
	string out;
	int nBlancos = 0;
	for (int i = 0; i < in.size(); i++)
	{
        if( i == 0 && in.at(0) == ' ')
            continue;
        
        if( i == (int)in.size() - 1 && in.at((int)in.size() - 1) == ' ')
            continue;
        
		if (in.at(i) == ' ')
			nBlancos++;
		else
			nBlancos = 0;

		if (nBlancos < 2)
			out.push_back(in.at(i));
	}

	return out;
	//return in;
}

string StringFunctions::toCamelCase(string in)
{
	string out;
	std::locale loc;

    string tmp;
    for (int i = 0; i < in.size(); i++)
        tmp.push_back(std::tolower(in.at(i), loc));
    in = tmp;
    
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

string StringFunctions::ReplaceChar(string str, char ch1, char ch2)
{
  for (int i = 0; i < str.length(); ++i) {
    if (str[i] == ch1)
      str[i] = ch2;
  }

  return str;
}

string StringFunctions::toLatex(string str)
{
  string output=string("");

  for (int i = 0; i < str.length(); ++i) 
  {
    if (str[i] == '_')
		output.append(Char2String('\\')+Char2String(str[i]));
	else
		output.append(Char2String(str[i]));
  }

  return output;
}


vector<string> StringFunctions::ExplodeF(char separator , ifstream *f)
{
	char line[MAXLINE];
	f->getline(line,MAXLINE);

	string str = string(line);
	vector<string>  result;
	size_t pos1 = 0;
	size_t pos2 = 0;

	while ( pos2 != str.npos )
	{
		pos2 = str.find(separator, pos1);
			if ( pos2 != str.npos )
			{
				if ( pos2 > pos1 )
				{
					result.push_back( str.substr(pos1, pos2-pos1) );
				}
				else
				{
					result.push_back( string("-") );
					//cout << str.substr(pos1, pos2-pos1) << "|" << pos1 << "|" << pos2 << endl;
				}
				pos1 = pos2+1;
			}
	}
	result.push_back( str.substr(pos1, str.size()-pos1) );
	return result;
}





string StringFunctions::Double2String(double d,int p)
{
	ostringstream str;
	str.precision(p);
	str << d;
	return str.str();
}

string StringFunctions::Int2String(int d)
{
	ostringstream str;
	str << d;
	return str.str();
}

string StringFunctions::Char2String(char c)
{
	stringstream str;
	str << c;
	return str.str();
}

string StringFunctions::Long2String(long d)
{
	ostringstream str;
	str << d;
	return str.str();
}
