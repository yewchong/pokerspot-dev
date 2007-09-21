; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCommentDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "poker.h"
LastPage=0

ClassCount=24
Class1=CAboutDlg
Class2=CActionBar
Class3=CActionButton
Class4=CBuyInQueryDlg
Class5=CChatDlg
Class6=CChatEdit
Class7=CChatFrame
Class8=CWaitDlg
Class9=CCheckButton
Class10=CFloorMapView
Class11=CLoginDlg
Class12=CMainWnd
Class13=CMotdDlg
Class14=CMotdFrame
Class15=CMotdView
Class16=CPokerApp
Class17=CPokerDlg
Class18=CSendPDUDlg
Class19=CTableView

ResourceCount=21
Resource1=IDD_FLOORMANMSG
Resource2=IDD_ABOUTBOX
Resource3=IDD_ACTIONBAR2 (English (U.S.))
Resource4=IDD_PROMPTINPUT
Resource5=IDD_WAITING
Resource6=IDD_ABOUTBOX (English (U.S.))
Resource7=IDD_INPUT (English (U.S.))
Resource8=IDD_ACTIONBAR (English (U.S.))
Resource9=IDD_INPUT1 (English (U.S.))
Resource10=IDD_MOTD
Resource11=IDM_TABLEVIEW_DEBUG
Resource12=IDD_POKER_DIALOG (English (U.S.))
Resource13=IDR_MAINFRAME (English (U.S.))
Class20=CChatPropertiesDlg
Resource14=IDM_POPUP
Resource15=IDD_BUYINQUERY
Class21=COptionsDlg
Class22=CPromptInputDlg
Resource16=IDD_OPTIONS
Resource17=IDD_LOGIN
Resource18=IDD_EDITCOMMENT
Class23=CInputDlg
Resource19=IDD_CHAT
Resource20=IDM_TABLEVIEW
Class24=CCommentDialog
Resource21=IDR_ACCELERATOR1

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=aboutdlg.h
ImplementationFile=aboutdlg.cpp
LastObject=CAboutDlg

[CLS:CActionBar]
Type=0
HeaderFile=ui\actionbar.h
ImplementationFile=ui\actionbar.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_ROOT

[CLS:CActionButton]
Type=0
BaseClass=CButton
HeaderFile=ActionButton.h
ImplementationFile=ActionButton.cpp

[CLS:CBuyInQueryDlg]
Type=0
BaseClass=CDialog
HeaderFile=buyinquerydlg.h
ImplementationFile=buyinquerydlg.cpp

[CLS:CChatDlg]
Type=0
HeaderFile=ui\chatdlg.h
ImplementationFile=ui\chatdlg.cpp
BaseClass=CDialog
LastObject=IDC_RADIO_USER
Filter=D
VirtualFilter=dWC

[CLS:CChatEdit]
Type=0
BaseClass=CEdit
HeaderFile=ChatEdit.h
ImplementationFile=ChatEdit.cpp

