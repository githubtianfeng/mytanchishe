// ̰����2.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "̰����-2.h"
#include <stdio.h>
#include <deque>

#define MAX_LOADSTRING 100

#define SNAKE_SIZE 40
#define X_FORM 20
#define Y_FORM 15
#define X_AREA X_FORM * SNAKE_SIZE
#define Y_AREA Y_FORM * SNAKE_SIZE
#define BOUND_SIZE 10



HDC g_hdc;
HDC g_hdcBuf;
HBITMAP g_hBitmap;
std::deque<POINT> deqSnake;
HBRUSH g_hBrushOut = (HBRUSH)CreateSolidBrush(RGB(0x98, 0xFB, 0x98));	//#98FB98
HBRUSH g_hBrushIn = (HBRUSH)CreateSolidBrush(RGB(0x9A, 0xCD, 0x32));	//#9ACD32
HBRUSH g_hBrushFood = (HBRUSH)CreateSolidBrush(RGB(0xff, 0x20, 0x20));
POINT g_ptFood = { 0 };
BOOL g_GameFail;

typedef enum tagDIRECT
{
	D_UP = 0,
	D_LEFT,
	D_RIGHT,
	D_DOWN
};
tagDIRECT g_direct;
tagDIRECT g_predirect;
INT g_temp;


// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
VOID Paint_Game();
VOID CentWindow(HWND hWnd, int nSizeX, int nSizeY);
VOID Init_Snake();
VOID Delete(HWND hWnd);
VOID CreateFood();
VOID Move_Snake();
VOID Move_Snake();
BOOL CheckSnake(POINT ptHead);



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2));

	// ����Ϣѭ��: 
// 	while (GetMessage(&msg, NULL, 0, 0))
// 	{
// 		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
// 		{
// 			TranslateMessage(&msg);
// 			DispatchMessage(&msg);
// 		}
// 	}
	DWORD tPre = 0, tNow = 0;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			tNow = GetTickCount();
			if (tNow - tPre > 50)
			{
				tPre = tNow;
				Paint_Game();
			}
			
		}
	}
	return (int)msg.wParam;
}



