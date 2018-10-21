// FaultSimView.h : interface of the CFaultSimView class
//
/////////////////////////////////////////////////////////////////////////////

class CFaultSimView : public CScrollView
{
protected: // create from serialization only
	CFaultSimView();
	DECLARE_DYNCREATE(CFaultSimView)

// Attributes
public:
	CFaultSimDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFaultSimView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFaultSimView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFaultSimView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FaultSimView.cpp
inline CFaultSimDoc* CFaultSimView::GetDocument()
   { return (CFaultSimDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
