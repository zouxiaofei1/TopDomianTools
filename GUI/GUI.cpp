#include "stdafx.h"
#include "GUI.h"
#include "Shlobj.h" 
#include "WndShadow.h"
#include "Actions.h"
#include "TestFunctions.h"
#include <winsock.h>

#define BUTTON_IN(x,y) if(x == Hash(y))
#define Delta 10

#pragma comment(lib, "urlmon.lib")//下载文件用的Lib   
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Imm32.lib")
#pragma comment(lib, "ws2_32.lib")    //Winsock API 库
#pragma comment(lib, "netapi32.lib")

#pragma warning(disable:4244)
#pragma warning(disable:4996)

VOID				InitCathy();//部分(重要)函数的前向声明
VOID				InitScreen();
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);//主窗口
LRESULT CALLBACK	CatchProc(HWND, UINT, WPARAM, LPARAM);//第二窗口
VOID	CALLBACK	TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);//计时器

HINSTANCE hInst;// 当前实例备份变量，CreateWindow时需要
TCHAR szWindowClass[] = L"GUI";//主窗口类名
TCHAR CatchWindow[] = L"CatchWindow";//第二窗口类名
TCHAR ScreenWindow[] = L"ScreenWindow";//截图伪装窗口类名

//自己定义的 有点乱

BOOL Effect = TRUE;//特效开关
TCHAR Path[301], Name[301];//程序路径 and 路径+程序名 
TCHAR SethcPath[255];//Sethc路径
BOOL FC = TRUE, FS = TRUE;//是否第一次启动窗口并注册类
HBITMAP hZXFBitmap;
ULONG CurrentProcessId;//当前程序Pid，用于自动防控制
INT ScreenState;//截图伪装状态 1 = 截图 2 = 显示
HDESK hVirtualDesk, hCurrentDesk, defaultDesk;//虚拟桌面 & 当前桌面 & 默认桌面
TCHAR szVDesk[] = L"Cathy";//虚拟桌面名称
BOOL Admin; //是否管理员
INT Bit;//系统位数 32 34 64
HHOOK KeyboardHook, MouseHook;//键盘/鼠标钩子
BOOL HideState;//窗口是否隐藏
BOOL oneclick;//一键安装状态
int GameMode;//游戏模式？
int EasterEggState;//CopyLeft文字循环状态
wchar_t EasterEggStr[11][15] = { L"AnswerKey(?)",L"Left(?)" ,L"Left(?)",L"Right(?)",L"Down(?)",L"Up(?)",L"In(?)"\
,L"On(?)",L"Back(?)",L"Front(?)",L"Teacher(?)" };
BOOL slient = FALSE;//是否命令行

//笔刷 + 笔
HBRUSH DBlueBrush, LBlueBrush, WhiteBrush, BlueBrush, green, grey, yellow;
HPEN YELLOW, RED, BLACK, White, GREEN, LGREY, BLUE;

HWND FileList;//语言选择hwnd
BOOL lock = FALSE;//Game按钮锁定
INT timerleft = -2;//吃窗口倒计时
TCHAR zxfback[101];//倒计时备份按钮名称

std::unordered_map<int, bool>Eatpid;//吃窗口hWnd记录链表
struct _Eatlist
{
	HWND value;
	_Eatlist *next;
}Eatlist, *Eating;
int CurEat;

HDC hdc, rdc;//主窗口缓冲hdc + 贴图hdc
HBITMAP hBmp;//主窗口hbmp
HDC pdc;//截图伪装窗口hdc
CWndShadow Cshadow;//主窗口阴影特效
HDC tdc, Hdc;//吃窗口缓冲hdc + 贴图hdc
HBITMAP HBMP;//吃窗口hbmp

BOOL TOP;//是否置顶
const int numGames = 6;//游戏数
BOOL GameExist[numGames + 1];
TCHAR GameName[numGames + 1][25] = { L"Games\\xiaofei.exe", L"Games\\fly.exe",L"Games\\2048.exe",L"Games\\block.exe", \
L"Games\\1.exe" , L"Games\\chess.exe" };//游戏名
DWORD expid[100], tdpid[100];//explorer PID + 被监视窗口 PID
HWND tdh[101]; //被监视窗口hwnd
int tdhcur, displaycur;//被监视窗口数量 + 正在被监视的窗口编号

class CathyClass
{
public:
	VOID InitClass(HINSTANCE HInstance)
	{
		hInstance = HInstance;
		CurButton = CurFrame = CurCheck = CurLine = CurText = 0;
		CurWnd = 1;
		DPI = 1;

		DefFont = CreateFontW(16 * DPI, 8 * DPI, 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));

