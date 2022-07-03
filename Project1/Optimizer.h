// Optimizer.h: interface for the COptimizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIMIZER_H__DE756241_A647_11D3_91D8_525400EA8C8F__INCLUDED_)
#define AFX_OPTIMIZER_H__DE756241_A647_11D3_91D8_525400EA8C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COptimizer  
{
	struct TInstruction { CString op, arg1, arg2, arg3, arg4; };
	CList <TInstruction, TInstruction&> ListeInstructions;

public:
	COptimizer(CLdjTextFile *Fichier, CHPPASCALCompilerCtrl *dialogue);
	virtual ~COptimizer();

	CLdjTextFile *generatedFile;

private:

	CHPPASCALCompilerCtrl *dlg;

	void Chargement (CLdjTextFile *Fichier);
	void Traduction ();
	void Sauvegarde ();

	CList <CString, CString&> FonctionsUtilisees;

	int DecalagePile (COptimizer::TInstruction current);
	POSITION Recherche (CString Instruction);
	POSITION Recherche (CString Instruction, CString arg1);
	void Deplace (POSITION SourceStart, POSITION SourceEnd, POSITION Destination);
};

#endif // !defined(AFX_OPTIMIZER_H__DE756241_A647_11D3_91D8_525400EA8C8F__INCLUDED_)
