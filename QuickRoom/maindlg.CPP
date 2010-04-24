//////////////////////////////////////////////////////////////////////
//  ������:	    2009��10��15�� 18:22
//  �ļ���:     maindlg.CPP
//  ��  ��:     lynn
//  ��  վ:     www.pleee.com, www.lynnmh.com
//  ��ϵ��ʽ��  lynnux@yeah.net
//  ˵  ����  ������������ô�Դ�룬�����ܸ��Ļ��Ƴ���Ȩ��Ϣ
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "MainDlg.h"
#include "VirtualizerSDK.h "
#include <atlctrls.h >
#define CURVERSION _T("QuickRoom 0.41")
HWND g_hwndVS = NULL;
BOOL g_bStart = FALSE;
INT  g_vi[500];
INT  g_iTotal;
INT  g_iSleep; //����Ƶ�
HWND g_hMainDlg;
TCHAR g_pszRoomName[MAX_PATH];
BOOL g_bRoomRandom = TRUE;
BOOL g_bSingle = FALSE;
int nTimes = 0;
void ShowInfo(LPCTSTR lpInfo)
{
    ::SetDlgItemText(g_hMainDlg, IDC_STATIC_MSG, lpInfo);
    HWND hMsg = ::GetDlgItem(g_hMainDlg, IDC_STATIC_MSG);
    RECT rc;
    ::GetWindowRect(hMsg, &rc);
    //::ScreenToClient(g_hMainDlg, &rc);
    ::MapWindowPoints(NULL, g_hMainDlg, (LPPOINT)&rc, 2);   // ��ͬ::ScreenToClient(g_hMainDlg, &rc);
    ::InvalidateRect(g_hMainDlg, &rc, TRUE);   // Ī���������ʾ����
}

void IncTime()
{
	SYSTEMTIME time;
	::GetLocalTime(&time);
	time.wSecond += 15;
	if (time.wSecond >=60)
	{
		time.wMinute++;
		time.wSecond -= 60;
		if (time.wMinute >= 60)
		{
			time.wMinute -= 60;
			time.wHour++;
			if (time.wHour >= 24)
			{
				time.wHour -=24;
				time.wDay++;
			}
		}
	}
	::SetLocalTime(&time);
}

