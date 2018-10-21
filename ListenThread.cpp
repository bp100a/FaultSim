// ListenThread.cpp : implementation file
//

#include "stdafx.h"
#include "FaultSim.h"
#include "ListenThread.h"
#include "workerthread.h"     // the worker thread list
#include "FaultSimDoc.h"      // so we can send our messages back

#define _afxSockThreadState AfxGetModuleThreadState()
#define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListenThread

IMPLEMENT_DYNCREATE(CListenThread, CWinThread)

CListenThread::CListenThread()
{
  m_hSocket        = INVALID_SOCKET;
  m_wListeningPort = 81; // default listening port
  m_pParent        = NULL;
  m_pstFaults      = NULL;
  m_stNumFaults    = 0;
  m_iParentDocID   = 0;
  m_pWnd           = NULL; // our socket notification sink

  InitializeCriticalSection(&m_lockFaults);
}

CListenThread::~CListenThread()
{
  if (m_hSocket != INVALID_SOCKET)
    closesocket(m_hSocket);
}

BOOL CListenThread::InitInstance()
{
  m_hSocket = socket(PF_INET, SOCK_STREAM, 0); 
  if (INVALID_SOCKET == m_hSocket)
    return FALSE; // failed

  // Create a target for the Socket Messages, a CWnd
  // that calls back to us. All we need is a CWnd since
  // we are managing ::PreTranslateMessage() for the thread
  // and will get all messages *before* they're dispatched
  // to a window

  CWnd* m_pWnd = new CWnd();
  ASSERT(m_pWnd != NULL);
  if (NULL == m_pWnd)
    return FALSE;

  m_pWnd->m_hWnd = NULL;
	if (!m_pWnd->CreateEx(0, AfxRegisterWndClass(0),
				_T("Listen Thread Socket Sink"),
				WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL))
	{
	}
	ASSERT(m_pWnd->m_hWnd != NULL);
	ASSERT(CWnd::FromHandlePermanent(m_pWnd->m_hWnd) == m_pWnd);
 	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
	pState->m_hSocketWindow = m_pWnd->m_hWnd;


  // Now setup the socket for listening...
  BOOL bStatus = SetupSocket();

  int iNumThreads = 10;
  for (int i = 0; i < iNumThreads; i++)
  {
    // Now try to make our simulated client
    CWorkerThread* pThread = (CWorkerThread *)AfxBeginThread(RUNTIME_CLASS(CWorkerThread),
                                                             THREAD_PRIORITY_NORMAL,
                                                             0,
                                                             CREATE_SUSPENDED);
    ASSERT(pThread != NULL);
    if (NULL == pThread)
      break;

    m_FreeThreadList.AddTail(pThread->m_nThreadID);
    pThread->SetFaultLock(&m_lockFaults);
    pThread->SetParent(this);
    pThread->ResumeThread();
  }

#define TIMER_ELAPSED 2
  UINT uiTimerId   = SetTimer(NULL, 0, TIMER_ELAPSED, NULL);

  BOOL bAllIsWell = (iNumThreads == m_FreeThreadList.GetCount() );
  return bAllIsWell;
}

int CListenThread::ExitInstance()
{
  delete m_pWnd;

  while (!m_FreeThreadList.IsEmpty() )
  {
    DWORD dwWorkerThreadId = m_FreeThreadList.RemoveHead();
    ::PostThreadMessage(dwWorkerThreadId, WM_QUIT, (WPARAM)0, (LPARAM)0);
  }

  return CWinThread::ExitInstance();
}

