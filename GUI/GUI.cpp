#include "stdafx.h"


void Catchi();
void s(int cathy);
void s(LPCWSTR cathy);

#include <TlHelp32.h>
#include <commdlg.h>
#include "GUI.h"
#include "Shlobj.h" 
#include <shellapi.h>
#include <ctime>
#include <atlimage.h>
#include <cmath>
#include "WndShadow.h"
#include "Actions.h"
#include "TestFunctions.h"
#pragma comment(lib, "urlmon.lib")//下载文件用的Lib   
#pragma warning(disable: 4244)
#pragma warning(disable:4996)
#define MAX_LOADSTRING 100
#define CathyageBox(cathy) s(cathy)

VOID                InitCathy();
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    CatchProc(HWND, UINT, WPARAM, LPARAM);
NOTIFYICONDATA tnd;//伪装托盘图标信息
WNDCLASSEXW cat;//第二窗口的类
HINSTANCE hInst;// 当前实例
HINSTANCE hinst;//第二窗口实例(类似于hwnd)
WCHAR szTitle[MAX_LOADSTRING];//标题栏文本
HWND hWndParent;
WCHAR szWindowClass[MAX_LOADSTRING];//主窗口类名
WCHAR CatchWindow[MAX_LOADSTRING] = L"CatchWindow";//第二窗口类名

												   //自己定义的 有点乱

HBITMAP hBitmap;
ULONG mypid;//当前程序pid
HWND Catch;//捕捉窗口句柄
int FC;//是否第一次打开捕捉窗口？是否需要调用InitCathy()
HDESK hVirtualDesk, hCurrentDesk, defaultDesk;//虚拟桌面 & 当前桌面 & 默认桌面
HANDLE explorerInfo;

POINT point;//当前鼠标坐标结构体
//HWND main; //主窗口的hwnd,挂钩用s
TCHAR Path[MAX_PATH + 1] = { 0 }, Name[MAX_PATH + 1] = { 0 }, Gutc[2001] = { 0 };//程序路径 and 路径+程序名 and 冗余
TCHAR A[20], Num[20];
BOOL Admin; //是否管理员
int Bit;//系统位数 32 34 64
BOOL First = FALSE;//是否第一次在当前系统上运行
HHOOK myhook; //测试钩子
HDC hdc;//捕捉窗口专用全局hdc
int Ctrl;//左Ctrl按键是否按下
int hide;//窗口是否隐藏
int oneclick;//一键安装状态?
int GameMode;
//HFONT st;//宋体
HFONT st34;//Catch专用宋体
WCHAR text1[2550];//GetWindowText专用缓冲区

HBRUSH DBlueBrush = CreateSolidBrush(RGB(225, 255, 255));//专用蓝色笔刷
HBRUSH LBlueBrush = CreateSolidBrush(RGB(235, 255, 255));//专用蓝色笔刷
HBRUSH WhiteBrush = CreateSolidBrush(RGB(255, 255, 255));//专用白色笔刷
HBRUSH green = CreateSolidBrush(RGB(0, 206, 209));//专用元亮色笔刷
HBRUSH grey = CreateSolidBrush(RGB(245, 245, 245));//专用辉色笔刷
HPEN RED = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));//红
HPEN BLACK = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));//黑
HPEN White = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));//专用白色笔刷
HPEN GREEN = CreatePen(PS_SOLID, 2, RGB(5, 195, 195));//check专用绿色
HPEN LGREY = CreatePen(PS_SOLID, 1, RGB(120, 120, 120));//check专用灰色
HPEN BLUE = CreatePen(PS_SOLID, 1, RGB(40, 130, 240));//check专用蓝色
HFONT st = CreateFontW(16, 8, 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));

int timerleft = -2;//吃窗口倒计时
HWND Eatlist[1000];//已经吃掉的窗口句柄
int cureat;//已经吃掉的窗口数量
int Eatpid[1000], curpid;//要吃的pid & pid数量
wchar_t zxfback[101];//倒计时备份按钮名称
STARTUPINFO si;//CreateProcess专用
PROCESS_INFORMATION pi;//CreateProcess专用
LPCWSTR SethcPath;//路径
wchar_t szVDesk[] = L"Cathy";

#define BUTTON_IN(x,y) if(wcscmp(x,y)==0)

class CathyClass
{
public:
	BOOL InitClass(HWND Hwnd)
	{

		hInstance = GetModuleHandle(0);
		hWnd = Hwnd;
		CurButton = 0;
		CurFrame = 0;
		CurCheck = 0;
		CurLine = 0;
		CurText = 0;
		CurWnd = 1;
		DPI = 1;
		EnableShadow = false;
		//WM_DISPLAYCHANGE

		//double PScreenW, PScreenH, ScreenW, ScreenH,t;
		//ScreenW = GetSystemMetrics(SM_CXSCREEN);
		//ScreenH = GetSystemMetrics(SM_CYSCREEN);
		//PScreenW=GetDeviceCaps(GetDC(0), HORZSIZE);
		//PScreenH = GetDeviceCaps(GetDC(0), VERTSIZE);
		//t = ScreenH /( PScreenH/10)*2.54;//t=t/ 3.94;
		//DPI = t / 96;
		//if (DPI * 1300 > ScreenW)DPI = ScreenW / 1300;
		st = CreateFontW(16 * DPI, 8 * DPI, 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));

