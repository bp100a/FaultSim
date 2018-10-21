// FaultSimView.cpp : implementation of the CFaultSimView class
//

#include "stdafx.h"
#include "FaultSim.h"

#include "FaultSimDoc.h"
#include "FaultSimView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFaultSimView

IMPLEMENT_DYNCREATE(CFaultSimView, CScrollView)

BEGIN_MESSAGE_MAP(CFaultSimView, CScrollView)
	//{{AFX_MSG_MAP(CFaultSimView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaultSimView construction/destruction

CFaultSimView::CFaultSimView()
{
	// TODO: add construction code here

}

CFaultSimView::~CFaultSimView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CFaultSimView drawing

void CFaultSimView::OnDraw(CDC* pDC)
{
	CFaultSimDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
  
  CStringList* pList = &pDoc->m_lstStrings;

  TEXTMETRIC txtMetrics; 
  BOOL bStatus = pDC->GetTextMetrics(&txtMetrics);
  if (!bStatus)
    return;

  int Height  = pDoc->GetLineHeight(txtMetrics.tmHeight);
  int MapMode = pDC->GetMapMode();

  COLORREF redText = RGB(255,0,0);
  COLORREF blueText = RGB(0, 0, 255);

  COLORREF faultText = pDC->GetNearestColor(redText); 
  COLORREF errorText = pDC->GetNearestColor(blueText);
  COLORREF normText = 0; // black
  COLORREF currentColor = pDC->GetTextColor();
  
  // For optimal drawing, get the bounding rectangle
  RECT rect;
  pDC->GetClipBox(&rect);
  //
  // (rect.left,rect.top) -> (rect.right, rect.bottom)
  // 
  
  int LineStart = abs(rect.top) / Height;
  if (0 != LineStart)
    LineStart--;
  int iY = LineStart * Height * (MM_LOENGLISH == MapMode ? -1 : 1);
  int iX = 0;
/*
  COLORREF bkColor = pDC->GetBkColor();
  CBrush brush;
  brush.CreateSolidBrush(bkColor);
  pDC->FillRect(&rect, &brush);
*/
  int Bottom = abs(rect.bottom) + Height;
  for (POSITION pos = pList->FindIndex(LineStart); pos; NULL)
  {
    CString strLine = pList->GetNext(pos);
    CString strType = strLine.Left(2);
    LPCSTR lpLine   = strLine;
    lpLine          = &lpLine[2]; // get pass message type

    messageType mtMessage = (messageType)atoi(strType);
    COLORREF newColor;
    switch(mtMessage)
    {
    case FAULT_MESSAGE:
      {
        newColor = faultText;
        break;
      }
    case ERROR_MESSAGE:
      {
        newColor = errorText;
        break;
      }
    case INFO_MESSAGE:
    case STATUS_MESSAGE:
    default:
      {
        newColor = normText;
        break;
      }
    }

    if (currentColor != newColor)
    {
      pDC->SetTextColor(newColor);
      currentColor = newColor;
    }

    bStatus = pDC->TextOut(iX, iY, lpLine, strLine.GetLength() - 2);
    if (!bStatus)
      return;

    if (MM_LOENGLISH == MapMode)
      iY -= Height;
    else
      iY += Height;


    if (Bottom < abs(iY) )
      break;
  }
}


/////////////////////////////////////////////////////////////////////////////
// CFaultSimView diagnostics

#ifdef _DEBUG
void CFaultSimView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CFaultSimView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CFaultSimDoc* CFaultSimView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFaultSimDoc)));
	return (CFaultSimDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFaultSimView message handlers

void CFaultSimView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  // Now see what stuff the document has to view
  CFaultSimDoc* pDoc = (CFaultSimDoc*)GetDocument();
  // Implement a GetDocSize( ) member function in 
  // your document class; it returns a CSize.
  CSize docSize = pDoc->GetDocSize(this);
  int HeightPerLine = pDoc->GetLineHeight(this);

  CSize pageSize(docSize.cx /2, docSize.cy / 4);
  
  CSize lineSize(2, HeightPerLine/3);

  SetScrollSizes(MM_LOENGLISH, docSize, pageSize, lineSize);

  // It would be nice to resize window the very first time...
  //  ResizeParentToFit();   // Default bShrinkOnly argument

  const CStringList* pList = &pDoc->m_lstStrings;
//  if (pList->IsEmpty() )
//    return; // nothing to see
  
  CScrollView::OnUpdate(pSender, lHint, pHint);
}

void CFaultSimView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
  CDC* pDC = GetDC();
  int HeightPerLine = 20;
  int AveCharWidth  = 20;
  if (pDC != NULL)
  {
    TEXTMETRIC txtMetrics; 
    BOOL bStatus = pDC->GetTextMetrics(&txtMetrics);
    if (bStatus)
    {
      HeightPerLine = txtMetrics.tmHeight;
      AveCharWidth  = txtMetrics.tmAveCharWidth;
    }
    ReleaseDC(pDC);
    pDC = NULL;
  }

  CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = AveCharWidth * 64;
  sizeTotal.cy = HeightPerLine * 16;
	SetScaleToFitSize(sizeTotal);
  SetScrollSizes(MM_LOENGLISH, sizeTotal);
  ResizeParentToFit(FALSE);
}
