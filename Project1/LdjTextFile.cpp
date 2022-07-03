// LdjTextFile.cpp: implementation of the CLdjTextFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LdjTextFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLdjTextFile::CLdjTextFile(CString Directory, CString Fichier)
{
	Nom = Fichier;
	Repertoire = Directory;
	count = 0;
}

CLdjTextFile::~CLdjTextFile()
{

}

//////////////////////////////////////////////////////////////////////

bool CLdjTextFile::eof ()
{
	return (count==Donnees.GetSize());
}

int CLdjTextFile::GetLength ()
{
	return (Donnees.GetSize());
}

//////////////////////////////////////////////////////////////////////

void CLdjTextFile::StartReading()
{	
	count=0;
}

void CLdjTextFile::Open()
{	
	CFile FichierEntree (Repertoire+Nom, CFile::modeRead);

	char buffer[512]; Donnees.RemoveAll();
	
	while (true)
	{
		int size = FichierEntree.Read(buffer,512);
		if (size==512)
			for (int i = 0; i<512; i++) Donnees.Add(buffer[i]);
		else
		{
			for (int i = 0; i<size; i++) Donnees.Add(buffer[i]);
			break;
		}
	}

	FichierEntree.Close();

	count = 0;
}

void CLdjTextFile::Create()
{
	CFile FichierEntree (Repertoire+Nom, CFile::modeCreate|CFile::modeWrite);	
	FichierEntree.Write(Donnees.GetData(), Donnees.GetSize());
	FichierEntree.Close();
}

char CLdjTextFile::Read()
{
	if (count==Donnees.GetSize())
		return 0;
	else
		return (Donnees[count++]);
}

void CLdjTextFile::Write(char c)
{
	if (count==Donnees.GetSize())
	{
		Donnees.Add(c);
		count++;
	}
	else Donnees.SetAt(count++, c);
}

void CLdjTextFile::Write(CString s)
{
	for (int i=0; i<s.GetLength(); i++)
		Write (s[i]);
}

void CLdjTextFile::Seek (int depl)
{
	count += depl;
}