		CurCover = -1;
		CoverCheck = 0;
		ZeroMemory(Button, sizeof(Button));
		ZeroMemory(Frame, sizeof(Frame));
		ZeroMemory(Check, sizeof(Check));
		ZeroMemory(Line, sizeof(Line));
		ZeroMemory(Text, sizeof(Text));
		ZeroMemory(Edit, sizeof(Edit));
		return TRUE;
	}




	VOID CreateEdit(INT Left, INT Top, INT Width, INT Height, INT Page, LPCWSTR Name, LONG Defs, LONG ID, HFONT Font)
	{
		CurEdit++;
		Edit[CurEdit].Page = Page;
		Edit[CurEdit].Width = Width;
		Edit[CurEdit].Top = Top;
		Edit[CurEdit].Left = Left;
		Edit[CurEdit].Height = Height;
		LONG NewDefs = Defs | (ES_AUTOVSCROLL | WS_CHILD | WS_BORDER | DT_VCENTER | ES_CENTER | ES_MULTILINE);
		Edit[CurEdit].hWnd = CreateWindowW(L"EDIT", Name, NewDefs, Left*DPI, Top*DPI, Width*DPI, Height*DPI, hWnd, (HMENU)ID, hInstance, nullptr);
		::SendMessage(Edit[CurEdit].hWnd, WM_SETFONT, (WPARAM)Font, 1);
		RECT rc;
		GetClientRect(Edit[CurEdit].hWnd, &rc);
		OffsetRect(&rc, 0, (Edit[CurEdit].Height / 2 - 11)*DPI);
		::SendMessage(Edit[CurEdit].hWnd, EM_SETRECT, 0, (LPARAM)&rc);
		if (CurWnd == Page || Page == 0)ShowWindow(Edit[CurEdit].hWnd, SW_SHOW);
	}

	VOID CreateButtonEx(INT Number, INT Left, INT Top, INT Width, INT Height, INT Page, LPCWSTR Name, HBRUSH Leave, \
		HBRUSH Hover, HBRUSH Press, HPEN Leave2, HPEN Hover2, HPEN Press2, HFONT Font, BOOL Enabled, BOOL Visible, COLORREF FontRGB, LPCWSTR ID)
	{
		Button[Number].Left = Left;
		Button[Number].Top = Top;
		Button[Number].Width = Width;
		Button[Number].Height = Height;
		Button[Number].Page = Page;
		Button[Number].Leave = Leave;
		Button[Number].Hover = Hover;
		Button[Number].Press = Press;
		Button[Number].Leave2 = Leave2;
		Button[Number].Hover2 = Hover2;
		Button[Number].Press2 = Press2;
		Button[Number].Font = Font;
		Button[Number].Enabled = Enabled;
		Button[Number].Visible = Visible;
		Button[Number].FontRGB = FontRGB;
		wcscpy_s(Button[Number].Name, Name);
		wcscpy_s(Button[Number].ID, ID);
	}
	VOID CreateButton(INT Left, INT Top, INT Width, INT Height, INT Page, LPCWSTR Name, LPCWSTR ID)
	{
		HFONT st = CreateFontW(16, 8, 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
		CurButton++;
		CreateButtonEx(CurButton, Left, Top, Width, Height, Page, Name, WhiteBrush, DBlueBrush, LBlueBrush, BLACK, BLACK, BLACK, st, TRUE, TRUE, RGB(0, 0, 0), ID);
	}

	VOID CreateFrame(INT Left, INT Top, INT Width, INT Height, INT Page, LPCWSTR Name)
	{
		CurFrame++;
		Frame[CurFrame].Left = Left;
		Frame[CurFrame].Page = Page;
		Frame[CurFrame].Height = Height;
		Frame[CurFrame].Top = Top;
		Frame[CurFrame].Width = Width;
		wcscpy_s(Frame[CurFrame].Name, Name);
	}

	VOID CreateCheck(INT Left, INT Top, INT Page, INT Width, LPCWSTR Name)
	{
		CurCheck++;
		Check[CurCheck].Left = Left;
		Check[CurCheck].Top = Top;
		Check[CurCheck].Page = Page;
		Check[CurCheck].Width = Width;
		wcscpy_s(Check[CurCheck].Name, Name);
	}

	VOID CreateText(INT Left, INT Top, INT Page, LPCWSTR Name, COLORREF rgb)
	{
		CurText++;
		Text[CurText].Left = Left;
		Text[CurText].Top = Top;
		Text[CurText].Page = Page;
		Text[CurText].rgb = rgb;
		wcscpy_s(Text[CurText].Name, Name);
	}

	VOID CreateLine(INT StartX, INT StartY, INT EndX, INT EndY, INT Page)
	{
		CurLine++;
		Line[CurLine].StartX = StartX;
		Line[CurLine].StartY = StartY;
		Line[CurLine].EndX = EndX;
		Line[CurLine].EndY = EndY;
		Line[CurLine].Page = Page;
	}

	BOOL InsideButton(int bzz)
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		return (Button[bzz].Left*DPI <= point.x &&Button[bzz].Top*DPI <= point.y && (Button[bzz].Left + Button[bzz].Width)*DPI >= point.x && (Button[bzz].Top + Button[bzz].Height) *DPI >= point.y);
	}

	int InsideCheck(int minecraft)
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (Check[minecraft].Left*DPI <= point.x&&Check[minecraft].Top*DPI <= point.y&&Check[minecraft].Left*DPI + 15 * DPI + 1 >= point.x
			&&Check[minecraft].Top*DPI + 15 * DPI + 1 >= point.y)return 1;

		if (Check[minecraft].Left*DPI <= point.x&&Check[minecraft].Top*DPI <= point.y&&Check[minecraft].Left*DPI + Check[minecraft].Width*DPI >= point.x
			&&Check[minecraft].Top*DPI + 15 * DPI + 1 >= point.y)return 2;
		return 0;
	}
	void DrawFrames()
	{
		for (int i = 1; i <= CurFrame; i++)
		{
			if (Frame[i].Page == CurWnd || Frame[i].Page == 0)
			{
				HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

				SelectObject(hdc, pen);
				MoveToEx(hdc, Frame[i].Left*DPI, Frame[i].Top*DPI, NULL);
				LineTo(hdc, Frame[i].Left*DPI, Frame[i].Top*DPI + Frame[i].Height*DPI);
				MoveToEx(hdc, Frame[i].Left*DPI, Frame[i].Top*DPI, NULL);
				LineTo(hdc, Frame[i].Left*DPI + Frame[i].Width*DPI, Frame[i].Top*DPI);
				MoveToEx(hdc, Frame[i].Left*DPI + Frame[i].Width*DPI, Frame[i].Top*DPI + Frame[i].Height*DPI, NULL);
				LineTo(hdc, Frame[i].Left*DPI + Frame[i].Width*DPI, Frame[i].Top*DPI - 1);
				MoveToEx(hdc, Frame[i].Left*DPI + Frame[i].Width*DPI, Frame[i].Top*DPI + Frame[i].Height*DPI, NULL);
				LineTo(hdc, Frame[i].Left*DPI - 1, Frame[i].Top *DPI + Frame[i].Height*DPI);
				SetTextColor(hdc, Frame[i].rgb);
				RECT rc = GetRECTf(i);
				SetBkMode(hdc, OPAQUE);
				DrawTextW(hdc, Frame[i].Name, (int)wcslen(Frame[i].Name), &rc, NULL);
				SetTextColor(hdc, NULL);
			}
		}

	}
	void DrawButtons()
	{
		for (int i = 1; i <= CurButton; i++)
		{
			if (Button[i].Page == CurWnd || Button[i].Page == 0)// || (GameMode&& zxf[i].cur == 6))
			{
				SetTextColor(hdc, Button[i].FontRGB);
				if (CurCover == i)
					if (Press == 1)
					{
						SelectObject(hdc, Button[i].Press);
						SelectObject(hdc, Button[i].Press2);
					}
					else
					{
						SelectObject(hdc, Button[i].Hover);
						SelectObject(hdc, Button[i].Hover2);
					}
				else
				{
					SelectObject(hdc, Button[i].Leave);
					SelectObject(hdc, Button[i].Leave2);
				}
				SelectObject(hdc, Button[i].Font);
				if (Button[i].Download != 0)SelectObject(hdc, Button[i].Leave), SelectObject(hdc, Button[i].Leave2);
				Rectangle(hdc, Button[i].Left*DPI, Button[i].Top*DPI, Button[i].Left*DPI + Button[i].Width*DPI, Button[i].Top*DPI + Button[i].Height*DPI);
				if (Button[i].Download != 0)
				{
					SelectObject(hdc, Button[i].Hover);
					Rectangle(hdc, Button[i].Left*DPI, Button[i].Top*DPI, Button[i].Left*DPI + Button[i].Width*DPI*(Button[i].Download - 1) / 100, Button[i].Top*DPI + Button[i].Height*DPI);
				}

				RECT rc = GetRECT(i);

				SetBkMode(hdc, TRANSPARENT);//去掉文字背景
				if (Button[i].Download == 0)
					DrawTextW(hdc, Button[i].Name, (int)wcslen(Button[i].Name), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				else
				{
					if (Button[i].Download == 101)
					{
						DrawTextW(hdc, L"下载成功", 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						Button[i].Download = 0;
					}
					else
						DrawTextW(hdc, L"正在下载", 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}

		}
		SetTextColor(hdc, RGB(0, 0, 0));
	}
	void DrawChecks()
	{
		for (int i = 1; i <= CurCheck; i++)
			if (Check[i].Page == 0 || Check[i].Page == CurWnd)
			{
				if (i == CoverCheck)SelectObject(hdc, BLUE); else SelectObject(hdc, LGREY);

				SelectObject(hdc, grey);
				Rectangle(hdc, Check[i].Left*DPI, Check[i].Top*DPI, Check[i].Left*DPI + 15 * DPI, Check[i].Top*DPI + 15 * DPI);
				TextOut(hdc, Check[i].Left*DPI + 20 * DPI, Check[i].Top*DPI, Check[i].Name, (int)wcslen(Check[i].Name));
				if (Check[i].Value == 1)//打勾
				{
					SelectObject(hdc, GREEN);
					MoveToEx(hdc, Check[i].Left*DPI + 2 * DPI, Check[i].Top*DPI + 7 * DPI, NULL);
					LineTo(hdc, Check[i].Left*DPI + 7 * DPI, Check[i].Top*DPI + 12 * DPI);
					MoveToEx(hdc, Check[i].Left *DPI + 7 * DPI, Check[i].Top*DPI + 12 * DPI, NULL);
					LineTo(hdc, Check[i].Left*DPI + 12 * DPI, Check[i].Top*DPI + 2 * DPI);
				}
			}
	}
	void DrawLines()
	{
		for (int i = 1; i <= CurLine; i++)
		{
			if (Line[i].Page == 0 || Line[i].Page == CurWnd)
			{
				SelectObject(hdc, BLACK);
				MoveToEx(hdc, Line[i].StartX*DPI, Line[i].StartY*DPI, NULL);
				LineTo(hdc, Line[i].EndX*DPI, Line[i].EndY*DPI);
			}
		}
	}
	void DrawTexts()
	{
		for (int i = 1; i <= CurText; i++)
		{
			if (Text[i].Page == 0 || Text[i].Page == CurWnd)
			{
				SetTextColor(hdc, Text[i].rgb);
				TextOutW(hdc, Text[i].Left*DPI, Text[i].Top*DPI, Text[i].Name, (int)wcslen(Text[i].Name));
			}
		}
	}
	void DrawEVERYTHING()
	{
		DrawFrames();
		DrawButtons();
		DrawChecks();
		DrawLines();
		DrawTexts();
	}
	RECT GetRECT(int csp)//更新rc
	{
		RECT rc;
		rc.left = Button[csp].Left*DPI;
		rc.right = Button[csp].Left*DPI + Button[csp].Width*DPI;
		rc.top = Button[csp].Top*DPI;
		rc.bottom = Button[csp].Top*DPI + Button[csp].Height*DPI;
		return rc;
	}

	RECT GetRECTf(int djl)
	{
		RECT rc;
		rc.left = Frame[djl].Left*DPI + 10 * DPI;
		rc.right = Frame[djl].Left*DPI + Frame[djl].Width*DPI;
		rc.top = Frame[djl].Top*DPI - 7 * DPI;
		rc.bottom = Frame[djl].Top*DPI + 30 * DPI;
		return rc;
	}
	int Msv;
	VOID LeftButtonDown()
	{
		if (CurCover != -1)
		{
			Press = 1;
			RECT rc;
			rc = GetRECT(CurCover);
			InvalidateRect(hWnd, &rc, FALSE);
		}
	}

	VOID MouseMove()
	{

		if (CurCover == -1 && Button[CurCover].Download == 0)//在外面
		{
			for (int i = 0; i <= CurButton; i++)
			{
				if (Button[i].Page == CurWnd || Button[i].Page == 0)
				{
					if (InsideButton(i))
					{
						CurCover = i;
						RECT rc;
						rc = GetRECT(i);
						InvalidateRect(hWnd, &rc, FALSE);
						break;
					}
				}
			}
		}
		if (CurCover >= 0 && Button[CurCover].Download == 0)
		{
			if (Button[CurCover].Page == CurWnd || Button[CurCover].Page == 0)
			{
				if (!InsideButton(CurCover))
				{
					RECT rc;
					rc = GetRECT(CurCover);
					CurCover = -1;
					InvalidateRect(hWnd, &rc, FALSE);
				}
			}
			else
			{
				RECT rc;
				rc = GetRECT(CurCover);
				CurCover = -1;
				InvalidateRect(hWnd, &rc, FALSE);
			}
		}
		if (CoverCheck == 0)//在外面
		{
			for (int i = 1; i <= CurCheck; i++)
			{
				if (Check[i].Page == CurWnd || Check[i].Page == 0)
				{
					if (InsideCheck(i) != 0)
					{
						RECT rc;
						CoverCheck = i;
						rc.left = Check[i].Left*DPI - 1;
						rc.top = Check[i].Top*DPI - 1;
						rc.right = Check[i].Left *DPI + 15 * DPI + 1;
						rc.bottom = Check[i].Top *DPI + 15 * DPI + 1;
						InvalidateRect(hWnd, &rc, FALSE);
						break;
					}
				}
			}
		}

		if (CoverCheck > 0)
		{
			if (Check[CoverCheck].Page == CurWnd || Check[CoverCheck].Page == 0)
			{
				if (InsideCheck(CoverCheck) == 0 && Button[CoverCheck].Download == 0)
				{
					RECT rc;
					rc.left = Check[CoverCheck].Left*DPI - 1 * DPI;
					rc.top = Check[CoverCheck].Top*DPI - 1 * DPI;
					rc.right = Check[CoverCheck].Left*DPI + 15 * DPI + 1;
					rc.bottom = Check[CoverCheck].Top *DPI + 15 * DPI + 1;
					CoverCheck = 0;
					InvalidateRect(hWnd, &rc, FALSE);
				}
			}
			else
			{
				RECT rc;
				rc.left = Check[CoverCheck].Left - 1 * DPI;
				rc.top = Check[CoverCheck].Top - 1 * DPI;
				rc.right = Check[CoverCheck].Left + Check[CoverCheck].Width;
				rc.bottom = Check[CoverCheck].Top + 15 * DPI + 1;
			}
		}
		if (Msv == 0)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hWnd;
			tme.dwFlags = TME_LEAVE;
			TrackMouseEvent(&tme);
			Msv = 1;
		}
		else {
			Msv = 0;
		}
	}
	VOID SetPage(INT Page)
	{
		if (Page == CurWnd)return;
		CurWnd = Page;
		for (int i = 1; i <= CurEdit; i++)
		{
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)ShowWindow(Edit[i].hWnd, SW_SHOW); else ShowWindow(Edit[i].hWnd, SW_HIDE);
		}
		InvalidateRect(hWnd, 0, FALSE);
	}
	VOID SetDPI(DOUBLE NewDPI)
	{
		DPI = NewDPI;
		st = CreateFontW(16 * DPI, 8 * DPI, 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
		SetWindowPos(hWnd, NULL, 0, 0, Width * DPI, Height* DPI, SWP_NOMOVE);
		for (int i = 1; i <= CurEdit; i++)
		{
			SetWindowPos(Edit[i].hWnd, NULL, Edit[i].Left * DPI, Edit[i].Top*DPI, Edit[i].Width * DPI, Edit[i].Height* DPI, SWP_NOCOPYBITS);
			::SendMessage(Edit[i].hWnd, WM_SETFONT, (WPARAM)st, 1);
			RECT rc;
			GetClientRect(Edit[i].hWnd, &rc);
			OffsetRect(&rc, 0, (Edit[i].Height / 2 - 11)*DPI);
			::SendMessage(Edit[i].hWnd, EM_SETRECT, 0, (LPARAM)&rc);
		}
	}

	VOID CreateMain(INT Left, INT Top, INT Wid, INT Hei, LPCWSTR Name, LONG Defs)
	{
		Width = Wid;
		Height = Hei;
		hWnd = CreateWindowW(szWindowClass, Name, WS_OVERLAPPEDWINDOW, Left, Top, Wid * DPI, Hei * DPI, hWndParent, nullptr, hInstance, nullptr);
	}

	LPCWSTR GetCurInsideID()
	{
		for (int i = 0; i <= CurButton; i++)
			if (Button[i].Page == CurWnd || Button[i].Page == 0)
				if (InsideButton(i) && Button[i].Download == 0)
					return Button[i].ID;
		return Button[0].ID;
	}
	INT GetNumbyID(LPCWSTR ID)
	{
		for (int i = 0; i <= CurButton; i++)if (wcscmp(Button[i].ID, ID) == 0)return i;
	}
	struct ButtonEx
	{
		int Left, Top, Width, Height, Page, Download;
		bool Visible, Enabled;
		HBRUSH Leave, Hover, Press;
		HPEN Leave2, Hover2, Press2;
		HFONT Font;
		wchar_t Name[31], ID[11];
		COLORREF FontRGB;
	}Button[100];

	struct FrameEx
	{
		int Left, Top, Width, Height, Page;
		COLORREF rgb;
		wchar_t Name[31];
	}Frame[20];
	struct CheckEx
	{
		int Left, Top, Page, Width;//width跟绘制没什么关系，见Insidec函数
		bool Value;
		wchar_t Name[31];
	}Check[20];
	struct LineEx
	{
		int StartX, StartY, EndX, EndY, Page;
	}Line[20];

	struct TextEx
	{
		int Left, Top, Page;
		COLORREF rgb;
		wchar_t Name[201];
	}Text[20];

	struct EditEx
	{
		int Left, Top, Width, Height, Page;
		HWND hWnd;
		long defs;
	}Edit[20];
	int CurButton;
	int CurFrame;
	int CurCheck;
	int CurLine;
	int CurText;
	int CurEdit;
	double DPI;
	int CurCover;
	int CoverCheck;
	int CurWnd;
	int Press;
	HDC hdc;
	bool EnableShadow;
	int Width, Height;
	HWND hWnd;
	HINSTANCE hInstance;
private:

};

CathyClass Main, CatchWnd;






int top;//是否置顶
int AlwaysHide;//永久隐藏
int BOS;//Big or Small
int wid, hei;

int GameExist[100];
DWORD expid[100];


HANDLE ShellCreateInVDesk(PTSTR szName)
{
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.lpDesktop = szVDesk;
	PROCESS_INFORMATION pi;
	if (!CreateProcess(NULL, szName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(NULL, L"启动桌面失败", L"提示", MB_ICONINFORMATION);
		ExitProcess(1);
	}
	return pi.hProcess;
}


void Catchi()
{
	int width = GetSystemMetrics(SM_CXSCREEN), heigth = GetSystemMetrics(SM_CYSCREEN);
	HDC hdcWindow = GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hdcWindow, BITSPIXEL);
	CImage image;
	image.Create(width, heigth, nBitPerPixel);
	BitBlt(image.GetDC(), 0, 0, width, heigth, hdcWindow, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hdcWindow);
	image.ReleaseDC();
	image.Save(L"D:\\1.bmp");
}



class DownloadProgress : public IBindStatusCallback {
public:
	int curi;
	HRESULT __stdcall QueryInterface(const IID &, void **) { return E_NOINTERFACE; }
	ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }//暂时没用的函数，从msdn上抄下来的
	ULONG STDMETHODCALLTYPE Release(void) { return 1; }
	HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, IBinding *pib) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG *pnPriority) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, LPCWSTR szError) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID riid, IUnknown *punk) { return E_NOTIMPL; }

	virtual HRESULT __stdcall OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		if (ulProgressMax != 0)
		{
			double percentage = double(ulProgress * 1.0 / ulProgressMax * 100);
			if (Main.Button[curi].Download != (int)percentage + 1)
			{
				Main.Button[curi].Download = (int)percentage + 1;
				RECT rc = Main.GetRECT(curi);
				InvalidateRect(Main.hWnd, &rc, FALSE);
			}
		}
		return S_OK;
	}
};


BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	ULONG nProcessID; HANDLE hProcessHandle;
	LONG A;
	A = GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;

	if (A != 0)//可能会结束EXPLORER
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		if (mypid == nProcessID)goto a;
		for (int i = 0; i <= 100; i++)if (expid[i] == nProcessID)goto a;
		hProcessHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, nProcessID);
		::TerminateProcess(hProcessHandle, 4);
	}
a:
	return 1;
}

void KillTop()
{
	ZeroMemory(&expid, sizeof(expid));
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); int Cur = 0;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	while (Process32Next(hSnapShot, &pe))
	{
		if (pe.szExeFile[0] == 'e' &&pe.szExeFile[1] == 'x'&&pe.szExeFile[2] == 'p')
		{
			expid[Cur] = pe.th32ProcessID;
			Cur++;
		}
	}
	EnumWindows(lpEnumFunc, NULL);
}


BOOL KillProcess(LPCWSTR ProcessName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;

	while (Process32Next(hSnapShot, &pe))
	{
		int name = 0;

		if (towupper(pe.szExeFile[0]) == towupper(ProcessName[0]))name++;
		if (towupper(pe.szExeFile[1]) == towupper(ProcessName[1]))name++;
		if (towupper(pe.szExeFile[2]) == towupper(ProcessName[2]))name++;

		if (name == 3)
		{
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, TRUE, dwProcessID);
			::TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
	}
	return TRUE;
}

void GetPath()//得到两个路径
{
	GetModuleFileName(NULL, Path, MAX_PATH);
	GetModuleFileName(NULL, Name, MAX_PATH);
	(_tcsrchr(Path, _T('\\')))[1] = 0;
	OutputDebugString(L"1");
}


VOID GetInfo(__out LPSYSTEM_INFO lpSystemInfo)//得到环境变量
{
	if (NULL == lpSystemInfo)return;
	typedef VOID(WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");;
	if (NULL != fnGetNativeSystemInfo)fnGetNativeSystemInfo(lpSystemInfo); else GetSystemInfo(lpSystemInfo);
}

VOID GetBit()//系统位数
{
	SYSTEM_INFO si;
	GetInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) Bit = 64; else Bit = 32;
	//32 means 32bit GUI on 32bit System
	//34 means 32bit GUI on 64bit System
	//64 means 64bit GUI on 64bit System
	if (sizeof(int*) * 8 == 32 && Bit == 64)Bit = 34;
}

VOID Backup()
{
	CreateDirectory(L"C:\\SAtemp", NULL);
	if (Bit != 34)
		CopyFile(_T("C:\\Windows\\System32\\sethc.exe"), _T("C:\\SAtemp\\sethc.exe"), TRUE);//都要双斜线...
	else
		CopyFile(_T("C:\\Windows\\SysNative\\sethc.exe"), _T("C:\\SAtemp\\sethc.exe"), TRUE);//sysnative
}


VOID DeleteSethc()
{
	char tmp[100];
	if (Bit != 34)
		strcpy_s(tmp, "sc config TrustedInstaller binPath= \"cmd.exe /c del /f C:\\Windows\\system32\\sethc.exe\"");
	else
		strcpy_s(tmp, "sc config TrustedInstaller binPath= \"cmd.exe /c del /f C:\\Windows\\sysnative\\sethc.exe\"");

	if (_waccess(SethcPath, 0) == 0)
	{
		WinExec(tmp, SW_HIDE);
		WinExec("net start TrustedInstaller", SW_HIDE);
		if (Bit != 34)
			DeleteFile(_T("C:\\Windows\\System32\\Sethc.exe"));
		else
			DeleteFile(_T("C:\\Windows\\SysNative\\Sethc.exe"));
	}
}

VOID SwitchLanguage(LPWSTR Name)
{
	bool Mainf = false, Catchf = false, Buttonf = false, Checkf = false;
	wchar_t ReadTmp[1001];
	FILE *fp;
	_wfopen_s(&fp, L"D:\\1.ini",L"r,ccs=UNICODE");
	if (fp == NULL)s(1);
	while (!feof(fp))
	{
		fgetws(ReadTmp, 1000, fp); 
		if (wcsstr(ReadTmp, L"<Main>") != 0)Mainf=true;
		if (wcsstr(ReadTmp, L"</Main>") != 0)Mainf = false;
		if(wcsstr(ReadTmp, L"<Buttons>") != 0)Buttonf = true;
		if (wcsstr(ReadTmp, L"</Buttons>") != 0)Buttonf = false;
		if (ReadTmp[0] != '<')
		{
			wchar_t *pos = wcsstr(ReadTmp, L"=");
			pos[0] = '\0';
			wchar_t *space= wcsstr(ReadTmp, L" ");
			if (space != 0)space[0] = '\0';
			if (Buttonf == true && Mainf == true)
			{
				//s(Main.Button[1].Name[2]);
				for (int i = 1; i <= Main.CurButton; ++i)
				{
					if (wcscmp(ReadTmp, Main.Button[i].ID) == 0)
					{
						wchar_t *str1= wcsstr(&pos[1], L"\"");
						wchar_t *str2 = wcsstr(&str1[1], L"\"");
						str2[0] = '\0';
						wcscpy_s(Main.Button[i].Name,&str1[1]);
						str1= wcsstr(&str2[1], L",");
						str2 = wcsstr(&str1[1], L",");
						str2[0] = '\0';
						int NewLeft = _wtoi(&str1[1]);
						str1 = wcsstr(&str2[1], L",");
						str1[0] = '\0';
						int NewTop = _wtoi(&str2[1]);
						str2 = wcsstr(&str1[1], L",");
						str2[0] = '\0';
						int NewWidth = _wtoi(&str1[1]);
						int NewHeight = _wtoi(&str2[1]);
						if(NewLeft!=-1)Main.Button[i].Left = NewLeft;
						if (NewTop != -1)Main.Button[i].Top = NewTop;
						if (NewWidth != -1)Main.Button[i].Width = NewWidth;
						if (NewHeight != -1)Main.Button[i].Height = NewHeight;
						break;
					}
				}
			}
			InvalidateRect(Main.hWnd, 0, false);
			//s(pos);
		}
		//s(ReadTmp);
	}
}
VOID SetupSethc()
{
	//if (_waccess(SethcPath, 0) == 0)MessageBox(main, L"1", NULL, NULL);//0是文件存在
	wcscpy_s(Gutc, Path);
	LPCWSTR sethc = L"sethc.exe";
	wcscat_s(Gutc, sethc);

	CopyFile(Gutc, _T("C:\\Windows\\System32\\sethc.exe"), false);
	if (Bit != 34)
		CopyFile(Gutc, _T("C:\\Windows\\System32\\sethc.exe"), false);
	else
		CopyFile(Gutc, _T("C:\\Windows\\SysNative\\sethc.exe"), false);
	//if (_waccess(SethcPath, 0) != 0)CathyageBox(L"失败");
}

