// Сыгинь_4.cpp : Определяет точку входа для приложения.
//


#define bitmap1 131
#define cursor1 132
#define cursor2 133
#define icon1 129
#define icon2 130

#define Menu1 134

#define LAUNCH_1 32798

#include "framework.h"
#include "Сыгинь_4.h"
#include <commdlg.h>

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD WINAPI ThreadFunc1(PVOID pvParam);

DWORD WINAPI ThreadFunc2(PVOID pvParam);

HINSTANCE hInst;

TCHAR ClassName1[] = "Windows Class";
TCHAR AppTitle1[] = "Главное окно группа АСУ-19  Сыгинь";

TCHAR ClassName2[] = "PopupWindow Class";
TCHAR AppTitle2[] = " Временное окно";

HWND hMainWnd, hWndPopup;

WNDCLASS wc_main, wc_popup;
HBRUSH FIRST_BRUSH, SECOND_BRUSH,hBrush;
HICON hIcon1;
HCURSOR hCursor1;
// --- Главная функция приложения WinMain

struct PARAM {
	
	BOOL stop;
	HWND hWnd;
};

HANDLE hThread1, hThread2;
LPDWORD ThreadId1, ThreadId2;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{


	FIRST_BRUSH = CreateSolidBrush(RGB(255, 255, 255));

	SECOND_BRUSH = CreateSolidBrush(RGB(255, 255, 255));

	MSG msg;
	
	int m_width, m_height;
	m_width = GetSystemMetrics(SM_CXFULLSCREEN);
	m_height = GetSystemMetrics(SM_CYFULLSCREEN);


	HMENU hMenu1 = LoadMenu(hInst, MAKEINTRESOURCE(Menu1));


	if ((hMainWnd = FindWindow(ClassName1, NULL)) != NULL)
	{

		int msgID1 = MessageBox(hMainWnd, "Приложение уже запущено хотите продолжить?", "Информация", MB_OKCANCEL);
		switch (msgID1)
		{
		case IDOK:

			if (IsIconic(hMainWnd))
			{


				ShowWindow(hMainWnd, SW_RESTORE);

			}
			break;

		case IDCANCEL:
			PostQuitMessage(0);
			DestroyWindow(hMainWnd);
			break;
		}
		
		
		return FALSE;
	}

	hInst = hInstance; // Сохран. дескриптора экз. прилож.
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(icon2));
	HCURSOR	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(cursor1));
	ZeroMemory(&wc_main, sizeof(wc_main)); // Очистка полей структуры
	// Заполнение полей стр-ры WNDCLASS
	wc_main.lpszClassName = ClassName1; // Имя класса окон
	//Адрес оконной ф-ции
	wc_main.lpfnWndProc = (WNDPROC)MainWndProc;
	wc_main.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // Стиль класса окон
	wc_main.hInstance = hInstance; // Экземпляр приложения
	// Пиктограмма для окон
	wc_main.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(icon2));
	// Курсор мыши для окон
	wc_main.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(cursor1));
	// Кисть для окон
	wc_main.hbrBackground = FIRST_BRUSH;
	wc_main.lpszMenuName = NULL; // Ресурс меню окон
	wc_main.cbClsExtra = 0; // Дополнительная память класса
	wc_main.cbWndExtra = 0; // Дополнительная память окна
	// Pегистрация класса окна.
	RegisterClass(&wc_main);
	// Создание главного окна приложения.
	hMainWnd = CreateWindow(
		ClassName1, // Имя класса окон
		AppTitle1, // Заголовок окна
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, // Стиль окна
		0, // X-координата
		0, // Y-координата
		m_width, // Ширина окна
		m_height, // Высота окна
		NULL, // Дескриптор окна-родителя
		NULL, // Дескриптор меню окна
		hInst, // Дескриптор экз. приложения
		NULL); // Дополнительная информация


	if (!hMainWnd)
	{
		// Окно не создано, выдается сообщение.
		MessageBox(NULL, "Window create error", AppTitle1, MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	// Отображение окна и обновление его
	ShowWindow(hMainWnd, SW_NORMAL);
	UpdateWindow(hMainWnd);

	SetMenu(hMainWnd, hMenu1);

	// Запуск цикла обработки очереди сообщений.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// Преобразов. сообщений клавиатуры в символьные
		TranslateMessage(&msg);
		// Отправление сообщения оконной функции
		DispatchMessage(&msg);
	}
	return msg.wParam; // Завершение работы приложения

}

