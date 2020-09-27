//这是TopDomainTools工程源代码的主文件
//by zouxiaofei1 2015 - 2020

//头文件
#include "stdafx.h"
#include "GUI.h"
#include "TestFunctions.h"
#include "Hotkey.h"
#include "myProcessHacker.h"
#include "Actions.h"
#include "myPaExec.h"
#pragma comment(lib, "urlmon.lib")//下载文件用的Lib
#pragma comment(lib,"Imm32.lib")//自定义输入法位置用的Lib
#pragma comment(lib, "ws2_32.lib")//Winsock API 库

#pragma warning(disable:6320)
//部分(重要)函数的前向声明
BOOL				InitInstance();//初始化
LRESULT	CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//主窗口
LRESULT CALLBACK	CatchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//"捕捉窗口"的窗口
LRESULT CALLBACK	ScreenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//截图伪装窗口
LRESULT CALLBACK	BSODProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//蓝屏伪装窗口
LRESULT CALLBACK	FakeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//伪装工具条窗口
void	CALLBACK	TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);//计时器
BOOL SearchTool(LPCWSTR lpPath, int type);

//自己定义的全局变量 有点乱

//"全局"的全局变量
HINSTANCE hInst;//当前实例，CreateWindow&LoadIcon时需要
const wchar_t szWindowClass[] = L"MainWnd",		/*主窗口类名*/		CatchWindow[] = L"CatchWnd";/*第二窗口类名*/
const wchar_t ScreenWindow[] = L"ScreenWnd",	/*截图伪装窗口类名*/BSODWindow[] = L"BSODWnd";/*伪装蓝屏窗口类名*/
const wchar_t FakeWindow[] = L"FakeToolBar";	/*伪装工具条窗口类名*/
BOOL FC = TRUE, FS = TRUE, FB = TRUE, FF = TRUE;//是否第一次启动窗口并注册类 C=catch S=screen B=BSOD F=FakeToolBar
BOOL Admin;//是否拥有管理员权限
int Bit;//系统位数 32 34 64
wchar_t Path[MAX_PATH], Name[MAX_PATH], TDTempPath[] = L"C:\\SAtemp\\";
//程序路径 and 路径+程序名 and 缓存路径 and sethc备份路径(注意Path最后有"\")

//和绘图有关的全局变量
HBRUSH DBlueBrush, LLGreyBrush, LBlueBrush, WhiteBrush, NormalBlueBrush, LGreyBrush, YellowBrush, DGreyBrush, BSODBrush, BlackBrush;//各色笔刷
HPEN YellowPen, BlackPen, WhitePen, DGreyPen, DDGreyPen, LGreyPen, CheckGreenPen, NormalGreyPen, DarkGreyPen, NormalBluePen, DBluePen, LBluePen, BSODPen;//各色笔
HBITMAP BSODBitmap, CatchBitmap, FakeBitmap;//各窗口hbmp
HDC chdc, ctdc; //各窗口缓冲dc & 真实dc
HDC shdc, stdc;//截图伪装窗口dc
HDC bhdc, btdc;//蓝屏伪装窗口dc
HDC thdc, ttdc;//不使用CC的窗口只能用这些奇怪的名字(待遇不公= =)
HWND FakeWnd;//工具条伪装窗口hWnd
COLORREF crCustColors[16];//颜色选择中的自定义颜色
BOOL LowResource = FALSE;//在屏幕分辨率较低的电脑上自动降低画质
BOOL SizingLock;

//第一页的全局变量
BOOL OneClick = FALSE;//一键安装状态
wchar_t SethcPath[MAX_PATH];//Sethc路径
wchar_t TDPath[MAX_PATH], TDName[MAX_PATH];//极域路径 and 默认极域进程名
BOOL TDsearched = FALSE;//是否已经寻找过极域?
BOOL SethcState = TRUE;//System32目录下sethc是否存在
BOOL SethcInstallState = FALSE;//Sethc方案状态
BOOL HookState = FALSE, FirstHook = TRUE;//全局键盘钩子方案状态
BOOL FirstFlag;//是否在这台电脑上第一次运行?(根据sethc是否备份过检测)
wchar_t ExplorerPath[] = L"C:\\Windows\\explorer";//符合系统位数的explorer路径
wchar_t szVDesk[] = L"TDTdesk", fBSODdesk[] = L"TDTBSOD";//虚拟桌面 & 蓝屏伪装窗口桌面的名称
BOOL NewDesktop = FALSE;//是否处于新桌面中?

//第二、三页的全局变量
BOOL GameMode;//游戏模式是否打开?
constexpr int numGames = 6;// (游戏)数
BOOL GameExist[numGames + 1];//标记的文件是否存在?
constexpr wchar_t GameName[numGames + 1][12]{ L"xiaofei.exe", L"fly.exe",L"2048.exe",\
L"block.exe", L"1.exe" , L"chess.exe",L"14Kwds.ini" };//(游戏)名
constexpr wchar_t GameURLsuffix[numGames + 1][3]{ L"95", L"88",L"61",\
L"72", L"50" , L"80",L"25" };//链接后缀
BOOL GameButtonLock = FALSE;//Game按钮锁定
constexpr wchar_t GameURLprefix[] = L"http://www.zlian.ga/u/15993785";//游戏存储库目录

DWORD TDPID;//记录极域程序PID
BOOL FakeToolbarNew;//显示的是否是新版本的伪装工具条
DWORD FakeTimer;//记录伪装工具条缩回时间的变量
BOOL FakenewUp = FALSE;//伪装工具条是否缩回 - 刚开始时为FALSE
BOOL FakeThreadLock = FALSE;//伪装工具条窗口伸缩线程锁
HANDLE DeleteFileHandle;//文件删除驱动句柄

//第四页的全局变量
HBITMAP hZXFsign;//xiaofei签名图片句柄
int EasterEggState;//CopyLeft文字循环状态
BOOL EasterEggFlag = FALSE;//是否显示文字循环
constexpr char EasterEggStr[11][8]{ "Answer","Left" ,"Left",\
"Right","Down","Up","In","On","Back","Front","Teacher" };//滚动显示的字符
BOOL InfoChecked = FALSE;//是否已经检查过系统信息

//第五页的全局变量
int ScreenState;//截图伪装状态 1 = 截图 2 = 显示
HHOOK KeyboardHook, MouseHook;//键盘、鼠标钩子
int HideState;//窗口是否隐藏
HWND FileList;//语言选择hWnd
BOOL TOP;//是否置顶

//捕捉窗口 & 伪装蓝屏窗体的全局变量
HWND CatchWnd;//捕捉窗口的窗口句柄
HWND TDhWndChild, TDhWndParent, TDhWndGrandParent;//极域广播窗口子窗口hWnd & 极域广播窗口hWnd
int CatchWndTimerLeft = -2;//捕捉窗口倒计时
Map<int, BOOL>Eatpid;//记录捕捉窗口hWnd的map
HWND EatList[101];//被捕捉的窗口的hWnd将被压入这个"栈"
Map<int, BOOL>expid, tdpid;//explorer PID + 被监视窗口 PID
HWND MonitorList[101]; //被监视窗口hWnd
int MonitorTot, MonitorCur;//被监视窗口数量 + 正在被监视的窗口编号
int TopCount;//CatchWnd窗口置顶延迟变量
int sdl = 5;//systemdefaultlanguage
constexpr int QRcode[]{ 0x1fc9e7f,0x1053641,0x175f65d,0x174e05d,0x175075d,0x105a341,0x1fd557f,0x19500,0x1a65d76,0x17a6dc1,0x18ec493,0x1681960,
0x1471bcb,0x2255ed,0x17c7475,0xea388a,0x18fd1fc,0x1f51d,0x1fd8b53,0x104d51d,0x1745df2,0x1751d14,0x174ce1d,0x1056dc8,0x1fd9ba3
};//信不信这是一个二维码= =
const char word1[] = "A problem has been detected and windows has been shut down to prevent damage to your computer. ", word2[] = "IRQL_NOT_LESS_OR_EQUAL ",
word3[] = "An executive worker thread is being terminated without having gone through the worker thread rundown code.work items queued to the Ex worker queue must not terminate their threads.A stack trace should indicate the culprit. ",
word4[] = "If this is the first time you've seen this Stop error screen, restart your computer. If this screen appears again, follow these steps: "
, word5[] = "Check to make sure any new hardware of software is properly installed. If this is new installation, ask your hardware or software manufacturer for any windows updates you might need. "
, word6[] = "If problems continue,disable or remove any newly installed hardware or software. Disable BIOS memory options such as caching or shadowing.\
 If you need to use Safe Mode to remove or disable components, restart your computer, press F8 to select Advanced Startup Options, and then select Safe Mode. ",
	word7[] = "Technical information: ", word8[] = "*** STOP: 0x0000000A (0x00000000,0xD0000002, 0x00000001,0x8082C582)", word9[] = "*** wdf01000.sys - Address 97C141AC base at 97C0E000, DateStamp 4fd91f51 ", * words[9] = { word1,word2,word3 ,word4 ,word5 ,word6 ,word7 ,word8,word9 };
int BSODstate;//蓝屏文字显示的标记
HWND BSODhwnd;//蓝屏窗体的hWnd

//杂项全局变量
int ColorChangingState = 1;
int ColorChangingLeft = 5;
int xLength, yLength;
Mypair DragState;
POINT DragDefPoint;
byte strWmap[65536];//记录字符宽度的数组
BOOL slient = FALSE;//是否命令行
HANDLE hdlWrite;//命令行窗口的句柄
RECT UTrc;//UT"窗口"贴图的位置
BOOL UTState = FALSE;//是否启用"超级置顶"
BOOL UTCheck = FALSE;//是否已经创建了"超级置顶"这一个Check
HDC DeskDC;//桌面DC
HWND Deskwnd;//桌面hWnd
BOOL LButtonDown;//记录鼠标左键是否按下
POINT UTMpoint;//记录鼠标坐标
POINT UTMpoint2;
BOOL FE = true;


//空下几行为Class留位置


class CathyClass//控件主类
{
public:
	void InitClass(HINSTANCE HInstance)
	{
		hInstance = HInstance;//设置hInst
		CurButton = CurFrame = CurCheck = CurLine = CurText = CurArea = CurEdit = 0;
		Obredraw = TRUE;
		Msv = 0;
		CurWnd = 1;//清"零"
		CoverButton = -1;
		Main.DPI = 1;
		ExpLine = ExpHeight = ExpWidth = 0;
	}
	__forceinline wchar_t* GetStr(LPCWSTR ID) { return str[Hash(ID)]; }//通过Hash + map 来快速索引字符串的数据结构		
																	   //ID(用于索引的字符串) -> Hash -(map)> 字符串地址

	void SetStr(LPCWSTR Str, LPCWSTR ID)//通过ID设置字符串
	{
		const unsigned int HashTemp = Hash(ID);
		if (str[HashTemp] != 0)HeapFree(GetProcessHeap(), 0, str[HashTemp]);//删除当前ID中原有的字符串
		str[HashTemp] = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * (mywcslen(Str) + 1));//申请相应的内存空间
		mywcscpy(str[HashTemp], Str);//复制新的
	}

	void CreateString(LPCWSTR Str, LPCWSTR ID)//创建新字符串
	{//注意:应仅在使用常量初始化时使用本函数,用变量初始化是建议CurString++再用SetStr
		++CurString;
		if (Str != NULL)//默认仅复制指针(危险!)
#ifdef _DEBUG
			str[Hash(ID)] = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * (mywcslen(Str) + 1)),
			mywcscpy(str[Hash(ID)], Str);
#elif _WIN64
			str[Hash(ID)] = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * (mywcslen(Str) + 1)),
			mywcscpy(str[Hash(ID)], Str);
#else
			str[Hash(ID)] = (LPWSTR)Str;