VOID DeleteSethcS()//可以压代码
{
	DeleteSethc();
	if (Bit == 32)
	{
		//删除sethc
		WinExec("net stop deletefile", SW_HIDE);
		WinExec("sc delete deletefile", SW_HIDE);
		LPCWSTR drv1 = L"x32\\deletefile.sys";//64位的deletefile坏了
		wcscpy_s(Gutc, Path);
		wcscat_s(Gutc, drv1);
		LPCWSTR drv2 = L" type=kernel";
		TCHAR drv3[200] = L"sc create deletefile binpath=";
		wcscat_s(drv3, Gutc);
		wcscat_s(drv3, drv2);

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		CreateProcessW(NULL, drv3, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WinExec("net start deletefile", SW_HIDE);

		//kill
		WinExec("net stop kill", SW_HIDE);
		WinExec("sc delete kill", SW_HIDE);
		drv1 = L"x32\\kill.sys";
		wcscpy_s(Gutc, Path);
		wcscat_s(Gutc, drv1);
		drv2 = L" type=kernel";
		TCHAR drv4[200] = L"sc create kill binpath=";
		wcscat_s(drv4, Gutc);
		wcscat_s(drv4, drv2);
		//CathyageBox(drv4);
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		CreateProcessW(NULL, drv4, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	}
	else
	{
		//删除sethc
		WinExec("net stop deletefile", SW_HIDE);
		WinExec("sc delete deletefile", SW_HIDE);
		LPCWSTR drv1 = L"x64\\deletefile.sys";//64位的deletefile坏了
		wcscpy_s(Gutc, Path);
		wcscat_s(Gutc, drv1);
		LPCWSTR drv2 = L" type=kernel";
		TCHAR drv3[200] = L"sc create deletefile binpath=";
		wcscat_s(drv3, Gutc);
		wcscat_s(drv3, drv2);

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		CreateProcessW(NULL, drv3, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WinExec("net start deletefile", SW_HIDE);

		//kill
		WinExec("net stop kill", SW_HIDE);
		WinExec("sc delete kill", SW_HIDE);
		drv1 = L"x64\\kill.sys";
		wcscpy_s(Gutc, Path);
		wcscat_s(Gutc, drv1);
		drv2 = L" type=kernel";
		TCHAR drv4[200] = L"sc create kill binpath=";
		wcscat_s(drv4, Gutc);
		wcscat_s(drv4, drv2);
		//CathyageBox(drv4);
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		CreateProcessW(NULL, drv4, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	}
}

VOID CopySethcS()
{
	wcscpy_s(Gutc, Path);
	LPCWSTR sethc = L"x32\\sethc.exe";
	wcscat_s(Gutc, sethc);
	if (Bit != 34)
		CopyFile(Gutc, _T("C:\\Windows\\System32\\sethc.exe"), false);
	else
		CopyFile(Gutc, _T("C:\\Windows\\SysNative\\sethc.exe"), false);
}
VOID CopyNTSD()
{
	wcscpy_s(Gutc, Path);
	LPCWSTR sethc = L"ntsd.exe";
	wcscat_s(Gutc, sethc);
	if (Bit != 34)
		CopyFile(Gutc, _T("C:\\Windows\\System32\\ntsd.exe"), false);
	else
		CopyFile(Gutc, _T("C:\\Windows\\SysNative\\ntsd.exe"), false);
}








LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam; // 获取按键消息
	if (nCode >= 0)
	{
		// WM_SYSKEYDOWN WM_SYSKEYUP
		if (p->vkCode == 0xA2)if (wParam == WM_KEYUP)Ctrl = 0; else Ctrl = 1;
		//if (Ctrl == 1 && wParam == WM_KEYUP)if (p->vkCode == 0x52)MessageBox(NULL, L"R", L"      ", 0);
	}
	//return 1;//禁止按键
	return CallNextHookEx(myhook, nCode, wParam, lParam);
}

DWORD WINAPI ThreadFun(LPVOID pM)
{
	while (top == 1)
		SetWindowPos(Main.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
	//SwitchToThisWindow(Main.hWnd,true);
	SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	return 0;
}
DWORD WINAPI DownloadThread(LPVOID pM)
{
	int *tp = (int *)pM;
	DownloadProgress progress;
	if (*tp == 1)
	{
		progress.curi = 42;
		URLDownloadToFileW(NULL, L"http://dl.360safe.com/pclianmeng/n/1__3112522__3f7372633d6c6d266c733d6e33366163663466393961__68616f2e3336302e636e__0cd2.exe", L"e:\\340.exe", 0, &progress);
	}
	return 0;
}
//
//COLORREF DoSelectColour(HWND hwnd)
//{
//	CHOOSECOLOR cc = { sizeof(CHOOSECOLOR) };
//	cc.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
//	cc.hwndOwner = hwnd;
//	cc.rgbResult = g_rgbBackground;
//	cc.lpCustColors = g_rgbCustom;
//	if (ChooseColor(&cc))
//	{
//		g_rgbBackground = cc.rgbResult;
//	}
//	return cc.rgbResult;
//}


BOOL CALLBACK CathyThread(HWND hwnd, LPARAM lParam)
{
	ULONG nProcessID;// HANDLE hProcessHandle;
	if (GetParent(hwnd) == 0)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		for (int i = 1; i <= curpid; i++)
			if (Eatpid[i] == nProcessID)
			{
				cureat++;
				Eatlist[cureat] = hwnd;
				//OpenProcess()
				//SetWindowsHookEx()
				//OpenProcessToken()
				//long tst=GetWindowLong(hwnd, GWL_EXSTYLE);
				//s(tst);
				//WS_CHILD
				//ShowWindow(hwnd, SW_HIDE);
				//tst = tst &~WS_EX_TOPMOST;

				//SetWindowLong(hwnd,GWL_EXSTYLE, tst);
				//OpenProcess(PROCESS_ALL_ACCESS, false, nProcessID);
				//ShowWindow(hwnd, SW_HIDE);
				SetParent(hwnd, Catch);
				//SetWindowPos(hwnd, HWND_NOTOPMOST, 100, 100, 0, 0, SWP_NOSIZE);
			}
	}
	return 1;
}

VOID ReturnWindows()
{
	for (int i = 1; i <= cureat; ++i)SetParent(Eatlist[i], NULL);
	cureat = 0;
	InvalidateRect(Catch, NULL, FALSE);
}

int Cathy()
{
	GetWindowTextW(CatchWnd.Edit[1].hWnd, text1, 255);
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	curpid = 0;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return 1;

	while (Process32Next(hSnapShot, &pe))
	{
		int name = 0;
		if (towupper(text1[0]) == towupper(pe.szExeFile[0]))name++;
		if (towupper(text1[1]) == towupper(pe.szExeFile[1]))name++;
		if (towupper(text1[2]) == towupper(pe.szExeFile[2]))name++;
		if (name == 3)
		{
			curpid++;
			Eatpid[curpid] = pe.th32ProcessID;
		}

		TCHAR T[10];
		_itow_s(pe.th32ProcessID, T, 10);
		//if (name == 3)CathyageBox(T);
	}
	EnumWindows(CathyThread, NULL);
	return 0;
}
int cg;
wchar_t CG[10][10] = { L"AnswerKey",L"Left" ,L"Right",L"Down",L"Up",L"In",L"On",L"Back",L"Front",L"Teacher" };
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	switch (nTimerid)
	{
	case 1:
		//PostMessage(main, WM_MOUSEMOVE, NULL, 0);
		//SetWindowPos(main, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
		if (Main.Check[8].Value == 1)
		{
			GetWindowText(Main.Edit[5].hWnd, text1, 255);
			KillProcess(text1);
		}
		if (Main.Check[9].Value == 1)SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
		break;
	case 2:
		timerleft--;
		if (timerleft >= 0)
		{
			ZeroMemory(&A, sizeof(A));
			wcscat_s(A, L"剩余 ");
			_itow_s(timerleft, Num, 10);
			wcscat_s(A, Num);
			wcscat_s(A, L" 秒");
			wcscpy_s(CatchWnd.Button[1].Name, (LPCWSTR)A);
			//s(A);
			InvalidateRect(Catch, NULL, FALSE);
			if (timerleft == 0)Cathy();
		}
		if (timerleft == -1)wcscpy_s(CatchWnd.Button[1].Name, zxfback), InvalidateRect(Catch, NULL, FALSE);
		break;
	case 3:
		cg++;
		wchar_t xf[101];
		wcscpy_s(xf, L"Copy");
		wcscat_s(xf, CG[cg % 10]);
		wcscat_s(xf, L"(c) SA软件 2015 - 2018");
		wcscpy_s(Main.Text[9].Name, xf);
		InvalidateRect(Main.hWnd, 0, false);
		break;
	}

}

void s(LPCWSTR cathy)
{
	MessageBox(NULL, cathy, L"", NULL);
}


VOID SDesktop()
{
	if (hCurrentDesk == defaultDesk)
	{
		SetThreadDesktop(hVirtualDesk);
		SwitchDesktop(hVirtualDesk);
		hCurrentDesk = hVirtualDesk;
	}
	else
	{
		SetThreadDesktop(defaultDesk);
		SwitchDesktop(defaultDesk);
		hCurrentDesk = defaultDesk;
	}
}
BOOL CDesktop()
{
	HDESK       hdeskCurrent;
	HDESK       hdesk;
	HWINSTA   hwinstaCurrent;
	HWINSTA   hwinsta;
	//     
	//   Save   the   current   Window   station   
	//     
	hwinstaCurrent = GetProcessWindowStation();
	if (hwinstaCurrent == NULL)
		return   FALSE;
	//     
	//   Save   the   current   desktop   
	//     
	hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
	if (hdeskCurrent == NULL)
		return   FALSE;
	//     
	//   Obtain   a   handle   to   WinSta0   -   service   must   be   running   
	//   in   the   LocalSystem   account   
	//     
	hwinsta = OpenWindowStationA("winsta0", FALSE,
		WINSTA_ACCESSCLIPBOARD |
		WINSTA_ACCESSGLOBALATOMS |
		WINSTA_CREATEDESKTOP |
		WINSTA_ENUMDESKTOPS |
		WINSTA_ENUMERATE |
		WINSTA_EXITWINDOWS |
		WINSTA_READATTRIBUTES |
		WINSTA_READSCREEN |
		WINSTA_WRITEATTRIBUTES);
	if (hwinsta == NULL)
		return   FALSE;
	//     
	//   Set   the   windowstation   to   be   winsta0   
	//     
	if (!SetProcessWindowStation(hwinsta))
		return   FALSE;

	//     
	//   Get   the   default   desktop   on   winsta0   
	//     
	hdesk = OpenDesktopA("Winlogon", 0, FALSE,
		DESKTOP_CREATEMENU |
		DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE |
		DESKTOP_HOOKCONTROL |
		DESKTOP_JOURNALPLAYBACK |
		DESKTOP_JOURNALRECORD |
		DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP |
		DESKTOP_WRITEOBJECTS);
	if (hdesk == NULL)
		return   FALSE;

	//     
	//   Set   the   desktop   to   be   "default"   
	//     
	if (!SetThreadDesktop(hdesk))
		return   FALSE;
	/*==================================执行体,显示窗口，做你想做的事情
	=============================*/
	//ServiceSub();

	//     
	//   Reset   the   Window   station   and   desktop   
	//     
	if (!SetProcessWindowStation(hwinstaCurrent))
		return   FALSE;

	if (!SetThreadDesktop(hdeskCurrent))
		return   FALSE;

	//     
	//   Close   the   windowstation   and   desktop   handles   
	//     
	if (!CloseWindowStation(hwinsta))
		return   FALSE;
	if (!CloseDesktop(hdesk))
		return   FALSE;
	return   TRUE;


}

void openfile()
{
	OPENFILENAME ofn;
	char strFile[MAX_PATH];
	memset(&ofn, 0, sizeof(OPENFILENAME));
	memset(strFile, 0, sizeof(char)*MAX_PATH);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = L"*.*";
	ofn.lpstrFile = (LPWSTR)strFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileNameW(&ofn) == TRUE)SetWindowText(Main.Edit[4].hWnd, (LPCWSTR)strFile);

}

void BrowseFolder()
{
	TCHAR path[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = L"打开文件夹";
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		SHGetPathFromIDList(pidl, path);
		SetCurrentDirectory(path);
		SetWindowText(Main.Edit[4].hWnd, path);
	}
}

VOID SearchTool(LPCWSTR lpPath, int type)//1 打开极域 2 删除shutdown
{
	wchar_t szFind[255], szFile[255];
	WIN32_FIND_DATA FindFileData;
	wcscpy_s(szFind, lpPath);
	wcscat_s(szFind, L"*.*");
	HANDLE hFind = ::FindFirstFile((LPCWSTR)szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)return;
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				wcscpy_s(szFile, lpPath);
				wcscat_s(szFile, FindFileData.cFileName);
				wcscat_s(szFile, L"\\");
				SearchTool(szFile, type);
			}
		}
		else
		{
			_wcslwr_s(FindFileData.cFileName);
			if (type == 1)
			{
				if (wcsstr(FindFileData.cFileName, L"studentmain.exe") != NULL)//注意！ ————————————————————————----————————
				{										//旧版TD主程序叫stu+年份 —————————————————————————————————————-
														//FindFileData.cFileName;//注意识别——————————————————————————————————————————————————-
					ZeroMemory(&si, sizeof(si));
					ZeroMemory(&pi, sizeof(pi));
					wcscpy_s(szFile, lpPath);
					wcscat_s(szFile, FindFileData.cFileName);
					CreateProcess(NULL, szFile, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
				}
			}
			else
			{
				if (wcsstr(FindFileData.cFileName, L"shutdown.exe") != NULL)
				{
					wcscpy_s(szFile, lpPath);
					wcscat_s(szFile, FindFileData.cFileName);
					DeleteFile(szFile);
				}
			}
		}
		if (!FindNextFile(hFind, &FindFileData))    break;
	}
	FindClose(hFind);
}



