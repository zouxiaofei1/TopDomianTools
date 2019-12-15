﻿//这是TopDomainTools工程源代码的主文件
//by zouxiaofei1 2015 - 2020

//头文件
#include "stdafx.h"
#include "GUI.h"
#include "TestFunctions.h"
#include "Actions.h"

#pragma comment(lib, "urlmon.lib")//下载文件用的Lib
#pragma comment(lib,"Imm32.lib")//自定义输入法位置用的Lib
#pragma comment(lib, "ws2_32.lib")//Winsock API 库
#pragma comment(lib, "netapi32.lib")//也是Winsock API 库

//部分(重要)函数的前向声明
BOOL				InitInstance(HINSTANCE, int);//初始化
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);//主窗口
LRESULT CALLBACK	CatchProc(HWND, UINT, WPARAM, LPARAM);//"捕捉窗口"的窗口
LRESULT CALLBACK	ScreenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//截图伪装窗口
LRESULT CALLBACK	BSODProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//蓝屏伪装窗口
LRESULT CALLBACK	FakeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//伪装工具条窗口
void	CALLBACK	TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);//计时器
bool SearchTool(LPCWSTR lpPath, int type);

//自己定义的全局变量 有点乱


//“全局”的全局变量
HINSTANCE hInst;//当前实例，CreateWindow&LoadIcon时需要
const wchar_t szWindowClass[] = L"GUI",			/*主窗口类名*/		CatchWindow[] = L"CatchWindow";/*第二窗口类名*/
const wchar_t ScreenWindow[] = L"ScreenWindow",	/*截图伪装窗口类名*/BSODWindow[] = L"BSODWindow";/*伪装蓝屏窗口类名*/
const wchar_t FakeWindow[] = L"FakeToolBar";	/*伪装工具条窗口类名*/
BOOL FC = TRUE, FS = TRUE, FB = TRUE, FF = TRUE;//是否第一次启动窗口并注册类 C=catch S=screen B=BSOD F=FakeToolBar
BOOL Admin;//是否拥有管理员权限
int Bit;//系统位数 32 34 64
BOOL slient = FALSE;//是否命令行
HANDLE hdlWrite;//命令行窗口的句柄
wchar_t Path[MY_MAX_PATH], Name[MY_MAX_PATH], TempPath[] = L"C:\\SAtemp\\";//程序路径 and 路径+程序名 amd 缓存路径(Path最后有"\")

//和绘图有关的全局变量
HBRUSH DBlueBrush, LBlueBrush, WhiteBrush, NormalBlueBrush, TitleBrush, LGreyBrush, YellowBrush, DGreyBrush, BSODBrush, BlackBrush;//各色笔刷
HPEN YellowPen, BlackPen, WhitePen, TitlePen, CheckGreenPen, NormalGreyPen, NormalBluePen, DBluePen, LBluePen, BSODPen;//各色笔
HBITMAP hBmp, lBmp;//主窗口hbmp & 蓝屏窗口hbmp
HDC sdc, pdc;//截图伪装窗口dc
HDC ldc, bdc;//蓝屏伪装窗口dc
HDC fdc, gdc;//工具条伪装窗口dc
HWND FakeWnd;//工具条伪装窗口hwnd
HBITMAP FakeBitmap;//工具条伪装窗口Bitmap
//不使用CC的窗口只能用这些奇怪的名字(待遇不公= =)
BOOL Effect = TRUE;//特效开关
COLORREF crCustColors[16];//颜色选择中的自定义颜色

//第一页的全局变量

BOOL OneClick;//一键安装状态
wchar_t SethcPath[MY_MAX_PATH];//Sethc路径
wchar_t TDPath[MY_MAX_PATH], TDName[MY_MAX_PATH] = L"StudentMain.exe";//极域路径 and 默认极域进程名
bool TDsearched = false;//是否已经寻找过极域?
bool SethcState = true;//System32目录下sethc是否存在
bool SethcInstallState = false;//Sethc方案状态
bool HookState = false;//全局键盘钩子方案状态
BOOL FirstFlag;//是否在这台电脑上第一次运行?(根据sethc是否备份过检测)
wchar_t ExplorerPath[] = L"C:\\windows\\explorer";//符合系统位数的explorer路径
HDESK hVirtualDesk, hCurrentDesk, defaultDesk;//虚拟桌面 & 当前桌面 & 默认桌面
wchar_t szVDesk[] = L"TDTdesk", fBSODdesk[] = L"TDTBSOD";//虚拟桌面 & 蓝屏伪装窗口桌面的名称

//第二、三页的全局变量
bool GameMode;//游戏模式是否打开?
constexpr int numGames = 6;// (游戏)数
BOOL GameExist[numGames + 1];//标记的文件是否存在?
wchar_t GameName[numGames + 1][25] = { L"Games\\xiaofei.exe", L"Games\\fly.exe",L"Games\\2048.exe",L"Games\\block.exe", \
L"Games\\1.exe" , L"Games\\chess.exe" };//(游戏)名
bool GameLock = false;//Game按钮锁定
const wchar_t GitGame[] = L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Games/";//游戏存储库目录

DWORD TDPID;//记录极域程序PID
bool TDProcess;//极域是否在运行
bool FakeNew;//显示的是否是新版本的伪装工具条
DWORD FakeTimer;//记录伪装工具条缩回时间的变量
bool FakenewUp;//伪装工具条是否缩回 - 默认为false
bool FakeLock = false;//伪装工具条窗口伸缩线程锁
bool FileDeleting;//是否在删除文件
HANDLE DeleteFileHandle;//文件删除驱动句柄

//第四页的全局变量
HBITMAP hZXFBitmap, hZXFsign;//头像 & 签名两个图片句柄
int EasterEggState;//CopyLeft文字循环状态
bool EasterEggFlag = false;//CopyLeft是否显示
wchar_t EasterEggStr[11][8] = { L"Answer",L"Left" ,L"Left",L"Right",\
L"Down",L"Up",L"In",L"On",L"Back",L"Front",L"Teacher" };//滚动显示的字符
bool InfoChecked = false;//是否已经检查过系统信息

//第五页的全局变量
int ScreenState;//截图伪装状态 1 = 截图 2 = 显示
HHOOK KeyboardHook, MouseHook;//键盘、鼠标钩子
BOOL HideState;//窗口是否隐藏
HWND FileList;//语言选择hWnd
BOOL TOP;//是否置顶
BOOL BlackBoard;//是否用截图窗口显示黑屏(伪装蓝屏用) - 默认为false
wchar_t CurrentLanguage[351];//当前加载的语言文件的路径

//捕捉窗口 & 伪装蓝屏窗体的全局变量
HWND CatchWnd;//捕捉窗口的窗口句柄
HWND tdhw, tdhw2, tdhwscr;//极域广播窗口子窗口hWnd & 极域广播窗口hWnd
int timerleft = -2;//捕捉窗口倒计时
std::map<int, bool>Eatpid;//记录捕捉窗口hWnd的map
HWND EatList[101];//被捕捉的窗口的hWnd将被压入这个"栈"
std::map<int, bool>expid, tdpid;//explorer PID + 被监视窗口 PID
HWND tdh[101]; //被监视窗口hWnd
int sdl = 7;//(小声~)
int tdhcur, displaycur;//被监视窗口数量 + 正在被监视的窗口编号
int BSODstate;//蓝屏文字显示的标记
HWND BSODhwnd;//蓝屏窗体的hWnd


//空下几行为Class留位置


