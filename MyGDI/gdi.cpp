#include <windows.h>
#include <windowsx.h>
#include "resource.h"

//函数声明
BOOL InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WinProcGame(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL MyPolyline(HDC hdc, const POINT* lppt, int cPoints);
BOOL MyPolyPolyline(HDC hdc, const POINT* lppt, const DWORD *lpdwPolyPoints, int cPoints);
BOOL MyPolyPolygon(HDC hdc, const POINT* lppt, const int *lpdwPolyPoints, int cPoints);

int WINAPI WinMain(HINSTANCE hInstance,         //当前实例句柄
	HINSTANCE hPrevInstance,     //前一个实例句柄
	PSTR szCmdLine,              //命令行字符
	int iCmdShow)                //窗口显示方式
{
	MSG msg;

	if (!InitWindow(hInstance, iCmdShow))
		return FALSE;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

BOOL InitWindow(HINSTANCE hInstance, int iCmdShow)
{
	TCHAR szAppName[] = TEXT("EasyWin");		  //应用程序名称
	HWND hwnd;                                    //窗口句柄
	WNDCLASS wcMainWnd;                           //窗口类结构
	ATOM a;

	wcMainWnd.style = CS_VREDRAW | CS_HREDRAW;
	wcMainWnd.lpfnWndProc = WinProcGame;
	wcMainWnd.cbClsExtra = 0;
	wcMainWnd.cbWndExtra = 0;
	wcMainWnd.hInstance = hInstance;
	wcMainWnd.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcMainWnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcMainWnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcMainWnd.lpszMenuName = NULL;
	wcMainWnd.lpszClassName = szAppName;

	a = RegisterClass(&wcMainWnd);

	if (!a)
	{
		MessageBox(NULL, TEXT("注册窗口类失败！"), szAppName,
			MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, 						//窗口类名称
		TEXT("Windows GDI"),  //窗口标题
		WS_OVERLAPPEDWINDOW,							//窗口风格
		100,											//窗口位置的x坐标
		100,											//窗口位置的y坐标
		400,											//窗口的宽度
		300,											//窗口的高度
		NULL,											//父窗口句柄
		LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)),//菜单句柄
		hInstance,										//应用程序实例句柄
		NULL);											//窗口创建数据指针

	if (!hwnd) return FALSE;

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}

LRESULT CALLBACK WinProcGame(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc;
	PAINTSTRUCT ps;
	RECT        rect;
	LOGPEN        logpen;
	HMENU         hMenu;
	static HMENU  hPopMenu;
	static int    iSelectionFunc = IDM_FUNC_LINETO ;
	static int    iSelectionPen = IDM_PEN_BLACK ;
	static int    iSelectionBrush = IDM_BRUSH_WHITE ;
	static HPEN   hRedDashdotPen, hBlueSolidPen;
	HPEN          hOldPen;
	HBRUSH        hNewBrush = RGB(0, 0, 0), hOldBrush = RGB(0, 0, 0);
	POINT         ptDiamond[7] = {50, 50, 100, 50, 100, 100, 50, 50, 75, 75, 100, 50, 50, 100}, pt;
	DWORD		  lpps[3] = {3, 2, 2};

	switch (message)
	{
	case WM_CREATE:
		//创建红色点划线画笔
		hRedDashdotPen = CreatePen (PS_DASHDOT, 1, RGB (255, 0, 0));			

		//创建宽度为3的蓝色画笔
		logpen.lopnStyle = PS_SOLID;
		logpen.lopnWidth.x = 3;
		logpen.lopnColor = RGB (0, 0, 255); 
		hBlueSolidPen = CreatePenIndirect (&logpen);

		//获取快捷菜单句柄
		hPopMenu = LoadMenu (((LPCREATESTRUCT)lParam)->hInstance,
			TEXT ("PopMenu"));
		//再获取 hPopMenu 标识的快捷菜单的唯一的菜单便笺句柄
		hPopMenu = GetSubMenu (hPopMenu, 0);
		return 0;

	case WM_COMMAND:
		hMenu = GetMenu (hWnd) ;

		switch (LOWORD (wParam))
		{
		case IDM_FILE_EXIT:
			SendMessage (hWnd, WM_CLOSE, 0, 0) ;
			return 0 ;

		case IDM_FUNC_LINETO:
		case ID_POLYLINE_POLYLINE:
		case ID_POLYLINE_POLYPOLYLINE:
		case ID_POLYLINE_POLYGON:
		case ID_POLYLINE_POLYPOLYGEN:
		case IDM_FUNC_ARC:
		case IDM_FUNC_RECTANGLE:
		case IDM_FUNC_ELLIPSE:
		case IDM_FUNC_ROUNDRECT:
		case IDM_FUNC_CHORD:
		case IDM_FUNC_PIE:
		case IDM_FUNC_POLYGON:
		case ID_MYPOLY_MYPOLYLINE:
		case ID_MYPOLY_MYPOLYPOLYLINE:
			CheckMenuItem (hMenu, iSelectionFunc, MF_UNCHECKED) ;
			iSelectionFunc = LOWORD (wParam) ;
			CheckMenuItem (hMenu, iSelectionFunc, MF_CHECKED) ;

			InvalidateRect (hWnd, NULL, TRUE) ;
			return 0 ;

		case IDM_PEN_BLACK:
		case IDM_PEN_REDDASHDOT:
		case IDM_PEN_BLUESOLID: 

			CheckMenuItem (hMenu, iSelectionPen, MF_UNCHECKED) ;
			iSelectionPen = LOWORD (wParam) ;
			CheckMenuItem (hMenu, iSelectionPen, MF_CHECKED) ;

			InvalidateRect (hWnd, NULL, TRUE) ;
			return 0 ;

		case IDM_BRUSH_WHITE:
		case IDM_BRUSH_LTGRAY:
		case IDM_BRUSH_COLORSOLID: 
		case IDM_BRUSH_CROSS:

			CheckMenuItem (hMenu, iSelectionBrush, MF_UNCHECKED) ;
			iSelectionBrush = LOWORD (wParam) ;
			CheckMenuItem (hMenu, iSelectionBrush , MF_CHECKED) ;

			InvalidateRect (hWnd, NULL, TRUE) ;
			return 0 ;

		case IDM_ABOUT:
			MessageBox (hWnd, TEXT ("基本GDI程序：基本绘图函数以及画笔、画刷对象的使用"),
				TEXT ("基本GDI程序"), MB_OK | MB_ICONINFORMATION) ;
			return 0 ;
		}


	case WM_RBUTTONUP:
		pt.x = LOWORD (lParam);
		pt.y = HIWORD (lParam);
		ClientToScreen (hWnd, &pt);
		TrackPopupMenu (hPopMenu,                            //快捷菜单句柄
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, //标志选项
			pt.x, pt.y, 0, hWnd, NULL);
		return 0;

	case WM_PAINT:
		hdc=BeginPaint (hWnd, &ps);
		GetClientRect (hWnd, &rect);

		//选用新画笔
		switch (iSelectionPen)
		{
		case IDM_PEN_BLACK:
			hOldPen = (HPEN)SelectObject (hdc, GetStockObject(BLACK_PEN));
			break;
		case IDM_PEN_REDDASHDOT:
			hOldPen = (HPEN)SelectObject (hdc, hRedDashdotPen);
			break;
		case IDM_PEN_BLUESOLID:
			hOldPen = (HPEN)SelectObject (hdc, hBlueSolidPen);
			break;
		}

		//选用新画刷
		switch (iSelectionBrush)
		{
		case IDM_BRUSH_WHITE:
			hOldBrush = (HBRUSH)SelectObject (hdc, GetStockObject(WHITE_BRUSH));
			break;
		case IDM_BRUSH_LTGRAY:
			hOldBrush = (HBRUSH)SelectObject (hdc, GetStockObject(LTGRAY_BRUSH));
			break;
		case IDM_BRUSH_COLORSOLID:
			//创建并选择彩色实体画刷
			hNewBrush = CreateSolidBrush (RGB (0, 255, 255));
			hOldBrush = (HBRUSH)SelectObject (hdc, hNewBrush);
			break;
		case IDM_BRUSH_CROSS:
			//创建并选择绿色十字影线画刷
			hNewBrush = CreateHatchBrush (HS_CROSS, RGB (0, 255, 0));
			hOldBrush = (HBRUSH)SelectObject (hdc, hNewBrush);
			break;
		}

		//调用相应的GDI绘图函数绘制演示图形
		switch (iSelectionFunc)
		{
		case IDM_FUNC_LINETO:
			MoveToEx (hdc, 0, rect.bottom / 2, NULL);
			LineTo (hdc, rect.right, rect.bottom / 2);
			MoveToEx (hdc, rect.right / 2, 0, NULL);
			LineTo (hdc, rect.right / 2, rect.bottom);
			break;

		case ID_POLYLINE_POLYLINE:
			Polyline (hdc, ptDiamond, 7);
			break;

		case ID_POLYLINE_POLYPOLYLINE:
			PolyPolyline(hdc, ptDiamond, lpps, 3);
			break;

		case ID_POLYLINE_POLYGON:
			Polygon(hdc, ptDiamond, 7);
			break;

		case ID_POLYLINE_POLYPOLYGEN:
			// PolyPolygon(hdc, ptDiamond, (int*)lpps, 3);
			MyPolyPolygon(hdc, ptDiamond, (int*)lpps, 3);
			break;

		case ID_MYPOLY_MYPOLYLINE:
			MyPolyline (hdc, ptDiamond, 7);
			break;

		case ID_MYPOLY_MYPOLYPOLYLINE:
			MyPolyPolyline(hdc, ptDiamond, lpps, 3);
			break;

		case IDM_FUNC_ARC:
			Arc (hdc, rect.left, rect.top, rect.right, rect.bottom,
				rect.right, 3 * rect.bottom / 4,
				rect.left, rect.bottom / 4);
			break;

		case IDM_FUNC_RECTANGLE:
			Rectangle (hdc, rect.right / 4, rect.bottom / 4, 
				3 * rect.right / 4, 3 * rect.bottom / 4);
			break;

		case IDM_FUNC_ELLIPSE:
			Ellipse (hdc, rect.right / 4, rect.bottom / 4, 
				3 * rect.right / 4, 3 * rect.bottom / 4);
			break;

		case IDM_FUNC_ROUNDRECT:
			RoundRect (hdc, rect.right / 4, rect.bottom / 4, 
				3 * rect.right / 4, 3 * rect.bottom / 4,
				rect.right / 4, rect.bottom / 4);
			break;

		case IDM_FUNC_CHORD:
			Chord (hdc, rect.left, rect.top, rect.right, rect.bottom,
				rect.right, 3 * rect.bottom / 4,
				rect.right / 2, rect.top);
			break;

		case IDM_FUNC_PIE:
			Pie (hdc, rect.left, rect.top, rect.right, rect.bottom,
				rect.right, 3 * rect.bottom / 4,
				rect.right / 2, rect.top);
			break;

		case IDM_FUNC_POLYGON:
			if (GetPolyFillMode(hdc)==WINDING)
				SetPolyFillMode(hdc, ALTERNATE);
			else
				SetPolyFillMode(hdc, WINDING);

			ptDiamond[0].x = 0;
			ptDiamond[0].y = rect.bottom / 3;

			ptDiamond[1].x = rect.right;
			ptDiamond[1].y = rect.bottom / 3;

			ptDiamond[2].x = rect.right / 4;
			ptDiamond[2].y = rect.bottom;

			ptDiamond[3].x = rect.right / 2;
			ptDiamond[3].y = 0;

			ptDiamond[4].x = 3 * rect.right / 4;
			ptDiamond[4].y = rect.bottom;

			Polygon (hdc, ptDiamond, 5);
			break;

		}

		//画笔、画刷还原
		SelectObject (hdc, hOldPen);
		SelectObject (hdc, hOldBrush);

		//删除自建画刷
		DeleteObject (hNewBrush);

		EndPaint ( hWnd, &ps ); 
		return 0;

	case WM_DESTROY:      
		//删除自建画笔
		DeleteObject (hRedDashdotPen);
		DeleteObject (hBlueSolidPen);

		PostQuitMessage (0); 
		return 0;
	}

	return DefWindowProc (hWnd, message, wParam, lParam);

} //函数 WinProc 结束

