; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFaultSimApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "FaultSim.h"
LastPage=0

ClassCount=10
Class1=CFaultSimApp
Class2=CFaultSimDoc
Class3=CFaultSimView
Class4=CMainFrame

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_CONFIGURATION
Class5=CAboutDlg
Class6=CChildFrame
Class7=CListenWindow
Class8=CListenThread
Class9=CWorkerThread
Class10=CConfigurationDialog
Resource4=IDR_FAULTSTYPE

[CLS:CFaultSimApp]
Type=0
HeaderFile=FaultSim.h
ImplementationFile=FaultSim.cpp
Filter=N
LastObject=ID_SAVE_LOG
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CFaultSimDoc]
Type=0
HeaderFile=FaultSimDoc.h
ImplementationFile=FaultSimDoc.cpp
Filter=N
LastObject=CFaultSimDoc
BaseClass=CDocument
VirtualFilter=DC

[CLS:CFaultSimView]
Type=0
HeaderFile=FaultSimView.h
ImplementationFile=FaultSimView.cpp
Filter=C
BaseClass=CScrollView
VirtualFilter=VWC
LastObject=CFaultSimView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CMDIFrameWnd
VirtualFilter=fWC


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
LastObject=CChildFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=FaultSim.cpp
ImplementationFile=FaultSim.cpp
Filter=D
LastObject=CAboutDlg
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=ID_VIEW_TOOLBAR
Command6=ID_VIEW_STATUS_BAR
Command7=ID_APP_ABOUT
CommandCount=7

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_CONFIGURE
CommandCount=9

[MNU:IDR_FAULTSTYPE]
Type=1
Class=CFaultSimView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_SAVE_ALL
Command7=ID_FILE_MRU_FILE1
Command8=ID_APP_EXIT
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_VIEW_TOOLBAR
Command14=ID_VIEW_STATUS_BAR
Command15=ID_WINDOW_CASCADE
Command16=ID_WINDOW_TILE_HORZ
Command17=ID_WINDOW_ARRANGE
Command18=ID_CLEAR_LOG
Command19=ID_SAVE_LOG
Command20=ID_APP_ABOUT
CommandCount=20

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[CLS:CListenWindow]
Type=0
HeaderFile=ListenWindow.h
ImplementationFile=ListenWindow.cpp
BaseClass=CMDIChildWnd
Filter=M
VirtualFilter=mfWC
LastObject=CListenWindow

[CLS:CListenThread]
Type=0
HeaderFile=ListenThread.h
ImplementationFile=ListenThread.cpp
BaseClass=CWinThread
Filter=N
VirtualFilter=TC
LastObject=CListenThread

[CLS:CWorkerThread]
Type=0
HeaderFile=WorkerThread.h
ImplementationFile=WorkerThread.cpp
BaseClass=CWinThread
Filter=N
VirtualFilter=TC
LastObject=CWorkerThread

[DLG:IDD_CONFIGURATION]
Type=1
Class=CConfigurationDialog
ControlCount=9
Control1=IDC_TCPIP_PORT,edit,1350631552
Control2=IDC_STATIC,static,1342308352
Control3=IDC_FAULT_LIST,listbox,1352728841
Control4=IDC_STATIC,button,1342177287
Control5=IDOK,button,1342242816
Control6=IDCANCEL,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_ALL,button,1342242816
Control9=IDC_NONE,button,1342242816

[CLS:CConfigurationDialog]
Type=0
HeaderFile=ConfigurationDialog.h
ImplementationFile=ConfigurationDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_NONE
VirtualFilter=dWC

