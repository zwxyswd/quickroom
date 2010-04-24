//////////////////////////////////////////////////////////////////////
//  创建于:	    2009年10月15日 18:22
//  文件名:     maindlg.CPP
//  作  者:     lynn
//  网  站:     www.pleee.com, www.lynnmh.com
//  联系方式：  lynnux@yeah.net
//  说  明：  你可以自由利用此源码，但不能更改或移除版权信息
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
INT  g_iSleep; //以秒计的
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
    ::MapWindowPoints(NULL, g_hMainDlg, (LPPOINT)&rc, 2);   // 等同::ScreenToClient(g_hMainDlg, &rc);
    ::InvalidateRect(g_hMainDlg, &rc, TRUE);   // 莫名其妙的显示问题
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
//    MessageBox(NULL, (LPCTSTR)LocalLock(hLocal), _T("错误提示"), MB_OK|MB_ICONINFORMATION);
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
#define GO2IF0(x) if(!x){goto over2;};  //这样貌似还是有些效率吧

    TCHAR pszStr[MAX_PATH];
    ZeroMemory(pszStr, sizeof(pszStr));
    VIRTUALIZER_START
    while (g_bStart)
    {
        //Sleep(100); //100都还看的到那个对话框

        //关闭vip提示窗口
        HWND hwndMessage = FindWindow(_T("#32770"), _T("VS竞技游戏平台"));
        GO2IF0(hwndMessage);
        HWND hChild = FindWindowExA(hwndMessage, 0, "Shell Embedding", NULL);
        GO2IF0(hChild);
        hChild = FindWindowExA(hChild, 0, "Shell DocObject View", NULL);
        GO2IF0(hChild);
        if (hwndMessage)
        {
            PostMessage(hwndMessage, WM_KEYDOWN, VK_ESCAPE, 0);//汗，这里应该是PostMessage才行！
        }
over2:
        //关闭请稍候再试窗口
        hwndMessage = FindWindow(_T("#32770"), _T("VSClient"));
        GO1IF0(hwndMessage);
        hChild = FindWindowExA(hwndMessage, 0, "Button", "确定");
        GO1IF0(hChild);
        hChild = ::GetWindow(hChild, GW_HWNDNEXT);    //static
        GO1IF0(hChild);
		GetWindowText(hChild, pszStr, MAX_PATH);
		if (!lstrcmpi(pszStr, _T("当前网络连接不可用,不能进入房间")) && hwndMessage)
		{
			PostMessage(hwndMessage, WM_KEYDOWN, VK_ESCAPE, 0);
			goto over1;
		}
        hChild = ::GetWindow(hChild, GW_HWNDNEXT);
        GO1IF0(hChild);
        GetWindowText(hChild, pszStr, MAX_PATH);
        if (!lstrcmpi(pszStr, _T("请稍后再试")) && hwndMessage)
        {
            PostMessage(hwndMessage, WM_KEYDOWN, VK_ESCAPE, 0);
        }
over1:
        //关闭进房提示窗口
        hwndMessage = FindWindow(_T("#32770"), _T(""));
		while (hwndMessage)
		{
			hChild = FindWindowExA(hwndMessage, 0, "Button", "确定");
			GO0IF0(hChild);
			hChild = ::GetWindow(hChild, GW_HWNDNEXT);    //取消
			GO0IF0(hChild);
			hChild = ::GetWindow(hChild, GW_HWNDNEXT);
			GO0IF0(hChild);
			GetWindowText(hChild, pszStr, MAX_PATH);
			if(0 == lstrcmpi(pszStr, _T(" 你已在一个房间了，要离开吗？")) && hwndMessage)
			{
				PostMessage(hwndMessage, WM_KEYDOWN, VK_ESCAPE, 0);
				g_bStart = FALSE;//看到此窗口表明挤房成功了！
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
    ShowInfo(_T("开始挤房间……"));
    VIRTUALIZER_START;
    if(!g_bSingle)
    {
        int iItem;
        int iAll = ListView_GetItemCount(hwndListView);

        while(ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED)>g_vi[0] && g_bStart)   //&& g_bStart防止意外情况
            SendMessage(hwndListView, WM_KEYDOWN, VK_PRIOR, 0);

        while(g_bStart)
        {
            iItem = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
            if(iItem == (iAll-1) || iItem == g_vi[(g_iTotal==0) ? 0 : (g_iTotal-1)]) //到最后一行，或者是符合标准的最后一个就立刻返回到第一行
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
            if(i != g_iTotal)   //如果g_vi里没有i当然就是g_iTotal啦
            {
                SendMessage(hwndListView, WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(0, 0));    //要等点击了人满那个提示后才返回，关闭那个提示是另外一个线程干的事
                Sleep(g_iSleep*1000);
            }
            SendMessage(hwndListView, WM_KEYDOWN, VK_DOWN, 0);
        }
    }
    else
    {
		
        while(g_bStart)
        {
            SendMessage(hwndListView, WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(0, 0));    //要等点击了人满那个提示后才返回，关闭那个提示是另外一个线程干的事
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
    ShowInfo( _T("已停止挤房"));
    SetDlgItemText(g_hMainDlg, IDOK, _T("给我挤"));
    ExitThread(0);
    return 0;
}
//保存当前SysListView32的所有数据并且筛选出符合我们要的数据，以备ThreadStart挤房使用
void CMainDlg::GetRecord(HWND hwndToClick)
{
#define GO0IF0(x) if(!x){goto over0;};

    ShowInfo( _T("正在搜集数据，稍等……"));
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
    g_iTotal = 0;   //上几个版本居然没这句？！
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
            ListView_GetItemTextEx(hwndToClick, iItem, 0, (LPTSTR)lpGet, MAX_PATH);//0房间名
            GO0IF0(*(TCHAR*)lpStr);
            CString strRoomName(lpStr);
            iFind = strRoomName.Find(g_pszRoomName);
            GO0IF0((iFind!=-1));
        }
        ListView_GetItemTextEx(hwndToClick, iItem, 2, (LPTSTR)lpGet, MAX_PATH);//2进入等级
        GO0IF0(*(TCHAR*)lpStr);
        UINT iTemp = StrToInt(lpStr);
        if( iTemp>= iLevel1 && iTemp<= iLevel2)
        {
            ListView_GetItemTextEx(hwndToClick, iItem, 3, (LPTSTR)lpGet, MAX_PATH);//3最高ping值
            GO0IF0(*(TCHAR*)lpStr);
            if (lstrcmpi(_T("NoPing"), lpStr))
            {
                UINT iTemp = StrToInt(lpStr);
                if (iTemp < iPing)
                {
                    ListView_GetItemTextEx(hwndToClick, iItem, 1, (LPTSTR)lpGet, MAX_PATH);//1在线人数
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

    ShowInfo( _T("数据搜集完毕，开始挤房"));

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
        CString strVSCaption = _T("VS竞技游戏平台 -- 测试版-");
		CString strVSCaption2 = _T("VS竞技游戏平台 -- 正式版-");
        strVSCaption.Append(strVSVersion);
		strVSCaption2.Append(strVSVersion);
        g_hwndVS = FindWindow(_T("#32770"), strVSCaption);
        if (0 == g_hwndVS)
        {
			g_hwndVS = FindWindow(_T("#32770"), strVSCaption2);
			if (0 == g_hwndVS)
			{
				ShowInfo(_T("必须登录VS后才能使用，或者你填写的VS版本不对，请查看VS窗口标题"));
				return 0;
			}
        }

        //挤房共有设置
        g_iSleep = GetDlgItemInt(IDC_EDIT_SLEEP, NULL, FALSE);

        VIRTUALIZER_START;
        g_bSingle = IsDlgButtonChecked(IDC_RADIO2); //是否是挤单个房间
        if(!g_bSingle)
        {
            UINT i1 = GetDlgItemInt(IDC_EDIT_LEVEL1, NULL, FALSE);
            UINT i2 = GetDlgItemInt(IDC_EDIT_LEVEL2, NULL, FALSE);
            if (i1 > i2)
            {
                ShowInfo(_T("等级范围填写有误，小数填左，大数填右，可以填相同的数字"));
                return 0;
            }
            i1 = GetDlgItemInt(IDC_EDIT_PEOPLE1, NULL, FALSE);
            i2 = GetDlgItemInt(IDC_EDIT_PEOPLE2, NULL, FALSE);
            if (i1 > i2)
            {
                ShowInfo(_T("房间人数填写有误，小数填左，大数填右，可以填相同的数字"));
                return 0;
            }
            
            GetDlgItemText(IDC_EDIT_NAME, g_pszRoomName, MAX_PATH);
            g_bRoomRandom = (*(TCHAR*)g_pszRoomName == NULL) ? TRUE : FALSE;
        }

        HWND hwndTemp = FindWindowExA(g_hwndVS, 0, "Static", "Static");
        //跳过第一个 "Static", "Static" 窗口
        hwndTemp = ::GetWindow(hwndTemp, GW_HWNDNEXT);              
        //以前一直理解错了！FindWindowEx的第二个参数是第一个参数的子窗口，从第二个参数开始搜索，也就是第二个参数是一个起始索引
        hwndTemp = FindWindowExA(g_hwndVS, hwndTemp, "Static", "Static");  
        hwndTemp = FindWindowExA(hwndTemp, 0, "Static", "Static");
        hwndTemp = FindWindowExA(hwndTemp, 0, "Static", "Static");
        hwndTemp = FindWindowExA(hwndTemp, 0, "SysListView32", 0);

        if (!hwndTemp)
        {
            ShowInfo(_T("没有找到房间窗口！"));
            return 0;
        }
        if(!g_bSingle)
        {
            GetRecord(hwndTemp);    //搜集符合要求的数据
        }
        CreateThread(NULL, 0, ThreadStart, hwndTemp, 0, NULL);  //挤房线程
        CreateThread(NULL, 0, ThreadCloseWindow, NULL, 0, NULL);  //关闭提示窗口以及判断是否挤房成功
        VIRTUALIZER_END;
        g_bStart = TRUE;
        SetDlgItemText(IDOK, _T("不挤了"));
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
        ShowInfo(_T("已停止挤房"));
        SetDlgItemText(IDOK, _T("给我挤"));
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