class CathyClass//控件主类
{
public:
	void InitClass(HINSTANCE HInstance)//新Class使用之前最好Init一下
	{								//  （不Init也行）
		hInstance = HInstance;//设置hInst
		CurButton = CurFrame = CurCheck = CurLine = CurText = 0;
		CurWnd = 1;//清"零"
		CoverButton = -1;//ps:可以在InitClass之前设定DPI
		CoverCheck = 0;
		//默认宋体
		DefFont = CreateFontW((int)(16 * DPI), (int)(8 * DPI), 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
	}

	__forceinline wchar_t* GetStr(LPCWSTR ID) { return str[Hash(ID)]; }//通过Hash + map 来快速索引字符串的数据结构
	//															ID(索引字符串) -> Hash -(map)> 字符串地址

	void SetStr(LPCWSTR Str, LPCWSTR ID)//通过ID设置字符串
	{
		unsigned int tmp = Hash(ID);
		if (str[tmp] != 0)str[tmp] = 0;//删除当前ID中原有的字符串
		str[tmp] = new wchar_t[wcslen(Str) + 1];//申请相应的内存空间
		wcscpy(str[tmp], Str);//复制新的
	}

	void CreateString(LPCWSTR Str, LPCWSTR ID)//创建新字符串
	{//注意:应仅在使用常量初始化时使用本函数,用变量初始化是建议CurString++再用SetStr
		++CurString;
		if (Str != NULL)//仅复制指针(危险!)
			string[CurString].str = (LPWSTR)Str;
		string[CurString].ID = (LPWSTR)ID;
		str[Hash(ID)] = string[CurString].str;
	}

	void CreateEditEx(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, LPCWSTR ID, HFONT Font, BOOL Ostr)
	{//创建自绘输入框
		++CurEdit;
		Edit[CurEdit].Left = Left; Edit[CurEdit].Top = Top;
		Edit[CurEdit].Width = Wid; Edit[CurEdit].Height = Hei;
		Edit[CurEdit].Page = Page;//设置长宽高等信息

		if (!Ostr)
		{//设置灰色标识的提示字符串
			wcscpy_s(Edit[CurEdit].OStr, name);
			Edit[CurEdit].font = Font;//有提示字符串时不能有正常字符串
			Edit[CurEdit].str = new wchar_t[21];
		}
		else//没有提示字符串:
			SetEditStrOrFont(name, Font, CurEdit);
		wcscpy_s(Edit[CurEdit].ID, ID);
	}

	void CreateArea(int Left, int Top, int Wid, int Hei, int Page)//创建点击区域
	{
		++CurArea;//设置长宽高等信息
		Area[CurArea].Left = Left; Area[CurArea].Top = Top;
		Area[CurArea].Width = Wid; Area[CurArea].Height = Hei;
		Area[CurArea].Page = Page;
	}

	void CreateButtonEx(int Number, int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, HBRUSH Leave, \
		HBRUSH Hover, HBRUSH press, HPEN Leave2, HPEN Hover2, HPEN Press2, HFONT Font, BOOL Enabled, COLORREF FontRGB, LPCWSTR ID)
	{//创建按钮的复杂函数...
		Button[Number].Left = Left; Button[Number].Top = Top;//上下左右
		Button[Number].Width = Wid; Button[Number].Height = Hei;
		Button[Number].Page = Page; Button[Number].Leave = Leave;
		Button[Number].Hover = Hover; Button[Number].Press = press;//离开 & 悬浮 & 点击 , HBRUSH & HPEN
		Button[Number].Leave2 = Leave2; Button[Number].Hover2 = Hover2;
		Button[Number].Press2 = Press2; Button[Number].Font = Font;
		Button[Number].Enabled = Enabled;
		Button[Number].FontRGB = FontRGB;
		wcscpy_s(Button[Number].Name, name);
		wcscpy_s(Button[Number].ID, ID);
		but[Hash(ID)] = Number;

		LOGBRUSH LogBrush;//从HBRUSH中提取出RGB颜色
		LOGPEN LogPen;//	(渐变色需要)
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
		++CurButton;//这里的name Wid Hei 不用全名是因为警告"隐藏了全局声明"
		Button[CurButton].Left = Left; Button[CurButton].Top = Top;//上下左右
		Button[CurButton].Width = Wid; Button[CurButton].Height = Hei;
		Button[CurButton].Page = Page;
		wcscpy_s(Button[CurButton].Name, name);
		wcscpy_s(Button[CurButton].ID, ID);
		but[Hash(ID)] = CurButton;
		Button[CurButton].Leave2 = Button[CurButton].Hover2 = Button[CurButton].Press2 = BlackPen;
		Button[CurButton].Leave = WhiteBrush;
		Button[CurButton].Hover = DBlueBrush;
		Button[CurButton].Press = LBlueBrush;
		Button[CurButton].b1[0] = Button[CurButton].b1[1] = Button[CurButton].b1[2] = 255;
		Button[CurButton].b2[0] = 210; Button[CurButton].b2[1] = Button[CurButton].b2[2] = 255;
	}

	void CreateFrame(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name)//创建内容框
	{
		++CurFrame;//															--- Example -----
		Frame[CurFrame].Left = Left; Frame[CurFrame].Page = Page;//			    |				|
		Frame[CurFrame].Height = Hei; Frame[CurFrame].Top = Top;//			    |				|
		Frame[CurFrame].Width = Wid;//										    |				|
		wcscpy_s(Frame[CurFrame].Name, name);//								    -----------------
	}

	void CreateCheck(int Left, int Top, int Page, int Wid, LPCWSTR name)//创建"√"
	{
		++CurCheck;//设置长宽高等信息
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
	{//start & end 传入的都是未缩放坐标
		++CurLine;
		Line[CurLine].StartX = StartX; Line[CurLine].StartY = StartY;
		Line[CurLine].EndX = EndX; Line[CurLine].EndY = EndY;
		Line[CurLine].Page = Page; Line[CurLine].Color = rgb;
	}

	BOOL InsideButton(int cur, POINT& point)//根据传入的point判断鼠标指针是否在按钮内
	{//cur:按钮的编号，不是ID
		return (Button[cur].Left * DPI <= point.x && Button[cur].Top * DPI <= point.y && (long)((Button[cur].Left + Button[cur].Width) * DPI) >= point.x && (long)((Button[cur].Top + Button[cur].Height) * DPI) >= point.y);
	}

	int InsideCheck(int cur, POINT& point)//同理 判断鼠标指针是否在check内
	{
		if (Check[cur].Left * DPI <= point.x && Check[cur].Top * DPI <= point.y && Check[cur].Left * DPI + 15 * DPI + 1 >= point.x
			&& Check[cur].Top * DPI + 15 * DPI + 1 >= point.y)return 1;//在check的方框内

		if (Check[cur].Left * DPI <= point.x && Check[cur].Top * DPI <= point.y && Check[cur].Left * DPI + Check[cur].Width * DPI >= point.x
			&& Check[cur].Top * DPI + 15 * DPI + 1 >= point.y)return 2;//在check方框右侧一定距离内
		return 0;//哪边都不在
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
				SelectObject(hdc, BlackPen);//绘制方框
				SelectObject(hdc, DefFont);
				MoveToEx(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI), NULL);//多加点(int)xx*DPI 减少警告
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI));
				SetTextColor(hdc, Frame[i].rgb);//自定义文字颜色
				RECT rc = GetRECTf(i);
				//s((int)hdc);
				SetBkMode(hdc, OPAQUE);//打印上方文字
				DrawTextW(hdc, Frame[i].Name, (int)wcslen(Frame[i].Name), &rc, NULL);
				SetTextColor(hdc, NULL);
			}
			if (cur != 0)return;
		}
	}

	void DrawButtons(int cur)//绘制按钮
	{
		int i;//如果使用ObjectRedraw则跳过其他Button
		if (cur != 0) { i = cur; goto begin; }//结构示意：选择颜色(渐变 or 禁用 or 默认) -> 选择字体
		for (i = 1; i <= CurButton; ++i)//				-> 绘制方框 -> 绘制下载进度条 -> 绘制文字 -> 清理
		{
		begin:
			if (Button[i].Page == CurWnd || Button[i].Page == 0)
			{
				HPEN TmpPen = 0; HBRUSH TmpBrush = 0;
				if (Button[i].Enabled == false)//禁用则显示灰色
				{
					SelectObject(hdc, DGreyBrush);
					SelectObject(hdc, Button[i].Leave2);
					SetTextColor(hdc, COLOR_NORMAL_GREY);
					goto colorok;//直接跳过渐变色
				}
				SetTextColor(hdc, Button[i].FontRGB);
				if (Button[i].Percent != 0 && Button[i].Percent != 100 && Button[i].DownTot == 0)//渐变色绘制
				{
					TmpPen = CreatePen(PS_SOLID, 1, RGB((Button[i].p2[0] - Button[i].p1[0]) * Button[i].Percent / 100 + Button[i].p1[0], \
						(Button[i].p2[1] - Button[i].p1[1]) * Button[i].Percent / 100 + Button[i].p1[1], (Button[i].p2[2] - Button[i].p1[2]) * Button[i].Percent / 100 + Button[i].p1[2]));
					//临时创建画笔和画刷
					SelectObject(hdc, TmpPen);
					TmpBrush = CreateSolidBrush(RGB((Button[i].b2[0] - Button[i].b1[0]) * Button[i].Percent / 100 + Button[i].b1[0], \
						(Button[i].b2[1] - Button[i].b1[1]) * Button[i].Percent / 100 + Button[i].b1[1], (Button[i].b2[2] - Button[i].b1[2]) * Button[i].Percent / 100 + Button[i].b1[2]));
					SelectObject(hdc, TmpBrush);
					goto colorok;
				}
				if (CoverButton == i && Button[i].DownTot == 0)//没有禁用 也没有渐变色 -> 默认颜色
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
			colorok:
				if (Button[i].Font == NULL)SelectObject(hdc, DefFont); else SelectObject(hdc, Button[i].Font);//字体

				Rectangle(hdc, (int)(Button[i].Left * DPI + 0.5), (int)(Button[i].Top * DPI + 0.5),
					(int)(Button[i].Left * DPI + Button[i].Width * DPI), (int)(Button[i].Top * DPI + Button[i].Height * DPI));//绘制方框

				if (Button[i].DownTot != 0)//下载进度条
				{
					SelectObject(hdc, Button[i].Hover);
					Rectangle(hdc, (int)(Button[i].Left * DPI), (int)(Button[i].Top * DPI),
						(int)(Button[i].Left * DPI + Button[i].Width * DPI * (Button[i].Download - 1) / 100), (int)(Button[i].Top * DPI + Button[i].Height * DPI));
				}

				RECT rc = GetRECT(i);

				SetBkMode(hdc, TRANSPARENT);//去掉文字背景
				if (Button[i].DownTot == 0)//打印文字(默认)
					DrawTextW(hdc, Button[i].Name, (int)wcslen(Button[i].Name), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				else
				{//正在下载
					if (Button[i].Download >= 101 && (Button[i].DownTot == Button[i].DownCur))
					{//已全部下载完成
						DrawTextW(hdc, GetStr(L"Loaded"), 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						Button[i].Download = Button[i].DownTot = Button[i].DownCur = 0;
					}
					else
					{
						if (Button[i].DownTot < 2)//正在下载 (总数为1)
							DrawTextW(hdc, GetStr(L"Loading"), 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						else
						{
							wchar_t tmp1[101], tmp2[11];//正在下载 (已下载个数)/(总数)
							wcscpy_s(tmp1, GetStr(L"Loading"));
							wcscat_s(tmp1, L" ");
							_itow_s(Button[i].DownCur, tmp2, 10);
							wcscat_s(tmp1, tmp2);
							wcscat_s(tmp1, L"/");//拼接字符串
							_itow_s(Button[i].DownTot, tmp2, 10);
							wcscat_s(tmp1, tmp2);
							DrawTextW(hdc, tmp1, (int)wcslen(tmp1), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}
					}
				}
				if (TmpPen != NULL)DeleteObject(TmpPen);//回收句柄
				if (TmpBrush != NULL)DeleteObject(TmpBrush);
			}
			if (cur != 0)return;//使用ObjectRedraw时直接结束
		}
		SetTextColor(hdc, COLOR_BLACK);
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
				if (i == CoverCheck)SelectObject(hdc, NormalBluePen); else SelectObject(hdc, NormalGreyPen);

				SelectObject(hdc, LGreyBrush);
				SelectObject(hdc, DefFont);//check默认边长为15，并不能调整
				Rectangle(hdc, (int)(Check[i].Left * DPI), (int)(Check[i].Top * DPI), (int)(Check[i].Left * DPI + 15 * DPI), (int)(Check[i].Top * DPI + 15 * DPI));
				TextOut(hdc, (int)(Check[i].Left * DPI + 20 * DPI), (int)(Check[i].Top * DPI), Check[i].Name, (int)wcslen(Check[i].Name));
				if (Check[i].Value == 1)//打勾
				{						//比较难看
					SelectObject(hdc, CheckGreenPen);//试过了直接贴图，更难看
					MoveToEx(hdc, (int)(Check[i].Left * DPI + 2 * DPI), (int)(Check[i].Top * DPI + 7 * DPI), NULL);//所以只能这样了 = =
					LineTo(hdc, (int)(Check[i].Left * DPI + 6 * DPI), (int)(Check[i].Top * DPI + 12 * DPI));
					LineTo(hdc, (int)(Check[i].Left * DPI + 12 * DPI), (int)(Check[i].Top * DPI + 3 * DPI));
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
				SelectObject(hdc, CreatePen(0, 1, Line[i].Color));//直接用LineTo
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
	{//注释只有一个，也不用ObjectRedraw
		if (ExpExist == false)return;//注释不存在？
		SelectObject(hdc, DefFont);
		SelectObject(hdc, YellowPen);//选择注释专用的黄色背景
		SelectObject(hdc, YellowBrush);
		Rectangle(hdc, ExpPoint.x, ExpPoint.y, ExpWidth + ExpPoint.x, ExpHeight + ExpPoint.y);
		SetTextColor(hdc, COLOR_BLACK);
		for (int i = 1; i <= ExpLine; ++i)//逐行打印
			TextOutW(hdc, ExpPoint.x + 4, (int)(ExpPoint.y - 12 * DPI + 16 * i * DPI), Exp[i], (int)wcslen(Exp[i]));//注意这里的ExpPoint , ExpWidth等都是真实坐标
	}

	void DrawEdits(int cur)//绘制输入框
	{
		int i, MOffset = 0, showBegin = 0, showEnd = 0;//结构示意:	创建缓存dc -> 绘制边框(蓝色 or 灰色) -> 打印文字(提示文字)
		HDC mdc;//													->打印文字(未选中) ->打印文字(选中) -> 从缓存dc贴图 -> 清理
		mdc = CreateCompatibleDC(hdc);

		SelectObject(mdc, EditBitmap);//Edit专用“三缓冲”bitmap和dc
		SetBkMode(mdc, TRANSPARENT);
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Edits

		for (i = 1; i <= CurEdit; ++i)
		{
		begin:
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
			{
				SelectObject(mdc, WhitePen);//清空缓存dc
				SelectObject(mdc, WhiteBrush);
				Rectangle(mdc, 0, 0, MAX_EDIT_WIDTH, MAX_EDIT_HEIGHT);

				SelectObject(hdc, WhiteBrush);
				if (i == CoverEdit)SelectObject(hdc, NormalBluePen); else SelectObject(hdc, BlackPen);//如果当前Edit被选中则用蓝色绘制边框
				Rectangle(hdc, (int)((Edit[i].Left - 5) * DPI), (int)(Edit[i].Top * DPI),//否则仅用默认的黑色
					(int)((Edit[i].Left + Edit[i].Width + 5) * DPI), (int)((Edit[i].Top + Edit[i].Height) * DPI));

				if (*Edit[i].OStr != 0)//如果当前Edit显示的是Ostr(仅用于提示的灰色文字)
				{//在Hdc上直接打印完走人
					SetTextColor(hdc, COLOR_DARKER_GREY);
					RECT rc = { (long)((Edit[i].Left - 5) * DPI) , (long)(Edit[i].Top * DPI),(long)((Edit[i].Left + Edit[i].Width + 5) * DPI),(long)((Edit[i].Top + Edit[i].Height) * DPI) };
					DrawTextW(hdc, Edit[i].OStr, (int)wcslen(Edit[i].OStr), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					continue;
				}

				SIZE sel, ser;//pos1 / pos2:确定打印时“选中部分”真正的左右两端点
				int pos1, pos2;//因为有时候从左到右移动鼠标选中文字，有时从右向左
				if (Edit[i].Pos1 > Edit[i].Pos2&& Edit[i].Pos2 != -1)pos1 = Edit[i].Pos2, pos2 = Edit[i].Pos1; else pos1 = Edit[i].Pos1, pos2 = Edit[i].Pos2;
				//pos1&pos2里记录的数值只表示选中的先后顺序，不代表左右，因此这里要特殊处理

				if (Edit[i].font != NULL)SelectObject(mdc, Edit[i].font); else SelectObject(mdc, DefFont);//字体
				if (Edit[i].XOffset != 0)
				{//当XOffset不为0时，可能大部分文字都不会显示在屏幕上，这时候它们无需被打印
					showBegin = max(MyGetTextExtentPoint32Binary(i, Edit[i].XOffset) - 1, 0);
					SIZE se;//获取被显示在屏幕上的文字开始处和结尾处
					MyGetTextExtentPoint32(i, -1, showBegin - 1, se);
					MOffset = se.cx;//获取MOffset(相对于XOffset后向左的偏移量)
					//当没有这个优化时，缓存上打印时总是打印整个字符串，贴图时从XOffset开始贴
					//有这一优化时，若XOffset不为0，在mdc上打印时从横坐标0开始打印，较原来向左偏移了(字符串前端未被打印的字符的长度)个单位，所以贴图时也应向左移MOffset单位.
					showEnd = min(MyGetTextExtentPoint32Binary(i, (int)(Edit[i].XOffset + Edit[i].Width * DPI)), Edit[i].strLength - 1);
				}
				if (pos2 == -1)
				{//如果没有选中，直接打印+贴图 -> 走人
					if (Edit[i].XOffset != 0)
						TextOutW(mdc, 0, 4, Edit[i].str + showBegin, showEnd - showBegin + 1);
					else
						TextOutW(mdc, 0, 4, Edit[i].str, Edit[i].strLength);
					goto next;
				}
				//如果选中:较为复杂的情况
				MyGetTextExtentPoint32(i, -1, pos1 - 1, sel);//选中条左边字符总长度
				MyGetTextExtentPoint32(i, -1, pos2 - 1, ser);//选中条长度+左边字符总长度
				if (Edit[i].XOffset != 0)
				{//XOffset不为0，使用MOffset优化的情况
					if (pos1 >= showBegin)
					{//注意XOffset和Moffset都是乘上过DPI的真实坐标
						SetTextColor(mdc, COLOR_BLACK);
						TextOutW(mdc, 0, 4, Edit[i].str + showBegin, min(pos1, showEnd) - showBegin);//黑色打印选中条左边文字
					}//注:此优化为最近添加，可能不稳定
					if (pos2 >= showBegin)
					{
						SelectObject(mdc, NormalBluePen);
						SelectObject(mdc, NormalBlueBrush);//用蓝色绘制选中条背景
						Rectangle(mdc, max(sel.cx - MOffset, 0), 0, min(ser.cx - MOffset, int(Edit[i].XOffset + Edit[i].Width * DPI)), (int)(ser.cy + 5 * DPI));
						SetTextColor(mdc, COLOR_WHITE);
						TextOutW(mdc, max(sel.cx - MOffset, 0), 4, max(Edit[i].str + pos1, Edit[i].str + showBegin), min(pos2, showEnd) - max(pos1, showBegin) + 1);//白色打印选中条中间文字
					}
					if (pos2 <= showEnd)
					{
						SetTextColor(mdc, COLOR_BLACK);
						TextOutW(mdc, max(ser.cx - MOffset, 0), 4, max(Edit[i].str + pos2, Edit[i].str + showBegin), showEnd - max(pos2, showBegin) + 1);//黑色打印选中条右边文字
					}
				}
				else
				{
					SelectObject(mdc, NormalBlueBrush);
					SelectObject(mdc, NormalBluePen);//用蓝色绘制选中条背景
					Rectangle(mdc, sel.cx, 0, ser.cx, (int)(ser.cy + 5 * DPI));
					SetTextColor(mdc, COLOR_BLACK);
					TextOutW(mdc, 0, 4, Edit[i].str, pos1);//黑色打印选中条左边文字
					SetTextColor(mdc, COLOR_WHITE);
					TextOutW(mdc, sel.cx, 4, &Edit[i].str[pos1], pos2 - pos1);//白色打印选中条中间文字
					SetTextColor(mdc, COLOR_BLACK);
					TextOutW(mdc, ser.cx, 4, &Edit[i].str[pos2], Edit[i].strLength - pos2);//黑色打印选中条右边文字
				}
			next:
				int yMax = (int)((Edit[i].Top + Edit[i].Height / 2) * DPI - 4 - Edit[i].strHeight / 2);
				if (yMax < Edit[i].Top * DPI + 1)yMax = (int)(Edit[i].Top * DPI + 1);//贴图
				if (Edit[i].XOffset == 0)
					BitBlt(hdc, (int)((Edit[i].Left + Edit[i].Width / 2) * DPI) - Edit[i].strWidth / 2, yMax//Xoffset为0时居中
						, Edit[i].strWidth
						, Edit[i].strHeight + 4, mdc, 0, 0, SRCCOPY);
				else
					BitBlt(hdc, (int)(Edit[i].Left * DPI), yMax//有Xoffset时直接根据Xoffset-MOffset贴
						, (int)(Edit[i].Width * DPI)
						, Edit[i].strHeight + 4, mdc, Edit[i].XOffset - MOffset, 0, SRCCOPY);
			}
			if (cur != 0)goto end;
		}
	end:
		DeleteDC(mdc);
	}

	void RedrawObject(int type, int cur)//1=Frame,2=Button,3=Check,4=Text,5=Edit
	{//ObjectRedraw技术的分派函数
		switch (type)
		{
		case 1:DrawFrames(cur); break;//type和控件类型的关系不好记啊~
		case 2:DrawButtons(cur); break;
		case 3:DrawChecks(cur); break;
		case 4:DrawTexts(cur); break;
		case 5:DrawEdits(cur); break;
		default:DrawExp();
		}//如果type不是1~5就自动DrawExp , 毕竟Exp只能有一个
	}
	//自动绘制所有控件的函数，效率低，不应经常使用
	void DrawEVERYTHING() { DrawFrames(0); DrawButtons(0); DrawChecks(0); DrawLines(); DrawTexts(0); DrawEdits(0); DrawExp(); }
	RECT GetRECT(int cur)//更新Buttons的rc 自带DPI缩放
	{
		RECT rc = { (long)(Button[cur].Left * DPI), (long)(Button[cur].Top * DPI),
			(long)(Button[cur].Left * DPI + Button[cur].Width * DPI),(long)(Button[cur].Top * DPI + Button[cur].Height * DPI) };
		return rc;
	}
	RECT GetRECTf(int cur)//更新Frames的rc 自带DPI缩放
	{
		RECT rc = { (long)(Frame[cur].Left * DPI + 10 * DPI) ,(long)(Frame[cur].Top * DPI - 7 * DPI) ,
		(long)(Frame[cur].Left * DPI + Frame[cur].Width * DPI),(long)(Frame[cur].Top * DPI + 30 * DPI) };
		return rc;
	}

	RECT GetRECTe(int cur)//更新Edit的rc 自带DPI缩放
	{
		RECT rc{ (long)((Edit[cur].Left - 5) * DPI) ,(long)(Edit[cur].Top * DPI),
		(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI),(long)((Edit[cur].Top + Edit[cur].Height) * DPI) };
		return rc;
	}

	RECT GetRECTc(int cur)//更新Check的rc 自带DPI缩放
	{
		RECT rc{ (long)(Check[cur].Left * DPI) ,(long)(Check[cur].Top * DPI) ,
			(long)(Check[cur].Left * DPI + 15 * DPI) ,(long)(Check[cur].Top * DPI + 15 * DPI) };
		return rc;
	}

	void RefreshEditSize(int cur)
	{//当更改Edit的内容后，刷新Edit字符的宽度
		HDC mdc;
		HBITMAP bmp;//创建一个空bmp用来GetTextExtentPoint32(获取字符宽度)
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(mdc, bmp);//设置字体
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font); else SelectObject(mdc, DefFont);
		delete[]Edit[cur].strW;//删除旧的宽度
		Edit[cur].strW = new int[Edit[cur].strLength + 1];
		SIZE se;
		GetTextExtentPoint32(mdc, Edit[cur].str, 1, &se);
		Edit[cur].strW[0] = se.cx;
		Edit[cur].strHeight = se.cy;
		for (int i = 1; i < Edit[cur].strLength; ++i)
		{//依次获取每一位的宽度
			GetTextExtentPoint32(mdc, Edit[cur].str + i, 1, &se);
			Edit[cur].strW[i] = Edit[cur].strW[i - 1] + se.cx;//strW的第i位指的是第0位到第i位的宽度之和
			Edit[cur].strHeight = max(Edit[cur].strHeight, se.cy);//strHeight是高度的最大值
		}
		DeleteDC(mdc);
		DeleteObject(bmp);
	}
	void SetEditStrOrFont(LPCWSTR Newstr, HFONT font, int cur)//改变指定Edit的字体或文字
	{
		if (font != NULL) Edit[cur].font = font;//先设置font，因为文字宽度和字体有关

		if (Newstr != NULL)//改变文字
		{
			Edit[cur].OStr[0] = 0;
			if (Edit[cur].str != NULL)if (*Edit[cur].str != NULL)delete[] Edit[cur].str;
			Edit[cur].strLength = (int)wcslen(Newstr);
			Edit[cur].str = new wchar_t[Edit[cur].strLength + 1];
			wcscpy(Edit[cur].str, Newstr);
		}
		RefreshEditSize(cur);//刷新字符串宽度
		Edit[cur].strWidth = Edit[cur].strW[Edit[cur].strLength - 1];
		if ((int)(Edit[cur].Width * DPI) < Edit[cur].strWidth) {
			if (Edit[cur].XOffset == 0)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI) / 2;
		}
		else Edit[cur].XOffset = 0;
	}
	int GetNearestChar(int cur, POINT Point)//试着获取输入框中离光标最近的字符
	{//Point为窗体中真实坐标
		POINT point;
		if (Edit[cur].strWidth <= Edit[cur].Width * DPI)//如果字符较少没有填满
		{//将窗体中真实坐标转化为Edit中的坐标
			if (Point.x < (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI - Edit[cur].strWidth / 2))return 0;
			if (Point.x > (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI + Edit[cur].strWidth / 2))return Edit[cur].strLength;
			point.x = Point.x - (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) + Edit[cur].strWidth / 2;
		}
		else//反之直接用XOffset计算
			point.x = (long)(Point.x - Edit[cur].Left * DPI + Edit[cur].XOffset);
		int pos = MyGetTextExtentPoint32Binary(cur, point.x);//获取大于point.x的第一个字符
		SIZE sel, ser;
		MyGetTextExtentPoint32(cur, -1, pos - 1, sel);//计算前一个字符宽度
		MyGetTextExtentPoint32(cur, -1, pos, ser);//计算后一个
		if (point.x < (sel.cx + ser.cx) / 2)return pos; else return pos + 1;//判断是选中光标左边还是右边的字符
	}
	void EditPaste(int cur)//将剪贴板中文字粘贴到Edit中的函数
	{
		if (cur == 0 || CoverEdit == 0)return;//如果没有选中Edit则退出 （一般不会出现这种情况的......吧）
		char* buffer = NULL;
		if (OpenClipboard(hWnd))
		{//打开剪切板
			HANDLE hData = GetClipboardData(CF_TEXT);
			if (hData == 0)return;//如果剪切板中格式不是CF_TEXT，则打开失败并退出
			buffer = (char*)GlobalLock(hData);
			GlobalUnlock(hData);
			CloseClipboard();
		}
		if (buffer == 0)return;
		size_t len = strlen(buffer), len2 = Edit[cur].strLength + 1;//这段代码只能粘贴文字
		wchar_t* ClipBoardtmp = new wchar_t[len + 1], * Edittmp = new wchar_t[len + len2];
		ZeroMemory(ClipBoardtmp, sizeof(wchar_t) * len);
		ZeroMemory(Edittmp, sizeof(wchar_t) * (len + len2));
		if (buffer != NULL)//读取
			MultiByteToWideChar(CP_ACP, 0, buffer, -1, ClipBoardtmp, (int)(sizeof(wchar_t) * len));
		int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2), pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		if (pos1 == -1)
		{//如果只有单光标选中
			wchar_t t = Edit[cur].str[pos2];
			Edit[cur].str[pos2] = '\0';
			wcscpy(Edittmp, Edit[cur].str);
			wcscat(Edittmp, ClipBoardtmp);
			Edit[cur].str[pos2] = t;//在光标后面加入剪切板中字符并拼接
			wcscat(Edittmp, &Edit[cur].str[pos2]);
			Edit[cur].Pos1 += (int)wcslen(ClipBoardtmp);
			SetEditStrOrFont(Edittmp, 0, cur);
			RefreshXOffset(cur);
			RefreshCaretByPos(cur);
		}
		else//选中区段
		{
			Edit[cur].str[pos1] = '\0';
			wcscpy(Edittmp, Edit[cur].str);
			wcscat(Edittmp, ClipBoardtmp);//将选择部分替换成剪切板中字符并拼接
			wcscat(Edittmp, &Edit[cur].str[pos2]);
			Edit[cur].Pos1 += (int)wcslen(ClipBoardtmp);
			Edit[cur].Pos2 = -1;
			SetEditStrOrFont(Edittmp, 0, cur);
			RefreshXOffset(cur);
			RefreshCaretByPos(cur);
		}
		delete[] Edittmp;//清理内存
		delete[] ClipBoardtmp;
		EditRedraw(cur);//重绘控件
	}

	void EditHotKey(int wParam)//Edit框按下热键时的分派函数
	{
		if (CoverEdit == 0)return;
		switch (wParam)
		{
		case 34:EditMove(CoverEdit, -1); break;//<-键
		case 35:EditMove(CoverEdit, 1); break;//->键
		case 36:EditPaste(CoverEdit); break;//粘贴
		case 37:EditCopy(CoverEdit); break;//复制
		case 38://剪切
			if (Edit[CoverEdit].Pos2 != -1)
				EditCopy(CoverEdit),//剪切其实是先复制再删除
				EditDelete(CoverEdit, min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2),
					max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2));
			break;
		case 39:EditAll(CoverEdit); break;//全选
		case 40://Delete键（不是Backspace!）
			if (Edit[CoverEdit].strLength == Edit[CoverEdit].Pos1)break;
			int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)//Backspace键直接算在WM_CHAR里面了
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else EditDelete(CoverEdit, pos1, pos1 + 1);
			break;
		}
		return;
	}

	void EditUnHotKey()//取消注册Edit的热键
	{//在点击一个Edit外部时自动执行
		for (int i = 34; i < 41; ++i)UnregisterHotKey(hWnd, i);
		HideCaret(hWnd);//隐藏闪烁的光标
	}

	void EditRegHotKey()//注册Edit的热键
	{//在点击一个Edit时自动执行
		RegisterHotKey(hWnd, 34, NULL, VK_LEFT);//<-
		RegisterHotKey(hWnd, 35, NULL, VK_RIGHT);//-?
		RegisterHotKey(hWnd, 36, MOD_CONTROL, 'V');//粘贴
		RegisterHotKey(hWnd, 37, MOD_CONTROL, 'C');//复制
		RegisterHotKey(hWnd, 38, MOD_CONTROL, 'X');//剪切
		RegisterHotKey(hWnd, 39, MOD_CONTROL, 'A');//全选
		RegisterHotKey(hWnd, 40, NULL, VK_DELETE);//Delete键
		DestroyCaret();//在点击的地方创建闪烁的光标
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
	}
	void EditCHAR(wchar_t wParam)//输入
	{
		if (Edit[CoverEdit].Press == true || CoverEdit == 0)return;//没有选择Edit时退出
		if (wParam >= 0x20 && wParam != 0x7F)//当按下的是正常按键时:
		{
			int pos = min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			int pos2 = max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			if (pos == -1)//没有选中文字(增加)
				EditAdd(CoverEdit, pos2, pos2, wParam);
			else//选中了一段文字(替换)
				EditAdd(CoverEdit, pos, pos2, wParam);
		}
		else if (wParam == VK_BACK)//Backspace键
		{
			int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)//删除光标的前一个文字
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else//删除选中的一段文字
				EditDelete(CoverEdit, pos1 - 1, pos1);
		}
		return;
	}

	void EditAdd(int cur, int Left, int Right, wchar_t Char)//向某个Edit中添加一个字符 或 把一段字符替换成一个字符
	{//这里的Left&Right是字符位置而不是坐标
		int len = Edit[cur].strLength + 5;//计算原Edit中文字的长度
		wchar_t* Tempstr = new wchar_t[len], t = 0;//申请对应长度的缓存空间
		if (Left == Right)t = Edit[cur].str[Left];//如果只是添加字符，则先备份光标右侧的字符
		Edit[cur].str[Left] = '\0';
		wcscpy(Tempstr, Edit[cur].str);//将光标左侧的所有字符复制到缓存中
		Tempstr[Left] = Char;
		Tempstr[Left + 1] = 0;
		Edit[cur].str[Left] = t;//添加新的字符
		wcscat(Tempstr, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left + 1;//将右侧的字符再接上去
		Edit[cur].Pos2 = -1;
		ZeroMemory(Edit[cur].str, sizeof(Edit[cur].str));
		SetEditStrOrFont(Tempstr, 0, cur);//刷新
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}

	void EditDelete(int cur, int Left, int Right)//删除一个或一段字符
	{
		if (Left == -1)return;//在最左边还按下BackSpace ?
		wchar_t* Tempstr = new wchar_t[Edit[cur].strLength];
		Edit[cur].str[Left] = '\0';
		wcscpy(Tempstr, Edit[cur].str);
		wcscat(Tempstr, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left;
		Edit[cur].Pos2 = -1;
		SetEditStrOrFont(Tempstr, 0, cur);
		RefreshXOffset(cur);//刷新
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}

	void EditAll(int cur)//选中一个Edit中所有字符
	{
		if (cur == 0)return;//未选中任何Edit
		Edit[cur].Pos1 = 0;
		Edit[cur].Pos2 = Edit[cur].strLength;
		RefreshXOffset(CoverEdit);
		RefreshCaretByPos(CoverEdit);
		EditRedraw(cur);//刷新
	}

	void EditMove(int cur, int offset)//移动选中的Edit中光标的位置
	{//offset可以为负
		int xback;//offset也可以不是1
		xback = Edit[cur].XOffset;
		if (Edit[cur].Pos2 != -1)return;
		Edit[cur].Pos1 += offset;
		if (Edit[cur].Pos1 < 0)Edit[cur].Pos1 = 0;
		if (Edit[cur].Pos1 > Edit[cur].strLength)Edit[cur].Pos1 = Edit[cur].strLength;
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
	}

	void EditCopy(int cur)//复制一个Edit中已选中的内容
	{
		if (cur == 0)return;
		wchar_t* EditStr, t;
		char* ClipBoardStr;
		if (Edit[cur].Pos2 == -1)return;
		int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2);
		int pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		EditStr = new wchar_t[pos2 - pos1 + 1];
		ClipBoardStr = new char[(pos2 - pos1 + 1) * 2];
		t = Edit[cur].str[pos2];
		Edit[cur].str[pos2] = '\0';
		wcscpy(EditStr, Edit[cur].str + pos1);
		Edit[cur].str[pos2] = t;
		WideCharToMultiByte(CP_ACP, 0, EditStr, -1, ClipBoardStr, 0xffff, NULL, NULL);

		if (OpenClipboard(hWnd))
		{
			HGLOBAL clipbuffer;
			char* buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(ClipBoardStr) + 1);
			buffer = (char*)GlobalLock(clipbuffer);
			if (buffer != NULL)strcpy(buffer, LPCSTR(ClipBoardStr));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
	}
	void MyGetTextExtentPoint32(int cur, int start, int end, SIZE& se)
	{//自制获取字符串宽度的函数
		if (*Edit[cur].OStr != 0)return;//若还有提示字符串则退出
		if (start == -1)//记录数组里0位时第1个字符的宽度
		{//若想从第1个字符左边开始获取，则start应为-1
			if (end == -1)se.cx = 0; else se.cx = Edit[cur].strW[end];
		}//然而数组[]不能为-1，这里要特殊处理
		else se.cx = Edit[cur].strW[end] - Edit[cur].strW[start];
		se.cy = Edit[cur].strHeight;
	}

	int MyGetTextExtentPoint32Binary(int cur, int point)
	{//通过二分查找，获取指定Edit中宽度大于search的第一个字符位置
		int left = 0, right = Edit[cur].strLength - 1, mid;
		while (right >= left)//循环直至左右指针交叉(找不到)
		{
			mid = (left + right) >> 1;//中=(左+右)/2
			if (Edit[cur].strW[mid] < point)left = mid + 1;//当在point左边时，向右二分
			if (Edit[cur].strW[mid] >= point)//若在point右边
			{//且mid-1在point左边->找到了->退出
				if (Edit[cur].strW[mid - 1] < point)return mid; else
					right = mid - 1;//太过右边->向左二分
			}
		}
		if (left == 0)return 0; else return Edit[cur].strLength - 1;//找不到->返回最左 or 最右
	}

	void RefreshCaretByPos(int cur)//刷新选中的Edit中光标的位置
	{
		if (Edit[cur].Pos1 == -1)return;//指定Edit未被选中->退出
		if (Edit[cur].strLength == 0) {
			SetCaretPos((long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI), (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - 6 * DPI));
			goto finish;
		}
		SIZE se;//通过这个Edit的Pos1的字符来计算字符长度
		if (Edit[cur].Pos2 != -1)
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos2 - 1, se);
		else
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos1 - 1, se);
		if (Edit[cur].XOffset == 0)//减去Xoffset，再加上Edit的坐标就是光标位置了
			SetCaretPos(se.cx + (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) - Edit[cur].strWidth / 2, (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2));
		else
			SetCaretPos((int)(se.cx + Edit[cur].Left * DPI - Edit[cur].XOffset), (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2));
	finish:
		ShowCaret(hWnd);
	}
	void EditDown(int cur)//鼠标左键在某个Edit上按下
	{
		EditRegHotKey();//先注册下热键再说
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CoverEdit != cur)Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1, EditRedraw(CoverEdit);//如果直接从一个Edit点到
		CoverEdit = cur;//														另一个Edit，那么先把之前的Pos和蓝框问题解决好
		if (*Edit[cur].OStr != 0)//去掉灰色的提示文字
		{
			*Edit[cur].OStr = 0;
			ZeroMemory(Edit[cur].str, sizeof(Edit[cur].str));
		}
		Edit[cur].Pos1 = Edit[cur].Pos2 = -1;
		Edit[cur].Press = true;
		Edit[cur].Pos1 = GetNearestChar(cur, point);//计算Pos1

		RefreshCaretByPos(cur);//计算 闪烁的光标 的位置
		EditRedraw(cur);//重绘这个Edit
	}

	BOOL InsideArea(int cur, POINT& point)//通过POINT判断是否在指定Area内
	{//POINT为鼠标真实坐标
		return (Area[cur].Left * DPI <= point.x) && (Area[cur].Top * DPI <= point.y) &&
			((Area[cur].Left + Area[cur].Width) * DPI >= point.x) && ((Area[cur].Top + Area[cur].Height) * DPI >= point.y);
	}

	BOOL InsideEdit(int cur, POINT& point)//通过POINT判断是否在指定Edit内
	{//POINT为鼠标真实坐标
		return ((Edit[cur].Left - 5) * DPI <= point.x && Edit[cur].Top * DPI <= point.y &&
			(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI) >= point.x && (Edit[cur].Top + Edit[cur].Height) * DPI >= point.y);
	}

	void LeftButtonDown()//鼠标左键按下
	{
		POINT point;
		GetCursorPos(&point);//获取真实坐标
		ScreenToClient(hWnd, &point);
		if (CoverButton != -1)//当按钮按下 & 停留在按钮上时
		{
			Press = 1;//重绘这个按钮
			RECT rc = GetRECT(CoverButton);
			if (Obredraw)Readd(2, CoverButton);
			Redraw(rc);
		}
		if (!InsideEdit(CoverEdit, point) && Edit[CoverEdit].Pos2 == -1 && CoverEdit != 0)
		{
			int tmp0 = CoverEdit;//原来一个Edit被激活
			CoverEdit = 0;//现在鼠标点在那个Edit外面时
			if (Obredraw)Readd(5, tmp0);//重绘原来的Edit
			RECT rc = GetRECTe(tmp0);
			Redraw(rc);
			EditUnHotKey();//取消热键
		}
		if (EditPrv != 0)
		{//鼠标点在另一个Edit上时同样重绘Edit(好吧有点啰嗦= =)
			if (Obredraw)Readd(5, EditPrv);
			RECT rc = GetRECTe(EditPrv);
			Redraw(rc);
			EditPrv = 0;
		}
		if (CoverEdit != 0)//停留在Edit上时
			EditDown(CoverEdit);
		else
			EditUnHotKey();
		Timer = GetTickCount();//重置exp的计时器
		DestroyExp();//任何操作都会导致exp的关闭
	}

	void CheckGetNewInside(POINT& point)//检查鼠标是否在任何Check内
	{//POINT为鼠标真实坐标
		for (int i = 1; i <= CurCheck; ++i)//遍历所有check
		{
			if (Check[i].Page == CurWnd || Check[i].Page == 0)//不在同一页面 -> 直接跳过
				if (InsideCheck(i, point) != 0)//在check的文字中或方框内
				{
					CoverCheck = i;//设置covercheck
					if (Obredraw)Readd(3, i);
					RECT rc = GetRECTc(i);//重绘
					Redraw(rc);
					return;
				}
		}
	}
	void ButtonGetNewInside(POINT& point)//检查鼠标是否在任何Button内
	{//POINT为鼠标真实坐标
		for (int i = 0; i <= CurButton; ++i)//历史原因，Button编号是从0开始的
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled)
				if (InsideButton(i, point))//在按钮中
				{
					CoverButton = i;//设置CoverButton
					if (ButtonEffect)//特效开启
					{//设定渐变色
						Button[i].Percent += 40;//先给40%的颜色 （太淡了看不出来）
						if (Button[i].Percent > 100)Button[i].Percent = 100;
					}
					if (Obredraw)Readd(2, i);
					RECT rc = GetRECT(i);//重绘
					Redraw(rc);
					return;
				}
	}
	void AreaGetNewInside(POINT& point)//检查鼠标是否在任何ClickArea内
	{//POINT为鼠标真实坐标
		for (int i = 1; i <= CurArea; ++i)
			if (Area[i].Page == CurWnd || Area[i].Page == 0)
				if (InsideArea(i, point))
				{
					CoverArea = i;
					return;
				}
	}
	void EditGetNewInside(POINT& point)//检查鼠标是否在任何Edit内
	{//POINT为鼠标真实坐标
		for (int i = 1; i <= CurEdit; ++i)
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
				if (InsideEdit(i, point))
				{
					EditPrv = CoverEdit;//记录好之前的Edit编号
					CoverEdit = i;
					EditRegHotKey();
					return;
				}
	}
	void MouseMove()//鼠标移动
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CoverButton == -1)ButtonGetNewInside(point);//原来不在按钮内 -> 看看现在是否移进按钮
		else//原来在
		{
			if (!Button[CoverButton].Enabled) { CoverButton = -1; goto disabled; }//这个按钮被禁用了  直接跳到下面
			if ((Button[CoverButton].Page != CurWnd && Button[CoverButton].Page != 0) || !InsideButton(CoverButton, point))
			{//现在不在
				if (Obredraw)Readd(2, CoverButton);
				if (ButtonEffect)
				{//CoverButton设为-1 , 重绘
					Button[CoverButton].Percent -= Delta;
					if (Button[CoverButton].Percent < 0)Button[CoverButton].Percent = 0;
				}
				RECT rc = GetRECT(CoverButton);
				CoverButton = -1;
				Redraw(rc);
				ButtonGetNewInside(point);//有可能从一个按钮直接移进另一个按钮内
			}
		}
	disabled:
		if (CoverCheck == 0)CheckGetNewInside(point);//在外面 -> 寻找新check
		else
		{//同理(真的)
			if ((Check[CoverCheck].Page != CurWnd && Check[CoverCheck].Page != 0) || InsideCheck(CoverCheck, point) == 0)
			{
				if (Obredraw)Readd(3, CoverCheck);
				RECT rc = GetRECTc(CoverCheck);
				CoverCheck = 0;
				Redraw(rc);
				CheckGetNewInside(point);
			}
		}
		if (CoverEdit != 0 && Edit[CoverEdit].Press == true)
		{
			//如果Edit被按下 (同时在拖动选择条)
			int t = Edit[CoverEdit].Pos2;
			Edit[CoverEdit].Pos2 = GetNearestChar(CoverEdit, point);//寻找和鼠标指针最近的字符
			RefreshCaretByPos(CoverEdit);//移动Caret(闪烁的光标)
			if (Edit[CoverEdit].Pos2 == Edit[CoverEdit].Pos1) { Edit[CoverEdit].Pos2 = -1; goto end; }//只选择了一个字符
			if (Edit[CoverEdit].Pos2 != t && Edit[CoverEdit].Width < Edit[CoverEdit].strWidth && !InsideEdit(CoverEdit, point))RefreshXOffset(CoverEdit);//Edit中文本过长，移动到了框外面
			if (Edit[CoverEdit].Pos2 != t)EditRedraw(CoverEdit);//只要和原来有任何不同就重绘
		}
	end:
		if (CoverArea == 0)
			AreaGetNewInside(point);
		else
			if (!InsideArea(CoverArea, point))CoverArea = 0;

		if (Msv == 0)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);//检测鼠标移进移出的函数
			tme.hwndTrack = hWnd;//在鼠标移出窗体时会触发一个WM_LEAVE消息，根据这个可以改变按钮颜色
			tme.dwFlags = TME_LEAVE;//缺点是当焦点直接被另一个窗口夺取时(比如按下windows键)
			TrackMouseEvent(&tme);//什么反应都没有
			Msv = 1;//移出
		}
		else Msv = 0;//移进
		if (point.x != ExpPoint2.x || point.y != ExpPoint2.y)
		{//鼠标移动时销毁Exp
			ExpPoint2 = point;
			Timer = GetTickCount();//重置exp计时器
			DestroyExp();
		}
	}
	void EditRedraw(int cur)//重绘Edit的外壳函数
	{
		if (Obredraw)Readd(5, cur);
		RECT rc = GetRECTe(cur);
		Redraw(rc);//标准ObjectRedraw写法
	}
	void InfoBox(LPCWSTR Str)//全自动的MessageBox
	{
		const bool GUIStrExist = (bool)GetStr(Str);
		if (!slient)//如果Str中是GUIstr的ID则打印str的内容，否则直接打印Str
			if (GUIStrExist)MessageBox(hWnd, GetStr(Str), GetStr(L"Info"), 0x40L); else MessageBox(hWnd, Str, GetStr(L"Info"), 0x40L);
		else
		{
			wchar_t tmpstr[MY_MAX_PATH];
			wcscpy_s(tmpstr, GetStr(L"TDT"));
			if (GUIStrExist)wcscat_s(tmpstr, GetStr(Str)); else wcscat_s(tmpstr, Str);
			WriteConsole(hdlWrite, tmpstr, (DWORD)wcslen(tmpstr), nullptr, nullptr);
			WriteConsole(hdlWrite, L"\n", 2, nullptr, nullptr);
		}//打印到命令行中
	}
	void RefreshXOffset(int cur)//重新计算Edit的Xoffset
	{
		if (Edit[cur].strWidth < Edit[cur].Width * DPI) {
			Edit[cur].XOffset = 0; return;//如果Edit中内容太少，XOffset直接为0，退出
		}
		SIZE se;

		if (Edit[cur].Pos2 != -1)
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos2 - 1, se);//计算长度
		else
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos1 - 1, se);//计算长度//过程比较复杂，但原理简单
		if (se.cx < Edit[cur].XOffset)Edit[cur].XOffset = se.cx - 1;
		if (se.cx > (long)(Edit[cur].XOffset + Edit[cur].Width * DPI))Edit[cur].XOffset += (int)(se.cx - (Edit[cur].XOffset + Edit[cur].Width * DPI) - 1);
		if (Edit[cur].strWidth - Edit[cur].XOffset < Edit[cur].Width * DPI)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}

	//int GetNumByIDe(LPCWSTR ID)//通过Edit的ID获取其编号
	//{
	//	for (int i = 1; i <= CurEdit; ++i)if (wcscmp(ID, Edit[i].ID) == 0)return i;
	//	return 0;//找不到返回0
	//}

	void SetPage(int newPage)//设置窗口的页数
	{
		if (newPage == CurWnd)return;//点了当前页的按钮，直接退出
		HideCaret(hWnd);//换页时自动隐藏闪烁的光标
		Edit[CoverEdit].Press = false;
		Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1;
		CoverEdit = 0;//取消Edit的选中
		CurWnd = newPage;
		rs[0].first = 0;
		es[0].top = 0;
		Redraw();//切换页面时当然需要全部重绘啦
	}
	void SetDPI(DOUBLE NewDPI)//改变窗口的缩放大小
	{//							(由于某历史原因，缩放大小的变量被我命名成了DPI)
		if (DPI == NewDPI)return;
		DPI = NewDPI;//创建新大小的字体
		DeleteObject(DefFont);
		DefFont = CreateFontW((int)(16 * DPI), (int)(8 * DPI), 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
		for (int i = 1; i <= CurEdit; ++i)SetEditStrOrFont(0, DefFont, i), RefreshXOffset(i);//设置字体
		RefreshCaretByPos(CoverEdit);
		if (Width != 0 && Height != 0)SetWindowPos(hWnd, NULL, 0, 0, (int)(Width * DPI), (int)(Height * DPI), SWP_NOMOVE | SWP_NOREDRAW);
		DestroyCaret();//设置闪烁的光标
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
		Redraw();//全部重绘
	}

	LPWSTR GetCurInsideID(POINT& point)//获取当前鼠标处于的按钮的ID
	{
		for (int i = 0; i <= CurButton; ++i)
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled)
				if (InsideButton(i, point) && Button[i].DownTot == 0)
					return Button[i].ID;//返回ID
		return Button[0].ID;//返回一个空值
	}
	inline int GetNumbyID(LPCWSTR ID) { return but[Hash(ID)]; }//通过按钮的ID获取其编号

	void EnableButton(int cur, bool enable)
	{
		Button[cur].Enabled = enable;
		if (Button[cur].Page == CurWnd || Button[cur].Page == 0)
		{
			Readd(2, cur);
			Redraw(GetRECT(cur));
		}
	}
	void SetHDC(HDC HDc)//给要绘制的窗口设置一个新的hdc
	{
		hdc = HDc;
		if (EditBitmap != NULL)DeleteObject(EditBitmap);//给Edit创建一个Bitmap
		EditBitmap = CreateCompatibleBitmap(hdc, MAX_EDIT_WIDTH, MAX_EDIT_HEIGHT);
	}
	void Try2CreateExp()//尝试解析一个Exp的内容并绘制
	{
		if (ExpExist == true || CoverButton == -1)return;//如果Exp已经存在，，或者Exp内容为空，那么就没他什么事了
		if (Button[CoverButton].Exp == 0)return;
		if (wcslen(Button[CoverButton].Exp) == 0)return;
		ExpExist = true;
		ExpLine = 0;//清零
		ZeroMemory(Exp, sizeof(Exp));

		wchar_t* x = Button[CoverButton].Exp, * y = Button[CoverButton].Exp;
		ExpHeight = 8;
		ExpWidth = 0;
		while (1)
		{
			ExpLine++;
			x = wcsstr(x + 1, L"\\n");//在Exp的字符串中寻找\n字符
			if (x != 0)x[0] = '\0';//然后存储在一个二维数组内
			if (ExpLine == 1)wcscpy(Exp[ExpLine], y); else wcscpy(Exp[ExpLine], y + 2);
			SIZE* se = new SIZE;
			if (ExpLine == 1)GetTextExtentPoint32(hdc, y, (int)wcslen(y), se); else GetTextExtentPoint32(hdc, y + 2, (int)wcslen(y + 2), se);//获取字符串的宽度
			if (x != 0)x[0] = '\\';
			ExpHeight += se->cy;//计算这个Exp的宽和高
			ExpWidth = max(ExpWidth - 8, se->cx) + 8;
			if (x == 0)break;
			y = x;
		}
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		ExpPoint = point;//绘制这个Exp
		if (ExpPoint.x > (int)(Width * DPI / 2.0))ExpPoint.x -= (ExpWidth + 6); else ExpPoint.x += 12;//自动选择注释的位置
		if (ExpPoint.y > (int)(Height * DPI / 2.0))ExpPoint.y -= (ExpHeight + 6); else ExpPoint.y += 14;//防止打印到窗口外面
		RECT rc{ ExpPoint.x, ExpPoint.y, ExpPoint.x + ExpWidth, ExpPoint.y + ExpHeight };//注意这里的ExpPoint等都是真实坐标
		Readd(6, 1);
		Redraw(rc);
	}
	void DestroyExp()//清除这个Exp
	{
		if (ExpExist == false)return;
		ExpExist = false;
		RECT rc{ ExpPoint.x, ExpPoint.y, ExpPoint.x + ExpWidth, ExpPoint.y + ExpHeight };
		ExpLine = ExpHeight = ExpWidth = 0;//注意这里的ExpPoint等都是真实坐标

		Redraw(rc);//删除Exp时要绘制这个Exp下面的按钮什么的，所以分类绘制很麻烦，干脆就全部刷新一下吧
	}
	FORCEINLINE void Erase(RECT& rc) { es[++es[0].top] = rc; }//设置要擦除的区域
	void Redraw(const RECT& rc) { InvalidateRect(hWnd, &rc, FALSE); UpdateWindow(hWnd); }//自动重绘 & 刷新指定区域
	void Redraw()
	{
		rs[0].first = 0;
		es[0].top = 0;
		InvalidateRect(hWnd, nullptr, false);
		UpdateWindow(hWnd);
	}//添加要刷新的控件-、
	void Readd(int type, int cur) { rs[++rs[0].first] = { type,cur }; }//1=Frame,2=Button,3=Check,4=Text,5=Edit
	bool GetLanguage(GETLAN& a)
	{
		__try
		{
			wchar_t* str1 = wcsstr(a.begin, L"\"");
			wchar_t* str2 = wcsstr(str1 + 1, L"\"");
			*str2 = 0;//不想说什么了
			a.str1 = str1 + 1;//Name
			str1 = wcsstr(str2 + 1, L",");
			if (str1 == NULL)return true;//Left
			str2 = wcsstr(str1 + 1, L",");
			if (str2 != NULL)*str2 = 0;
			a.Left = _wtoi(str1 + 1);//Top
			if (str2 == NULL)return true;
			str1 = wcsstr(str2 + 1, L",");//Width
			if (str1 != NULL)*str1 = 0;
			a.Top = _wtoi(str2 + 1);
			if (str1 == NULL)return true;
			str2 = wcsstr(str1 + 1, L",");//Height
			if (str2 != NULL)*str2 = 0;
			a.Width = _wtoi(str1 + 1);
			if (str2 == NULL)return true;
			str1 = wcsstr(str2 + 1, L",");
			if (str1 != NULL)*str1 = 0;
			a.Height = _wtoi(str2 + 1);
			if (str1 == 0)return true;
			str1 = wcsstr(str1 + 1, L"\"");//Str
			str2 = wcsstr(str1 + 1, L"\"");
			*str2 = '\0';
			a.str2 = str1 + 1;
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{//语言文件不规范时会说error
			error();
			return false;
		}
	}
	void DispatchLanguage(LPWSTR ReadTmp, int type)
	{//将语言文件中读取到的一行设置到Class中
		__try
		{
			GETLAN gl = { 0 };
			wchar_t* pos = wcsstr(ReadTmp, L"=");
			if (pos == 0)return;
			*pos = 0; gl.begin = pos + 1;
			wchar_t* space = wcsstr(ReadTmp, L" ");
			if (space != 0)space[0] = '\0';//分不同的控件讨论
			if (type == 1)//button
			{
				int cur = GetNumbyID(ReadTmp);
				if (!GetLanguage(gl))return;

				if (gl.Left != -1)Button[cur].Left = gl.Left;
				if (gl.Top != -1)Button[cur].Top = gl.Top;
				if (gl.Width != -1)Button[cur].Width = gl.Width;
				if (gl.Height != -1)Button[cur].Height = gl.Height;
				if (gl.str1 != NULL)wcscpy_s(Button[cur].Name, gl.str1);
				if (Button[cur].Exp != NULL)if (*Button[cur].Exp != NULL)
				{
					ZeroMemory(Button[cur].Exp, wcslen(Button[cur].Exp) * sizeof(wchar_t));
					Button[cur].Exp = 0;
				}
				if (gl.str2 != NULL)
				{
					Button[cur].Exp = new wchar_t[wcslen(gl.str2) + 1];
					wcscpy(Button[cur].Exp, gl.str2);
				}
				return;
			}
			if (type == 2)//check
			{
				int cur = _wtoi(ReadTmp + 1);
				if (!GetLanguage(gl))return;
				if (gl.Left != -1)Check[cur].Left = gl.Left;
				if (gl.Top != -1)Check[cur].Top = gl.Top;
				if (gl.Width != -1)Check[cur].Width = gl.Width;
				if (gl.str1 != NULL)wcscpy_s(Check[cur].Name, gl.str1);
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
				SetStr(tmp, ReadTmp);
				return;
			}
			if (type == 4)//frame
			{
				int cur = _wtoi(ReadTmp + 1);
				if (!GetLanguage(gl))return;
				if (gl.Left != -1)Frame[cur].Left = gl.Left;
				if (gl.Top != -1)Frame[cur].Top = gl.Top;
				if (gl.Width != -1)Frame[cur].Width = gl.Width;
				if (gl.Height != -1)Frame[cur].Height = gl.Height;
				if (gl.str1 != NULL)wcscpy_s(Frame[cur].Name, gl.str1);
				return;
			}
			if (type == 5)//text
			{
				int cur = _wtoi(ReadTmp + 1);
				wchar_t* tmpstr = wcsstr(pos + 1, L",");
				*tmpstr = 0;
				int NewLeft = _wtoi(pos + 1);
				int NewTop = _wtoi(tmpstr + 1);
				if (NewLeft != -1)Text[cur].Left = NewLeft;
				if (NewTop != -1)Text[cur].Top = NewTop;
				return;
			}
			if (type == 6)//edit
			{
				int cur = _wtoi(ReadTmp + 1);
				if (!GetLanguage(gl))return;
				if (gl.Left != -1)Edit[cur].Left = gl.Left;
				if (gl.Top != -1)Edit[cur].Top = gl.Top;
				if (gl.Width != -1)Edit[cur].Width = gl.Width;
				if (gl.Height != -1)Edit[cur].Height = gl.Height;
				if (gl.str1 != NULL && Edit[cur].strLength == 0)wcscpy_s(Edit[cur].OStr, gl.str1);
				return;
			}
		}//仍然是长但原理简单的代码
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			wchar_t tmpstr[MY_MAX_PATH] = L"error:";
			wcscat_s(tmpstr, ReadTmp);
			InfoBox(tmpstr);
		}
	}

	//下面是Class的变量

	struct ButtonEx//按钮
	{
		long Left, Top, Width, Height, Page, Download, Percent, DownCur, DownTot;
		bool Enabled = true;
		HBRUSH Leave, Hover, Press;//离开 and 悬浮 and 按下
		HPEN Leave2, Hover2, Press2;
		HFONT Font;
		wchar_t Name[31], ID[11], * Exp = 0;
		COLORREF FontRGB;
		BYTE b1[3], b2[3], p1[3] = { 0 }, p2[3] = { 0 };
	}Button[MAX_BUTTON];//只有按钮使用了ID
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
		int strWidth, strHeight, Pos1, Pos2, XOffset, strLength;
		int* strW;
		bool Press;
		wchar_t* str, ID[11], OStr[21];
		HFONT font;
	}Edit[MAX_EDIT];
	struct ClickAreaEx//点击区域
	{
		int Left, Top, Width, Height, Page;
	}Area[MAX_AREA];
	struct GUIString//带ID标签的字符串
	{
		wchar_t* str, * ID;
		bool first = true;
	}string[MAX_STRING];

	int ExpLine, ExpHeight, ExpWidth;//关于Explaination的几个变量
	wchar_t Exp[MAX_EXPLINES][MAX_EXPLENGTH];
	POINT ExpPoint, ExpPoint2;
	BOOL ShowExp = FALSE;//Exp是否被显示
	DWORD Timer;//exp开启的时间
	bool ExpExist = false;//exp是否存在

	std::map<unsigned int, wchar_t*> str;//GUIstr的ID ->编号(用于索引)
	std::map<unsigned int, int>but;//button的ID -> 编号
	HFONT DefFont;//默认的字体
	int Msv;//鼠标移出检测变量
	int CurString, CurButton, CurFrame, CurCheck, CurLine, CurText, CurEdit, CurArea;//各种控件的数量
	double DPI = 1;
	int CoverButton, CoverCheck, CoverEdit, CoverArea;//当前被鼠标覆盖的东西
	bool Obredraw = false;//是否启用ObjectRedraw技术
	bool ButtonEffect = false;//是否开启渐变色
	int CurWnd;//当前的页面
	int Press;//鼠标左键是否按下
	struct Mypair { int first, second; };

	Mypair rs[101];//重绘列表 1=Frame,2=Button,3=Check,4=Text,5=Edit
	RECT es[101];//清理列表
	HDC hdc;//缓存dc
	HDC tdc;//真实dc
	HBITMAP EditBitmap;//Edit专用缓存bitmap
	HBITMAP Bitmap;//缓存窗口bitmap
	int Width, Height;//窗口的宽和高
	HWND hWnd;//Class绘制的窗体的hwnd
	HINSTANCE hInstance;//程序的hInst
	int EditPrv = 0;//之前被激活的edit序号
