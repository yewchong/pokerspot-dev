# Microsoft Developer Studio Project File - Name="Poker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Poker - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Poker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Poker.mak" CFG="Poker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Poker - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Poker - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Poker - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "shell\Release"
# PROP Intermediate_Dir "shell\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Ob1 /I "." /I "d:\openssl-0.9.4\inc32" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "PCLIENT_MODULE_" /D "HTML_VIEW_" /D "PSPOT_TABLE_MODULE_" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /map /debug /machine:I386 /out:"table.exe"

!ELSEIF  "$(CFG)" == "Poker - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "shell\Debug"
# PROP Intermediate_Dir "shell\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "d:\openssl-0.9.4\inc32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "PCLIENT_MODULE_" /D "HTML_VIEW_" /D "PSPOT_TABLE_MODULE_" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /map /debug /machine:I386 /out:"table.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Poker - Win32 Release"
# Name "Poker - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\shell\Poker.cpp
# End Source File
# Begin Source File

SOURCE=.\shell\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\chatdlg.h
# End Source File
# Begin Source File

SOURCE=.\chatframe.h
# End Source File
# Begin Source File

SOURCE=.\chatview.h
# End Source File
# Begin Source File

SOURCE=.\floormapview.h
# End Source File
# Begin Source File

SOURCE=.\inputdlg.h
# End Source File
# Begin Source File

SOURCE=.\mainwnd.h
# End Source File
# Begin Source File

SOURCE=.\motddlg1.h
# End Source File
# Begin Source File

SOURCE=.\motdframe.h
# End Source File
# Begin Source File

SOURCE=.\motdview.h
# End Source File
# Begin Source File

SOURCE=.\Poker.h
# End Source File
# Begin Source File

SOURCE=.\PokerDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tableview.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\arrowdow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrowdown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrowlef.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrowleft.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrowrig.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrowright.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrowup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrowup1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\back.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cards.bmp
# End Source File
# Begin Source File

SOURCE=.\res\check.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checkmask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\corner4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cornerma.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dlr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dlrmask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Poker.ico
# End Source File
# Begin Source File

SOURCE=.\res\Poker.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Cashreg.wav
# End Source File
# Begin Source File

SOURCE=.\res\Chimes.wav
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
