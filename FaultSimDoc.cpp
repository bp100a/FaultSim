// FaultSimDoc.cpp : implementation of the CFaultSimDoc class
//

#include "stdafx.h"
#include "FaultSim.h"

#include "FaultSimDoc.h"
#include "ListenThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFaultSimDoc

IMPLEMENT_DYNCREATE(CFaultSimDoc, CDocument)

BEGIN_MESSAGE_MAP(CFaultSimDoc, CDocument)
	//{{AFX_MSG_MAP(CFaultSimDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaultSimDoc construction/destruction

int CFaultSimDoc::m_iDocCount = 0;
BOOL CFaultSimDoc::m_bIPaddressInitialized = FALSE;
IN_ADDR CFaultSimDoc::m_iaListenAdapter = {0};

CFaultSimDoc::CFaultSimDoc()
{
	// TODO: add one-time construction code here
  m_iDocCount++;
  m_iDocID = m_iDocCount; // starts at 1
  m_pstFaults = new size_t[1];
  if (NULL != m_pstFaults)
    m_pstFaults[0] = 81; // our default port
  m_stNumFaults = 0;

  m_bValidPort = TRUE;
  m_pListenThread = NULL;
}

CFaultSimDoc::~CFaultSimDoc()
{
  delete []m_pstFaults;
}

/////////////////////////////////////////////////////////////////////////////
// CFaultSimDoc serialization

void CFaultSimDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
    ar << m_stNumFaults;
    for (size_t i = 0; i < (m_stNumFaults + 1); i++)
    {
      if (NULL == m_pstFaults)
        ar << (size_t)0xFFFF;
      else
      ar << m_pstFaults[i];
    }
  }
	else
	{
    ar >> m_stNumFaults;
    m_pstFaults = new size_t[m_stNumFaults + 1];
    if (m_pstFaults != NULL)
    {
      for (size_t i = 0; i < (m_stNumFaults + 1); i++)
        ar >> m_pstFaults[i];
    }

    InitializeListeningThread();
  }
}

/////////////////////////////////////////////////////////////////////////////
// CFaultSimDoc diagnostics

#ifdef _DEBUG
void CFaultSimDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFaultSimDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFaultSimDoc commands

void CFaultSimDoc::DeleteContents() 
{
  m_lstStrings.RemoveAll();
  m_stLongestLine = 32; // default
	CDocument::DeleteContents();
}


BOOL CFaultSimDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//
// routine: SetFaults():
// purpose: Please note that the first entry in the 'fault' list 
//          is the listening port value. Therefore the # faults is
//          1 less than the size of the array.
//
void CFaultSimDoc::SetFaults(size_t stNumFaults, const size_t* pstFaults)
{
  delete []m_pstFaults;
  m_pstFaults = NULL;
  m_stNumFaults = stNumFaults;

  m_pstFaults = new size_t[m_stNumFaults + 1];
  if (NULL == pstFaults)
  {
    ASSERT(FALSE);
    m_stNumFaults = 0;
    return;
  }
  memcpy(m_pstFaults, pstFaults, sizeof(size_t) * (m_stNumFaults + 1));
}

const size_t * CFaultSimDoc::GetFaults()
{
  if (m_stNumFaults <1)
    return NULL;

  return &m_pstFaults[1];
}

UINT CFaultSimDoc::GetListeningPort()
{
  if (NULL == m_pstFaults)
    return 81;

  return m_pstFaults[0];
}


void CFaultSimDoc::SetTitle(LPCTSTR lpszTitle) 
{
  CString strTitle = lpszTitle;

  int iPos = strTitle.Find(":");
  if (iPos != -1)
    strTitle = strTitle.Left(iPos);

  char szPort[32];
  sprintf(szPort, " listen port %u", GetListeningPort() );
  CString strTitlePort = strTitle + ":" + szPort;
  if (!m_bValidPort)
    strTitlePort += "[conflict]";

  CDocument::SetTitle(strTitlePort);
}

