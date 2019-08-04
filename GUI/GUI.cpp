//这是TopDomainTools工程源代码的主文件
//by zouxiaofei1 2015 - 2019

#include "stdafx.h"//头文件
#include "GUI.h"
#include "WndShadow.h"
#include "Actions.h"
#include "TestFunctions.h"
#include <atlimage.h>

#pragma comment(lib, "urlmon.lib")//下载文件用的Lib
#pragma comment(lib,"Imm32.lib")//自定义输入法位置用的Lib
#pragma comment(lib, "ws2_32.lib")//Winsock API 库
#pragma comment(lib, "netapi32.lib")//同上

BOOL				InitInstance(HINSTANCE, int);//部分(重要)函数的前向声明
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);//主窗口
LRESULT CALLBACK	CatchProc(HWND, UINT, WPARAM, LPARAM);//"捕捉窗口"的窗口
LRESULT CALLBACK	UpGProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//下载文件窗口
ATOM				InitScreen();//注册截图伪装窗口的Class
ATOM				InitBSOD();
void	CALLBACK	TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);//计时器
void SearchLanguageFiles();

//自己定义的 有点乱

HINSTANCE hInst;// 当前实例备份变量，CreateWindow&LoadIcon时需要
const wchar_t szWindowClass[] = L"GUI";//主窗口类名
const wchar_t CatchWindow[] = L"CatchWindow";//第二窗口类名
const wchar_t ScreenWindow[] = L"ScreenWindow";//截图伪装窗口类名
const wchar_t BSODWindow[] = L"BSODWindow";//伪装蓝屏窗口类名
const wchar_t UpWindow[] = L"UpdateWindow";//窗口类名
BOOL Effect = TRUE;//特效开关
wchar_t Path[301], Name[301];//程序路径 and 路径+程序名 
wchar_t SethcPath[255], ntsdPath[255];//Sethc路径 & ntsd路径
BOOL FC = TRUE, FS = TRUE, FU = TRUE, FB = TRUE;//是否第一次启动窗口并注册类
HBITMAP hZXFBitmap, hZXFsign;//两个图片句柄
int ScreenState;//截图伪装状态 1 = 截图 2 = 显示
HDESK hVirtualDesk, hCurrentDesk, defaultDesk;//虚拟桌面 & 当前桌面 & 默认桌面
wchar_t szVDesk[] = L"GUIdesk", fBSODdesk[] = L"GUIBSOD";//虚拟桌面名称
BOOL Admin; //是否管理员
int Bit;//系统位数 32 34 64
HHOOK KeyboardHook, MouseHook;//键盘/鼠标钩子
BOOL HideState;//窗口是否隐藏
BOOL oneclick;//一键安装状态
int GameMode;//游戏模式？
int EasterEggState;//CopyLeft文字循环状态
BOOL EasterEggFlag = FALSE;
wchar_t EasterEggStr[11][15] = { L"AnswerKey",L"Left" ,L"Left",L"Right",L"Down",L"Up",L"In",L"On",L"Back",L"Front",L"Teacher" };
BOOL slient = FALSE;//是否命令行
HBRUSH DBlueBrush, LBlueBrush, WhiteBrush, BlueBrush, green, grey, yellow, Dgrey, BSODBrush, BlackBrush;//笔刷
HPEN YELLOW, RED, BLACK, White, GREEN, LGREY, BLUE, DBlue, LBlue, BSODPen;//笔
HWND FileList;//语言选择hwnd
BOOL lock = FALSE;//Game按钮锁定
int timerleft = -2;//吃窗口倒计时
int BSODstate = 0;
HWND BSODhwnd;
wchar_t zxfback[101];//倒计时备份按钮名称
std::map<int, bool>Eatpid;//吃窗口hWnd记录map
std::stack<HWND>EatList;
HDC hdc, rdc;//主窗口缓冲hdc + 贴图hdc
HBITMAP hBmp, lBmp;//主窗口hbmp
HDC pdc, ldc;//截图伪装窗口hdc
CWndShadow Cshadow;//主窗口阴影特效
BOOL TOP;//是否置顶
BOOL BlackBoard = false;
const int numGames = 6, numFiles = 12;// 游戏/文件 数
BOOL GameExist[numGames + 1], FileExist[numFiles + 1];
wchar_t GameName[numGames + 1][25] = { L"Games\\xiaofei.exe", L"Games\\fly.exe",L"Games\\2048.exe",L"Games\\block.exe", \
L"Games\\1.exe" , L"Games\\chess.exe" };//(游戏)名
const wchar_t FileName[numFiles + 1][34] = { L"hook.exe" ,L"sethc.exe",L"ntsd.exe" ,L"PsExec.exe",L"cheat\\old.exe", L"cheat\\new.exe" ,L"deleter\\DrvDelFile.exe", L"arp\\arp.exe",\
L"ProcessHacker\\ProcessHacker.exe",L"x32\\sethc.exe",L"x64\\Kill.sys",L"language\\English.ini" };
int FDcur, FDtot;//UpWnd中 已下载的文件 \ 总文件数
DWORD expid[100], tdpid[100];//explorer PID + 被监视窗口 PID
HWND tdh[101]; //被监视窗口hwnd
int tdhcur, displaycur;//被监视窗口数量 + 正在被监视的窗口编号
bool haveInfo = false;//是否已经检查过系统信息
wchar_t wip[101];//ip地址字符串
bool SethcState = true;//System32目录下sethc是否存在
bool SethcInstallState = false;//Sethc方案状态
DWORD TDPID;//记录极域程序PID
bool TDProcess;//极域是否在运行


//空下几行为Class留位置


