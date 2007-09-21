# Microsoft Developer Studio Project File - Name="Lounge" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Lounge - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lounge.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lounge.mak" CFG="Lounge - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lounge - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Lounge - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lounge - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I ".." /I "." /I "..\..\..\lib\openssl\inc32" /I "..\..\..\lib\common\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "PCLIENT_MODULE_" /D "HTML_VIEW_" /D "LOUNGE_DLG_" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40b /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"../lounge.exe"
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "Lounge - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "." /I "..\..\lib\openssl\inc32" /I "..\..\lib\common\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "PCLIENT_MODULE_" /D "HTML_VIEW_" /D "LOUNGE_DLG_" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40b /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"../lounge.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Lounge - Win32 Release"
# Name "Lounge - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ui"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\ui\aboutdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\cashierdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\chatdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\ChatEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\chatframe.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\ChatView.cpp
# End Source File
# Begin Source File

SOURCE=.\creditdarddlg.cpp
# End Source File
# Begin Source File

SOURCE=.\editordlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Filler.cpp
# End Source File
# Begin Source File

SOURCE=..\lounge\floormapview.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\global.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\logindlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\motddlg.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\motdframe.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\motdview.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgInd.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\soundmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\ui\splashwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\tabledetailsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\transactiondlg.cpp
# End Source File
# End Group
# Begin Group "network"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\network\errorlog.cpp
# End Source File
# Begin Source File

SOURCE=..\network\network.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduaccountbalance.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduactionrequestspreadlimit.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduanteallin.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduchat.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduchatenable.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pducreditcardedit.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pducreditcardreply2.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pducreditcardresponse.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pducreditcardtypes.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduerror.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduloginstats.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pdulounge.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduloungebuyinresponse.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduloungecashoutresponse.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduloungegamenames.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduloungehandshake.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduloungetransactionreply.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pdunetworkerror.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pdunotify.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduping.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduplayerfinished.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduqueuefull.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduqueueupdate.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduredirectlogin.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pdureseat.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pdutournamentdetailsreply.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pdutournamentparams.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduupgraderequest.cpp
# End Source File
# Begin Source File

SOURCE=..\network\pduverbosetablereply.cpp
# End Source File
# Begin Source File

SOURCE=..\network\server.cpp
# End Source File
# Begin Source File

SOURCE=..\network\ssl.cpp
# End Source File
# End Group
# Begin Group "base"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\base\autoupgrade.cpp
# End Source File
# Begin Source File

SOURCE=..\base\base_registry.cpp
# End Source File
# Begin Source File

SOURCE=..\dib\CDib.cpp
# End Source File
# Begin Source File

SOURCE=..\base\cipher.cpp
# End Source File
# Begin Source File

SOURCE="..\dib\Dib File Handler Targa.cpp"
# End Source File
# Begin Source File

SOURCE="..\dib\Dib File Handler.cpp"
# End Source File
# Begin Source File

SOURCE=..\base\draw.cpp
# End Source File
# Begin Source File

SOURCE=..\base\gamelogic.cpp
# End Source File
# Begin Source File

SOURCE=..\base\jpgimage.cpp
# End Source File
# Begin Source File

SOURCE=..\base\md5.cpp
# End Source File
# Begin Source File

SOURCE=..\base\misc.cpp
# End Source File
# Begin Source File

SOURCE=..\base\transparentbitmap.cpp
# End Source File
# End Group
# Begin Group "md5"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=..\md5\md5c.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\..\lib\COMMON\src\chips.cpp

!IF  "$(CFG)" == "Lounge - Win32 Release"

!ELSEIF  "$(CFG)" == "Lounge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\lib\COMMON\src\stringoutput.cpp

!IF  "$(CFG)" == "Lounge - Win32 Release"

!ELSEIF  "$(CFG)" == "Lounge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\banner.cpp
# End Source File
# Begin Source File

SOURCE=.\floormap.cpp
# End Source File
# Begin Source File

SOURCE=.\listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Lounge.cpp
# End Source File
# Begin Source File

SOURCE=.\Lounge.rc
# End Source File
# Begin Source File

SOURCE=.\loungedata.cpp
# End Source File
# Begin Source File

SOURCE=.\LoungeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\tabctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\tournamentdetailsdlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cashierdlg.h
# End Source File
# Begin Source File

SOURCE=.\creditcarddlg.h
# End Source File
# Begin Source File

SOURCE=.\editordlg.h
# End Source File
# Begin Source File

SOURCE=.\Lounge.h
# End Source File
# Begin Source File

SOURCE=.\LoungeDlg.h
# End Source File
# Begin Source File

SOURCE=.\pduloungecashoutresponse.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tabledetailsdlg.h
# End Source File
# Begin Source File

SOURCE=.\transactiondlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Lounge.ico
# End Source File
# Begin Source File

SOURCE=.\res\Lounge.rc2
# End Source File
# Begin Source File

SOURCE=.\res\radio1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\radio_on.bmp
# End Source File
# Begin Source File

SOURCE=.\res\radiobtn.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