static struct PARAM p1,p2;
static int x = 0, y = 0;
static int widthH, heightH;
RECT rect1,rect2;
static OPENFILENAME ofn;
 STARTUPINFO si;
 PROCESS_INFORMATION pi;
 char szFileName[260];

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

{
	PAINTSTRUCT ps;
	HANDLE h1;
	HDC hDC,BitmapDC,hDCPOP;
	int cx, cy, width, height;
	static HBITMAP hBitmap,hOldBitmap;
	BITMAP bmp;
	TEXTMETRIC tm;
	int Priority, ClassPrior = 0;
	char str1_1[40], str1_2[40] = "Приоритет потока № 1  =  ";
	char  str2_1[40], str2_2[40] = "Приоритет потока № 2 = ";
	GetClientRect(hWnd, &rect1);
	hDC = GetDC(hWnd);
	hDCPOP = GetDC(hWndPopup);
	
	switch (uMsg)
	{
		
		case WM_CREATE:
			hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(bitmap1));

			GetObject(hBitmap, sizeof(bmp), &bmp);
			widthH = bmp.bmWidth;
			heightH = bmp.bmHeight;

			cx = GetSystemMetrics(SM_CXSCREEN);
			cy = GetSystemMetrics(SM_CYSCREEN);

			hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(icon1));
			hCursor1 = LoadCursor(hInst, MAKEINTRESOURCE(cursor2));
			ZeroMemory(&wc_popup, sizeof(wc_popup)); // Очистка полей структуры
			// Заполнение полей стр-ры WNDCLASS
			wc_popup.lpszClassName = ClassName2; // Имя класса окон
			//Адрес оконной ф-ции
			wc_popup.lpfnWndProc = (WNDPROC)PopupWndProc;
			wc_popup.style = CS_HREDRAW | CS_VREDRAW; // Стиль класса окон
			wc_popup.hInstance = hInst; // Экземпляр приложения
			// Пиктограмма для окон
			wc_popup.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(icon1));
			// Курсор мыши для окон
			wc_popup.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(cursor2));
			// Кисть для окон
			wc_popup.hbrBackground = SECOND_BRUSH;
			wc_popup.lpszMenuName = NULL; // Ресурс меню окон
			wc_popup.cbClsExtra = 0; // Дополнительная память класса
			wc_popup.cbWndExtra = 0; // Дополнительная память окна
			// Pегистрация класса окна.
			RegisterClass(&wc_popup);
			hWndPopup = CreateWindow(ClassName2,
			AppTitle2, WS_POPUPWINDOW | WS_CAPTION |
				WS_MINIMIZEBOX |
				WS_MAXIMIZEBOX | WS_BORDER, 200, 0, 600, 600,
				hMainWnd, NULL, hInst, NULL);
		
			if (hWndPopup == NULL)
			{ // Уничтожение главного окна при неудаче
				DestroyWindow(hMainWnd);
				return FALSE;
			}
			UpdateWindow(hWndPopup);
				break;

			case WM_SIZE:
		
				width = LOWORD(lParam);
				height = HIWORD(lParam);	
				break;
		
		
		
		
			case WM_PAINT:
			{
				hDC = BeginPaint(hMainWnd, &ps);
				if (p1.stop != 1)
				{
					BitmapDC = CreateCompatibleDC(hDC);
					hOldBitmap = (HBITMAP)SelectObject(BitmapDC, hBitmap);

					int iHorSize = GetDeviceCaps(BitmapDC, HORZSIZE);
					int iVerSize = GetDeviceCaps(BitmapDC, VERTSIZE);
					BitBlt(hDC, x, y, iHorSize, iVerSize, BitmapDC, 0, 0, SRCCOPY);
					SelectObject(BitmapDC, hOldBitmap);
					DeleteDC(BitmapDC);
				}
				EndPaint(hMainWnd, &ps);
			}
				break;
		
			case WM_COMMAND:
			{
				UINT idItem = LOWORD(wParam);
				switch (idItem)
				{
		
				case LAUNCH_1:
					p1.stop = 0;
					if (p1.stop == 0) 
					{
						hThread1 = CreateThread(NULL, 0, ThreadFunc1, (PVOID)(&p1), CREATE_SUSPENDED, ThreadId1);
						ResumeThread(hThread1);
						
					}
					break;
		
				case STOP_1:
					SuspendThread(hThread1);
					MessageBox(hWnd, "Вы приостановили работу Потока №1", "Сообщение", MB_OK);
					break;
		
				case RESUME_1:
					ResumeThread(hThread1);
					MessageBox(hWnd, "Вы возобновили работу Потока №1", "Сообщение", MB_OK);
					break;
		
		
				case CLOSE_THREAD_1:
					p1.stop = 1;
					MessageBox(hWnd, "Вы завершили работу Потока №1", "Сообщение", MB_OK);
					break;
		
				case IDLE_1:
					SuspendThread(hThread1);
					SetThreadPriority(hThread1, THREAD_PRIORITY_IDLE);
					ResumeThread(hThread1);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority = GetThreadPriority(hThread1);
					Priority += ClassPrior;
					GetTextMetrics(hDC, &tm);
					_itoa_s(Priority, str1_1, 10);
					strcat_s(str1_2, str1_1);
					MessageBox(hWnd, str1_2, "Приоритет потока", MB_OK);
					break;
		
				case LOWEST_1:			
					SuspendThread(hThread1);
					SetThreadPriority(hThread1, THREAD_PRIORITY_LOWEST);
					ResumeThread(hThread1);
					Priority = GetThreadPriority(hThread1);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDC, &tm);
					_itoa_s(Priority, str1_1, 10);
					strcat_s(str1_2, str1_1);
					MessageBox(hWnd, str1_2, "Приоритет потока", MB_OK);
					break;
		
		
				case BELOW_NORMAL_1:
					SuspendThread(hThread1);
					SetThreadPriority(hThread1, THREAD_PRIORITY_BELOW_NORMAL);
					ResumeThread(hThread1);
					Priority = GetThreadPriority(hThread1);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDC, &tm);
					_itoa_s(Priority, str1_1, 10);
					strcat_s(str1_2, str1_1);
					MessageBox(hWnd, str1_2, "Приоритет потока", MB_OK);
					break;
		
				case NORMAL_1:
					SuspendThread(hThread1);
					SetThreadPriority(hThread1, THREAD_PRIORITY_NORMAL);
					ResumeThread(hThread1);
					Priority = GetThreadPriority(hThread1);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDC, &tm);
					_itoa_s(Priority, str1_1, 10);
					strcat_s(str1_2, str1_1);
					MessageBox(hWnd, str1_2, "Приоритет потока", MB_OK);
					break;
		
				case ABOVE_NORMAL_1:
					SuspendThread(hThread1);
					SetThreadPriority(hThread1, THREAD_PRIORITY_ABOVE_NORMAL);
					ResumeThread(hThread1);
					Priority = GetThreadPriority(hThread1);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDC, &tm);
					_itoa_s(Priority, str1_1, 10);
					strcat_s(str1_2, str1_1);
					MessageBox(hWnd, str1_2, "Приоритет потока", MB_OK);
					break;
		
				case HIGHEST_1:
					SuspendThread(hThread1);
					SetThreadPriority(hThread1, THREAD_PRIORITY_HIGHEST);
					ResumeThread(hThread1);
					Priority = GetThreadPriority(hThread1);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDC, &tm);
					_itoa_s(Priority, str1_1, 10);
					strcat_s(str1_2, str1_1);
					MessageBox(hWnd, str1_2, "Приоритет потока", MB_OK);
					break;
		
				case TIME_CRITICAL_1:
					SuspendThread(hThread1);
					SetThreadPriority(hThread1, THREAD_PRIORITY_TIME_CRITICAL);
					ResumeThread(hThread1);
					Priority = GetThreadPriority(hThread1);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDC, &tm);
					_itoa_s(Priority, str1_1, 10);
					strcat_s(str1_2, str1_1);
					MessageBox(hWnd, str1_2, "Приоритет потока", MB_OK);
					break;
		
		
				case LAUNCH_2:
					p2.stop = 0;
					if (p2.stop == 0)
					{
						hThread2 = CreateThread(NULL, 0, ThreadFunc2, (PVOID)(&p2), CREATE_SUSPENDED, ThreadId2);
						ResumeThread(hThread2);

						ShowWindow(hWndPopup, SW_RESTORE);
					}
					
					break;
		
				case STOP_2:
					SuspendThread(hThread2);
					MessageBox(hWnd, "Вы остановили работу Потока №2", "Сообщение", MB_OK);
					break;
				case RESUME_2:
					ResumeThread(hThread2);
					MessageBox(hWnd, "Вы возобновили работу Потока №2", "Сообщение", MB_OK);
					break;
				case CLOSE_THREAD_2:
					p2.stop = 1;
					MessageBox(hWnd, "Вы завершили работу Потока №2", "Сообщение", MB_OK);
					break;
				case IDLE_2:
					SuspendThread(hThread2);
					SetThreadPriority(hThread2, THREAD_PRIORITY_IDLE);
					ResumeThread(hThread2);
					Priority = GetThreadPriority(hThread2);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDCPOP, &tm);
					_itoa_s(Priority, str2_1, 10);
					strcat_s(str2_2, str2_1);
					MessageBox(hWnd, str2_2, "Приоритет потока", MB_OK);
					break;
		
				case LOWEST_2:
					SuspendThread(hThread2);
					SetThreadPriority(hThread2, THREAD_PRIORITY_LOWEST);
					ResumeThread(hThread2);
					Priority = GetThreadPriority(hThread2);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDCPOP, &tm);
					_itoa_s(Priority, str2_1, 10);
					strcat_s(str2_2, str2_1);
					MessageBox(hWnd, str2_2, "Приоритет потока", MB_OK);
					break;
		
		
				case BELOW_NORMAL_2:
					
					SuspendThread(hThread2);
					SetThreadPriority(hThread2, THREAD_PRIORITY_BELOW_NORMAL);
					ResumeThread(hThread2);
					Priority = GetThreadPriority(hThread2);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDCPOP, &tm);
					_itoa_s(Priority, str2_1, 10);
					strcat_s(str2_2, str2_1);
					MessageBox(hWnd, str2_2, "Приоритет потока", MB_OK);
					break;
		
				case NORMAL_2:
					SuspendThread(hThread2);
					SetThreadPriority(hThread2, THREAD_PRIORITY_NORMAL);
					ResumeThread(hThread2);
					Priority = GetThreadPriority(hThread2);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDCPOP, &tm);
					_itoa_s(Priority, str2_1, 10);
					strcat_s(str2_2, str2_1);
					MessageBox(hWnd, str2_2, "Приоритет потока", MB_OK);
					break;
		
				case ABOVE_NORMAL_2:
					SuspendThread(hThread2);
					SetThreadPriority(hThread2, THREAD_PRIORITY_ABOVE_NORMAL);
					ResumeThread(hThread2);
					Priority = GetThreadPriority(hThread2);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDCPOP, &tm);
					_itoa_s(Priority, str2_1, 10);
					strcat_s(str2_2, str2_1);
					MessageBox(hWnd, str2_2, "Приоритет потока", MB_OK);
					break;
		
				case HIGHEST_2:
					SuspendThread(hThread2);
					SetThreadPriority(hThread2, THREAD_PRIORITY_HIGHEST);
					ResumeThread(hThread2);
					Priority = GetThreadPriority(hThread2);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDCPOP, &tm);
					_itoa_s(Priority, str2_1, 10);
					strcat_s(str2_2, str2_1);
					MessageBox(hWnd, str2_2, "Приоритет потока", MB_OK);
					break;
		
				case TIME_CRITICAL_2:
					SuspendThread(hThread2);
					SetThreadPriority(hThread2, THREAD_PRIORITY_TIME_CRITICAL);
					ResumeThread(hThread2);
					Priority = GetThreadPriority(hThread2);
					h1 = GetCurrentProcess();
					ClassPrior = GetPriorityClass(h1);
					Priority += ClassPrior;
					GetTextMetrics(hDCPOP, &tm);
					_itoa_s(Priority, str2_1, 10);
					strcat_s(str2_2, str2_1);
					MessageBox(hWnd, str2_2, "Приоритет потока", MB_OK);
					break;
		
				case CHOOSE:
				
	
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hMainWnd;
					ofn.lpstrFile = szFileName;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFileName);
					ofn.lpstrFilter = "Все\0*.*\0Текстовые\0*.TXT\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		
					GetOpenFileName(&ofn);
				
				break;
		
				case LAUNCH_PROCESS:
		
		
					ZeroMemory(&si, sizeof(si)); 
					si.cb = sizeof(si);
		
					if (strlen(ofn.lpstrFile) == 0)
					{
						MessageBox(hMainWnd, "Выберите фалй", "Предупреждение", MB_OK | MB_ICONINFORMATION);
					}
					else {
						CreateProcess(NULL, ofn.lpstrFile, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
						return 0;
					}
					break;
		
				case EXIT:
					TerminateProcess(pi.hProcess, 0);
					DestroyWindow(hWnd);
		
					break;
				}
		
		
		
		
				}
			break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;




	default:

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
return 0;

}





