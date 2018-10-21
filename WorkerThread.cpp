// WorkerThread.cpp : implementation file
//

#define __WorkerThread_cpp

#include "stdafx.h"
#include "FaultSim.h"
#include "WorkerThread.h"
#include "adm_ids.h" // admin item category ids
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "ListenThread.h" 

#if !defined(__GenerateData_h)
    #include "GenerateData.h"
#endif

#define _afxSockThreadState AfxGetModuleThreadState()
#define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE

/////////////////////////////////////////////////////////////////////////////
// CWorkerThread

IMPLEMENT_DYNCREATE(CWorkerThread, CWinThread)
IMPLEMENT_DYNCREATE(CForwardObject, CObject)

size_t CWorkerThread::m_stTotalFaults = 0;

CWorkerThread::CWorkerThread()
{
  m_hClientSocket    = INVALID_SOCKET;
  m_hServerSocket    = INVALID_SOCKET;
  m_bAutoDelete      = TRUE;
  m_pClientForward   = NULL;
  m_pClientReceiving = NULL;
  m_pServerForward   = NULL;
  m_ttAccepted       = 0; // disables timeout
  m_stNumFaults      = 0;
  m_pLockFaults      = NULL;
  m_pParent          = NULL;
  m_pWnd             = NULL; // target for socket notifications

  m_stFaultToSimulate = 0;

  if (0 == m_stTotalFaults)
  {
	size_t i = 0;
    for (i = 0; glb_lpFaultStrings[i] != NULL; i++) NULL;

    m_stTotalFaults = i;
  }

  m_pFaults = new BOOL[m_stTotalFaults];
  for (size_t i = 0; i < m_stTotalFaults; i++)
    m_pFaults[i] = FALSE; // fault not set
}

CWorkerThread::~CWorkerThread()
{
  if (m_hClientSocket != INVALID_SOCKET)
    closesocket(m_hClientSocket);
  if (m_hServerSocket != INVALID_SOCKET)
    closesocket(m_hServerSocket);

  delete []m_pFaults;
}

BOOL CWorkerThread::InitInstance()
{
  // Create a target for the Socket Messages, a CWnd
  // that calls back to us

  m_pWnd = new CWnd();
  ASSERT(m_pWnd != NULL);
  if (NULL == m_pWnd)
    return FALSE;

  m_pWnd->m_hWnd = NULL;
	if (!m_pWnd->CreateEx(0, AfxRegisterWndClass(0),
				_T("Worker Thread Socket Sink"),
				WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL))
	{
    return FALSE;
	}

	ASSERT(m_pWnd->m_hWnd != NULL);
	ASSERT(CWnd::FromHandlePermanent(m_pWnd->m_hWnd) == m_pWnd);
 	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
	pState->m_hSocketWindow = m_pWnd->m_hWnd;

  // Now allocate our buffers for receiving (then sending)
  // client/server data
  for (int i = 0; i < 5; i++)
  {
    CForwardObject* pObj = new CForwardObject(50000); // large buffers
    m_lstFree.AddTail(pObj);
  }

  
  UINT uiTimerId = SetTimer(pState->m_hSocketWindow, 1, 5000, NULL);
  
  m_pClientReceiving = NULL; // what client is receiving with
  return TRUE;
}

int CWorkerThread::ExitInstance()
{
  delete m_pWnd;
  while(!m_lstFree.IsEmpty() )
  {
    CForwardObject* pObj = (CForwardObject *)m_lstFree.RemoveHead();
    delete pObj;
  }

  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWorkerThread, CWinThread)
	//{{AFX_MSG_MAP(CWorkerThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkerThread message handlers