		CurCover = -1;
		CoverCheck = 0;
	}

	wchar_t *GetStr(LPCWSTR ID)
	{
		return str[Hash(ID)];
	}
	VOID SetString(LPCWSTR Str, LPCWSTR ID)
	{
		wcscpy(str[Hash(ID)], Str);
	}
	VOID CreateString(LPCWSTR Str, LPCWSTR ID)
	{
		CurString++;
		if (Str != NULL)wcscpy_s(string[CurString].str, Str);
		wcscpy_s(string[CurString].ID, ID);
		str[Hash(ID)] = string[CurString].str;
	}

	VOID CreateEditEx(INT Left, INT Top, INT Wid, INT Hei, INT Page, LPCWSTR name, LPCWSTR ID, HFONT Font)
	{
		CurEdit++;
		Edit[CurEdit].Left = Left;
		Edit[CurEdit].Top = Top;
		Edit[CurEdit].Width = Wid;
		Edit[CurEdit].Height = Hei;
		Edit[CurEdit].Page = Page;
		SetEditStrOrFont(name, Font, CurEdit);
		wcscpy_s(Edit[CurEdit].ID, ID);
	}

	VOID CreateButtonEx(INT Number, INT Left, INT Top, INT Wid, INT Hei, INT Page, LPCWSTR name, HBRUSH Leave, \
		HBRUSH Hover, HBRUSH press, HPEN Leave2, HPEN Hover2, HPEN Press2, HFONT Font, BOOL Enabled, BOOL Visible, COLORREF FontRGB, LPCWSTR ID)
	{
		Button[Number].Left = Left;
		Button[Number].Top = Top;
		Button[Number].Width = Wid;
		Button[Number].Height = Hei;
		Button[Number].Page = Page;
		Button[Number].Leave = Leave;
		Button[Number].Hover = Hover;
		Button[Number].Press = press;
		Button[Number].Leave2 = Leave2;
		Button[Number].Hover2 = Hover2;
		Button[Number].Press2 = Press2;
		Button[Number].Font = Font;
		Button[Number].Enabled = Enabled;
		Button[Number].Visible = Visible;
		Button[Number].FontRGB = FontRGB;
		wcscpy_s(Button[Number].Name, name);
		wcscpy_s(Button[Number].ID, ID);
		but[Hash(ID)] = Number;

		LOGBRUSH LogBrush;
		LOGPEN LogPen;
		GetObject(Leave, sizeof(LogBrush), &LogBrush);
		Button[Number].b1[0] = (byte)LogBrush.lbColor;
		Button[Number].b1[1] = (byte)(LogBrush.lbColor >> 8);
		Button[Number].b1[2] = (byte)(LogBrush.lbColor >> 16);
		GetObject(Hover, sizeof(LogBrush), &LogBrush);
		Button[Number].b2[0] = (byte)LogBrush.lbColor;
		Button[Number].b2[1] = (byte)(LogBrush.lbColor >> 8);
		Button[Number].b2[2] = (byte)(LogBrush.lbColor >> 16);
		GetObject(Leave2, sizeof(LogPen), &LogPen);
		Button[Number].p1[0] = (byte)LogPen.lopnColor;
		Button[Number].p1[1] = (byte)(LogPen.lopnColor >> 8);
		Button[Number].p1[2] = (byte)(LogPen.lopnColor >> 16);

		GetObject(Hover2, sizeof(LogPen), &LogPen);
		Button[Number].p2[0] = (byte)LogPen.lopnColor;
		Button[Number].p2[1] = (byte)(LogPen.lopnColor >> 8);
		Button[Number].p2[2] = (byte)(LogPen.lopnColor >> 16);
	}
	VOID CreateButton(INT Left, INT Top, INT Wid, INT Hei, INT Page, LPCWSTR name, LPCWSTR ID)
	{
		CurButton++;
		CreateButtonEx(CurButton, Left, Top, Wid, Hei, Page, name, WhiteBrush, DBlueBrush, LBlueBrush, BLACK, BLACK, BLACK, 0, TRUE, TRUE, RGB(0, 0, 0), ID);
	}
	//这里的name Wid Hei 不用全名是因为警告"隐藏了全局声明"
	VOID CreateFrame(INT Left, INT Top, INT Wid, INT Hei, INT Page, LPCWSTR name)
	{
		CurFrame++;
		Frame[CurFrame].Left = Left;
		Frame[CurFrame].Page = Page;
		Frame[CurFrame].Height = Hei;
		Frame[CurFrame].Top = Top;
		Frame[CurFrame].Width = Wid;
		wcscpy_s(Frame[CurFrame].Name, name);
	}

	VOID CreateCheck(INT Left, INT Top, INT Page, INT Wid, LPCWSTR name)
	{
		CurCheck++;
		Check[CurCheck].Left = Left;
		Check[CurCheck].Top = Top;
		Check[CurCheck].Page = Page;
		Check[CurCheck].Width = Wid;
		wcscpy_s(Check[CurCheck].Name, name);
	}

	VOID CreateText(INT Left, INT Top, INT Page, LPCWSTR name, COLORREF rgb)
	{
		CurText++;
		Text[CurText].Left = Left;
		Text[CurText].Top = Top;
		Text[CurText].Page = Page;
		Text[CurText].rgb = rgb;
		wcscpy_s(Text[CurText].Name, name);
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

	BOOL InsideButton(int bzz, POINT &point)
	{
		return (Button[bzz].Left*DPI <= point.x &&
			Button[bzz].Top*DPI <= point.y &&
			(long)((Button[bzz].Left + Button[bzz].Width)*DPI) >= point.x
			&& (long)((Button[bzz].Top + Button[bzz].Height) *DPI) >= point.y);
	}

	int InsideCheck(int cur, POINT &point)
	{
		if (Check[cur].Left*DPI <= point.x&&Check[cur].Top*DPI <= point.y&&Check[cur].Left*DPI + 15 * DPI + 1 >= point.x
			&&Check[cur].Top*DPI + 15 * DPI + 1 >= point.y)return 1;

		if (Check[cur].Left*DPI <= point.x&&Check[cur].Top*DPI <= point.y&&Check[cur].Left*DPI + Check[cur].Width*DPI >= point.x
			&&Check[cur].Top*DPI + 15 * DPI + 1 >= point.y)return 2;
		return 0;
	}
	void DrawFrames(int cur)
	{
		int i;
		if (cur != 0)
		{
			i = cur;
			goto begin;
		}
		for (i = 1; i <= CurFrame; i++)
		{
		begin:
			if (Frame[i].Page == CurWnd || Frame[i].Page == 0)
			{
				HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

				SelectObject(hdc, pen);
				SelectObject(hdc, DefFont);
				MoveToEx(hdc, Frame[i].Left*DPI, Frame[i].Top*DPI, NULL);
				LineTo(hdc, Frame[i].Left*DPI, Frame[i].Top*DPI + Frame[i].Height*DPI);
				LineTo(hdc, Frame[i].Left*DPI + Frame[i].Width*DPI, Frame[i].Top*DPI + Frame[i].Height*DPI);
				LineTo(hdc, Frame[i].Left*DPI + Frame[i].Width*DPI, Frame[i].Top*DPI);
				LineTo(hdc, Frame[i].Left*DPI, Frame[i].Top *DPI);
				SetTextColor(hdc, Frame[i].rgb);
				RECT rc = GetRECTf(i);
				SetBkMode(hdc, OPAQUE);
				DrawTextW(hdc, Frame[i].Name, (int)wcslen(Frame[i].Name), &rc, NULL);
				SetTextColor(hdc, NULL);
			}
			if (cur != 0)return;
		}

	}
	void DrawButtons(int cur)
	{
		int i;
		if (cur != 0)
		{
			i = cur;
			goto begin;
		}
		for (i = 1; i <= CurButton; i++)
		{
		begin:
			if (Button[i].Page == CurWnd || Button[i].Page == 0)
			{
				HPEN tmp = 0; HBRUSH tmb = 0;
				SetTextColor(hdc, Button[i].FontRGB);
				if (Button[i].Percent != 0 && Button[i].Percent != 100 && Button[i].Download == 0)
				{
					tmp = CreatePen(PS_SOLID, 1, RGB((Button[i].p2[0] - Button[i].p1[0])*Button[i].Percent / 100 + Button[i].p1[0], \
						(Button[i].p2[1] - Button[i].p1[1])*Button[i].Percent / 100 + Button[i].p1[1], (Button[i].p2[2] - Button[i].p1[2])*Button[i].Percent / 100 + Button[i].p1[2]));

					SelectObject(hdc, tmp);
					tmb = CreateSolidBrush(RGB((Button[i].b2[0] - Button[i].b1[0])*Button[i].Percent / 100 + Button[i].b1[0], \
						(Button[i].b2[1] - Button[i].b1[1])*Button[i].Percent / 100 + Button[i].b1[1], (Button[i].b2[2] - Button[i].b1[2])*Button[i].Percent / 100 + Button[i].b1[2]));
					SelectObject(hdc, tmb);
					goto ok;
				}
				if (CurCover == i && Button[i].Download == 0)
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
			ok:
				if (Button[i].Font == NULL)SelectObject(hdc, DefFont); else SelectObject(hdc, Button[i].Font);

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
						DrawTextW(hdc, GetStr(L"Loaded"), 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						Button[i].Download = 0;
					}
					else
						DrawTextW(hdc, GetStr(L"Loading"), 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				if (tmp != NULL)DeleteObject(tmp);
				if (tmb != NULL)DeleteObject(tmb);
			}

			if (cur != 0)return;
		}
		SetTextColor(hdc, RGB(0, 0, 0));

	}
	void DrawChecks(int cur)
	{
		int i;
		if (cur != 0)
		{
			i = cur;
			goto begin;
		}
		for (i = 1; i <= CurCheck; i++)
		{
		begin:
			if (Check[i].Page == 0 || Check[i].Page == CurWnd)
			{
				if (i == CoverCheck)SelectObject(hdc, BLUE); else SelectObject(hdc, LGREY);

				SelectObject(hdc, grey);
				SelectObject(hdc, DefFont);
				Rectangle(hdc, Check[i].Left*DPI, Check[i].Top*DPI, Check[i].Left*DPI + 15 * DPI, Check[i].Top*DPI + 15 * DPI);
				TextOut(hdc, Check[i].Left*DPI + 20 * DPI, Check[i].Top*DPI, Check[i].Name, (int)wcslen(Check[i].Name));
				if (Check[i].Value == 1)//打勾
				{
					SelectObject(hdc, GREEN);
					MoveToEx(hdc, Check[i].Left*DPI + 2 * DPI, Check[i].Top*DPI + 7 * DPI, NULL);
					LineTo(hdc, Check[i].Left*DPI + 7 * DPI, Check[i].Top*DPI + 12 * DPI);
					LineTo(hdc, Check[i].Left*DPI + 12 * DPI, Check[i].Top*DPI + 2 * DPI);
				}
			}
			if (cur != 0)return;
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
				SelectObject(hdc, DefFont);

				wchar_t *tmp = str[Hash(Text[i].Name)];
				TextOutW(hdc, Text[i].Left*DPI, Text[i].Top*DPI, tmp, (int)wcslen(tmp));
			}
		}
	}
	void DrawExp()
	{
		if (ExpExist == false)return;
		SelectObject(hdc, DefFont);
		SelectObject(hdc, YELLOW);
		SelectObject(hdc, yellow);
		if (ExpPoint.x > Width / 2)ExpPoint.x -= (ExpWidth + 6); else ExpPoint.x += 12;
		if (ExpPoint.y > Height / 2)ExpPoint.y -= (ExpHeight + 6); else ExpPoint.y += 14;
		Rectangle(hdc, ExpPoint.x, ExpPoint.y, ExpWidth + ExpPoint.x, ExpHeight + ExpPoint.y);
		SetTextColor(hdc, RGB(0, 0, 0));
		for (int i = 1; i <= ExpLine; i++)
			TextOutW(hdc, ExpPoint.x + 4, ExpPoint.y - 12 * DPI + 16 * i*DPI, Exp[i], (int)wcslen(Exp[i]));
	}

	void DrawEdits(int cur)
	{
		int i;
		HDC mdc;
		mdc = CreateCompatibleDC(hdc);

		SelectObject(mdc, bitmap);
		SetBkMode(mdc, TRANSPARENT);
		//ReleaseDC(hWnd, hdc);
		if (cur != 0)
		{
			i = cur;
			goto begin;
		}


		for (i = 1; i <= CurEdit; ++i)
		{
		begin:
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
			{
				SelectObject(mdc, White);
				SelectObject(mdc, WhiteBrush);
				Rectangle(mdc, 0, 0, 4000, 400);
				SelectObject(hdc, WhiteBrush);
				if (i == CoverEdit)SelectObject(hdc, BLUE); else SelectObject(hdc, BLACK);
				Rectangle(hdc, (Edit[i].Left - 5)*DPI, Edit[i].Top*DPI, (Edit[i].Left + Edit[i].Width + 5)*DPI, (Edit[i].Top + Edit[i].Height)*DPI);
				SIZE sel, ser;
				int pos1, pos2;
				if (Edit[i].Pos1 > Edit[i].Pos2&&Edit[i].Pos2 != -1)pos1 = Edit[i].Pos2, pos2 = Edit[i].Pos1; else pos1 = Edit[i].Pos1, pos2 = Edit[i].Pos2;

				if (Edit[i].font != NULL)SelectObject(mdc, Edit[i].font); else SelectObject(mdc, DefFont);
				if (pos2 == -1)
				{
					TextOutW(mdc, 0, 4, Edit[i].str, wcslen(Edit[i].str));
					goto next;
				}
				GetTextExtentPoint32(mdc, Edit[i].str, pos1, &sel);
				GetTextExtentPoint32(mdc, Edit[i].str, pos2, &ser);
				SelectObject(mdc, BLUE);
				SelectObject(mdc, BlueBrush);
				Rectangle(mdc, sel.cx, 0, ser.cx, ser.cy + 5 * DPI);
				SetTextColor(mdc, RGB(0, 0, 0));
				TextOutW(mdc, 0, 4, Edit[i].str, pos1);
				SetTextColor(mdc, RGB(255, 255, 255));
				TextOutW(mdc, sel.cx, 4, &Edit[i].str[pos1], pos2 - pos1);
				SetTextColor(mdc, RGB(0, 0, 0));
				TextOutW(mdc, ser.cx, 4, &Edit[i].str[pos2], wcslen(&Edit[i].str[pos2]));
			next:
				int yMax = (Edit[i].Top + Edit[i].Height / 2)* DPI - 4 - Edit[i].strHeight / 2;
				if (yMax < Edit[i].Top*DPI + 1)yMax = Edit[i].Top*DPI + 1;
				if (Edit[i].XOffset == 0)
					BitBlt(hdc, (Edit[i].Left + Edit[i].Width / 2)*DPI - Edit[i].strWidth / 2, yMax
						, Edit[i].strWidth \
						, Edit[i].strHeight + 4, mdc, 0, 0, SRCCOPY);
				else
					//s(0),
					BitBlt(hdc, Edit[i].Left*DPI, yMax
						, Edit[i].Width *DPI\
						, Edit[i].strHeight + 4, mdc, Edit[i].XOffset, 0, SRCCOPY);
			}
			if (cur != 0)goto end;
		}
	end:
		DeleteDC(mdc);
	}

	void RedrawObject()
	{
		if (RedrawType == 1)DrawFrames(RedrawCur);
		if (RedrawType == 2)DrawButtons(RedrawCur);
		if (RedrawType == 3)DrawChecks(RedrawCur);
		if (RedrawType == 5)DrawEdits(RedrawCur);
		DrawExp();
		RedrawType = RedrawCur = 0;
	}

	void DrawEVERYTHING()
	{
		DrawFrames(NULL);
		DrawButtons(NULL);
		DrawChecks(NULL);
		DrawLines();
		DrawTexts();
		DrawEdits(NULL);
		DrawExp();
	}
	RECT GetRECT(int cur)//更新rc
	{
		RECT rc;
		rc.left = Button[cur].Left*DPI;
		rc.right = Button[cur].Left*DPI + Button[cur].Width*DPI;
		rc.top = Button[cur].Top*DPI;
		rc.bottom = Button[cur].Top*DPI + Button[cur].Height*DPI;
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
	void SetEditStrOrFont(LPCWSTR Newstr, HFONT font, int cur)
	{
		HDC mdc;
		HBITMAP bmp;
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(mdc, bmp);
		ReleaseDC(hWnd, hdc);
		if (font != NULL) Edit[cur].font = font;
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font); else SelectObject(mdc, DefFont);
		if (Newstr != NULL && wcslen(Newstr) <= 1000)wcscpy_s(Edit[cur].str, Newstr);
		SIZE se;
		GetTextExtentPoint32(mdc, Edit[cur].str, wcslen(Edit[cur].str), &se);
		Edit[cur].strWidth = se.cx; if (se.cy != 0) Edit[cur].strHeight = se.cy;
		if (Edit[cur].Width*DPI < se.cx)Edit[cur].XOffset = (se.cx - Edit[cur].Width*DPI) / 2; else Edit[cur].XOffset = 0;
		ReleaseDC(hWnd, mdc);
		DeleteDC(mdc);
		DeleteObject(bmp);
	}
	int GetNearestChar(int cur, POINT Point)
	{
		POINT point;
		if (Edit[cur].strWidth <= Edit[cur].Width*DPI)
		{
			if (Point.x < (long)((Edit[cur].Left + Edit[cur].Width / 2)*DPI - Edit[cur].strWidth / 2))return 0;
			if (Point.x > (long)((Edit[cur].Left + Edit[cur].Width / 2)*DPI + Edit[cur].strWidth / 2))return wcslen(Edit[cur].str);
			point.x =Point.x- (long)((Edit[cur].Left + Edit[cur].Width / 2)*DPI) + Edit[cur].strWidth / 2;
		}
		else
			point.x = Point.x - Edit[cur].Left*DPI + Edit[cur].XOffset;
		HDC mdc;
		HBITMAP bmp;
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		if (hdc == NULL)s(0);
		SelectObject(mdc, bmp);
		ReleaseDC(hWnd, hdc);
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font); else SelectObject(mdc, DefFont);
		int l = wcslen(Edit[cur].str), sum = 0, pos = -1;
		for (int i = 0; i <= l - 1; i++)
		{
			pos++;
			SIZE se = { 0 };
			GetTextExtentPoint32(mdc, &Edit[cur].str[i], 1, &se);
			sum += se.cx;
			if (sum >= point.x)  break;
		}
		SIZE sel, ser; //int t;
		GetTextExtentPoint32(mdc, Edit[cur].str, pos, &sel);
		GetTextExtentPoint32(mdc, Edit[cur].str, pos + 1, &ser);
		//if (point.x < (sel.cx + ser.cx) / 2)t = sel.cx; else t = ser.cx;

		DeleteDC(mdc);
		DeleteObject(bmp);
		//s(l);
		if (point.x < (sel.cx + ser.cx) / 2)return pos; else return pos + 1;
	}
	VOID EditPaste(int cur)
	{
		if (cur == 0)return;
		if (CoverEdit == 0)return;
		char *buffer = NULL;
		if (OpenClipboard(hWnd))
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			buffer = (char *)GlobalLock(hData);
			GlobalUnlock(hData);
			CloseClipboard();
		}
		wchar_t Buffer[1001] = { 0 };
		if (buffer != NULL)charTowchar(buffer, Buffer, strlen(buffer) * 2);
		wchar_t zxf[2001] = { 0 };
		int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2), pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		if (pos1 == -1)
		{
			wchar_t t = Edit[cur].str[pos2];
			Edit[cur].str[pos2] = '\0';
			wcscpy_s(zxf, Edit[cur].str);
			wcscat_s(zxf, Buffer);
			Edit[cur].str[pos2] = t;
			wcscat_s(zxf, &Edit[cur].str[pos2]);
			Edit[cur].Pos1 += wcslen(Buffer);
			SetEditStrOrFont(zxf, 0, cur);
			RefreshXOffset(cur);
			RefreshCaretByPos(cur);
		}
		else
		{
			Edit[cur].str[pos1] = '\0';
			wcscpy_s(zxf, Edit[cur].str);
			wcscat_s(zxf, Buffer);
			wcscat_s(zxf, &Edit[cur].str[pos2]);
			Edit[cur].Pos1 += wcslen(Buffer);
			Edit[cur].Pos2 = -1;
			SetEditStrOrFont(zxf, 0, cur);
			RefreshXOffset(cur);
			RefreshCaretByPos(cur);
		}
		EditRedraw(cur);
	}
	VOID EditHotKey(int wParam)
	{
		switch (wParam)
		{
		case 34:
			EditMove(CoverEdit, -1);
			break;
		case 35:
			EditMove(CoverEdit, 1);
			break;
		case 36:
			EditPaste(CoverEdit);
			break;
		case 37:
			EditCopy(CoverEdit);
			break;
		case 38:
			if (Edit[CoverEdit].Pos2 != -1)
			{
				EditCopy(CoverEdit);
				EditDelete(CoverEdit, min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2), \
					max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2));
			}
			break;
		case 39:
			EditAll(CoverEdit);
			break;
		}
		return;
	}
	VOID EditUnHotKey()
	{
		for (int i = 34; i < 40; ++i)UnregisterHotKey(hWnd, i);
		HideCaret(hWnd);
	}
	VOID EditRegHotKey()
	{
		RegisterHotKey(hWnd, 34, NULL, VK_LEFT);
		RegisterHotKey(hWnd, 35, NULL, VK_RIGHT);
		RegisterHotKey(hWnd, 36, MOD_CONTROL, 'V');
		RegisterHotKey(hWnd, 37, MOD_CONTROL, 'C');
		RegisterHotKey(hWnd, 38, MOD_CONTROL, 'X');
		RegisterHotKey(hWnd, 39, MOD_CONTROL, 'A');
		DestroyCaret();
		CreateCaret(hWnd, NULL, 1, 20 * DPI);
	}
	VOID EditCHAR(wchar_t wParam)
	{
		if (Edit[CoverEdit].Press == true || CoverEdit == 0)return;
		if (wParam >= 0x20 && wParam != 0x7F)
		{
			int pos = min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			int pos2 = max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			if (pos == -1)
				EditAdd(CoverEdit, pos2, pos2, wParam);
			else
				EditAdd(CoverEdit, pos, pos2, wParam);
		}
		else if (wParam == VK_BACK)
		{
			int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else
				EditDelete(CoverEdit, pos1 - 1, pos1);
		}
		return;
	}
	VOID EditAdd(int cur, int Left, int Right, wchar_t Char)
	{
		wchar_t zxf[1001] = { 0 }, t = 0;
		if (Left == Right)t = Edit[cur].str[Left];
		Edit[cur].str[Left] = '\0';
		wcscpy_s(zxf, Edit[cur].str);
		zxf[Left] = Char;
		Edit[cur].str[Left] = t;
		wcscat(zxf, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left + 1;
		Edit[cur].Pos2 = -1;
		ZeroMemory(Edit[cur].str, sizeof(Edit[cur].str));
		SetEditStrOrFont(zxf, 0, cur);
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}

	VOID EditDelete(int cur, int Left, int Right)
	{
		if (Left == -1)return;
		wchar_t zxf[1001] = { 0 };
		Edit[cur].str[Left] = '\0';
		wcscpy_s(zxf, Edit[cur].str);
		wcscat(zxf, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left;
		Edit[cur].Pos2 = -1;
		SetEditStrOrFont(zxf, 0, cur);
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}

	VOID EditAll(int cur)
	{
		Edit[cur].Pos1 = 0;
		Edit[cur].Pos2 = wcslen(Edit[cur].str);
		RefreshXOffset(CoverEdit);
		RefreshCaretByPos(CoverEdit);
		EditRedraw(cur);
	}

	VOID EditMove(int cur, int off)
	{
		int xback;
		xback = Edit[cur].XOffset;
		if (Edit[cur].Pos2 != -1)return;
		Edit[cur].Pos1 += off;
		if (Edit[cur].Pos1 < 0)Edit[cur].Pos1 = 0;
		if ((unsigned int)Edit[cur].Pos1 > wcslen(Edit[cur].str))Edit[cur].Pos1 = wcslen(Edit[cur].str);
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		//ShowCaret(hWnd);

	}
	VOID EditCopy(int cur)
	{
		if (cur == 0)return;
		wchar_t source[1001] = { 0 }, t;
		char Source[2001] = { 0 };
		if (Edit[cur].Pos2 == -1)return;
		int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2);
		int pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		t = Edit[cur].str[pos2];
		Edit[cur].str[pos2] = '\0';
		wcscpy_s(source, Edit[cur].str + pos1);
		Edit[cur].str[pos2] = t;
		wcharTochar(source, Source, sizeof(Source));

		if (OpenClipboard(hWnd))
		{
			HGLOBAL clipbuffer;
			char * buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(Source) + 1);
			buffer = (char*)GlobalLock(clipbuffer);
			if (buffer != NULL)strcpy(buffer, LPCSTR(Source));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
	}
	VOID RefreshCaretByPos(int cur)
	{
		if (Edit[cur].Pos1 == -1)return;
		HDC mdc;
		HBITMAP bmp;
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(mdc, bmp);
		ReleaseDC(hWnd, hdc);
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font); else SelectObject(mdc, DefFont);
		SIZE se;
		if (Edit[cur].Pos2 != -1)
			GetTextExtentPoint32(mdc, Edit[cur].str, Edit[cur].Pos2, &se);
		else
			GetTextExtentPoint32(mdc, Edit[cur].str, Edit[cur].Pos1, &se);
		if (Edit[cur].XOffset == 0)
			SetCaretPos(se.cx + (long)((Edit[cur].Left + Edit[cur].Width / 2)*DPI) - Edit[cur].strWidth / 2, (Edit[cur].Top + Edit[cur].Height / 2 - 4) *DPI - Edit[cur].strHeight / 2);
		else
			SetCaretPos(se.cx + Edit[cur].Left*DPI - Edit[cur].XOffset, (Edit[cur].Top + Edit[cur].Height / 2 - 4)*DPI - Edit[cur].strHeight / 2);
		ShowCaret(hWnd);
		DeleteDC(mdc);
		DeleteObject(bmp);
	}
	VOID EditDown(int cur)
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CoverEdit != cur)Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1, EditRedraw(CoverEdit);
		CoverEdit = cur;
		Edit[cur].Pos1 = Edit[cur].Pos2 = -1;
		Edit[cur].Press = true;
		Edit[cur].Pos1 = GetNearestChar(cur, point);
		//s(point.x);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	RECT GetRECTe(int cur)
	{
		RECT rc;
		rc.left = (Edit[cur].Left - 5)*DPI;
		rc.right = (Edit[cur].Left + Edit[cur].Width + 5)*DPI;
		rc.top = Edit[cur].Top*DPI;
		rc.bottom = (Edit[cur].Top + Edit[cur].Height)*DPI;
		return rc;
	}
	RECT GetRECTc(int cur)
	{
		RECT rc;
		rc.left = Check[cur].Left*DPI;
		rc.top = Check[cur].Top*DPI;
		rc.right = Check[cur].Left *DPI + 15 * DPI;
		rc.bottom = Check[cur].Top *DPI + 15 * DPI;
		return rc;
	}
	inline BOOL InsideEdit(int cur, POINT point)
	{
		return ((Edit[cur].Left - 5)*DPI <= point.x &&Edit[cur].Top*DPI <= point.y && (long)((Edit[cur].Left + Edit[cur].Width + 5)*DPI) >= point.x && (Edit[cur].Top + Edit[cur].Height)*DPI >= point.y);
	}

	VOID LeftButtonDown()
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CurCover != -1)
		{
			Press = 1;
			RECT rc;
			rc = GetRECT(CurCover);
			if (Obredraw)
				RedrawType = 2,
				RedrawCur = CurCover;
			InvalidateRect(hWnd, &rc, FALSE);
		}
		if (CoverEdit != 0)
		{
			if (InsideEdit(CoverEdit, point))Edit[CoverEdit].Press = true;
		}
		for (int i = 1; i <= CurEdit; i++)
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
				if (InsideEdit(i, point) != 0) { EditDown(i);  break; }
		Timer = GetTickCount();
		DestroyExp();
	}
	void CheckGetNewInside(POINT &point)
	{
		for (int i = 1; i <= CurCheck; i++)
		{
			if (Check[i].Page == CurWnd || Check[i].Page == 0)
			{
				if (InsideCheck(i, point) != 0)
				{
					CoverCheck = i;
					if (Obredraw)
						RedrawType = 3,
						RedrawCur = i;
					RECT rc = GetRECTc(i);
					InvalidateRect(hWnd, &rc, FALSE);
					break;
				}
			}
		}
	}
	void ButtonGetNewInside(POINT &point)
	{
		for (int i = 0; i <= CurButton; ++i)
		{
			if (Button[i].Page == CurWnd || Button[i].Page == 0)
			{
				if (InsideButton(i, point))
				{
					CurCover = i;
					if (ButtonEffect)
					{
						Button[i].Percent += 40;
						if (Button[i].Percent > 100)Button[i].Percent = 100;
					}
					if (Obredraw)
						RedrawType = 2,
						RedrawCur = i;
					RECT rc = GetRECT(i);
					InvalidateRect(hWnd, &rc, FALSE);
					return;
				}
			}
		}
	}
	VOID MouseMove()
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CurCover == -1)ButtonGetNewInside(point);
		if (CurCover >= 0)
		{
			if (Button[CurCover].Page == CurWnd || Button[CurCover].Page == 0)
			{
				if (!InsideButton(CurCover, point))
				{
					if (Obredraw)
						RedrawType = 2,
						RedrawCur = CurCover;
					if (ButtonEffect)
					{
						Button[CurCover].Percent -= Delta;
						if (Button[CurCover].Percent < 0)Button[CurCover].Percent = 0;
					}
					RECT rc = GetRECT(CurCover);
					InvalidateRect(hWnd, &rc, FALSE);
					CurCover = -1;
					UpdateWindow(hWnd);

					ButtonGetNewInside(point);
				}
			}
			else CurCover = -1;
		}
		if (CoverCheck == 0)CheckGetNewInside(point);//在外面

		if (CoverCheck > 0)
		{
			if (Check[CoverCheck].Page == CurWnd || Check[CoverCheck].Page == 0)
			{
				if (InsideCheck(CoverCheck, point) == 0)
				{
					if (Obredraw)RedrawType = 3, RedrawCur = CoverCheck;
					RECT rc = GetRECTc(CoverCheck);
					InvalidateRect(hWnd, &rc, FALSE);
					CoverCheck = 0;
					UpdateWindow(hWnd);
				}
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
		if (CoverEdit != 0 && Edit[CoverEdit].Press == true)
		{
			int t = Edit[CoverEdit].Pos2;
			Edit[CoverEdit].Pos2 = GetNearestChar(CoverEdit, point);
			//if (Edit[CoverEdit].Pos2 == 0)
			RefreshCaretByPos(CoverEdit);
			if (Edit[CoverEdit].Pos2 == Edit[CoverEdit].Pos1) { Edit[CoverEdit].Pos2 = -1; goto end; }
			if (Edit[CoverEdit].Pos2 != t && Edit[CoverEdit].Width < Edit[CoverEdit].strWidth && !InsideEdit(CoverEdit, point))RefreshXOffset(CoverEdit);
			if (Edit[CoverEdit].Pos2 != t)EditRedraw(CoverEdit);
		}
	end:
		if (point.x != ExpPoint2.x || point.y != ExpPoint2.y)
		{
			//out(point.x);
			//out(ExpPoint2.x);
			//out(0);
			ExpPoint2 = point;
			Timer = GetTickCount();
			DestroyExp();
		}
	}
	void EditRedraw(int cur)
	{
		if (Obredraw)
			RedrawType = 5,
			RedrawCur = cur;
		RECT rc = GetRECTe(cur);
		InvalidateRect(hWnd, &rc, TRUE);
	}
	void InfoBox(LPCWSTR Str)
	{
		if (!slient)MessageBox(hWnd, Str, GetStr(L"Info"), MB_ICONINFORMATION);
		else printf("%ls\n", Str);
	}
	VOID RefreshXOffset(int cur)
	{
		if (Edit[cur].strWidth < Edit[cur].Width*DPI) {
			Edit[cur].XOffset = 0; return;
		}
		HDC mdc;
		HBITMAP bmp;
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(mdc, bmp);
		ReleaseDC(hWnd, hdc);
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font); else SelectObject(mdc, DefFont);
		SIZE se;

		if (Edit[cur].Pos2 != -1)
			GetTextExtentPoint32(mdc, Edit[cur].str, Edit[cur].Pos2, &se);
		else
			GetTextExtentPoint32(mdc, Edit[cur].str, Edit[cur].Pos1, &se);
		if (se.cx < Edit[cur].XOffset)Edit[cur].XOffset = se.cx - 1;// s(se.cx);
		if (se.cx > Edit[cur].XOffset + Edit[cur].Width*DPI)Edit[cur].XOffset += se.cx - (Edit[cur].XOffset + Edit[cur].Width*DPI) - 1;
		if (Edit[cur].strWidth - Edit[cur].XOffset < Edit[cur].Width*DPI)Edit[cur].XOffset = Edit[cur].strWidth - Edit[cur].Width*DPI;
		RefreshCaretByPos(cur);
		EditRedraw(cur);
		DeleteDC(mdc);
		DeleteObject(bmp);
	}
	inline INT GetNumByIDe(LPCWSTR ID)
	{
		for (int i = 1; i <= CurEdit; ++i)if (wcscmp(ID, Edit[i].ID) == 0)return i;
		return 0;
	}
	VOID SetPage(INT Page)
	{
		if (Page == CurWnd)return;
		HideCaret(hWnd);
		Edit[CoverEdit].Press = false;
		Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1;
		CoverEdit = 0;
		CurWnd = Page;

		InvalidateRect(hWnd, 0, FALSE);
	}
	VOID SetDPI(DOUBLE NewDPI)
	{
		DPI = NewDPI;
		DefFont = CreateFontW(16 * DPI, 8 * DPI, 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
		for (int i = 1; i <= CurEdit; ++i)SetEditStrOrFont(0, 0, i), RefreshXOffset(i);
		RefreshCaretByPos(CoverEdit);
		SetWindowPos(hWnd, NULL, 0, 0, Width * DPI, Height* DPI, SWP_NOMOVE | SWP_NOREDRAW);
		DestroyCaret();
		CreateCaret(hWnd, NULL, 1, 20 * DPI);

		RedrawType = 0;
		InvalidateRect(hWnd, NULL, false);
		UpdateWindow(hWnd);
	}

	VOID CreateMain(INT Left, INT Top, INT Wid, INT Hei, LPCWSTR name, LONG Defs, bool obredraw)
	{
		Obredraw = obredraw;
		Width = Wid;
		Height = Hei;
		hWnd = CreateWindowW(szWindowClass, name, Defs, Left, Top, Wid * DPI, Hei * DPI, NULL, nullptr, hInstance, nullptr);
		Timer = GetTickCount();
		//s(Timer);
		SetTimer(hWnd, 4, 100, (TIMERPROC)TimerProc);
		CreateCaret(hWnd, NULL, 1, 20);
		SetCaretBlinkTime(500);
		GetCursorPos(&ExpPoint2);
		ScreenToClient(hWnd, &ExpPoint2);
	}

	LPWSTR GetCurInsideID()
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		for (int i = 0; i <= CurButton; i++)
			if (Button[i].Page == CurWnd || Button[i].Page == 0)
				if (InsideButton(i, point) && Button[i].Download == 0)
					return Button[i].ID;
		return Button[0].ID;
	}
	INT GetNumbyID(LPCWSTR ID)
	{
		return but[Hash(ID)];
	}
	void SetHDC(HDC HDc)
	{
		hdc = HDc;
		bitmap = CreateCompatibleBitmap(hdc, 10000, 200);
	}
	void Try2CreateExp()
	{
		if (ExpExist == true || CurCover == -1)return;
		if (wcslen(Button[CurCover].Exp) == 0)return;
		CurButtonBack = CurButton;
		ExpExist = true;
		ExpLine = 0;
		ZeroMemory(Exp, sizeof(Exp));


		wchar_t *x = Button[CurCover].Exp, *y = Button[CurCover].Exp;
		ExpHeight = 8;
		ExpWidth = 0;
		while (1)
		{
			ExpLine++;
			x = wcsstr(x + 1, L"\\n");
			if (x != 0)x[0] = '\0';
			if (ExpLine == 1)wcscpy(Exp[ExpLine], y); else wcscpy(Exp[ExpLine], y + 2);
			//s(Exp[ExpLine]);
			SIZE *se = new SIZE;
			if (ExpLine == 1)GetTextExtentPoint32(hdc, y, wcslen(y), se); else GetTextExtentPoint32(hdc, y + 2, wcslen(y + 2), se);//获取字符串的宽度
			if (x != 0)x[0] = '\\';
			ExpHeight += se->cy;
			ExpWidth = max(ExpWidth - 8, se->cx) + 8;
			if (x == 0)break;
			y = x;
		}
		//ExpHeight = 108;
		//ExpWidth = 100;
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		ExpPoint = point;
		InvalidateRect(hWnd, 0, 0);
	}
	VOID DestroyExp()
	{
		if (ExpExist == false)return;
		ExpExist = false;
		ExpLine = ExpHeight = ExpWidth = 0;
		InvalidateRect(hWnd, 0, 0);
	}
	int ExpLine = 0;
	int ExpHeight;
	int ExpWidth;
	wchar_t Exp[101][301];
	POINT ExpPoint, ExpPoint2;
	struct ButtonEx
	{
		long Left, Top, Width, Height, Page, Download, Percent;
		bool Visible, Enabled;
		HBRUSH Leave, Hover, Press;
		HPEN Leave2, Hover2, Press2;
		HFONT Font;
		wchar_t Name[31], ID[11], Exp[301];
		COLORREF FontRGB;
		BYTE b1[3], b2[3], p1[3], p2[3];
	}Button[100];

	struct FrameEx
	{
		int Left, Top, Width, Height, Page;
		COLORREF rgb;
		wchar_t Name[51];
	}Frame[20];
	struct CheckEx
	{
		int Left, Top, Page, Width;//width跟绘制没什么关系，见Insidec函数
		bool Value;
		wchar_t Name[51];
	}Check[20];
	struct LineEx
	{
		int StartX, StartY, EndX, EndY, Page;
	}Line[10];

	struct TextEx
	{
		int Left, Top, Page;
		COLORREF rgb;
		wchar_t Name[201];
	}Text[20];

	struct EditEx
	{
		int Left, Top, Width, Height, Page;
		int strWidth, strHeight, Pos1, Pos2, XOffset;
		bool Press;
		wchar_t str[1001], ID[11];
		HFONT font;
	}Edit[10];
	struct GUIString
	{
		wchar_t str[301], ID[11];
	}string[100];
	std::unordered_map<unsigned int, wchar_t*> str;
	std::unordered_map<unsigned int, int>but;
	HFONT DefFont;
	int Msv;
	int CurString;
	int CurButton;
	int CurFrame;
	int CurCheck;
	int CurLine;
	int CurText;
	int CurEdit;
	double DPI;
	int CurCover;
	bool Obredraw = false;
	bool ButtonEffect = false;
	int CoverCheck;
	int CurWnd;
	int CoverEdit;
	int Press;
	int RedrawType;//1 = Frame  2 = Button  3 = Check  4 = Explainations  5 = Edit
	int RedrawCur;
	HDC hdc;
	HBITMAP bitmap;
	int Width, Height;
	HWND hWnd;
	HINSTANCE hInstance;
	DWORD Timer;
	BOOL ShowExp = FALSE;
	int CurButtonBack;
	bool ExpExist = false;
