//为了减短GUI.cpp长度
//一部分和Class不相关的定义和函数会被放到这里

#pragma once

#include "resource.h"

//定义版本号和位数
#ifndef _WIN64
constexpr auto Bitstr = L"32Bit Update 269";
#else
constexpr auto Bitstr = L"64Bit Update 269";
#endif

#define Delta 10 //按钮渐变色速度
#define MY_MAX_PATH	300
#define error() Main.InfoBox(L"unk")
#define KEY_DOWN(VK_NNAME) ((GetKeyState(VK_NNAME) & 0x8000) ? 1:0)

//定义颜色
#define COLOR_WHITE			RGB(255,255,255)
#define COLOR_BLACK			RGB(0,0,0)
#define COLOR_RED			RGB(255,0,0)
#define COLOR_NORMAL_BLUE	RGB(40, 130, 240)
#define COLOR_LIGHTER_BLUE	RGB(230, 255, 255)
#define COLOR_LIGHT_BLUE	RGB(210, 255, 255)
#define COLOR_BSOD_BLUE		RGB(0,   117, 213)
#define COLOR_OBSOD_BLUE	RGB(1,   0, 0x80)
#define COLOR_LIGHTER_GREY	RGB(243, 243, 243)
#define COLOR_LIGHT_GREY	RGB(220, 220, 220)
#define COLOR_DARKER_GREY	RGB(150, 150, 150)
#define COLOR_NORMAL_GREY	RGB(135, 135, 135)
#define COLOR_DARKEST_GREY	RGB(50,50,50)
#define COLOR_DARK_YELLOW	RGB(193, 205, 205)
#define COLOR_LIGHT_YELLOW	RGB(244, 238, 175)
#define COLOR_TITLE_1		RGB(0x66, 0xCC, 0xFF)
#define COLOR_TITLE_2		RGB(0x70, 0xCF, 0xFF)
#define COLOR_GREEN			RGB(20, 200, 80)
#define COLOR_GREENEST		RGB(0, 255, 0)
#define COLOR_ORANGE		RGB(255, 100, 0)
#define COLOR_LIGHT_ORANGE	RGB(255, 180, 10)
#define COLOR_DARKER_BLUE	0xDA8B01
#define COLOR_CLOSE_LEAVE	RGB(255, 109, 109)
#define COLOR_CLOSE_HOVER	RGB(250, 100, 100)
#define COLOR_CLOSE_PRESS	RGB(232, 95, 95)
#define COLOR_OK			RGB(5, 200, 135)
#define COLOR_RECOMMENDED	RGB(10, 255, 10)
#define COLOR_NOTREC		RGB(0x63, 0xB8, 0xFF)

//定义笔刷
#define InitBrushs \
	WhiteBrush = CreateSolidBrush(COLOR_WHITE);\
	DBlueBrush = CreateSolidBrush(COLOR_LIGHT_BLUE);\
	LBlueBrush = CreateSolidBrush(COLOR_LIGHTER_BLUE);\
	NormalBlueBrush = CreateSolidBrush(COLOR_NORMAL_BLUE);\
	DBluePen = CreatePen(PS_SOLID, 1, COLOR_LIGHT_BLUE);\
	LBluePen = CreatePen(PS_SOLID, 1, COLOR_LIGHTER_BLUE);\
	LGreyBrush = CreateSolidBrush(COLOR_LIGHTER_GREY);\
	DGreyBrush = CreateSolidBrush(COLOR_LIGHT_GREY);\
	YellowBrush = CreateSolidBrush(COLOR_LIGHT_YELLOW);\
	TitleBrush = CreateSolidBrush(COLOR_TITLE_1); \
	YellowPen = CreatePen(PS_SOLID, 1, COLOR_DARK_YELLOW);\
	BlackPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);\
	WhitePen = CreatePen(PS_SOLID, 1, COLOR_WHITE);\
	TitlePen = CreatePen(PS_SOLID, 2, COLOR_TITLE_2);\
	CheckGreenPen = CreatePen(PS_SOLID, 2,COLOR_GREEN);\
	NormalGreyPen = CreatePen(PS_SOLID, 1, COLOR_NORMAL_GREY );\
	NormalBluePen = CreatePen(PS_SOLID, 1,COLOR_NORMAL_BLUE );\
	BSODBrush=CreateSolidBrush(COLOR_BSOD_BLUE);\
	BSODPen=CreatePen(PS_SOLID, 1,COLOR_BSOD_BLUE);\
	BlackBrush=CreateSolidBrush(COLOR_BLACK);

struct GETLAN
{//自定义的一个获取语言文件信息的结构体
	int Left, Top, Width, Height;
	wchar_t* begin, * str1, * str2;
};

//定义最大项目数
#define MAX_BUTTON		45
#define MAX_CHECK		20
#define MAX_FRAME		12
#define MAX_TEXT		30
#define MAX_EDIT		10
#define MAX_LINE		20
#define MAX_STRING		110
#define MAX_AREA		5
#define MAX_EXPLINES	8
#define MAX_EXPLENGTH	61	//每行的Exp最多字符数
#define MAX_EDIT_WIDTH	500
#define MAX_EDIT_HEIGHT	80