BOOL CFaultSimDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace/* = TRUE*/)
{
  CString strTitle = GetTitle();
  int iPos = strTitle.Find(":");
  CString strTempTitle = strTitle;
  if (iPos != -1)
    strTempTitle = strTempTitle.Left(iPos);

  CDocument::SetTitle(strTempTitle);
  BOOL bStatus = CDocument::DoSave(lpszPathName, bReplace);
  SetTitle(strTempTitle);
  return bStatus;
}

int CFaultSimDoc::GetLineHeight(int Height)
{
  int HeightPerLine = (int)( (float)Height * 1.2);
  if (0 == HeightPerLine)
    HeightPerLine = 20; // default!
  return HeightPerLine;
}

int CFaultSimDoc::GetLineHeight(CView* pView)
{
  CDC* pDC = pView->GetDC();
  int HeightPerLine = GetLineHeight(0);
  if (pDC != NULL)
  {
    TEXTMETRIC txtMetrics; 
    BOOL bStatus = pDC->GetTextMetrics(&txtMetrics);
    if (bStatus)
      HeightPerLine = GetLineHeight(txtMetrics.tmHeight);
  
    pView->ReleaseDC(pDC);
    pDC = NULL;
  }

  return HeightPerLine;
}

int CFaultSimDoc::GetLineWidth(CView* pView)
{
  CDC* pDC = pView->GetDC();
  int WidthPerCharacter;
  if (pDC != NULL)
  {
    TEXTMETRIC txtMetrics; 
    BOOL bStatus = pDC->GetTextMetrics(&txtMetrics);
    if (bStatus)
      WidthPerCharacter = txtMetrics.tmAveCharWidth;
  
    pView->ReleaseDC(pDC);
    pDC = NULL;
  }

  int WidthLongestLine = WidthPerCharacter * m_stLongestLine;
  return WidthLongestLine;
}

CSize CFaultSimDoc::GetDocSize(CView* pView)
{
  int NumLines = m_lstStrings.GetCount();
  int HeightPerLine = GetLineHeight(pView);
  int MaxLineWidth  = GetLineWidth(pView);

  NumLines++;

  //
  // Okay, calculate how big the "virtual" region should be
  // to support this many lines
  //
  int Height = NumLines * HeightPerLine;
  CSize sizeDocument(MaxLineWidth, Height);

  return sizeDocument;
}

void CFaultSimDoc::InvalidatePort(UINT uiPort)
{
   UINT uiListeningPort =GetListeningPort();
   if (uiPort == uiListeningPort)
   {
     m_bValidPort = FALSE;
     CString strTitle = GetTitle();
     SetTitle(strTitle);
   }
}

void CFaultSimDoc::ValidatePort(UINT uiPort)
{
   UINT uiListeningPort =GetListeningPort();
   if (uiPort == uiListeningPort)
   {
     m_bValidPort = TRUE;
     CString strTitle = GetTitle();
     SetTitle(strTitle);
   }
}

BOOL CFaultSimDoc::OnNewDocument() 
{
	// TODO: Add your specialized code here and/or call the base class

  InitializeListeningThread();
	return CDocument::OnNewDocument();
}

BOOL CFaultSimDoc::InitializeListeningThread()
{
  // figure out what adapter to use...
  InitializeLocalIPAddresses();
  
  if (NULL == m_pListenThread)
  {
  // Create our listening thread
    m_pListenThread = (CListenThread *)AfxBeginThread(RUNTIME_CLASS(CListenThread),
                                                      THREAD_PRIORITY_NORMAL,
                                                      0,
                                                      CREATE_SUSPENDED);

    if (NULL == m_pListenThread)
    {
      AfxMessageBox("Error creating listening thread!!!");
      return FALSE;
    }
#pragma message("Currently listening to first adapter, port 81")
    m_pListenThread->SetListeningPort(GetListeningPort()); // for now
    m_pListenThread->SetIPaddress(m_iaListenAdapter.s_addr);
    CWinApp* pMyApp = AfxGetApp(); 
    m_pListenThread->SetParent(pMyApp);
    m_pListenThread->SetDocId(m_iDocID);
    m_pListenThread->ResumeThread();
  }

  return TRUE;
}