private:

}Main, CatchWnd;

DWORD ShellCreateInVDesk(PTSTR szName)
{
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.lpDesktop = szVDesk;
	PROCESS_INFORMATION pi;
	if (!CreateProcess(NULL, szName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		Main.InfoBox(Main.GetStr(L"StartFail"));
	return pi.dwProcessId;
}
class DownloadProgress : public IBindStatusCallback {
public:
	TCHAR curi[11];
	HRESULT __stdcall QueryInterface(const IID &, void **) { return E_NOINTERFACE; }
	ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }//暂时没用的函数，从msdn上抄下来的
	ULONG STDMETHODCALLTYPE Release(void) { return 1; }
	HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, IBinding *pib) { UNREFERENCED_PARAMETER(dwReserved); UNREFERENCED_PARAMETER(pib); return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG *pnPriority) { UNREFERENCED_PARAMETER(pnPriority); return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved) { UNREFERENCED_PARAMETER(reserved); return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, LPCWSTR szError) { UNREFERENCED_PARAMETER(hresult); UNREFERENCED_PARAMETER(szError); return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo) { UNREFERENCED_PARAMETER(grfBINDF); UNREFERENCED_PARAMETER(pbindinfo); return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed) {
		UNREFERENCED_PARAMETER(grfBSCF); UNREFERENCED_PARAMETER(dwSize); UNREFERENCED_PARAMETER(pformatetc); UNREFERENCED_PARAMETER(pstgmed); return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID riid, IUnknown *punk) { UNREFERENCED_PARAMETER(riid); UNREFERENCED_PARAMETER(punk); return E_NOTIMPL; }

	virtual HRESULT __stdcall OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		UNREFERENCED_PARAMETER(ulStatusCode);
		UNREFERENCED_PARAMETER(szStatusText);
		if (ulProgressMax != 0)
		{
			double percentage = double(ulProgress * 1.0 / ulProgressMax * 100);
			if (Main.Button[Main.GetNumbyID(curi)].Download != (int)percentage + 1)
			{
				Main.Button[Main.GetNumbyID(curi)].Download = (int)percentage + 1;
				RECT rc = Main.GetRECT(Main.GetNumbyID(curi));
				Main.RedrawType = 2;
				Main.RedrawCur = Main.GetNumbyID(curi);
				InvalidateRect(Main.hWnd, &rc, FALSE);
			}
		}
		return S_OK;
	}
};
BOOL CALLBACK EnumFunc(HWND hwnd, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID;
	LONG A;
	A = GetWindowLongW(hwnd, GWL_STYLE) & WS_VISIBLE;
	if (A != 0 && GetParent(hwnd) == NULL)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		for (unsigned int i = 1; i <= tdpid[0]; i++)if (tdpid[i] == nProcessID) { tdh[++tdhcur] = hwnd; }

	}
	return 1;
}
void FindTop(wchar_t *name)
{
	name[3] = 0;
	_wcslwr(name);
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); int Cur = 0;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = '\0';
		_wcslwr(pe.szExeFile);

		if (wcsstr(pe.szExeFile, name) != 0)tdpid[++Cur] = pe.th32ProcessID;
	}
	tdpid[0] = Cur;
	//s(Cur);
	EnumWindows(EnumFunc, NULL);
}


BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID; HANDLE hProcessHandle;
	LONG A;
	A = GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;

	if (A != 0)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		if (CurrentProcessId == nProcessID)goto a;
		for (unsigned int i = 1; i <= expid[0]; i++)if (expid[i] == nProcessID)goto a;
		hProcessHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, nProcessID);
		::TerminateProcess(hProcessHandle, 4);
	}
a:
	return 1;
}
void KillTop()
{
	ZeroMemory(&expid, sizeof(expid));
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); DWORD Cur = 0;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = '\0';
		if (wcsstr(pe.szExeFile, L"exp") != 0)expid[++Cur] = pe.th32ProcessID;
	}
	expid[0] = Cur;
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
wchar_t wip[101];
VOID CheckIP()    //定义checkIP函数，用于取本机的ip地址  
{
	WSADATA wsaData;
	char name[155];
	char *ip;
	PHOSTENT hostinfo;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
				charTowchar(ip, wip, sizeof(wip) * 2);
			}
		}
		WSACleanup();
	}
}


