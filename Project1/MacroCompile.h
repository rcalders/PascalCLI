// MacroCompile.h: interface for the CMacroCompile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MACROCOMPILE_H__90DDDD80_A0F4_11D3_91D8_525400EA8C8F__INCLUDED_)
#define AFX_MACROCOMPILE_H__90DDDD80_A0F4_11D3_91D8_525400EA8C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMacroCompile  
{
	struct TInstruction { CString op, arg1, arg2, arg3, arg4; };
	CList <TInstruction, TInstruction&> ListeInstructions;

public:
	CMacroCompile(CLdjTextFile * Fichier, CHPPASCALCompilerCtrl *dialogue);
	virtual ~CMacroCompile();

	CLdjTextFile *generatedFile;

private:

	CHPPASCALCompilerCtrl *dlg;

	CString DirectorySystem;

	CString ProcName;

	CList <CString, CString&> FonctionsHard;
	void AppelFonctionHard (CString nom);

	void Chargement (CLdjTextFile *Fichier);
	void Traduction ();

	CString buf[10];
	int count;
	void Send (CString chaine);
	void SendNoOptimize(CString chaine);
	void EndSending ();
	bool OptFlag;

	void TraduitLOAD(TInstruction current);
	void TraduitSAVE(TInstruction current);
	void TraduitCV(TInstruction current);
	void TraduitCST(TInstruction current);
	void TraduitOP(TInstruction current);
	void TraduitDUP(TInstruction current);
	void TraduitDROP(TInstruction current);
	void TraduitProc (TInstruction current);
	void TraduitEndProc (TInstruction current);
	void TraduitEND ();
	void TraduitCONST (TInstruction current);
	void TraduitSautConditionnel(TInstruction current);
	void TraduitHARDMULT(TInstruction current);

	void Depile (CString registre, CString datatype);
	void Empile (CString registre, CString datatype);
	void LectureMemoire (CString registre, CString datatype);
	void EcritureMemoire (CString registre, CString datatype);

	CString typCONST; int typCONST2;

	int Size (CString type);
	char Champ (CString type);

};

#endif // !defined(AFX_MACROCOMPILE_H__90DDDD80_A0F4_11D3_91D8_525400EA8C8F__INCLUDED_)
