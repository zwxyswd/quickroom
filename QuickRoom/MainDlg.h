//////////////////////////////////////////////////////////////////////
//  ������:	    2009��10��15�� 18:22
//  �ļ���:     MainDlg.h
//  ��  ��:     lynn
//  ��  վ:     www.pleee.com, www.lynnmh.com
//  ��ϵ��ʽ��  lynnux@yeah.net
//  ˵  ����  ������������ô�Դ�룬�����ܸ��Ļ��Ƴ���Ȩ��Ϣ
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
