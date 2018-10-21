// FaultSimDoc.h : interface of the CFaultSimDoc class
//
/////////////////////////////////////////////////////////////////////////////

#include "FaultSimMessages.h" // get message type definition

class CListenThread;

class CFaultSimDoc : public CDocument
{
protected: // create from serialization only
	CFaultSimDoc();
	DECLARE_DYNCREATE(CFaultSimDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFaultSimDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFaultSimDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFaultSimDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  static int m_iDocCount;
  int m_iDocID; // our unique document identifier
  int GetDocId() { return m_iDocID; }
  CStringList m_lstStrings;

  void SetFaults(size_t m_stNumFaults, const size_t* m_pFaults);
  size_t GetNumFaults()     { return m_stNumFaults; }
  const size_t* GetFaults();
  UINT   GetListeningPort();

  CSize GetDocSize(CView* pView);
  int   GetLineHeight(int Height);
  int   GetLineHeight(CView* pView);
  int   GetLineWidth(CView* pView); // width of longest line in logical units
  void  InvalidatePort(UINT uiPort);
  void  ValidatePort(UINT uiPort);
  size_t GetLongestLine() { return m_stLongestLine; }
  void   SetLongestLine(size_t stLength) { m_stLongestLine = stLength; }

  DWORD  GetListeningThreadId();

  void SetMessage(LPSTR lpMessage, messageType mtMessage);

  void ClearLog();
  void SaveLog(CString& strLogName);

protected:
  virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);

private:
  BOOL    InitializeListeningThread();
  size_t  m_stNumFaults;
  size_t* m_pstFaults;
  BOOL    m_bValidPort;
  size_t  m_stLongestLine;

  CListenThread* m_pListenThread;

#define MAX_IPADDRESSES 4
  IN_ADDR m_iaLocalIPAddresses[MAX_IPADDRESSES];
  static IN_ADDR m_iaListenAdapter;
  int m_iLocalNetCardCount;
  BOOL InitializeLocalIPAddresses();

  static BOOL m_bIPaddressInitialized;
};

/////////////////////////////////////////////////////////////////////////////