#endif
	}
	void CreateEditEx(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, HFONT Font, BOOL Ostr)
	{//创建自绘输入框
		++CurEdit;
		Edit[CurEdit].Left = Left; Edit[CurEdit].Top = Top;
		Edit[CurEdit].Width = Wid; Edit[CurEdit].Height = Hei;
		Edit[CurEdit].Page = Page;//设置长宽位置等信息

		if (Ostr)
		{//设置灰色标识的提示字符串
			mywcscpy(Edit[CurEdit].OStr, name);
			Edit[CurEdit].font = Font;//有提示字符串时不能有正常字符串
			Edit[CurEdit].str = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 8);
		}
		else//没有提示字符串:
			SetEditStrOrFont(name, Font, CurEdit);
	}
	void CreateArea(int Left, int Top, int Wid, int Hei, int Page)//创建点击区域
	{
		++CurArea;//设置长宽位置等信息
		Area[CurArea].Left = Left; Area[CurArea].Top = Top;
		Area[CurArea].Width = Wid; Area[CurArea].Height = Hei;
		Area[CurArea].Page = Page;
	}
	void CreateButtonEx(int Number, int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, HBRUSH Leave, \
		HBRUSH Hover, HBRUSH press, HPEN Leave2, HPEN Hover2, HPEN Press2, HFONT Font, BOOL Enabled, BOOL Dshadow, COLORREF FontRGB, LPCWSTR ID)
	{//创建按钮的复杂函数...
		Button[Number].Left = Left; Button[Number].Top = Top;//上下左右
		Button[Number].Width = Wid; Button[Number].Height = Hei;
		Button[Number].Page = Page; Button[Number].Leave = Leave;
		Button[Number].Hover = Hover; Button[Number].Press = press;//离开 & 悬浮 & 点击 , HBRUSH & HPEN
		Button[Number].Leave2 = Leave2; Button[Number].Hover2 = Hover2;
		Button[Number].Press2 = Press2; Button[Number].Font = Font;
		Button[Number].Enabled = Enabled; Button[Number].FontRGB = FontRGB;
		mywcscpy(Button[Number].Name, name); mywcscpy(Button[Number].ID, ID); but[Hash(ID)] = Number;
		Button[Number].Shadow = Dshadow;

		LOGBRUSH LogBrush;//从HBRUSH中提取出RGB颜色
		LOGPEN LogPen;//	(渐变色需要)
		GetObject(Leave, sizeof(LogBrush), &LogBrush); Button[Number].b1[0] = GetRValue(LogBrush.lbColor);
		Button[Number].b1[1] = GetGValue(LogBrush.lbColor); Button[Number].b1[2] = GetBValue(LogBrush.lbColor);
		GetObject(Hover, sizeof(LogBrush), &LogBrush); Button[Number].b2[0] = GetRValue(LogBrush.lbColor);
		Button[Number].b2[1] = GetGValue(LogBrush.lbColor); Button[Number].b2[2] = GetBValue(LogBrush.lbColor);
		GetObject(Leave2, sizeof(LogPen), &LogPen); Button[Number].p1[0] = GetRValue(LogPen.lopnColor);
		Button[Number].p1[1] = GetGValue(LogPen.lopnColor); Button[Number].p1[2] = GetBValue(LogPen.lopnColor);
		GetObject(Hover2, sizeof(LogPen), &LogPen); Button[Number].p2[0] = GetRValue(LogPen.lopnColor);
		Button[Number].p2[1] = GetGValue(LogPen.lopnColor); Button[Number].p2[2] = GetBValue(LogPen.lopnColor);
	}
	void CreateButton(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, LPCWSTR ID)//创建按钮（简化版）
	{
		++CurButton;//这里的name Wid Hei 不用全名是因为警告"隐藏了全局声明"
		Button[CurButton].Left = Left; Button[CurButton].Top = Top;//上下左右
		Button[CurButton].Width = Wid; Button[CurButton].Height = Hei;
		Button[CurButton].Page = Page; mywcscpy(Button[CurButton].Name, name);
		Button[CurButton].Enabled = TRUE; Button[CurButton].Shadow = 1 | 2;
		mywcscpy(Button[CurButton].ID, ID); but[Hash(ID)] = CurButton;
		Button[CurButton].Leave2 = Button[CurButton].Hover2 = Button[CurButton].Press2 = BlackPen;
		Button[CurButton].Leave = WhiteBrush; Button[CurButton].Hover = DBlueBrush; Button[CurButton].Press = LBlueBrush;
		Button[CurButton].b1[0] = Button[CurButton].b1[1] = Button[CurButton].b1[2] = 255;
		Button[CurButton].b2[0] = 210; Button[CurButton].b2[1] = Button[CurButton].b2[2] = 255;
	}
	void CreateFrame(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name)//创建内容框
	{//设置长宽位置等信息
		++CurFrame;//															-- Example ------
		Frame[CurFrame].Left = Left; Frame[CurFrame].Page = Page;//			    |				|
		Frame[CurFrame].Height = Hei; Frame[CurFrame].Top = Top;//			    |				|
		Frame[CurFrame].Width = Wid;//										    |				|
		mywcscpy(Frame[CurFrame].Name, name);//								    -----------------
	}
	void CreateCheck(int Left, int Top, int Page, int Wid, LPCWSTR name)//创建"√"
	{
		++CurCheck;//设置(没有长宽)位置等信息
		Check[CurCheck].Left = Left; Check[CurCheck].Top = Top;
		Check[CurCheck].Page = Page; Check[CurCheck].Width = Wid;
		mywcscpy(Check[CurCheck].Name, name);
	}
	void CreateText(int Left, int Top, int Page, LPCWSTR name, COLORREF rgb)//创建注释文字
	{
		++CurText;
		Text[CurText].Left = Left; Text[CurText].Top = Top;
		Text[CurText].Page = Page; Text[CurText].rgb = rgb;
		mywcscpy(Text[CurText].Name, name);
	}
	void CreateLine(int StartX, int StartY, int length, BOOL focus, int Page, COLORREF rgb)//创建线段
	{//注意:Create系列函数传入的都是 原始 坐标，
		++CurLine;//不受DPI缩放系统的影响
		Line[CurLine].StartX = StartX; Line[CurLine].StartY = StartY;
		Line[CurLine].Length = length; Line[CurLine].Focus = focus;//(Draw系列函数都是缩放过的坐标)
		Line[CurLine].Page = Page; Line[CurLine].Color = rgb;
	}
	BOOL InsideButton(int cur, POINT& point)//根据传入的Point判断鼠标指针是否在按钮内
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
		int i; RECT UPrc{0};
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Frame
		GetUpdateRect(hWnd, &UPrc, false);
		for (i = 1; i <= CurFrame; ++i)
		{
		begin:
			if (Frame[i].Page == CurWnd || Frame[i].Page == 0)
			{
				if (UPrc.right != 0)if (UPrc.right > (long)((Frame[i].Left + Frame[i].Width) * DPI) &&
					UPrc.left< (long)(Frame[i].Left * DPI) && UPrc.bottom>(long)((Frame[i].Top + Frame[i].Height) * DPI)
					&& UPrc.top < (long)(Frame[i].Top * DPI))continue;
				SelectObject(hdc, BlackPen);//绘制方框
				SelectObject(hdc, DefFont);
				MoveToEx(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI), NULL);//多加点(int)xx*DPI 减少警告
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI));
				SetTextColor(hdc, Frame[i].rgb);//自定义文字颜色
				RECT rc = GetRECTf(i);
				SetBkMode(hdc, OPAQUE);//打印上方文字
				DrawTextW(hdc, Frame[i].Name, (int)mywcslen(Frame[i].Name), &rc, NULL);
			}
			if (cur != 0)return;
		}
	}
	void DrawButtons(int cur)//绘制按钮
	{
		RECT UPrc = { 0 };
		int i;//如果使用ObjectRedraw则跳过其他Button
		if (cur != 0) { i = cur; goto begin; }//结构示意：选择颜色(渐变 or 禁用 or 默认) -> 选择字体

		GetUpdateRect(hWnd, &UPrc, false);
		for (i = 0; i <= CurButton; ++i)//				-> 绘制方框 -> (绘制下载进度条) -> 绘制文字 -> 清理
		{
		begin://										(在用于其他工程时，下载进度条部分可删除)

			if (Button[i].Page == CurWnd || Button[i].Page == 0)
			{
				if (Width < Button[i].Left)continue;
				if (UPrc.right != 0)if (UPrc.right > (long)((Button[i].Left + Button[i].Width) * DPI) &&
					UPrc.left< (long)(Button[i].Left * DPI) && UPrc.bottom>(long)((Button[i].Top + Button[i].Height) * DPI)
					&& UPrc.top < (long)(Button[i].Top * DPI))continue;

				HPEN TmpPen = 0; HBRUSH TmpBrush = 0;
				if (Button[i].Enabled == FALSE)//禁用则显示灰色
				{
					SelectObject(hdc, DGreyBrush);
					SelectObject(hdc, Button[i].Leave2);
					SetTextColor(hdc, COLOR_DARKEST_GREY);
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
					if (Press) {
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

				if (Button[i].Shadow && ButtonEffect)//在按钮的右下方画一圈阴影
				{
					SelectObject(hdc, DDGreyPen);
					MoveToEx(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI), (int)(Button[i].Top * DPI + 0.5), 0);
					LineTo(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI), (int)(Button[i].Top * DPI + Button[i].Height * DPI));
					LineTo(hdc, (int)(Button[i].Left * DPI + 0.5) - 1, (int)(Button[i].Top * DPI + Button[i].Height * DPI));

					SelectObject(hdc, DGreyPen);
					MoveToEx(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 1, (int)(Button[i].Top * DPI + 0.5), 0);
					LineTo(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 1, (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 1);
					LineTo(hdc, (int)(Button[i].Left * DPI + 0.5), (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 1);

					SelectObject(hdc, LGreyPen);
					MoveToEx(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 2, (int)(Button[i].Top * DPI + 0.5), 0);
					LineTo(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 2, (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 1);
					MoveToEx(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 1, (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 2, 0);
					LineTo(hdc, (int)(Button[i].Left * DPI + 0.5), (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 2);
				}

				RECT rc = GetRECT(i);

				SetBkMode(hdc, TRANSPARENT);//去掉文字背景
				if (Button[i].DownTot == 0)//打印文字(默认)
				{
					if (CoverButton == i)
					{
						rc.left += 2; /*rc.top += 2; */if (Press)rc.top += 2;
					}
					DrawTextW(hdc, Button[i].Name, (int)mywcslen(Button[i].Name), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
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
							wchar_t TempStr[20], TempNumber[MAX_NUM];//正在下载 (已下载个数)/(总数)
							myZeroMemory(TempNumber, sizeof(wchar_t) * MAX_NUM);
							mywcscpy(TempStr, GetStr(L"Loading"));
							myitow(Button[i].DownCur, TempNumber, MAX_NUM);
							mywcscat(TempStr, TempNumber);
							mywcscat(TempStr, L"/");//拼接字符串
							myitow(Button[i].DownTot, TempNumber, MAX_NUM);
							mywcscat(TempStr, TempNumber);
							DrawTextW(hdc, TempStr, (int)mywcslen(TempStr), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
		RECT UPrc{0};
		int i;
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Check
		GetUpdateRect(hWnd, &UPrc, false);
		for (i = 1; i <= CurCheck; ++i)
		{
		begin:
			if (Check[i].Page == 0 || Check[i].Page == CurWnd)
			{
				if (UPrc.right != 0)if (UPrc.right > (long)((Check[i].Left + Check[i].Width) * DPI) &&
					UPrc.left< (long)(Check[i].Left * DPI) && UPrc.bottom>(long)((Check[i].Top + 15) * DPI)
					&& UPrc.top < (long)(Check[i].Top * DPI))continue;
				if (Check[i].Value)SetTextColor(hdc, COLOR_DARKEST_GREY); else SetTextColor(hdc, COLOR_BLACK);
				if (i == CoverCheck)SelectObject(hdc, NormalBluePen); else SelectObject(hdc, NormalGreyPen);

				SelectObject(hdc, LGreyBrush);
				SelectObject(hdc, DefFont);//check默认边长为15，并不能调整
				Rectangle(hdc, (int)(Check[i].Left * DPI), (int)(Check[i].Top * DPI), (int)(Check[i].Left * DPI + 15 * DPI), (int)(Check[i].Top * DPI + 15 * DPI));
				SelectObject(hdc, WhiteBrush);
				SelectObject(hdc, WhitePen);
				Rectangle(hdc, (int)((Check[i].Left + 15) * DPI), (int)(Check[i].Top * DPI), (int)(Check[i].Left * DPI + Check[i].Width * DPI), (int)(Check[i].Top * DPI + 15 * DPI));
				TextOut(hdc, (int)(Check[i].Left * DPI + 20 * DPI), (int)(Check[i].Top * DPI), Check[i].Name, (int)mywcslen(Check[i].Name));
				if (Check[i].Value == 1)//用LineTo简单地打勾
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
				HPEN tmpPen = CreatePen(0, 1, Line[i].Color);
				SelectObject(hdc, tmpPen);//直接用LineTo
				MoveToEx(hdc, (int)(Line[i].StartX * DPI), (int)(Line[i].StartY * DPI), NULL);
				LineTo(hdc, (int)(Line[i].StartX * DPI + (!Line[i].Focus * Line[i].Length) * DPI), (int)(Line[i].StartY * DPI + (Line[i].Focus * Line[i].Length) * DPI));
				DeleteObject(tmpPen);
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
				wchar_t* TempText = str[Hash(Text[i].Name)];
				TextOutW(hdc, (int)(Text[i].Left * DPI), (int)(Text[i].Top * DPI), TempText, (int)mywcslen(TempText));
			}
			if (cur != 0)return;
		}
	}
#pragma warning(disable:26495)//禁用"始终初始化成员变量"的警告
	void DrawExp()//绘制注释
	{//注释只有一个，因此不需要ObjectRedraw
		if (!ExpExist)return;//注释不存在?
		SelectObject(hdc, DefFont);
		SelectObject(hdc, YellowPen);//选择注释专用的黄色背景
		SelectObject(hdc, YellowBrush);
		Rectangle(hdc, ExpPoint.x, ExpPoint.y, ExpWidth + ExpPoint.x, ExpHeight + ExpPoint.y + 2);
		SetTextColor(hdc, COLOR_BLACK);//逐行打印
		for (int i = 1; i <= ExpLine; ++i)//注意这里的ExpPoint , ExpWidth等都是缩放后坐标
			TextOutW(hdc, ExpPoint.x + 4, (int)(ExpPoint.y - 12 * DPI + 16 * i * DPI + 2 * i), Exp[i], (int)mywcslen(Exp[i]));
	}
	void DrawEdits(int cur)//绘制输入框
	{//(全Class最复杂的一个控件)
		RECT UPrc;
		int i, MOffset = 0, showBegin = 0, showEnd = 0;//结构示意:	创建缓存dc -> 绘制边框(蓝色 or 灰色) -> 打印文字(提示文字)
		HDC mdc;//													->打印文字(未选中) ->打印文字(选中) -> 从缓存dc贴图 -> 清理
		mdc = CreateCompatibleDC(hdc);
		GetUpdateRect(hWnd, &UPrc, false);
		SelectObject(mdc, EditBitmap);//Edit专用"三缓冲"Bitmap和dc
		SetBkMode(mdc, TRANSPARENT);
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Edits

		for (i = 1; i <= CurEdit; ++i)
		{
		begin:
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
			{
				if (UPrc.right != 0)if (UPrc.right > (long)((Edit[i].Left + Edit[i].Width) * DPI) &&
					UPrc.left< (long)(Edit[i].Left * DPI) && UPrc.bottom>(long)((Edit[i].Top + Edit[i].Height) * DPI)
					&& UPrc.top < (long)(Edit[i].Top * DPI))continue;
				SelectObject(mdc, WhitePen);//清空缓存dc
				SelectObject(mdc, WhiteBrush);
				Rectangle(mdc, 0, 0, MAX_EDIT_WIDTH, MAX_EDIT_HEIGHT);

				SelectObject(hdc, WhiteBrush);
				if (i == CoverEdit)SelectObject(hdc, NormalBluePen); else SelectObject(hdc, DarkGreyPen);//如果当前Edit被选中则用蓝色绘制边框
				Rectangle(hdc, (int)((Edit[i].Left - 5) * DPI), (int)(Edit[i].Top * DPI),//否则仅用默认的黑色
					(int)((Edit[i].Left + Edit[i].Width + 5) * DPI), (int)((Edit[i].Top + Edit[i].Height) * DPI));

				if (*Edit[i].OStr != 0)//如果当前Edit显示的是Ostr(仅用于提示的灰色文字)
				{//在Hdc上直接打印完走人
					SetTextColor(hdc, COLOR_DARKER_GREY);
					RECT rc{ (long)((Edit[i].Left - 5) * DPI) , (long)(Edit[i].Top * DPI),(long)((Edit[i].Left + Edit[i].Width + 5) * DPI),(long)((Edit[i].Top + Edit[i].Height) * DPI) };
					DrawTextW(hdc, Edit[i].OStr, (int)mywcslen(Edit[i].OStr), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					continue;
				}

				SIZE sel, ser;//pos1 / pos2:确定打印时"选中部分"真正的左右两端点
				int pos1, pos2;//因为有时候从左到右移动鼠标选中文字，有时从右向左
				if (Edit[i].Pos1 > Edit[i].Pos2 && Edit[i].Pos2 != -1)pos1 = Edit[i].Pos2, pos2 = Edit[i].Pos1; else pos1 = Edit[i].Pos1, pos2 = Edit[i].Pos2;
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
					}
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
					BitBlt(hdc, (int)((Edit[i].Left + Edit[i].Width / 2) * DPI) - Edit[i].strWidth / 2, yMax//Xoffset为0时居中打印
						, Edit[i].strWidth, Edit[i].strHeight + 4, mdc, 0, 0, SRCCOPY);
				else
					BitBlt(hdc, (int)(Edit[i].Left * DPI), yMax//有Xoffset时直接根据Xoffset-MOffset贴图
						, (int)(Edit[i].Width * DPI), Edit[i].strHeight + 4, mdc, Edit[i].XOffset - MOffset, 0, SRCCOPY);
			}
			if (cur != 0)goto end;
		}
	end:
		DeleteDC(mdc);
	}
	BOOL RedrawObject()//ObjectRedraw技术的分派函数
	{
		if (es[0].top != 0)//根据es来擦除区域
		{
			SelectObject(hdc, WhitePen);
			SelectObject(hdc, WhiteBrush);
			for (int i = 1; i <= es[0].top; ++i)
				Rectangle(hdc, es[i].left, es[i].top, es[i].right, es[i].bottom);
			es[0].top = 0;
		}
		if (rs[0].first != 0)
		{
			for (int i = 1; i <= rs[0].first; ++i)
				switch (rs[i].first) {
				case REDRAW_FRAME:DrawFrames(rs[i].second); break;
				case REDRAW_BUTTON: {DrawButtons(rs[i].second); break; }
				case REDRAW_CHECK: {  DrawChecks(rs[i].second); break; }
				case REDRAW_TEXT: {DrawTexts(rs[i].second); break; }
				case REDRAW_EDIT: {DrawEdits(rs[i].second); break; }
				default:DrawExp();
				}
			rs[0].first = 0;
			return TRUE;
		}
		return FALSE;
	}
	void DrawEVERYTHING() { DrawFrames(0); DrawButtons(0); DrawChecks(0); DrawTexts(0);  DrawLines(); DrawEdits(0); DrawExp(); }//自动绘制所有控件的函数，效率低，不应经常使用
	RECT GetRECT(int cur)//更新Buttons的rc 自带DPI缩放
	{
		return { (long)(Button[cur].Left * DPI), (long)(Button[cur].Top * DPI),
			(long)(Button[cur].Left * DPI + Button[cur].Width * DPI),(long)(Button[cur].Top * DPI + Button[cur].Height * DPI) };
	}
	RECT GetRECTf(int cur)//更新Frames的rc 自带DPI缩放
	{
		return { (long)(Frame[cur].Left * DPI + 10 * DPI) ,(long)(Frame[cur].Top * DPI - 7 * DPI) ,
		(long)(Frame[cur].Left * DPI + Frame[cur].Width * DPI),(long)(Frame[cur].Top * DPI + 30 * DPI) };
	}
	RECT GetRECTe(int cur)//更新Edit的rc 自带DPI缩放
	{
		return { (long)((Edit[cur].Left - 5) * DPI) ,(long)(Edit[cur].Top * DPI),
		(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI),(long)((Edit[cur].Top + Edit[cur].Height) * DPI) };
	}
	RECT GetRECTc(int cur)//更新Check的rc 自带DPI缩放
	{
		return { (long)(Check[cur].Left * DPI) ,(long)(Check[cur].Top * DPI) ,
			(long)(Check[cur].Left * DPI + Check[cur].Width * DPI) ,(long)(Check[cur].Top * DPI + 15 * DPI) };
	}
	void RefreshEditSize(int cur)
	{//当更改Edit的内容后，刷新Edit字符的宽度
		HDC mdc;
		HBITMAP bmp;//创建一个空bmp用来GetTextExtentPoint32(获取字符宽度)
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(mdc, bmp);//设置字体
		LOGFONTW Editfont;
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font), GetObject(Edit[cur].font, sizeof(LOGFONTW), &Editfont);
		else SelectObject(mdc, DefFont), GetObject(DefFont, sizeof(LOGFONTW), &Editfont);

		if (Edit[cur].strW != 0)HeapFree(GetProcessHeap(), 0, Edit[cur].strW);//删除旧的宽度
		Edit[cur].strW = (int*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(int) * (Edit[cur].strLength + 1));
		if (Edit[cur].strW == 0)return;
		SIZE se;
		GetTextExtentPoint32(mdc, Edit[cur].str, 1, &se);
		*Edit[cur].strW = se.cx;
		Edit[cur].strHeight = se.cy;
		for (int i = 1; i < Edit[cur].strLength; ++i)//依次获取每一位的宽度
		{//由于GetTextExtentPoint32函数效率出奇的慢，我们这里划出64k内存建立一个表格，
			int curstr = *(Edit[cur].str + i);//记录每个字符在字体为16号时的宽度，
			if (strWmap[curstr] == 0)//这样当每个字符第一次用到时使用GetTextExtentPoint32，之后查表就行了
			{//这样缺点是整个项目Edit中只能用同一种字体（比如宋体,楷体）,不过切换字体大小还是可以的
				GetTextExtentPoint32(mdc, Edit[cur].str + i, 1, &se);
				Edit[cur].strW[i] = Edit[cur].strW[i - 1] + se.cx;
				Edit[cur].strHeight = max(Edit[cur].strHeight, se.cy);
				strWmap[curstr] = (byte)(se.cx * 8 / Editfont.lfWidth);
				if (strWmap[curstr] == 0)strWmap[curstr] = 255;
			}
			else
			{
				if (strWmap[curstr] == 255)
					Edit[cur].strW[i] = Edit[cur].strW[i - 1];
				else//strW的第i位指的是第0位到第i位的宽度之和
					Edit[cur].strW[i] = Edit[cur].strW[i - 1] + (strWmap[curstr] * Editfont.lfWidth / 8);
				Edit[cur].strHeight = Editfont.lfHeight;//strHeight是高度的最大值
			}
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
			if (Edit[cur].str != NULL)if (*Edit[cur].str != NULL)HeapFree(GetProcessHeap(), 0, Edit[cur].str);
			Edit[cur].strLength = (int)mywcslen(Newstr);

			Edit[cur].str = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, (Edit[cur].strLength + 1) * sizeof(wchar_t));
			mywcscpy(Edit[cur].str, Newstr);
		}
		if (Edit[cur].str == 0) { Edit[cur].strWidth; return; }
		RefreshEditSize(cur);//刷新字符串宽度
		Edit[cur].strWidth = Edit[cur].strW[Edit[cur].strLength - 1];
		if ((int)(Edit[cur].Width * DPI) < Edit[cur].strWidth) {
			if (Edit[cur].XOffset == 0)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI) / 2;
		}
		else Edit[cur].XOffset = 0;
	}
	int GetNearestChar(int cur, POINT Point)//试着获取输入框中离光标最近的字符
	{//Point为窗体中真实坐标(缩放后坐标)
		POINT point;
		if (Edit[cur].strWidth <= Edit[cur].Width * DPI)//如果字符较少没有填满
		{//将窗体中真实坐标转化为Edit中的坐标
			if (Point.x < (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI - Edit[cur].strWidth / 2))return 0;
			if (Point.x > (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI + Edit[cur].strWidth / 2))return Edit[cur].strLength;
			point.x = Point.x - (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) + Edit[cur].strWidth / 2;
		}
		else//反之直接用XOffset计算
			point.x = (long)(Point.x - (long)(Edit[cur].Left * DPI) + Edit[cur].XOffset);
		const int pos = MyGetTextExtentPoint32Binary(cur, point.x);//获取大于point.x的第一个字符
		SIZE sel, ser;
		MyGetTextExtentPoint32(cur, -1, pos - 1, sel);//计算前一个字符宽度
		MyGetTextExtentPoint32(cur, -1, pos, ser);//计算后一个
		if (point.x < (sel.cx + ser.cx) / 2)return pos; else return pos + 1;//判断是选中光标左边还是右边的字符
	}
	void EditPaste(int cur)//将剪贴板中文字粘贴到Edit中的函数
	{
		if (cur == 0 || CoverEdit == 0)return;//如果没有选中Edit则退出 （一般不会出现这种情况的......吧）
		if (OpenClipboard(hWnd))
		{//打开剪切板
			HANDLE hData = GetClipboardData(CF_TEXT);
			if (hData == 0)return;//如果剪切板中格式不是CF_TEXT，则打开失败并退出
			char* buffer = (char*)GlobalLock(hData);
			if (buffer == 0)return;
			const size_t len = strlen(buffer), len2 = Edit[cur].strLength + 1;//因为这段代码只能粘贴文字
			wchar_t* ClipBoardTemp = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * (len + 1)),
				* EditTemp = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * (len + len2));
			if (ClipBoardTemp == 0 || EditTemp == 0)return;
			MultiByteToWideChar(CP_ACP, 0, buffer, -1, ClipBoardTemp, (int)(len + 1));
			GlobalUnlock(hData);
			CloseClipboard();

			int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2), pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
			if (pos1 == -1)
			{//如果只有单光标选中
				const wchar_t t = Edit[cur].str[pos2];
				Edit[cur].str[pos2] = 0;
				mywcscpy(EditTemp, Edit[cur].str);
				mywcscat(EditTemp, ClipBoardTemp);
				Edit[cur].str[pos2] = t;//在光标后面加入剪切板中字符并拼接
				mywcscat(EditTemp, &Edit[cur].str[pos2]);
				Edit[cur].Pos1 += (int)mywcslen(ClipBoardTemp);
				HeapFree(GetProcessHeap(), 0, ClipBoardTemp);
				SetEditStrOrFont(EditTemp, 0, cur);
				RefreshXOffset(cur);
				RefreshCaretByPos(cur);
			}
			else//选中区段
			{
				Edit[cur].str[pos1] = 0;
				mywcscpy(EditTemp, Edit[cur].str);
				mywcscat(EditTemp, ClipBoardTemp);//将选择部分替换成剪切板中字符并拼接
				mywcscat(EditTemp, &Edit[cur].str[pos2]);
				Edit[cur].Pos1 += (int)mywcslen(ClipBoardTemp); HeapFree(GetProcessHeap(), 0, ClipBoardTemp);
				Edit[cur].Pos2 = -1;
				SetEditStrOrFont(EditTemp, 0, cur);
				RefreshXOffset(cur);
				RefreshCaretByPos(cur);
			}
			HeapFree(GetProcessHeap(), 0, EditTemp);//清理内存
			EditRedraw(cur);//重绘控件
		}
	}
	void EditHotKey(int wParam)//Edit框按下热键时的分派函数
	{
		if (CoverEdit == 0)return;//如果没有选中Edit则退出 
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
			const int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)//Backspace键直接算在WM_CHAR里面了
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else EditDelete(CoverEdit, pos1, pos1 + 1);
			break;
		}
		return;
	}
	void EditUnHotKey()//取消注册Edit的热键
	{//在点击一个Edit外部时自动执行
		for (int i = 34; i < 41; ++i)AutoUnregisterHotKey(hWnd, i);
		HideCaret(hWnd);//隐藏闪烁的光标
	}
	void EditRegHotKey()//注册Edit的热键
	{//在点击一个Edit时自动执行
		AutoRegisterHotKey(hWnd, 34, NULL, VK_LEFT);//<-
		AutoRegisterHotKey(hWnd, 35, NULL, VK_RIGHT);//->
		AutoRegisterHotKey(hWnd, 36, MOD_CONTROL, 'V');//粘贴
		AutoRegisterHotKey(hWnd, 37, MOD_CONTROL, 'C');//复制
		AutoRegisterHotKey(hWnd, 38, MOD_CONTROL, 'X');//剪切
		AutoRegisterHotKey(hWnd, 39, MOD_CONTROL, 'A');//全选
		AutoRegisterHotKey(hWnd, 40, NULL, VK_DELETE);//Delete键
		DestroyCaret();//在点击的地方创建闪烁的光标
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
	}
	void EditCHAR(wchar_t wParam)//输入
	{
		if (Edit[CoverEdit].Press == TRUE || CoverEdit == 0)return;//没有选择Edit或鼠标按下时退出
		if (wParam >= 0x20 && wParam != 0x7F)//当按下的是正常按键时:
		{
			const int pos = min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			const int pos2 = max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			if (pos == -1)//没有选中文字(增加)
				EditAdd(CoverEdit, pos2, pos2, wParam);
			else//选中了一段文字(替换)
				EditAdd(CoverEdit, pos, pos2, wParam);
		}
		else if (wParam == VK_BACK)//Backspace键
		{
			const int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)//删除光标的前一个文字
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else//删除选中的一段文字
				EditDelete(CoverEdit, pos1 - 1, pos1);
		}
		return;
	}
	void EditAdd(int cur, int Left, int Right, wchar_t Char)//向某个Edit中添加一个字符 或 把一段字符替换成一个字符
	{//这里的Left&Right是字符位置而不是坐标
		const int len = Edit[cur].strLength + 5;//计算原Edit中文字的长度
		wchar_t* Tempstr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * len), t = 0;//申请对应长度的缓存空间
		if (Edit[cur].str == 0)Edit[cur].str = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * len);
		if (Tempstr == 0 || Edit[cur].str == 0)return;

		if (Left == Right)t = Edit[cur].str[Left];//如果只是添加字符，则先备份光标右侧的字符

		Edit[cur].str[Left] = 0;

		mywcscpy(Tempstr, Edit[cur].str);//将光标左侧的所有字符复制到缓存中
		Tempstr[Left] = Char;
		Tempstr[Left + 1] = 0;
		Edit[cur].str[Left] = t;//添加新的字符
		mywcscat(Tempstr, Edit[cur].str + Right);

		Edit[cur].Pos1 = Left + 1;//将右侧的字符再接上去
		Edit[cur].Pos2 = -1;
		myZeroMemory(Edit[cur].str, sizeof(wchar_t) * mywcslen(Edit[cur].str));
		SetEditStrOrFont(Tempstr, 0, cur);//刷新
		HeapFree(GetProcessHeap(), 0, Tempstr);
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	void EditDelete(int cur, int Left, int Right)//删除一个或一段字符
	{
		if (Left == -1)return;//在最左边还按下BackSpace ?
		wchar_t* Tempstr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * Edit[cur].strLength);
		if (Tempstr == 0)return;
		Edit[cur].str[Left] = 0;
		mywcscpy(Tempstr, Edit[cur].str);
		mywcscat(Tempstr, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left;
		Edit[cur].Pos2 = -1;
		SetEditStrOrFont(Tempstr, 0, cur);
		HeapFree(GetProcessHeap(), 0, Tempstr);
		RefreshXOffset(cur);//刷新
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	void EditAll(int cur)//选中一个Edit中所有字符
	{
		if (cur == 0)return;//未选中任何Edit
		Edit[cur].Pos1 = 0;
		Edit[cur].Pos2 = Edit[cur].strLength;
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);//刷新
	}

	void EditMove(int cur, int offset)//移动选中的Edit中光标的位置
	{//offset可以为负
		int xback;//offset也可以不是±1
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
		const int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2);
		const int pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		EditStr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * (pos2 - pos1 + 1));//这一段代码和EditPaste很像
		ClipBoardStr = (char*)HeapAlloc(GetProcessHeap(), 0, sizeof(char) * (pos2 - pos1 + 1) * 2);//(暂时)就不介绍了
		if (EditStr == 0 || ClipBoardStr == 0)return;
		t = Edit[cur].str[pos2];
		Edit[cur].str[pos2] = 0;
		mywcscpy(EditStr, Edit[cur].str + pos1);
		Edit[cur].str[pos2] = t;
		WideCharToMultiByte(CP_ACP, 0, EditStr, -1, ClipBoardStr, (pos2 - pos1 + 1) * 2, NULL, NULL);

		if (OpenClipboard(hWnd))
		{
			char* buffer;
			EmptyClipboard();
			HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(ClipBoardStr) + 1);
			if (!clipbuffer)return;
			buffer = (char*)GlobalLock(clipbuffer);
			if (buffer != NULL)strcpy(buffer, LPCSTR(ClipBoardStr));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
		HeapFree(GetProcessHeap(), 0, EditStr);
		HeapFree(GetProcessHeap(), 0, ClipBoardStr);
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
				if (Edit[cur].strW[mid - 1] < point)return mid;
				else
					right = mid - 1;//太过右边->向左二分
			}
		}
		if (left == 0)return 0; else return Edit[cur].strLength - 1;//找不到->返回最左 or 最右
	}

	void RefreshCaretByPos(int cur)//刷新选中的Edit中光标的位置
	{
		if (Edit[cur].Pos1 == -1)return;//指定Edit未被选中->退出
		if (Edit[cur].strLength == 0) {
			CaretPos.x = (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI), CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - 6 * DPI);
			goto finish;
		}
		SIZE se;//通过这个Edit的Pos1的字符来计算字符长度
		if (Edit[cur].Pos2 != -1)
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos2 - 1, se);
		else
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos1 - 1, se);
		if (Edit[cur].XOffset == 0)//减去Xoffset，再加上Edit的坐标就是光标位置了
			CaretPos.x = se.cx + (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) - Edit[cur].strWidth / 2, CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2);
		else
			CaretPos.x = (long)(se.cx + (long)(Edit[cur].Left * DPI) - Edit[cur].XOffset), CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2);
	finish:
		SetCaretPos(CaretPos.x, CaretPos.y);
		ShowCaret(hWnd); ShowCrt = TRUE;
	}
	void EditDown(int cur)//鼠标左键在某个Edit上按下
	{
		EditRegHotKey();//先注册下热键再说
		if (CoverEdit != cur)Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1, EditRedraw(CoverEdit);//如果直接从一个Edit点到
		CoverEdit = cur;//														另一个Edit，那么先把之前的Pos和蓝框问题解决好
		if (*Edit[cur].OStr != 0)//去掉灰色的提示文字
		{
			*Edit[cur].OStr = 0;
			myZeroMemory(Edit[cur].str, sizeof(wchar_t) * mywcslen(Edit[cur].str));
		}
		Edit[cur].Pos1 = Edit[cur].Pos2 = -1;
		Edit[cur].Press = TRUE;
		Edit[cur].Pos1 = GetNearestChar(cur, GetPos());//计算Pos1

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
	POINT GetPos()//获取鼠标坐标的函数
	{//正常的GetCursorPos要3行，GetPos()仅1行(不过速度稍慢)
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		return point;
	}
	void EditComposition()//为Edit设置输入法显示位置。
	{
		POINT point = GetPos();
		LOGFONT lf;
		COMPOSITIONFORM cf;
		HIMC himc = ImmGetContext(hWnd);
		if (himc)
		{
			GetCaretPos(&point);
			cf.dwStyle = CFS_POINT;
			cf.ptCurrentPos.y = point.y + 10;
			cf.ptCurrentPos.x = point.x + 10;
			ImmSetCompositionWindow(himc, &cf);

			//设置输入法字体样式(很多时候没用)
			GetObject(DefFont, sizeof(LOGFONT), &lf);
			ImmSetCompositionFont(himc, &lf);

			ImmReleaseContext(hWnd, himc);
		}
	}
	void LeftButtonUp()//鼠标左键抬起
	{
		if (CoverButton != -1)
		{
			Press = 0;//取消被按下按钮的颜色
			ButtonRedraw(CoverButton);
		}
		Edit[CoverEdit].Press = FALSE;
	}
	inline void LeftButtonUp2()//鼠标左键抬起(位于WM_LBUTTONUP消息的最后执行)
	{
		if (CoverCheck != 0)//更改Check的值并重绘
		{
			Check[CoverCheck].Value = !Check[CoverCheck].Value;
			Readd(REDRAW_CHECK, CoverCheck);
			Redraw(GetRECTc(CoverCheck));
		}
		Timer = GetTickCount();
		DestroyExp();//关闭exp
	}
	bool TestInside()
	{
		POINT point = Main.GetPos();
		Main.EditGetNewInside(point);//试图预先确定一下是否点在某个控件内
		Main.ButtonGetNewInside(point);//(这是为了决定是否允许拖动窗口)
		Main.CheckGetNewInside(point);
		Main.AreaGetNewInside(point);
		if (Main.CoverButton != -1 || Main.CoverCheck != 0 || Main.CoverEdit != 0 || Main.CoverArea != 0)return true;
		else return false;
	}
	void LeftButtonDown()//鼠标左键按下
	{
		POINT point = GetPos();
		if (CoverButton != -1)//当按钮按下 & 停留在按钮上时
		{
			Press = 1;//重绘这个按钮
			ButtonRedraw(CoverButton);
		}
		if (!InsideEdit(CoverEdit, point) && CoverEdit != 0)
		{
			Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = 0;
			const int TempCoverEdit = CoverEdit;//原来一个Edit被激活
			CoverEdit = 0;//现在鼠标点在那个Edit外面时
			EditRedraw(TempCoverEdit);//就要将CoverEdit设为0;
			EditUnHotKey();//取消热键
		}
		if (EditPrv != 0)
		{//鼠标点在另一个Edit上时为了改变边框的颜色，同样需要重绘
			EditRedraw(EditPrv);
			EditPrv = 0;
		}
		if (CoverEdit != 0)//停留在Edit上时~
			EditDown(CoverEdit);
		else EditUnHotKey();
#pragma warning(disable:28159)//GetTickCount会每49天溢出一次
		//但GetTickCount64仅在vista以上支持
		Timer = GetTickCount();//重置exp的计时器
		DestroyExp();//任何操作都会导致exp的关闭
	}

	void CheckGetNewInside(POINT& point)//检查鼠标是否在任何Check内
	{//POINT为鼠标真实坐标
		for (int i = 1; i <= CurCheck; ++i)//遍历所有Check
			if (Check[i].Page == CurWnd || Check[i].Page == 0)//不在同一页面 -> 直接跳过
				if (InsideCheck(i, point) != 0)//在Check的文字中或方框内
				{
					CoverCheck = i;//设置CoverCheck
					if (Obredraw)Readd(REDRAW_CHECK, i);
					Redraw(GetRECTc(i));//重绘
					return;
				}
	}
	void ButtonGetNewInside(POINT& point)//检查鼠标是否在任何Button内
	{//POINT为鼠标真实坐标
		for (int i = 0; i <= CurButton; ++i)//历史原因，Button编号是从0开始的
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled && Button[i].DownTot == 0)
				if (InsideButton(i, point))//在按钮中
				{
					CoverButton = i;//设置CoverButton
					if (ButtonEffect)//特效开启
					{//设定渐变色
						Button[i].Percent += 3;//先给30%的颜色 （太淡了看不出来）
						if (Button[i].Percent > 100)Button[i].Percent = 100;
					}
					ButtonRedraw(i);//重绘
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
					return;
				}
	}
	void MouseMove()//鼠标移动
	{
		POINT point = GetPos();
		if (CoverButton == -1)ButtonGetNewInside(point);//原来不在按钮内 -> 看看现在是否移进按钮
		else//原来在
		{
			if (!Button[CoverButton].Enabled) { CoverButton = -1; goto disabled; }//这个按钮被禁用了  直接跳到下面
			if ((Button[CoverButton].Page != CurWnd && Button[CoverButton].Page != 0) || !InsideButton(CoverButton, point))
			{//现在不在
				if (Obredraw)Readd(REDRAW_BUTTON, CoverButton);
				if (ButtonEffect)
				{//CoverButton设为-1 , 重绘
					Button[CoverButton].Percent -= Delta;
					if (Button[CoverButton].Percent < 0)Button[CoverButton].Percent = 0;
				}
				const RECT rc = GetRECT(CoverButton);
				CoverButton = -1;
				Redraw(rc);
				ButtonGetNewInside(point);//有可能从一个按钮直接移进另一个按钮内
			}
		}
	disabled:
		if (CoverCheck == 0)CheckGetNewInside(point);//在外面 -> 寻找新check
		else
		{//同理
			if ((Check[CoverCheck].Page != CurWnd && Check[CoverCheck].Page != 0) || InsideCheck(CoverCheck, point) == 0)
			{
				if (Obredraw)Readd(REDRAW_CHECK, CoverCheck);
				const RECT rc = GetRECTc(CoverCheck);
				CoverCheck = 0;
				Redraw(rc);
				CheckGetNewInside(point);
			}
		}
		if (CoverEdit != 0 && Edit[CoverEdit].Press == TRUE)
		{
			//如果Edit被按下 (同时在拖动选择条)
			const int t = Edit[CoverEdit].Pos2;
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
		if (Obredraw)Readd(REDRAW_EDIT, cur);
		Redraw(GetRECTe(cur));//标准ObjectRedraw写法
	}
	void ButtonRedraw(int cur)//重绘Button的外壳函数
	{
		if (Obredraw)Readd(REDRAW_BUTTON, cur);
		Redraw(GetRECT(cur));//标准ObjectRedraw写法
	}
	BOOL YesNoBox(LPCWSTR Str)
	{
		return MessageBox(hWnd, GetStr(Str), GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) == IDOK;
	}
	void InfoBox(LPCWSTR Str)//全自动的MessageBox
	{
		const wchar_t* GUIStrExist = GetStr(Str);
		if (!slient)//如果Str中是GUIstr的ID则打印str的内容，否则直接打印Str
		{
			if (noMsgbox) {//当不允许弹出MessageBox时直接用TextOut方式打印到窗口中
				if (GUIStrExist)TextOut(hdc, 200, 55, GetStr(Str), (int)mywcslen(GetStr(Str))); else TextOut(hdc, 200, 55, Str, (int)mywcslen(Str));
			}
			else {
				if (GUIStrExist)MessageBox(hWnd, GetStr(Str), GetStr(L"Info"), 0x40L); else MessageBox(hWnd, Str, GetStr(L"Info"), 0x40L);
			}
		}
		else
		{
			wchar_t ConsoleTemp[MAX_STR];
			mywcscpy(ConsoleTemp, GetStr(L"TDT"));
			if (GUIStrExist)mywcscat(ConsoleTemp, GetStr(Str)); else mywcscat(ConsoleTemp, Str);
			WriteConsole(hdlWrite, ConsoleTemp, (DWORD)mywcslen(ConsoleTemp), nullptr, nullptr);
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
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos1 - 1, se);//计算长度	过程比较复杂，但原理简单
		if (se.cx < Edit[cur].XOffset)Edit[cur].XOffset = se.cx - 1;
		if (se.cx > (long)(Edit[cur].XOffset + Edit[cur].Width * DPI))Edit[cur].XOffset += (int)(se.cx - (Edit[cur].XOffset + Edit[cur].Width * DPI) - 1);
		if (Edit[cur].strWidth - Edit[cur].XOffset < Edit[cur].Width * DPI)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	void SetPage(int newPage)//设置窗口的页数
	{
		if (newPage == CurWnd)return;//点了当前页的按钮，直接退出
		/*EnableButton(CurWnd, true);
		EnableButton(newPage, false);*/
		HideCaret(hWnd);//换页时自动隐藏闪烁的光标
		ShowCrt = FALSE;
		Edit[CoverEdit].Press = FALSE;
		Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1;
		CoverEdit = 0;//取消Edit的选中
		CurWnd = newPage;
		Redraw();//切换页面时当然需要全部重绘啦
	}
	void SetDPI(float NewDPI)//改变窗口的缩放大小
	{//						(由于某历史原因，缩放大小的变量被我命名成了DPI)
		if (DPI == NewDPI)return;
		DPI = NewDPI;//创建新大小的字体
		if (DefFont)DeleteObject(DefFont);
		DefFont = CreateFontW(max((int)(16 * DPI), 8), max((int)(8 * DPI), 4), 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"宋体");

		for (int i = 1; i <= CurEdit; ++i)SetEditStrOrFont(0, DefFont, i), RefreshXOffset(i);//设置字体
		RefreshCaretByPos(CoverEdit);
		if (Width != 0 && Height != 0)SetWindowPos(hWnd, NULL, 0, 0, (int)(Width * DPI - 0.5), (int)(Height * DPI - 0.5), SWP_NOMOVE | SWP_NOREDRAW);

		DestroyCaret();//设置闪烁的光标
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
		Redraw();
	}

	LPWSTR GetCurInsideID(POINT& point)//获取当前鼠标处于的按钮的ID
	{
		for (int i = 0; i <= CurButton; ++i)
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled)
				if (InsideButton(i, point) && Button[i].DownTot == 0)
					return Button[i].ID;//返回ID
		return Button[0].ID;//返回一个空值
	}

	void EnableButton(int cur, BOOL enable)
	{
		Button[cur].Enabled = enable;
		if (Button[cur].Page == CurWnd || Button[cur].Page == 0)ButtonRedraw(cur);
	}
	byte* pvBits;
	void WindowPreparation(int MaxWidth, int MaxHeight)//给要绘制的窗口设置双缓冲hdc和bitmap
	{
		tdc = GetDC(hWnd);
		hdc = CreateCompatibleDC(tdc);
		//Bitmap = CreateCompatibleBitmap(tdc, MaxWidth, MaxHeight);


		BITMAPINFO bmi;        // bitmap header

		ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = MaxWidth;
		bmi.bmiHeader.biHeight = MaxHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;         // four 8-bit components
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = MaxWidth * MaxHeight * 3;
		Bitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pvBits, NULL, 0);

		if (Bitmap != 0)SelectObject(hdc, Bitmap);
		ReleaseDC(hWnd, tdc);
		if (EditBitmap != NULL)DeleteObject(EditBitmap);//给Edit创建一个Bitmap
		EditBitmap = CreateCompatibleBitmap(hdc, MAX_EDIT_WIDTH, MAX_EDIT_HEIGHT);
		//ReleaseDC(hWnd,hdc);
	}
	void Try2CreateExp()//尝试解析一个Exp的内容并准备用于绘制
	{
		SIZE se;
		if (ExpExist == TRUE || CoverButton == -1)return;//如果Exp已经存在或内容为空，那么就直接退出
		if (Button[CoverButton].Exp == 0)return;
		if (*Button[CoverButton].Exp == 0)return;
		ExpExist = TRUE;
		ExpLine = 0;//清零
		myZeroMemory(Exp, sizeof(Exp));
		wchar_t* x = Button[CoverButton].Exp, * y = Button[CoverButton].Exp;
		ExpHeight = 8;
		ExpWidth = 0;
		while (1)
		{
			++ExpLine;
			x = mywcsstr(x + 1, L"\\n");//在Exp的字符串中寻找\n字符
			if (x != 0)x[0] = 0;//然后存储在一个二维数组内
			if (ExpLine == 1)mywcscpy(Exp[ExpLine], y); else mywcscpy(Exp[ExpLine], y + 2);
			if (ExpLine == 1)GetTextExtentPoint32(hdc, y, (int)mywcslen(y), &se); else GetTextExtentPoint32(hdc, y + 2, (int)mywcslen(y + 2), &se);//获取字符串的宽度
			if (x != 0)x[0] = '\\';
			ExpHeight += (se.cy + 2);//计算这个Exp的宽和高
			ExpWidth = max(ExpWidth - 8, se.cx) + 8;
			if (x == 0)break;
			y = x;
		}
		ExpPoint = GetPos();
		if (ExpPoint.x > (int)((float)Width * DPI / (float)2.0))ExpPoint.x -= (ExpWidth + 6); else ExpPoint.x += 12;//自动选择注释的位置
		if (ExpPoint.y > (int)((float)Height * DPI / (float)2.0))ExpPoint.y -= (ExpHeight + 6); else ExpPoint.y += 14;//防止打印到窗口外面
		RECT rc{ ExpPoint.x, ExpPoint.y, ExpPoint.x + ExpWidth, ExpPoint.y + ExpHeight + 2 };//注意这里的ExpPoint等都是真实坐标
		Readd(REDRAW_EXP, 1);
		Redraw(rc);
	}
	void DestroyExp()//清除这个Exp
	{
		if (!ExpExist)return;
		ExpExist = FALSE;//删除Exp时要绘制这个Exp下面的控件，分类绘制很麻烦，干脆就全部刷新一下吧
		RECT rc{ ExpPoint.x, ExpPoint.y, ExpPoint.x + ExpWidth, ExpPoint.y + ExpHeight + 2 };
		es[++es[0].top] = rc;
		Redraw(rc);
		ExpLine = ExpHeight = ExpWidth = 0;//注意:ExpWidth等值一定要先使用再清空
	}
	void Redraw(const RECT& rc) { InvalidateRect(hWnd, &rc, FALSE), UpdateWindow(hWnd); }//自动重绘 & 刷新指定区域
	void Redraw() {
		rs[0].first = es[0].top = 0;
		InvalidateRect(hWnd, nullptr, FALSE);
		UpdateWindow(hWnd);
	}//添加要刷新的控件~
	void Readd(int type, int cur) { rs[++rs[0].first] = { type,cur }; }//1=Frame,2=Button,3=Check,4=Text,5=Edit


	BOOL GetLanguage(GETLAN& getlan)//将一行文字解析成可用的信息
	{//这些函数都是代码长而原理简单，因此没有注释
		wchar_t* str1 = mywcsstr(getlan.begin, L"\"");
		if (str1 == 0)return false;
		wchar_t* str2 = mywcsstr(str1 + 1, L"\"");
		if (str2 == 0) goto error;
		*str2 = 0;
		getlan.str1 = str1 + 1;//Name
		str1 = mywcsstr(str2 + 1, L",");
		if (str1 == NULL)return TRUE;//Left
		str2 = mywcsstr(str1 + 1, L",");
		if (str2 != NULL)*str2 = 0;
		getlan.Left = mywtoi(str1 + 1);//Top
		if (str2 == NULL)return TRUE;
		str1 = mywcsstr(str2 + 1, L",");//Width
		if (str1 != NULL)*str1 = 0;
		getlan.Top = mywtoi(str2 + 1);
		if (str1 == NULL)return TRUE;
		str2 = mywcsstr(str1 + 1, L",");//Height
		if (str2 != NULL)*str2 = 0;
		getlan.Width = mywtoi(str1 + 1);
		if (str2 == NULL)return TRUE;
		str1 = mywcsstr(str2 + 1, L",");
		if (str1 != NULL)*str1 = 0;
		getlan.Height = mywtoi(str2 + 1);
		if (str1 == 0)return TRUE;
		str1 = mywcsstr(str1 + 1, L"\"");//Str
		if (str1 == 0)goto error;
		str2 = mywcsstr(str1 + 1, L"\"");
		if (str2 == 0)goto error;
		*str2 = 0;
		getlan.str2 = str1 + 1;
		return TRUE;
	error:
		InfoBox(L"unk");
		return FALSE;
	}
	void DispatchLanguage(LPWSTR ReadTmp, int type)
	{//将语言文件中读取到的一行设置到Class中
		wchar_t Readm[MAX_STR + 100];
		mywcscpy(Readm, ReadTmp);
		GETLAN gl = { 0 };
		//myZeroMemory(&gl, sizeof(GETLAN));
		wchar_t* pos = mywcsstr(Readm, L"=");
		if (pos == 0)return;
		*pos = 0; gl.begin = pos + 1;
		wchar_t* space = mywcsstr(Readm, L" ");
		if (space != 0)*space = 0;//分不同的控件讨论

		if (type == 1)//button
		{
			int cur = but[Hash(Readm)];
			if (!GetLanguage(gl))goto error;
			if (gl.Left != -1)Button[cur].Left = gl.Left;
			if (gl.Top != -1)Button[cur].Top = gl.Top;
			if (gl.Width != -1)Button[cur].Width = gl.Width;
			if (gl.Height != -1)Button[cur].Height = gl.Height;
			if (gl.str1 != NULL)mywcscpy(Button[cur].Name, gl.str1);
			if (Button[cur].Exp != nullptr)if (*Button[cur].Exp != NULL)
			{
				HeapFree(GetProcessHeap(), 0, Button[cur].Exp);
				Button[cur].Exp = 0;
			}
			if (gl.str2 != NULL)
			{
				Button[cur].Exp = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * (mywcslen(gl.str2) + 1));
				mywcscpy(Button[cur].Exp, gl.str2);
			}
			return;
		}
		if (type == 2)//check
		{
			int cur = mywtoi(Readm + 1);
			if (!GetLanguage(gl))goto error;
			if (gl.Left != -1)Check[cur].Left = gl.Left;
			if (gl.Top != -1)Check[cur].Top = gl.Top;
			if (gl.Width != -1)Check[cur].Width = gl.Width;
			if (gl.str1 != NULL)mywcscpy(Check[cur].Name, gl.str1);
			return;
		}
		if (type == 3)//string
		{
			wchar_t tmp[MAX_STR]; BOOL f = FALSE;
			myZeroMemory(tmp, sizeof(wchar_t) * 256);
			//mywcscpy(tmp2, gl.begin);

			wchar_t* str1 = mywcsstr(pos + 1, L"\""), * str2, * str3;
			if (str1 == 0)goto error;
			str2 = str1;
			str3 = str2 + 1;
			while (1)
			{
				str2 = mywcsstr(str2, L"\\n");
				if (str2 == NULL)break;
				f = TRUE;
				*str2 = 0;
				mywcscat(tmp, str3);
				mywcscat(tmp, L"\n");
				str3 = str2 + 2;
				str2 = str2 + 1;
			}
			if (f == FALSE)
				str2 = mywcsstr(str1 + 1, L"\"");
			else
				str2 = mywcsstr(str3, L"\"");
			if (str2 == NULL)goto error;
			*str2 = 0;
			mywcscat(tmp, str3);
			SetStr(tmp, Readm);
			return;
		}
		if (type == 4)//frame
		{
			int cur = mywtoi(Readm + 1);
			if (!GetLanguage(gl))goto error;
			if (gl.Left != -1)Frame[cur].Left = gl.Left;
			if (gl.Top != -1)Frame[cur].Top = gl.Top;
			if (gl.Width != -1)Frame[cur].Width = gl.Width;
			if (gl.Height != -1)Frame[cur].Height = gl.Height;
			if (gl.str1 != NULL)mywcscpy(Frame[cur].Name, gl.str1);
			return;
		}
		if (type == 5)//text
		{
			int cur = mywtoi(Readm + 1);
			wchar_t* tmpstr = mywcsstr(pos + 1, L",");
			if (tmpstr == 0)goto error;
			*tmpstr = 0;
			const int NewLeft = mywtoi(pos + 1);
			const int NewTop = mywtoi(tmpstr + 1);
			if (NewLeft != -1)Text[cur].Left = NewLeft;
			if (NewTop != -1)Text[cur].Top = NewTop;
			return;
		}
		if (type == 6)//edit
		{
			int cur = mywtoi(Readm + 1);
			if (!GetLanguage(gl))goto error;
			if (gl.Left != -1)Edit[cur].Left = gl.Left;
			if (gl.Top != -1)Edit[cur].Top = gl.Top;
			if (gl.Width != -1)Edit[cur].Width = gl.Width;
			if (gl.Height != -1)Edit[cur].Height = gl.Height;
			if (gl.str1 != NULL && Edit[cur].strLength == 0)mywcscpy(Edit[cur].OStr, gl.str1);
			return;
		}//仍然是长但原理简单的代码

		return;
	error:
		wchar_t tmpstr[MAX_STR];
		mywcscpy(tmpstr, L"error:");
		mywcscat(tmpstr, Readm);
		InfoBox(tmpstr);
		return;
	}

	void DrawTitleBar()
	{
		SelectObject(hdc, TitleBar.hPen);
		SelectObject(hdc, TitleBar.hBrush);
		Rectangle(hdc, 0, 0, (int)(DPI * Width + 1), (int)(DPI * TitleBar.Height));
	}
	void SetTitleBar(int newColor, int newHeight)
	{
		if (TitleBar.hPen)DeleteObject(TitleBar.hPen);
		if (TitleBar.hBrush)DeleteObject(TitleBar.hBrush);
		TitleBar.hPen = CreatePen(PS_SOLID, 1, newColor);
		TitleBar.hBrush = CreateSolidBrush(newColor);
		TitleBar.Height = newHeight;
		TitleBar.col = newColor;
	}

	//
	//下面是Class的变量
	//

	struct TitleEx
	{
		HPEN hPen; HBRUSH hBrush; int Height; COLORREF col;
	}TitleBar;
	struct ButtonEx//按钮
	{
		long Left, Top, Width, Height, Page, Download, Percent, DownCur, DownTot;
		BOOL Enabled, Shadow;
		HBRUSH Leave, Hover, Press;//离开 and 悬浮 and 按下
		HPEN Leave2, Hover2, Press2;
		HFONT Font;
		wchar_t Name[31], ID[MAX_ID], * Exp;
		COLORREF FontRGB;
		BYTE b1[3], b2[3], p1[3], p2[3];
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
		BOOL Value;
		wchar_t Name[51];
	}Check[MAX_CHECK];
	struct LineEx//线段
	{
		int StartX, StartY, Length, Page;//线段的起始坐标和终点坐标
		BOOL Focus;
		COLORREF Color;
	}Line[MAX_LINE];
	struct TextEx//文字
	{
		int Left, Top, Page;
		COLORREF rgb;
		wchar_t Name[MAX_ID];//这里的"Name"其实是GUIString的ID
	}Text[MAX_TEXT];
	struct EditEx//输入框
	{
		int Left, Top, Width, Height, Page;
		int strWidth, strHeight, Pos1, Pos2, XOffset, strLength;
		int* strW;
		BOOL Press;
		wchar_t* str, OStr[21];
		HFONT font;
	}Edit[MAX_EDIT];
	struct ClickAreaEx//点击区域
	{
		int Left, Top, Width, Height, Page;
	}Area[MAX_AREA];

	int ExpLine, ExpHeight, ExpWidth;//关于Explaination的几个变量
	wchar_t Exp[MAX_EXPLINES][MAX_EXPLENGTH];
	POINT ExpPoint, ExpPoint2;
	BOOL ShowExp;//Exp是否被显示
	DWORD Timer;//Exp开启的时间
	BOOL ExpExist;//Exp是否存在

	Map<unsigned int, wchar_t*> str;//字符串的ID ->编号(用于索引)
	Map<unsigned int, int>but;//Button的ID -> 编号
	HFONT DefFont;//默认的字体
	int Msv;//鼠标移出检测变量
	int CurString, CurButton, CurFrame, CurCheck, CurLine, CurText, CurEdit, CurArea;//各种控件的数量
	float DPI;//缩放大小
	int CoverButton, CoverCheck, CoverEdit, CoverArea;//当前被鼠标覆盖的东西
	BOOL Obredraw;//是否启用ObjectRedraw技术 - 默认开启
	BOOL ButtonEffect;//是否开启渐变色
	int CurWnd;//当前的页面
	int Press;//鼠标左键是否按下
	POINT CaretPos;//闪烁的光标的位置
	BOOL ShowCrt;//是否显示闪烁的光标
	BOOL noMsgbox;//是否将提示信息打印在界面上，而不是弹出MessageBox
	Mypair rs[MAX_RS];//重绘列表 1=Frame,2=Button,3=Check,4=Text,5=Edit
	RECT es[MAX_ES];//清理列表
	HDC hdc, tdc;//缓存 and 真实dc
	HBITMAP EditBitmap;//Edit专用缓存bitmap
	HBITMAP Bitmap;//缓存窗口bitmap
	int Width, Height;//窗口的宽和高
	HWND hWnd;//Class绘制的窗体的hwnd
	HINSTANCE hInstance;//程序的hInst
	int EditPrv;//之前被激活的edit序号

	//没有任何private变量或函数= =
}Main;

