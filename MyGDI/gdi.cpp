#include <windows.h>
#include <windowsx.h>
#include "resource.h"

//��������
BOOL InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WinProcGame(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL MyPolyline(HDC hdc, const POINT* lppt, int cPoints);
BOOL MyPolyPolyline(HDC hdc, const POINT* lppt, const DWORD *lpdwPolyPoints, int cPoints);
BOOL MyPolyPolygon(HDC hdc, const POINT* lppt, const int *lpdwPolyPoints, int cPoints);

int WINAPI WinMain(HINSTANCE hInstance,         //��ǰʵ�����
	HINSTANCE hPrevInstance,     //ǰһ��ʵ�����
	PSTR szCmdLine,              //�������ַ�
	int iCmdShow)                //������ʾ��ʽ
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
	TCHAR szAppName[] = TEXT("EasyWin");		  //Ӧ�ó�������
	HWND hwnd;                                    //���ھ��
	WNDCLASS wcMainWnd;                           //������ṹ
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
		MessageBox(NULL, TEXT("ע�ᴰ����ʧ�ܣ�"), szAppName,
			MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, 						//����������
		TEXT("Windows GDI"),  //���ڱ���
		WS_OVERLAPPEDWINDOW,							//���ڷ��
		100,											//����λ�õ�x����
		100,											//����λ�õ�y����
		400,											//���ڵĿ��
		300,											//���ڵĸ߶�
		NULL,											//�����ھ��
		LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)),//�˵����
		hInstance,										//Ӧ�ó���ʵ�����
		NULL);											//���ڴ�������ָ��

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
		//������ɫ�㻮�߻���
		hRedDashdotPen = CreatePen (PS_DASHDOT, 1, RGB (255, 0, 0));			

		//�������Ϊ3����ɫ����
		logpen.lopnStyle = PS_SOLID;
		logpen.lopnWidth.x = 3;
		logpen.lopnColor = RGB (0, 0, 255); 
		hBlueSolidPen = CreatePenIndirect (&logpen);

		//��ȡ��ݲ˵����
		hPopMenu = LoadMenu (((LPCREATESTRUCT)lParam)->hInstance,
			TEXT ("PopMenu"));
		//�ٻ�ȡ hPopMenu ��ʶ�Ŀ�ݲ˵���Ψһ�Ĳ˵������
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
			MessageBox (hWnd, TEXT ("����GDI���򣺻�����ͼ�����Լ����ʡ���ˢ�����ʹ��"),
				TEXT ("����GDI����"), MB_OK | MB_ICONINFORMATION) ;
			return 0 ;
		}


	case WM_RBUTTONUP:
		pt.x = LOWORD (lParam);
		pt.y = HIWORD (lParam);
		ClientToScreen (hWnd, &pt);
		TrackPopupMenu (hPopMenu,                            //��ݲ˵����
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, //��־ѡ��
			pt.x, pt.y, 0, hWnd, NULL);
		return 0;

	case WM_PAINT:
		hdc=BeginPaint (hWnd, &ps);
		GetClientRect (hWnd, &rect);

		//ѡ���»���
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

		//ѡ���»�ˢ
		switch (iSelectionBrush)
		{
		case IDM_BRUSH_WHITE:
			hOldBrush = (HBRUSH)SelectObject (hdc, GetStockObject(WHITE_BRUSH));
			break;
		case IDM_BRUSH_LTGRAY:
			hOldBrush = (HBRUSH)SelectObject (hdc, GetStockObject(LTGRAY_BRUSH));
			break;
		case IDM_BRUSH_COLORSOLID:
			//������ѡ���ɫʵ�廭ˢ
			hNewBrush = CreateSolidBrush (RGB (0, 255, 255));
			hOldBrush = (HBRUSH)SelectObject (hdc, hNewBrush);
			break;
		case IDM_BRUSH_CROSS:
			//������ѡ����ɫʮ��Ӱ�߻�ˢ
			hNewBrush = CreateHatchBrush (HS_CROSS, RGB (0, 255, 0));
			hOldBrush = (HBRUSH)SelectObject (hdc, hNewBrush);
			break;
		}

		//������Ӧ��GDI��ͼ����������ʾͼ��
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

		//���ʡ���ˢ��ԭ
		SelectObject (hdc, hOldPen);
		SelectObject (hdc, hOldBrush);

		//ɾ���Խ���ˢ
		DeleteObject (hNewBrush);

		EndPaint ( hWnd, &ps ); 
		return 0;

	case WM_DESTROY:      
		//ɾ���Խ�����
		DeleteObject (hRedDashdotPen);
		DeleteObject (hBlueSolidPen);

		PostQuitMessage (0); 
		return 0;
	}

	return DefWindowProc (hWnd, message, wParam, lParam);

} //���� WinProc ����

