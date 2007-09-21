; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFloorMapView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "lounge.h"
LastPage=0

ClassCount=4
Class1=CFloorMapView
Class2=CMainWnd
Class3=CPokerApp

ResourceCount=11
Resource1=IDD_DIALOG1
Resource2=IDM_FLOORMAPVIEW
Resource3=IDD_MOTD
Resource4=IDD_LOGGINGIN
Resource5=IDD_LOGIN
Resource6=IDD_PDU
Resource7=IDR_MAINFRAME (English (U.S.))
Resource8=IDD_CHAT
Resource9=IDD_ACTIONBAR (English (U.S.))
Resource10=IDD_ABOUTBOX (English (U.S.))
Class4=CLoungeDlg
Resource11=IDR_ACCELERATOR1

[CLS:CFloorMapView]
Type=0
BaseClass=CView
HeaderFile=floormapview.h
ImplementationFile=floormapview.cpp
Filter=C
VirtualFilter=VWC
LastObject=CFloorMapView

[CLS:CMainWnd]
Type=0
BaseClass=CFrameWnd
HeaderFile=mainwnd.h
ImplementationFile=mainwnd.cpp

[CLS:CPokerApp]
Type=0
BaseClass=CWinApp
HeaderFile=poker.h
ImplementationFile=poker.cpp

[MNU:IDM_FLOORMAPVIEW]
Type=1
Class=?
Command1=ID_LOGIN
Command2=ID_JOIN_TABLE
Command3=ID_LOGOUT
Command4=ID_APP_ABOUT
Command5=ID_APP_EXIT
Command6=ID_FAKE_PDULOUNGEACCEPT
Command7=ID_FAKE_PDUSTATUS
Command8=ID_FAKE_PDUSITTING
Command9=ID_FAKE_PDULOGOUTWARNING
CommandCount=9

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_TITLE,static,1073741953
Control2=IDC_COPYRIGHT,static,1073741825
Control3=IDOK,button,1342373889
Control4=IDB_LOGO,static,1342177294

[DLG:IDD_ACTIONBAR (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_CHECKPOSTBLIND,button,1342242819
Control2=IDC_CHECKSITOUT,button,1342242819
Control3=IDC_CHECKFOLD,button,1342242819
Control4=IDC_STATIC,button,1342177287

[DLG:IDD_LOGIN]
Type=1
Class=?
ControlCount=10
Control1=IDC_USERNAME,edit,1350631552
Control2=IDC_PASSWORD,edit,1350631584
Control3=IDC_EDITHOST,edit,1350631552
Control4=IDC_EDITPORT,edit,1350639744
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[DLG:IDD_MOTD]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_MESSAGEPANE,button,1073741831

[DLG:IDD_LOGGINGIN]
Type=1
Class=?
ControlCount=1
Control1=IDC_PROMPT,static,1342308865

[DLG:IDD_CHAT]
Type=1
Class=?
ControlCount=2
Control1=IDC_EDITCHAT,edit,1350631552
Control2=IDC_TAB1,SysTabControl32,1342177280

[DLG:IDD_PDU]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST1,SysListView32,1350631939

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_APP_ABOUT
Command2=ID_LOGIN
Command3=ID_MOTD
Command4=ID_APP_EXIT
CommandCount=4

[ACL:IDR_ACCELERATOR1]
Type=1
Class=?
Command1=ID_RELOAD_CONFIGURATION
CommandCount=1

[DLG:IDD_DIALOG1]
Type=1
Class=CLoungeDlg
ControlCount=8
Control1=IDC_TAB1,SysTabControl32,1342242816
Control2=IDC_LIST1,SysListView32,1342211073
Control3=IDC_STATIC,static,1342308352
Control4=IDC_USERNAME,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATUS,static,1342308364
Control7=IDC_RADIO1,button,1342177289
Control8=IDC_RADIO2,button,1342177289

[CLS:CLoungeDlg]
Type=0
HeaderFile=loungedlg.h
ImplementationFile=loungedlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CLoungeDlg
VirtualFilter=dWC