BOOL CListenThread::SetupSocket(BOOL bCloseSocket /* = FALSE */)
{
  if (bCloseSocket)
  {
    closesocket(m_hSocket);
    m_hSocket = socket(PF_INET, SOCK_STREAM, 0); 
    if (INVALID_SOCKET == m_hSocket)
      return FALSE; // failed
  }

  // Bind the socket
	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
  sockAddr.sin_addr.s_addr = m_iaIPaddress.s_addr;
	sockAddr.sin_port = htons((u_short)m_wListeningPort);

  int iStatus = bind(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
  if (SOCKET_ERROR == iStatus)
  {
    int iLastError = WSAGetLastError();
    BOOL bStatus = ::PostThreadMessage(m_pParent->m_nThreadID, WM_INVALID_PORT, (WPARAM)m_iParentDocID, (LPARAM)m_wListeningPort);
    char* lpMessage = new char[132];
    if (lpMessage != NULL)
    {
      sprintf(lpMessage, "%%Error%% - error binding to port %u ('%s', errcode = %u)", m_wListeningPort, SOCKETERROR_STRING(iLastError), iLastError); 
      if (!OutputMessage(lpMessage, ERROR_MESSAGE) )
        delete []lpMessage;
    }

    return FALSE;
  }
  BOOL bStatus = ::PostThreadMessage(m_pParent->m_nThreadID, WM_VALID_PORT, (WPARAM)m_iParentDocID, (LPARAM)m_wListeningPort);

  // Now direct socket messages to us
  // m_pMainWnd is our main window (a CWnd)

  _AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
  long lEvent = FD_ACCEPT | FD_CLOSE | FD_CONNECT;
  iStatus = WSAAsyncSelect(m_hSocket, pState->m_hSocketWindow, WM_SOCKET_NOTIFY, lEvent);
  if (SOCKET_ERROR == iStatus)
  {
    return FALSE;
  }

  // Now listen for connections
  iStatus = listen(m_hSocket, 50); // only queue up 50 pending sockets
  if (SOCKET_ERROR == iStatus)
  {
    return FALSE;
  }

  char* lpMessage = new char[64];
  if (lpMessage != NULL)
  {
    sprintf(lpMessage, "%%Info%% - listening on port %u", m_wListeningPort); 
    if (!OutputMessage(lpMessage) )
      delete []lpMessage;
  }

  return TRUE;
}

BEGIN_MESSAGE_MAP(CListenThread, CWinThread)
	//{{AFX_MSG_MAP(CListenThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListenThread message handlers

void CListenThread::OnAccept(int nErrorCode)
{
  ASSERT(INVALID_SOCKET != m_hSocket);

  if (INVALID_SOCKET == m_hSocket)
    return;

  SOCKET hNewSocket = accept(m_hSocket, NULL, NULL);
  if (INVALID_SOCKET == hNewSocket)
    return;

  // Need to dispatch this socket handle to a worker
  // thread
  if (!m_FreeThreadList.IsEmpty() )
  {
    DWORD dwWorkerThreadId = m_FreeThreadList.RemoveHead();
    WPARAM wParam = (WPARAM)hNewSocket;
    LPARAM lParam = m_nThreadID; // parent thread
    BOOL bStatus  = ::PostThreadMessage(dwWorkerThreadId, WM_CONNECT_ACCEPT, wParam, lParam);
    if (bStatus)
      return;

    // Send a message to ourselve
    bStatus = ::PostThreadMessage(m_nThreadID, WM_ENQUEUE_WORKERTHREAD, (WPARAM)0, (LPARAM)dwWorkerThreadId);
    ASSERT(!bStatus);
  }
  
  TRACE("%%ERROR%% - failed to get a worker thread \n");

  // didn't get a thread or couldn't dispatch to one...
  closesocket(hNewSocket);
}

void CListenThread::OnClose(int nErrorCode)
{
}

void CListenThread::OnConnect(int nErrorCode)
{
}

void CListenThread::OnTimer(MSG* pMsg)
{
  ASSERT(pMsg != NULL);
  static long lTimer = 0;
  lTimer++;
}

BOOL CListenThread::PreTranslateMessage(MSG* pMsg) 
{
  switch(pMsg->message)
  {
    case WM_QUIT:
    {
      ExitInstance(); // shut down our worker threads
      return TRUE;
    }

    case WM_CONFIGURE_THREAD:
    {
      EnterCriticalSection(&m_lockFaults);
      delete []m_pstFaults;
      m_pstFaults = NULL;
      m_stNumFaults = (size_t)pMsg->wParam;
      size_t* pstSelections = (size_t *)pMsg->lParam;
      if (m_stNumFaults > 0)
      {
        m_pstFaults = new size_t[m_stNumFaults];
        memcpy(m_pstFaults, &pstSelections[1], sizeof(size_t) * m_stNumFaults);
      }

      WORD wPort = m_wListeningPort;
      m_wListeningPort = (WORD)(pstSelections[0] & 0xFFFF);
      LeaveCriticalSection(&m_lockFaults);
      if (m_wListeningPort != wPort) // changed
      {
        // have to rebind and listen to this new port value!
        BOOL bStatus = SetupSocket(TRUE); // close existing socket
        if (!bStatus)
          AfxMessageBox("Problem changing listening socket!!");
      }
      delete []pstSelections;
      return TRUE;
    }

    case WM_DISPLAY_TEXT:
    {
      LPSTR lpMessage = (LPSTR)pMsg->lParam;
      messageType mtMessage = (messageType)LOWORD(pMsg->wParam);
      BOOL bStatus = OutputMessage(lpMessage, mtMessage);
      if (!bStatus)
        delete []lpMessage;
      return TRUE;
    }

    case WM_ENQUEUE_WORKERTHREAD:
    // Worker threads wanting to be queued up
    {
      DWORD dwThreadId = pMsg->lParam;
      m_FreeThreadList.AddHead(dwThreadId);
      return TRUE;
    }

    case WM_SOCKET_DEAD:
	  {
      WPARAM wParam = pMsg->wParam;
      LPARAM lParam = pMsg->lParam;
	    if (wParam == 0 && lParam == 0)
		    return TRUE;

      SOCKET hSocket = (SOCKET)wParam;
      if (INVALID_SOCKET == hSocket)
        return TRUE;
      if (hSocket != m_hSocket)
        return TRUE;

	    closesocket(m_hSocket);
      m_hSocket = INVALID_SOCKET;
	    break;
	  }

    case WM_SOCKET_NOTIFY:
    {
      WPARAM wParam = pMsg->wParam;
      LPARAM lParam = pMsg->lParam;

	    if (wParam == 0 && lParam == 0)
		    return TRUE;

      //
      // If this message is for a socket other
      // than the current socket, ignore it!
      //
      SOCKET hSocket = (SOCKET)wParam;
      if (INVALID_SOCKET == hSocket)
	      return TRUE;

      if (hSocket != m_hSocket)
        return TRUE;

      int nErrorCode = WSAGETSELECTERROR(lParam);
      switch (WSAGETSELECTEVENT(lParam))
      {
        case FD_READ:
        case FD_WRITE:
        case FD_OOB:
          break;
        case FD_CLOSE:
          OnClose(nErrorCode);
          break;
        case FD_ACCEPT:
          OnAccept(nErrorCode);
        case FD_CONNECT:
          OnConnect(nErrorCode);
          break;
      }
	  break;
    }

    case WM_TIMER:
	    OnTimer(pMsg);
	    return TRUE;

    default:
      break;
  }

  return FALSE;
}

BOOL CListenThread::OnIdle(LONG lCount) 
{
  //
  // by dumbing down ::OnIdle() we'll end up in 
  // ::PumpMessage() which is just a ::GetMessage()
  // loop with calls to PreTranslateMessage(), which
  // we've overridden
  //
  return FALSE;
}

size_t CListenThread::GetNumFaults()
{
  EnterCriticalSection(&m_lockFaults);
  size_t stNumFaults = m_stNumFaults;
  LeaveCriticalSection(&m_lockFaults);
  return stNumFaults;
}

size_t* CListenThread::GetFaults()
{
  EnterCriticalSection(&m_lockFaults);
  size_t* pstFaults = m_pstFaults;
  LeaveCriticalSection(&m_lockFaults);
  return pstFaults;
}

BOOL CListenThread::OutputMessage(LPSTR lpMessage, messageType mtMessage)
{
  if (NULL == m_pParent)
    return FALSE;

  WPARAM wParam = MAKEWPARAM((WORD)mtMessage, (WORD)m_iParentDocID);
  BOOL bStatus = ::PostThreadMessage(m_pParent->m_nThreadID, WM_DISPLAY_TEXT, wParam, (LPARAM)lpMessage);
  return bStatus;
}


