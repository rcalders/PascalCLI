// Syntaxe.h: interface for the CSyntaxe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNTAXE_H__EB173FC0_50D2_11D3_91D7_9E4F7FBBB228__INCLUDED_)
#define AFX_SYNTAXE_H__EB173FC0_50D2_11D3_91D7_9E4F7FBBB228__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum {KEYWORD, ID, ENTIER, FLOTTANT, CARACTERE, CHAINE, ASSEMBLER, END_OF_FILE} TLexemeTypes;

class CSyntaxe  
{

public:

	// lexemes correspondant au fichier lu

	struct TLexeme
	{
		TLexemeTypes Type;
		CString Valeur;
		int Ligne;
		int Abscisse;
	};

	CList <TLexeme, TLexeme&> lexemes;

	// erreurs rencontrées lors de l'analyse syntaxiques

	struct TErreur
	{
		CString Fichier;
		CString Message;
		int Ligne;
		int Abscisse;
	} Erreur;

	// construction de l'objet associé au fichier "repertoire/fichier"

	CSyntaxe(CString repertoire, CString fichier, CHPPASCALCompilerCtrl *dialogue, CString tryFirst="");
	virtual ~CSyntaxe();

	// débogage : génération d'un fichier de lexemes

	void Debug ();


private:

	// permet d'afficher l'évolution dans la lecture du fichier source
	CHPPASCALCompilerCtrl *dlg;

	// gestion du fichier source
	CLdjTextFile* FichierEntree;
	CList <int, int&> DebutLignes;
	int Abscisse (int pos);
	int Ordonnee (int pos);
	int count;
	int count_eof;
	bool Eof;

	// gestion de la directive ASM
	bool Asm_Flag;
	
	// fichier destination
	TLexeme tmp;

	// fonctions internes
	CString LireMot (CString accumulateur);
	CString LireSymbole (char tampon);

	char LireOctet ();
	void RendreOctet ();

	void LireNombreEntier (CString accumulateur);
	void LireNombreReel (CString accumulateur);
	void LireChaine (CString accumulateur);
	void LireCommentaire ();
	void McOuId (CString mot);
	void McOuErreur (CString mot);
	void LireLexemeAsm ();
	void LireLexeme ();

};

#endif // !defined(AFX_SYNTAXE_H__EB173FC0_50D2_11D3_91D7_9E4F7FBBB228__INCLUDED_)
