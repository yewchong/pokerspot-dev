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
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
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
# ADD CPP /nologo /MD /W3 /GX /Zi /Ob1 /I "." /I "..\..\lib\openssl\inc32" /I "..\..\lib\common\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "PCLIENT_MODULE_" /D "HTML_VIEW_" /D "PSPOT_TABLE_MODULE_" /D "NO_TABLEIMPL_DLL_" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /map /debug /machine:I386 /out:"holdem.exe"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "..\lib\openssl\inc32" /I "..\lib\common\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "PCLIENT_MODULE_" /D "HTML_VIEW_" /D "PSPOT_TABLE_MODULE_" /D "NO_TABLEIMPL_DLL_" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /map /debug /machine:I386 /out:"holdem.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Poker - Win32 Release"
# Name "Poker - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "TexasHoldem"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\tem\community.cpp
# End Source File
# Begin Source File

SOURCE=.\tem\house.cpp
# End Source File
# Begin Source File

SOURCE=.\tem\remoteplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\tem\statebet.cpp
# End Source File
# Begin Source File

SOURCE=.\tem\statedealpocket.cpp
# End Source File
# Begin Source File

SOURCE=.\tem\statepromptpostblind.cpp
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\network\errorlog.cpp
# End Source File
# Begin Source File

SOURCE=.\network\network.cpp
# End Source File
# Begin Source File

SOURCE=.\network\networkbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduaccountbalance.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduaction.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduactionecho.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduactionrequest.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduactionrequestspreadlimit.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduallin.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduannounce.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduanteallin.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdublindcard.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdublindrequest.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdubutton.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduchat.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pducommunitycard.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pducreditcardedit.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pducreditcardresponse.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdudealcards.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdudealcardsflex.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdueditrules.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduerror.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdugamenumber.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduhandlogrequest.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduloginstats.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdulounge.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdunetworkerror.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdunotify.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduping.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduplayerfinished.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduplayerinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdupocketcard.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduqueuefull.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduqueueupdate.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdureseat.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdusetup.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdushowcardsquery.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdushowdown.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdusitinquery.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pdutournamentparams.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduupgraderequest.cpp
# End Source File
# Begin Source File

SOURCE=.\network\pduverbosetablereply.cpp
# End Source File
# Begin Source File

SOURCE=.\network\server.cpp
# End Source File
# Begin Source File

SOURCE=.\network\ssl.cpp
# End Source File
# End Group
# Begin Group "Base"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\base\actionimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\base\base_registry.cpp
# End Source File
# Begin Source File

SOURCE=.\base\cards.cpp
# End Source File
# Begin Source File

SOURCE=.\base\cardsanimate.cpp
# End Source File
# Begin Source File

SOURCE=.\base\chipsanimate.cpp
# End Source File
# Begin Source File

SOURCE=.\base\cipher.cpp
# End Source File
# Begin Source File

SOURCE=.\base\draw.cpp
# End Source File
# Begin Source File

SOURCE=.\base\gamelogic.cpp
# End Source File
# Begin Source File

SOURCE=.\base\gamestate.cpp
# End Source File
# Begin Source File

SOURCE=.\base\graphic_chips.cpp
# End Source File
# Begin Source File

SOURCE=.\base\jpgimage.cpp
# End Source File
# Begin Source File

SOURCE=.\base\layout.cpp
# End Source File
# Begin Source File

SOURCE=.\base\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\base\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\base\player.cpp
# End Source File
# Begin Source File

SOURCE=.\base\stateante.cpp
# End Source File
# Begin Source File

SOURCE=.\base\statedealcommunity.cpp
# End Source File
# Begin Source File

SOURCE=.\base\statedealpocket2.cpp
# End Source File
# Begin Source File

SOURCE=.\base\statedealupcard.cpp
# End Source File
# Begin Source File

SOURCE=.\base\stateshowdown.cpp
# End Source File
# Begin Source File

SOURCE=.\base\statewait.cpp
# End Source File
# Begin Source File

SOURCE=.\base\sync.cpp
# End Source File
# Begin Source File

SOURCE=.\base\transparentbitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\base\turnindicator.cpp
# End Source File
# Begin Source File

SOURCE=.\base\winsys.cpp
# End Source File
# End Group
# Begin Group "Dib"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\dib\CDib.cpp
# End Source File
# Begin Source File

SOURCE=".\dib\Dib File Handler Targa.cpp"
# End Source File
# Begin Source File

SOURCE=".\dib\Dib File Handler.cpp"
# End Source File
# End Group
# Begin Group "ui"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\ui\aboutdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\actionbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\ActionButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\animatewinner.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\buyinquerydlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\chatdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\ChatEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\chatframe.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\ChatView.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\checkbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\commentdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\global.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\inputdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\logindlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\maildoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\mainwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\optionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\ownerdrawmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\promptinputdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\soundmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\splashwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\tableview.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\waitdlg.cpp
# End Source File
# End Group
# Begin Group "md5"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=.\md5\md5c.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\lib\COMMON\src\chips.cpp

!IF  "$(CFG)" == "Poker - Win32 Release"

!ELSEIF  "$(CFG)" == "Poker - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\lib\COMMON\src\stringoutput.cpp

!IF  "$(CFG)" == "Poker - Win32 Release"

!ELSEIF  "$(CFG)" == "Poker - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\shell\Poker.cpp
# End Source File
# Begin Source File

SOURCE=.\Poker.rc
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

SOURCE=.\ChatPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\chatview.h
# End Source File
# Begin Source File

SOURCE=.\commentdialog.h
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

SOURCE=.\optionsdlg.h
# End Source File
# Begin Source File

SOURCE=.\Poker.h
# End Source File
# Begin Source File

SOURCE=.\PokerDlg.h
# End Source File
# Begin Source File

SOURCE=.\promptinputdlg.h
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

SOURCE=.\res\checkbtn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checkbtnmask.bmp
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

SOURCE=.\res\dollarimage.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dollarmask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\invalidcard.bmp
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

SOURCE=.\res\chips_betting.wav
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\your_turn.wav
# End Source File
# End Target
# End Project
