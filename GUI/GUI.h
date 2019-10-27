//为了减短GUI.cpp长度
//一部分和Class不相关的定义和函数会被放到这里

#pragma once

#include "resource.h"

//定义版本号和位数
#ifndef _WIN64
constexpr auto Bitstr = L"32Bit Update 248";
#else
constexpr auto Bitstr = L"64Bit Update 248";
#endif

#define BUTTON_IN(x,y) if(x == Hash(y))//判断所在按钮的ID是否与之相等
#define Delta 10 //按钮渐变色速度

//定义笔刷
#define InitBrushs DBlueBrush = CreateSolidBrush(RGB(210, 255, 255));\
	LBlueBrush = CreateSolidBrush(RGB(230, 255, 255));\
	WhiteBrush = CreateSolidBrush(RGB(255, 255, 255));\
	BlueBrush = CreateSolidBrush(RGB(40, 130, 240));\
	green = CreateSolidBrush(RGB(0x66, 0xCC, 0xFF));\
	grey = CreateSolidBrush(RGB(248, 248, 248));\
	Dgrey = CreateSolidBrush(RGB(220, 220, 220));\
	yellow = CreateSolidBrush(RGB(244, 238, 175));\
	DBlue = CreatePen(PS_SOLID, 1, RGB(210, 255, 255));\
	LBlue = CreatePen(PS_SOLID, 1, RGB(230, 255, 255));\
	YELLOW = CreatePen(PS_SOLID, 1, RGB(0xC1, 0xCD, 0xCD));\
	RED = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));\
	BLACK = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));\
	White = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));\
	GREEN = CreatePen(PS_SOLID, 2, RGB(0x70, 0xCF, 0xFF));\
	GREEN2 = CreatePen(PS_SOLID, 2, RGB(5, 195, 195));\
	LGREY = CreatePen(PS_SOLID, 1, RGB(115, 115, 115));\
	BLUE = CreatePen(PS_SOLID, 1, RGB(40, 130, 240));\
	BSODBrush=CreateSolidBrush(RGB(0x00, 0x75, 0xD5));\
	BSODPen=CreatePen(PS_SOLID, 1,RGB(0x00, 0x75, 0xD5));\
	BlackBrush=CreateSolidBrush(RGB(0x00, 0x00, 0x00));

//定义最大项目数
constexpr auto MAX_BUTTON = 50;;
constexpr auto MAX_CHECK = 20;
constexpr auto MAX_FRAME = 15;
constexpr auto MAX_TEXT = 25;
constexpr auto MAX_EDIT = 10;
constexpr auto MAX_LINE = 20;
constexpr auto MAX_STRING = 90;
constexpr auto MAX_AREA = 5;
constexpr auto MAX_EXPLENGTH = 190;
constexpr auto MAX_EXPLINES = 15;

