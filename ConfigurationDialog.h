// ConfigurationDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigurationDialog dialog

class CConfigurationDialog : public CDialog
{
// Construction
public:
	CConfigurationDialog(CWnd* pParent = NULL);   // standard constructor
  size_t GetNumSelected() { return m_stNumSelected; }
  size_t* GetSelections();
  ~CConfigurationDialog();

  void SetNumSelected(size_t stNumSelected) { m_stNumSelected = stNumSelected; }
  void SetSelections(size_t* pstSelections) { m_pstSelections = pstSelections; }
  void SetPortValue(int iPort) { m_iPortValue = iPort; }

  // Dialog Data
	//{{AFX_DATA(CConfigurationDialog)
	enum { IDD = IDD_CONFIGURATION };
	CString	m_strPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigurationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigurationDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAll();
	afx_msg void OnNone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  size_t* m_pstSelections;
  size_t  m_stNumSelected;
  int     m_iPortValue;
};
