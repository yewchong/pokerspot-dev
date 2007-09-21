# Microsoft Developer Studio Project File - Name="Load Balancer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Load Balancer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Load Balancer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Load Balancer.mak" CFG="Load Balancer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Load Balancer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Load Balancer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Poker/Server/Load Balancer", SJMAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Load Balancer - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "..\..\lib\common\include" /I "..\..\lib\openssl\inc32" /I "../../lib/sgi_stl" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386 /out:"loadbalancer.exe"

!ELSEIF  "$(CFG)" == "Load Balancer - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "..\..\lib\common\include" /I "..\..\lib\openssl\inc32" /I "../../lib/sgi_stl" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /out:"loadbalancer.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Load Balancer - Win32 Release"
# Name "Load Balancer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "common"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\..\lib\common\src\errorlog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\common\src\filelogger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\common\src\log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\common\src\ssl_funcs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\common\src\stringoutput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\common\src\sync.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lib\common\src\sys.cpp
# End Source File
# End Group
# Begin Group "loadbalancer"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\loadbalancer\loadbalancer.cpp
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\lounge.cpp
# End Source File
# End Group
# Begin Group "pdu"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\pdu\pdu.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduallocator.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloungeinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloungelogin.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloungestats.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdunotify.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerhandshake.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerloggedin.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerloggedout.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduqueueupdate.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduredirectlogin.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdurouteloungepdu.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pdurouteplayerpdu.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduunknown.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduwaitinglistconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\pdu\pduwaitinglistevent.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\accepter.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\poller.cpp
# End Source File
# Begin Source File

SOURCE=.\server.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\accepter.h
# End Source File
# Begin Source File

SOURCE=.\def.h
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\loadbalancer.h
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\lounge.h
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\loungeiterator.h
# End Source File
# Begin Source File

SOURCE=.\loadbalancer\loungeptr.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pdu.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduallocator.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloungeinfo.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloungelogin.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduloungestats.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pdunotify.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerhandshake.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerloggedin.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduplayerloggedout.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduqueueupdate.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduredirectlogin.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pdurouteloungepdu.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pdurouteplayerpdu.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduunknown.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduwaitinglistconfig.h
# End Source File
# Begin Source File

SOURCE=.\pdu\pduwaitinglistevent.h
# End Source File
# Begin Source File

SOURCE=.\poller.h
# End Source File
# Begin Source File

SOURCE=.\server.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
