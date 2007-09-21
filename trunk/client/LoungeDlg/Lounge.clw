; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCreditCardDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Lounge.h"

ClassCount=7
Class1=CLoungeApp
Class2=CLoungeDlg
Class3=CAboutDlg

ResourceCount=15
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_LOUNGE_DIALOG
Resource4=IDD_MOTD (English (U.S.))
Resource5=IDD_CHAT (English (U.S.))
Resource6=IDD_EDITOR
Resource7=IDD_TABLEDETAILS
Resource8=IDD_CASHIER
Class4=CTableDetailsDlg
Resource9=IDD_LOGIN1 (English (U.S.))
Class5=CCashierDlg
Resource10=IDD_LOGIN (English (U.S.))
Resource11=IDD_TRANSACTION
Class6=CTransactionDlg
Resource12=IDD_LOUNGE_DIALOG (English (U.S.))
Resource13=IDD_TOURNAMENTDETAILS
Resource14=IDD_ABOUTBOX (English (U.S.))
Class7=CCreditCardDlg
Resource15=IDD_CREDITCARDS

[CLS:CLoungeApp]
Type=0
HeaderFile=Lounge.h
ImplementationFile=Lounge.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CLoungeApp

[CLS:CLoungeDlg]
Type=0
HeaderFile=LoungeDlg.h
ImplementationFile=LoungeDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CLoungeDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=LoungeDlg.h
ImplementationFile=LoungeDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_LOUNGE_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CLoungeDlg

[DLG:IDD_LOUNGE_DIALOG (English (U.S.))]
Type=1
Class=CLoungeDlg
ControlCount=16
Control1=IDC_LD_TAB1,SysTabControl32,1342308864
Control2=IDC_LD_LIST1,SysListView32,1342211073
Control3=IDC_PROMPT_USERNAME,static,1073872896
Control4=IDC_PROMPT_STATUS,static,1073872896
Control5=IDC_LD_RADIO1,button,1073741833
Control6=IDC_LD_RADIO2,button,1073741961
Control7=IDC_PROMPT_NUMTABLES,static,1073872896
Control8=IDC_PROMPT_NUMPLAYERS,static,1073872896
Control9=IDC_CASHIER,button,1342242816
Control10=IDC_LOGIN,button,1342242816
Control11=IDC_LOUNGE_GROUP1,button,1073741831
Control12=IDC_LD_STATUS,edit,1350568064
Control13=IDC_LD_NAME,edit,1350568064
Control14=IDC_LD_NUMPLAYERS,edit,1350568064
Control15=IDC_LD_NUMTABLES,edit,1350568064
Control16=IDC_EDIT_BANNER,edit,1073744000

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308481
Control3=IDC_STATIC,static,1342308353
Control4=IDOK,button,1342373889
Control5=IDC_GROUPBOX_VERSION,button,1342177287
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_LOGIN (English (U.S.))]
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

[DLG:IDD_MOTD (English (U.S.))]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_MESSAGEPANE,button,1073741831

[DLG:IDD_CHAT (English (U.S.))]
Type=1
Class=?
ControlCount=3
Control1=IDC_EDITCHAT,edit,1350631552
Control2=IDC_TAB1,SysTabControl32,1342177280
Control3=IDC_CHECK_SYSTEMONLY,button,1342242819

[CLS:CTableDetailsDlg]
Type=0
HeaderFile=tabledetailsdlg.h
ImplementationFile=tabledetailsdlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CTableDetailsDlg
VirtualFilter=dWC

[DLG:IDD_TABLEDETAILS]
Type=1
Class=CTableDetailsDlg
ControlCount=8
Control1=IDC_STATIC,static,1342308352
Control2=IDC_TD_LIST1,SysListView32,1350664193
Control3=IDC_TD_AVGPOT,edit,1350633600
Control4=IDC_FLOPPRCNT_PROMPT,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_TD_FLOPPRCT,edit,1350633600
Control7=IDC_TD_NUMPLAYERS,edit,1350633600
Control8=IDC_OBSERVE,button,1342242816

[CLS:CCashierDlg]
Type=0
HeaderFile=cashierdlg.h
ImplementationFile=cashierdlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CREDITCARDS
VirtualFilter=dWC