void CWorkerThread::AcceptSocket(MSG* pMsg)
{
  ASSERT(INVALID_SOCKET == m_hClientSocket);
  SOCKET hSocket = pMsg->wParam;
  m_dwParentThreadId = pMsg->lParam;
  TRACE2("[CLIENT:%Xh]FD_ACCEPT socket %u\n", m_nThreadID, hSocket);

  if (INVALID_SOCKET == hSocket)
    return;

  m_hClientSocket = hSocket;
 	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
  long lEvent = FD_READ | FD_CLOSE;
  int iStatus = WSAAsyncSelect(m_hClientSocket, pState->m_hSocketWindow, WM_SOCKET_NOTIFY, lEvent);
  if (SOCKET_ERROR == iStatus)
  {
    closesocket(hSocket);
    m_hClientSocket = INVALID_SOCKET;
    ExitThread();
  }

  m_ttAccepted = time(NULL);
  m_ClientState = CWorkerThread::WAIT_INIT;

  struct tm* ptm = localtime(&m_ttAccepted);
  if (ptm != NULL)
  {
    LPSTR lpMessage = new char[255];
    if (NULL != lpMessage)
    {
      sprintf(lpMessage,"Client connected at %u:%u:%02u [Socket:%u]", 
              ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_hClientSocket);
      OutputMessage(lpMessage);
    }
  }

  m_doFaults = TRUE; // enable fault simulation (turned off it appId not fido)

  // Get a copy of the fault configuration
  m_stNumFaults = (m_pParent != NULL ? m_pParent->GetNumFaults() : 0);

  // turn off all faults
  memset(m_pFaults, 0, sizeof(BOOL) * m_stTotalFaults);
 
  if (0 != m_stNumFaults)
  {
    EnterCriticalSection(m_pLockFaults);
    size_t *pstFaults = (m_pParent != NULL ? m_pParent->GetFaults() : NULL);
    for (size_t i = 0; i < m_stNumFaults; i++)
    {
      size_t stIndex = pstFaults[i];
      m_pFaults[stIndex] = TRUE; // this fault active...
    }
    LeaveCriticalSection(m_pLockFaults);
  }

  // figure out what fault we should simulate
  for (int j = 0; j < 2; j++)
  {
	size_t i = 0;
    for (i = m_stFaultToSimulate; i < m_stTotalFaults; i++)
    {
      if (m_pFaults[i]) // found one...
      {
        m_stFaultToSimulate = i;
        break;
      }
    }
    if (i == m_stTotalFaults)
      m_stFaultToSimulate = 0;
    else
      break;
  }

  TRACE2("[CLIENT:%Xh] FaultSimulation - next fault to simulate is %u\n", m_nThreadID, m_stFaultToSimulate);

  if (SIMULATE_FAULT(FAULT_NODATA) )
  {
    char szMessage[64];
    sprintf(szMessage,"FAULT - close before data [Socket:%u]", m_hClientSocket);
    TRACE1("**** %s\n", szMessage);
    OutputMessage((LPCSTR)szMessage, FAULT_MESSAGE);
    ExitThread();
    return;
  }
}

