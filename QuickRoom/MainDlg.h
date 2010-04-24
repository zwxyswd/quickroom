//////////////////////////////////////////////////////////////////////
//  创建于:	    2009年10月15日 18:22
//  文件名:     MainDlg.h
//  作  者:     lynn
//  网  站:     www.pleee.com, www.lynnmh.com
//  联系方式：  lynnux@yeah.net
//  说  明：  你可以自由利用此源码，但不能更改或移除版权信息
//////////////////////////////////////////////////////////////////////

#pragma once
#include <atlframe.h>
#include <atlmisc.h>
#include "resource.h"
#include "Ini.h"

class CMainDlg 
    : public CDialogImpl<CMainDlg>
    , public CUpdateUI<CMainDlg>
    , public CMessageFilter
    , public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC,OnCtlColorStatic)
    END_MSG_MAP()

    LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void CloseDialog(int nVal);
//protected:
//    static DWORD WINAPI ThreadStart(
//        LPVOID lpParameter
//        );
private:
    CIni m_ini;
    void CMainDlg::GetRecord(HWND hwndToClick);
};
