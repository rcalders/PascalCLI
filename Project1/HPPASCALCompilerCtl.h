#if !defined(AFX_HPPASCALCOMPILERCTL_H__6DDE27CA_BEDA_11D3_91D8_525400EA8C8F__INCLUDED_)
#define AFX_HPPASCALCOMPILERCTL_H__6DDE27CA_BEDA_11D3_91D8_525400EA8C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
// HPPASCALCompilerCtl.h : Declaration of the CHPPASCALCompilerCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CHPPASCALCompilerCtrl : See HPPASCALCompilerCtl.cpp for implementation.

class CHPPASCALCompilerCtrl 
{

// Constructor
public:
	CHPPASCALCompilerCtrl();

	void CompileAux ();
	void SyntaxeAux ();

	void Update ();
	void EtapeMessage (CString mess);
	void EtapeFichier (CString mess);
	void EtapeJauge (int i);
	
	CString directoryDestination();
	CString directorySource();
	CString directoryResource();
	CString directoryUnit();
	
	CString fileSource();
	CString fileDestination();
	
	int memoryBITMAP ();
	int memorySTACK ();
	int memoryRPL ();
	int memoryGlobal;

	CMachine calculatrice;

// Implementation
public:
	~CHPPASCALCompilerCtrl();
	CString m_erreurFichier;
	afx_msg void OnErreurFichierChanged();
	BOOL m_erreur;
	afx_msg void OnErreurChanged();
	long m_erreurAbscisse;
	afx_msg void OnErreurAbscisseChanged();
	long m_erreurOrdonnee;
	afx_msg void OnErreurOrdonneeChanged();
	CString m_fileSource;
	afx_msg void OnFileSourceChanged();
	CString m_erreurMessage;
	afx_msg void OnErreurMessageChanged();
	CString m_fileDestination;
	afx_msg void OnFileDestinationChanged();
	CString m_directorySource;
	afx_msg void OnDirectorySourceChanged();
	CString m_directoryUnit;
	afx_msg void OnDirectoryUnitChanged();
	CString m_directoryResource;
	afx_msg void OnDirectoryResourceChanged();
	CString m_directoryDestination;
	afx_msg void OnDirectoryDestinationChanged();
	long m_memoryBITMAP;
	afx_msg void OnMemoryBITMAPChanged();
	long m_memorySTACK;
	afx_msg void OnMemorySTACKChanged();
	long m_memoryRPL;
	afx_msg void OnMemoryRPLChanged();
	CString m_etapeMessage;
	afx_msg void OnEtapeMessageChanged();
	CString m_etapeFichier;
	afx_msg void OnEtapeFichierChanged();
	long m_etapeJauge;
	afx_msg void OnEtapeJaugeChanged();
	long m_acces;
	afx_msg void OnVersionChanged();
	afx_msg BOOL Compile();
	afx_msg BOOL CheckSyntax();
	

public:
	enum {
	//{{AFX_DISP_ID(CHPPASCALCompilerCtrl)
	dispidErreurFichier = 1L,
	dispidErreur = 2L,
	dispidErreurAbscisse = 3L,
	dispidErreurOrdonnee = 4L,
	dispidFileSource = 5L,
	dispidErreurMessage = 6L,
	dispidFileDestination = 7L,
	dispidDirectorySource = 8L,
	dispidDirectoryUnit = 9L,
	dispidDirectoryResource = 10L,
	dispidDirectoryDestination = 11L,
	dispidMemoryBITMAP = 12L,
	dispidMemorySTACK = 13L,
	dispidMemoryRPL = 14L,
	dispidEtapeMessage = 15L,
	dispidEtapeFichier = 16L,
	dispidEtapeJauge = 17L,
	dispidVersion = 18L,
	dispidCompile = 19L,
	dispidCheckSyntax = 20L,
	eventidEvenement = 1L,
	eventidFini = 2L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HPPASCALCOMPILERCTL_H__6DDE27CA_BEDA_11D3_91D8_525400EA8C8F__INCLUDED)
