//为了减短GUI.cpp长度
//多数define会被放到这里
//by zouxiaofei1 2015 - 2022


#pragma once
#include "resource.h"

//定义版本号和位数
#ifndef _WIN64
constexpr auto Bitstr = L"32Bit Update 309";
#else
constexpr auto Bitstr = L"64Bit Update 309";
#endif
wchar_t VERSTR[] = L"ver=309";

//自定义的用于减少程序体积的Pair
struct Mypair { int first, second; };

//杂项
#define MAX_STR					256											//默认最大字符串长度
#define MAX_NUM					10											//默认最大数值
#define error()					Main.InfoBox(L"unk")						//一键报错
#define KEY_DOWN(VK_NNAME)		((GetKeyState(VK_NNAME) & 0x8000) ? 1:0)	//判断是否按下一个键
#define UT_MESSAGE				(LPARAM)999									//UltraTopMost专用信息标识
#define CONTINUE_SEARCH			1
#define STOP_SEARCH				0
#define HELP_START				80											//help.txt中"帮助"的开始和结束行数
#define HELP_END					102
#define MYSETHC_SIZE				2976										//自己编写的sethc大小，用来识别是否已安装sethc
#define DRAG_BUTTON				1											//可拖拽的控件类型
#define DRAG_EDIT				2
#define DRAG_CHECK				3
#define GAMINGSPEED				20											//"打游戏"界面展开的速度
#define MAX_TDPATHLENGHT			41											//在"关于"界面上显示的路径最大长度
#define LINE_X					0
#define LINE_Y					1
#define WM_TDICON				34
#define CHINESE_LANID			0x0804
#define TDSEARCH_NOTFOUND		0
#define TDSEARCH_SUCCEED			1
#define TDSEARCH_FAILED			2
#define TDSEARCH_FINDING			3

//绘图相关
#define Delta					5		//按钮渐变色速度
#define PRESS_PERCENTAGE			70
#define MAIN_WIDTH				621		//主窗口宽度
#define MAIN_HEIGHT				549		//主窗口高度
#define MAIN_WIDTHMINI			215		//mini模式宽度
#define MAIN_HEIGHTMINI			185		//mini模式高度
#define TDR_WIDTH				751		//TDR宽度
#define TDR_HEIGHT				444		//TDR高度
#define MAX_DPI					2		//最大缩放比(用于确定BitMap大小)
#define MAX_ES					500		//Erase Stack最大大小
#define MAX_RS					500		//Redraw Stack最大大小
#define TITLEBAR_HEIGHT			50		//窗口顶部区域的高度

//定义颜色
#define COLOR_WHITE			RGB(253,253,253)
#define COLOR_BLACK			RGB(0,0,0)
#define COLOR_RED			RGB(255,0,0)
#define COLOR_NORMAL_BLUE	RGB(40, 130, 240)
#define COLOR_LIGHTER_BLUE	RGB(230, 250, 255)
#define COLOR_LIGHT_BLUE		RGB(210, 246, 255)
#define COLOR_BSOD_BLUE		RGB(0,   117, 213)
#define COLOR_OBSOD_BLUE		RGB(1,   0, 0x80)
#define COLOR_251_GREY		RGB(253, 253, 253)
#define COLOR_LIGHTEST_GREY	RGB(248, 248, 248)
#define COLOR_LIGHTER_GREY	RGB(238, 238, 238)
#define COLOR_LIGHT_GREY		RGB(225, 225, 225)
#define COLOR_NORMAL_GREY	RGB(160, 160, 160)
#define COLOR_DARKER_GREY	RGB(120, 120, 120)
#define COLOR_DARKEST_GREY	RGB(75,75,75)
#define COLOR_DARK_YELLOW	RGB(193, 205, 205)//按钮注释
#define COLOR_LIGHT_YELLOW	RGB(0xFF,0xF8,0xDC)
//#define COLOR_TITLE_1		RGB(102,204,255)
#define COLOR_TITLE_1		RGB(88,199,255)
#define COLOR_GREEN			RGB(20, 200, 80)
#define COLOR_GREENEST		RGB(0, 255, 0)
#define COLOR_ORANGE			RGB(255, 100, 0)
#define COLOR_LIGHT_ORANGE	RGB(242, 171, 10)
#define COLOR_DARKER_BLUE	RGB(0x01, 0x8B,0xDA)
#define COLOR_CLOSE_LEAVE	RGB(255, 109, 109)
#define COLOR_CLOSE_HOVER	RGB(250, 100, 100)
#define COLOR_CLOSE_PRESS	RGB(232, 95, 95)
#define COLOR_OK				RGB(5, 200, 135)
#define COLOR_RECOMMENDED	RGB(10, 255, 10)
#define COLOR_NOTREC			RGB(0x63, 0xB8, 0xFF)
#define COLOR_PINK			RGB(255, 178, 255)
#define COLOR_PIRPLE			RGB(181, 108, 255)
#define COLOR_WEB_BLUE		RGB(65,105,225)

