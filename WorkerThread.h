// WorkerThread.h : header file
//
#include <afxtempl.h>
#include "HttpRequestParser.h"
#include "FaultSimMessages.h"

/////////////////////////////////////////////////////////////////////////////
// CWorkerThread thread
class CListenThread;

class CWorkerThread : public CWinThread
{
	DECLARE_DYNCREATE(CWorkerThread)
protected:
	CWorkerThread();           // protected constructor used by dynamic creation

// Attributes
public:
  void SetFaultLock(CRITICAL_SECTION* pLock) { m_pLockFaults = pLock; }
  void SetParent(CListenThread* pParent) { m_pParent = pParent; }

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkerThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWorkerThread();

  void OnReceive    (int nErrorCode, SOCKET hSocket);
  void OnClose      (int nErrorCode, SOCKET hSocket);
  void OnConnect    (int nErrorCode, SOCKET hSocket);
  
  void OnTimer      (MSG* pMsg);
  void AcceptSocket (MSG* pMsg);

	// Generated message map functions
	//{{AFX_MSG(CWorkerThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
  CWnd*  m_pWnd;
  SOCKET m_hClientSocket; // read/write client
  SOCKET m_hServerSocket; // read/write server
  DWORD  m_dwParentThreadId; // set by the WM_CONNECT_ACCEPT msg.
  
  typedef enum {WAIT_INIT,
                WAIT_URL,
                WAIT_ENTITY_HEADER,
                WAIT_ENTITY_BODY,
                FORWARD_REQUEST} clientStates;
  clientStates m_ClientState;
  size_t m_stContentLength;
  size_t m_stEntityBodyOffset;

  httpStatus ProcessClientData(CForwardObject* pObj);

  CList<CForwardObject*, CForwardObject*> m_lstFree;     // free buffers for receive
  CForwardObject* m_pClientReceiving;

  httpMethods m_Method;
  SOCKADDR_IN m_Server;

  httpStatus ProcessServerData(CForwardObject* pObj); // here's were faults will be injected
  httpStatus ForwardClientRequest(CForwardObject* pObj);
  BOOL ReadSocket(SOCKET hSocket, CForwardObject* pObj);

  CForwardObject* m_pServerForward;
  CForwardObject* m_pClientForward;
  void ExitThread();
  void OutputMessage(LPSTR lpMessage, messageType mtMessage = INFO_MESSAGE);
  void OutputMessage(LPCSTR lpMessage, messageType mtMessage = INFO_MESSAGE);

  time_t m_ttAccepted;

  size_t  m_stNumFaults;
  BOOL* m_pFaults; // array of indices
  CRITICAL_SECTION* m_pLockFaults;
  CListenThread* m_pParent; // our parent thread
  static size_t m_stTotalFaults;

  size_t m_stFaultToSimulate;
  BOOL   m_doFaults;
  CHttpRequestParser m_HttpRequestParser;
};

#define SIMULATE_FAULT(a) (m_pFaults != NULL ? m_pFaults[a] && (a == m_stFaultToSimulate) : FALSE)

/////////////////////////////////////////////////////////////////////////////