[DLG:IDD_CASHIER]
Type=1
Class=CCashierDlg
ControlCount=17
Control1=IDC_BUYIN,button,1342242816
Control2=IDC_CREDITCARD,combobox,1478557699
Control3=IDC_BUYINAMOUNT,edit,1484857472
Control4=IDC_CASHOUT,button,1342242816
Control5=IDC_CASHOUTAMOUNT,edit,1484857472
Control6=IDC_COMMIT,button,1476460544
Control7=IDC_EDIT_COMMIT,edit,1350633600
Control8=IDOK,button,1342242817
Control9=IDC_PROMPT_CREDITCARD,static,1073872896
Control10=IDC_PROMPT_BUYINAMOUNT,static,1073872896
Control11=IDC_CASHIER_GROUP1,button,1073741831
Control12=IDC_CASHIER_GROUP2,button,1073741831
Control13=IDC_PROMPT_CASHOUTAMOUNT,static,1073872896
Control14=IDC_CASHIER_GROUP3,button,1073741831
Control15=IDC_CASHIER_GROUP4,button,1073741831
Control16=IDC_PROMPT_ACTION,static,1073872896
Control17=IDC_CREDITCARDS,button,1342242816

[DLG:IDD_LOGIN1 (English (U.S.))]
Type=1
Class=?
ControlCount=13
Control1=IDC_USERNAME,edit,1350631552
Control2=IDC_PASSWORD,edit,1350631584
Control3=IDC_REMEMBER_NAMEPW,button,1342242819
Control4=IDC_RADIO_REALMONEY,button,1342177289
Control5=IDC_RADIO_PLAYMONEY,button,1342177289
Control6=IDOK,button,1342242817
Control7=IDCANCEL,button,1342242816
Control8=IDC_EDITHOST,edit,1082196096
Control9=IDC_EDITPORT,edit,1082204288
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,button,1342177287
Control13=IDC_STATIC,button,1342177287

[DLG:IDD_TRANSACTION]
Type=1
Class=CTransactionDlg
ControlCount=2
Control1=IDC_PROGRESS1,msctls_progress32,1350565889
Control2=IDC_STATIC,static,1342308352

[CLS:CTransactionDlg]
Type=0
HeaderFile=transactiondlg.h
ImplementationFile=transactiondlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CTransactionDlg

[DLG:IDD_TOURNAMENTDETAILS]
Type=1
Class=?
ControlCount=10
Control1=IDC_TD_BUYIN,edit,1350633600
Control2=IDC_TD_STARTTIME,edit,1350633600
Control3=IDC_TD_PLAYERS,edit,1350633600
Control4=IDC_TD_DESCRIPTION,edit,1350633540
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_EDITRULES,button,1476460544
Control10=IDC_EDITFORMAT,button,1476460544

[DLG:IDD_EDITOR]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT,edit,1350631556

[DLG:IDD_CREDITCARDS]
Type=1
Class=CCreditCardDlg
ControlCount=34
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CC1_TYPE,combobox,1344339971
Control4=IDC_CC1_NUMBER,edit,1350639744
Control5=IDC_CC1_EXPRMONTH,combobox,1344340227
Control6=IDC_CC1_EXPRYEAR,combobox,1344340227
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_CC2_TYPE,combobox,1344339971
Control11=IDC_CC2_NUMBER,edit,1350639744
Control12=IDC_CC2_EXPRMONTH,combobox,1344340227
Control13=IDC_CC2_EXPRYEAR,combobox,1344340227
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_CC3_TYPE,combobox,1344339971
Control18=IDC_CC3_NUMBER,edit,1350639744
Control19=IDC_CC3_EXPRMONTH,combobox,1344340227
Control20=IDC_CC3_EXPRYEAR,combobox,1344340227
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_CC4_TYPE,combobox,1344339971
Control25=IDC_CC4_NUMBER,edit,1350639744
Control26=IDC_CC4_EXPRMONTH,combobox,1344340227
Control27=IDC_CC4_EXPRYEAR,combobox,1344340227
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_STATIC,button,1342177287
Control32=IDC_STATIC,button,1342177287
Control33=IDC_STATIC,button,1342177287
Control34=IDC_STATIC,button,1342177287

[CLS:CCreditCardDlg]
Type=0
HeaderFile=creditcarddlg.h
ImplementationFile=creditdarddlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CCreditCardDlg
VirtualFilter=dWC