bool haveInfo = false;
void UpdateInfo()
{
	wchar_t tmp[301] = { 0 }; int f;
	GetOSDisplayString(tmp);
	if (Bit == 32)wcscat(Main.GetStr(L"Tbit"), L"32"); else wcscat(Main.GetStr(L"Tbit"), L"64");
	wcscat(Main.GetStr(L"Twinver"), tmp);
	if (Bit == 34)f = GetFileAttributes(L"C:\\windows\\sysnative\\cmd.exe"); else f = GetFileAttributes(L"C:\\windows\\system32\\cmd.exe");
	if (f != -1)wcscat(Main.GetStr(L"Tcmd"), Main.GetStr(L"TcmdOK")); else wcscat(Main.GetStr(L"Tcmd"), Main.GetStr(L"TcmdNO"));
	CheckIP();
	wcscat(Main.GetStr(L"TIP"), wip);
}
void SetFrame()
{
	const int t[] = { 1,4,5,6,8 };
	for (int i = 0; i < 5; ++i)
	{
		Main.Frame[t[i]].rgb = RGB(255, 0, 0);
		wcscat_s(Main.Frame[t[i]].Name, Main.GetStr(L"Useless"));
	}
	Main.Frame[2].rgb = RGB(5, 200, 135);
	wcscat_s(Main.Frame[2].Name, Main.GetStr(L"Usable"));
	Main.Frame[3].rgb = RGB(10, 255, 10);
	wcscat_s(Main.Frame[3].Name, Main.GetStr(L"Rec"));
}
void GetPath()//得到两个路径
{
	GetModuleFileName(NULL, Path, MAX_PATH);
	GetModuleFileName(NULL, Name, MAX_PATH);
	(_tcsrchr(Path, _T('\\')))[1] = 0;
}
VOID GetInfo(__out LPSYSTEM_INFO lpSystemInfo)//得到环境变量
{
	if (NULL == lpSystemInfo)return;
	typedef VOID(WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	HMODULE H = GetModuleHandle(L"Kernel32");
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = 0;
	if (H != NULL)fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(H, "GetNativeSystemInfo");
	if (fnGetNativeSystemInfo != NULL)fnGetNativeSystemInfo(lpSystemInfo); else GetSystemInfo(lpSystemInfo);
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
BOOL Backup()
{
	if (GetFileAttributes(L"C:\\SAtemp") != -1)return FALSE;
	CreateDirectory(L"C:\\SAtemp", NULL);
	if (Bit != 34)
		CopyFile(L"C:\\Windows\\System32\\sethc.exe", L"C:\\SAtemp\\sethc.exe", TRUE);
	else
		CopyFile(L"C:\\Windows\\SysNative\\sethc.exe", L"C:\\SAtemp\\sethc.exe", TRUE);//sysnative
	return TRUE;
}
void SwitchLanguage(LPWSTR name)
{
	__try
	{
		bool Mainf = false, Catchf = false, Buttonf = false, Checkf = false, Stringf = false, \
			Framef = false, Textf = false;
		wchar_t ReadTmp[1001];
		FILE *fp;

		_wfopen_s(&fp, name, L"r,ccs=UNICODE");
		if (fp == NULL)s(1);
		while (!feof(fp))
		{
			fgetws(ReadTmp, 1000, fp);
			//s(ReadTmp);
			if (wcsstr(ReadTmp, L"<Main>") != 0)Mainf = true;
			if (wcsstr(ReadTmp, L"</Main>") != 0)Mainf = false;
			if (wcsstr(ReadTmp, L"<Catch>") != 0)Catchf = true;
			if (wcsstr(ReadTmp, L"</Catch>") != 0)Catchf = false;
			if (wcsstr(ReadTmp, L"<Buttons>") != 0)Buttonf = true;
			if (wcsstr(ReadTmp, L"</Buttons>") != 0)Buttonf = false;
			if (wcsstr(ReadTmp, L"<Checks>") != 0)Checkf = true;
			if (wcsstr(ReadTmp, L"</Checks>") != 0)Checkf = false;
			if (wcsstr(ReadTmp, L"<Strings>") != 0)Stringf = true;
			if (wcsstr(ReadTmp, L"</Strings>") != 0)Stringf = false;
			if (wcsstr(ReadTmp, L"<Frames>") != 0)Framef = true;
			if (wcsstr(ReadTmp, L"</Frames>") != 0)Framef = false;
			if (wcsstr(ReadTmp, L"<Texts>") != 0)Textf = true;
			if (wcsstr(ReadTmp, L"</Texts>") != 0)Textf = false;
			if (ReadTmp[0] != '<')
			{
				wchar_t *pos = wcsstr(ReadTmp, L"=");
				pos[0] = '\0';
				wchar_t *space = wcsstr(ReadTmp, L" ");
				if (space != 0)space[0] = '\0';

				if (Buttonf == true && Mainf == true)
				{
					int cur = Main.GetNumbyID(ReadTmp);
					wchar_t *str1 = wcsstr(pos + 1, L"\"");
					wchar_t *str2 = wcsstr(str1 + 1, L"\"");
					*str2 = '\0';
					wcscpy_s(Main.Button[cur].Name, &str1[1]);
					str1 = wcsstr(str2 + 1, L",");
					str2 = wcsstr(str1 + 1, L",");
					*str2 = '\0';
					int NewLeft = _wtoi(str1 + 1);
					str1 = wcsstr(str2 + 1, L",");
					*str1 = '\0';
					int NewTop = _wtoi(str2 + 1);
					str2 = wcsstr(str1 + 1, L",");
					*str2 = '\0';
					int NewWidth = _wtoi(str1 + 1);
					int NewHeight = _wtoi(str2 + 1);
					if (NewLeft != -1)Main.Button[cur].Left = NewLeft;
					if (NewTop != -1)Main.Button[cur].Top = NewTop;
					if (NewWidth != -1)Main.Button[cur].Width = NewWidth;
					if (NewHeight != -1)Main.Button[cur].Height = NewHeight;
					str1 = wcsstr(str2 + 1, L"\"");
					if (str1 == 0)continue;
					str2 = wcsstr(str1 + 1, L"\"");
					*str2 = '\0';
					wcscpy_s(Main.Button[cur].Exp, str1 + 1);
					continue;
				}
				if (Checkf == true && Mainf == true)
				{
					int cur = _wtoi(ReadTmp + 1);
					wchar_t *str1 = wcsstr(pos + 1, L"\"");
					wchar_t *str2 = wcsstr(str1 + 1, L"\"");
					*str2 = '\0';
					wcscpy_s(Main.Check[cur].Name, str1 + 1);
					str1 = wcsstr(str2 + 1, L",");
					str2 = wcsstr(str1 + 1, L",");
					*str2 = '\0';
					int NewLeft = _wtoi(str1 + 1);
					str1 = wcsstr(str2 + 1, L",");
					*str1 = '\0';
					int NewTop = _wtoi(str2 + 1);
					int NewWidth = _wtoi(str1 + 1);
					if (NewLeft != -1)Main.Check[cur].Left = NewLeft;
					if (NewTop != -1)Main.Check[cur].Top = NewTop;
					if (NewWidth != -1)Main.Check[cur].Width = NewWidth;
					continue;
				}
				if (Stringf == true && Mainf == true)
				{
					wchar_t tmp[301]; bool f = false;
					ZeroMemory(tmp, sizeof(tmp));
					wchar_t *str1 = wcsstr(pos + 1, L"\""), *str2, *str3;
					str2 = str1;
					str3 = str2 + 1;
					while (1)
					{
						str2 = wcsstr(str2, L"\\n");
						if (str2 == NULL)break;
						f = true;
						*str2 = '\0';
						wcscat_s(tmp, str3);
						wcscat_s(tmp, L"\n");
						str3 = str2 + 2;
						str2 = str2 + 1;
					}
					if (f == false)
						str2 = wcsstr(str1 + 1, L"\"");
					else
						str2 = wcsstr(str3, L"\"");

					str2[0] = '\0';
					wcscat_s(tmp, str3);
					Main.SetString(tmp, ReadTmp);
				}
				if (Framef == true && Mainf == true)
				{
					int cur = _wtoi(ReadTmp + 1);
					wchar_t *str1 = wcsstr(pos + 1, L"\"");
					wchar_t *str2 = wcsstr(str1 + 1, L"\"");
					*str2 = '\0';
					wcscpy_s(Main.Frame[cur].Name, str1 + 1);
					str1 = wcsstr(str2 + 1, L",");
					str2 = wcsstr(str1 + 1, L",");
					*str2 = '\0';
					int NewLeft = _wtoi(str1 + 1);
					str1 = wcsstr(str2 + 1, L",");
					*str1 = '\0';
					int NewTop = _wtoi(str2 + 1);
					str2 = wcsstr(str1 + 1, L",");
					*str2 = '\0';
					int NewWidth = _wtoi(str1 + 1);
					int NewHeight = _wtoi(str2 + 1);
					if (NewLeft != -1)Main.Frame[cur].Left = NewLeft;
					if (NewTop != -1)Main.Frame[cur].Top = NewTop;
					if (NewWidth != -1)Main.Frame[cur].Width = NewWidth;
					if (NewHeight != -1)Main.Frame[cur].Height = NewWidth;
					continue;
				}
				if (Textf == true && Mainf == true)
				{
					int cur = _wtoi(ReadTmp + 1);
					wchar_t *str = wcsstr(pos + 1, L",");
					*str = '\0';
					int NewLeft = _wtoi(pos + 1);
					int NewTop = _wtoi(str + 1);
					if (NewLeft != -1)Main.Text[cur].Left = NewLeft;
					if (NewTop != -1)Main.Text[cur].Top = NewTop;
					continue;
				}
				if (Catchf == true)
				{
					wchar_t *str1 = wcsstr(pos + 1, L"\"");
					wchar_t *str2 = wcsstr(str1 + 1, L"\"");
					*str1 = '\0'; *str2 = '\0';
					if (wcsstr(ReadTmp, L"CW") != 0)wcscpy_s(CatchWnd.Button[1].Name, str1 + 1);
					if (wcsstr(ReadTmp, L"MW") != 0)wcscpy_s(CatchWnd.Button[2].Name, str1 + 1);
					if (wcsstr(ReadTmp, L"RW") != 0)wcscpy_s(CatchWnd.Button[3].Name, str1 + 1);
					if (wcsstr(ReadTmp, L"Eat1") != 0)CatchWnd.SetString(str1 + 1, L"Eat1");
					if (wcsstr(ReadTmp, L"Eat2") != 0)CatchWnd.SetString(str1 + 1, L"Eat2");
					if (wcsstr(ReadTmp, L"Timer1") != 0)CatchWnd.SetString(str1 + 1, L"Timer1");
					if (wcsstr(ReadTmp, L"Timer2") != 0)CatchWnd.SetString(str1 + 1, L"Timer2");
					continue;
				}
			}
		}
		fclose(fp);
		UpdateInfo();
		haveInfo = true;
		SetFrame();
		InvalidateRect(CatchWnd.hWnd, 0, 0);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		s(L"error");
	}
}
int SetupSethc()
{
	wchar_t tmp[301];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, L"sethc.exe");
	if (GetFileAttributes(tmp) == -1)return 2;
	if (Bit != 34)
		return CopyFile(tmp, L"C:\\Windows\\System32\\sethc.exe", false);
	else
		return CopyFile(tmp, L"C:\\Windows\\SysNative\\sethc.exe", false);
}

void AutoSetupSethc()
{
	int flag = SetupSethc();
	if (flag == 0)Main.InfoBox(Main.GetStr(L"CSFail"));
	if (flag == 2)Main.InfoBox(Main.GetStr(L"NoSethc"));
	return;
}
bool DeleteSethc()
{
	if (GetFileAttributes(SethcPath) == -1)return true;
	TakeOwner(SethcPath);
	return DeleteFile(SethcPath);
}

bool DeleteDirectory(wchar_t *DirName) //新加删除某个不为空的文件夹 
{
	wchar_t szCurPath[2550];       //用于定义搜索格式  
	wcscpy_s(szCurPath, DirName);
	wcscat_s(szCurPath, L"\\*.*");
	WIN32_FIND_DATA FindFileData;
	ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATA));
	HANDLE hFile = FindFirstFile(szCurPath, &FindFileData);
	BOOL IsFinded = TRUE;
	while (IsFinded)
	{
		IsFinded = FindNextFile(hFile, &FindFileData); //递归搜索其他的文件  
		if (wcslen(FindFileData.cFileName) != 0 && wcscmp(FindFileData.cFileName, L".") && wcscmp(FindFileData.cFileName, L"..")) //如果不是"." ".."目录  
		{
			wchar_t strFileName[2550] = { 0 }, temp[2550] = { 0 };
			wcscpy_s(strFileName, DirName);
			wcscat_s(strFileName, L"\\");
			wcscat_s(strFileName, FindFileData.cFileName);
			//s(strFileName);
			wcscpy_s(temp, strFileName);
			if (GetFileAttributes(temp) & 16) //如果是目录，则递归地调用  
			{
				DeleteDirectory(temp);
			}
			else
			{
				DeleteFile(strFileName);
			}
		}
	}
	FindClose(hFile);

	BOOL bRet = RemoveDirectory(DirName);
	if (bRet == 0) //删除目录  
	{
		return FALSE;
	}
	return TRUE;
}

void AutoDeleteSethc()
{
	if (!DeleteSethc())Main.InfoBox(Main.GetStr(L"DSR3Fail"));
	return;
}
bool DeleteSethcS()
{
	bool flag1 = false, flag2 = false, dl;
	dl = DeleteSethc();//)return true;
	if (!dl)UnloadNTDriver(L"deletefile");
	if (dl)flag1 = true;
	if (Bit == 32)
	{
		if (!dl)flag1 = LoadNTDriver(L"deletefile", L"x32\\deletefile.sys");
		UnloadNTDriver(L"kill");
		flag2 = LoadNTDriver(L"kill", L"x32\\kill.sys");
	}
	else
	{
		if (!dl)flag1 = LoadNTDriver(L"deletefile", L"x64\\deletefile.sys");
		UnloadNTDriver(L"kill");
		flag2 = LoadNTDriver(L"kill", L"x64\\kill.sys");
	}
	WinExec("net stop kill", SW_HIDE);
	return flag1 & flag2;
}
int SetupSethcS()
{
	wchar_t tmp[301];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, L"x32\\sethc.exe");
	if (GetFileAttributes(tmp) == -1)return 2;
	if (Bit != 34)
		return CopyFile(tmp, L"C:\\Windows\\System32\\sethc.exe", false);
	else
		return CopyFile(tmp, L"C:\\Windows\\SysNative\\sethc.exe", false);
}
int CopyNTSD()
{
	int f;
	if (Bit != 34)
		f = GetFileAttributes(L"C:\\Windows\\System32\\ntsd.exe");
	else
		f = GetFileAttributes(L"C:\\Windows\\SysNative\\ntsd.exe");
	if (f != -1)return true;
	wchar_t tmp[301];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, L"ntsd.exe");
	if (GetFileAttributes(tmp) == -1)return 2;

	if (Bit != 34)
		return CopyFile(tmp, L"C:\\Windows\\System32\\ntsd.exe", false);
	else
		return CopyFile(tmp, _T("C:\\Windows\\SysNative\\ntsd.exe"), false);
}
void AutoCopyNTSD()
{
	int flag = CopyNTSD();
	if (flag == 0)Main.InfoBox(Main.GetStr(L"CNTSDFail"));
	if (flag == 2)Main.InfoBox(Main.GetStr(L"NoNTSD"));
	return;
}