#define REDRAW_FRAME	1
#define REDRAW_BUTTON	2
#define REDRAW_CHECK	3
#define REDRAW_TEXT		4
#define REDRAW_EDIT		5
#define REDRAW_EXP		6

//用FILE_XXX代替资源编号
#define FILE_HELP	IDR_JPG1
#define IMG_ZXF		IDR_JPG2
#define FILE_HOOK	IDR_JPG3
//#define FILE_NTSD	IDR_JPG4
#define FILE_SETHC	IDR_JPG5
#define FILE_PSEXEC	IDR_JPG6
#define FILE_KPH32	IDR_JPG7
#define FILE_KPH64	IDR_JPG8
//IDR_JPG9到IDR_JPG15已被弃用
#define FILE_TAX32	IDR_JPG16
#define FILE_TAX64	IDR_JPG17
#define FILE_CALLER	IDR_JPG18
#define IMG_FAKEOLD	IDR_JPG19
#define IMG_FAKENEW	IDR_JPG20
#define FILE_CHN	IDR_JPG21
#define FILE_ENG	IDR_JPG22
#define FILE_TRA	IDR_JPG23

//用CHK_XXX代替Check的的编号
#define CHK_FTOLD	1
#define CHK_FTNEW	2
#define CHK_TOOLBAR	3
#define CHK_TDINVD	4
#define CHK_RETD	5
#define CHK_T_A_	6
#define CHK_TOP		7
#define CHK_BSOD	8
#define CHK_SHUTD	9
#define CHK_SCSHOT	10
#define CHK_REKILL	11
#define CHK_FMACH	12
#define CHK_NOHOOK	13
#define CHK_KEYCTRL	14
#define CHK_EFFECT	15
#define CHK_DPI		16
#define CHK_KPH		17
#define CHK_UT		18


//用BUT_XXX代替按钮的编号
#define BUT_MAIN	1
#define BUT_TOOLS	2
#define BUT_OTHERS	3
#define BUT_ABOUT	4
#define BUT_SETT	5
#define BUT_ONEK	6

#define BUT_SETHC	7
#define BUT_HOOK	8
#define BUT_START	9
#define BUT_SWITCH	10
#define BUT_CATCH	11
#define BUT_VIEW	12
#define BUT_RELS	13

#define BUT_APPCH	14
#define BUT_CLPSWD	15	//Clear Password
#define BUT_VIPSWD	16	//View Password
#define BUT_CHPSWD1	17	//Change Password
#define BUT_CHPSWD2	18
#define BUT_KILLTD	19
#define BUT_RETD	20
#define BUT_ACTD	21	//Auto Catch TD
#define BUT_SHUTD	22	//Anti-Shutdown
#define BUT_SDESK	23	//Safe Desktop
#define BUT_AUTO	24	//Auto Anti Control

#define BUT_VFILE	25
#define BUT_VFOLDER	26
#define BUT_DELETE	27
#define BUT_BSOD	28
#define BUT_RESTART	29
#define BUT_ARP		30
#define BUT_SYSCMD	31
#define BUT_360		32

#define BUT_MORE	33
#define BUT_SYSINF	34

#define BUT_HIDEST	35
#define BUT_CLEAR	36

#define BUT_GAMES	37
#define BUT_GAME1	38
#define BUT_GAME2	39
#define BUT_GAME3	40
#define BUT_GAME4	41
#define BUT_GAME5	42
#define BUT_GAME6	43

#define BUT_CLOSE	44


//用TIMER_XXX代替TimerProc的编号
#define TIMER_KILLPROCESS	1
#define TIMER_CATCHWINDOW	2
#define TIMER_COPYLEFT		3
#define TIMER_EXPLAINATION	4
#define TIMER_BUTTONEFFECT	5
#define TIMER_UPDATECATCH	6
#define TIMER_ANTYHOOK		7
#define TIMER_REFRESH_TD	8
#define TIMER_UPDATEBSOD	9
#define TIMER_TOP			10
#define TIMER_TOOLBAR		11
#define TIMER_CATCHEDTD		12
#define TIMER_UT1			13
#define TIMER_UT2			14
#define TIMER_UT3			15

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

//
#define HELP_START		19
#define HELP_END		37
//

