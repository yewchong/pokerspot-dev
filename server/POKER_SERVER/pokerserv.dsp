# Microsoft Developer Studio Project File - Name="pokerserv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=pokerserv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pokerserv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pokerserv.mak" CFG="pokerserv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pokerserv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "pokerserv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Poker/Server/Table Server", IVGAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pokerserv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\mysql\include" /I "." /I "..\poker-eval\include" /I "..\..\lib\openssl\inc32" /I "..\..\lib\common\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "PSPOT_TABLESERVER_MODULE_" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 libpoker.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"\mysql\lib\opt" /libpath:"..\..\lib\pthread\lib" /libpath:"..\..\lib\openssl\out32" /libpath:"..\poker-eval\lib"

!ELSEIF  "$(CFG)" == "pokerserv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "\mysql\include" /I "." /I "..\poker-eval\include" /I "..\..\lib\openssl\inc32" /I "..\..\lib\common\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "PSPOT_TABLESERVER_MODULE_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libmysql.lib libpoker.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"pokerserv.exe" /pdbtype:sept /libpath:"\mysql\lib\opt" /libpath:"..\..\lib\pthread\lib" /libpath:"..\..\lib\openssl\out32" /libpath:"..\poker-eval\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "pokerserv - Win32 Release"
# Name "pokerserv - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "table"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\table\table.cpp
# End Source File
# Begin Source File

SOURCE=.\table\table.h
# End Source File
# End Group
# Begin Group "player"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\player\pingcounter.cpp
# End Source File
# Begin Source File

SOURCE=.\player\player.cpp
# End Source File
# Begin Source File

SOURCE=.\player\player.h
# End Source File
# End Group
# Begin Group "deck"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\deck\card.cpp
# End Source File
# Begin Source File

SOURCE=.\deck\card.h
# End Source File
# Begin Source File

SOURCE=.\deck\deck.cpp
# End Source File
# Begin Source File

SOURCE=.\deck\deck.h
# End Source File
# End Group
# Begin Group "poller"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\poller\poller.cpp
# End Source File
# Begin Source File

SOURCE=.\poller\poller.h
# End Source File
# End Group
# Begin Group "pduHandler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduHandler\pduHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\pduHandler\pduHandler.h
# End Source File
# Begin Source File

SOURCE=.\pduHandler\pduunknown.cpp
# End Source File
# End Group
# Begin Group "pduLogin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduLogin\pduLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\pduLogin\pduLogin.h
# End Source File
# Begin Source File

SOURCE=.\pduLogin\pduloginreject.cpp
# End Source File
# Begin Source File

SOURCE=.\pdulogin\pduobserverlogin.cpp
# End Source File
# End Group
# Begin Group "pduChat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduChat\pduChat.cpp
# End Source File
# Begin Source File

SOURCE=.\pduChat\pduChat.h
# End Source File
# End Group
# Begin Group "ring"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ring\ring.cpp
# End Source File
# Begin Source File

SOURCE=.\ring\ring.h
# End Source File
# End Group
# Begin Group "dbHandler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dbHandler\dbHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\dbHandler\dbHandler.h
# End Source File
# End Group
# Begin Group "pduPlayerInfo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduPlayerInfo\pduPlayerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlayerInfo\pduPlayerInfo.h
# End Source File
# End Group
# Begin Group "pduBuyinRequest"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduBuyinRequest\pduBuyinRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\pduBuyinRequest\pduBuyinRequest.h
# End Source File
# End Group
# Begin Group "pduPlayer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduPlayer\pduplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlayer\pduplayer.h
# End Source File
# End Group
# Begin Group "pduLogout"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduLogout\pdulogout.cpp
# End Source File
# Begin Source File

SOURCE=.\pduLogout\pdulogout.h
# End Source File
# Begin Source File

SOURCE=.\pdulogout\pduobserverlogout.cpp
# End Source File
# End Group
# Begin Group "pdulogoutconfirm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduLogoutConfirm\pdulogoutconfirm.cpp
# End Source File
# Begin Source File

SOURCE=.\pduLogoutConfirm\pdulogoutconfirm.h
# End Source File
# End Group
# Begin Group "pduPlay"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduPlay\pduaction.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduaction.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduactionecho.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduactionecho.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduactionrequest.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduactionrequest.h
# End Source File
# Begin Source File

SOURCE=.\pduplay\pduactionrequestspreadlimit.cpp
# End Source File
# Begin Source File

SOURCE=.\pduplay\pduactionspreadlimit.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduallin.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduallin.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduannounce.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pduannounce.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdublindrequest.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdublindrequest.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdubutton.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdubutton.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdudealcards.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdudealcards.h
# End Source File
# Begin Source File

SOURCE=.\pduplay\pdudealcardsflex.cpp
# End Source File
# Begin Source File

SOURCE=.\pduplay\pdudealcardsflex.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdudealcommunity.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdudealcommunity.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdugamenumber.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdugamenumber.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdupostblind.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdupostblind.h
# End Source File
# Begin Source File