struct GETLAN
{//自定义的一个获取语言文件信息的结构体
	long Left, Top, Width, Height;
	wchar_t* begin, * str1, * str2;
};

//定义最大项目数
#define MAX_BUTTON			45
#define MAX_CHECK			20
#define MAX_FRAME			12
#define MAX_TEXT				32
#define MAX_EDIT				10
#define MAX_LINE				15
#define MAX_STRING			110
#define MAX_AREA				10
#define MAX_OTHERS			3
#define MAX_EXPLINES			8
#define MAX_EXPLENGTH		61		//每行的Exp最多字符数
#define MAX_EDIT_WIDTH		500		//每个Edit控件的最大宽度(缩放后)
#define MAX_EDIT_HEIGHT		80		//每个Edit控件的最大高度(缩放后)
#define MAX_LANGUAGE_LEN		11000	//默认语言文件最大长度
#define MAX_ID				11		//ID的最大长度

#define REDRAW_FRAME		1
#define REDRAW_BUTTON	2
#define REDRAW_CHECK		3
#define REDRAW_TEXT		4
#define REDRAW_EDIT		5
#define REDRAW_EXP		6

//用FILE_XXX代替资源编号
#define FILE_HELP		IDR_JPG1
#define IMG_ZXF			IDR_JPG2
#define FILE_SETHC		IDR_JPG5
#define FILE_KPH32		IDR_JPG7
#define FILE_KPH64		IDR_JPG8
//IDR_JPG9到IDR_JPG15已被弃用
#define FILE_TAX32		IDR_JPG16
#define FILE_TAX64		IDR_JPG17
#define FILE_CALLER		IDR_JPG18
#define IMG_FAKEOLD		IDR_JPG19
#define IMG_FAKENEW		IDR_JPG20
#define FILE_CHN			IDR_JPG21
#define FILE_ENG			IDR_JPG22

//用CHK_XXX代替Check的的编号
#define CHK_FTNEW		1
#define CHK_FTOLD		2
#define CHK_TOOLBAR		3
#define CHK_TDINVD		4
#define CHK_RETD			5
#define CHK_OFFLINE		6
#define CHK_T_A_			7
#define CHK_TOP			8
#define CHK_BSOD			9
#define CHK_SHUTD		10
#define CHK_SCSHOT		11
#define CHK_REKILL		12
#define CHK_FMACH		13
#define CHK_NOHOOK		14
#define CHK_KEYCTRL		15
#define CHK_DPI			16
#define CHK_KPH			17
#define CHK_UT			18
#define CHK_OLDBSOD		19


//用BUT_XXX代替按钮的编号
#define BUT_MAIN		1
#define BUT_TOOLS	2
#define BUT_OTHERS	3
#define BUT_ABOUT	4
#define BUT_SETT		5
#define BUT_ONEK		6

#define BUT_SETHC	7
#define BUT_HOOK		8
#define BUT_START	9
#define BUT_SWITCH	10
#define BUT_CATCH	11
#define BUT_VIEW		12
#define BUT_RELS		13

#define BUT_APPCH	14
#define BUT_CLPSWD	15	//Clear Password
#define BUT_VIPSWD	16	//View Password
#define BUT_CHPSWD1	17	//Change Password
#define BUT_CHPSWD2	18
#define BUT_KILLTD	19
#define BUT_RETD	20
#define BUT_ACTD	21	//Auto Catch TD
#define BUT_SHUTD	22	//Anti-Shutdown
#define BUT_DSOUND	23	//Delete Sound Files
#define BUT_AUTO	24	//Auto Anti Control