//没有任何private变量或函数= =
}Main;

//Class的声明结束
//下面是各种函数

#pragma warning(disable:4100)//禁用警告
class DownloadProgress : public IBindStatusCallback {
public://这些函数有些参数没有用到，会导致大量警告.
	wchar_t curi[11] = { 0 };/*推送下载进度信息的按钮ID*/ int factmax = 0;//手动设置被下载文件的真实大小
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
			double percentage;
			if (factmax == 0) percentage = double(ulProgress * 1.0 / ulProgressMax * 100);//计算下载百分比
			else  percentage = double(ulProgress * 1.0 / factmax * 100);//注意！有时函数无法得到正确的ulProgressMax，虽然没有下完
			if (Main.Button[Main.GetNumbyID(curi)].Download != (int)percentage + 1)//percentage仍会等于100%，导致一系列错误
			{//																		这时候应尽量传入正确大小(factmax)
				if (!Effect && percentage <= 100)return S_OK;//低画质时不显示进度条
				Main.Button[Main.GetNumbyID(curi)].Download = (int)percentage + 1;
				RECT rc = Main.GetRECT(Main.GetNumbyID(curi));
				Main.Readd(2, Main.GetNumbyID(curi));//重绘
				Main.Redraw(rc);
			}
		}
		return S_OK;
	}
};

BOOL CALLBACK EnumChildwnd(HWND hwnd, LPARAM lParam)//查找极域广播子窗口的枚举函数.
{
	LONG A;//遍历子窗口
	A = GetWindowLongW(hwnd, GWL_STYLE) & WS_VISIBLE;
	if (A == 0)return 1;//极域2015和2016版本的广播窗口不是单一的，
	wchar_t title[301];//正常看到的全屏窗口里面还有TDDesk Render Window(广播内容窗口)和工具栏窗口。
	GetWindowText(hwnd, title, 300);//这里枚举极域广播窗口的子窗口，找到TDDesk Render Window，记录下来，
	if (wcsstr(title, L"TDDesk Render Window") != 0)//然后捕捉到CatchWnd里面去
	{
		SetParent(hwnd, CatchWnd);
		EatList[0]++;
		EatList[(size_t)(EatList[0])] = hwnd;
		tdhw = hwnd;
		return 0;//return 0 代表中断查找
	}
	return 1;
}
BOOL CALLBACK EnumBroadcastwnd(HWND hwnd, LPARAM lParam)//查找"屏幕广播"窗口的枚举函数.
{
	LONG A;//遍历所有窗口
	A = GetWindowLongW(hwnd, GWL_STYLE) & WS_VISIBLE;
	if (A == 0)return 1;
	wchar_t title[301];
	GetWindowText(hwnd, title, 300);
	if (wcscmp(title, L"屏幕广播") == 0)//这里枚举枚举极域广播窗口
	{//2015、2016中极域广播窗口一般叫"屏幕广播"。但极域曾是多语言的，在英语系统上可能会出问题
		BlackBoard = true;//找到"屏幕广播"窗口后，继续寻找它的子窗口
		if (FS) { FS = false; MyRegisterClass(hInst, ScreenProc, ScreenWindow, false); }
		tdhwscr = CreateWindow(ScreenWindow, L"You can't see me.", WS_POPUP, 0, 0, 100, 100, nullptr, nullptr, hInst, nullptr);
		SetParent(hwnd, tdhwscr);//子窗口被捕捉后，"屏幕广播"窗口也应该得到妥善处置。
		tdhw2 = hwnd;//曾今试过DestroyWindow、ShowWindow(SW_HIDE)这些标准方法，但都没有效果
		EnumChildWindows(hwnd, EnumChildwnd, NULL);//这里创建一个看不见的窗口，把"屏幕广播"窗口捕捉进去
		return 0;
	}
	return 1;
}
BOOL CALLBACK EnumBroadcastwndOld(HWND hwnd, LPARAM lParam)//查找被极域广播窗口的枚举函数.
{
	LONG A;//遍历所有窗口
	A = GetWindowLongW(hwnd, GWL_STYLE) & WS_VISIBLE;//这个函数用于2007、2010、2012版本的极域
	if (A == 0)return 1;
	wchar_t title[301];//较老版本的极域里面没有TDDesk Render Window，极域广播窗口也不叫"屏幕广播"，所以需要专门处理
	GetWindowText(hwnd, title, 300);
	if (wcsstr(title, L"屏幕演播室窗口") != 0 || wcsstr(title, L"屏幕广播窗口") != 0)//屏幕演播室窗口->2010 or 2012版，屏幕广播窗口->2007版
	{
		tdhw = hwnd; tdhw2 = (HWND)(-2);
		SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_WINDOWEDGE | WS_EX_TOPMOST | WS_EX_LEFT | WS_EX_CLIENTEDGE);
		SetParent(hwnd, CatchWnd);//2010、2012版极域使用这个功能效果较好，
		EatList[0]++;//但是2007版本不建议使用。因为2007版本的极域广播不支持自动缩放，
		EatList[(size_t)(EatList[0])] = hwnd;//窗口化后只能显示老师屏幕的一部分
		return 0;
	}
	return 1;
}
BOOL CALLBACK EnumMonitoredwnd(HWND hwnd, LPARAM lParam)//查找被监视窗口的枚举函数.
{//(to be removed)
	ULONG nProcessID;
	LONG A;//遍历所有窗口
	A = GetWindowLongW(hwnd, GWL_STYLE) & WS_VISIBLE;
	if (A != 0 && GetParent(hwnd) == NULL)
	{
		RECT rc;
		GetWindowRect(hwnd, &rc);
		if (rc.right - rc.left < 15 || rc.bottom - rc.top < 15)return 1;//过小的窗口将不被监视
		::GetWindowThreadProcessId(hwnd, &nProcessID);//如果pid正确，把hwnd记录下来
		if (tdpid[nProcessID])
			tdh[++tdhcur] = hwnd;
	}
	return 1;
}
#pragma warning(default:4100)//恢复警告

void FindMonitoredhWnd(wchar_t* ProcessName)//查找被监视的窗口.
{
	tdpid.clear();//清空映射
	ProcessName[3] = 0;
	_wcslwr(ProcessName);
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = '\0';
		_wcslwr(pe.szExeFile);//记录下符合要求的pid
		if (wcsstr(pe.szExeFile, ProcessName) != 0)tdpid[pe.th32ProcessID] = true;
	}
	if (!EnumWindows(EnumMonitoredwnd, NULL))error();
}

DWORD WINAPI TopThread(LPVOID pM)//置顶线程.
{
	UNREFERENCED_PARAMETER(pM);
	while (TOP == 1)//循环会直接占用一个CPU线程，在较差的电脑建议打开"低画质"
	{//连续置顶	(比不过任务管理器，人家有CreateWindowWithBand)
		if (Main.Check[CHK_EFFECT].Value == true)
		{//开"低画质"时自动改用计时器置顶
			SetTimer(Main.hWnd, TIMER_TOP, 10, (TIMERPROC)TimerProc);
			return 0;
		}
		SetWindowPos(Main.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
		if (CatchWnd != NULL)SetWindowPos(CatchWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
	}//停止前取消置顶
	SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	if (CatchWnd != NULL)SetWindowPos(CatchWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	return 0;
}

BOOL CALLBACK EnumFullScreenWnd(HWND hwnd, LPARAM lParam)//杀掉全屏窗口的枚举函数
{//枚举置顶窗口
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID; HANDLE hProcessHandle = 0;
	RECT rc; GetWindowRect(hwnd, &rc);
	::GetWindowThreadProcessId(hwnd, &nProcessID);//寻找全屏窗口
	if (rc.left == 0 && rc.top == 0 && rc.bottom == GetSystemMetrics(SM_CYSCREEN) && rc.right == GetSystemMetrics(SM_CXSCREEN))
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		if (GetCurrentProcessId() == nProcessID || expid[nProcessID] == true)goto skipped;//如果是被zi保ji护de的进程ID -> 跳过
		hProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, nProcessID);//现在结束进程全部改用不带Nt的函数
		TerminateProcess(hProcessHandle, 1);//(反正，，我是没遇到NtTerminateProc起作用过,只见到他崩溃过)
	}
skipped:
	return 1;
}
void KillFullScreen()//杀掉置顶窗口
{
	expid.clear();
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = '\0';
		if (wcsstr(pe.szExeFile, L"exp") != 0)expid[pe.th32ProcessID] = true;
	}//将Explorer的Pid加入保护名单中
	if (!EnumWindows(EnumFullScreenWnd, NULL))error();
}

BOOL KillProcess(LPCWSTR ProcessName)//根据进程名结束进程.
{
	wchar_t MyProcessName[1001] = { 0 }, tmp[1501], * a, * b, filepath[301];
	wcscpy_s(filepath, TempPath);

	if (Main.Check[CHK_FMACH].Value == true)
	{//完全匹配进程名
		wcscpy_s(MyProcessName, ProcessName);
		_wcslwr_s(MyProcessName);
		if (wcsstr(MyProcessName, L".exe") == 0)wcscat_s(MyProcessName, L".exe");
	}
	else
	{
		wcscpy(tmp, ProcessName);
		b = a = tmp;
		while (wcsstr(a, L"|") != 0)
		{
			b = wcsstr(a, L"|");
			*b = 0;//处理用"|"分隔的不同进程名
			a[3] = 0;//将每个"|"前的字符串转为小写并只保留前三个字母
			wcscat_s(MyProcessName, a);
			wcscat_s(MyProcessName, L"|");
			a = b + 1;
		}
		a[3] = 0;
		wcscat_s(MyProcessName, a);
		_wcslwr_s(MyProcessName);
	}

	BOOL ReturnValue = FALSE, NoProcess = TRUE;
	HANDLE PhKphHandle = 0;
	BOOL ConnectSuccess = FALSE;//尝试连接KProcessHacker
	if (KphConnect(&PhKphHandle) >= 0)ConnectSuccess = TRUE;
	if (ConnectSuccess && Bit == 34)
	{//32位TDT在64位系统上用KProcessHacker结束进程时需要用Win64KPHcaller
		wcscat_s(filepath, L"Win64KPHcaller.exe");
		ReleaseRes(filepath, FILE_CALLER, L"JPG");
	}
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;//创建进程快照
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;
	while (Process32Next(hSnapShot, &pe))
	{
		if (Main.Check[CHK_FMACH].Value == false)pe.szExeFile[3] = 0;//根据进程名前三个字符标识
		_wcslwr_s(pe.szExeFile);
		if (wcsstr(MyProcessName, pe.szExeFile) != 0 || ProcessName == NULL)
		{
			NoProcess = FALSE;
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = 0;
			if (Main.Check[CHK_KPH].Value && ConnectSuccess)
			{//连接成功->用驱动结束进程
				if (Bit != 34)
				{
#pragma warning(disable:4312)//64位下这里会报HANDLE大于DWORD的警告
					PhOpenProcess(&hProcess, 1, (HANDLE)dwProcessID, PhKphHandle);
#pragma warning(default:4312)
					ReturnValue |= PhTerminateProcess(hProcess, 1, PhKphHandle) >= 0;
				}
				else
				{
					wchar_t cmdline[MY_MAX_PATH], tmpstr[20] = { 0 };
					wcscpy_s(cmdline, filepath);//使用Win64KPHcaller
					_itow_s(dwProcessID, tmpstr, 10);
					wcscat_s(cmdline, L" ");
					wcscat_s(cmdline, tmpstr);
					ReturnValue |= RunEXE(cmdline, NULL, nullptr);
				}
			}
			else
			{
				hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
				ReturnValue |= TerminateProcess(hProcess, 1);//否则使用普通的OpenProcess和TerminateProcess
			}
			CloseHandle(hProcess);
		}
	}
	if (Bit == 34)DeleteFile(filepath);//删除Win64KPHcaller
	return ReturnValue || NoProcess;//如果找到了进程，但没结束成功，返回false
}