class CathyClass//控件主类
{
public:
	void InitClass(HINSTANCE HInstance)//新Class使用之前最好Init一下
	{								//  （不Init也行）
		hInstance = HInstance;
		CurButton = CurFrame = CurCheck = CurLine = CurText = 0;
		CurWnd = 1;//清零
		CurCover = -1;
		CoverCheck = 0;

		//默认宋体
		DefFont = CreateFontW((int)(16 * DPI), (int)(8 * DPI), 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
	}

	ATOM RegisterClass(HINSTANCE h, WNDPROC proc, LPCWSTR ClassName)
	{//注册Class
		WNDCLASSEXW wcex = { 0 };
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = proc;
		wcex.hInstance = h;
		wcex.hIcon = LoadIcon(h, MAKEINTRESOURCE(IDI_GUI));//大图标
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
		wcex.lpszClassName = ClassName;//自定义ClassName和WndProc
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUI));//小图标
		return RegisterClassExW(&wcex);
	}
	inline wchar_t* GetStr(LPCWSTR ID) { return str[Hash(ID)]; }//通过Hash + map 来快速索引字符串的数据结构
	//															ID(索引字符串) -> Hash -(map)> 字符串地址

	void SetStr(LPCWSTR Str, LPCWSTR ID)//通过ID设置字符串
	{
		delete[]str[Hash(ID)];
		str[Hash(ID)] = new wchar_t[wcslen(Str) + 1];
		wcscpy(str[Hash(ID)], Str);
	}

	void CreateString(LPCWSTR Str, LPCWSTR ID)//创建新字符串
	{
		CurString++;
		if (Str != NULL)
		{
			string[CurString].str = new wchar_t[wcslen(Str) + 1];
			wcscpy(string[CurString].str, Str);
		}
		wcscpy_s(string[CurString].ID, ID);
		str[Hash(ID)] = string[CurString].str;
	}

	void CreateEditEx(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, LPCWSTR ID, HFONT Font, BOOL Ostr)//创建自绘输入框
	{
		++CurEdit;
		Edit[CurEdit].Left = Left; Edit[CurEdit].Top = Top;
		Edit[CurEdit].Width = Wid; Edit[CurEdit].Height = Hei;
		Edit[CurEdit].Page = Page;

		if (!Ostr)
			wcscpy_s(Edit[CurEdit].OStr, name),
			Edit[CurEdit].font = Font,
			Edit[CurEdit].str = new wchar_t[21];
		else
			SetEditStrOrFont(name, Font, CurEdit);
		wcscpy_s(Edit[CurEdit].ID, ID);
	}

	void CreateArea(int Left, int Top, int Wid, int Hei, int Page)//创建点击区域
	{
		++CurArea;
		Area[CurArea].Left = Left; Area[CurArea].Top = Top;
		Area[CurArea].Width = Wid; Area[CurArea].Height = Hei;
		Area[CurArea].Page = Page;
	}

	void CreateButtonEx(int Number, int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, HBRUSH Leave, \
		HBRUSH Hover, HBRUSH press, HPEN Leave2, HPEN Hover2, HPEN Press2, HFONT Font, BOOL Enabled, BOOL Visible, COLORREF FontRGB, LPCWSTR ID)//创建按钮
	{
		Button[Number].Left = Left; Button[Number].Top = Top;
		Button[Number].Width = Wid; Button[Number].Height = Hei;
		Button[Number].Page = Page; Button[Number].Leave = Leave;
		Button[Number].Hover = Hover; Button[Number].Press = press;
		Button[Number].Leave2 = Leave2; Button[Number].Hover2 = Hover2;
		Button[Number].Press2 = Press2; Button[Number].Font = Font;
		Button[Number].Enabled = Enabled; Button[Number].Visible = Visible;
		Button[Number].FontRGB = FontRGB;
		wcscpy_s(Button[Number].Name, name);
		wcscpy_s(Button[Number].ID, ID);
		but[Hash(ID)] = Number;

		LOGBRUSH LogBrush;
		LOGPEN LogPen;//RGBTRIPLE
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
	void CreateButton(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, LPCWSTR ID)//创建按钮（简化版）
	{
		++CurButton;
		CreateButtonEx(CurButton, Left, Top, Wid, Hei, Page, name, WhiteBrush, DBlueBrush, LBlueBrush, BLACK, BLACK, BLACK, 0, TRUE, TRUE, RGB(0, 0, 0), ID);
	}

	//这里的name Wid Hei 不用全名是因为警告"隐藏了全局声明"

	void CreateFrame(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name)//创建内容框
	{
		++CurFrame;
		Frame[CurFrame].Left = Left; Frame[CurFrame].Page = Page;
		Frame[CurFrame].Height = Hei; Frame[CurFrame].Top = Top;
		Frame[CurFrame].Width = Wid;
		wcscpy_s(Frame[CurFrame].Name, name);
	}

	void CreateCheck(int Left, int Top, int Page, int Wid, LPCWSTR name)//创建check
	{
		++CurCheck;
		Check[CurCheck].Left = Left; Check[CurCheck].Top = Top;
		Check[CurCheck].Page = Page; Check[CurCheck].Width = Wid;
		wcscpy_s(Check[CurCheck].Name, name);
	}

	void CreateText(int Left, int Top, int Page, LPCWSTR name, COLORREF rgb)//创建注释文字
	{
		++CurText;
		Text[CurText].Left = Left; Text[CurText].Top = Top;
		Text[CurText].Page = Page; Text[CurText].rgb = rgb;
		wcscpy_s(Text[CurText].Name, name);
	}

	void CreateLine(int StartX, int StartY, int EndX, int EndY, int Page, COLORREF rgb)//创建线段
	{
		++CurLine;
		Line[CurLine].StartX = StartX; Line[CurLine].StartY = StartY;
		Line[CurLine].EndX = EndX; Line[CurLine].EndY = EndY;
		Line[CurLine].Page = Page; Line[CurLine].Color = rgb;
	}

	inline BOOL InsideButton(int cur, POINT & point)//判断鼠标指针是否在按钮内
	{
		return (Button[cur].Left * DPI <= point.x && Button[cur].Top * DPI <= point.y && (long)((Button[cur].Left + Button[cur].Width) * DPI) >= point.x && (long)((Button[cur].Top + Button[cur].Height) * DPI) >= point.y);
	}

	int InsideCheck(int cur, POINT & point)//同理 判断鼠标指针是否在check内
	{
		if (Check[cur].Left* DPI <= point.x && Check[cur].Top * DPI <= point.y && Check[cur].Left * DPI + 15 * DPI + 1 >= point.x//在check的方框内
			&& Check[cur].Top * DPI + 15 * DPI + 1 >= point.y)return 1;

		if (Check[cur].Left* DPI <= point.x && Check[cur].Top * DPI <= point.y && Check[cur].Left * DPI + Check[cur].Width * DPI >= point.x//在check方框右侧一定距离
			&& Check[cur].Top * DPI + 15 * DPI + 1 >= point.y)return 2;
		return 0;//不在check内
	}

	void DrawFrames(int cur)//绘制Frames
	{
		int i;
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Frame
		for (i = 1; i <= CurFrame; ++i)
		{
		begin:
			if (Frame[i].Page == CurWnd || Frame[i].Page == 0)
			{
				SelectObject(hdc, BLACK);//绘制方框
				SelectObject(hdc, DefFont);
				MoveToEx(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI), NULL);//多加点(int)xx*DPI 减少警告
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI));
				SetTextColor(hdc, Frame[i].rgb);//打印文字
				RECT rc = GetRECTf(i);
				SetBkMode(hdc, OPAQUE);
				DrawTextW(hdc, Frame[i].Name, (int)wcslen(Frame[i].Name), &rc, NULL);
				SetTextColor(hdc, NULL);
			}
			if (cur != 0)return;
		}
	}

	void DrawButtons(int cur)//绘制按钮
	{
		int i;
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Button(复制粘贴)
		for (i = 1; i <= CurButton; ++i)
		{
		begin:
			if (Button[i].Page == CurWnd || Button[i].Page == 0)
			{
				HPEN tmp = 0; HBRUSH tmb = 0;
				if (Button[i].Enabled == false)//禁用则显示灰色
				{
					SelectObject(hdc, Dgrey);
					SelectObject(hdc, Button[i].Leave2);
					SetTextColor(hdc, RGB(100, 100, 100));
					goto ok;
				}
				SetTextColor(hdc, Button[i].FontRGB);
				if (Button[i].Percent != 0 && Button[i].Percent != 100 && Button[i].DownTot == 0)//渐变色绘制
				{
					tmp = CreatePen(PS_SOLID, 1, RGB((Button[i].p2[0] - Button[i].p1[0]) * Button[i].Percent / 100 + Button[i].p1[0], \
						(Button[i].p2[1] - Button[i].p1[1]) * Button[i].Percent / 100 + Button[i].p1[1], (Button[i].p2[2] - Button[i].p1[2]) * Button[i].Percent / 100 + Button[i].p1[2]));

					SelectObject(hdc, tmp);
					tmb = CreateSolidBrush(RGB((Button[i].b2[0] - Button[i].b1[0]) * Button[i].Percent / 100 + Button[i].b1[0], \
						(Button[i].b2[1] - Button[i].b1[1]) * Button[i].Percent / 100 + Button[i].b1[1], (Button[i].b2[2] - Button[i].b1[2]) * Button[i].Percent / 100 + Button[i].b1[2]));
					SelectObject(hdc, tmb);
					goto ok;
				}
				if (CurCover == i && Button[i].DownTot == 0)//没有禁用&渐变色 -> 默认颜色
					if (Press == 1) {
						SelectObject(hdc, Button[i].Press);//按下按钮
						SelectObject(hdc, Button[i].Press2);
					}
					else {
						SelectObject(hdc, Button[i].Hover);//悬浮
						SelectObject(hdc, Button[i].Hover2);
					}
				else
				{
					SelectObject(hdc, Button[i].Leave);//离开
					SelectObject(hdc, Button[i].Leave2);
				}
			ok:
				if (Button[i].Font == NULL)SelectObject(hdc, DefFont); else SelectObject(hdc, Button[i].Font);//字体

				Rectangle(hdc, (int)(Button[i].Left * DPI), (int)(Button[i].Top * DPI),
					(int)(Button[i].Left * DPI + Button[i].Width * DPI), (int)(Button[i].Top * DPI + Button[i].Height * DPI));//绘制方框

				if (Button[i].DownTot != 0)//下载进度条
				{
					SelectObject(hdc, Button[i].Hover);
					Rectangle(hdc, (int)(Button[i].Left * DPI), (int)(Button[i].Top * DPI),
						(int)(Button[i].Left * DPI + Button[i].Width * DPI * (Button[i].Download - 1) / 100), (int)(Button[i].Top * DPI + Button[i].Height * DPI));
				}

				RECT rc = GetRECT(i);

				SetBkMode(hdc, TRANSPARENT);//去掉文字背景
				if (Button[i].DownTot == 0)
					DrawTextW(hdc, Button[i].Name, (int)wcslen(Button[i].Name), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				else
				{
					if (Button[i].Download == 101 && (Button[i].DownTot < 2 || Button[i].DownTot == Button[i].DownCur))//下载文字
					{
						DrawTextW(hdc, GetStr(L"Loaded"), 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						Button[i].Download = Button[i].DownTot = 0;
					}
					else
					{
						if (Button[i].DownTot < 2)
							DrawTextW(hdc, GetStr(L"Loading"), 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						else
						{
							wchar_t tmp1[101], tmp2[11];//正在下载 （已下载个数）/（总数）
							wcscpy_s(tmp1, GetStr(L"Loading"));
							wcscat_s(tmp1, L" ");
							_itow_s(Button[i].DownCur, tmp2, 10);
							wcscat_s(tmp1, tmp2);
							wcscat_s(tmp1, L"/");
							_itow_s(Button[i].DownTot, tmp2, 10);
							wcscat_s(tmp1, tmp2);
							DrawTextW(hdc, tmp1, wcslen(tmp1), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}
					}
				}
				if (tmp != NULL)DeleteObject(tmp);//回收句柄
				if (tmb != NULL)DeleteObject(tmb);
			}
			if (cur != 0)return;
		}
		SetTextColor(hdc, RGB(0, 0, 0));
	}

	void DrawChecks(int cur)//绘制Checks
	{
		int i;
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Check
		for (i = 1; i <= CurCheck; ++i)
		{
		begin:
			if (Check[i].Page == 0 || Check[i].Page == CurWnd)
			{
				if (i == CoverCheck)SelectObject(hdc, BLUE); else SelectObject(hdc, LGREY);

				SelectObject(hdc, grey);
				SelectObject(hdc, DefFont);//check默认边长为15，并不能调整
				Rectangle(hdc, (int)(Check[i].Left * DPI), (int)(Check[i].Top * DPI), (int)(Check[i].Left * DPI + 15 * DPI), (int)(Check[i].Top * DPI + 15 * DPI));
				TextOut(hdc, (int)(Check[i].Left * DPI + 20 * DPI), (int)(Check[i].Top * DPI), Check[i].Name, (int)wcslen(Check[i].Name));
				if (Check[i].Value == 1)//打勾
				{						//比较难看
					SelectObject(hdc, GREEN);//试过了直接贴图，更难看
					MoveToEx(hdc, (int)(Check[i].Left * DPI + 2 * DPI), (int)(Check[i].Top * DPI + 7 * DPI), NULL);//然后就只能这样了
					LineTo(hdc, (int)(Check[i].Left * DPI + 7 * DPI), (int)(Check[i].Top * DPI + 12 * DPI));
					LineTo(hdc, (int)(Check[i].Left * DPI + 12 * DPI), (int)(Check[i].Top * DPI + 2 * DPI));
				}
			}
			if (cur != 0)return;
		}
	}
	void DrawLines()//绘制线段
	{//线段一般不需要重绘
		for (int i = 1; i <= CurLine; ++i)//因此没有加ObjectRedraw
			if (Line[i].Page == 0 || Line[i].Page == CurWnd)
			{
				SelectObject(hdc, CreatePen(0, 1, Line[i].Color));
				MoveToEx(hdc, (int)(Line[i].StartX * DPI), (int)(Line[i].StartY * DPI), NULL);
				LineTo(hdc, (int)(Line[i].EndX * DPI), (int)(Line[i].EndY * DPI));
			}
	}
	void DrawTexts(int cur)//绘制文字
	{
		int i;
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Texts
		for (i = 1; i <= CurText; ++i)
		{
		begin:
			if (Text[i].Page == 0 || Text[i].Page == CurWnd)
			{
				SetTextColor(hdc, Text[i].rgb);
				SelectObject(hdc, DefFont);//文字的字体缩放效果不太理想
				wchar_t* tmp = str[Hash(Text[i].Name)];
				TextOutW(hdc, (int)(Text[i].Left * DPI), (int)(Text[i].Top * DPI), tmp, (int)wcslen(tmp));
			}
			if (cur != 0)return;
		}
	}
	void DrawExp()//绘制注释
	{//注释只有一个，自然不用ObjectRedraw
		if (ExpExist == false)return;//注释不存在？
		SelectObject(hdc, DefFont);
		SelectObject(hdc, YELLOW);
		SelectObject(hdc, yellow);
		if (ExpPoint.x > Width / 2)ExpPoint.x -= (ExpWidth + 6); else ExpPoint.x += 12;//自动选择注释的位置
		if (ExpPoint.y > Height / 2)ExpPoint.y -= (ExpHeight + 6); else ExpPoint.y += 14;//防止打印到窗口外面
		Rectangle(hdc, ExpPoint.x, ExpPoint.y, ExpWidth + ExpPoint.x, ExpHeight + ExpPoint.y);
		SetTextColor(hdc, RGB(0, 0, 0));
		for (int i = 1; i <= ExpLine; ++i)//逐行打印
			TextOutW(hdc, ExpPoint.x + 4, (int)(ExpPoint.y - 12 * DPI + 16 * i * DPI), Exp[i], (int)wcslen(Exp[i]));//注意这里的ExpPoint是真实坐标
	}

	void DrawEdits(int cur)//绘制输入框
	{
		int i;
		HDC mdc;//创建缓存dc
		mdc = CreateCompatibleDC(tdc);

		SelectObject(mdc, bitmap);
		SetBkMode(mdc, TRANSPARENT);
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Edits

		for (i = 1; i <= CurEdit; ++i)
		{
		begin:
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
			{
				SelectObject(mdc, White);//清空缓存dc
				SelectObject(mdc, WhiteBrush);
				Rectangle(mdc, 0, 0, 8000, 80);

				SelectObject(hdc, WhiteBrush);
				if (i == CoverEdit)SelectObject(hdc, BLUE); else SelectObject(hdc, BLACK);//如果当前Edit被选中则用蓝色绘制边框
				Rectangle(hdc, (int)((Edit[i].Left - 5) * DPI), (int)(Edit[i].Top * DPI),
					(int)((Edit[i].Left + Edit[i].Width + 5) * DPI), (int)((Edit[i].Top + Edit[i].Height) * DPI));

				if (*Edit[i].OStr != 0)//如果当前Edit显示的是Ostr(仅用于提示的灰色文字)
				{//在Hdc上直接打印完走人
					SetTextColor(hdc, RGB(150, 150, 150));
					RECT rc = { (long)((Edit[i].Left - 5) * DPI) , (long)(Edit[i].Top * DPI),(long)((Edit[i].Left + Edit[i].Width + 5) * DPI),(long)((Edit[i].Top + Edit[i].Height) * DPI) };
					DrawTextW(hdc, Edit[i].OStr, wcslen(Edit[i].OStr), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					continue;
				}

				SIZE sel, ser;//pos1 / pos2:确定打印时“选中部分”真正的左右两端点
				int pos1, pos2;//因为有时候从左到右移动鼠标选中文字，有时从右向左
				if (Edit[i].Pos1 > Edit[i].Pos2&& Edit[i].Pos2 != -1)pos1 = Edit[i].Pos2, pos2 = Edit[i].Pos1; else pos1 = Edit[i].Pos1, pos2 = Edit[i].Pos2;
				//pos1&pos2里记录的数值只表示选中的先后顺序，不代表左右，因此这里要特殊处理

				if (Edit[i].font != NULL)SelectObject(mdc, Edit[i].font); else SelectObject(mdc, DefFont);//字体
				if (pos2 == -1)
				{//如果没有选中，直接打印+贴图 -> 走人
					TextOutW(mdc, 0, 4, Edit[i].str, wcslen(Edit[i].str));
					goto next;
				}
				//如果选中:较为复杂的情况
				GetTextExtentPoint32(mdc, Edit[i].str, pos1, &sel);//选中条左边字符总长度
				GetTextExtentPoint32(mdc, Edit[i].str, pos2, &ser);//选中条长度+左边字符总长度
				SelectObject(mdc, BLUE);
				SelectObject(mdc, BlueBrush);//用蓝色绘制选中条背景
				Rectangle(mdc, sel.cx, 0, ser.cx, (int)(ser.cy + 5 * DPI));
				SetTextColor(mdc, RGB(0, 0, 0));
				TextOutW(mdc, 0, 4, Edit[i].str, pos1);//黑色打印选中条左边文字
				SetTextColor(mdc, RGB(255, 255, 255));
				TextOutW(mdc, sel.cx, 4, &Edit[i].str[pos1], pos2 - pos1);//白色打印选中条中间文字
				SetTextColor(mdc, RGB(0, 0, 0));
				TextOutW(mdc, ser.cx, 4, &Edit[i].str[pos2], wcslen(&Edit[i].str[pos2]));//黑色打印选中条右边文字

			next:
				int yMax = (int)((Edit[i].Top + Edit[i].Height / 2) * DPI - 4 - Edit[i].strHeight / 2);
				if (yMax < Edit[i].Top* DPI + 1)yMax = (int)(Edit[i].Top * DPI + 1);//贴图
				if (Edit[i].XOffset == 0)
					BitBlt(hdc, (int)((Edit[i].Left + Edit[i].Width / 2)* DPI) - Edit[i].strWidth / 2, yMax//Xoffset为0时居中
						, Edit[i].strWidth
						, Edit[i].strHeight + 4, mdc, 0, 0, SRCCOPY);
				else
					BitBlt(hdc, (int)(Edit[i].Left* DPI), yMax//有Xoffset时直接根据Xoffset贴
						, (int)(Edit[i].Width* DPI)//注意Xoffset不用再乘上DPI
						, Edit[i].strHeight + 4, mdc, Edit[i].XOffset, 0, SRCCOPY);
			}
			if (cur != 0)goto end;
		}
	end:
		DeleteDC(mdc);
	}

	void RedrawObject(int type, int cur)//ObjectRedraw技术的分派函数
	{
		if (type == 1)DrawFrames(cur);
		if (type == 2)DrawButtons(cur);
		if (type == 3)DrawChecks(cur);
		if (type == 4)DrawTexts(cur);
		if (type == 5)DrawEdits(cur);
		DrawExp();//如果type不是1~5就自动DrawExp , 毕竟Exp只能有一个
	}

	//自动绘制所有控件的函数，效率低，不应经常使用
	void DrawEVERYTHING() { DrawFrames(0); DrawButtons(0); DrawChecks(0); DrawLines(); DrawTexts(0); DrawEdits(0); DrawExp(); }
	RECT GetRECT(int cur)//更新Buttons的rc
	{
		RECT rc = { (long)(Button[cur].Left * DPI), (long)(Button[cur].Top * DPI),
			(long)(Button[cur].Left * DPI + Button[cur].Width * DPI),(long)(Button[cur].Top * DPI + Button[cur].Height * DPI) };
		return rc;
	}
	RECT GetRECTf(int cur)//更新Frames的rc
	{
		RECT rc = { (long)(Frame[cur].Left * DPI + 10 * DPI) ,(long)(Frame[cur].Top * DPI - 7 * DPI) ,
		(long)(Frame[cur].Left * DPI + Frame[cur].Width * DPI),(long)(Frame[cur].Top * DPI + 30 * DPI) };
		return rc;
	}
	void SetEditStrOrFont(LPCWSTR Newstr, HFONT font, int cur)//改变制定Edit的字体或文字
	{
		HDC mdc;
		HBITMAP bmp;//创建一个空bmp用来GetTextExtentPoint32
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(mdc, bmp);
		if (font != NULL) Edit[cur].font = font;//先设置font，因为文字宽度和字体有关
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font); else SelectObject(mdc, DefFont);
		if (Newstr != NULL)//改变文字
		{
			if (Edit[cur].str != NULL)if (*Edit[cur].str != NULL)delete[] Edit[cur].str;
			Edit[cur].str = new wchar_t[wcslen(Newstr) + 1];
			wcscpy(Edit[cur].str, Newstr);
		}
		SIZE se;
		GetTextExtentPoint32(mdc, Edit[cur].str, wcslen(Edit[cur].str), &se);
		Edit[cur].strWidth = se.cx; if (se.cy != 0) Edit[cur].strHeight = se.cy;
		if ((int)(Edit[cur].Width* DPI) < se.cx)Edit[cur].XOffset = (int)(se.cx - Edit[cur].Width * DPI) / 2; else Edit[cur].XOffset = 0;
		DeleteDC(mdc);
		DeleteObject(bmp);
	}
	int GetNearestChar(int cur, POINT Point)
	{
		POINT point;
		if (Edit[cur].strWidth <= Edit[cur].Width * DPI)
		{
			if (Point.x < (long)((Edit[cur].Left + Edit[cur].Width / 2)* DPI - Edit[cur].strWidth / 2))return 0;
			if (Point.x > (long)((Edit[cur].Left + Edit[cur].Width / 2)* DPI + Edit[cur].strWidth / 2))return wcslen(Edit[cur].str);
			point.x = Point.x - (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) + Edit[cur].strWidth / 2;
		}
		else
			point.x = (long)(Point.x - Edit[cur].Left * DPI + Edit[cur].XOffset);
		HDC mdc;
		HBITMAP bmp;
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		if (hdc == NULL)s(L"error");
		SelectObject(mdc, bmp);
		ReleaseDC(hWnd, hdc);
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font); else SelectObject(mdc, DefFont);
		int l = wcslen(Edit[cur].str), sum = 0, pos = -1;
		for (int i = 0; i <= l - 1; ++i)
		{
			++pos;
			SIZE se;
			GetTextExtentPoint32(mdc, &Edit[cur].str[i], 1, &se);
			sum += se.cx;
			if (sum >= point.x)  break;
		}
		SIZE sel, ser; //int t;
		GetTextExtentPoint32(mdc, Edit[cur].str, pos, &sel);
		GetTextExtentPoint32(mdc, Edit[cur].str, pos + 1, &ser);

		DeleteDC(mdc);
		DeleteObject(bmp);
		if (point.x < (sel.cx + ser.cx) / 2)return pos; else return pos + 1;
	}
	void EditPaste(int cur)
	{
		if (cur == 0)return;
		if (CoverEdit == 0)return;
		char* buffer = NULL;
		if (OpenClipboard(hWnd))
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			buffer = (char*)GlobalLock(hData);
			GlobalUnlock(hData);
			CloseClipboard();
		}
		int len = strlen(buffer), len2 = wcslen(Edit[cur].str) + 1;
		wchar_t* Buffer = new wchar_t[len + 1], *zxf = new wchar_t[len + len2];
		ZeroMemory(Buffer, sizeof(wchar_t) * len);
		ZeroMemory(zxf, sizeof(wchar_t) * (len + len2));
		if (buffer != NULL)
			MultiByteToWideChar(CP_ACP, 0, buffer, -1, Buffer, sizeof(wchar_t)* len);
		int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2), pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		if (pos1 == -1)
		{
			wchar_t t = Edit[cur].str[pos2];
			Edit[cur].str[pos2] = '\0';
			wcscpy(zxf, Edit[cur].str);
			wcscat(zxf, Buffer);
			Edit[cur].str[pos2] = t;
			wcscat(zxf, &Edit[cur].str[pos2]);
			Edit[cur].Pos1 += wcslen(Buffer);
			SetEditStrOrFont(zxf, 0, cur);
			RefreshXOffset(cur);
			RefreshCaretByPos(cur);
		}
		else
		{
			Edit[cur].str[pos1] = '\0';
			wcscpy(zxf, Edit[cur].str);
			wcscat(zxf, Buffer);
			wcscat(zxf, &Edit[cur].str[pos2]);
			Edit[cur].Pos1 += wcslen(Buffer);
			Edit[cur].Pos2 = -1;
			SetEditStrOrFont(zxf, 0, cur);
			RefreshXOffset(cur);
			RefreshCaretByPos(cur);
		}
		delete[] zxf;
		delete[] Buffer;
		EditRedraw(cur);
	}
	void EditHotKey(int wParam)
	{
		if (CoverEdit == 0)return;
		switch (wParam)
		{
		case 34:EditMove(CoverEdit, -1); break;
		case 35:EditMove(CoverEdit, 1); break;
		case 36:EditPaste(CoverEdit); break;
		case 37:EditCopy(CoverEdit); break;
		case 38:
			if (Edit[CoverEdit].Pos2 != -1)
				EditCopy(CoverEdit),
				EditDelete(CoverEdit, min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2),
					max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2));
			break;
		case 39:EditAll(CoverEdit); break;
		case 40:
			if (wcslen(Edit[CoverEdit].str) == (UINT)Edit[CoverEdit].Pos1)break;
			int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else EditDelete(CoverEdit, pos1, pos1 + 1);
			break;
		}
		return;
	}
	void EditUnHotKey()
	{
		for (int i = 34; i < 41; ++i)UnregisterHotKey(hWnd, i);
		HideCaret(hWnd);
	}
	void EditRegHotKey()
	{
		RegisterHotKey(hWnd, 34, NULL, VK_LEFT);
		RegisterHotKey(hWnd, 35, NULL, VK_RIGHT);
		RegisterHotKey(hWnd, 36, MOD_CONTROL, 'V');
		RegisterHotKey(hWnd, 37, MOD_CONTROL, 'C');
		RegisterHotKey(hWnd, 38, MOD_CONTROL, 'X');
		RegisterHotKey(hWnd, 39, MOD_CONTROL, 'A');
		RegisterHotKey(hWnd, 40, NULL, VK_DELETE);
		DestroyCaret();
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
	}
	void EditCHAR(wchar_t wParam)
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

	void EditAdd(int cur, int Left, int Right, wchar_t Char)
	{
		int len = wcslen(Edit[cur].str) + 5;
		wchar_t* zxf = new wchar_t[len], t = 0;
		if (Left == Right)t = Edit[cur].str[Left];
		Edit[cur].str[Left] = '\0';
		wcscpy(zxf, Edit[cur].str);
		zxf[Left] = Char;
		zxf[Left + 1] = 0;
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

	void EditDelete(int cur, int Left, int Right)
	{
		if (Left == -1)return;
		wchar_t* zxf = new wchar_t[wcslen(Edit[cur].str)];
		Edit[cur].str[Left] = '\0';
		wcscpy(zxf, Edit[cur].str);
		wcscat(zxf, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left;
		Edit[cur].Pos2 = -1;
		SetEditStrOrFont(zxf, 0, cur);
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}

	void EditAll(int cur)
	{
		if (cur == 0)return;
		Edit[cur].Pos1 = 0;
		Edit[cur].Pos2 = wcslen(Edit[cur].str);
		RefreshXOffset(CoverEdit);
		RefreshCaretByPos(CoverEdit);
		EditRedraw(cur);
	}
	void EditMove(int cur, int off)
	{
		int xback;
		xback = Edit[cur].XOffset;
		if (Edit[cur].Pos2 != -1)return;
		Edit[cur].Pos1 += off;
		if (Edit[cur].Pos1 < 0)Edit[cur].Pos1 = 0;
		if ((unsigned int)Edit[cur].Pos1 > wcslen(Edit[cur].str))Edit[cur].Pos1 = wcslen(Edit[cur].str);
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
	}
	void EditCopy(int cur)
	{
		if (cur == 0)return;
		wchar_t* source, t;
		char* Source;
		if (Edit[cur].Pos2 == -1)return;
		int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2);
		int pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		source = new wchar_t[pos2 - pos1 + 1];
		Source = new char[pos2 - pos1 + 1];
		t = Edit[cur].str[pos2];
		Edit[cur].str[pos2] = '\0';
		wcscpy(source, Edit[cur].str + pos1);
		Edit[cur].str[pos2] = t;
		WideCharToMultiByte(CP_ACP, 0, source, -1, Source, 0xffff, NULL, NULL);

		if (OpenClipboard(hWnd))
		{
			HGLOBAL clipbuffer;
			char* buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(Source) + 1);
			buffer = (char*)GlobalLock(clipbuffer);
			if (buffer != NULL)strcpy(buffer, LPCSTR(Source));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
	}
	void RefreshCaretByPos(int cur)
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
			SetCaretPos(se.cx + (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) - Edit[cur].strWidth / 2, (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2));
		else
			SetCaretPos((int)(se.cx + Edit[cur].Left * DPI - Edit[cur].XOffset), (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2));
		ShowCaret(hWnd);
		DeleteDC(mdc);
		DeleteObject(bmp);
	}
	void EditDown(int cur)
	{
		EditRegHotKey();
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CoverEdit != cur)Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1, EditRedraw(CoverEdit);
		CoverEdit = cur;
		if (*Edit[cur].OStr != 0)
		{
			*Edit[cur].OStr = 0;
			ZeroMemory(Edit[cur].str, sizeof(Edit[cur].str));
		}
		Edit[cur].Pos1 = Edit[cur].Pos2 = -1;
		Edit[cur].Press = true;
		Edit[cur].Pos1 = GetNearestChar(cur, point);

		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}

	RECT GetRECTe(int cur)//更新Edit的rc
	{
		RECT rc{ (long)((Edit[cur].Left - 5) * DPI) ,(long)(Edit[cur].Top * DPI),
		(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI),(long)((Edit[cur].Top + Edit[cur].Height) * DPI) };
		return rc;
	}

	RECT GetRECTc(int cur)//更新Check的rc
	{
		RECT rc{ (long)(Check[cur].Left * DPI) ,(long)(Check[cur].Top * DPI) ,
			(long)(Check[cur].Left * DPI + 15 * DPI) ,(long)(Check[cur].Top * DPI + 15 * DPI) };
		return rc;
	}

	BOOL InsideArea(int cur, POINT point)//通过POINT判断是否在指定Area内
	{
		return (Area[cur].Left * DPI <= point.x) && (Area[cur].Top * DPI <= point.y) &&
			((Area[cur].Left + Area[cur].Width) * DPI >= point.x) && ((Area[cur].Top + Area[cur].Height) * DPI >= point.y);
	}

	BOOL InsideEdit(int cur, POINT point)//通过POINT判断是否在指定Edit内
	{
		return ((Edit[cur].Left - 5) * DPI <= point.x && Edit[cur].Top * DPI <= point.y &&
			(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI) >= point.x && (Edit[cur].Top + Edit[cur].Height) * DPI >= point.y);
	}

	void LeftButtonDown()//鼠标左键按下
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);//惯例 获取坐标
		if (CurCover != -1)//当按钮按下 & 停留在按钮上时
		{
			Press = 1;
			RECT rc = GetRECT(CurCover);
			if (Obredraw)Readd(2, CurCover);
			Redraw(&rc);
		}
		if (!InsideEdit(CoverEdit, point) && Edit[CoverEdit].Pos2 == -1)CoverEdit = 0, EditUnHotKey();//Edit没被按下 同理
		if (CoverEdit != 0)//停留在Edit上时
			EditDown(CoverEdit);
		else
			EditUnHotKey();
		Timer = GetTickCount();
		DestroyExp();//任何操作都会导致exp的关闭
	}

	void CheckGetNewInside(POINT & point)//检查point是否在check内
	{
		for (int i = 1; i <= CurCheck; ++i)//遍历所有check
		{
			if (Check[i].Page == CurWnd || Check[i].Page == 0)//不再同一页面 -> 直接跳过
				if (InsideCheck(i, point) != 0)//在check的文字中或方框内
				{
					CoverCheck = i;//设置covercheck
					if (Obredraw)Readd(3, i);
					RECT rc = GetRECTc(i);//重绘
					Redraw(&rc);
					break;
				}
		}
	}
	void ButtonGetNewInside(POINT & point)//检查point是否在check内
	{
		for (int i = 0; i <= CurButton; ++i)//历史原因，Button编号是从0开始的
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled)
				if (InsideButton(i, point))//在按钮中
				{
					CurCover = i;//设置curcover
					if (ButtonEffect)//特效开启
					{
						Button[i].Percent += 40;//先给40%的颜色 （太淡了看不出来）
						if (Button[i].Percent > 100)Button[i].Percent = 100;
					}
					if (Obredraw)Readd(2, i);
					RECT rc = GetRECT(i);//重绘
					Redraw(&rc);
					return;
				}
	}
	void AreaGetNewInside(POINT & point)//Area 同理
	{
		for (int i = 1; i <= CurArea; ++i)
			if (Area[i].Page == CurWnd || Area[i].Page == 0)
				if (InsideArea(i, point))CoverArea = i;
	}
	void EditGetNewInside(POINT & point) //Edit 同理
	{
		for (int i = 1; i <= CurEdit; ++i)//Edit 同理
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
				if (InsideEdit(i, point))CoverEdit = i, EditRegHotKey();
	}
	void MouseMove()//鼠标移动
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CurCover == -1)ButtonGetNewInside(point);//原来不在按钮内 -> 看看现在是否移进按钮
		else//原来在
		{
			if (!Button[CurCover].Enabled) { CurCover = -1; goto disabled; }//这个按钮被禁用了
			if ((Button[CurCover].Page != CurWnd && Button[CurCover].Page != 0) || !InsideButton(CurCover, point))
			{//现在不在
				if (Obredraw)Readd(2, CurCover);
				if (ButtonEffect)
				{//curcover设为-1 , 重绘
					Button[CurCover].Percent -= Delta;
					if (Button[CurCover].Percent < 0)Button[CurCover].Percent = 0;
				}
				RECT rc = GetRECT(CurCover);
				CurCover = -1;
				Redraw(&rc);
				ButtonGetNewInside(point);//有可能从一个按钮直接移进另一个按钮内
			}
		}
	disabled:
		if (CoverCheck == 0)CheckGetNewInside(point);//在外面 -> 寻找新check
		else
		{//同理
			if ((Check[CoverCheck].Page != CurWnd && Check[CoverCheck].Page != 0) || InsideCheck(CoverCheck, point) == 0)
			{
				if (Obredraw)Readd(3, CoverCheck);
				RECT rc = GetRECTc(CoverCheck);
				CoverCheck = 0;
				Redraw(&rc);
				CheckGetNewInside(point);
			}
		}
		if (CoverEdit == 0)EditGetNewInside(point);
		else
		{
			if (Edit[CoverEdit].Press == true)
			{//如果Edit被按下 (拖动选择条)
				int t = Edit[CoverEdit].Pos2;
				Edit[CoverEdit].Pos2 = GetNearestChar(CoverEdit, point);//寻找和鼠标指针最近的字符
				RefreshCaretByPos(CoverEdit);//移动Caret(闪烁的光标)
				if (Edit[CoverEdit].Pos2 == Edit[CoverEdit].Pos1) { Edit[CoverEdit].Pos2 = -1; goto end; }//只选择了一个字符
				if (Edit[CoverEdit].Pos2 != t && Edit[CoverEdit].Width < Edit[CoverEdit].strWidth && !InsideEdit(CoverEdit, point))RefreshXOffset(CoverEdit);//Edit中文本过长，移动到了框外面
				if (Edit[CoverEdit].Pos2 != t)EditRedraw(CoverEdit);//只要和原来有任何不同就重绘
			}
			if (!InsideEdit(CoverEdit, point) && Edit[CoverEdit].Pos2 == -1)CoverEdit = 0, EditUnHotKey();//Edit没被按下 同理
		}
	end:
		if (CoverArea == 0)
			AreaGetNewInside(point);
		else
			if (!InsideArea(CoverArea, point))CoverArea = 0;

		if (Msv == 0)
		{//检测鼠标移进移出的代码
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hWnd;
			tme.dwFlags = TME_LEAVE;
			TrackMouseEvent(&tme);
			Msv = 1;//移出
		}
		else Msv = 0;//移进
		if (point.x != ExpPoint2.x || point.y != ExpPoint2.y)
		{
			ExpPoint2 = point;
			Timer = GetTickCount();
			DestroyExp();
		}
	}
	void EditRedraw(int cur)//重绘Edit的外壳函数
	{
		if (Obredraw)Readd(5, cur);
		RECT rc = GetRECTe(cur);
		Redraw(&rc);//标准ObjectRedraw写法
	}
	void InfoBox(LPCWSTR Str)//全自动的MessageBox
	{
		bool f = (bool)GetStr(Str);
		if (!slient)
			if (f)MessageBox(hWnd, GetStr(Str), GetStr(L"Info"), 0x40L); else MessageBox(hWnd, Str, GetStr(L"Info"), 0x40L);
		else if (f)printf("%ls\n", GetStr(Str)); else printf("%ls\n", Str);
	}
	void RefreshXOffset(int cur)
	{
		if (Edit[cur].strWidth < Edit[cur].Width* DPI) {
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
		if (se.cx > (long)(Edit[cur].XOffset + Edit[cur].Width * DPI))Edit[cur].XOffset += (int)(se.cx - (Edit[cur].XOffset + Edit[cur].Width * DPI) - 1);
		if (Edit[cur].strWidth - Edit[cur].XOffset < Edit[cur].Width * DPI)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
		DeleteDC(mdc);
		DeleteObject(bmp);
	}

	int GetNumByIDe(LPCWSTR ID)//通过Edit的ID获取其编号
	{
		for (int i = 1; i <= CurEdit; ++i)if (wcscmp(ID, Edit[i].ID) == 0)return i;
		return 0;
	}

	void SetPage(int Page)
	{
		if (Page == CurWnd)return;
		HideCaret(hWnd);
		Edit[CoverEdit].Press = false;
		Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1;
		CoverEdit = 0;
		CurWnd = Page;
		while (!rs.empty())rs.pop();
		while (!es.empty())es.pop();
		Redraw(NULL);
	}
	void SetDPI(DOUBLE NewDPI)
	{
		DPI = NewDPI;
		DefFont = CreateFontW((int)(16 * DPI), (int)(8 * DPI), 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
		for (int i = 1; i <= CurEdit; ++i)SetEditStrOrFont(0, 0, i), RefreshXOffset(i);
		RefreshCaretByPos(CoverEdit);
		SetWindowPos(hWnd, NULL, 0, 0, (int)(Width * DPI), (int)(Height * DPI), SWP_NOMOVE | SWP_NOREDRAW);
		DestroyCaret();
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));

		while (!rs.empty())rs.pop();
		Redraw(NULL);
	}

	LPWSTR GetCurInsideID()
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		for (int i = 0; i <= CurButton; ++i)
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled)
				if (InsideButton(i, point) && Button[i].DownTot == 0)
					return Button[i].ID;
		return Button[0].ID;
	}
	inline int GetNumbyID(LPCWSTR ID) { return but[Hash(ID)]; }

	void SetHDC(HDC HDc)
	{
		hdc = HDc;
		if (bitmap != NULL)DeleteObject(bitmap);
		bitmap = CreateCompatibleBitmap(hdc, 8000, 80);
	}
	void Try2CreateExp()
	{
		if (ExpExist == true || CurCover == -1)return;
		if (wcslen(Button[CurCover].Exp) == 0)return;
		CurButtonBack = CurButton;
		ExpExist = true;
		ExpLine = 0;
		ZeroMemory(Exp, sizeof(Exp));

		wchar_t* x = Button[CurCover].Exp, *y = Button[CurCover].Exp;
		ExpHeight = 8;
		ExpWidth = 0;
		while (1)
		{
			ExpLine++;
			x = wcsstr(x + 1, L"\\n");
			if (x != 0)x[0] = '\0';
			if (ExpLine == 1)wcscpy(Exp[ExpLine], y); else wcscpy(Exp[ExpLine], y + 2);
			SIZE * se = new SIZE;
			if (ExpLine == 1)GetTextExtentPoint32(hdc, y, wcslen(y), se); else GetTextExtentPoint32(hdc, y + 2, wcslen(y + 2), se);//获取字符串的宽度
			if (x != 0)x[0] = '\\';
			ExpHeight += se->cy;
			ExpWidth = max(ExpWidth - 8, se->cx) + 8;
			if (x == 0)break;
			y = x;
		}
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		ExpPoint = point;
		Readd(6, 1);
		Redraw(NULL);
	}
	void DestroyExp()
	{
		if (ExpExist == false)return;
		ExpExist = false;
		ExpLine = ExpHeight = ExpWidth = 0;
		Redraw(NULL);
	}
	FORCEINLINE void Erase(RECT & rc) { es.push(rc); }
	void Redraw(const RECT * rc) { InvalidateRect(hWnd, rc, FALSE); UpdateWindow(hWnd); }
	void Readd(int type, int cur) { rs.push(std::make_pair(type, cur)); }
	int ExpLine, ExpHeight, ExpWidth;
	wchar_t Exp[MAX_EXPLINES][81];
	POINT ExpPoint, ExpPoint2;
	struct ButtonEx//按钮
	{
		long Left, Top, Width, Height, Page, Download, Percent, DownCur, DownTot;
		bool Visible, Enabled, Border = true;
		HBRUSH Leave, Hover, Press;
		HPEN Leave2, Hover2, Press2;
		HFONT Font;
		wchar_t Name[31], ID[11], Exp[MAX_EXPLENGTH];
		COLORREF FontRGB;
		BYTE b1[3], b2[3], p1[3], p2[3];
	}Button[MAX_BUTTON];
	struct FrameEx//控件框结构体
	{
		int Left, Top, Width, Height, Page;
		COLORREF rgb;//自定义颜色
		wchar_t Name[51];
	}Frame[MAX_FRAME];//现在为了节约内存空间都用MAX_XXX了 ， 具体可以到GUI.h里改
	struct CheckEx//选择框结构体
	{
		int Left, Top, Page, Width;//width跟绘制无关，用来检测是否按下
		bool Value;
		wchar_t Name[51];
	}Check[MAX_CHECK];
	struct LineEx//线段
	{
		int StartX, StartY, EndX, EndY, Page;//线段的起始坐标和终点坐标
		COLORREF Color;
	}Line[MAX_LINE];
	struct TextEx//文字
	{
		int Left, Top, Page;
		COLORREF rgb;
		wchar_t Name[11];//这里的"Name"其实是GUIString的ID
	}Text[MAX_TEXT];
	struct EditEx//输入框
	{
		int Left, Top, Width, Height, Page;
		int strWidth, strHeight, Pos1, Pos2, XOffset;
		bool Press;
		wchar_t* str, ID[11], OStr[21];
		HFONT font;
	}Edit[MAX_EDIT];
	struct AreaEx//点击区域
	{
		int Left, Top, Width, Height, Page;
	}Area[MAX_AREA];
	struct GUIString//GUI工程专用带ID标签的字符串
	{
		wchar_t* str, ID[11];
	}string[MAX_STRING];
	std::map<unsigned int, wchar_t*> str;
	std::map<unsigned int, int>but;
	HFONT DefFont;
	int Msv;//鼠标移出检测变量
	int CurString, CurButton, CurFrame, CurCheck, CurLine, CurText, CurEdit, CurArea;
	double DPI = 1;
	int CurCover, CoverCheck, CoverEdit, CoverArea;
	bool Obredraw = false;
	bool ButtonEffect = false;
	int CurWnd;
	int Press;
	std::stack<std::pair<int, int>>rs;
	std::stack<RECT>es;
	HDC hdc;//缓存dc
	HDC tdc;//真实dc
	HBITMAP bitmap;//Edit专用缓存bitmap
	HBITMAP Bitmap;//缓存窗口bitmap
	int Width, Height;
	HWND hWnd;
	HINSTANCE hInstance;
	DWORD Timer;
	BOOL ShowExp = FALSE;
	int CurButtonBack;
	bool ExpExist = false;