#define BUT_VFILE	25
#define BUT_VFOLDER	26
#define BUT_DELETE	27
#define BUT_BSOD	28
#define BUT_RESTART	29
#define BUT_ARP		30
#define BUT_SYSCMD	31
#define BUT_SDESK	32	//Safe Desktop

#define BUT_MORE	33
#define BUT_SYSINF	34

#define BUT_HIDEST	35
#define BUT_CLEAR	36

#define BUT_GAMES	37
#define BUT_CLOSE	0

//用TIMER_XXX代替TimerProc的编号
#define TIMER_KILLPROCESS		1
#define TIMER_CATCHWINDOW		2
#define TIMER_COPYLEFT			3
#define TIMER_EXPLAINATION		4
#define TIMER_BUTTONEFFECT		5
#define TIMER_UPDATECATCH		6
#define TIMER_ANTIHOOK			7
#define TIMER_REFRESH_TD		8
#define TIMER_UPDATEBSOD		9
#define TIMER_BITBLTBSOD		10
#define TIMER_TOOLBAR			11
#define TIMER_CATCHEDTD			12
#define TIMER_UT1				13
#define TIMER_UT2				14
#define TIMER_UT3				15
#define TIMER_ONEKEY			16
#define TIMER_BUTTONEFFECT2		17
//用FRA_XXX代替Frame的编号
#define FRA_PROCESS		1
#define FRA_WINDOW		2
#define FRA_CHANNEL		3
#define FRA_PASSWORD	4
#define FRA_TOPDOMAIN	5
#define FRA_DELETER		6
#define FRA_POWER		7
#define FRA_OTHERS		8
#define FRA_GAMES		9
#define FRA_ZXF			10

//用EDIT_XXX代替Edit的编号
#define EDIT_RUNINVD	1
#define EDIT_PROCNAME	2
#define EDIT_DELAY		3
#define EDIT_APPLYCH	4
#define EDIT_CPSWD		5
#define EDIT_FILEVIEW	6
#define EDIT_TDNAME		7

//用HOTKEY_XXX代替热键的编号
#define MAIN_HOTKEY_SHOW			1
#define MAIN_HOTKEY_VDESKTOP		2
#define MAIN_HOTKEY_AUTOKILLTD		3
#define MAIN_HOTKEY_BSOD			4
#define MAIN_HOTKEY_RESTART			5
#define MAIN_HOTKEY_SCREENSHOT		6
#define MAIN_HOTKEY_CTRL_ALT_K		7
#define MAIN_HOTKEY_LEFTKEY			8
#define MAIN_HOTKEY_RIGHTKEY		9
#define MAIN_HOTKEY_MOVELEFT		10
#define MAIN_HOTKEY_MOVEUP			11
#define MAIN_HOTKEY_MOVERIGHT		12
#define MAIN_HOTKEY_MOVEDOWN		13
#define MAIN_HOTKEY_RESTART_TD		14

#define SCREEN_HOTKEY_ESCAPE		16
#define CATCH_HOTKEY_WNDLEFT		17
#define CATCH_HOTKEY_WNDRIGHT		18
#define CATCH_HOTKEY_ESCAPE			19
#define CATCH_HOTKEY_WNDSHOW		20

#define ACT_SHUTDOWN	1
#define ACT_RESTART		2
#define ACT_CLOSE		3
#define ACT_WINDOWFY	4

#define ACTEXT_EXE		1
#define ACTEXT_HTTP		2
#define ACTEXT_MESSAGE	3

//当增减Text数量时
//
//RefreshTDstate()
//SetTDPathStr()
//
//WndProc -> LBUTTONUP -> Area -> case 1
//TimerProc -> case TIMER_COPYLEFT
//这四个地方需要格外注意
//(Text没有加入ID的后果)


