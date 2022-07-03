// LdjTextFile.h: interface for the CLdjTextFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LDJTEXTFILE_H__07034980_A8D6_11D3_91D8_525400EA8C8F__INCLUDED_)
#define AFX_LDJTEXTFILE_H__07034980_A8D6_11D3_91D8_525400EA8C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLdjTextFile  
{
public:
	CLdjTextFile(CString Repertoire, CString Fichier);
	virtual ~CLdjTextFile();

	// Accès au disque
	void StartReading();
	void Open ();
	void Create ();

	// Accès au buffer
	void Write (char c);
	void Write(CString s);
	char Read ();
	void Seek (int depl);

	// Information
	int GetLength ();
	bool eof ();

private:
	CString Repertoire, Nom;
	CArray <char, char&> Donnees;
	int count;
};

#endif // !defined(AFX_LDJTEXTFILE_H__07034980_A8D6_11D3_91D8_525400EA8C8F__INCLUDED_)
