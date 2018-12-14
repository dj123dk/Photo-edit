#include "stdafx.h"
#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

HINSTANCE g_hInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wndclass;

	g_hInstance = hInstance;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = (WNDPROC)WndProc;
	wndclass.lpszClassName = "Paint";
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wndclass.style = CS_HREDRAW|CS_VREDRAW;
	RegisterClass(&wndclass);

	hWnd = CreateWindow("Paint", "Paint", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while(GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

struct {
	int x1, y1, x2, y2;
	char type;
	char lineType;
} saveDraw[10000];

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	HPEN hOldPen;

	int i;
	static HPEN hPen;
	static HMENU hMenu, hSubMenu;
	static char type = 0, lineType = 0;
	static int count = 0;
	static int oldX = 0, oldY = 0, nowX = 0, nowY = 0;
	static bool bDraw = false;

	switch(iMsg) {
	case WM_CREATE:
		hMenu = GetMenu(hWnd);
		hSubMenu = GetSubMenu(hMenu, 0);
		CheckMenuRadioItem(hSubMenu, 0, 2, 0, MF_BYPOSITION);
		hSubMenu = GetSubMenu(hMenu, 1);
		CheckMenuRadioItem(hSubMenu, 0, 2, 0, MF_BYPOSITION);
		hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		for(i=0; i<count; i++) {
			if(saveDraw[i].lineType == 0)
				hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			else if(saveDraw[i].lineType == 1)
				hPen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
			else if(saveDraw[i].lineType == 2) 
				hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
			else if(saveDraw[i].lineType == 3)
				hPen = CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
			else if(saveDraw[i].lineType == 4) 
				hPen = CreatePen(PS_DASHDOTDOT, 1, RGB(0, 0, 0));

			SelectObject(hDC, hPen);
			if(saveDraw[i].type <= 1) {
				MoveToEx(hDC, saveDraw[i].x1, saveDraw[i].y1, NULL);
				LineTo(hDC, saveDraw[i].x2, saveDraw[i].y2);
			} else if(saveDraw[i].type == 2) {
				Ellipse(hDC, saveDraw[i].x1, saveDraw[i].y1, saveDraw[i].x2, saveDraw[i].y2);
			} else if(saveDraw[i].type == 3) {
				Rectangle(hDC, saveDraw[i].x1, saveDraw[i].y1, saveDraw[i].x2, saveDraw[i].y2);
			}
		}
		EndPaint(hWnd, &ps);

		if(lineType == 0)
			hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		else if(lineType == 1)
			hPen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
		else if(lineType == 2)
			hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
		else if(lineType == 3)
			hPen = CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
		else if(lineType == 4)
			hPen = CreatePen(PS_DASHDOTDOT, 1, RGB(0, 0, 0));
		break;
	case WM_LBUTTONDOWN:
		bDraw = true;
		nowX = oldX = LOWORD(lParam);
		nowY = oldY = HIWORD(lParam);
		break;
	case WM_MOUSEMOVE:
		if(bDraw) {
			hDC = GetDC(hWnd);
			hOldPen = (HPEN)SelectObject(hDC, hPen);
			if(type == 0) {
				nowX = LOWORD(lParam);
				nowY = HIWORD(lParam);
				MoveToEx(hDC, oldX, oldY, NULL);
				LineTo(hDC, nowX, nowY);

				saveDraw[count].x1 = oldX;
				saveDraw[count].y1 = oldY;
				saveDraw[count].x2 = nowX;
				saveDraw[count].y2 = nowY;
				saveDraw[count].type = type;
				saveDraw[count++].lineType = lineType;
	
				oldX = nowX;
				oldY = nowY;
			} else if(type == 1) {
				SetROP2(hDC, R2_NOT);
				MoveToEx(hDC, oldX, oldY, NULL);
				LineTo(hDC, nowX, nowY);
				nowX = LOWORD(lParam);
				nowY = HIWORD(lParam);
				MoveToEx(hDC, oldX, oldY, NULL);
				LineTo(hDC, nowX, nowY);
			} else if(type == 2) {
				SetROP2(hDC, R2_NOT);
				Ellipse(hDC, oldX, oldY, nowX, nowY);
				nowX = LOWORD(lParam);
				nowY = HIWORD(lParam);
				Ellipse(hDC, oldX, oldY, nowX, nowY);
			} else if(type == 3) {
				SetROP2(hDC, R2_NOT);
				Rectangle(hDC, oldX, oldY, nowX, nowY);
				nowX = LOWORD(lParam);
				nowY = HIWORD(lParam);
				Rectangle(hDC, oldX, oldY, nowX, nowY);
			}
			SelectObject(hDC, hOldPen);
			ReleaseDC(hWnd, hDC);
		}
		break;
	case WM_LBUTTONUP:
		bDraw = false;
		hDC = GetDC(hWnd);
		hOldPen = (HPEN)SelectObject(hDC, hPen);
		if(type == 1) {
			MoveToEx(hDC, oldX, oldY, NULL);
			LineTo(hDC, nowX, nowY);
		} else if(type == 2) {
			Ellipse(hDC, oldX, oldY, nowX, nowY);
		} else if(type == 3) {
			Rectangle(hDC, oldX, oldY, nowX, nowY);
		}
		SelectObject(hDC, hOldPen);
		ReleaseDC(hWnd, hDC);

		saveDraw[count].x1 = oldX;
		saveDraw[count].y1 = oldY;
		saveDraw[count].x2 = nowX;
		saveDraw[count].y2 = nowY;
		saveDraw[count].type = type;
		saveDraw[count++].lineType = lineType;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case ID_DRAW_POINT:
			type = 0;
			hSubMenu = GetSubMenu(hMenu, 0);
			CheckMenuRadioItem(hSubMenu, 0, 3, 0, MF_BYPOSITION);
			break;
		case ID_DRAW_LINE:
			type = 1;
			hSubMenu = GetSubMenu(hMenu, 0);
			CheckMenuRadioItem(hSubMenu, 0, 3, 1, MF_BYPOSITION);
			break;
		case ID_DRAW_CIRCLE:
			type = 2;
			hSubMenu = GetSubMenu(hMenu, 0);
			CheckMenuRadioItem(hSubMenu, 0, 3, 2, MF_BYPOSITION);
			break;
		case ID_DRAW_RECTANGLE:
			type = 3;
			hSubMenu = GetSubMenu(hMenu, 0);
			CheckMenuRadioItem(hSubMenu, 0, 3, 3, MF_BYPOSITION);
			break;
		case ID_LINE_SOLID:
			lineType = 0;
			hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			hSubMenu = GetSubMenu(hMenu, 1);
			CheckMenuRadioItem(hSubMenu, 0, 4, 0, MF_BYPOSITION);
			break;
		case ID_LINE_DASH:
			lineType = 1;
			hPen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
			hSubMenu = GetSubMenu(hMenu, 1);
			CheckMenuRadioItem(hSubMenu, 0, 4, 1, MF_BYPOSITION);
			break;
		case ID_LINE_DOT:
			lineType = 2;
			hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
			hSubMenu = GetSubMenu(hMenu, 1);
			CheckMenuRadioItem(hSubMenu, 0, 4, 2, MF_BYPOSITION);
			break;
		case ID_LINE_DASHDOT:
			lineType = 3;
			hPen = CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
			hSubMenu = GetSubMenu(hMenu, 1);
			CheckMenuRadioItem(hSubMenu, 0, 4, 3, MF_BYPOSITION);
			break;
		case ID_LINE_DASHDOTDOT:
			lineType = 4;
			hPen = CreatePen(PS_DASHDOTDOT, 1, RGB(0, 0, 0));
			hSubMenu = GetSubMenu(hMenu, 1);
			CheckMenuRadioItem(hSubMenu, 0, 4, 4, MF_BYPOSITION);
			break;
		}
		break;
	case WM_DESTROY:
		DeleteObject(hPen);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}