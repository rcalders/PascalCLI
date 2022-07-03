#include "stdafx.h"
#include "utils.h"
#include <math.h>

int StrToInt (CString chaine)
{
	char* stopstring; 
	int resultat = _wtoi(chaine.GetString());
//	int resultat = strtol(chaine.GetString(), &stopstring, 10);
	return resultat;
}

double StrToFloat (CString chaine)
{
	char* stopstring;   
	double resultat = _wtof(chaine.GetString());
//	double resultat = (double)strtod((const char *)chaine.GetBuffer(), &stopstring);
	return resultat;
}

CString IntToStr (int count)
{
	char buffer[16];
	_itoa_s(count, buffer, 10);
	return (CString)buffer;
  
}

CString FloatToStr (double nombre)
{
	CString zeros = "0000000000000000";

	if (nombre==0) return ((CString)"0");

	// Signe de la mantisse

	CString signeMantisse = "";
	if (nombre<0) 
	{ 
		signeMantisse = "-";
		nombre=-nombre; 
	}
	
	// Détermination de l'exposant

	int exp=0;
	
	if (nombre<1)	
	{
		while (nombre<1)
		{
			nombre *= 10;
			exp-=1;
		}
	}
	
	if (nombre>=10)	
	{
		while (nombre>=10)
		{
			nombre /= 10;
			exp+=1;
		}
	}
	
	// Signe de l'exposant
	
	CString signeExposant = "";
	if (exp<0) 
	{ 
		signeExposant = "-";
		exp=-exp; 
	}
	
	// Calcule la mantisse
	
	unsigned long int Mantisse = (unsigned long int)(nombre * pow (10, 8));

	// Convertit en String

	char buffer[16];
	_itoa_s(Mantisse, buffer, 10);
	CString MantisseStr = buffer;
	MantisseStr = MantisseStr[0]+","+MantisseStr.Right(MantisseStr.GetLength()-1);

	_itoa_s(exp, buffer, 10);
	CString ExposantStr = buffer;
	ExposantStr = zeros.Left(2-ExposantStr.GetLength())+ExposantStr;

	CString rien=signeMantisse+MantisseStr+"E"+signeExposant+ExposantStr;
	return (rien);
}

CString HexaFloat (double nombre)
{
	CString zeros = "0000000000000000";

	if (nombre==0) return ((CString)"0");

	// Signe de la mantisse

	CString signeMantisse = "0";
	if (nombre<0) 
	{ 
		signeMantisse = "1";
		nombre=-nombre; 
	}
	
	// Détermination de l'exposant

	int exp=0;
	
	if (nombre<1)	
	{
		while (nombre<1)
		{
			nombre *= 10;
			exp-=1;
		}
	}
	
	if (nombre>=10)	
	{
		while (nombre>=10)
		{
			nombre /= 10;
			exp+=1;
		}
	}
	
	// Signe de l'exposant
	
	CString signeExposant = "0";
	if (exp<0) 
	{ 
		signeExposant = "1";
		exp=1000+exp; 
	}
	
	// Calcule la mantisse
	
	unsigned long int Mantisse = (unsigned long int)(nombre * pow (10, 8));

	// Convertit en String

	char buffer[16];
	_itoa_s(Mantisse, buffer, 10);
	CString MantisseStr = buffer;
	MantisseStr += "000";

	_itoa_s(exp, buffer, 10);
	CString ExposantStr = buffer;
	ExposantStr = zeros.Left(3-ExposantStr.GetLength())+ExposantStr;

	CString rien="#"+signeMantisse+MantisseStr+ExposantStr;
	return (rien);
}

CString HexaInteger (int nombre)
{
	char buffer[16];
	if (nombre<0) nombre+=1048576;
	_itoa_s(nombre, buffer, 16);
	CString Zeros = "00000";
	CString res = (CString)"#"+Zeros.Left(5-((CString)buffer).GetLength())+(CString)buffer;
	res.MakeUpper();
	return res.Left(6);
}

CString HexaChar (BYTE nombre)
{
	char buffer[16];
	_itoa_s(nombre, buffer, 16);
	CString Zeros = "00";
	CString res = (CString)"#"+Zeros.Left(2-((CString)buffer).GetLength())+(CString)buffer;
	res.MakeUpper();
	return res.Left(3);
}

CString HexaString (CString chaine)
{
   CString Resultat = "";
     
   for (int i=0; i<=Resultat.GetLength()-1; i++)
	   Resultat += (HexaChar(chaine[i])).Right(1);
    return ((CString)"#"+Resultat);
}

CString ReverseHexa (CString chaine)
{
	CString Resultat = "#";
	for (int i=0; i<=chaine.GetLength()-2; i++)
		Resultat += chaine[chaine.GetLength()-1-i];
    return (Resultat);
}

CString NextLabel (int initialisation)
{
	static int count = 1;
	if (initialisation != -1) count = initialisation;

	return ("LABEL"+IntToStr(count++));
}

CString NextLabel ()
{
	return NextLabel (-1);
}