void AutoDelete(wchar_t *tmp)
{
	int FileType;
	FileType = GetFileAttributes(tmp);
	if (FileType == -1) { Main.InfoBox(Main.GetStr(L"TINotF")); return; }
	TakeOwner(tmp);

	if (FileType & FILE_ATTRIBUTE_DIRECTORY)
	{
		DeleteDirectory(tmp);
	}
	else
		DeleteFile(tmp);
}
void RegMouseKey()
{
	RegisterHotKey(Main.hWnd, 8, MOD_CONTROL, 188);
	RegisterHotKey(Main.hWnd, 9, MOD_CONTROL, 190);
	RegisterHotKey(Main.hWnd, 10, MOD_CONTROL, VK_LEFT);
	RegisterHotKey(Main.hWnd, 11, MOD_CONTROL, VK_UP);
	RegisterHotKey(Main.hWnd, 12, MOD_CONTROL, VK_RIGHT);
	RegisterHotKey(Main.hWnd, 13, MOD_CONTROL, VK_DOWN);
}
void UnMouseKey()
{
	for (int i = 8; i < 14; ++i)UnregisterHotKey(Main.hWnd, i);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(MouseHook, nCode, wParam, lParam);
}
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}
DWORD WINAPI GameThread(LPVOID pM)
{
	UNREFERENCED_PARAMETER(pM);
	if (Main.Width < 700)
	{
		lock = true;
		if (!Effect)
		{
			SetWindowPos(Main.hWnd, NULL, 0, 0, (Main.Width + 600)*Main.DPI, Main.Height *Main.DPI, SWP_NOMOVE | SWP_NOREDRAW);
			Main.Button[Main.GetNumbyID(L"Close")].Left += 600;
			InvalidateRect(Main.hWnd, NULL, FALSE);
			UpdateWindow(Main.hWnd);
			goto next;
		}
		for (int j = 1; j <= 600; j += 20)
		{
			::SetWindowPos(Main.hWnd, NULL, 0, 0, (Main.Width + j)*Main.DPI, Main.Height *Main.DPI, SWP_NOMOVE | SWP_NOREDRAW);
			RECT Rc;
			Rc.left = Main.Width*Main.DPI;
			Rc.top = 0;
			Rc.right = (Main.Width + j)*Main.DPI;
			Rc.bottom = Main.Height*Main.DPI;
			InvalidateRect(Main.hWnd, &Rc, FALSE);
			Rc = Main.GetRECT(Main.GetNumbyID(L"Close"));
			Rc.left -= 20 * Main.DPI;
			InvalidateRect(Main.hWnd, &Rc, FALSE);
			UpdateWindow(Main.hWnd);
			Main.Button[Main.GetNumbyID(L"Close")].Left += 20;
		}
		Main.Button[Main.GetNumbyID(L"Close")].Left -= 20;
	next:
		Main.Width += 600;
	}
	else
	{
		if (Effect)
			for (int j = 1; j <= 600; j += 20)
			{
				::SetWindowPos(Main.hWnd, NULL, 0, 0, (Main.Width - j)*Main.DPI, Main.Height*Main.DPI, SWP_NOMOVE | SWP_NOREDRAW);//经测试不能使用RedrawObject
				InvalidateRect(Main.hWnd, 0, FALSE);
				UpdateWindow(Main.hWnd);
				Main.Button[Main.GetNumbyID(L"Close")].Left -= 20;
			}
		Main.Width -= 600;
		if (Effect)Main.Button[Main.GetNumbyID(L"Close")].Left += 20; else Main.Button[Main.GetNumbyID(L"Close")].Left -= 600;
		::SetWindowPos(Main.hWnd, NULL, 0, 0, Main.Width *Main.DPI, Main.Height *Main.DPI, SWP_NOMOVE);
	}
	lock = false;
	return 0;
}
DWORD WINAPI ThreadFun(LPVOID pM)
{
	UNREFERENCED_PARAMETER(pM);
	while (TOP == 1)
	{
		SetWindowPos(Main.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
		if (CatchWnd.hWnd != NULL)SetWindowPos(CatchWnd.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
	}
	if (CatchWnd.hWnd != NULL)SetWindowPos(CatchWnd.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	return 0;
}
DWORD WINAPI DownloadThread(LPVOID pM)
{
	int *tp = (int *)pM;
	int cur = *tp;
	//s(cur);
	DownloadProgress progress;
	TCHAR tmp[501];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, GameName[cur - 1]);
	switch (cur)
	{
	case 1://小飞
		wcscpy_s(progress.curi, L"Game1");
		URLDownloadToFileW(NULL, L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Games/xiaofei.exe", tmp, 0, &progress);
		wcscpy_s(tmp, Path);
		wcscat_s(tmp, L"Games\\14000词库.ini");
		URLDownloadToFileW(NULL, L"https://github.com/zouxiaofei1/TopDomianTools/raw/master/Games/14000%E8%AF%8D%E5%BA%93.ini", tmp, 0, NULL);
		break;
	case 2://Flappy
		wcscpy_s(progress.curi, L"Game2");
		URLDownloadToFileW(NULL, L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Games/fly.exe", tmp, 0, &progress);
		break;
	case 3://2048
		wcscpy_s(progress.curi, L"Game3");
		URLDownloadToFileW(NULL, L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Games/2048.exe", tmp, 0, &progress);
		break;
	case 4://俄罗斯方块
		wcscpy_s(progress.curi, L"Game4");
		URLDownloadToFileW(NULL, L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Games/block.exe", tmp, 0, &progress);
		break;
	case 5://见缝插针
		wcscpy_s(progress.curi, L"Game5");
		URLDownloadToFileW(NULL, L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Games/1.exe", tmp, 0, &progress);
		break;
	case 6://Chess
		wcscpy_s(progress.curi, L"Game6");
		URLDownloadToFileW(NULL, L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Games/chess.exe", tmp, 0, &progress);
		break;
	}
	if (GetFileAttributes(tmp) != -1)GameExist[cur - 1] = true;
	return 0;
}
COLORREF DoSelectColour()
{
	static CHOOSECOLOR cc;
	static COLORREF crCustColors[16];
	cc.lStructSize = sizeof(cc);
	cc.lpCustColors = crCustColors;
	cc.Flags = CC_ANYCOLOR;
	cc.hInstance = 0;
	cc.lpTemplateName = 0;
	cc.lpfnHook = 0;
	cc.rgbResult = RGB(1, 1, 1);

	ChooseColor(&cc);
	int sum = (byte)cc.rgbResult + (byte)(cc.rgbResult >> 8) + (byte)(cc.rgbResult >> 16);
	if (sum <= 384)Main.Text[18].rgb = RGB(255, 255, 255); else Main.Text[18].rgb = 0;
	return cc.rgbResult;
}
BOOL CALLBACK CathyThread(HWND hwnd, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID;
	if (GetParent(hwnd) == 0)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		if (Eatpid[nProcessID] == true)
			if (SetParent(hwnd, CatchWnd.hWnd) != NULL)
			{
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOZORDER);
				Eating->value = hwnd;
				Eating->next = new _Eatlist;
				Eating = Eating->next;
				CurEat++;
			}
	}
	return 1;
}
VOID ReturnWindows()
{
	Eating = &Eatlist;
	while (Eating != NULL)
	{
		if (Eating->value == NULL)break;
		SetParent(Eating->value, NULL);
		_Eatlist *back = Eating;
		Eating = Eating->next;
		if (Eating->next == NULL)break;
		if (back != &Eatlist&&back != NULL)delete back;
	}
	CurEat = 0;
	InvalidateRect(CatchWnd.hWnd, NULL, FALSE);
}
int Cathy()
{
	wchar_t tmp[1001];
	wcscpy_s(tmp, CatchWnd.Edit[CatchWnd.GetNumByIDe(L"E_Pname")].str);
	tmp[3] = '\0';
	_wcslwr_s(tmp);
	Eatpid.clear();
	PROCESSENTRY32 pe;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return 1;
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = '\0';
		_wcslwr_s(pe.szExeFile);
		if (wcsstr(tmp, pe.szExeFile) != 0)Eatpid[pe.th32ProcessID] = true;
	}
	Eating = &Eatlist;
	EnumWindows(CathyThread, NULL);
	return 0;
}
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(nMsg);
	UNREFERENCED_PARAMETER(dwTime);
	switch (nTimerid)
	{
	case 1:
		if (Main.Check[8].Value == 1)
			KillProcess(Main.Edit[Main.GetNumByIDe(L"E_TDname")].str);
		if (Main.Check[9].Value == 1)KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
		if (Main.Check[10].Value == 1)MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInst, 0);
		break;
	case 2:
		timerleft--;
		if (timerleft >= 0)
		{
			wchar_t tmp[301], tmp2[11];
			wcscpy_s(tmp, CatchWnd.GetStr(L"Timer1"));
			_itow_s(timerleft, tmp2, 10);
			wcscat_s(tmp, tmp2);
			wcscat_s(tmp, CatchWnd.GetStr(L"Timer2"));
			wcscpy_s(CatchWnd.Button[1].Name, tmp);
			InvalidateRect(CatchWnd.hWnd, NULL, FALSE);
			if (timerleft == 0)Cathy();
		}
		if (timerleft == -1)wcscpy_s(CatchWnd.Button[1].Name, CatchWnd.GetStr(L"back")), InvalidateRect(CatchWnd.hWnd, NULL, FALSE);
		break;
	case 3:
		EasterEggState = (EasterEggState + 1) % 11;
		wchar_t tmp[101];
		wcscpy_s(tmp, L"Copy");
		wcscat_s(tmp, EasterEggStr[EasterEggState % 11]);
		wcscat_s(tmp, Main.GetStr(L"Tleft"));
		Main.SetString(tmp, L"_Tleft");
		InvalidateRect(Main.hWnd, 0, false);
		break;
	case 4:
		if (GetTickCount() - Main.Timer >= 1000 && Main.ExpExist == false)Main.Try2CreateExp();
		//out(GetTickCount() - Main.Timer);
		break;
	case 5:
		for (int i = 1; i <= Main.CurButton; ++i)
		{
			if (Main.CurCover != i && Main.Button[i].Percent > 0)
			{
				Main.Button[i].Percent -= Delta;
				if (Main.Button[i].Percent < 0)Main.Button[i].Percent = 0;
				Main.RedrawType = 2;
				Main.RedrawCur = i;
				RECT rc = Main.GetRECT(i);
				InvalidateRect(Main.hWnd, &rc, FALSE);
				UpdateWindow(Main.hWnd);
			}
		}
		if (Main.CurCover != -1 && Main.Button[Main.CurCover].Percent < 100)
		{
			Main.Button[Main.CurCover].Percent += 2 * Delta;
			if (Main.Button[Main.CurCover].Percent > 100)Main.Button[Main.CurCover].Percent = 100;
			Main.RedrawType = 2;
			Main.RedrawCur = Main.CurCover;
			RECT rc = Main.GetRECT(Main.CurCover);
			InvalidateRect(Main.hWnd, &rc, FALSE);
			UpdateWindow(Main.hWnd);
		}
		break;
	case 6:
		InvalidateRect(CatchWnd.hWnd, NULL, FALSE);
		UpdateWindow(CatchWnd.hWnd);
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
void openfile()
{
	OPENFILENAMEW ofn = { 0 };
	wchar_t strFile[MAX_PATH] = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = (LPWSTR)strFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileNameW(&ofn) == TRUE)Main.SetEditStrOrFont(strFile, 0, Main.GetNumByIDe(L"E_View"));
	Main.RedrawType = 5;
	Main.RedrawCur = Main.GetNumByIDe(L"E_View");
	RECT rc = Main.GetRECTe(Main.GetNumByIDe(L"E_View"));
	InvalidateRect(Main.hWnd, &rc, FALSE);
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
		Main.SetEditStrOrFont(path, 0, Main.GetNumByIDe(L"E_View"));
		Main.RedrawType = 5;
		Main.RedrawCur = Main.GetNumByIDe(L"E_View");
		RECT rc = Main.GetRECTe(Main.GetNumByIDe(L"E_View"));
		InvalidateRect(Main.hWnd, &rc, FALSE);
	}
}
VOID SearchTool(LPCWSTR lpPath, int type)//1 打开极域 2 删除shutdown
{
	//s(lpPath);
	wchar_t szFind[255], szFile[255];
	WIN32_FIND_DATA FindFileData;
	wcscpy_s(szFind, lpPath);
	wcscat_s(szFind, L"\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) {
		/*s(L"failed");*/ return;
	}
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				wcscpy_s(szFile, lpPath);
				wcscat_s(szFile, L"\\");
				wcscat_s(szFile, FindFileData.cFileName);
				SearchTool(szFile, type);
			}
		}
		else
		{
			//s(FindFileData.cFileName);
			_wcslwr_s(FindFileData.cFileName);
			if (type == 1)
			{
				if (wcsstr(FindFileData.cFileName, L"studentmain.exe") != NULL)
				{
					wcscpy_s(szFile, lpPath);
					wcscat_s(szFile, L"\\");
					wcscat_s(szFile, FindFileData.cFileName);
					RunEXE(szFile);
				}
			}
			else
			{
				if (wcsstr(FindFileData.cFileName, L"shutdown.exe") != NULL)
				{
					wcscpy_s(szFile, lpPath);
					wcscat_s(szFile, L"\\");
					wcscat_s(szFile, FindFileData.cFileName);
					//s(szFile);
					TakeOwner(szFile);
					DeleteFile(szFile);
				}
			}
		}
		if (!FindNextFile(hFind, &FindFileData))    break;
	}
	FindClose(hFind);
}
void ReopenTD()
{
	if (Bit == 32)
	{
		SearchTool(L"C:\\Program Files\\Mythware", 1),
			SearchTool(L"C:\\Program Files\\TopDomain", 1);
	}
	else
	{
		SearchTool(L"C:\\Program Files (x86)\\Mythware", 1),
			SearchTool(L"C:\\Program Files (x86)\\TopDomain", 1);
	}
}
void DeleteShutdown()
{
	if (Bit == 32)
	{
		SearchTool(L"C:\\Program Files\\Mythware", 2);
		SearchTool(L"C:\\Program Files\\TopDomain", 2);
		SearchTool(L"C:\\Windows\\System32", 2);
	}
	else
	{
		SearchTool(L"C:\\Program Files (x86)\\Mythware", 2);
		SearchTool(L"C:\\Program Files (x86)\\TopDomain", 2);
		SearchTool(L"C:\\Windows\\System32", 2);
		SearchTool(L"C:\\Windows\\SysNative", 2);
	}
}
bool RunHOOK()
{
	wchar_t tmp[501];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, L"hook.exe");

	if (GetFileAttributes(tmp) == -1)return false;
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	return CreateProcessW(tmp, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
}
VOID BSOD()
{
	UnloadNTDriver(L"BSOD");

	if (Bit == 32)
		LoadNTDriver(L"BSOD", L"x32\\BSOD.sys");
	else
		LoadNTDriver(L"BSOD", L"x64\\BSOD.sys");

	KillProcess(L"svc");
}
void EasterEgg(bool flag)
{
	if (flag)
		SetTimer(Main.hWnd, 3, 100, (TIMERPROC)TimerProc);
	else
		KillTimer(Main.hWnd, 3);
}

void AutoViewPass()
{
	HKEY hKey;
	LONG ret;
	wchar_t szLocation[300];
	DWORD dwSize = sizeof(wchar_t) * 300;
	DWORD dwType = REG_SZ;
	ZeroMemory(&szLocation, sizeof(szLocation));
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);

	if (ret != 0 && slient == false)
		if (MessageBox(Main.hWnd, Main.GetStr(L"VPFail"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK) { RegCloseKey(hKey); return; }
	ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType, (LPBYTE)&szLocation, &dwSize);
	if (ret != 0 && slient == false)
		if (MessageBox(Main.hWnd, Main.GetStr(L"VPUKE"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK) { RegCloseKey(hKey); return; }
	if (szLocation[0] != 0)
	{
		WCHAR Tmp[300];

		wcscpy_s(Tmp, Main.GetStr(L"pswdis"));
		wcscat_s(Tmp, szLocation);
		Main.InfoBox(Tmp);
	}
	else
		Main.InfoBox(Main.GetStr(L"VPNULL"));
	RegCloseKey(hKey);
}
void AutoClearPassWd()
{
	const BYTE key[] = { 0x37,0x6A,0x12,0x11,0xDB,0x6E,0xE4,0x1A,0x20,0x18,0xE6,0x43,0xDE,0x38,0x17,0x06,0x92,\
		0x22,0xA4,0x33,0x82,0x36,0xB9,0x2B,0xCA,0x05,0x49,0x01,0x34,0x62,0x5F,0x12,0xA1,0x71,0x0F,0x15,0xAB,0x07,0x22,\
		0x5C,0x2E,0x39,0x60,0x0F,0x73,0x07,0xB3,0x4D,0x2C,0x14,0xBB,0x67,0x35,0x2C,0x39,0x67,0x03,0x1A,0x8B,0x3D,\
		0xAE,0x3B,0x71,0x59,0x27,0x6F,0x81,0x36,0xAA,0x60,0x43,0x46,0x69,0x60,0xD5,0x3D,0xB6,0x64,0x85,0x59,0x55,\
		0x03,0x40,0x5E,0x55,0x6C,0xF1,0x79,0x55,0x27,0x77,0x50,0x93,0x38,0x25,0x35,0x12,0x19,0x87,0x35,0x7B,0x05,\
		0x7F,0x12,0x6F,0x14,0x94,0x59,0x3A,0x09,0x75,0x37 };

	wchar_t tmp = 0;
	HKEY hKey;
	LONG ret, ret2;

	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class\\Student", 0, KEY_SET_VALUE, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student", 0, KEY_SET_VALUE, &hKey);

	if (ret != 0)
	{
		Main.InfoBox(Main.GetStr(L"ACFail"));
		RegCloseKey(hKey);
		return;
	}

	ret = RegSetValueEx(hKey, L"Knock", 0, REG_BINARY, (const BYTE*)&key, sizeof(key));
	ret2 = RegSetValueEx(hKey, L"Knock1", 0, REG_BINARY, (const BYTE*)&key, sizeof(key));

	if (ret != 0 || ret2 != 0)
	{
		Main.InfoBox(Main.GetStr(L"ACUKE"));
		RegCloseKey(hKey);
		return;
	}

	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);

	if (ret != 0)
	{
		Main.InfoBox(Main.GetStr(L"ACFail"));
		RegCloseKey(hKey);
		return;
	}

	ret = RegSetValueEx(hKey, L"UninstallPasswd", 0, REG_SZ, (const BYTE*)&tmp, sizeof(tmp));

	if (ret != 0)
	{
		Main.InfoBox(Main.GetStr(L"ACUKE"));
		RegCloseKey(hKey);
		return;
	}

	Main.InfoBox(Main.GetStr(L"ACOK"));
	RegCloseKey(hKey);
}
void ChangePasswordEx(int type)
{
	if (type == 1 || type == 2)
	{
		wchar_t tmp[1001] = { 0 };
		if (wcscmp(Main.Edit[Main.GetNumByIDe(L"E_CP")].str, Main.GetStr(L"E_CP")) == 0)
		{
			wchar_t tmp2[501];
			wcscpy_s(tmp2, Main.GetStr(L"CPAsk1"));
			wcscat_s(tmp2, L"\"");
			wcscat_s(tmp2, Main.Edit[Main.GetNumByIDe(L"E_CP")].str);
			wcscat_s(tmp2, L"\"");
			wcscat_s(tmp2, Main.GetStr(L"CPAsk2"));
			if (MessageBox(Main.hWnd, tmp2, Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_YESNO) != IDYES)return;
		}
		HKEY hKey;
		LONG ret;
		if (Bit != 64)
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);
		else
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);

		if (ret != 0)
		{
			Main.InfoBox(Main.GetStr(L"ACFail"));
			RegCloseKey(hKey);
			return;
		}
		if (type == 2)wcscpy_s(tmp, L"Passwd"), wcscat_s(tmp, Main.Edit[Main.GetNumByIDe(L"E_CP")].str);
		else wcscpy_s(tmp, Main.Edit[Main.GetNumByIDe(L"E_CP")].str);
		ret = RegSetValueEx(hKey, L"UninstallPasswd", 0, REG_SZ, (const BYTE*)&tmp, sizeof(wchar_t)*wcslen(tmp));
		if (ret != 0)
		{
			Main.InfoBox(Main.GetStr(L"ACUKE"));
			RegCloseKey(hKey);
			return;
		}
		RegCloseKey(hKey);
		Main.InfoBox(Main.GetStr(L"ACOK"));
		return;
	}
	if (type == 3)sub_430CD0(NULL, NULL, NULL);
	return;
}