BOOL CWorkerThread::PreTranslateMessage(MSG* pMsg) 
{
  switch(pMsg->message)
  {
    case WM_QUIT:
      ExitInstance();
      return TRUE;

    case WM_CONNECT_ACCEPT:
      AcceptSocket(pMsg);
      break;

    case WM_SOCKET_DEAD:
	  {
      WPARAM wParam = pMsg->wParam;
      LPARAM lParam = pMsg->lParam;
	    if (wParam == 0 && lParam == 0)
		    return TRUE;

      SOCKET hSocket = (SOCKET)wParam;
      if (INVALID_SOCKET == hSocket)
        return TRUE;
      if ( (hSocket != m_hClientSocket) &&
           (hSocket != m_hServerSocket) )
        return TRUE;

	    closesocket(hSocket);

      if (m_hClientSocket == hSocket)
        m_hClientSocket = INVALID_SOCKET;
      if (m_hServerSocket == hSocket)
        m_hServerSocket = INVALID_SOCKET;

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

      if ( (hSocket != m_hClientSocket) && 
           (hSocket != m_hServerSocket) )
        return TRUE;

      if (hSocket == m_hClientSocket)
        TRACE1("[CLIENT:%Xh]",m_nThreadID);
      else
        TRACE1("[SERVER:%Xh]", m_nThreadID);

      int nErrorCode = WSAGETSELECTERROR(lParam);
      switch (WSAGETSELECTEVENT(lParam))
      {
        case FD_READ:
          TRACE1("FD_READ for socket %u\n", hSocket);
          OnReceive(nErrorCode, hSocket);
          break;
        case FD_CLOSE:
          TRACE1("FD_CLOSE for socket %u\n", hSocket);
          OnClose(nErrorCode, hSocket);
          break;
        case FD_CONNECT:
        {
          TRACE1("FD_CONNECT for socket %u", hSocket);
          time_t ttNow = time(NULL);
          CTime ctTime(ttNow);
          TRACE3(" at %u:%u:%02u\n", ctTime.GetHour(), ctTime.GetMinute(), ctTime.GetSecond() );

          OnConnect(nErrorCode, hSocket);
          break;
        }
        case FD_ACCEPT:
        case FD_WRITE:
        case FD_OOB:
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

void CWorkerThread::OnTimer(MSG* pMsg)
{
  if (0 == m_ttAccepted)
    return;
  time_t ttNow = time(NULL);
  time_t ttElapsed = ttNow - m_ttAccepted;
  if (ttElapsed < 300) // 5 minutes??
    return;

  TRACE("%%WARNING%% - thread timed out!!!\n");
  OutputMessage( (LPCSTR)"%Warning% - thread timed out!");
  ExitThread();
}

void CWorkerThread::OnConnect(int nErrorCode, SOCKET hSocket)
{
  ASSERT(INVALID_SOCKET != hSocket); // no invalids here!!
  if (hSocket == m_hServerSocket)
  {
    if (nErrorCode != 0)
    {
      TRACE3("[SERVER:%Xh], OnConnect() error %u for socket %u\n", 
            m_nThreadID, nErrorCode, hSocket);

      closesocket(hSocket);
      m_hServerSocket = INVALID_SOCKET;

      if (m_hClientSocket != INVALID_SOCKET)
        OnClose(0, m_hClientSocket);

      return;
    }

    if (NULL == m_pServerForward)
    {
      TRACE2("[SERVER:%Xh]OnConnect(), closing socket %u\n", m_nThreadID,
        hSocket);
      closesocket(m_hServerSocket);
      m_hServerSocket = INVALID_SOCKET;
      return;
    }
  
    // we connected, time to send...
    LPCSTR lpStart = (LPCSTR)&m_pServerForward->m_pData[m_pServerForward->m_iStart];
    size_t stLength = m_pServerForward->m_iEnd - m_pServerForward->m_iStart;
    int iStatus = send(hSocket, lpStart, stLength, 0);
    if (SOCKET_ERROR == iStatus)
    {
      int iLastError = WSAGetLastError();
      TRACE3("[SERVER:%Xh]OnConnect(), error sending [%d], closing socket %u\n", m_nThreadID, iLastError, hSocket);
      closesocket(hSocket);
      m_hServerSocket = INVALID_SOCKET;
      return;
    }

    if (stLength != (size_t)iStatus)
    {
      char cMessage[132];
      sprintf(cMessage,"Error sending data to server, closing socket [Socket:%u]", hSocket);
      OutputMessage((LPCSTR)cMessage, ERROR_MESSAGE);

      TRACE1("[SERVER:%Xh]OnConnect(), error sending", m_nThreadID);
      TRACE3("[%d bytes vs. %d bytes], closing socket %u\n", iStatus, stLength, hSocket);
      closesocket(hSocket);
      m_hServerSocket = INVALID_SOCKET;
    }

    {
      char cMessage[132];
      sprintf(cMessage,"%u bytes forward to server [Socket:%u]", stLength, hSocket);
      OutputMessage( (LPCSTR)cMessage, INFO_MESSAGE);
    }
    TRACE3("[SERVER:%Xh] forwarded %u bytes to socket %u\n", m_nThreadID, stLength, hSocket);

    // Now wait for data to come back...
    m_pServerForward->Reset();
    m_lstFree.AddTail(m_pServerForward);
    m_pServerForward = NULL;
    
    if (SIMULATE_FAULT(FAULT_HALF_CLOSE) )
    {
#define NO_SEND     1
#define NO_RECEIVES 0
#define NO_ANYTHING 3
      char szMessage[64];
      sprintf(szMessage,"FAULT - TCP half close [Socket:%u]", hSocket);
      TRACE1("**** %s\n", szMessage);
      OutputMessage((LPCSTR)szMessage, FAULT_MESSAGE);
      int iShutDown = shutdown(hSocket, NO_SEND);
      if (0 != iShutDown)
      {
        int LastError = WSAGetLastError();
        TRACE3("[SERVER:%Xh] - error %d shutting down the send of socket %u\n", m_nThreadID, LastError, hSocket);
      }
    }
    return;
  }

  ASSERT(FALSE); // shouldn't get here on client stuff!
}

void CWorkerThread::OnClose(int nErrorCode, SOCKET hSocket)
{
  if (hSocket == m_hServerSocket)
  {
    OnReceive(0, hSocket); // read any straggling data and forward
    m_hServerSocket = INVALID_SOCKET;
    if (m_hClientSocket != INVALID_SOCKET)
    {
      char cMessage[132];
      sprintf(cMessage,"Server socket closed [Socket:%u], closing client socket [Socket:%u]",
              hSocket, m_hClientSocket);
      OutputMessage( (LPCSTR)cMessage, INFO_MESSAGE);
      TRACE2("[CLIENT:%Xh]OnClose() closing socket %u\n", m_nThreadID, hSocket);
      closesocket(m_hClientSocket);
      m_hClientSocket = INVALID_SOCKET;
    }

    // send ourselves back to the listening thread!
    ExitThread();
  }

  if (hSocket == m_hClientSocket)
  {
    char cMessage[132];
    sprintf(cMessage, "Socket closed by client [Socket:%u]", m_hClientSocket);
    OutputMessage((LPCSTR)cMessage, INFO_MESSAGE);

    m_hClientSocket = INVALID_SOCKET;
    if (m_hServerSocket != INVALID_SOCKET)
    {
      TRACE2("...[SERVER:%Xh] closing server socket (%u)\n", m_nThreadID, m_hServerSocket);
      closesocket(m_hServerSocket);
      m_hServerSocket = INVALID_SOCKET;
    }

    // send ourselves back to the listening thread!
    ExitThread();
  }
}
void CWorkerThread::OnReceive(int nErrorCode, SOCKET hSocket)
{
  // ******************************
  // It's client data (from client)
  // ******************************
  if (hSocket == m_hClientSocket)
  {
    if (NULL == m_pClientReceiving)
    {
      if (m_lstFree.IsEmpty() )
      {
        TRACE1("%%ERROR%% - [CLIENT:%Xh] - no more buffers\n", m_nThreadID);
        closesocket(hSocket);
        m_hClientSocket = INVALID_SOCKET;
        return;
      }
      m_pClientReceiving = m_lstFree.RemoveHead();
    }

    BOOL bStatus = ReadSocket(hSocket, m_pClientReceiving);

    if (!bStatus)
    {
      m_lstFree.AddTail(m_pClientReceiving);
      m_pClientReceiving = NULL;
      return;
    }

    httpStatus ParsingStatus = ProcessClientData(m_pClientReceiving);
    if (ParsingStatus == HTTP_EXIT)
    {
      ExitThread();
      return;
    }

    // TODO: If an error, clean things up....

    return;
  }

  // ****************************
  // It's Server data coming back
  // ****************************
  if (hSocket == m_hServerSocket)
  {
    if (NULL == m_pClientForward)
      m_pClientForward = m_lstFree.RemoveHead();

    if (NULL == m_pClientForward)
    {
      TRACE("[SERVER] - couldn't get buffer for receive\n");
      ExitThread(); // couldn't get data, shut down and exit...
      return;
    }

    BOOL bStatus = ReadSocket(hSocket, m_pClientForward);
    if (!bStatus)
    {
      m_lstFree.AddTail(m_pClientForward);
      m_pClientForward = NULL;
      return;
    }

    // Okay we've read some data that is to go back to the 
    // client.
    
    httpStatus ParsingStatus = ProcessServerData(m_pClientForward);
    m_lstFree.AddTail(m_pClientForward);
    m_pClientForward = NULL;
    if (ParsingStatus == HTTP_EXIT)
    {
      ExitThread();
      return;
    }
  }
}

BOOL CWorkerThread::ReadSocket(SOCKET hSocket, CForwardObject* pObj)
{                                             
  // determine how much data is in the socket
  DWORD dwBytesInSocket = 0;
  int iStatus = ioctlsocket(hSocket, FIONREAD, &dwBytesInSocket);
  if (SOCKET_ERROR == iStatus)
  {
    pObj->Reset();
    closesocket(hSocket);
    return FALSE;
  }

  INT iNumInSocket = (INT)dwBytesInSocket;
  if (0 == iNumInSocket)
    return TRUE;

  INT iNumCanFit = pObj->m_iSize - pObj->m_iOffset;
  if (iNumCanFit <= 0)
    return FALSE; // should get another FD_READ

  INT iNumToRead = (iNumCanFit < iNumInSocket ? iNumCanFit : iNumInSocket);
  iStatus = recv(hSocket, (char *)&(pObj->m_pData[pObj->m_iOffset]), iNumToRead, 0);
  if (SOCKET_ERROR == iStatus)
  {
    pObj->m_iEnd   = 0;
    pObj->m_iStart = 0;
    closesocket(hSocket);
    return FALSE;
  }

  if (hSocket == m_hClientSocket)
    TRACE1("[CLIENT:%Xh]",m_nThreadID);
  if (hSocket == m_hServerSocket)
    TRACE1("[SERVER:%Xh]", m_nThreadID);

  TRACE2("read %u bytes from socket %u\n", iStatus, hSocket);

  pObj->m_iOffset += iStatus;
  return TRUE;
}

BOOL CWorkerThread::OnIdle(LONG lCount) 
{
  // by dumbing out of ::OnIdle() we go into a rather
  // benign ::GetMessage() loop
  return FALSE;
}

httpStatus CWorkerThread::ProcessClientData(CForwardObject* pObj)
{
  ASSERT(pObj != NULL);
  if ( (NULL == pObj) || (NULL == pObj->m_pData) )
    return HTTP_ERROR;

  LPSTR lpEnd = (LPSTR)&(pObj->m_pData[pObj->m_iOffset]);

  httpStatus Parsing = HTTP_PARSEMORE;

  while(Parsing == HTTP_PARSEMORE)
  {
    switch(m_ClientState)
    {
      case CWorkerThread::WAIT_INIT:
        m_ClientState = CWorkerThread::WAIT_URL;
        m_HttpRequestParser.Reset();
        break;
      case CWorkerThread::WAIT_URL: // waiting for the URL
      {
        TRACE("...[CLIENT] looking for URL\n");

        LPSTR lpEndScheme = strstr((LPSTR)pObj->m_pData,"\r\n");
        if (NULL == lpEndScheme)
          return HTTP_READMORE; // need more data

        BOOL bStatus = m_HttpRequestParser.ParseRequestLine(pObj);
        if (!bStatus)
          return HTTP_EXIT; // problem parsing URI

        // If not an application we know about, turn off fault
        // simulation...
        if (APP_UNKNOWN == m_HttpRequestParser.GetApplicationId())
        {
          char cMessage[132];
          sprintf(cMessage,"HTTP request - unknown application, probably browser [Socket:%u]", m_hClientSocket);
          m_doFaults = FALSE;
          TRACE1("[CLIENT] %s\n", cMessage);
          OutputMessage((LPCSTR)cMessage, INFO_MESSAGE);
        }
        m_ClientState = CWorkerThread::WAIT_ENTITY_HEADER;
        m_Method = m_HttpRequestParser.GetMethod();
        
        time_t ttNow = time(NULL);
        struct tm* ptm = localtime(&ttNow);
        if (ptm != NULL)
        {
          char lpMessage[255];
          size_t stEnd = 48;
          if (pObj->m_iOffset < stEnd)  // don't go off end of buffer
            stEnd = pObj->m_iOffset; 
          char cTemp = pObj->m_pData[stEnd];
          pObj->m_pData[stEnd] = '\0';
          sprintf(lpMessage,"HTTP request: \"%s\" at %u:%u:%02u [Socket:%u]", 
                  (LPSTR)pObj->m_pData, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, m_hClientSocket);
          pObj->m_pData[stEnd] = cTemp;
          OutputMessage((LPCSTR)lpMessage, INFO_MESSAGE);
        }

        DWORD CategoryId = m_HttpRequestParser.GetCategoryId();
        if (APP_UNKNOWN != m_HttpRequestParser.GetApplicationId())
        {
          char cMessage[132];
          sprintf(cMessage,"CategoryId: %u (%xH), LastId: %u [Socket:%u]",
                  CategoryId, CategoryId, m_HttpRequestParser.GetLastId(),
                  m_hClientSocket);
          OutputMessage((LPCSTR)cMessage, INFO_MESSAGE);
        }
        if ( (CategoryId == dkAdm_Registration) &&
             SIMULATE_FAULT(FAULT_BAD_REGISTRATIONID)&& m_doFaults)
        {
          // Okay send back a bogus response with a *bad* registration
          // Id...
          const char kszBadRegistration[] = "HTTP/1.0 200 Debug - Bad Registration\r\nContent-Length:19\r\n\r\nREGID:123456789\r\n\r\n"; 
          send(m_hClientSocket, kszBadRegistration, strlen(kszBadRegistration),0);
          char szMessage[64];
          sprintf(szMessage,"FAULT - bad registration Id (9 digits) [Socket:%u]", m_hClientSocket);
          TRACE1("**** %s\n", szMessage);
          OutputMessage((LPCSTR)szMessage);
          return HTTP_EXIT;
        }
        break;
      }
      case CWorkerThread::WAIT_ENTITY_HEADER:
      {
        // We found the URL, now we are looking for the 'content-length'
        TRACE("...[CLIENT] looking for entity header\n");
        LPSTR lpEndOfEntityHeader = strstr((LPSTR)pObj->m_pData, "\r\n\r\n");
        if (NULL == lpEndOfEntityHeader)
          return HTTP_READMORE;

        size_t stOffset = 0;
        size_t stSize = pObj->m_iOffset;
        httpStatus Status = m_HttpRequestParser.ParseEntityHeader(pObj);
        if (HTTP_DONE != Status)
          return Status; // error, exits & readmores

        m_stEntityBodyOffset = (lpEndOfEntityHeader + 4) - (LPCSTR)pObj->m_pData;
        if (pcnMETHOD_GET == m_Method)
        {
          m_ClientState = CWorkerThread::FORWARD_REQUEST; // done reading...
          break;
        }

        // Okay we found the content length string. the format of the data is:
        //
        // "content-length:<value>\r\n"

        m_stContentLength = m_HttpRequestParser.GetContentLength();
        if (0 == m_stContentLength)
          return HTTP_ERROR;
        m_ClientState = CWorkerThread::WAIT_ENTITY_BODY;
        break;
      }

      case CWorkerThread::WAIT_ENTITY_BODY:
      {
        size_t stMaxSize = 1500;
        if ( (m_stContentLength > stMaxSize) && SIMULATE_FAULT(FAULT_SMALL_UPLOADS) )
        {
          char szMessage[64];
          sprintf(szMessage,"FAULT - failing large upload til < %u bytes [Socket:%u]", stMaxSize, m_hClientSocket);
          TRACE1("**** %s\n", szMessage);
          OutputMessage((LPCSTR)szMessage, FAULT_MESSAGE);
          return HTTP_EXIT; // fail large uploads
        }
        TRACE1("...[CLIENT] waiting for entity body (must be POST) %u\n", m_hClientSocket);
        {
          char cMessage[132];
          sprintf(cMessage,"...waiting for entity body %u bytes long [Socket:%u]", 
                  m_stContentLength, m_hClientSocket);
          OutputMessage((LPCSTR)cMessage, INFO_MESSAGE);
        }
        if (pObj->m_iOffset >= (m_stContentLength + m_stEntityBodyOffset) )
          m_ClientState = CWorkerThread::FORWARD_REQUEST;
        else
          return HTTP_READMORE;
        
        break;
      }

      case CWorkerThread::FORWARD_REQUEST:
      {
        //
        // If simulating a timeout, then do nothing with the
        // data, don't close the connection and just kill
        // some time....
        //
        if (SIMULATE_FAULT(FAULT_TIMEOUT) )
        {
          CTime ctNow(time(NULL) );
          char szTime[32];
          sprintf(szTime,"FAULT - Timeout at %u:%u:%02u [Socket:%u]",
                  ctNow.GetHour(),
                  ctNow.GetMinute(),
                  ctNow.GetSecond(),
                  m_hClientSocket);
          TRACE1("**** %s\n", szTime);
          OutputMessage((LPCSTR)szTime, FAULT_MESSAGE);
          return HTTP_DONE;
        }

        if (SIMULATE_FAULT(FAULT_CLOSE_AFTER_DATA) )
        {
          char szMessage[64];
          sprintf(szMessage,"FAULT - close after data [Socket:%u]", m_hClientSocket);
          TRACE1("**** %s\n", szMessage);
          OutputMessage((LPCSTR)szMessage,FAULT_MESSAGE);
          return HTTP_EXIT;
        }


        if (SIMULATE_FAULT(FAULT_HTML_RESPONSE) )
        {
          char szMessage[64];
          sprintf(szMessage,"FAULT - respond HTML body [Socket:%u]", m_hClientSocket);
          TRACE1("**** %s\n", szMessage);
          OutputMessage((LPCSTR)szMessage,FAULT_MESSAGE);
          const char ksz200HTMLdata[] = "HTTP/1.0 200 Ok\r\n"
                                        "Content-type:text/html;\r\n";

          const char kszContentLength[] = "Content-Length:";
          const char ksz200HTMLbody[] = "<HTML>\r\n<HEAD>\r\n<TITLE>Test message</TITLE>\r\n"
                                        "</HEAD>\r\n"
                                        "<BODY>\r\n"
                                        "This is a test message sent instead of a data stream"
                                        "<P>\r\n</BODY>\r\n</HTML>";

          // How much do we need?
          UInt tlen = sizeof(ksz200HTMLdata) + sizeof(ksz200HTMLbody) + sizeof(kszContentLength) + 64;
          LPSTR lpResponse = new char[tlen];
          if (NULL == lpResponse)
              return HTTP_ERROR;

          UInt length = strlen(ksz200HTMLbody);
          sprintf(lpResponse,"%s%s:%u\r\n\r\n%s", 
                  ksz200HTMLdata,
                  kszContentLength,
                  length,
                  ksz200HTMLbody);
          UInt rlen = strlen(lpResponse);
          send(m_hClientSocket, lpResponse, rlen, 0);
          delete []lpResponse;
          return HTTP_EXIT;
        }

        TRACE("...[CLIENT] forwarding request to server\n");

        // First we have to compact the request since there's an IP address
        // in it...
        // "<method><sp>http://...../FIDO..."
        //              ^-----------^

        if (SIMULATE_FAULT(FAULT_200_NODATA))
        {
          const char ksz200NoData[] = "HTTP/1.0 200 Debug - Empty Response\r\n\r\n"; 
          send(m_hClientSocket, ksz200NoData, strlen(ksz200NoData),0);
          char szMessage[64];
          sprintf(szMessage,"FAULT - reply '200' no data [Socket:%u]", m_hClientSocket);
          TRACE1("**** %s\n", szMessage);
          OutputMessage((LPCSTR)szMessage, FAULT_MESSAGE);
          return HTTP_EXIT;
        }

        if (SIMULATE_FAULT(FAULT_204_DATA) )
        {
          const char ksz204Data[] = "HTTP/1.0 204 Debug - with data\r\nContent-Length:10\r\n\r\n1234567890"; 
          send(m_hClientSocket, ksz204Data, strlen(ksz204Data),0);

          char szMessage[64];
          sprintf(szMessage,"FAULT - reply '204' with data [Socket:%u]", m_hClientSocket);
          TRACE1("**** %s\n", szMessage);
          OutputMessage((LPCSTR)szMessage,FAULT_MESSAGE);
          return HTTP_EXIT;
        }

        // If this request came from a proxy....
        //
        // GET http://206.181.15.33:8081/FIDO-1
        //     ^4                       ^29
        // lpSchemeStart = 4
        // lpIPend       = 29
        //
        // stMethodLength = 4
        //
        // GET http://206.181.15.33:GET /FIDO-1
        //                          ^25
        //
        const char kszScheme[] = "http://";
        size_t stMethodLength = 0;
        LPSTR lpSchemeStart = (LPSTR)strstr((LPCSTR)pObj->m_pData, kszScheme);
        if (NULL != lpSchemeStart)
        {
            LPSTR lpIPend = (LPSTR)strstr(&lpSchemeStart[sizeof(kszScheme)], "/");
            if (NULL == lpIPend)
                return HTTP_ERROR;

            stMethodLength = lpSchemeStart - (LPCSTR)pObj->m_pData;
            lpIPend--;
            for (size_t i = 0; i < stMethodLength; i++)
                lpIPend[0-i] = pObj->m_pData[stMethodLength - 1 - i];

            // if we have to forward this request, we don't forward the
            // IP address since FaultSim is acting as a proxy server...
            pObj->m_iStart = (size_t)(lpIPend - (LPCSTR)pObj->m_pData) - stMethodLength + 1;
        
        }
        else
        {
            // There is no scheme, so it looks like this:
            // "GET /xxxxx"
            LPSTR lpURLstart = (LPSTR)strstr((LPCSTR)pObj->m_pData, "/");
            if (NULL == lpURLstart)
                return HTTP_ERROR;

            stMethodLength = lpURLstart - (LPCSTR)pObj->m_pData;
        }

        // If this is a post, then block it!!!
        if (SIMULATE_FAULT(FAULT_NO_POST) )
        {
          // See if the method is a 'POST'
          const char kszPOSTmethod[] = "POST";
          char szMethod[32];
          strncpy(szMethod, (const char *)pObj->m_pData, stMethodLength + 1);
          szMethod[stMethodLength] = '\0';
          if (strstr(szMethod,kszPOSTmethod) != NULL)
          {
            char szMessage[64];
            sprintf(szMessage,"FAULT - closing socket for POST method [Socket:%u]", m_hClientSocket);
            TRACE1("**** %s\n", szMessage);
            OutputMessage((LPCSTR)szMessage, FAULT_MESSAGE);
            return HTTP_EXIT;
          }
        }

        //
        // This fault will generate data responses for the
        // request (NOTE: Some are passed through if I don't
        //                want to deal with complicated responses)
        //
        if (SIMULATE_FAULT(FAULT_GENERATE_RESPONSES) )
        {
            CGenerateData gen;
            CGenerateData::genStatus Status = gen.GenerateResponse(&m_HttpRequestParser, m_hClientSocket);
            if (CGenerateData::RESPONSE_GENERATED == Status)
            {
                char szMessage[64];
                sprintf(szMessage,"FAULT - Simulated Response [Socket:%u]", m_hClientSocket);
                TRACE1("**** %s\n", szMessage);
                OutputMessage((LPCSTR)szMessage, FAULT_MESSAGE);
                return HTTP_EXIT;
            }
            if (CGenerateData::RESPONSE_ERROR == Status)
                return HTTP_ERROR;

            // for RESPONSE_NOTGENERATED we'll just proceed to the
            // server...
        }

        // Create our socket address 
        m_Server.sin_port = htons((u_short)m_HttpRequestParser.GetPort() );
        LPCSTR lpAddress = m_HttpRequestParser.GetTargetAddress();
        m_Server.sin_addr.s_addr = inet_addr(lpAddress);

	      if (m_Server.sin_addr.s_addr == INADDR_NONE)
	      {
          LPHOSTENT lphost;
          lphost = gethostbyname(lpAddress);
          if (lphost != NULL)
            m_Server.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
          else
          {
            TRACE1("[SERVER] - error getting address for '%s'\n", lpAddress);
            WSASetLastError(WSAEINVAL);
            return HTTP_EXIT;
          }
        }

        httpStatus forward = ForwardClientRequest(pObj);
        return forward;
      }
      default:
        break;
    }
  }

  return HTTP_READMORE;
}

//
// routine: ProcessServerData():
// purpose: This is where the data *from* the server comes back
//          and we would inject any faults.
//
httpStatus CWorkerThread::ProcessServerData(CForwardObject* pObj)
{
  // We have some data from the server, forward what we have to the client
  size_t stLength = pObj->m_iOffset;
  TRACE3("[SERVER:%Xh]Sending %u bytes to client socket %u\n", m_nThreadID, stLength, m_hClientSocket);
  if (0 == stLength)
    return HTTP_DONE;
  httpStatus retStatus = HTTP_DONE;
  
  INT iStatus;
  if (SIMULATE_FAULT(FAULT_PARTIAL) )
  {
    char szMessage[64];
    sprintf(szMessage,"FAULT - refusing upload of size %u for [Socket:%u]", stLength, m_hClientSocket);
    TRACE1("**** %s\n", szMessage);
    OutputMessage((LPCSTR)szMessage,FAULT_MESSAGE);
    size_t stPartialLength = stLength / 2;
    if (stPartialLength == 0)
      stPartialLength = stLength;
    iStatus = send(m_hClientSocket, (LPCSTR)pObj->m_pData, stPartialLength, 0);
    if (stPartialLength != stLength)
      retStatus = HTTP_EXIT;
  }
  else if (SIMULATE_FAULT(FAULT_SOLITARY_BYTE) )
  {
    char szMessage[64];
    sprintf(szMessage,"FAULT - Send first byte seperately to client [Socket:%u]", m_hClientSocket);
    TRACE1("**** %s\n", szMessage);
    OutputMessage((LPCSTR)szMessage,FAULT_MESSAGE);
    iStatus = send(m_hClientSocket, (LPCSTR)pObj->m_pData, 1, 0);
    Sleep(500); // wait 1/2 second
    iStatus = send(m_hClientSocket, (LPCSTR)(&pObj->m_pData[1]), stLength - 1, 0);
    if (iStatus != SOCKET_ERROR)
    {
      if (iStatus == (INT)(stLength-1))
        iStatus = stLength;
    }
  }
  else
    iStatus = send(m_hClientSocket, (LPCSTR)pObj->m_pData, stLength, 0);

  pObj->Reset();
  if (SOCKET_ERROR == iStatus)
  {
    TRACE2("[CLIENT:%Xh]Error sending server data to client %u\n", m_nThreadID, m_hClientSocket);
    int iLastError = WSAGetLastError();
    closesocket(m_hClientSocket);
    m_hClientSocket = INVALID_SOCKET;
    return HTTP_ERROR;
  }

  if (stLength != (size_t)iStatus)
  {
    TRACE1("[CLIENT:%Xh]", m_nThreadID);
    TRACE3("Error sending server data to client %u (%u of %u sent)\n",
            m_hClientSocket, iStatus, stLength);
    closesocket(m_hClientSocket);
    m_hClientSocket = INVALID_SOCKET;
    return HTTP_ERROR;
  }
  else
  {
    char szMessage[132];
    sprintf(szMessage,">>> %u bytes sent to client [Socket:%u]", iStatus, m_hClientSocket);
    OutputMessage((LPCSTR)szMessage, INFO_MESSAGE);
  }
  return retStatus;
}

httpStatus CWorkerThread::ForwardClientRequest(CForwardObject* pObj)
{
  m_pServerForward   = pObj;
  m_pClientReceiving = NULL;

  m_pServerForward->m_iEnd = m_pServerForward->m_iOffset;

  if (m_hServerSocket != INVALID_SOCKET)
  {
    closesocket(m_hServerSocket);
    m_hServerSocket = INVALID_SOCKET;
  }

  m_hServerSocket = socket(PF_INET, SOCK_STREAM, 0); 
  if (INVALID_SOCKET == m_hServerSocket)
  {
    TRACE1("[SERVER:%Xh] failed to create socket!\n", m_nThreadID);
    return HTTP_ERROR; // failed
  }
  // Now tie the socket to our listening window...
  _AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;

  // Now direct socket messages to us
  // m_pMainWnd is our main window (a CWnd)

  long lEvent = FD_CLOSE | FD_CONNECT | FD_READ;
  int iStatus = WSAAsyncSelect(m_hServerSocket, pState->m_hSocketWindow, WM_SOCKET_NOTIFY, lEvent);
  if (SOCKET_ERROR == iStatus)
  {
    TRACE1("[SERVER:%Xh] failed to select events\n", m_nThreadID);
    closesocket(m_hServerSocket);
    m_hServerSocket = INVALID_SOCKET;
    return HTTP_ERROR;
  }

  m_Server.sin_family = PF_INET;
  memset(m_Server.sin_zero, 0, 8);

  char lpAddress[32];
  sprintf(lpAddress,"%u.%u.%u.%u",
          m_Server.sin_addr.s_net,
          m_Server.sin_addr.s_host,
          m_Server.sin_addr.s_lh,
          m_Server.sin_addr.s_impno);

  TRACE1("[SERVER:%Xh]", m_nThreadID);
  TRACE3("connecting to %s, port %u, socket %u",
         lpAddress, ntohs(m_Server.sin_port), m_hServerSocket);
  
  time_t ttNow = time(NULL);
  CTime ctTime(ttNow);
  TRACE3(" at %u:%u:%02u\n", ctTime.GetHour(), ctTime.GetMinute(), ctTime.GetSecond() );

  iStatus = connect(m_hServerSocket, (SOCKADDR *)&m_Server, sizeof(SOCKADDR_IN) );
  if (iStatus != 0)
  {
    int iLastError = WSAGetLastError();
    switch(iLastError)
    {
      case WSAEWOULDBLOCK:
        TRACE2("...[SERVER:%Xh] - WSAEWOULDBLOCK for %u\n", m_nThreadID, m_hServerSocket);
        return HTTP_DONE;
      default:
        return HTTP_ERROR;
  
    }
  }
  OnConnect(0, m_hServerSocket); // synchronous, connect is done!
  return HTTP_DONE;
}

void CWorkerThread::ExitThread()
{
  m_stFaultToSimulate++; // advance our fault simulation pointer

  if (m_hClientSocket != INVALID_SOCKET)
  {
    closesocket(m_hClientSocket);
    m_hClientSocket = INVALID_SOCKET;
  }

  if (m_hServerSocket != INVALID_SOCKET)
  {
    closesocket(m_hServerSocket);
    m_hServerSocket = INVALID_SOCKET;
  }

  if (m_pClientReceiving != NULL)
  {
    m_pClientReceiving->Reset();
    m_lstFree.AddTail(m_pClientReceiving);
    m_pClientReceiving = NULL;
  }

  if (m_pServerForward != NULL)
  {
    m_pServerForward->Reset();
    m_lstFree.AddTail(m_pServerForward);
    m_pServerForward = NULL;
  }


  TRACE("**** ExitThread[%u]\n", m_nThreadID);
  m_ttAccepted = 0; // disables timeout checks
  BOOL bStatus = ::PostThreadMessage(m_dwParentThreadId, WM_ENQUEUE_WORKERTHREAD, (WPARAM)0, (LPARAM)m_nThreadID);
}

void CWorkerThread::OutputMessage(LPSTR lpMessage, messageType mtMessage)
{
  WPARAM wParam = MAKEWPARAM((WORD)mtMessage, 0);
  BOOL bStatus = ::PostThreadMessage(m_dwParentThreadId, WM_DISPLAY_TEXT, wParam, (LPARAM)lpMessage);
}

void CWorkerThread::OutputMessage(LPCSTR lpMessage, messageType mtMessage /* = FAULT_MESSAGE */)
{
  LPSTR lpNewMessage = new char[strlen(lpMessage) + 1];
  if (NULL == lpNewMessage)
    return;
  strcpy(lpNewMessage, lpMessage);
  OutputMessage(lpNewMessage, mtMessage);
}


// --------------------------------------------------------------------