VOID BSOD()
{
	WinExec("net stop BSOD", SW_HIDE);
	WinExec("sc delete BSOD", SW_HIDE);
	LPCWSTR drv1;
	if (Bit == 32)
		drv1 = L"x32\\BSOD.sys";
	else
		drv1 = L"x64\\BSOD.sys";

	wcscpy_s(Gutc, Path);
	wcscat_s(Gutc, drv1);
	LPCWSTR drv2 = L" type=kernel";
	TCHAR drv3[200] = L"sc create BSOD binpath=";
	wcscat_s(drv3, Gutc);
	wcscat_s(drv3, drv2);

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	CreateProcessW(NULL, drv3, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	wcscpy_s(drv3, L"net start BSOD");
	CreateProcessW(NULL, drv3, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	KillProcess(L"svc");
}


void ColorfulEggs(bool x)
{
	if (x)
		SetTimer(Main.hWnd, 3, 200, (TIMERPROC)TimerProc);
	else
		KillTimer(Main.hWnd, 3);
}
int __cdecl sub_430CD0(void *Src, LPCWSTR lpSubKey, LPCWSTR lpValueName)
{
	const WCHAR *v3;
	unsigned int v4, v5, v10, v12;
	BYTE *v6, *v7, v8, *v9, *v11, *i;
	const WCHAR *v14;
	LSTATUS v15;
	int result;
	HKEY phkResult;
	LPCWSTR lpSubKeya;
	v3 = lpSubKey;
	phkResult = 0;
	if (!lpSubKey)
		v3 = L"Software\\TopDomain\\e-Learning Class\\Student";
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, v3, 0, 0x20006u, &phkResult))
		goto LABEL_25;
	v4 = _time64(0);
	srand(v4);
	v5 = (rand() % 40 + 83) & 0xFFFFFFFC;
	v6 = (BYTE *)operator new[](v5);
	v7 = v6;
	if (v5 >> 1)
	{
		lpSubKeya = (LPCWSTR)(v5 >> 1);
		do
		{
			*(WORD *)v7 = rand();
			v7 += 2;
			lpSubKeya = (LPCWSTR)((char *)lpSubKeya - 1);
		} while (lpSubKeya);
	}
	v8 = rand() % (v5 - 68) + 2;
	*v6 = v8;
	v6[v5 - 1] = v8;
	if (Src && wcslen((const wchar_t *)Src))
		memcpy(&v6[v8], Src, 2 * wcslen((const wchar_t *)Src) + 2);
	else
	{
		v9 = &v6[v8];
		*v9 = 0;
		v9[1] = 0;
	}
	v10 = v5 >> 2;
	v11 = v6;
	if (v5 >> 2)
	{
		v12 = v5 >> 2;
		do
		{
			*(DWORD *)v11 ^= 0x454C4350u;
			v11 += 4;
			--v12;
		} while (v12);
	}
	for (i = v6; v10; --v10)
	{
		*(DWORD *)i ^= 0x50434C45u;
		i += 4;
	}
	v14 = lpValueName;
	if (!lpValueName)
		v14 = L"Knock1";
	v15 = RegSetValueExW(phkResult, v14, 0, 3u, v6, v5);
	operator delete[](v6);
	if (v15)
	{
	LABEL_25:
		if (phkResult)RegCloseKey(phkResult);
		result = 0;
	}
	else
	{
		RegCloseKey(phkResult);
		result = 1;
	}
	return result;
}

int ChangePasswordEx(int type)
{
	if (type == 1 || type == 2)
	{
		GetWindowTextW(Main.Edit[3].hWnd, text1, 255);
		if (text1[0] == 36755)
			if (MessageBox(Main.hWnd, L"确定要把密码改成\"输入密码\"么？", L"提示", MB_ICONINFORMATION | MB_YESNO) != IDYES)return 34;
		WCHAR Shell[3000];
		if (Bit == 64)
			wcscpy_s(Shell, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00\" /v UninstallPasswd /d \"");
		else
			wcscpy_s(Shell, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00\" /v UninstallPasswd /d \"");
		if (type == 2)wcscat_s(Shell, L"Passwd");
		wcscat_s(Shell, text1);
		wcscat_s(Shell, L"\" /t reg_sz /f");
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		CreateProcess(NULL, Shell, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		MessageBox(Main.hWnd, L"修改成功", L"提示", MB_ICONINFORMATION);

	}
	if (type == 3)sub_430CD0(NULL, NULL, NULL);
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,//入口点
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 初始化全局字符串
	//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadStringW(hInstance, IDC_GUI, szWindowClass, MAX_LOADSTRING);
	wcscpy_s(szWindowClass, L"GUI");

	GetPath();
	Admin = IsUserAnAdmin();
	//Admin = CopyFile(Name, _T("C:\\Test.exe"), false);

	DeleteFile(_T("C:\\Test.exe"));//较慢

	GetBit();
	if (Bit != 34)
		SethcPath = L"C:\\Windows\\System32\\sethc.exe";
	else
		SethcPath = L"C:\\Windows\\SysNative\\sethc.exe";

	defaultDesk = GetThreadDesktop(GetCurrentThreadId());//创建虚拟桌面
	hVirtualDesk = CreateDesktop(szVDesk, NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);
	TCHAR szProcess[] = TEXT("explorer");
	explorerInfo = ShellCreateInVDesk(szProcess);

	MyRegisterClass(hInstance);

	Backup();

	//top = 1;
	//CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);

	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUI));

	MSG msg;
	// 主消息循环: 
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		GetCursorPos(&point);
		ScreenToClient(Main.hWnd, &point);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


