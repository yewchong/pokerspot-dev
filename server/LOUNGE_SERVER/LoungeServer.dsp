# Microsoft Developer Studio Project File - Name="LoungeServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=LoungeServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LoungeServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LoungeServer.mak" CFG="LoungeServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LoungeServer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "LoungeServer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LoungeServer - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\common\include" /I "." /I "..\..\lib\sgi_stl" /I "\mysql\include" /I "..\..\lib\openssl\inc32" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 libmysql.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"LoungeServer.exe" /libpath:"..\..\lib\pthread\lib" /libpath:"..\..\lib\openssl\out32" /libpath:"\mysql\lib\opt"

!ELSEIF  "$(CFG)" == "LoungeServer - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\lib\common\include" /I "." /I "..\..\lib\sgi_stl" /I "C:\mysql\include" /I "..\..\lib\openssl\inc32" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40b /d "_DEBUG"
# ADD RSC /l 0x40b /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libmysql.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"LoungeServer.exe" /pdbtype:sept /libpath:"..\..\lib\pthread\lib" /libpath:"..\..\lib\openssl\out32" /libpath:"\mysql\lib\opt"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LoungeServer - Win32 Release"
# Name "LoungeServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "pdu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pdu\pdu.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduaccountbalance.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduallocator.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdubalancequery.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdubroadtablequery.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdubroadtablereply.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdubuyinreply.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdubuyinrequest.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducancelsitdown.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducashoutreply.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducashoutrequest.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduchat.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduchatenable.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducreditcardedit.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducreditcardquery.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducreditcardquery2.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducreditcardreply.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducreditcardreply2.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pducreditcardtypes.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdulogin.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloginaccept.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloginreject.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdulogout.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdulogoutconfirmed.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloungestats.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdunotify.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduobserve.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduobserving.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduping.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerloginquery.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerloginreply.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerobservequery.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerobservereply.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdupong.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduqueuefull.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduqueueupdate.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdusitdown.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdusitting.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduspawntable.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdustatus.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdusurvey.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdutablelogin.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdutablelogout.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdutableupdate.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdutransactionquery.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdutransactionresponse.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduunknown.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduupgraderequest.cpp
# End Source File
# End Group
# Begin Group "lounge"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lounge\game.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\lounge.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\player.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\queueservice.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\table.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\tablemaster.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\waitinglist.cpp
# End Source File
# End Group
# Begin Group "dbhandler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dbhandler\dbhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\dbhandler\dbinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\dbhandler\md5.cpp
# End Source File
# End Group
# Begin Group "upgrade"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\upgrade\autoupgrade.cpp
# End Source File
# Begin Source File

SOURCE=.\upgrade\compute_md5_checksum.cpp
# End Source File
# Begin Source File

SOURCE=.\upgrade\md5c.c
# End Source File
# End Group
# Begin Group "tournament"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\tournament\action.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\basetournamentqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\endstate.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\event.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdudissolveinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdudissolvetable.cpp
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

SOURCE=.\tournament\pdureseatnack.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdureseatplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdusaveformat.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdusaverules.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pduseatplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdusynchronize.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdutournamentdetailsquery.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdutournamentdetailsreply.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\pdutournamentparams.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\runstate.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\runstateunfrozen.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\satellitequeue.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\seater.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\startstate.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\tournament.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\tournamentqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\tournamentstate.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\waitnplayersstate.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\waitpause.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\waitpredicate.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\waitseated.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\waitstartstate.cpp
# End Source File
# Begin Source File

SOURCE=.\tournament\waitstate.cpp
# End Source File
# End Group
# Begin Group "sync"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\sync\sync.cpp
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\..\lib\common\src\chips.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\COMMON\src\mssqlimp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\common\src\stringoutput.cpp
# End Source File
# End Group
# Begin Group "loadbalancer"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\loadbalancer\loadbalancer.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\loungeproxy.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pduloungeinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pduloungelogin.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pduloungelogout.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pduplayerloggedin.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pduplayerloggedout.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pdurouteloungepdu.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pdurouteplayerpdu.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pduwaitinglistconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\pduwaitinglistevent.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\playerproxy.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\tableproxy.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\accepter.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\pingcounter.cpp
# End Source File
# Begin Source File

SOURCE=.\pinger.cpp
# End Source File
# Begin Source File

SOURCE=.\poller.cpp
# End Source File
# Begin Source File

SOURCE=.\server.cpp
# End Source File
# Begin Source File

SOURCE=.\ssl_funcs.cpp
# End Source File
# Begin Source File

SOURCE=.\sys.cpp
# End Source File
# Begin Source File

SOURCE=.\lounge\tester.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