//Class的声明结束
//下面是各种函数

#pragma warning(default:26495)
#pragma warning(disable:4100)//禁用警告
class DownloadProgress : public IBindStatusCallback {
public://这些函数中有些参数没有用到，会导致大量警告.
	int curi;/*推送下载进度信息的按钮编号*/ int factmax;//手动设置被下载文件的真实大小
	HRESULT __stdcall QueryInterface(const IID&, void**) { return E_NOINTERFACE; }
	ULONG STDMETHODCALLTYPE AddRef() { return 1; }//暂时没用的函数，从msdn上抄下来的
	ULONG STDMETHODCALLTYPE Release() { return 1; }
	HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, IBinding* pib) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetPriority(LONG* pnPriority) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved) { return S_OK; }
	HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, LPCWSTR szError) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD* grfBINDF, BINDINFO* pbindinfo) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pformatetc, STGMEDIUM* pstgmed) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID riid, IUnknown* punk) { return E_NOTIMPL; }
	HRESULT __stdcall OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		if (ulProgressMax != 0)//ulProgress：已下载的字节数
		{//ulProgressMax：总字节数
			float percentage;
			if (factmax == 0) percentage = (float)ulProgress / (float)ulProgressMax * 100;//计算下载百分比
			else  percentage = (float)ulProgress / (float)factmax * 100;//注意！有时函数无法得到正确的ulProgressMax，虽然没有下完
			if (Main.Button[curi].Download != (int)percentage + 1)//percentage仍会等于100%，导致一系列错误
			{//																		这时候应尽量传入文件的正确大小(factmax)
				Main.Button[curi].Download = (int)percentage + 1;
				Main.ButtonRedraw(curi);
			}
		}
		return S_OK;
	}
};

BOOL CALLBACK EnumChildwnd(HWND hwnd, LPARAM lParam)//查找"屏幕广播"子窗口的枚举函数.
{
	if (lParam == 1)//lParam == 1：用于"一键安装"启用极域的所有按钮
	{//2016版极域广播窗口右上角有一个控制着全屏广播的按钮，默认禁用
		EnableWindow(hwnd, TRUE);//把它启用后，就能手动窗口化广播了
		return CONTINUE_SEARCH;
	}
	if (!IsWindowVisible(hwnd))return CONTINUE_SEARCH;//极域2015和2016版本的广播窗口不是单一的，
	wchar_t title[MAX_STR];//正常看到的全屏窗口里面还有TDDesk Render Window(广播内容窗口)和工具栏窗口。
	GetWindowText(hwnd, title, MAX_STR);//这里枚举极域广播窗口的子窗口，找到TDDesk Render Window，记录下来，
	if (mywcsstr(title, L"TDDesk Render Window") != 0)//然后捕捉到CatchWnd里面去
	{
		SetParent(hwnd, CatchWnd);
		++EatList[0];//注意HWND类型的变量值一定都是4的倍数
		EatList[(size_t)(EatList[0]) / 4] = hwnd;//例如HWND a=0;++a;得到的值为4
		TDhWndChild = hwnd;
		return STOP_SEARCH;
	}
	return CONTINUE_SEARCH;
}
BOOL CALLBACK EnumAllBroadcastwnds(HWND hwnd, LPARAM lParam)//查找极域"屏幕广播"窗口的枚举函数(用于一键安装)
{
	if (!IsWindowVisible(hwnd))return CONTINUE_SEARCH;
	wchar_t title[MAX_STR];
	GetWindowText(hwnd, title, MAX_STR);
	if (mywcsstr(title, L"屏幕广播") != 0 || mywcsstr(title, L"屏幕演播室窗口") != 0 || mywcsstr(title, L"屏幕广播窗口") != 0)
	{
		EnumChildWindows(hwnd, EnumChildwnd, 1);
		return CONTINUE_SEARCH;
	}
	return CONTINUE_SEARCH;
}
BOOL CALLBACK EnumBroadcastwnd(HWND hwnd, LPARAM lParam)//查找"屏幕广播"窗口的枚举函数.
{
	if (!IsWindowVisible(hwnd))return CONTINUE_SEARCH;
	wchar_t title[MAX_STR];
	GetWindowText(hwnd, title, MAX_STR);
	if (mywcsstr(title, L"屏幕广播") != 0)//枚举极域广播窗口
	{//(2015、2016中极域广播窗口一般叫"屏幕广播"。但极域是多语言的，在英语系统上可能会出问题)
		EnumChildWindows(hwnd, EnumChildwnd, NULL);
		if (FS) { FS = FALSE; MyRegisterClass(hInst, ScreenProc, ScreenWindow, NULL); }
		TDhWndGrandParent = CreateWindow(ScreenWindow, L"You can't see me.", WS_POPUP, 0, 0, 100, 100, nullptr, nullptr, hInst, nullptr);
		SetParent(hwnd, TDhWndGrandParent);//子窗口被捕捉后，"屏幕广播"窗口也应该得到妥善处置。
		TDhWndParent = hwnd;//曾经试过DestroyWindow、ShowWindow(SW_HIDE)这些标准方法，但都没有效果
		return STOP_SEARCH;//这里创建一个看不见的窗口，把"屏幕广播"窗口捕捉进去
	}
	return  CONTINUE_SEARCH;
}
BOOL CALLBACK EnumBroadcastwndOld(HWND hwnd, LPARAM lParam)//查找被极域广播窗口的枚举函数.
{
	if (!IsWindowVisible(hwnd))return CONTINUE_SEARCH;
	wchar_t title[MAX_STR];//较老版本的极域里面没有TDDesk Render Window，极域广播窗口也不叫"屏幕广播"，所以需要专门处理
	GetWindowText(hwnd, title, MAX_STR);//这个函数用于2007、2010、2012版本的极域
	if (mywcsstr(title, L"屏幕演播室窗口") != 0 || mywcsstr(title, L"屏幕广播窗口") != 0)//屏幕演播室窗口->2010 or 2012版，屏幕广播窗口->2007版
	{
		TDhWndChild = hwnd; TDhWndParent = (HWND)(-2);
		SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_WINDOWEDGE | WS_EX_TOPMOST | WS_EX_LEFT | WS_EX_CLIENTEDGE);
		SetParent(hwnd, CatchWnd);//2010、2012版极域使用这个功能效果较好，
		++EatList[0];//但是2007版本不建议使用。因为2007版本的极域广播不支持自动缩放，
		EatList[(size_t)(EatList[0]) / 4] = hwnd;//窗口化后只能显示老师屏幕的一部分
		return STOP_SEARCH;
	}
	return CONTINUE_SEARCH;
}
BOOL CALLBACK EnumMonitoredwnd(HWND hwnd, LPARAM lParam)//查找被监视窗口的枚举函数.
{//这是一个很久以前写好的功能，意图通过窗口监视，复制极域广播内容到自己窗口上来实现"极域窗口化"
	ULONG nProcessID;//不过呢现在已经有了两个更好的方案，这一功能就没什么作用了。
	if (IsWindowVisible(hwnd))//可能会在下一个版本移除(还有下一个版本么= =)
	{
		RECT rc;
		GetWindowRect(hwnd, &rc);
		if (rc.right - rc.left < 15 || rc.bottom - rc.top < 15)return 1;//过小的窗口将不被监视
		::GetWindowThreadProcessId(hwnd, &nProcessID);//如果pid正确，把hWnd记录下来
		if (tdpid[nProcessID])
			MonitorList[++MonitorTot] = hwnd;
	}
	return CONTINUE_SEARCH;
}
#pragma warning(default:4100)//恢复警告

void FindMonitoredhWnd(wchar_t* ProcessName)//查找被监视的窗口.
{
	Map<int, BOOL>::Iterator it = tdpid.Begin();
	if (it != 0)
		while (it != tdpid.End()) {
			(*it).second = FALSE;
			++it;
		}
	ProcessName[3] = 0;
	mywcslwr(ProcessName);
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = 0;
		mywcslwr(pe.szExeFile);//记录下符合要求的pid
		if (mywcsstr(pe.szExeFile, ProcessName) != 0)tdpid[pe.th32ProcessID] = TRUE;
	}
	if (!EnumWindows(EnumMonitoredwnd, NULL))error();
}

BOOL CALLBACK EnumFullScreenWnd(HWND hwnd, LPARAM lParam)//杀掉全屏窗口的枚举函数
{//枚举置顶窗口
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID; HANDLE hProcessHandle = 0;
	RECT rc; GetWindowRect(hwnd, &rc);
	::GetWindowThreadProcessId(hwnd, &nProcessID);//寻找全屏窗口
	if (rc.left == 0 && rc.top == 0 && rc.bottom == yLength && rc.right == xLength)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		if (GetCurrentProcessId() == nProcessID || expid[nProcessID] == TRUE)goto skipped;//如果是被zi保ji护de的进程ID -> 跳过
		hProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, nProcessID);//现在结束进程全部改用不带Nt的函数
		TerminateProcess(hProcessHandle, 1);//(反正，，我是没遇到NtTerminateProc起作用过,只见到他崩溃过)
	}
skipped:
	return 1;
}
bool KillFullScreen()//杀掉全屏窗口
{
	Map<int, BOOL>::Iterator it = expid.Begin();//清空map
	if (it != 0)while (it != expid.End()) (*it).second = FALSE, ++it;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = 0;
		if (mywcsstr(pe.szExeFile, L"exp") != 0)expid[pe.th32ProcessID] = TRUE;
	}//将Explorer的Pid加入保护名单中
	if (!EnumWindows(EnumFullScreenWnd, NULL)) { error(); return false; }
	return true;
}

#pragma warning(disable:28182)
#pragma warning(disable:28183)
BOOL KillProcess(LPCWSTR ProcessName)//根据进程名结束进程.
{
	if (*ProcessName == 0)return FALSE;
	wchar_t* MyProcessName = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * MAX_STR * 4), TempStr[MAX_STR * 4], * Pointer1, * Pointer2, DriverPath[MAX_PATH];

	if (Main.Check[CHK_FMACH].Value == TRUE)
	{//完全匹配进程名
		mywcscpy(MyProcessName, ProcessName);
		mywcslwr(MyProcessName);
		if (mywcsstr(MyProcessName, L".exe") == 0)mywcscat(MyProcessName, L".exe");
	}
	else
	{
		mywcscpy(TempStr, ProcessName);
		Pointer1 = Pointer2 = TempStr;
		while (mywcsstr(Pointer1, L"|") != 0)
		{
			Pointer2 = mywcsstr(Pointer1, L"|");
			Pointer2[0] = 0;//处理用"|"分隔的不同进程名
			Pointer1[3] = 0;//将每个"|"前的字符串转为小写并只保留前三个字母
			mywcscat(MyProcessName, Pointer1);
			mywcscat(MyProcessName, L"|");
			Pointer1 = Pointer2 + 1;
		}
		Pointer1[3] = 0;
		mywcscat(MyProcessName, Pointer1);
		mywcslwr(MyProcessName);
	}

	BOOL ReturnValue = FALSE, NoProcess = TRUE;
	HANDLE PhKphHandle = 0;
	BOOL ConnectSuccess = FALSE;//尝试连接KProcessHacker
	if (KphConnect(&PhKphHandle) >= 0)ConnectSuccess = TRUE;
	if (ConnectSuccess && Bit == 34)
	{//32位TDT在64位系统上用KProcessHacker结束进程时需要用Win64KPHcaller
		mywcscpy(DriverPath, TDTempPath);
		mywcscat(DriverPath, L"Win64KPHcaller.exe");
		ReleaseRes(DriverPath, FILE_CALLER, L"JPG");
	}
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;//创建进程快照
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;
	while (Process32Next(hSnapShot, &pe))
	{
		if (Main.Check[CHK_FMACH].Value == FALSE)pe.szExeFile[3] = 0;//根据进程名前三个字符标识
		mywcslwr(pe.szExeFile);
		if (mywcsstr(MyProcessName, pe.szExeFile) != 0 || ProcessName == NULL)
		{
			NoProcess = FALSE;
			HANDLE hProcess = 0;
			if (Main.Check[CHK_KPH].Value && ConnectSuccess)
			{//连接成功->用驱动结束进程
				if (Bit != 34)
				{
#pragma warning(disable:4312)//64位下这里会报HANDLE大于DWORD的警告
					PhOpenProcess(&hProcess, 1, (HANDLE)pe.th32ProcessID, PhKphHandle);
#pragma warning(default:4312)
					ReturnValue |= PhTerminateProcess(hProcess, 1, PhKphHandle) >= 0;
				}
				else
				{
					wchar_t cmdline[MAX_PATH], PIDStr[MAX_NUM];
					myZeroMemory(PIDStr, sizeof(wchar_t) * MAX_NUM);
					mywcscpy(cmdline, DriverPath);//使用Win64KPHcaller
					myitow(pe.th32ProcessID, PIDStr, MAX_NUM);
					mywcscat(cmdline, L" ");
					mywcscat(cmdline, PIDStr);
					ReturnValue |= RunEXE(cmdline, NULL, nullptr);
				}
			}
			else
			{
				hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
				ReturnValue |= TerminateProcess(hProcess, 1);//否则使用普通的OpenProcess和TerminateProcess
			}
			if (hProcess)CloseHandle(hProcess);
		}
	}
	if (ConnectSuccess && Bit == 34)DeleteFile(DriverPath);//删除Win64KPHcaller

	HeapFree(GetProcessHeap(), 0, MyProcessName);
	return ReturnValue || NoProcess;//如果找到了进程，但没结束成功，返回FALSE
}
#pragma warning(default:28182)
#pragma warning(default:28183)

BOOL GetTDVer(wchar_t* source)//获取极域版本.
{//返回值复制到source里
	if (source == NULL)return FALSE;//连source都没有当然直接退出
	mywcscpy(source, Main.GetStr(L"_TTDv"));//在前面加上"极域版本："之类的字符串
	HKEY hKey;
	LONG ret;//标准的注册表操作
	wchar_t szLocation[100];
	myZeroMemory(szLocation, sizeof(wchar_t) * 100);
	DWORD dwSize = sizeof(wchar_t) * 100;
	DWORD dwType = REG_SZ;
	if (Bit != 64)//64位时要手动重定向到WOW6432Node里
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-Learning Class V6.0", 0, KEY_READ, &hKey);
	else//据说现在极域好像也有64位了怎么办
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-Learning Class V6.0", 0, KEY_READ, &hKey);

	if (ret != 0)//打开失败
	{
		if (Bit != 64)//再看看上级目录存不存在
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain", 0, KEY_READ, &hKey);
		else//(因为很旧版的极域没有v6.0这个东西，完全无法获取版本号)
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain", 0, KEY_READ, &hKey);
		if (ret != 0)//连TopDomain目录都没有，极域可能不存在
			mywcscat(source, Main.GetStr(L"TTDunk"));
		else//极域版本为2007之类，给个模棱两可的答案
			mywcscat(source, Main.GetStr(L"TTDold"));
		RegCloseKey(hKey);
		return TRUE;
	}//打开成功，至少有这个目录
	ret = RegQueryValueExW(hKey, L"Version", 0, &dwType, (LPBYTE)&szLocation, &dwSize);
	if (ret != 0)return FALSE;//打开成功却没有键值? -> 找不到
	if (*szLocation != 0)mywcscat(source, szLocation);//找到 ->正常返回版本号
	else return FALSE;//键值为空?? -> 找不到
	RegCloseKey(hKey);//关闭句柄
	return TRUE;
}