bool GetTDVer(wchar_t* source)//获取极域版本.
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
	ret = RegQueryValueExW(hKey, L"Version", 0, &dwType, (LPBYTE)&szLocation, &dwSize);
	if (ret != 0)return false;//打开成功却没有键值? -> 找不到
	if (*szLocation != 0)//一切正常
		wcscat(source, szLocation);
	else return false;
	RegCloseKey(hKey);//关闭句柄
	return true;
}

void TDSearchDirect()
{//直接调用SearchTool寻找studentmain.exe的函数
	SearchTool(L"C:\\Program Files\\Mythware", 1);
	SearchTool(L"C:\\Program Files\\TopDomain", 1);//先试着在专用目录里找
	SearchTool(L"C:\\Program Files (x86)\\Mythware", 1);
	SearchTool(L"C:\\Program Files (x86)\\TopDomain", 1);
	if (*TDPath != NULL)return;
	SearchTool(L"C:\\Program Files (x86)", 1);
	if (*TDPath != NULL)return;//再试着再整个Program Files里找
	SearchTool(L"C:\\Program Files", 1);
	if (*TDPath != NULL)return;
	SearchTool(L"D:\\", 1);//还不行就再其他盘里找
	SearchTool(L"E:\\", 1);//再找不到就算了
}
void SetTDPathStr()//更改并自动重绘第四页中"极域路径"这个字符串
{
	wchar_t tmpstr[MY_MAX_PATH + 10];
	wcscpy_s(tmpstr, Main.GetStr(L"_TPath"));
	if (*TDPath == 0)//找不到极域
		wcscat_s(tmpstr, Main.GetStr(L"TPunk"));
	else
		wcscat_s(tmpstr, TDPath);
	Main.SetStr(tmpstr, L"TPath");
	if (Main.CurWnd != 4)return;
	RECT rc{ (int)(170 * Main.DPI), (int)(365 * Main.DPI),(int)(Main.Width * Main.DPI),(int)(390 * Main.DPI) };
	Main.es[++Main.es[0].top] = rc;
	Main.Readd(4, 22);
	Main.Redraw(rc);
}
DWORD WINAPI ReopenThread2(LPVOID pM)//尝试打开极域(线程)
{
	(pM);
	if (!TDsearched)
	{
		TDsearched = true;
		TDSearchDirect();
	}
	SetTDPathStr();
	return 0;
}
void UpdateInfo()//修改"关于"界面信息的函数.
{
	wchar_t tmp[34] = { 0 }, tmp2[34] = { 0 }; int f;
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
	wcscpy_s(tmp2, Main.GetStr(L"TIP")); wcscat(tmp2, CheckIP());//ip地址
	delete[]wip;
	Main.SetStr(tmp2, L"TIP");//极域版本
	if (GetTDVer(tmp2))Main.SetStr(tmp2, L"TTDv");
	CreateThread(0, 0, ReopenThread2, 0, 0, 0);
}
void SetFrameColor()//根据是否有管理员权限的两种情况改变Frame上的文字.
{
	Main.Frame[FRA_POWER].rgb = COLOR_LIGHT_ORANGE;
	Main.Frame[FRA_PASSWORD].rgb = COLOR_NOTREC;
	Main.Frame[FRA_TOPDOMAIN].rgb = COLOR_DARKER_BLUE;
	wcscat_s(Main.Frame[FRA_PASSWORD].Name, Main.GetStr(L"nRec"));
	int  ok[] = { FRA_WINDOW,FRA_CHANNEL }, i;
	for (i = 0; i < 2; ++i)Main.Frame[ok[i]].rgb = COLOR_OK, wcscat_s(Main.Frame[ok[i]].Name, Main.GetStr(L"Usable"));

	if (!Admin)
	{
		int useless[] = { FRA_PROCESS, FRA_DELETER,FRA_OTHERS };
		for (i = 0; i < 3; ++i)Main.Frame[useless[i]].rgb = COLOR_RED, wcscat_s(Main.Frame[useless[i]].Name, Main.GetStr(L"Useless"));
	}
	else
	{
		Main.Frame[FRA_PROCESS].rgb = COLOR_NOTREC;
		wcscat_s(Main.Frame[FRA_PROCESS].Name, Main.GetStr(L"nRec"));
	}
}
void GetPath()//得到程序路径 & ( 程序路径 + 程序名 ).
{
	GetModuleFileName(NULL, Path, MY_MAX_PATH);//直接获取程序名
	wcscpy_s(Name, Path);
	for (int i = wcslen(Path) - 1; i >= 0; --i)
		if (Path[i] == L'\\') {
			Path[i + 1] = 0; return;
		}//把程序名字符串中最后一个"\\"后面的字符去掉就是路径
}

void GetBit()//系统位数.
{
	SYSTEM_INFO si;//较有效的检测系统位数的方法
	MyGetInfo(&si);//AMD64 \ Intel64 ?
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) Bit = 64; else Bit = 32;
	//32 means 32bit GUI on 32bit System
	//34 means 32bit GUI on 64bit System
	//64 means 64bit GUI on 64bit System
	if (sizeof(int*) * 8 == 32 && Bit == 64)Bit = 34;//检测程序自身的位数
}
BOOL BackupSethc()//备份sethc.exe.
{
	if (GetFileAttributes(SethcPath) == INVALID_FILE_ATTRIBUTES) { SethcState = false; return false; }//如果sethc本来就不存在 -> 退出 -> 不是第一次运行
	if (GetFileAttributes(L"C:\\SAtemp") == FILE_ATTRIBUTE_DIRECTORY)return false;//SAtemp目录已经存在 -> sethc可能已经备份过 -> 退出 -> 不是第一次运行
	CreateDirectory(L"C:\\SAtemp", NULL);//创建一个SAtemp临时文件夹(说是"临时"，其实一般不会自动删除)
	return CopyFile(SethcPath, L"C:\\SAtemp\\sethc.exe", TRUE);//因为大多数机子上是有还原卡的嘛
}

bool EnableTADeleter()
{//尝试和DeleteFile驱动通信.
	if (DeleteFileHandle != 0)return true;//已经连接上了 -> 退出
	wchar_t tmpstr[MY_MAX_PATH];
	wcscpy_s(tmpstr, TempPath);
	wcscat_s(tmpstr, L"DeleteFile.sys");
	if (Bit == 32)//释放驱动文件到程序目录里
		ReleaseRes(tmpstr, FILE_TAX32, L"JPG");
	else
		ReleaseRes(tmpstr, FILE_TAX64, L"JPG");
	UnloadNTDriver(L"DeleteFile");//加载驱动
	LoadNTDriver(L"DeleteFile", tmpstr);
	AdjustPrivileges(SE_DEBUG_NAME);

	OBJECT_ATTRIBUTES objectAttributes;
	HMODULE hModule = ::GetModuleHandle(L"ntdll.dll");
	if (hModule == NULL)return false;//寻找ntdll
	NTOPENFILE NtOpenFile = (NTOPENFILE)GetProcAddress(hModule, "NtOpenFile");
	UNICODE_STRING on;
	RtlInitUnicodeString(&on, L"\\Device\\DeleteFile");
	InitializeObjectAttributes(&objectAttributes, &on, 0x40, NULL, NULL);
	IO_STATUS_BLOCK isb;
	NtOpenFile(//尝试取得驱动的句柄
		&DeleteFileHandle,
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,
		&objectAttributes,
		&isb,//创建ISB
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0x40
	);
	return (bool)DeleteFileHandle;
}

bool EnableKPH()
{//加载KProcessHacker驱动(但不取得通信).
	if (Main.Check[CHK_KPH].Value)return true;//如果已经加载，就不用再加载了
	wchar_t tmp[301];
	wcscpy_s(tmp, TempPath);
	wcscat_s(tmp, L"kprocesshacker");
	if (Bit != 32)wcscat_s(tmp, L"64");
	wcscat_s(tmp, L".sys");
	if (Bit == 32)
		ReleaseRes(tmp, FILE_KPH32, L"JPG");
	else
		ReleaseRes(tmp, FILE_KPH64, L"JPG");
	UnloadNTDriver(L"KProcessHacker2");
	AdjustPrivileges(SE_DEBUG_NAME);
	return LoadNTDriver(L"KProcessHacker2", tmp);
}

/*  语言切换  */

void UpdateCatchedWindows();
void SwitchLanguage(LPWSTR name)//改变语言的函数
{//这个函数在用在其他工程时不能直接照抄，因为涉及到不同窗体的问题
	__try {//为了防止直接崩溃这边都用_try包住了
		wcscpy_s(CurrentLanguage, name);//测试这段代码时工程崩溃了不下20次
		bool Mainf = false;
		int type = 0; DWORD NumberOfBytesRead;
		wchar_t AllTmp[50001], * point1, * point2;
		HANDLE hf = CreateFile(name, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
		if (hf == 0)return;
		ReadFile(hf, AllTmp, 50001, &NumberOfBytesRead, NULL);
		if (NumberOfBytesRead == 0)return;
		point1 = AllTmp;
		while (1)
		{
			point2 = wcsstr(point1, L"\n");
			if (point2 == 0)break;
			*point2 = 0;
			if (wcsstr(point1, L"<Main>") != 0)Mainf = true;//用<>来区分不同窗体
			if (wcsstr(point1, L"</Main>") != 0)Mainf = false;//看起来有点像xml代码
			if (wcsstr(point1, L"<Buttons>") != 0)type = 1;
			if (wcsstr(point1, L"<Checks>") != 0)type = 2;
			if (wcsstr(point1, L"<Strings>") != 0)type = 3;
			if (wcsstr(point1, L"<Frames>") != 0)type = 4;
			if (wcsstr(point1, L"<Texts>") != 0)type = 5;
			if (wcsstr(point1, L"<Edits>") != 0)type = 6;
			if (point1[0] != L'<')
			{
				if (Mainf)Main.DispatchLanguage(point1, type);//分配(各个)窗体的语言
			}
			point1 = point2 + 1;
		}
		CloseHandle(hf);//读取完文件后一定要记得关闭
		UpdateInfo();//修改"关于"界面信息
		SetWindowText(Main.hWnd, Main.GetStr(L"Title"));//标题
		if (CatchWnd != NULL)SetWindowText(CatchWnd, Main.GetStr(L"Title2"));
		InfoChecked = true;
		SetFrameColor();//Frame文字
		Main.EnableButton(BUT_MORE, !(bool)wcsstr(name, L"English"));//几个文字会改变的按钮
		if (OneClick)wcscpy_s(Main.Button[Main.GetNumbyID(L"QuickSetup")].Name, Main.GetStr(L"unQS"));
		if (GameMode == 1)wcscpy_s(Main.Button[Main.GetNumbyID(L"Games")].Name, Main.GetStr(L"Gamee"));
		if (FileDeleting)wcscpy_s(Main.Button[Main.GetNumbyID(L"TI")].Name, Main.GetStr(L"deleting"));
		UpdateCatchedWindows();//更新被捕窗口的个数
		Main.Redraw();
		if (CatchWnd)InvalidateRect(CatchWnd, NULL, FALSE), UpdateWindow(CatchWnd);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { error(); }
}

BOOL SetupSethc()//安装sethc
{
	wchar_t mySethcPath[MY_MAX_PATH];
	wcscpy_s(mySethcPath, TempPath);
	wcscat_s(mySethcPath, L"mysethc.exe");
	if (GetFileAttributes(mySethcPath) == -1)ReleaseRes(mySethcPath, FILE_SETHC, L"JPG");//文件不存在的话从资源里释放
	if (GetFileAttributes(mySethcPath) == -1)return 2;//释放之后文件还是不存在?
	return CopyFile(mySethcPath, SethcPath, false);//复制成功 or 失败
}

bool AutoSetupSethc()//自动安装sethc的外壳函数.
{
	int flag = SetupSethc();
	if (flag == 0) { Main.InfoBox(L"CSFail"); return false; }//复制失败
	if (flag == 2) { Main.InfoBox(L"NoSethc"); return false; }//找不到文件
	return true;
}

bool MyDeleteFile(LPCWSTR UserPath)
{//自动使用驱动 or 应用层删除一个文件.
	if (Main.Check[CHK_T_A_].Value == 1)
	{
		if (DeleteFileHandle == 0)return false;
		DWORD ReturnValue[2];//useless value
		wchar_t DriverPath[300], * Pointer1 = DriverPath, ** Pointer2 = &Pointer1;//注意传进去的是指向指针的指针
		wcscpy_s(DriverPath, L"\\??\\");/*路径前要加上\??\			*/
		wcscat_s(DriverPath, UserPath);
		DeviceIoControl(DeleteFileHandle, CTL_CODE(0x22, 0x360, 0, 0), Pointer2, \
			sizeof(Pointer2), ReturnValue, sizeof(ReturnValue), &ReturnValue[1], nullptr);//传进去的CTL_CODE(控制码)中的第二项function一般是0x800~0x2000
		return true;//然而这里很不标准，使用的是0x360 (0x0~0x799都保留给系统，不应给普通应用程序使用)
	}
	else return DeleteFile(UserPath);

}
bool DeleteSethc()//删除sethc(删不掉的话自动使用驱动).
{
	if (GetFileAttributes(SethcPath) == -1)return true;//sethc已经没了 -> 成功
	TakeOwner(SethcPath);//取得所有权
	wchar_t XPsethc[] = L"C:\\Windows\\system32\\dllcache\\sethc.exe";//删除xp中sethc备份文件
	TakeOwner(XPsethc);
	DeleteFile(XPsethc);
	if (DeleteFile(SethcPath))return true;//已经删掉了 -> 退出
	else
	{
		if (Admin == false)return false;//删不掉，又没有管理员权限 -> 失败退出
		EnableTADeleter();
		Main.Check[CHK_T_A_].Value = 1;//加载驱动
		MyDeleteFile(XPsethc);
		return MyDeleteFile(L"C:\\Windows\\system32\\sethc.exe");//返回是否成功
	}
}

bool SearchTool(LPCWSTR lpPath, int type)//1 打开极域 2 删除shutdown 3 删除文件夹.
{//有多个功能的函数，所以叫它"Tool"
	wchar_t szFind[MY_MAX_PATH], szFile[MY_MAX_PATH];//因为都用的是同一段搜索代码，所以就干脆把三个按钮功能合起来了
	WIN32_FIND_DATA FindFileData;
	wcscpy_s(szFind, lpPath);
	wcscat_s(szFind, L"\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) return true;
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{//发现是一个文件夹
			if (!(FindFileData.cFileName[0] == '.' && (wcslen(FindFileData.cFileName) == 1 || (wcslen(FindFileData.cFileName) == 2 && FindFileData.cFileName[1] == '.'))))
			{//不是开头带"."的回退文件夹
				wcscpy_s(szFile, lpPath);
				wcscat_s(szFile, L"\\");
				wcscat_s(szFile, FindFileData.cFileName);
				if (!SearchTool(szFile, type))return false;//递归查找
				if (type == 3)RemoveDirectory(szFile);
			}//删除完这个文件夹内的文件后删除这个空文件夹
		}
		else
		{//发现是一个文件
			_wcslwr_s(FindFileData.cFileName);
			if (type == 1) {
				if (wcsstr(FindFileData.cFileName, L"studentmain.exe") != NULL)
				{//查找并运行studentmain
					wcscpy_s(TDName, FindFileData.cFileName);
					wcscpy_s(szFile, lpPath);
					wcscat_s(szFile, L"\\");
					wcscat_s(szFile, FindFileData.cFileName);
					wcscpy_s(TDPath, szFile);
					return false;
				}
			}
			if (type == 2)//删除shutdown
				if (wcsstr(FindFileData.cFileName, L"shutdown.exe") != NULL)
				{
					wcscpy_s(szFile, lpPath);
					wcscat_s(szFile, L"\\");
					wcscat_s(szFile, FindFileData.cFileName);
					TakeOwner(szFile);
					DeleteFile(szFile);
				}
			if (type == 3)
			{//递归删除一个文件夹
				wcscpy_s(szFile, lpPath);
				wcscat_s(szFile, L"\\");
				wcscat_s(szFile, FindFileData.cFileName);
				TakeOwner(szFile);//取得这个文件的所有权
				MyDeleteFile(szFile);
				RemoveDirectory(szFile);
			}
		}
		if (!FindNextFile(hFind, &FindFileData))break;//没有下一个文件了
	}
	FindClose(hFind);//关闭句柄
	return true;
}
void AutoDelete(wchar_t* FilePath, bool sli)//自动删除文件.
{
	int FileType = GetFileAttributes(FilePath);
	if (FileType == -1)
	{//不是文件也不是文件夹？是否强制删除？
		if (!sli)
		{
			wchar_t tmpStr[MY_MAX_PATH] = L"\"";
			wcscat_s(tmpStr, FilePath);
			wcscat_s(tmpStr, L"\"");
			wcscat_s(tmpStr, Main.GetStr(L"TINotF"));
			if (MessageBox(Main.hWnd, tmpStr, Main.GetStr(L"Info"), MB_YESNO | MB_ICONQUESTION) == 6)goto forcedelete;
		}
		return;
	}
forcedelete:
	TakeOwner(FilePath);

	if (FileType & FILE_ATTRIBUTE_DIRECTORY)
	{
		wchar_t DirectoryName[MY_MAX_PATH];//是文件夹
		wcscpy_s(DirectoryName, FilePath);//SHFileOperation要求字符串最后两位都是NULL,
		SearchTool(DirectoryName, 3);//直接在原字符串上操作可能会出问题
		MyRemoveDirectory(DirectoryName);
	}
	else
		MyDeleteFile(FilePath);//是文件，直接删除
}

bool UninstallSethc()//恢复原来的sethc.
{//不是常用的函数，但（又是历史原因）还是被保留下来
	DeleteFile(SethcPath);//这时sethc已经删除过一次，已经拥有了其所有权
	if (CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE) == false) { Main.InfoBox(L"USFail"); return false; }//恢复sethc失败?
	else { SethcState = true; return true; }
}

void AutoRegisterHotKey(HWND hWnd, int id, UINT ms, UINT vk)
{//自动注册热键
	if (!RegisterHotKey(hWnd, id, ms, vk))
	{//先尝试直接注册热键
		if (RegisterHotKey(hWnd, id, MOD_CONTROL | MOD_ALT | MOD_WIN, vk))
		{//直接注册失败(热键冲突)的话尝试换一个注册
			if (TOP)return;//显示在安全桌面上时不能提示
			wchar_t tmpstr[MY_MAX_PATH], tmp2[10] = { 0 }; tmp2[0] = (wchar_t)vk;
			wcscpy_s(tmpstr, Main.GetStr(L"HKF1"));
			if (ms & MOD_CONTROL)wcscat_s(tmpstr, Main.GetStr(L"HKFctrl"));
			if (ms & MOD_ALT)wcscat_s(tmpstr, Main.GetStr(L"HKFalt"));
			if (ms & MOD_SHIFT)wcscat_s(tmpstr, Main.GetStr(L"HKFshift"));
			if (vk > 125 || vk < 45)_itow_s(vk, tmp2, 10);
			wcscat_s(tmpstr, tmp2);//提示用户热键已更改
			wcscat_s(tmpstr, Main.GetStr(L"HKF2"));
			wcscat_s(tmpstr, Main.GetStr(L"HKFctrl"));
			wcscat_s(tmpstr, Main.GetStr(L"HKFalt"));
			wcscat_s(tmpstr, Main.GetStr(L"HKFwin"));
			if (vk > 125 || vk < 45)_itow_s(vk, tmp2, 10);
			wcscat_s(tmpstr, tmp2);
			Main.InfoBox(tmpstr);
		}
		else Main.InfoBox(L"HKF3");
	}
}
void RegMouseKey()//注册键盘控制鼠标的热键.
{
	AutoRegisterHotKey(Main.hWnd, 8, MOD_CONTROL, 188);//左键
	AutoRegisterHotKey(Main.hWnd, 9, MOD_CONTROL, 190);//右键
	AutoRegisterHotKey(Main.hWnd, 10, MOD_CONTROL, VK_LEFT);//左移
	AutoRegisterHotKey(Main.hWnd, 11, MOD_CONTROL, VK_UP);//上移
	AutoRegisterHotKey(Main.hWnd, 12, MOD_CONTROL, VK_RIGHT);//右移
	AutoRegisterHotKey(Main.hWnd, 13, MOD_CONTROL, VK_DOWN);//下移
}
inline void UnMouseKey() { for (int i = 8; i < 14; ++i)UnregisterHotKey(Main.hWnd, i); }//注销热键.

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(MouseHook, nCode, wParam, lParam); }//空的全局钩子函数
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(KeyboardHook, nCode, wParam, lParam); }//防止极域钩住这些