//注册窗口类。
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUI));//大图标
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUI));//小图标

	return RegisterClassExW(&wcex);
}
HWND FileList;
void SearchLanguageFiles()
{
	wchar_t lpPath[255] = { 0 }, szFind[255] = { 0 };
	WIN32_FIND_DATA FindFileData;
	GetModuleFileName(NULL, lpPath, MAX_PATH);
	(_tcsrchr(lpPath, _T('\\')))[1] = 0;
	wcscpy_s(szFind, lpPath);
	wcscat_s(szFind, L"*.ini");
	HANDLE hFind = ::FindFirstFile((LPCWSTR)szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)return;
	while (TRUE)
	{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			SendMessage(FileList, LB_ADDSTRING, 0, (LPARAM)FindFileData.cFileName);
		}
		if (!FindNextFile(hFind, &FindFileData))break;
	}
	FindClose(hFind);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//CaptureImage(GetDesktopWindow(), "D:\\", "zxf");
	hInst = hInstance; // 将实例句柄存储在全局变量中
	Main.InitClass(0);
	Main.CreateMain(CW_USEDEFAULT, CW_USEDEFAULT, 621, 550, L"极域破解C++版v0.6", 0);

	::SetWindowLong(Main.hWnd, GWL_STYLE, GetWindowLong(Main.hWnd, GWL_STYLE)& ~WS_CAPTION & ~WS_THICKFRAME&~WS_SYSMENU&~WS_GROUP&~WS_TABSTOP);
	Main.CreateEdit(325, 420, 110, 50, 1, L"explorer.exe", 0, 34, st);
	Main.CreateEdit(185, 102, 110, 40, 2, L"输入端口", ES_NUMBER, 35, st);
	Main.CreateEdit(365, 175, 210, 50, 2, L"输入密码", 0, 36, st);
	Main.CreateEdit(195, 102, 310, 37, 3, L"浏览文件/文件夹", 0, 37, st);
	Main.CreateEdit(277, 206, 138, 25, 5, L"StudentMain", 0, 38, st);
	FileList = CreateWindowW(L"ListBox", NULL, WS_CHILD  | LBS_STANDARD, 180, 405, 340, 120,Main.hWnd, (HMENU)1, hInstance, 0);
	wid = 621 * Main.DPI, hei = 550 * Main.DPI;
	SearchLanguageFiles();
	
	//SetWindowLong(hWnd,GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetWindowLong(Main.hWnd, GWL_EXSTYLE, GetWindowLong(Main.hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(Main.hWnd, NULL, 234, LWA_ALPHA);
	::SendMessage(FileList, WM_SETFONT, (WPARAM)st, 1);

	if (Admin == TRUE)top = 1,
		CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);

	RegisterHotKey(Main.hWnd, 1, MOD_CONTROL, 'P');
	RegisterHotKey(Main.hWnd, 2, MOD_CONTROL, 'B');
	RegisterHotKey(Main.hWnd, 3, NULL, VK_SCROLL);
	RegisterHotKey(Main.hWnd, 4, MOD_CONTROL, 'R');
	RegisterHotKey(Main.hWnd, 5, MOD_CONTROL, 'T');
	RegisterHotKey(Main.hWnd, 6, MOD_CONTROL | MOD_ALT, 'P');

	hBitmap = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ZXF), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);


	Main.CreateButton(0, 50, 140, 65, 0, L"安装/卸载", L"P1");
	Main.CreateButton(0, 114, 140, 65, 0, L"极域工具箱", L"P2");
	Main.CreateButton(0, 178, 140, 65, 0, L"其他工具", L"P3");
	Main.CreateButton(0, 242, 140, 65, 0, L"关于", L"P4");
	Main.CreateButton(0, 306, 140, 65, 0, L"设置", L"P5");//MENU
	Main.CreateButton(0, 370, 140, 180, 0, L"一键安装", L"QuickSetup");//6

	Main.CreateButton(195, 100, 110, 50, 1, L"应用层", L"DelR3");
	Main.CreateButton(325, 100, 110, 50, 1, L"驱动层", L"DelR0");//SETHC

	Main.CreateButton(195, 180, 110, 50, 1, L"应用层", L"SethcR3");
	Main.CreateButton(325, 180, 110, 50, 1, L"驱动层", L"SethcR0");


	Main.CreateFrame(170, 75, 410, 175, 1, L" Sethc ");
	Main.CreateFrame(170, 275, 410, 95, 1, L" 更强的HOOK ");
	Main.CreateFrame(170, 395, 410, 122, 1, L" 虚拟桌面 ");

	Main.CreateButton(195, 300, 110, 50, 1, L"安装", L"hookS");//HOOK
	Main.CreateButton(325, 300, 110, 50, 1, L"卸载", L"hookU");//12

	Main.CreateButton(195, 420, 110, 50, 1, L"运行程序", L"runinVD");//桌面
	Main.CreateButton(450, 420, 110, 50, 1, L"切换桌面", L"SwitchD");

	Main.CreateLine(185, 165, 565, 165, 1);

	Main.CreateText(470, 117, 1, L"删除文件", RGB(255, 100, 0));
	Main.CreateText(470, 197, 1, L"复制文件", RGB(255, 100, 0));
	Main.CreateText(195, 485, 1, L"按快捷键Ctrl+B切换", RGB(255, 100, 0));

	Main.CreateFrame(160, 75, 160, 146, 2, L" 频道工具 ");
	Main.CreateFrame(345, 75, 250, 272, 2, L" 管理员密码工具 ");


	Main.CreateCheck(165, 255, 2, 130, L" 伪装工具条旧");
	Main.CreateCheck(165, 280, 2, 80, L" 伪装工具条新");
	Main.CreateCheck(165, 305, 2, 130, L" 伪装托盘图标");

	Main.CreateButton(185, 155, 110, 45, 2, L"应用", L"ApplyPass");

	Main.CreateButton(365, 102, 97, 45, 2, L"清空密码", L"ClearPass");//16
	Main.CreateButton(477, 102, 97, 45, 2, L"查看密码", L"ViewPass");
	Main.CreateButton(365, 235, 60, 45, 2, L"改密1", L"CP1");
	Main.CreateButton(440, 235, 60, 45, 2, L"改密2", L"CP2");
	Main.CreateButton(515, 235, 60, 45, 2, L"改密3", L"CP3");

	Main.CreateButton(165, 370, 120, 55, 2, L"重新打开极域", L"re-TD");
	Main.CreateButton(305, 370, 120, 55, 2, L"程序窗口化", L"windows.ex");
	Main.CreateButton(445, 370, 120, 55, 2, L"防止关机", L"ANTI-");
	Main.CreateButton(165, 440, 120, 55, 2, L"反控制(测试)", L"NULL");
	Main.CreateButton(305, 440, 120, 55, 2, L"全自动防控制", L"auto-5");
	Main.CreateButton(445, 440, 120, 55, 2, L"卸载ntsd", L"Untsd");//26

	Main.CreateText(365, 295, 2, L"极域不同版本密码格式不一样", RGB(50, 50, 50));
	Main.CreateText(365, 317, 2, L"并不一定能修改成功", RGB(255, 100, 0));
	Main.CreateLine(360, 160, 583, 160, 2);

	Main.CreateFrame(170, 75, 410, 150, 3, L" 文件粉碎机 ");
	Main.CreateButton(520, 102, 36, 36, 3, L"...", L"viewfile");
	Main.CreateButton(436, 151, 120, 55, 3, L"打开文件夹", L"folder");
	Main.CreateButton(325, 151, 97, 55, 3, L"开始粉碎", L"TI");
	Main.CreateButton(195, 151, 115, 55, 3, L"T.A.粉碎机", L"TA");

	Main.CreateFrame(170, 255, 273, 105, 3, L" 电源 - 慎用 ");
	Main.CreateButton(192, 280, 115, 60, 3, L"BSOD(蓝屏)", L"BSOD");
	Main.CreateButton(324, 280, 100, 60, 3, L"瞬间重启", L"NtShutdown");

	Main.CreateButton(466, 255, 115, 106, 3, L"打游戏", L"Games");

	Main.CreateFrame(170, 388, 410, 105, 3, L" 杂项 ");
	Main.CreateButton(192, 412, 100, 60, 3, L"ARP攻击", L"ARP");//34
	Main.CreateButton(304, 412, 140, 60, 3, L"System权限CMD", L"SuperCMD");
	Main.CreateButton(455, 412, 105, 60, 3, L"干掉360", L"Killer");
	Main.CreateFrame(170, 75, 135, 170, 4, L"");
	Main.CreateText(325, 80, 4, L"作者：minecraft cxy villager", NULL);
	Main.CreateText(325, 105, 4, L"版本：C++ v0.6 测试版", NULL);
	Main.CreateText(372, 130, 4, L"64Bit Update 74", NULL);
	Main.CreateLine(170, 400, 590, 400, 4);
	Main.CreateText(170, 415, 4, L"CopyLeft(c) SA软件 2015 - 2018", NULL);
	Main.CreateText(170, 440, 4, L"版权 - 不存在的", NULL);
	Main.CreateButton(490, 415, 100, 50, 4, L"更多", L"more.txt");
	Main.CreateButton(490, 477, 100, 50, 4, L"系统信息", L"sysinfo");//38

	Main.CreateCheck(180, 90, 5, 100, L" 窗口置顶");
	Main.CreateCheck(180, 120, 5, 160, L" Ctrl+R 紧急蓝屏");
	Main.CreateCheck(180, 150, 5, 160, L" Ctrl+T 瞬间重启");
	Main.CreateCheck(180, 180, 5, 200, L" Ctrl+Alt+P 截图/显示");//新
	Main.CreateCheck(180, 210, 5, 300, L" 连续结束                   .exe");
	Main.CreateCheck(180, 240, 5, 160, L" 禁止键盘钩子");
	Main.CreateCheck(180, 270, 5, 160, L" 禁止鼠标钩子");//新
	Main.CreateCheck(180, 300, 5, 160, L" 测试");

	Main.CreateButton(180, 335, 100, 50, 5, L"最小化", L"minisize");
	Main.CreateButton(300, 335, 100, 50, 5, L"永久隐藏", L"hidest");
	Main.CreateButton(420, 335, 100, 50, 5, L"放大", L"better");

	Main.CreateButton(720, 105, 300, 300, 6, L"下载一个340", L"test");//game结构体
	Main.CreateFrame(139, 50, 481, 499, 0, L"");
	Main.CreateFrame(139, 50, 1080, 499, 0, L"");

	Main.CurButton++;

	if (Admin == 0)Main.CreateText(60, 17, 0, L"极域破解C++版v0.6 不是管理员", RGB(255, 255, 255));
	else Main.CreateText(60, 17, 0, L"极域破解C++版v0.6", RGB(255, 255, 255));

	Main.CreateButtonEx(Main.CurButton, 530, 10, 60, 30, 0, L"×", \
		CreateSolidBrush(RGB(255, 106, 106)), CreateSolidBrush(RGB(250, 102, 102)), CreateSolidBrush(RGB(238, 99, 99)), \
		CreatePen(PS_SOLID, 1, RGB(255, 106, 106)), CreatePen(PS_SOLID, 1, RGB(250, 102, 102)), CreatePen(PS_SOLID, 1, RGB(238, 99, 99)), \
		st, 1, 1, RGB(255, 255, 255), L"Close");

	hCurrentDesk = defaultDesk;

	if (!Main.hWnd)return FALSE;

	if (Admin == FALSE)
	{
		Main.Frame[1].rgb = RGB(255, 0, 0);
		wcscpy_s(Main.Frame[1].Name, L" Sethc - 不可用 ");
		Main.Frame[6].rgb = RGB(255, 0, 0);
		wcscpy_s(Main.Frame[6].Name, L" 文件粉碎机 - 不可用 ");
		Main.Frame[7].rgb = RGB(255, 0, 0);
		wcscpy_s(Main.Frame[7].Name, L" 电源 - 不可用 ");
		Main.Frame[2].rgb = RGB(5, 200, 135);
		wcscpy_s(Main.Frame[2].Name, L" 更强的HOOK - 可用 ");
		Main.Frame[3].rgb = RGB(10, 255, 10);
		wcscpy_s(Main.Frame[3].Name, L" 虚拟桌面 - 推荐 ");
	}

	ShowWindow(Main.hWnd, nCmdShow);
	UpdateWindow(Main.hWnd);

	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(_In_ HWND hWnd, _In_ LPARAM lParam)
{
	if (IsWindowVisible(hWnd) && GetParent(hWnd) != hWndParent)
	{
		SetParent(hWnd, hWndParent);
	}

	return TRUE;
}

