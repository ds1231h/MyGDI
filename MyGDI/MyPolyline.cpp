#include <windows.h>
#include <windowsx.h>

BOOL MyPolyline(HDC hdc, const POINT* lppt, int cPoints)
{
	MoveToEx(hdc, lppt[0].x, lppt[0].y, NULL);
	for (int i = 1; i < cPoints; i++)
	{
		LineTo(hdc, lppt[i].x, lppt[i].y);
	}
	return TRUE;
}

BOOL MyPolyPolyline(HDC hdc, const POINT* lppt, const DWORD *lpdwPolyPoints, int cPoints)
{
	for(int i = 0; i < cPoints; i++)
	{
		Polyline(hdc, lppt, lpdwPolyPoints[i]);
		lppt += lpdwPolyPoints[i];
	}
	return TRUE;
}

BOOL MyPolyPolygon(HDC hdc, const POINT* lppt, const int *lpdwPolyPoints, int cPoints)
{
	for(int i = 0; i < cPoints; i++)
	{
		Polygon(hdc, lppt, lpdwPolyPoints[i]);
		lppt += lpdwPolyPoints[i];
	}
	return TRUE;
}