void TDSearchDirect()
{//直接调用SearchTool寻找studentmain.exe的函数
	SearchTool(L"C:\\Program Files\\Mythware", 1);
	SearchTool(L"C:\\Program Files\\TopDomain", 1);//先试着在专用目录里找
	SearchTool(L"C:\\Program Files (x86)\\Mythware", 1);
	SearchTool(L"C:\\Program Files (x86)\\TopDomain", 1);
	if (*TDPath != NULL)return;
	SearchTool(L"C:\\Program Files (x86)", 1);
	if (*TDPath != NULL)return;//再试着在整个Program Files里找
	SearchTool(L"C:\\Program Files", 1);
	if (*TDPath != NULL)return;
	SearchTool(L"C:\\Users", 1);
	if (*TDPath != NULL)return;//再试着在ProgramData，AppData这些目录里找
	SearchTool(L"C:\\ProgramData", 1);
}
void SetTDPathStr()//更改并自动重绘第四页中"极域路径"这个字符串
{
	if (!TDsearched)return;
	wchar_t TDPathStr[MAX_PATH];
	myZeroMemory(TDPathStr, sizeof(wchar_t) * MAX_PATH);
	mywcscpy(TDPathStr, Main.GetStr(L"_TPath"));
	if (*TDPath == 0)//找不到极域时显示TPunk字符串
		mywcscat(TDPathStr, Main.GetStr(L"TPunk"));
	else
		mywcscat(TDPathStr, TDPath);
	if (mywcslen(TDPathStr) > 45)mywcscpy(TDPathStr + 44, L"...");
	Main.SetStr(TDPathStr, L"TPath");
	if (Main.CurWnd != 4)return;//不在"关于"页面的时候就不用刷新了
	RECT rc{ (int)(170 * Main.DPI), (int)(365 * Main.DPI),(int)(DEFAULT_WIDTH * Main.DPI),(int)(390 * Main.DPI) };
	Main.es[++Main.es[0].top] = rc;
	Main.Readd(REDRAW_TEXT, 24);//增减text时记得更改"24"这个数据
	Main.Redraw(rc);
}
DWORD WINAPI ReopenThread2(LPVOID pM)//尝试找到极域(线程)
{
	(pM);
	if (!TDsearched) TDSearchDirect(), TDsearched = TRUE;
	SetTDPathStr();
	return 0;
}
void UpdateInfo()//修改"关于"界面信息的函数.
{
	wchar_t tmp[MAX_PATH], tmp2[MAX_PATH]; int f;

	myZeroMemory(tmp, sizeof(wchar_t) * MAX_PATH);

	mywcscpy(tmp2, Main.GetStr(L"Tbit"));//系统位数
	if (Bit == 32)mywcscat(tmp2, L"32"); else mywcscat(tmp2, L"64");
	Main.SetStr(tmp2, L"Tbit");
	GetOSDisplayString(tmp);//系统版本
	mywcscpy(tmp2, Main.GetStr(L"Twinver")); mywcscat(tmp2, tmp);
	Main.SetStr(tmp2, L"Twinver");
	if (Bit == 34)f = GetFileAttributes(L"C:\\windows\\sysnative\\cmd.exe"); else f = GetFileAttributes(L"C:\\windows\\system32\\cmd.exe");
	mywcscpy(tmp2, Main.GetStr(L"Tcmd"));//是否有cmd
	if (f != -1)mywcscat(tmp2, Main.GetStr(L"TcmdOK")); else mywcscat(tmp2, Main.GetStr(L"TcmdNO"));

	Main.SetStr(tmp2, L"Tcmd");
	mywcscpy(tmp2, Main.GetStr(L"TIP"));
	CheckIP(tmp2);//ip地址
	Main.SetStr(tmp2, L"TIP");//极域版本
	if (GetTDVer(tmp2))Main.SetStr(tmp2, L"TTDv");
	CreateThread(0, 0, ReopenThread2, 0, 0, 0);//寻找极域路径
}
void RefreshFrameText()//根据是否有管理员权限的两种情况改变Frame上的文字.
{
	mywcscat(Main.Frame[FRA_PASSWORD].Name, Main.GetStr(L"nRec"));
	const int ok[]{ FRA_WINDOW,FRA_CHANNEL }, useless[]{ FRA_PROCESS, FRA_DELETER,FRA_OTHERS };
	for (int i = 0; i < 2; ++i)Main.Frame[ok[i]].rgb = COLOR_OK, mywcscat(Main.Frame[ok[i]].Name, Main.GetStr(L"Usable"));

	if (!Admin)
		for (int i = 0; i < 3; ++i)Main.Frame[useless[i]].rgb = COLOR_RED, mywcscat(Main.Frame[useless[i]].Name, Main.GetStr(L"Useless"));
	else
	{
		Main.Frame[FRA_PROCESS].rgb = COLOR_NOTREC;
		mywcscat(Main.Frame[FRA_PROCESS].Name, Main.GetStr(L"nRec"));
	}
	Main.Frame[FRA_GAMES].rgb = COLOR_GREEN;
}
void GetPath()//得到程序路径 & ( 程序路径 + 程序名 ).
{
	GetModuleFileName(NULL, Path, MAX_PATH);//直接获取程序名
	mywcscpy(Name, Path);
	for (int i = (int)mywcslen(Path) - 1; i >= 0; --i)
		if (Path[i] == L'\\') {//把程序名字符串中最后一个"\\"后面的字符去掉就是路径
			Path[i + 1] = 0; return;
		}
}

void GetBit()//系统位数.
{
	SYSTEM_INFO si;//较有效的检测系统位数的方法
	MyGetInfo(&si);//AMD64 \ Intel64 ?
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) Bit = 64; else Bit = 32;
	//32 means 32bit Program on 32bit System
	//34 means 32bit Program on 64bit System
	//64 means 64bit Program on 64bit System
#ifndef _WIN64
	if (Bit == 64)Bit = 34;//检测程序自身的位数
#endif
}
BOOL BackupSethc()//备份sethc.exe.
{
	if (GetFileAttributes(TDTempPath) == FILE_ATTRIBUTE_DIRECTORY)return FALSE;//临时文件目录已经存在 -> sethc可能已经备份过 -> 不是第一次运行
	CreateDirectory(TDTempPath, NULL);//创建一个临时文件夹(说是"临时"，其实一般不会自动删除)
	if (GetFileAttributes(SethcPath) == INVALID_FILE_ATTRIBUTES) { SethcState = FALSE; return FALSE; }//sethc本来就不存在 -> 不是第一次运行
	return CopyFile(SethcPath, L"C:\\SAtemp\\sethc", TRUE);//因为大多数机子上是有还原卡的嘛
}

bool EnableTADeleter()
{//尝试和DeleteFile驱动通信.
	if (DeleteFileHandle != 0)return TRUE;//已经和驱动连接上了 -> 退出
	wchar_t DriverPath[MAX_PATH];
	mywcscpy(DriverPath, TDTempPath);
	mywcscat(DriverPath, L"DeleteFile.sys");
	if (Bit == 32)//释放驱动文件到程序目录里
		ReleaseRes(DriverPath, FILE_TAX32, L"JPG");
	else
		ReleaseRes(DriverPath, FILE_TAX64, L"JPG");
	UnloadNTDriver(L"DeleteFile");//加载驱动
	LoadNTDriver(L"DeleteFile", DriverPath, TRUE);
	EnablePrivilege(SE_DEBUG_NAME, 0);//获取用于通讯的DEBUG权限?

	OBJECT_ATTRIBUTES objectAttributes;
	HMODULE hModule = ::GetModuleHandle(L"ntdll.dll");
	if (hModule == NULL)return FALSE;//寻找ntdll
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

BOOL EnableKPH()
{//加载KProcessHacker驱动(但不取得通信).
	if (Main.Check[CHK_KPH].Value)return TRUE;//如果已经加载，就不用再加载了
	wchar_t DriverPath[MAX_PATH];
	mywcscpy(DriverPath, TDTempPath);
	mywcscat(DriverPath, L"kprocesshacker.sys");
	if (Bit == 32)
		ReleaseRes(DriverPath, FILE_KPH32, L"JPG");
	else
		ReleaseRes(DriverPath, FILE_KPH64, L"JPG");
	UnloadNTDriver(L"KProcessHacker2");
	EnablePrivilege(SE_DEBUG_NAME, 0);
	return LoadNTDriver(L"KProcessHacker2", DriverPath, TRUE);
}

void UpdateCatchedWindows()
{//刷新主界面上"已捕捉 X 个窗口"这个字符串
	wchar_t tmpstr[MAX_PATH], tmpnum[MAX_NUM];
	myZeroMemory(tmpstr, sizeof(wchar_t) * MAX_PATH);
	myZeroMemory(tmpnum, sizeof(wchar_t) * MAX_NUM);
	myitow((int)((size_t)EatList[0] / 4), tmpnum, MAX_NUM);
	mywcscpy(tmpstr, Main.GetStr(L"Eat1"));
	mywcscat(tmpstr, tmpnum);
	mywcscat(tmpstr, Main.GetStr(L"Eat2"));
	Main.SetStr(tmpstr, L"_Eat");
	if (Main.CurWnd != 1)return;
	Main.Readd(REDRAW_TEXT, 5);
	RECT rc{ (long)(195 * Main.DPI),(long)(480 * Main.DPI),(long)(400 * Main.DPI),(long)(505 * Main.DPI) };
	Main.es[++Main.es[0].top] = rc;
	Main.Redraw(rc);
	if ((int)((size_t)EatList[0] / 4 == 0))Main.EnableButton(BUT_RELS, FALSE); else Main.EnableButton(BUT_RELS, TRUE);
}

/*  语言切换  */

void SwitchLanguage(LPWSTR FilePath)//改变语言的函数
{
	//这个函数在用在其他工程时不能直接照抄，
	//因为涉及到不同窗体和需要特殊处理的控件的问题

	//BOOL Mainf = FALSE;//本工程中只用到了一个窗口，如果要使用多个窗口，可以借助<Main>之类的标签来区分

	int type = 0; //标识控件类型的变量
	DWORD NumberOfBytesRead;
	const bool ANSI = mywcsstr(FilePath, L"ANSI");//为了节省空间，英语类语言文件用ANSI编码存储

	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	if (!hHeap)return;
	wchar_t* AllTmp = (wchar_t*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(wchar_t) * MAX_LANGUAGE_LENGTH), * point1, * point2;
	char* ANSIstr = (char*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(char) * MAX_LANGUAGE_LENGTH * 2);//申请堆内存
	if (AllTmp == 0 || ANSIstr == 0)return;

	HANDLE FileHandle = CreateFile(FilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (FileHandle == 0)return;//打开文件
	if (!ReadFile(FileHandle, ANSIstr, MAX_LANGUAGE_LENGTH * 2, &NumberOfBytesRead, NULL))return;
	if (ANSI) { for (unsigned int i = 0; i < NumberOfBytesRead; ++i)AllTmp[i] = ANSIstr[i]; }
	else MultiByteToWideChar(CP_ACP, 0, ANSIstr, -1, AllTmp, MAX_LANGUAGE_LENGTH);//对ANSI编码存储的文件进行特殊处理
	if (NumberOfBytesRead == 0)return;

	point1 = AllTmp;
	while (1)
	{
		point2 = mywcsstr(point1, L"\n");
		if (point2 == 0)break;
		*point2 = 0;
		//if (mywcsstr(point1, L"<Main>") != 0)Mainf = TRUE;//用<>来区分不同窗体
		//if (mywcsstr(point1, L"</Main>") != 0)Mainf = FALSE;//看起来有点像xml代码
		if (mywcsstr(point1, L"<Buttons>") != 0)type = 1;
		if (mywcsstr(point1, L"<Checks>") != 0)type = 2;
		if (mywcsstr(point1, L"<Strings>") != 0)type = 3;
		if (mywcsstr(point1, L"<Frames>") != 0)type = 4;
		if (mywcsstr(point1, L"<Texts>") != 0)type = 5;
		if (mywcsstr(point1, L"<Edits>") != 0)type = 6;
		if (point1[0] != L'<')
		{
			/*if (Mainf)*/Main.DispatchLanguage(point1, type);//分配(各个)窗体的语言
		}
		point1 = point2 + 1;
	}
	CloseHandle(FileHandle);//读取完文件后一定要记得关闭

	if (InfoChecked)UpdateInfo();//修改"关于"界面信息
	SetWindowText(Main.hWnd, Main.GetStr(L"Tmain2"));//更新标题
	if (CatchWnd != NULL)SetWindowText(CatchWnd, Main.GetStr(L"Title2"));
	RefreshFrameText();//Frame文字

	Main.EnableButton(BUT_MORE, !(bool)mywcsstr(FilePath, L"English"));//几个文字会改变的按钮
	if (OneClick)mywcscpy(Main.Button[BUT_ONEK].Name, Main.GetStr(L"unQs"));
	if (GameMode == 1)mywcscpy(Main.Button[BUT_GAMES].Name, Main.GetStr(L"Gamee"));
	if (SethcInstallState)mywcscpy(Main.Button[BUT_SETHC].Name, Main.GetStr(L"unSet"));
	if (Main.Button[BUT_SHUTD].Enabled == FALSE)mywcscpy(Main.Button[BUT_SHUTD].Name, Main.GetStr(L"Deleted"));
	if (mywcsstr(FilePath, L"English"))//"不是管理员"文字的范围
		Main.Area[4].Left = 230, Main.Area[4].Width = 110; else Main.Area[4].Left = 176, Main.Area[4].Width = 85;

	UpdateCatchedWindows();//更新被捕窗口的个数
	Main.Redraw();
	if (CatchWnd)InvalidateRect(CatchWnd, NULL, FALSE), UpdateWindow(CatchWnd);
	HeapDestroy(hHeap);//释放内存
}

BOOL SetupSethc()//安装sethc
{
	wchar_t mySethcPath[MAX_PATH];
	mywcscpy(mySethcPath, TDTempPath);
	mywcscat(mySethcPath, L"mysethc.exe");
	if (GetFileAttributes(mySethcPath) == -1)ReleaseRes(mySethcPath, FILE_SETHC, L"JPG");//文件不存在的话从资源里释放
	if (GetFileAttributes(mySethcPath) == -1)return 2;//释放之后文件还是不存在??
	return CopyFile(mySethcPath, SethcPath, FALSE);//复制成功 or 失败
}

BOOL AutoSetupSethc()//自动安装sethc的外壳函数.
{
	const int Flag = SetupSethc();
	if (Flag == 0) { Main.InfoBox(L"CSFail"); return FALSE; }//复制失败
	if (Flag == 2) { Main.InfoBox(L"NoSethc"); return FALSE; }//找不到文件
	Main.InfoBox(L"suc");
	return TRUE;
}

BOOL MyDeleteFile(LPCWSTR FilePath)
{//自动使用驱动 or 应用层删除一个文件.
	if (Main.Check[CHK_T_A_].Value == 1)
	{
		if (!DeleteFileHandle)return FALSE;//驱动未连接上则退出
		DWORD ReturnValue[2];//useless value
		wchar_t DriverPath[MAX_PATH], * Pointer1 = DriverPath, ** Pointer2 = &Pointer1;//注意传进去的是指向指针的指针
		mywcscpy(DriverPath, L"\\??\\");/*路径前要加上 "\??\" 这一字符串		*/
		mywcscat(DriverPath, FilePath);
		DeviceIoControl(DeleteFileHandle, CTL_CODE(0x22, 0x360, 0, 0), Pointer2, \
			sizeof(Pointer2), ReturnValue, sizeof(ReturnValue), &ReturnValue[1], nullptr);//传进去的CTL_CODE(控制码)中的第二项function一般是0x800~0x2000
		return TRUE;//然而这里很不标准，使用的是0x360 (0x0~0x799都保留给系统，不应给普通应用程序使用)
	}
	else return DeleteFile(FilePath);

}

void GetDeviceRect()
{
	HDC hdcScreen = GetDC(NULL);
	xLength = GetDeviceCaps(hdcScreen, HORZRES);
	yLength = GetDeviceCaps(hdcScreen, VERTRES);
	DeleteObject(hdcScreen);
}
void GetRealCommandLine(wchar_t* cmdl)
{
	//*cmdl = 0;
	wchar_t* t = GetCommandLine(), * a, * b;
	//s(t);
	if (t == 0)return;
	a = mywcsstr(t, L" ");
	if (a == 0)  return;
	b = mywcsstr(t, L"\"");
	if (b == 0 || b > a) { mywcscpy(cmdl, a + 1); return; }
	else
	{
		b = mywcsstr(b + 1, L"\"");
		if (b == 0)return;
		mywcscpy(cmdl, b + 1);
	}
}
BOOL DeleteSethc()//删除sethc(删不掉的话自动使用驱动).
{
	if (GetFileAttributes(SethcPath) == -1)return TRUE;//sethc已经没了 -> 成功
	TakeOwner(SethcPath);//取得所有权
	const wchar_t XPsethcPath[] = L"C:\\Windows\\system32\\dllcache\\sethc.exe";//删除xp中sethc备份文件
	TakeOwner(XPsethcPath);
	DeleteFile(XPsethcPath);
	if (DeleteFile(SethcPath))return TRUE;//已经删掉了 -> 退出
	else
	{
		if (Admin == FALSE)return FALSE;//删不掉，又没有管理员权限 -> 失败退出
		EnableTADeleter();
		Main.Check[CHK_T_A_].Value = 1;//加载驱动
		MyDeleteFile(XPsethcPath);
		return MyDeleteFile(L"C:\\Windows\\system32\\sethc.exe");//返回是否成功
	}
}

BOOL SearchTool(LPCWSTR lpPath, int type)//1:打开极域 2:删除shutdown 3:删除文件夹.
{//有多个功能的函数，所以叫它"Tool"
	wchar_t szFind[MAX_PATH], szFile[MAX_PATH];//三个功能的搜索代码大同小异，所以就把它们合并起来了
	WIN32_FIND_DATA FindFileData;//注:这两个数值不需要清零
	mywcscpy(szFind, lpPath);
	mywcscat(szFind, L"\\*.*");
	HANDLE hFind = FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) return TRUE;
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{//发现是一个文件夹
			if (!(FindFileData.cFileName[0] == '.' && (mywcslen(FindFileData.cFileName) == 1 || (mywcslen(FindFileData.cFileName) == 2 && FindFileData.cFileName[1] == '.'))))
			{//不是开头带"."的回退文件夹
				mywcscpy(szFile, lpPath);
				mywcscat(szFile, L"\\");
				mywcscat(szFile, FindFileData.cFileName);
				if (!SearchTool(szFile, type))return FALSE;//递归查找
				if (type == 3)RemoveDirectory(szFile);
			}//删除完这个文件夹内的文件后删除这个空文件夹
		}
		else
		{//发现是一个文件
			mywcslwr(FindFileData.cFileName);
			if (type == 1) {
				if (mywcsstr(FindFileData.cFileName, L"studentmain.exe") != NULL)
				{//查找并运行studentmain
					mywcscpy(TDName, FindFileData.cFileName);
					mywcscpy(szFile, lpPath);
					mywcscat(szFile, L"\\");
					mywcscat(szFile, FindFileData.cFileName);
					mywcscpy(TDPath, szFile);
					return FALSE;
				}
			}
			if (type == 2)//删除shutdown
				if (mywcsstr(FindFileData.cFileName, L"shutdown.exe") != NULL)
				{
					mywcscpy(szFile, lpPath);
					mywcscat(szFile, L"\\");
					mywcscat(szFile, FindFileData.cFileName);
					TakeOwner(szFile);
					DeleteFile(szFile);
				}
			if (type == 3)
			{//递归删除一个文件夹
				mywcscpy(szFile, lpPath);
				mywcscat(szFile, L"\\");
				mywcscat(szFile, FindFileData.cFileName);
				TakeOwner(szFile);//取得这个文件的所有权
				MyDeleteFile(szFile);
				RemoveDirectory(szFile);
			}
		}
		if (!FindNextFile(hFind, &FindFileData))break;//没有下一个文件了
	}
	FindClose(hFind);//关闭句柄
	return TRUE;
}
void AutoDelete(const wchar_t* FilePath, BOOL sli)//自动删除文件.
{
	const int FileType = GetFileAttributes(FilePath);
	if (FileType == -1)
	{//不是文件也不是文件夹?是否强制删除?
		if (!sli)
		{
			wchar_t tmpStr[MAX_PATH];
			mywcscpy(tmpStr, L"\"");
			mywcscat(tmpStr, FilePath);
			mywcscat(tmpStr, L"\"");
			mywcscat(tmpStr, Main.GetStr(L"TINotF"));
			if (MessageBox(Main.hWnd, tmpStr, Main.GetStr(L"Info"), MB_YESNO | MB_ICONQUESTION) == 6)goto forcedelete;
		}
		return;
	}
forcedelete:
	TakeOwner(FilePath);

	if (FileType & FILE_ATTRIBUTE_DIRECTORY)
	{
		wchar_t DirectoryName[MAX_PATH];//是文件夹
		mywcscpy(DirectoryName, FilePath);//SHFileOperation要求字符串最后两位都是NULL,
		SearchTool(DirectoryName, 3);//在尝试删除多个文件时，
		MyRemoveDirectory(DirectoryName);//直接在原字符串上操作一定会出问题
	}
	else
		MyDeleteFile(FilePath);//是文件，直接删除
}

BOOL UninstallSethc()//恢复原来的sethc.
{//不是常用的函数，但（又是历史原因）还是被保留下来
	DeleteFile(SethcPath);//这时sethc被删除过一次，应该已拥有其所有权
	if (!CopyFile(L"C:\\SAtemp\\sethc", SethcPath, FALSE)) { Main.InfoBox(L"USFail"); return FALSE; }//恢复sethc失败?
	else { Main.InfoBox(L"suc"); SethcState = TRUE; return TRUE; }
}

void RegMouseKey()//注册键盘控制鼠标的热键.
{
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_LEFTKEY, MOD_ALT, 188);//左键
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_RIGHTKEY, MOD_ALT, 190);//右键
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_MOVELEFT, MOD_ALT, VK_LEFT);//左移
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_MOVEUP, MOD_ALT, VK_UP);//上移
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_MOVERIGHT, MOD_ALT, VK_RIGHT);//右移
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_MOVEDOWN, MOD_ALT, VK_DOWN);//下移
}
__forceinline void UnMouseKey() { for (int i = MAIN_HOTKEY_LEFTKEY; i <= MAIN_HOTKEY_MOVEDOWN; ++i)AutoUnregisterHotKey(Main.hWnd, i); }//注销热键.

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(MouseHook, nCode, wParam, lParam); }//空的全局钩子函数
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(KeyboardHook, nCode, wParam, lParam); }//防止极域钩住这些

DWORD WINAPI DeleteThread(LPVOID pM)
{//删除文件(夹)的线程.
	(pM);//防止删除一个超大文件夹时主界面卡顿过久
	//try
	{
		const int len = mywcslen(Main.Edit[EDIT_FILEVIEW].str);
		if (len < 5)return 0;
		int prv = 0;
		//s(sizeof(wchar_t) * len);
		HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
		if (!hHeap)return 0;
		wchar_t* Tempstr = (wchar_t*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(wchar_t) * len);
		if (Tempstr == 0)return 0;
		mywcscpy(Tempstr, Main.Edit[EDIT_FILEVIEW].str);
		mywcscpy(Main.Button[BUT_DELETE].Name, Main.GetStr(L"deleting"));
		Main.EnableButton(BUT_DELETE, FALSE);
		//s(Tempstr);
		if (mywcsstr(Tempstr, L"|") == 0)AutoDelete(Tempstr, FALSE);
		else
		{

			for (int i = 0; i < len - 1; ++i)
				if (Tempstr[i] == L'|')
				{//解析每一个字符串
					Tempstr[i] = 0;
					//s(&Tempstr[prv]);
					AutoDelete(&Tempstr[prv], FALSE);
					prv = i + 1;
				}
		}
		mywcscpy(Main.Button[BUT_DELETE].Name, Main.GetStr(L"deleted"));
		Main.EnableButton(BUT_DELETE, TRUE);
		HeapDestroy(hHeap);//据说可以不用HeapFree
		//GlobalUnlock(hg);
		//GlobalFree(hg);

		return 0;
	}

}

DWORD WINAPI GameThread(LPVOID pM)
{
	UNREFERENCED_PARAMETER(pM);
	if (Main.Width < 700)
	{//展开窗口
		GameButtonLock = TRUE;//自制线程锁
		for (int j = 20; j <= 240; j += GAMINGSPEED)
		{
			Main.Width += GAMINGSPEED;
			::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((DEFAULT_WIDTH + j) * Main.DPI), (int)(Main.Height * Main.DPI - 0.5), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
			RECT Rc{ (long)((511) * Main.DPI) ,0,(long)(Main.Width * Main.DPI) ,(long)(Main.Height * Main.DPI - 0.5) };
			Main.Button[BUT_CLOSE].Left += GAMINGSPEED;//右移"关闭按钮"
			Main.es[++Main.es[0].top] = Rc;
			Main.Redraw(Rc);//重绘展开部分
			if (UTState)UTrc.right += (int)(GAMINGSPEED * Main.DPI);
			Sleep(1);
		}
		if (UTState)UTrc.right -= (int)(GAMINGSPEED * Main.DPI);
		Main.Width = DEFAULT_WIDTH + 240;
	}
	else
	{//缩回游戏部分
		//基本上就是反过来
		for (int j = 20; j <= 240; j += GAMINGSPEED)
		{
			::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((Main.Width - j) * Main.DPI), (int)(Main.Height * Main.DPI - 0.5), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
			RECT Rc{ (long)((511) * Main.DPI) ,0,(long)(Main.Width * Main.DPI) ,(long)(Main.Height * Main.DPI - 0.5) };
			Main.Button[BUT_CLOSE].Left -= GAMINGSPEED;
			Main.Redraw(Rc);
			if (UTState)UTrc.right -= (int)(GAMINGSPEED * Main.DPI);
			Sleep(1);
		}
		Main.Width = DEFAULT_WIDTH;
		if (UTState)UTrc.right = UTrc.left + (int)(DEFAULT_WIDTH * Main.DPI);
	}
	GameButtonLock = FALSE;//关闭线程锁
	return 0;
}
BOOL DownloadGames(const wchar_t* url, const wchar_t* file, DownloadProgress* p, int ButID, int tot, int cur, int maxs)
{//下载(游戏)
	wchar_t Fp[501], URL[121];
	mywcscpy(Fp, L"C:\\SAtemp\\Games\\");
	mywcscat(Fp, url);//拼接下载目录和源目录
	mywcscpy(URL, GameURLprefix);
	mywcscat(URL, file);
	mywcscat(URL, L"qk.txt");
	if (ButID != NULL)
	{
		Main.Button[ButID].Download = 1;
		Main.Button[ButID].DownTot = tot;
		Main.Button[ButID].DownCur = cur;
		p->curi = ButID;
		p->factmax = maxs;
	}
	return URLDownloadToFileW(NULL, URL, Fp, 0, p) == S_OK;
}
DWORD WINAPI DownloadThread(LPVOID pM)//分发下载(游戏)任务的线程.
{
	int cur = *(int*)pM;
	BOOL f = FALSE;
	DownloadProgress progress;
	progress.curi = progress.factmax = 0;
	switch (cur)
	{
	case 1:
		f = DownloadGames(GameName[0], GameURLsuffix[0], &progress, BUT_GAME1, 2, 1, 0);//1号小游戏有2个文件要特殊处理
		DownloadGames(GameName[6], GameURLsuffix[6], &progress, BUT_GAME1, 2, 2, 899000);//14000词库.ini
		break;//	由于某些原因无法得到它的文件大小，要在2这里设置
	case 2:case 3:case 4:case 5:case 6:
		//下载第2~6个游戏
		f = DownloadGames(GameName[cur - 1], GameURLsuffix[cur - 1], &progress, BUT_GAME1 + cur - 1, 1, 1, 0);
		break;
	case 7:
	{//ARP attack
		BOOL WPinstalled = TRUE;
		const wchar_t WPPath[] = L"C:\\SAtemp\\WinPcap.exe";
		progress.curi = BUT_ARP;
		if (GetFileAttributes(L"C:\\Windows\\System32\\wpcap.dll") == -1)//WinPcap未安装
		{
			if (!Main.YesNoBox(L"WPAsk"))return 0;
			//是否同意下载一个?
			WPinstalled = FALSE;
			Main.Button[BUT_ARP].Download = Main.Button[BUT_ARP].DownCur = 1;
			Main.Button[BUT_ARP].DownTot = 2;
			if (URLDownloadToFileW(NULL, L"http://download.skycn.com/hao123-soft-online-bcs/soft/W/WinPcap_4.1.3.exe", WPPath, 0, &progress) == S_OK)RunWithAdmin((LPWSTR)WPPath);
			else return 0;//下载后以管理员身份运行
		}
		Main.Button[BUT_ARP].Download = 1;
		Main.Button[BUT_ARP].DownTot = Main.Button[BUT_ARP].DownCur = 2 - (int)WPinstalled;//下载arp.exe(不自动运行)
		//s(1);
		if (URLDownloadToFileW(NULL, L"http://www.zlian.ga/u/1599378482qk.txt", L"C:\\SAtemp\\arp.exe", 0, &progress) != S_OK)return 0;
		break;
	}
	default:return 0;
	}
	GameExist[cur - 1] = f;
	return 0;
}

