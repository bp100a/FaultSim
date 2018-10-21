// ListenThread.h : header file
//
#include <afxtempl.h>
#include "winsock.h" // need some definitions
#include "FaultSimMessages.h"

/////////////////////////////////////////////////////////////////////////////
// CListenThread thread

class CListenThread : public CWinThread
{
	DECLARE_DYNCREATE(CListenThread)
protected:
	CListenThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListenThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
public:
  void SetListeningPort(WORD wListeningPort) { m_wListeningPort = wListeningPort;}
  void SetIPaddress(DWORD dwIPaddress) { m_iaIPaddress.s_addr = dwIPaddress; }
  void SetParent(CWinApp* pParent)     { m_pParent = pParent; }
  void SetParentDocId(int iDocId)      { m_iParentDocID = iDocId; }
 
  size_t  GetNumFaults();
  size_t* GetFaults();
  int GetPortValue() { return (int)m_wListeningPort; }
  int GetDocId()     { return m_iParentDocID; }
  void SetDocId(int iDocId) { m_iParentDocID = iDocId; }
protected:

  BOOL     SetupSocket(BOOL bCloseSocket = FALSE);
  WORD     m_wListeningPort;   // port we are listening on
  IN_ADDR  m_iaIPaddress;
  CWinApp* m_pParent;
  int      m_iParentDocID;
  size_t   m_stNumFaults;
  size_t*  m_pstFaults;
  CRITICAL_SECTION m_lockFaults;
  CWnd*    m_pWnd;

	SOCKET  m_hSocket;          // what we're listening to
 	CList<int, int>				m_FreeThreadList;

	void OnAccept  (int nErrorCode);
	void OnConnect (int nErrorCode);
	void OnClose   (int nErrorCode);

  void OnTimer   (MSG* pMsg);

	virtual ~CListenThread();
  BOOL OutputMessage(LPSTR lpMessage, messageType mtMessage = INFO_MESSAGE);

	// Generated message map functions
	//{{AFX_MSG(CListenThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
