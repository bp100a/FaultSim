// FaultSim.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FaultSim.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "FaultSimDoc.h"
#include "FaultSimView.h"
#include "listenwindow.h"
#include "ConfigurationDialog.h"

#include "FaultSimMessages.h" // our user messages

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFaultSimApp

BEGIN_MESSAGE_MAP(CFaultSimApp, CWinApp)
	//{{AFX_MSG_MAP(CFaultSimApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_COMMAND(ID_SAVE_ALL, OnSaveAll)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_CLEAR_LOG, OnClearLog)
	ON_COMMAND(ID_SAVE_LOG, OnSaveLog)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaultSimApp construction

CFaultSimApp::CFaultSimApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFaultSimApp object

CFaultSimApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFaultSimApp initialization

BOOL CFaultSimApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_FAULTSTYPE,
		RUNTIME_CLASS(CFaultSimDoc),
		RUNTIME_CLASS(CListenWindow /* CChildFrame*/), // custom MDI child frame
		RUNTIME_CLASS(CFaultSimView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFaultSimApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFaultSimApp commands

BOOL CFaultSimApp::PreTranslateMessage(MSG* pMsg) 
{
  switch(pMsg->message)
  {
    case WM_VALID_PORT:
    {
      int iTargetDocId = (int)pMsg->wParam;
      UINT uiPort      = (UINT)pMsg->lParam;
      CFaultSimDoc* pDoc = GetDocument(iTargetDocId);
      if (NULL != pDoc)
        pDoc->ValidatePort(uiPort); 

      return TRUE;
    }

    case WM_INVALID_PORT:
    {
      int iTargetDocId = (int)pMsg->wParam;
      UINT uiPort      = (UINT)pMsg->lParam;
      CFaultSimDoc* pDoc = GetDocument(iTargetDocId);
      if (NULL != pDoc)
        pDoc->InvalidatePort(uiPort); 

      return TRUE;
    }

    case WM_DISPLAY_TEXT:
    {
      int iTargetDocId = (int)HIWORD(pMsg->wParam);
      messageType mtMessage = (messageType)LOWORD(pMsg->wParam);
      LPSTR lpMessage  = (LPSTR)pMsg->lParam;
      CFaultSimDoc* pDoc = GetDocument(iTargetDocId);
      if (NULL != pDoc)
      {
        // track longest line for window scaling
        size_t stLength = strlen(lpMessage);
        if (stLength > pDoc->GetLongestLine() )
          pDoc->SetLongestLine(stLength);

        pDoc->SetMessage(lpMessage, mtMessage);
        pDoc->UpdateAllViews(NULL);
        return TRUE;
      }
      delete []lpMessage;
      return TRUE;
    }

    default:
      break;
  }

	return CWinApp::PreTranslateMessage(pMsg);
}


void CFaultSimApp::OnConfigure() 
{
  CFaultSimDoc* pDoc = GetActiveDoc();
  if (NULL == pDoc)
  {
    AfxMessageBox("Can't get document");
    return;
  }

  CConfigurationDialog cfgDialog;
  int iPortValue = pDoc->GetListeningPort();
  size_t stNumFaults = pDoc->GetNumFaults();
  const size_t* pstThreadSelections = pDoc->GetFaults();
  size_t* pstSelections = NULL;
  if (pstThreadSelections != NULL)
  {
    pstSelections = new size_t[stNumFaults];
    memcpy(pstSelections, pstThreadSelections, sizeof(size_t) * stNumFaults);
  }

  cfgDialog.SetNumSelected(stNumFaults);
  cfgDialog.SetSelections(pstSelections);
  cfgDialog.SetPortValue(iPortValue);

  int iStatus = cfgDialog.DoModal();
  if (IDOK == iStatus)
  {
    stNumFaults    = cfgDialog.GetNumSelected();
    pstSelections  = cfgDialog.GetSelections();

    // Update the documents listening thread...
    pDoc->SetFaults(stNumFaults, pstSelections);

    // Send configuration to the listening thread....
    DWORD dwThreadId = pDoc->GetListeningThreadId();
    BOOL bStatus = ::PostThreadMessage(dwThreadId, WM_CONFIGURE_THREAD, (WPARAM)stNumFaults, (LPARAM)pstSelections);
    if (!bStatus) // didn't get sent!
      delete [] pstSelections;
    
    pDoc->SetTitle(pDoc->GetTitle());
  }
}

CFaultSimDoc* CFaultSimApp::GetDocument(int iTargetDocId)
{
  for (POSITION pos = GetFirstDocTemplatePosition(); pos; NULL)
  {
    CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
    for (POSITION docPos = pDocTemplate->GetFirstDocPosition(); docPos; NULL)
    {
      CFaultSimDoc* pDoc = (CFaultSimDoc *)GetNextDocTemplate(docPos);
      int iDocId = pDoc->GetDocId();
      if (iDocId == iTargetDocId)
        return pDoc;
    }
  }

  return NULL;
}

// purpose: This routine will save all proxy document windows
//          to a single file
//
void CFaultSimApp::OnSaveAll() 
{
  CString newName = "faultsim.all";
  BOOL bReplace = TRUE;
	if (!DoPromptFileName(newName,
		  IDS_SAVEALL,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, NULL))
			return;       // don't even attempt to save

  for (POSITION pos = GetFirstDocTemplatePosition(); pos; NULL)
  {
    CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
    for (POSITION docPos = pDocTemplate->GetFirstDocPosition(); docPos; NULL)
    {
      CFaultSimDoc* pDoc = (CFaultSimDoc *)GetNextDocTemplate(docPos);
      if (NULL == pDoc)
        break;

      // Okay we have a document, now save it...
      int iDocId = pDoc->GetDocId();
      char szDocId[32];
      sprintf(szDocId, ".doc.%u", iDocId);
      CString docSaveName = newName + szDocId;
      pDoc->OnSaveDocument(docSaveName);
    }
  }

}

void CFaultSimApp::OnFileOpen() 
{
  // TODO : Figure out if this is a single or multiple-document save
 	CString newName;
	if (!DoPromptFileName(newName, AFX_IDS_OPENFILE,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, NULL))
		return; // open cancelled

  CWinApp::OpenDocumentFile(newName);
}

void CFaultSimApp::OnClearLog() 
{
  CFaultSimDoc* pDoc = GetActiveDoc();
  if (NULL == pDoc)
    return;
  pDoc->ClearLog();
}

CFaultSimDoc* CFaultSimApp::GetActiveDoc()
{
  CMainFrame *pMain = (CMainFrame *)m_pMainWnd;
  if (NULL != pMain)
  {
    CListenWindow* pChild = (CListenWindow *)pMain->MDIGetActive();
    if (NULL == pChild)
    {
      AfxMessageBox("Can't find active FaultSim window!");
      return NULL;
    }

    CView* pView = pChild->GetActiveView();
    if (NULL == pView)
    {
      AfxMessageBox("No Active view");
      return NULL;
    }

    CFaultSimDoc* pDoc = (CFaultSimDoc *)pView->GetDocument();
    if (NULL == pDoc)
    {
      AfxMessageBox("Can't get document");
      return NULL;
    }
    return pDoc;
  }

  return NULL;
}

void CFaultSimApp::OnSaveLog() 
{
  CFaultSimDoc* pDoc = GetActiveDoc();
  if (NULL == pDoc)
    return;

  CString logName = "faultsim.log";
  BOOL bReplace = TRUE;
	if (!DoPromptFileName(logName,
		  IDS_SAVELOG,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, NULL))
			return;       // don't even attempt to save

  pDoc->SaveLog(logName);
}
