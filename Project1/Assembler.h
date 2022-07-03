// Assembler.h: interface for the CAssembler clAss.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AssEMBLER_H__E39F95E0_54A5_11D3_91D7_DF10DC6CE92B__INCLUDED_)
#define AFX_AssEMBLER_H__E39F95E0_54A5_11D3_91D7_DF10DC6CE92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAssembler  
{
private:

	// Constantes

	char Version48, Version49;

	CString ST3 [3];
	CString COD3 [8];
	CString ST4 [9];
	CString ST4bis;
	CString ST4ter;
	CString COD4 [10];
	CString ST5 [12];	    
	CString ST5bis;	     
	CString ST6 [8];
	CString ST6bis;
	CString ST56;
	CString ST7[2];
	CString LSTR;
	CString CHAMP [9];
	CString JP1 [10];
	CString JP2 [10];

	// Structure gérant les LABELs

	struct TVariable
	{
		CString name;
		char typ;
		WORD cont, adr1, adr2;
	};
	
	CList <TVariable, TVariable&> labels;
	TVariable ptr;

	// Stockage du code compilé
	// ------------------------

	char tab [128000];

	// Variables de compilation
	// ------------------------

	bool ok,test,LineNul;
	char choix;

	WORD numline, count;

	CString line, linebis;    
	CString str1, str2, result;
	
	// Fonctions auxiliaires
	// ---------------------

	// Fonctions de conversion
	char Valeur (char k);
	char Car (char k);
	void ConvertAscii (CString& st);
	CString Adr (int n);
	void Val (CString chaine, int& resultat, int& erreur);
	int Pos (CString contenu, CString conteneur);

	// Contrôle du format
	bool OkHexa (CString& st);
	bool OkName (CString& st);

	// Contrôle des dupplications
	bool OkEtiq ();	
	bool Duplic (CString &st);
	
	// Calcul des offset	
	CString Offset (int n, int a1, int a2);

	// Lecture du code source	
	CString Item (CString& line);

	// Ajout de code dans le BUFFER
	void Ajouter (CString& st);
	void AjouterBack (int ad, int n, CString st);
	void MergeRessource ();

	// Erreurs
	void DoErreur (int n);

	// Compilateur	
	// -----------

	int Argument();
	char Nibble (CString& st, bool flag);
	void ReplEtiq ();
	void Field (CString stA, CString stB, CString st);
	
	void Div3 (int p);
	void Ass56 ();
	void LoadCst ();
	void LoadD (int p);
	
	void Ass3 ();
	void Ass4 ();
	void Ass5 ();
	void Ass6 ();
	void Ass7 ();

	void Equate ();
	void IfTest ();
	void Saut ();
	void LoadAC ();
	
	void Etiq ();
	void Assembleur (CString& str1, CString& str2);

	void Assembler_Ligne ();

	CHPPASCALCompilerCtrl *dlg;

public:

	// Constructeur / Destructeur
	CAssembler(CLdjTextFile *Fichier, CHPPASCALCompilerCtrl *dialogue);
	virtual ~CAssembler();	

	// Gestion des erreurs
	bool Erreur;

	struct TErreur
	{
		CString Fichier;
		CString Message;
		int Ligne;
	};

	TErreur ErreurInfos;

	// Assemble une ligne
	void AssLine (CString ligne); 

	// Fin de compilation
	void StopAssLine ();

};

#endif // !defined(AFX_AssEMBLER_H__E39F95E0_54A5_11D3_91D7_DF10DC6CE92B__INCLUDED_)