private://没有任何private变量或函数= =
}Main, CatchWnd, UpWnd;
#pragma warning(disable:4100)//禁用警告
class DownloadProgress : public IBindStatusCallback {
public://这些函数有些参数没有用到，会导致大量警告.
	wchar_t curi[11];
	HRESULT __stdcall QueryInterface(const IID&, void**) { return E_NOINTERFACE; }
	ULONG STDMETHODCALLTYPE AddRef() { return 1; }//暂时没用的函数，从msdn上抄下来的
	ULONG STDMETHODCALLTYPE Release() { return 1; }
	HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, IBinding* pib) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG* pnPriority) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, LPCWSTR szError) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD* grfBINDF, BINDINFO* pbindinfo) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pformatetc, STGMEDIUM* pstgmed) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID riid, IUnknown* punk) { return E_NOTIMPL; }
	virtual HRESULT __stdcall OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		if (ulProgressMax != 0)//ulProgress：已下载的字节数
		{//ulProgressMax：总字节数
			double percentage = double(ulProgress * 1.0 / ulProgressMax * 100);//计算下载百分比
			if (Main.Button[Main.GetNumbyID(curi)].Download != (int)percentage + 1)
			{
				Main.Button[Main.GetNumbyID(curi)].Download = (int)percentage + 1;
				RECT rc = Main.GetRECT(Main.GetNumbyID(curi));
				Main.Readd(2, Main.GetNumbyID(curi));//重绘
				Main.Redraw(&rc);
			}
		}
		return S_OK;
	}//同样没有private..
};
#pragma warning(default:4100)//恢复警告

BOOL CALLBACK EnumTDwnd(HWND hwnd, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID;
	LONG A;//遍历所有窗口
	A = GetWindowLongW(hwnd, GWL_STYLE) & WS_VISIBLE;
	if (A != 0 && GetParent(hwnd) == NULL)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);//如果pid正确，把hwnd记录下来
		for (unsigned int i = 1; i <= tdpid[0]; ++i)if (tdpid[i] == nProcessID) { tdh[++tdhcur] = hwnd; }
	}
	return 1;
}

void FindTDhwnd(wchar_t* name)//查找被监视的窗口
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
		_wcslwr(pe.szExeFile);//记录下符合要求的pid
		if (wcsstr(pe.szExeFile, name) != 0)tdpid[++Cur] = pe.th32ProcessID;
	}
	tdpid[0] = Cur;//数量存在tdpid[0]里
	EnumWindows(EnumTDwnd, NULL);
}

DWORD WINAPI TopThread(LPVOID pM)//置顶线程
{
	UNREFERENCED_PARAMETER(pM);
	while (TOP == 1)
	{//连续置顶	(然而比不过任务管理器，人家有CreateWindowWithBand)
		SetWindowPos(Main.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
		if (CatchWnd.hWnd != NULL)SetWindowPos(CatchWnd.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
	}
	if (CatchWnd.hWnd != NULL)SetWindowPos(CatchWnd.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);//取消置顶
	SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	return 0;
}

BOOL CALLBACK EnumTopWnd(HWND hwnd, LPARAM lParam)
{//枚举置顶窗口
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID; HANDLE hProcessHandle = 0;
	LONG A;
	A = GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;
	if (A != 0)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		if (GetCurrentProcessId() == nProcessID)goto ok;//如果是被保护的进程ID -> 跳过
		for (unsigned int i = 1; i <= expid[0]; ++i)if (expid[i] == nProcessID)goto ok;//用unsigned int 以避免"有符号/无符号不匹配"
		MyOpenProcess(&hProcessHandle, nProcessID);
		MyTerminateProcess(hProcessHandle);
	}
ok:
	return 1;
}
void KillTop()//杀掉置顶窗口
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
	EnumWindows(EnumTopWnd, NULL);
}

BOOL KillProcess(LPCWSTR ProcessName)//结束进程
{
	if (Main.Check[14].Value)//如果开启ntsd & processhacker 结束进程
	{
		bool fileexist = false;
		wchar_t tmp[501];
		wcscpy_s(tmp, L"ntsd.exe -c q -pn ");
		wcscat(tmp, ProcessName);
		if(RunEXE(tmp, CREATE_NO_WINDOW, nullptr))fileexist=true;
		if (Bit != 32 && GetFileAttributes(L"ProcessHacker\\ProcessHackerx64.exe") != INVALID_FILE_ATTRIBUTES)
			wcscpy_s(tmp, L"ProcessHacker\\ProcessHackerx64.exe -n ");
		else
			wcscpy_s(tmp, L"ProcessHacker\\ProcessHacker.exe -n ");
		wcscat(tmp, ProcessName);
		if (RunEXE(tmp, CREATE_NO_WINDOW, nullptr))fileexist = true;
		if (fileexist == false)Main.Check[14].Value = false,Main.InfoBox(L"NPFail"),Main.Redraw(NULL);
	}
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;//创建进程快照
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = 0;//根据进程名前三个字符标识
		_wcslwr_s(pe.szExeFile);
		if (wcscmp(ProcessName, pe.szExeFile) == 0 || ProcessName == NULL)
		{
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = 0;
			MyOpenProcess(&hProcess, dwProcessID);//使用NtOpenProcess和NtTerminateProcess
			MyTerminateProcess(hProcess);//应该不会比普通的Open + Terminate更强
			CloseHandle(hProcess);
		}
	}
	return TRUE;
}

void CheckIP()//取本机的ip地址  
{
	WSADATA wsaData;
	char name[155];
	char* ip;
	PHOSTENT hostinfo;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
	{//具体原理不太清楚，详见百科
		if (gethostname(name, sizeof(name)) == 0)
			if ((hostinfo = gethostbyname(name)) != NULL)
			{//wip存ip地址字符串
				ip = inet_ntoa(*(struct in_addr*) * hostinfo->h_addr_list);
				charTowchar(ip, wip, sizeof(wip) * 2);
			}
		WSACleanup();
	}
}

bool GetTDVer(wchar_t* source)//获取极域版本
{//返回值复制到source里
	if (source == NULL)return false;//连source都没有当然直接退出
	wcscpy(source, Main.GetStr(L"_TTDv"));//在前面加上"极域版本："之类的字符串
	HKEY hKey;
	LONG ret;//标准的注册表操作
	wchar_t szLocation[100] = { 0 };
	DWORD dwSize = sizeof(wchar_t) * 100;
	DWORD dwType = REG_SZ;
	if (Bit != 64)//分32位和64位讨论
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-Learning Class V6.0", 0, KEY_READ, &hKey);
	else//然而现在极域好像也有64位了怎么办
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-Learning Class V6.0", 0, KEY_READ, &hKey);

	if (ret != 0)//打开失败
	{
		if (Bit != 64)//再看看上级目录存不存在
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain", 0, KEY_READ, &hKey);
		else//(因为很旧版的极域没有v6.0这个东西，完全无法获取版本号)
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain", 0, KEY_READ, &hKey);
		if (ret != 0)//连TopDomain目录都没有，极域可能不存在
			wcscat(source, Main.GetStr(L"TTDunk"));
		else//极域版本为2007之类，给个模棱两可的答案
			wcscat(source, Main.GetStr(L"TTDold"));
		RegCloseKey(hKey);
		return true;
	}//打开成功，至少有这个目录
	ret = RegQueryValueExW(hKey, L"Version", 0, &dwType, (LPBYTE)& szLocation, &dwSize);
	if (ret != 0)return false;//打开成功却没有键值？
	if (*szLocation != 0)//一切正常
		wcscat(source, szLocation);
	else return false;
	RegCloseKey(hKey);
	return true;
}
void UpdateInfo()//修改"关于"界面信息的函数
{
	wchar_t tmp[301] = { 0 }, tmp2[1001] = { 0 }; int f;
	wcscpy_s(tmp2, Main.GetStr(L"Tbit"));//系统位数
	if (Bit == 32)wcscat_s(tmp2, L"32"); else wcscat_s(tmp2, L"64");
	Main.SetStr(tmp2, L"Tbit");
	GetOSDisplayString(tmp);//系统版本
	wcscpy_s(tmp2, Main.GetStr(L"Twinver")); wcscat(tmp2, tmp);
	Main.SetStr(tmp2, L"Twinver");
	if (Bit == 34)f = GetFileAttributes(L"C:\\windows\\sysnative\\cmd.exe"); else f = GetFileAttributes(L"C:\\windows\\system32\\cmd.exe");
	wcscpy_s(tmp2, Main.GetStr(L"Tcmd"));//是否有cmd
	if (f != -1)wcscat(tmp2, Main.GetStr(L"TcmdOK")); else wcscat(tmp2, Main.GetStr(L"TcmdNO"));
	Main.SetStr(tmp2, L"Tcmd");
	CheckIP();//ip地址
	wcscpy_s(tmp2, Main.GetStr(L"TIP")); wcscat(tmp2, wip);
	Main.SetStr(tmp2, L"TIP");//极域版本
	if (GetTDVer(tmp2))Main.SetStr(tmp2, L"TTDv");
}
void SetFrame()//根据管理员改变Frame上的文字
{
	Main.Frame[8].rgb = RGB(255, 180, 10);
	if (!Admin)
	{
		const int t[] = { 1,7,9 }, ok[] = { 2,4 }, rec[] = { 3 }, nrec[] = { 5 };
		for (int i = 0; i < 3; ++i)Main.Frame[t[i]].rgb = RGB(255, 0, 0), wcscat_s(Main.Frame[t[i]].Name, Main.GetStr(L"Useless"));
		for (int i = 0; i < 2; ++i)Main.Frame[ok[i]].rgb = RGB(5, 200, 135), wcscat_s(Main.Frame[ok[i]].Name, Main.GetStr(L"Usable"));
		for (int i = 0; i < 1; ++i)Main.Frame[rec[i]].rgb = RGB(10, 255, 10), wcscat_s(Main.Frame[rec[i]].Name, Main.GetStr(L"Rec"));
		for (int i = 0; i < 1; ++i)Main.Frame[nrec[i]].rgb = RGB(0x63, 0xB8, 0xFF), wcscat_s(Main.Frame[nrec[i]].Name, Main.GetStr(L"nRec"));
	}
	else
	{
		const int  ok[] = { 3,4 }, rec[] = { 1 }, nrec[] = { 2,5 };
		for (int i = 0; i < 2; ++i)Main.Frame[ok[i]].rgb = RGB(5, 200, 135), wcscat_s(Main.Frame[ok[i]].Name, Main.GetStr(L"Usable"));
		for (int i = 0; i < 1; ++i)Main.Frame[rec[i]].rgb = RGB(10, 255, 10), wcscat_s(Main.Frame[rec[i]].Name, Main.GetStr(L"Rec"));
		for (int i = 0; i < 2; ++i)Main.Frame[nrec[i]].rgb = RGB(0x63, 0xB8, 0xFF), wcscat_s(Main.Frame[nrec[i]].Name, Main.GetStr(L"nRec"));
	}
}
void GetPath()//得到两个路径
{
	GetModuleFileName(NULL, Path, MAX_PATH);
	wcscpy_s(Name, Path);
	(_tcsrchr(Path, _T('\\')))[1] = 0;
}
void GetInfo(__out LPSYSTEM_INFO lpSystemInfo)//得到环境变量
{
	if (NULL == lpSystemInfo)return;
	typedef void(WINAPI * LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	HMODULE H = GetModuleHandle(L"Kernel32");
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = 0;
	if (H != NULL)fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(H, "GetNativeSystemInfo");
	if (fnGetNativeSystemInfo != NULL)fnGetNativeSystemInfo(lpSystemInfo); else GetSystemInfo(lpSystemInfo);
}
void GetBit()//系统位数
{
	SYSTEM_INFO si;//据说是有效的检测系统位数的方法
	GetInfo(&si);//AMD64 \ Intel64 ?
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) Bit = 64; else Bit = 32;
	//32 means 32bit GUI on 32bit System
	//34 means 32bit GUI on 64bit System
	//64 means 64bit GUI on 64bit System
	if (sizeof(int*) * 8 == 32 && Bit == 64)Bit = 34;
}
BOOL Backup()//备份sethc.exe
{
	if (GetFileAttributes(SethcPath) == INVALID_FILE_ATTRIBUTES) { SethcState = false; return false; }//如果sethc本来就不存在 -> 退出
	if (GetFileAttributes(L"C:\\SAtemp") == FILE_ATTRIBUTE_DIRECTORY)return FALSE;//SAtemp目录已经存在 -> sethc可能已经备份过 -> 退出
	CreateDirectory(L"C:\\SAtemp", NULL);
	CopyFile(SethcPath, L"C:\\SAtemp\\sethc.exe", TRUE);
	return TRUE;
}
struct GETLAN
{
	int Left, Top, Width, Height;
	wchar_t* begin, * str1, * str2;
};