//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MY2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	hWnd = CreateWindow(szWindowClass, L"̰����", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
	g_hdc = GetDC(hWnd);
	g_hdcBuf = CreateCompatibleDC(g_hdc);//��������DC
	g_hBitmap = CreateCompatibleBitmap(g_hdc, X_FORM*SNAKE_SIZE, Y_FORM*SNAKE_SIZE);
	SelectObject(g_hdcBuf, g_hBitmap);

	

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		CentWindow(hWnd, X_AREA + 2 * BOUND_SIZE, Y_AREA + 2 * BOUND_SIZE);
		Init_Snake();
		CreateFood();
		SetTimer(hWnd, 1, 300, NULL);	
		break;
	case WM_TIMER:
		g_temp ^= 1;
		Move_Snake();
		if (g_GameFail)
		{
			KillTimer(hWnd, 1);
			MessageBox(NULL, L"��Ϸ����", L"FAIL", MB_OK);
			break;
		}
	//	Paint_Game();
	//	InvalidateRect(hWnd, 0, TRUE);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			if (g_direct != D_DOWN) g_direct = D_UP;
			break;
		case VK_DOWN:
			if (g_direct != D_UP) g_direct = D_DOWN;
			break;
		case VK_LEFT:
			if (g_direct != D_RIGHT) g_direct = D_LEFT;
			break;
		case VK_RIGHT:
			if (g_direct != D_LEFT) g_direct = D_RIGHT;
			break;
		case VK_SPACE:
			system("pause");
			break;
		default:
			break;
		}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  �ڴ���������ͼ����...

	//	Paint_Game();
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		Delete(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

VOID Paint_Game()
{
	std::deque<POINT>::iterator it;
	SelectObject(g_hdcBuf, (HBRUSH)GetStockObject(GRAY_BRUSH));
	Rectangle(g_hdcBuf, 0, 0, X_AREA, Y_AREA);

	//����

	POINT ptHead = deqSnake.back();
	POINT pt1, pt2;

	switch (g_direct)
	{
	case D_UP:
		pt1.x = ptHead.x;
		pt1.y = ptHead.y;
		pt2.x = ptHead.x + 1;
		pt2.y = ptHead.y;
		break;
	case D_LEFT:
		pt1.x = ptHead.x;
		pt1.y = ptHead.y + 1;
		pt2.x = ptHead.x;
		pt2.y = ptHead.y;
		break;
	case D_RIGHT:
		pt1.x = ptHead.x + 1;
		pt1.y = ptHead.y;
		pt2.x = ptHead.x + 1;
		pt2.y = ptHead.y + 1;
		break;
	case D_DOWN:
		pt1.x = ptHead.x + 1;
		pt1.y = ptHead.y + 1;
		pt2.x = ptHead.x;
		pt2.y = ptHead.y + 1;
		break;
	default:
		break;
	}
	

// 	Rectangle(g_hdcBuf, SNAKE_SIZE*ptHead.x, SNAKE_SIZE*ptHead.y,
// 		SNAKE_SIZE*(ptHead.x + 1) - SNAKE_SIZE / 2, SNAKE_SIZE*(ptHead.y + 1));



	//�Ȼ���ͷ
	if (g_temp)
	{
		SelectObject(g_hdcBuf, g_hBrushIn);
		Pie(g_hdcBuf, SNAKE_SIZE*ptHead.x, SNAKE_SIZE*ptHead.y,
			SNAKE_SIZE*(ptHead.x + 1), SNAKE_SIZE*(ptHead.y + 1),
			SNAKE_SIZE*pt1.x, SNAKE_SIZE*pt1.y,
			SNAKE_SIZE*pt2.x, SNAKE_SIZE*pt2.y);	//Pie������
	}
	else
	{
		SelectObject(g_hdcBuf, g_hBrushIn);
		Ellipse(g_hdcBuf, ptHead.x * SNAKE_SIZE + 3, ptHead.y * SNAKE_SIZE + 3,
			(ptHead.x + 1)*SNAKE_SIZE - 3, (ptHead.y + 1)*SNAKE_SIZE - 3);
	}

	//�ٻ�����λ��
	for (it = deqSnake.begin(); it != deqSnake.end() - 1; ++it)
	{
		SelectObject(g_hdcBuf, g_hBrushOut);
		Rectangle(g_hdcBuf, it->x*SNAKE_SIZE, it->y*SNAKE_SIZE,
			(it->x + 1)*SNAKE_SIZE, (it->y + 1)*SNAKE_SIZE);
		SelectObject(g_hdcBuf, g_hBrushIn);
		Rectangle(g_hdcBuf, it->x * SNAKE_SIZE + 3, it->y * SNAKE_SIZE + 3,
			(it->x + 1)*SNAKE_SIZE - 3, (it->y + 1)*SNAKE_SIZE - 3);
	}

	//��ʳ��
	SelectObject(g_hdcBuf, g_hBrushFood);
	Ellipse(g_hdcBuf, g_ptFood.x * SNAKE_SIZE, g_ptFood.y * SNAKE_SIZE,
		(g_ptFood.x + 1)*SNAKE_SIZE, (g_ptFood.y + 1)*SNAKE_SIZE);

	BitBlt(g_hdc, BOUND_SIZE, BOUND_SIZE, X_AREA, Y_AREA, g_hdcBuf, 0, 0, SRCCOPY);

}

VOID CentWindow(HWND hWnd, int nSizeX, int nSizeY)
{
	int nWinX, nWinY, nClientX, nClientY;
	RECT rect;
	int nScreenX = GetSystemMetrics(SM_CXSCREEN);
	int nScreenY = GetSystemMetrics(SM_CYSCREEN);

	GetWindowRect(hWnd, &rect);
	nWinX = rect.right - rect.left;
	nWinY = rect.bottom - rect.top;

	GetClientRect(hWnd, &rect);
	nClientX = rect.right - rect.left;
	nClientY = rect.bottom - rect.top;

	nSizeX += (nWinX - nClientX);
	nSizeY += (nWinY - nClientY);

	MoveWindow(hWnd, (nScreenX - nSizeX) / 2, (nScreenY - nSizeY) / 2, nSizeX, nSizeY, TRUE);

}

VOID Init_Snake()
{
	POINT stNode = { 0 };

	for (int i = 0; i < 4; ++i)
	{
		stNode.x = 4 - i;
		stNode.y = 1;
		deqSnake.push_front(stNode);
	}
	g_GameFail = FALSE;
	g_direct = D_RIGHT;
}

VOID Delete(HWND hWnd)
{
	ReleaseDC(hWnd, g_hdc);
	DeleteObject(g_hBitmap);
	DeleteObject(g_hBrushIn);
	DeleteObject(g_hBrushOut);
	DeleteObject(g_hdcBuf);
}

VOID CreateFood()
{
	std::deque<POINT>::iterator it;
	srand(GetTickCount());
	while (true)
	{
		g_ptFood.x = rand() % X_FORM;
		g_ptFood.y = rand() % Y_FORM;

		for (it = deqSnake.begin(); it != deqSnake.end(); ++it)
		{
			if (it->x == g_ptFood.x && it->y == g_ptFood.y)
			{
				break;
			}

		}
		if (it == deqSnake.end())
		{
			break;
		}
	}
}

VOID Move_Snake()
{
	POINT stNode;
	stNode.x = deqSnake.back().x;
	stNode.y = deqSnake.back().y;

	if (g_predirect + g_direct == 3)
	{
		g_direct = g_predirect;
	}
	switch (g_direct)
	{
	case D_UP:
		stNode.y--;
		break;
	case D_DOWN:
		stNode.y++;
		break;
	case D_LEFT:
		stNode.x--;
		break;
	case D_RIGHT:
		stNode.x++;
		break;
	default:
		break;
	}

	if (CheckSnake(stNode))
	{
		deqSnake.push_back(stNode);

		if (stNode.x == g_ptFood.x && stNode.y == g_ptFood.y)
		{
			CreateFood();
		}
		else
		{
			deqSnake.pop_front();
		}
		
	}
	else
	{
		g_GameFail = TRUE;
	}
	g_predirect = g_direct;
}

BOOL CheckSnake(POINT ptHead)
{
	if (ptHead.x < 0 || ptHead.x > X_FORM - 1) return FALSE;
	
	if (ptHead.y < 0 || ptHead.y > Y_FORM) return FALSE;
		
	for (POINT pt : deqSnake)
	{
		if (ptHead.x == pt.x && ptHead.y == pt.y)
		{
			g_GameFail = TRUE;
			return FALSE;		
		}
	}
	return TRUE;
}