//用FILE_XXX代替不易记住的编号
#define FILE_HELP	IDR_JPG1
#define IMG_ZXF		IDR_JPG2
#define FILE_HOOK	IDR_JPG3
#define FILE_NTSD	IDR_JPG4
#define FILE_SETHC	IDR_JPG5
#define FILE_PSEXEC	IDR_JPG6
//IDR_JPG7到IDR_JPG15已被弃用
#define FILE_TAX32	IDR_JPG16
#define FILE_TAX64	IDR_JPG17
#define IMG_FAKEOLD	IDR_JPG19
#define IMG_FAKENEW	IDR_JPG20
#define FILE_CHN	IDR_JPG21
#define FILE_ENG	IDR_JPG22
#define FILE_KPH32	IDR_JPG23
#define FILE_KPH64	IDR_JPG24

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
	Main.CreateString(L"Sethc删除失败！\n可能因为权限不足/驱动文件不存在", L"DSR0Fail");\
	Main.CreateString(L"Sethc复制失败！\n权限不足", L"CSFail");\
	Main.CreateString(L"Sethc复制失败！\n文件不存在", L"NoSethc");\
	Main.CreateString(L"Sethc卸载失败！\n备份文件丢失", L"USFail");\
	Main.CreateString(L"NTSD复制失败！\n权限不足", L"CNTSDFail");\
	Main.CreateString(L"NTSD复制失败！\n文件不存在", L"NoNTSD");\
	Main.CreateString(L"打开键值失败!\n可能因为没有足够权限或极域未安装", L"ACFail");\
	Main.CreateString(L"修改键值失败!\n(未知错误)", L"ACUKE");\
	Main.CreateString(L"修改成功", L"ACOK");\
	Main.CreateString(L"出错了...可能是因为键值不存在\n按确定强制读取", L"VPFail");\
	Main.CreateString(L"出错了...\n按确定强制读取", L"VPUKE");\
	Main.CreateString(L"密码为空或键值不存在", L"VPNULL");\
	Main.CreateString(L"删除文件", L"Tdelete");\
	Main.CreateString(L"复制文件", L"Tcopy");\
	Main.CreateString(L"按快捷键Ctrl+B切换", L"Tctrl+b");\
	Main.CreateString(L"极域不同版本密码格式不一样", L"Tcp1");\
	Main.CreateString(L"不保证修改成功", L"Tcp2");\
	Main.CreateString(L"作者：zouxiaofei1 cxy villager", L"Tcoder");\
	Main.CreateString(L"版本：C++ v1.9.2 测试版", L"Tver");\
	Main.CreateString(Bitstr, L"Tver2");\
	Main.CreateString(L"CopyLeft© SA软件 2015 - 2019", L"_Tleft");\
	Main.CreateString(L"© SA软件 2015 - 2019", L"Tleft");\
	Main.CreateString(L"版权 - 不存在的", L"Tleft2");\
	Main.CreateString(L"极域破解v1.9.2 不是管理员", L"Tmain");\
	Main.CreateString(L"极域破解v1.9.2", L"Tmain2");\
	Main.CreateString(L"这不是文件 / 文件夹！\n继续尝试粉碎吗？", L"TINotF");\
	Main.CreateString(L"请启动360！360可能会报加载驱动，请放行！\n如果执行后蓝屏，请把dump送至作者邮箱", L"360Start");\
	Main.CreateString(L"加载驱动失败!\n可能是因为权限不足/操作被360拦截/文件不存在", L"360Fail");\
	Main.CreateString(L"密码为:", L"pswdis");\
	Main.CreateString(L"极域电子教室 - 已连接至教师端", L"tnd");\
	CatchWnd.CreateString(L"剩余 ", L"Timer1");\
	CatchWnd.CreateString(L" 秒", L"Timer2");\
	CatchWnd.CreateString(NULL, L"back");\
	CatchWnd.CreateString(L"进程名称:", L"Processnam");\
	CatchWnd.CreateString(L"延迟:", L"Delay");\
	CatchWnd.CreateString(L" 秒", L"second");\
	Main.CreateString(L"explorer.exe", L"E_runinVD");\
	Main.CreateString(L"输入端口", L"E_ApplyCh");\
	Main.CreateString(L"输入密码", L"E_CP");\
	Main.CreateString(L"浏览文件/文件夹", L"E_View");\
	Main.CreateString(L"StudentMain", L"E_TDname");\
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
	Main.CreateString(L"系统位数:", L"Tbit");\
	Main.CreateString(L"系统版本:", L"Twinver");\
	Main.CreateString(L"cmd状态:", L"Tcmd");\
	Main.CreateString(L"", L"TTDv");\
	Main.CreateString(L"极域版本: ", L"_TTDv");\
	Main.CreateString(L"未知或不存在", L"TTDunk");\
	Main.CreateString(L"2010或之前", L"TTDold");\
	Main.CreateString(L"IP地址:", L"TIP");\
	Main.CreateString(L"存在", L"TcmdOK");\
	Main.CreateString(L"不存在", L"TcmdNO");\
	Main.CreateString(L"打游戏", L"Games");\
	Main.CreateString(L"停止", L"Gamee");\
	Main.CreateString(L"已删除", L"Deleted");\
	Main.CreateString(L"已安装", L"Installed");\
	Main.CreateString(L"应用层", L"Ring3");\
	Main.CreateString(L"驱动层", L"Ring0");\
	Main.CreateString(L"安装", L"Setup");\
	Main.CreateString(L"已卸载", L"Uned");\
	Main.CreateString(L"极域状态: ", L"TDState");\
	Main.CreateString(L"PID: \\", L"TDPID");\
	Main.CreateString(L"极域状态: ", L"_TDState");\
	Main.CreateString(L"PID: ", L"_TDPID");\
	Main.CreateString(L"下载失败", L"DownFail");\
	Main.CreateString(L"极域破解1.9.2", L"Title");\
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
	CatchWnd.CreateString(L"已经吃掉了 ", L"Eat1");\
	CatchWnd.CreateString(L" 个窗口", L"Eat2");\
	CatchWnd.CreateString(L"捕捉窗口", L"Title");