void GetLanguage(GETLAN & a)
{
	__try
	{
		wchar_t* str1 = wcsstr(a.begin, L"\"");
		wchar_t* str2 = wcsstr(str1 + 1, L"\"");
		*str2 = '\0';
		a.str1 = str1 + 1;
		str1 = wcsstr(str2 + 1, L",");
		if (str1 == NULL)return;
		str2 = wcsstr(str1 + 1, L",");
		if (str2 != NULL)* str2 = '\0';
		a.Left = _wtoi(str1 + 1);
		if (str2 == NULL)return;
		str1 = wcsstr(str2 + 1, L",");
		if (str1 != NULL)* str1 = '\0';
		a.Top = _wtoi(str2 + 1);
		if (str1 == NULL)return;
		str2 = wcsstr(str1 + 1, L",");
		if (str2 != NULL)* str2 = '\0';
		a.Width = _wtoi(str1 + 1);
		if (str2 == NULL)return;
		str1 = wcsstr(str2 + 1, L",");
		if (str1 != NULL)* str1 = '\0';
		a.Height = _wtoi(str2 + 1);
		if (str1 == 0)return;
		str1 = wcsstr(str1 + 1, L"\"");
		str2 = wcsstr(str1 + 1, L"\"");
		*str2 = '\0';
		a.str2 = str1 + 1;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		s(L"error");
	}
}

void DispatchLanguage(LPWSTR ReadTmp, int type, CathyClass * a)
{
	__try
	{
		GETLAN gl = { 0 };
		wchar_t* pos = wcsstr(ReadTmp, L"=");
		*pos = '\0'; gl.begin = pos + 1;
		wchar_t* space = wcsstr(ReadTmp, L" ");
		if (space != 0)space[0] = '\0';
		if (type == 1)//button
		{
			int cur = a->GetNumbyID(ReadTmp);
			GetLanguage(gl);

			if (gl.Left != -1)a->Button[cur].Left = gl.Left;
			if (gl.Top != -1)a->Button[cur].Top = gl.Top;
			if (gl.Width != -1)a->Button[cur].Width = gl.Width;
			if (gl.Height != -1)a->Button[cur].Height = gl.Height;
			if (gl.str1 != NULL)wcscpy_s(a->Button[cur].Name, gl.str1);
			if (gl.str2 != NULL)wcscpy_s(a->Button[cur].Exp, gl.str2); else ZeroMemory(a->Button[cur].Exp, sizeof(a->Button[cur].Exp));
			return;
		}
		if (type == 2)//check
		{
			int cur = _wtoi(ReadTmp + 1);
			GetLanguage(gl);
			if (gl.Left != -1)a->Check[cur].Left = gl.Left;
			if (gl.Top != -1)a->Check[cur].Top = gl.Top;
			if (gl.Width != -1)a->Check[cur].Width = gl.Width;
			if (gl.str1 != NULL)wcscpy_s(a->Check[cur].Name, gl.str1);
			return;
		}
		if (type == 3)//string
		{
			wchar_t tmp[301]; bool f = false;
			ZeroMemory(tmp, sizeof(tmp));
			wchar_t* str1 = wcsstr(pos + 1, L"\""), * str2, * str3;
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
			a->SetStr(tmp, ReadTmp);
			return;
		}
		if (type == 4)//frame
		{
			int cur = _wtoi(ReadTmp + 1);
			GetLanguage(gl);
			if (gl.Left != -1)a->Frame[cur].Left = gl.Left;
			if (gl.Top != -1)a->Frame[cur].Top = gl.Top;
			if (gl.Width != -1)a->Frame[cur].Width = gl.Width;
			if (gl.Height != -1)a->Frame[cur].Height = gl.Height;
			if (gl.str1 != NULL)wcscpy_s(a->Frame[cur].Name, gl.str1);
			return;
		}
		if (type == 5)//text
		{
			int cur = _wtoi(ReadTmp + 1);
			wchar_t* str = wcsstr(pos + 1, L",");
			*str = '\0';
			int NewLeft = _wtoi(pos + 1);
			int NewTop = _wtoi(str + 1);
			if (NewLeft != -1)a->Text[cur].Left = NewLeft;
			if (NewTop != -1)a->Text[cur].Top = NewTop;
			return;
		}
		if (type == 6)//edit
		{
			int cur = a->GetNumByIDe(ReadTmp);
			GetLanguage(gl);
			if (gl.Left != -1)a->Edit[cur].Left = gl.Left;
			if (gl.Top != -1)a->Edit[cur].Top = gl.Top;
			if (gl.Width != -1)a->Edit[cur].Width = gl.Width;
			if (gl.Height != -1)a->Edit[cur].Height = gl.Height;
			if (gl.str1 != NULL)wcscpy_s(a->Edit[cur].OStr, gl.str1);
			return;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { s(L"error"); }
}
void SwitchLanguage(LPWSTR name)
{
	__try {
		bool Mainf = false, Catchf = false, Upf = false;
		int type = 0;
		wchar_t ReadTmp[1001];
		FILE* fp;

		_wfopen_s(&fp, name, L"r,ccs=UNICODE");
		if (fp == NULL)Main.InfoBox(L"StartFail");
		while (!feof(fp))
		{
			fgetws(ReadTmp, 1000, fp);
			if (wcsstr(ReadTmp, L"<Main>") != 0)Mainf = true;
			if (wcsstr(ReadTmp, L"</Main>") != 0)Mainf = false;
			if (wcsstr(ReadTmp, L"<Catch>") != 0)Catchf = true;
			if (wcsstr(ReadTmp, L"</Catch>") != 0)Catchf = false;
			if (wcsstr(ReadTmp, L"<Up>") != 0)Upf = true;
			if (wcsstr(ReadTmp, L"</Up>") != 0)Upf = false;
			if (wcsstr(ReadTmp, L"<Buttons>") != 0)type = 1;
			if (wcsstr(ReadTmp, L"<Checks>") != 0)type = 2;
			if (wcsstr(ReadTmp, L"<Strings>") != 0)type = 3;
			if (wcsstr(ReadTmp, L"<Frames>") != 0)type = 4;
			if (wcsstr(ReadTmp, L"<Texts>") != 0)type = 5;
			if (wcsstr(ReadTmp, L"<Edits>") != 0)type = 6;
			if (ReadTmp[0] != '<')
			{
				if (Mainf)DispatchLanguage(ReadTmp, type, &Main);
				if (Catchf)DispatchLanguage(ReadTmp, type, &CatchWnd);
				if (Upf)DispatchLanguage(ReadTmp, type, &UpWnd);
			}
		}
		fclose(fp);
		UpdateInfo();
		SetWindowText(Main.hWnd, Main.GetStr(L"Title"));
		SetWindowText(CatchWnd.hWnd, CatchWnd.GetStr(L"Title"));
		SetWindowText(UpWnd.hWnd, UpWnd.GetStr(L"Title"));
		haveInfo = true;
		SetFrame();
		Main.Redraw(NULL);
		if (CatchWnd.hWnd)CatchWnd.Redraw(NULL);
		if (UpWnd.hWnd)UpWnd.Redraw(NULL);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { s(L"error"); }
}
int SetupSethc()//安装sethc
{
	wchar_t tmp[301];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, L"sethc.exe");
	if (GetFileAttributes(tmp) == -1)ReleaseRes(tmp,IDR_JPG5,L"JPG");
	if (GetFileAttributes(tmp) == -1)return 2;//文件不存在
	return CopyFile(tmp, SethcPath, false);//复制成功 \ 失败
}

bool AutoSetupSethc()//安装sethc的外壳函数
{
	int flag = SetupSethc();
	if (flag == 0) { Main.InfoBox(L"CSFail"); return false; }
	if (flag == 2) { Main.InfoBox(L"NoSethc"); return false; }
	return true;
}
bool DeleteSethc()//删除sethc
{
	if (GetFileAttributes(SethcPath) == -1)return true;//sethc已经没了 -> 成功
	TakeOwner(SethcPath);
	wchar_t tmp[] = L"C:\\Windows\\system32\\dllcache\\sethc.exe";//删除xp中sethc备份文件
	TakeOwner(tmp);
	DeleteFile(tmp);
	return DeleteFile(SethcPath);
}

bool DeleteSethcS()//用驱动删除sethc.exe
{
	bool flag1 = false, flag2 = false;
	wchar_t p1[34], p2[34];
	if (DeleteSethc())return true;//先尝试应用层删除
	if (Bit == 32)
		wcscpy_s(p1, L"x32\\deletefile.sys"),//32位和64位选择不同驱动
		wcscpy_s(p2, L"x32\\kill.sys");
	else
		wcscpy_s(p1, L"x64\\deletefile.sys"),
		wcscpy_s(p2, L"x64\\kill.sys");
	UnloadNTDriver(L"deletefile");

	flag1 = LoadNTDriver(L"deletefile", p1);
	UnloadNTDriver(L"kill");//加载deletefile和kill两个驱动
	flag2 = LoadNTDriver(L"kill", p2);
	WinExec("net stop kill", SW_HIDE);
	UnloadNTDriver(L"deletefile");
	return flag1 & flag2;
}
int SetupSethcS()
{
	wchar_t tmp[301];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, L"x32\\sethc.exe");
	if (GetFileAttributes(tmp) == -1)return 2;//没有驱动sethc
	return CopyFile(tmp, SethcPath, false);//返回是否成功
}
int CopyNTSD()
{
	int f=0;
	if (Bit != 34)
		f = GetFileAttributes(L"C:\\Windows\\System32\\ntsd.exe");
	else
		f = GetFileAttributes(L"C:\\Windows\\SysNative\\ntsd.exe");
	if (f != -1)return true;
	wchar_t tmp[301];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, L"ntsd.exe");
	s(tmp);
	if (GetFileAttributes(tmp) == -1)ReleaseRes(tmp, IDR_JPG4, L"JPG");
	if (GetFileAttributes(tmp) == -1)return 2;//文件不存在

	if (Bit != 34)
		return CopyFile(tmp, L"C:\\Windows\\System32\\ntsd.exe", false);
	else
		return CopyFile(tmp, _T("C:\\Windows\\SysNative\\ntsd.exe"), false);
}
bool AutoCopyNTSD()
{
	int flag = CopyNTSD();
	if (flag == 0) { Main.InfoBox(L"CNTSDFail"); return false; }
	if (flag == 2) { Main.InfoBox(L"NoNTSD"); return false; }
	return true;
}

void SearchTool(LPCWSTR lpPath, int type)//1 打开极域 2 删除shutdown 3 删除文件夹
{
	wchar_t szFind[255], szFile[255];
	WIN32_FIND_DATA FindFileData;
	wcscpy_s(szFind, lpPath);
	wcscat_s(szFind, L"\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) return;
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
			_wcslwr_s(FindFileData.cFileName);
			if (type == 1)
				if (wcsstr(FindFileData.cFileName, L"studentmain.exe") != NULL)
				{
					wcscpy_s(szFile, lpPath);
					wcscat_s(szFile, L"\\");
					wcscat_s(szFile, FindFileData.cFileName);
					RunEXE(szFile, NULL, nullptr);
				}
			if (type == 2)
				if (wcsstr(FindFileData.cFileName, L"shutdown.exe") != NULL)
				{
					wcscpy_s(szFile, lpPath);
					wcscat_s(szFile, L"\\");
					wcscat_s(szFile, FindFileData.cFileName);
					TakeOwner(szFile);
					DeleteFile(szFile);
				}
			if (type == 3)
			{
				wcscpy_s(szFile, lpPath);
				wcscat_s(szFile, L"\\");
				wcscat_s(szFile, FindFileData.cFileName);
				TakeOwner(szFile);
				DeleteFile(szFile);
				RemoveDirectory(lpPath);
			}
		}
		if (!FindNextFile(hFind, &FindFileData))break;
	}
	FindClose(hFind);
}

void AutoDelete(wchar_t* tmp)//自动删除文件
{
	int FileType;
	FileType = GetFileAttributes(tmp);
	if (FileType == -1) { Main.InfoBox(L"TINotF"); return; }//不是文件也不是文件夹
	TakeOwner(tmp);

	if (FileType& FILE_ATTRIBUTE_DIRECTORY)
		SearchTool(tmp, 3);
	else
		DeleteFile(tmp);
}

bool UninstallSethc()//恢复原来的sethc
{//不是常用的函数，但（历史原因）还是被保留下来
	DeleteFile(SethcPath);
	if (CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE) == false) { Main.InfoBox(L"USFail"); return false; }
	else { SethcState = true; return true; }
}

void RegMouseKey()//注册键盘控制鼠标的热键
{
	RegisterHotKey(Main.hWnd, 8, MOD_CONTROL, 188);
	RegisterHotKey(Main.hWnd, 9, MOD_CONTROL, 190);
	RegisterHotKey(Main.hWnd, 10, MOD_CONTROL, VK_LEFT);
	RegisterHotKey(Main.hWnd, 11, MOD_CONTROL, VK_UP);
	RegisterHotKey(Main.hWnd, 12, MOD_CONTROL, VK_RIGHT);
	RegisterHotKey(Main.hWnd, 13, MOD_CONTROL, VK_DOWN);
}
void UnMouseKey() { for (int i = 8; i < 14; ++i)UnregisterHotKey(Main.hWnd, i); }//注销热键

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(MouseHook, nCode, wParam, lParam); }//空的全局钩子函数
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(KeyboardHook, nCode, wParam, lParam); }

DWORD WINAPI GameThread(LPVOID pM)
{
	UNREFERENCED_PARAMETER(pM);
	if (Main.Width < 700)
	{//展开窗口
		lock = true;//自制线程锁？
		if (!Effect)//无特效
		{
			SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((Main.Width + 260) * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE | SWP_NOREDRAW);
			Main.Button[Main.GetNumbyID(L"Close")].Left += 260;
			Main.Redraw(NULL);
			goto next;
		}
		for (int j = 1; j <= 260; j += 20)
		{
			::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((Main.Width + j) * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE | SWP_NOREDRAW);
			RECT Rc{ (long)(Main.Width * Main.DPI) ,0,(long)((Main.Width + j) * Main.DPI) ,(long)(Main.Height * Main.DPI) };
			Main.Redraw(&Rc);//重绘展开部分
			Rc = Main.GetRECT(Main.GetNumbyID(L"Close"));
			Rc.left -= (long)(20 * Main.DPI);
			Main.Redraw(&Rc);//重绘“关闭”按钮
			Main.Button[Main.GetNumbyID(L"Close")].Left += 20;//右移“关闭按钮”
		}
		Main.Button[Main.GetNumbyID(L"Close")].Left -= 20;
	next:
		Main.Width += 260;
	}
	else
	{
		if (Effect)
			for (int j = 1; j <= 260; j += 20)
			{
				::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((Main.Width - j) * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE | SWP_NOREDRAW);//经测试不能使用RedrawObject
				InvalidateRect(Main.hWnd, 0, FALSE);
				UpdateWindow(Main.hWnd);
				Main.Button[Main.GetNumbyID(L"Close")].Left -= 20;
			}
		Main.Width -= 260;
		if (Effect)Main.Button[Main.GetNumbyID(L"Close")].Left += 20; else Main.Button[Main.GetNumbyID(L"Close")].Left -= 260;
		::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)(Main.Width * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE);
	}
	lock = false;
	return 0;
}
const wchar_t GitGame[] = L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Games/";
bool DownloadGames(const wchar_t* url, const wchar_t* file, DownloadProgress * p, const wchar_t* but, int tot, int cur)
{
	wchar_t Fp[501], URL[121];
	wcscpy_s(Fp, Path);
	wcscat_s(Fp, file);
	wcscpy_s(URL, GitGame);
	wcscat_s(URL, url);
	if (but != NULL)
	{
		Main.Button[Main.GetNumbyID(but)].DownTot = tot;
		Main.Button[Main.GetNumbyID(but)].DownCur = cur;
		Main.Button[Main.GetNumbyID(but)].Download = 0;
		wcscpy_s(p->curi, but);
	}
	if (URLDownloadToFileW(NULL, URL, Fp, 0, p) == S_OK)return true; else return false;
}
DWORD WINAPI DownloadThread(LPVOID pM)//下载游戏
{
	bool f;
	const wchar_t t[5][10] = { L"fly.exe",L"2048.exe",L"block.exe",L"1.exe",L"chess.exe" },
		g[5][6] = { L"Game2",L"Game3" ,L"Game4" ,L"Game5" ,L"Game6" };
	int cur = *(int*)pM;
	DownloadProgress progress;
	if (cur == 1) {
		f = DownloadGames(L"xiaofei.exe", GameName[0], &progress, L"Game1", 2, 1);
		DownloadGames(L"14000%E8%AF%8D%E5%BA%93.ini", L"Games\\14000词库.ini", &progress, L"Game1", 2, 2);
	}
	else
		f = DownloadGames(t[cur - 2], GameName[cur - 1], &progress, g[cur - 2], 1, 1);
	GameExist[cur - 1] = f;
	return 0;
}
BOOL DownFail = false;
wchar_t Git[] = L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Files/";
void Updownload(wchar_t* a, wchar_t* b, const wchar_t* c)//下载文件的三个函数
{
	wcscpy(a, Git);//b tpath
	wcscat(a, c); //a tUrl
	wcscpy(b, Path);
	wcscat(b, c);
	if (wcsstr(a, L"\\") != 0)* _tcsrchr(a, _T('\\')) = '/';
	if (URLDownloadToFileW(NULL, a, b, 0, NULL) == S_OK)FDcur++; else
	{
		FDtot--;
		if (!DownFail)Main.InfoBox(L"DownFail"), DownFail = true;
	}
	InvalidateRect(UpWnd.hWnd, NULL, FALSE);
}
void Create_tPath(wchar_t* t, const wchar_t* x) { wcscpy(t, Path); wcscat(t, x); CreateDirectory(t, NULL); }
DWORD WINAPI DownloadThreadUp(LPVOID pM)
{
	int* tp = (int*)pM;
	int cur = *tp;
	wchar_t tURL[501], tPath[501];
	const wchar_t name[][34] = { L"deleter\\DrvDelFile.exe",L"deleter\\DeleteFile.sys",L"deleter\\DeleteFile_x64.sys" };
	const wchar_t name2[][34] = { L"arp\\wpcap.dll" ,L"arp\\npf.sys",L"arp\\npptools.dll",L"arp\\Packet.dll",L"arp\\WanPacket.dll" , L"arp\\arp.exe" };
	const wchar_t name3[][34 * 2] = { L"ProcessHacker\\ProcessHacker.exe" ,L"ProcessHacker\\ProcessHackerx64.exe",L"ProcessHacker\\kprocesshacker.sys",L"ProcessHacker\\kprocesshacker64.sys" };
	const wchar_t name4[][34] = { L"x32\\360.sys" ,L"x32\\BSOD.sys",L"x32\\Kill.sys",L"x32\\DeleteFile.sys" ,L"x32\\sethc.exe" };
	const wchar_t name5[][34] = { L"x64\\360.sys" ,L"x64\\BSOD.sys",L"x64\\Kill.sys",L"x64\\DeleteFile.sys" };
	const wchar_t name6[][34] = { L"language\\English.ini" ,L"language\\Chinese.ini" };
	switch (cur)
	{
	case 1:case 2:case 3:case 4:case 5:case 6:
		if (cur == 5 || cur == 6) { Create_tPath(tPath, L"cheat"); }
		wcscpy_s(tURL, Git);
		wcscat_s(tURL, FileName[cur - 1]);
		wcscpy_s(tPath, Path);
		wcscat_s(tPath, FileName[cur - 1]);
		if (wcsstr(tURL, L"\\") != 0)* _tcsrchr(tURL, _T('\\')) = '/';
		FDtot++;
		if (URLDownloadToFileW(NULL, tURL, tPath, 0, NULL) == S_OK)FDcur++; else
		{
			FDtot--;
			if (!DownFail)Main.InfoBox(L"DownFail"), DownFail = true;
		}
		InvalidateRect(UpWnd.hWnd, NULL, FALSE);
		break;
	case 7:
		Create_tPath(tPath, L"deleter"); FDtot += 3;
		for (int i = 0; i < 3; ++i) { Updownload(tPath, tURL, name[i]); }
		break;
	case 8:
		Create_tPath(tPath, L"arp"); FDtot += 6;
		for (int i = 0; i < 6; ++i) { Updownload(tPath, tURL, name2[i]); }
		break;
	case 9:
		Create_tPath(tPath, L"ProcessHacker"); FDtot += 4;
		for (int i = 0; i < 4; ++i) { Updownload(tPath, tURL, name3[i]); }
		break;
	case 10:
		Create_tPath(tPath, L"x32"); FDtot += 5;
		for (int i = 0; i < 5; ++i) { Updownload(tPath, tURL, name4[i]); }
		break;
	case 11:
		Create_tPath(tPath, L"x64"); FDtot += 4;
		for (int i = 0; i < 4; ++i) { Updownload(tPath, tURL, name5[i]); }
		break;
	case 12:
		Create_tPath(tPath, L"language"); FDtot += 2;
		for (int i = 0; i < 2; ++i) { Updownload(tPath, tURL, name6[i]); }
		SearchLanguageFiles();
		break;
	}

	if (GetFileAttributes(tPath) != -1)FileExist[cur - 1] = true;
	return 0;
}
COLORREF DoSelectColour()//选择颜色
{
	static CHOOSECOLOR cc = { 0 };
	static COLORREF crCustColors[16];
	cc.lStructSize = sizeof(cc);
	cc.lpCustColors = crCustColors;
	cc.Flags = CC_ANYCOLOR;
	ChooseColor(&cc);
	if (((byte)cc.rgbResult + (byte)(cc.rgbResult >> 8) + (byte)(cc.rgbResult >> 16)) <= 384)Main.Text[18].rgb = RGB(255, 255, 255); else Main.Text[18].rgb = 0;
	return cc.rgbResult;
}
BOOL CALLBACK CathyThread(HWND hwnd, LPARAM lParam)//捕捉窗口
{
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID;
	if (GetParent(hwnd) == 0)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		if (Eatpid[nProcessID] == true)
			if (SetParent(hwnd, CatchWnd.hWnd) != NULL)
				EatList.push(hwnd);
	}
	return 1;
}
void ReturnWindows()//归还窗口
{
	while (!EatList.empty())
	{
		SetParent(EatList.top(), NULL);
		EatList.pop();
	}
	InvalidateRect(CatchWnd.hWnd, NULL, FALSE);
}