//定义字符串
#define CreateStrs \
	Main.CreateString(L"安装成功，请按Scroll Lock键查看效果！", L"OneOK");\
	Main.CreateString(L"安装失败\n文件不存在", L"OneFail");\
	Main.CreateString(L"卸载成功", L"unQSOK");\
	Main.CreateString(L"提示", L"Info");\
	Main.CreateString(L"一键安装", L"setQS");\
	Main.CreateString(L"卸载", L"unQS");\
	Main.CreateString(L"启动失败", L"StartFail");\
	Main.CreateString(L"Sethc删除失败！权限不足", L"DSR3Fail");\
	Main.CreateString(L"Sethc复制失败！\n权限不足", L"CSFail");\
	Main.CreateString(L"Sethc复制失败！\n文件不存在", L"NoSethc");\
	Main.CreateString(L"Sethc卸载失败！\n备份文件丢失", L"USFail");\
	Main.CreateString(L"打开键值失败!\n可能因为没有足够权限或极域未安装", L"ACFail");\
	Main.CreateString(L"修改键值失败!\n(未知错误)", L"ACUKE");\
	Main.CreateString(L"修改成功", L"ACOK");\
	Main.CreateString(L"出错了...可能是因为键值不存在\n按确定强制读取", L"VPFail");\
	Main.CreateString(L"出错了...\n按确定强制读取", L"VPUKE");\
	Main.CreateString(L"密码为空或键值不存在", L"VPNULL");\
	Main.CreateString(L"按快捷键Ctrl+B切换", L"Tctrl+b");\
	Main.CreateString(L"极域不同版本密码格式不一样", L"Tcp1");\
	Main.CreateString(L"不保证修改成功", L"Tcp2");\
	Main.CreateString(L"作者：zouxiaofei1 cxy villager", L"Tcoder");\
	Main.CreateString(L"版本：C++ v1.9.3 正式版", L"Tver");\
	Main.CreateString(Bitstr, L"Tver2");\
	Main.CreateString(L"CopyLeft© SA软件 2015 - 2020", L"_Tleft");\
	Main.CreateString(L"© SA软件 2015 - 2020", L"Tleft");\
	Main.CreateString(L"版权 - 不存在的", L"Tleft2");\
	Main.CreateString(L"极域破解v1.9.3 不是管理员", L"Tmain");\
	Main.CreateString(L"极域破解v1.9.3", L"Tmain2");\
	Main.CreateString(L"不是文件 / 文件夹！\n继续尝试粉碎吗？", L"TINotF");\
	Main.CreateString(L"请启动360！360可能会报加载驱动，请放行！\n如果执行后蓝屏，请把dump送至作者邮箱", L"360Start");\
	Main.CreateString(L"加载驱动失败!\n可能是因为权限不足/操作被360拦截/文件不存在", L"360Fail");\
	Main.CreateString(L"密码为:", L"pswdis");\
	Main.CreateString(L"极域电子教室 - 已连接至教师端", L"tnd");\
	Main.CreateString(L"剩余 ", L"Timer1");\
	Main.CreateString(L" 秒", L"Timer2");\
	Main.CreateString(NULL, L"back");\
	Main.CreateString(L"进程名称:", L"Processnam");\
	Main.CreateString(L"延迟:", L"Delay");\
	Main.CreateString(L" 秒", L"second");\
	Main.CreateString(L"确定要把密码改成", L"CPAsk1");\
	Main.CreateString(L"么？", L"CPAsk2");\
	Main.CreateString(L"下载成功", L"Loaded");\
	Main.CreateString(L"正在下载", L"Loading");\
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
	Main.CreateString(L"打游戏", L"Games");\
	Main.CreateString(L"停止", L"Gamee");\
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
	Main.CreateString(L"极域破解1.9.3", L"Title");\
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
	Main.CreateString(L"需要下载WinPcap并手动安装，是否同意?",L"WPAsk");\
	Main.CreateString(L"\n按\"确定\"将密码复制到剪贴板中。",L"Copypswd");\
	Main.CreateString(L"操作失败!未知错误",L"unk");\
	Main.CreateString(L"粉碎中",L"deleting");\
	Main.CreateString(L"开始粉碎",L"deleted");\
	Main.CreateString(L"极域路径: ",L"TPath");\
	Main.CreateString(L"极域路径: ",L"_TPath");\
	Main.CreateString(L"未知",L"TPunk");\
	Main.CreateString(L"操作失败!未知命令",L"unkcmd");\
	Main.CreateString(L"极域破解: ",L"TDT");\
	Main.CreateString(L"已经吃掉了 0 个窗口", L"_Eat");\
	Main.CreateString(L"已经吃掉了 ", L"Eat1");\
	Main.CreateString(L" 个窗口", L"Eat2");\
	Main.CreateString(L"捕捉窗口", L"Title2");\
	Main.CreateString(L"未找到极域广播窗口", L"NoTDwnd");\
	Main.CreateString(L"注册热键 ", L"HKF1");\
	Main.CreateString(L"Ctrl +", L"HKFctrl");\
	Main.CreateString(L"Alt +", L"HKFalt");\
	Main.CreateString(L"Shift +", L"HKFshift");\
	Main.CreateString(L"Win +", L"HKFwin");\
	Main.CreateString(L" 失败!已更改为 ", L"HKF2");\
	Main.CreateString(L"注册热键彻底失败!", L"HKF3");\
	Main.CreateString(L"操作成功完成", L"suc");\
	Main.CreateString(L"语言切换暂不可用", L"nolg");\
	Main.CreateString(L"超级置顶", L"UT");