//定义字符串
#define CreateStrs \
	Main.CreateString(L"安装成功，请按Scroll Lock键查看效果！", L"hookOK");\
	Main.CreateString(L"安装成功，被控制时请点击屏幕右上角图标解除!", L"OneOK");\
	Main.CreateString(L"卸载成功", L"unQSOK");\
	Main.CreateString(L"提示", L"Info");\
	Main.CreateString(L"一键安装", L"setQS");\
	Main.CreateString(L"卸载", L"unQS");\
	Main.CreateString(L"启动失败", L"StartFail");\
	Main.CreateString(L"文件删除失败！权限不足", L"DSR3Fail");\
	Main.CreateString(L"Sethc复制失败！权限不足", L"CSFail");\
	Main.CreateString(L"Sethc复制失败！\n文件不存在", L"NoSethc");\
	Main.CreateString(L"Sethc卸载失败！\n备份文件丢失", L"USFail");\
	Main.CreateString(L"频道应为1~255之间的整数，是否继续?", L"ACE");\
	Main.CreateString(L"打开键值失败!\n可能因为没有足够权限或极域未安装", L"ACFail");\
	Main.CreateString(L"修改键值失败!\n(未知错误)", L"ACUKE");\
	Main.CreateString(L"修改成功", L"ACOK");\
	Main.CreateString(L"出错了...可能是因为键值不存在\n按确定强制读取", L"VPFail");\
	Main.CreateString(L"出错了...\n按确定强制读取", L"VPUKE");\
	Main.CreateString(L"密码为空或键值不存在", L"VPNULL");\
	Main.CreateString(L"按快捷键Ctrl+B切换桌面", L"Tctrl+b");\
	Main.CreateString(L"极域各版本密码格式不同", L"Tcp1");\
	Main.CreateString(L"不保证修改成功", L"Tcp2");\
	Main.CreateString(L"作者：zouxiaofei1 cxy villager", L"Tcoder");\
	Main.CreateString(L"版本：C++ v1.9.8 正式版", L"Tver");\
	Main.CreateString(Bitstr, L"Tver2");\
	Main.CreateString(L"CopyLeft(c) SA软件 2015 - 2022", L"_Tleft");\
	Main.CreateString(L"© SA软件 2015 - 2022", L"Tleft");\
	Main.CreateString(L"版权 - 不存在的", L"Tleft2");\
	Main.CreateString(L"极域破解v1.9.8 不是管理员", L"Tmain");\
	Main.CreateString(L"极域破解v1.9.8", L"Tmain2");\
	Main.CreateString(L"不是文件 / 文件夹！\n继续尝试粉碎吗？", L"TINotF");\
	Main.CreateString(L"加载驱动失败!\n可能是因为权限不足/操作被360拦截/文件不存在", L"DrvFail");\
	Main.CreateString(L"密码为:", L"pswdis");\
	Main.CreateString(L"学生: STU02\n已连接至: T01", L"tnd");\
	Main.CreateString(L"剩余 ", L"Timer1");\
	Main.CreateString(L" 秒", L"Timer2");\
	Main.CreateString(L"捕捉窗口", L"cback");\
	Main.CreateString(L"进程名称:", L"Processnam");\
	Main.CreateString(L"延迟:", L"Delay");\
	Main.CreateString(L" 秒", L"second");\
	Main.CreateString(L"确定要把密码改成", L"CPAsk1");\
	Main.CreateString(L"么？", L"CPAsk2");\
	Main.CreateString(L"- 不可用 ", L"Useless");\
	Main.CreateString(L"- 可用 ", L"Usable");\
	Main.CreateString(L"- 推荐 ", L"Rec");\
	Main.CreateString(L"- 不推荐 ", L"nRec");\
	Main.CreateString(L"按 Ctrl+P 组合键可显示/隐藏此窗口", L"Ttip1");\
	Main.CreateString(L"一般情况下建议使用“一键安装”或“虚拟桌面”", L"Ttip2");\
	Main.CreateString(L"系统位数: ", L"Tbit");\
	Main.CreateString(L"系统版本: ", L"Twinver");\
	Main.CreateString(L"cmd状态: ", L"Tcmd");\
	Main.CreateString(L"", L"TTDv");\
	Main.CreateString(L"极域版本: ", L"_TTDv");\
	Main.CreateString(L"未知或不存在", L"TTDunk");\
	Main.CreateString(L"2010或之前", L"TTDold");\
	Main.CreateString(L"IP地址: ", L"TIP");\
	Main.CreateString(L"存在", L"TcmdOK");\
	Main.CreateString(L"不存在", L"TcmdNO");\
	Main.CreateString(L"已删除", L"Deleted");\
	Main.CreateString(L"已安装", L"Installed");\
	Main.CreateString(L"安装sethc", L"sSethc");\
	Main.CreateString(L"全局键盘钩子", L"hook");\
	Main.CreateString(L"安装", L"Setup");\
	Main.CreateString(L"已卸载", L"Uned");\
	Main.CreateString(L"极域状态: ", L"TDState");\
	Main.CreateString(L"PID: \\", L"TDPID");\
	Main.CreateString(L"极域状态: ", L"_TDState");\
	Main.CreateString(L"PID: ", L"_TDPID");\
	Main.CreateString(L"下载失败", L"DownFail");\
	Main.CreateString(L"打开文件夹",L"OpenFolder");\
	Main.CreateString(L"使用ntsd & processhacker结束进程失败！文件不存在",L"NPFail");\
	Main.CreateString(L"你的电脑遇到问题，需要重新启动。我们将为你重新启动",L"BSOD1");\
	Main.CreateString(L"有关此问题的详细信息和可能的解决方法",L"BSOD2");\
	Main.CreateString(L"请访问 http://windows.com/stopcode",L"BSOD3");\
	Main.CreateString(L"如果致电支持人员，请向他们提供以下信息:",L"BSOD4");\
	Main.CreateString(L"终止代码 : STSTEM THREAD EXCEPTION NOT HANDELED",L"BSOD5");\
	Main.CreateString(L"失败的操作 : tpsr32.sys",L"BSOD6");\
	Main.CreateString(L"% 完成",L"BSOD7");\
	Main.CreateString(L"by Tesla.Angla",L"T.A.");\
	Main.CreateString(L"\n按\"确定\"将密码复制到剪贴板中。",L"Copypswd");\
	Main.CreateString(L"操作失败!未知错误",L"unk");\
	Main.CreateString(L"粉碎中",L"Deleting");\
	Main.CreateString(L"开始粉碎",L"deleted");\
	Main.CreateString(L"极域路径: 寻找中...",L"TPath");\
	Main.CreateString(L"极域路径: ",L"_TPath");\
	Main.CreateString(L"未知",L"TPunk");\
	Main.CreateString(L"操作失败!未知命令",L"unkcmd");\
	Main.CreateString(L"极域破解: ",L"TDT");\
	Main.CreateString(L"已经捕捉了 0 个窗口", L"_Eat");\
	Main.CreateString(L"已经捕捉了 ", L"Eat1");\
	Main.CreateString(L" 个窗口", L"Eat2");\
	Main.CreateString(L"捕捉窗口", L"Title2");\
	Main.CreateString(L"未找到极域广播窗口", L"NoTDwnd");\
	Main.CreateString(L"操作成功完成", L"suc");\
	Main.CreateString(L"语言切换暂不可用", L"nolg");\
	Main.CreateString(L"语言切换:", L"swlg");\
	Main.CreateString(L" 超级置顶", L"UT");\
	Main.CreateString(L"启动中", L"starting");\
	Main.CreateString(L"欢迎使用极域破解!\n对于初学者而言，点击左下角的“一键安装”是不错的选择\n鼠标悬停在按钮上可以看到其具体的使用方法", L"Firststr");\
	Main.CreateString(L"如何“进阶”使用这款软件?\n我们建议你每个按钮都试着点一下(｀・ω・´)", L"First2");\
	Main.CreateString(L"卸载Sethc", L"unSet");\
	Main.CreateString(L"网站：", L"Tweb");\
	Main.CreateString(L"https://tdt.mysxl.cn", L"Tweb2");\
	Main.CreateString(L"操作失败,没有找到极域!", L"noTDF");\
	Main.CreateString(L"不留痕迹", L"ADExit");\
	Main.CreateString(L"点击此选项后，将本窗口隐藏，布置好给老师演示的画面，最后点击Scroll Lock键退出极域\n退出后，教师端不会收到通知，而是一直显示退出前的画面\n学生端也不受任何监视或控制", L"AoInfo");\
	Main.CreateString(L"极域破解", L"Tmain3"); 