BOOL CFaultSimDoc::InitializeLocalIPAddresses()
{
  if (m_bIPaddressInitialized)
    return TRUE;

  WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);

	int nResult = WSAStartup(wVersionRequested, &wsaData);
	if (nResult != 0)
	    return FALSE;

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
	    WSACleanup();
    	return FALSE;
	}
  
  if (!AfxSocketInit())
	{
		return FALSE;
	}

  char szHostName[64];
  int iStatus = gethostname(szHostName, sizeof (szHostName));
  if (iStatus != 0)
  {
	 	SetLastError(WSAGetLastError()) ;
		return FALSE;
	}

  hostent* psa = gethostbyname((const char*) szHostName );
	
  int m_iLocalNetCardCount = 0;
	if (psa != NULL)
	{

    for (int i = 0; psa->h_addr_list[i] != NULL; i++)
		{
      in_addr iaAddress;
      
      // to avoid alignment issues
      memcpy(&iaAddress, psa->h_addr_list[i], sizeof(in_addr) );

      m_iaLocalIPAddresses[m_iLocalNetCardCount].s_addr = iaAddress.s_addr;
		  if (m_iLocalNetCardCount < MAX_IPADDRESSES)  // only handle 'MAX_IPADDRESSES' adapters
			  m_iLocalNetCardCount++;
		}
	}
  else
  {
    SetLastError(WSAGetLastError()) ;
    return FALSE;
  }

  // for now initialize our adapter useage to be the firs5t one
  #pragma message("%%INFO%% - Currently listen to first adapter found")
  m_iaListenAdapter.s_addr = m_iaLocalIPAddresses[0].s_addr;

  m_bIPaddressInitialized = TRUE; // we're initialized, avoid doing it twice
	return TRUE;
}

DWORD CFaultSimDoc::GetListeningThreadId()
{
  if (NULL == m_pListenThread)
  {
    AfxMessageBox("%%ERROR%% - don't have listening thread for this proxy!");
    return 0;
  }
  return m_pListenThread->m_nThreadID;
}

void CFaultSimDoc::OnCloseDocument() 
{
  // How about an orderly shutdown of this document's threads?
  
  DWORD dwThreadId = GetListeningThreadId();
  if (0 != dwThreadId)
  {
    PostThreadMessage(dwThreadId, WM_QUIT, (WPARAM)0, (LPARAM)0);
  }
	CDocument::OnCloseDocument();
}

//
// routine: SetMessage():
// purpose: Will set out message string. This will eventually
//          make use of the message type to notify the user
//          via sound & color of events.
//
void CFaultSimDoc::SetMessage(LPSTR lpMessage, messageType mtMessage)
{
  ASSERT(lpMessage != NULL);
  if (NULL == lpMessage)
    return;

  char szType[5];
  sprintf(szType,"%02u", (unsigned int)mtMessage);
  CString strMessage = CString(szType) + lpMessage;
  delete []lpMessage;
  m_lstStrings.AddTail(strMessage);

  switch(mtMessage)
  {
  case ERROR_MESSAGE:
    {
      Beep(100,100);
      break;
    }
  default:
    break;
  }
}

void CFaultSimDoc::ClearLog()
{
   m_lstStrings.RemoveAll();
   UpdateAllViews(NULL);
}

void CFaultSimDoc::SaveLog(CString& strLogName)
{
  CStdioFile logFile;
  CFileException cfe;
  BOOL bOpen = logFile.Open(strLogName, (CFile::modeCreate | CFile::modeWrite), &cfe);
  if (!bOpen)
  {
    return;
  }

  for (POSITION pos = m_lstStrings.GetHeadPosition(); pos; NULL)
  {
    CString String = m_lstStrings.GetNext(pos);
    if (!String.IsEmpty() )
    {
      size_t stLength = String.GetLength();
      if (stLength > 2)
        String = String.Right(stLength - 2);
      logFile.WriteString(String);
    }

    logFile.WriteString("\n");
  }

  logFile.Close();
}