void RefreshTDstate()//刷新极域的状态
{
	RECT rc = { (LONG)(165 * Main.DPI), (LONG)(390 * Main.DPI),(LONG)(320 * Main.DPI),(LONG)(505 * Main.DPI) };
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return;

	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = 0;
		_wcslwr_s(pe.szExeFile);
		if (wcscmp(L"stu", pe.szExeFile) == 0)
		{
			wchar_t tmp[101] = { 0 }, tmp2[11] = { 0 };
			if (TDPID != 0)return;//如果已经知道studentmain.exe的pid则退出
			TDPID = pe.th32ProcessID;//否则设置stu的pid
			TDProcess = true;
			
			wcscpy_s(tmp, Main.GetStr(L"_TDPID"));
			//s(tmp);
			_itow_s(TDPID, tmp2, 10);
			wcscat_s(tmp, tmp2);
			Main.SetStr(tmp, L"TDPID");
			wcscpy_s(Main.Text[8].Name, L"TcmdOK");
			Main.Text[8].rgb = RGB(0, 255, 0);
			Main.Button[Main.GetNumbyID(L"kill-TD")].Enabled = true;
			Main.Button[Main.GetNumbyID(L"re-TD")].Enabled = false;
			//s(Main.Text[6].Name);
			Main.Readd(4, 6); Main.Readd(4, 7); Main.Readd(4, 8);
			Main.Readd(2, Main.GetNumbyID(L"kill-TD"));
			Main.Readd(2, Main.GetNumbyID(L"re-TD"));
			//draw:
			Main.Erase(rc);
			Main.Redraw(&rc);
			return;
		}
	}
	if (TDPID == 0)return;
	TDPID = 0;
	TDProcess = false;
	wchar_t tmp[101], tmp2[11];
	wcscpy_s(tmp, Main.GetStr(L"_TDPID"));
	_itow_s(TDPID, tmp2, 10);
	wcscat_s(tmp, L"\\");
	Main.SetStr(tmp, L"TDPID");
	wcscpy_s(Main.Text[8].Name, L"TcmdNO");
	Main.Text[8].rgb = RGB(255, 0, 0);
	Main.Button[Main.GetNumbyID(L"re-TD")].Enabled = true;
	Main.Button[Main.GetNumbyID(L"kill-TD")].Enabled = false;
	Main.Readd(4, 6); Main.Readd(4, 7); Main.Readd(4, 8);
	Main.Readd(2, Main.GetNumbyID(L"kill-TD"));
	Main.Readd(2, Main.GetNumbyID(L"re-TD"));
	Main.Erase(rc);
	Main.Redraw(&rc);
	return;
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
	EnumWindows(CathyThread, NULL);
	return 0;
}
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)//主计时器
{
	UNREFERENCED_PARAMETER(hWnd); UNREFERENCED_PARAMETER(nMsg); UNREFERENCED_PARAMETER(dwTime);
	switch (nTimerid)
	{
	case 1://连续结束进程
		if (Main.Check[8].Value == 1)
			KillProcess(Main.Edit[Main.GetNumByIDe(L"E_TDname")].str);
		break;
	case 2://延时捕捉窗口
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
	case 3://copyleft
		EasterEggState = (EasterEggState + 1) % 11;
		wchar_t tmp[101];
		wcscpy_s(tmp, L"Copy");
		wcscat_s(tmp, EasterEggStr[EasterEggState % 11]);
		wcscat_s(tmp, Main.GetStr(L"Tleft"));
		Main.SetStr(tmp, L"_Tleft");
		InvalidateRect(Main.hWnd, 0, false);
		break;
	case 4://定时创建exp
		if (GetTickCount() - Main.Timer >= 1000 && Main.ExpExist == false)Main.Try2CreateExp();
		break;
	case 5://按钮特效
		for (int i = 1; i <= Main.CurButton; ++i)
		{
			if (Main.CurCover != i && Main.Button[i].Percent > 0)
			{
				Main.Button[i].Percent -= Delta;
				if (Main.Button[i].Percent < 0)Main.Button[i].Percent = 0;
				Main.Readd(2, i);
				RECT rc = Main.GetRECT(i);
				Main.Redraw(&rc);
			}
		}
		if (Main.CurCover != -1 && Main.Button[Main.CurCover].Percent < 100)
		{
			Main.Button[Main.CurCover].Percent += 2 * Delta;
			if (Main.Button[Main.CurCover].Percent > 100)Main.Button[Main.CurCover].Percent = 100;
			Main.Readd(2, Main.CurCover);
			RECT rc = Main.GetRECT(Main.CurCover);
			Main.Redraw(&rc);
		}
		break;
	case 6:
		CatchWnd.Redraw(NULL);
		break;
	case 7://刷新HOOK
		if (Main.Check[9].Value == 1)KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
		if (Main.Check[10].Value == 1)MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInst, 0);
		break;
	case 8://刷新极域状态
		if(Main.CurWnd==2)RefreshTDstate();
		break;
	case 9:
		BSODstate++;
		//if (BSODstate == 1)ShowWindow(BSODhwnd, SW_SHOW);
		if (BSODstate == 8 || BSODstate == 20 || BSODstate == 40)InvalidateRect(BSODhwnd, NULL, FALSE);
		break;
	}
}
void ReleaseDrvFiles()
{
	const wchar_t Filename[5][16] = { L"360.sys",L"BSOD.sys",L"DeleteFile.sys",L"Kill.sys",L"sethc.exe" };
	wchar_t tmp[301] = { 0 };
	if (Bit == 32)
	{
		for (int i = 0; i <= 4; ++i)
		{
			wcscpy_s(tmp, Path);
			wcscat_s(tmp, L"x32\\");
			if (i == 0)CreateDirectory(tmp, NULL);
			wcscat_s(tmp, Filename[i]);
			if (GetFileAttributes(tmp) == -1)ReleaseRes(tmp, i + 152, L"JPG");
		}

	}
	else
	{
		for (int i = 0; i <= 3; ++i)
		{
			wcscpy_s(tmp, Path);
			wcscat_s(tmp, L"x64\\");
			if(i==0)CreateDirectory(tmp,NULL);
			wcscat_s(tmp, Filename[i]);
			//s(tmp);
			if (GetFileAttributes(tmp) == -1)ReleaseRes(tmp, 157+i, L"JPG");
		}

	}
}
void SDesktop()//切换桌面
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
void openfile()//打开文件
{
	//ReleaseDrvFiles();
	OPENFILENAMEW ofn = { 0 };
	wchar_t strFile[MAX_PATH] = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = (LPWSTR)strFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileNameW(&ofn) == TRUE)Main.SetEditStrOrFont(strFile, 0, Main.GetNumByIDe(L"E_View"));
	Main.EditRedraw(Main.GetNumByIDe(L"E_View"));
}
void BrowseFolder()//打开文件夹
{
	wchar_t path[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = Main.GetStr(L"OpenFolder");
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		SHGetPathFromIDList(pidl, path);
		SetCurrentDirectory(path);
		Main.SetEditStrOrFont(path, 0, Main.GetNumByIDe(L"E_View"));
		Main.EditRedraw(Main.GetNumByIDe(L"E_View"));
	}
}