void DecTime()
{
	SYSTEMTIME time;
	::GetLocalTime(&time);
	if (time.wSecond > 14)
	{
		time.wSecond -= 15;
	} 
	else
	{
		time.wSecond += 45;
		if (time.wMinute > 0)
		{
			time.wMinute--;
		} 
		else
		{
			time.wMinute = 59;
			if (time.wHour > 0)
			{
				time.wHour--;
			} 
			else
			{
				time.wHour = 23;
				if (time.wDay > 1)
				{
					time.wDay--;
				} 
				else
				{
					
				}
			}
		}
	}
	::SetLocalTime(&time);
}
//void __ErrShow()
//{
//    DWORD  dwErrCode = GetLastError();
//    HLOCAL hLocal    = NULL;
//
//    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
//        NULL,
//        dwErrCode,
//        0,
//        (PTSTR)&hLocal,
//        0,
//        NULL);
//
//    MessageBox(NULL, (LPCTSTR)LocalLock(hLocal), _T("������ʾ"), MB_OK|MB_ICONINFORMATION);
//    LocalFree(hLocal);	
//}
void EnablePrivilege(void)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp = {0};
    HANDLE hProcess = GetCurrentProcess();
    if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
        &hToken))
        return;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
    {
        CloseHandle(hToken);
        return;
    }
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES),
        NULL, NULL);
    CloseHandle(hToken);
}
DWORD WINAPI ThreadCloseWindow(
                         LPVOID /*lpParameter*/
                         )
{
#define GO0IF0(x) if(!x){goto over0;};
#define GO1IF0(x) if(!x){goto over1;};
#define GO2IF0(x) if(!x){goto over2;};  //����ò�ƻ�����ЩЧ�ʰ�

    TCHAR pszStr[MAX_PATH];
    ZeroMemory(pszStr, sizeof(pszStr));
    VIRTUALIZER_START
    while (g_bStart)
    {
        //Sleep(100); //100�������ĵ��Ǹ��Ի���

        //�ر�vip��ʾ����
        HWND hwndMessage = FindWindow(_T("#32770"), _T("VS������Ϸƽ̨"));
        GO2IF0(hwndMessage);
        HWND hChild = FindWindowExA(hwndMessage, 0, "Shell Embedding", NULL);
        GO2IF0(hChild);
        hChild = FindWindowExA(hChild, 0, "Shell DocObject View", NULL);
        GO2IF0(hChild);
        if (hwndMessage)
        {
            PostMessage(hwndMessage, WM_KEYDOWN, VK_ESCAPE, 0);//��������Ӧ����PostMessage���У�
        }
over2:
        //�ر����Ժ����Դ���
        hwndMessage = FindWindow(_T("#32770"), _T("VSClient"));
        GO1IF0(hwndMessage);
        hChild = FindWindowExA(hwndMessage, 0, "Button", "ȷ��");
        GO1IF0(hChild);
        hChild = ::GetWindow(hChild, GW_HWNDNEXT);    //static
        GO1IF0(hChild);
		GetWindowText(hChild, pszStr, MAX_PATH);
		if (!lstrcmpi(pszStr, _T("��ǰ�������Ӳ�����,���ܽ��뷿��")) && hwndMessage)
		{
			PostMessage(hwndMessage, WM_KEYDOWN, VK_ESCAPE, 0);
			goto over1;
		}
        hChild = ::GetWindow(hChild, GW_HWNDNEXT);
        GO1IF0(hChild);
        GetWindowText(hChild, pszStr, MAX_PATH);
        if (!lstrcmpi(pszStr, _T("���Ժ�����")) && hwndMessage)
        {
            PostMessage(hwndMessage, WM_KEYDOWN, VK_ESCAPE, 0);
        }
over1:
        //�رս�����ʾ����
        hwndMessage = FindWindow(_T("#32770"), _T(""));
		while (hwndMessage)
		{
			hChild = FindWindowExA(hwndMessage, 0, "Button", "ȷ��");
			GO0IF0(hChild);
			hChild = ::GetWindow(hChild, GW_HWNDNEXT);    //ȡ��
			GO0IF0(hChild);
			hChild = ::GetWindow(hChild, GW_HWNDNEXT);
			GO0IF0(hChild);
			GetWindowText(hChild, pszStr, MAX_PATH);
			if(0 == lstrcmpi(pszStr, _T(" ������һ�������ˣ�Ҫ�뿪��")) && hwndMessage)
			{
				PostMessage(hwndMessage, WM_KEYDOWN, VK_ESCAPE, 0);
				g_bStart = FALSE;//�����˴��ڱ��������ɹ��ˣ�
				break;
			}
over0:
			hwndMessage = FindWindowExW(NULL, hwndMessage, _T("#32770"), _T(""));
		}
    }
    VIRTUALIZER_END
    ExitThread(0);
    return 0;
#undef GO0IF0
#undef GO1IF0
#undef GO2IF0
}
DWORD WINAPI ThreadStart(
                         LPVOID lpParameter
                         )
{
    HWND hwndListView = (HWND)lpParameter;
    ShowInfo(_T("��ʼ�����䡭��"));
    VIRTUALIZER_START;
    if(!g_bSingle)
    {
        int iItem;
        int iAll = ListView_GetItemCount(hwndListView);

        while(ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED)>g_vi[0] && g_bStart)   //&& g_bStart��ֹ�������
            SendMessage(hwndListView, WM_KEYDOWN, VK_PRIOR, 0);

        while(g_bStart)
        {
            iItem = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
            if(iItem == (iAll-1) || iItem == g_vi[(g_iTotal==0) ? 0 : (g_iTotal-1)]) //�����һ�У������Ƿ��ϱ�׼�����һ�������̷��ص���һ��
            {
                while(ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED)>g_vi[0] && g_bStart)
                    SendMessage(hwndListView, WM_KEYDOWN, VK_PRIOR, 0);
            }
            int i=0;
            for (; i<g_iTotal; i++)
            {
                if(g_vi[i] == iItem)
                    break;
            }
            if(i != g_iTotal)   //���g_vi��û��i��Ȼ����g_iTotal��
            {
                SendMessage(hwndListView, WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(0, 0));    //Ҫ�ȵ���������Ǹ���ʾ��ŷ��أ��ر��Ǹ���ʾ������һ���̸߳ɵ���
                Sleep(g_iSleep*1000);
            }
            SendMessage(hwndListView, WM_KEYDOWN, VK_DOWN, 0);
        }
    }
    else
    {
		
        while(g_bStart)
        {
            SendMessage(hwndListView, WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(0, 0));    //Ҫ�ȵ���������Ǹ���ʾ��ŷ��أ��ر��Ǹ���ʾ������һ���̸߳ɵ���
            Sleep(g_iSleep*1000);
			IncTime();
			nTimes++;
        }
		
		while (nTimes > 0)
		{
			nTimes--;
			DecTime();
		}
		nTimes = 0;
    }
    VIRTUALIZER_END;
    g_bStart = FALSE;
    ShowInfo( _T("��ֹͣ����"));
    SetDlgItemText(g_hMainDlg, IDOK, _T("���Ҽ�"));
    ExitThread(0);
    return 0;
}
//���浱ǰSysListView32���������ݲ���ɸѡ����������Ҫ�����ݣ��Ա�ThreadStart����ʹ��
void CMainDlg::GetRecord(HWND hwndToClick)
{
#define GO0IF0(x) if(!x){goto over0;};

    ShowInfo( _T("�����Ѽ����ݣ��Եȡ���"));
    UINT iLevel1 = GetDlgItemInt(IDC_EDIT_LEVEL1, NULL, FALSE);
    UINT iLevel2 = GetDlgItemInt(IDC_EDIT_LEVEL2, NULL, FALSE);
    UINT iPeople1 = GetDlgItemInt(IDC_EDIT_PEOPLE1, NULL, FALSE);
    UINT iPeople2 = GetDlgItemInt(IDC_EDIT_PEOPLE2, NULL, FALSE);
    UINT iPing   = GetDlgItemInt(IDC_EDIT_PING, NULL, FALSE);

    VIRTUALIZER_START;
    DWORD dwPid;
    GetWindowThreadProcessId(g_hwndVS, &dwPid);
    HANDLE hWar3 = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    LPVOID lpGet = VirtualAllocEx(hWar3, NULL, sizeof(TCHAR)*MAX_PATH + sizeof(LV_ITEM), MEM_COMMIT, PAGE_READWRITE);
    LV_ITEM _ms_lvi;

    TCHAR  lpStr[MAX_PATH];
    int iAll = ListView_GetItemCount(hwndToClick);
    ZeroMemory(g_vi, sizeof(g_vi));
    g_iTotal = 0;   //�ϼ����汾��Ȼû��䣿��
    int iItem = 0;

#define ListView_GetItemTextEx(hwndLV, i, iSubItem_, pszText_, cchTextMax_) \
    {\
    _ms_lvi.iSubItem = iSubItem_;\
    _ms_lvi.cchTextMax = cchTextMax_;\
    _ms_lvi.pszText = pszText_;\
    WriteProcessMemory(hWar3, (LPVOID)((BYTE*)lpGet + sizeof(TCHAR)*MAX_PATH), &_ms_lvi, sizeof(LV_ITEM), NULL);\
    SNDMSG((hwndLV), LVM_GETITEMTEXT, (WPARAM)(i), (LPARAM)(LV_ITEM *)((BYTE*)lpGet+sizeof(TCHAR)*MAX_PATH));\
    ReadProcessMemory(hWar3, lpGet, lpStr, sizeof(lpStr), NULL);\
    }

    do
    {
        INT iFind = 0;
        if (!g_bRoomRandom && *(TCHAR*)g_pszRoomName != 0)
        {
            ListView_GetItemTextEx(hwndToClick, iItem, 0, (LPTSTR)lpGet, MAX_PATH);//0������
            GO0IF0(*(TCHAR*)lpStr);
            CString strRoomName(lpStr);
            iFind = strRoomName.Find(g_pszRoomName);
            GO0IF0((iFind!=-1));
        }
        ListView_GetItemTextEx(hwndToClick, iItem, 2, (LPTSTR)lpGet, MAX_PATH);//2����ȼ�
        GO0IF0(*(TCHAR*)lpStr);
        UINT iTemp = StrToInt(lpStr);
        if( iTemp>= iLevel1 && iTemp<= iLevel2)
        {
            ListView_GetItemTextEx(hwndToClick, iItem, 3, (LPTSTR)lpGet, MAX_PATH);//3���pingֵ
            GO0IF0(*(TCHAR*)lpStr);
            if (lstrcmpi(_T("NoPing"), lpStr))
            {
                UINT iTemp = StrToInt(lpStr);
                if (iTemp < iPing)
                {
                    ListView_GetItemTextEx(hwndToClick, iItem, 1, (LPTSTR)lpGet, MAX_PATH);//1��������
                    GO0IF0(*(TCHAR*)lpStr);
                    UINT iTemp = StrToInt(lpStr);
                    if (iTemp>= iPeople1 && iTemp <= iPeople2)
                    {
                        g_vi[g_iTotal++] = iItem;
                    }
                }
            }
        }
over0:
        NULL;
    }while (++iItem < iAll);

    VirtualFreeEx(hWar3, lpStr, 0, MEM_RELEASE);
    CloseHandle(hWar3);
    VIRTUALIZER_END;

    ShowInfo( _T("�����Ѽ���ϣ���ʼ����"));

#undef ListView_GetItemTextEx
#undef GO0IF0
}
LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if(!g_bStart)
    {
        //firstly, check whether vs is running
        CString strVSVersion;
        GetDlgItemText(IDC_EDIT_VSVERSION,strVSVersion);
        CString strVSCaption = _T("VS������Ϸƽ̨ -- ���԰�-");
		CString strVSCaption2 = _T("VS������Ϸƽ̨ -- ��ʽ��-");
        strVSCaption.Append(strVSVersion);
		strVSCaption2.Append(strVSVersion);
        g_hwndVS = FindWindow(_T("#32770"), strVSCaption);
        if (0 == g_hwndVS)
        {
			g_hwndVS = FindWindow(_T("#32770"), strVSCaption2);
			if (0 == g_hwndVS)
			{
				ShowInfo(_T("�����¼VS�����ʹ�ã���������д��VS�汾���ԣ���鿴VS���ڱ���"));
				return 0;
			}
        }

        //������������
        g_iSleep = GetDlgItemInt(IDC_EDIT_SLEEP, NULL, FALSE);

        VIRTUALIZER_START;
        g_bSingle = IsDlgButtonChecked(IDC_RADIO2); //�Ƿ��Ǽ���������
        if(!g_bSingle)
        {
            UINT i1 = GetDlgItemInt(IDC_EDIT_LEVEL1, NULL, FALSE);
            UINT i2 = GetDlgItemInt(IDC_EDIT_LEVEL2, NULL, FALSE);
            if (i1 > i2)
            {
                ShowInfo(_T("�ȼ���Χ��д����С�����󣬴������ң���������ͬ������"));
                return 0;
            }
            i1 = GetDlgItemInt(IDC_EDIT_PEOPLE1, NULL, FALSE);
            i2 = GetDlgItemInt(IDC_EDIT_PEOPLE2, NULL, FALSE);
            if (i1 > i2)
            {
                ShowInfo(_T("����������д����С�����󣬴������ң���������ͬ������"));
                return 0;
            }
            
            GetDlgItemText(IDC_EDIT_NAME, g_pszRoomName, MAX_PATH);
            g_bRoomRandom = (*(TCHAR*)g_pszRoomName == NULL) ? TRUE : FALSE;
        }

        HWND hwndTemp = FindWindowExA(g_hwndVS, 0, "Static", "Static");
        //������һ�� "Static", "Static" ����
        hwndTemp = ::GetWindow(hwndTemp, GW_HWNDNEXT);              
        //��ǰһֱ������ˣ�FindWindowEx�ĵڶ��������ǵ�һ���������Ӵ��ڣ��ӵڶ���������ʼ������Ҳ���ǵڶ���������һ����ʼ����
        hwndTemp = FindWindowExA(g_hwndVS, hwndTemp, "Static", "Static");  
        hwndTemp = FindWindowExA(hwndTemp, 0, "Static", "Static");
        hwndTemp = FindWindowExA(hwndTemp, 0, "Static", "Static");
        hwndTemp = FindWindowExA(hwndTemp, 0, "SysListView32", 0);

        if (!hwndTemp)
        {
            ShowInfo(_T("û���ҵ����䴰�ڣ�"));
            return 0;
        }
        if(!g_bSingle)
        {
            GetRecord(hwndTemp);    //�Ѽ�����Ҫ�������
        }
        CreateThread(NULL, 0, ThreadStart, hwndTemp, 0, NULL);  //�����߳�
        CreateThread(NULL, 0, ThreadCloseWindow, NULL, 0, NULL);  //�ر���ʾ�����Լ��ж��Ƿ񼷷��ɹ�
        VIRTUALIZER_END;
        g_bStart = TRUE;
        SetDlgItemText(IDOK, _T("������"));
    }
    else
    {
        g_bStart = FALSE;
		while (nTimes > 0)
		{
			nTimes--;
			DecTime();
		}
		nTimes=0;
        ShowInfo(_T("��ֹͣ����"));
        SetDlgItemText(IDOK, _T("���Ҽ�"));
    }
    return 0;
}



LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CenterWindow();
    HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);
    UIAddChildWindowContainer(m_hWnd);

    //////////////////////////////////////////////////////////////////////////
    CString strVSVersion;
    INT i;
    m_ini.SetIniName();
    i = m_ini.GetInt(_T("People1"), 150);
    SetDlgItemInt(IDC_EDIT_PEOPLE1, i, FALSE);
    i = m_ini.GetInt(_T("People2"), 200);
    SetDlgItemInt(IDC_EDIT_PEOPLE2, i, FALSE);
    i = m_ini.GetInt(_T("Level1"), 1);
    SetDlgItemInt(IDC_EDIT_LEVEL1, i, FALSE);
    i = m_ini.GetInt(_T("Level2"), 1);
    SetDlgItemInt(IDC_EDIT_LEVEL2, i, FALSE);
    i = m_ini.GetInt(_T("Ping"), 100);
    SetDlgItemInt(IDC_EDIT_PING, i, FALSE);
    g_iSleep = m_ini.GetInt(_T("Sleep"), 0);
    SetDlgItemInt(IDC_EDIT_SLEEP, g_iSleep, FALSE);
    i = m_ini.GetInt(_T("Single"), 0);
    CheckDlgButton(i == 0 ? IDC_RADIO1:IDC_RADIO2, BST_CHECKED);
    CheckDlgButton(i == 0 ? IDC_RADIO2:IDC_RADIO1, BST_UNCHECKED);
    m_ini.GetString(_T("VSVersion"), (PTSTR)(LPCTSTR)strVSVersion, MAX_PATH, _T("2.5.3"));
    SetDlgItemText(IDC_EDIT_VSVERSION, strVSVersion);
    m_ini.GetString(_T("Name"), (PTSTR)(LPCTSTR)strVSVersion);
    SetDlgItemText(IDC_EDIT_NAME, strVSVersion);
    EnablePrivilege();
    g_hMainDlg = m_hWnd;
    g_bStart = FALSE;
    //strVSVersion.Format(_T("%s%x"), CURVERSION, GetTickCount());
    strVSVersion = CURVERSION;
    SetWindowText(strVSVersion);
    return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);
	while (nTimes > 0)
	{
		nTimes--;
		DecTime();
	}
	nTimes = 0;
    //////////////////////////////////////////////////////////////////////////
    CString strVSVersion;
    INT i;
    GetDlgItemText(IDC_EDIT_VSVERSION, strVSVersion);
    m_ini.SetString(_T("VSVersion"), strVSVersion);
    GetDlgItemText(IDC_EDIT_NAME, strVSVersion);
    m_ini.SetString(_T("Name"), strVSVersion);
    i = GetDlgItemInt(IDC_EDIT_PEOPLE1, NULL, FALSE);
    m_ini.SetInt(_T("People1"), i);
    i = GetDlgItemInt(IDC_EDIT_PEOPLE2, NULL, FALSE);
    m_ini.SetInt(_T("People2"), i);
    i = GetDlgItemInt(IDC_EDIT_LEVEL1, NULL, FALSE);
    m_ini.SetInt(_T("Level1"), i);
    i = GetDlgItemInt(IDC_EDIT_LEVEL2, NULL, FALSE);
    m_ini.SetInt(_T("Level2"), i);
    i = GetDlgItemInt(IDC_EDIT_PING, NULL, FALSE);
    m_ini.SetInt(_T("Ping"), i);
    i = GetDlgItemInt(IDC_EDIT_SLEEP, NULL, FALSE);
    m_ini.SetInt(_T("Sleep"), i);
    i = IsDlgButtonChecked(IDC_RADIO2);
    m_ini.SetInt(_T("Single"), i);
    return 0;
}

BOOL CMainDlg::PreTranslateMessage( MSG* pMsg )
{
    return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
    return FALSE;
}

void CMainDlg::CloseDialog( int nVal )
{
    DestroyWindow();
    ::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnCancel( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
    CloseDialog(wID);
    return 0;
}

LRESULT CMainDlg::OnCtlColorStatic( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ )
{
    if ((HWND)lParam == GetDlgItem(IDC_STATIC_MSG))
    {
        SetTextColor((HDC)wParam, 0x0000ff); 
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)::GetSysColorBrush(CTLCOLOR_DLG);
    }
    else
    {
        return 0;
    }

}