SOURCE=.\pduplay\pdurefuseblind.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdushowcards.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdushowcards.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdushowdown.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdushowdown.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdusitin.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdusitin.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdusitinquery.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdusitinquery.h
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdusitout.cpp
# End Source File
# Begin Source File

SOURCE=.\pduPlay\pdusitout.h
# End Source File
# End Group
# Begin Group "pduBuyinQuery"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pdubuyinquery\pdubuyinmore.cpp
# End Source File
# Begin Source File

SOURCE=.\pduBuyinQuery\pduBuyinQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\pduBuyinQuery\pduBuyinQuery.h
# End Source File
# End Group
# Begin Group "sync"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sync\sync.cpp
# End Source File
# Begin Source File

SOURCE=.\sync\sync.h
# End Source File
# End Group
# Begin Group "plm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\plm\plm.cpp
# End Source File
# Begin Source File

SOURCE=.\plm\plm.h
# End Source File
# Begin Source File

SOURCE=.\plm\plm_allin.cpp
# End Source File
# Begin Source File

SOURCE=.\plm\plm_ante.cpp
# End Source File
# Begin Source File

SOURCE=.\plm\plm_bet.cpp
# End Source File
# Begin Source File

SOURCE=.\plm\plm_cleanup.cpp
# End Source File
# Begin Source File

SOURCE=.\plm\plm_deal.cpp
# End Source File
# Begin Source File

SOURCE=.\plm\plm_rake.cpp
# End Source File
# Begin Source File

SOURCE=.\plm\plm_rebuildring.cpp
# End Source File
# Begin Source File

SOURCE=.\plm\plm_showdown.cpp
# End Source File
# End Group
# Begin Group "pgl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pgl\pgl.cpp
# End Source File
# Begin Source File

SOURCE=.\pgl\pgl.h
# End Source File
# End Group
# Begin Group "dbInterface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dbInterface\dbInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\dbInterface\dbInterface.h
# End Source File
# Begin Source File

SOURCE=.\dbinterface\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\dbinterface\md5.h
# End Source File
# End Group
# Begin Group "pot"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pot\pot.cpp
# End Source File
# Begin Source File

SOURCE=.\pot\pot.h
# End Source File
# End Group
# Begin Group "hand"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hand\hand.cpp
# End Source File
# Begin Source File

SOURCE=.\hand\hand.h
# End Source File
# End Group
# Begin Group "lounge"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lounge\loungeserver.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pdubroadtablequery.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pdubroadtablereply.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pduplayerloginquery.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pduplayerloginreply.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pduplayerobservequery.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pduplayerobservereply.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pdutablelogin.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pdutablelogout.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\pdutableupdate.cpp
# End Source File
# End Group
# Begin Group "pdunotify"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pdunotify\pdunotify.cpp
# End Source File
# End Group
# Begin Group "pduHandlogRequest"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pduhandlogrequest\pduhandlogrequest.cpp
# End Source File
# End Group
# Begin Group "pdupingpong"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pdupingpong\pduheartbeat.cpp
# End Source File
# Begin Source File

SOURCE=.\pdupingpong\pduping.cpp
# End Source File
# Begin Source File

SOURCE=.\pdupingpong\pdupong.cpp
# End Source File
# End Group
# Begin Group "plm_7stud"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\plm_7stud\plm_7stud.cpp
# End Source File
# Begin Source File

SOURCE=.\plm_7stud\plm_7stud_ante.cpp
# End Source File
# Begin Source File

SOURCE=.\plm_7stud\plm_7stud_bet.cpp
# End Source File
# End Group
# Begin Group "tournament"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tournament\pduanteallin.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pduclientreseat.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdudissolveinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdudissolvetable.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pduplayerfinished.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pduplayerseated.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pduplayerunseated.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdureseatack.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdureseatplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pduseatplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdusynchronize.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdutournamentparams.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\tournament.cpp
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\common\src\chips.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\common\src\stringoutput.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\accepter.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain.cpp
# End Source File
# Begin Source File

SOURCE=.\ipcheck.cpp
# End Source File
# Begin Source File

SOURCE=.\pinger.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmemory.c
# End Source File
# Begin Source File

SOURCE=.\pokerserv.cpp
# End Source File
# Begin Source File

SOURCE=.\psi_assert.cpp
# End Source File
# Begin Source File

SOURCE=.\psi_win32.c
# End Source File
# Begin Source File

SOURCE=.\random_generator.cpp
# End Source File
# Begin Source File

SOURCE=.\ssl_funcs.cpp
# End Source File
# Begin Source File

SOURCE=.\sys.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\database.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\gettime.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\includes.h
# End Source File
# Begin Source File

SOURCE=.\player\includes.h
# End Source File
# Begin Source File

SOURCE=.\pdutypes.h
# End Source File
# Begin Source File

SOURCE=.\Pmemory.h
# End Source File
# Begin Source File

SOURCE=.\pokerserv.h
# End Source File
# Begin Source File

SOURCE=.\psi_win32.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
