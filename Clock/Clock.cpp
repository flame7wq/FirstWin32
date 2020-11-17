// Clock.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Clock.h"
#include <math.h>

#define MAX_LOADSTRING 100
#define IDM_TOPMOST 101
#define IDM_HELP	102
#define IDT_TIMER1  103

// 全局变量:
HINSTANCE hInst;												// 当前实例
WCHAR szTitle[MAX_LOADSTRING] = TEXT("ClockContent");           // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING] = TEXT("Clock");            // 主窗口类名
static BOOL g_bTopmost;
static int g_xClient;
static int g_yClinet;

TCHAR szMD[MAX_LOADSTRING];										// 输出日期时间

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

VOID				SetISOTROPIC(HDC, int, int);
VOID				DrawClockFace(HDC);
VOID				DrawHand(HDC, int, int, int, COLORREF);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	
	// TODO: 在此处放置代码。

	// 初始化全局字符串
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;
	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON3));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLOCK);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON3));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP | WS_SYSMENU | WS_SIZEBOX,
		//弹出风格|标题框上带有窗口菜单(须指定WS_CAPTION样式)|可调节边框大小
		100, 100, 300, 300, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// 向系统菜单中添加自定义的项
		HMENU hSysMenu = GetSystemMenu(hWnd, FALSE);
		AppendMenu(hSysMenu, MF_SEPARATOR, 0, NULL); // MF_SEPARATOR分割线
		AppendMenu(hSysMenu, MF_STRING, IDM_TOPMOST, TEXT("TopMost"));
		AppendMenu(hSysMenu, MF_SEPARATOR, 0, NULL); // 分隔线
		AppendMenu(hSysMenu, MF_STRING, IDM_HELP, TEXT("Help"));

		// 设置定时器
		SetTimer(hWnd, IDT_TIMER1, 1000, NULL);
		g_bTopmost = FALSE;
		break;
	}
	case WM_TIMER:
	{
		// 如果窗口处于最小化状态就什么也不做
		if (IsIconic(hWnd))	// IsIconic函数用来判断窗口是否处于最小化状态
			break;

		// 取得系统时间
		SYSTEMTIME time;
		GetLocalTime(&time);
		
		// 建立坐标系
		HDC hdc = GetDC(hWnd);
		SetISOTROPIC(hdc, g_xClient, g_yClinet);
		DrawClockFace(hdc);
		DrawHand(hdc, 400, 3, time.wSecond * 6, RGB(0, 0, 0));
		DrawHand(hdc, 180, 12, time.wHour * 30 + time.wMinute / 2, RGB(0, 0, 0));
		DrawHand(hdc, 360, 8, time.wMinute * 6, RGB(0, 0, 0));
		
		break;
	}
	case WM_SYSCOMMAND:
	{
		int nID = wParam;
		if (nID == IDM_HELP)
			MessageBox(hWnd, TEXT("Just a simple Clock!"), TEXT("Clock说明"), 0);
		else if (nID == IDM_TOPMOST)
		{
			HMENU hSysMenu = GetSystemMenu(hWnd, FALSE);
			if (!g_bTopmost)
			{
				CheckMenuItem(hSysMenu, IDM_TOPMOST, MF_CHECKED);
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);
											 // 保留当前位置（忽略X和Y参数）| 不重绘更改 | 保留当前大小（忽略cx和cy参数）
				g_bTopmost = TRUE;
			}
			else
			{
				CheckMenuItem(hSysMenu, IDM_TOPMOST, MF_UNCHECKED);
				// 设置窗口最顶层
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
				g_bTopmost = FALSE;
			}
		}
		// 其他消息依旧交给系统处理
		return DefWindowProc(hWnd, WM_SYSCOMMAND, nID, 0);
	}
	case WM_CONTEXTMENU:
	{	
		POINT pt; // 获取当前鼠标坐标
		pt.x = LOWORD(lParam); 
		pt.y = HIWORD(lParam);
		// 取得系统菜单的句柄
		HMENU hSysMenu = GetSystemMenu(hWnd, FALSE);
		// 弹出系统菜单,TPM_RETURNCMD返回所选菜单的ID号
		int nID = TrackPopupMenu(hSysMenu, TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);
		if (nID > 0)
			// 程序给自己发一个消息
			SendMessageW(hWnd, WM_SYSCOMMAND, nID, NULL);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// 设置坐标系
		SetISOTROPIC(hdc, g_xClient, g_yClinet);
		// 绘制时钟外观
		DrawClockFace(hdc);

		SYSTEMTIME time;
		GetLocalTime(&time);

		// 绘制指针
		// 经过1个小时时针走30度（360/12），经过1分钟时针走0.5度（30/60）
		DrawHand(hdc, 180, 12, time.wHour * 30 + time.wMinute / 2, RGB(0, 0, 0));
		// 经过1分钟分针走6度（360/60）
		DrawHand(hdc, 360, 8, time.wMinute * 6, RGB(0, 0, 0));
		// 经过1秒钟秒针走6度（360/60）
		DrawHand(hdc, 400, 3, time.wSecond * 6, RGB(0, 0, 0));
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		g_xClient = LOWORD(lParam);
		g_yClinet = HIWORD(lParam);
		break;
	}
	case WM_NCHITTEST:  // Windows 下每个鼠标消息都是由 WM_NCHITTEST 消息产生，参数包含鼠标位置
	{
		// 即使没有标题栏也可以拖动窗口
		UINT nHitTest;
		// 该消息经由DefWP处理后函数返回一个值告诉Windows鼠标按下的是窗口的哪一部分
		nHitTest = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);  
		// 鼠标左键被按下，GetAsyncKeyState返回值小于0
		if (nHitTest == HTCLIENT && GetAsyncKeyState(MK_LBUTTON) < 0)
			nHitTest = HTCAPTION;
		// 欺骗操作系统，让它认为是在拖动标题栏
		return nHitTest;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