bool RunCmdLine(LPWSTR str)
{
	if (wcsstr(str, L"-top") != NULL)
	{
		Main.Check[4].Value = 1;
		TOP = TRUE;
		CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);
		return false;
	}
	DWORD pid = GetParentProcessID(GetCurrentProcessId());
	AttachConsole(pid);
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);
	setlocale(LC_ALL, "chs");
	slient = true;
	printf("\n");

	if (wcsstr(str, L"-sethc") != NULL)
	{
		AutoDeleteSethc();
		AutoSetupSethc();
		AutoCopyNTSD();
		ExitProcess(0);
	}
	if (wcsstr(str, L"-hook") != NULL)
	{
		KillProcess(L"hoo");
		AutoCopyNTSD();
		wchar_t tmp[1001];
		wcscpy_s(tmp, Path);
		wcscat_s(tmp, L"hook.exe");
		if (!RunEXE(tmp))Main.InfoBox(Main.GetStr(L"OneFail"));
		ExitProcess(0);
	}
	if (wcsstr(str, L"-auto") != NULL) { CurrentProcessId = GetCurrentProcessId(); KillTop(); ExitProcess(0); }
	if (wcsstr(str, L"-unsethc") != NULL) {
		DeleteFile(SethcPath);
		CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE); ExitProcess(0);
	}
	if (wcsstr(str, L"-unhook") != NULL) { KillProcess(L"hoo"); ExitProcess(0); }
	if (wcsstr(str, L"-viewpass") != NULL) { AutoViewPass(); ExitProcess(0); }
	if (wcsstr(str, L"-antishutdown") != NULL) { DeleteShutdown(); ExitProcess(0); }
	if (wcsstr(str, L"-reopen") != NULL) { ReopenTD(); ExitProcess(0); }
	if (wcsstr(str, L"-BSOD") != NULL) { BSOD(); ExitProcess(0); }
	if (wcsstr(str, L"-restart") != NULL) { Restart(); ExitProcess(0); }
	if (wcsstr(str, L"-clear") != NULL) { AutoClearPassWd(); ExitProcess(0); }
	if (wcsstr(str, L"-rekill") != NULL)
	{
		wchar_t tmp[1001], *tmp1 = wcsstr(str, L"-rekill");
		if (!Findquotations(tmp1, tmp))ExitProcess(0);
		while (1) { KillProcess(tmp); Sleep(50); }
	}
	if (wcsstr(str, L"-delete") != NULL)
	{
		wchar_t tmp[1001], *tmp1 = wcsstr(str, L"-delete");
		if (!Findquotations(tmp1, tmp))ExitProcess(0);
		AutoDelete(tmp);
		ExitProcess(0);
	}
	return false;
}
void CreateString() { CreateStrs }

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,//入口点
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	wchar_t szProcess[] = L"C:\\windows\\explorer";

	GetPath();
	Admin = IsUserAnAdmin();

	GetBit();
	if (Bit != 34)
		wcscpy(SethcPath, L"C:\\Windows\\System32\\sethc.exe");
	else
		wcscpy(SethcPath, L"C:\\Windows\\SysNative\\sethc.exe");

	CreateString();

	BOOL Flag = Backup();
	if (wcslen(lpCmdLine) != 0) { if (RunCmdLine(lpCmdLine) == true)goto cosl; }

	defaultDesk = GetThreadDesktop(GetCurrentThreadId());//创建虚拟桌面
	hVirtualDesk = CreateDesktop(szVDesk, NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);
	if (Flag == TRUE)ShellCreateInVDesk(szProcess);
	hCurrentDesk = defaultDesk;

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))return FALSE;

cosl:
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUI));

	MSG msg;
	// 主消息循环: 
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
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
	WNDCLASSEXW wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUI));//大图标
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUI));//小图标

	return RegisterClassExW(&wcex);
}
void SearchLanguageFiles()
{
	wchar_t lpPath[301] = { 0 }, szFind[301] = { 0 };
	WIN32_FIND_DATA FindFileData;
	GetModuleFileName(NULL, lpPath, MAX_PATH);
	(_tcsrchr(lpPath, _T('\\')))[1] = 0;
	wcscpy_s(szFind, lpPath);
	wcscat_s(szFind, L"language\\*.ini");
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
	DBlueBrush = CreateSolidBrush(RGB(210, 255, 255));//笔刷
	LBlueBrush = CreateSolidBrush(RGB(230, 255, 255));
	WhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	BlueBrush = CreateSolidBrush(RGB(40, 130, 240));
	green = CreateSolidBrush(RGB(0, 206, 209));
	grey = CreateSolidBrush(RGB(245, 245, 245));
	yellow = CreateSolidBrush(RGB(0xEE, 0xE6, 0x85));
	YELLOW = CreatePen(PS_SOLID, 1, RGB(0xC1, 0xCD, 0xCD));//笔
	RED = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	BLACK = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	White = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	GREEN = CreatePen(PS_SOLID, 2, RGB(5, 195, 195));
	LGREY = CreatePen(PS_SOLID, 1, RGB(120, 120, 120));
	BLUE = CreatePen(PS_SOLID, 1, RGB(40, 130, 240));

	hInst = hInstance; // 将实例句柄存储在全局变量中
	Main.InitClass(hInst);
	Main.CreateMain(CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, L"极域破解v1.8.4", 0, true);

	if (Effect)
	{
		Main.ButtonEffect = true;//按钮渐变色特效
		SetTimer(Main.hWnd, 5, 33, (TIMERPROC)TimerProc);//启用渐变色计时器/30fps
	}

	SetWindowLong(Main.hWnd, GWL_STYLE, GetWindowLong(Main.hWnd, GWL_STYLE)& ~WS_CAPTION & ~WS_THICKFRAME&~WS_SYSMENU&~WS_GROUP&~WS_TABSTOP);
	//无边框窗口?

	if (Effect)
	{
		SetWindowLong(Main.hWnd, GWL_EXSTYLE, GetWindowLong(Main.hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(Main.hWnd, NULL, 234, LWA_ALPHA);//半透明特效
	}

	FileList = CreateWindowW(L"ListBox", NULL, WS_CHILD | LBS_STANDARD, 180, 400, 265, 120, Main.hWnd, (HMENU)1, hInstance, 0);
	//创建语言文件选择ListBox

	SearchLanguageFiles();

	::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);

	RegisterHotKey(Main.hWnd, 1, MOD_CONTROL, 'P');
	RegisterHotKey(Main.hWnd, 2, MOD_CONTROL, 'B');
	RegisterHotKey(Main.hWnd, 7, MOD_CONTROL | MOD_ALT, 'K');
	Main.EditRegHotKey();

	hZXFBitmap = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ZXF), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);//资源文件中加载zxf头像

	Main.CreateEditEx(325 + 5, 420, 110 - 10, 50, 1, L"explorer.exe", L"E_runinVD", 0);
	Main.CreateEditEx(185 + 5, 102, 110 - 10, 40, 2, L"输入端口", L"E_ApplyCh", 0);
	Main.CreateEditEx(365 + 5, 175, 210 - 10, 50, 2, L"输入密码", L"E_CP", 0);
	Main.CreateEditEx(195 + 5, 102, 310 - 10, 37, 3, L"浏览文件/文件夹", L"E_View", 0);
	Main.CreateEditEx(277 + 5, 206, 138 - 10, 25, 5, L"StudentMain", L"E_TDname", 0);

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
	Main.CreateFrame(170, 275, 410, 95, 1, L" 更强的Hook ");
	Main.CreateFrame(170, 395, 410, 122, 1, L" 虚拟桌面 ");

	Main.CreateButton(195, 300, 110, 50, 1, L"安装", L"hookS");//HOOK
	Main.CreateButton(325, 300, 110, 50, 1, L"卸载", L"hookU");//12

	Main.CreateButton(195, 420, 110, 50, 1, L"运行程序", L"runinVD");//桌面
	Main.CreateButton(450, 420, 110, 50, 1, L"切换桌面", L"SwitchD");

	Main.CreateLine(185, 165, 565, 165, 1);

	Main.CreateText(470, 117, 1, L"Tdelete", RGB(255, 100, 0));
	Main.CreateText(470, 197, 1, L"Tcopy", RGB(255, 100, 0));
	Main.CreateText(195, 485, 1, L"Tctrl+b", RGB(255, 100, 0));

	Main.CreateFrame(160, 75, 160, 146, 2, L" 频道工具 ");
	Main.CreateFrame(345, 75, 250, 272, 2, L" 管理员密码工具 ");


	Main.CreateCheck(165, 255, 2, 130, L" 伪装工具条旧");
	Main.CreateCheck(165, 280, 2, 80, L" 伪装工具条新");
	Main.CreateCheck(165, 305, 2, 130, L" 伪装托盘图标");

	Main.CreateButton(185, 155, 110, 45, 2, L"应用", L"ApplyCh");

	Main.CreateButton(365, 102, 97, 45, 2, L"清空密码", L"ClearPass");//16
	Main.CreateButton(477, 102, 97, 45, 2, L"查看密码", L"ViewPass");
	Main.CreateButton(365, 235, 60, 45, 2, L"改密1", L"CP1");
	Main.CreateButton(440, 235, 60, 45, 2, L"改密2", L"CP2");
	Main.CreateButton(515, 235, 60, 45, 2, L"改密3", L"CP3");

	Main.CreateButton(165, 370, 120, 55, 2, L"重新打开极域", L"re-TD");
	Main.CreateButton(305, 370, 120, 55, 2, L"程序窗口化", L"windows.ex");
	Main.CreateButton(445, 370, 120, 55, 2, L"防止教师关机", L"ANTI-");
	Main.CreateButton(165, 440, 120, 55, 2, L"显示于安全桌面", L"desktop");
	Main.CreateButton(305, 440, 120, 55, 2, L"全自动防控制", L"auto-5");
	Main.CreateButton(445, 440, 120, 55, 2, L"卸载sethc", L"Usethc");//26

	Main.CreateText(365, 295, 2, L"Tcp1", RGB(50, 50, 50));
	Main.CreateText(365, 317, 2, L"Tcp2", RGB(255, 100, 0));
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
	Main.CreateText(325, 80, 4, L"Tcoder", NULL);
	Main.CreateText(325, 105, 4, L"Tver", NULL);
	Main.CreateText(372, 130, 4, L"Tver2", NULL);
	Main.CreateText(170, 260, 4, L"Ttip1", NULL);
	Main.CreateText(170, 285, 4, L"Ttip2", NULL);
	Main.CreateText(170, 320, 4, L"Tbit", NULL);
	Main.CreateText(285, 320, 4, L"Twinver", NULL);
	Main.CreateText(455, 320, 4, L"Tcmd", NULL);
	Main.CreateText(170, 345, 4, L"TTDv", NULL);
	Main.CreateText(375, 345, 4, L"TIP", NULL);
	Main.CreateLine(170, 400, 590, 400, 4);
	Main.CreateText(170, 415, 4, L"_Tleft", NULL);
	Main.CreateText(170, 440, 4, L"Tleft2", NULL);
	Main.CreateButton(490, 415, 100, 50, 4, L"更多", L"more.txt");
	Main.CreateButton(490, 477, 100, 50, 4, L"系统信息", L"sysinfo");//38

	Main.CreateCheck(180, 90, 5, 100, L" 窗口置顶");
	Main.CreateCheck(180, 120, 5, 160, L" Ctrl+R 紧急蓝屏");
	Main.CreateCheck(180, 150, 5, 160, L" Ctrl+T 瞬间重启");
	Main.CreateCheck(180, 180, 5, 200, L" Ctrl+Alt+P 截图/显示");//新
	Main.CreateCheck(180, 210, 5, 94, L" 连续结束                   .exe");
	Main.CreateCheck(180, 240, 5, 160, L" 禁止键盘钩子");
	Main.CreateCheck(180, 270, 5, 160, L" 禁止鼠标钩子");//新
	Main.CreateCheck(180, 300, 5, 240, L" Ctrl+Alt+K 键盘操作鼠标");
	Main.CreateCheck(180, 330, 5, 160, L" 高画质");
	Main.CreateCheck(180, 360, 5, 160, L" 放大/缩小");

	Main.CreateButton(470, 400, 100, 50, 5, L"永久隐藏", L"hidest");
	Main.CreateButton(470, 464, 100, 50, 5, L"最小化", L"minisize");


	Main.CreateFrame(655, 75, 170, 420, 0, L" 游戏 ");

	Main.CreateButton(680, 95, 120, 50, 0, L"小飞猜词", L"Game1");
	Main.CreateButton(680, 160, 120, 50, 0, L"Flappy Bird", L"Game2");
	Main.CreateButton(680, 225, 120, 50, 0, L"2048", L"Game3");
	Main.CreateButton(680, 290, 120, 50, 0, L"俄罗斯方块", L"Game4");
	Main.CreateButton(680, 355, 120, 50, 0, L"见缝插针", L"Game5");//game结构体
	Main.CreateButton(680, 420, 120, 50, 0, L"五子棋", L"Game6");

	Main.CreateFrame(139, 50, 481, 499, 0, L"");
	Main.CreateFrame(139, 50, 1080, 499, 0, L"");

	Main.CurButton++;

	if (Admin == 0)Main.CreateText(60, 17, 0, L"Tmain", RGB(255, 255, 255));
	else Main.CreateText(60, 17, 0, L"Tmain2", RGB(255, 255, 255));
	Main.CreateButtonEx(Main.CurButton, 530, 10, 60, 30, 0, L"×", \
		CreateSolidBrush(RGB(255, 106, 106)), CreateSolidBrush(RGB(250, 102, 102)), CreateSolidBrush(RGB(238, 99, 99)), \
		CreatePen(PS_SOLID, 1, RGB(255, 106, 106)), CreatePen(PS_SOLID, 1, RGB(250, 102, 102)), CreatePen(PS_SOLID, 1, RGB(238, 99, 99)), \
		Main.DefFont, 1, 1, RGB(255, 255, 255), L"Close");

	if (!Main.hWnd)return FALSE;

	if (Admin == FALSE)SetFrame();

	SetWindowPos(Main.hWnd, 0, 0, 0, 621, 550, SWP_NOMOVE);
	Main.Width = 621;
	Main.Height = 550;
	UpdateWindow(Main.hWnd);
	ShowWindow(Main.hWnd, nCmdShow);

	//SwitchToThisWindow(Main.hWnd, true);

	typedef struct tagCHANGEFILTERSTRUCT {
		DWORD cbSize;
		DWORD ExtStatus;
	} CHANGEFILTERSTRUCT, *PCHANGEFILTERSTRUCT;
	typedef BOOL(WINAPI* ZXF)(HWND hwnd, UINT message, DWORD action, PCHANGEFILTERSTRUCT pChangeFilterStruct);
	ZXF ChangeWindowMessageFilterEx = NULL;
	HMODULE hModule = GetModuleHandle(L"user32.dll");
	if (hModule != NULL)ChangeWindowMessageFilterEx = (ZXF)GetProcAddress(hModule, "ChangeWindowMessageFilterEx");
	if (ChangeWindowMessageFilterEx == NULL)return TRUE;
	ChangeWindowMessageFilterEx(Main.hWnd, WM_DROPFILES, 1, 0);
	ChangeWindowMessageFilterEx(Main.hWnd, 0x0049, 1, NULL);

	return TRUE;
}