DWORD WINAPI DeleteThread(LPVOID pM)
{//删除文件(夹)的线程.
	(pM);//防止删除一个超大文件夹时卡顿过久
	wchar_t c[25600]; int len, prv = 0;
	wcscpy_s(c, Main.Edit[EDIT_FILEVIEW].str);
	wcscpy_s(Main.Button[BUT_DELETE].Name, Main.GetStr(L"deleting"));
	Main.EnableButton(BUT_DELETE, false);

	len = (int)wcslen(c);
	if (wcsstr(c, L"|") == 0)AutoDelete(c, false);
	else
	{
		for (int i = 0; i < len; ++i)
		{
			if (c[i] == '|')
			{//解析每一个字符串
				c[i] = 0;
				AutoDelete(&c[prv], false);
				prv = i + 1;
			}
		}
	}
	wcscpy_s(Main.Button[BUT_DELETE].Name, Main.GetStr(L"deleted"));
	Main.EnableButton(BUT_DELETE, true);
	return 0;
}
DWORD WINAPI GameThread(LPVOID pM)
{
	UNREFERENCED_PARAMETER(pM);
	int cur = Main.GetNumbyID(L"Close");
	if (Main.Width < 700)
	{//展开窗口
		GameLock = true;//自制线程锁
		if (!Effect)//无特效
		{
			SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((Main.Width + 260) * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE | SWP_NOREDRAW);
			Main.Button[cur].Left += 260;
			Main.Redraw();//直接展开游戏部分
			goto next;
		}
		for (int j = 1; j <= 260; j += 20)
		{
			Main.Width += 20;
			::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((621 + j) * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
			RECT Rc{ (long)(621 * Main.DPI) ,0,(long)((621 + j) * Main.DPI) ,(long)(Main.Height * Main.DPI) };
			Main.Redraw(Rc);//重绘展开部分
			Rc = Main.GetRECT(cur);
			Rc.left -= (long)(20 * Main.DPI);
			Main.Redraw(Rc);//重绘“关闭”按钮
			Main.Button[cur].Left += 20;//右移“关闭按钮”
		}
		Main.Button[cur].Left -= 20;
	next:
		Main.Width = 881;
	}
	else
	{//缩回游戏部分
		if (Effect)//基本上就是反过来
			for (int j = 1; j <= 260; j += 20)
			{
				::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((Main.Width - j) * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
				RECT Rc = Main.GetRECT(cur);
				Rc.right += (long)(20 * Main.DPI);
				Main.Readd(2, cur);
				Main.Redraw(Rc);
				Main.Button[cur].Left -= 20;
			}
		Main.Width -= 260;
		if (Effect)Main.Button[cur].Left += 20; else Main.Button[cur].Left -= 260;
		::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)(Main.Width * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE);
		Main.Redraw();
	}
	GameLock = false;//关闭线程锁
	return 0;
}
bool DownloadGames(const wchar_t* url, const wchar_t* file, DownloadProgress* p, const wchar_t* ButID, int tot, int cur, int maxs)
{//下载(游戏)
	wchar_t Fp[501], URL[121];
	wcscpy_s(Fp, TempPath);
	wcscat_s(Fp, file);//拼接下载目录和源目录
	wcscpy_s(URL, GitGame);
	wcscat_s(URL, url);
	if (ButID != NULL)
	{
		int tmpID = Main.GetNumbyID(ButID);
		Main.Button[tmpID].Download = 1;
		Main.Button[tmpID].DownTot = tot;
		Main.Button[tmpID].DownCur = cur;
		wcscpy_s(p->curi, ButID);
		p->factmax = maxs;
	}
	return URLDownloadToFileW(NULL, URL, Fp, 0, p) == S_OK;
}
DWORD WINAPI DownloadThread(LPVOID pM)//分发下载(游戏)任务的线程.
{
	int cur = *(int*)pM;
	bool f = false;
	const wchar_t t[5][10] = { L"fly.9exe",L"2048.exe",L"block.exe",L"1.exe",L"chess.exe" },
		g[5][6] = { L"Game2",L"Game3" ,L"Game4" ,L"Game5" ,L"Game6" };
	DownloadProgress progress;
	switch (cur)
	{
	case 1:
		f = DownloadGames(L"xiaofei.exe", GameName[0], &progress, L"Game1", 2, 1, 0);//1号小游戏有2个文件要特殊处理
		DownloadGames(L"14000%E8%AF%8D%E5%BA%93.ini", L"Games\\14000词库.ini", &progress, L"Game1", 2, 2, 899000);//14000词库.ini
		break;//	由于某些原因无法得到它的文件大小，要在2这里设置
	case 2:case 3:case 4:case 5:case 6:
		//下载第2~6个游戏
		f = DownloadGames(t[cur - 2], GameName[cur - 1], &progress, g[cur - 2], 1, 1, 0);
		break;
	case 7:
	{//ARP attack
		bool WPinstalled = true;
		wchar_t WPPath[] = L"C:\\SAtemp\\WinPcap.exe";
		int num = Main.GetNumbyID(L"ARP");
		wcscpy_s(progress.curi, L"ARP");
		if (GetFileAttributes(L"C:\\Windows\\System32\\wpcap.dll") == -1)//WinPcap未安装
		{
			if (MessageBox(Main.hWnd, Main.GetStr(L"WPAsk"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)return 0;
			//是否同意下载一个？
			WPinstalled = false;
			Main.Button[num].Download = 1;
			Main.Button[num].DownTot = 2;
			Main.Button[num].DownCur = 1;
			if (URLDownloadToFileW(NULL, L"http://download.skycn.com/hao123-soft-online-bcs/soft/W/WinPcap_4.1.3.exe", WPPath, 0, &progress) == S_OK)RunWithAdmin(WPPath);
			else return 0;//下载后以管理员身份运行
		}
		Main.Button[num].Download = 1;
		Main.Button[num].DownTot = Main.Button[num].DownCur = 2 - (int)WPinstalled;
		wchar_t ARPPath[301];
		wcscpy_s(ARPPath, TempPath);//下载arp.exe
		wcscat_s(ARPPath, L"arp.exe");//下载后不自动运行
		if (URLDownloadToFileW(NULL, L"https://raw.githubusercontent.com/zouxiaofei1/TopDomianTools/master/Files/arp/arp.exe", ARPPath, 0, &progress) != S_OK)return 0;
		break;
	}
	case 8:
	{//= =
		wchar_t tmp[MY_MAX_PATH], tmp2[MY_MAX_PATH];
		wcscpy_s(tmp, GitGame);
		wcscat_s(tmp, L"ban.exe");
		wcscpy_s(tmp2, TempPath);
		wcscat_s(tmp2, L"0.exe");
		if (URLDownloadToFileW(NULL, tmp, tmp2, 0, &progress) == S_OK)RestartDirect();
		break;
	}
	default:return 0;
	}
	GameExist[cur - 1] = f;
	return 0;
}

void SearchLanguageFiles()//在当前目录里寻找语言文件
{
	wchar_t tmp[MY_MAX_PATH];//先释放自带的两个中英文语言文件
	wcscpy_s(tmp, TempPath);
	wcscat_s(tmp, L"language\\");//创建language目录
	CreateDirectory(tmp, NULL);
	wcscat_s(tmp, L"Chinese.ini");
	ReleaseRes(tmp, FILE_CHN, L"JPG");
	wcscpy_s(tmp, TempPath);
	wcscat_s(tmp, L"language\\English.ini");
	ReleaseRes(tmp, FILE_ENG, L"JPG");

	SendMessage(FileList, LB_RESETCONTENT, 0, 0);//在寻找新文件前清空listbox
	wchar_t szFind[MY_MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	wcscpy_s(szFind, TempPath);
	wcscat_s(szFind, L"language\\*.ini");

	HANDLE hFind = ::FindFirstFile((LPCWSTR)szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)return;
	while (TRUE)//寻找
	{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))//找到后发送消息至FileList添加
			SendMessage(FileList, LB_ADDSTRING, 0, (LPARAM)FindFileData.cFileName);
		if (!FindNextFile(hFind, &FindFileData))break;
	}//找到最后一个后结束
	FindClose(hFind);
}

DWORD WINAPI FakeNewThread(LPVOID pM)
{//控制伪装工具条(新)窗体的伸出 or 缩回的进程
	if (FakeLock == true)  return 0;//自制线程锁
	FakeLock = true;
	int offset = *(int*)pM;
	RECT rc;
	GetWindowRect(FakeWnd, &rc);
	for (int i = 1; i <= 29; ++i)
	{
		Sleep(10);//伸缩动画
		SetWindowPos(FakeWnd, 0, rc.left, rc.top + i * 2 * offset, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	if (offset == -1 && FakeNew)//设置一下工具条最终位置
		SetWindowPos(FakeWnd, 0, rc.left, -61, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	else
		SetWindowPos(FakeWnd, 0, rc.left, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	FakeLock = false;//关闭线程锁
	return 0;
}

COLORREF DoSelectColour()//选择颜色.
{//点击窗体左上角的logo就可以触发(隐藏功能)
	if (!Effect)return 0;//低画质时不给切换颜色
	static CHOOSECOLOR cc = { 0 };
	cc.lStructSize = sizeof(cc);
	cc.lpCustColors = crCustColors;
	cc.Flags = CC_ANYCOLOR;
	if (!ChooseColor(&cc))return 0;//选择“取消”时不切换颜色
	if (((int)((byte)cc.rgbResult) + (int)((byte)(cc.rgbResult >> 8)) + (int)((byte)(cc.rgbResult >> 16))) <= 384)
		Main.Text[25].rgb = COLOR_WHITE; else Main.Text[25].rgb = COLOR_BLACK;//颜色较浅时字体为黑色，反之亦然
	return cc.rgbResult;//增减text时记得更改"25"这个数组
}
BOOL CALLBACK CatchThread(HWND hwnd, LPARAM lParam)//捕捉窗口.
{
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID;
	if (GetParent(hwnd) == 0)
	{//如果是顶级窗口(没有父窗口的窗口)
		::GetWindowThreadProcessId(hwnd, &nProcessID);//窗口在被捕名单上且可见
		if (Eatpid[nProcessID] == true && IsWindowVisible(hwnd))
		{
			RECT rc, rc2;
			GetWindowRect(hwnd, &rc);
			if (rc.bottom<0 || rc.right<0 || rc.left> GetSystemMetrics(SM_CXSCREEN) || rc.top> GetSystemMetrics(SM_CYSCREEN))return 1;//不在屏幕范围内的窗口将不被捕捉
			if (rc.right - rc.left < 15 || rc.bottom - rc.top < 15)return 1;//过小的窗口将不被捕捉
			if (SetParent(hwnd, CatchWnd) != NULL)
			{
				EatList[0]++;//捕捉并将hWnd加入到EatList中
				EatList[(size_t)(EatList[0])] = hwnd;
				GetClientRect(CatchWnd, &rc2);
				GetWindowRect(hwnd, &rc);
				if (rc.top >= rc2.bottom || rc.left >= rc2.right)SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOSIZE);
			}
		}
	}
	return 1;
}
void UpdateCatchedWindows()
{//刷新主界面上"已捕捉 X 个窗口"这个字符串
	wchar_t tmpstr[MY_MAX_PATH], tmpnum[11];
	_itow_s((int)(size_t)EatList[0], tmpnum, 10);
	wcscpy_s(tmpstr, Main.GetStr(L"Eat1"));
	wcscat_s(tmpstr, tmpnum);
	wcscat_s(tmpstr, Main.GetStr(L"Eat2"));
	Main.SetStr(tmpstr, L"_Eat");
	if (Main.CurWnd != 1)return;
	Main.Readd(4, 5);
	RECT rc{ (long)(195 * Main.DPI),(long)(480 * Main.DPI),(long)(500 * Main.DPI),(long)(505 * Main.DPI) };
	Main.es[++Main.es[0].top] = rc;
	Main.Redraw(rc);
}
void ReturnWindows()//归还窗口.
{
	if (tdhw != 0 && tdhw2 != 0 && tdhw2 != (HWND)-1)
	{
		KillTimer(Main.hWnd, TIMER_CATCHEDTD);
		SetParent(tdhw, NULL);
		SetWindowPos(tdhw, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER);
		tdhw2 = tdhw = 0;
	}
	for (size_t i = 1; i <= (size_t)(EatList[0]); ++i)
	{//直接弹出栈内所有元素
		SetParent(EatList[i], NULL);
	}EatList[0] = 0;
	UpdateCatchedWindows();
}

bool CatchWindows()//经过延时后正式开始捕捉窗口
{
	wchar_t tmp[4] = { 0 }, * a = Main.Edit[EDIT_PROCNAME].str;
	for (int i = 0; i < 3; ++i)if (a[i] != 0)tmp[i] = a[i]; else break;
	_wcslwr_s(tmp);
	Eatpid.clear();//清空
	PROCESSENTRY32 pe;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return false;
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = '\0';//把要吃的程序的pid加进去
		_wcslwr_s(pe.szExeFile);
		if (wcsstr(tmp, pe.szExeFile) != 0)Eatpid[pe.th32ProcessID] = true;
	}
	EnumWindows(CatchThread, NULL);
	UpdateCatchedWindows();
	return true;
}
void MyExitProcess()
{//退出前进行清理.
	if (EatList[0] != 0)ReturnWindows();
	if (MouseHook != NULL)UnhookWindowsHookEx(MouseHook);
	if (KeyboardHook != NULL)UnhookWindowsHookEx(KeyboardHook);
	if (DeleteFileHandle != 0)UnloadNTDriver(L"DeleteFile");
	UnloadNTDriver(L"KProcessHacker2");
	ExitProcess(0);
}
bool RefreshTDstate()//刷新极域的状态
{
	RECT rc = { (LONG)(165 * Main.DPI), (LONG)(400 * Main.DPI),(LONG)(320 * Main.DPI),(LONG)(505 * Main.DPI) };
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//记录着极域进程工具frame位置的rc
	wchar_t myTD[4] = { 0 }; myTD[0] = TDName[0]; myTD[1] = TDName[1]; myTD[2] = TDName[2];
	_wcslwr_s(myTD);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return false;

	while (Process32Next(hSnapShot, &pe))
	{//寻找极域进程
		pe.szExeFile[3] = 0;
		_wcslwr_s(pe.szExeFile);
		if (wcscmp(myTD, pe.szExeFile) == 0)
		{//极域被启动了
			wchar_t tmpStr[101] = { 0 }, numStr[11] = { 0 };
			if (TDPID != 0)return true;//如果已经知道studentmain.exe的pid则退出
			TDPID = pe.th32ProcessID;//否则设置stu的pid
			TDProcess = true;

			wcscpy_s(tmpStr, Main.GetStr(L"_TDPID"));
			_itow_s(TDPID, numStr, 10);
			wcscat_s(tmpStr, numStr);
			Main.SetStr(tmpStr, L"TDPID");
			wcscpy_s(Main.Text[10].Name, L"TcmdOK");
			Main.Text[10].rgb = COLOR_GREENEST;//设置一些按钮的状态，然后重绘
			Main.Button[Main.GetNumbyID(L"kill-TD")].Enabled = true;
			Main.Button[Main.GetNumbyID(L"re-TD")].Enabled = false;
			Main.Readd(4, 8); Main.Readd(4, 9); Main.Readd(4, 10);
			Main.Readd(2, Main.GetNumbyID(L"kill-TD"));
			Main.Readd(2, Main.GetNumbyID(L"re-TD"));
			Main.Erase(rc);
			Main.Redraw(rc);
			return true;
		}
	}
	if (TDPID == 0)return true;
	TDPID = 0;//极域原来存在，但现在被结束了
	TDProcess = false;
	wchar_t tmpStr[101];
	wcscpy_s(tmpStr, Main.GetStr(L"_TDPID"));
	wcscat_s(tmpStr, L"\\");
	Main.SetStr(tmpStr, L"TDPID");
	wcscpy_s(Main.Text[10].Name, L"TcmdNO");
	Main.Text[10].rgb = COLOR_RED;//设置一些按钮的状态，然后重绘
	Main.Button[Main.GetNumbyID(L"re-TD")].Enabled = true;
	Main.Button[Main.GetNumbyID(L"kill-TD")].Enabled = false;
	Main.Readd(4, 8); Main.Readd(4, 9); Main.Readd(4, 10);
	Main.Readd(2, Main.GetNumbyID(L"kill-TD"));
	Main.Readd(2, Main.GetNumbyID(L"re-TD"));
	Main.Erase(rc);
	Main.Redraw(rc);
	return true;
}
void CreateDownload(int cur)
{//创建下载游戏线程的外壳函数.
	CreateThread(NULL, 0, DownloadThread, &cur, 0, NULL);
	Sleep(1);//新线程内需要用到传入的参数，
	return;//有时候线程内还没备份好参数，主线程内就把参数清空了
}//因此这里最好延迟1ms.

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)//主计时器
{
	UNREFERENCED_PARAMETER(hWnd); UNREFERENCED_PARAMETER(nMsg); UNREFERENCED_PARAMETER(dwTime);
	switch (nTimerid)
	{
	case TIMER_KILLPROCESS://连续结束进程
	{
		if (Main.Check[CHK_REKILL].Value == 1)
			if (!KillProcess(Main.Edit[EDIT_TDNAME].str))
			{
				Main.Check[CHK_REKILL].Value = 0;
				KillTimer(hWnd, TIMER_KILLPROCESS);//结束进程_全部_失败则退出
				RECT rc(Main.GetRECTc(CHK_REKILL));
				Main.Readd(3, CHK_REKILL);
				Main.Redraw(rc);
				error();
			}
		break;
	}
	case TIMER_CATCHWINDOW://延时捕捉窗口
	{
		timerleft--;
		if (timerleft >= 0)
		{
			if (!IsWindowVisible(CatchWnd))
			{
				timerleft = -1;
				wcscpy_s(Main.Button[BUT_CATCH].Name, Main.GetStr(L"back"));
				Main.EnableButton(BUT_CATCH, true);
				KillTimer(Main.hWnd, TIMER_CATCHWINDOW);
			}
			wchar_t tmp[301], tmp2[11];
			wcscpy_s(tmp, Main.GetStr(L"Timer1"));
			_itow_s(timerleft, tmp2, 10);
			wcscat_s(tmp, tmp2);
			wcscat_s(tmp, Main.GetStr(L"Timer2"));
			wcscpy_s(Main.Button[BUT_CATCH].Name, tmp);//拼接按钮中"剩余XX秒"的文字
			Main.EnableButton(BUT_CATCH, false);
			if (timerleft == 0)CatchWindows();
		}
		if (timerleft == -1)KillTimer(Main.hWnd, TIMER_CATCHWINDOW), wcscpy_s(Main.Button[BUT_CATCH].Name, Main.GetStr(L"back")), Main.EnableButton(BUT_CATCH, true);
		break;
	}
	case TIMER_COPYLEFT://copyleft
	{
		EasterEggState = (EasterEggState + 1) % 11;
		wchar_t CopyLeftstr[101] = L"Copy";
		wcscat_s(CopyLeftstr, EasterEggStr[EasterEggState % 11]);
		wcscat_s(CopyLeftstr, Main.GetStr(L"Tleft"));
		Main.SetStr(CopyLeftstr, L"_Tleft");
		Main.Redraw();
		break;
	}
	case TIMER_EXPLAINATION://定时创建exp & 其他功能
	{
		if (--sdl == 0)CreateDownload(8);
		if (GetForegroundWindow() != Main.hWnd)
		{//slow
			Main.EditUnHotKey();
		}
		if (GetTickCount() - Main.Timer >= 1000 && Main.ExpExist == false)Main.Try2CreateExp();
		break;
	}
	case TIMER_BUTTONEFFECT://按钮特效
	{
		if (!Main.ButtonEffect)break;
		for (int i = 1; i <= Main.CurButton; ++i)
		{
			if (Main.CoverButton != i && Main.Button[i].Percent > 0)
			{//不在鼠标上的按钮颜色逐渐变淡
				Main.Button[i].Percent -= Delta;
				if (Main.Button[i].Percent < 0)Main.Button[i].Percent = 0;
				Main.Readd(2, i);
				RECT rc = Main.GetRECT(i);
				Main.Redraw(rc);
			}
		}
		if (Main.CoverButton != -1 && Main.Button[Main.CoverButton].Percent < 100)
		{//在鼠标上的按钮颜色以双倍速度变深
			Main.Button[Main.CoverButton].Percent += 2 * Delta;
			if (Main.Button[Main.CoverButton].Percent > 100)Main.Button[Main.CoverButton].Percent = 100;
			Main.Readd(2, Main.CoverButton);
			RECT rc = Main.GetRECT(Main.CoverButton);
			Main.Redraw(rc);
		}
		break;
	}
	case TIMER_UPDATECATCH:
		InvalidateRect(CatchWnd, NULL, FALSE);
		UpdateWindow(CatchWnd);
		break;
	case TIMER_ANTYHOOK://刷新鼠标键盘钩子
		if (Main.Check[CHK_NOHOOK].Value == 1)
		{//只有最后加入的钩子才有效，这样极域就没法用钩子来禁用键盘了
			MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInst, 0);
			KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
		}
		break;
	case TIMER_REFRESH_TD://刷新极域状态
		if (Main.CurWnd == 2)RefreshTDstate();
		break;//200ms刷新一次，经测试效果不错
	case TIMER_UPDATEBSOD://刷新伪装蓝屏窗口上的文字
		BSODstate++;
		if (BSODstate == 8 || BSODstate == 20 || BSODstate == 40)InvalidateRect(BSODhwnd, NULL, FALSE), UpdateWindow(BSODhwnd);
		break;
	case TIMER_TOP://循环置顶(低画质时启用)
		if (Main.ButtonEffect)CreateThread(NULL, 0, TopThread, NULL, 0, NULL), KillTimer(Main.hWnd, TIMER_TOP);
		SetWindowPos(Main.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
		if (CatchWnd != NULL)SetWindowPos(CatchWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
		break;
	case TIMER_TOOLBAR:
		if (GetTickCount() - FakeTimer > 2200)
		{//鼠标离开约3s后收回伪装工具条(新)
			RECT rc;
			GetWindowRect(FakeWnd, &rc);
			if (rc.bottom == 63 && FakeLock == false && FakenewUp == false)
			{
				FakeTimer = GetTickCount();
				FakenewUp = true;
				int typ = -1;//这里只创建线程
				CreateThread(NULL, 0, FakeNewThread, &typ, 0, NULL);
				Sleep(1);
			}
		}
		break;
	case TIMER_CATCHEDTD:
	{
		RECT rc;
		GetClientRect(CatchWnd, &rc);
		if (!SetWindowPos(tdhw, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOREDRAW))
		{
			if (tdhwscr != NULL)DestroyWindow(tdhwscr), tdhwscr = 0;
			KillTimer(Main.hWnd, TIMER_CATCHEDTD);
		}
		break;
	}
	}
}

DWORD WINAPI SDThread(LPVOID pM)
{//useless?
	(pM);
	if (hCurrentDesk == defaultDesk)//从原始桌面切换到新桌面
	{
		HDESK vdback = OpenDesktopW(szVDesk, DF_ALLOWOTHERACCOUNTHOOK, false, GENERIC_ALL);
		SetThreadDesktop(vdback);
		SwitchDesktop(vdback);
		hCurrentDesk = vdback;
		while (hCurrentDesk == vdback)Sleep(100);
	}
	else//切换回来
	{

		SetThreadDesktop(defaultDesk);
		SwitchDesktop(defaultDesk);
		hCurrentDesk = defaultDesk;//设置当前桌面
		while (hCurrentDesk == defaultDesk)Sleep(100);
	}
	return 0;
}
void openfile()//显示"打开文件"的对话框
{
	OPENFILENAMEW ofn = { 0 };
	wchar_t strFile[MY_MAX_PATH] = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = (LPWSTR)strFile;
	ofn.nMaxFile = MY_MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn) == TRUE)Main.SetEditStrOrFont(strFile, 0, EDIT_FILEVIEW);
	Main.EditRedraw(EDIT_FILEVIEW);//将结果设置到Edit中
}
void BrowseFolder()//显示"打开文件夹"的对话框
{
	wchar_t path[MY_MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = Main.GetStr(L"OpenFolder");//标题
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		SHGetPathFromIDList(pidl, path);
		SetCurrentDirectory(path);//设置"当前工作目录"为选择的文件夹的上个目录
		Main.SetEditStrOrFont(path, 0, EDIT_FILEVIEW);
		Main.EditRedraw(EDIT_FILEVIEW);//将结果设置到Edit中
	}
}
void ClearUp()//清理文件并退出
{
	KillProcess(L"hook.exe");
	if (Admin)if (EnableTADeleter())Main.Check[CHK_T_A_].Value = TRUE;
	DeleteFile(SethcPath);//恢复sethc
	CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE);
	AutoDelete(TempPath, true);//删除SAtemp文件夹
	if (Main.Check[CHK_T_A_].Value)AutoDelete(Name, true);
	MyExitProcess();
}
void ShutdownDeleter()
{
	SearchTool(L"C:\\Program Files\\Mythware", 2);
	SearchTool(L"C:\\Program Files\\TopDomain", 2);
	SearchTool(L"C:\\Windows\\System32", 2);//仍然是各个目录寻找
	SearchTool(L"C:\\Program Files (x86)\\Mythware", 2);
	SearchTool(L"C:\\Program Files (x86)\\TopDomain", 2);
	SearchTool(L"C:\\Windows\\SysNative", 2);//System目录删除时可能有点慢
}
DWORD WINAPI ShutdownDeleterThread(LPVOID pM)
{
	(pM);
	Main.EnableButton(BUT_SHUTD, false);
	ShutdownDeleter();
	Main.EnableButton(BUT_SHUTD, true);
	return 0;
}

void AutoTerminateTD()
{
	if (!KillProcess(TDName) && Admin)
	{
		if (EnableKPH())
		{//结束失败后尝试用KPH
			Main.Check[CHK_KPH].Value = true;
			KillProcess(TDName);
		}
	}
}
void ReopenTD()
{
	if (GetFileAttributes(TDPath) != -1)
	{
		if (Main.Check[CHK_TDINVD].Value == 1)
		{
			STARTUPINFO si = { 0 };
			si.cb = sizeof(si);
			si.lpDesktop = szVDesk;//可能要在虚拟桌面里运行
			RunEXE(TDPath, NULL, &si);
		}
		else RunEXE(TDPath, NULL, nullptr);
	}
}
DWORD WINAPI ReopenThread(LPVOID pM)//尝试打开极域
{//(仅用于创建线程)
	(pM);
	if (Main.Button[BUT_RETD].Enabled == false)return 0;
	if (!TDsearched)
	{
		TDsearched = true;
		if (!slient)
			Main.EnableButton(BUT_RETD, false);
		//各种目录都找一遍就行了
		TDSearchDirect();
		if (!slient)
			Main.EnableButton(BUT_RETD, true);
		SetTDPathStr();
	}
	ReopenTD();
	if (slient)MyExitProcess();
	return 0;
}

bool RunHOOK()//运行hook.exe
{
	wchar_t tmp[501];
	wcscpy_s(tmp, TempPath);
	wcscat_s(tmp, L"hook.exe");
	if (GetFileAttributes(tmp) == -1)ReleaseRes(tmp, FILE_HOOK, L"JPG");//如果hook.exe不存在就从资源文件中释放
	return RunEXE(tmp, CREATE_NO_WINDOW, nullptr);
}
void FakeBSOD()//召唤伪装蓝屏的窗口.
{//这个函数应当只在BSODdesk中启动的另一个进程里被调用
	if (FB == TRUE)
	{//初始化
		FB = FALSE;
		MyRegisterClass(hInst, BSODProc, BSODWindow, false);
	}
	BSODhwnd = CreateWindow(BSODWindow, L"BSOD wnd", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInst, nullptr);
	BSODstate = 0;//创建窗口
	SetTimer(BSODhwnd, TIMER_UPDATEBSOD, 100, (TIMERPROC)TimerProc);
	InvalidateRect(BSODhwnd, NULL, FALSE);
	UpdateWindow(BSODhwnd);//之后的工作在BSODProc里
	ShowWindow(BSODhwnd, SW_SHOW);
}
void BSOD()//尝试蓝屏.
{
	LockCursor();//锁住鼠标
	//默认使用伪装蓝屏 + NtShutdown

	wchar_t tmp[340];
	HDESK VirtualDesk = CreateDesktop(fBSODdesk, NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);
	//有时候主桌面会运行个任务管理器什么的程序，能够浮在POPUP窗口的上面
	STARTUPINFO si = { 0 };//这样伪装蓝屏就露馅了
	si.cb = sizeof(si);//所以我们创建一个虚拟桌面，把蓝屏窗口显示在本来空无一物的那里比较安全
	si.lpDesktop = fBSODdesk;

	bool oldBSOD = true;
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);//检查一次版本号，小于8000则应是win7或更旧的系统
	if (bOsVersionInfoEx && osvi.dwBuildNumber > 8000)oldBSOD = false;//此时默认用旧版蓝屏

	if (oldBSOD)
		wcscpy_s(tmp, L"d -showbsod \"old\"");
	else//命令行调用，由另一个GUI.exe进程完成蓝屏
		wcscpy_s(tmp, L"d -showbsod \"new\"");
	if (wcslen(CurrentLanguage) != 0)
	{//其他文字的蓝屏选项
		wcscat_s(tmp, L" /");
		wcscat_s(tmp, CurrentLanguage);
	}
	PROCESS_INFORMATION pi = { 0 };

	CreateProcess(Name, tmp, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
	BlackBoard = true;
	MyRegisterClass(hInst, ScreenProc, ScreenWindow, false);
	HWND t = CreateWindow(ScreenWindow, L"You can't see me.", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInst, nullptr);
	ShowWindow(t, SW_SHOW);
	for (int i = 0; i < 10; ++i)InvalidateRect(t, NULL, false), UpdateWindow(t);
	//切换到那个桌面去
	SwitchDesktop(VirtualDesk);//另一个程序启动也需要时间，在这之前打开一个黑屏窗口过渡

	Sleep(4000);//延迟一段时间
	RestartDirect();//重启
}
VOID Restart()//瞬间重启
{
	RestartDirect();
	BlackBoard = true;
	MyRegisterClass(hInst, ScreenProc, ScreenWindow, false);
	HWND t = CreateWindow(ScreenWindow, L"You can't see me.", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInst, nullptr);
	ShowWindow(t, SW_SHOW);
	LockCursor();
	KillProcess(L"Taskmgr.exe");//关闭可能浮在自己上面的任务管理器
	KillFullScreen();//关闭全屏的窗口
}
void EasterEgg(bool flag)//开关easteregg(计时器)
{
	if (flag)
		SetTimer(Main.hWnd, TIMER_COPYLEFT, 80, (TIMERPROC)TimerProc), EasterEggFlag = true;
	else
		KillTimer(Main.hWnd, TIMER_COPYLEFT), EasterEggFlag = false;
}

void AutoChangeChannel(int ChannelID)//自动更改广播频道
{
	HKEY hKey;
	LONG ret;
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class\\Student", 0, KEY_SET_VALUE, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student", 0, KEY_SET_VALUE, &hKey);
	if (ret != 0)
	{
		Main.InfoBox(L"ACFail");//打不开键值
		RegCloseKey(hKey);
		return;
	}
	ret = RegSetValueEx(hKey, L"ChannelId", 0, REG_DWORD, (const BYTE*)&ChannelID, sizeof(int));
	if (ret != 0)
	{
		Main.InfoBox(L"ACUKE");//打开了，但是设置不了键值(一般不会这样)
		RegCloseKey(hKey);
		return;
	}
	RegCloseKey(hKey);
	Main.InfoBox(L"ACOK");//成功
	return;
}
void AutoPassBox(const wchar_t* str)
{//自动将获得的密码用MessageBox显示

	wchar_t Tmp[300];
	wcscpy_s(Tmp, Main.GetStr(L"pswdis"));
	wcscat_s(Tmp, L"\"");
	wcscat_s(Tmp, str);
	wcscat_s(Tmp, L"\"");
	if (slient) { Main.InfoBox(Tmp); return; }
	wcscat_s(Tmp, Main.GetStr(L"Copypswd"));
	if (MessageBox(Main.hWnd, Tmp, Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
	{
		char* ClipBoardStr = new char[wcslen(str) * 2 + 1];
		WideCharToMultiByte(CP_ACP, 0, str, -1, ClipBoardStr, 0xffff, NULL, NULL);
		if (OpenClipboard(Main.hWnd))
		{
			HGLOBAL clipbuffer;
			char* buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(ClipBoardStr) + 1);
			buffer = (char*)GlobalLock(clipbuffer);
			if (buffer != NULL)strcpy(buffer, LPCSTR(ClipBoardStr));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
	}
}

void AutoViewPass()//读取密码并显示
{
	HKEY hKey;//读取文件夹的handle
	LONG ret;//是否读取键值成功的状态
	wchar_t szLocation[300] = { 0 };//接受读取内容的字符串
	DWORD dwSize = sizeof(wchar_t) * 300;//字符串的大小
	DWORD dwType = REG_SZ;//键值的种类(字符串)
	BYTE* a;
	bool error = false;
	BYTE bits[300] = { 0 };
	int start, cur = 0;//一些其他的临时变量
	DWORD dwType2 = REG_BINARY, dwSize2 = 300;
	wchar_t str[100] = { 0 };

	if (Bit != 64)//首先尝试读取未加密过的(如明文密码或前面加Passwd的明文密码)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	else//尝试打开键值
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	if (ret != 0 && slient == false)//打不开文件夹
	{
		if (slient) { error = true; goto finish; }//命令行调用时直接结束
		if (MessageBox(Main.hWnd, Main.GetStr(L"VPFail"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)goto finish;
	}

	ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType, (LPBYTE)&szLocation, &dwSize);//尝试读取键值
	if (ret != 0 && slient == false)goto encrypted;//读取不了键值，尝试到knock中读取
	if (wcslen(szLocation) != 0)//读取到了明文密码
	{
		if (wcsstr(szLocation, L"Passwd") != 0)//可能是带passwd的密码
		{
			if (wcsstr(szLocation, L"[123456]") != 0)goto encrypted;//密码中含有带方括号的123456，很可能是加密过的
			else
				AutoPassBox(szLocation + 6);//显示读取结果
		}
		else AutoPassBox(szLocation);//显示读取结果
	}
	else Main.InfoBox(L"VPNULL");
	goto finish;

encrypted:
	if (hKey != 0)RegCloseKey(hKey);
	if (Bit != 64)//然后尝试读取加密后的密码
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class\\Student", 0, KEY_QUERY_VALUE, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student", 0, KEY_QUERY_VALUE, &hKey);
	if (ret != 0 && slient == false)//打不开文件夹
	{
		if (slient) { error = true; goto finish; }//命令行调用时直接结束
		if (MessageBox(Main.hWnd, Main.GetStr(L"VPFail"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)goto finish;
	}

	ret = RegQueryValueExW(hKey, L"Knock1", 0, &dwType2, (LPBYTE)bits, &dwSize2);//尝试读取键值
	if (ret != 0)//读取不了键值,尝试从Knock中读取	
	{
		ret = RegQueryValueExW(hKey, L"Knock", 0, &dwType2, (LPBYTE)bits, &dwSize2);
		if (ret != 0)goto encrypted2;//如果Knock和Knock1都不存在，那么尝试从Key中读取
	}
	a = bits;
	while (*a != 0)
	{//从头到尾把密文和0x150f0f15异或
		*(DWORD*)a ^= 0x150f0f15u;
		a += 4;
	}
	start = bits[0];
	a = bits + start;//读取出来的内容中第一个字节存有有效数据的位置

	while ((*a != 0) || (*(a + 1) != 0))
	{//把单个字节的数据拼接成字符串
		str[cur] = (*(a + 1) << 8) + (*(a));
		a += 2;
		cur++;
	}
	if (wcslen(str) == 0)Main.InfoBox(L"VPNULL"); else AutoPassBox(str);
	goto finish;

encrypted2:
	if (hKey != 0)RegCloseKey(hKey);
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	else//这里是2015年版的异或加密，保存在Key中
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	if (ret != 0 && slient == false)//打不开文件夹
	{
		if (slient) { error = true; goto finish; }//命令行调用时直接结束
		if (MessageBox(Main.hWnd, Main.GetStr(L"VPFail"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)goto finish;
	}

	ret = RegQueryValueExW(hKey, L"Key", 0, &dwType2, (LPBYTE)&bits, &dwSize2);//尝试读取键值
	if (ret != 0 && slient == false)//打不开文件夹
	{
		if (slient) { error = true; goto finish; }//命令行调用时直接结束
		if (MessageBox(Main.hWnd, Main.GetStr(L"VPFail"), Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)goto finish;
	}

	for (int i = 0; i < 150; ++i)
	{//从头到尾把密文和0x4350u异或
		if (bits[i * 2] == 0 && bits[i * 2 + 1] == 0)break;
		bits[i * 2] ^= 0x50;
		bits[i * 2 + 1] ^= 0x43;
	}
	ZeroMemory(str, sizeof(str));
	a = bits;
	while ((*a != 0) || (*(a + 1) != 0))
	{//把单个字节的数据拼接成字符串
		str[cur] = (*(a + 1) << 8) + (*(a));
		a += 2;
		cur++;
	}
	if (wcslen(str) == 0)Main.InfoBox(L"VPNULL"); else AutoPassBox(str);
finish:
	if (error)Main.InfoBox(L"VPNULL");
	RegCloseKey(hKey);
	return;
}

void AutoChangePassword(wchar_t* a, int type)//自动更改密码
{
	wchar_t tmp[201] = { 0 };
	HKEY hKey; LONG ret; DWORD dwType2 = REG_SZ, dwSize2 = 201, dwType = REG_BINARY;
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
	else//打开键值
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
	if (ret != 0) { Main.InfoBox(L"ACFail"); RegCloseKey(hKey); return; }//打开失败
	if (type == 1)
	{
		ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType2, (LPBYTE)tmp, &dwSize2);//尝试读取键值
		if (wcsstr(tmp, L"Passwd[123456]") != 0 || ret != 0)goto nobasic;
	}
	ZeroMemory(tmp, sizeof(tmp));
	if (type == 2)wcscpy_s(tmp, L"Passwd"), wcscat_s(tmp, a);//是否在之前加上Passwd
	else wcscpy_s(tmp, a);
	ret = RegSetValueEx(hKey, L"UninstallPasswd", 0, REG_SZ, (const BYTE*)&tmp, sizeof(wchar_t) * (DWORD)wcslen(tmp));
	if (ret != 0) { Main.InfoBox(L"ACUKE"); RegCloseKey(hKey); return; }//失败
nobasic:
	RegCloseKey(hKey);
	if (type == 1)//额外修改key和knock异或加密
	{
		wcscpy_s(tmp, L"Passwd");//下面说说knock密码的加密方式
		BYTE data[2000];//这是2018年用IDA研究出来的，以后加密方式可能会变
		wcscat(tmp, a);//具体就是，采用UNICODE编码，转化为二进制后
		int len = (int)wcslen(tmp) * 2;//把他们和0x4350u异或，得到的值前8位存在一个BYTE数组的偶数位，
		for (int i = 0; i < len; ++i)tmp[i] ^= 0x4350u;//后8位存在一个BYTE数组的奇数位(C语言中数组是从0开始的)
		for (int i = 0; i < len; ++i)//有时密码比较短，后面的空位直接用随机数值补齐
		{//这样就使得密码很多时候看起来像是非对称加密得来的，有一定迷惑性(吧)
			data[i * 2 + 1] = tmp[i] >> 8;
			data[i * 2] = (BYTE)tmp[i] - (tmp[i] >> 8 << 8);
		}//然后，就没了。只是一个异或而已，强度并不大。
		if (Bit != 64)
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
		else//这里是2015年版的异或加密，保存在Key中
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);

		ret = RegQueryValueExW(hKey, L"Key", 0, &dwType, (LPBYTE)tmp, &dwSize2);//尝试读取键值
		if (ret != 0)goto nokey;

		RegSetValueEx(hKey, L"Key", 0, REG_BINARY, (const BYTE*)data, sizeof(char) * len);
	nokey:
		if (Bit != 64)change(a, false); else change(a, true);//这里是2016版的异或加密，保存在Knock中
	}
	Main.InfoBox(L"ACOK");
	return;
}

void AutoCreateCatchWnd()
{
	if (FC == TRUE)//注册类
		MyRegisterClass(hInst, CatchProc, CatchWindow, false),
		FC = FALSE;//First start CatchWnd

	if (CatchWnd != 0)
	{
		if (!IsWindowVisible(CatchWnd))ShowWindow(CatchWnd, SW_SHOW);
		return;
	}
	CatchWnd = CreateWindowW(CatchWindow, Main.GetStr(L"Title2"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, nullptr, nullptr, hInst, nullptr);
	ShowWindow(CatchWnd, SW_SHOW);
}
bool RunCmdLine(LPWSTR str)//解析启动时的命令行并执行
{
	BOOL console; DWORD pid;
	_wcslwr(str);
	if (GetSystemDefaultLangID() == 0x0409)
	{
		wchar_t tmp[MY_MAX_PATH];//英文系统上自动切换语言
		wcscpy_s(tmp, TempPath);
		wcscat_s(tmp, L"language\\");//创建language目录
		CreateDirectory(tmp, NULL);
		wcscat_s(tmp, L"English.ini");
		ReleaseRes(tmp, FILE_ENG, L"JPG");
		SwitchLanguage(tmp);
	}

	if (wcsstr(str, L"-showbsod") != NULL)
	{//显示伪装蓝屏
		slient = true;
		typedef DWORD(CALLBACK* SEtProcessDPIAware)(void);
		SEtProcessDPIAware SetProcessDPIAware;
		HMODULE huser;
		huser = LoadLibrary(L"user32.dll");//显示蓝屏界面时需要关闭系统的DPI缩放
		SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
		if (SetProcessDPIAware != NULL)SetProcessDPIAware();
		wchar_t tmp[1001] = { 0 }, * tmp1 = wcsstr(str, L"-showbsod"), * tmp2 = 0;
		tmp2 = wcsstr(str, L"/");
		if (tmp2 != 0)SwitchLanguage(tmp2 + 1);
		if (!Findquotations(tmp1, tmp)) {
			FakeBSOD(); goto noreturn;
		}
		if (wcscmp(tmp, L"old") == 0)Main.Check[17].Value = true;
		FakeBSOD();
		goto noreturn;
	}
	if (wcsstr(str, L"-top") != NULL)//显示在安全桌面上用
	{
		Main.Check[CHK_TOP].Value = 1;
		TOP = TRUE;
		CreateThread(NULL, 0, TopThread, NULL, 0, NULL);
		goto noreturn;//false表示继续运行
	}
	pid = GetParentProcessID(GetCurrentProcessId());
	console = AttachConsole(pid);//附加到父进程命令行上
	hdlWrite = GetStdHandle(STD_OUTPUT_HANDLE);
	//setlocale(LC_ALL, "chs");//这一行语句占30k大小，谨慎使用
	slient = true;

	if (wcsstr(str, L"-help") != NULL)//安装sethc
	{
		wchar_t tmp2[301];
		wcscpy_s(tmp2, TempPath);
		wcscat_s(tmp2, L"关于&帮助.txt");
		ReleaseRes(tmp2, FILE_HELP, L"JPG");
		DWORD NumberOfBytesRead;
		wchar_t AllTmp[10001], * point1, * point2;
		HANDLE hf = CreateFile(tmp2, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
		if (hf == 0)goto okreturn;
		ReadFile(hf, AllTmp, 10001, &NumberOfBytesRead, NULL);
		point1 = AllTmp;
		if (NumberOfBytesRead == 0)goto okreturn;
		for (int i = 1; i < HELP_END; ++i)
		{
			point2 = wcsstr(point1, L"\n");
			if (point2 == 0)break;
			*point2 = 0;
			if (i >= HELP_START)Main.InfoBox(point1);
			point1 = point2 + 1;
		}
		CloseHandle(hf);
		goto okreturn;
	}
	if (wcsstr(str, L"-sethc") != NULL)//安装sethc
	{
		if (!DeleteSethc()) { Main.InfoBox(L"DSR3Fail"); goto okreturn; }
		if (AutoSetupSethc())Main.InfoBox(L"suc");
		goto okreturn;
	}
	if (wcsstr(str, L"-auto") != NULL) { KillFullScreen(); Main.InfoBox(L"suc"); goto okreturn; }//自动结束置顶进程
	if (wcsstr(str, L"-unsethc") != NULL) {
		DeleteFile(SethcPath);
		if (CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE))Main.InfoBox(L"suc"); else Main.InfoBox(L"USFail");
		goto okreturn;
	}
	if (wcsstr(str, L"-viewpass") != NULL) { AutoViewPass(); goto okreturn; }
	if (wcsstr(str, L"-antishutdown") != NULL) { ShutdownDeleter(); Main.InfoBox(L"suc");  goto okreturn; }
	if (wcsstr(str, L"-reopen") != NULL) { TDSearchDirect(); ReopenTD(); if (TDPath[0] != 0) Main.InfoBox(L"suc"); goto okreturn; }//这些功能不细说了
	if (wcsstr(str, L"-bsod") != NULL) { BSOD(); goto okreturn; }//看 关于&帮助.txt 里面有介绍
	if (wcsstr(str, L"-restart") != NULL) { Restart(); goto okreturn; }
	if (wcsstr(str, L"-clear") != NULL) {
		wchar_t tmp[10] = { 0 }; AutoChangePassword(tmp, 1); goto okreturn;
	}
	if (wcsstr(str, L"-rekill") != NULL)
	{
		wchar_t tmp[1001], * tmp1 = wcsstr(str, L"-rekill");
		if (!Findquotations(tmp1, tmp))goto error;
		while (1) { KillProcess(tmp); Sleep(50); }
	}
	if (wcsstr(str, L"-delete") != NULL)
	{
		wchar_t tmp[1001], * tmp1 = wcsstr(str, L"-delete");
		if (!Findquotations(tmp1, tmp))goto error;
		AutoDelete(tmp, true);
		goto okreturn;
	}
	if (wcsstr(str, L"-changewithpasswd") != NULL)//二者顺序不能调换
	{//因为changewithpasswd包括change这个字符串
		wchar_t tmp[1001] = { 0 }, * tmp1 = wcsstr(str, L"-changewithpasswd");
		if (!Findquotations(tmp1, tmp))goto error;
		AutoChangePassword(tmp, 2);
		goto okreturn;
	}
	if (wcsstr(str, L"-channel") != NULL)
	{
		wchar_t tmp[1001] = { 0 }, * tmp1 = wcsstr(str, L"-channel");
		if (!Findquotations(tmp1, tmp))goto error;
		AutoChangeChannel(_wtoi(tmp));
		goto okreturn;
	}
	if (wcsstr(str, L"-change") != NULL)
	{
		wchar_t tmp[1001] = { 0 }, * tmp1 = wcsstr(str, L"-change");
		if (!Findquotations(tmp1, tmp))goto error;
		AutoChangePassword(tmp, 1);
		goto okreturn;
	}
error:
	Main.InfoBox(L"unkcmd");
okreturn:
	if (!console)return true;
	FreeConsole();
	keybd_event(13, 0, 0, 0);
	keybd_event(13, 0, KEYEVENTF_KEYUP, 0);
	return true;
noreturn:
	return false;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,//程序入口点
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{//和程序界面无关的初始化
	UNREFERENCED_PARAMETER(hPrevInstance);

	GetPath();//获取自身目录
	Admin = IsUserAnAdmin();//是否管理员

	GetBit();//获取位数
	if (Bit != 34)
		wcscpy_s(SethcPath, L"C:\\Windows\\System32\\sethc.exe");
	else//根据系统位数设置path
		wcscpy_s(SethcPath, L"C:\\Windows\\SysNative\\sethc.exe");

	FirstFlag = BackupSethc();//备份

	CreateStrs; //创建字符串
	if (*lpCmdLine != 0) { if (RunCmdLine(lpCmdLine) == true)return 0; }

	hVirtualDesk = CreateDesktop(szVDesk, NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);//创建虚拟桌面
	defaultDesk = OpenDesktopW(L"Default", DF_ALLOWOTHERACCOUNTHOOK, false, GENERIC_ALL);
	hCurrentDesk = defaultDesk;
	if (FirstFlag == TRUE)
	{//未备份 -> 程序可能在这台电脑上第一次启动 -> 虚拟桌面中运行explorer
		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		si.lpDesktop = szVDesk;
		RunEXE(ExplorerPath, NULL, &si);
	}

	typedef DWORD(CALLBACK* SEtProcessDPIAware)(void);
	SEtProcessDPIAware SetProcessDPIAware;
	HMODULE huser;//让系统不对这个程序进行缩放
	huser = LoadLibrary(L"user32.dll");//在一些笔记本上有用
	SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
	if (SetProcessDPIAware != NULL)SetProcessDPIAware();//为了自动设置DPI，这里提早调用

	if (!InitInstance(hInstance, nCmdShow))return FALSE;//和程序界面有关的初始化

	MSG msg;
	// 主消息循环: 
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);//分派消息
	}
	return (int)msg.wParam;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)//初始化
{
	InitBrushs;//创建画笔 & 画刷
	//放在TestFunctions.h里以减少GUI.cpp长度

	hInst = hInstance; // 将实例句柄存储在全局变量中

	int yLength = GetSystemMetrics(SM_CYSCREEN);
	if (yLength >= 1400)Main.DPI = 1.5;//根据屏幕高度预先设定缩放
	if (yLength <= 1000)Main.DPI = 0.75;
	if (yLength <= 700)Main.DPI = 0.6;

	Main.InitClass(hInst);//初始化主类
	if (!MyRegisterClass(hInst, WndProc, szWindowClass, true))return FALSE;//初始化Class
	Main.Obredraw = true;//默认使用ObjectRedraw
	Main.hWnd = CreateWindowEx(WS_EX_LAYERED, szWindowClass, Main.GetStr(L"Title"), WS_POPUP, 150, 150, 1, 1, NULL, nullptr, hInstance, nullptr);
	Main.Timer = GetTickCount();
	SetTimer(Main.hWnd, TIMER_EXPLAINATION, 500, (TIMERPROC)TimerProc);//开启Exp计时器
	CreateCaret(Main.hWnd, NULL, 1, 20);
	SetCaretBlinkTime(500);//初始化闪烁光标
	if (!Main.hWnd)return FALSE;//创建主窗口失败就直接退出
	if (Effect)
	{
		Main.ButtonEffect = true;//按钮渐变色特效
		SetTimer(Main.hWnd, TIMER_BUTTONEFFECT, 16, (TIMERPROC)TimerProc);//启用渐变色计时器 30fps
		SetLayeredWindowAttributes(Main.hWnd, NULL, 234, LWA_ALPHA);//半透明特效
	}
	//SetWindowLong(Main.hWnd, GWL_STYLE, GetWindowLong(Main.hWnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_SYSMENU & ~WS_GROUP & ~WS_TABSTOP);//无边框窗口
	//SetClassLong(Main.hWnd, GCL_STYLE, GetClassLong(Main.hWnd, GCL_STYLE) | CS_DROPSHADOW);
	hZXFBitmap = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ZXF1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);//资源文件中加载头像
	hZXFsign = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ZXF2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);//加载签名

	Main.CreateEditEx(325 + 5, 220, 110 - 10, 50, 1, L"explorer.exe", L"E_runinVD", 0, true);//创建输入框
	Main.CreateEditEx(195 + 5, 355, 240 - 10, 45, 1, L"StudentMain.exe", L"E_Pname", 0, true);
	Main.CreateEditEx(455 + 5, 355, 50 - 10, 45, 1, L"5", L"E_Delay", 0, true);
	Main.CreateEditEx(185 + 5, 102, 110 - 10, 40, 2, L"输入新端口", L"E_ApplyCh", 0, false);
	Main.CreateEditEx(365 + 5, 175, 210 - 10, 50, 2, L"输入新密码", L"E_CP", 0, false);
	Main.CreateEditEx(195 + 5, 102, 310 - 10, 37, 3, L"浏览文件/文件夹", L"E_View", 0, false);
	Main.CreateEditEx(277 + 5, 186, 138 - 10, 25, 5, L"StudentMain", L"E_TDname", 0, true);

	Main.CreateButtonEx(1, 1, 50, 139, 64, 0, L"主要功能", WhiteBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, true, 0, L"P1");//切换页面按钮
	Main.CreateButtonEx(2, 1, 115, 139, 64, 0, L"极域工具箱", WhiteBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, true, 0, L"P2");//
	Main.CreateButtonEx(3, 1, 180, 139, 64, 0, L"其他工具", WhiteBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, true, 0, L"P3");
	Main.CreateButtonEx(4, 1, 245, 139, 64, 0, L"关于", WhiteBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, true, 0, L"P4");
	Main.CreateButtonEx(5, 1, 310, 139, 64, 0, L"设置", WhiteBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, true, 0, L"P5");
	Main.CreateButtonEx(6, 1, 375, 139, 175, 0, L"一键安装", WhiteBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, true, 0, L"QuickSetup");
	Main.CurButton = 6;
	Main.CreateLine(140, 51, 140, 549, 0, COLOR_DARKER_GREY);
	Main.CreateLine(0, 51, 0, 549, 0, COLOR_DARKER_GREY);//切换页面按钮边上的线
	Main.CreateLine(2, 114, 139, 114, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 179, 139, 179, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 244, 139, 244, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 309, 139, 309, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 374, 139, 374, 0, COLOR_DARKER_GREY);
	Main.CreateLine(622, 51, 622, 549, 0, 0);

	Main.CreateFrame(170, 75, 415, 95, 1, L" 进程方案 ");
	Main.CreateButton(195, 100, 110, 50, 1, L"安装sethc", L"Sethc");//sethc
	Main.CreateButton(325, 100, 110, 50, 1, L"全局键盘钩子", L"hook");//hook
	if (SethcState == false)wcscpy_s(Main.Button[Main.CurButton - 1].Name, Main.GetStr(L"unQS"));//sethc已经被删除

	Main.CreateFrame(170, 195, 415, 320, 1, L" 窗口方案 ");
	Main.CreateButton(195, 220, 110, 50, 1, L"运行程序", L"runinVD");//虚拟桌面
	Main.CreateButton(455, 220, 110, 50, 1, L"切换桌面", L"SwitchD");
	Main.CreateText(195, 285, 1, L"Tctrl+b", COLOR_ORANGE);
	Main.CreateLine(185, 315, 565, 315, 1, 0);

	Main.CreateText(195, 330, 1, L"Processnam", COLOR_NORMAL_GREY);//想要捕捉的进程名
	Main.CreateText(463, 330, 1, L"Delay", COLOR_NORMAL_GREY);//延迟时间
	Main.CreateText(510, 370, 1, L"second", COLOR_NORMAL_GREY);//s
	Main.CreateButton(195, 415, 110, 50, 1, L"捕捉窗口", L"CatchW");
	Main.CreateButton(325, 415, 110, 50, 1, L"监视窗口", L"CopyW");
	Main.CreateButton(455, 415, 110, 50, 1, L"释放窗口", L"ReturnW");//13
	Main.CreateText(195, 480, 1, L"_Eat", COLOR_DARKER_BLUE);

	Main.CreateFrame(160, 75, 160, 146, 2, L" 频道工具 ");
	Main.CreateButton(185, 155, 110, 45, 2, L"应用", L"ApplyCh");

	Main.CreateFrame(345, 75, 250, 280, 2, L" 管理员密码工具 ");
	Main.CreateButton(365, 102, 97, 45, 2, L"清空密码", L"ClearPass");
	Main.CreateButton(477, 102, 97, 45, 2, L"查看密码", L"ViewPass");
	Main.CreateButton(365, 237, 97, 50, 2, L"改密方案1", L"CP1");
	Main.CreateButton(477, 237, 97, 50, 2, L"改密方案2", L"CP2");
	Main.CreateText(365, 300, 2, L"Tcp1", COLOR_DARKEST_GREY);
	Main.CreateText(365, 325, 2, L"Tcp2", COLOR_ORANGE);
	Main.CreateLine(360, 160, 583, 160, 2, 0);

	Main.CreateCheck(165, 238, 2, 135, L" 伪装工具条旧");
	Main.CreateCheck(165, 264, 2, 135, L" 伪装工具条新");
	Main.CreateCheck(165, 290, 2, 135, L" 伪装托盘图标");
	Main.CreateCheck(165, 316, 2, 170, L" 新桌面中启动极域");
	Main.CreateCheck(165, 342, 2, 170, L" Ctrl+Y 启动极域");

	Main.CreateFrame(160, 385, 160, 130, 2, L" 极域进程工具 ");
	Main.CreateText(175, 405, 2, L"TDState", 0);
	Main.CreateText(175, 430, 2, L"TDPID", 0);
	Main.CreateText(250, 405, 2, L"TcmdNO", COLOR_RED);
	Main.CreateButton(175, 458, 60, 42, 2, L"结束", L"kill-TD");
	Main.Button[Main.CurButton].Enabled = false;
	Main.CreateButton(245, 458, 60, 42, 2, L"启动", L"re-TD");

	Main.CreateButton(345, 385, 115, 55, 2, L"自动捕捉极域", L"catchTD");
	Main.CreateButton(480, 385, 115, 55, 2, L"防止教师关机", L"ANTI-");
	Main.CreateButton(345, 460, 115, 55, 2, L"显示于安全桌面", L"desktop");
	Main.CreateButton(480, 460, 115, 55, 2, L"自动防控制", L"auto-5");//24

	Main.CreateFrame(170, 75, 410, 150, 3, L" 文件粉碎机 ");
	Main.CreateButton(520, 102, 36, 36, 3, L"...", L"viewfile");
	Main.CreateButton(436, 151, 120, 55, 3, L"打开文件夹", L"folder");
	Main.CreateButton(325, 151, 97, 55, 3, L"开始粉碎", L"Delete");
	Main.CreateCheck(195, 160, 3, 100, L" 加载驱动");
	Main.CreateText(195, 185, 3, L"T.A.", 0);

	Main.CreateFrame(170, 255, 273, 105, 3, L" 电源 - 慎用 ");
	Main.CreateButton(192, 280, 115, 60, 3, L"BSOD(蓝屏)", L"BSOD");
	Main.CreateButton(324, 280, 100, 60, 3, L"快速重启", L"NtShutdown");

	Main.CreateFrame(170, 388, 410, 105, 3, L" 杂项 ");
	Main.CreateButton(192, 412, 100, 60, 3, L"ARP攻击", L"ARP");
	Main.CreateButton(304, 412, 140, 60, 3, L"System权限CMD", L"SuperCMD");
	Main.CreateButton(455, 412, 105, 60, 3, L"干掉360", L"Killer");//32

	Main.CreateText(325, 75, 4, L"Tcoder", NULL);//"关于"中的一堆文字
	Main.CreateText(325, 100, 4, L"Tver", NULL);
	Main.CreateText(372, 125, 4, L"Tver2", NULL);
	Main.CreateText(170, 255, 4, L"Ttip1", NULL);
	Main.CreateText(170, 280, 4, L"Ttip2", NULL);
	Main.CreateText(170, 315, 4, L"Tbit", NULL);//一些系统信息的text
	Main.CreateText(285, 315, 4, L"Twinver", NULL);
	Main.CreateText(455, 315, 4, L"Tcmd", NULL);
	Main.CreateText(170, 340, 4, L"TTDv", NULL);
	Main.CreateText(375, 340, 4, L"TIP", NULL);
	Main.CreateText(170, 365, 4, L"TPath", NULL);
	Main.CreateLine(170, 400, 590, 400, 4, 0);
	Main.CreateText(170, 417, 4, L"_Tleft", NULL);
	Main.CreateText(170, 442, 4, L"Tleft2", NULL);
	Main.CreateButton(490, 415, 100, 50, 4, L"更多", L"more.txt");
	Main.CreateButton(490, 477, 100, 50, 4, L"系统信息", L"sysinfo");//34

	Main.CreateCheck(180, 70, 5, 100, L" 窗口置顶");
	Main.CreateCheck(180, 100, 5, 160, L" Ctrl+R 紧急蓝屏");
	Main.CreateCheck(180, 130, 5, 160, L" Ctrl+T 快速重启");
	Main.CreateCheck(180, 160, 5, 200, L" Ctrl+Alt+P 截图/显示");
	Main.CreateCheck(180, 190, 5, 94, L" 连续结束                   .exe");
	Main.CreateCheck(180, 220, 5, 230, L" (结束进程) 完全匹配进程名");
	Main.CreateCheck(180, 250, 5, 130, L" 禁止键鼠钩子");
	Main.CreateCheck(180, 280, 5, 230, L" Ctrl+Alt+K 键盘操作鼠标");
	Main.CreateCheck(180, 310, 5, 90, L" 低画质");
	Main.CreateCheck(180, 340, 5, 110, L" 缩小/放大");
	Main.CreateCheck(180, 370, 5, 245, L" 使用ProcessHacker结束进程");

	Main.CreateButton(470, 410, 105, 50, 5, L"永久隐藏", L"hidest");
	Main.CreateButton(470, 470, 105, 50, 5, L"清理并退出", L"clearup");//36

	Main.CreateButton(466, 255, 115, 106, 3, L"打游戏", L"Games");//37
	Main.CreateFrame(655, 75, 170, 420, 0, L" 游戏 ");
	Main.CreateButton(680, 95, 120, 50, 0, L"小飞猜词", L"Game1");
	Main.CreateButton(680, 160, 120, 50, 0, L"Flappy Bird", L"Game2");
	Main.CreateButton(680, 225, 120, 50, 0, L"2048", L"Game3");
	Main.CreateButton(680, 290, 120, 50, 0, L"俄罗斯方块", L"Game4");
	Main.CreateButton(680, 355, 120, 50, 0, L"见缝插针", L"Game5");
	Main.CreateButton(680, 420, 120, 50, 0, L"五子棋", L"Game6");//43
	Main.CreateFrame(169, 69, 136, 171, 4, L"");

	Main.CreateArea(20, 10, 32, 32, 0);//极域图标
	Main.CreateArea(170, 70, 135, 170, 4);//zxf头像
	Main.CreateArea(170, 365, 80, 20, 4);
	if (!Admin)Main.CreateArea(176, 15, 97, 18, 0);//以管理员权限重启

	Main.CurButton++;

	if (Admin == 0)Main.CreateText(60, 17, 0, L"Tmain", COLOR_WHITE);
	else Main.CreateText(60, 17, 0, L"Tmain2", COLOR_WHITE);
	Main.CreateButtonEx(Main.CurButton, 530, 10, 60, 30, 0, L"×", \
		CreateSolidBrush(COLOR_CLOSE_LEAVE), CreateSolidBrush(COLOR_CLOSE_HOVER), CreateSolidBrush(COLOR_CLOSE_PRESS), \
		CreatePen(PS_SOLID, 1, COLOR_CLOSE_LEAVE), CreatePen(PS_SOLID, 1, COLOR_CLOSE_HOVER), CreatePen(PS_SOLID, 1, COLOR_CLOSE_PRESS), \
		Main.DefFont, 1, COLOR_WHITE, L"Close");//关闭 按钮 44

	SetFrameColor();//改变Frame颜色

	if (GetSystemDefaultLangID() == 0x0409)
	{
		wchar_t tmp[MY_MAX_PATH];//英文系统上自动切换语言
		wcscpy_s(tmp, TempPath);
		wcscat_s(tmp, L"language\\");//创建language目录
		CreateDirectory(tmp, NULL);
		wcscat_s(tmp, L"English.ini");
		ReleaseRes(tmp, FILE_ENG, L"JPG");
		SwitchLanguage(tmp);
	}
	if (!slient)AutoRegisterHotKey(Main.hWnd, 1, MOD_CONTROL, 'P');//注册热键显示 隐藏
	AutoRegisterHotKey(Main.hWnd, 2, MOD_CONTROL, 'B');//切换桌面
	AutoRegisterHotKey(Main.hWnd, 7, MOD_CONTROL | MOD_ALT, 'K');//键盘控制鼠标
	if (FirstFlag)AutoRegisterHotKey(Main.hWnd, 3, NULL, VK_SCROLL);//第一次启动时自动"一键安装"

	Main.Width = 621; Main.Height = 550;

	if (!slient)ShowWindow(Main.hWnd, nCmdShow);
	SetWindowPos(Main.hWnd, 0, 0, 0, (int)(621 * Main.DPI), (int)(550 * Main.DPI), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
	Main.Redraw();//第一次创建窗口时全部重绘

	typedef struct tagCHANGEFILTERSTRUCT {//使程序接受非管理员程序(explorer)的拖拽请求
		DWORD cbSize;
		DWORD ExtStatus;
	} CHANGEFILTERSTRUCT, * PCHANGEFILTERSTRUCT;
	typedef BOOL(WINAPI* CHANGEWINEOWMESSAGEFILTEREX)(HWND hwnd, UINT message, DWORD action, PCHANGEFILTERSTRUCT pChangeFilterStruct);
	CHANGEWINEOWMESSAGEFILTEREX ChangeWindowMessageFilterEx = NULL;
	HMODULE hModule = GetModuleHandle(L"user32.dll");//使用win7以后才有的新函数
	if (hModule != NULL)ChangeWindowMessageFilterEx = (CHANGEWINEOWMESSAGEFILTEREX)GetProcAddress(hModule, "ChangeWindowMessageFilterEx");
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
		if (DeleteFileHandle != 0)UnloadNTDriver(L"DeleteFile");
		UnloadNTDriver(L"KProcessHacker2");
		MyExitProcess();
		break;
	case WM_CREATE://创建窗口
	{
		Main.tdc = GetDC(Main.hWnd);//创建bitmap
		HDC h = CreateCompatibleDC(Main.tdc);
		Main.Bitmap = CreateCompatibleBitmap(Main.tdc, 1330, 1100);
		SelectObject(h, Main.Bitmap);
		Main.SetHDC(h);
		ReleaseDC(Main.hWnd, Main.tdc);//?
		DragAcceptFiles(hWnd, true);
		break;
	}
	case WM_DROPFILES://接受文件拖拽信息
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (Main.InsideEdit(EDIT_FILEVIEW, point) == TRUE)
		{
			wchar_t tmp[25601] = { 0 }, * a = Main.Edit[EDIT_FILEVIEW].OStr;
			int sum = 0;
			HDROP hDrop = (HDROP)wParam;
			UINT numFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, 0, 0);
			for (unsigned int i = 0; i < numFiles; ++i)
			{
				wchar_t tmp2[MY_MAX_PATH] = { 0 };
				DragQueryFileW(hDrop, i, tmp2, MY_MAX_PATH);
				if (numFiles != 1)wcscat_s(tmp2, L"|");
				sum += (int)wcslen(tmp2);
				if (sum > 25600)break;
				wcscat_s(tmp, tmp2);
			}
			DragFinish(hDrop);
			if (*a != 0)*a = 0;
			Main.SetEditStrOrFont(tmp, NULL, EDIT_FILEVIEW);
			Main.EditRedraw(EDIT_FILEVIEW);
		}
		break;
	}
	case WM_HOTKEY://热键
	{
		switch (wParam)
		{
		case 1: {//隐藏 \ 显示
			if (HideState == -1)break;//永久隐藏时不显示
			ShowWindow(Main.hWnd, 5 * HideState);
			HideState = !HideState;
			Main.Redraw();
			break; }
		case 2: {CreateThread(0, 0, SDThread, 0, 0, 0); break; }//切换桌面
		case 3: {AutoTerminateTD(); break; }//scroll lock结束极域
		case 4: {BSOD(); break; }//ctrl+r蓝屏
		case 5: {Restart(); break; }//ctrl+t重启
		case 6://截图 \ 显示
		{
			ScreenState++;
			if (ScreenState == 1)CaptureImage();
			if (ScreenState == 2)
				CreateWindow(ScreenWindow, L"1", WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, hInst, nullptr);
			break;
		}
		case 7:
		{
			if (Main.Check[CHK_KEYCTRL].Value == false)RegMouseKey(), Main.Check[CHK_KEYCTRL].Value = true; else UnMouseKey(), Main.Check[CHK_KEYCTRL].Value = false;
			Main.Readd(3, CHK_KEYCTRL);
			RECT rc = Main.GetRECTc(CHK_KEYCTRL);
			Main.Redraw(rc);
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
			POINT poi = { 0 };//直接用mouse_event来向左移动似乎有问题
			GetCursorPos(&poi);//这里只能用绝对坐标了
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (DWORD)((poi.x - 10 * Main.DPI) * 65536 / (double)GetSystemMetrics(SM_CXSCREEN)), (DWORD)(poi.y * 65536 / (double)GetSystemMetrics(SM_CYSCREEN)), 0, 0);//左移
			break; }
		case 11: {
			POINT poi = { 0 };
			GetCursorPos(&poi);
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (DWORD)(poi.x * 65536 / (double)GetSystemMetrics(SM_CXSCREEN)), (DWORD)((poi.y - 10 * Main.DPI) * 65536 / (double)GetSystemMetrics(SM_CYSCREEN)), 0, 0);//左移
			break; }
#pragma warning(default:4245)
		case 12: {
			mouse_event(MOUSEEVENTF_MOVE, (DWORD)(10 * Main.DPI), 0, 0, 0);//右移
			break; }
		case 13: {
			mouse_event(MOUSEEVENTF_MOVE, 0, (DWORD)(10 * Main.DPI), 0, 0);
			break; }
		case 14: {//重新打开极域
			if (TDPID == 0)CreateThread(NULL, 0, ReopenThread, NULL, 0, NULL);
		}
		}
		Main.EditHotKey((int)wParam);//分派Edit操作的热键信息
		break;
	}
	case WM_KILLFOCUS://这个事件在鼠标 选中 其他窗口时触发 
	{//不代表鼠标 移出 窗口
		Main.EditUnHotKey();
		for (int i = 1; i <= Main.CurButton; ++i)Main.Button[i].Percent = 0;
		break;
	}
	case WM_PAINT://绘图
	{

		HBRUSH BitmapBrush = NULL; HICON hicon;
		RECT rc; bool f = false;
		GetUpdateRect(hWnd, &rc, false);
		if (rc.top != 0)f = true;
		PAINTSTRUCT ps;
		Main.tdc = BeginPaint(hWnd, &ps);
		if (Main.es[0].top != 0)//根据es来擦除区域
		{
			SelectObject(Main.hdc, WhitePen);
			SelectObject(Main.hdc, WhiteBrush);
			for (int i = 1; i <= Main.es[0].top; ++i)
			{
				Rectangle(Main.hdc, Main.es[i].left, Main.es[i].top, Main.es[i].right, Main.es[i].bottom);
			}
			Main.es[0].top = 0;
		}
		if (Main.rs[0].first != 0)
		{
			for (int i = 1; i <= Main.rs[0].first; ++i)
			{

				Main.RedrawObject(Main.rs[i].first, Main.rs[i].second);//根据rs用redrawobject绘制
			}
			Main.rs[0].first = 0;
			goto finish;
		}
		SelectObject(Main.hdc, WhiteBrush);//白色背景
		SelectObject(Main.hdc, WhitePen);
		SetBkMode(Main.tdc, TRANSPARENT);//使得打印的文字不会覆盖背景
		SetBkMode(Main.hdc, TRANSPARENT);


		Rectangle(Main.hdc, 0, 0, (int)(900 * Main.DPI), (int)(Main.Height * Main.DPI + 1));

		SelectObject(Main.hdc, TitlePen);//(蓝色)绿色顶部
		SelectObject(Main.hdc, TitleBrush);
		Rectangle(Main.hdc, 0, 0, (int)(900 * Main.DPI), (int)(50 * Main.DPI));

		SetTextColor(Main.hdc, COLOR_BLACK);
		SelectObject(Main.hdc, BlackPen);
		SelectObject(Main.hdc, WhiteBrush);

		Main.DrawEVERYTHING();//重绘全部

		hicon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_GUI));
		DrawIconEx(Main.hdc, (int)(20 * Main.DPI), (int)(10 * Main.DPI), hicon, (int)(32 * Main.DPI), (int)(32 * Main.DPI), 0, NULL, DI_NORMAL | DI_COMPAT);
		DeleteObject(hicon);

		if (f == true)  goto finish;//xiaofei头像一般不需要重绘
		//如果是区域绘制就直接跳过这部分
		if (Main.CurWnd == 4)
		{
			LoadPicture(L"C:\\SAtemp\\1.jpg", Main.hdc, 170, 70, Main.DPI);
			if (EasterEggFlag == true)
			{
				BitmapBrush = CreatePatternBrush(hZXFsign);//绘制xiaofei签名
				SelectObject(Main.hdc, BitmapBrush);
				SelectObject(Main.hdc, WhitePen);
				Rectangle(Main.hdc, 105 * 2, 75 * 12, 105 * 3, 75 * 13);
				StretchBlt(Main.hdc, (int)(165 * Main.DPI), (int)(465 * Main.DPI), (int)(105 * Main.DPI), (int)(75 * Main.DPI), Main.hdc, 105 * 2, 75 * 12, 105, 75, SRCCOPY);
			}
			DeleteObject(BitmapBrush);
		}
	finish://贴图
		BitBlt(Main.tdc, rc.left, rc.top, max((long)(Main.Width * Main.DPI), rc.right - rc.left), max((long)(Main.Height * Main.DPI), rc.bottom - rc.top), Main.hdc, rc.left, rc.top, SRCCOPY);
		EndPaint(hWnd, &ps);
	}
	break;

	case 34:
		if (lParam == WM_LBUTTONUP)
		{//点击伪装的极域图zhuang标bu后xua打qu开le极域
			NOTIFYICONDATA tnd;
			tnd.cbSize = sizeof(NOTIFYICONDATA);
			tnd.hWnd = Main.hWnd;
			tnd.uID = IDR_MAINFRAME;
			Shell_NotifyIcon(NIM_DELETE, &tnd);
			CreateThread(0, 0, ReopenThread, 0, 0, 0);
			Main.Check[CHK_TOOLBAR].Value = false;
		}
		break;
	case WM_COMMAND://当控件接收到消息时会触发这个
		int wmId; wmId = LOWORD(wParam);
		switch (wmId)//TDT中只用了List一个控件，所以下面的内容肯定是文件选择框了= =
		{
		case 1:
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				wchar_t tmp[1001], tmp2[1001];
				SendMessage(FileList, LB_GETTEXT, ::SendMessage(FileList, LB_GETCURSEL, 0, 0), (LPARAM)tmp);
				wcscpy_s(tmp2, TempPath);
				wcscat_s(tmp2, L"language\\");
				wcscat_s(tmp2, tmp);
				SwitchLanguage(tmp2);//根据点击的内容合成字符串，然后切换语言
				Main.Redraw();
				break;
			}
			break;
		}
		break;

	case WM_LBUTTONDOWN://点下鼠标左键时
	{
		POINT point; GetCursorPos(&point); ScreenToClient(Main.hWnd, &point);
		Main.EditGetNewInside(point);//试图预先确定一下是否点在某个控件内
		Main.ButtonGetNewInside(point);
		Main.CheckGetNewInside(point);
		Main.AreaGetNewInside(point);//点在控件内 -> 触发控件特效
		if (Main.CoverButton != -1 || Main.CoverCheck != 0 || Main.CoverEdit != 0 || Main.CoverArea != 0)Main.LeftButtonDown();
		else PostMessage(Main.hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);//点在外面 -> 拖动窗口
		break;
	}
	case WM_LBUTTONUP://抬起鼠标左键时
	{
		if (Main.CoverButton != -1)//这时候就要做出相应的动作了
		{
			Main.Press = 0;
			RECT rc = Main.GetRECT(Main.CoverButton);
			Main.Readd(2, Main.CoverButton);
			Main.Redraw(rc);
			if (Main.CoverEdit != 0)
			{
				int tmp0 = Main.CoverEdit;
				Main.EditUnHotKey();
				Main.Edit[Main.CoverEdit].Press = false;
				Main.CoverEdit = 0;
				if (Main.Obredraw)Main.Readd(5, tmp0);//重绘原来的Edit
				rc = Main.GetRECTe(tmp0);
				Main.Redraw(rc);
				if (Main.Edit[tmp0].Pos2 != -1)break;;
			}
		}
		if (Main.CoverEdit == 0)Main.EditUnHotKey();
		Main.Edit[Main.CoverEdit].Press = false;
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(Main.hWnd, &point);

		switch (Main.CoverArea)
		{
		case 1:
		{
			DWORD col;//点击左上角logo切换颜色
			col = DoSelectColour();
			if (col == 0)break;
			DeleteObject(TitleBrush);
			DeleteObject(TitlePen);
			TitleBrush = CreateSolidBrush(col);
			TitlePen = CreatePen(PS_SOLID, 2, col);
			Main.Redraw();//切换页面时当然需要全部重绘啦
			break;
		}
		case 2://显示xiaofei签名
			if (Effect)EasterEgg(true);//如果开低画质就不显示了
			break;//毕竟滚动文字(相对)浪费资源
		case 3:
		{
			OPENFILENAMEW ofn = { 0 };
			wchar_t strFile[MY_MAX_PATH] = { 0 }, str2[MY_MAX_PATH] = { 0 };
			if (*TDPath != 0)
			{
				wcscpy_s(str2, TDPath);
				(_tcsrchr(str2, _T('\\')))[1] = 0;
			}
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrFile = strFile;
			ofn.nMaxFile = MY_MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.lpstrInitialDir = str2;
			if (GetOpenFileName(&ofn) == TRUE)
			{
				wchar_t* t = wcsrchr(strFile, L'\\');
				wcscpy_s(TDPath, strFile);
				wcscpy_s(TDName, t + 1);
				TDsearched = true;
				wchar_t tmpstr[MY_MAX_PATH + 10];
				wcscpy_s(tmpstr, Main.GetStr(L"_TPath"));
				wcscat_s(tmpstr, TDPath);
				Main.SetStr(tmpstr, L"TPath");
				RECT rc{ (int)(170 * Main.DPI), (int)(365 * Main.DPI),(int)(621 * Main.DPI),(int)(390 * Main.DPI) };
				Main.Readd(4, 20);
				Main.es[++Main.es[0].top] = rc;
				Main.Redraw(rc);
			}
			break;
		}
		case 4:
			if (RunWithAdmin(Name))ExitProcess(0);//以管理员权限重启
			break;
		}

		switch (Main.CoverButton)//按钮
		{
		case BUT_MAIN: { Main.SetPage(1); EasterEgg(false); ShowWindow(FileList, SW_HIDE);  break; }//切换页面
		case BUT_TOOLS: { Main.SetPage(2); EasterEgg(false); ShowWindow(FileList, SW_HIDE); SetTimer(Main.hWnd, TIMER_REFRESH_TD, 250, (TIMERPROC)TimerProc); RefreshTDstate();   break; }
		case BUT_OTHERS: { Main.SetPage(3); EasterEgg(false); ShowWindow(FileList, SW_HIDE); break; }
		case BUT_ABOUT:
		{
			if (!InfoChecked)
			{
				UpdateInfo();
				ReleaseRes(L"C:\\SAtemp\\1.JPG", IMG_ZXF, L"JPG");
				InfoChecked = true;
			}
			Main.SetPage(4);
			ShowWindow(FileList, SW_HIDE);
			break; }
		case BUT_SETT:
		{
			if (FileList == NULL)
			{
				FileList = CreateWindowW(L"ListBox", NULL, WS_CHILD | LBS_STANDARD, (int)(180 * Main.DPI), \
					(int)(410 * Main.DPI), (int)(265 * Main.DPI), (int)(120 * Main.DPI), Main.hWnd, (HMENU)1, Main.hInstance, 0);//创建语言文件选择ListBox
				::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);
			}
			Main.SetPage(5);
			EasterEgg(false);
			ShowWindow(FileList, SW_SHOW);
			SearchLanguageFiles(); break;
		}//切换到第五页时搜索语言文件
		case BUT_ONEK:
		{//一键安装
			OneClick = 1 - OneClick;//说是"安装"，其实就注册了一个热键而已
			if (OneClick == 1)
			{
				AutoRegisterHotKey(Main.hWnd, 3, NULL, VK_SCROLL);
				wcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"unQS"));
				Main.InfoBox(L"OneOK");//切换按钮状态
			}
			else
			{
				UnregisterHotKey(Main.hWnd, 3);
				wcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"setQS"));
				Main.InfoBox(L"unQSOK");;
			}
			break;
		}
		case BUT_SETHC:
		{//安装 or 卸载 sethc
			int cur = Main.GetNumbyID(L"Sethc");
			if (SethcInstallState == false)
			{//安装sethc
				if (DeleteSethc() == false)
				{
					Main.InfoBox(L"DSR3Fail"); break;
				}
				else SethcState = false;//删除sethc

				if (AutoSetupSethc())
				{
					wcscpy_s(Main.Button[cur].Name, Main.GetStr(L"unQS"));
					SethcInstallState = true;
					Main.Readd(2, cur);
					Main.Redraw(Main.GetRECT(cur));
				}
			}
			else
			{
				if (UninstallSethc())//这边执行封装好的函数就可以了
				{//主要的工作是改变按钮内容和重绘
					wcscpy_s(Main.Button[cur].Name, Main.GetStr(L"sSethc"));
					SethcInstallState = false;
					Main.Readd(2, cur);
					Main.Redraw(Main.GetRECT(cur)); //L"sSethc"
				}
			}
			break;
		}
		case BUT_HOOK://安装hook,又名"全局键盘钩子"
		{
			int cur = Main.GetNumbyID(L"hook");
			if (HookState == false)
			{
				KillProcess(L"hook.exe");
				if (!RunHOOK())Main.InfoBox(L"OneFail");
				else
				{
					wcscpy_s(Main.Button[cur].Name, Main.GetStr(L"unQS"));
					HookState = true;
					Main.Readd(2, cur);
					Main.Redraw(Main.GetRECT(cur));
				}
			}
			else
			{
				KillProcess(L"hook.exe");
				wcscpy_s(Main.Button[cur].Name, Main.GetStr(L"hook"));
				HookState = false;
				Main.Readd(2, cur);
				Main.Redraw(Main.GetRECT(cur));
			}
			break;
		}
		case BUT_START://在虚拟桌面里运行一个进程
		{
			//int num = Main.GetNumByIDe(L"E_runinVD");
			STARTUPINFO si = { 0 };
			si.cb = sizeof(si);
			si.lpDesktop = szVDesk;
			if (Bit == 34)if (wcsstr(Main.Edit[EDIT_RUNINVD].str, L"explorer") != 0)//如果是32位程序运行在64位上，则运行
				Main.SetEditStrOrFont(L"C:\\Windows\\explorer.exe", nullptr, EDIT_RUNINVD);//Windows目录下64位的explorer
			RunEXE(Main.Edit[EDIT_RUNINVD].str, NULL, &si);//(而不是syswow64下的)，否则explorer会不能全屏
			Main.EditRedraw(EDIT_RUNINVD);
			break;
		}
		case BUT_SWITCH: {CreateThread(0, 0, SDThread, 0, 0, 0); break; }//切换桌面
		case BUT_CATCH:
		{
			AutoCreateCatchWnd();
			timerleft = _wtoi(Main.Edit[EDIT_DELAY].str);
			Main.SetStr(Main.Button[Main.CoverButton].Name, L"back");
			if (timerleft < 1)timerleft = 1;
			SetTimer(Main.hWnd, TIMER_CATCHWINDOW, 1000, (TIMERPROC)TimerProc);
			break;
		}
		case BUT_VIEW:
		{
			AutoCreateCatchWnd();
			wchar_t tmpstr[1001];
			wcscpy_s(tmpstr, Main.Edit[EDIT_PROCNAME].str);
			tdhcur = 0;
			ReturnWindows();
			FindMonitoredhWnd(tmpstr);
			if (tdhcur != 0)displaycur = 1; else displaycur = 0;
			SetTimer(Main.hWnd, TIMER_UPDATECATCH, 33, (TIMERPROC)TimerProc);//注意:这里的timerproc是绑定在Main上的
			AutoRegisterHotKey(CatchWnd, 513, MOD_ALT, VK_LEFT);
			AutoRegisterHotKey(CatchWnd, 514, MOD_ALT, VK_RIGHT);
			AutoRegisterHotKey(CatchWnd, 515, NULL, VK_ESCAPE);//注册热键
			AutoRegisterHotKey(CatchWnd, 516, MOD_ALT, 'H');
			break;
		}
		case BUT_RELS: {ReturnWindows(); break; }
		case BUT_APPCH: { AutoChangeChannel(_wtoi(Main.Edit[EDIT_APPLYCH].str)); break; }//改变频道
		case BUT_CLPSWD: { wchar_t tmp[10] = { 0 }; AutoChangePassword(tmp, 1);  break; }//A U T O
		case BUT_VIPSWD: { AutoViewPass(); break; }
		case BUT_CHPSWD1:case BUT_CHPSWD2:
		{
			size_t len = wcslen(Main.Edit[EDIT_CPSWD].str) + 2;
			wchar_t* str = new wchar_t[len];
			ZeroMemory(str, sizeof(wchar_t) * len);
			wcscpy(str, Main.Edit[EDIT_CPSWD].str);
			str[32] = str[33] = 0;
			if (Main.CoverButton == BUT_CHPSWD1)AutoChangePassword(str, 1); else AutoChangePassword(str, 2); break;
		}
		case BUT_KILLTD: { AutoTerminateTD(); break; }
		case BUT_RETD: { CreateThread(NULL, 0, ReopenThread, NULL, 0, NULL);  break; }
		case BUT_ACTD:
		{
			//HWND t=FindWindow(L"Afx:01B80000:b:00000000:00000006:00010B54", L"TDDesk Render Window");
			AutoCreateCatchWnd();
			ReturnWindows();
			tdhcur = 0;
			KillTimer(hWnd, TIMER_UPDATECATCH);
			for (int i = 513; i < 517; ++i)UnregisterHotKey(CatchWnd, i);
			tdhw = tdhw2 = 0;
			EnumWindows(EnumBroadcastwnd, NULL);
			if (tdhw == 0)EnumWindows(EnumBroadcastwndOld, NULL);
			if (tdhw == 0)Main.InfoBox(L"NoTDwnd");
			else
			{
				if (!Main.Check[CHK_NOHOOK].Value)SetTimer(hWnd, TIMER_ANTYHOOK, 100, (TIMERPROC)TimerProc), Main.Check[CHK_NOHOOK].Value = TRUE;
				if (tdhw2 == (HWND)-2)
				{
					RECT rc;
					GetClientRect(CatchWnd, &rc);
					SetWindowPos(tdhw, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOREDRAW);
				}
				else
					SetTimer(Main.hWnd, TIMER_CATCHEDTD, 100, (TIMERPROC)TimerProc);
				TOP = FALSE;
				KillTimer(hWnd, TIMER_TOP);
				Main.Check[CHK_TOP].Value = false;
				if (CatchWnd != NULL)SetWindowPos(CatchWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);//取消置顶
				SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
			}
			break;
		}
		case BUT_SHUTD: { CreateThread(NULL, 0, ShutdownDeleterThread, 0, 0, NULL); break; }
		case BUT_SDESK:
		{//用paexec把自己运行在安全桌面上
			if (!Admin)error();
			wchar_t  tmp2[301];
			wcscpy_s(tmp2, TempPath);//历史原因，这里仍然叫psexec = =
			wcscat_s(tmp2, L"psexec.exe");
			if (GetFileAttributes(tmp2) == -1)ReleaseRes(tmp2, FILE_PSEXEC, L"JPG");
			wcscat_s(tmp2, L" -x -i -s -d ");
			wcscat_s(tmp2, Name);
			wcscat_s(tmp2, L" -top\"");
			s(tmp2);
			if (!RunEXE(tmp2, CREATE_NO_WINDOW, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		case BUT_AUTO: { KillFullScreen(); break; }//自动关闭置顶进程
		case BUT_VFILE: { openfile(); break; }//选择文件
		case BUT_VFOLDER: { BrowseFolder(); break; }//选择文件夹
		case BUT_DELETE: {CreateThread(NULL, 0, DeleteThread, 0, 0, NULL); break; }//开始删除文件 or 文件夹
		case BUT_BSOD: { BSOD(); break; }//蓝屏
		case BUT_RESTART: { Restart(); break; }//快速重启
		case BUT_ARP:
		{//和Gamexx按钮一样的待遇
			if (Main.Button[BUT_ARP].DownTot != 0)break;
			wchar_t tmp[301];
			wcscpy_s(tmp, TempPath);
			wcscat_s(tmp, L"arp.exe");
			if (GetFileAttributes(tmp) != -1)//arp.exe文件存在
			{//直接运行
				if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			}//下载
			else CreateDownload(7);
			break;
		}
		case BUT_SYSCMD:
		{//system权限cmd
			if (!Admin)error();
			wchar_t tmp2[MY_MAX_PATH];
			wcscpy_s(tmp2, TempPath);//和“安全桌面上运行”基本一样的命令，只是少了一个"-x"
			wcscat_s(tmp2, L"psexec.exe");
			if (GetFileAttributes(tmp2) == -1)ReleaseRes(tmp2, FILE_PSEXEC, L"JPG");
			wcscat_s(tmp2, L" -i -s -d cmd.exe");
			if (!RunEXE(tmp2, CREATE_NO_WINDOW, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		case BUT_360://驱动结束360
		{
			Main.InfoBox(L"360Start");
			if (!EnableKPH())Main.InfoBox(L"360Fail");
			else
			{
				Main.Check[CHK_KPH].Value = true;
				Main.Check[CHK_FMACH].Value = false;//暂时关闭进程名完全匹配
				KillProcess(L"360");
				KillProcess(L"zhu");
				KillProcess(L"sof");
			}
			break;
		}
		case BUT_MORE://关于
		{//可能多数人都读不懂，吧
			wchar_t tmp[321] = L"Notepad ", tmp2[301];
			wcscpy_s(tmp2, TempPath);
			wcscat_s(tmp2, L"关于&帮助.txt");
			ReleaseRes(tmp2, FILE_HELP, L"JPG");
			wcscat_s(tmp, tmp2);
			if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		case BUT_SYSINF://系统信息
		{
			wchar_t tmp[] = L"C:\\Windows\\System32\\Msinfo32.exe";
			if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		case BUT_HIDEST: { HideState = -1; ShowWindow(Main.hWnd, SW_HIDE); break; }//永久隐藏
		case BUT_CLEAR: { ClearUp(); break; }//清理文件并退出
		case BUT_GAMES:
		{
			if (GameMode == 0)
			{//准备展开游戏界面
				wchar_t tmp[301];
				wcscpy_s(tmp, TempPath);
				wcscat_s(tmp, L"games");//创建games目录
				CreateDirectory(tmp, NULL);

				for (int i = 0; i < numGames; ++i)
				{//搜索已存在的目录
					wcscpy_s(tmp, TempPath);
					wcscat_s(tmp, GameName[i]);
					if (GetFileAttributes(tmp) != -1)GameExist[i] = TRUE;
				}

				wcscpy_s(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Gamee"));//把按钮名变成"停止"
				GameMode = 1;
				if (!GameLock)GameLock = true, CreateThread(NULL, 0, GameThread, 0, 0, NULL);//创建展开动画线程
				else GameMode = 0, wcscpy_s(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Games"));
			}
			else
			{//关闭游戏界面
				GameMode = 0;
				wcscpy_s(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Games"));
				if (!GameLock)GameLock = true, CreateThread(NULL, 0, GameThread, 0, 0, NULL);
				else GameMode = 1, wcscpy_s(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Gamee"));
			}
			break;
		}
		case BUT_GAME1:case BUT_GAME2:case BUT_GAME3:case BUT_GAME4:case BUT_GAME5:case BUT_GAME6: {//hangman and so on
			if (GameExist[Main.CoverButton - BUT_GAME1])
			{
				wchar_t tmppath[MY_MAX_PATH] = L"C:\\SAtemp\\"; wcscat_s(tmppath, GameName[Main.CoverButton - BUT_GAME1]);
				//
				RunEXE(tmppath, NULL, nullptr);
			}
			else { CreateDownload(Main.CoverButton - BUT_GAME1 + 1); }break;
		}

		case BUT_CLOSE: { MyExitProcess(); }//"关闭"按钮
		default:
			break;
		}


		if (!Main.Check[Main.CoverCheck].Value)
		{//未选中->选中
			switch (Main.CoverCheck)
			{
			case CHK_FTOLD:case CHK_FTNEW: {//伪装工具条

				if (FF == TRUE)//注册类
				{
					MyRegisterClass(hInst, FakeProc, FakeWindow, true);
					FF = FALSE;
					FakeWnd = CreateWindowW(FakeWindow, Main.GetStr(L"Title"), WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInst, nullptr);//创建窗口
					SetWindowLong(FakeWnd, GWL_EXSTYLE, GetWindowLong(FakeWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
				}//没有任务栏图标的"工具栏窗口"
				if (Main.CoverCheck == CHK_FTOLD)
				{
					ReleaseRes(L"C:\\SAtemp\\Fakeold.jpg", IMG_FAKEOLD, L"JPG");
					FakeNew = false;//释放图片
					FakenewUp = false;
					Main.Check[CHK_FTNEW].Value = false;
					SetWindowPos(FakeWnd, HWND_TOPMOST, 200, 0, 82, 48, NULL);
					KillTimer(FakeWnd, TIMER_TOOLBAR);
				}
				else
				{
					HRGN rgn1 = CreateRoundRectRgn(0, 0, 342, 63, 11, 11), rgn2 = CreateRectRgn(0, 0, 342, 10), rgn = CreateRectRgn(0, 0, 0, 0);
					CombineRgn(rgn, rgn1, rgn2, RGN_OR);
					SetWindowRgn(FakeWnd, rgn, false);
					ReleaseRes(L"C:\\SAtemp\\Fakenew.jpg", IMG_FAKENEW, L"JPG");
					FakeNew = true;
					FakenewUp = false;//伪装(新)要开启伸展/缩回线程
					Main.Check[CHK_FTOLD].Value = false;
					SetWindowPos(FakeWnd, HWND_TOPMOST, 200, 0, 340, 63, NULL);
					FakeTimer = GetTickCount();
					SetTimer(FakeWnd, TIMER_TOOLBAR, 100, (TIMERPROC)TimerProc);
				}
				Main.Readd(3, 1); Main.Readd(3, 2); Main.Redraw();
				ShowWindow(FakeWnd, SW_SHOW);
				InvalidateRect(FakeWnd, nullptr, TRUE);
				UpdateWindow(FakeWnd);
				break; }
			case CHK_TOOLBAR: {
				NOTIFYICONDATA tnd;//伪装图标
				tnd.cbSize = sizeof(NOTIFYICONDATA);
				tnd.hWnd = Main.hWnd;
				tnd.uID = IDR_MAINFRAME;
				tnd.uFlags = 2 | 1 | 4;
				tnd.uCallbackMessage = 34;
				tnd.hIcon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_TD));
				wcscpy_s(tnd.szTip, Main.GetStr(L"tnd"));
				if (!Shell_NotifyIcon(NIM_ADD, &tnd))Main.InfoBox(L"StartFail"), Main.Check[CHK_TOOLBAR].Value = true;
				break; }//CHK_TDINVD 4
			case CHK_RETD: {AutoRegisterHotKey(Main.hWnd, 14, MOD_CONTROL, 'Y'); break; }
			case CHK_T_A_:
			{//加载驱动删除文件
				if (!EnableTADeleter())Main.Check[CHK_T_A_].Value = !Main.Check[CHK_T_A_].Value;
				break;
			}
			case CHK_TOP: {
				TOP = TRUE;
				if (Effect)
					CreateThread(NULL, 0, TopThread, NULL, 0, NULL);
				else//"低画质"时改用计时器置顶
					SetTimer(hWnd, TIMER_TOP, 10, (TIMERPROC)TimerProc);
				break; }//置顶

			case CHK_BSOD: {AutoRegisterHotKey(Main.hWnd, 4, MOD_CONTROL, 'R'); break; }//蓝屏
			case CHK_SHUTD: {AutoRegisterHotKey(Main.hWnd, 5, MOD_CONTROL, 'T'); break; }//重启
			case CHK_SCSHOT: {//截图伪装
				if (FS == TRUE)MyRegisterClass(hInst, ScreenProc, ScreenWindow, false), FS = FALSE;
				AutoRegisterHotKey(Main.hWnd, 6, MOD_CONTROL | MOD_ALT, 'P');
				break; }
			case CHK_REKILL: {KillProcess(Main.Edit[EDIT_TDNAME].str); SetTimer(hWnd, TIMER_KILLPROCESS, 1500, (TIMERPROC)TimerProc); break; }//连续结束进程	//CHK_FMACH 12
			case CHK_NOHOOK: {SetTimer(hWnd, TIMER_ANTYHOOK, 100, (TIMERPROC)TimerProc); break; }//禁止键盘（鼠标）钩子
			case CHK_KEYCTRL: {RegMouseKey(); break; }//键盘控制鼠标
			case CHK_EFFECT: {Effect = Main.ButtonEffect = false; break; }//低画质
			case CHK_DPI: {//缩小/放大
				Main.SetDPI(0.75);

				SetWindowPos(FileList, 0, (int)(180 * Main.DPI), (int)(420 * Main.DPI), (int)(265 * Main.DPI), (int)(110 * Main.DPI), NULL);
				SendMessage(FileList, WM_SETFONT, WPARAM(Main.DefFont), 0);
				break; }
			case CHK_KPH:
			{//使用KProcessHacker结束进程
				if (!EnableKPH())Main.Check[CHK_KPH].Value = !Main.Check[CHK_KPH].Value;
				break;
			}
			}
		}
		else
		{//选中 -> 未选中
			switch (Main.CoverCheck)
			{//关闭"伪装工具条"
			case CHK_FTOLD: case CHK_FTNEW: {ShowWindow(FakeWnd, SW_HIDE); KillTimer(FakeWnd, TIMER_TOOLBAR); break; }
			case CHK_TOOLBAR: {
				NOTIFYICONDATA tnd;
				tnd.cbSize = sizeof(NOTIFYICONDATA);
				tnd.hWnd = Main.hWnd;
				tnd.uID = IDR_MAINFRAME;
				Shell_NotifyIcon(NIM_DELETE, &tnd);
				break; }//CHK_TDINVD 4
			case CHK_RETD: {UnregisterHotKey(Main.hWnd, 14); }
			case CHK_TOP: {
				TOP = FALSE;
				KillTimer(hWnd, TIMER_TOP);
				if (CatchWnd != NULL)SetWindowPos(CatchWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);//取消置顶
				SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
				break;
			}
			case CHK_BSOD: {UnregisterHotKey(Main.hWnd, 4); break; }
			case CHK_SHUTD: {UnregisterHotKey(Main.hWnd, 5); break; }
			case CHK_SCSHOT: {UnregisterHotKey(Main.hWnd, 6); break; }
			case CHK_REKILL: {KillTimer(hWnd, TIMER_KILLPROCESS); break; }//CHK_FMACH 12
			case CHK_NOHOOK: {KillTimer(hWnd, TIMER_ANTYHOOK); break; }
			case CHK_KEYCTRL: {UnMouseKey(); break; }
			case CHK_EFFECT: {Effect = Main.ButtonEffect = true; break; }
			case CHK_DPI: {//基本上就是把之前的过程反过来
				Main.SetDPI(1.5);
				SetWindowPos(FileList, 0, (int)(180 * Main.DPI), (int)(420 * Main.DPI), (int)(265 * Main.DPI), (int)(110 * Main.DPI), NULL);
				SendMessage(FileList, WM_SETFONT, WPARAM(Main.DefFont), 0);
				break;
			}
			}
		}
		if (Main.CoverCheck != 0)
		{
			Main.Check[Main.CoverCheck].Value = !Main.Check[Main.CoverCheck].Value;
			Main.Readd(3, Main.CoverCheck);
			Main.Redraw(Main.GetRECTc(Main.CoverCheck));
		}

		Main.Timer = (DWORD)time(0);
		Main.DestroyExp();
		break;
	}
	case WM_MOUSEMOVE: {Main.MouseMove(); break; }
	case WM_IME_STARTCOMPOSITION:
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		LOGFONT lf;
		COMPOSITIONFORM cf;
		HIMC himc = ImmGetContext(hWnd);
		if (himc)
		{
			//为Edit设置输入法显示位置。
			GetCaretPos(&point);
			cf.dwStyle = CFS_POINT;
			cf.ptCurrentPos.y = point.y + 10;
			cf.ptCurrentPos.x = point.x + 10;
			ImmSetCompositionWindow(himc, &cf);

			//设置输入法字体样式(很多时候没用)
			GetObject(Main.DefFont, sizeof(LOGFONT), &lf);
			ImmSetCompositionFont(himc, &lf);

			ImmReleaseContext(hWnd, himc);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_MOUSELEAVE://TrackMouseEvent带来的消息
		PostMessage(Main.hWnd, WM_MOUSEMOVE, NULL, 0);
		break;
	case WM_CHAR://给Edit转发消息
		Main.EditCHAR((wchar_t)wParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ScreenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{//截图伪装窗口
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		sdc = GetDC(hWnd);//创建缓存DC
		pdc = CreateCompatibleDC(sdc);
		RegisterHotKey(hWnd, 1, NULL, VK_ESCAPE);//注册Esc为关闭热键
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
			HBITMAP bitmap;//从SAtemp中取出贴图
			bitmap = (HBITMAP)LoadImage(hInst, L"C:\\SAtemp\\ScreenShot.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			SelectObject(pdc, bitmap);

			SetStretchBltMode(sdc, HALFTONE);//先将贴图打印在缓存DC上，然后再贴到真实DC上以提高稳定性(防止中途切换分辨率)
			StretchBlt(sdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), pdc, 0, 0, curDevMode.dmPelsWidth, curDevMode.dmPelsHeight, SRCCOPY);
			DeleteObject(bitmap);
		}
		else//所谓的"截图伪装窗口"在伪装蓝屏时也用来黑屏过渡
		{
			SetStretchBltMode(sdc, HALFTONE);
			StretchBlt(sdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), pdc, 0, 0, curDevMode.dmPelsWidth, curDevMode.dmPelsHeight, BLACKNESS);
		}
		EndPaint(hWnd, &ps);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
int code[26] = { 0x1fc9e7f,0x1053641,0x175f65d,0x174e05d,0x175075d,0x105a341,0x1fd557f,0x19500,0x1a65d76,0x17a6dc1,0x18ec493,0x1681960,
0x1471bcb,0x2255ed,0x17c7475,0xea388a,0x18fd1fc,0x1f51d,0x1fd8b53,0x104d51d,0x1745df2,0x1751d14,0x174ce1d,0x1056dc8,0x1fd9ba3
};//信不信这是一个二维码= =
bool FBoldFirst = true;
wchar_t words[9][300] =
{ L"A problem has been detected and windows has been shut down to prevent damage to your computer. ",
L"IRQL_NOT_LESS_OR_EQUAL ",//win7及更旧版本的系统的蓝屏文字
L"An executive worker thread is being terminated without having gone through the worker thread rundown code.work items queued to the Ex worker queue must not terminate their threads.A stack trace should indicate the culprit. ",
L"If this is the first time you've seen this Stop error screen, restart your computer. If this screen appears again, follow these steps: "
,L"Check to make sure any new hardware of software is properly installed. If this is new installation, ask your hardware or software manufacturer for any windows updates you might need. "
,L"If problems continue,disable or remove any newly installed hardware or software. Disable BIOS memory options such as caching or shadowing. If you need to use Safe Mode to remove or disable components, restart your computer, press F8 to select Advanced Startup Options, and then select Safe Mode. ",
L"Technical information: ",L"*** STOP: 0x0000000A (0x00000000,0xD0000002, 0x00000001,0x8082C582)",L"*** wdf01000.sys - Address 97C141AC base at 97C0E000, DateStamp 4fd91f51 " };
LRESULT CALLBACK BSODProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{//伪装蓝屏窗体的响应函数
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CLOSE:
		MyExitProcess();
		break;
	case WM_CREATE:
		ldc = GetDC(hWnd);//创建缓存DC
		bdc = CreateCompatibleDC(ldc);
		lBmp = CreateCompatibleBitmap(ldc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		SelectObject(bdc, lBmp);
		ReleaseDC(hWnd, ldc);
		break;
	case WM_PAINT:
	{
		LockCursor();
		ldc = BeginPaint(hWnd, &ps);
		if (Main.Check[17].Value == false)//false -> 新版蓝屏
		{
			SetTextColor(bdc, COLOR_WHITE);
			SelectObject(bdc, BSODPen);
			SelectObject(bdc, BSODBrush);
			SetBkMode(bdc, 1);//新版蓝屏界面一共用了三种大小的字体
			HFONT A = CreateFontW(40, 15, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑")),
				B = CreateFontW(140, 70, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Century Gothic")),
				C = CreateFontW(26, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑"));
			SelectObject(bdc, B);
			Rectangle(bdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			TextOut(bdc, 180, 120, L":(", 2);//打印:(
			SelectObject(bdc, A);//Century Gothic字体在win7及以前的系统中默认没有
			TextOut(bdc, 180, 290, Main.GetStr(L"BSOD1"), (int)wcslen(Main.GetStr(L"BSOD1")));
			wchar_t tmp[21];
			if (BSODstate > 20)BSODstate = 20;
			_itow_s(int(BSODstate * 2.5), tmp, 10);
			wcscat_s(tmp, Main.GetStr(L"BSOD7"));//打印完成比例
			TextOut(bdc, 180, 360, tmp, (int)wcslen(tmp));
			SelectObject(bdc, WhiteBrush);
			Rectangle(bdc, 180, 440, 180 + 141, 440 + 141);
			SelectObject(bdc, BSODBrush);
			for (int i = 0; i < 25; ++i)//打印二维码
				for (int j = 0; j < 25; ++j)if ((code[i] >> (24 - j)) % 2 == 1)Rectangle(bdc, 188 + 5 * j, 448 + 5 * i, 188 + 5 + 5 * j, 448 + 5 + 5 * i);
			SelectObject(bdc, C);

			TextOut(bdc, 345, 440, Main.GetStr(L"BSOD2"), (int)wcslen(Main.GetStr(L"BSOD2")));
			TextOut(bdc, 345, 470, Main.GetStr(L"BSOD3"), (int)wcslen(Main.GetStr(L"BSOD3")));//打印各项文字
			TextOut(bdc, 345, 520, Main.GetStr(L"BSOD4"), (int)wcslen(Main.GetStr(L"BSOD4")));
			TextOut(bdc, 345, 550, Main.GetStr(L"BSOD5"), (int)wcslen(Main.GetStr(L"BSOD5")));
			TextOut(bdc, 345, 580, Main.GetStr(L"BSOD6"), (int)wcslen(Main.GetStr(L"BSOD6")));
			BitBlt(ldc, 0, 0, GetDeviceCaps(GetDC(NULL), HORZRES), GetDeviceCaps(GetDC(NULL), VERTRES), bdc, 0, 0, SRCCOPY);
			DeleteObject(A); DeleteObject(B); DeleteObject(C);//贴图
		}
		else//旧版蓝屏
		{
			int ybegin = 20, xbegin = 2, xmax = 640, ymax = 480, left, right, s = 8;

			SetTextColor(bdc, COLOR_WHITE);
			SelectObject(bdc, CreateSolidBrush(COLOR_OBSOD_BLUE));
			SelectObject(bdc, CreatePen(PS_SOLID, 1, COLOR_OBSOD_BLUE));
			SetBkMode(bdc, 1);
			HFONT A = CreateFontW(14, 8, 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Lucida Console"));
			SelectObject(bdc, A);
			Rectangle(bdc, 0, 0, 640, 480);
			for (int i = 0; i < 10; ++i)
			{
				if (ymax - 50 <= ybegin)break;
				left = right = 0;
				xbegin = 2;

				while (1)
				{
					while (words[i][right] != ' ' && words[i][right] != '\0')right++;
					if (words[i][right] == '\0')break;//根据屏幕宽度自动换行打印
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
						ybegin += (int)(s * 1.8);
						right = left;
					}
				}
				ybegin += (int)(s * 3.6);
			}
			if (BSODstate >= 4)TextOut(bdc, 2, ybegin, L"Collecting data for crash dump...", 33), ybegin += (int)(s * 1.8);
			if (BSODstate >= 10)TextOut(bdc, 2, ybegin, L"Initializing disk for crash dump...", 35);
			//BitBlt(ldc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), bdc, 0, 0, SRCCOPY);
			StretchBlt(ldc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), bdc, 0, 0, 640, 480, SRCCOPY);
		}
		EndPaint(hWnd, &ps);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
HDC chdc, ctdc; HBITMAP cbmp;
LRESULT CALLBACK CatchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{//捕捉窗口的窗体响应函数
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
	{//创建缓存DC
		ctdc = GetDC(CatchWnd);
		chdc = CreateCompatibleDC(ctdc);
		cbmp = CreateCompatibleBitmap(ctdc, GetDeviceCaps(GetDC(NULL), HORZRES), GetDeviceCaps(GetDC(NULL), VERTRES));
		SelectObject(chdc, cbmp);
		ReleaseDC(hWnd, ctdc);
	}
	case WM_CLOSE://关闭
		KillTimer(Main.hWnd, TIMER_UPDATECATCH);
		ReturnWindows();//自动归还窗口
		DestroyWindow(CatchWnd);
		CatchWnd = 0;
		break;
	case WM_PAINT:
	{//窗口重绘
		ctdc = BeginPaint(hWnd, &ps);
		if (tdhcur != 0)//监视窗口贴图
		{

			HDC hdctd = GetDC(tdh[displaycur]);
			RECT rc, rc2;
			int left, width, top, height;

			GetClientRect(tdh[displaycur], &rc);//获取被监视&自己窗口大小
			GetClientRect(hWnd, &rc2);

			SelectObject(chdc, WhiteBrush);//白色背景
			SelectObject(chdc, WhitePen);
			Rectangle(chdc, 0, 0, rc2.right - rc2.left, rc2.bottom - rc2.top);

			if ((rc2.right - rc2.left) == 0 || (rc2.bottom - rc2.top) == 0 || (rc.right - rc.left) == 0 || (rc.bottom - rc.top) == 0)break;
			const double wh1 = (double)(rc.right - rc.left) / (double)(rc.bottom - rc.top),
				wh2 = (double)(rc2.right - rc2.left) / (double)(rc2.bottom - rc2.top);
			if (wh1 > wh2)
			{//被监视窗口长宽比 > 自己长宽比
				left = 0;
				width = rc2.right - rc2.left;
				top = (int)((rc2.bottom - rc2.top) / 2 - (rc2.right - rc2.left) / wh1 / 2);
				height = (int)((rc2.right - rc2.left) / wh1);
			}
			else
			{//被监视窗口长宽比 <= 自己长宽比
				top = 0;
				height = rc2.bottom - rc2.top;
				left = (int)((rc2.right - rc2.left) / 2 - (rc2.bottom - rc2.top) * wh1 / 2.0);
				width = (int)((rc2.bottom - rc2.top) * wh1);
			}

			SetStretchBltMode(ctdc, HALFTONE);
			StretchBlt(ctdc, left, top, width, height, hdctd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SRCCOPY);
			ReleaseDC(tdh[displaycur], hdctd);//贴图
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SETFOCUS:
	{
		if (tdhcur != 0)
		{//获得焦点时注册热键(不一定有用)
			AutoRegisterHotKey(hWnd, 513, MOD_ALT, VK_LEFT);
			AutoRegisterHotKey(hWnd, 514, MOD_ALT, VK_RIGHT);
			AutoRegisterHotKey(hWnd, 515, NULL, VK_ESCAPE);
			AutoRegisterHotKey(hWnd, 516, MOD_ALT, 'H');
		}
		break;
	}
	case WM_KILLFOCUS: {
		for (int i = 513; i < 517; ++i)UnregisterHotKey(hWnd, i);
		break; }
	case WM_HOTKEY: {
		if (wParam == 513) { displaycur--; }
		if (wParam == 514) { displaycur++; }
		if (wParam == 515) { tdhcur = 0, KillTimer(hWnd, TIMER_UPDATECATCH); for (int i = 513; i < 517; ++i)UnregisterHotKey(hWnd, i); }
		if (wParam == 516)
			if (IsWindowVisible(tdh[displaycur]))ShowWindow(tdh[displaycur], SW_HIDE); else ShowWindow(tdh[displaycur], SW_SHOW);
		if (displaycur == 0)displaycur = tdhcur;
		if (displaycur > tdhcur)displaycur = 1;
		if (wParam < 516)InvalidateRect(CatchWnd, NULL, FALSE), UpdateWindow(CatchWnd);
		break; }
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK FakeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{//伪装工具栏窗口的响应函数
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
	{
		gdc = GetDC(hWnd);
		fdc = CreateCompatibleDC(gdc);
		FakeBitmap = CreateCompatibleBitmap(gdc, 400, 70);
		SelectObject(fdc, FakeBitmap);
		ReleaseDC(FakeWnd, gdc);
	}
	case WM_PAINT:
	{
		gdc = BeginPaint(hWnd, &ps);
		SelectObject(fdc, WhiteBrush);//白色背景
		SelectObject(fdc, WhitePen);
		Rectangle(fdc, 0, 0, 400, 70);
		if (FakeNew)
			LoadPicture(L"C:\\SAtemp\\Fakenew.JPG", fdc, 0, 0, 1);
		else
			LoadPicture(L"C:\\SAtemp\\Fakeold.JPG", fdc, 0, 0, 1);
		BitBlt(gdc, 0, 0, 400, 70, fdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
	{PostMessage(FakeWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
	break; }
	case WM_MOUSEMOVE:
	{
		FakeTimer = GetTickCount();
		if (FakenewUp == true)
		{
			FakenewUp = false;
			int typ = 1;
			CreateThread(NULL, 0, FakeNewThread, &typ, 0, NULL);
			Sleep(1);
		}
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}