VOID SetISOTROPIC(HDC hdc, int x, int y)
{
	// 当绘制的图形随窗口大小而改变的时候，使用 MM_ISOTROPIC 和 MM_ANISOTROPIC 坐标映射方式最合适了
	SetMapMode(hdc, MM_ISOTROPIC);				// 改变DC中的坐标映射方式
	SetWindowExtEx(hdc, 1000, 1000, NULL);		// 设置坐标系的逻辑单位，新的宽度，新的高度
	SetViewportExtEx(hdc, x, -y, NULL);			// 设置x,y坐标轴的方向和坐标系包含的范围，第三个参数为负，说明Y与默认相反
	SetViewportOrgEx(hdc, x / 2, y / 2, NULL); // 原点坐标， 第三个值为正
}
VOID DrawClockFace(HDC hdc)
{
	const int SQUARESIZE = 20;
	static POINT pT[] = {
		0,450,
		225,390,
		390,225,
		450,0,
		390,-225,
		225,-390,
		0,-450,
		-225,-390,
		-390,-225,
		-450,0,
		-390,225,
		-225,390 };
	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	Rectangle(hdc, -5000, 5000, 5000, -5000);
	SYSTEMTIME time;
	GetLocalTime(&time);
	wsprintf(szMD, TEXT("%2d/%2d"), time.wMonth,time.wDay);
	TextOut(hdc, -350, 30, szMD, 5);
	wsprintf(szMD, TEXT("%2d:%02d"), time.wHour,time.wMinute);
	TextOut(hdc, 180, 30, szMD, 5);

	SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	for (int i = 0; i < 12; i++)
		Ellipse(hdc, pT[i].x - SQUARESIZE, pT[i].y + SQUARESIZE, pT[i].x + SQUARESIZE, pT[i].y - SQUARESIZE);
}

// 指针的长度、宽度、相对于0点偏移的角度、颜色分别由参数nLength、nWidth、nDegrees、clrColor指定
void DrawHand(HDC hdc, int nLength, int nWidth, int nDegrees, COLORREF clrColor)
{
	// 将角度nDegrees转化成弧度 .	2*3.1415926/360 == 0.0174533
	double nRadians = (double)nDegrees * 0.0174533;

	// 计算坐标
	POINT pt[2];
	pt[0].x = (int)(nLength * sin(nRadians));
	pt[0].y = (int)(nLength * cos(nRadians));
	pt[1].x = -pt[0].x / 5;
	pt[1].y = -pt[0].y / 5;

	// 创建画笔，并选如DC结构中
	HPEN hPen = CreatePen(PS_SOLID, nWidth, clrColor);
	SelectObject(hdc, hPen);

	// 画线
	MoveToEx(hdc, pt[0].x, pt[0].y, NULL);
	LineTo(hdc, pt[1].x, pt[1].y);

	DeleteObject(hPen);
}

// Conclusion
/*
	1. 定时器，每隔一定时间向窗口发送 WM_TIMER 消息
		SetTimer(hWnd, IDT_TIMER1, 1000, NULL);
	2. 获取系统时间
		SYSTEMTIME time;
		GetLocalTime(&time);
	3. 鼠标消息类型 WM_NCHITTEST  鼠标右键消息类型 WM_NCHITTEST
	4. 系统菜单
		获取
		HMENU hSysMenu = GetSystemMenu(hWnd, FALSE);
		AppendMenu(hSysMenu, MF_SEPARATOR, 0, NULL);
		弹出
		TrackPopupMenu(hSysMenu, TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);
	5. 置于顶层
		CheckMenuItem(hSysMenu, IDM_TOPMOST, MF_CHECKED);
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
	6. 绘图与坐标系映射
	7. 进一步理解消息机制
*/