# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=FaultSim - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to FaultSim - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "FaultSim - Win32 Release" && "$(CFG)" !=\
 "FaultSim - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "FaultSim.mak" CFG="FaultSim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FaultSim - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FaultSim - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "FaultSim - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "FaultSim - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\FaultSim.exe"

CLEAN : 
	-@erase "$(INTDIR)\ConfigurationDialog.obj"
	-@erase "$(INTDIR)\FaultSim.obj"
	-@erase "$(INTDIR)\FaultSim.pch"
	-@erase "$(INTDIR)\FaultSim.res"
	-@erase "$(INTDIR)\FaultSimDoc.obj"
	-@erase "$(INTDIR)\FaultSimView.obj"
	-@erase "$(INTDIR)\GenerateData.obj"
	-@erase "$(INTDIR)\HttpRequestParser.obj"
	-@erase "$(INTDIR)\ListenThread.obj"
	-@erase "$(INTDIR)\ListenWindow.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\WorkerThread.obj"
	-@erase "$(OUTDIR)\FaultSim.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/FaultSim.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/FaultSim.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/FaultSim.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/FaultSim.pdb" /machine:I386 /out:"$(OUTDIR)/FaultSim.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ConfigurationDialog.obj" \
	"$(INTDIR)\FaultSim.obj" \
	"$(INTDIR)\FaultSim.res" \
	"$(INTDIR)\FaultSimDoc.obj" \
	"$(INTDIR)\FaultSimView.obj" \
	"$(INTDIR)\GenerateData.obj" \
	"$(INTDIR)\HttpRequestParser.obj" \
	"$(INTDIR)\ListenThread.obj" \
	"$(INTDIR)\ListenWindow.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WorkerThread.obj"

"$(OUTDIR)\FaultSim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FaultSim - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\FaultSim.exe"

CLEAN : 
	-@erase "$(INTDIR)\ConfigurationDialog.obj"
	-@erase "$(INTDIR)\FaultSim.obj"
	-@erase "$(INTDIR)\FaultSim.pch"
	-@erase "$(INTDIR)\FaultSim.res"
	-@erase "$(INTDIR)\FaultSimDoc.obj"
	-@erase "$(INTDIR)\FaultSimView.obj"
	-@erase "$(INTDIR)\GenerateData.obj"
	-@erase "$(INTDIR)\HttpRequestParser.obj"
	-@erase "$(INTDIR)\ListenThread.obj"
	-@erase "$(INTDIR)\ListenWindow.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\WorkerThread.obj"
	-@erase "$(OUTDIR)\FaultSim.exe"
	-@erase "$(OUTDIR)\FaultSim.map"
	-@erase "$(OUTDIR)\FaultSim.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/FaultSim.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/FaultSim.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/FaultSim.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /incremental:no /map /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/FaultSim.pdb" /map:"$(INTDIR)/FaultSim.map" /debug\
 /machine:I386 /out:"$(OUTDIR)/FaultSim.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ConfigurationDialog.obj" \
	"$(INTDIR)\FaultSim.obj" \
	"$(INTDIR)\FaultSim.res" \
	"$(INTDIR)\FaultSimDoc.obj" \
	"$(INTDIR)\FaultSimView.obj" \
	"$(INTDIR)\GenerateData.obj" \
	"$(INTDIR)\HttpRequestParser.obj" \
	"$(INTDIR)\ListenThread.obj" \
	"$(INTDIR)\ListenWindow.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WorkerThread.obj"

"$(OUTDIR)\FaultSim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "FaultSim - Win32 Release"
# Name "FaultSim - Win32 Debug"

!IF  "$(CFG)" == "FaultSim - Win32 Release"

!ELSEIF  "$(CFG)" == "FaultSim - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "FaultSim - Win32 Release"