void ReopenTD()//尝试打开极域
{
	SearchTool(L"C:\\Program Files\\Mythware", 1);
	SearchTool(L"C:\\Program Files\\TopDomain", 1);
	SearchTool(L"C:\\Program Files (x86)\\Mythware", 1);
	SearchTool(L"C:\\Program Files (x86)\\TopDomain", 1);
}
void DeleteShutdown()//尝试删除shutdown.exe
{
	SearchTool(L"C:\\Program Files\\Mythware", 2);
	SearchTool(L"C:\\Program Files\\TopDomain", 2);
	SearchTool(L"C:\\Windows\\System32", 2);
	SearchTool(L"C:\\Program Files (x86)\\Mythware", 2);
	SearchTool(L"C:\\Program Files (x86)\\TopDomain", 2);
	SearchTool(L"C:\\Windows\\SysNative", 2);
}
bool RunHOOK()//运行hook.exe
{
	wchar_t tmp[501];
	wcscpy_s(tmp, Path);
	wcscat_s(tmp, L"hook.exe");
	if (GetFileAttributes(tmp) == -1)ReleaseRes(tmp, IDR_JPG3, L"JPG");
	return RunEXE(tmp, CREATE_NO_WINDOW, nullptr);
}
void FakeBSOD()
{
	if (FB == TRUE)
	{
		FB = FALSE;
		InitBSOD();
	}
	BSODhwnd = CreateWindow(BSODWindow, L"fake BSOD window", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInst, nullptr);
	BSODstate = 0;
	SetTimer(BSODhwnd, 9, 100, (TIMERPROC)TimerProc);
	InvalidateRect(BSODhwnd, NULL, FALSE);
	UpdateWindow(BSODhwnd);
	ShowWindow(BSODhwnd, SW_SHOW);
}
void BSOD()//尝试蓝屏
{
	LockCursor();
	if (Admin == FALSE)
	{
		wchar_t tmp[34];
		HDESK VirtualDesk = CreateDesktop(fBSODdesk, NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);

		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		si.lpDesktop = fBSODdesk;
		if (Main.Check[15].Value)
			wcscpy_s(tmp, L"d -showbsod \"old\"");
		else
			wcscpy_s(tmp, L"d -showbsod \"new\"");
		PROCESS_INFORMATION pi = { 0 };

		CreateProcess(Name, tmp, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
		BlackBoard = true;
		InitScreen();
		HWND t = CreateWindow(ScreenWindow, L"You can't see me.", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInst, nullptr);
		ShowWindow(t, SW_SHOW);
		for (int i = 0; i < 10; ++i)InvalidateRect(t, NULL, false), UpdateWindow(t);

		SwitchDesktop(VirtualDesk);
		//Sleep(4000);
		//SwitchDesktop(defaultDesk);
	}
	else
	{
		ReleaseDrvFiles();
		UnloadNTDriver(L"BSOD");
		if (Bit == 32)LoadNTDriver(L"BSOD", L"x32\\BSOD.sys"); else LoadNTDriver(L"BSOD", L"x64\\BSOD.sys");
		Main.Check[14].Value = true;
		KillProcess(L"svc");
		KillProcess(L"sys");
	}
	RestartDirect();
}
void EasterEgg(bool flag)//开关easteregg
{
	if (flag)SetTimer(Main.hWnd, 3, 100, (TIMERPROC)TimerProc), EasterEggFlag = true; else KillTimer(Main.hWnd, 3), EasterEggFlag = false;
}
void AutoViewPass()//读取密码并显示
{//不支持加密过的(因为懒得写)
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
		if (MessageBox(Main.hWnd, Main.GetStr(L"VPFail"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)goto finish;
	ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType, (LPBYTE)& szLocation, &dwSize);
	if (ret != 0 && slient == false)
		if (MessageBox(Main.hWnd, Main.GetStr(L"VPUKE"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)goto finish;
	if (szLocation[0] != 0)
	{
		wchar_t Tmp[300];
		wcscpy_s(Tmp, Main.GetStr(L"pswdis"));
		wcscat_s(Tmp, szLocation);
		Main.InfoBox(Tmp);
	}
	else
		Main.InfoBox(L"VPNULL");
finish:
	RegCloseKey(hKey);
}
void AutoClearPassWd()//自动清空密码
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
		Main.InfoBox(L"ACFail");
		RegCloseKey(hKey);
		return;
	}
	ret = RegSetValueEx(hKey, L"Knock", 0, REG_BINARY, (const BYTE*)& key, sizeof(key));
	ret2 = RegSetValueEx(hKey, L"Knock1", 0, REG_BINARY, (const BYTE*)& key, sizeof(key));
	if (ret != 0 || ret2 != 0)
	{
		Main.InfoBox(L"ACUKE");
		RegCloseKey(hKey);
		return;
	}
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);
	if (ret != 0) { Main.InfoBox(L"ACFail"); RegCloseKey(hKey); return; }
	ret = RegSetValueEx(hKey, L"UninstallPasswd", 0, REG_SZ, (const BYTE*)& tmp, sizeof(tmp));
	if (ret != 0) { Main.InfoBox(L"ACUKE"); RegCloseKey(hKey); return; }
	Main.InfoBox(L"ACOK");
	RegCloseKey(hKey);
}
void ChangePasswordEx(wchar_t* a, int type)//自动更改密码
{
	wchar_t tmp[1001] = { 0 };
	HKEY hKey; LONG ret;
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);
	else//打开键值
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);
	if (ret != 0) { Main.InfoBox(L"ACFail"); RegCloseKey(hKey); return; }//打开失败
	if (type == 2)wcscpy_s(tmp, L"Passwd"), wcscat_s(tmp, a);//是否在之前加上Passwd
	else wcscpy_s(tmp, a);
	ret = RegSetValueEx(hKey, L"UninstallPasswd", 0, REG_SZ, (const BYTE*)& tmp, sizeof(wchar_t) * wcslen(tmp));
	if (ret != 0) { Main.InfoBox(L"ACUKE"); RegCloseKey(hKey); return; }//失败
	RegCloseKey(hKey);
	if (type == 1)//额外修改knock和异或加密
	{
		wcscpy_s(tmp, L"Passwd");
		BYTE data[2000];
		wcscat(tmp, a);
		int len = wcslen(tmp) * 2;
		for (int i = 0; i < len; ++i)tmp[i] ^= 0x4350u;
		for (int i = 0; i < len; ++i)
		{
			data[i * 2 + 1] = tmp[i] >> 8;
			data[i * 2] = (BYTE)tmp[i] - (tmp[i] >> 8 << 8);
		}
		if (Bit != 64)
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);
		else
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE, &hKey);
		RegSetValueEx(hKey, L"Key", 0, REG_BINARY, (const BYTE*)data, sizeof(char) * len);

		if (Bit != 64)change(a, false); else change(a, true);
	}
	Main.InfoBox(L"ACOK");
	return;
}
bool RunCmdLine(LPWSTR str)
{
	_wcslwr(str);
	if (wcsstr(str, L"-showbsod") != NULL)
	{
		slient = true;
		typedef DWORD(CALLBACK * SEtProcessDPIAware)(void);
		SEtProcessDPIAware SetProcessDPIAware;
		HMODULE huser;
		huser = LoadLibrary(L"user32.dll");
		SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
		if (SetProcessDPIAware != NULL)SetProcessDPIAware();
		wchar_t tmp[1001] = { 0 }, *tmp1 = wcsstr(str, L"-showbsod");
		if (!Findquotations(tmp1, tmp)) { FakeBSOD(); return false; }
		if (wcscmp(tmp, L"old") == 0)Main.Check[15].Value = true;
		FakeBSOD();
		return false;
	}
	if (wcsstr(str, L"-top") != NULL)//显示在安全桌面上用
	{
		Main.Check[4].Value = 1;
		TOP = TRUE;
		CreateThread(NULL, 0, TopThread, NULL, 0, NULL);
		return false;//false表示继续运行
	}
	DWORD pid = GetParentProcessID(GetCurrentProcessId());
	AttachConsole(pid);//附加到父进程命令行上
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);
	setlocale(LC_ALL, "chs");
	slient = true;
	printf("\n");

	if (wcsstr(str, L"-sethc") != NULL)
	{
		if (!DeleteSethc())Main.InfoBox(L"DSR3Fail");
		AutoSetupSethc();
		AutoCopyNTSD();
		return true;
	}
	if (wcsstr(str, L"-hook") != NULL)
	{
		KillProcess(L"hoo");
		if (!RunHOOK())Main.InfoBox(L"OneFail");
		return true;
	}
	if (wcsstr(str, L"-auto") != NULL) { KillTop(); return true; }
	if (wcsstr(str, L"-unsethc") != NULL) {
		DeleteFile(SethcPath);
		CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE); return true;
	}
	if (wcsstr(str, L"-unhook") != NULL) { KillProcess(L"hoo"); return true; }
	if (wcsstr(str, L"-viewpass") != NULL) { AutoViewPass(); return true; }
	if (wcsstr(str, L"-antishutdown") != NULL) { DeleteShutdown(); return true; }
	if (wcsstr(str, L"-reopen") != NULL) { ReopenTD(); return true; }
	if (wcsstr(str, L"-bsod") != NULL) { BSOD(); return true; }
	//s(1);

	if (wcsstr(str, L"-restart") != NULL) { Restart(); return true; }
	if (wcsstr(str, L"-clear") != NULL) { AutoClearPassWd(); return true; }
	if (wcsstr(str, L"-rekill") != NULL)
	{
		wchar_t tmp[1001], * tmp1 = wcsstr(str, L"-rekill");
		if (!Findquotations(tmp1, tmp))return true;
		while (1) { KillProcess(tmp); Sleep(50); }
	}
	if (wcsstr(str, L"-delete") != NULL)
	{
		wchar_t tmp[1001], * tmp1 = wcsstr(str, L"-delete");
		if (!Findquotations(tmp1, tmp))return true;
		AutoDelete(tmp);
		return true;
	}
	if (wcsstr(str, L"-changewithpasswd") != NULL)//二者顺序不能调换
	{
		wchar_t tmp[1001], * tmp1 = wcsstr(str, L"-changewithpasswd");
		if (!Findquotations(tmp1, tmp))ExitProcess(0);
		ChangePasswordEx(tmp, 2);
		return true;
	}
	if (wcsstr(str, L"-change") != NULL)
	{
		wchar_t tmp[1001], * tmp1 = wcsstr(str, L"-change");
		if (!Findquotations(tmp1, tmp))ExitProcess(0);
		ChangePasswordEx(tmp, 1);
		return true;
	}
	return false;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,//入口点
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	wchar_t szProcess[] = L"C:\\windows\\explorer";

	GetPath();//获取自身目录
	Admin = IsUserAnAdmin();//是否管理员

	GetBit();//获取位数
	if (Bit != 34)
		wcscpy_s(SethcPath, L"C:\\Windows\\System32\\sethc.exe"),
		wcscpy_s(ntsdPath, L"C:\\Windows\\System32\\ntsd.exe");
	else//根据系统位数设置path
		wcscpy_s(SethcPath, L"C:\\Windows\\SysNative\\sethc.exe"),
		wcscpy_s(ntsdPath, L"C:\\Windows\\SysNative\\ntsd.exe");

	BOOL Flag = Backup();

	CreateStrs; //创建字符串

	if (*lpCmdLine != 0) { if (RunCmdLine(lpCmdLine) == true)return 0; }

	defaultDesk = GetThreadDesktop(GetCurrentThreadId());//创建虚拟桌面
	hVirtualDesk = CreateDesktop(szVDesk, NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);
	if (Flag == TRUE)
	{
		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		si.lpDesktop = szVDesk;
		RunEXE(szProcess, NULL, &si);
	}
	hCurrentDesk = defaultDesk;

	if (!InitInstance(hInstance, nCmdShow))return FALSE;
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
	InitBrushs;//创建画笔 & 画刷
	//放在TestFunctions.h里以减少GUI.cpp长度

	hInst = hInstance; // 将实例句柄存储在全局变量中
	Main.InitClass(hInst);
	if (!Main.RegisterClassW(hInst, WndProc, szWindowClass))return FALSE;

	Main.Obredraw = true;
	Main.hWnd = CreateWindowW(szWindowClass, Main.GetStr(L"Title"), NULL, CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, NULL, nullptr, hInstance, nullptr);
	Main.Timer = GetTickCount();
	SetTimer(Main.hWnd, 4, 100, (TIMERPROC)TimerProc);
	CreateCaret(Main.hWnd, NULL, 1, 20);
	SetCaretBlinkTime(500);
	if (!Main.hWnd)return FALSE;
	if (Effect)
	{
		Main.ButtonEffect = true;//按钮渐变色特效
		SetTimer(Main.hWnd, 5, 33, (TIMERPROC)TimerProc);//启用渐变色计时器/30fps
		SetWindowLong(Main.hWnd, GWL_EXSTYLE, GetWindowLong(Main.hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(Main.hWnd, NULL, 234, LWA_ALPHA);//半透明特效
	}
	SetWindowLong(Main.hWnd, GWL_STYLE, GetWindowLong(Main.hWnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_SYSMENU & ~WS_GROUP & ~WS_TABSTOP);//无边框窗口

	FileList = CreateWindowW(L"ListBox", NULL, WS_CHILD | LBS_STANDARD, 180, 430, 265, 110, Main.hWnd, (HMENU)1, hInstance, 0);//创建语言文件选择ListBox
	SearchLanguageFiles();//寻找语言文件
	::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);

	RegisterHotKey(Main.hWnd, 1, MOD_CONTROL, 'P');//显示 隐藏
	RegisterHotKey(Main.hWnd, 2, MOD_CONTROL, 'B');//切换桌面
	RegisterHotKey(Main.hWnd, 7, MOD_CONTROL | MOD_ALT, 'K');//键盘控制鼠标

	hZXFBitmap = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ZXF1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);//资源文件中加载图像
	hZXFsign = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ZXF2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	Main.CreateEditEx(325 + 5, 420, 110 - 10, 50, 1, L"explorer.exe", L"E_runinVD", 0, true);//创建输入框
	Main.CreateEditEx(185 + 5, 102, 110 - 10, 40, 2, L"输入端口", L"E_ApplyCh", 0, false);
	Main.CreateEditEx(365 + 5, 175, 210 - 10, 50, 2, L"输入密码", L"E_CP", 0, false);
	Main.CreateEditEx(195 + 5, 102, 310 - 10, 37, 3, L"浏览文件/文件夹", L"E_View", 0, false);
	Main.CreateEditEx(277 + 5, 186, 138 - 10, 25, 5, L"StudentMain", L"E_TDname", 0, true);

	Main.CreateButtonEx(1, 1, 50, 139, 64, 0, L"安装/卸载", WhiteBrush, DBlueBrush, LBlueBrush, White, DBlue, LBlue, 0, true, true, 0, L"P1");//切换页面按钮
	Main.CreateButtonEx(2, 1, 115, 139, 64, 0, L"极域工具箱", WhiteBrush, DBlueBrush, LBlueBrush, White, DBlue, LBlue, 0, true, true, 0, L"P2");
	Main.CreateButtonEx(3, 1, 180, 139, 64, 0, L"其他工具", WhiteBrush, DBlueBrush, LBlueBrush, White, DBlue, LBlue, 0, true, true, 0, L"P3");
	Main.CreateButtonEx(4, 1, 245, 139, 64, 0, L"关于", WhiteBrush, DBlueBrush, LBlueBrush, White, DBlue, LBlue, 0, true, true, 0, L"P4");
	Main.CreateButtonEx(5, 1, 310, 139, 64, 0, L"设置", WhiteBrush, DBlueBrush, LBlueBrush, White, DBlue, LBlue, 0, true, true, 0, L"P5");
	Main.CreateButtonEx(6, 1, 375, 139, 175, 0, L"一键安装", WhiteBrush, DBlueBrush, LBlueBrush, White, DBlue, LBlue, 0, true, true, 0, L"QuickSetup");
	Main.CurButton = 6;
	Main.CreateLine(140, 51, 140, 549, 0, RGB(150, 150, 150));
	Main.CreateLine(0, 51, 0, 549, 0, RGB(150, 150, 150));//页面按钮边上的线
	Main.CreateLine(2, 114, 139, 114, 0, RGB(150, 150, 150));
	Main.CreateLine(2, 179, 139, 179, 0, RGB(150, 150, 150));
	Main.CreateLine(2, 244, 139, 244, 0, RGB(150, 150, 150));
	Main.CreateLine(2, 309, 139, 309, 0, RGB(150, 150, 150));
	Main.CreateLine(2, 374, 139, 374, 0, RGB(150, 150, 150));
	Main.CreateLine(622, 51, 622, 549, 0, 0);

	Main.CreateFrame(170, 75, 410, 175, 1, L" Sethc ");
	Main.CreateButton(195, 100, 110, 50, 1, L"应用层", L"DelR3");//sethc
	Main.CreateButton(325, 100, 110, 50, 1, L"驱动层", L"DelR0");
	if (SethcState == false)
	{//如果sethc已经被删除
		Main.Button[Main.CurButton].Enabled = Main.Button[Main.CurButton - 1].Enabled = false;
		wcscpy_s(Main.Button[Main.CurButton].Name, Main.GetStr(L"Deleted"));
		wcscpy_s(Main.Button[Main.CurButton - 1].Name, Main.GetStr(L"Deleted"));
	}
	Main.CreateLine(185, 165, 565, 165, 1, 0);//中间的那根线
	Main.CreateText(470, 117, 1, L"Tdelete", RGB(255, 100, 0));//删除文件
	Main.CreateText(470, 197, 1, L"Tcopy", RGB(255, 100, 0));//复制文件
	Main.CreateButton(195, 180, 110, 50, 1, L"应用层", L"SethcR3");
	Main.CreateButton(325, 180, 110, 50, 1, L"驱动层", L"SethcR0");

	Main.CreateFrame(170, 275, 410, 95, 1, L" 全局键盘钩子 ");//hook
	Main.CreateButton(195, 300, 110, 50, 1, L"安装", L"hookS");
	Main.CreateButton(325, 300, 110, 50, 1, L"卸载", L"hookU");

	Main.CreateFrame(170, 395, 410, 122, 1, L" 虚拟桌面 ");
	Main.CreateButton(195, 420, 110, 50, 1, L"运行程序", L"runinVD");//虚拟桌面
	Main.CreateButton(450, 420, 110, 50, 1, L"切换桌面", L"SwitchD");
	Main.CreateText(195, 485, 1, L"Tctrl+b", RGB(255, 100, 0));

	Main.CreateFrame(160, 75, 160, 146, 2, L" 频道工具 ");
	Main.CreateButton(185, 155, 110, 45, 2, L"应用", L"ApplyCh");

	Main.CreateFrame(345, 75, 250, 272, 2, L" 管理员密码工具 ");
	Main.CreateButton(365, 102, 97, 45, 2, L"清空密码", L"ClearPass");
	Main.CreateButton(477, 102, 97, 45, 2, L"查看密码", L"ViewPass");
	Main.CreateButton(365, 235, 97, 45, 2, L"改密1", L"CP1");
	Main.CreateButton(477, 235, 97, 45, 2, L"改密2", L"CP2");
	Main.CreateText(365, 295, 2, L"Tcp1", RGB(50, 50, 50));
	Main.CreateText(365, 317, 2, L"Tcp2", RGB(255, 100, 0));
	Main.CreateLine(360, 160, 583, 160, 2, 0);

	Main.CreateCheck(165, 255, 2, 150, L" 伪装工具条旧");
	Main.CreateCheck(165, 280, 2, 150, L" 伪装工具条新");
	Main.CreateCheck(165, 305, 2, 150, L" 伪装托盘图标");

	Main.CreateFrame(160, 370, 160, 135, 2, L" 进程工具 ");
	Main.CreateText(175, 390, 2, L"TDState", 0);
	Main.CreateText(175, 415, 2, L"TDPID", 0);
	Main.CreateText(250, 390, 2, L"TcmdNO", RGB(255, 0, 0));
	Main.CreateButton(175, 445, 60, 45, 2, L"结束", L"kill-TD");
	Main.Button[Main.CurButton].Enabled = false;
	Main.CreateButton(245, 445, 60, 45, 2, L"启动", L"re-TD");

	Main.CreateButton(345, 370, 115, 55, 2, L"程序窗口化", L"windows.ex");
	Main.CreateButton(480, 370, 115, 55, 2, L"防止教师关机", L"ANTI-");
	Main.CreateButton(345, 450, 115, 55, 2, L"显示于安全桌面", L"desktop");
	Main.CreateButton(480, 450, 115, 55, 2, L"自动防控制", L"auto-5");

	Main.CreateFrame(170, 75, 410, 150, 3, L" 文件粉碎机 ");
	Main.CreateButton(520, 102, 36, 36, 3, L"...", L"viewfile");
	Main.CreateButton(436, 151, 120, 55, 3, L"打开文件夹", L"folder");
	Main.CreateButton(325, 151, 97, 55, 3, L"开始粉碎", L"TI");
	Main.CreateButton(195, 151, 115, 55, 3, L"T.A.粉碎机", L"TA");

	Main.CreateFrame(170, 255, 273, 105, 3, L" 电源 - 慎用 ");
	Main.CreateButton(192, 280, 115, 60, 3, L"BSOD(蓝屏)", L"BSOD");
	Main.CreateButton(324, 280, 100, 60, 3, L"瞬间重启", L"NtShutdown");

	Main.CreateFrame(170, 388, 410, 105, 3, L" 杂项 ");
	Main.CreateButton(192, 412, 100, 60, 3, L"ARP攻击", L"ARP");//34
	Main.CreateButton(304, 412, 140, 60, 3, L"System权限CMD", L"SuperCMD");
	Main.CreateButton(455, 412, 105, 60, 3, L"干掉360", L"Killer");

	Main.CreateText(325, 80, 4, L"Tcoder", NULL);//"关于"中的一堆文字
	Main.CreateText(325, 105, 4, L"Tver", NULL);
	Main.CreateText(372, 130, 4, L"Tver2", NULL);
	Main.CreateText(170, 260, 4, L"Ttip1", NULL);
	Main.CreateText(170, 285, 4, L"Ttip2", NULL);
	Main.CreateText(170, 320, 4, L"Tbit", NULL);
	Main.CreateText(285, 320, 4, L"Twinver", NULL);
	Main.CreateText(455, 320, 4, L"Tcmd", NULL);
	Main.CreateText(170, 345, 4, L"TTDv", NULL);
	Main.CreateText(375, 345, 4, L"TIP", NULL);
	Main.CreateLine(170, 400, 590, 400, 4, 0);
	Main.CreateText(170, 415, 4, L"_Tleft", NULL);
	Main.CreateText(170, 440, 4, L"Tleft2", NULL);
	Main.CreateButton(490, 415, 100, 50, 4, L"更多", L"more.txt");
	Main.CreateButton(490, 477, 100, 50, 4, L"系统信息", L"sysinfo");

	Main.CreateCheck(180, 70, 5, 100, L" 窗口置顶");
	Main.CreateCheck(180, 100, 5, 160, L" Ctrl+R 紧急蓝屏");
	Main.CreateCheck(180, 130, 5, 160, L" Ctrl+T 瞬间重启");
	Main.CreateCheck(180, 160, 5, 200, L" Ctrl+Alt+P 截图/显示");
	Main.CreateCheck(180, 190, 5, 94, L" 连续结束                   .exe");
	Main.CreateCheck(180, 220, 5, 160, L" 禁止键盘钩子");
	Main.CreateCheck(180, 250, 5, 160, L" 禁止鼠标钩子");
	Main.CreateCheck(180, 280, 5, 240, L" Ctrl+Alt+K 键盘操作鼠标");
	Main.CreateCheck(180, 310, 5, 160, L" 低画质");
	Main.CreateCheck(180, 340, 5, 160, L" 缩小/放大");
	Main.CreateCheck(180, 370, 5, 310, L" 使用ProcessHacker和ntsd结束进程");
	if (Admin == 0)Main.CreateCheck(180, 400, 5, 200, L" 使用旧版伪装蓝屏");

	Main.CreateButton(470, 430, 100, 45, 5, L"永久隐藏", L"hidest");
	Main.CreateButton(470, 485, 100, 45, 5, L"下载文件", L"upgrade");

	Main.CreateButton(466, 255, 115, 106, 3, L"打游戏", L"Games");
	Main.CreateFrame(655, 75, 170, 420, 0, L" 游戏 ");
	Main.CreateButton(680, 95, 120, 50, 0, L"小飞猜词", L"Game1");
	Main.CreateButton(680, 160, 120, 50, 0, L"Flappy Bird", L"Game2");
	Main.CreateButton(680, 225, 120, 50, 0, L"2048", L"Game3");
	Main.CreateButton(680, 290, 120, 50, 0, L"俄罗斯方块", L"Game4");
	Main.CreateButton(680, 355, 120, 50, 0, L"见缝插针", L"Game5");
	Main.CreateButton(680, 420, 120, 50, 0, L"五子棋", L"Game6");

	Main.CreateArea(20, 10, 32, 32, 0);//极域图标
	Main.CreateArea(170, 75, 135, 165, 4);//zxf头像

	Main.CurButton++;

	if (Admin == 0)Main.CreateText(60, 17, 0, L"Tmain", RGB(255, 255, 255));//关闭 按钮
	else Main.CreateText(60, 17, 0, L"Tmain2", RGB(255, 255, 255));
	Main.CreateButtonEx(Main.CurButton, 530, 10, 60, 30, 0, L"×", \
		CreateSolidBrush(RGB(255, 109, 109)), CreateSolidBrush(RGB(250, 100, 100)), CreateSolidBrush(RGB(232, 95, 95)), \
		CreatePen(PS_SOLID, 1, RGB(255, 109, 109)), CreatePen(PS_SOLID, 1, RGB(250, 100, 100)), CreatePen(PS_SOLID, 1, RGB(232, 95, 95)), \
		Main.DefFont, 1, 1, RGB(255, 255, 255), L"Close");

	CatchWnd.InitClass(hInst);

	CatchWnd.CreateText(18, 10, 0, L"Processnam", NULL);
	CatchWnd.CreateText(303, 10, 0, L"Delay", NULL);
	CatchWnd.CreateText(355, 50, 0, L"second", NULL);
	CatchWnd.CreateEditEx(15 + 5, 35, 260 - 10, 45, 0, L"StudentMain.exe", L"E_Pname", CatchWnd.DefFont, true);
	CatchWnd.CreateEditEx(295 + 5, 35, 50 - 10, 45, 0, L"5", L"E_Delay", CatchWnd.DefFont, true);
	CatchWnd.CreateButton(15, 100, 100, 50, 0, L"捕捉窗口", L"CatchW");
	CatchWnd.CreateButton(130, 100, 100, 50, 0, L"监视窗口", L"CopyW");
	CatchWnd.CreateButton(245, 100, 100, 50, 0, L"释放窗口", L"ReturnW");

	UpWnd.InitClass(hInst);

	UpWnd.CreateCheck(15, 10, 0, 100, L" hook.exe");
	UpWnd.CreateCheck(15, 40, 0, 100, L" sethc.exe");
	UpWnd.CreateCheck(15, 70, 0, 100, L" ntsd.exe");
	UpWnd.CreateCheck(15, 100, 0, 100, L" PsExec.exe");
	UpWnd.CreateCheck(15, 130, 0, 100, L" 伪装工具条旧");
	UpWnd.CreateCheck(15, 160, 0, 100, L" 伪装工具条新");
	UpWnd.CreateCheck(15, 190, 0, 100, L" 驱动删除文件");
	UpWnd.CreateCheck(15, 220, 0, 100, L" ARP攻击");
	UpWnd.CreateCheck(15, 250, 0, 100, L" ProcessHacker");
	UpWnd.CreateCheck(15, 280, 0, 100, L" 32位驱动");
	UpWnd.CreateCheck(15, 310, 0, 100, L" 64位驱动");
	UpWnd.CreateCheck(15, 340, 0, 100, L" 语言文件");

	SetFrame();
	SetWindowPos(Main.hWnd, 0, 0, 0, 621, 550, SWP_NOMOVE);
	Main.Width = 621; Main.Height = 550;

	typedef DWORD(CALLBACK * SEtProcessDPIAware)(void);
	SEtProcessDPIAware SetProcessDPIAware;
	HMODULE huser;
	huser = LoadLibrary(L"user32.dll");
	SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
	if (SetProcessDPIAware != NULL)SetProcessDPIAware();


	Main.Redraw(NULL);//第一次创建窗口时全部重绘
	if (!slient)ShowWindow(Main.hWnd, nCmdShow);

	typedef struct tagCHANGEFILTERSTRUCT {//使程序接受非管理员程序(explorer)的拖拽请求
		DWORD cbSize;
		DWORD ExtStatus;
	} CHANGEFILTERSTRUCT, * PCHANGEFILTERSTRUCT;
	typedef BOOL(WINAPI * ZXF)(HWND hwnd, UINT message, DWORD action, PCHANGEFILTERSTRUCT pChangeFilterStruct);
	ZXF ChangeWindowMessageFilterEx = NULL;
	HMODULE hModule = GetModuleHandle(L"user32.dll");
	if (hModule != NULL)ChangeWindowMessageFilterEx = (ZXF)GetProcAddress(hModule, "ChangeWindowMessageFilterEx");
	if (ChangeWindowMessageFilterEx == NULL)return TRUE;
	ChangeWindowMessageFilterEx(Main.hWnd, WM_DROPFILES, 1, 0);
	ChangeWindowMessageFilterEx(Main.hWnd, 0x0049, 1, NULL);

	return TRUE;
}

