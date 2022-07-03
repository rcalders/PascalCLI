// Compiler.h: interface for the CCompiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPILER_H__44E622C0_5176_11D3_91D7_B73CA87C7A2D__INCLUDED_)
#define AFX_COMPILER_H__44E622C0_5176_11D3_91D7_B73CA87C7A2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Syntaxe.h"

class CCompiler
{

public:

	// constructeur/destructeur
	CCompiler(CHPPASCALCompilerCtrl *dialogue);
	virtual ~CCompiler();

	// Méthodes
	void DoCompile();
	void DoCheckSyntaxe();

	// Gestion des erreurs
	CSyntaxe::TErreur Erreur;

	// Gestion du fichier de sortie
	CLdjTextFile *generatedFile;

	// Gestion de la lecture du fichier source
	CSyntaxe* FichierEntree;

	// Mémoire utilisée par les variables globales
	int GlobalMemory;

	// Gestion des types

	struct TType
	{
		CString Nom; // Le type
		bool Indirect; // Est-ce indirect (pointeur)
		bool Tableau; // Est-ce un tableau ?
		int Dimension; // Sa dimension
		int Debut[4], Fin[4];

		int Taille ();
		int NbElts ();
	};

	// Gestion des constantes non typées

	struct TCste
	{
		CString nom;
		struct TCsteValue { TLexemeTypes Type; CString Valeur; } valeur;
	};


	// Gestion des variables

	struct TVariable
	{
		CString Nom; // Nom de la variable
		int Adresse; // Son adresse
		TType Type; // Son Type	
	};

private:

	// boite de dialogue dans laquelle on affiche la progression
	CHPPASCALCompilerCtrl *dlg;
	CString statusCompileSyntaxe; // contient "compilation" ou "vérification syntaxique"

	// lecture des lexemes

	POSITION IndexSource;
	
	CSyntaxe::TLexeme lexeme;
	void LireLexeme ();
	bool IsKeyWord (CString mot);

	// gestion des cstes non typées

	CList <TCste, TCste&> Constantes;

	// gestion des variables

	CList <TVariable, TVariable&> VariablesGlobales;
	CList <TVariable, TVariable&> VariablesLocales;
	CList <TVariable, TVariable&> VariablesStatiques;

	CList <CString, CString&> NomVariablesConstantes;

	POSITION localStatique, localCste;

	TVariable newVar(CString nom, TType type, CList <TVariable, TVariable&> *liste);
	bool IsVariable (CString nom, CList <TVariable, TVariable&> *liste, POSITION posStart);
	bool IsVariable (CString nom);
	bool IsProcFun (CString nom);
	
	CString VariableContext (CString nom);
	CCompiler::TVariable VariableNamed (CString nom);
	
	CString AccesVariable ();
	
	// Gestion des USES

	CList <CString, CString&> ListeUses;

	// Gestion des procédures et fonctions

	CList <TVariable, TVariable&> VariablesParametres; // Variables passées en paramètre

	struct TProcFun
	{
		CString Nom; // Nom de la procédure ou fonction
		int version;
		POSITION PremiereVariable; // Pointeur sur la premiere variable parametre
		POSITION DerniereVariable; // Pointeur sur la derniere variable parametre
		TType Resultat; // Type du résultat (fonction)
	};

	CList <TProcFun, TProcFun&> ProceduresEtFonctions;

	bool CCompiler::sameFonction (TProcFun f1, TProcFun f2);
	bool CCompiler::isFonction (TProcFun fonc);

	// génération du fichier de sortie

	bool EnableWriting;
	void Send (CString text);

	// Fonctions principales

	TType stringType(char longueur);
	CString MaxType (CString Type1, CString Type2);
	CString LabelBreak, LabelContinue;
	CString ProcName;
	int ProcVersion;

	void Compile_In (CString type);
	void CompileConversion (CString Type1, CString Type2);
	CString LireDonnee ();
	CString Eval (int priorite);
	CString CompileAppelFonction (bool isFonction = true);
	void CompileAppelProcedure ();
	void CompileInstrs ();
	void CompileInstr ();
	void CompileProcFun ();	
	void CompileVar (CList <TVariable, TVariable&> *ListeVariables, bool Fonc, bool Constante);
	void CompileUses ();

	void Compile ();	
	void CompileUnit ();
	void CompileRessource ();
};

#endif // !defined(AFX_COMPILER_H__44E622C0_5176_11D3_91D7_B73CA87C7A2D__INCLUDED_)
