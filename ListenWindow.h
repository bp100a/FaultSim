// ListenWindow.h : header file
//

#include "listenthread.h"

class CMainFrame;

/////////////////////////////////////////////////////////////////////////////
// CListenWindow frame

class CListenWindow : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CListenWindow)
protected:
	CListenWindow();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

  // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListenWindow)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CListenWindow();

	// Generated message map functions
	//{{AFX_MSG(CListenWindow)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