!ELSEIF  "$(CFG)" == "FaultSim - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FaultSim.cpp
DEP_CPP_FAULT=\
	".\ChildFrm.h"\
	".\ConfigurationDialog.h"\
	".\FaultSim.h"\
	".\FaultSimDoc.h"\
	".\FaultSimMessages.h"\
	".\FaultSimView.h"\
	".\ListenThread.h"\
	".\ListenWindow.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\FaultSim.obj" : $(SOURCE) $(DEP_CPP_FAULT) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "FaultSim - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/FaultSim.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\FaultSim.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "FaultSim - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/FaultSim.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\FaultSim.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\FaultSim.h"\
	".\ListenThread.h"\
	".\ListenWindow.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FaultSimDoc.cpp
DEP_CPP_FAULTS=\
	".\FaultSim.h"\
	".\FaultSimDoc.h"\
	".\ListenThread.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\FaultSimDoc.obj" : $(SOURCE) $(DEP_CPP_FAULTS) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FaultSimView.cpp
DEP_CPP_FAULTSI=\
	".\FaultSim.h"\
	".\FaultSimDoc.h"\
	".\FaultSimView.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\FaultSimView.obj" : $(SOURCE) $(DEP_CPP_FAULTSI) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FaultSim.rc
DEP_RSC_FAULTSIM=\
	".\res\FaultSim.ico"\
	".\res\FaultSim.rc2"\
	".\res\FaultSimDoc.ico"\
	".\res\Toolbar.bmp"\
	

"$(INTDIR)\FaultSim.res" : $(SOURCE) $(DEP_RSC_FAULTSIM) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ListenWindow.cpp
DEP_CPP_LISTE=\
	".\FaultSim.h"\
	".\FaultSimDoc.h"\
	".\FaultSimMessages.h"\
	".\ListenThread.h"\
	".\ListenWindow.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ListenWindow.obj" : $(SOURCE) $(DEP_CPP_LISTE) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ListenThread.cpp
DEP_CPP_LISTEN=\
	".\FaultSim.h"\
	".\FaultSimDoc.h"\
	".\FaultSimMessages.h"\
	".\HttpParsingInfo.h"\
	".\HttpRequestParser.h"\
	".\ListenThread.h"\
	".\StdAfx.h"\
	".\WorkerThread.h"\
	

"$(INTDIR)\ListenThread.obj" : $(SOURCE) $(DEP_CPP_LISTEN) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\WorkerThread.cpp
DEP_CPP_WORKE=\
	".\ADM_IDS.H"\
	".\CATIDS.H"\
	".\FaultSim.h"\
	".\FaultSimMessages.h"\
	".\HttpParsingInfo.h"\
	".\HttpRequestParser.h"\
	".\ListenThread.h"\
	".\StdAfx.h"\
	".\WorkerThread.h"\
	".\XPTYPES.H"\
	
NODEP_CPP_WORKE=\
	".\CObject.h"\
	".\CString.h"\
	".\MBPrintf.h"\
	".\pprint.h"\
	".\UDebugging.h"\
	

"$(INTDIR)\WorkerThread.obj" : $(SOURCE) $(DEP_CPP_WORKE) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ConfigurationDialog.cpp
DEP_CPP_CONFI=\
	".\ConfigurationDialog.h"\
	".\FaultSim.h"\
	".\FaultSimMessages.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ConfigurationDialog.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\HttpRequestParser.cpp
DEP_CPP_HTTPR=\
	".\HttpParsingInfo.h"\
	".\HttpRequestParser.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\HttpRequestParser.obj" : $(SOURCE) $(DEP_CPP_HTTPR) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GenerateData.cpp
DEP_CPP_GENER=\
	".\CATIDS.H"\
	".\GenerateData.h"\
	".\HttpParsingInfo.h"\
	".\HttpRequestParser.h"\
	".\XPTYPES.H"\
	
NODEP_CPP_GENER=\
	".\CObject.h"\
	".\CString.h"\
	".\MBPrintf.h"\
	".\pprint.h"\
	".\UDebugging.h"\
	

"$(INTDIR)\GenerateData.obj" : $(SOURCE) $(DEP_CPP_GENER) "$(INTDIR)"\
 "$(INTDIR)\FaultSim.pch"


# End Source File
# End Target
# End Project
################################################################################