//响应函数
HDC rdc;
HBITMAP hBmp;
CWndShadow c;
HDC tdc;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


	switch (message)
	{
	case	WM_CREATE:
		c.Initialize(hInst);
		c.Create(hWnd);

		rdc = GetDC(Main.hWnd);
		hdc = CreateCompatibleDC(rdc);
		tdc = CreateCompatibleDC(rdc);
		hBmp = CreateCompatibleBitmap(rdc, 4000, 4000);
		SelectObject(hdc, hBmp);
		ReleaseDC(Main.hWnd, rdc);
		break;
	case WM_HOTKEY://热键
	{
		switch (wParam)
		{
		case 1:
			if (hide == 0)
				ShowWindow(Main.hWnd, SW_HIDE), hide = 1;
			else
				if (!AlwaysHide)ShowWindow(Main.hWnd, SW_SHOW), hide = 0;
			break;
		case 2:
			SDesktop();
			break;
		case 3:
			if (oneclick)
			{
				KillProcess(L"stu");
			}
			break;
		case 4:
			if (Main.Check[5].Value == 1)BSOD();
			break;
		case 5:
			if (Main.Check[6].Value == 1)Restart();
			break;
		case 6:
			if (Main.Check[7].Value == 1)CathyageBox(L"截图?");
			break;
		}
	}

	case WM_PAINT:
	{
		Main.hdc = hdc;
		PAINTSTRUCT ps;
		rdc = BeginPaint(hWnd, &ps);

		SelectObject(hdc, st);//宋体
		SetBkMode(rdc, TRANSPARENT);
		SetBkMode(hdc, TRANSPARENT);
		SetBkMode(tdc, TRANSPARENT);

		SelectObject(hdc, WhiteBrush);//白色背景
		SelectObject(hdc, White);
		Rectangle(hdc, 0, 0, 2000, 2000);

		SelectObject(hdc, GREEN);//绿色顶部
		SelectObject(hdc, green);
		Rectangle(hdc, 0, 0, 1230 * Main.DPI, 50 * Main.DPI);


		SetTextColor(hdc, RGB(0, 0, 0));
		SelectObject(hdc, BLACK);
		SelectObject(hdc, WhiteBrush);

		Main.DrawEVERYTHING();

		HICON hicon;
		hicon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_GUI)); \
			DrawIconEx(hdc, 20 * Main.DPI, 10 * Main.DPI, hicon, 32 * Main.DPI, 32 * Main.DPI, 0, NULL, DI_NORMAL | DI_COMPAT);
		//SetBrushOrgEx(hdc,)


		HBRUSH bruss = CreatePatternBrush(hBitmap);
		SelectObject(hdc, bruss);
		if (Main.CurWnd == 4)Rectangle(hdc, 135 * 22, 170 * 18, 135 * 23, 170 * 19);
		//s((int)hBitmap);
		if (Main.CurWnd == 4)StretchBlt(hdc, 170 * Main.DPI, 75 * Main.DPI, 135 * Main.DPI, 170 * Main.DPI, hdc, 135 * 22, 170 * 18, 135, 170, SRCCOPY);
		//170, 75, 135, 150,
		BitBlt(rdc, 0, 0, 2000, 2000, hdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}

	break;

	case 34:
		if (lParam == WM_LBUTTONUP)CathyageBox(L"鼠标");
		break;

	case WM_COMMAND:
		int wmId; wmId = LOWORD(wParam);
		switch (wmId)
		{
		case 1:
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				ZeroMemory(Gutc, sizeof(Gutc));
				SendMessage(FileList, LB_GETTEXT, ::SendMessage(FileList, LB_GETCURSEL, 0, 0), (LPARAM)Gutc);
				SwitchLanguage(Gutc);
				
				break;
			}
			break;
		case 35:
			GetWindowTextW(Main.Edit[2].hWnd, text1, 255);
			if (text1[0] == 36755)SetWindowTextW(Main.Edit[2].hWnd, NULL);//unicode转10进制
			break;
		case 36:
			GetWindowTextW(Main.Edit[3].hWnd, text1, 255);
			if (text1[0] == 36755)SetWindowTextW(Main.Edit[3].hWnd, NULL);//unicode转10进制
			break;
		case 37:
			GetWindowTextW(Main.Edit[4].hWnd, text1, 255);
			if (text1[0] == 27983)SetWindowTextW(Main.Edit[4].hWnd, NULL);
			break;
		case 38:
			GetWindowTextW(Main.Edit[5].hWnd, text1, 255);
			if (text1[0] == 'S'&&text1[1] == 't'&&text1[2] == 'u'&&text1[7] == 'M')SetWindowTextW(Main.Edit[5].hWnd, NULL);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		BOOL f; f = FALSE;

		for (int i = 0; i <= Main.CurButton; i++)
			if (Main.Button[i].Page == Main.CurWnd || Main.Button[i].Page == 0 || (GameMode&& Main.Button[i].Page == 6))
				if (Main.InsideButton(i) && Main.Button[i].Download == 0) { f = TRUE; break; }

		if (f == FALSE)
		{
			for (int i = 1; i <= Main.CurCheck; i++)
				if (Main.Check[i].Page == Main.CurWnd || Main.Check[i].Page == 0)
					if (Main.InsideCheck(i) != 0) { f = TRUE; break; }
			if (f == FALSE)
			{
				PostMessage(Main.hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
			}
		}
		Main.LeftButtonDown();
		break;

	case WM_LBUTTONUP:

		if (Main.CurCover != -1)
		{
			Main.Press = 0;
			RECT rc;
			rc = Main.GetRECT(Main.CurCover);
			InvalidateRect(Main.hWnd, &rc, FALSE);
		}

		LPCWSTR x;
		x = Main.GetCurInsideID();
		BUTTON_IN(x, L"P1") { Main.SetPage(1); ColorfulEggs(false); ShowWindow(FileList, SW_HIDE); break; }
		BUTTON_IN(x, L"P2") { Main.SetPage(2); ColorfulEggs(false); ShowWindow(FileList, SW_HIDE);  break; }
		BUTTON_IN(x, L"P3") { Main.SetPage(3); ColorfulEggs(false); ShowWindow(FileList, SW_HIDE); break; }
		BUTTON_IN(x, L"P4") { Main.SetPage(4); ColorfulEggs(true); ShowWindow(FileList, SW_HIDE); break; }
		BUTTON_IN(x, L"P5") { Main.SetPage(5); ColorfulEggs(false); ShowWindow(FileList, SW_SHOW); break; }
		BUTTON_IN(x, L"QuickSetup")
		{
			oneclick = 1 - oneclick;
			if (oneclick == 1)
				MessageBox(Main.hWnd, L"安装成功，请按Scroll Lock键查看效果！", L"提示", MB_ICONINFORMATION);
			else
				MessageBox(Main.hWnd, L"卸载成功", L"提示", MB_ICONINFORMATION);
			break;
		}
		BUTTON_IN(x, L"DelR3") { DeleteSethc(); break; }
		BUTTON_IN(x, L"DelR0") { DeleteSethcS(); break; }
		BUTTON_IN(x, L"SethcR3") { SetupSethc(); CopyNTSD(); break; }
		BUTTON_IN(x, L"SethcR0") { CopySethcS(); break; }
		BUTTON_IN(x, L"hookS")
		{
			KillProcess(L"hoo");
			CopyNTSD();

			wcscpy_s(Gutc, Path);
			wcscat_s(Gutc, L"hook.exe");

			si = { 0 }; pi = { 0 };
			CreateProcessW(Gutc, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
			break;
		}
		BUTTON_IN(x, L"hookU") { KillProcess(L"hoo"); break; }
		BUTTON_IN(x, L"runinVD")
		{
			si = { 0 };
			si.cb = sizeof(si);
			si.lpDesktop = szVDesk;
			GetWindowTextW(Main.Edit[1].hWnd, text1, 255);
			if (!CreateProcess(NULL, text1, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				MessageBox(NULL, TEXT("启动失败！"), TEXT("Error"), 0);
			break;
		}
		BUTTON_IN(x, L"SwitchD") { SDesktop(); break; }

		BUTTON_IN(x, L"ApplyPass")
		{
			GetWindowTextW(Main.Edit[2].hWnd, text1, 2550);
			for (int i = 1; i <= (int)wcslen(text1); i++)//有待改进，小于48？NULL
			{
				if (text1[i] > 57)
				{
					MessageBox(Main.hWnd, L"不能这样做", L"提示", MB_ICONQUESTION);
					break;
				}
			}
			WCHAR a[2650];
			wcscpy_s(a, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\TopDomain\\e-learning Class\\Student\" /v ChannelId /t reg_dword /d ");
			wcscat_s(a, text1);
			wcscat_s(a, L" /f");
			//CathyageBox(a);
			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			CreateProcess(NULL, a, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			MessageBox(Main.hWnd, L"修改成功", L"提示", MB_ICONINFORMATION);
			break;
		}
		BUTTON_IN(x, L"ClearPass")
		{
			wchar_t A[1000], B[1000], C[1000];
			//const wchar_t Knock=
			if (Bit != 64)
			{
				wcscpy_s(A, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\TopDomain\\e-learning Class\\Student\" /v  \"Knock1\" /d \"376A1211DB6EE41A2018E643DE3817069222A4338236B92BCA05490134625F12A1710F15AB07225C2E39600F7307B34D2C14BB67352C3967031A8B3DAE3B7159276F8136AA6043466960D53DB66485595503405E556CF1795527775093382535121987357B057F126F1494593A097537\" /t reg_binary /f");
				wcscpy_s(B, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00\" /v \"UninstallPasswd\" /d \"\" /t reg_sz /f");
				wcscpy_s(C, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\TopDomain\\e-learning Class\\Student\" /v  \"Knock\" /d \"376A1211DB6EE41A2018E643DE3817069222A4338236B92BCA05490134625F12A1710F15AB07225C2E39600F7307B34D2C14BB67352C3967031A8B3DAE3B7159276F8136AA6043466960D53DB66485595503405E556CF1795527775093382535121987357B057F126F1494593A097537\" /t reg_binary /f");
			}
			else
			{//极域只有32位的，所以注册表目录会被重定向到WOW6432Node
			 //然而GUI可能是64位，此时要做特殊处理
				wcscpy_s(A, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student\" /v  \"Knock1\" /d \"376A1211DB6EE41A2018E643DE3817069222A4338236B92BCA05490134625F12A1710F15AB07225C2E39600F7307B34D2C14BB67352C3967031A8B3DAE3B7159276F8136AA6043466960D53DB66485595503405E556CF1795527775093382535121987357B057F126F1494593A097537\" /t reg_binary /f");
				wcscpy_s(B, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00\" /v \"UninstallPasswd\" /d \"\" /t reg_sz /f");
				wcscpy_s(C, L"reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student\" /v  \"Knock\" /d \"376A1211DB6EE41A2018E643DE3817069222A4338236B92BCA05490134625F12A1710F15AB07225C2E39600F7307B34D2C14BB67352C3967031A8B3DAE3B7159276F8136AA6043466960D53DB66485595503405E556CF1795527775093382535121987357B057F126F1494593A097537\" /t reg_binary /f");

			}
			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			CreateProcess(NULL, A, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			CreateProcess(NULL, B, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			CreateProcess(NULL, C, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			MessageBox(Main.hWnd, L"修改成功", L"提示", MB_ICONINFORMATION);
			break;
		}
		BUTTON_IN(x, L"ViewPass")
		{
			HKEY hKey;
			LONG ret;
			WCHAR szLocation[300];
			DWORD dwSize; dwSize = sizeof(WCHAR) * 300;
			DWORD dwType; dwType = REG_SZ;
			ZeroMemory(&szLocation, sizeof(szLocation));
			if (Bit != 64)
				ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
			else
				ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);

			if (ret != 0)
				if (MessageBox(Main.hWnd, L"出错了...可能是因为键值不存在\n按确定强制读取", L"提示", MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)break;
			ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType, (LPBYTE)&szLocation, &dwSize);
			if (ret != 0)
				if (MessageBox(Main.hWnd, L"出错了...\n按确定强制读取", L"提示", MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)break;
			if (szLocation[0] != 0)
			{
				WCHAR Tmp[300];
				wcscpy_s(Tmp, L"密码为：");
				wcscat_s(Tmp, szLocation);
				MessageBox(Main.hWnd, Tmp, L"提示", MB_ICONINFORMATION);
			}
			else
				MessageBox(Main.hWnd, L"密码为空或键值不存在", L"提示", MB_ICONINFORMATION);
			break;
		}
		BUTTON_IN(x, L"CP1") { ChangePasswordEx(1); break; }
		BUTTON_IN(x, L"CP2") { ChangePasswordEx(2); break; }
		BUTTON_IN(x, L"CP3") { ChangePasswordEx(3); break; }
		BUTTON_IN(x, L"re-TD")
		{
			if (Bit == 32)
				SearchTool(L"C:\\Program Files\\Mythware\\", 1);
			else
				SearchTool(L"C:\\Program Files (x86)\\Mythware\\", 1);
			break;
		}
		BUTTON_IN(x, L"windows.ex")
		{
			if (FC == 0)
			{
				CatchWnd.InitClass(0);
				InitCathy();
				FC = 1;//first start catchwindow
			}
			if (Catch != 0)ShowWindow(Catch, SW_HIDE);
			Catch = CreateWindowW(CatchWindow, L"捕捉窗口", WS_OVERLAPPEDWINDOW, 200, 200, 625, 550, nullptr, nullptr, hinst, nullptr);
			CatchWnd.hWnd = Catch;
			CatchWnd.CurEdit = 0;
			CatchWnd.CreateEdit(20, 20, 250, 50, 0, L"StudentMain.exe", 0, 34, st);
			CatchWnd.CreateEdit(20, 200, 40, 50, 0, L"5", ES_NUMBER, 35, st);
			ShowWindow(Catch, SW_SHOW);
			UpdateWindow(Catch);
			break;
		}

		BUTTON_IN(x, L"ANTI-")
		{
			if (Bit == 32)
				SearchTool(L"C:\\Program Files\\Mythware\\", 2);
			else
				SearchTool(L"C:\\Program Files (x86)\\Mythware\\", 2);
			break;
		}
		BUTTON_IN(x, L"NULL")
		{
			//do nothing
			break;
		}
		BUTTON_IN(x, L"auto-5")
		{
			mypid = GetCurrentProcessId();
			KillTop();
			break;
		}
		BUTTON_IN(x, L"Untsd")
		{
			//do nothing
			break;
		}
		BUTTON_IN(x, L"viewfile") { openfile(); break; }
		BUTTON_IN(x, L"folder") { BrowseFolder(); break; }
		BUTTON_IN(x, L"TI")
		{
			GetWindowText(Main.Edit[4].hWnd, text1, 2550);
			int FileType;
			FileType = GetFileAttributes(text1);
			if (FileType == -1 || FileType == 22) { MessageBox(Main.hWnd, L"这不是文件/文件夹！", L"提示", MB_ICONINFORMATION); break; }
			if (FileType != 16)
				wcscpy_s(Gutc, L"sc config TrustedInstaller binPath= \"cmd.exe /C del /F ");
			else
				wcscpy_s(Gutc, L"sc config TrustedInstaller binPath= \"cmd.exe /C rd /S /Q ");
			wcscat_s(Gutc, text1);
			wcscat_s(Gutc, L"\"");
			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			CreateProcessW(NULL, Gutc, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			wcscpy_s(Gutc, L"net start TrustedInstaller");
			CreateProcessW(NULL, Gutc, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			break;
		}
		BUTTON_IN(x, L"TI") { WinExec("deleter\\DrvDelFile.exe", SW_SHOW); break; }
		BUTTON_IN(x, L"BSOD") { BSOD(); break; }
		BUTTON_IN(x, L"NtShutdown") { Restart(); break; }

		BUTTON_IN(x, L"Games")
		{
			if (GameMode == 0)
			{
				wcscpy_s(Gutc, Path);
				wcscat_s(Gutc, L"games");
				CreateDirectory(Gutc, NULL);
				wcscpy_s(Gutc, Path);
				wcscat_s(Gutc, L"games\\xiaofei.exe");
				if (_waccess(Gutc, 0) == 0)GameExist[1] = 1;
				wcscpy_s(Main.Button[Main.CurCover].Name, L"停止");
				GameMode = 1;
				for (int j = 1; j <= 600; j += 10)
				{
					::SetWindowPos(hWnd, NULL, 0, 0, (Main.Width + j)*Main.DPI, Main.Height *Main.DPI, SWP_NOMOVE | SWP_NOREDRAW);
					Main.Button[Main.GetNumbyID(L"Close")].Left += 10;
					UpdateWindow(Main.hWnd);
					InvalidateRect(Main.hWnd, 0, FALSE);
				}

				Main.Width += 600;
				::SetWindowPos(hWnd, NULL, 0, 0, Main.Width *Main.DPI, Main.Height *Main.DPI, SWP_NOMOVE | SWP_NOREDRAW);
			}
			else
			{
				GameMode = 0;
				wcscpy_s(Main.Button[Main.CurCover].Name, L"打游戏");
				for (int j = 1; j <= 600; j += 10)
				{
					::SetWindowPos(Main.hWnd, NULL, 0, 0, (Main.Width - j)*Main.DPI, Main.Height*Main.DPI, SWP_NOMOVE | SWP_NOREDRAW);
					Main.Button[Main.GetNumbyID(L"Close")].Left -= 10;
					UpdateWindow(Main.hWnd);
					InvalidateRect(Main.hWnd, 0, FALSE);
				}
				//InvalidateRect(Main.hWnd, NULL, FALSE);
				Main.Width -= 600;
				::SetWindowPos(hWnd, NULL, 0, 0, Main.Width *Main.DPI, Main.Height *Main.DPI, SWP_NOMOVE | SWP_NOREDRAW);

			}
			break;
		}
		BUTTON_IN(x, L"ARP")//cathy
		{
			TCHAR arp[50];
			wcscpy_s(Gutc, Path);
			wcscat_s(Gutc, L"arp\\npf.sys");
			CopyFile(Gutc, L"C:\\Windows\\System32\\drivers\\npf.sys", FALSE);
			CopyFile(Gutc, L"C:\\Windows\\SysNative\\drivers\\npf.sys", FALSE);
			wcscpy_s(arp, L"arp\\arp.exe");
			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			CreateProcessW(NULL, arp, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			break;
		}
		BUTTON_IN(x, L"SuperCMD")
		{
			wcscpy_s(Gutc, L"reg add \"HKEY_CURRENT_USER\\Software\\Sysinternals\\PsExec\" /v \"EulaAccepted\" /t reg_dword /d \"00000001\" /f");
			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			CreateProcessW(NULL, Gutc, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			wcscpy_s(Gutc, L"psexec.exe -i -s -d cmd.exe");
			CreateProcessW(NULL, Gutc, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			break;
		}
		BUTTON_IN(x, L"Killer")
		{
			WinExec("net stop 360", SW_HIDE);
			WinExec("sc delete 360", SW_HIDE);//以后做一个LoadDriverEx();
			LPCWSTR drv1;
			if (Bit == 32)drv1 = L"x32\\360.sys"; else drv1 = L"x64\\360.sys";
			wcscpy_s(Gutc, Path);
			wcscat_s(Gutc, drv1);
			LPCWSTR drv2; drv2 = L" type=kernel";
			TCHAR drv3[200];
			wcscpy_s(drv3, L"sc create 360 binpath=");
			wcscat_s(drv3, Gutc);
			wcscat_s(drv3, drv2);
			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			MessageBox(Main.hWnd, L"请启动360！360可能会报加载驱动，请放行！\n如果执行后蓝屏，请把dump送至作者邮箱", L"提示", MB_ICONINFORMATION);
			CreateProcessW(NULL, drv3, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			wcscpy_s(drv3, L"net start 360");
			CreateProcessW(NULL, drv3, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			break;
		}
		BUTTON_IN(x, L"more.txt")
		{
			ChooseColor()

			wcscpy_s(Gutc, L"Notepad ");
			wcscat_s(Gutc, Path);
			wcscat_s(Gutc, L"关于&帮助.txt");
			ZeroMemory(&pi, sizeof(pi));
			CreateProcessW(NULL, Gutc, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			break;
		}
		BUTTON_IN(x, L"sysinfo")
		{
			wcscpy_s(Gutc, L"C:\\Windows\\System32\\Msinfo32.exe");
			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			CreateProcessW(NULL, Gutc, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			break;
		}
		BUTTON_IN(x, L"minisize") { ShowWindow(Main.hWnd, SW_MINIMIZE); break; }
		//	
		BUTTON_IN(x, L"hidest") { AlwaysHide = 1; ShowWindow(Main.hWnd, SW_HIDE);	break; }
		BUTTON_IN(x, L"better") { Main.SetDPI(Main.DPI*1.1); break; }
		BUTTON_IN(x, L"test")
		{
			int typ;
			typ = 1;
			CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL);
			break;
		}
		BUTTON_IN(x, L"Close") { ExitProcess(0); break; }

		for (int i = 1; i <= Main.CurCheck; i++)
		{
			if (Main.Check[i].Page == 0 || Main.Check[i].Page == Main.CurWnd)
			{
				int result;
				result = Main.InsideCheck(i);
				if (result != 0)
				{
					RECT rc;
					rc.left = Main.Check[i].Left*Main.DPI;
					rc.top = Main.Check[i].Top*Main.DPI;
					rc.right = Main.Check[i].Left *Main.DPI + 15 * Main.DPI;
					rc.bottom = Main.Check[i].Top *Main.DPI + 15 * Main.DPI;
					InvalidateRect(Main.hWnd, &rc, FALSE);
					if (!Main.Check[i].Value)
					{
						switch (i)
						{
						case 1:
							//SetProcessDPIAware();
							wcscpy_s(Gutc, Path);
							wcscat_s(Gutc, L"cheat\\old.exe");
							ZeroMemory(&si, sizeof(si));
							ZeroMemory(&pi, sizeof(pi));
							CreateProcessW(NULL, Gutc, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
							break;
						case 2:
							wcscpy_s(Gutc, Path);
							wcscat_s(Gutc, L"cheat\\new.exe");
							ZeroMemory(&si, sizeof(si));
							ZeroMemory(&pi, sizeof(pi));
							CreateProcessW(NULL, Gutc, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
							break;
						case 3:

							tnd.cbSize = sizeof(NOTIFYICONDATA);
							tnd.hWnd = Main.hWnd;
							tnd.uID = IDR_MAINFRAME;
							tnd.uFlags = 2 | 1 | 4;
							tnd.uCallbackMessage = 34;
							tnd.hIcon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_GUI));   //Í¼±êID 
																					  //wcscpy_s(tnd.szTip, L"极域电子教室学生端 - 已连接至教师");
							wcscpy_s(tnd.szTip, L"34");
							Shell_NotifyIcon(NIM_ADD, &tnd);
							break;
						case 4:
							top = 1;
							CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);
							break;
						case 5:
							if (Admin == 0)MessageBox(Main.hWnd, L"当前非管理员模式，蓝屏可能无效", L"提示", MB_ICONINFORMATION);
							break;
						case 6:
							if (Admin == 0)MessageBox(Main.hWnd, L"当前非管理员模式，瞬间重启可能无效", L"提示", MB_ICONINFORMATION);
							break;
						case 8:
						case 9:
							SetTimer(hWnd, 1, 100, (TIMERPROC)TimerProc);
							break;
						case 10:
							typedef DWORD(CALLBACK* SEtProcessDPIAware)(void);
							SEtProcessDPIAware SetProcessDPIAware;
							HMODULE huser;
							huser = LoadLibrary(L"user32.dll");
							SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
							SetProcessDPIAware();
							break;
						case 11:
							//ShellExecute(0, L"runas", Name, 0, 0, SW_SHOWNORMAL);
							//ExitProcess(0);
							HDESK hdesk;
							hdesk = OpenDesktopA("Winlogon", 0, FALSE,
								DESKTOP_CREATEMENU |
								DESKTOP_CREATEWINDOW |
								DESKTOP_ENUMERATE |
								DESKTOP_HOOKCONTROL |
								DESKTOP_JOURNALPLAYBACK |
								DESKTOP_JOURNALRECORD |
								DESKTOP_READOBJECTS |
								DESKTOP_SWITCHDESKTOP |
								DESKTOP_WRITEOBJECTS);
							if (hdesk == NULL) {
								s("failed"); break;
							}
							si = { 0 };
							si.cb = sizeof(si);
							wchar_t aa[10] = L"Winlogon";
							si.lpDesktop = aa;
							GetWindowText(Main.Edit[5].hWnd, Gutc, 200);
							if (!CreateProcess(NULL, Gutc, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
								MessageBox(NULL, TEXT("启动失败！"), TEXT("Error"), 0);
							break;
						}
					}
					else
					{
						switch (i)
						{
						case 1:
							KillProcess(L"old");
							break;
						case 2:
							KillProcess(L"new");
							break;
						case 3:
							Shell_NotifyIcon(NIM_DELETE, &tnd);
							break;
						case 4:
							top = 0;
							break;
						}
					}
					Main.Check[i].Value = 1 - Main.Check[i].Value;
				}
			}
		}

		break;
	case WM_MOUSEMOVE:
		Main.MouseMove();
		break;

	case WM_MOUSELEAVE:
		PostMessage(Main.hWnd, WM_MOUSEMOVE, NULL, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		ExitProcess(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK CatchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC Hdc;
	PAINTSTRUCT ps;
	int i;
	GetCursorPos(&point);
	ScreenToClient(Catch, &point);
	switch (message)
	{
	case WM_PAINT:
		Hdc = BeginPaint(Catch, &ps);

		CatchWnd.hdc = Hdc;
		CatchWnd.DrawButtons();

		TCHAR T[20], zou[20];
		wcscpy_s(T, L"已经吃掉了 ");
		_itow_s(cureat, zou, 10);
		wcscat_s(T, zou);
		wcscat_s(T, L" 个窗口");
		TextOutW(Hdc, 20, 155, T, (int)wcslen(T));
		break;
	case WM_LBUTTONDOWN:

		CatchWnd.LeftButtonDown();
		break;
	case WM_MOUSEMOVE:
		CatchWnd.MouseMove();

		break;
	case WM_LBUTTONUP:
		if (CatchWnd.CurCover != -1)
		{
			CatchWnd.Press = 0;
			RECT rc = CatchWnd.GetRECT(CatchWnd.CurCover);
			InvalidateRect(Catch, &rc, FALSE);
		}

		for (i = 1; i <= CatchWnd.CurButton; i++)
		{
			if (CatchWnd.InsideButton(i))
			{
				switch (i)
				{
				case 1:
					wchar_t tmp1[12];
					ZeroMemory(tmp1, sizeof(tmp1));
					GetWindowTextW(CatchWnd.Edit[2].hWnd, tmp1, 10);
					timerleft = _wtoi(tmp1);
					//s(tmp1);
					//timerleft = 6;
					wcscpy_s(zxfback, CatchWnd.Button[1].Name);
					SetTimer(Catch, 2, 1000, (TIMERPROC)TimerProc);
					break;
				case 2:
					ReturnWindows();
				}
			}
		}
		break;
	case WM_DESTROY:
		ReturnWindows();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

VOID InitCathy()
{
	hinst = GetModuleHandle(CatchWindow);
	cat.cbSize = sizeof(WNDCLASSEX);
	cat.style = CS_HREDRAW | CS_VREDRAW;
	cat.lpfnWndProc = CatchProc;
	cat.cbClsExtra = 0;
	cat.cbWndExtra = 0;
	cat.hInstance = hinst;
	cat.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_GUI));//大图标
	cat.hCursor = LoadCursor(nullptr, IDC_ARROW);
	cat.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	cat.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
	cat.lpszClassName = CatchWindow;
	cat.hIconSm = LoadIcon(cat.hInstance, MAKEINTRESOURCE(IDI_GUI));//小图标
	RegisterClassExW(&cat);
	CatchWnd.CreateButton(10, 100, 130, 50, 0, L"捕捉窗口", L"");
	CatchWnd.CreateButton(160, 100, 130, 50, 0, L"释放窗口", L"");

}