//响应函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//主窗口响应函数
{
	switch (message)
	{
	case WM_CLOSE://关闭
		PostQuitMessage(0);
		break;
	case	WM_CREATE://创建窗口
		if (Effect)
		{//启动阴影特效
			Cshadow.Initialize(hInst);
			Cshadow.Create(hWnd);
		}
		rdc = GetDC(Main.hWnd);//创建bitmap
		hdc = CreateCompatibleDC(rdc);
		hBmp = CreateCompatibleBitmap(rdc, 1330, 1100);
		SelectObject(hdc, hBmp);
		ReleaseDC(Main.hWnd, rdc);
		DragAcceptFiles(hWnd, true);
		break;
	case WM_DROPFILES://接受文件拖拽信息
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (Main.InsideEdit(4, point) == TRUE)
		{
			wchar_t tmp[501], * a = Main.Edit[Main.GetNumByIDe(L"E_View")].OStr;
			HDROP hDrop = (HDROP)wParam;
			DragQueryFile(hDrop, 0, tmp, MAX_PATH);
			if (*a != 0)* a = 0;
			Main.SetEditStrOrFont(tmp, NULL, Main.GetNumByIDe(L"E_View"));
			Main.EditRedraw(Main.GetNumByIDe(L"E_View"));
		}
		break;
	}
	case WM_HOTKEY://热键
	{
		switch (wParam)
		{
		case 1: {//隐藏 \ 显示
			ShowWindow(Main.hWnd, 5 * HideState);
			HideState = 1 - HideState;
			break; }
		case 2: {SDesktop(); break; }//切换桌面
		case 3: {KillProcess(L"stu"); break; }//scroll lock结束极域
		case 4: {BSOD(); break; }//ctrl+r蓝屏
		case 5: {Restart(); break; }//ctrl+t重启
		case 6://截图 \ 显示
		{
			ScreenState++;
			if (ScreenState == 1)CaptureImage();
			if (ScreenState == 2)
				CreateWindow(ScreenWindow, L"You can't see me.", WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInst, nullptr);
			break;
		}
		case 7:
		{
			if (Main.Check[11].Value == false)RegMouseKey(), Main.Check[11].Value = true; else UnMouseKey(), Main.Check[11].Value = false;
			Main.Readd(3, 11);
			RECT rc = Main.GetRECTc(11);
			Main.Redraw(&rc);
			break;
		}
		case 8: {//键盘操作鼠标
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);//左键
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			break; }
		case 9: {
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);//右键
			break; }
#pragma warning(disable:4245)
		case 10: {
			mouse_event(MOUSEEVENTF_MOVE, (DWORD)(-10 * Main.DPI), 0, 0, 0);//左移
			break; }
		case 11: {
			mouse_event(MOUSEEVENTF_MOVE, 0, (DWORD)(-10 * Main.DPI), 0, 0);//上移
			break; }
#pragma warning(default:4245)
		case 12: {
			mouse_event(MOUSEEVENTF_MOVE, (DWORD)(10 * Main.DPI), 0, 0, 0);//右移
			break; }
		case 13: {
			mouse_event(MOUSEEVENTF_MOVE, 0, (DWORD)(10 * Main.DPI), 0, 0);
			break; }
		}

		Main.EditHotKey(wParam);
		break;
	}
	case WM_KILLFOCUS://这个事件在鼠标 选中 其他窗口时触发 
	{//不代表鼠标 移出 窗口，因此(没什么用)
		Main.EditUnHotKey();
		for (int i = 1; i <= Main.CurButton; ++i)Main.Button[i].Percent = 0;
		break;
	}
	case WM_PAINT://绘图
	{
		HBRUSH BitmapBrush=NULL; HICON hicon;
		RECT rc; bool f = false;
		GetUpdateRect(hWnd, &rc, false);
		if (rc.top != 0)f = true;
		if (Main.hdc == NULL)Main.SetHDC(hdc);
		PAINTSTRUCT ps;
		rdc = BeginPaint(hWnd, &ps);
		if (!Main.es.empty())
		{
			SelectObject(Main.hdc, White);
			SelectObject(Main.hdc, WhiteBrush);
			while (!Main.es.empty())
			{
				Rectangle(Main.hdc, Main.es.top().left, Main.es.top().top, Main.es.top().right, Main.es.top().bottom);
				Main.es.pop();
			}
		}
		if (!Main.rs.empty())
		{
			while (!Main.rs.empty())
			{
				Main.RedrawObject(Main.rs.top().first, Main.rs.top().second);
				Main.rs.pop();
			}
			goto finish;
		}
		if (f == true)goto next;
		SetBkMode(rdc, TRANSPARENT);
		SetBkMode(hdc, TRANSPARENT);

		SelectObject(hdc, WhiteBrush);//白色背景
		Rectangle(hdc, 0, 0, (int)(900 * Main.DPI), (int)(Main.Height * Main.DPI + 1));

		SelectObject(hdc, GREEN);//绿色顶部
		SelectObject(hdc, green);
		Rectangle(hdc, 0, 0, (int)(900 * Main.DPI), (int)(50 * Main.DPI));

		SetTextColor(hdc, RGB(0, 0, 0));
		SelectObject(hdc, BLACK);
		SelectObject(hdc, WhiteBrush);
	next:
		Main.DrawEVERYTHING();

		if (f == true)  goto finish;

		hicon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_GUI));
		DrawIconEx(hdc, (int)(20 * Main.DPI), (int)(10 * Main.DPI), hicon, (int)(32 * Main.DPI), (int)(32 * Main.DPI), 0, NULL, DI_NORMAL | DI_COMPAT);
		DeleteObject(hicon);
		if (Main.CurWnd == 4)
		{
			CImage img;
			img.Load(L"C:\\SAtemp\\1.JPG");
			RECT rc1{ (int)(170 * Main.DPI), (int)(75 * Main.DPI), (int)((135+170) * Main.DPI), (int)((170+75) * Main.DPI) };
			img.Draw(hdc, rc1);
			if (EasterEggFlag)
			{
				BitmapBrush = CreatePatternBrush(hZXFsign);
				SelectObject(hdc, BitmapBrush);
				SelectObject(hdc, White);
				Rectangle(hdc, 105 * 2, 75 * 12, 105 * 3, 75 * 13);
				StretchBlt(hdc, (int)(165 * Main.DPI), (int)(465 * Main.DPI), (int)(105 * Main.DPI), (int)(75 * Main.DPI), hdc, 105 * 2, 75 * 12, 105, 75, SRCCOPY);
			}
			DeleteObject(BitmapBrush);
		}
	finish:
		BitBlt(rdc, rc.left, rc.top, max((long)(Main.Width * Main.DPI), rc.right - rc.left), max((long)(Main.Height * Main.DPI), rc.bottom - rc.top), hdc, rc.left, rc.top, SRCCOPY);
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
			Main.Check[3].Value = false;
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
	{
		POINT point; GetCursorPos(&point); ScreenToClient(Main.hWnd, &point);
		Main.EditGetNewInside(point);
		Main.ButtonGetNewInside(point);
		Main.CheckGetNewInside(point);
		Main.AreaGetNewInside(point);
		if (Main.CurCover != -1 || Main.CoverCheck != 0 || Main.CoverEdit != 0 || Main.CoverArea != 0)Main.LeftButtonDown();
		else PostMessage(Main.hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
		break;
	}
	case WM_LBUTTONUP:
		if (Main.CurCover != -1)
		{
			Main.Press = 0;
			RECT rc;
			rc = Main.GetRECT(Main.CurCover);
			InvalidateRect(Main.hWnd, &rc, FALSE);
		}
		if (Main.CoverEdit == 0)Main.EditUnHotKey();
		Main.Edit[Main.CoverEdit].Press = false;
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(Main.hWnd, &point);

		for (int i = 1; i <= Main.CurArea; ++i)
		{
			if (Main.InsideArea(i, point) == TRUE && (Main.Area[i].Page == 0 || Main.Area[i].Page == Main.CurWnd))
			{
				switch (i)
				{
				case 1:
					DWORD col;
					col = DoSelectColour();
					if (col == 0)break;
					green = CreateSolidBrush(col);
					GREEN = CreatePen(PS_SOLID, 2, col);
					InvalidateRect(Main.hWnd, 0, 0);
					break;
				case 2:
					if (Effect)EasterEgg(true);
					break;

				}
			}
		}
		unsigned int x;
		x = Hash(Main.GetCurInsideID());
		BUTTON_IN(x, L"P1") { Main.SetPage(1); EasterEgg(false); ShowWindow(FileList, SW_HIDE); KillTimer(Main.hWnd, 8); break; }
		BUTTON_IN(x, L"P2") { Main.SetPage(2); EasterEgg(false); ShowWindow(FileList, SW_HIDE); SetTimer(Main.hWnd, 8, 200, (TIMERPROC)TimerProc); RefreshTDstate();   break; }
		BUTTON_IN(x, L"P3") { Main.SetPage(3); EasterEgg(false); ShowWindow(FileList, SW_HIDE); KillTimer(Main.hWnd, 8); break; }
		BUTTON_IN(x, L"P4") { if (!haveInfo) UpdateInfo(), ReleaseRes(L"C:\\SAtemp\\1.JPG", IDR_JPG2, L"JPG"), haveInfo = true; Main.SetPage(4); ShowWindow(FileList, SW_HIDE); KillTimer(Main.hWnd, 8); break; }
		BUTTON_IN(x, L"P5") { Main.SetPage(5); EasterEgg(false); ShowWindow(FileList, SW_SHOW); KillTimer(Main.hWnd, 8); break; }
		BUTTON_IN(x, L"QuickSetup")
		{
			oneclick = 1 - oneclick;
			if (oneclick == 1)
			{
				RegisterHotKey(Main.hWnd, 3, NULL, VK_SCROLL);
				Main.InfoBox(L"OneOK");
				wcscpy(Main.Button[Main.GetNumbyID(L"QuickSetup")].Name, Main.GetStr(L"unQS"));
			}
			else
			{
				UnregisterHotKey(Main.hWnd, 3);
				Main.InfoBox(L"unQSOK");
				wcscpy(Main.Button[Main.GetNumbyID(L"QuickSetup")].Name, Main.GetStr(L"setQS"));
			}
			break;
		}
		BUTTON_IN(x, L"DelR3")
		{
			if (!DeleteSethc())Main.InfoBox(L"DSR3Fail"); else goto delok;
			break;
		}
		BUTTON_IN(x, L"DelR0")
		{
			if (!DeleteSethcS())Main.InfoBox(L"DSR0Fail");
			else
			{
			delok:
				Main.Button[Main.GetNumbyID(L"DelR0")].Enabled = Main.Button[Main.GetNumbyID(L"DelR3")].Enabled = false;
				wcscpy_s(Main.Button[Main.GetNumbyID(L"DelR0")].Name, Main.GetStr(L"Deleted"));
				wcscpy_s(Main.Button[Main.GetNumbyID(L"DelR3")].Name, Main.GetStr(L"Deleted"));
				SethcState = false;
				InvalidateRect(Main.hWnd, NULL, false);
			}
			break;
		}
		BUTTON_IN(x, L"SethcR0")
		{
			if (SethcInstallState == false)
			{
				ReleaseDrvFiles();
				int flag = SetupSethcS();
				if (flag == 0)Main.InfoBox(L"CSFail");//安装失败
				if (flag == 2)Main.InfoBox(L"NoSethc");//没有文件
				if (flag == 1)//成功
				{
					Main.Button[Main.GetNumbyID(L"SethcR3")].Enabled = false;
					wcscpy_s(Main.Button[Main.GetNumbyID(L"SethcR3")].Name, Main.GetStr(L"Installed"));
					wcscpy_s(Main.Button[Main.GetNumbyID(L"SethcR0")].Name, Main.GetStr(L"unQS"));
					SethcInstallState = true;
					InvalidateRect(Main.hWnd, NULL, false);
				}
			}
			else goto unsethc;
			break;
		}

		BUTTON_IN(x, L"SethcR3")
		{
			if (SethcInstallState == false)
			{
				if (AutoSetupSethc() && AutoCopyNTSD())
				{
					Main.Button[Main.GetNumbyID(L"SethcR0")].Enabled = false;
					wcscpy_s(Main.Button[Main.GetNumbyID(L"SethcR0")].Name, Main.GetStr(L"Installed"));
					wcscpy_s(Main.Button[Main.GetNumbyID(L"SethcR3")].Name, Main.GetStr(L"unQS"));
					SethcInstallState = true;
					InvalidateRect(Main.hWnd, NULL, false);
				}
			}
			else
			{
			unsethc:
				if (UninstallSethc())
				{
					wcscpy_s(Main.Button[Main.GetNumbyID(L"SethcR0")].Name, Main.GetStr(L"Ring0"));
					wcscpy_s(Main.Button[Main.GetNumbyID(L"SethcR3")].Name, Main.GetStr(L"Ring3"));
					wcscpy_s(Main.Button[Main.GetNumbyID(L"DelR0")].Name, Main.GetStr(L"Ring0"));
					wcscpy_s(Main.Button[Main.GetNumbyID(L"DelR3")].Name, Main.GetStr(L"Ring3"));
					Main.Button[Main.GetNumbyID(L"SethcR3")].Enabled = Main.Button[Main.GetNumbyID(L"SethcR0")].Enabled = Main.Button[Main.GetNumbyID(L"DelR0")].Enabled = Main.Button[Main.GetNumbyID(L"DelR3")].Enabled = true;
					SethcInstallState = false;
					InvalidateRect(Main.hWnd, NULL, false);
				}
			}
			break;
		}
		BUTTON_IN(x, L"hookS")
		{
			KillProcess(L"hoo");
			if (!RunHOOK())Main.InfoBox(L"OneFail");
			else
			{
				wcscpy_s(Main.Button[Main.GetNumbyID(L"hookS")].Name, Main.GetStr(L"Installed"));
				wcscpy_s(Main.Button[Main.GetNumbyID(L"hookU")].Name, Main.GetStr(L"unQS"));
				Main.Button[Main.GetNumbyID(L"hookS")].Enabled = false;
				Main.Button[Main.GetNumbyID(L"hookU")].Enabled = true;
				InvalidateRect(Main.hWnd, NULL, false);
			}
			break;
		}
		BUTTON_IN(x, L"hookU")
		{
			KillProcess(L"hoo");
			wcscpy_s(Main.Button[Main.GetNumbyID(L"hookS")].Name, Main.GetStr(L"Setup"));
			wcscpy_s(Main.Button[Main.GetNumbyID(L"hookU")].Name, Main.GetStr(L"Uned"));
			Main.Button[Main.GetNumbyID(L"hookS")].Enabled = true;
			Main.Button[Main.GetNumbyID(L"hookU")].Enabled = false;
			InvalidateRect(Main.hWnd, NULL, false);
			break;
		}
		BUTTON_IN(x, L"runinVD")
		{
			STARTUPINFO si = { 0 };
			si.cb = sizeof(si);
			si.lpDesktop = szVDesk;
			RunEXE(Main.Edit[Main.GetNumByIDe(L"E_runinVD")].str, NULL, &si);
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
				Main.InfoBox(L"ACFail");
				RegCloseKey(hKey);
				break;
			}
			int a = _wtoi(tmp);
			ret = RegSetValueEx(hKey, L"ChannelId", 0, REG_DWORD, (const BYTE*)& a, sizeof(int));
			if (ret != 0)
			{
				Main.InfoBox(L"ACUKE");
				RegCloseKey(hKey);
				break;
			}
			RegCloseKey(hKey);
			Main.InfoBox(L"ACOK");
			break;
		}
		BUTTON_IN(x, L"ClearPass") { AutoClearPassWd(); break; }
		BUTTON_IN(x, L"ViewPass") { AutoViewPass(); break; }
		BUTTON_IN(x, L"CP1") { ChangePasswordEx(Main.Edit[Main.GetNumByIDe(L"E_CP")].str, 1); break; }
		BUTTON_IN(x, L"CP2") { ChangePasswordEx(Main.Edit[Main.GetNumByIDe(L"E_CP")].str, 2); break; }
		BUTTON_IN(x, L"kill-TD") { KillProcess(L"stu"); break; }
		BUTTON_IN(x, L"re-TD") { ReopenTD(); break; }
		BUTTON_IN(x, L"windows.ex")
		{
			if (FC == TRUE)
				CatchWnd.RegisterClassW(hInst, CatchProc, CatchWindow),
				FC = FALSE;//First start CatchWnd

			if (CatchWnd.hWnd != 0)DestroyWindow(CatchWnd.hWnd);
			CatchWnd.hWnd = CreateWindowW(CatchWindow, CatchWnd.GetStr(L"Title"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, nullptr, nullptr, hInst, nullptr);
			CreateCaret(CatchWnd.hWnd, NULL, 1, 20);
			ShowWindow(CatchWnd.hWnd, SW_SHOW);
			UpdateWindow(CatchWnd.hWnd);
			break;
		}

		BUTTON_IN(x, L"ANTI-") { DeleteShutdown(); break; }
		BUTTON_IN(x, L"desktop")
		{
			wchar_t tmp[351] = { L"psexec.exe -x -i -s -d \"" },tmp2[301];
			wcscpy_s(tmp2, Path);
			wcscat_s(tmp2, L"psexec.exe");
			if (GetFileAttributes(tmp2) == -1)ReleaseRes(tmp2, IDR_JPG6, L"JPG");
			wcscat_s(tmp, Name);
			wcscat_s(tmp, L"\" -top");
			DWORD word = 1; HKEY hKey; LONG ret;
			ret = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Sysinternals\\PsExec", 0, KEY_SET_VALUE, &hKey);
			ret = RegSetValueEx(hKey, L"EulaAccepted", 0, REG_DWORD, (const BYTE*)& word, sizeof(DWORD));
			if (!RunEXE(tmp, CREATE_NO_WINDOW, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		BUTTON_IN(x, L"auto-5") { KillTop(); break; }
		BUTTON_IN(x, L"viewfile") { openfile(); break; }
		BUTTON_IN(x, L"folder") { BrowseFolder(); break; }
		BUTTON_IN(x, L"TI")
		{
			wchar_t tmp[1001];
			wcscpy_s(tmp, Main.Edit[Main.GetNumByIDe(L"E_View")].str);
			AutoDelete(tmp);
			break;
		}
		BUTTON_IN(x, L"TA") \
		{
			wchar_t tmp[301], tmp2[321];
			wcscpy_s(tmp, Path);
			wcscat_s(tmp, L"deleter\\");
			CreateDirectory(tmp, NULL);
			wcscpy_s(tmp2, tmp);
			wcscat_s(tmp2, L"DeleteFile.sys");
			ReleaseRes(tmp2, IDR_JPG16, L"JPG");
			wcscpy_s(tmp2, tmp);
			wcscat_s(tmp2, L"DeleteFile_x64.sys");
			ReleaseRes(tmp2, IDR_JPG17, L"JPG");
			wcscpy_s(tmp2, tmp);
			wcscat_s(tmp2, L"DrvDelFile.exe");
			ReleaseRes(tmp2, IDR_JPG18, L"JPG");
			WinExec("deleter\\DrvDelFile.exe", SW_SHOW); 
			break; 
		}
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

				wcscpy_s(Main.Button[Main.CurCover].Name, Main.GetStr(L"Gamee"));
				GameMode = 1;
				if (!lock)lock = true, CreateThread(NULL, 0, GameThread, 0, 0, NULL); else GameMode = 0, wcscpy_s(Main.Button[Main.CurCover].Name, Main.GetStr(L"Games"));
			}
			else
			{
				GameMode = 0;
				wcscpy_s(Main.Button[Main.CurCover].Name, Main.GetStr(L"Games"));
				if (!lock)lock = true, CreateThread(NULL, 0, GameThread, 0, 0, NULL); else GameMode = 1, wcscpy_s(Main.Button[Main.CurCover].Name, Main.GetStr(L"Gamee"));
			}
			break;
		}
		BUTTON_IN(x, L"ARP")
		{
			static wchar_t tmp[] = L"arp\\arp.exe";
			CopyFile(L"arp\\npf.sys", L"C:\\Windows\\System32\\drivers\\npf.sys", FALSE);
			CopyFile(L"arp\\npf.sys", L"C:\\Windows\\SysNative\\drivers\\npf.sys", FALSE);
			CopyFile(L"arp\\npf.sys", L"C:\\Windows\\SysWOW64\\drivers\\npf.sys", FALSE);
			if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			break;

		}
		BUTTON_IN(x, L"SuperCMD")
		{
			wchar_t tmp[] = L"psexec.exe -i -s -d cmd.exe",tmp2[301];
			wcscpy_s(tmp2, Path);
			wcscat_s(tmp2, L"psexec.exe");
			if (GetFileAttributes(tmp2) == -1)ReleaseRes(tmp2, IDR_JPG6, L"JPG");
			DWORD word = 1;
			HKEY hKey;
			LONG ret;
			ret = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Sysinternals\\PsExec", 0, KEY_SET_VALUE, &hKey);
			ret = RegSetValueEx(hKey, L"EulaAccepted", 0, REG_DWORD, (const BYTE*)& word, sizeof(DWORD));
			if (!RunEXE(tmp, CREATE_NO_WINDOW, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		BUTTON_IN(x, L"Killer")
		{
			ReleaseDrvFiles();
			UnloadNTDriver(L"360");
			bool flag;
			Main.InfoBox(L"360Start");
			if (Bit == 32)
				flag = LoadNTDriver(L"360", L"x32\\360.sys");
			else
				flag = LoadNTDriver(L"360", L"x64\\360.sys");
			if (flag == false)
				Main.InfoBox(L"360Fail");
			break;
		}
		BUTTON_IN(x, L"more.txt")
		{
			wchar_t tmp[321] = L"Notepad ",tmp2[301];
			wcscpy_s(tmp2, Path);
			wcscat_s(tmp2, L"关于&帮助.txt");
			ReleaseRes(tmp2, IDR_JPG1, L"JPG");
			wcscat_s(tmp, tmp2);
			if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		BUTTON_IN(x, L"sysinfo")
		{
			wchar_t tmp[] = L"C:\\Windows\\System32\\Msinfo32.exe";
			if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		BUTTON_IN(x, L"hidest") { HideState = 5; ShowWindow(Main.hWnd, SW_HIDE);	break; }
		BUTTON_IN(x, L"upgrade")
		{
			if (FU == TRUE)
				UpWnd.RegisterClassW(hInst, UpGProc, UpWindow),
				FU = FALSE;

			if (UpWnd.hWnd != 0)DestroyWindow(UpWnd.hWnd);
			wchar_t tmp[255];
			for (int i = 0; i < numFiles; ++i)
			{
				wcscpy_s(tmp, Path);
				wcscat_s(tmp, FileName[i]);
				if (GetFileAttributes(tmp) != -1)FileExist[i] = TRUE; else FileExist[i] = FALSE;
				UpWnd.Check[i + 1].Value = FileExist[i];
			}
			UpWnd.hWnd = CreateWindowW(UpWindow, UpWnd.GetStr(L"Title"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, 200, 200, 210, 440, nullptr, nullptr, hInst, nullptr);
			ShowWindow(UpWnd.hWnd, SW_SHOW);
			UpdateWindow(UpWnd.hWnd);
			break;
		}

		BUTTON_IN(x, L"Game1") {
			if (GameExist[0])RunEXE(GameName[0], NULL, nullptr);
			else { int typ = 1; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }break;
		}
		BUTTON_IN(x, L"Game2") {
			if (GameExist[1])RunEXE(GameName[1], NULL, nullptr);
			else { int typ = 2; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }break;
		}
		BUTTON_IN(x, L"Game3") {
			if (GameExist[2])RunEXE(GameName[2], NULL, nullptr);
			else { int typ = 3; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }break;
		}
		BUTTON_IN(x, L"Game4") {
			if (GameExist[3])RunEXE(GameName[3], NULL, nullptr);
			else { int typ = 4; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }break;
		}
		BUTTON_IN(x, L"Game5") {
			if (GameExist[4])RunEXE(GameName[4], NULL, nullptr);
			else { int typ = 5; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }break;
		}
		BUTTON_IN(x, L"Game6") {
			if (GameExist[5])RunEXE(GameName[5], NULL, nullptr);
			else { int typ = 6; CreateThread(NULL, 0, DownloadThread, &typ, 0, NULL); }break;
		}

		BUTTON_IN(x, L"Close") { PostQuitMessage(0); }

		for (int i = 1; i <= Main.CurCheck; ++i)//分派check事件
		{
			if (Main.Check[i].Page == 0 || Main.Check[i].Page == Main.CurWnd)
			{
				int result;
				result = Main.InsideCheck(i, point);
				if (result != 0)
				{
					if (!Main.Check[i].Value)
					{//未选中->选中
						switch (i)
						{
						case 1:case 2: {//伪装工具条
							wchar_t tmp[301];
							wcscpy_s(tmp, Path);
							wcscat_s(tmp, L"cheat\\");
							CreateDirectory(tmp, NULL);
							
							
							if (i == 1)wcscat_s(tmp, L"old.exe"), ReleaseRes(tmp, IDR_JPG19, L"JPG"); else wcscat_s(tmp, L"new.exe"), ReleaseRes(tmp, IDR_JPG20, L"JPG");
							
							if (!RunEXE(tmp, NULL, NULL))Main.InfoBox(L"StartFail"), Main.Check[i].Value = true;
							break; }
						case 3: {
							NOTIFYICONDATA tnd;//伪装图标
							tnd.cbSize = sizeof(NOTIFYICONDATA);
							tnd.hWnd = Main.hWnd;
							tnd.uID = IDR_MAINFRAME;
							tnd.uFlags = 2 | 1 | 4;
							tnd.uCallbackMessage = 34;
							tnd.hIcon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_TD));
							wcscpy_s(tnd.szTip, Main.GetStr(L"tnd"));
							if (!Shell_NotifyIcon(NIM_ADD, &tnd))Main.InfoBox(L"StartFail"), Main.Check[3].Value = true;
							break; }
						case 4: {TOP = TRUE; CreateThread(NULL, 0, TopThread, NULL, 0, NULL); break; }//置顶
						case 5: {if (Admin == 0)Main.InfoBox(L"BSODAsk"); RegisterHotKey(Main.hWnd, 4, MOD_CONTROL, 'R'); break; }//蓝屏
						case 6: {RegisterHotKey(Main.hWnd, 5, MOD_CONTROL, 'T'); break; }//重启
						case 7: {//截图伪装
							if (FS == TRUE)InitScreen(), FS = FALSE;
							RegisterHotKey(Main.hWnd, 6, MOD_CONTROL | MOD_ALT, 'P');
							break; }
						case 8: {SetTimer(hWnd, 1, 500, (TIMERPROC)TimerProc); break; }//连续结束进程
						case 9:case 10: {SetTimer(hWnd, 7, 100, (TIMERPROC)TimerProc); break; }//禁止键盘（鼠标）钩子
						case 11: {RegMouseKey(); break; }//键盘控制鼠标
						case 12: {//低画质
							Effect = false;
							ShowWindow(Cshadow.m_hWnd, SW_HIDE);
							Main.ButtonEffect = false;
							KillTimer(Main.hWnd, 5);
							break; }
						case 13: {//缩小/放大
							Main.SetDPI(0.75);
							SetWindowPos(FileList, 0, (int)(180 * Main.DPI), (int)(420 * Main.DPI), (int)(265 * Main.DPI), (int)(110 * Main.DPI), NULL);
							SendMessage(FileList, WM_SETFONT, WPARAM(Main.DefFont), 0);
							break; }
						}
					}
					else
					{
						switch (i)
						{
						case 1: {KillProcess(L"old"); break; }
						case 2: {KillProcess(L"new"); break; }
						case 3: {
							NOTIFYICONDATA tnd;
							tnd.cbSize = sizeof(NOTIFYICONDATA);
							tnd.hWnd = Main.hWnd;
							tnd.uID = IDR_MAINFRAME;
							Shell_NotifyIcon(NIM_DELETE, &tnd);
							break; }
						case 4: {TOP = FALSE; break; }
						case 5:case 6:case 7: {UnregisterHotKey(Main.hWnd, i - 1); break; }
						case 8: {KillTimer(hWnd, 1); break; }
						case 9:case 10: {KillTimer(hWnd, 7); break; }
						case 11: {UnMouseKey(); break; }
						case 12: {
							Effect = true;
							Main.ButtonEffect = true;
							SetTimer(Main.hWnd, 5, 33, (TIMERPROC)TimerProc);
							ShowWindow(Cshadow.m_hWnd, SW_SHOW); }
								 break;
						case 13: {
							Main.SetDPI(1.5);
							SetWindowPos(FileList, 0, (int)(180 * Main.DPI), (int)(420 * Main.DPI), (int)(265 * Main.DPI), (int)(110 * Main.DPI), NULL);
							SendMessage(FileList, WM_SETFONT, WPARAM(Main.DefFont), 0);
							break;
						}
						}
					}
					RECT rc = Main.GetRECTc(i);
					Main.Check[i].Value = 1 - Main.Check[i].Value;
					Main.Readd(3, i);
					Main.Redraw(&rc);
				}
			}
		}
		Main.Timer = (DWORD)time(0);
		Main.DestroyExp();
		break;
	case WM_MOUSEMOVE: {Main.MouseMove(); break; }
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
		Main.EditCHAR((wchar_t)wParam);
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
		sdc = BeginPaint(hWnd, &ps);
		DEVMODE curDevMode;
		curDevMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMode);
		if (!BlackBoard)
		{
			HBITMAP bitmap;
			bitmap = (HBITMAP)LoadImage(hInst, L"C:\\SAtemp\\ScreenShot.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

			SelectObject(pdc, bitmap);

			SetStretchBltMode(sdc, HALFTONE);
			StretchBlt(sdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), pdc, 0, 0, curDevMode.dmPelsWidth, curDevMode.dmPelsHeight, SRCCOPY);
			DeleteObject(bitmap);
		}
		else
		{
			//s(0);
			HBITMAP tb = CreateCompatibleBitmap(sdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			SelectObject(pdc, tb);
			SelectObject(pdc, BLACK);
			SelectObject(pdc, BlackBrush);
			Rectangle(pdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			SetStretchBltMode(sdc, HALFTONE);
			StretchBlt(sdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), pdc, 0, 0, curDevMode.dmPelsWidth, curDevMode.dmPelsHeight, SRCCOPY);
			DeleteObject(tb);
		}
		EndPaint(hWnd, &ps);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
HDC bdc;
int code[26] = { 0x1fc9e7f,0x1053641,0x175f65d,0x174e05d,0x175075d,0x105a341,0x1fd557f,0x19500,0x1a65d76,0x17a6dc1,0x18ec493,0x1681960,
0x1471bcb,0x2255ed,0x17c7475,0xea388a,0x18fd1fc,0x1f51d,0x1fd8b53,0x104d51d,0x1745df2,0x1751d14,0x174ce1d,0x1056dc8,0x1fd9ba3
};
wchar_t words[10][320] =
{ L"A problem has been detected and windows has been shut down to prevent damage to your computer. ",
L"IRQL_NOT_LESS_OR_EQUAL ",
L"If this is the first time you've seen this Stop error screen, restart your computer. If this screen appears again, follow these steps: "
,L"Check to make sure any new hareware of software is properly installed. If this is new installation, ask your hardware or software manufacturer for any windows updates you might need. "
,L"If problems continue,disable or remove any newly installed hardware or software. Disable BIOS memory options such as caching or shadowing. If you need to use Safe Mode to remove or disable components, restart your computer, press F8 to select Advanced Startup Options, and then select Safe Mode. ",
L"Technical information: ",L"*** STOP: 0x0000000A (0x00000000,0xD0000002, 0x00000001,0x8082C582)",L"*** wdf01000.sys - Address 97C141AC base at 97C0E000, DateStamp 4fd91f51 " };
LRESULT CALLBACK BSODProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		ldc = GetDC(hWnd);
		bdc = CreateCompatibleDC(ldc);
		lBmp = CreateCompatibleBitmap(ldc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		SelectObject(bdc, lBmp);
		ReleaseDC(hWnd, ldc);
		break;
	case WM_PAINT:
	{
		ldc = BeginPaint(hWnd, &ps);
		if (Main.Check[15].Value == false)
		{
			SetTextColor(bdc, RGB(255, 255, 255));
			SelectObject(bdc, BSODPen);
			SelectObject(bdc, BSODBrush);
			SetBkMode(bdc, 1);
			HFONT A = CreateFontW(40, 15, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑")),
				B = CreateFontW(140, 70, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Century Gothic")),
				C = CreateFontW(26, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑"));
			SelectObject(bdc, B);
			Rectangle(bdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			TextOut(bdc, 180, 120, L":(", 2);
			SelectObject(bdc, A);
			TextOut(bdc, 180, 290, Main.GetStr(L"BSOD1"), wcslen(Main.GetStr(L"BSOD1")));
			wchar_t tmp[21];
			if (BSODstate > 20)BSODstate = 20;
			_itow_s(int(BSODstate * 2.5), tmp, 10);
			wcscat_s(tmp, Main.GetStr(L"BSOD7"));
			TextOut(bdc, 180, 360, tmp, wcslen(tmp));
			SelectObject(bdc, WhiteBrush);
			Rectangle(bdc, 180, 440, 180 + 141, 440 + 141);
			SelectObject(bdc, BSODBrush);
			for (int i = 0; i < 25; ++i)
				for (int j = 0; j < 25; ++j)if ((code[i] >> (24 - j)) % 2 == 1)Rectangle(bdc, 188 + 5 * j, 448 + 5 * i, 188 + 5 + 5 * j, 448 + 5 + 5 * i);
			SelectObject(bdc, C);

			TextOut(bdc, 345, 440, Main.GetStr(L"BSOD2"), wcslen(Main.GetStr(L"BSOD2")));
			TextOut(bdc, 345, 470, Main.GetStr(L"BSOD3"), wcslen(Main.GetStr(L"BSOD3")));
			//if (GetSystemMetrics(SM_CYSCREEN) < 700)return 0;
			TextOut(bdc, 345, 520, Main.GetStr(L"BSOD4"), wcslen(Main.GetStr(L"BSOD4")));
			TextOut(bdc, 345, 550, Main.GetStr(L"BSOD5"), wcslen(Main.GetStr(L"BSOD5")));
			TextOut(bdc, 345, 580, Main.GetStr(L"BSOD6"), wcslen(Main.GetStr(L"BSOD6")));
			BitBlt(ldc, 0, 0, GetDeviceCaps(GetDC(NULL), HORZRES), GetDeviceCaps(GetDC(NULL), VERTRES), bdc, 0, 0, SRCCOPY);
			DeleteObject(A); DeleteObject(B); DeleteObject(C);
		}
		else
		{
			int ybegin = 20, xbegin = 2, xmax = 480, ymax = 360, left, right, s = 7;//GetSystemMetrics(SM_CYSCREEN)

			SetTextColor(bdc, RGB(255, 255, 255));
			SelectObject(bdc, CreateSolidBrush(RGB(1, 0, 0x80)));
			SelectObject(bdc, CreatePen(PS_SOLID, 1, RGB(1, 0, 0x80)));
			SetBkMode(bdc, 1);
			HFONT A = CreateFontW(14, 7, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Lucida Console"));
			SelectObject(bdc, A);
			Rectangle(bdc, 0, 0, 480, 360);
			for (int i = 0; i < 9; ++i)
			{
				if (ymax - 50 <= ybegin)break;
				left = right = 0;
				xbegin = 2;

				while (1)
				{
					while (words[i][right] != ' ' && words[i][right] != '\0')right++;
					if (words[i][right] == '\0')break;
					SIZE se;
					GetTextExtentPoint32(bdc, &words[i][left], right - left + 1, &se);
					if (se.cx + xbegin <= xmax - 2 * s)
					{
						TextOut(bdc, xbegin, ybegin, &words[i][left], right - left + 1);
						xbegin += se.cx;
						left = right + 1;
						right = left;
					}
					else
					{
						xbegin = 2;
						ybegin += s * 1.9;
						right = left;
					}
				}
				ybegin += (int)(s * 3);
			}
			if (BSODstate >= 4)TextOut(bdc, 2, ybegin, L"Collecting data for crash dump...", 33), ybegin += s * 2;
			if (BSODstate >= 10)TextOut(bdc, 2, ybegin, L"Initializing disk for crash dump...", 35);
			StretchBlt(ldc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), bdc, 0, 0, 480, 360, SRCCOPY);
		}
		EndPaint(hWnd, &ps);
		break;
	}
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
	{
		CatchWnd.tdc = GetDC(CatchWnd.hWnd);
		HDC h = CreateCompatibleDC(CatchWnd.tdc);
		CatchWnd.Bitmap = CreateCompatibleBitmap(CatchWnd.tdc, GetDeviceCaps(GetDC(NULL), HORZRES), GetDeviceCaps(GetDC(NULL), VERTRES));
		SelectObject(h, CatchWnd.Bitmap);
		CatchWnd.SetHDC(h);
		ReleaseDC(CatchWnd.hWnd, CatchWnd.tdc);
	}
	case WM_CLOSE:
		KillTimer(hWnd, 4);
		ReturnWindows();
		DestroyWindow(CatchWnd.hWnd);
		CatchWnd.hWnd = 0;
		break;
	case WM_PAINT:
		CatchWnd.tdc = BeginPaint(CatchWnd.hWnd, &ps);
		if (tdhcur == 0)
		{
			SelectObject(CatchWnd.hdc, WhiteBrush);//白色背景
			SelectObject(CatchWnd.hdc, White);
			Rectangle(CatchWnd.hdc, 0, 0, GetDeviceCaps(GetDC(NULL), HORZRES), GetDeviceCaps(GetDC(NULL), VERTRES));

			CatchWnd.DrawEVERYTHING();
			wchar_t tmp1[50], tmp2[50];
			wcscpy_s(tmp1, CatchWnd.GetStr(L"Eat1"));
			_itow_s(EatList.size(), tmp2, 10);
			wcscat_s(tmp1, tmp2);
			wcscat_s(tmp1, CatchWnd.GetStr(L"Eat2"));
			TextOutW(CatchWnd.hdc, 20, 165, tmp1, (int)wcslen(tmp1));
			BitBlt(CatchWnd.tdc, 0, 0, GetDeviceCaps(GetDC(NULL), HORZRES), GetDeviceCaps(GetDC(NULL), VERTRES), CatchWnd.hdc, 0, 0, SRCCOPY);
		}
		if (tdhcur != 0)
		{

			HDC hdctd = GetDC(tdh[displaycur]);
			RECT rc, rc2;
			int left, width, top, height;

			GetClientRect(tdh[displaycur], &rc);
			GetClientRect(CatchWnd.hWnd, &rc2);

			if ((rc2.right - rc2.left) == 0 || (rc2.bottom - rc2.top) == 0 || (rc.right - rc.left) == 0 || (rc.bottom - rc.top) == 0)break;
			const double wh1 = (double)(rc.right - rc.left) / (double)(rc.bottom - rc.top),
				wh2 = (double)(rc2.right - rc2.left) / (double)(rc2.bottom - rc2.top);
			if (wh1 > wh2)
			{
				left = 0;
				width = rc2.right - rc2.left;
				top = (int)((rc2.bottom - rc2.top) / 2 - (rc2.right - rc2.left) / wh1 / 2);
				height = (int)((rc2.right - rc2.left) / wh1);
			}
			else
			{
				top = 0;
				height = rc2.bottom - rc2.top;
				left = (int)((rc2.right - rc2.left) / 2 - (rc2.bottom - rc2.top) * wh1 / 2.0);
				width = (int)((rc2.bottom - rc2.top) * wh1);
			}

			SetStretchBltMode(CatchWnd.tdc, HALFTONE);
			StretchBlt(CatchWnd.tdc, left, top, width, height, hdctd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SRCCOPY);
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
			const RECT rc = CatchWnd.GetRECT(CatchWnd.CurCover);
			InvalidateRect(CatchWnd.hWnd, &rc, FALSE);
		}

		CatchWnd.Edit[CatchWnd.CoverEdit].Press = false;
		for (int i = 1; i <= CatchWnd.CurButton; ++i)
		{
			if (CatchWnd.InsideButton(i, point))
			{
				switch (i)
				{
				case 1:
					timerleft = _wtoi(CatchWnd.Edit[CatchWnd.GetNumByIDe(L"E_Delay")].str);
					CatchWnd.SetStr(CatchWnd.Button[1].Name, L"back");
					SetTimer(CatchWnd.hWnd, 2, 1000, (TIMERPROC)TimerProc);
					break;
				case 2:
				{
					wchar_t x[1001];
					wcscpy_s(x, CatchWnd.Edit[CatchWnd.GetNumByIDe(L"E_Pname")].str);
					tdhcur = 0;
					FindTDhwnd(x);
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
		CatchWnd.EditCHAR((wchar_t)wParam);
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
	case WM_KILLFOCUS: {
		for (int i = 513; i < 517; ++i)UnregisterHotKey(hWnd, i);
		CatchWnd.EditUnHotKey();
		break; }
	case WM_HOTKEY: {
		if (wParam == 513)displaycur--;
		if (wParam == 514)displaycur++;
		if (wParam == 515) { tdhcur = 0, KillTimer(hWnd, 6), InvalidateRect(hWnd, NULL, TRUE); for (int i = 513; i < 517; ++i)UnregisterHotKey(hWnd, i); }
		if (wParam == 516)
			if ((GetWindowLongW(tdh[displaycur], GWL_STYLE)& WS_VISIBLE) != 0)ShowWindow(tdh[displaycur], SW_HIDE); else ShowWindow(tdh[displaycur], SW_SHOW);
		if (displaycur == 0)displaycur = tdhcur;
		if (displaycur > tdhcur)displaycur = 1;
		CatchWnd.EditHotKey(wParam);
		break; }
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK UpGProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
	{
		UpWnd.tdc = GetDC(hWnd);
		HDC h = CreateCompatibleDC(UpWnd.tdc);
		UpWnd.Bitmap = CreateCompatibleBitmap(UpWnd.tdc, 200, 500);
		SelectObject(h, UpWnd.Bitmap);
		UpWnd.SetHDC(h);
		ReleaseDC(UpWnd.hWnd, UpWnd.tdc);
	}
	case WM_PAINT:
		UpWnd.tdc = BeginPaint(hWnd, &ps);
		SelectObject(UpWnd.hdc, WhiteBrush);//白色背景
		SelectObject(UpWnd.hdc, White);
		Rectangle(UpWnd.hdc, 0, 0, 200, 500);
		UpWnd.DrawChecks(0);
		if (FDtot != 0)
		{
			wchar_t tmp[34], tmp2[101];
			if (FDtot == FDcur) { wcscpy_s(tmp2, Main.GetStr(L"Loaded")); FDtot = FDcur = 0; goto ok; }
			wcscpy_s(tmp2, Main.GetStr(L"Loading"));
			_itow_s(FDcur, tmp, 10);
			wcscat_s(tmp2, tmp);
			wcscat_s(tmp2, L"/");
			_itow_s(FDtot, tmp, 10);
			wcscat_s(tmp2, tmp);
		ok:
			TextOut(UpWnd.hdc, 20, 370, tmp2, wcslen(tmp2));
		}
		BitBlt(UpWnd.tdc, 0, 0, 200, 500, UpWnd.hdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		UpWnd.LeftButtonDown();
		break;
	case WM_LBUTTONUP:
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);

		if (UpWnd.CoverCheck != -1)
		{
			UpWnd.Press = 0;
			const RECT rc = UpWnd.GetRECTc(UpWnd.CoverCheck);
			InvalidateRect(UpWnd.hWnd, &rc, FALSE);
		}

		for (int i = 1; i <= UpWnd.CurCheck; ++i)
		{
			if (UpWnd.Check[i].Page == 0 || UpWnd.Check[i].Page == UpWnd.CurWnd)
			{
				int result;
				result = UpWnd.InsideCheck(i, point);
				if (result != 0)
				{
					if (!UpWnd.Check[i].Value)
					{
						int typ = i;
						CreateThread(NULL, 0, DownloadThreadUp, &typ, 0, NULL);
						InvalidateRect(UpWnd.hWnd, NULL, FALSE);
					}
					else
					{
						wchar_t tmp[34], tmp2[501];
						wcscpy_s(tmp, FileName[i - 1]);
						if (wcsstr(tmp, L"\\") != 0 && i != 4 && i != 5)(_tcsrchr(tmp, _T('\\')))[1] = 0;
						wcscpy_s(tmp2, Path);
						wcscat_s(tmp2, tmp);
						SearchTool(tmp2, 3);
						DeleteFile(tmp2);
					}
					UpWnd.Check[i].Value = 1 - UpWnd.Check[i].Value;
				}
			}
		}
		break;
	case WM_MOUSEMOVE:UpWnd.MouseMove(); break;
	case WM_CLOSE:UpWnd.hWnd = 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM InitScreen()//注册窗口类
{
	WNDCLASSEXW scr = { 0 };
	scr.cbSize = sizeof(WNDCLASSEX);
	scr.lpfnWndProc = ScreenProc;
	scr.hInstance = hInst;
	scr.hCursor = LoadCursor(nullptr, IDC_ARROW);
	scr.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	scr.lpszClassName = ScreenWindow;
	return RegisterClassExW(&scr);
}
ATOM InitBSOD()
{
	WNDCLASSEXW scr = { 0 };
	scr.cbSize = sizeof(WNDCLASSEX);
	scr.lpfnWndProc = BSODProc;
	scr.hInstance = hInst;
	scr.hCursor = LoadCursor(nullptr, IDC_ARROW);
	scr.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	scr.lpszClassName = BSODWindow;
	return RegisterClassExW(&scr);
}