//响应函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		KillTimer(hWnd, 4);
		PostQuitMessage(0);
		//ExitProcess(0);
		break;
	case	WM_CREATE:
		if (Effect)
		{
			Cshadow.Initialize(hInst);
			Cshadow.Create(hWnd);
		}

		rdc = GetDC(Main.hWnd);
		hdc = CreateCompatibleDC(rdc);
		hBmp = CreateCompatibleBitmap(rdc, 4000, 4000);
		SelectObject(hdc, hBmp);
		ReleaseDC(Main.hWnd, rdc);
		DragAcceptFiles(hWnd, true);
		break;
	case WM_DROPFILES:
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (Main.InsideEdit(4, point) == TRUE)
		{
			wchar_t tmp[501];
			HDROP hDrop = (HDROP)wParam;
			DragQueryFile(hDrop, 0, tmp, MAX_PATH);
			Main.SetEditStrOrFont(tmp, NULL, Main.GetNumByIDe(L"E_View"));
			Main.RedrawType = 5;
			Main.RedrawCur = Main.GetNumByIDe(L"E_View");
			RECT rc = Main.GetRECTe(Main.GetNumByIDe(L"E_View"));
			InvalidateRect(Main.hWnd, &rc, FALSE);
		}
		break;
	}
	case WM_HOTKEY://热键
	{
		switch (wParam)
		{
		case 1:
		{
			ShowWindow(Main.hWnd, 5 * HideState);
			HideState = 1 - HideState;
			break;
		}
		case 2:
		{
			SDesktop();
			break;
		}
		case 3:
		{
			KillProcess(L"stu");
			break;
		}
		case 4:
		{
			BSOD();
			break;
		}
		case 5:
		{
			Restart();
			break;
		}
		case 6:
		{
			ScreenState++;
			if (ScreenState == 1)CaptureImage();
			if (ScreenState == 2)
				CreateWindow(ScreenWindow, L"1", WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInst, nullptr);
		}
		break;
		case 7:
		{
			if (Main.Check[11].Value == false)RegMouseKey(), Main.Check[11].Value = true; else UnMouseKey(), Main.Check[11].Value = false;
			Main.RedrawType = 3;
			Main.RedrawCur = 11;
			RECT rc = Main.GetRECTc(11);
			InvalidateRect(Main.hWnd, &rc, false);
			break;
		}
		case 8:
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			break;
		}
		case 9:
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			break;
#pragma warning(disable:4245)
		case 10:
			mouse_event(MOUSEEVENTF_MOVE, -10, 0, 0, 0);
			break;
		case 11:
			mouse_event(MOUSEEVENTF_MOVE, 0, -10, 0, 0);
			break;
#pragma warning(default:4245)
		case 12:
			mouse_event(MOUSEEVENTF_MOVE, 10, 0, 0, 0);
			break;
		case 13:
			mouse_event(MOUSEEVENTF_MOVE, 0, 10, 0, 0);
			break;
		}

		Main.EditHotKey(wParam);
		break;
	}
	case WM_SETFOCUS: {Main.EditRegHotKey(); break; }
	case WM_KILLFOCUS: {Main.EditUnHotKey(); break; }
	//case WM_NCHITTEST:
	//{
	//	POINT point;
	//	GetCursorPos(&point);
	//	ScreenToClient(Main.hWnd, &point);
	//	RECT rect;
	//	GetClientRect(hWnd, &rect);
	//	if (point.x >= rect.right - 10 && point.y >= rect.bottom - 10)
	//		return HTBOTTOMRIGHT;
					   
	//	else return HTCLIENT;

	//	//s(point.x);
	//}
	//case WM_SIZE:
	//{
	//	double x = 621 / 550.0;
	//	POINT point;
	//	GetCursorPos(&point);
	//	ScreenToClient(Main.hWnd, &point);
	//	if (point.x <= 0 || point.y <= 0)return 0;
	//	RECT rect;
	//	GetClientRect(hWnd, &rect);
	//	int wid = rect.right - rect.left, hei = rect.bottom - rect.top;
	//	//s(wid);
	//	//if (wid == 1 || hei == 1)return 0;
	//	if ((double)(point.x / point.y) < x)
	//	{
	//		SetWindowPos(hWnd, NULL, 0, 0, wid, wid / x, SWP_NOMOVE | SWP_NOREDRAW);
	//		double nd = (double)wid / 621.0;
	//		if(abs(Main.DPI-nd)>0.01)Main.SetDPI(nd);
	//	}
	//	else
	//		if ((double)(point.x / point.y) > x)
	//		{
	//			SetWindowPos(hWnd, NULL, 0, 0, hei*x, hei, SWP_NOMOVE | SWP_NOREDRAW);
	//			double nd = (double)hei / 550.0;
	//			if (abs(Main.DPI - nd) > 0.01)Main.SetDPI(nd);
	//		}
	//		else return 0;
	//	InvalidateRect(hWnd, NULL, FALSE);
	//}
	case WM_ERASEBKGND: {return 0; }
	case WM_PAINT:
	{
		HBRUSH BitmapBrush; HICON hicon;
		RECT rc; bool f = false;
		GetUpdateRect(hWnd, &rc, false);
		if (rc.top != 0)f = true;
		if (Main.hdc == NULL)Main.SetHDC(hdc);
		PAINTSTRUCT ps;
		rdc = BeginPaint(hWnd, &ps);
		if (Main.RedrawType != 0) {
			Main.RedrawObject(); goto finish;
		}
		if (f == true)goto next;
		SetBkMode(rdc, TRANSPARENT);
		SetBkMode(hdc, TRANSPARENT);

		SelectObject(hdc, WhiteBrush);//白色背景
		Rectangle(hdc, 0, 0, 1230 * Main.DPI, Main.Height*Main.DPI);

		SelectObject(hdc, GREEN);//绿色顶部
		SelectObject(hdc, green);
		Rectangle(hdc, 0, 0, 1230 * Main.DPI, 50 * Main.DPI);

		SetTextColor(hdc, RGB(0, 0, 0));
		SelectObject(hdc, BLACK);
		SelectObject(hdc, WhiteBrush);
	next:
		//s(0);
		Main.DrawEVERYTHING();

		if (f == true)  goto finish;

		hicon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_GUI)); \
			DrawIconEx(hdc, 20 * Main.DPI, 10 * Main.DPI, hicon, 32 * Main.DPI, 32 * Main.DPI, 0, NULL, DI_NORMAL | DI_COMPAT);

		BitmapBrush = CreatePatternBrush(hZXFBitmap);
		SelectObject(hdc, BitmapBrush);
		if (Main.CurWnd == 4)Rectangle(hdc, 135 * 22, 170 * 18, 135 * 23, 170 * 19);
		if (Main.CurWnd == 4)StretchBlt(hdc, 170 * Main.DPI, 75 * Main.DPI, 135 * Main.DPI, 170 * Main.DPI, hdc, 135 * 22, 170 * 18, 135, 170, SRCCOPY);
		DeleteObject(hicon);
		DeleteObject(BitmapBrush);
	finish:
		BitBlt(rdc, rc.left, rc.top, max((long)Main.Width*Main.DPI, rc.right - rc.left), max((long)Main.Height*Main.DPI, rc.bottom - rc.top), hdc, rc.left, rc.top, SRCCOPY);
		EndPaint(hWnd, &ps);
	}
	break;

	case 34:
		if (lParam == WM_LBUTTONUP)
		{
			NOTIFYICONDATA tnd;
			tnd.cbSize = sizeof(NOTIFYICONDATA);
			tnd.hWnd = Main.hWnd;
			tnd.uID = IDR_MAINFRAME;
			Shell_NotifyIcon(NIM_DELETE, &tnd);
			ReopenTD();
		}
		break;
	case WM_COMMAND:
		int wmId; wmId = LOWORD(wParam);
		switch (wmId)
		{
		case 1:
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				wchar_t tmp[1001], tmp2[1001];
				SendMessage(FileList, LB_GETTEXT, ::SendMessage(FileList, LB_GETCURSEL, 0, 0), (LPARAM)tmp);
				wcscpy_s(tmp2, Path);
				wcscat_s(tmp2, L"language\\");
				wcscat_s(tmp2, tmp);
				SwitchLanguage(tmp2);
				InvalidateRect(Main.hWnd, NULL, FALSE);
				break;
			}
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		POINT pt;
		GetCursorPos(&pt);
		::ScreenToClient(Main.hWnd, &pt);

		BOOL f; f = FALSE;

		for (int i = 0; i <= Main.CurButton; i++)
			if (Main.Button[i].Page == Main.CurWnd || Main.Button[i].Page == 0 || (GameMode&& Main.Button[i].Page == 6))
				if (Main.InsideButton(i, pt) && Main.Button[i].Download == 0) { f = TRUE; break; }

		for (int i = 1; i <= Main.CurEdit; i++)
			if (Main.Edit[i].Page == Main.CurWnd || Main.Edit[i].Page == 0)
				if (Main.InsideEdit(i, pt) != 0) { Main.EditDown(i); f = TRUE; break; }


		if (pt.x >= 20 * Main.DPI&&pt.x <= 52 * Main.DPI&&pt.y >= 10 * Main.DPI&&pt.y <= 42 * Main.DPI) { f = TRUE; break; }
		if (pt.x >= 170 * Main.DPI&&pt.x <= 305 * Main.DPI&&pt.y >= 75 * Main.DPI&&pt.y <= 240 * Main.DPI&&Main.CurWnd == 4) { f = TRUE; break; }
		if (f == FALSE)
		{
			for (int i = 1; i <= Main.CurCheck; i++)
				if (Main.Check[i].Page == Main.CurWnd || Main.Check[i].Page == 0)
					if (Main.InsideCheck(i, pt) != 0) { f = TRUE; break; }

			if (f == FALSE)
				PostMessage(Main.hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
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
		Main.Edit[Main.CoverEdit].Press = false;
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(Main.hWnd, &point);

		if (point.x >= 20 * Main.DPI&&point.x <= 52 * Main.DPI&&point.y >= 10 * Main.DPI&&point.y <= 42 * Main.DPI)
		{
			DWORD col;
			col = DoSelectColour();
			if (col == 0)break;
			green = CreateSolidBrush(col);
			GREEN = CreatePen(PS_SOLID, 2, col);
			InvalidateRect(Main.hWnd, 0, 0);
			break;
		}

		if (point.x >= 170 * Main.DPI&&point.x <= 305 * Main.DPI&&point.y >= 75 * Main.DPI&&point.y <= 240 * Main.DPI&&Main.CurWnd == 4) {
			EasterEgg(true);
			break;
		}
		unsigned int x;
		x = Hash(Main.GetCurInsideID());
		BUTTON_IN(x, L"P1") { Main.SetPage(1); EasterEgg(false); ShowWindow(FileList, SW_HIDE); break; }
		BUTTON_IN(x, L"P2") { Main.SetPage(2); EasterEgg(false); ShowWindow(FileList, SW_HIDE);  break; }
		BUTTON_IN(x, L"P3") { Main.SetPage(3); EasterEgg(false); ShowWindow(FileList, SW_HIDE); break; }
		BUTTON_IN(x, L"P4") { if (!haveInfo) UpdateInfo(), haveInfo = true; Main.SetPage(4); ShowWindow(FileList, SW_HIDE); break; }
		BUTTON_IN(x, L"P5") { Main.SetPage(5); EasterEgg(false); ShowWindow(FileList, SW_SHOW); break; }
		BUTTON_IN(x, L"QuickSetup")
		{
			oneclick = 1 - oneclick;
			if (oneclick == 1)
			{
				RegisterHotKey(Main.hWnd, 3, NULL, VK_SCROLL);
				Main.InfoBox(Main.GetStr(L"OneOK"));
				wcscpy(Main.Button[Main.GetNumbyID(L"QuickSetup")].Name, Main.GetStr(L"unQS"));
			}
			else
			{
				UnregisterHotKey(Main.hWnd, 3);
				Main.InfoBox(Main.GetStr(L"unQSOK"));
				wcscpy(Main.Button[Main.GetNumbyID(L"QuickSetup")].Name, Main.GetStr(L"setQS"));
			}
			break;
		}
		BUTTON_IN(x, L"DelR3") { AutoDeleteSethc(); break; }
		BUTTON_IN(x, L"DelR0")
		{
			if (!DeleteSethcS())Main.InfoBox(Main.GetStr(L"DSR0Fail"));
			break;
		}
		BUTTON_IN(x, L"SethcR3") { AutoSetupSethc(); AutoCopyNTSD(); break; }
		BUTTON_IN(x, L"SethcR0")
		{
			int flag = SetupSethcS();
			if (flag == 0)Main.InfoBox(Main.GetStr(L"CSFail"));
			if (flag == 2)Main.InfoBox(Main.GetStr(L"NoSethc"));
			break;
		}
		BUTTON_IN(x, L"hookS")
		{
			KillProcess(L"hoo");

			AutoCopyNTSD();

			wchar_t tmp[1001];
			wcscpy_s(tmp, Path);
			wcscat_s(tmp, L"hook.exe");
			if (!RunEXE(tmp))Main.InfoBox(Main.GetStr(L"OneFail"));
			break;
		}
		BUTTON_IN(x, L"hookU") { KillProcess(L"hoo"); break; }
		BUTTON_IN(x, L"runinVD")
		{
			STARTUPINFO si = { 0 };
			PROCESS_INFORMATION pi = { 0 };
			TCHAR tmp[1001];
			si.cb = sizeof(si);
			si.lpDesktop = szVDesk;
			wcscpy_s(tmp, Main.Edit[Main.GetNumByIDe(L"E_runinVD")].str);
			if (!CreateProcess(NULL, tmp, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				Main.InfoBox(Main.GetStr(L"StartFail"));
			break;
		}
		BUTTON_IN(x, L"SwitchD") { SDesktop(); break; }

		BUTTON_IN(x, L"ApplyCh")
		{
			wchar_t tmp[1001];
			wcscpy_s(tmp, Main.Edit[Main.GetNumByIDe(L"E_ApplyCh")].str);

			HKEY hKey;
			LONG ret;
			if (Bit != 64)
				ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class\\Student", 0, KEY_SET_VALUE, &hKey);
			else
				ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student", 0, KEY_SET_VALUE, &hKey);

			if (ret != 0)
			{
				Main.InfoBox(Main.GetStr(L"ACFail"));
				RegCloseKey(hKey);
				break;
			}
			int a = _wtoi(tmp);
			ret = RegSetValueEx(hKey, L"ChannelId", 0, REG_DWORD, (const BYTE*)&a, sizeof(int));
			if (ret != 0)
			{
				Main.InfoBox(Main.GetStr(L"ACUKE"));
				RegCloseKey(hKey);
				break;
			}
			RegCloseKey(hKey);
			Main.InfoBox(Main.GetStr(L"ACOK"));
			break;
		}
		BUTTON_IN(x, L"ClearPass")
		{
			AutoClearPassWd();
			break;
		}
		BUTTON_IN(x, L"ViewPass")
		{
			AutoViewPass();
			break;
		}
		BUTTON_IN(x, L"CP1") { ChangePasswordEx(1); break; }
		BUTTON_IN(x, L"CP2") { ChangePasswordEx(2); break; }
		BUTTON_IN(x, L"CP3") { ChangePasswordEx(3); break; }
		BUTTON_IN(x, L"re-TD") { ReopenTD(); break; }
		BUTTON_IN(x, L"windows.ex")
		{
			if (FC == TRUE)
			{
				CatchWnd.InitClass(hInst);
				InitCathy();
				FC = FALSE;//First start CatchWnd
			}

			if (CatchWnd.hWnd != 0)ShowWindow(CatchWnd.hWnd, SW_HIDE);
			CatchWnd.hWnd = CreateWindowW(CatchWindow, L"捕捉窗口", WS_OVERLAPPEDWINDOW, 200, 200, 625, 550, nullptr, nullptr, hInst, nullptr);
			CreateCaret(CatchWnd.hWnd, NULL, 1, 20);
			ShowWindow(CatchWnd.hWnd, SW_SHOW);
			UpdateWindow(CatchWnd.hWnd);
			break;
		}

		BUTTON_IN(x, L"ANTI-") { DeleteShutdown(); break; }
		BUTTON_IN(x, L"desktop")
		{
			wchar_t tmp[351];
			wcscpy_s(tmp, L"psexec.exe -x -i -s -d \"");
			wcscat_s(tmp, Name);
			wcscat_s(tmp, L"\" -top");
			//s(tmp);
			DWORD word = 1; HKEY hKey; LONG ret;
			ret = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Sysinternals\\PsExec", 0, KEY_SET_VALUE, &hKey);
			ret = RegSetValueEx(hKey, L"EulaAccepted", 0, REG_DWORD, (const BYTE*)&word, sizeof(DWORD));
			RunEXE(tmp);
			break;
		}
		BUTTON_IN(x, L"auto-5")
		{
			CurrentProcessId = GetCurrentProcessId();
			KillTop();
			break;
		}
		BUTTON_IN(x, L"Usethc")
		{
			DeleteFile(SethcPath);
			CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE);
			break;
		}
		BUTTON_IN(x, L"viewfile") { openfile(); break; }
		BUTTON_IN(x, L"folder") { BrowseFolder(); break; }
		BUTTON_IN(x, L"TI")
		{
			wchar_t tmp[1001];
			wcscpy_s(tmp, Main.Edit[Main.GetNumByIDe(L"E_View")].str);
			AutoDelete(tmp);
			break;
		}
		BUTTON_IN(x, L"TA") { WinExec("deleter\\DrvDelFile.exe", SW_SHOW); break; }
		BUTTON_IN(x, L"BSOD") { BSOD(); break; }
		BUTTON_IN(x, L"NtShutdown") { Restart(); break; }

		BUTTON_IN(x, L"Games")
		{
			if (GameMode == 0)
			{
				wchar_t tmp[301];
				wcscpy_s(tmp, Path);
				wcscat_s(tmp, L"games");
				CreateDirectory(tmp, NULL);

				for (int i = 0; i < numGames; ++i)
				{
					wcscpy_s(tmp, Path);
					wcscat_s(tmp, GameName[i]);
					if (GetFileAttributes(tmp) != -1)GameExist[i] = TRUE;
				}

				wcscpy_s(Main.Button[Main.CurCover].Name, L"停止");
				GameMode = 1;
				if (!lock)lock = true, CreateThread(NULL, 0, GameThread, 0, 0, NULL); else GameMode = 0, wcscpy_s(Main.Button[Main.CurCover].Name, L"打游戏");
			}
			else
			{
				GameMode = 0;
				wcscpy_s(Main.Button[Main.CurCover].Name, L"打游戏");
				if (!lock)lock = true, CreateThread(NULL, 0, GameThread, 0, 0, NULL); else GameMode = 1, wcscpy_s(Main.Button[Main.CurCover].Name, L"停止");
			}
			break;
		}
		BUTTON_IN(x, L"ARP")//cathy
		{
			wchar_t tmp[] = L"arp\\arp.exe";
			CopyFile(L"arp\\npf.sys", L"C:\\Windows\\System32\\drivers\\npf.sys", FALSE);
			CopyFile(L"arp\\npf.sys", L"C:\\Windows\\SysNative\\drivers\\npf.sys", FALSE);
			CopyFile(L"arp\\npf.sys", L"C:\\Windows\\SysWOW64\\drivers\\npf.sys", FALSE);

			RunEXE(tmp);
			break;
		}
		BUTTON_IN(x, L"SuperCMD")
		{
			wchar_t tmp[] = L"psexec.exe -i -s -d cmd.exe";
			DWORD word = 1;
			HKEY hKey;
			LONG ret;
			ret = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Sysinternals\\PsExec", 0, KEY_SET_VALUE, &hKey);
			ret = RegSetValueEx(hKey, L"EulaAccepted", 0, REG_DWORD, (const BYTE*)&word, sizeof(DWORD));
			RunEXE(tmp);
			break;
		}
		BUTTON_IN(x, L"Killer")
		{
			UnloadNTDriver(L"360");
			bool flag;
			Main.InfoBox(Main.GetStr(L"360Start"));
			if (Bit == 32)
				flag = LoadNTDriver(L"360", L"x32\\360.sys");
			else
				flag = LoadNTDriver(L"360", L"x64\\360.sys");
			if (flag == false)
				Main.InfoBox(Main.GetStr(L"360Fail"));
			break;
		}
		BUTTON_IN(x, L"more.txt")
		{
			wchar_t tmp[301];
			wcscpy_s(tmp, L"Notepad ");
			wcscat_s(tmp, Path);
			wcscat_s(tmp, L"关于&帮助.txt");
			RunEXE(tmp);
			break;
		}
		BUTTON_IN(x, L"sysinfo")
		{
			wchar_t tmp[34];
			wcscpy_s(tmp, L"C:\\Windows\\System32\\Msinfo32.exe");
			RunEXE(tmp);
			break;
		}
		BUTTON_IN(x, L"hidest") { HideState = 5; ShowWindow(Main.hWnd, SW_HIDE);	break; }
		BUTTON_IN(x, L"minisize") { ShowWindow(Main.hWnd, SW_MINIMIZE); break; }

		BUTTON_IN(x, L"Game1") {
			if (GameExist[0])RunEXE(GameName[0]);
			else { int typ = 1; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }
			break;
		}
		BUTTON_IN(x, L"Game2") {
			if (GameExist[1])RunEXE(GameName[1]);
			else { int typ = 2; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }
			break;
		}
		BUTTON_IN(x, L"Game3") {
			if (GameExist[2])RunEXE(GameName[2]);
			else { int typ = 3; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }
			break;
		}
		BUTTON_IN(x, L"Game4") {
			if (GameExist[3])RunEXE(GameName[3]);
			else { int typ = 4; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }
			break;
		}
		BUTTON_IN(x, L"Game5")
		{
			if (GameExist[4])RunEXE(GameName[4]);
			else { int typ = 5; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }
			break;
		}
		BUTTON_IN(x, L"Game6")
		{
			if (GameExist[5])RunEXE(GameName[5]);
			else { int typ = 6; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }
			break;
		}

		BUTTON_IN(x, L"Close") { PostQuitMessage(0); break; }

		for (int i = 1; i <= Main.CurCheck; i++)
		{
			if (Main.Check[i].Page == 0 || Main.Check[i].Page == Main.CurWnd)
			{
				int result;
				result = Main.InsideCheck(i, point);
				if (result != 0)
				{
					if (!Main.Check[i].Value)
					{
						switch (i)
						{
						case 1:
						{
							wchar_t tmp[301];
							wcscpy_s(tmp, Path);
							wcscat_s(tmp, L"cheat\\old.exe");
							RunEXE(tmp);
							break;
						}
						case 2:
						{
							wchar_t tmp[301];
							wcscpy_s(tmp, Path);
							wcscat_s(tmp, L"cheat\\new.exe");
							RunEXE(tmp);
							break;
						}
						case 3:
						{
							NOTIFYICONDATA tnd;
							tnd.cbSize = sizeof(NOTIFYICONDATA);
							tnd.hWnd = Main.hWnd;
							tnd.uID = IDR_MAINFRAME;
							tnd.uFlags = 2 | 1 | 4;
							tnd.uCallbackMessage = 34;
							tnd.hIcon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_TD));
							wcscpy_s(tnd.szTip, Main.GetStr(L"tnd"));
							Shell_NotifyIcon(NIM_ADD, &tnd);
							break;
						}
						case 4:
						{
							TOP = TRUE;
							CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);
							break;
						}
						case 5:
						{
							if (Admin == 0)Main.InfoBox(Main.GetStr(L"BSODAsk"));
							RegisterHotKey(Main.hWnd, 4, MOD_CONTROL, 'R');
							break;
						}
						case 6:
						{
							RegisterHotKey(Main.hWnd, 5, MOD_CONTROL, 'T');
							break;
						}
						case 7:
						{
							if (FS == TRUE)InitScreen(), FS = FALSE;
							RegisterHotKey(Main.hWnd, 6, MOD_CONTROL | MOD_ALT, 'P');
							break;
						}
						case 8:
						case 9:
						case 10:
							SetTimer(hWnd, 1, 100, (TIMERPROC)TimerProc);
							break;
						case 11:
							RegMouseKey();
							break;
						case 12:
							typedef DWORD(CALLBACK* SEtProcessDPIAware)(void);
							SEtProcessDPIAware SetProcessDPIAware;
							HMODULE huser;
							huser = LoadLibrary(L"user32.dll");
							SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
							if (SetProcessDPIAware != NULL)SetProcessDPIAware();
							break;
						case 13:
							Main.SetDPI(1.5);
							SetWindowPos(FileList, 0, 180 * Main.DPI, 400 * Main.DPI, 265 * Main.DPI, 120 * Main.DPI, NULL);
							SendMessage(FileList, WM_SETFONT, WPARAM(Main.DefFont), 0);
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
							NOTIFYICONDATA tnd;
							tnd.cbSize = sizeof(NOTIFYICONDATA);
							tnd.hWnd = Main.hWnd;
							tnd.uID = IDR_MAINFRAME;
							Shell_NotifyIcon(NIM_DELETE, &tnd);
							break;
						case 4:
							TOP = FALSE;
							break;
						case 5:
							UnregisterHotKey(Main.hWnd, 4);
							break;
						case 6:
							UnregisterHotKey(Main.hWnd, 5);
							break;
						case 7:
							UnregisterHotKey(Main.hWnd, 6);
							break;
						case 8:
							KillTimer(hWnd, 1);
							break;
						case 9:
							KillTimer(hWnd, 1);
							UnhookWindowsHookEx(KeyboardHook);
							break;
						case 11:
							UnMouseKey();
							break;
						case 13:
							Main.SetDPI(0.75);
							SetWindowPos(FileList, 0, 180 * Main.DPI, 400 * Main.DPI, 265 * Main.DPI, 120 * Main.DPI, NULL);
							SendMessage(FileList, WM_SETFONT, WPARAM(Main.DefFont), 0);
							break;
						}
					}
					RECT rc = Main.GetRECTc(i);
					Main.Check[i].Value = 1 - Main.Check[i].Value;
					Main.RedrawType = 3;
					Main.RedrawCur = i;
					InvalidateRect(Main.hWnd, &rc, FALSE);
				}
			}
		}
		Main.Timer = time(0);
		Main.DestroyExp();
		break;
	case WM_MOUSEMOVE:
		Main.MouseMove();
		break;
	case WM_IME_STARTCOMPOSITION:
	{
		LOGFONT lf;
		COMPOSITIONFORM cf;
		HIMC himc = ImmGetContext(hWnd);
		if (himc)
		{
			// 设置输入法显示位置。
			//POINT point;
			GetCaretPos(&point);
			cf.dwStyle = CFS_POINT;
			cf.ptCurrentPos.y = point.y + 10;
			cf.ptCurrentPos.x = point.x + 10;
			ImmSetCompositionWindow(himc, &cf);

			//输入法字体样式
			GetObject(Main.DefFont, sizeof(LOGFONT), &lf);
			ImmSetCompositionFont(himc, &lf);

			ImmReleaseContext(hWnd, himc);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		//break;
	}
	case WM_MOUSELEAVE:
		PostMessage(Main.hWnd, WM_MOUSEMOVE, NULL, 0);
		break;
	case WM_CHAR:
		Main.EditCHAR(wParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
HDC sdc;

LRESULT CALLBACK ScreenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		sdc = GetDC(hWnd);
		pdc = CreateCompatibleDC(sdc);
		RegisterHotKey(hWnd, 1, NULL, VK_ESCAPE);
		SwitchToThisWindow(hWnd, NULL);
		break;
	case WM_CLOSE:
		UnregisterHotKey(hWnd, 1);
		ScreenState = 0;
		ShowWindow(hWnd, SW_HIDE);
		break;
	case WM_HOTKEY:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_PAINT:
		HBITMAP bitmap;
		bitmap = (HBITMAP)LoadImage(hInst, L"C:\\SAtemp\\ScreenShot.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		SelectObject(pdc, bitmap);

		sdc = BeginPaint(hWnd, &ps);

		DEVMODE curDevMode;
		curDevMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMode);
		SetStretchBltMode(sdc, HALFTONE);
		StretchBlt(sdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), pdc, 0, 0, curDevMode.dmPelsWidth, curDevMode.dmPelsHeight, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK CatchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		Hdc = GetDC(CatchWnd.hWnd);
		tdc = CreateCompatibleDC(Hdc);
		HBMP = CreateCompatibleBitmap(Hdc, 2000, 2000);
		SelectObject(tdc, HBMP);
		ReleaseDC(CatchWnd.hWnd, Hdc);
		CatchWnd.SetHDC(tdc);
	case WM_CLOSE:
		KillTimer(hWnd, 4);
		ReturnWindows();
		ShowWindow(hWnd, SW_HIDE);
		break;
	case WM_PAINT:
		Hdc = BeginPaint(CatchWnd.hWnd, &ps);

		if (tdhcur == 0)
		{
			SelectObject(tdc, WhiteBrush);//白色背景
			SelectObject(tdc, White);
			SelectObject(tdc, CatchWnd.DefFont);
			Rectangle(tdc, 0, 0, 2000, 2000);


			//s(CatchWnd.CurButton);
			CatchWnd.DrawButtons(0);
			CatchWnd.DrawEdits(0);
			TCHAR tmp1[20], tmp2[20];
			wcscpy_s(tmp1, CatchWnd.GetStr(L"Eat1"));
			_itow_s(CurEat, tmp2, 10);
			wcscat_s(tmp1, tmp2);
			wcscat_s(tmp1, CatchWnd.GetStr(L"Eat2"));
			TextOutW(tdc, 20, 155, tmp1, (int)wcslen(tmp1));
			BitBlt(Hdc, 0, 0, 2000, 2000, tdc, 0, 0, SRCCOPY);
		}
		if (tdhcur != 0)
		{

			HDC hdctd = GetDC(tdh[displaycur]);
			RECT rc, rc2;
			int left, width, top, height;

			GetClientRect(tdh[displaycur], &rc);

			GetClientRect(CatchWnd.hWnd, &rc2);
			Rectangle(tdc, 0, 0, rc2.right-rc2.left, rc2.bottom-rc2.top);
			if ((rc2.right - rc2.left) == 0 || (rc2.bottom - rc2.top) == 0 || (rc.right - rc.left) == 0 || (rc.bottom - rc.top) == 0)break;
			double wh1 = (double)(rc.right - rc.left) / (double)(rc.bottom - rc.top),
				wh2 = (double)(rc2.right - rc2.left) / (double)(rc2.bottom - rc2.top);
			if (wh1 > wh2)
			{
				left = 0;
				width = rc2.right - rc2.left;
				top = (rc2.bottom - rc2.top) / 2 - (rc2.right - rc2.left) / wh1 / 2;
				height = (rc2.right - rc2.left) / wh1;
			}
			else
			{
				top = 0;
				height = rc2.bottom - rc2.top;
				left = (rc2.right - rc2.left) / 2 - (rc2.bottom - rc2.top) * wh1 / 2.0;
				width = (rc2.bottom - rc2.top) * wh1;
			}

			SetStretchBltMode(CatchWnd.hdc, HALFTONE);
			StretchBlt(Hdc,
				left, top,
				width, height,
				hdctd,
				0, 0,
				rc.right - rc.left,
				rc.bottom - rc.top,
				SRCCOPY);
			//BitBlt(Hdc, left, top,width, height, tdc, left, top, SRCCOPY);
			ReleaseDC(tdh[displaycur], hdctd);
		}
		EndPaint(CatchWnd.hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		CatchWnd.LeftButtonDown();
		break;
	case WM_MOUSEMOVE:
		CatchWnd.MouseMove();

		break;
	case WM_LBUTTONUP:
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);

		if (CatchWnd.CurCover != -1)
		{
			CatchWnd.Press = 0;
			RECT rc = CatchWnd.GetRECT(CatchWnd.CurCover);
			InvalidateRect(CatchWnd.hWnd, &rc, FALSE);
		}

		CatchWnd.Edit[CatchWnd.CoverEdit].Press = false;
		for (int i = 1; i <= CatchWnd.CurButton; i++)
		{
			if (CatchWnd.InsideButton(i, point))
			{
				switch (i)
				{
				case 1:
					timerleft = _wtoi(CatchWnd.Edit[CatchWnd.GetNumByIDe(L"E_Delay")].str);
					CatchWnd.SetString(CatchWnd.Button[1].Name, L"back");
					SetTimer(CatchWnd.hWnd, 2, 1000, (TIMERPROC)TimerProc);
					break;
				case 2:
				{
					wchar_t x[1001];
					wcscpy_s(x, CatchWnd.Edit[CatchWnd.GetNumByIDe(L"E_Pname")].str);
					tdhcur = 0;
					FindTop(x);
					if (tdhcur != 0)displaycur = 1; else displaycur = 0;
					SetTimer(CatchWnd.hWnd, 6, 16, (TIMERPROC)TimerProc);
					RegisterHotKey(hWnd, 513, MOD_ALT, VK_LEFT);
					RegisterHotKey(hWnd, 514, MOD_ALT, VK_RIGHT);
					RegisterHotKey(hWnd, 515, NULL, VK_ESCAPE);
					RegisterHotKey(hWnd, 516, MOD_ALT, 'H');
					Sleep(10);
					break;
				}
				case 3: {ReturnWindows(); break; }
				}
			}
		}
		break;
	case WM_CHAR:
	{
		CatchWnd.EditCHAR(wParam);
		break;
	}
	case WM_SETFOCUS:
	{
		if (tdhcur != 0)
		{
			RegisterHotKey(hWnd, 513, MOD_ALT, VK_LEFT);
			RegisterHotKey(hWnd, 514, MOD_ALT, VK_RIGHT);
			RegisterHotKey(hWnd, 515, NULL, VK_ESCAPE);
			RegisterHotKey(hWnd, 516, MOD_ALT, 'H');
		}
		CatchWnd.EditRegHotKey();
		break;
	}
	case WM_KILLFOCUS:
	{
		for (int i = 513; i < 517; ++i)UnregisterHotKey(hWnd, i);

		CatchWnd.EditUnHotKey();
		break;
	}
	case WM_HOTKEY:
	{
		if (wParam == 513)displaycur--;
		if (wParam == 514)displaycur++;
		if (wParam == 515) { tdhcur = 0, KillTimer(hWnd, 6), InvalidateRect(hWnd, NULL, TRUE); for (int i = 513; i < 517; ++i)UnregisterHotKey(hWnd, i); }
		if (wParam == 516)
			if ((GetWindowLongW(tdh[displaycur], GWL_STYLE) & WS_VISIBLE) != 0)ShowWindow(tdh[displaycur], SW_HIDE); else ShowWindow(tdh[displaycur], SW_SHOW);
		if (displaycur == 0)displaycur = tdhcur;
		if (displaycur > tdhcur)displaycur = 1;
		CatchWnd.EditHotKey(wParam);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

VOID InitCathy()
{
	WNDCLASSEXW cat = { 0 };
	cat.cbSize = sizeof(WNDCLASSEX);
	cat.style = CS_HREDRAW | CS_VREDRAW;
	cat.lpfnWndProc = CatchProc;
	cat.hInstance = hInst;
	cat.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_GUI));//大图标
	cat.hCursor = LoadCursor(nullptr, IDC_ARROW);
	cat.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	cat.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
	cat.lpszClassName = CatchWindow;
	cat.hIconSm = LoadIcon(cat.hInstance, MAKEINTRESOURCE(IDI_GUI));//小图标
	RegisterClassExW(&cat);
	CatchWnd.CreateEditEx(10 + 5, 20, 260 - 10, 50, 0, L"StudentMain.exe", L"E_Pname", CatchWnd.DefFont);
	CatchWnd.CreateEditEx(285 + 5, 20, 45 - 10, 50, 0, L"5", L"E_Delay", CatchWnd.DefFont);
	CatchWnd.CreateButton(10, 85, 100, 50, 0, L"捕捉窗口", L"");
	CatchWnd.CreateButton(120, 85, 100, 50, 0, L"监视窗口", L"");
	CatchWnd.CreateButton(230, 85, 100, 50, 0, L"释放窗口", L"");
}
VOID InitScreen()
{
	WNDCLASSEXW scr = { 0 };
	scr.cbSize = sizeof(WNDCLASSEX);
	scr.lpfnWndProc = ScreenProc;
	scr.hInstance = hInst;
	scr.hCursor = LoadCursor(nullptr, IDC_ARROW);
	scr.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	scr.lpszClassName = ScreenWindow;
	RegisterClassExW(&scr);
}