// --- Оконная функция временное окно
static int width_ellips = 100, height_ellips = 100;
static int 	x1 = 0, x2=x1 + width_ellips, y1 = 210, y2 = y1 + height_ellips;


LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	
	int cx, cy;
	static int  width, height;
	
	hDC = GetDC(hWnd);
	GetClientRect(hWnd, &rect2);
	
	static int x3 = rect2.left;
	static int y3 = rect2.top;
	PAINTSTRUCT ps;



	switch (uMsg)
	{



	case WM_CREATE:
		cx = GetSystemMetrics(SM_CXSCREEN);
		cy = GetSystemMetrics(SM_CYSCREEN);
		
		break;

	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);


		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		if (p2.stop != 1)
		{
			SelectObject(hDC, hBrush);
			Ellipse(hDC, x1, y1, x2 , y2);
			EndPaint(hWnd, &ps);	

		}

		EndPaint(hWnd, &ps);
	}
	break;

	

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

return 0;

}



DWORD WINAPI ThreadFunc1(PVOID pvParam)
{
	DWORD dwResult = 0;
	static int k = 1;	
	struct PARAM* p = (struct PARAM*)pvParam;
	HDC hdc = GetDC(p->hWnd);
	PAINTSTRUCT ps;
	int x1, y1;
	x1 = GetSystemMetrics(SM_CXSCREEN);
	y1 = GetSystemMetrics(SM_CYSCREEN);
	while (!p->stop)
	{
		switch (k) {
		case 1:
			y = y + 30;
			if (y >= rect1.bottom - heightH-30) {
				k = k + 1;
			}
			break;

		case 2:
			x = x + 30;
			if (x >= rect1.right - widthH-30) {
				k = k + 1;
			}
			break;

		case 3:
			y = y - 30;
			if (y <= rect1.top) {
				k = k + 1;
			}
			break;

		case 4:
			x = x - 30;
			if (x <= rect1.left) {
				k = 1;
			}
			break;
		}

		InvalidateRect(hMainWnd, NULL, true);
		Sleep(500);

	}
	FillRect(hdc, &rect1, (HBRUSH)(COLOR_WINDOW + 1));
	InvalidateRect(p1.hWnd, NULL, true);
	return(dwResult);
}
static int x_offset=x2-x1;

DWORD WINAPI ThreadFunc2(PVOID pvParam)
{
	DWORD dwResult = 0;
	struct PARAM* p = (struct PARAM*)pvParam;
	HDC hdc = GetDC(p->hWnd);
	static bool flag = true;
	while (!p->stop)
	{


		if (flag == true) {
			x1 = x1 + 20;
			x2 = x2 + 20;
			if (x2 >= rect2.right-20) {
				flag = false;
			}
		}
		else {
			x1 = x1 - 20;
			x2 = x2 - 20;
			if (x1 <= rect2.left) {
				flag = true;
			}
		}
		
		InvalidateRect(hWndPopup, NULL, true);
		Sleep(400);
	}
	FillRect(hdc, &rect2, (HBRUSH)(COLOR_WINDOW + 1));
	InvalidateRect(p2.hWnd, NULL, true);
	return(dwResult);

}
