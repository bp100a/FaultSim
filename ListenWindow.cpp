// ListenWindow.cpp : implementation file
//

#include "stdafx.h"
#include "FaultSim.h"
#include "ListenWindow.h"
#include "FaultSimDoc.h"
#include "mainfrm.h"
#include "FaultSimMessages.h" // get WM_CONFIGURE_THREAD

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListenWindow

IMPLEMENT_DYNCREATE(CListenWindow, CMDIChildWnd)

CListenWindow::CListenWindow()
{
}

CListenWindow::~CListenWindow()
{
}


BEGIN_MESSAGE_MAP(CListenWindow, CMDIChildWnd)
	//{{AFX_MSG_MAP(CListenWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListenWindow message handlers

BOOL CListenWindow::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
  BOOL bStatus = CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);

  return bStatus;
}