bool LanguageSearched = false;
void SearchLanguageFiles()//在当前目录里寻找语言文件
{
	if (LanguageSearched)return;
	LanguageSearched = true;

	SendMessage(FileList, LB_RESETCONTENT, 0, 0);//在寻找新文件前清空listbox
	wchar_t szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	mywcscpy(szFind, TDTempPath);
	mywcscat(szFind, L"language\\*.ini");

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
	if (FakeThreadLock)  return 0;//自制线程锁
	FakeThreadLock = TRUE;
	const int offset = *(int*)pM;
	RECT rc;
	GetWindowRect(FakeWnd, &rc);
	for (int i = 1; i <= 30; ++i)
	{
		Sleep(10);//伸缩动画
		SetWindowPos(FakeWnd, 0, rc.left, rc.top + i * 2 * offset, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	if (!FakeToolbarNew)SetWindowPos(FakeWnd, 0, rc.left, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	FakeThreadLock = FALSE;//关闭线程锁
	return 0;
}

BOOL CALLBACK CatchThread(HWND hwnd, LPARAM lParam)//捕捉窗口.
{
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID;
	if (GetParent(hwnd) == 0)
	{//如果是顶级窗口(没有父窗口的窗口)
		::GetWindowThreadProcessId(hwnd, &nProcessID);//窗口在被捕名单上且可见
		if (Eatpid[nProcessID] == TRUE && IsWindowVisible(hwnd))
		{
			RECT rc, rc2;
			GetWindowRect(hwnd, &rc);
			if (rc.bottom<0 || rc.right<0 || rc.left> xLength || rc.top>yLength)return 1;//不在屏幕范围内的窗口将不被捕捉
			if (rc.right - rc.left < 15 || rc.bottom - rc.top < 15)return 1;//过小的窗口将不被捕捉
			if (SetParent(hwnd, CatchWnd) != NULL)
			{
				++EatList[0];//捕捉并将hWnd加入到EatList中
				EatList[((size_t)EatList[0]) / 4] = hwnd;
				GetClientRect(CatchWnd, &rc2);
				GetWindowRect(hwnd, &rc);
				if (rc.top >= rc2.bottom || rc.left >= rc2.right)SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOSIZE);
			}
		}
	}
	return 1;
}

void ULTRATopMost()//开启"超级置顶"
{//下面大致介绍一下"超级置顶"这一功能
	UTState = TRUE;//"超级置顶"简称UT,是一项基于(类似于DirectUI的技术)的技术
	Main.noMsgbox = TRUE;//"超级置顶"本身根本就不是一个窗口。它直接将画好的图像打印在屏幕DC上。
	Deskwnd = GetDesktopWindow();//这样绘制的图像就能"绕过"窗口管理系统，直接悬浮在最上面。
	GetWindowRect(Main.hWnd, &UTrc);//但是，正常的窗口依靠窗口管理系统接受消息，如WM_MOUSEMOVE等，
	Main.CreateFrame(180, 410, 264, 113, 5, L"");//UT放弃了窗口后，只能通过计时器获取鼠标位置，
	SetTimer(Main.hWnd, TIMER_UT1, 1, (TIMERPROC)TimerProc);//通过全局键盘钩子获取输入的信息，
	SetTimer(Main.hWnd, TIMER_UT2, 15, (TIMERPROC)TimerProc);//操作起来十分麻烦
	LButtonDown = KEY_DOWN(VK_LBUTTON);//同时，没有了窗口，也就没有了子窗口的概念。一般VC上的控件，
	SetLayeredWindowAttributes(Main.hWnd, NULL, 1, LWA_ALPHA);
	Main.Redraw();//如Button,Edit等普通控件都不能使用。这是其主要的局限性。
}

void noULTRA()//关闭UT
{//UT开启的时候浮在最上面，看不到弹出的MessageBox,因此Class里的InfoBox暂时不能使用
	UTState = FALSE; Main.noMsgbox = FALSE;
	--Main.CurFrame;//正如上面所说，UT开启时无法显示List,所以我们在这里显示一个占位用的Frame
	KillTimer(Main.hWnd, TIMER_UT1);//关闭计时器
	KillTimer(Main.hWnd, TIMER_UT2);
	SetLayeredWindowAttributes(Main.hWnd, NULL, 230, LWA_ALPHA);
	SetWindowPos(Main.hWnd, 0, UTrc.left, UTrc.top, UTrc.right - UTrc.left, UTrc.bottom - UTrc.top, SWP_NOREDRAW | SWP_NOZORDER);
	Main.Redraw();
}

void ReturnWindows()//归还窗口.
{
	if (TDhWndChild != 0 && TDhWndParent != 0 && TDhWndParent != (HWND)-1)
	{//当CatchWnd内抓了一个极域广播窗口时，归还窗口要做特殊处理(停掉计时器等)
		KillTimer(Main.hWnd, TIMER_CATCHEDTD);
		SetParent(TDhWndChild, NULL);
		SetWindowPos(TDhWndChild, NULL, 0, 0, xLength, yLength, SWP_NOZORDER);
		TDhWndParent = TDhWndChild = 0;
	}
	for (size_t i = 1; i <= (size_t)(EatList[0]); ++i)SetParent(EatList[i], NULL);
	EatList[0] = 0;
	UpdateCatchedWindows();
}
DWORD WINAPI TopThread(LPVOID pM)//置顶线程.
{
	UNREFERENCED_PARAMETER(pM);
	while (TOP == 1)//循环会直接占用一个CPU线程，在较差的电脑建议打开"低画质"
	{//连续置顶	(比不过任务管理器，人家有CreateWindowWithBand)
		if (HideState) { Sleep(10); continue; }
		SetWindowPos(Main.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
		if (LowResource)Sleep(1);
		if (CatchWnd != NULL) {//同时开启Main和CatchWnd并将两窗口部分重叠在一起时，重叠部分会出现不断闪烁的现象。
			if ((TopCount % 50) == 0)SetWindowPos(CatchWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);//这一特性暂时无解，
			++TopCount;//这里故意降低CatchWnd的置顶速度来缓解此"特性"~
		}
	}//退出线程前前取消置顶
	SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	if (CatchWnd != NULL)SetWindowPos(CatchWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	return 0;
}

BOOL CatchWindows()//经过延时后正式开始捕捉窗口
{
	wchar_t ExeName[MAX_PATH];
	myZeroMemory(ExeName, sizeof(wchar_t) * MAX_PATH);
	for (int i = 0; i < 3; ++i)if (Main.Edit[EDIT_PROCNAME].str[i] != 0)ExeName[i] = Main.Edit[EDIT_PROCNAME].str[i]; else break;
	mywcslwr(ExeName);
	Map<int, BOOL>::Iterator it = Eatpid.Begin();//清空map
	if (it != 0)while (it != Eatpid.End())(*it).second = FALSE, ++it;
	PROCESSENTRY32 pe;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = 0;//把要捕捉的程序的pid加进去
		mywcslwr(pe.szExeFile);
		if (mywcsstr(ExeName, pe.szExeFile) != 0)Eatpid[pe.th32ProcessID] = TRUE;
	}
	EnumWindows(CatchThread, NULL);
	UpdateCatchedWindows();
	return TRUE;
}
void MyExitProcess()
{//退出前进行清理.
	if (Admin)
	{
		UnloadNTDriver(L"KProcessHacker2");//卸载驱动
		UnloadNTDriver(L"DeleteFile");
	}
	if (UTState)InvalidateRect(Deskwnd, &UTrc, TRUE);
	if (EatList[0] != 0)ReturnWindows();//归还窗口
	if (MouseHook != NULL)UnhookWindowsHookEx(MouseHook);//关闭全局键盘钩子
	if (KeyboardHook != NULL)UnhookWindowsHookEx(KeyboardHook);
	ExitProcess(0);
}

BOOL RefreshTDstate()//刷新极域的状态
{
	const RECT rc{ (LONG)(165 * Main.DPI), (LONG)(400 * Main.DPI),(LONG)(320 * Main.DPI),(LONG)(505 * Main.DPI) };
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//记录着极域进程工具frame位置的rc
	wchar_t myTDName[MAX_PATH];
	myZeroMemory(myTDName, sizeof(wchar_t) * MAX_PATH);
	for (int i = 0; i < 3; ++i)myTDName[i] = TDName[i];
	mywcslwr(myTDName);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;

	while (Process32Next(hSnapShot, &pe))
	{//寻找极域进程
		pe.szExeFile[3] = 0;
		mywcslwr(pe.szExeFile);
		if (mywcscmp(myTDName, pe.szExeFile) == 0)
		{//极域被启动了
			wchar_t tmpStr[MAX_STR], numStr[MAX_NUM];
			if (TDPID != 0)return TRUE;//如果已经知道StudentMain.exe的pid则退出
			TDPID = pe.th32ProcessID;//否则设置stu的pid
			myZeroMemory(numStr, sizeof(wchar_t) * MAX_NUM);
			mywcscpy(tmpStr, Main.GetStr(L"_TDPID"));
			myitow(TDPID, numStr, 10);
			mywcscat(tmpStr, numStr);
			Main.SetStr(tmpStr, L"TDPID");//增减Text时注意更改"10"这个值
			mywcscpy(Main.Text[10].Name, L"TcmdOK");
			Main.Text[10].rgb = COLOR_GREENEST;//设置一些按钮的状态，然后重绘
			Main.Button[BUT_KILLTD].Enabled = TRUE;
			Main.Button[BUT_RETD].Enabled = FALSE;
			Main.Readd(REDRAW_TEXT, 8); Main.Readd(REDRAW_TEXT, 9); Main.Readd(REDRAW_TEXT, 10);
			Main.Readd(REDRAW_BUTTON, BUT_KILLTD);
			Main.Readd(REDRAW_BUTTON, BUT_RETD);
			Main.es[++Main.es[0].top] = rc;
			Main.Redraw(rc);
			return TRUE;
		}
	}
	if (TDPID == 0)return TRUE;
	TDPID = 0;//极域原来存在，但现在被结束了
	wchar_t tmpStr[MAX_STR];
	mywcscpy(tmpStr, Main.GetStr(L"_TDPID"));
	mywcscat(tmpStr, L"\\");
	Main.SetStr(tmpStr, L"TDPID");
	mywcscpy(Main.Text[10].Name, L"TcmdNO");
	Main.Text[10].rgb = COLOR_RED;//设置一些按钮的状态，然后重绘
	Main.Button[BUT_RETD].Enabled = TRUE;
	Main.Button[BUT_KILLTD].Enabled = FALSE;
	Main.Readd(REDRAW_TEXT, 8); Main.Readd(REDRAW_TEXT, 9); Main.Readd(REDRAW_TEXT, 10);
	Main.Readd(REDRAW_BUTTON, BUT_KILLTD);
	Main.Readd(REDRAW_BUTTON, BUT_RETD);
	Main.es[++Main.es[0].top] = rc;
	Main.Redraw(rc);
	return TRUE;
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
				KillTimer(hWnd, TIMER_KILLPROCESS);//结束进程 (全部) 失败则退出
				const RECT rc(Main.GetRECTc(CHK_REKILL));
				Main.Readd(REDRAW_CHECK, CHK_REKILL);
				Main.Redraw(rc);
				error();
			}
		break;
	}
	case TIMER_CATCHWINDOW://延时捕捉窗口
	{
		if (--CatchWndTimerLeft > 0)
		{
			if (!IsWindowVisible(CatchWnd))
			{
				CatchWndTimerLeft = -1;
				mywcscpy(Main.Button[BUT_CATCH].Name, Main.GetStr(L"back"));
				Main.EnableButton(BUT_CATCH, TRUE);
				KillTimer(Main.hWnd, TIMER_CATCHWINDOW);
			}
			wchar_t TempString[MAX_STR], TempNumber[MAX_NUM];
			myZeroMemory(TempNumber, sizeof(wchar_t) * MAX_NUM);
			mywcscpy(TempString, Main.GetStr(L"Timer1"));
			myitow(CatchWndTimerLeft, TempNumber, MAX_NUM);
			mywcscat(TempString, TempNumber);
			mywcscat(TempString, Main.GetStr(L"Timer2"));
			mywcscpy(Main.Button[BUT_CATCH].Name, TempString);//拼接按钮中"剩余XX秒"的文字
			Main.EnableButton(BUT_CATCH, FALSE);
		}
		if (CatchWndTimerLeft <= 0)
		{
			CatchWindows();
			KillTimer(Main.hWnd, TIMER_CATCHWINDOW);
			mywcscpy(Main.Button[BUT_CATCH].Name, Main.GetStr(L"back"));
			Main.EnableButton(BUT_CATCH, TRUE);
		}
		break;
	}
	case TIMER_COPYLEFT://copyleft
	{
		if (Main.CurWnd != 4)EasterEggFlag = false, KillTimer(Main.hWnd, TIMER_COPYLEFT);

		EasterEggState = (EasterEggState + 1) % 55;
		wchar_t CopyLeftstr[MAX_STR], Tempstr[10];//设置文字滚动
		mywcscpy(CopyLeftstr, L"Copy");
		myZeroMemory(Tempstr, sizeof(wchar_t) * 10);
		MultiByteToWideChar(CP_ACP, 0, EasterEggStr[(EasterEggState % 55) / 5], -1, Tempstr, 10);
		mywcscat(CopyLeftstr, Tempstr);
		mywcscat(CopyLeftstr, Main.GetStr(L"Tleft"));
		Main.SetStr(CopyLeftstr, L"_Tleft");

		int RGB[3]{ GetRValue(Main.TitleBar.col),GetGValue(Main.TitleBar.col),GetBValue(Main.TitleBar.col) }, a = ColorChangingState < 0 ? -1 : 1;

		RGB[ColorChangingState * a - 1] += (4 + myrand() % 3) * a;
		for (int i = 0; i < 3; ++i)
		{
			if (RGB[i] < 10)RGB[i] = 10, ColorChangingState = i + 1;
			if (RGB[i] >= 245)RGB[i] = 245, ColorChangingState = -i - 1;
		}
		--ColorChangingLeft;
		if (myrand() % 25 == 0 && ColorChangingLeft <= 0)
		{
			ColorChangingLeft = 8;
			int t2 = ColorChangingState;
		useless:
			ColorChangingState = myrand() % 7 - 3;
			if (ColorChangingState == 0 || ColorChangingState == t2 || ColorChangingState == -t2)goto useless;
		}
		if (RGB[0] + RGB[1] + RGB[2] <= 300)Main.Text[29].rgb = COLOR_WHITE;
		if (RGB[0] + RGB[1] + RGB[2] >= 450)Main.Text[29].rgb = COLOR_BLACK;//颜色较浅时字体为黑色，反之亦然

		Main.SetTitleBar(RGB(RGB[0], RGB[1], RGB[2]), TITLEBAR_HEIGHT);
		Main.Redraw();
		break;
	}
	case TIMER_EXPLAINATION://定时创建exp
	{
		if (!--sdl)
		{
			typedef struct tagCHANGEFILTERSTRUCT {//使程序接受非管理员程序(explorer)的拖拽请求
				DWORD cbSize;
				DWORD ExtStatus;
			} CHANGEFILTERSTRUCT, * PCHANGEFILTERSTRUCT;
			typedef BOOL(WINAPI* CHANGEWINEOWMESSAGEFILTEREX)(HWND hwnd, UINT message, DWORD action, PCHANGEFILTERSTRUCT pChangeFilterStruct);
			CHANGEWINEOWMESSAGEFILTEREX ChangeWindowMessageFilterEx = NULL;
			HMODULE hModule = GetModuleHandle(L"user32.dll");//这是win7以后才有的新函数
			if (hModule != NULL)ChangeWindowMessageFilterEx = (CHANGEWINEOWMESSAGEFILTEREX)GetProcAddress(hModule, "ChangeWindowMessageFilterEx");
			if (ChangeWindowMessageFilterEx == NULL)break;
			ChangeWindowMessageFilterEx(Main.hWnd, WM_DROPFILES, 1, 0);
			ChangeWindowMessageFilterEx(Main.hWnd, 0x0049, 1, NULL);

			if (GetSystemDefaultLangID() == 0x0804 && Main.Button[BUT_MORE].Enabled)//中文系统上自动"切换语言"来显示注释
			{
				wchar_t Tempstr[MAX_STR];
				ReleaseLanguageFiles(TDTempPath, 1, Tempstr);
				SwitchLanguage(Tempstr);
			}
		}
		if (GetForegroundWindow() != Main.hWnd)Main.EditUnHotKey();
		if (GetTickCount() - Main.Timer >= 500 && Main.ExpExist == FALSE)Main.Try2CreateExp();
		break;
	}
	case TIMER_BUTTONEFFECT://按钮特效
	{
		if (!Main.ButtonEffect)break;
		for (int i = 0; i <= Main.CurButton; ++i)
		{
			if (Main.CoverButton != i && Main.Button[i].Percent > 0)
			{//不在鼠标上的按钮颜色逐渐变淡
				Main.Button[i].Percent -= 2 * Delta;
				if (Main.Button[i].Percent < 0)Main.Button[i].Percent = 0;
				Main.ButtonRedraw(i);
			}
		}
		if (Main.CoverButton != -1 && Main.Button[Main.CoverButton].Percent < 100)
		{//在鼠标上的按钮颜色以双倍速度变深
			Main.Button[Main.CoverButton].Percent += 3 * Delta;
			if (Main.Button[Main.CoverButton].Percent > 100)Main.Button[Main.CoverButton].Percent = 100;
			Main.ButtonRedraw(Main.CoverButton);
		}
		break;
	}
	case TIMER_UPDATECATCH:
		InvalidateRect(CatchWnd, NULL, FALSE);
		UpdateWindow(CatchWnd);
		break;
	case TIMER_ANTIHOOK://刷新鼠标键盘钩子
		if (Main.Check[CHK_NOHOOK].Value == 1)
		{//只有最后加入的钩子才有效，这样极域就没法用钩子来禁用键盘了
			MouseHook = (HHOOK)SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)MouseProc, hInst, 0);
			KeyboardHook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hInst, 0);
		}
		break;
	case TIMER_REFRESH_TD://刷新极域状态
		if (Main.CurWnd == 2)RefreshTDstate();
		break;//250ms刷新一次，经测试效果不错
	case TIMER_UPDATEBSOD://刷新伪装蓝屏窗口上的文字
	{
		BSODstate += 3;
		InvalidateRect(BSODhwnd, NULL, FALSE), UpdateWindow(BSODhwnd);
		DeskDC = GetDC(0);
		if (Main.Check[CHK_OLDBSOD].Value)
			StretchBlt(DeskDC, 0, 0, xLength, yLength, bhdc, 0, 0, 640, 480, SRCCOPY);
		else
			BitBlt(DeskDC, 0, 0, xLength, yLength, bhdc, 0, 0, SRCCOPY);
		ReleaseDC(Deskwnd, DeskDC);
		break;
	}
	case TIMER_TOOLBAR:
		if (GetTickCount() - FakeTimer > 2200)
		{//鼠标离开约3s后收回伪装工具条(新)
			RECT rc;
			GetWindowRect(FakeWnd, &rc);
			if (rc.bottom == 63 && FakeThreadLock == FALSE && FakenewUp == FALSE)
			{
				FakeTimer = GetTickCount();
				FakenewUp = TRUE;
				int typ = -1;//这个计时器只用于创建线程
				CreateThread(NULL, 0, FakeNewThread, &typ, 0, NULL);
				Sleep(1);
			}
		}
		break;
	case TIMER_CATCHEDTD://更新被捕极域窗口的大小
	{//("自动捕捉极域"后他的窗口在CatchWnd内，因此窗口大小就应该随着CatchWnd的大小变动)
		RECT rc;
		GetClientRect(CatchWnd, &rc);
		SetWindowPos(TDhWndChild, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOREDRAW);
		if (!IsWindow(TDhWndChild))
		{
			KillTimer(Main.hWnd, TIMER_CATCHEDTD);
			if (TDhWndGrandParent != NULL)DestroyWindow(TDhWndGrandParent), TDhWndGrandParent = 0;//当极域广播结束时，自动关闭CatchWnd
			if ((size_t)EatList[0] < 8) { ReturnWindows(), DestroyWindow(CatchWnd); CatchWnd = 0; }
			TDhWndChild = TDhWndParent = 0;
		}
		break;
	}
	case TIMER_UT1:
	{//对于直接贴图到桌面DC的"窗口"需要不断重新绘制，不然会被其他窗口覆盖
		SendMessage(hWnd, WM_PAINT, 0, UT_MESSAGE); break; //重绘的频率越高越好
	}//一般显示器刷新率为60fps，但实际上，将计时器至少调到120fps才有不错的效果
	case TIMER_UT2:
	{//用于获得鼠标信息的计时器
		SetWindowPos(Main.hWnd, HWND_TOPMOST, UTrc.left, UTrc.top, UTrc.right - UTrc.left, UTrc.bottom - UTrc.top, SWP_NOREDRAW);
		//UT本身不是一个窗口，因此在UT上点击会直接"穿透"过去，将点击信号同时发送给下一层的正常窗口
		const BOOL LBnew = KEY_DOWN(VK_LBUTTON);
		BOOL CSinside = FALSE;//为了避免这一现象，这里将Main变成了正常置顶，刚好处于UT正下方来"阻挡"住点击信号。
		POINT point;//唯一的问题是，当UT悬浮在"强力"的窗口置顶(如任务管理器)上时，Main无法保持在UT的下方，这时会出现点击穿透的bug
		GetCursorPos(&point);
		if (point.x > UTrc.left && point.y > UTrc.top && point.x < UTrc.right && point.y < UTrc.bottom)
			Main.MouseMove(), CSinside = TRUE;
		if (LButtonDown != LBnew && CSinside)
		{//判断点击信号是LBUTTONDOWN还是LBUTTONUP
			LButtonDown = LBnew;//然后正常将信号发送给WndProc，之后的处理和平常一样
			if (LBnew) WndProc(Main.hWnd, WM_LBUTTONDOWN, 0, UT_MESSAGE); else  WndProc(Main.hWnd, WM_LBUTTONUP, 0, UT_MESSAGE);
		}
		break;
	}
	case TIMER_UT3:
	{//拖动UT"窗口"时启用的计时器
		const RECT rcback = UTrc;
		POINT point;
		GetCursorPos(&point);
		UTrc.left = point.x - UTMpoint.x;
		UTrc.top = point.y - UTMpoint.y;
		UTrc.right = point.x - UTMpoint.x + rcback.right - rcback.left;
		UTrc.bottom = point.y - UTMpoint.y + rcback.bottom - rcback.top;
		SetWindowPos(Main.hWnd, 0, UTrc.left, UTrc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		if (!KEY_DOWN(VK_LBUTTON))KillTimer(Main.hWnd, TIMER_UT3);
		break;
	}
	case TIMER_ONEKEY: {EnumWindows(EnumAllBroadcastwnds, NULL); break; }//"一键安装"用的计时器，定时启用极域的所有按钮
	}
}
bool FirstSD = true;
DWORD WINAPI SDThread(LPVOID pM)//切换桌面的线程
{//(据说)SetThreadDesktop要求调用线程没有创建任何窗口，因此这里需要创建一个新的线程~
	(pM);
	if (FirstSD && FirstFlag)
	{//未备份 -> 程序可能在这台电脑上第一次启动 -> 虚拟桌面中运行explorer
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		myZeroMemory(&si, sizeof(STARTUPINFO));
		myZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		si.cb = sizeof(si);
		si.lpDesktop = szVDesk;
		CreateProcess(NULL, ExplorerPath, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		FirstSD = false;
	}
	HDESK vdback;
	if (!NewDesktop)//从原始桌面切换到新桌面
		vdback = OpenDesktopW(szVDesk, DF_ALLOWOTHERACCOUNTHOOK, FALSE, GENERIC_ALL);

	else//切换回来
		vdback = OpenDesktopW(L"Default", DF_ALLOWOTHERACCOUNTHOOK, FALSE, GENERIC_ALL);
	NewDesktop = !NewDesktop;
	SetThreadDesktop(vdback);
	SwitchDesktop(vdback);
	return 0;
}
void openfile()//显示"打开文件"的对话框
{
	OPENFILENAME ofn;
	wchar_t strFile[MAX_PATH];
	myZeroMemory(&ofn, sizeof(OPENFILENAME));
	myZeroMemory(strFile, sizeof(wchar_t) * MAX_PATH);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = (LPWSTR)strFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn) == TRUE)Main.SetEditStrOrFont(strFile, 0, EDIT_FILEVIEW);
	Main.EditRedraw(EDIT_FILEVIEW);//将结果设置到Edit中
}
void BrowseFolder()//显示"打开文件夹"的对话框
{
	wchar_t TempPath[MAX_PATH];
	BROWSEINFO bi;
	myZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.lpszTitle = Main.GetStr(L"OpenFolder");//标题
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		SHGetPathFromIDList(pidl, TempPath);
		SetCurrentDirectory(TempPath);//设置"当前工作目录"为选择的文件夹的上个目录
		Main.SetEditStrOrFont(TempPath, 0, EDIT_FILEVIEW);
		Main.EditRedraw(EDIT_FILEVIEW);//将结果设置到Edit中
	}
}
void ClearUp()//清理文件并退出
{
	ShowWindow(Main.hWnd, SW_HIDE);//先隐藏窗口，使清理速度"看起来"变快
	if (Admin)if (EnableTADeleter())Main.Check[CHK_T_A_].Value = TRUE;
	DeleteFile(SethcPath);//恢复sethc
	CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE);
	AutoDelete(L"C:\\SAtemp\\deletefile.sys", TRUE);
	AutoDelete(TDTempPath, TRUE);//删除整个缓存文件夹
	if (Main.Check[CHK_T_A_].Value)AutoDelete(Name, TRUE);
	MyExitProcess();
}
void ShutdownDeleter()//删除所有的Shutdown.exe
{
	SearchTool(L"C:\\Program Files\\Mythware", 2);
	SearchTool(L"C:\\Program Files\\TopDomain", 2);
	SearchTool(L"C:\\Windows\\System32", 2);//仍然是各个目录寻找
	SearchTool(L"C:\\Program Files (x86)\\Mythware", 2);
	SearchTool(L"C:\\Program Files (x86)\\TopDomain", 2);
	SearchTool(L"C:\\Windows\\SysNative", 2);//System目录删除时可能有点慢
}
DWORD WINAPI ShutdownDeleterThread(LPVOID pM)//调用ShutdownDeleter的线程
{
	(pM);
	Main.EnableButton(BUT_SHUTD, FALSE);
	ShutdownDeleter();
	if (Admin)
	{
		mywcscpy(Main.Button[BUT_SHUTD].Name, Main.GetStr(L"Deleted"));
		Main.ButtonRedraw(BUT_SHUTD);
	}
	else Main.EnableButton(BUT_SHUTD, TRUE);
	return 0;
}
void AutoTerminateTD()//自动结束极域电子教室
{
	if (!KillProcess(TDName) && Admin)
		if (EnableKPH())
		{//结束失败后尝试用KPH
			Main.Check[CHK_KPH].Value = TRUE;
			KillProcess(TDName);
		}
}
void ReopenTD()//在已知路径的情况下，重新打开极域电子教室
{
	if (GetFileAttributes(TDPath) != -1)
	{
		if (Main.Check[CHK_TDINVD].Value == 1)
		{
			STARTUPINFO si;
			myZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(si);
			si.lpDesktop = szVDesk;//可能要在虚拟桌面里运行
			if (!RunEXE(TDPath, NULL, &si))Main.InfoBox(L"StartFail");
		}
		else if (!RunEXE(TDPath, NULL, nullptr))Main.InfoBox(L"StartFail");
	}
}
DWORD WINAPI SizingThread(LPVOID pM)//尝试寻找并打开极域
{
	(pM);
	if (Main.CurWnd == 5)ShowWindow(FileList, SW_HIDE);
	UTState = true;
	while (1)
	{
		POINT point;
		GetCursorPos(&point);
		RECT rc;
		GetWindowRect(Main.hWnd, &rc);
		point.x -= rc.left; point.y -= rc.top;
		float newDPI = min((float)point.x / (float)Main.Width, (float)point.y / (float)Main.Height);
		if (newDPI < 0.3)newDPI = (float)0.3;
		if (newDPI > 2)newDPI = (float)2;
		if (!KEY_DOWN(VK_LBUTTON))
		{
			UTState = false;
			Main.SetDPI(newDPI);
			SizingLock = false;
			if (Main.CurWnd == 5)
			{
				ShowWindow(FileList, SW_SHOW);
				SetWindowPos(FileList, 0, (int)(180 * Main.DPI), (int)(410 * Main.DPI), (int)(265 * Main.DPI), (int)(120 * Main.DPI), 0);
				::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);
			}
			return 0;
		}
		SetWindowPos(Main.hWnd, NULL, 0, 0, (int)(Main.Width * newDPI - 0.5), (int)(Main.Height * newDPI - 0.5), SWP_NOMOVE | SWP_NOREDRAW);
		Main.tdc = GetDC(Main.hWnd);
		SetStretchBltMode(Main.tdc, HALFTONE);
		StretchBlt(Main.tdc, 0, 0, (int)(Main.Width * newDPI - 0.5), (int)(Main.Height * newDPI - 0.5), Main.hdc, 0, 0, (int)(Main.Width * Main.DPI - 0.5), (int)(Main.Height * Main.DPI - 0.5), SRCCOPY);
		ReleaseDC(Main.hWnd, Main.tdc);
		Sleep(10);
	}
	return 0;
}

DWORD WINAPI ReopenThread(LPVOID pM)//尝试寻找并打开极域
{
	(pM);
	if (Main.Button[BUT_RETD].Enabled == FALSE)return 0;
	if (!TDsearched)
	{
		wchar_t tmpstr[MAX_PATH];
		myZeroMemory(tmpstr, sizeof(wchar_t) * MAX_STR);
		TDsearched = TRUE;
		if (!slient)
		{
			mywcscpy(tmpstr, Main.Button[BUT_RETD].Name);
			mywcscpy(Main.Button[BUT_RETD].Name, Main.GetStr(L"starting"));
			Main.EnableButton(BUT_RETD, FALSE);
		}
		//各种目录都找一遍就行了
		TDSearchDirect();
		if (!slient)
		{
			mywcscpy(Main.Button[BUT_RETD].Name, tmpstr);
			Main.EnableButton(BUT_RETD, TRUE);
		}
		SetTDPathStr();
	}
	ReopenTD();
	if (slient)MyExitProcess();
	return 0;
}

