// HPPASCALCompilerCtl.cpp : Implementation of the CHPPASCALCompilerCtrl ActiveX Control class.

#include "stdafx.h"
//#include "HP PASCAL Compiler.h"
#include "HPPASCALCompilerCtl.h"
//#include "HPPASCALCompilerPpg.h"

#include <process.h>

#include "assembler.h"
#include "MacroCompile.h"
#include "Optimizer.h"
#include "Compiler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHPPASCALCompilerCtrl::CHPPASCALCompilerCtrl()
{
}

CHPPASCALCompilerCtrl::~CHPPASCALCompilerCtrl()
{
}



/////////////////////////////////////////////////////////////////////////////
// CHPPASCALCompilerCtrl message handlers

CString CHPPASCALCompilerCtrl::directoryDestination()
{
	return (m_directoryDestination);
}

CString CHPPASCALCompilerCtrl::directorySource()
{
	return (m_directorySource);
}

CString CHPPASCALCompilerCtrl::directoryResource()
{
	return (m_directoryResource);
}

CString CHPPASCALCompilerCtrl::directoryUnit()
{
	return (m_directoryUnit);
}

CString CHPPASCALCompilerCtrl::fileSource()
{
	return (m_fileSource);
}

CString CHPPASCALCompilerCtrl::fileDestination()
{
	return (m_fileDestination);
}

int CHPPASCALCompilerCtrl::memoryBITMAP ()
{
	return (m_memoryBITMAP);
}

int CHPPASCALCompilerCtrl::memorySTACK ()
{
	return (m_memorySTACK);
}

int CHPPASCALCompilerCtrl::memoryRPL ()
{
	return (m_memoryRPL);
}

void CHPPASCALCompilerCtrl::Update ()
{

}

void CHPPASCALCompilerCtrl::EtapeMessage (CString mess)
{
	m_etapeMessage = mess;
}

void CHPPASCALCompilerCtrl::EtapeFichier (CString mess)
{
	m_etapeFichier = mess;
}

void CHPPASCALCompilerCtrl::EtapeJauge (int i)
{
	m_etapeJauge = i;
}

// Le compilateur
// --------------

void CHPPASCALCompilerCtrl::CompileAux ()
{
	// TODO: Add your dispatch handler code here

	// En mode DEBUG, on génère RESULTAT1,2,3,4.TXT
#ifdef _DEBUG
	bool debug = true;	
#endif
	
#ifdef NDEBUG
	bool debug = false;	
#endif

	// Controle d'accès	

	if (m_acces==65536745) calculatrice=HP48;
	if (m_acces==65536746) calculatrice=HP49;

	if ((unsigned long)(m_acces-65536745)<2)
	{
		
		// Compilation
		
		m_erreur = false;
		
		CCompiler *Compilateur;
		Compilateur = new CCompiler (this);
		Compilateur->DoCompile ();
		
		if (debug) Compilateur->FichierEntree->Debug();
		
		if (Compilateur->Erreur.Message!="")
		{
			m_erreur = true;
			m_erreurOrdonnee = Compilateur->Erreur.Ligne;
			m_erreurAbscisse = Compilateur->Erreur.Abscisse;
			m_erreurMessage = Compilateur->Erreur.Message;			
			m_erreurFichier = Compilateur->Erreur.Fichier;
			return;
		}
		
		if (debug) Compilateur->generatedFile->Create();
		
		memoryGlobal = Compilateur->GlobalMemory;
		
		// Optimisation
		
		EtapeMessage ("Optimisation");
		EtapeFichier ("");
		EtapeJauge (0);
		Update();
		
		COptimizer *optimize;
		optimize = new COptimizer(Compilateur->generatedFile, this);
		
		if (debug) optimize->generatedFile->Create();
		
		// Production du code assembleur
		
		EtapeMessage ("Création du code assembleur");
		EtapeFichier ("");
		EtapeJauge (0);
		Update();
		
		CMacroCompile *macrocomp;
		macrocomp = new CMacroCompile (optimize->generatedFile, this);
		
		if (debug) macrocomp->generatedFile->Create();
		
		// Assemblage
		
		EtapeMessage ("Assemblage");
		EtapeFichier ("");
		EtapeJauge (0);
		Update();
		
		CAssembler *assembler;
		assembler = new CAssembler (macrocomp->generatedFile, this);
		
		if (!assembler->Erreur) assembler->StopAssLine();
		
		if (assembler->Erreur)
		{
			m_erreur = true;		
			m_erreurOrdonnee = assembler->ErreurInfos.Ligne;
			m_erreurAbscisse = 1;
			m_erreurMessage = assembler->ErreurInfos.Message;
			m_erreurFichier = assembler->ErreurInfos.Fichier;
		}	
		
		return;
	}

}



// Le vérificateur de syntaxe
// --------------------------

void CHPPASCALCompilerCtrl::SyntaxeAux ()
{
	// TODO: Add your dispatch handler code here

	// En mode DEBUG, on génère RESULTAT1,2,3,4.TXT
#ifdef _DEBUG
	bool debug = true;	
#endif
	
#ifdef NDEBUG
	bool debug = false;	
#endif

	// Controle d'accès	

	if (m_acces==65536745)
	{
		
		// Compilation
		
		m_erreur = false;
		
		CCompiler *Compilateur;
		Compilateur = new CCompiler (this);
		Compilateur->DoCheckSyntaxe ();
		
		if (debug) Compilateur->FichierEntree->Debug();
		
		if (Compilateur->Erreur.Message!="")
		{
			m_erreur = true;
			m_erreurOrdonnee = Compilateur->Erreur.Ligne;
			m_erreurAbscisse = Compilateur->Erreur.Abscisse;
			m_erreurMessage = Compilateur->Erreur.Message;			
			m_erreurFichier = Compilateur->Erreur.Fichier;
			return;
		}
		
		if (debug) Compilateur->generatedFile->Create();

		return;
	}

}



void CHPPASCALCompilerCtrl::OnErreurFichierChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnErreurChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnErreurAbscisseChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnErreurOrdonneeChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnFileSourceChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnErreurMessageChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnFileDestinationChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnDirectorySourceChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnDirectoryUnitChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnDirectoryResourceChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnDirectoryDestinationChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnMemoryBITMAPChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnMemorySTACKChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnMemoryRPLChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnEtapeMessageChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnEtapeFichierChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnEtapeJaugeChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void CHPPASCALCompilerCtrl::OnVersionChanged() 
{
	// TODO: Add notification handler code

//	SetModifiedFlag();
}

void daube (void *obj)
{
	CHPPASCALCompilerCtrl* objet;
	objet = (CHPPASCALCompilerCtrl*)obj;

	objet->CompileAux();
}

void daube2 (void *obj)
{
	CHPPASCALCompilerCtrl* objet;
	objet = (CHPPASCALCompilerCtrl*)obj;

	objet->SyntaxeAux();
}

BOOL CHPPASCALCompilerCtrl::Compile() 
{
	_beginthread (daube, 0, this);
	return true;
}

BOOL CHPPASCALCompilerCtrl::CheckSyntax() 
{
	// TODO: Add your dispatch handler code here
	_beginthread (daube2, 0, this);
	return true;
}
