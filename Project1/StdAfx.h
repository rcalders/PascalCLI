#if !defined(AFX_STDAFX_H__6DDE27C1_BEDA_11D3_91D8_525400EA8C8F__INCLUDED_)
#define AFX_STDAFX_H__6DDE27C1_BEDA_11D3_91D8_525400EA8C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

typedef enum {HP48, HP49} CMachine;
#define _DEBUG
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Comon Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes
////#include <afxdb.h>			// MFC database classes
#include <afxdao.h>			// MFC DAO database classes

#include <afx.h>
#include <afxcmn.h>
#include <afxtempl.h>
#include <afxwin.h>
#include "afxdb.h"

#include "utils.h"
#include "LdjTextFile.h"
#include "HPPASCALCompilerCtl.h"			

//{{AFX_INSERT_LOCATION}}

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_STDAFX_H__6DDE27C1_BEDA_11D3_91D8_525400EA8C8F__INCLUDED_)