void BSOD(int type)//尝试伪装蓝屏
{//0=auto;1=new;2=old
	LockCursor();//锁住鼠标
	//默认使用伪装蓝屏 + NtShutdown

	if (UTState)noULTRA();//防止Main的窗口阻挡BSOD
	ShowWindow(Main.hWnd, SW_HIDE);

	if (type == 0)
	{
		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;
		myZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);//检查一次版本号，小于8000则应是win7或更旧的系统
		if (bOsVersionInfoEx && osvi.dwBuildNumber < 8000)Main.Check[CHK_OLDBSOD].Value = TRUE;//此时默认用旧版蓝屏
	}
	else Main.Check[CHK_OLDBSOD].Value = type - 1;
	Deskwnd = GetDesktopWindow();
	if (FB) { FB = FALSE; MyRegisterClass(hInst, BSODProc, BSODWindow, NULL); }//初始化
	BSODhwnd = CreateWindow(BSODWindow, L"BSOD wnd", WS_POPUP, 0, 0, xLength, yLength, nullptr, nullptr, hInst, nullptr);
	BSODstate = -1;//创建窗口
	SetTimer(BSODhwnd, TIMER_UPDATEBSOD, 16, (TIMERPROC)TimerProc);
	ShowWindow(BSODhwnd, SW_SHOW);

	RestartDirect();//重启
}
VOID Restart()//瞬间重启
{
	if (UTState)noULTRA();
	ShowWindow(Main.hWnd, SW_HIDE);
	KillProcess(L"taskmgr.exe");//关闭可能浮在自己上面的任务管理器
	KillFullScreen();//关闭全屏的窗口
	RestartDirect();

	if (FB) { FB = FALSE; MyRegisterClass(hInst, BSODProc, BSODWindow, NULL); }//初始化
	BSODhwnd = CreateWindow(BSODWindow, L"BSOD", WS_POPUP, 0, 0, xLength, yLength, nullptr, nullptr, hInst, nullptr);
	Main.Check[CHK_OLDBSOD].Value = 2;
	ShowWindow(BSODhwnd, SW_SHOW);
	SetTimer(BSODhwnd, TIMER_UPDATEBSOD, 16, (TIMERPROC)TimerProc);
	LockCursor();
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
	wchar_t TempStr[MAX_STR];
	mywcscpy(TempStr, Main.GetStr(L"pswdis"));
	mywcscat(TempStr, L"\"");
	mywcscat(TempStr, str);
	mywcscat(TempStr, L"\"");
	if (slient || UTState) { Main.InfoBox(TempStr); return; }
	mywcscat(TempStr, Main.GetStr(L"Copypswd"));
	if (MessageBox(Main.hWnd, TempStr, Main.GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
	{
		const int len = mywcslen(str) * 2 + 1;
		HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
		if (!hHeap)return;
		char* ClipBoardStr = (char*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(char) * len);
		if (ClipBoardStr == 0)return;
		WideCharToMultiByte(CP_ACP, 0, str, -1, ClipBoardStr, len, NULL, NULL);
		if (OpenClipboard(Main.hWnd))
		{
			EmptyClipboard();
			HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(ClipBoardStr) + 1);
			if (!clipbuffer)return;
			char* buffer = (char*)GlobalLock(clipbuffer);
			if (buffer != NULL)strcpy(buffer, LPCSTR(ClipBoardStr));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
		HeapDestroy(hHeap);
	}
}
void AutoViewPass()//读取密码并显示
{
	HKEY hKey;//读取文件夹的handle
	LONG ret;//是否读取键值成功的状态
	wchar_t szLocation[MAX_STR];//接受读取内容的字符串
	DWORD dwSize = sizeof(wchar_t) * MAX_STR;//字符串的大小
	DWORD dwType = REG_SZ;//键值的种类(字符串)
	BYTE* a;
	BOOL error = FALSE;
	BYTE bits[MAX_STR];
	int start, cur = 0;//一些其他的临时变量
	DWORD dwType2 = REG_BINARY, dwSize2 = MAX_STR;
	wchar_t str[MAX_STR];
	myZeroMemory(str, sizeof(wchar_t) * MAX_STR);
	myZeroMemory(szLocation, sizeof(wchar_t) * MAX_STR);
#ifndef _WIN64
	myZeroMemory(bits, sizeof(BYTE) * MAX_STR);
#else
	* bits = 0;
#endif
	if (Bit != 64)//首先尝试读取未加密过的(如明文密码或前面加Passwd的明文密码)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	else//尝试打开键值
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	if (ret != 0 && slient == FALSE)//打不开文件夹
	{
		if (slient) { error = TRUE; goto finish; }//命令行调用时直接结束
		if (!Main.YesNoBox(L"VPFail"))goto finish;
	}

	ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType, (LPBYTE)&szLocation, &dwSize);//尝试读取键值
	if (ret != 0 && slient == FALSE) goto encrypted;//读取不了键值，尝试到knock中读取
	if (mywcslen(szLocation) != 0)//读取到了明文密码
	{
		if (mywcsstr(szLocation, L"Passwd") != 0)//可能是带passwd的密码
		{
			if (mywcsstr(szLocation, L"[123456]") != 0)goto encrypted;//密码中含有带方括号的123456，很可能是加密过的
			if (Bit != 64)//尝试"预读取"加密后的密码
				ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class\\Student", 0, KEY_QUERY_VALUE, &hKey);
			else
				ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student", 0, KEY_QUERY_VALUE, &hKey);
			if (RegQueryValueExW(hKey, L"Knock1", 0, &dwType2, (LPBYTE)bits, &dwSize2) == ERROR_SUCCESS ||
				RegQueryValueExW(hKey, L"Knock", 0, &dwType2, (LPBYTE)bits, &dwSize2) == ERROR_SUCCESS)goto encrypted;

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
	if (ret != 0 && slient == FALSE)//打不开文件夹
	{
		if (slient) { error = TRUE; goto finish; }//命令行调用时直接结束
		if (!Main.YesNoBox(L"VPFail"))goto finish;
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
		++cur;
	}
	if (mywcslen(str) == 0)Main.InfoBox(L"VPNULL"); else AutoPassBox(str);
	goto finish;

encrypted2:
	if (hKey != 0)RegCloseKey(hKey);
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	else//这里是2015年版的异或加密，保存在Key中
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	if (ret != 0 && slient == FALSE)//打不开文件夹
	{
		if (slient) { error = TRUE; goto finish; }//命令行调用时直接结束
		if (!Main.YesNoBox(L"VPFail"))goto finish;
	}

	ret = RegQueryValueExW(hKey, L"Key", 0, &dwType2, (LPBYTE)&bits, &dwSize2);//尝试读取键值
	if (ret != 0 && slient == FALSE)//打不开文件夹
	{
		if (slient) { error = TRUE; goto finish; }//命令行调用时直接结束
		if (!Main.YesNoBox(L"VPFail"))goto finish;
	}

	for (int i = 0; i < 150; ++i)
	{//从头到尾把密文和0x4350u异或
		if (bits[i * 2] == 0 && bits[i * 2 + 1] == 0)break;
		bits[i * 2] ^= 0x50;
		bits[i * 2 + 1] ^= 0x43;
	}
	myZeroMemory(str, sizeof(str));
	a = bits;
	while ((*a != 0) || (*(a + 1) != 0))
	{//把单个字节的数据拼接成字符串
		str[cur] = (*(a + 1) << 8) + (*(a));
		a += 2;
		++cur;
	}
	if (mywcslen(str) == 0)Main.InfoBox(L"VPNULL"); else AutoPassBox(str);
finish:
	if (error)Main.InfoBox(L"VPNULL");
	RegCloseKey(hKey);
	return;
}
void AutoChangePassword(wchar_t* a, int type)//自动更改密码
{
	wchar_t tmp[MAX_STR];
	myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
	HKEY hKey; LONG ret; DWORD dwType2 = REG_SZ, dwSize2 = MAX_STR, dwType = REG_BINARY;
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
	else//打开键值
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
	if (ret != 0) { Main.InfoBox(L"ACFail"); RegCloseKey(hKey); return; }//打开失败
	if (type == 1)
	{
		ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType2, (LPBYTE)tmp, &dwSize2);//尝试读取键值
		if (mywcsstr(tmp, L"Passwd[123456]") != 0 || ret != 0)goto nobasic;
	}
	myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
	if (type == 2)mywcscpy(tmp, L"Passwd"), mywcscat(tmp, a);//是否在之前加上Passwd
	else mywcscpy(tmp, a);
	ret = RegSetValueEx(hKey, L"UninstallPasswd", 0, REG_SZ, (const BYTE*)&tmp, sizeof(wchar_t) * (DWORD)mywcslen(tmp));
	if (ret != 0) { Main.InfoBox(L"ACUKE"); RegCloseKey(hKey); return; }//失败
nobasic:
	RegCloseKey(hKey);
	if (type == 1)//额外修改key和knock异或加密
	{
		mywcscpy(tmp, L"Passwd");//下面说说knock密码的加密方式
		BYTE data[2000];//这是2018年用IDA研究出来的，以后加密方式可能会变
		mywcscat(tmp, a);//具体就是，采用UNICODE编码，转化为二进制后
		const int len = (int)mywcslen(tmp) * 2;//把他们和0x4350u异或，得到的值前8位存在一个BYTE数组的偶数位，
		for (int i = 0; i < len; ++i)tmp[i] ^= 0x4350u;//后8位存在一个BYTE数组的奇数位
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
		if (Bit != 64)change(a, FALSE); else change(a, TRUE);//这里是2016版的异或加密，保存在Knock中
	}
	Main.InfoBox(L"ACOK");
	return;
}

void AutoCreateCatchWnd()
{
	if (FC)MyRegisterClass(hInst, CatchProc, CatchWindow, FALSE), FC = FALSE;//注册类
	if (CatchWnd != 0)
	{
		if (!IsWindowVisible(CatchWnd))ShowWindow(CatchWnd, SW_SHOW);
		return;
	}
	CatchWnd = CreateWindowW(CatchWindow, Main.GetStr(L"Title2"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, nullptr, nullptr, hInst, nullptr);
	ShowWindow(CatchWnd, SW_SHOW);
	return;
}

BOOL RunCmdLine(LPWSTR str)//解析启动时的命令行并执行
{
	BOOL console; DWORD pid;
	mywcslwr(str);//全部转化为小写字母
	for (unsigned int i = 0; i < mywcslen(str); ++i)if (str[i] == L'/')str[i] = L'-';
	if (GetSystemDefaultLangID() == 0x0409)//英文系统上自动切换语言
	{
		wchar_t Tempstr[MAX_STR];
		ReleaseLanguageFiles(TDTempPath, 2, Tempstr);
		SwitchLanguage(Tempstr);
	}

	if (mywcsstr(str, L"-top") != NULL)//显示在安全桌面上用
	{
		Main.Check[CHK_TOP].Value = 1;
		TOP = TRUE;
		CreateThread(NULL, 0, TopThread, NULL, 0, NULL);
		goto noreturn;//FALSE表示继续运行
	}
	pid = GetParentProcessID(GetCurrentProcessId());
	console = AttachConsole(pid);//附加到父进程命令行上
	hdlWrite = GetStdHandle(STD_OUTPUT_HANDLE);

	slient = TRUE;
	if (mywcsstr(str, L"-help") != NULL || mywcsstr(str, L"-?") != NULL)//显示帮助
	{
		wchar_t HelpPath[MAX_PATH];
		mywcscpy(HelpPath, TDTempPath);
		mywcscat(HelpPath, L"help.txt");
		ReleaseRes(HelpPath, FILE_HELP, L"JPG");
		DWORD NumberOfBytesRead;

		HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
		if (!hHeap)goto error;
		wchar_t* AllTmp = (wchar_t*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(wchar_t) * MAX_LANGUAGE_LENGTH), * Pointer1, * Pointer2;
		char* ANSItmp = (char*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(char) * MAX_LANGUAGE_LENGTH);
		if (AllTmp == 0 || ANSItmp == 0)goto error;
		HANDLE hf = CreateFile(HelpPath, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
		if (hf == 0)goto okreturn;
		if (!ReadFile(hf, ANSItmp, MAX_LANGUAGE_LENGTH, &NumberOfBytesRead, NULL))goto error;
		MultiByteToWideChar(CP_ACP, 0, ANSItmp, -1, AllTmp, MAX_LANGUAGE_LENGTH);
		Pointer1 = AllTmp;
		if (NumberOfBytesRead == 0)goto okreturn;
		for (int i = 1; i < HELP_END; ++i)
		{
			Pointer2 = mywcsstr(Pointer1, L"\n");
			if (Pointer2 == 0)break;
			Pointer2[0] = 0;
			if (i >= HELP_START)Main.InfoBox(Pointer1);
			Pointer1 = Pointer2 + 1;
		}
		CloseHandle(hf);
		HeapDestroy(hHeap);
		goto okreturn;
	}
	if (mywcsstr(str, L"-sethc") != NULL)//安装sethc
	{
		if (!DeleteSethc()) { Main.InfoBox(L"DSR3Fail"); goto okreturn; }
		AutoSetupSethc();
		goto okreturn;
	}
	if (mywcsstr(str, L"-enablebut") != NULL) { SetTimer(0, TIMER_ONEKEY, 1500, (TIMERPROC)TimerProc); Main.InfoBox(L"suc"); goto noreturn; }//一键安装(EnableButton)
	if (mywcsstr(str, L"-killtop") != NULL) { if (KillFullScreen()) Main.InfoBox(L"suc"); goto okreturn; } //自动结束置顶进程
	if (mywcsstr(str, L"-unsethc") != NULL) { UninstallSethc(); goto okreturn; }//卸载sethc
	if (mywcsstr(str, L"-vp") != NULL) { AutoViewPass(); goto okreturn; }//显示密码
	if (mywcsstr(str, L"-antishutdown") != NULL) { ShutdownDeleter(); Main.InfoBox(L"suc");  goto okreturn; }//删除shutdown
	if (mywcsstr(str, L"-retd") != NULL) { TDSearchDirect(); ReopenTD(); if (TDPath[0] != 0) Main.InfoBox(L"suc"); goto okreturn; }
	if (mywcsstr(str, L"-bsod") != NULL)
	{
		if (mywcscmp(str, L"old") != 0) { BSOD(2); goto noreturn; }
		if (mywcscmp(str, L"new") != 0) { BSOD(1); goto noreturn; }
		BSOD(0); goto noreturn;
	}
	if (mywcsstr(str, L"-reboot") != NULL) { Restart(); goto okreturn; }
	if (mywcsstr(str, L"-clear") != NULL)
	{
		wchar_t tmp[MAX_STR];
		myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
		AutoChangePassword(tmp, 1);
		goto okreturn;
	}
	if (mywcsstr(str, L"-rekill") != NULL)
	{
		wchar_t tmp[MAX_PATH], * tmp1 = mywcsstr(str, L"-rekill");
		if (!Findquotations(tmp1, tmp))goto error;
		while (1) { if (KillProcess(tmp))Main.InfoBox(L"suc"); else error();  Sleep(500); }
	}
	if (mywcsstr(str, L"-del") != NULL)
	{
		wchar_t tmp[MAX_PATH], * tmp1 = mywcsstr(str, L"-del");
		myZeroMemory(tmp, sizeof(wchar_t) * MAX_PATH);
		if (!Findquotations(tmp1, tmp))goto error;
		AutoDelete(tmp, TRUE);
		goto okreturn;
	}
	if (mywcsstr(str, L"-changepasswd") != NULL)//前后二者顺序不能调换
	{//因为changepasswd包括change这个字符串
		wchar_t tmp[MAX_PATH], * tmp1 = mywcsstr(str, L"-changewithpasswd");
		myZeroMemory(tmp, sizeof(wchar_t) * MAX_PATH);
		if (!Findquotations(tmp1, tmp))goto error;
		AutoChangePassword(tmp, 2);
		goto okreturn;
	}
	if (mywcsstr(str, L"-channel") != NULL)
	{
		wchar_t tmp[MAX_PATH], * tmp1 = mywcsstr(str, L"-channel");
		myZeroMemory(tmp, sizeof(wchar_t) * MAX_PATH);
		if (!Findquotations(tmp1, tmp))goto error;
		AutoChangeChannel(mywtoi(tmp));
		goto okreturn;
	}
	if (mywcsstr(str, L"-change") != NULL)
	{
		wchar_t tmp[MAX_PATH], * tmp1 = mywcsstr(str, L"-change");
		myZeroMemory(tmp, sizeof(wchar_t) * MAX_PATH);
		if (!Findquotations(tmp1, tmp))goto error;
		AutoChangePassword(tmp, 1);
		goto okreturn;
	}
error:
	SetConsoleTextAttribute(hdlWrite, FOREGROUND_RED | FOREGROUND_INTENSITY);
	Main.InfoBox(L"unkcmd");//报错时使用红色字体
	SetConsoleTextAttribute(hdlWrite, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
okreturn:
	if (!console)return TRUE;
	FreeConsole();//在结束前模拟一下回车键换行
	keybd_event(13, 0, 0, 0);
	keybd_event(13, 0, KEYEVENTF_KEYUP, 0);
	return TRUE;
noreturn:
	return FALSE;
}

#ifdef _DEBUG
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#else
int main()//程序入口点
#endif
{//和程序界面无关的初始化
#ifdef _DEBUG
	(hPrevInstance); (nCmdShow);
#endif

	GetPath();//获取自身目录
	Admin = IsUserAnAdmin();//是否管理员
	FirstFlag = (GetFileAttributes(TDTempPath) == -1);

	wchar_t CmdLine[MAX_PATH];
	myZeroMemory(CmdLine, sizeof(wchar_t) * MAX_PATH);
	GetRealCommandLine(CmdLine);

	if (Admin)//判断是否为服务程序
		if (mywcslen(CmdLine) > 1) {
			if (mywcscmp(Name, L"C:\\SAtemp\\myPaExec.exe") == 0) { myPAExec(TRUE); return 0; }
			if (mywcscmp(Name, L"C:\\SAtemp\\myPaExec2.exe") == 0) { myPAExec(FALSE); return 0; }
		}
	Main.str._t.InitRBTree();Main.but._t.InitRBTree();
	Eatpid._t.InitRBTree();expid._t.InitRBTree();
	tdpid._t.InitRBTree();//将几个Map清零

	GetBit();//获取系统位数
	if (Bit != 34)
		mywcscpy(SethcPath, L"C:\\Windows\\System32\\sethc.exe");
	else//根据系统位数设置SethcPath
		mywcscpy(SethcPath, L"C:\\Windows\\SysNative\\sethc.exe");

	BackupSethc();//备份sethc

	CreateStrs; //创建字符串

	mywcscpy(TDName, L"StudentMain.exe");

	if (mywcslen(CmdLine) > 1) if (RunCmdLine(CmdLine) == TRUE)return 0;

	CreateDesktop(szVDesk, NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);//创建虚拟桌面

	SetProcessAware();//让系统不对这个程序进行缩放,在一些笔记本上有用

	hInst = GetModuleHandle(0); // 将实例句柄存储在全局变量中
	if (!InitInstance())return FALSE;//和程序界面有关的初始化
	MSG msg;//消息循环: 
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);//分派消息
	}
	return (int)msg.wParam;
}

DWORD WINAPI InitThread(LPVOID pM)//创建各种控件(线程)
{
	(pM);
	Main.CreateEditEx(325 + 5, 220, 110 - 10, 50, 1, L"explorer.exe", 0, FALSE);//创建输入框
	Main.CreateEditEx(195 + 5, 355, 240 - 10, 45, 1, L"StudentMain.exe", 0, FALSE);
	Main.CreateEditEx(455 + 5, 355, 50 - 10, 45, 1, L"5", 0, FALSE);
	Main.CreateEditEx(185 + 5, 102, 110 - 10, 40, 2, L"输入新端口", 0, TRUE);
	Main.CreateEditEx(365 + 5, 175, 210 - 10, 50, 2, L"输入新密码", 0, TRUE);
	Main.CreateEditEx(195 + 5, 102, 310 - 10, 37, 3, L"浏览文件/文件夹", 0, TRUE);
	Main.CreateEditEx(277 + 5, 186, 138 - 10, 25, 5, L"StudentMain", 0, FALSE);

	Main.CreateButtonEx(1, 1, 50, 139, 64, 0, L"主要功能", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P1");//切换页面按钮
	Main.CreateButtonEx(2, 1, 115, 139, 64, 0, L"极域工具箱", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P2");//
	Main.CreateButtonEx(3, 1, 180, 139, 64, 0, L"其他工具", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P3");
	Main.CreateButtonEx(4, 1, 245, 139, 64, 0, L"关于", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P4");
	Main.CreateButtonEx(5, 1, 310, 139, 64, 0, L"设置", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P5");
	Main.CreateButtonEx(6, 1, 375, 139, 173, 0, L"一键安装", LGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"QS");
	Main.CurButton = 6;
	Main.CreateLine(140, 51, 498, LINE_Y, 0, COLOR_DARKER_GREY);
	Main.CreateLine(0, 51, 498, LINE_Y, 0, COLOR_DARKER_GREY);//切换页面按钮边上的线
	Main.CreateLine(2, 114, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 179, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 244, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 309, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 374, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(DEFAULT_WIDTH + 1, 50, DEFAULT_HEIGHT + 1 - 50, LINE_Y, 0, COLOR_DARKER_GREY);//页面最右边的线

	Main.CreateFrame(170, 75, 419, 95, 1, L" 进程方案 ");
	Main.CreateButton(195, 100, 110, 50, 1, L"安装sethc", L"Sethc");//sethc
	Main.CreateButton(325, 100, 110, 50, 1, L"全局键盘钩子", L"hook");//hook
	if (SethcState)
	{
		WIN32_FIND_DATA fileInfo;
		HANDLE hFind = FindFirstFile(SethcPath, &fileInfo);
		if (hFind != INVALID_HANDLE_VALUE && fileInfo.nFileSizeLow == MYSETHC_SIZE)
		{//判断sethc是否已经安装
			SethcInstallState = TRUE;
			mywcscpy(Main.Button[Main.CurButton - 1].Name, Main.GetStr(L"unSet"));
		}
		FindClose(hFind);
	}

	Main.CreateFrame(170, 195, 419, 320, 1, L" 窗口方案 ");//虚拟桌面系列
	Main.CreateButton(195, 220, 110, 50, 1, L"运行程序", L"runinVD");
	Main.CreateButton(455, 220, 110, 50, 1, L"切换桌面", L"SwitchD");
	Main.CreateText(195, 285, 1, L"Tctrl+b", COLOR_ORANGE);
	Main.CreateLine(185, 315, 380, LINE_X, 1, COLOR_BLACK);

	Main.CreateButton(195, 415, 110, 50, 1, L"捕捉窗口", L"CatchW");
	Main.CreateButton(325, 415, 110, 50, 1, L"监视窗口", L"CopyW");//捕捉窗口系列
	Main.CreateButton(455, 415, 110, 50, 1, L"释放窗口", L"ReturnW");
	Main.CreateText(195, 328, 1, L"Processnam", COLOR_DARKERR_GREY);//想要捕捉的进程名
	Main.CreateText(455, 328, 1, L"Delay", COLOR_DARKERR_GREY);//延迟时间
	Main.CreateText(510, 370, 1, L"second", COLOR_DARKERR_GREY);//s
	Main.CreateText(195, 480, 1, L"_Eat", COLOR_DARKER_BLUE);
	Main.Button[BUT_RELS].Enabled = FALSE;

	Main.CreateFrame(160, 75, 160, 146, 2, L" 频道工具 ");//极域"管理员设置"系列
	Main.CreateFrame(345, 75, 250, 280, 2, L" 管理员密码工具 ");
	Main.CreateText(365, 300, 2, L"Tcp1", COLOR_DARKEST_GREY);
	Main.CreateText(365, 325, 2, L"Tcp2", COLOR_ORANGE);
	Main.CreateLine(360, 160, 223, LINE_X, 2, COLOR_BLACK);

	Main.CreateFrame(160, 385, 160, 130, 2, L" 极域进程工具 ");
	Main.CreateText(175, 405, 2, L"TDState", 0);
	Main.CreateText(175, 430, 2, L"TDPID", 0);
	Main.CreateText(250, 405, 2, L"TcmdNO", COLOR_RED);

	Main.CreateFrame(170, 75, 410, 150, 3, L" 文件粉碎机 ");
	Main.CreateText(195, 185, 3, L"T.A.", 0);

	Main.CreateFrame(170, 255, 273, 105, 3, L" 电源 - 慎用 ");
	Main.CreateFrame(170, 388, 410, 105, 3, L" 杂项 ");

	Main.CreateText(325, 75, 4, L"Tcoder", NULL);//"关于"中的一堆文字
	Main.CreateText(325, 100, 4, L"Tver", NULL);
	Main.CreateText(372, 125, 4, L"Tver2", NULL);
	Main.CreateText(325, 150, 4, L"Tweb", NULL);
	Main.CreateText(372, 150, 4, L"Tweb2", COLOR_WEB_BLUE);
	Main.CreateText(170, 255, 4, L"Ttip1", NULL);
	Main.CreateText(170, 280, 4, L"Ttip2", NULL);
	Main.CreateText(170, 315, 4, L"Tbit", NULL);//一些系统信息的text
	Main.CreateText(285, 315, 4, L"Twinver", NULL);
	Main.CreateText(455, 315, 4, L"Tcmd", NULL);
	Main.CreateText(170, 340, 4, L"TTDv", NULL);
	Main.CreateText(375, 340, 4, L"TIP", NULL);
	Main.CreateText(170, 365, 4, L"TPath", NULL);
	Main.CreateLine(170, 398, 420, LINE_X, 4, COLOR_BLACK);
	Main.CreateText(170, 416, 4, L"_Tleft", NULL);
	Main.CreateText(170, 441, 4, L"Tleft2", NULL);
	
	Main.CreateText(240, 455, 5, L"nolg", COLOR_DARKEST_GREY);
	Main.CreateText(177, 375, 5, L"swlg", COLOR_DARKEST_GREY);

	Main.CreateFrame(655, 75, 170, 420, 0, L" 游戏 - 需下载 ");
	Main.CreateFrame(169, 69, 136, 171, 4, L"");

	Main.CreateArea(20, 10, 32, 32, 0);//极域图标
	Main.CreateArea(170, 70, 135, 170, 4);//zxf头像
	Main.CreateArea(170, 365, 80, 20, 4);//自选极域路径
	
	Main.CreateArea(176, 15, 85, 18, 0);//以管理员权限重启
Main.CreateArea(370, 148, 170, 40, 4);//
	if (Admin == 0)Main.CreateText(60, 17, 0, L"Tmain", COLOR_WHITE);
	else Main.CreateText(60, 17, 0, L"Tmain2", COLOR_WHITE);

	Main.CreateButtonEx(0, 530, 10, 60, 30, 0, L"×", \
		CreateSolidBrush(COLOR_CLOSE_LEAVE), CreateSolidBrush(COLOR_CLOSE_HOVER), CreateSolidBrush(COLOR_CLOSE_PRESS), \
		CreatePen(PS_SOLID, 1, COLOR_CLOSE_LEAVE), CreatePen(PS_SOLID, 1, COLOR_CLOSE_HOVER), CreatePen(PS_SOLID, 1, COLOR_CLOSE_PRESS), \
		Main.DefFont, 1, FALSE, COLOR_WHITE, L"Close");//关闭按钮

	Main.Frame[FRA_POWER].rgb = COLOR_LIGHT_ORANGE;
	Main.Frame[FRA_PASSWORD].rgb = COLOR_NOTREC;
	Main.Frame[FRA_TOPDOMAIN].rgb = COLOR_DARKER_BLUE;
	RefreshFrameText();//改变Frame颜色和文字
	return 0;
}

BOOL InitInstance()//初始化
{
	InitBrushs;//创建画笔 & 画刷
	InitHotKey();//初始化热键系统

	Main.InitClass(hInst);//初始化Class

	Main.Width = DEFAULT_WIDTH; Main.Height = DEFAULT_HEIGHT;

	GetDeviceRect();
	if (yLength >= 1400)Main.DPI = 1.5;//根据屏幕高度预先设定缩放比例
	if (yLength <= 1000)Main.DPI = 0.75;//在屏幕分辨率较低的电脑上自动降低画质
	if (yLength <= 600)Main.DPI = 0.5;
	if (yLength <= 1070)LowResource = TRUE;//注:这几行代码一定要放在窗口创建之前

	Main.DefFont = CreateFontW(max((int)(16 * Main.DPI), 10), max((int)(8 * Main.DPI), 5),//创建默认字体
		0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"宋体");

	Main.SetTitleBar(COLOR_TITLE_1, TITLEBAR_HEIGHT);//设置TitleBar

	if (!MyRegisterClass(hInst, WndProc, szWindowClass, CS_DROPSHADOW))return FALSE;//注册窗口类

	//多线程初始化
	//由于CreateWindowEx需要的时间是InitThread的7倍，差距足够大，这里不增加线程同步
	CreateThread(0, 0, InitThread, 0, 0, 0);

	Main.hWnd = CreateWindowEx(WS_EX_LAYERED, szWindowClass, Main.GetStr(L"Tmain2"), WS_POPUP, 290, 290, \
		(int)(DEFAULT_WIDTH * Main.DPI), (int)(DEFAULT_HEIGHT * Main.DPI), NULL, nullptr, hInst, nullptr);//创建主窗口

	if (!Main.hWnd)return FALSE;//创建主窗口失败就直接退出

	Main.ButtonEffect = TRUE;//按钮渐变色特效
	SetTimer(Main.hWnd, TIMER_BUTTONEFFECT, 5, (TIMERPROC)TimerProc);//启用渐变色计时器
	SetLayeredWindowAttributes(Main.hWnd, NULL, 234, LWA_ALPHA);//半透明特效

	if (!slient)AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_SHOW, MOD_CONTROL, 'P');//注册热键显示 隐藏
	RegisterHotKey(Main.hWnd, MAIN_HOTKEY_VDESKTOP, MOD_CONTROL, 'B');//切换桌面

	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_CTRL_ALT_K, MOD_CONTROL | MOD_ALT, 'K');//键盘控制鼠标
	if (FirstFlag)AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_AUTOKILLTD, NULL, VK_SCROLL);//第一次启动时自动"一键安装"

	if (!LowResource)
		if (GetSystemDefaultLangID() == 0x0409)//英文系统上自动切换语言
		{
			wchar_t Tempstr[MAX_STR];//缺点是启动速度会慢很多
			ReleaseLanguageFiles(TDTempPath, 1, Tempstr);
			SwitchLanguage(Tempstr);
		}

	Main.Timer = GetTickCount();//特别注意: GetTickCount并非实时发送，而是由系统每18ms发送一次，因此其最小精度为18ms(摘自百度百科)
	if ((Main.Timer % 49) == 0)Main.SetTitleBar(COLOR_PINK, TITLEBAR_HEIGHT);//因此将GetTickCount取模时如果和18的最大公约数不是1，就会造成概率不正确的问题
	if ((Main.Timer % 0x513) == 0)Main.SetTitleBar(COLOR_PIRPLE, TITLEBAR_HEIGHT);//例如 Main.Timer % 50 == 0 这条语句，实际触发概率是25分之1

	SetTimer(Main.hWnd, TIMER_EXPLAINATION, 250, (TIMERPROC)TimerProc);//开启Exp计时器
	if (!slient)//显示主窗口
	{
		ShowWindow(Main.hWnd, SW_SHOW);FE = FALSE;
		Main.Redraw();
	}

	Main.CreateButton(185, 155, 110, 45, 2, L"应用", L"ApplyCh");//为了加快启动速度而在显示主窗口后创建的按钮
	Main.CreateButton(365, 102, 97, 45, 2, L"清空密码", L"ClearPass");
	Main.CreateButton(477, 102, 97, 45, 2, L"查看密码", L"ViewPass");
	Main.CreateButton(365, 237, 97, 50, 2, L"改密方案1", L"CP1");
	Main.CreateButton(477, 237, 97, 50, 2, L"改密方案2", L"CP2");

	Main.CreateCheck(165, 238, 2, 135, L" 伪装工具条旧");
	Main.CreateCheck(165, 264, 2, 135, L" 伪装工具条新");
	Main.CreateCheck(165, 290, 2, 135, L" 伪装托盘图标");
	Main.CreateCheck(165, 316, 2, 170, L" 新桌面中启动极域");
	Main.CreateCheck(165, 342, 2, 170, L" Ctrl+Y 启动极域");

	Main.CreateButton(175, 458, 60, 42, 2, L"结束", L"kill-TD");
	Main.Button[Main.CurButton].Enabled = FALSE;
	Main.CreateButton(245, 458, 60, 42, 2, L"启动", L"re-TD");

	Main.CreateButton(345, 385, 115, 55, 2, L"自动捕捉极域", L"catchTD");
	Main.CreateButton(480, 385, 115, 55, 2, L"防止教师关机", L"ANTI-");
	Main.CreateButton(345, 460, 115, 55, 2, L"显示于安全桌面", L"desktop");
	Main.CreateButton(480, 460, 115, 55, 2, L"自动防控制", L"auto-5");//24

	Main.CreateButton(520, 102, 36, 36, 3, L"...", L"viewfile");
	Main.CreateButton(436, 151, 120, 55, 3, L"打开文件夹", L"folder");
	Main.CreateButton(325, 151, 97, 55, 3, L"开始粉碎", L"Delete");
	Main.CreateCheck(195, 160, 3, 100, L" 加载驱动");

	Main.CreateButton(192, 280, 115, 60, 3, L"BSOD(蓝屏)", L"BSOD");
	Main.CreateButton(324, 280, 100, 60, 3, L"快速重启", L"Res");

	Main.CreateButton(192, 412, 100, 60, 3, L"ARP攻击", L"ARP");
	Main.CreateButton(304, 412, 140, 60, 3, L"System权限CMD", L"sysCMD");
	Main.CreateButton(455, 412, 105, 60, 3, L"干掉360", L"Killer");//32

	Main.CreateButton(490, 414, 100, 50, 4, L"帮助文档", L"more");
	Main.CreateButton(490, 476, 100, 50, 4, L"系统信息", L"info");//34

	Main.CreateCheck(180, 70, 5, 100, L" 窗口置顶");
	Main.CreateCheck(180, 100, 5, 160, L" Ctrl+R 紧急蓝屏");
	Main.CreateCheck(180, 130, 5, 160, L" Ctrl+T 快速重启");
	Main.CreateCheck(180, 160, 5, 200, L" Ctrl+Alt+P 截图/显示");
	Main.CreateCheck(180, 190, 5, 94, L" 连续结束                   .exe");
	Main.CreateCheck(180, 220, 5, 230, L" (结束进程) 完全匹配进程名");
	Main.CreateCheck(180, 250, 5, 130, L" 禁止键鼠钩子");
	Main.CreateCheck(180, 280, 5, 230, L" Ctrl+Alt+K 键盘操作鼠标");
	Main.CreateCheck(180, 310, 5, 110, L" 缩小/放大");
	Main.CreateCheck(180, 340, 5, 245, L" 使用ProcessHacker结束进程");

	Main.CreateButton(470, 404, 105, 50, 5, L"永久隐藏", L"hidest");
	Main.CreateButton(470, 464, 105, 50, 5, L"清理并退出", L"clearup");//36

	Main.CreateButton(466, 255, 115, 106, 3, L"打游戏", L"Games");//37
	Main.CreateButton(680, 95, 120, 50, 0, L"小飞猜词", L"G1");
	Main.CreateButton(680, 160, 120, 50, 0, L"Flappy Bird", L"G2");
	Main.CreateButton(680, 225, 120, 50, 0, L"2048", L"G3");
	Main.CreateButton(680, 290, 120, 50, 0, L"俄罗斯方块", L"G4");
	Main.CreateButton(680, 355, 120, 50, 0, L"见缝插针", L"G5");
	Main.CreateButton(680, 420, 120, 50, 0, L"五子棋", L"G6");//43

	if ((Main.Timer % 3) == 0 && FirstFlag && !slient)
	{
		Main.InfoBox(L"Firststr");
		if ((Main.Timer % 49) == 0)Main.InfoBox(L"First2");
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//主窗口响应函数
{

	switch (message)
	{
	case WM_CLOSE: {MyExitProcess(); break; }//关闭
	case WM_CREATE://创建窗口
	{
		Main.WindowPreparation(xLength, yLength);//创建缓冲dc和bitmap
		DragAcceptFiles(hWnd, TRUE);//允许接受文件拖拽信息

		break;
	}
	case WM_HOTKEY://热键
	{

		switch (wParam)
		{
		case MAIN_HOTKEY_SHOW: //隐藏 \ 显示
		{
			if (HideState == -1)break;//永久隐藏时不显示
			ShowWindow(Main.hWnd, 5 * HideState);
			if (CatchWnd)ShowWindow(CatchWnd, 5 * HideState);
			HideState = !HideState;
			break;
		}
		case MAIN_HOTKEY_VDESKTOP: {CreateThread(0, 0, SDThread, 0, 0, 0); break; }//切换桌面
		case MAIN_HOTKEY_AUTOKILLTD: {if (HookState)AutoTerminateTD(); break; }//Scroll Lock结束极域
		case MAIN_HOTKEY_BSOD: {BSOD(0); break; }//Ctrl+R蓝屏
		case MAIN_HOTKEY_RESTART: {Restart(); break; }//Ctrl+T重启
		case MAIN_HOTKEY_SCREENSHOT://截图 or 显示
		{
			++ScreenState;
			if (ScreenState == 1)CaptureImage();
			if (ScreenState == 2)
				CreateWindow(ScreenWindow, L"You can't see me.", WS_POPUP | WS_VISIBLE, 0, 0, xLength, yLength, nullptr, nullptr, hInst, nullptr);
			break;
		}
		case MAIN_HOTKEY_CTRL_ALT_K:
		{
			if (Main.Check[CHK_KEYCTRL].Value == FALSE)RegMouseKey(), Main.Check[CHK_KEYCTRL].Value = TRUE; else UnMouseKey(), Main.Check[CHK_KEYCTRL].Value = FALSE;
			if (Main.CurWnd == 5)Main.Readd(REDRAW_CHECK, CHK_KEYCTRL), Main.Redraw(Main.GetRECTc(CHK_KEYCTRL));
			break;//键盘操作鼠标
		}
		case MAIN_HOTKEY_LEFTKEY: {mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); break; }//左键
		case MAIN_HOTKEY_RIGHTKEY: { mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0); break; }//右键
		case MAIN_HOTKEY_MOVELEFT: {mouse_event(MOUSEEVENTF_MOVE, 0xffffffff - (DWORD)(10 * Main.DPI) + 1, 0, 0, 0); break; }//左移
		case MAIN_HOTKEY_MOVEUP: {mouse_event(MOUSEEVENTF_MOVE, 0, 0xffffffff - (DWORD)(10 * Main.DPI) + 1, 0, 0); break; }//上移
		case MAIN_HOTKEY_MOVERIGHT: {mouse_event(MOUSEEVENTF_MOVE, (DWORD)(10 * Main.DPI), 0, 0, 0); break; }//右移
		case MAIN_HOTKEY_MOVEDOWN: {mouse_event(MOUSEEVENTF_MOVE, 0, (DWORD)(10 * Main.DPI), 0, 0); break; }//下移
		case MAIN_HOTKEY_RESTART_TD: {if (TDPID == 0)CreateThread(NULL, 0, ReopenThread, NULL, 0, NULL); break; }//重新打开极域
		default:Main.EditHotKey((int)wParam); break;//分派Edit操作的热键信息
		}
		break;
	}
	case WM_KILLFOCUS://这个事件在鼠标 选中 其他窗口时触发 
	{//不代表鼠标"移出"窗口
		if (!UTState)Main.EditUnHotKey();
		for (int i = 0; i <= Main.CurButton; ++i)Main.Button[i].Percent = 0;
		break;
	}
	case WM_PAINT://绘图
	{
		HBRUSH BitmapBrush = NULL; HICON hicon;
		RECT rc; PAINTSTRUCT ps;
		myZeroMemory(&rc, sizeof(RECT));
		myZeroMemory(&ps, sizeof(PAINTSTRUCT));
		if (lParam == UT_MESSAGE)goto finish;
		if (hWnd != 0)GetUpdateRect(hWnd, &rc, FALSE);

		if (hWnd != 0)Main.tdc = BeginPaint(hWnd, &ps);

		if (Main.RedrawObject()) { goto finish; }

		SelectObject(Main.hdc, WhiteBrush);//打印白色背景
		SelectObject(Main.hdc, WhitePen);
		Rectangle(Main.hdc, 0, 0, (int)(Main.Width * Main.DPI), (int)(Main.Height * Main.DPI + 1));

		if (FE) { FE = false; return 0; }
		Main.DrawTitleBar();

		Main.DrawEVERYTHING();//重绘全部

		hicon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_GUI));
		DrawIconEx(Main.hdc, (int)(20 * Main.DPI), (int)(10 * Main.DPI), hicon, (int)(32 * Main.DPI), (int)(32 * Main.DPI), 0, NULL, DI_NORMAL | DI_COMPAT);
		DeleteObject(hicon);

		if (Main.CurWnd == 4)
		{
			LoadPicture(L"C:\\SAtemp\\xiaofei.jpg", Main.hdc, 170, 70, Main.DPI);//绘制xiaofei头像
			if (EasterEggFlag)
			{
				if (hZXFsign == nullptr)hZXFsign = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ZXF2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);//加载签名

				BitmapBrush = CreatePatternBrush(hZXFsign);//绘制xiaofei签名
				SelectObject(Main.hdc, BitmapBrush);
				SelectObject(Main.hdc, WhitePen);
				Rectangle(Main.hdc, 0, 74 * 12, 99 * 1, 74 * 13);//绘制bitmap纵坐标应大于825,小于975
				StretchBlt(Main.hdc, (int)(170 * Main.DPI), (int)(465 * Main.DPI), (int)(99 * Main.DPI), (int)(74 * Main.DPI), Main.hdc, 0, 74 * 12, 99, 74, SRCCOPY);
				DeleteObject(BitmapBrush);
			}
		}
	finish://贴图


		if (UTState)//"超级置顶"开启时,不能使用系统自带的Caret,这里只好自己绘制
		{
			const int count = GetTickCount();
			if (Main.ShowCrt && Main.CoverEdit != 0 && ((count / 1000.0 - count / 1000) < 0.5))
				SelectObject(Main.hdc, BlackPen); else SelectObject(Main.hdc, WhitePen);
			Rectangle(Main.hdc, Main.CaretPos.x, Main.CaretPos.y + 2, Main.CaretPos.x + 1, Main.CaretPos.y + (int)(22 * Main.DPI));
		}

		if (!UTState)
		{
			BitBlt(Main.tdc, rc.left, rc.top, max((long)(Main.Width * Main.DPI), rc.right - rc.left), \
				max((long)(Main.Height * Main.DPI), rc.bottom - rc.top), Main.hdc, rc.left, rc.top, SRCCOPY);
		}
		else
			if (HideState == 0)
			{
				DeskDC = GetDC(0);
				BitBlt(DeskDC, UTrc.left, UTrc.top, UTrc.right - UTrc.left, UTrc.bottom - UTrc.top, Main.hdc, 0, 0, SRCCOPY);
				ReleaseDC(Deskwnd, DeskDC);
			}
		if (hWnd != 0)EndPaint(hWnd, &ps);
	}

	case 34:
		if (lParam == WM_LBUTTONUP)
		{//点击伪装的极域图zhuang标bu后xua打qu开le极域
			NOTIFYICONDATA tnd;
			tnd.cbSize = sizeof(NOTIFYICONDATA);
			tnd.hWnd = Main.hWnd;
			tnd.uID = 1;
			Shell_NotifyIcon(NIM_DELETE, &tnd);
			CreateThread(0, 0, ReopenThread, 0, 0, 0);
			Main.Check[CHK_TOOLBAR].Value = FALSE;
		}
		break;
	case WM_COMMAND://当控件接收到消息时会触发这个
	{
		if (HIWORD(wParam) == LBN_SELCHANGE) {//TDT中只用了List一个控件，所以下面的内容肯定是文件选择框了= =
			wchar_t LanguageName[MAX_PATH], LanguagePath[MAX_PATH];
			SendMessage(FileList, LB_GETTEXT, ::SendMessage(FileList, LB_GETCURSEL, 0, 0), (LPARAM)LanguageName);
			mywcscpy(LanguagePath, TDTempPath);
			mywcscat(LanguagePath, L"language\\");
			mywcscat(LanguagePath, LanguageName);
			SwitchLanguage(LanguagePath);//根据点击的内容合成字符串，然后切换语言
		}
		break;
	}
	case WM_LBUTTONDOWN://点下鼠标左键时
	{
		if (UTState && lParam != UT_MESSAGE)break;//UltraTopMost时依靠全局键盘钩子来获取输入信息
		if (SizingLock)break;
		const POINT p = Main.GetPos();
		if (p.x + p.y > (int)((Main.Width + Main.Height) * Main.DPI) - 30 && !SizingLock && !LowResource)
		{
			CreateThread(NULL, 0, SizingThread, NULL, 0, NULL);
			HCURSOR hc = LoadCursor(NULL, IDC_SIZENWSE);
			SetCursor(hc);
			DeleteObject(hc);
			SizingLock = true;
			break;
		}

		if (Main.TestInside())
		{
			if (KEY_DOWN(VK_CONTROL))
			{
				POINT point = Main.GetPos();
				if (Main.CoverButton != 0)
				{
					DragState.first = DRAG_BUTTON, DragState.second = Main.CoverButton;
					DragDefPoint = { (long)(point.x / Main.DPI) - Main.Button[Main.CoverButton].Left,(long)(point.y / Main.DPI) - Main.Button[Main.CoverButton].Top };
				}
				if (Main.CoverEdit != 0)
				{
					DragState.first = DRAG_EDIT; DragState.second = Main.CoverEdit;
					DragDefPoint = { (long)(point.x / Main.DPI) - Main.Edit[Main.CoverEdit].Left,(long)(point.y / Main.DPI) - Main.Edit[Main.CoverEdit].Top };
				}
				if (Main.CoverCheck != 0)
				{
					DragState.first = DRAG_CHECK; DragState.second = Main.CoverCheck;
					DragDefPoint = { (long)(point.x / Main.DPI) - Main.Check[Main.CoverCheck].Left,(long)(point.y / Main.DPI) - Main.Check[Main.CoverCheck].Top };
				}
				if (!Main.InsideEdit(Main.CoverEdit, point) && Main.CoverEdit != 0)
				{
					Main.Edit[Main.CoverEdit].Pos1 = Main.Edit[Main.CoverEdit].Pos2 = 0;
					const int TempCoverEdit = Main.CoverEdit;//原来一个Edit被激活
					Main.CoverEdit = 0;//现在鼠标点在那个Edit外面时
					Main.EditRedraw(TempCoverEdit);//就要将CoverEdit设为0;
					Main.EditUnHotKey();//取消热键
				}
			}
			else
				Main.LeftButtonDown();
		}
		else//点在控件内 -> 触发控件特效
		{
			if (UTState)
			{//UltraTopMost时依靠计时器来拖动窗口
				GetCursorPos(&UTMpoint);
				UTMpoint2 = UTMpoint;
				ScreenToClient(Main.hWnd, &UTMpoint);
				SetTimer(Main.hWnd, TIMER_UT3, 1, (TIMERPROC)TimerProc);
			}
			else
			{
				PostMessage(Main.hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);//点在外面 -> 拖动窗口
				SetWindowPos(Main.hWnd, 0, 0, 0, (int)(Main.Width * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE | SWP_NOZORDER);
			}
		}
		break;
	}
	case WM_LBUTTONUP://抬起鼠标左键时
	{
		if (UTState && lParam != UT_MESSAGE)break;//UltraTopMost时主窗口不响应消息

		Main.LeftButtonUp();
		if (DragState.first > 0) { DragState.first = FALSE; break; }
		const POINT point = Main.GetPos();
		
		switch (Main.CoverArea)
		{
		//s(Main.CoverArea);
		case 1://点击左上角logo切换颜色
		{//(隐藏功能)
			CHOOSECOLOR cc;
			myZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(cc);
			cc.lpCustColors = crCustColors;
			cc.Flags = CC_ANYCOLOR;
			if (!ChooseColor(&cc))break;//选择"取消"时不切换颜色
			if (GetRValue(cc.rgbResult) + GetGValue(cc.rgbResult) + GetBValue(cc.rgbResult) <= 384)
				Main.Text[29].rgb = COLOR_WHITE; else Main.Text[29].rgb = COLOR_BLACK;//增减Text时记得更改"27"这个数字
			Main.SetTitleBar(cc.rgbResult, TITLEBAR_HEIGHT);
			Main.Redraw({ 0, 0, (int)(Main.Width * Main.DPI), (int)(50 * Main.DPI) });
			break;
		}
		case 2://显示xiaofei签名
		{
			SetTimer(Main.hWnd, TIMER_COPYLEFT, 20, (TIMERPROC)TimerProc);
			EasterEggFlag = TRUE;//
			break;//
		}
		case 3:
		{//手动选择极域路径
			OPENFILENAMEW ofn;
			wchar_t strFile[MAX_PATH], str2[MAX_PATH];
			myZeroMemory(str2, sizeof(wchar_t) * MAX_PATH);
			myZeroMemory(strFile, sizeof(wchar_t) * MAX_PATH);
			myZeroMemory(&ofn, sizeof(OPENFILENAMEW));
			if (*TDPath != 0)
			{
				mywcscpy(str2, TDPath);
				for (int i = (int)mywcslen(str2) - 1; i >= 0; --i)
					if (str2[i] == L'\\') {//把程序名字符串中最后一个"\\"后面的字符去掉就是路径
						str2[i + 1] = 0; break;
					}
			}
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrFile = strFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.lpstrInitialDir = str2;
			if (GetOpenFileName(&ofn) == TRUE)
			{
				wchar_t* t = mywcsrchr(strFile, L'\\');
				mywcscpy(TDPath, strFile);
				mywcscpy(TDName, t + 1);
				TDsearched = TRUE;
				SetTDPathStr();
			}
			break;
		}
		case 4: 
		{if(!Admin)if (RunWithAdmin(Name))ExitProcess(0);//以管理员权限重启
			break;
			
		}
		case 5:
		{
			//s(12345);
			ShellExecute(NULL, L"open", L"https://tdt.mysxl.cn", NULL, NULL, SW_SHOW);
			//RunEXE(L"www.baidu.com",0,0); 
			break;
		}
		}


		if (Main.Button[Main.CoverButton].Percent < 20)goto nobutton;
		switch (Main.CoverButton)//按钮
		{
		case BUT_MAIN: { Main.SetPage(1); ShowWindow(FileList, SW_HIDE); break; }//切换页面
		case BUT_TOOLS:
		{
			Main.SetPage(2); ShowWindow(FileList, SW_HIDE);
			SetTimer(Main.hWnd, TIMER_REFRESH_TD, 250, (TIMERPROC)TimerProc);
			RefreshTDstate();
			break;
		}
		case BUT_OTHERS:
		{
			Main.SetPage(3);
			ShowWindow(FileList, SW_HIDE);
			break;
		}
		case BUT_ABOUT:
		{
			if (!InfoChecked)
			{//切换到页面4时会自动寻找极域，因此CPU占用率稍高是正常现象
				InfoChecked = TRUE;
				UpdateInfo();
				wchar_t Tempstr[MAX_PATH];
				mywcscpy(Tempstr, TDTempPath);
				mywcscat(Tempstr, L"xiaofei.jpg");
				ReleaseRes(Tempstr, IMG_ZXF, L"JPG");
			}
			ShowWindow(FileList, SW_HIDE);
			Main.SetPage(4);
			break; }
		case BUT_SETT:
		{
			if (FileList == NULL)
			{
				FileList = CreateWindowW(L"ListBox", 0, WS_CHILD | LBS_STANDARD, (int)(180 * Main.DPI), (int)(404 * Main.DPI), \
					(int)(265 * Main.DPI), (int)(125 * Main.DPI), Main.hWnd, (HMENU)1, Main.hInstance, 0);
				//创建语言文件选择ListBox
				::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);
				ReleaseLanguageFiles(TDTempPath, 0, 0);
			}
			SearchLanguageFiles();
			Main.SetPage(5);
			ShowWindow(FileList, SW_SHOW);
			break;
		}//切换到第五页时搜索语言文件

		case BUT_ONEK:
		{//一键安装
			OneClick = !OneClick;
			if (OneClick)
			{
				SetTimer(Main.hWnd, TIMER_ONEKEY, 1500, (TIMERPROC)TimerProc);
				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"unQS"));
				Main.InfoBox(L"OneOK");//切换按钮状态
			}
			else
			{
				KillTimer(Main.hWnd, TIMER_ONEKEY);
				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"setQS"));
				Main.InfoBox(L"unQSOK");
			}
			Main.ButtonRedraw(Main.CoverButton);//说是"安装"，其实就注册了一个计时器而已
			break;
		}
		case BUT_SETHC:
		{//安装 or 卸载 sethc
			if (SethcInstallState == FALSE)
			{//安装sethc
				if (DeleteSethc() == FALSE) { Main.InfoBox(L"DSR3Fail"); break; }
				else SethcState = FALSE;//删除sethc

				if (AutoSetupSethc())
				{
					mywcscpy(Main.Button[BUT_SETHC].Name, Main.GetStr(L"unQS"));
					SethcInstallState = TRUE;
				}
			}
			else
			{
				if (UninstallSethc())//这边执行封装好的函数就可以了
				{//主要的工作是改变按钮内容和重绘
					mywcscpy(Main.Button[BUT_SETHC].Name, Main.GetStr(L"sSethc"));
					SethcInstallState = FALSE;
				}
			}
			Main.ButtonRedraw(BUT_SETHC);
			break;
		}
		case BUT_HOOK://安装hook,又名"全局热键"
		{
			if (HookState == FALSE)
			{
				if (FirstHook)if (!AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_AUTOKILLTD, NULL, VK_SCROLL))break;
				FirstHook = FALSE;
				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"unQS"));
				Main.InfoBox(L"hookOK");//切换按钮状态
			}
			else
			{
				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"hook"));
				Main.InfoBox(L"unQSOK");
			}
			HookState = !HookState;
			Main.ButtonRedraw(BUT_HOOK);
			break;
		}
		case BUT_START://在虚拟桌面里运行一个进程
		{
			STARTUPINFO* si = (STARTUPINFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(STARTUPINFO));
			si->cb = sizeof(si);
			si->lpDesktop = szVDesk;
			if (Bit == 34)if (mywcsstr(Main.Edit[EDIT_RUNINVD].str, L"explorer") != 0)//如果是32位程序运行在64位上，则运行
				Main.SetEditStrOrFont(L"C:\\Windows\\explorer.exe", nullptr, EDIT_RUNINVD);//Windows目录下64位的explorer
			RunEXE(Main.Edit[EDIT_RUNINVD].str, NULL, si);//(而不是syswow64下的)，否则explorer会不能全屏
			Main.EditRedraw(EDIT_RUNINVD);
			HeapFree(GetProcessHeap(), 0, si);
			break;
		}
		case BUT_SWITCH: {CreateThread(0, 0, SDThread, 0, 0, 0); break; }//切换桌面
		case BUT_CATCH://捕捉窗口
		{
			AutoCreateCatchWnd();

			CatchWndTimerLeft = mywtoi(Main.Edit[EDIT_DELAY].str);
			Main.SetStr(Main.Button[Main.CoverButton].Name, L"back");
			if (CatchWndTimerLeft < 1)CatchWndTimerLeft = 0;

			SetTimer(Main.hWnd, TIMER_CATCHWINDOW, 1000, (TIMERPROC)TimerProc);
			if (!Main.Check[CHK_NOHOOK].Value)Main.Check[CHK_NOHOOK].Value = TRUE, SetTimer(hWnd, TIMER_ANTIHOOK, 100, (TIMERPROC)TimerProc);
			break;
		}
		case BUT_VIEW://监视窗口
		{
			AutoCreateCatchWnd();
			wchar_t tmpstr[MAX_STR * 4];
			mywcscpy(tmpstr, Main.Edit[EDIT_PROCNAME].str);
			MonitorTot = 0;
			ReturnWindows();
			FindMonitoredhWnd(tmpstr);
			if (MonitorTot != 0)MonitorCur = 1; else MonitorCur = 0;
			SetTimer(Main.hWnd, TIMER_UPDATECATCH, 33, (TIMERPROC)TimerProc);//注意:这里的timerproc是绑定在Main上的
			AutoRegisterHotKey(CatchWnd, CATCH_HOTKEY_WNDLEFT, MOD_ALT, VK_LEFT);
			AutoRegisterHotKey(CatchWnd, CATCH_HOTKEY_WNDRIGHT, MOD_ALT, VK_RIGHT);
			AutoRegisterHotKey(CatchWnd, CATCH_HOTKEY_ESCAPE, NULL, VK_ESCAPE);//注册热键
			AutoRegisterHotKey(CatchWnd, CATCH_HOTKEY_WNDSHOW, MOD_ALT, 'H');
			break;
		}
		case BUT_RELS: {ReturnWindows(); break; }//释放窗口
		case BUT_APPCH: { AutoChangeChannel(mywtoi(Main.Edit[EDIT_APPLYCH].str)); break; }//改变频道
		case BUT_CLPSWD:
		{ //A U T O
			wchar_t tmp[MAX_STR];
			myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
			AutoChangePassword(tmp, 1);
			break;
		}
		case BUT_VIPSWD: { AutoViewPass(); break; }//查看密码
		case BUT_CHPSWD1:case BUT_CHPSWD2://更改密码
		{
			const size_t len = mywcslen(Main.Edit[EDIT_CPSWD].str) + 2;

			HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
			if (!hHeap)break;

			wchar_t* str = (wchar_t*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(wchar_t) * (max(34, len)));
			if (str == 0) { error(); break; }
			mywcscpy(str, Main.Edit[EDIT_CPSWD].str);
			str[32] = str[33] = 0;
			if (Main.CoverButton == BUT_CHPSWD1)AutoChangePassword(str, 1); else AutoChangePassword(str, 2);
			HeapDestroy(hHeap);
			break;
		}
		case BUT_KILLTD: { AutoTerminateTD(); break; }//结束 or 重新打开极域
		case BUT_RETD: { CreateThread(NULL, 0, ReopenThread, NULL, 0, NULL);  break; }
		case BUT_ACTD://自动捕捉极域
		{
			if (TDhWndChild != 0 || TDhWndParent != 0)break;//如果极域已经被捕捉 -> 退出
			AutoCreateCatchWnd();//准备CatchWnd
			ReturnWindows();//归还已经被捕捉的窗口
			MonitorTot = 0;
			KillTimer(hWnd, TIMER_UPDATECATCH);//关闭窗口监视
			for (int i = CATCH_HOTKEY_WNDLEFT; i < CATCH_HOTKEY_WNDSHOW + 1; ++i)AutoUnregisterHotKey(CatchWnd, i);
			TDhWndChild = TDhWndParent = 0;
			EnumWindows(EnumBroadcastwnd, NULL);//首先寻找2016版的极域
			if (TDhWndChild == 0)EnumWindows(EnumBroadcastwndOld, NULL);//找不到的话再寻找旧版
			if (TDhWndChild == 0)Main.InfoBox(L"NoTDwnd");
			else
			{//找到后 -> 自动开启 禁止键鼠钩子
				if (!Main.Check[CHK_NOHOOK].Value)SetTimer(hWnd, TIMER_ANTIHOOK, 100, (TIMERPROC)TimerProc), Main.Check[CHK_NOHOOK].Value = TRUE;
				if (TDhWndParent == (HWND)-2)
				{
					RECT rc;
					GetClientRect(CatchWnd, &rc);
					SetWindowPos(TDhWndChild, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOREDRAW);
				}
				else
					SetTimer(Main.hWnd, TIMER_CATCHEDTD, 100, (TIMERPROC)TimerProc);
				TOP = FALSE;
				Main.Check[CHK_TOP].Value = Main.Check[CHK_UT].Value = FALSE;//取消两个窗口的置顶
				if (CatchWnd != NULL)SetWindowPos(CatchWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
				SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
				UpdateCatchedWindows();
			}
			break;
		}
		case BUT_SHUTD:
		{
			CreateThread(NULL, 0, ShutdownDeleterThread, 0, 0, NULL);
			break;
		}
		case BUT_SDESK:
		{//用自制的PaExec把自己运行在安全桌面上
			if (!Admin) { Main.InfoBox(L"StartFail"); break; }
			CopyFile(Name, L"C:\\SAtemp\\myPaExec2.exe", FALSE);
			UnloadNTDriver(L"myPaExec2");
			LoadNTDriver(L"myPaExec2", L"C:\\SAtemp\\myPaExec2.exe", FALSE);
			UnloadNTDriver(L"myPaExec2");
			break;
		}
		case BUT_AUTO: { KillFullScreen(); break; }//自动关闭置顶进程
		case BUT_VFILE: { openfile(); break; }//选择文件
		case BUT_VFOLDER: { BrowseFolder(); break; }//选择文件夹
		case BUT_DELETE: {CreateThread(NULL, 0, DeleteThread, 0, 0, NULL); break; }//开始删除文件 or 文件夹
		case BUT_BSOD: { BSOD(0); break; }//蓝屏
		case BUT_RESTART: { Restart(); break; }//快速重启
		case BUT_ARP:
		{//和Gamexx按钮一样的待遇
			if (Main.Button[BUT_ARP].DownTot != 0)break;
			wchar_t tmp[MAX_PATH];
			mywcscpy(tmp, TDTempPath);
			mywcscat(tmp, L"arp.exe");
			if (GetFileAttributes(tmp) != -1)//arp.exe文件存在
			{//直接运行
				if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			}//下载
			else CreateDownload(7);
			break;
		}
		case BUT_SYSCMD:
		{//system权限cmd
			if (!Admin) { Main.InfoBox(L"StartFail"); break; }
			CopyFile(Name, L"C:\\SAtemp\\myPaExec.exe", FALSE);
			UnloadNTDriver(L"myPaExec");
			LoadNTDriver(L"myPaExec", L"C:\\SAtemp\\myPaExec.exe", FALSE);
			break;
		}
		case BUT_360://驱动结束360
		{
			Main.InfoBox(L"360Start");
			if (!EnableKPH())Main.InfoBox(L"DrvFail");
			else
			{
				Main.Check[CHK_KPH].Value = TRUE;
				Main.Check[CHK_FMACH].Value = FALSE;//暂时关闭进程名完全匹配
				KillProcess(L"360"); KillProcess(L"zhu"); KillProcess(L"sof");
			}
			break;
		}
		case BUT_MORE://关于
		{//(可能多数人都读不懂吧)
			wchar_t tmp[MAX_PATH], tmp2[MAX_PATH];
			mywcscpy(tmp, L"Notepad ");
			mywcscpy(tmp2, TDTempPath);
			mywcscat(tmp2, L"help.txt");
			ReleaseRes(tmp2, FILE_HELP, L"JPG");
			mywcscat(tmp, tmp2);
			if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		case BUT_SYSINF://系统信息
		{
			wchar_t TempPath[] = L"C:\\Windows\\System32\\Msinfo32.exe";
			if (!RunEXE(TempPath, NULL, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		case BUT_HIDEST:
		{
			HideState = -1;
			if (CatchWnd)
			{
				ReturnWindows();
				ShowWindow(CatchWnd, SW_HIDE);
			}
			ShowWindow(Main.hWnd, SW_HIDE);
			break;
		}//永久隐藏
		case BUT_CLEAR: { ClearUp(); break; }//清理文件并退出
		case BUT_GAMES:
		{
			if (GameMode == 0)
			{//准备展开游戏界面
				wchar_t tmp[MAX_PATH];
				mywcscpy(tmp, TDTempPath);
				mywcscat(tmp, L"Games\\");//创建games目录
				CreateDirectory(tmp, NULL);

				for (int i = 0; i < numGames; ++i)
				{//搜索已存在的目录
					mywcscat(tmp, GameName[i]);
					if (GetFileAttributes(tmp) != -1)GameExist[i] = TRUE;
				}

				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Gamee"));//把按钮名变成"停止"
				GameMode = 1;
				if (!GameButtonLock)GameButtonLock = TRUE, CreateThread(NULL, 0, GameThread, 0, 0, NULL);//创建展开动画线程
				else GameMode = 0, mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Games"));
			}
			else
			{//关闭游戏界面
				GameMode = 0;
				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Games"));
				if (!GameButtonLock)GameButtonLock = TRUE, CreateThread(NULL, 0, GameThread, 0, 0, NULL);
				else GameMode = 1, mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Gamee"));
			}
			Main.ButtonRedraw(Main.CoverButton);
			break;
		}
		case BUT_GAME1:case BUT_GAME2:case BUT_GAME3:case BUT_GAME4:case BUT_GAME5:case BUT_GAME6: {//hangman and so on
			if (GameExist[Main.CoverButton - BUT_GAME1])
			{
				wchar_t TempPath[MAX_PATH];
				mywcscpy(TempPath, L"C:\\SAtemp\\Games\\");
				mywcscat(TempPath, GameName[Main.CoverButton - BUT_GAME1]);
				if (!RunEXE(TempPath, NULL, nullptr))Main.InfoBox(L"StartFail");
			}
			else { CreateDownload(Main.CoverButton - BUT_GAME1 + 1); }break;
		}
		case BUT_CLOSE: { MyExitProcess(); break; }//"关闭"按钮
		default:break;
		}
	nobutton:
		if (!Main.Check[Main.CoverCheck].Value)
		{//未选中->选中
			switch (Main.CoverCheck)
			{
			case CHK_FTOLD:case CHK_FTNEW: {//伪装工具条
				if (FF == TRUE)//注册类
				{
					MyRegisterClass(hInst, FakeProc, FakeWindow, CS_DROPSHADOW);
					FF = FALSE;
					FakeWnd = CreateWindowW(FakeWindow, Main.GetStr(L"Title"), WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInst, nullptr);//创建窗口
					SetWindowLong(FakeWnd, GWL_EXSTYLE, GetWindowLong(FakeWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
				}//WS_EX_TOOLWINDOW:没有任务栏图标的"工具栏窗口"
				if (Main.CoverCheck == CHK_FTOLD)
				{
					ReleaseRes(L"C:\\SAtemp\\Fakeold.jpg", IMG_FAKEOLD, L"JPG");
					FakeToolbarNew = FALSE;//释放图片
					FakenewUp = FALSE;
					Main.Check[CHK_FTNEW].Value = FALSE;
					SetWindowPos(FakeWnd, HWND_TOPMOST, xLength / 2 - 41, 0, 82, 48, NULL);
					KillTimer(FakeWnd, TIMER_TOOLBAR);
				}
				else
				{
					HRGN rgn1 = CreateRoundRectRgn(0, 0, 342, 63, 11, 11), rgn2 = CreateRectRgn(0, 0, 342, 10), rgn = CreateRectRgn(0, 0, 0, 0);
					CombineRgn(rgn, rgn1, rgn2, RGN_OR);
					SetWindowRgn(FakeWnd, rgn, FALSE);
					ReleaseRes(L"C:\\SAtemp\\Fakenew.jpg", IMG_FAKENEW, L"JPG");
					FakeToolbarNew = TRUE;
					FakenewUp = FALSE;//伪装(新)要开启伸展/缩回线程
					Main.Check[CHK_FTOLD].Value = FALSE;
					SetWindowPos(FakeWnd, HWND_TOPMOST, xLength / 2 - 170, 0, 340, 63, NULL);
					FakeTimer = GetTickCount();
					SetTimer(FakeWnd, TIMER_TOOLBAR, 100, (TIMERPROC)TimerProc);
				}
				Main.Readd(REDRAW_CHECK, CHK_FTOLD); Main.Readd(REDRAW_CHECK, CHK_FTNEW);
				Main.Redraw({ (long)(165 * Main.DPI),(long)(238 * Main.DPI),(long)(300 * Main.DPI),(long)(279 * Main.DPI) });
				ShowWindow(FakeWnd, SW_SHOW);
				InvalidateRect(FakeWnd, nullptr, TRUE);
				UpdateWindow(FakeWnd);
				break; }
			case CHK_TOOLBAR: {
				NOTIFYICONDATA tnd;//伪装图标
				tnd.cbSize = sizeof(NOTIFYICONDATA);
				tnd.hWnd = Main.hWnd;
				tnd.uID = 1;
				tnd.uFlags = 2 | 1 | 4;
				tnd.uCallbackMessage = 34;
				tnd.hIcon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_TD));
				mywcscpy(tnd.szTip, Main.GetStr(L"tnd"));
				if (!Shell_NotifyIcon(NIM_ADD, &tnd))Main.InfoBox(L"StartFail"), Main.Check[CHK_TOOLBAR].Value = TRUE;
				break; }
			case CHK_RETD: {
				if (!AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_RESTART_TD, MOD_CONTROL, 'Y'))
					Main.Check[CHK_RETD].Value = TRUE; Main.Redraw(Main.GetRECTc(CHK_RETD));
				break; }
			case CHK_T_A_:
			{//加载驱动删除文件
				if (!EnableTADeleter())Main.InfoBox(L"DrvFail"), Main.Check[CHK_T_A_].Value = !Main.Check[CHK_T_A_].Value;
				break;
			}
			case CHK_TOP: {
				TOP = TRUE;
				CreateThread(NULL, 0, TopThread, NULL, 0, NULL);
				if (!UTCheck)
				{
					UTCheck = TRUE;//显示"超级置顶"
					Main.CreateCheck(290, 70, 5, 120, Main.GetStr(L"UT"));
					Main.Readd(REDRAW_CHECK, CHK_UT);
					Main.Redraw();
				}
				break;
			}//置顶
			case CHK_BSOD: {AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_BSOD, MOD_CONTROL, 'R'); break; }//蓝屏
			case CHK_SHUTD: {AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_RESTART, MOD_CONTROL, 'T'); break; }//重启
			case CHK_SCSHOT: {//截图伪装
				if (FS == TRUE)MyRegisterClass(hInst, ScreenProc, ScreenWindow, NULL), FS = FALSE;
				AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_SCREENSHOT, MOD_CONTROL | MOD_ALT, 'P');
				break; }
			case CHK_REKILL: {KillProcess(Main.Edit[EDIT_TDNAME].str); SetTimer(hWnd, TIMER_KILLPROCESS, 1500, (TIMERPROC)TimerProc); break; }//连续结束进程
			case CHK_NOHOOK: {SetTimer(hWnd, TIMER_ANTIHOOK, 100, (TIMERPROC)TimerProc); break; }//禁止键盘（鼠标）钩子
			case CHK_KEYCTRL: {RegMouseKey(); break; }//键盘控制鼠标
			case CHK_DPI: {Main.SetDPI(0.75); ::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);
				SetWindowPos(FileList, 0, (int)(180 * Main.DPI), (int)(410 * Main.DPI), (int)(265 * Main.DPI), (int)(120 * Main.DPI), 0);
				if (UTState)GetWindowRect(Main.hWnd, &UTrc); break; }//缩小/放大
			case CHK_KPH:
			{//使用KProcessHacker结束进程
				if (!EnableKPH())Main.InfoBox(L"DrvFail"), Main.Check[CHK_KPH].Value = !Main.Check[CHK_KPH].Value;
				break; }
			case CHK_UT:
			{
				TOP = FALSE; Main.Check[CHK_TOP].Value = TRUE;
				ULTRATopMost(); }
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
				tnd.uID = 1;
				Shell_NotifyIcon(NIM_DELETE, &tnd);
				break; }//CHK_TDINVD 4
			case CHK_RETD: {AutoUnregisterHotKey(Main.hWnd, MAIN_HOTKEY_RESTART_TD); Main.Redraw(Main.GetRECTc(CHK_RETD)); break; }
			case CHK_TOP: {
				TOP = FALSE;
				if (UTState)noULTRA();
				Main.Check[CHK_UT].Value = FALSE;
				Main.Readd(REDRAW_CHECK, CHK_UT);
				Main.Redraw(Main.GetRECTc(CHK_UT));
				break;
			}
			case CHK_BSOD:case CHK_SHUTD: case CHK_SCSHOT: {AutoUnregisterHotKey(Main.hWnd, Main.CoverCheck - 4); break; }
			case CHK_REKILL: {KillTimer(hWnd, TIMER_KILLPROCESS); break; }//CHK_FMACH 12
			case CHK_NOHOOK: {KillTimer(hWnd, TIMER_ANTIHOOK); break; }
			case CHK_KEYCTRL: {UnMouseKey(); break; }//基本上就是把之前的过程反过来
			case CHK_DPI:
			{
				Main.SetDPI(1.5);
				::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);
				SetWindowPos(FileList, 0, (int)(180 * Main.DPI), (int)(410 * Main.DPI), (int)(265 * Main.DPI), (int)(120 * Main.DPI), 0);
				if (UTState)GetWindowRect(Main.hWnd, &UTrc); break; }
			case CHK_UT:
			{
				noULTRA();
				TOP = TRUE;
				CreateThread(NULL, 0, TopThread, NULL, 0, NULL);
			}
			}
		}
		Main.LeftButtonUp2();
		break;
	}
	case WM_MOUSEMOVE:
	{
		Main.MouseMove();
		if (DragState.first > 0 && KEY_DOWN(VK_CONTROL))
		{
			const POINT point = Main.GetPos();
			if (DragState.first == DRAG_BUTTON)
			{
				Main.Button[DragState.second].Left = (long)(point.x / Main.DPI) - DragDefPoint.x;
				Main.Button[DragState.second].Top = (long)(point.y / Main.DPI) - DragDefPoint.y;
			}
			if (DragState.first == DRAG_EDIT)
			{
				Main.Edit[DragState.second].Left = (long)(point.x / Main.DPI) - DragDefPoint.x;
				Main.Edit[DragState.second].Top = (long)(point.y / Main.DPI) - DragDefPoint.y;
			}
			if (DragState.first == DRAG_CHECK)
			{
				Main.Check[DragState.second].Left = (long)(point.x / Main.DPI) - DragDefPoint.x;
				Main.Check[DragState.second].Top = (long)(point.y / Main.DPI) - DragDefPoint.y;
			}
			Main.Redraw();
		}
		else DragState.first = 0;
		const POINT p = Main.GetPos();
		if (p.x + p.y > (int)((Main.Width + Main.Height) * Main.DPI) - 30)
		{
			HCURSOR hc = LoadCursor(NULL, IDC_SIZENWSE);
			SetCursor(hc);
			DeleteObject(hc);
		}
		break;
	}
	case WM_IME_STARTCOMPOSITION: {Main.EditComposition(); break; }//设置输入法位置
	case WM_DROPFILES://接受文件拖拽信息
	{
		POINT point = Main.GetPos();
		if (Main.InsideEdit(EDIT_FILEVIEW, point) == TRUE)//只当鼠标选中EDIT_FILEVIEW才"真正"允许拖拽文件
		{
			Main.SetEditStrOrFont(L" ", NULL, EDIT_FILEVIEW);

			HDROP hDrop = (HDROP)wParam;//explorer允许一次拖拽多个文件，
			const unsigned int numFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, 0, 0);//因此在接受信息前要统计一下一共有多少文件，

			HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);//然后申请相应的内存空间
			if (!hHeap)break;//注:DragQueryFile的第二个参数填0xFFFFFFFF时返回的时文件个数
			wchar_t* tmp = (wchar_t*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(wchar_t) * numFiles * MAX_PATH);
			if (tmp == 0) { error(); break; }
			for (unsigned int i = 0; i < numFiles; ++i)
			{
				wchar_t tmp2[MAX_PATH];
				myZeroMemory(tmp2, sizeof(wchar_t) * MAX_STR);
				DragQueryFileW(hDrop, i, tmp2, MAX_PATH);
				if (numFiles != 1)mywcscat(tmp2, L"|");
				mywcscat(tmp, tmp2);
			}
			DragFinish(hDrop);
			if (*Main.Edit[EDIT_FILEVIEW].OStr != 0)*Main.Edit[EDIT_FILEVIEW].OStr = 0;
			Main.SetEditStrOrFont(tmp, NULL, EDIT_FILEVIEW);
			Main.EditRedraw(EDIT_FILEVIEW);
			HeapDestroy(hHeap);
		}
		break;
	}
	case WM_DISPLAYCHANGE:
	{
		GetDeviceRect();
		break;
	}
	case WM_MOUSELEAVE://TrackMouseEvent带来的消息
		PostMessage(Main.hWnd, WM_MOUSEMOVE, NULL, 0);
		break;
	case WM_CHAR: { Main.EditCHAR((wchar_t)wParam); break; }//给Edit转发消息
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ScreenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//截图伪装窗口
{
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		stdc = GetDC(hWnd);//创建缓存DC(但不创建缓存bitmap)
		shdc = CreateCompatibleDC(stdc);
		AutoRegisterHotKey(hWnd, SCREEN_HOTKEY_ESCAPE, NULL, VK_ESCAPE);//注册Esc为关闭热键
		SwitchToThisWindow(hWnd, NULL);
		break;
	case WM_HOTKEY:
		AutoUnregisterHotKey(hWnd, SCREEN_HOTKEY_ESCAPE);
		ScreenState = 0;//按下热键时关闭窗口
		DestroyWindow(hWnd);
		break;
	case WM_PAINT:
	{
		stdc = BeginPaint(hWnd, &ps);

		HBITMAP bitmap;//从缓存文件夹中取出贴图
		bitmap = (HBITMAP)LoadImage(hInst, L"C:\\SAtemp\\ScreenShot.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SelectObject(shdc, bitmap);

		SetStretchBltMode(stdc, HALFTONE);//先将贴图打印在缓存DC上，然后再贴到真实DC上以提高稳定性(防止中途切换分辨率)
		StretchBlt(stdc, 0, 0, xLength, yLength, shdc, 0, 0, xLength, yLength, SRCCOPY);
		DeleteObject(bitmap);

		EndPaint(hWnd, &ps);
		break; }
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK BSODProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{//伪装蓝屏窗体的响应函数
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CLOSE://伪装蓝屏窗口被关闭时退出整个程序
		MyExitProcess();
		break;
	case WM_CREATE:
		btdc = GetDC(hWnd);//创建缓存DC和缓存bitmap
		bhdc = CreateCompatibleDC(btdc);
		BSODBitmap = CreateCompatibleBitmap(btdc, xLength, yLength);
		SelectObject(bhdc, BSODBitmap);
		ReleaseDC(hWnd, btdc);
		break;
	case WM_PAINT:
	{
		LockCursor();//伪装蓝屏时把鼠标锁在右下角以防被发现
		btdc = BeginPaint(hWnd, &ps);
		if (Main.Check[CHK_OLDBSOD].Value == FALSE)//FALSE -> 新版蓝屏
		{
			SetTextColor(bhdc, COLOR_WHITE);
			SelectObject(bhdc, BSODPen);
			SelectObject(bhdc, BSODBrush);
			SetBkMode(bhdc, 1);//新版蓝屏界面一共用了三种大小的字体
			HFONT A = CreateFontW(40, 15, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑"),
				B = CreateFontW(140, 70, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Century Gothic"),
				C = CreateFontW(26, 10, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");
			SelectObject(bhdc, B);
			Rectangle(bhdc, 0, 0, xLength, yLength);
			TextOut(bhdc, 180, 120, L":(", 2);//打印:(
			SelectObject(bhdc, A);//Century Gothic字体在win7及以前的系统中默认没有
			TextOut(bhdc, 180, 290, Main.GetStr(L"BSOD1"), (int)mywcslen(Main.GetStr(L"BSOD1")));
			wchar_t tmp[MAX_STR];
			myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
			if (BSODstate > 500)BSODstate = 500;
			myitow(int(BSODstate / 5), tmp, 10);
			mywcscat(tmp, Main.GetStr(L"BSOD7"));//打印"完成比例"
			TextOut(bhdc, 180, 360, tmp, (int)mywcslen(tmp));
			SelectObject(bhdc, WhiteBrush);
			Rectangle(bhdc, 180, 440, 180 + 141, 440 + 141);
			SelectObject(bhdc, BSODBrush);
			for (int i = 0; i < 25; ++i)//打印二维码
				for (int j = 0; j < 25; ++j)if ((QRcode[i] >> (24 - j)) % 2)Rectangle(bhdc, 188 + 5 * j, 448 + 5 * i, 188 + 5 + 5 * j, 448 + 5 + 5 * i);
			SelectObject(bhdc, C);

			TextOut(bhdc, 345, 440, Main.GetStr(L"BSOD2"), (int)mywcslen(Main.GetStr(L"BSOD2")));
			TextOut(bhdc, 345, 470, Main.GetStr(L"BSOD3"), (int)mywcslen(Main.GetStr(L"BSOD3")));//打印各项文字
			TextOut(bhdc, 345, 520, Main.GetStr(L"BSOD4"), (int)mywcslen(Main.GetStr(L"BSOD4")));
			TextOut(bhdc, 345, 550, Main.GetStr(L"BSOD5"), (int)mywcslen(Main.GetStr(L"BSOD5")));
			TextOut(bhdc, 345, 580, Main.GetStr(L"BSOD6"), (int)mywcslen(Main.GetStr(L"BSOD6")));
			DeleteObject(A); DeleteObject(B); DeleteObject(C);//贴图
		}
		if (Main.Check[CHK_OLDBSOD].Value == TRUE)//旧版蓝屏
		{
			int ybegin = 20, xbegin = 2, left, right;
			constexpr int xmax = 640, ymax = 480, s = 8;
			HPEN OBSODPen = CreatePen(PS_SOLID, 1, COLOR_OBSOD_BLUE);
			HBRUSH OBSODBrush = CreateSolidBrush(COLOR_OBSOD_BLUE);
			SetTextColor(bhdc, COLOR_WHITE);
			SelectObject(bhdc, OBSODPen);
			SelectObject(bhdc, OBSODBrush);
			SetBkMode(bhdc, 1);
			HFONT A = CreateFontW(14, 8, 0, 0, FW_THIN, 0, 0, 0, 0, 0, 0, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Lucida Console");
			SelectObject(bhdc, A);
			Rectangle(bhdc, 0, 0, 640, 480);
			for (int i = 0; i < 9; ++i)
			{
				if (ymax - 50 <= ybegin)break;
				left = right = 0;
				xbegin = 2;

				while (1)
				{
					while (words[i][right] != ' ' && words[i][right] != '\0')++right;
					if (words[i][right] == '\0')break;
					SIZE se;
					GetTextExtentPoint32A(bhdc, &words[i][left], right - left + 1, &se);
					if (se.cx + xbegin <= xmax - 2 * s)
					{
						TextOutA(bhdc, xbegin, ybegin, &words[i][left], right - left + 1);
						xbegin += se.cx;
						left = right + 1;
						right = left;
					}
					else
					{//当屏幕宽度不足时，自动换行打印
						xbegin = 2;
						ybegin += (int)(s * 1.8);
						right = left;
					}
				}
				ybegin += (int)(s * 3.6);
			}
			if (BSODstate >= 50)TextOutA(bhdc, 2, ybegin, "Collecting data for crash dump...", 33), ybegin += s * 2;
			if (BSODstate >= 150)TextOutA(bhdc, 2, ybegin, "Initializing disk for crash dump...", 35);
			DeleteObject(OBSODBrush);
			DeleteObject(OBSODPen);
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
{//捕捉窗口的窗体响应函数
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
	{//创建缓存DC和缓存bitmap
		ctdc = GetDC(CatchWnd);
		chdc = CreateCompatibleDC(ctdc);
		CatchBitmap = CreateCompatibleBitmap(ctdc, xLength, yLength);
		SelectObject(chdc, CatchBitmap);
		ReleaseDC(hWnd, ctdc);
		break;
	}
	case WM_CLOSE://关闭
	{
		KillTimer(Main.hWnd, TIMER_UPDATECATCH);
		ReturnWindows();//自动归还窗口
		TDhWndChild = TDhWndParent = 0;
		DeleteObject(CatchBitmap);
		DestroyWindow(CatchWnd);
		CatchWnd = 0;
		break;
	}
	case WM_PAINT:
	{//窗口重绘(仅用于监视窗口贴图)
		ctdc = BeginPaint(hWnd, &ps);
		if (MonitorTot != 0) {
			HDC hdctd = GetDC(MonitorList[MonitorCur]);
			RECT rc, rc2;
			int left, width, top, height;

			GetClientRect(MonitorList[MonitorCur], &rc);//获取被监视&自己窗口大小
			GetClientRect(hWnd, &rc2);

			SelectObject(chdc, WhiteBrush);//白色背景
			SelectObject(chdc, WhitePen);
			Rectangle(chdc, 0, 0, rc2.right - rc2.left, rc2.bottom - rc2.top);

			if ((rc2.right - rc2.left) == 0 || (rc2.bottom - rc2.top) == 0 || (rc.right - rc.left) == 0 || (rc.bottom - rc.top) == 0)break;
			const float wh1 = (float)(rc.right - rc.left) / (float)(rc.bottom - rc.top),
				wh2 = (float)(rc2.right - rc2.left) / (float)(rc2.bottom - rc2.top);
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
				left = (int)((rc2.right - rc2.left) / 2 - (rc2.bottom - rc2.top) * wh1 / (float)2.0);
				width = (int)((rc2.bottom - rc2.top) * wh1);
			}

			SetStretchBltMode(ctdc, HALFTONE);
			StretchBlt(ctdc, left, top, width, height, hdctd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SRCCOPY);
			ReleaseDC(MonitorList[MonitorCur], hdctd);//贴图

		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SETFOCUS:
	{
		if (MonitorTot != 0)
		{//获得焦点时注册热键(不一定有用)
			AutoRegisterHotKey(hWnd, CATCH_HOTKEY_WNDLEFT, MOD_ALT, VK_LEFT);
			AutoRegisterHotKey(hWnd, CATCH_HOTKEY_WNDRIGHT, MOD_ALT, VK_RIGHT);
			AutoRegisterHotKey(hWnd, CATCH_HOTKEY_ESCAPE, NULL, VK_ESCAPE);
			AutoRegisterHotKey(hWnd, CATCH_HOTKEY_WNDSHOW, MOD_ALT, 'H');
		}
		break;
	}
	case WM_KILLFOCUS: {
		for (int i = CATCH_HOTKEY_WNDLEFT; i < CATCH_HOTKEY_WNDSHOW + 1; ++i)AutoUnregisterHotKey(hWnd, i);
		break; }
	case WM_HOTKEY: {
		if (wParam == CATCH_HOTKEY_WNDLEFT)  --MonitorCur;
		if (wParam == CATCH_HOTKEY_WNDRIGHT) ++MonitorCur;
		if (wParam == CATCH_HOTKEY_ESCAPE) { MonitorTot = 0, KillTimer(hWnd, TIMER_UPDATECATCH); for (int i = CATCH_HOTKEY_WNDLEFT; i < CATCH_HOTKEY_WNDSHOW + 1; ++i)AutoUnregisterHotKey(hWnd, i); }
		if (wParam == CATCH_HOTKEY_WNDSHOW)
			if (IsWindowVisible(MonitorList[MonitorCur]))ShowWindow(MonitorList[MonitorCur], SW_HIDE); else ShowWindow(MonitorList[MonitorCur], SW_SHOW);
		if (MonitorCur == 0)MonitorCur = MonitorTot;
		if (MonitorCur > MonitorTot)MonitorCur = 1;
		if (wParam < CATCH_HOTKEY_WNDSHOW)InvalidateRect(CatchWnd, NULL, FALSE), UpdateWindow(CatchWnd);
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
	{//创建缓存DC和缓存bitmap
		ttdc = GetDC(hWnd);
		thdc = CreateCompatibleDC(ttdc);
		FakeBitmap = CreateCompatibleBitmap(ttdc, 400, 70);
		SelectObject(thdc, FakeBitmap);
		ReleaseDC(FakeWnd, ttdc);
		break;
	}
	case WM_PAINT:
	{
		ttdc = BeginPaint(hWnd, &ps);
		if (FakeToolbarNew)//直接简单地贴图就可以了
			LoadPicture(L"C:\\SAtemp\\Fakenew.jpg", thdc, 0, 0, 1);
		else
			LoadPicture(L"C:\\SAtemp\\Fakeold.jpg", thdc, 0, 0, 1);
		BitBlt(ttdc, 0, 0, 400, 70, thdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
	{PostMessage(FakeWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
	break; }
	case WM_MOUSEMOVE:
	{
		FakeTimer = GetTickCount();
		if (FakenewUp == TRUE)
		{
			FakenewUp = FALSE;
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