[CLS:CChatFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=chatframe.h
ImplementationFile=chatframe.cpp

[CLS:CCheckButton]
Type=0
BaseClass=CButton
HeaderFile=checkbutton.h
ImplementationFile=checkbutton.cpp

[CLS:CFloorMapView]
Type=0
BaseClass=CView
HeaderFile=floormapview.h
ImplementationFile=floormapview.cpp

[CLS:CLoginDlg]
Type=0
BaseClass=CDialog
HeaderFile=logindlg.h
ImplementationFile=logindlg.cpp

[CLS:CMainWnd]
Type=0
HeaderFile=ui\mainwnd.h
ImplementationFile=ui\mainwnd.cpp
BaseClass=CFrameWnd
Filter=T
VirtualFilter=fWC
LastObject=CMainWnd

[CLS:CMotdDlg]
Type=0
BaseClass=CDialog
HeaderFile=motddlg.h
ImplementationFile=motddlg.cpp

[CLS:CMotdFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=motdframe.h
ImplementationFile=motdframe.cpp

[CLS:CMotdView]
Type=0
BaseClass=CHtmlView
HeaderFile=motdview.h
ImplementationFile=motdview.cpp

[CLS:CPokerApp]
Type=0
HeaderFile=shell\poker.h
ImplementationFile=shell\poker.cpp
BaseClass=CWinApp
Filter=N
VirtualFilter=AC
LastObject=CPokerApp

[CLS:CPokerDlg]
Type=0
BaseClass=CDialog
HeaderFile=PokerDlg.h
ImplementationFile=PokerDlg.cpp

[CLS:CSendPDUDlg]
Type=0
BaseClass=CDialog
HeaderFile=sendpdudlg.h
ImplementationFile=sendpdudlg.cpp

[CLS:CTableView]
Type=0
HeaderFile=ui\tableview.h
ImplementationFile=ui\tableview.cpp
BaseClass=CView
Filter=C
VirtualFilter=VWC
LastObject=CTableView

[CLS:CWaitDlg]
Type=0
HeaderFile=ui\waitdlg.h
ImplementationFile=ui\waitdlg.cpp
BaseClass=CDialog
LastObject=CWaitDlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_CHAT]
Type=1
Class=CChatDlg
ControlCount=5
Control1=IDC_EDITCHAT,edit,1350631552
Control2=IDC_TAB1,SysTabControl32,1342177280
Control3=IDC_RADIO_ALL,button,1342177289
Control4=IDC_RADIO_DEALER,button,1342177289
Control5=IDC_RADIO_USER,button,1342177289

[DLG:IDD_LOGIN]
Type=1
Class=CLoginDlg
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
Class=CMotdDlg
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_MESSAGEPANE,button,1073741831

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
Class=CActionBar
ControlCount=12
Control1=IDC_CHECKPOSTBLIND,button,1342242819
Control2=IDC_CHECKSITOUT,button,1342242819
Control3=IDC_CHECKFOLD,button,1342242819
Control4=IDC_MUCKHAND,button,1342242819
Control5=IDC_CHECKCALL,button,1342242819
Control6=IDC_CHECKRAISE,button,1342242819
Control7=IDC_STATIC,button,1342177287
Control8=IDC_RAISEAMOUNT,edit,1484857472
Control9=IDC_BTN_PLACEHOLDER1,button,1073741824
Control10=IDC_BTN_PLACEHOLDER2,button,1073741824
Control11=IDC_BTN_PLACEHOLDER3,button,1073741824
Control12=IDC_SLIDER1,msctls_trackbar32,1476460568

[DLG:IDD_INPUT (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_EDIT1,edit,1350639744
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC1,static,1342308352

[DLG:IDD_POKER_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352

[DLG:IDD_INPUT1 (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LOW,button,1342177289
Control4=IDC_HI,button,1342177289

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

[DLG:IDD_BUYINQUERY]
Type=1
Class=?
ControlCount=5
Control1=IDC_CHIPS,edit,1350639744
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1073807360
Control4=IDC_STATIC,static,1342308352
Control5=IDC_PROMPT,static,1342308352

[MNU:IDM_TABLEVIEW]
Type=1
Class=?
Command1=ID_LOGIN
Command2=ID_LOGOUT
Command3=ID_BUYIN
Command4=ID_REQUESTPREVHAND
Command5=ID_REQUEST5HANDLOG
Command6=ID_OPTIONS
Command7=ID_APP_ABOUT
Command8=ID_APP_EXIT
CommandCount=8

[CLS:CChatPropertiesDlg]
Type=0
HeaderFile=ChatPropertiesDlg.h
ImplementationFile=ChatPropertiesDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CChatPropertiesDlg

[DLG:IDD_ACTIONBAR2 (English (U.S.))]
Type=1
Class=CActionBar
ControlCount=12
Control1=IDC_CHECKPOSTBLIND,button,1342242819
Control2=IDC_CHECKSITOUT,button,1342242819
Control3=IDC_CHECKFOLD,button,1342242819
Control4=IDC_MUCKHAND,button,1342242819
Control5=IDC_CHECKCALL,button,1342242819
Control6=IDC_CHECKRAISE,button,1342242819
Control7=IDC_GROUP1,button,1342177287
Control8=IDC_RAISEAMOUNT,edit,1484857472
Control9=IDC_BTN_PLACEHOLDER1,button,1073741824
Control10=IDC_BTN_PLACEHOLDER2,button,1073741824
Control11=IDC_BTN_PLACEHOLDER3,button,1073741824
Control12=IDC_SLIDER1,msctls_trackbar32,1476460568

[DLG:IDD_OPTIONS]
Type=1
Class=COptionsDlg
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDC_SETTINGS_SOUNDS,button,1342242819
Control3=IDC_STATIC,button,1342177287
Control4=IDC_SETTINGS_TTIPS,button,1342242819
Control5=IDC_SETTINGS_HORZACTIONBAR,button,1342242819
Control6=IDC_STATIC,static,1342308352
Control7=IDC_SETTINGS_EMAILADDRESS,edit,1350631552
Control8=IDC_STATIC,button,1342177287
Control9=IDC_TAKEFOCUS,button,1342242819
Control10=IDC_SETTINGS_DOCKACTIONBAR,button,1342242819
Control11=IDC_SETTINGS_DOCKCHATWND,button,1342242819
Control12=IDC_SETTINGS_DOCKCHATLEFT,button,1342242819

[CLS:COptionsDlg]
Type=0
HeaderFile=ui\optionsdlg.h
ImplementationFile=ui\optionsdlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_SETTINGS_DOCKCHATLEFT

[DLG:IDD_PROMPTINPUT]
Type=1
Class=CPromptInputDlg
ControlCount=4
Control1=IDC_EDIT1,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_EDIT2,edit,1342179332

[CLS:CPromptInputDlg]
Type=0
HeaderFile=promptinputdlg.h
ImplementationFile=promptinputdlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CPromptInputDlg
VirtualFilter=dWC

[DLG:IDD_WAITING]
Type=1
Class=CWaitDlg
ControlCount=1
Control1=IDC_PROMPT,static,1342308865

[MNU:IDM_TABLEVIEW_DEBUG]
Type=1
Class=?
Command1=ID_LOGIN
Command2=ID_LOGOUT
Command3=ID_REQUESTPREVHAND
Command4=ID_REQUEST5HANDLOG
Command5=ID_OPTIONS
Command6=ID_APP_ABOUT
Command7=ID_APP_EXIT
Command8=ID_VIEW_BACKGROUND
Command9=ID_RELOAD_CONFIGURATION
Command10=ID_FAKE_PDUBUYINQUERY
Command11=ID_FAKE_PDUPLAYER
Command12=ID_FAKE_PDUBUTTON
Command13=ID_FAKE_PDUBLINDPOST
Command14=ID_FAKE_PDUDEALCARDS
Command15=ID_FAKE_PDUACTIONREQUEST
Command16=ID_FAKE_PDUCOMMUNITYCARD
Command17=ID_FAKE_PDUSHOWDOWN
Command18=ID_FAKE_PDUANNOUNCE
Command19=ID_FAKE_PDULOGOUTCONFIRM
Command20=ID_FAKE_PDUERROR
Command21=ID_SPLITPOT
Command22=ID_FAKEFOLD
CommandCount=22

[DLG:IDD_ABOUTBOX]
Type=1
Class=?
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308481
Control3=IDC_STATIC,static,1342308353
Control4=IDOK,button,1342373889
Control5=IDC_GROUPBOX_VERSION,button,1342177287
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_FLOORMANMSG]
Type=1
Class=CInputDlg
ControlCount=4
Control1=IDC_EDIT_NOTE,edit,1350635588
Control2=IDOK,button,1342242816
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308352

[CLS:CInputDlg]
Type=0
HeaderFile=ui\inputdlg.h
ImplementationFile=ui\inputdlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CInputDlg

[MNU:IDM_POPUP]
Type=1
Class=?
Command1=ID_EDITCOMMENT
CommandCount=1

[DLG:IDD_EDITCOMMENT]
Type=1
Class=CCommentDialog
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT1,edit,1350635652

[CLS:CCommentDialog]
Type=0
HeaderFile=ui\commentdialog.h
ImplementationFile=ui\commentdialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CCommentDialog

