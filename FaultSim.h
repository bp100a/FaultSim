// FaultSim.h : main header file for the FAULTSIM application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CFaultSimDoc;

/////////////////////////////////////////////////////////////////////////////
// CFaultSimApp:
// See FaultSim.cpp for the implementation of this class
//

class CFaultSimApp : public CWinApp
{
public:
	CFaultSimApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFaultSimApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFaultSimApp)
	afx_msg void OnAppAbout();
	afx_msg void OnConfigure();
	afx_msg void OnSaveAll();
	afx_msg void OnFileOpen();
	afx_msg void OnClearLog();
	afx_msg void OnSaveLog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  protected:
    CFaultSimDoc* GetDocument(int iDocId);
    CFaultSimDoc* GetActiveDoc();


};


/////////////////////////////////////////////////////////////////////////////
