// ConfigurationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FaultSim.h"
#include "ConfigurationDialog.h"
#include "FaultSimMessages.h" // get string definitions

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigurationDialog dialog


CConfigurationDialog::CConfigurationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigurationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigurationDialog)
	m_strPort = _T("");
	//}}AFX_DATA_INIT
  m_iPortValue = 81;
}

void CConfigurationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigurationDialog)
	DDX_Text(pDX, IDC_TCPIP_PORT, m_strPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigurationDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigurationDialog)
	ON_BN_CLICKED(IDC_ALL, OnAll)
	ON_BN_CLICKED(IDC_NONE, OnNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigurationDialog message handlers

BOOL CConfigurationDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

  char cPort[32];
  sprintf(cPort,"%u", m_iPortValue);
  m_strPort = cPort; // default

  SendDlgItemMessage(IDC_TCPIP_PORT, WM_SETTEXT, (WPARAM)0, (LPARAM)(LPCSTR)m_strPort);

  size_t i = 0;
  for (i = 0; glb_lpFaultStrings[i] != NULL; i++)
  {
    LRESULT lStatus = SendDlgItemMessage(IDC_FAULT_LIST, LB_ADDSTRING, (WPARAM)0, (LPARAM)glb_lpFaultStrings[i]);
    if (lStatus >= 0)
      continue;
    break;
  }

  if (m_pstSelections != NULL)
  {
    for (i = 0; i < m_stNumSelected; i++)
    {
      size_t stIndex = m_pstSelections[i];
      LPARAM lParam = MAKELPARAM((WORD)stIndex, (WORD)stIndex);
      LRESULT lStatus = SendDlgItemMessage(IDC_FAULT_LIST, LB_SELITEMRANGE, (WPARAM)TRUE, lParam);

    }
  }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CConfigurationDialog::~CConfigurationDialog()
{
  delete []m_pstSelections;
}

size_t* CConfigurationDialog::GetSelections()
{
  size_t* pSelections = m_pstSelections;
  m_pstSelections = NULL;
  return pSelections;
}

void CConfigurationDialog::OnOK() 
{
  UpdateData();
  
  m_stNumSelected = 0;
  LRESULT lStatus = SendDlgItemMessage(IDC_FAULT_LIST, LB_GETSELCOUNT, (WPARAM)0, (LPARAM)0);
  if (lStatus >= 0)
    m_stNumSelected = (size_t)lStatus;
  
  m_pstSelections = new size_t[m_stNumSelected+1];
  m_pstSelections[0] = (size_t)atoi(m_strPort);
  if (m_stNumSelected != 0)
  {
    if (NULL != m_pstSelections)
    {
      lStatus = SendDlgItemMessage(IDC_FAULT_LIST, LB_GETSELITEMS, (WPARAM)m_stNumSelected, (LPARAM)&m_pstSelections[1]);
      if (m_stNumSelected != (size_t)lStatus)
        m_stNumSelected = 0;
    }
  }

	CDialog::OnOK();
}

void CConfigurationDialog::OnAll() 
{
  size_t i = 0;
  for (i = 0; glb_lpFaultStrings[i] != NULL; i++) NULL;
  
  LPARAM lParam = MAKELPARAM(0, i);
  LRESULT lStatus = SendDlgItemMessage(IDC_FAULT_LIST, LB_SELITEMRANGE, (WPARAM)TRUE, lParam);
}

void CConfigurationDialog::OnNone() 
{
  size_t i = 0;

  for (i = 0; glb_lpFaultStrings[i] != NULL; i++) NULL;
  
  LPARAM lParam = MAKELPARAM(0, i);
  LRESULT lStatus = SendDlgItemMessage(IDC_FAULT_LIST, LB_SELITEMRANGE, (WPARAM)FALSE, lParam);
}
